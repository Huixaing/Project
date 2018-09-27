#include "stdafx.h"

#include <time.h>
#include "tg_schedule_base.h"
#include "tg_function_config.h"
#include "function.h"
#include "sysshare.h"
#include "tg_filebase.h"
#include "tg_basicpara.h"
#define  MAX_SYNC_TIME  60
// defined in xml_utility.cpp
extern const char *GetXMLAttribStringValue(const char *xml,const char *attr);
extern const char *SetXMLAttribStringValue(const char *xml,const char *attr,const char *value,int limitlen);
extern void GpcLogTrainMessage(short process_id, long id, const char *fmt, ...);

//#define TG_DEBUG
#define  WHOLE_DAY_SECOND 86400
CTG_ScheduleInfo::CTG_ScheduleInfo()
{
	m_nType=SCHEDULE_TYPE_NULL;
	m_nSectionID=0;
	m_tStartTime=0;
	m_tEndTime=0;
	m_nShiftID=0;
	m_nParentSectionID=0;

	m_nAutoMove=FALSE;
	m_nTotelLen=0;
	m_nHistoryLen=0;
}

int   CTG_ScheduleInfo::GetShiftNo()
{
	return m_nShiftID - m_nShiftID/100*100;
}

CTG_ScheduleInfo::CTG_ScheduleInfo(SCHEDULE_TYPE type,int con_id,int shift_id)
{
	m_nAutoMove=FALSE;

	m_nType=type;
	m_nSectionID=con_id;
	m_nShiftID=shift_id;
	m_nHistoryLen=0;
	m_nTotelLen=24;

	int nShiftDate = (shift_id/1000000 - 1990) * 380 +
		(shift_id/10000-(shift_id/1000000)*100 - 1) * 31 + 
		shift_id/100 - (shift_id/10000)*100  - 1 ;

	int nShiftNo = shift_id - shift_id/100*100;
	if(nShiftNo == 2)
	{
		m_tStartTime=long_time(OsGetPrevDate(0,nShiftDate),18*3600);
		m_tEndTime=long_time(nShiftDate,24*3600);
	}
	else
	{
		m_tStartTime=long_time(OsGetPrevDate(0,nShiftDate),6*3600);
		m_tEndTime=long_time(nShiftDate,12*3600);
	}
}

CTG_ScheduleInfo::CTG_ScheduleInfo(const CTG_ScheduleInfo &src)
{
	m_nType=src.m_nType;
	m_nSectionID=src.m_nSectionID;
	m_tStartTime=src.m_tStartTime;
	m_tEndTime=src.m_tEndTime;
	m_nShiftID=src.m_nShiftID;

	m_nAutoMove=src.m_nAutoMove;
	m_nTotelLen=src.m_nTotelLen;
	m_nHistoryLen=src.m_nHistoryLen;
}
CTG_ScheduleInfo& CTG_ScheduleInfo::operator= (const CTG_ScheduleInfo &src)
{
	m_nType=src.m_nType;
	m_nSectionID=src.m_nSectionID;
	m_tStartTime=src.m_tStartTime;
	m_tEndTime=src.m_tEndTime;
	m_nShiftID=src.m_nShiftID;

	m_nAutoMove=src.m_nAutoMove;
	m_nTotelLen=src.m_nTotelLen;
	m_nHistoryLen=src.m_nHistoryLen;
	return *this;
}

bool CTG_ScheduleInfo::IsEqualScheduleInfo(const CTG_ScheduleInfo &src)
{
	if((m_nType!=src.m_nType) || (m_nSectionID!=src.m_nSectionID ))
		return false;
	return true;
}
    
const char  *CTG_ScheduleInfo::GetShiftName()
{
	static char tmp[128];
	long id=m_nShiftID;
	int CurYear  = id/1000000;
	int CurMonth = id/10000-CurYear*100;
	int CurDay   = id/100-(id/10000)*100;
    int CurBanbie=id-(id/10)*10;
    char *bbname=(CurBanbie==1) ? "第一班" : "第二班";
	sprintf(tmp,"%4d年%02d月%02d日 %s",CurYear,CurMonth,CurDay,bbname);
	return tmp;
}

void CTG_ScheduleInfo::GetShiftManagerRange(TIME &start,TIME &end) //实际操作时间
{
	long id=m_nShiftID;
	int CurYear  = id/1000000;
	int CurMonth = id/10000-CurYear*100;
	int CurDay   = id/100-(id/10000)*100;
	int CurBanbie=id-(id/10)*10;
	struct tm  t;
	t.tm_year=CurYear-1900;
	t.tm_mon=CurMonth-1;
	t.tm_mday=CurDay;
	t.tm_hour=0;
	t.tm_min=0;
	t.tm_sec=0;
	time_t curday0=mktime(&t);
	if(CurBanbie==1)
	{
		start=curday0-6*3600;
		end=curday0+6*3600;
	}
	else
	{
		start=curday0+6*3600;
		end=curday0+18*3600;
	}
}

void  CTG_ScheduleInfo::GetViewScopeRange(TIME &start,TIME &end) const
{
	if(gpcdef.IsNewAutoMoveMode())
	{
		if(IS_BASIC_SCHD(m_nType))
		{
			start = m_tStartTime;
			end =  m_tEndTime;
		}
		else
		{
			if(TRUE==m_nAutoMove)
			{
				TIME ctm=time(NULL);
				ctm=ctm-ctm%60;
				start = ctm - m_nHistoryLen*3600;
				end   = start + m_nTotelLen*3600;
			}
			else
			{
				start = m_tStartTime+12*3600;
				end   = m_tStartTime+24*3600;
			}
		}
	}
	else
	{
		start=m_tStartTime;
		end=m_tEndTime;
	}
}

void CTG_ScheduleInfo::GetTermAcceptSchdTimeRange(TIME& nStartTime, TIME& nEndTime) const
{
	if(gpcdef.IsNewAutoMoveMode())
	{
		if(IS_BASIC_SCHD(m_nType) || m_nType==CTG_ScheduleInfo::SCHEDULE_TYPE_WORK_REQ)
		{
			nStartTime = m_tStartTime;
			nEndTime =  m_tEndTime;
		}
		else
		{
			GetViewScopeRange(nStartTime, nEndTime);
			if(m_nType==CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT)
			{
				nEndTime = nEndTime+12*3600;  // 用于日班计划调度范围-需要增加12小时
			}
			else
			{
				nEndTime = nEndTime+6*3600;  // 对于调整图调度范围-需要增加6小时
			}
		}
	}
	else
	{
		nStartTime = m_tStartTime;
		nEndTime =  m_tEndTime;
	}
}

void CTG_ScheduleInfo::ShiftSchedule(bool bIgnore)
{
	if(gpcdef.IsNewAutoMoveMode())
	{
		if(IS_BASIC_SCHD(m_nType))
		{
			m_nAutoMove=FALSE;
			m_nHistoryLen=0;
			m_nTotelLen=24;
		}
		else if(m_nType==SCHEDULE_TYPE_WORK || m_nType==SCHEDULE_TYPE_SHIFT)
		{
			int delaytm=4*3600; // 延长2小时
			if(bIgnore)
			{
				delaytm=6*3600; 
			}
			TIME nStartTime=m_tStartTime+12*3600;
			TIME nEndTime=m_tEndTime-6*3600;
			TIME ctm=time(NULL);
			if(nStartTime<=ctm && ctm<nEndTime)
			{
				m_nAutoMove=TRUE;
				m_nHistoryLen=gpcdef.GetHistoryHourLen();
				m_nTotelLen=24;
			}
			else if(nEndTime<=ctm && ctm<m_tEndTime-delaytm)
			{
				m_nAutoMove=TRUE;
				m_nHistoryLen=gpcdef.GetHistoryHourLen();
				m_nTotelLen=24;
			}
			else
			{
				m_nAutoMove=FALSE;
				m_nHistoryLen=0;
				m_nTotelLen=12;
			}
		}
		else
		{
			m_nAutoMove=FALSE;
			m_nHistoryLen=0;
			m_nTotelLen=12;
		}
		CTime btm(m_tStartTime), etm(m_tEndTime);
		GpcLogTrainMessage(0, 0, "[ShiftSchedule] schd_type %d [%s-%s] AutoMove %d History %d Totel %d.",
			m_nType, btm.Format("%Y-%m-%d %H:%M:%S"), etm.Format("%Y-%m-%d %H:%M:%S"), m_nAutoMove, m_nHistoryLen, m_nTotelLen);
	}
}

long CTG_ScheduleInfo::Get_CurrentShiftid()
{
	return ::Get_CurrentShiftid();
	
}

long CTG_ScheduleInfo::Get_NextShiftid(long CurShiftid)
{ 
	return ::Get_NextShiftid(CurShiftid);
}

long CTG_ScheduleInfo::Get_PreShiftid(long CurShiftid)
{
	return ::Get_PreShiftid(CurShiftid);
	
}

long CTG_ScheduleInfo::Get_ShiftDate(long shift_id)
{
    int nShiftDate = (shift_id/1000000 - 1990) * 380 +
					(shift_id/10000-(shift_id/1000000)*100 - 1) * 31 + 
					shift_id/100 - (shift_id/10000)*100  - 1 ;

	int nShiftNo = shift_id - shift_id/100*100;
	//if(nShiftNo == 2)
	//   return  nShiftDate;
	//else
	   return  OsGetPrevDate(0,nShiftDate);
}

CTG_ScheduleInfo::SCHEDULE_TYPE  CTG_ScheduleInfo::ScheduleTypeConvert(int type)
{
	// 归档基本图
	if(21<=type&&type<33)
		return CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_BACKUP;
	return (CTG_ScheduleInfo::SCHEDULE_TYPE)type;
}

TIME CTG_ScheduleInfo::GetStartShiftTime(TIME ltime)
{
	TIME result;
	long utime, udate;

	if (NULL_TIME == ltime)
		ltime = time(NULL);

	utime= minute_time(ltime, &udate);
	utime=utime/60;
	if ((utime >= 6*60) && (utime < 18*60)) 
	{
		utime=6*60;
		result= long_time(udate, utime*60);
	}
	else if (utime < 6*60)  // 00:00 -- 6:00
	{
		utime= 18*60;
		result= long_time(udate, utime*60) - 24*3600;
	}
	else  // 18:00 -- 23:59
	{
		utime= 18*60;
		result= long_time(udate, utime*60);
	}

	return(result);
}

CTG_LogicObject::CTG_LogicObject()
{
	m_flagDelete=FALSE;
	m_nAlarmStatus=0;
	for(int i=0;i<SYCFLAG_MAX;i++)
		m_tSyncStartTime[i]=0;
}

CTG_LogicObject& CTG_LogicObject::operator= (const CTG_LogicObject &src)
{
	int i;
	m_flagDelete=src.m_flagDelete;      // 被删除的对象
	m_nAlarmStatus=src.m_nAlarmStatus; 
	for(i=0;i<SYCFLAG_MAX+1;i++)
        m_tSyncStartTime[i]=src.m_tSyncStartTime[i];     // 非0表示正在同步中，启动同步的时间
	m_flagNeedToSync.SetLongValue(src.m_flagNeedToSync.GetLongValue()) ;  // 32 BIT 表示 0-31个接受者的同步请求,是否应该同步
	m_flagSyncStatus.SetLongValue(src.m_flagSyncStatus.GetLongValue()) ;  // 32 BIT 表示 0-31个接受者的同步状态
	return *this;
}

void  CTG_LogicObject::StartSyncing(int sync_flag)
{
	if(!CHECK_VALID_SYNCFLAG(sync_flag))
		return;
	if(sync_flag==SYCFLAG_NULL)
		return;
	if(sync_flag==SYCFLAG_ALL)
	{
		for(int i=0;i<SYCFLAG_MAX;i++)
			m_tSyncStartTime[i]=time(NULL);
	}	
	else
		m_tSyncStartTime[sync_flag]=time(NULL);
}

void  CTG_LogicObject::StopSyncing(int sync_flag)
{
	if(!CHECK_VALID_SYNCFLAG(sync_flag))
		return;
	if(sync_flag==SYCFLAG_NULL)
		return;
	if(sync_flag==SYCFLAG_ALL)
	{
		for(int i=0;i<SYCFLAG_MAX;i++)
			m_tSyncStartTime[i]=0;
	}	
	else
		m_tSyncStartTime[sync_flag]=0;
}

BOOL    CTG_LogicObject::IsSyncing(int sync_flag) 
{
	if(!CHECK_VALID_SYNCFLAG(sync_flag))
		return FALSE;
	if(sync_flag==SYCFLAG_NULL)
		return FALSE;
	if(sync_flag==SYCFLAG_ALL)
		return FALSE;

	if(CheckSyncStatus(sync_flag)) // 已经同步，所以...
	{
			m_tSyncStartTime[sync_flag]=0;
			return TRUE; 
	}

	if(m_tSyncStartTime[sync_flag]>0)
	{
		time_t curtm=time(NULL);
		if(curtm-m_tSyncStartTime[sync_flag]<MAX_SYNC_TIME)
			return TRUE;
		m_tSyncStartTime[sync_flag]=0;
		return FALSE;
	}
	else
		return FALSE;
}

void    CTG_LogicObject::SetNeedSyncFlag(int sync_flag,BOOL setva)
{
	if(!CHECK_VALID_SYNCFLAG(sync_flag))
		return;
	if(sync_flag==SYCFLAG_NULL)
		return;
	if(sync_flag==SYCFLAG_ALL)
		m_flagNeedToSync.SetAllBitStatus(setva);
	else
		m_flagNeedToSync.SetBit(sync_flag,setva);
}

void    CTG_LogicObject::SetSyncStatus(int sync_flag,BOOL setva)
{
	if(!CHECK_VALID_SYNCFLAG(sync_flag))
		return;
	if(sync_flag==SYCFLAG_NULL)
		return;
	if(sync_flag==SYCFLAG_ALL)
		m_flagSyncStatus.SetAllBitStatus(setva);
	else
		m_flagSyncStatus.SetBit(sync_flag,setva);
	StopSyncing(sync_flag);  // 当同步状态变化时，设置正在同步标志为0
}

void    CTG_LogicObject::SetSyncStatusX(int sync_flag,BOOL setva)
{
	if(sync_flag!=0)
		sync_flag=sync_flag;
	if(!CHECK_VALID_SYNCFLAG(sync_flag))
		return;
	if(sync_flag==SYCFLAG_ALL)
		return;
	for(int i=0;i<=SYCFLAG_MAX;i++)
	{
		if(i==sync_flag)
			SetSyncStatus(i,setva);
		else
			SetSyncStatus(i,!setva);
	}
}

BOOL    CTG_LogicObject::CheckMultiSyncStatus(unsigned  long multi_sync_flag) const
	{
		if(m_flagSyncStatus.CheckMultiBitValue(multi_sync_flag,1))
			return TRUE;
		else
			return FALSE;
	}

BOOL    CTG_LogicObject::CheckSyncStatus(int sync_flag) const
	{
		BOOL retv=TRUE;
		if(sync_flag==SYCFLAG_NULL)
			return FALSE;
		if(sync_flag==SYCFLAG_ALL)
		{
			if(!(m_flagSyncStatus.CheckMultiBitValue(m_flagNeedToSync.GetLongValue(),1)))
				retv=FALSE;
		}
		else
		{
			if(m_flagSyncStatus.GetBit(sync_flag)==0)
			{
				retv=FALSE;
		
			}
		}
		
		return retv;
	}

void CTG_LogicObject:: SetAlarmStatus(unsigned long statusbit,BOOL value)
{
	if(value)
		m_nAlarmStatus|=statusbit;
	else
		m_nAlarmStatus&=~statusbit;
}

CTG_TrainInfo::CTG_TrainInfo()
{
	memset(&m_data,0,sizeof(m_data));
	m_data.delete_flag=-1; 
	m_info_expand.Init();
}

CTG_TrainInfo::CTG_TrainInfo(const CTG_TrainInfo &src)
{
	m_data=src.m_data;
	m_info=src.m_info; 
	m_info_expand=src.m_info_expand;
}

CTG_TrainInfo& CTG_TrainInfo::operator=(const CTG_TrainInfo &src)
{
	if(this == &src)
		return *this;
	CTG_LogicObject::operator=(src);
	m_data=src.m_data;
	m_info=src.m_info;
	m_info_expand=src.m_info_expand;
	return *this;
}

BOOL CTG_TrainInfo::operator== (const CTG_TrainInfo &src) const // NA??? 需要重新按成员逐一比较
{
	if(memcmp(&m_data,&src.m_data ,sizeof(m_data)))
		return FALSE;
	if(m_info!=src.m_info)
		return FALSE;
	if(!m_info_expand.IsSame(src.m_info_expand))
		return FALSE;
	return TRUE;
}

bool CTG_TrainInfo::IsTrainInfoExpandSame(const CTG_TrainInfo& src) const
{ 
	return m_info_expand.IsSame(src.m_info_expand);
}

//////////////////////////////////////////////////////////////
CTG_TrainEngineInfo::CTG_TrainEngineInfo()
{
	memset(&m_data,0,sizeof(m_data));
}

CTG_TrainEngineInfo::CTG_TrainEngineInfo(const CTG_TrainEngineInfo &src)
{
	m_data=src.m_data;
}

CTG_TrainEngineInfo& CTG_TrainEngineInfo::operator=(const CTG_TrainEngineInfo &src)
{
	m_data=src.m_data;
	return *this;
}

BOOL CTG_TrainEngineInfo::operator== (const CTG_TrainEngineInfo &src) const  // NA??? 需要重新按成员逐一比较
{
	if(memcmp(&m_data,&src.m_data ,sizeof(m_data)))
		return FALSE;
	else
		return TRUE;
}

const char *CTG_TrainEngineInfo::GetEngineName() const
{
	static char tmpstr[24];
	memset(tmpstr,0,sizeof(tmpstr)-1);
	if(strlen(m_data.engine_id)<=3)
		return m_data.engine_id;   // 不是标准的XXX....的类型？
	strncpy(tmpstr,m_data.engine_type,sizeof(tmpstr)-1);
	strncat(tmpstr,"-",sizeof(tmpstr)-1);
	strncat(tmpstr,m_data.engine_id+3,sizeof(tmpstr)-1);
	return tmpstr;
}

const char *CTG_TrainEngineInfo::GetEngineID() const
{
	return m_data.engine_id; 
}

bool CTG_TrainRecord::HaveWarnInfo()
{
	return m_stplan_warning.GetCount()>0?true:false;
}

void CTG_TrainRecord::ClearStplanWarnInfo()
{
	m_stplan_warning.RemoveAll();
	m_stplan_warning_id.RemoveAll();
}

void CTG_TrainRecord::AddStplanWarnInfo(WORD id, CString& text)
{
	m_stplan_warning.Add(text);
	m_stplan_warning_id.Add(id);
}

bool CTG_TrainRecord::GetStplanWarnInfo(int index, WORD& id, CString& text)
{
	if(index<m_stplan_warning.GetCount() && index<m_stplan_warning_id.GetCount())
	{
		id=m_stplan_warning_id.GetAt(index);
		text=m_stplan_warning.GetAt(index);
		return true;
	}

	return false;
}

void CTG_TrainRecord::Init()
{
	memset(&m_data,0,sizeof(m_data));
	memset(&m_stop_out_of_signal,0,sizeof(m_stop_out_of_signal));
	m_record_expand.Init();
	m_strStationText.empty();
	m_strAbsText.empty(); 
	m_strAlarmText.empty(); 
	m_strNoteText.empty();
	rec_index = 0;
	m_nAlarmStatus=0;
	m_modify_abs_time=FALSE;
	m_stplan_warning.RemoveAll();
	m_stplan_warning_id.RemoveAll();
	tdms_entry=0;
	tdms_exit=0;
}

CTG_TrainRecord::CTG_TrainRecord()
{
	Init();
}

CTG_TrainRecord::CTG_TrainRecord(const CTG_TrainRecord &src)
{
	rec_index = src.rec_index;
	m_data=src.m_data;
	m_record_expand=src.m_record_expand;
	m_stop_out_of_signal=src.m_stop_out_of_signal;
	m_listAbsStop.assign(src.m_listAbsStop.begin(),src.m_listAbsStop.end());

	m_strStationText=src.m_strStationText; 

	m_strAbsText=src.m_strAbsText;
	m_nAlarmStatus=src.m_nAlarmStatus;
	m_strAlarmText=src.m_strAlarmText; 
	m_strNoteText=src.m_strNoteText;
	m_modify_abs_time=src.m_modify_abs_time;
	tdms_entry=src.tdms_entry;
	tdms_exit=src.tdms_exit;
}

CTG_TrainRecord& CTG_TrainRecord::operator=(const CTG_TrainRecord &src)
{
	rec_index = src.rec_index;
	m_data=src.m_data;
	m_record_expand=src.m_record_expand;
	m_stop_out_of_signal=src.m_stop_out_of_signal;
	m_listAbsStop.assign(src.m_listAbsStop.begin(),src.m_listAbsStop.end());

	m_strStationText=src.m_strStationText;   // point to g_pTextArray

	m_strAbsText=src.m_strAbsText;
	m_nAlarmStatus=src.m_nAlarmStatus;
	m_strAlarmText=src.m_strAlarmText;
	m_strNoteText=src.m_strNoteText;
	m_modify_abs_time=src.m_modify_abs_time;
	tdms_entry=src.tdms_entry;
	tdms_exit=src.tdms_exit;
	return *this;
}

//BOOL CTG_TrainRecord::operator== (const CTG_TrainRecord &src) const // NA??? 需要重新按成员逐一比较
//{
//	if(memcmp(&m_data,&src.m_data ,sizeof(m_data)))
//		return FALSE;
//	
//	if(memcmp(&m_stop_out_of_signal,&src.m_stop_out_of_signal ,sizeof(m_stop_out_of_signal)))
//		return FALSE;
//
//	if(m_strStationText!=src.m_strStationText)
//		return FALSE;
//
//	if(m_strAbsText!=src.m_strAbsText)
//		return FALSE;
//
//	return TRUE;
//}

BOOL CTG_TrainRecord::operator== (const CTG_TrainRecord &src0) const // NA??? 需要重新按成员逐一比较
{
	CTG_TrainRecord obj = *this;
	CTG_TrainRecord src = src0;

	// 字符串逐项比较
	int max_len = sizeof(TRAIN_NO);
	if(memcmp((char*)obj.m_data.arrive_train_id, (char*)src.m_data.arrive_train_id, max_len))
	{
		CString tmp=obj.m_data.arrive_train_id;
		memset((char*)m_data.arrive_train_id, 0, max_len);
		strncpy((char*)m_data.arrive_train_id, tmp.GetBuffer(), max_len-1);
	}
	if(strcmp(obj.m_data.arrive_train_id, src.m_data.arrive_train_id))
		return FALSE;

	memset(obj.m_data.arrive_train_id, 0, max_len);
	memset(src.m_data.arrive_train_id, 0, max_len);


	if(memcmp((char*)obj.m_data.depart_train_id, (char*)src.m_data.depart_train_id, max_len))
	{
		CString tmp=obj.m_data.depart_train_id;
		memset((char*)m_data.depart_train_id, 0, max_len);
		strncpy((char*)m_data.depart_train_id, tmp.GetBuffer(), max_len-1);
	}
	if(strcmp(obj.m_data.depart_train_id, src.m_data.depart_train_id))
		return FALSE;

	memset(obj.m_data.depart_train_id, 0, max_len);
	memset(src.m_data.depart_train_id, 0, max_len);


	if(memcmp((char*)obj.m_data.original_arrive_train_id, (char*)src.m_data.original_arrive_train_id, max_len))
	{
		CString tmp=obj.m_data.original_arrive_train_id;
		memset((char*)m_data.original_arrive_train_id, 0, max_len);
		strncpy((char*)m_data.original_arrive_train_id, tmp.GetBuffer(), max_len-1);
	}
	if(strcmp(obj.m_data.original_arrive_train_id, src.m_data.original_arrive_train_id))
		return FALSE;

	memset(obj.m_data.original_arrive_train_id, 0, max_len);
	memset(src.m_data.original_arrive_train_id, 0, max_len);

	
	if(memcmp((char*)obj.m_data.original_depart_train_id, (char*)src.m_data.original_depart_train_id, max_len))
	{
		CString tmp=obj.m_data.original_depart_train_id;
		memset((char*)m_data.original_depart_train_id, 0, max_len);
		strncpy((char*)m_data.original_depart_train_id, tmp.GetBuffer(), max_len-1);
	}
	if(strcmp(obj.m_data.original_depart_train_id, src.m_data.original_depart_train_id))
		return FALSE;

	memset(obj.m_data.original_depart_train_id, 0, max_len);
	memset(src.m_data.original_depart_train_id, 0, max_len);

	if(memcmp(&obj.m_data, &src.m_data, sizeof(obj.m_data)))
		return FALSE;
	
	if(memcmp(&obj.m_stop_out_of_signal,&src.m_stop_out_of_signal ,sizeof(m_stop_out_of_signal)))
		return FALSE;

	if(m_strStationText!=src.m_strStationText)
		return FALSE;

	if(m_strAbsText!=src.m_strAbsText)
		return FALSE;

	if(m_strNoteText!=src.m_strNoteText)
		return FALSE;

	if(!m_record_expand.IsSame(src.m_record_expand))
		return FALSE;
	return TRUE;
}

