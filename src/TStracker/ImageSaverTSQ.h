#ifndef _IMAGE_SAVER_TSQ_H_
#define _IMAGE_SAVER_TSQ_H_



// For multi threadding
#include <afxwin.h>

#define _TS_VER_2_
// TStracker lib
#include "TiffWriter.h"

// Spinview lib
// General API
#include "Spinnaker.h"
#include "SpinGenApi\SpinnakerGenApi.h"

using namespace Spinnaker;

// standard lib
#include <queue>
#include <string>
#include <random>
#include <fstream>

using namespace std;

// OpenCv lib
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio/videoio_c.h>
#include <opencv2/highgui/highgui_c.h>

using namespace cv;


#include "TSImage.h"
#include "ThreadSafeQueue.h"
#include "utils.h"

#define DEFAULT_FILENAME_LENGTH 5

#define MAX_TIFF_STACK_SIZE 2000

typedef TSImage * ImageSaverTSQItemType;
typedef ThreadSafeQueue<ImageSaverTSQItemType> ImageSaverTSQContainerType;
typedef ImageSaverTSQContainerType* ImageSaverTSQContainerTypePtr;

//class SavingThreadControllerTSQ
//{
//public:
//	string fileName;                    // Name of the file to save the image down
//	ImageSaverTSQContainerTypePtr container;                       // Pointer to a container containing the images needed to be saved (can be queue, stack, vector, ect)
//	bool fileIsOpen;                    // Is there a file openning and ready to be written to
//	HANDLE mtx;                         // Pointer to Mutex object used for making the thread multi-threading safe
//
//										// Change the fileIsOpen = true to false to prevent automatic image saving when the thread is started
//	SavingThreadControllerTSQ(string fileName, ImageSaverTSQContainerTypePtr container, bool fileIsOpen = true);
//
//	// Taken the last item off the to-be-saved list
//	void removeFromToSave();
//};

class ImageSaverTSQ
{
public:
	CWinThread* threadObject;                                    // The object of the saving thread 

	string fileName;                    // Name of the file to save the image down
	ImageSaverTSQContainerTypePtr container;                       // Pointer to a container containing the images needed to be saved (can be queue, stack, vector, ect)
	bool fileIsOpen;                    // Is there a file openning and ready to be written to
	bool filling;						// Flag showing if the queue of this thread is being filled up
	HANDLE mtx;                         // Pointer to Mutex object used for making the thread multi-threading safe

public:
	ImageSaverTSQ();

	// Change the fileIsOpen = true to false to prevent automatic image saving when the thread is started
	ImageSaverTSQ(std::string fileName, bool fileIsOpen = false, bool filling = true);

	~ImageSaverTSQ();

	// Check if there is a saving thread ready to save item 
	bool isOpen();

	// Check if the saving thread is still running
	bool isThreadRunning();

	// Adding the item to to-be-saved list
	void addToSave(ImageSaverTSQItemType item);

	// Get mutex object
	HANDLE getThreadMutex();

	// Signal thread to terminate
	void signalTermination();

	// Taken the last item off the to-be-saved list
	void removeFromToSave();

	// Signal the the thread that the queue of this thread is no longer being filled
	void Detach();

	// Check if the queue has been detached
	bool isDetached();
};

// Processor of the the saving thread
// Parameter is a pointer pointing to an SavingThreadController object 
UINT __cdecl savingThreadProcessorTSQ(LPVOID para);

#endif