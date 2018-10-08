#if !defined(AFX_ABSSLOWPROPERTY_H__805BD4D8_1540_4140_8D16_D85FD1598B7B__INCLUDED_)
#define AFX_ABSSLOWPROPERTY_H__805BD4D8_1540_4140_8D16_D85FD1598B7B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AbsSlowProperty.h : header file
//

#include "WintgDoc.h"

/////////////////////////////////////////////////////////////////////////////
// CAbsSlowProperty dialog
#include "absmark.h"

class CAbsSlowProperty : public CDialog
{
// Construction
public:
	void InitIdentify();
	CAbsSlowProperty(CWintgDoc *pDoc,CAbsMark * pFigure,CWnd* pParent = NULL);   // standard constructor
    CAbsMark * m_pFigure;

// Dialog Data
	//{{AFX_DATA(CAbsSlowProperty)
	enum { IDD = IDD_ABS_SLOW_PROPERTY };
	CComboBox	m_TextPos;
	CComboBox	m_LinePos;
	CComboBox	m_nBlockType;
	CComboBox	m_strReason;
	CComboBox	m_nDirect;
	int		m_nDistance;
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
    CWintgDoc *m_pDoc;
	// Generated message map functions
	//{{AFX_MSG(CAbsSlowProperty)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ABSSLOWPROPERTY_H__805BD4D8_1540_4140_8D16_D85FD1598B7B__INCLUDED_)
