#ifndef CIE_GEO_CONTIGUOUS_SPACE_TREE_HPP
#define CIE_GEO_CONTIGUOUS_SPACE_TREE_HPP

// --- GEO Includes ---
#include "packages/macros/inc/typedefs.hpp"
#include "packages/primitives/inc/Traits.hpp"
#include "packages/primitives/inc/Cube.hpp"
#include "packages/primitives/inc/Box.hpp"

// --- Utility Includes ---
#include "packages/trees/inc/ContiguousTree.hpp"
#include "packages/maths/inc/power.hpp"
#include "packages/stl_extension/inc/StaticArray.hpp"
#include "packages/compile_time/packages/type_select/inc/integer.hpp"


namespace cie::geo {


namespace detail {


template <Size Dim, concepts::Integer TIndex>
class CSTNode
{
private:
    template <concepts::Object TG, concepts::Integer TI>
    friend class ContiguousSpaceTree;

public:
    using Index = TIndex;

    static constexpr const inline Size Dimension = Dim;

    static constexpr const inline Size ChildrenPerNode = intPow(2, Dimension);

public:
    CSTNode() noexcept;

    CIE_DEFINE_CLASS_DEFAULT_MOVES(CSTNode)

    CSTNode(TIndex parent) noexcept;

    CSTNode(TIndex parent, TIndex childBegin) noexcept;

    static CSTNode makeRoot();

    void setNull() noexcept;

    Bool isNull() const noexcept;

    Bool isLeaf() const noexcept;

    TIndex getParentIndex() const noexcept;

    TIndex getChildBegin() const noexcept;

    void setChildBegin(TIndex childBegin) noexcept;

    void unsetChildBegin() noexcept;

private:
    CIE_DEFINE_CLASS_DEFAULT_COPIES(CSTNode)

private:
    TIndex _iParent;

    TIndex _iChildBegin;
}; // class CSTNode


} // namespace detail


template <concepts::Object TGeometry,
          concepts::Integer TIndex,
          class TParallelTag = tags::Serial>
class ContiguousSpaceTree : public TGeometry,
                            public utils::ContiguousTree<detail::CSTNode<TGeometry::Dimension,TIndex>,
                                                         TIndex,
                                                         TParallelTag>
{
public:
    using Geometry = TGeometry;

    using Coordinate = typename TGeometry::Coordinate;

    static constexpr const inline Size Dimension = Geometry::Dimension;

    using Index = TIndex;

    using Node = detail::CSTNode<Dimension,TIndex>;

    CIE_DEFINE_CLASS_POINTERS(ContiguousSpaceTree)

private:
    using TreeBase = utils::ContiguousTree<Node,TIndex,TParallelTag>;

    using GeometryBase = TGeometry;

    using TraceIndex = typename ct::Unsigned::ShortestWithRange<0,Node::ChildrenPerNode>;

    using NodeTrace = Size;

    static constexpr unsigned maxLevels();

public:
    ContiguousSpaceTree() noexcept = default;

    CIE_DEFINE_CLASS_DEFAULT_MOVES(ContiguousSpaceTree)

    CIE_DEFINE_CLASS_DEFAULT_COPIES(ContiguousSpaceTree)

    template <class ...TArgs>
    ContiguousSpaceTree(RightRef<TArgs>... rArguments);

    /** @brief Get the base and length(s) of the node at the given index.
     *  @tparam TBaseIt: Output iterator of @ref Coordinate types.
     *  @tparam TLengthIt: Output iterator of @ref Coordinate types.
     *  @param iNode: node index
     *  @param itBaseBegin: Output iterator pointing to the first component of the base.
     *                       The referenced container must have at least size @p Node::Dimension.
     *  @param itLengthBegin: Output iterator pointing to the first component of the lengths.
     *                         If @a TGeometry is a @ref Box, the referenced container must have
     *                         at least size @p Node::Dimension.
     */
    template <concepts::Iterator<Coordinate> TBaseIt, concepts::Iterator<Coordinate> TLengthIt>
    void getNodeGeometry(TIndex iNode, TBaseIt itBaseBegin, TLengthIt itLengthBegin) const;

    /** @brief Get the base and length(s) of the node at the given index.
     *  @tparam TBaseIt: Output iterator of @ref Coordinate types.
     *  @tparam TLengthIt: Output iterator of @ref Coordinate types.
     *  @param rNode: node
     *  @param itBaseBegin: Output iterator pointing to the first component of the base.
     *                       The referenced container must have at least size @p Node::Dimension.
     *  @param itLengthBegin: Output iterator pointing to the first component of the lengths.
     *                         If @a TGeometry is a @ref Box, the referenced container must have
     *                         at least size @p Node::Dimension.
     */
    template <concepts::Iterator<Coordinate> TBaseIt, concepts::Iterator<Coordinate> TLengthIt>
    void getNodeGeometry(Ref<const Node> rNode, TBaseIt itBaseBegin, TLengthIt itLengthBegin) const;

    /** @brief Subdivide a node at the given index.
     *  @param iNode: index of the node to be split.
     *  @returns The index of the first child.
     *  @throws - If the node at the given index is null.
     *          - [debug] If the node at the given index does not exist.
     *          - If an error occurs during the construction of child nodes.
     *          - If splitting requires reallocation, which fails.
     *  @note This function is thread-safe iff @ref TreeBase::insert is thread-safe.
     */
    TIndex split(TIndex iNode);

    /** @brief Split nodes while a provided predicate says so or until the max depth is reached.
     *  @tparam TPredicate: Unary predicate with the following signature: @code bool(Ref<const Node>, TIndex) @endcode
     *          The first argument refers to the current node, while the second one is its level.
     *  @param rPredicate: predicate deciding whether a node should be split.
     *  @note Nodes at @a maxDepth are not fed to the predicate.
     */
    template <concepts::CallableWith<Ref<const detail::CSTNode<TGeometry::Dimension,TIndex>>,TIndex> TPredicate>
    void scan(TPredicate&& rPredicate);

    TIndex getIndex(Ref<const Node> rNode) const;

private:
    [[nodiscard]] std::pair<NodeTrace,unsigned> getNodeTrace(Ref<const Node> rNode) const;

    template <class TPredicate>
    void scanImpl(TPredicate&& rPredicate, TIndex maxDepth);

    template <concepts::Iterator<Coordinate> TBaseIt, concepts::Iterator<Coordinate> TLengthIt>
    void getNodeGeometryImpl(Ref<const Node> rNode,
                             TBaseIt itBaseBegin,
                             TLengthIt itLengthBegin) const
    requires concepts::Cube<TGeometry>;

    template <concepts::Iterator<Coordinate> TBaseIt, concepts::Iterator<Coordinate> TLengthIt>
    void getNodeGeometryImpl(Ref<const Node> rNode,
                             TBaseIt itBaseBegin,
                             TLengthIt itLengthBegin) const
    requires concepts::Box<TGeometry>;
}; // class ContiguousSpaceTree


template <class TValue,
          class TIndex,
          unsigned Dimension,
          class TExecutionTag = tags::Serial>
using ContiguousCubeTree = ContiguousSpaceTree<Cube<Dimension,TValue>,TIndex>;


template <class TValue,
          class TIndex,
          unsigned Dimension,
          class TExecutionTag = tags::Serial>
using ContiguousBoxTree = ContiguousSpaceTree<Box<Dimension,TValue>,TIndex>;


} // namespace cie::geo

#include "packages/trees/impl/ContiguousSpaceTree_impl.hpp"

#endif
