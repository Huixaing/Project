// TCC.cpp : 实现文件
//

#include "stdafx.h"
#include "tg.h"
#include <Tcc/C3/TCC.h>
#include <Tcc/C2/Tcc.h>
#include <BaseMsg/clbth.h>
#include <sysshare.h>


IMPORT_C_VISITOR(visitorofWintg);
                    				
DEFINE_BASEMSG_PROC(MsgStccCommandList2, pMsg, vpParam, visitorofWintg)
{
	return CLBTH::HRCODE(CLBTH::HRCODE::HC_SUCCESS);

	TCC_C3::TCC_COMMAND cmd;
	cmd.nConsoleID = pMsg->console_no;		// 调度台 台号
	TCC_C3::SysidFromByteArray(cmd.sys_id, pMsg->sys_id, 16);		// 系统标识
	cmd.nCmdID = pMsg->cmd_no;			// 命令号（0~9999）
	cmd.nStaID[0] = pMsg->sta_no[0];		// 开始、结束站号
	cmd.nStaID[1] = pMsg->sta_no[1];
	cmd.nLineID = pMsg->line_no;		// 线路号
	cmd.bFullLimit = pMsg->bFullLimit;		// 是否全线限速
	cmd.nKmMark[0].flag       = pMsg->km_marker[0].nKmsysCode;	// 开始、结束公里标（以m为单位）
	cmd.nKmMark[0].info.km_cl = pMsg->km_marker[0].k_cl;
	cmd.nKmMark[0].info.m_cl  = pMsg->km_marker[0].m_cl;
	cmd.nKmMark[0].info.val   = pMsg->km_marker[0].val;
	cmd.nKmMark[1].flag       = pMsg->km_marker[1].nKmsysCode;
	cmd.nKmMark[1].info.km_cl = pMsg->km_marker[1].k_cl;
	cmd.nKmMark[1].info.m_cl  = pMsg->km_marker[1].m_cl;
	cmd.nKmMark[1].info.val   = pMsg->km_marker[1].val;
	cmd.bStartNow = pMsg->bStartNow;		// 是否立即开始
	cmd.bEndless = pMsg->bEndless;		// 是否永久有效
	cmd.nTime[0] = pMsg->time[0];		// 开始、结束时间
	cmd.nTime[1] = pMsg->time[1];
	cmd.nSpeedMaxVal = pMsg->speed;			// 限速值

	cmd.nTccID = pMsg->tcc_no;			// 受令站（列控）
	cmd.nTccLineID = pMsg->tcc_line_no;	// 受令站线路
	cmd.nTccSenderID = pMsg->tcc_sender;	// 受令站发令人
	cmd.nTccKmMarkFlag[0] = ((pMsg->tcc_km_marker[0].nKmsysCode >> 6) & 0x03);	// 受令站公里标（以m为单位）
	cmd.nTccKmMarkFlag[1] = ((pMsg->tcc_km_marker[1].nKmsysCode >> 6) & 0x03);
	
	//TCC_C3::Receive_TccCommand(cmd, 0);

	return CLBTH::HRCODE(CLBTH::HRCODE::HC_SUCCESS);
}

