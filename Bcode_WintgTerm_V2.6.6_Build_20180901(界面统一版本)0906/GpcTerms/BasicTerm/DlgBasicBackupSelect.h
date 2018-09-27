#if !defined(AFX_DLGBASICBACKUP_H__47EBEFA2_7572_46A5_9A72_B87204BA93A0__INCLUDED_)
#define AFX_DLGBASICBACKUP_H__47EBEFA2_7572_46A5_9A72_B87204BA93A0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgBasicBackupSelect.h : header file
//
#include "resource.h"
class CTgDocument;
/////////////////////////////////////////////////////////////////////////////
// CDlgBasicBackupSelect dialog

class CDlgBasicBackupSelect : public CDialog
{
// Construction
public:
	CDlgBasicBackupSelect(int con_id, CWnd* pParent=NULL);   // standard constructor
	~CDlgBasicBackupSelect();
// Dialog Data
	//{{AFX_DATA(CDlgBasicBackupSelect)
	enum { IDD = IDD_DLG_BASIC_BACKUP };
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgBasicBackupSelect)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CDlgBasicBackupSelect)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	void OnSelBackupBasicSchd();
	LRESULT OnCommMsg(WPARAM wParam,LPARAM lParam);
	DECLARE_MESSAGE_MAP()

	CComboBox m_combo_backup;
	CEdit sttm, edtm;
	int m_con_id;
public:
	int m_nSelSchdType;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGBASICBACKUP_H__47EBEFA2_7572_46A5_9A72_B87204BA93A0__INCLUDED_)
