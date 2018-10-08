#if !defined(AFX_SUPERPASSDLG_H__EAD49E1A_D522_4A73_B942_10276DCCFBF3__INCLUDED_)
#define AFX_SUPERPASSDLG_H__EAD49E1A_D522_4A73_B942_10276DCCFBF3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SuperPassDlg.h : header file
//

#include "resource.h"
/////////////////////////////////////////////////////////////////////////////
// CSuperPassDlg dialog

class CSuperPassDlg : public CDialog
{
// Construction
public:
	CSuperPassDlg(CString title = "",CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSuperPassDlg)
	enum { IDD = IDD_SUPER_PASS };
	CString	m_super_pass;
	//}}AFX_DATA
    CString m_strTitle;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSuperPassDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSuperPassDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	CString m_strPrompt;

public:
	bool PasswordIsOk();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SUPERPASSDLG_H__EAD49E1A_D522_4A73_B942_10276DCCFBF3__INCLUDED_)
