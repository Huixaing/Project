// DlgWait.cpp : implementation file
//

#include "stdafx.h"

#include "DlgWait.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgWait dialog


CDlgWait::CDlgWait(const char *str,CWnd* pParent /*=NULL*/)
	: CDialog(CDlgWait::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgWait)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	Create(CDlgWait::IDD, pParent);				
	GetDlgItem(IDC_WAIT_STR)->SetWindowText(str);
	CRect  rect_scr,rect_w;
	GetDesktopWindow()->GetClientRect(&rect_scr); 
	GetWindowRect(&rect_w);
	m_oriRect = rect_w;
	//MoveWindow(0,0,rect_w.Width(),55);
	GetWindowRect(&rect_w);
	SetWindowPos(&wndTopMost,rect_scr.Width()/2-rect_w.Width()/2,
		       rect_scr.Height()/2-rect_w.Height()/2,
			   rect_w.Width(),57,0);  
	ShowWindow(SW_SHOW);
	UpdateWindow();
	
}

CDlgWait::~CDlgWait()
{
	DestroyWindow();
}

void CDlgWait::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgWait)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgWait, CDialog)
	//{{AFX_MSG_MAP(CDlgWait)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgWait message handlers
CString CDlgWait::GetDisplayStr()
{
	CString str;
	GetDlgItem(IDC_WAIT_STR)->GetWindowText(str);
	return str;
}

void  CDlgWait::UpdateDisplay(const char *str, const char *str2)
{
	if (str2!=NULL)
	{
		CRect  rect_scr;
		GetDesktopWindow()->GetClientRect(&rect_scr); 
		//MoveWindow(0,0,m_oriRect.Width(),m_oriRect.Height());
		SetWindowPos(&wndTopMost,rect_scr.Width()/2-m_oriRect.Width()/2,
			rect_scr.Height()/2-m_oriRect.Height()/2,
			m_oriRect.Width(), m_oriRect.Height(),0);  
		
		GetDlgItem(IDC_EDIT1)->SetWindowText(str2);
	}
	else
	{
		//MoveWindow(0,0,m_oriRect.Width(),55);
		CRect  rect_scr;
		GetDesktopWindow()->GetClientRect(&rect_scr); 
		//MoveWindow(0,0,m_oriRect.Width(),m_oriRect.Height());
		SetWindowPos(&wndTopMost,rect_scr.Width()/2-m_oriRect.Width()/2,
			rect_scr.Height()/2-m_oriRect.Height()/2,
			m_oriRect.Width(),57,0);  
		GetDlgItem(IDC_EDIT1)->SetWindowText("");
	}

	GetDlgItem(IDC_WAIT_STR)->SetWindowText(str);
	UpdateWindow();
}
