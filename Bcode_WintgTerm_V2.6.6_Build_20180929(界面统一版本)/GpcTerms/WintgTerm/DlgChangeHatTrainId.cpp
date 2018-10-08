// DlgChangeHatTrainId.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "DlgChangeHatTrainId.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgChangeHatTrainId dialog


CDlgChangeHatTrainId::CDlgChangeHatTrainId(CString newid,CString hatid,CWnd* pParent /*=NULL*/)
	: CDialog(CDlgChangeHatTrainId::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgChangeHatTrainId)
	m_strNewTrainid = _T("");
	m_strHatid = _T("");
	//}}AFX_DATA_INIT

	m_strNewTrainid = newid;
	m_strHatid = hatid;
}


void CDlgChangeHatTrainId::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgChangeHatTrainId)
	DDX_Text(pDX, IDC_CHANGE_NEWID, m_strNewTrainid);
	DDV_MaxChars(pDX, m_strNewTrainid, 9);
	DDX_Text(pDX, IDC_CHANGE_HATID, m_strHatid);
	DDV_MaxChars(pDX, m_strHatid, 9);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgChangeHatTrainId, CDialog)
	//{{AFX_MSG_MAP(CDlgChangeHatTrainId)
	ON_BN_CLICKED(IDC_CHANGE_ACK, OnChangeAck)
	ON_BN_CLICKED(IDC_CHANGE_CANCEL, OnChangeCancel)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgChangeHatTrainId message handlers

void CDlgChangeHatTrainId::OnChangeAck() 
{
	// TODO: Add your control notification handler code here
	
	UpdateData(TRUE);
	CDialog::OnOK();
}

void CDlgChangeHatTrainId::OnChangeCancel() 
{
	// TODO: Add your control notification handler code here
	SendMessage(WM_CLOSE);
}


BOOL CDlgChangeHatTrainId::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	InitDlgPosition(this);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
