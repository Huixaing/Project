#if !defined(AFX_ABSELEFIXGDLG_H__6CD9294A_0167_4ABD_92E3_8252322A328B__INCLUDED_)
#define AFX_ABSELEFIXGDLG_H__6CD9294A_0167_4ABD_92E3_8252322A328B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AbsEleFixProperty.h : header file
//
#include "resource.h"
#include "tgdoc.h"

/////////////////////////////////////////////////////////////////////////////
// CAbsEleFixProperty dialog

class CAbsEleFixProperty : public CDialog
{
// Construction
public:
	CAbsEleFixProperty(CTGDoc *pDoc,CTg_BLOCK * pFigure,CWnd* pParent);   // standard constructor
    ~CAbsEleFixProperty();

	void CreateCheckBox();
		
// Dialog Data
	//{{AFX_DATA(CAbsEleFixProperty)
	enum { IDD = IDD_ABS_ELE_FIX_G };
	CComboBox	m_TextPos;
	CComboBox	m_title;
	CComboBox	m_direction;
	BOOL	m_lockprestation;
	BOOL	m_locknextstation;
	int		m_s_kilo;
	int		m_e_meter;
	int		m_s_meter;
	int		m_e_kilo;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAbsEleFixProperty)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
    CTGDoc *m_pDoc;
	// Generated message map functions
	//{{AFX_MSG(CAbsEleFixProperty)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	CString m_ele_title;
	CTg_BLOCK * m_pFigure;
	CButton *m_pcheckBoxList;

	int m_linecount;
	struct LINE_INFO
	{
		char name[32];
		int  lineNo;
		int  dir;
	} m_line_list[128]; 
	afx_msg void OnCbnSelchangeComboDirection();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ABSELEFIXGDLG_H__6CD9294A_0167_4ABD_92E3_8252322A328B__INCLUDED_)
