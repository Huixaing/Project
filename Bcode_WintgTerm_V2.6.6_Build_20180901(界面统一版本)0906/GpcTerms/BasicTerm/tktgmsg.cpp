#include "stdafx.h"
#include "widgets.h"  //wangjinhai
#include "tg.h"
#include "oscalls.h"
#include "sysshare.h"
#include "messages.h"
#include "dbfunc.h"
#include "history.h"
#include "msg_id.h"
#include "mainframe.h"

#include "tgmsg.h"
#include "function.h"
#include "plotdll/plotview.h"
//#include "NewDispatcherCommand.h"
#include "MessageDlg.h"

#include <baseMsg\msgid.h>
#include <baseMsg\clbth.h>
//#include "MacroSwitch.h"
#include "icommtask.h"
#include "tg.h"
#include "db_msgproc.h"
#include "tgdocument.h"
#include "MainFrame.h"
#define HEAD_LEN   64  // 
#include "DlgMessageBox.h"



static int nSchdType;
static long nCurrentTime,nCurrentDate;

short InitStationEntryAbsnumConvertTable(void);
int OsPurgeAllConflictInform( );
static void	terminate_task(void);
static int	process_msg(void);


DECLARE_C_VISITOR(visitorofWintg);

//默认消息处理
DEFINE_BASEMSG_DEFPROC(vpParam, visitorofWintg)
{
	return CLBTH::HRCODE(CLBTH::HRCODE::HC_SUCCESS);
}

//实体连接状态
DEFINE_BASEMSG_PROC(MsgStPlanStatusAnswer, pBufStPlanStatusAnswer, vpParam, visitorofWintg)
{
	return CLBTH::HRCODE(CLBTH::HRCODE::HC_SUCCESS);
}  

// 行车事故概括
DEFINE_BASEMSG_PROC(MsgAccidentOper, pBufMsgAccidentOper, vpParam, visitorofWintg)
{  
	CWnd * pwnd=CWnd::FindWindow(NULL,"行车事故概况");
	if(pwnd)
		pwnd->SendMessage(GIVE_ACCIDENT, (WPARAM)pBufMsgAccidentOper, (LPARAM)0);
	return CLBTH::HRCODE(CLBTH::HRCODE::HC_SUCCESS);

}  

DEFINE_BASEMSG_PROC(MsgUserManager, pBufMsgUserManager, vpParam, visitorofWintg)
{  
	if(pBufMsgUserManager->oper == CLBTH::MsgUserManager::USERMGR_CHANGE_PROP)
	{  
		CWnd * pwnd=CWnd::FindWindow(NULL,"修改密码");
		if(pwnd)
		{ 
			pwnd->SendMessage(UM_CHANGE_PASSWORD_MSG, (WPARAM)pBufMsgUserManager, (LPARAM)0);
			return CLBTH::HRCODE(CLBTH::HRCODE::HC_SUCCESS); 
		}

	}
	return CLBTH::HRCODE(CLBTH::HRCODE::HC_SUCCESS);
}  

// 列车甩挂
DEFINE_BASEMSG_PROC(MsgGroupTrainWorkFormat, pBufMsgGroupTrainWorkFormat, vpParam, visitorofWintg)
{
	return CLBTH::HRCODE(CLBTH::HRCODE::HC_SUCCESS);
}  

