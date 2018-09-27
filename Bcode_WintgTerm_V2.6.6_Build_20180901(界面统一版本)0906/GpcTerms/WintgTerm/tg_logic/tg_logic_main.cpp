#include "stdafx.h"
#include "tg_work_schdule_holder.h"
#include "tgdocument.h"
#include <baseMsg\msgid.h>
#include <baseMsg\clbth.h>
#include <baseMsg\msg_schd.h>
#include "schd_newserver.h"
#include "tg_msgcopy_newctcmsg.h"
#include "tg_function_config.h"
#include "tg_basicpara.h"
#include "stplan/stplan.h"
#include "comm_pub.h"
#include "tgmsg.h"
#include "sysshare.h"
#include "basic_schd_config.h"
#include "tg.h"
#include <map>
#include "station_side_full_check.h"

extern basic_schd_config* GetBasicConfig();

BOOL  DB_RemoveObsoleteSaveFiles(const char *filepath);

static CTGFunctionConfig  g_tg_config;
static CTGBasicParam  g_tg_basicparam(&g_tg_config);
static BOOL g_enable_polling=FALSE;
static StationSideOccupyCheck  g_side_full_check;

map<int,TG_STATION_STATUS>  m_mapStationStatus;
map<long,int> m_mapEntityStatus;  // 1 active  0 inavtive

TG_STATION_STATUS *GetStationStatusPtr(int station)
{
	map<int,TG_STATION_STATUS>::iterator iter =m_mapStationStatus.find(station);
	if(iter!=m_mapStationStatus.end())
		return &iter->second; 
	else
		return NULL;
}


void  DB_SetStpcPlanStatus(int station,TG_STATION_STATUS::PLAN_SEND_STATUS  st)
{
	TG_STATION_STATUS *pstat=GetStationStatusPtr(station);
	if(!pstat)
		return;
	pstat->stpc_plan_status=st;
	CTgDocument::GlobalProcessStationStatusInform(station);
}

void  DB_SetLircPlanStatus(int station,TG_STATION_STATUS::PLAN_SEND_STATUS  st)
{
	TG_STATION_STATUS *pstat=GetStationStatusPtr(station);
	if(!pstat)
		return;
	pstat->lirc_plan_status=st;
	CTgDocument::GlobalProcessStationStatusInform(station);
}

void DB_SetPlanSendStatus(long eid,TG_STATION_STATUS::PLAN_SEND_STATUS  st)
{
	GPCENTITY entityInfo; 
	if(!gpcdef.GetEntityInfo(eid,entityInfo))
	{
		return;
	}
	int nStationNo =  entityInfo.nSpecial1;
	if(entityInfo.nGroupId == STPC_WACHER)
		DB_SetStpcPlanStatus(nStationNo,st);
	else if(entityInfo.nGroupId == EQUIP_LIRC)   //lirc
		DB_SetLircPlanStatus(nStationNo,st);
}

void  DB_SetEntityStatus(long eid,int status)
{
	m_mapEntityStatus[eid]=status;
	GpcLogSysMessage(0,"set entity %d status to %d(0:dead 1:alive)", eid,  status);
}

int   DB_GetEntityStatus(long eid)
{
	map<long,int>::iterator  iter=m_mapEntityStatus.find(eid);
	if(iter!=m_mapEntityStatus.end())
		return iter->second;
	else
		return -1;
}

void  DB_SetListenEntitysDead()
{
	map<long,int>::iterator  iter=m_mapEntityStatus.begin();
	while(iter!=m_mapEntityStatus.end())
	{
		iter->second=0;
		iter++;
	}

	// 向综合维护报告各种服务器中断
	if(gpcdef.IsMSSAlarmEnable())
	{
		map<long,int>::iterator  iter=m_mapEntityStatus.begin();
		for(;iter!=m_mapEntityStatus.end();iter++)
		{	
			short AttribIndex=0;
			AttribIndex = GetMSLinkAttribIndexByEntity(iter->first);
			if (AttribIndex>0)
			{
				SetMSLinkAlarm(AttribIndex,0,1);
			}
		}
	}
}

