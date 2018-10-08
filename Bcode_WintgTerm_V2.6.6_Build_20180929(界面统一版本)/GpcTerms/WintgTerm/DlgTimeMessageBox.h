
#pragma once
#include "resource.h"
// DlgTimeMessageBox.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgTimeMessageBox dialog

class CDlgTimeMessageBox : public CDialog
{
// Construction
public:
	CDlgTimeMessageBox(CString sMessage, int nTimeSecond, int x, int y, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgTimeMessageBox)
	enum { IDD = IDD_DLG_TIME_MESSAGE};
	CString	m_text;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgTimeMessageBox)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgTimeMessageBox)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	virtual void PostNcDestroy();
	int m_nTimeSecond;
public:
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnClose();

	int mx,my;
	static BYTE WindowPosition[15][3];
};




