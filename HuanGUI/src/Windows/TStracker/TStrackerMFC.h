#pragma once
#include <afxwin.h>
#include "TStracker.h"

class TStrackerMain : public CWinApp
{
// Method
public:
	TStrackerMain();

	~TStrackerMain();

	BOOL InitInstance();

	// Camera Selection Dialog click Double Click Event handler
	// Read the ClickedEventCallback for function signature
	static void CameraSelectionDialogCamDoubleClickHandler(
		bool isCamera,
		Spinnaker::CameraPtr* pCamera,
		Spinnaker::InterfacePtr* pInterface,
		Spinnaker::SystemPtr* pSystem,
		bool isSystem);

// Fields
public:
	// For GUI 
	static GUI::GUIFactory gui;
	static CameraSelectionDlg * camSelectDlg;		// Pointer to the camera selector dialog

	CameraPtr cam;
	
	static SystemPtr spinSys;						// Pointer to the kernel of Spinnaker SDK
};



class TStrackerMainWnd : public CFrameWnd
{
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct); 
	DECLARE_MESSAGE_MAP()
private:
	CButton ShowCamSelectDialog;
};