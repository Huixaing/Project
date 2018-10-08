// DlgTCCSelect.cpp : implementation file
//

#include "stdafx.h"
#include "DlgTCCSelect.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgTCCSelect dialog
CDlgTCCSelect::CDlgTCCSelect(CWnd* pParent)
: CDialog(CDlgTCCSelect::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgTCCSelect)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_nSelSchdType=-1;
}
CDlgTCCSelect::~CDlgTCCSelect()
{
}
void CDlgTCCSelect::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgTCCSelect)
	DDX_Control(pDX, IDC_COMBO1, m_combo_sel);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlgTCCSelect, CDialog)
	//{{AFX_MSG_MAP(CDlgTCCSelect)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, OnOK)
	ON_CBN_SELCHANGE(IDC_COMBO1, OnSelBackupBasicSchd)
END_MESSAGE_MAP()

BOOL CDlgTCCSelect::OnInitDialog() 
{
	CDialog::OnInitDialog();
	m_nSelSchdType=-1;
		
	int index=m_combo_sel.AddString("既有列控");
	m_combo_sel.SetItemData(index, 2);

	index=m_combo_sel.AddString("客专列控");
	m_combo_sel.SetItemData(index, 3);

	// TODO: Add extra initialization here
	InitDlgPosition(this);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgTCCSelect::OnSelBackupBasicSchd()
{
	int nSel = m_combo_sel.GetCurSel();
	if(nSel>=0)
	{
		m_nSelSchdType = m_combo_sel.GetItemData(nSel);
	}
	else
	{
		m_nSelSchdType=-1;
	}
}

/////////////////////////////////////////////////////////////////////////////
void CDlgTCCSelect::OnOK() 
{
	if(m_nSelSchdType!=2 && m_nSelSchdType!=3)
	{
		AfxMessageBox("请选择列控类型！！");
		return;
	}

	CDialog::OnOK();
}

