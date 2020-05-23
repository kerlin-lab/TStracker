#ifndef _IMAGE_INFO_H_

#define _IMAGE_INFO_H_

// OpenCv lib
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio/videoio_c.h>
#include <opencv2/highgui/highgui_c.h>

using namespace cv;


// Spinview lib
// General API
#include "Spinnaker.h"
#include "SpinGenApi\SpinnakerGenApi.h"

using namespace Spinnaker;

class ImageInfo
{
public:
	Mat img;		// The image saved by this object
	int imgWidth;	// Width of the image
	int imgHeight;	// Height of the image
	int imgSize;	// Size = Width * Height

	ImageInfo(int imgWidth, int imgHeight);

	// TODO 1: Implement this function
	void getFromImgPtr(ImagePtr& spin_con);
};

#endif // !_IMAGE_INFO_H_
