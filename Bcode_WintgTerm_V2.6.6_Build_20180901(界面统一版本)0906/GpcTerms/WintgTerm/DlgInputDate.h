#if !defined(AFX_DLGINPUTDATE_H__51413626_CBA6_4ABA_8615_2ACF20EF9EBF__INCLUDED_)
#define AFX_DLGINPUTDATE_H__51413626_CBA6_4ABA_8615_2ACF20EF9EBF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgInputDate.h : header file
//
#include "resource.h"
/////////////////////////////////////////////////////////////////////////////
// CDlgInputDate dialog

class CDlgInputDate : public CDialog
{
// Construction
public:
	CDlgInputDate(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgInputDate)
	enum { IDD = IDD_DLG_INPUTDATE };
	CTime	m_inputDate;
	UINT	m_inputBanbie;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgInputDate)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgInputDate)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGINPUTDATE_H__51413626_CBA6_4ABA_8615_2ACF20EF9EBF__INCLUDED_)
