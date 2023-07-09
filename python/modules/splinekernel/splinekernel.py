import numpy as np
import cie.splinekernel as splinekernel

class SplineKernel:
    # Constructor -------------------------------------------------------------
    def __init__(self):
        self.reset()

    # GARBAGE COLLECTION AND RESET --------------------------------------------
    def reset(self):
        self.polynomialOrder = 3
        self.interpolationPoints = [[],[]]
        self.controlPoints = [[],[]]
        self.knotVector = []
        self.samplePoints = []
        self.curvePoints = [[],[]]

    # CALCULATION -------------------------------------------------------------
    def getSamplePoints(self):
        # Create sample points (10 for every spline segment - not related to segment boundaries)
        return np.linspace(0.0, 1.0, (len(self.interpolationPoints[0])-1)*10 )

    def updateSpline(self, lastPoint=[]):
        # Interpolate the stored points - if the last point is not empty, interpolate that too
        if len(lastPoint) > 1:
            self.controlPoints, self.knotVector = splinekernel.interpolateWithBSplineCurve(
                    [self.interpolationPoints[0]+[lastPoint[0]], self.interpolationPoints[1]+[lastPoint[1]]],
                    self.polynomialOrder)
        else:
            self.controlPoints, self.knotVector = splinekernel.interpolateWithBSplineCurve(
                    self.interpolationPoints,
                    self.polynomialOrder)

    def updatePoints(self):
        # Compute points on the interpolating spline
        x, y = splinekernel.evaluate2DCurveDeBoor(
                self.samplePoints,
                self.controlPoints[0],
                self.controlPoints[1],
                self.knotVector)
        return (x,y)

    # SET/GET -----------------------------------------------------------------
    def push(self, point):
        # Add an interpolation point - does not add the point if is equal to the last one (return False)
        if len(self.interpolationPoints[0]) > 0:
            if point[0] != self.interpolationPoints[0][-1] and point[1] != self.interpolationPoints[1][-1]:
                self.interpolationPoints[0].append(point[0])
                self.interpolationPoints[1].append(point[1])
            else:
                return False
        else:
            self.interpolationPoints[0].append(point[0])
            self.interpolationPoints[1].append(point[1])
        return True

    def pop(self):
        # Remove the last interpolation point
        if len(self.interpolationPoints[0]) > 0:
            self.interpolationPoints[0].pop()
            self.interpolationPoints[1].pop()
        else:
            return False
        return True

    def getPoints(self,lastPoint=[]):
        # Generate points on the spline - checks whether the minimum number of points are stored for the
        # given polynomial order
        if len(self.interpolationPoints[0]) + len(lastPoint)/2 > self.polynomialOrder:
            self.updateSpline(lastPoint)
            self.samplePoints = self.getSamplePoints()
            self.curvePoints = self.updatePoints()
        else:
            return False
        return True