ENTRY_ID  CTG_TrainRecord::GetEntryNumber(TRAIN_IN_OUT in_out) const
{
	if(TRAIN_OUT_STATION == in_out)
		return m_data.exit;

	return m_data.entry;
}

BOOL   CTG_TrainRecord::IfInTimeScope(TIME stime, TIME etime) const
{
	if(m_data.arrive>=stime && m_data.arrive<=etime)
			return TRUE;
	if(m_data.depart>=stime && m_data.depart<=etime)
		    return TRUE;
	if(m_data.arrive<=stime && m_data.depart>=etime)
		    return TRUE;
	return FALSE;
}

bool CTG_TrainRecord::BasicTrainIfInTimeScope(int banbie) const
{
	TIME arrive=(m_data.arrive)%86400;
	TIME depart=(m_data.depart)%86400;
	// 第二班
	if(banbie==2)
	{
		TIME stime=6*3600, etime=18*3600;
		if(arrive>=stime && arrive<=etime)
			return true;
		if(depart>=stime && depart<=etime)
			return true;
		if(arrive<=stime && depart>=etime)
			return true;
	}
	else
	{
		TIME stime=0, etime=6*3600;
		if(arrive>=stime && arrive<=etime)
			return true;
		if(depart>=stime && depart<=etime)
			return true;
		if(arrive<=stime && depart>=etime)
			return true;

		stime=18*3600, etime=24*3600-1;
		if(arrive>=stime && arrive<=etime)
			return true;
		if(depart>=stime && depart<=etime)
			return true;
		if(arrive<=stime && depart>=etime)
			return true;
	}
	return false;
}

const  char *CTG_TrainRecord::GetStationText(int flag) const 
{ 
     
  if(flag==STATION_TXT_INFO_TYPE_SHUAIGUA) 

    {
		return GetXMLAttribStringValue(m_strStationText.c_str(),"shginfo"); 

    } 
  
  else if(flag==STATION_TXT_INFO_TYPE_OPMS) 

    { 
     
            return GetXMLAttribStringValue(m_strStationText.c_str(),"opmsinfo"); 
     
    } 

  else 
       return NULL; 

} 

const char *CTG_TrainRecord::GetOpmsSationTextState() const
{
	return GetXMLAttribStringValue(m_strStationText.c_str(),"state");
}

bool CTG_TrainRecord::IsOpmsSetSationText()
{
	const char *tmp = GetXMLAttribStringValue(m_strStationText.c_str(),"state"); 

	if(NULL == tmp)
	{
		return true;
	}
	else
	{
		CString state = tmp;
		if(state == "1") //  1 OPMS给的值 0 调度员给的值
			return true;
		else
			return false;
	}
}

void CTG_TrainRecord::SetOpmsSationTextState(const char *str)
{
	CString text = str, stationTest = m_strStationText.c_str();
	if(!stationTest.IsEmpty())
	   m_strStationText = SetXMLAttribStringValue(m_strStationText.c_str(),"state",str,1); 
}



void  CTG_TrainRecord::SetStationText(const char *str,int flag) 
{ 
	CString text = str;
	CString tmpshg="";
    
	if(flag==STATION_TXT_INFO_TYPE_SHUAIGUA) 
    { 
		const char* chtext = GetStationText(STATION_TXT_INFO_TYPE_SHUAIGUA);
		if(NULL != chtext)
		{
			tmpshg = chtext;
		}

		if(!text.IsEmpty() || (text.IsEmpty() && !tmpshg.IsEmpty()))
	         m_strStationText=SetXMLAttribStringValue(m_strStationText.c_str(),"shginfo",str,1);
    } 

    else if(flag==STATION_TXT_INFO_TYPE_OPMS) 
    { 

		const char* chtext = GetStationText(STATION_TXT_INFO_TYPE_OPMS);
		if(NULL != chtext)
		{
			tmpshg = chtext;
		}

		if(!text.IsEmpty() || (text.IsEmpty() && !tmpshg.IsEmpty()))
			m_strStationText=SetXMLAttribStringValue(m_strStationText.c_str(),"opmsinfo",str,1);
    } 

} 
int  CTG_TrainRecord::GetArriveTGIndex() const
{
	// qiaoyt 20090805 无效返回值是-1, 避免255
	int index = (m_data.late_reason>>24)&0xFF;
	if (index == 255) 
	{
		index =-1;
	}
	return index;
}
int  CTG_TrainRecord::GetDepartTGIndex() const
{
	// qiaoyt 20090805 无效返回值是-1, 避免255
	int index = (m_data.late_reason>>16)&0xFF;
	if (index == 255) 
	{
		index =-1;
	}
	return index;
}
void CTG_TrainRecord::SetArriveTGIndex(int va)
{
	if(va>=0 && va<255)
	{
		m_data.late_reason=(((long)va<<24)|(m_data.late_reason & 0x00FFFFFF));
	}
	else if(va == -1 || va == 255)
	{
		m_data.late_reason |= ~0x00FFFFFF;
	}

	return;
}
void  CTG_TrainRecord::SetDepartTGIndex(int va)
{
	if(va>=0 && va<255)
	{
		m_data.late_reason=(((long)va<<16)|(m_data.late_reason & 0xFF00FFFF));
	}
	else if(va == -1  || va == 255)
	{
		m_data.late_reason |= ~0xFF00FFFF;
	}

	return;
	
}

BOOL       CTG_TrainRecord::HasImportantFlag() const
{
	if(IsStationSpace())
		return TRUE;
	if(TG_IsSpecialGaurdRank(m_data))
		return TRUE;
    if(TG_IsGoustRank(m_data))
		return TRUE;
    if(TG_IsMilitaryRank(m_data))
		return TRUE;
	return FALSE;
}

void  CTG_TrainRecord::SetRecordKK(const TG_TrainOPMSRecordInfo& opms)
{
	if(GetTGFunctionConfig()->IsEnableTDMS5_0())
	{
		m_record_expand.work_flag=opms.work_flag;
		m_record_expand.oper_flag=opms.oper_flag;
		m_record_expand.important_flag=opms.important_flag;
		m_record_expand.oper_info=opms.oper_info;
		m_record_expand.note_text=opms.note_text;
	}
}

//////////////////////////////////////////////////////////////////////////////////
CTG_TrainRecordList::CTG_TrainRecordList()
{
}
CTG_TrainRecordList::CTG_TrainRecordList(const CTG_TrainRecordList &src)
{
	m_listRecords.assign(src.m_listRecords.begin(),src.m_listRecords.end());
}
CTG_TrainRecordList& CTG_TrainRecordList::operator=(const CTG_TrainRecordList &src)
{
	CTG_LogicObject::operator=(src);
	m_listRecords.assign(src.m_listRecords.begin(),src.m_listRecords.end());
	return *this;
}

BOOL CTG_TrainRecordList::operator== (const CTG_TrainRecordList &src) const // NA??? 需要重新按成员逐一比较
{
	if(m_listRecords.size()!=src.m_listRecords.size())
		return FALSE;
	vector<CTG_TrainRecord>::const_iterator iter1=m_listRecords.begin();
	vector<CTG_TrainRecord>::const_iterator iter2=src.m_listRecords.begin();
	while(iter1!= m_listRecords.end()) 
	{
		if(!(*iter1==*iter2))
			return FALSE;
		iter1++;
		iter2++;
	}
	return TRUE;
}
int   CTG_TrainRecordList::GetRecordByStation(int station) const
{
	for(int i=0;i<m_listRecords.size();i++)
	{
		if(m_listRecords[i].m_data.station==station)
			return i;
	}
	return -1;
}

void  CTG_TrainRecordList::DeleteRecord(int index)
{
	if(index>=0 && index<m_listRecords.size())
	{
		m_listRecords.erase(m_listRecords.begin()+index);
	}
}

CTG_TrainEngineList::CTG_TrainEngineList()
{
}
CTG_TrainEngineList::CTG_TrainEngineList(const CTG_TrainEngineList &src)
{
	m_listRecords.assign(src.m_listRecords.begin(),src.m_listRecords.end());
}
CTG_TrainEngineList& CTG_TrainEngineList::operator=(const CTG_TrainEngineList &src)
{
	CTG_LogicObject::operator=(src);
	m_listRecords.assign(src.m_listRecords.begin(),src.m_listRecords.end());
	return *this;
}

BOOL CTG_TrainEngineList::operator== (const CTG_TrainEngineList &src) const // NA??? 需要重新按成员逐一比较
{
	if(m_listRecords.size()!=src.m_listRecords.size())
		return FALSE;
	vector<CTG_TrainEngineInfo>::const_iterator iter1=m_listRecords.begin();
	vector<CTG_TrainEngineInfo>::const_iterator iter2=src.m_listRecords.begin();
	while(iter1!= m_listRecords.end()) 
	{
		if(!(*iter1==*iter2))
			return FALSE;
		iter1++;
		iter2++;
	}
	return TRUE;
}

CTG_SingleMark::CTG_SingleMark()
{
	 m_nBlockIndex=0;
	 memset(&m_data,0,sizeof(m_data));
	 m_bSelfUpdate=FALSE;
}

CTG_SingleMark::CTG_SingleMark(const CTG_SingleMark &src)
{
	m_nBlockIndex=src.m_nBlockIndex;
	m_data=src.m_data; 
	m_info=src.m_info; 
	m_bSelfUpdate=src.m_bSelfUpdate;
}

BOOL CTG_SingleMark::IsLocalCreated() const
{
	return TRUE;
}
CTG_SingleMark& CTG_SingleMark::operator= (const CTG_SingleMark &src)
{
	CTG_LogicObject::operator=(src);
	m_nBlockIndex=src.m_nBlockIndex;
	m_data=src.m_data; 
	m_info=src.m_info;
	return *this;
}

BOOL CTG_SingleMark::operator== (const CTG_SingleMark &src) const
{
	if(m_nBlockIndex!=src.m_nBlockIndex)
		return FALSE;
	if(memcmp(&m_data,&src.m_data,sizeof(m_data)))
		return FALSE;
	if(m_info!=src.m_info)
		return FALSE;
	return TRUE;
}

BOOL  CTG_SingleMark::IfInTimeScope(TIME stime, TIME etime) const
{
	if(m_data.start_time>=stime && m_data.start_time<=etime)
		return TRUE;
	if(m_data.end_time>=stime && m_data.end_time<=etime)
		return TRUE;
	if(m_data.start_time<=stime && m_data.end_time>=etime)
		return TRUE;
	return FALSE;
}

BOOL  CTG_SingleMark::IsStationMarkType() const
{
	if(TG_IS_INSTATION_MARK(GetMarkType()))
		return TRUE;
	else
		return FALSE;
}

BOOL  CTG_SingleMark::IsAbsMarkType() const
{
	if(TG_IS_ABS_MARK(GetMarkType()))
		return TRUE;
	else
		return FALSE;
}

int   CTG_SingleMark::GetStation() const
{
	if(IsStationMarkType())
		return m_data.station;
	else
		return -1;

}
int   CTG_SingleMark::GetStation2() const
{
	return -1;
}

BOOL  CTG_SingleMark::MatchStationSide(int station1,int side1) const 
{
	if(!IsStationMarkType())
		return FALSE;
	if(m_data.station!=station1)
		return FALSE;
	for(int j=0; j<m_data.ele_count; j++)
		{
			SIDE_ID x = (SIDE_ID) m_data.ele_number[j];
			if(x==side1)
				return TRUE;
		}
	return FALSE;
}

BOOL  CTG_SingleMark::MatchAbsNumber(int absnum) const
{
	if(!IsAbsMarkType())
		return FALSE;
	for(int j=0; j<m_data.ele_count; j++)
	{
		int x = m_data.ele_number[j];
		if(x==absnum)
			return TRUE;
	}
	return FALSE;
}

BOOL  CTG_SingleMark::MatchStation(int station1,int station2) const
{
	if(station1==station2 && station1==m_data.station)
		return TRUE;
	if(m_data.station1==station1 && m_data.station2==station2 || m_data.station1==station2 && m_data.station2==station1)
		return TRUE;
	else
		return FALSE;

}

const char *CTG_SingleMark::GetMarkID() const
{
	static char tmp[64];
	sprintf(tmp,"%d",GetBlockIndex());
	return tmp;
}

const char *CTG_SingleMark::GetMarkInfo(int flag) const
{
	if(flag==MARK_TXT_INFO_TYPE_INSIDE)
		return m_data.text;
	else
		return m_info.c_str(); // TMP! 
}

void  CTG_SingleMark::SetMarkInfo(const char *infostr,int flag) 
{
	if(flag==MARK_TXT_INFO_TYPE_INSIDE)
	{
		strncpy(m_data.text,infostr,sizeof(m_data.text)-1);
	}
	else
		m_info=infostr; // TMP!
}


//////////////////////////////////////////////////////////
CTG_TrainRelation::CTG_TrainRelation()
{
	Init();
}
void CTG_TrainRelation::Init()
{
	prev_train_index=0;
	prev_basic_index=0;
	prev_shift_index=0;
	next_shift_index=0;
	prev_dest_sta=0;
	next_dest_sta=0;
	czh_no_str="";
}
CTG_TrainRelation::CTG_TrainRelation(const CTG_TrainRelation &src)
{
	prev_train_index=src.prev_train_index;
	prev_basic_index=src.prev_basic_index;
	prev_shift_index=src.prev_shift_index;
	next_shift_index = src.next_shift_index;
	prev_dest_sta=src.prev_dest_sta;
	next_dest_sta=src.next_dest_sta;
	czh_no_str=src.czh_no_str;
}

CTG_TrainRelation& CTG_TrainRelation::operator= (const CTG_TrainRelation &src)
{
	if(this == &src)
		return *this;
	prev_train_index=src.prev_train_index;
	prev_basic_index=src.prev_basic_index;
	prev_shift_index=src.prev_shift_index;
	next_shift_index = src.next_shift_index;
	prev_dest_sta=src.prev_dest_sta;
	next_dest_sta=src.next_dest_sta;
	czh_no_str=src.czh_no_str;

	return *this;
}

bool CTG_TrainRelation::IsCoreSame(const CTG_TrainRelation &src) const
{
	if(prev_train_index!=src.prev_train_index)
		return false;
	if(prev_basic_index!=src.prev_basic_index)
		return false;
	if(prev_shift_index!=src.prev_shift_index)
		return false;
	if(next_shift_index!=src.next_shift_index)
		return false;
	if(prev_dest_sta!=src.prev_dest_sta)
		return false;
	if(next_dest_sta!=src.next_dest_sta)
		return false;
	if(czh_no_str!=src.czh_no_str)
		return false;
	return true;
}

//////////////////CTG_TrainKediaoInfo/////////////////////
CTG_TrainKediaoInfo::CTG_TrainKediaoInfo()
{
	gtid=0;
	msgid=0;
	type=0;	//类型 1:客调 2：特调
}

CTG_TrainKediaoInfo::CTG_TrainKediaoInfo(const CTG_TrainKediaoInfo &src)
{
	type=src.type;
	content = src.content;
}

CTG_TrainKediaoInfo& CTG_TrainKediaoInfo::operator=(const CTG_TrainKediaoInfo &src)
{
	if(this == &src)
		return *this;
	type=src.type;
	content = src.content;
	return *this;
}

bool CTG_TrainKediaoInfo::IsCoreSame(const CTG_TrainKediaoInfo &src) const 
{
	if(type!=src.type)
		return false;
	if(content!=src.content)
	{
		GpcLogTrainMessage(0,0,"[err]客调内容不一致:%s", src.content);
		GpcLogTrainMessage(0,0,"[err]客调内容不一致:%s", content);
		return false;
	}
	return true;
}

////////////////////CTG_TrainYingyeList///////////////////
CTG_TrainYingyeList::CTG_TrainYingyeList()
{
	gtid=0;
	msgid=0;
}

CTG_TrainYingyeList::CTG_TrainYingyeList(const CTG_TrainYingyeList &src)
{
	gtid=src.gtid;
	msgid=src.msgid;
	m_listRecords.assign(src.m_listRecords.begin(),src.m_listRecords.end());
}
CTG_TrainYingyeList& CTG_TrainYingyeList::operator= (const CTG_TrainYingyeList &src)
{
	if(this == &src)
		return *this;

	gtid=src.gtid;
	msgid=src.msgid;
	m_listRecords.assign(src.m_listRecords.begin(),src.m_listRecords.end());
	return *this;
}
bool CTG_TrainYingyeList::IsCoreSame(const CTG_TrainYingyeList &src) const
{
	if(m_listRecords.size()!=src.m_listRecords.size())
		return false;
	vector<TG_TrainYingyeInfo>::const_iterator iter1=m_listRecords.begin();
	vector<TG_TrainYingyeInfo>::const_iterator iter2=src.m_listRecords.begin();
	while(iter1!= m_listRecords.end()) 
	{
		if( (CString(iter1->station_name) != CString(iter2->station_name))
			|| (iter1->arrive_time!=iter2->arrive_time) 
			|| (iter1->depart_time!=iter2->depart_time))
			return false;
	
		iter1++;
		iter2++;
	}
	return true;
}
TG_TrainYingyeInfo* CTG_TrainYingyeList::GetRecord(int index)
{
	if(index>=0 && index<m_listRecords.size())
	{
		return &m_listRecords[index];
	}
	return NULL;
}
const TG_TrainYingyeInfo* CTG_TrainYingyeList::GetRecord_Const(int index) const
{
	if(index>=0 && index<m_listRecords.size())
	{
		return &m_listRecords[index];
	}
	return NULL;
}
void CTG_TrainYingyeList::AddRecord(const TG_TrainYingyeInfo& rd) 
{ 
	if(GetTGFunctionConfig()->IsEnableTDMS5_0())
		m_listRecords.push_back(rd);
}
////////////////////CTG_TrainCrewList///////////////////
CTG_TrainCrewList::CTG_TrainCrewList()
{
	gtid=0;
	msgid=0;
}

CTG_TrainCrewList::CTG_TrainCrewList(const CTG_TrainCrewList &src)
{
	m_listRecords.assign(src.m_listRecords.begin(),src.m_listRecords.end());
}
CTG_TrainCrewList& CTG_TrainCrewList::operator= (const CTG_TrainCrewList &src)
{
	if(this == &src)
		return *this;
	m_listRecords.assign(src.m_listRecords.begin(),src.m_listRecords.end());
	return *this;
}
bool CTG_TrainCrewList::IsCoreSame(const CTG_TrainCrewList &src) const
{
	if(m_listRecords.size()!=src.m_listRecords.size())
		return false;
	vector<TG_TrainCrewInfo>::const_iterator iter1=m_listRecords.begin();
	vector<TG_TrainCrewInfo>::const_iterator iter2=src.m_listRecords.begin();
	while(iter1!= m_listRecords.end()) 
	{
		if((iter1->crew_type!=iter2->crew_type)
			|| (CString(iter1->crew_info)!=CString(iter2->crew_info))
			|| (CString(iter1->crew_bureau)!=CString(iter2->crew_bureau)))
			return false;

		iter1++;
		iter2++;
	}
	return true;
}
TG_TrainCrewInfo* CTG_TrainCrewList::GetRecord(int index)
{
	if(index>=0 && index<m_listRecords.size())
	{
		return &m_listRecords[index];
	}
	return NULL;
}
const TG_TrainCrewInfo* CTG_TrainCrewList::GetRecord_Const(int index) const
{
	if(index>=0 && index<m_listRecords.size())
	{
		return &m_listRecords[index];
	}
	return NULL;
}

CString CTG_TrainCrewList::toString() const
{
	CString text, str;
	for(int i=0; i<m_listRecords.size(); i++)
	{
		int crew_type=m_listRecords[i].crew_type;
		
		//0:未知 1：司机	2：机械师	3：乘警	4：保洁	5：餐饮
		if(crew_type==1)
			str="司机:";
		else if(crew_type==2)
			str="机械师:";
		else if(crew_type==3)
			str="乘警:";
		else if(crew_type==4)
			str="保洁:";
		else if(crew_type==5)
			str="餐饮:";
		else
			str="未知:";

		text += str;
		text += m_listRecords[i].crew_info;
		text += m_listRecords[i].crew_bureau;
		text+="\r\n";
	}

	return text;
}

CString CTG_TrainCrewList::toString2() const
{
	CString text, str;
	for(int i=0; i<m_listRecords.size(); i++)
	{
		int crew_type=m_listRecords[i].crew_type;
		if(CString(m_listRecords[i].crew_info)!="" || CString(m_listRecords[i].crew_bureau)!="")
		{
			//0:未知 1：司机	2：机械师	3：乘警	4：保洁	5：餐饮
			if(crew_type==1)
				str="司机:";
			else if(crew_type==2)
				str="机械师:";
			else if(crew_type==3)
				str="乘警:";
			else if(crew_type==4)
				str="保洁:";
			else if(crew_type==5)
				str="餐饮:";
			else
				str="未知:";

			text += str;
			text += m_listRecords[i].crew_info;
			text += m_listRecords[i].crew_bureau;
			text+="\r\n";
		}
	}

	return text;
}

void CTG_TrainCrewList::AddRecord(const TG_TrainCrewInfo& rd) 
{ 
	if(GetTGFunctionConfig()->IsEnableTDMS5_0())
		m_listRecords.push_back(rd);
}
////////////////////CTG_TrainTicketList///////////////////
CTG_TrainTicketList::CTG_TrainTicketList()
{
	gtid=0;
	msgid=0;
}

CTG_TrainTicketList::CTG_TrainTicketList(const CTG_TrainTicketList &src)
{
	m_listRecords.assign(src.m_listRecords.begin(),src.m_listRecords.end());
}
CTG_TrainTicketList& CTG_TrainTicketList::operator= (const CTG_TrainTicketList &src)
{
	if(this == &src)
		return *this;
	m_listRecords.assign(src.m_listRecords.begin(),src.m_listRecords.end());
	return *this;
}
bool CTG_TrainTicketList::IsCoreSame(const CTG_TrainTicketList &src) const
{
	if(m_listRecords.size()!=src.m_listRecords.size())
		return false;
	vector<TG_TrainTicketInfo>::const_iterator iter1=m_listRecords.begin();
	vector<TG_TrainTicketInfo>::const_iterator iter2=src.m_listRecords.begin();
	while(iter1!= m_listRecords.end()) 
	{
		if( (CString(iter1->on_station_name) != CString(iter2->on_station_name))
			|| (CString(iter1->down_station_name) != CString(iter2->down_station_name))
			|| (iter1->on_station_index!=iter2->on_station_index)
			|| (iter1->down_station_index!=iter2->down_station_index)
			|| (iter1->sale!=iter2->sale)
			|| (CString(iter1->seat)!=CString(iter2->seat)) )
			return false;

		iter1++;
		iter2++;
	}
	return true;
}
TG_TrainTicketInfo* CTG_TrainTicketList::GetRecord(int index)
{
	if(index>=0 && index<m_listRecords.size())
	{
		return &m_listRecords[index];
	}
	return NULL;
}
const TG_TrainTicketInfo* CTG_TrainTicketList::GetRecord_Const(int index) const
{
	if(index>=0 && index<m_listRecords.size())
	{
		return &m_listRecords[index];
	}
	return NULL;
}
void CTG_TrainTicketList::AddRecord(const TG_TrainTicketInfo& rd) 
{ 
	if(GetTGFunctionConfig()->IsEnableTDMS5_0())
		m_listRecords.push_back(rd);
}
////////////////////CTG_EngineCrewList///////////////////
CTG_EngineCrewList::CTG_EngineCrewList()
{
	gtid=0;
	msgid=0;
}

