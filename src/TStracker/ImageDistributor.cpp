#include "ImageDistributor.h"

#define QUEUE_THRES 2000					// Maximum number of tiff image per tiff file
#define TRIAL_START_INDEX 1					// First trial index
#define MAX_CONCURRENT_SAVER 5				// The highest number of saver can concurrently run
#define WAIT_TILL_ONE_SAVER_TERMINATE 100	

ImageDistributor::ImageDistributor(RAWQueue* rawQueue, GUIQueue* guiQueue, string camSerial, ThreadSafeVariable<bool>* imageMinerStopped, ThreadSafeVariable<bool>* distributionStopped, string savePath)
	:savePath(savePath)
	,rawQueue(rawQueue)
	,guiQueue(guiQueue)
	,camSerial(camSerial)
	, imageMinerStopped(imageMinerStopped)
	, distributionStopped(distributionStopped)
	, currentSaveQueueTotalImageCounter(0)
	,imageSaverCounter(UINT64_MAX)
	,trialCounter(TRIAL_START_INDEX)
	,maxSaverQueue(MAX_CONCURRENT_SAVER)
{
	this->currentRunningSaverCounter = new SaverCounter(0);
	this->currentSaver = NULL;
	this->getNewImageSaver(false);
	// run the thread
	this->imageDistributorThreadHandler = AfxBeginThread(runDistribution, this);
}

ImageDistributor::~ImageDistributor()
{
	delete this->rawQueue;
	// Make sure no writer is running before delete the queue
	this->currentRunningSaverCounter->waitTillZero();
	delete this->currentRunningSaverCounter;
}


void ImageDistributor::Distribute()
{
	//// Distribute one image from raw to the current saver
	
	// Get the image
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

		// Increase total iamge counter for the current saver
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

	if (this->currentRunningSaverCounter != NULL)
	{
		// Only create new saver when the number of currently running saver of this 
		// CamRecorder is below the limit
		// if not, let's wait for one saver to stop
		unsigned tmp;
		do
		{
			tmp = this->currentRunningSaverCounter->read();
			SwitchToThread();
		} while (tmp >= this->maxSaverQueue);
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

	// Get new ImageSaver
	this->currentSaver = new ImageSaverTSQ(generateFileName(savePath, camSerial, trialCounter, imageSaverCounter), currentRunningSaverCounter);

	// Update saver counter
	if (this->currentRunningSaverCounter != NULL)
	{
		WaitForSingleObject(this->currentRunningSaverCounter->mtx, INFINITE);
		this->currentRunningSaverCounter->var++;
		ReleaseMutex(this->currentRunningSaverCounter->mtx);
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

	// Making this thread a high priority
	SetThreadPriority(controller->imageDistributorThreadHandler, THREAD_PRIORITY_ABOVE_NORMAL);

	// Distribute
	while (true)
	{
		if (controller->imageMinerStopped->read() && !controller->rawQueue->size())
		{
			// Stop distributing when the imageMiner of the camera has stopped and there is no more image in the rawQueue to be distributed
			break;
		}
		else
		{
			// Otherwise, distribute the image to the saver
			controller->Distribute();
		}
	}

	// Close the current writing file
	controller->currentSaver->Detach();

	// Wait for all saver to close 
	controller->currentRunningSaverCounter->waitTillZero();

	// Let CVDisplay know that this ImageDistributor has stopped distributing images (no more image go to queue)
	controller->distributionStopped->write(true);


	// free everything associate with ImageDistributor
	delete controller;

	return 0;
}