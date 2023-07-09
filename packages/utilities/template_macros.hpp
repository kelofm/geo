#ifndef CIE_GEO_TEMPLATE_MACROS_HPP
#define CIE_GEO_TEMPLATE_MACROS_HPP

/**
 * A collection of macros for instantiating templates
 * with specified numeric types and dimensions.
 * 
 *  - dimensions:           1, 2, 3
 *  - coordinate types:     int, Size, float, double
 *  - value types:          bool, int, Size, float, double, std::complex<double>
 */

// --- Utility Includes ---
#include "packages/types/inc/types.hpp"

// --- STL Includes ---
#include <complex>


#define CIE_GEO_DETAIL_INSTANTIATE_TEMPLATE_DIMENSION_COORDINATE_0(CLASS_NAME, COORDINATE_TYPE)                     \
    template class CLASS_NAME<1, COORDINATE_TYPE>;                                                                  \
    template class CLASS_NAME<2, COORDINATE_TYPE>;                                                                  \
    template class CLASS_NAME<3, COORDINATE_TYPE>;


#define CIE_GEO_DETAIL_INSTANTIATE_TEMPLATE_DIMENSION_COORDINATE_1(CLASS_NAME)                                      \
    CIE_GEO_DETAIL_INSTANTIATE_TEMPLATE_DIMENSION_COORDINATE_0(CLASS_NAME, int)                                     \
    CIE_GEO_DETAIL_INSTANTIATE_TEMPLATE_DIMENSION_COORDINATE_0(CLASS_NAME, Size)                                    \
    CIE_GEO_DETAIL_INSTANTIATE_TEMPLATE_DIMENSION_COORDINATE_0(CLASS_NAME, float)                                   \
    CIE_GEO_DETAIL_INSTANTIATE_TEMPLATE_DIMENSION_COORDINATE_0(CLASS_NAME, double)                                  \


#define CIE_GEO_INSTANTIATE_POINT_TEMPLATE(CLASS_NAME)                                                              \
    CIE_GEO_DETAIL_INSTANTIATE_TEMPLATE_DIMENSION_COORDINATE_1(CLASS_NAME)


#define CIE_GEO_DETAIL_INSTANTIATE_TEMPLATE_DIMENSION_VALUE_COORDINATE_0(CLASS_NAME, VALUE_TYPE, COORDINATE_TYPE)   \
    template class CLASS_NAME<1, VALUE_TYPE, COORDINATE_TYPE>;                                                      \
    template class CLASS_NAME<2, VALUE_TYPE, COORDINATE_TYPE>;                                                      \
    template class CLASS_NAME<3, VALUE_TYPE, COORDINATE_TYPE>;


#define CIE_GEO_DETAIL_INSTANTIATE_TEMPLATE_DIMENSION_VALUE_COORDINATE_1(CLASS_NAME, VALUE_TYPE)                    \
    CIE_GEO_DETAIL_INSTANTIATE_TEMPLATE_DIMENSION_VALUE_COORDINATE_0(CLASS_NAME, VALUE_TYPE, int)                   \
    CIE_GEO_DETAIL_INSTANTIATE_TEMPLATE_DIMENSION_VALUE_COORDINATE_0(CLASS_NAME, VALUE_TYPE, Size)                  \
    CIE_GEO_DETAIL_INSTANTIATE_TEMPLATE_DIMENSION_VALUE_COORDINATE_0(CLASS_NAME, VALUE_TYPE, float)                 \
    CIE_GEO_DETAIL_INSTANTIATE_TEMPLATE_DIMENSION_VALUE_COORDINATE_0(CLASS_NAME, VALUE_TYPE, double)


#define CIE_GEO_DETAIL_INSTANTIATE_TEMPLATE_DIMENSION_VALUE_COORDINATE_2(CLASS_NAME)                                \
    CIE_GEO_DETAIL_INSTANTIATE_TEMPLATE_DIMENSION_VALUE_COORDINATE_1(CLASS_NAME, Bool)                              \
    CIE_GEO_DETAIL_INSTANTIATE_TEMPLATE_DIMENSION_VALUE_COORDINATE_1(CLASS_NAME, int)                               \
    CIE_GEO_DETAIL_INSTANTIATE_TEMPLATE_DIMENSION_VALUE_COORDINATE_1(CLASS_NAME, Size)                              \
    CIE_GEO_DETAIL_INSTANTIATE_TEMPLATE_DIMENSION_VALUE_COORDINATE_1(CLASS_NAME, float)                             \
    CIE_GEO_DETAIL_INSTANTIATE_TEMPLATE_DIMENSION_VALUE_COORDINATE_1(CLASS_NAME, double)                            \
    CIE_GEO_DETAIL_INSTANTIATE_TEMPLATE_DIMENSION_VALUE_COORDINATE_1(CLASS_NAME, std::complex<double>)


#define CIE_GEO_INSTANTIATE_POINT_VALUE_TEMPLATE(CLASS_NAME)                                                        \
    CIE_GEO_DETAIL_INSTANTIATE_TEMPLATE_DIMENSION_VALUE_COORDINATE_2(CLASS_NAME)


#endif