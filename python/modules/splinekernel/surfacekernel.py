# --- CiE Imports ---
import cie.splinekernel as splinekernel

# --- External Imports ---
import numpy as np


# -----------------------------------------------------------------------------
# CLASS DEFINITIONS
# -----------------------------------------------------------------------------
class SurfaceKernel:
    # Constructor -------------------------------------------------------------
    def __init__(self,interpolationPoints=np.array( [ [[],[]], [[],[]], [[],[]] ] ), polynomialOrders=[3,3]):
        # Reset
        self.reset()

        # Set interpolation points
        self.setInterpolationPoints(interpolationPoints=interpolationPoints)

        # Set polynomial order
        self.setPolynomialOrder(polynomialOrders=polynomialOrders)


    # GARBAGE COLLECTION AND RESET --------------------------------------------
    def reset(self):
        self.meshSize               = [0,0]
        self.polynomialOrders       = [3,3]
        self.interpolationPoints    = np.array( [ [[],[]], [[],[]], [[],[]] ] )
        self.controlPoints          = [[],[],[]]
        self.knotVectors            = [[],[]]
        self.tolerance              = 1e-15


    # MISC ------------------------------------------------------------
    def checkAxis(self,value,axis):
        # Check if value is already in axis
        tolerance   = 1e-15
        if len(self.interpolationPoints[axis]) != 0:
            for component in self.interpolationPoints[axis][0]:
                if abs(value-component)<tolerance:
                    return True

            shape   = self.interpolationPoints[axis].shape
            if shape[1] != 0:
                for component in (self.interpolationPoints[axis].T)[0]:
                    if abs(value-component)<tolerance:
                        return True
        return False


    def findIndexInAxis(self,value,axis):
        # Return the index of the first component in axis greater than value
        components  = []
        tolerance   = 1e-15
        if len( self.interpolationPoints[axis][0] )>1:
            if abs(self.interpolationPoints[axis][0][0]-self.interpolationPoints[axis][0][1])<tolerance:
                components = ((self.interpolationPoints[axis]).T)[0]
            else:
                components = self.interpolationPoints[axis][0]
        else:
            return 0

        try:
            return next( (i for i,x in enumerate(components) if abs(x-value)<tolerance ) )
        except:
            return len(components)


    def addPoint(self,point):
        # Check for existing components
        xMatch = self.checkAxis(point[0],0)
        yMatch = self.checkAxis(point[1],1)
        # Push point if necessary
        if not (xMatch and yMatch):
            # Find indices to insert point
            pos2 = self.findIndexInAxis(point[0],0)
            pos1 = self.findIndexInAxis(point[1],1)
            # Insert components
            print(np.array(point).reshape((3,1)))
            self.interpolationPoints = np.insert(
                self.interpolationPoints,
                [pos2],
                np.array(point).reshape((3,1)),
                axis=2
            )
            print(self.interpolationPoints)
            # Insert new row
            if not xMatch:
                self.interpolationPoints[0] = np.insert(self.interpolationPoints[0],pos1,
                    self.interpolationPoints[0][0])
                self.interpolationPoints[1] = np.insert(self.interpolationPoints[1],pos1,
                    [ point[1] for whatever in range(len( self.interpolationPoints[1][0] )) ])
                self.interpolationPoints[2] = np.insert(self.interpolationPoints[2],pos1,
                    [ 0.0 for whatever in range(len( self.interpolationPoints[2][0] )) ])





    # CALCULATION -------------------------------------------------------------
    def interpolate(self):
        self.controlPoints, self.knotVectors = splinekernel.interpolateWithBSplineSurface(
                self.interpolationPoints,
                self.polynomialOrders[0],
                self.polynomialOrders[1])


    # SET/GET -----------------------------------------------------------------
    def setPolynomialOrder(self,polynomialOrders=(3,3)):
        if polynomialOrders[0]>0 and polynomialOrders[1]>0:
            if type(polynomialOrders[0]) == int and type(polynomialOrders[1]) == int:
                self.polynomialOrders = polynomialOrders
            else:
                raise(ValueError('Invalid polynomial order'))
        else:
            raise(ValueError('Invalid polynomial order'))


    def setInterpolationPoints(self,interpolationPoints=[[],[],[]]):
        if len(interpolationPoints) == 3 and len(interpolationPoints[0]) == len(interpolationPoints[1]) == len(interpolationPoints[2]):
            self.interpolationPoints    = np.array(interpolationPoints)
        else:
            raise(ValueError('Number of components in interpolationPoints must match!'))


    # SURFACE GENERATION ------------------------------------------------------
    def generatePoints(self,nSamples=(10,10)):
        self.interpolate()
        return splinekernel.evaluateSurface(
                self.knotVectors,
                self.controlPoints,
                nSamples)