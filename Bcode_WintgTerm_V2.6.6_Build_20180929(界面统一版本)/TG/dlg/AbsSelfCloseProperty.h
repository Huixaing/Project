#if !defined(AFX_ABSSLEFCLOSEPROPERTY_H__805BD4D8_1540_4140_8D16_D85FD1598B7B__INCLUDED_)
#define AFX_ABSSLEFCLOSEPROPERTY_H__805BD4D8_1540_4140_8D16_D85FD1598B7B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AbsSelfCloseProperty.h : header file
//

#include "resource.h"
#include "tgdoc.h"
/////////////////////////////////////////////////////////////////////////////
// CAbsSelfCloseProperty dialog

class CAbsSelfCloseProperty : public CDialog
{
// Construction
public:
	CAbsSelfCloseProperty(CTGDoc *pDoc,CTg_BLOCK * pFigure,CWnd* pParent = NULL);   // standard constructor
    
// Dialog Data
	//{{AFX_DATA(CAbsSelfCloseProperty)
	enum { IDD = IDD_ABS_SELF_CLOSE_PROPERTY };
	CComboBox	m_TextPos;
	CComboBox	m_nBlockType;
	CComboBox	m_strReason;
	CComboBox	m_nDirect;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAbsSelfCloseProperty)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
    CTGDoc *m_pDoc;
	CTg_BLOCK * m_pFigure;
	void CreateCheckBox();
	// Generated message map functions
	//{{AFX_MSG(CAbsSelfCloseProperty)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	CString m_strreason;
	CButton *m_pcheckBoxList;

	int m_linecount;
	struct LINE_INFO
	{
		char name[32];
		int  lineNo;
		int  dir;
	} m_line_list[128]; 
public:
	afx_msg void OnCbnSelchangeCombo1();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ABSSLOWPROPERTY_H__805BD4D8_1540_4140_8D16_D85FD1598B7B__INCLUDED_)
