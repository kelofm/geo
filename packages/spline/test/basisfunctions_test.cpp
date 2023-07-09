// --- Utility Includes ---
#include "packages/testing/inc/essentials.hpp"

// --- Internal Includes ---
#include "packages/spline/inc/basisfunctions.hpp" 

// --- STL Includes ---
#include <vector>


namespace cie::geo {


CIE_TEST_CASE("Linear interpolation", "[splinekernel]")
{
    CIE_TEST_CASE_INIT( "Linear interpolation" );

    std::vector<double> knotVector{ 0.0, 0.0, 0.5, 1.0, 1.0 };

    const size_t p = 1;

    // First basis function
    CIE_TEST_REQUIRE_NOTHROW(evaluateBSplineBasis(0.0, 0, p, knotVector));

    CIE_TEST_CHECK(evaluateBSplineBasis(0.00, 0, p, knotVector) == Approx(1.0));
    CIE_TEST_CHECK(evaluateBSplineBasis(0.25, 0, p, knotVector) == Approx(0.5));
    CIE_TEST_CHECK(evaluateBSplineBasis(0.50, 0, p, knotVector) == Approx(0.0));
    CIE_TEST_CHECK(evaluateBSplineBasis(0.75, 0, p, knotVector) == Approx(0.0));
    CIE_TEST_CHECK(evaluateBSplineBasis(1.00, 0, p, knotVector) == Approx(0.0));

    //  Second basis function
    CIE_TEST_REQUIRE_NOTHROW(evaluateBSplineBasis(0.0, 1, p, knotVector));

    CIE_TEST_CHECK(evaluateBSplineBasis(0.00, 1, p, knotVector) == Approx(0.0));
    CIE_TEST_CHECK(evaluateBSplineBasis(0.25, 1, p, knotVector) == Approx(0.5));
    CIE_TEST_CHECK(evaluateBSplineBasis(0.50, 1, p, knotVector) == Approx(1.0));
    CIE_TEST_CHECK(evaluateBSplineBasis(0.75, 1, p, knotVector) == Approx(0.5));
    CIE_TEST_CHECK(evaluateBSplineBasis(1.00, 1, p, knotVector) == Approx(0.0));

    //  Third basis function
    CIE_TEST_REQUIRE_NOTHROW(evaluateBSplineBasis(0.0, 2, p, knotVector));

    CIE_TEST_CHECK(evaluateBSplineBasis(0.00, 2, p, knotVector) == Approx(0.0));
    CIE_TEST_CHECK(evaluateBSplineBasis(0.25, 2, p, knotVector) == Approx(0.0));
    CIE_TEST_CHECK(evaluateBSplineBasis(0.50, 2, p, knotVector) == Approx(0.0));
    CIE_TEST_CHECK(evaluateBSplineBasis(0.75, 2, p, knotVector) == Approx(0.5));
    CIE_TEST_CHECK(evaluateBSplineBasis(1.00, 2, p, knotVector) == Approx(1.0));

    // Check if evaluating more basis functions throws an error
    CIE_TEST_CHECK_THROWS(evaluateBSplineBasis(0.0, 3, p, knotVector));
}

CIE_TEST_CASE( "Quadratic C1 interpolation", "[splinekernel]" )
{
    CIE_TEST_CASE_INIT( "Quadratic C1 interpolation" )

    std::vector<double> knotVector{ 0.0, 0.0, 0.0, 0.5, 1.0, 1.0, 1.0 };

    const size_t p = 2;

    // First basis functiontype ‘struct type_caster’ violates the C++ One Definition Rule
    CIE_TEST_REQUIRE_NOTHROW(evaluateBSplineBasis(0.0, 0, p, knotVector));

    CIE_TEST_CHECK(evaluateBSplineBasis(0.00, 0, p, knotVector) == Approx(1.0));
    CIE_TEST_CHECK(evaluateBSplineBasis(0.25, 0, p, knotVector) == Approx(0.25));
    CIE_TEST_CHECK(evaluateBSplineBasis(0.50, 0, p, knotVector) == Approx(0.0));
    CIE_TEST_CHECK(evaluateBSplineBasis(0.75, 0, p, knotVector) == Approx(0.0));
    CIE_TEST_CHECK(evaluateBSplineBasis(1.00, 0, p, knotVector) == Approx(0.0));

    //  Second basis function
    CIE_TEST_REQUIRE_NOTHROW(evaluateBSplineBasis(0.0, 1, p, knotVector));

    CIE_TEST_CHECK(evaluateBSplineBasis(0.00, 1, p, knotVector) == Approx(0.0));
    CIE_TEST_CHECK(evaluateBSplineBasis(0.25, 1, p, knotVector) == Approx(0.625));
    CIE_TEST_CHECK(evaluateBSplineBasis(0.50, 1, p, knotVector) == Approx(0.5));
    CIE_TEST_CHECK(evaluateBSplineBasis(0.75, 1, p, knotVector) == Approx(0.125));
    CIE_TEST_CHECK(evaluateBSplineBasis(1.00, 1, p, knotVector) == Approx(0.0));

    //  Third basis function
    CIE_TEST_REQUIRE_NOTHROW(evaluateBSplineBasis(0.0, 2, p, knotVector));

    CIE_TEST_CHECK(evaluateBSplineBasis(0.00, 2, p, knotVector) == Approx(0.0));
    CIE_TEST_CHECK(evaluateBSplineBasis(0.25, 2, p, knotVector) == Approx(0.125));
    CIE_TEST_CHECK(evaluateBSplineBasis(0.50, 2, p, knotVector) == Approx(0.5));
    CIE_TEST_CHECK(evaluateBSplineBasis(0.75, 2, p, knotVector) == Approx(0.625));
    CIE_TEST_CHECK(evaluateBSplineBasis(1.00, 2, p, knotVector) == Approx(0.0));

    //  Fourth basis function
    CIE_TEST_REQUIRE_NOTHROW(evaluateBSplineBasis(0.0, 3, p, knotVector));

    CIE_TEST_CHECK(evaluateBSplineBasis(0.00, 3, p, knotVector) == Approx(0.0));
    CIE_TEST_CHECK(evaluateBSplineBasis(0.25, 3, p, knotVector) == Approx(0.0));
    CIE_TEST_CHECK(evaluateBSplineBasis(0.50, 3, p, knotVector) == Approx(0.0));
    CIE_TEST_CHECK(evaluateBSplineBasis(0.75, 3, p, knotVector) == Approx(0.25));
    CIE_TEST_CHECK(evaluateBSplineBasis(1.00, 3, p, knotVector) == Approx(1.0));

    // Check if evaluating more basis functions throws an error
    CIE_TEST_CHECK_THROWS(evaluateBSplineBasis(0.0, 4, p, knotVector));
}

} // cie::geo
