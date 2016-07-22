/////////////////////////////////////////////////////////////////////////////
// BALL CATCHER
// By BaranCODE
// This program detects an object with OpenCV, triangulates its location,
// and calculates its trajectory. It is used to power a robot that catches balls.
//
// NOTES:
// - A right-handed coordinate system is used.
// - The ball moves along the X-axis
//////////////////////////////////////////////////////////////////////////////

#include "Header.h"

int main(int argc, char** argv)
{
	Serial* arduino = new Serial(port);

	if (!arduino->IsConnected()) {
		cout << "Could not connect to Arduino on " << port << endl;
		Sleep(3000);
		return -1;
	}

	loadSettings();

	int numCams = CLEyeGetCameraCount();
	if (numCams != 2)
	{
		cout << "Only found " << numCams << " camera(s)! Need 2 to run" << endl;
		Sleep(3000);
		return -1;
	}
	cout << "Found " << numCams << " cameras" << endl;

	setupScreen();
	setupCamera(0);
	setupCamera(1);

	int key;
	char *buffer = "";
	while ((key = cvWaitKey(0)) != 0x1b) {
		// The cameras do their thing in a separate thread
		// Just send estimated X Y coordinates to Arduino here
		if (estimated) {
			sprintf(buffer, "%ix%i", estXY[0], estXY[1]);
			arduino->WriteData(buffer, strlen(buffer));
		}
	}

	arduino->~Serial();
	for (int i = 0; i < numCams; i++)
	{
		cout << "Stopping capture on camera " << i + 1 << endl;
		cam[i]->StopCapture();
		delete cam[i];
	}
	return 0;
}