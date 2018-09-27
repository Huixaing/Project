// MyData.cpp : implementation file
//
/////////////////////////////////////////////////////////////////////////////
//
// Copyright ?1999, Stefan Belopotocan, http://welcome.to/StefanBelopotocan
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TGPspData.h"
#include "tg_basicpara.h"
#include "tg_schedule_base.h"
#include "sysshare.h"
#include "function.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


extern BYTE  GetTrainAbsTimeType(const CTG_SingleTrain* ptrain);

// SET_TYPE_INFO(8,"区间其它作业")

// 获取属性的整数形式，一般仅用于字符串形式返回为空时
long  TGPSP_GetItemIntValue(const CTG_LogicObject *pobj,int id, int stationid)
{
	if(pobj->GetObjectType()==TG_OBJ_TRAIN)
	{
		const CTG_SingleTrain *pTrain=(const CTG_SingleTrain *)pobj;
		const CTG_TrainInfo   *pTrainInfo=pTrain->GetTrainInfo_Const();
	
		int sid=TGPSP_GET_PROP_ID(id);
		switch(sid)
		{
		case ID_PD_TRAIN_IMPORTRANK:
			return pTrainInfo->m_data.super_rank; 
		case ID_PD_TRAIN_SUPER_RANK:
			return pTrainInfo->m_data.space_rank;
			break;
		
		case ID_PD_TRAIN_MILITARY_RANK:
			return pTrain->GetMilitaryRank(); 
			break;
		case ID_PD_TRAIN_ELEC:
			return pTrain->IsElectricTrain(); 
		case ID_PD_TRAIN_TYPE:
			return pTrainInfo->m_data.type;
			break;
		case ID_PD_TRAIN_RUN_TYPE:
			return pTrainInfo->m_data.run_type;
			break;
		case ID_PD_TRAIN_ABS_TIME_TYPE:
			{
				BYTE nAbsTimeType=GetTrainAbsTimeType(pTrain);
				return nAbsTimeType;
			}
			break;
		case ID_PD_TRAIN_START_PLAN_TIME:
			return pTrainInfo->m_data.start_plan_time;
		case ID_PD_TRAIN_START_ACTUAL_TIME:
			return pTrainInfo->m_data.start_actual_time;
		case ID_PD_TRAIN_SBORDER_PLAN_TIME:
			return pTrainInfo->m_data.sborder_plan_time;
		case ID_PD_TRAIN_SBORDER_ACTUAL_TIME:
			return pTrainInfo->m_data.sborder_actual_time;
		case  ID_PD_TRAIN_EBORDER_PLAN_TIME:
			return pTrainInfo->m_data.eborder_plan_time;
		case  ID_PD_TRAIN_EBORDER_ACTUAL_TIME:
			return pTrainInfo->m_data.eborder_actual_time;
		case  ID_PD_TRAIN_SBORDER_STA:
			return pTrainInfo->m_data.sborder_sta;
		case  ID_PD_TRAIN_EBORDER_STA:
			return pTrainInfo->m_data.eborder_sta;
		default:
			;
		}
		
		int rec_index=TGPSP_GET_STATION_ID(id);
		const CTG_TrainRecord *pRecord=pTrain->GetTrainRecordByRecIndex_Const(rec_index); 
		if(pRecord==NULL)
			return -1;
		if(pRecord->GetStationNo()!=stationid)
			return -1;

		switch(sid)
		{
		case ID_PD_ST_ARRIVE_SIDE_ID:
			return pRecord->m_data.arrive_side;
			
		case ID_PD_ST_DEPART_SIDE_ID:
			return pRecord->m_data.depart_side; 
		case ID_PD_ST_PLAN_ARRIVE_SIDE_ID:
			return pRecord->m_data.plan_arrive_side;
		case ID_PD_ST_PLAN_DEPART_SIDE_ID:
			return pRecord->m_data.plan_depart_side; 
	
		case ID_PD_ST_MIN_STOP_TIME:
			return pRecord->m_data.min_stop_time/60; 
		case ID_PD_ST_SCHD_ARRIVE_TIME:
			return pRecord->m_data.plan_arrive;
		case ID_PD_ST_SCHD_DEPART_TIME:
			return pRecord->m_data.plan_depart;
		case ID_PD_ST_ARRIVE_TIME:
			return pRecord->m_data.arrive;
		case ID_PD_ST_DEPART_TIME:
			return pRecord->m_data.depart;
		case ID_PD_ST_ENTRY_ID:
			{
				if(pRecord->IsStartRec()) // 2011.01.06 修正BUG
					break;   // 始发不显示上一区间
				ushort value_absnum=-1,dir=-1;
				ushort entry=pRecord->GetEntry();
				if(GetTGBasicParam()->ConvertStationEntryToAbsnum(stationid,entry,&value_absnum,&dir))  // 2011.01.06 修正BUG,首点无法显示区间
					return value_absnum;
			}
			break;
		case ID_PD_ST_EXIT_ID:
			{
				if(pRecord->IsTerminalRec()) // 2011.01.06 修正BUG
					break;   // 终到不显示下一区间
				ushort value_absnum=-1,dir=-1;
				ushort exit=pRecord->GetExit();
				if(GetTGBasicParam()->ConvertStationEntryToAbsnum(stationid,exit,&value_absnum,&dir))  // 2011.01.06 修正BUG,尾点无法显示区间
					return value_absnum;
			}
			break;
		case  ID_PD_ST_DEPART_BY_PLAN:
			if(TG_IsMakeDepartRouteByPlan(pRecord->m_data))
				return 1;
			else
				return 0;
		case  ID_PD_ST_TECH_STOP:
			if(TG_IsTechStopTrain(pRecord->m_data))
				return 1;
			else
				return 0;
		case ID_PD_ST_STATION_RECINDEX:
			return pRecord->GetIndex();
		case ID_PD_ST_MANUALSET_PASS:
			if(TG_IsDefinePass(pRecord->m_data)) 
				return 1;
			else
				return 0;
		case ID_PD_ST_NOT_MEET:
			return pRecord->m_data.can_not_meet; // 20090918 XIAN
		case ID_PD_ST_ELEC_LOCO:
			if(TG_IsElecLocoFlag(pRecord->m_data))
				return 1;
			else
				return 0;
		case ID_PD_ST_ARRIVE_RANK:
			return pRecord->GetArriveSpace(); 
		case ID_PD_ST_DEPART_RANK:
			return pRecord->GetDepartSpace(); 
		default:
			return -1;
		}
	}
	else if(pobj->GetObjectType()==TG_OBJ_MARK)
	{
		const CTG_SingleMark *pMark=(const CTG_SingleMark *)pobj;
		id=TGPSP_GET_PROP_ID(id);
		switch(id)
		{
		case ID_PD_ST_MARK_START_TIME:
			return pMark->m_data.start_time;
		case ID_PD_ST_MARK_END_TIME:
			return pMark->m_data.end_time;
		case ID_PD_ST_MARK_TYPE:
			return pMark->m_data.type;
		case ID_PD_ST_MARK_FLAG:
			return pMark->m_data.flag;
		case ID_PD_ST_MARK_DIR:
			return (pMark->m_data.line&0x03);
			//return pMark->m_data.direction; 
		case ID_PD_ST_MARK_STATION:
			return pMark->m_data.station;
		case ID_PD_ST_MARK_POSITION:
			return pMark->m_data.position;

		case ID_PD_ST_MARK_EKILO:
			return pMark->m_data.e_kilo;
		}
	}
	return -1;
}
// 获取属性的字符串形式，这是优先使用的----20090727 溢出错误
const char *TGPSP_GetItemValue(const CTG_LogicObject *pobj,int id, int stationid)
{
	static CString strRet;
	strRet.Empty();

	if(pobj->GetObjectType()==TG_OBJ_TRAIN)
	{
		const CTG_SingleTrain *pTrain=(const CTG_SingleTrain *)pobj;
		const CTG_TrainInfo *pTrainInfo=pTrain->GetTrainInfo_Const();
		if(pTrainInfo==NULL)
			return NULL;
		int sid=TGPSP_GET_PROP_ID(id);
		switch(sid)
		{
		case ID_PD_TRAIN_INDEX:
			strRet.Format("%u",pTrain->GetTrainIndex());
			return (const char*)strRet;
		case ID_PD_TRAIN_SHIFT_INDEX:
			strRet.Format("%u",pTrain->GetShiftTrainIndex());
			return (const char*)strRet;
		case ID_PD_TRAIN_IN_SECTION_TIME:
			{
				if(pTrain->GetTrainRecordCount()<1)
					return NULL;
				CTime ctm(pTrain->GetTrainRecordByRecIndex_Const(0)->m_data.arrive);
				strRet = ctm.Format("%d日 %H:%M:%S");
				return (const char*)strRet;
			}
		case ID_PD_TRAIN_OUT_SECTION_TIME:
			{
				if(pTrain->GetTrainRecordCount()<1)
					return NULL;
				CTime ctm(pTrain->GetTrainRecordByRecIndex_Const(pTrain->GetTrainRecordCount()-1)->m_data.depart);
				strRet = ctm.Format("%d日 %H:%M:%S");
				return (const char*)strRet;
			}
		case ID_PD_TRAIN_WEIGHT:
			strRet.Format("%d.%d",pTrainInfo->m_data.train_weight/10,pTrainInfo->m_data.train_weight%10);
			return (const char*)strRet;
		case ID_PD_TRAIN_LENGTH:
			strRet.Format("%d.%d",pTrainInfo->m_data.train_length/10,pTrainInfo->m_data.train_length%10);
			return (const char*)strRet;
		case ID_PD_TRAIN_COUNT:
			{
				long count = ((pTrainInfo->m_data.status>>16)&0xFFFF); // bit 16-31: 最高两个字节存放 辆数  乔永涛 20100715 应兰州局需求
				strRet.Format("%d", count);
				return (const char*)strRet;
			}
			break;
		case ID_PD_TRAIN_ID:
			strRet.Format("%s",pTrain->GetTrainID());
			return (const char*)strRet;
		case ID_PD_ST_STATION:
			strRet.Format("%s",gpcdef.GetStationName(stationid));
			return (const char*)strRet;
		case  ID_PD_TRAIN_START_STA_NAME:
			strRet.Format("%s",pTrainInfo->m_data.start_sta_name);
			return (const char*)strRet;
		case  ID_PD_TRAIN_END_STA_NAME:
			strRet.Format("%s",pTrainInfo->m_data.end_sta_name);
			return (const char*)strRet;
		case ID_PD_TRAIN_SBORDER_DELAYTIME:
			strRet.Format("%d",(pTrainInfo->m_data.sborder_actual_time-pTrainInfo->m_data.sborder_plan_time)/60);
			return (const char*)strRet;
		case ID_PD_TRAIN_TRACT_ENGINE:
			{
				strRet = GetTrainEngineDesc(pTrain,1);
				return (const char*)strRet;
			}
			break;
		case ID_PD_TRAIN_ATTACH_ENGINE:
			{
				strRet = GetTrainEngineDesc(pTrain,2);
				return (const char*)strRet;
			}
			break;
		case ID_PD_TRAIN_FIRST_STA_NAME:
			{
				const CTG_TrainRecord *pfirst_rec=pTrain->GetTrainRecordByRecIndex_Const(0);
				if(pfirst_rec)
				{ 
					strRet = gpcdef.GetStationName(pfirst_rec->GetStationNo()); 
					return (const char*)strRet;
				}
			}
			break;
		case ID_PD_TRAIN_ROUTE_NAME:
			{
				const CTG_TrainRecord *pfirst_rec=pTrain->GetTrainRecordByRecIndex_Const(0);
				const CTG_TrainRecord *plast_rec=pTrain->GetTrainRecordByRecIndex_Const(pTrain->GetTrainRecordCount()-1);
				if(pfirst_rec && plast_rec)
				{
					strRet.Format("%s-%s",gpcdef.GetStationName(pfirst_rec->GetStationNo()),gpcdef.GetStationName(plast_rec->GetStationNo()));
					return (const char*)strRet;
				}
			}
			break;
		default:
			;
		}

		int rec_index=TGPSP_GET_STATION_ID(id);
		const CTG_TrainRecord *pRecord=pTrain->GetTrainRecordByRecIndex_Const(rec_index);
		if(pRecord==NULL)
			return NULL;

		if(pRecord->GetStationNo()!=stationid)
			return NULL;

		switch(sid)
		{
		case ID_PD_ST_AUTOTRIG:
			{
				int flag=0;
				if(TG_IsAutoMakeArriveRoute(pRecord->m_data)) 
					flag+=1;
				if(TG_IsAutoMakeDepartRoute(pRecord->m_data)) 
					flag+=2;

				strRet="接发车均不允许自触";
				if(flag==1)
					strRet="接车允许自触发车不允许自触";
				if(flag==2)
					strRet="接车不允许自触发车允许自触";
				if(flag==3)
					strRet="接发车均允许自触";
			}
			break;
		case ID_PD_ST_FORCE_SIDE:
			if(TG_IsForceSide(pRecord->m_data))
				strRet="锁定";
			else
				strRet="没锁定";
			break;
				
		case ID_PD_ST_ARRIVE_TRAINID:
			strRet.Format("%s",pRecord->m_data.arrive_train_id);  
			break;
		case ID_PD_ST_DEPART_TRAINID:
			strRet.Format("%s",pRecord->m_data.depart_train_id);
			break;
		case ID_PD_ST_IMPINFO:
			return GetTrainShortImportantInfo(pRecord, pTrainInfo->m_data.space_rank);
			break;
		case ID_PD_ST_STPINFO:
			return GetTrainStopInfo(pRecord);
			break;
		case ID_PD_TRAIN_TRACT_ENGINE:
			{
				strRet = GetTrainEngineDesc(pTrain,1);
				return (const char*)strRet;
			}
			break;
		case ID_PD_TRAIN_ATTACH_ENGINE:
			{
				strRet = GetTrainEngineDesc(pTrain,2);
				return (const char*)strRet;
			}
			break;
		case ID_PD_TRAIN_FIRST_STA_NAME:
			{
				const CTG_TrainRecord *pfirst_rec=pTrain->GetTrainRecordNoDelete_Const(0);
				if(pfirst_rec)
				{ 
					strRet = gpcdef.GetStationName(pfirst_rec->GetStationNo()); 
					return (const char*)strRet;
				}
			}
			break;
		case ID_PD_TRAIN_ROUTE_NAME:
			{
				const CTG_TrainRecord *pfirst_rec=pTrain->GetTrainRecordNoDelete_Const(0);
				const CTG_TrainRecord *plast_rec=pTrain->GetTrainRecordNoDelete_Const(pTrain->GetTrainRecordCount()-1);
				if(pfirst_rec && plast_rec)
				{
					strRet.Format("%s-%s",gpcdef.GetStationName(pfirst_rec->GetStationNo()),gpcdef.GetStationName(plast_rec->GetStationNo()));
				}
			}
			break;
		default:
			;
		}
	}
	else if(pobj->GetObjectType()==TG_OBJ_MARK)
	{
		const CTG_SingleMark *pMark=(const CTG_SingleMark *)pobj;
		id=TGPSP_GET_PROP_ID(id);
		switch(id)
		{
		case ID_PD_ST_MARK_TEXT:
			strRet.Format("%s",pMark->m_data.text);
			break;
		case ID_PD_ST_MARK_STATION1:
			{
				int station1=0,station2=0;
				if(GetMarkStation(pMark,station1,station2))
					strRet.Format("%s",gpcdef.GetStationName(station1));
			}
			break;
		case ID_PD_ST_MARK_STATION2:
			{
				int station1=0,station2=0;
				if(GetMarkStation(pMark,station1,station2))
					strRet.Format("%s",gpcdef.GetStationName(station2));
			}
			break;
		case ID_PD_ST_MARK_MPLAN_ID:
			strRet.Format("%s",pMark->GetShiftIndexString());
			break;
		}
	}
	else if(pobj->GetObjectType()==TG_OBJ_SCHD)
	{
		const CTG_TrainScheduleBase *pbase=(const CTG_TrainScheduleBase *)pobj;
		CTG_ScheduleInfo schd;
		int passcnt,freicnt,markcnt;
		pbase->GetScheduleInfo(schd);
		pbase->GetTrainCount1(&passcnt,&freicnt);
		markcnt=pbase->GetMarkCount();
		id=TGPSP_GET_PROP_ID(id);
		if(schd.m_nType==CTG_ScheduleInfo::SCHEDULE_TYPE_WORK)
		{
			switch(id)
			{
			case ID_PD_CON_ID:
				strRet.Format("%d",schd.m_nSectionID);
			case ID_PD_CON_NAME:
				strRet.Format("%s",gpcdef.GetSectionName(schd.m_nSectionID));
				break;
			case ID_PD_CON_SHIFT:
				strRet.Format("%s",schd.GetShiftName());
				break;
			case ID_PD_CON_START_TIME:
				{
					TIME start,end;
					schd.GetShiftManagerRange(start,end);
					CTime ctm(start);
					strRet = ctm.Format("%Y年%m月%d日 %H时");
				}
				break;
			case ID_PD_CON_END_TIME:
				{
					TIME start,end;
					schd.GetShiftManagerRange(start,end);
					CTime ctm(end);
					strRet = ctm.Format("%Y年%m月%d日 %H时");
				}
				break;
			case ID_PD_CON_PASSENGER_TRAIN_COUNT:
				strRet.Format("%d",passcnt);
				break;
			case ID_PD_CON_FREIGHT_TRAIN_COUNT:
				strRet.Format("%d",freicnt);
				break;
			case ID_PD_CON_TRAIN_COUNT:
				strRet.Format("%d",freicnt+passcnt);
				break;
			case ID_PD_CON_MARK_COUNT:
				strRet.Format("%d",markcnt);
				break;
			}
		}
		else if(schd.m_nType==CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT)
		{
			switch(id)
			{
			case ID_PD_CON_PASSENGER_TRAIN_COUNT_SHIFT:
				strRet.Format("%d",passcnt);
				break;
			case ID_PD_CON_FREIGHT_TRAIN_COUNT_SHIFT:
				strRet.Format("%d",freicnt);
				break;
			case ID_PD_CON_TRAIN_COUNT_SHIFT:
				strRet.Format("%d",freicnt+passcnt);
				break;
			case ID_PD_CON_MARK_COUNT_SHIFT:
				strRet.Format("%d",markcnt);
				break;
			}
		}
		else if(schd.m_nType==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC)
		{
			switch(id)
			{
			case ID_PD_CON_PASSENGER_TRAIN_COUNT_BASIC:
				strRet.Format("%d",passcnt);
				break;
			case ID_PD_CON_FREIGHT_TRAIN_COUNT_BASIC:
				strRet.Format("%d",freicnt);
				break;
			case ID_PD_CON_TRAIN_COUNT_BASIC:
				strRet.Format("%d",freicnt+passcnt);
				break;
			case ID_PD_CON_MARK_COUNT_BASIC:
				strRet.Format("%d",markcnt);
				break;
			}
		}
	}
	if (strRet.IsEmpty())
		return NULL;

	return (const char*)strRet;
}

