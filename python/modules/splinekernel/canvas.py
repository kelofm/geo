import numpy as np
from vispy import scene
from cie.splinekernel.editmarkervisual import EditMarkerVisual
from cie.splinekernel.splinevisual import SplineVisual


class CanvasSpline2D(scene.SceneCanvas):
    def __init__(self):
        # Initialize canvas
        scene.SceneCanvas.__init__(self, keys='interactive',
                                   size=(800, 600))
        # Unfreeze
        self.unfreeze()
        # Create some initial points
        self.pos = np.empty((1,3),dtype=np.float32)
        # create new editable line
        self.objects = {
            'markers'   : EditMarkerVisual( pos=self.pos,
                                            face_color='w',
                                            scaling=True    ),
            'spline'    : SplineVisual( pos=self.pos,
                                        color='w',
                                        width=1,
                                        antialias=False,
                                        method='gl'         )
            }
        # Add view and camera
        self.view = self.central_widget.add_view()
        self.view.camera = scene.PanZoomCamera(rect=(-100, -100, 200, 200),
                                               aspect=1.0)
        # Disable mouse press events for the camera
        self.view.camera._viewbox.events.mouse_move.disconnect(
            self.view.camera.viewbox_mouse_event)
        # Visual objects
        self.view.add( self.objects['markers'] )
        self.view.add( self.objects['spline'] )
        scene.visuals.GridLines(parent=self.view.scene)
        # Add first point (cursor)
        self.activeCursor   = True
        self.lastPoint      = [1.0,1.0]
        self.objects['spline'].addPoint([0.0,0.0])
        # Settings
        self.settings={
            'mode'              : ['define',    self.setMode],
            'polynomialOrder'   : [3,           self.objects['spline'].setPolynomialOrder],
            'snapToGrid'        : [False,       lambda state: state]
        }
        # Modes
        self.modes = {
            'define'    : CanvasDefineMode(self),
            'edit'      : CanvasEditMode(self)
        }
        # Initialize event handlers
        self.eventHandlers = {
                'on_mouse_press'        : self.modes[self.settings['mode'][0]].on_mouse_press,
                'on_mouse_release'      : self.modes[self.settings['mode'][0]].on_mouse_release,
                'on_mouse_move'         : self.modes[self.settings['mode'][0]].on_mouse_move
            }
        # Freeze
        self.freeze()


    # SET FUNCTIONS ---------------------------------------------------------------------------------------
    def updateSettings(self, **kwargs):
        for key,value in kwargs.items():
            if self.settings[key][0] != value:
                self.settings[key][1](value)
                self.settings[key][0] = value

    def setMode(self,mode):
        # Check if mode is changed
        if self.settings['mode'] != mode:
            # Switch to define mode
            if self.settings['mode'][0] == 'define':
                if self.activeCursor:
                    self.objects['spline'].pop()
                    self.objects['markers'].updateMarkers()
            # Switch to edit mode
            elif mode == 'define':
                if self.activeCursor:
                    self.objects['spline'].addPoint([0.0,0.0])
            # Refresh event handlers
            self.eventHandlers['on_mouse_press']    = self.modes[self.settings['mode'][0]].on_mouse_press
            self.eventHandlers['on_mouse_release']  = self.modes[self.settings['mode'][0]].on_mouse_release
            self.eventHandlers['on_mouse_move']     = self.modes[self.settings['mode'][0]].on_mouse_move


    # COORDINATE TRANSFORM FUNCTIONS ----------------------------------------------------------------------
    def snapToGrid(self,pos):
        if self.settings['snapToGrid'][0]:
            pos[0] = round(pos[0] / self.objects['markers'].gridsize) * self.objects['markers'].gridsize
            pos[1] = round(pos[1] / self.objects['markers'].gridsize) * self.objects['markers'].gridsize
        # Return position
        return pos

    def getPos(self,event):
        # Get exact position
        transform   = self.scene.node_transform(self.objects['markers'])
        pos         = transform.map(event.pos)
        # Return position
        return pos


    # EVENT HANDLER ROUTERS ---------------------------------------------------------------------
    def on_mouse_press(self,event):
        self.modes[self.settings['mode'][0]].on_mouse_press(event)


    def on_mouse_release(self,event):
        self.modes[self.settings['mode'][0]].on_mouse_release(event)


    def on_mouse_move(self,event):
        self.modes[self.settings['mode'][0]].on_mouse_move(event)






