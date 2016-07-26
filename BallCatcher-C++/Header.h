#include <iostream>
#include <math.h>
#include <Windows.h>
#include <string>
#include <vector>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/legacy/compat.hpp>
#include <opencv2/opencv.hpp>
#include <Eigen/Dense>
#include <gsl/gsl_multifit.h>
#include <stdbool.h>
#include <CLEyeMulticam.h>

#include "SerialClass.h"

int i = 0;

using namespace cv;
using namespace std;
using namespace Eigen;

void setupScreen();
void setupCamera(int i);
bool processCamera(int i, Mat imgOriginal);
Vector3d rotateX(Vector3d v, double angle);
Vector3d rotateY(Vector3d v, double angle);
Vector3d rotateZ(Vector3d v, double angle);
bool polynomialfit(int obs, int degree, double *dx, double *dy, double *store);
void fpsPrint();
void triangulate();
void trajectoryCalc();
void arduinoSend();

int demo = 1;
int dataCollectEnabled = 0;
vector<double> xCoords;
vector<double> yCoords;
vector<double> zCoords;
double XZcoeff[3] = { 0 };
double XYcoeff[2] = { 0 };

int estXY[2] = { 0 }; // sum of estimated values

#include "CLEye.h" // This needs to be here b/c it uses the functions, namespaces, variables above

char *port = "COM4";

class Camera {
public:
	double rot[3];
	double rot2[3];
	Vector3d pos;
	double planeDist;
	double planeX;
	double planeY;
	double screenX;
	double screenY;
	Vector3d ballDir;
};
Camera cameras[2];

CLEyeCameraCapture *cam[2];

int minHue, maxHue, minSat, maxSat, minValue, maxValue;

int catcherHeight = 15; // Height of robot end-effector, in cm (at what height the ball will be caught)
double minX = 8.7; // Minimum distance of center of catcher to origin, in cm
double minY = 8.7;
double maxX = 52.7;
double maxY = 48.2;

int lastTick = 0;

char buffer[256] = "";

Serial* arduino;

#include "Functions.h"