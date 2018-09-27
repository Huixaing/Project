#if !defined(AFX_BLANKVIEW_H__9F9941BE_7326_43C5_83C7_D00202590B19__INCLUDED_)
#define AFX_BLANKVIEW_H__9F9941BE_7326_43C5_83C7_D00202590B19__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BlankView.h : header file
//
#define  TIMER_IDEVENT_SYSTEMTIME 201
/////////////////////////////////////////////////////////////////////////////
// CBlankView view

class CBlankView : public CView
{
protected:
	CBlankView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CBlankView)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBlankView)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
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
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CTime t_cur;
  
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BLANKVIEW_H__9F9941BE_7326_43C5_83C7_D00202590B19__INCLUDED_)
