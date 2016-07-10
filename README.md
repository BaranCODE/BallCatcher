# BallCatcher
Software that powers a ball-catching robot.

The C++ code uses OpenCV to track a ball through multiple cameras, triangulates its position, and calculates its trajectory.
It depends on OpenCV and Eigen.

The Arduino code then takes the ball's estimated falling position, and uses it to control a Cartesian coordinate robot.