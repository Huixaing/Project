#if !defined(AFX_INPUTHATTRAIN_H__5B0FB156_7983_4D2E_8373_2D9AB92746AC__INCLUDED_)
#define AFX_INPUTHATTRAIN_H__5B0FB156_7983_4D2E_8373_2D9AB92746AC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// InputHatTrain.h : header file
//
#include "..\WintgTerm\resource.h"
#include "..\mvc\tgdoc.h"
/////////////////////////////////////////////////////////////////////////////
// CInputHatTrain dialog

class CInputHatTrain : public CDialog
{
// Construction
public:
	CInputHatTrain(CTGDoc *pDoc,CWnd* pParent = NULL);   // standard constructor
	USHORT m_usCommQueueHandle;
//	CInputHatTrain(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CInputHatTrain)
	enum { IDD = IDD_INPUT_HAT_TRAIN };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInputHatTrain)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CTGDoc *m_pDoc;

	// Generated message map functions
	//{{AFX_MSG(CInputHatTrain)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_INPUTHATTRAIN_H__5B0FB156_7983_4D2E_8373_2D9AB92746AC__INCLUDED_)
