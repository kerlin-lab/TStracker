#include "ImageMiner.h"

#define WAIT_TIME 3000

ImageMiner::ImageMiner(int index, RAWQueue* destQueue)
{
	this->cam = camList.GetByIndex(index);
	this->rawQueue = destQueue;
	this->handler = AfxBeginThread(spawnImageMiner, this);
	this->imageMiningStopped = new ThreadSafeVariable<bool>(false);
	this->loopRunning = new ThreadSafeVariable<bool>(true);
}

ImageMiner::~ImageMiner()
{
	delete loopRunning;
}

void ImageMiner::Terminate()
{
	this->loopRunning->write(false);
}

UINT __cdecl spawnImageMiner(LPVOID params)
{
	ImageMiner* miner = (ImageMiner*)params;
	ImagePtr img;
	while (miner->loopRunning->read()) {
		img = miner->cam->GetNextImage(WAIT_TIME);
		miner->rawQueue->enqueue(img);
	}
	miner->imageMiningStopped->write(true);
	delete miner;
	return 0;
}