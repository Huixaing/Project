// DlgMessageBox.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "DlgMessageBox.h"
#include ".\dlgmessagebox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgMessageBox dialog


CDlgMessageBox::CDlgMessageBox(CString sMessage,CWnd* pParent)
	: CDialog(CDlgMessageBox::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgMessageBox)
	m_text = _T("");
	//}}AFX_DATA_INIT

	m_text = sMessage;
}


void CDlgMessageBox::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgMessageBox)
	DDX_Text(pDX, IDC_TEXT, m_text);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgMessageBox, CDialog)
	//{{AFX_MSG_MAP(CDlgMessageBox)
	//}}AFX_MSG_MAP
	ON_WM_CLOSE()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgMessageBox message handlers

BOOL CDlgMessageBox::OnInitDialog() 
{
	CDialog::OnInitDialog();

    UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

