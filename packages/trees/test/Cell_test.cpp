// --- Utility Includes ---
#include "packages/testing/inc/essentials.hpp"

// --- Internal Includes ---
#include "packages/trees/inc/Cell.hpp"
#include "packages/primitives/inc/Cube.hpp"
#include "packages/primitives/inc/Box.hpp"
#include "cmake_variables.hpp"


namespace cie::geo {


CIE_TEST_CASE( "Cell", "[trees]" )
{
    CIE_TEST_CASE_INIT( "Cell" )
    /**
     *    y
     *    ^
     *    |
     *
     *    4 -----------
     *      |    |    |
     *      |    |    |
     *    3 |----|----|
     *      |    |    |
     *      |    |    |
     *    2 -----------
     *      2    3    4  ->x
    */

    {
        CIE_TEST_CASE_INIT( "box Cell" )
        using PrimitiveType = Cube<2,Double>;
        using TestCell      = Cell<PrimitiveType>;

        CIE_TEST_REQUIRE_NOTHROW( TestCell( TestCell::Point({1.0,1.0}), 2.0) );
        TestCell cell( TestCell::Point({2.0,2.0}), 2.0 );

        CIE_TEST_REQUIRE_NOTHROW( cell.split() );
        auto constructorArgumentContainer = cell.split();
        CIE_TEST_REQUIRE( constructorArgumentContainer.size() == 4 );

        CIE_TEST_CHECK( std::abs( std::get<0>(constructorArgumentContainer.at(0))[0] - 2.0 ) < 1e-16 ); // <-- base.x
        CIE_TEST_CHECK( std::abs( std::get<0>(constructorArgumentContainer.at(0))[1] - 2.0 ) < 1e-16 ); // <-- base.y
        CIE_TEST_CHECK( std::abs( std::get<1>(constructorArgumentContainer.at(0)) - 1.0    ) < 1e-16 ); // <-- length

        CIE_TEST_CHECK( std::abs( std::get<0>(constructorArgumentContainer.at(1))[0] - 3.0 ) < 1e-16 ); // <-- base.x
        CIE_TEST_CHECK( std::abs( std::get<0>(constructorArgumentContainer.at(1))[1] - 2.0 ) < 1e-16 ); // <-- base.y
        CIE_TEST_CHECK( std::abs( std::get<1>(constructorArgumentContainer.at(1)) - 1.0    ) < 1e-16 ); // <-- length

        CIE_TEST_CHECK( std::abs( std::get<0>(constructorArgumentContainer.at(2))[0] - 2.0 ) < 1e-16 ); // <-- base.x
        CIE_TEST_CHECK( std::abs( std::get<0>(constructorArgumentContainer.at(2))[1] - 3.0 ) < 1e-16 ); // <-- base.y
        CIE_TEST_CHECK( std::abs( std::get<1>(constructorArgumentContainer.at(2)) - 1.0    ) < 1e-16 ); // <-- length

        CIE_TEST_CHECK( std::abs( std::get<0>(constructorArgumentContainer.at(3))[0] - 3.0 ) < 1e-16 ); // <-- base.x
        CIE_TEST_CHECK( std::abs( std::get<0>(constructorArgumentContainer.at(3))[1] - 3.0 ) < 1e-16 ); // <-- base.y
        CIE_TEST_CHECK( std::abs( std::get<1>(constructorArgumentContainer.at(3)) - 1.0    ) < 1e-16 ); // <-- length

        for (const auto& constructorArguments : constructorArgumentContainer)
            CIE_TEST_CHECK_NOTHROW( std::make_from_tuple<TestCell>(constructorArguments) );
    }
}




CIE_TEST_CASE( "boolean::Cell", "[trees]" )
{
    CIE_TEST_CASE_INIT( "boolean::Cell" )
    /**
     *    y
     *    ^
     *    |
     *
     *    3 ---------------------
     *      |    |              |
     *      |    |              |
     *    2 |----|--------------|
     *      |    |              |
     *      |    |              |
     *    1 ---------------------
     *      0    1    2    3    4  ->x
    */

    using PrimitiveType = Box<2,Double>;
    using TestCell      = Cell<PrimitiveType>;

    CIE_TEST_REQUIRE_NOTHROW( TestCell( TestCell::Point({1.0,1.0}), TestCell::Point({0.0,1.0}) ) );
    TestCell cell( TestCell::Point({0.0,1.0}), TestCell::Point({4.0,2.0}) );

    CIE_TEST_REQUIRE_NOTHROW( cell.split(TestCell::Point({1.0,2.0})) );
    auto constructorArgumentContainer = cell.split( TestCell::Point({1.0,2.0}) );
    CIE_TEST_REQUIRE( constructorArgumentContainer.size() == 4 );

    CIE_TEST_CHECK( std::abs( std::get<0>(constructorArgumentContainer.at(0))[0] - 0.0) < 1e-16 ); // <-- base.x
    CIE_TEST_CHECK( std::abs( std::get<0>(constructorArgumentContainer.at(0))[1] - 1.0) < 1e-16 ); // <-- base.y
    CIE_TEST_CHECK( std::abs( std::get<1>(constructorArgumentContainer.at(0))[0] - 1.0) < 1e-16 ); // <-- length.x
    CIE_TEST_CHECK( std::abs( std::get<1>(constructorArgumentContainer.at(0))[1] - 1.0) < 1e-16 ); // <-- length.y

    CIE_TEST_CHECK( std::abs( std::get<0>(constructorArgumentContainer.at(1))[0] - 1.0) < 1e-16 ); // <-- base.x
    CIE_TEST_CHECK( std::abs( std::get<0>(constructorArgumentContainer.at(1))[1] - 1.0) < 1e-16 ); // <-- base.y
    CIE_TEST_CHECK( std::abs( std::get<1>(constructorArgumentContainer.at(1))[0] - 3.0) < 1e-16 ); // <-- length.x
    CIE_TEST_CHECK( std::abs( std::get<1>(constructorArgumentContainer.at(1))[1] - 1.0) < 1e-16 ); // <-- length.y

    CIE_TEST_CHECK( std::abs( std::get<0>(constructorArgumentContainer.at(2))[0] - 0.0) < 1e-16 ); // <-- base.x
    CIE_TEST_CHECK( std::abs( std::get<0>(constructorArgumentContainer.at(2))[1] - 2.0) < 1e-16 ); // <-- base.y
    CIE_TEST_CHECK( std::abs( std::get<1>(constructorArgumentContainer.at(2))[0] - 1.0) < 1e-16 ); // <-- length.x
    CIE_TEST_CHECK( std::abs( std::get<1>(constructorArgumentContainer.at(2))[1] - 1.0) < 1e-16 ); // <-- length.y

    CIE_TEST_CHECK( std::abs( std::get<0>(constructorArgumentContainer.at(3))[0] - 1.0) < 1e-16 ); // <-- base.x
    CIE_TEST_CHECK( std::abs( std::get<0>(constructorArgumentContainer.at(3))[1] - 2.0) < 1e-16 ); // <-- base.y
    CIE_TEST_CHECK( std::abs( std::get<1>(constructorArgumentContainer.at(3))[0] - 3.0) < 1e-16 ); // <-- length.x
    CIE_TEST_CHECK( std::abs( std::get<1>(constructorArgumentContainer.at(3))[1] - 1.0) < 1e-16 ); // <-- length.y

    for (const auto& constructorArguments : constructorArgumentContainer)
        CIE_TEST_CHECK_NOTHROW( std::make_from_tuple<TestCell>(constructorArguments) );
}


} // namespace cie::geo