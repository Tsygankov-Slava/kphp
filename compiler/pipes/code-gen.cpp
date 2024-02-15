// Compiler for PHP (aka KPHP)
// Copyright (c) 2020 LLC «V Kontakte»
// Distributed under the GPL v3 License, see LICENSE.notice.txt

#include "compiler/pipes/code-gen.h"

#include "compiler/code-gen/code-gen-task.h"
#include "compiler/code-gen/code-generator.h"
#include "compiler/code-gen/common.h"
#include "compiler/code-gen/declarations.h"
#include "compiler/code-gen/files/cmake-lists-txt.h"
#include "compiler/code-gen/files/const-vars-init.h"
#include "compiler/code-gen/files/function-header.h"
#include "compiler/code-gen/files/function-source.h"
#include "compiler/code-gen/files/global-vars-declarations.h"
#include "compiler/code-gen/files/global-vars-memory-stats.h"
#include "compiler/code-gen/files/global-vars-reset.h"
#include "compiler/code-gen/files/init-scripts.h"
#include "compiler/code-gen/files/json-encoder-tags.h"
#include "compiler/code-gen/files/lib-header.h"
#include "compiler/code-gen/files/shape-keys.h"
#include "compiler/code-gen/files/tl2cpp/tl2cpp.h"
#include "compiler/code-gen/files/tracing-autogen.h"
#include "compiler/code-gen/files/type-tagger.h"
#include "compiler/code-gen/raw-data.h"
#include "compiler/compiler-core.h"
#include "compiler/cpp-dest-dir-initializer.h"
#include "compiler/data/class-data.h"
#include "compiler/data/function-data.h"
#include "compiler/data/lib-data.h"
#include "compiler/data/src-file.h"
#include "compiler/function-pass.h"
#include "compiler/inferring/public.h"
#include "compiler/pipes/collect-forkable-types.h"
#include "compiler/type-hint.h"
#include "compiler/vertex-util.h"

size_t CodeGenF::calc_count_of_parts(size_t cnt_global_vars) {
  return 1u + cnt_global_vars / G->settings().globals_split_count.get();
}


void CodeGenF::execute(FunctionPtr function, DataStream<std::unique_ptr<CodeGenRootCmd>> &unused_os __attribute__ ((unused))) {
  if (function->does_need_codegen() || function->is_imported_from_static_lib()) {
    prepare_generate_function(function);
    G->stats.on_function_processed(function);
    tmp_stream << function;
  }
}

std::vector<std::pair<std::string, int>> sort_map_by_count(const std::unordered_map<std::string, int> &type_to_count) {
  std::vector<std::pair<std::string, int>> result(type_to_count.begin(), type_to_count.end());

  std::sort(result.begin(), result.end(), [](const auto &c1, const auto &c2) { return c1.second > c2.second; });

  return result;
}

