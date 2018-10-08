#if !defined(AFX_TRAINATTRIBUTESHEET_H__80DDBE47_C696_473B_94D2_BE7D6ED5D235__INCLUDED_)
#define AFX_TRAINATTRIBUTESHEET_H__80DDBE47_C696_473B_94D2_BE7D6ED5D235__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TrainAttributeSheet.h : header file
//
#include "pagetrainattribute.h"
#include "pagetrainattributeother.h"
#include "tgdoc.h"

/////////////////////////////////////////////////////////////////////////////
// CTrainAttributeSheet

class CTrainAttributeSheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CTrainAttributeSheet)

// Construction
public:
	CTrainAttributeSheet(CWnd* pParentWnd, LPCTSTR pszCaption = _T("¡–≥µ Ù–‘"), UINT iSelectPage = 0);

// Attributes
private:
    CPageTrainAttribute *m_page1;
    CPageTrainAttributeOther *m_page2;

// Operations
public:
	HICON m_hIcon;


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
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	
public:
	virtual BOOL Create(CWnd* pParentWnd = NULL, DWORD dwStyle = (DWORD)-1, DWORD dwExStyle = 0);
protected:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual void PostNcDestroy();
	void SendTrainUpdate(int nNotChangeRunType);
	void SetCenterPosition();
public:
	CTG_TRAIN m_train;
	CTGDoc *m_pDoc;
	ULONG m_train_index;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TRAINATTRIBUTESHEET_H__80DDBE47_C696_473B_94D2_BE7D6ED5D235__INCLUDED_)
