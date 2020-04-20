#include "TStrackerMFC.h"

// Static member initialization
CameraSelectionDlg * TStrackerMain::camSelectDlg= nullptr;		// Pointer to the camera selector dialog
SystemPtr TStrackerMain::spinSys;								// Pointer to the kernel of Spinnaker SDK
map<string, CameraPtr*> TStrackerMain::CamList;				
GUI::GUIFactory TStrackerMain::gui;

TStrackerMain::TStrackerMain()
{	
}
TStrackerMain::~TStrackerMain()
{
}

CameraSelectionDlg* camSelectInitializer()
{
	CameraSelectionDlg* dlg = new GUI::CameraSelectionDlg();
	dlg->RegisterDoubleClickedCallback(TStrackerMain::CameraSelectionDialogCamDoubleClickHandler);	// Register the handler when user double click on a camera name
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
	if (isCamera)
	{
		// Check if there is already a connection to this camera in CamList
		//string camSerial = (*pCamera)->DeviceSerialNumber();
		//if (TStrackerMain::CamList.count(camSerial))
		//{
		//	// There is one CameraPtr in the list to this camera
		//	try
		//	{
		//		// De-Init the old pointer
		//		(*CamList[camSerial])->DeInit();
		//	}
		//	catch (Spinnaker::Exception& e)
		//	{
		//		if (e.GetError() != SPINNAKER_ERR_TIMEOUT)
		//		{
		//			MessageBox(NULL, e.GetFullErrorMessage(), "Error", MB_OK);
		//		}
		//	}
		//	// Save the new pointer to camera
		//	CamList[camSerial] = pCamera;
		//}
		// Open Acquisition and the property dialog box for the clicked 
		(*pCamera)->Init();										//Init the camera first otherwise, DialogBox->Connect(cam) will result in runtime errors
		// Set up property dialog for this camera
		TStrackerMain::gui.ConnectGUILib(*pCamera);				// This some how prevent the camera selection box from disapearing after choosing a camera
		PropertyGridDlg* dlg = TStrackerMain::gui.GetPropertyGridDlg();
		//PropertyGridDlg* dlg = new GUI::PropertyGridDlg();
		dlg->Connect(*pCamera);									// Connect the dialog to the camera
		dlg->Open();											// show the dialog
		// Set up a thread uses OpenCV to acquire and save image
		AfxBeginThread(openCVCamCapture, pCamera);
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
		free(TStrackerMain::camSelectDlg);
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
	try
	{
		// Close the current cam select dialog, this might potential cause problem in the future?
		TStrackerMain::camSelectDlg->Close();
		free(TStrackerMain::camSelectDlg);
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
