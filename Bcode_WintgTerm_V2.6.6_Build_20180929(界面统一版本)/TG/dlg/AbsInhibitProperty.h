#if !defined(AFX_ABSINHIBITPROPERTY_H__F20D795E_8350_4F28_8157_CFD872B9E7A4__INCLUDED_)
#define AFX_ABSINHIBITPROPERTY_H__F20D795E_8350_4F28_8157_CFD872B9E7A4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AbsInhibitProperty.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAbsInhibitProperty dialog
#include "resource.h"
#include "tgdoc.h"
#include "gridctrl.h"
#include "gridcellcheck.h"

class CAbsInhibitProperty : public CDialog
{
// Construction
public:
	void InitIdentify();
	CAbsInhibitProperty(CTGDoc *pDoc,CTg_BLOCK * pFigure,CWnd* pParent = NULL);   // standard constructor
    ~CAbsInhibitProperty();
// Dialog Data
	//{{AFX_DATA(CAbsInhibitProperty)
	enum { IDD = IDD_ABS_INHIBIT_PROPERTY };
	CComboBox	m_LinePos;
	CComboBox	m_TextPos;
	CComboBox	m_strReason;
	CComboBox	m_nDirect;
	int		m_s_kilo;
	int		m_e_meter;
	int		m_s_meter;
	int		m_e_kilo;
	CString	m_strreason;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAbsInhibitProperty)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
    CTGDoc *m_pDoc;
	CButton *m_pcheckBoxList;
	CTg_BLOCK * m_pFigure;

	int m_linecount;
	struct LINE_INFO
	{
		char name[32];
		int  lineNo;
		int  dir;
	}m_line_list[128];
	
	void CreateCheckBox();
	// Generated message map functions
	//{{AFX_MSG(CAbsInhibitProperty)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnCbnSelchangeCombo1();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ABSINHIBITPROPERTY_H__F20D795E_8350_4F28_8157_CFD872B9E7A4__INCLUDED_)
