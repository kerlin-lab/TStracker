#include "CVDisplay.h"

string CV_DISPLAY_ALL_CAM_RECORD_WINDOWS_NAME = "Recording all cameras";

#define GENERAL_BUTTON_HEIGHT 33
#define CAPTION_HEIGHT 30
#define WINDOW_PADDING 10


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


// Launching the CVGUI thread of this class. Once the thread is launched, queueList is immutable
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
	//MessageBox(NULL, "GUI thread terminated", "Error", MB_OK);
	return 0;
}

void runGUI(CVDisplay * controller)
{
	TSImagePtr GUIWindow;		// The frame of the displayed window

	// TODO 5: adding frame rate into TSImage and delete this frameRate vector below
	vector<int> frameRate;
	vector<uint64_t> duration, intialTimestamp;
	uint64_t timestamp;

	// These below are to calculate the height and width of the display window
	int maxHeight, sumWidth;
	maxHeight = sumWidth = 0;

	// Create a list of "background" TSImage which will be used to display
	TSImageList imgList(controller->getNumberGUIQueue());

	// A flag to indicate if there is still image in the queue to be displayed
	bool notAllQueueEmpty;

	// Flag to indicate if this is the iteration of the while loop
	bool firstIteration = true;

	while(true)
	{
		//MessageBox(NULL,"GUI running", "Notice", MB_OK);
		// Pull the next image to be displayed from each camera queue and place them to the display list to be displayed
		notAllQueueEmpty = false;
		for (unsigned i = 0; i < controller->size(); i++)
		{
			if (controller->at(i)->size())
			{
				// There is something in the queue
				notAllQueueEmpty = true;		// set flag not empty to prevent this while loop gets broken
				// Before putting new image to the list, free the current image in the list
				if (!firstIteration)
				{
					// Only free the image if this is not the first iteration
					delete imgList[i];
				}
				// Put new image to the display list
				imgList[i] = controller->at(i)->dequeue();
			}
			else
			{
				if (firstIteration)
				{
					// If this is the first iteration, each slot in imgList needed to be filled
					// so keep polling to see if images comes yet
					i--;
					continue;
				}
				// Do nothing if the queue is empty or not first iteration
			}
		}

		if (firstIteration)
		{
			// This is when the first image from each queue has been retrived
			// Do the calculation for the display window here
			// This block will only run once in the first iteration
			// Find max height
			for (unsigned i = 0; i < imgList.size(); i++)
			{
				if (maxHeight < imgList[i]->imgHeight)
				{
					maxHeight = imgList[i]->imgHeight;
				}
				// Find total width
				sumWidth += imgList[i]->imgWidth;
			}

			// Initialize the Mat that will be used as the frame of the displayed window
			// Create the Mat object to hold images and the GUI compoenents (buttons. windows)
			//ImageInfo GUIWindow(N * sumWidth + (N+1) * WINDOW_PADDING, maxHeight + GENERAL_BUTTON_HEIGHT + CAPTION_HEIGHT + 2 * WINDOW_PADDING);
			GUIWindow = new TSImage(sumWidth + (imgList.size() + 1) * WINDOW_PADDING, maxHeight + GENERAL_BUTTON_HEIGHT + CAPTION_HEIGHT + 2 * WINDOW_PADDING);

			// TODO: This is just place holder, try to fix in the future
			frameRate.push_back(0);

			// Reserve slot for first image timestamp saver
			intialTimestamp.push_back(0);
		}

		firstIteration = false;			// Set flag so that iteration after the first one will be marked not first iteration

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
			// Get timestamp
			timestamp = imgList[i]->timestamp;
			if (intialTimestamp[i] == UINT64_MAX)
			{
				intialTimestamp[i] = timestamp;
			}
			timestamp -= intialTimestamp[i];

			// Save the adjusted timestamp not the original timestmap since the original timestamp does not get reseted before each run
			imgList[i]->timestamp = timestamp;

			// Draw timestamp and framerate
			drawTimeAndFPS(imgList[i]->img, timestamp / 1000000000.0, frameRate[i]);
		}

		// --------------------- Drawing the GUI -----------------------------------
		WaitForSingleObject(mtx, INFINITE);

		cvui::context(CV_DISPLAY_ALL_CAM_RECORD_WINDOWS_NAME);

		// Draw the cvui gui ( Draw GUI after drawing the image to make the GUI on top
		drawGUIAllCam(GUIWindow->img, imgList,controller);

		// Draw the change to the window
		cvui::imshow(CV_DISPLAY_ALL_CAM_RECORD_WINDOWS_NAME, GUIWindow->img);
		//MessageBox(NULL,"Should show something", "Notice", MB_OK);

		// Update the window
		waitKey(1);
		ReleaseMutex(mtx);
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
		cvui::button("Waiting to stop");			// No action handler needed
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
			cvui::text(string("  Serial: ") + imgList[i]->camSerial, 0.5, 0);
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

