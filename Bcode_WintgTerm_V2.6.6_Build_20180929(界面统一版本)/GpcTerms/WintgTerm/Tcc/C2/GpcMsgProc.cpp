// TCC.cpp : 实现文件
//

#include "stdafx.h"
#include "TCC.h"
#include <BaseMsg/clbth.h>
#include <sysshare.h>

IMPORT_C_VISITOR(visitorofWintg);
DEFINE_BASEMSG_PROC(MsgStccCmdStatus, pMsg, vpParam, visitorofWintg)
{
	//DWORD status = TCC::TCC_UNKNOWN;// 未知
	//switch(pMsg->status_type)
	//{
	//case CLBTH::MsgStccCmdStatus::LIRC_RECEIVED:
	//	status = TCC::LIRC_RECEIVED;		// 自律机收到
	//	break;
	//case CLBTH::MsgStccCmdStatus::LIRC_SEND_FAIL:
	//	status = TCC::LIRC_SEND_FAIL;	// 自律机发送失败(协议帧错误)
	//	break;
	//case CLBTH::MsgStccCmdStatus::STCC_RECEIVED:
	//	status = TCC::TCC_RECEIVED;		// 列控收到(协议帧正常)
	//	break;
	//case CLBTH::MsgStccCmdStatus::STCC_ACCEPT:
	//	status = TCC::TCC_ACCEPT;	// 无错误
	//	break;

	//case CLBTH::MsgStccCmdStatus::STCC_WAITFOR_EXEC:
	//	status = TCC::TCC_WAITFOR_EXEC;	// 排队，等待执行
	//	break;
	//case CLBTH::MsgStccCmdStatus::STCC_EXECUTING:
	//	status = TCC::TCC_EXECUTING;		// 正在执行
	//	break;
	//case CLBTH::MsgStccCmdStatus::STCC_ZOMBIE:
	//	status = TCC::TCC_ZOMBIE;		// 执行结束(结束时间到)
	//	break;
	//case CLBTH::MsgStccCmdStatus::STCC_CANCELED:
	//	status = TCC::TCC_CANCELED;		// 已取消(人工, 正常消亡)
	//	break;

	//case CLBTH::MsgStccCmdStatus::ERR_START_KM:
	//case CLBTH::MsgStccCmdStatus::ERR_END_KM:
	//case CLBTH::MsgStccCmdStatus::ERR_ALL_KM:
	//	status = TCC::TCC_ERR_KM;	// 公里标非法
	//	break;
	//case CLBTH::MsgStccCmdStatus::ERR_SPEED:
	//	status = TCC::TCC_ERR_SPEED;	// 限速非法
	//	break;
	//case CLBTH::MsgStccCmdStatus::ERR_REGION_LONG:
	//	status = TCC::TCC_ERR_REGION_LONG;	// 区域太长
	//	break;
	//case CLBTH::MsgStccCmdStatus::ERR_REGION_EXIST:
	//	status = TCC::TCC_ERR_REGION_EXIST;	// 区域已存在
	//	break;
	//case CLBTH::MsgStccCmdStatus::ERR_REGION_INVALID:
	//	status = TCC::TCC_ERR_REGION_INVALID;// 区域非法
	//	break;
	//case CLBTH::MsgStccCmdStatus::ERR_DUP_CMDNO:
	//	status = TCC::TCC_ERR_DUP_CMDNO;		// 命令号重复
	//	break;
	//case CLBTH::MsgStccCmdStatus::ERR_TIME_FAIL:
	//	status = TCC::TCC_ERR_TIME_FAIL;		// 时间校验失败
	//	break;

	//case CLBTH::MsgStccCmdStatus::STCC_CONFIRM_OK: // 被确认
	//	status = TCC::TCC_CONFIRM_OK;
	//	break;
	//case CLBTH::MsgStccCmdStatus::STCC_CONFIRM_CANCEL:	// 被取消
	//	status = TCC::TCC_CONFIRM_CANCEL;
	//	break;
	//case CLBTH::MsgStccCmdStatus::STCC_CONFIRM_TIMEOUT:	// 超时，未确认
	//	status = TCC::TCC_CONFIRM_TIMEOUT;
	//	break;

	//case CLBTH::MsgStccCmdStatus::ERR_UNKNOWN:// 未知错误
	//	break;
	//};

	//switch(pMsg->action_type)
	//{
	//case CLBTH::MsgStccCmdStatus::COMMAND_REPLY:
	//	//TCC::Tcc_SetCmdStatus(pMsg->sys_id, pMsg->cmd_no, pMsg->sta_no, status);
	//	break;

	//case CLBTH::MsgStccCmdStatus::STCC_REPORT:{
	//	//TCC::TCC_COMMAND cmd;
	//	//cmd.nSysID = pMsg->sys_id;
	//	//cmd.nCmdID = pMsg->cmd_no;
	//	//cmd.nTccLineID = pMsg->line_no;

	//	//cmd.nKmMark[0] = pMsg->km_marker[0];
	//	//cmd.nKmMark[1] = pMsg->km_marker[1];
	//	//cmd.nSpeedMaxVal = pMsg->speed;
	//	//cmd.nTime[0] = pMsg->time[0];
	//	//cmd.nTime[1] = pMsg->time[1];

	//	//TCC::Tcc_SetHistoryCmd(cmd.nCmdID, pMsg->sta_no, cmd, status);
	//	}break;
	//}

	return CLBTH::HRCODE(CLBTH::HRCODE::HC_SUCCESS);
}

