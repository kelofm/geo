#ifndef CIE_GEO_CONTIGUOUS_SPACE_TREE_IMPL_HPP
#define CIE_GEO_CONTIGUOUS_SPACE_TREE_IMPL_HPP

// --- GEO Includes ---
#include "packages/exceptions/inc/exception.hpp"
#include "packages/macros/inc/checks.hpp"
#include "packages/macros/inc/exceptions.hpp"
#include "packages/macros/inc/typedefs.hpp"
#include "packages/trees/inc/ContiguousSpaceTree.hpp"

// --- STL Includes ---
#include <cstddef>
#include <iterator>
#include <limits>
#include <stack>


namespace cie::geo {


namespace detail {


template <Size Dim, concepts::Integer TIndex>
CSTNode<Dim,TIndex>
CSTNode<Dim,TIndex>::makeRoot()
{
    return CSTNode(0);
}


template <Size Dim, concepts::Integer TIndex>
CSTNode<Dim,TIndex>::CSTNode() noexcept
    : CSTNode(std::numeric_limits<TIndex>::max(), 0)
{
}


template <Size Dim, concepts::Integer TIndex>
CSTNode<Dim,TIndex>::CSTNode(TIndex parent) noexcept
    : CSTNode(parent, 0)
{
}


template <Size Dim, concepts::Integer TIndex>
CSTNode<Dim,TIndex>::CSTNode(TIndex parent, TIndex childBegin) noexcept
    : _i_parent(parent),
      _i_childBegin(childBegin)
{
}


template <Size Dim, concepts::Integer TIndex>
inline void
CSTNode<Dim,TIndex>::setNull() noexcept
{
    *this = CSTNode();
}


template <Size Dim, concepts::Integer TIndex>
inline Bool
CSTNode<Dim,TIndex>::isNull() const noexcept
{
    return _i_parent == std::numeric_limits<TIndex>::max() && _i_childBegin == 0;
}


template <Size Dim, concepts::Integer TIndex>
inline Bool
CSTNode<Dim,TIndex>::isLeaf() const noexcept
{
    return !_i_childBegin;
}


template <Size Dim, concepts::Integer TIndex>
inline TIndex
CSTNode<Dim,TIndex>::getParentIndex() const noexcept
{
    return _i_parent;
}


template <Size Dim, concepts::Integer TIndex>
inline TIndex
CSTNode<Dim,TIndex>::getChildBegin() const noexcept
{
    return _i_childBegin;
}


template <Size Dim, concepts::Integer TIndex>
inline void
CSTNode<Dim,TIndex>::setChildBegin(TIndex childBegin) noexcept
{
    _i_childBegin = childBegin;
}


template <Size Dim, concepts::Integer TIndex>
inline void
CSTNode<Dim,TIndex>::unsetChildBegin() noexcept
{
    _i_childBegin = 0;
}


} // namespace detail


template <concepts::Object TGeometry, concepts::Integer TIndex, class TTag>
constexpr unsigned ContiguousSpaceTree<TGeometry,TIndex,TTag>::maxLevels()
{
    Size size = std::numeric_limits<TIndex>::max();
    unsigned output = 0;

    do {
        size /= Node::ChildrenPerNode;
        ++output;
    } while (size);
    return output;
}


template <concepts::Object TGeometry, concepts::Integer TIndex, class TTag>
template <class ...TArgs>
ContiguousSpaceTree<TGeometry,TIndex,TTag>::ContiguousSpaceTree(RightRef<TArgs>... r_arguments)
    : GeometryBase(std::forward<TArgs>(r_arguments)...),
      TreeBase()
{
}


template <concepts::Object TGeometry, concepts::Integer TIndex, class TTag>
template <concepts::Iterator<typename TGeometry::Coordinate> TBaseIt, concepts::Iterator<typename TGeometry::Coordinate> TLengthIt>
inline void
ContiguousSpaceTree<TGeometry,TIndex,TTag>::getNodeGeometry(TIndex i_node, TBaseIt it_baseBegin, TLengthIt it_lengthBegin) const
{
    Ref<const Node> r_node = this->_nodes[i_node];
    this->getNodeGeometry(r_node, it_baseBegin, it_lengthBegin);
}


template <concepts::Object TGeometry, concepts::Integer TIndex, class TTag>
template <concepts::Iterator<typename TGeometry::Coordinate> TBaseIt, concepts::Iterator<typename TGeometry::Coordinate> TLengthIt>
inline void
ContiguousSpaceTree<TGeometry,TIndex,TTag>::getNodeGeometry(Ref<const Node> r_node, TBaseIt it_baseBegin, TLengthIt it_lengthBegin) const
{
    this->getNodeGeometryImpl(r_node, it_baseBegin, it_lengthBegin);
}


namespace detail {


/// @brief Dummy iterator returning identical nodes sharing the same parent.
template <class TNode>
class CSTNodeInputIterator
{
public:
    using Index = typename TNode::Index;