DEFINE_BASEMSG_PROC(MsgStccReport2, pMsg, vpParam, visitorofWintg)
{

	TCC_C3::TCC_STATUS status;
	status.comm_active = pMsg->comm_active;	// 列控通信状态	(0=断;1=单机;2=双机)
	status.init_status = pMsg->init_status;
	status.nStaID = pMsg->sta_no;	// 站号
	for(int iSta=0; iSta<pMsg->nStaNum && iSta<sizeof(status.sta)/sizeof(status.sta[0]); iSta++)
	{
		status.sta[iSta].nTccID = pMsg->sta[iSta].nStaID;
		status.sta[iSta].standby_status = pMsg->sta[iSta].stby_status;
		status.sta[iSta].master_status = pMsg->sta[iSta].master_status;
		for(int i=0; i<2; i++)
			status.sta[iSta].ilock_channel[i] = pMsg->sta[iSta].ilock_channel[i];	// 联锁通道状态
		for(i=0; i<2; i++)
			status.sta[iSta].ctc_channel[i] = pMsg->sta[iSta].ctc_channel[i];	// CTC通道状态
		for(i=0; i<2; i++)
			status.sta[iSta].switch_channel[i] = pMsg->sta[iSta].switch_channel[i];	// 交换机通道状态

		for(i=0; i<16; i++)
		{
			status.sta[iSta].leu[i].bValid = pMsg->sta[iSta].leu[i].bValid;
			for(int j=0; j<2; j++)
				status.sta[iSta].leu[i].leu_channel[j] =pMsg->sta[iSta].leu[i].leu_channel[j];	// LEU通道状态

			for(j=0; j<4; j++)
				status.sta[iSta].leu[i].leu_ack[j]  = pMsg->sta[iSta].leu[i].leu_ans[j];	// LEU1应答器状态
		}
		status.sta[iSta].nZCNum = pMsg->sta[iSta].nZCNum;
		for(i=0; i<pMsg->sta[iSta].nZCNum && i<8; i++)
		{
			status.sta[iSta].ZC[i].nStaID = pMsg->sta[iSta].ZC[i].nStaID;
			status.sta[iSta].ZC[i].channel[0] = pMsg->sta[iSta].ZC[i].channel[0];
			status.sta[iSta].ZC[i].channel[1] = pMsg->sta[iSta].ZC[i].channel[1];
		}
	}
	status.nStaNum = iSta;

	//TCC_C3::Receive_TccReport(pMsg->sta_no, status);
	return CLBTH::HRCODE(CLBTH::HRCODE::HC_SUCCESS);
}


// LiRC对下命令操作的返回
                 
