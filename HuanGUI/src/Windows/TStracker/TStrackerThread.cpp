#include "TStrackerThread.h"

CamAcquireThreadInfo::CamAcquireThreadInfo(
	string cSerial,
	CameraPtr* camPTR,
	GUI::GUIFactory gui,
	boolean runGui,
	boolean acsignal,
	boolean runRecord,
	boolean cInitStatus,
	boolean threadStt)
{
	// initionalizing
	this->camSerial = cSerial;
	this->camPtr = camPTR;
	this->runGUI = runGui;
	this->acquireSignal = acsignal;
	this->runRecord = runRecord;
	this->cameraInitStatus = cInitStatus;
	this->threadStatus = threadStt;

	// Open Acquisition and the property dialog box for the clicked camera

	// Initialize the camera 
	(*camPTR)->Init();														//Init the camera first otherwise, DialogBox->Connect(cam) will result in runtime errors
																			// Set up property dialog for this camera
	gui.ConnectGUILib(*camPTR);												// This some how prevent the camera selection box from disapearing after choosing a camera
	this->propDialog = new GUI::PropertyGridDlg();							// this can be replaced with new GUI::PropertyGridDlg();
	(this->propDialog)->Connect(*camPTR);									// Connect the dialog to the camera
	(this->propDialog)->Open();												// show the dialog
																			// Set up a thread uses OpenCV to acquire and save image
	this->threadObjectPtr = AfxBeginThread(openCVCamCapture, this);
}

// @para: a pointer to a CamAcquireThreadInfo of this thread
UINT __cdecl openCVCamCapture(LPVOID para)
{
	// parameter is a pointer to a CamAcquireThreadInfo
	CamAcquireThreadInfo* threadInfo = (CamAcquireThreadInfo*)para;
	CameraPtr pCam = *(threadInfo->camPtr);
	
	// Run acquisition
	RunAcquisition(threadInfo);
	
	// Marking thread is terminated (using thread-safe operation)
	WaitForSingleObject(mtx, INFINITE);
	threadInfo->threadStatus = false;
	ReleaseMutex(mtx);
	return 0;
}

// This function acquires and saves 10 images from a device.
// @para runGUI: a boolean variable instrcuts the acquiring loop when to stop
int AcquireAndShowImages(CameraPtr pCam, INodeMap& nodeMap, INodeMap& nodeMapTLDevice, CamAcquireThreadInfo* threadInfo)
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


		while (threadInfo->runGUI)
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
			if (threadInfo->acquireSignal)
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

						//imshow("test", imgFrame);

						// Save frame to file if recording is running
						runRecordFeature(threadInfo->runRecord, imgFrame, imgWidth, imgHeight, frameRate, vOut, camSerial, ".avi");

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
			drawGUI(displayFrame, imgFrame, imgWidth, imgHeight, imgSize, frameRate, pCam, nodeMap, threadInfo);

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
int RunAcquisition(CamAcquireThreadInfo* threadInfo)
{
	int result;
	CameraPtr pCam = *threadInfo->camPtr;

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
		result = result | AcquireAndShowImages(pCam, nodeMap, nodeMapTLDevice, threadInfo);

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

// Draw GUI components
// This function is thread-safe as long as it is called between WaitForSingleObject and ReleaseMutex

void drawGUI(Mat& frame, Mat& imgFrame, int& imgWidth, int& imgHeight, int& imgSize, int& frameRate, CameraPtr& pCam, INodeMap& nodeMap, CamAcquireThreadInfo* threadInfo)
{

	static char* BUTTON_START = "Start Acquisition";
	static char* BUTTON_STOP = "Stop Acquisition";
	static char* BUTTON_START_RECORD = "Start Recording";
	static char* BUTTON_STOP_RECROD = "Stop Recording";
	static char* BUTTON_STOP_CAMERA = "Detach Camera";

	// Assigning alias
	string camSerial = threadInfo->camSerial;
	boolean& acquireSignal = threadInfo->acquireSignal;
	boolean& runGUI = threadInfo->runGUI;
	boolean& runRecord = threadInfo->runRecord;

	//// Let get to work down here

	// Alternating button label
	static unordered_map<string, pair<char*, char*>> ButtonManager;	// pair<Start/Stop button label,Start/Stop Record button label>
	if (!ButtonManager.count(camSerial))
	{
		// This is the first time the camera is used so 
		// initilize the labels
		ButtonManager[camSerial].first = BUTTON_START;
		ButtonManager[camSerial].second = BUTTON_START_RECORD;
	}

	char*& SS_Button_label = ButtonManager[camSerial].first;
	char*& SSRecord_Button_label = ButtonManager[camSerial].second;

	Mat canvas(Size(imgWidth, 100), CV_8UC1, Scalar(30, 30, 30));

	vconcat(canvas, imgFrame, frame);
	//frame = imgFrame.clone();

	// Draw the acquisition control button
	if (cvui::button(frame, 0, 3, SS_Button_label))
	{
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
		runGUI = false;
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
