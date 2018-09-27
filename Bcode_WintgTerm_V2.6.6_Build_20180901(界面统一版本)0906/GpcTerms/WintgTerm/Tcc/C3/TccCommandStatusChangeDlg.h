#if !defined(TccCommandStatusChangeDlg_H)
#define TccCommandStatusChange_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// TccCommandStatusChange.h : header file
//

#include "tcc.h"

namespace TCC_C3
{

// CTccCommandStatusChangeDlg 对话框

class CTccCommandStatusChangeDlg : public CDialog
{
	DECLARE_DYNAMIC(CTccCommandStatusChangeDlg)

public:
	CTccCommandStatusChangeDlg(stano_t nTccID, STOCK_COMMAND& stockcmd,
		ITccClientInfoProvider *pInfoProvider,
		ITccAdviseSink *pAdviseSink,
		CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CTccCommandStatusChangeDlg();

// 对话框数据
	enum { IDD = IDD_TCCCOMMAND_STATUS_CHANGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	CComboBox m_StatusList;
	CDateTimeCtrl m_ExecTime;

	stano_t m_nTccID;
	STOCK_COMMAND m_StockCmd;

	ITccClientInfoProvider *m_pInfoProvider;
	ITccAdviseSink *m_pAdviseSink;

	DECLARE_MESSAGE_MAP()
	LRESULT OnReceiveTccCommandResult(WPARAM wParam, LPARAM lParam);
public:
	CString m_sContent,m_sStatus;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedOk();
};

}

#endif //if !defined(TccCommandStatusChangeDlg_H)