CTG_EngineCrewList::CTG_EngineCrewList(const CTG_EngineCrewList &src)
{
	m_listRecords.assign(src.m_listRecords.begin(),src.m_listRecords.end());
}
CTG_EngineCrewList& CTG_EngineCrewList::operator= (const CTG_EngineCrewList &src)
{
	if(this == &src)
		return *this;
	m_listRecords.assign(src.m_listRecords.begin(),src.m_listRecords.end());
	return *this;
}
bool CTG_EngineCrewList::IsCoreSame(const CTG_EngineCrewList &src) const
{
	if(m_listRecords.size()!=src.m_listRecords.size())
		return false;
	vector<TG_EngineCrewInfo>::const_iterator iter1=m_listRecords.begin();
	vector<TG_EngineCrewInfo>::const_iterator iter2=src.m_listRecords.begin();
	while(iter1!= m_listRecords.end()) 
	{
		if((CString(iter1->station_name) != CString(iter2->station_name)) || (iter1->overwork_time != iter2->overwork_time))
			return false;

		iter1++;
		iter2++;
	}
	return true;
}
TG_EngineCrewInfo* CTG_EngineCrewList::GetRecord(int index)
{
	if(index>=0 && index<m_listRecords.size())
	{
		return &m_listRecords[index];
	}
	return NULL;
}
const TG_EngineCrewInfo* CTG_EngineCrewList::GetRecord_Const(int index) const
{
	if(index>=0 && index<m_listRecords.size())
	{
		return &m_listRecords[index];
	}
	return NULL;
}
void CTG_EngineCrewList::AddRecord(const TG_EngineCrewInfo& rd) 
{ 
	if(GetTGFunctionConfig()->IsEnableTDMS5_0())
		m_listRecords.push_back(rd);
}
////////////////////////////////////////////////////////////////////////
CTG_SingleTrain::CTG_SingleTrain()
{
	m_nTrainIndex=0;
	memset(m_cNextTrainName, 0, sizeof(m_cNextTrainName));
	memset(m_cBeforeTrainName, 0, sizeof(m_cBeforeTrainName));
	m_nNextTrainIndex=0;
	m_nBeforeTrainIndex=0;
	m_nAddOrUpdate=0;

	m_curSelRecIdx=-1;
	m_bSelfUpdate=FALSE;
	
	m_bAdjusting=FALSE;
	m_nCurrConTrainIndex=0;
	m_nTipToDdy=0;
}
CTG_SingleTrain::~CTG_SingleTrain()
{
}

CTG_SingleTrain::CTG_SingleTrain(const CTG_SingleTrain &src)
{
	m_nTrainIndex=src.m_nTrainIndex;
	strcpy(m_cNextTrainName, src.m_cNextTrainName);
	strcpy(m_cBeforeTrainName, src.m_cBeforeTrainName);
	m_nNextTrainIndex=src.m_nNextTrainIndex;
	m_nBeforeTrainIndex=src.m_nBeforeTrainIndex;

	m_cTrainInfo=src.m_cTrainInfo;
	m_cEngineList=src.m_cEngineList;
	m_cRecordList=src.m_cRecordList;
	m_bSelfUpdate=src.m_bSelfUpdate;
	m_nAddOrUpdate=src.m_nAddOrUpdate;
	
	m_bAdjusting=src.m_bAdjusting;	
	m_strSource=src.m_strSource;
	m_nCurrConTrainIndex=src.m_nCurrConTrainIndex;
	m_nTipToDdy=src.m_nTipToDdy;
}

CTG_SingleTrain& CTG_SingleTrain::operator= (const CTG_SingleTrain &src)
{
	CTG_LogicObject::operator=(src);
	m_nTrainIndex=src.m_nTrainIndex;
	strcpy(m_cNextTrainName, src.m_cNextTrainName);
	strcpy(m_cBeforeTrainName, src.m_cBeforeTrainName);
	m_nNextTrainIndex=src.m_nNextTrainIndex;
	m_nBeforeTrainIndex=src.m_nBeforeTrainIndex;

	m_cTrainInfo=src.m_cTrainInfo;
	m_cEngineList=src.m_cEngineList;
	m_cRecordList=src.m_cRecordList;
	m_nAddOrUpdate=src.m_nAddOrUpdate;

	m_bAdjusting=src.m_bAdjusting;
	m_strSource=src.m_strSource;
	m_nCurrConTrainIndex=src.m_nCurrConTrainIndex;
	m_nTipToDdy=src.m_nTipToDdy;

	return *this;
}

BOOL CTG_SingleTrain::UpdateTrain(const CTG_SingleTrain &src,int source_flag)
{
	if(src.IsDeletedObject())// 被删除的对象无意义
		return FALSE;
	if(m_nTrainIndex!=src.m_nTrainIndex)
		return FALSE;
	if(m_cTrainInfo==src.m_cTrainInfo)
	{
		m_cTrainInfo.SetSyncStatus(source_flag,TRUE);
	}
	else
	{
		m_cTrainInfo=src.m_cTrainInfo;
		m_cTrainInfo.SetSyncStatusX(source_flag,TRUE);
	}
	if(m_cEngineList==src.m_cEngineList)
	{
		m_cEngineList.SetSyncStatus(source_flag,TRUE);
	}
	else
	{
		m_cEngineList=src.m_cEngineList;
		m_cEngineList.SetSyncStatusX(source_flag,TRUE);
	}
	if(m_cRecordList==src.m_cRecordList)
	{
		m_cRecordList.SetSyncStatus(source_flag,TRUE);
	}
	else
	{
		m_cRecordList=src.m_cRecordList;
		m_cRecordList.SetSyncStatusX(source_flag,TRUE);
	}
	StopSyncing(source_flag);
	return TRUE;
}

BOOL CTG_SingleTrain::operator== (const CTG_SingleTrain &src)  // NA??? 需要重新按成员逐一比较
{
	if(!(m_nTrainIndex==src.m_nTrainIndex))
		return FALSE;
	if(!(m_cTrainInfo==src.m_cTrainInfo))
		return FALSE;
	if(!(m_cEngineList==src.m_cEngineList))
		return FALSE;
	if(!(m_cRecordList==src.m_cRecordList))
		return FALSE;
	return TRUE;
}

BOOL CTG_SingleTrain::CompareCoreData(const CTG_TrainRecordList &src0)  // 比较重要和运行图界面显示的成员
{
	CTG_TrainRecordList src=src0;
	if(src.GetRecordCount()!=m_cRecordList.GetRecordCount()) 
		return FALSE;
	for(int i=0;i<src.GetRecordCount() ;i++)  // 忽略发送计划时间
	{
		CTG_TrainRecord *prec=&src[i];
		CTG_TrainRecord *prec_old=&m_cRecordList[i];
		prec->m_data.send_plan_time=prec_old->m_data.send_plan_time;
	}
	if(src==m_cRecordList)
		return TRUE;
	else
		return FALSE;
}

// 不要内存比较----容易出现问题不停同步问题,导致TD死机
BOOL CTG_SingleTrain::CompareCoreData(const CTG_TrainInfo &src0)  // 比较重要和运行图界面显示的成员
{
	if(!m_cTrainInfo.IsTrainInfoExpandSame(src0))
		return FALSE;
	CTG_TrainInfo src=src0;
	CTG_TrainInfo obj=m_cTrainInfo;
	
	src.m_data.update_time=obj.m_data.update_time; // 忽略更新时间

	// 字符数组逐项比较
	if(memcmp(obj.m_data.train_name, src.m_data.train_name, MAX_TRAIN_NAME_LEN))
	{
		CString strObj, strSrc, tmpch;
		for(int i=0; i<MAX_TRAIN_NAME_LEN; i++)
		{
			tmpch.Format("%0.2X", obj.m_data.train_name[i]);
			strObj += tmpch;
			tmpch.Format("%0.2X", src.m_data.train_name[i]);
			strSrc += tmpch;
		}
		GpcLogTrainMessage(0, 0, "CompareCoreData train_name obj %s src %s", strObj, strSrc);
		
		CString tmp=obj.m_data.train_name;
		memset(m_cTrainInfo.m_data.train_name, 0, MAX_TRAIN_NAME_LEN);
		strncpy(m_cTrainInfo.m_data.train_name, tmp.GetBuffer(), MAX_TRAIN_NAME_LEN-1);
	}
	if(strcmp(obj.m_data.train_name, src.m_data.train_name))
		return FALSE;

	memset(src.m_data.train_name, 0, MAX_TRAIN_NAME_LEN);
	memset(obj.m_data.train_name, 0, MAX_TRAIN_NAME_LEN);
	

	if(memcmp(obj.m_data.shift_string, src.m_data.shift_string, 32))
	{
		CString strObj, strSrc, tmpch;
		for(int i=0; i<32; i++)
		{
			tmpch.Format("%0.2X", obj.m_data.shift_string[i]);
			strObj += tmpch;
			tmpch.Format("%0.2X", src.m_data.shift_string[i]);
			strSrc += tmpch;
		}
		GpcLogTrainMessage(0, 0, "CompareCoreData shift_string obj %s src %s", strObj, strSrc);

		CString tmp=obj.m_data.shift_string;
		memset(m_cTrainInfo.m_data.shift_string, 0, 32);
		strncpy(m_cTrainInfo.m_data.shift_string, tmp.GetBuffer(), 31);
	}
	if(strcmp(obj.m_data.shift_string, src.m_data.shift_string))
		return FALSE;

	memset(src.m_data.shift_string, 0, 32);
	memset(obj.m_data.shift_string, 0, 32);
	

	if(memcmp(obj.m_data.start_sta_name, src.m_data.start_sta_name, MAX_STA_NAME_LEN))
	{
		CString strObj, strSrc, tmpch;
		for(int i=0; i<MAX_STA_NAME_LEN; i++)
		{
			tmpch.Format("%0.2X", obj.m_data.start_sta_name[i]);
			strObj += tmpch;
			tmpch.Format("%0.2X", src.m_data.start_sta_name[i]);
			strSrc += tmpch;
		}
		GpcLogTrainMessage(0, 0, "CompareCoreData start_sta_name obj %s src %s", strObj, strSrc);

		CString tmp=obj.m_data.start_sta_name;
		memset(m_cTrainInfo.m_data.start_sta_name, 0, MAX_STA_NAME_LEN);
		strncpy(m_cTrainInfo.m_data.start_sta_name, tmp.GetBuffer(), MAX_STA_NAME_LEN-1);
	}
	if(strcmp(obj.m_data.start_sta_name, src.m_data.start_sta_name))
		return FALSE;

	memset(src.m_data.start_sta_name, 0, MAX_STA_NAME_LEN);
	memset(obj.m_data.start_sta_name, 0, MAX_STA_NAME_LEN);
	

	if(memcmp(obj.m_data.end_sta_name, src.m_data.end_sta_name, MAX_STA_NAME_LEN))
	{
		CString strObj, strSrc, tmpch;
		for(int i=0; i<MAX_STA_NAME_LEN; i++)
		{
			tmpch.Format("%0.2X", obj.m_data.end_sta_name[i]);
			strObj += tmpch;
			tmpch.Format("%0.2X", src.m_data.end_sta_name[i]);
			strSrc += tmpch;
		}
		GpcLogTrainMessage(0, 0, "CompareCoreData end_sta_name obj %s src %s", strObj, strSrc);

		CString tmp=obj.m_data.end_sta_name;
		memset(m_cTrainInfo.m_data.end_sta_name, 0, MAX_STA_NAME_LEN);
		strncpy(m_cTrainInfo.m_data.end_sta_name, tmp.GetBuffer(), MAX_STA_NAME_LEN-1);
	}
	if(strcmp(obj.m_data.end_sta_name, src.m_data.end_sta_name))
		return FALSE;

	memset(src.m_data.end_sta_name, 0, MAX_STA_NAME_LEN);
	memset(obj.m_data.end_sta_name, 0, MAX_STA_NAME_LEN);

	if(memcmp(&obj.m_data,&src.m_data,sizeof(obj.m_data)))
		return FALSE;
	else
		return TRUE;
}

BOOL CTG_SingleTrain::CompareCoreData(const CTG_SingleTrain &src0)  // 比较重要和运行图界面显示的成员
{
	if(!(m_nTrainIndex==src0.m_nTrainIndex))
		return FALSE;
	if(!CompareCoreData(src0.m_cTrainInfo))
		return FALSE;
	if(!(m_cEngineList==src0.m_cEngineList))
		return FALSE;
	if(!CompareCoreData(src0.m_cRecordList))
		return FALSE;
	return TRUE;
}

BOOL   CTG_SingleTrain::UpdateTrainInfo(const CTG_TrainInfo &traininfo,int source_flag)
{
	if(m_cTrainInfo==traininfo)
	{
		m_cTrainInfo.SetSyncStatus(source_flag,TRUE);
	}
	else
	{
		m_cTrainInfo=traininfo;
		m_cTrainInfo.SetSyncStatusX(source_flag,TRUE);
	}
	StopSyncing(source_flag);
	return TRUE;
}

BOOL   CTG_SingleTrain::UpdateTrainRecords(const CTG_TrainRecordList &trainrecord,int source_flag) //NULL means clear!
{
	if(m_cRecordList==trainrecord)
	{
		m_cRecordList.SetSyncStatus(source_flag,TRUE);
	}
	else
	{
		m_cRecordList=trainrecord;
		m_cRecordList.SetSyncStatusX(source_flag,TRUE);
	}
	StopSyncing(source_flag);
	return TRUE;
}

BOOL   CTG_SingleTrain::UpdateEngineInfo(const CTG_TrainEngineList &trainengine,int source_flag) //NULL means clear!
{
	if(m_cEngineList==trainengine)
	{
		m_cEngineList.SetSyncStatus(source_flag,TRUE);
	}
	else
	{
		m_cEngineList=trainengine;
		m_cEngineList.SetSyncStatusX(source_flag,TRUE);
	}
	StopSyncing(source_flag);
	return TRUE;
}

void  CTG_SingleTrain::SetTrainRecordCountToDefault()
{
	m_cTrainInfo.m_data.delete_flag=m_cRecordList.GetRecordCount();  
	for (int i=0; i<m_cTrainInfo.m_data.delete_flag; i++) // 在此设置新增记录的rec_index  20090818
	{
		CTG_TrainRecord *rec = GetTrainRecord(i);
		rec->rec_index=i; 
	}
}

void  CTG_SingleTrain::SetTrainRecordCount(int x)
{
	for (int i=x; i<m_cTrainInfo.m_data.delete_flag; i++)
	{
		CTG_TrainRecord *rec = GetTrainRecord(i);
		rec->m_data.late_reason = rec->GetIndex();   // 删除后的记录INDEX暂存此处
	}

	m_cTrainInfo.m_data.delete_flag=x;
}
BOOL  CTG_SingleTrain::ComparePrevAndUpateFlags(const CTG_SingleTrain &prev) //// 512计划优化处理
{
	const CTG_TrainRecord *prev_rec;
	CTG_TrainRecord *now_rec;
	int count=GetTrainRecordCount();
	m_cTrainInfo.m_data.delete_flag=count;  
    //modified by zhaoliang for for engine problem when Continue Draw line
	//兰州发现继续画线后如果出现 -1 的情况会出现打印不出机车，修改机车如果数据集无变化则会删除机车
	//此外兰州希望继续画线后机车随新线走
	if(count!=prev.GetTrainRecordCount())  //record 发生变化,重新计算engine中的索引
	{
		for(int i=0;i<GetEngineCount();i++)
		{
			 CTG_TrainEngineInfo *pengine=GetEngine(i);
			 if(pengine==NULL)
				 continue;
			 if(pengine->m_data.attach_sta>=0)  //挂站
			 {
				 const CTG_TrainRecord  *prevrec=prev.GetTrainRecordNoDelete_Const(pengine->m_data.attach_sta);
				 if(prevrec)
				 {
					 if (pengine->m_data.attach_sta <= count-1)//如果原来机车挂点在在新运行线的范围内则不变
					 {
						  pengine->m_data.attach_sta=m_cRecordList.GetRecordByStation(prevrec->GetStationNo());
					 } 
					 else//如果原来的挂点不在新运行线的范围内，则挂在起点
					 {
						 pengine->m_data.attach_sta=0;//机车挂在新运行线的起点
					 }
				 }
				 else
				 {
					 pengine->m_data.unattach_sta=0;//机车挂在新运行线的起点
				 }
			 }
			 if(pengine->m_data.unattach_sta>=0)  //甩机车所在站
			 {
				 const CTG_TrainRecord  *prevrec=prev.GetTrainRecordNoDelete_Const(pengine->m_data.unattach_sta);
				 if(prevrec)
				 {
					 if (pengine->m_data.unattach_sta <= count-1)//如果原来机车甩点在在新运行线的范围内则不变
					 {
						 if(GetTGFunctionConfig()->IsEngineChangeGiveOutTip())
						 {
							CString Text;
							Text.Format("机车%s需要继续牵引至终点站么？否则甩机车位置将保持不变。",pengine->GetEngineName());
							if (MessageBox(NULL,Text,"提示", MB_YESNO)==IDYES)
							{
								pengine->m_data.unattach_sta=count-1;//机车继续前进至继续画线终点
							}	
							else
							{
								pengine->m_data.unattach_sta=m_cRecordList.GetRecordByStation(prevrec->GetStationNo());//机车在原位置甩
							}
						 }
						 else
						 {
							 pengine->m_data.unattach_sta=count-1;//机车继续前进至继续画线终点
						 }
					 } 
					 else//如果原来的挂点不在新运行线的范围内，则甩在终点
					 {
						 pengine->m_data.unattach_sta=count-1;//机车甩在新运行线的起点
					 }
				 }	
				 else
				 {
					 pengine->m_data.unattach_sta=count-1;//机车甩在新运行线的终点
				 }
			 }
		}
	}

	for(int i=0;i<prev.GetTrainRecordCountWithDeleted();i++)
	{		
		prev_rec=prev.GetTrainRecord_Const(i); 
		if(!prev_rec)
			continue;
		CTG_TrainRecord rec=*prev_rec;
		now_rec=GetTrainRecord(i);
		if(now_rec && (prev_rec->GetStationNo()==now_rec->GetStationNo()))
			continue;
		if(rec.m_data.send_plan_time>0)  // 计划下达过,所以应保存记录
		{
			if (i < prev.GetTrainRecordCount())
				rec.m_data.late_reason=prev_rec->GetIndex();   // 删除后的记录INDEX暂存此处
			m_cRecordList.AddRecord(rec); 
		}
	}

	// 该段程序处理:既给终端(stpc\lirc)既下删除又下更新,这样终端设置的信息丢失问题
	// 遍历每一个删除车站,如果该车站是有效站,那么删除该删除车站
	if (GetTGFunctionConfig()->IsDeleteRepeatDeleteStplan())
	{
		for(int m=GetTrainRecordCountWithDeleted()-1; m>=GetTrainRecordCount(); m--)
		{
			const CTG_TrainRecord* delete_rec=GetTrainRecord_Const(m); 
			if(delete_rec==NULL)
				continue;

			bool bFind=false;
			for(int n=0; n<GetTrainRecordCount(); n++)
			{
				CTG_TrainRecord* prec=GetTrainRecord(n); 
				if(prec==NULL)
					continue;
				
				if(delete_rec->GetStationNo()==prec->GetStationNo())
				{
					if(delete_rec->m_data.late_reason == prec->GetIndex())
					{
						bFind=true;
						prec->m_data.send_plan_time=delete_rec->m_data.send_plan_time; //!!!重要 删除多余删除计划需要记下需要向车站发计划
						break;
					}
				}
			}

			if(bFind)
			{
				m_cRecordList.DeleteRecord(m);	
			}
		}
	}

	return TRUE;
}

int    CTG_SingleTrain::GetTrainRecordCount() const
{
	int all_count=GetTrainRecordCountWithDeleted();
	if(m_cTrainInfo.m_data.delete_flag>all_count)  //// 512计划优化处理 
	{
		return all_count;
	}
	if(m_cTrainInfo.m_data.delete_flag<0)
		return all_count;
	return m_cTrainInfo.m_data.delete_flag;
}

int    CTG_SingleTrain::GetFirstPlanRecordIndex()
{
	int nRecIndex=GetTrainRecordCount()-1;
	for(int i=nRecIndex; i>=0; i--)
	{
		CTG_TrainRecord* rec = GetTrainRecord(i);
		if(rec==NULL)
			continue;
		if(rec->IsActualArriveTimeRecord() || rec->IsActualDepartTimeRecord())
			break;
		nRecIndex=i;
	}

	return nRecIndex;
}

////////////////////////////
CTG_TrainRecord *CTG_SingleTrain::GetTrainRecordByRecIndex(int rec_index)
{
	return GetTrainRecordNoDelete(rec_index);
} 

const CTG_TrainRecord *CTG_SingleTrain::GetTrainRecordByRecIndex_Const(int rec_index) const 
{
	return GetTrainRecordNoDelete_Const(rec_index);
} 

CTG_TrainRecord *CTG_SingleTrain::GetTrainRecordByStation(int stno)//// 512计划优化处理 
{
	for(int i=0;i<GetTrainRecordCount();i++)
	{
		CTG_TrainRecord *prec=GetTrainRecord(i);
		if(prec->GetStationNo()==stno)
			return prec;
	}
	return NULL;
} 

const CTG_TrainRecord *CTG_SingleTrain::GetTrainRecordByStation_Const(int stno) const//// 512计划优化处理 
{
	for(int i=0;i<GetTrainRecordCount();i++)
	{
		const CTG_TrainRecord *prec=GetTrainRecord_Const(i);
		if(prec) 
		{
			if(prec->GetStationNo()==stno)
				return prec;
		}
	}
	return NULL;
} 

// 邻台计划,对于邻台列车应该按照站号从后往前查找
CTG_TrainRecord *CTG_SingleTrain::GetTrainRecordByStation_Reverse(int stno)
{
	int count=GetTrainRecordCount();
	for(int i=count-1;i>=0;i--)
	{
		CTG_TrainRecord *prec=GetTrainRecord(i);
		if(prec->GetStationNo()==stno)
			return prec;
	}
	return NULL;
}

// 邻台计划,对于邻台列车应该按照站号从后往前查找
const CTG_TrainRecord *CTG_SingleTrain::GetTrainRecordByStation_Reverse_Const(int stno) const
{
	int count=GetTrainRecordCount();
	for(int i=count-1;i>=0;i--)
	{
		const CTG_TrainRecord *prec=GetTrainRecord_Const(i);
		if(prec) 
		{
			if(prec->GetStationNo()==stno)
				return prec;
		}
	}
	return NULL;
}

const CTG_TrainRecord *CTG_SingleTrain::GetTrainPreRecordByStation_Const(int stno) const
{
	for(int i=0;i<GetTrainRecordCount();i++)
	{
		const CTG_TrainRecord *prec=GetTrainRecord_Const(i);
		if(prec)
		{
		if(prec->GetStationNo()==stno)
		{
			if(i<=0)
				return NULL;
			else
				return GetTrainRecord_Const(i-1); 
			}
		}
	}
	return NULL;
} 

/////////////// CTG_LogicObject Overload
BOOL   CTG_SingleTrain::CheckSyncStatus(int sync_flag) const
{
	if(!m_cTrainInfo.CheckSyncStatus(sync_flag))
		return FALSE;
	if(!m_cEngineList.CheckSyncStatus(sync_flag))
		return FALSE;
	if(!m_cRecordList.CheckSyncStatus(sync_flag))
		return FALSE;
	return TRUE;
}
BOOL   CTG_SingleTrain::CheckMultiSyncStatus(unsigned long mul_sync_flag) const
{
	if(!m_cTrainInfo.CheckMultiSyncStatus(mul_sync_flag))
		return FALSE;
	if(!m_cEngineList.CheckMultiSyncStatus(mul_sync_flag))
		return FALSE;
	if(!m_cRecordList.CheckMultiSyncStatus(mul_sync_flag))
		return FALSE;
	return TRUE;
}
void   CTG_SingleTrain::SetSyncStatus(int sync_flag,BOOL setva)
{
	m_cTrainInfo.SetSyncStatus(sync_flag,setva);
	m_cEngineList.SetSyncStatus(sync_flag,setva);
	m_cRecordList.SetSyncStatus(sync_flag,setva);
}

void   CTG_SingleTrain::SetNeedSyncFlag(int sync_flag,BOOL setva)
{
	m_cTrainInfo.SetNeedSyncFlag(sync_flag,setva);
	m_cEngineList.SetNeedSyncFlag(sync_flag,setva);
	m_cRecordList.SetNeedSyncFlag(sync_flag,setva);
}

void   CTG_SingleTrain::SetSyncStatusX(int sync_flag,BOOL setva) // 设置指定，其他设置为相反
{
	m_cTrainInfo.SetSyncStatusX(sync_flag,setva);
	m_cEngineList.SetSyncStatusX(sync_flag,setva);
	m_cRecordList.SetSyncStatusX(sync_flag,setva);
}

unsigned long CTG_SingleTrain::GetSyncStatus() const
{
	unsigned long rc=m_cTrainInfo.GetSyncStatus();
	rc&=m_cEngineList.GetSyncStatus();
	rc&=m_cRecordList.GetSyncStatus();
	return rc;
}
BOOL CTG_SingleTrain::IsLocalCreated() const
{
	return TRUE;
}

