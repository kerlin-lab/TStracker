#include "TStrackerThread.h"

CamAcquireThreadInfo::CamAcquireThreadInfo(string cSerial, CameraPtr * camPTR, GUI::GUIFactory gui ,boolean runAcqui, boolean cInitStatus, boolean threadStt)
{
	// initionalizing
	this->camSerial = cSerial;
	this->camPtr = camPTR;
	this->runAcquisition = runAcqui;
	this->cameraInitStatus = cInitStatus;
	this->threadStatus = threadStt;

	// Open Acquisition and the property dialog box for the clicked camera

	// Initialize the camera 
	(*camPTR)->Init();														//Init the camera first otherwise, DialogBox->Connect(cam) will result in runtime errors
																			// Set up property dialog for this camera
	gui.ConnectGUILib(*camPTR);												// This some how prevent the camera selection box from disapearing after choosing a camera
	this->propDialog = new GUI::PropertyGridDlg();							// this can be replaced with new GUI::PropertyGridDlg();
	(this->propDialog)->Connect(*camPTR);									// Connect the dialog to the camera
	(this->propDialog)->Open();												// show the dialog
																			// Set up a thread uses OpenCV to acquire and save image
	this->threadObjectPtr = AfxBeginThread(openCVCamCapture, this);
}

// @para: a pointer to a CamAcquireThreadInfo of this thread
UINT __cdecl openCVCamCapture(LPVOID para)
{
	// parameter is a pointer to a CamAcquireThreadInfo
	CamAcquireThreadInfo* threadInfo = (CamAcquireThreadInfo*)para;
	CameraPtr pCam = *(threadInfo->camPtr);
	
	// Run acquisition
	RunAcquisition(pCam,&threadInfo->runAcquisition,&threadInfo->cameraInitStatus);
	
	// Marking thread is terminated
	threadInfo->threadStatus = false;
	return 0;
}