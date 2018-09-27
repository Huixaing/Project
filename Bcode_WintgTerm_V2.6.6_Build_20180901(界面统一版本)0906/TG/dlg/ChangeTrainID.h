#if !defined(AFX_CHANGETRAINID_H__ECACB123_B486_11D1_9D2F_0000C05319CE__INCLUDED_)
#define AFX_CHANGETRAINID_H__ECACB123_B486_11D1_9D2F_0000C05319CE__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// ChangeTrainID.h : header file
//
#include "resource.h"
#include "tgdoc.h"

// CChangeTrainID dialog

class CChangeTrainID : public CDialog
{
// Construction
public:
	CChangeTrainID(CString strOldTrainId,CString title, CWnd* pParent);   // standard constructor
// Dialog Data
	//{{AFX_DATA(CChangeTrainID)
	enum { IDD = IDD_DLG_CHANGE_TID };
	CString	m_strNewTrainId;
	CString	m_oldTrainId;
	//}}AFX_DATA
	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChangeTrainID)
	public:
	virtual BOOL PreTranslateMessage(MSG* lpMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CChangeTrainID)
	afx_msg void OnChangeAck();
	afx_msg void OnChangeCancel();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	CTGDoc *m_pDoc;
	ULONG m_train_index;
	int   m_nSelRecIdx;
	int   m_nSelRectIdx;
    BYTE   m_nFlag;
private:
	void SendTrainUpdate();
	void SetCenterPosition();
	CString m_title;
protected:
	virtual void PostNcDestroy();
};
//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHANGETRAINID_H__ECACB123_B486_11D1_9D2F_0000C05319CE__INCLUDED_)
