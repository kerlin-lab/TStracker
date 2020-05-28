#include "ImageSaver.h"

// Don't put this in header file otherwise you will get LNK2005
std::string DEFAULT_EXTENSION = ".avi";

ImageSaver::ImageSaver()
{
    this->saveQueue = new ContainerType();
    this->threadController = new SavingThreadController<ContainerType>(random_string() + DEFAULT_EXTENSION,this->saveQueue);
    // Create the thread
    this->threadObject = AfxBeginThread(savingThreadProcessor,this->threadController);
}

ImageSaver::ImageSaver(std::string fileName)
{
    this->saveQueue = new ContainerType();
    this->threadController = new SavingThreadController<ContainerType>(fileName + DEFAULT_EXTENSION,this->saveQueue);
    // Create the thread
    this->threadObject = AfxBeginThread(savingThreadProcessor,this->threadController);
}

// Copy constructor
ImageInfo::ImageInfo(const ImageInfo &obj)
{
    this->img= obj.img.clone();
    this->imgWidth = obj.imgWidth;
    this->imgHeight = obj.imgHeight;
    this->imgSize = obj.imgSize;
	this->camSerial = obj.camSerial;
}

// Assignment = operator
ImageInfo& ImageInfo::operator= (const ImageInfo& obj)
{
	this->img = obj.img.clone();
	this->imgWidth = obj.imgWidth;
	this->imgHeight = obj.imgHeight;
	this->imgSize = obj.imgSize;
	this->camSerial = obj.camSerial;
	return *this;
}



ImageSaver::~ImageSaver()
{
	// Release Mat object in queue
	while (saveQueue->size())
	{
		saveQueue->front()->img.release();
		saveQueue->pop();
	}
	// Delete the queue
	delete saveQueue;
	// Delete the controller object
    delete threadController;
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
	GetExitCodeThread(this->threadObject->m_hThread, &exitCode);
    return exitCode == STILL_ACTIVE;
}

// Adding the item to to-be-saved list
void ImageSaver::addToSave(ItemType item)
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
	unsigned listSize;
	SavingThreadController<ContainerType>* threadController = (SavingThreadController<ContainerType>*) para;
    
    // TODO N: Check the saving procedure below
    WaitForSingleObject(threadController->mtx,INFINITE);
    // TODO 1: Implement the OpenFIleToWirte function
    //threadController->fileIsOpen = OpenStorage();
    ReleaseMutex(threadController->mtx);

    // Running the loop to save everything in the container down to file
    // TODO 4: implement IsExternalSignalDone()
	while(threadController->fileIsOpen)
    //while(threadController->fileIsOpen || threadController->size() || IsExternalSignalDone())
    {
		WaitForSingleObject(threadController->mtx, INFINITE);
		listSize = threadController->container->size();
		ReleaseMutex(threadController->mtx);

		while (listSize--)
		{
			// Thread-safe sake, also, make sure the run time between WaitForSingleObject and ReleaseMutex is as short as possible since the GUI is blocked between these functions
			WaitForSingleObject(threadController->mtx, INFINITE);
			// Save pointer to the image
			image = threadController->container->front();
			// Remove image from container
			threadController->removeFromToSave();
			// This release must be as close as possible to WaitForSingleObject to reduce the processing time since the GUI window is blocked between these 2 functions
			ReleaseMutex(threadController->mtx);
			// TODO 2: Implement the saving function
			//SavetoStore(image);
			// Release the memory
			//if (getWindowProperty("Image received", cv::WND_PROP_VISIBLE) <= 0.5)
			//{
			//	//Windows is closed or does not exist
			//	// Create a OpenCV window for displaying
			//	namedWindow("Image received");
			//}
			//cv::imshow("Image received", image->img);
			//waitKey(1);
			//Sleep(100);
			delete image;
		}
    }
	//cv::destroyWindow("Image received");


    // TODO 3: implement the storage close function
    // Closing and flush everything to the file
    //CloseStorage();
	//MessageBox(NULL, "here2", "Error", MB_OK); 
	return 0;
}

// Generate a random string with given size
string random_string(size_t length)
{
    const string CHARACTERS = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

    random_device random_device;
    mt19937 generator(random_device());
    uniform_int_distribution<> distribution(0, CHARACTERS.size() - 1);

    string random_string;

    for (size_t i = 0; i < length; ++i)
    {
        random_string += CHARACTERS[distribution(generator)];
    }

    return random_string;
}

