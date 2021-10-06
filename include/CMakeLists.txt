add_library(RGRI INTERFACE
    grb/algorithms/algorithms.hpp
    grb/algorithms/ewise_algs.hpp
    grb/algorithms/mxv.hpp
    grb/containers/backend/csr_matrix.hpp
    grb/containers/backend/csr_matrix_impl.hpp
    grb/containers/backend/csr_matrix_iterator.hpp
    grb/containers/backend/csr_matrix_reference.hpp
    grb/containers/backend/dense_matrix.hpp
    grb/containers/backend/dense_matrix_impl.hpp
    grb/containers/backend/dense_matrix_iterator.hpp
    grb/containers/backend/dense_matrix_reference.hpp
    grb/containers/functional/functional.hpp
    grb/containers/functional/op_definitions.hpp
    grb/containers/functional/semiring.hpp
    grb/containers/functional/semiring_definitions.hpp
    grb/containers/matrix.hpp
    grb/containers/matrix_impl.hpp
    grb/containers/matrix_iterator.hpp
    grb/containers/matrix_reference.hpp
    grb/containers/vector.hpp
    grb/containers/views/transform_view.hpp
    grb/containers/views/transpose_matrix_view.hpp
    grb/containers/views/transpose_matrix_view_iterator.hpp
    grb/containers/views/transpose_matrix_view_reference.hpp
    grb/detail/monoid_traits.hpp
    grb/detail/pack_includes.hpp
    grb/exceptions/exception.hpp
    grb/experimental/transform_matrix_view.hpp
    grb/experimental/transform_matrix_view_iterator.hpp
    grb/experimental/transform_matrix_view_reference.hpp
    grb/grb.hpp
    grb/util/index_t.hpp
    grb/util/matrix_hints.hpp
    grb/util/matrix_io.hpp
    grb/util/printing.hpp
    grb/util/util.hpp
)
add_library(RGRI::RGRI ALIAS RGRI)
target_include_directories(RGRI INTERFACE "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>")
target_compile_features(RGRI INTERFACE cxx_std_17)
set_target_properties(
    RGRI 
    PROPERTIES
        LINKER_LANGUAGE CXX
        EXPORT_NAME RGRI
        VERSION ${RGRI_VERSION}
)
