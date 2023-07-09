#ifndef CIE_GEO_TREES_CARTESIAN_INDEX_CONVERTER_HPP
#define CIE_GEO_TREES_CARTESIAN_INDEX_CONVERTER_HPP

// --- Utility Includes ---
#include "packages/types/inc/types.hpp"
#include "packages/stl_extension/inc/StaticArray.hpp"
#include "packages/stl_extension/inc/DynamicArray.hpp"
#include "packages/compile_time/packages/concepts/inc/basic_concepts.hpp"
#include "packages/compile_time/packages/concepts/inc/iterator_concepts.hpp"
#include "packages/types/inc/tags.hpp"
#include "packages/macros/inc/typedefs.hpp"

// --- STL Includes ---
#include <vector>
#include <memory>


namespace cie::geo {


namespace detail {


template <Size Dim,
          concepts::UnsignedInteger TIndex,
          concepts::UnsignedInteger TFlat>
struct CartesianIndexConverterTraits
{
    constexpr static const Size Dimension = Dim;

    using Index = TIndex;

    using Flat = TFlat;

    using SpatialIndex = StaticArray<Size,Dimension>;

    using Resolution = StaticArray<TIndex,Dimension>;
}; // class CartesianIndexConverterTraits


template <Size Dim,
          concepts::UnsignedInteger TIndex,
          concepts::UnsignedInteger TFlat,
          class ExecutionTag>
class CartesianIndexConverter
{}; // class CartesianIndexConverter


template <Size Dim,
          concepts::UnsignedInteger TIndex,
          concepts::UnsignedInteger TFlat>
class CartesianIndexConverter<Dim,TIndex,TFlat,tags::Eager> : private CartesianIndexConverterTraits<Dim,TIndex,TFlat>
{
private:
    using Traits = CartesianIndexConverterTraits<Dim,TIndex,TFlat>;

public:
    using Index = typename Traits::Index;

    using Flat = typename Traits::Flat;

    using SpatialIndex = typename Traits::SpatialIndex;

    using Resolution = typename Traits::Resolution;

    using ReturnType = const typename CartesianIndexConverter::SpatialIndex&;

public:
    CartesianIndexConverter(const Resolution& r_resolution);

    ReturnType convert(Flat index) const;

    Size size() const noexcept;

private:
    DynamicArray<SpatialIndex> _indexMap;
}; // class CartesianIndexConverter<tags::Eager>


template <Size Dim,
          concepts::UnsignedInteger TIndex,
          concepts::UnsignedInteger TFlat>
class CartesianIndexConverter<Dim,TIndex,TFlat,tags::Lazy>
{
private:
    using Traits = CartesianIndexConverterTraits<Dim,TIndex,TFlat>;

public:
    using Index = typename Traits::Index;

    using Flat = typename Traits::Flat;

    using SpatialIndex = typename Traits::SpatialIndex;

    using Resolution = typename Traits::Resolution;

    using ReturnType = SpatialIndex;

public:
    CartesianIndexConverter(const Resolution& r_resoution);

    ReturnType convert(Flat index) const;

    Size size() const noexcept;

private:
    StaticArray<Flat,Dim+1> _prefixPopulation;
}; // class CartesianIndexConverter<tags::Lazy>


} // namespace detail


/** @brief Map flat grid indices to k-dimensional ones.
 *  @ingroup geo
 */
template <Size Dim,
          concepts::UnsignedInteger TIndex,
          concepts::UnsignedInteger TFlat,
          class ExecutionTag>
class CartesianIndexConverter
{
private:
    using Impl = detail::CartesianIndexConverter<Dim,TIndex,TFlat,ExecutionTag>;

public:
    using Resolution = typename Impl::Resolution;

    using ReturnType = typename Impl::ReturnType;

    using SpatialIndex = typename Impl::SpatialIndex;

    CIE_DEFINE_CLASS_POINTERS(CartesianIndexConverter)

public:
    CartesianIndexConverter(const Resolution& r_resolution);

    /// @brief Uniform discretization in all directions.
    CartesianIndexConverter(TIndex pointsPerDirection);

    ReturnType convert(TFlat index) const;

    Size size() const noexcept;

    template <concepts::Iterator<TIndex> TBegin, concepts::Iterator<TIndex> TEnd>
    void setResolution(TBegin begin, TEnd end);

    void setResolution(const Resolution& r_resolution);

private:
    detail::CartesianIndexConverter<Dim,TIndex,TFlat,ExecutionTag> _converter;
};


} // namespace cie::geo

#include "packages/trees/impl/CartesianIndexConverter_impl.hpp"

#endif