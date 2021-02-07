// TSSettingDialog.cpp : implementation file
//

#include "TStracker.h"
#include "TSSettingDialog.h"
#include "afxdialogex.h"

// TSSettingDialog dialog

IMPLEMENT_DYNAMIC(TSSettingDialog, CDialog)

TSSettingDialog::TSSettingDialog(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_DIALOG2, pParent)
{
}

TSSettingDialog::~TSSettingDialog()
{
}

void TSSettingDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, trialBreakDetectThreshold);
	DDX_Control(pDX, IDC_EDIT2, guiFPS);
}


BEGIN_MESSAGE_MAP(TSSettingDialog, CDialog)
	ON_BN_CLICKED(IDOK, &TSSettingDialog::OnBnClickedOk)
END_MESSAGE_MAP()


// TSSettingDialog message handlers


void TSSettingDialog::OnBnClickedOk()
{
	char buff[100];
	trialBreakDetectThreshold.GetLine(0, buff);
	recorderSetting.wait_time = std::stoi(buff);
	guiFPS.GetLine(0, buff);
	recorderSetting.gui_fps= std::stoi(buff);
	if (!recorderSetting.saveConfig(CONFIG_FILE_NAME))
	{
		MessageBox("Cannot save config file!", "Error", MB_OK);
	}
	CDialog::OnOK();
}


BOOL TSSettingDialog::OnInitDialog()
{
	CDialog::OnInitDialog();
	if (recorderSetting.loadConfig(CONFIG_FILE_NAME))
	{
		trialBreakDetectThreshold.SetWindowTextA(to_string(recorderSetting.wait_time).c_str());
		guiFPS.SetWindowTextA(to_string(recorderSetting.gui_fps).c_str());
	}

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}
