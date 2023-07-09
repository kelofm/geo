// --- Utility Includes ---
#include "packages/macros/inc/exceptions.hpp"
#include "packages/stl_extension/inc/StaticArray.hpp"

// --- Internal Includes ---
#include "packages/triangulation/inc/meshgenerator_helper.hpp"

// --- STL Includes ---
#include <cmath>
#include <numeric>
#include <algorithm>
#include <stdexcept>


namespace cie::geo {
namespace meshgeneratorhelper {


Vector2D minus( const Vector2D& v1, const Vector2D& v2 )
{
  return { v1[0] - v2[0], v1[1] - v2[1] };
}

double length( const Vector2D& v )
{
  return std::sqrt( v[0] * v[0] + v[1] * v[1] );
}

double distance( const Vertex2D& v1, const Vertex2D& v2 )
{
  return length( minus( v2, v1 ) );
}

double computeAngle( const Vertex2DVector& vertices, const IndexVector& region, size_t v1, size_t v2, size_t v3 )
{
  double tolerance = 1.0e-8;

  auto axis1 = minus( vertices[region[v1]], vertices[region[v2]] );
  auto axis2 = minus( vertices[region[v3]], vertices[region[v2]] );

  double angle = std::inner_product( axis1.begin( ), axis1.end( ), axis2.begin( ), 0.0 );
  double lengths = length( axis1 ) * length( axis2 );

  angle /= lengths;

  if( angle > 1.0 - tolerance )
  {
    return 0.0;
  }
  else if( lengths > tolerance )
  {
    return std::acos( angle );
  }
  else
  {
    return 0.0;
  }
}

double computeSmallestAngle( const Vertex2DVector& vertices, const IndexVector& region, size_t i, size_t j )
{
  size_t numberOfVertices = region.size( );

  StaticArray<double, 4> angles
  {
    computeAngle( vertices, region, j, i, (i + 1) % numberOfVertices ),
    computeAngle( vertices, region, j, i, (i - 1) % numberOfVertices ),
    computeAngle( vertices, region, i, j, (j - 1) % numberOfVertices ),
    computeAngle( vertices, region, i, j, (j + 1) % numberOfVertices )
  };

  return *std::min_element( angles.begin( ), angles.end( ) );
}

void divideRegion( Vertex2DVector& vertices,
                   std::vector<IndexVector>& allRegions,
                   size_t regionToDivide,
                   size_t firstVertexId,
                   size_t secondVertexId,
                   double edgeLength )
{
  auto& region = allRegions[regionToDivide];

  auto firstVertex = vertices[region[firstVertexId]];

  auto line = minus( vertices[region[secondVertexId]], firstVertex );

  size_t numberOfVertices = vertices.size( );
  size_t numberOfSegments = static_cast<size_t>( std::round( length( line ) / edgeLength ) );

  if( numberOfSegments == 0 )
  {
    numberOfSegments = 1;
  }

  IndexVector newIds( numberOfSegments - 1 );

  for( size_t i = 1; i < numberOfSegments; ++i )
  {
    double ti = i / static_cast<double>( numberOfSegments );

    vertices.push_back( Vertex2D{ firstVertex[0] + ti * line[0], firstVertex[1] + ti * line[1] } );

    newIds[i - 1] = numberOfVertices + i - 1;
  } // for i

  // copy [id1, id2] into a new vector and appended it to allRegions
  allRegions.emplace_back( region.begin( ) + firstVertexId, region.begin( ) + secondVertexId + 1 );

  auto& region1 = allRegions[regionToDivide]; // The region reference from above is invalid as vector has reallocated
  auto& region2 = allRegions.back( );

  // remove [id1, id2] from region and add new ids instead
  region1.erase( region1.begin( ) + firstVertexId + 1, region1.begin( ) + secondVertexId );
  region1.insert( region1.begin( ) + firstVertexId + 1, newIds.begin( ), newIds.end( ) );

  // add to region2 and reverse the order
  region2.insert( region2.end( ), newIds.begin( ), newIds.end( ) );
  std::reverse( region2.end( ) - numberOfSegments + 1, region2.end( ) );

  if( region1.size( ) < 3 || region2.size( ) < 3 )
      CIE_THROW( std::runtime_error, "Regions too small after division" )
} // divideRegion

bool attemptDivision( Vertex2DVector& vertices,
                      std::vector<IndexVector>& allRegions,
                      size_t regionToDivide,
                      double angleCriterium,
                      double edgeLength )
{
  size_t numberOfVertices = allRegions[regionToDivide].size( );

  if( numberOfVertices < 4 )
  {
    return true;
  }

  for( size_t i = 0; i < numberOfVertices; ++i )
  {
    for( size_t j = i + 2; j < numberOfVertices; ++j )
    {
      if( computeSmallestAngle( vertices, allRegions[regionToDivide], i, j ) > angleCriterium && !( i == 0 && j == numberOfVertices - 1 )  )
      {
        divideRegion(vertices, allRegions, regionToDivide, i, j, edgeLength );

        return true;
      }
    } // for j
  } // for i

  return false;
}

bool checkBreakingCriteria( std::vector<IndexVector>& allRegions )
{
  auto emptyRegion = std::remove( allRegions.begin( ), allRegions.end( ), IndexVector{ } );

  while( emptyRegion != allRegions.end( ) )
  {
    allRegions.erase( emptyRegion );

    emptyRegion = std::remove( allRegions.begin( ), allRegions.end( ), IndexVector{ } );
  }

  return !allRegions.empty( );
}

double chopQuality( const Vertex2DVector& vertices,
                    const IndexVector& region,
                    size_t id1,
                    size_t id2,
                    size_t id3 )
{
  double lengthOfLine0 = distance( vertices[region[id1]], vertices[region[id2]] );
  double lengthOfLine1 = distance( vertices[region[id2]], vertices[region[id3]] );
  double lengthOfNewLine = distance( vertices[region[id1]], vertices[region[id3]] );

  double ratio1 = std::min( lengthOfLine0, lengthOfNewLine ) / std::max( lengthOfLine0, lengthOfNewLine );
  double ratio2 = std::min( lengthOfLine1, lengthOfNewLine ) / std::max( lengthOfLine1, lengthOfNewLine );

  return std::min( ratio1, ratio2 );
}

bool chopTriangle( Triangulation& triangulation,
                   IndexVector& region,
                   double choppingCriteria )
{
  const auto& vertices = triangulation.first;
  auto& connectivity = triangulation.second;

  size_t numberOfVertices = region.size( );

  if( numberOfVertices == 3 )
  {
    connectivity.push_back( TriangleConnectivity{ region[0], region[1], region[2] } );

    region.erase( region.begin( ), region.end( ) );

    return false;
  }
  else if( numberOfVertices < 3 )
      CIE_THROW( std::runtime_error, "Region has less than 3 vertices" )

  for( size_t i = 0; i < numberOfVertices; ++i )
  {
    size_t id2 = (i + 1) % numberOfVertices;
    size_t id3 = (i + 2) % numberOfVertices;

    if( chopQuality( vertices, region, i, id2, id3 ) >= choppingCriteria )
    {
      connectivity.push_back( TriangleConnectivity{ region[i], region[id2], region[id3] } );

      region.erase( region.begin( ) + id2 );

      return true;
    }
  } // for i

  return false;
}

void prepareForTriangulating( const Vertex2DVector& vertices,
                              const std::vector<IndexVector>& polygonRegions,
                              TriangulationParameters& parameters )
{
  if( polygonRegions.empty( ) )
      CIE_THROW( std::invalid_argument, "No region was passed" )

  for( const auto& region : polygonRegions )
  {
    if( region.size( ) < 3 )
        CIE_THROW( std::invalid_argument, "Region has less than 3 points!" )
  }
  if( parameters.edgeLength == 0.0 )
  {
    parameters.edgeLength = meshgeneratorhelper::distance( vertices[polygonRegions[0][0]], vertices[polygonRegions[0][1]] );
  }
}

} // namespace meshgeneratorhelper
} // namespace cie::geo