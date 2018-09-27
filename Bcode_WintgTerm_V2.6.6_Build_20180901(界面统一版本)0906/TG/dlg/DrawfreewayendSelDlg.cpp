// DrawfreewayendSelDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DrawfreewayendSelDlg.h"
#include ".\drawfreewayendseldlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDrawfreewayendSelDlg dialog


CDrawfreewayendSelDlg::CDrawfreewayendSelDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDrawfreewayendSelDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDrawfreewayendSelDlg)
		// NOTE: the ClassWizard will add member initialization here
	nSel = -1;
	//}}AFX_DATA_INIT
}


void CDrawfreewayendSelDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDrawfreewayendSelDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDrawfreewayendSelDlg, CDialog)
	//{{AFX_MSG_MAP(CDrawfreewayendSelDlg)
	ON_BN_CLICKED(IDC_CONTINUE, OnContinue)
	ON_BN_CLICKED(IDC_STOP, OnStop)
	ON_BN_CLICKED(IDC_CANCEL, OnCancel)
	//}}AFX_MSG_MAP
	ON_WM_CLOSE()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDrawfreewayendSelDlg message handlers

//交出
void CDrawfreewayendSelDlg::OnContinue() 
{
   nSel =0;	
   CDialog::OnOK();
}
//终到
void CDrawfreewayendSelDlg::OnStop() 
{
    nSel =1;	
	CDialog::OnOK();
}
//取消
void CDrawfreewayendSelDlg::OnCancel() 
{
 	 nSel = 2;
	 CDialog::OnOK();
}
//返回
void CDrawfreewayendSelDlg::OnOK() 
{
}

BOOL CDrawfreewayendSelDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDrawfreewayendSelDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
}
