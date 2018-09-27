#pragma once

#include "tgdoc.h"
// CDlgCheZuHao 对话框

class CDlgCheZuHao : public CDialog
{
	DECLARE_DYNAMIC(CDlgCheZuHao)

public:
	CDlgCheZuHao(CTG_TRAIN* train, CTGDoc *pDoc, CWnd* pParent);   // 标准构造函数
	virtual ~CDlgCheZuHao();

// 对话框数据
	enum { IDD = IDD_DLG_CZH };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	// Implementation
protected:
   
	// Generated message map functions
	//{{AFX_MSG(CTransportInfo)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	CString	m_strCheZuHao;
	CString m_origStStation;
	CString m_origEdStation;
		
	CTGDoc* m_pDoc;
	CTG_TRAIN m_train;
public:
	afx_msg void OnBnClickedOk();
};
