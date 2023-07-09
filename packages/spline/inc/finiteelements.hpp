#ifndef CIE_GEO_SPLINE_FINITEELEMENTS_HPP
#define CIE_GEO_SPLINE_FINITEELEMENTS_HPP

// --- Utility Includes ---
#include "packages/stl_extension/inc/StaticArray.hpp"

// --- STL Includes ---
#include <vector>
#include <tuple>
#include <array>
#include <functional>
#include <cstdint>

namespace cie::geo {

using ScipyIndexType = std::int32_t;
using SparseDataStructure = std::tuple<ScipyIndexType*, ScipyIndexType*, double*, size_t>;
using LinearSystem = std::pair<SparseDataStructure, std::vector<double>>;

//! given a vector of x and a vector of y values, return a vector of function evaluations
using SpatialFunction = std::function<double( double, double)>;

using IntegrationPoints = StaticArray<std::vector<double>, 2>;
using IntegrationPointProvider = std::function<IntegrationPoints( size_t )>;

class BSplineFiniteElementMesh
{
public:
    BSplineFiniteElementMesh( StaticArray<size_t, 2> numberOfElements,
                              StaticArray<size_t, 2> polynomialDegrees,
                              StaticArray<size_t, 2> continuities,
                              StaticArray<double, 2> lengths,
                              StaticArray<double, 2> origin,
                              IntegrationPointProvider integrationPointProvider );

    std::vector<size_t> boundaryDofIds( size_t normal, bool side ) const;

    LinearSystem assembleLinearSystem( const SpatialFunction& sourceFunction ) const;

    StaticArray<std::vector<double>, 2> knotVectors( ) const;

private:
    StaticArray<size_t, 2> numberOfElements_, polynomialDegrees_, continuities_;
    StaticArray<double, 2> lengths_, origin_;

    StaticArray<std::vector<double>, 2> knotVectors_;
    std::vector<std::vector<size_t>> locationMaps_;

    IntegrationPointProvider integrationPointProvdier_;

    StaticArray<double, 2> mapToGlobal( StaticArray<double, 2> localCoordinates,
                                       StaticArray<size_t, 2> elementIndices ) const;

    void evaluateBasisAt( StaticArray<double, 2> globalCoordinates,
                          StaticArray<size_t, 2> elementIndices,
                          std::vector<double>& shapesTarget,
                          std::vector<double>& shapesDxTarget,
                          std::vector<double>& shapesDyTarget ) const;
};

} // cie::geo

#endif // CIE_FINITEELEMENTS_HPP
