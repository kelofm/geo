#pragma once

// help the language server
#include "packages/partitioning/inc/AABBoxNode.hpp"

// --- Utility Includes ---
#include "packages/macros/inc/exceptions.hpp"
#include "packages/stl_extension/inc/resize.hpp"

// --- STL Includes ---
#include <algorithm>
#include <limits>
#include <packages/trees/inc/abstree.hpp>


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
    AABBoxNode* pContainingNode = nullptr;

    auto visitor = [&pObject, &pContainingNode](AABBoxNode* pNode) -> bool {
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
Ptr<AABBoxNode<TObject>> AABBoxNode<TObject>::find(Ref<const Point> rPoint)
{
    AABBoxNode* pContainingNode = nullptr;
    Size containingLevel = 0ul;

    const auto visitor = [&rPoint, &pContainingNode, &containingLevel](AABBoxNode* pNode) noexcept -> bool {
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
std::span<Ptr<const TObject>>
AABBoxNode<TObject>::contained() const noexcept
{
    return this->_containedObjects;
}


template <concepts::BoxBoundable TObject>
std::span<Ptr<TObject>>
AABBoxNode<TObject>::contained() noexcept
{
    return this->_containedObjects;
}


template <concepts::BoxBoundable TObject>
std::span<Ptr<const TObject>>
AABBoxNode<TObject>::intersected() const noexcept
{
    return this->_intersectedObjects;
}


template <concepts::BoxBoundable TObject>
std::span<Ptr<TObject>>
AABBoxNode<TObject>::intersected() noexcept
{
    return this->_intersectedObjects;
}


template <concepts::BoxBoundable TObject>
Ptr<AABBoxNode<TObject>> AABBoxNode<TObject>::parent() noexcept
{
    return this->_pParent;
}


template <concepts::BoxBoundable TObject>
Ptr<const AABBoxNode<TObject>> AABBoxNode<TObject>::parent() const noexcept
{
    return this->_pParent;
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


} // namespace cie::geo
