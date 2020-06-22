#include "CamRecorder.h"

CamRecorder::CamRecorder(int index, string camSerial,string savePath, uint64_t waitTime):
	camIndex(index)
	,waitTime(waitTime)
	,camSerial(camSerial)
	,savePath(savePath)
{
	this->rawQueue = new RAWQueue();
	this->guiQueue = new GUIQueue();
	this->imageMinerStopped = new ThreadSafeVariable<bool>(false);
	this->distributionStopped= new ThreadSafeVariable<bool>(false);		// if the ImageDistributor stopped yet
	this->imgMiner = new ImageMiner(index, this->rawQueue,this->imageMinerStopped,waitTime);
	this->imgDist = new ImageDistributor(this->rawQueue, this->guiQueue, camSerial , this->imageMinerStopped, this->distributionStopped,savePath);
	this->mtx = CreateMutex(NULL, FALSE, NULL);
}

CamRecorder::~CamRecorder()
{
	//delete this->rawQueue;	// This is not needed because rawQueue will be free by ImageDistributor
	//delete this->imgMiner;	// ImageMiner frees itself
	//delete this->imgDist;		// imgDist frees itself
	delete this->guiQueue;
	delete this->imageMinerStopped;
	delete this->distributionStopped;
	CloseHandle(this->mtx);
}

void CamRecorder::Detach()
{
	this->imgMiner->Terminate();		// Terminate Image miner to imageMinerStopped adding more image to the rawQueue
}
