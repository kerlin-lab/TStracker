#include "CamRecorder.h"

CamRecorder::CamRecorder(int index, string camSerial)
{
	this->camIndex = index;
	this->camSerial = camSerial;
	this->rawQueue = new RAWQueue();
	this->guiQueue = new GUIQueue();
	this->imgMiner = new ImageMiner(index, this->rawQueue);
	this->imgDist = new ImageDistributor(this->rawQueue, this->guiQueue, camSerial , this->imgMiner->imageMiningStopped);
	this->mtx = CreateMutex(NULL, FALSE, NULL);
}

CamRecorder::~CamRecorder()
{
	// TODO 6: implement the comment below
	//delete this->rawQueue;	// This is not needed because rawQueue will be free by ImageDistributor
	// TODO 5: implement the comment below
	//delete this->guiQueue;	// This is not needed because rawQueue will be closed by CVDisplay
	//delete this->imgMiner;	// ImageMiner frees itself
	//delete this->imgDist;		// imgDist frees itself
	CloseHandle(this->mtx);
}

void CamRecorder::Detach()
{
	this->imgMiner->Terminate();		// Terminate Image miner to imageMinerStopped adding more image to the rawQueue
}
