/////////////////////////////////////////////////////////////////////////////
// BALL CATCHER
// By BaranCODE
// This program detects an object with OpenCV, triangulates its location,
// and calculates its trajectory. It is used to power a robot that catches balls.
//
// NOTES:
// - A right-handed coordinate system is used.
// - The ball moves along the X-axis
//
// CREDITS:
// - Created by BaranCODE
// - OpenCV ball detection code taken from Shermal Fernando: http://opencv-srf.blogspot.com/2010/09/object-detection-using-color-seperation.html
// - Triangulation method taken from ja72: http://stackoverflow.com/questions/10551555/need-an-algorithm-for-3d-vectors-intersection
/////////////////////////////////////////////////////////////////////////////


//////////////////////// HEADER //////////////////////// 
#include <iostream>
#include <math.h>
#include <Windows.h>
#include <string>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <Eigen/Dense>

using namespace cv;
using namespace std;
// TODO: Use OpenCV matrices instead of Eigen? <-------
using namespace Eigen;

class Camera {
public:
	VideoCapture cam;
	int lastX = -1;
	int lastY = -1;
	Mat imgLines;
	double rot[3];
	Vector3d pos;
	double planeDist;
	double planeX;
	double planeY;
	int screenX;
	int screenY;
};

Camera cameras[2];

Vector3d catcherPos(0, 0, 0); // TEMPORARY <----- TODO: REMOVE

int minHue, maxHue, minSat, maxSat, minValue, maxValue;

int dataCollectEnabled = 0;
Vector2d firstXY(0, 0);
Vector2d averageXY(0, 0);

void setupScreen();
void setupCamera(int i);
Vector3d runCamera(int i);
Vector3d rotateX(Vector3d v, double angle);
Vector3d rotateY(Vector3d v, double angle);
Vector3d rotateZ(Vector3d v, double angle);


/////////////// CALIBRATION INSTRUCTIONS ///////////////
// The pixels on the cameras' screens need to be mapped out to direction vectors
// relative to the cameras'. The following procedure is used:
// Place a ruler or plane of known dimensions at a certain (measured) distance
// from the camera. Record the vertical and horizontal lengths that are in the
// camera's view, at that distance. Half of these lengths, divided by distance,
// gives us the tangent value of the camera's maximum angular distance (half of
// the field of view). A ratio can then be used to map pixels to hypothetical
// dots on the plane, and hence direction vectors.
// Values are configured below:

void loadSettings() {
	cameras[0].planeDist = 10;		// distance of camera from plane (in cm)
	cameras[0].planeX = 12 / 2;		// observed width of plane / 2 (in cm)
	cameras[0].planeY = 8 / 2;		// observed height of plane / 2 (in cm)
	cameras[0].screenX = 640 / 2;	// horizontal resolution of camera / 2 (in pixels)
	cameras[0].screenY = 480 / 2;	// vetical resolution of camera / 2 (in pixels)

	cameras[1].planeDist = 10;
	cameras[1].planeX = 12 / 2;
	cameras[1].planeY = 8 / 2;
	cameras[1].screenX = 640 / 2;
	cameras[1].screenY = 360 / 2;
	//////// END OF CAMERA CALIB ////////

	// Rotation of camera around axes X, Y, and Z, in that order.
	// Default orientation is towards -Z, so that screen coordinates (X, Y) are consistent with world coordinates (+X -> right, +Y -> up)
	cameras[0].rot[0] = 90;
	cameras[0].rot[1] = 0;
	cameras[0].rot[2] = 78.69006753;

	cameras[1].rot[0] = 90;
	cameras[1].rot[1] = 0;
	cameras[1].rot[2] = 101.3099325;

	// Position of camera in terms of X, Y, and Z (default position is origin) (in cm for consistency)
	cameras[0].pos[0] = 500;
	cameras[0].pos[1] = -100;
	cameras[0].pos[2] = 0;

	cameras[1].pos[0] = 500;
	cameras[1].pos[1] = 100;
	cameras[1].pos[2] = 0;

	// Catcher position
	catcherPos[0] = 500;
	catcherPos[1] = 0;
	catcherPos[2] = 0;

	// HSV threshold values (configurable via UI)
	// TODO: Add config file for these <-------
	minHue = 21;
	maxHue = 61;
	minSat = 173;
	maxSat = 255;
	minValue = 24;
	maxValue = 140;
}


