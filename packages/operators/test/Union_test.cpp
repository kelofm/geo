// --- Utility Includes ---
#include "packages/testing/inc/essentials.hpp"

// --- Internal Includes ---
#include "packages/operators/inc/Union.hpp"
#include "packages/primitives/inc/Cube.hpp"


namespace cie::geo {


CIE_TEST_CASE( "Union", "[operators]" )
{
    CIE_TEST_CASE_INIT( "Union" )

    const Size Dimension = 2;

    using CoordinateType = Double;
    using Operator       = Union<Dimension,CoordinateType>;
    using Primitive      = boolean::Cube<Dimension,CoordinateType>;
    using Point          = Primitive::Point;

    Point base0 { 0.0, 0.0 };
    Point base1 { 0.25, 0.25 };
    CoordinateType length0 = 1.0;
    CoordinateType length1 = 0.75;

    Point truePoint( {0.1, 0.1} );
    Point falsePoint { -0.5, -0.5 };

   Object<Dimension,Bool,CoordinateType>::SharedPointer p_lhs(
        new Primitive( base0, length0 )
    );
   Object<Dimension,Bool,CoordinateType>::SharedPointer p_rhs(
        new Primitive( base1, length1 )
    );

    // Default constructor
    CIE_TEST_REQUIRE_NOTHROW( Operator() );
    Operator operator0;

    CIE_TEST_REQUIRE_NOTHROW( operator0.bindLhs(p_lhs) );
    CIE_TEST_REQUIRE_NOTHROW( operator0.bindRhs(p_rhs) );

    CIE_TEST_CHECK_NOTHROW( operator0.at( truePoint ) );
    CIE_TEST_CHECK( operator0.at( truePoint ) );
    CIE_TEST_CHECK( !operator0.at( falsePoint ) );

    CIE_TEST_REQUIRE_NOTHROW( operator0.emplaceLhs<Primitive>( base0, length0 ) );
    CIE_TEST_REQUIRE_NOTHROW( operator0.emplaceRhs<Primitive>( base1, length1 ) );
    CIE_TEST_CHECK_NOTHROW( operator0.at( truePoint ) );
    CIE_TEST_CHECK( operator0.at( truePoint ) );
    CIE_TEST_CHECK( !operator0.at( falsePoint ) );

    // Constructor with operand
    CIE_TEST_REQUIRE_NOTHROW( Operator(p_lhs,p_rhs) );
    Operator operator1( p_lhs, p_rhs );

    CIE_TEST_CHECK_NOTHROW( operator1.at( truePoint ) );
    CIE_TEST_CHECK( operator1.at( truePoint ) );
    CIE_TEST_CHECK( !operator1.at( falsePoint ) );
}


} // namespace cie::geo