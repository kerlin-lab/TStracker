#include "CVDisplay.h"
#include "stdafx.h"

#define GENERAL_BUTTON_HEIGHT 33
#define CAPTION_HEIGHT 30
#define WINDOW_PADDING 10

string CV_DISPLAY_ALL_CAM_RECORD_WINDOWS_NAME = "Recording all cameras";


CVDisplay::CVDisplay(CamRecorderPtrListPtr camRecorderList, ThreadSafeVariable<bool> * running)
{
	this->camRecorderList = camRecorderList;
	this->guiIsRunning = running;
	this->guiIsRunning->write(true);
	this->stopSignalSent = false;
	this->launchGUI();
}

CVDisplay::~CVDisplay()
{
	CamRecorder * camRecorder;
	// Delete individual CamRecorder in the Camrecorder List
	while (this->camRecorderList->size())
	{
		camRecorder = this->camRecorderList->back();
		this->camRecorderList->pop_back();
		// Wait until the imageDistributor of this camRecorder stopped then delete this cam recorder
		while (!camRecorder->distributionStopped->read());
		delete camRecorder;
	}
	// Delete the CamreCorder vector
	delete this->camRecorderList;
	// Tell the RunOperator that it is fine to start another run
	this->guiIsRunning->write(false);
}


void CVDisplay::launchGUI()
{
	this->guiThreadHandle = AfxBeginThread(cvGUIRunProc, this);
}

GUIQueuePtr CVDisplay::getGUIQueue(unsigned i)
{	
	return this->camRecorderList->at(i)->guiQueue;
}

GUIQueuePtr CVDisplay::at(unsigned i)
{
	return this->getGUIQueue(i);
}

unsigned CVDisplay::getNumberGUIQueue()
{
	return this->camRecorderList->size();
}

unsigned CVDisplay::size()
{
	return this->getNumberGUIQueue();
}

void CVDisplay::stopAcquiring()
{
	for (CamRecorder* cam : *this->camRecorderList) {
		cam->Detach();
	}
}

bool CVDisplay::isAllDistributorStopped()
{
	for (CamRecorder * camRec : *(this->camRecorderList))
	{
		if (!camRec->distributionStopped->read())
		{
			return false;
		}
	}
	return true;
}


UINT __cdecl cvGUIRunProc(LPVOID para)
{
	// Decoding the address of the CVDisplay object
	CVDisplay* controller = (CVDisplay*)para;

	// Run the GUI
	runGUI(controller);

	// Free memory, including CVDisplay object and the GUIQueueList
	delete controller;

	// Test for termination of threads, uncomment this and the messagebox at the end of savingThreadProcessor to test
	return 0;
}

