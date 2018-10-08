#if !defined(AFX_DLGCREWNOTICE_H__AFAA8796_2D5B_4700_B510_E542806C79CD__INCLUDED_)
#define AFX_DLGCREWNOTICE_H__AFAA8796_2D5B_4700_B510_E542806C79CD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgCrewNotice.h : header file
//

#include "../GridCtrl_src/GridCtrl.h"
#include "../GridCtrl_src/GridCellCheck.h"
#include "../uneditableGridCell.h"
#include "resource.h"

#include <vector>
class CTgDocument;
/////////////////////////////////////////////////////////////////////////////
// CDlgCrewNotice dialog
class CDlgCrewNotice : public CDialog
{
// Construction
public:
	CDlgCrewNotice(CTgDocument* pDoc, CWnd* pParent = NULL);   // standard constructor
	~CDlgCrewNotice();
// Dialog Data
	//{{AFX_DATA(CDlgCrewNotice)
	enum { IDD = IDD_CREW_PLAN_NOTICE_DLG };
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgCrewNotice)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	BOOL ResetTrainSelected();
    void initialGridCtrl();
	void fillGrid();	
	CGridCtrl * m_pGridCtrl;
	
	// Generated message map functions
	//{{AFX_MSG(CDlgCrewNotice)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnButtonSelall();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	void SetXML(const char* token, const char* xml);
public:
	std::vector<ULONG> mVecGTID;

private:
	CTgDocument* m_pDoc;
	int m_nSoundCout;
public:
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnClose();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGCREWNOTICE_H__AFAA8796_2D5B_4700_B510_E542806C79CD__INCLUDED_)
