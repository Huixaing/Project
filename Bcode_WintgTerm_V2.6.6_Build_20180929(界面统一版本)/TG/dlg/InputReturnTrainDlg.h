#pragma once
#include "afxwin.h"


// CInputReturnTrainDlg 对话框

class CInputReturnTrainDlg : public CDialog
{
	DECLARE_DYNAMIC(CInputReturnTrainDlg)

public:
	CInputReturnTrainDlg(CWnd* pParent, const CStringArray& list_trainno, const CDWordArray& list_trainindex);   // 标准构造函数
	virtual ~CInputReturnTrainDlg();

// 对话框数据
	enum { IDD = IDD_DLG_INPUT_RETURN_ID };

protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();

private:
	CStringArray m_list_trainno;
	CDWordArray m_list_trainindex;

public:
	long m_return_trainindex;
};
