#ifndef _IMAGE_DISTRIBUTOR_H_
#define _IMAGE_DISTRIBUTOR_H_

#include "ThreadSafeQueue.h"
#include "TSImage.h"
#include "Spinnaker.h"
#include "SpinGenApi\SpinnakerGenApi.h"
#include <vector>

using namespace Spinnaker;

typedef ThreadSafeQueue<ImagePtr> RAWQueue;
typedef ThreadSafeQueue<TSImage*> GUIQueue;
typedef ThreadSafeQueue<TSImage*> SaveQueue;

class ImageDistributor
{
public:
	ImageDistributor(RAWQueue*, GUIQueue*, int);
	void Terminate();
	void Distribute();
public:
	RAWQueue* rawQueue;
	GUIQueue* guiQueue;
	bool stop = false;
	int maxSize;
	std::vector<SaveQueue*> saveQueue;
};


#endif //_IMAGE_DISTRIBUTOR_H_
