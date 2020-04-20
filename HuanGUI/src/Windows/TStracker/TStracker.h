#pragma once
/*
*  Program when run will acquire image from FLIR cam
*  extract timestamp of each image
*  and show the acquire image continuously with
*  the timestamp displayed in the bottom left corner
*/
// Standard Lib
#include <string>
#include <iostream>
#include <sstream>
#include <unordered_map>

using namespace std;

// For GUI thread
#include <afxwin.h>

// Spinview lib
// General API
#include "Spinnaker.h"
#include "SpinGenApi\SpinnakerGenApi.h"
// GUI API
#include "GUI\SpinnakerGUI.h"
#include "GUI\SpinnakerGUI_WPF.h"

using namespace Spinnaker;
using namespace Spinnaker::GenApi;
using namespace Spinnaker::GenICam;
using namespace Spinnaker::GUI;
using namespace Spinnaker::GUI_WPF;


// OpenCv lib
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

using namespace cv;

#define APP_NAME "TStracker"

#define TEXT_OFFSET 10

// Return time from running acquisition in sub milisecond
uint64_t getReadableTimestamp(uint64_t timestamp);

void drawTime(Mat& frame, double time);

void dbhere(int a = 0);

/*
*  Return a CONTINUOUS mono 8 Mat
*  with each pixel of type uchar and
*  each size width*height
*/
void createMono8Mat(Mat& frame, int width, int height);


/*
* The function convert a mono 8-bit gray scale image
* from Spinview ImagePtr to OpenCV Mat
* for the sake of performance
*
* NO type checking
* NO size checking
* the Input cv_con must be a continuous Mat (use the function createMono8Mat to get one)
* the size is width * height
*
* User need to do the task before calling the function
* ideally, the check should be done before acquisition takes place
*/
void ImagePtr2CVMat_CV_8UC1(ImagePtr& spin_con, Mat& cv_con, int size);

// This function acquires and saves 10 images from a device.
// @para runSignal: a boolean variable instrcuts the acquiring loop when to stop
int AcquireAndShowImages(CameraPtr pCam, INodeMap& nodeMap, INodeMap& nodeMapTLDevice, boolean* runSignal);

// This function acts as the body of the example; please see NodeMapInfo example
// for more in-depth comments on setting up cameras.
int RunAcquisition(CameraPtr pCam, boolean* runAcquireSignal, boolean* camStatus);




//int oldmain(int /*argc*/, char** /*argv*/);
