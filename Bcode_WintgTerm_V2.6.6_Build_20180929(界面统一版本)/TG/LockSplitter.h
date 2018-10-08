#if !defined(AFX_LOCKSPLITTER_H__8233A34E_7CBD_11D1_9CF4_000021000CE6__INCLUDED_)
#define AFX_LOCKSPLITTER_H__8233A34E_7CBD_11D1_9CF4_000021000CE6__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// LockSplitter.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CLockSplitter window
 
class  CLockSplitter : public CSplitterWnd
{
// Construction
public:
	CLockSplitter(BOOL bFlag = TRUE);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLockSplitter)
  	virtual void OnDrawSplitter( CDC* pDC, ESplitType nType, const CRect& rect );
	//}}AFX_VIRTUAL

// Implementation
public:
	void SetDrawFlag(int nDrawFlag);
	void SetLockFlag(BOOL bLockFlag);
	BOOL m_bLocked,m_bDrawFlag;
	virtual ~CLockSplitter();

	// Generated message map functions
protected:
	//{{AFX_MSG(CLockSplitter)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
 	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LOCKSPLITTER_H__8233A34E_7CBD_11D1_9CF4_000021000CE6__INCLUDED_)
