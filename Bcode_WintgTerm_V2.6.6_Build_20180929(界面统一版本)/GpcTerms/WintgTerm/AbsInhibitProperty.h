#if !defined(AFX_ABSINHIBITPROPERTY_H__F20D795E_8350_4F28_8157_CFD872B9E7A4__INCLUDED_)
#define AFX_ABSINHIBITPROPERTY_H__F20D795E_8350_4F28_8157_CFD872B9E7A4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AbsInhibitProperty.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAbsInhibitProperty dialog

#include "absmark.h"
#include "wintgdoc.h"

#define WIDTHSPACE  6 
#define HEIGHTSPACE 4 


class CAbsInhibitProperty : public CDialog
{
// Construction
public:
	void InitIdentify();
	CAbsInhibitProperty(CWintgDoc *pDoc,CAbsMark * pFigure,CWnd* pParent = NULL);   // standard constructor
    ~CAbsInhibitProperty();

	CAbsMark * m_pFigure;
    int abs_line_count;
	int abs_line[16];
	int m_nCurInhibitSel,m_nCurAllSel;
// Dialog Data
	//{{AFX_DATA(CAbsInhibitProperty)
	enum { IDD = IDD_ABS_INHIBIT_PROPERTY };
	CListBox	m_ctrlInhibitAbsLine;
	CListBox	m_ctrlAllAbsLine;
	CComboBox	m_LinePos;
	CComboBox	m_TextPos;
	CComboBox	m_strReason;
	CComboBox	m_nDirect;
	int		m_nDistance;
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
	BOOL GetLineName(int lineNo,CString& lineName);
    CWintgDoc *m_pDoc;
	CButton *m_pcheckBoxList;
	
	//added by qianguorong 2003-07-29
	int m_linecount;
	int m_line_list[64]; //实际的列出来的线号
	//added by qianguorong 2003-07-29 ends here

	void CreateCheckBox();
	// Generated message map functions
	//{{AFX_MSG(CAbsInhibitProperty)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnAdd();
	afx_msg void OnDel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ABSINHIBITPROPERTY_H__F20D795E_8350_4F28_8157_CFD872B9E7A4__INCLUDED_)
