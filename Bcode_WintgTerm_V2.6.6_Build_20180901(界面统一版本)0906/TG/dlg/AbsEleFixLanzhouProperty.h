#if !defined(AFX_ABSELEFIXGDLG_H__6CD9294A_0167_4ABD_92E3_8252322A328B__INCLUDED_LZ)
#define AFX_ABSELEFIXGDLG_H__6CD9294A_0167_4ABD_92E3_8252322A328B__INCLUDED_LZ

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AbsEleFixLanzhouProperty.h : header file
//
#include "resource.h"
#include "tgdoc.h"

/////////////////////////////////////////////////////////////////////////////
// CAbsEleFixLanzhouProperty dialog

class CAbsEleFixLanzhouProperty : public CDialog
{
// Construction
public:
	CAbsEleFixLanzhouProperty(CTGDoc *pDoc,CTg_BLOCK * pFigure,CWnd* pParent);   // standard constructor
    ~CAbsEleFixLanzhouProperty();

	void CreateCheckBox();
		
// Dialog Data
	//{{AFX_DATA(CAbsEleFixLanzhouProperty)
	enum { IDD = IDD_ABS_LANZHOU_FIX_PROPERTY };
	CComboBox	m_TextPos;
	CComboBox	m_title;
	CComboBox	m_direction;
	int		m_s_kilo;
	int		m_e_meter;
	int		m_s_meter;
	int		m_e_kilo;
	BOOL	m_lockprestation;
	BOOL	m_locknextstation;
	BOOL	m_UpFlag;
	BOOL	m_DownFlag;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAbsEleFixLanzhouProperty)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
    CTGDoc *m_pDoc;
	// Generated message map functions
	//{{AFX_MSG(CAbsEleFixLanzhouProperty)
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
	void SetPosCheck();
	afx_msg void OnCbnSelchangeComboDirection();
	afx_msg void OnBnClickedRadioSjxy();
	afx_msg void OnBnClickedRadioSjxj();
	afx_msg void OnBnClickedRadioSyxy();
	afx_msg void OnBnClickedRadioSyxj();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ABSELEFIXGDLG_H__6CD9294A_0167_4ABD_92E3_8252322A328B__INCLUDED_LZ)
