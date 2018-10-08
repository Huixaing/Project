#if !defined(AFX_DLGDIVIDEPORTLATETIME_H__F5AF743E_12EF_47F0_BC8C_85C7365EE318__INCLUDED_)
#define AFX_DLGDIVIDEPORTLATETIME_H__F5AF743E_12EF_47F0_BC8C_85C7365EE318__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgDividePortLateTime.h : header file
//
#include "tg_schedule_base.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgDividePortLateTime dialog

class CDlgDividePortLateTime : public CDialog
{
// Construction
public:
	CDlgDividePortLateTime(CTG_SingleTrain *ptrain, CWnd* pParent);   // standard constructor
    
// Dialog Data
	//{{AFX_DATA(CDlgDividePortLateTime)
	enum { IDD = IDD_DIVIDEPORT_LATETIME };
	CString	m_strTime;
	CString	m_strTimeEx;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgDividePortLateTime)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgDividePortLateTime)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CTG_SingleTrain *m_ptrain;
	
	ULONG  m_sborder,m_eborder;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGDIVIDEPORTLATETIME_H__F5AF743E_12EF_47F0_BC8C_85C7365EE318__INCLUDED_)
