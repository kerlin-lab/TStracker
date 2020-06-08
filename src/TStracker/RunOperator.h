#ifndef _RUN_OPERATOR_H_
#define _RUN_OPERATOR_H_

#include "Spinnaker.h"
#include "SpinGenApi\SpinnakerGenApi.h"
#include "CamRecorder.h"
#include "CVDisplay.h"
#include <vector>

using namespace Spinnaker;

class RunOperator
{
public:
	RunOperator();
	void Stop();
private:
	std::vector<CamRecorder*> camRecs;
	CameraList camList;
	CVDisplay *cvDisplay;
};


#endif // !_RUN_OPERATOR_H_
