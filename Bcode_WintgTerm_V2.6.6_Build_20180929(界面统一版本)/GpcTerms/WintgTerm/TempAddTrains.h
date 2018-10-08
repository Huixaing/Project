#if !defined(AFX_TEMPADDTRAINS_H__DAB04248_7ACB_48CD_8C9F_3649879C832F__INCLUDED_)
#define AFX_TEMPADDTRAINS_H__DAB04248_7ACB_48CD_8C9F_3649879C832F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TempAddTrains.h : header file
//
//#include "wintgdoc.h"
#include "afxdtctl.h"
#include "resource.h"
#include "tgdocument.h"

#define  ADD_TMP_TRAIN_MSG   WM_USER+1000

class CTgEditView;

struct TmpTrainInfo
{
	int nSelPasswayId;//径路
	int nBeginStaIndex;//开始车站
	int nEndStaIndex;//结束车站
	long beginTime;//开始时间
	BYTE nRunType; //列车运行时分类型
	char trainid[10];//车次号
	BYTE nEngineCount;
	char engineId[5][9];//机车号
	DWORD status[5];
	BYTE start_mode;
	BYTE end_mode;
	bool bSecret;
	TmpTrainInfo()
	{
		memset(this, 0, sizeof(TmpTrainInfo));
		bSecret=false;
		nRunType=255;
	}
};
/////////////////////////////////////////////////////////////////////////////
// CTempAddTrains dialog
class CTempAddTrains : public CDialog
{
// Construction
public:
	CTempAddTrains(CTgDocument *pDoc,int conid, CTgEditView* pParent);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CTempAddTrains)
	enum { IDD = IDD_TEMP_ADD_TRAINS };
	CString m_strTrainId;
	CString m_strEngineId;
	CListCtrl m_engine_list;
	CComboBox m_comboxEngineType;
	CComboBox m_comboxStartStation;
	CComboBox m_comboxEndStation;
	CComboBox m_comboxQianYin;
	CComboBox m_strAbsRunType;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTempAddTrains)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CTempAddTrains)
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnChangePlanTrainid();
	afx_msg void OnSelchangeComboRoute();
	afx_msg void OnNMClickList2(NMHDR *pNMHDR, LRESULT *pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void LoadPasswayInfo();
    void SetCheck();
	void InitEngineInfo();
private:
	int m_nSelList;
	CTgDocument* m_pDoc;
	int m_start_mode;
	int m_end_mode;
	int m_nSectionID;
	int passway_count; 
	TG_PassWayInfo passway[200];//径路数据
	CTgEditView* m_pEditView;

public:
	afx_msg void OnBnClickedButtonAdd();
	afx_msg void OnBnClickedRadioAccept();
	afx_msg void OnBnClickedRadioStart();
	afx_msg void OnBnClickedRadioGiveout();
	afx_msg void OnBnClickedRadioTerminal();
	afx_msg void OnBnClickedButtonClear();
protected:
	virtual void PostNcDestroy();
public:
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedButton2();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TEMPADDTRAINS_H__DAB04248_7ACB_48CD_8C9F_3649879C832F__INCLUDED_)
