#if !defined(AFX_DLGROUTECTRLMODE_H__6C106F3B_584C_4249_9E3D_AC3B585D4DBA__INCLUDED_)
#define AFX_DLGROUTECTRLMODE_H__6C106F3B_584C_4249_9E3D_AC3B585D4DBA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgRouteCtrlMode.h : header file
//
#include "resource.h"
#include "tgdoc.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgRouteCtrlMode dialog

class CDlgRouteCtrlMode : public CDialog
{
// Construction
public:
	CDlgRouteCtrlMode(CTGDoc *pDoc,int nStNo,CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgRouteCtrlMode)
	enum { IDD = IDD_DLG_ROUTE_MODE };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgRouteCtrlMode)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgRouteCtrlMode)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	int nStationNo;
	CTGDoc *m_pDoc;
public:
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGROUTECTRLMODE_H__6C106F3B_584C_4249_9E3D_AC3B585D4DBA__INCLUDED_)
