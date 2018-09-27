// TGColorDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "TG.h"
#include "TGColorDlg.h"
#include ".\tgcolordlg.h"


// CTGColorDlg 对话框

IMPLEMENT_DYNAMIC(CTGColorDlg, CDialog)
CTGColorDlg::CTGColorDlg(CWnd* pParent)
	: CDialog(CTGColorDlg::IDD, pParent)
{
}

CTGColorDlg::~CTGColorDlg()
{
}

void CTGColorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SLIDER1, m_slider);
}


BEGIN_MESSAGE_MAP(CTGColorDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	
END_MESSAGE_MAP()


// CTGColorDlg 消息处理程序

BOOL CTGColorDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	m_slider.SetRange(0, 50);
	m_slider.SetPos(mColor);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CTGColorDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	mColor = m_slider.GetPos();
	OnOK();
}