void InitStationStatusMap()
{
	int stationlist[1024];
	int stationcount=1024;
	stationcount=gpcdef.GetStationList(stationcount,stationlist);
	for(int i=0;i<stationcount;i++)
	{
		TG_STATION_STATUS x;
		x.station_no= stationlist[i];
		m_mapStationStatus[stationlist[i]]=x;
	}
}

CTGBasicParam *GetTGBasicParam()
{
	return &g_tg_basicparam;
}
CTGFunctionConfig *GetTGFunctionConfig()
{
	return &g_tg_config;
}

StationSideOccupyCheck* GetSideFullCheck()  
{
	return &g_side_full_check;
}

BOOL  DB_Init(CString strDataPath)
{
	do
	{
		DB_RemoveObsoleteSaveFiles(gpcdef.GetXMLDir());
		if(!g_tg_config.InitData(strDataPath))
			break;
		if(!g_tg_basicparam.InitData())
			break;
		if(!CWintgNewServer::InitSetvers())
			break;
		if(!CWorkScheduleHolder::InitTGHolders())
			break;
		if(!CTgDocument::InitTGDocuments())
			break;
		g_tg_config.checkConfig();
		InitStationStatusMap();
		if(GetTGFunctionConfig()->IsNotMeetBySpeed())
		{
			if(!gpcdef.LoadNotMeetBySpeed())
			{
				AfxMessageBox("加载data目录下文件can_not_meet.xml失败!");
				break;
			}
		}
		else
		{
			gpcdef.LoadNotMeetByType();
		}
		//CTgDocument::InitWS();
		g_enable_polling=TRUE;
		return TRUE;
	}while(0);
	return FALSE;
}

BOOL  DB_RemoveObsoleteSaveFiles(const char *filepath)
{
	CString matchfiles;
	matchfiles.Format("%s/*.*",filepath);
	CFileFind find;
	time_t curtm=time(NULL);
	curtm-=GetTGFunctionConfig()->GetFileKeepTimeHour()*3600;
	if(find.FindFile(matchfiles))
	{
		BOOL  ffrc=TRUE;
		while(ffrc)
		{
			CTime filetime;
			ffrc=find.FindNextFile();
			CString find_file_path=find.GetFilePath();
			if(find.IsDirectory())
			{
				if(find_file_path.Right(1)!=".")  // 特殊的目录
					DB_RemoveObsoleteSaveFiles(find_file_path);  // 递归删除
			}
			else if(find.GetLastWriteTime(filetime)) 
			{
				if(filetime.GetTime() <curtm)
				{
					unlink(find_file_path);
				}
			}
		}
	}
	find.Close();
	return TRUE;
}

BOOL  DB_DeinitData()
{
	g_enable_polling=FALSE;
	g_tg_basicparam.DeinitData();
	CTgDocument::DeleteWS();
	CTgDocument::DeleteAllTgDocuments(); 
	return TRUE;
}


BOOL UR_CanOperateSchedule()
{
	if(gpcdef.GetUserIdentify() == GPC_DISPATCHER ||  
		gpcdef.GetUserIdentify() == GPC_OPERATOR || 
		gpcdef.GetUserIdentify() == GPC_MAINTAIN_DISPATCH ||
		gpcdef.GetUserIdentify() == GPC_TEST)
		return TRUE;
	else
		return FALSE;
}

BOOL UR_CanOperateBasicSchedule()
{
	if(gpcdef.GetUserIdentify() == GPC_TEST)
		return TRUE;
	else
		return FALSE;
}

DECLARE_C_VISITOR(visitorofNewWintg);
DEFINE_BASEMSG_DEFPROC(vpParam, visitorofNewWintg)
{
	int rs = 0;	USHORT usMessageType,usTempLength;

	BYTE *ptr = (BYTE*)vpParam.buf_ptr + CLBTH::MSGHEAD::SIZEOF;
	usTempLength = vpParam.buf_len -   CLBTH::MSGHEAD::SIZEOF;
	usMessageType = vpParam.msgid;
	//CWintgServer::ProcessServerTGMessage(usMessageType,ptr,usTempLength); 
	return CLBTH::HRCODE(CLBTH::HRCODE::HC_SUCCESS);
}

