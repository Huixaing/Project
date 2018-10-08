#include "tcc.h"
#include "afxdtctl.h"
#include "afxcmn.h"
#include <Tcc\Grid.h>
#include "afxwin.h"

#pragma once

namespace TCC {

// CTccCommandInputDlg 对话框

class CTccCommandInputDlg : public CDialog
{
	DECLARE_DYNAMIC(CTccCommandInputDlg)

public:
	CTccCommandInputDlg(RAW_STOCK_COMMAND *pcmd, 
		BOOL bCheckCommand,
		ITccClientInfoProvider *pInfoProvider, 
		ITccAdviseSink *pAdviseSink,
		CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CTccCommandInputDlg();

// 对话框数据
	enum { IDD = IDD_TCC_COMMAND_INPUT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

private:
	RAW_STOCK_COMMAND m_cmd;
	BOOL  m_bCheckCommand;
	ITccClientInfoProvider *m_pInfoProvider;
	ITccAdviseSink *m_pAdviseSink;
	
	BOOL  m_bManualChangeCommandID;
	DWORD dwServerConfirmCookie;
	bool  m_bCmdIDConfirmed;	//服务器是否已确认
public:
	RAW_STOCK_COMMAND* GetCommand() { return &m_cmd; }

private:

	enum {
		STATION_LIMIT,	//站内限速
		ABSAPP_LIMIT,	//区间限速
		ABSAPP_FULL_LIMIT,	//区间全线限速
		BLOCK_LIMIT,	//侧线限速
	};
	// 命令类型
	CComboBox m_CmdType;

	// 限速命令号
	DWORD m_nCmdID;
	CEdit m_CmdID;
	CButton m_btnChangeCmdID;
	CButton m_btnAutoComputeCmdID;//自动选号

	// 是否取消命令
	BOOL m_bCancelLimit;
	CButton m_CancelLimit;
	// 是否生成取消命令
	BOOL m_bGenCancelCommand;
	CButton m_GenCancelCommand;
	// 开始站, 结束站
	CComboBox m_StaBegin, m_StaEnd;
	// 线路, 限速值
	CComboBox m_AbsLine;
	// 起始公里标
	int m_nKmBegin;
	int m_nMBegin;
	CEdit m_KmBegin;
	CEdit m_MBegin;
	// 结束公里标
	int m_nKmEnd;
	int m_nMEnd;
	CEdit m_KmEnd;
	CEdit m_MEnd;
	CComboBox m_StartLongKM;
	CComboBox m_StartLongM;
	CComboBox m_EndLongKM;
	CComboBox m_EndLongM;
	// 开始时间
	INT m_bTimeBegin;
	CDateTimeCtrl m_tmBegin;
	CButton m_BeginRadio;
	CButton m_BeginRadio2;
	// 结束时间
	INT m_bTimeEnd;
	CDateTimeCtrl m_tmEnd;
	CButton m_EndRadio;
	CButton m_EndRadio2;

	// 限速值
	CComboBox m_LimitVal[5];
	CButton   m_Limit[5];	//m_Limit[0]没有,仅仅是为了索引方便
	CEdit     m_TrainID[5];

	// 命令内容
	CString m_CmdString;

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();

	afx_msg void OnCbnSelchangeTccCmdSta();
	afx_msg void OnCbnSelchangeTccCmdSta2();

	afx_msg void OnBtnClickedTccCmdTimeendRadio2();
	afx_msg void OnBtnClickedTccCmdTimeendRadio();
	afx_msg void OnBtnClickedTccCmdTimebeginRadio2();
	afx_msg void OnBtnClickedTccCmdTimebeginRadio();
	afx_msg void OnCbnSelchangeTccCmdType();
	afx_msg void OnCbnSelchangeTccLine();
	afx_msg void OnBtnClickedLimit2();
	afx_msg void OnBtnClickedLimit3();
	afx_msg void OnBtnClickedLimit4();
	afx_msg void OnBtnClickedLimit5();
	afx_msg void OnSelCancelLimit();
	afx_msg void OnChangeTccCmdID();
	afx_msg void OnBtnClickedAutoComputeCmdID();

	afx_msg LRESULT OnServerConfirmCommandID(WPARAM wParam, LPARAM lParam);
protected:
	virtual void OnOK();
	virtual void OnCancel();

private:
	void RequestCommandID(cmdid_t nCmdID);

	CGridCtrl m_TccList;
	void InitListCtrl(CGridCtrl& grid);
};


};