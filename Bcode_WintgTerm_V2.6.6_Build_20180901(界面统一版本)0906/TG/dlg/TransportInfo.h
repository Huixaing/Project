#if !defined(AFX_TRANSPORTINFO_H__20722814_2E47_4240_B335_90B796932D61__INCLUDED_)
#define AFX_TRANSPORTINFO_H__20722814_2E47_4240_B335_90B796932D61__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TransportInfo.h : header file
//
#include "resource.h"
#include "tgdoc.h"

/////////////////////////////////////////////////////////////////////////////
// CTransportInfo dialog

class CTransportInfo : public CDialog
{
// Construction
public:
	CTransportInfo(CWnd* pParent);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CTransportInfo)
	enum { IDD = IDD_DLG_TRANSPORT_INFO };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTransportInfo)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
   
	// Generated message map functions
	//{{AFX_MSG(CTransportInfo)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	CString	m_strTicket;
	
	bool    m_bEdit;
	CString	m_strStation;
	CString	m_strAbs;
	CString m_strOpmsStationText;
	int     m_opmsSetStationText;

	ULONG m_train_index;
	int   m_rec_index;
	CTGDoc* m_pDoc;
	
	
protected:
	void SetCenterPosition();
	void SendTrainUpdate();

	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedOk();
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TRANSPORTINFO_H__20722814_2E47_4240_B335_90B796932D61__INCLUDED_)
