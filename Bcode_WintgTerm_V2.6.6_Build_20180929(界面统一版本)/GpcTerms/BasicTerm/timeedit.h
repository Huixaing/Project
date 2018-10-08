#if !defined(AFX_TIMEEDIT_H__A93D9B23_C1E3_11D1_9D40_0000C05319CE__INCLUDED_)
#define AFX_TIMEEDIT_H__A93D9B23_C1E3_11D1_9D40_0000C05319CE__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// TimeEdit.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTimeEdit window

class  CTimeEdit : public CEdit
{
// Construction
public:
	CTimeEdit();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTimeEdit)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CTimeEdit();

	// Generated message map functions
protected:
	//{{AFX_MSG(CTimeEdit)
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TIMEEDIT_H__A93D9B23_C1E3_11D1_9D40_0000C05319CE__INCLUDED_)
