#if !defined(AFX_FINDTRAINDLG_H__54EBAC1D_B828_4DAB_8C9F_A0F6C67280B6__INCLUDED_)
#define AFX_FINDTRAINDLG_H__54EBAC1D_B828_4DAB_8C9F_A0F6C67280B6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FindTrainDlg.h : header file
//

#include "resource.h"
#include "afxwin.h"

/////////////////////////////////////////////////////////////////////////////
// CFindTrainDlg dialog

class CFindTrainDlg : public CDialog
{
// Construction
public:
	CFindTrainDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CFindTrainDlg)
	enum { IDD = IDD_FIND_TRAIN };
	CString	m_train_id;
	DWORD m_train_index;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFindTrainDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFindTrainDlg)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
protected:
	CButton m_findByTrainIndex;
	CString	train_id;
	DWORD train_index;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FINDTRAINDLG_H__54EBAC1D_B828_4DAB_8C9F_A0F6C67280B6__INCLUDED_)
