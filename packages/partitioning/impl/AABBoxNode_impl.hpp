#pragma once

// help the language server
#include "packages/partitioning/inc/AABBoxNode.hpp"

// --- Utility Includes ---
#include "packages/macros/inc/exceptions.hpp"
#include "packages/stl_extension/inc/resize.hpp"
#include "packages/trees/inc/abstree.hpp"

// --- STL Includes ---
#include <algorithm>
#include <stack>


namespace cie::geo {


template <concepts::BoxBoundable TObject>
AABBoxNode<TObject>::AABBoxNode(const Point& rBase,
                                const Point& rLenghts,
                                AABBoxNode* pParent) noexcept
    : AABBoxNode<TObject>::CellBase(rBase, rLenghts),
      AABBoxNode<TObject>::TreeBase(),
      _containedObjects(),
      _intersectedObjects(),
      _pParent(pParent)
{
    if (_pParent)
        this->_level = _pParent->_level + 1;
    else
        this->_level = 0;
}


template <concepts::BoxBoundable TObject>
AABBoxNode<TObject>::AABBoxNode() noexcept
    : AABBoxNode<TObject>(detail::makeOrigin<TObject::BoundingBox::Dimension,typename TObject::BoundingBox::Coordinate>(),
                          detail::makeOrigin<TObject::BoundingBox::Dimension,typename TObject::BoundingBox::Coordinate>(),
                          nullptr)
{
}


template <concepts::BoxBoundable TObject>
bool AABBoxNode<TObject>::insert(Ptr<ObjectType> pObject)
{
    if (this->parent()) CIE_THROW(Exception, "attempt to insert object into non-root node")
    if (!this->children().empty()) CIE_THROW(Exception, "attempt to insert into non-empty root node")

    if (this->contains(boundingBox(*pObject)))
        AABBoxNode::insertObject(pObject, this->_containedObjects);
    else if (this->intersects(boundingBox(*pObject)))
        AABBoxNode::insertObject(pObject, this->_intersectedObjects);
    else
        return false;
    return true;
}


template <concepts::BoxBoundable TObject>
void AABBoxNode<TObject>::shrink()
{
    auto visitor = [](AABBoxNode* pNode) -> bool {
        std::fill_n(pNode->lengths().data(), AABBoxNode::Dimension, static_cast<typename TObject::Coordinate>(0));

        for (const auto& pChild : pNode->children()) {
            for (unsigned iDimension=0u; iDimension<AABBoxNode::Dimension; ++iDimension) {
                pNode->include(*pChild);
            } // for iDimension in range(Dimension)
        } // for pChild in pNode->children()

        for (auto pObject : pNode->_containedObjects) {
            pNode->include(boundingBox(*pObject));
        } // for object in objects

        return true;
    };

    this->visit(visitor, utils::VisitStrategy::ReverseBreadthFirst);
}


template <concepts::BoxBoundable TObject>
Ptr<AABBoxNode<TObject>> AABBoxNode<TObject>::find(Ptr<ObjectType> pObject)
{
    Ptr<AABBoxNode> pContainingNode = nullptr;

    auto visitor = [&pObject, &pContainingNode](Ptr<AABBoxNode> pNode) -> bool {
        const auto& rBoundingBox = boundingBox(*pObject);

        if (pNode->contains(rBoundingBox)) {
            const bool found = AABBoxNode::findObject(pObject, pNode->_containedObjects) != pNode->_containedObjects.end();
            if (found) {
                pContainingNode = pNode;
                return false;
            } else {
                return true;
            }
        } else if (pNode->intersects(rBoundingBox)) {
            const bool found = AABBoxNode::findObject(pObject, pNode->_intersectedObjects) != pNode->_intersectedObjects.end();
            if (found) {
                pContainingNode = pNode;
                return false;
            } else {
                return true;
            }
        } else {
            return false;
        }
    };

    // Check whether the object is in this box,
    // if it is not, there is no point in searching subcells
    //if (visitor(this))
        this->visit(visitor);

    return pContainingNode;
}


template <concepts::BoxBoundable TObject>
Ptr<const AABBoxNode<TObject>> AABBoxNode<TObject>::find(Ptr<const ObjectType> pObject) const
{
    Ptr<const AABBoxNode> pContainingNode = nullptr;

    auto visitor = [&pObject, &pContainingNode](Ptr<const AABBoxNode> pNode) -> bool {
        const auto& rBoundingBox = boundingBox(*pObject);

        if (pNode->contains(rBoundingBox)) {
            const bool found = AABBoxNode::findObject(pObject, pNode->_containedObjects) != pNode->_containedObjects.end();
            if (found) {
                pContainingNode = pNode;
                return false;
            } else {
                return true;
            }
        } else if (pNode->intersects(rBoundingBox)) {
            const bool found = AABBoxNode::findObject(pObject, pNode->_intersectedObjects) != pNode->_intersectedObjects.end();
            if (found) {
                pContainingNode = pNode;
                return false;
            } else {
                return true;
            }
        } else {
            return false;
        }
    };

    // Check whether the object is in this box,
    // if it is not, there is no point in searching subcells
    //if (visitor(this))
        this->visit(visitor);

    return pContainingNode;
}


template <concepts::BoxBoundable TObject>
Ptr<const AABBoxNode<TObject>> AABBoxNode<TObject>::find(Ref<const Point> rPoint) const
{
    Ptr<const AABBoxNode> pContainingNode = nullptr;
    Size containingLevel = 0ul;

    const auto visitor = [&rPoint, &pContainingNode, &containingLevel](Ptr<const AABBoxNode> pNode) noexcept -> bool {
        if (pNode->at(rPoint)) {
            if (containingLevel <= pNode->level()) {
                pContainingNode = pNode;
                containingLevel = pNode->level();
            }

            if constexpr (concepts::SamplableGeometry<TObject>) {
                for (const auto& rpObject : pNode->contained()) {
                    if (rpObject->at(rPoint)) {
                        pContainingNode = pNode;
                        return false;
                    }
                }

                for (const auto& rpObject : pNode->intersected()) {
                    if (rpObject->at(rPoint)) {
                        pContainingNode = pNode;
                        return false;
                    }
                }
            }

            if (pNode->isLeaf()) {
                return false;
            } else {
                return true;
            }
        } else {
            return false;
        }
    };

    this->visit(visitor);
    return pContainingNode;
}


template <concepts::BoxBoundable TObject>
Ptr<AABBoxNode<TObject>> AABBoxNode<TObject>::find(Ref<const Point> rPoint)
{
    Ptr<AABBoxNode> pContainingNode = nullptr;
    Size containingLevel = 0ul;

    const auto visitor = [&rPoint, &pContainingNode, &containingLevel](Ptr<AABBoxNode> pNode) noexcept -> bool {
        if (pNode->at(rPoint)) {
            if (containingLevel <= pNode->level()) {
                pContainingNode = pNode;
                containingLevel = pNode->level();
            }

            if constexpr (concepts::SamplableGeometry<TObject>) {
                for (const auto& rpObject : pNode->contained()) {
                    if (rpObject->at(rPoint)) {
                        pContainingNode = pNode;
                        return false;
                    }
                }

                for (const auto& rpObject : pNode->intersected()) {
                    if (rpObject->at(rPoint)) {
                        pContainingNode = pNode;
                        return false;
                    }
                }
            }

            if (pNode->isLeaf()) {
                return false;
            } else {
                return true;
            }
        } else {
            return false;
        }
    };

    this->visit(visitor);
    return pContainingNode;
}


template <concepts::BoxBoundable TObject>
bool AABBoxNode<TObject>::partition(Size maxObjects,
                                    Size maxLevel)
{
    bool objectLimitReached = true;

    auto visitor = [=,&objectLimitReached](AABBoxNode* pNode) -> bool {
        if (auto pParent = pNode->parent()) {
            // Not root
            pNode->_containedObjects.clear();
            pNode->_intersectedObjects.clear();

            constexpr unsigned Dimension = AABBoxNode::Dimension;
            StaticArray<typename AABBoxNode::Point,intPow(2, Dimension)> corners;
            pNode->makeCorners(std::span<Point,intPow(2,Dimension)> {corners.data(), corners.size()});

            // Pull contained objects from the parent
            for (auto pObject : pParent->contained()) {
                if (pObject) {
                    if (pNode->contains(boundingBox(*pObject))) {
                        AABBoxNode::insertObject(pObject, pNode->_containedObjects);
                    } else if (pNode->intersects(boundingBox(*pObject))) {

                        // If the stored objects can be sampled, make sure that
                        // they're only intersected if the objects themselves are
                        // intersected, not only their bounding boxes.
                        bool intersected = false;
                        if constexpr (concepts::SamplableGeometry<TObject>) {
                            bool hasCornerInside = false, hasCornerOutside = false;
                            for (const auto& rCorner : corners) {
                                if (pObject->at(rCorner)) hasCornerInside = true;
                                else hasCornerOutside = true;
                                if (hasCornerInside && hasCornerOutside) {
                                    intersected = true;
                                    break;
                                }
                            } // for rCorner in corners
                        } /*if SamplableGeometry<TObject>*/ {
                            intersected = true;
                        }

                        if (intersected)
                            AABBoxNode::insertObject(pObject, pNode->_intersectedObjects);
                    }
                } // if pObject
            } // for pObject in pParent->contained()

            // Remove inserted objects from the parent.
            AABBoxNode::eraseObjects(pNode->_containedObjects, pParent->_containedObjects);
            AABBoxNode::eraseObjects(pNode->_intersectedObjects, pParent->_intersectedObjects);
        } // if pParent

        // Stop partitioning if the number of objects is within the limit
        if (pNode->_containedObjects.size() + pNode->_intersectedObjects.size() <= maxObjects)
            return true;

        // Stop partitioning if maximum recursion limit is reached
        if (maxLevel <= pNode->_level) {
            objectLimitReached = false;
            return true;
        }

        // Get node midpoint to use as splitting point
        typename AABBoxNode<TObject>::Point midPoint;
        utils::resize(midPoint, AABBoxNode<TObject>::Dimension);
        for (Size dim=0; dim<AABBoxNode<TObject>::Dimension; ++dim)
            midPoint[dim] = pNode->_base[dim] + pNode->_lengths[dim] / 2.0;

        // Subdivide node
        auto cellConstructors = pNode->split(midPoint);

        for (const auto& rCellConstructor : cellConstructors)
            pNode->_children.emplace_back(new AABBoxNode<TObject>(
                std::get<0>(rCellConstructor),      // base
                std::get<1>(rCellConstructor),      // lengths
                pNode                               // parent
           ));

        return true;
    };

    this->visit(visitor);

    return objectLimitReached;
}


template <concepts::BoxBoundable TObject>
std::span<const Ptr<const TObject>>
AABBoxNode<TObject>::contained() const noexcept
{
    return _containedObjects;
}


template <concepts::BoxBoundable TObject>
std::span<Ptr<TObject>>
AABBoxNode<TObject>::contained() noexcept
{
    return _containedObjects;
}


template <concepts::BoxBoundable TObject>
std::span<const Ptr<const TObject>>
AABBoxNode<TObject>::intersected() const noexcept
{
    return _intersectedObjects;
}


template <concepts::BoxBoundable TObject>
std::span<Ptr<TObject>>
AABBoxNode<TObject>::intersected() noexcept
{
    return _intersectedObjects;
}


template <concepts::BoxBoundable TObject>
Ptr<AABBoxNode<TObject>> AABBoxNode<TObject>::parent() noexcept
{
    return _pParent;
}


template <concepts::BoxBoundable TObject>
Ptr<const AABBoxNode<TObject>> AABBoxNode<TObject>::parent() const noexcept
{
    return _pParent;
}


template <concepts::BoxBoundable TObject>
void
AABBoxNode<TObject>::insertObject(Ptr<TObject> pObject, Ref<DynamicArray<Ptr<TObject>>> rSet)
{
    rSet.insert(std::upper_bound(rSet.begin(),
                                 rSet.end(),
                                 pObject,
                                 std::less<Ptr<TObject>>()),
                pObject);
}


template <concepts::BoxBoundable TObject>
DynamicArray<Ptr<TObject>>::iterator
AABBoxNode<TObject>::findObject(Ptr<TObject> pObject, Ref<DynamicArray<Ptr<TObject>>> rSet)
{
    auto it = std::lower_bound(rSet.begin(),
                               rSet.end(),
                               pObject,
                               std::less<Ptr<TObject>>());
    return (it != rSet.end() && (*it) == pObject) ? it : rSet.end();
}


template <concepts::BoxBoundable TObject>
DynamicArray<Ptr<TObject>>::const_iterator
AABBoxNode<TObject>::findObject(Ptr<TObject> pObject, Ref<const DynamicArray<Ptr<TObject>>> rSet)
{
    auto it = std::lower_bound(rSet.begin(),
                               rSet.end(),
                               pObject,
                               std::less<Ptr<TObject>>());
    return (it != rSet.end() && (*it) == pObject) ? it : rSet.end();
}


template <concepts::BoxBoundable TObject>
void
AABBoxNode<TObject>::eraseObjects(std::span<Ptr<TObject>> objects, Ref<DynamicArray<Ptr<TObject>>> rSet)
{
    DynamicArray<Ptr<TObject>> difference;
    difference.reserve(rSet.size());

    std::set_difference(rSet.begin(),
                        rSet.end(),
                        objects.begin(),
                        objects.end(),
                        std::back_inserter(difference));

    rSet = std::move(difference);
}


template <concepts::Numeric TCoordinate,
          unsigned Dimension,
          concepts::UnsignedInteger TObjectIndex,
          class TAllocator>
std::span<const TObjectIndex>
FlatAABBoxTree<TCoordinate,Dimension,TObjectIndex,TAllocator>::Node::contained() const noexcept
{
    const Ptr<const std::byte> pNodeBegin = static_cast<Ptr<const std::byte>>(static_cast<Ptr<const void>>(this));
    const Ptr<const std::byte> pContainedBegin = pNodeBegin + sizeof(Node);
    return {
        static_cast<Ptr<const TObjectIndex>>(static_cast<Ptr<const void>>(pContainedBegin)),
        containedCount
    };
}


template <concepts::Numeric TCoordinate,
          unsigned Dimension,
          concepts::UnsignedInteger TObjectIndex,
          class TAllocator>
std::span<TObjectIndex>
FlatAABBoxTree<TCoordinate,Dimension,TObjectIndex,TAllocator>::Node::contained() noexcept
{
    const Ptr<std::byte> pNodeBegin = static_cast<Ptr<std::byte>>(static_cast<Ptr<void>>(this));
    const Ptr<std::byte> pContainedBegin = pNodeBegin + sizeof(Node);
    return {
        static_cast<Ptr<TObjectIndex>>(static_cast<Ptr<void>>(pContainedBegin)),
        containedCount
    };
}


template <concepts::Numeric TCoordinate,
          unsigned Dimension,
          concepts::UnsignedInteger TObjectIndex,
          class TAllocator>
std::span<const TObjectIndex>
FlatAABBoxTree<TCoordinate,Dimension,TObjectIndex,TAllocator>::Node::intersected() const noexcept
{
    const auto contained = this->contained();
    return {
        contained.data() + contained.size(),
        intersectedCount
    };
}


template <concepts::Numeric TCoordinate,
          unsigned Dimension,
          concepts::UnsignedInteger TObjectIndex,
          class TAllocator>
std::span<TObjectIndex>
FlatAABBoxTree<TCoordinate,Dimension,TObjectIndex,TAllocator>::Node::intersected() noexcept
{
    const auto contained = this->contained();
    return {
        contained.data() + contained.size(),
        intersectedCount
    };
}


template <concepts::Numeric TCoordinate,
          unsigned Dimension,
          concepts::UnsignedInteger TObjectIndex,
          class TAllocator>
Ptr<const typename FlatAABBoxTree<TCoordinate,Dimension,TObjectIndex,TAllocator>::Node>
FlatAABBoxTree<TCoordinate,Dimension,TObjectIndex,TAllocator>::Node::next() const noexcept
{
    const auto intersected = this->intersected();
    const auto pNext = static_cast<Ptr<const Node>>(
        static_cast<Ptr<const void>>(
            intersected.data() + intersected.size()
        )
    );
    return pNext;
}


template <concepts::Numeric TCoordinate,
          unsigned Dimension,
          concepts::UnsignedInteger TObjectIndex,
          class TAllocator>
Ptr<typename FlatAABBoxTree<TCoordinate,Dimension,TObjectIndex,TAllocator>::Node>
FlatAABBoxTree<TCoordinate,Dimension,TObjectIndex,TAllocator>::Node::next() noexcept
{
    const auto intersected = this->intersected();
    const auto pNext = static_cast<Ptr<Node>>(
        static_cast<Ptr<void>>(
            intersected.data() + intersected.size()
        )
    );
    return pNext;
}


template <concepts::Numeric TCoordinate,
          unsigned Dimension,
          concepts::UnsignedInteger TObjectIndex,
          class TAllocator>
std::optional<Ptr<const typename FlatAABBoxTree<TCoordinate,Dimension,TObjectIndex,TAllocator>::Node>>
FlatAABBoxTree<TCoordinate,Dimension,TObjectIndex,TAllocator>::Node::child() const noexcept
{
    const auto pMaybeChild = this->next();
    if (pMaybeChild == this->maybeSibling) {
        return {};
    } else {
        return pMaybeChild;
    }
}


template <concepts::Numeric TCoordinate,
          unsigned Dimension,
          concepts::UnsignedInteger TObjectIndex,
          class TAllocator>
std::optional<Ptr<typename FlatAABBoxTree<TCoordinate,Dimension,TObjectIndex,TAllocator>::Node>>
FlatAABBoxTree<TCoordinate,Dimension,TObjectIndex,TAllocator>::Node::child() noexcept
{
    const auto pMaybeChild = this->next();
    if (pMaybeChild == this->maybeSibling) {
        return {};
    } else {
        return pMaybeChild;
    }
}


template <concepts::Numeric TCoordinate,
          unsigned Dimension,
          concepts::UnsignedInteger TObjectIndex,
          class TAllocator>
std::optional<Ptr<const typename FlatAABBoxTree<TCoordinate,Dimension,TObjectIndex,TAllocator>::Node>>
FlatAABBoxTree<TCoordinate,Dimension,TObjectIndex,TAllocator>::Node::sibling() const noexcept
{
    if (this->maybeSibling == nullptr) {
        return {};
    } else {
        return maybeSibling;
    }
}


template <concepts::Numeric TCoordinate,
          unsigned Dimension,
          concepts::UnsignedInteger TObjectIndex,
          class TAllocator>
std::optional<Ptr<typename FlatAABBoxTree<TCoordinate,Dimension,TObjectIndex,TAllocator>::Node>>
FlatAABBoxTree<TCoordinate,Dimension,TObjectIndex,TAllocator>::Node::sibling() noexcept
{
    if (this->maybeSibling == nullptr) {
        return {};
    } else {
        return maybeSibling;
    }
}


template <concepts::Numeric TCoordinate,
          unsigned Dimension,
          concepts::UnsignedInteger TObjectIndex,
          class TAllocator>
bool
FlatAABBoxTree<TCoordinate,Dimension,TObjectIndex,TAllocator>::Node::isLeaf() const noexcept
{
    return !this->child().has_value();
}


template <concepts::Numeric TCoordinate,
          unsigned Dimension,
          concepts::UnsignedInteger TObjectIndex,
          class TAllocator>
template <concepts::SamplableGeometry TObject>
std::optional<Ptr<const TObject>>
FlatAABBoxTree<TCoordinate,Dimension,TObjectIndex,TAllocator>::Node::find(Ref<const typename Geometry::Point> rPoint,
                                                                          std::span<const TObject> objects) const noexcept
{
    if (this->geometry.at(rPoint)) {
        for (const auto iObject : this->contained()) {
            Ref<const TObject> rObject = objects[iObject];
            if (rObject.at(rPoint)) return &rObject;
        } // for iObject in this->contained()

        for (const auto iObject : this->intersected()) {
            Ref<const TObject> rObject = objects[iObject];
            if (rObject.at(rPoint)) return &rObject;
        } // for iObject in this->intersected()

        const auto pMaybeChild = this->child();
        if (pMaybeChild.has_value()) {
            return pMaybeChild.value()->find(rPoint, objects);
        } /*if pMaybeChild.has_value()*/ else {
            return {};
        }
    } /*if this->geometry.at(rPoint)*/ else {
        const auto pMaybeSibling = this->sibling();
        if (pMaybeSibling.has_value()) {
            return pMaybeSibling.value()->find(rPoint, objects);
        } /*if pMaybeSibling.has_value()*/ else {
            return {};
        }
    } /*if this->geometry.at(rPoint) else*/

    return {};
}


template <concepts::Numeric TCoordinate,
          unsigned Dimension,
          concepts::UnsignedInteger TObjectIndex,
          class TAllocator>
FlatAABBoxTree<TCoordinate,Dimension,TObjectIndex,TAllocator>::FlatAABBoxTree(TAllocator&& rAllocator) noexcept
    : _data(),
      _allocator(std::move(rAllocator))
{
}


template <concepts::Numeric TCoordinate,
          unsigned Dimension,
          concepts::UnsignedInteger TObjectIndex,
          class TAllocator>
FlatAABBoxTree<TCoordinate,Dimension,TObjectIndex,TAllocator>::~FlatAABBoxTree()
{
    _allocator.deallocate(_data.data(), _data.size());
}


template <concepts::Numeric TCoordinate,
          unsigned Dimension,
          concepts::UnsignedInteger TObjectIndex,
          class TAllocator>
std::optional<Ptr<const typename FlatAABBoxTree<TCoordinate,Dimension,TObjectIndex,TAllocator>::Node>>
FlatAABBoxTree<TCoordinate,Dimension,TObjectIndex,TAllocator>::root() const noexcept
{
    if (_data.size()) {
        return static_cast<Ptr<const FlatAABBoxTree::Node>>(
            static_cast<Ptr<const void>>(_data.data())
        );
    } else {
        return {};
    }
}


template <concepts::Numeric TCoordinate,
          unsigned Dimension,
          concepts::UnsignedInteger TObjectIndex,
          class TAllocator>
std::optional<Ptr<typename FlatAABBoxTree<TCoordinate,Dimension,TObjectIndex,TAllocator>::Node>>
FlatAABBoxTree<TCoordinate,Dimension,TObjectIndex,TAllocator>::root() noexcept
{
    if (_data.size()) {
        return static_cast<Ptr<FlatAABBoxTree::Node>>(
            static_cast<Ptr<void>>(_data.data())
        );
    } else {
        return {};
    }
}


template <concepts::Numeric TCoordinate,
          unsigned Dimension,
          concepts::UnsignedInteger TObjectIndex,
          class TAllocator>
template <concepts::SamplableGeometry TObject,
          concepts::FunctionWithSignature<TObjectIndex,Ref<const TObject>> THasher>
requires (TObject::Dimension == Dimension && std::is_same_v<typename TObject::Coordinate,TCoordinate>)
FlatAABBoxTree<TCoordinate,Dimension,TObjectIndex,TAllocator>
FlatAABBoxTree<TCoordinate,Dimension,TObjectIndex,TAllocator>::flatten(Ref<const AABBoxNode<TObject>> rRoot,
                                                                       THasher&& rHasher,
                                                                       TAllocator&& rAllocator)
{
    FlatAABBoxTree flatTree(std::move(rAllocator));

    // First, find out how much memory the flattened tree requires.
    std::size_t byteCount = 0ul;
    {
        rRoot.visit([&byteCount] (Ptr<const AABBoxNode<TObject>> pNode) -> bool {
            byteCount += sizeof(typename FlatAABBoxTree::Node);
            byteCount += sizeof(TObjectIndex) * (pNode->contained().size() + pNode->intersected().size());
            return true;
        });

    }

    // Early exit if the tree is empty.
    if (byteCount) {
        // Allocate memory for the tree.
        flatTree._data = {flatTree._allocator.allocate(byteCount), byteCount};
        if (!flatTree._data.data()) {
            CIE_THROW(
                MemoryException,
                "Failed to allocate " << byteCount << " bytes while flattening a BVH."
            )
        }

        // Fill the flat tree.
        using NodeLevelPair = std::pair<
            Ptr<typename FlatAABBoxTree::Node>,     // <==  pointer to the previous node
            unsigned                                // <== level of the previous node
        >;
        std::stack<NodeLevelPair> trace;
        Ptr<typename FlatAABBoxTree::Node> pCurrent = flatTree.root().value();
        //trace.emplace(flatTree.root().value(), rRoot.level());

        rRoot.visit([&trace, &pCurrent, &rHasher] (Ptr<const AABBoxNode<TObject>> pNode) -> bool {
            Ref<typename FlatAABBoxTree::Node> rCurrent = *pCurrent;
            const unsigned currentLevel = pNode->level();

            // Fill unique data of the current node.
            rCurrent.geometry           = *static_cast<const typename FlatAABBoxTree::Node::Geometry*>(pNode);
            rCurrent.maybeSibling       = nullptr; // <== temporary init
            rCurrent.containedCount     = pNode->contained().size();
            rCurrent.intersectedCount   = pNode->intersected().size();

            {
                const auto objects          = pNode->contained();
                const auto objectIndices    = rCurrent.contained();
                for (unsigned iObject=0u; iObject<rCurrent.containedCount; ++iObject) {
                    objectIndices[iObject] = rHasher(*objects[iObject]);
                }
            }

            {
                const auto objects          = pNode->intersected();
                const auto objectIndices    = rCurrent.intersected();
                for (unsigned iObject=0u; iObject<rCurrent.intersectedCount; ++iObject) {
                    objectIndices[iObject] = rHasher(*objects[iObject]);
                }
            }

            // Link siblings and update the trace stack.
            if (!trace.empty()) {
                // The node at the top of the trace stack must either be
                // - the parent
                // - the youngest elder sibling
                // - the youngest child of the youngest elder sibling
                // of the current node, depending on its level.
                const unsigned previousLevel = trace.top().second;

                if (previousLevel < currentLevel) {
                    // The node at the top of the trace stack is the parent,
                    // so we don't know who its younger sibling is or whether
                    // it even has one, so an extra trace point has to be
                    // pushed to the trace stack.
                    trace.emplace(&rCurrent, currentLevel);
                } else if (previousLevel == currentLevel) {
                    // The node at the top of the trace stack is the youngest elder sibling.
                    // This is the only case where the sibling pointer is set retroactively.
                    trace.top().first->maybeSibling = &rCurrent;
                    trace.top().first = &rCurrent;
                } else {
                    // The node at the top of the trace stack is the
                    // youngest child of the youngest elder sibling.
                    // We know for sure that it does not have a
                    // younger sibling so it can be safely ejected
                    // from the trace stack.
                    do {trace.pop();} while (!trace.empty() && trace.top().second < currentLevel);
                }
            } else {
                trace.emplace(pCurrent, currentLevel);
            }

            pCurrent = rCurrent.next();
            return true;
        });
    } // if byteCount

    return flatTree;
}


} // namespace cie::geo
