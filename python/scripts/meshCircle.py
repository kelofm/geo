from cie.geo import *
import numpy
import math
import matplotlib.pyplot as plt
import matplotlib.tri as tri

n = 25
r = 1.0

parameters = pygeo.TriangulationParameters( )

polygonVertices = [ [ math.sin( ( 2.0 * math.pi * i ) / n ), math.cos( ( 2.0 * math.pi * i ) / n ) ] for i in range( n ) ]

[vertices, connectivity] = pygeo.triangulate( polygonVertices, parameters )

vertices = numpy.array( vertices )
connectivity = numpy.array( connectivity )

triangulation = tri.Triangulation( vertices[:, 0], vertices[:, 1], connectivity )

plt.triplot( triangulation )
plt.plot( numpy.array(polygonVertices)[range( -1, n ), 0], numpy.array(polygonVertices)[range( -1, n ), 1] )

plt.show( )