long g_nSourceEntityIDInMessage;
BOOL  DB_ProcessServerTGMessage(int source_eid,BYTE *ptr,int length)
{
	g_nSourceEntityIDInMessage=source_eid;
	CLBTH::Archive RecDataBuffervar( CLBTH::Archive::load,length, ptr);
	CLBTH::EventSink::PostEvent(RecDataBuffervar, GET_C_VISITOR(visitorofNewWintg));
	return TRUE;
}

BOOL  DB_ProcessServerCommStatus(long server_id,BOOL  active_flag)
{
	CWintgNewServer::ProcessServerCommStatus(server_id,active_flag);
	return TRUE;
}

bool TGSendStringMessage(const char *token,const char *content,long eid)
{
	return SendStringMessage(token,content,eid);
}


BOOL  DB_PollingData(void)
{
	if(!g_enable_polling)
		return FALSE;

	// 自动换班
	CTime cur_tm=CTime::GetCurrentTime();

	////////////////////////////////////////
	int shift1 = GetTGFunctionConfig()->GetAutoShift1Time();
	int shift1Hour = shift1/60;
	int shift1Minute = shift1%60;

	int shift2 = GetTGFunctionConfig()->GetAutoShift2Time();
	int shift2Hour = shift2/60;
	int shift2Minute = shift2%60;

	int cur_hour = cur_tm.GetHour();
	int cur_minute = cur_tm.GetMinute();
	if(cur_hour== shift1Hour && cur_minute == shift1Minute)
	{
		CTgDocument::GlobalAutoShiftSchedule();  
	}
	else
	if(cur_hour== shift2Hour && cur_minute == shift2Minute)
	{
		CTgDocument::GlobalAutoShiftSchedule();
	}
	return CWorkScheduleHolder::Polling(cur_tm.GetTime()); 

}

DEFINE_BASEMSG_PROC(MsgHugeDataCtrl, pBuf , vpParam, visitorofNewWintg)
{
    CLBTH::MsgHugeDataCtrl *pMsg= ( CLBTH::MsgHugeDataCtrl *)pBuf;
	TG_MsgStamp stamp;
	int count=0;
	if (pMsg->status==CLBTH::MsgHugeDataCtrl::MSG_END)
	{
		count = (pMsg->msgpara>>16)&0xFFFF;
		stamp.con_id=(pMsg->msgpara>>8)&0xFF; 
		stamp.schedule_type=pMsg->msgpara&0xFF;
	}
	else
	{
		stamp.con_id=(pMsg->msgpara>>8)&0xFF; 
		stamp.schedule_type=pMsg->msgpara&0xFF;
	}

	BOOL va;
	if(pMsg->status==CLBTH::MsgHugeDataCtrl::MSG_END)
		va=TRUE;
	else
		va=FALSE;
	if(pMsg->msgid==MSGID_MsgSchdBlockData)
		CWintgNewServer::GlobalSetInitStatus(stamp,TG_OBJ_MARK,va,count);
	else if(pMsg->msgid==MSGID_MsgTrainData)
		CWintgNewServer::GlobalSetInitStatus(stamp,TG_OBJ_TRAIN,va,count);
	return CLBTH::HRCODE(CLBTH::HRCODE::HC_SUCCESS);
}