const char *CTG_SingleTrain::GetTrainID() const
{
	if(m_cRecordList.GetRecordCount()>0)
	{
		//打印折角车次不对，列车车次应为到达车次或train_name  yuzhu
		if(GetTGFunctionConfig()->IsTrainIdUseArriveId())
		{
			if(strlen(m_cRecordList[0].m_data.arrive_train_id)>0)
				return m_cRecordList[0].m_data.arrive_train_id;

			if(strlen(m_cRecordList[0].m_data.depart_train_id)>0)
				return m_cRecordList[0].m_data.depart_train_id; 
		}
		else
		{
			if(strlen(m_cRecordList[0].m_data.depart_train_id)>0)
				return m_cRecordList[0].m_data.depart_train_id;

			if(strlen(m_cRecordList[0].m_data.arrive_train_id)>0)
				return m_cRecordList[0].m_data.arrive_train_id;
		}
	}
	return m_cTrainInfo.m_data.train_name;
}

const char *CTG_SingleTrain::GetEndTrainID() const
{
	int tem = m_cRecordList.GetRecordCount();
	if(tem>0)
	{
		if(strlen(m_cRecordList[tem-1].m_data.depart_train_id)>0)
			return m_cRecordList[tem-1].m_data.depart_train_id; 
		if(strlen(m_cRecordList[tem-1].m_data.arrive_train_id)>0)
			return m_cRecordList[tem-1].m_data.arrive_train_id; 
	}
	return m_cTrainInfo.m_data.train_name;
}

BOOL   CTG_SingleTrain::MatchTrainID(const char *tid)
{
	for(int i=0;i<GetTrainRecordCount();i++)
	{	
		if(!strcmp(m_cRecordList[i].m_data.arrive_train_id,tid))
				return TRUE;

		if(i+1==m_cRecordList.GetRecordCount())
		{
			if(TG_IsFlagTerminal(m_cRecordList[i].m_data.flag))
			{
				return FALSE;
			}
		}
		 if(!strcmp(m_cRecordList[i].m_data.depart_train_id,tid))
			return TRUE;
	  }
	  return FALSE;
}

void   CTG_SingleTrain::SetTrainIDEx(const char *tid,BOOL orig_tid,int from_rec,BOOL from_arrive,int to_rec,BOOL to_depart)
{
	int rec_count=m_cRecordList.GetRecordCount();
	if(rec_count<=0)
		return;
	if(from_rec<0)
		from_rec=0;
	if(to_rec<0)
		to_rec=rec_count-1;
	for(int i=from_rec;i<=to_rec;i++)
	{	
		if(i!=from_rec ||  from_arrive)
		{
			if(orig_tid)
			{
				TG_CopyTrainId(m_cRecordList[i].m_data.original_arrive_train_id,tid);
			}
			else
			{
				TG_CopyTrainId(m_cRecordList[i].m_data.arrive_train_id,tid);
			}
		}
		if(i!=to_rec ||  to_depart)
		{
			if(orig_tid)
			{
				TG_CopyTrainId(m_cRecordList[i].m_data.original_depart_train_id,tid);
			}
			else
			{
				TG_CopyTrainId(m_cRecordList[i].m_data.depart_train_id,tid);
			}
		}
	}
}

void   CTG_SingleTrain::SetTrainID(const char *tid)
{
		SetTrainIDEx(tid,FALSE,-1,TRUE,-1,TRUE);
		TG_CopyTrainId(m_cTrainInfo.m_data.train_name,tid);
}

void   CTG_SingleTrain::SetOrigTrainID(const char *tid)
{
		SetTrainIDEx(tid,TRUE,-1,TRUE,-1,TRUE);
}

BOOL   CTG_SingleTrain::IsDownDirection() const
{
	const char *tid=GetTrainID();
	if(tid && strlen(tid)>0)
	{
		return (tid[strlen(tid)-1] % 2)?TRUE:FALSE;
	}
	return FALSE;
}

BOOL   CTG_SingleTrain::IsEndDownDirection() const
{
	const char *tid=GetEndTrainID();
	if(tid && strlen(tid)>0)
	{
		return (tid[strlen(tid)-1] % 2)?TRUE:FALSE;
	}
	return FALSE;
}

BOOL   CTG_SingleTrain::IsSecretTrain() const
{
	return m_cTrainInfo.IsSecretTrain();
}

BOOL   CTG_SingleTrain::IsUpDirection() const
{
	const char *tid=GetTrainID();
	if(tid && strlen(tid)>0)
	{
		return (tid[strlen(tid)-1] % 2)?FALSE:TRUE;
	}
	return FALSE;
}

BOOL   CTG_SingleTrain::IsEndUpDirection() const
{
	const char *tid=GetEndTrainID();
	if(tid && strlen(tid)>0)
	{
		return (tid[strlen(tid)-1] % 2)?FALSE:TRUE;
	}
	return FALSE;
}

BOOL   CTG_SingleTrain::IsElectricTrain(int station) const // 如果station==-1,则只要任一车站记录有电力标志，则为TRUE
{
	const CTG_TrainRecord *prec;
	const CTG_TrainRecord *ppre;
	if(station>0)
	{
		prec=GetTrainRecordByStation_Const(station);
		ppre=GetTrainPreRecordByStation_Const(station);
		if(prec && ppre)
		{
			return (TG_IsElecLocoFlag(prec->m_data) || TG_IsElecLocoFlag(ppre->m_data));
		}
		else if(prec)
		{
			return TG_IsElecLocoFlag(prec->m_data);
		}
		else if(ppre)
		{
			return TG_IsElecLocoFlag(ppre->m_data);
		}
		else
			return TRUE;
	}
	for(int i=0;i<GetTrainRecordCount();i++)
	{
		prec=GetTrainRecord_Const(i);
		if(prec)
		{
			if(TG_IsElecLocoFlag(prec->m_data))
				return TRUE;
		}
	}
	return FALSE;
}

int    CTG_SingleTrain::GetSpaceRank(int rec_index) const // 如果rec_index==-1,查找记录中最大的超限值
{
	const CTG_TrainRecord *prec;
	if(rec_index>=0)
	{
		prec=GetTrainRecordByRecIndex_Const(rec_index);
		if(prec)
		{
			if(prec->IsStationSpace()>0)
				return (prec->GetArriveSpace()>prec->GetDepartSpace())? prec->GetArriveSpace():prec->GetDepartSpace();
			else
			{
			   const CTG_TrainRecord *prev_rec=GetTrainRecordByRecIndex_Const(rec_index-1);
			   if(prev_rec)
				   return prev_rec->GetDepartSpace();
			}
			return 0;
		}
		return 0;
	}

	return  m_cTrainInfo.m_data.space_rank;
}

void   CTG_SingleTrain::SetSpaceRank(int  va,int fromindex,  bool include_arrive,  bool bmodifyinfo)
{
	for(int i=fromindex;i<GetTrainRecordCount();i++)
	{
		CTG_TrainRecord *prec=GetTrainRecord(i);
		if(i!=fromindex || include_arrive)
			prec->SetArriveSpace(va);
		prec->SetDepartSpace(va);
	}

	if(fromindex>0)
	{
		CTG_TrainRecord *prec=GetTrainRecord(fromindex);
		CTG_TrainRecord *ppre=GetTrainRecord(fromindex-1);
		if(prec&&ppre)
		{
			BYTE nDepart=ppre->GetDepartSpace();
			BYTE nArrive=prec->GetArriveSpace();
			if(nDepart>nArrive)
			{
				CString str;
				str.Format("%s 在 %s 发车超限等级大于 %s 接车超限等级, %s 接车超限等级不修改!", 
					this->GetTrainID(),
					gpcdef.GetStationName(ppre->m_data.station), 
					gpcdef.GetStationName(prec->m_data.station),
					gpcdef.GetStationName(prec->m_data.station));

				AfxMessageBox(str, MB_OK|MB_ICONWARNING);

				GpcLogTrainMessage(0, 0, "%s", str);
			}
		}
	}
	// 修正
	for(int i=0;i<GetTrainRecordCount();i++)
	{
		CTG_TrainRecord *prec=GetTrainRecord(i);
		CTG_TrainRecord *pnext=GetTrainRecord(i+1);
		if(pnext)
		{
			BYTE nDepart=prec->GetDepartSpace();
			BYTE nArrive=pnext->GetArriveSpace();
			if(nDepart<nArrive)
			{
				prec->SetDepartSpace(nArrive);
			}
			else if(nDepart>nArrive)
			{
				pnext->SetArriveSpace(nDepart);
			}
		}
		if(prec)
		{
			if(prec->IsStationSpace())
				TG_SetSpaceRank(prec->m_data);
			else
				TG_SetNoSpaceRank(prec->m_data);
		}
	}

	if(bmodifyinfo)
		m_cTrainInfo.m_data.space_rank=va;
	else	if(m_cTrainInfo.m_data.space_rank<va)
		m_cTrainInfo.m_data.space_rank=va;
	else
	{
		BYTE nTmpSpace=0;
		for(int i=0;i<GetTrainRecordCount();i++)
		{
			CTG_TrainRecord *prec=GetTrainRecord(i);
			if(prec)
			{
				BYTE nArrive=prec->GetArriveSpace();
				BYTE nDepart=prec->GetDepartSpace();
				if(nArrive>nTmpSpace)
					nTmpSpace=nArrive;
				if(nDepart>nTmpSpace)
					nTmpSpace=nDepart;
			}
		}
		m_cTrainInfo.m_data.space_rank=nTmpSpace;
	}

}

// 实现老数据兼容
void CTG_SingleTrain::SetTrainStSpace()
{
	if(GetTrainInfo()->m_data.space_rank>4)
	{
		GpcLogTrainMessage(0, 0, "列车 %s(%d) 超限标志不合法 space_rank = %d, 强制设置为三级超限", 
			GetTrainID(),GetTrainIndex(),GetTrainInfo()->m_data.space_rank);
		GetTrainInfo()->m_data.space_rank=4;
	}

	if (GetTrainInfo()->m_data.space_rank<0)
	{
		GpcLogTrainMessage(0, 0, "列车 %s(%d) 超限标志不合法 space_rank = %d, 强制设置为正常", 
			GetTrainID(),GetTrainIndex(),GetTrainInfo()->m_data.space_rank);
		GetTrainInfo()->m_data.space_rank=0;
	}
	
		
	bool bupdate=false;
	for (int i=0 ;i<GetTrainRecordCount();i++)
	{
		CTG_TrainRecord *rec = GetTrainRecordByRecIndex(i);
		if(rec)
		{
			if((rec->IsStationSpace()<=0)&&(TG_IsSpaceRank(rec->m_data)))
			{
				rec->SetArriveSpace(GetTrainInfo()->m_data.space_rank);
				rec->SetDepartSpace(GetTrainInfo()->m_data.space_rank);
				bupdate=true;
			}
		}
	}

	if(bupdate)
	{
		for (int i= 0 ;i<GetTrainRecordCount()-1;i++)
		{
			CTG_TrainRecord *rec = GetTrainRecordByRecIndex(i);
			CTG_TrainRecord *next = GetTrainRecordByRecIndex(i+1);
			if(next)
			{
				BYTE nArriveSpace=next->GetArriveSpace();
				BYTE nDepartSpace=rec->GetDepartSpace();
				if(nDepartSpace<nArriveSpace)
					rec->SetDepartSpace(nArriveSpace);
				else if(nDepartSpace>nArriveSpace)
					next->SetArriveSpace(nDepartSpace);
			}
		}
	}

	return;
}

int    CTG_SingleTrain::GetMilitaryRank(int station) const // 如果station==-1,查找记录中最大的值
 {
	const CTG_TrainRecord *prec;
	if(m_cTrainInfo.m_data.military_rank>0)
		return m_cTrainInfo.m_data.military_rank;
	if(station>0)
	{
		prec=GetTrainRecordByStation_Const(station);
		if(prec)
			return prec->GetMilitaryRank();
		else
			return 0;
	}
	for(int i=0;i<GetTrainRecordCount();i++)
	{
		prec=GetTrainRecord_Const(i);
		if(prec)
		{
			if(prec->GetMilitaryRank()>0)
				return prec->GetMilitaryRank();
		}
	}
	return 0;
 } 

 void CTG_SingleTrain::SetMilitaryRank(int va,int fromstation)  
 {
	 int fromindex;
	 if(fromstation>0)
		 fromindex=m_cRecordList.GetRecordByStation(fromstation);
	 else
		 fromindex=0;
	 for(int i=fromindex;i<GetTrainRecordCount();i++)
	 {
		 CTG_TrainRecord *prec=GetTrainRecord(i);
		 prec->SetMilitaryRank(va); 
	 }
	 if(va>0)
		 m_cTrainInfo.m_data.military_rank=va;  
	 else
	 {
		 m_cTrainInfo.m_data.military_rank=0;  
		 for(int i=0;i<GetTrainRecordCount();i++)
		 {
			 CTG_TrainRecord *prec=GetTrainRecord(i);
			 if(prec->GetMilitaryRank()>0)
			 {
				 m_cTrainInfo.m_data.military_rank=prec->GetMilitaryRank();
				 break;
			 }
		 }
	 }
 } 


TIME   CTG_SingleTrain::GetStartTime() const
{
	const CTG_TrainRecord *prec=GetTrainRecordNoDelete_Const(0);
	if(!prec)
		return 0;
	if(!prec->IsStartRec())
		return prec->GetArriveTime();
	else
		return prec->GetDepartTime();

}

TIME   CTG_SingleTrain::GetEndTime() const
{
	const CTG_TrainRecord *prec=GetTrainRecordNoDelete_Const(GetTrainRecordCount()-1);
	if(!prec)
		return 0;
	if(!prec->IsTerminalRec())
		return prec->GetDepartTime();
	else
		return prec->GetArriveTime();
		

}

BOOL   CTG_SingleTrain::IfInTimeScope(TIME stime, TIME etime, BOOL IsStart) const
{
		int rec_count=m_cRecordList.GetRecordCount();
		if(rec_count<=0)
			return FALSE;
		int i;
		for( i=0;i<rec_count;i++)
		{
			if(IsStart && i > 0)
				return FALSE;
			if(m_cRecordList[i].IfInTimeScope(stime,etime))
				break;			
		}
		if(i<rec_count)
			return TRUE;
		else
			return FALSE;
}

TRAIN_MEET_RELATION CTG_SingleTrain::GetMeetRelation(const CTG_TrainRecord& thisrec, const CTG_SingleTrain &another_train, int another_train_recindex) const
{
	const CTG_TrainRecord *ano_rec = another_train.GetTrainRecordNoDelete_Const(another_train_recindex);
	if (NULL == ano_rec || TG_IsFlagTerminal(thisrec.m_data.flag) || thisrec.GetStationNo() != ano_rec->GetStationNo())
		return NO_MEET;

	const TG_TrainRecord &rec = thisrec.m_data;	
	TIME btime = rec.arrive;
	TIME etime = rec.depart;
	// if train started or terminated, 30 minutes is used to define the meet relation
	if (TG_IsFlagStart(rec.flag))
		btime = etime - 1800;
	if (TG_IsFlagTerminal(rec.flag))
		etime = btime + 1800;

	TRAIN_MEET_RELATION rc=NO_MEET;	
	if (ano_rec->GetEntry() == rec.exit)
	{
		if (ano_rec->GetArriveTime() >= btime && ano_rec->GetArriveTime() <= etime)
		{
			// use trainindex to exclude two trains meet each other
			if (ano_rec->GetArriveTime() == btime && ano_rec->GetDepartTime() == etime && GetTrainIndex()>another_train.GetTrainIndex())
				rc = NO_MEET;
			else
				rc = DIFF_DIR_MEET;
		}
	}
	if (ano_rec->GetExit() == rec.exit  && ano_rec->GetDepartTime() >= btime && ano_rec->GetDepartTime() <= etime) 
	{
		// if depart time is same, use trainindex to exclude two trains meet each other which may cause lirc logically dead lock
		if (ano_rec->GetDepartTime() == etime && GetTrainIndex()>another_train.GetTrainIndex()) 
			rc = NO_MEET;
		else
			rc = SAME_DIR_MEET;
	}
	return rc;
}

// bActual=false nFlag=1, 平移nRecordNo及其后车站的到发点
// bActual=false nFlag=0, 平移nRecordNo发点及其后车站的到发点
// bActual=true nFlag=1, 平移nRecordNo的到发点
// bActual=true nFlag=0, 平移nRecordNo发点
BOOL  CTG_SingleTrain::OffsetScheduleTime(int nRecordNo, int nTimeOffset,BOOL  bActual, int nFlag)
{
	CTG_TrainRecord *precord=GetTrainRecord(nRecordNo);
	if(!precord)
		return FALSE;
	TG_TrainRecord  *rec=&precord->m_data;
	
	if(bActual)
	{
		if(!(nFlag == 0))
		{
			rec->arrive += nTimeOffset;
			
		}

		// last station and terminal
		if(precord->IsTerminalRec() && nRecordNo+1 == GetTrainRecordCount())
			rec->depart = rec->arrive;
		else
			rec->depart += nTimeOffset;
	}
	else
	{
		for(int i=nRecordNo;i < GetTrainRecordCount();i++)
		{
			precord=GetTrainRecord(i);
			if(!precord)
				continue;
			rec=&precord->m_data;
			if(!((nFlag == 0) && i == nRecordNo))
			{
				rec->arrive += nTimeOffset;
			}
			
			// last station and terminal
			if(precord->IsTerminalRec() && i+1 == GetTrainRecordCount())
				rec->depart = rec->arrive;
			else
				rec->depart += nTimeOffset;
		}
	}
	return TRUE;
}

BOOL IsCrhTrain(int type, int run_type)
{
	if((type>=22 && type<=25) || (type>=17 && type<=19) || (run_type==5))
		return TRUE;
	else
		return FALSE;
}

BOOL  CTG_SingleTrain::IsCRHTrain() const
{
	int  tp=GetTrainType();
	return IsCrhTrain(tp, m_cTrainInfo.m_data.run_type);
}

BOOL  CTG_SingleTrain::IsSpecialZ() const
{
	int  tp=GetTrainType();
	if(tp==15)
		return TRUE;
	else
		return FALSE;
}

const char *CTG_SingleTrain::GetAlarmInfo(int flag) const 
{
	static string alarm_string;
	alarm_string.clear(); 
	int rec_count=GetTrainRecordCount();
	int index=1;
	for(int i=0;i<rec_count;i++)
	{
		const CTG_TrainRecord *prec=GetTrainRecord_Const(i);
		if(!prec)
			continue;
		//char tmp[10];
		//sprintf(tmp,">警告%d:",index++);
		//alarm_string+=tmp;
		alarm_string+=prec->m_strAlarmText; 
	}
	return alarm_string.c_str(); 
}

bool CreateRecursiveDir(const char* filename);
BOOL ReadGenernalXMLFile(CString &dest,const char *filepath);
BOOL WriteGenernalXMLFile(const char *xml,const char *filepath);

void  CTG_SingleTrain::SetImportInfo(const char *info,int flag)
{
	CString text;
	CString filename;
	filename.Format("%s\\OPMS_INFO\\%d.xml", gpcdef.GetXMLDir(),m_cTrainInfo.m_data.shift_train_index);
	if(!ReadGenernalXMLFile(text,filename))
	{
		CreateRecursiveDir(filename);
	}

	if(flag==TRAIN_TXT_INFO_TYPE_IMPORTANT)
	{
		text = info;
		m_cTrainInfo.m_info=info;
	}
	else
	if(flag==STATION_TXT_INFO_TYPE_SHUAIGUA)
	{
		text = SetXMLAttribStringValue(m_cTrainInfo.m_info.c_str(),"shginfo",info,0);
		m_cTrainInfo.m_info=text;
	}
	else
	if(flag==TRAIN_TXT_INFO_TYPE_KEDIAO)
	{
		text=SetXMLAttribStringValue(m_cTrainInfo.m_info.c_str(),"kdinfo",info,0);
		m_cTrainInfo.m_info=text;
	}
	else
	if(flag==TRAIN_TXT_INFO_TYPE_YINGYE)
	{
		text=SetXMLAttribStringValue(m_cTrainInfo.m_info.c_str(),"yyinfo",info,0); 
		m_cTrainInfo.m_info=text;
	}
	else
	if(flag==TRAIN_TXT_INFO_TYPE_JUTEYUN)
	{
		text=SetXMLAttribStringValue(m_cTrainInfo.m_info.c_str(),"ttinfo",info,0);
		m_cTrainInfo.m_info=text;
	}
	else
	if(flag==TRAIN_TXT_INFO_TYPE_CREW)
	{
		text=SetXMLAttribStringValue(m_cTrainInfo.m_info.c_str(),"tkinfo",info,0);
		m_cTrainInfo.m_info=text;
	}

	WriteGenernalXMLFile(text,filename);
}

CString CTG_SingleTrain::toString() const
{
	CString strRet,tmp;
	for(int index=0; index <GetTrainRecordCount(); index++)
	{
		const CTG_TrainRecord* prec=GetTrainRecordNoDelete_Const(index);
		if(prec==NULL)
			continue;
		tmp.Format("%d(%0.2X)(%d-%d)(%d/%d-%d/%d)(%d-%d) ",prec->GetStationNo(), (BYTE)prec->m_data.adjust_status, 
			prec->m_data.entry, prec->m_data.exit,
			prec->m_data.plan_arrive_side, prec->m_data.plan_depart_side,
			prec->m_data.arrive_side, prec->m_data.depart_side,
			prec->m_data.arrive, prec->m_data.depart);
		strRet += tmp;
	}

	return strRet;
}

const char *CTG_SingleTrain::GetImportInfo(int flag) const
{
	CString text;
	CString filename;
	filename.Format("%s\\OPMS_INFO\\%d.xml", gpcdef.GetXMLDir(),m_cTrainInfo.m_data.shift_train_index);

	static char tmp[4048];
	strcpy(tmp,"");
	if(flag==TRAIN_TXT_INFO_TYPE_SHUAIGUA)
	{
		tmp[0]=0;
		const CTG_TrainRecord *prec;
		for(int r=0;r<GetTrainRecordCount();r++)
		{
			prec=GetTrainRecord_Const(r);
			if(prec)
			{
				const char *ptmpstr=prec->GetStationText(STATION_TXT_INFO_TYPE_SHUAIGUA);
				if(ptmpstr && strlen(ptmpstr)>0)
				{
					char shuaigua[512];
					memset(shuaigua, 0x00, sizeof(shuaigua));
					_snprintf(shuaigua, sizeof(shuaigua)-1, "%s: %s",gpcdef.GetStationName(prec->GetStationNo()),ptmpstr);
					strncat(tmp, shuaigua, sizeof(tmp)-strlen(tmp)-1-2/*"\r\n"*/);
					strcat(tmp,"\r\n");
				}
			}
		}
		return tmp;
	}
	else if(flag==STATION_TXT_INFO_TYPE_OPMS)
	{
		tmp[0]=0;
		const CTG_TrainRecord *prec;
		for(int r=0;r<GetTrainRecordCount();r++)
		{
			prec=GetTrainRecord_Const(r);
			if(prec)
			{
				const char *ptmpstr=prec->GetStationText(STATION_TXT_INFO_TYPE_OPMS);
				if(ptmpstr && strlen(ptmpstr)>0)
				{
					char shuaigua[512];
					memset(shuaigua, 0x00, sizeof(shuaigua));
					_snprintf(shuaigua, sizeof(shuaigua)-1, "%s: %s",gpcdef.GetStationName(prec->GetStationNo()),ptmpstr);
					strncat(tmp, shuaigua, sizeof(tmp)-strlen(tmp)-1-2/*"\r\n"*/);
					strcat(tmp,"\r\n");
				}
			}
		}
		return tmp;
	}
	else 
	{
		if (GetShiftTrainIndex()==NULL_TRAIN_INDEX)
			return tmp;
		
		if(!ReadGenernalXMLFile(text,filename))
			return tmp;

		if(flag==TRAIN_TXT_INFO_TYPE_IMPORTANT)
		{
			return text;
		}
		else if(flag==TRAIN_TXT_INFO_TYPE_KEDIAO)
		{
			return GetXMLAttribStringValue(text,"kdinfo");
		}
		else if(flag==TRAIN_TXT_INFO_TYPE_YINGYE)
		{
			return GetXMLAttribStringValue(text,"yyinfo");
		}
		else if(flag==TRAIN_TXT_INFO_TYPE_JUTEYUN)
		{
			return GetXMLAttribStringValue(text,"ttinfo");
		}
		else if(flag==TRAIN_TXT_INFO_TYPE_CREW)
		{
			return GetXMLAttribStringValue(text,"tkinfo");
		}
	}

	return tmp;
}

int  CTG_SingleTrain::GetStationElecLocoTypeByFlag(int station) const // 根据flag判断是否电力牵引 ,返回1:电力 0:非电力, -1无信息
{
	const CTG_TrainRecord *prec;
	int rc=-1;
	for(int i=0;i<GetTrainRecordCount();i++)
	{
		prec=GetTrainRecord_Const(i);
		if(!prec)
			continue;
		if(station<0 || prec->GetStationNo()==station)
		{
			if(TG_IsElecLocoFlag(prec->m_data))
			{
				rc=1;
				break;
			}
		}
		rc=0;
	}
	return rc;
}

