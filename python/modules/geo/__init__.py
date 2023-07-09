
# --- STD Imports ---
import pathlib as __pathlib
from importlib import util as __importutil
import sys as __sys

# Find binary package
__scriptDir = __pathlib.Path(__file__).absolute().parent
__pythonBindingsName = "geo_python_bindings"
__libDir = __scriptDir.parent / "lib"
__pythonBindings = __libDir.glob(__pythonBindingsName + "*")

if not __pythonBindings:
    raise FileNotFoundError(f"Could not find bindings for geo in ${__libDir}")

for __bindings in __pythonBindings:
    # Import the binary module
    __spec = __importutil.spec_from_file_location(__pythonBindingsName, str(__bindings))
    __module = __importutil.module_from_spec(__spec)
    __sys.modules[__spec.name] = __module
    __spec.loader.exec_module(__module)

    # Import everything from the binary module
    # and fill the current namespace
    for __attribute in dir(__module):
        if not __attribute.startswith("_"):
            globals()[__attribute] = getattr(__module, __attribute)

from .spacetree import loadSpaceTreeCSV, SpaceTreeCanvas
