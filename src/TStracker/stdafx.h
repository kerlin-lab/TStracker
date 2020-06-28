#include <afxwin.h>

#include "Spinnaker.h"
using namespace Spinnaker;

extern SystemPtr spinSystem;
extern HANDLE SpinSysMTX;

/*
- Some rule to live along with Spinnaker lib
	+ When you need to have a CameraList, use the spinSystem object above and call
	CameraList camList = spinSystem->GetCameras()
	+ Should not run camList.Clear()
	+ Do not call spinSystem->ReleaseInstance(), this is done automatically by the destructor of TStrackerWinMain class
	+ Only use the spinSystem variable above when you need to get a System instance
	+ After you get a CameraPtr from camList by something like camList.GetByIndex(index):
		- Remember to Init() the camera before you do anything with it
		- Do not run Deinit() (documentation suggests so but, if you are having multiple threads using 1 camera, when you call Deinit in one thread, the other thread will get error, so don't)
- Some rule to have a beter multi-thread system:
	+ Don't call Sleep() in a thread if you just want a small delay, call SwitchToThread() to yield to another thread and your thread will have a small delay
*/