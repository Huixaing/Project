// InsertSimpleDlg.cpp : implementation file
//

#include "stdafx.h"
#include "InsertSimpleDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CInsertSimpleDlg dialog


CInsertSimpleDlg::CInsertSimpleDlg(LPCTSTR title,CWintgDoc *pDoc /*=NULL*/,CWnd* pParent /*=NULL*/)
	: CDialog(CInsertSimpleDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CInsertSimpleDlg)
	m_sStaionid = _T("");
	
	//}}AFX_DATA_INIT
	m_pDoc  = pDoc;
	this->title = title;
}


void CInsertSimpleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInsertSimpleDlg)
	DDX_Text(pDX, IDC_EDIT1, m_sStaionid);
	DDV_MaxChars(pDX, m_sStaionid,  9);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CInsertSimpleDlg, CDialog)
	//{{AFX_MSG_MAP(CInsertSimpleDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInsertSimpleDlg message handlers

BOOL CInsertSimpleDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	SetWindowText(title);
	InitDlgPosition(this);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
void CInsertSimpleDlg::OnOK() 
{
	UpdateData(TRUE);

	if( m_sStaionid.Find(" ")>=0)
	{
		MessageBox("车次中有空格,请新输入!");
		return  ;
	}
	UpdateData(FALSE);
	if(m_sStaionid.GetLength( ) == 0){
		AfxMessageBox("请输入车次号");	
	    return;
	}

	CDialog::OnOK();

}