//GPC件消息确认
DEFINE_BASEMSG_PROC(MsgGpcMessage, pBuf0, vpParam, visitorofWintg)
{ 
	CLBTH::MsgGpcMessage  *pBuf  = (CLBTH::MsgGpcMessage *)pBuf0;
	if(pBuf->cmd_type == CLBTH::MsgGpcMessage::Msg_REQUEST)
	{
		char pack[1025];
		memcpy(pack, pBuf->package, pBuf->pack_len);
		pack[pBuf->pack_len] = 0;

		CString strText;
		strText.Format("请确认：%s", pack);
		strText.TrimLeft(); strText.TrimRight();
		CDlgMessageBox dlg(strText,AfxGetMainWnd());

		CLBTH::MsgGpcMessage  mMsgGpcMessage ;
		if(dlg.DoModal() != IDOK)
		{
			GpcLogTrainMessage(0,0,"[Answer],我拒绝确认消息： %s,来自%d, 目标：%d,请求时间：%d", 
				strText,pBuf->source_id, pBuf->dest_id,pBuf->dest_id, pBuf->tm_request );
			mMsgGpcMessage.answer_type = CLBTH::MsgGpcMessage::ANSWER_DENAY;

		}
		else
		{
			GpcLogTrainMessage(0,0,"[Answer],我确认消息： %s,来自%d, 目标：%d,请求时间：%d", 
				strText,pBuf->source_id, pBuf->dest_id,pBuf->dest_id, pBuf->tm_request );
			mMsgGpcMessage.answer_type = CLBTH::MsgGpcMessage::ANSWER_OK;
		}



		CTime tCurrent = CTime::GetCurrentTime();
		GPCENTITY  entityInfo;
		gpcdef.GetEntityInfo(gpcdef.GetCurUserName(),entityInfo);

		mMsgGpcMessage.label = pBuf->label;            
		mMsgGpcMessage.source_id = entityInfo.nEntityId;  
		mMsgGpcMessage.dest_id =  pBuf->source_id;  
		mMsgGpcMessage.cmd_type = CLBTH::MsgGpcMessage::Msg_REPLY;



		mMsgGpcMessage.tm_request = pBuf->tm_request;  
		mMsgGpcMessage.tm_reply = tCurrent.GetTime();


		mMsgGpcMessage.pack_len = pBuf->pack_len;

		strncpy(mMsgGpcMessage.package, pBuf->package, mMsgGpcMessage.pack_len );


		SendBMsg(&mMsgGpcMessage,mMsgGpcMessage.dest_id); 

		GpcLogTrainMessage(0,0,"[PMTG]:消息传送,下发目标:%d",mMsgGpcMessage.dest_id);
	}
	return CLBTH::HRCODE(CLBTH::HRCODE::HC_SUCCESS);

}  

bool TGMessageProcessNew(long entity_id,unsigned char *ptr,int length)
{
 	CLBTH::Archive RecDataBuffervar( CLBTH::Archive::load,length, ptr);
	CLBTH::EventSink::PostEvent(RecDataBuffervar, GET_C_VISITOR(visitorofWintg));
	DB_ProcessServerTGMessage(entity_id,ptr,length); // 本地数据库处理
	return true;
}

bool LocalTGRequestMessageProcess(unsigned char *ptr,int length)// 原先由界面任务发来的
{
 	CLBTH::Archive RecDataBuffervar( CLBTH::Archive::load,length, ptr);
	CLBTH::EventSink::PostEvent(RecDataBuffervar, GET_C_VISITOR(visitorofWintg));
//	DB_ProcessTGRequestMessage(ptr,length); // 本地数据库处理
	return true;
}

//// 供电臂设置
DEFINE_BASEMSG_PROC(MsgElecArmAnswer, pBuf, vpParam, visitorofWintg)
{
    CLBTH::MsgElecArmAnswer  *pArm = ( CLBTH::MsgElecArmAnswer *)pBuf;

	CFrameWnd *pWnd = ((CWintg *)AfxGetApp())->GetMainFrame();
	if(pWnd == NULL)
		return CLBTH::HRCODE(CLBTH::HRCODE::HC_SUCCESS);

	if(pArm->status==CLBTH::MsgElecArmAnswer::ELEC_ENABLE)
		pWnd->SendMessage(UM_ARMS_STATUS_INFORM, (pArm->elec_arm_id<<16)+pArm->elec_sta,3);
	else
		pWnd->SendMessage(UM_ARMS_STATUS_INFORM, (pArm->elec_arm_id<<16)+pArm->elec_sta,1);
	return CLBTH::HRCODE(CLBTH::HRCODE::HC_SUCCESS);
}

static int	AddStationList(int nStationNo,int nAllStationNum,int *nAllStationList)
{
	int i;
	for(i=0;i < nAllStationNum;i++)
	{
		if(nAllStationList[i] == nStationNo)
			break;
	}
	if(i >= nAllStationNum)
	{
		nAllStationList[nAllStationNum] = nStationNo;
		return 0;
	}
	return -1;
}

int SendTGCommMessage(int section_id,ushort usMsgSize,const char *abBufferMsg,long entity_id)
{
	if(entity_id==0)
	    entity_id=GetSpecialEntityID(SERVER_TYPE_TG,section_id);
	if(entity_id>=0)
	{
		SendCommMessage(entity_id,0,(BYTE *)abBufferMsg,usMsgSize); //entityInfo.nEntityId
		//DB_ProcessTGCommSendMessage((BYTE *)abBufferMsg,usMsgSize);// 截获发往服务器的消息用于本地数据库处理
		return RET_SUCCESS;
	}
	return -2;
}

