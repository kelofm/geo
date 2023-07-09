// --- Linalg Includes ---
#include "packages/types/inc/matrix.hpp"
#include "packages/solvers/inc/gaussiansolver.hpp"

// --- Utility Includes ---
#include "packages/macros/inc/exceptions.hpp"
#include "packages/stl_extension/inc/StaticArray.hpp"

// --- Internal Includes ---
#include "packages/spline/inc/interpolation.hpp"
#include "packages/spline/inc/basisfunctions.hpp"
#include "cmake_variables.hpp"

// --- STL Includes ---
#include <string>
#include <cmath>
#include <exception>
#include <fstream>


namespace cie::geo {



ControlPointsAndKnotVector3D interpolateWithBSplineSurface(const VectorOfMatrices& interpolationPoints,
                                                           size_t polynomialDegreeR,
                                                           size_t polynomialDegreeS) {
    // Get sizes
    size_t sizeR(interpolationPoints[0].size1());
    size_t sizeS(interpolationPoints[0].size2());
    size_t numberOfPoints(sizeR*sizeS);
    // Get parameter positions at interpolation points
    StaticArray<std::vector<double>,2> parameterPositionsRS = centripetalParameterPositions(interpolationPoints);
    // Get knot vectors
    std::vector<double> knotVectorR(knotVectorUsingAveraging(parameterPositionsRS[0], polynomialDegreeR));
    std::vector<double> knotVectorS(knotVectorUsingAveraging(parameterPositionsRS[1], polynomialDegreeS));
    // Assemble matrix
    double temp;
    size_t column, pR, pS;
    linalg::Matrix shapeFunctionMatrix(numberOfPoints, numberOfPoints);
    shapeFunctionMatrix(0, 0) = 1.0;
    for (size_t row = 1; row < numberOfPoints-1; ++row) {
        pR = row % sizeR;
        pS = row / sizeR;
        for (size_t s = 0; s < sizeS; ++s) {
            temp = evaluateBSplineBasis(parameterPositionsRS[1][pS], s, polynomialDegreeS, knotVectorS);
            for (size_t r = 0; r < sizeR; ++r) {

                column = s * sizeR + r;
                shapeFunctionMatrix(row, column) = evaluateBSplineBasis(parameterPositionsRS[0][pR], r, polynomialDegreeR, knotVectorR) * temp;

            }
        }
    }
    shapeFunctionMatrix(numberOfPoints-1, numberOfPoints-1) = 1.0;

    // Solve for components
    VectorOfMatrices controlPoints(3);
    controlPoints[0] = linalg::Matrix(
        linalg::solve(shapeFunctionMatrix, interpolationPoints[0].data()),
        sizeR
    );
    controlPoints[1] = linalg::Matrix(
        linalg::solve(shapeFunctionMatrix, interpolationPoints[1].data()),
        sizeR
    );
    controlPoints[2] = linalg::Matrix(
        linalg::solve(shapeFunctionMatrix, interpolationPoints[2].data()),
        sizeR
    );
    //
    VectorPair knotVectors({ knotVectorR,knotVectorS });
    return std::make_pair(controlPoints,knotVectors);
}



ControlPointsAndKnotVector interpolateWithBSplineCurve( const ControlPoints2D& interpolationPoints,
                                                    size_t polynomialDegree )
{
    size_t numberOfPoints = interpolationPoints[0].size( );

    if( interpolationPoints[1].size( ) != numberOfPoints )
        CIE_THROW( OutOfRangeException, "Inconsistent sizes in interpolate Curve" )

    std::vector<double> parameterPositions = centripetalParameterPositions( interpolationPoints );
    std::vector<double> knotVector = knotVectorUsingAveraging( parameterPositions, polynomialDegree );

    linalg::Matrix N( numberOfPoints, numberOfPoints );

    // Set up interpolation matrix
    for( size_t iInterpolationPoint = 0; iInterpolationPoint < numberOfPoints; ++iInterpolationPoint )
    {
        for( size_t iControlPoint = 0; iControlPoint < numberOfPoints; ++iControlPoint )
        {
            N( iInterpolationPoint, iControlPoint ) = evaluateBSplineBasis( parameterPositions[iInterpolationPoint],
                iControlPoint, polynomialDegree, knotVector );

        } // iControlPoint
    } // iInterpolationPoint

    ControlPoints2D controlPoints;

    // First x, then y coordinates
    for( size_t iAxis = 0; iAxis < 2; ++iAxis )
    {
        controlPoints[iAxis] = linalg::solve( N, interpolationPoints[iAxis] );

    } // iAxis

    return { controlPoints, knotVector };
}



std::vector<double> centripetalParameterPositions( const ControlPoints2D& interpolationPoints )
    {
    size_t numberOfPoints = interpolationPoints[0].size( );

    std::vector<double> parameterPositions( numberOfPoints, 0.0 );

    for( size_t k = 1; k < numberOfPoints; ++k )
    {
        double dx = interpolationPoints[0][k] - interpolationPoints[0][k - 1];
        double dy = interpolationPoints[1][k] - interpolationPoints[1][k - 1];

        double dk = std::sqrt( dx * dx + dy * dy );

        parameterPositions[k] = parameterPositions[k - 1] + std::sqrt( dk );
    }

    for( size_t k = 1; k < numberOfPoints; ++k )
    {
        parameterPositions[k] /= parameterPositions.back( );
    }

    return parameterPositions;
}



StaticArray<std::vector<double>, 2> centripetalParameterPositions(const VectorOfMatrices& interpolationPoints)
    {
    size_t sizeR(interpolationPoints[0].size1());
    size_t sizeS(interpolationPoints[0].size2());
    std::vector<double> parameterPositionsR(sizeR, 0.0);
    std::vector<double> parameterPositionsS(sizeS, 0.0);
    double dx, dy, dz, d;
    for (size_t i = 1; i < sizeR; ++i) {
        dx = interpolationPoints[0](i, 0) - interpolationPoints[0](i - 1, 0);
        dy = interpolationPoints[1](i, 0) - interpolationPoints[1](i - 1, 0);
        dz = interpolationPoints[2](i, 0) - interpolationPoints[2](i - 1, 0);
        d = std::sqrt(dx*dx + dy*dy + dz*dz);
        parameterPositionsR[i] = parameterPositionsR[i - 1] + std::sqrt(d);
    }
    for (size_t i = 1; i < sizeS; ++i) {
        dx = interpolationPoints[0](0, i) - interpolationPoints[0](0, i - 1);
        dy = interpolationPoints[1](0, i) - interpolationPoints[1](0, i - 1);
        dz = interpolationPoints[2](0, i) - interpolationPoints[2](0, i - 1);
        d = std::sqrt(dx*dx + dy * dy + dz * dz);
        parameterPositionsS[i] = parameterPositionsS[i - 1] + std::sqrt(d);
    }
    for (auto i = parameterPositionsR.begin(); i != parameterPositionsR.end(); ++i) {
        *i /= parameterPositionsR.back();
    }
    for (auto i = parameterPositionsS.begin(); i != parameterPositionsS.end(); ++i) {
        *i /= parameterPositionsS.back();
    }
    return { parameterPositionsR, parameterPositionsS };
}



std::vector<double> knotVectorUsingAveraging( const std::vector<double>& parameterPositions,
                                              size_t polynomialDegree )
{
    size_t numberOfPoints = parameterPositions.size( );
    size_t numberOfKnots = numberOfPoints + polynomialDegree + 1;
    size_t numberOfInnerKnots = numberOfPoints - polynomialDegree - 1;

    if( polynomialDegree >= numberOfPoints )
        CIE_THROW(
            std::runtime_error,
            "Polynomial degree " + std::to_string( polynomialDegree ) +
                                  " is too high for " + std::to_string( numberOfPoints ) +
                                  " interpolation points."
        )

    std::vector<double> knotVector( numberOfKnots, 0.0 );

    for( size_t i = 0; i < numberOfInnerKnots; ++i )
    {
        double sum = 0.0;

        for( size_t j = 1; j < polynomialDegree + 1; ++j )
        {
            sum += parameterPositions[i + j];
        }

        knotVector[i + polynomialDegree + 1] = sum / polynomialDegree;
    }

    for( size_t i = 0; i < polynomialDegree + 1; ++i )
    {
        knotVector[numberOfInnerKnots + polynomialDegree + 1 + i] = 1.0;
    }

    return knotVector;
}



} // cie::geo
