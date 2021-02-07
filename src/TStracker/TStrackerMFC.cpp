#include "TStrackerMFC.h"

unordered_map<string, CamAcquireGUIThreadInfo*>	ThreadList;		// Mapping the camera to the object using its serial

																// Static member initialization
CameraSelectionDlg * TStrackerMain::camSelectDlg= nullptr;		// Pointer to the camera selector dialog

																//unordered_map<string, CamAcquireGUIThreadInfo*> TStrackerMain::ThreadList;				
GUI::GUIFactory TStrackerMain::gui;

RunOperator * runOp = nullptr;

char* waitTimePrompt = "Input the time detection threshold (in ms)? (0 or blank mean don't implement trial scheme).\nThe program will use this number to detect the break between trials.\nRecommend input half the break length";

char* summaryPrompt = "Start recording with following parameters: \nBreak detect threshold: %dms\nDisplaying FPS: %d\nSaving folder: %s";

TStrackerMain::TStrackerMain()
{	
	// Initializing mutex handle
	mtx = CreateMutex(NULL, FALSE, NULL);
	if (mtx == NULL)
	{
		MessageBox(NULL,"Error", "CreateMutex error",0);
	}
	spinSystem = System::GetInstance();
	SpinSysMTX = CreateMutex(NULL, FALSE, NULL);
}
TStrackerMain::~TStrackerMain()
{
	spinSystem->ReleaseInstance();
	CloseHandle(mtx);
	CloseHandle(SpinSysMTX);
}

CameraSelectionDlg* camSelectInitializer()
{
	static boolean isCallBackRegistered = false;
	CameraSelectionDlg* dlg = new GUI::CameraSelectionDlg();				// Using new will create a lot of instances of CameraSelectionDlg, but using gui.GetCemraSelectionDlg() will resultin problem if the user close the dialog intentionally by click X of the dialog
	if (!isCallBackRegistered)
	{
		// This prevent the program from registering the call back function with new dialog
		// Without this, the program will create a chain of the same call back and the call back funcion will be called many time when 1 event occurs
		dlg->RegisterDoubleClickedCallback(TStrackerMain::CameraSelectionDialogCamDoubleClickHandler);	// Register the handler when user double click on a camera name
		isCallBackRegistered = true;
	}
	return dlg;
}

BOOL TStrackerMain::InitInstance()
{

	// *** NOTES ***
	// Dialog based MFC applications may incorrectly initialize the threading model to MTA.
	// To prevent this, initialize the threading model to STA with the following code block.
	// Without this code block, the STA error will rise
	const HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
	if (FAILED(hr))
	{
		AfxMessageBox("CoInitializeEx initialization failed");
		//return FALSE;
	}


	// Initialize the camera selector dialog
	camSelectDlg=camSelectInitializer();


	// Initialize the main window of this application
	// Without this window, the application will terminates right after this function returns
	TStrackerMainWnd *pnframe = new TStrackerMainWnd;
	m_pMainWnd = pnframe;
	pnframe->Create(0, APP_NAME);
	pnframe->SetWindowPos(m_pMainWnd, 100, 100, 200, 170, SWP_NOZORDER);
	pnframe->ShowWindow(SW_SHOW);
	return TRUE;
}

// Camera Selection Dialog click Double Click Event handler
// Read the ClickedEventCallback for function signature
void TStrackerMain::CameraSelectionDialogCamDoubleClickHandler(
	bool isCamera,
	Spinnaker::CameraPtr* pCamera,
	Spinnaker::InterfacePtr* pInterface,
	Spinnaker::SystemPtr* pSystem,
	bool isSystem)
{
	// for now, only process if a camera is clicked
	try
	{

		if (isCamera)
		{
			// Init to the camera to get 
			(*pCamera)->Init();														//Init the camera first otherwise, DialogBox->Connect(cam) will result in runtime errors

			// Get the camera Serial number
			string camSerial = (*pCamera)->DeviceSerialNumber();

			// Check if there is already a connection to this camera in ThreadList
			if (ThreadList.count(camSerial))
			{
				// There is one CameraPtr in the list to this camera

				CamAcquireGUIThreadInfo* threadInfo = ThreadList[camSerial];
				// Check if thread has been terminated
				if (!threadInfo->threadStatus)
				{
					// Thread has been terminated
					// So delete the object of the old thread
					delete threadInfo;
					// create a new thread and get it running acquisition 
					threadInfo = ThreadList[camSerial] = new CamAcquireGUIThreadInfo(openCVCamTuning,camSerial, pCamera, TStrackerMain::gui);
				}
				else
				{
					// Thread has not been terminated

					// Inform user about this, may be he accidentally click the camera again
					MessageBox(NULL, "One thread asccociating with this camera is running", "Warning", MB_OK);

					// Or maybe he just wants to reopen the Property Dialog box. so show it!
					threadInfo->propDialog->Open();
				}
			}
			else
			{
				// This camera is new!

				// Create a thread object to acquire the image from this camera
				ThreadList[camSerial] = new CamAcquireGUIThreadInfo(openCVCamTuning, camSerial, pCamera, TStrackerMain::gui);
			}
		}
	}
	catch (Spinnaker::Exception e)
	{
		MessageBox(NULL, e.what(), "Error", MB_OK);
	}
}


