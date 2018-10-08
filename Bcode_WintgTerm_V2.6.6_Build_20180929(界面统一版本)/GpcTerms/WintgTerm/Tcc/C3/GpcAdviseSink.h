
#ifndef __TCC_C3_GPC_ADIVSESINK_H__
#define __TCC_C3_GPC_ADVISESINK_H__

#include "tcc.h"
#include "GpcInfoProvider.h"

#pragma once

namespace TCC_C3
{

class CTccGpcAdviseSink : public ITccAdviseSink
{
public:
	CTccGpcAdviseSink(CTccGpcInfoProvider *pInfoProvider, CWnd *pWintgForm = NULL)
		: m_pWintgFormWnd(pWintgForm), m_pInfoProvider(pInfoProvider) { }
	virtual ~CTccGpcAdviseSink() {}

public:
	// 向一个列控中心发送限速命令，发送结果由函数值返回(0:成功，1:失败)
	virtual INT OnSendTccCommand(TCC_COMMAND& cmd, DWORD flag, HWND hTccDlg);
	virtual INT OnSendInitCommand(
		stano_t nTccID, 
		lineid_t nTccLineID, 
		dir_t dir,
		HWND hTccDlg);
	// 申请限速命令列表
	virtual INT OnRequestTccCmdList(stano_t nTccID, HWND hTccDlg);

	// 在实时列表上取消限速
	virtual INT OnCancelTccCommand(TCC_COMMAND& cmd, STOCK_COMMAND *pStockCmd, HWND hTccDlg);
	// 在确认列表上取消限速
	virtual INT OnCancelStockCommand(STOCK_COMMAND& cmd, HWND hTccDlg);

	// 发送确认命令
	virtual INT OnSendStockCommand(STOCK_COMMAND& cmd, HWND hTddDlg);
	// 删除某受令站确认命令
	virtual INT OnDelStockCommand(STOCK_COMMAND& cmd, stano_t nTccID, HWND hTccDlg);

	virtual INT OnRequestStockCommandList(stano_t nTccID, HWND hTccDlg, int flag);
	virtual INT OnRequestStockCommandList(WORD nConsoleID, stano_t nTccID, const dcmd_id_t& dcmd_id, time_t tmScopeBegin, time_t tmScopeEnd, HWND hTccDlg, int flag);

	virtual INT OnTccCommandFileChanged(
					LPCSTR szFilename, 
					DWORD dwOper);

	// 预留
	virtual INT OnTccCommandStatusChanged(
					TCC_COMMAND &cmd,
					DWORD status,
					DWORD dwParam);

	virtual void OnLogPrint(LPCSTR szLog);
	void SetStockCommandStatus(STOCKCOMMAND_STATUS_SETTING_MODE mode, STOCK_COMMAND_STATUS_CHANGE_INFO& info, HWND hTccDlg);

	BOOL ConfirmCommandID(int nConsoleID, cmdid_t nCommandID, cmdid_t nMinCommandID, cmdid_t nMaxCommandID, int& who_confirm_cmdid, DWORD dwCookie, HWND hTccDlg);

private:
	CWnd *m_pWintgFormWnd;
	CTccGpcInfoProvider *m_pInfoProvider;
};


};

#endif