const char *TGPSP_GetItemChoice(const CTG_LogicObject *pobj,CTC_DATA_DEF datatp,int sid, int stationid,int index,int *value)
{
	if(pobj->GetObjectType()!=TG_OBJ_TRAIN)
		return NULL;

	int	id=TGPSP_GET_PROP_ID(sid);
	switch(id)
	{
		case ID_PD_TRAIN_TYPE:
			return GetTGBasicParam()->GetTrainTypeName(index,value);
		case ID_PD_TRAIN_RUN_TYPE:
			return GetTGBasicParam()->GetAbsTypeName(index,value);
		case ID_PD_TRAIN_ABS_TIME_TYPE:
			return GetTGBasicParam()->GetAbsTimeTypeNameByIndex(index,value);
		case ID_PD_ST_ARRIVE_SIDE_ID:
		case ID_PD_ST_DEPART_SIDE_ID:
		case ID_PD_ST_PLAN_ARRIVE_SIDE_ID:
		case ID_PD_ST_PLAN_DEPART_SIDE_ID:
			return GetTGBasicParam()->GetSideNameByIndex(stationid,index,value);
		default:
			;
	};
	
	
	int rec_index=TGPSP_GET_STATION_ID(sid);
	const CTG_SingleTrain *pTrain=(const CTG_SingleTrain *)pobj;
	const CTG_TrainRecord *pRecord=pTrain->GetTrainRecordByRecIndex_Const(rec_index); 
	if(pRecord!=NULL)
	{
		if(pRecord->GetStationNo()==stationid)
		{
			switch(id)
			{
				case ID_PD_ST_STATION:
				{
					*value=pRecord->GetStationNo();
					return gpcdef.GetStationName(pRecord->GetStationNo());
				}
				case ID_PD_ST_ENTRY_ID:
				{
					const CTG_TrainRecord *pRecordPrev=pTrain->GetTrainRecordByRecIndex_Const(rec_index-1); 
					if(pRecordPrev)
						return 	GetTGBasicParam()->GetAbsNameBetweenStations(pRecord->GetStationNo(),pRecordPrev->GetStationNo(),index,value);   
					else
						return 	GetTGBasicParam()->GetAbsNameBetweenStations(pRecord->GetStationNo(),0,index,value);   
				}
				case ID_PD_ST_EXIT_ID:
				{
					const CTG_TrainRecord *pRecordNext=pTrain->GetTrainRecordByRecIndex_Const(rec_index+1); 
					if(pRecordNext)
						return 	GetTGBasicParam()->GetAbsNameBetweenStations(pRecord->GetStationNo(),pRecordNext->GetStationNo(),index,value);   
					else
						return 	GetTGBasicParam()->GetAbsNameBetweenStations(pRecord->GetStationNo(),0,index,value);   
				}
				default:
					;
			}
		}
	}
	
	return GetCTCTypeChoiceString(datatp,index,value);
}

