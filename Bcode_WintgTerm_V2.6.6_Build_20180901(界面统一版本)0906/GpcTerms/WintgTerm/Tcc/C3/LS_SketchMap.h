#if !defined(LS_SketchMap_H)
#define LS_SketchMap_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// LS_SketchMap.h : header file
//

#include "Tcc_base.h"
//#include "MySliderCtrl.h"
namespace TCC_C3 {

/////////////////////////////////////////////////////////////////////////////

class CLS_SketchMap : public CWnd
{
// Construction
public:
	CLS_SketchMap(ITccClientInfoProvider *pInfoProvider);
	long AllLength;
	long f;
	CScrollBar scrollbar;
	CRect RT;
	CRect rt;
	CRect RedrawRT;
	long xStart;
	long BtnLpos;//◊Ûª¨øÈ≥ı ºŒª÷√
	long BtnRpos;//”“ª¨øÈ≥ı ºŒª÷√
	long BtnTop;
	CButton BtnL;
	CButton BtnR;
	CRect BtnLRect;
	CRect BtnRRect;
	BOOL btnStatus;
	BOOL btnStatusR;
	BOOL BtnS;
	int StaDis;
	BOOL m_BtnEnV;
	BOOL BtnSLpos; //
	BOOL BtnSRpos; //
	BOOL BtnSpos;
//	CMySliderCtrl sliderctrl;
	void Init();
	vector<long> Tic;
// Attributes
public:
	CBrush m_brushNormal;
	COLORREF m_clrBkgnd;
	kmmark_t m_nKmMark[2];
	kmmark_t m_nKmMark0;
	kmmark_t m_nKmMark1;
	kmmark_t m_Tcclist_Begin;
	kmmark_t m_Tcclist_End;
	dir_t  dir_limit;
protected:
	CSize	m_totalLog;
	CSize	m_totalDev;

	std::vector<RAW_STOCK_COMMAND::_TCC_STRUCT> tcclist;	
	const LINE_STRUCT *m_pLine;
	ITccClientInfoProvider *m_pInfoProvider;

// Operations
public:
	void SetKmMark(int builtin_id, kmmark_t nKmBegin, kmmark_t nKmEnd);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLS_SketchMap)
	public:
	virtual BOOL Create(LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID,BOOL BtnEn);
	virtual void OnPrepareDC(CDC* pDC);
	virtual void OnDraw(CDC* pDC);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CLS_SketchMap();
	double computef(int stas,int secs);

private:

	// Generated message map functions
protected:
	//{{AFX_MSG(LS_SketchMap)
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnNcPaint();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

}//end of namespace TCC_C3

#endif // !defined(LS_SketchMap_H)
