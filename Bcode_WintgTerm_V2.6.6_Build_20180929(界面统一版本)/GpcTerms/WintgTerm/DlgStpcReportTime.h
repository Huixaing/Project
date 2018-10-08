#if !defined(AFX_DLGSTPCREPORTTIME_H__4BF82B22_173D_439A_89F1_2F99B0E77B41__INCLUDED_)
#define AFX_DLGSTPCREPORTTIME_H__4BF82B22_173D_439A_89F1_2F99B0E77B41__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgStpcReportTime.h : header file
//
#include "tgmsg.h"
//#include "GridCtrl_src/GridCtrl.h"
#include "wintgdoc.h"
#include "headerlist.h"



/////////////////////////////////////////////////////////////////////////////
// CDlgStpcReportTime dialog

class CDlgStpcReportTime : public CDialog
{
// Construction
public:
	CDlgStpcReportTime(CWintgDoc *pDoc,CWnd* pParent = NULL);   // standard constructor
//	CGridCtrl *m_pGridCtrl;
//	CHeaderList m_list;
    int m_nCurrentStationSel,m_nCurrentRecSel;
    CWintgDoc *m_pDoc;
	time_t nPrevTime;
	bool m_bFirstClick,m_bSecondClick;
	// Dialog Data
	//{{AFX_DATA(CDlgStpcReportTime)
	enum { IDD = IDD_STPC_REPORT_TIME };
	CHeaderList	m_list;
	CComboBox	m_cmbStation;
	//}}AFX_DATA
	REPORT_TIME_VECTOR  m_vectorStpcReportTime;
    int m_nCurStation;
	bool ShowReportTimeRecord();
    void OperateRecord(int flag = 0);
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgStpcReportTime)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgStpcReportTime)
	afx_msg void OnDestroy();
	virtual BOOL OnInitDialog();
	afx_msg void OnOk();
	virtual void OnCancel();
	afx_msg void OnGrid();
	afx_msg void OnDblclkReportRecord(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemdblclickReportRecord(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangeCmbStation();
	afx_msg void OnDelete();
	afx_msg void OnItemchangedReportRecord(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRdblclkReportRecord(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	afx_msg LRESULT OnStpcReportTime(WPARAM wParam,LPARAM lParam);
 	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGSTPCREPORTTIME_H__4BF82B22_173D_439A_89F1_2F99B0E77B41__INCLUDED_)
