#ifndef _IMAGE_SAVER_H_
#define _IMAGE_SAVER_H_

	

// For multi threadding
#include <afxwin.h>

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


#include "ImageInfo.h"
#include "utils.h"

#define MAX_TIFF_STACK_SIZE 2000

typedef ImageInfo* ImageSaverItemType;
typedef queue<ImageSaverItemType> ImageSaverContainerType;

template <typename T>
class SavingThreadController
{
    public:
    string fileName;                    // Name of the file to save the image down
    T* container;                       // Pointer to a container containing the images needed to be saved (can be queue, stack, vector, ect)
    bool fileIsOpen;                    // Is there a file openning and ready to be written to
    HANDLE mtx;                         // Pointer to Mutex object used for making the thread multi-threading safe
    
	// Change the fileIsOpen = true to false to prevent automatic image saving when the thread is started
    SavingThreadController(string fileName,T* container, bool fileIsOpen=true);

	// Taken the last item off the to-be-saved list
	void removeFromToSave();
};

class ImageSaver
{
    public:
    ImageSaverContainerType* saveQueue;                                // The queue contains list of image need to be saved to the output file
                                                                // Use pointer so that the saving thread can access this
    SavingThreadController<ImageSaverContainerType>* threadController;    // The controller of the saving thread
    CWinThread* threadObject;                                    // The object of the saving thread 

    ImageSaver();
    
	ImageSaver(std::string fileName);

    ~ImageSaver();

    // Check if there is a saving thread ready to save item 
    bool isOpen();

    // Check if the saving thread is still running
    bool isThreadRunning();

    // Adding the item to to-be-saved list
    void addToSave(ImageSaverItemType item);

    // Get mutex object
    HANDLE getThreadMutex();

    // Signal thread to terminate
    void signalTermination();
};


// Processor of the the saving thread
// Parameter is a pointer pointing to an SavingThreadController object 
UINT __cdecl savingThreadProcessor(LPVOID para);
#endif