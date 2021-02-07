/*
	Very good 
	example of how to create Dialog
	https://www.tenouk.com/visualcplusmfc/visualcplusmfc5.html
	And this for how to set static text
	https://groups.google.com/forum/#!topic/microsoft.public.vc.mfc/OoKIkvj34bA
*/

#ifndef TSTRACKERMFC_H
#define TSTRACKERMFC_H

#include <afxwin.h>
#include <afxdlgs.h>
#include <afxcmn.h>

#include "TStracker.h"
#include "TStrackerThread.h"
#include "RunOperator.h"
#include "rc\resource.h"
#include "InputDialog.h"
#include "TSSettingDialog.h"
#include "stdafx.h"

#define CVUI_IMPLEMENTATION
#include "cvui.h"

#define CAM_SELECT_DIALOG_BUTTON_ID 1
#define RECORD_ALL_CAMS_BUTTON_ID 2
#define TSTRACKER_SETTING_BUTTON_ID 3

// Global variable
extern unordered_map<string, CamAcquireGUIThreadInfo*>	ThreadList;		// Mapping the camera to the object using its serial


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
};


// Main window

class TStrackerMainWnd : public CFrameWnd
{
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	// These button handler function must be decleared inside TStrackerMainWnd so that they can be mentioned in the class's MESSAGE_BEGIN_MAP block
	afx_msg void OpenCamSelectDialogButtonClickHandler();
	afx_msg void RecordAllCamButtonClickHandler();
	afx_msg void TStrackerSettingButtonClickHandler();

private:
	CButton CamTuning;
	CButton RecordAllCam;
	CButton TStrackerSetting;

	DECLARE_MESSAGE_MAP()
};

uint64_t readWaitTime(string duration);

#endif // !TSTRACKERMFC_H


