// --- Utility Includes ---
#include "packages/testing/inc/essentials.hpp"

// --- Internal Includes ---
#include "packages/primitives/inc/Sphere.hpp"


namespace cie::geo {


CIE_TEST_CASE( "Sphere", "[primitives]" )
{
    CIE_TEST_CASE_INIT( "Sphere" )

    const Size Dimension = 2;
    using CoordinateType = Double;
    using Primitive      = boolean::Sphere<Dimension,CoordinateType>;
    using Point          = Primitive::Point;

    Point center { 1.0, 1.0 };
    CoordinateType radius = 0.5;

    Point truePoint { 0.9, 0.9 };
    Point falsePoint { 0.2, 0.2 };

    CIE_TEST_REQUIRE_NOTHROW( Primitive( center, radius ) );
    Primitive primitive( center, radius );

    CIE_TEST_REQUIRE_NOTHROW( primitive.at(truePoint) );
    CIE_TEST_CHECK( !primitive.isDegenerate() );
    CIE_TEST_CHECK( primitive.at(truePoint) );
    CIE_TEST_CHECK( !primitive.at(falsePoint) );

    // Degenerate sphere
    CIE_TEST_REQUIRE_NOTHROW( Primitive(center, 0.0) );
    Primitive degenerate( center, 0.0 );
    CIE_TEST_REQUIRE_NOTHROW( degenerate.at(truePoint) );
    CIE_TEST_CHECK( degenerate.isDegenerate() );
    CIE_TEST_CHECK( !degenerate.at(truePoint) );
    CIE_TEST_CHECK( !degenerate.at(falsePoint) );

    // Invalid sphere
    #ifdef CIE_ENABLE_RUNTIME_GEOMETRY_CHECKS
    CIE_TEST_REQUIRE_THROWS( Primitive( center, -1.0 ) );
    #endif
}


} // namespace cie::geo