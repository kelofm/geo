#ifndef CIE_MESH_KERNEL_MESHGENERATOR_HPP
#define CIE_MESH_KERNEL_MESHGENERATOR_HPP

// --- Utility Includes ---
#include "packages/types/inc/types.hpp"
#include "packages/stl_extension/inc/StaticArray.hpp"

// --- STL Includes ---
#include <vector>
#include <array>
#include <tuple>

namespace cie::geo
{

// Define aliases (equivalent to typedefs)
using Vector2D = StaticArray<double, 2>;
using Vertex2D = Vector2D;
using Vertex2DVector = std::vector<Vertex2D>;

using IndexVector = std::vector<Size>;

using TriangleConnectivity = std::array<Size, 3>;
using Triangulation = std::pair<Vertex2DVector, std::vector<TriangleConnectivity>>;

// Aggregates triangulation quality parameters
struct TriangulationParameters
{
  double edgeLength    = 0.0;
  double goodChopRatio = 0.68;
  double divisionAngle = 3.14159265 / 6.0;
};

Triangulation triangulate( const Vertex2DVector& polygon,
                           TriangulationParameters parameters );

Triangulation triangulate( const Vertex2DVector& vertices,
                           const std::vector<IndexVector>& polygonRegions,
                           TriangulationParameters parameters );

} // namespace cie::geo

#endif // MESHGENERATOR_HPP
