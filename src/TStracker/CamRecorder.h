#ifndef _CAM_RECORDER_H_
#define _CAM_RECORDER_H_

// TStracker libs

#include "ImageMiner.h"
#include "ImageDistributor.h"
#include "ThreadSafeQueue.h"
#include "ThreadSafeVariable.h"

// Spinview lib
// Spinview lib
// General API
#include "Spinnaker.h"
#include "SpinGenApi\SpinnakerGenApi.h"

using namespace Spinnaker;

typedef ThreadSafeQueue<TSImage*> RAWQueue;
typedef ThreadSafeQueue<TSImage*> GUIQueue;

class CamRecorder
{
public:
	int camIndex;
	string camSerial;
	RAWQueue * rawQueue;			// Queue to save spinaker image obtained from camera
	GUIQueue * guiQueue;			// Queue used to save TSimage will be displayed by CVDisplay
	ImageMiner * imgMiner;			// Image mineer thread
	ImageDistributor * imgDist;		// Image distributor thread
	ThreadSafeVariable<bool>* imageMinerStopped;		// if the imageMiner thread has stopped yet
	ThreadSafeVariable<bool>* distributionStopped;		// if the ImageDistributor stopped yet
	HANDLE mtx;						// This is for thread-safe access

public:
	CamRecorder(int index, string camSerial);
	~CamRecorder();

	// A way for RunOperator to signal termination
	void Detach();
};


#endif // _CAM_RECORDER_H_
