#include "ImageMiner.h"

ImageMiner::ImageMiner(int index, RAWQueue* destQueue)
{
	this->cam = camList.GetByIndex(index);
	this->rawQueue = destQueue;
	this->handler = AfxBeginThread(spawnImageMiner, this);
}

void ImageMiner::Terminate()
{
	this->stop = true;
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