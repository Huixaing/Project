#if !defined(AFX_WRAPPERVIEW_H__AC579D20_8E3D_11D3_8410_E1CD9A2C01DC__INCLUDED_)
#define AFX_WRAPPERVIEW_H__AC579D20_8E3D_11D3_8410_E1CD9A2C01DC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WrapperView.h : header file
//

#include "MyPreviewView.h"




/////////////////////////////////////////////////////////////////////////////
// CWrapperView view

class CWrapperView : public CScrollView
{
protected:
	DECLARE_DYNCREATE(CWrapperView)
	
	CFrameWnd *m_pFrameWnd;

// Attributes
public:

	CString m_sPlotName;
// Operations
public:
	CWrapperView();
	virtual ~CWrapperView();

	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

	void OnFilePrintPreview(CFrameWnd *pFrame);
	
	BOOL DoPrintPreview(UINT nIDResource, CView* pPrintView,
		CRuntimeClass* pPreviewViewClass, CPrintPreviewState* pState);

	void OnEndPrintPreview(CDC* pDC, CPrintInfo* pInfo,
			POINT, CMyPreviewView* pView);
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWrapperView)
	protected:
	virtual void OnPrint(CDC* pDC,CPrintInfo* pInfo);      // overridden to draw this view
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
protected:
	
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CWrapperView)
		// NOTE - the ClassWizard will add and remove member functions here.
	afx_msg void OnFilePrint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WRAPPERVIEW_H__AC579D20_8E3D_11D3_8410_E1CD9A2C01DC__INCLUDED_)
