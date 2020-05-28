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
    this->threadController = new SavingThreadController<ContainerType>(fileName,this->saveQueue);
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


ImageSaver::~ImageSaver()
{
    delete saveQueue;
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


// Signal thread to terminate
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
    SavingThreadController<ContainerType>* threadController = (SavingThreadController<ContainerType>*) para;
    
    // TODO N: Check the saving procedure below
    WaitForSingleObject(threadController->mtx,INFINITE);
    // TODO 1: Implement the OpenFIleToWirte function
    //threadController->fileIsOpen = OpenStorage();
    ReleaseMutex(threadController->mtx);

    // Running the loop to save everything in the container down to file
    // TODO 4: implement IsExternalSignalDone()
	while(threadController->fileIsOpen || threadController->container->size())
    //while(threadController->fileIsOpen || threadController->size() || IsExternalSignalDone())
    {
        // Thread-safe sake
        WaitForSingleObject(threadController->mtx,INFINITE);
        // TODO 2: Implement the saving function
        //SavetoStore(threadController->container->back());
        // Remove image from container
        threadController->container->pop();
        ReleaseMutex(threadController->mtx);
    }


    // TODO 3: implement the storage close function
    // Closing and flush everything to the file
    //CloseStorage();
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

