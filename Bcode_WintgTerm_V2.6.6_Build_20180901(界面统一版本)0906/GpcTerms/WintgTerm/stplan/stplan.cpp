#include "stdafx.h"
#include <baseMsg\msgid.h>
#include <baseMsg\clbth.h>
#include <baseMsg\msg_schd.h>
#include "tg_schedule_base.h"
#include "tg_function_config.h"
#include "tg_basicpara.h"
#include "stplan.h"
#include "icommtask.h"
#include "tg_structdef.h"
#include "tg_other_structdef.h"
#include "tgmsg.h"
#include "tgdocument.h"
#include "memshare.h"
#include "schd_tgmmi.h"
#include "TGPspData.h"
extern void  DB_SetPlanSendStatus(long eid,TG_STATION_STATUS::PLAN_SEND_STATUS  st);

#define MAX_STORED_STPLAN_NUM  24
static struct{
	cStPlan *pStPlan;
}g_StPlans[MAX_STORED_STPLAN_NUM];

bool IsTDCommSvr(long nEntityId)
{
	GPCENTITY entityInfo;
	gpcdef.GetEntityInfo(nEntityId,entityInfo);
	if((CString("tdcommsvr")==entityInfo.sName) || CString("tdcomm")==entityInfo.sName)
	{
		return true;
	}
	else if(CString(entityInfo.sExpression)=="tdcommsvr" || nEntityId==GetSpecialEntityID(SERVER_TYPE_TDCOMM))
	{
		return true;
	}
	else
	{
		return false;
	}
}

static BOOL GlobalRegister(cStPlan *p)
{
	int i;
	for(i=0;i<MAX_STORED_STPLAN_NUM;i++)
	{
		if(g_StPlans[i].pStPlan==NULL)
			break;
	}
	if(i>=MAX_STORED_STPLAN_NUM)
		return FALSE;
	g_StPlans[i].pStPlan=p;
	return TRUE;
}

static BOOL GlobalUnRegister(cStPlan *p)
{
	int i;
	for(i=0;i<MAX_STORED_STPLAN_NUM;i++)
	{
		if(g_StPlans[i].pStPlan==p)
			break;
	}
	if(i>=MAX_STORED_STPLAN_NUM)
		return FALSE;
	g_StPlans[i].pStPlan=NULL;
	return TRUE;
}

BOOL cStPlan::GlobalProcessMessage(long eid,  BaseMsg *pmsg)
{
	int i;
	for(i=0;i<MAX_STORED_STPLAN_NUM;i++)
	{
		if(g_StPlans[i].pStPlan)
			g_StPlans[i].pStPlan->ProcessMessage(eid, pmsg);
	}
	return TRUE;
}

cStPlan::cStPlan(CTgDocument *pdoc,CTGBasicParam *pParam)
{
	m_pDoc=pdoc;
	m_pWorkSchedule=m_pDoc->GetScheduleData(CTG_ScheduleInfo::SCHEDULE_TYPE_WORK); 
	m_pBasicSchedule=m_pDoc->GetScheduleData(CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC);
	m_pBasicPara=pParam;
	m_pSendStPlanStatus = new cSendStPlanStatus(GetTGFunctionConfig()->GetMaxPlanReceverCount()*2);

	m_pSendLog = new cStPlanSendingLog();
	GlobalRegister(this);
	InitStationPlanReceiveInfo(get_current_con_id());  // GPC移植
	m_hPlanMMIWindow=NULL;
	m_pTgMMI=NULL;
}

cStPlan::~cStPlan()
{
	delete m_pSendLog;

	delete m_pSendStPlanStatus;
	GlobalUnRegister(this);
}
short cStPlan::get_current_con_id()
{
	CTG_ScheduleInfo schd;
	m_pWorkSchedule->GetScheduleInfo(schd); 
	return schd.m_nSectionID;
}

TIME cStPlan::GetAdjustStartTime()
{
	return CTG_ScheduleInfo::GetStartShiftTime(NULL_TIME);
}

short cStPlan::proc_msg(ENTITY_ID entity, const MsgStPlanTrainRecv &msg)
{
	m_pSendLog->LogMsg(msg);

	return TG_FAILED;
}

void cStPlan::SetPlanFlag(DWORD entity, int station, int index)
{
	if(index>=0 && index<256)
	{
		gRetInfo[index].entity = entity;
		gRetInfo[index].station = station;
		gRetInfo[index].rec = 0;
		gRetInfo[index].sign = 0;
	}
}
void cStPlan::ClearPlanFlag()
{
	memset(gRetInfo, 0, sizeof(gRetInfo));
}

short cStPlan::proc_msg(ENTITY_ID entity, const MsgStPlanRecvALL &msg)
{
	for(int i=0; i<256; i++)
	{
		if(gRetInfo[i].entity ==  msg.st && gRetInfo[i].station == msg.station)
		{
		    if (msg.status == MsgStPlanRecvALL::RECV) 
			{
				if(gRetInfo[i].rec > 0)
					return TG_SUCCESS;
				gRetInfo[i].rec = 200;
			} 
			else if (msg.status == MsgStPlanRecvALL::SIGN) 
			{
				if(gRetInfo[i].sign > 0)
					return TG_SUCCESS;
				gRetInfo[i].sign = 200;
			}
			else if (msg.status == MsgStPlanRecvALL::WRONG) 
			{
				;
			}
			else 
			{
				GpcLogDebugMessage(0,"unknown SetSendStPlanStatus status %d", msg.status);
				return TG_FAILED;
			}

			break;
		}
	}

	m_pSendLog->LogMsg(msg);
   
	GpcLogSysMessage(0,"recv MsgStPlanRecvALL sta %d e%d status %d", msg.station, msg.st, msg.status);

	// set cSendStPlanStatus
	cSendStPlanStatus::STATUS status;
	if (msg.status == MsgStPlanRecvALL::RECV) 
	{
		status = cSendStPlanStatus::PLAN_RECV;
	} 
	else if (msg.status == MsgStPlanRecvALL::SIGN) 
	{
		status = cSendStPlanStatus::PLAN_SIGN;
	}
	else if (msg.status == MsgStPlanRecvALL::WRONG) 
	{
		status = cSendStPlanStatus::PLAN_CONFIG_WRONG;
		GpcLogDebugMessage(0,"stplan recv err config sta %d e%d status %d", msg.station, msg.st, msg.status);
	}
	else 
	{
		GpcLogDebugMessage(0,"unknown SetSendStPlanStatus status %d", msg.status);
		return TG_FAILED;
	}

	if (TG_FAILED == m_pSendStPlanStatus->SetSendStPlanStatus(msg.station, msg.st, status, msg.send_time, msg.recv_time, msg.recver_name)) 
	{
		GpcLogDebugMessage(0,"SetSendStPlanStatus err sta %d entity%d status %d send_time %d recv_time %d", msg.station, msg.st, status, msg.send_time, msg.recv_time);
		return TG_FAILED;
	}

	MsgStPlanStatusAnswer answer;
	if(RequestPlanStatus(msg.station,entity,answer))
		ProcessPlanAnswer(&answer);

	DB_SetPlanSendStatus(entity,TG_STATION_STATUS::SENDOK); 
	return TG_SUCCESS;
}

// PUBLIC STUFF

BOOL cStPlan::RequestPlanStatus(STATION_NO plan_sta,ENTITY_ID eid, MsgStPlanStatusAnswer &answer)
{
	STATION_NO recv_sta=plan_sta;
	answer.dest =eid;

	if (TG_FAILED == m_pSendStPlanStatus->QuerySendStPlanStatus(plan_sta, recv_sta, answer.dest, &(answer.recv_status), &(answer.begin_time), &(answer.end_time),&(answer.send_time), &(answer.recv_time), answer.sender_name, answer.recver_name)) 
	{
		GpcLogDebugMessage(0,"QuerySendStPlanStatus sta %d %d dest %d err", plan_sta, recv_sta, answer.dest);
		return FALSE;
	}
	answer.con_id  = get_current_con_id();
	answer.station = plan_sta;
	return TRUE;
}

short cStPlan::get_meet_store_pos( MsgStPlanInsert &msg, const CTG_TrainRecord *prec, TRAIN_MEET_RELATION flag, int *need_plus_count)
{
	short pos = msg.meet_count;

	*need_plus_count = 1;

	// if cannot contain so many record, just replace into the earliest of same type, or into the earliest of diff type
	if (msg.meet_count+1 > sizeof(msg.meet_train_index)/sizeof(msg.meet_train_index[0])) 
	{
		short sd_index = -1,       dd_index = -1;
		TIME  sd_time = NULL_TIME, dd_time = NULL_TIME;
		if (flag==SAME_DIR_MEET) 
			sd_time = prec->m_data.depart;
		else
			dd_time = prec->m_data.arrive;

		// get the earliest of two kinds
		for(int j=0; j<msg.meet_count; j++) 
		{
			CTG_SingleTrain *meet_train=m_pWorkSchedule->GetSingleTrain(msg.meet_train_index[j]);
			if(!meet_train)
				continue;
			CTG_TrainRecord *meet_rec=meet_train->GetTrainRecord(msg.meet_rec_index[j]);
			if(!meet_rec)
				continue;

			TIME tmp_time = (MsgStPlanInsert::MEET_SAME_DIR == msg.meet_status[j]) ? meet_rec->GetDepartTime() : meet_rec->GetArriveTime();					
			TIME *cmp_ptr = (MsgStPlanInsert::MEET_SAME_DIR == msg.meet_status[j]) ? &sd_time  : &dd_time;
			short *index  = (MsgStPlanInsert::MEET_SAME_DIR == msg.meet_status[j]) ? &sd_index : &dd_index;

			if (NULL_TIME == *cmp_ptr) 
			{
				*index = j;
				*cmp_ptr = tmp_time;
			}

			if (*cmp_ptr > tmp_time) 
			{
				*index = j;
				*cmp_ptr = tmp_time;
			}
		}

		pos = sd_index;
		if ((SAME_DIR_MEET == flag && -1 == sd_index)
			||  (DIFF_DIR_MEET == flag && -1 != dd_index))
			pos = dd_index;				

		if (pos < msg.meet_count && -1 != pos)
			msg.meet_count--;

		*need_plus_count = 0;
	}

	return pos;
}

BOOL  cStPlan::SetMeetRelationInfo(const CTG_SingleTrain &desttrain, MsgStPlanInsert &insertmsg, const CTG_TrainRecord& rec, CTG_TrainPtrGroup &trains_in_range)
{
	insertmsg.meet_count = 0;
	//if (IsMakeDepartRouteByPlan(rec)) 需要和陈宣确认一下是否需要
	//	return FALSE;
		
	for(int i=0;i<trains_in_range.GetTrainCount(); i++)
	{
		CTG_SingleTrain *ptrain=trains_in_range.GetTrainPtr(i);
		if (ptrain->GetTrainIndex() == desttrain.GetTrainIndex()) continue;

		for (int j=0; j<ptrain->GetTrainRecordCount(); j++)
		{
			const CTG_TrainRecord *prec=ptrain->GetTrainRecord_Const(j); 
			if (!prec) // 不可能
				continue; 

			TRAIN_MEET_RELATION rela = desttrain.GetMeetRelation(rec, *ptrain, j);
			if(rela == NO_MEET) 
				continue;

			int need_plus = 0;
			short pos = get_meet_store_pos(insertmsg, prec, rela, &need_plus);

			insertmsg.meet_status[pos] = (DIFF_DIR_MEET == rela) ? MsgStPlanInsert::MEET_DIFF_DIR : MsgStPlanInsert::MEET_SAME_DIR;
			insertmsg.meet_train_index[pos] = ptrain->GetTrainIndex(); 
			insertmsg.meet_rec_index[pos] = prec->GetIndex();
			if (need_plus)
				insertmsg.meet_count++;
		}
	}
	return TRUE;
}

BYTE get_stplan_status(const TG_TrainRecord &rec)
{
	BYTE adjust_status = (rec.adjust_status & 0x0003) | ((rec.adjust_status >> 8) & 0x000C);

	if (TG_IsTechStopTrain(rec))
		adjust_status |= 0x10;

	if (TG_IsElecLocoFlag(rec))
		adjust_status |= 0x20;
	
	// 压前 
	if(TG_IsBeforePressure(rec))
		adjust_status |= 0x40;
	// 压后
	if(TG_IsAfterPressure(rec))
		adjust_status |= 0x80;

	return adjust_status;
}

SIDE_ID get_stplan_sideno(BYTE sideno)
{
	SIDE_ID stplan_sideno=sideno;
	if(1<=stplan_sideno && stplan_sideno<=254)
		return stplan_sideno;
	else
		return -1;
}

BYTE get_stplan_stop_condition(bool bEnable3Function, const TG_TrainRecord &rec)
{
	BYTE stop_condition=rec.stop_condition;
	// 3.0功能是否卡控股道
	if(bEnable3Function)
	{
		if(TG_IsNotCheckTrainSide(rec))
			stop_condition |= 0x01;
		else
			stop_condition &= 0xFE;
	}
	else
	{
		if(GetTGFunctionConfig()->IsCompatibleOldStPlan())
		{
			;// 不修改
		}
		else
		{
			stop_condition &= 0xFE;
		}
	}
	return stop_condition;
}

BYTE get_stplan_flag(bool bEnable3Function, const TG_TrainRecord &rec)
{
	BYTE flag=(rec.flag & 0x001F);
	// 3.0功能是否卡控出入口
	if(bEnable3Function)
	{
		if(TG_IsNotCheckTrainExitEntry(rec))
			flag |= 0x80;
		else
			flag &= 0x7F;
	}
	else 
	{
		if(GetTGFunctionConfig()->IsCompatibleOldStPlan())
		{
			;// 不修改
		}
		else
		{
			flag &= 0x7F;
		}
	}
	return flag;
}

