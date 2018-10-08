#if !defined(AFX_MYBAR_H__076239B0_BC34_446B_A632_0D3EDE574BD9__INCLUDEDXX_)
#define AFX_MYBAR_H__076239B0_BC34_446B_A632_0D3EDE574BD9__INCLUDEDXX_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// mybar.h : header file
//
/////////////////////////////////////////////////////////////////////////////
// mybar window
#include "sizecbar.h"
#include "SheetTGProperty.h"
class CTgView;
#include <afxinet.h>

#include <afxdtctl.h>


//////////////////////////////////////////////////////
class CTgBar : public CSizingControlBar
{
// Construction
public:
	CTgBar();

// Attributes
public:
	void UpdateShowData(CTgView *pview,const CTG_LogicObject *pobj,int para,BOOL switchobject, int conid);
	void SetCurPage(int page_index);
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTgBar)
	//}}AFX_VIRTUAL

// Implementation
public:
	
	virtual ~CTgBar();

	// Generated message map functions
protected:
	//{{AFX_MSG(CTgBar)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnDestroy();
	//}}AFX_MSG
public:
	DECLARE_MESSAGE_MAP()
protected:
	CSheetTGProperty m_sheet;
public:
	void SetDocument(CTgDocument *pdoc) {m_sheet.SetDocument(pdoc);}
	void SetReadOnly(BOOL va) {m_sheet.SetReadOnly(va);} 
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MYBAR_H__076239B0_BC34_446B_A632_0D3EDE574BD9__INCLUDED_)