BOOL  TGPSP_SetItemValue(CTG_LogicObject *pobj,int id, int stationid,const char *strvalue)
{
	BOOL rc=FALSE;
	if(pobj->GetObjectType()==TG_OBJ_TRAIN)
	{
		CTG_SingleTrain *pTrain=(CTG_SingleTrain *)pobj;
		int rec_index=TGPSP_GET_STATION_ID(id);
		const CTG_TrainRecord *pRecord=	pTrain->GetTrainRecordByRecIndex_Const(rec_index); 
		if(pRecord->GetStationNo()!=stationid)
			return FALSE;
		id=TGPSP_GET_PROP_ID(id);
		switch(id)
		{
		case ID_PD_TRAIN_INDEX:
			break;
		case ID_PD_TRAIN_ID:
			break;
		default:
			break;
		}
	}
	else if(pobj->GetObjectType()==TG_OBJ_MARK)
	{
		CTG_SingleMark *pMark=(CTG_SingleMark *)pobj;
		id=TGPSP_GET_PROP_ID(id);
		switch(id)
		{
		case ID_PD_ST_MARK_TEXT:
			strncpy(pMark->m_data.text,strvalue,sizeof(pMark->m_data.text)-1);
			break;
		case ID_PD_ST_MARK_MPLAN_ID:
			STR_CPY(pMark->m_data.shift_string,strvalue); 
			break;
		}
	}
	return rc;
}

