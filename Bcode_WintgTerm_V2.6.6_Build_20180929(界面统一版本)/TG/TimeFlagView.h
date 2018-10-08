#if !defined(AFX_CFVIEW_H__F0C0BD0C_5025_4674_9D9D_35AAE645724E__INCLUDED_)
#define AFX_CFVIEW_H__F0C0BD0C_5025_4674_9D9D_35AAE645724E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CFView.h : header file
//

#include "tgdoc.h"
/////////////////////////////////////////////////////////////////////////////
// CTimeFlagView view

class CTimeFlagView : public CView
{
protected:
	CTimeFlagView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CTimeFlagView)
	
// Operations
public:
	CTGDoc *m_pDoc;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTimeFlagView)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CTimeFlagView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CTimeFlagView)
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CBitmap * m_pOldBitmap;
	CBitmap m_bmpMemBitmap;
	CDC     m_dcMemoryDC;
	CFont   m_fontTimeString;	
private:
	void ReLayoutBmpPageSize();
	void ShowTimeString(CDC *pCDC,long lColor);
	void ShowTimeStringAutoMove(CDC *pCDC,long lColor);
public:
	void SetMyCtrl(CTGCtrl* ctrl) { m_pctrl = ctrl;}
private:
	CTGCtrl* m_pctrl;
public:
	afx_msg void OnDestroy();
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CFVIEW_H__F0C0BD0C_5025_4674_9D9D_35AAE645724E__INCLUDED_)
