// MainFrm.h : interface of the CDlgMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__5C3F89A6_932E_11D3_8420_FC56F1C91E86__INCLUDED_)
#define AFX_MAINFRM_H__5C3F89A6_932E_11D3_8420_FC56F1C91E86__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "wrapperview.h"

class CDlgMainFrame : public CFrameWnd
{
	
public:		
	CDlgMainFrame(DRAWFUN pDraw,CWnd*pOldW,CWnd* pCallW,BOOL bDirect,LPCTSTR stTitle,CString & plotName,int screenNum);	
	CWnd*	pOldWnd;
	CWnd*	pCallWnd;
	DRAWFUN Draw;
	BOOL	bDirectPrint;
protected: 
	DECLARE_DYNAMIC(CDlgMainFrame)

// Attributes
public:
	int m_nScreenNum;

	CString m_sPlotName;
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgMainFrame)
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CDlgMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;
	//CMyPreviewView *m_pView;
	//CChildView  *m_pView;
	CWrapperView  *m_pView;

// Generated message map functions
protected:
	//{{AFX_MSG(CDlgMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);	
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__5C3F89A6_932E_11D3_8420_FC56F1C91E86__INCLUDED_)
