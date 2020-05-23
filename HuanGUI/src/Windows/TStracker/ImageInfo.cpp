#include "ImageInfo.h"

ImageInfo::ImageInfo(int imgWidth, int imgHeight)
{
	this->imgWidth = imgWidth;
	this->imgHeight = imgHeight;
	this->img.create(imgHeight, imgWidth, CV_8UC1);
}
