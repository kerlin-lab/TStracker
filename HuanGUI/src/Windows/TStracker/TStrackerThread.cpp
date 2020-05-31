#include "TStrackerThread.h"

extern string ALL_CAM_RECORD_WINDOWS_NAME = "All cameras";


CamAcquireGUIThreadInfo::CamAcquireGUIThreadInfo(
	AFX_THREADPROC threadProc,
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

	if (camPTR != nullptr)
	{
		// if camPtr is not a null pointer, so this thread is to control a single camera

		// Open Acquisition and the property dialog box for the clicked camera

		// Initialize the camera 
		(*camPTR)->Init();														//Init the camera first otherwise, DialogBox->Connect(cam) will result in runtime errors
																				// Set up property dialog for this camera
		gui.ConnectGUILib(*camPTR);												// This some how prevent the camera selection box from disapearing after choosing a camera
		this->propDialog = new GUI::PropertyGridDlg();							// this can be replaced with new GUI::PropertyGridDlg();
		(this->propDialog)->Connect(*camPTR);									// Connect the dialog to the camera
		(this->propDialog)->Open();												// show the dialog
																				// Set up a thread uses OpenCV to acquire and save image
	}

	// Run the thread
	this->threadObjectPtr = AfxBeginThread(threadProc, this);
}

void CamAcquireGUIThreadInfo::CleanUpThreadInfo()
{
	WaitForSingleObject(mtx, INFINITE);
	this->threadStatus = false;
	this->runGUI = false;
	this->acquireSignal = false;
	this->cameraInitStatus = false;
	this->runRecord = false;
	ReleaseMutex(mtx);
}

// @para: a pointer to a CamAcquireGUIThreadInfo of this thread
UINT __cdecl openCVCamTuning(LPVOID para)
{
	// parameter is a pointer to a CamAcquireGUIThreadInfo
	CamAcquireGUIThreadInfo* threadInfo = (CamAcquireGUIThreadInfo*)para;
	CameraPtr pCam = *(threadInfo->camPtr);
	
	// Run acquisition
	RunAcquisition(threadInfo);
	
	// Marking thread terminated (using thread-safe operation)
	threadInfo->CleanUpThreadInfo();
	return 0;
}

// Run the all cameras record process
UINT __cdecl openCVAllCamRecord(LPVOID para)
{

	// parameter is a pointer to a CamAcquireGUIThreadInfo
	// Path to the folder is sent thru cSerial camSerial parameter
	CamAcquireGUIThreadInfo* threadInfo = (CamAcquireGUIThreadInfo*)para;
	//MessageBox(NULL, "Running", "OK", MB_OK);

	// Run the GUI

	RunRecordAll(threadInfo);

	// Marking thread terminated (using thread-safe operation)
	threadInfo->CleanUpThreadInfo();
	return 0;
}

// Execute the camrecord all process
void RunRecordAll(CamAcquireGUIThreadInfo* threadInfo)
{
	// TODO N: Check the Code down here
	CameraList camList;

	//Config all cameras for simultenous recording
	if (!configAllCams4SimultenousRecording(camList))
	{
		// If cannot do this
		MessageBox(NULL, "Attempt to config all cameras failed", "Error", MB_OK);
	}
	else
	{
		/// Run the GUI
		runGUIRecordAllCams(threadInfo,camList);
	}

	// De-init all cam
	deinitAllCam(camList);
}

