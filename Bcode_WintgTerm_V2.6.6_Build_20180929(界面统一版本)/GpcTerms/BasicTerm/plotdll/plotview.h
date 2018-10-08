#if !defined(AFX_PLOTVIEW_H__D47480D4_F5BD_11D1_9D7A_0000C05319CE__INCLUDED_)
#define AFX_PLOTVIEW_H__D47480D4_F5BD_11D1_9D7A_0000C05319CE__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// PlotView.h : header file
//
#include "resource.h"
#include <DEFINES.H>	// Added by ClassView
#include "plotdoc.h"
#include "tgmsg.h"


#ifndef __AFXEXT_H__
#include <afxext.h>
#endif


#define CLR_PLOT RGB(92,180,90)


class CPlotView : public  CScrollView
{
protected:
	CPlotView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CPlotView)
	virtual ~CPlotView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
   	virtual CPlotDoc *GetDocument( );
#endif

// Operations
public: 
    int  GetStationidInList(int nStationNo);
	int IsSideInfoExist(ushort nStationNo,ushort phase,char *sSideName);
	static bool CreateRecursiveDir(char* filename);
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPlotView)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void OnDraw(CDC* pDC);
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPlotView)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	
	DECLARE_MESSAGE_MAP()

public:
	
public:
	CRect m_rectViewPort;
	
	int nFlag;  
	
	int m_nPageHeight;
	int m_nPageWidth;

    struct _tagStationStoreTrain Info[64][4]; // struct to store train info
	int nStart_time;
	int nEnd_time;
	
	CBrush m_brushBackground;

	CBitmap * m_pOldBitmap;
	CBitmap m_MemBitmap;
	CDC m_dcMemory;

	int nReceiveStatus;
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PLOTVIEW_H__D47480D4_F5BD_11D1_9D7A_0000C05319CE__INCLUDED_)
