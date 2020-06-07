#include "ImageAcquirer.h"


ImageAcquirer::ImageAcquirer()
{
	CameraList tmpCamlist;
	this->imgQueueList = new ImageAQList(tmpCamlist.GetSize());
	this->controller = new ImageAcquirerController(this->imgQueueList);
	// Create and run Image acquirer thread for each camera
	AfxBeginThread(runImageAcquiringLoop, this->controller);
}

ImageAcquirer::~ImageAcquirer()
{
	delete this->imgQueueList;
	delete this->controller;
}

ImageAcquirerController::ImageAcquirerController()
{
	this->run = true;	// Automatically run the acquiring loop
	this->imgQueueList = NULL;
	this->mtx = CreateMutex(NULL, FALSE, NULL);
}

ImageAcquirerController::ImageAcquirerController(ImageAQList * imgQueueList, bool run): imgQueueList(imgQueueList), run(run)
{
	this->mtx = CreateMutex(NULL, FALSE, NULL);
}

ImageAcquirerController::~ImageAcquirerController()
{
	this->run = false;			// Make sure the loop is stopped
	CloseHandle(this->mtx);		// Delte the handle
}


// Run image acquisition from all camera
UINT __cdecl runImageAcquiringLoop(LPVOID para)
{

	// parameter is a pointer to a ImageAcquirerController
	// Path to the folder is sent thru cSerial camSerial parameter
	ImageAcquirerController * controller = (ImageAcquirerController*)para;

	// Setting before run acquisition all camera
	CameraList camList;		// List of camera

	// TODO 2: Implement this function
	setupAllCams(camList);

	while (true)
	{
		// Getting image from each camera and push it to the queue
		// TODO 1: implement this loop, or you might want to have a thread to acquire from each camera?
	}
	return 0;
}

