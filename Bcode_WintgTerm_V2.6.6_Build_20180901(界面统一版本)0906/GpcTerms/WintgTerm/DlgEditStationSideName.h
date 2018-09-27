#if !defined(AFX_DLGEDITSTATIONSIDENAME_H__3E02DF85_26F6_40E4_B941_C48847D2CAAB__INCLUDED_)
#define AFX_DLGEDITSTATIONSIDENAME_H__3E02DF85_26F6_40E4_B941_C48847D2CAAB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgEditStationSideName.h : header file
//
#include "QueryStationCurrentStoreTrain.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgEditStationSideName dialog

class CDlgEditStationSideName : public CDialog
{
// Construction
public:
	CDlgEditStationSideName(struct _tagStationStoreTrain * pInfo,CWnd* pParent = NULL);   // standard constructor
	struct _tagStationStoreTrain *m_pInfo;

// Dialog Data
	//{{AFX_DATA(CDlgEditStationSideName)
	enum { IDD = IDD_EIDT_STATION_SIDE };
	CListBox	m_ListSideName;
	CString	m_strSideName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgEditStationSideName)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
    int m_nCurSel;
	// Generated message map functions
	//{{AFX_MSG(CDlgEditStationSideName)
	virtual BOOL OnInitDialog();
	afx_msg void OnAdd();
	afx_msg void OnDel();
	virtual void OnOK();
	afx_msg void OnEdit();
	afx_msg void OnSelchangeList();
	afx_msg void OnUp();
	afx_msg void OnDown();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGEDITSTATIONSIDENAME_H__3E02DF85_26F6_40E4_B941_C48847D2CAAB__INCLUDED_)
