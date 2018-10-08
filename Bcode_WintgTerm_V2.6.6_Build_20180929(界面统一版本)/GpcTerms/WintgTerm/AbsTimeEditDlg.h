#pragma once

#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
class CAbsTimeEditDlg : public CDialog
{
// Construction
public:
	CAbsTimeEditDlg(CWnd* pParent);   // standard constructor//modi by cf after 9-9
	~CAbsTimeEditDlg();

// Dialog Data
	//{{AFX_DATA(CAbsTimeEditDlg)
	enum { IDD = IDD_DLG_ABS_EDIT };
	//}}AFX_DATA

public:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAbsTimeEditDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CAbsTimeEditDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	int m_start_tm, m_stop_tm, m_run_tm;
};
