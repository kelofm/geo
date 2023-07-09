#define _USE_MATH_DEFINES

// --- Utility Includes ---
#include "packages/testing/inc/essentials.hpp"

// --- Internal Includes ---
#include "packages/partitioning/inc/AABBoxNode.hpp"

// --- STL Inlcudes ---
#include <deque>
#include <memory>
#include <iostream>
#include <math.h>


namespace cie::geo {


class TestAABBoxNodeObjectType final :
    public AbsBoundableObject<2,Double>,
    public AABBox<2,Double>
{
public:
    TestAABBoxNodeObjectType( const typename TestAABBoxNodeObjectType::Point& r_base,
                              const typename TestAABBoxNodeObjectType::Point& r_lengths ) :
        AbsBoundableObject<2,Double>(),
        AABBox<2,Double>( r_base, r_lengths ) {}
private:
    void computeBoundingBox_impl( typename TestAABBoxNodeObjectType::bounding_box& r_box ) override
    { r_box = *this; }
};


CIE_TEST_CASE( "AABBoxNode", "[partitioning]" )
{
    CIE_TEST_CASE_INIT( "AABBoxNode" )

    const Size Dimension = TestAABBoxNodeObjectType::Dimension;
    using CoordinateType = TestAABBoxNodeObjectType::Coordinate;

    using PointType = typename Traits<Dimension,CoordinateType>::Point;

    {
        CIE_TEST_CASE_INIT( "box objects" )

        using Object    = TestAABBoxNodeObjectType;
        using ObjectPtr = std::shared_ptr<Object>;
        using Node      = AABBoxNode<Object>;
        using NodePtr   = std::shared_ptr<Node>;

        // Generate objects
        std::deque<ObjectPtr> objects;

        Size numberOfCellsPerDimension = 5;

        for ( Size i=0; i<numberOfCellsPerDimension; ++i )
            for ( Size j=0; j<numberOfCellsPerDimension; ++j )
            {
                auto p_object = ObjectPtr( new Object(
                    { Double(i)/numberOfCellsPerDimension, Double(j)/numberOfCellsPerDimension },
                    { 1.0 / numberOfCellsPerDimension, 1.0 / numberOfCellsPerDimension }
                ) );
                objects.push_back( p_object );
            }

        // Generate root node
        Double delta = 0.0;
        auto p_root = NodePtr(
            new Node( { -delta, -delta },
                    { 1.0+2*delta, 1.0+2*delta },
                    typename Node::self_ptr() )
        );

        for ( auto& rp_object : objects )
            CIE_TEST_CHECK_NOTHROW( p_root->addObject(rp_object) );

        CIE_TEST_CHECK( p_root->containedObjects().size() == numberOfCellsPerDimension*numberOfCellsPerDimension );

        // Partition
        const Size maxObjects = 3;
        const Size maxLevel   = 5;
        bool partitionSuccess = false;

        CIE_TEST_CHECK_NOTHROW( partitionSuccess = p_root->partition( maxObjects, maxLevel ) );
        CIE_TEST_CHECK( partitionSuccess );

        CIE_TEST_CHECK_NOTHROW( p_root->shrink() );

        // Check number of objects and maximum levels
        {
            auto nodeVisitFunction = [maxObjects,maxLevel]( Node* p_node ) -> bool
            {
                CIE_TEST_CHECK( p_node->level() <= maxLevel );

                if ( p_node->isLeaf() )
                    CIE_TEST_CHECK( p_node->containedObjects().size() <= maxObjects );

                return true;
            };

            CIE_TEST_CHECK_NOTHROW( p_root->visit(nodeVisitFunction) );
        }
    }


    {
        CIE_TEST_CASE_INIT( "point objects" )

        using Object    = PointType;
        using ObjectPtr = std::shared_ptr<Object>;
        using Node      = AABBoxNode<Object>;
        using NodePtr   = std::shared_ptr<Node>;

        // Create points on a circle
        const Size numberOfObjects = 100;
        const double dt = 2.0 * M_PI / (numberOfObjects-1);

        std::deque<ObjectPtr> objects;
        for ( double t=0.0; t<2.0*M_PI; t+=dt )
            objects.emplace_back( new Object{
                0.5 * std::cos(t) + 0.5,
                0.5 * std::sin(t) + 0.5 + 1e-10
            } );

        // Construct root and add points to it
        CIE_TEST_REQUIRE_NOTHROW( NodePtr( new Node(
            { 0.0, 0.0 },
            { 1.0, 1.0 },
            Node::self_ptr()
        ) ) );
        auto p_root = NodePtr( new Node(
            { 0.0, 0.0 },
            { 1.0, 1.0 },
            Node::self_ptr()
        ) );

        for ( auto& rp_object : objects )
            CIE_TEST_CHECK_NOTHROW( p_root->addObject( rp_object ) );

        CIE_TEST_CHECK( p_root->containedObjects().size() == numberOfObjects );
        CIE_TEST_CHECK( p_root->intersectedObjects().size() == 0 );

        // Check partitioning
        const Size maxObjects = 5;
        const Size maxLevel   = 3;
        bool partitionSuccess = false;

        CIE_TEST_CHECK_NOTHROW( partitionSuccess = p_root->partition(maxObjects,maxLevel) );
        CIE_TEST_CHECK( partitionSuccess );

        // Check number of objects and maximum level
        Size objectCounter = 0;

        auto nodeVisitFunctor = [&objectCounter,maxObjects,maxLevel]( Node* p_node ) -> bool
        {
            CIE_TEST_CHECK( p_node->intersectedObjects().size() == 0 );
            CIE_TEST_CHECK( p_node->level() <= maxLevel );

            if ( p_node->isLeaf() )
            {
                Size numberOfContainedObjects = p_node->containedObjects().size();
                CIE_TEST_CHECK( numberOfContainedObjects <= maxObjects );
                objectCounter += numberOfContainedObjects;
            }

            return true;
        };

        CIE_TEST_CHECK_NOTHROW( p_root->visit(nodeVisitFunctor) );
        CIE_TEST_CHECK( objectCounter == numberOfObjects );
    }
}


} // namespace cie::geo