#if !defined(AFX_DLGBASICPARAM_H__47EBEFA2_7572_46A5_9A72_B87204BA93A0__INCLUDED_)
#define AFX_DLGBASICPARAM_H__47EBEFA2_7572_46A5_9A72_B87204BA93A0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgBasicParam.h : header file
//
#include "resource.h"
class CTgDocument;
/////////////////////////////////////////////////////////////////////////////
// CDlgBasicParam dialog

class CDlgBasicParam : public CDialog
{
// Construction
public:
	CDlgBasicParam(CTgDocument *pDoc, int schdtype, CWnd* pParent);   // standard constructor
	~CDlgBasicParam();
// Dialog Data
	//{{AFX_DATA(CDlgBasicParam)
	enum { IDD = IDD_DLG_BASIC_PARA };
	CProgressCtrl	m_CtrlPro;
	CDateTimeCtrl m_basic_start_day;
	CDateTimeCtrl m_basic_start_time;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgBasicParam)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CDlgBasicParam)
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
	int m_nschdtype;
	CEdit m_name;
	CString m_strName;
	CWnd* m_pParant;
	ENTITY_ID m_myEntity;
public:
	afx_msg void OnClose();
	afx_msg void OnBnClickedOk2();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGBASICPARAM_H__47EBEFA2_7572_46A5_9A72_B87204BA93A0__INCLUDED_)