DEFINE_BASEMSG_PROC(MsgStccCommandSendResult2, pMsg, vpParam, visitorofWintg)
{
	return CLBTH::HRCODE(CLBTH::HRCODE::HC_SUCCESS);
	USHORT nTaskId;
	if(pMsg->bInitOpration)
	{
		TCC_C3::dir_t dir = TCC_C3::DIR_UNKNOWN;
		switch(pMsg->section_dir)
		{
		case 0:
			dir = TCC_C3::DIR_UP;
			break;
		case 1:
			dir = TCC_C3::DIR_DOWN;
			break;
		case 2:
			dir = TCC_C3::DIR_DUAL;
			break;
		default:
			break;
		}

		char buf[1024];
		memset(buf, 0x0, sizeof(buf));
		pMsg->GetComment(buf, 1024);
		//AfxMessageBox(buf);

		//TCC_C3::Receive_TccLineInitialCommandResult(pMsg->tcc_no, pMsg->tcc_line_no, dir, pMsg->result, buf);
	}
	else
	{
		TCC_C3::TCC_COMMAND_RESULT cmd_result;
		cmd_result.cmd.nConsoleID = pMsg->console_no;
		TCC_C3::SysidFromByteArray(cmd_result.cmd.sys_id, pMsg->sys_id, sizeof(pMsg->sys_id));
		cmd_result.cmd.nCmdID = pMsg->cmd_no;		// 命令号(0~9999)
		cmd_result.cmd.nStaID[0] = pMsg->sta_no[0];		// 开始站
		cmd_result.cmd.nStaID[1] = pMsg->sta_no[1];		// 结束站
		cmd_result.cmd.nLineID = pMsg->line_no;		// 线路号
		cmd_result.cmd.bFullLimit = pMsg->bFullLimit;	// 是否全线限速
		cmd_result.cmd.nKmMark[0].flag       = pMsg->km_marker[0].nKmsysCode;	// 开始、结束公里标（以m为单位）
		cmd_result.cmd.nKmMark[0].info.km_cl = pMsg->km_marker[0].k_cl;
		cmd_result.cmd.nKmMark[0].info.m_cl  = pMsg->km_marker[0].m_cl;
		cmd_result.cmd.nKmMark[0].info.val   = pMsg->km_marker[0].val;
		cmd_result.cmd.nKmMark[1].flag       = pMsg->km_marker[1].nKmsysCode;
		cmd_result.cmd.nKmMark[1].info.km_cl = pMsg->km_marker[1].k_cl;
		cmd_result.cmd.nKmMark[1].info.m_cl  = pMsg->km_marker[1].m_cl;
		cmd_result.cmd.nKmMark[1].info.val   = pMsg->km_marker[1].val;
		cmd_result.cmd.bStartNow = pMsg->bStartNow;		// 是否立即开始
		cmd_result.cmd.bEndless = pMsg->bEndless;		// 是否永久有效
		cmd_result.cmd.nTime[0] = pMsg->time[0];			// 开始时间(秒)
		cmd_result.cmd.nTime[1] = pMsg->time[1];			// 结束时间(秒)
		cmd_result.cmd.nSpeedMaxVal = pMsg->speed;		// 限速值(5KM/H的倍数)

		cmd_result.cmd.nTccID = pMsg->tcc_no;
		cmd_result.cmd.nTccLineID = pMsg->tcc_line_no;
		cmd_result.cmd.nTccSenderID = pMsg->tcc_sender;
		cmd_result.cmd.bCancelLimit = pMsg->bCancelLimit;
		cmd_result.cmd.nTccKmMarkFlag[0] = ((pMsg->tcc_km_marker[0].nKmsysCode >> 6) & 0x03);
		cmd_result.cmd.nTccKmMarkFlag[1] = ((pMsg->tcc_km_marker[1].nKmsysCode >> 6) & 0x03);

		cmd_result.result = pMsg->result;

		char buf[1024];
		memset(buf, 0x0, sizeof(buf));
		pMsg->GetComment(buf, sizeof(buf));
		cmd_result.strComment = CString(buf, sizeof(buf));

		bool b = false;
		if(HWND hWnd = TCC_C3::GetTccCommandSendDlgHandle())
		{
			if(::IsWindow(hWnd))
			{
				//b = TCC_C3::Receive_TccCommandResult(cmd_result, hWnd);
			}
		}
		if(!b)
		{
			CString str, szLog;
			szLog.Format("[TCC]: 限速命令号%d, 站号%d的列控操作反馈结果：%d(%s)。发令窗口过早关闭，服务器状态可能不正确!", 
				cmd_result.cmd.nCmdID, cmd_result.cmd.nTccID,
				cmd_result.result, cmd_result.strComment);
			GpcLogDebugMessage(0, szLog);

			//执行成功后,修改服务器相应命令的状态, 用老消息，因为CheckorID无值
			if(4 == cmd_result.result)
			{
				CLBTH::MsgTccCommandStatusChanged msg;

				msg.console_no = cmd_result.cmd.nConsoleID;
				TCC_C3::SysidToByteArray(cmd_result.cmd.sys_id, msg.sys_id, sizeof(msg.sys_id));		// 系统标识
				msg.cmd_no = cmd_result.cmd.nCmdID;
				msg.tcc_no = cmd_result.cmd.nTccID;
				
				msg.b_status_changed = TRUE;
				msg.tcc_status = TCC_C3::EXECUTING;

				msg.b_sender_set = TRUE;
				msg.tcc_sender = cmd_result.cmd.nTccSenderID;	// 受令站发令人

				msg.b_time_set = TRUE;
				time_t tm;
				time(&tm);
				msg.tcc_time = tm;

				SendBMsg(&msg, 0);

				TCC_C3::ITccClientInfoProvider *pInfoProvider = NULL;

				pInfoProvider = ((CWintg *)AfxGetApp())->GetMainFrame()->m_pTcc3InfoProvider;
				
				CString szStaName;
				if(pInfoProvider)
					szStaName = pInfoProvider->GetStationName(msg.tcc_no);

				GpcLogTrainMessage(0, msg.tcc_no, 
					"[TCC]: 设置服务器确认命令状态 命令号=%d sysid=%s 站=%s[%d] 状态:已执行[%d]",
					msg.cmd_no, TCC_C3::SysidToString(cmd_result.cmd.sys_id),
					szStaName, msg.tcc_no,
					msg.tcc_status);
			}
		}
	}

	return CLBTH::HRCODE(CLBTH::HRCODE::HC_SUCCESS);
}


// 服务器下载确认限速命令列表
// 命令结构: STOCK_COMMAND
// 命令状态与发令人等要设置到原始命令内容中
                    
