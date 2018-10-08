#if !defined(AFX_DLGBASICCHANGE_H__47EBEFA2_7572_46A5_9A72_B87204BA93A0__INCLUDED_)
#define AFX_DLGBASICCHANGE_H__47EBEFA2_7572_46A5_9A72_B87204BA93A0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgBasicChange.h : header file
//
#include "resource.h"
class CTgDocument;
/////////////////////////////////////////////////////////////////////////////
// CDlgBasicChange dialog

class CDlgBasicChange : public CDialog
{
// Construction
public:
	CDlgBasicChange(CTgDocument *pDoc, CTG_ScheduleInfo::SCHEDULE_TYPE schdtype, CWnd* pParent = NULL);   // standard constructor
	~CDlgBasicChange();
// Dialog Data
	//{{AFX_DATA(CDlgBasicChange)
	enum { IDD = IDD_DLG_BASIC_CHANGE };
	CProgressCtrl	m_CtrlPro;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgBasicChange)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CDlgBasicChange)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	
	LRESULT OnCommMsg(WPARAM wParam,LPARAM lParam);
	DECLARE_MESSAGE_MAP()

private:
	int m_nConid;
	int nChangeStatusPos;
	CTgDocument *m_pDoc;
	int m_nObjBackupSchdSubType;
	
	CComboBox	m_combo_cur;
	CComboBox	m_combo_cur_name;
	CComboBox	m_combo_obj;
	CComboBox	m_combo_obj_name;

public:
	CTG_ScheduleInfo::SCHEDULE_TYPE m_nSrcSchdType; // 源基本图的类型
	CTG_ScheduleInfo::SCHEDULE_TYPE m_nObjSchdType; // 目的基本图的类型

public:
	afx_msg void OnBnClickedOk();
	void OnSelObjSchdType();
	void OnSelObjSchdName();
	BOOL m_bSucceed;
	afx_msg void OnBnClickedCancel();
	afx_msg void OnClose();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGBASICCHANGE_H__47EBEFA2_7572_46A5_9A72_B87204BA93A0__INCLUDED_)
