// --- Linalg Includes ---
#include "packages/types/inc/sparse.hpp"

// --- Utility Includes ---
#include "packages/macros/inc/exceptions.hpp"
#include "packages/stl_extension/inc/StaticArray.hpp"

// --- Internal Include ---
#include "packages/spline/inc/finiteelements.hpp"
#include "packages/spline/inc/basisfunctions.hpp"

// --- STL Includes ---
#include <cmath>
#include <algorithm>
#include <numeric>

namespace cie::geo {
namespace detail {

StaticArray<std::vector<double>, 2> constructOpenKnotVector(StaticArray<size_t, 2> numberOfElements,
                                                            StaticArray<size_t, 2> polynomialDegrees,
                                                            StaticArray<size_t, 2> continuities,
                                                            StaticArray<double, 2> lengths,
                                                            StaticArray<double, 2> origin )
{
    StaticArray<std::vector<double>, 2> knotVectors;

    for( size_t i = 0; i < 2; ++i )
    {
        size_t multiplicity = polynomialDegrees[i] - continuities[i];
        size_t numberOfInternalKnots = ( numberOfElements[i] - 1 ) * multiplicity;

        knotVectors[i].resize( numberOfInternalKnots + 2 * ( polynomialDegrees[i] + 1 ) );

        for( size_t iOpenKnot = 0; iOpenKnot < polynomialDegrees[i] + 1; ++iOpenKnot )
        {
            knotVectors[i][iOpenKnot] = origin[i];
            knotVectors[i][iOpenKnot + numberOfInternalKnots + polynomialDegrees[i] + 1] = origin[i] + lengths[i];
        }

        for( size_t iElement = 1; iElement < numberOfElements[i]; ++iElement )
        {
            for( size_t iKnotRepitition = 0; iKnotRepitition < multiplicity; ++iKnotRepitition )
            {
                size_t index = polynomialDegrees[i] + 1 + ( iElement - 1 ) * multiplicity;

                knotVectors[i][index + iKnotRepitition] = iElement * lengths[i] / numberOfElements[i] + origin[i];
            }
        }
    }

    return knotVectors;
}

std::vector<std::vector<size_t>> constructLocationMaps( StaticArray<size_t, 2> numberOfElements,
                                                        StaticArray<size_t, 2> polynomialDegrees,
                                                        StaticArray<size_t, 2> continuities,
                                                        size_t numberOfKnotsInY )
{
    size_t numberOfDofsInY = numberOfKnotsInY - polynomialDegrees[1] - 1;

    std::vector<std::vector<size_t>> locationMaps( numberOfElements[0] * numberOfElements[1] );

    // Setup location maps
    for( size_t iElement = 0; iElement < numberOfElements[0]; ++iElement )
    {
        for( size_t jElement = 0; jElement < numberOfElements[1]; ++jElement )
        {
            auto& locationMap = locationMaps[iElement * numberOfElements[1] + jElement];

            locationMap.reserve( ( polynomialDegrees[0] + 1 ) * ( polynomialDegrees[1] + 1 ) );

            for( size_t iDof = 0; iDof < polynomialDegrees[0] + 1; ++iDof )
            {
                for( size_t jDof = 0; jDof < polynomialDegrees[1] + 1; ++jDof )
                {
                    size_t dofIndexR = ( polynomialDegrees[0] - continuities[0] ) * iElement + iDof;
                    size_t dofIndexS = ( polynomialDegrees[1] - continuities[1] ) * jElement + jDof;

                    locationMap.push_back( dofIndexR * numberOfDofsInY + dofIndexS );

                } // for jDof
            } // for iDof
        } // for jElement
    } // for iElement

    return locationMaps;
}

} // namespace detail

BSplineFiniteElementMesh::BSplineFiniteElementMesh( StaticArray<size_t, 2> numberOfElements,
                                                    StaticArray<size_t, 2> polynomialDegrees,
                                                    StaticArray<size_t, 2> continuities,
                                                    StaticArray<double, 2> lengths,
                                                    StaticArray<double, 2> origin,
                                                    IntegrationPointProvider integrationPointProvider ) :
    numberOfElements_( numberOfElements ),
    polynomialDegrees_( polynomialDegrees ),
    continuities_( continuities ),
    lengths_( lengths ),
    origin_( origin ),
    integrationPointProvdier_( integrationPointProvider )
{
    knotVectors_ = detail::constructOpenKnotVector( numberOfElements, polynomialDegrees, continuities, lengths, origin );
    locationMaps_ = detail::constructLocationMaps( numberOfElements, polynomialDegrees, continuities, knotVectors_[1].size( ) );

} // constructor

StaticArray<double, 2> BSplineFiniteElementMesh::mapToGlobal( StaticArray<double, 2> localCoordinates,
                                                             StaticArray<size_t, 2> elementIndices ) const
{

    StaticArray<double, 2> globalCoordinates;

    for( size_t axis = 0; axis < 2; ++axis )
    {
        double step = lengths_[axis] / numberOfElements_[axis];

        double t0 = elementIndices[axis] * step + origin_[axis];
        double t1 = ( elementIndices[axis] + 1 ) * step + origin_[axis];

        globalCoordinates[axis] = ( localCoordinates[axis] + 1.0 ) / 2.0 * ( t1 - t0 ) + t0;
    }

    return globalCoordinates;
}

std::vector<size_t> BSplineFiniteElementMesh::boundaryDofIds( size_t normal, bool side ) const
{
    if (normal >= 2)
        CIE_THROW( std::runtime_error, "Invalid normal!" )

    StaticArray<size_t, 2> numberOfDofs { knotVectors_[0].size( ) - polynomialDegrees_[0] - 1,
                                         knotVectors_[1].size( ) - polynomialDegrees_[1] - 1 };

    StaticArray<size_t, 2> steps { 1, numberOfDofs[1] };
    StaticArray<size_t, 2> start { side * ( numberOfDofs[1] * ( numberOfDofs[0] - 1 ) ), // 0 or nx * (ny - 1)
                                  side * ( numberOfDofs[1] - 1 ) };                     // 0 or ny - 1

    std::vector<size_t> dofIndices( numberOfDofs[1 - normal] );

    for( size_t i = 0; i < numberOfDofs[1 - normal]; ++i )
    {
        dofIndices[i] = start[normal] + i * steps[normal];
    }

    return dofIndices;
}

StaticArray<std::vector<double>, 2> BSplineFiniteElementMesh::knotVectors( ) const
{
    return knotVectors_;
}

void BSplineFiniteElementMesh::evaluateBasisAt( StaticArray<double, 2> patchCoordinates,
                                                StaticArray<size_t, 2> elementIndices,
                                                std::vector<double>& shapesTarget,
                                                std::vector<double>& shapesDxTarget,
                                                std::vector<double>& shapesDyTarget ) const
{
    size_t numberOfDofs = ( polynomialDegrees_[0] + 1 ) * ( polynomialDegrees_[1] + 1 );

    shapesTarget.resize( numberOfDofs );
    shapesDxTarget.resize( numberOfDofs );
    shapesDyTarget.resize( numberOfDofs );

    size_t support0 = elementIndices[0] * ( polynomialDegrees_[0] - continuities_[0] );
    size_t support1 = elementIndices[1] * ( polynomialDegrees_[1] - continuities_[1] );

    // Compute tensor product
    for( size_t iDof = 0; iDof < polynomialDegrees_[0] + 1; ++iDof )
    {
        double Nr = evaluateBSplineBasis( patchCoordinates[0], support0 + iDof, polynomialDegrees_[0], knotVectors_[0] );
        double dNr = evaluateBSplineDerivative( patchCoordinates[0], support0 + iDof, polynomialDegrees_[0], knotVectors_[0] );

        for( size_t jDof = 0; jDof < polynomialDegrees_[1] + 1; ++jDof )
        {
            double Ns = evaluateBSplineBasis( patchCoordinates[1], support1 + jDof, polynomialDegrees_[1], knotVectors_[1] );
            double dNs = evaluateBSplineDerivative( patchCoordinates[1], support1 + jDof, polynomialDegrees_[1], knotVectors_[1] );

            size_t index = iDof * ( polynomialDegrees_[1] + 1 ) + jDof;

            shapesTarget[index] = Nr * Ns;
            shapesDxTarget[index] = dNr * Ns;
            shapesDyTarget[index] = Nr * dNs;
        }
    }
}

LinearSystem BSplineFiniteElementMesh::assembleLinearSystem( const SpatialFunction& sourceFunction ) const
{
    linalg::CompressedSparseRowMatrix<ScipyIndexType> globalMatrix;

    globalMatrix.allocate( locationMaps_ );

    size_t totalNumberOfDofs = locationMaps_.back( ).back( ) + 1;

    std::vector<double> globalSourceVector( totalNumberOfDofs, 0.0 );
    std::vector<double> N, dNdx, dNdy;

    // Integrate with order p + 1
    StaticArray<size_t, 2> integrationOrders { polynomialDegrees_[0] + 1, polynomialDegrees_[1] + 1 };

    // Callback to numpy.polynomial.legendre.leggauss which we set up in python
    const IntegrationPoints& integrationPointsI = integrationPointProvdier_( integrationOrders[0] );
    const IntegrationPoints& integrationPointsJ = integrationPointProvdier_( integrationOrders[1] );

    size_t numberOfElementDofs = ( polynomialDegrees_[0] + 1 ) * ( polynomialDegrees_[1] + 1 );

    for( size_t iElement = 0; iElement < numberOfElements_[0]; ++iElement )
    {
        for( size_t jElement = 0; jElement < numberOfElements_[1]; ++jElement )
        {
            linalg::Matrix elementMatrix( numberOfElementDofs, numberOfElementDofs );

            // Location maps are stored as vector, so we have to find the correct index
            auto locationMap = locationMaps_[iElement * numberOfElements_[1] + jElement];

            for( size_t iIntegrationPoint = 0; iIntegrationPoint < integrationOrders[0]; ++iIntegrationPoint )
            {
                for( size_t jIntegrationPoint = 0; jIntegrationPoint < integrationOrders[1]; ++jIntegrationPoint )
                {
                    StaticArray<double, 2> localCoordinates { integrationPointsI[0][iIntegrationPoint],
                                                             integrationPointsJ[0][jIntegrationPoint] };

                    StaticArray<size_t, 2> elementIndices { iElement, jElement };
                    StaticArray<double, 2> globalCoordinates = mapToGlobal( localCoordinates, elementIndices );

                    evaluateBasisAt( globalCoordinates, elementIndices, N, dNdx, dNdy );

                    double weight = integrationPointsI[1][iIntegrationPoint] * integrationPointsJ[1][jIntegrationPoint];
                    double detJ = 4.0 * ( lengths_[0] / numberOfElements_[0] ) * ( lengths_[1] / numberOfElements_[1] );

                    // Callback to python
                    double sourceValue = sourceFunction( globalCoordinates[0], globalCoordinates[1] );

                    // compute Ke_ij += dot( grad( N_i ), grad ( N_j ) ) and Fe_i += N_i * f
                    for( size_t iDof = 0; iDof < numberOfElementDofs; ++iDof )
                    {
                        for( size_t jDof = 0; jDof < numberOfElementDofs; ++jDof )
                        {
                            elementMatrix( iDof, jDof ) += ( dNdx[iDof] * dNdx[jDof] + dNdy[iDof] * dNdy[jDof] ) * weight * detJ;

                        } // for jDof

                        globalSourceVector[locationMap[iDof]] += N[iDof] * sourceValue * weight * detJ;
                    } // for iDof

                } // for jIntegrationPoint
            } // for iIntegrationPoint

            globalMatrix.scatter( elementMatrix, locationMap );

        } // for jElement
    } // for iElement

    return { globalMatrix.release( ), globalSourceVector };
}

} // cie::geo