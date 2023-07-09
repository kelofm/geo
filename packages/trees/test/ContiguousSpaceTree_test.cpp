// --- Utility Includes ---
#include "packages/testing/inc/essentials.hpp"

// --- Linalg Includes ---
#include "packages/types/inc/typeoperations.hpp"

// --- GEO Includes ---
#include "packages/trees/inc/ContiguousSpaceTree.hpp"

// --- STL Includes ---
#include <limits>


namespace cie::geo {


CIE_TEST_CASE("ContiguousSpaceTree", "[trees]")
{
    CIE_TEST_CASE_INIT("ContiguousSpaceTree")

    using Coordinate = Float;
    using Index = uint16_t;

    {
        CIE_TEST_CASE_INIT("Square")
        const Size Dimension = 2;
        using Geometry = Cube<2,Coordinate>;
        using Tree = ContiguousSpaceTree<Geometry,Index>;

        CIE_TEST_CHECK_NOTHROW(Tree(Tree::Point {-1, -1}, 2));
        Tree tree(Tree::Point {0, 0}, 1);

        const auto unitCircle = [&tree](Ref<const Tree::Node> r_node) {
            Tree::Point base;
            Tree::Coordinate edge;
            tree.getNodeGeometry(r_node, base.begin(), &edge);
            bool isInside = linalg::norm2(base) < 1;
            base[0] += edge;
            if (isInside != linalg::norm2(base) < 1) return true;
            base[1] += edge;
            if (isInside != linalg::norm2(base) < 1) return true;
            base[0] -= edge;
            if (isInside != linalg::norm2(base) < 1) return true;
            return false;
        };

        // Check fresh scan
        CIE_TEST_REQUIRE_NOTHROW(tree.scan(unitCircle, 3));
        CIE_TEST_CHECK(tree.size() == 45);

        // Check scan following a full tree
        const auto infiniteBoundary = [](Ref<const Tree::Node> r_node) {return true;};
        CIE_TEST_REQUIRE_NOTHROW(tree.scan(infiniteBoundary, 3));
        CIE_TEST_REQUIRE_NOTHROW(tree.scan(unitCircle, 3));
        CIE_TEST_CHECK(tree.size() == 45);
    }

    {
        CIE_TEST_CASE_INIT("Box")
        const Size Dimension = 2;
        using Geometry = Box<2,Coordinate>;
        using Tree = ContiguousSpaceTree<Geometry,Index>;

        CIE_TEST_CHECK_NOTHROW(Tree(Tree::Point {-1, -1},
                               Tree::Point{2, 2}));
        Tree tree(Tree::Point {-1.0f, 0.0f},
                  Tree::Point { 2.0f - std::numeric_limits<Coordinate>::epsilon(), 1.0f});

        const auto unitCircle = [&tree](Ref<const Tree::Node> r_node) {
            Tree::Point base;
            Tree::Point edges;
            tree.getNodeGeometry(r_node, base.begin(), edges.begin());
            bool isInside = linalg::norm2(base) < 1;
            base[0] += edges[0];
            if (isInside != linalg::norm2(base) < 1) return true;
            base[1] += edges[1];
            if (isInside != linalg::norm2(base) < 1) return true;
            base[0] -= edges[0];
            if (isInside != linalg::norm2(base) < 1) return true;
            return false;
        };

        // Check fresh scan
        CIE_TEST_REQUIRE_NOTHROW(tree.scan(unitCircle, 3));
        CIE_TEST_CHECK(tree.size() == 61);

        // Check scan following a full tree
        const auto infiniteBoundary = [](Ref<const Tree::Node> r_node) {return true;};
        CIE_TEST_REQUIRE_NOTHROW(tree.scan(infiniteBoundary, 3));
        CIE_TEST_REQUIRE_NOTHROW(tree.scan(unitCircle, 3));
        CIE_TEST_CHECK(tree.size() == 61);

    }
}


} // namespace cie::geo