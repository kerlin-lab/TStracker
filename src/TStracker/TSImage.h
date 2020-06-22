#ifndef _TS_IMAGE_H_
#define _TS_IMAGE_H_

// Spinview lib
// General API
#include "Spinnaker.h"
#include "SpinGenApi\SpinnakerGenApi.h"

// OpenCv lib
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio/videoio_c.h>
#include <opencv2/highgui/highgui_c.h>

using namespace cv;

// standard lib
#include <string>

using namespace std;

class TSImage
{
public:
	Mat img;		// The image saved by this object
	int imgWidth;	// Width of the image
	int imgHeight;	// Height of the image
	int imgSize;	// Size = Width * Height
	uint64_t timestamp;	// Timestamp of the image
	uint64_t frameID;	// Frame ID of the image, this is used to detect missed frame
	uint64_t streamID;	// ID of the image in the stream, not sure if can be used to detect missed frame or missing image during transmission
	int framerate;		// The framerate of the camera captured this image
	std::string camSerial;	// The serial of the camera from which the object img of this is obtained
	unsigned trialNumber;	// Which trial does this image belong to

	TSImage();

	TSImage::TSImage(int imgWidth, int imgHeight, std::string camSerial = "", uint64_t timestamp = 0, uint64_t frameID = 0, uint64_t streamID = 0,int framerate = 0);

	// Copy constructor
	TSImage(const TSImage &obj);

	// Assignment = operator
	TSImage& operator= (const TSImage&);

	~TSImage();

	/*
	* The function convert a mono 8-bit gray scale image
	* from Spinview ImagePtr to OpenCV Mat img in this class
	* for the sake of performance
	*
	* NO type checking
	* NO size checking
	* the Input cv_con must be a continuous Mat (use the function createMono8Mat to get one)
	* the size is width * height
	*
	* User need to do the task before calling the function
	* ideally, the check should be done before acquisition takes place
	* Copy the image saved in ImagePtr spin_con to the Mat of this object
	*/
	void getFromImgPtr(Spinnaker::ImagePtr& spin_con,string camSerial="",int frameRate=-1);

	/*
	* Changing the size of the Mat member of this class
	*/
	void changeImgSize(int width, int height);
};

typedef TSImage * TSImagePtr;

#endif // !_TS_IMAGE_H_
