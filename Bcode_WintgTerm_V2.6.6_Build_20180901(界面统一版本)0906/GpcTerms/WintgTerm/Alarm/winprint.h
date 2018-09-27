#if !defined(AFX_WINPRINT_H__73463734_86F6_11D1_9D01_000021000CE6__INCLUDED_)
#define AFX_WINPRINT_H__73463734_86F6_11D1_9D01_000021000CE6__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Resource.h"

// WinPrint.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CWinPrint thread

class  CWinPrint : public CWinThread
{
	DECLARE_DYNCREATE(CWinPrint)
public:
	CWinPrint();           // protected constructor used by dynamic creation
	virtual ~CWinPrint();
// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWinPrint)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual BOOL OnIdle(LONG lCount);
	//}}AFX_VIRTUAL

// Implementation
protected:


	// Generated message map functions
	//{{AFX_MSG(CWinPrint)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////





/////////////////////////////////////////////////////////////////////////////
// CPrintFrame frame

class CPrintFrame : public CFrameWnd
{
	DECLARE_DYNCREATE(CPrintFrame)
public:
	CPrintFrame();           // protected constructor used by dynamic creation
	virtual ~CPrintFrame();
// Attributes
public:
	BOOL m_bAlwaysTopMost;
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPrintFrame)
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	//}}AFX_VIRTUAL
private:
	CToolBar m_tb;

// Implementation
protected:


	// Generated message map functions
	//{{AFX_MSG(CPrintFrame)
	afx_msg void OnClose();
	afx_msg void OnAlwaysTopmost();
	afx_msg void OnAlwaysTopmost_UpdateUI(CCmdUI* pCmdUI);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg LRESULT OnShowTopmost(WPARAM wParam, LPARAM lParam); 
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////





/////////////////////////////////////////////////////////////////////////////
// CInfoForm form view
#include "SysAlarmPage.h"

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif


class CInfoForm : public CFormView
{
protected:
	CInfoForm();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CInfoForm)

// Form Data
public:
	//{{AFX_DATA(CInfoForm)
	enum { IDD = IDD_FORM_INFO };
	CListBox 	m_listGeneralLog;
	//}}AFX_DATA
	CString  m_szFilterKey;
// Attributes
private:
	//CPropertySheet mSheet;
	CSysAlarmPage  mSysAlarmPage;

// Operations
public:
//DEL	HANDLE m_hPrintSemaphore;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInfoForm)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CInfoForm();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CInfoForm)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLayoutlog();
	afx_msg void OnLayoutlog_UpdateUI(CCmdUI* pCmdUI);
	afx_msg void OnGenerallog();
	afx_msg void OnGenerallog_UpdateUI(CCmdUI* pCmdUI);
	afx_msg void OnCtcalarmlog();
	afx_msg void OnCtcalarmlog_UpdateUI(CCmdUI* pCmdUI); 
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////




//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WINPRINT_H__73463734_86F6_11D1_9D01_000021000CE6__INCLUDED_)
