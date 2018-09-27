#pragma once

#include ".\czhgridview.h"

#include "tgdoc.h"
#include "tgform.h"
// CJiaoLuEditDlg 对话框
#define MSG_UPDATE_RELATION 10012
class CJiaoLuEditDlg : public CDialog
{
	DECLARE_DYNAMIC(CJiaoLuEditDlg)

public:
	CJiaoLuEditDlg(CString title, TRAIN_INDEX train_index, CTGDoc *pDoc, CWnd* pParent);   // 标准构造函数
	virtual ~CJiaoLuEditDlg();

// 对话框数据
	enum { IDD = IDD_DLG_JIAOLU_EIDT };

	int   OnCreate(LPCREATESTRUCT lpCreat);
// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CJiaoLuEditDlg)
	virtual BOOL OnInitDialog();
	CString m_czh;
	CString m_trainid;
	//}}AFX_MSG

protected:
	CCZHGridView m_grid;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual void PostNcDestroy();
	void OnSize(UINT nType,int cx,int cy);
	afx_msg LRESULT OnUpdateTrainRelation(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()

	CString GetSelectJiaolu();
	void ShowCZHTrains();

	void UpdateTrainRelationGroup();

	afx_msg LRESULT OnSelectTrain(WPARAM wParam,LPARAM lParam);
	void SetDeleteTrainInfo(CString trainid, CString trainindexstr);
private:
	CString m_title;
	CTGDoc* m_pDoc;
	TRAIN_INDEX sel_train_index;
	CComboBox m_group;
	std::vector<TrainGroupInfo> mListRL;
	
public:
	afx_msg void OnShowSelTrainRelation();
	afx_msg void OnClose();
	
	afx_msg void OnBnAddTrainRelation();
	afx_msg void OnBnDelTrainRelation();
	afx_msg void OnBnClickedRefresh();
};
