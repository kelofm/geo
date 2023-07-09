// --- Utility Includes ---
#include "packages/testing/inc/essentials.hpp"

// --- Internal Includes ---
#include "packages/marchingprimitives/inc/connectivitytables.hpp"


namespace cie::geo {


CIE_TEST_CASE( "marchingCubesConnectivityMap", "[marchingprimitives]" )
{
    CIE_TEST_CASE_INIT( "marchingCubesConnectivityMap" )

    auto table = detail::marchingCubesConnectivityMap;
}


} // namespace cie::geo