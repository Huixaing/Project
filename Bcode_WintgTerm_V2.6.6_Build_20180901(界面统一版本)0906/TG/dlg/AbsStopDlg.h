#pragma once


// CAbsStopDlg 对话框

class CAbsStopDlg : public CDialog
{
	DECLARE_DYNAMIC(CAbsStopDlg)

public:
	CAbsStopDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CAbsStopDlg();

// 对话框数据
	enum { IDD = IDD_DLG_ABS_STOP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CString m_reason;
	DWORD m_abs_stop_time;
};
