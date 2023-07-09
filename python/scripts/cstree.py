# --- External Imports ---
from matplotlib import pyplot
import numpy

# --- CiE Imports ---
import cie.geo


def predicate(node: cie.geo.ContiguousCubeTree2DNode):
    vertices = node.getVertices()
    s = sum(n < 0.5 for n in [numpy.linalg.norm(vertex - numpy.array([0.5, 0.5])) for vertex in vertices])
    return 0 < s and s < len(vertices)


maxLevel = 8


trees = [cie.geo.ContiguousCubeTree2D(base, 0.5) for base in ([0.0, 0.0], [0.5, 0.0], [0.0, 0.5], [0.5, 0.5])]
for tree in trees:
    tree.scan(predicate, maxLevel)


figure, axes = pyplot.subplots()
def plotNode(node, level) -> bool:
    #if node.isLeaf() and level == maxLevel:
        vertices = numpy.array(node.getVertices()).transpose()
        indexMap = [0, 1, 3, 2, 0]
        axes.plot(vertices[0,indexMap],
                  vertices[1,indexMap])
        return True
    #return True
for tree in trees:
    tree.visit(plotNode)

axes.set_aspect("equal", "box")
pyplot.show()
