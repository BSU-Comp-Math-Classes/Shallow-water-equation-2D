# Shallow-water-equation-2D
This code solves shallow water equations (SWE) in a rectangular domain. Below is a more thorough description of the math behind the problem, and how to run the code.

## Shallow water equations
Shallow water equation is a system of partial diﬀerential equations which approximates the propagation of ocean waves, river and estuary ﬂows, etc., where the assumption of a relative “shallowness” of water, i.e. ratio of the depth to horizontal directions being small, holds. The system of equations takes the following form:
$$ \dfrac{\partial h}{\partial t} + \dfrac{\partial F_h}{\partial x} + \dfrac{\partial G_h}{\partial y}= 0$$
