#include "RunOperator.h"
#include"stdafx.h"

RunOperator::RunOperator(string savePath, uint64_t waitTime):savePath(savePath),waitTime(waitTime)
{
	// Reserve memory
	this->camRecs = new CamRecorderPtrList();
	this->running = new ThreadSafeVariable<bool>(true);
	// Filling
	CameraPtr cam;
	vector<string> camSerial;

	// Getting Serial from each camera
	WaitForSingleObject(SpinSysMTX, INFINITE);
	CameraList cameraList = spinSystem->GetCameras();
	ReleaseMutex(SpinSysMTX);
	for (unsigned i = 0; i < cameraList.GetSize(); i++)
	{
		try
		{
			cam = cameraList.GetByIndex(i);
			cam->Init();
			camSerial.push_back((string)cam->DeviceSerialNumber());
		}
		catch (Spinnaker::Exception e)
		{
			MessageBox(NULL, (string("RunOperator unables to obtain device serial due to") + string(e.what())).c_str(), "Error", MB_OK);
		}
	}

	// Create CamRecorder for each camera
	for (unsigned i = 0; i < camSerial.size(); i++) 
	{
			CamRecorder * camRec = new CamRecorder(i, camSerial[i],savePath,waitTime);
			camRecs->push_back(camRec);
	}
	this->cvDisplay = new CVDisplay(this->camRecs,this->running);
}

RunOperator::~RunOperator()
{
	delete this->running;
}
