#pragma once


// CDlgLogFilter 对话框

class CDlgLogFilter : public CDialog
{
	DECLARE_DYNAMIC(CDlgLogFilter)

public:
	CDlgLogFilter(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgLogFilter();

// 对话框数据
	enum { IDD = IDD_DLG_LOG_FILTER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();
public:
	afx_msg void OnLbnDblclkListStationName();
	afx_msg void OnLbnDblclkListUsefulKeys();
	CString m_szFilterKey;
	afx_msg void OnBnClickedClearKey();
	afx_msg void OnBnClickedOk();
};
