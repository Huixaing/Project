#if !defined(AFX_PLOTFRAME_H__D47480D3_F5BD_11D1_9D7A_0000C05319CE__INCLUDED_)
#define AFX_PLOTFRAME_H__D47480D3_F5BD_11D1_9D7A_0000C05319CE__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// PlotFrame.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPlotFrame frame

class CPlotFrame : public CFrameWnd
{
	DECLARE_DYNCREATE(CPlotFrame)
protected:
	CPlotFrame();           // protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPlotFrame)
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CPlotFrame();

	// Generated message map functions
	//{{AFX_MSG(CPlotFrame)
	afx_msg void OnPlotExit();
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLineDirect();
	afx_msg void OnUpdateLineDirect(CCmdUI *pCmdUI);
	afx_msg void OnArlAndDepart();
	afx_msg void OnUpdateArlAndDepart(CCmdUI *pCmdUI);
	afx_msg void OnDirectArlAndDepart();
	afx_msg void OnUpdateDirectArlAndDepart(CCmdUI *pCmdUI);
	afx_msg void OnWavePass();
	afx_msg void OnUpdateWavePass(CCmdUI *pCmdUI);
	afx_msg void OnPassenger();
	afx_msg void OnUpdatePassenger(CCmdUI *pCmdUI);
	afx_msg void OnGoods();
	afx_msg void OnUpdateGoods(CCmdUI *pCmdUI);
	afx_msg void OnPessagegoods();
	afx_msg void OnUpdatePessagegoods(CCmdUI *pCmdUI);
	afx_msg void OnThinLine();
	afx_msg void OnUpdateThinLine(CCmdUI *pCmdUI);
	
	afx_msg void OnShowVirtualStationPlot();
	afx_msg void OnUpdateVirtualStationPlot(CCmdUI *pCmdUI);

	afx_msg void OnShowCzhConnectLine();
	afx_msg void OnUpdateCzhConnectLine(CCmdUI *pCmdUI);

	afx_msg void OnNotShowTechStopMark();
	afx_msg void OnUpdateNotShowTechStopMark(CCmdUI *pCmdUI);
	
	afx_msg void OnTrainType(UINT nID);
	afx_msg void OnUpdateTrainType(CCmdUI *pCmdUI);

	afx_msg void OnTrainDirect(UINT nID);
	afx_msg void OnUpdateTrainDirect(CCmdUI *pCmdUI);
	afx_msg void OnInitMenu(CMenu* pMenu);

private:
	int FindMenuItem(CMenu* Menu, LPCTSTR MenuString);

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PLOTFRAME_H__D47480D3_F5BD_11D1_9D7A_0000C05319CE__INCLUDED_)
