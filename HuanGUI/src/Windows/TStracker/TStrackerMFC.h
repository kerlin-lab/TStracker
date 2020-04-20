#pragma once
#include <afxwin.h>
#include "TStracker.h"

class TStrackerMain : public CWinApp
{
public:
	TStrackerMain();

	~TStrackerMain();

	BOOL InitInstance();

private:
	GUI::GUIFactory gui;
	CameraSelectionDlg * camSelectDlg;
	CameraPtr cam;
};