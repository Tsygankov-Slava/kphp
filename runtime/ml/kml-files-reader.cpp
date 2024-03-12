#include "kml-files-reader.h"

#include <fstream>

namespace {

class KmlFileReader {
  std::ifstream fi;

public:
  explicit KmlFileReader(const char * kml_filename) {
    fi = std::ifstream(kml_filename, std::ios::binary);
    if (!fi.is_open()) {
      throw std::invalid_argument("can't open " + std::string(kml_filename) + " for reading");
    }
  }

  ~KmlFileReader() {
    fi.close();
  }

  int read_int() noexcept {
    int v;
    fi.read((char *)&v, sizeof(int));
    return v;
  }

  void read_int(int &v) noexcept {
    fi.read((char *)&v, sizeof(int));
  }

  void read_uint32(uint32_t &v) noexcept {
    fi.read((char *)&v, sizeof(uint32_t));
  }

  void read_uint64(uint64_t &v) noexcept {
    fi.read((char *)&v, sizeof(uint64_t));
  }

  void read_float(float &v) noexcept {
    fi.read((char *)&v, sizeof(float));
  }

  void read_double(double &v) noexcept {
    fi.read((char *)&v, sizeof(double));
  }

  void read_string(std::string &v) noexcept {
    int len;
    fi.read((char *)&len, sizeof(int));
    v.resize(len);
    fi.read(v.data(), len);
  }

  void read_bool(bool &v) noexcept {
    v = static_cast<bool>(read_int());
  }

  void read_byte(unsigned char &v) noexcept {
    read_bytes((void *)&v, sizeof(unsigned char));
  }

  void read_bytes(void *v, size_t len) noexcept {
    fi.read((char *)v, static_cast<std::streamsize>(len));
  }

