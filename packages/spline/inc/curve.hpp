#ifndef CIE_GEO_SPLINE_CURVE_HPP
#define CIE_GEO_SPLINE_CURVE_HPP

// --- Utility Includes ---
#include "packages/stl_extension/inc/StaticArray.hpp"

// --- STL Includes ---
#include <vector>

namespace cie::geo {

/** @brief Evaluate B-Spline curve by summing up basis functions times control points.
 *  @param tCoordinates Parametric coordinates at which the curve is evaluated.
 *  @param xCoordinates The @p x coordinates of the control points.
 *  @param yCoordinates The @p y coordinates of the control points.
 *  @param knotVector Parametric coordinates of the control points.
 *  @return A vector of @p x and a vector of @p y coordinates with one value for each parametric
 *          coordinate tCoordinates.
 */
StaticArray<std::vector<double>, 2> evaluate2DCurve(const std::vector<double>& tCoordinates,
                                                    const std::vector<double>& xCoordinates,
                                                    const std::vector<double>& yCoordinates,
                                                    const std::vector<double>& knotVector);

//! Identical to evaluate2DCurve, but using De Boor's algorithm.
StaticArray<std::vector<double>, 2> evaluate2DCurveDeBoor( const std::vector<double>& tCoordinates,
                                                          const std::vector<double>& xCoordinates,
                                                          const std::vector<double>& yCoordinates,
                                                          const std::vector<double>& knotVector );

/*! De Boor's algorithm for evaluating (x, y) at one parametric coordinate t. The parameter
 *  recursionLevel has a default value of 1, which will be used if no argument is passed. */
StaticArray<double, 2> deBoor( double t,
                              size_t knotSpanIndex,
                              size_t polynomialDegree,
                              const std::vector<double>& knotVector,
                              const std::vector<double>& xCoordinates,
                              const std::vector<double>& yCoordinates,
                              size_t recursionLevel = 1 );

//! Same as above but without recursion.
StaticArray<double, 2> deBoorOptimized( double t,
                                       size_t i,
                                       size_t p,
                                       const std::vector<double>& knotVector,
                                       const std::vector<double>& xCoordinates,
                                       const std::vector<double>& yCoordinates );

//! Determines the knot span of the parametric coordinate t.
size_t findKnotSpan( double t,
                     size_t numberOfControlPoints,
                     const std::vector<double>& knotVector );

} // cie::geo

#endif // CIE_CURVE_HPP
