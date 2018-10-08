#include "stdafx.h"
#include "..\basemsg\clbth.h"
#include "tg_schedule_base.h"
#include "../database/caddata.h"
#include "../database/caddataserver_schd.h"
#include "../tgmsg.h"
#include "tg_datacopy.h"
#include "tg_msgcopy_oldctc.h"
#include "schd_oldmmi.h"
#include "schd_oldserver.h"
#include "schd_database.h"
#include "tg_work_schdule_holder.h"

#include "wintgdoc.h"
#include "widgets.h"  //wangjinhai
#include "MacroSwitch.h"
#include "tgmsg.h"
#include "sysshare.h"
#include "mainframe.h"
#include "../database/db_util.h"
#include "db_msgproc.h"

extern CWintgDoc  *m_pWintgDocPtr;


CWintgMMI  *g_pWintgWorkScheduleMMI=NULL;
CWintgMMI  *g_pWintgBasicScheduleMMI=NULL;
CWintgServer *g_pWintgServerWork=NULL;
CWintgServer *g_pWintgServerBasic=NULL;
CWorkScheduleHolder *g_pCurrentWorkScheduleData=NULL;
CSchduleDatabaseInterface *g_pDatabaseServer=NULL;

CWorkScheduleHolder * CreateWorkScheduleHolder(int section);


static BOOL  InitLogicData(int section);
BOOL DB_ClearTGDocData(int sch_type);

BOOL  DB_InitData(int conid)
{
	//return InitLogicData(conid);
	return FALSE;
}
BOOL  DB_DeinitData()
{
	 if(g_pWintgBasicScheduleMMI)
		 delete g_pWintgBasicScheduleMMI;
	 if(g_pWintgWorkScheduleMMI)
		 delete g_pWintgWorkScheduleMMI;
	 if(g_pWintgServerWork)
		 delete g_pWintgServerWork;
	 if(g_pWintgServerBasic)
		 delete g_pWintgServerBasic;
	 if(g_pDatabaseServer)
		 delete g_pDatabaseServer;
	 g_pWintgBasicScheduleMMI=NULL;
	 g_pWintgWorkScheduleMMI=NULL;
	 g_pWintgServerWork=NULL;
	 g_pWintgServerBasic=NULL;
	 g_pDatabaseServer=NULL;
	 return TRUE;
}


BOOL  TestTrainExsitance(TRAIN_INDEX index)  // 特殊应用于：更新WINTGFORM 必须知道车次是否存在
{
	if(!m_pWintgDocPtr)
		return FALSE;
	if(m_pWintgDocPtr->FindTrain(index,WORK_SCHEDULE)>=0)
		return TRUE;
	else
		return FALSE;
}

BOOL TestMarkExsitance(TRAIN_INDEX index)
{
	if(!m_pWintgForm)
		return FALSE;
	
	if(m_pWintgForm->m_nAbsList.GetFigure(index))
		return TRUE;
	else
		return FALSE;
}


BOOL   DB_UpdateSchdule_BlockMark(int con_id, int schd_type,int oper_type,const struct _tagMarkItem *pItem) 
{
	if(!g_pWintgWorkScheduleMMI)
		return FALSE;
	CADDATA_ABS_MARK mark_item;
	mark_item.m_nSyncFlag = 0;  // NOT USED
	mark_item.m_data = *pItem;
	return g_pWintgWorkScheduleMMI->ProcessMMIOperation(con_id,schd_type,oper_type,mark_item); 
}

BOOL   DB_UpdateSchdule_DeleteTrain(int con_id,int schd_type,unsigned long train_index)
{
	if(!g_pWintgWorkScheduleMMI)
		return FALSE;
	return g_pWintgWorkScheduleMMI->ProcessMMIOperation_DeleteTrain(con_id,schd_type,train_index); 
}

BOOL   DB_UpdateSchdule_SingleTrain(int con_id, int schd_type,int oper_type,int sub_type,CADDATA_SINGLE_TRAIN &train)
{
	if(!g_pWintgWorkScheduleMMI)
		return FALSE;
	return g_pWintgWorkScheduleMMI->ProcessMMIOperation(con_id,schd_type,oper_type,sub_type,train); 
}

