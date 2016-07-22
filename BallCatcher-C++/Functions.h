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

	// HSV threshold values (configurable via UI)
	// TODO: Add config file for these <-------
	minHue = 21;
	maxHue = 61;
	minSat = 173;
	maxSat = 255;
	minValue = 24;
	maxValue = 140;
}

void setupScreen() {
	std::cout << "PRESS ESC TO QUIT" << endl;
	namedWindow("Settings", 2);

	createTrackbar("Enabled", "Settings", &dataCollectEnabled, 1);

	createTrackbar("Min Hue", "Settings", &minHue, 255); // Hue (0 - 255)
	createTrackbar("Max Hue", "Settings", &maxHue, 255);
	createTrackbar("Min Sat.", "Settings", &minSat, 255); // Saturation (0 - 255)
	createTrackbar("Max Sat.", "Settings", &maxSat, 255);
	createTrackbar("Min Value", "Settings", &minValue, 255); // Value (0 - 255)
	createTrackbar("Max Value", "Settings", &maxValue, 255);
}

void setupCamera(int i) {
	GUID guid = CLEyeGetCameraUUID(i);
	printf("Camera %d GUID: [%08x-%04x-%04x-%02x%02x%02x%02x%02x%02x%02x%02x]\n",
		i + 1, guid.Data1, guid.Data2, guid.Data3,
		guid.Data4[0], guid.Data4[1], guid.Data4[2],
		guid.Data4[3], guid.Data4[4], guid.Data4[5],
		guid.Data4[6], guid.Data4[7]);
	cam[i] = new CLEyeCameraCapture(guid,
		CLEYE_COLOR_RAW,
		CLEYE_VGA, 60);
	cam[i]->setID(i);
	printf("Starting capture on camera %d\n", i + 1);
	cam[i]->StartCapture();

	// The following code was for using a standard webcam through OpenCV (instead of PS3 Eye)
	/*cameras[i].cam = VideoCapture(i);
	cameras[i].cam.set(CV_CAP_PROP_FRAME_WIDTH, 640);
	cameras[i].cam.set(CV_CAP_PROP_FRAME_HEIGHT, 480);
	if (!cameras[i].cam.isOpened()) {
	std::cout << "Error opening webcam #" << i + 1 << endl;
	}

	Mat imgTmp;// Capture a temporary image from the camera
	cameras[i].cam.read(imgTmp);
	cameras[i].imgLines = Mat::zeros(imgTmp.size(), CV_8UC3); // Create a black image with size matching camera's
	std::cout << "Camera #" << i + 1 << ", Height: " << imgTmp.size().height << ", Width: " << imgTmp.size().width << endl;*/
}

void processCamera(int i, Mat imgOriginal) {
	Mat imgThresholded;

	/*if (!cameras[i].cam.read(imgOriginal)) {
	std::cout << "Error reading frame on webcam #" << i + 1 << endl;
	return Vector3d::Zero();
	}*/

	cvtColor(imgOriginal, imgThresholded, CV_BGR2HSV); // Convert captured frame from BGR to HSV
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

		circle(imgOriginal, Point(posX, posY), 30, Scalar(0, 0, 255), -1);

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

	imshow("Thresholded " + to_string(i + 1), imgThresholded);
	imshow("Original " + to_string(i + 1), imgOriginal);

	waitKey(1);

	cameras[i].ballDir = tmpV;
}

void fpsPrint() {
	if (lastTick != 0) cout << "FPS: " << 1000 / (float)(GetTickCount() - lastTick) << endl;
	lastTick = GetTickCount();
}

void triangulate() {
	// This method, by ja72, gives us the intersection point of two
	// 3d lines (in the form of a point and direction vector).
	// If there is no intersection, it finds the point where the
	// lines are nearest.

	Vector3d v1 = cameras[0].ballDir;
	Vector3d v2 = cameras[1].ballDir;

	v1 = v1 / v1.norm();
	v2 = v2 / v2.norm();

	double u = v1.dot(v2);
	double t1 = (cameras[1].pos - cameras[0].pos).dot(v1);
	double t2 = (cameras[1].pos - cameras[0].pos).dot(v2);

	double d1 = (t1 - u*t2) / (1 - u*u);
	double d2 = (t2 - u*t1) / (u*u - 1);

	Vector3d p = ((cameras[0].pos + d1 * v1) + (cameras[1].pos + d2 * v2)) / 2;
	xCoords.push_back(p[0]);
	yCoords.push_back(p[1]);
	zCoords.push_back(p[2]);
	//cout << "Point:\n" << p << endl;
}

void trajectoryCalc() {
	// Use the ball's recorded positions to calculate
	// its trajectory, and find where it will fall.
	if (zCoords.size() >= 3) {
		polynomialfit(zCoords.size(), 3, &xCoords[0], &zCoords[0], XZcoeff);
		// Solve equation for x: 0 = x*x*XZcoeff[2] + x*XZcoeff[1] + XZcoeff[0] - catcherHeight
		double x = 0;
		for (int i = -1; i <= 1; i += 2) {
			double tmpx = (-XZcoeff[1] + i * sqrt(XZcoeff[1] * XZcoeff[1] - 4 * XZcoeff[2] * (XZcoeff[0] - catcherHeight))) / (2 * XZcoeff[2]);
			if (i == -1) x = tmpx;
			else if (tmpx > x) x = tmpx;
		}
		cout << "X: " << x << endl;

		polynomialfit(xCoords.size(), 2, &xCoords[0], &yCoords[0], XYcoeff);
		// Solve equation for y (given x): y = mx + c
		double y = XYcoeff[1] * x + XYcoeff[0];
		cout << "Y: " << y << endl;

		// TODO: Check for bounds !!!!!!!!!!!!!!!!
		// TODO: Do conversion from coordinates to motor steps !!!!!!!!!!!!
		estXY[0] = (int)x;
		estXY[1] = (int)y;
		estimated = true;
	}
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

bool polynomialfit(int obs, int degree, double *dx, double *dy, double *store) {
	gsl_multifit_linear_workspace *ws;
	gsl_matrix *cov, *X;
	gsl_vector *y, *c;
	double chisq;

	int i, j;

	X = gsl_matrix_alloc(obs, degree);
	y = gsl_vector_alloc(obs);
	c = gsl_vector_alloc(degree);
	cov = gsl_matrix_alloc(degree, degree);

	for (i = 0; i < obs; i++) {
		for (j = 0; j < degree; j++) {
			gsl_matrix_set(X, i, j, pow(dx[i], j));
		}
		gsl_vector_set(y, i, dy[i]);
	}

	ws = gsl_multifit_linear_alloc(obs, degree);
	gsl_multifit_linear(X, y, c, cov, &chisq, ws);

	for (i = 0; i < degree; i++)
	{
		store[i] = gsl_vector_get(c, i);
	}

	gsl_multifit_linear_free(ws);
	gsl_matrix_free(X);
	gsl_matrix_free(cov);
	gsl_vector_free(y);
	gsl_vector_free(c);
	return true;
}