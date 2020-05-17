#include "TStrackerMFC.h"

unordered_map<string, CamAcquireThreadInfo*>	CamList;		// Mapping the camera to the object using its serial

																// Static member initialization
CameraSelectionDlg * TStrackerMain::camSelectDlg= nullptr;		// Pointer to the camera selector dialog
SystemPtr TStrackerMain::spinSys;								// Pointer to the kernel of Spinnaker SDK
//unordered_map<string, CamAcquireThreadInfo*> TStrackerMain::CamList;				
GUI::GUIFactory TStrackerMain::gui;

TStrackerMain::TStrackerMain()
{	
}
TStrackerMain::~TStrackerMain()
{
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
	//camSelectDlg->Open();				// Show camera selector


	// Initialize the pointer to communicate with Spinnaker kernel
	spinSys = System::GetInstance();

	// Initialize the main window of this application
	// Without this window, the application will terminates right after this function returns
	TStrackerMainWnd *pnframe = new TStrackerMainWnd;
	m_pMainWnd = pnframe;
	pnframe->Create(0, APP_NAME);
	pnframe->SetWindowPos(m_pMainWnd, 100, 100, 200, 150, SWP_NOZORDER);
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
	//MessageBox(NULL, "CALLED", "INFORM", MB_OK);
	try
	{

		if (isCamera)
		{
			// Init to the camera to get 
			(*pCamera)->Init();														//Init the camera first otherwise, DialogBox->Connect(cam) will result in runtime errors

			// Get the camera Serial number
			string camSerial = (*pCamera)->DeviceSerialNumber();

			// Check if there is already a connection to this camera in CamList
			if (CamList.count(camSerial))
			{
				//MessageBox(NULL, "OLD", "INFORM", MB_OK);
				// There is one CameraPtr in the list to this camera

				CamAcquireThreadInfo* threadInfo = CamList[camSerial];
				// Check if thread has been terminated
				if (!threadInfo->threadStatus)
				{
					// Thread has been terminated
					//MessageBox(NULL, "Thread terminated", "INFORM", MB_OK);
					// So delete the object of the old thread
					delete threadInfo;
					// create a new thread and get it running acquisition 
					threadInfo = CamList[camSerial] = new CamAcquireThreadInfo(camSerial, pCamera, TStrackerMain::gui);
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
				//MessageBox(NULL, "NEW", "INFORM", MB_OK);
				// This camera is new!

				// Create a thread object to acquire the image from this camera
				CamList[camSerial] = new CamAcquireThreadInfo(camSerial, pCamera, TStrackerMain::gui);
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
END_MESSAGE_MAP()

int TStrackerMainWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	CFrameWnd::OnCreate(lpCreateStruct);

	// Camera select dialog button
	ShowCamSelectDialog.Create(_T("Select Camera"),
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		CRect(20, 20, 160, 40),
		this, CAM_SELECT_DIALOG_BUTTON_ID);

	// Start record all cams button
	RecordAllCam.Create(_T("Record all Cameras"),
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		CRect(20, 50, 160, 70),
		this, RECORD_ALL_CAMS_BUTTON_ID);

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
	return;
}