DEFINE_BASEMSG_PROC(MsgTccCommand2, pMsg, vpParam, visitorofWintg)
{
	// 从消息中提取确认命令
	TCC_C3::STOCK_COMMAND cmd;
	cmd.dcmd_id = (char*)pMsg->dcmd_no;
	cmd.dcmd_send_entity = pMsg->dcmd_entity;	//调度命令索引
	cmd.dcmd_create_time = pMsg->create_time;

	cmd.nConsoleID = pMsg->console_no;
	TCC_C3::SysidFromByteArray(cmd.sys_id, pMsg->sys_id, 16);
	TCC_C3::SysidFromByteArray(cmd.raw_sysid, pMsg->raw_sysid, 16);
	cmd.nCmdID = pMsg->cmd_no;
	cmd.nStaID[0] = pMsg->sta_no[0];		// 开始、结束站号
	cmd.nStaID[1] = pMsg->sta_no[1];
	cmd.nLineID = pMsg->line_no;		// 线路号
	cmd.bFullLimit = pMsg->bFullLimit;		// 是否全线限速
	cmd.nKmMark[0].flag       = pMsg->km_marker[0].nKmsysCode;	// 开始、结束公里标（以m为单位）
	cmd.nKmMark[0].info.km_cl = pMsg->km_marker[0].k_cl;
	cmd.nKmMark[0].info.m_cl  = pMsg->km_marker[0].m_cl;
	cmd.nKmMark[0].info.val   = pMsg->km_marker[0].val;
	cmd.nKmMark[1].flag       = pMsg->km_marker[1].nKmsysCode;
	cmd.nKmMark[1].info.km_cl = pMsg->km_marker[1].k_cl;
	cmd.nKmMark[1].info.m_cl  = pMsg->km_marker[1].m_cl;
	cmd.nKmMark[1].info.val   = pMsg->km_marker[1].val;
	cmd.bStartNow  = pMsg->bStartNow;		// 是否立即开始
	cmd.bEndless   = pMsg->bEndless;		// 是否永久有效
	cmd.nTime[0] = pMsg->time[0];		// 开始、结束时间
	cmd.nTime[1] = pMsg->time[1];
	cmd.nSpeedMaxVal = pMsg->speed;			// 限速值
	cmd.bCancelLimit = pMsg->bCancelLimit;		// 是否 取消限速命令

	cmd.nCreatorID = pMsg->creator;		// 原始命令草拟人
	cmd.nCheckorID = pMsg->checkor;		// 中心校验者
	cmd.nConfirmorID = pMsg->confirmor;	// 确认分解者
	cmd.nCreateTime = pMsg->create_time;

	cmd.tcc.resize(pMsg->tcc_num);		// 受令站个数
	for(int i=0; i<cmd.tcc.size(); i++)
	{
		cmd.tcc[i].nID = pMsg->tcc_no[i];		// 受令站（列控）
		cmd.tcc[i].nTccLineID = pMsg->tcc_line_no[i]; // 受令站线路
		cmd.tcc[i].nCheckorID = pMsg->tcc_checkor[i];
		cmd.tcc[i].nTime = pMsg->tcc_time[i];
		cmd.tcc[i].nStatus = pMsg->tcc_status[i];	// 受令站命令状态
		cmd.tcc[i].nSenderID = pMsg->tcc_sender[i];	// 受令站发令人（车站、助调、行调）
		cmd.tcc[i].nKmMarkFlag[0] = ((pMsg->tcc_km_marker[i][0].nKmsysCode >> 6) & 0x03);
		cmd.tcc[i].nKmMarkFlag[1] = ((pMsg->tcc_km_marker[i][1].nKmsysCode >> 6) & 0x03);
	}

	// 确认命令UI显示
	//TCC_C3::Receive_StockCommand(cmd);

	return CLBTH::HRCODE(CLBTH::HRCODE::HC_SUCCESS);
}


