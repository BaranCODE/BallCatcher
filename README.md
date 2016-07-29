# Catch-Bot
Software that powers a ball-catching robot. 

The C++ code uses OpenCV to track a ball through multiple cameras, triangulates its position, and calculates its trajectory.
It depends on OpenCV for computer vision, Eigen for matrix calculations, and GSL for the data analysis and polynomial curve fitting.
Additionally, it depends on CL PS3 Eye Multicam SDK to be able to use Playstation 3 eye cameras.

The Arduino code then takes the ball's estimated falling position from the C++ program over the serial port, and uses it to control a Cartesian coordinate robot.

Website: https://barancode.com/projects/catchbot/

####Note:
The trajectory calculations (consisting of calculating the best fit curve through the triangulated coordinate data points, and extrapolating the fall position) don't work reliably. However, the triangulation code itself works reasonably well. Therefore a demo mode has been implemented (can be toggled from Settings window), in which the trajectory calculation code is disabled, and the robot is told to shadow the ball's movement. When the ball is thrown vertically (upwards) from next to the robot, with only small horizontal motions, the robot is able to catch the ball. For this configuration, the cameras must be facing upwards (instead of forward, as seen in the diagram).

##Credits:
- Created by BaranCODE
- OpenCV ball detection code taken from Shermal Fernando: http://opencv-srf.blogspot.com/2010/09/object-detection-using-color-seperation.html
- Triangulation method taken from ja72: http://stackoverflow.com/questions/10551555/need-an-algorithm-for-3d-vectors-intersection
- Polynomial curve fitting code taken from Rosetta-Code: https://rosettacode.org/wiki/Polynomial_regression#C
- PS3 Eye camera code taken from CL sample: https://codelaboratories.com/research/view/cl-eye-platform-cpp-sample
- Arduino Serial communication library taken from: http://playground.arduino.cc/Interfacing/CPPWindows
