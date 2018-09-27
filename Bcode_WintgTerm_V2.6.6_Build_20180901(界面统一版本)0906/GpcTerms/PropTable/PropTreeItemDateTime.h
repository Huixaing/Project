#if !defined(AFX_PROPTREEITEMEDIT_H__642536B1_1162_4F99_B09D_9B1BD2CF88B6asd__INCLUDED_)
#define AFX_PROPTREEITEMEDIT_H__642536B1_1162_4F99_B09D_9B1BD2CF88B6asd__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PropTreeItemEdit.h : header file
//
//  Copyright (C) 1998-2001 Scott Ramsay
//	sramsay@gonavi.com
//	http://www.gonavi.com
//
//  This material is provided "as is", with absolutely no warranty expressed
//  or implied. Any use is at your own risk.
// 
//  Permission to use or copy this software for any purpose is hereby granted 
//  without fee, provided the above notices are retained on all copies.
//  Permission to modify the code and to distribute modified code is granted,
//  provided the above notices are retained, and a notice that the code was
//  modified is included with the above copyright notice.
// 
//	If you use this code, drop me an email.  I'd like to know if you find the code
//	useful.

#include "PropTreeItem.h"

/////////////////////////////////////////////////////////////////////////////
// CPropTreeItemEdit window

class PROPTREE_API CPropTreeItemDateTime : public CDateTimeCtrl, public CPropTreeItem
{
// Construction
public:
	CPropTreeItemDateTime();
	virtual ~CPropTreeItemDateTime();

// Attributes
public:

	// Retrieve the item's attribute value
	virtual LPARAM GetItemValue();
	virtual int  GetRelaItemValue(); // cuihu INT value
	// Set the item's attribute value
	virtual void SetItemValue(LPARAM lParam);
	virtual void SetRelaItemValue(int va);
	// Called when attribute area has changed size
	virtual void OnMove();

	// Called when the item needs to refresh its data
	virtual void OnRefresh();

	// Called when the item needs to commit its changes
	virtual void OnCommit();

	// Called to activate the item
	virtual void OnActivate();

	BOOL CreateCtrl(DWORD dwStyle);
	
	virtual void DrawAttribute(CDC* pDC, const RECT& rc);
protected:
	CTime   m_time;
	time_t  m_tTimeValue;
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPropTreeItemDateTime)
	//}}AFX_VIRTUAL

// Implementation
public:

	// Generated message map functions
protected:
	//{{AFX_MSG(CPropTreeItemDateTime)
	afx_msg UINT OnGetDlgCode();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKillfocus(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnSelchange(NMHDR *pNMHDR, LRESULT *pResult); 
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPTREEITEMEDIT_H__642536B1_1162_4F99_B09D_9B1BD2CF88B6__INCLUDED_)
