# CMAKE generated file: DO NOT EDIT!
# Generated by CMake Version 3.22
cmake_policy(SET CMP0009 NEW)

# KPHP_RUNTIME_ALL_HEADERS at kphp/runtime/runtime.cmake:165 (file)
file(GLOB_RECURSE NEW_GLOB LIST_DIRECTORIES false RELATIVE "/opt/KasperskyOS-Community-Edition-1.1.1.13/examples/kphp/kphp" "/opt/KasperskyOS-Community-Edition-1.1.1.13/examples/kphp/kphp/runtime/*.h")
set(OLD_GLOB
  "runtime/allocator.h"
  "runtime/array_functions.h"
  "runtime/array_iterator.h"
  "runtime/bcmath.h"
  "runtime/common_template_instantiations.h"
  "runtime/confdata-functions.h"
  "runtime/confdata-global-manager.h"
  "runtime/confdata-keys.h"
  "runtime/critical_section.h"
  "runtime/ctype.h"
  "runtime/curl-async.h"
  "runtime/curl.h"
  "runtime/datetime/date_interval.h"
  "runtime/datetime/datetime.h"
  "runtime/datetime/datetime_functions.h"
  "runtime/datetime/datetime_immutable.h"
  "runtime/datetime/datetime_interface.h"
  "runtime/datetime/datetime_zone.h"
  "runtime/datetime/timelib_wrapper.h"
  "runtime/declarations.h"
  "runtime/dummy-visitor-methods.h"
  "runtime/env.h"
  "runtime/exception.h"
  "runtime/exec.h"
  "runtime/ffi.h"
  "runtime/files.h"
  "runtime/from-json-processor.h"
  "runtime/include.h"
  "runtime/instance-cache.h"
  "runtime/instance-copy-processor.h"
  "runtime/inter-process-mutex.h"
  "runtime/inter-process-resource.h"
  "runtime/interface.h"
  "runtime/job-workers/client-functions.h"
  "runtime/job-workers/job-interface.h"
  "runtime/job-workers/processing-jobs.h"
  "runtime/job-workers/server-functions.h"
  "runtime/json-functions.h"
  "runtime/json-processor-utils.h"
  "runtime/json-writer.h"
  "runtime/kphp-backtrace.h"
  "runtime/kphp_core.h"
  "runtime/kphp_tracing.h"
  "runtime/kphp_tracing_binlog.h"
  "runtime/kphp_type_traits.h"
  "runtime/mail.h"
  "runtime/math_functions.h"
  "runtime/mbstring.h"
  "runtime/memcache.h"
  "runtime/memory_resource/dealer.h"
  "runtime/memory_resource/details/memory_chunk_list.h"
  "runtime/memory_resource/details/memory_chunk_tree.h"
  "runtime/memory_resource/details/memory_ordered_chunk_list.h"
  "runtime/memory_resource/details/universal_reallocate.h"
  "runtime/memory_resource/extra-memory-pool.h"
  "runtime/memory_resource/heap_resource.h"
  "runtime/memory_resource/memory_resource.h"
  "runtime/memory_resource/monotonic_buffer_resource.h"
  "runtime/memory_resource/resource_allocator.h"
  "runtime/memory_resource/unsynchronized_pool_resource.h"
  "runtime/memory_usage.h"
  "runtime/migration_php8.h"
  "runtime/misc.h"
  "runtime/msgpack-serialization.h"
  "runtime/msgpack/adaptor_base.h"
  "runtime/msgpack/adaptors.h"
  "runtime/msgpack/check_instance_depth.h"
  "runtime/msgpack/object.h"
  "runtime/msgpack/object_visitor.h"
  "runtime/msgpack/packer.h"
  "runtime/msgpack/parser.h"
  "runtime/msgpack/sysdep.h"
  "runtime/msgpack/unpack_exception.h"
  "runtime/msgpack/unpacker.h"
  "runtime/msgpack/zone.h"
  "runtime/mysql.h"
  "runtime/net_events.h"
  "runtime/null_coalesce.h"
  "runtime/on_kphp_warning_callback.h"
  "runtime/oom_handler.h"
  "runtime/openssl.h"
  "runtime/optional.h"
  "runtime/pdo/abstract_pdo_driver.h"
  "runtime/pdo/abstract_pdo_statement.h"
  "runtime/pdo/mysql/mysql_pdo_driver.h"
  "runtime/pdo/mysql/mysql_pdo_emulated_statement.h"
  "runtime/pdo/pdo.h"
  "runtime/pdo/pdo_statement.h"
  "runtime/pdo/pgsql/pgsql_pdo_driver.h"
  "runtime/pdo/pgsql/pgsql_pdo_emulated_statement.h"
  "runtime/php_assert.h"
  "runtime/profiler.h"
  "runtime/refcountable_php_classes.h"
  "runtime/regexp.h"
  "runtime/resumable.h"
  "runtime/rpc.h"
  "runtime/serialize-functions.h"
  "runtime/shape.h"
  "runtime/signal_safe_hashtable.h"
  "runtime/spl/array_iterator.h"
  "runtime/storage.h"
  "runtime/streams.h"
  "runtime/string-list.h"
  "runtime/string_cache.h"
  "runtime/string_functions.h"
  "runtime/tcp.h"
  "runtime/tl/rpc_function.h"
  "runtime/tl/rpc_request.h"
  "runtime/tl/rpc_response.h"
  "runtime/tl/rpc_server.h"
  "runtime/tl/rpc_tl_query.h"
  "runtime/tl/tl_builtins.h"
  "runtime/tl/tl_func_base.h"
  "runtime/tl/tl_magics_decoding.h"
  "runtime/to-array-processor.h"
  "runtime/to-json-processor.h"
  "runtime/typed_rpc.h"
  "runtime/uber-h3.h"
  "runtime/udp.h"
  "runtime/url.h"
  "runtime/vkext.h"
  "runtime/vkext_stats.h"
  "runtime/zlib.h"
  "runtime/zstd.h"
  )
if(NOT "${NEW_GLOB}" STREQUAL "${OLD_GLOB}")
  message("-- GLOB mismatch!")
  file(TOUCH_NOCREATE "/opt/KasperskyOS-Community-Edition-1.1.1.13/examples/kphp/build/CMakeFiles/cmake.verify_globs")
endif()
