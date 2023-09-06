// --- Utility Includes ---
#include "packages/macros/inc/exceptions.hpp"
#include "packages/stl_extension/inc/StaticArray.hpp"

// --- Internal Includes ---
#include "packages/spline/inc/basisfunctions.hpp"
#include "packages/spline/inc/curve.hpp"

// --- STL Includes ---
#include <algorithm>
#include <array>
#include <cmath>
#include <string>
#include <stdexcept>

namespace cie::geo {

StaticArray<std::vector<double>, 2> evaluate2DCurve( const std::vector<double>& tCoordinates,
                                                    const std::vector<double>& xCoordinates,
                                                    const std::vector<double>& yCoordinates,
                                                    const std::vector<double>& knotVector )
{
    size_t numberOfSamples = tCoordinates.size( );
    size_t numberOfPoints = xCoordinates.size( );
    size_t m = knotVector.size( );
    size_t p = m - numberOfPoints - 1;

    if( yCoordinates.size( ) != numberOfPoints )
        CIE_THROW( OutOfRangeException, "Inconsistent size in evaluate2DCurve" )

    std::vector<double> curveX( numberOfSamples, 0.0 );
    std::vector<double> curveY( numberOfSamples, 0.0 );

    for( size_t i = 0; i < numberOfSamples; ++i )
    {
        double t = tCoordinates[i];

        for( size_t j = 0; j < numberOfPoints; ++j )
        {
            double N = evaluateBSplineBasis( t, j, p, knotVector );

            curveX[i] += N * xCoordinates[j];
            curveY[i] += N * yCoordinates[j];
        }
    }

    return { curveX, curveY };
}

StaticArray<double, 2> deBoorOptimized( double t,
                                       size_t knotSpanIndex,
                                       size_t polynomialDegree,
                                       const std::vector<double>& knotVector,
                                       const std::vector<double>& xCoordinates,
                                       const std::vector<double>& yCoordinates )
{
    // For performance reasons this should be static (or thread_local) or given by reference as target
    std::vector<double> dx, dy;

    dx.resize( polynomialDegree + 1 );
    dy.resize( polynomialDegree + 1 );

    for( size_t j = 0; j < polynomialDegree + 1; ++j )
    {
        dx[j] = xCoordinates[j + knotSpanIndex - polynomialDegree];
        dy[j] = yCoordinates[j + knotSpanIndex - polynomialDegree];
    }

    for( size_t r = 1; r < polynomialDegree + 1; ++r )
    {
        for( size_t j = polynomialDegree; j > r - 1; --j )
        {
            double tj = knotVector[j + knotSpanIndex - polynomialDegree];
            double alpha = ( tj - t ) / ( tj - knotVector[j + knotSpanIndex + 1 - r] );

            dx[j] = ( 1.0 - alpha ) * dx[j - 1] + alpha * dx[j];
            dy[j] = ( 1.0 - alpha ) * dy[j - 1] + alpha * dy[j];
        }
    }

    return { dx[polynomialDegree], dy[polynomialDegree] };
}

StaticArray<double, 2> deBoor( double t,
                              size_t knotSpanIndex,
                              size_t polynomialDegree,
                              const std::vector<double>& knotVector,
                              const std::vector<double>& xCoordinates,
                              const std::vector<double>& yCoordinates,
                              size_t refinementLevel )
{
    if( refinementLevel == polynomialDegree + 1 )
    {
        return { xCoordinates[knotSpanIndex], yCoordinates[knotSpanIndex] };
    }

    double a = ( t - knotVector[knotSpanIndex] ) / ( knotVector[knotSpanIndex + refinementLevel] - knotVector[knotSpanIndex] );

    StaticArray<double, 2> P1 = deBoor( t, knotSpanIndex - 1, polynomialDegree, knotVector, xCoordinates, yCoordinates, refinementLevel + 1 );
    StaticArray<double, 2> P2 = deBoor( t, knotSpanIndex, polynomialDegree, knotVector, xCoordinates, yCoordinates, refinementLevel + 1 );

    double Px = ( 1.0 - a ) * P1[0] + a * P2[0];
    double Py = ( 1.0 - a ) * P1[1] + a * P2[1];

    return { Px, Py};
}

size_t findKnotSpan( double t,
                     size_t numberOfControlPoints,
                     const std::vector<double>& knotVector )
{
    double tolerance = 1e-10;

    // Check if t resides within the allowed bounds
    if( t < knotVector.front( ) || t > knotVector.back( ) )
    {
        CIE_THROW(
            OutOfRangeException,
            "t out range: t = " + std::to_string( t ) +
                                 " but can only be within " + std::to_string( knotVector.front( ) ) +
                                 " and " + std::to_string( knotVector.back( ) ) + "\n"
            )
    }

    if( std::abs( t - knotVector[numberOfControlPoints + 1] ) < tolerance )
    {
        return numberOfControlPoints - 1;
    }

    auto result = std::upper_bound( knotVector.begin( ), knotVector.end( ), t );

    return std::distance( knotVector.begin( ), result - 1 );
}

StaticArray<std::vector<double>, 2> evaluate2DCurveDeBoor( const std::vector<double>& tCoordinates,
                                                          const std::vector<double>& xCoordinates,
                                                          const std::vector<double>& yCoordinates,
                                                          const std::vector<double>& knotVector )
{
    size_t numberOfSamples = tCoordinates.size( );
    size_t numberOfPoints = xCoordinates.size( );
    size_t m = knotVector.size( );
    size_t p = m - numberOfPoints - 1;

    if( yCoordinates.size( ) != numberOfPoints )
        CIE_THROW( OutOfRangeException, "Inconsistent size in evaluate2DCurveDeBoor" );

    std::vector<double> curveX( numberOfSamples, 0.0 );
    std::vector<double> curveY( numberOfSamples, 0.0 );

    //#pragma omp parallel for collapse(2)
    for( int i = 0; i < static_cast<int>(numberOfSamples); ++i ) {
        for( size_t j = 0; j < numberOfPoints; ++j ) {
            size_t s = findKnotSpan( tCoordinates[i], numberOfPoints, knotVector );
            StaticArray<double, 2> Point = deBoorOptimized( tCoordinates[i], s, p, knotVector, xCoordinates, yCoordinates );
            curveX[i] = Point[0];
            curveY[i] = Point[1];
        }
    }

    return {curveX, curveY};
}

} // cie::geo