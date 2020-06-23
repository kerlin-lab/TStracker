#include "RunOperator.h"

RunOperator::RunOperator(string savePath, uint64_t waitTime):savePath(savePath),waitTime(waitTime)
{
	// Reserve memory
	this->camRecs = new CamRecorderPtrList();
	this->running = new ThreadSafeVariable<bool>(true);
	// Filling
	SystemPtr sys = System::GetInstance();
	CameraList camList = sys->GetCameras();
	CameraPtr cam;
	for (unsigned i = 0; i < camList.GetSize(); i++) {
		try
		{
			cam = camList.GetByIndex(i);
			cam->Init();
			CamRecorder * camRec = new CamRecorder(i, string(cam->DeviceSerialNumber()),savePath,waitTime);
			if (!cam->IsStreaming())
			{
				cam->DeInit();
			}
			camRecs->push_back(camRec);
		}
		catch (Spinnaker::Exception e)
		{
			MessageBox(NULL, (string("HereUnable to start acquisition of all cameras due to ") + string(e.what())).c_str(), "Error", MB_OK);
		}
	}
	//sys->ReleaseInstance();
	this->cvDisplay = new CVDisplay(this->camRecs,this->running);
}

RunOperator::~RunOperator()
{
	delete this->running;
	//delete this->camRecs;			// Delete by CVDisplay
}
