#if !defined(AFX_MYTREEVIEW_H__5E3C51E2_1E7D_4B83_B3DF_BEA5347573E6__INCLUDED_)
#define AFX_MYTREEVIEW_H__5E3C51E2_1E7D_4B83_B3DF_BEA5347573E6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MyTreeView.h : header file
//
#include "wintgdoc.h"
#include  "afxcview.h"

/////////////////////////////////////////////////////////////////////////////
// CMyTreeView view

class CMyTreeView : public CTreeView
{
protected:
	CMyTreeView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CMyTreeView)

// Attributes
public:
	CWintgDoc* GetDocument();

// Operations
public:
	HTREEITEM tSchedule, tHistory;
	HTREEITEM tUpdir, tDowndir;
	HTREEITEM tUpdirPassenger,tUpdirFreight,tDowndirPassenger,tDowndirFreight;
	HTREEITEM tPassenger, tFreight;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMyTreeView)
	public:
	virtual void OnInitialUpdate();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CMyTreeView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CMyTreeView)
	afx_msg void OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClick(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MYTREEVIEW_H__5E3C51E2_1E7D_4B83_B3DF_BEA5347573E6__INCLUDED_)
