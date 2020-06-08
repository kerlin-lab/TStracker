#ifndef _IMAGEMINER_H_
#define _IMAGEMINER_H_

#include "ThreadSafeQueue.h"
// Spinview lib
// General API
#include "Spinnaker.h"
#include "SpinGenApi\SpinnakerGenApi.h"

using namespace Spinnaker;

typedef ThreadSafeQueue<ImagePtr> RAWQueue;

class ImageMiner
{
public:
	ImageMiner(int, RAWQueue*);
	void Terminate();
public:
	CameraList camList;
	CameraPtr cam;
	RAWQueue* rawQueue;
	bool stop = false;
};

#endif // !_IMAGEMINER_H_
