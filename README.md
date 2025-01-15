# Shallow-water-equation-2D
This code solves shallow water equations (SWE) in a rectangular domain. Below is a more thorough description of the math behind the problem, and how to run the code.

## Shallow water equations
Shallow water equation is a system of partial diﬀerential equations which approximates the propagation of ocean waves, river and estuary ﬂows, etc., where the assumption of a relative “shallowness” of water, i.e. ratio of the depth to horizontal directions being small, holds. The system of equations takes the following form:

$$\frac{\partial h}{\partial t} + \frac{\partial F_h}{\partial x} + \frac{\partial G_h}{\partial y} = 0$$
$$\frac{\partial (uh)}{\partial t} + \frac{\partial F_{uh}}{\partial x} + \frac{\partial G_{uh}}{\partial y} = 0$$
$$\frac{\partial (vh)}{\partial t} + \frac{\partial F_{vh}}{\partial x} + \frac{\partial G_{vh}}{\partial y} = 0$$

where $h$ is the water height, $u$ is the velocity in the x direction, $v$ is the velocity in the y direction. Variables $uh$ and $vh$ correspond to momentum in the x and y directions and are used as prognostic variables instead of $u$ and $v$. We can calculate velocities from  $h$, $uh$, and $vh$ by dividing momentum variables by height $h$. $F$ and $G$ are the ﬂuxes deﬁned as:

$$F_{h} = uh$$
$$G_h = vh$$
$$F_{uh} = \frac{(uh)^2}{h} + \frac12 gh^2$$
$$G_{uh} = \frac{(uh)\cdot(vh)}{h}$$
$$F_{vh} = \frac{(uh)\cdot(vh)}{h}$$
$$G_{vh} = \frac{(vh)^2}{h} + \frac12 gh^2$$

The gravitational acceleration is $g = 9.81\ m^2/s$.
We want to solve 2D shallow water equations in a rectangular domain of size $L_x \times L_y$, and for simplicity, we assume the domain is square. At the edges of the domain, we prescribe reﬂecting boundary conditions, described further in the numerics part.

## Numerical discretization with finite volume scheme
To solve the system numerically, we employ a ﬁnite volume scheme, where we divide the square domain into a grid of $N_x \times N_y$ elements (again, for simplicity, we assume $N_x = N_y$) and locate variables $uh$, $vh$ at the center of each element. The diagram below explains the indexing of elements and the creation of ghost elements to prescribe boundary conditions.
<img width="832" alt="image" src="https://github.com/user-attachments/assets/b330a24a-294b-45f7-a33f-6380e2429045" />


