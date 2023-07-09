// --- Utility Includes ---
#include "packages/testing/inc/essentials.hpp"

// --- Internal Includes ---
#include "packages/operators/inc/Complement.hpp"
#include "packages/primitives/inc/Cube.hpp"


namespace cie::geo {


CIE_TEST_CASE( "Complement", "[operators]" )
{
    CIE_TEST_CASE_INIT( "Complement" )

    const Size Dimension = 2;

    using CoordinateType = Double;
    using Operator       = Complement<Dimension,CoordinateType>;
    using Primitive      = boolean::Cube<Dimension,CoordinateType>;
    using Point          = Primitive::Point;

    Point base { 0.0, 0.0 };
    CoordinateType length = 1.0;

    Point truePoint( {-0.5, -0.5} );
    Point falsePoint { 0.5, 0.5 };

    Object<Dimension,Bool,CoordinateType>::SharedPointer p_primitive(
        new Primitive( base, length )
    );

    // Default constructor
    CIE_TEST_REQUIRE_NOTHROW( Operator() );
    Operator operator0;

    CIE_TEST_REQUIRE_NOTHROW( operator0.bindRhs(p_primitive) );

    CIE_TEST_CHECK_NOTHROW( operator0.at( truePoint ) );
    CIE_TEST_CHECK( operator0.at( truePoint ) );
    CIE_TEST_CHECK( !operator0.at( falsePoint ) );

    CIE_TEST_REQUIRE_NOTHROW( operator0.emplaceRhs<Primitive>( base, length ) );
    CIE_TEST_CHECK_NOTHROW( operator0.at( truePoint ) );
    CIE_TEST_CHECK( operator0.at( truePoint ) );
    CIE_TEST_CHECK( !operator0.at( falsePoint ) );

    // Constructor with operand
    CIE_TEST_REQUIRE_NOTHROW( Operator(p_primitive) );
    Operator operator1( p_primitive );

    CIE_TEST_CHECK_NOTHROW( operator1.at( truePoint ) );
    CIE_TEST_CHECK( operator1.at( truePoint ) );
    CIE_TEST_CHECK( !operator1.at( falsePoint ) );
}


} // namespace cie::geo