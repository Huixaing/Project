#if !defined(AFX_TRAINENGINEINFODLG_H__E55DBA43_5427_4B2F_A4BC_1B53DB0BD972__INCLUDED_)
#define AFX_TRAINENGINEINFODLG_H__E55DBA43_5427_4B2F_A4BC_1B53DB0BD972__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TrainEngineInfoDlg.h : header file
//
#include <vector>
#include "resource.h"
#include "tgmsg.h"
const int ITEM_NUM = 8;
/////////////////////////////////////////////////////////////////////////////
// CTrainEngineInfoDlg dialog
class CTrainEngineInfoDlg : public CDialog
{
// Construction
public:
	CTrainEngineInfoDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CTrainEngineInfoDlg)
	enum { IDD = IDD_DIALOG_ENGINE };
	CListCtrl	m_engine_list;
	CListBox    m_list_box;
	//}}AFX_DATA
		
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTrainEngineInfoDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CTrainEngineInfoDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	bool RequestEngineInfo();
	void ShowEngineCrewInfo(int i);
protected:
	virtual void PostNcDestroy();
	afx_msg void OnClose();

public:
	struct QueryParam
	{
		ULONG train_index;
		int   section;
		long  entity;
		long  shift_start_time;
	}mQueryParam;

private:
	struct EngineCrewInfo
	{
		CString jclx;
		CString info[ITEM_NUM];
	};

	int m_nEngineCrewInfoCount;
	EngineCrewInfo m_aEngineCrewInfo[32];
public:
	afx_msg void OnLbnSelchangeListEngineCrew();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TRAINENGINEINFODLG_H__E55DBA43_5427_4B2F_A4BC_1B53DB0BD972__INCLUDED_)
