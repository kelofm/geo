// --- Utility Includes ---
#include "packages/testing/inc/essentials.hpp"

// --- Internal Includes ---
#include "packages/primitives/inc/Ellipsoid.hpp"


namespace cie::geo {


CIE_TEST_CASE( "Ellipsoid", "[primitives]" )
{
    CIE_TEST_CASE_INIT( "Ellipsoid" )

    const Size Dimension = 2;
    using CoordinateType = Double;
    using PrimitiveType  = boolean::Ellipsoid<Dimension,CoordinateType>;

    using PointType      = PrimitiveType::Point;

    const PointType center { 1.0, 2.0 };
    const PointType radii { 3.0, 4.0 };

    PrimitiveType primitive( center, radii );

    CIE_TEST_CHECK( !primitive.evaluate( {-2.1, 2.0} ) );
    CIE_TEST_CHECK( !primitive.evaluate( {4.1, 2.0} ) );

    CIE_TEST_CHECK( primitive.evaluate( {-1.9, 2.0} ) );
    CIE_TEST_CHECK( primitive.evaluate( {3.9, 2.0} ) );

    CIE_TEST_CHECK( primitive.evaluate( {1.0, -1.9} ) );
    CIE_TEST_CHECK( primitive.evaluate( {1.0, 5.9} ) );

    CIE_TEST_CHECK( !primitive.evaluate( {1.0, -2.1} ) );
    CIE_TEST_CHECK( !primitive.evaluate( {1.0, 6.1} ) );
}


} // namespace cie::geo