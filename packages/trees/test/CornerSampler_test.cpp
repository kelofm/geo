// --- Utility Includes ---
#include "packages/testing/inc/essentials.hpp"

// --- Internal Includes ---
#include "packages/trees/inc/CornerSampler.hpp"


namespace cie::geo {


CIE_TEST_CASE( "CornerSampler", "[trees]" )
{
    CIE_TEST_CASE_INIT( "CornerSampler" )

    {
        CIE_TEST_CASE_INIT( "Cube" )

        using Primitive = Cube<2,double>;
        using Sampler = CornerSampler<Primitive>;

        Primitive primitive( {-1.0, 0.0}, 2.0 );

        CIE_TEST_CHECK_NOTHROW( Sampler() );
        Sampler sampler;
        CIE_TEST_REQUIRE( sampler.size() == 4 );

        Primitive::Point samplePoint;
        CIE_TEST_CHECK_NOTHROW( samplePoint = sampler.getSamplePoint(primitive, 0) );
        CIE_TEST_CHECK( samplePoint[0] == Approx(-1.0) );
        CIE_TEST_CHECK( samplePoint[1] == Approx(0.0) );

        CIE_TEST_CHECK_NOTHROW( samplePoint = sampler.getSamplePoint(primitive, 1) );
        CIE_TEST_CHECK( samplePoint[0] == Approx(1.0) );
        CIE_TEST_CHECK( samplePoint[1] == Approx(0.0) );

        CIE_TEST_CHECK_NOTHROW( samplePoint = sampler.getSamplePoint(primitive, 2) );
        CIE_TEST_CHECK( samplePoint[0] == Approx(-1.0) );
        CIE_TEST_CHECK( samplePoint[1] == Approx(2.0) );

        CIE_TEST_CHECK_NOTHROW( samplePoint = sampler.getSamplePoint(primitive, 3) );
        CIE_TEST_CHECK( samplePoint[0] == Approx(1.0) );
        CIE_TEST_CHECK( samplePoint[1] == Approx(2.0) );
    }

    {
        CIE_TEST_CASE_INIT( "Box" )

        using Primitive = Box<2,double>;
        using Sampler = CornerSampler<Primitive>;

        Primitive primitive( Primitive::Point {-1.0, 0.0}, Primitive::Point {1.0, 2.0} );

        CIE_TEST_CHECK_NOTHROW( Sampler() );
        Sampler sampler;
        CIE_TEST_REQUIRE( sampler.size() == 4 );

        Primitive::Point samplePoint;
        CIE_TEST_CHECK_NOTHROW( samplePoint = sampler.getSamplePoint(primitive, 0) );
        CIE_TEST_CHECK( samplePoint[0] == Approx(-1.0) );
        CIE_TEST_CHECK( samplePoint[1] == Approx(0.0) );

        CIE_TEST_CHECK_NOTHROW( samplePoint = sampler.getSamplePoint(primitive, 1) );
        CIE_TEST_CHECK( samplePoint[0] == Approx(0.0) );
        CIE_TEST_CHECK( samplePoint[1] == Approx(0.0) );

        CIE_TEST_CHECK_NOTHROW( samplePoint = sampler.getSamplePoint(primitive, 2) );
        CIE_TEST_CHECK( samplePoint[0] == Approx(-1.0) );
        CIE_TEST_CHECK( samplePoint[1] == Approx(2.0) );

        CIE_TEST_CHECK_NOTHROW( samplePoint = sampler.getSamplePoint(primitive, 3) );
        CIE_TEST_CHECK( samplePoint[0] == Approx(0.0) );
        CIE_TEST_CHECK( samplePoint[1] == Approx(2.0) );
    }
}


} // namespace cie::geo