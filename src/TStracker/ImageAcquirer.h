#ifndef _IMAGE_ACQUIRER_H_
#define _IMAGE_ACQUIRER_H_

// Standard Lib
#include <string>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <queue>

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

//// Win32 lib
//#include <Windows.h>

// OpenCV gui
#include "cvui.h"

#include "ThreadSafeQueue.h"

typedef ThreadSafeQueue<ImagePtr> InQueue;
typedef vector<InQueue * > ImageAQList;

class ImageAcquirerController
{
public:
	bool run;
	ImageAQList * imgQueueList;
	HANDLE mtx;			// Handle for threadsafe operation when changing member of this class

public:
	ImageAcquirerController();

	ImageAcquirerController(ImageAQList * imgQueueList, bool run = true);

	~ImageAcquirerController();

};

typedef vector<ImageAcquirerController *> ImageAcquirerList;

class ImageAcquirer
{
public:
	ImageAcquirerController * controller;
	ImageAcquirerList * imgQueueList;

public:
	ImageAcquirer();

	~ImageAcquirer();
};

UINT __cdecl runImageAcquiringLoop(LPVOID para);

#endif // _IMAGE_ACQUIRER_H_
