#if !defined(AFX_MESSAGEDLG_H__64B8DBB7_F99A_49A8_A48E_A31BCB6676B4__INCLUDED_)
#define AFX_MESSAGEDLG_H__64B8DBB7_F99A_49A8_A48E_A31BCB6676B4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MessageDlg.h : header file
//
#include "resource.h"
//#include "NewDispatcherCommand.h"
/////////////////////////////////////////////////////////////////////////////
// CMessageDlg dialog

class CMessageDlg : public CDialog
{
// Construction
public:
	CMessageDlg(CWnd* pParent = NULL,CString = "");   // standard constructor
	CString msg;
// Dialog Data
	//{{AFX_DATA(CMessageDlg)
	enum { IDD = IDD_DIALOG_MESSAGE };
	CStatic	m_static;
	CString	m_strTime;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMessageDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:
	int m_valTimeout;
	// Generated message map functions
	//{{AFX_MSG(CMessageDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MESSAGEDLG_H__64B8DBB7_F99A_49A8_A48E_A31BCB6676B4__INCLUDED_)
