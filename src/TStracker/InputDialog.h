#pragma once
#include "afxwin.h"
#include "rc/resource.h"


// InputDialog dialog

class InputDialog : public CDialog
{
	DECLARE_DYNAMIC(InputDialog)

public:
	InputDialog(CWnd* pParent = NULL);   // standard constructor
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
//	string Prompt;
	CString UserResponse;
	CButton OKButton;
	CButton CancelButton;
	// Setting the dialog prompt text
	void setPrompt(string prompt);
//	afx_msg void OnStnClickedPrompt();
	CStatic prompt;
	string displayPrompt;
	afx_msg void OnBnClickedOk();
	string answer;
};
