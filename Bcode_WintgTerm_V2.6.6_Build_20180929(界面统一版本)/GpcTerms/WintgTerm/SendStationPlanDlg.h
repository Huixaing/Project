#if !defined(AFX_SENDSTATIONPLANDLG_H__ADE66BDB_A8FD_42BA_B62C_B900AFF1286D__INCLUDED_)
#define AFX_SENDSTATIONPLANDLG_H__ADE66BDB_A8FD_42BA_B62C_B900AFF1286D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SendStationPlanDlg.h : header file
//
#include "..\\wintg\resource.h"
#include "wintgdoc.h"
#include "GridCtrl_src/GridCtrl.h"
#include <afxdtctl.h>
 
/////////////////////////////////////////////////////////////////////////////
// CSendStationPlanDlg dialog

class CSendStationPlanDlg : public CDialog
{
// Construction
public:
//	void ChangeStationReceiveStatus(int nStationNo,int nFlag);
	CSendStationPlanDlg(CWintgDoc *pDoc,CWnd* pParent = NULL);   // standard constructor
    CWintgDoc *m_pDoc;
 	CGridCtrl *m_pGridCtrl;
	int		m_nFixCols;
	int		m_nFixRows;
	int		m_nCols;
	int		m_nRows;
    int nUnitNum;
	CDateTimeCtrl m_StartDateTime;
	CDateTimeCtrl m_EndDateTime;
	UINT m_nPrevStyle;

// Dialog Data
	//{{AFX_DATA(CSendStationPlanDlg)
	enum { IDD = IDD_SEND_STATION_PLAN };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSendStationPlanDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:
    bool bAllStationFlag;
	// Generated message map functions
	//{{AFX_MSG(CSendStationPlanDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnGrid();
	afx_msg LRESULT ChangeStationReceiveStatus(WPARAM wParam,LPARAM lParam);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SENDSTATIONPLANDLG_H__ADE66BDB_A8FD_42BA_B62C_B900AFF1286D__INCLUDED_)
