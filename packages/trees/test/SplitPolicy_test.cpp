// --- Utility Includes ---
#include "packages/testing/inc/essentials.hpp"
#include "packages/stl_extension/inc/StaticArray.hpp"

// --- Internal Includes ---
#include "packages/trees/inc/MidPointSplitPolicy.hpp"
#include "packages/trees/inc/LinearSplitPolicy.hpp"
#include "packages/trees/inc/WeightedSplitPolicy.hpp"

// --- STL Includes ---
#include <array>
#include <vector>
#include <deque>


namespace cie::geo {


CIE_TEST_CASE( "MidPointSplitPolicy", "[trees]" )
{
    CIE_TEST_CASE_INIT( "MidPointSplitPolicy" )

    const Size Dimension    = 2;
    using CoordinateType    = Double;
    using PointType         = StaticArray<CoordinateType,Dimension>;
    using ValueType         = Size;

    std::vector<PointType> points;
    points.push_back( PointType({ 0.0, 0.0 }) );
    points.push_back( PointType({ 1.0, 0.0 }) );
    points.push_back( PointType({ 0.0, 1.0 }) );
    points.push_back( PointType({ 1.0, 1.0 }) );

    std::deque<ValueType> values { 0, 1, 2, 3 };

    using Splitter = MidPointSplitPolicy< typename std::vector<PointType>::iterator,
                                          typename std::deque<ValueType>::iterator >;
    Splitter splitter;
    PointType splitPoint = splitter( values.begin(),
                                     values.end(),
                                     points.begin() );

    CIE_TEST_CHECK( splitPoint.size() == Dimension );
    for ( const auto& r_component : splitPoint )
        CIE_TEST_CHECK( r_component == Approx(0.5) );
}



CIE_TEST_CASE( "LinearSplitPolicy", "[trees]" )
{
    CIE_TEST_CASE_INIT( "LinearSplitPolicy" )

    const Size Dimension    = 2;
    using CoordinateType    = Double;
    using PointType         = StaticArray<CoordinateType,Dimension>;
    using ValueType         = int;

    std::vector<PointType> points;
    points.push_back( PointType({ 0.0, 0.0 }) );
    points.push_back( PointType({ 1.0, 0.0 }) );
    points.push_back( PointType({ 0.0, 1.0 }) );
    points.push_back( PointType({ 1.0, 1.0 }) );

    std::deque<ValueType> values { -1, 2, 3, -3 };

    using Splitter = LinearSplitPolicy< typename std::vector<PointType>::iterator,
                                        typename std::deque<ValueType>::iterator >;
    Splitter splitter;
    PointType splitPoint = splitter( values.begin(),
                                     values.end(),
                                     points.begin() );

    CIE_TEST_REQUIRE( splitPoint.size() == Dimension );
    CIE_TEST_CHECK( splitPoint[0] == Approx( 2.0/3.0 ) );
    CIE_TEST_CHECK( splitPoint[1] == Approx( 1.0 ) );
}


} // namespace cie::geo