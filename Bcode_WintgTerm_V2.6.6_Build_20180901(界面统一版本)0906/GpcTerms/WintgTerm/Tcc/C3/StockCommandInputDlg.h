#if !defined(StockCommandInputDlg_H)
#define StockCommandInputDlg_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// StockCommandInputDlg.h : header file
//

#include "tcc.h"
#include "afxdtctl.h"
#include "afxcmn.h"
#include <Tcc\Grid.h>
#include "afxwin.h"

#pragma once

namespace TCC_C3 {

// CStockCommandInputDlg 对话框

class CStockCommandInputDlg : public CDialog
{
	DECLARE_DYNAMIC(CStockCommandInputDlg)

public:
	CStockCommandInputDlg(RAW_STOCK_COMMAND *pcmd, 
		BOOL bCheckCommand,
		ITccClientInfoProvider *pInfoProvider, 
		ITccAdviseSink *pAdviseSink,
		CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CStockCommandInputDlg();

// 对话框数据
	enum { IDD = IDD_TCC_COMMAND_INPUT2 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

private:
	RAW_STOCK_COMMAND m_cmd;
	BOOL  m_bCheckCommand;
	ITccClientInfoProvider *m_pInfoProvider;
	ITccAdviseSink *m_pAdviseSink;
	BOOL m_BtnEn;//示意图是否可以显示滑块
	
	BOOL  m_bManualChangeCommandID;
	DWORD dwServerConfirmCookie;
	bool  m_bCmdIDConfirmed;	//服务器是否已确认
public:
	RAW_STOCK_COMMAND* GetCommand() { return &m_cmd; }

private:

	enum {
		STATION_LIMIT		= 1,	//站内限速
		ABSAPP_LIMIT		= 2,		//区间限速
		ABSAPP_FULL_LIMIT	= 3,	//区间全线限速
		KM_LIMIT = 4, //公里标限速
	};
	// 命令类型
	CComboBox m_CmdType;
	int m_nCmdType;

	// 限速命令号
	DWORD m_nCmdID;
	CEdit m_CmdID;
	CButton m_btnChangeCmdID;
	CButton m_btnAutoComputeCmdID;//自动选号

	// 是否取消命令
	BOOL m_bCancelLimit;
	CButton m_CancelLimit;
	// 开始站, 结束站
	CComboBox m_StaBegin, m_StaEnd;
	stano_t m_nStaBegin, m_nStaEnd;
	// 线路, 限速值
	CComboBox m_AbsLine;
	int m_nLineID;
	// 起始公里标
	CString m_szKmBegin;
	CEdit m_KmBegin;
	CEdit m_MBegin;
	CComboBox m_Kcl[2], m_Mcl[2];
	// 结束公里标
	CString m_szKmEnd;
	CEdit m_KmEnd;
	CEdit m_MEnd;
	// 公里标最大与最小值
	CString m_szKmBegin2, m_szKmEnd2;
	kmmark_t m_nMinKm, m_nMaxKm;
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

	CComboBox m_LimitVal[1];

	CEdit   m_ValidKmMarker;//线路有效公里标范围
	CEdit   m_AdjustKmMarker;//线路归档后公里标范围
	CButton m_btnDirDown, m_btnDirUp, m_btnDirDual;

	// 命令内容
	CString m_CmdString;

	kmmark_t m_nKmBeginAJUST,m_nKmEndAJUST;
	BOOL m_bKmMarkAdjustAck;

	int m_nDirection, m_nDirection2;
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();

	afx_msg void OnBtnClickedTccCmdTimeendRadio2();
	afx_msg void OnBtnClickedTccCmdTimeendRadio();
	afx_msg void OnBtnClickedTccCmdTimebeginRadio2();
	afx_msg void OnBtnClickedTccCmdTimebeginRadio();
	afx_msg void OnCbnSelchangeTccCmdType();
	afx_msg void OnCbnSelchangeTccCmdSta();
	afx_msg void OnCbnSelchangeTccCmdSta2();
	afx_msg void OnCbnSelchangeTccLine();
	afx_msg void OnKmMarkerChanged();
	void AdjustKmMarker();
    afx_msg void OnBnClickedBtnAdjustAck(); //IDC_BTN_ADJUST_ACK
	afx_msg void OnSelDirection();
	afx_msg void OnSketchMap();
	afx_msg void OnChangeTccCmdID();
	afx_msg void OnBtnClickedAutoComputeCmdID();

	afx_msg LRESULT OnServerConfirmCommandID(WPARAM wParam, LPARAM lParam);
protected:
	virtual void OnOK();
	virtual void OnCancel();

private:
	void RequestCommandID(cmdid_t nCmdID);

	class CTccList
	{
	public:
		CTccList();
		virtual ~CTccList();

		BOOL Create(const RECT& rect, CWnd *pParent, UINT nID);
		void Clear();
		void SetTccList(vector<RAW_STOCK_COMMAND::_TCC_STRUCT>& tccs, ITccClientInfoProvider *pInfoProvider);
		void GetTccList(vector<RAW_STOCK_COMMAND::_TCC_STRUCT>& tccs);
	private:
		CGridCtrl grid;
		std::vector<RAW_STOCK_COMMAND::_TCC_STRUCT> m_tccs;
	} m_TccList;
};


};

#endif //if !defined(StockCommandInputDlg_H)
