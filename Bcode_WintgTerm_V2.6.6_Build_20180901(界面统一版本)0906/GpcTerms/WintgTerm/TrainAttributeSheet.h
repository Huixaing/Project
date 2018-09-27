#if !defined(AFX_TRAINATTRIBUTESHEET_H__80DDBE47_C696_473B_94D2_BE7D6ED5D235__INCLUDED_)
#define AFX_TRAINATTRIBUTESHEET_H__80DDBE47_C696_473B_94D2_BE7D6ED5D235__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TrainAttributeSheet.h : header file
//
#include "pagetrainattribute.h"
#include "pagetrainattributeother.h"
#include "wintgdoc.h"

/////////////////////////////////////////////////////////////////////////////
// CTrainAttributeSheet

class CTrainAttributeSheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CTrainAttributeSheet)

// Construction
public:
	CTrainAttributeSheet(UINT nIDCaption,CWintgDoc *pDoc,int schd_type = WORK_SCHEDULE, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CTrainAttributeSheet(LPCTSTR pszCaption,CWintgDoc *pDoc,int schd_type = WORK_SCHEDULE, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

// Attributes
public:
	CWintgDoc *m_pDoc;
    CPageTrainAttribute *m_page1;
    CPageTrainAttributeOther *m_page2;
    int m_nSchdType;
// Operations
public:
	HICON m_hIcon;
	void SetSheetPropsFromDoc(CWintgDoc* pDoc);
	void SetDocFromSheet(CWintgDoc* pDoc);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTrainAttributeSheet)
	public:
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

// Implementation
public:
	void AddPropertyPage();
	virtual ~CTrainAttributeSheet();

	// Generated message map functions
protected:
	//{{AFX_MSG(CTrainAttributeSheet)
//	afx_msg void OnApplyNow();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	void  InitIdentify();
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TRAINATTRIBUTESHEET_H__80DDBE47_C696_473B_94D2_BE7D6ED5D235__INCLUDED_)
