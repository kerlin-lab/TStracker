#include "RunOperator.h"

RunOperator::RunOperator()
{
	cvDisplay = new CVDisplay();
	for (int i = 0; i < camList.GetSize(); i++) {
		CamRecorder *camRec = new CamRecorder(i);
		cvDisplay->Attach(camRec->guiQueue);
		camRecs.push_back(camRec);
	}
}

void RunOperator::Stop()
{
	for (CamRecorder* cam : camRecs) {
		cam->Detach();
	}
}