int  CTG_SingleTrain::GetRecordElecLocoTypeByEngineInfo(int index) const // 根据engine info判断是否电力牵引 ,返回1:电力 0:非电力, -1无信息
{
	BOOL has_engine=FALSE;
	for(int i=0;i<GetEngineCount();i++)
	{
		const CTG_TrainEngineInfo *pengine=GetEngine_Const(i);
		if(!pengine->IsMainTractEngine())
			continue;
		has_engine=TRUE;
		if(!pengine->IsElecTypeEngine())
			continue;
		if(index<0 || index>=pengine->GetAttachRecord() && index<=pengine->GetUnAttachRecord())
			return 1;
	}
	if(has_engine)
		return 0;
	else
		return -1;
}

int  CTG_SingleTrain::GetStationElecLocoTypeByEngineInfo(int station) const // 根据engine info判断是否电力牵引 ,返回1:电力 0:非电力, -1无信息
{
	const CTG_TrainRecord *prec;
	int index=-1;
	if(station>0 && (prec=GetTrainRecordByStation_Const(station)))
	 {
		 index=prec->GetIndex(); 
	 }
	return GetRecordElecLocoTypeByEngineInfo(index);
}

BOOL  CTG_SingleTrain::DirectChangeTrainLocoTypeFlag(BOOL elec,int fromstation) // fromstation=-1
{
     CTG_TrainRecord *prec;
	 int fromindex=0;
	 if(fromstation>0 && (prec=GetTrainRecordByStation(fromstation)))
	 {
		 fromindex=prec->GetIndex(); 
	 }
	 for(int i=fromindex;i<GetTrainRecordCount();i++)
	 {
		 if(GetRecordElecLocoTypeByEngineInfo(i)==1 && !elec)  // 与机车信息不一致
			 continue;
		 if(GetRecordElecLocoTypeByEngineInfo(i)==0 && elec)   // 与机车信息不一致
			 continue;
		 prec=GetTrainRecord(i);
		 if(elec)
			TG_SetElecLocoFlag(prec->m_data); 
		 else
            TG_SetNoElecLocoFlag(prec->m_data); 
	 }
	 return TRUE;
}

void  CTG_SingleTrain::SetElecFlagByEngineInfo()
{
	CTG_TrainRecord *prec;
	int rc=-1;
	for(int i=0;i<GetTrainRecordCount();i++)
	{
		prec=GetTrainRecord(i);
		if(!prec)
			continue;
		int elec_rc=GetRecordElecLocoTypeByEngineInfo(i);
		if(elec_rc==1)
			TG_SetElecLocoFlag(prec->m_data); 
		else
		if(elec_rc==0)
			TG_SetNoElecLocoFlag(prec->m_data); 
	}
}

BOOL  CTG_SingleTrain::HasSentPlan() const // 是否已经下过计划
{
    const CTG_TrainRecord *prec;
	int rc=-1;
	int all_count=GetTrainRecordCountWithDeleted();
	if(all_count>GetTrainRecordCount()) // 存在删除记录,必然是下过计划
		return TRUE;

	for(int i=0;i<GetTrainRecordCount();i++)
	{
		prec=GetTrainRecord_Const(i);
		if(prec)
		{
			if(prec->m_data.send_plan_time>0)
				return TRUE;
		}
	}
	return FALSE;
}

int CTG_SingleTrain::HasActualRecord(int fromindex) const // 是否包含实际点
{
	const CTG_TrainRecord *prec;
	for(int i=fromindex;i<GetTrainRecordCount();i++)
	{
		prec=GetTrainRecordNoDelete_Const(i);
		if(prec)
		{
			if(prec->IsActualArriveTimeRecord() || prec->IsActualDepartTimeRecord())
				return i;
		}
	}
	return -1;
}


void  CTG_SingleTrain::SetDefaultAttribByDefinedFlag(unsigned short  locotype)  // 注意: locotype 是在train_type.xml中指定的值
{
		if(locotype&0x01)
			DirectChangeTrainLocoTypeFlag(TRUE);
		else if(locotype&0x02)
			DirectChangeTrainLocoTypeFlag(FALSE);
		if(locotype&0x04)
			SetSpaceRank(1, 0, true, true);
		if(locotype&0x08)
			SetMilitaryRank(1);
}


BOOL  CTG_SingleTrain::OnlyNodeIndexWithoutExtryExit() const
{
	 for(int i=0;i<GetTrainRecordCount();i++)
	 {
		 const CTG_TrainRecord *prec=GetTrainRecord_Const(i);
		 if(prec)
		 {
			if(prec->GetArriveTGIndex()>=255 || prec->GetArriveTGIndex()<0)
				return FALSE;
			if(prec->GetDepartTGIndex()>=255 || prec->GetDepartTGIndex()<0)
				return FALSE;
			if(IS_VALID_ENTRY(prec->GetEntry()))
				return FALSE;
			if(IS_VALID_ENTRY(prec->GetExit()))
				return FALSE;
		 }
	 }
	 return TRUE;
}

void  CTG_SingleTrain::SetTrainStartEndFlag()
{
	int num = GetTrainRecordCount();
	if(num<=1)
		return;

	for(int i=0;i<num;i++)
	{
		CTG_TrainRecord *prec=GetTrainRecord(i);
		if(NULL == prec)
			continue;
		
		TG_ClearFlagGiveoutTerminal(prec->m_data.flag);
		TG_ClearFlagStartAccept(prec->m_data.flag);

		if(0 == i)
		{
			if (0 < prec->GetEntry())
				TG_SetFlagAccept(prec->m_data.flag);
			else
				TG_SetFlagStart(prec->m_data.flag);							
		}
		if(num-1 == i)
		{
			if (0 < prec->GetExit())
				TG_SetFlagGiveout(prec->m_data.flag);
			else
				TG_SetFlagTerminal(prec->m_data.flag);			
		}
	}
}

const char *CTG_SingleTrain::GetImportInfoText(int flag) const
{
	static char tmp[4048];
	strcpy(tmp,"");
	const TG_TrainInfoExpand* pTrainExpand=GetTrainInfoExpand_Const();
	CString str,text;
	if(flag==TRAIN_TXT_INFO_TYPE_CREW)
	{
		text=pTrainExpand->GetTrainCrewList_Const()->toString();
		strcpy(tmp,text);
		return tmp;
	}
	else if(flag==TRAIN_TXT_INFO_TYPE_YINGYE)
	{
		text="";
		const CTG_TrainYingyeList* pYYList=pTrainExpand->GetTrainYYList_Const();
		for(int i=0; i<pYYList->GetCount(); i++)
		{
			const TG_TrainYingyeInfo* p=pYYList->GetRecord_Const(i);
			if(p)
			{
				text += p->station_name;
				text+=" ";

				if(p->arrive_time>=0)
				{
					CTime ct(p->arrive_time);
					text += ct.Format("%Y年%m月%d日 %H:%M:%S");
					text+=" ";
				}
				if(p->depart_time>=0)
				{
					CTime ct(p->depart_time);
					text += ct.Format("%Y年%m月%d日 %H:%M:%S");
				}
				text+="\r\n";
			}
		}
		strcpy(tmp,text);
		return tmp;
	}
	else if(flag==TRAIN_TXT_INFO_TYPE_KEDIAO)
	{
		const CTG_TrainKediaoInfo* pKD=pTrainExpand->GetTrainKediao_Const();
		////类型 1:客调 2：特调
		if(pKD->type==1)
			str="客调信息:";
		else if(pKD->type==2)
			str="特调信息:";
		else
			str="";

		text = str;
		text += pKD->content;
		text+="\r\n";
		strcpy(tmp,text);
		return tmp;
	}
	
	return tmp;
}

void CTG_SingleTrain::ModifyTrainRunTimeByMinute(int begin_sta_index, int end_sta_index, DWORD begin_time, DWORD end_time, int minute)
{
	if(minute==0)
		return;
	
	int rec_count=GetTrainRecordCount();
	if(rec_count<=0)
		return;

	int sta_index_list[64];
	int sta_index_cnt=0;
	if (begin_sta_index<end_sta_index)
	{
		sta_index_cnt=end_sta_index-begin_sta_index+1;
		for (int n=0;n<sta_index_cnt;n++)
		{
			sta_index_list[n]=begin_sta_index+n;
		}
	}
	else if (begin_sta_index > end_sta_index)
	{
		sta_index_cnt=begin_sta_index-end_sta_index+1;
		for (int n=0;n<sta_index_cnt;n++)
		{
			sta_index_list[n]=begin_sta_index-n;
		}
	}

	for(int i=0;i<rec_count;i++)
	{
		CTG_TrainRecord *rec = GetTrainRecordByRecIndex(i);
		CTG_TrainRecord *next_rec = GetTrainRecordByRecIndex(i+1);
		if (rec==NULL || next_rec==NULL)
			return;

		if(next_rec->IsActualArriveTimeRecord() || next_rec->IsActualDepartTimeRecord())
		{
			continue;
		}

		for (int x=0; x<sta_index_cnt; x++)
		{
			if (rec->GetDepartTGIndex() == sta_index_list[x] && x!=sta_index_cnt-1)
			{
				if(next_rec->GetArriveTGIndex() == sta_index_list[x+1])
				{
					if (rec->IfInTimeScope(begin_time, end_time) && next_rec->IfInTimeScope(begin_time, end_time))
					{
						int offset = next_rec->GetArriveTime() - rec->GetDepartTime();
						if (offset<0)
							continue;

						this->OffsetScheduleTime(i, minute*60, FALSE, 1);
						
						sysprint(0,0,"[平移调整] 自动调整 列车 %s(%d) 在 %s(%d) 站的到发时间",
							GetTrainID(), GetTrainIndex(), gpcdef.GetStationName(next_rec->m_data.station),next_rec->m_data.station);
						return;
					}
				}
			}
		}
	}	
}

void CTG_SingleTrain::ModifyTrainRunTimeByPercent(int begin_sta_index, int end_sta_index, DWORD begin_time, DWORD end_time, int percent)
{
	if(percent==0)
		return;
	
	int rec_count=GetTrainRecordCount();
	if(rec_count<=0)
		return;

	int sta_index_list[64];
	int sta_index_cnt=0;
	if (begin_sta_index<end_sta_index)
	{
		sta_index_cnt=end_sta_index-begin_sta_index+1;
		for (int n=0;n<sta_index_cnt;n++)
		{
			sta_index_list[n]=begin_sta_index+n;
		}
	}
	else if (begin_sta_index > end_sta_index)
	{
		sta_index_cnt=begin_sta_index-end_sta_index+1;
		for (int n=0;n<sta_index_cnt;n++)
		{
			sta_index_list[n]=begin_sta_index-n;
		}
	}

	for(int i=0;i<rec_count;i++)
	{
		CTG_TrainRecord *rec = GetTrainRecordByRecIndex(i);
		CTG_TrainRecord *next_rec = GetTrainRecordByRecIndex(i+1);
		if (rec==NULL || next_rec==NULL)
			return;
		
		if(next_rec->IsActualArriveTimeRecord() || next_rec->IsActualDepartTimeRecord())
		{
			continue;
		}

		for (int x=0; x<sta_index_cnt; x++)
		{
			if (rec->GetDepartTGIndex() == sta_index_list[x] && x!=sta_index_cnt-1)
			{
				if(next_rec->GetArriveTGIndex() == sta_index_list[x+1])
				{
					if (rec->IfInTimeScope(begin_time, end_time) && next_rec->IfInTimeScope(begin_time, end_time))
					{
						int offset = next_rec->GetArriveTime() - rec->GetDepartTime();
						if (offset<0)
							continue;

						int minute = offset/60;
						offset = minute*percent*0.1;

						this->OffsetScheduleTime(i+1, offset*60, FALSE, 2);

						sysprint(0,0,"[慢行调整] 自动调整 列车 %s(%d) 在 %s(%d) 站的到发时间",
							GetTrainID(), GetTrainIndex(), gpcdef.GetStationName(next_rec->m_data.station),next_rec->m_data.station);

					}
				}
			}
		}
	}	
}

/////////////////////////////////////////////////////////////////////////////////////////
CTG_SingleTrain* CTG_TrainScheduleBase::GetTrainForUpdateExtent(TRAIN_INDEX train_index)
{
	if(!GetTGFunctionConfig()->IsEnableTDMS5_0() || (train_index==0)) //TD5.0
		return NULL;

	CTG_SingleTrain *ptrain=NULL;
	IterMapTrain iter=FirstTrain();
	while(iter!=LastTrain())
	{
		TRAIN_INDEX train_index_itr=NULL;
		if(m_cScheduleInfo.m_nType== CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT)
			train_index_itr=iter->second.GetTrainIndex();
		else if(m_cScheduleInfo.m_nType== CTG_ScheduleInfo::SCHEDULE_TYPE_WORK)
			train_index_itr=iter->second.GetShiftTrainIndex();
		else
			break;

		if(!iter->second.IsDeletedObject() 
			&&  (train_index_itr & 0x3FFFFFFF)==(train_index&0x3FFFFFFF) // 20141204 车次分割 30bit为1,解决南宁全局ID分割重复
			&& iter->second.GetTrainRecordCount())
		{
			ptrain=&iter->second;
			break;
		}
		iter++;
	}

	return ptrain;
}

BOOL   CTG_TrainScheduleBase::Lock()
{
	#ifdef LOCK_ENABLED_WINDOWS
	return m_csCriticalSection.Lock(2000);
	#else
	return TRUE;
	#endif

}
BOOL   CTG_TrainScheduleBase::UnLock()
{
	#ifdef LOCK_ENABLED_WINDOWS
	return m_csCriticalSection.Unlock();
	#else
	return TRUE;
	#endif
}

CTG_TrainScheduleBase::CTG_TrainScheduleBase()
{
	m_nUserSpecial=0;
	m_nFileSaveFlag=0;
	m_nPollingBeginAddTime=0;
	m_nPollingBeginDelTime=0;
}

void  CTG_TrainScheduleBase::SetScheduleInfo(const CTG_ScheduleInfo &info)
{
	m_cScheduleInfo=info;
}
void  CTG_TrainScheduleBase::GetScheduleInfo(CTG_ScheduleInfo &info) const
{
	info=m_cScheduleInfo;
}

void CTG_TrainScheduleBase::SetNeedSyncFlag(int sync_flag, BOOL va)
{
	if(!CHECK_VALID_SYNCFLAG(sync_flag))
		return;
	if(sync_flag==SYCFLAG_NULL)
		return;
	if(sync_flag==SYCFLAG_ALL)
		m_flagNeedToSync.SetAllBitStatus(va);
	else
		m_flagNeedToSync.SetBit(sync_flag,va);
}

BOOL   CTG_TrainScheduleBase::CheckAllSynced(const CTG_LogicObject *p) const
{
	return p->CheckMultiSyncStatus(m_flagNeedToSync.GetLongValue());
}

CTG_SingleTrain * CTG_TrainScheduleBase::GetSingleTrain(TRAIN_INDEX train_index,BOOL with_deleted,BOOL IsStart)
{
	if (train_index < 1) return NULL;

	IterMapTrain iter = m_mapTrains.find(train_index);

	if (iter != m_mapTrains.end())
	{
		if(iter->second.IsDeletedObject() && !with_deleted)
			return NULL;
		else
			return &iter->second;
	}
	else
		return NULL;
}
CTG_SingleMark * CTG_TrainScheduleBase::GetSingleMark(TRAIN_INDEX block_index,BOOL with_deleted)
{
	if (block_index < 1) return NULL;

	IterMapMark iter = m_mapMarks.find(block_index);

	if (iter != m_mapMarks.end())
	{
		if(iter->second.IsDeletedObject() && !with_deleted)
			return NULL;
		else
			return &iter->second;
	}
	else
		return NULL;
}

TRAIN_INDEX   CTG_TrainScheduleBase::AppendTrain(const CTG_ScheduleInfo &schd,const CTG_SingleTrain  &train,int source_flag)
{
	if(!m_cScheduleInfo.IsEqualScheduleInfo(schd)) 
		return NULL_TRAININDEX;
	IterMapTrain iter; 
	if(train.m_nTrainIndex!=NULL_TRAININDEX &&
		(iter=m_mapTrains.find(train.m_nTrainIndex))!= m_mapTrains.end())
	{
		if(iter->second.IsDeletedObject())
		{
			iter->second.SetDeletedFlag(FALSE);
			iter->second.SetSyncStatusX(source_flag,TRUE);
		}
		UpdateTrain(schd,train,source_flag); // 当存在时直接更新，是否更合理 20090911, 当接受基本图时，如果已经删除，并且未同步时，无法重新获取添加
		return train.m_nTrainIndex;
	}

	// 基本图不判定时间范围
    if (!IS_BASIC_SCHD(m_cScheduleInfo.m_nType)) 
	{
		TIME nStartTime=0, nEndTime=0;
		m_cScheduleInfo.GetTermAcceptSchdTimeRange(nStartTime, nEndTime);
	
		if(!train.IfInTimeScope(nStartTime, nEndTime)) 
		{
			CTime btm(nStartTime), etm(nEndTime);
			GpcLogTrainMessage(0, 0, "train %s schd %d 增加列车, 该列车不在 调度时间范围:%s-%s", train.GetTrainID(),  
				m_cScheduleInfo.m_nType, btm.Format("%Y-%m-%d %H:%M:%S"), etm.Format("%Y-%m-%d %H:%M:%S"));
			return NULL_TRAININDEX;
		}
	}

	TRAIN_INDEX index=train.m_nTrainIndex;
	if(index==NULL_TRAININDEX) // new append
		index=GetAvaTrainIndex();
	if(index==NULL_TRAININDEX)
	{
		CString str;
		str.Format("列车 %s 由于生成索引号无效,故暂时不能加车,请联系相关人员!",train.GetTrainID());
		AfxMessageBox(str);
		GpcLogTrainMessage(0, 0, "train %s schd %d 增加列车, 该列车索引号无效，故无法加车", train.GetTrainID(),  m_cScheduleInfo.m_nType);
		return NULL_TRAININDEX;
	}
	m_mapTrains[index] = train;
	m_mapTrains[index].SetTrainIndex(index); 
	//m_mapTrains[index].SetGPCTrainIndex(GetAvaGPCIndex());
	m_mapTrains[index].SetSyncStatusX(source_flag,TRUE);
	CTG_SingleTrain &dest_train=m_mapTrains[index];
	if(strlen(dest_train.m_cTrainInfo.m_data.train_name)<=0)
		strncpy(dest_train.m_cTrainInfo.m_data.train_name,dest_train.GetTrainID(),sizeof(dest_train.m_cTrainInfo.m_data.train_name)); // GetTrainID()可以从RECORD中获取车次 
	if(dest_train.GetTrainInfo()->m_data.delete_flag<0 || dest_train.GetTrainInfo()->m_data.delete_flag>dest_train.GetRecordList()->GetRecordCount())
	{
		dest_train.GetTrainInfo()->m_data.delete_flag=dest_train.GetRecordList()->GetRecordCount();
	}
	if(m_nFileSaveFlag)
		TG_SaveTrainToFile(m_cScheduleInfo,&dest_train);

	m_staList.UpdateTrain(&dest_train);  // 更新车站索引列车数据

	return index;
}

TRAIN_INDEX   CTG_TrainScheduleBase::AppendMark(const CTG_ScheduleInfo &schd,const CTG_SingleMark  &block_info,int source_flag)
{
	if(!m_cScheduleInfo.IsEqualScheduleInfo(schd)) 
		return NULL_TRAININDEX;
	if(!IS_BASIC_SCHD(m_cScheduleInfo.m_nType)) 
	{
		TIME nStartTime=0, nEndTime=0, nNoUseEndTime=0;
		m_cScheduleInfo.GetTermAcceptSchdTimeRange(nStartTime, nEndTime);

		if(!block_info.IfInTimeScope(nStartTime, nEndTime)) 
		{
			CTime btm(nStartTime), etm(nEndTime);
			GpcLogTrainMessage(0, 0, "block %d schd %d 增加施工符号, 该符号不在 调度时间范围:%s(%d)-%s(%d)", block_info.m_nBlockIndex,  
				m_cScheduleInfo.m_nType, btm.Format("%Y-%m-%d %H:%M:%S"), nStartTime, etm.Format("%Y-%m-%d %H:%M:%S"), nEndTime);
			return NULL_TRAININDEX;
		}
	}
	if(block_info.m_nBlockIndex!=NULL_TRAININDEX &&
		m_mapMarks.find(block_info.m_nBlockIndex)!= m_mapMarks.end())
		return NULL_TRAININDEX;
	TRAIN_INDEX index=block_info.m_nBlockIndex;
	if(index==NULL_TRAININDEX) // new append
		index=GetAvaTrainIndex();
	if(index==NULL_TRAININDEX)
	{
		CString str;
		str.Format("施工符号由于生成索引号无效,故暂时不能加车,请联系相关人员!");
		AfxMessageBox(str);
		GpcLogTrainMessage(0, 0, "增加施工符号时索引号无效，故无法增加");
		return NULL_TRAININDEX;
	}
	m_mapMarks[index] = block_info;
	m_mapMarks[index].SetBlockIndex(index);
	//m_mapMarks[index].SetGPCTrainIndex(GetAvaGPCIndex());
	m_mapMarks[index].SetSyncStatusX(source_flag,TRUE);
	if(m_nFileSaveFlag)
		TG_SaveMarkToFile(m_cScheduleInfo,&m_mapMarks[index]);
	return index; // 此处原为TRUE, BUG!
}

BOOL   CTG_TrainScheduleBase::SaveExpandTrain(const CTG_ScheduleInfo &schd,const CTG_SingleTrain*  ptrain)
{
	if (ptrain==NULL) return FALSE;
	if (ptrain->m_nTrainIndex < 1) return FALSE;
	if(!m_cScheduleInfo.IsEqualScheduleInfo(schd)) 
		return FALSE;
	
	if(m_nFileSaveFlag)
		TG_SaveTrainToFile(m_cScheduleInfo,ptrain);
	return TRUE;
}

BOOL   CTG_TrainScheduleBase::UpdateTrain(const CTG_ScheduleInfo &schd,const CTG_SingleTrain  &train,int source_flag)
{
	if (train.m_nTrainIndex < 1) return FALSE;
	if(!m_cScheduleInfo.IsEqualScheduleInfo(schd)) 
		return FALSE;
	IterMapTrain iter = m_mapTrains.find(train.m_nTrainIndex);
	if (iter != m_mapTrains.end())
	{
		if(m_nFileSaveFlag)
			TG_SaveTrainToFile(m_cScheduleInfo,&train);
		if(iter->second.UpdateTrain(train,source_flag))
		{
			m_staList.UpdateTrain(&iter->second);  // 更新车站索引列车数据
			return TRUE;
		}
		else
			return FALSE;
	}
	else
	{
		if(NULL_TRAININDEX!=AppendTrain(schd,train,source_flag))
			return TRUE;
		else
			return FALSE;
	}
}

CTG_SingleTrain * CTG_TrainScheduleBase::GetSingleTrainForNeib(TRAIN_INDEX train_index)
{
	if (train_index < 1) return NULL;

	IterMapTrain iter = m_mapTrains.begin();
	for ( ; iter != m_mapTrains.end(); iter++)
	{
		if(iter->second.IsDeletedObject())
			continue;
		
		if(iter->second.m_nCurrConTrainIndex==train_index)
			return &iter->second;
	}
	
	return NULL;
}

void   CTG_TrainScheduleBase::SetTrainFromOPMS(const CTG_ScheduleInfo &schd,const CTG_SingleTrain  &train)
{
	if (train.m_nTrainIndex < 1) return;
	if(!m_cScheduleInfo.IsEqualScheduleInfo(schd)) 
		return;
	IterMapTrain iter = m_mapTrains.find(train.m_nTrainIndex);
	if (iter != m_mapTrains.end())
	{
		iter->second.m_strSource="日班计划";
		GpcLogTrainMessage(0, 0, "train %s train_index %u shift_index %u 邻台计划 设置源 日班计划", train.GetTrainID(), train.m_nTrainIndex, train.GetShiftTrainIndex());
	}
	else
	{
		GpcLogTrainMessage(0, 0, "train %s train_index %u shift_index %u 邻台计划 设置源 日班计划 失败, 找不到该列车", train.GetTrainID(), train.m_nTrainIndex, train.GetShiftTrainIndex());
	}
}

BOOL   CTG_TrainScheduleBase::UpdateTrainInfo(const CTG_ScheduleInfo &schd,TRAIN_INDEX train_index,const CTG_TrainInfo  &train_info,int source_flag)
{
	if (train_index < 1) return FALSE;
	if(!m_cScheduleInfo.IsEqualScheduleInfo(schd)) 
		return FALSE;
	IterMapTrain iter = m_mapTrains.find(train_index);
	if (iter != m_mapTrains.end())
		return iter->second.UpdateTrainInfo(train_info, source_flag);
	else
		return FALSE;
}

