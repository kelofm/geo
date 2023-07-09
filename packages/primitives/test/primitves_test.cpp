// --- Utility Includes ---
#include "packages/testing/inc/essentials.hpp"

// --- Linalg Includes ---
#include "packages/types/inc/vectortypes.hpp"
#include "packages/types/inc/arraytypes.hpp"

// --- Internal Includes ---
#include "packages/primitives/inc/Primitive.hpp"
#include "packages/primitives/inc/Box.hpp"
#include "packages/primitives/inc/Cube.hpp"

namespace cie::geo {

namespace boolean {


CIE_TEST_CASE( "Cube", "[primitives]" )
{
    CIE_TEST_CASE_INIT( "Cube" )

    const Size Dimension = 2;
    using CoordinateType = Double;
    using PrimitiveType  = Cube<Dimension,CoordinateType>;
    using PointType      = typename PrimitiveType::Point;

    PointType base{ 0.0, 0.0 };
    CoordinateType length;

    // Unit cube
    {
        length = 1.0;
        CIE_TEST_REQUIRE_NOTHROW( PrimitiveType(base, length) );
        PrimitiveType primitive( base, length );
        CIE_TEST_CHECK( !primitive.isDegenerate() );
        CIE_TEST_REQUIRE( primitive.base().size() == Dimension );
        CIE_TEST_CHECK( primitive.base()[0] == Approx(base[0]) );
        CIE_TEST_CHECK( primitive.base()[1] == Approx(base[1]) );
        CIE_TEST_CHECK( primitive.length() == Approx(length) );
    }

    // Degenerate cube
    {
        length = 0.0;
        CIE_TEST_REQUIRE_NOTHROW( PrimitiveType(base, length) );
        PrimitiveType primitive( base, length );
        CIE_TEST_CHECK( primitive.isDegenerate() );
        CIE_TEST_REQUIRE( primitive.base().size() == Dimension );
        CIE_TEST_CHECK( primitive.base()[0] == Approx(base[0]) );
        CIE_TEST_CHECK( primitive.base()[1] == Approx(base[1]) );
        CIE_TEST_CHECK( primitive.length() == Approx(length) );
    }

    // Invalid cube
    {
        #ifdef CIE_ENABLE_RUNTIME_GEOMETRY_CHECKS
        length = -1.0;
        CIE_TEST_CHECK_THROWS( PrimitiveType(base, length) );
        #endif
    }
}



CIE_TEST_CASE( "Box", "[primitives]" )
{
    CIE_TEST_CASE_INIT( "Box" )

    const Size Dimension = 2;
    using CoordinateType = Double;
    using PrimitiveType  = Box<Dimension,CoordinateType>;
    using PointType      = typename PrimitiveType::Point;

    PointType base{ 0.0, 0.0 };
    PointType lengths;

    // Unit cubeas box
    {
        lengths = { 1.0, 1.0 };
        CIE_TEST_REQUIRE_NOTHROW( PrimitiveType(base, lengths) );
        PrimitiveType primitive( base, lengths );
        CIE_TEST_CHECK( !primitive.isDegenerate() );
        CIE_TEST_REQUIRE( primitive.base().size() == Dimension );
        CIE_TEST_CHECK( primitive.base()[0] == Approx(base[0]) );
        CIE_TEST_CHECK( primitive.base()[1] == Approx(base[1]) );
        CIE_TEST_REQUIRE( primitive.lengths().size() == Dimension );
        CIE_TEST_CHECK( primitive.lengths()[0] == Approx(lengths[0]) );
        CIE_TEST_CHECK( primitive.lengths()[1] == Approx(lengths[1]) );
    }

    // Degenerate boxes
    {
        lengths = { 1.0, 0.0 };
        CIE_TEST_REQUIRE_NOTHROW( PrimitiveType(base, lengths) );
        PrimitiveType primitive( base, lengths );
        CIE_TEST_CHECK( primitive.isDegenerate() );

        lengths = { 0.0, 1.0 };
        CIE_TEST_REQUIRE_NOTHROW( PrimitiveType(base,lengths) );
        primitive = PrimitiveType( base, lengths );
        CIE_TEST_CHECK( primitive.isDegenerate() );
    }

    // Invalid boxes
    {
        #ifdef CIE_ENABLE_RUNTIME_GEOMETRY_CHECKS
        lengths = { -1.0, 1.0 };
        CIE_TEST_CHECK_THROWS( PrimitiveType(base, lengths) );

        lengths = { 1.0, -1.0 };
        CIE_TEST_CHECK_THROWS( PrimitiveType(base, lengths) );
        #endif
    }
}



CIE_TEST_CASE( "boolean::Cube", "[primitives]" )
{
    CIE_TEST_CASE_INIT( "boolean::Cube" )

    const Size dimension = 2;

    CIE_TEST_REQUIRE_NOTHROW( Cube<dimension,Double>(DoubleArray<dimension>({10.0,20.0}),2.0) );
    Cube<dimension,Double> cube( DoubleArray<dimension>({10.0,20.0}),
                                    2.0 );
    CIE_TEST_CHECK( Cube<dimension,Double>::Dimension == dimension );

    DoubleArray<dimension> point = {0.0, 0.0};

    CIE_TEST_REQUIRE_NOTHROW( cube.evaluate(point) );
    CIE_TEST_CHECK( cube.evaluate(point) == false );

    CIE_TEST_REQUIRE_NOTHROW( cube.evaluate(cube.base()) );
    CIE_TEST_CHECK( cube.evaluate( cube.base() ) == true );

    CIE_TEST_REQUIRE_NOTHROW( cube.evaluate( DoubleVector({11.999,21.999}) ) );
    CIE_TEST_CHECK( cube.evaluate( DoubleVector({11.999,21.999}) ) == true );
}


CIE_TEST_CASE( "boolean::Box", "[primitives]" )
{
    CIE_TEST_CASE_INIT( "boolean::Box" )

    const Size dimension = 2;
    Box<dimension,Double> box( DoubleArray<dimension>({10.0,20.0}),
                               DoubleArray<dimension>({1.0,2.0}) );
    DoubleArray<dimension> point = {0.0, 0.0};

    CIE_TEST_REQUIRE_NOTHROW( box.evaluate(point) );
    CIE_TEST_CHECK( box.evaluate(point) == false );

    CIE_TEST_REQUIRE_NOTHROW( box.evaluate(box.base()) );
    CIE_TEST_CHECK( box.evaluate( box.base() ) == true );

    CIE_TEST_REQUIRE_NOTHROW( box.evaluate( DoubleVector({10.999999,21.99999}) ) );
    CIE_TEST_CHECK( box.evaluate( DoubleVector({10.999999,21.99999}) ) == true );
}


} // namespace boolean


} // namespace cie::geo