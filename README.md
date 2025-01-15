# Shallow-water-equation-2D
This code solves shallow water equations (SWE) in a rectangular domain. Below is a more thorough description of the math behind the problem and how to run the code.

## Shallow water equations
The shallow water equation is a system of partial diﬀerential equations that approximates the propagation of ocean waves, river and estuary ﬂows, etc., where the assumption of a relative “shallowness” of water, i.e., the ratio of the depth to horizontal directions being small, holds. The system of equations takes the following form:

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
In each direction, we have a total of $N_x$ elements plus two ghost elements to hold boundary condition values. On the horizontal boundaries (left and right), we prescribe the reﬂecting boundary condition as follows:

$$h_{i,0} = h_{i,1},\qquad uh_{i,0} = -uh_{i,1},\qquad vh_{i,0} = vh_{i,1}$$\
$$h_{i,N_x+1} = h_{i,N_x},\qquad uh_{i,N_x+1} = -uh_{i,N_x},\qquad vh_{i,N_x+1} = vh_{i,N_x}$$

The vertical boundary conditions are:
$$h_{0,j} = h_{1,j},\qquad uh_{0,j} = uh_{1,j},\qquad vh_{0,j} = -vh_{1,j},$$\
$$h_{N_x+1,j} = h_{N_x,j},\qquad uh_{N_x+1,j} = uh_{N_x,j},\qquad vh_{N_x+1,j} = -vh_{N_x,j}.$$

This way, the momentum is always reﬂected back into the domain, and the wave “bounces oﬀ” the walls of our “square bathtub” domain.

## Lax-Friedrichs time-stepping method
To discretize our equations, we use the Lax-Friedrichs scheme, which can be expressed as follows:

$$h_{i,j}^{n+1} = \frac14 \left( h_{i+1,j}^{n} + h_{i-1,j}^{n} + h_{i,j+1}^{n} + h_{i,j-1}^{n}\right) + \frac{\Delta t}{2\Delta x} \left(F_{h,\ i,j+1}^{n} - F_{h,\ i,j-1}^{n} \right) + \frac{\Delta t}{2\Delta y} \left(G_{h,\ i+1,j}^{n} - G_{h,\ i-1,j}^{n} \right) $$\
$$uh_{i,j}^{n+1} = \frac14 \left( uh_{i+1,j}^{n} + uh_{i-1,j}^{n} + uh_{i,j+1}^{n} + uh_{i,j-1}^{n}\right) + \frac{\Delta t}{2\Delta x} \left(F_{uh,\ i,j+1}^{n} - F_{uh,\ i,j-1}^{n} \right) + \frac{\Delta t}{2\Delta y} \left(G_{uh,\ i+1,j}^{n} - G_{uh,\ i-1,j}^{n} \right) $$\
$$vh_{i,j}^{n+1} = \frac14 \left( vh_{i+1,j}^{n} + vh_{i-1,j}^{n} + vh_{i,j+1}^{n} + vh_{i,j-1}^{n}\right) + \frac{\Delta t}{2\Delta x} \left(F_{vh,\ i,j+1}^{n} - F_{vh,\ i,j-1}^{n} \right) + \frac{\Delta t}{2\Delta y} \left(G_{vh,\ i+1,j}^{n} - G_{vh,\ i-1,j}^{n} \right) $$

The $^n$ superscript indicates the current value of the variable (at time $t_n$), and $^{n+1}$ indicates the next time-level value we are trying to predict. $\Delta t$ is the length of the time-step (the difference between $t^{n+1}$ and $t^n$, and $\Delta x$ and $\Delta y$ are the x and y dimensions of each element. 

## Running the code
The code for the above problem is implemented in the swe_2d.c ﬁle, and works only for one processor and can be run (after it is compiled with the attached Makeﬁle) using the following command:

`swe_2d 1000 0.0008 10 1.5`

where the argument list is $N_x = 1000$ elements (we assume $N_x = N_y$, so we get a total of N_x^2 elements, in this case 1,000,000), $\Delta t = 0.0008$s time step size (which needs to correspond to element size, i.e. if we have 2x more elements, we need 2x smaller $\Delta t$), $L_x = 10$ m domain length (we assume $L_x = L_y$ for a square bathtub), and $t_{final} = 1.5$s which is the final simulation time. You can plot the results on your local machine using the attached Python script

`python plot_wave_2d.py <file_to_plot>`

or provided Matlab script. The data ﬁles will be quite large, so if you want to quickly see how the result looks, you can decrease the resolution to $N_x = 400$. 