void runGUI(CVDisplay * controller)
{
	TSImagePtr GUIWindow;						// The frame of the displayed window
	TSImagePtr tmpImage;
	vector<int> acquisitionFrameRate;			// Acquisition rate of each camera
	vector<unsigned int> frameDropPerCam;		// How many frame need to be dropped for each camera to get the set display fps
	int delayBetweenFrames;
	TickMeter imgRetrTimer;						// Timer to measure how long image retrieval process last and adjust the delay between frames accordingly so that the frames are displayed at the gui_fps rate

	// Controlling when to display images
	// This help synchronize the timestamps of displayed images from all cameras
	// ImagePtr array controlling when the new images from cameras are displayed. These images are displayed only when all cameras has gotten their new images with the right index to display
	vector<TSImagePtr> newImgToDisplay(controller->getNumberGUIQueue(), NULL);
	bool newImgArrayIsFilled;			// Are all slot in the newImgToDisplay has been filled?, if this turns to true, these images in the array will be displayed

	// These below are to calculate the height and width of the display window
	int maxHeight, sumWidth;
	maxHeight = sumWidth = 0;

	// Create a list of "background" or place holder TSImage which will be used to display
	TSImageList imgList(controller->getNumberGUIQueue());

	// A flag to indicate if there is still image in the queue to be displayed
	bool notAllQueueEmpty;

	// Bootstrap the imgList with background images with the same size as the image each camera 
	int frameWidth,frameHeight;
	WaitForSingleObject(SpinSysMTX, INFINITE);
	CameraList cameraList = spinSystem->GetCameras();
	ReleaseMutex(SpinSysMTX);


	for (unsigned i = 0; i < cameraList.GetSize(); i++)
	{
		// Get and configure the camera

		CameraPtr& cam= cameraList.GetByIndex(i);
		cam->Init();

		// Get information from camera
		INodeMap& nodeMap = cam->GetNodeMap();
		frameWidth = ((CIntegerPtr)nodeMap.GetNode("Width"))->GetValue();
		frameHeight = ((CIntegerPtr)nodeMap.GetNode("Height"))->GetValue();
		acquisitionFrameRate.push_back((int)((CFloatPtr)nodeMap.GetNode("AcquisitionResultingFrameRate"))->GetValue());

		// Configure the place holder TSImage of this camera
		imgList[i] = new TSImage(frameWidth, frameHeight, string(cam->DeviceSerialNumber()));
		imgList[i]->img = cv::Scalar(0, 0, 0);			// This just to make the place holder window has a different color from the parent GUI windows, can be deleted

		// Some calculation for the parent GUI windows
		// This is when the first image from each queue has been retrived
		// Do the calculation for the display window here
		// This block will only run once in the first iteration
		// Find max height
		if (maxHeight < imgList[i]->imgHeight)
		{
			maxHeight = imgList[i]->imgHeight;
		}
		// Find total width
		sumWidth += imgList[i]->imgWidth;

		// Calculating how many frame needs to be dropped to reduce frame rate to the display framerate
		frameDropPerCam.push_back(ceil(((double)acquisitionFrameRate.back()) / recorderSetting.gui_fps));
	}


	// Calculate delay between displayed base on the recordingSetting gui framerate
	// delayBetweenFrames = ceil(1000.0 / (*max_element(acquisitionFrameRate.begin(), acquisitionFrameRate.end())));
	delayBetweenFrames = ceil(1000.0 / recorderSetting.gui_fps);

	// Configure the TSImage for the GUIparent window
	// Initialize the Mat that will be used as the frame of the displayed window
	// Create the Mat object to hold images and the GUI compoenents (buttons. windows)
	//ImageInfo GUIWindow(N * sumWidth + (N+1) * WINDOW_PADDING, maxHeight + GENERAL_BUTTON_HEIGHT + CAPTION_HEIGHT + 2 * WINDOW_PADDING);
	GUIWindow = new TSImage(sumWidth + (imgList.size() + 1) * WINDOW_PADDING, maxHeight + GENERAL_BUTTON_HEIGHT + CAPTION_HEIGHT + 2 * WINDOW_PADDING);


	// The main GUI loop
	while(true)
	{
		// Start timer measuring how long image retrieval process last
		imgRetrTimer.reset();
		imgRetrTimer.start();
		// Pull the next image to be displayed from each camera queue and place them to the display list to be displayed
		notAllQueueEmpty = false;
		// Preset the flag to check if all cameras have the correct images of which indexes are correct to be displayed
		newImgArrayIsFilled = true;
		for (unsigned i = 0; i < controller->size(); i++)
		{
			if (newImgToDisplay[i] == NULL)
			{
				if (controller->at(i)->size())
				{
					// There is something in the queue
					notAllQueueEmpty = true;							// set flag not empty to prevent this while loop gets broken

					if (newImgToDisplay[i] != NULL)
					{
						continue;
					}

					// Applying frame skipping to speed up gui display
					while (!controller->stopSignalSent)
					{
						if (!controller->at(i)->size())
						{
							// Stop getting images from the queue when the queue is empty
							newImgArrayIsFilled = false;
							break;
						}
						tmpImage = controller->at(i)->dequeue();
						if (tmpImage->frameID % frameDropPerCam[i] == 0)
						{
							newImgToDisplay[i] = tmpImage;
							//// Break out to show the image
							break;
						}
						else
						{
							delete tmpImage;
						}
					}
					if (controller->stopSignalSent)
					{
						while (controller->at(i)->size())
						{
							delete controller->at(i)->dequeue();
						}
						newImgArrayIsFilled = false;
					}
				}
				else
				{
					newImgArrayIsFilled = false;
				}
			}
		}

		// If all cameras have their images to be dislayed, copy over to imgList to display them
		if (newImgArrayIsFilled)
		{
			for (unsigned i = 0; i < controller->size(); i++)
			{
				// Before putting new image to the list, free the current image in the list
				delete imgList[i];

				// Put new image to the display list
				imgList[i] = newImgToDisplay[i];
				newImgToDisplay[i] = NULL;
			}
		}

		// If all queue are empty, then imageMinerStopped displaying
		if ( controller->isAllDistributorStopped() && !notAllQueueEmpty)
		{
			break;
		}

		// Check if 
		// TODO N: Check if the GUI rendering code below is correct

		// Check if the windows exists if not create one
		// This also prevent stalled windows when user close windows by the x button not the detach camera button
		// Solution proposed by https://medium.com/@mh_yip/opencv-detect-whether-a-window-is-closed-or-close-by-press-x-button-ee51616f7088
		if (getWindowProperty(CV_DISPLAY_ALL_CAM_RECORD_WINDOWS_NAME, cv::WND_PROP_VISIBLE) <= 0.5)
		{
			//Windows is closed or does not exist
			// Create a OpenCV window for displaying
			namedWindow(CV_DISPLAY_ALL_CAM_RECORD_WINDOWS_NAME);
			// Register with cvui
			cvui::init(CV_DISPLAY_ALL_CAM_RECORD_WINDOWS_NAME);
		}


		// Process each images in the image list
		for (unsigned i = 0; i<imgList.size(); i++)
		{
			// Draw timestamp and framerate
			drawTimeAndFPS(imgList[i]->img, imgList[i]->timestamp / 1000000000.0, acquisitionFrameRate[i]);
		}

		// --------------------- Drawing the GUI -----------------------------------
		WaitForSingleObject(mtx, INFINITE);

		cvui::context(CV_DISPLAY_ALL_CAM_RECORD_WINDOWS_NAME);

		// Draw the cvui gui ( Draw GUI after drawing the image to make the GUI on top
		drawGUIAllCam(GUIWindow->img, imgList, controller);


		// Draw the change to the window
		cvui::imshow(CV_DISPLAY_ALL_CAM_RECORD_WINDOWS_NAME, GUIWindow->img);

		// Measure the see how long image retrieval process took
		imgRetrTimer.stop();

		// Update the window
		if (newImgArrayIsFilled)
		{
			// If there are new images to be displayed, then
			// Wait with an approriate amount of time to keep the display rate equal gui_fps, minimum delay is 1ms
			waitKey((delayBetweenFrames - imgRetrTimer.getTimeMilli()) >= 1 ? delayBetweenFrames - imgRetrTimer.getTimeMilli() : 1);
		}
		else
		{
			// Just the fastest frame refresh to check the button evenclick and prevent the windows from being frozen
			waitKey(1);
		}
		ReleaseMutex(mtx);
	}
	
	//Free remaining image in GUIQueue and the newImageToBeDisplayed queue
	for (unsigned i = 0; i < controller->size();i++)
	{
		while (controller->at(i)->size())
		{
			delete controller->at(i)->dequeue();
		}
		if (newImgToDisplay[i] != NULL)
		{
			delete newImgToDisplay[i];
		}
	}

	
	// Free all the "background" images in the image list
	for (unsigned i = 0; i < imgList.size();i++)
	{
		delete imgList[i];
	}

	// Destroy OpenCV window
	destroyWindow(CV_DISPLAY_ALL_CAM_RECORD_WINDOWS_NAME);			// This is important as if the OpenCV window does not get destroyed, the next time you call imshow with the same window name, OpenCV won't create new windows. It would be just silence
}


