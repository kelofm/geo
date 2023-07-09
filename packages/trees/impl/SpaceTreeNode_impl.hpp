#ifndef CIE_GEO_SPACETREENODE_IMPL_HPP
#define CIE_GEO_SPACETREENODE_IMPL_HPP

// --- Utility Includes ---
#include "packages/concurrency/inc/ThreadPoolSingleton.hpp"
#include "packages/macros/inc/checks.hpp"
#include "packages/macros/inc/exceptions.hpp"
#include "packages/stl_extension/inc/resize.hpp"
#include "packages/stl_extension/inc/make_shared_from_tuple.hpp"
#include "cmake_variables.hpp"

// --- GEO Includes ---
#include "packages/trees/inc/SpaceTreeNode.hpp"

// --- STL Includes ---
#include <tuple>


namespace cie::geo {


template <class TCell, class TValue>
template <class ...Args>
SpaceTreeNode<TCell,TValue>::SpaceTreeNode(const typename SpaceTreeNode<TCell,TValue>::sampler_ptr& p_sampler,
                                           const typename SpaceTreeNode<TCell,TValue>::split_policy_ptr& p_splitPolicy,
                                           Size level,
                                           Args&&... r_args) :
    SpaceTreeNode<TCell,TValue>::Base(level),
    TCell(std::forward<Args>(r_args)...),
    _p_splitPolicy(p_splitPolicy),
    _p_sampler(p_sampler),
    _isBoundary(detail::BoundaryTag::Null)
{
}


template <class TCell, class TValue>
inline bool
SpaceTreeNode<TCell,TValue>::divide(Ref<const Target> r_target,
                                    Size level)
{
    CIE_BEGIN_EXCEPTION_TRACING

    auto pool = mp::ThreadPoolSingleton::get();
    bool result = this->divide(
        r_target,
        level,
        pool
    );
    pool.barrier();

    return result;

    CIE_END_EXCEPTION_TRACING
}


template <class TCell, class TValue>
template <concepts::ThreadPool TPool>
inline bool
SpaceTreeNode<TCell,TValue>::divide(const typename SpaceTreeNode<TCell,TValue>::Target& r_target,
                                    Size level,
                                    TPool& r_threadPool)
{
    CIE_BEGIN_EXCEPTION_TRACING

    bool result = this->divideImpl(
        r_target,
        level,
        r_threadPool
    );
    r_threadPool.barrier();

    return result;

    CIE_END_EXCEPTION_TRACING
}


template <class TCell, class TValue>
template <concepts::CallableWith<const SpaceTreeNode<TCell,TValue>&> TFunction>
inline void
SpaceTreeNode<TCell,TValue>::scan(const Target& r_target, const TFunction& r_function, const Size level)
{
    auto pool = mp::ThreadPoolSingleton::get();
    this->scan(r_target, r_function, level, pool);
}


template <class TCell, class TValue>
template <concepts::CallableWith<const SpaceTreeNode<TCell,TValue>&> TFunction, concepts::ThreadPool TPool>
inline void
SpaceTreeNode<TCell,TValue>::scan(const Target& r_target, const TFunction& r_function, const Size level, TPool& r_pool)
{
    this->scanImpl(r_target, r_function, level, r_pool);
    r_pool.barrier();
}


template <class TCell, class TValue>
template <concepts::CallableWith<const SpaceTreeNode<TCell,TValue>&> TFunction, concepts::ThreadPool TPool>
void
SpaceTreeNode<TCell,TValue>::scanImpl(const Target& r_target, const TFunction& r_function, const Size level, TPool& r_pool)
{
    CIE_BEGIN_EXCEPTION_TRACING
    CIE_PROFILE_SCOPE

    if (level > this->level())
    {
        // Evaluate target and set the boundary flag
        value_container_type values;
        this->evaluate(r_target, values);

        // Split if boundary
        if (this->isBoundary())
        {
            const auto splitPoint = _p_splitPolicy->operator()(
                values.begin(),
                values.end(),
                SamplePointIterator(0, this)
            );

            auto nodeConstructor = std::make_tuple(_p_sampler, _p_splitPolicy, this->level() + 1);
            auto cellConstructors = this->split(splitPoint);

            r_pool.queueJob([nodeConstructor = std::move(nodeConstructor),
                             cellConstructors = std::move(cellConstructors),
                             &r_function,
                             &r_target,
                             level,
                             &r_pool]()
            {
                for (const auto& r_cellConstructor : cellConstructors)
                {
                    const auto compoundConstructor = std::tuple_cat(nodeConstructor, r_cellConstructor);
                    auto node = std::make_from_tuple<SpaceTreeNode>(compoundConstructor);
                    node.scanImpl(r_target, r_function, level, r_pool);
                }
            });
        }
    }
    else
        this->evaluate(r_target);

    r_function(*this);

    CIE_END_EXCEPTION_TRACING
}


template <class TCell, class TValue>
inline void
SpaceTreeNode<TCell,TValue>::evaluate(const typename SpaceTreeNode<TCell,TValue>::Target& r_target)
{
    CIE_BEGIN_EXCEPTION_TRACING
    CIE_PROFILE_SCOPE

    // Init
    _isBoundary = detail::BoundaryTag::Null;
    SamplePointIterator it_point(0,this);

    // Evaluate first point separately to set the sign flag
    const bool isFirstValuePositive = 0 < r_target(*it_point);
    ++it_point;

    // Evaluate the rest of the points
    for (Size i=1; i<_p_sampler->size(); ++i)
    {
        const auto value = r_target(*it_point);
        ++it_point;
        if ((0 < value) != isFirstValuePositive)
        {
            _isBoundary = detail::BoundaryTag::True;
            break;
        }
    }

    // If the boundary flag hasn't been modified -> not a boundary
    if (_isBoundary == detail::BoundaryTag::Null)
        _isBoundary = detail::BoundaryTag::False;

    CIE_END_EXCEPTION_TRACING
}


template <class TCell, class TValue>
inline void
SpaceTreeNode<TCell,TValue>::evaluate(const typename SpaceTreeNode<TCell,TValue>::Target& r_target,
                                      typename SpaceTreeNode<TCell,TValue>::value_container_type& r_valueContainer)
{
    CIE_BEGIN_EXCEPTION_TRACING
    CIE_PROFILE_SCOPE

    // Init
    _isBoundary = detail::BoundaryTag::Null;
    cie::utils::resize(r_valueContainer, _p_sampler->size());
    auto it_value = r_valueContainer.begin();
    typename SpaceTreeNode<TCell,TValue>::SamplePointIterator it_point(0,this);

    // Evaluate first point separately and set sign flag
    *it_value = r_target(*it_point);
    const bool isFirstValuePositive = 0 < (*it_value);
    ++it_value;
    ++it_point;

    const auto it_valueEnd = r_valueContainer.end();

    // Evaluate the rest of the points
    for (; it_value!=it_valueEnd; ++it_value,++it_point)
    {
        *it_value = r_target(*it_point);

        // Boundary check
        if ((0 < (*it_value)) != isFirstValuePositive)
        {
            _isBoundary = detail::BoundaryTag::True;
            //break;
        }
    }

    // If the boundary flag hasn't been modified -> not a boundary
    if (_isBoundary == detail::BoundaryTag::Null)
        _isBoundary = detail::BoundaryTag::False;

    CIE_END_EXCEPTION_TRACING
}


template <class TCell, class TValue>
inline void
SpaceTreeNode<TCell,TValue>::clear()
{
    // Clear data
    Base::clear();

    // Reset flags
    _isBoundary = detail::BoundaryTag::Null;
}


template <class TCell, class TValue>
inline bool
SpaceTreeNode<TCell,TValue>::isBoundary() const
{
    CIE_CHECK(_isBoundary != detail::BoundaryTag::Null,
              "SpaceTreeNode::isBoundary expects the node to be evaluated")

    return _isBoundary == detail::BoundaryTag::True ? true : false;
}


template <class TCell, class TValue>
typename SpaceTreeNode<TCell,TValue>::SamplePointContainer
SpaceTreeNode<TCell,TValue>::getSamplePoints() const
{
    return SamplePointContainer(
        SamplePointIterator(0, this),
        SamplePointIterator(_p_sampler->size(), this)
    );
}


template <class TCell, class TValue>
inline void
SpaceTreeNode<TCell,TValue>::setSplitPolicy(typename SpaceTreeNode<TCell,TValue>::split_policy_ptr p_splitPolicy)
{
    _p_splitPolicy = p_splitPolicy;
    this->_children.clear();
}


template <class TCell, class TValue>
inline void
SpaceTreeNode<TCell,TValue>::setSampler(typename SpaceTreeNode<TCell,TValue>::sampler_ptr p_sampler)
{
    // Set pointer
    _p_sampler = p_sampler;

    // Reset flags
    _isBoundary = detail::BoundaryTag::Null;
}


template <class TCell, class TValue>
inline const typename SpaceTreeNode<TCell,TValue>::split_policy_ptr&
SpaceTreeNode<TCell,TValue>::splitPolicy() const
{
    return _p_splitPolicy;
}


template <class TCell, class TValue>
inline const typename SpaceTreeNode<TCell,TValue>::sampler_ptr&
SpaceTreeNode<TCell,TValue>::sampler() const
{
    return _p_sampler;
}


template <class TCell, class TValue>
template <concepts::ThreadPool TPool>
inline bool
SpaceTreeNode<TCell,TValue>::divideImpl(const typename SpaceTreeNode<TCell,TValue>::Target& r_target,
                                        Size level,
                                        TPool& r_pool)
{
    CIE_BEGIN_EXCEPTION_TRACING

    // Clear children
    this->children().clear();

    // Evaluate target and set boundary flag
    value_container_type values;
    this->evaluate(r_target, values);

    // Do nothing if this is the last level
    if (level <= this->level())
        return false;

    // Split if boundary
    if (this->isBoundary())
    {
        const auto splitPoint = _p_splitPolicy->operator()(
            values.begin(),
            values.end(),
            SamplePointIterator(0, this)
        );

        const auto nodeConstructor = std::make_tuple(_p_sampler,
                                               _p_splitPolicy,
                                               this->level() + 1);
        const auto cellConstructors = this->split(splitPoint);
        utils::reserve(this->children(), cellConstructors.size());

        for (const auto& cellConstructor : cellConstructors)
        {
            // Construct a child
            const auto compoundConstructor = std::tuple_cat(nodeConstructor, cellConstructor);
            auto node = std::make_from_tuple<SpaceTreeNode>(compoundConstructor);
            if (node.isDegenerate()) [[unlikely]]
                continue;

            this->children().emplace_back(std::move(node));
            auto& r_node = this->children().back();

            // Schedule divide on child
            r_pool.queueJob([&r_node,&r_target,&r_pool,level]() -> void
                {r_node.divideImpl(r_target, level, r_pool);}
            );
        }

        return true;
    }

    return false;

    CIE_END_EXCEPTION_TRACING
}


} // namespace cie::geo

#endif