void runGUIRecordAllCams(CamAcquireGUIThreadInfo* threadInfo, CameraList& camList)
{
	//// Some setting up before run GUI
	CameraPtr pCam;	// This is just middleman
	int result = 0;

	// Activate running record right away
	WaitForSingleObject(mtx, INFINITE);	// TODO N+1: Is this thread-safe wrapping needed here?
	// Setting up flags
	threadInfo->runRecord = true;		// Automatically run recording
	threadInfo->acquireSignal = true;
	ReleaseMutex(mtx);

	// Creating map of image captured from each camera
	int maxHeight, sumWidth;
	maxHeight = sumWidth = 0;
	
	vector<ImageSaver*> saverThreads;
	vector<ImageInfo> camCapImg;
	vector<int> frameRate;
	uint64_t timestamp, intialTimestamp = UINT64_MAX;
	

	for(unsigned i=0;i<camList.GetSize();i++)
	{
		// Get the camera object
		pCam = camList.GetByIndex(i);
		// Create an image object obtained from the camera
		camCapImg.emplace_back();
		// Reverse space to save framerate
		frameRate.push_back(0);
		// Retrive information about the image represneted by this object from the camera
		retriveImageInfo(pCam->GetNodeMap(),camCapImg[i].img,camCapImg[i].imgWidth,camCapImg[i].imgHeight,camCapImg[i].imgSize,frameRate[i],pCam);
		// Save the camera Serial to each object too
		camCapImg.back().camSerial = pCam->DeviceSerialNumber();
		//// Do some calculation for the GUI mat
		// Find max height
		if(maxHeight < camCapImg[i].imgHeight)
		{
			maxHeight = camCapImg[i].imgHeight;
		}
		// Find total width
		sumWidth += camCapImg[i].imgWidth;
		
		// TODO : may be you want to have a mechanism to name the output file
		// Creating saving thread for each camera
		// Path to the folder is sent thru camSerial parameter
		saverThreads.push_back(new ImageSaver(threadInfo->camSerial+string("\\")+ pCam->DeviceSerialNumber().c_str()));
	}

	//// Multiple screen with 1 cameras test
	//int N = 2;
	//for (unsigned i = 0; i<N; i++)
	//{
	//	// Get the camera object
	//	pCam = camList.GetByIndex(0);
	//	// Create an image object obtained from the camera
	//	camCapImg.emplace_back();
	//	// Retrive information about the image represneted by this object from the camera
	//	retriveImageInfo(pCam->GetNodeMap(), camCapImg[0].img, camCapImg[0].imgWidth, camCapImg[0].imgHeight, camCapImg[0].imgSize, frameRate, pCam);
	//	// Save the camera Serial to each object too
	//	camCapImg.back().camSerial = pCam->DeviceSerialNumber();
	//	//// Do some calculation for the GUI mat
	//	// Find max height
	//	if (maxHeight < camCapImg[0].imgHeight)
	//	{
	//		maxHeight = camCapImg[0].imgHeight;
	//	}
	//	// Find total width
	//	sumWidth += camCapImg[0].imgWidth;

	//	// TODO : may be you want to have a mechanism to name the output file
	//	// Creating saving thread for each camera
	//	saverThreads.emplace_back();
	//}

	//// Rendering the GUI

	// Create the Mat object to hold images and the GUI compoenents (buttons. windows)
	//ImageInfo GUIWindow(N * sumWidth + (N+1) * WINDOW_PADDING, maxHeight + GENERAL_BUTTON_HEIGHT + CAPTION_HEIGHT + 2 * WINDOW_PADDING);
	ImageInfo GUIWindow(sumWidth + (camList.GetSize()+1) * WINDOW_PADDING, maxHeight + GENERAL_BUTTON_HEIGHT + CAPTION_HEIGHT + 2 * WINDOW_PADDING);

	// start acquisition on all cameras
	runAcquisitionAllCams(camList);

	while (threadInfo->runGUI)
	{
		// TODO N: Check if the GUI rendering code below is correct

		// Check if the windows exists if not create one
		// This also prevent stalled windows when user close windows by the x button not the detach camera button
		// Solution proposed by https://medium.com/@mh_yip/opencv-detect-whether-a-window-is-closed-or-close-by-press-x-button-ee51616f7088
		if (getWindowProperty(ALL_CAM_RECORD_WINDOWS_NAME, cv::WND_PROP_VISIBLE) <= 0.5)
		{
			//Windows is closed or does not exist
			// Create a OpenCV window for displaying
			namedWindow(ALL_CAM_RECORD_WINDOWS_NAME);
			// Register with cvui
			cvui::init(ALL_CAM_RECORD_WINDOWS_NAME);
		}

		// Draw image if is capturing

		if (threadInfo->acquireSignal && threadInfo->runRecord)
		{
			try
			{
				// Get image from each camera
				for(unsigned i=0;i<camList.GetSize();i++)
				{
					// Get camera pointer
					pCam = camList.GetByIndex(i);

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

					if (pResultImage->IsIncomplete())
					{
						// Retrieve and print the image status description
						//cout << "Image incomplete: " << Image::GetImageStatusDescription(pResultImage->GetImageStatus())
						//	<< "..." << endl
						//	<< endl;
						MessageBox(NULL, (string("Image incomplete in acquisition of camera") + to_string(i)).c_str(), "Error", MB_OK);
						waitKey(500);
					}
					else
					{

						ImagePtr convertedImage = pResultImage->Convert(PixelFormatEnums::PixelFormat_Mono8, ColorProcessingAlgorithm::HQ_LINEAR);
						// Converting to OpenCV Mat
						// ImagePtr2CVMat_CV_8UC1(convertedImage, imgFrame, imgSize);
						camCapImg[i].getFromImgPtr(convertedImage);
						// Get timestamp
						timestamp = convertedImage->GetTimeStamp();
						if (intialTimestamp == UINT64_MAX)
						{
							intialTimestamp = timestamp;
						}
						timestamp -= intialTimestamp;
						// Draw timestamp
						drawTimeAndFPS(camCapImg[i].img, getReadableTimestamp(timestamp) / 1000000.0, frameRate[i]);

						// Save frame to file if recording is running

						// TODO N: check this implementation of image saving is correct
						saverThreads[i]->addToSave(new ImageInfo(camCapImg[i]));
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
			}
			catch (Spinnaker::Exception& e)
			{
				if (e.GetError() != Spinnaker::Error::SPINNAKER_ERR_TIMEOUT)
				{
					MessageBox(NULL, e.GetFullErrorMessage(), "Error", MB_OK);
					break;
				}
				result = -1;
			}
		}

		// --------------------- Drawing the GUI -----------------------------------
		WaitForSingleObject(mtx, INFINITE);

		cvui::context(ALL_CAM_RECORD_WINDOWS_NAME);

		// Draw the cvui gui ( Draw GUI after drawing the image to make the GUI on top
		drawGUIAllCam(GUIWindow.img, camCapImg,threadInfo, camList);

		// Draw the change to the window
		cvui::imshow(ALL_CAM_RECORD_WINDOWS_NAME, GUIWindow.img);

		// Update the window
		waitKey(1);
		ReleaseMutex(mtx);
	}

	// Destroy OpenCV window
	destroyWindow(ALL_CAM_RECORD_WINDOWS_NAME);			// This is important as if the OpenCV window does not get destroyed, the next time you call imshow with the same window name, OpenCV won't create new windows. It would be just silence

	// End Streaming
	for (unsigned i = 0; i < camList.GetSize(); i++)
	{
		pCam = camList.GetByIndex(i);
		if (pCam->IsStreaming())
		{
			pCam->EndAcquisition();
		}
	}
	// Signaling the saverThreads to terminate
	for(unsigned i = 0 ;i<saverThreads.size();i++)
	{
		saverThreads[i]->signalTermination();		// This function is thread-safe
		// Free the ImageSaver object
		// Note: freeing ImageSaver object won't affect the operationg of the saving thread
		// ie: the thread will continue to run until it saves all the images in its queue down to file
		// this behavior allows the user to continue running another recording session without waiting for the last session to be saved
		delete saverThreads[i];
	}


	// Test for termination of threads, uncomment this and the messagebox at the end of savingThreadProcessor to test
	//MessageBox(NULL, "GUI thread terminated", "Error", MB_OK);
}


void drawGUIAllCam(Mat& displayFrame, vector<ImageInfo>& camCapImg,CamAcquireGUIThreadInfo* threadInfo, CameraList camList)
{
	static bool textDrawn = false;			// This prevent text caption from being drawn many times

	//// Draw frame of two windows and one button
	cvui::beginColumn(displayFrame, WINDOW_PADDING, WINDOW_PADDING,-1,-1,WINDOW_PADDING);

	// Render the button
	if(cvui::button("Stop Recording"))
	{
		// Stop the recording
		threadInfo->runGUI = false;
	}

	// Render the an images captured from each camera.
	cvui::beginRow(-1,-1, WINDOW_PADDING);
	// Draw images
	for(unsigned i = 0; i < camCapImg.size();i++)
	{
		// Draw an image with a caption beneath
		cvui::beginColumn(-1, -1, WINDOW_PADDING);
		cvui::image(camCapImg[i].img);
		if (!textDrawn)
		{
			cvui::text(string("  Serial: ") + camCapImg[0].camSerial, 0.5, 0);
		}
		cvui::endColumn();
	}

	// Test N windows using 1 camera.  Remember to multiply the GUIWindow's sumWidth 
	// by N, comment the code snippet below "Draw images" above and change the camList.getSize() to N to run this test
	//unsigned N = 2;
	//for(unsigned i = 0; i < N;i++)
	//{
	//	// Draw an image with a caption beneath
	//	cvui::beginColumn(-1,-1, WINDOW_PADDING);
	//	cvui::image(camCapImg[0].img);
	//	if (!textDrawn)
	//	{
	//		cvui::text(string("   Serial: ") + camCapImg[0].camSerial, 0.5, 0);
	//	}
	//	cvui::endColumn();
	//}

	// Prevent the images' captions from being drawn next time
	textDrawn = true;

	cvui::endRow();	

	cvui::endColumn();
}


void initAllCam(CameraList& camList)
{
	CameraPtr camPtr;
	for (unsigned i = 0; i < camList.GetSize(); i++)
	{
		camPtr = camList.GetByIndex(i);
		camPtr->Init();
	}
}

void deinitAllCam(CameraList& camList)
{
	CameraPtr camPtr;
	for (unsigned i = 0; i < camList.GetSize(); i++)
	{
		camPtr = camList.GetByIndex(i);
		camPtr->DeInit();
	}
}

// Configure the external trigger to use when IEEE1588 is not available
bool ConfigureExternalTrigger()
{
	// TODO 5: implement configure the external trigger
	return true;
}
// Activate acquisition on all camera
// This function needs to be thread-safe because we want all cameras to start at the same time

bool runAcquisitionAllCams(CameraList& camList)
{
	WaitForSingleObject(mtx, NULL);
	for (unsigned i = 0; i < camList.GetSize(); i++)
	{
		try
		{
			camList.GetByIndex(i)->BeginAcquisition();
		}
		catch (Spinnaker::Exception e)
		{
			MessageBox(NULL, (string("Unable to start acquisition of all cameras due to ") + string(e.what())).c_str(), "Error", MB_OK);
			return false;
		}
	}
	ReleaseMutex(mtx);
	return true;
}


// Congifuring all available cameras
// Return true if succesfully initializing all cameras
bool configAllCams4SimultenousRecording(CameraList& camList)
{
	SystemPtr system = System::GetInstance();
	camList = system->GetCameras();

	// Finish if there are no cameras
	if (camList.GetSize() == 0)
	{
		// Clear camera list before releasing system
		camList.Clear();

		// Release system
		system->ReleaseInstance();

		MessageBox(NULL, "No camera detected", "Error", MB_OK);
		return false;
	}
	else
	{
		// Init all camera
		initAllCam(camList);

		//// TODO 6: Uncomment this
		//// Synchronize all camera timer by enabling IEE1588
		//if (ConfigureIEEE1588(camList))
		//{
		//	return true;
		//} // if IEEE1588 is not available, then try to use external trigger system
		//else if(ConfigureExternalTrigger())
		//{
		//	return true;
		//}
		//else // TODO N: Maybe considering using software trigger here
		//{
		//	return false;
		//}
	}
	return true;
}

// This function configures IEEE 1588 settings on each camera
// It enables IEEE 1588
bool ConfigureIEEE1588(const CameraList& camList)
{
	int result = 0;
	CameraPtr pCam = nullptr;

	try
	{
		// Enable IEEE 1588 settings for each camera
		for (unsigned int i = 0; i < camList.GetSize(); i++)
		{
			// Select camera
			pCam = camList.GetByIndex(i);

			// Enable IEEE 1588 settings
			CBooleanPtr ptrIEEE1588 = pCam->GetNodeMap().GetNode("GevIEEE1588");
			if (!IsAvailable(ptrIEEE1588))
			{
				MessageBox(NULL, (string("Camera ") + to_string(i) + string(" IEEE 1588 is not available. Aborting")).c_str(), "Error", MB_OK);
				return false;
			}
			else if(!IsWritable(ptrIEEE1588))
			{
				MessageBox(NULL, (string("Camera ") + to_string(i) + string(" IEEE1588 can not be configured")).c_str(), "Error", MB_OK);
				return false;
			}
			else
			{
				// If no error then
				// Enable IEEE 1588
				ptrIEEE1588->SetValue(true);
			}
		}

		// Needs at least 6 secconds for the IEEE1588 to be enabled fully throughout the network, so let wait
		MessageBox(NULL, "Waiting for IEEE1588 to be enabled", "Note", MB_OK);
		Sleep(10000);
		MessageBox(NULL, "Done wainting, move on to checking", "Note", MB_OK);
		// Check if IEEE 1588 settings is enabled for each camera

		for (unsigned int i = 0; i < camList.GetSize(); i++)
		{
			// Select camera
			pCam = camList.GetByIndex(i);
			CCommandPtr ptrGevIEEE1588DataSetLatch = pCam->GetNodeMap().GetNode("GevIEEE1588DataSetLatch");
			if (!IsAvailable(ptrGevIEEE1588DataSetLatch))
			{
				MessageBox(NULL, (string("Camera ") + to_string(i) + string(" Unable to execute IEEE 1588 data set latch (node retrieval)")).c_str(), "Error", MB_OK);
				return false;
			}
			ptrGevIEEE1588DataSetLatch->Execute();

			// Check if 1588 status is not in intialization
			CEnumerationPtr ptrGevIEEE1588StatusLatched = pCam->GetNodeMap().GetNode("GevIEEE1588StatusLatched");
			if (!IsAvailable(ptrGevIEEE1588StatusLatched) || !IsReadable(ptrGevIEEE1588StatusLatched))
			{
				MessageBox(NULL, (string("Camera ") + to_string(i) + string(" Unable to read IEEE1588 status (node retrieval). Aborting...")).c_str(), "Error", MB_OK);
				return false;
			}

			CEnumEntryPtr ptrGevIEEE1588StatusLatchedInitializing =
				ptrGevIEEE1588StatusLatched->GetEntryByName("Initializing");
			if (!IsAvailable(ptrGevIEEE1588StatusLatchedInitializing) ||
				!IsReadable(ptrGevIEEE1588StatusLatchedInitializing))
			{
				MessageBox(NULL, (string("Camera ") + to_string(i) + string(" Unable to get IEEE1588 status (enum entry retrieval). Aborting")).c_str(), "Error", MB_OK);
				return false;
			}

			if (ptrGevIEEE1588StatusLatched->GetIntValue() == ptrGevIEEE1588StatusLatchedInitializing->GetValue())
			{
				MessageBox(NULL, (string("Camera ") + to_string(i) + string(" is in Initializing mode. It can't send action command.")).c_str(), "Error", MB_OK);
				return false;
			}

			// Check if camera(s) is(are) synchronized to master camera
			// Verify if camera offset from master is larger than 1000ns which means camera(s) is(are) not synchronized
			CIntegerPtr ptrGevIEEE1588OffsetFromMasterLatched =
				pCam->GetNodeMap().GetNode("GevIEEE1588OffsetFromMasterLatched");
			if (!IsAvailable(ptrGevIEEE1588OffsetFromMasterLatched) ||
				!IsReadable(ptrGevIEEE1588OffsetFromMasterLatched))
			{
				MessageBox(NULL, (string("Camera ") + to_string(i) + string(" Unable to read IEEE1588 offset (node retrieval). Aborting")).c_str(), "Error", MB_OK);
				return false;
			}

			if (ptrGevIEEE1588OffsetFromMasterLatched->GetValue() > 1000)
			{
				MessageBox(NULL, (string("Camera ") + to_string(i) + string(" has offset higher than 1000ns. Camera(s) is(are) not synchronized")).c_str(), "Error", MB_OK);
				return false;
			}
		}
	}
	catch (Spinnaker::Exception& e)
	{
		MessageBox(NULL, e.what(), "Error note", MB_OK);
	}
	return false;
}


// @para runGUI: a boolean variable instrcuts the acquiring loop when to stop
int AcquireAndShowImages(CameraPtr pCam, INodeMap& nodeMap, INodeMap& nodeMapTLDevice, CamAcquireGUIThreadInfo* threadInfo)
{
	int result = 0;
	int imgWidth, imgHeight, imgSize, frameRate;
	uint64_t intialTimestamp = UINT64_MAX;

	Mat imgFrame;						// imageFrame is to hold the captured image
	Mat displayFrame;					// displayFrame is to hold the captured image and the GUI components to be displayed

	VideoWriter vOut;

	uint64_t timestamp;
	string vOutFileName;
	string camSerial;


	// Getting camera serial
	camSerial = pCam->DeviceSerialNumber();
	// Getting timestamp offset
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

						ImagePtr convertedImage = pResultImage->Convert(PixelFormatEnums::PixelFormat_Mono8, ColorProcessingAlgorithm::HQ_LINEAR);
						// Converting to OpenCV Mat
						ImagePtr2CVMat_CV_8UC1(convertedImage, imgFrame, imgSize);
						// Get timestamp
						timestamp = convertedImage->GetTimeStamp();
						if (intialTimestamp == UINT64_MAX)
						{
							intialTimestamp = timestamp;
						}
						timestamp -= intialTimestamp;
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
					if (e.GetError() != Spinnaker::Error::SPINNAKER_ERR_TIMEOUT)
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
		if (e.GetError() != Spinnaker::Error::SPINNAKER_ERR_TIMEOUT)
		{
			MessageBox(NULL, e.GetFullErrorMessage(), "Error", MB_OK);
		}
		return -1;
	}


	return result;
}




// This function acts as the body of the example; please see NodeMapInfo example
// for more in-depth comments on setting up cameras.
int RunAcquisition(CamAcquireGUIThreadInfo* threadInfo)
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
		if (e.GetError() != Spinnaker::Error::SPINNAKER_ERR_TIMEOUT)
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
void drawGUI(Mat& frame, Mat& imgFrame, int& imgWidth, int& imgHeight, int& imgSize, int& frameRate, CameraPtr& pCam, INodeMap& nodeMap, CamAcquireGUIThreadInfo* threadInfo)
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

	Mat canvas(Size(imgWidth, GENERAL_BUTTON_HEIGHT * 3), CV_8UC1, Scalar(30, 30, 30));

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
