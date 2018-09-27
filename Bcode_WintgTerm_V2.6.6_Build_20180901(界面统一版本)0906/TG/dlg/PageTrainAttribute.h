#if !defined(AFX_PAGETRAINATTRIBUTE_H__C68C7AD9_A0B7_4845_83F2_3CE40831E36F__INCLUDED_)
#define AFX_PAGETRAINATTRIBUTE_H__C68C7AD9_A0B7_4845_83F2_3CE40831E36F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PageTrainAttribute.h : header file
//
#include "resource.h"
#include "tgdoc.h"

/////////////////////////////////////////////////////////////////////////////
// CPageTrainAttribute dialog


class CPageTrainAttribute : public CPropertyPage
{
	DECLARE_DYNCREATE(CPageTrainAttribute)
public:
// Construction
	CPageTrainAttribute();
 	CPageTrainAttribute(CTG_TRAIN* train, CTGDoc *pDoc);
	~CPageTrainAttribute();
public:  	
// Dialog Data
	//{{AFX_DATA(CPageTrainAttribute)
	enum { IDD = IDD_PGE_TRAIN_ATT1 };
	CComboBox	m_ctlTrainAbs;
	CComboBox	m_ctrTrainType;
	CComboBox	m_ctrAbsTimeType;
 	CString	m_nTrainId;
	BOOL	m_check_join_adjust;
    BOOL    m_check_lock_arrrival;
	BOOL    m_check_lock_depart;
	//}}AFX_DATA

	// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPageTrainAttribute)
	protected:
 	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPageTrainAttribute)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeAttributeTraintype();
	afx_msg void OnSelTrainRunType();
	afx_msg void OnSelTrainRuntimeType();
	afx_msg void OnCheck1();
	afx_msg void OnCheck2();
 	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	int m_nTrainType,m_nTrainAbs;
	BYTE m_nAbsTimeType;
private:
	void InitIdentify();

	CTGDoc *m_pDoc;
    CTG_TRAIN *m_pTrain;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PAGETRAINATTRIBUTE_H__C68C7AD9_A0B7_4845_83F2_3CE40831E36F__INCLUDED_)