//////////////////////// FUNCTIONS ////////////////////////

int main(int argc, char** argv)
{
	loadSettings();

	setupScreen();
	setupCamera(0);
	setupCamera(1);

	int lastTick = 0;

	while (true){
		Vector3d v1 = runCamera(0);
		Vector3d v2 = runCamera(1);

		if (dataCollectEnabled == 1) {
			//////// TRIANGULATION ////////
			// This method, by ja72, gives us the intersection point of two
			// 3d lines (in the form of a point and direction vector).
			// If there is no intersection, it finds the point where the
			// lines are nearest.

			v1 = v1 / v1.norm();
			v2 = v2 / v2.norm();

			double u = v1.dot(v2);
			double t1 = (cameras[1].pos - cameras[0].pos).dot(v1);
			double t2 = (cameras[1].pos - cameras[0].pos).dot(v2);

			double d1 = (t1 - u*t2) / (1 - u*u);
			double d2 = (t2 - u*t1) / (u*u - 1);

			Vector3d p = ((cameras[0].pos + d1 * v1) + (cameras[1].pos + d2 * v2)) / 2;

			cout << "Point:\n" << p << endl;

			//////// END OF TRIANG ////////

			//////// TRAJECTORY ////////
			// Use the ball positions to calculate its trajectory, and
			// find where it will fall.

			if (firstXY == Vector2d::Zero()) {
				firstXY[0] = p[0];
				firstXY[1] = p[1];
				cout << "=====================\nSET THE FIRSTXY\n=====================" << endl;
			} else {
				Vector2d tmp(0, 0);
				tmp[0] = p[0];
				tmp[1] = p[1];
				// Get vector from first detected point to last
				averageXY = tmp - firstXY;
				// Scale it so that X=1
				averageXY = averageXY / averageXY[0];
				float estY = (averageXY * catcherPos[0])[1];
				cout << "ESTIMATED Y COORDINATE: " << estY << endl;
			}
			//////// END OF TRAJ ////////
		}

		//if (lastTick != 0) cout << "FPS: " << 1000 / (float)(GetTickCount() - lastTick) << endl;
		lastTick = GetTickCount();

		if (waitKey(1) == 27){ // Wait for ESC key
			cout << "Exiting" << endl;
			break;
		}
	}

	return 0;
}

void setupScreen() {
	cout << "PRESS ESC TO QUIT" << endl;
	namedWindow("Settings", CV_WINDOW_AUTOSIZE);

	createTrackbar("Enabled", "Settings", &dataCollectEnabled, 1);

	createTrackbar("Min Hue", "Settings", &minHue, 255); // Hue (0 - 255)
	createTrackbar("Max Hue", "Settings", &maxHue, 255);
	createTrackbar("Min Sat.", "Settings", &minSat, 255); // Saturation (0 - 255)
	createTrackbar("Max Sat.", "Settings", &maxSat, 255);
	createTrackbar("Min Value", "Settings", &minValue, 255); // Value (0 - 255)
	createTrackbar("Max Value", "Settings", &maxValue, 255);
}

void setupCamera(int i) {
	cameras[i].cam = VideoCapture(i);
	if (!cameras[i].cam.isOpened()) {
		cout << "Error opening webcam #" << i+1 << endl;
	}
	
	Mat imgTmp;// Capture a temporary image from the camera
	cameras[i].cam.read(imgTmp);
	cameras[i].imgLines = Mat::zeros(imgTmp.size(), CV_8UC3); // Create a black image with size matching camera's
	cout << "Camera #" << i+1 << ", Height: " << imgTmp.size().height << ", Width: " << imgTmp.size().width << endl;
}

