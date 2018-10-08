#if !defined(AFX_DLGSTATISTICSTRAIN_H__AE89592B_0DB8_4AD0_ADD2_7B6F592F24E7__INCLUDED_)
#define AFX_DLGSTATISTICSTRAIN_H__AE89592B_0DB8_4AD0_ADD2_7B6F592F24E7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgStatisticsTrain.h : header file
//
#include "StatisticsStoreStructure4train.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgStatisticsTrain dialog

class CDlgStatisticsTrain : public CDialog
{
// Construction
public:
	CDlgStatisticsTrain(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgStatisticsTrain)
	enum { IDD = IDD_DLG_STATISTICSTRAIN };
	//}}AFX_DATA

	CStatisticsStoreStructure4Train m_datastore;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgStatisticsTrain)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation

	long m_shiftdate;
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgStatisticsTrain)
	afx_msg void OnButtonClose();
	afx_msg void OnButtonLoad();
	afx_msg void OnButtonDatasend();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	afx_msg LRESULT OnSchdAddTrain(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnSchdStatus(WPARAM wParam,LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGSTATISTICSTRAIN_H__AE89592B_0DB8_4AD0_ADD2_7B6F592F24E7__INCLUDED_)