BOOL   DB_UpdateSchdule_SingleTrain(int con_id, int schd_type,int oper_type,int change_sub_type,const char *train_id, CSingleTrain *pTrain)
{
	CADDATA_SINGLE_TRAIN train;
	CopySingleTrainToCADTRAIN(pTrain,&train);
	if(train.m_nTrainInfo.train_id_count==0 && train_id)
	{
		train.m_nTrainInfo.train_id_count=1;
		strncpy(&train.m_nTrainInfo.train_id[0][0] ,train_id,10);
		strncpy(train.m_strTrainId,train_id,sizeof(train.m_strTrainId)-1);
		train.m_nTrainInfo.begin_sta[0]=0; 
		train.m_nTrainInfo.end_sta[0]=train.GetRecordCount()-1; 
	}
	return DB_UpdateSchdule_SingleTrain(con_id,schd_type,oper_type,change_sub_type,train);
}

BOOL DB_ClearTGDocData(int sch_type)
{
	if(sch_type==WORK_SCHEDULE)
	{
	 if(m_pWintgForm)
	 {
		m_pWintgForm->Redraw(FALSE);
		return TRUE;
	 }
	}
	return FALSE;
}


BOOL  DB_RequestHistorySchedule(int con_id,int shift_id,unsigned long  starttime,unsigned long endtime)
{
	if(!g_pWintgWorkScheduleMMI)
		return FALSE;
	if(!g_pCurrentWorkScheduleData)
		return FALSE;
	/*
	CTG_ScheduleInfo info;
	info.m_nSectionID=con_id;
	info.m_nType = CTG_ScheduleInfo::SCHEDULE_TYPE_HIST;
	info.m_tStartTime = starttime;
	info.m_tEndTime = endtime;
	if(!g_pCurrentWorkScheduleData->WriteSchedule(info,g_pWintgWorkScheduleMMI,TRUE))
		return FALSE;
	else*/
		return TRUE;
}


BOOL  DB_LoadTGInitData(int con_id,int schd_type,int shift_id,unsigned long starttime,unsigned long endtime)
{
	CTG_ScheduleInfo info;
	info.m_nSectionID=con_id;
	info.m_tStartTime = starttime;
	info.m_tEndTime = endtime;

	if(!g_pCurrentWorkScheduleData)
		return FALSE;

	CTG_ScheduleInfo prev_info;
	g_pCurrentWorkScheduleData->GetWorkScheduleInfo(prev_info);

	if(schd_type==WORK_SCHEDULE)
	{
		if(info.m_nSectionID !=prev_info.m_nSectionID) 
			g_pCurrentWorkScheduleData->InitScheduleData(info.m_nSectionID,CTG_ScheduleInfo::SCHEDULE_TYPE_WORK);
		if(info.m_tStartTime!=prev_info.m_tStartTime  || info.m_tEndTime!=prev_info.m_tEndTime)
		{
			g_pCurrentWorkScheduleData->ShiftWorkSchedule(info);   
			g_pCurrentWorkScheduleData->ReloadData(); 
		}

		g_pWintgWorkScheduleMMI->SetMMIWindow(m_pWintgForm->GetSafeHwnd()); 
		
		if(g_pCurrentWorkScheduleData->WriteSchedule(CTG_ScheduleInfo::SCHEDULE_TYPE_WORK ,g_pWintgWorkScheduleMMI,TRUE))
		{
			g_pWintgWorkScheduleMMI->SetCommReady(TRUE); // 允许更新
			return TRUE;
		}
		else
			return FALSE;
	}
	return FALSE;
}

int  GetCurrentTGSectionNo()
{
	if(!m_pWintgDocPtr)
		return -1;
	return m_pWintgDocPtr->m_nCurrentSectionNo; 
}


static BOOL zProcessTrainDelete_Request(void *ptr);
static BOOL	zProcessPMTGInit_Request(void *ptr);

