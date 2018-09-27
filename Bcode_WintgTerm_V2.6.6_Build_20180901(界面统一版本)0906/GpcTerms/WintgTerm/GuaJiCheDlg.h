#if !defined(AFX_GUAJICHEDLG_H__62E42A0C_8ACB_4A63_A87A_2C09AECEF5EE__INCLUDED_)
#define AFX_GUAJICHEDLG_H__62E42A0C_8ACB_4A63_A87A_2C09AECEF5EE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GuaJiCheDlg.h : header file
//
//#define MAX_ENGINEER_NUM 8
#define REAL_MAX_ENGIN_NUM  6
#include "resource.h"
#include "wintgdoc.h"

typedef struct _tagguajiche
{
	BYTE     engine_count;
	char     engine_id[MAX_ENGINEER_NUM][20];
    BYTE     engine_type[MAX_ENGINEER_NUM];
    USHORT   attach_station[MAX_ENGINEER_NUM];  //是rec的下标
    USHORT   unattach_station[MAX_ENGINEER_NUM]; //是rec的下标
}GUAJICHE_INFO;

/////////////////////////////////////////////////////////////////////////////
// CGuaJiCheDlg dialog

class CGuaJiCheDlg : public CDialog
{
// Construction
public:
	CGuaJiCheDlg(CWnd* pParent = NULL);   // standard constructor
public:
	void fillLeftContent(int curSel);
	BOOL initialButtonStatus();
	BOOL initLeftColumn();
	CGuaJiCheDlg(CSingleTrain *pTrain,int rec_index,CWnd *pParent);
	GUAJICHE_INFO m_GuaJiChe;
	BOOL InitialListCtrl();

// Dialog Data
	//{{AFX_DATA(CGuaJiCheDlg)
	enum { IDD = IDD_DLG_GUAJICHE };
	CListCtrl	m_List;
	CString	m_jiCheNo;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGuaJiCheDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CGuaJiCheDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeComboShuai();
	afx_msg void OnSelchangeTrainList();
	afx_msg void OnSave();
	afx_msg void OnDelete();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	int m_curRecIndex;
	CSingleTrain * m_pTrain;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GUAJICHEDLG_H__62E42A0C_8ACB_4A63_A87A_2C09AECEF5EE__INCLUDED_)
