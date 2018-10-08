#if !defined(AFX_ABSROUTEDLG_H__71883463_434B_11D5_9C5A_0050BA679B44__INCLUDED_)
#define AFX_ABSROUTEDLG_H__71883463_434B_11D5_9C5A_0050BA679B44__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AbsRouteDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAbsRouteDlg dialog

class CAbsRouteDlg : public CDialog
{
// Construction
public:
	int nDirect;
	CAbsRouteDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAbsRouteDlg)
	enum { IDD = IDD_ABS_INHIBIT_G };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAbsRouteDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAbsRouteDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ABSROUTEDLG_H__71883463_434B_11D5_9C5A_0050BA679B44__INCLUDED_)
