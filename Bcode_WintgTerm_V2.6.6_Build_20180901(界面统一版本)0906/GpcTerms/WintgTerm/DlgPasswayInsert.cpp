// DlgPasswayInsert.cpp : 实现文件
//

#include "stdafx.h"
#include "resource.h"
#include "DlgPasswayInsert.h"
#include ".\dlgpasswayinsert.h"


// CDlgPasswayInsert 对话框

IMPLEMENT_DYNAMIC(CDlgPasswayInsert, CDialog)
CDlgPasswayInsert::CDlgPasswayInsert(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgPasswayInsert::IDD, pParent)

{
}

CDlgPasswayInsert::~CDlgPasswayInsert()
{
}

void CDlgPasswayInsert::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgPasswayInsert, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgPasswayInsert 消息处理程序

void CDlgPasswayInsert::OnBnClickedOk()
{
	CString strText;
	CWnd *pWnd = GetDlgItem(IDC_EDIT_PASSWAY_ID);
	pWnd->GetWindowText(strText);
	m_nPasswayId = atoi(strText);

	pWnd = GetDlgItem(IDC_EDIT_PASSWAY_NAME);
	pWnd->GetWindowText(strText);	 
	m_nPasswayName = strText;

	
	OnOK();
}
