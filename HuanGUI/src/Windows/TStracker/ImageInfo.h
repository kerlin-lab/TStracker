#ifndef _IMAGE_INFO_H_

#define _IMAGE_INFO_H_

// Spinview lib
// General API
#include "Spinnaker.h"
#include "SpinGenApi\SpinnakerGenApi.h"

using namespace Spinnaker;	

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


#define DEFAULT_BACKGROUND cv::Scalar(49, 52, 49)

class ImageInfo
{
public:
	Mat img;		// The image saved by this object
	int imgWidth;	// Width of the image
	int imgHeight;	// Height of the image
	int imgSize;	// Size = Width * Height
	std::string camSerial;	// The serial of the camera from which the object img of this is obtained

	ImageInfo();

	ImageInfo(int imgWidth, int imgHeight, string camSerial = "");

	// Copy constructor
	ImageInfo(const ImageInfo &obj) ;
	
	~ImageInfo();

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
	void getFromImgPtr(ImagePtr& spin_con);

	/*
	* Changing the size of the Mat member of this class
	*/
	void changeImgSize(int width, int height);
};

#endif // !_IMAGE_INFO_H_
