#pragma once

// --- Utility Includes ---
#include "packages/trees/inc/abstree.hpp"

// --- Internal Includes ---
#include "packages/partitioning/inc/AABBox.hpp"
#include "packages/partitioning/inc/boundingBox.hpp"
#include "packages/trees/inc/Cell.hpp"

// --- STL Includes ---
#include <deque>


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

    using ObjectPtrContainer = std::deque<Ptr<ObjectType>>;

    using typename CellBase::Point;

public:
    AABBoxNode(const Point& rBase,
               const Point& rLengths,
               AABBoxNode* pParent) noexcept;

    AABBoxNode() noexcept;

    /**
     * If the object fits in this box, add it to the list of objects
     * and send it down the tree
     * @param pObject pointer to object to add
     */
    void insert(Ptr<ObjectType> pObject);

    /**
     * Readjust box to fit all remaining objects,
     * then adjust children sizes as well
     */
    void shrink();

    /**
     * @brief Find the highest level node that contains the input object.
     * @param pObject Pointer to input object.
     * @return Pointer to node containing the input object, or nullptr
     *         if the search fails.
     */
    AABBoxNode* find(Ptr<ObjectType> pObject);

    /// @brief Find the highest level node that contains the input point.
    /// @param rPoint Pointer to input point.
    /// @return Pointer to node containing the input point, or nullptr
    ///         if the search fails.
    Ptr<AABBoxNode> find(Ref<const Point> rPoint);

    /**
     * Subdivide nodes until the number of contained objects reaches or
     * drops below the specified limit, or the recursion depth is reached
     * @param maxObjects maximum number of objects in leaf nodes (termination criterion)
     * @param maxLevel recursion depth limit (termination constraint)
     * @return false if recursion depth is reached before the object limit
     */
    bool partition(Size maxObjects,
                   Size maxLevel);

    /// Access to contained objects.
    const ObjectPtrContainer& containedObjects() const noexcept;

    /// Access to intersected objects.
    const ObjectPtrContainer& intersectedObjects() const noexcept;

    /**
     * Parent node access
     * @return default constructed pointer if this node is the root
     */
    AABBoxNode* parent();

    /**
     * Parent node access
     * @return reference to default constructed pointer if this node is the root
     */
    const AABBoxNode* parent() const;

protected:
    ObjectPtrContainer _containedObjects;

    ObjectPtrContainer _intersectedObjects;

    AABBoxNode* _pParent;
};


} // namespace cie::geo

#include "packages/partitioning/impl/AABBoxNode_impl.hpp"
