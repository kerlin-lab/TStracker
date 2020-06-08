#include "CamRecorder.h"

CamRecorder::CamRecorder(int index)
{
	this->camIndex = index;
	this->rawQueue = new RAWQueue();
	this->guiQueue = new GUIQueue();
	this->imgMiner = new ImageMiner(index, this->rawQueue);
	// TODO 3: Adding this constructor to ImageDistributor
	this->imgDist = new ImageDistributor(this->rawQueue, this->guiQueue, QUEUE_THRES);
	this->mtx = CreateMutex(NULL, FALSE, NULL);
}

CamRecorder::~CamRecorder()
{
	// TODO 6: implement the comment below
	//delete this->rawQueue;	// This is not needed because rawQueue will be free by ImageDistributor
	// TODO 5: implement the comment below
	//delete this->guiQueue;	// This is not needed because rawQueue will be closed by CVDisplay
	// These 2 lines below is not needed becaues ImageMiner and imgDistributor will free memeory themself when Terminate is called
	//delete this->imgMiner;
	//delete this->imgDist;
	CloseHandle(this->mtx);
}

void CamRecorder::Detach()
{
	this->imgMiner->Terminate();		// Terminate Image miner to stop adding more image to the rawQueue
}

UINT __cdecl spawnImageMiner(LPVOID params)
{
	ImageMiner* miner = (ImageMiner*)params;
	while (!miner->stop) {
		ImagePtr img = miner->cam->GetNextImage(3000);
		miner->rawQueue->enqueue(img);
	}
	delete miner;
}
