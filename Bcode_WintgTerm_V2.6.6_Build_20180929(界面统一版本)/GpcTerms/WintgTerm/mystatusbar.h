#if !defined(AFX_MYSTATUSBAR_H__CB361711_4C91_11D4_8757_006008389263__INCLUDED_)
#define AFX_MYSTATUSBAR_H__CB361711_4C91_11D4_8757_006008389263__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MyStatusBar.h : header file
//
/////////////////////////////////////////////////////////////////////////////
// CMyStatusBar window
extern CString strWintgVersion;
class  CMyStatusBar : public CStatusBar
{
// Construction
public:
	CMyStatusBar();

// Attributes
public:
 
// Operations
public:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMyStatusBar)
	//}}AFX_VIRTUAL

// Implementation

	struct
	{
		COLORREF m_cBackColor;
		COLORREF m_cFrontColor;
		BOOL     m_bBoldFont;
	}m_aPanelExtInfo[32];
	int m_nFlashFlag;
public:
	void SetOpeator();
	void _SetPaneText(UINT index,LPCTSTR text);//add by cf after 7-18
	void SetPanelColor(UINT index,COLORREF bc,COLORREF fc,BOOL bold=FALSE);
	void SetPanelBoldText(UINT index,BOOL bold);
	 
	virtual ~CMyStatusBar();
	// Generated message map functions
protected:
	//{{AFX_MSG(CMyStatusBar)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnDestroy();
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	//}}AFX_MSG

	virtual void DrawItem(LPDRAWITEMSTRUCT);
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MYSTATUSBAR_H__CB361711_4C91_11D4_8757_006008389263__INCLUDED_)
