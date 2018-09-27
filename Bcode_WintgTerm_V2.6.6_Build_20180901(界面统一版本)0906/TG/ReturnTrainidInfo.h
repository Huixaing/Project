#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// CReturnTrainidInfo 对话框

class CReturnTrainidInfo : public CDialog
{
	DECLARE_DYNAMIC(CReturnTrainidInfo)

public:
	CReturnTrainidInfo(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CReturnTrainidInfo();

// 对话框数据
	enum { IDD = IDD_DLG_GEN_RETURNID };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual void PostNcDestroy();
	afx_msg void OnClose();
	//{{AFX_MSG(CReturnTrainidInfo)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_list;
	CProgressCtrl m_pross;
	int m_total;
public:
	void SetTotalProcessNum(int all);
	void StepAt(int step);
	void AddGenMessage(CString text, CString czh);
	CEdit m_detailText;
	afx_msg void OnNMClickList4(NMHDR *pNMHDR, LRESULT *pResult);
};
