// --- Linalg Includes ---
#include "packages/types/inc/matrix.hpp"
#include "packages/utilities/inc/linalghelper.hpp"

// --- Utility Includes ---
#include "packages/testing/inc/essentials.hpp"
#include "packages/macros/inc/fileio.hpp"

// --- Internal Includes ---
#include "packages/spline/inc/interpolation.hpp"

// --- STL Includes ---
#include <algorithm>


namespace cie::geo {

CIE_TEST_CASE( "centripetal parameter positions", "[splinekernel]" )
{
    CIE_TEST_CASE_INIT( "centripetal parameter positions" )

    // Define points to be interpolated
    ControlPoints2D interpolationPoints;
    interpolationPoints[0] = { 0.0, 15.0, 171.0, 307.0, 907.0 }; // x-coordinates
    interpolationPoints[1] = { 0.0, 20.0, 85.0, 340.0, 515.0 };  // y-coordinates

    // Get parameter positions
    std::vector<double> parameterPositions(interpolationPoints[0].size());

    CIE_TEST_REQUIRE_NOTHROW(parameterPositions = centripetalParameterPositions(interpolationPoints));

    // Check parameter positions
    double d = 5.0 + 13.0 + 17.0 + 25.0; // d = 60

    CIE_TEST_REQUIRE(parameterPositions.size() == interpolationPoints[0].size());

    CIE_TEST_CHECK(parameterPositions[0] == Approx(0.0));
    CIE_TEST_CHECK(parameterPositions[1] == Approx(5.0 / d));
    CIE_TEST_CHECK(parameterPositions[2] == Approx((5.0 + 13.0) / d));
    CIE_TEST_CHECK(parameterPositions[3] == Approx((5.0 + 13.0 + 17.0) / d));
    CIE_TEST_CHECK(parameterPositions[4] == Approx(1.0));
}

CIE_TEST_CASE( "averaged knot vector", "[splinekernel]" )
{
    CIE_TEST_CASE_INIT( "averaged knot vector" )

    // Define parameter positions
    std::vector<double> parameterPositions{ 0.0, 1.0 / 2.0, 2.0 / 3.0, 3.0 / 4.0, 1.0 };
    std::vector<double> knotVector;

    // Test for p = 2
    size_t p = 2;

    CIE_TEST_REQUIRE_NOTHROW(knotVector = knotVectorUsingAveraging(parameterPositions, p));

    CIE_TEST_REQUIRE(knotVector.size() == parameterPositions.size() + p + 1);

    CIE_TEST_CHECK(knotVector[0] == Approx(0.0));
    CIE_TEST_CHECK(knotVector[1] == Approx(0.0));
    CIE_TEST_CHECK(knotVector[2] == Approx(0.0));
    CIE_TEST_CHECK(knotVector[3] == Approx(7.0 / 12.0));
    CIE_TEST_CHECK(knotVector[4] == Approx(17.0 / 24.0));
    CIE_TEST_CHECK(knotVector[5] == Approx(1.0));
    CIE_TEST_CHECK(knotVector[6] == Approx(1.0));
    CIE_TEST_CHECK(knotVector[7] == Approx(1.0));

    // Test for p = 3
    p = 3;

    CIE_TEST_REQUIRE_NOTHROW(knotVector = knotVectorUsingAveraging(parameterPositions, p));

    CIE_TEST_REQUIRE(knotVector.size() == parameterPositions.size() + p + 1);

    CIE_TEST_CHECK(knotVector[0] == Approx(0.0));
    CIE_TEST_CHECK(knotVector[1] == Approx(0.0));
    CIE_TEST_CHECK(knotVector[2] == Approx(0.0));
    CIE_TEST_CHECK(knotVector[3] == Approx(0.0));
    CIE_TEST_CHECK(knotVector[4] == Approx(23.0 / 36.0));
    CIE_TEST_CHECK(knotVector[5] == Approx(1.0));
    CIE_TEST_CHECK(knotVector[6] == Approx(1.0));
    CIE_TEST_CHECK(knotVector[7] == Approx(1.0));
    CIE_TEST_CHECK(knotVector[8] == Approx(1.0));

    // Test for p = 4 (no internal knots)
    p = 4;

    CIE_TEST_REQUIRE_NOTHROW(knotVector = knotVectorUsingAveraging(parameterPositions, p));

    CIE_TEST_REQUIRE(knotVector.size() == parameterPositions.size() + p + 1);

    CIE_TEST_CHECK(knotVector[0] == Approx(0.0));
    CIE_TEST_CHECK(knotVector[1] == Approx(0.0));
    CIE_TEST_CHECK(knotVector[2] == Approx(0.0));
    CIE_TEST_CHECK(knotVector[3] == Approx(0.0));
    CIE_TEST_CHECK(knotVector[4] == Approx(0.0));
    CIE_TEST_CHECK(knotVector[5] == Approx(1.0));
    CIE_TEST_CHECK(knotVector[6] == Approx(1.0));
    CIE_TEST_CHECK(knotVector[7] == Approx(1.0));
    CIE_TEST_CHECK(knotVector[8] == Approx(1.0));
    CIE_TEST_CHECK(knotVector[9] == Approx(1.0));

    CIE_TEST_CHECK_THROWS(knotVectorUsingAveraging(parameterPositions, 5));
}

CIE_TEST_CASE( "interpolate with BSpline", "[splinekernel]" )
{
    CIE_TEST_CASE_INIT( "interpolate with BSpline" )

    ControlPoints2D interpolationPoints;
    ControlPointsAndKnotVector result;

    // Define points to be interpolated
    interpolationPoints[0] = { 0.0, 15.0, 171.0, 307.0,907.0 }; // x-coordinates
    interpolationPoints[1] = { 0.0, 20.0, 85.0, 340.0, 515.0 }; // y-coordinates

    double d = 5.0 + 13.0 + 17.0 + 25.0; // d = 60

    // Check p = 1
    size_t p = 1;

    // Perform interpolation
    CIE_TEST_REQUIRE_NOTHROW(result = interpolateWithBSplineCurve(interpolationPoints, p));

    // Check control points (should be the same as the interpolation points for p = 1)
    CIE_TEST_REQUIRE(result.first[0].size() == interpolationPoints[0].size()); // Check # of x-components
    CIE_TEST_REQUIRE(result.first[1].size() == interpolationPoints[1].size()); // Check # of y-components

    for (size_t i = 0; i < result.first[0].size(); ++i)
    {
        CIE_TEST_CHECK(result.first[0][i] == interpolationPoints[0][i]); // Check x component
        CIE_TEST_CHECK(result.first[1][i] == interpolationPoints[1][i]); // Check y component
    }

    // Check knot vector
    CIE_TEST_REQUIRE(result.second.size() == interpolationPoints[0].size() + p + 1);

    CIE_TEST_CHECK(result.second[0] == Approx(0.0));
    CIE_TEST_CHECK(result.second[1] == Approx(0.0));
    CIE_TEST_CHECK(result.second[2] == Approx(5.0 / d));
    CIE_TEST_CHECK(result.second[3] == Approx((5.0 + 13) / d));
    CIE_TEST_CHECK(result.second[4] == Approx((5.0 + 13 + 17) / d));
    CIE_TEST_CHECK(result.second[5] == Approx(1.0));
    CIE_TEST_CHECK(result.second[6] == Approx(1.0));

    // Check p = 3
    p = 3;

    // Perform interpolation
    CIE_TEST_REQUIRE_NOTHROW(result = interpolateWithBSplineCurve(interpolationPoints, p));

    // Check control points
    CIE_TEST_REQUIRE(result.first[0].size() == interpolationPoints[0].size()); // Check # of x-components
    CIE_TEST_REQUIRE(result.first[1].size() == interpolationPoints[1].size()); // Check # of y-components

    // Check x components
    CIE_TEST_CHECK(result.first[0][0] == Approx(0.0));
    CIE_TEST_CHECK(result.first[0][1] == Approx(-12.5687));
    CIE_TEST_CHECK(result.first[0][2] == Approx(383.886));
    CIE_TEST_CHECK(result.first[0][3] == Approx(236.859));
    CIE_TEST_CHECK(result.first[0][4] == Approx(907.0));

    // Check y components
    CIE_TEST_CHECK(result.first[1][0] == Approx(0.0));
    CIE_TEST_CHECK(result.first[1][1] == Approx(31.9598));
    CIE_TEST_CHECK(result.first[1][2] == Approx(30.3872));
    CIE_TEST_CHECK(result.first[1][3] == Approx(672.775));
    CIE_TEST_CHECK(result.first[1][4] == Approx(515.0));

    // Check knot vector
    CIE_TEST_REQUIRE(result.second.size() == interpolationPoints[0].size() + p + 1);

    CIE_TEST_CHECK(result.second[0] == Approx(0.0));
    CIE_TEST_CHECK(result.second[1] == Approx(0.0));
    CIE_TEST_CHECK(result.second[2] == Approx(0.0));
    CIE_TEST_CHECK(result.second[3] == Approx(0.0));
    CIE_TEST_CHECK(result.second[4] == Approx((3 * 5.0 + 2 * 13.0 + 17.0) / 3 / d));
    CIE_TEST_CHECK(result.second[5] == Approx(1.0));
    CIE_TEST_CHECK(result.second[6] == Approx(1.0));
    CIE_TEST_CHECK(result.second[7] == Approx(1.0));
    CIE_TEST_CHECK(result.second[8] == Approx(1.0));

    // Check p = 4 (no inner knots )
    p = 4;

    // Perform interpolation
    CIE_TEST_REQUIRE_NOTHROW(result = interpolateWithBSplineCurve(interpolationPoints, p));

    // Check control points
    CIE_TEST_REQUIRE(result.first[0].size() == interpolationPoints[0].size()); // Check # of x-components
    CIE_TEST_REQUIRE(result.first[1].size() == interpolationPoints[1].size()); // Check # of y-components

    // Check x components
    CIE_TEST_CHECK(result.first[0][0] == Approx(0.0));
    CIE_TEST_CHECK(result.first[0][1] == Approx(-38.5422077922));
    CIE_TEST_CHECK(result.first[0][2] == Approx(718.2705627706));
    CIE_TEST_CHECK(result.first[0][3] == Approx(-139.3798701299));
    CIE_TEST_CHECK(result.first[0][4] == Approx(907.0));

    // Check y components
    CIE_TEST_CHECK(result.first[1][0] == Approx(0.0));
    CIE_TEST_CHECK(result.first[1][1] == Approx(80.2976190476));
    CIE_TEST_CHECK(result.first[1][2] == Approx(-71.8650793651));
    CIE_TEST_CHECK(result.first[1][3] == Approx(883.5119047619));
    CIE_TEST_CHECK(result.first[1][4] == Approx(515.0));

    // Check knot vector
    CIE_TEST_REQUIRE(result.second.size() == interpolationPoints[0].size() + p + 1);

    CIE_TEST_CHECK(result.second[0] == Approx(0.0));
    CIE_TEST_CHECK(result.second[1] == Approx(0.0));
    CIE_TEST_CHECK(result.second[2] == Approx(0.0));
    CIE_TEST_CHECK(result.second[3] == Approx(0.0));
    CIE_TEST_CHECK(result.second[4] == Approx(0.0));
    CIE_TEST_CHECK(result.second[5] == Approx(1.0));
    CIE_TEST_CHECK(result.second[6] == Approx(1.0));
    CIE_TEST_CHECK(result.second[7] == Approx(1.0));
    CIE_TEST_CHECK(result.second[8] == Approx(1.0));
    CIE_TEST_CHECK(result.second[9] == Approx(1.0));

    // Check for inconsistent input component sizes
    interpolationPoints[0] = { 0, 1, 2, 3, 4, 5 };
    interpolationPoints[1] = { 0, 1, 2, 3, 4 };

    CIE_TEST_REQUIRE_THROWS(result = interpolateWithBSplineCurve(interpolationPoints, p));

    interpolationPoints[0] = { 0, 1, 2, 3, 4 };
    interpolationPoints[1] = { 0, 1, 2, 3, 4 };

    // Check for invalid polynomial degrees
    CIE_TEST_REQUIRE_THROWS(result = interpolateWithBSplineCurve(interpolationPoints, 0));
    CIE_TEST_REQUIRE_THROWS(result = interpolateWithBSplineCurve(interpolationPoints, 5));

    // Check if parameter positions and knot vectors are sorted
    std::vector<double> parameterPositions, knotVector;

    CIE_TEST_REQUIRE_NOTHROW(parameterPositions = centripetalParameterPositions(interpolationPoints));

    CIE_TEST_CHECK(is_sorted(parameterPositions.begin(), parameterPositions.end()));

    CIE_TEST_REQUIRE_NOTHROW(knotVector = knotVectorUsingAveraging(parameterPositions, p));

    CIE_TEST_CHECK(is_sorted(knotVector.begin(), knotVector.end()));
}

CIE_TEST_CASE( "interpolate with BSpline surface", "[splinekernel]" )
{
    CIE_TEST_CASE_INIT( "interpolate with BSpline surface" )

    // Interpolation points
    linalg::Matrix xGrid(
        {
            -3.0,	-3.0,	-3.0,
            -1.0,	-1.0,	-1.0,
            1.0,	1.0,	1.0,
            3.0,	3.0,	3.0
        },
        4
    );
    linalg::Matrix yGrid(
        {
            -1.0,	0.0,	1.0,
            -1.0,	0.0,	1.0,
            -1.0,	0.0,	1.0,
            -1.0,	0.0,	1.0
        },
        4
    );
    linalg::Matrix zGrid(
        {
            1.0,	1.0,	1.0,
            1.0,	49.0,	1.0,
            1.0,	49.0,	1.0,
            1.0,	1.0,	1.0,
        },
        4
        );
    VectorOfMatrices grid({ xGrid,yGrid,zGrid });

    CIE_TEST_FILE_OUTPUT( "interpolateWithBSplineSurface.grid_in.csv",
        linalg::linalghelper::write( grid[0], TEST_FILE );
    )

    //std::cout << "\n";
    // Polynomial degrees
    size_t polynomialDegreeR(3), polynomialDegreeS(2);
    // Test parameter positions
    VectorPair parameterPositionsRS = centripetalParameterPositions(grid);
    for (size_t i = 0; i < parameterPositionsRS[0].size(); ++i) {
        CIE_TEST_CHECK( parameterPositionsRS[0][i] == Approx(i/3.0) );
    }
    for (size_t i = 0; i < parameterPositionsRS[1].size(); ++i) {
        CIE_TEST_CHECK( parameterPositionsRS[1][i] == Approx(i/2.0) );
    }
    // Get knot vectors
    VectorPair knotVectors({ knotVectorUsingAveraging(parameterPositionsRS[0],polynomialDegreeR),knotVectorUsingAveraging(parameterPositionsRS[1], polynomialDegreeS) });
    // Interpolate
    ControlPointsAndKnotVector3D outputPair = interpolateWithBSplineSurface(grid, polynomialDegreeR, polynomialDegreeS);

    CIE_TEST_FILE_OUTPUT( "interpolateWithBSplineSurface.grid_out.csv",
        linalg::linalghelper::write( grid[0], TEST_FILE );
    )

    // Check values
    for (size_t i = 0; i < parameterPositionsRS[0].size(); ++i) {
        for (size_t j = 0; j < parameterPositionsRS[1].size(); ++j) {
            // Check x
            //CIE_TEST_CHECK(outputPair.first[0](i, j) == Approx(xGrid(i, j)));
        }
    }
}

} // cie::geo
