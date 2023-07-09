#ifndef CIE_GEO_PARTITIONING_AABBOX_NODE_IMPL_HPP
#define CIE_GEO_PARTITIONING_AABBOX_NODE_IMPL_HPP

// --- Utility Includes ---
#include "packages/macros/inc/exceptions.hpp"
#include "packages/stl_extension/inc/resize.hpp"

// --- STL Includes ---
#include <algorithm>
#include <limits>


namespace cie::geo {


template <concepts::BoxBoundable ObjectType>
AABBoxNode<ObjectType>::AABBoxNode( const typename AABBoxNode<ObjectType>::Point& r_base,
                                    const typename AABBoxNode<ObjectType>::Point& r_lengths,
                                    typename AABBoxNode<ObjectType>::self_ptr p_parent ) :
    AABBoxNode<ObjectType>::CellBase( r_base, r_lengths ),
    AABBoxNode<ObjectType>::TreeBase(),
    _containedObjects(),
    _intersectedObjects(),
    _p_parent( p_parent )
{
    CIE_BEGIN_EXCEPTION_TRACING

    if ( auto p_tmp = _p_parent.lock() )
        this->_level = p_tmp->_level + 1;
    else
        this->_level = 0;

    CIE_END_EXCEPTION_TRACING
}


template <concepts::BoxBoundable ObjectType>
AABBoxNode<ObjectType>::AABBoxNode() :
    AABBoxNode<ObjectType>( detail::makeOrigin<ObjectType::bounding_box::Dimension,typename ObjectType::bounding_box::Coordinate>(),
                            detail::makeOrigin<ObjectType::bounding_box::Dimension,typename ObjectType::bounding_box::Coordinate>(),
                            typename AABBoxNode<ObjectType>::self_ptr() )
{
}


template <concepts::BoxBoundable ObjectType>
void
AABBoxNode<ObjectType>::addObject( typename AABBoxNode<ObjectType>::object_ptr p_object )
{
    auto nodeVisitFunction = [&p_object]( AABBoxNode<ObjectType>* p_node ) -> bool
    {
        CIE_BEGIN_EXCEPTION_TRACING

        if ( auto p_tmp = p_object.lock() )
        {
            if ( p_node->contains( boundingBox(*p_tmp) ) )
                p_node->_containedObjects.push_back( p_object );
            else if ( p_node->intersects( boundingBox(*p_tmp) ) )
                p_node->_intersectedObjects.push_back( p_object );
        }

        return true;

        CIE_END_EXCEPTION_TRACING
    };

    this->visit( nodeVisitFunction );
}


template <concepts::BoxBoundable ObjectType>
void
AABBoxNode<ObjectType>::eraseExpired()
{
    auto nodeVisitFunction = []( AABBoxNode<ObjectType>* p_node ) -> bool
    {
        std::erase_if( p_node->_containedObjects,
                       []( const auto& rp_object ) -> bool { return rp_object.expired(); } );
        std::erase_if( p_node->_intersectedObjects,
                       []( const auto& rp_object ) -> bool { return rp_object.expired(); } );
        return true;
    };
}


template <concepts::BoxBoundable ObjectType>
void
AABBoxNode<ObjectType>::shrink()
{
    auto nodeVisitFunction = []( AABBoxNode<ObjectType>* p_node ) -> bool
    {
        CIE_BEGIN_EXCEPTION_TRACING

        if ( p_node->_containedObjects.empty() && p_node->_intersectedObjects.empty() )
        {
            std::fill( p_node->_lengths.begin(),
                       p_node->_lengths.end(),
                       0 );
            return true;
        }

        typename AABBoxNode<ObjectType>::Point minPoint;
        utils::resize( minPoint, AABBoxNode<ObjectType>::Dimension );
        std::fill( minPoint.begin(),
                   minPoint.end(),
                   std::numeric_limits<typename AABBoxNode<ObjectType>::Coordinate>().max() );

        typename AABBoxNode<ObjectType>::Point maxPoint;
        utils::resize( maxPoint, AABBoxNode<ObjectType>::Dimension );
        std::fill( maxPoint.begin(),
                   maxPoint.end(),
                   std::numeric_limits<typename AABBoxNode<ObjectType>::Coordinate>().min() );

        for ( auto& rp_object : p_node->_containedObjects )
        {
            if ( auto p_tmp = rp_object.lock() )
            {
                const auto& r_box = boundingBox(*p_tmp);

                for ( Size dim=0; dim<AABBoxNode<ObjectType>::Dimension; ++dim )
                {
                    auto boxMax = r_box.base()[dim] + r_box.lengths()[dim];

                    if ( r_box.base()[dim] < minPoint[dim] )
                        minPoint[dim] = r_box.base()[dim];

                    if ( maxPoint[dim] < boxMax )
                        maxPoint[dim] = boxMax;
                } // for dim
            } // lock object
        } // for object in objects

        for ( Size dim=0; dim<AABBoxNode<ObjectType>::Dimension; ++dim )
        {
            p_node->_base[dim]    = minPoint[dim];
            p_node->_lengths[dim] = maxPoint[dim] - minPoint[dim];
        }

        return true;

        CIE_END_EXCEPTION_TRACING
    };

    this->visit( nodeVisitFunction );
}


template <concepts::BoxBoundable ObjectType>
typename AABBoxNode<ObjectType>::self_ptr
AABBoxNode<ObjectType>::find( const typename AABBoxNode<ObjectType>::object_ptr p_object )
{
    if ( p_object.expired() )
        CIE_THROW( Exception, "Attempt to find expired object in box tree" )

    auto p_objectInternal = p_object.lock();

    typename AABBoxNode<ObjectType>::self_ptr p_containingNode;

    auto nodeVisitFunction = [&p_objectInternal,&p_containingNode]( AABBoxNode<ObjectType>* p_node ) -> bool
    {
        CIE_BEGIN_EXCEPTION_TRACING

        bool hasObject = false;

        if ( std::find( p_node->_containedObjects.begin(),
                        p_node->_containedObjects.end(),
                        p_objectInternal )
             != p_node->_containedObjects.end() )
        { hasObject = true; }

        else if ( std::find( p_node->_intersectedObjects.begin(),
                             p_node->_intersectedObjects.end(),
                             p_objectInternal )
             != p_node->_intersectedObjects.end() )
        { hasObject = true; }

        // Terminate search if the node does not have the object
        // Continue the search if the node has the object but is not a leaf
        // Terminate the search if the node has the object and is a leaf
        if ( hasObject )
        {
            if ( p_node->isLeaf() )
            {
                p_containingNode = p_node->weak_from_this();
                return false;
            }
            else
                return true;
        }
        else
            return false;

        CIE_END_EXCEPTION_TRACING
    };

    // Check whether the object is in this box,
    // if it is not, there is no point in searching subcells
    if ( nodeVisitFunction(this) )
        this->visit( nodeVisitFunction );

    return p_containingNode;
}


template <concepts::BoxBoundable ObjectType>
bool
AABBoxNode<ObjectType>::partition( Size maxObjects,
                                   Size maxLevel )
{
    bool objectLimitReached = true;

    auto nodeVisitFunction = [=,&objectLimitReached]( AABBoxNode<ObjectType>* p_node ) -> bool
    {
        if ( auto p_parent = p_node->parent().lock() ) // Not root
        {
            p_node->_containedObjects.clear();
            p_node->_intersectedObjects.clear();

            // Pull contained objects from the parent
            for ( auto& rp_object : p_parent->_containedObjects )
                if ( auto p_tmp = rp_object.lock() )
                    if ( p_node->contains( boundingBox(*p_tmp) ) )
                        p_node->_containedObjects.push_back( rp_object );

            // Pull intersected objects from the parent
            for ( auto& rp_object : p_parent->_intersectedObjects )
                if ( auto p_tmp = rp_object.lock() )
                    if ( p_node->contains( boundingBox(*p_tmp) ) )
                        p_node->_intersectedObjects.push_back( rp_object );
        }

        // Stop partitioning if the number of objects is within the limit
        if ( p_node->_containedObjects.size() + p_node->_intersectedObjects.size() <= maxObjects )
            return true;

        // Stop partitioning if maximum recursion limit is reached
        if ( maxLevel <= p_node->_level )
        {
            objectLimitReached = false;
            return true;
        }

        // Get node midpoint to use as splitting point
        typename AABBoxNode<ObjectType>::Point midPoint;
        utils::resize( midPoint, AABBoxNode<ObjectType>::Dimension );
        for ( Size dim=0; dim<AABBoxNode<ObjectType>::Dimension; ++dim )
            midPoint[dim] = p_node->_base[dim] + p_node->_lengths[dim] / 2.0;

        // Subdivide node
        auto cellConstructors = p_node->split( midPoint );

        for ( const auto& r_cellConstructor : cellConstructors )
            p_node->_children.emplace_back( new AABBoxNode<ObjectType>(
                std::get<0>( r_cellConstructor ),   // base
                std::get<1>( r_cellConstructor ),   // lengths
                p_node->weak_from_this()            // parent
            ));

        return true;
    };

    this->visit( nodeVisitFunction );

    return objectLimitReached;
}


template <concepts::BoxBoundable ObjectType>
inline const typename AABBoxNode<ObjectType>::object_ptr_container&
AABBoxNode<ObjectType>::containedObjects() const
{
    return this->_containedObjects;
}


template <concepts::BoxBoundable ObjectType>
inline const typename AABBoxNode<ObjectType>::object_ptr_container&
AABBoxNode<ObjectType>::intersectedObjects() const
{
    return this->_intersectedObjects;
}


template <concepts::BoxBoundable ObjectType>
inline typename AABBoxNode<ObjectType>::self_ptr
AABBoxNode<ObjectType>::parent()
{
    return this->_p_parent;
}


template <concepts::BoxBoundable ObjectType>
inline const typename AABBoxNode<ObjectType>::self_ptr&
AABBoxNode<ObjectType>::parent() const
{
    return this->_p_parent;
}


} // namespace cie::geo

#endif