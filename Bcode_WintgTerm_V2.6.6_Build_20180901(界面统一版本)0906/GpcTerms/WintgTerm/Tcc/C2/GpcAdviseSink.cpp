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
	
namespace TCC
{


INT CTccGpcAdviseSink::OnSendInitCommand(stano_t nTccID, lineid_t nTccLineID, dir_t dir,HWND hTccDlg)
{
	CLBTH::MsgStccCommandSend msg;

	//msg.console_no = cmd.nConsoleID;
	//TCC::SysidToByteArray(cmd.sys_id, msg.sys_id, sizeof(msg.sys_id));
	//msg.cmd_no = cmd.nCmdID;		// 命令号(0~9999)
	//msg.sta_no[0] = cmd.nStaID[0];		// 开始站
	//msg.sta_no[1] = cmd.nStaID[1];		// 结束站
	//msg.line_no = cmd.nLineID;		// 线路号
	//msg.bFullLimit = cmd.bFullLimit;	// 是否全线限速
	//msg.km_marker[0] = cmd.nKmMark[0];	// 开始公理标
	//msg.km_marker[1] = cmd.nKmMark[1];	// 结束公理标
	//msg.bStartNow = cmd.bStartNow;		// 是否立即开始
	//msg.bEndless = cmd.bEndless;		// 是否永久有效
	//msg.time[0] = cmd.nTime[0];			// 开始时间(秒)
	//msg.time[1] = cmd.nTime[1];			// 结束时间(秒)
	//msg.speed = cmd.nSpeedMaxVal;		// 限速值(5KM/H的倍数)

	msg.tcc_no = nTccID;
	msg.tcc_line_no = nTccLineID;
	msg.tcc_sender = 0;
	//msg.bCancelLimit = cmd.bCancelLimit;

	msg.bInitOpration = TRUE;
	switch(dir)
	{
	case DIR_DOWN:
		msg.section_dir = 1;
		break;
	case DIR_UP:
		msg.section_dir = 0;
		break;
	case DIR_DUAL:
		msg.section_dir = 2;
		break;
	default:
		return -1;
		break;
	}

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

		SendBMsg(&msg,entity.nEntityId);


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
INT CTccGpcAdviseSink::OnSendTccCommand(TCC_COMMAND& cmd, HWND hTccDlg)
{
	ASSERT(cmd.nTccID != 0);

	CLBTH::MsgStccCommandSend msg;
	msg.bInitOpration = FALSE;

	msg.console_no = cmd.nConsoleID;
	SysidToByteArray(cmd.sys_id, msg.sys_id, sizeof(msg.sys_id));
	msg.cmd_no = cmd.nCmdID;		// 命令号(0~9999)
	msg.sta_no[0] = cmd.nStaID[0];		// 开始站
	msg.sta_no[1] = cmd.nStaID[1];		// 结束站
	msg.line_no = cmd.nLineID;		// 线路号
	msg.bFullLimit = cmd.bFullLimit;	// 是否全线限速
	msg.km_marker[0] = cmd.nKmMark[0].GetDWORD();	// 开始公理标
	msg.km_marker[1] = cmd.nKmMark[1].GetDWORD();	// 结束公理标
	msg.bStartNow = cmd.bStartNow;		// 是否立即开始
	msg.bEndless = cmd.bEndless;		// 是否永久有效
	msg.time[0] = cmd.nTime[0];			// 开始时间(秒)
	msg.time[1] = cmd.nTime[1];			// 结束时间(秒)
	msg.speed = cmd.nSpeedMaxVal;		// 限速值

	msg.tcc_no = cmd.nTccID;
	msg.tcc_line_no = cmd.nTccLineID;
	msg.tcc_sender = 0;
	msg.bCancelLimit = cmd.bCancelLimit;

	GPCENTITY entity;
	if(gpcdef.GetEntityInfo(gpcdef.GetCurUserName(),entity))
	{
		msg.tcc_sender = entity.nEntityId;
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

		SendBMsg(&msg, entity.nEntityId);

		CString szLog, sz;
		szLog.Format("[TCC]: send TCC_COMMAND to 列控(%s[%d]) 命令号(%d) 线路(%s[%d]) 公里标%s--%s ",
			m_pInfoProvider->GetStationName(cmd.nTccID), cmd.nTccID,
			cmd.nCmdID, 
			m_pInfoProvider->GetTccLineName(cmd.nTccID, cmd.nTccLineID), cmd.nTccLineID,
			cmd.nKmMark[0].toString(), cmd.nKmMark[1].toString()
			);

		if(cmd.bCancelLimit)
			sz.Format("取消限速");
		else
			sz.Format("限速值(%d)", cmd.nSpeedMaxVal);
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

	return 0;
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

		SendBMsg(&msg, entity.nEntityId);
		

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



void OnCancelStockCommand(WPARAM wParam, LPARAM lParam)
{
	
	
	return;
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

		//if(m_pWintgForm)
		//	m_pWintgForm->SendMessage(UM_TCC_LIMITSPEED_CMD_CANCEL, (WPARAM)&cmd, (LPARAM)pStockCmd);
		// 给调度命令终端发送取消消息
		
		TCC::RAW_STOCK_COMMAND c2rawcmd;

		if(pStockCmd != NULL)
		{
			pStockCmd->ComposeRawStcokCommand(c2rawcmd);
		}
		else
		{
			cmd.ComposeRawStcokCommand(c2rawcmd);
		}
		
		// 特殊处理--自律机重起后区间全限速公里标返回为9999999,但是库中为0000000
		if(c2rawcmd.bFullLimit)
		{
			c2rawcmd.nKmMark[0].val = 0;
			c2rawcmd.nKmMark[1].val = 0;
		}

		SendCancelDCmdC2(c2rawcmd, cmd.nConsoleID);			
		
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
	CLBTH::MsgTccCommandAdd msg;
	msg.console_no = cmd.nConsoleID;	// 调度台 台号
	SysidToByteArray(cmd.sys_id, msg.sys_id, 16);		// 系统标识
	SysidToByteArray(cmd.raw_sysid, msg.raw_sysid, 16);// 原始命令系统标识

	msg.cmd_no = cmd.nCmdID;
	msg.sta_no[0] = cmd.nStaID[0];		// 开始、结束站号
	msg.sta_no[1] = cmd.nStaID[1];

	msg.line_no   = cmd.nLineID;		// 线路号
	msg.bFullLimit= cmd.bFullLimit;		// 是否全线限速
	msg.km_marker[0] = cmd.nKmMark[0].GetDWORD();	// 开始、结束公里标（以m为单位）
	msg.km_marker[1] = cmd.nKmMark[1].GetDWORD();
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
		msg.tcc_no[i] = cmd.tcc[i].nID;		// 受令站（列控）
		msg.tcc_line_no[i] = cmd.tcc[i].nTccLineID; // 受令站线路
		msg.tcc_status[i] = cmd.tcc[i].nStatus;	// 受令站命令状态
		msg.tcc_sender[i] = cmd.tcc[i].nSenderID;	// 受令站发令人（车站、助调、行调）
	}


	long  eid=GetSpecialEntityID(SERVER_TYPE_DATASTORE, m_pInfoProvider->GetConsoleID());
	SendBMsg(&msg,eid);
    GpcLogTrainMessage(0,0, "[TCC]: 向服务器 %d 发送确认命令 命令号=%d sysid=%s",
		eid, cmd.nCmdID,SysidToString(cmd.sys_id));
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
	

	GpcLogTrainMessage(0,0, "[TCC]: 删除服务器 %d 确认命令 命令号=%d 站号=%s[%d] sysid=%s",
		eid, cmd.nCmdID, m_pInfoProvider->GetStationName(nTccID), nTccID, SysidToString(cmd.sys_id));

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
	SendBMsg(&msg,eid);
	
	CString szLog, sz;
	szLog.Format("[TCC]: 申请服务器确认命令列表 站=%s[%d]", 
		m_pInfoProvider->GetStationName(nTccID), nTccID);

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
#if 0
	if(m_pWintgForm)
		((CWintgForm *)m_pWintgForm)->SetFigureStatusColor(dwParam, status);
#endif
	return 0;
}

BOOL CTccGpcAdviseSink::ConfirmCommandID(int nConsoleID, cmdid_t nCommandID, cmdid_t nMinCommandID, cmdid_t nMaxCommandID, int& who_confirm_cmdid, DWORD dwCookie, HWND hTccDlg)
{
	if(!gpcdef.IsTccCmdIDRequireServerConfirm(2))
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
	SendBMsg(&msg,eid);
	
	GpcLogTrainMessage(0,0, "[TCC]: 向服务器申请限速命令号确认(台号=%d, 当前命令号=%d)", nConsoleID, nCommandID);

	return TRUE;
}

}
