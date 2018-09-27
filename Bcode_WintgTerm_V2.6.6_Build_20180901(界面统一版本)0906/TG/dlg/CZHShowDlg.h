#pragma once

#include ".\czhgridview.h"

#include "tgdoc.h"
#include "tgform.h"
// CCZHShowDlg 对话框

class CCZHShowDlg : public CDialog
{
	DECLARE_DYNAMIC(CCZHShowDlg)

public:
	CCZHShowDlg(CString title, CTG_TRAIN* train, CTGDoc *pDoc, CWnd* pParent);   // 标准构造函数
	virtual ~CCZHShowDlg();

// 对话框数据
	enum { IDD = IDD_DLG_CZH_SHOW };

	int   OnCreate(LPCREATESTRUCT lpCreat);
// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CCZHShowDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG

protected:
	CCZHGridView m_grid;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual void PostNcDestroy();
	void UpdateSelectComboBox(CString czh);
	void OnSize(UINT nType,int cx,int cy);
	afx_msg LRESULT OnUpdateSelCZHTrain(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()

	void ShowCZHTrains(TRAIN_CHEZUHAO* pTrainsCZH);
private:
	CComboBox m_group;
	CTGDoc* m_pDoc;
	CTG_TRAIN* m_train;
	CString m_title;
public:
	afx_msg void OnSelCZHChange();
	afx_msg void OnClose();
};
