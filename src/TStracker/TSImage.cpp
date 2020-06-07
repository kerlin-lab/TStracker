#include "TSImage.h"

#include "ImageInfo.h"

using namespace Spinnaker;

// Put this in the header and you will get LNK2005 when compiling
TSImage::TSImage():imgWidth(0), imgHeight(0), imgSize(0), camSerial(""), timestamp(0)
{
	// Fill the background with a nice color
	this->img = DEFAULT_BACKGROUND;
}

TSImage::TSImage(int imgWidth, int imgHeight, std::string camSerial, uint64_t timestamp, uint64_t frameID, uint64_t streamID)
{
	this->changeImgSize(imgWidth, imgHeight);
	this->camSerial = camSerial;
	this->timestamp = timestamp;
	this->frameID = frameID;
	this->streamID = streamID;
}

TSImage::~TSImage() {}

// Copy the image saved in ImagePtr spin_con to the Mat of this object
// This function is based on the ImagePtr2CVMat_CV_8UC1 function in TStracker.h
void TSImage::getFromImgPtr(ImagePtr& spin_con)
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

	// Save timestamp
	this->timestamp = spin_con->GetTimeStamp();
	this->frameID = spin_con->GetFrameID();
	this->streamID = spin_con->GetID();

	//MessageBox(NULL, to_string(spin_con->GetID()).c_str(), "streamID", MB_OK);
	//MessageBox(NULL, to_string(spin_con->GetFrameID()).c_str(), "FrameID", MB_OK);
}

/*
* Changing the size of the Mat member of this class
*/
void TSImage::changeImgSize(int imgWidth, int imgHeight)
{
	this->imgWidth = imgWidth;
	this->imgHeight = imgHeight;
	this->imgSize = imgWidth * imgHeight;
	this->img.release();
	this->img.create(imgHeight, imgWidth, CV_8UC1);
	// Fill the background with a nice color
	this->img = DEFAULT_BACKGROUND;
}

// Copy constructor
TSImage::TSImage(const TSImage &obj)
{
	this->img = obj.img.clone();
	this->imgWidth = obj.imgWidth;
	this->imgHeight = obj.imgHeight;
	this->imgSize = obj.imgSize;
	this->camSerial = obj.camSerial;
	this->timestamp = obj.timestamp;
	this->frameID = obj.frameID;
	this->streamID = obj.streamID;
}

// Assignment = operator
TSImage& TSImage::operator= (const TSImage& obj)
{
	this->img = obj.img.clone();
	this->imgWidth = obj.imgWidth;
	this->imgHeight = obj.imgHeight;
	this->imgSize = obj.imgSize;
	this->camSerial = obj.camSerial;
	this->timestamp = obj.timestamp;
	this->frameID = obj.frameID;
	this->streamID = obj.streamID;
	return *this;
}
