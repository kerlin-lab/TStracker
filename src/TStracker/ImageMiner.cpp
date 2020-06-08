#include "ImageMiner.h"

ImageMiner::ImageMiner(int index, RAWQueue* destQueue)
{
	this->cam = camList.GetByIndex(index);
	this->rawQueue = destQueue;
}

void ImageMiner::Terminate()
{
	this->stop = true;
}