// 批量数据
DEFINE_BASEMSG_PROC(MsgSchdBlockData, pBuf , vpParam, visitorofNewWintg)
{ 
	CLBTH::MsgSchdBlockData *pMsg= ( CLBTH::MsgSchdBlockData *)pBuf;
	CTG_SingleMark mark;
	TG_MsgStamp stamp;
	BlockCopy(mark,*pMsg);
	CopyStamp(stamp,pMsg->stamp);
	mark.SetBlockIndex(stamp.index);    
	CWintgNewServer::GlobalInitAppendData(stamp,mark);
	return CLBTH::HRCODE(CLBTH::HRCODE::HC_SUCCESS);
}
// 标记增加
DEFINE_BASEMSG_PROC(MsgSchdBlockAdd, pBuf , vpParam, visitorofNewWintg)
{ 
	//here only listen this message, and set flag to stimulate the "load schedule" message 
	CLBTH::MsgSchdBlockAdd *pMsg= ( CLBTH::MsgSchdBlockAdd *)pBuf;
	CTG_SingleMark mark;
	TG_MsgStamp stamp;
	BlockCopy(mark,*pMsg);
	CopyStamp(stamp,pMsg->stamp);
	mark.SetBlockIndex(stamp.index); 
	CWintgNewServer::GlobalUpdateData(stamp,IScheduleComm::TG_OPER_ADD,mark);
	return CLBTH::HRCODE(CLBTH::HRCODE::HC_SUCCESS);
}
// 标记删除
DEFINE_BASEMSG_PROC(MsgSchdBlockDelete, pBuf , vpParam, visitorofNewWintg)
{ 
	//here only listen this message, and set flag to stimulate the "load schedule" message 
	CLBTH::MsgSchdBlockDelete *pMsg= ( CLBTH::MsgSchdBlockDelete *)pBuf;
	CTG_SingleMark mark;
	TG_MsgStamp stamp;
	BlockCopy(mark,*pMsg);
	CopyStamp(stamp,pMsg->stamp);
	mark.SetBlockIndex(stamp.index); 
	CWintgNewServer::GlobalUpdateData(stamp,IScheduleComm::TG_OPER_DELETE,mark);
	return CLBTH::HRCODE(CLBTH::HRCODE::HC_SUCCESS);
}
// 标记更新
DEFINE_BASEMSG_PROC(MsgSchdBlockUpdate, pBuf , vpParam, visitorofNewWintg)
{ 
	//here only listen this message, and set flag to stimulate the "load schedule" message 
	CLBTH::MsgSchdBlockUpdate *pMsg= ( CLBTH::MsgSchdBlockUpdate *)pBuf;
	CTG_SingleMark mark;
	TG_MsgStamp stamp;
	BlockCopy(mark,*pMsg);
	CopyStamp(stamp,pMsg->stamp);
	mark.SetBlockIndex(stamp.index); 
	CWintgNewServer::GlobalUpdateData(stamp,IScheduleComm::TG_OPER_UPDATE,mark);
	return CLBTH::HRCODE(CLBTH::HRCODE::HC_SUCCESS);
}

// 接收服务器MsgTrainExtent数据
DEFINE_BASEMSG_PROC(MsgTrainExtent, pBuf , vpParam, visitorofNewWintg)
{ 
	CLBTH::MsgTrainExtent *pMsg= ( CLBTH::MsgTrainExtent *)pBuf;
	TG_MsgStamp stamp;
	CTG_TrainExtent trainExtent;

	CopyStamp(stamp,pMsg->stamp);
	TrainExtentCopy(trainExtent, *pMsg);
	trainExtent.SetTrainIndex(stamp.index);

	CWintgNewServer::GlobalTrainExtent(stamp, trainExtent);
	return CLBTH::HRCODE(CLBTH::HRCODE::HC_SUCCESS);
}

// 批量数据
DEFINE_BASEMSG_PROC(MsgTrainData, pBuf , vpParam, visitorofNewWintg)
{ 
	//here only listen this message, and set flag to stimulate the "load schedule" message 
	CLBTH::MsgTrainData *pMsg= ( CLBTH::MsgTrainData *)pBuf;
	CTG_SingleTrain train;
	TG_MsgStamp stamp;
	TrainCopy(train,*pMsg);
	CopyStamp(stamp,pMsg->stamp);
	train.SetTrainIndex(stamp.index); 
	CWintgNewServer::GlobalInitAppendData(stamp,train);
	return CLBTH::HRCODE(CLBTH::HRCODE::HC_SUCCESS);
}

