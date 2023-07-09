#ifndef CIE_GEO_SPACE_TREE_NODE_HPP
#define CIE_GEO_SPACE_TREE_NODE_HPP

// --- Linalg Includes ---
#include "packages/exceptions/inc/exception.hpp"
#include "packages/macros/inc/checks.hpp"
#include "packages/macros/inc/typedefs.hpp"

// --- GEO Includes ---
#include "packages/trees/inc/AbsCell.hpp"
#include "packages/trees/inc/SplitPolicy.hpp"

// --- Utility Includes ---
#include "packages/trees/inc/abstree.hpp"
#include "packages/stl_extension/inc/DynamicArray.hpp"
#include "packages/concurrency/inc/ThreadPool.hpp"
#include "packages/compile_time/packages/concepts/inc/functional.hpp"

// --- STL Includes ---
#include <iterator>
#include <stdint.h>
#include <memory>
#include <functional>
#include <array>
#include <ranges>

namespace cie::geo {


namespace detail {

template <class TSelf, class TCell, class TValue>
struct SpaceTreeNodeTraits
{
    using Tree = utils::AbsTree<TSelf,DynamicArray,TSelf>;
}; // struct SpaceTreeNodeTraits

enum class BoundaryTag
{
    True,
    False,
    Null
}; // enum class BoundaryTag

} // namespace detail


template <class TCell, class TValue>
class SpaceTreeNode :
    public detail::SpaceTreeNodeTraits<SpaceTreeNode<TCell,TValue>,TCell,TValue>::Tree,
    public TCell
{
private:
    using Self = SpaceTreeNode<TCell,TValue>;

    using Base = typename detail::SpaceTreeNodeTraits<SpaceTreeNode<TCell,TValue>,TCell,TValue>::Tree;

public:
    /// Point iterator that lazily generates sample points.
    class SamplePointIterator
    {
    public:
        using value_type = typename SpaceTreeNode::Point;

        using difference_type = int;

        using pointer = value_type*;

        using reference = value_type&;

        using iterator_category = std::random_access_iterator_tag;

    public:
        SamplePointIterator()
            : _counter(0),
              _p_node(nullptr)
        {}

        SamplePointIterator(difference_type counter, const SpaceTreeNode<TCell,TValue>* p_node) :
            _counter(counter),
            _p_node(p_node)
        {}

        SamplePointIterator(SamplePointIterator&& r_rhs) = default;

        SamplePointIterator(const SamplePointIterator& r_rhs) = default;

        SamplePointIterator& operator=(SamplePointIterator&& r_rhs) = default;

        SamplePointIterator& operator=(const SamplePointIterator& r_rhs) = default;

        SamplePointIterator& operator++()
        {++_counter; return *this;}

        SamplePointIterator operator++(int)
        {auto copy = *this; ++_counter; return copy;}

        SamplePointIterator& operator--()
        {--_counter; return *this;}

        SamplePointIterator operator--(int)
        {auto copy = *this; --_counter; return copy;}

        SamplePointIterator& operator+=(difference_type difference)
        {_counter += difference; return *this;}

        SamplePointIterator& operator-=(difference_type difference)
        {_counter -= difference; return *this;}

        value_type operator*() const
        {return _p_node->_p_sampler->getSamplePoint(*_p_node, _counter);}

    public:
        friend bool operator!=(const SamplePointIterator& r_lhs, const SamplePointIterator& r_rhs)
        {return r_lhs._counter != r_rhs._counter || r_lhs._p_node != r_rhs._p_node;}

        friend bool operator==(const SamplePointIterator& r_lhs, const SamplePointIterator& r_rhs)
        {return !(r_lhs != r_rhs);}

        friend bool operator<(const SamplePointIterator& r_lhs, const SamplePointIterator& r_rhs)
        {
            CIE_OUT_OF_RANGE_CHECK(r_lhs._p_node == r_rhs._p_node)
            return r_lhs._counter < r_rhs._counter;
        }

        friend SamplePointIterator operator+(const SamplePointIterator& r_lhs, difference_type rhs)
        {return SamplePointIterator(r_lhs._counter + rhs, r_lhs._p_node);}

        friend SamplePointIterator operator-(const SamplePointIterator& r_lhs, difference_type rhs)
        {return SamplePointIterator(r_lhs._counter - rhs, r_lhs._p_node);}

        friend difference_type operator-(const SamplePointIterator& r_lhs, const SamplePointIterator& r_rhs)
        {
            CIE_OUT_OF_RANGE_CHECK(r_lhs._p_node == r_rhs._p_node)
            return r_lhs._counter - r_rhs._counter;
        }

    private:
        Size _counter;

        const SpaceTreeNode<TCell,TValue>* _p_node;
    };

public:
    using Target  = std::function<TValue(const typename TCell::Point&)>;

    using Cell = TCell;

    using value_type = TValue;

    using value_container_type = std::vector<value_type>;

    using value_iterator = typename value_container_type::const_iterator;

    using SamplePointContainer = std::ranges::subrange<SamplePointIterator>;

    using sampler_ptr = PrimitiveSamplerPtr<typename TCell::Primitive>;

    using split_policy_ptr = SplitPolicyPtr<SamplePointIterator,value_iterator>;

    CIE_DEFINE_CLASS_POINTERS(SpaceTreeNode)

public:
    SpaceTreeNode() = delete;

    /** Constructor that forwards its arguments to the
     *  constructor of its primitive (through the cell constructor).
     */
    template <class ...Args>
    SpaceTreeNode(const sampler_ptr& p_sampler,
                  const split_policy_ptr& p_splitPolicy,
                  Size level,
                  Args&&... r_args);

    SpaceTreeNode(SpaceTreeNode&& r_rhs) = default;

    explicit SpaceTreeNode(const SpaceTreeNode& r_rhs) = default;

    SpaceTreeNode& operator=(SpaceTreeNode&& r_rhs) = default;

    SpaceTreeNode& operator=(const SpaceTreeNode& r_rhs) = default;

    bool divide(Ref<const Target> r_target, Size level);

    template <concepts::ThreadPool TPool>
    bool divide(const Target& r_target, Size level, TPool& r_pool);

    template <concepts::CallableWith<const SpaceTreeNode&> TFunction>
    void scan(const Target& r_target,
              const TFunction& r_function,
              const Size level);

    template <concepts::CallableWith<const SpaceTreeNode&> TFunction, concepts::ThreadPool TPool>
    void scan(const Target& r_target,
              const TFunction& r_function,
              const Size level,
              TPool& r_pool);

    /// Evaluate the target function at sample points to set the boundary flag
    void evaluate(const Target& r_target);

    /// Evaluate the target function at all sample points and store the results.
    void evaluate(const Target& r_target,
                  value_container_type& r_valueContainer);

    /// Clear data container and call clear on children.
    virtual void clear() override;

    /** Check whether this cell is cut.
     *  The node needs to be in an evaluated state, otherwise
     *  an exception is thrown.
     */
    bool isBoundary() const;

    SamplePointContainer getSamplePoints() const;

    void setSplitPolicy(split_policy_ptr p_splitPolicy);

    void setSampler(sampler_ptr p_sampler);

    const split_policy_ptr& splitPolicy() const;

    const sampler_ptr& sampler() const;

protected:
    template <concepts::ThreadPool TPool>
    bool divideImpl(const Target& r_target,
                    Size level,
                    TPool& r_pool);

    template <concepts::CallableWith<const SpaceTreeNode&> TFunction, concepts::ThreadPool TPool>
    void scanImpl(const Target& r_target,
                  const TFunction& r_function,
                  const Size level,
                  TPool& r_pool);

protected:
    split_policy_ptr _p_splitPolicy;

private:
    sampler_ptr _p_sampler;

    detail::BoundaryTag _isBoundary;
}; // class SpaceTreeNode



template <class TCell, class TValue>
using SpaceTreeNodePtr = std::shared_ptr<SpaceTreeNode<TCell,TValue>>;


} // namespace cie::geo

#include "packages/trees/impl/SpaceTreeNode_impl.hpp"

#endif