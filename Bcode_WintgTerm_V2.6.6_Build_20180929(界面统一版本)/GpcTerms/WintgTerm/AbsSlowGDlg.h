#if !defined(AFX_ABSSLOWGDLG_H__9B262253_9812_453D_B7A2_DFFF93002C38__INCLUDED_)
#define AFX_ABSSLOWGDLG_H__9B262253_9812_453D_B7A2_DFFF93002C38__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AbsSlowGDlg.h : header file
//
#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// CAbsSlowGDlg dialog

class CAbsSlowGDlg : public CDialog
{
// Construction
public:
	int m_nDistance,m_nSpeed;
	int m_nDirection;
	CAbsSlowGDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAbsSlowGDlg)
	enum { IDD = IDD_ABS_SLOW_G };
	CComboBox	m_ctrlSpeed;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAbsSlowGDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAbsSlowGDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnOk();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ABSSLOWGDLG_H__9B262253_9812_453D_B7A2_DFFF93002C38__INCLUDED_)
