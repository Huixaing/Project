#if !defined(AFX_BLANK2VIEW_H__581CBF58_92BD_480C_B864_0B9FC43F24D2__INCLUDED_)
#define AFX_BLANK2VIEW_H__581CBF58_92BD_480C_B864_0B9FC43F24D2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Blank2View.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CBlank2View view

class CBlank2View : public CView
{
protected:
	CBlank2View();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CBlank2View)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBlank2View)
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CBlank2View();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CBlank2View)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BLANK2VIEW_H__581CBF58_92BD_480C_B864_0B9FC43F24D2__INCLUDED_)
