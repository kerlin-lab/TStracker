#include "ImageInfo.h"

ImageInfo(int imgWidth, int imgHeight, string camSerial)
{
	this->changeImgSize(imgWidth,imgHeight);
	this->camSerial = camSerial;
}

~ImageInfo()
{
	this->img.release();
}

// Copy the image saved in ImagePtr spin_con to the Mat of this object
// This function is based on the ImagePtr2CVMat_CV_8UC1 function in TStracker.h
void ImageInfo::getFromImgPtr(ImagePtr& spin_con)
{
	uchar* rawData = (uchar*)spin_con->GetData();
	uchar* p = this->img.ptr<uchar>(0);
	int size = this->imgSize;
	while (size--)
	{
		*(p++) = *(rawData++);
	}
}

/*
* Changing the size of the Mat member of this class
*/
void changeImgSize(int imgWidth, int imgHeight)
{
	this->imgWidth = imgWidth;
	this->imgHeight = imgHeight;
	this->imgSize = imgWidth * imgHeight;
	this->img.release();
	this->img.create(imgHeight, imgWidth, CV_8UC1);
}