int  GetNoteChineseChar(CString strNotes, int len)
{
	BYTE* logbuf = new BYTE[strNotes.GetLength()+1];
	if(NULL == logbuf)
		return len;

	memset(logbuf, 0, strNotes.GetLength()+1);
	strncpy((char*)logbuf,  strNotes,  strNotes.GetLength());
	logbuf[strNotes.GetLength()]=0;

	int flag=0;
	for(int i=0; i<len&&i<strNotes.GetLength(); i++)
	{
		BYTE ch = logbuf[i];
		if(ch>=128) // 汉字
		{
			flag++;
		}
	}

	delete []logbuf;
	logbuf=NULL;
	// 完整汉字
	if(flag%2 == 0)
		return len;
	else
		return len-1;
}

bool IsNeedSendByPlanEntryExit(ENTRY_ID plan_entry, ENTRY_ID plan_exit, ENTRY_ID entry, ENTRY_ID exit)
{
	if(!IS_VALID_ENTRY(plan_entry))
		plan_entry=0;
	if(!IS_VALID_ENTRY(plan_exit))
		plan_exit=0;
	if(plan_entry == entry && plan_exit == exit)
	{
		return true;
	}
    return false;
}

short cStPlan::OrganizeMsg(MsgStPlanInsert &msg, STATION_NO recv_sta, bool bEnable3Function, const CTG_SingleTrain *singletrain, BYTE rec_index,  short prev_count, short next_count, CTG_TrainPtrGroup &trains_in_range, CString &report_early, DWORD dest)
{
	if (prev_count > sizeof(msg.prev_sta)/sizeof(msg.prev_sta[0])) 
	{
		prev_count = sizeof(msg.prev_sta)/sizeof(msg.prev_sta[0]);
	}
	if (next_count > sizeof(msg.next_sta)/sizeof(msg.next_sta[0])) 
	{
		next_count = sizeof(msg.next_sta)/sizeof(msg.next_sta[0]);
	}

	const CTG_TrainInfo *pTrainInfo=singletrain->GetTrainInfo_Const();
	const CTG_TrainRecord* precord=singletrain->GetTrainRecord_Const(rec_index);
	if(!precord || !pTrainInfo)
	{
		return TG_FAILED;
	}
	
	bool bChngeTrainNo=true;
	if(GetTGFunctionConfig()->IsDisableAutoTrainNo())
	{
		bChngeTrainNo=false;
	}

	if(precord->IsActualDepartTimeRecord())
	{
		CString logstr;
		logstr.Format("阶段计划: %s 在 %d站 发点为实际点",  
					singletrain->GetTrainID(), precord->GetStationNo());

		TIME ltime = time(NULL);
		SaveLogMessageStr("stplan",ltime,logstr);
	}

	if(precord->IsActualArriveTimeRecord())
	{
		CString logstr;
		logstr.Format("阶段计划: %s 在 %d站 到点为实际点",  
					singletrain->GetTrainID(), precord->GetStationNo());
		
		
		TIME ltime = time(NULL);
		SaveLogMessageStr("stplan",ltime,logstr);
	}

	int rec_count=singletrain->GetTrainRecordCount();

	msg.train_index = singletrain->GetTrainIndex();
	msg.rec_index   = precord->GetIndex();
	msg.station     = precord->m_data.station;
	msg.arrive      = precord->m_data.arrive;
	msg.depart      = precord->m_data.depart;
	msg.plan_arrive = precord->m_data.plan_arrive;
	msg.plan_depart = precord->m_data.plan_depart;

	long nEarlyTime=GetTGFunctionConfig()->GetStPlanReportEarlyTimeRange()*60;
	if((nEarlyTime>0) && (singletrain->IsPassengerTrain()))
	{
		if (precord->m_data.arrive!=precord->m_data.depart)
		{
			long dlt=precord->m_data.plan_depart;
			dlt = dlt - precord->m_data.depart;
			if (dlt>nEarlyTime)
			{
				CString text;
				text.Format("%s次列车在%s(%d)站计划发车时间早于图定发点!\r\n", 
					singletrain->GetTrainID(), gpcdef.GetStationName(precord->GetStationNo()),precord->GetStationNo());

				if (!strstr(report_early, text))
					report_early+=text;
			}
		}
	}
	
	msg.entry       = (char)precord->m_data.entry;
	msg.exit        = (char)precord->m_data.exit;
	msg.arrive_side = get_stplan_sideno(precord->m_data.arrive_side);
	msg.depart_side = get_stplan_sideno(precord->m_data.depart_side);
	msg.min_stop_time = 0;  // precord->m_data.min_stop_time; OPMS发送过来的营业时分不正确

	if((msg.rec_index+1==singletrain->GetTrainRecordCount()) && precord->IsTerminalRec())
	{
		if(bChngeTrainNo)
		{
			TG_CopyTrainId(msg.arrive_train_id, precord->m_data.arrive_train_id);
			TG_CopyTrainId(msg.depart_train_id, precord->m_data.depart_train_id);
		}
		else
		{
			TG_CopyTrainId(msg.arrive_train_id, precord->m_data.arrive_train_id);
			TG_CopyTrainId(msg.depart_train_id, precord->m_data.arrive_train_id);
		}
	}
	else
	{
		TG_CopyTrainId(msg.arrive_train_id, precord->m_data.arrive_train_id);
		TG_CopyTrainId(msg.depart_train_id, precord->m_data.depart_train_id);
	}

	const char *pshuaigua_str=precord->GetStationText(STATION_TXT_INFO_TYPE_SHUAIGUA);
	const char *pimport_str=GetTrainShortImportantInfo(precord, pTrainInfo->m_data.space_rank); 

	CString strNotes = "";
	if(pimport_str)
		strNotes += pimport_str; 
	if(pshuaigua_str)
		strNotes += pshuaigua_str; 
		
	// 计划说明 G09 
	const CTG_TrainRecord* nxtrecord=singletrain->GetTrainRecord_Const(rec_index+1);
	const CTG_TrainRecord* nxtnxtrecord=singletrain->GetTrainRecord_Const(rec_index+2);
	if(NULL != nxtrecord && NULL != nxtnxtrecord)
	{
		if(GetTGFunctionConfig()->IsSendPlanTip(msg.con_id, precord->GetStationNo(), nxtrecord->GetStationNo()))
		{
			CString staName = gpcdef.GetStationName(nxtnxtrecord->GetStationNo());
			if(!staName.IsEmpty())
			{
				strNotes += staName+"@";
			}
		}
	}

	memset(msg.notes, 0, sizeof(msg.notes));	
	if(strNotes.GetLength() >= sizeof(msg.notes))
	{
		GpcLogSysMessage(0, "阶段计划: 实际记事(%d >= 64) %s", strNotes.GetLength(), strNotes);
		int len = GetNoteChineseChar(strNotes, 63);
		strNotes = strNotes.Left(len);
		GpcLogSysMessage(0, "阶段计划: 下发记事(%d <= 63) %s", strNotes.GetLength(), strNotes);
	}

	if(strNotes.GetLength()>0)
		strncpy(msg.notes, strNotes,  strNotes.GetLength());

	msg.early_depart_time  = 0x8000; //precord->m_data.early_depart_time; 不使用该功能
	msg.status             = get_stplan_status(precord->m_data);
	
	msg.stop_condition 	   = get_stplan_stop_condition(bEnable3Function, precord->m_data);
	msg.run_fast_slow_time = precord->m_data.run_fast_slow_time;

	msg.flag = get_stplan_flag(bEnable3Function, precord->m_data);

	short i;
	msg.prev_sta_count=prev_count>rec_index?rec_index:prev_count;
	for(i=0; i<msg.prev_sta_count; i++)
	{
		const CTG_TrainRecord  *prev_record=singletrain->GetTrainRecord_Const(rec_index-i-1);
		if(!precord) continue;

		msg.prev_sta[i]    = prev_record->m_data.station;
		msg.prev_entry[i]  = prev_record->m_data.entry;
		msg.prev_exit[i]   = prev_record->m_data.exit;
		if(GetTGFunctionConfig()->IsConsiderStplanSide())
			msg.prev_side[i]   = get_stplan_sideno(prev_record->m_data.depart_side);
		else
			msg.prev_side[i]   = prev_record->m_data.depart_side;

		msg.prev_arrive[i] = prev_record->m_data.arrive;
		msg.prev_depart[i] = prev_record->m_data.depart;
		msg.prev_status[i] = get_stplan_status(prev_record->m_data);
		msg.prev_flag[i]   = get_stplan_flag(false,prev_record->m_data);
	}

	msg.next_sta_count=(next_count+rec_index>=rec_count)?(rec_count-rec_index-1):next_count;
	for(i=0; i<msg.next_sta_count; i++)
	{
		const CTG_TrainRecord  *next_record=singletrain->GetTrainRecord_Const(rec_index+i+1);
		if(!precord) continue;

		msg.next_sta[i]    = next_record->m_data.station;
		msg.next_entry[i]  = next_record->m_data.entry;
		msg.next_exit[i]   = next_record->m_data.exit;
		if(GetTGFunctionConfig()->IsConsiderStplanSide())
			msg.next_side[i]   = get_stplan_sideno(next_record->m_data.arrive_side);
		else
			msg.next_side[i]   = next_record->m_data.arrive_side;
		msg.next_arrive[i] = next_record->m_data.arrive;
		msg.next_depart[i] = next_record->m_data.depart;
		msg.next_status[i] = get_stplan_status(next_record->m_data);
		msg.next_flag[i]   = get_stplan_flag(false,next_record->m_data);
	}

	msg.engine_count=0;
	for(i=0; i<singletrain->GetEngineCount(); i++)
	{
		const CTG_TrainEngineInfo *pengine=singletrain->GetEngine_Const(i);
		if(!pengine)
			break;
		if(pengine->m_data.attach_sta> rec_index)
			continue;
		else if (pengine->m_data.unattach_sta >0 && pengine->m_data.unattach_sta < rec_index)
			continue;

		TG_CopyTrainId(msg.engine_id[msg.engine_count], pengine->m_data.engine_id);
		msg.engine_type[msg.engine_count] = pengine->GetEngineType(); 
		msg.engine_count ++;

		if (msg.engine_count >= sizeof(msg.engine_type)/sizeof(msg.engine_type[0]))
			break;
	}

	const TG_TrainInfo* coninfo = &singletrain->GetTrainInfo_Const()->m_data;
	msg.type = coninfo->type;
	
	msg.run_type = coninfo->run_type;
	if(msg.type == 255) // 未知列车
	{
		if(GetTGFunctionConfig()->IsEnableUnknowTrainStPlanRunType())
			msg.run_type = 255;

		if(GetTGFunctionConfig()->IsLocalLogSendStPlan())
		{
			TIME ltime = time(NULL);
			CString logstr;
			logstr.Format("未知列车:%s type %d run_type %d",	singletrain->GetTrainID(), msg.type, msg.run_type);
			SaveLogMessageStr("stplan",ltime,logstr);
		}
	}

	msg.space_rank=0;
	BYTE nArriveSpace=precord->GetArriveSpace();
	BYTE nDepartSpace=precord->GetDepartSpace();
	
	if(GetTGFunctionConfig()->IsStplanSpaceRankByStation())
		msg.space_rank = (nArriveSpace&0x07)+((nDepartSpace&0x07)<<3);
	else
	{
		if(nArriveSpace>0 && nDepartSpace>0)
		{
			msg.space_rank = (nArriveSpace&0x07)+((nDepartSpace&0x07)<<3);
		}
		else if(nArriveSpace>0 && nDepartSpace<=0)
		{
			nDepartSpace=coninfo->space_rank;
			if(nDepartSpace<=0)
				nDepartSpace=nArriveSpace;
			
			msg.space_rank = (nArriveSpace&0x07)+((nDepartSpace&0x07)<<3);
			if(nDepartSpace==1)
				GpcLogSysMessage(0, "列车 %s  发车超级超限(nDepartSpace<=0 && nArriveSpace>0)",  singletrain->GetTrainID());
			else if(nDepartSpace>=2)
				GpcLogSysMessage(0, "列车 %s  发车%d级超限(nDepartSpace<=0 && nArriveSpace>0)",  singletrain->GetTrainID(), nDepartSpace-1);
		}
		else if(nArriveSpace<=0 && nDepartSpace>0)
		{
			nArriveSpace=coninfo->space_rank;
			if(nArriveSpace<=0)
				nArriveSpace=nDepartSpace;
			
			msg.space_rank = (nArriveSpace&0x07)+((nDepartSpace&0x07)<<3);
			if(nArriveSpace==1)
				GpcLogSysMessage(0, "列车%s  接车超级超限(nArriveSpace<=0 && nDepartSpace>0)",  singletrain->GetTrainID());
			else if(nArriveSpace>=2)
				GpcLogSysMessage(0, "列车 %s  接车%d级超限(nArriveSpace<=0 && nDepartSpace>0)",  singletrain->GetTrainID(), nArriveSpace-1);
		}
		else
		{
			BYTE nSpace=coninfo->space_rank;
			if(nSpace>0)
				msg.space_rank = (nSpace&0x07)+((nSpace&0x07)<<3);

			if(nSpace==1)
				GpcLogSysMessage(0, "列车 %s  接车发车超级超限",  singletrain->GetTrainID());
			else if(nSpace>=2)
				GpcLogSysMessage(0, "列车 %s  接车发车%d级超限",  singletrain->GetTrainID(), nSpace-1);
		}
	}

	if(msg.space_rank>0)
		GpcLogSysMessage(0, "列车 %s  超限组合值(bit0-bit2 为接车 bit3-bit5 为发车) %X", singletrain->GetTrainID(), msg.space_rank);

	msg.length_rank = coninfo->length_rank;
	msg.super_rank = coninfo->super_rank;  // precord->rank NA?????
	msg.military_rank = coninfo->military_rank;
	msg.disconn_count = 0;
	msg.conn_count = 0;
	msg.train_length = coninfo->train_length;
	msg.train_weight = coninfo->train_weight;

	SetMeetRelationInfo(*singletrain, msg, *precord, trains_in_range);//btime, etime+30*60);  // 在此处设置MEET INFO

	if (msg.station != recv_sta)
	{
		msg.station = recv_sta;
	}

	//TDCS的STPC 不能接受含,的字符
	for(i=0; i<strlen(msg.notes); i++)
		if (msg.notes[i] == ',')  msg.notes[i] = ';';

	for(i=0; i<strlen(msg.arrive_train_id); i++)
		if (msg.arrive_train_id[i] == ',')  msg.arrive_train_id[i] = ';';

	for(i=0; i<strlen(msg.depart_train_id); i++)
		if (msg.depart_train_id[i] == ',')  msg.depart_train_id[i] = ';';
	// 珠三角城际 下计划新需求
	if(GetTGFunctionConfig()->IsSupportInterCityStPlan())
	{
		CString logstr;
		CString strCZH=pTrainInfo->m_data.shift_string;
		if(msg.rec_index==0)
		{
			if(precord->IsStartRec())
			{
				long side_utility=0;
				long arrive=GetBeforeTrainArrive(strCZH, precord->m_data.station, msg.train_index,side_utility);
				if(arrive>0)
				{
					logstr.Format("列车 %s 是始发车, 折返标志:%x", singletrain->GetTrainID(),(side_utility&0x6));
					SaveLogMessageStr("stplan", 0, logstr);
					if(IsNewReturn(side_utility))
					{
						msg.flag &= ~0x60;
						msg.arrive = arrive;
						msg.stop_condition &= ~0x20;
						if(IsStationOutReturn(side_utility))
						{
							msg.flag |= 0x40;
							logstr.Format("列车 %s 计划设置为:站后折返", singletrain->GetTrainID());
						}
						else
						{
							msg.flag |= 0x20;
							logstr.Format("列车 %s 计划设置为:原地折返", singletrain->GetTrainID());
						}
						SaveLogMessageStr("stplan", 0, logstr);
					}
					else
					{
						logstr.Format("前车无折返, 计划不设置折返标志");
						SaveLogMessageStr("stplan", 0, logstr);
					}
				}
				else
				{
					logstr.Format("找不到前续终到车,计划不设置折返标志");
					SaveLogMessageStr("stplan", 0, logstr);
				}
			}
			else
			{
				logstr.Format("列车 %s 是接入车, 计划不设置折返标志", singletrain->GetTrainID());
				SaveLogMessageStr("stplan", 0, logstr);
			}
		}


		if(msg.rec_index+1==singletrain->GetTrainRecordCount())
		{
			if(precord->IsTerminalRec())
			{
				long side_utility=pTrainInfo->m_data.side_utility;
				logstr.Format("列车 %s 是终到车, 折返标志:%x", singletrain->GetTrainID(),(side_utility&0x6));
				SaveLogMessageStr("stplan", 0, logstr);
				if(IsNewReturn(side_utility))
				{
					CString logstr;

					CString trainid;
					long depart=GetNextTrainDepart(strCZH, precord->m_data.station, msg.train_index,trainid);
					if(depart>0)
					{
						TG_CopyTrainId(msg.depart_train_id, trainid);
						msg.flag &= ~0x60;
						msg.depart = depart;
						msg.stop_condition &= ~0x40;
						if(IsStationOutReturn(side_utility))
						{
							msg.flag |= 0x40;
							logstr.Format("列车 %s 计划设置为:站后折返", singletrain->GetTrainID());
						}
						else
						{
							msg.flag |= 0x20;
							logstr.Format("列车 %s 计划设置为:原地折返", singletrain->GetTrainID());
						}
						SaveLogMessageStr("stplan", 0, logstr);
					}
					else
					{	
						logstr.Format("找不到后续始发车,计划不设置折返标志");
						SaveLogMessageStr("stplan", 0, logstr);
					}
				}
				else
				{
					logstr.Format("无折返, 计划不设置折返标志");
					SaveLogMessageStr("stplan", 0, logstr);
				}
			}
			else
			{
				logstr.Format("列车 %s 是交出车, 计划不设置折返标志", singletrain->GetTrainID());
				SaveLogMessageStr("stplan", 0, logstr);
			}
		}
	}
	// 在不改变运行线数据的情况下, 下发车站自律机和车务终端临时修改出发车次
	else if(!GetTGFunctionConfig()->IsChangeTrainID()) 
	{
		if((dest != GetSpecialEntityID(SERVER_TYPE_TDCOMM)) && bChngeTrainNo)
		{
			if(msg.rec_index+1==singletrain->GetTrainRecordCount())
			{
				if(GetTGFunctionConfig()->IsEnableNewCzh())
				{
					GpcLogSysMessage(0, "下发车站 %d 车次 %s 计划", msg.station, msg.arrive_train_id);

					if(precord->IsTerminalRec())
					{	
						TRAIN_NO trainid;
						if(GetStartTrainDepartNo(precord->m_data.station, msg.train_index,trainid))
						{
							TG_CopyTrainId(msg.depart_train_id, trainid);
						}
					}
					else
					{
						GpcLogSysMessage(0, "下发车站 %d 车次 %s 计划, 但是该车在本站不是终到, 不设置折返车次.", msg.station, msg.arrive_train_id);
					}
				}
				else if(GetTGFunctionConfig()->IsEnableCzh())
				{
					CString strCZH=pTrainInfo->m_data.shift_string;
					GpcLogSysMessage(0, "下发车站 %d 车次 %s 车组号 %s 计划", msg.station, msg.arrive_train_id, strCZH);

					if(precord->IsTerminalRec() && strCZH!="")
					{	
						TRAIN_NO trainid;
						if(GetStartTrainDepartNo(precord->m_data.station, msg.train_index,trainid))
						{
							TG_CopyTrainId(msg.depart_train_id, trainid);
						}
					}
					else
					{
						GpcLogSysMessage(0, "下发车站 %d 车次 %s 没有车组号 或者 不是终到车, 不设置折返车次.", msg.station, msg.arrive_train_id);
					}
				}
			}
		}
	}		
	return TG_SUCCESS;
}

