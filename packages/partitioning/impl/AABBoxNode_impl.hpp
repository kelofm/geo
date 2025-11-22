#pragma once

// help the language server
#include "packages/partitioning/inc/AABBoxNode.hpp"

// --- Utility Includes ---
#include "packages/macros/inc/exceptions.hpp"
#include "packages/stl_extension/inc/resize.hpp"
#include "packages/trees/inc/abstree.hpp"
#include "packages/exceptions/inc/exception.hpp"

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

            using TCoordinate = typename AABBoxNode::Point::value_type;
            constexpr unsigned Dimension = AABBoxNode::Dimension;
            //StaticArray<typename AABBoxNode::Point,intPow(2, Dimension)> corners;
            StaticArray<TCoordinate,Dimension*intPow(2,Dimension)> corners;
            pNode->makeCorners(
                std::span<TCoordinate,Dimension>(pNode->base().data(), Dimension),
                std::span<TCoordinate,Dimension>(pNode->lengths().data(), Dimension),
                std::span<TCoordinate,Dimension*intPow(2,Dimension)>(corners.data(), corners.size()));

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
                            constexpr unsigned cornerCount = intPow(2,Dimension);
                            for (unsigned iCorner=0u; iCorner<cornerCount; ++iCorner) {
                                StaticArray<TCoordinate,Dimension> corner;
                                std::copy_n(corners.data() + iCorner * Dimension,
                                            Dimension,
                                            corner.data());
                                if (pObject->at(corner)) hasCornerInside = true;
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
          concepts::Allocator<std::byte> TAllocator>
template <bool TMutable>
std::span<const TCoordinate,Dimension>
FlatAABBoxTree<TCoordinate,Dimension,TObjectIndex,TAllocator>::Node<TMutable>::base() const noexcept
{
    Ptr<const TCoordinate> pBegin = static_cast<Ptr<const TCoordinate>>(static_cast<Ptr<const void>>(_data));
    return std::span<const TCoordinate,Dimension>(
        pBegin,
        Dimension
    );
}


template <concepts::Numeric TCoordinate,
          unsigned Dimension,
          concepts::UnsignedInteger TObjectIndex,
          concepts::Allocator<std::byte> TAllocator>
template <bool TMutable>
std::span<TCoordinate,Dimension>
FlatAABBoxTree<TCoordinate,Dimension,TObjectIndex,TAllocator>::Node<TMutable>::base() noexcept
requires TMutable
{
    Ptr<TCoordinate> pBegin = static_cast<Ptr<TCoordinate>>(static_cast<Ptr<void>>(_data));
    return std::span<TCoordinate,Dimension>(
        pBegin,
        Dimension
    );
}


template <concepts::Numeric TCoordinate,
          unsigned Dimension,
          concepts::UnsignedInteger TObjectIndex,
          concepts::Allocator<std::byte> TAllocator>
template <bool TMutable>
std::span<const TCoordinate,Dimension>
FlatAABBoxTree<TCoordinate,Dimension,TObjectIndex,TAllocator>::Node<TMutable>::lengths() const noexcept
{
    const auto base = this->base();
    return std::span<const TCoordinate,Dimension>(
        base.data() + base.size(),
        Dimension
    );
}


template <concepts::Numeric TCoordinate,
          unsigned Dimension,
          concepts::UnsignedInteger TObjectIndex,
          concepts::Allocator<std::byte> TAllocator>
template <bool TMutable>
std::span<TCoordinate,Dimension>
FlatAABBoxTree<TCoordinate,Dimension,TObjectIndex,TAllocator>::Node<TMutable>::lengths() noexcept
requires TMutable
{
    const auto base = this->base();
    return std::span<TCoordinate,Dimension>(
        base.data() + base.size(),
        Dimension
    );
}


template <concepts::Numeric TCoordinate,
          unsigned Dimension,
          concepts::UnsignedInteger TObjectIndex,
          concepts::Allocator<std::byte> TAllocator>
template <bool TMutable>
std::size_t
FlatAABBoxTree<TCoordinate,Dimension,TObjectIndex,TAllocator>::Node<TMutable>::iSibling() const noexcept
{
    const auto lengths = this->lengths();
    Ptr<const TCoordinate> pLengthEnd = lengths.data() + lengths.size();
    Ptr<const std::size_t> pSiblingBegin = static_cast<Ptr<const std::size_t>>(
        static_cast<Ptr<const void>>(pLengthEnd)
    );
    return *pSiblingBegin;
}


template <concepts::Numeric TCoordinate,
          unsigned Dimension,
          concepts::UnsignedInteger TObjectIndex,
          concepts::Allocator<std::byte> TAllocator>
template <bool TMutable>
Ref<std::size_t>
FlatAABBoxTree<TCoordinate,Dimension,TObjectIndex,TAllocator>::Node<TMutable>::iSibling() noexcept
requires TMutable
{
    const auto lengths = this->lengths();
    Ptr<TCoordinate> pLengthEnd = lengths.data() + lengths.size();
    Ptr<std::size_t> pSiblingBegin = static_cast<Ptr<std::size_t>>(
        static_cast<Ptr<void>>(pLengthEnd)
    );
    return *pSiblingBegin;
}


template <concepts::Numeric TCoordinate,
          unsigned Dimension,
          concepts::UnsignedInteger TObjectIndex,
          concepts::Allocator<std::byte> TAllocator>
template <bool TMutable>
unsigned
FlatAABBoxTree<TCoordinate,Dimension,TObjectIndex,TAllocator>::Node<TMutable>::containedCount() const noexcept
{
    const auto lengths = this->lengths();
    Ptr<const std::byte> pLengthEnd = static_cast<Ptr<const std::byte>>(
        static_cast<Ptr<const void>>(
            lengths.data() + lengths.size()
        )
    );
    Ptr<const std::byte> pContainedCount = pLengthEnd + sizeof(std::size_t);
    return *static_cast<Ptr<const unsigned>>(
        static_cast<Ptr<const void>>(
            pContainedCount
        )
    );
}


template <concepts::Numeric TCoordinate,
          unsigned Dimension,
          concepts::UnsignedInteger TObjectIndex,
          concepts::Allocator<std::byte> TAllocator>
template <bool TMutable>
Ref<unsigned>
FlatAABBoxTree<TCoordinate,Dimension,TObjectIndex,TAllocator>::Node<TMutable>::containedCount() noexcept
requires TMutable
{
    const auto lengths = this->lengths();
    Ptr<std::byte> pLengthEnd = static_cast<Ptr<std::byte>>(
        static_cast<Ptr<void>>(
            lengths.data() + lengths.size()
        )
    );
    Ptr<std::byte> pContainedCount = pLengthEnd + sizeof(std::size_t);
    return *static_cast<Ptr<unsigned>>(
        static_cast<Ptr<void>>(
            pContainedCount
        )
    );
}


template <concepts::Numeric TCoordinate,
          unsigned Dimension,
          concepts::UnsignedInteger TObjectIndex,
          concepts::Allocator<std::byte> TAllocator>
template <bool TMutable>
unsigned
FlatAABBoxTree<TCoordinate,Dimension,TObjectIndex,TAllocator>::Node<TMutable>::intersectedCount() const noexcept
{
    const auto lengths = this->lengths();
    Ptr<const std::byte> pLengthEnd = static_cast<Ptr<const std::byte>>(
        static_cast<Ptr<const void>>(
            lengths.data() + lengths.size()
        )
    );
    Ptr<const std::byte> pIntersectedCount = pLengthEnd + sizeof(std::size_t) + sizeof(unsigned);
    return *static_cast<Ptr<const unsigned>>(
        static_cast<Ptr<const void>>(
            pIntersectedCount
        )
    );
}


template <concepts::Numeric TCoordinate,
          unsigned Dimension,
          concepts::UnsignedInteger TObjectIndex,
          concepts::Allocator<std::byte> TAllocator>
template <bool TMutable>
Ref<unsigned>
FlatAABBoxTree<TCoordinate,Dimension,TObjectIndex,TAllocator>::Node<TMutable>::intersectedCount() noexcept
requires TMutable
{
    const auto lengths = this->lengths();
    Ptr<std::byte> pLengthEnd = static_cast<Ptr<std::byte>>(
        static_cast<Ptr<void>>(
            lengths.data() + lengths.size()
        )
    );
    Ptr<std::byte> pIntersectedCount = pLengthEnd + sizeof(std::size_t) + sizeof(unsigned);
    return *static_cast<Ptr<unsigned>>(
        static_cast<Ptr<void>>(
            pIntersectedCount
        )
    );
}


template <concepts::Numeric TCoordinate,
          unsigned Dimension,
          concepts::UnsignedInteger TObjectIndex,
          concepts::Allocator<std::byte> TAllocator>
template <bool TMutable>
std::span<const TObjectIndex>
FlatAABBoxTree<TCoordinate,Dimension,TObjectIndex,TAllocator>::Node<TMutable>::contained() const noexcept
{
    const auto lengths = this->lengths();
    Ptr<const std::byte> pLengthEnd = static_cast<Ptr<const std::byte>>(
        static_cast<Ptr<const void>>(
            lengths.data() + lengths.size()
        )
    );
    Ptr<const std::byte> pContainedBegin = pLengthEnd + sizeof(std::size_t) + 2 * sizeof(unsigned);
    return {
        static_cast<Ptr<const TObjectIndex>>(
            static_cast<Ptr<const void>>(
                pContainedBegin
            )
        ),
        this->containedCount()
    };
}


template <concepts::Numeric TCoordinate,
          unsigned Dimension,
          concepts::UnsignedInteger TObjectIndex,
          concepts::Allocator<std::byte> TAllocator>
template <bool TMutable>
std::span<TObjectIndex>
FlatAABBoxTree<TCoordinate,Dimension,TObjectIndex,TAllocator>::Node<TMutable>::contained() noexcept
requires TMutable
{
    const auto lengths = this->lengths();
    Ptr<std::byte> pLengthEnd = static_cast<Ptr<std::byte>>(
        static_cast<Ptr<void>>(
            lengths.data() + lengths.size()
        )
    );
    Ptr<std::byte> pContainedBegin = pLengthEnd + sizeof(std::size_t) + 2 * sizeof(unsigned);
    return {
        static_cast<Ptr<TObjectIndex>>(
            static_cast<Ptr<void>>(
                pContainedBegin
            )
        ),
        this->containedCount()
    };
}


template <concepts::Numeric TCoordinate,
          unsigned Dimension,
          concepts::UnsignedInteger TObjectIndex,
          concepts::Allocator<std::byte> TAllocator>
template <bool TMutable>
std::span<const TObjectIndex>
FlatAABBoxTree<TCoordinate,Dimension,TObjectIndex,TAllocator>::Node<TMutable>::intersected() const noexcept
{
    const auto contained = this->contained();
    return {
        contained.data() + contained.size(),
        this->intersectedCount()
    };
}


template <concepts::Numeric TCoordinate,
          unsigned Dimension,
          concepts::UnsignedInteger TObjectIndex,
          concepts::Allocator<std::byte> TAllocator>
template <bool TMutable>
std::span<TObjectIndex>
FlatAABBoxTree<TCoordinate,Dimension,TObjectIndex,TAllocator>::Node<TMutable>::intersected() noexcept
requires TMutable
{
    const auto contained = this->contained();
    return {
        contained.data() + contained.size(),
        this->intersectedCount()
    };
}


template <concepts::Numeric TCoordinate,
          unsigned Dimension,
          concepts::UnsignedInteger TObjectIndex,
          concepts::Allocator<std::byte> TAllocator>
template <bool TMutable>
typename FlatAABBoxTree<TCoordinate,Dimension,TObjectIndex,TAllocator>::template Node<false>
FlatAABBoxTree<TCoordinate,Dimension,TObjectIndex,TAllocator>::Node<TMutable>::next() const noexcept
{
    const auto intersected = this->intersected();
    const auto pNext = static_cast<Ptr<const std::byte>>(
        static_cast<Ptr<const void>>(
            intersected.data() + intersected.size()
        )
    );
    return typename FlatAABBoxTree::template Node<false> {pNext};
}


template <concepts::Numeric TCoordinate,
          unsigned Dimension,
          concepts::UnsignedInteger TObjectIndex,
          concepts::Allocator<std::byte> TAllocator>
template <bool TMutable>
typename FlatAABBoxTree<TCoordinate,Dimension,TObjectIndex,TAllocator>::template Node<true>
FlatAABBoxTree<TCoordinate,Dimension,TObjectIndex,TAllocator>::Node<TMutable>::next() noexcept
requires TMutable
{
    const auto intersected = this->intersected();
    const auto pNext = static_cast<Ptr<std::byte>>(
        static_cast<Ptr<void>>(
            intersected.data() + intersected.size()
        )
    );
    return typename FlatAABBoxTree::template Node<true>(pNext);
}


template <concepts::Numeric TCoordinate,
          unsigned Dimension,
          concepts::UnsignedInteger TObjectIndex,
          concepts::Allocator<std::byte> TAllocator>
template <bool TMutable>
unsigned
FlatAABBoxTree<TCoordinate,Dimension,TObjectIndex,TAllocator>::Node<TMutable>::size() const noexcept
{
    return   this->staticSize()
           + this->containedCount() * sizeof(TObjectIndex)
           + this->intersectedCount() * sizeof(TObjectIndex)
           ;
}


template <concepts::Numeric TCoordinate,
          unsigned Dimension,
          concepts::UnsignedInteger TObjectIndex,
          concepts::Allocator<std::byte> TAllocator>
template <bool TMutable>
template <concepts::SamplableGeometry TObject>
std::optional<std::size_t>
FlatAABBoxTree<TCoordinate,Dimension,TObjectIndex,TAllocator>::Node<TMutable>::find(Ref<const typename Geometry::Point> rPoint,
                                                                                    Ref<const std::span<const TObject>> rObjects) const noexcept
{
    //std::cout << "node at " << std::distance(rData.data(), this->_data) << ":\n"
    //          << "\tbase            : " << this->base()[0] << " " << this->base()[1] << "\n"
    //          << "\tlengths         : " << this->lengths()[0] << " " << this->lengths()[1] << "\n"
    //          << "\tsibling         : " << this->iSibling() << "\n"
    //          << "\tcontained       : [" << this->containedCount() << "] "; for (auto c : this->contained()) {std::cout << c << " ";} std::cout << "\n"
    //          << "\tintersected     : [" << this->intersectedCount() << "] "; for (auto c : this->intersected()) {std::cout << c << " ";} std::cout << "\n"
    //          << "\tpoint " << (inNode ? "in" : "not in") << " node\n"
    //          ;

    for (const auto iObject : this->contained()) {
        Ref<const TObject> rObject = rObjects[iObject];
        const bool inObject = rObject.at(rPoint);
        if (inObject) return iObject;
    } // for iObject in this->contained()

    for (const auto iObject : this->intersected()) {
        Ref<const TObject> rObject = rObjects[iObject];
        const bool inObject = rObject.at(rPoint);
        if (inObject) return iObject;
    } // for iObject in this->intersected()

    return {};
}


template <concepts::Numeric TCoordinate,
          unsigned Dimension,
          concepts::UnsignedInteger TObjectIndex,
          concepts::Allocator<std::byte> TAllocator>
FlatAABBoxTree<TCoordinate,Dimension,TObjectIndex,TAllocator>::FlatAABBoxTree(TAllocator&& rAllocator) noexcept
    : _data(),
      _allocator(std::move(rAllocator))
{
}


template <concepts::Numeric TCoordinate,
          unsigned Dimension,
          concepts::UnsignedInteger TObjectIndex,
          concepts::Allocator<std::byte> TAllocator>
FlatAABBoxTree<TCoordinate,Dimension,TObjectIndex,TAllocator>::~FlatAABBoxTree()
{
    _allocator.deallocate(_data.data(), _data.size());
}


template <concepts::Numeric TCoordinate,
          unsigned Dimension,
          concepts::UnsignedInteger TObjectIndex,
          concepts::Allocator<std::byte> TAllocator>
std::optional<typename FlatAABBoxTree<TCoordinate,Dimension,TObjectIndex,TAllocator>::template Node</*TMutable=*/false>>
FlatAABBoxTree<TCoordinate,Dimension,TObjectIndex,TAllocator>::root() const noexcept
{
    if (!_data.empty()) {
        return {typename FlatAABBoxTree::template Node<false>(
            _data.data()
        )};
    } else {
        return {};
    }
}


template <concepts::Numeric TCoordinate,
          unsigned Dimension,
          concepts::UnsignedInteger TObjectIndex,
          concepts::Allocator<std::byte> TAllocator>
std::optional<typename FlatAABBoxTree<TCoordinate,Dimension,TObjectIndex,TAllocator>::template Node</*TMutable=*/true>>
FlatAABBoxTree<TCoordinate,Dimension,TObjectIndex,TAllocator>::root() noexcept
{
    if (!_data.empty()) {
        return {typename FlatAABBoxTree::template Node<true>(
            _data.data()
        )};
    } else {
        return {};
    }
}


template <concepts::Numeric TCoordinate,
          unsigned Dimension,
          concepts::UnsignedInteger TObjectIndex,
          concepts::Allocator<std::byte> TAllocator>
template <concepts::SamplableGeometry TObject>
std::optional<std::size_t>
FlatAABBoxTree<TCoordinate,Dimension,TObjectIndex,TAllocator>::find(Ref<const std::span<const TCoordinate,Dimension>> rPoint,
                                                                    Ref<const std::span<const TObject>> rObjects) const noexcept
{
    auto maybeRoot = this->root();
    if (!maybeRoot.has_value()) {
        return {};
    } else {
        // Copy point data to a type the objects expect.
        using Geometry = typename FlatAABBoxTree::template Node<false>::Geometry;
        typename Geometry::Point point;
        std::copy_n(rPoint.data(), Dimension, point.data());
        const auto pDataEnd = _data.data() + _data.size();

        // Avoid recursion. Search at most TMaxLevels levels.
        std::optional<std::size_t> output;
        std::optional<typename FlatAABBoxTree::template Node<false>> maybeNode = maybeRoot;

        // Search the tree depth-first in a stack-unrolled recursive manner.
        do {
            const auto node = maybeNode.value();
            maybeNode.reset();

            // Check whether the point lies within the node.
            const bool inNode = Geometry::at(rPoint.data(),
                                             node.base().data(),
                                             node.lengths().data());

            if (inNode) {
                // The point lies within the current node.
                // => search it.
                output = node.find(point, rObjects);
                if (output.has_value()) {
                    // The point was found in one of the objects.
                    break;
                } /*if output.has_value()*/ else {
                    // The point was not found in any of the objects.
                    // Search the child if this node has one, otherwise
                    // no object contains the provided point.
                    const auto next = node.next();
                    if (next._data < pDataEnd) {
                        const auto iSibling = node.iSibling();
                        if (iSibling == 0ul || _data.data() + iSibling != next._data) {
                            maybeNode.emplace(next._data);
                        } // is this node has a child
                    } // if next exists
                }/*if output.has_value() else*/
            } /*if inNode*/ else {
                // The point does not lie within the current node.
                // => search the sibling (if this node has one)
                const std::size_t iSibling = node.iSibling();
                if (iSibling != 0ul) {
                    const auto pSibling = _data.data() + iSibling;
                    maybeNode.emplace(pSibling);
                }
            } /*if inNode else*/
        } while (maybeNode.has_value());

        return output;
    }
}


template <concepts::Numeric TCoordinate,
          unsigned Dimension,
          concepts::UnsignedInteger TObjectIndex,
          concepts::Allocator<std::byte> TAllocator>
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
            byteCount += Node<false>::staticSize();
            byteCount += sizeof(TObjectIndex) * (pNode->contained().size() + pNode->intersected().size());
            return true;
        });

    }

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
            typename FlatAABBoxTree::Node<true>,    // <==  pointer to the previous node
            unsigned                                // <== level of the previous node
        >;
        std::stack<NodeLevelPair> trace;
        typename FlatAABBoxTree::Node<true> current = flatTree.root().value();

        rRoot.visit([&trace, &current, &flatTree, &rHasher] (Ptr<const AABBoxNode<TObject>> pNode) -> bool {
            const unsigned currentLevel = pNode->level();

            // Fill unique data of the current node.
            std::copy_n(pNode->base().data(),
                        Dimension,
                        current.base().data());
            std::copy_n(pNode->lengths().data(),
                        Dimension,
                        current.lengths().data());
            current.iSibling()          = 0ul; // <== temporary init
            current.containedCount()    = pNode->contained().size();
            current.intersectedCount()  = pNode->intersected().size();

            {
                const auto objects          = pNode->contained();
                const auto objectIndices    = current.contained();
                for (unsigned iObject=0u; iObject<objects.size(); ++iObject) {
                    objectIndices[iObject] = rHasher(*objects[iObject]);
                }
            }

            {
                const auto objects          = pNode->intersected();
                const auto objectIndices    = current.intersected();
                for (unsigned iObject=0u; iObject<objects.size(); ++iObject) {
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
                    trace.emplace(current, currentLevel);
                } else if (previousLevel == currentLevel) {
                    // The node at the top of the trace stack is the youngest elder sibling.
                    // This is the only case where the sibling pointer is set retroactively.
                    trace.top().first.iSibling() = std::distance(flatTree._data.data(), current._data);
                    trace.top().first = current;
                } else {
                    // The node at the top of the trace stack is the
                    // youngest child of the youngest elder sibling.
                    // We know for sure that it does not have a
                    // younger sibling so it can be safely ejected
                    // from the trace stack.
                    do {trace.pop();} while (!trace.empty() && currentLevel < trace.top().second);
                    if (!trace.empty() && trace.top().second == currentLevel) {
                        trace.top().first.iSibling() = std::distance(flatTree._data.data(), current._data);
                        trace.top().first = current;
                    }
                }
            } /*if !trace.empty()*/ else {
                trace.emplace(current, currentLevel);
            } /*if !trace.empty() else*/

            current = current.next();
            return true;
        }, utils::VisitStrategy::DepthFirst);
    } // if byteCount

    return flatTree;
}


} // namespace cie::geo
