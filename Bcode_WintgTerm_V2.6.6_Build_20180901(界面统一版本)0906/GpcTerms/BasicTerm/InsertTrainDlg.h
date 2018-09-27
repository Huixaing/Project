#if !defined(AFX_CHANGETRAINID_H__ECACB123_B486_11D1_9D2F_0000C05319CE__INCLUDED_)
#define AFX_CHANGETRAINID_H__ECACB123_B486_11D1_9D2F_0000C05319CE__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// ChangeTrainID.h : header file
//

#include "tgmsg.h"
#include "SoftKeyBoardDlg.h"
#include "afxwin.h"
#include "tg_structdef.h"
/////////////////////////////////////////////////////////////////////////////
// InsertTrain.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CInsertTrain dialog

class CInsertTrain : public CDialog
{
// Construction
public:
	void fillDataStruct(int total, int pos);//add by cf
		
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
	CInsertTrain(int con_id,LPCTSTR text = NULL,CWnd* pParent = NULL);   // standard constructor

    int m_nRouteNo,m_nDirect;
	int m_nCurSel;
// Dialog Data
	//{{AFX_DATA(CInsertTrain)
	enum { IDD = IDD_DLG_INSERT_TRAIN };
	CComboBox	m_strRoute;
	CComboBox	m_strAbsRunType;
	CString	m_strInsertTrainId;
	CString	m_strArrivalTrainId;
	
	//}}AFX_DATA
	int m_nSectionID;
 	int m_nCurrFirstRouteNo;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInsertTrain)
	public:
 	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
    bool rec_flag;
	// Generated message map functions
	//{{AFX_MSG(CInsertTrain)
 	afx_msg void OnInsertGraphic();
	afx_msg void OnInsertList();
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonOk();
	afx_msg void OnButtonCancel();
	afx_msg void OnOk();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnGraphic();
	afx_msg void OnEditchangeRoute();
	afx_msg void OnSelchangeRoute();
	afx_msg void OnShift();
	afx_msg void OnDestroy();
	afx_msg void OnMoving( UINT nSide, LPRECT lpRect );
	afx_msg void OnWindowPosChanged( WINDOWPOS* lpwndpos );
	afx_msg void OnEditchangeTrainID();
	afx_msg void OnEnChangeEdit1();

	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	int passway_count;
	TG_PassWayInfo info[200];//径路数据
	int Flag_moredata;       
	void LoadPasswayInfo();

	CSoftKeyBoardDlg *m_pKbDlg;
	BYTE m_nKBStyle;//表示键盘显示在输入dlg的右面还是下面

};
//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHANGETRAINID_H__ECACB123_B486_11D1_9D2F_0000C05319CE__INCLUDED_)
