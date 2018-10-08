#if !defined(AFX_CHANGETRAINID_H__ECACB123_B486_11D1_9D2F_0000C05319CE__INCLUDED_LZ)
#define AFX_CHANGETRAINID_H__ECACB123_B486_11D1_9D2F_0000C05319CE__INCLUDED_LZ

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// ChangeTrainID.h : header file
//

#include "tgmsg.h"
#include "SoftKeyBoardDlg.h"
#include "afxwin.h"
#include "tg_structdef.h"
#include "tgdocument.h"
/////////////////////////////////////////////////////////////////////////////
// InsertTrainLanzhou.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CInsertTrainLanzhou dialog

class CInsertTrainLanzhou : public CDialog
{
// Construction
public:
	void fillDataStruct(int total, int pos);
		
	int m_nStartMode,m_nEndMode;
	BOOL m_bAddGraphic;
    bool m_bChanged;
	bool m_bAddShift;
	bool m_bSecret;
	int  m_nRunType;
	BOOL m_bAutoSelectShiftAddTrain;
 	int GetInsertTrainMethod(int &nRouteNo,int &nMode);
	int m_nInsertMethod;
	char * GetTrainId(char *pbInputBuffer);
	CInsertTrainLanzhou(CTgDocument* pDoc, int con_id,LPCTSTR text = NULL,CWnd* pParent = NULL);   // standard constructor

    int m_nRouteNo,m_nDirect;
	int m_nCurSel;
	CTgDocument* m_pDoc;
// Dialog Data
	//{{AFX_DATA(CInsertTrainLanzhou)
	enum { IDD = IDD_DLG_INSERT_TRAIN_LANZHOU };
	CComboBox	m_strRoute;
	CString	m_strInsertTrainId;
	CString	m_strArrivalTrainId;
	CString m_strEngineId;
	CListCtrl m_engine_list;
	CComboBox m_combox;
	CComboBox m_comboxStartStation;
	CComboBox m_comboxEndStation;
	CComboBox m_comboxQianYin;
	CComboBox m_strAbsRunType;
	//}}AFX_DATA
	int m_nSectionID;
 	int m_nCurrFirstRouteNo;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInsertTrainLanzhou)
	public:
 	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
    bool rec_flag;
	// Generated message map functions
	//{{AFX_MSG(CInsertTrainLanzhou)
 	afx_msg void OnInsertGraphic();
	afx_msg void OnInsertList();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnGraphic();
	afx_msg void OnSelchangeRoute();
	afx_msg void OnShift();
	afx_msg void OnDestroy();
	afx_msg void OnMoving( UINT nSide, LPRECT lpRect );
	afx_msg void OnWindowPosChanged( WINDOWPOS* lpwndpos );
	afx_msg void OnEditchangeTrainID();
	afx_msg void OnEnChangeEdit1();
	afx_msg void OnNMClickList2(NMHDR *pNMHDR, LRESULT *pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	int m_nSelList;
	int passway_count;
	TG_PassWayInfo info[200];//径路数据
	int Flag_moredata;       
	void LoadPasswayInfo();

	CSoftKeyBoardDlg *m_pKbDlg;
	BYTE m_nKBStyle;//表示键盘显示在输入dlg的右面还是下面

public:
	CStringArray m_aryEngineId;
	CDWordArray  m_aryStatus;
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedButton2();
};
//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHANGETRAINID_H__ECACB123_B486_11D1_9D2F_0000C05319CE__INCLUDED_LZ)
