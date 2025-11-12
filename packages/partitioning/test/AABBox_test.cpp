// --- Utility Includes ---
#include "packages/testing/inc/essentials.hpp"

// --- Internal Includes ---
#include "packages/partitioning/inc/AABBox.hpp"


namespace cie::geo {


CIE_TEST_CASE("AABBox", "[partitioning]")
{
    CIE_TEST_CASE_INIT("AABBox")

    {
        CIE_TEST_CASE_INIT("2D floating point")

        using BoundingBox = AABBox<2,Double>;

        BoundingBox testBox;
        BoundingBox refBox(
            BoundingBox::Point {0.0, 0.0},
            BoundingBox::Point {1.0, 1.0}
        );

        // Define lengths
        BoundingBox::Coordinate L  = 1.0;
        BoundingBox::Coordinate L2 = 2.0 * L;
        BoundingBox::Coordinate hL = L / 2.0;
        BoundingBox::Coordinate l  = 0.1;
        BoundingBox::Coordinate hl = l / 2.0;

        /* --- y: negative side --- */

        // x: negative side
        // -> completely outside
        testBox = BoundingBox(BoundingBox::Point {-L2, -L2 },
                              BoundingBox::Point {l, l });
        CIE_TEST_CHECK(refBox.contains(testBox) == false);
        CIE_TEST_CHECK(refBox.intersects(testBox) == false);

        // x: intersect negative side
        // -> completely outside
        testBox = BoundingBox(BoundingBox::Point { -hl, -L2 },
                              BoundingBox::Point { l, l });
        CIE_TEST_CHECK(refBox.contains(testBox) == false);
        CIE_TEST_CHECK(refBox.intersects(testBox) == false);

        // x: inside
        // -> completely outside
        testBox = BoundingBox(BoundingBox::Point { hL-hl, -L2 },
                              BoundingBox::Point { l, l });
        CIE_TEST_CHECK(refBox.contains(testBox) == false);
        CIE_TEST_CHECK(refBox.intersects(testBox) == false);

        // x: intersect positive side
        // -> completely outside
        testBox = BoundingBox(BoundingBox::Point { L-hl, -L2 },
                              BoundingBox::Point { l, l });
        CIE_TEST_CHECK(refBox.contains(testBox) == false);
        CIE_TEST_CHECK(refBox.intersects(testBox) == false);

        // x: positive side
        // -> completely outside
        testBox = BoundingBox(BoundingBox::Point { L2, -L2 },
                              BoundingBox::Point { l, l });
        CIE_TEST_CHECK(refBox.contains(testBox) == false);
        CIE_TEST_CHECK(refBox.intersects(testBox) == false);

        /* --- y: intersect negative side --- */

        // x: negative side
        // -> completely outside
        testBox = BoundingBox(BoundingBox::Point { -L2, -hl },
                              BoundingBox::Point { l, l });
        CIE_TEST_CHECK(refBox.contains(testBox) == false);
        CIE_TEST_CHECK(refBox.intersects(testBox) == false);

        // x: intersect negative side
        // -> completely outside
        testBox = BoundingBox(BoundingBox::Point { -hl, -hl },
                              BoundingBox::Point { l, l });
        CIE_TEST_CHECK(refBox.contains(testBox) == false);
        CIE_TEST_CHECK(refBox.intersects(testBox) == true);

        // x: inside
        // -> completely outside
        testBox = BoundingBox(BoundingBox::Point { hL-hl, -hl },
                              BoundingBox::Point { l, l });
        CIE_TEST_CHECK(refBox.contains(testBox) == false);
        CIE_TEST_CHECK(refBox.intersects(testBox) == true);

        // x: intersect positive side
        // -> completely outside
        testBox = BoundingBox(BoundingBox::Point { L-hl, -hl },
                              BoundingBox::Point { l, l });
        CIE_TEST_CHECK(refBox.contains(testBox) == false);
        CIE_TEST_CHECK(refBox.intersects(testBox) == true);

        // x: positive side
        // -> completely outside
        testBox = BoundingBox(BoundingBox::Point { L2, -hl },
                              BoundingBox::Point { l, l });
        CIE_TEST_CHECK(refBox.contains(testBox) == false);
        CIE_TEST_CHECK(refBox.intersects(testBox) == false);

        /* --- y: inside --- */

        // x: negative side
        // -> completely outside
        testBox = BoundingBox(BoundingBox::Point { -L2, hL-hl },
                              BoundingBox::Point { l, l });
        CIE_TEST_CHECK(refBox.contains(testBox) == false);
        CIE_TEST_CHECK(refBox.intersects(testBox) == false);

        // x: intersect negative side
        // -> completely outside
        testBox = BoundingBox(BoundingBox::Point { -hl, hL-hl },
                              BoundingBox::Point { l, l });
        CIE_TEST_CHECK(refBox.contains(testBox) == false);
        CIE_TEST_CHECK(refBox.intersects(testBox) == true);

        // x: inside
        // -> completely outside
        testBox = BoundingBox(BoundingBox::Point { hL-hl, hL-hl },
                              BoundingBox::Point { l, l });
        CIE_TEST_CHECK(refBox.contains(testBox) == true);
        CIE_TEST_CHECK(refBox.intersects(testBox) == false);

        // x: intersect positive side
        // -> completely outside
        testBox = BoundingBox(BoundingBox::Point { L-hl, hL-hl },
                              BoundingBox::Point { l, l });
        CIE_TEST_CHECK(refBox.contains(testBox) == false);
        CIE_TEST_CHECK(refBox.intersects(testBox) == true);

        // x: positive side
        // -> completely outside
        testBox = BoundingBox(BoundingBox::Point { L2, hL-hl },
                              BoundingBox::Point { l, l });
        CIE_TEST_CHECK(refBox.contains(testBox) == false);
        CIE_TEST_CHECK(refBox.intersects(testBox) == false);

        /* --- y: intersect positive side --- */

        // x: negative side
        // -> completely outside
        testBox = BoundingBox(BoundingBox::Point { -L2, L-hl },
                              BoundingBox::Point { l, l });
        CIE_TEST_CHECK(refBox.contains(testBox) == false);
        CIE_TEST_CHECK(refBox.intersects(testBox) == false);

        // x: intersect negative side
        // -> completely outside
        testBox = BoundingBox(BoundingBox::Point { -hl, L-hl },
                              BoundingBox::Point { l, l });
        CIE_TEST_CHECK(refBox.contains(testBox) == false);
        CIE_TEST_CHECK(refBox.intersects(testBox) == true);

        // x: inside
        // -> completely outside
        testBox = BoundingBox(BoundingBox::Point { hL-hl, L-hl },
                              BoundingBox::Point { l, l });
        CIE_TEST_CHECK(refBox.contains(testBox) == false);
        CIE_TEST_CHECK(refBox.intersects(testBox) == true);

        // x: intersect positive side
        // -> completely outside
        testBox = BoundingBox(BoundingBox::Point { L-hl, L-hl },
                              BoundingBox::Point { l, l });
        CIE_TEST_CHECK(refBox.contains(testBox) == false);
        CIE_TEST_CHECK(refBox.intersects(testBox) == true);

        // x: positive side
        // -> completely outside
        testBox = BoundingBox(BoundingBox::Point { L2, L-hl },
                              BoundingBox::Point { l, l });
        CIE_TEST_CHECK(refBox.contains(testBox) == false);
        CIE_TEST_CHECK(refBox.intersects(testBox) == false);

        /* --- y: positive side --- */

        // x: negative side
        // -> completely outside
        testBox = BoundingBox(BoundingBox::Point { -L2, L2 },
                              BoundingBox::Point { l, l });
        CIE_TEST_CHECK(refBox.contains(testBox) == false);
        CIE_TEST_CHECK(refBox.intersects(testBox) == false);

        // x: intersect negative side
        // -> completely outside
        testBox = BoundingBox(BoundingBox::Point { -hl, L2 },
                              BoundingBox::Point { l, l });
        CIE_TEST_CHECK(refBox.contains(testBox) == false);
        CIE_TEST_CHECK(refBox.intersects(testBox) == false);

        // x: inside
        // -> completely outside
        testBox = BoundingBox(BoundingBox::Point { hL-hl, L2 },
                              BoundingBox::Point { l, l });
        CIE_TEST_CHECK(refBox.contains(testBox) == false);
        CIE_TEST_CHECK(refBox.intersects(testBox) == false);

        // x: intersect positive side
        // -> completely outside
        testBox = BoundingBox(BoundingBox::Point { L-hl, L2 },
                              BoundingBox::Point { l, l });
        CIE_TEST_CHECK(refBox.contains(testBox) == false);
        CIE_TEST_CHECK(refBox.intersects(testBox) == false);

        // x: positive side
        // -> completely outside
        testBox = BoundingBox(BoundingBox::Point { L2, L2 },
                              BoundingBox::Point { l, l });
        CIE_TEST_CHECK(refBox.contains(testBox) == false);
        CIE_TEST_CHECK(refBox.intersects(testBox) == false);

        /* --- Marginal cases --- */

        // Touch negative side from the inside
        testBox = BoundingBox(BoundingBox::Point { 0.0, hL-hl },
                              BoundingBox::Point { l, l });
        CIE_TEST_CHECK(refBox.contains(testBox) == true);
        CIE_TEST_CHECK(refBox.intersects(testBox) == false);

        // Touch positive side from the outside
        testBox = BoundingBox(BoundingBox::Point { L, hL-hl },
                              BoundingBox::Point { l, l });
        CIE_TEST_CHECK(refBox.contains(testBox) == false);
        CIE_TEST_CHECK(refBox.intersects(testBox) == false);

        // Touch negative corner from the inside
        testBox = BoundingBox(BoundingBox::Point { 0.0, 0.0 },
                              BoundingBox::Point { l, l });
        CIE_TEST_CHECK(refBox.contains(testBox) == true);
        CIE_TEST_CHECK(refBox.intersects(testBox) == false);

        // Touch positive corner from the outside
        testBox = BoundingBox(BoundingBox::Point { L, L },
                              BoundingBox::Point { l, l });
        CIE_TEST_CHECK(refBox.contains(testBox) == false);
        CIE_TEST_CHECK(refBox.intersects(testBox) == false);

        /* --- Box expansion --- */

        testBox = BoundingBox(BoundingBox::Point { 0.0, 0.0 },
                              BoundingBox::Point { L2, l });
        CIE_TEST_CHECK_NOTHROW(refBox.include(testBox));
        CIE_TEST_CHECK(refBox.base()[0] == Approx(0.0));
        CIE_TEST_CHECK(refBox.base()[1] == Approx(0.0));
        CIE_TEST_CHECK(refBox.lengths()[0] == Approx(L2));
        CIE_TEST_CHECK(refBox.lengths()[1] == Approx(L));

        testBox = BoundingBox(BoundingBox::Point { -l, -hl },
                              BoundingBox::Point { l, L2 });
        CIE_TEST_CHECK_NOTHROW(refBox.include(testBox));
        CIE_TEST_CHECK(refBox.base()[0] == Approx(-l));
        CIE_TEST_CHECK(refBox.base()[1] == Approx(-hl));
        CIE_TEST_CHECK(refBox.lengths()[0] == Approx(L2 + l));
        CIE_TEST_CHECK(refBox.lengths()[1] == Approx(L2));
    }
}


} // namespace cie::geo