Vector3d runCamera(int i) {
	Mat imgOriginal;
	Mat imgThresholded;

	if (!cameras[i].cam.read(imgOriginal)) {
		cout << "Error reading frame on webcam #" << i+1 << endl;
		return Vector3d::Zero();
	}

	cvtColor(imgOriginal, imgThresholded, COLOR_BGR2HSV); // Convert captured frame from BGR to HSV
	inRange(imgThresholded, Scalar(minHue, minSat, minValue), Scalar(maxHue, maxSat, maxValue), imgThresholded); // Threshold the image

	// Morphological opening (removes small objects from the foreground)
	erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
	dilate(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));

	// Morphological closing (removes small holes from the foreground)
	dilate(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
	erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));

	// Calculate the moments of the thresholded image
	Moments oMoments = moments(imgThresholded);
	double dM01 = oMoments.m01;
	double dM10 = oMoments.m10;
	double dArea = oMoments.m00;

	Vector3d tmpV(0, 0, 0);
	tmpV[2] = -cameras[i].planeDist;

	// If Area <= 10000, assume there are no objects in image and it's due to noise
	if (dArea > 10000) {
		// Calculate position of ball
		int posX = dM10 / dArea;
		int posY = dM01 / dArea;

		if (cameras[i].lastX >= 0 && cameras[i].lastY >= 0 && posX >= 0 && posY >= 0) {
			//Draw line from previous point to current point
			line(cameras[i].imgLines, Point(posX, posY), Point(cameras[i].lastX, cameras[i].lastY), Scalar(0, 0, 255), 2);
		}

		cameras[i].lastX = posX;
		cameras[i].lastY = posY;


		// Move origin to the middle of the screen
		posX = posX - cameras[i].screenX;
		posY = posY - cameras[i].screenY;

		// Find X and Y components of direction vector
		tmpV[0] = posX / cameras[i].screenX * cameras[i].planeX;
		tmpV[1] = posY / cameras[i].screenY * cameras[i].planeY;
	}

	// Rotate direction vector according to the camera
	tmpV = rotateX(tmpV, cameras[i].rot[0]);
	tmpV = rotateY(tmpV, cameras[i].rot[1]);
	tmpV = rotateZ(tmpV, cameras[i].rot[2]);

	imshow("Thresholded " + to_string(i+1), imgThresholded);
	imgOriginal = imgOriginal + cameras[i].imgLines;
	imshow("Original " + to_string(i+1), imgOriginal);
	
	return tmpV;
}

Vector3d rotateX(Vector3d v, double angle) {
	Matrix3d m = Matrix3d::Identity();
	m(1, 1) = cos(angle*M_PI / 180);
	m(1, 2) = -sin(angle*M_PI / 180);
	m(2, 1) = sin(angle*M_PI / 180);
	m(2, 2) = cos(angle*M_PI / 180);
	return m*v;
}

Vector3d rotateY(Vector3d v, double angle) {
	Matrix3d m = Matrix3d::Identity();
	m(0, 0) = cos(angle*M_PI / 180);
	m(0, 2) = sin(angle*M_PI / 180);
	m(2, 0) = -sin(angle*M_PI / 180);
	m(2, 2) = cos(angle*M_PI / 180);
	return m*v;
}

Vector3d rotateZ(Vector3d v, double angle) {
	Matrix3d m = Matrix3d::Identity();
	m(0, 0) = cos(angle*M_PI / 180);
	m(0, 1) = -sin(angle*M_PI / 180);
	m(1, 0) = sin(angle*M_PI / 180);
	m(1, 1) = cos(angle*M_PI / 180);
	return m*v;
}