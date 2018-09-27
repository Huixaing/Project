#if !defined(AFX_DLGBASICPARAM_H__47EBEFA2_7572_46A5_9A72_B87204BA93A0__INCLUDED_)
#define AFX_DLGBASICPARAM_H__47EBEFA2_7572_46A5_9A72_B87204BA93A0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgDDYNameQuery.h : header file
//
#include "resource.h"
class CTgDocument;
/////////////////////////////////////////////////////////////////////////////
// CDlgDDYNameQuery dialog

class CDlgDDYNameQuery : public CDialog
{
// Construction
public:
	CDlgDDYNameQuery(CTgDocument *pDoc, int conid, long shiftid, CWnd* pParent);   // standard constructor
	~CDlgDDYNameQuery();
// Dialog Data
	//{{AFX_DATA(CDlgDDYNameQuery)
	enum { IDD = IDD_DLG_REQ_DDYNAME };
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgDDYNameQuery)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CDlgDDYNameQuery)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	
	LRESULT OnCommMsg(WPARAM wParam,LPARAM lParam);
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnClose();

private:
	void Query_GetDdyName();
	CTgDocument *m_pDoc;
	CString m_ddyName1, m_ddyName2;
	int m_conid;
	long m_shiftid;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGBASICPARAM_H__47EBEFA2_7572_46A5_9A72_B87204BA93A0__INCLUDED_)
