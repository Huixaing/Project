#pragma once


// CDlgWordMaintain 对话框

class CDlgWordMaintain : public CDialog
{
	DECLARE_DYNAMIC(CDlgWordMaintain)

public:
	CDlgWordMaintain(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgWordMaintain();

// 对话框数据
	enum { IDD = IDD_DLG_WORD_MAINTAIN };
	CEdit m_newword; //输入新词汇
	CListBox m_listallwords;//常用词汇列表框
	CButton m_buttonadd;//添加按纽
	CButton m_buttondel;//删除按纽
	CButton m_buttonedit;//编辑按钮
	CComboBox m_cbotype;

	CString   m_szFileName;
	BOOL      m_bEdited;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnWordAdd();
	afx_msg void OnBnClickedBtnWordEdit();
	afx_msg void OnBnClickedBtnWordDelete();
	afx_msg void OnSelTypeChange();
	afx_msg void OnSelWordChange();
	afx_msg void OnSave();
	BOOL OnInitDialog();
	void RefreshList();
	void OnOK();
};
