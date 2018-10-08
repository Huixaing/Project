#if !defined(AFX_DLGABOUT_H__7EDBA8CA_C237_4DBC_A702_26E2000B9065__INCLUDED_)
#define AFX_DLGABOUT_H__7EDBA8CA_C237_4DBC_A702_26E2000B9065__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgAbout.h : header file
//
/////////////////////////////////////////////////////////////////////////////
// CDlgAbout dialog
#include "resource.h"
#include ".\Logo.h"
class CDlgAbout : public CDialog
{
// Construction
public:
	CDlgAbout(CWnd* pParent = NULL);   // standard constructor
	CBitmap mBmp;
// Dialog Data
	//{{AFX_DATA(CDlgAbout)
	enum { IDD = IDD_ABOUT };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgAbout)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgAbout)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CLogo pic;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGABOUT_H__7EDBA8CA_C237_4DBC_A702_26E2000B9065__INCLUDED_)
