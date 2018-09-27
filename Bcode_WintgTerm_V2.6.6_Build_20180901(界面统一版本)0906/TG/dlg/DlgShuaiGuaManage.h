#pragma once
#include "afxcmn.h"
#include "afxwin.h"

#include "tgdoc.h"
// CDlgShuaiGuaManage 对话框

class CDlgShuaiGuaManage : public CDialog
{
	DECLARE_DYNAMIC(CDlgShuaiGuaManage)

public:
	CDlgShuaiGuaManage(CTGDoc* pdoc, CTG_TRAIN* train, CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgShuaiGuaManage();

// 对话框数据
	enum { IDD = IDD_DLG_SHUAI_MANAGER };

protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	CListCtrl m_train_record_list;
	CComboBox m_combox;

	DECLARE_MESSAGE_MAP()

public:
    CTG_TRAIN mTrain;
	CTGDoc* m_pDoc;
	
	CString itemno,strStaText,sta_name,sg_type,sg_info,sg_str,dlg_title;

	int m_nAttachHeavyCnt;
	int m_nAttachEmptyCnt;

	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedOk();
	afx_msg void OnNMClickList2(NMHDR *pNMHDR, LRESULT *pResult);
	
	void Statistic();
	afx_msg void OnCbnSelchangeCombo1();
	
};
