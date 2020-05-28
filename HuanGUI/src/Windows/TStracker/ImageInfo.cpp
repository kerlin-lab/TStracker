#include "ImageInfo.h"

// Put this in the header and you will get LNK2005 when compiling
ImageInfo::ImageInfo() :imgWidth(0), imgHeight(0), imgSize(0), camSerial("") 
{
	// Fill the background with a nice color
	this->img = DEFAULT_BACKGROUND;
}

ImageInfo::ImageInfo(int imgWidth, int imgHeight, std::string camSerial)
{
	this->changeImgSize(imgWidth,imgHeight);
	this->camSerial = camSerial;
}

ImageInfo::~ImageInfo() {}

// Copy the image saved in ImagePtr spin_con to the Mat of this object
// This function is based on the ImagePtr2CVMat_CV_8UC1 function in TStracker.h
void ImageInfo::getFromImgPtr(ImagePtr& spin_con)
{
	uchar* rawDataFromCam = (uchar*)spin_con->GetData();
	uchar* Dest = this->img.ptr<uchar>(0);
	unsigned size = this->imgSize;
	memcpy(Dest, rawDataFromCam, size);
	
	// Old style copy, assume to be slower than memcpy
	//while (size--)
	//{
	//	*(Dest++) = *(rawDataFromCam++);
	//}
}

/*
* Changing the size of the Mat member of this class
*/
void ImageInfo::changeImgSize(int imgWidth, int imgHeight)
{
	this->imgWidth = imgWidth;
	this->imgHeight = imgHeight;
	this->imgSize = imgWidth * imgHeight;
	this->img.release();
	this->img.create(imgHeight, imgWidth, CV_8UC1);
	// Fill the background with a nice color
	this->img = DEFAULT_BACKGROUND;
}