BOOL  cStPlan::SendPlan(const MsgStPlanSendOrder &order, bool bSecretSendToSta, std::vector<stplannotice>& predict_notice, CString &report_early, bool bEnable3Function, int match)
{
	stplannotice notice;
	// 台间站阶段计划处理
	std::vector<NeibStation> neibStaPlan;
	GetTGFunctionConfig()->GetNeibStations(order.con_id, order.station, neibStaPlan);

	GpcLogSysMessage(0, "\n---------下发车站 %d 阶段计划--------", order.station);

	for(int i=0; i<neibStaPlan.size(); i++)
	{
		GpcLogSysMessage(0, "台间站配置: con_id %d nPreStaNo %d nNeibStaNo %d nNextStaNo %d nNextNextStaNo %d", 
			order.con_id,
			neibStaPlan[i].nPreStation, 
			neibStaPlan[i].nNeibStaNo,
			neibStaPlan[i].nStationNo, 
			neibStaPlan[i].nNextNextStationNo);
	}
	
	if(match>0)
		GpcLogTrainMessage(0,0,"按照出入口条件发送阶段计划时section=%d station=%d 能够处理始发和终到列车", order.con_id, order.station);

	GpcLogTrainMessage(0,0,"向entity=%d 发送如下出入口条件阶段计划(entry=-1表示不限制入口 exit=-1表示不限制出口)",  order.st);
	for(int k=0; k<order.count; k++)
	{
		CString tmp="";
		tmp.Format("entry=%d exit=%d", (char)order.entry[k], (char)order.exit[k]);
		GpcLogTrainMessage(0,0,"允许下达 %s 计划", tmp);
	}
	
	MsgStPlanStatusAnswer answer;
	ENTITY_ID entity=GetLocalEntityID();

	// in order to support dmis-format convert, delete seconds from send_time 
	TIME send_time = order.send_time-order.send_time%60;

	TIME begin_time = order.begin_time;
	TIME end_time   = order.end_time;

	m_pSendLog->LogMsg(order, entity);

	if (!m_pBasicPara->VerifyStationNumber(order.station)) 
	{
		GpcLogDebugMessage(0,"send stplan err sta %d to e%d", order.station, order.st);
		return FALSE;
	}
	if ((ULONG)(end_time - begin_time) > 5*3600 || begin_time < GetAdjustStartTime() - 3*3600) 
	{
		GpcLogDebugMessage(0,"send stplan sta %d invalid time %d -> %d", order.station, begin_time, end_time);
		GpcLogDebugMessage(0,"send stplan sendtime %d AdjustStartTime %d cur %d", send_time, GetAdjustStartTime(), GetCurrentTime());
		return FALSE;
	}
	if (order.prev_sta_count >= 8 || order.next_sta_count >= 8) 
	{
		GpcLogDebugMessage(0,"send stplan sta %d to e%d err prev_sta_count %d next_sta_count %d", order.station, order.st, order.prev_sta_count, order.next_sta_count);
		return FALSE;
	}

	GpcLogSysMessage(0,"recv StPlanSendOrder e%d sta %d sendtime %d %d->%d", order.st, order.station, send_time, begin_time, end_time);

	Archive ar;
	ENTITY_ID dest = order.st;
	STATION_NO plan_sta = order.station;
	STATION_NO recv_sta = order.station_dmis;

	answer.con_id     = order.con_id;
	answer.begin_time = begin_time;
	answer.end_time   = end_time;
	answer.send_time  = send_time;
	answer.recv_time  = NULL_TIME;
	memcpy(answer.sender_name, order.sender_name, sizeof(USERNAME));
	memset(answer.recver_name, 0x00, sizeof(USERNAME));
	answer.station = plan_sta;
	answer.dest = dest;

	// 对象死活不作为必要的发送条件
	{
		// 曾向该车站下过计划记录, 应该发删除计划
		CTG_TrainPtrGroup  trains_delete;
		// 曾向该车站下过计划记录, 应该发更新计划
		CTG_TrainPtrGroup  trains_update; 
		// 存放当前时间前1小时----当前时间后18小时内的车
		CTG_TrainPtrGroup  trains_in_range, trains_in_ranges;
		m_pWorkSchedule->FindTrainsForStplan(trains_in_range, plan_sta, begin_time-1*3600, begin_time+18*3600);
		m_pWorkSchedule->FindTrainsForStplan(trains_in_ranges, plan_sta, begin_time-1*1800, begin_time+6*3600);
		
		trains_delete.Clear();
		trains_update.Clear();
		// 查找不在下计划时间段内,但是需要删除和更新的车
		for(int iter=0;iter<trains_in_range.GetTrainCount();iter++)
		{
			const CTG_SingleTrain *singletrain=trains_in_range.GetTrainPtr(iter);
			const TG_TrainInfo *info   = &singletrain->GetTrainInfo_Const()->m_data;

			for (int j=0; j<singletrain->GetTrainRecordCountWithDeleted()/*GetTrainRecordCount()*/; j++)
			{
				const CTG_TrainRecord *prec = singletrain->GetTrainRecord_Const(j);
				if (NULL == prec || prec->GetStationNo() != plan_sta) continue;

				const TG_TrainRecord &currec=prec->m_data;				

				if (!prec->IfInTimeScope(order.begin_time, order.end_time))
				{
					if(currec.send_plan_time>0)
					{
						if(NULL_TIME != currec.arrive && NULL_TIME != currec.depart)
						{
							if ((end_time < currec.arrive) && (end_time < currec.depart))
							{
								trains_delete.AddTrainPtrNotOrder(trains_in_range.GetTrainPtr(iter));
								continue;
							}
							if ((currec.arrive < begin_time) && (end_time < currec.depart))
							{
								trains_update.AddTrainPtrNotOrder(trains_in_range.GetTrainPtr(iter));
								continue;
							}
							if((currec.arrive < begin_time) && (currec.depart < begin_time))
							{
								if (!prec->IsActualArriveTimeRecord() && currec.arrive > begin_time-3600)
								{
									int realRecCount=singletrain->GetTrainRecordCount();
									int allRecCount=singletrain->GetTrainRecordCountWithDeleted();
									if(j+1 == realRecCount/*最后一个点*/ && j+1 < allRecCount/*后续有删除点*/)
									{
										trains_update.AddTrainPtrNotOrder(trains_in_range.GetTrainPtr(iter));
										continue;
									}

									// 折返车
									const CTG_TrainRecord *pnext = singletrain->GetTrainRecord_Const(j+1);
									if(pnext && plan_sta == pnext->GetStationNo())
									{
										trains_update.AddTrainPtrNotOrder(trains_in_range.GetTrainPtr(iter));
										continue;
									}
								}
							}
						}
					}
					continue;
				}
			}

			// 曾经下过计划, 后来拖出当前时间3小时范围外, 没有再下计划就被删除的车
			if(0 == singletrain->GetTrainRecordCount())
			{
				for (int j=0; j<singletrain->GetTrainRecordCountWithDeleted(); j++)
				{
					const CTG_TrainRecord *prec = singletrain->GetTrainRecord_Const(j);
					if (NULL == prec || prec->GetStationNo() != plan_sta) continue;

					const TG_TrainRecord &currec=prec->m_data;				

					if (!prec->IfInTimeScope(order.begin_time, order.end_time))
					{
						if(currec.send_plan_time>0)
						{
							trains_delete.AddTrainPtrNotOrder(trains_in_range.GetTrainPtr(iter));
							
							continue;
						}
					}
				}
			}
		}  //while(ALL trains)

		if (begin_time != order.begin_time || end_time != order.end_time)
			GpcLogSysMessage(0,"send stplan send time scope is expand to %d %d", begin_time, end_time);

		// send msg init
		MsgStPlanSendInit msginit;
		msginit.station = recv_sta;
		msginit.con_id  = get_current_con_id();
		msginit.gpc     = entity;
		memcpy(msginit.sender_name, order.sender_name, sizeof(USERNAME));
		msginit.send_time = send_time;
		msginit.send_echo = ((MsgStPlanSendInit::StPlanEchoMode)(int)order.st_echo);
		ar << msginit;

		MsgStPlanInsert insertmsg; // used to get data
		MsgStPlanDelete delmsg;

		insertmsg.con_id = get_current_con_id();
		insertmsg.gpc    = entity;
		delmsg.con_id =  get_current_con_id();
		delmsg.gpc    = entity;
		delmsg.station = recv_sta;

		// 存放存放begin_time<=----<=end_time内的车
		trains_in_range.Clear();
		m_pWorkSchedule->FindTrainsForStplan(trains_in_range, plan_sta, begin_time, end_time);

		short del_count=0, add_count=0;
		// 发送删除计划	
		for(int iter=0;iter<trains_in_range.GetTrainCount();iter++)
		{
			const CTG_SingleTrain *singletrain=trains_in_range.GetTrainPtr(iter);
			const TG_TrainInfo *info = &singletrain->GetTrainInfo_Const()->m_data;

			for (int j=0; j<singletrain->GetTrainRecordCountWithDeleted(); j++)
			{
				const CTG_TrainRecord *prec = singletrain->GetTrainRecord_Const(j);
				if (NULL == prec || prec->GetStationNo() != plan_sta) 
					continue;

				const TG_TrainRecord &currec=prec->m_data;
				int flag = 1;
				if (0 == prec->GetDepartTime() || (TIME)-1 == prec->GetDepartTime())
				{
					if (prec->GetArriveTime() > end_time) 
						flag = 0;
				}
				else if (0 == prec->GetArriveTime() || (TIME)-1 == prec->GetArriveTime())
				{
					if (prec->GetDepartTime() > end_time) 
						flag = 0;
				}
				else
				{
					if (prec->GetArriveTime() > end_time && prec->GetDepartTime() > end_time) 
						flag = 0;
				}

				if (  ( !flag /*在发送时间段后*/ || info->delete_flag <= j/* 删除信息*/)
					&& (currec.send_plan_time>0))  // 只下曾经发过计划的删除计划
				{
					delmsg.train_index = singletrain->GetTrainIndex();
					if (info->delete_flag <= j)
						delmsg.rec_index = prec->m_data.late_reason; // 删除后的记录INDEX暂存此处  
					else
						delmsg.rec_index = prec->GetIndex();
					memcpy(delmsg.arrive_train_id, currec.arrive_train_id, sizeof(delmsg.arrive_train_id));
					memcpy(delmsg.depart_train_id, currec.depart_train_id, sizeof(delmsg.depart_train_id));
					delmsg.arrive  = currec.arrive;
					delmsg.depart  = currec.depart;
					delmsg.station = recv_sta;

					if (!CheckSendMessage(dest, ar, delmsg)) 
					{
						GpcLogDebugMessage(0,"send stplan to e%d err", dest);
						return FALSE;
					}

					m_pSendLog->LogMsg(delmsg, dest);

					// 事件处理4: 下发阶段计划(删除计划)
					GPCENTITY eventEntity;
					if(gpcdef.GetEntityInfo(136,0,0,eventEntity))
					{
						MsgEventData data;
						data.nSrcEntityID = GetLocalEntityID();
						data.tmEventTime = time(NULL);
						data.nEventRank = NORMAL_EVENT;

						data.nEventID =  EVENT_GPC_SEND_STPLAN;
						data.SetKey(KeyStationId, recv_sta);
						data.SetKey(KeyStationName, gpcdef.GetStationName(recv_sta));
						data.SetKey(KeyArrTrainNo, currec.arrive_train_id);
						data.SetKey(KeyDptTrainNo, currec.depart_train_id);
						data.SetKey(KeyTrainIndex, singletrain->GetTrainIndex());
						data.SetKey(KeyArriveTime, currec.arrive);
						data.SetKey(KeyDepartTime, currec.depart);
						data.SetKey(KeyBusinessTime, currec.min_stop_time);					
						SendBMsg(&data, eventEntity.nEntityId);
					}

					ar << delmsg;
					del_count++;

					notice.flag=1;
					notice.train_index=delmsg.train_index;
					notice.rec_index=delmsg.rec_index;

					predict_notice.push_back(notice);
				}
			}				
		}
		
		// 发送删除计划(运行线,曾下过计划,但后来移出下计划时间范围)	
		for(int iter=0;iter<trains_delete.GetTrainCount();iter++)
		{
			const CTG_SingleTrain *singletrain=trains_delete.GetTrainPtr(iter);
			const TG_TrainInfo *info = &singletrain->GetTrainInfo_Const()->m_data;

			for (int j=0; j<singletrain->GetTrainRecordCountWithDeleted(); j++)
			{
				const CTG_TrainRecord *prec = singletrain->GetTrainRecord_Const(j);
				if (NULL == prec || prec->GetStationNo() != plan_sta) 
					continue;

				const TG_TrainRecord &currec=prec->m_data;
				
				int flag = 1;
				if (0 == prec->GetDepartTime() || (TIME)-1 == prec->GetDepartTime())
				{
					if (prec->GetArriveTime() > end_time) 
						flag = 0;
				}
				else if (0 == prec->GetArriveTime() || (TIME)-1 == prec->GetArriveTime())
				{
					if (prec->GetDepartTime() > end_time) 
						flag = 0;
				}
				else
				{
					if (prec->GetArriveTime() > end_time && prec->GetDepartTime() > end_time) 
						flag = 0;
				}

				if (  ( !flag /*在发送时间段后*/ || info->delete_flag <= j/* 删除信息*/)
					&& (currec.send_plan_time>0))  // 只下曾经发过计划的删除计划
				{
					delmsg.train_index = singletrain->GetTrainIndex();
					if (info->delete_flag <= j)
						delmsg.rec_index = prec->m_data.late_reason; // 删除后的记录INDEX暂存此处  
					else
						delmsg.rec_index = prec->GetIndex();
					memcpy(delmsg.arrive_train_id, currec.arrive_train_id, sizeof(delmsg.arrive_train_id));
					memcpy(delmsg.depart_train_id, currec.depart_train_id, sizeof(delmsg.depart_train_id));
					delmsg.arrive  = currec.arrive;
					delmsg.depart  = currec.depart;
					delmsg.station = recv_sta;

					if (!CheckSendMessage(dest, ar, delmsg)) 
					{
						GpcLogDebugMessage(0,"send stplan to e%d err", dest);
						return FALSE;
					}

					GpcLogSysMessage(0,"send expand train delete stplan arrive %d depart %d", delmsg.arrive, delmsg.depart);
					m_pSendLog->LogMsg(delmsg, dest);

					// 事件处理4: 下发阶段计划(删除计划)
					GPCENTITY eventEntity;
					if(gpcdef.GetEntityInfo(136,0,0,eventEntity))
					{
						MsgEventData data;
						data.nSrcEntityID = GetLocalEntityID();
						data.tmEventTime = time(NULL);
						data.nEventRank = NORMAL_EVENT;

						data.nEventID =  EVENT_GPC_SEND_STPLAN;
						data.SetKey(KeyStationId, recv_sta);
						data.SetKey(KeyStationName, gpcdef.GetStationName(recv_sta));
						data.SetKey(KeyArrTrainNo, currec.arrive_train_id);
						data.SetKey(KeyDptTrainNo, currec.depart_train_id);
						data.SetKey(KeyTrainIndex, singletrain->GetTrainIndex());
						data.SetKey(KeyArriveTime, currec.arrive);
						data.SetKey(KeyDepartTime, currec.depart);
						data.SetKey(KeyBusinessTime, currec.min_stop_time);
						SendBMsg(&data, eventEntity.nEntityId);
					}

					ar << delmsg;
					del_count++;

					notice.flag=1;
					notice.train_index=delmsg.train_index;
					notice.rec_index=delmsg.rec_index;

					predict_notice.push_back(notice);
				}
			}				
		}

		// 发送增加信息
		for(int iter=0;iter<trains_in_range.GetTrainCount();iter++)
		{
			const CTG_SingleTrain *singletrain=trains_in_range.GetTrainPtr(iter);
			const TG_TrainInfo *info = &singletrain->GetTrainInfo_Const()->m_data;

			if(singletrain->IsSecretTrain() && !bSecretSendToSta)
				continue;

			for (int j=0; j<singletrain->GetTrainRecordCount(); j++)
			{
				const CTG_TrainRecord *prec = singletrain->GetTrainRecord_Const(j);
				if (NULL == prec || prec->GetStationNo() != plan_sta) 
					continue;

				const TG_TrainRecord &currec=prec->m_data;
				if (prec->IfInTimeScope(begin_time, end_time) /*在时间段内*/ ) 
				{
					// 台间站阶段计划处理
					if(neibStaPlan.size() > 0)
					{
						const CTG_TrainRecord *pnext = singletrain->GetTrainRecordNoDelete_Const(j+1);
						if(NULL != pnext)
						{
							int nb=0;
							for(; nb<neibStaPlan.size(); nb++)
							{
								int staNo = pnext->GetStationNo();

								if(neibStaPlan[nb].nNextNextStationNo > 0)
								{
									int nextnextstano=0;
									const CTG_TrainRecord *pnextnext = singletrain->GetTrainRecordNoDelete_Const(j+2);
									if(NULL != pnextnext)
										nextnextstano = pnextnext->GetStationNo();

									if(neibStaPlan[nb].nPreStation <= 0)
									{
										if(neibStaPlan[nb].nStationNo == staNo && neibStaPlan[nb].nNextNextStationNo == nextnextstano)
										{
											GpcLogSysMessage(0, "台间站,符合规则 sta %d (%d->%d-%d) trainid %s", plan_sta, 
												plan_sta, staNo, nextnextstano, prec->GetArriveTID());
											break; // 符合规则下发该台间站计划
										}
									}
									else
									{
										int prestano=0;
										const CTG_TrainRecord *ppre  = singletrain->GetTrainRecordNoDelete_Const(j-1);
										if(NULL != ppre)
											prestano = ppre->GetStationNo();
										if(neibStaPlan[nb].nStationNo == staNo 
											&& neibStaPlan[nb].nNextNextStationNo == nextnextstano
											&& neibStaPlan[nb].nPreStation == prestano)
										{
											GpcLogSysMessage(0, "台间站,符合规则 sta %d (%d-%d->%d-%d) trainid %s", plan_sta, 
												prestano, plan_sta, staNo, nextnextstano, prec->GetArriveTID());
											break; // 符合规则下发该台间站计划
										}
									}
								}
								else
								{
									if(neibStaPlan[nb].nPreStation <= 0)
									{
										if(neibStaPlan[nb].nStationNo == staNo)
										{
											GpcLogSysMessage(0, "台间站,符合规则 sta %d (%d->%d) trainid %s", plan_sta, 
												plan_sta, staNo, prec->GetArriveTID());
											break; // 符合规则下发该台间站计划
										}
									}
									else
									{
										int prestano=0;
										const CTG_TrainRecord *ppre  = singletrain->GetTrainRecordNoDelete_Const(j-1);
										if(NULL != ppre)
											prestano = ppre->GetStationNo();
										if(neibStaPlan[nb].nStationNo == staNo
											&& neibStaPlan[nb].nPreStation == prestano)
										{
											GpcLogSysMessage(0, "台间站,符合规则 sta %d (%d-%d->%d) trainid %s", plan_sta, 
												prestano, plan_sta, staNo, prec->GetArriveTID());
											break; // 符合规则下发该台间站计划
										}
									}
								}
							}

							if(nb>=neibStaPlan.size())
							{
								GpcLogSysMessage(0, "台间站,不符合规则,不发送计划 sta %d trainid %s", plan_sta, prec->GetArriveTID());
								continue;
							}
						}
						else 
						{
							if  (!(prec->IsTerminalRec()))  // 非台间站终到
							{
								continue;
							}
						}
					}
					
					for (short i=0; i<order.count; i++) 
					{
						// filter the train with entry, exit and train type
						ENTRY_ID entry = order.entry[i];
						ENTRY_ID exit  = order.exit[i];
						BYTE  yard  = order.yard[i];
						BYTE train_type = order.train_type[i];

						if(match>0)
						{
							if(!IsNeedSendByPlanEntryExit(currec.entry, currec.exit, entry, exit))
								continue;
							GpcLogSysMessage(0,"[station plan]train %s 在 station %d 符合 entry:%d exit:%d,发送该计划给车站", 
								prec->GetArriveTID(), currec.station, entry, exit);
						}
						else
						{
							if (IS_VALID_ENTRY(entry) && currec.entry != entry)
								continue;
							if (IS_VALID_ENTRY(exit) && currec.exit != exit)
								continue;
						}

						if ((BYTE)0xFF != train_type) 
						{
							if (!train_type && TG_IsFreightTrain(info))
								continue;
							if (train_type == 1 && TG_IsPassengerTrain(info))
								continue;
						}
						
						bool bSetTrigeRouteByPlan=false;
						if(!GetTGFunctionConfig()->IsDefaultTrigByPlan())
						{
							CTG_SingleTrain *single=trains_in_range.GetTrainPtr(iter);
							bSetTrigeRouteByPlan=SetTrigeRouteByPlan(order.con_id, single, j, trains_in_ranges, begin_time);
						}

						// organize message
						if (TG_FAILED == OrganizeMsg(insertmsg, recv_sta, bEnable3Function, singletrain, j, order.prev_sta_count, order.next_sta_count, trains_in_range, report_early, dest))
						{
							GpcLogDebugMessage(0,"send stplan organize msg to e%d err", dest);
							continue;
						}

						if(!GetTGFunctionConfig()->IsDefaultTrigByPlan())
						{
							if((insertmsg.arrive!=insertmsg.depart)&&(end_time<insertmsg.depart)&&(insertmsg.arrive<end_time))
							{
								insertmsg.stop_condition |= 0x80;
								GpcLogSysMessage(0, "设置按照计划发车,满足条件(end_time<depart && arrive<end_time) sta %d trainid %s begin_time %d end_time %d arrive %d depart %d", plan_sta, prec->GetArriveTID(),
									begin_time, end_time, insertmsg.arrive, insertmsg.depart);
							}
						}
						// log, gather or send message
						GpcLogSysMessage(0, "发送计划 sta %d trainid %s", plan_sta, prec->GetArriveTID());
						m_pSendLog->LogMsg(insertmsg, dest);
						if (!CheckSendMessage(dest, ar, insertmsg)) 
						{
							GpcLogDebugMessage(0,"send insert stplan to e%d err", dest);
							//return TG_FAILED;
						}

						// 事件处理4: 下发阶段计划(新增计划)
						GPCENTITY eventEntity;
						if(gpcdef.GetEntityInfo(136,0,0,eventEntity))
						{
							MsgEventData data;
							data.nSrcEntityID = GetLocalEntityID();
							data.tmEventTime = time(NULL);
							data.nEventRank = NORMAL_EVENT;

							data.nEventID =  EVENT_GPC_SEND_STPLAN;
							data.SetKey(KeyStationId, recv_sta);
							data.SetKey(KeyStationName, gpcdef.GetStationName(recv_sta));
							data.SetKey(KeyArrTrainNo, currec.arrive_train_id);
							data.SetKey(KeyDptTrainNo, currec.depart_train_id);
							data.SetKey(KeyTrainIndex, singletrain->GetTrainIndex());
							data.SetKey(KeyArriveTime, currec.arrive);
							data.SetKey(KeyDepartTime, currec.depart);
							data.SetKey(KeyBusinessTime, currec.min_stop_time);
							
							SendBMsg(&data, eventEntity.nEntityId);
						}

						ar << insertmsg;
						add_count++;

						notice.flag=0;
						notice.train_index=insertmsg.train_index;
						notice.rec_index=insertmsg.rec_index;

						predict_notice.push_back(notice);

						CTG_SingleTrain *single=trains_in_range.GetTrainPtr(iter);
						CTG_TrainRecord *rec = single->GetTrainRecord(j);
						rec->m_data.send_plan_time = time(NULL);
						
						if (GetTGFunctionConfig()->IsStplanTrainIndexSaveInList())
							m_pWorkSchedule->AddTrainInSendPlanList(single->GetTrainIndex());
						
						if(bSetTrigeRouteByPlan && m_pTgMMI)
							m_pTgMMI->UpdateMMITrain_Directly(single->GetTrainIndex());

						single->GetRecordList()->SetSyncStatus(m_pDoc->GetServerCommInterface()->GetInterfaceFlag(),FALSE); // 2010.06.25 此处不能用SetSyncStatusX
						break;
					}
					
					if(i>=order.count)
					{
						GpcLogSysMessage(0, "不发送计划 sta %d trainid %s entry %d  exit %d, 出入口不满足下计划限制", plan_sta, prec->GetArriveTID(), currec.entry, currec.exit);
					}
				}
			}
		}

		// 发送更新计划
		for(int iter=0;iter<trains_update.GetTrainCount();iter++)
		{
			const CTG_SingleTrain *singletrain=trains_update.GetTrainPtr(iter);
			const TG_TrainInfo *info = &singletrain->GetTrainInfo_Const()->m_data;

			if(singletrain->IsSecretTrain() && !bSecretSendToSta)
				continue;

			for (int j=0; j<singletrain->GetTrainRecordCount(); j++)
			{
				const CTG_TrainRecord *prec = singletrain->GetTrainRecord_Const(j);
				if (NULL == prec || prec->GetStationNo() != plan_sta) 
					continue;

				const TG_TrainRecord &currec=prec->m_data;

				{
					// 台间站阶段计划处理
					if(neibStaPlan.size() > 0)
					{
						const CTG_TrainRecord *pnext = singletrain->GetTrainRecordNoDelete_Const(j+1);
						if(NULL != pnext)
						{
							int nb=0;
							for(; nb<neibStaPlan.size(); nb++)
							{
								int staNo = pnext->GetStationNo();

								if(neibStaPlan[nb].nNextNextStationNo > 0)
								{
									int nextnextstano=0;
									const CTG_TrainRecord *pnextnext = singletrain->GetTrainRecordNoDelete_Const(j+2);
									if(NULL != pnextnext)
										nextnextstano = pnextnext->GetStationNo();

									if(neibStaPlan[nb].nPreStation <= 0)
									{
										if(neibStaPlan[nb].nStationNo == staNo && neibStaPlan[nb].nNextNextStationNo == nextnextstano)
										{
											GpcLogSysMessage(0, "台间站,符合规则 sta %d (%d->%d-%d) trainid %s", plan_sta, 
												plan_sta, staNo, nextnextstano, prec->GetArriveTID());
											break; // 符合规则下发该台间站计划
										}
									}
									else
									{
										int prestano=0;
										const CTG_TrainRecord *ppre  = singletrain->GetTrainRecordNoDelete_Const(j-1);
										if(NULL != ppre)
											prestano = ppre->GetStationNo();
										if(neibStaPlan[nb].nStationNo == staNo 
											&& neibStaPlan[nb].nNextNextStationNo == nextnextstano
											&& neibStaPlan[nb].nPreStation == prestano)
										{
											GpcLogSysMessage(0, "台间站,符合规则 sta %d (%d-%d->%d-%d) trainid %s", plan_sta, 
												prestano, plan_sta, staNo, nextnextstano, prec->GetArriveTID());
											break; // 符合规则下发该台间站计划
										}
									}
								}
								else
								{
									if(neibStaPlan[nb].nPreStation <= 0)
									{
										if(neibStaPlan[nb].nStationNo == staNo)
										{
											GpcLogSysMessage(0, "台间站,符合规则 sta %d (%d->%d) trainid %s", plan_sta, 
												plan_sta, staNo, prec->GetArriveTID());
											break; // 符合规则下发该台间站计划
										}
									}
									else
									{
										int prestano=0;
										const CTG_TrainRecord *ppre  = singletrain->GetTrainRecordNoDelete_Const(j-1);
										if(NULL != ppre)
											prestano = ppre->GetStationNo();
										if(neibStaPlan[nb].nStationNo == staNo
											&& neibStaPlan[nb].nPreStation == prestano)
										{
											GpcLogSysMessage(0, "台间站,符合规则 sta %d (%d-%d->%d) trainid %s", plan_sta, 
												prestano, plan_sta, staNo, prec->GetArriveTID());
											break; // 符合规则下发该台间站计划
										}
									}
								}
							}

							if(nb>=neibStaPlan.size())
							{
								GpcLogSysMessage(0, "台间站,不符合规则,不发送计划 sta %d trainid %s", plan_sta, prec->GetArriveTID());
								continue;
							}
						}
						else 
						{
							if  (!(prec->IsTerminalRec()))  // 非台间站终到
							{
								continue;
							}
						}
					}
					
					for (short i=0; i<order.count; i++) 
					{
						// filter the train with entry, exit and train type
						ENTRY_ID entry = order.entry[i];
						ENTRY_ID exit  = order.exit[i];
						BYTE  yard  = order.yard[i];
						BYTE train_type = order.train_type[i];

						if(match>0) // 出口和入口必须同时满足才下计划
						{
							if(!IsNeedSendByPlanEntryExit(currec.entry, currec.exit, entry, exit))
								continue;
							GpcLogSysMessage(0,"[station plan]train %s 在 station %d 符合 entry:%d exit:%d,发送该计划给车站", 
								prec->GetArriveTID(), currec.station, entry, exit);
						}
						else
						{
							if (IS_VALID_ENTRY(entry) && currec.entry != entry)
								continue;
							if (IS_VALID_ENTRY(exit) && currec.exit != exit)
								continue;
						}

						if ((BYTE)0xFF != train_type) 
						{
							if (!train_type && TG_IsFreightTrain(info))
								continue;
							if (train_type == 1 && TG_IsPassengerTrain(info))
								continue;
						}

						bool bSetTrigeRouteByPlan=false;
						if(!GetTGFunctionConfig()->IsDefaultTrigByPlan())
						{
							CTG_SingleTrain *single=trains_update.GetTrainPtr(iter);
							bSetTrigeRouteByPlan=SetTrigeRouteByPlan(order.con_id, single, j, trains_in_ranges, begin_time);
						}

						// organize message
						if (TG_FAILED == OrganizeMsg(insertmsg, recv_sta, bEnable3Function, singletrain, j, order.prev_sta_count, order.next_sta_count, trains_update, report_early, dest))
						{
							GpcLogDebugMessage(0,"send stplan organize msg to e%d err", dest);
							continue;
						}

						if(!GetTGFunctionConfig()->IsDefaultTrigByPlan())
						{
							if((insertmsg.arrive!=insertmsg.depart)&&(end_time<insertmsg.depart)&&(insertmsg.arrive<end_time))
							{
								insertmsg.stop_condition |= 0x80;
								GpcLogSysMessage(0, "设置按照计划发车,满足条件(end_time<depart && arrive<end_time) sta %d trainid %s begin_time %d end_time %d arrive %d depart %d", plan_sta, prec->GetArriveTID(),
									begin_time, end_time, insertmsg.arrive, insertmsg.depart);
							}
						}

						// log, gather or send message
						GpcLogSysMessage(0, "发送计划 sta %d trainid %s", plan_sta, prec->GetArriveTID());
						m_pSendLog->LogMsg(insertmsg, dest);
						if (!CheckSendMessage(dest, ar, insertmsg)) 
						{
							GpcLogDebugMessage(0,"send insert stplan to e%d err", dest);
							//return TG_FAILED;
						}

						// 事件处理4: 下发阶段计划(更新计划)
						GPCENTITY eventEntity;
						if(gpcdef.GetEntityInfo(136,0,0,eventEntity))
						{
							MsgEventData data;
							data.nSrcEntityID = GetLocalEntityID();
							data.tmEventTime = time(NULL);
							data.nEventRank = NORMAL_EVENT;

							data.nEventID =  EVENT_GPC_SEND_STPLAN;
							data.SetKey(KeyStationId, recv_sta);
							data.SetKey(KeyStationName, gpcdef.GetStationName(recv_sta));
							data.SetKey(KeyArrTrainNo, currec.arrive_train_id);
							data.SetKey(KeyDptTrainNo, currec.depart_train_id);
							data.SetKey(KeyTrainIndex, singletrain->GetTrainIndex());
							data.SetKey(KeyArriveTime, currec.arrive);
							data.SetKey(KeyDepartTime, currec.depart);
							data.SetKey(KeyBusinessTime, currec.min_stop_time);

							SendBMsg(&data, eventEntity.nEntityId);
						}

						ar << insertmsg;
						add_count++;

						notice.flag=0;
						notice.train_index=insertmsg.train_index;
						notice.rec_index=insertmsg.rec_index;
						predict_notice.push_back(notice);

						CTG_SingleTrain *single=trains_update.GetTrainPtr(iter);
						CTG_TrainRecord *rec = single->GetTrainRecord(j);
						rec->m_data.send_plan_time = time(NULL);

						if (GetTGFunctionConfig()->IsStplanTrainIndexSaveInList())
							m_pWorkSchedule->AddTrainInSendPlanList(single->GetTrainIndex());

						if(bSetTrigeRouteByPlan && m_pTgMMI)
							m_pTgMMI->UpdateMMITrain_Directly(single->GetTrainIndex());
						   
						single->GetRecordList()->SetSyncStatus(m_pDoc->GetServerCommInterface()->GetInterfaceFlag(),FALSE); // 2010.06.25 此处不能用SetSyncStatusX
						break;
					}
					if(i>=order.count)
					{
						GpcLogSysMessage(0, "不发送计划 sta %d trainid %s entry %d  exit %d, 出入口不满足下计划限制", plan_sta, prec->GetArriveTID(), currec.entry, currec.exit);
					}
				}
			}
		}

		if (!add_count && !del_count) 
		{
			// inform gpc no data change
			m_pSendStPlanStatus->SetSendStPlanStatus(plan_sta, recv_sta, dest, cSendStPlanStatus::PLAN_NO_CHANGE, begin_time,end_time, send_time, order.sender_name);
			answer.recv_status = MsgStPlanStatusAnswer::PLAN_NO_DATA;

			GpcLogSysMessage(0,"Inform gpc e%d dest e%d sta %d %d plan is not change", entity, dest, plan_sta, recv_sta);
		} 
		else 
		{
			// inform gpc msg is sending
			m_pSendStPlanStatus->SetSendStPlanStatus(plan_sta, recv_sta, dest, cSendStPlanStatus::PLAN_SENDING, begin_time, end_time, send_time, order.sender_name);
			answer.recv_status = MsgStPlanStatusAnswer::PLAN_SENDING;

			// send st msg complete
			MsgStPlanSendComplete overmsg;
			overmsg.con_id  = get_current_con_id();
			overmsg.gpc     = entity;
			overmsg.station = recv_sta;
			overmsg.insert_cnt = (BYTE)add_count;
			overmsg.delete_cnt = (BYTE)del_count;

			ar << overmsg;
			m_pSendLog->LogMsg(overmsg, dest);
			if (!SendArMessage(dest, ar)) 
			{
				GpcLogDebugMessage(0,"send blcomm err in send stplan e%d", dest);
				//return FALSE;
			}

			DB_SetPlanSendStatus(dest,TG_STATION_STATUS::SENDING); 
			GpcLogSysMessage(0,"send plan of sta %d %d insert %d del %d to %d by gpc e%d", plan_sta, recv_sta, add_count, del_count, dest, entity);
		}
    }

	m_pSendLog->LogMsg(answer);

	ProcessPlanAnswer(&answer);

	return TRUE;	
}