BOOL  ChangeTrainElecLocoFlag(CTG_SingleTrain *pTrain,int station,BOOL elec)
{
	if(pTrain->GetStationElecLocoTypeByEngineInfo(station)==0 && elec ||
		pTrain->GetStationElecLocoTypeByEngineInfo(station)==1 && !elec)
	{
		AfxGetMainWnd()->MessageBox("机车信息与设置的电力属性不一致,请在机车信息中修改","提示",MB_OK);
		return FALSE;
	}
	pTrain->DirectChangeTrainLocoTypeFlag(elec,station);
	return TRUE;
}

const char *GetObjectImportantInfo(const CTG_LogicObject *p)
{
	static CString tmpstr;

	if(p->GetObjectType()== TG_OBJ_TRAIN)
	{
		const CTG_SingleTrain *ptrain=(const CTG_SingleTrain *)p;
		const CTG_TrainInfo *pTrainInfo=ptrain->GetTrainInfo_Const();
		CString t;
		tmpstr.Format("车次 %s",ptrain->GetTrainID());
		if(ptrain->GetMilitaryRank()>0) 
		{
			t.Format("军运等级:%s,",GetCTCValueDispString(CTC_DATA_RANK,ptrain->GetMilitaryRank(),0));
			tmpstr+=t;
		}
		if(ptrain->GetSpaceRank()>0)
		{
			t.Format("超限等级:%s,",GetCTCValueDispString(CTC_DATA_RANK,ptrain->GetSpaceRank(),0));
			tmpstr+=t;
		}
		tmpstr+="\r\n";
		
		CString prev_subinfo;
		int     start_station=-1;
		int     end_station=-1;
		for(int i=0;i<ptrain->GetTrainRecordCount();i++)
		{
			const CTG_TrainRecord *prec=ptrain->GetTrainRecord_Const(i); 
			const char *psubinfo=GetTrainShortImportantInfo(prec, pTrainInfo->m_data.space_rank); 
			t.Empty();
			CString  cur_subinfo;
			if((prec->IsStationSpace() ||
				TG_IsSpecialGaurdRank(prec->m_data) ||
				TG_IsGoustRank(prec->m_data) ||
				TG_IsMilitaryRank(prec->m_data)) && psubinfo && strlen(psubinfo)>0)
				cur_subinfo=psubinfo;
			else
				cur_subinfo="";

			if(prev_subinfo==cur_subinfo)
			{
				end_station=prec->GetStationNo();
				continue;
			}
			else
			{
				if(!prev_subinfo.IsEmpty()) 
				{
					if(start_station>0 && end_station>0 && start_station!=end_station)
						t.Format("%s-%s:%s",gpcdef.GetStationName(start_station),gpcdef.GetStationName(end_station),prev_subinfo);   
					else
					{
						if(start_station==end_station || end_station<0)
							t.Format("%s:%s",gpcdef.GetStationName(start_station),prev_subinfo);   
					}
					tmpstr+=t;
				}
				start_station=prec->GetStationNo();
				end_station=start_station;
				prev_subinfo=cur_subinfo;
			}
		}
		if(!prev_subinfo.IsEmpty()) 
		{
			if(start_station>0 && end_station>0 && start_station!=end_station)
				t.Format("%s-%s:%s",gpcdef.GetStationName(start_station),gpcdef.GetStationName(end_station),prev_subinfo);   
			else
			{
				if(start_station==end_station || end_station<0)
					t.Format("%s:%s",gpcdef.GetStationName(start_station),prev_subinfo);   
			}
			tmpstr+=t;
		}

		for(int i=0;i<ptrain->GetTrainRecordCount();i++)
		{	
			const CTG_TrainRecord *prec=ptrain->GetTrainRecord_Const(i); 
			const char *psubinfo=GetTrainStopInfo(prec); 
			if((psubinfo!=NULL) && strlen(psubinfo)>0)
			{
				t.Format("%s:%s",gpcdef.GetStationName(prec->GetStationNo()),psubinfo);   
				tmpstr+=t;
				tmpstr+=";";
			}
		}
	}

	return (const char *)tmpstr;
}

