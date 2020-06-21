#include "RunOperator.h"

RunOperator::RunOperator()
{
	// Reserve memory
	this->camRecs = new CamRecorderPtrList();
	this->running = new ThreadSafeVariable<bool>(true);
	// Filling
	for (int i = 0; i < camList.GetSize(); i++) {
		CamRecorder * camRec = new CamRecorder(i);
		camRecs->push_back(camRec);
	}
	this->cvDisplay = new CVDisplay(this->camRecs,this->running);
}

RunOperator::~RunOperator()
{
	delete this->running;
	//delete this->camRecs;			// Delete by CVDisplay
}
