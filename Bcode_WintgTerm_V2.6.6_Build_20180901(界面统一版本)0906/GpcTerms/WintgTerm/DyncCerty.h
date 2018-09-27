#if !defined(AFX_DYNCCERTY_H__EA1EB20B_5E46_47CB_9A1A_B97B2C138E0B__INCLUDED_)
#define AFX_DYNCCERTY_H__EA1EB20B_5E46_47CB_9A1A_B97B2C138E0B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DyncCerty.h : header file
//
#include "resource.h"
/////////////////////////////////////////////////////////////////////////////
// CDyncCerty dialog

class CDyncCerty : public CDialog
{
// Construction
public:
	bool isOk;
	CString m_strPwd;
	CString m_strUser;
	CDyncCerty(CWnd* pParent = NULL);   // standard constructor
	~CDyncCerty();
// Dialog Data
	//{{AFX_DATA(CDyncCerty)
	enum { IDD = IDD_DYNC_CERFY };
	CEdit	m_pwd;
	CEdit	m_userid;
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDyncCerty)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDyncCerty)
	afx_msg void OnOk();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DYNCCERTY_H__EA1EB20B_5E46_47CB_9A1A_B97B2C138E0B__INCLUDED_)
