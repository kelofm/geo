#ifndef CIE_DENSE_MATRIX_CONVERSION
#define CIE_DENSE_MATRIX_CONVERSION

// --- External Includes ---
#include "pybind11/pybind11.h"
#include "pybind11/numpy.h"

// --- LinAlg Includes ---
#include "packages/matrix/inc/DynamicEigenMatrix.hpp"

// --- Utility Includes ---
#include "packages/types/inc/types.hpp"


namespace pybind11 {
namespace detail {

// Template specialization for pybind11::detail::type_caster, see
// https://pybind11.readthedocs.io/en/stable/advanced/cast/custom.html
template<>
struct type_caster<cie::linalg::DynamicEigenMatrix<cie::Double>>
{
public:

PYBIND11_TYPE_CASTER(cie::linalg::DynamicEigenMatrix<cie::Double>, _("cie::linalg::DynamicEigenMatrix<cie::Double>"));

    // Conversion from python numpy array to our C++ cie::linalg::Matrix
    bool load(pybind11::handle src, bool convert) {
        if (convert || pybind11::array_t<cie::Double>::check_( src )) {
            auto numpyArray = pybind11::array_t<cie::Double, pybind11::array::c_style |
                                                        pybind11::array::forcecast>::ensure( src );

            if ( numpyArray && numpyArray.ndim( ) == 2 ) {
                cie::Size rowSize = numpyArray.shape( )[0];
                cie::Size columnSize = numpyArray.shape( )[1];

                // value is a member defined by the PYBIND11_TYPE_CASTER macro
                value = cie::linalg::DynamicEigenMatrix<double>(
                    cie::linalg::DynamicEigenMatrix<double>::Wrapped::Zero(rowSize, columnSize)
                );

                // Copy matrix
                for( cie::Size i = 0; i < rowSize; i++ ) {
                    for( cie::Size j = 0; j < columnSize; j++ ) {
                        value(i, j) = numpyArray.at( i, j );
                    }
                }

                return true; // success
            }
        }
        return false; // failure
    }

    // Conversion from our C++ cie::linalg::Matrix to python numpy array
    static pybind11::handle cast(const cie::linalg::DynamicEigenMatrix<cie::Double>& src,
                                 pybind11::return_value_policy policy,
                                 pybind11::handle parent)
    {
        // Construct array with shape ( rowSize, columnSize ) from the given cie::linalg::Matrix
        return pybind11::array( std::vector<cie::Size>{ src.rowSize( ), src.columnSize( ) },
                                &const_cast<cie::linalg::DynamicEigenMatrix<cie::Double>&>( src )( 0, 0 ) ).release( );
    }
};

} // detail
} // pybind11

#endif // CIE_DENSE_MATRIX_CONVERSION
