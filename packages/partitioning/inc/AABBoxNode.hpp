#pragma once

// --- Utility Includes ---
#include "packages/trees/inc/abstree.hpp"
#include "packages/stl_extension/inc/DynamicArray.hpp"
#include "packages/compile_time/packages/concepts/inc/functional.hpp"
#include "packages/compile_time/packages/concepts/inc/container_concepts.hpp"

// --- GEO Includes ---
#include "packages/partitioning/inc/AABBox.hpp"
#include "packages/partitioning/inc/boundingBox.hpp"
#include "packages/trees/inc/Cell.hpp"
#include "packages/primitives/inc/Object.hpp"

// --- STL Includes ---
#include <span> // std::span
#include <optional> // std::optional


namespace cie::geo {


/**
 * Class for enclosing a set of boundable objects, and providing
 * a space partitioned access to them.
 */
template <concepts::BoxBoundable TObject>
class AABBoxNode :
    public Cell<AABBox<GetTraits<TObject>::Type::Dimension,typename GetTraits<TObject>::Type::Coordinate>>,
    public utils::AbsTree<AABBoxNode<TObject>,std::vector,Ptr<AABBoxNode<TObject>>>,
    public std::enable_shared_from_this<AABBoxNode<TObject>>
{
private:
    using CellBase = Cell<AABBox<GetTraits<TObject>::Type::Dimension,typename GetTraits<TObject>::Type::Coordinate>>;

    using TreeBase = utils::AbsTree<AABBoxNode<TObject>,std::vector,Ptr<AABBoxNode<TObject>>>;

public:
    using ObjectType = TObject;

    using typename CellBase::Point;

public:
    AABBoxNode(const Point& rBase,
               const Point& rLengths,
               AABBoxNode* pParent) noexcept;

    AABBoxNode() noexcept;

    /**
     * If the object fits in this box, add it to the list of objects.
     * @param pObject pointer to object to add
     */
    bool insert(Ptr<ObjectType> pObject);

    /**
     * Readjust box to fit all children and remaining objects.
     */
    void shrink();

    /**
     * @brief Find the highest level node that contains the input object.
     * @param pObject Pointer to input object.
     * @return Pointer to node containing the input object, or nullptr
     *         if the search fails.
     */
    Ptr<const AABBoxNode> find(Ptr<const ObjectType> pObject) const;

    /**
     * @brief Find the highest level node that contains the input object.
     * @param pObject Pointer to input object.
     * @return Pointer to node containing the input object, or nullptr
     *         if the search fails.
     */
    Ptr<AABBoxNode> find(Ptr<ObjectType> pObject);

    /// @brief Find the highest level node that contains the input point.
    /// @param rPoint Pointer to input point.
    /// @return Pointer to node containing the input point, or nullptr
    ///         if the search fails.
    Ptr<const AABBoxNode> find(Ref<const Point> rPoint) const;

    /// @brief Find the highest level node that contains the input point.
    /// @param rPoint Pointer to input point.
    /// @return Pointer to node containing the input point, or nullptr
    ///         if the search fails.
    Ptr<AABBoxNode> find(Ref<const Point> rPoint);

    /**
     * @details Subdivide nodes until the number of contained objects reaches or
     *          drops below the specified limit, or the maximum recursion depth is reached.
     * @param maxObjects maximum number of objects in leaf nodes (termination criterion)
     * @param maxLevel recursion depth limit (termination constraint)
     * @return false if recursion depth is reached before the object limit
     */
    bool partition(Size maxObjects, Size maxLevel);

    /// @brief Access to contained objects.
    std::span<const Ptr<const TObject>> contained() const noexcept;

    /// @brief Mutable access to contained objects.
    std::span<Ptr<TObject>> contained() noexcept;

    /// @brief Access to intersected objects.
    std::span<const Ptr<const TObject>> intersected() const noexcept;

    /// @brief Mutable access to intersected objects.
    std::span<Ptr<TObject>> intersected() noexcept;

    /**
     * Parent node access
     * @return default constructed pointer if this node is the root
     */
    Ptr<AABBoxNode> parent() noexcept;

    /**
     * Parent node access
     * @return reference to default constructed pointer if this node is the root
     */
    Ptr<const AABBoxNode> parent() const noexcept;

private:
    bool nonRecursiveInsert(Ptr<TObject> pObject);

    static void insertObject(Ptr<TObject> pObject, Ref<DynamicArray<Ptr<TObject>>> rSet);

    static DynamicArray<Ptr<TObject>>::iterator findObject(Ptr<TObject> pObject, Ref<DynamicArray<Ptr<TObject>>> rSet);

    static DynamicArray<Ptr<TObject>>::const_iterator findObject(Ptr<TObject> pObject, Ref<const DynamicArray<Ptr<TObject>>> rSet);

    static void eraseObjects(std::span<Ptr<TObject>> objects, Ref<DynamicArray<Ptr<TObject>>> rSet);

    DynamicArray<Ptr<TObject>> _containedObjects;

    DynamicArray<Ptr<TObject>> _intersectedObjects;

    AABBoxNode* _pParent;
};


template <concepts::Numeric TCoordinate,
          unsigned Dimension,
          concepts::UnsignedInteger TObjectIndex = unsigned,
          concepts::Allocator<std::byte> TAllocator = std::allocator<std::byte>>
class FlatAABBoxTree
{
public:
    template <bool TMutable>
    struct Node {
        using Geometry = boolean::Box<Dimension,TCoordinate>;

