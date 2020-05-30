/*
* This class is a solution from
* https://schneide.blog/2015/11/16/multi-page-tiffs-with-cpp/
* 
* Tiff image created by this module 
* will have timestamp as a string of time in NANOsecond
* saved in the TIFF_DATETIME tag (index 306)
*/

#include "TiffWriter.h"

using namespace tf;

TiffWriter::TiffWriter(std::string filename, bool multiPage) : page(0), multiPage(multiPage)
{
	tiff = TIFFOpen(filename.c_str(), "w");
}

void TiffWriter::write(const unsigned char* buffer, int width, int height)
{
	if (multiPage) {
		/*
		* I seriously don't know if this is supposed to be supported by the format,
		* but it's the only we way can write the page number without knowing the
		* final number of pages in advance.
		*/
		TIFFSetField(tiff, TIFFTAG_PAGENUMBER, page, 0);
		TIFFSetField(tiff, TIFFTAG_SUBFILETYPE, FILETYPE_PAGE);
	}
	TIFFSetField(tiff, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
	TIFFSetField(tiff, TIFFTAG_IMAGEWIDTH, width);
	TIFFSetField(tiff, TIFFTAG_IMAGELENGTH, height);
	TIFFSetField(tiff, TIFFTAG_SAMPLEFORMAT, SAMPLEFORMAT_UINT);
	TIFFSetField(tiff, TIFFTAG_ROWSPERSTRIP, TIFFDefaultStripSize(tiff, (unsigned int)-1));
	TIFFSetField(tiff, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK);		// Gray scale image with black is zero

	unsigned int samples_per_pixel = 1;
	unsigned int bits_per_sample = 8;
	TIFFSetField(tiff, TIFFTAG_BITSPERSAMPLE, bits_per_sample);
	TIFFSetField(tiff, TIFFTAG_SAMPLESPERPIXEL, samples_per_pixel);

	std::size_t stride = width;
	for (unsigned int y = 0; y < height; ++y) {
		TIFFWriteScanline(tiff, (void*)(buffer + y * stride), y, 0);
	}

	TIFFWriteDirectory(tiff);
	page++;
}
// Write the mat to file
void TiffWriter::write(const cv::Mat& img, int width, int height)
{
	const uchar* Dest = img.ptr<uchar>(0);
	this->write(Dest, width, height);
}

// Write the cv::Mat to file with timestamp
void TiffWriter::write(const cv::Mat& img, int width, int height, uint64_t timestamp)
{
	//TIFFSetField(tiff, TIFFTAG_IMAGEDESCRIPTION, to_string(timestamp).c_str());
	
	// Save timestamp as a string of time in NANOsecond
	TIFFSetField(tiff, TIFFTAG_DATETIME, to_string(timestamp).c_str());
	// Save the image data	
	this->write(img, width, height);

	//MessageBox(NULL, to_string(timestamp).c_str(), "Time", MB_OK);
}


TiffWriter::~TiffWriter()
{
	TIFFClose(tiff);
}

// Getting a handle to a new and writable TIFF file
TiffWriter* TiffWriter::OpenNewTIFFtoWrite(std::string filename, bool isMultipage)
{
	return new TiffWriter(filename, isMultipage); ;
}
// Close the handle to the TIFF file and save the images
void TiffWriter::CloseTIFFFile(TiffWriter * tiff)
{
	// release memmory
	delete tiff;
}

// Save the image contained in img to file
void TiffWriter::SavetoTIFFFile(ImageInfo* img)
{
	this->write(img->img, img->imgWidth, img->imgHeight,img->timestamp);
}