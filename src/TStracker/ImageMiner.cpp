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
	//MessageBox(NULL, "Img -1", "Error", MB_OK);
	SystemPtr sys = System::GetInstance();
	//MessageBox(NULL, "Img 0", "Error", MB_OK);
	CameraList camList = sys->GetCameras();
	CameraPtr cam = camList.GetByIndex(miner->camIndex);
	//MessageBox(NULL, "Img 1", "Error", MB_OK);
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
	while (miner->loopRunning->read()) {
		try
		{
			// Obtain an image from the camera
			//MessageBox(NULL, "Img 4", "Error", MB_OK);
			img = cam->GetNextImage(WAIT_TIME);
			if (img->IsIncomplete())
			{
				continue;
			}

			// Create a copy of the obtained Spinnaker image by converting to TSImage object
			//MessageBox(NULL, "Img 5", "Error", MB_OK);
			tsimg = new TSImage();
			tsimg->getFromImgPtr(img);

			// Put the TSImage object to the Raw queue
			//MessageBox(NULL, "Img 6", "Error", MB_OK);
			miner->rawQueue->enqueue(tsimg);

			// Release the Spinnaker Image to free buffer memmory
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
		camList.Clear();
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