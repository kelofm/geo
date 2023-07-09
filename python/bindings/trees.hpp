#ifndef CIE_GEO_TREES_PYTHON_BINDINGS_HPP
#define CIE_GEO_TREES_PYTHON_BINDINGS_HPP

// --- External Includes ---
#include "pybind11/pybind11.h"

// --- Utility Includes ---
#include "packages/types/inc/types.hpp"


namespace cie::geo {


void makeGEOTreeBindings(Ref<pybind11::module_> r_module);


} // namespace cie::geo


#endif
