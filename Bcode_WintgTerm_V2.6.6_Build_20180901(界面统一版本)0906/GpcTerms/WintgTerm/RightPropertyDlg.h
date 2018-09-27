#pragma once
// UserPropertyDlg.h : header file
//

#include "resource.h"
#include "tgmsg.h"
/////////////////////////////////////////////////////////////////////////////
// CRightPropertyDlg dialog

class CRightPropertyDlg : public CDialog
{
	// Construction
public:
	CRightPropertyDlg(bool insert = true, CWnd* pParent = NULL);   // standard constructor

	// Dialog Data
	//{{AFX_DATA(CRightPropertyDlg)
	enum { IDD = IDD_RIGHT_PROPERTY };

	CString m_con_name;
	int m_con_id;
	int m_entity_id;
	CString m_entity_name;
	int m_parament;

	CComboBox m_con_list;
	BOOL m_Modify_Acutal;
	int  m_modify_time;
	char role_param[2048];
	//}}AFX_DATA
	bool m_insert;

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRightPropertyDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CRightPropertyDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnCbnSelchangeConList();
	afx_msg void OnBnClickedEnter();
	afx_msg void OnBnClickedClose();
	afx_msg void OnBnClickedCheckEntity();

private:
	void ParseRoleParam();
	void SetRoleParam();
public:
	afx_msg void OnBnClickedCheckModifyActual();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
