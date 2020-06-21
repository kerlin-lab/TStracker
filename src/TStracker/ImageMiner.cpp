#include "ImageMiner.h"

#define WAIT_TIME 500

ImageMiner::ImageMiner(int index, RAWQueue* destQueue)
{	
	this->camIndex = index;
	this->rawQueue = destQueue;
	this->imageMiningStopped = new ThreadSafeVariable<bool>(false);
	this->loopRunning = new ThreadSafeVariable<bool>(true);
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
	// Getting the handle of the camera
	SystemPtr sys = System::GetInstance();
	CameraPtr cam = sys->GetCameras().GetByIndex(miner->camIndex);
	// Setting up the camera
	try
	{
		cam->Init();
		cam->BeginAcquisition();
	}
	catch (Spinnaker::Exception e)
	{
		MessageBox(NULL, (string("Unable to start acquisition of all cameras due to ") + string(e.what())).c_str(), "Error", MB_OK);
		return false;
	}

	// Run acquiring loop
	TSImage * tsimg;
	while (miner->loopRunning->read()) {
		try
		{
			img = cam->GetNextImage(WAIT_TIME);
			tsimg = new TSImage();
			tsimg->getFromImgPtr(img);
			miner->rawQueue->enqueue(tsimg);
			MessageBox(NULL, to_string(img->GetTimeStamp()).c_str(), "Error", MB_OK);
			img->Release();
		}
		catch (Spinnaker::Exception e)
		{
			switch (e.GetError())
			{
			case SPINNAKER_ERR_TIMEOUT:
				// This happens when we read the camera and there is no image on the RAM
				// So do nothing
				MessageBox(NULL,"Test", "Error", MB_OK);
				break;
			default:
				MessageBox(NULL, (string("Unable to start acquisition of all cameras due to ") + string(e.what())).c_str(), "Error", MB_OK);
				return false;
			}
		}
	}

	// Safely release the camera
	try 
	{
		cam->EndAcquisition();
		cam->DeInit();
		sys->ReleaseInstance();
	}
	catch (Spinnaker::Exception e)
	{
		MessageBox(NULL, (string("Unable to start acquisition of all cameras due to ") + string(e.what())).c_str(), "Error", MB_OK);
		return false;
	}

	// Announce
	miner->imageMiningStopped->write(true);
	// Free memmory
	delete miner;
	return 0;
}