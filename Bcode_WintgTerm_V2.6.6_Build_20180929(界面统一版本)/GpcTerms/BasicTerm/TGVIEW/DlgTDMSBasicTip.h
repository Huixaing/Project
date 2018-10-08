#pragma once

#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgTDMSBasicTip dialog
class CDlgTDMSBasicTip : public CDialog
{
// Construction
public:
	CDlgTDMSBasicTip(CWnd* pParent = NULL);   // standard constructor
	~CDlgTDMSBasicTip();
// Dialog Data
	//{{AFX_DATA(CDlgTDMSBasicTip)
	enum { IDD = IDD_DLG_GET_TDMS_BASIC };
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgTDMSBasicTip)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:	
	// Generated message map functions
	//{{AFX_MSG(CDlgTDMSBasicTip)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	CButton m_check;
	bool m_bSelTDMSTrainGenBasic;
	afx_msg void OnSelTDMSStateChange();
};