BOOL   CTG_TrainScheduleBase::UpdateTrainInfo(const CTG_ScheduleInfo &schd,const CTG_SingleTrain  &train,int source_flag)
{
	return UpdateTrainInfo(schd,train.m_nTrainIndex,train.m_cTrainInfo,source_flag);
}

BOOL   CTG_TrainScheduleBase::UpdateTrainRecords(const CTG_ScheduleInfo &schd,TRAIN_INDEX train_index,const CTG_TrainRecordList  &record_list,int source_flag)
{
	if (train_index < 1) return FALSE;
	if(!m_cScheduleInfo.IsEqualScheduleInfo(schd)) 
		return FALSE;
	IterMapTrain iter = m_mapTrains.find(train_index);
	if (iter != m_mapTrains.end())
		return iter->second.UpdateTrainRecords(record_list, source_flag);
	else
		return FALSE;
}
BOOL   CTG_TrainScheduleBase::UpdateTrainRecords(const CTG_ScheduleInfo &schd,const CTG_SingleTrain  &train,int source_flag)
{
	return UpdateTrainRecords(schd,train.m_nTrainIndex,train.m_cRecordList,source_flag);
}

BOOL   CTG_TrainScheduleBase::UpdateEngineInfo(const CTG_ScheduleInfo &schd,TRAIN_INDEX train_index,const CTG_TrainEngineList &trainengine,int source_flag)
	{
	if (train_index < 1) return FALSE;
	if(!m_cScheduleInfo.IsEqualScheduleInfo(schd)) 
		return FALSE;
	IterMapTrain iter = m_mapTrains.find(train_index);
	if (iter != m_mapTrains.end())
		return iter->second.UpdateEngineInfo(trainengine, source_flag);
	else
		return FALSE;
	}

BOOL   CTG_TrainScheduleBase::UpdateEngineInfo(const CTG_ScheduleInfo &schd,const CTG_SingleTrain  &train,int source_flag)
{
	return UpdateEngineInfo(schd,train.m_nTrainIndex,train.m_cEngineList,source_flag);
}

BOOL   CTG_TrainScheduleBase::UpdateMark(const CTG_ScheduleInfo &schd,TRAIN_INDEX block_index,const CTG_SingleMark  &block_info,int source_flag)
{
	if (block_index < 1) return FALSE;
	if(!m_cScheduleInfo.IsEqualScheduleInfo(schd)) 
		return FALSE;
	IterMapMark iter = m_mapMarks.find(block_index);
	if (iter != m_mapMarks.end())
	{
		if(iter->second==block_info)   //可能是服务器操作反馈，更新同步标志
		{
			iter->second.SetSyncStatus(source_flag,TRUE);
		}
		else
		{
			iter->second=block_info;
			iter->second.SetSyncStatusX(source_flag,TRUE);
		}
		return TRUE;
	}
	else
		return FALSE;
}

BOOL   CTG_TrainScheduleBase::UpdateMark(const CTG_ScheduleInfo &schd,const CTG_SingleMark  &block_info,int source_flag)
{
	if(UpdateMark(schd,block_info.m_nBlockIndex, block_info,source_flag))
	{
		if(m_nFileSaveFlag)
			TG_SaveMarkToFile(m_cScheduleInfo,&block_info);
		return TRUE;
	}
	else
		return AppendMark(schd,block_info,source_flag);
}

BOOL   CTG_TrainScheduleBase::DeleteTrain(const CTG_ScheduleInfo &schd,TRAIN_INDEX train_index,int source_flag)
{
	if (train_index < 1) return FALSE;
	if(!m_cScheduleInfo.IsEqualScheduleInfo(schd)) 
		return FALSE;
	IterMapTrain iter = m_mapTrains.find(train_index);
	if (iter != m_mapTrains.end())
	{
		if(iter->second.IsDeletedObject()) // 可能是服务器操作反馈，更新同步标志
			iter->second.SetSyncStatus(source_flag,TRUE);
		else
		{
		  iter->second.SetDeletedFlag(TRUE);
		  iter->second.SetSyncStatusX(source_flag,TRUE);

		  m_staList.DeleteTrain(train_index);  // 更新车站索引的列车数据
		}
		return TRUE;
	}
	else
		return FALSE;
}


BOOL   CTG_TrainScheduleBase::DeleteTrain(const CTG_ScheduleInfo &schd,const CTG_SingleTrain  &train,int source_flag)
{
	return DeleteTrain(schd,train.m_nTrainIndex,source_flag); 
}

BOOL   CTG_TrainScheduleBase::DeleteMark(const CTG_ScheduleInfo &schd,TRAIN_INDEX block_index,int source_flag)
{
	if (block_index < 1) return FALSE;
	if(!m_cScheduleInfo.IsEqualScheduleInfo(schd)) 
		return FALSE;
//	m_mapMarks.erase(block_index);
	IterMapMark iter = m_mapMarks.find(block_index);
	if (iter != m_mapMarks.end())
	{
		if(iter->second.IsDeletedObject())  // 可能是服务器操作反馈，更新同步标志
			iter->second.SetSyncStatus(source_flag,TRUE);
		else
		{
		iter->second.SetDeletedFlag(TRUE);
		iter->second.SetSyncStatusX(source_flag,TRUE);
		}
		return TRUE;
	}
	else
		return FALSE;
}
BOOL   CTG_TrainScheduleBase::DeleteMark(const CTG_ScheduleInfo &schd,const CTG_SingleMark  &block_info,int source_flag)
{
	return DeleteMark(schd,block_info.m_nBlockIndex,source_flag); 
}



BOOL   CTG_TrainScheduleBase::DeleteAllData(const CTG_ScheduleInfo &schd,int source_flag)
{
	//return FALSE;
	//if(!(m_cScheduleInfo==schd))   // 无条件删除
	//	return FALSE;
	m_mapTrains.clear();	
	m_mapMarks.clear();
	m_staList.DeleteAllTrains(); // 更新车站索引的列车数据
	return TRUE;
}

BOOL   CTG_TrainScheduleBase::CopyAllData(const CTG_ScheduleInfo &schd,IScheduleComm *pOut,BOOL all_flag)
{
	if(gpcdef.IsNewAutoMoveMode())
	{
		return CopyAllDataNew(schd, pOut, all_flag);
	}

	if(!m_cScheduleInfo.IsEqualScheduleInfo(schd)) 
		return FALSE;

	do
	{
		IterMapTrain train_iter=FirstTrain();
		while(train_iter!=LastTrain())
		{
			if(!train_iter->second.IsDeletedObject())
			{
				if(all_flag || !train_iter->second.CheckSyncStatus(pOut->GetInterfaceFlag()))
				{
					if(all_flag)
					{
						if(pOut->AppendTrain(schd,train_iter->second,GetInterfaceFlag()))
							train_iter->second.SetSyncStatus(pOut->GetInterfaceFlag(),TRUE);
					}
					else if(pOut->UpdateTrain(schd,train_iter->second,GetInterfaceFlag()))
						train_iter->second.SetSyncStatus(pOut->GetInterfaceFlag(),TRUE);
				}
			}
			else
			{
				train_iter->second.SetSyncStatus(pOut->GetInterfaceFlag(),TRUE);
			}
			train_iter++;
		}
		IterMapMark  mark_iter=FirstMark();
		while(mark_iter!=LastMark())
		{
			if(!mark_iter->second.IsDeletedObject())
			{
				if(all_flag || !mark_iter->second.CheckSyncStatus(pOut->GetInterfaceFlag()))
				{
					if(all_flag)
					{
						if(pOut->AppendMark(schd,mark_iter->second,GetInterfaceFlag()))
							mark_iter->second.SetSyncStatus(pOut->GetInterfaceFlag(),TRUE);
					}
					else if(pOut->UpdateMark(schd,mark_iter->second,GetInterfaceFlag()))
						mark_iter->second.SetSyncStatus(pOut->GetInterfaceFlag(),TRUE);
				}
			}
			else
			{
				mark_iter->second.SetSyncStatus(pOut->GetInterfaceFlag(),TRUE);
			}
			mark_iter++;
		}
		return TRUE;
	}while(0);
	return FALSE;
}

//////////////////////////////////////////////////////////////////////////
BOOL CTG_TrainScheduleBase::CopyAllDataNew(const CTG_ScheduleInfo &schd,IScheduleComm *pOut,BOOL all_flag)
{
	if(!m_cScheduleInfo.IsEqualScheduleInfo(schd)) 
		return FALSE;
	
	TIME nStartTime=0, nEndTime=0;
	m_cScheduleInfo.GetViewScopeRange(nStartTime, nEndTime);
	
	CTime btm(nStartTime), etm(nEndTime);
	GpcLogTrainMessage(0, 0, "交接班 首先将内存中section=%d schdtype=%d的[%s - %s]数据提交给OCX.",
				m_cScheduleInfo.m_nSectionID, m_cScheduleInfo.m_nType,
				btm.Format("%Y-%m-%d %H:%M:%S"), etm.Format("%Y-%m-%d %H:%M:%S"));
	do
	{
		IterMapTrain train_iter=FirstTrain();
		while(train_iter!=LastTrain())
		{
			if(!train_iter->second.IsDeletedObject())
			{
				if((all_flag || !train_iter->second.CheckSyncStatus(pOut->GetInterfaceFlag())) 
					&&	train_iter->second.IfInTimeScope(nStartTime, nEndTime))
				{
					if(all_flag)
					{
						if(pOut->AppendTrain(schd,train_iter->second,GetInterfaceFlag()))
							train_iter->second.SetSyncStatus(pOut->GetInterfaceFlag(),TRUE);
					}
					else
					{
						if(pOut->UpdateTrain(schd,train_iter->second,GetInterfaceFlag()))
							train_iter->second.SetSyncStatus(pOut->GetInterfaceFlag(),TRUE);
					}
				}
			}
			else
			{
				train_iter->second.SetSyncStatus(pOut->GetInterfaceFlag(),TRUE);
			}
			train_iter++;
		}
		IterMapMark  mark_iter=FirstMark();
		while(mark_iter!=LastMark())
		{
			if(!mark_iter->second.IsDeletedObject())
			{
				if((all_flag || !mark_iter->second.CheckSyncStatus(pOut->GetInterfaceFlag()))
					&& mark_iter->second.IfInTimeScope(nStartTime, nEndTime))
				{
					if(all_flag)
					{
						if(pOut->AppendMark(schd,mark_iter->second,GetInterfaceFlag()))
							mark_iter->second.SetSyncStatus(pOut->GetInterfaceFlag(),TRUE);
					}
					else
					{
						if(pOut->UpdateMark(schd,mark_iter->second,GetInterfaceFlag()))
							mark_iter->second.SetSyncStatus(pOut->GetInterfaceFlag(),TRUE);
					}
				}
			}
			else
			{
				mark_iter->second.SetSyncStatus(pOut->GetInterfaceFlag(),TRUE);
			}
			mark_iter++;
		}	
		return TRUE;
	}while(0);

	return FALSE;
}

int    CTG_TrainScheduleBase::GetTrainAndMarkCount() const
{
	 return m_mapMarks.size()+m_mapTrains.size();
}
//////////////////////////////////////////////////////////////////////////

int CTG_TrainScheduleBase::GetNoSyncTrainCount(int sync_flag,CString &info)
{
	 int cnt=0;
	 CString tmp;
	 IterMapTrain train_iter=FirstTrain();
	 while(train_iter!=LastTrain())
	 {
		 if(!train_iter->second.CheckSyncStatus(sync_flag))
		 {
			cnt++;
			if(cnt>0)
			{
				if (cnt == 1) 
				{
					tmp.Format(" TRAIN: %s[%u]",train_iter->second.GetTrainID(),train_iter->second.GetTrainIndex());
					info += tmp;
				}
				else
				{
					tmp.Format(",%s[%u]",train_iter->second.GetTrainID(),train_iter->second.GetTrainIndex());
					info += tmp;
				}
			}
	  	 }
		 train_iter++;
	 }
	return cnt;
}

int CTG_TrainScheduleBase::GetNoSyncMarkCount(int sync_flag,CString &info)
{
	 int cnt=0;
	 CString tmp;
	 IterMapMark  mark_iter=FirstMark();
	 while(mark_iter!=LastMark())
	 {
		 if(!mark_iter->second.CheckSyncStatus(sync_flag))
		 {
			cnt++;
			if(cnt>0)
			{
				if (cnt == 1) 
				{
					tmp.Format(" MARK: %d",mark_iter->second.GetBlockIndex());
					info += tmp;
				}
				else
				{
					tmp.Format(",%d",mark_iter->second.GetBlockIndex());
					info += tmp;
				}
			}
		 }
		 mark_iter++;
	 }
	 return cnt;
}

TRAIN_INDEX CTG_TrainScheduleBase::GetMaxTrainIndex()
{
	 TRAIN_INDEX rc=0;
	 IterMapTrain train_iter=FirstTrain();
	 while(train_iter!=LastTrain())
	 {
		 if(train_iter->second.GetTrainIndex()>rc)
			 rc=train_iter->second.GetTrainIndex();
		 train_iter++;
	 }
	 return rc;
}

TRAIN_INDEX CTG_TrainScheduleBase::GetMaxBlockIndex()
{
	TRAIN_INDEX rc=0;
    IterMapMark  mark_iter=FirstMark();
	 while(mark_iter!=LastMark())
	 {
		 if(mark_iter->second.m_nBlockIndex>rc)
			 rc=mark_iter->second.m_nBlockIndex;
		 mark_iter++;
	 }
	 return rc;
}
CTG_SingleTrain *CTG_TrainScheduleBase::Find1NoSyncTrain(CTG_ScheduleInfo *schd,int source_flag,BOOL include_syncing)
{
	IterMapTrain train_iter=FirstTrain();
	 while(train_iter!=LastTrain())
	 {
		 if(!train_iter->second.CheckSyncStatus(source_flag))
		 {
		   if(include_syncing || !train_iter->second.IsSyncing(source_flag))
		   {
		    *schd=m_cScheduleInfo;
			return &train_iter->second;
		   }
	  	 }
		train_iter++;
	 }
	 return NULL;
}

CTG_SingleMark *CTG_TrainScheduleBase::Find1NoSyncMark(CTG_ScheduleInfo *schd,int source_flag,BOOL include_syncing)
{
	int cnt=0;
	 IterMapMark  mark_iter=FirstMark();
	 while(mark_iter!=LastMark())
	 {
		 if(!mark_iter->second.CheckSyncStatus(source_flag))
		 {
		   if(include_syncing || !mark_iter->second.IsSyncing(source_flag))
		   {
			 *schd=m_cScheduleInfo;
			return &mark_iter->second;
		   }
		 }
		 mark_iter++;
	 }
	 return NULL;
}

BOOL   CTG_TrainScheduleBase::PollingForUpdate(BOOL all_flag,TIME curtm)
{
	 IterMapTrain train_iter=FirstTrain();
	 while(train_iter!=LastTrain())
	 {
		 if(curtm>0 && !train_iter->second.IsDeletedObject() && train_iter->second.GetTrainRecordCount()<=0)  // 512计划优化
			{
				if(train_iter->second.GetUpdateTime()<(curtm-4*3600)) // 删除四小时前
				{
					 train_iter->second.SetDeletedFlag(TRUE);
					 train_iter->second.SetSyncStatusX(SYCFLAG_NULL,TRUE);
					 GpcLogTrainMessage(0, 0, "PollingForUpdate SetDeletedFlag(TRUE) section=%d schdtype=%d train_index=%u", 
						 m_cScheduleInfo.m_nSectionID, m_cScheduleInfo.m_nType, train_iter->second.GetTrainIndex());
				}
			}

		 if(train_iter->second.IsDeletedObject() && 
			 train_iter->second.CheckMultiSyncStatus(m_flagNeedToSync.GetLongValue()) &&  // 检查整体同步
			 train_iter->second.CheckSyncStatus(SYCFLAG_ALL)) //检查个别需要同步
		 {
		   if(m_nFileSaveFlag)
				 TG_DeleteTrainFile(m_cScheduleInfo,train_iter->first);
		   GpcLogTrainMessage(0, 0, "PollingForUpdate Delete train from term memory section=%d schdtype=%d train_index=%u", 
					m_cScheduleInfo.m_nSectionID, m_cScheduleInfo.m_nType, train_iter->second.GetTrainIndex());
		   train_iter=m_mapTrains.erase(train_iter);
	  	 }
		 else
		train_iter++;
	 }
	 IterMapMark  mark_iter=FirstMark();
	 while(mark_iter!=LastMark())
	 {
		  if(mark_iter->second.IsDeletedObject() && 
			 mark_iter->second.CheckMultiSyncStatus(m_flagNeedToSync.GetLongValue()) &&
			 mark_iter->second.CheckSyncStatus(SYCFLAG_ALL))
		 {
		  if(m_nFileSaveFlag)
				 TG_DeleteMarkFile(m_cScheduleInfo,mark_iter->first);
		   mark_iter=m_mapMarks.erase(mark_iter);
	  	 }
		 else
		mark_iter++;
	 }

	 m_staList.DeleteObsoleteTrains(curtm-3600); // 更新车站索引的列车数据,删除过时的数据，暂定1小时前为过时数据
	 return TRUE;
}

CTG_TrainScheduleBase::IterMapTrain CTG_TrainScheduleBase::FindMatchedTrain(const CTG_SingleTrain &srctrain, int flag,const IterMapTrain &prev)  // flag DEFINED AS MATCH_TRAIN_XXXXXX
{
	 IterMapTrain train_iter=prev;
	 while(train_iter!=LastTrain())
	 {
		 do
		 {
			 if(train_iter->second.IsDeletedObject())
				 break;

		     if(flag & MATCH_FLAG_TRAIN_ID)
			 {
			   if(stricmp(train_iter->second.GetTrainID(),srctrain.GetTrainID()))
			  	  break;
			 }
			 if(flag & MATCH_FLAG_LOCAL_CREATED)
			 {
				 if(!train_iter->second.IsLocalCreated()) 
					 break;
			 }
			 return  train_iter;
		 }while(0);
		train_iter++;
	 }		
	 return train_iter;
}

CTG_SingleTrain *CTG_TrainScheduleBase::GetSingleTrain(const TRAIN_NO train_no)
{
	IterMapTrain iter=FirstTrain();
	while(iter!=LastTrain())
	{
		if(!iter->second.IsDeletedObject())  
		{
		if(iter->second.MatchTrainID(train_no))
			return &iter->second;
		}
		iter++;
	}
	return NULL;
}	

int CTG_TrainScheduleBase::GetAllTrainCount()
{
	int count=0;
	IterMapTrain iter=FirstTrain();
	while(iter!=LastTrain())
	{
		if(!iter->second.IsDeletedObject())  
		{
			count++;
		}
		iter++;
	}

	return count;
}

int  CTG_TrainScheduleBase::GetAllMarkCount()
{
	int count=0;
	IterMapMark iter=FirstMark();
	while(iter!=LastMark())
	{
		if(!iter->second.IsDeletedObject())  
		{
			count++;
		}
		iter++;
	}

	return count;
}

BOOL   CTG_TrainScheduleBase::FindMarks(CTG_ObjectPtrGroup &result,TIME begin_time,TIME end_time)
{
	IterMapMark iter=FirstMark();
	while(iter!=LastMark())
	{
		if(!iter->second.IsDeletedObject())  
		{
		if((begin_time==NULL_TIME && end_time==NULL_TIME) || iter->second.IfInTimeScope(begin_time,end_time))
				result.AddPtr(&iter->second);
		}
		iter++;
	}
	if(result.GetCount()>0)
		return TRUE;
	else
		return FALSE;
}

BOOL   CTG_TrainScheduleBase::FindMarksByShiftString(CTG_ObjectPtrGroup &result,const char *planid)
{
	if(!planid)
		return FALSE;
	if(strlen(planid)==0)
		return FALSE;
	IterMapMark iter=FirstMark();
	while(iter!=LastMark())
	{
		if(!iter->second.IsDeletedObject())  
		{
			if(!stricmp(iter->second.GetShiftIndexString(),planid))
				result.AddPtr(&iter->second);
		}
		iter++;
	}
	if(result.GetCount()>0)
		return TRUE;
	else
		return FALSE;
}

BOOL   CTG_TrainScheduleBase::FindTrainsHasStation(CTG_TrainPtrGroup &result,STATION_NO station)
{
    IterMapTrain iter=FirstTrain();
	while(iter!=LastTrain())
	{
		if(iter->second.IsDeletedObject())
		{
			iter++;
			continue;
		}

		CTG_TrainRecord *prec=iter->second.GetTrainRecordByStation(station);
		if(NULL!=prec)
		{
			result.AddTrainPtrNotOrder(&iter->second);
		}
		iter++;
	}
	if(result.GetTrainCount()>0)
		return TRUE;
	else
		return FALSE;
}
BOOL   CTG_TrainScheduleBase::FindAllTrains(CTG_TrainPtrGroup &result)
{
	IterMapTrain iter=FirstTrain();
	while(iter!=LastTrain())
	{
		if(iter->second.IsDeletedObject())
		{
			iter++;
			continue;
		}

		result.AddTrainPtrNotOrder(&iter->second);
		iter++;
	}
	//result.SetTrainsPtr();
	if(result.GetTrainCount()>0)
		return TRUE;
	else
		return FALSE;
	
}

BOOL   CTG_TrainScheduleBase::FindTrainsForStplan(CTG_TrainPtrGroup &result,STATION_NO station,TIME begin_time,TIME end_time)
{
	IterMapTrain iter=FirstTrain();
	while(iter!=LastTrain())
	{
		if(iter->second.IsDeletedObject())
		{
			iter++;
			continue;
		}

		// 查找车站
		int cnt=iter->second.GetTrainRecordCountWithDeleted();
		for(int i=0;i<cnt;i++)
		{
			CTG_TrainRecord *prec=iter->second.GetTrainRecord(i);
			if(prec && prec->GetStationNo()== station)
			{
				if((begin_time==NULL_TIME && end_time==NULL_TIME) || prec->IfInTimeScope (begin_time,end_time))
				{
					result.AddTrainPtr(&iter->second);
					break;
				}
			}
		}
		
		iter++;
	}
	result.SetTrainsPtr();
	if(result.GetTrainCount()>0)
		return TRUE;
	else
		return FALSE;
}

BOOL   CTG_TrainScheduleBase::FindTrainsByEndTm(CTG_TrainPtrGroup &result,STATION_NO station,TIME begin_time,TIME end_time,BOOL include_deleted,BOOL IsStart)
{
	IterMapTrain iter=FirstTrain();
	while(iter!=LastTrain())
	{
		if(iter->second.IsDeletedObject())
		{
			iter++;
			continue;
		}
		if(!include_deleted && iter->second.GetTrainRecordCount()<=0)
		{
			iter++;
			continue;
		}
		if(station!=NULL_STATION)
		{
			int i;
			int cnt;
			CTG_TrainRecord *prec=NULL;
			if(include_deleted) cnt=iter->second.GetTrainRecordCountWithDeleted();
			else cnt=iter->second.GetTrainRecordCount();
			for(i=0;i< cnt;i++)
			{
				prec=iter->second.GetTrainRecord(i);
				if(prec && prec->GetStationNo()== station)
					break;
			}
			if(i==cnt)
			{
				iter++;
				continue;
			}
			if((begin_time==NULL_TIME && end_time==NULL_TIME) || prec->IfInTimeScope(begin_time,end_time))
				result.AddTrainPtrByEndTmOrder(&iter->second);
		}
		else
		if((begin_time==NULL_TIME && end_time==NULL_TIME) || iter->second.IfInTimeScope(begin_time,end_time,IsStart))
				result.AddTrainPtrByEndTmOrder(&iter->second);
		iter++;
	}
	result.SetTrainsPtr();
	if(result.GetTrainCount()>0)
		return TRUE;
	else
		return FALSE;
}

BOOL   CTG_TrainScheduleBase::FindPessangerTrains(CTG_TrainPtrGroup &result)
{
	IterMapTrain iter=FirstTrain();
	while(iter!=LastTrain())
	{
		if(iter->second.IsDeletedObject())
		{
			iter++;
			continue;
		}
		
		if(iter->second.IsPassengerTrain())
			result.AddTrainPtr(&iter->second);
		iter++;
	}
	result.SetTrainsPtr();
	if(result.GetTrainCount()>0)
		return TRUE;
	else
		return FALSE;

}