# EVENT HANDLER CLASSES ---------------------------------------------------------------------
class CanvasDefineMode:
    def __init__(self,canvas):
        self.canvas     = canvas

    def on_mouse_press(self, event):
        # Get mouse position and snap to grid if specified
        pos = self.canvas.snapToGrid( self.canvas.getPos(event) )
        # Disable cursor and store clicked point
        if self.canvas.activeCursor:
            self.canvas.lastPoint      = pos
            self.canvas.objects['spline'].setPoint(self.canvas.lastPoint)
            self.canvas.objects['markers'].addPoint(self.canvas.lastPoint)
            self.canvas.activeCursor   = False


    def on_mouse_release(self,event):
        self.canvas.objects['markers'].updateMarkers()


    def on_mouse_move(self, event):
        # Get mouse position
        pos = self.canvas.snapToGrid( self.canvas.getPos(event) )
        # Check if cursor is active
        if not self.canvas.activeCursor:
            if self.canvas.objects['spline'].addPoint(pos):
                self.canvas.activeCursor = True
        else:
            # Set cursor
            self.canvas.objects['spline'].setPoint(pos)





class CanvasEditMode:
    def __init__(self,canvas):
        self.canvas     = canvas

    def on_mouse_press(self, event):
        # Get mouse position
        pos = self.canvas.getPos(event)
        # Update selection
        self.canvas.objects['markers'].selected_point, self.canvas.objects['markers'].selected_index = self.canvas.objects['markers'].selectPoint(pos)


    def on_mouse_drag(self,pos):
        if self.canvas.objects['markers'].selected_point is not None:
            # Snap to grid
            self.canvas.objects['markers'].selected_point = pos
            # Update polygon
            self.canvas.objects['markers'].setPoint(self.canvas.objects['markers'].selected_index,self.canvas.objects['markers'].selected_point)
            self.canvas.objects['markers'].updateMarkers(self.canvas.objects['markers'].selected_index)
            # Update spline
            self.canvas.objects['spline'].setPoint(self.canvas.objects['markers'].selected_point,index=self.canvas.objects['markers'].selected_index)


    def on_mouse_release(self,event):
        # Clear selection
        self.canvas.objects['markers'].selected_point = None
        self.canvas.objects['markers'].selected_index = -1
        self.canvas.objects['markers'].updateMarkers()


    def on_mouse_move(self, event):
        # Position
        pos = self.canvas.snapToGrid( self.canvas.getPos(event) )
        # Drag
        if event.button == 1:
            self.on_mouse_drag(pos)