const char *GetObjectWorkInfo(const CTG_LogicObject *p)
{
	static CString tmpstr; 
	if(p->GetObjectType()== TG_OBJ_TRAIN) 
	{ 
		const CTG_SingleTrain *ptrain=(const CTG_SingleTrain *)p; 
		if(GetTGFunctionConfig()->IsEnableTDMS5_0())
		{
			CString trainid; 
			trainid.Format("列车 %s ",ptrain->GetTrainID());
			tmpstr = "";

			tmpstr+=trainid+" 乘务信息:\r\n"; 
			tmpstr+=ptrain->GetImportInfoText(TRAIN_TXT_INFO_TYPE_CREW); 
			tmpstr+="\r\n============\r\n";

			if(GetTGFunctionConfig()->IsAcceptImportNote())
			{
				tmpstr+=trainid+" 重点事项:"; 
				for(int rec_index=0; rec_index<ptrain->GetTrainRecordCount(); rec_index++)
				{
					const CTG_TrainRecord *prec = ptrain->GetTrainRecordNoDelete_Const(rec_index);
					if(NULL != prec)
					{
						if(!prec->m_strNoteText.empty())
						{
							tmpstr+="\r\n";
							tmpstr+=gpcdef.GetStationName(prec->m_data.station);
							tmpstr+=":";
							tmpstr+=prec->m_strNoteText.c_str();
						}
					}
				}
				tmpstr+="\r\n============\r\n";
			}

			tmpstr+=trainid+" 甩挂信息:"; 
			tmpstr+="\r\n";
			tmpstr+=ptrain->GetImportInfo(STATION_TXT_INFO_TYPE_OPMS);
			tmpstr+="\r\n============\r\n";

			tmpstr+=trainid+" 营业信息:\r\n"; 
			tmpstr+=ptrain->GetImportInfoText(TRAIN_TXT_INFO_TYPE_YINGYE); 
			tmpstr+="\r\n============\r\n";

			tmpstr+=trainid+" 客调信息:\r\n"; 
			tmpstr+=ptrain->GetImportInfoText(TRAIN_TXT_INFO_TYPE_KEDIAO); 
			tmpstr+="\r\n============\r\n";			
		}
		else
		{
			CString trainid; 
			trainid.Format("列车 %s ",ptrain->GetTrainID()); 
			tmpstr = "";
			tmpstr+="乘务信息:"; 
			tmpstr+="\r\n";
			tmpstr+=ptrain->GetImportInfo(TRAIN_TXT_INFO_TYPE_CREW); 
			tmpstr+="\r\n============\r\n";

			if(GetTGFunctionConfig()->IsAcceptImportNote())
			{
				tmpstr+=trainid+"重点事项:"; 
				for(int rec_index=0; rec_index<ptrain->GetTrainRecordCount(); rec_index++)
				{
					const CTG_TrainRecord *prec = ptrain->GetTrainRecordNoDelete_Const(rec_index);
					if(NULL != prec)
					{
						if(!prec->m_strNoteText.empty())
						{
							tmpstr+="\r\n";
							tmpstr+=gpcdef.GetStationName(prec->m_data.station);
							tmpstr+=":";
							tmpstr+=prec->m_strNoteText.c_str();
						}
					}
				}
				tmpstr+="\r\n============\r\n";
			}

			tmpstr+=trainid+"甩挂信息:"; 
			tmpstr+="\r\n";
			tmpstr+=ptrain->GetImportInfo(STATION_TXT_INFO_TYPE_OPMS);
			tmpstr+="\r\n============\r\n";

			tmpstr+=trainid+"营业信息:"; 
			tmpstr+="\r\n";
			tmpstr+=ptrain->GetImportInfo(TRAIN_TXT_INFO_TYPE_YINGYE); 
			tmpstr+="\r\n============\r\n";

			tmpstr+=trainid+"客调信息:"; 
			tmpstr+="\r\n";
			tmpstr+=ptrain->GetImportInfo(TRAIN_TXT_INFO_TYPE_KEDIAO); 
		}
	} 
	else if(p->GetObjectType()== TG_OBJ_MARK)
	{
		const CTG_SingleMark *pmark=(const CTG_SingleMark *)p;
		const char *pmarktype=GetCTCValueDispString(CTC_DATA_MARK_TYPE,pmark->GetMarkType(),0);
		if(pmarktype)
		{
			int station1=0,station2=0;
			if(!GetMarkStation(pmark,station1,station2))
				tmpstr.Format("%s:%s", pmarktype,pmark->m_data.text);
			else
				if(pmark->IsAbsMarkType())
				{
					tmpstr.Format("%s-%s %s:%s",gpcdef.GetStationName(station1),gpcdef.GetStationName(station2),pmarktype,pmark->m_data.text);
				}
				else
				{
					tmpstr.Format("%s%s-%s:",gpcdef.GetStationName(pmark->GetStation()), pmarktype,pmark->m_data.text);
				}
		}
		else
			return "";
	}
	return (const char *)tmpstr;
}