BOOL  cStPlan::ProcessMessage(long eid, BaseMsg *pmsg)
{
	int rs;
	if(pmsg->isKindOf(MsgStPlanTrainRecv::getTypeIDStatic()))
		rs=proc_msg(eid,(const MsgStPlanTrainRecv &)*pmsg);
	else
		if(pmsg->isKindOf(MsgStPlanRecvALL::getTypeIDStatic()))
		{
			rs=proc_msg(eid,(const MsgStPlanRecvALL &)*pmsg);
		}
		else
			return FALSE;
	if(rs==TG_SUCCESS)
		return TRUE;
	else
		return FALSE;
}

BOOL  cStPlan::ProcessPlanAnswer(const MsgStPlanStatusAnswer *pBuf)  // 原来是GPC的工作
{

	do
	{		
		if(pBuf == NULL) break;
		if(pBuf->con_id !=get_current_con_id())
			break;

		for(int i=0;i< m_StationPlanInfoNew.nEntityNum;i++)
		{
			if(IsTDCommSvr(m_StationPlanInfoNew.m_StationPlan[i].Entityid))
			{
				if((pBuf->dest == m_StationPlanInfoNew.m_StationPlan[i].Entityid)
				  && (pBuf->station == m_StationPlanInfoNew.m_StationPlan[i].nStation))
				{
					break;
				}
			}
			else
			{
				if(pBuf->dest == m_StationPlanInfoNew.m_StationPlan[i].Entityid)
					break;
			}
		}
		if(i>=m_StationPlanInfoNew.nEntityNum)
			break;
		//found one entity
		m_StationPlanInfoNew.m_StationPlan[i].bReceiveFlag=pBuf->recv_status; //1 status
		strcpy(m_StationPlanInfoNew.m_StationPlan[i].sSendName,pBuf->sender_name); // 2 send name 
		strcpy(m_StationPlanInfoNew.m_StationPlan[i].strSignName,pBuf->recver_name); // 3 rec name

		m_StationPlanInfoNew.m_StationPlan[i].begin_time  //begin time
			=pBuf->begin_time ;	
		m_StationPlanInfoNew.m_StationPlan[i].end_time  //end time
			=pBuf->end_time ;
		m_StationPlanInfoNew.m_StationPlan[i].send_time  //end time
			=pBuf->send_time ;
		m_StationPlanInfoNew.m_StationPlan[i].recv_time  //end time
			=pBuf->recv_time;

		int nIndex =  m_StationPlanInfoNew.m_StationPlan[i].nEntityIndex;
		if(m_hPlanMMIWindow && IsWindow(m_hPlanMMIWindow))
			PostMessage(m_hPlanMMIWindow,UM_STATION_RECV_STATUS_MSG,(WPARAM)(nIndex),(LPARAM)pBuf->recv_status);

	}while(0);

	return TRUE;
}


