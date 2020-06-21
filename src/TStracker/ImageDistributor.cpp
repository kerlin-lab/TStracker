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
	if (this->rawQueue->size())
	{
		MessageBox(NULL, "1", "Error", MB_OK);
		// if there is image in the queue
		TSImage * img = this->rawQueue->dequeue();
		// Duplicating the image
		MessageBox(NULL, "2", "Errorasd", MB_OK);
		MessageBox(NULL, to_string(img->timestamp).c_str(), "Errorasd", MB_OK);
		TSImage* ts1 = img;
		//MessageBox(NULL, "3", "Error", MB_OK);
		//ts1->getFromImgPtr(img);
		//MessageBox(NULL, "4", "Error", MB_OK);
		TSImage* ts2 = img;
		//MessageBox(NULL, "5", "Error", MB_OK);
		//ts2->getFromImgPtr(img);
		// Release the memeory of the Raw image
		//img->Release();

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
			MessageBox(NULL, "Get Here", "Error", MB_OK);
			controller->Distribute();
		}
	}

	// Close the current writing file
	controller->currentSaver->Detach();
	// free everything associate with ImageDistributor
	delete controller;
	return 0;
}