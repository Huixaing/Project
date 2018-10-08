#if !defined(AFX_DLGELECARMSCONDIRM_H__C2824465_ED28_4899_A981_257ED0328A34__INCLUDED_)
#define AFX_DLGELECARMSCONDIRM_H__C2824465_ED28_4899_A981_257ED0328A34__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgElecArmsConfirm.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgElecArmsConfirm dialog

class CDlgElecArmsConfirm : public CDialog
{
// Construction
public:
	CDlgElecArmsConfirm(CString sMessage,CWnd* pParent = NULL);   // standard constructor
// Dialog Data
	//{{AFX_DATA(CDlgElecArmsConfirm)
	enum { IDD = IDD_DIG_ARMS_CONFIRM};
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgElecArmsConfirm)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CString	m_text;
	
	// Generated message map functions
	//{{AFX_MSG(CDlgElecArmsConfirm)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGELECARMSCONDIRM_H__C2824465_ED28_4899_A981_257ED0328A34__INCLUDED_)
