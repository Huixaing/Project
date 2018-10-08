#pragma once

#include "resource.h"
#include "tg_work_schdule_holder.h"
/////////////////////////////////////////////////////////////////////////////
// CDlgSelTDMSBasicGen dialog
class CDlgSelTDMSBasicGen : public CDialog
{
// Construction
public:
	CDlgSelTDMSBasicGen(CWnd* pParent, CTG_TrainScheduleBase* pData, CWorkScheduleHolder* pHolder);   // standard constructor
	~CDlgSelTDMSBasicGen();
// Dialog Data
	//{{AFX_DATA(CDlgSelTDMSBasicGen)
	enum { IDD = IDD_DLG_SEL_TDMS_GENTRAINN };
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgSelTDMSBasicGen)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	virtual void PostNcDestroy();
// Implementation
protected:	
	// Generated message map functions
	//{{AFX_MSG(CDlgSelTDMSBasicGen)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedGenBasic();
	afx_msg void OnBnClickedClearSel();
	afx_msg void OnBnClickedAppendMultiSel();
	afx_msg void OnBnClickedAppendAll();
	afx_msg void OnClose();
private:
	CString AddGenBasicTrain(CString str, DWORD gid);

private:
	CListBox m_all_list;
	CListBox m_sel_list;
	CWorkScheduleHolder *m_pHolder;
	CTG_TrainScheduleBase *m_pData;	
public:
	afx_msg void OnLbnDblclkList1();
	afx_msg void OnLbnDblclkList2();
};

