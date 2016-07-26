# BallCatcher
Software that powers a ball-catching robot. 

The C++ code uses OpenCV to track a ball through multiple cameras, triangulates its position, and calculates its trajectory.
It depends on OpenCV for computer vision, Eigen for matrix calculations, and GSL for the data analysis and polynomial curve fitting.
Additionally, it depends on CL PS3 Eye Multicam SDK to be able to use Playstation 3 eye cameras.

The Arduino code then takes the ball's estimated falling position, and uses it to control a Cartesian coordinate robot.

####Note:
This was created as a project in a summer engineering program. It wasn't fully finished in time, meaning that the robot is currently not capable of catching the ball.
The trajectory calculation and extrapolation processes aren't working reliably; however, the triangulation itself is. Therefore a demo mode has been implemented, in which the robot copies the motion of the ball in one's hand. This can be toggled from the Settings window.

##Credits:
- Created by BaranCODE
- OpenCV ball detection code taken from Shermal Fernando: http://opencv-srf.blogspot.com/2010/09/object-detection-using-color-seperation.html
- Triangulation method taken from ja72: http://stackoverflow.com/questions/10551555/need-an-algorithm-for-3d-vectors-intersection
- Polynomial curve fitting code taken from Rosetta-Code: https://rosettacode.org/wiki/Polynomial_regression#C
- PS3 Eye camera code taken from CL sample: https://codelaboratories.com/research/view/cl-eye-platform-cpp-sample
- Arduino Serial communication library taken from: http://playground.arduino.cc/Interfacing/CPPWindows
