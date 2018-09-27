// DlgMessageBoxPassword.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "DlgMessageBox.h"
#include "SuperPassDlg.h"
#include "DlgMessageBoxPassword.h"
#include ".\DlgMessageBoxPassword.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgMessageBoxPassword dialog
CDlgMessageBoxPassword::CDlgMessageBoxPassword(CStringArray& alarm, CWnd* pParent)
	: CDialog(CDlgMessageBoxPassword::IDD, pParent)
{
	m_alarm.RemoveAll();
	for(int i=0; i<alarm.GetCount(); i++)
	{
		m_alarm.Add(alarm.GetAt(i));
	}
}

void CDlgMessageBoxPassword::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_list);
}

BEGIN_MESSAGE_MAP(CDlgMessageBoxPassword, CDialog)
	ON_WM_CLOSE()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgMessageBoxPassword message handlers
BOOL CDlgMessageBoxPassword::OnInitDialog() 
{
	CDialog::OnInitDialog();
	m_list.ResetContent();
	for(int i=0; i<m_alarm.GetCount(); i++)
	{
		m_list.AddString(m_alarm.GetAt(i));
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgMessageBoxPassword::OnClose()
{
	// ´Ëº¯ÊýÆÁ±ÎÓÃ»§²æµô¹Ø±Õ¶Ô»°¿ò
}

void CDlgMessageBoxPassword::OnOK() 
{
	CSuperPassDlg dlg;
	if (dlg.DoModal() == IDOK)
	{
		if (dlg.m_super_pass.Compare("1234") != 0)
		{
			CDlgMessageBox dlg("ÃÜÂë´íÎó!");
			dlg.DoModal();
		}
		else
		{
			CDialog::OnOK();
		}

	}
}
