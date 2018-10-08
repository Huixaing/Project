// DlgElecArmsConfirm.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "DlgElecArmsConfirm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgElecArmsConfirm dialog

CDlgElecArmsConfirm::CDlgElecArmsConfirm(CString sMessage,CWnd* pParent /*=NULL*/)
	: CDialog(CDlgElecArmsConfirm::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgElecArmsConfirm)
	//}}AFX_DATA_INIT

	m_text = sMessage;
}

void CDlgElecArmsConfirm::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgElecArmsConfirm)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlgElecArmsConfirm, CDialog)
	//{{AFX_MSG_MAP(CDlgElecArmsConfirm)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgElecArmsConfirm message handlers

BOOL CDlgElecArmsConfirm::OnInitDialog() 
{
	CDialog::OnInitDialog();

    UpdateData(FALSE);
	InitDlgPosition(this);

	CWnd* pWnd = GetDlgItem(IDC_STATIC_TEXT);
	if(NULL != pWnd)
	{
		pWnd->SetWindowText(m_text);
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

