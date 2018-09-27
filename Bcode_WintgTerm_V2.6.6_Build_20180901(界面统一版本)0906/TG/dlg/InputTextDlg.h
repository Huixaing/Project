
#pragma once
//

#include "resource.h"
#include "afxwin.h"

/////////////////////////////////////////////////////////////////////////////
// CInputTextDlg dialog

class CInputTextDlg : public CDialog
{
// Construction
public:
	CInputTextDlg(BYTE flag, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CInputTextDlg)
	enum { IDD = IDD_DLG_INPUT };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInputTextDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
    BYTE m_flag;
	// Generated message map functions
	//{{AFX_MSG(CInputTextDlg)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	CString inputtext;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

