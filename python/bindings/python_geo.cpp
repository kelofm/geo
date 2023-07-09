// --- External Includes ---
#include "pybind11/pybind11.h"
#include "pybind11/stl.h"
#include "pybind11/numpy.h"
#include "pybind11/functional.h"

// --- Utility Includes ---
#include "python/bindings/inc/stl_extension.hpp"

// --- GEO Includes ---
#include "packages/clustering/inc/minimumdisc.hpp"
#include "packages/triangulation/inc/meshgenerator.hpp"
#include "packages/spline/inc/basisfunctions.hpp"
#include "packages/spline/inc/curve.hpp"
#include "packages/spline/inc/surface.hpp"
#include "packages/spline/inc/interpolation.hpp"
#include "packages/spline/inc/finiteelements.hpp"
#include "python/bindings/denseMatrixConversion.hpp"
#include "python/bindings/sparseMatrixConversion.hpp"
#include "python/bindings/trees.hpp"


PYBIND11_MODULE(geo_python_bindings, m){
    m.doc() = "Geometry module";

    cie::geo::makeGEOTreeBindings(m);

    pybind11::class_<cie::geo::MinimumEnclosingDisc>(m, "MinimumEnclosingDisc")
        .def(pybind11::init<cie::geo::PointSet2D>())
        .def("build", &cie::geo::MinimumEnclosingDisc::build)
        .def("getCenter", &cie::geo::MinimumEnclosingDisc::getCenter)
        .def("getRadius", &cie::geo::MinimumEnclosingDisc::getRadius)
        .def("getActiveIndices",&cie::geo::MinimumEnclosingDisc::getActiveIndices)
        ;

    m.def("evaluateBSplineBasis", &cie::geo::evaluateBSplineBasis, "Evaluates a single b-spline basis function.");
    m.def("evaluate2DCurve", &cie::geo::evaluate2DCurve, "Evaluates B-Spline curve by multiplying control points and basis functions.");
    m.def("evaluate2DCurveDeBoor", &cie::geo::evaluate2DCurveDeBoor, "Evaluates B-Spline using DeBoor");
    m.def("evaluateSurface", &cie::geo::evaluateSurface, "Evaluates B-Spline surface");
    m.def("interpolateWithBSplineCurve", &cie::geo::interpolateWithBSplineCurve,
          "Finds control points for b-spline curve that interpolates given points.");
    m.def("interpolateWithBSplineSurface", &cie::geo::interpolateWithBSplineSurface,
          "Finds control points for b-spline surface that interpolates given points.");

    pybind11::class_<cie::geo::BSplineFiniteElementMesh> mesh(m, "BSplineFiniteElementMesh");

    mesh.def(pybind11::init<cie::StaticArray<size_t, 2>,
                            cie::StaticArray<size_t, 2>,
                            cie::StaticArray<size_t, 2>,
                            cie::StaticArray<double, 2>,
                            cie::StaticArray<double, 2>,
                            cie::geo::IntegrationPointProvider>()); // constructor

    mesh.def("boundaryDofIds", &cie::geo::BSplineFiniteElementMesh::boundaryDofIds);
    mesh.def("assembleLinearSystem", &cie::geo::BSplineFiniteElementMesh::assembleLinearSystem);
    mesh.def("knotVectors", &cie::geo::BSplineFiniteElementMesh::knotVectors);

    pybind11::class_<cie::geo::TriangulationParameters>(m, "TriangulationParameters")
        .def(pybind11::init<>())
        .def(pybind11::init<double, double, double>())
        .def_readwrite("edgeLength",&cie::geo::TriangulationParameters::edgeLength)
        .def_readwrite("goodChopRatio",&cie::geo::TriangulationParameters::goodChopRatio)
        .def_readwrite("divisionAngle",&cie::geo::TriangulationParameters::divisionAngle)
        ;

    // Function cast is required as we have multiple overloads of triangulation
    m.def("triangulate", (cie::geo::Triangulation (*)(const cie::geo::Vertex2DVector&,
                                                      const std::vector<cie::geo::IndexVector>&,
                                                      cie::geo::TriangulationParameters))
            &cie::geo::triangulate, "Triangulates a given polygon");

    m.def("triangulate", (cie::geo::Triangulation (*)(const cie::geo::Vertex2DVector&,
                                                      cie::geo::TriangulationParameters))
            &cie::geo::triangulate, "Triangulates a given polygon");
}
