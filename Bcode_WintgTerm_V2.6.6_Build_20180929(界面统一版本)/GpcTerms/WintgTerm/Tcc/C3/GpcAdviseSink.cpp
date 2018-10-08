// TCC.cpp : 实现文件
//

#include "stdafx.h"
#include "GpcAdviseSink.h"
#include <Tcc\BMsgSend.h>
#include <sysshare.h>
#include <oscalls.h>
#include <widgets.h>
#include <MsgDispatch.h>
#include "..\TCC_COMMON_FILE.h"
		
namespace TCC_C3
{


INT CTccGpcAdviseSink::OnSendInitCommand(stano_t nTccID, lineid_t nTccLineID, dir_t dir,HWND hTccDlg)
{
	CLBTH::MsgStccInitComplete msg;//		MsgStccCommandSend msgSCS;

//		msg.bCancelLimit = stTccInfo.bCancelLimit;
	msg.console_no = 0;
//		SysidToByteArray(stTccInfo.sys_id,msgSCS.sys_id,sizeof(msgSCS.sys_id));
//		msgSCS.cmd_no = stTccInfo.nCmdID;
//		msgSCS.sta_no[0] = stTccInfo.nStaID[0];
//		msgSCS.sta_no[1] = stTccInfo.nStaID[1];
//		msgSCS.line_no = stTccInfo.nLineID;
//		msgSCS.bFullLimit = stTccInfo.bFullLimit;
//		msgSCS.km_marker[0] = stTccInfo.nKmMark[0];
//		msgSCS.km_marker[1] = stTccInfo.nKmMark[1];
//		msgSCS.bStartNow = stTccInfo.bStartNow;
//		msgSCS.bEndless = stTccInfo.bEndless;
//		msgSCS.time[0] = stTccInfo.nTime[0];
//		msgSCS.time[1] = stTccInfo.nTime[1];
//		msgSCS.speed = stTccInfo.nSpeedMaxVal;
	msg.tcc_no = nTccID;
	//msg.tcc_sender = g_csStpcConfig.GetLocalEntityID();
	//msgSCS.tcc_line_no = nLineID;
	//msgSCS.bInitOpration = TRUE;
	//switch(nDir)
	//{
	//case DIR_DOWN:
	//	msgSCS.section_dir = 1;
	//	break;
	//case DIR_UP:
	//	msgSCS.section_dir = 0;
	//	break;
	//case DIR_DUAL:
	//	msgSCS.section_dir = 2;
	//	break;
	//default:
	//	break;
	//}

	GPCENTITY entity;
	if(gpcdef.GetEntityInfo(gpcdef.GetCurUserName(), entity))
	{
		msg.tcc_sender = entity.nEntityId;
	}

	CString prompt;
	INT nRet = 1;

	do {
		if(0 == nTccID || !gpcdef.GetEntityInfo(102, nTccID, entity))
		{
			prompt.Format("站%d 找不到自律机, 操作失败!", nTccID);
			break;
		}

		//本站自律机没有逻辑功能，查找相邻站的自律机
		if(entity.nSpecial2 != 0 && !gpcdef.GetEntityInfo(102, entity.nSpecial2, entity))
		{
			prompt.Format("站%d 找不到相邻站的自律机, 操作失败!", nTccID);
			break;
		}

		SendBMsg(&msg, entity.nEntityId);
		

		GpcLogTrainMessage(0, nTccID, 
			"[TCC]: send 限速初始化命令 to 列控=%s[%d] 线路=%s[%d] 方向=%d ", 
			m_pInfoProvider->GetStationName(nTccID), nTccID, 
			m_pInfoProvider->GetTccLineName(nTccID, nTccLineID), nTccLineID, 
			dir);

		nRet = 0;
	} while(0);
	if(0 != nRet)
	{
		if(hTccDlg)
			::MessageBox(hTccDlg, prompt, NULL, MB_OK);
		else
			AfxMessageBox(prompt);
	}

	return nRet;
}

// 发送TCC_COMMAND到列控
INT CTccGpcAdviseSink::OnSendTccCommand(TCC_COMMAND& cmd, DWORD flag, HWND hTccDlg)
{
	ASSERT(cmd.nTccID != 0);

	CLBTH::MsgStccCommandSend2 msg;
	msg.bInitOpration = FALSE;

	msg.console_no = cmd.nConsoleID;
	SysidToByteArray(cmd.sys_id, msg.sys_id, sizeof(msg.sys_id));
	msg.cmd_no = cmd.nCmdID;		// 命令号(0~9999)
	msg.sta_no[0] = cmd.nStaID[0];		// 开始站
	msg.sta_no[1] = cmd.nStaID[1];		// 结束站
	msg.line_no = cmd.nLineID;		// 线路号
	msg.bFullLimit = cmd.bFullLimit;	// 是否全线限速
	msg.km_marker[0].nKmsysCode = cmd.nKmMark[0].flag;	// 开始、结束公里标（以m为单位）
	msg.km_marker[0].k_cl       = cmd.nKmMark[0].info.km_cl;
	msg.km_marker[0].m_cl       = cmd.nKmMark[0].info.m_cl;
	msg.km_marker[0].val        = cmd.nKmMark[0].info.val;
	msg.km_marker[1].nKmsysCode = cmd.nKmMark[1].flag;
	msg.km_marker[1].k_cl       = cmd.nKmMark[1].info.km_cl;
	msg.km_marker[1].m_cl       = cmd.nKmMark[1].info.m_cl;
	msg.km_marker[1].val        = cmd.nKmMark[1].info.val;
	msg.bStartNow = cmd.bStartNow;		// 是否立即开始
	msg.bEndless = cmd.bEndless;		// 是否永久有效
	msg.time[0] = cmd.nTime[0];			// 开始时间(秒)
	msg.time[1] = cmd.nTime[1];			// 结束时间(秒)
	msg.speed = cmd.nSpeedMaxVal;		// 限速值(5KM/H的倍数)

	msg.tcc_no = cmd.nTccID;
	msg.tcc_line_no = cmd.nTccLineID;
	msg.tcc_sender = 0;
	msg.tcc_km_marker[0].nKmsysCode = ((cmd.nTccKmMarkFlag[0] & 0x03) << 6) + (cmd.nKmMark[0].flag & 0x3F);
	msg.tcc_km_marker[0].k_cl       = cmd.nKmMark[0].info.km_cl;
	msg.tcc_km_marker[0].m_cl       = cmd.nKmMark[0].info.m_cl;
	msg.tcc_km_marker[0].val        = cmd.nKmMark[0].info.val;
	msg.tcc_km_marker[1].nKmsysCode = ((cmd.nTccKmMarkFlag[1] & 0x03) << 6) + (cmd.nKmMark[1].flag & 0x3F);
	msg.tcc_km_marker[1].k_cl       = cmd.nKmMark[1].info.km_cl;
	msg.tcc_km_marker[1].m_cl       = cmd.nKmMark[1].info.m_cl;
	msg.tcc_km_marker[1].val        = cmd.nKmMark[1].info.val;

	msg.bCancelLimit = cmd.bCancelLimit;

	if(flag == 1)
		msg.op = 1;
	else if(flag == 2)
		msg.op = 2;
	else if(flag == 3)
		msg.op = 3;
	else
	{
		MessageBox(hTccDlg, "不认识的操作标志", NULL, MB_OK);
		return 1;
	}

	GPCENTITY entity;
	if(gpcdef.GetEntityInfo(gpcdef.GetCurUserName(),entity))
	{
		msg.tcc_sender = entity.nEntityId;
		
		cmd.nTccSenderID = entity.nEntityId;//最好是自律机返回
	}

	CString prompt;
	INT nRet = 1;
	do {
		if(0 == cmd.nTccID || !gpcdef.GetEntityInfo(102, cmd.nTccID, entity))
		{
			prompt.Format("站%d 找不到自律机, 操作失败!", cmd.nTccID);
			break;
		}

		//本站自律机没有逻辑功能，查找相邻站的自律机
		if(entity.nSpecial2 != 0 && !gpcdef.GetEntityInfo(102, entity.nSpecial2, entity))
		{
			prompt.Format("站%d 找不到相邻站的自律机, 操作失败!", cmd.nTccID);
			break;
		}

		SendBMsg(&msg,entity.nEntityId);
		

		CString szLog, sz;
		szLog.Format("[TCC]: send TCC_COMMAND to 列控(%s[%d]) 命令号(%d) 线路(%s[%d]) 公里标%s--%s ",
			m_pInfoProvider->GetStationName(cmd.nTccID), cmd.nTccID,
			cmd.nCmdID, 
			m_pInfoProvider->GetTccLineName(cmd.nTccID, cmd.nTccLineID), cmd.nTccLineID,
			cmd.nKmMark[0].toString(m_pInfoProvider, cmd.nLineID),
			cmd.nKmMark[1].toString(m_pInfoProvider, cmd.nLineID)
			);

		if(cmd.bCancelLimit)
			sz.Format("取消限速");
		else
			sz.Format("限速值(%d)", cmd.nSpeedMaxVal);
		szLog += sz;

		switch(flag)
		{
		case 1:
			sz = " 校验";
			break;
		case 2:
			sz = " 执行";
			break;
		case 3:
			sz = " 初始化完成";
			break;
		}
		szLog += sz;

		GpcLogTrainMessage(0, cmd.nTccID, szLog);

		nRet = 0;
	} while(0);
	if(0 != nRet)
	{
		if(hTccDlg)
			::MessageBox(hTccDlg, prompt, NULL, MB_OK);
		else
			AfxMessageBox(prompt);
	}

	return nRet;
}

// 向列控申请 限速命令列表
INT CTccGpcAdviseSink::OnRequestTccCmdList(stano_t nTccID, HWND hTccDlg)
{
	CLBTH::MsgStccCommandListQuery msg;

	msg.inquirer = 0;	// 发令设备
	GPCENTITY entity;
	if(gpcdef.GetEntityInfo(gpcdef.GetCurUserName(),entity))
		msg.inquirer = entity.nEntityId;

	msg.tcc_no = nTccID;

	CString prompt;
	INT nRet = 1;
	do {
		if(0 == nTccID || !gpcdef.GetEntityInfo(102, nTccID, entity))
		{
			prompt.Format("站%d 找不到自律机, 操作失败!", nTccID);
			break;
		}

		//本站自律机没有逻辑功能，查找相邻站的自律机
		if(entity.nSpecial2 != 0 && !gpcdef.GetEntityInfo(102, entity.nSpecial2, entity))
		{
			prompt.Format("站%d 找不到相邻站的自律机, 操作失败!", nTccID);
			break;
		}
	   SendBMsg(&msg,entity.nEntityId);
		
		GpcLogDebugMessage(0, "[TCC]: send MsgStccCommandListQuery to 列控=%s[%d]",
			m_pInfoProvider->GetStationName(nTccID), nTccID);

		nRet = 0;
	} while(0);
	if(0 != nRet)
	{
		if(hTccDlg)
			::MessageBox(hTccDlg, prompt, NULL, MB_OK);
		else
			AfxMessageBox(prompt);
	}

	return nRet;
}



// 从实时列表取消命令，要求三方校验
INT CTccGpcAdviseSink::OnCancelTccCommand(TCC_COMMAND& cmd, STOCK_COMMAND *pStockCmd, HWND hTccDlg)
{
	INT nRet = -1;
	CString prompt;

	do {
		/*GPCENTITY entity;
		if(!gpcdef.GetEntityInfo(cmd.nTccSenderID, entity))
		{
			prompt.Format("保护命令， 不能取消");
			break;
		}

		// 助调、行调
		if(1 != entity.nGroupId && 2 != entity.nGroupId)
		{
			prompt.Format("不是助调、或行调台");
			break;
		}
		else if(entity.nSpecial1 != cmd.nConsoleID && 0 != cmd.nConsoleID)
		{
			prompt.Format("不是本台下发的命令，不能取消!");
			break;
		}*/
		if(0 != cmd.nConsoleID)
		{
			if(m_pInfoProvider->GetConsoleID() != cmd.nConsoleID ||
				pStockCmd && pStockCmd->nConsoleID != m_pInfoProvider->GetConsoleID() )
			{
				prompt.Format("此限速命令不是本调度台下发的， 无权取消!");
				break;
			}
		}
//#if 0
//		if(m_pWintgForm)
//			m_pWintgForm->SendMessage(UM_TCC_LIMITSPEED_CMD_CANCEL, (WPARAM)&cmd, (LPARAM)pStockCmd);
//#endif 
		TCC_C3::RAW_STOCK_COMMAND raw_cmd;		
		if(pStockCmd != NULL)
		{
			pStockCmd->ComposeRawStcokCommand(raw_cmd);
		}
		else
		{
			cmd.ComposeRawStcokCommand(raw_cmd);
		}
		
		raw_cmd.bCancelLimit = TRUE;
		raw_cmd.bGenCancelCommand = FALSE;
		
		if(raw_cmd.nCmdType == 0)
		{
			if (raw_cmd.nLineID <= 0)
				raw_cmd.nCmdType = 1;
			else
				raw_cmd.nCmdType = 4;
		}
		
		SendCancelDCmdC3(raw_cmd,cmd.nConsoleID,2);
		
		nRet = 0;
	} while(0);

	if(0 != nRet)
	{
		if(hTccDlg)
			MessageBox(hTccDlg, prompt, "注意", MB_ICONWARNING|MB_OK);
		else
			AfxMessageBox(prompt);
	}

	return nRet;
}

INT CTccGpcAdviseSink::OnCancelStockCommand(STOCK_COMMAND& cmd, HWND hTccDlg)
{
	return 0;
}


// 向服务器发送确认命令
INT CTccGpcAdviseSink::OnSendStockCommand(STOCK_COMMAND& cmd, HWND hTccDlg)
{
	CLBTH::MsgTccCommandAdd2 msg;
	memcpy(msg.dcmd_no, cmd.dcmd_id, sizeof(msg.dcmd_no));	//调度命令号
	msg.console_no = cmd.nConsoleID;	// 调度台 台号
	SysidToByteArray(cmd.sys_id, msg.sys_id, 16);		// 系统标识
	SysidToByteArray(cmd.raw_sysid, msg.raw_sysid, 16);// 原始命令系统标识

	msg.cmd_no = cmd.nCmdID;			// 命令号（0~9999）
	msg.sta_no[0] = cmd.nStaID[0];		// 开始、结束站号
	msg.sta_no[1] = cmd.nStaID[1];

	msg.line_no   = cmd.nLineID;		// 线路号
	msg.bFullLimit= cmd.bFullLimit;		// 是否全线限速
	msg.km_marker[0].nKmsysCode = cmd.nKmMark[0].flag;	// 开始、结束公里标（以m为单位）
	msg.km_marker[0].k_cl       = cmd.nKmMark[0].info.km_cl;
	msg.km_marker[0].m_cl       = cmd.nKmMark[0].info.m_cl;
	msg.km_marker[0].val        = cmd.nKmMark[0].info.val;
	msg.km_marker[1].nKmsysCode = cmd.nKmMark[1].flag;
	msg.km_marker[1].k_cl       = cmd.nKmMark[1].info.km_cl;
	msg.km_marker[1].m_cl       = cmd.nKmMark[1].info.m_cl;
	msg.km_marker[1].val        = cmd.nKmMark[1].info.val;
	msg.bStartNow = cmd.bStartNow;		// 是否立即开始
	msg.bEndless  = cmd.bEndless;		// 是否永久有效
	msg.time[0]   = cmd.nTime[0];		// 开始、结束时间
	msg.time[1]   = cmd.nTime[1];
	msg.speed     = cmd.nSpeedMaxVal;	// 限速值
	msg.bCancelLimit = cmd.bCancelLimit;// 是否 取消限速命令

	msg.creator = cmd.nCreatorID;		// 原始命令草拟人
	msg.checkor = cmd.nCheckorID;		// 中心校验者
	msg.confirmor = cmd.nConfirmorID;	// 确认分解者

	msg.tcc_num = cmd.tcc.size();		// 受令站个数
	for(int i=0; i<cmd.tcc.size(); i++)
	{
		msg.tcc_no[i] = cmd.tcc[i].nID;				// 受令站（列控）
		msg.tcc_line_no[i] = cmd.tcc[i].nTccLineID; // 受令站线路
		msg.tcc_status[i] = cmd.tcc[i].nStatus;		// 受令站命令状态
		msg.tcc_sender[i] = cmd.tcc[i].nSenderID;	// 受令站发令人（车站、助调、行调）
		msg.tcc_km_marker[i][0].nKmsysCode = ((cmd.tcc[i].nKmMarkFlag[0] & 0x03) << 6) + (cmd.nKmMark[0].flag & 0x3F);
		msg.tcc_km_marker[i][0].k_cl       = cmd.nKmMark[0].info.km_cl;
		msg.tcc_km_marker[i][0].m_cl       = cmd.nKmMark[0].info.m_cl;
		msg.tcc_km_marker[i][0].val        = cmd.nKmMark[0].info.val;
		msg.tcc_km_marker[i][1].nKmsysCode = ((cmd.tcc[i].nKmMarkFlag[1] & 0x03) << 6) + (cmd.nKmMark[1].flag & 0x3F);
		msg.tcc_km_marker[i][1].k_cl       = cmd.nKmMark[1].info.km_cl;
		msg.tcc_km_marker[i][1].m_cl       = cmd.nKmMark[1].info.m_cl;
		msg.tcc_km_marker[i][1].val        = cmd.nKmMark[1].info.val;
		msg.tcc_time[i] = 0;
	}

	msg.nMarkIndex = 0;		// 运行图区间慢行标志

	long  eid=GetSpecialEntityID(SERVER_TYPE_DATASTORE, m_pInfoProvider->GetConsoleID());
	SendBMsg(&msg, eid);
	
	GpcLogTrainMessage(0,0, "[TCC]: 增加服务器确认命令 命令号=%d sysid=%s",
		cmd.nCmdID, SysidToString(cmd.sys_id));

	return 0;
}

// 删除服务器确认命令 STOCK_COMMAND 的 受令站
// 确认命令的受令站完全删除后 则删除确认命令
INT CTccGpcAdviseSink::OnDelStockCommand(STOCK_COMMAND& cmd, stano_t nTccID, HWND hTccDlg)
{
	CLBTH::MsgTccCommandDel msg;
	msg.console_no = cmd.nConsoleID;		// 调度台 台号
	SysidToByteArray(cmd.sys_id, msg.sys_id, 16);
	msg.cmd_no = cmd.nCmdID;
	msg.tcc_no = nTccID;

	long  eid=GetSpecialEntityID(SERVER_TYPE_DATASTORE, m_pInfoProvider->GetConsoleID()); 
	SendBMsg(&msg, eid);
	
	GpcLogTrainMessage(0,0, "[TCC]: 删除服务器确认命令 命令号=%d 站号=%s[%d] sysid=%s",
		cmd.nCmdID, m_pInfoProvider->GetStationName(nTccID), nTccID, SysidToString(cmd.sys_id));

	return 0;
}

// 向服务器申请确认命令列表 STOCK_COMMAND
INT CTccGpcAdviseSink::OnRequestStockCommandList(stano_t nTccID, HWND hTccDlg, int flag)
{
	CLBTH::MsgTccCommandQuery2 msg;
	msg.console_no = 0;
	msg.tcc_no = nTccID;
	if(0 == nTccID)
		msg.flag = CLBTH::MsgTccCommandQuery2::QUERY_ALL;
	else
		msg.flag = CLBTH::MsgTccCommandQuery2::QUERY_BY_TCCNO;
	if(flag == 0)
		msg.type = CLBTH::MsgTccCommandQuery2::QUERY_CURRENT;
	else
		msg.type = CLBTH::MsgTccCommandQuery2::QUERY_HISTORY;

	long  eid=GetSpecialEntityID(SERVER_TYPE_DATASTORE, m_pInfoProvider->GetConsoleID()); 
	SendBMsg(&msg, eid);

	CString szLog, sz;
	szLog.Format("[TCC]: 申请服务器 %d 确认命令列表 站=%s[%d]", 
		eid, m_pInfoProvider->GetStationName(nTccID), nTccID);

	if(flag == 0)
		sz.Format(" 查询确认列表");
	else
		sz.Format(" 查询历史列表");
	szLog += sz;

	GpcLogDebugMessage(0, sz);

	return 0;
}

INT CTccGpcAdviseSink::OnRequestStockCommandList(WORD nConsoleID, stano_t nTccID, const dcmd_id_t& dcmd_id, time_t tmScopeBegin, time_t tmScopeEnd, HWND hTccDlg, int flag)
{
	CLBTH::MsgTccCommandQuery2 msg;

	if(flag == 0)
		msg.type = CLBTH::MsgTccCommandQuery2::QUERY_CURRENT;
	else
		msg.type = CLBTH::MsgTccCommandQuery2::QUERY_HISTORY;

	msg.console_no = nConsoleID;
	msg.tcc_no = nTccID;
	memcpy(msg.dcmd_no, dcmd_id, sizeof(msg.dcmd_no));
	msg.time_scope[0] = tmScopeBegin;
	msg.time_scope[1] = tmScopeEnd;
	msg.flag = CLBTH::MsgTccCommandQuery2::QUERY_BY_TCCNO;

	long  eid=GetSpecialEntityID(SERVER_TYPE_DATASTORE, m_pInfoProvider->GetConsoleID());
	SendBMsg(&msg, eid);

	CString szLog, sz;
	szLog.Format("[TCC]: 申请服务器 %d 确认命令列表 站=%s[%d]", 
		eid, m_pInfoProvider->GetStationName(nTccID), nTccID);

	if(flag == 0)
		sz.Format(" 查询确认列表");
	else
		sz.Format(" 查询历史列表");
	szLog += sz;

	GpcLogDebugMessage(0, sz);

	return 0;
}


INT CTccGpcAdviseSink::OnTccCommandFileChanged(
		LPCSTR szFilename, 
		DWORD dwOper)
{
	return 0;
}


INT CTccGpcAdviseSink::OnTccCommandStatusChanged(
		TCC_COMMAND &cmd,
		DWORD status,
		DWORD dwParam)
{
	//if(m_pWintgFormWnd)
	//	((CWintgForm*)m_pWintgFormWnd)->SetFigureStatusColor(dwParam, status);

	return 0;
}


void CTccGpcAdviseSink::OnLogPrint(LPCSTR szLog)
{

	USHORT usCommQueueHandle = -1;
	USHORT nTaskId;
	OsGetTaskId(&nTaskId);

	sysprint(0, nTaskId, "[TCC]: %s", szLog);
}

void CTccGpcAdviseSink::SetStockCommandStatus(STOCKCOMMAND_STATUS_SETTING_MODE mode, STOCK_COMMAND_STATUS_CHANGE_INFO& info, HWND hTccDlg)
{
	CLBTH::MsgTccCommandSet msg;
	msg.console_no = info.nConsoleID;		// 调度台 台号
	SysidToByteArray(info.sys_id, msg.sys_id, sizeof(msg.sys_id));		// 系统标识
	msg.cmd_no = info.nCmdID;			// 命令号（0~9999）
	msg.tcc_no = info.nTccID;			// 受令站（0为相同命令号的所有受令站命令）

	msg.tcc_status = info.nStatus;         // 定义和MsgTccCommand中tcc_status相同
	msg.tcc_time = info.nTime;	    // 受令站命令生效时间

	msg.tcc_checkor = info.nCheckorID; // 受令站校验者
	msg.tcc_sender = info.nConsoleID;	// 受令站发令人（车站、助调、行调）

	long  eid=GetSpecialEntityID(SERVER_TYPE_DATASTORE, m_pInfoProvider->GetConsoleID());
	SendBMsg(&msg, eid);

	CString szLog, sz;
	switch(mode)
	{
	case ITccAdviseSink::MAN_SET_STOCKCOMMAND_STATUS:
		sz = "[TCC]: 人工设置服务器确认命令状态 ";
		break;
	case ITccAdviseSink::LIRC_SET_STOCKCOMMAND_STATUS:
	default:
		sz = "[TCC]: 设置服务器确认命令状态 ";
		break;
	}

	GpcLogTrainMessage(0, info.nTccID, 
		"%s 命令号=%d sysid=%s 站=%s[%d] 状态=%s[%d]",
		sz, info.nCmdID, SysidToString(info.sys_id),
		m_pInfoProvider->GetStationName(info.nTccID), info.nTccID,
		GetStockCommandStatusString(info.nStatus), info.nStatus);
}

BOOL CTccGpcAdviseSink::ConfirmCommandID(int nConsoleID, cmdid_t nCommandID, cmdid_t nMinCommandID, cmdid_t nMaxCommandID, int& who_confirm_cmdid, DWORD dwCookie, HWND hTccDlg)
{
	if(!gpcdef.IsTccCmdIDRequireServerConfirm(3))
	{
		return ITccAdviseSink::ConfirmCommandID(nConsoleID, nCommandID, nMinCommandID, nMaxCommandID, who_confirm_cmdid, dwCookie, hTccDlg);
	}

	who_confirm_cmdid = OTHER_CONFIRM_COMMAND_ID;

	CLBTH::MsgStccCommandIDRequest msg;
	msg.oper_mode = CLBTH::MsgStccCommandIDRequest::GET_CMDID;
	msg.console_no = (WORD)nConsoleID;
	msg.cmd_no = nCommandID;
	msg.min_cmd_no = nMinCommandID;
	msg.max_cmd_no = nMaxCommandID;
	msg.dwCookie = dwCookie;

	long  eid=GetSpecialEntityID(SERVER_TYPE_DATASTORE, m_pInfoProvider->GetConsoleID()); 
	SendBMsg(&msg, eid);

	GpcLogTrainMessage(0,0, "[TCC]: 向服务器申请限速命令号确认(台号=%d, 当前命令号=%d)", nConsoleID, nCommandID);

	return TRUE;
}

}