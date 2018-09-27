#if !defined(AFX_SELECTSCHEDULETRAIN_H__A898E959_11BB_444B_831D_3CC100DAB674__INCLUDEDXXX_)
#define AFX_SELECTSCHEDULETRAIN_H__A898E959_11BB_444B_831D_3CC100DAB674__INCLUDEDXXX_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SelectScheduleTrain.h : header file
//
#include "headerlist.h"
#include "tooltipwnd.h"
#include "tg_structdef.h"
#include "markgrid.h"
/////////////////////////////////////////////////////////////////////////////
// CSelectScheduleMark dialog
class CWorkScheduleHolder;
class CSelectScheduleMark : public CDialog
{
// Construction
public:
	void ShowTrainList();
	CSelectScheduleMark(CWorkScheduleHolder *pHdr,CWnd* pParent = NULL);
	int  m_listcount;//add by cf after 9-9
    int m_nStartTime;
	void placeAnOrder();//add by cf after 9-4
	int *intArray;
	
	//modify by cf after 9-4
	int m_nSelectFromSchdType;
	BOOL m_bEnableChoiceSchedule;
	BOOL m_check;
	CMarkGridView  m_grid;
	CToolTipWnd m_ctrlTooltipWnd;

	CWorkScheduleHolder *m_pHolder;
	int m_nCurrentSel;
    
	int m_nPermitOperationFlag;

	int m_nSelectCount;
	
	struct TGSelectedBasicMark info[512];
    int  m_nCount;
// Dialog Data
	//{{AFX_DATA(CSelectScheduleMark)
	enum { IDD = IDD_DLG_MANUAL_SELECT_MARK };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSelectScheduleMark)
	public:
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSelectScheduleMark)
	virtual BOOL OnInitDialog();
	afx_msg void OnSbtAck();
	afx_msg void OnSbtCancel();
	afx_msg void OnDestroy();
	afx_msg void OnSelchangeSbtStationlist();
	afx_msg void OnSelchangeSbtEndtime();
	afx_msg void OnSelchangeSbtStarttime();
 	afx_msg void OnSbtGet();
	//}}AFX_MSG
	
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SELECTSCHEDULETRAIN_H__A898E959_11BB_444B_831D_3CC100DAB674__INCLUDED_)
