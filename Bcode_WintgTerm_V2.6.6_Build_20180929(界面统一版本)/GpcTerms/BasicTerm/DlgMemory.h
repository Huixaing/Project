#pragma once
#include "utility/xmlview.h"

// CDlgMemory 对话框

class CDlgMemory : public CDialog
{
	DECLARE_DYNAMIC(CDlgMemory)

public:
	CDlgMemory(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgMemory();

// 对话框数据
	enum { IDD = IDD_DLG_NOTIFY };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

	CXMLDataGrid   m_grid;
	int            m_currentRow;
public:
	CString m_szNote;

	CDateTimeCtrl m_cBeginNotifyTime;
	CDateTimeCtrl m_cEndNotifyTime;
	afx_msg void OnBnClickedBtnAdd();
	afx_msg void OnBnClickedBtnEdit();
	afx_msg void OnBnClickedBtnDelete();
	afx_msg void OnBnClickedBtnClose();
	afx_msg void OnGridSelectChange(NMHDR* pNMHDR, LRESULT* pResult);
	BOOL OnInitDialog();
	void OnTimer(UINT nIDEvent);
	void RefreshShow();
	afx_msg void OnEnChangeEdNotestr();
	BOOL CreateMe(CWnd* pParent);
	afx_msg void OnBnClickedBtnEdit2();
};