DEFINE_BASEMSG_PROC(MsgStccCommandList, pMsg, vpParam, visitorofWintg)
{
	TCC::TCC_COMMAND cmd;
	cmd.nConsoleID = pMsg->console_no;		// 调度台 台号
	TCC::SysidFromByteArray(cmd.sys_id, pMsg->sys_id, 16);		// 系统标识
	cmd.nCmdID = pMsg->cmd_no;			// 命令号（0~9999）
	cmd.nStaID[0] = pMsg->sta_no[0];		// 开始、结束站号
	cmd.nStaID[1] = pMsg->sta_no[1];
	cmd.nLineID = pMsg->line_no;		// 线路号
	cmd.bFullLimit = pMsg->bFullLimit;		// 是否全线限速
	cmd.nKmMark[0] = pMsg->km_marker[0];	// 开始、结束公里标（以m为单位）
	cmd.nKmMark[1] = pMsg->km_marker[1];
	cmd.bStartNow = pMsg->bStartNow;		// 是否立即开始
	cmd.bEndless = pMsg->bEndless;		// 是否永久有效
	cmd.nTime[0] = pMsg->time[0];		// 开始、结束时间
	cmd.nTime[1] = pMsg->time[1];
	cmd.nSpeedMaxVal = pMsg->speed;			// 限速值

	cmd.nTccID = pMsg->tcc_no;			// 受令站（列控）
	cmd.nTccLineID = pMsg->tcc_line_no;	// 受令站线路
	cmd.nTccSenderID = pMsg->tcc_sender;	// 受令站发令人

	//TCC::Receive_TccCommand(cmd, 0);   qiaoyt del
    TRACE("\n MsgStccCommandList");
	return CLBTH::HRCODE(CLBTH::HRCODE::HC_SUCCESS);
}

DEFINE_BASEMSG_PROC(MsgStccReport, pMsg, vpParam, visitorofWintg)
{
	TCC::TCC_STATUS status;
	pMsg->sta_no;	// 站号
	status.comm_active = pMsg->comm_active;	// 列控通信状态	(0=断;1=单机;2=双机)
	for(int i=0; i<2; i++)
		status.stcc_master[i] = pMsg->stcc_master[i];	// 列控主备状态
	for(i=0; i<2; i++)
		status.ilock_channel[i] = pMsg->ilock_channel[i];	// 联锁通道状态
	for(i=0; i<2; i++)
		status.ctc_channel[i] = pMsg->ctc_channel[i];	// CTC通道状态
	for(i=0; i<4; i++)
		status.leu_channel[i]= pMsg->leu_channel[i];	// LEU通道状态


	for(i=0; i<4; i++)
		status.leu_ack[i]  = pMsg->leu1_ans[i];	// LEU1应答器状态
	for(i=0; i<4; i++)
		status.leu_ack[4+i]= pMsg->leu2_ans[i];	// LEU2应答器状态
	for(i=0; i<4; i++)
		status.leu_ack[8+i]= pMsg->leu3_ans[i];	// LEU3应答器状态
	for(i=0; i<4; i++)
		status.leu_ack[12+i]= pMsg->leu4_ans[i];	// LEU4应答器状态

	status.init_section_num = pMsg->init_section_num;
	for(i=0; i<status.init_section_num; i++)
	{
		status.init_line_no[i] = pMsg->init_line_no[i];

		status.init_direction[i] = TCC::DIR_UNKNOWN;
		switch(pMsg->init_direction[i])
		{
		case 0:
			status.init_direction[i] = TCC::DIR_UP;
			break;
		case 1:
			status.init_direction[i] = TCC::DIR_DOWN;
			break;
		default:
			break;
		}
	}
	TRACE("\n MsgStccReport");
//	TCC::Receive_TccReport(pMsg->sta_no, status);  qiaoyt del
	
	return CLBTH::HRCODE(CLBTH::HRCODE::HC_SUCCESS);
}