void CodeGenF::on_finish(DataStream<std::unique_ptr<CodeGenRootCmd>> &os) {
  vk::singleton<CppDestDirInitializer>::get().wait();

  G->get_ffi_root().bind_symbols();
  TracingAutogen::finished_appending_and_prepare();

  stage::set_name("GenerateCode");
  stage::set_file(SrcFilePtr());
  stage::die_if_global_errors();

  std::forward_list<FunctionPtr> all_functions = tmp_stream.flush();   // functions to codegen, order doesn't matter
  const std::vector<ClassPtr> &all_classes = G->get_classes();
  std::set<ClassPtr> all_json_encoders;

  std::vector<VarPtr> all_global_vars = G->get_global_vars();
  for (FunctionPtr f : all_functions) {
    all_global_vars.insert(all_global_vars.end(), f->static_var_ids.begin(), f->static_var_ids.end());
  }
  size_t parts_cnt = calc_count_of_parts(all_global_vars.size());

  std::vector<VarPtr> all_constants;
  for (VarPtr var : all_global_vars) {
    if (var->is_constant()) {
      all_constants.push_back(var);
    }
  }
  // todo get_constants_linear_mem() should return const, and here we should use some init()?
  G->get_constants_linear_mem().prepare_constants_linear_mem_and_assign_offsets(all_constants);

  for (FunctionPtr f : all_functions) {
    code_gen_start_root_task(os, std::make_unique<FunctionH>(f));
    code_gen_start_root_task(os, std::make_unique<FunctionCpp>(f));
  }

  for (ClassPtr c : all_classes) {
    if (c->kphp_json_tags && G->get_class("JsonEncoder")->is_parent_of(c)) {
      all_json_encoders.insert(c);
    }
    if (!c->does_need_codegen()) {
      continue;
    }

    switch (c->class_type) {
      case ClassType::klass:
        code_gen_start_root_task(os, std::make_unique<ClassDeclaration>(c));
        code_gen_start_root_task(os, std::make_unique<ClassMembersDefinition>(c));
        break;
      case ClassType::interface:
        code_gen_start_root_task(os, std::make_unique<InterfaceDeclaration>(c));
        break;
      case ClassType::ffi_scope:
        code_gen_start_root_task(os, std::make_unique<FFIDeclaration>(c));
        break;

      default:
        kphp_assert(false);
    }
  }

  if (G->settings().enable_global_vars_memory_stats.get()) {
    code_gen_start_root_task(os, std::make_unique<GlobalVarsMemoryStats>(G->get_main_file()));
  }
  code_gen_start_root_task(os, std::make_unique<InitScriptsCpp>(G->get_main_file()));

  std::unordered_map<std::string, int> functions_using_constants;
  std::unordered_map<std::string, int> functions_using_globals;
  std::unordered_map<std::string, int> functions_using_statics;
  for (FunctionPtr f : all_functions) {
    if (!f->explicit_const_var_ids.empty() || !f->explicit_header_const_var_ids.empty()) {
      functions_using_constants[f->name] += f->explicit_const_var_ids.size() + f->explicit_header_const_var_ids.size();
    }
    if (!f->global_var_ids.empty()) {
      functions_using_globals[f->name] += f->global_var_ids.size();
    }
    if (!f->static_var_ids.empty()) {
      functions_using_statics[f->name] += f->static_var_ids.size();
    }
  }
  printf("functions_using_constants %d\n", static_cast<int>(functions_using_constants.size()));
  for (const auto &[fname, count] : sort_map_by_count(functions_using_constants)) {
    printf("- %s = %d\n", fname.c_str(), count);
  }
  printf("\n\n");
  printf("functions_using_globals %d\n", static_cast<int>(functions_using_globals.size()));
  for (const auto &[fname, count] : sort_map_by_count(functions_using_globals)) {
    printf("- %s = %d\n", fname.c_str(), count);
  }
  printf("\n\n");
  printf("functions_using_statics %d\n", static_cast<int>(functions_using_statics.size()));
  for (const auto &[fname, count] : sort_map_by_count(functions_using_statics)) {
    printf("- %s = %d\n", fname.c_str(), count);
  }
  printf("\n\n");

  std::unordered_map<std::string, int> globals_used_by_functions;
  for (FunctionPtr f : all_functions) {
    for (VarPtr global_var : f->global_var_ids) {
      globals_used_by_functions[global_var->name]++;
    }
  }
  printf("globals_used_by_functions %d\n", static_cast<int>(globals_used_by_functions.size()));
  int count_1 = 0;
  for (const auto &[global_name, count] : sort_map_by_count(globals_used_by_functions)) {
    if (count == 1) {
      count_1++;
    } else {
      printf("- %s = %d\n", global_name.c_str(), count);
    }
  }
  printf("count_1 = %d\n", count_1);
  printf("\n\n");

  int n_const_total = 0;
  int n_const_string = 0;
  int n_const_regexp = 0;
  int n_const_array_any = 0;
  int n_const_mixed = 0;
  int n_const_other = 0;
  std::unordered_map<std::string, int> constants_by_type;

  int n_globals_total = 0;
  int n_globals_static_fields = 0;
  int n_globals_function_statics = 0;
  int n_globals_nonconst_defines = 0;
  int n_globals_require_once = 0;
  int n_globals_superglobal = 0;
  int n_globals_other = 0;
  std::unordered_set<ClassPtr> classes_having_static_fields;
  std::unordered_set<FunctionPtr> functions_having_static_vars;
  std::unordered_map<std::string, int> globals_by_type;

  for (VarPtr var : all_global_vars) {
    if (var->is_constant()) {
      n_const_total++;
      const TypeData *type = tinf::get_type(var);
      constants_by_type[type->as_human_readable()]++;
      if (!type->use_optional() && type->ptype() == tp_string) {
        n_const_string++;
      } else if (!type->use_optional() && type->ptype() == tp_regexp) {
        n_const_regexp++;
      } else if (!type->use_optional() && type->ptype() == tp_array) {
        n_const_array_any++;
      } else if (!type->use_optional() && type->ptype() == tp_mixed) {
        n_const_mixed++;
      } else {
        //        printf("const other $%s %s at %s\n", var->name.c_str(), type->as_human_readable().c_str(), var->init_val ?
        //        var->init_val->location.as_human_readable().c_str() : "(no location)");
        n_const_other++;
      }
    } else if (var->is_in_global_scope()) {
      n_globals_total++;
      const TypeData *type = tinf::get_type(var);
      globals_by_type[type->as_human_readable()]++;
      if (var->is_class_static_var()) {
        n_globals_static_fields++;
        classes_having_static_fields.insert(var->class_id);
      } else if (var->is_function_static_var()) {
        n_globals_function_statics++;
        functions_having_static_vars.insert(var->holder_func);
      } else if (vk::string_view{var->name}.starts_with("d$")) {
        n_globals_nonconst_defines++;
      } else if (vk::string_view{var->name}.ends_with("$called")) {
        n_globals_require_once++;
      } else if (var->is_builtin_global()) {
        n_globals_superglobal++;
      } else {
        n_globals_other++;
        //        printf("other $%s\n", var->name.c_str());
      }
    } else {
      printf("unknown global var type $%s\n", var->name.c_str());
    }
  }

  printf("n_const_total %d\n"
         "- n_const_string %d\n"
         "- n_const_regexp %d\n"
         "- n_const_array_any %d\n"
         "- n_const_mixed %d\n"
         "- n_const_other %d\n\n",
         n_const_total, n_const_string, n_const_regexp, n_const_array_any, n_const_mixed, n_const_other);
  //  auto constants_by_type_sorted = sort_map_by_count(constants_by_type);
  //  printf("constants by type (%d unique types)\n", static_cast<int>(constants_by_type.size()));
  //  for (const auto &[type_human_readable, count] : constants_by_type_sorted) {
  //    printf("- %s = %d\n", type_human_readable.c_str(), count);
  //  }
  //  printf("\n\n");

  printf("n_globals_total %d\n"
         "- n_globals_static_fields %d  (%d classes)\n"
         "- n_globals_function_statics %d (%d functions)\n"
         "- n_globals_nonconst_defines %d\n"
         "- n_globals_require_once %d\n"
         "- n_globals_superglobal %d\n"
         "- n_globals_other %d\n\n",
         n_globals_total, n_globals_static_fields, static_cast<int>(classes_having_static_fields.size()), n_globals_function_statics,
         static_cast<int>(functions_having_static_vars.size()), n_globals_nonconst_defines, n_globals_require_once, n_globals_superglobal, n_globals_other);
  //  auto globals_by_type_sorted = sort_map_by_count(globals_by_type);
  //  printf("globals by type (%d unique types)\n", static_cast<int>(globals_by_type_sorted.size()));
  //  for (const auto &[type_human_readable, count] : globals_by_type_sorted) {
  //    printf("- %s = %d\n", type_human_readable.c_str(), count);
  //  }
  //  printf("\n\n");

  std::vector<std::vector<VarPtr>> globals_batches(parts_cnt);
  std::vector<std::vector<VarPtr>> constants_batches(parts_cnt);
  std::vector<int> max_dep_levels(parts_cnt);
  for (VarPtr var : all_global_vars) {
    int part_id = vk::std_hash(var->name) % parts_cnt;
    if (var->is_constant()) {
      constants_batches[part_id].emplace_back(var);
      max_dep_levels[part_id] = std::max(max_dep_levels[part_id], var->dependency_level);
    } else {
      globals_batches[part_id].emplace_back(var);
    }
  }
  for (size_t part_id = 0; part_id < parts_cnt; ++part_id) {
    code_gen_start_root_task(os, std::make_unique<GlobalVarsDeclarationsPart>(std::move(globals_batches[part_id]), part_id));
    code_gen_start_root_task(os, std::make_unique<ConstVarsInitPart>(std::move(constants_batches[part_id]), part_id));
  }
  code_gen_start_root_task(os, std::make_unique<ConstVarsInit>(std::move(max_dep_levels), parts_cnt));
  code_gen_start_root_task(os, std::make_unique<GlobalVarsReset>(G->get_main_file()));

  if (G->settings().is_static_lib_mode()) {
    std::vector<FunctionPtr> exported_functions;
    for (FunctionPtr f : all_functions) {
      if (f->kphp_lib_export) {
        code_gen_start_root_task(os, std::make_unique<LibHeaderH>(f));
        exported_functions.emplace_back(f);
      }
    }
    std::sort(exported_functions.begin(), exported_functions.end());
    code_gen_start_root_task(os, std::make_unique<LibHeaderTxt>(std::move(exported_functions)));
    code_gen_start_root_task(os, std::make_unique<StaticLibraryRunGlobalHeaderH>());
  }

  // TODO: should be done in lib mode also, but in some other way
  if (!G->settings().is_static_lib_mode()) {
    code_gen_start_root_task(os, std::make_unique<TypeTagger>(vk::singleton<ForkableTypeStorage>::get().flush_forkable_types(), vk::singleton<ForkableTypeStorage>::get().flush_waitable_types()));
    code_gen_start_root_task(os, std::make_unique<ShapeKeys>(TypeHintShape::get_all_registered_keys()));
    code_gen_start_root_task(os, std::make_unique<JsonEncoderTags>(std::move(all_json_encoders)));
    code_gen_start_root_task(os, std::make_unique<CmakeListsTxt>());
  }

  if (!TracingAutogen::empty()) {
    code_gen_start_root_task(os, std::make_unique<TracingAutogen>());
  }

  code_gen_start_root_task(os, std::make_unique<TlSchemaToCpp>());
  code_gen_start_root_task(os, std::make_unique<LibVersionHFile>());
  if (!G->settings().is_static_lib_mode()) {
    code_gen_start_root_task(os, std::make_unique<CppMainFile>());
  }
}

