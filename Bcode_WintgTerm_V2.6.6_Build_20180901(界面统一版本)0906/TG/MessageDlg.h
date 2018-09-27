#pragma once
#include "afxwin.h"


// CMessageDlg 对话框

class CMessageDlg : public CDialog
{
	DECLARE_DYNAMIC(CMessageDlg)

public:
	CMessageDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CMessageDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG_MSG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
    virtual void PostNcDestroy();

	DECLARE_MESSAGE_MAP()
public:
	void ShowMessage(CStringArray& text);
	CEdit m_textmsg;
	afx_msg void OnBnClickedOk();
	afx_msg void OnClose();
};
