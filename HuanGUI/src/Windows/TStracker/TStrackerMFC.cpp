#include "TStrackerMFC.h"

// Static member initialization
CameraSelectionDlg * TStrackerMain::camSelectDlg= nullptr;		// Pointer to the camera selector dialog
SystemPtr TStrackerMain::spinSys;						// Pointer to the kernel of Spinnaker SDK

TStrackerMain::TStrackerMain()
{

	
}

TStrackerMain::~TStrackerMain()
{
	// camSelectDlg->Close();
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
	camSelectDlg = gui.GetCameraSelectionDlg();
	camSelectDlg->RegisterDoubleClickedCallback(TStrackerMain::CameraSelectionDialogCamDoubleClickHandler);	// Register the handler when user double click on a camera name
	camSelectDlg->Open();

	// Initialize the pointer to communicate with Spinnaker kernel
	spinSys = System::GetInstance();

	// Initialize the main window of this application
	// Without this window, the application will terminates right after this function returns
	TStrackerMainWnd *pnframe = new TStrackerMainWnd;
	m_pMainWnd = pnframe;
	pnframe->Create(0, APP_NAME);
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
		// Open Acquisition and the property dialog box for the clicked 

		//Init the camera first otherwise, DialogBox->Connect(cam) will result in runtime errors
		(*pCamera)->Init();
		
		// Get this camera a Property Control Dialog

		// Reopen selection dialog and keep the Dialog always on the screen
		camSelectDlg->Open();
		MessageBox(NULL, "Yes", "Test", MB_OK);
		return;
	}
}

BEGIN_MESSAGE_MAP(TStrackerMainWnd,CFrameWnd)
		ON_WM_CREATE()
END_MESSAGE_MAP()

int TStrackerMainWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	CFrameWnd::OnCreate(lpCreateStruct);
	MessageBox("Yes", "Test", MB_OK);
	ShowCamSelectDialog.Create(_T("My button"),
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		CRect(10, 10, 100, 30),
		this, 1);
	return 0;
}
