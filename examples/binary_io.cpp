#include <grb/grb.hpp>

#include <string>
#include <fstream>
#include <ranges>
#include <nlohmann/json.hpp>
#include <H5Cpp.h>

template <typename U>
inline H5::PredType get_hdf5_native_type() {
  using T = std::decay_t<U>;
  if constexpr(std::is_same_v<T, char>) {
    return H5::PredType::NATIVE_CHAR;
  } else if constexpr (std::is_same_v<T, unsigned char>) {
    return H5::PredType::NATIVE_UCHAR;
  } else if constexpr (std::is_same_v<T, short>) {
    return H5::PredType::NATIVE_SHORT;
  } else if constexpr (std::is_same_v<T, unsigned short>) {
    return H5::PredType::NATIVE_USHORT;
  } else if constexpr (std::is_same_v<T, int>) {
    return H5::PredType::NATIVE_INT;
  } else if constexpr (std::is_same_v<T, unsigned int>) {
    return H5::PredType::NATIVE_UINT;
  } else if constexpr (std::is_same_v<T, long>) {
    return H5::PredType::NATIVE_LONG;
  } else if constexpr (std::is_same_v<T, unsigned long>) {
    return H5::PredType::NATIVE_ULONG;
  } else if constexpr (std::is_same_v<T, long long>) {
    return H5::PredType::NATIVE_LLONG;
  } else if constexpr (std::is_same_v<T, unsigned long long>) {
    return H5::PredType::NATIVE_ULLONG;
  } else if constexpr (std::is_same_v<T, float>) {
    return H5::PredType::NATIVE_FLOAT;
  } else if constexpr (std::is_same_v<T, double>) {
    return H5::PredType::NATIVE_DOUBLE;
  } else if constexpr (std::is_same_v<T, long double>) {
    return H5::PredType::NATIVE_LDOUBLE;
  } else {
    assert(false);
  }
}

template <typename U>
inline H5::PredType get_hdf5_standard_type() {
  using T = std::decay_t<U>;
  if constexpr(std::is_same_v<T, char>) {
    return H5::PredType::STD_I8LE;
  } else if constexpr(std::is_same_v<T, unsigned char>) {
    return H5::PredType::STD_U8LE;
  } else if constexpr(std::is_same_v<T, int8_t>) {
    return H5::PredType::STD_I8LE;
  } else if constexpr (std::is_same_v<T, uint8_t>) {
    return H5::PredType::STD_U8LE;
  } else if constexpr (std::is_same_v<T, int16_t>) {
    return H5::PredType::STD_I16LE;
  } else if constexpr (std::is_same_v<T, uint16_t>) {
    return H5::PredType::STD_U16LE;
  } else if constexpr (std::is_same_v<T, int32_t>) {
    return H5::PredType::STD_I32LE;
  } else if constexpr (std::is_same_v<T, uint32_t>) {
    return H5::PredType::STD_U32LE;
  } else if constexpr (std::is_same_v<T, int64_t>) {
    return H5::PredType::STD_I64LE;
  } else if constexpr (std::is_same_v<T, uint64_t>) {
    return H5::PredType::STD_U64LE;
  } else if constexpr (std::is_same_v<T, std::size_t>) {
    return H5::PredType::STD_U64LE;
  } else if constexpr (std::is_same_v<T, float>) {
    return H5::PredType::IEEE_F32LE;
  } else if constexpr (std::is_same_v<T, double>) {
    return H5::PredType::IEEE_F64LE;
  } else {
    assert(false);
  }
}

