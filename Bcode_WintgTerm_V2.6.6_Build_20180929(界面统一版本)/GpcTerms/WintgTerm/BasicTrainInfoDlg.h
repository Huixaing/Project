#if !defined(AFX_BASICTRAININFODLG_H__5734795A_8F97_4774_B0B3_2D42D30472F4__INCLUDED_)
#define AFX_BASICTRAININFODLG_H__5734795A_8F97_4774_B0B3_2D42D30472F4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BasicTrainInfoDlg.h : header file
//
#include "wintgdoc.h"
#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// CBasicTrainInfoDlg dialog

class CBasicTrainInfoDlg : public CDialog
{
// Construction
public:
	CBasicTrainInfoDlg(CWintgDoc *pDoc,CWnd* pParent = NULL);   // standard constructor
    CWintgDoc *m_pDoc;
// Dialog Data
	//{{AFX_DATA(CBasicTrainInfoDlg)
	enum { IDD = IDD_DLG_BASIC_PARA };
	int		m_exit;
	int		m_entry;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBasicTrainInfoDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CBasicTrainInfoDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BASICTRAININFODLG_H__5734795A_8F97_4774_B0B3_2D42D30472F4__INCLUDED_)
