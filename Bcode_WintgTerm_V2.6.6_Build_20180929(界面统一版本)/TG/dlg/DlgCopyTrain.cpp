// DlgCopyTrain.cpp : 实现文件
//

#include "stdafx.h"
#include "TG.h"
#include "DlgCopyTrain.h"
#include ".\dlgcopytrain.h"


// CDlgCopyTrain 对话框

IMPLEMENT_DYNAMIC(CDlgCopyTrain, CDialog)
CDlgCopyTrain::CDlgCopyTrain(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgCopyTrain::IDD, pParent)
	, m_trainId(_T(""))
{
}

CDlgCopyTrain::~CDlgCopyTrain()
{
}

void CDlgCopyTrain::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_CHANGE_NEWID, m_trainId);
	DDV_MaxChars(pDX, m_trainId, 9);
}


BEGIN_MESSAGE_MAP(CDlgCopyTrain, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgCopyTrain 消息处理程序

void CDlgCopyTrain::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	CWnd* pWnd = GetDlgItem(IDC_CHANGE_NEWID);
	if(pWnd != NULL)
	{
		pWnd->GetWindowText(m_trainId);
	}

	if(m_trainId.GetLength()>0)
		OnOK();
}
