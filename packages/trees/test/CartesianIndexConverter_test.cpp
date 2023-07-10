// --- GEO Includes ---
#include "packages/trees/inc/CartesianIndexConverter.hpp"

// --- Utility Includes ---
#include "packages/testing/inc/essentials.hpp"


namespace cie::geo {


CIE_TEST_CASE("CartesianIndexConverter", "[trees]")
{
    CIE_TEST_CASE_INIT("CartesianIndexConverter")

    const Size Dimension            = 2;
    using Converter = CartesianIndexConverter<Dimension,Size,Size,tags::Eager>;

    Converter::Resolution resolution {3, 3};

    CIE_TEST_REQUIRE_NOTHROW(Converter(resolution));
    Converter converter(resolution);

    CIE_TEST_REQUIRE(converter.size() == 9);

    Converter::SpatialIndex test;
    Size counter = 0;

    // 0
    CIE_TEST_CHECK_NOTHROW(test = converter.convert(counter++));
    CIE_TEST_REQUIRE(test.size() == Dimension);
    CIE_TEST_CHECK(test[0] == 0);
    CIE_TEST_CHECK(test[1] == 0);

    // 1
    CIE_TEST_CHECK_NOTHROW(test = converter.convert(counter++));
    CIE_TEST_REQUIRE(test.size() == Dimension);
    CIE_TEST_CHECK(test[0] == 1);
    CIE_TEST_CHECK(test[1] == 0);

    // 2
    CIE_TEST_CHECK_NOTHROW(test = converter.convert(counter++));
    CIE_TEST_REQUIRE(test.size() == Dimension);
    CIE_TEST_CHECK(test[0] == 2);
    CIE_TEST_CHECK(test[1] == 0);

    // 3
    CIE_TEST_CHECK_NOTHROW(test = converter.convert(counter++));
    CIE_TEST_REQUIRE(test.size() == Dimension);
    CIE_TEST_CHECK(test[0] == 0);
    CIE_TEST_CHECK(test[1] == 1);

    // 4
    CIE_TEST_CHECK_NOTHROW(test = converter.convert(counter++));
    CIE_TEST_REQUIRE(test.size() == Dimension);
    CIE_TEST_CHECK(test[0] == 1);
    CIE_TEST_CHECK(test[1] == 1);

    // 5
    CIE_TEST_CHECK_NOTHROW(test = converter.convert(counter++));
    CIE_TEST_REQUIRE(test.size() == Dimension);
    CIE_TEST_CHECK(test[0] == 2);
    CIE_TEST_CHECK(test[1] == 1);

    // 6
    CIE_TEST_CHECK_NOTHROW(test = converter.convert(counter++));
    CIE_TEST_REQUIRE(test.size() == Dimension);
    CIE_TEST_CHECK(test[0] == 0);
    CIE_TEST_CHECK(test[1] == 2);

    // 7
    CIE_TEST_CHECK_NOTHROW(test = converter.convert(counter++));
    CIE_TEST_REQUIRE(test.size() == Dimension);
    CIE_TEST_CHECK(test[0] == 1);
    CIE_TEST_CHECK(test[1] == 2);

    // 8
    CIE_TEST_CHECK_NOTHROW(test = converter.convert(counter++));
    CIE_TEST_REQUIRE(test.size() == Dimension);
    CIE_TEST_CHECK(test[0] == 2);
    CIE_TEST_CHECK(test[1] == 2);

    #ifdef CIE_ENABLE_OUT_OF_RANGE_CHECKS
    // 9 - error
    CIE_TEST_CHECK_THROWS(test = converter.convert(counter++));
    #endif

    resolution = {2, 2};
    converter.setResolution(resolution);
    CIE_TEST_REQUIRE(converter.size() == 4);

    counter = 0;

    // 0
    CIE_TEST_CHECK_NOTHROW(test = converter.convert(counter++));
    CIE_TEST_REQUIRE(test.size() == Dimension);
    CIE_TEST_CHECK(test[0] == 0);
    CIE_TEST_CHECK(test[1] == 0);

    // 1
    CIE_TEST_CHECK_NOTHROW(test = converter.convert(counter++));
    CIE_TEST_REQUIRE(test.size() == Dimension);
    CIE_TEST_CHECK(test[0] == 1);
    CIE_TEST_CHECK(test[1] == 0);

    // 2
    CIE_TEST_CHECK_NOTHROW(test = converter.convert(counter++));
    CIE_TEST_REQUIRE(test.size() == Dimension);
    CIE_TEST_CHECK(test[0] == 0);
    CIE_TEST_CHECK(test[1] == 1);

    // 3
    CIE_TEST_CHECK_NOTHROW(test = converter.convert(counter++));
    CIE_TEST_REQUIRE(test.size() == Dimension);
    CIE_TEST_CHECK(test[0] == 1);
    CIE_TEST_CHECK(test[1] == 1);

    #ifdef CIE_ENABLE_OUT_OF_RANGE_CHECKS
    // 4 - error
    CIE_TEST_CHECK_THROWS(test = converter.convert(counter++));
    #endif
}


} // namespace cie::geo