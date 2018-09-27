// DlgSimpleInput.cpp : 实现文件
//

#include "stdafx.h"
#include "resource.h"
#include "DlgSimpleInput.h"


// CDlgSimpleInput 对话框

IMPLEMENT_DYNAMIC(CDlgSimpleInput, CDialog)
CDlgSimpleInput::CDlgSimpleInput(const char *caption,const char *inform,const char *defvalue,CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSimpleInput::IDD, pParent)
{
	m_szCaption=caption;
	m_szInform=inform;
	m_szValue=defvalue;
}

CDlgSimpleInput::~CDlgSimpleInput()
{
}

void CDlgSimpleInput::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgSimpleInput, CDialog)
END_MESSAGE_MAP()


// CDlgSimpleInput 消息处理程序

BOOL CDlgSimpleInput::OnInitDialog() 
{
	CDialog::OnInitDialog();
	SetWindowText(m_szCaption);
	GetDlgItem(IDC_CAPTION)->SetWindowText(m_szInform);
	GetDlgItem(IDC_ED_VALUE)->SetWindowText(m_szValue);
	return TRUE;
}
void CDlgSimpleInput::OnOK()
{
	GetDlgItem(IDC_ED_VALUE)->GetWindowText(m_szValue);
	CDialog::OnOK();
}