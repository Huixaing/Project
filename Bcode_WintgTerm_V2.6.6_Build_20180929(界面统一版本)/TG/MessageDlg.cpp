// MessageDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "TG.h"
#include "MessageDlg.h"
#include ".\messagedlg.h"


// CMessageDlg 对话框

IMPLEMENT_DYNAMIC(CMessageDlg, CDialog)
CMessageDlg::CMessageDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMessageDlg::IDD, pParent)
{
}

CMessageDlg::~CMessageDlg()
{
}

void CMessageDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_TEXT, m_textmsg);
}


BEGIN_MESSAGE_MAP(CMessageDlg, CDialog)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()

// CMessageDlg 消息处理程序
void CMessageDlg::ShowMessage(CStringArray& text)
{
	CString warning;
	for(int i=0; i<text.GetCount(); i++)
	{
		if(i==0)
			warning = text.GetAt(i);
		else
			warning += "\r\n"+text.GetAt(i);
	}
	m_textmsg.SetWindowText(warning);
}

void CMessageDlg::OnBnClickedOk()
{
	DestroyWindow();
}

void CMessageDlg::OnClose()
{
	DestroyWindow();
}

void CMessageDlg::PostNcDestroy() 
{
	delete this;
}