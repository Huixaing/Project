#pragma once

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// mybar.h : header file
//
/////////////////////////////////////////////////////////////////////////////
// mybar window
#include "sizecbar.h"
#include <afxinet.h>

#include <afxdtctl.h>
#include "utility/xmlview.h"

//////////////////////////////////////////////////////
class CMMBar : public CSizingControlBar
{
// Construction
public:
	CMMBar();

// Attributes
public:
	void UpdateShow(const char *xml,const char *node);
	void UpdateShowJieShi(const char *xml,const char *node);
	void UpdateShowTrnInfo(const char *xml,const char *node);
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMMBar)
	//}}AFX_VIRTUAL

// Implementation
public:
	
	virtual ~CMMBar();

	// Generated message map functions
protected:
	//{{AFX_MSG(CMMBar)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	CSize CalcDynamicLayout(int   nLength,   DWORD   nMode);
	BOOL  GetViewInfo(CString &dest,const char *xml,const char *node);
public:
	DECLARE_MESSAGE_MAP()
protected:
	CStatic      m_caption;
	CXMLDataGrid m_grid;

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.


