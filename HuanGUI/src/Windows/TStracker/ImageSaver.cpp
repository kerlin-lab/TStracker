#include "ImageSaver.h"

ImageSaver()
{
    this->saveQueue = new ContainerType();
    this->threadController = new SavingThreadController( random_string + DEFAULT_EXTENSION,this->saveQueue);
    // Create the thread
    this->threadObject = AfxBeginThread(savingThreadProcessor,this->threadController);
}

ImageSaver(string fileName)
{
    this->saveQueue = new ContainerType();
    this->threadController = new SavingThreadController( fileName,this->saveQueue);
    // Create the thread
    this->threadObject = AfxBeginThread(savingThreadProcessor,this->threadController);
}

// Copy constructor
ImageInfo(const ImageInfo &obj)
{
    this->img= obj.img.clone();
    this->imgWidth = obj.imgWidth;
    this->imgHeight = obj.imgHeight;
    this->imgSize = obj.imgSize;
}


~ImageSaver()
{
    delete saveQueue;
    delete threadController;
}

HANDLE ImageSaver::getThreadMutex()
{
    this->threadController->mtx;
}

SavingThreadController(string fileName,T* container, bool fileIsOpen=false);
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
    return GetExitCodeThread(this->m_hThread) == STILL_ACTIVE;
}

// Adding the item to to-be-saved list
void ImageSaver::addToSave(ItemType item)
{
    WaitForSingleObject(this->getThreadMutex(),INFINITE);
    this->saveQueue->push(item);
    ReleaseMutex(this->getThreadMutex(),INFINITE);
}


// Signal thread to terminate
void ImageSaver::signalTermination()
{
    WaitForSingleObject(this->getThreadMutex(),INFINITE);
    this->threadController->fileIsOpen = false;
    ReleaseMutex(this->getThreadMutex(),INFINITE);
}

// Processor of the the saving thread
// Parameter is a pointer pointing to an SavingThreadController object 
UINT __cdecl savingThreadProcessor(LPVOID para)
{
    SavingThreadController* threadController = (SavingThreadController*) para;
    
    // TODO N: Check the saving procedure below
    WaitForSingleObject(threadController->mtx,INFINITE);
    // TODO 1: Implement the OpenFIleToWirte function
    threadController->fileIsOpen = OpenStorage();
    ReleaseMutex(threadController->mtx);

    // Running the loop to save everything in the container down to file
    // TODO 4: implement IsExternalSignalDone()
    while(threadController->fileIsOpen || threadController->size() || IsExternalSignalDone())
    {
        // Thread-safe sake
        WaitForSingleObject(threadController->mtx,INFINITE);
        // TODO 2: Implement the saving function
        SavetoStore(threadController->container->back());
        // Remove image from container
        threadController->container->pop();
        ReleaseMutex(threadController->mtx);
    }


    // TODO 3: implement the storage close function
    // Closing and flush everything to the file
    CloseStorage();
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

