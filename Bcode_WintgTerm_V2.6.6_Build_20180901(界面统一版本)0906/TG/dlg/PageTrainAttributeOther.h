#if !defined(AFX_PAGETRAINATTRIBUTEOTHER_H__BC3A6724_97E5_4F9C_A899_2379C91C0116__INCLUDED_)
#define AFX_PAGETRAINATTRIBUTEOTHER_H__BC3A6724_97E5_4F9C_A899_2379C91C0116__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PageTrainAttributeOther.h : header file
//
#include "resource.h"
#include "tgdoc.h"

/////////////////////////////////////////////////////////////////////////////
// CPageTrainAttributeOther dialog

class CPageTrainAttributeOther : public CPropertyPage
{
	DECLARE_DYNCREATE(CPageTrainAttributeOther)

// Construction
public:
	void InitIdentify();
  	CPageTrainAttributeOther();
  	CPageTrainAttributeOther(CTG_TRAIN* train, CTGDoc *pDoc);
	~CPageTrainAttributeOther();
	CTGDoc *m_pDoc;
    
    CTG_TRAIN *m_pTrain;
	int m_nCurSel;
	int m_nCC,m_nCX,m_nJJ,m_nZY;
	int m_nChange;

	TgTRAIN_MORE_INFO* m_pTrainInfo;
    
    bool bNotChanged;
// Dialog Data
	//{{AFX_DATA(CPageTrainAttributeOther)
	enum { IDD = IDD_PGE_TRAIN_ATT2 };
	CComboBox	m_combo_zy;
	CComboBox	m_combo_jj;
	CComboBox	m_combo_cx;
	CComboBox	m_combo_cc;
	BOOL	m_check1;
	BOOL	m_check2;
	BOOL	m_check3;
	BOOL	m_check4;
	BOOL	m_check5;
	BOOL	m_check6;
	CString	m_behind_no;
	CString	m_e_a_time;
	CString	m_engine;
	CString	m_f_a_time;
	CString	m_f_plan_time;
	CString	m_length;
	CString	m_motoman;
	CString	m_speed;
	CString	m_train_id;
	CString	m_weight;
	CString	m_e_plan_time;
	CString	m_e_station;
	CString	m_f_station;
	CString	m_train_num;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPageTrainAttributeOther)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPageTrainAttributeOther)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeAttributeTrainCC();
	afx_msg void OnSelchangeAttributeTrainCX();
	afx_msg void OnSelchangeAttributeTrainJJ();
	afx_msg void OnSelchangeAttributeTrainZY();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnEnChangeEdit1FStation();
	afx_msg void OnEnChangeEdit1EStation();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PAGETRAINATTRIBUTEOTHER_H__BC3A6724_97E5_4F9C_A899_2379C91C0116__INCLUDED_)