        Node() = delete;

        Node(Ptr<std::conditional_t<TMutable,std::byte,const std::byte>> pData)
            : _data(pData)
        {}

        std::span<const TCoordinate,Dimension> base() const noexcept;

        std::span<TCoordinate,Dimension> base() noexcept
        requires TMutable;

        std::span<const TCoordinate,Dimension> lengths() const noexcept;

        std::span<TCoordinate,Dimension> lengths() noexcept
        requires TMutable;

        std::size_t iSibling() const noexcept;

        Ref<std::size_t> iSibling() noexcept
        requires TMutable;

        unsigned containedCount() const noexcept;

        Ref<unsigned> containedCount() noexcept
        requires TMutable;

        unsigned intersectedCount() const noexcept;

        Ref<unsigned> intersectedCount() noexcept
        requires TMutable;

        std::span<const TObjectIndex> contained() const noexcept;

        std::span<TObjectIndex> contained() noexcept
        requires TMutable;

        std::span<const TObjectIndex> intersected() const noexcept;

        std::span<TObjectIndex> intersected() noexcept
        requires TMutable;

        Node</*TMutable=*/false> next() const noexcept;

        Node</*TMutable=*/true> next() noexcept
        requires TMutable;

    private:
        friend class FlatAABBoxTree;

        static constexpr unsigned staticSize() noexcept
        {
            return   Dimension * sizeof(TCoordinate)    // <== base
                   + Dimension * sizeof(TCoordinate)    // <== lengths
                   + sizeof(std::size_t)                // <== pointer to oldest younger sibling
                   + sizeof(unsigned)                   // <== number of contained objects
                   + sizeof(unsigned)                   // <== number of intersected objects
                   ;
        }

        /// @brief Size of the current instance in bytes.
        unsigned size() const noexcept;

        /// @brief Find an object that contains the provided point.
        /// @param rPoint Coordinates of the point to find.
        /// @param rObjects Range of objects the original @ref AABBoxNode "tree" was built over.
        /// @param pEnd Sentinel of the tree's allocated memory region.
        template <concepts::SamplableGeometry TObject>
        std::optional<Ptr<const TObject>> find(Ref<const std::span<const TCoordinate,Dimension>> rPoint,
                                               Ref<const std::span<const TObject>> rObjects,
                                               Ref<const std::span<const std::byte>> rData) const noexcept;

        Ptr<std::conditional_t<
            TMutable,
            std::byte,
            const std::byte
        >> _data;
    }; // struct Node

    /// @brief Find an object that contains the provided point.
    /// @param rPoint Coordinates of the point to find.
    /// @param rObjects Range of objects the original @ref AABBoxNode "tree" was built over.
    template <concepts::SamplableGeometry TObject>
    std::optional<Ptr<const TObject>> find(Ref<const std::span<const TCoordinate,Dimension>> rPoint,
                                           Ref<const std::span<const TObject>> rObjects) const noexcept;

    template <concepts::FunctionWithSignature<bool,Node</*TMutable=*/false>> TVisitor>
    void visit(TVisitor&& rVisitor) {
        typename FlatAABBoxTree::template Node<false> current(_data.data());
        const Ptr<const std::byte> pEnd = _data.data() + _data.size();
        while (current._data < pEnd) {
            if (!rVisitor(current)) break;
            current = current.next();
        }
    }

    template <concepts::SamplableGeometry TObject,
              concepts::FunctionWithSignature<TObjectIndex,Ref<const TObject>> THasher>
    requires (TObject::Dimension == Dimension && std::is_same_v<typename TObject::Coordinate,TCoordinate>)
    static FlatAABBoxTree flatten(Ref<const AABBoxNode<TObject>> rRoot,
                                  THasher&& rHasher,
                                  TAllocator&& rAllocator);

    ~FlatAABBoxTree();

private:
    FlatAABBoxTree(TAllocator&& rAllocator) noexcept;

    FlatAABBoxTree(const FlatAABBoxTree&) noexcept = default;

    FlatAABBoxTree& operator=(const FlatAABBoxTree&) noexcept = delete;

    std::optional<Node</*TMutable=*/false>> root() const noexcept;

    std::optional<Node</*TMutable=*/true>> root() noexcept;

    std::span<std::byte> _data;

    TAllocator _allocator;
}; // class FlatAABBoxTree


} // namespace cie::geo

#include "packages/partitioning/impl/AABBoxNode_impl.hpp"