// 加车
DEFINE_BASEMSG_PROC(MsgTrainAdd, pBuf , vpParam, visitorofNewWintg)
{ 
	//here only listen this message, and set flag to stimulate the "load schedule" message 
	CLBTH::MsgTrainAdd *pMsg= ( CLBTH::MsgTrainAdd *)pBuf;
	CTG_SingleTrain train;
	TG_MsgStamp stamp;
	TrainCopy(train,*pMsg);
	CopyStamp(stamp,pMsg->stamp);
	train.SetTrainIndex(stamp.index); 
	if(train.GetTrainRecordCount()<=0)
		CWintgNewServer::GlobalUpdateData(stamp,IScheduleComm::TG_OPER_ADD,train);
	else
		CWintgNewServer::GlobalUpdateData(stamp,IScheduleComm::TG_OPER_ADD,train);
	return CLBTH::HRCODE(CLBTH::HRCODE::HC_SUCCESS);
}
// 车次删除
DEFINE_BASEMSG_PROC(MsgTrainDelete, pBuf , vpParam, visitorofNewWintg)
{ 
	//here only listen this message, and set flag to stimulate the "load schedule" message 
	CLBTH::MsgTrainDelete *pMsg= ( CLBTH::MsgTrainDelete *)pBuf;
	CTG_SingleTrain train;
	TG_MsgStamp stamp;
	TrainCopy(train,*pMsg);
	CopyStamp(stamp,pMsg->stamp);
	train.SetTrainIndex(stamp.index); 
	CWintgNewServer::GlobalUpdateData(stamp,IScheduleComm::TG_OPER_DELETE,train);
	return CLBTH::HRCODE(CLBTH::HRCODE::HC_SUCCESS);
}
// 车次整体更新
DEFINE_BASEMSG_PROC(MsgTrainUpdate, pBuf , vpParam, visitorofNewWintg)
{ 
	//here only listen this message, and set flag to stimulate the "load schedule" message 
	CLBTH::MsgTrainUpdate *pMsg= ( CLBTH::MsgTrainUpdate *)pBuf;
	CTG_SingleTrain train;
	TG_MsgStamp stamp;
	TrainCopy(train,*pMsg);
	CopyStamp(stamp,pMsg->stamp);
	train.SetTrainIndex(stamp.index); 
	CWintgNewServer::GlobalUpdateData(stamp,IScheduleComm::TG_OPER_UPDATE,train);
	return CLBTH::HRCODE(CLBTH::HRCODE::HC_SUCCESS);
}

// 接收服务器train_expand数据
DEFINE_BASEMSG_PROC(MsgTrainExpand, pBuf , vpParam, visitorofNewWintg)
{ 

	return CLBTH::HRCODE(CLBTH::HRCODE::HC_SUCCESS);
}

//////////// STPLAN
DEFINE_BASEMSG_PROC(MsgStPlanTrainRecv, pBuf , vpParam, visitorofNewWintg)
{ 
	//here only listen this message, and set flag to stimulate the "load schedule" message 
	CLBTH::MsgStPlanTrainRecv *pMsg= ( CLBTH::MsgStPlanTrainRecv *)pBuf;
	cStPlan::GlobalProcessMessage(g_nSourceEntityIDInMessage,pMsg);
	return CLBTH::HRCODE(CLBTH::HRCODE::HC_SUCCESS);
}

DEFINE_BASEMSG_PROC(MsgStPlanRecvALL, pBuf , vpParam, visitorofNewWintg)
{ 
	//here only listen this message, and set flag to stimulate the "load schedule" message 
	CLBTH::MsgStPlanRecvALL *pMsg= ( CLBTH::MsgStPlanRecvALL *)pBuf;
	cStPlan::GlobalProcessMessage(g_nSourceEntityIDInMessage,pMsg);
	return CLBTH::HRCODE(CLBTH::HRCODE::HC_SUCCESS);
}

