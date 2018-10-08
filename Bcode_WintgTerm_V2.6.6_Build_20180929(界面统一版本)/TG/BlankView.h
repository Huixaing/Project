#if !defined(AFX_BLANKVIEW_H__9F9941BE_7326_43C5_83C7_D00202590B19__INCLUDED_)
#define AFX_BLANKVIEW_H__9F9941BE_7326_43C5_83C7_D00202590B19__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BlankView.h : header file
//
#define  TIMER_IDEVENT_SYSTEMTIME 201
#include "tgdoc.h"
/////////////////////////////////////////////////////////////////////////////
// CBlankView view

class CBlankView : public CView
{
protected:
	CBlankView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CBlankView)

// Attributes
public:
	CTGDoc *m_pDoc;
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBlankView)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CBlankView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CBlankView)
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CFont m_fontTime;
	CFont m_fontDate;
	
	CBitmap * m_pOldBitmap;
	CBitmap m_bmpMemBitmap;
	CDC     m_dcMemoryDC;

public:
	afx_msg void OnDestroy();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	void SetTimeString();

public:
	void SetMyCtrl(CTGCtrl* ctrl) { m_pctrl = ctrl;}
private:
	CTGCtrl* m_pctrl;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BLANKVIEW_H__9F9941BE_7326_43C5_83C7_D00202590B19__INCLUDED_)
