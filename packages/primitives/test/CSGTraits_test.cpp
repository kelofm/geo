// --- Utility Includes ---
#include "packages/testing/inc/essentials.hpp"

// --- Internal Includes ---
#include "packages/primitives/inc/Traits.hpp"


namespace cie::geo {


namespace detail {
template <concepts::Object T>
bool isObject(const T&) {return true;}

template <class T>
requires (!concepts::Object<T>)
bool isObject(const T&) {return false;}
} // namespace detail


CIE_TEST_CASE( "Traits", "[primitives]" )
{
    CIE_TEST_CASE_INIT( "Traits" )

    using RawTraits = Traits<1,Double>;
    RawTraits rawTraits;
    CIE_TEST_CHECK( detail::isObject(rawTraits) );

    using DeducedTraits = GetTraits<RawTraits>::Type;
    DeducedTraits deducedTraits;
    CIE_TEST_CHECK( detail::isObject(deducedTraits) );

    using PointTraits = GetTraits<RawTraits::Point>::Type;
    PointTraits pointTraits;
    CIE_TEST_CHECK( detail::isObject(pointTraits) );
}


} // namespace cie::geo