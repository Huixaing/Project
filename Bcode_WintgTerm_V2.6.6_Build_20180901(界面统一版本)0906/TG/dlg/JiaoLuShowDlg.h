#pragma once

#include ".\czhgridview.h"

#include "tgdoc.h"
#include "tgform.h"
// CCZHShowDlg 对话框

class CJiaoLuShowDlg : public CDialog
{
	DECLARE_DYNAMIC(CJiaoLuShowDlg)

public:
	CJiaoLuShowDlg(CString title, TRAIN_INDEX train_index, CTGDoc *pDoc, CWnd* pParent);   // 标准构造函数
	virtual ~CJiaoLuShowDlg();

// 对话框数据
	enum { IDD = IDD_DLG_JIAOLU_VIEW };

	int   OnCreate(LPCREATESTRUCT lpCreat);
// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CJiaoLuShowDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG

protected:
	CCZHGridView m_grid;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual void PostNcDestroy();

	void OnSize(UINT nType,int cx,int cy);
	afx_msg LRESULT OnUpdateTrainRelation(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()

	void ShowCZHTrains();
private:
	CString m_title;
	CTGDoc* m_pDoc;
	CTG_TRAIN* m_train;
	CComboBox m_group;
	TRAIN_INDEX sel_train_index;
	std::vector<TrainGroupInfo> mListRL;
public:
	afx_msg void OnShowSelTrainRelation();
	afx_msg void OnClose();
	afx_msg void OnBnClickedRefresh();
};
