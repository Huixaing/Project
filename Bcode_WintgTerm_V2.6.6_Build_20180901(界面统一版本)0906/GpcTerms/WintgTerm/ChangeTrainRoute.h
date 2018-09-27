#if !defined(AFX_CHANGETRAINROUTE_H__2EA8A7ED_184F_4654_8DA2_307B55D89BE2__INCLUDED_)
#define AFX_CHANGETRAINROUTE_H__2EA8A7ED_184F_4654_8DA2_307B55D89BE2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ChangeTrainRoute.h : header file
//
#include "resource.h"
/////////////////////////////////////////////////////////////////////////////
// CChangeTrainRoute dialog

class CChangeTrainRoute : public CDialog
{
// Construction
public:
	int  m_nFlag;
	int  m_nSectionID;
	int  m_nStationID;
	void LoadPasswayInfo();
	CChangeTrainRoute(int sectionid,int station,int flag=0, CWnd* pParent = NULL);   // standard constructor
    int m_nRouteNo;
// Dialog Data
	//{{AFX_DATA(CChangeTrainRoute)
	enum { IDD = IDD_DLG_CHANGE_ROUTE };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChangeTrainRoute)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CChangeTrainRoute)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHANGETRAINROUTE_H__2EA8A7ED_184F_4654_8DA2_307B55D89BE2__INCLUDED_)
