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
	arduino = new Serial("COM4");

	if (!arduino->IsConnected()) {
		cout << "Could not connect to arduino!" << endl;
		return -1;
	}
	cout << "Connected to arduino\nPriming..." << endl;

	while (buffer[0] != '#') { // check for response
		arduino->WriteData("-", 1); // send priming character
		Sleep(100);
		arduino->ReadData(buffer, 1);
	}
	strcpy(buffer, "");

	cout << "Robot ready\nPreparing cameras..." << endl;


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

	cout << "Cameras ready" << endl;

	int key;
	while ((key = cvWaitKey(0)) != 0x1b) { }

	for (int i = 0; i < numCams; i++)
	{
		cout << "Stopping capture on camera " << i + 1 << endl;
		cam[i]->StopCapture();
		delete cam[i];
	}
	return 0;
}