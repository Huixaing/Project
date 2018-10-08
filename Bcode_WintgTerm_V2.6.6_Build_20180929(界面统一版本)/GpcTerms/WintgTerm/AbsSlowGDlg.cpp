// AbsSlowGDlg.cpp : implementation file
//

#include "stdafx.h"
//#include "wintg.h"
#include "AbsSlowGDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAbsSlowGDlg dialog


CAbsSlowGDlg::CAbsSlowGDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAbsSlowGDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAbsSlowGDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CAbsSlowGDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAbsSlowGDlg)
	DDX_Control(pDX, IDC_ABSSLOW_SPEED, m_ctrlSpeed);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAbsSlowGDlg, CDialog)
	//{{AFX_MSG_MAP(CAbsSlowGDlg)
	ON_BN_CLICKED(IDOK, OnOk)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAbsSlowGDlg message handlers
_TCHAR *_gszSpeed[20] = 
{
	_T("15"),_T("20"),_T("25"),_T("30"),_T("35"),
	_T("40"),_T("45"),_T("50"),_T("55"),_T("60"),
	_T("65"),_T("70"),_T("75"),_T("80"),_T("85"),
	_T("90"),_T("95"),_T("100"),_T("105"),_T("110")
};
BOOL CAbsSlowGDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
  	CButton *pWnd;
 	pWnd = (CButton *)GetDlgItem(IDC_UP);
    pWnd->SetCheck(1);
	GetDlgItem(IDC_ABSSLOW_DISTANCE)->SetWindowText("1000");
	
	for(int i=0;i < sizeof(_gszSpeed)/sizeof(_gszSpeed[0]);i++)
		m_ctrlSpeed.AddString(_gszSpeed[i]);
	m_ctrlSpeed.SetCurSel(0);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CAbsSlowGDlg::OnOk() 
{
	CString text;
	GetDlgItem(IDC_ABSSLOW_DISTANCE)->GetWindowText(text);
	if(text.IsEmpty( ) || text.GetLength( ) == 0)
	{
//		ShowWarn("没有输入缓行距离");
		MessageBox("没有输入缓行距离");
		return;
	}
	m_nDistance = atoi((LPCTSTR)text);

	GetDlgItem(IDC_ABSSLOW_SPEED)->GetWindowText(text);
 	if(text.IsEmpty( ) || text.GetLength( ) == 0)
	{
//		ShowWarn("没有选择缓行速度");
        MessageBox("没有选择缓行速度");
	    return;
	}
	m_nSpeed = atoi((LPCTSTR)text);
 
  	CButton *pWnd;
 	pWnd = (CButton *)GetDlgItem(IDC_UP);

    if(pWnd->GetCheck())
	   m_nDirection = 0;
 	pWnd = (CButton *)GetDlgItem(IDC_DOWN);
    if(pWnd->GetCheck())
	   m_nDirection = 1;
 	pWnd = (CButton *)GetDlgItem(IDC_UPANDDOWN);
    if(pWnd->GetCheck())
	   m_nDirection = 2;

	CDialog::OnOK();
	
}
