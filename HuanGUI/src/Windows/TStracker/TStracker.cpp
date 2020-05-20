#include "TStracker.h"

HANDLE mtx;


// Return time from running acquisition in sub milisecond
uint64_t getReadableTimestamp(uint64_t timestamp)
{
	static uint64_t ts = 0;
	if (ts == 0)
	{
		ts = timestamp;
		return 0;
	}
	else
	{
		return (timestamp - ts) / 1000;
	}
}


void drawTime(Mat& frame, double time)
{
	putText(frame, to_string(time) + "s", Point(TEXT_OFFSET, frame.rows - TEXT_OFFSET), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 255, 255));
}


void drawTimeAndFPS(Mat& frame, double time, int fps)
{
	putText(frame, to_string(time) + "s at " + to_string(fps) + " fps", Point(TEXT_OFFSET, frame.rows - TEXT_OFFSET), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 255, 255));
}


void dbhere(int a)
{
	cout << "Here " << a << endl;
}

/*
*  Return a CONTINUOUS mono 8 Mat
*  with each pixel of type uchar and
*  each size width*height
*/
void createMono8Mat(Mat& frame, int width, int height)
{
	frame.create(height, width, CV_8UC1);
}

/*
* The function convert a mono 8-bit gray scale image
* from Spinview ImagePtr to OpenCV Mat
* for the sake of performance
*
* NO type checking
* NO size checking
* the Input cv_con must be a continuous Mat (use the function createMono8Mat to get one)
* the size is width * height
*
* User need to do the task before calling the function
* ideally, the check should be done before acquisition takes place
*/
void ImagePtr2CVMat_CV_8UC1(ImagePtr& spin_con, Mat& cv_con, int size)
{
	uchar* rawData = (uchar*)spin_con->GetData();
	uchar* p = cv_con.ptr<uchar>(0);
	while (size--)
	{
		*(p++) = *(rawData++);
	}
}


// Get infor
void retriveImageInfo(INodeMap& nodeMap, Mat& imgFrame, int& imgWidth, int& imgHeight, int& imgSize, int& frameRate, CameraPtr& pCam)
{
	// Get the current frame rate; acquisition frame rate recorded in hertz
	CFloatPtr ptrAcquisitionFrameRate = nodeMap.GetNode("AcquisitionFrameRate");

	if (!IsAvailable(ptrAcquisitionFrameRate) || !IsReadable(ptrAcquisitionFrameRate))
	{
		MessageBox(NULL, "Unable to retrieve frame rate. Aborting...", "Error", MB_OK);
		return;
	}

	frameRate = (int)ptrAcquisitionFrameRate->GetValue();
	// Get image width and height in the setting
	CIntegerPtr width = nodeMap.GetNode("Width");
	CIntegerPtr height = nodeMap.GetNode("Height");

	imgWidth = (int)width->GetValue();
	imgHeight = (int)height->GetValue();
	imgSize = imgWidth*imgHeight;

	// Reserve memmory that OpenCv will use to hold the image
	createMono8Mat(imgFrame, imgWidth, imgHeight);

	// Warning to turn off trigger, otherwise , will see blank window
	// "Failed waiting for EventData on NEW_BUFFER_DATA event" error will happen
	if (pCam->TriggerMode.GetValue() == Spinnaker::TriggerModeEnums::TriggerMode_On)
	{
		MessageBox(NULL, "Trigger mode is ON,\nif you don't have hardware trigger set up,\nconsidering setting the trigger to OFF,\notherwise, the camera will not send image data\n and you will see a blank window when acquisition starts", "Warning", MB_ICONWARNING);
	}

	// Warning to put Acquisition in Continuous mode
	if (pCam->AcquisitionMode.GetValue() != Spinnaker::AcquisitionModeEnums::AcquisitionMode_Continuous)
	{
		MessageBox(NULL, "Acquisition mode is NOT in continuous mode \nConsidering setting the Acquisition Mode to Continuous\nOtherwise, the camera will show only one image if it is in single mode when acquisition starts.\nRemember to stop the acquisition before attempting to change acquisition mode!", "Warning", MB_ICONWARNING);
	}
}

// Processing recording feature
void runRecordFeature(bool runRecord, Mat imgFrame, int imgWidth, int imgHeight, int frameRate, VideoWriter& vOut, string file_prefix, string videoType)
{
	static int count = 0;
	count++;
	if (runRecord)
	{
		if (!vOut.isOpened())
		{
			vOut.open(file_prefix + "_" + to_string(count) + videoType, CODEC, frameRate, Size(imgWidth, imgHeight), false);
			MessageBox(NULL, (file_prefix + "_" + to_string(count) + videoType).c_str(), "Warning", MB_OK);
		}
		vOut.write(imgFrame);
	}
	else
	{
		if (vOut.isOpened())
		{
			MessageBox(NULL, "Stop recording", "Warning", MB_OK);
			vOut.release();
		}
	}
}





// Example entry point; please see Enumeration example for more in-depth
// comments on preparing and cleaning up the system.
//int oldmain(int /*argc*/, char** /*argv*/)
//{
//	int result = 0;
//	// Retrieve singleton reference to system object
//	SystemPtr system = System::GetInstance();
//
//	// Retrieve list of cameras from the system
//	CameraList camList = system->GetCameras();
//
//	const unsigned int numCameras = camList.GetSize();
//
//	cout << "Number of cameras detected: " << numCameras << endl << endl;
//
//	// Finish if there are no cameras
//	if (numCameras == 0)
//	{
//		// Clear camera list before releasing system
//		camList.Clear();
//
//		// Release system
//		system->ReleaseInstance();
//
//		cout << "Not enough cameras!" << endl;
//		cout << "Done! Press Enter to exit..." << endl;
//		getchar();
//
//		return -1;
//	}
//	else
//	{
//		// Camera found, so run
//
//		CameraPtr pCam = nullptr;
//
//
//		// use the first camera found
//		pCam = camList.GetByIndex(0);
//
//		// Run acquisition
//		RunAcquisition(pCam);
//
//		// Clean UP
//		pCam = nullptr;
//
//		// Clear camera list before releasing system
//		camList.Clear();
//
//		// Release system
//		system->ReleaseInstance();
//
//		cout << endl << "Done! Press Enter to exit..." << endl;
//		getchar();
//
//	}
//
//	return result;
//}

/*


// Open Acquisition and the property dialog box for the clicked
(*pCamera)->Init();										//Init the camera first otherwise, DialogBox->Connect(cam) will result in runtime errors
// Set up property dialog for this camera
TStrackerMain::gui.ConnectGUILib(*pCamera);				// This some how prevent the camera selection box from disapearing after choosing a camera
PropertyGridDlg* dlg = TStrackerMain::gui.GetPropertyGridDlg();

//PropertyGridDlg* dlg = new GUI::PropertyGridDlg();
dlg->Connect(*pCamera);									// Connect the dialog to the camera
dlg->Open();											// show the dialog

// Set up a thread uses OpenCV to acquire and save image
AfxBeginThread(openCVCamCapture, pCamera);*/