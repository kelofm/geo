// --- Utility Includes ---
#include "packages/testing/inc/essentials.hpp"
#include "packages/stl_extension/inc/StaticArray.hpp"

// --- Internal Includes ---
#include "packages/spline/inc/curve.hpp"

// --- STL Includes ---
#include <vector>


namespace cie::geo {

CIE_TEST_CASE( "find knot span", "[splinekernel]" )
{
    CIE_TEST_CASE_INIT( "find knot span" )

    std::vector<double> knotVector{ 0.0, 0.0, 0.0, 0.0, 1.0, 4.0, 9.0, 9.0, 9.0, 9.0 };

    std::vector<double> x{ 2.0, 3.0, 0.5, 0.2, 0.5, 3.0 };
    std::vector<double> y{ 1.0, 3.0, 3.0, 4.0, 5.0, 6.0 };

    // So p = 3 but thats irrelevant here;
    size_t n = x.size( );

    CIE_TEST_CHECK( findKnotSpan( 0.0, n, knotVector ) == 3 );
    CIE_TEST_CHECK( findKnotSpan( 0.2, n, knotVector ) == 3 );
    CIE_TEST_CHECK( findKnotSpan( 1.1, n, knotVector ) == 4 );
    CIE_TEST_CHECK( findKnotSpan( 9.0, n, knotVector ) == 5 );

    CIE_TEST_CHECK_THROWS(findKnotSpan(9.1, n, knotVector));
}

CIE_TEST_CASE( "DeBoor test", "[splinekernel]" )
{
    CIE_TEST_CASE_INIT( "deboor test" )

    std::vector<double> knotVector{ 0.0, 0.0, 0.0, 0.0, 1.0, 4.0, 9.0, 9.0, 9.0, 9.0 };

    std::vector<double> x{ 0.5, 10.0, 9.0, 4.5, 1.5, 1.0 };
    std::vector<double> y{ 0.5,  1.0, 4.0, 7.5, 6.0, 1.0 };

    StaticArray<double, 3> t{ 0.0, 1.1, 9.0 };

    StaticArray<double, 2> P;

    size_t n = x.size( );
    size_t m = knotVector.size();
    size_t p = m - n - 1;
    size_t s = 77;

    // For t0 (Beginning of curve)
    CIE_TEST_REQUIRE_NOTHROW( s = findKnotSpan( t[0], n, knotVector ) );
    CIE_TEST_REQUIRE_NOTHROW( P = deBoor( t[0], s, p, knotVector, x, y ) );

    // x-coordinates of curve
    CIE_TEST_CHECK( P[0] == Approx( x[0] ) );
    CIE_TEST_CHECK( P[1] == Approx( y[0] ) );

    // For t1 in the middle of the curve (see skript)
    CIE_TEST_REQUIRE_NOTHROW( s = findKnotSpan( t[1], n, knotVector ) );
    CIE_TEST_REQUIRE_NOTHROW( P = deBoor(t[1], s, p, knotVector, x, y ) );

    CIE_TEST_CHECK( P[0] == Approx(9.3419010416666683 ) );
    CIE_TEST_CHECK( P[1] == Approx(2.6049366319444447 ) );

    // For t3, so at the end of the curve
    CIE_TEST_REQUIRE_NOTHROW( s = findKnotSpan(t[2], n, knotVector ) );
    CIE_TEST_REQUIRE_NOTHROW( P = deBoor(t[2], s, p, knotVector, x, y ) );

    CIE_TEST_CHECK( P[0] == Approx( x.back( ) ) );
    CIE_TEST_CHECK( P[1] == Approx( y.back( ) ) );
}

CIE_TEST_CASE( "deboor curve test", "[splinekernel]" )
{
    CIE_TEST_CASE_INIT( "deboor curve test" )

    // This is the same test as in the curve test because given the same setup,
    // DeBoor should deliver the same points on a curve as using the classic way: N * P
    std::vector<double> knotVector{ 0.0, 0.0, 0.5, 1.0, 1.0 };
    std::vector<double> x{ 2.0, 3.0, 0.5 };
    std::vector<double> y{ 1.0, 3.0, 3.0 };

    std::vector<double> t{ 0.0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0 };

    StaticArray<std::vector<double>, 2> C;

    CIE_TEST_REQUIRE_NOTHROW( C = evaluate2DCurveDeBoor( t, x, y, knotVector ) );

    CIE_TEST_REQUIRE( C[0].size() == t.size( ) );
    CIE_TEST_REQUIRE( C[1].size() == t.size( ) );

    // x-coordinates of curve
    CIE_TEST_CHECK(C[0][0] == Approx( 2.0 ) );
    CIE_TEST_CHECK(C[0][1] == Approx( 2.2 ) );
    CIE_TEST_CHECK(C[0][2] == Approx( 2.4 ) );
    CIE_TEST_CHECK(C[0][3] == Approx( 2.6 ) );
    CIE_TEST_CHECK(C[0][4] == Approx( 2.8 ) );
    CIE_TEST_CHECK(C[0][5] == Approx( 3.0 ) );
    CIE_TEST_CHECK(C[0][6] == Approx( 2.5 ) );
    CIE_TEST_CHECK(C[0][7] == Approx( 2.0 ) );
    CIE_TEST_CHECK(C[0][8] == Approx( 1.5 ) );
    CIE_TEST_CHECK(C[0][9] == Approx( 1.0 ) );
    CIE_TEST_CHECK(C[0][10] == Approx( 0.5 ) );

    // y-coordinates of curve
    CIE_TEST_CHECK(C[1][0] == Approx( 1.0 ) );
    CIE_TEST_CHECK(C[1][1] == Approx( 1.4 ) );
    CIE_TEST_CHECK(C[1][2] == Approx( 1.8 ) );
    CIE_TEST_CHECK(C[1][3] == Approx( 2.2 ) );
    CIE_TEST_CHECK(C[1][4] == Approx( 2.6 ) );
    CIE_TEST_CHECK(C[1][5] == Approx( 3.0 ) );
    CIE_TEST_CHECK(C[1][6] == Approx( 3.0 ) );
    CIE_TEST_CHECK(C[1][7] == Approx( 3.0 ) );
    CIE_TEST_CHECK(C[1][8] == Approx( 3.0 ) );
    CIE_TEST_CHECK(C[1][9] == Approx( 3.0 ) );
    CIE_TEST_CHECK(C[1][10] == Approx( 3.0 ) );
}

CIE_TEST_CASE( "deboor curve script", "[splinekernel]" )
{
    CIE_TEST_CASE_INIT( "deboor curve script" )

    std::vector<double> knotVector { 0.0, 0.0, 0.0, 0.0, 1.0, 4.0, 9.0, 9.0, 9.0, 9.0 };
    std::vector<double> x { 0.0, 10.0, 9.0, 4.5, 1.5, 1.0};
    std::vector<double> y { 0.0,  1.0, 4.0, 7.5, 6.0, 1.0};
    std::vector<double> t { 0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0 };

    StaticArray<std::vector<double>, 2> C;

    CIE_TEST_REQUIRE_NOTHROW( C = evaluate2DCurveDeBoor( t, x, y, knotVector ) );

    CIE_TEST_REQUIRE( C[0].size( ) == t.size( ) );
    CIE_TEST_REQUIRE( C[1].size( ) == t.size( ) );

    // x-coordinates of curve
    CIE_TEST_CHECK( C[0][0] == Approx( 0.0 ) );
    CIE_TEST_CHECK( C[0][1] == Approx( 9.4375 ) );
    CIE_TEST_CHECK( C[0][2] == Approx( 8.3385 ) );
    CIE_TEST_CHECK( C[0][3] == Approx( 7.0208 ) );

    CIE_TEST_CHECK( C[0][5] == Approx( 4.336 ) );
    CIE_TEST_CHECK( C[0][6] == Approx( 3.1724 ) );
    CIE_TEST_CHECK( C[0][7] == Approx( 2.197 ) );
    CIE_TEST_CHECK( C[0][8] == Approx( 1.45713 ) );
    CIE_TEST_CHECK( C[0][9] == Approx( 1.0 ) );

    CIE_TEST_CHECK( C[1][0] == Approx( 0.0 ) );
    CIE_TEST_CHECK( C[1][1] == Approx( 2.40972 ) );
    CIE_TEST_CHECK( C[1][2] == Approx( 4.12413 ) );
    CIE_TEST_CHECK( C[1][3] == Approx( 5.33333 ) );
    CIE_TEST_CHECK( C[1][4] == Approx( 6.07378 ) );
    CIE_TEST_CHECK( C[1][5] == Approx( 6.35778 ) );
    CIE_TEST_CHECK( C[1][6] == Approx( 6.10094 ) );
    CIE_TEST_CHECK( C[1][7] == Approx( 5.19472 ) );
    CIE_TEST_CHECK( C[1][8] == Approx( 3.53059 ) );
    CIE_TEST_CHECK( C[1][9] == Approx( 1.0 ) );
}

} //cie::geo
