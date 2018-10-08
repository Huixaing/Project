#pragma once

#include ".\czhgridview.h"

#include "tgdoc.h"
#include "tgform.h"
// CZHMultiEditDlg 对话框
#define MSG_UPDATE_CZH 10012
class CZHMultiEditDlg : public CDialog
{
	DECLARE_DYNAMIC(CZHMultiEditDlg)

public:
	CZHMultiEditDlg(CString title, CString strInitCzh, CTGDoc *pDoc, CWnd* pParent, BYTE flag);   // 标准构造函数
	virtual ~CZHMultiEditDlg();

// 对话框数据
	enum { IDD = IDD_DLG_CZH_MULTI_EDIT };

	int   OnCreate(LPCREATESTRUCT lpCreat);
// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CZHMultiEditDlg)
	virtual BOOL OnInitDialog();
	CString m_czh;
	CString m_trainid;
	//}}AFX_MSG

protected:
	CCZHGridView m_grid;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual void PostNcDestroy();
	void OnSize(UINT nType,int cx,int cy);
	afx_msg LRESULT OnUpdateSelCZHTrain(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()

	CString GetEditCZH();
	CString GetSelectCZH();
	void ShowCZHTrains(TRAIN_CHEZUHAO* pTrainsCZH);

	void UpdateSelectComboBox(CString czh);

	afx_msg LRESULT OnSelectTrain(WPARAM wParam,LPARAM lParam);
	void SetDeleteTrainInfo(CString trainid, CString trainindexstr);
private:
	CTGDoc* m_pDoc;
	CString m_strInitCzh;
	CString m_title;
	CComboBox m_group;
	BYTE m_nFlag;
   
public:
	afx_msg void OnSelCZHChange();
	afx_msg void OnClose();
	afx_msg void OnBnAddCZH();
	afx_msg void OnBnModifyCZH();
	afx_msg void OnBnDeleteCZH();
	afx_msg void OnBnAddTrain();
	afx_msg void OnBnDeleteTrain();
};
