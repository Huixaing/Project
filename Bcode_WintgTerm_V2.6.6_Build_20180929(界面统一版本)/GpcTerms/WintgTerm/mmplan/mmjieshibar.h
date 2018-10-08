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
class CMMJieshiBar : public CSizingControlBar
{
// Construction
public:
	CMMJieshiBar();

// Attributes
public:
	void UpdateShowJieShi(const char *xml,const char *node);
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMMJieshiBar)
	//}}AFX_VIRTUAL

// Implementation
public:
	
	virtual ~CMMJieshiBar();

	// Generated message map functions
protected:
	//{{AFX_MSG(CMMJieshiBar)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	CSize CalcDynamicLayout(int   nLength,   DWORD   nMode);
	BOOL  GetViewInfo(CString &dest,const char *xml,const char *node);
public:
	DECLARE_MESSAGE_MAP()
protected:
	CStatic m_jieshicap;
	CEdit  m_edit;

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.


