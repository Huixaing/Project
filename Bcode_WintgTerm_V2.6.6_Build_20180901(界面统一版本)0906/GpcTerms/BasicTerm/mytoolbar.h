#if !defined(AFX_MYTOOLBAR_H__AE5E5AB3_8779_11D4_A762_006008389263__INCLUDED_)
#define AFX_MYTOOLBAR_H__AE5E5AB3_8779_11D4_A762_006008389263__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MyToolBar.h : header file
//

#include ".\Logo.h"

/////////////////////////////////////////////////////////////////////////////
// CMyToolBar window

class CMyToolBar : public CToolBar
{
// Construction
public:
	CMyToolBar();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMyToolBar)
	//}}AFX_VIRTUAL

// Implementation
public:
	CBrush m_Brush;
	CComboBox m_toolBarSecCombo;
  	virtual ~CMyToolBar();

	// Generated message map functions
protected:
	//{{AFX_MSG(CMyToolBar)
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
public:
	void OnSize(UINT nType, int cx, int cy);
	BOOL CreateLogo(CString fileName);
private:
	bool  bSize;
	CLogo m_cascoLogo;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MYTOOLBAR_H__AE5E5AB3_8779_11D4_A762_006008389263__INCLUDED_)
