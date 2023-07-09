#ifndef CIE_GEO_PARTITIONING_ABS_BOUNDABLE_OBJECT_HPP
#define CIE_GEO_PARTITIONING_ABS_BOUNDABLE_OBJECT_HPP

// --- Utility Includes ---
#include "packages/compile_time/packages/concepts/inc/basic_concepts.hpp"

// --- Internal Includes ---
#include "packages/partitioning/inc/AABBox.hpp"

// --- STL Includes ---
#include <concepts>


namespace cie::geo {


template < Size Dimension,
           concepts::Numeric CoordinateType >
class AbsBoundableObject : public Traits<Dimension,CoordinateType>
{
public:
    using bounding_box = AABBox<Dimension,CoordinateType>;

public:
    AbsBoundableObject();
    AbsBoundableObject( const AbsBoundableObject<Dimension,CoordinateType>& r_rhs ) = default;
    AbsBoundableObject<Dimension,CoordinateType>& operator=( const AbsBoundableObject<Dimension,CoordinateType>& r_rhs ) = default;

    const bounding_box& boundingBox();

protected:
    void boundingBoxShouldRecompute();

    void computeBoundingBox();
    virtual void computeBoundingBox_impl( bounding_box& r_box ) = 0;

private:
    bool         _recomputeBoundingBox;
    bounding_box _boundingBox;
};


} // namespace cie::geo



/* --- Concepts --- */

namespace cie::concepts {

template <class T, class _ = void>
struct HasBoundingBoxType : std::false_type {};

template <class ...Args>
struct HasBoundingBoxTypeHelper {};

template <class T>
struct HasBoundingBoxType<
    T,
    std::conditional_t<
        false,
        HasBoundingBoxTypeHelper<
        typename T::bounding_box
        >,
        void
    >
> : public std::true_type {};

template <class T>
concept HasBoundingBox
= HasBoundingBoxType<T>::value
  && requires ( T instance )
{
    { instance.boundingBox() };
};

template <class T>
concept HasNoBoundingBox
= !HasBoundingBox<T>;

} // namespace cie::concepts



#include "packages/partitioning/impl/AbsBoundableObject_impl.hpp"

#endif