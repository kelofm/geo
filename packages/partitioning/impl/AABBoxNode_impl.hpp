#pragma once

// help the language server
#include "packages/partitioning/inc/AABBoxNode.hpp"

// --- Utility Includes ---
#include "packages/macros/inc/exceptions.hpp"
#include "packages/stl_extension/inc/resize.hpp"

// --- STL Includes ---
#include <algorithm>
#include <limits>


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
void AABBoxNode<TObject>::insert(Ptr<ObjectType> pObject)
{
    auto visitor = [&pObject](AABBoxNode* pNode) -> bool {
        if (pNode->contains(boundingBox(*pObject)))
            pNode->_containedObjects.push_back(pObject);
        else if (pNode->intersects(boundingBox(*pObject)))
            pNode->_intersectedObjects.push_back(pObject);
        else
            return false;
        return true;
    };

    this->visit(visitor);
}


template <concepts::BoxBoundable TObject>
void AABBoxNode<TObject>::shrink()
{
    auto visitor = [](AABBoxNode* pNode) -> bool {
        if (pNode->_containedObjects.empty() && pNode->_intersectedObjects.empty()) {
            std::fill(pNode->_lengths.begin(),
                      pNode->_lengths.end(),
                      0);
            return true;
        }

        typename AABBoxNode::Point minPoint;
        utils::resize(minPoint, AABBoxNode::Dimension);
        std::fill(minPoint.begin(),
                  minPoint.end(),
                  std::numeric_limits<typename AABBoxNode::Coordinate>().max());

        typename AABBoxNode::Point maxPoint;
        utils::resize(maxPoint, AABBoxNode::Dimension);
        std::fill(maxPoint.begin(),
                  maxPoint.end(),
                  std::numeric_limits<typename AABBoxNode::Coordinate>().min());

        for (auto pObject : pNode->_containedObjects) {
            if (pObject) {
                const auto& r_box = boundingBox(*pObject);

                for (Size dim=0; dim<AABBoxNode::Dimension; ++dim) {
                    auto boxMax = r_box.base()[dim] + r_box.lengths()[dim];

                    if (r_box.base()[dim] < minPoint[dim])
                        minPoint[dim] = r_box.base()[dim];

                    if (maxPoint[dim] < boxMax)
                        maxPoint[dim] = boxMax;
                } // for dim
            }
        } // for object in objects

        for (Size dim=0; dim<AABBoxNode::Dimension; ++dim) {
            pNode->_base[dim]    = minPoint[dim];
            pNode->_lengths[dim] = maxPoint[dim] - minPoint[dim];
        }

        return true;
    };

    this->visit(visitor);
}


template <concepts::BoxBoundable TObject>
AABBoxNode<TObject>* AABBoxNode<TObject>::find(Ptr<ObjectType> pObject)
{
    AABBoxNode* pContainingNode = nullptr;

    auto visitor = [&pObject, &pContainingNode](AABBoxNode* pNode) -> bool {
        CIE_BEGIN_EXCEPTION_TRACING

        bool hasObject = false;

        if (std::find(pNode->_containedObjects.begin(),
                      pNode->_containedObjects.end(),
                      pObject)
            != pNode->_containedObjects.end()) {
            hasObject = true;
        } else if (std::find(pNode->_intersectedObjects.begin(),
                             pNode->_intersectedObjects.end(),
                             pObject)
                   != pNode->_intersectedObjects.end())
        { hasObject = true; }

        // Terminate search if the node does not have the object
        // Continue the search if the node has the object but is not a leaf
        // Terminate the search if the node has the object and is a leaf
        if (hasObject) {
            if (pNode->isLeaf()) {
                pContainingNode = pNode;
                return false;
            } else
                return true;
        } else
            return false;

        CIE_END_EXCEPTION_TRACING
    };

    // Check whether the object is in this box,
    // if it is not, there is no point in searching subcells
    if (visitor(this))
        this->visit(visitor);

    return pContainingNode;
}


template <concepts::BoxBoundable TObject>
Ptr<AABBoxNode<TObject>> AABBoxNode<TObject>::find(Ref<const Point> rPoint)
{
    AABBoxNode* pContainingNode = nullptr;

    const auto visitor = [&rPoint, &pContainingNode](AABBoxNode* pNode) noexcept -> bool {
        if (pNode->at(rPoint)) {
            if (pNode->isLeaf()) {
                pContainingNode = pNode;
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

            // Pull contained objects from the parent
            for (auto pObject : pParent->_containedObjects)
                if (pObject) {
                    if (pNode->contains(boundingBox(*pObject))) {
                        pNode->_containedObjects.push_back(pObject);
                    } else if (pNode->intersects(boundingBox(*pObject))) {
                        pNode->_intersectedObjects.push_back(pObject);
                    }
                }

            // Pull intersected objects from the parent
            for (auto pObject : pParent->_intersectedObjects)
                if (pObject)
                    if (pNode->intersects(boundingBox(*pObject)))
                        pNode->_intersectedObjects.push_back(pObject);
        }

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

        for (const auto& r_cellConstructor : cellConstructors)
            pNode->_children.emplace_back(new AABBoxNode<TObject>(
                std::get<0>(r_cellConstructor),   // base
                std::get<1>(r_cellConstructor),   // lengths
                pNode                               // parent
           ));

        return true;
    };

    this->visit(visitor);

    return objectLimitReached;
}


template <concepts::BoxBoundable TObject>
const typename AABBoxNode<TObject>::ObjectPtrContainer&
AABBoxNode<TObject>::containedObjects() const noexcept
{
    return this->_containedObjects;
}


template <concepts::BoxBoundable TObject>
const typename AABBoxNode<TObject>::ObjectPtrContainer&
AABBoxNode<TObject>::intersectedObjects() const noexcept
{
    return this->_intersectedObjects;
}


template <concepts::BoxBoundable TObject>
AABBoxNode<TObject>* AABBoxNode<TObject>::parent()
{
    return this->_pParent;
}


template <concepts::BoxBoundable TObject>
const AABBoxNode<TObject>* AABBoxNode<TObject>::parent() const
{
    return this->_pParent;
}


} // namespace cie::geo
