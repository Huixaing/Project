#if !defined(AFX_ABSSLOWPROPERTYEX_H__B70F7322_9656_40C3_8AE8_6AA4B45593D7__INCLUDED_)
#define AFX_ABSSLOWPROPERTYEX_H__B70F7322_9656_40C3_8AE8_6AA4B45593D7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AbsSlowProperty.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAbsSlowProperty dialog

#include "tgdoc.h"
#include "resource.h"

class CAbsSlowProperty : public CDialog
{
// Construction
public:
	void InitIdentify();
	CAbsSlowProperty(CTGDoc *pDoc,CTg_BLOCK * pFigure,CWnd* pParent = NULL);   // standard constructor
	~CAbsSlowProperty();
    
// Dialog Data
	//{{AFX_DATA(CAbsSlowProperty)
	enum { IDD = IDD_ABS_SLOW_PROPERTYEX };
	CComboBox	m_TextPos;
	CComboBox	m_LinePos;
	CComboBox	m_nBlockType;
	CComboBox	m_strReason;
	CComboBox	m_nDirect;
	int		m_s_kilo;
	int		m_nSpeed;
	int		m_s_meter;
	int		m_e_kilo;
	int		m_e_meter;
	int		m_append_time;
	CString	m_strreason;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAbsSlowProperty)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
    CTGDoc *m_pDoc;
	CTg_BLOCK *m_pFigure;
	CButton *m_pcheckBoxList;

	int m_linecount;
	struct LINE_INFO
	{
		char name[32];
		int  lineNo;
		int  dir;
	}m_line_list[128]; 
	
	void CreateCheckBox();
	// Generated message map functions
	//{{AFX_MSG(CAbsSlowProperty)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnCbnSelchangeCombo1();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ABSSLOWPROPERTYEX_H__B70F7322_9656_40C3_8AE8_6AA4B45593D7__INCLUDED_)