/*
template <typename T>
struct hdf5_type;

template <>
struct hdf5_type<char> {
  using type = H5::PredType::NATIVE_CHAR;
};

template <>
struct hdf5_type<unsigned char> {
  using type = H5::PredType::NATIVE_UCHAR;
};

template <>
struct hdf5_type<short> {
  using type = H5::PredType::NATIVE_SHORT;
};

template <>
struct hdf5_type<unsigned short> {
  using type = H5::PredType::NATIVE_USHORT;
};

template <>
struct hdf5_type<int> {
  using type = H5::PredType::NATIVE_INT;
};

template <>
struct hdf5_type<unsigned int> {
  using type = H5::PredType::NATIVE_UINT;
};

template <>
struct hdf5_type<long> {
  using type = H5::PredType::NATIVE_LONG;
};

template <>
struct hdf5_type<unsigned long> {
  using type = H5::PredType::NATIVE_ULONG;
};

template <>
struct hdf5_type<long long> {
  using type = H5::PredType::NATIVE_LLONG;
};

template <>
struct hdf5_type<unsigned long long> {
  using type = H5::PredType::NATIVE_ULLONG;
};

template <>
struct hdf5_type<float> {
  using type = H5::PredType::NATIVE_FLOAT;
};

template <>
struct hdf5_type<double> {
  using type = H5::PredType::NATIVE_DOUBLE;
};

template <>
struct hdf5_type<long double> {
  using type = H5::PredType::NATIVE_LDOUBLE;
};

template <typename T>
using hdf5_type_t = typename hdf5_type<T>::type;
*/

void parse_json(std::string fname) {
  std::ifstream f(fname);
  using json = nlohmann::json;
  auto data = json::parse(f);
  std::string format = data["format"];
  printf("got format %s\n", format.c_str());
  std::vector<std::size_t> shape = data["shape"];
  printf("shape %lu x %lu\n", shape[0], shape[1]);
}

template <std::ranges::contiguous_range R>
void write_dataset(H5::H5File& f, const std::string& label, R&& r) {
  using T = std::ranges::range_value_t<R>;
  hsize_t size = std::ranges::size(r);
  H5::DataSpace dataspace(1, &size);
  auto dataset = f.createDataSet(label.c_str(), get_hdf5_standard_type<T>(), dataspace);

  dataset.write(std::ranges::data(r), get_hdf5_native_type<T>());
  dataset.close();
  dataspace.close();
}

template <typename T>
std::vector<T> read_dataset(H5::H5File& f, const std::string& label) {
  H5::DataSet dataset = f.openDataSet(label.c_str());
  H5::DataSpace space = dataset.getSpace();
  hsize_t ndims = space.getSimpleExtentNdims();
  assert(ndims == 1);
  hsize_t dims;
  space.getSimpleExtentDims(&dims, &ndims);
  std::vector<T> v(dims);
  dataset.read(v.data(), get_hdf5_native_type<T>());
  dataset.close();
  space.close();
  return v;
}

template <typename T>
void write_file(std::string fname, const std::vector<T>& v) {
  H5::H5File f(fname.c_str(), H5F_ACC_TRUNC);

  write_dataset(f, "data", v);
  write_dataset(f, "metadata", "this is my metadata");

  f.close();
}

/*
template <typename T>
void write_file(std::string fname, const std::vector<T>& v) {
  H5::H5File f(fname.c_str(), H5F_ACC_TRUNC);

  hsize_t size = v.size();
  H5::DataSpace dataspace(1, &size);
  auto dataset = f.createDataSet("data", get_hdf5_standard_type<T>(), dataspace);

  dataset.write(v.data(), get_hdf5_native_type<T>());
  dataset.close();
  dataspace.close();
  f.close();
}
*/

template <typename T>
void read_file(std::string fname) {
  H5::H5File f(fname.c_str(), H5F_ACC_RDWR);

  auto v = read_dataset<T>(f, "data");

  auto metadata = read_dataset<unsigned char>(f, "metadata");
  std::string view(metadata.begin(), metadata.end());

  printf("metadata: %s\n", view.c_str());

  for (auto e : v) {
    printf("%d\n", e);
  }

  f.close();
}

