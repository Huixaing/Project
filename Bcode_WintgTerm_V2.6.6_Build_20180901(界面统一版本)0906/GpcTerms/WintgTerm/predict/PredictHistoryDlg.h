#if !defined(AFX_IDD_DLG_PREDICT_HISTORY_H__20722814_2E47_4240_B335_90B796932D61__INCLUDED_)
#define AFX_IDD_DLG_PREDICT_HISTORY_H__20722814_2E47_4240_B335_90B796932D61__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PredictHistoryDlg.h : header file
//
#include "resource.h"
#include ".\stationpredict.h"
#include ".\predictgridview.h"
#include ".\PredictHistoryDlg.h"
#define MAX_PREDICT_STATION_PLAN 64
/////////////////////////////////////////////////////////////////////////////

class CPredictHistoryDlg : public CDialog
{
// Construction
public:
	CPredictHistoryDlg(int nSectionID, CWnd* pParent = NULL);   // standard constructor
	
// Dialog Data
	//{{AFX_DATA(CPredictHistoryDlg)
	enum { IDD = IDD_DLG_PREDICT_HIS };
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPredictHistoryDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	int   OnCreate(LPCREATESTRUCT lpCreat);
// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPredictHistoryDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	CPredictGridView m_grid;
	map<UINT, TrainPredict> mPlan;

private:
	CStationPredict* m_pStPlans[MAX_PREDICT_STATION_PLAN];
    int m_nSectionID;
	CString m_strShiftid;
private:
	afx_msg void LoadFromHistory();
	void AppendPredictPlan(TrainPredict& predict);
    void ReloadStPlan();
	void UpdateStationList();
private:
	void  OnSize(UINT nType,int cx,int cy);
	
public:
	void OnCbnSelchangeStation();
	
	afx_msg void OnClose();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IDD_DLG_PREDICT_HISTORY_H__20722814_2E47_4240_B335_90B796932D61__INCLUDED_)