BOOL  cStPlan::InitStationPlanReceiveInfo(int section_id)
{
	GPCENTITY entityInfo;
	CGpcdef *pGpcDef=GetTGFunctionConfig()->GetGPCDEFx();
	if(!pGpcDef)
		return FALSE;

	//当前区段的实体个数和列表
	int nSendPlanIndexList[128/*MAX_STATION_PER_SECTION*/],nEntityNum;
	nEntityNum=gpcdef.GetSpecialGPCList(section_id, nSendPlanIndexList);
	if (nEntityNum==0)
		nEntityNum = pGpcDef->GetSectionPlanReceivers(section_id,nSendPlanIndexList);

	////下发列车计划的内容
	m_StationPlanInfoNew.nEntityNum = nEntityNum;      
	GPCPLANRECEIVER rec; CString Entityname;
	for(int k=0;k < nEntityNum;k++)
	{
		memset(&rec, 0x00, sizeof(rec));     // wx@ty
		pGpcDef->GetReceiverInfo(nSendPlanIndexList[k],rec);

		//站号
		m_StationPlanInfoNew.m_StationPlan[k].nStation = rec.nstationNo; 
		strcpy(m_StationPlanInfoNew.m_StationPlan[k].EntityName, rec.sentity); 

		if (false != pGpcDef->GetEntityInfo(rec.sentity, entityInfo))  // wx@ty
		{			
			if(entityInfo.nGroupId == STPC_WACHER)
				Entityname = "车站终端";
			else if(entityInfo.nGroupId ==  EQUIP_LIRC)
				Entityname = "自律机";
			else if(entityInfo.nGroupId ==  STPC_DISPACHER)
				Entityname = "站调";
			else if(entityInfo.nGroupId ==  EQUIP_SERVER)
			{
				if(IsTDCommSvr(entityInfo.nEntityId))
				{
					Entityname = "客服";
				}
				else
				{
					Entityname.Format("未知计划接收终端");
				}
			}
			else
				Entityname.Format("未定义%d", entityInfo.nGroupId);
			
			strcpy(m_StationPlanInfoNew.m_StationPlan[k].EntityName,(char*)(LPCTSTR)Entityname ); 

			//实体ID
			m_StationPlanInfoNew.m_StationPlan[k].Entityid  = entityInfo.nEntityId;
		}
		else
		{
			strcpy(m_StationPlanInfoNew.m_StationPlan[k].EntityName, "未定义"); 

			//实体ID
			m_StationPlanInfoNew.m_StationPlan[k].Entityid  = 0;

		}
		//实体index
		m_StationPlanInfoNew.m_StationPlan[k].nEntityIndex =  nSendPlanIndexList[k];
		m_StationPlanInfoNew.m_StationPlan[k].bReceiveFlag  = 6;
		m_StationPlanInfoNew.m_StationPlan[k].strSignName[0]  = 0x00;
	}
	return TRUE;
}

