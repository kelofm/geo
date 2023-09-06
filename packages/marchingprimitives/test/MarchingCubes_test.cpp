// --- Utility Includes ---
#include "packages/concurrency/inc/ThreadPoolSingleton.hpp"
#include "packages/testing/inc/essentials.hpp"

// --- GEO Includes ---
#include "packages/primitives/inc/Cube.hpp"
#include "packages/primitives/inc/Sphere.hpp"

// --- Internal Includes ---
#include "packages/marchingprimitives/inc/MarchingCubes.hpp"
#include "packages/structured/inc/cartesianmesh.hpp"

// --- STL Includes ---
#include <vector>


namespace cie::geo {


CIE_TEST_CASE( "MarchingSquares", "[marchingprimitives]" )
{
    CIE_TEST_CASE_INIT( "MarchingSquares" )

    const Size Dimension = 2;
    using CoordinateType = Double;
    using PointType      = MeshTraits<Dimension,CoordinateType>::Point;
    using TargetType     = geo::boolean::Sphere<Dimension,CoordinateType>;

    {
        CIE_TEST_CASE_INIT( "UnstructuredMarchingSquares" )
        using MarchingSquares = UnstructuredMarchingCubes<TargetType>;

        PointType meshOrigin { -2.0, -2.0 };
        MarchingSquares::Resolution numberOfSquares { 2, 2 };
        CoordinateType edgeLength = 2.0;

        auto p_unitCircle = MarchingSquares::target_ptr(
            new TargetType( {0.0,0.0}, 1.0 )
        );

        auto outputFunctor = [](Size, const MarchingSquares::output_arguments&) -> void
        {};

        auto p_primitives = MarchingSquares::primitive_container_ptr(
            new MarchingSquares::primitive_container
        );

        makeCartesianMesh<MarchingSquares::primitive_type>(
            numberOfSquares,
            edgeLength,
            meshOrigin,
            *p_primitives
        );

        auto pool = mp::ThreadPoolSingleton::get();
        MarchingSquares marchingSquares( p_unitCircle,
                                         p_primitives,
                                         outputFunctor );
        marchingSquares.execute(pool);
    }
}


CIE_TEST_CASE( "MarchingCubes", "[marchingprimitives]" )
{
    CIE_TEST_CASE_INIT( "MarchingCubes" )

    const Size Dimension = 3;
    using CoordinateType = double;
    using PointType      = MeshTraits<Dimension,CoordinateType>::Point;
    using TargetType     = geo::boolean::Sphere<Dimension,CoordinateType>;

    {
        CIE_TEST_CASE_INIT( "UnstructuredMarchingCubes" )
        using TestType = UnstructuredMarchingCubes<TargetType>;

        PointType meshOrigin { -2.0, -2.0, -2.0 };
        TestType::Resolution numberOfCubes { 2, 2, 2 };
        CoordinateType edgeLength = 2.0;

        PointType origin     = geo::detail::makeOrigin<Dimension,CoordinateType>();
        auto p_unitSphere = TestType::target_ptr(
            new TargetType( origin, 1.0 )
        );

        std::vector<TestType::output_arguments> outputs;
        auto outputFunctor = [](Size, const TestType::output_arguments&) -> void
        {};

        auto p_primitives = TestType::primitive_container_ptr(
            new TestType::primitive_container
        );

        makeCartesianMesh<TestType::primitive_type>( numberOfCubes,
                                                    edgeLength,
                                                    meshOrigin,
                                                    *p_primitives );

        TestType marchingCubes( p_unitSphere,
                                p_primitives,
                                outputFunctor );

        auto pool = mp::ThreadPoolSingleton::get();
        marchingCubes.execute(pool);

//      for ( const auto& r_triangle : outputs )
//      {
//          for ( const auto& r_edge : r_triangle )
//              std::cout << r_edge.first << " " << r_edge.second << std::endl;
//
//          std::cout << std::endl;
//      }
    }

    {
        CIE_TEST_CASE_INIT( "StructuredMarchingCubes" )

        using TestType = StructuredMarchingCubes<TargetType>;

        TestType::Domain domain {{ {-2.0,2.0}, {-2.0,2.0}, {-2.0,2.0} }};
        TestType::Resolution numberOfPoints { 3, 3, 3 };

        PointType origin     = geo::detail::makeOrigin<Dimension,CoordinateType>();
        auto p_unitSphere = TestType::target_ptr(
            new TargetType( origin, 1.0 )
        );

        std::vector<TestType::output_arguments> outputs;
        auto outputFunctor = [](Size, const TestType::output_arguments&) -> void
        {};

        auto pool = mp::ThreadPoolSingleton::get();
        TestType(p_unitSphere,
                 domain,
                 numberOfPoints,
                 outputFunctor).execute(pool);
    }
}


CIE_TEST_CASE( "MarchingBoxes", "[marchingprimitives]" )
{
    CIE_TEST_CASE_INIT( "MarchingBoxes" )

    const Size Dimension = 3;
    using CoordinateType = double;
    using PointType      = MeshTraits<Dimension,CoordinateType>::Point;
    using PrimitiveType  = geo::Box<Dimension,CoordinateType>;
    using TargetType     = geo::boolean::Sphere<Dimension,CoordinateType>;

    {
        CIE_TEST_CASE_INIT( "UnstructuredMarchingBoxes" )
        using TestType = UnstructuredMarchingCubes<TargetType,PrimitiveType>;

        TestType::Domain domain {{ {-1.0, 1.0}, {-1.1, 1.1}, {-1.2, 1.2} }};
        TestType::Resolution resolution { 3, 4, 5 };

        PointType origin     = geo::detail::makeOrigin<Dimension,CoordinateType>();
        auto p_unitSphere = TestType::target_ptr(
            new TargetType( origin, 1.0 )
        );

        std::vector<TestType::output_arguments> outputs;
        auto outputFunctor = [](Size, const TestType::output_arguments&) -> void
        {};

        auto p_primitives = TestType::primitive_container_ptr(
            new TestType::primitive_container
        );

        makeCartesianMesh<TestType::primitive_type>( domain,
                                                     resolution,
                                                     *p_primitives );

        TestType marchingBoxes( p_unitSphere,
                                p_primitives,
                                outputFunctor );

        auto pool = mp::ThreadPoolSingleton::get();
        marchingBoxes.execute(pool);

//      for ( const auto& r_triangle : outputs )
//      {
//          for ( const auto& r_edge : r_triangle )
//              std::cout << r_edge.first << " " << r_edge.second << std::endl;
//
//          std::cout << std::endl;
//      }
    }

    {
        CIE_TEST_CASE_INIT( "StructuredMarchingBoxes" )

        using TestType = StructuredMarchingCubes<TargetType,PrimitiveType>;

        TestType::Domain domain {{ {-2.0,2.0}, {-2.0,2.0}, {-2.0,2.0} }};
        TestType::Resolution numberOfPoints { 3, 4, 5 };

        PointType origin     = geo::detail::makeOrigin<Dimension,CoordinateType>();
        auto p_unitSphere = TestType::target_ptr(
            new TargetType( origin, 1.0 )
        );

        std::vector<TestType::output_arguments> outputs;
        auto outputFunctor = [](Size, const TestType::output_arguments&) -> void
        {};

        auto pool = mp::ThreadPoolSingleton::get();
        TestType( p_unitSphere,
                  domain,
                  numberOfPoints,
                  outputFunctor ).execute(pool);
    }
}


} // namespace cie::geo