#if !defined(AFX_CANCELWORK_H__20722814_2E47_4240_B335_90B796932D61__INCLUDED_)
#define AFX_CANCELWORK_H__20722814_2E47_4240_B335_90B796932D61__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CancelWorkDlg.h : header file
//
#include "resource.h"


/////////////////////////////////////////////////////////////////////////////
// CCancelWorkDlg dialog

class CCancelWorkDlg : public CDialog
{
// Construction
public:
	CCancelWorkDlg(CWnd* pParent);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CCancelWorkDlg)
	enum { IDD = IDD_DLG_CANCEL_WORK };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCancelWorkDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CCancelWorkDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	CString m_strPlanID;
	BYTE    m_nCancelCode;
	CString	m_strCancelReason;
	CString	m_strText;

public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CANCELWORK_H__20722814_2E47_4240_B335_90B796932D61__INCLUDED_)
