#ifndef TSTRACKERTHREAD_H
#define TSTRACKERTHREAD_H

#include "TStracker.h"
#include "cvui.h"


class CamAcquireThreadInfo
{
public:
	string camSerial;
	PropertyGridDlg* propDialog;
	CameraPtr*	camPtr;
	boolean runAcquisition;			// Controlling the image acquisition process of this camera, if false, the image acquiring loop will be terminated
	boolean cameraInitStatus;		// Controlling the camera Init() status, if false, the camPtr->DeInit() will be called
	boolean threadStatus;			// false means the thread associated with the object has been terminated
	CWinThread* threadObjectPtr;	// pointer to the created thread object
public:
	/*
	@Param run
	Acqui: Controlling the image acquisition process of this camera, if false, the image acquiring loop will be terminated
	@Param cInitStatus: Controlling the camera Init() status, if false, the camPtr->DeInit() will be called
	@Param threadStt: false means the thread associated with the object has been terminated, should be true when creating a new thread
	*/
	CamAcquireThreadInfo(string cSerial,
		CameraPtr* camPTR,
		GUI::GUIFactory gui,
		boolean runAcqui = true,			// Make it false to prevent OpenCV automatically run acquisition when user just click on the camera
		boolean cInitStatus = true,
		boolean threadStt = true
	);
};


// Using openCV to show image in a different thread
UINT __cdecl openCVCamCapture(LPVOID camPtr);


// This function acquires and saves 10 images from a device.
// @para runSignal: a boolean variable instrcuts the acquiring loop when to stop
int AcquireAndShowImages(CameraPtr pCam, INodeMap& nodeMap, INodeMap& nodeMapTLDevice, boolean* runSignal);

// This function acts as the body of the example; please see NodeMapInfo example
// for more in-depth comments on setting up cameras.
//int RunAcquisition(CameraPtr pCam, boolean* runAcquireSignal, boolean* camStatus);
int RunAcquisition(CameraPtr pCam, boolean* runAcquireSignal, boolean* camStatus);


#endif