bool  cStPlan::SetTrigeRouteByPlan(int con_id, CTG_SingleTrain *desttrain, int rec_index, CTG_TrainPtrGroup &trains_in_range, TIME begin_time)
{
	if(desttrain==NULL)
		return false;
	
	if(!GetTGFunctionConfig()->IsEnableThirtyByPlan(con_id))
		return false;

	if(desttrain->IsPassengerTrain())
		return false;
	
	CTG_TrainRecord* precord=desttrain->GetTrainRecordNoDelete(rec_index);
	if(!precord)
	{
		return false;
	}
		
	if(!gpcdef.IsCtcStation(precord->GetStationNo()))
	{
		return false;
	}

	if(precord->IsPassRec() || precord->IsTerminalRec() || precord->IsStartRec())
		return false;

	long depart=precord->GetDepartTime();
	if(/*begin_time+6*60*60<depart || */depart<begin_time) // 2013.05.09 30分钟按照计划排路不设置右侧范围
		return false;

	if(TG_IsMakeDepartRouteByPlan(precord->m_data))
	{
		return false;
	}

	bool bFind=false;
	int dlt=999999;
	for(int i=0;i<trains_in_range.GetTrainCount(); i++)
	{
		CTG_SingleTrain *ptrain=trains_in_range.GetTrainPtr(i);
		if (ptrain->GetTrainIndex() == desttrain->GetTrainIndex()) 
			continue;

		for (int j=0; j<ptrain->GetTrainRecordCount(); j++)
		{
			const CTG_TrainRecord *prec=ptrain->GetTrainRecordNoDelete(j); 
			if (prec==NULL)
				continue; 

			if(prec->IsTerminalRec())
				continue;

			if(prec->IsActualDepartTimeRecord())
				continue;

			if(precord->GetStationNo()!=prec->GetStationNo())
				continue;

			if(precord->GetExit()!=prec->GetExit())
				continue;

			if(precord->GetArriveTime()<prec->GetArriveTime() && prec->GetDepartTime()<precord->GetDepartTime())
			{
				int dltmp = precord->GetDepartTime()-prec->GetDepartTime();
				if(dltmp<=dlt)
				{
					dlt=dltmp;
					bFind=true;
				}
			}
		}
	}

	if(dlt>30*60 && bFind)
	{
		if(TG_IsMakeDepartRouteByMeet(precord->m_data))
		{
			TG_SetMakeDepartRouteByPlan(precord->m_data);
			GpcLogTrainMessage(0,0,"下发计划,设定列车 %s 在 %d  发车时机 为按照计划", 
						desttrain->GetTrainID(), precord->GetStationNo());
			return true;
		}
		else
		{
			return false;
		}
	}
	
	return false;
}

// 找后续车的发点及车次 20150623 珠三角城际
long  cStPlan::GetNextTrainDepart(CString czh, int station, long curr_train_index,CString& trainid)
{
	CTG_SingleTrain* ptrain=NULL;
	for(int i=0; i<m_relation.size(); i++)
	{
		if(czh==m_relation[i].cCzh)
		{
			for(int n=0; n<m_relation[i].nTrainCount; n++)
			{
				if(curr_train_index==m_relation[i].nTrainIndex[n])
				{
					break;
				}
			}
			if(n+1<m_relation[i].nTrainCount)
			{
				long next_index=m_relation[i].nTrainIndex[n+1];
				ptrain=m_pWorkSchedule->GetSingleTrain(next_index);
			}
			break;
		}
	}
	
	if(ptrain)
	{
		CTG_TrainRecord* rd=ptrain->GetTrainFirstRecord();
		if(rd)
		{
			if(rd->m_data.station==station)
			{
				trainid=rd->m_data.depart_train_id;
				return rd->m_data.depart;
			}
		}
	}

	return 0;
}

// 找前一车的到点及折返标志 20150623 珠三角城际
long  cStPlan::GetBeforeTrainArrive(CString czh, int station, long curr_train_index,long&side_utility)
{
	CTG_SingleTrain* ptrain=NULL;
	for(int i=0; i<m_relation.size(); i++)
	{
		if(czh==m_relation[i].cCzh)
		{
			for(int n=0; n<m_relation[i].nTrainCount; n++)
			{
				if(curr_train_index==m_relation[i].nTrainIndex[n])
				{
					break;
				}
			}
			if(n>0)
			{
				long before_index=m_relation[i].nTrainIndex[n-1];
				ptrain=m_pWorkSchedule->GetSingleTrain(before_index);
			}
			break;
		}
	}
	
	if(ptrain)
	{
		CTG_TrainRecord* rd=ptrain->GetTrainLastRecord();
		if(rd)
		{
			if(rd->m_data.station==station)
			{
				side_utility=ptrain->GetTrainInfo()->m_data.side_utility;
				return rd->m_data.arrive;
			}
		}
	}

	return 0;
}

// 调用OCX输出当前的车组关系到data目录下文件czh_relation.xml 20150623 珠三角城际
void  cStPlan::OutprintCZHRelation()
{
	if(!GetTGFunctionConfig()->IsSupportInterCityStPlan())
	{
		if(GetTGFunctionConfig()->IsChangeTrainID())
			return;
	}
	m_pTgMMI->OutprintCZHRelation();
}

// 读取OCX输出的车组关系,data目录下文件czh_relation.xml 20150623 珠三角城际
#import  <msxml.dll> named_guids
using namespace MSXML;
void  cStPlan::LoadTrainRelation()
{
	m_relation.clear();

	if(!GetTGFunctionConfig()->IsSupportInterCityStPlan())
	{
		if(GetTGFunctionConfig()->IsChangeTrainID())
			return;
	}
	CString strFileName;
	strFileName.Format("%s\\czh_relation.xml", gpcdef.GetDataDir());
	
	GpcLogTrainMessage(0, 0, "下发阶段计划读取车组关系文件:%s", strFileName);

	MSXML::IXMLDOMDocumentPtr pXMLDoc;
	HRESULT hr = pXMLDoc.CreateInstance(MSXML::CLSID_DOMDocument);
	if(FAILED(hr))
	{
		_com_error er(hr);
		AfxMessageBox(er.ErrorMessage());
		return;
	}

	if(VARIANT_TRUE != pXMLDoc->load( _bstr_t( strFileName ) ) )
	{
		CString  prompt;
		prompt.Format("车组关系%s不存在!", strFileName);
		AfxMessageBox(prompt);
		return; 
	}

	MSXML::IXMLDOMNodePtr pRoot = pXMLDoc->selectSingleNode( "root");
	if(pRoot == NULL)
	{
		CString text;
		text.Format("车组关系文件 %s 不符合xml语法!",strFileName);
		AfxMessageBox(text);
		return;
	}
	_variant_t v;
	MSXML::IXMLDOMNodePtr pNode, pChild;
	CTime curtm=CTime::GetCurrentTime();
	long nowtm=curtm.GetTime();
	pChild = pRoot->selectSingleNode( "@tm" );
	if(pChild)
	{
		v = pChild->GetnodeTypedValue();
		CString ss=v.bstrVal;
		long tm=atol(ss);
		GpcLogTrainMessage(0, 0, "车组:生成时间%d, 当前时间%d", tm, nowtm);
		if(abs(nowtm-tm)>=60)
		{
			AfxMessageBox("车组的生成时间超过60秒!");
			return;
		}
	}

	MSXML::IXMLDOMNodeListPtr pCZHList = pRoot->selectNodes("CZH");
	for(int m = 0; m <pCZHList->Getlength(); m++)
	{
		Train_Relation relation;
		relation.nTrainCount=0;
		MSXML::IXMLDOMNodePtr pCZHNode = pCZHList->Getitem(m);
		pChild = pCZHNode->selectSingleNode( "@name" );
		if(pChild)
		{
			v = pChild->GetnodeTypedValue();
			CString ss=v.bstrVal;
			if(ss.GetLength()>0)
			{
				int size=sizeof(relation.cCzh)/sizeof(relation.cCzh[0]);
				if(ss.GetLength()>=size)
				{
					strncpy(relation.cCzh, ss, size-1);
					relation.cCzh[size-1]=0;
				}
				else
				{
					strcpy(relation.cCzh, ss);
				}
			}
			SysFreeString(v.bstrVal);
			v.Clear();
		}
		GpcLogTrainMessage(0, 0, "车组:%s", relation.cCzh);
		MSXML::IXMLDOMNodeListPtr pTrainList = pCZHNode->selectNodes("TRAIN");
		for(int n = 0; n <pTrainList->Getlength(); n++)
		{
			MSXML::IXMLDOMNodePtr pNode = pTrainList->Getitem(n);
			CString strTrainId;
			long nTrainIndex=0;
			pChild = pNode->selectSingleNode( "@name" );
			if(pChild)
			{
				v = pChild->GetnodeTypedValue();
				strTrainId=v.bstrVal;
				SysFreeString(v.bstrVal);
				v.Clear();
			}
			pChild = pNode->selectSingleNode( "@index" );
			if(pChild)
			{
				v = pChild->GetnodeTypedValue();
				nTrainIndex = atol(CString((v.bstrVal)));
				SysFreeString(v.bstrVal);
				v.Clear();
			}

			if(nTrainIndex<=0)
			{
				GpcLogTrainMessage(0, 0, "%s 车组关系非法丢弃", relation.cCzh);
				break;
			}			
			int i=relation.nTrainCount;
			relation.nTrainIndex[i]=nTrainIndex;
			relation.nTrainCount++;
			GpcLogTrainMessage(0, 0, "Train:%s Index:%d", strTrainId, nTrainIndex);
		}

		if(relation.nTrainCount>1)
		{
			m_relation.push_back(relation);
		}
	}
	
	GpcLogTrainMessage(0, 0, "完成车组关系文件读取");
	//if (m_relation.size()<=0)
	//	AfxMessageBox("车组关系未生成,请确认!");
	return;
}

