// AbsRouteDlg.cpp : implementation file
//

#include "stdafx.h"
#include "..\wintg\resource.h"
#include "AbsRouteDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAbsRouteDlg dialog


CAbsRouteDlg::CAbsRouteDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAbsRouteDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAbsRouteDlg)
	//}}AFX_DATA_INIT
}


void CAbsRouteDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAbsRouteDlg)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAbsRouteDlg, CDialog)
	//{{AFX_MSG_MAP(CAbsRouteDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAbsRouteDlg message handlers

BOOL CAbsRouteDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
  	CButton *pWnd;
 	pWnd = (CButton *)GetDlgItem(IDC_UP);
    pWnd->SetCheck(1);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CAbsRouteDlg::OnOK() 
{
	// TODO: Add extra validation here
  	CButton *pWnd;
 	pWnd = (CButton *)GetDlgItem(IDC_UP);

    if(pWnd->GetCheck())
	   nDirect = 0;
 	pWnd = (CButton *)GetDlgItem(IDC_DOWN);
    if(pWnd->GetCheck())
	   nDirect = 1;
 	pWnd = (CButton *)GetDlgItem(IDC_UPANDDOWN);
    if(pWnd->GetCheck())
	   nDirect = 2;

	CDialog::OnOK();
}
