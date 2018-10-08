#if !defined(AFX_PLOTSTYLE_H__D2C5DFF3_880F_11D2_9DCD_000021000CE6__INCLUDED_)
#define AFX_PLOTSTYLE_H__D2C5DFF3_880F_11D2_9DCD_000021000CE6__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// PlotStyle.h : header file
//
#include "container.h"

/////////////////////////////////////////////////////////////////////////////
// CPlotStyle dialog

class CPlotStyle : public CDialog
{
// Construction
public:
	CPlotStyle(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPlotStyle)
	enum { IDD = IDD_DLG_STYLE };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA
//	CObArray IconList;
	CBitmapButton m_ack,m_cancel,m_modify;
	CContainer m_sample;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPlotStyle)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPlotStyle)
	afx_msg void OnStyleAck();
	afx_msg void OnStyleCancel();
	afx_msg void OnStyleChange();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	virtual BOOL OnInitDialog();
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PLOTSTYLE_H__D2C5DFF3_880F_11D2_9DCD_000021000CE6__INCLUDED_)
