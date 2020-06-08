#ifndef _CAM_RECORDER_H_
#define _CAM_RECORDER_H_
// Spinview lib
// Spinview lib
// General API
#include "Spinnaker.h"
#include "SpinGenApi\SpinnakerGenApi.h"

using namespace Spinnaker;

// TStracker libs

#include "ImageMiner.h"
#include "ImageDistributor.h"
#include "ThreadSafeQueue.h"
#include "TSImage.h"
#include "TStrackerType.h"



class CamRecorder
{
public:
	int camIndex;
	RAWQueue * rawQueue;			// Queue to save spinaker image obtained from camera
	GUIQueue * guiQueue;			// Queue used to save TSimage will be displayed by CVDisplay
	ImageMiner * imgMiner;			// Image mineer thread
	ImageDistributor * imgDist;		// Image distributor thread
	HANDLE mtx;						// This is for thread-safe access

public:
	CamRecorder(int index);
	~CamRecorder();

	// A way for RunOperator to signal termination
	Detach();
};


#endif // _CAM_RECORDER_H_
