#pragma once
#ifndef CV_DISPLAY_H_
#define CV_DISPLAY_H_

#include "ThreadSafeQueue.h"
#include "TSImage.h"
#include <vector>

typedef ThreadSafeQueue<TSImage*> GUIQueue;

class CVDisplay
{
public:
	CVDisplay();
	void Attach(GUIQueue*);
private:
	std::vector<GUIQueue*> queueList;
};

#endif  // CV_DISPLAY_H