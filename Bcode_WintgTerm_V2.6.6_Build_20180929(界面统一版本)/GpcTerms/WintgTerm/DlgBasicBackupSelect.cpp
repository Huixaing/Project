// DlgBasicBackupSelect.cpp : implementation file
//

#include "stdafx.h"
#include "tg.h"
#include "tgdocument.h"
#include "TgEditView.h"
#include "DlgBasicBackupSelect.h"
#include "SuperPassDlg.h"
#include "DlgMessageBox.h"
#include "tgmsg.h"
#include "sysshare.h"
#include <baseMsg\msgid.h>
#include <BaseMsg/clbth.h>
#include ".\dlgbasicparam.h"
#include "basic_schd_config.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern basic_schd_config* GetBasicConfig();

/////////////////////////////////////////////////////////////////////////////
// CDlgBasicBackupSelect dialog
CDlgBasicBackupSelect::CDlgBasicBackupSelect(int con_id, CWnd* pParent)
: CDialog(CDlgBasicBackupSelect::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgBasicBackupSelect)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_con_id = con_id;
	m_nSelSchdType=-1;
}
CDlgBasicBackupSelect::~CDlgBasicBackupSelect()
{
}
void CDlgBasicBackupSelect::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgBasicBackupSelect)
	DDX_Control(pDX, IDC_COMBO1, m_combo_backup);
	DDX_Control(pDX, IDC_EDIT3, sttm);
	DDX_Control(pDX, IDC_EDIT2, edtm);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlgBasicBackupSelect, CDialog)
	//{{AFX_MSG_MAP(CDlgBasicBackupSelect)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, OnOK)
	ON_CBN_SELCHANGE(IDC_COMBO1, OnSelBackupBasicSchd)
END_MESSAGE_MAP()

BOOL CDlgBasicBackupSelect::OnInitDialog() 
{
	CDialog::OnInitDialog();
	m_nSelSchdType=-1;
	m_combo_backup.ResetContent();
	SECTION_BASIC_NAME* pBasic = GetBasicConfig()->GetBasicSchdNameStruct(m_con_id);
	if(pBasic)
	{
		for(int i=0; i<pBasic->num; i++)
		{
			CString text = pBasic->basic_backup_schd[i].name;
			int     type = pBasic->basic_backup_schd[i].type;
			int index=m_combo_backup.AddString(text);
			m_combo_backup.SetItemData(index, type);
		}
	}
	
	sttm.SetWindowText("");
	edtm.SetWindowText("");
	// TODO: Add extra initialization here
	InitDlgPosition(this);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgBasicBackupSelect::OnSelBackupBasicSchd()
{
	int nSel = m_combo_backup.GetCurSel();
	if(nSel>=0)
	{
		m_nSelSchdType = m_combo_backup.GetItemData(nSel);
		SECTION_BASIC_NAME* pBasic = GetBasicConfig()->GetBasicSchdNameStruct(m_con_id);
		if(pBasic)
		{
			for(int i=0; i<pBasic->num; i++)
			{
				int type = pBasic->basic_backup_schd[i].type;
				if(m_nSelSchdType == type)
				{
					CTime tm(pBasic->basic_backup_schd[i].starttm),tm1(pBasic->basic_backup_schd[i].endtm);
					sttm.SetWindowText(tm.Format("%Y-%m-%d %H:%M"));
					edtm.SetWindowText(tm1.Format("%Y-%m-%d %H:%M"));
				}
			}
		}
	}
	else
	{
		m_nSelSchdType=-1;
	}
}

/////////////////////////////////////////////////////////////////////////////
void CDlgBasicBackupSelect::OnOK() 
{
	if(m_nSelSchdType < 21 || m_nSelSchdType>32)
	{
		AfxMessageBox("ÇëÑ¡Ôñ¹éµµ»ù±¾Í¼£¡£¡");
		return;
	}

	CDialog::OnOK();
}

