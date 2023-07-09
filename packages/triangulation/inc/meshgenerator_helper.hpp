#ifndef CIE_MESH_KERNEL_MESHGENERATOR_HELPER_HPP_
#define CIE_MESH_KERNEL_MESHGENERATOR_HELPER_HPP_

// --- Internal Includes ---
#include "packages/triangulation/inc/meshgenerator.hpp"

namespace cie::geo
{
namespace meshgeneratorhelper
{

/* Computes angle between three vertices a, b and c in polygon.
 * Given are the indices in region, which are again indices. Thus
 * the vertices are:
 *
 * a = vertices[region[v1]]
 * b = vertices[region[v2]]
 * c = vertices[region[v3]]
 */
double computeAngle( const Vertex2DVector& vertices,
                     const IndexVector& region,
                     Size v1,
                     Size v2,
                     Size v3 );


/* Given a vector of vertices and a region, the smallest
 * angle for a given division line is computed. That is,
 * all four angles between the division and the adjacent
 * vertices are computed and the minimum is returned.
 */
double computeSmallestAngle( const Vertex2DVector& vertices,
                             const IndexVector& region,
                             Size i,
                             Size j );


/*
 * Divides a region along given indices. The division line
 * is divided into segments with specified edgeLength. The
 * new vertices are added to the vertex vector. One of the
 * two new regions is appended to the region vector. The
 * that was divided is replaced by the second new region.
 */
void divideRegion( Vertex2DVector& vertices,
                   std::vector<IndexVector>& allRegions,
                   Size regionToDivide,
                   Size firstVertexId,
                   Size secondVertexId,
                   double edgeLength );


/*
 * Tries to divide given region according to the angleCriterium.
 * Returns true if division was possible or the passed region
 * has less than four vertices.
 */
bool attemptDivision( Vertex2DVector& vertices,
                      std::vector<IndexVector>& allRegions,
                      Size regionToDivide,
                      double angleCriterium,
                      double edgeLength );


// Removes empty regions and returns true if there are still non-empty ones
bool checkBreakingCriteria( std::vector<IndexVector>& allRegions );

/* Determines quality of new triangle a - b - c, where
 *
 * a = vertices[region[v1]]
 * b = vertices[region[v2]]
 * c = vertices[region[v3]]
 *
 * The criteria returned is computed as follows:
 * d1 = || a - b || (the first existing segment)
 * d2 = || c - b || (the second existing segment)
 * dn = || c - a || (the segment which would be new)
 *
 * q1 = min( d1, dn ) / max( d1, dn )
 * q2 = min( d2, dn ) / max( d2, dn )
 *
 * quality = min( q1, q2 )
 */
double chopQuality( const Vertex2DVector& vertices,
                    const IndexVector& region,
                    Size id1,
                    Size id2,
                    Size id3 );

// Chops first triangle that fulfills given criteria and returns true if a triangle was chopped.
bool chopTriangle( Triangulation& triangulation,
                   IndexVector& region,
                   double choppingAngleCriteria );

// Returns v1 - v2
Vector2D minus( const Vector2D& v1, const Vector2D& v2 );

// Returns || v ||
double length( const Vector2D& v );

// Returns distance between v1 and v2 by computing || v1 - v2 ||
double distance( const Vertex2D& v1, const Vertex2D& v2 );

// Checks input validity and sets parameters.edgeLength if it was zero.
void prepareForTriangulating( const Vertex2DVector& vertices,
                              const std::vector<IndexVector>& polygonRegions,
                              TriangulationParameters& parameters );

} // namespace meshgeneratorhelper
} // namespace cie::geo

#endif // MESHGENERATOR_HELPER_HPP_
