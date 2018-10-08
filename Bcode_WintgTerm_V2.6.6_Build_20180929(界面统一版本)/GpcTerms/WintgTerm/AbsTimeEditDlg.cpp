// CAbsTimeEditDlg.cpp : implementation file
//
#include "stdafx.h"

#include "AbsTimeEditDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAbsTimeEditDlg dialog


CAbsTimeEditDlg::CAbsTimeEditDlg(CWnd* pParent /*=NULL*/) 
: CDialog(CAbsTimeEditDlg::IDD, pParent)
{
	m_start_tm=0;
	m_stop_tm=0;
	m_run_tm=0;
}

CAbsTimeEditDlg::~CAbsTimeEditDlg()
{
}


void CAbsTimeEditDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAbsTimeEditDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAbsTimeEditDlg, CDialog)
	//{{AFX_MSG_MAP(CAbsTimeEditDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAbsTimeEditDlg message handlers
BOOL CAbsTimeEditDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	CString text;
	text.Format("%d", m_start_tm);
	CWnd* pWnd = GetDlgItem(IDC_EDIT1);
	if(pWnd)
	{
		pWnd->SetWindowText(text);
	}

	text.Format("%d", m_stop_tm);
	pWnd = GetDlgItem(IDC_EDIT2);
	if(pWnd)
	{
		pWnd->SetWindowText(text);
	}

	text.Format("%d", m_run_tm);
	pWnd = GetDlgItem(IDC_EDIT3);
	if(pWnd)
	{
		pWnd->SetWindowText(text);
	}
	InitDlgPosition(this);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CAbsTimeEditDlg::OnOK() 
{
	CString text;
	CWnd* pWnd = GetDlgItem(IDC_EDIT1);
	if(pWnd)
	{
		pWnd->GetWindowText(text);
		m_start_tm=atoi(text);
	}

	pWnd = GetDlgItem(IDC_EDIT2);
	if(pWnd)
	{
		pWnd->GetWindowText(text);
		m_stop_tm=atoi(text);
	}

	pWnd = GetDlgItem(IDC_EDIT3);
	if(pWnd)
	{
		pWnd->GetWindowText(text);
		m_run_tm=atoi(text);
	}

	if(m_run_tm <= 0)
	{
		AfxMessageBox("区间运行时分不能小于等于0");
	}
	else
	{
		CDialog::OnOK();
	}
}