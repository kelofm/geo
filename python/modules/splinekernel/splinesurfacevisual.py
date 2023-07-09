from vispy import scene
from cie.splinekernel.surfacekernel import SurfaceKernel


class SplineSurfaceVisual(scene.visuals.SurfaceVisual):
    def __init__(self, *args, **kwargs):
        scene.visuals.SurfaceVisual.__init__(*args,**kwargs)
        self.reset()

    def reset(self):
        self.unfreeze()
        self.surfaceKernel = SurfaceKernel()
        self.freeze()

    # SET/GET ---------------------------------------------------------
    def addPoint(self,point):
        return self.surfaceKernel.addPoint(point)

    def pop(self):
        pass

    def setPoint(self,point,index=-1):
        pass

    def setPolynomialOrders(self,polynomialOrders=[3,3]):
        pass

    def draw(self):
        pass