# Shallow-water-equation-2D
This code solves shallow water equations (SWE) in a rectangular domain. Below is a more thorough description of the math behind the problem, and how to run the code.

## Shallow water equations
Shallow water equation is a system of partial diﬀerential equations which approximates the propagation of ocean waves, river and estuary ﬂows, etc., where the assumption of a relative “shallowness” of water, i.e. ratio of the depth to horizontal directions being small, holds. The system of equations takes the following form:

$$\frac{\partial h}{\partial t} + \frac{\partial F_h}{\partial x} + \frac{\partial G_h}{\partial y} = 0$$
$$\frac{\partial (uh)}{\partial t} + \frac{\partial F_{uh}}{\partial x} + \frac{\partial G_{uh}}{\partial y} = 0$$
$$\frac{\partial (vh)}{\partial t} + \frac{\partial F_{vh}}{\partial x} + \frac{\partial G_{vh}}{\partial y} = 0$$

where *h* is the water height, *u* is the velocity in the x direction, *v* is the velocity in the y direction. Variables *uh* and *vh* correspond to momentum in the x and y directions and are used as prognostic variables instead of *u* and *v*, which are computed from *h*, *uh*, and *vh*. *F* and *G* are the ﬂuxes deﬁned as:
$F_{h} = uh$, $G_h = vh$ 