BOOL   CTG_TrainScheduleBase::FindTrains(CTG_TrainPtrGroup &result,STATION_NO station,TIME begin_time,TIME end_time,BOOL include_deleted,BOOL IsStart)
{
	IterMapTrain iter=FirstTrain();
	while(iter!=LastTrain())
	{
		if(iter->second.IsDeletedObject())
		{
			iter++;
			continue;
		}
		if(!include_deleted && iter->second.GetTrainRecordCount()<=0)
		{
			iter++;
			continue;
		}
		if(station!=NULL_STATION)
		{
			int i;
			int cnt;
			CTG_TrainRecord *prec=NULL;
			if(include_deleted) cnt=iter->second.GetTrainRecordCountWithDeleted();
			else cnt=iter->second.GetTrainRecordCount();
			for(i=0;i< cnt;i++)
			{
				prec=iter->second.GetTrainRecord(i);
				if(prec && prec->GetStationNo()== station)
					break;
			}
			if(i==cnt)
			{
				iter++;
				continue;
			}
			if((begin_time==NULL_TIME && end_time==NULL_TIME) || prec->IfInTimeScope(begin_time,end_time))
				result.AddTrainPtr(&iter->second);
		}
		else
		if((begin_time==NULL_TIME && end_time==NULL_TIME) || iter->second.IfInTimeScope(begin_time,end_time,IsStart))
				result.AddTrainPtr(&iter->second);
		iter++;
	}
	result.SetTrainsPtr();
	if(result.GetTrainCount()>0)
		return TRUE;
	else
		return FALSE;

}

BOOL   CTG_TrainScheduleBase::FindTrainsNotOrder(CTG_TrainPtrGroup &result,STATION_NO station,TIME begin_time,TIME end_time,BOOL include_deleted,BOOL IsStart)
{
	IterMapTrain iter=FirstTrain();
	while(iter!=LastTrain())
	{
		if(iter->second.IsDeletedObject())
		{
			iter++;
			continue;
		}
		if(!include_deleted && iter->second.GetTrainRecordCount()<=0)
		{
			iter++;
			continue;
		}

		if(station!=NULL_STATION)
		{
			int i;
			int cnt;
			CTG_TrainRecord *prec=NULL;
			if(include_deleted) cnt=iter->second.GetTrainRecordCountWithDeleted();
			else cnt=iter->second.GetTrainRecordCount();
			for(i=0;i< cnt;i++)
			{
				prec=iter->second.GetTrainRecord(i);
				if(prec && prec->GetStationNo()== station)
					break;
			}
			if(i==cnt)
			{
				iter++;
				continue;
			}
			if((begin_time==NULL_TIME && end_time==NULL_TIME) || prec->IfInTimeScope(begin_time,end_time))
				result.AddTrainPtrNotOrder(&iter->second);
		}
		else
			if((begin_time==NULL_TIME && end_time==NULL_TIME) || iter->second.IfInTimeScope(begin_time,end_time,IsStart))
				result.AddTrainPtrNotOrder(&iter->second);
		iter++;
	}

	if(result.GetTrainCount()>0)
		return TRUE;
	else
		return FALSE;

}

BOOL  CTG_TrainScheduleBase::FindTrainsForBasicStplan(CTG_TrainPtrGroup &result,STATION_NO station)
{
	IterMapTrain iter=FirstTrain();
	while(iter!=LastTrain())
	{
		if(iter->second.IsDeletedObject())
		{
			iter++;
			continue;
		}

		// 查找车站
		int cnt=iter->second.GetTrainRecordCount();
		for(int i=0;i<cnt;i++)
		{
			CTG_TrainRecord *prec=iter->second.GetTrainRecordNoDelete(i);
			if(prec && prec->GetStationNo()== station)
			{
				result.AddTrainPtrNotOrder(&iter->second);
				break;
			}
		}
		
		iter++;
	}

	if(result.GetTrainCount()>0)
		return TRUE;
	else
		return FALSE;
}

int    CTG_TrainScheduleBase::GetTrainCount1(int *passcnt, int *frecnt) const
{
	CMapTrain::const_iterator iter=m_mapTrains.begin();
	*passcnt=0;
	*frecnt=0;
	while(iter!=m_mapTrains.end())
	{
		if(iter->second.IsDeletedObject())
		{
			iter++;
			continue;
		}
		if(iter->second.IsPassengerTrain())
			(*passcnt)++;
		else
			(*frecnt)++;
		iter++;
	}
	return *frecnt+*passcnt;
}

int    CTG_TrainScheduleBase::GetMarkCount() const
{
	 return m_mapMarks.size();
}

TRAIN_INDEX  CTG_TrainScheduleBase::GetAvaGPCIndex()
{
	 TRAIN_INDEX rc=0;
	 IterMapTrain train_iter=FirstTrain();
	 while(train_iter!=LastTrain())
	 {
		 TRAIN_INDEX x=train_iter->second.GetGPCTrainIndex() & 0x7FFFFFFF;
		 if(x>=rc)
			 rc=x+1;
		 train_iter++;
	 }
	 IterMapMark mark_iter=FirstMark();
	 while(mark_iter!=LastMark())
	 {
		 TRAIN_INDEX x=mark_iter->second.GetGPCIndex() & 0x7FFFFFFF;
		 if(x>=rc)
			 rc=x+1;
		  mark_iter++;
	 }
	 if(rc>=0x80000000)
		 return 0;
	 return rc;	
}

// TrainIndex: [Gpcid(7 bits)]    [shift_date( 11bits) shift_no(1 bit)][index(13 bits)]
TRAIN_INDEX  CTG_TrainScheduleBase::GetAvaTrainIndex()
{
	 //TRAIN_INDEX curtm=time(NULL);
	 TRAIN_INDEX low13,high19;
	 if(m_nUserSpecial>0)
		 high19=m_nUserSpecial&0x3F;
	 else
		 high19=m_cScheduleInfo.m_nSectionID&0x3F;
	 //low24=curtm>>8;
	 long id=m_cScheduleInfo.m_nShiftID;
	 int CurYear  = id/1000000;
	 int CurMonth = id/10000-CurYear*100;
	 int CurDay   = id/100-(id/10000)*100;
	 int CurBanbie=id-(id/10)*10;

	 TRAIN_INDEX tmp=(CurYear-2009)*380+CurMonth*31+CurDay; 
	 tmp&=0x7FF;
	 tmp<<=1;
	 if(CurBanbie==2)
 		tmp|=0x01; 
	 // tmp 是班别共12BITs
	 high19<<=12;
	 high19|=tmp;

	 low13=0;

	 IterMapTrain train_iter=FirstTrain();
	 while(train_iter!=LastTrain())
	 {
		  TRAIN_INDEX x_high=train_iter->second.GetTrainIndex()>>13;
		  TRAIN_INDEX x_low=train_iter->second.GetTrainIndex() & 0x1FFF;
		  if(x_high==high19 && x_low>=low13)
		  {
		    low13=x_low+1;
		  }
		 train_iter++;
	 }
	 IterMapMark mark_iter=FirstMark();
	 while(mark_iter!=LastMark())
	 {
		 TRAIN_INDEX x_high=mark_iter->second.GetBlockIndex() >>13;
		 TRAIN_INDEX x_low=mark_iter->second.GetBlockIndex() & 0x1FFF;
		  if(x_high==high19 && x_low>=low13)
		  {
		    low13=x_low+1;
		  }
		 mark_iter++;
	 }

	 return( (high19<<13)|low13);
}

// 勾连车组关系使用
CTG_SingleTrain *CTG_TrainScheduleBase::GetNextTrainByShiftIndex(TRAIN_INDEX train_index,STATION_NO station)
{
	if(train_index==0)
		return NULL;

	IterMapTrain iter=FirstTrain();
	TRAIN_INDEX prev_shift_index=0;
	while(iter!=LastTrain())
	{
		prev_shift_index=iter->second.GetPrevTrainShiftTrainIndex();
		if(!iter->second.IsDeletedObject() && (prev_shift_index!=0) 
			&&  (prev_shift_index & 0x3FFFFFFF)==(train_index&0x3FFFFFFF) // 20141204 车次分割 30bit为1,解决南宁全局ID分割重复
			&& iter->second.GetTrainRecordCount())
		{
			CTG_SingleTrain *ptrain=&iter->second;
			CTG_TrainRecord *prec=ptrain->GetTrainRecord(0);
			if(prec && prec->GetStationNo()!=station)
			{
				iter++;
				continue;
			}
			
			return ptrain;
		}
		iter++;
	}	
	
	return NULL;
}

// 勾连车组关系使用
CTG_SingleTrain *CTG_TrainScheduleBase::GetPrevTrainByShiftIndex(TRAIN_INDEX train_index,STATION_NO station)
{
	if(train_index==0)
		return NULL;

	IterMapTrain iter=FirstTrain();
	TRAIN_INDEX this_shift_index=0;
	while(iter!=LastTrain())
	{
		this_shift_index=iter->second.GetShiftTrainIndex();
		if(!iter->second.IsDeletedObject()  && (this_shift_index!=0)
			&&  (this_shift_index & 0x3FFFFFFF)==(train_index&0x3FFFFFFF) // 20141204 车次分割 30bit为1,解决南宁全局ID分割重复
			&& iter->second.GetTrainRecordCount())
		{
			CTG_SingleTrain *ptrain=&iter->second;
			CTG_TrainRecord *prec=ptrain->GetTrainLastRecord();
			if(prec && prec->GetStationNo()!=station)
			{
				iter++;
				continue;
			}
			return ptrain;
		}
		iter++;
	}	
	
	return NULL;
}

// 勾连车组关系使用
CTG_SingleTrain *CTG_TrainScheduleBase::GetPrevTrainByBasicIndex(TRAIN_INDEX train_index,STATION_NO station, TIME tm, CString trainid)
{
	if(train_index==0)
		return NULL;

	CTG_SingleTrain *pfindtrain=NULL;
	int max_dlt=GetTGFunctionConfig()->GetFindBasicTrainBeforeTime();
	IterMapTrain iter=FirstTrain();
	TRAIN_INDEX this_basic_index=0;
	while(iter!=LastTrain())
	{
		this_basic_index=iter->second.GetBasicTrainIndex();
		if(!iter->second.IsDeletedObject()  && (this_basic_index!=0)
			&&  (this_basic_index & 0x3FFFFFFF)==(train_index&0x3FFFFFFF) // 20141204 车次分割 30bit为1,解决南宁全局ID分割重复
			&& iter->second.GetTrainRecordCount())
		{
			CTG_SingleTrain *ptrain=&iter->second;
			CTG_TrainRecord *prec=ptrain->GetTrainLastRecord();
			if(prec && prec->GetStationNo()!=station)
			{
				iter++;
				continue;
			}
			if(prec)
			{
				TIME arrivetm=prec->GetArriveTime();
				if(arrivetm>=tm)
				{
					iter++;
					continue;
				}
				
				CTG_SingleTrain *pnexttrain=GetNextTrainByPrevTrainIndex(ptrain->GetTrainIndex());
				if(pnexttrain==NULL)
				{
					int dlt=tm - arrivetm;
					if(dlt<max_dlt)
					{
						max_dlt=dlt;
						pfindtrain=ptrain;
					}
				}
				else
				{
					GpcLogTrainMessage(0,0,"列车 %s(%u)已经是列车 %s(%u)的后序列车,因此列车 %s(%u)不能再作为列车 %s(%u)的后序列车", 
						pnexttrain->GetTrainID(), pnexttrain->GetTrainIndex(), 
						ptrain->GetTrainID(), ptrain->GetTrainIndex(), 
						trainid, train_index, ptrain->GetTrainID(), ptrain->GetTrainIndex());
				}
			}
		}
		iter++;
	}	
	
	return pfindtrain;
}

// 勾连车组关系使用
CTG_SingleTrain *CTG_TrainScheduleBase::GetNextTrainByBasicIndex(TRAIN_INDEX train_index,STATION_NO station,TIME tm, CString trainid)
{
	if(train_index==0)
		return NULL;

	CTG_SingleTrain *pfindtrain=NULL;
	int max_dlt=GetTGFunctionConfig()->GetFindBasicTrainAfterTime();
	IterMapTrain iter=FirstTrain();
	TRAIN_INDEX prev_basic_index=0;
	while(iter!=LastTrain())
	{
		prev_basic_index=iter->second.GetPrevTrainBasicTrainIndex();
		if(!iter->second.IsDeletedObject() && (prev_basic_index!=0) 
			&&  (prev_basic_index & 0x3FFFFFFF)==(train_index&0x3FFFFFFF) // 20141204 车次分割 30bit为1,解决南宁全局ID分割重复
			&& iter->second.GetTrainRecordCount())
		{
			CTG_SingleTrain *ptrain=&iter->second;
			CTG_TrainRecord *prec=ptrain->GetTrainRecord(0);
			if(prec && prec->GetStationNo()!=station)
			{
				iter++;
				continue;
			}
			
			if(prec)
			{
				TIME departtm=prec->GetDepartTime();
				if(departtm<=tm)
				{
					iter++;
					continue;
				}

				CTG_SingleTrain *pprevtrain=GetSingleTrain(ptrain->GetPrevTrainIndex());
				if(pprevtrain==NULL)
				{
					int dlt=departtm - tm;
					if(dlt<max_dlt)
					{
						max_dlt=dlt;
						pfindtrain=ptrain;
					}
				}
				else
				{
					GpcLogTrainMessage(0,0,"列车 %s(%u)已经是列车 %s(%u)的前序列车,因此列车 %s(%u)不能再作为列车 %s(%u)的前序列车", 
						pprevtrain->GetTrainID(), pprevtrain->GetTrainIndex(), 
						ptrain->GetTrainID(), ptrain->GetTrainIndex(), 
						trainid, train_index, ptrain->GetTrainID(), ptrain->GetTrainIndex());
				}
			}
		}
		iter++;
	}	
	
	return pfindtrain;
}

// 解除勾连关系使用,由前车索引找后车
CTG_SingleTrain *CTG_TrainScheduleBase::GetNextTrainByPrevTrainIndex(TRAIN_INDEX train_index)
{
	if(train_index==0)
		return NULL;

	IterMapTrain iter=FirstTrain();
	TRAIN_INDEX prev_train_index=0;
	while(iter!=LastTrain())
	{
		prev_train_index=iter->second.GetPrevTrainIndex();
		if(!iter->second.IsDeletedObject() && (prev_train_index!=0) 
			&&  (prev_train_index & 0x3FFFFFFF)==(train_index&0x3FFFFFFF) // 20141204 车次分割 30bit为1,解决南宁全局ID分割重复
			&& iter->second.GetTrainRecordCount())
		{
			CTG_SingleTrain *ptrain=&iter->second;			
			return ptrain;
		}
		iter++;
	}	
	
	return NULL;
}

CTG_SingleTrain *CTG_TrainScheduleBase::GetSingleTrainByShiftIndex(TRAIN_INDEX train_index,STATION_NO station)
{
	// 增加对于train_index为0判断201709026
	if(train_index==NULL_TRAIN_INDEX)
		return NULL;

	IterMapTrain iter=FirstTrain();
	while(iter!=LastTrain())
	{
		if(!iter->second.IsDeletedObject() 
			&&  (iter->second.GetShiftTrainIndex() & 0x3FFFFFFF)==(train_index&0x3FFFFFFF) // 20141204 车次分割 30bit为1,解决南宁全局ID分割重复
			&& iter->second.GetTrainRecordCount())
		{
			CTG_SingleTrain *ptrain=&iter->second;
			if(station!=NULL_STATION)
			{
				CTG_TrainRecord *prec=ptrain->GetTrainRecord(0);
				if(prec && prec->GetStationNo()!=station)
				{
					iter++;
					continue;
				}
			}
			else
			{
				if(iter->second.GetShiftTrainIndex()!=train_index)
				{
					iter++;
					continue;
				}
			}
			return ptrain;
		}
		iter++;
	}	
	
	return NULL;
}

CTG_SingleMark  *CTG_TrainScheduleBase::GetSingleMarkByShiftString(const char *planid)
{
	if(!planid)
		return FALSE;
	if(strlen(planid)==0)
		return FALSE;
	IterMapMark iter=FirstMark();
	while(iter!=LastMark())
	{
		if(!iter->second.IsDeletedObject() && !stricmp(iter->second.GetShiftIndexString(),planid))
			 return	&iter->second;
		iter++;
	}
	return NULL;
}

BOOL   CTG_TrainScheduleBase::FindTrainsByShiftIndex(CTG_TrainPtrGroup &result,TRAIN_INDEX shiftindex)
{
	// 增加对于shiftindex为0判断20171016
	if(shiftindex==NULL_TRAIN_INDEX)
		return NULL;

	IterMapTrain iter=FirstTrain();
	while(iter!=LastTrain())
	{
		if(!iter->second.IsDeletedObject() && iter->second.GetShiftTrainIndex()==shiftindex)
			result.AddTrainPtr(&iter->second);
		iter++;
	}
	result.SetTrainsPtr();
	if(result.GetTrainCount()>0)
		return TRUE;
	else
		return FALSE;
}

void   CTG_TrainScheduleBase::GetHistoryData(TIME nViewBeginTm, std::vector<TRAIN_INDEX>& train_vec, std::vector<TRAIN_INDEX>& block_vec)
{
	train_vec.clear();
	block_vec.clear();
	
	if(0==m_nPollingBeginDelTime) // 初始值为0
		m_nPollingBeginDelTime=nViewBeginTm-120;
	
	if(m_nPollingBeginDelTime>=nViewBeginTm)
	{
		m_nPollingBeginDelTime=nViewBeginTm-120;
	}

	CTime tmdel(m_nPollingBeginDelTime), tmbegin(nViewBeginTm);
	GpcLogTrainMessage(0,0,"整分移动删除数据时间范围:%s -- %s", tmdel.Format("%Y-%m-%d %H:%M:%S"), tmbegin.Format("%Y-%m-%d %H:%M:%S"));
	IterMapTrain iter=FirstTrain();
	while(iter!=LastTrain())
	{
		if(iter->second.GetTrainRecordCount()>0)  // 仅对于未删除列车 重要!!!
		{
			TIME et=iter->second.GetEndTime();
			if(m_nPollingBeginDelTime<et && et<nViewBeginTm)
			{
				train_vec.push_back(iter->first);
			}
		}
		iter++;
	}

	IterMapMark iter1=FirstMark();
	while(iter1!=LastMark())
	{
		TIME et=iter1->second.GetEndTime();
		if(m_nPollingBeginDelTime<et && et<nViewBeginTm)
		{
			block_vec.push_back(iter1->first);
		}
		iter1++;
	}
	m_nPollingBeginDelTime=nViewBeginTm-60;
}

void   CTG_TrainScheduleBase::GetNewlyData(TIME nViewEndTm, std::vector<TRAIN_INDEX>& train_vec, std::vector<TRAIN_INDEX>& block_vec)
{
	train_vec.clear();
	block_vec.clear();

	if(0==m_nPollingBeginAddTime) // 初始值为0
		m_nPollingBeginAddTime=nViewEndTm-120;
	
	if(m_nPollingBeginAddTime>=nViewEndTm)
	{
		m_nPollingBeginAddTime=nViewEndTm-120;
	}

	CTime tmAdd(m_nPollingBeginAddTime), tmEnd(nViewEndTm);
	GpcLogTrainMessage(0,0,"整分移动新增数据时间范围:%s -- %s", tmAdd.Format("%Y-%m-%d %H:%M:%S"), tmEnd.Format("%Y-%m-%d %H:%M:%S"));

	IterMapTrain iter=FirstTrain();
	while(iter!=LastTrain())
	{
		if(iter->second.GetTrainRecordCount()>0) // 仅对于未删除列车 重要!!!
		{
			TIME et=iter->second.GetStartTime();
			if(m_nPollingBeginAddTime<=et && et<=nViewEndTm)
			{
				train_vec.push_back(iter->first);
			}
		}
		
		iter++;
	}

	IterMapMark iter1=FirstMark();
	while(iter1!=LastMark())
	{
		TIME et=iter1->second.GetStartTime();
		if(m_nPollingBeginAddTime<=et && et<=nViewEndTm)
		{
			block_vec.push_back(iter1->first);
		}
		iter1++;
	}
	m_nPollingBeginAddTime=nViewEndTm-60;
	GpcLogTrainMessage(0,0, "WintgTerm中列车个数=%d 施工个数=%d", m_mapTrains.size(), m_mapMarks.size());
}

void   CTG_TrainScheduleBase::DirectDeleteBlock(TRAIN_INDEX train_index)
{
	IterMapMark iter = m_mapMarks.find(train_index);
	if (iter != m_mapMarks.end())
	{
		m_mapMarks.erase(iter);
		GpcLogTrainMessage(0, 0, "将施工符号 %u 从WintgTerm内存中移除 section=%d schdtype=%d", 
					train_index, m_cScheduleInfo.m_nSectionID, m_cScheduleInfo.m_nType);
	}
}

void   CTG_TrainScheduleBase::DirectDeleteTrain(TRAIN_INDEX train_index)
{
	IterMapTrain iter = m_mapTrains.find(train_index);
	if (iter != m_mapTrains.end())
	{
		GpcLogTrainMessage(0, 0, "将列车 %s(%u)从WintgTerm内存中移除 section=%d schdtype=%d", 
		   iter->second.GetTrainID(), train_index, m_cScheduleInfo.m_nSectionID, m_cScheduleInfo.m_nType);

		m_mapTrains.erase(iter);
	}
}

// 目前用于接收日班或者基本图计划时删除原有记录，当时间参数全为0，则删除所有 增加type选项 type:0 删除所有 1删除客车 2删除货车
BOOL   CTG_TrainScheduleBase::DirectDeleteTrainsInRange(TIME starttime,TIME endtime,BOOL onlystarttime,BOOL onlyendtime,int type) 
{
	IterMapTrain iter=FirstTrain();
	for(;iter!=LastTrain();iter++)
	{
		TIME st=iter->second.GetStartTime();
		TIME et=iter->second.GetEndTime();
		if(onlystarttime)
		{
			if((st>=starttime || starttime==0)  && (st<=endtime || endtime==0))
			{
				if (type==2 && iter->second.IsPassengerTrain())
					continue;
				
				if (type==1 && !iter->second.IsPassengerTrain())
					continue;
				
				iter->second.SetDeletedFlag(TRUE);  
				iter->second.SetSyncStatusX(SYCFLAG_NULL,TRUE);
			}
		}
		if(onlyendtime)
		{
			if((et>=starttime || starttime==0) && (et<=endtime ||endtime==0) )
			{
				if (type==2 && iter->second.IsPassengerTrain())
					continue;
				
				if (type==1 && !iter->second.IsPassengerTrain())
					continue;
				
				iter->second.SetDeletedFlag(TRUE);
				iter->second.SetSyncStatusX(SYCFLAG_NULL,TRUE);
			}
		}
	}
	return TRUE;
}

BOOL   CTG_TrainScheduleBase::DirectDeleteMarksInRange(TIME starttime,TIME endtime,BOOL onlystarttime,BOOL onlyendtime) // 目前用于接收日班计划时删除原有记录
{
	IterMapMark iter=FirstMark();
	while(iter!=LastMark())
	{
		TIME st=iter->second.GetStartTime();
		TIME et=iter->second.GetEndTime();
		if(onlystarttime)
		{
			if((st>starttime || starttime==0) && (st<endtime || endtime==0))
			{
				iter->second.SetDeletedFlag(TRUE);
				iter->second.SetSyncStatusX(SYCFLAG_NULL,TRUE);
			}
		}
		if(onlyendtime)
		{
			if((et>starttime || starttime==0) && (et<endtime || endtime==0))
			{
				iter->second.SetDeletedFlag(TRUE);
				iter->second.SetSyncStatusX(SYCFLAG_NULL,TRUE);
			}
		}
		iter++;
	}
	return TRUE;
}

BOOL   CTG_TrainScheduleBase::DeleteDataInMemory(TIME starttime,TIME endtime,BOOL onlystarttime,BOOL onlyendtime)
{
	IterMapTrain iter=FirstTrain();
	while(iter!=LastTrain())
	{
		TIME st=iter->second.GetStartTime();
		TIME et=iter->second.GetEndTime();
		if(onlystarttime)
		{
			if(st>starttime && st<endtime)
			{
				iter=m_mapTrains.erase(iter);
				continue;
			}
		}
		if(onlyendtime)
		{
			if(et>starttime && et<endtime)
			{
				iter=m_mapTrains.erase(iter);
				continue;
			}
		}
		iter++;
	}

	IterMapMark markiter=FirstMark();
	while(markiter!=LastMark())
	{
		TIME st=markiter->second.GetStartTime();
		TIME et=markiter->second.GetEndTime();
		if(onlystarttime)
		{
			if(st>starttime && st<endtime)
			{
				markiter=m_mapMarks.erase(markiter);
				continue;
			}
		}
		if(onlyendtime)
		{
			if(et>starttime && et<endtime)
			{
				markiter=m_mapMarks.erase(markiter);
				continue;
			}
		}
		markiter++;
	}

	return TRUE;
}

