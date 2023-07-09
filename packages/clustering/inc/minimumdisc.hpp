#ifndef CIE_GEO_MINIMUM_DISC_HPP
#define CIE_GEO_MINIMUM_DISC_HPP

// --- Linalg Includes ---
#include "packages/types/inc/vectortypes.hpp"
#include "packages/types/inc/arraytypes.hpp"
#include "packages/types/inc/typeoperations.hpp"

// --- STL Includes ---
#include <vector>
#include <array>
#include <memory>

namespace cie::geo {


// Type aliases -----------------------------------------------------------------
using PointSet2D            = std::vector<DoubleArray<2>>;
using PointSet2DPtr         = std::shared_ptr<PointSet2D>;
using ConstPointSet2DPtr    = std::shared_ptr<const PointSet2D>;


// Quick norm definition --------------------------------------------------------
double distance(const DoubleArray<2>& point1, const DoubleArray<2>& point2);


// Quick disc definition --------------------------------------------------------
struct Disc {
    Disc(const DoubleArray<2>& center, double radius);
    Disc(const DoubleArray<2>& point1, const DoubleArray<2>& point2);
    Disc(const DoubleArray<2>& point1, const DoubleArray<2>& point2, const DoubleArray<2>& point3);
    bool isInside(const DoubleArray<2>& point);

    double _radius2;
    DoubleArray<2> _center;
};


// Solver ----------------------------------------------------------------------
// Define class for encapsulating everything related to computing
// the minimum enclosing disc of a 2D point set
class MinimumEnclosingDisc {
public:
    MinimumEnclosingDisc(const PointSet2D& pointSet);
    int				build(double tolerance=1e-10);
    double          getRadius() const;
    DoubleArray<2>  getCenter() const;
	IntVector		getActiveIndices() const;

private:
    const DoubleArray<2>&   getPoint(size_t index) const;
	int						checkEnclosure(double tolerance) const;
	int						addActiveIndex(int index);
	void					removeActiveIndex(int index);

    Disc                _disc;
    ConstPointSet2DPtr  _pointSetPtr;
    IntArray<4>         _activeIndices;
    UIntVector          _map;
};


} // namespace cie::geo

#endif