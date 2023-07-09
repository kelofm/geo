#ifndef CIE_DENSE_MATRIX_CONVERSION
#define CIE_DENSE_MATRIX_CONVERSION

// --- Utility Includes ---
#include "packages/types/inc/types.hpp"

namespace pybind11 {
namespace detail {

// Template specialization for pybind11::detail::type_caster, see
// https://pybind11.readthedocs.io/en/stable/advanced/cast/custom.html
template<>
struct type_caster<cie::linalg::Matrix<cie::Double>>
{
public:

    PYBIND11_TYPE_CASTER( cie::linalg::Matrix<cie::Double>, _( "cie::linalg::Matrix<cie::Double>" ) );

    // Conversion from python numpy array to our C++ cie::linalg::Matrix
    bool load( pybind11::handle src, bool convert )
    {
        if ( convert || pybind11::array_t<cie::Double>::check_( src ) )
        {
            auto numpyArray = pybind11::array_t<cie::Double, pybind11::array::c_style |
                                                        pybind11::array::forcecast>::ensure( src );

            if ( numpyArray && numpyArray.ndim( ) == 2 )
            {
                cie::Size size1 = numpyArray.shape( )[0];
                cie::Size size2 = numpyArray.shape( )[1];

                // value is a member defined by the PYBIND11_TYPE_CASTER macro
                value = cie::linalg::Matrix( size1, size2, 0.0 );

                // Copy matrix
                for( cie::Size i = 0; i < size1; i++ )
                {
                    for( cie::Size j = 0; j < size2; j++ )
                    {
                        value( i, j ) = numpyArray.at( i, j );
                    }
                }

                return true; // success
            }
        }
        return false; // failure
    }

    // Conversion from our C++ cie::linalg::Matrix to python numpy array
    static pybind11::handle cast( const cie::linalg::Matrix<cie::Double>& src,
                                  pybind11::return_value_policy policy,
                                  pybind11::handle parent )
    {
        // Construct array with shape ( size1, size2 ) from the given cie::linalg::Matrix
        return pybind11::array( std::vector<cie::Size>{ src.size1( ), src.size2( ) },
                                &const_cast<cie::linalg::Matrix<cie::Double>&>( src )( 0, 0 ) ).release( );
    }
};

} // detail
} // pybind11

#endif // CIE_DENSE_MATRIX_CONVERSION