/*
template <typename T>
std::vector<T> read_file(std::string fname) {
  H5::H5File f(fname.c_str(), H5F_ACC_RDWR);
  H5::DataSet dataset = f.openDataSet("data");
  H5::DataSpace space = dataset.getSpace();
  hsize_t ndims = space.getSimpleExtentNdims();
  assert(ndims == 1);
  hsize_t dims;
  space.getSimpleExtentDims(&dims, &ndims);
  std::vector<T> v(dims);
  dataset.read(v.data(), get_hdf5_native_type<T>());
  dataset.close();
  space.close();
  f.close();
  return v;
}
*/

template <grb::MatrixRange M>
void write_matrix(std::string fname, M&& matrix) {

  using index_type = grb::matrix_index_t<M>;
  using value_type = grb::matrix_scalar_t<M>;

  std::vector<std::ranges::range_value_t<M>> tuples(matrix.begin(), matrix.end());

  auto sort_fn = [](const auto& a, const auto& b) {
                   auto&& [a_index, a_value] = a;
                   auto&& [b_index, b_value] = b;
                   auto&& [a_i, a_j] = a_index;
                   auto&& [b_i, b_j] = b_index;
                   if (a_i < b_i) {
                     return true;
                   }
                   else if (a_i == b_i) {
                     if (a_j < b_j) {
                      return true;
                     }
                   }
                   return false;
                 };

  std::sort(tuples.begin(), tuples.end(), sort_fn);

  std::vector<grb::matrix_index_t<M>> rows;
  std::vector<grb::matrix_index_t<M>> cols;
  std::vector<grb::matrix_scalar_t<M>> vals;

  rows.reserve(matrix.size());
  cols.reserve(matrix.size());
  vals.reserve(matrix.size());

  for (auto&& [index, value] : tuples) {
    auto&& [row, col] = index;
    rows.push_back(row);
    cols.push_back(col);
    vals.push_back(value);
  }

  H5::H5File f(fname.c_str(), H5F_ACC_TRUNC);

  write_dataset(f, "values", vals);
  write_dataset(f, "indices_0", rows);
  write_dataset(f, "indices_1", cols);

  std::string json_string =
  "{\n"
  "  \"format\": \"COO\",\n"
  "  \"shape\": [";
  json_string += std::to_string(matrix.shape()[0]) + ", " + std::to_string(matrix.shape()[1]) +
  "],\n" +
  "  \"nnz\": " + std::to_string(matrix.size()) + "\n" +
  "}\n";

  write_dataset(f, "metadata", json_string);

  f.close();
}

template <typename T, typename I = std::size_t, typename Hint = grb::sparse>
grb::matrix<T, I, Hint> read_matrix(std::string fname) {
  H5::H5File f(fname.c_str(), H5F_ACC_RDWR);

  auto metadata = read_dataset<char>(f, "metadata");

  using json = nlohmann::json;
  auto data = json::parse(metadata);

  if (data["format"] == "COO") {
    auto nrows = data["shape"][0];
    auto ncols = data["shape"][1];
    auto nnz = data["nnz"];

    auto values = read_dataset<T>(f, "values");
    auto rows = read_dataset<T>(f, "indices_0");
    auto cols = read_dataset<T>(f, "indices_1");

    grb::matrix<T, I, Hint> m({nrows, ncols});

    std::vector<std::tuple<std::tuple<I, I>, T>> tuples;

    for (size_t i = 0; i < nnz; i++) {
      tuples.push_back({{rows[i], cols[i]}, values[i]});
    }

    m.insert(tuples.begin(), tuples.end());

    return m;
  } else {
    assert(false);
  }
  f.close();
}

int main(int argc, char** argv) {
  // parse_json("binary.json");

  write_file("short_array.hdf5", std::vector<int>({1, 2, 3, 4}));

  read_file<int>("short_array.hdf5");

  auto m = grb::generate_random<float>({10, 10}, 0.25);

  grb::print(m, "Before Printing");

  get_hdf5_standard_type<std::size_t>();
  get_hdf5_native_type<std::size_t>();

  write_matrix("matrix.hdf5", m);
  auto read_m = read_matrix<float>("matrix.hdf5");

  grb::print(read_m, "After Printing");

  return 0;
}