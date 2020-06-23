#include "ImageDistributor.h"

#define QUEUE_THRES 1000
#define TRIAL_START_INDEX 1
#define MAX_CONCURRENT_SAVER 5				// The highest number of saver can concurrently run

ImageDistributor::ImageDistributor(RAWQueue* rawQueue, GUIQueue* guiQueue, string camSerial, ThreadSafeVariable<bool>* imageMinerStopped, ThreadSafeVariable<bool>* distributionStopped, string savePath)
	:savePath(savePath)
	,rawQueue(rawQueue)
	,guiQueue(guiQueue)
	,camSerial(camSerial)
	, imageMinerStopped(imageMinerStopped)
	, distributionStopped(distributionStopped)
	, currentSaveQueueTotalImageCounter(0)
	,imageSaverCounter(0)
	,trialCounter(TRIAL_START_INDEX)
	,maxSaverQueue(MAX_CONCURRENT_SAVER)
{
	this->saverQueue = new SAVERQueue();
	this->currentSaver = NULL;
	this->getNewImageSaver(false);
	// run the thread
	this->imageDistributorThreadHandler = AfxBeginThread(runDistribution, this);
}

ImageDistributor::~ImageDistributor()
{
	delete this->rawQueue;
	// Make sure no writer is running before delete the queue
	while (this->saverQueue->size());
	delete this->saverQueue;
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

		// Check if this is for a new trial
		if (ts2->trialNumber != this->trialCounter)
		{
			// Update trial number
			this->trialCounter = ts2->trialNumber;

			//// Get a new saver for this trial
			this->getNewImageSaver(true);
		}

		// Distribute one copy to be saved
		this->currentSaver->addToSave(ts2);

		// Increase total iamge couner for the current saver
		this->currentSaveQueueTotalImageCounter++;


		// Checking if we should move to the a different file
		if (this->currentSaveQueueTotalImageCounter == QUEUE_THRES)
		{
			// Get new ImageSaver
			this->getNewImageSaver(false);
		}
	}	
}

ImageSaverTSQ * ImageDistributor::getNewImageSaver(bool resetSaverCounter)
{
	// Detach current imageSaver
	if (this->currentSaver != NULL)
	{
		this->currentSaver->Detach();
	}

	if (this->saverQueue != NULL)
	{
		// Only create new saver when the number of currently running saver of this 
		// CamRecorder is below the limit
		// if not, let's wait for one saver to stop
		while (this->saverQueue->size() >= this->maxSaverQueue);
	}

	// Get new ImageSaver
	this->currentSaver = new ImageSaverTSQ(generateFileName(savePath, camSerial, trialCounter, imageSaverCounter), saverQueue);

	// Put the new saver to the saverQueue to be monitored
	if (this->saverQueue != NULL)
	{
		this->saverQueue->enqueue(this->currentSaver);
	}

	// Reset total image counter of current saver
	this->currentSaveQueueTotalImageCounter = 0;

	// Update imageSavercounter
	if (resetSaverCounter)
	{
		this->imageSaverCounter = 0;
	}
	else
	{
		// Increase imageSaver counter
		this->imageSaverCounter++;
	}

	return this->currentSaver;
}

string generateFileName(string savePath, string camSerial, unsigned trailNumber, unsigned fileNumber)
{
	if (savePath.size())
	{
		return 	savePath + "\\" + camSerial + "_trial_" + to_string(trailNumber) + "_" + to_string(fileNumber);
	}
	else
	{
		return 	camSerial + "_trial_" + to_string(trailNumber) + "_" + to_string(fileNumber);
	}
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


	// Wait for all saver to close 
	while (controller->saverQueue->size());

	// Let CVDisplay know that this ImageDistributor has stopped distributing images (no more image go to queue)
	controller->distributionStopped->write(true);


	// free everything associate with ImageDistributor
	delete controller;

	return 0;
}