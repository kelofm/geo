#ifndef CIE_GEO_PARTITIONING_AABBOX_NODE_HPP
#define CIE_GEO_PARTITIONING_AABBOX_NODE_HPP

// --- Utility Includes ---
#include "packages/trees/inc/abstree.hpp"

// --- Internal Includes ---
#include "packages/partitioning/inc/AABBox.hpp"
#include "packages/partitioning/inc/boundingBox.hpp"
#include "packages/trees/inc/Cell.hpp"

// --- STL Includes ---
#include <deque>
#include <memory>


namespace cie::geo {


/**
 * Class for enclosing a set of boundable objects, and providing
 * a space partitioned access to them.
 */
template <concepts::BoxBoundable TObject>
class AABBoxNode :
    public Cell<AABBox<GetTraits<TObject>::Type::Dimension,typename GetTraits<TObject>::Type::Coordinate>>,
    public utils::AbsTree<AABBoxNode<TObject>,std::deque,std::shared_ptr<AABBoxNode<TObject>>>,
    public std::enable_shared_from_this<AABBoxNode<TObject>>
{
private:
    using CellBase = Cell<AABBox<GetTraits<TObject>::Type::Dimension,typename GetTraits<TObject>::Type::Coordinate>>;

    using TreeBase = utils::AbsTree<AABBoxNode<TObject>,std::deque,std::shared_ptr<AABBoxNode<TObject>>>;

public:
    using object_type          = TObject;

    using object_ptr           = std::weak_ptr<object_type>;

    using object_ptr_container = std::deque<object_ptr>;

    using self_ptr             = std::weak_ptr<AABBoxNode<TObject>>;

public:
    AABBoxNode(const typename AABBoxNode<TObject>::Point& r_base,
               const typename AABBoxNode<TObject>::Point& r_lengths,
               self_ptr p_parent);

    AABBoxNode();

    /**
     * If the object fits in this box, add it to the list of objects
     * and send it down the tree
     * @param p_object pointer to object to add
     */
    void addObject(object_ptr p_object);

    /// Recursively erase expired objects from storage
    void eraseExpired();

    /**
     * Readjust box to fit all remaining objects,
     * then adjust children sizes as well
     */
    void shrink();

    /**
     * @brief Find highest level node that contains the query object
     * @param p_object pointer to query object
     * @return pointer to node containing the query object, or a default
     * constructed pointer if the search fails
     */
    self_ptr find(const object_ptr p_object);

    /**
     * Subdivide nodes until the number of contained objects reaches or
     * drops below the specified limit, or the recursion depth is reached
     * @param maxObjects maximum number of objects in leaf nodes (termination criterion)
     * @param maxLevel recursion depth limit (termination constraint)
     * @return false if recursion depth is reached before the object limit
     */
    bool partition(Size maxObjects,
                    Size maxLevel);

    /// Access to contained objects
    const object_ptr_container& containedObjects() const;

    /// Access to intersected objects
    const object_ptr_container& intersectedObjects() const;

    /**
     * Parent node access
     * @return default constructed pointer if this node is the root
     */
    self_ptr parent();

    /**
     * Parent node access
     * @return reference to default constructed pointer if this node is the root
     */
    const self_ptr& parent() const;

protected:
    object_ptr_container _containedObjects;

    object_ptr_container _intersectedObjects;

    self_ptr             _p_parent;
};


} // namespace cie::geo

#include "packages/partitioning/impl/AABBoxNode_impl.hpp"

#endif