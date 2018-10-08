#if !defined(AFX_STATIONFORM_H__8CC2A1F3_912E_11D1_9D0E_000021000CE6__INCLUDED_)
#define AFX_STATIONFORM_H__8CC2A1F3_912E_11D1_9D0E_000021000CE6__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// StationForm.h : header file
//
#include "tgdoc.h"
#include "ToolTipWnd.h"	
/////////////////////////////////////////////////////////////////////////////
// CStationView view

class CStationView : public CView
{
protected:
	CStationView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CStationView)

// Attributes
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStationView)
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual void OnInitialUpdate();     // first time after construct
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CStationView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	// Generated message map functions
	//{{AFX_MSG(CStationView)
	afx_msg void OnSetStationErrorSide();
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	CTGDoc *m_pDoc;
	
private:
	
	CFont m_fontSide; 
	CFont m_fontKilo; 
	CFont m_fontStatus; 
	CFont m_fontStation;
	int     m_nCurSelDistanceNo;
	CBitmap * m_pOldBitmap;
	CBitmap m_bmpMemBitmap;
	CDC     m_dcMemoryDC;
    int X[7];
private:
	void OnShowStationSides();
	void ShowStationString(CDC *pDC,long lColor);
	void ShowVirtualStationString(CDC * pDC, long lColor);
	void ReLayoutBmpPageSize(int width);
public:
	void SetMyCtrl(CTGCtrl* ctrl) { m_pctrl = ctrl;}
private:
	CTGCtrl* m_pctrl;
public:
	afx_msg void OnDestroy();
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STATIONFORM_H__8CC2A1F3_912E_11D1_9D0E_000021000CE6__INCLUDED_)
