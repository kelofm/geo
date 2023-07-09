# --- Python Imports ---
import numpy as np
import csv
from timeit import default_timer

# --- Visualization Imports ---
from cie.glvisuals import ModularCanvas

# --- Vispy Imports ---
from vispy import scene, app

# -----------------------------------------------------------
def matchSigns(data):
    result = data[0] > 0.0
    for item in data:
        if (item>0.0) != result:
            return False
    return True


def loadSpaceTreeCSV(fileName):
    data    = dict()
    header  = []
    with open(fileName) as file:

        reader = csv.reader(file)

        for rowIndex, row in enumerate(reader):

            if rowIndex == 0:
                for item in row:
                    data[item] = []
                header = tuple(data.keys())

            else:
                for itemIndex, item in enumerate(row):
                    data[header[itemIndex]].append(float(item))

    # Collect centers and values
    numberOfCells   = len(data["center0"])
    minimumEdge     = np.min(data["length"])
    centers         = []
    values          = []
    for name in header:
        if "center" in name:
            centers.append(name)
        elif "value" in name:
            values.append(name)

    centers     = [ [ data[name][index] for name in centers ] for index in range(numberOfCells) ]
    values      = [ [ data[name][index] for name in values  ] for index in range(numberOfCells) ]

    data        = { "center"    : centers,
                    "value"     : values,
                    "length"    : data["length"],
                    "boundary"  : [ ((length<1.1*minimumEdge) and (not matchSigns(value)))
                                        for value, length in zip(values, data["length"]) ]  }

    return data


class SpaceTreeCanvas(ModularCanvas):
    def __init__(   self,
                    data,
                    boundary=False,
                    visual="cell",
                    animation=None):
        ModularCanvas.__init__(self)

        self.unfreeze()

        self.dimensions = len(data["center"][0])
        self.boundary   = boundary
        self.visual     = visual
        self.animation  = animation
        self._timer     = app.Timer('auto', connect=self.on_timer, start='true')
        self.t0         = default_timer()
        self.t          = 0

        pos             = np.empty( (1,3), dtype=np.float32 )
        if self.visual == "cell" and self.dimensions > 1:
            self.addVisualObject(0,scene.visuals.Line(  pos=np.asarray(pos,dtype=np.float32),
                                                        connect="segments"))
        elif self.visual == "point" or self.dimensions==1:
            self.addVisualObject(0,scene.visuals.Markers(   pos=np.asarray(pos,dtype=np.float32),
                                                            size=3))
        else:
            raise ValueError("Invalid visual type!")

        self.updateData(data)

        if self.dimensions > 2:
            self.view.camera = scene.ArcballCamera(fov=0)

        self.freeze()


    def updateData(self,data):
        pos = []
        for boundary, edgeLength, center in zip( data["boundary"], data["length"], data["center"] ):

            if self.boundary:
                if not boundary:
                    continue

            x = center[0]
            y = 0.0
            z = 0.0

            if self.dimensions > 1:
                y = center[1]
            if self.dimensions > 2:
                z = center[2]

            if self.visual == "cell":
                radius = edgeLength/2.0
                pos.append( [x-radius, y-radius, z-radius] )
                pos.append( [x+radius, y-radius, z-radius] )
                pos.append( [x+radius, y-radius, z-radius] )
                pos.append( [x+radius, y+radius, z-radius] )
                pos.append( [x+radius, y+radius, z-radius] )
                pos.append( [x-radius, y+radius, z-radius] )
                pos.append( [x-radius, y+radius, z-radius] )
                pos.append( [x-radius, y-radius, z-radius] )

                pos.append( [x-radius, y-radius, z+radius] )
                pos.append( [x+radius, y-radius, z+radius] )
                pos.append( [x+radius, y-radius, z+radius] )
                pos.append( [x+radius, y+radius, z+radius] )
                pos.append( [x+radius, y+radius, z+radius] )
                pos.append( [x-radius, y+radius, z+radius] )
                pos.append( [x-radius, y+radius, z+radius] )
                pos.append( [x-radius, y-radius, z+radius] )

                pos.append( [x-radius, y-radius, z-radius] )
                pos.append( [x-radius, y-radius, z+radius] )
                pos.append( [x-radius, y+radius, z-radius] )
                pos.append( [x-radius, y+radius, z+radius] )
                pos.append( [x+radius, y-radius, z-radius] )
                pos.append( [x+radius, y-radius, z+radius] )
                pos.append( [x+radius, y+radius, z-radius] )
                pos.append( [x+radius, y+radius, z+radius] )

            else:
                pos.append( [x,y,z] )


        if self.visual == "cell":
            self.objects[0].set_data( pos=np.asarray(pos,dtype=np.float32) )
        else:
            self.objects[0].set_data( pos=np.asarray(pos,dtype=np.float32), size=3 )


    def on_timer(self,event):
        if self.animation != None:
            self.t = 2.0 - (default_timer() - self.t0) / 10.0
            if self.t < 2.0 and self.t > 0.0:
                self.animation(self)
            else:
                app.quit()