const char * GetObjectAllInfo(const CTG_LogicObject *p)
{
	static CString tmpstr;
	tmpstr=GetObjectWorkInfo(p);
	tmpstr+="\r\n";
	//tmpstr+=GetObjectImportantInfo(p);
	return (const char *)tmpstr;
}


const char * GetTrainShortImportantInfo(const CTG_TrainRecord *prec, BYTE space_rank)
{
	static CString tmpstr;
	tmpstr.Empty();
	if(prec==NULL)
		return NULL;
	
	if(TG_IsElecLocoFlag(prec->m_data))
		tmpstr+="电力,";
	if(prec->IsStationSpace())
	{
		BYTE nArriveSpace=prec->GetArriveSpace();
		BYTE nDepartSpace=prec->GetDepartSpace();

		if(GetTGFunctionConfig()->IsStplanSpaceRankByStation())
		{
			if(nArriveSpace==nDepartSpace && nArriveSpace>0)
			{
				if(nArriveSpace==1)
					tmpstr+="超级超限,";
				else if(nArriveSpace==2)
					tmpstr+="一级超限,";
				else if(nArriveSpace==3)
					tmpstr+="二级超限,";
				else if(nArriveSpace==4)
					tmpstr+="三级超限,";
			}
			else
			{
				if(nArriveSpace==1)
					tmpstr+="接车超级超限,";
				else if(nArriveSpace==2)
					tmpstr+="接车一级超限,";
				else if(nArriveSpace==3)
					tmpstr+="接车二级超限,";
				else if(nArriveSpace==4)
					tmpstr+="接车三级超限,";

				if(nDepartSpace==1)
					tmpstr+="发车超级超限,";
				else if(nDepartSpace==2)
					tmpstr+="发车一级超限,";
				else if(nDepartSpace==3)
					tmpstr+="发车二级超限,";
				else if(nDepartSpace==4)
					tmpstr+="发车三级超限,";
			}
		}
		else
		{
			if(nArriveSpace>0 && nDepartSpace>0)
			{
				if(nArriveSpace == nDepartSpace)
				{
					if(nArriveSpace==1)
						tmpstr+="超级超限,";
					else if(nArriveSpace==2)
						tmpstr+="一级超限,";
					else if(nArriveSpace==3)
						tmpstr+="二级超限,";
					else if(nArriveSpace==4)
						tmpstr+="三级超限,";
				}
				else
				{
					if(nArriveSpace==1)
						tmpstr+="接车超级超限,";
					else if(nArriveSpace==2)
						tmpstr+="接车一级超限,";
					else if(nArriveSpace==3)
						tmpstr+="接车二级超限,";
					else if(nArriveSpace==4)
						tmpstr+="接车三级超限,";

					if(nDepartSpace==1)
						tmpstr+="发车超级超限,";
					else if(nDepartSpace==2)
						tmpstr+="发车一级超限,";
					else if(nDepartSpace==3)
						tmpstr+="发车二级超限,";
					else if(nDepartSpace==4)
						tmpstr+="发车三级超限,";
				}
			}
			else 
			{
				if(nArriveSpace>0 && nDepartSpace<=0)
				{
					nDepartSpace=space_rank;
					if(nDepartSpace<=0)
						nDepartSpace=nArriveSpace;
				}
				else if(nArriveSpace<=0 && nDepartSpace>0)
				{
					nArriveSpace=space_rank;
					if(nArriveSpace<=0)
						nArriveSpace=nDepartSpace;
				}
				else
				{
					BYTE nSpace=space_rank;
					if(nSpace>0)
					{
						nArriveSpace = nSpace;
						nDepartSpace = nSpace;
					}
				}

				if(nArriveSpace==1)
					tmpstr+="接车超级超限,";
				else if(nArriveSpace==2)
					tmpstr+="接车一级超限,";
				else if(nArriveSpace==3)
					tmpstr+="接车二级超限,";
				else if(nArriveSpace==4)
					tmpstr+="接车三级超限,";

				if(nDepartSpace==1)
					tmpstr+="发车超级超限,";
				else if(nDepartSpace==2)
					tmpstr+="发车一级超限,";
				else if(nDepartSpace==3)
					tmpstr+="发车二级超限,";
				else if(nDepartSpace==4)
					tmpstr+="发车三级超限,";
			}
		}
	}

	if(TG_IsSpecialGaurdRank(prec->m_data))
		tmpstr+="警卫,";
	if(TG_IsGoustRank(prec->m_data))
		tmpstr+="毒品,";
	if(TG_IsMilitaryRank(prec->m_data))
		tmpstr+="军运,";

	return (const char *)tmpstr;
}

