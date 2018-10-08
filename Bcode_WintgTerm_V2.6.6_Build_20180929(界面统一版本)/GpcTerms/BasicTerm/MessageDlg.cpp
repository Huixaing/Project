// MessageDlg.cpp : implementation file
//

#include "stdafx.h"
//#include "wintg.h"
#include "MessageDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define TIMER_CANCEL_DLG 200
#define TIMER_TIMEOUT_VAL 2

/////////////////////////////////////////////////////////////////////////////
// CMessageDlg dialog


CMessageDlg::CMessageDlg(CWnd* pParent /*=NULL*/,CString str)
	: CDialog(CMessageDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMessageDlg)
	m_strTime = _T("");
	//}}AFX_DATA_INIT
	msg = str;
	//memcpy(&m_item,pSend,sizeof(struct DispatchCommandSend));  
	//m_item = *pSend ;  //20070327
}

void CMessageDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMessageDlg)
	DDX_Control(pDX, IDC_STATIC_MESSAGE, m_static);
	DDX_Text(pDX, IDC_STATIC_TIME, m_strTime);
	DDV_MaxChars(pDX, m_strTime, 2);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMessageDlg, CDialog)
	//{{AFX_MSG_MAP(CMessageDlg)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMessageDlg message handlers

BOOL CMessageDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	GetDlgItem(IDC_STATIC_MESSAGE)->SetWindowText(msg);
	SetTimer(TIMER_CANCEL_DLG,500,NULL);
	m_valTimeout = TIMER_TIMEOUT_VAL;
	m_strTime.Format("%d",m_valTimeout);
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CMessageDlg::OnOK() 
{	
 
	DestroyWindow();
}

void CMessageDlg::PostNcDestroy() 
{
	delete this;
}

void CMessageDlg::OnCancel() 
{
	DestroyWindow();
}

void CMessageDlg::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	if(nIDEvent == TIMER_CANCEL_DLG)
	{
		m_valTimeout--;
		m_strTime.Format("%d",m_valTimeout);
		UpdateData(FALSE);
		if(m_valTimeout <= 0)
		{
			OnCancel();
			return;
		}

	}
	CDialog::OnTimer(nIDEvent);
}
