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
#include "CamRecorder.h"

// Standard lib
#include <vector>



typedef ThreadSafeQueue<ImagePtr> RAWQueue;
typedef RAWQueue* RAWQueuePtr;


typedef ThreadSafeQueue<TSImage*> GUIQueue;
typedef GUIQueue * GUIQueuePtr;

typedef std::vector<GUIQueue*> GUIQueueList;
typedef GUIQueueList* GUIQueueListPtr;

typedef vector<TSImage*> TSImageList;

typedef vector<CamRecorder*> CamRecorderPtrList;
typedef vector<CamRecorder*> * CamRecorderPtrListPtr;

class CVDisplay
{
public:

	CVDisplay(CamRecorderPtrListPtr camRecorderList, ThreadSafeVariable<bool> * running);
	~CVDisplay();

	// Launching the CVGUI thread of this class. Once the thread is launched, queueList is immutable
	void launchGUI();

	// Get the ith GUIQueue
	GUIQueuePtr getGUIQueue(unsigned i);	

	// Get the ith GUIQueuee
	GUIQueuePtr at(unsigned i);

	// Get number of GUIQueue
	unsigned getNumberGUIQueue();					

	// Get number of GUIQueue
	unsigned size();

	// Stop the acquiring 
	void stopAcquiring();

	// Check if all imageDistributor stopped
	bool isAllDistributorStopped();

public:
	CamRecorderPtrListPtr camRecorderList;			// List of all camrecorder
	ThreadSafeVariable<bool> * guiIsRunning;
	bool stopSignalSent;							// Whether a signal to imageMinerStopped recording has been sent
	CWinThread * guiThreadHandle;

};

UINT __cdecl cvGUIRunProc(LPVOID para);

void runGUI(GUIQueueListPtr guiQueue);

void drawGUIAllCam(Mat& displayFrame, TSImageList& imgList);

#endif  // CV_DISPLAY_H