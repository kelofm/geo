import numpy as np
from vispy import app, scene

class EditMarkerVisual(scene.visuals.Markers):
    # CONSTRUCTOR AND INITIALZER ----------------------------------------------------------------------
    def __init__(self, *args, **kwargs):
        scene.visuals.Markers.__init__(self, *args, **kwargs)
        self.unfreeze()
        self.pos    = kwargs['pos']
        self.reset()
        
    def reset(self):
        self.unfreeze()
        # initialize point markers
        self.marker_colors = np.ones((len(self.pos), 4), dtype=np.float32)
        self.set_data(pos=self.pos, symbol="s", edge_color="red",
                              size=6)
        self.selected_point = None
        self.selected_index = -1
        # Grid size
        self.gridsize = 5
        self.freeze()

    # MARKER CONTROLS ------------------------------------------------------------
    def updateMarkers(self, selected_index=-1, highlight_color=(1, 0, 0, 1)):
        # Update marker colors, and highlight a marker with a given color
        self.marker_colors.fill(1)
        # default shape (non-highlighted)
        shape = "o"
        size = 6
        if 0 <= selected_index < len(self.marker_colors):
            self.marker_colors[selected_index] = highlight_color
            shape = "s"
            size = 8
        self.set_data(  pos=self.pos, 
                        symbol=shape, 
                        edge_color='red',
                        size=size, 
                        face_color=self.marker_colors   )
    
    def highlightMarkers(self, pos):
        #  if no button is pressed, just highlight the marker that would be
        # selected on click
        hl_point, hl_index = self.selectPoint(pos)
        self.updateMarkers(hl_index, highlight_color=(0.5, 0.5, 1.0, 1.0))
        self.update()

    # SET/GET FUNCTIONS ---------------------------------------------------------
    def addPoint(self, pos):
        if len(self.pos)==1 and self.pos[0][0]==0.0 and self.pos[0][1]==0.0:
            # Set first point
            self.setPoint(0,pos)
        else:
            self.pos = np.append(self.pos, [pos[:3]], axis=0)
            self.set_data(pos=self.pos)
            self.marker_colors = np.ones((len(self.pos), 4), dtype=np.float32)
            
        
    def setPoint(self,index,pos):
        for k in range(min( (len(pos)),3 )):
            self.pos[index][k] = pos[k]
        self.set_data(pos=self.pos)

    # POINT SELECTION MANAGERS --------------------------------------------------
    def selectPoint(self, pos, radius=3):
        # Check if input point is within a specified radius of an already stored point
        # (temporary implementation, horribly slow)
        for index,p in enumerate(self.pos):
            if np.linalg.norm(pos[:3] - p) < radius:
                return p, index
        # no point found, return None
        return None, -1

    # MISC FUNCTIONS ------------------------------------------------------------
    def enable(self):
        self._visible = True

    def disable(self):
        self._visible = False

    # EVENT HANDLERS ------------------------------------------------------------
    def on_draw(self, event):
        self.updateMarkers(self.selected_index)