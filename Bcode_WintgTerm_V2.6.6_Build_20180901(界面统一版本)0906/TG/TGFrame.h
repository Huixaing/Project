#if !defined(AFX_WINTGFRAME_H__27E02CF9_7B29_11D1_9CEF_000021000CE6__INCLUDED_)
#define AFX_WINTGFRAME_H__27E02CF9_7B29_11D1_9CEF_000021000CE6__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "tgform.h"
#include "locksplitter.h"
#include "tgdoc.h"

#define MAX_SAME_TRAIN_ID_NUM 64

struct msgCtrlMode
{
	UINT staId;
	BYTE route_mode;
	BYTE sta_mode;
};

struct msgPlanParam
{
	long staId;
	BYTE type;
	DWORD color;
};

class CTGFrame : public CFrameWnd
{
	DECLARE_DYNCREATE(CTGFrame)
public:
	CTGFrame();          
	virtual ~CTGFrame();
// Operations
public:
	
  	CLockSplitter m_wndSplitterHorz;
public:
	void SetMyDocument(CTGDoc* pDocument);
	void SetMyCtrl(CTGCtrl* ctrl);
private:
	CTGCtrl* myPCtrl;
	CTGForm* m_pWintgForm;
	 
public:
	CTG_Mange myDataManage;
public:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTGFrame)
	protected:
		virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
		
	//}}AFX_VIRTUAL

// Implementation
protected:

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	// Generated message map functions
	//{{AFX_MSG(CTGFrame)
	afx_msg void    OnSysCommand(UINT nID, LPARAM lParam);	
	afx_msg LRESULT OnAddNewTrainGraphEnd(WPARAM wParam,LPARAM lParam); 
	afx_msg LRESULT OnShowDialog(WPARAM wParam,LPARAM lParam); 
	afx_msg LRESULT OnSchdRecieveStatus(WPARAM wParam,LPARAM lParam);
	
	

	

	afx_msg LRESULT OnStationStatus(WPARAM wParam, LPARAM lParam); 
	afx_msg LRESULT OnSetCtrlMode(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSetPlanStatus(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnRefreshTG(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSelectSection(WPARAM wParam,LPARAM lParam);
	
	afx_msg LRESULT OnGetPlotParam(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnGetTGData(WPARAM wParam, LPARAM lParam);
	
	afx_msg LRESULT OnLiveWindow(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnGenTrainIndex(WPARAM wParam, LPARAM lParam);

	afx_msg LRESULT OnMoveTrainFocus(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
public:
	
	CString OnSetTrainDefault(CTG_TRAIN& train);

	CTGForm* GetFormView();
	void  SetMutiSelFlag(short flag);
	void  ShowTrainReverse(short flag);
	short GetTrainReverse(short flag);
	
	void OutputTrainsRelation();
	void ProhibitEditRealTime(short type);
	void KeyDown(short type);
	void TrainReDraw(int nFlag);
		
	void drawSpecialTrain(short trainType);
	
	void OnNewShiftBanci();
				
	void FindTrain();
	void FindTrain(DWORD train_index);
	void MoveTrainFocusFromFindTrainResult(long position, long train_index);
	void SetDeleteMultiTrainFlag();

	void OnReceivePoint();

	CString SetTrainExitEntryByStationIndex(CTG_TRAIN& train);
	CString GetTrainJiaoRelation(TRAIN_INDEX train_index);
private:
	CDlgWait* m_waitDlg;
	bool m_bLoad;
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);

	short GetZoomStatus();
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WINTGFRAME_H__27E02CF9_7B29_11D1_9CEF_000021000CE6__INCLUDED_)
