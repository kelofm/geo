// --- Utility Includes ---
#include "packages/testing/inc/essentials.hpp"

// --- Internal Includes ---
#include "packages/partitioning/inc/boundingBox.hpp"


namespace cie::geo {


class TestBoundableObject : public BoxBoundable<1,Double>
{
public:
    TestBoundableObject() :
        BoxBoundable<1,Double>(),
        _counter( 0 )
    { this->computeBoundingBox(); }

    Size counter() const
    { return this->_counter; }

    using BoxBoundable<1,Double>::geometryChanged;

private:
    void computeBoundingBoxImpl(TestBoundableObject::BoundingBox& r_box) noexcept override
    {
        this->_counter++;
        r_box.base() = { Double(this->_counter) };
        r_box.lengths() = { Double(this->_counter) };
    }

private:
    Size _counter;
};


template <class T>
concept NotBoxBoundable
= !concepts::BoxBoundable<T>;


template <concepts::BoxBoundable T>
bool isBoxBoundable(const T&)
{return true;}


template <class T>
requires (!concepts::BoxBoundable<T>)
bool isBoxBoundable(const T&)
{return false;}




CIE_TEST_CASE( "AbsBoundableObject", "[partitioning]" )
{
    CIE_TEST_CASE_INIT( "AbsBoundableObject" )


    CIE_TEST_REQUIRE_NOTHROW( TestBoundableObject() );
    TestBoundableObject test;

    CIE_TEST_CHECK( isBoxBoundable(test) );
    CIE_TEST_CHECK( !isBoxBoundable(0) );

    CIE_TEST_CHECK( test.counter() == 1 );

    CIE_TEST_REQUIRE_NOTHROW( boundingBox(test) );

    CIE_TEST_REQUIRE( boundingBox(test).base().size() > 0 );
    CIE_TEST_CHECK( boundingBox(test).base().size() == 1 );
    CIE_TEST_CHECK( boundingBox(test).base()[0] == Approx(1.0) );

    CIE_TEST_REQUIRE( boundingBox(test).lengths().size() > 0 );
    CIE_TEST_CHECK( boundingBox(test).lengths().size() == 1 );
    CIE_TEST_CHECK( boundingBox(test).lengths()[0] == Approx(1.0) );

    CIE_TEST_CHECK_NOTHROW( test.geometryChanged() );
    CIE_TEST_CHECK( test.counter() == 1 );

    CIE_TEST_REQUIRE_NOTHROW( boundingBox(test) );

    CIE_TEST_CHECK( test.counter() == 2 );

    CIE_TEST_REQUIRE( boundingBox(test).base().size() > 0 );
    CIE_TEST_CHECK( boundingBox(test).base().size() == 1 );
    CIE_TEST_CHECK( boundingBox(test).base()[0] == Approx(2.0) );

    CIE_TEST_REQUIRE( boundingBox(test).lengths().size() > 0 );
    CIE_TEST_CHECK( boundingBox(test).lengths().size() == 1 );
    CIE_TEST_CHECK( boundingBox(test).lengths()[0] == Approx(2.0) );
}


} // namespace cie::geo