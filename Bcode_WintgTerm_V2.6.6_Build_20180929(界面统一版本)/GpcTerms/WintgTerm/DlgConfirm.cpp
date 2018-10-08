
#include "stdafx.h"
#include "resource.h"
#include "DlgConfirm.h"

CDlgConfirm::CDlgConfirm(const CString& tipText,const CStringArray& stringArray, CWnd* pParent)
	: CDialog(CDlgConfirm::IDD, pParent)
{
	m_tipText=tipText;
	m_stringArray.RemoveAll();
	for(int i=0; i<stringArray.GetCount(); i++)
	{
		CString str=stringArray.GetAt(i);
		if(str!="")
			m_stringArray.Add(str);
	}
}

CDlgConfirm::~CDlgConfirm()
{
}

void CDlgConfirm::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_listBox);
}


BEGIN_MESSAGE_MAP(CDlgConfirm, CDialog)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
BOOL CDlgConfirm::OnInitDialog() 
{
	CDialog::OnInitDialog();
	CWnd* pWnd=this->GetDlgItem(IDC_STATIC1);
	if(pWnd)
	{
		pWnd->SetWindowText(m_tipText);
	}
	for(int i=0; i<m_stringArray.GetCount(); i++)
	{
		CString str=m_stringArray.GetAt(i);
		m_listBox.AddString(str);
	}
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CDlgConfirm::OnOK() 
{
	GpcLogTrainMessage(0, 0, "调度员确认如下内容");
	GpcLogTrainMessage(0, 0, "%s", m_tipText);
	for(int i=0; i<m_stringArray.GetCount(); i++)
	{
		CString str=m_stringArray.GetAt(i);
		GpcLogTrainMessage(0, 0, "%s", str);
	}
	CDialog::OnOK();
}

