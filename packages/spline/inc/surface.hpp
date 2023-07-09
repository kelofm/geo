#ifndef CIE_GEO_SPLINE_SURFACE_HPP
#define CIE_GEO_SPLINE_SURFACE_HPP

// --- Linalg Includes ---
#include "packages/types/inc/matrix.hpp"

// --- Utility Includes ---
#include "packages/types/inc/types.hpp"
#include "packages/stl_extension/inc/StaticArray.hpp"

// --- STL Includes ---
#include <vector>
#include <array>
#include <algorithm>


namespace cie::geo {

using VectorOfMatrices = std::vector<linalg::Matrix<Double>>;

/* Evaluates a 2D B-Spline patch.
 * @param knotVectors Two knot vectors in r and s directions
 * @param controlPoints A vector of matrices for each control point component. The control points
 *                      of the patch form a 2D grid. They may have one single value when a scalar
 *                      field is represented or they may have multiple components when, e.g. x, y
 *                      and z coordinates are interpolated. Each matrix represents one component
 *                      (e.g. x-value) for all control points.
 * @param numberOfSamples The number of times the patch shall be evaluated in each coordinate
 *                        direction. For example passing (50, 50) evaluates the entire patch on a
 *                        50 x 50 grid.
 * @return A vector of matrices, similar to the controlPoints but now with the dimensions specified
 *         by numberOfSamples. In the above example we would return a 50 x 50 matrix of x-values,
 *         a 50 x 50 matrix of y-values, etc.
 */
VectorOfMatrices evaluateSurface( const StaticArray<std::vector<double>, 2>& knotVectors,
                                  const VectorOfMatrices& controlPoints,
                                  StaticArray<size_t, 2> numberOfSamplePoints );

} // cie::geo

#endif // CIE_SURFACE_HPP