bool  cStPlan::GetStartTrainDepartNo(int station, long curr_train_index, TRAIN_NO trainid)
{
	memset(trainid, 0, sizeof(trainid));
	long next_index=0;
	CTG_SingleTrain* ptrain=NULL;
	CString czh;
	for(int i=0; i<m_relation.size(); i++)
	{
		for(int n=0; n<m_relation[i].nTrainCount; n++)
		{
			if(curr_train_index==m_relation[i].nTrainIndex[n])
			{
				czh=m_relation[i].cCzh;
				break;
			}
		}
		if(n+1<m_relation[i].nTrainCount)
		{
			next_index=m_relation[i].nTrainIndex[n+1];
			ptrain=m_pWorkSchedule->GetSingleTrain(next_index);
		}
	}
	
	if(ptrain)
	{
		CTG_TrainRecord* first=ptrain->GetTrainFirstRecord();
		if(first)
		{
			if(first->IsStartRec())
			{
				if(first->m_data.station==station)
				{
					TG_CopyTrainId(trainid, first->m_data.depart_train_id);
					GpcLogSysMessage(0, "下发车站 %d 列车 %u 计划, 找到后续列车 %u 车组号 %s,设置折返车次 %s.", 
						station, curr_train_index, next_index, czh, trainid);
					return true;
				}
				else
				{	
					GpcLogSysMessage(0, "下发车站 %d 列车 %u 计划, 找到后续列车 %u 车组号 %s, 但是后续列车首站 %d 和前车终到站不是同一站,不能设置折返车次.", 
						station, curr_train_index, next_index, czh, first->m_data.station);
				}
			}
			else
			{
				GpcLogSysMessage(0, "下发车站 %d 列车 %u 计划, 找到后续列车 %u 车组号 %s, 但是后续列车首站不是始发,不能设置折返车次.", 
					station, curr_train_index, next_index, czh);
			}
		}
		else
		{
			GpcLogSysMessage(0, "下发车站 %d 列车 %u 计划, 找到后续列车 %u 车组号 %s, 但是获取第一个记录失败,不能设置折返车次.", 
				station, curr_train_index, next_index, czh);
		}
	}
	else
	{
		GpcLogSysMessage(0, "下发车站 %d 列车 %u 计划, 找不到后续始发车, 不能设置折返车次.", station, curr_train_index, czh);
	}

	return false;
}

void cStPlan::SendBasicPlan(STATION_NO station, int con_id, ENTITY_ID sender, ENTITY_ID dest)
{
	if(m_bplan.plan_count!=0)
	{
		m_pSendLog->LogMsg(m_bplan, dest);
		SendBMsg(&m_bplan, dest);
		m_nbPlanCount+=m_bplan.plan_count;
		m_bplan.plan_count=0;
	}

	TIME ltime = time(NULL);
	CString logstr;
	logstr.Format("结束发送基本图计划： 车站号 %d 台号 %d 发送设备号 %d 接受设备号 %d 增加个数 %d.", station, con_id, sender, dest, m_nbPlanCount);
	SaveLogMessageStr("stplan",ltime,logstr);
}

void cStPlan::SendBasicPlan(CTG_TrainPtrGroup& result, STATION_NO plan_sta, STATION_NO recv_sta, ENTITY_ID dest, CString strArTid, CString strDpTid)
{
	bool bFind=false;
	for(int n=0;n<result.GetTrainCount();n++)
	{
		CTG_SingleTrain *pbasictrain=result.GetTrainPtr(n);
		if(pbasictrain==NULL)
			continue;

		for(int i=0; i<pbasictrain->GetTrainRecordCount(); i++)
		{
			CTG_TrainRecord *prec=pbasictrain->GetTrainRecordByRecIndex(i);
			if(prec==NULL)
			{
				continue;
			}

			if(prec->m_data.station==plan_sta && (strArTid==prec->GetArriveTID() || strDpTid==prec->GetDepartTID()))					
			{
				CTG_TrainRecord *prec_pre=pbasictrain->GetTrainRecordByRecIndex(i-1);
				CTG_TrainRecord *prec_next=pbasictrain->GetTrainRecordByRecIndex(i+1);

				MsgBaseTrainPlan::base_plan plan;

				TG_CopyTrainId(plan.arrive_train, prec->m_data.arrive_train_id);
				TG_CopyTrainId(plan.depart_train, prec->m_data.depart_train_id);

				plan.entry       = (char)prec->m_data.entry;
				plan.side		 = get_stplan_sideno(prec->m_data.arrive_side);
				plan.exit        = (char)prec->m_data.exit;

				if(prec_pre!=NULL)
					plan.prev_station  = prec_pre->m_data.station;
				else
					plan.prev_station  = 0;

				if(prec_next!=NULL)
					plan.next_station  = prec_next->m_data.station;
				else
					plan.next_station  = 0;

				plan.arrive_time       = (prec->GetArriveTime()%86400);
				plan.depart_time       = (prec->GetDepartTime()%86400);
				
				// 预留
				memset(plan.reserve, 0, sizeof(plan.reserve));

				m_bplan.base_plan_record[m_bplan.plan_count]=plan;
				m_bplan.plan_count++;

				bFind=true;
				if(m_bplan.plan_count==MsgBaseTrainPlan::MAX_PLAN_COUNT)
				{
					m_pSendLog->LogMsg(m_bplan, dest);
					SendBMsg(&m_bplan, dest);
					m_nbPlanCount+=m_bplan.plan_count;
					m_bplan.plan_count=0;
				}
			}
		}
	}

	if(!bFind)
	{
		TIME ltime = time(NULL);
		CString logstr;
		logstr.Format("发送基本图计划： 在基本图中找不到 车站号 %d 到达车次 %s 出发车次 %s", m_bplan.station, strArTid, strDpTid);
		SaveLogMessageStr("stplan",ltime,logstr);
	}
}

