#if !defined(AFX_CHANGEPASSWORDDLG_H__95CE9C5A_1528_4AED_8BC8_BA7AAF4BAFF6__INCLUDED_)
#define AFX_CHANGEPASSWORDDLG_H__95CE9C5A_1528_4AED_8BC8_BA7AAF4BAFF6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ChangePasswordDlg.h : header file
//
#include "resource.h"
#include "Tooltipwnd.h"

/////////////////////////////////////////////////////////////////////////////
// CChangePasswordDlg dialog

class CChangePasswordDlg : public CDialog
{
// Construction
public:
	CChangePasswordDlg(CWnd* pParent = NULL);   // standard constructor
    //CToolTipWnd m_ctrlTooltipWnd;
// Dialog Data
	//{{AFX_DATA(CChangePasswordDlg)
	enum { IDD = IDD_CHANGE_PASSWORD };
	CEdit	m_newCtrl2;
	CEdit	m_newCtrl;
	CEdit	m_oldCtrl;
	CEdit	m_useridCtrl;
	CEdit	m_userNameCtrl;
	CProgressCtrl m_prog;
	CString	m_userid;
	CString m_username;
	CString	m_old_password;
	CString	m_new_password;
	CString	m_new_password2;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChangePasswordDlg)
	public:
	
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void Send_ChangePassword();
	afx_msg void OnClose();
	// Generated message map functions
	//{{AFX_MSG(CChangePasswordDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnCancel();
	afx_msg void OnOk();
	void OnTimer(UINT nIDEvent);
	LRESULT OnMsgGpcUserManager(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	void SendMsgUserManagerQuery();
	CString m_strCtcPwd;
	UINT mtimes;
	bool bSuccess;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHANGEPASSWORDDLG_H__95CE9C5A_1528_4AED_8BC8_BA7AAF4BAFF6__INCLUDED_)
