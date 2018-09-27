// NeibPlanFuncChoiceDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "NeibPlanFuncChoiceDlg.h"
#include ".\neibplanfuncchoicedlg.h"


// CNeibPlanFuncChoiceDlg 对话框

IMPLEMENT_DYNAMIC(CNeibPlanFuncChoiceDlg, CDialog)
CNeibPlanFuncChoiceDlg::CNeibPlanFuncChoiceDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CNeibPlanFuncChoiceDlg::IDD, pParent)
{
	nRet=0;
}

CNeibPlanFuncChoiceDlg::~CNeibPlanFuncChoiceDlg()
{
}

void CNeibPlanFuncChoiceDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CNeibPlanFuncChoiceDlg, CDialog)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
END_MESSAGE_MAP()


// CNeibPlanFuncChoiceDlg 消息处理程序

void CNeibPlanFuncChoiceDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	nRet=0;
	OnOK();
}

void CNeibPlanFuncChoiceDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	nRet=1;
	OnOK();
}

void CNeibPlanFuncChoiceDlg::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	nRet=2;
	OnOK();
}

BOOL CNeibPlanFuncChoiceDlg::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->message==WM_KEYDOWN && pMsg->wParam==VK_ESCAPE)
	{
		nRet=0;
		OnOK();
		return TRUE;
	}
	return CDialog::PreTranslateMessage(pMsg);
}