void drawGUIAllCam(Mat& displayFrame, TSImageList& imgList, CVDisplay* controller)
{
	static bool textDrawn = false;			// This prevent text caption from being drawn many times

											//// Draw frame of two windows and one button
	cvui::beginColumn(displayFrame, WINDOW_PADDING, WINDOW_PADDING, -1, -1, WINDOW_PADDING);

	// Render the button
	if (controller->stopSignalSent)
	{
		cvui::button("Saving down frames and stop...");			// No action handler needed
	}
	else
	{
		if (cvui::button("Stop Recording"))
		{
			controller->stopAcquiring();
			controller->stopSignalSent = true;
		}
	}

	// Render the an images captured from each camera.
	cvui::beginRow(-1, -1, WINDOW_PADDING);
	// Draw images
	for (unsigned i = 0; i < imgList.size(); i++)
	{
		// Draw an image with a caption beneath
		cvui::beginColumn(-1, -1, WINDOW_PADDING);
		cvui::image(imgList[i]->img);
		if (!textDrawn)
		{
			cvui::text(imgList[i]->camSerial, 0.5, 0);
		}
		cvui::endColumn();
	}

	// Test N windows using 1 camera.  Remember to multiply the GUIWindow's sumWidth 
	// by N, comment the code snippet below "Draw images" above and change the cameraList.getSize() to N to run this test
	//unsigned N = 2;
	//for(unsigned i = 0; i < N;i++)
	//{
	//	// Draw an image with a caption beneath
	//	cvui::beginColumn(-1,-1, WINDOW_PADDING);
	//	cvui::image(imgList[0].img);
	//	if (!textDrawn)
	//	{
	//		cvui::text(string("   Serial: ") + imgList[0].camSerial, 0.5, 0);
	//	}
	//	cvui::endColumn();
	//}

	// Prevent the images' captions from being drawn next time
	textDrawn = true;

	cvui::endRow();

	cvui::endColumn();
}

