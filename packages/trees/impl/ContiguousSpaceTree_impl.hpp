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
    : _iParent(parent),
      _iChildBegin(childBegin)
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
    return _iParent == std::numeric_limits<TIndex>::max() && _iChildBegin == 0;
}


template <Size Dim, concepts::Integer TIndex>
inline Bool
CSTNode<Dim,TIndex>::isLeaf() const noexcept
{
    return !_iChildBegin;
}


template <Size Dim, concepts::Integer TIndex>
inline TIndex
CSTNode<Dim,TIndex>::getParentIndex() const noexcept
{
    return _iParent;
}


template <Size Dim, concepts::Integer TIndex>
inline TIndex
CSTNode<Dim,TIndex>::getChildBegin() const noexcept
{
    return _iChildBegin;
}


template <Size Dim, concepts::Integer TIndex>
inline void
CSTNode<Dim,TIndex>::setChildBegin(TIndex childBegin) noexcept
{
    _iChildBegin = childBegin;
}


template <Size Dim, concepts::Integer TIndex>
inline void
CSTNode<Dim,TIndex>::unsetChildBegin() noexcept
{
    _iChildBegin = 0;
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
ContiguousSpaceTree<TGeometry,TIndex,TTag>::ContiguousSpaceTree(RightRef<TArgs>... rArguments)
    : GeometryBase(std::forward<TArgs>(rArguments)...),
      TreeBase()
{
}


template <concepts::Object TGeometry, concepts::Integer TIndex, class TTag>
template <concepts::Iterator<typename TGeometry::Coordinate> TBaseIt, concepts::Iterator<typename TGeometry::Coordinate> TLengthIt>
inline void
ContiguousSpaceTree<TGeometry,TIndex,TTag>::getNodeGeometry(TIndex iNode, TBaseIt itBaseBegin, TLengthIt itLengthBegin) const
{
    Ref<const Node> rNode = this->_nodes[iNode];
    this->getNodeGeometry(rNode, itBaseBegin, itLengthBegin);
}


template <concepts::Object TGeometry, concepts::Integer TIndex, class TTag>
template <concepts::Iterator<typename TGeometry::Coordinate> TBaseIt, concepts::Iterator<typename TGeometry::Coordinate> TLengthIt>
inline void
ContiguousSpaceTree<TGeometry,TIndex,TTag>::getNodeGeometry(Ref<const Node> rNode, TBaseIt itBaseBegin, TLengthIt itLengthBegin) const
{
    this->getNodeGeometryImpl(rNode, itBaseBegin, itLengthBegin);
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

    CSTNodeInputIterator(Index iParent) noexcept
        : _iParent(iParent)
    {}

    CIE_DEFINE_CLASS_DEFAULT_MOVES(CSTNodeInputIterator)

    CIE_DEFINE_CLASS_DEFAULT_COPIES(CSTNodeInputIterator)

    value_type operator*() const noexcept
    {return TNode(_iParent);}

    Ref<CSTNodeInputIterator> operator++() noexcept
    {return *this;}

    CSTNodeInputIterator operator++(int) noexcept
    {return CSTNodeInputIterator(*this);}

    friend CSTNodeInputIterator operator+(CSTNodeInputIterator lhs, difference_type)
    {return CSTNodeInputIterator(lhs._iParent);}

    friend Bool operator<(CSTNodeInputIterator, CSTNodeInputIterator)
    {return false;}

    friend Bool operator!=(CSTNodeInputIterator, CSTNodeInputIterator)
    {CIE_THROW(NotImplementedException, "") return true;}

private:
    Index _iParent;
}; // class NodeInputIterator


} // namespace detail


template <concepts::Object TGeometry, concepts::Integer TIndex, class TTag>
inline TIndex
ContiguousSpaceTree<TGeometry,TIndex,TTag>::split(TIndex iNode)
{
    CIE_CHECK(!this->_nodes[iNode].isNull(), "Split null node at " << iNode)
    CIE_CHECK(this->_nodes[iNode].isLeaf(), "Resplit a node at " << iNode)
    const auto iChild = this->insert(detail::CSTNodeInputIterator<Node>(iNode));
    this->_nodes[iNode].setChildBegin(iChild);
    return iChild;
}