// TStrackerMainWnd messages handler
BEGIN_MESSAGE_MAP(TStrackerMainWnd,CFrameWnd)
		ON_WM_CREATE()
		ON_BN_CLICKED(CAM_SELECT_DIALOG_BUTTON_ID, OpenCamSelectDialogButtonClickHandler)
		ON_BN_CLICKED(RECORD_ALL_CAMS_BUTTON_ID, RecordAllCamButtonClickHandler)
		ON_BN_CLICKED(TSTRACKER_SETTING_BUTTON_ID, TStrackerSettingButtonClickHandler)
END_MESSAGE_MAP()

int TStrackerMainWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	CFrameWnd::OnCreate(lpCreateStruct);

	// Camera select dialog button
	CamTuning.Create(_T("Tune Camera"),
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		CRect(20, 20, 160, 40),
		this, CAM_SELECT_DIALOG_BUTTON_ID);

	// Start record all cams button
	RecordAllCam.Create(_T("Record all Cameras"),
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		CRect(20, 50, 160, 70),
		this, RECORD_ALL_CAMS_BUTTON_ID);

	// Setting button
	TStrackerSetting.Create(_T("Setting"),
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		CRect(20, 80, 160, 100),
		this, TSTRACKER_SETTING_BUTTON_ID);

	return 0;
}


// Camera select dialog button handler
void TStrackerMainWnd::OpenCamSelectDialogButtonClickHandler()
{
	try
	{
		// Close the current cam select dialog, this might potential cause problem in the future?
		TStrackerMain::camSelectDlg->Close();
	}
	catch (Spinnaker::Exception e)
	{
		MessageBox(e.GetFullErrorMessage(), "Error", MB_OK);
	}
	// Create a new cam select dialog
	TStrackerMain::camSelectDlg = camSelectInitializer();
	// Show it
	TStrackerMain::camSelectDlg->Open();
	/*
	Why not just call camSelectDlg->Open() ?
	Because if the user previously close the Cam select dialog, the
	pointer camSelectDlg would point to an invalid memory thus calling camSelectDlg->Open() would cause error
	So call Close on the current one (Don't know why this does not cause error) then create a new one
	*/
}

// Record all cameras button handler
void TStrackerMainWnd::RecordAllCamButtonClickHandler()
{
	// Check if there is any camera available, if not , don't run recording
	WaitForSingleObject(SpinSysMTX, INFINITE);
	CameraList camList = spinSystem->GetCameras();
	ReleaseMutex(SpinSysMTX);
	if(camList.GetSize() == 0)
	{
		MessageBox("No camera found, aborting recording", "Note", MB_OK);
		return;
	}

	//// Close all currently running single camera video windows
	
	for (auto thread: ThreadList)
	{
		// Terminate GUI thread of windows that is still alive (only single-camera windows)
		if (thread.second->threadStatus && thread.first.compare(ALL_CAM_RECORD_WINDOWS_NAME)!=0)
		{
			// This needs to be threadsafe, so
			WaitForSingleObject(mtx, INFINITE);
			thread.second->runGUI = false;		// Signal the thread to end
			ReleaseMutex(mtx);
			WaitForSingleObject(thread.second->threadObjectPtr, INFINITE);	// Wait for the thread actually 
			// Turn off the property dialog if it is still openning
			thread.second->propDialog->Close();
		}
	}

	if (!recorderSetting.loadConfig(CONFIG_FILE_NAME))
	{
		MessageBox("No config file found, if you have not set the recording configuration, please click Setting", "Error", MB_OK);
		return;
	}

	MessageBox("Please pick an EMPTY folder to save the images", "Note", MB_OK);

	CFolderPickerDialog dlg;
	
	if (dlg.DoModal() != IDOK)
	{
		// The user did not pick a folder
		MessageBox("No folder picked, aborting all cameraca recording", "Note", MB_OK);
		return;
	}

	// Show summary of the recording setting
	char prompt[200];
	sprintf(prompt, summaryPrompt, recorderSetting.wait_time, recorderSetting.gui_fps, (string(dlg.GetPathName())).c_str());
	if (MessageBox(prompt, "Summary", MB_OKCANCEL) == IDCANCEL)
	{
		return;
	}

	//// Create the thread that displays GUI and control panel
	if (runOp == nullptr)
	{
		// THis is first run
		runOp = new RunOperator(string(dlg.GetPathName()),recorderSetting.wait_time);				// This run the run all camera
	}
	else
	{
		// There already exists a ThreadInfo object
		// Check if the thread is still alive
		if (runOp->running->read())
		{
			// This thread is still running so may be user accidentally click record all again
			// Inform them then
			MessageBox("The all cameras recording process is still running!", "Notice", MB_OK);
		}
		else
		{
			delete runOp;
			runOp = new RunOperator(string(dlg.GetPathName()), recorderSetting.wait_time);
		}
	}
	return;
}

void TStrackerMainWnd::TStrackerSettingButtonClickHandler()
{
	TSSettingDialog settingDiag;
	settingDiag.DoModal();
}


uint64_t readWaitTime(string duration)
{
	uint64_t value;
	try
	{
		value = stoll(duration);
	}
	catch (exception e)
	{
		value = 0;
	}
	return value;
}
