// --- Utility Includes ---
#include "packages/testing/inc/essentials.hpp"
#include "packages/stl_extension/inc/StaticArray.hpp"

// --- LinAlg Includes ---
#include "packages/matrix/inc/DynamicEigenMatrix.hpp"

// --- Internal Includes ---
#include "packages/spline/inc/surface.hpp"

// --- STL Includes ---
#include <array>
#include <vector>


namespace cie::geo {


CIE_TEST_CASE( "Linear interpolation surface", "[surface]" )
{
    std::vector<double> knotVectorR{ 0.0, 0.0, 0.5, 1.0, 1.0 };
    std::vector<double> knotVectorS{ 0.0, 0.0, 0.5, 1.0, 1.0 };
    StaticArray<std::vector<double>, 2> knotVectors{ knotVectorR, knotVectorS };

    size_t numberOfSamplesR(11), numberOfSamplesS(9);

    VectorOfMatrices controlGrid;
    {
        linalg::DynamicEigenMatrix<double>::Wrapped xGrid(3, 3);
        xGrid << -1.0, -1.0, -1.0,
                  0.0,  0.0,  0.0,
                  1.0,  1.0,  1.0;
        linalg::DynamicEigenMatrix<double>::Wrapped yGrid(3, 3);
        yGrid << -1.0,  0.0,  1.0,
                 -1.0,  0.0,  1.0,
                 -1.0,  0.0,  1.0;
        linalg::DynamicEigenMatrix<double>::Wrapped zGrid(3, 3);
        zGrid <<  1.0,  1.0,  1.0,
                  1.0,  2.0,  1.0,
                  1.0,  1.0,  1.0;
        controlGrid.push_back(std::move(xGrid));
        controlGrid.push_back(std::move(yGrid));
        controlGrid.push_back(std::move(zGrid));
    }

    // Evaluate
    VectorOfMatrices C;
    CIE_TEST_REQUIRE_NOTHROW( C = evaluateSurface(knotVectors, controlGrid, { numberOfSamplesR, numberOfSamplesS }) );

    // Check sizes
    CIE_TEST_REQUIRE(C.size() == 3);
    CIE_TEST_REQUIRE(C[0].rowSize() == numberOfSamplesR);
    CIE_TEST_REQUIRE(C[1].rowSize() == numberOfSamplesR);
    CIE_TEST_REQUIRE(C[2].rowSize() == numberOfSamplesR);
    CIE_TEST_REQUIRE(C[0].columnSize() == numberOfSamplesS);
    CIE_TEST_REQUIRE(C[1].columnSize() == numberOfSamplesS);
    CIE_TEST_REQUIRE(C[2].columnSize() == numberOfSamplesS);

    // Check X-Y grid and surface values
    double X, Y, Z;

    for (size_t r = 0; r < numberOfSamplesR; ++r) {
        for (size_t s = 0; s < numberOfSamplesS; ++s) {

            X = 2 * ((double)r) / (numberOfSamplesR - 1) - 1.0;
            Y = 2 * ((double)s) / (numberOfSamplesS - 1) - 1.0;
            Z = 1 + (std::abs(X) - 1)*(std::abs(Y) - 1);

            CIE_TEST_CHECK(C[0](r, s) == Approx(X));
            CIE_TEST_CHECK(C[1](r, s) == Approx(Y));
            CIE_TEST_CHECK(C[2](r, s) == Approx(Z));

            //std::cout << Z << ",\t";        // Uncomment to print correct values

        }
        //std::cout << "\n";                // Uncomment to print correct values
    }

} // CIE_TEST_CASE("Linear interpolation surface")










CIE_TEST_CASE( "Cubic-linear interpolation surface", "[surface]" )
{

    std::vector<double> knotVectorR{ 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 1.0 };
    std::vector<double> knotVectorS{ 0.0, 0.0, 0.5, 1.0, 1.0 };
    StaticArray<std::vector<double>, 2> knotVectors{ knotVectorR, knotVectorS };

    size_t numberOfSamplesR(7), numberOfSamplesS(5);

    VectorOfMatrices controlGrid;
    {
        linalg::DynamicEigenMatrix<double>::Wrapped xGrid(4, 3);
        xGrid << -3.0, -3.0, -3.0,
                 -1.0, -1.0, -1.0,
                  1.0,  1.0,  1.0,
                  3.0,  3.0,  3.0;
        linalg::DynamicEigenMatrix<double>::Wrapped yGrid(4, 3);
        yGrid << -1.0,  0.0,  1.0,
                 -1.0,  0.0,  1.0,
                 -1.0,  0.0,  1.0,
                 -1.0,  0.0,  1.0;
        linalg::DynamicEigenMatrix<double>::Wrapped zGrid(4, 3);
        zGrid <<  1.0,  1.0,   1.0,
                  1.0,  49.0,  1.0,
                  1.0,  49.0,  1.0,
                  1.0,  1.0,   1.0;
        controlGrid.push_back(std::move(xGrid));
        controlGrid.push_back(std::move(yGrid));
        controlGrid.push_back(std::move(zGrid));
    }

    // Evaluate
    VectorOfMatrices C;
    CIE_TEST_REQUIRE_NOTHROW(C = evaluateSurface(knotVectors, controlGrid, { numberOfSamplesR, numberOfSamplesS }));

    // Check sizes
    CIE_TEST_REQUIRE(C.size() == 3);
    CIE_TEST_REQUIRE(C[0].rowSize() == numberOfSamplesR);
    CIE_TEST_REQUIRE(C[1].rowSize() == numberOfSamplesR);
    CIE_TEST_REQUIRE(C[2].rowSize() == numberOfSamplesR);
    CIE_TEST_REQUIRE(C[0].columnSize() == numberOfSamplesS);
    CIE_TEST_REQUIRE(C[1].columnSize() == numberOfSamplesS);
    CIE_TEST_REQUIRE(C[2].columnSize() == numberOfSamplesS);

    // Check X-Y grid and surface values
    double X, Y, Z, increment(0.0);

    for (size_t r = 0; r < numberOfSamplesR; ++r) {


        for (size_t s = 0; s < numberOfSamplesS; ++s) {

            X = 6 * ((double)r) / (numberOfSamplesR - 1) - 3.0;
            Y = 2 * ((double)s) / (numberOfSamplesS - 1) - 1.0;
            Z = 1 + (2 - std::abs((double)s - 2)) * increment;

            CIE_TEST_CHECK(C[0](r, s) == Approx(X));
            CIE_TEST_CHECK(C[1](r, s) == Approx(Y));
            CIE_TEST_CHECK(C[2](r, s) == Approx(Z));

            //std::cout << Z << ",\t";                // Uncomment to print correct values

        }

        increment += 10 - 4 * (double)r;
        //std::cout << "\n";                        // Uncomment to print correct values
    }



} // CIE_TEST_CASE("Cubic-linear interpolation surface")

} // cie::geo
