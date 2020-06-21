#ifndef _IMAGEMINER_H_
#define _IMAGEMINER_H_

#include <afxwin.h>

#include "ThreadSafeQueue.h"
#include "ThreadSafeVariable.h"

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
	~ImageMiner();
	void Terminate();
public:
	CameraList camList;
	CameraPtr cam;
	RAWQueue* rawQueue;
	ThreadSafeVariable<bool> * imageMiningStopped;
	ThreadSafeVariable<bool> * loopRunning;
	CWinThread* handler;
};

UINT __cdecl spawnImageMiner(LPVOID);

#endif // !_IMAGEMINER_H_
