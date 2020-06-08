#include "CVDisplay.h"

CVDisplay::CVDisplay()
{

}

void CVDisplay::Attach(GUIQueue* queue)
{
	queueList.push_back(queue);
}