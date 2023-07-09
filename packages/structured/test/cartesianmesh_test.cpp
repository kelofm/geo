// --- Utility Includes ---
#include "packages/testing/inc/essentials.hpp"

// --- Internal Includes ---
#include "packages/structured/inc/cartesianmesh.hpp"


namespace cie::geo {


CIE_TEST_CASE( "makeCartesianMesh", "[structured]" )
{
    CIE_TEST_CASE_INIT( "makeCartesianMesh" )

    const Size Dimension = 2;
    using CoordinateType = Double;
    using TestTraits     = MeshTraits<Dimension,CoordinateType>;

    {
        CIE_TEST_CASE_INIT( "Point" )

        using PointType = TestTraits::Point;

        TestTraits::Domain domain {{ {-1.0,1.0}, {-2.0, 2.0} }};
        TestTraits::Resolution resolution { 2, 3 };

        CIE_TEST_REQUIRE_NOTHROW( makeCartesianMesh<PointType>( domain, resolution ) );
        auto mesh = makeCartesianMesh<PointType>( domain, resolution );

        CIE_TEST_REQUIRE( mesh.size() == resolution[0] * resolution[1] );

        auto* p_point = &mesh[0];
        CIE_TEST_CHECK( p_point->at(0) == Approx(-1.0) );
        CIE_TEST_CHECK( p_point->at(1) == Approx(-2.0) );

        p_point = &mesh[1];
        CIE_TEST_CHECK( p_point->at(0) == Approx(1.0) );
        CIE_TEST_CHECK( p_point->at(1) == Approx(-2.0) );

        p_point = &mesh[2];
        CIE_TEST_CHECK( p_point->at(0) == Approx(-1.0) );
        CIE_TEST_CHECK( p_point->at(1) == Approx(0.0) );

        p_point = &mesh[3];
        CIE_TEST_CHECK( p_point->at(0) == Approx(1.0) );
        CIE_TEST_CHECK( p_point->at(1) == Approx(0.0) );

        p_point = &mesh[4];
        CIE_TEST_CHECK( p_point->at(0) == Approx(-1.0) );
        CIE_TEST_CHECK( p_point->at(1) == Approx(2.0) );

        p_point = &mesh[5];
        CIE_TEST_CHECK( p_point->at(0) == Approx(1.0) );
        CIE_TEST_CHECK( p_point->at(1) == Approx(2.0) );
    }

    {
        CIE_TEST_CASE_INIT( "Cube" )

        using PrimitiveType = geo::Cube<Dimension,CoordinateType>;

        TestTraits::Resolution numberOfPrimitives { 2, 4 };
        CoordinateType edgeLength = 1.0;
        TestTraits::Point origin { 0.0, 0.0 };

        CIE_TEST_REQUIRE_NOTHROW(
            makeCartesianMesh<PrimitiveType>( numberOfPrimitives,
                                              edgeLength,
                                              origin )
        );

        auto mesh = makeCartesianMesh<PrimitiveType>( numberOfPrimitives,
                                                      edgeLength,
                                                      origin );

        CIE_TEST_REQUIRE( mesh.size() == numberOfPrimitives[0] * numberOfPrimitives[1] );

        auto p_primitive = mesh[0];
        CIE_TEST_CHECK( p_primitive->base()[0] == Approx(0.0) );
        CIE_TEST_CHECK( p_primitive->base()[1] == Approx(0.0) );
        CIE_TEST_CHECK( p_primitive->length() == Approx(edgeLength) );

        p_primitive = mesh[1];
        CIE_TEST_CHECK( p_primitive->base()[0] == Approx(1.0) );
        CIE_TEST_CHECK( p_primitive->base()[1] == Approx(0.0) );
        CIE_TEST_CHECK( p_primitive->length() == Approx(edgeLength) );

        p_primitive = mesh[2];
        CIE_TEST_CHECK( p_primitive->base()[0] == Approx(0.0) );
        CIE_TEST_CHECK( p_primitive->base()[1] == Approx(1.0) );
        CIE_TEST_CHECK( p_primitive->length() == Approx(edgeLength) );

        p_primitive = mesh[3];
        CIE_TEST_CHECK( p_primitive->base()[0] == Approx(1.0) );
        CIE_TEST_CHECK( p_primitive->base()[1] == Approx(1.0) );
        CIE_TEST_CHECK( p_primitive->length() == Approx(edgeLength) );

        p_primitive = mesh[4];
        CIE_TEST_CHECK( p_primitive->base()[0] == Approx(0.0) );
        CIE_TEST_CHECK( p_primitive->base()[1] == Approx(2.0) );
        CIE_TEST_CHECK( p_primitive->length() == Approx(edgeLength) );

        p_primitive = mesh[5];
        CIE_TEST_CHECK( p_primitive->base()[0] == Approx(1.0) );
        CIE_TEST_CHECK( p_primitive->base()[1] == Approx(2.0) );
        CIE_TEST_CHECK( p_primitive->length() == Approx(edgeLength) );

        p_primitive = mesh[6];
        CIE_TEST_CHECK( p_primitive->base()[0] == Approx(0.0) );
        CIE_TEST_CHECK( p_primitive->base()[1] == Approx(3.0) );
        CIE_TEST_CHECK( p_primitive->length() == Approx(edgeLength) );

        p_primitive = mesh[7];
        CIE_TEST_CHECK( p_primitive->base()[0] == Approx(1.0) );
        CIE_TEST_CHECK( p_primitive->base()[1] == Approx(3.0) );
        CIE_TEST_CHECK( p_primitive->length() == Approx(edgeLength) );
    }

    {
        CIE_TEST_CASE_INIT( "Box" )

        using PrimitiveType = geo::Box<Dimension,CoordinateType>;

        TestTraits::Domain domain {{ {0.0,2.0}, {0.0,4.0} }};
        TestTraits::Resolution resolution { 2, 4 };

        CIE_TEST_REQUIRE_NOTHROW(
            makeCartesianMesh<PrimitiveType>( domain, resolution )
        );

        auto mesh = makeCartesianMesh<PrimitiveType>( domain, resolution );

        CIE_TEST_REQUIRE( mesh.size() == resolution[0] * resolution[1] );

        auto p_primitive = mesh[0];
        CIE_TEST_CHECK( p_primitive->base()[0] == Approx(0.0) );
        CIE_TEST_CHECK( p_primitive->base()[1] == Approx(0.0) );
        CIE_TEST_CHECK( p_primitive->lengths()[0] == Approx(1.0) );
        CIE_TEST_CHECK( p_primitive->lengths()[1] == Approx(1.0) );

        p_primitive = mesh[1];
        CIE_TEST_CHECK( p_primitive->base()[0] == Approx(1.0) );
        CIE_TEST_CHECK( p_primitive->base()[1] == Approx(0.0) );
        CIE_TEST_CHECK( p_primitive->lengths()[0] == Approx(1.0) );
        CIE_TEST_CHECK( p_primitive->lengths()[1] == Approx(1.0) );

        p_primitive = mesh[2];
        CIE_TEST_CHECK( p_primitive->base()[0] == Approx(0.0) );
        CIE_TEST_CHECK( p_primitive->base()[1] == Approx(1.0) );
        CIE_TEST_CHECK( p_primitive->lengths()[0] == Approx(1.0) );
        CIE_TEST_CHECK( p_primitive->lengths()[1] == Approx(1.0) );

        p_primitive = mesh[3];
        CIE_TEST_CHECK( p_primitive->base()[0] == Approx(1.0) );
        CIE_TEST_CHECK( p_primitive->base()[1] == Approx(1.0) );
        CIE_TEST_CHECK( p_primitive->lengths()[0] == Approx(1.0) );
        CIE_TEST_CHECK( p_primitive->lengths()[1] == Approx(1.0) );

        p_primitive = mesh[4];
        CIE_TEST_CHECK( p_primitive->base()[0] == Approx(0.0) );
        CIE_TEST_CHECK( p_primitive->base()[1] == Approx(2.0) );
        CIE_TEST_CHECK( p_primitive->lengths()[0] == Approx(1.0) );
        CIE_TEST_CHECK( p_primitive->lengths()[1] == Approx(1.0) );

        p_primitive = mesh[5];
        CIE_TEST_CHECK( p_primitive->base()[0] == Approx(1.0) );
        CIE_TEST_CHECK( p_primitive->base()[1] == Approx(2.0) );
        CIE_TEST_CHECK( p_primitive->lengths()[0] == Approx(1.0) );
        CIE_TEST_CHECK( p_primitive->lengths()[1] == Approx(1.0) );

        p_primitive = mesh[6];
        CIE_TEST_CHECK( p_primitive->base()[0] == Approx(0.0) );
        CIE_TEST_CHECK( p_primitive->base()[1] == Approx(3.0) );
        CIE_TEST_CHECK( p_primitive->lengths()[0] == Approx(1.0) );
        CIE_TEST_CHECK( p_primitive->lengths()[1] == Approx(1.0) );

        p_primitive = mesh[7];
        CIE_TEST_CHECK( p_primitive->base()[0] == Approx(1.0) );
        CIE_TEST_CHECK( p_primitive->base()[1] == Approx(3.0) );
        CIE_TEST_CHECK( p_primitive->lengths()[0] == Approx(1.0) );
        CIE_TEST_CHECK( p_primitive->lengths()[1] == Approx(1.0) );
    }
}


} // namespace cie::geo