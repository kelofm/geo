// --- Utility Includes ---
#include "packages/testing/inc/essentials.hpp"
#include "packages/stl_extension/inc/StaticArray.hpp"

// --- Internal Includes ---
#include "packages/trees/inc/SpaceTreeNode.hpp"
#include "packages/trees/inc/Cell.hpp"
#include "packages/trees/inc/MidPointSplitPolicy.hpp"
#include "packages/trees/inc/LinearSplitPolicy.hpp"
#include "packages/trees/inc/WeightedSplitPolicy.hpp"
#include "packages/trees/inc/CartesianGridSampler.hpp"
#include "cmake_variables.hpp"

// --- STL includes ---
#include <deque>
#include <concepts>


namespace cie::geo {


template <class TPoint, class TValue>
requires (!std::is_same_v<TValue,Bool>)
TValue unitCircle(const TPoint& r_point)
{
    typename TPoint::value_type value = 0.0;
    for (const auto& r_component : r_point)
        value += r_component*r_component;

    return TValue(1.0 - value);
}


template <class TPoint, class TValue>
requires std::is_same_v<TValue,Bool>
TValue unitCircle(const TPoint& r_point)
{
    return unitCircle<TPoint,Float>(r_point) >= 0;
}


CIE_TEST_CASE("SpaceTreeNode", "[trees]")
{
    CIE_TEST_CASE_INIT("SpaceTreeNode")

    // Basic parameters
    const Size Dimension                = 2;
    using CoordinateType                = Double;
    using PointType                     = StaticArray<CoordinateType,Dimension>;
    using BoolValue                     = Bool;
    using NumericValue                  = Float;
    Size depth                          = 5;

    // Node with cube cell
    {
        CIE_TEST_CASE_INIT("Cube base with midpoint splitting")

        // Define specialised types
        using PrimitiveType = Cube<Dimension,CoordinateType>;
        using CellType      = Cell<PrimitiveType>;
        using NodeType      = SpaceTreeNode<CellType,BoolValue>;

        // Define primitive constructor parameters
        typename PrimitiveType::Point base = { 0.0, 0.0 };
        CoordinateType length                   = 1.0;

        // Construct components
        auto p_sampler = typename NodeType::sampler_ptr(
            new CartesianGridSampler<PrimitiveType>({3, 3})
        );

        auto p_splitPolicy = typename NodeType::split_policy_ptr(
            new MidPointSplitPolicy<typename NodeType::SamplePointIterator,
                                    typename NodeType::value_iterator>()
        );

        // Construct a node
        CIE_TEST_REQUIRE_NOTHROW(NodeType(p_sampler,
                                          p_splitPolicy,
                                          0,
                                          base,
                                          length));
        NodeType root(p_sampler,
                      p_splitPolicy,
                      0,
                      base,
                      length);

        CIE_TEST_CHECK_THROWS(root.isBoundary());

        // Evaluate unit circle
        NodeType::value_container_type values;
        CIE_TEST_CHECK_NOTHROW(root.evaluate(unitCircle<PointType,BoolValue>, values));
        CIE_TEST_REQUIRE(values.size() == 9);
        CIE_TEST_CHECK(root.isBoundary());

        CIE_TEST_CHECK(values[0] == true);  // <-- { 0.0, 0.0 }
        CIE_TEST_CHECK(values[1] == true);  // <-- { 0.5, 0.0 }
        CIE_TEST_CHECK(values[2] == true);  // <-- { 1.0, 0.0 }
        CIE_TEST_CHECK(values[3] == true);  // <-- { 0.0, 0.5 }
        CIE_TEST_CHECK(values[4] == true);  // <-- { 0.5, 0.5 }
        CIE_TEST_CHECK(values[5] == false); // <-- { 1.0, 0.5 }
        CIE_TEST_CHECK(values[6] == true);  // <-- { 0.0, 1.0 }
        CIE_TEST_CHECK(values[7] == false); // <-- { 0.5, 1.0 }
        CIE_TEST_CHECK(values[8] == false); // <-- { 1.0, 1.0 }

        // Divide
        CIE_TEST_CHECK_NOTHROW(root.divide(unitCircle<PointType,BoolValue>, depth));
    }

    // Node with box cell
    {
        CIE_TEST_CASE_INIT("Box base")

        // Define specialised types
        using PrimitiveType = Box<Dimension,CoordinateType>;
        using CellType      = Cell<PrimitiveType>;

        { // Midpoint splitting
            CIE_TEST_CASE_INIT("Midpoint splitter")

            using NodeType      = SpaceTreeNode<CellType,BoolValue>;

            // Define primitive constructor parameters
            typename PrimitiveType::Point base     = { 0.0, 0.0 };
            typename PrimitiveType::Point lengths  = { 1.0, 1.0 };

            // Construct components
            auto p_sampler = typename NodeType::sampler_ptr(
                new CartesianGridSampler<PrimitiveType>({3, 3})
            );

            auto p_splitPolicy = typename NodeType::split_policy_ptr(
                new MidPointSplitPolicy<typename NodeType::SamplePointIterator,
                                        typename NodeType::value_iterator>()
            );

            // Construct a node
            CIE_TEST_REQUIRE_NOTHROW(NodeType(  p_sampler,
                                        p_splitPolicy,
                                        0,
                                        base,
                                        lengths));
            NodeType root( p_sampler,
                            p_splitPolicy,
                            0,
                            base,
                            lengths);

            CIE_TEST_CHECK_THROWS(root.isBoundary());

            // Divide
            CIE_TEST_CHECK_NOTHROW(root.divide(unitCircle<PointType,BoolValue>, depth));
        } // Midpoint splitting

        { // Linear splitting
            CIE_TEST_CASE_INIT("Linear splitter")

            using NodeType      = SpaceTreeNode<CellType,NumericValue>;

            // Define primitive constructor parameters
            typename PrimitiveType::Point base     = { 0.0, 0.0 };
            typename PrimitiveType::Point lengths  = { 1.0, 1.0 };

            // Construct components
            auto p_sampler = typename NodeType::sampler_ptr(
                new CartesianGridSampler<PrimitiveType>({3, 3})
            );

            auto p_splitPolicy = typename NodeType::split_policy_ptr(
                new LinearSplitPolicy< typename NodeType::SamplePointIterator,
                                       typename NodeType::value_iterator>()
            );

            // Construct a node
            CIE_TEST_REQUIRE_NOTHROW(NodeType(  p_sampler,
                                        p_splitPolicy,
                                        0,
                                        base,
                                        lengths));
            NodeType root( p_sampler,
                            p_splitPolicy,
                            0,
                            base,
                            lengths);

            CIE_TEST_CHECK_THROWS(root.isBoundary());

            // Divide
            CIE_TEST_CHECK_NOTHROW(root.divide(unitCircle<PointType,NumericValue>, depth));
        } // Linear splitting

        { // Weighted splitting
            CIE_TEST_CASE_INIT("Weighted splitter")

            using NodeType      = SpaceTreeNode<CellType,NumericValue>;

            // Define primitive constructor parameters
            typename PrimitiveType::Point base     = { 0.0, 0.0 };
            typename PrimitiveType::Point lengths  = { 1.0, 1.0 };

            // Construct components
            auto p_sampler = typename NodeType::sampler_ptr(
                new CartesianGridSampler<PrimitiveType>({3, 3})
            );

            auto p_splitPolicy = typename NodeType::split_policy_ptr(
                new WeightedSplitPolicy< typename NodeType::SamplePointIterator,
                                         typename NodeType::value_iterator>()
            );

            // Construct a node
            CIE_TEST_REQUIRE_NOTHROW(NodeType(  p_sampler,
                                        p_splitPolicy,
                                        0,
                                        base,
                                        lengths));
            NodeType root( p_sampler,
                            p_splitPolicy,
                            0,
                            base,
                            lengths);

            CIE_TEST_CHECK_THROWS(root.isBoundary());

            // Divide
            CIE_TEST_CHECK_NOTHROW(root.divide(unitCircle<PointType,NumericValue>, depth));
        } // Weighted splitting

    } // Node with box cell
}


} // namespace cie::geo
