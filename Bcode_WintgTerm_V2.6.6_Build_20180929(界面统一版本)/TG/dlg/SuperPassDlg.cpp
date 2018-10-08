// SuperPassDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SuperPassDlg.h"

#include "messages.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSuperPassDlg dialog


CSuperPassDlg::CSuperPassDlg(CString title,CWnd* pParent /*=NULL*/)
	: CDialog(CSuperPassDlg::IDD, pParent)
	, m_strPrompt(_T(""))
{
	//{{AFX_DATA_INIT(CSuperPassDlg)
	m_super_pass = _T("");
	//}}AFX_DATA_INIT
	m_strTitle = title;
}


void CSuperPassDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSuperPassDlg)
	DDX_Text(pDX, IDC_PASSWORD, m_super_pass);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSuperPassDlg, CDialog)
	//{{AFX_MSG_MAP(CSuperPassDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSuperPassDlg message handlers

BOOL CSuperPassDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	if(m_strTitle != "")
		SetWindowText(m_strTitle);
	if(!m_strPrompt.IsEmpty())
	{
		CWnd *pWnd = GetDlgItem(IDC_STATIC_PROMPT);
		ASSERT(pWnd != NULL);
		pWnd->SetWindowText(m_strPrompt);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
