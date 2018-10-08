
#pragma once

#include "resource.h"
#include "afxwin.h"
#include "tgdoc.h"
/////////////////////////////////////////////////////////////////////////////
// CTrainPartRunDlg dialog

class CTrainPartRunDlg : public CDialog
{
// Construction
public:
	CTrainPartRunDlg(CTGDoc *pDoc, CWnd* pParent, int station, int station_next);   // standard constructor

// Dialog Data
	enum { IDD = IDD_DIG_PART_RUN_TRAIN };
	CString	m_sStaionid;
	CComboBox m_next_comboBox;
	CComboBox m_cur_comboBox;
	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTrainPartRunDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual BOOL OnInitDialog();
	afx_msg void OnEnChangeEdit1();
	virtual void OnOK();
	afx_msg void OnSelWholeOrPartTrain();
	afx_msg void OnSelHelpStartStation();
	afx_msg void OnSelHelpDirectStation();
	DECLARE_MESSAGE_MAP()

private:
	int m_station;
	int m_station_next;
	CTGDoc *m_pDoc;
public:
	CString m_sRetTrainId;
	int m_start_station;
	int m_end_station;
};


