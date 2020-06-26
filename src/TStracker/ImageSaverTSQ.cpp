#include "ImageSaverTSQ.h"


ImageSaverTSQ::ImageSaverTSQ()
{
	this->fileName = getNoNExistFileName(random_string());
	this->container = new ImageSaverTSQContainerType();
	this->fileIsOpen = false;
	this->filling = true;
	this->mtx = CreateMutex(NULL, FALSE, NULL);
	if (mtx == NULL)
	{
		MessageBox(NULL, "Error", "CreateMutex error", 0);
	}

	// Create the thread
	this->threadObject = AfxBeginThread(savingThreadProcessorTSQ, this);

}

ImageSaverTSQ::ImageSaverTSQ(std::string fileName, SaverCounter * saverCounter, bool fileIsOpen, bool filling):
	fileName(fileName),
	fileIsOpen(fileIsOpen),
	filling(filling),
	currentRunningSaverCounter(saverCounter)
{
	this->container = new ImageSaverTSQContainerType();
	this->mtx = CreateMutex(NULL, FALSE, NULL);
	if (mtx == NULL)
	{
		MessageBox(NULL, "Error", "CreateMutex error", 0);
	}
	// Create the thread
	this->threadObject = AfxBeginThread(savingThreadProcessorTSQ, this);
}


ImageSaverTSQ::~ImageSaverTSQ()
{
	// Release Mat object in queue
	//while (saveQueue->size())
	//{
	//	saveQueue->front()->img.release();
	//	saveQueue->pop();
	//}
	delete this->container;
	CloseHandle(this->mtx);

}

HANDLE ImageSaverTSQ::getThreadMutex()
{
	return this->mtx;
}

// Check if there is a saving thread ready to save item 
bool ImageSaverTSQ::isOpen()
{
	return this->fileIsOpen;
}

// Check if the saving thread is still running
bool ImageSaverTSQ::isThreadRunning()
{
	DWORD exitCode;
	GetExitCodeThread(HANDLE(this->threadObject), &exitCode);
	return exitCode == STILL_ACTIVE;
}

// Adding the item to to-be-saved list
void ImageSaverTSQ::addToSave(ImageSaverTSQItemType item)
{
	this->container->enqueue(item);
}

// Taken the last item off the to-be-saved list
// NOTE: This function DOES NOT RELEASE the memory contained by the removed item 
// So make sure you implement memory releasing by yourself to prevent memmory leak
void ImageSaverTSQ::removeFromToSave()
{
	this->container->pop();
}
// Signal the the thread that the queue of this thread is no longer being filled
void ImageSaverTSQ::Detach()
{
	WaitForSingleObject(this->getThreadMutex(), INFINITE);
	this->filling= false;
	ReleaseMutex(this->getThreadMutex());
}

// Check if the queue has been detached
bool ImageSaverTSQ::isDetached()
{
	bool flag;
	WaitForSingleObject(this->getThreadMutex(), INFINITE);
	flag = !this->filling;
	ReleaseMutex(this->getThreadMutex());
	return flag;
}


// Signal thread to terminate
// This function is thread-safe
void ImageSaverTSQ::signalTermination()
{
	WaitForSingleObject(this->getThreadMutex(), INFINITE);
	this->fileIsOpen = false;
	ReleaseMutex(this->getThreadMutex());
}

// Processor of the the saving thread
// Parameter is a pointer pointing to an ImageSaverTSQ object 
UINT __cdecl savingThreadProcessorTSQ(LPVOID para)
{
	TSImagePtr image;
	TiffWriter* tfWriter;
	int listSize;

	listSize = 0;

	ImageSaverTSQ * threadController = (ImageSaverTSQ*) para;

	// TODO N: Check the saving procedure below
	WaitForSingleObject(threadController->mtx, INFINITE);
	try
	{
		// Attempting to create a TIFF file handle to write to
		tfWriter = TiffWriter::OpenNewTIFFtoWrite(threadController->fileName);
		threadController->fileIsOpen = true;
	}
	catch (unsigned e)
	{
		MessageBox(NULL, "Error openning tiff file to write", "Error", MB_OK);
		threadController->fileIsOpen = false;
		ReleaseMutex(threadController->mtx);
		return -1;
	}
	ReleaseMutex(threadController->mtx);

	// Running the loop to save everything in the container down to file
	// TODO N: implement IsExternalSignalDone()

	while (true)
		//while(threadController->fileIsOpen || threadController->size() || IsExternalSignalDone())
	{
		//MessageBox(NULL, "here1", threadController->fileName.c_str(), MB_OK);
		while (listSize--)
		{
			//MessageBox(NULL, "here2", threadController->fileName.c_str(), MB_OK);

			// Save pointer to the image
			image = threadController->container->dequeue();
			
			try
			{
				// Save the obtained image to the tiff file
				tfWriter->SavetoTIFFFile(image);
			}
			catch (int e)
			{
				MessageBox(NULL, "Error saving", "Error", MB_OK);
			}

			//// The commented snippet is to test how does the received image look like, just to make sure the queue does not give us trash
			//if (getWindowProperty("Image received", cv::WND_PROP_VISIBLE) <= 0.5)
			//{
			//	//Windows is closed or does not exist
			//	// Create a OpenCV window for displaying
			//	namedWindow("Image received");
			//}
			//cv::imshow("Image received", image->img);
			//waitKey(1);
			//Sleep(100);

			// Release the memory
			delete image;
		}

		//MessageBox(NULL, "here3", threadController->fileName.c_str(), MB_OK);

		// Check the number of images in the queue
		listSize = threadController->container->size();

		//MessageBox(NULL, "here4", threadController->fileName.c_str(), MB_OK);

		if (!threadController->fileIsOpen)
		{
			MessageBox(NULL, "Error cannot open file to write", threadController->fileName.c_str(), MB_OK);
			break;
		}


		if (listSize == 0 && threadController->isDetached())
		{
			//MessageBox(NULL, "break", threadController->fileName.c_str(), MB_OK);
			break;
		}

	}
	
	//cv::destroyWindow("Image received");

	// Closing and save everything to the file
	TiffWriter::CloseTIFFFile(tfWriter);

	// If currentRunningSaverCounter is given, decrease the running saver counter
	if (threadController->currentRunningSaverCounter != NULL)
	{
		WaitForSingleObject(threadController->currentRunningSaverCounter->mtx,INFINITE);
		//MessageBox(NULL, to_string(threadController->currentRunningSaverCounter->read()).c_str(), "Error", MB_OK);
		threadController->currentRunningSaverCounter->var--;
		//MessageBox(NULL, to_string(threadController->currentRunningSaverCounter->read()).c_str(), "Error", MB_OK);
		ReleaseMutex(threadController->currentRunningSaverCounter->mtx);
	}

	//// Cleaning up all allocated memory used for this thread

	// Free memory of the ImageSaverTSQ object of this thread
	string name = threadController->fileName;
	delete threadController;
	//MessageBox(NULL, name.c_str(), "Error", MB_OK);
	return 0;
}
