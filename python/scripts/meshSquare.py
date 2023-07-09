from cie.geo import *
import numpy
import matplotlib.pyplot as plt
import matplotlib.tri as tri

n = 20
l = 1.0

parameters = TriangulationParameters( )

# bottom edge, right edge, top edge, left edge
polygonVertices = [ [ ( l * i ) / n, 0.0        ] for i in range(n) ] + \
                  [ [ l, ( l * i ) / float( n ) ] for i in range(n) ] + \
                  [ [ l * ( n - i ) / n, l      ] for i in range(n) ] + \
                  [ [ 0.0, l * ( n - i ) / n    ] for i in range(n) ]

[vertices, connectivity] = triangulate( polygonVertices, parameters )

vertices = numpy.array( vertices )
connectivity = numpy.array( connectivity )

triangulation = tri.Triangulation( vertices[:, 0], vertices[:, 1], connectivity )

plt.triplot( triangulation )
plt.plot( numpy.array(polygonVertices)[range( -1, 4 * n ), 0], numpy.array(polygonVertices)[range( -1, 4 * n ), 1] )

plt.show( )