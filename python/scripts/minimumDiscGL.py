# --- Python imports ---
import numpy as np

# --- Vispy imports ---
from vispy import app

# --- Visualization imports ---
from cie.glvisuals import LineMarkerCanvas, Observer
from cie.glvisuals import LineVisual

# --- GEO Imports ---
import cie.geo as geo


class MinimumDiscObserver(Observer):
    def __init__(self, subject):
        Observer.__init__(self,subject)
        self.discResolution = 50

    def attach(self,subject):
        subject.addVisualObject('minimum_disc',LineVisual(pos=np.zeros((self.discResolution,3),dtype=np.float32)))


    def detach(self):
        del self.subject.objects['minimum_disc']
        self.subject=None

    def onSubjectChange(self,event):
        pts     = np.transpose( self.subject.objects['markers'].pos, (1,0) )[:2,:]
        disc    = geo.MinimumEnclosingDisc(np.transpose(pts,(1,0)))
        disc.build(1e-10)
        center  = disc.getCenter()
        radius  = disc.getRadius()

        n = self.subject.objects['minimum_disc'].pos.size / 3
        for i in range(int(n)):
            phi = i*2*np.pi/(n-1)
            self.subject.objects['minimum_disc'].pos[i,:] = [
                center[0]+radius*np.cos(phi),
                center[1]+radius*np.sin(phi),
                0.0
            ]
        self.subject.objects["minimum_disc"].set_data(self.subject.objects["minimum_disc"].pos)


canvas = LineMarkerCanvas()
canvas.attachObserver(MinimumDiscObserver(canvas))
canvas.objects['lines'].visible = False

canvas.view.camera._viewbox.events.mouse_move.disconnect(
            canvas.view.camera.viewbox_mouse_event)


canvas.show()
app.run()