DEFINE_BASEMSG_PROC(MsgRouteStatusInform, pBuf , vpParam, visitorofNewWintg)
{ 
	//here only listen this message, and set flag to stimulate the "load schedule" message 
	CLBTH::MsgRouteStatusInform *pMsg= ( CLBTH::MsgRouteStatusInform *)pBuf;
	cStPlan::GlobalProcessMessage(g_nSourceEntityIDInMessage,pMsg);
	return CLBTH::HRCODE(CLBTH::HRCODE::HC_SUCCESS);
}

DEFINE_BASEMSG_PROC(MsgSchdMsgEcho, pBuf , vpParam, visitorofNewWintg)
{ 
	//here only listen this message, and set flag to stimulate the "load schedule" message 
	CLBTH::MsgSchdMsgEcho *pMsg= ( CLBTH::MsgSchdMsgEcho *)pBuf;
	TG_MsgStamp stamp;
	//TrainCopy(train,*pMsg);	
	TG_OBJECT_TYPE  obj_type;
	if(pMsg->msgid==MsgTrainAdd::getTypeIDStatic() ||
		pMsg->msgid==MsgTrainDelete::getTypeIDStatic() ||
		pMsg->msgid==MsgTrainUpdate::getTypeIDStatic() ||
		pMsg->msgid==MsgTrainUpdateInfo::getTypeIDStatic() ||
		pMsg->msgid==MsgTrainUpdateRecord::getTypeIDStatic() ||
		pMsg->msgid==MsgTrainUpdateEngine::getTypeIDStatic())
		obj_type=TG_OBJ_TRAIN;
	else
	if(pMsg->msgid==MsgSchdBlockAdd::getTypeIDStatic() || 
			pMsg->msgid==MsgSchdBlockDelete::getTypeIDStatic() || 
			pMsg->msgid==MsgSchdBlockUpdate::getTypeIDStatic()) 
		obj_type=TG_OBJ_MARK;
	else
		return CLBTH::HRCODE(CLBTH::HRCODE::HC_SUCCESS);
	CopyStamp(stamp,pMsg->stamp);
	CWintgNewServer::GlobalProcessServerErrorEcho(stamp,obj_type,pMsg->ecode);
	return CLBTH::HRCODE(CLBTH::HRCODE::HC_SUCCESS);
}

////// 阶段记事

DEFINE_BASEMSG_PROC(MsgWordPhasePlanSend, pBuf , vpParam, visitorofNewWintg)
{ 
	//here only listen this message, and set flag to stimulate the "load schedule" message 
	CLBTH::MsgWordPhasePlanSend *pMsg= ( CLBTH::MsgWordPhasePlanSend *)pBuf;
	CTgDocument::GlobalProcessMessage(g_nSourceEntityIDInMessage,pMsg);
	return CLBTH::HRCODE(CLBTH::HRCODE::HC_SUCCESS);
}
DEFINE_BASEMSG_PROC(MsgWordPhasePlanStatus, pBuf , vpParam, visitorofNewWintg)
{ 
	//here only listen this message, and set flag to stimulate the "load schedule" message 
	CLBTH::MsgWordPhasePlanStatus *pMsg= ( CLBTH::MsgWordPhasePlanStatus *)pBuf;
	CTgDocument::GlobalProcessMessage(g_nSourceEntityIDInMessage,pMsg);
	return CLBTH::HRCODE(CLBTH::HRCODE::HC_SUCCESS);
}

