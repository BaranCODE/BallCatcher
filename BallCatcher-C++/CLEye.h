class CLEyeCameraCapture
{
	GUID _cameraGUID;
	CLEyeCameraInstance _cam;
	CLEyeCameraColorMode _mode;
	CLEyeCameraResolution _resolution;
	int _fps;
	HANDLE _hThread;
	bool _running;

	int _id;
public:
	CLEyeCameraCapture(GUID cameraGUID,
		CLEyeCameraColorMode mode,
		CLEyeCameraResolution resolution, int fps) :
		_cameraGUID(cameraGUID), _cam(NULL), _mode(mode),
		_resolution(resolution), _fps(fps), _running(false){}

	void setID(int i) {
		_id = i;
	}

	bool StartCapture()
	{
		_running = true;
		//cvNamedWindow(_windowName, CV_WINDOW_AUTOSIZE);
		_hThread = CreateThread(NULL, 0, &CLEyeCameraCapture::CaptureThread, this, 0, 0);
		if (_hThread == NULL)
		{
			MessageBox(NULL, "Could not create capture thread", "CLEyeMulticamTest", MB_ICONEXCLAMATION);
			return false;
		}
		return true;
	}

	void StopCapture()
	{
		if (!_running)   return;
		_running = false;
		WaitForSingleObject(_hThread, 1000);
		//cvDestroyWindow(_windowName);
	}

	void IncrementCameraParameter(int param)
	{
		if (!_cam)   return;
		CLEyeSetCameraParameter(_cam,
			(CLEyeCameraParameter)param,
			CLEyeGetCameraParameter(_cam, (CLEyeCameraParameter)param) + 10);
	}

	void DecrementCameraParameter(int param)
	{
		if (!_cam)   return;
		CLEyeSetCameraParameter(_cam,
			(CLEyeCameraParameter)param,
			CLEyeGetCameraParameter(_cam, (CLEyeCameraParameter)param) - 10);
	}

	void Capture()
	{
		int w, h;
		IplImage *pCapImage;
		PBYTE pCapBuffer = NULL;
		// Create camera instance 
		_cam = CLEyeCreateCamera(_cameraGUID, _mode, _resolution, _fps);
		if (_cam == NULL)        return;
		// Get camera frame dimensions 
		CLEyeCameraGetFrameDimensions(_cam, w, h);
		// Depending on color mode chosen, create the appropriate OpenCV image 
		pCapImage = cvCreateImage(cvSize(w, h), IPL_DEPTH_8U, 4);
		// Set some camera parameters 
		/*CLEyeSetCameraParameter(_cam, CLEYE_GAIN, 20);
		CLEyeSetCameraParameter(_cam, CLEYE_EXPOSURE, 511);
		CLEyeSetCameraParameter(_cam, CLEYE_ZOOM, (int)(GetRandomNormalized()*100.0));
		CLEyeSetCameraParameter(_cam, CLEYE_ROTATION, (int)(GetRandomNormalized()*300.0));*/
		// Start capturing 
		CLEyeCameraStart(_cam);
		// image capturing loop 
		while (_running)
		{
			cvGetImageRawData(pCapImage, &pCapBuffer);
			CLEyeCameraGetFrame(_cam, pCapBuffer);
			//cvShowImage(_windowName, pCapImage);
			bool detected = processCamera(_id, cv::cvarrToMat(pCapImage));
			if (_id == 1) {
				fpsPrint();
				if (dataCollectEnabled == 1 && detected) {
					triangulate();
					trajectoryCalc();
				}
				else {
					if (xCoords.size() > 0) {
						xCoords.clear();
						yCoords.clear();
						zCoords.clear();
						estXY[0] = 0;
						estXY[1] = 0;
					}
				}
			}
		}
		// Stop camera capture 
		CLEyeCameraStop(_cam);
		// Destroy camera object 
		CLEyeDestroyCamera(_cam);
		// Destroy the allocated OpenCV image 
		cvReleaseImage(&pCapImage);
		_cam = NULL;
	}

	static DWORD WINAPI CaptureThread(LPVOID instance)
	{
		// forward thread to Capture function 
		CLEyeCameraCapture *pThis = (CLEyeCameraCapture *)instance;
		pThis->Capture();
		return 0;
	}
};