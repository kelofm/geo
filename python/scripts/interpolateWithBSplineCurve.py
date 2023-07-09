# --- Python imports ---
import numpy as np
import matplotlib.pyplot as plt

# --- Splinekernel imports ---
from cie.splinekernel import interpolateWithBSplineCurve, evaluate2DCurve

# -----------------------------------------------------
interpolationPoints = [ [ 0.0, 3.0, -1.0, -4.0, -4.0, -3.0 ],
                        [ 0.0, 4.0,  4.0,  0.0, -3.0, -3.0 ] ]

polynomialDegree = 3

controlPoints, knotVector   = interpolateWithBSplineCurve( interpolationPoints, polynomialDegree )
t                           = np.linspace( 0.0, 1.0, 100 )
xc, yc                      = evaluate2DCurve( t, controlPoints[0], controlPoints[1], knotVector )

plt.plot( controlPoints[0], controlPoints[1], 'r-x')
plt.plot( xc, yc, 'b' )
plt.plot( interpolationPoints[0], interpolationPoints[1], 'o')

plt.show( )