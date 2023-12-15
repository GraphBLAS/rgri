#pragma once

#include <H5Cpp.h>
#include <ranges>
#include <vector>

namespace hdf5_tools {

template <typename U>
inline H5::PredType get_hdf5_native_type() {
  using T = std::decay_t<U>;
  if constexpr (std::is_same_v<T, char>) {
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
  if constexpr (std::is_same_v<T, char>) {
    return H5::PredType::STD_I8LE;
  } else if constexpr (std::is_same_v<T, unsigned char>) {
    return H5::PredType::STD_U8LE;
  } else if constexpr (std::is_same_v<T, int8_t>) {
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

template <std::ranges::contiguous_range R>
void write_dataset(H5::H5File& f, const std::string& label, R&& r) {
  using T = std::ranges::range_value_t<R>;
  hsize_t size = std::ranges::size(r);
  H5::DataSpace dataspace(1, &size);
  auto dataset =
      f.createDataSet(label.c_str(), get_hdf5_standard_type<T>(), dataspace);

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

} // namespace hdf5_tools
