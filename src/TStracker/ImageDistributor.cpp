#include "ImageDistributor.h"

ImageDistributor::ImageDistributor(RAWQueue* rawQueue, GUIQueue* guiQueue, int maxSize)
{
	this->rawQueue = rawQueue;
	this->guiQueue = guiQueue;
	this->saveQueue.push_back(new SaveQueue());
	this->maxSize = maxSize;
}

void ImageDistributor::Terminate()
{
	this->stop = true;
}

void ImageDistributor::Distribute()
{
	// Distribute one image from raw to the according save
	ImagePtr img = this->rawQueue->dequeue();
	TSImage* ts1 = new TSImage();
	ts1->getFromImgPtr(img);
	TSImage* ts2 = new TSImage();
	ts2->getFromImgPtr(img);
	img->Release();
	this->guiQueue->enqueue(ts1);

	std::vector<SaveQueue*>::size_type size = this->saveQueue.size();
	for (int i = 0; i < size; i++)
	{
		WaitForSingleObject(this->saveQueue[i]->mtx, INFINITE);
		if (this->saveQueue[i]->)
	}
}