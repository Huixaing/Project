#if !defined(TccCommand_FullCancel_H)
#define TccCommand_FullCancel_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// TccCommand_FullCancel.h : header file
//


#include "tcc.h"
#include "afxwin.h"
#include "afxdtctl.h"
#include <Tcc\Grid.h>
#include "resource.h"
// CTccCommand_FullCancel 对话框

namespace TCC_C3
{

class CTccCommand_FullCancel : public CDialog
{
	DECLARE_DYNAMIC(CTccCommand_FullCancel)

public:
	CTccCommand_FullCancel(stano_t nTccID, 
		ITccClientInfoProvider *pInfoProvider, 
		ITccAdviseSink *pAdviseSink,
		CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CTccCommand_FullCancel();
// 对话框数据
	enum { IDD = IDD_DLG_TCC_COMMAND_FULLCANCEL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	ITccClientInfoProvider *m_pInfoProvider;
	ITccAdviseSink *m_pAdviseSink;

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();

	// 目标受令站
	CComboBox m_TccStation;
	stano_t m_nTccID;
	// 线路
	CComboBox m_AbsLine;
protected:

public:
	afx_msg void OnCbnSelchangeTccStation();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedBtnDtccsend();
};

}

#endif //if !defined(TccCommand_FullCancel_H)