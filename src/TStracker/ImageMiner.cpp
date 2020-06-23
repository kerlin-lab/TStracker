#include "ImageMiner.h"

#define DEFAULT_WAIT_TIME 41
#define TRIAL_COUNTER_START 1

ImageMiner::ImageMiner(int index, RAWQueue* destQueue, ThreadSafeVariable<bool>* imageMiningStopped, uint64_t waitTime)
{	
	this->camIndex = index;
	this->rawQueue = destQueue;
	this->imageMiningStopped = imageMiningStopped;
	this->loopRunning = new ThreadSafeVariable<bool>(true);
	this->waitTime = (waitTime == 0 ? DEFAULT_WAIT_TIME : waitTime);				// Should use 1/2 of wait time to detect trial stop
	// Getting camera Serial
	SystemPtr sys = System::GetInstance();
	CameraList camList = sys->GetCameras();
	CameraPtr cam = camList.GetByIndex(index);
	cam->Init();
	this->camSerial = cam->DeviceSerialNumber();
	if (!cam->IsStreaming())
	{
		cam->DeInit();
	}
	camList.Clear();
	sys->ReleaseInstance();
	// Start running the imageMiner thread
	this->handler = AfxBeginThread(spawnImageMiner, this);
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
	uint64_t initialTimestamp = 0;

	// Making this thread a high priority
	SetThreadPriority(miner->handler, THREAD_PRIORITY_TIME_CRITICAL);

	// Getting the handle of the camera
	SystemPtr sys = System::GetInstance();
	CameraList camList = sys->GetCameras();
	CameraPtr cam = camList.GetByIndex(miner->camIndex);

	// Setting up the camera
	try
	{
		cam->Init();
		//MessageBox(NULL, "Img 2", "Error", MB_OK);
		cam->BeginAcquisition();
		//MessageBox(NULL, "Img 3", "Error", MB_OK);
	}
	catch (Spinnaker::Exception e)
	{
		MessageBox(NULL, (string("Unable to start acquisition of all cameras due to ") + string(e.what())).c_str(), "Error", MB_OK);
		return false;
	}

	// Run acquiring loop
	TSImage * tsimg;
	bool lastTrialEnd = true;
	unsigned trialCounter = TRIAL_COUNTER_START;

	while (miner->loopRunning->read()) 
	{
		try
		{
			// Obtain an image from the camera
			//MessageBox(NULL, "Img 4", "Error", MB_OK);
			img = cam->GetNextImage(miner->waitTime);
			if (img->IsIncomplete())
			{
				continue;
			}

			// still receiving image, so not end yet 
			lastTrialEnd = false;

			// Create a copy of the obtained Spinnaker image by converting to TSImage object
			//MessageBox(NULL, "Img 5", "Error", MB_OK);
			tsimg = new TSImage();
			tsimg->getFromImgPtr(img);
			// Inject other information (camera , trailnumber, timestamp normalize ...)
			tsimg->camSerial = miner->camSerial;
			tsimg->trialNumber = trialCounter;
			// Normalized timestamp (otherwise, timestamp will start with a very large number)
			if (initialTimestamp == 0)
			{
				initialTimestamp = tsimg->timestamp;
			}
			tsimg->timestamp -= initialTimestamp;

			// Put the TSImage object to the Raw queue
			miner->rawQueue->enqueue(tsimg);

			// Release the Spinnaker Image to free buffer memmory
			img->Release();
		}
		catch (Spinnaker::Exception e)
		{
			bool needToBreak ;
			switch (e.GetError())
			{
			case SPINNAKER_ERR_TIMEOUT:
				// This can happens when we read the camera and there is no image on the RAM
				// Or after the wait time, no image is received

				// there is a 2-second break between each trial, detect this break and change to new trial
				if (miner->waitTime != DEFAULT_WAIT_TIME)
				{
					// Check if trial scheme is activated
					if (!lastTrialEnd)
					{
						lastTrialEnd = true;
						trialCounter++;
					}
				}

				needToBreak = false;
				break;
			default:
				MessageBox(NULL, (string("Unable to start acquisition of all cameras due to ") + string(e.what())).c_str(), "Error", MB_OK);
				needToBreak = true;
			}
			if (needToBreak)
			{
				break;
			}
		}
	}

	// Safely release the camera
	try 
	{
		cam->EndAcquisition();
		cam->DeInit();
		camList.Clear();
		sys->ReleaseInstance();
	}
	catch (Spinnaker::Exception e)
	{
		MessageBox(NULL, (string("Unable to start acquisition of all cameras due to ") + string(e.what())).c_str(), "Error", MB_OK);
	}

	// Announce
	miner->imageMiningStopped->write(true);


	// Free memmory
	delete miner;
	MessageBox(NULL, (string("Image miner stopped") ).c_str(), "Yup", MB_OK);

	return 0;
}