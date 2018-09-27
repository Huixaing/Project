// TransportInfo.cpp : implementation file
//

#include "stdafx.h"
#include ".\workcancel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCancelWorkDlg dialog


CCancelWorkDlg::CCancelWorkDlg(CWnd* pParent)
	: CDialog(CCancelWorkDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCancelWorkDlg)
	//}}AFX_DATA_INIT
	m_nCancelCode=-1;
	m_strCancelReason="";
	m_strText="";
}


void CCancelWorkDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCancelWorkDlg)
	//}}AFX_DATA_MAP
	DDX_Text(pDX, IDC_REPORT_CONTENT, m_strText);
	DDV_MaxChars(pDX, m_strText, 255);
}


BEGIN_MESSAGE_MAP(CCancelWorkDlg, CDialog)
	//{{AFX_MSG_MAP(CCancelWorkDlg)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCancelWorkDlg message handlers

BOOL CCancelWorkDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

    CWnd *pWnd = GetDlgItem(IDC_EDIT1);	
	if(pWnd)
		pWnd->SetWindowText(m_strPlanID);
	

    CComboBox* pComboBox = (CComboBox*)GetDlgItem(IDC_COMBO1);	
	if(pComboBox)
	{
		pComboBox->AddString("天气不良");
		pComboBox->AddString("部局令取消");
		pComboBox->AddString("施工单位未登记");
		pComboBox->AddString("车流调整");
		pComboBox->AddString("其它");
		pComboBox->SetCurSel(0);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CCancelWorkDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	CComboBox* pComboBox = (CComboBox*)GetDlgItem(IDC_COMBO1);
	if(pComboBox)
	{
		m_nCancelCode = pComboBox->GetCurSel()+1;
		pComboBox->GetWindowText(m_strCancelReason);
	}

	OnOK();
}

void CCancelWorkDlg::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	OnCancel();
}
