// DlgMarkResult.cpp : 实现文件
//

#include "stdafx.h"
#include "resource.h"
#include "DlgMarkResult.h"


// CDlgMarkResult 对话框

IMPLEMENT_DYNAMIC(CDlgMarkResult, CDialog)
CDlgMarkResult::CDlgMarkResult(const char *caption,const char *inform,CWnd* pParent /*=NULL*/)
: CDialog(CDlgMarkResult::IDD, pParent)
{
	m_szCaption=caption;
	m_szInform=inform;
}

CDlgMarkResult::~CDlgMarkResult()
{
}

void CDlgMarkResult::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgMarkResult, CDialog)
END_MESSAGE_MAP()


// CDlgMarkResult 消息处理程序

BOOL CDlgMarkResult::OnInitDialog() 
{
	CDialog::OnInitDialog();
	//SetWindowText(m_szCaption);
	GetDlgItem(IDC_RESULT)->SetWindowText(m_szCaption);
	GetDlgItem(IDC_RESULT_EDITBOX)->SetWindowText(m_szInform);
	return TRUE;
}
void CDlgMarkResult::OnOK()
{	
	CDialog::OnOK();
}