#if !defined(AFX_ADDNEWTRAINDLG_H__8FCC690E_527C_4A61_8E55_9037B06FD419__INCLUDED_)
#define AFX_ADDNEWTRAINDLG_H__8FCC690E_527C_4A61_8E55_9037B06FD419__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AddNewTrainDlg.h : header file
//
#include "resource.h"
#include "tgmsg.h"
#include "tg_basicpara.h"
/////////////////////////////////////////////////////////////////////////////
// CAddNewTrainDlg dialog
class CTgDocument;

class CAddNewTrainDlg : public CDialog
{
// Construction
public:
	CAddNewTrainDlg(CTgDocument *pDoc,int schd_type,CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAddNewTrainDlg)
	enum { IDD = IDD_DLG_ADD_TRAINS };
	CComboBox	m_ctrlRoute;
	CComboBox	m_ctrlStation;
	CComboBox	m_ctrlRunType;
	CListBox	m_ctrlList;
	CString	m_strTrainId;
	CString	m_strReceiveTime;
	//}}AFX_DATA
    struct _REPORT_TRAIN_{
	   ushort nStation;
       ushort nRoute;
       char strTrainId[10];
	   long usReceiveDate;
	   long usReceiveTime;
	   byte btTrainType;
       byte nFlag;
	   BOOL giveout;
	   BOOL fetch;
	   BYTE nRunType;
	}m_nReportTrainInfo[100];
    int m_nTrainCount;
    int m_nStationCount;
	int m_nCurrStation;
	int m_nRunType;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAddNewTrainDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

public: 
	int passway_count;
private:
	int Flag_moredata;
    TG_PassWayInfo     Info[128];         //¾¶Â·Êý¾Ý
	int Schd_type;
	void LoadPasswayInfo();
	
// Implementation
protected:
    CTgDocument *m_pDoc;

	// Generated message map functions
	//{{AFX_MSG(CAddNewTrainDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonAdd();
	afx_msg void OnButtonDel();
	afx_msg void OnSelchangeComboRoute();
	afx_msg void OnKillfocusComboRoute();
	afx_msg void OnSelchangeComboStation();
	afx_msg void OnSelchangeRunType();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ADDNEWTRAINDLG_H__8FCC690E_527C_4A61_8E55_9037B06FD419__INCLUDED_)