template <concepts::Object TGeometry, concepts::Integer TIndex, class TTag>
template <concepts::CallableWith<Ref<const detail::CSTNode<TGeometry::Dimension,TIndex>>,TIndex> TPredicate>
inline void
ContiguousSpaceTree<TGeometry,TIndex,TTag>::scan(TPredicate&& rPredicate)
{
    CIE_BEGIN_EXCEPTION_TRACING

    using Pair = std::pair<TIndex,TIndex>; // <== {iNode, depth}
    std::stack<Pair,DynamicArray<Pair>> indexStack;

    { // Split root
        Ref<Node> rNode = this->_nodes[0];
        if (rPredicate(rNode, 0)) {
            indexStack.emplace(rNode.isLeaf() ? this->split(0) : rNode.getChildBegin(),
                               1);
        } else if (!rNode.isLeaf()) {
            this->erase(rNode.getChildBegin());
            rNode.unsetChildBegin();
        }
    } // Split root

    while (!indexStack.empty()) {
        const auto pair = indexStack.top();
        indexStack.pop();
        const auto newDepth = pair.second + 1;

        // Loop over the siblings of the popped node (including the popped node)
        for (TIndex iSibling=0; iSibling<Node::ChildrenPerNode; ++iSibling) {
            const TIndex iNode = pair.first + iSibling;               // <== ID of the sibling
            Ref<Node> rNode = this->_nodes[iNode];                    // <== mutable ref to the sibling
            Ref<const Node> rConstNode = rNode;                       // <== immutable ref to the sibling

            // Recurse a level further if the predicate approves AND the next level is enabled
            if (rPredicate(rConstNode, newDepth)) {
                indexStack.emplace(rNode.isLeaf() ? this->split(iNode) : rNode.getChildBegin(),
                                    newDepth);
            } else if (!rNode.isLeaf()) { // <== recursion denied => erase the children
                this->erase(rNode.getChildBegin());
                rNode.unsetChildBegin();
            }
        } // for iSibling
    } // while indexStack

    CIE_END_EXCEPTION_TRACING
}


template <concepts::Object TGeometry, concepts::Integer TIndex, class TTag>
inline TIndex
ContiguousSpaceTree<TGeometry,TIndex,TTag>::getIndex(Ref<const Node> rNode) const
{
    CIE_OUT_OF_RANGE_CHECK(!this->empty())
    CIE_OUT_OF_RANGE_CHECK(this->_nodes.data() <= &rNode && &rNode < this->_nodes.data() + this->_nodes.size())
    return std::distance(this->_nodes.data(), &rNode);
}


template <concepts::Object TGeometry, concepts::Integer TIndex, class TTag>
inline std::pair<typename ContiguousSpaceTree<TGeometry,TIndex,TTag>::NodeTrace,unsigned>
ContiguousSpaceTree<TGeometry,TIndex,TTag>::getNodeTrace(Ref<const Node> rNode) const
{
    auto index = this->getIndex(rNode);
    Ptr<const Node> pNode = &rNode;
    unsigned bitOffset = 0u; // <== offset for bit operations
    NodeTrace trace = 0;

    while (!pNode->isNull() && index) {
        unsigned localIndex = (index - 1) % Node::ChildrenPerNode;
        trace |= localIndex << bitOffset;

        // Get ready for the next iteration
        pNode = &this->_nodes[pNode->getParentIndex()];
        index = this->getIndex(*pNode);
        bitOffset += Dimension;
    }

    return std::make_pair(trace, bitOffset);
}


template <concepts::Object TGeometry, concepts::Integer TIndex, class TTag>
template <concepts::Iterator<typename TGeometry::Coordinate> TBaseIt, concepts::Iterator<typename TGeometry::Coordinate> TLengthIt>
inline void
ContiguousSpaceTree<TGeometry,TIndex,TTag>::getNodeGeometryImpl(Ref<const Node> rNode, TBaseIt itBaseBegin, TLengthIt itLengthBegin) const
requires concepts::Cube<TGeometry>
{
    CIE_BEGIN_EXCEPTION_TRACING

    std::copy(this->base().begin(),
              this->base().end(),
              itBaseBegin);
    auto [trace, bitOffset] = this->getNodeTrace(rNode);

    Ref<Coordinate> rLength = *itLengthBegin;
    rLength = this->length();

    for (; bitOffset; bitOffset-=Dimension) {
        rLength /= 2;
        auto itBase = itBaseBegin;
        const auto localIndex = trace >> (bitOffset - Dimension); // <== cell index within its parent
        for (unsigned iDim=0; iDim<Dimension; ++iDim, ++itBase) {
            *itBase += bool(localIndex & (1 << iDim)) * rLength;
        }
    }

    CIE_END_EXCEPTION_TRACING
}


template <concepts::Object TGeometry, concepts::Integer TIndex, class TTag>
template <concepts::Iterator<typename TGeometry::Coordinate> TBaseIt, concepts::Iterator<typename TGeometry::Coordinate> TLengthIt>
inline void
ContiguousSpaceTree<TGeometry,TIndex,TTag>::getNodeGeometryImpl(Ref<const Node> rNode, TBaseIt itBaseBegin, TLengthIt itLengthBegin) const
requires concepts::Box<TGeometry>
{
    CIE_BEGIN_EXCEPTION_TRACING

    std::copy(this->base().begin(),
              this->base().end(),
              itBaseBegin);

    std::copy(this->lengths().begin(),
              this->lengths().end(),
              itLengthBegin);

    auto [trace, bitOffset] = this->getNodeTrace(rNode);

    for (; bitOffset; bitOffset-=Dimension) {
        auto itBase = itBaseBegin;
        auto itLength = itLengthBegin;
        const auto localIndex = trace >> (bitOffset - Dimension); // <== cell index within its parent
        for (unsigned iDim=0; iDim<Dimension; ++iDim, ++itBase, ++itLength) {
            *itLength /= 2;
            *itBase += bool(localIndex & (1 << iDim)) * (*itLength);
        }
    }

    CIE_END_EXCEPTION_TRACING
}


} // namespace cie::geo


#endif
