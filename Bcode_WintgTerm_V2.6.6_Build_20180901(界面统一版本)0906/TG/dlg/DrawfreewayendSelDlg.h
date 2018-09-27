#if !defined(AFX_DRAWFREEWAYENDSELDLG_H__2D018E60_582C_43FE_9078_68A9F8C14312__INCLUDED_)
#define AFX_DRAWFREEWAYENDSELDLG_H__2D018E60_582C_43FE_9078_68A9F8C14312__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DrawfreewayendSelDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDrawfreewayendSelDlg dialog
#include "resource.h"

class CDrawfreewayendSelDlg : public CDialog
{
// Construction
public:
	int nSel;
	CDrawfreewayendSelDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDrawfreewayendSelDlg)
	enum { IDD = IDD_DLG_DRAW_ENDWAY_SELECT };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDrawfreewayendSelDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDrawfreewayendSelDlg)
	afx_msg void OnContinue();
	afx_msg void OnStop();
	afx_msg void OnCancel();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnClose();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DRAWFREEWAYENDSELDLG_H__2D018E60_582C_43FE_9078_68A9F8C14312__INCLUDED_)
