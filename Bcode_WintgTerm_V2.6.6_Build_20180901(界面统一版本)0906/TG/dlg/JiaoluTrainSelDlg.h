#pragma once
#include "afxwin.h"


// CJiaoluTrainSelDlg 对话框

class CJiaoluTrainSelDlg : public CDialog
{
	DECLARE_DYNAMIC(CJiaoluTrainSelDlg)

public:
	CJiaoluTrainSelDlg(CWnd* pParent, const CStringArray& list_trainno, const CDWordArray& list_trainindex);   // 标准构造函数
	virtual ~CJiaoluTrainSelDlg();

// 对话框数据
	enum { IDD = IDD_DLG_JIAOLU_SELECT_TRAIN };

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
