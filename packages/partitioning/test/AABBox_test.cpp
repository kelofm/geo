// --- Utility Includes ---
#include "packages/testing/inc/essentials.hpp"

// --- Internal Includes ---
#include "packages/partitioning/inc/AABBox.hpp"


namespace cie::geo {


CIE_TEST_CASE( "AABBox", "[partitioning]" )
{
    CIE_TEST_CASE_INIT( "AABBox" )

    {
        CIE_TEST_CASE_INIT( "2D floating point" )

        using BoundingBox = AABBox<2,Double>;

        BoundingBox testBox;
        BoundingBox referenceBox(
            { 0.0, 0.0 },
            { 1.0, 1.0 }
        );

        // Define lengths
        typename BoundingBox::Coordinate L  = 1.0;
        typename BoundingBox::Coordinate L2 = 2.0 * L;
        typename BoundingBox::Coordinate hL = L / 2.0;
        typename BoundingBox::Coordinate l  = 0.1;
        typename BoundingBox::Coordinate hl = l / 2.0;

        /* --- y: negative side --- */

        // x: negative side
        // -> completely outside
        testBox = BoundingBox( { -L2, -L2 },
                               { l, l } );
        CIE_TEST_CHECK( referenceBox.contains(testBox) == false );
        CIE_TEST_CHECK( referenceBox.intersects(testBox) == false );

        // x: intersect negative side
        // -> completely outside
        testBox = BoundingBox( { -hl, -L2 },
                               { l, l } );
        CIE_TEST_CHECK( referenceBox.contains(testBox) == false );
        CIE_TEST_CHECK( referenceBox.intersects(testBox) == false );

        // x: inside
        // -> completely outside
        testBox = BoundingBox( { hL-hl, -L2 },
                               { l, l } );
        CIE_TEST_CHECK( referenceBox.contains(testBox) == false );
        CIE_TEST_CHECK( referenceBox.intersects(testBox) == false );

        // x: intersect positive side
        // -> completely outside
        testBox = BoundingBox( { L-hl, -L2 },
                               { l, l } );
        CIE_TEST_CHECK( referenceBox.contains(testBox) == false );
        CIE_TEST_CHECK( referenceBox.intersects(testBox) == false );

        // x: positive side
        // -> completely outside
        testBox = BoundingBox( { L2, -L2 },
                               { l, l } );
        CIE_TEST_CHECK( referenceBox.contains(testBox) == false );
        CIE_TEST_CHECK( referenceBox.intersects(testBox) == false );

        /* --- y: intersect negative side --- */

        // x: negative side
        // -> completely outside
        testBox = BoundingBox( { -L2, -hl },
                               { l, l } );
        CIE_TEST_CHECK( referenceBox.contains(testBox) == false );
        CIE_TEST_CHECK( referenceBox.intersects(testBox) == false );

        // x: intersect negative side
        // -> completely outside
        testBox = BoundingBox( { -hl, -hl },
                               { l, l } );
        CIE_TEST_CHECK( referenceBox.contains(testBox) == false );
        CIE_TEST_CHECK( referenceBox.intersects(testBox) == true );

        // x: inside
        // -> completely outside
        testBox = BoundingBox( { hL-hl, -hl },
                               { l, l } );
        CIE_TEST_CHECK( referenceBox.contains(testBox) == false );
        CIE_TEST_CHECK( referenceBox.intersects(testBox) == true );

        // x: intersect positive side
        // -> completely outside
        testBox = BoundingBox( { L-hl, -hl },
                               { l, l } );
        CIE_TEST_CHECK( referenceBox.contains(testBox) == false );
        CIE_TEST_CHECK( referenceBox.intersects(testBox) == true );

        // x: positive side
        // -> completely outside
        testBox = BoundingBox( { L2, -hl },
                               { l, l } );
        CIE_TEST_CHECK( referenceBox.contains(testBox) == false );
        CIE_TEST_CHECK( referenceBox.intersects(testBox) == false );

        /* --- y: inside --- */

        // x: negative side
        // -> completely outside
        testBox = BoundingBox( { -L2, hL-hl },
                               { l, l } );
        CIE_TEST_CHECK( referenceBox.contains(testBox) == false );
        CIE_TEST_CHECK( referenceBox.intersects(testBox) == false );

        // x: intersect negative side
        // -> completely outside
        testBox = BoundingBox( { -hl, hL-hl },
                               { l, l } );
        CIE_TEST_CHECK( referenceBox.contains(testBox) == false );
        CIE_TEST_CHECK( referenceBox.intersects(testBox) == true );

        // x: inside
        // -> completely outside
        testBox = BoundingBox( { hL-hl, hL-hl },
                               { l, l } );
        CIE_TEST_CHECK( referenceBox.contains(testBox) == true );
        CIE_TEST_CHECK( referenceBox.intersects(testBox) == false );

        // x: intersect positive side
        // -> completely outside
        testBox = BoundingBox( { L-hl, hL-hl },
                               { l, l } );
        CIE_TEST_CHECK( referenceBox.contains(testBox) == false );
        CIE_TEST_CHECK( referenceBox.intersects(testBox) == true );

        // x: positive side
        // -> completely outside
        testBox = BoundingBox( { L2, hL-hl },
                               { l, l } );
        CIE_TEST_CHECK( referenceBox.contains(testBox) == false );
        CIE_TEST_CHECK( referenceBox.intersects(testBox) == false );

        /* --- y: intersect positive side --- */

        // x: negative side
        // -> completely outside
        testBox = BoundingBox( { -L2, L-hl },
                               { l, l } );
        CIE_TEST_CHECK( referenceBox.contains(testBox) == false );
        CIE_TEST_CHECK( referenceBox.intersects(testBox) == false );

        // x: intersect negative side
        // -> completely outside
        testBox = BoundingBox( { -hl, L-hl },
                               { l, l } );
        CIE_TEST_CHECK( referenceBox.contains(testBox) == false );
        CIE_TEST_CHECK( referenceBox.intersects(testBox) == true );

        // x: inside
        // -> completely outside
        testBox = BoundingBox( { hL-hl, L-hl },
                               { l, l } );
        CIE_TEST_CHECK( referenceBox.contains(testBox) == false );
        CIE_TEST_CHECK( referenceBox.intersects(testBox) == true );

        // x: intersect positive side
        // -> completely outside
        testBox = BoundingBox( { L-hl, L-hl },
                               { l, l } );
        CIE_TEST_CHECK( referenceBox.contains(testBox) == false );
        CIE_TEST_CHECK( referenceBox.intersects(testBox) == true );

        // x: positive side
        // -> completely outside
        testBox = BoundingBox( { L2, L-hl },
                               { l, l } );
        CIE_TEST_CHECK( referenceBox.contains(testBox) == false );
        CIE_TEST_CHECK( referenceBox.intersects(testBox) == false );

        /* --- y: positive side --- */

        // x: negative side
        // -> completely outside
        testBox = BoundingBox( { -L2, L2 },
                               { l, l } );
        CIE_TEST_CHECK( referenceBox.contains(testBox) == false );
        CIE_TEST_CHECK( referenceBox.intersects(testBox) == false );

        // x: intersect negative side
        // -> completely outside
        testBox = BoundingBox( { -hl, L2 },
                               { l, l } );
        CIE_TEST_CHECK( referenceBox.contains(testBox) == false );
        CIE_TEST_CHECK( referenceBox.intersects(testBox) == false );

        // x: inside
        // -> completely outside
        testBox = BoundingBox( { hL-hl, L2 },
                               { l, l } );
        CIE_TEST_CHECK( referenceBox.contains(testBox) == false );
        CIE_TEST_CHECK( referenceBox.intersects(testBox) == false );

        // x: intersect positive side
        // -> completely outside
        testBox = BoundingBox( { L-hl, L2 },
                               { l, l } );
        CIE_TEST_CHECK( referenceBox.contains(testBox) == false );
        CIE_TEST_CHECK( referenceBox.intersects(testBox) == false );

        // x: positive side
        // -> completely outside
        testBox = BoundingBox( { L2, L2 },
                               { l, l } );
        CIE_TEST_CHECK( referenceBox.contains(testBox) == false );
        CIE_TEST_CHECK( referenceBox.intersects(testBox) == false );

        /* --- Marginal cases --- */

        // Touch negative side from the inside
        testBox = BoundingBox( { 0.0, hL-hl },
                               { l, l } );
        CIE_TEST_CHECK( referenceBox.contains(testBox) == true );
        CIE_TEST_CHECK( referenceBox.intersects(testBox) == false );

        // Touch positive side from the outside
        testBox = BoundingBox( { L, hL-hl },
                               { l, l } );
        CIE_TEST_CHECK( referenceBox.contains(testBox) == false );
        CIE_TEST_CHECK( referenceBox.intersects(testBox) == false );

        // Touch negative corner from the inside
        testBox = BoundingBox( { 0.0, 0.0 },
                               { l, l } );
        CIE_TEST_CHECK( referenceBox.contains(testBox) == true );
        CIE_TEST_CHECK( referenceBox.intersects(testBox) == false );

        // Touch positive corner from the outside
        testBox = BoundingBox( { L, L },
                               { l, l } );
        CIE_TEST_CHECK( referenceBox.contains(testBox) == false );
        CIE_TEST_CHECK( referenceBox.intersects(testBox) == false );

        /* --- Box expansion --- */

        testBox = BoundingBox( { 0.0, 0.0 },
                               { L2, l } );
        CIE_TEST_CHECK_NOTHROW( referenceBox.include(testBox) );
        CIE_TEST_CHECK( referenceBox.base()[0] == Approx(0.0) );
        CIE_TEST_CHECK( referenceBox.base()[1] == Approx(0.0) );
        CIE_TEST_CHECK( referenceBox.lengths()[0] == Approx(L2) );
        CIE_TEST_CHECK( referenceBox.lengths()[1] == Approx(L) );

        testBox = BoundingBox( { -l, -hl },
                               { l, L2 } );
        CIE_TEST_CHECK_NOTHROW( referenceBox.include(testBox) );
        CIE_TEST_CHECK( referenceBox.base()[0] == Approx(-l) );
        CIE_TEST_CHECK( referenceBox.base()[1] == Approx(-hl) );
        CIE_TEST_CHECK( referenceBox.lengths()[0] == Approx(L2 + l) );
        CIE_TEST_CHECK( referenceBox.lengths()[1] == Approx(L2) );
    }
}


} // namespace cie::geo