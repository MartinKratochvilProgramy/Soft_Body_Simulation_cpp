# How it works?
Program was written in c++ using Microsoft Visual Studio, you can load it the VS using the .sln file.

Default gravity points in the positive y direction (downwards). Line between the center of the soft body and mouse cursor is displayed, by pressing LMB user may change the gravity vector to point towards the cursor, where it's strenght will be proportional to the distance. There is some low amount of drag defined.

Soft body is constructed from connected springs and is filled with virtual pressure. Number of strings is defined by the variable number_of_elements. In the main loop (starting at line 253 in main.cpp) user input is checked, then the gravity is set and the two most important functions are run: accumulateForces() and integrateEuler(). Function accumulateForces loops through all the springs (each spring having 2 vertices) and calculates forces on each vertex from gravity, then accumulates forces from springs and pressure at each vertex, where pressure is calculated by assuming ideal gas inside the ball p = nRT/V. After finding all forces acting on vertices, their new position is calculated in the integrateEuler() function, which uses the Euler integration method for solving differential equations. Another integration method here is defined in integrateHeun(), however it is not being used.


https://user-images.githubusercontent.com/94861828/229893601-0b1a505e-3fe7-47a8-919e-87d81ef8e437.mp4




