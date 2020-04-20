#include "TStrackerMFC.h"

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

	SystemPtr system = System::GetInstance();

	CFrameWnd *pnframe = new CFrameWnd;
	m_pMainWnd = pnframe;
	pnframe->Create(0, "Buster");
	pnframe->ShowWindow(SW_SHOW);
	return TRUE;
}