BOOL  cStPlan::SendBasicPlan(const MsgStPlanSendOrder &order, bool bSecretSendToSta, int match)
{
	// 台间站阶段计划处理
	std::vector<NeibStation> neibStaPlan;
	GetTGFunctionConfig()->GetNeibStations(order.con_id, order.station, neibStaPlan);

	GpcLogSysMessage(0, "\n---------下发车站 %d 基本图计划--------", order.station);

	for(int i=0; i<neibStaPlan.size(); i++)
	{
		GpcLogSysMessage(0, "台间站配置: con_id %d nPreStaNo %d nNeibStaNo %d nNextStaNo %d nNextNextStaNo %d", 
			order.con_id,
			neibStaPlan[i].nPreStation, 
			neibStaPlan[i].nNeibStaNo,
			neibStaPlan[i].nStationNo, 
			neibStaPlan[i].nNextNextStationNo);
	}
	
	if(match>0)
		GpcLogTrainMessage(0,0,"按照出入口条件发送基本图阶段计划时section=%d station=%d 能够处理始发和终到列车", order.con_id, order.station);

	GpcLogTrainMessage(0,0,"向entity=%d 发送如下出入口条件阶段计划(entry=-1表示不限制入口 exit=-1表示不限制出口)",  order.st);
	for(int k=0; k<order.count; k++)
	{
		CString tmp="";
		tmp.Format("entry=%d exit=%d", (char)order.entry[k], (char)order.exit[k]);
		GpcLogTrainMessage(0,0,"允许下达 %s 计划", tmp);
	}
	
	ENTITY_ID entity=GetLocalEntityID();

	// in order to support dmis-format convert, delete seconds from send_time 
	TIME send_time = order.send_time-order.send_time%60;

	TIME begin_time = order.begin_time;
	TIME end_time   = order.end_time;

	m_pSendLog->LogMsg(order, entity);

	if (!m_pBasicPara->VerifyStationNumber(order.station)) 
	{
		GpcLogDebugMessage(0,"send stplan err sta %d to e%d", order.station, order.st);
		return FALSE;
	}
	if ((ULONG)(end_time - begin_time) > 5*3600 || begin_time < GetAdjustStartTime() - 3*3600) 
	{
		GpcLogDebugMessage(0,"send stplan sta %d invalid time %d -> %d", order.station, begin_time, end_time);
		GpcLogDebugMessage(0,"send stplan sendtime %d AdjustStartTime %d cur %d", send_time, GetAdjustStartTime(), GetCurrentTime());
		return FALSE;
	}
	if (order.prev_sta_count >= 8 || order.next_sta_count >= 8) 
	{
		GpcLogDebugMessage(0,"send stplan sta %d to e%d err prev_sta_count %d next_sta_count %d", order.station, order.st, order.prev_sta_count, order.next_sta_count);
		return FALSE;
	}

	GpcLogSysMessage(0,"recv StPlanSendOrder e%d sta %d sendtime %d %d->%d", order.st, order.station, send_time, begin_time, end_time);

	ENTITY_ID dest = order.st;
	STATION_NO plan_sta = order.station;
	STATION_NO recv_sta = order.station_dmis;

	CString report_early;
	m_bplan.station=recv_sta;
	m_bplan.plan_count=0;
	m_nbPlanCount=0;

	CTG_TrainPtrGroup basic_trains;
	if(NULL == m_pBasicSchedule)
	{
		GpcLogSysMessage(0,"BasicSchedule point is NULL, can't get basic trains.");
		return FALSE;
	}
	m_pBasicSchedule->FindTrainsForBasicStplan(basic_trains, plan_sta);
	GpcLogSysMessage(0,"the basic train count is %d.", basic_trains.GetTrainCount());

	// 对象死活不作为必要的发送条件
	{
		// 曾向该车站下过计划记录, 应该发删除计划
		CTG_TrainPtrGroup  trains_delete;
		// 曾向该车站下过计划记录, 应该发更新计划
		CTG_TrainPtrGroup  trains_update; 
		// 存放当前时间前1小时----当前时间后18小时内的车
		CTG_TrainPtrGroup  trains_in_range, trains_in_ranges;
		m_pWorkSchedule->FindTrainsForStplan(trains_in_range, plan_sta, begin_time-1*3600, begin_time+18*3600);
		m_pWorkSchedule->FindTrainsForStplan(trains_in_ranges, plan_sta, begin_time-1*1800, begin_time+6*3600);
		
		trains_delete.Clear();
		trains_update.Clear();
		// 查找不在下计划时间段内,但是需要删除和更新的车
		for(int iter=0;iter<trains_in_range.GetTrainCount();iter++)
		{
			const CTG_SingleTrain *singletrain=trains_in_range.GetTrainPtr(iter);
			const TG_TrainInfo *info   = &singletrain->GetTrainInfo_Const()->m_data;

			for (int j=0; j<singletrain->GetTrainRecordCountWithDeleted()/*GetTrainRecordCount()*/; j++)
			{
				const CTG_TrainRecord *prec = singletrain->GetTrainRecord_Const(j);
				if (NULL == prec || prec->GetStationNo() != plan_sta) continue;

				const TG_TrainRecord &currec=prec->m_data;				

				if (!prec->IfInTimeScope(order.begin_time, order.end_time))
				{
					if(currec.send_plan_time>0)
					{
						if(NULL_TIME != currec.arrive && NULL_TIME != currec.depart)
						{
							if ((end_time < currec.arrive) && (end_time < currec.depart))
							{
								trains_delete.AddTrainPtrNotOrder(trains_in_range.GetTrainPtr(iter));
								continue;
							}
							if ((currec.arrive < begin_time) && (end_time < currec.depart))
							{
								trains_update.AddTrainPtrNotOrder(trains_in_range.GetTrainPtr(iter));
								continue;
							}
							if((currec.arrive < begin_time) && (currec.depart < begin_time))
							{
								if (!prec->IsActualArriveTimeRecord() && currec.arrive > begin_time-3600)
								{
									int realRecCount=singletrain->GetTrainRecordCount();
									int allRecCount=singletrain->GetTrainRecordCountWithDeleted();
									if(j+1 == realRecCount/*最后一个点*/ && j+1 < allRecCount/*后续有删除点*/)
									{
										trains_update.AddTrainPtrNotOrder(trains_in_range.GetTrainPtr(iter));
										continue;
									}

									// 折返车
									const CTG_TrainRecord *pnext = singletrain->GetTrainRecord_Const(j+1);
									if(pnext && plan_sta == pnext->GetStationNo())
									{
										trains_update.AddTrainPtrNotOrder(trains_in_range.GetTrainPtr(iter));
										continue;
									}
								}
							}
						}
					}
					continue;
				}
			}

			// 曾经下过计划, 后来拖出当前时间3小时范围外, 没有再下计划就被删除的车
			if(0 == singletrain->GetTrainRecordCount())
			{
				for (int j=0; j<singletrain->GetTrainRecordCountWithDeleted(); j++)
				{
					const CTG_TrainRecord *prec = singletrain->GetTrainRecord_Const(j);
					if (NULL == prec || prec->GetStationNo() != plan_sta) continue;

					const TG_TrainRecord &currec=prec->m_data;				

					if (!prec->IfInTimeScope(order.begin_time, order.end_time))
					{
						if(currec.send_plan_time>0)
						{
							trains_delete.AddTrainPtrNotOrder(trains_in_range.GetTrainPtr(iter));
							
							continue;
						}
					}
				}
			}
		}  //while(ALL trains)

		if (begin_time != order.begin_time || end_time != order.end_time)
			GpcLogSysMessage(0,"send stplan send time scope is expand to %d %d", begin_time, end_time);

		//MsgStPlanInsert insertmsg; // used to get data
		
		//insertmsg.con_id = get_current_con_id();
		//insertmsg.gpc    = entity;
		
		// 存放存放begin_time<=----<=end_time内的车
		trains_in_range.Clear();
		m_pWorkSchedule->FindTrainsForStplan(trains_in_range, plan_sta, begin_time, end_time);

		short del_count=0;
		for(int iter=0;iter<trains_in_range.GetTrainCount();iter++)
		{
			const CTG_SingleTrain *singletrain=trains_in_range.GetTrainPtr(iter);
			const TG_TrainInfo *info = &singletrain->GetTrainInfo_Const()->m_data;

			if(singletrain->IsSecretTrain() && !bSecretSendToSta)
				continue;

			for (int j=0; j<singletrain->GetTrainRecordCount(); j++)
			{
				const CTG_TrainRecord *prec = singletrain->GetTrainRecord_Const(j);
				if (NULL == prec || prec->GetStationNo() != plan_sta) 
					continue;

				const TG_TrainRecord &currec=prec->m_data;
				if (prec->IfInTimeScope(begin_time, end_time) /*在时间段内*/ ) 
				{
					// 台间站阶段计划处理
					if(neibStaPlan.size() > 0)
					{
						const CTG_TrainRecord *pnext = singletrain->GetTrainRecordNoDelete_Const(j+1);
						if(NULL != pnext)
						{
							int nb=0;
							for(; nb<neibStaPlan.size(); nb++)
							{
								int staNo = pnext->GetStationNo();

								if(neibStaPlan[nb].nNextNextStationNo > 0)
								{
									int nextnextstano=0;
									const CTG_TrainRecord *pnextnext = singletrain->GetTrainRecordNoDelete_Const(j+2);
									if(NULL != pnextnext)
										nextnextstano = pnextnext->GetStationNo();

									if(neibStaPlan[nb].nPreStation <= 0)
									{
										if(neibStaPlan[nb].nStationNo == staNo && neibStaPlan[nb].nNextNextStationNo == nextnextstano)
										{
											GpcLogSysMessage(0, "台间站,符合规则 sta %d (%d->%d-%d) trainid %s", plan_sta, 
												plan_sta, staNo, nextnextstano, prec->GetArriveTID());
											break; // 符合规则下发该台间站计划
										}
									}
									else
									{
										int prestano=0;
										const CTG_TrainRecord *ppre  = singletrain->GetTrainRecordNoDelete_Const(j-1);
										if(NULL != ppre)
											prestano = ppre->GetStationNo();
										if(neibStaPlan[nb].nStationNo == staNo 
											&& neibStaPlan[nb].nNextNextStationNo == nextnextstano
											&& neibStaPlan[nb].nPreStation == prestano)
										{
											GpcLogSysMessage(0, "台间站,符合规则 sta %d (%d-%d->%d-%d) trainid %s", plan_sta, 
												prestano, plan_sta, staNo, nextnextstano, prec->GetArriveTID());
											break; // 符合规则下发该台间站计划
										}
									}
								}
								else
								{
									if(neibStaPlan[nb].nPreStation <= 0)
									{
										if(neibStaPlan[nb].nStationNo == staNo)
										{
											GpcLogSysMessage(0, "台间站,符合规则 sta %d (%d->%d) trainid %s", plan_sta, 
												plan_sta, staNo, prec->GetArriveTID());
											break; // 符合规则下发该台间站计划
										}
									}
									else
									{
										int prestano=0;
										const CTG_TrainRecord *ppre  = singletrain->GetTrainRecordNoDelete_Const(j-1);
										if(NULL != ppre)
											prestano = ppre->GetStationNo();
										if(neibStaPlan[nb].nStationNo == staNo
											&& neibStaPlan[nb].nPreStation == prestano)
										{
											GpcLogSysMessage(0, "台间站,符合规则 sta %d (%d-%d->%d) trainid %s", plan_sta, 
												prestano, plan_sta, staNo, prec->GetArriveTID());
											break; // 符合规则下发该台间站计划
										}
									}
								}
							}

							if(nb>=neibStaPlan.size())
							{
								GpcLogSysMessage(0, "台间站,不符合规则,不发送计划 sta %d trainid %s", plan_sta, prec->GetArriveTID());
								continue;
							}
						}
						else 
						{
							if  (!(prec->IsTerminalRec()))  // 非台间站终到
							{
								continue;
							}
						}
					}
					
					for (short i=0; i<order.count; i++) 
					{
						// filter the train with entry, exit and train type
						ENTRY_ID entry = order.entry[i];
						ENTRY_ID exit  = order.exit[i];
						BYTE  yard  = order.yard[i];
						BYTE train_type = order.train_type[i];

						if(match>0) // 出口和入口必须同时满足才下计划
						{
							if(!IsNeedSendByPlanEntryExit(currec.entry, currec.exit, entry, exit))
								continue;
							GpcLogSysMessage(0,"[station plan]train %s 在 station %d 符合 entry:%d exit:%d,发送该计划给车站", 
								prec->GetArriveTID(), currec.station, entry, exit);
						}
						else
						{
							if (IS_VALID_ENTRY(entry) && currec.entry != entry)
								continue;
							if (IS_VALID_ENTRY(exit) && currec.exit != exit)
								continue;
						}

						if ((BYTE)0xFF != train_type) 
						{
							if (!train_type && TG_IsFreightTrain(info))
								continue;
							if (train_type == 1 && TG_IsPassengerTrain(info))
								continue;
						}
						
						// organize message
						//if (TG_FAILED == OrganizeMsg(insertmsg, recv_sta, singletrain, j, order.prev_sta_count, order.next_sta_count, trains_in_range, report_early, dest))
						//{
						//	GpcLogDebugMessage(0,"send stplan organize msg to e%d err", dest);
						//	continue;
						//}
						
						GpcLogSysMessage(0, "发送计划 sta %d trainid %s", plan_sta, prec->GetArriveTID());
						SendBasicPlan(basic_trains,plan_sta,recv_sta,dest,currec.arrive_train_id,currec.depart_train_id);
						
						break;
					}
					
					if(i>=order.count)
					{
						GpcLogSysMessage(0, "不发送计划 sta %d trainid %s entry %d  exit %d, 出入口不满足下计划限制", plan_sta, prec->GetArriveTID(), currec.entry, currec.exit);
					}
				}
			}
		}

		// 发送更新计划
		for(int iter=0;iter<trains_update.GetTrainCount();iter++)
		{
			const CTG_SingleTrain *singletrain=trains_update.GetTrainPtr(iter);
			const TG_TrainInfo *info = &singletrain->GetTrainInfo_Const()->m_data;

			if(singletrain->IsSecretTrain() && !bSecretSendToSta)
				continue;

			for (int j=0; j<singletrain->GetTrainRecordCount(); j++)
			{
				const CTG_TrainRecord *prec = singletrain->GetTrainRecord_Const(j);
				if (NULL == prec || prec->GetStationNo() != plan_sta) 
					continue;

				const TG_TrainRecord &currec=prec->m_data;

				{
					// 台间站阶段计划处理
					if(neibStaPlan.size() > 0)
					{
						const CTG_TrainRecord *pnext = singletrain->GetTrainRecordNoDelete_Const(j+1);
						if(NULL != pnext)
						{
							int nb=0;
							for(; nb<neibStaPlan.size(); nb++)
							{
								int staNo = pnext->GetStationNo();

								if(neibStaPlan[nb].nNextNextStationNo > 0)
								{
									int nextnextstano=0;
									const CTG_TrainRecord *pnextnext = singletrain->GetTrainRecordNoDelete_Const(j+2);
									if(NULL != pnextnext)
										nextnextstano = pnextnext->GetStationNo();

									if(neibStaPlan[nb].nPreStation <= 0)
									{
										if(neibStaPlan[nb].nStationNo == staNo && neibStaPlan[nb].nNextNextStationNo == nextnextstano)
										{
											GpcLogSysMessage(0, "台间站,符合规则 sta %d (%d->%d-%d) trainid %s", plan_sta, 
												plan_sta, staNo, nextnextstano, prec->GetArriveTID());
											break; // 符合规则下发该台间站计划
										}
									}
									else
									{
										int prestano=0;
										const CTG_TrainRecord *ppre  = singletrain->GetTrainRecordNoDelete_Const(j-1);
										if(NULL != ppre)
											prestano = ppre->GetStationNo();
										if(neibStaPlan[nb].nStationNo == staNo 
											&& neibStaPlan[nb].nNextNextStationNo == nextnextstano
											&& neibStaPlan[nb].nPreStation == prestano)
										{
											GpcLogSysMessage(0, "台间站,符合规则 sta %d (%d-%d->%d-%d) trainid %s", plan_sta, 
												prestano, plan_sta, staNo, nextnextstano, prec->GetArriveTID());
											break; // 符合规则下发该台间站计划
										}
									}
								}
								else
								{
									if(neibStaPlan[nb].nPreStation <= 0)
									{
										if(neibStaPlan[nb].nStationNo == staNo)
										{
											GpcLogSysMessage(0, "台间站,符合规则 sta %d (%d->%d) trainid %s", plan_sta, 
												plan_sta, staNo, prec->GetArriveTID());
											break; // 符合规则下发该台间站计划
										}
									}
									else
									{
										int prestano=0;
										const CTG_TrainRecord *ppre  = singletrain->GetTrainRecordNoDelete_Const(j-1);
										if(NULL != ppre)
											prestano = ppre->GetStationNo();
										if(neibStaPlan[nb].nStationNo == staNo
											&& neibStaPlan[nb].nPreStation == prestano)
										{
											GpcLogSysMessage(0, "台间站,符合规则 sta %d (%d-%d->%d) trainid %s", plan_sta, 
												prestano, plan_sta, staNo, prec->GetArriveTID());
											break; // 符合规则下发该台间站计划
										}
									}
								}
							}

							if(nb>=neibStaPlan.size())
							{
								GpcLogSysMessage(0, "台间站,不符合规则,不发送计划 sta %d trainid %s", plan_sta, prec->GetArriveTID());
								continue;
							}
						}
						else 
						{
							if  (!(prec->IsTerminalRec()))  // 非台间站终到
							{
								continue;
							}
						}
					}
					
					for (short i=0; i<order.count; i++) 
					{
						// filter the train with entry, exit and train type
						ENTRY_ID entry = order.entry[i];
						ENTRY_ID exit  = order.exit[i];
						BYTE  yard  = order.yard[i];
						BYTE train_type = order.train_type[i];

						if(match>0) // 出口和入口必须同时满足才下计划
						{
							if(!IsNeedSendByPlanEntryExit(currec.entry, currec.exit, entry, exit))
								continue;
							GpcLogSysMessage(0,"[station plan]train %s 在 station %d 符合 entry:%d exit:%d,发送该计划给车站", 
								prec->GetArriveTID(), currec.station, entry, exit);
						}
						else
						{
							if (IS_VALID_ENTRY(entry) && currec.entry != entry)
								continue;
							if (IS_VALID_ENTRY(exit) && currec.exit != exit)
								continue;
						}

						if ((BYTE)0xFF != train_type) 
						{
							if (!train_type && TG_IsFreightTrain(info))
								continue;
							if (train_type == 1 && TG_IsPassengerTrain(info))
								continue;
						}

						// organize message
						//if (TG_FAILED == OrganizeMsg(insertmsg, recv_sta, singletrain, j, order.prev_sta_count, order.next_sta_count, trains_update, report_early, dest))
						//{
						//	GpcLogDebugMessage(0,"send stplan organize msg to e%d err", dest);
						//	continue;
						//}

						GpcLogSysMessage(0, "发送计划 sta %d trainid %s", plan_sta, prec->GetArriveTID());
						SendBasicPlan(basic_trains,plan_sta,recv_sta,dest,currec.arrive_train_id,currec.depart_train_id);
						
						break;
					}
					if(i>=order.count)
					{
						GpcLogSysMessage(0, "不发送计划 sta %d trainid %s entry %d  exit %d, 出入口不满足下计划限制", plan_sta, prec->GetArriveTID(), currec.entry, currec.exit);
					}
				}
			}
		}
    }

    // 发送剩余未发送基本图计划
	SendBasicPlan(plan_sta, order.con_id, entity, dest);

	return TRUE;
}

