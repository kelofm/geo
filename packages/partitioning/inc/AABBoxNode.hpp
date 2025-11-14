#pragma once

// --- Utility Includes ---
#include "packages/trees/inc/abstree.hpp"
#include "packages/stl_extension/inc/DynamicArray.hpp"

// --- GEO Includes ---
#include "packages/partitioning/inc/AABBox.hpp"
#include "packages/partitioning/inc/boundingBox.hpp"
#include "packages/trees/inc/Cell.hpp"
#include "packages/primitives/inc/Object.hpp"

// --- STL Includes ---
#include <span> // std::span


namespace cie::geo {


/**
 * Class for enclosing a set of boundable objects, and providing
 * a space partitioned access to them.
 */
template <concepts::BoxBoundable TObject>
class AABBoxNode :
    public Cell<AABBox<GetTraits<TObject>::Type::Dimension,typename GetTraits<TObject>::Type::Coordinate>>,
    public utils::AbsTree<AABBoxNode<TObject>,std::deque,Ptr<AABBoxNode<TObject>>>,
    public std::enable_shared_from_this<AABBoxNode<TObject>>
{
private:
    using CellBase = Cell<AABBox<GetTraits<TObject>::Type::Dimension,typename GetTraits<TObject>::Type::Coordinate>>;

    using TreeBase = utils::AbsTree<AABBoxNode<TObject>,std::deque,Ptr<AABBoxNode<TObject>>>;

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


} // namespace cie::geo

#include "packages/partitioning/impl/AABBoxNode_impl.hpp"
