#pragma once
#include "afxwin.h"
#include "rc/resource.h"


class InputDialog : public CDialog
{
	DECLARE_DYNAMIC(InputDialog)

public:
	InputDialog(CWnd* pParent = NULL);
	InputDialog(string prompt);
	int OnInitDialog();
	virtual ~InputDialog();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG1 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	// The prompt of the Dialog
	CString UserResponse;
	CButton OKButton;
	CButton CancelButton;
	// Setting the dialog prompt text
	CStatic prompt;
	string displayPrompt;
	string answer;

public:
	afx_msg void OnBnClickedOk();
	void setPrompt(string prompt);
};
