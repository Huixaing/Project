#if !defined(__MYBAR_H__)
#define __MYBAR_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// mybar.h : header file
//

#include "sizecbar.h"

/////////////////////////////////////////////////////////////////////////////
// CMyBar window

#ifndef baseCMyBar
#define baseCMyBar CSizingControlBar
#endif

class CMyBar : public baseCMyBar
{
// Construction
public:
    CMyBar();

// Attributes
public:

// Overridables
    virtual void OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler);
	CListBox  m_ListBox;
	CComboBox m_ComboStation,m_ComboTrain;

 	Pstruct _tagConflictInformNode *curr;
    int  m_nCurSel;
	int OsDeselectConflict(Pstruct _tagConflictInformNode *selectconflict);
 	HANDLE m_hPrintSemaphore;

// Operations
public:
	int m_nListItemNum;
    
// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CMyBar)
    //}}AFX_VIRTUAL

// Implementation
public:
    virtual ~CMyBar();

protected:
//    CButton m_wndChild;
    CFont   m_font;

    // Generated message map functions
protected:
    //{{AFX_MSG(CMyBar)
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDblclkList1();
	afx_msg void OnSelchangeList1();
// 	afx_msg LRESULT OnAlarmMessage(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnConflictPurge(WPARAM wParam, LPARAM lParam);
 	afx_msg LRESULT OnConflictInsert(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnConflictDelete(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(__MYBAR_H__)
