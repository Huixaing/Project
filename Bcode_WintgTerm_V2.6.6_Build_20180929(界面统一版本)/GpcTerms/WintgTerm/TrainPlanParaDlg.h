#if !defined(AFX_TRAINPLANPARADLG_H__467A4345_8827_4722_AB48_B8AE49632250__INCLUDED_)
#define AFX_TRAINPLANPARADLG_H__467A4345_8827_4722_AB48_B8AE49632250__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TrainPlanParaDlg.h : header file
//
#include "resource.h"
#include "wintgdoc.h"

/////////////////////////////////////////////////////////////////////////////
// CTrainPlanParaDlg dialog

class CTrainPlanParaDlg : public CDialog
{
// Construction
public:
	CTrainPlanParaDlg(CWintgDoc *pDoc,int schd_type = WORK_SCHEDULE,CWnd* pParent = NULL);   // standard constructor
	BOOL IsSendBehind();
	int m_nCurrExit,m_nNextEntry;
	int m_nPrevExit,m_nCurrEntry;
	int abs_line[30];

	//added by Qian Guorong 2003-4-17//增加修改上行区间功能
//	int nEntry_CurentRec;
//	int nExit_PreviousRec;
	int abs_line_previous[30];
	int m_nSchdType;
	CSingleTrain *m_pTrain;
	int m_dispNum;
	DISPDISTANCE (*pDispDistance)[128]; // add by cf after 7-18
	int m_nTrackNo;
	int m_uPermitoutTime;
	BOOL m_check_limit;
 // Dialog Data
	//{{AFX_DATA(CTrainPlanParaDlg)
	enum { IDD = IDD_DLG_PLAN_PARA };
	CEdit	m_permit_out_date;
	CComboBox	m_combo_previous_abs;
	CButton	m_limit;
	CButton	m_ctrlPermitOutButton;
	CButton	m_ctrlEngineButton;
	CButton	m_ctrlAbsTimeButton;
	CButton	m_ctrlTrackButton;
	CEdit	m_ctrlAbsTime;
	CEdit	m_load_engine;
	CEdit	m_permit_out_time;
	CComboBox	m_combo_track;
	CComboBox	m_combo_next_abs;
	BOOL	m_check_adjust_start;
	BOOL	m_check_engine;
	BOOL	m_check_track;
	int		m_next_abs_time;
	BOOL	m_check_stop_train;
	BOOL	m_check_pass_side;
	BOOL	m_check_permit_out;
	CString	m_station;
	CString	m_train_id;
	CString	m_stop_time;
	CString	m_stop_time_min;
	BOOL	m_check_abs_time;
	CString	m_sLoadEngine;
	CString	m_sAbsTime;
	CString	m_Plan_arrival_time;
	CString	m_Plan_depart_time;
	CString	m_arrival_time;
	CString	m_depart_time;
	CString	m_Collide_reason;
	CString	m_stop_reason;
	int		m_exit;
	int		m_entry;
	CString	m_Plan_arrival_date;
	CString	m_Plan_depart_date;
	CString	m_spermit_out_time;
	CString	m_spermit_out_date;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTrainPlanParaDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
private:
	long orignStopTime;
			//ADD BY CF AFTER 7-18
	BOOL isSendBehind;
	BOOL okAvailable;
	CString originalAbsTime;
	CString originalStopTime;
	CBrush m_Brush;
public:
	void InitIdentify();
	BOOL IsSameAsPassToStay();
	BOOL sameAsPasstoStay;
 
	BOOL returnOK; //add by cf after 7-18
	int nEntry;//add by cf after 7-18
// Implementation
protected:
   	Pstruct TRAINRECORD *rec;
    CWintgDoc *m_pDoc;
	// Generated message map functions
	//{{AFX_MSG(CTrainPlanParaDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnCheckEngine();
	afx_msg void OnCheckTrack();
	afx_msg void OnCheckAbsTime();
	afx_msg void OnPermitOut();
	virtual void OnOK();
	afx_msg void OnKillfocusStopTime();
	afx_msg void OnSelchangeComboNextabs();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TRAINPLANPARADLG_H__467A4345_8827_4722_AB48_B8AE49632250__INCLUDED_)
