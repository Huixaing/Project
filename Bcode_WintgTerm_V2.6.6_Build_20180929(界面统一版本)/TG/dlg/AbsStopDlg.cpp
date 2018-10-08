// AbsStopDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "TG.h"
#include "AbsStopDlg.h"


// CAbsStopDlg 对话框

IMPLEMENT_DYNAMIC(CAbsStopDlg, CDialog)
CAbsStopDlg::CAbsStopDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAbsStopDlg::IDD, pParent)
	, m_reason(_T(""))
	, m_abs_stop_time(0)
{
}

CAbsStopDlg::~CAbsStopDlg()
{
}

void CAbsStopDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_reason);
	DDV_MaxChars(pDX, m_reason, 200);
	DDX_Text(pDX, IDC_EDIT2, m_abs_stop_time);
	DDV_MinMaxUInt(pDX, m_abs_stop_time, 1, 255);
}


BEGIN_MESSAGE_MAP(CAbsStopDlg, CDialog)
END_MESSAGE_MAP()


// CAbsStopDlg 消息处理程序
