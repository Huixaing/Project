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


CDlgWait::CDlgWait(const char *str, CWnd* pParent)
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
	MoveWindow(rect_scr.Width()/2-rect_w.Width()/2,
		       rect_scr.Height()/2-rect_w.Height()/2,
			   rect_w.Width(), rect_w.Height(),FALSE);  
	ShowWindow(SW_SHOW);
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
void CDlgWait::SetWaitText(CString str)
{
	GetDlgItem(IDC_WAIT_STR)->SetWindowText(str);
}