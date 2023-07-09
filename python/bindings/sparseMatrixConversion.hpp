#ifndef CIE_SPARSE_MATRIX_CONVERSION
#define CIE_SPARSE_MATRIX_CONVERSION

namespace pybind11 {
namespace detail {

// Template specialization for pybind11::detail::type_caster, see
// https://pybind11.readthedocs.io/en/stable/advanced/cast/custom.html
template<>
struct type_caster<cie::geo::SparseDataStructure>
{
public:

    PYBIND11_TYPE_CASTER( cie::geo::SparseDataStructure, _( "cie::linalg::CompressedSparseRowMatrix" ) );

    // Convet sparse data structure to three numpy arrays and make sure memory is freed properly
    static pybind11::handle cast( const cie::geo::SparseDataStructure& constSrc,
                                  pybind11::return_value_policy policy,
                                  pybind11::handle parent )
    {
        auto& src = const_cast<cie::geo::SparseDataStructure&>( constSrc );

        auto* indices = std::get<0>( src );
        auto* indptr = std::get<1>( src );
        double* data = std::get<2>( src );

        size_t nrows = std::get<3>( src );
        std::size_t nnz = indptr[nrows];

        // Construct array with shape ( size1, size2 ) from the given linalg::Matrix
        pybind11::list arrays;

        using IndexType = std::remove_pointer<decltype( indices )>::type;

        // Define cleanup callbacks
        pybind11::capsule freeIndices ( indices, []( void *f ) { delete[] reinterpret_cast<IndexType*>( f ); } );
        pybind11::capsule freeIndptr ( indptr, []( void *f ) { delete[] reinterpret_cast<IndexType*>( f ); } );
        pybind11::capsule freeData ( data, []( void *f ) { delete[] reinterpret_cast<double*>( f ); } );

        // Append sparse data structure
        arrays.append( pybind11::array_t<IndexType>( { nnz }, { sizeof( IndexType ) }, indices, freeIndices ) );
        arrays.append( pybind11::array_t<IndexType>( { nrows + 1 }, { sizeof( IndexType ) }, indptr, freeIndptr ) );
        arrays.append( pybind11::array_t<double>( { nnz }, { sizeof( double ) }, data, freeData ) );

        return arrays.release( );
    }
};

} // detail
} // pybind11

#endif // CIE_SPARSE_MATRIX_CONVERSION
