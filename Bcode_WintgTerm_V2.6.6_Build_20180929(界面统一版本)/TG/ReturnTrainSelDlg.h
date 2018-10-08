#pragma once
#include "afxwin.h"


// CReturnTrainSelDlg 对话框

class CReturnTrainSelDlg : public CDialog
{
	DECLARE_DYNAMIC(CReturnTrainSelDlg)

public:
	CReturnTrainSelDlg(CWnd* pParent, const CStringArray& list_trainno, const CDWordArray& list_trainindex);   // 标准构造函数
	virtual ~CReturnTrainSelDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG_SEL_TRAIN };

protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();

private:
	CComboBox m_list;
	CStringArray m_list_trainno;
	CDWordArray m_list_trainindex;

public:
	long m_return_trainindex;
};
