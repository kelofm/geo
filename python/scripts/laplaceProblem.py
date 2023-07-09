# --- Splinekernel imports ---
from cie.splinekernel import BSplineFiniteElementMesh, evaluateSurface

# --- Python imports ---
import numpy as np

import sys

import scipy.sparse
import scipy.sparse.linalg

from mpl_toolkits.mplot3d import Axes3D
import matplotlib.cm as colormap
import matplotlib.pyplot as plt

# -----------------------------------------------------
polynomialDegrees = [2, 2]
numberOfKnotSpans = [10, 10]
lengths = [1.0, 0.4]
origin = [0.0, 0.0]
continuity = [p - 1 for p in polynomialDegrees]

source = lambda x, y : 5.0

topBoundaryFunction = lambda x : ( x < 0.3 or x > 0.7 ) / 2.0 + 0.5

integrationPointProvider = lambda order : np.polynomial.legendre.leggauss( order )

mesh = BSplineFiniteElementMesh( numberOfKnotSpans, polynomialDegrees, continuity,
                                                lengths, origin, integrationPointProvider )

print( "Assembling linear system ..." )

( ( indices, indptr, data ), F ) = mesh.assembleLinearSystem( source )

F = np.array( F )

numberOfDofs = len( indptr ) - 1;

K = scipy.sparse.csr_matrix( ( numberOfDofs, numberOfDofs ) )
K.indices, K.indptr, K.data = indices, indptr, data
K.has_sorted_indices = True

print( "    number of dofs: " + str( numberOfDofs ) )
print( "    number of non-zeros in sparse matrix: " + str( K.nnz ) )
print( "    matrix requires " + str( ( 2 * K.nnz + numberOfDofs + 1 ) * 8 / 1e6 ) + " MB memory." )

print( "Imposing boundary conditions ..." )

bottomBoundaryDofs = mesh.boundaryDofIds( 1, 0 )
topBoundaryDofs = mesh.boundaryDofIds( 1, 1 )

topBoundaryValues = [ topBoundaryFunction( x ) for x in np.linspace( 0.0, 1.0, len( topBoundaryDofs ) ) ]

boundaryIndices = np.concatenate( [ bottomBoundaryDofs, topBoundaryDofs ] )
boundaryValues = np.concatenate( [ [ 0.0 for _ in bottomBoundaryDofs ], topBoundaryValues ] )

K[boundaryIndices, boundaryIndices] = 1e8
F[boundaryIndices] = boundaryValues * 1e8

print( "Solving linear system ..." )

def countIterations( _ ):
    countIterations.count += 1

    sys.stdout.write( '\r    number of iterations: ' + str( countIterations.count ) )
    sys.stdout.flush( )

countIterations.count = 0

invDiag = 1.0 / K.diagonal( ) # diagonal / Jacobi preconditioning
preconditioner = scipy.sparse.linalg.LinearOperator( ( K.shape[0], K.shape[1] ), lambda x : x * invDiag )
solutionDofs, exitCode = scipy.sparse.linalg.bicgstab( K, F, M=preconditioner, tol=1e-10, callback=countIterations )

print( "\n    || K u - f || / || f || = " + str( np.linalg.norm( K * solutionDofs - F ) / np.linalg.norm( F ) ) )

print( "Postprocessing ... " )

samplesPerSpan = [ 2 * ( p - 1 ) + 2 for p in polynomialDegrees ]

numberOfSamples = [ numberOfKnotSpans[i] * samplesPerSpan[i] + 1 for i in ( 0, 1 ) ]
numberOfDofsPerDirection = [ len( knotVector ) - p - 1 for knotVector, p in zip( mesh.knotVectors( ), polynomialDegrees ) ]

functionValues, = evaluateSurface( mesh.knotVectors( ), [ solutionDofs.reshape( numberOfDofsPerDirection ) ], numberOfSamples )

X, Y = np.meshgrid( np.linspace( origin[0], origin[0] + lengths[0], numberOfSamples[0] ),
                       np.linspace( origin[1], origin[1] + lengths[1], numberOfSamples[1] ), indexing='ij' )

colorMap = colormap.jet( functionValues / np.max( functionValues ) )

figure = plt.figure()
ax = figure.add_subplot( projection='3d' )
ax.plot_surface( X, Y, functionValues, facecolors=colorMap, rstride=1, cstride=1 )
ax.plot_wireframe( X, Y, functionValues, color="black", rstride=samplesPerSpan[0], cstride=samplesPerSpan[1] )

plt.show( )