const char * GetTrainStopInfo(const CTG_TrainRecord *prec)
{
	static CString tmpstr;
	tmpstr.Empty();
	if(prec)
	{
		if(TG_IsMustNotStop(prec->m_data))
			tmpstr+="禁停";
		if(TG_IsMustStop(prec->m_data))
			tmpstr+="必停,";
		if(TG_IsMustNoPass(prec->m_data))  // "禁止通过";
			tmpstr+="禁通,";
	}
	
	return (const char *)tmpstr;
}

BOOL  GetMarkStation(const CTG_SingleMark  *pmark,int &station1,int &station2)
{
	if(pmark->IsStationMarkType())
	{
		station1=pmark->m_data.station;  
		station2=0;
		return TRUE;
	}
	else if(pmark->IsAbsMarkType())
	{
		station1=pmark->m_data.station1;
		station2=pmark->m_data.station2;
		return TRUE;
	}
	return FALSE;
}


const char *GetTrainEngineDesc(const CTG_SingleTrain *ptrain,int flag) // flag:1 TRACT, 2:ATTACH, 3: ALL
{
	static CString tmpstr;
	tmpstr.Empty();
	for(int i=0;i<ptrain->GetEngineCount();i++)
	{
		const CTG_TrainEngineInfo *prec=ptrain->GetEngine_Const(i);
		if(prec->IsMainTractEngine() && (flag & 1))
		{
			tmpstr+=prec->GetEngineName();
			tmpstr+=",";
		}
		if(!prec->IsMainTractEngine() && (flag & 2))
		{
			tmpstr+=prec->GetEngineName();
			tmpstr+=",";
		}
	}
	return (const char *)tmpstr;
}

