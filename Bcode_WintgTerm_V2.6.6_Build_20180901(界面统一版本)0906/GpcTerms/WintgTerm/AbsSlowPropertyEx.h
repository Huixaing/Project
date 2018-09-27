#if !defined(AFX_ABSSLOWPROPERTYEX_H__B70F7322_9656_40C3_8AE8_6AA4B45593D7__INCLUDED_)
#define AFX_ABSSLOWPROPERTYEX_H__B70F7322_9656_40C3_8AE8_6AA4B45593D7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AbsSlowPropertyEx.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAbsSlowPropertyEx dialog

#include "WintgDoc.h"
#include "absmark.h"

#define WIDTHSPACE  6 
#define HEIGHTSPACE 4 

class CAbsSlowPropertyEx : public CDialog
{
// Construction
public:
	void InitIdentify();
	CAbsSlowPropertyEx(CWintgDoc *pDoc,CAbsMark * pFigure,CWnd* pParent = NULL);   // standard constructor
	~CAbsSlowPropertyEx();
    CAbsMark * m_pFigure;

// Dialog Data
	//{{AFX_DATA(CAbsSlowPropertyEx)
	enum { IDD = IDD_ABS_SLOW_PROPERTYEX };
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
	//{{AFX_VIRTUAL(CAbsSlowPropertyEx)
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
	//{{AFX_MSG(CAbsSlowPropertyEx)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ABSSLOWPROPERTYEX_H__B70F7322_9656_40C3_8AE8_6AA4B45593D7__INCLUDED_)
