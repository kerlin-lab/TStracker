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
typedef ThreadSafeVariable<unsigned> SaverCounter;
typedef ThreadSafeVariable<bool> BoolFlag;

class ImageDistributor
{
public:
	ImageDistributor(RAWQueue* rawQueue, GUIQueue* guiQueue,string camSerail,ThreadSafeVariable<bool>* imageMinerStopped, ThreadSafeVariable<bool>* distributionStopped,string savePath);
	~ImageDistributor();
	void Distribute();
public:
	RAWQueue * rawQueue;
	GUIQueue * guiQueue;
	SaverCounter * currentRunningSaverCounter;
	BoolFlag * imageMinerStopped;
	BoolFlag * distributionStopped;
	ImageSaverTSQ * currentSaver;
	string camSerial;
	string savePath;
	uint64_t currentSaveQueueTotalImageCounter;
	uint64_t imageSaverCounter;
	uint64_t trialCounter;
	unsigned maxSaverQueue;
	CWinThread * imageDistributorThreadHandler;
	ImageSaverTSQ * getNewImageSaver(bool resetSaverCounter);
};

string generateFileName(string savePath, string camSerial, unsigned trailNumber, unsigned fileNumber);

UINT __cdecl runDistribution(LPVOID para);
#endif //_IMAGE_DISTRIBUTOR_H_