void CodeGenF::prepare_generate_function(FunctionPtr func) {
  std::string file_name = func->name;
  std::replace(file_name.begin(), file_name.end(), '$', '@');

  func->header_name = file_name + ".h";
  func->subdir = calc_subdir_for_function(func);

  if (!func->is_inline) {
    func->src_name = file_name + ".cpp";
  }

  func->header_full_name =
    func->is_imported_from_static_lib()
    ? func->file_id->owner_lib->headers_dir() + func->header_name
    : func->subdir + "/" + func->header_name;

  // todo should be not needed for static/global
  std::sort(func->static_var_ids.begin(), func->static_var_ids.end());
  std::sort(func->global_var_ids.begin(), func->global_var_ids.end());
  std::sort(func->local_var_ids.begin(), func->local_var_ids.end());

  if (func->kphp_tracing) {
    TracingAutogen::register_function_marked_kphp_tracing(func);
  }
}

std::string CodeGenF::calc_subdir_for_function(FunctionPtr func) {
  // place __construct and __invoke of lambdas to a separate dir, like lambda classes are placed to cl_l/
  if (func->is_lambda()) {
    return "o_l";
  }
  if (func->modifiers.is_instance() && func->class_id->is_lambda_class()) {
    return "o_l";
  }
  if (func->modifiers.is_instance() && func->class_id->is_typed_callable_interface()) {
    return "o_l";
  }

  int bucket = vk::std_hash(func->file_id->short_file_name) % 100;
  return "o_" + std::to_string(bucket);
}


void CodeGenForDiffF::execute(std::unique_ptr<CodeGenRootCmd> cmd, DataStream<WriterData *> &os) {
  stage::set_name("Code generation for diff");

  // re-launch cmd not with "calc hashes", but with "store cpp/h contents and php comments" mode
  // all generated files will be passed to os (to WriteFilesF)
  CodeGenerator W(false, os);
  cmd->compile(W);
  // now cmd is destroyed
}
