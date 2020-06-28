#ifndef _TIFF_WRITER_H
#define _TIFF_WRITER_H

/*
* This class is a solution from
* https://schneide.blog/2015/11/16/multi-page-tiffs-with-cpp/
*
* Tiff image created by this module
* will have timestamp as a string of time in NANOsecond
* saved in the TIFF_DATETIME tag (index 306)
*/

#include <string>
#include <regex>

#include <opencv2/core.hpp>


#include "TSImage.h"
typedef TSImage ImageType;

namespace tf
{
	#include <tiffio.h>
}

#define TIMESTAMP_TAG_INDEX_LOW 67000
#define TIMESTAMP_TAX_INDEX_HIGH 67001

extern std::string DEFAULT_EXTENSION;

class TiffWriter
{
public:
	TiffWriter(std::string filename, bool multiPage=true, bool addingExtension=true);
	TiffWriter(const TiffWriter&) = delete;
	TiffWriter& operator=(const TiffWriter&) = delete;
	~TiffWriter();

	void write(const unsigned char* buffer, int width, int height);
	
	// Write the cv::Mat to file
	void write(const cv::Mat& img, int width, int height);

	// Getting a handle to a new and writable TIFF file
	static TiffWriter* OpenNewTIFFtoWrite(std::string filename, bool isMultipage = true);

	// Close the handle to the TIFF file and save the images
	static void CloseTIFFFile(TiffWriter* tiff);

	// Save the image contained in img to file
	void SavetoTIFFFile(ImageType* img);

private:
	tf::TIFF* tiff;
	bool multiPage;
	unsigned int page;
};

string GenJSONImageDesp(uint64_t frameID, uint64_t streamID);

#endif // !_TIFF_WRITER_H
