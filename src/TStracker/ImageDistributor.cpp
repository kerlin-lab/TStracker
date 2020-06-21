#include "ImageDistributor.h"

#define QUEUE_THRES 2000

ImageDistributor::ImageDistributor(RAWQueue* rawQueue, GUIQueue* guiQueue, string camSerial, ThreadSafeVariable<bool>* imageMinerStopped)
{
	this->rawQueue = rawQueue;
	this->guiQueue = guiQueue;
	this->camSerial = camSerial;
	this->imageMinerStopped = imageMinerStopped;
	this->distributionStopped = new  ThreadSafeVariable<bool>(false);
	this->currentSaveQueueTotalImageCounter = 0;
	this->imageSaverCounter = 0;
	this->trailCounter = 0;
	this->currentSaver = getNewImageSaver(camSerial,this->trailCounter,this->imageSaverCounter);
	// run the thread
	this->imageDistributorThreadHandler = AfxBeginThread(runDistribution, this);
}

ImageDistributor::~ImageDistributor()
{
	delete this->rawQueue;
	delete this->imageMinerStopped;
}


void ImageDistributor::Distribute()
{
	// Distribute one image from raw to the according save

	// Get the image
	if (!this->rawQueue->size())
	{
		// if there is image in the queue
		ImagePtr img = this->rawQueue->dequeue();
		// Duplicating the image
		TSImage* ts1 = new TSImage();
		ts1->getFromImgPtr(img);
		TSImage* ts2 = new TSImage();
		ts2->getFromImgPtr(img);
		// Release the memeory of the Raw image
		img->Release();

		// Distribute one copy to be displayed
		this->guiQueue->enqueue(ts1);

		// Distribute one copy to be saved
		this->currentSaver->addToSave(ts2);
		this->currentSaveQueueTotalImageCounter++;

		// Checking if we should move to the a different file
		if (this->currentSaveQueueTotalImageCounter == QUEUE_THRES)
		{
			// Yes move to new file

			// Reset counter
			this->currentSaveQueueTotalImageCounter = 0;
			// Detach current imageSaver
			this->currentSaver->Detach();
			// Get new ImageSaver
			this->imageSaverCounter++;
			this->currentSaver = getNewImageSaver(camSerial, this->trailCounter, this->imageSaverCounter);
		}
	}	
}

ImageSaverTSQ * getNewImageSaver(string camSerial, unsigned trailNumber, unsigned fileNumber)
{
	return new ImageSaverTSQ(generateFileName(camSerial, trailNumber, trailNumber));
}

string generateFileName(string camSerial, unsigned trailNumber, unsigned fileNumber)
{
	return 	camSerial + "_" + to_string(trailNumber) + "_" + to_string(fileNumber);
}

UINT __cdecl runDistribution(LPVOID para)
{
	ImageDistributor * controller = (ImageDistributor *)para;
	while (true)
	{
		if (controller->imageMinerStopped->read() && !controller->rawQueue->size())
		{
			controller->distributionStopped->write(true);
			break;
		}
		else
		{
			controller->Distribute();
		}
	}

	// Close the current writing file
	controller->currentSaver->Detach();
	// free everything associate with ImageDistributor
	delete controller;
	return 0;
}