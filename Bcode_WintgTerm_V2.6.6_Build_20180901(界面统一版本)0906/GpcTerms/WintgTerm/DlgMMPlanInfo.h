#pragma once

#include "utility/xmlview.h"
// CDlgMMPlanInfo 对话框
class  CTgDocument; 
class CDlgMMPlanInfo : public CDialog
{
	DECLARE_DYNAMIC(CDlgMMPlanInfo)

public:
	CDlgMMPlanInfo(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgMMPlanInfo();

// 对话框数据
	enum { IDD = IDD_DLG_MPLAN_INFO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
	CTabCtrl m_tab;

	BOOL OnInitDialog(); 
	void OnSelchangeTabctrl(NMHDR* pNMHDR, LRESULT* pResult);
	int OnCreate(LPCREATESTRUCT lpCreateStruct);
	void OnClose();

public:

	CTgDocument *m_pDoc;
	CString  m_planid;
	CString  m_info;
	CString  m_jieshi;
	CString  m_trninfo;

	CXMLDataGrid m_info_grid;
	CXMLDataGrid m_trn_grid;
	CEdit        m_jieshi_edit;

	afx_msg void OnTcnSelchangeMplanTab(NMHDR *pNMHDR, LRESULT *pResult);
	void  UpdateShow();
	void  SetDocument(CTgDocument *pdoc) {m_pDoc=pdoc;}
	afx_msg void OnBnClickedBtnRefresh();
	BOOL  m_bCanClose;
	void  OnBnClickedCancel();
};
