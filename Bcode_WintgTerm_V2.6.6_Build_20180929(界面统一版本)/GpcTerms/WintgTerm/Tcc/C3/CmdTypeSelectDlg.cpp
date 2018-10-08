// PasswordInputDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "CmdTypeSelectDlg.h"

namespace TCC_C3
{
// CPasswordInputDlg 对话框

IMPLEMENT_DYNAMIC(CCmdTypeSelectDlg, CDialog)
CCmdTypeSelectDlg::CCmdTypeSelectDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCmdTypeSelectDlg::IDD, pParent)
{
}

CCmdTypeSelectDlg::~CCmdTypeSelectDlg()
{
}

BEGIN_MESSAGE_MAP(CCmdTypeSelectDlg, CDialog)
END_MESSAGE_MAP()


// CCmdTypeSelectDlg 消息处理程序
BOOL CCmdTypeSelectDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	if(CWnd *pParent = GetParent())
	{
		CRect rcParentWnd;
		pParent->GetWindowRect(&rcParentWnd);

		CRect rcWnd;
		GetWindowRect(&rcWnd);

		CPoint pt;
		pt.x = rcParentWnd.left + (rcParentWnd.Width() - rcWnd.Width())/2;
		pt.y = rcParentWnd.top  + (rcParentWnd.Height()- rcWnd.Height())/2;

		SetWindowPos(NULL, pt.x, pt.y, rcWnd.Width(), rcWnd.Height(), SWP_NOSIZE);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


}