#if !defined(AFX_USERPROPERTYDLG_H__402E11E9_6247_4155_A666_B1AC9B4FED61__INCLUDED_)
#define AFX_USERPROPERTYDLG_H__402E11E9_6247_4155_A666_B1AC9B4FED61__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// UserPropertyDlg.h : header file
//

#include "resource.h"
#include "tgmsg.h"
/////////////////////////////////////////////////////////////////////////////
// CUserPropertyDlg dialog

class CUserPropertyDlg : public CDialog
{
// Construction
public:
	CUserPropertyDlg(bool insert = true, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CUserPropertyDlg)
	enum { IDD = IDD_USER_PROPERTY };
	CStatic	m_show_pass;
	CStatic	m_btn_show_pass;
	CString	m_passwd;
	CString	m_re_passwd;
	int		m_propperty;
	CString	m_user_id;
	CString	m_user_name;
	
	//}}AFX_DATA
	bool m_insert;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUserPropertyDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CUserPropertyDlg)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_USERPROPERTYDLG_H__402E11E9_6247_4155_A666_B1AC9B4FED61__INCLUDED_)
