#pragma once
#include <afxwin.h>
#include "TStracker.h"

#define CAM_SELECT_DIALOG_BUTTON_ID 1


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
	//static map<
	CameraPtr cam;
	
	static SystemPtr spinSys;						// Pointer to the kernel of Spinnaker SDK
};


// Main window

class TStrackerMainWnd : public CFrameWnd
{
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct); 
	// These button handler function must be decleared inside TStrackerMainWnd so that they can be mentioned in the class's MESSAGE_BEGIN_MAP block
	afx_msg void OpenCamSelectDialogButtonClick();
private:
	CButton ShowCamSelectDialog;
	DECLARE_MESSAGE_MAP()
};



// Button handlers