bool SendTGBMsg(int section_id,CLBTH::BaseMsg* pOneMsg,long dest_entity,short flag) // 默认以本机ID,自动LABEL,不带序号发出
{
	if(dest_entity==0)
		dest_entity=GetSpecialEntityID(SERVER_TYPE_TG,section_id);
	return SendBMsg(pOneMsg,dest_entity,flag); // 默认以本机ID,自动LABEL,不带序号发出	
}

//////////////////////////////////////////////////////////////////////////////
int	AddTGSectionList(int nSectionNo,int nTgSectionNum,int *nTgSectionList, int max)
{
	int i;
	for(i=0; i<nTgSectionNum&&i<max; i++)
	{
		if(nTgSectionList[i] == nSectionNo)
			break;
	}
	if(i >= nTgSectionNum)
	{
		nTgSectionList[nTgSectionNum] = nSectionNo;
		return 0;
	}

	return -1;
}

void AddEntityToMsg(MsgEntityConnStatusOrder &msg,ENTITY_ID eid)
{
	ASSERT(msg.entity_count<LOGIN_MAX_ENTITY_COUNT);
	for(int i=0; i<msg.entity_count; i++)
	{
		if(msg.entity_list[i]==eid)
		{
			return;
		}
	}
	msg.entity_list[msg.entity_count++]=eid;
}

void  RequestAllEntityStatus()
{
    MsgEntityConnStatusOrder msg;
	GPCENTITY entityInfo;
	
	// svr_td
	AddEntityToMsg(msg,GetSpecialEntityID(SERVER_TYPE_TDCOMM,0));
	GpcLogTrainMessage(0, 0, "查询 td svrid=%d", GetSpecialEntityID(SERVER_TYPE_TDCOMM,0));

	if (gpcdef.IsOtherMsgToGateway())
	{
		// svr_gateway
		AddEntityToMsg(msg,GetSpecialEntityID(SERVER_TYPE_GATEWAY,0));
		GpcLogTrainMessage(0, 0, "查询 svr_gateway svrid=%d", GetSpecialEntityID(SERVER_TYPE_GATEWAY,0));
	}

	// svr_basic
	AddEntityToMsg(msg,GetSpecialEntityID(SERVER_TYPE_BASIC_TG,0));
	GpcLogTrainMessage(0, 0, "查询 svr_basic svrid=%d", GetSpecialEntityID(SERVER_TYPE_BASIC_TG,0));

	// svr_sdb
	AddEntityToMsg(msg,GetSpecialEntityID(SERVER_TYPE_PARA,0));
	GpcLogTrainMessage(0, 0, "查询 svr_sdb svrid=%d", GetSpecialEntityID(SERVER_TYPE_PARA,0));

	msg.flag=MsgEntityConnStatusOrder::CONN_INIT; 
	SendBMsg(&msg,0,0);
}

///////////////////////// 网络状态处理
bool  NetworkEntityStatusProcessNew(int evt_type,long entity_id, int status)
{
	GpcLogTrainMessage(0, 0, "收到unicomm状态通知, evt_type=%d, entity_id=%d, status=%d", evt_type, entity_id, status);

	if(evt_type==0) // connect status
	{
		if(status==0) // network is broken
		{
			GpcLogTrainMessage(0, 0, "WintgTerm和Commsvr网络连接中断");// 2013.05.07处理运行图与Commsvr闪断
		
		}
		else if (status==1) // 正在连接
		{
			GpcLogTrainMessage(0, 0, "WintgTerm和Commsvr正在连接");
		}
		else if (status==2)
		{
			GpcLogTrainMessage(0, 0, "WintgTerm和Commsvr建立连接");
		}
		else if(status==3) // network is ok
		{
			GpcLogTrainMessage(0, 0, "WintgTerm登陆Commsvr成功");// 2013.05.07处理运行图与Commsvr闪断
			RequestAllEntityStatus();  // 向通信服务器获取Entity状态
		}
		else if(status==254) // 通信任务即将启动 设置relaentity
		{
			GpcLogTrainMessage(0, 0, "WintgTerm正在启动通信任务");
		}
		else if (status==255) // network is off
		{
			GpcLogTrainMessage(0, 0, "WintgTerm和Commsvr通信任务终止,unicomm不再自动重连");
			
		}

		return true;
	}

	return true;
}
