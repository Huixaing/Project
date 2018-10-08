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
class CMMDcmdBar : public CSizingControlBar
{
// Construction
public:
	CMMDcmdBar();

// Attributes
public:
	void UpdateShow(const char *xml,const char *rootnode);
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMMDcmdBar)
	//}}AFX_VIRTUAL

// Implementation
public:
	
	virtual ~CMMDcmdBar();

	// Generated message map functions
protected:
	//{{AFX_MSG(CMMDcmdBar)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	afx_msg void OnGridSelectChange(NMHDR* pNMHDR, LRESULT* pResult); 
 	//}}AFX_MSG
public:
	DECLARE_MESSAGE_MAP()
protected:
	CXMLDataGrid m_grid;
	CEdit        m_edit;
	CStatic      m_caption;
public:
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.


