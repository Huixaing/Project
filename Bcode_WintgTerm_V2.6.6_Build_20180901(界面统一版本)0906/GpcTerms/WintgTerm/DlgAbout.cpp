// DlgAbout.cpp : implementation file
//

#include "stdafx.h"
#include "DlgAbout.h"
#include "MyStatusBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgAbout dialog
CDlgAbout::CDlgAbout(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgAbout::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgAbout)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDlgAbout::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgAbout)
	DDX_Control(pDX, IDC_STATIC_MY_LOGO, pic);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgAbout, CDialog)
	//{{AFX_MSG_MAP(CDlgAbout)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgAbout message handlers
#define CLR_PALEGRAY RGB(192,192,192)
BOOL CDlgAbout::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CWnd* pWnd = GetDlgItem(IDC_STATIC_MY_LOGO);
	CRect rect;
	pWnd->GetClientRect(&rect);
	pic.SetPicSize(rect, IDB_BITMAP_ABOUT,true);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
