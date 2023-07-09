// --- Utility Includes ---
#include "packages/testing/inc/essentials.hpp"

// --- Internal Includes ---
#include "packages/clustering/inc/minimumdisc.hpp"

// --- STL Includes ---
#include <cmath>


namespace cie::geo {

CIE_TEST_CASE( "Disc", "[clustering]" )
{
    CIE_TEST_CASE_INIT( "Disc" )

    // Disc defined by center and radius
    Disc disc( {0.1,0.2}, 0.5 );
    CIE_TEST_CHECK( disc._center[0]     == Approx(0.1) );
    CIE_TEST_CHECK( disc._center[1]     == Approx(0.2) );
    CIE_TEST_CHECK( disc._radius2       == Approx(0.25) );

    // Disc defined by 2 points on the same diameter
    disc    = Disc({0.0,0.0},{1.0,1.0});
    CIE_TEST_CHECK( disc._center[0]     == Approx(0.5) );
    CIE_TEST_CHECK( disc._center[1]     == Approx(0.5) );
    CIE_TEST_CHECK( disc._radius2       == Approx(0.5) );

    // Disc defined by 3 points on its arc
    disc    = Disc( {1.0,1.0}, {2.0,2.0}, {3.0,1.0} );
    CIE_TEST_CHECK( disc._center[0]     == Approx(2.0) );
    CIE_TEST_CHECK( disc._center[1]     == Approx(1.0) );
    CIE_TEST_CHECK( disc._radius2       == Approx(1.0) );

    // Extreme cases
    DoubleArray<2> point1   = {1.0,1.0};
    DoubleArray<2> point2   = {1.0,1.0};
    DoubleArray<2> point3   = {1.0,2.0};

    disc =  Disc(point1,point2);
    CIE_TEST_CHECK( disc._center[0]  == Approx( 1.0 ) );
    CIE_TEST_CHECK( disc._center[1]  == Approx( 1.0 ) );
    CIE_TEST_CHECK( disc._radius2    == Approx( 0.0 ) );

    disc = Disc(point1,point3);
    CIE_TEST_CHECK( disc._center[0]  == Approx( 1.0 ) );
    CIE_TEST_CHECK( disc._center[1]  == Approx( 1.5 ) );
    CIE_TEST_CHECK( disc._radius2    == Approx( 0.25 ) );

    disc = Disc(point3,point1);
    CIE_TEST_CHECK( disc._center[0]  == Approx( 1.0 ) );
    CIE_TEST_CHECK( disc._center[1]  == Approx( 1.5 ) );
    CIE_TEST_CHECK( disc._radius2    == Approx( 0.25 ) );

    disc = Disc(point1,point2,point3);
    CIE_TEST_CHECK( disc._center[0]  == Approx( 1.0 ) );
    CIE_TEST_CHECK( disc._center[1]  == Approx( 1.5 ) );
    CIE_TEST_CHECK( disc._radius2    == Approx( 0.25 ) );

    disc = Disc(point1,point3,point2);
    CIE_TEST_CHECK( disc._center[0]  == Approx( 1.0 ) );
    CIE_TEST_CHECK( disc._center[1]  == Approx( 1.5 ) );
    CIE_TEST_CHECK( disc._radius2    == Approx( 0.25 ) );

    disc = Disc(point3,point2,point1);
    CIE_TEST_CHECK( disc._center[0]  == Approx( 1.0 ) );
    CIE_TEST_CHECK( disc._center[1]  == Approx( 1.5 ) );
    CIE_TEST_CHECK( disc._radius2    == Approx( 0.25 ) );

    // Disc with 3 equal points
    disc    = Disc(point1,point1,point1);
    CIE_TEST_CHECK( disc._center[0]  == Approx( 1.0 ) );
    CIE_TEST_CHECK( disc._center[1]  == Approx( 1.0 ) );
    CIE_TEST_CHECK( disc._radius2    == Approx( 0.0 ) );
}


/*
CIE_TEST_CASE("Minimum enclosing disc")
{
	// Create vector of points
	PointSet2D points = {	{0.0,0.0},{1.0,1.0},{3.0,3.0},
							{0.5,0.2},{-1.0,-1.0},{-3.0,-3.0},
							{-0.2,0.0},{1.0,-1.0},{0.4,1.2}		};

	// Find minimum enclosing disc
	MinimumEnclosingDisc minimumDisc(points);
	minimumDisc.build();
}
*/


} // namespace cie::geo