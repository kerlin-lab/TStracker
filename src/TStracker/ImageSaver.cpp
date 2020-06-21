#include "ImageSaver.h"


ImageSaver::ImageSaver()
{
    this->saveQueue = new ImageSaverContainerType();
    this->threadController = new SavingThreadController<ImageSaverContainerType>(getNoNExistFileName(random_string()),this->saveQueue);
    // Create the thread
    this->threadObject = AfxBeginThread(savingThreadProcessor,this->threadController);
}

ImageSaver::ImageSaver(std::string fileName)
{
    this->saveQueue = new ImageSaverContainerType();
    this->threadController = new SavingThreadController<ImageSaverContainerType>(getNoNExistFileName(fileName),this->saveQueue);
    // Create the thread
    this->threadObject = AfxBeginThread(savingThreadProcessor,this->threadController);
}


ImageSaver::~ImageSaver()
{
	// Release Mat object in queue
	//while (saveQueue->size())
	//{
	//	saveQueue->front()->img.release();
	//	saveQueue->pop();
	//}
}

HANDLE ImageSaver::getThreadMutex()
{
    return this->threadController->mtx;
}

template <typename T>
SavingThreadController<T>::SavingThreadController(string fileName, T* container, bool fileIsOpen)
{
    this->fileName =  fileName;
    this->container = container;
    this->fileIsOpen = fileIsOpen;
	this->mtx = CreateMutex(NULL, FALSE, NULL);
	if (mtx == NULL)
	{
		MessageBox(NULL,"Error", "CreateMutex error",0);
	}
}

// Check if there is a saving thread ready to save item 
bool ImageSaver::isOpen()
{
    return this->threadController->fileIsOpen;
}

// Check if the saving thread is still running
bool ImageSaver::isThreadRunning()
{
	DWORD exitCode;
	GetExitCodeThread(HANDLE(this->threadObject), &exitCode);
    return exitCode == STILL_ACTIVE;
}

// Adding the item to to-be-saved list
void ImageSaver::addToSave(ImageSaverItemType item)
{
    WaitForSingleObject(this->getThreadMutex(),INFINITE);
	this->saveQueue->push(item);
    ReleaseMutex(this->getThreadMutex());
}

// Taken the last item off the to-be-saved list
// NOTE: This function DOES NOT RELEASE the memory contained by the removed item 
// So make sure you implement memory releasing by yourself to prevent memmory leak
template <typename T>
void SavingThreadController<T>::removeFromToSave()
{
	WaitForSingleObject(this->mtx, INFINITE);
	if (this->container->size())
	{
		// Only pop if there is something in the list
		this->container->pop();
	}
	ReleaseMutex(this->mtx);
}


// Signal thread to terminate
// This function is thread-safe
void ImageSaver::signalTermination()
{
    WaitForSingleObject(this->getThreadMutex(),INFINITE);
    this->threadController->fileIsOpen = false;
    ReleaseMutex(this->getThreadMutex());
}

// Processor of the the saving thread
// Parameter is a pointer pointing to an SavingThreadController object 
UINT __cdecl savingThreadProcessor(LPVOID para)
{
	ImageInfo* image;
	TiffWriter* tfWriter;
	int listSize, c_stackSize,tiff_part;

	listSize = 0;
	c_stackSize = 0;
	tiff_part = 0;

	SavingThreadController<ImageSaverContainerType>* threadController = (SavingThreadController<ImageSaverContainerType>*) para;
    
    // TODO N: Check the saving procedure below
    WaitForSingleObject(threadController->mtx,INFINITE);
	try
	{
		// Attempting to create a TIFF file handle to write to
		tfWriter = TiffWriter::OpenNewTIFFtoWrite(threadController->fileName+string("_")+to_string(tiff_part));
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

	try
	{
		while (true)
			//while(threadController->fileIsOpen || threadController->size() || IsExternalSignalDone())
		{
			//MessageBox(NULL, "here1", threadController->fileName.c_str(), MB_OK);
			while (listSize--)
			{
				//MessageBox(NULL, "here2", threadController->fileName.c_str(), MB_OK);

				// Thread-safe sake, also, make sure the run time between WaitForSingleObject and ReleaseMutex is as short as possible since the GUI is blocked between these functions
				WaitForSingleObject(threadController->mtx, INFINITE);
				// Save pointer to the image
				image = threadController->container->front();
				// Remove image from container
				threadController->container->pop();
				// This release must be as close as possible to WaitForSingleObject to reduce the processing time since the GUI window is blocked between these 2 functions
				ReleaseMutex(threadController->mtx);

				// Save the obtained image to the tiff file
				tfWriter->SavetoTIFFFile(image);

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

				// Update stack size counter of this stack
				c_stackSize++;

				// Check if we need to write to a different tiff file to maintain writing speed
				if (c_stackSize == MAX_TIFF_STACK_SIZE)
				{
					//MessageBox(NULL, "Splitting", "Noted", MB_OK);

					// Close current tiff file
					TiffWriter::CloseTIFFFile(tfWriter);
					// Update tiff file counter
					tiff_part++;
					// Open new file to write
					tfWriter = TiffWriter::OpenNewTIFFtoWrite(threadController->fileName + string("_") + to_string(tiff_part));
					// Restart stack size counter
					c_stackSize = 0;
				}
			}

			//MessageBox(NULL, "here3", threadController->fileName.c_str(), MB_OK);

			// Check the number of images in the queue
			WaitForSingleObject(threadController->mtx, INFINITE);
			listSize = threadController->container->size();
			ReleaseMutex(threadController->mtx);

			//MessageBox(NULL, "here4", threadController->fileName.c_str(), MB_OK);

			if (!(threadController->fileIsOpen || listSize))
			{
				//MessageBox(NULL, "break", threadController->fileName.c_str(), MB_OK);
				break;
			}
		}
	}
	catch (int e)
	{
		MessageBox(NULL, "Error saving", "Error", MB_OK);
	}
	//cv::destroyWindow("Image received");

    // Closing and save everything to the file
    TiffWriter::CloseTIFFFile(tfWriter);

	//// Cleaning up all allocated memory used for this thread
	// Free the memory of the list associating with this thread
	delete threadController->container;
	// Free memory of the SavingThreadController object of this thread
	delete threadController;

	//MessageBox(NULL, "Saving thread terminating", "Error", MB_OK); 
	return 0;
}

