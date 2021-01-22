#ifndef TSTRACKER_H
#define TSTRACKER_H

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
#include <opencv2/videoio/videoio_c.h>
#include <opencv2/highgui/highgui_c.h>

using namespace cv;


// TStracker lib
#include "TSRecordingSetting.h"

// Global variables
extern HANDLE mtx;
extern TSRecordingSetting recorderSetting;


// OpenCV gui
#include "cvui.h"

#define APP_NAME "TStracker"

#define TEXT_OFFSET 5

#define CODEC CV_FOURCC('M','J','P','G')


// Return time from running acquisition in sub milisecond
uint64_t getReadableTimestamp(uint64_t timestamp);

void drawTime(Mat& frame, double time);

void drawTimeAndFPS(Mat& frame, double time, int fps);

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
void ImagePtr2CVMat_CV_8UC1(ImagePtr& spin_con, Mat& cv_con, unsigned size);


// Get infor
void retriveImageInfo(INodeMap& nodeMap, Mat& imgFrame, int& imgWidth, int& imgHeight, int& imgSize, int& frameRate, CameraPtr& pCam);

// Processing recording feature
void runRecordFeature(bool runRecord, Mat imgFrame, int imgWidth, int imgHeight, int frameRate, VideoWriter& vOut, string file_prefix, string videoType);
#endif // !TStracker_H