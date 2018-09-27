#if !defined(AFX_ABSTIME_H__F78DB493_490D_11D5_9C64_0050BA679B44__INCLUDED_)
#define AFX_ABSTIME_H__F78DB493_490D_11D5_9C64_0050BA679B44__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AbsTime.h : header file
//
#include "resource.h"
#include "wintgdoc.h"
//#include "HeaderList.h"

#include "GridListCtrl.h"
#include "InPlaceEdit.h"
  
/////////////////////////////////////////////////////////////////////////////
// CAbsTimeDlg dialog

class CAbsTimeDlg : public CDialog
{
// Construction
public:
	void SetListText();
	int m_nCurrentSel;
	CString GetTrainTypeStr(int nType);
//	CAbsTimeDlg(CWintgDoc *pDoc,CWnd* pParent = NULL);   // standard constructor
  	CAbsTimeDlg(CWnd* pParent = NULL);   // standard constructor
 //  CWintgDoc * m_pDoc;
	~CAbsTimeDlg();
  
//	CInPlaceEdit *m_pListEdit;

	typedef struct _uint{
		ushort station_no1;
		ushort station_no2;
        byte exit;
		byte entry;
		byte direction;	
		byte train_type;
		ushort stop_time;
		ushort start_time;
		ushort run_time;
 
	}UNIT;
	UNIT  unit[150];
    int numUnit;
	// Dialog Data
	//{{AFX_DATA(CAbsTimeDlg)
	enum { IDD = IDD_ABS_TIME };
	CString	m_edit1;
	//}}AFX_DATA
//	CInPlaceEdit *m_pListEdit;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAbsTimeDlg)
	public:
//	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAbsTimeDlg)
	afx_msg void OnAbsTimeMsg(WPARAM wParam, LPARAM lParam);
 	virtual BOOL OnInitDialog();
	afx_msg void OnClickList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkList(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ABSTIME_H__F78DB493_490D_11D5_9C64_0050BA679B44__INCLUDED_)
