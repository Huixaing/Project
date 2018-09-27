// InputTextDlg.cpp : implementation file
//

#include "stdafx.h"

#include "InputTextDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CInputTextDlg dialog


CInputTextDlg::CInputTextDlg(BYTE flag, CWnd* pParent /*=NULL*/)
	: CDialog(CInputTextDlg::IDD, pParent)
{
	m_flag=flag;
}


void CInputTextDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInputTextDlg)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CInputTextDlg, CDialog)
	//{{AFX_MSG_MAP(CInputTextDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInputTextDlg message handlers

void CInputTextDlg::OnOK() 
{
	UpdateData(TRUE);
	inputtext="";
	CEdit* pEdit=(CEdit*)GetDlgItem(IDC_TEXT);
	if(pEdit)
		pEdit->GetWindowText(inputtext);
	
	CDialog::OnOK();
}

BOOL CInputTextDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	//IDD_DLG_INPUT
	if(m_flag==1)
	{
		this->SetWindowText("ÊäÈë³µ×éºÅ");
		CEdit* pEdit=(CEdit*)GetDlgItem(IDC_TEXT);
		if(pEdit)
		{
			pEdit->SetLimitText(24);
			pEdit->SetFocus();
		}
	}
	// TODO: Add extra initialization here
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

