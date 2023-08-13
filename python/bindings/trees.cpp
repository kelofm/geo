// --- External Includes ---
#include "pybind11/attr.h"
#include "pybind11/functional.h"
#include "pybind11/numpy.h"

// --- GEO Includes ---
#include "python/bindings/trees.hpp"
#include "packages/trees/inc/ContiguousSpaceTree.hpp"

// --- Utility Includes ---
#include "python/bindings/inc/stl_extension.hpp"
#include "packages/ranges/inc/TransformView.hpp"
#include "packages/maths/inc/power.hpp"

// --- STL Includes ---
#include <memory>
#include <climits>
#include <optional>


namespace cie::geo {


template <class TTree>
class NodeWrapper
{
public:
    using Index = typename TTree::Index;

public:
    NodeWrapper(Ref<const typename TTree::Node> r_node,
                Ref<const TTree> r_tree) noexcept
        : _p_node(&r_node),
          _p_tree(&r_tree)
    {}

    NodeWrapper(const NodeWrapper& r_rhs) noexcept = default;

    auto getVertices() const noexcept
    {
        static_assert(TTree::Dimension < sizeof(unsigned) * CHAR_BIT,
                      "unsigned is insufficient for representing a bitfield of size TTree::Dimension");

        // Get node geometry from the tree
        typename TTree::Coordinate base[TTree::Dimension], edges[TTree::Dimension];
        this->_p_tree->getNodeGeometry(*this->_p_node, base, edges);
        if constexpr (concepts::Cube<TTree>) { // <== all edges of a cube are of equal length
            std::fill(edges + 1, edges + TTree::Dimension, *edges);
        }

        constexpr unsigned numberOfVertices = intPow(2u, TTree::Dimension);
        constexpr std::array<Size,2> shape {numberOfVertices, TTree::Dimension};
        auto output = makeNumpyArray<typename TTree::Coordinate,2>(shape.data());

        for (unsigned i_vertex=0; i_vertex<numberOfVertices; ++i_vertex) {
            for (unsigned i_dim=0; i_dim<TTree::Dimension; ++i_dim) {
                output.mutable_at(i_vertex, i_dim) = base[i_dim] + bool(i_vertex & (1<<i_dim)) * edges[i_dim];
            }
        }
        return output;
    }

    bool isLeaf() const noexcept
    {
        return this->_p_node->isLeaf();
    }

private:
    Ptr<const typename TTree::Node> _p_node;

    Ptr<const TTree> _p_tree;
}; // class NodeWrapper


template <class TFirst, class TSecond>
struct DummyPair
{
    TFirst first;
    TSecond second;
};


template <class TTree>
void makeContiguousSpaceTreeBindings(Ref<pybind11::module_> r_module,
                                     std::string r_name)
{
    constexpr unsigned Dimension = TTree::Dimension;
    using View = utils::TransformView<TTree,NodeWrapper<TTree>>;
    using Pair = DummyPair<TTree,std::optional<View>>;

    auto nodeBindings = pybind11::class_<NodeWrapper<TTree>, std::shared_ptr<NodeWrapper<TTree>>>(
            r_module,
            (r_name + "Node").c_str())
        .def(pybind11::init<Ref<const NodeWrapper<TTree>>>())
        .def("getVertices", &NodeWrapper<TTree>::getVertices)
        .def("isLeaf", &NodeWrapper<TTree>::isLeaf)
        ;

    auto treeBindings = pybind11::class_<Pair, std::shared_ptr<Pair>>(r_module, r_name.c_str())
        .def(pybind11::init([] (Ref<const StaticArray<double,2>> r_base, double edge) -> Pair {
                return Pair {TTree(r_base, edge), std::optional<View>()};
        }))
        .def("scan", [] (Ref<Pair> r_pair,
                         std::function<bool(Ref<const NodeWrapper<TTree>>)> predicate,
                         unsigned maxDepth) {
            // Convert the predicate to one that takes a normal node instead of a wrapped one.
            const auto wrappedPredicate = [predicate = std::move(predicate), &r_pair] (Ref<const typename TTree::Node> r_node) -> bool {
                NodeWrapper<TTree> node(r_node, r_pair.first);
                const bool output = predicate(node);
                return output;
            };
            r_pair.first.scan(wrappedPredicate, maxDepth);
        })
        .def("visit", [] (Ref<const Pair> r_pair,
                          Ref<const std::function<bool(Ref<const NodeWrapper<TTree>>, typename TTree::Index)>> r_functor) {
            const auto wrappedFunctor = [&r_pair, &r_functor] (Ref<const typename TTree::Node> r_node,
                                                               typename TTree::Index level) {
                NodeWrapper<TTree> node(r_node, r_pair.first);
                return r_functor(node, level);
            };
            r_pair.first.visit(wrappedFunctor);
        })
        .def("__iter__", [] (Ref<Pair> r_pair) {
            r_pair.second.emplace(utils::makeTransformView<NodeWrapper<TTree>,TTree>(
                r_pair.first,
                [&r_pair] (Ref<const typename TTree::Node> r_node) -> NodeWrapper<TTree> {
                    return NodeWrapper<TTree>(r_node, r_pair.first);
            }));
            return pybind11::make_iterator(r_pair.second.value().begin(),
                                           r_pair.second.value().end());
        }, pybind11::keep_alive<0,1>())
        .def("size", [] (Ref<const Pair> r_pair) {
            return r_pair.first.size();
        })
        ;
}



void makeGEOTreeBindings(Ref<pybind11::module_> r_module)
{
    makeContiguousSpaceTreeBindings<ContiguousCubeTree<double,Size,2>>(
        r_module,
        "ContiguousCubeTree2D"
    );
}


} // namespace cie::geo
