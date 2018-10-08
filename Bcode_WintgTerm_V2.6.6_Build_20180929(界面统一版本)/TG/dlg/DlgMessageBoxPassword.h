
#pragma once

#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgMessageBoxPassword dialog

class CDlgMessageBoxPassword : public CDialog
{
public:
	CDlgMessageBoxPassword(CStringArray& alarm,CWnd* pParent=NULL);   // standard constructor

	enum { IDD = IDD_DIALOG_WITH_PASSWORD};

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

protected:
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
	virtual void OnOK();
	DECLARE_MESSAGE_MAP()
	CListBox m_list;
private:
	CStringArray m_alarm;
};


