#if !defined(AFX_SELECTSCHEDULETRAIN_H__A898E959_11BB_444B_831D_3CC100DAB674__INCLUDED_)
#define AFX_SELECTSCHEDULETRAIN_H__A898E959_11BB_444B_831D_3CC100DAB674__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SelectScheduleTrain.h : header file
//
#include "headerlist.h"
#include "tooltipwnd.h"
#include "tg_structdef.h"
/////////////////////////////////////////////////////////////////////////////
// CSelectScheduleTrain dialog
class CWorkScheduleHolder;
class CSelectScheduleTrain : public CDialog
{
// Construction
public:
	void ShowTrainList();
	CSelectScheduleTrain(CWorkScheduleHolder *pHdr,CWnd* pParent = NULL);
	int  m_listcount;//add by cf after 9-9
    int m_nStartTime;
	void placeAnOrder();//add by cf after 9-4
	int *intArray;
	
	//modify by cf after 9-4
	int m_nSelectFromSchdType;
	BOOL m_bEnableChoiceSchedule;

	CHeaderList m_list;
	CToolTipWnd m_ctrlTooltipWnd;

	CWorkScheduleHolder *m_pHolder;
	int m_nCurrentSel;
    
	int m_nPermitOperationFlag;

	int m_nSelectCount;
	
	struct TGSelectedBasicTrain info[1024];
    int  m_nCount;
// Dialog Data
	//{{AFX_DATA(CSelectScheduleTrain)
	enum { IDD = IDD_DLG_SELECT_TRAIN };
	CBitmapButton	m_cancelButton;
	CBitmapButton	m_ackButton;
	BOOL	m_check;
	BOOL	m_checksorttrainid;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSelectScheduleTrain)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSelectScheduleTrain)
	virtual BOOL OnInitDialog();
	afx_msg void OnSbtAck();
	afx_msg void OnSbtCancel();
	afx_msg void OnSbtDelete();
	afx_msg void OnDestroy();
	afx_msg void OnSbtFreight();
	afx_msg void OnSbtKeche();
	afx_msg void OnSbtReceive();
	afx_msg void OnSbtStart();
	afx_msg void OnSbtDowndir();
	afx_msg void OnSbtUpdir();
	afx_msg void OnSelchangeSbtStationlist();
	afx_msg void OnSbtAlldir();
	afx_msg void OnSelchangeSbtEndtime();
	afx_msg void OnSelchangeSbtStarttime();
 	afx_msg void OnSbtGet();
	afx_msg void OnCheckSorttrainid();
	//}}AFX_MSG
	
	DECLARE_MESSAGE_MAP()

private:
	void  ShowSelectScheTrains();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SELECTSCHEDULETRAIN_H__A898E959_11BB_444B_831D_3CC100DAB674__INCLUDED_)
