
#ifndef __TCC_GPC_ADIVSESINK_H__
#define __TCC_GPC_ADVISESINK_H__

#include "tcc.h"
#include "GpcInfoProvider.h"

#pragma once

namespace TCC
{

class CTccGpcAdviseSink : public ITccAdviseSink
{
public:
	CTccGpcAdviseSink(CTccGpcInfoProvider *pInfoProvider, CWnd *pWintgForm = NULL)
		: m_pWintgForm(pWintgForm), m_pInfoProvider(pInfoProvider) { }
	virtual ~CTccGpcAdviseSink() {}

public:
	// 向一个列控中心发送限速命令，发送结果由函数值返回(0:成功，1:失败)
	virtual INT OnSendTccCommand(TCC::TCC_COMMAND& cmd, HWND hTccDlg);
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


	BOOL ConfirmCommandID(int nConsoleID, cmdid_t nCommandID, cmdid_t nMinCommandID, cmdid_t nMaxCommandID, int& who_confirm_cmdid, DWORD dwCookie, HWND hTccDlg);

private:
	CWnd *m_pWintgForm;
	CTccGpcInfoProvider *m_pInfoProvider;
};

}

#endif