// LiRC对下命令操作的返回
DEFINE_BASEMSG_PROC(MsgStccCommandSendResult, pMsg, vpParam, visitorofWintg)
{
	if(pMsg->bInitOpration)
	{
		TCC::dir_t dir = TCC::DIR_UNKNOWN;
		switch(pMsg->section_dir)
		{
		case 0:
			dir = TCC::DIR_UP;
			break;
		case 1:
			dir = TCC::DIR_DOWN;
			break;
		case 2:
			dir = TCC::DIR_DUAL;
			break;
		default:
			break;
		}

		char buf[1024];
		memset(buf, 0x0, sizeof(buf));
		pMsg->GetComment(buf, 1024);
		//AfxMessageBox(buf);

		//TCC::Receive_TccLineInitialCommandResult(pMsg->tcc_no, pMsg->tcc_line_no, dir, pMsg->result, buf);
	}
	else
	{
		TCC::TCC_COMMAND cmd;
		cmd.nConsoleID = pMsg->console_no;
		TCC::SysidFromByteArray(cmd.sys_id, pMsg->sys_id, sizeof(pMsg->sys_id));
		cmd.nCmdID = pMsg->cmd_no;		// 命令号(0~9999)
		cmd.nStaID[0] = pMsg->sta_no[0];		// 开始站
		cmd.nStaID[1] = pMsg->sta_no[1];		// 结束站
		cmd.nLineID = pMsg->line_no;		// 线路号
		cmd.bFullLimit = pMsg->bFullLimit;	// 是否全线限速
		cmd.nKmMark[0] = pMsg->km_marker[0];	// 开始公理标
		cmd.nKmMark[1] = pMsg->km_marker[1];	// 结束公理标
		cmd.bStartNow = pMsg->bStartNow;		// 是否立即开始
		cmd.bEndless = pMsg->bEndless;		// 是否永久有效
		cmd.nTime[0] = pMsg->time[0];			// 开始时间(秒)
		cmd.nTime[1] = pMsg->time[1];			// 结束时间(秒)
		cmd.nSpeedMaxVal = pMsg->speed;		

		cmd.nTccID = pMsg->tcc_no;
		cmd.nTccLineID = pMsg->tcc_line_no;
		cmd.nTccSenderID = pMsg->tcc_sender;
		cmd.bCancelLimit = pMsg->bCancelLimit;

		char buf[1024];
		memset(buf, 0x0, sizeof(buf));
		pMsg->GetComment(buf, 1024);
		//AfxMessageBox(buf);

		//TCC::Receive_TccCommandResult(cmd, pMsg->result, buf);
	}

	return CLBTH::HRCODE(CLBTH::HRCODE::HC_SUCCESS);
}


// 服务器下载确认限速命令列表
// 命令结构: STOCK_COMMAND
// 命令状态与发令人等要设置到原始命令内容中
DEFINE_BASEMSG_PROC(MsgTccCommand, pMsg, vpParam, visitorofWintg)
{
	// 从消息中提取确认命令
	TCC::STOCK_COMMAND cmd;
	cmd.nConsoleID = pMsg->console_no;
	TCC::SysidFromByteArray(cmd.sys_id, pMsg->sys_id, 16);
	TCC::SysidFromByteArray(cmd.raw_sysid, pMsg->raw_sysid, 16);
	cmd.nCmdID = pMsg->cmd_no;
	cmd.nStaID[0] = pMsg->sta_no[0];		// 开始、结束站号
	cmd.nStaID[1] = pMsg->sta_no[1];
	cmd.nLineID = pMsg->line_no;		// 线路号
	cmd.bFullLimit = pMsg->bFullLimit;		// 是否全线限速
	cmd.nKmMark[0] = pMsg->km_marker[0];	// 开始、结束公里标（以m为单位）
	cmd.nKmMark[1] = pMsg->km_marker[1];
	cmd.bStartNow  = pMsg->bStartNow;		// 是否立即开始
	cmd.bEndless   = pMsg->bEndless;		// 是否永久有效
	cmd.nTime[0] = pMsg->time[0];		// 开始、结束时间
	cmd.nTime[1] = pMsg->time[1];
	cmd.nSpeedMaxVal = pMsg->speed;			// 限速值
	cmd.bCancelLimit = pMsg->bCancelLimit;		// 是否 取消限速命令

	cmd.nCreatorID = pMsg->creator;		// 原始命令草拟人
	cmd.nCheckorID = pMsg->checkor;		// 中心校验者
	cmd.nConfirmorID = pMsg->confirmor;	// 确认分解者

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
	//TCC::Receive_StockCommand(cmd);

	return CLBTH::HRCODE(CLBTH::HRCODE::HC_SUCCESS);
}
