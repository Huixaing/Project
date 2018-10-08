#if !defined(AFX_SENDSTATIONPLANDLGEX_H__ADE66BDB_A8FD_42BA_B62C_B900AFF1286D__INCLUDED_)
#define AFX_SENDSTATIONPLANDLGEX_H__ADE66BDB_A8FD_42BA_B62C_B900AFF1286D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SendStationPlanDlgEx.h : header file
//
#include "resource.h"
#include ".\GridCtrl_src\GridCtrl.h"
#include <afxdtctl.h>
 #include <baseMsg\gpc_msgid.h>
#include <BaseMsg/clbth.h>
#include "widgets.h"
#include "tgdocument.h"
#include "stplan/stplan.h"
#include ".\StplanGrid.h"

/////////////////////////////////////////////////////////////////////////////
// CSendStationPlanDlgEx dialog

class CSendStationPlanDlgEx : public CPropertyPage   //CExpandingDialog
{
// Construction
public:
	CSendStationPlanDlgEx(CTgDocument *pDoc,CWnd* pParent = NULL);   // standard constructor
	virtual ~CSendStationPlanDlgEx();

public:
	int GetEntityList(int nStationNo,int *nEntityList);
	void QueryStStatus();
	void SendMsgStPlanStatusRequest(CLBTH::Archive &ar,int nStationNo);

public:
	CButton m_disableAutoChangeTrainNO;
    CTgDocument *m_pDoc;
 	CGridCtrl *m_pGridCtrl;
	int		m_nFixCols;
	int		m_nFixRows;
	int		m_nCols;
    int nUnitNum;
 	UINT m_nPrevStyle;

	long start_time;
	long end_time;

	time_t  m_nLastRecvStatuTime;

	CStplanGrid *m_pStplanGrid;
	void   SetStplanWarnList(CStplanGrid *pStplan) {m_pStplanGrid=pStplan;}

// Dialog Data
	//{{AFX_DATA(CSendStationPlanDlgEx)
	enum { IDD = IDD_SEND_STATION_PLAN_EX };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSendStationPlanDlgEx)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	int  CheckAllTrainDataIsValid(int nSelectedList[], int nSelectedNum);
	void CheckPassengerTrainSideAndTm(CTG_TrainRecord  *rec, int flag, int prev_station, int next_station, BOOL bCrhTrain);
	int  CheckStationSideFull(CTG_TrainScheduleBase *pdata);
    BOOL bAllStationFlag;
	void ShowStatus(int id, CString str);
	// Generated message map functions
	//{{AFX_MSG(CSendStationPlanDlgEx)
	virtual BOOL OnInitDialog();
	
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnGrid();
	afx_msg LRESULT ChangeStationReceiveStatus(WPARAM wParam,LPARAM lParam);
	afx_msg void OnDestroy();
	virtual void OnCancel();

	afx_msg void OnCheckAll();
	afx_msg void OnCheckStpc();
	afx_msg void OnCheckLirc();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	int GetStationList(int *nStList);
public:
	void SetBasicSchduleInfo();// 生成基本图信息
	virtual void OnOK();
	CString GetPlanDCmdId();
	afx_msg void OnBnClickedShowDetail();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnCbnSelchangeCboTimeRange();
	afx_msg void OnBnClickedClearAlarm();
////////////// 20080506 CUIHU //////////
	void  SetSendPlanTimeRange();
	afx_msg void OnCbnEditchangeCboTimeRange();
	afx_msg void OnBnClickedCheckCtcOnly();
	afx_msg void OnBnClickedCheckAutoclose();
	void ShowSendPlanText(CString text);
	void SendNoticeToDCmd(int nFlag);
	void SendStplanOnlySelLIRC();
	void SelAllStplanRecEntity();
protected:
	int  m_nSelectedStaNum ;
	int  m_nSelectedStaList[MAX_STATION_PER_SECTION];

	
	int m_nRecNum;
	int m_nRecList[MAX_STATION_PER_SECTION];
	
	int  AddSelectedSta( int nStNo );
	bool IsSelectedSta( int nStNo );
	
	bool IsNeedCheckCtcModeStplan(int station);
	
	STATIONPLANINFONEW *m_pPlanReceiveInfo;  
	ULONG nCurentId;
	CTrainDepartPredictHolder* m_pPredictServer;
public:
	afx_msg void OnCbnSelchangeCboGroup();
	afx_msg void OnBnClickedCheck2();
	void SetChngTrainButtonByConfig();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SENDSTATIONPLANDLG_H__ADE66BDB_A8FD_42BA_B62C_B900AFF1286D__INCLUDED_)
