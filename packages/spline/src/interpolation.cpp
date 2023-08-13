// --- External Includes ---
#include "Eigen/Dense"

// --- Linalg Includes ---
#include "packages/matrix/inc/DynamicEigenMatrix.hpp"
#include "packages/spline/inc/interpolation.hpp"
#include "packages/spline/inc/basisfunctions.hpp"

// --- Utility Includes ---
#include "packages/macros/inc/exceptions.hpp"
#include "packages/stl_extension/inc/StaticArray.hpp"
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
    CIE_CHECK(!interpolationPoints.empty(), "")
    size_t sizeR(interpolationPoints[0].rowSize());
    size_t sizeS(interpolationPoints[0].columnSize());
    size_t numberOfPoints = sizeR * sizeS;

    for (const auto& r_matrix : interpolationPoints) {
        CIE_OUT_OF_RANGE_CHECK(r_matrix.rowSize() == sizeR, "")
        CIE_OUT_OF_RANGE_CHECK(r_matrix.columnSize() == sizeS, "")
    }

    // Get parameter positions at interpolation points
    StaticArray<std::vector<double>,2> parameterPositionsRS = centripetalParameterPositions(interpolationPoints);

    // Get knot vectors
    std::vector<double> knotVectorR(knotVectorUsingAveraging(parameterPositionsRS[0], polynomialDegreeR));
    std::vector<double> knotVectorS(knotVectorUsingAveraging(parameterPositionsRS[1], polynomialDegreeS));

    // Assemble matrix
    double temp;
    size_t column, pR, pS;
    linalg::DynamicEigenMatrix<double> shapeFunctionMatrix(numberOfPoints, numberOfPoints);
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
    const auto factorized = shapeFunctionMatrix.wrapped().partialPivLu();
    for (unsigned i=0; i<3; ++i) {
        const Eigen::Map<const Eigen::Matrix<double,Eigen::Dynamic,1>> flattened(
            interpolationPoints[i].wrapped().data(),
            numberOfPoints);
        controlPoints[i] = factorized.solve(flattened);
    }

    //
    VectorPair knotVectors({
        std::move(knotVectorR),
        std::move(knotVectorS)
    });
    return std::make_pair(
        std::move(controlPoints),
        std::move(knotVectors)
    );
}



ControlPointsAndKnotVector interpolateWithBSplineCurve(const ControlPoints2D& interpolationPoints,
                                                       size_t polynomialDegree)
{
    size_t numberOfPoints = interpolationPoints[0].size( );

    CIE_CHECK(interpolationPoints[1].size( ) == numberOfPoints,
              "Inconsistent sizes in interpolate curve: " << interpolationPoints[0].size() << " != " << interpolationPoints[1].size())

    CIE_CHECK(0 < polynomialDegree && polynomialDegree < interpolationPoints[0].size(),
              "Invalid polynomial degree " << polynomialDegree << " for " << interpolationPoints[0].size() << " points")

    std::vector<double> parameterPositions = centripetalParameterPositions( interpolationPoints );
    std::vector<double> knotVector = knotVectorUsingAveraging( parameterPositions, polynomialDegree );

    linalg::DynamicEigenMatrix<double> N( numberOfPoints, numberOfPoints );

    // Set up interpolation matrix
    for( size_t iInterpolationPoint = 0; iInterpolationPoint < numberOfPoints; ++iInterpolationPoint ) {
        for( size_t iControlPoint = 0; iControlPoint < numberOfPoints; ++iControlPoint ) {
            N( iInterpolationPoint, iControlPoint ) = evaluateBSplineBasis( parameterPositions[iInterpolationPoint],
                iControlPoint, polynomialDegree, knotVector );
        } // iControlPoint
    } // iInterpolationPoint

    ControlPoints2D controlPoints;

    // First x, then y coordinates
    const auto factorized = N.wrapped().partialPivLu();
    for( size_t i_axis = 0; i_axis < 2; ++i_axis ) {
        Eigen::Map<const Eigen::Matrix<double,Eigen::Dynamic,1>> rhs(interpolationPoints[i_axis].data(),
                                                                     interpolationPoints[i_axis].size());
        controlPoints[i_axis].resize(interpolationPoints[i_axis].size());
        Eigen::Map<Eigen::Matrix<double,Eigen::Dynamic,1>> result(controlPoints[i_axis].data(),
                                                                  controlPoints[i_axis].size());
        result = factorized.solve(rhs);
    } // i_axis

    return {controlPoints, knotVector};
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
    size_t sizeR(interpolationPoints[0].rowSize());
    size_t sizeS(interpolationPoints[0].columnSize());
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