///////////////////////
DEFINE_BASEMSG_PROC(MsgStringMessage, pBuf , vpParam, visitorofNewWintg)
{ 
	//here only listen this message, and set flag to stimulate the "load schedule" message 
	CLBTH::MsgStringMessage *pMsg= ( CLBTH::MsgStringMessage *)pBuf;
	GpcLogDebugMessage(0,"收到MsgStringMessage %s.", pMsg->GetToken());
	if(stricmp(pMsg->GetToken(),"RE_LOGIN")==0)
	{
		GpcLogDebugMessage(0,"收到调度命令调度员名字申请 %s.", pMsg->GetContent());
		////// 通知调度命令管理程序
		CString token,content;
		CString username;
		username=gpcdef.GetCurrentUsername(); 
		if(GenXMLMessage_InformDCMDLogin(token,content,username))
		{
			long nEntityOfDCMD=GetSpecialEntityID(GPCTERM_TYPE_DCMD,0);
			if(nEntityOfDCMD>0)
			{
				TGSendStringMessage(token,content,nEntityOfDCMD);
				GpcLogDebugMessage(0,"应调度命令申请发送调度员名字 %s.", content);
			}
		}
	}
	else
	{
		CTgDocument::GlobalProcessMessage(g_nSourceEntityIDInMessage,pMsg);
	}
	return CLBTH::HRCODE(CLBTH::HRCODE::HC_SUCCESS);
}
// 通信服务器发来的Entity状态
DEFINE_BASEMSG_PROC(MsgEntityConnStatus, pBuf, vpParam, visitorofNewWintg)
{ 
	CLBTH::MsgEntityConnStatus *pEntityStatus= ( CLBTH::MsgEntityConnStatus *)pBuf;
	GPCENTITY entityInfo;
	CString sClExpress; int nStationNo;

	DWORD ulEntiryIDCl = pEntityStatus->s_client;

	//GpcLogDebugMessage(0,"收到MsgEntityConnStatus server:%d client:%d board_no:%d flag:%d status:%d",pEntityStatus->s_server, pEntityStatus->s_client, pEntityStatus->board_no, pEntityStatus->nABFlag, pEntityStatus->status);
	if(pEntityStatus->status == CLBTH::MsgEntityConnStatus::ENTITY_CONNECT)
		DB_SetEntityStatus(ulEntiryIDCl,1);
	else
		DB_SetEntityStatus(ulEntiryIDCl,0);
	
	do
	{
		if (!gpcdef.GetEntityInfo(ulEntiryIDCl,entityInfo))
		{
			GpcLogDebugMessage(0,"收到entity %d 状态,但无法从entity.cfg中查找相关信息",ulEntiryIDCl);
			break;
		}

		// 服务器不用主动设置，因为new_server会自己去查询entity_ID状态
		if(entityInfo.nGroupId==EQUIP_SERVER) // 服务器状态
		{

			// 2010.06.28 当服务器状态快速变化时, GPC检测不到, 导致数据不更新,所以在次
			// 直接设置状态, 只处理服务器变死的情况
			if(pEntityStatus->status != CLBTH::MsgEntityConnStatus::ENTITY_CONNECT)
			{
				DB_ProcessServerCommStatus(ulEntiryIDCl,FALSE);
			}
			
			if (gpcdef.IsMSSAlarmEnable())
			{
				short AttribIndex=0;
				AttribIndex = GetMSLinkAttribIndexByEntity(ulEntiryIDCl);
				if (AttribIndex>0)
				{
					if(pEntityStatus->status == CLBTH::MsgEntityConnStatus::ENTITY_CONNECT)
						SetMSLinkAlarm(AttribIndex,1,1);
					else
						SetMSLinkAlarm(AttribIndex,0,1);
				}
			}

			GpcLogSysMessage(0,"Server entity %d status changed to %d",ulEntiryIDCl,pEntityStatus->status);
			break;
		}
		// 车站状态
  		nStationNo =  entityInfo.nSpecial1;
		if(entityInfo.nGroupId != STPC_WACHER && //stpc
			entityInfo.nGroupId != EQUIP_LIRC)   //lirc
		{
	 		GpcLogDebugMessage(0,"收到entity %d 状态,但不是STPC 或者LIRC",ulEntiryIDCl);
			break;
		}
		TG_STATION_STATUS *pstat=GetStationStatusPtr(nStationNo);
		if(!pstat)
		{
			GpcLogDebugMessage(0,"收到entity %d 状态,但找不到对应车站 %d",ulEntiryIDCl,nStationNo);
			break;
		}
	
		if(pEntityStatus->status == CLBTH::MsgEntityConnStatus::ENTITY_CONNECT)
		{
			if(entityInfo.nGroupId == STPC_WACHER)
				pstat->stpc_status= TG_STATION_STATUS::ENTITY_ACTIVE;
			else
				pstat->lirc_status= TG_STATION_STATUS::ENTITY_ACTIVE;
		}
		else
		{
			if(entityInfo.nGroupId == STPC_WACHER)
				pstat->stpc_status= TG_STATION_STATUS::ENTITY_INACTIVE;
			else
				pstat->lirc_status= TG_STATION_STATUS::ENTITY_INACTIVE;
		}
		CTgDocument::GlobalProcessStationStatusInform(nStationNo);
	}while(0);
	return CLBTH::HRCODE(CLBTH::HRCODE::HC_SUCCESS);
}  

