#ifndef CIE_GEO_SPLINE_BSPLINE_HPP
#define CIE_GEO_SPLINE_BSPLINE_HPP

// --- STL Includes ---
#include <vector>
#include "stddef.h"

namespace cie::geo {

/*! Evaluates one B-Spline basis function.
 *  @param t The parametric coordinate
 *  @param i The basis function index
 *  @param p The polynomial degree
 *  @param knotVector
 *  @return The function value
 */
double evaluateBSplineBasis( double t, size_t i, size_t p, const std::vector<double>& knotVector );

//! Computes derivative of evaluateBSplineBasis, otherwise equivalent.
double evaluateBSplineDerivative( double t, size_t i, size_t p, const std::vector<double>& knotVector );

} // cie::geo

#endif // CIE_BSPLINE_HPP
