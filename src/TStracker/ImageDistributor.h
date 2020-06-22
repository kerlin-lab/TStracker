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

typedef ThreadSafeQueue<TSImage*> RAWQueue;
typedef ThreadSafeQueue<TSImage*> GUIQueue;

class ImageDistributor
{
public:
	ImageDistributor(RAWQueue* rawQueue, GUIQueue* guiQueue,string camSerail,ThreadSafeVariable<bool>* imageMinerStopped, ThreadSafeVariable<bool>* distributionStopped,string savePath);
	~ImageDistributor();
	void Distribute();
public:
	RAWQueue* rawQueue;
	GUIQueue* guiQueue;
	ThreadSafeVariable<bool>* imageMinerStopped;
	ThreadSafeVariable<bool>* distributionStopped;
	ImageSaverTSQ * currentSaver;
	string camSerial;
	string savePath;
	uint64_t currentSaveQueueTotalImageCounter;
	uint64_t imageSaverCounter;
	uint64_t trialCounter;
	CWinThread * imageDistributorThreadHandler;
};

string generateFileName(string savePath, string camSerial, unsigned trailNumber, unsigned fileNumber);

ImageSaverTSQ * getNewImageSaver(string savePath, string camSerial, unsigned trailNumber, unsigned fileNumber);

UINT __cdecl runDistribution(LPVOID para);
#endif //_IMAGE_DISTRIBUTOR_H_