//自律机控制模式状态
DEFINE_BASEMSG_PROC(MsgStationModeReport, pBuf, vpParam, visitorofNewWintg)
{ 
	CLBTH::MsgStationModeReport *pMsg= ( CLBTH::MsgStationModeReport *)pBuf;
    TG_STATION_STATUS *pstat=GetStationStatusPtr(pMsg->nStationNo);
	if(pstat)
	{
		pstat->ctrl_mode =pMsg->nStationMode;
		pstat->route_mode =pMsg->nRouteMode;
		pstat->lirc_status=TG_STATION_STATUS::ENTITY_ACTIVE;
	 	CTgDocument::GlobalProcessStationStatusInform(pMsg->nStationNo);
		GpcLogDebugMessage(0,"收到车站 %d 控制模式状态, 控制模式 %d 进路模式 %d",pMsg->nStationNo, pMsg->nStationMode, pMsg->nRouteMode);
	}
	else
		GpcLogDebugMessage(0,"收到车站 %d 控制模式状态,但找不到对应车站",pMsg->nStationNo);
	return CLBTH::HRCODE(CLBTH::HRCODE::HC_SUCCESS);

}  

DEFINE_BASEMSG_PROC(MsgStPlanRequest, pBuf, vpParam, visitorofNewWintg)
{ 
	CLBTH::MsgStPlanRequest *pMsg= ( CLBTH::MsgStPlanRequest *)pBuf;
	TG_STATION_STATUS *pstat=GetStationStatusPtr(pMsg->station);
	if(pstat)
	{
		pstat->lirc_plan_status= TG_STATION_STATUS::NEED_RESEND;      
		CTgDocument::GlobalProcessStationStatusInform(pMsg->station);
	}
	else
		GpcLogDebugMessage(0,"收到车站 %d 计划请求,但找不到对应车站",pMsg->station);
	return CLBTH::HRCODE(CLBTH::HRCODE::HC_SUCCESS);
}  

DEFINE_BASEMSG_PROC(MsgBasicScheduleSet, pBuf, vpParam, visitorofNewWintg)
{ 
	CLBTH::MsgBasicScheduleSet *pMsg= ( CLBTH::MsgBasicScheduleSet *)pBuf;

	// 2017-09-11 这段代码和 CDlgBasicDeleteName中重复，但是不能删除，很有可能对话框不存在
	pMsg->nScheduleName[MAX_SCHEDULE_NAME_LEN-1]=0;
	GpcLogDebugMessage(0,"收到基本图名字 con_id %d schd_type %d basic_name %s",
					pMsg->nConId, pMsg->nScheduleId, pMsg->nScheduleName);

	if(GetBasicConfig()->SetBasicSchdName(pMsg->nConId, pMsg->nScheduleId, pMsg->nScheduleName, pMsg->nStartTime, pMsg->nEndTime, pMsg->nEstimateStartTime))
		GpcLogDebugMessage(0,"保存基本图名字成功");
	else
		GpcLogDebugMessage(0,"保存基本图名字失败");
	
	return CLBTH::HRCODE(CLBTH::HRCODE::HC_SUCCESS);
}  