    using value_type = TNode;

    using pointer = Ptr<value_type>;

    using reference = Ref<value_type>;

    using difference_type = std::ptrdiff_t;

    using iterator_category = std::forward_iterator_tag;

public:
    CSTNodeInputIterator() = delete;

    CSTNodeInputIterator(Index i_parent) noexcept
        : _i_parent(i_parent)
    {}

    CIE_DEFINE_CLASS_DEFAULT_MOVES(CSTNodeInputIterator)

    CIE_DEFINE_CLASS_DEFAULT_COPIES(CSTNodeInputIterator)

    value_type operator*() const noexcept
    {return TNode(_i_parent);}

    Ref<CSTNodeInputIterator> operator++() noexcept
    {return *this;}

    CSTNodeInputIterator operator++(int) noexcept
    {return CSTNodeInputIterator(*this);}

    friend CSTNodeInputIterator operator+(CSTNodeInputIterator lhs, difference_type)
    {return CSTNodeInputIterator(lhs._i_parent);}

    friend Bool operator<(CSTNodeInputIterator, CSTNodeInputIterator)
    {return false;}

    friend Bool operator!=(CSTNodeInputIterator, CSTNodeInputIterator)
    {CIE_THROW(NotImplementedException, "") return true;}

private:
    Index _i_parent;
}; // class NodeInputIterator


} // namespace detail


template <concepts::Object TGeometry, concepts::Integer TIndex, class TTag>
inline TIndex
ContiguousSpaceTree<TGeometry,TIndex,TTag>::split(TIndex i_node)
{
    CIE_CHECK(!this->_nodes[i_node].isNull(), "Split null node at " << i_node)
    CIE_CHECK(this->_nodes[i_node].isLeaf(), "Resplit a node at " << i_node)
    const auto i_child = this->insert(detail::CSTNodeInputIterator<Node>(i_node));
    this->_nodes[i_node].setChildBegin(i_child);
    return i_child;
}


template <concepts::Object TGeometry, concepts::Integer TIndex, class TTag>
template <concepts::CallableWith<Ref<const detail::CSTNode<TGeometry::Dimension,TIndex>>> TPredicate>
inline void
ContiguousSpaceTree<TGeometry,TIndex,TTag>::scan(TPredicate&& r_predicate, TIndex maxDepth)
{
    CIE_BEGIN_EXCEPTION_TRACING

    if (maxDepth) {
        using Pair = std::pair<TIndex,TIndex>; // <== {i_node, depth}
        std::stack<Pair,DynamicArray<Pair>> indexStack;

        { // Split root
            Ref<Node> r_node = this->_nodes[0];
            if (r_predicate(r_node) && 1 <= maxDepth) {
                indexStack.emplace(r_node.isLeaf() ? this->split(0) : r_node.getChildBegin(),
                                   1);
            } else if (!r_node.isLeaf()) {
                this->erase(r_node.getChildBegin());
                r_node.unsetChildBegin();
            }
        } // Split root

        while (!indexStack.empty()) {
            const auto pair = indexStack.top();
            indexStack.pop();
            const auto newDepth = pair.second + 1;

            // Loop over the siblings of the popped node (including the popped node)
            for (TIndex i_sibling=0; i_sibling<Node::ChildrenPerNode; ++i_sibling) {
                const TIndex i_node = pair.first + i_sibling;               // <== ID of the sibling
                Ref<Node> r_node = this->_nodes[i_node];                    // <== mutable ref to the sibling
                Ref<const Node> r_constNode = r_node;                       // <== immutable ref to the sibling

                // Recurse a level further if the predicate approves AND the next level is enabled
                if (newDepth <= maxDepth && r_predicate(r_constNode)) {
                    indexStack.emplace(r_node.isLeaf() ? this->split(i_node) : r_node.getChildBegin(),
                                       newDepth);
                } else if (!r_node.isLeaf()) { // <== recursion denied => erase the children
                    this->erase(r_node.getChildBegin());
                    r_node.unsetChildBegin();
                }
            } // for i_sibling
        } // while indexStack
    } // if maxDepth

    CIE_END_EXCEPTION_TRACING
}


template <concepts::Object TGeometry, concepts::Integer TIndex, class TTag>
inline TIndex
ContiguousSpaceTree<TGeometry,TIndex,TTag>::getIndex(Ref<const Node> r_node) const
{
    CIE_OUT_OF_RANGE_CHECK(!this->empty())
    CIE_OUT_OF_RANGE_CHECK(this->_nodes.data() <= &r_node && &r_node < this->_nodes.data() + this->_nodes.size())
    return std::distance(this->_nodes.data(), &r_node);
}


template <concepts::Object TGeometry, concepts::Integer TIndex, class TTag>
inline std::pair<typename ContiguousSpaceTree<TGeometry,TIndex,TTag>::NodeTrace,unsigned>
ContiguousSpaceTree<TGeometry,TIndex,TTag>::getNodeTrace(Ref<const Node> r_node) const
{
    auto index = this->getIndex(r_node);
    Ptr<const Node> p_node = &r_node;
    unsigned bitOffset = 0u; // <== offset for bit operations
    NodeTrace trace = 0;

    while (!p_node->isNull() && index) {
        unsigned localIndex = (index - 1) % Node::ChildrenPerNode;
        trace |= localIndex << bitOffset;

        // Get ready for the next iteration
        p_node = &this->_nodes[p_node->getParentIndex()];
        index = this->getIndex(*p_node);
        bitOffset += Dimension;
    }

    return std::make_pair(trace, bitOffset);
}


template <concepts::Object TGeometry, concepts::Integer TIndex, class TTag>
template <concepts::Iterator<typename TGeometry::Coordinate> TBaseIt, concepts::Iterator<typename TGeometry::Coordinate> TLengthIt>
inline void
ContiguousSpaceTree<TGeometry,TIndex,TTag>::getNodeGeometryImpl(Ref<const Node> r_node, TBaseIt it_baseBegin, TLengthIt it_lengthBegin) const
requires concepts::Cube<TGeometry>
{
    CIE_BEGIN_EXCEPTION_TRACING

    std::copy(this->base().begin(),
              this->base().end(),
              it_baseBegin);
    auto [trace, bitOffset] = this->getNodeTrace(r_node);

    Ref<Coordinate> r_length = *it_lengthBegin;
    r_length = this->length();

    for (; bitOffset; bitOffset-=Dimension) {
        r_length /= 2;
        auto it_base = it_baseBegin;
        const auto localIndex = trace >> (bitOffset - Dimension); // <== cell index within its parent
        for (unsigned i_dim=0; i_dim<Dimension; ++i_dim, ++it_base) {
            *it_base += bool(localIndex & (1 << i_dim)) * r_length;
        }
    }

    CIE_END_EXCEPTION_TRACING
}


template <concepts::Object TGeometry, concepts::Integer TIndex, class TTag>
template <concepts::Iterator<typename TGeometry::Coordinate> TBaseIt, concepts::Iterator<typename TGeometry::Coordinate> TLengthIt>
inline void
ContiguousSpaceTree<TGeometry,TIndex,TTag>::getNodeGeometryImpl(Ref<const Node> r_node, TBaseIt it_baseBegin, TLengthIt it_lengthBegin) const
requires concepts::Box<TGeometry>
{
    CIE_BEGIN_EXCEPTION_TRACING

    std::copy(this->base().begin(),
              this->base().end(),
              it_baseBegin);

    std::copy(this->lengths().begin(),
              this->lengths().end(),
              it_lengthBegin);

    auto [trace, bitOffset] = this->getNodeTrace(r_node);

    for (; bitOffset; bitOffset-=Dimension) {
        auto it_base = it_baseBegin;
        auto it_length = it_lengthBegin;
        const auto localIndex = trace >> (bitOffset - Dimension); // <== cell index within its parent
        for (unsigned i_dim=0; i_dim<Dimension; ++i_dim, ++it_base, ++it_length) {
            *it_length /= 2;
            *it_base += bool(localIndex & (1 << i_dim)) * (*it_length);
        }
    }

    CIE_END_EXCEPTION_TRACING
}


} // namespace cie::geo


#endif
