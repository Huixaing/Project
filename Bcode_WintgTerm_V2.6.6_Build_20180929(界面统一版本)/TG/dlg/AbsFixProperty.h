#if !defined(AFX_ABSFIXPROPERTY_H__62820384_1285_4F65_9961_62C442ED5B23__INCLUDED_)
#define AFX_ABSFIXPROPERTY_H__62820384_1285_4F65_9961_62C442ED5B23__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AbsFixProperty.h : header file
//
#include "resource.h"
#include "tgdoc.h"

/////////////////////////////////////////////////////////////////////////////
// CAbsFixProperty dialog

class CAbsFixProperty : public CDialog
{
// Construction
public:
	CAbsFixProperty(CTGDoc *pDoc,CTg_BLOCK * pFigure,CWnd* pParent);   // standard constructor
	~CAbsFixProperty();

	void CreateCheckBox();
		
// Dialog Data
	//{{AFX_DATA(CAbsFixProperty)
	enum { IDD = IDD_ABS_FIX_PROPERTY };
	CComboBox	m_strReason;
	CComboBox	m_TextPos;
	CComboBox	m_LinePos;
	CComboBox	m_nDirect;
	CString     m_strreason;
	BOOL	m_lockprestation;
	BOOL	m_locknextstation;
	int		m_s_kilo;
	int		m_e_meter;
	int		m_s_meter;
	int		m_e_kilo;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAbsFixProperty)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
    CTGDoc *m_pDoc;
	// Generated message map functions
	//{{AFX_MSG(CAbsFixProperty)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	CString m_str_reason;
	CTg_BLOCK * m_pFigure;
	CButton *m_pcheckBoxList;

	int m_linecount;
	struct LINE_INFO
	{
		char name[32];
		int  lineNo;
		int  dir;
	} m_line_list[128]; 
	afx_msg void OnCbnSelchangeCombo1();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ABSFIXPROPERTY_H__62820384_1285_4F65_9961_62C442ED5B23__INCLUDED_)
