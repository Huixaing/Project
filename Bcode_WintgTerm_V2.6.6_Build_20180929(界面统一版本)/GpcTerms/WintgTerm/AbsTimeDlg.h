#if !defined(AFX_CABSTIMEDLG_H__92CA5997_9434_11D1_88D5_444553540000__INCLUDED_)
#define AFX_CABSTIMEDLG_H__92CA5997_9434_11D1_88D5_444553540000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// AbsTimeDlg.h : header file
//
#include "resource.h"
#include "tgmsg.h"
//#include "macroswitch.h"
#include "GridCtrl_src/GridCtrl.h"


/////////////////////////////////////////////////////////////////////////////
// CAbsTimeDlg dialog
//#define MAX_NUM 20 

struct ABS_TEXT_MAP
{
	CWordArray   nAryTypeVal;
	CStringArray szAryText;
};
class CAbsTimeDlg : public CDialog
{
// Construction
public:
	void ShowRecord();
	int m_nSectionID;
    int m_nCurStation;
	int nIndex;
    int m_nCurrentStationList[256];
	int m_nStationCount;

	ushort ConvertStrToInt(CString str );
	CAbsTimeDlg(int con_id,CWnd* pParent = NULL);   // standard constructor//modi by cf after 9-9
	~CAbsTimeDlg();
    CGridCtrl *m_pGridCtrl;
// Dialog Data
	//{{AFX_DATA(CAbsTimeDlg)
	enum { IDD = IDD_ABS_TIME };
	CComboBox	m_cmbStation;
	CListCtrl   m_list;
	//}}AFX_DATA

public:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAbsTimeDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAbsTimeDlg)
	virtual BOOL OnInitDialog();
 	virtual void OnOK();
	afx_msg void OnSelchangeCmbStation();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	
private:
	CString GetTrainTypeStr(int nType);

public:
	afx_msg void OnBnClickedRestoreOrigdata();
	afx_msg void OnModifyAbsTime(NMHDR *pNMHDR, LRESULT *pResult);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CABSTIMEDLG_H_H__92CA5997_9434_11D1_88D5_444553540000__INCLUDED_)
