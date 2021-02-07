#pragma once
#include "afxwin.h"
#include "rc/resource.h"


// TSSettingDialog dialog

class TSSettingDialog : public CDialog
{
	DECLARE_DYNAMIC(TSSettingDialog)

public:
	TSSettingDialog(CWnd* pParent = nullptr);   // standard constructor
	virtual ~TSSettingDialog();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG2 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	CEdit trialBreakDetectThreshold;
	CEdit guiFPS;
	virtual BOOL OnInitDialog();
};
