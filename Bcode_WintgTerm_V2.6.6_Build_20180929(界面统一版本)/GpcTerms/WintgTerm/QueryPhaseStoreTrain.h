#if !defined(AFX_QUERYPHASESTORETRAIN_H__D87BD3FD_5D00_4B5F_A9DA_061B5EF79480__INCLUDED_)
#define AFX_QUERYPHASESTORETRAIN_H__D87BD3FD_5D00_4B5F_A9DA_061B5EF79480__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// QueryPhaseStoreTrain.h : header file
//
#include "headerlist.h"
#include "QueryStationCurrentStoreTrain.h" /////useful structure for 
#include "afxmt.h"//including critical section class
#include <baseMsg/gpc_msgid.h>
#include <BaseMsg/clbth.h>

/////////////////////////////////////////////////////////////////////////////
// CQueryPhaseStoreTrain dialog

class CQueryPhaseStoreTrain : public CDialog
{
// Construction
public:
	CQueryPhaseStoreTrain(CTGBasicParam *pBasicPara,int nConID,int nShiftid,BOOL readonly,CWnd* pParent = NULL);  

private:
	CHeaderList m_ctrlSectionStoreTrainList;
	
	int   m_nSectionID;
	int   m_nShiftID;
	BOOL  m_bReadOnly;
	
	CTGBasicParam *m_pBasicPara;
	int banbie;
	int nStart_time;
	int nEnd_time;
	int m_nCurrentPhase; //e.g: 0,6,12,18
	int nStationNum;
	int nStationList[64];
	int m_nCurrentSelectStation;
    int m_nCurrentSelStaIndex;
	int m_StdLineNo[64];//各个车站对应的在列表框中的起始行号
	
	CCriticalSection m_criticalShowStdInfo;//多行显示时候的临界区
	
// Dialog Data
	//{{AFX_DATA(CQueryPhaseStoreTrain)
	enum { IDD = IDD_DLG_PHASE_STORE_TRAIN };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CQueryPhaseStoreTrain)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CQueryPhaseStoreTrain)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg LRESULT OnSelectStation(WPARAM wParam,LPARAM lParam);
	afx_msg void OnSave();
	afx_msg LRESULT OnStationCurrentStoredTrain(WPARAM wParam,LPARAM lParam);
	afx_msg void OnCurrent();
	afx_msg void OnPhase1();
	afx_msg void OnPhase2();
	afx_msg void OnPhase3();
	afx_msg void OnButtonprintsingle();
	afx_msg void OnButtonprintall();
	afx_msg void OnClose();
	afx_msg void OnPstRefresh();
	afx_msg void OnButtonprintwhole();
	afx_msg void OnButtonSave();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
    afx_msg LRESULT OnCommMsg(WPARAM wParam, LPARAM lParam);

private:
	void PrintWhole(CDC *pDC, CPrintInfo *pInfo);
	void PrintWholeStation();
	int GetChineseSplitFlag(CString &chinesestr);
	CString ConvertShiftIdToString(unsigned long id);
	void OnPrint(CDC* pDC,CPrintInfo* pInfo, int StationNo);
	int ConvertSelIndex2StationNO(int selIndex);
	BOOL PrintSingleStation(int stationNo);
	
	BOOL ShowStationInfoMultiLines(int stationno);

private:
	unsigned long GetTimeIdx();
	void GetStationCarsInfo(ushort StationIndex,byte type =1);
	void SendSideinfo(int StationIndex, unsigned long timeidx,byte type);
	void SetCurrentPhase(int index );
	int  GetStationidInList(int nStationNo);

	void SaveAllStationCar(WORD new_phase);
	int  IsSideInfoExist(ushort nStationNo, char *sSideName);
	int  nReceiveStatus;
	void SelectStation(int st);
	BOOL ProcessMessage(CLBTH::MsgSimpleStationCarCtrl *pBuf);
	BOOL ProcessMessage(CLBTH::MsgSimpleStationCarFormat *pBuf);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_QUERYPHASESTORETRAIN_H__D87BD3FD_5D00_4B5F_A9DA_061B5EF79480__INCLUDED_)
