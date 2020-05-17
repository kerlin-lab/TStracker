#include "TStracker.h"

#define CVUI_IMPLEMENTATION
#include "cvui.h"

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


// Draw GUI components
void drawGUI(Mat& frame, Mat& imgFrame,int& imgWidth, int& imgHeight, int& imgSize, int& frameRate, bool& acquireSignal, bool& runSignal, bool& runRecord, CameraPtr& pCam, INodeMap& nodeMap)
{
	static char* BUTTON_START = "Start Acquisition";
	static char* BUTTON_STOP = "Stop Acquisition";
	static char* BUTTON_START_RECORD = "Start Recording";
	static char* BUTTON_STOP_RECROD = "Stop Recording";
	static char* BUTTON_STOP_CAMERA = "Detach Camera";
	// Alternating button label
	static unordered_map<string, pair<bool, bool>> ButtonManager;
	static char* SS_Button_label = BUTTON_START;
	static char* SSRecord_Button_label = BUTTON_START_RECORD;


	Mat canvas(Size(imgWidth,100), CV_8UC1, Scalar(30,30,30));

	vconcat (canvas, imgFrame,frame);
	//frame = imgFrame.clone();

	// Draw the acquisition control button
	if (cvui::button(frame, 0, 3, SS_Button_label))
	{
		//*runSignal = !runSignal;
		acquireSignal = !acquireSignal;

		// Temporarily pause the Acquisition 
		if (!acquireSignal)
		{
			pCam->EndAcquisition();
			SS_Button_label = BUTTON_START;
		}
		else
		{
			retriveImageInfo(nodeMap, imgFrame, imgWidth, imgHeight, imgSize, frameRate, pCam);
			pCam->BeginAcquisition();
			SS_Button_label = BUTTON_STOP;
		}
	}

	// Draw the camera detachment control button
	if (cvui::button(frame, 0, 40, BUTTON_STOP_CAMERA))
	{
		runSignal = false;
		SS_Button_label = BUTTON_START;
	}

	// Draw recording control button
	if (cvui::button(frame, 0, 80, SSRecord_Button_label))
	{
		runRecord = !runRecord;
		if (runRecord)
		{
			SSRecord_Button_label = BUTTON_STOP_RECROD;
		}
		else
		{
			SSRecord_Button_label = BUTTON_START_RECORD;
		}
	}
}

