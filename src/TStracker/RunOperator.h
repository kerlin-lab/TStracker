#ifndef _RUN_OPERATOR_H_
#define _RUN_OPERATOR_H_

#include "CamRecorder.h"
#include "CVDisplay.h"
#include "ThreadSafeVariable.h"
#include <vector>


#include "Spinnaker.h"
#include "SpinGenApi\SpinnakerGenApi.h"
using namespace Spinnaker;

typedef vector<CamRecorder*> CamRecorderPtrList;
typedef vector<CamRecorder*> * CamRecorderPtrListPtr;

// waitTime = 0 means don't implement trial scheme
class RunOperator
{
public:
	// waitTime = 0 means don't implement trial scheme
	RunOperator(string savePath,uint64_t waitTime);

	~RunOperator();

public:
	CamRecorderPtrListPtr camRecs;
	CVDisplay *cvDisplay;
	HANDLE mtx;
	ThreadSafeVariable<bool> * running;				// Remember to release running
	string savePath;
	uint64_t waitTime;
};


#endif // !_RUN_OPERATOR_H_
