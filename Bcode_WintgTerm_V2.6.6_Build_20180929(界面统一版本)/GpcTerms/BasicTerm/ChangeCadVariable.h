#if !defined(AFX_CHANGECADVARIABLE_H__843FB964_A749_4AD3_89D4_C79D0B5DF8E8__INCLUDED_)
#define AFX_CHANGECADVARIABLE_H__843FB964_A749_4AD3_89D4_C79D0B5DF8E8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ChangeCadVariable.h : header file
//

#include "sectionruntimepage.h"
#include "inputsectioninform.h"
#include "wintgdoc.h"


#include "privateproperty.h"
/////////////////////////////////////////////////////////////////////////////
// CChangeCadVariable dialog

class CChangeCadVariable : public CDialog
{
// Construction
public:
	CChangeCadVariable(CWintgDoc *pDoc,CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CChangeCadVariable)
	enum { IDD = IDD_DLG_EDIT_CAD_VARIABLE };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA
	CWintgDoc *m_pDoc;
	CSectionRuntimePage *m_ctrlSectionRuntimePage;
	CInputSectionInform *m_ctrlInputSectionInformPage;

	CPrivatePropertySheet m_ctrlPropertySheet;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChangeCadVariable)
	public:
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CChangeCadVariable)
	virtual BOOL OnInitDialog();
	afx_msg void OnEditcadCancel();
	afx_msg void OnEditcadSavelocal();
	afx_msg void OnEditcadSaveremote();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHANGECADVARIABLE_H__843FB964_A749_4AD3_89D4_C79D0B5DF8E8__INCLUDED_)
