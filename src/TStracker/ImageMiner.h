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
#include "TSImage.h"

typedef ThreadSafeQueue<TSImage*> RAWQueue;

class ImageMiner
{
public:
	ImageMiner(int, RAWQueue*, ThreadSafeVariable<bool>*, uint64_t waitTime);
	~ImageMiner();
	void Terminate();
public:
	RAWQueue* rawQueue;
	ThreadSafeVariable<bool> * imageMiningStopped;
	ThreadSafeVariable<bool> * loopRunning;
	CWinThread* handler;
	unsigned camIndex;
	string camSerial;
	uint64_t waitTime;		// in ms
};

UINT __cdecl spawnImageMiner(LPVOID);

#endif // !_IMAGEMINER_H_