CTG_SingleTrain *CTG_TrainScheduleBase::GetSingleTrain(const TRAIN_NO trainno,TIME sttm,TIME endtm,BOOL null_shiftindex,BOOL IsStart)
{
	CTG_TrainPtrGroup prev_group;
	FindTrains(prev_group,NULL_STATION,sttm, endtm,FALSE,IsStart);
	for(int idx=0;idx<prev_group.GetTrainCount();idx++)
	   {
		   CTG_SingleTrain *prev_tmp=prev_group.GetTrainPtr(idx);
		   if(null_shiftindex && prev_tmp->GetShiftTrainIndex()!=NULL_TRAININDEX)
			   continue;
		   if(prev_tmp->MatchTrainID(trainno)) // 原来方式添加的车次
			   {
				   return prev_tmp;
			   }
	   }
   return NULL;
}

BOOL   CTG_TrainScheduleBase::FindTrainsByStation(CTG_ObjectPtrGroup &result,STATION_NO station,TIME begin_time,TIME end_time,BOOL arrive,BOOL depart)
{
	return m_staList.FindTrains(result,station,begin_time,end_time,arrive,depart);
}

BOOL   CTG_TrainScheduleBase::IsTrainInSendPlanList(CTG_SingleTrain *pTrain)
{
	std::map<TRAIN_INDEX,ULONG>::iterator it = CHasStPlanTrainList.find(pTrain->GetTrainIndex()); 
	if (it != CHasStPlanTrainList.end())
	{
		if (it->second > 0)
			return TRUE;		
	}
	return FALSE;
}

void   CTG_TrainScheduleBase::AddTrainInSendPlanList(TRAIN_INDEX trainindex)
{
	std::map<TRAIN_INDEX,ULONG>::iterator it = CHasStPlanTrainList.find(trainindex);
	if (it != CHasStPlanTrainList.end())
	{
		if (it->second < time(NULL))
			it->second = time(NULL);		
	}
	else
		CHasStPlanTrainList[trainindex]=time(NULL);
}

void   CTG_TrainScheduleBase::RemoveTrainInSendPlanList()
{
	std::map<TRAIN_INDEX,ULONG>::iterator it = CHasStPlanTrainList.begin(),tmpter;
	while(it != CHasStPlanTrainList.end())
	{
		tmpter=it;
		it++;
		if (tmpter->second + 12*3600 < time(NULL))
			CHasStPlanTrainList.erase(tmpter);
	}
}

void  CTG_TrainScheduleBase::SaveTrainExtent(const CTG_TrainExtent& trainExtent)
{
	mTrainExtent=trainExtent;
	GpcLogTrainMessage(0, 0, "接收到TrainExpand: train_index %u prev_train_index %u, prev_shift_index %u, prev_basic_index %u", 
			  mTrainExtent.train_index, mTrainExtent.prev_train_index, mTrainExtent.prev_shift_index, mTrainExtent.prev_basic_index);
}

void  CTG_TrainScheduleBase::FinishSetTrainExtent()
{
	mTrainExtent.train_index=0;
}

bool  CTG_TrainScheduleBase::SetTrainExtent(CTG_SingleTrain  &train)  const
{
	bool bResult=false;
	if(mTrainExtent.train_index==train.GetTrainIndex())
	{
		TG_TrainInfoExpand* pTrainExpand=train.GetTrainInfoExpand();
		CTG_TrainRelation relation;
		relation.prev_train_index=mTrainExtent.prev_train_index;
		relation.prev_basic_index=mTrainExtent.prev_basic_index;
		relation.prev_shift_index=mTrainExtent.prev_shift_index;
		relation.next_shift_index=mTrainExtent.next_shift_index;
		relation.prev_dest_sta=mTrainExtent.prev_dest_sta; 
		relation.next_dest_sta=mTrainExtent.next_dest_sta;
		relation.czh_no_str=mTrainExtent.chezu_no;
		pTrainExpand->UpdateTrainRelation(relation);

		CTG_TrainKediaoInfo kd;
		kd.type=mTrainExtent.kd_type;
		kd.content=mTrainExtent.kd_content;

		pTrainExpand->UpdateTrainKediao(kd);
			
		CTG_TrainYingyeList yy;
		for(int i=0; i<mTrainExtent.yy_list.size(); i++)
		{
			yy.AddRecord(mTrainExtent.yy_list[i]);
		}
		pTrainExpand->UpdateTrainYingye(yy);

		CTG_TrainCrewList crew;
		for(int i=0; i<mTrainExtent.crew_list.size(); i++)
		{
			crew.AddRecord(mTrainExtent.crew_list[i]);
		}
		pTrainExpand->UpdateTrainCrew(crew);

		CTG_TrainTicketList ticket;
		for(int i=0; i<mTrainExtent.ticket_list.size(); i++)
		{
			ticket.AddRecord(mTrainExtent.ticket_list[i]);
		}
		pTrainExpand->UpdateTrainTicket(ticket);

		CTG_EngineCrewList engine_crew;
		for(int i=0; i<mTrainExtent.engine_list.size(); i++)
		{
			engine_crew.AddRecord(mTrainExtent.engine_list[i]);
		}
		pTrainExpand->UpdateTrainEngineCrew(engine_crew);

		for(int i=0; i<mTrainExtent.opms_rd_list.size(); i++)
		{
			CTG_TrainRecord *prec=train.GetTrainRecordByRecIndex(i);
			if(prec!=NULL)
			{
				if(prec->m_data.station==mTrainExtent.opms_rd_list[i].station_no)
				{
					prec->SetRecordKK(mTrainExtent.opms_rd_list[i]);
				}
			}
		}

		bResult=true;
	}
	else
	{
		bResult=false;
	}
	return bResult;
}

///////////  CTG_TrainPtrGroup  ///

void CTG_TrainPtrGroup::AddTrainPtr(CTG_SingleTrain *p)
{
	//vector<CTG_SingleTrain *>::iterator iter=m_trainptrs.begin();	
	//while(iter!=m_trainptrs.end())
	//{
	//	if((*iter)->GetStartTime()>p->GetStartTime())
	//		break;
	//	iter++;
	//}
	//if(iter!=m_trainptrs.end())
	//	m_trainptrs.insert(iter,p);
	//else
	//	m_trainptrs.push_back(p);

	TrainResultTmp s;
	s.tm=p->GetStartTime();
	s.p=p;

	m_trainpter_tmp_list.push_back(s);
}

void CTG_TrainPtrGroup::AddTrainPtrByEndTmOrder(CTG_SingleTrain *p)
{
	if(p==NULL)
		return;
	if(p->GetTrainRecordCount()<=0)  // 防止车为删除车 GetEndTime 返回0
		return;
	/*vector<CTG_SingleTrain *>::iterator iter=m_trainptrs.begin();
	while(iter!=m_trainptrs.end())
	{
		if((*iter)->GetEndTime()>p->GetEndTime())
			break;
		iter++;
	}
	if(iter!=m_trainptrs.end())
		m_trainptrs.insert(iter,p);
	else
		m_trainptrs.push_back(p);*/

	TrainResultTmp s;
	s.tm=p->GetEndTime();
	s.p=p;

	m_trainpter_tmp_list.push_back(s);
}

void CTG_TrainPtrGroup::AddTrainPtrNotOrder(CTG_SingleTrain *p)
{
	for(int i=0; i<m_trainptrs_cnt; i++)
	{
		if(m_trainptrs[i] == p)
			return;
	}
	
	m_trainptrs[m_trainptrs_cnt]=p;
	m_trainptrs_cnt++;
}

bool sortbytm(const TrainResultTmp& n1, const TrainResultTmp& n2)
{
	return (n1.tm<n2.tm)?true:false;
}

void CTG_TrainPtrGroup::SetTrainsPtr()
{
	m_trainptrs_cnt=0;
	memset(m_trainptrs, 0, sizeof(m_trainptrs));
	std::sort(m_trainpter_tmp_list.begin(), m_trainpter_tmp_list.end(), sortbytm);
	for(int i=0; i<m_trainpter_tmp_list.size(); i++)
	{
		m_trainptrs[m_trainptrs_cnt]=m_trainpter_tmp_list[i].p;
		m_trainptrs_cnt++;
	}
}

/////////   CTG_StationTrainList ///////////
CTG_StationTrainList::CListStationTrain *CTG_StationTrainList::GetStationList(STATION_NO nStation)
{
	std::map<STATION_NO,CListStationTrain>::iterator  iter;
	iter=m_listStationTrains.find(nStation);
	if(iter!=m_listStationTrains.end())
		 return &iter->second;
	return NULL;
}

CTG_StationTrainList::CListStationTrain *CTG_StationTrainList::AddStationList(STATION_NO nStation)
{
	CListStationTrain *pList=GetStationList(nStation);
	if(pList)
		return pList;

	CListStationTrain list;
	m_listStationTrains[nStation]=list;
	return &m_listStationTrains[nStation];
}
BOOL CTG_StationTrainList::DeleteTrain(TRAIN_INDEX nTrainIndex)
{
	BOOL rc=FALSE;
	std::map<STATION_NO,CListStationTrain>::iterator iter;
	for(iter=m_listStationTrains.begin();iter!=m_listStationTrains.end();iter++)
	{
		CListStationTrain &sta_list=iter->second;
		std::list<CTG_StationTrainStruct>::iterator sta_iter=sta_list.begin();
		while(sta_iter!=sta_list.end())
		{
			if(sta_iter->nTrainIndex==nTrainIndex)
			{
				sta_iter=sta_list.erase(sta_iter);
				rc=TRUE;
			}
			else
				sta_iter++;
		}
	}
	return rc;
}

BOOL CTG_StationTrainList::DeleteObsoleteTrains(TIME before_time)
{
	BOOL rc=FALSE;
	std::map<STATION_NO,CListStationTrain>::iterator iter;
	for(iter=m_listStationTrains.begin();iter!=m_listStationTrains.end();iter++)
	{
		CListStationTrain &sta_list=iter->second;
		std::list<CTG_StationTrainStruct>::iterator sta_iter=sta_list.begin();
		while(sta_iter!=sta_list.end())
		{
			if(sta_iter->tDepartTime<before_time)
			{
				sta_iter=sta_list.erase(sta_iter);
				rc=TRUE;
			}
			else
				sta_iter++;
		}
	}
	return rc;
}

BOOL CTG_StationTrainList::DeleteAllTrains()
{
	m_listStationTrains.clear();
	return TRUE;
}


BOOL CTG_StationTrainList::UpdateTrain(CTG_SingleTrain *pTrain)
{
	CTG_StationTrainStruct node;
	node.nTrainIndex=pTrain->GetTrainIndex();
	node.pTrain=pTrain;
	for(int i=0;i<pTrain->GetTrainRecordCount();i++)
	{
		CTG_TrainRecord  *prec=pTrain->GetTrainRecord(i);
		if(!prec) continue; // impossible!
		int nStation=prec->GetStationNo();
		CListStationTrain *pList=AddStationList(nStation);
		std::list<CTG_StationTrainStruct>::iterator iter;
		iter=pList->begin();
		node.tArriveTime=prec->GetArriveTime();
		node.tDepartTime=prec->GetDepartTime();
		if(node.tArriveTime==NULL_TIME) node.tArriveTime=node.tDepartTime;
		if(node.tDepartTime==NULL_TIME) node.tDepartTime=node.tArriveTime;
		node.nEntry=prec->GetEntry();
		node.nExit=prec->GetExit();
		node.nRecIndex=i;
        BOOL no_insert=TRUE;
		while(iter!=pList->end())
		{
			if(iter->nTrainIndex==pTrain->GetTrainIndex())  // 删除原节点
			{
				iter=pList->erase(iter);
				continue;
			}
            if(no_insert && (iter->tDepartTime>node.tDepartTime))
			{
				pList->insert(iter,node);
				no_insert=FALSE;
			}
			iter++;
		}
		if(no_insert)
			pList->push_back(node);	
	}
	return TRUE;
}

BOOL CTG_StationTrainList::FindTrains(CTG_ObjectPtrGroup &result,STATION_NO station,TIME begin_time,TIME end_time,BOOL arrive,BOOL depart)
{
	CListStationTrain *pList=GetStationList(station);
	if(!pList)
		return FALSE;
	std::list<CTG_StationTrainStruct>::iterator iter;
	iter=pList->begin();
	while(iter!=pList->end())
		{
			// 删除失效的记录
			if(!iter->pTrain)
			{
				iter=pList->erase(iter);
				continue;
			}
			CTG_TrainRecord *prec=iter->pTrain->GetTrainRecord(iter->nRecIndex);
			if(!prec || prec->GetStationNo()!=station)
			{
				iter=pList->erase(iter);
				continue;
			}
				
			if(depart && 
				(iter->tDepartTime>=begin_time || begin_time==NULL_TIME) && 
				(iter->tDepartTime<=end_time || end_time==NULL_TIME))
				result.AddPtr(&*iter);
			else
			if(arrive && 
				(iter->tArriveTime>=begin_time || begin_time==NULL_TIME) && 
				(iter->tArriveTime<=end_time || end_time==NULL_TIME))
				result.AddPtr(&*iter);
			iter++;
		}
	if(result.GetCount()>0)
		return TRUE;
	else
		return FALSE;
}


BOOL CTG_StationTrainList::FindMeetTrains(CTG_ObjectPtrGroup &result_2,int recIndex,const CTG_SingleTrain *pTrain)
{

	BOOL rc;
	const CTG_TrainRecord *prec=pTrain->GetTrainRecordNoDelete_Const(recIndex);
	if(!prec)
		return FALSE;
	if(prec->IsPassRec() || prec->IsStartRec() || prec->IsTerminalRec())
		return FALSE;
	CTG_ObjectPtrGroup result;
	rc=FindTrains(result,prec->GetStationNo(),prec->GetArriveTime(),prec->GetDepartTime(),TRUE,TRUE);

	for(int i=0;i<result.GetCount();i++)
	{
		CTG_StationTrainStruct *pRelaTrain= (CTG_StationTrainStruct *)result.GetPtr(i);
		if(pRelaTrain->nEntry==prec->GetExit()
			|| pRelaTrain->nExit==prec->GetExit())
			result_2.AddPtr(pRelaTrain);
	}

	if(result_2.GetCount()>0)
		return TRUE;
	else
		return FALSE;
}

bool SetTrainExitEntry(CTG_SingleTrain& objtrain, const TrainForExitEntry& train)
{
	if(objtrain.GetTrainRecordCount()!=train.GetTrainRecordCount())
	{
		GpcLogTrainMessage(0, 0, "车站个数不同");
		return false;
	}
	for(int i=0; i<objtrain.GetTrainRecordCount(); i++)
	{
		CTG_TrainRecord* prec=objtrain.GetTrainRecordByRecIndex(i);
		const TmpRecord* pold=train.GetTrainRecord(i);
		if(prec!=NULL && pold!=NULL)
		{
			if(prec->m_data.station == pold->station)
			{
				prec->m_data.exit = pold->exit;
				prec->m_data.entry = pold->entry;
			}
			else
			{
				GpcLogTrainMessage(0, 0, "车站不对应");
				return false;
			}
		}
		else
		{
			GpcLogTrainMessage(0, 0, "列车长度不同");
			return false;
		}
	}
	return true;
}

CTG_SingleTrain *CTG_TrainScheduleBase::FindTrainsByStationAndTrainNo(STATION_NO station, const char *train_no)
{
	IterMapTrain iter=FirstTrain();
	while(iter!=LastTrain())
	{
		if(iter->second.IsDeletedObject())
		{
			iter++;
			continue;
		}
		if(iter->second.GetTrainRecordCount()<=0)
		{
			iter++;
			continue;
		}

		if(!iter->second.MatchTrainID(train_no))
		{
			iter++;
			continue;
		}

		CTG_TrainRecord *prec=NULL;
		int cnt=iter->second.GetTrainRecordCount();
		for(int i=0;i< cnt;i++)
		{
			prec=iter->second.GetTrainRecord(i);
			if(prec && prec->GetStationNo()== station)
			{
				return &iter->second;
			}
		}
		iter++;
	}

	return NULL;
}

CTG_TrainExtent::CTG_TrainExtent()
{
	train_index=0;
	memset(chezu_no, 0, sizeof(chezu_no));
	
	prev_shift_index=0;	//前趟车全局ID	 
	next_shift_index=0;	//直通ID	 
	prev_train_index=0;	//前趟车
	prev_basic_index=0;	//前趟车基本图ID
	prev_dest_sta=0;		//前一个方向站 
	next_dest_sta=0;		//后一个方向站 
	halt_state=0;         //停运标志

	kd_type=0;   //类型 1:客调 2：特调
	kd_content=""; //客调内容
}

CTG_TrainExtent::CTG_TrainExtent(const CTG_TrainExtent& rhs)
{
	train_index=rhs.train_index;
	STR_CPY(chezu_no, rhs.chezu_no);
	
	prev_shift_index=rhs.prev_shift_index;	//前趟车全局ID	 
	next_shift_index=rhs.next_shift_index;	//直通ID	 

	prev_train_index=rhs.prev_train_index;	
	prev_basic_index=rhs.prev_basic_index;	
	prev_dest_sta=rhs.prev_dest_sta;	
	next_dest_sta=rhs.next_dest_sta;	
	halt_state=rhs.halt_state;	

	kd_type=rhs.kd_type;
	kd_content=rhs.kd_content;

	yy_list.assign(rhs.yy_list.begin(), rhs.yy_list.end());
	crew_list.assign(rhs.crew_list.begin(), rhs.crew_list.end());
	ticket_list.assign(rhs.ticket_list.begin(), rhs.ticket_list.end());
	engine_list.assign(rhs.engine_list.begin(), rhs.engine_list.end());
	opms_rd_list.assign(rhs.opms_rd_list.begin(), rhs.opms_rd_list.end());
}

CTG_TrainExtent& CTG_TrainExtent::operator=(const CTG_TrainExtent& rhs)
{
	if(this==&rhs)
		return *this;

	train_index=rhs.train_index;
	STR_CPY(chezu_no, rhs.chezu_no);
	
	prev_shift_index=rhs.prev_shift_index;	//前趟车全局ID	 
	next_shift_index=rhs.next_shift_index;	//直通ID	 

	prev_train_index=rhs.prev_train_index;	
	prev_basic_index=rhs.prev_basic_index;	
	prev_dest_sta=rhs.prev_dest_sta;	
	next_dest_sta=rhs.next_dest_sta;	
	halt_state=rhs.halt_state;	

	kd_type=rhs.kd_type;
	kd_content=rhs.kd_content;

	yy_list.assign(rhs.yy_list.begin(), rhs.yy_list.end());
	crew_list.assign(rhs.crew_list.begin(), rhs.crew_list.end());
	ticket_list.assign(rhs.ticket_list.begin(), rhs.ticket_list.end());
	engine_list.assign(rhs.engine_list.begin(), rhs.engine_list.end());
	opms_rd_list.assign(rhs.opms_rd_list.begin(), rhs.opms_rd_list.end());
	return *this;
}


/////////////////////////////////////////////////////////////////////////////////////////
TG_TrainRecordExpand::TG_TrainRecordExpand()
{
	Init();
}

void TG_TrainRecordExpand::Init()
{
	work_flag=0;
	oper_flag=0;
	important_flag=0;

	oper_info=note_text="";
}	

TG_TrainRecordExpand::TG_TrainRecordExpand(const TG_TrainRecordExpand& rhs)
{
	work_flag=rhs.work_flag;
	oper_flag=rhs.oper_flag;
	important_flag=rhs.important_flag;

	oper_info=rhs.oper_info;
	note_text=rhs.note_text;
}

TG_TrainRecordExpand& TG_TrainRecordExpand::operator=(const TG_TrainRecordExpand& rhs)
{
	if(this==&rhs)
		return *this;

	work_flag=rhs.work_flag;
	oper_flag=rhs.oper_flag;
	important_flag=rhs.important_flag;
	
	oper_info=rhs.oper_info;
	note_text=rhs.note_text;

	return *this;
}

bool TG_TrainRecordExpand::IsSame(const TG_TrainRecordExpand& rhs) const
{
	if(GetTGFunctionConfig()->IsNeedCompareTrainExpand())
	{
		if(work_flag!=rhs.work_flag || oper_flag!=rhs.oper_flag || important_flag!=rhs.important_flag 
			|| oper_info!=rhs.oper_info || note_text!=rhs.note_text)
		{
			return false;
		}
	}
	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////
TG_TrainInfoExpand::TG_TrainInfoExpand()
{
	Init();
}

void TG_TrainInfoExpand::Init()
{
	m_cRelation.Init();
	m_cKediao.Init();
	m_cYingyeList.Init();
	m_cCrewList.Init();
	m_cTicketList.Init();
	m_cEngineCrewList.Init();
}

TG_TrainInfoExpand::TG_TrainInfoExpand(const TG_TrainInfoExpand &src)
{
	m_cRelation=src.m_cRelation;
	m_cKediao=src.m_cKediao;
	m_cYingyeList=src.m_cYingyeList;
	m_cCrewList=src.m_cCrewList;
	m_cTicketList=src.m_cTicketList;
	m_cEngineCrewList=src.m_cEngineCrewList;
}

TG_TrainInfoExpand& TG_TrainInfoExpand::operator= (const TG_TrainInfoExpand &src)
{
	if(this == &src)
		return *this;
	
	m_cRelation=src.m_cRelation;
	m_cKediao=src.m_cKediao;
	m_cYingyeList=src.m_cYingyeList;
	m_cCrewList=src.m_cCrewList;
	m_cTicketList=src.m_cTicketList;
	m_cEngineCrewList=src.m_cEngineCrewList;

	return *this;
}

bool TG_TrainInfoExpand::IsSame(const TG_TrainInfoExpand &src) const
{
	if(GetTGFunctionConfig()->IsNeedCompareTrainExpand())
	{
		if(!m_cRelation.IsCoreSame(src.m_cRelation))
			return false;
		if(!m_cKediao.IsCoreSame(src.m_cKediao))
			return false;
		if(!m_cYingyeList.IsCoreSame(src.m_cYingyeList))
			return false;
		if(!m_cCrewList.IsCoreSame(src.m_cCrewList))
			return false;
		if(!m_cTicketList.IsCoreSame(src.m_cTicketList))
			return false;
		if(!m_cEngineCrewList.IsCoreSame(src.m_cEngineCrewList))
			return false;
	}
	return true;
}

bool TG_TrainInfoExpand::IsTrainKediaoSame(const CTG_TrainKediaoInfo& kd) const
{
	return m_cKediao.IsCoreSame(kd);
}

bool TG_TrainInfoExpand::IsTrainYingyeSame(const CTG_TrainYingyeList& yingye) const
{
	return m_cYingyeList.IsCoreSame(yingye);
}

bool TG_TrainInfoExpand::IsTrainCrewSame(const CTG_TrainCrewList& crew) const
{
	return m_cCrewList.IsCoreSame(crew);
}

bool TG_TrainInfoExpand::IsTrainTicketSame(const CTG_TrainTicketList& ticket) const
{
	return m_cTicketList.IsCoreSame(ticket);
}

bool TG_TrainInfoExpand::IsTrainEngineCrewSame(const CTG_EngineCrewList& engine) const
{
	return m_cEngineCrewList.IsCoreSame(engine);
}

void TG_TrainInfoExpand::UpdateTrainRelation(const CTG_TrainRelation& relation)
{
	if(GetTGFunctionConfig()->IsEnableNewCzh())
		m_cRelation=relation;
}

void TG_TrainInfoExpand::UpdateTrainKediao(const CTG_TrainKediaoInfo& kd)
{
	if(GetTGFunctionConfig()->IsEnableTDMS5_0())
	{
		m_cKediao=kd;
		if(m_cKediao.type<=-1) // 防止TDi过来解析失败发生<=-1，这个需要防护正确值是0,1,2
			m_cKediao.type=0;
	}
}

void TG_TrainInfoExpand::UpdateTrainYingye(const CTG_TrainYingyeList& yingye)
{
	if(GetTGFunctionConfig()->IsEnableTDMS5_0())
		m_cYingyeList=yingye;
}

void TG_TrainInfoExpand::UpdateTrainCrew(const CTG_TrainCrewList& crew)
{
	if(GetTGFunctionConfig()->IsEnableTDMS5_0())
		m_cCrewList=crew;
}

void TG_TrainInfoExpand::UpdateTrainTicket(const CTG_TrainTicketList& ticket)
{
	if(GetTGFunctionConfig()->IsEnableTDMS5_0())
		m_cTicketList=ticket;
}

void TG_TrainInfoExpand::UpdateTrainEngineCrew(const CTG_EngineCrewList& engine)
{
	if(GetTGFunctionConfig()->IsEnableTDMS5_0())
		m_cEngineCrewList=engine;
}

