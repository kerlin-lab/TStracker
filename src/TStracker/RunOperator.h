#ifndef _RUN_OPERATOR_H_
#define _RUN_OPERATOR_H_

#include "Spinnaker.h"
#include "SpinGenApi\SpinnakerGenApi.h"
#include "CamRecorder.h"
#include "CVDisplay.h"
#include "ThreadSafeVariable.h"
#include <vector>

using namespace Spinnaker;

typedef vector<CamRecorder*> CamRecorderPtrList;
typedef vector<CamRecorder*> * CamRecorderPtrListPtr;

// TODO 1: Resolve the loopy dependency issue of CVDisplay and RunOperator
class RunOperator
{
public:
	RunOperator();
	~RunOperator();

public:
	CamRecorderPtrListPtr camRecs;
	CameraList camList;
	CVDisplay *cvDisplay;
	HANDLE mtx;
	ThreadSafeVariable<bool> * running;				// Remember to release running
};


#endif // !_RUN_OPERATOR_H_
