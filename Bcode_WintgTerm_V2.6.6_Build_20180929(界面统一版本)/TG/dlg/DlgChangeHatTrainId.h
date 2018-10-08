#if !defined(AFX_DLGCHANGEHATTRAINID_H__945248E0_0E37_4EAB_B6D2_F47E8976B885__INCLUDED_)
#define AFX_DLGCHANGEHATTRAINID_H__945248E0_0E37_4EAB_B6D2_F47E8976B885__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgChangeHatTrainId.h : header file
//
#include "resource.h"
/////////////////////////////////////////////////////////////////////////////
// CDlgChangeHatTrainId dialog

class CDlgChangeHatTrainId : public CDialog
{
// Construction
public:
	
	CDlgChangeHatTrainId(CString trainid,CString hatid,CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgChangeHatTrainId)
	enum { IDD = IDD_DLG_CHANGE_HATTID };
	CString	m_strTrainid;
	CString	m_strHatid;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgChangeHatTrainId)
	public:
	virtual BOOL PreTranslateMessage(MSG* lpMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgChangeHatTrainId)
	afx_msg void OnChangeAck();
	afx_msg void OnChangeCancel();
	afx_msg void OnChangeCancelACK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	ULONG m_train_index;
	CTGDoc* m_pDoc;
private:
	void SendTrainUpdate();
	void SetCenterPosition();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGCHANGEHATTRAINID_H__945248E0_0E37_4EAB_B6D2_F47E8976B885__INCLUDED_)
