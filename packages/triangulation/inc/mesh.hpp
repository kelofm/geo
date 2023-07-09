#ifndef CIE_MESH_KERNEL_MESH_HPP
#define CIE_MESH_KERNEL_MESH_HPP

// --- Linalg Includes ---
#include "packages/types/inc/vectortypes.hpp"

// --- Utility Includes ---
#include "packages/stl_extension/inc/StaticArray.hpp"

// --- STL Includes ---
#include <memory>

namespace cie::geo {


using VertexGrid = std::array<linalg::Matrix, 3>;
using TriangleVector = std::vector<UIntArray<3>>;

using VertexGridPtr = std::unique_ptr<VertexGrid>;
using TriangleVectorPtr = std::unique_ptr<TriangleVector>;


class TriangleMesh
{
public:
    TriangleMesh() = delete;
    explicit TriangleMesh(VertexGrid& vertices,
                          TriangleVector& faces);

private:
    VertexGridPtr _vertices;
    TriangleVectorPtr _faces;

};

} // namespace cie::geo

#endif