// 从服务器下载确认命令  
// 确认命令服务器发送的协议是C3格式的，但是C2移植到C3服务器上时服务器使用C3协议下发，所以在C2时必须转化成所需要的格式
DEFINE_BASEMSG_PROC(MsgTccCommandData2, pMsg, vpParam, visitorofWintg)
{
	extern BOOL IsTCC_C3();
	if (IsTCC_C3())
	{
		// 从消息中提取确认命令
		TCC_C3::STOCK_COMMAND cmd;
		cmd.dcmd_id = (char*)pMsg->dcmd_no;
		cmd.dcmd_send_entity = pMsg->dcmd_entity;	//调度命令索引
		cmd.dcmd_create_time = pMsg->create_time;

		cmd.nConsoleID = pMsg->console_no;
		TCC_C3::SysidFromByteArray(cmd.sys_id, pMsg->sys_id, 16);
		TCC_C3::SysidFromByteArray(cmd.raw_sysid, pMsg->raw_sysid, 16);
		cmd.nCmdID = pMsg->cmd_no;
		cmd.nStaID[0] = pMsg->sta_no[0];		// 开始、结束站号
		cmd.nStaID[1] = pMsg->sta_no[1];
		cmd.nLineID = pMsg->line_no;		// 线路号
		cmd.bFullLimit = pMsg->bFullLimit;		// 是否全线限速
		cmd.nKmMark[0].flag       = pMsg->km_marker[0].nKmsysCode;	// 开始、结束公里标（以m为单位）
		cmd.nKmMark[0].info.km_cl = pMsg->km_marker[0].k_cl;
		cmd.nKmMark[0].info.m_cl  = pMsg->km_marker[0].m_cl;
		cmd.nKmMark[0].info.val   = pMsg->km_marker[0].val;
		cmd.nKmMark[1].flag       = pMsg->km_marker[1].nKmsysCode;
		cmd.nKmMark[1].info.km_cl = pMsg->km_marker[1].k_cl;
		cmd.nKmMark[1].info.m_cl  = pMsg->km_marker[1].m_cl;
		cmd.nKmMark[1].info.val   = pMsg->km_marker[1].val;
		cmd.bStartNow  = pMsg->bStartNow;		// 是否立即开始
		cmd.bEndless   = pMsg->bEndless;		// 是否永久有效
		cmd.nTime[0] = pMsg->time[0];		// 开始、结束时间
		cmd.nTime[1] = pMsg->time[1];
		cmd.nSpeedMaxVal = pMsg->speed;			// 限速值
		cmd.bCancelLimit = pMsg->bCancelLimit;		// 是否 取消限速命令

		cmd.nCreatorID = pMsg->creator;		// 原始命令草拟人
		cmd.nCheckorID = pMsg->checkor;		// 中心校验者
		cmd.nConfirmorID = pMsg->confirmor;	// 确认分解者
		cmd.nCreateTime = pMsg->create_time;

		cmd.tcc.resize(pMsg->tcc_num);		// 受令站个数
		for(int i=0; i<cmd.tcc.size(); i++)
		{
			cmd.tcc[i].nID = pMsg->tcc_no[i];		// 受令站（列控）
			cmd.tcc[i].nTccLineID = pMsg->tcc_line_no[i]; // 受令站线路
			cmd.tcc[i].nCheckorID = pMsg->tcc_checkor[i];
			cmd.tcc[i].nTime = pMsg->tcc_time[i];
			cmd.tcc[i].nStatus = pMsg->tcc_status[i];	// 受令站命令状态
			cmd.tcc[i].nSenderID = pMsg->tcc_sender[i];	// 受令站发令人（车站、助调、行调）
			cmd.tcc[i].nKmMarkFlag[0] = ((pMsg->tcc_km_marker[i][0].nKmsysCode >> 6) & 0x03);
			cmd.tcc[i].nKmMarkFlag[1] = ((pMsg->tcc_km_marker[i][1].nKmsysCode >> 6) & 0x03);
		}

		// 确认命令UI显示
		switch(pMsg->type)
		{
		case CLBTH::MsgTccCommandData2::QUERY_CURRENT:
			//TCC_C3::Receive_StockCommand(cmd);
			break;
		case CLBTH::MsgTccCommandData2::QUERY_HISTORY:
			//TCC_C3::Receive_HistoryCommand(cmd);
			break;
		default:
			break;
		}
	}
	else
	{
		//发给C2的界面接收
		TCC::STOCK_COMMAND cmd;
		cmd.dcmd_id = (char*)pMsg->dcmd_no;
		cmd.dcmd_send_entity = pMsg->dcmd_entity;	//调度命令索引
		cmd.dcmd_create_time = pMsg->create_time;

		cmd.nConsoleID = pMsg->console_no;
		TCC::SysidFromByteArray(cmd.sys_id, pMsg->sys_id, 16);
		TCC::SysidFromByteArray(cmd.raw_sysid, pMsg->raw_sysid, 16);
		cmd.nCmdID = pMsg->cmd_no;
		cmd.nStaID[0] = pMsg->sta_no[0];		// 开始、结束站号
		cmd.nStaID[1] = pMsg->sta_no[1];
		cmd.nLineID = pMsg->line_no;		// 线路号
		cmd.bFullLimit = pMsg->bFullLimit;		// 是否全线限速
		cmd.nKmMark[0] = (pMsg->km_marker[0].val & 0x0FFFFFF) + ((pMsg->km_marker[0].k_cl & 0x3F) << 24) + ((pMsg->km_marker[0].m_cl & 0x01) << 30);	// 开始、结束公里标（以m为单位）
		cmd.nKmMark[1] = (pMsg->km_marker[1].val & 0x0FFFFFF) + ((pMsg->km_marker[1].k_cl & 0x3F) << 24) + ((pMsg->km_marker[1].m_cl & 0x01) << 30);
		cmd.bStartNow  = pMsg->bStartNow;		// 是否立即开始
		cmd.bEndless   = pMsg->bEndless;		// 是否永久有效
		cmd.nTime[0] = pMsg->time[0];		// 开始、结束时间
		cmd.nTime[1] = pMsg->time[1];
		cmd.nSpeedMaxVal = pMsg->speed;			// 限速值
		cmd.bCancelLimit = pMsg->bCancelLimit;		// 是否 取消限速命令

		cmd.nCreatorID = pMsg->creator;		// 原始命令草拟人
		cmd.nCheckorID = pMsg->checkor;		// 中心校验者
		cmd.nConfirmorID = pMsg->confirmor;	// 确认分解者
		cmd.nCreateTime = pMsg->create_time;

		cmd.tcc.resize(pMsg->tcc_num);		// 受令站个数
		for(int i=0; i<cmd.tcc.size(); i++)
		{
			cmd.tcc[i].nID = pMsg->tcc_no[i];		// 受令站（列控）
			cmd.tcc[i].nTccLineID = pMsg->tcc_line_no[i]; // 受令站线路
			cmd.tcc[i].nCheckorID = pMsg->tcc_checkor[i];
			cmd.tcc[i].nTime = pMsg->tcc_time[i];
			cmd.tcc[i].nStatus = pMsg->tcc_status[i];	// 受令站命令状态
			cmd.tcc[i].nSenderID = pMsg->tcc_sender[i];	// 受令站发令人（车站、助调、行调）
		}

		// 确认命令UI显示
		switch(pMsg->type)
		{
		case CLBTH::MsgTccCommandData2::QUERY_CURRENT:
		//	TCC::Receive_StockCommand(cmd);
			break;
		case CLBTH::MsgTccCommandData2::QUERY_HISTORY:
		//	TCC::Receive_HistoryCommand(cmd);
			break;
		default:
			break;
		}
	}

	return CLBTH::HRCODE(CLBTH::HRCODE::HC_SUCCESS);
}


/*DEFINE_BASEMSG_PROC(MsgStccCommandIDRequest, pMsg, vpParam, visitorofWintg)
{
	if(CLBTH::MsgStccCommandIDRequest::SET_CMDID != pMsg->oper_mode)
		return CLBTH::HRCODE(CLBTH::HRCODE::HC_FAIL);

	bool bValid;
	switch(pMsg->nStatus)
	{
	case CLBTH::MsgStccCommandIDRequest::TCC_CMDID_REQUEST_SUCCESS:
		bValid = true;
		break;
	case CLBTH::MsgStccCommandIDRequest::TCC_CMDID_REQUEST_FAIL:
		bValid = false;
		break;
	default:
		return CLBTH::HRCODE(CLBTH::HRCODE::HC_FAIL);
		break;
	}

	extern BOOL IsTCC_C3();
	if (IsTCC_C3())
		TCC_C3::ReceiveCommandIDConfirmation(bValid, pMsg->console_no, pMsg->cmd_no, pMsg->dwCookie);
	else
		TCC::ReceiveCommandIDConfirmation(bValid, pMsg->console_no, pMsg->cmd_no, pMsg->dwCookie);

	return CLBTH::HRCODE(CLBTH::HRCODE::HC_SUCCESS);
}
*/