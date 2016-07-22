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

using namespace cv;
using namespace std;
using namespace Eigen;

void setupScreen();
void setupCamera(int i);
void processCamera(int i, Mat imgOriginal);
Vector3d rotateX(Vector3d v, double angle);
Vector3d rotateY(Vector3d v, double angle);
Vector3d rotateZ(Vector3d v, double angle);
bool polynomialfit(int obs, int degree, double *dx, double *dy, double *store);
void fpsPrint();
void triangulate();
void trajectoryCalc();

int dataCollectEnabled = 0;

#include "CLEye.h" // This needs to be here b/c it uses the functions, namespaces, variables above

char *port = "COM4";

class Camera {
public:
	double rot[3];
	Vector3d pos;
	double planeDist;
	double planeX;
	double planeY;
	int screenX;
	int screenY;
	Vector3d ballDir;
};
Camera cameras[2];

CLEyeCameraCapture *cam[2];

int minHue, maxHue, minSat, maxSat, minValue, maxValue;

int catcherHeight = 15; // Height of robot end-effector, in cm (at what height the ball will be caught)

vector<double> xCoords;
vector<double> yCoords;
vector<double> zCoords;
double XZcoeff[3] = { 0 };
double XYcoeff[2] = { 0 };

int estXY[] = { 0 };
bool estimated = false;

int lastTick = 0;

#include "Functions.h"