#ifndef TSTRACKERTHREAD_H
#define TSTRACKERTHREAD_H

// Include some functions
#include "TStracker.h"

// For OpenCV gui
#include "cvui.h"

//// For Spinnaker Cameras
#include "ImageInfo.h"
#include "ImageSaver.h"

#define GENERAL_BUTTON_HEIGHT 33
#define CAPTION_HEIGHT 30
#define WINDOW_PADDING 10

extern string ALL_CAM_RECORD_WINDOWS_NAME;

class CamAcquireGUIThreadInfo
{
public:
	string camSerial;
	PropertyGridDlg* propDialog;
	CameraPtr*	camPtr;
	boolean runGUI;					// true -> the GUI for the camera is displayed, false otherwise
	boolean acquireSignal;			// Controlling the image acquisition process of this camera, if false, the image acquiring loop will be terminated
	boolean runRecord;				// True means recording is running, flase means no recording in progress
	boolean cameraInitStatus;		// Controlling the camera Init() status, if false, the camPtr->DeInit() will be called
	boolean threadStatus;			// false means the thread associated with the object has been terminated
	CWinThread* threadObjectPtr;	// pointer to the created thread object


									// The state of the acquire loop, if true -> acquiring and 
public:
	/*
	@Param run
	Acqui: Controlling the image acquisition process of this camera, if false, the image acquiring loop will be terminated
	@Param cInitStatus: Controlling the camera Init() status, if false, the camPtr->DeInit() will be called
	@Param threadStt: false means the thread associated with the object has been terminated, should be true when creating a new thread
	*/
	CamAcquireGUIThreadInfo(
		AFX_THREADPROC threadProc,		// The function this thread will run
		string cSerial,
		CameraPtr* camPTR,
		GUI::GUIFactory gui,
		boolean runGui = true,			// Make it true to start the gui loop, to show the initial GUI of the camera. No acquisition yet
		boolean acsignal = false,		// Make it false to prevent OpenCV automatically run acquisition when user just click on the camera
		boolean runRecord = false,		// No recording when starting
		boolean cInitStatus = true,
		boolean threadStt = true);		// True means thread is alive
	
	void CleanUpThreadInfo();
};


// Using openCV to show image in a different thread
UINT __cdecl openCVCamTuning(LPVOID camPtr);

// Run the all cameras record process
UINT __cdecl openCVAllCamRecord(LPVOID para);

// Draw GUI components
void drawGUI(Mat& frame, Mat& imgFrame, int& imgWidth, int& imgHeight, int& imgSize, int& frameRate, CameraPtr& pCam, INodeMap& nodeMap, CamAcquireGUIThreadInfo* threadInfo);

// Drawing the GUI for all cam capture
void drawGUIAllCam(Mat& displayFrame, vector<ImageInfo>& camCapImg, CamAcquireGUIThreadInfo* threadInfo, CameraList camList);

// Execute the camrecord all process
void RunRecordAll(CamAcquireGUIThreadInfo* threadInfo);

// This function acquires and saves 10 images from a device.
// @para runSignal: a boolean variable instrcuts the acquiring loop when to stop
int AcquireAndShowImages(CameraPtr pCam, INodeMap& nodeMap, INodeMap& nodeMapTLDevice, CamAcquireGUIThreadInfo* threadInfo);

// This function acts as the body of the example; please see NodeMapInfo example
// for more in-depth comments on setting up cameras.
//int RunAcquisition(CameraPtr pCam, boolean* runAcquireSignal, boolean* camStatus);
int RunAcquisition(CamAcquireGUIThreadInfo* threadInfo);

// Congifuring all available cameras
// Return true if succesfully initializing all cameras
bool configAllCams4SimultenousRecording(CameraList& camList);

void initAllCam(CameraList& camList);

void deinitAllCam(CameraList& camList);

// This function configures IEEE 1588 settings on each camera
// It enables IEEE 1588
bool ConfigureIEEE1588(const CameraList& camList);


// Configure the external trigger to use when IEEE1588 is not available
bool ConfigureExternalTrigger();

// Activate acquisition on all camera
bool runAcquisitionAllCams(CameraList& camList);

// Running the OpenCV GUI of recording all
void runGUIRecordAllCams(CamAcquireGUIThreadInfo* threadInfo, CameraList& camList);
#endif