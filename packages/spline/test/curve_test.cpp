// --- Utility Includes ---
#include "packages/testing/inc/essentials.hpp"
#include "packages/stl_extension/inc/StaticArray.hpp"

// --- Internal Includes ---
#include "packages/spline/inc/curve.hpp"

// --- STL Includes ---
#include <array>
#include <vector>

namespace cie::geo {

CIE_TEST_CASE( "Linear interpolation curve", "[splinekernel]" )
{
    CIE_TEST_CASE_INIT( "Linear interpolation curve" )

    std::vector<double> knotVector{ 0.0, 0.0, 0.5, 1.0, 1.0 };
    std::vector<double> x{ 2.0, 3.0, 0.5 };
    std::vector<double> y{ 1.0, 3.0, 3.0 };
    std::vector<double> t{ 0.0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0 };
    StaticArray<std::vector<double>, 2> C;

    CIE_TEST_REQUIRE_NOTHROW( C = evaluate2DCurve( t, x, y, knotVector ) );

    CIE_TEST_REQUIRE( C[0].size( ) == t.size( ) );
    CIE_TEST_REQUIRE( C[1].size( ) == t.size( ) );

    // x-coordinates of curve
    CIE_TEST_CHECK( C[0][0]  == Approx( 2.0 ) );
    CIE_TEST_CHECK( C[0][1]  == Approx( 2.2 ) );
    CIE_TEST_CHECK( C[0][2]  == Approx( 2.4 ) );
    CIE_TEST_CHECK( C[0][3]  == Approx( 2.6 ) );
    CIE_TEST_CHECK( C[0][4]  == Approx( 2.8 ) );
    CIE_TEST_CHECK( C[0][5]  == Approx( 3.0 ) );
    CIE_TEST_CHECK( C[0][6]  == Approx( 2.5 ) );
    CIE_TEST_CHECK( C[0][7]  == Approx( 2.0 ) );
    CIE_TEST_CHECK( C[0][8]  == Approx( 1.5 ) );
    CIE_TEST_CHECK( C[0][9]  == Approx( 1.0 ) );
    CIE_TEST_CHECK( C[0][10] == Approx( 0.5 ) );

    // y-coordinates of curve
    CIE_TEST_CHECK( C[1][0]  == Approx( 1.0 ) );
    CIE_TEST_CHECK( C[1][1]  == Approx( 1.4 ) );
    CIE_TEST_CHECK( C[1][2]  == Approx( 1.8 ) );
    CIE_TEST_CHECK( C[1][3]  == Approx( 2.2 ) );
    CIE_TEST_CHECK( C[1][4]  == Approx( 2.6 ) );
    CIE_TEST_CHECK( C[1][5]  == Approx( 3.0 ) );
    CIE_TEST_CHECK( C[1][6]  == Approx( 3.0 ) );
    CIE_TEST_CHECK( C[1][7]  == Approx( 3.0 ) );
    CIE_TEST_CHECK( C[1][8]  == Approx( 3.0 ) );
    CIE_TEST_CHECK( C[1][9]  == Approx( 3.0 ) );
    CIE_TEST_CHECK( C[1][10] == Approx( 3.0 ) );
}

} // cie::geo