  void check_not_eof() const {
    if (fi.is_open() && fi.eof()) {
      throw std::invalid_argument("unexpected eof");
    }
  }
};

// ModelKind::xgboost_trees_no_cat
void kml_file_read_xgboost_trees_no_cat(KmlFileReader &f, [[maybe_unused]] int version, ml_internals::MLModel &kml) {
  auto &xgb_model = std::get<ml_internals::XgbModel>(kml.impl);

  xgb_model.tparam_objective = static_cast<ml_internals::XGTrainParamObjective>(f.read_int());
  f.read_bytes(&xgb_model.calibration, sizeof(ml_internals::CalibrationMethod));
  f.read_float(xgb_model.base_score);
  f.read_int(xgb_model.num_features_trained);
  f.read_int(xgb_model.num_features_present);
  f.read_int(xgb_model.max_required_features);

  f.check_not_eof();
  if (xgb_model.num_features_present <= 0 || xgb_model.num_features_present > xgb_model.max_required_features) {
    throw std::invalid_argument("wrong num_features_present");
  }

  int num_trees = f.read_int();
  if (num_trees <= 0 || num_trees > 10000) {
    throw std::invalid_argument("wrong num_trees");
  }
  // todo compare performance with a single vector of all nodes of all trees (maybe, linear memory is better?)
  xgb_model.trees.resize(num_trees);
  for (ml_internals::XgbTree &tree : xgb_model.trees) {
    int num_nodes = f.read_int();
    if (num_nodes <= 0 || num_nodes > 10000) {
      throw std::invalid_argument("wrong num_nodes");
    }
    tree.nodes.resize(num_nodes);
    f.read_bytes(tree.nodes.data(), sizeof(ml_internals::XgbTreeNode) * num_nodes);
  }

  f.check_not_eof();
  xgb_model.offset_in_vec = new int[xgb_model.max_required_features];
  f.read_bytes(xgb_model.offset_in_vec, xgb_model.max_required_features * sizeof(int));
  f.check_not_eof();
  xgb_model.reindex_map_int2int = new int[xgb_model.max_required_features];
  f.read_bytes(xgb_model.reindex_map_int2int, xgb_model.max_required_features * sizeof(int));
  f.check_not_eof();

  int num_reindex_str2int = f.read_int();
  if (num_reindex_str2int < 0 || num_reindex_str2int > xgb_model.max_required_features) {
    throw std::invalid_argument("wrong num_reindex_str2int");
  }
  for (int i = 0; i < num_reindex_str2int; ++i) {
    uint64_t hash;
    int feature_id;
    f.read_uint64(hash);
    f.read_int(feature_id);
    xgb_model.reindex_map_str2int[hash] = feature_id;
  }
  f.check_not_eof();

  f.read_bool(xgb_model.skip_zeroes);
}

template<class T>
void kml_read_vec(KmlFileReader &f, std::vector<T> &v) {
  static_assert(std::is_standard_layout_v<T> && std::is_trivial_v<T>);
  int s;
  f.read_int(s);
  v.resize(s);
  f.read_bytes(v.data(), s * sizeof(T));
}

template<class T>
void kml_read_2d_vec(KmlFileReader &f, std::vector<std::vector<T>> &v) {
  int s;
  f.read_int(s);
  v.resize(s);
  for (auto &elem : v) {
    kml_read_vec(f, elem);
  }
}

void kml_read_catboost_bin_feat_index_value(KmlFileReader &f, cb_common::CatboostBinFeatureIndexValue &idx_val) {
  f.read_int(idx_val.bin_index);
  f.read_bool(idx_val.check_value_equal);
  f.read_byte(idx_val.value);
}

void kml_read_catboost_projection(KmlFileReader &f, cb_common::CatboostProjection &proj) {
  kml_read_vec(f, proj.transposed_cat_feature_indexes);

  int sz;
  f.read_int(sz);
  proj.binarized_indexes.resize(sz);
  for (auto &item : proj.binarized_indexes) {
    kml_read_catboost_bin_feat_index_value(f, item);
  }
}

void kml_read_catboost_model_ctr(KmlFileReader &f, cb_common::CatboostModelCtr &model_ctr) {
  f.read_uint64(model_ctr.base_hash);
  int tmp;
  f.read_int(tmp);
  model_ctr.base_ctr_type = static_cast<cb_common::CatboostModelCtrType>(tmp);
  f.read_int(model_ctr.target_border_idx);
  f.read_float(model_ctr.prior_num);
  f.read_float(model_ctr.prior_denom);
  f.read_float(model_ctr.shift);
  f.read_float(model_ctr.scale);
}

void kml_read_catboost_compressed_model_ctr(KmlFileReader &f, cb_common::CatboostCompressedModelCtr &ctr) {
  kml_read_catboost_projection(f, ctr.projection);
  int sz;
  f.read_int(sz);
  ctr.model_ctrs.resize(sz);
  for (auto &item : ctr.model_ctrs) {
    kml_read_catboost_model_ctr(f, item);
  }
}

void kml_read_catboost_catboost_ctr_mean_history(KmlFileReader &f, cb_common::CatboostCtrMeanHistory &vt) {
  f.read_float(vt.sum);
  f.read_int(vt.count);
}

void kml_read_catboost_ctr_value_table(KmlFileReader &f, cb_common::CatboostCtrValueTable &vt) {
  int sz;
  f.read_int(sz);
  vt.index_hash_viewer.reserve(sz);
  for (int i = 0; i < sz; ++i) {
    uint64_t key;
    f.read_uint64(key);
    uint32_t val;
    f.read_uint32(val);
    vt.index_hash_viewer[key] = val;
  }

  f.read_int(vt.target_classes_count);
  f.read_int(vt.counter_denominator);

  f.read_int(sz);
  vt.ctr_mean_history.resize(sz);
  for (auto &item : vt.ctr_mean_history) {
    kml_read_catboost_catboost_ctr_mean_history(f, item);
  }

  kml_read_vec(f, vt.ctr_total);
}

void kml_write_catboost_ctr_data(KmlFileReader &f, cb_common::CatboostCtrData &cmc) {
  int sz;
  f.read_int(sz);
  cmc.learn_ctrs.reserve(sz);
  for (int i = 0; i < sz; ++i) {
    uint64_t key;
    f.read_uint64(key);

    kml_read_catboost_ctr_value_table(f, cmc.learn_ctrs[key]);
  }
}

void kml_read_catboost_ctrs_container(KmlFileReader &f, [[maybe_unused]] cb_common::CatboostModelCtrsContainer &ctr) {
  bool has;
  f.read_bool(has);
  if (has) {
    f.read_int(ctr.used_model_ctrs_count);

    int cmc_size;
    f.read_int(cmc_size);
    ctr.compressed_model_ctrs.resize(cmc_size);
    for (auto &item : ctr.compressed_model_ctrs) {
      kml_read_catboost_compressed_model_ctr(f, item);
    }

    kml_write_catboost_ctr_data(f, ctr.ctr_data);
  }
}

void kml_file_read_catboost_trees(KmlFileReader &f, [[maybe_unused]] int version, ml_internals::MLModel &kml) {
  auto &cb_model = std::get<ml_internals::CbModel>(kml.impl);

  f.read_int(cb_model.float_feature_count);
  f.read_int(cb_model.cat_feature_count);
  f.read_int(cb_model.binary_feature_count);
  f.read_int(cb_model.tree_count);

  kml_read_vec(f, cb_model.float_features_index);
  kml_read_2d_vec(f, cb_model.float_feature_borders);
  kml_read_vec(f, cb_model.tree_depth);
  kml_read_vec(f, cb_model.cat_features_index);
  kml_read_vec(f, cb_model.one_hot_cat_feature_index);
  kml_read_2d_vec(f, cb_model.one_hot_hash_values);
  kml_read_2d_vec(f, cb_model.ctr_feature_borders);
  kml_read_vec(f, cb_model.leaf_values);
  kml_read_2d_vec(f, cb_model.leaf_values_vec);

  f.read_double(cb_model.scale);
  f.read_double(cb_model.bias);

  kml_read_vec(f, cb_model.biases);

  f.read_int(cb_model.dimension);

  // TODO maybe calculate std::hash from strings?

  int sz;
  f.read_int(sz);
  cb_model.cat_features_hashes.reserve(sz);
  for (int i = 0; i < sz; ++i) {
    std::string str;
    int hash;
    f.read_string(str);
    f.read_int(hash);
    cb_model.cat_features_hashes[str] = hash;
  }

  kml_read_catboost_ctrs_container(f, cb_model.model_ctrs);

  kml_read_vec(f, cb_model.tree_split);
}

} // namespace

ml_internals::MLModel kml_file_read(const char * kml_filename) {
  ml_internals::MLModel kml;
  KmlFileReader f(kml_filename);

  if (f.read_int() != ml_internals::KML_FILE_PREFIX) {
    throw std::invalid_argument("wrong .kml file prefix");
  }
  int version = f.read_int();
  if (version != ml_internals::KML_FILE_VERSION_100) {
    throw std::invalid_argument("wrong .kml file version");
  }

  kml.model_kind = static_cast<ml_internals::ModelKind>(f.read_int());
  kml.input_kind = static_cast<ml_internals::InputKind>(f.read_int());
  f.read_string(kml.model_name);
  f.check_not_eof();

  switch (kml.model_kind) {
    case ml_internals::ModelKind::xgboost_trees_no_cat:
      kml.impl = ml_internals::XgbModel();
      kml_file_read_xgboost_trees_no_cat(f, version, kml);
      break;
    case ml_internals::ModelKind::catboost_trees:
      kml.impl = ml_internals::CbModel();
      kml_file_read_catboost_trees(f, version, kml);
      break;
    default:
      throw std::invalid_argument("unsupported model_kind");
  }

  return kml;
}
