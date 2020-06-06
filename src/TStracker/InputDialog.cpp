// InputDialog.cpp : implementation file
//

#include "stdafx.h"
#include "TStracker.h"
#include "InputDialog.h"
#include "afxdialogex.h"


// InputDialog dialog

IMPLEMENT_DYNAMIC(InputDialog, CDialog)

InputDialog::InputDialog(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_DIALOG1, pParent)
	, UserResponse(_T(""))
{

}

InputDialog::InputDialog(string prompt)
{
	setPrompt(prompt);
}

int InputDialog::OnInitDialog()
{
	CDialog::OnInitDialog();
	this->prompt.SetWindowTextA(this->displayPrompt.c_str());
	return TRUE;
}

InputDialog::~InputDialog()
{
}

void InputDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, UserResponse);
	DDX_Control(pDX, IDOK, OKButton);
	DDX_Control(pDX, IDCANCEL, CancelButton);
	DDX_Control(pDX, IDC_PROMPT, prompt);
}


BEGIN_MESSAGE_MAP(InputDialog, CDialog)
//	ON_STN_CLICKED(IDC_PROMPT, &InputDialog::OnStnClickedPrompt)
ON_BN_CLICKED(IDOK, &InputDialog::OnBnClickedOk)
END_MESSAGE_MAP()


// InputDialog message handlers


// Setting the dialog prompt text
void InputDialog::setPrompt(string prompt)
{
	this->displayPrompt = prompt;
}


//void InputDialog::OnStnClickedPrompt()
//{
//	// TODO: Add your control notification handler code here
//}


void InputDialog::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	CDialog::OnOK();
}
