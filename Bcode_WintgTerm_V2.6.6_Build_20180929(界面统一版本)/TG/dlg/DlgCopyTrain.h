#pragma once


// CDlgCopyTrain 对话框

class CDlgCopyTrain : public CDialog
{
	DECLARE_DYNAMIC(CDlgCopyTrain)

public:
	CDlgCopyTrain(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgCopyTrain();

// 对话框数据
	enum { IDD = IDD_DLG_COPY_TRAIN };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	CString m_trainId;
};
