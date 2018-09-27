#if !defined(AFX_TRAINPLANPARADLG_H__467A4345_8827_4722_AB48_B8AE49632250__INCLUDED_)
#define AFX_TRAINPLANPARADLG_H__467A4345_8827_4722_AB48_B8AE49632250__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TrainPlanParaDlg.h : header file
//
#include "resource.h"
#include "tgdoc.h"
#include "afxdtctl.h"

/////////////////////////////////////////////////////////////////////////////
// CTrainPlanParaDlg dialog

class CTrainPlanParaDlg : public CDialog
{
// Construction
public:
	CTrainPlanParaDlg(CWnd* pParent);   // standard constructor
	int m_nCurrExit,m_nNextEntry;
	int m_nPrevExit,m_nCurrEntry;
	int abs_line[100];


	int abs_line_previous[100];
	
	int m_dispNum;
	int m_nTrackNo;
	int m_uPermitoutTime;
	BOOL m_check_limit;

 // Dialog Data
	//{{AFX_DATA(CTrainPlanParaDlg)
	enum { IDD = IDD_DLG_PLAN_PARA };

	CComboBox	m_combo_track;
	CComboBox	m_combo_next_abs;
	CComboBox	m_combo_previous_abs;

	CButton	m_limit;
	CButton	m_ctrlAbsTimeButton;
	CButton	m_ctrlTrackButton;
	CButton	m_sidePassButton;		
	
	BOOL	m_check_adjust_start;
	BOOL	m_check_stop_train;
		
	CString	m_station;
	CString	m_train_id;
			
	int		m_exit;
	int		m_entry;

	CDateTimeCtrl m_plan_arl_day;
	CDateTimeCtrl m_plan_arl_time;
	CDateTimeCtrl m_plan_dep_day;
	CDateTimeCtrl m_plan_dep_time;
	CDateTimeCtrl m_arrival;
	CDateTimeCtrl m_depart;
	CDateTimeCtrl m_stop_station_time;
	CDateTimeCtrl m_min_stop_station_time;
	CDateTimeCtrl m_next_abs_runtime;

	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTrainPlanParaDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
private:
	BOOL SendTrainUpdate(CTG_TRAIN* pTrain);
	void SetCenterPosition();

public:
	void InitIdentify();
	int nEntry;//add by cf after 7-18
// Implementation
protected:
   	   
	// Generated message map functions
	//{{AFX_MSG(CTrainPlanParaDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnCheckTrack();
	afx_msg void OnCheckAbsTime();
	virtual void OnOK();
	afx_msg void OnKillfocusStopTime();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	int m_train_index;
	int m_rec_index;
	int m_foucs_station;
	CTGDoc *m_pDoc;
	int mTgType;
	TgTRAIN_RECORD m_rec;
protected:
	virtual void PostNcDestroy();

private:
	void SetTimeCtrlTime(int nID, long tm);
	long GetTimeCtrlTime(int nID);
	void SetStopTime();
	void RemoveSameLineNo(int& lineno_count, int lineno[]);
public:
	afx_msg void OnBnClickedCancel();
	afx_msg void OnCbnSelSide();
	afx_msg void OnCbnSelPreviousAbs();
	afx_msg void OnCbnSelNextAbs();
	afx_msg void OnDtnDatetimechangeDatetimepicker6(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDtnDatetimechangeDatetimepicker5(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDtnDatetimechangeDatetimepicker7(NMHDR *pNMHDR, LRESULT *pResult);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TRAINPLANPARADLG_H__467A4345_8827_4722_AB48_B8AE49632250__INCLUDED_)
