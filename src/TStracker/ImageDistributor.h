#ifndef _IMAGE_DISTRIBUTOR_H_
#define _IMAGE_DISTRIBUTOR_H_

#include "ThreadSafeQueue.h"
#include "ThreadSafeVariable.h"
#include "TSImage.h"
#include "Spinnaker.h"
#include "SpinGenApi\SpinnakerGenApi.h"
#include "ImageSaverTSQ.h"
#include <vector>

using namespace Spinnaker;

typedef ThreadSafeQueue<ImagePtr> RAWQueue;
typedef ThreadSafeQueue<TSImage*> GUIQueue;

class ImageDistributor
{
public:
	ImageDistributor(RAWQueue* rawQueue, GUIQueue* guiQueue,string camSerail,ThreadSafeVariable<bool>* imageMinerStopped);
	~ImageDistributor();
	void Distribute();
public:
	RAWQueue* rawQueue;
	GUIQueue* guiQueue;
	ThreadSafeVariable<bool>* imageMinerStopped;
	ThreadSafeVariable<bool>* distributionStopped;
	ImageSaverTSQ * currentSaver;
	string camSerial;
	unsigned currentSaveQueueTotalImageCounter;
	unsigned imageSaverCounter;
	unsigned trailCounter;
	CWinThread * imageDistributorThreadHandler;
};

string generateFileName(string camSerial, unsigned trailNumber, unsigned fileNumber);

ImageSaverTSQ * getNewImageSaver(string camSerial, unsigned trailNumber, unsigned fileNumber);

UINT __cdecl runDistribution(LPVOID para);
#endif //_IMAGE_DISTRIBUTOR_H_