// Processing recording feature
void runRecordFeature(bool runRecord, Mat imgFrame, int imgWidth, int imgHeight, int frameRate, VideoWriter& vOut, string file_prefix, string videoType)
{
	static int count = 0;
	if (runRecord)
	{
		if (!vOut.isOpened())
		{
			vOut.open(file_prefix + "_" + to_string(count++) + videoType, CODEC, frameRate, Size(imgWidth, imgHeight), false);
			MessageBox(NULL, (file_prefix + "_" + to_string(count++) + videoType).c_str(), "Warning", MB_OK);
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
// This function acquires and saves 10 images from a device.
// @para runSignal: a boolean variable instrcuts the acquiring loop when to stop
int AcquireAndShowImages(CameraPtr pCam, INodeMap& nodeMap, INodeMap& nodeMapTLDevice,boolean* runSignal)
{
	int result = 0;
	int imgWidth, imgHeight, imgSize, frameRate;
	
	Mat imgFrame;						// imageFrame is to hold the captured image
	Mat displayFrame;					// displayFrame is to hold the captured image and the GUI components to be displayed

	VideoWriter vOut;

	uint64_t timestamp;
	string vOutFileName;
	string camSerial;


	// Getting camera serial
	camSerial = pCam->DeviceSerialNumber();
	try
	{

		//// Retrieve enumeration node from nodemap
		//CEnumerationPtr ptrAcquisitionMode = nodeMap.GetNode("AcquisitionMode");
		//if (!IsAvailable(ptrAcquisitionMode) || !IsWritable(ptrAcquisitionMode))
		//{
		//	cout << "Unable to set acquisition mode to continuous (enum retrieval). Aborting..." << endl << endl;
		//	return -1;
		//}

		//// Retrieve entry node from enumeration node
		//CEnumEntryPtr ptrAcquisitionModeContinuous = ptrAcquisitionMode->GetEntryByName("Continuous");
		//if (!IsAvailable(ptrAcquisitionModeContinuous) || !IsReadable(ptrAcquisitionModeContinuous))
		//{
		//	cout << "Unable to set acquisition mode to continuous (entry retrieval). Aborting..." << endl << endl;
		//	return -1;
		//}

		//// Retrieve integer value from entry node
		//const int64_t acquisitionModeContinuous = ptrAcquisitionModeContinuous->GetValue();

		//// Set integer value from entry node as new value of enumeration node
		//ptrAcquisitionMode->SetIntValue(acquisitionModeContinuous);

		//cout << "Acquisition mode set to continuous..." << endl;

		//// Turn off trigger, otherwise ,
		//// "Failed waiting for EventData on NEW_BUFFER_DATA event" error will happen
		////cout << "Turn off trigger mode" << endl;
		////pCam->TriggerMode.SetValue(Spinnaker::TriggerModeEnums::TriggerMode_Off);

		// Get the current frame rate; acquisition frame rate recorded in hertz
		//CFloatPtr ptrAcquisitionFrameRate = nodeMap.GetNode("AcquisitionFrameRate");

		//if (!IsAvailable(ptrAcquisitionFrameRate) || !IsReadable(ptrAcquisitionFrameRate))
		//{
		//	cout << "Unable to retrieve frame rate. Aborting..." << endl << endl;
		//	return -1;
		//}

		//int frameRate = (int)ptrAcquisitionFrameRate->GetValue();
		//// Get image width and height in the setting
		//CIntegerPtr width = nodeMap.GetNode("Width");
		//CIntegerPtr height = nodeMap.GetNode("Height");

		//imgWidth = (int)width->GetValue();
		//imgHeight = (int)height->GetValue();
		//imgSize = imgWidth*imgHeight;

		//// Reserve memmory that OpenCv will use to hold the image
		//createMono8Mat(imgFrame, imgWidth, imgHeight);
		retriveImageInfo(nodeMap, imgFrame, imgWidth, imgHeight, imgSize, frameRate, pCam);

		
		//
		// Begin acquiring images
		//
		// *** NOTES ***
		// What happens when the camera begins acquiring images depends on the
		// acquisition mode. Single frame captures only a single image, multi
		// frame captures a set number of images, and continuous captures a
		// continuous stream of images. Because the example calls for the
		// retrieval of 10 images, continuous mode has been set.
		//
		// *** LATER ***
		// Image acquisition must be ended when no more images are needed.
		//

		//pCam->BeginAcquisition();

		

		// Variables need for the acquisition loop
		bool acquireSignal = false;
		bool runSignal = true;
		bool runRecord = false;

		while (runSignal)
		{
			// Check if the windows exists if not create one
			// This also prevent stalled windows when user close windows by the x button not the detach camera button
			// Solution proposed by https://medium.com/@mh_yip/opencv-detect-whether-a-window-is-closed-or-close-by-press-x-button-ee51616f7088
			if (getWindowProperty(camSerial, cv::WND_PROP_VISIBLE) <= 0.5)
			{
				//Windows is closed or does not exist
				// Create a OpenCV window for displaying
				namedWindow(camSerial);
				// Register with cvui
				cvui::init(camSerial);
			}

			// Draw image if is capturing
			//if (*runSignal)
			if(acquireSignal)
			{
				try
				{
					//
					// Retrieve next received image
					//
					// *** NOTES ***
					// Capturing an image houses images on the camera buffer. Trying
					// to capture an image that does not exist will hang the camera.
					//
					// *** LATER ***
					// Once an image from the buffer is saved and/or no longer
					// needed, the image must be released in order to keep the
					// buffer from filling up.
					//
					ImagePtr pResultImage = pCam->GetNextImage(3000);

					//
					// Ensure image completion
					//
					// *** NOTES ***
					// Images can easily be checked for completion. This should be
					// done whenever a complete image is expected or required.
					// Further, check image status for a little more insight into
					// why an image is incomplete.
					//
					if (pResultImage->IsIncomplete())
					{
						// Retrieve and print the image status description
						//cout << "Image incomplete: " << Image::GetImageStatusDescription(pResultImage->GetImageStatus())
						//	<< "..." << endl
						//	<< endl;
						MessageBox(NULL, "Image incomplete", "Error", MB_OK);
						waitKey(500);
					}
					else
					{

						ImagePtr convertedImage = pResultImage->Convert(PixelFormat_Mono8, HQ_LINEAR);
						// Converting to OpenCV Mat
						ImagePtr2CVMat_CV_8UC1(convertedImage, imgFrame, imgSize);
						// Get timestamp
						timestamp = convertedImage->GetTimeStamp();
						// Draw timestamp
						drawTimeAndFPS(imgFrame, getReadableTimestamp(timestamp) / 1000000.0, frameRate);
						// Show the frame

						//imshow(camSerial, frame);
						
						// Save frame to file if recording is running
						runRecordFeature(runRecord, imgFrame, imgWidth, imgHeight, frameRate, vOut, camSerial, ".mkv");

						//if (waitKey(1) >= 0)
						//{
						//	break;
						//}
					}

					//
					// Release image
					//
					// *** NOTES ***
					// Images retrieved directly from the camera (i.e. non-converted
					// images) need to be released in order to keep from filling the
					// buffer.
					//
					pResultImage->Release();
				}
				catch (Spinnaker::Exception& e)
				{
					if (e.GetError() != SPINNAKER_ERR_TIMEOUT)
					{
						MessageBox(NULL, e.GetFullErrorMessage(), "Error", MB_OK);
						break;
					}
					result = -1;
				}

			}

			// --------------------- Drawing the GUI -----------------------------------
			WaitForSingleObject(mtx, INFINITE);

			cvui::context(camSerial);
			// Draw the cvui gui ( Draw GUI after drawing the image to make the GUI on to
			drawGUI(displayFrame, imgFrame, imgWidth, imgHeight, imgSize, frameRate, acquireSignal, runSignal, runRecord, pCam, nodeMap);

			// Draw the change to the window
			cvui::imshow(camSerial, displayFrame);
			//cvui::imshow(camSerial, imgFrame);

			// Update the window
			waitKey(1);
			ReleaseMutex(mtx);
		}

		// Destroy OpenCV window
		destroyWindow(camSerial);			// This is important as if the OpenCV window does not get destroyed, the next time you call imshow with the same window name, OpenCV won't create new windows. It would be just silence
		// Release matrix to save memmory
		imgFrame.release();
		//
		// End acquisition
		//
		// *** NOTES ***
		// Ending acquisition appropriately helps ensure that devices clean up
		// properly and do not need to be power-cycled to maintain integrity.
		//
		if (pCam->IsStreaming())
		{
			pCam->EndAcquisition();
		}
	}
	catch (Spinnaker::Exception& e)
	{
		if (e.GetError() != SPINNAKER_ERR_TIMEOUT)
		{
			MessageBox(NULL, e.GetFullErrorMessage(), "Error", MB_OK);
		}
		return -1;
	}


	return result;
}




// This function acts as the body of the example; please see NodeMapInfo example
// for more in-depth comments on setting up cameras.
int RunAcquisition(CameraPtr pCam, boolean* runAcquireSignal, boolean* camStatus)
{
	int result;

	try
	{
		// Initialize camera
		if (!pCam->IsInitialized())
		{
			pCam->Init();
		}


		// Retrieve TL device nodemap and print device information
		INodeMap& nodeMapTLDevice = pCam->GetTLDeviceNodeMap();


		// Retrieve GenICam nodemap
		INodeMap& nodeMap = pCam->GetNodeMap();

		// Acquire images, keep attempt to acquire until user turn off this camera
		//while(*camStatus)
		//{
			if (*runAcquireSignal)
			{
				result = result | AcquireAndShowImages(pCam, nodeMap, nodeMapTLDevice, runAcquireSignal);
			}
		//}

		// Deinitialize camera
		pCam->DeInit();					// DeInit() to make user open another Dialog next time he uses this camera
	}
	catch (Spinnaker::Exception& e)
	{
		if (e.GetError() != SPINNAKER_ERR_TIMEOUT)
		{
			MessageBox(NULL, e.GetFullErrorMessage(), "Error", MB_OK);
			waitKey(1000);
		}
		result = -1;
	}

	return result;
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