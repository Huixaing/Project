#if !defined(AFX_MYPREVIEWVIEW_H__9FCA8680_907C_11D3_8412_9D0C7145F5C4__INCLUDED_)
#define AFX_MYPREVIEWVIEW_H__9FCA8680_907C_11D3_8412_9D0C7145F5C4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MyPreviewView.h : header file
//
#include <afxpriv.h>

/////////////////////////////////////////////////////////////////////////////
// CMyPreviewView view

class CMyPreviewView : public CPreviewView
{
protected:
	CMyPreviewView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CMyPreviewView)

// Attributes
public:
	
// Operations
public:
	friend class CWrapperView;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMyPreviewView)
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual void OnEndPrintPreview(CDC* pDC, CPrintInfo* pInfo, POINT point, CPreviewView* pView);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CMyPreviewView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	virtual void OnActivateView(BOOL bActivate,
			CView* pActivateView, CView* pDeactiveView);

	// Generated message map functions
	//{{AFX_MSG(CMyPreviewView)
	afx_msg void OnPreviewClose();
	afx_msg void OnPreviewPrint();
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	friend BOOL CALLBACK _AfxMyPreviewCloseProc(CFrameWnd* pFrameWnd);

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MYPREVIEWVIEW_H__9FCA8680_907C_11D3_8412_9D0C7145F5C4__INCLUDED_)