const char *Compare2Trains(const CTG_SingleTrain *srctrain, const CTG_SingleTrain *desttrain,DWORD &flag, int nThisSection, WORD& nTipToDdy)
{
	nTipToDdy=0;
	static CString tmpstr;
	CString otherstr;
	tmpstr.Empty();
	flag=0;
	int rec_count=srctrain->GetTrainRecordCount();
	int dest_rec_count=desttrain->GetTrainRecordCount();

	bool isSameRecord=true;
	if(dest_rec_count==rec_count)
		isSameRecord=true;
	else
		isSameRecord=false;

	if(desttrain->GetTrainRecordCount()>rec_count)
		rec_count=desttrain->GetTrainRecordCount();

	for(int i=0;i<rec_count;i++)
	{
		const CTG_TrainRecord *prec=srctrain->GetTrainRecordNoDelete_Const(i);
		const CTG_TrainRecord *prec_dest=desttrain->GetTrainRecordNoDelete_Const(i);
		if(i>=srctrain->GetTrainRecordCount())
			prec=NULL;
		if(i>=desttrain->GetTrainRecordCount())
			prec_dest=NULL;
		if(prec==NULL && prec_dest!=NULL)
		{
			tmpstr.Format("径路延伸");
			nTipToDdy=1;
			break;
		}
		else if(prec!=NULL && prec_dest==NULL)
		{
			tmpstr.Format("径路缩短");
			nTipToDdy=1;
			break;
		}
		if(prec->GetStationNo()!=prec_dest->GetStationNo())
		{
			tmpstr.Format("径路改变");
			nTipToDdy=1;
			break;
		}
		else
		{
			if(CString(prec->m_data.arrive_train_id)!=CString(prec_dest->m_data.arrive_train_id) ||
				CString(prec->m_data.depart_train_id)!=CString(prec_dest->m_data.depart_train_id))
			{
				tmpstr.Format("车次变更");  
			}
		}
		if(i==0)
		{
			if(prec->IsStartRec()!=prec_dest->IsStartRec())
			{
				tmpstr.Format("接入标志改变"); 
				nTipToDdy=1;
			}
		}
		if(isSameRecord && (i+1==rec_count) && prec->IsTerminalRec()!=prec_dest->IsTerminalRec())
		{
			tmpstr.Format("交出标志改变");
			nTipToDdy=1;
		}
		if(prec->IsPassRec() && !prec_dest->IsPassRec())
		{
			tmpstr.Format("%s站停车",gpcdef.GetStationName(prec_dest->GetStationNo()));  
		}
		if(prec->m_data.min_stop_time!=prec_dest->m_data.min_stop_time)
		{
			tmpstr.Format("%s站作业变化",gpcdef.GetStationName(prec_dest->GetStationNo()));  
		}
		CString shuaigua=prec->GetStationText(STATION_TXT_INFO_TYPE_OPMS);
		CString dest_shuaigua=prec_dest->GetStationText(STATION_TXT_INFO_TYPE_OPMS);
		if(shuaigua!=dest_shuaigua)
		{
			tmpstr.Format("%s站作业变化",gpcdef.GetStationName(prec_dest->GetStationNo()));  
		}
	}

	for(int i=0;i<rec_count;i++)
	{
		const CTG_TrainRecord *prec=srctrain->GetTrainRecordNoDelete_Const(i);
		const CTG_TrainRecord *prec_dest=desttrain->GetTrainRecordNoDelete_Const(i);
		if(i>=srctrain->GetTrainRecordCount())
			prec=NULL;
		if(i>=desttrain->GetTrainRecordCount())
			prec_dest=NULL;
	
		if(prec==NULL || prec_dest==NULL)
			break;
		
		if(prec->GetStationNo()!=prec_dest->GetStationNo())
			break;
		if(i==0 || GetTGFunctionConfig()->IsEnableRecvAdjustPlanTime())  // 只比较始发接入时间
		{
			if(nThisSection>0&&GetTGFunctionConfig()->IsThisSectionStation(nThisSection, prec->m_data.station))
			{
				if(prec->GetArriveTime()!=prec_dest->GetArriveTime())
				{
					otherstr.Format("到达时间改变");  
					break;
				}
			}
			else if(nThisSection>0&&GetTGFunctionConfig()->IsNotThisSectionStation(nThisSection, prec->m_data.station))
			{
				if(prec->GetArriveTime()!=prec_dest->GetArriveTime())
				{
					otherstr.Format("到达时间改变");  
					break;
				}
				if(prec->m_data.arrive_side!=prec_dest->m_data.arrive_side || prec->m_data.depart_side!=prec_dest->m_data.depart_side)
				{
					otherstr.Format("%s 股道变化", gpcdef.GetStationName(prec->m_data.station));  
					break;
				}
			}
			else
			{
				if(prec->GetDepartTime()!=prec_dest->GetDepartTime())
				{
					otherstr.Format("发车时间改变");  
					break;
				}
				else if(prec->GetArriveTime()!=prec_dest->GetArriveTime())
				{
					otherstr.Format("到达时间改变");  
					break;
				}
			}
		}
	}
	if(tmpstr.IsEmpty())
	{
		if(otherstr.IsEmpty()) 
			flag=0;
		else
		{
			tmpstr=otherstr;
			flag=1;
		}
	}
	else
		flag=1;
	return (const char *)tmpstr;
}
