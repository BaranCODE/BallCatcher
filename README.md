# BallCatcher
Software that powers a ball-catching robot. 

The C++ code uses OpenCV to track a ball through multiple cameras, triangulates its position, and calculates its trajectory.
It depends on OpenCV for computer vision, Eigen for matrix calculations, and GSL for the data analysis and polynomial curve fitting.
Additionally, it depends on CL PS3 Eye Multicam SDK to be able to use Playstation 3 eye cameras.

The Arduino code then takes the ball's estimated falling position, and uses it to control a Cartesian coordinate robot.