// PasswordInputDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "PasswordInputDlg.h"

namespace TCC_C3
{
// CPasswordInputDlg 对话框

IMPLEMENT_DYNAMIC(CPasswordInputDlg, CDialog)
CPasswordInputDlg::CPasswordInputDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPasswordInputDlg::IDD, pParent)
{
}

CPasswordInputDlg::~CPasswordInputDlg()
{
}

void CPasswordInputDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_EDIT1, m_szPasswd);
}

BEGIN_MESSAGE_MAP(CPasswordInputDlg, CDialog)
END_MESSAGE_MAP()

// CPasswordInputDlg 消息处理程序
BOOL CPasswordInputDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	m_szPasswd = "";
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CPasswordInputDlg::OnOK()
{
	UpdateData(TRUE);

	CDialog::OnOK();
}



}