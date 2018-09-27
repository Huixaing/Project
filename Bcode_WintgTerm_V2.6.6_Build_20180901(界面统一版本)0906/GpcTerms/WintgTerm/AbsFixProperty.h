#if !defined(AFX_ABSFIXPROPERTY_H__62820384_1285_4F65_9961_62C442ED5B23__INCLUDED_)
#define AFX_ABSFIXPROPERTY_H__62820384_1285_4F65_9961_62C442ED5B23__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AbsFixProperty.h : header file
//

#include "absmark.h"
#include "WintgDoc.h"

#define WIDTHSPACE  6 
#define HEIGHTSPACE 4 

/////////////////////////////////////////////////////////////////////////////
// CAbsFixProperty dialog

class CAbsFixProperty : public CDialog
{
// Construction
public:
	BOOL GetLineName(int lineNo, CString &lineName);
	CButton *m_pcheckBoxList;
	
	//added by qianguorong 2003-07-29
	int m_linecount;
	int m_line_list[64]; //实际的列出来的线号
	//added by qianguorong 2003-07-29 ends here

	void CreateCheckBox();
	void InitIdentify();
//	CAbsFixProperty(CWnd* pParent = NULL);   // standard constructor
	CAbsFixProperty(CWintgDoc *pDoc,CAbsMark * pFigure,CWnd* pParent = NULL);   // standard constructor
	~CAbsFixProperty();
    CAbsMark * m_pFigure;

// Dialog Data
	//{{AFX_DATA(CAbsFixProperty)
	enum { IDD = IDD_ABS_FIX_PROPERTY };
	CComboBox	m_strReason;
	CComboBox	m_TextPos;
	CComboBox	m_LinePos;
	CComboBox	m_nDirect;
	CString     m_strreason;

	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAbsFixProperty)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
    CWintgDoc *m_pDoc;
	// Generated message map functions
	//{{AFX_MSG(CAbsFixProperty)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	CString m_str_reason;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ABSFIXPROPERTY_H__62820384_1285_4F65_9961_62C442ED5B23__INCLUDED_)
