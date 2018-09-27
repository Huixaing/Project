#if !defined(AFX_DLGTCC_SEL_H__47EBEFA2_7572_46A5_9A72_B87204BA93A0__INCLUDED_)
#define AFX_DLGTCC_SEL_H__47EBEFA2_7572_46A5_9A72_B87204BA93A0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgBasicBackupSelect.h : header file
//
#include "resource.h"
/////////////////////////////////////////////////////////////////////////////
// CDlgTCCSelect dialog

class CDlgTCCSelect : public CDialog
{
// Construction
public:
	CDlgTCCSelect(CWnd* pParent=NULL);   // standard constructor
	~CDlgTCCSelect();
// Dialog Data
	//{{AFX_DATA(CDlgTCCSelect)
	enum { IDD = IDD_DLG_TCC_SEL };
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgTCCSelect)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CDlgTCCSelect)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	void OnSelBackupBasicSchd();
	DECLARE_MESSAGE_MAP()

	CComboBox m_combo_sel;
public:
	int m_nSelSchdType;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGTCC_SEL_H__47EBEFA2_7572_46A5_9A72_B87204BA93A0__INCLUDED_)
