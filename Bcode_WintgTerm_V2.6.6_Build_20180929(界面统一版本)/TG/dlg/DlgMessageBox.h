#if !defined(AFX_DLGMESSAGEBOX_H__C2824465_ED28_4899_A981_257ED0328A34__INCLUDED_)
#define AFX_DLGMESSAGEBOX_H__C2824465_ED28_4899_A981_257ED0328A34__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgMessageBox.h : header file
//

#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgMessageBox dialog

class CDlgMessageBox : public CDialog
{
// Construction
public:
	CDlgMessageBox(CString sMessage,CWnd* pParent=NULL);   // standard constructor
	CBitmap mBmp;
// Dialog Data
	//{{AFX_DATA(CDlgMessageBox)
	enum { IDD = IDD_DLG_MESSAGEBOX};
	CString	m_text;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgMessageBox)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgMessageBox)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGMESSAGEBOX_H__C2824465_ED28_4899_A981_257ED0328A34__INCLUDED_)
