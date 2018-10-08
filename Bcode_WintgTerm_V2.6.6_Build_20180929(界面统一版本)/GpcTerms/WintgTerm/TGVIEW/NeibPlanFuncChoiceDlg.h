#pragma once

#include "resource.h"
// CNeibPlanFuncChoiceDlg 对话框

class CNeibPlanFuncChoiceDlg : public CDialog
{
	DECLARE_DYNAMIC(CNeibPlanFuncChoiceDlg)

public:
	CNeibPlanFuncChoiceDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CNeibPlanFuncChoiceDlg();

// 对话框数据
	enum { IDD = IDD_DLG_NEIG_PLAN_CHOICE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL PreTranslateMessage(MSG* pMSG);
	DECLARE_MESSAGE_MAP()
public:
	int nRet;
	afx_msg void OnClose();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
};
