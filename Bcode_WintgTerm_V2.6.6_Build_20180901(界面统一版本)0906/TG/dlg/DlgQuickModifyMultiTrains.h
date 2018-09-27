#pragma once
// UserPropertyDlg.h : header file
//
#include "resource.h"
#include "..\TGDoc.h"
#include "..\GridCtrl\GridCell.h"
#include "..\GridCtrl\GridCtrl.h"
#include "..\GridCtrl\GridCellCheck.h"
#include "..\GridCtrl\UnEditAbleGridCell.h"
/////////////////////////////////////////////////////////////////////////////
// CDlgQuickModifyMultiTrains dialog

class CDlgQuickModifyMultiTrains : public CDialog
{
	// Construction
public:
	CDlgQuickModifyMultiTrains(CTGDoc *pdoc, CWnd* pParent = NULL);   // standard constructor

	// Dialog Data
	//{{AFX_DATA(CDlgQuickModifyMultiTrains)
	enum { IDD = IDD_DLG_QUICK_MODIFY_MULTI_TRAINS };

	CComboBox m_start_station_list;
	CComboBox m_end_station_list;

	CComboBox m_buffer_list;
	CEdit m_move_min;

	CButton m_choose_move;
	CButton m_choose_slow;

	CDateTimeCtrl m_start_time;
	CDateTimeCtrl m_end_time;

	CGridCtrl * m_pGridCtrl;
	CTGDoc *m_pDoc;

	bool m_sel_all;
	CTime m_begin_tm, m_end_tm;
	int start_sta_idx, end_sta_idx;
	//}}AFX_DATA

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgQuickModifyMultiTrains)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgQuickModifyMultiTrains)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedFindTrains();
	afx_msg void OnBnClickedModifyTrains();
	afx_msg void OnBnClickedClose();
	afx_msg void OnCbnSelchangeStartStation();
	afx_msg void OnBnClickedModifyChkAll();
 	afx_msg void OnBnChooseMove();
	afx_msg void OnBnChooseSlow();

	void initialGridCtrl();
public:
	vector<DWORD> mSelTrainKey;
	afx_msg void OnBnClickedModifyTrainsClose();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
