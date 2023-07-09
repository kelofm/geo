// --- Internal Includes ---
#include "packages/triangulation/inc/meshgenerator.hpp"
#include "packages/triangulation/inc/meshgenerator_helper.hpp"

// --- STL Includes ---
#include <cmath>
#include <numeric>
#include <algorithm>

namespace cie::geo
{

Triangulation triangulate( const Vertex2DVector& polygon,
                           TriangulationParameters parameters )
{
  std::vector<IndexVector> regions{ IndexVector( polygon.size( ) ) };
  std::iota( regions[0].begin( ), regions[0].end( ), 0 );

  return triangulate( polygon, regions, parameters );
}

Triangulation triangulate( const Vertex2DVector& vertices,
                           const std::vector<IndexVector>& polygonRegions,
                           TriangulationParameters parameters )
{
  using namespace meshgeneratorhelper;

  meshgeneratorhelper::prepareForTriangulating( vertices, polygonRegions, parameters );

  Triangulation triangulation;
  triangulation.first = vertices;

  auto regions = polygonRegions; // create copy

  while( checkBreakingCriteria( regions ) )
  {
    for( size_t iRegion = 0; iRegion < regions.size( ); ++iRegion )
    {
      while( chopTriangle( triangulation, regions[iRegion], parameters.goodChopRatio ) );

      if( !attemptDivision( triangulation.first, regions, iRegion, parameters.divisionAngle, parameters.edgeLength ) )
      {
        chopTriangle( triangulation, regions[iRegion], 0.0 );
      }
    }
  }

  return triangulation;
}


}// namespace cie::geo