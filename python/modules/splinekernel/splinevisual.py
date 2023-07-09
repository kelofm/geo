import numpy as np
from vispy import scene
from cie.splinekernel.splinekernel import SplineKernel

class SplineVisual(scene.visuals.Line):
    # CONSTRUCTOR AND INITIALZER ----------------------------------------------------------------------
    def __init__(self, *args, **kwargs):
        scene.visuals.Line.__init__(self, *args, **kwargs)
        self.reset()
        
        
    def reset(self):
        self.unfreeze()
        self.splineKernel = SplineKernel()
        self.freeze()


    # MISC FUNCTIONS ------------------------------------------------------------
    def enable(self):
        self._visible = True


    def disable(self):
        self._visible = False


    # SPLINE EDIT ---------------------------------------------------------------
    def addPoint(self,point):
        return self.splineKernel.push( list(point[0:2]) )


    def pop(self):
        state = self.splineKernel.pop()
        self.drawSpline()
        return state
        

    def setPoint(self,point,index=-1):
        if index < len(self.splineKernel.interpolationPoints[0]) and len(self.splineKernel.interpolationPoints[0]) is not 0:
            self.splineKernel.interpolationPoints[0][index] = point[0]
            self.splineKernel.interpolationPoints[1][index] = point[1]
            self.drawSpline()
        else:
            return False
        return True


    def setPolynomialOrder(self,polynomialOrder):
        self.splineKernel.polynomialOrder = polynomialOrder
        self.drawSpline()


    def drawSpline(self):
        if len(self.splineKernel.interpolationPoints[0]) > self.splineKernel.polynomialOrder:
            self.splineKernel.getPoints()
            self.set_data( pos=np.transpose(
                np.append( self.splineKernel.curvePoints, np.zeros( [1,len(self.splineKernel.curvePoints[0])] ), axis=0 ),
                [1,0]
                ))
            self.update()
        else:
            return False
        return True

    # EVENT HANDLERS ------------------------------------------------------------