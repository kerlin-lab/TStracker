#include "ImageDistributor.h"

#define QUEUE_THRES 2000

ImageDistributor::ImageDistributor(RAWQueue* rawQueue, GUIQueue* guiQueue, string camSerial, ThreadSafeVariable<bool>* imageMinerStopped, ThreadSafeVariable<bool>* distributionStopped)
{
	this->rawQueue = rawQueue;
	this->guiQueue = guiQueue;
	this->camSerial = camSerial;
	this->imageMinerStopped = imageMinerStopped;
	this->distributionStopped = distributionStopped;
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
}


void ImageDistributor::Distribute()
{
	// Distribute one image from raw to the according save
	// Get the image
	//MessageBox(NULL, "0", "Error", MB_OK);
	if (this->rawQueue->size())
	{
		// if there is image in the queue, take it off
		TSImage * img = this->rawQueue->dequeue();

		// Duplicating the image and send a copy to be displayed

		// Duplicating
		TSImage* ts1 = new TSImage(*img);

		// Distribute one copy to be displayed
		this->guiQueue->enqueue(ts1);

		// Send the original image to Saver to be saved to disk
		TSImage* ts2 = img;
		//delete img;

		// Distribute one copy to be saved
		this->currentSaver->addToSave(ts2);

		// Increase total iamge couner for the current saver
		this->currentSaveQueueTotalImageCounter++;


		// Checking if we should move to the a different file
		if (this->currentSaveQueueTotalImageCounter == QUEUE_THRES)
		{
			// Yes move to new file

			// Reset counter
			this->currentSaveQueueTotalImageCounter = 0;

			// Detach current imageSaver
			this->currentSaver->Detach();

			// Increase imageSaver counter
			this->imageSaverCounter++;

			// Get new ImageSaver
			this->currentSaver = getNewImageSaver(camSerial, this->trailCounter, this->imageSaverCounter);

		}
	}	
}

ImageSaverTSQ * getNewImageSaver(string camSerial, unsigned trailNumber, unsigned fileNumber)
{
	return new ImageSaverTSQ(generateFileName(camSerial, trailNumber, fileNumber));
}

string generateFileName(string camSerial, unsigned trailNumber, unsigned fileNumber)
{
	return 	camSerial + "_" + to_string(trailNumber) + "_" + to_string(fileNumber);
}

UINT __cdecl runDistribution(LPVOID para)
{
	ImageDistributor * controller = (ImageDistributor *)para;
	//MessageBox(NULL, "Get Here 1", "Error", MB_OK);
	while (true)
	{
		if (controller->imageMinerStopped->read() && !controller->rawQueue->size())
		{
			break;
		}
		else
		{
			//MessageBox(NULL, "Get Here Distribute", "Error", MB_OK);
			controller->Distribute();
		}
	}

	// Close the current writing file
	controller->currentSaver->Detach();

	// Let CVDisplay know that this ImageDistributor has stopped distributing images (no more image go to queue)
	controller->distributionStopped->write(true);

	// free everything associate with ImageDistributor

	delete controller;

	return 0;
}