BOOL  DB_ProcessTGRequestMessage(BYTE *ptr,int msglen) // 处理界面产生的发往服务器的消息，用于单机模式
{
	if(!DB_InLocalDBActiveMode()) 
		return FALSE;

	BOOL rs = FALSE;	
	USHORT usMessageType;
	msglen=*(ushort *) ptr;
	ptr+=2;
	usMessageType =  *(ushort *)ptr;
	ptr+=2;
	switch ( usMessageType ) 
	{
	case INIT_PMTG_MESSAGE: //内部消息
		rs = zProcessPMTGInit_Request(ptr);
		break;
	case TRAIN_SCHEDULE_INSERT_INFORM:
		// rs=zProcessTrainScheduleMessage_Request(ptr);
		break;
		/*
	case R_GET_SIDE_BLOCK:
		rs = zProcessSideInhibitData_Request( ptr );
		break;

	case R_GET_ABS_BLOCK:
		rs = zProcessAbsInhibitData_Request( ptr );
		break;

	case R_GET_ABS_SLOWMOVE:
		rs = zProcessAbsSlowMoveData_Request( ptr );
		break;

	case SET_SIDE_BLOCK:
		rs = zSetSideInhibitData_Request(ptr);
		break;

	case SET_ABS_BLOCK:
		rs = zSetAbsInhibitData_Request(ptr);
		break;

	case SET_ABS_SLOWMOVE:
		rs = zSetAbsSlowMoveData_Request(ptr);
		break;
*/
	
	}
	return rs;
}

BOOL  DB_ProcessTGCommSendMessage(BYTE *ptr,int msglen)  //捕获发往CAD,COMM的消息，（不能完全捕获，因为有的消息通过SendTGBMsg发送)
{
	if(!DB_InLocalDBActiveMode()) 
		return FALSE;

	int rs = 0;	
	USHORT usMessageType;
	msglen=*(ushort *) ptr;
	ptr+=2;
	usMessageType =  *(ushort *)ptr;
	ptr+=2;
	switch ( usMessageType ) 
	{
	case TRAIN_SCHEDULE_DELETE_INFORM:
		rs = zProcessTrainDelete_Request(ptr);
		break;
	}
	return TRUE;
}


static ushort nStationNumber;
static ushort nStationList[96];
static ushort nSectionNumber;
static ushort nSectionList[32];

static BOOL	zProcessPMTGInit_Request(void *ptr)
{
	byte *buff;
	int i,theStatus = 1;
	ushort count;
	HWND hwnd;
	ushort nSchdType;
   
	do {
		buff = (byte *)ptr;
		nSchdType = *(ushort *)buff;				buff += 2;

		nSectionNumber = *(ushort *)buff;			buff += 2;
		for(i=0;i < nSectionNumber;i++)
		{
			nSectionList[i] = *(ushort *)buff;		buff += 2;
		}

		nStationNumber = *(ushort *)buff;			buff += 2;
		for(i=0;i < nStationNumber;i++)
		{
			nStationList[i] = *(ushort *)buff;		buff += 2;
		}
		int nStartDate,nStartTime,nEndDate,nEndTime;

		nStartDate = *(int *)buff;		buff += 4;
		nStartTime = *(int *)buff;		buff += 4;
		nEndDate = *(int *)buff;		buff += 4;
		nEndTime = *(int *)buff;		buff += 4;
		int hstyType = *(BYTE *)buff;      buff ++;

		if(nSchdType == WORK_SCHEDULE ||
			nSchdType == NEIGHBER_SCHEDULE ||
			nSchdType == HISTORY_SCHEDULE||
			nSchdType == UNCHANGE_HISTORY_SCHEDULE)
		{
			DB_LoadTGInitData(nSectionList[0],nSchdType,0,long_time(nStartDate,nStartTime),long_time(nEndDate,nEndTime));
			
		} else {//BASIC_SCHEDULE
			if(nStartDate<0)
				nStartDate=m_pWintgDocPtr->m_nShiftDate; 
			DB_LoadTGInitData(nSectionList[0],nSchdType,0,nStartDate,0);
		}
	return TRUE;
	}while(0);
	return FALSE;
}

static BOOL zProcessTrainDelete_Request(void *ptr)
{
	BYTE *msgptr=(BYTE *)ptr;
	int conid=*msgptr++;
	int schd_type=*msgptr++;
    int  count=NettoHostUShort(*(ushort *)msgptr);
	msgptr += 2;

	for (int i=0; i<count; i++)
	{
		unsigned long train_index =  NettoHostUInteger(*(unsigned long *)msgptr) ;
			// 本地数据库处理
		DB_UpdateSchdule_DeleteTrain(conid,schd_type,train_index);
		msgptr += 4;
	}

return TRUE;
		
}


////////////////////// 下面是数据更新操作 ///////////////////////////
