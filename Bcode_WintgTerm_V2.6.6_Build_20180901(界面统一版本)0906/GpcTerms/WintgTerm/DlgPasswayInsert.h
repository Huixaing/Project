#pragma once


// CDlgPasswayInsert 对话框

class CDlgPasswayInsert : public CDialog
{
	DECLARE_DYNAMIC(CDlgPasswayInsert)

public:
	CDlgPasswayInsert(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgPasswayInsert();

// 对话框数据
	enum { IDD = IDD_DLG_PASSWAY_INSERT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	// 径路号
	short m_nPasswayId;
	// 径路名称
	CString  m_nPasswayName;
	afx_msg void OnBnClickedOk();
};
