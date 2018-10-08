#if !defined(AFX_HistorySchdDlg_H__47202805_4C66_4F3E_B3FF_AE3F4ACEFCC8__INCLUDED_)
#define AFX_HistorySchdDlg_H__47202805_4C66_4F3E_B3FF_AE3F4ACEFCC8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LoginDlg.h : header file
//
#include "resource.h"
 
//#include "tgmsg.h"
/////////////////////////////////////////////////////////////////////////////
// CHistorySchdDlg dialog

class CHistorySchdDlg : public CDialog
{
// Construction
public:
	unsigned long id;
	void ConvertShiftidToString(unsigned long id);
	
	unsigned long nGetNextDay();
	unsigned long nGetToday();
	CHistorySchdDlg(CWnd* pParent = NULL);   // standard constructor
	unsigned long  g_banbie;      //save current banbie ,e.g, 2002060701 
    CTime t;     
    int nFlag;
	BOOL	m_bUnChange;
// Dialog Data
	//{{AFX_DATA(CHistorySchdDlg)
	enum { IDD = IDD_DLG_HISTORY };
	CEdit	m_BanbieCtrl;
	CEdit	m_DateCtrl;
 	CString	m_date;
	CString	m_banbie;
	BOOL	m_inputFlag;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHistorySchdDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	int GetConsoleid();
 	CString GetNextDay();
	COLORREF bk_color;
	CString password;
	CBrush m_Brush;

	// Generated message map functions
	//{{AFX_MSG(CHistorySchdDlg)
	virtual void OnOK();
	afx_msg LRESULT OnLoginMsg(WPARAM wParam, LPARAM lParam);
	virtual BOOL OnInitDialog();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnPrevious();
	afx_msg void OnNext();
	afx_msg void OnCheckInputdir();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	BOOL SelectEnable;
	int  m_nPreSchdReqNum; 

public:
	int  m_nPreSchdMaxNum; 
	bool m_bCtrlBanci; // ¿ØÖÆ°à´ÎµÇÂ¼
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HistorySchdDlg_H__47202805_4C66_4F3E_B3FF_AE3F4ACEFCC8__INCLUDED_)