'''
class CanvasSurface(scene.SceneCanvas):
    def __init__(self):
        # Initialize canvas
        scene.SceneCanvas.__init__(self, keys='interactive',
                                   size=(800, 600))
        # Unfreeze
        self.unfreeze()
        # Create some initial points
        self.pos = np.empty((1,3),dtype=np.float32)
        # create new editable line
        self.objects = {
            'markers'   : EditMarkerVisual(pos=self.pos, face_color='w', scaling=True),
            'spline'    : SplineVisual(pos=self.pos, color='w', width=1, antialias=False, method='gl')
            }
        # Add view and camera
        self.view = self.central_widget.add_view()
        self.view.camera = scene.PanZoomCamera(rect=(-100, -100, 200, 200),
                                               aspect=1.0)
        # Disable mouse press events for the camera
        self.view.camera._viewbox.events.mouse_move.disconnect(
            self.view.camera.viewbox_mouse_event)
        # Visual objects
        self.view.add( self.objects['markers'] )
        self.view.add( self.objects['spline'] )
        scene.visuals.GridLines(parent=self.view.scene)
        # Add first point (cursor)
        self.activeCursor   = True
        self.lastPoint      = [1.0,1.0]
        self.objects['spline'].addPoint([0.0,0.0])
        # Settings
        self.settings={
            'mode'              : ['define',    self.setMode],
            'polynomialOrder'   : [3,           self.objects['spline'].setPolynomialOrder],
            'snapToGrid'        : [False,       lambda state: state]
        }
        # modes
        self.modes = {
            'define' : {
                'on_mouse_press'    : self.on_mouse_press_define,
                'on_mouse_move'     : self.on_mouse_move_define,
                'on_mouse_release'  : self.on_mouse_release_define
                },
            'edit' : {
                'on_mouse_press'    : self.on_mouse_press_edit,
                'on_mouse_move'     : self.on_mouse_move_edit,
                'on_mouse_release'  : self.on_mouse_release_edit
            }
        }
        # Freeze
        self.freeze()


    # SET FUNCTIONS ---------------------------------------------------------------------------------------
    def updateSettings(self, **kwargs):
        for key,value in kwargs.items():
            if self.settings[key][0] is not value:
                self.settings[key][1](value)
                self.settings[key][0] = value

    def setMode(self,mode):
        if self.settings['mode'][0] is 'define' and self.activeCursor:
            self.objects['spline'].pop()
        elif mode is 'define' and self.activeCursor:
            self.objects['spline'].addPoint([0.0,0.0])

    # COORDINATE TRANSFORM FUNCTIONS ----------------------------------------------------------------------
    def snapToGrid(self,pos):
        if self.settings['snapToGrid'][0]:
            pos[0] = round(pos[0] / self.objects['markers'].gridsize) * self.objects['markers'].gridsize
            pos[1] = round(pos[1] / self.objects['markers'].gridsize) * self.objects['markers'].gridsize
        # Return position
        return pos

    def getPos(self,event):
        # Get exact position
        transform   = self.scene.node_transform(self.objects['markers'])
        pos         = transform.map(event.pos)
        # Return position
        return pos

    # EVENT HANDLERS - DEFINE -----------------------------------------------------------------------------
    def on_mouse_press_define(self, event):
        # Get mouse position and snap to grid if specified
        pos = self.snapToGrid( self.getPos(event) )
        # Disable cursor and store clicked point
        if self.activeCursor:
            self.lastPoint      = pos
            self.objects['spline'].setPoint(self.lastPoint)
            self.objects['markers'].addPoint(self.lastPoint)
            self.activeCursor   = False


    def on_mouse_release_define(self,event):
        pass


    def on_mouse_move_define(self, event):
        # Get mouse position
        pos = self.snapToGrid( self.getPos(event) )
        # Check if cursor is active
        if not self.activeCursor:
            if self.objects['spline'].addPoint(pos):
                self.activeCursor = True
        else:
            # Set cursor
            self.objects['spline'].setPoint(pos)


    # EVENT HANDLERS - EDIT -----------------------------------------------------------------------------
    def on_mouse_press_edit(self, event):
        # Get mouse position
        pos = self.getPos(event)
        # Update selection
        self.objects['markers'].selected_point, self.objects['markers'].selected_index = self.objects['markers'].selectPoint(pos)


    def on_mouse_drag_edit(self,pos):
        if self.objects['markers'].selected_point is not None:
            # Snap to grid
            self.objects['markers'].selected_point = pos
            # Update polygon
            self.objects['markers'].setPoint(self.objects['markers'].selected_index,self.objects['markers'].selected_point)
            self.objects['markers'].updateMarkers(self.objects['markers'].selected_index)
            # Update spline
            self.objects['spline'].setPoint(self.objects['markers'].selected_point,index=self.objects['markers'].selected_index)


    def on_mouse_release_edit(self,event):
        # Clear selection
        self.objects['markers'].selected_point = None
        self.objects['markers'].selected_index = -1
        self.objects['markers'].updateMarkers()


    def on_mouse_move_edit(self, event):
        # Position
        pos = self.snapToGrid( self.getPos(event) )
        # Drag
        if event.button == 1:
            self.on_mouse_drag_edit(pos)

    # EVENT HANDLER ROUTERS ---------------------------------------------------------------------
    def on_mouse_press(self,event):
        self.modes[self.settings['mode'][0]]['on_mouse_press'](event)


    def on_mouse_release(self,event):
        self.modes[self.settings['mode'][0]]['on_mouse_release'](event)


    def on_mouse_move(self,event):
        self.modes[self.settings['mode'][0]]['on_mouse_move'](event)

'''