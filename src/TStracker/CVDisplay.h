#pragma once
#ifndef CV_DISPLAY_H_
#define CV_DISPLAY_H_

// Windows lib
#include <afxwin.h>

// For OpenCV gui
#include "cvui.h"

// TStracker lib
#include "TStrackerType.h"
#include "ThreadSafeQueue.h"
#include "TSImage.h"
#include "TStracker.h"

// Standard lib
#include <vector>



typedef ThreadSafeQueue<ImagePtr> RAWQueue;
typedef RAWQueue* RAWQueuePtr;


typedef ThreadSafeQueue<TSImage*> GUIQueue;
typedef GUIQueue * GUIQueuePtr;

typedef std::vector<GUIQueue*> GUIQueueList;
typedef GUIQueueList* GUIQueueListPtr;

typedef vector<TSImage*> TSImageList;

class CVDisplay
{
public:

	CVDisplay();
	~CVDisplay();

	// Adding a pointer to a GUIQueue to the monitor list of the CVDisplay
	void Attach(GUIQueuePtr);

	// Launching the CVGUI thread of this class. Once the thread is launched, queueList is immutable
	void launchGUI();

	GUIQueueListPtr queueList;			// Memmory provided by this will be the deconstructor of this class
	bool guiIsRunning;
};

UINT __cdecl cvGUIRunProc(LPVOID para);

void runGUI(GUIQueueListPtr guiQueue);

void drawGUIAllCam(Mat& displayFrame, TSImageList& imgList);

#endif  // CV_DISPLAY_H