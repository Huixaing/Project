
/*
 * TCC 实现内部接口定义
 *
 */
#ifndef __TCC_BASE_H__
#define __TCC_BASE_H__

#include "Tcc.h"
#include <map>

using namespace std;

#pragma once

namespace TCC {

BOOL TccInitialize(ITccClientInfoProvider *pInfoProvider);

BOOL LoadTccEnv(LPCSTR szFilename);
BOOL LoadCommandIDInfo(LPCSTR szFilename);
BOOL SaveCommandIDInfo(LPCSTR szFilename);
BOOL LoadTccConfig(LPCSTR szFilename);

CString GetStockCommandStatusString(DWORD nStatus);
//BOOL CanEditLimitSpeedCommand(DWORD nStatus);
//BOOL CanReSendLimitSpeedCommand(DWORD nStatus);
//BOOL CanDeleteLimitSpeedCommand(DWORD nStatus);

BOOL GetTccStatus(stano_t nTccID, TCC_STATUS *pStatus);
void SetTccStatus(stano_t nTccID, TCC_STATUS& status);
void ResetTccStatus();

HWND GetTccDialogWndHandle();

BOOL LoadRawStockCommand(
		MSXML2::IXMLDOMNodePtr pCmdNode, 
		RAW_STOCK_COMMAND &cmd);
BOOL SaveRawStockCommand(
		MSXML2::IXMLDOMNodePtr pCmdNode, 
		RAW_STOCK_COMMAND &cmd);
BOOL DeletecCommandNode(MSXML2::IXMLDOMDocumentPtr pDOMDoc, sysid_t& sysid);
BOOL AddCommandNode(
		MSXML2::IXMLDOMDocumentPtr pDOMDoc, 
		RAW_STOCK_COMMAND& cmd, 
		BOOL bInsert);

BOOL DeleteCommand(RAW_STOCK_COMMAND& cmd, LPCSTR szFilename);
void DumpFile(LPCSTR szFilename);
void EraseZeroFileInDir(LPCSTR szDirectory);
BOOL IsOldCommandFile(LPCSTR szFilename);

CString GetDifferenceReason(RAW_STOCK_COMMAND& lhs, RAW_STOCK_COMMAND& rhs, ITccClientInfoProvider *pInfoProvider);

// 服务器确认命令号
#define UM_SERVER_CONFIRM_COMMAND_ID	(WM_USER + 101)
struct SERVER_CONFIRM_COMMAND_ID_STRUCT
{
	bool bValid;

	int nConsoleID;
	cmdid_t nCmdID;
	DWORD dwCookie;
};

void RegisterConfirmCommandIDReceiver(HWND hWnd);
void UnRegisterConfirmCommandIDReceiver(HWND hWnd);


};



#endif
