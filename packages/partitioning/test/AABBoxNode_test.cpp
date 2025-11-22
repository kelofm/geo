#define _USE_MATH_DEFINES

// --- Utility Includes ---
#include "packages/testing/inc/essentials.hpp"

// --- Internal Includes ---
#include "packages/partitioning/inc/AABBoxNode.hpp"

// --- STL Inlcudes ---
#include <cmath>
#include <iostream>


namespace cie::geo {


class TestAABBoxNodeObjectType final
    : public BoxBoundable<2,Double>,
      public AABBox<2,Double>
{
public:
    TestAABBoxNodeObjectType(const typename TestAABBoxNodeObjectType::Point& rBase,
                             const typename TestAABBoxNodeObjectType::Point& rLengths)
        : BoxBoundable<2,Double>(),
          AABBox<2,Double>(rBase, rLengths) {}
private:
    void computeBoundingBoxImpl(typename TestAABBoxNodeObjectType::BoundingBox& rBox) noexcept override
    {rBox = *this;}
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
        using Node      = AABBoxNode<Object>;

        // Generate objects
        std::vector<Object> objects;

        constexpr Size numberOfCellsPerDimension = 5;

        for ( Size i=0; i<numberOfCellsPerDimension; ++i ) {
            for ( Size j=0; j<numberOfCellsPerDimension; ++j ) {
                if (i > j) {
                    objects.emplace_back(
                        Object::Point {double(i)/numberOfCellsPerDimension, double(j)/numberOfCellsPerDimension},
                        Object::Point {1.0 / numberOfCellsPerDimension, 1.0 / numberOfCellsPerDimension});
                } else {
                    objects.emplace_back(
                        Object::Point {std::numeric_limits<CoordinateType>::max(), std::numeric_limits<CoordinateType>::max()},
                        Object::Point {0.0, 0.0}
                    );
                }
            }
        }

        // Generate root node
        double delta = 0.0;
        Node root(Object::Point {-delta, -delta},
                  Object::Point {1.0+2*delta, 1.0+2*delta},
                  nullptr);

        for (auto& rObject : objects)
            CIE_TEST_CHECK_NOTHROW(root.insert(&rObject));

        CIE_TEST_CHECK(root.contained().size() == numberOfCellsPerDimension * (numberOfCellsPerDimension - 1) / 2);

        // Partition
        constexpr Size maxObjects = 3;
        constexpr Size maxLevel   = 5;
        bool partitionSuccess = false;

        CIE_TEST_CHECK_NOTHROW( partitionSuccess = root.partition( maxObjects, maxLevel ) );
        CIE_TEST_CHECK( partitionSuccess );

        CIE_TEST_CHECK_NOTHROW( root.shrink() );

        // Check number of objects and maximum levels
        {
            auto nodeVisitFunction = [maxLevel=maxLevel,maxObjects=maxObjects](Node* p_node) -> bool {
                CIE_TEST_CHECK(p_node->level() <= maxLevel);
                if (p_node->isLeaf())
                    CIE_TEST_CHECK( p_node->contained().size() <= maxObjects );
                return true;
            };
            CIE_TEST_CHECK_NOTHROW( root.visit(nodeVisitFunction) );
        }

        {
            CIE_TEST_CASE_INIT("flattened")
            const auto flatTree = FlatAABBoxTree<
                CoordinateType,
                Dimension,
                unsigned,
                std::allocator<std::byte>
            >::flatten(root,
                       [&objects] (Ref<const Object> rObject) -> unsigned {
                         return std::distance(static_cast<Ptr<const Object>>(objects.data()), &rObject);
                       },
                       std::allocator<std::byte>());

            for (unsigned i=0; i<numberOfCellsPerDimension; ++i) {
                for (unsigned j=0; j<numberOfCellsPerDimension; ++j) {
                    const Object::Point center {
                        (double(i) + 0.5) / numberOfCellsPerDimension,
                        (double(j) + 0.5) / numberOfCellsPerDimension
                    };

                    CIE_TEST_CHECK_NOTHROW(flatTree.find(
                        std::span<const CoordinateType,Dimension>(center.data(), Dimension),
                        std::span<const Object> {objects.data(), objects.size()}
                    ));

                    const auto pMaybeObject = flatTree.find(
                        std::span<const CoordinateType,Dimension>(center.data(), Dimension),
                        std::span<const Object> {objects.data(), objects.size()}
                    );

                    if (i > j) {
                        CIE_TEST_CHECK(pMaybeObject.has_value());
                        if (pMaybeObject.has_value()) {
                            const std::size_t iObject = pMaybeObject.value();
                            CIE_TEST_CHECK(iObject == i * numberOfCellsPerDimension + j);
                        }
                    } else {
                        CIE_TEST_CHECK(!pMaybeObject.has_value());
                    }
                }
            }
        }
    }


    {
        CIE_TEST_CASE_INIT( "point objects" )

        using Object    = PointType;
        using Node      = AABBoxNode<Object>;

        // Create points on a circle
        const Size numberOfObjects = 100;
        const double dt = 2.0 * M_PI / (numberOfObjects-1);

        std::vector<Object> objects;
        for ( double t=0.0; t<2.0*M_PI; t+=dt )
            objects.emplace_back(Object {0.5 * std::cos(t) + 0.5,
                                         0.5 * std::sin(t) + 0.5 + 1e-10});

        // Construct root and add points to it
        CIE_TEST_REQUIRE_NOTHROW(Node(
            Object {0.0, 0.0},
            Object {1.0, 1.0},
            nullptr
        ));
        Node root(
            Object {0.0, 0.0},
            Object {1.0, 1.0},
            nullptr
        );

        for ( auto& rObject : objects )
            CIE_TEST_CHECK_NOTHROW( root.insert( &rObject ) );

        CIE_TEST_CHECK( root.contained().size() == numberOfObjects );
        CIE_TEST_CHECK( root.intersected().size() == 0 );

        // Check partitioning
        constexpr Size maxObjects = 5;
        constexpr Size maxLevel   = 3;
        bool partitionSuccess = false;

        CIE_TEST_CHECK_NOTHROW( partitionSuccess = root.partition(maxObjects,maxLevel) );
        CIE_TEST_CHECK( partitionSuccess );

        // Check number of objects and maximum level
        Size objectCounter = 0;

        auto nodeVisitFunctor = [&objectCounter, maxLevel=maxLevel, maxObjects=maxObjects]( Node* p_node ) -> bool {
            CIE_TEST_CHECK( p_node->intersected().size() == 0 );
            CIE_TEST_CHECK( p_node->level() <= maxLevel );
            if (p_node->isLeaf()) {
                Size numberOfcontained = p_node->contained().size();
                CIE_TEST_CHECK( numberOfcontained <= maxObjects );
                objectCounter += numberOfcontained;
            }
            return true;
        };

        CIE_TEST_CHECK_NOTHROW( root.visit(nodeVisitFunctor) );
        CIE_TEST_CHECK( objectCounter == numberOfObjects );
    }
}


} // namespace cie::geo