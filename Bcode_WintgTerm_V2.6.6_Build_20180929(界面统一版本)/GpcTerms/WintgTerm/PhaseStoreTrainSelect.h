#if !defined(AFX_PHASESTORETRAINSELECT_H__91792E87_0914_4EFE_B1BD_6C8EFCB12CF2__INCLUDED_)
#define AFX_PHASESTORETRAINSELECT_H__91792E87_0914_4EFE_B1BD_6C8EFCB12CF2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PhaseStoreTrainSelect.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPhaseStoreTrainSelect dialog

class CPhaseStoreTrainSelect : public CDialog
{
// Construction
public:
	int SetPhase(int index);
	int m_nPhase;
	int banbie;
	CPhaseStoreTrainSelect(int nbanbie,CWnd* pParent = NULL);   // standard constructor
    enum  STACAR_PHASE {
		STACAR_PHASE_0,
			STACAR_PHASE_6,
			STACAR_PHASE_12,
			STACAR_PHASE_18,
			STACAR_PHASE_CURRENT
	};
// Dialog Data
	//{{AFX_DATA(CPhaseStoreTrainSelect)
	enum { IDD = IDD_DLG_SAVE_SELECT };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPhaseStoreTrainSelect)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPhaseStoreTrainSelect)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnRadio1();
	afx_msg void OnRadio2();
	afx_msg void OnRadio3();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PHASESTORETRAINSELECT_H__91792E87_0914_4EFE_B1BD_6C8EFCB12CF2__INCLUDED_)
