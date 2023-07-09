#ifndef CIE_MESH_CARTESIAN_GRID_HPP
#define CIE_MESH_CARTESIAN_GRID_HPP

// --- GEO Includes ---
#include "packages/trees/inc/CartesianIndexConverter.hpp"

// --- Utility Includes ---
#include "packages/compile_time/packages/concepts/inc/basic_concepts.hpp"
#include "packages/compile_time/packages/concepts/inc/iterator_concepts.hpp"
#include "packages/compile_time/packages/concepts/inc/container_concepts.hpp"
#include "packages/macros/inc/typedefs.hpp"
#include "packages/stl_extension/inc/StaticArray.hpp"
#include "packages/types/inc/types.hpp"
#include "packages/types/inc/tags.hpp"

// --- STL Includes ---
#include <iterator>
#include <ranges>


namespace cie::geo {


/// @ingroup geo
template <Size Dim,
          concepts::Numeric TCoordinate,
          concepts::UnsignedInteger TIndex,
          class ExecutionTag = tags::Lazy>
class CartesianGrid
{
public:
    static constexpr const Size Dimension = Dim;

    using Point = StaticArray<TCoordinate,Dimension>;

    using Vector = StaticArray<TCoordinate,Dimension>;

    using Element = std::pair<Point,Vector>;

private:
    using IndexConverter = CartesianIndexConverter<Dim,TIndex,TIndex,ExecutionTag>;

    struct GetNode
    {
        using Value = Point;

        static Value get(Ptr<const CartesianGrid> p_grid, TIndex index) {p_grid->getNode(index);}
    }; // struct GetNode

    struct GetElement
    {
        using Value = Element;

        static Value get(Ptr<const CartesianGrid> p_grid, TIndex index) {p_grid->getElement(index);}
    }; // struct GetElement

    template <class TGetter>
    class Iterator
    {
    private:
        friend class CartesianGrid;

    public:
        using value_type = typename TGetter::Value;

        using pointer = Ptr<value_type>;

        using reference = Ref<value_type>;

        using difference_type = std::ptrdiff_t;

        using iterator_category = std::forward_iterator_tag;

    public:
        Iterator(Ptr<const CartesianGrid> p_grid, TIndex index) : _p_grid(p_grid), _index(index) {}

        Iterator(Ptr<const CartesianGrid> p_grid) : Iterator(p_grid, 0) {}

        Iterator(RightRef<Iterator> r_rhs) = default;

        Iterator(Ref<const Iterator> r_rhs) = default;

        Ref<Iterator> operator=(RightRef<const Iterator> r_rhs) = default;

        Ref<Iterator> operator=(Ref<const Iterator> r_rhs) = default;

        value_type operator*() const {TGetter::get(_p_grid, _index);}

        void operator++() {++_index;}

        Ref<Iterator> operator++(int) {Iterator copy(*this); ++(*this); return copy;}

        Ref<Iterator> operator+=(TIndex difference) {_index += difference;}

        friend difference_type operator-(Iterator lhs, Iterator rhs) {return lhs._index - rhs._index;}

        friend Bool operator<(Iterator lhs, Iterator rhs) {return lhs._index < rhs._index;}

        friend Bool operator!=(Iterator lhs, Iterator rhs) {return lhs._index != rhs._index;}

    private:
        Ptr<const CartesianGrid> _p_grid;

        TIndex _index;
    }; // class Iterator

public:
    using NodeIt = Iterator<Point>;

    using ElementIt = Iterator<Element>;

    using NodeRange = std::ranges::subrange<NodeIt>;

    using ElementRange = std::ranges::subrange<ElementIt>;

    using Resolution = typename IndexConverter::Resolution;

public:

    template <concepts::Container<TCoordinate> BaseContainer, concepts::Container<TCoordinate> LengthContainer>
    CartesianGrid(const BaseContainer& r_base,
                  const LengthContainer& r_lengths,
                  const Resolution& r_resolution);

    template <concepts::Iterator<TCoordinate> TBaseIt,
              concepts::Iterator<TCoordinate> TLenghtIt,
              concepts::Iterator<TIndex> TResolutionIt>
    CartesianGrid(TBaseIt it_base, TLenghtIt it_length, TResolutionIt it_resolution);

    Point getNode(TIndex index) const;

    template <concepts::Container<TIndex> TContainer>
    Point getNode(const TContainer& r_indices) const;

    template <concepts::Iterator<TIndex> TIndexIt>
    Point getNode(TIndexIt it_index) const;

    Element getElement(TIndex index) const;

    template <concepts::Iterator<TIndex> TIndexIt>
    Element getElement(TIndexIt it_index) const;

    template <concepts::Container<TIndex> TContainer>
    Element getElement(const TContainer& r_indices) const;

    /// @brief Get the total number of nodes in the grid.
    TIndex size() const noexcept;

    NodeRange nodes() const noexcept;

    ElementRange elements() const noexcept;

    NodeIt nodeBegin() const noexcept;

    NodeIt nodeEnd() const noexcept;

    ElementIt elementBegin() const noexcept;

    ElementIt elementEnd() const noexcept;

private:
    IndexConverter _nodeConverter;

    IndexConverter _elementConverter;

    Resolution _resolution;

    Point _base;

    Vector _increments;
}; // class CartesianGrid


} // namespace cie::geo

#include "packages/structured/impl/CartesianGrid_impl.hpp"

#endif
