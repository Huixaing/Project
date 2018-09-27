// DlgCheZuHao.cpp : 实现文件
//

#include "stdafx.h"
#include "TG.h"
#include "DlgCheZuHao.h"
#include ".\dlgchezuhao.h"


// CDlgCheZuHao 对话框

IMPLEMENT_DYNAMIC(CDlgCheZuHao, CDialog)
CDlgCheZuHao::CDlgCheZuHao(CTG_TRAIN* train, CTGDoc *pDoc, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgCheZuHao::IDD, pParent)
{
	m_train=*train;
	m_pDoc = pDoc;
}

CDlgCheZuHao::~CDlgCheZuHao()
{
}

void CDlgCheZuHao::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_CZH, m_strCheZuHao);
	DDV_MaxChars(pDX, m_strCheZuHao, 24);
}


BEGIN_MESSAGE_MAP(CDlgCheZuHao, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgCheZuHao 消息处理程序
BOOL CDlgCheZuHao::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_strCheZuHao=m_train.myTrainInfo.shift_index_string;
		
	TgTRAIN_RECORD *first = m_train.GetFirstRecord();
	TgTRAIN_RECORD *last = m_train.GetLastRecord();

	CEdit *pEdit = (CEdit*)GetDlgItem(IDC_EDIT1);	
	if(pEdit)
		pEdit->SetWindowText(m_pDoc->GetTrainShowId(m_train));

	if(first)
	{
		CEdit *pEdit = (CEdit*)GetDlgItem(IDC_EDIT2);	
		if(pEdit)
			pEdit->SetWindowText(m_pDoc->GetStationName(first->station));
	}
	if(last)
	{
		CEdit *pEdit = (CEdit*)GetDlgItem(IDC_EDIT7);	
		if(pEdit)
			pEdit->SetWindowText(m_pDoc->GetStationName(last->station));
	}

	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgCheZuHao::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	if(m_strCheZuHao.GetLength() > 24)
	{
		AfxMessageBox("请注意: 你输入的车组号超过了24个字节!,请缩减内容");
		return;
	}
	CDialog::OnOK();
}


