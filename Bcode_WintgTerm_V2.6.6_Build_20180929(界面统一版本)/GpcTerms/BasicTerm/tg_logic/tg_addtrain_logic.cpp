
#include "stdafx.h"
#include "tg_addtrain_logic.h"
#include "function.h"	
#include "TgDocument.h"
#include "gpcdef.h"
#include "sysshare.h"

extern BYTE  GetTrainAbsTimeType(const CTG_SingleTrain* ptrain);
extern void  SetTrainAbsTimeType(CTG_SingleTrain* train, BYTE run_type);
BOOL CTGAddTrainLogic::SetTrainRecordListFromPartPassway(CTG_SingleTrain *ptrain, int start_idx, int end_idx, const CSingle_Passway &passway_info)
{
	int rec_count=passway_info.m_vector_Passway_Sta_info.size();
	if(rec_count<=0 || end_idx>=rec_count || start_idx>=rec_count || start_idx>end_idx)
		return FALSE;
	CTG_TrainRecordList &rec_list=*ptrain->GetRecordList();
	rec_list.PurgeRecords();
	for(int i=start_idx; i<=end_idx&&i<rec_count;i++)
	{
		CTG_TrainRecord rec;
		const Passway_Sta &p_sta= passway_info.m_vector_Passway_Sta_info[i];
		rec.m_data.station = p_sta.Sta_id;
		rec.m_data.entry = p_sta.Entry;
		// NA??? 
		//rec.m_data.arrive_side = p_sta.Arrive_Side;
		//rec.m_data.depart_side = p_sta.Depart_Side;
		rec.m_data.exit = p_sta.Exit;	
		rec.SetArriveTGIndex(p_sta.in_sta_index);
		rec.SetDepartTGIndex(p_sta.out_sta_index);
		rec_list.AddRecord(rec);
	}
	return TRUE;
}

BOOL CTGAddTrainLogic::SetTrainRecordListFromPassway(CTG_SingleTrain *ptrain,const CSingle_Passway &passway_info)
{
	int rec_count=passway_info.m_vector_Passway_Sta_info.size();
	if(rec_count<=0)
		return FALSE;
	CTG_TrainRecordList &rec_list=*ptrain->GetRecordList();
	rec_list.PurgeRecords();
	for(int i=0;i<rec_count;i++)
	{
		CTG_TrainRecord rec;
		const Passway_Sta &p_sta= passway_info.m_vector_Passway_Sta_info[i];
		rec.m_data.station = p_sta.Sta_id;
		rec.m_data.entry = p_sta.Entry;
		// NA??? 
		//rec.m_data.arrive_side = p_sta.Arrive_Side;
		//rec.m_data.depart_side = p_sta.Depart_Side;
		rec.m_data.exit = p_sta.Exit;	
		rec.SetArriveTGIndex(p_sta.in_sta_index);
		rec.SetDepartTGIndex(p_sta.out_sta_index);
		rec_list.AddRecord(rec);
	}
	return TRUE;
}

BOOL CTGAddTrainLogic::SetTrainRecordListTime(CTG_SingleTrain *ptrain,STATION_NO station, TIME atime, TIME dtime)
{
	int sta_idx_b,sta_idx_e,rec_count;
	int i;
	int run_time;
	CTG_TrainRecordList &rec_list=*ptrain->GetRecordList();
	rec_count=rec_list.GetRecordCount();
	if(NULL_TIME == atime ||  NULL_TIME == dtime || rec_count<=0)
		return FALSE;
	if(NULL_STATION == station) 
	{
		// set first record
		sta_idx_b = sta_idx_e = 0;
		rec_list[0].m_data.arrive = atime;
		rec_list[0].m_data.depart = dtime;
	}
	else  // define one station 
	{
		// set specified record
		for (i=0; i<rec_count && rec_list[i].m_data.station != station; i++);
		if (i == rec_count) 
		{
			return  FALSE;
		}
		sta_idx_b = sta_idx_e = i;
		rec_list[sta_idx_b].m_data.arrive = atime;
		rec_list[sta_idx_b].m_data.depart = dtime;
	}

	if (sta_idx_b > 0) 
	{
		for (i=sta_idx_b-1; i>=0; i--) 
		{
			run_time = CalculateTrainRunTime(ptrain,i,TRAIN_OUT_STATION);
			rec_list[i].m_data.depart = rec_list[i+1].m_data.arrive - run_time;
			if (rec_list[i+1].m_data.arrive != rec_list[i+1].m_data.depart)
				rec_list[i].m_data.depart += CalculateTrainStopExtra(ptrain,i+1,TRAIN_INTO_STATION);

			rec_list[i].m_data.arrive = rec_list[i].m_data.depart;
		}
	}

	if (sta_idx_e < rec_count-1) 
	{
		for (i=sta_idx_e+1; i< rec_count; i++) 
		{
			run_time = CalculateTrainRunTime(ptrain,i, TRAIN_INTO_STATION);
			rec_list[i].m_data.arrive = rec_list[i-1].m_data.depart + run_time;
			if (rec_list[i-1].m_data.arrive != rec_list[i-1].m_data.depart || rec_list[i-1].IsStartRec()) 
				rec_list[i].m_data.arrive += CalculateTrainStartExtra(ptrain, i-1, TRAIN_OUT_STATION);

			if ((i==rec_count-1) && rec_list[i].IsTerminalRec())
				rec_list[i].m_data.arrive += CalculateTrainStopExtra(ptrain, i-1,TRAIN_INTO_STATION);

			rec_list[i].m_data.depart = rec_list[i].m_data.arrive;
		}
	}
	for (i=0; i<rec_count; i++)
	{
		TG_SetSideFromPathWay(rec_list[i].m_data);

		TG_SetRecTimeFromSimpleAdjust(rec_list[i].m_data, TRAIN_ARRIVE);
		TG_SetRecTimeFromSimpleAdjust(rec_list[i].m_data, TRAIN_DEPART);

		TG_SetPlanTimeFromManualSet(rec_list[i].m_data, TRAIN_ARRIVE);
		TG_SetPlanTimeFromManualSet(rec_list[i].m_data, TRAIN_DEPART);

		rec_list[i].m_data.plan_arrive = rec_list[i].m_data.arrive;
		rec_list[i].m_data.plan_depart = rec_list[i].m_data.depart;
	}
	return TRUE;
}

BOOL CTGAddTrainLogic::SetTrainRecordListFlag(CTG_SingleTrain *ptrain,BYTE start_flag, BYTE end_flag)
{
	CTG_TrainRecordList &rec_list=*ptrain->GetRecordList();
	int rec_count=rec_list.GetRecordCount();
	if(rec_count<1)
		return FALSE;
	if (start_flag == 1)
		TG_SetFlagStart(rec_list[0].m_data.flag);	// Start
	else
		TG_SetFlagAccept(rec_list[0].m_data.flag);	// Accept
	
	if (end_flag == 3)
		TG_SetFlagTerminal(rec_list[rec_count-1].m_data.flag);	// Terminal
	else
		TG_SetFlagGiveout(rec_list[rec_count-1].m_data.flag);		// Giveout
	
	return TRUE;
}

BOOL CTGAddTrainLogic::SetTrainInfoByTrainID(CTG_SingleTrain *ptrain,const TRAIN_NO train_no)
{
	BYTE type, run_type, prio;
	if(m_pBasicPara->GetTrainPrio(train_no, &type, &run_type, &prio))
	{
		ptrain->GetTrainInfo()->m_data.type = type;
		ptrain->GetTrainInfo()->m_data.run_type = run_type;
		ptrain->GetTrainInfo()->m_data.priority = prio;
	}
	return TRUE;
}

BOOL CTGAddTrainLogic::AdjustTrainRecordSide(CTG_SingleTrain *ptrain, int begin, int end, bool keep_old, bool bShiftSchdule)
{
	CTG_TrainRecordList &rec_list=*ptrain->GetRecordList();
	int i=0, rec_count=rec_list.GetRecordCount();

	if (begin != -1) i=begin;
	if (end != -1 && rec_count >end) rec_count = end;
		
	if(keep_old)
	{
		if((rec_count>0)&&
		   (!IS_VALID_SIDE(rec_list[rec_count-1].m_data.arrive_side) 
		     ||!IS_VALID_SIDE(rec_list[rec_count-1].m_data.depart_side)))
		{
			rec_list[rec_count-1].m_data.arrive_side=255;
			rec_list[rec_count-1].m_data.depart_side=255;

			// 对于客车终到站,是否设置为非技术停点
			PassengerTrainEndSetNotTechStop(ptrain);
			PassengerTrainStartSetNotTechStop(ptrain);
		}
	}
	else
	{
		// 对于客车终到站,是否设置为非技术停点
		PassengerTrainEndSetNotTechStop(ptrain);
		PassengerTrainStartSetNotTechStop(ptrain);
	}

	bool bSelFirstBackupSide=GetTGFunctionConfig()->IsShiftTrainSelFirstBackupSide();
	for(;i<rec_count;i++)
	{
		if(keep_old && IS_VALID_SIDE(rec_list[i].m_data.arrive_side))  // 可选保留原有合法股道，比如用于展开方式自由画线,调整计划原有股道
			continue;
		if(keep_old && IS_VALID_SIDE(rec_list[i].m_data.depart_side))
			continue;

		// 日班计划客车,南昌局办客只能选第一备用股道20161117
		if(bShiftSchdule && ptrain->IsPassengerTrain() && bSelFirstBackupSide)
		{
			m_pAutoAdjust->OnlyUseFirstStopSideForNanchang(ptrain, i);
			continue;
		}
		m_pAutoAdjust->ChangeSideForStopOrPass(ptrain,i,1,-1,m_pScheduleInfo->m_nType);  // 对于新生成列车应该允许生成默认股道,不考虑一些"股道不能被自动调整"因素
	}
	return TRUE;
}


BOOL CTGAddTrainLogic::GenTrainTmp_Passway(CTG_SingleTrain *ptrain,const CString train_no, 
										   int con_id,USHORT PasswayId, 
										   BYTE start_flag, BYTE end_flag, 
										   BYTE start_index, BYTE end_index, TIME atime, TIME dtime,BYTE run_type)
{
	short  rc = FALSE;

	CTG_TrainRecordList &rec_list=*ptrain->GetRecordList();
	int rec_count=rec_list.GetRecordCount();
	TG_TrainInfo &coninfo=ptrain->GetTrainInfo()->m_data;
	CSingle_Passway Single_Passway;

	do 
	{
		if(!m_pBasicPara->m_cPasswayInfo.GetSinglePassway(con_id,PasswayId,Single_Passway))
			break;
	
		// should set coninfo first
		if(!SetTrainInfoByTrainID(ptrain,train_no))
			break;
		TG_SetTrainFromPathWay(coninfo);
		coninfo.basic_train_index = 0;
		if(!SetTrainRecordListFromPartPassway(ptrain, start_index, end_index, Single_Passway))
			break;

		if(!SetTrainRecordListFlag(ptrain,start_flag,end_flag))
			break;

		BYTE nOldRunType=ptrain->GetTrainInfo()->m_data.run_type;

		// 2014.05.27 沈阳 区间时分类型
		bool enable=GetTGBasicParam()->IsEnableABSTimeType();
		if(enable)
		{
			BYTE nNewRunType=ptrain->GetTrainInfo()->m_data.run_type;
			if(run_type!=255)
				nNewRunType=run_type;

			SetTrainAbsTimeType(ptrain, nNewRunType);
			ptrain->GetTrainInfo()->m_data.run_type = nNewRunType;
		}
		GpcLogTrainMessage(0, 0, "按照径路加车, train=%s 使用选中的run_type %d", ptrain->GetTrainID(), ptrain->GetTrainInfo()->m_data.run_type);

		if(!SetTrainRecordListTime(ptrain,NULL_STATION, atime, dtime))
			break;
		
		if(enable)
			ptrain->GetTrainInfo()->m_data.run_type=nOldRunType;

		if(!AdjustTrainRecordSide(ptrain))
			break;

		ptrain->SetTrainID(train_no);  // 必须在记录生成后调用，否则记录中无车次
		ptrain->SetTrainRecordCountToDefault(); // 20090622
		SetTrainInitFlag(ptrain); //
		SetDefaultAttrib(ptrain);
		SetDefaultByBasicSchedule(ptrain, FALSE, 0, PASSWAY);
		if(m_pAutoAdjust != NULL)
			m_pAutoAdjust->SetTrainPassNonMainSide(ptrain);

		return TRUE;

	} while(0);

	return FALSE;
}

//   station: -1, atime, dtime is the first station 's arrive and depart time,
//            else the specified station in pathway
//   ltime == -1 && station == -1: use time in rec
BOOL CTGAddTrainLogic::GenTrain_Passway(CTG_SingleTrain *ptrain,const TRAIN_NO train_no, int con_id,USHORT PasswayId, BYTE start_flag, BYTE end_flag, STATION_NO station, TIME atime, TIME dtime,BYTE run_type)
{
	short  rc = FALSE;

	CTG_TrainRecordList &rec_list=*ptrain->GetRecordList();
	int rec_count=rec_list.GetRecordCount();
	TG_TrainInfo &coninfo=ptrain->GetTrainInfo()->m_data;
	CSingle_Passway Single_Passway;
//  20090919 XIAN, 判断经路属性
	
	do 
	{
		if(!m_pBasicPara->m_cPasswayInfo.GetSinglePassway(con_id,PasswayId,Single_Passway))
			break;
		int inflag=0,outflag=0;
        GetTGBasicParam()->GetPasswayFlag(con_id,PasswayId,&inflag,&outflag);  // 20090919XIAN
		if(start_flag == 0 && end_flag == 0) // 这是来之邻台的车,需根据Passway确定
		{
			start_flag = SCHD_ACCEPT; // 默认接入,交出，下面按照passway中限定修改 20091020
			end_flag=SCHD_GIVEOUT;
		}
		
		if(inflag==SCHD_START) // 只能始发
			start_flag=SCHD_START;
		if(outflag==SCHD_TERMINAL) // 只能终到
			end_flag=SCHD_TERMINAL;
		// should set coninfo first
		if(!SetTrainInfoByTrainID(ptrain,train_no))
			break;
		TG_SetTrainFromPathWay(coninfo);
		coninfo.basic_train_index = 0;
		if(!SetTrainRecordListFromPassway(ptrain,Single_Passway))
			break;

		if(!SetTrainRecordListFlag(ptrain,start_flag,end_flag))
			break;

		BYTE nOldRunType=ptrain->GetTrainInfo()->m_data.run_type;

		// 2014.05.27 沈阳 区间时分类型
		bool enable=GetTGBasicParam()->IsEnableABSTimeType();
		if(enable)
		{
			BYTE nNewRunType=ptrain->GetTrainInfo()->m_data.run_type;
			if(run_type!=255)
				nNewRunType=run_type;

			SetTrainAbsTimeType(ptrain, nNewRunType);
			ptrain->GetTrainInfo()->m_data.run_type = nNewRunType;
		}
		GpcLogTrainMessage(0, 0, "按照径路加车, train=%s 使用选中的run_type %d", ptrain->GetTrainID(), ptrain->GetTrainInfo()->m_data.run_type);
		
		if(!SetTrainRecordListTime(ptrain,station, atime, dtime))
		{
			ptrain->GetTrainInfo()->m_data.run_type=nOldRunType;
			break;
		}
		if(enable)
			ptrain->GetTrainInfo()->m_data.run_type=nOldRunType;
		
		if(!AdjustTrainRecordSide(ptrain))
			break;
	
		ptrain->SetTrainID(train_no);  // 必须在记录生成后调用，否则记录中无车次
		ptrain->SetTrainRecordCountToDefault(); // 20090622
		SetTrainInitFlag(ptrain); //
		SetDefaultAttrib(ptrain);
		SetDefaultByBasicSchedule(ptrain, FALSE, 0, PASSWAY);

		SetDefualtEntryAndExitBySide(ptrain); // 徐州东--徐州存车场间 股道决定出入口 放在股道确定之后
		if(m_pAutoAdjust != NULL)
			m_pAutoAdjust->SetTrainPassNonMainSide(ptrain);

		return TRUE;

	} while(0);

	return FALSE;
}


BOOL CTGAddTrainLogic::GenTrain_BasicSchedule(CTG_SingleTrain *ptrain,const CTG_SingleTrain &srctrain, STATION_NO station, TIME atime, TIME dtime, const TRAIN_NO spec_train, const TRAIN_NO spec_original_train)
{
	short    i;
	
	*ptrain=srctrain;   // 初值用BASIC
	CTG_TrainInfo  &train_info=*ptrain->GetTrainInfo();
	CTG_TrainRecordList &rec=*ptrain->GetRecordList();

	int    rec_count=rec.GetRecordCount();

	TG_SetTrainFromBasicSchedule(train_info.m_data);


	train_info.m_data.basic_train_index =srctrain.GetTrainIndex();
	train_info.m_data.shift_train_index = NULL_TRAININDEX;

	if (FALSE == supply_schedule_current_date(ptrain,station, atime, dtime, NULL)) 
	{
		GpcLogTrainMessage(0, 0, "supply_schedule_current_date fail, basic_index=%d",srctrain.GetTrainIndex());
		return FALSE;
	}

	for(i=0; i<rec_count; i++)
	{
		rec[i].m_data.plan_arrive_side = rec[i].m_data.arrive_side;
		rec[i].m_data.plan_depart_side = rec[i].m_data.depart_side;

		GpcLogTrainMessage(0, 0, "基本图股道 train=%s basic_index %d,  %s(%d):min_stop_time %d 到达股道 %d 出发股道 %d",
								srctrain.GetTrainID(), 
								srctrain.GetTrainIndex(),
								gpcdef.GetStationName(rec[i].m_data.station),  
								rec[i].m_data.station,
								rec[i].m_data.min_stop_time,
								rec[i].m_data.arrive_side, 
								rec[i].m_data.depart_side);

		TG_SetArrivePlan(rec[i].m_data);
		TG_SetDepartPlan(rec[i].m_data);

		TG_SetRecTimeFromDefault(rec[i].m_data, TRAIN_ARRIVE);
		TG_SetRecTimeFromDefault(rec[i].m_data, TRAIN_DEPART);

		TG_SetSideFromBasicSchedule(rec[i].m_data);

		TG_SetPlanTimeFromBasicSchedule(rec[i].m_data, TRAIN_ARRIVE);
		TG_SetPlanTimeFromBasicSchedule(rec[i].m_data, TRAIN_DEPART);

		// good train not use basic schule side	
		if(TG_IsFreightTrainType(ptrain->GetTrainType()))
		{
			if(GetTGFunctionConfig()->IsGoodTrainUseBasicSide())
			{
				// 货车使用基本图股道, 比如南昌局
				GpcLogTrainMessage(0, 0, "根据配置GoodTrainSideSameToPlan, train=%s %s(%d) 是货物列车, 配置使用基本图股道", 
									ptrain->GetTrainID(), 
									gpcdef.GetStationName(rec[i].m_data.station),
									rec[i].m_data.station);
			}
			else
			{
				// 货车不使用基本图股道, 比如沈阳局
				rec[i].m_data.arrive_side = 255;
				rec[i].m_data.depart_side = 255;
				GpcLogTrainMessage(0, 0, "train=%s %s(%d) 是货物列车, 不使用基本图股道", 
									ptrain->GetTrainID(), 
									gpcdef.GetStationName(rec[i].m_data.station),
									rec[i].m_data.station);
			}
		}
		
		if ((255 == rec[i].m_data.arrive_side) && (255 == rec[i].m_data.depart_side))
		{
			m_pAutoAdjust->ChangeSideForStopOrPass(ptrain,i);
		
			GpcLogTrainMessage(0, 0, "[股道分配 计算]:GenTrain_BasicSchedule %s(%d) 在车站%d(entry %d exit %d)计算后股道 arrive_side %d depart_side %d", 
					ptrain->GetTrainID(),ptrain->GetTrainIndex(),rec[i].m_data.station, rec[i].m_data.entry, rec[i].m_data.exit,
					rec[i].m_data.arrive_side, rec[i].m_data.depart_side);
		}
	}

	if(m_pConfig->IfCalculateBasicTrainType()) // 20161021 重点测试
	{
		BYTE type, run_type, prio;
		if(m_pBasicPara->GetTrainPrio(ptrain->GetTrainID(), &type, &run_type, &prio))
		{
			train_info.m_data.type = type;
			train_info.m_data.run_type = run_type;
			train_info.m_data.priority = prio;
		}
	}


	if(NULL != spec_train && !ptrain->MatchTrainID(spec_train))
	{
		ptrain->SetTrainID(spec_train);

		BYTE type, run_type, prio;
		m_pBasicPara->GetTrainPrio(spec_train, &type, &run_type, &prio);
		train_info.m_data.type = type;
		if (NULL == spec_original_train)
		{
			train_info.m_data.run_type = run_type;
			train_info.m_data.priority = prio;
		}
	}

	if (NULL != spec_original_train)
		ptrain->SetOrigTrainID(spec_original_train);

	SetTrainInitFlag(ptrain);
	SetDefaultAttrib(ptrain);
	if (NULL_STATION == station) 
		return TRUE;

	//change arrive and depart time according to atime and dtime at station
	if (NULL_TIME != atime && NULL_TIME != dtime && atime > dtime) 
	{
		GpcLogTrainMessage(0, 0, "err data specified sta %d atime %d dtime %d", station, atime, dtime);
		return FALSE;;
	}

	for (i=0; i<rec_count && rec[i].m_data.station != station; i++);
	if (i == rec_count) 
	{
		GpcLogTrainMessage(0, 0, "Cannot find specified sta %d atime %d dtime %d", station, atime, dtime);
		return FALSE;
	}

	long   time_off_set;
	USHORT sta_idx = i;
	if (NULL_TIME != atime) 
	{
		time_off_set = atime - rec[sta_idx].m_data.arrive;
		rec[sta_idx].m_data.arrive = atime;
		for (i=0; i<sta_idx; i++)
		{
			TG_SetArriveTimeOff(rec[i].m_data, time_off_set);
			TG_SetDepartTimeOff(rec[i].m_data, time_off_set);
		}
	}
	if (NULL_TIME != dtime) 
	{
		time_off_set = dtime - rec[sta_idx].m_data.depart;
		rec[sta_idx].m_data.depart = dtime;
		for (i=sta_idx+1; i<rec_count; i++) 
		{
			TG_SetArriveTimeOff(rec[i].m_data, time_off_set);
			TG_SetDepartTimeOff(rec[i].m_data, time_off_set);
		}
	}
	SetDefaultAttrib(ptrain);
	return TRUE;
}

BOOL CTGAddTrainLogic::supply_schedule_current_date(CTG_SingleTrain *ptrain, STATION_NO spec_sta, TIME spec_atime, TIME spec_dtime,long *time_off_set_ptr)
{
	ushort i,j;

	TIME btime,etime;
	
	btime=time(NULL)-ADDTRAIN_BASIC_SCHEDULE_MAX_BEFORE_TIME;
	etime=btime+ADDTRAIN_BASIC_SCHEDULE_MAX_TIME_RANGE;

	if (NULL_TIME == btime || NULL_TIME == etime)
		return FALSE;

	CTG_TrainRecordList &rec=*ptrain->GetRecordList();
	int    rec_count=rec.GetRecordCount();


	// set all default values
	if (NULL_STATION == spec_sta) 
	{
		TIME middle = TG_GetValidADTime(rec[0].m_data) /2 + TG_GetValidADTime(rec[rec_count-1].m_data) /2;
		for (i=rec_count-1; i>0; i--)
			if (TG_GetValidADTime(rec[i].m_data) < middle) break;
		spec_sta = rec[i].m_data.station;
	}
	else 
	{
		for (i=0; i<rec_count; i++)
			if (rec[i].m_data.station == spec_sta) break;
		if (i >= rec_count) 
		{
			//m_Task.ErrLog("can not find sta %d in qualify basic time", spec_sta);
			return FALSE;
		}
	}

	// calculate timeoff
	long  time_off_set = 0;

	TIME  tmp = TG_GetValidADTime(rec[i].m_data);

	if (NULL_TIME == tmp)
	{
		//m_Task.ErrLog("supply schedule current date rec[%d] err sta %d arrive depart is both invalid", i, rec[i].station);
		return FALSE;
	}

	long sdate;
	minute_time(btime, &sdate);
	TIME specified_time = long_time(sdate, tmp % (24*3600));

	if (specified_time < btime && specified_time+24*3600 <= etime)
	{
		specified_time += 24*3600;
	}
	else if (specified_time < btime && specified_time+24*3600 > etime)
	{
		if (btime - specified_time > specified_time+24*3600 - etime)
			specified_time += 24*3600;
	}	
	else if (specified_time > etime && specified_time-24*3600 >= btime)
	{
		specified_time -= 24*3600;
	}
	else if (specified_time > etime && specified_time-24*3600 < btime)
	{
		if (specified_time-etime > btime-(specified_time-24*3600))
			specified_time -= 24*3600;
	}

	time_off_set = specified_time - tmp;

	// get the time and plan time
	for (j=0; j<rec_count; j++) 
	{
		TG_TrainRecord &core_rec=rec[j].m_data;
		if (rec[j].m_data.arrive != NULL_TIME)
		{
			rec[j].m_data.arrive = rec[j].m_data.arrive + time_off_set;
			rec[j].m_data.plan_arrive = rec[j].m_data.arrive;
		}
		else
		{
			rec[j].m_data.plan_arrive = NULL_TIME;
		}

		if (rec[j].m_data.depart != NULL_TIME)
		{
			rec[j].m_data.depart = rec[j].m_data.depart + time_off_set;
			rec[j].m_data.plan_depart = rec[j].m_data.depart;
		}
	}

	time_off_set=0;
	// offset arrive and depart time before sta
	if (spec_atime != NULL_TIME)
		time_off_set = spec_atime - rec[i].m_data.arrive;

	if (NULL_TIME != rec[i].m_data.arrive) 
		rec[i].m_data.arrive += time_off_set;
	for (j=0;j<i; j++ ) 
	{
		if (rec[j].m_data.arrive != NULL_TIME)
			rec[j].m_data.arrive += time_off_set;

		if (rec[j].m_data.depart != NULL_TIME)
			rec[j].m_data.depart += time_off_set;
	}

	// offset arrive and depart time after sta
	if (spec_dtime != NULL_TIME)
		time_off_set = spec_dtime - rec[i].m_data.depart;

	if (NULL_TIME != rec[i].m_data.depart) rec[i].m_data.depart += time_off_set;
	for (j=i+1;j<rec_count;j++) 
	{
		if (rec[j].m_data.arrive != NULL_TIME)
			rec[j].m_data.arrive += time_off_set;

		if (rec[j].m_data.depart != NULL_TIME)
			rec[j].m_data.depart += time_off_set;
	}
	if (time_off_set_ptr != NULL)
		*time_off_set_ptr = time_off_set;


	// make plan arrive better
	time_off_set = rec[0].m_data.plan_depart - rec[0].m_data.depart;
	if (abs(time_off_set) > 24 * 3600)
	{
		if (time_off_set > 0) time_off_set = (-1)*24*3600;
		else if (time_off_set < 0) time_off_set = 24*3600;
		for (i=0; i<rec_count; i++) 
		{
			if (rec[i].m_data.plan_arrive != NULL_TIME)
				rec[i].m_data.plan_arrive += time_off_set;

			if (rec[i].m_data.plan_depart != NULL_TIME)
				rec[i].m_data.plan_depart += time_off_set;
		}
	}

	return TRUE;
}

int   CTGAddTrainLogic::CalculateTrainRunTime(CTG_SingleTrain *ptrain,int rec_index,TRAIN_IN_OUT in_out)
{
	short   run_time=-1;
	CTG_TrainRecordList &reclist=*ptrain->GetRecordList();
	int    rec_count=reclist.GetRecordCount();
	if(rec_index<0 || rec_index>=rec_count)
		return -1;
	CTG_TrainRecord &rec=reclist[rec_index];
	STATION_NO station=rec.GetStationNo();
	ENTRY_ID entry=rec.GetEntryNumber(in_out);
	BYTE  trn_type=ptrain->GetRunType();
	return m_pBasicPara->CalculateTrainRunTime(station, entry, in_out, trn_type);
}

int   CTGAddTrainLogic::CalculateTrainStopExtra(CTG_SingleTrain *ptrain,int rec_index,TRAIN_IN_OUT in_out)
{
	short   run_time=-1;
	CTG_TrainRecordList &reclist=*ptrain->GetRecordList();
	int    rec_count=reclist.GetRecordCount();
	if(rec_index<0 || rec_index>=rec_count)
		return -1;
	CTG_TrainRecord &rec=reclist[rec_index];
	STATION_NO station=rec.GetStationNo();
	ENTRY_ID entry=rec.GetEntryNumber(in_out);
	BYTE  trn_type=ptrain->GetRunType();
	return m_pBasicPara->CalculateTrainStopExtra(station, entry, in_out, trn_type);
}
int   CTGAddTrainLogic::CalculateTrainStartExtra(CTG_SingleTrain *ptrain,int rec_index,TRAIN_IN_OUT in_out)
{
	short   run_time=-1;
	CTG_TrainRecordList &reclist=*ptrain->GetRecordList();
	int    rec_count=reclist.GetRecordCount();
	if(rec_index<0 || rec_index>=rec_count)
		return -1;
	CTG_TrainRecord &rec=reclist[rec_index];
	STATION_NO station=rec.GetStationNo();
	ENTRY_ID entry=rec.GetEntryNumber(in_out);
	BYTE  trn_type=ptrain->GetRunType();
	return m_pBasicPara->CalculateTrainStartExtra(station, entry, in_out, trn_type);
}


BOOL CTGAddTrainLogic::IsBasicScheduleTrain(CTG_SingleTrain *ptrain)
{
	return TG_IsBasicScheduleTrain(ptrain->GetTrainInfo()->m_data);
}

BOOL CTGAddTrainLogic::SetTrainInitFlag(CTG_SingleTrain *ptrain)
{
	CTG_TrainRecordList &rec=*ptrain->GetRecordList();
	int    rec_count=rec.GetRecordCount();
	if(rec_count<=0)
		return FALSE;
	if (TG_IsFlagStart(rec[0].m_data.flag))
	{
		rec[0].m_data.arrive = rec[0].m_data.depart;
		TG_SetNotAutoMakeArriveRoute(rec[0].m_data);
		GpcLogTrainMessage(0, 0, "列车%s 在车站%d始发, 接车进路不允许自触", ptrain->GetTrainID(), rec[0].m_data.station);
		if (TG_IsPassengerTrainType(ptrain->GetTrainType())) 
			TG_SetMakeDepartRouteByPlan(rec[0].m_data);
		else
			TG_SetMakeDepartRouteByMeet(rec[0].m_data);
	}
	else 
	{
		TG_SetFlagAccept(rec[0].m_data.flag);
	}

	if (TG_IsFlagTerminal(rec[rec_count-1].m_data.flag))
	{
		rec[rec_count-1].m_data.depart = rec[rec_count-1].m_data.arrive;
		TG_SetNotAutoMakeDepartRoute(rec[rec_count-1].m_data);
		GpcLogTrainMessage(0, 0, "列车%s 在车站%d终到, 发车进路不允许自触", ptrain->GetTrainID(), rec[rec_count-1].m_data.station);
	}
	else if (rec_count >1)
	{
		TG_SetFlagGiveout(rec[rec_count-1].m_data.flag);
	}

	for (int i=0; i<rec_count; i++) 
	{
		// early depart time
		if (TG_IsPassengerTrainType(ptrain->GetTrainType())) 
		{
			if (rec[i].m_data.min_stop_time)
				TG_SetCanNotEarlyDepart(rec[i].m_data);
			else
				TG_SetNoLimitEarlyDepart(rec[i].m_data);
		}
		else
		{
			TG_SetNoLimitEarlyDepart(rec[i].m_data);
		}

		// set cannot auto trig route
		if (TG_IsFlagStopAbs(rec[i].m_data.flag)
			|| (i >0 && TG_IsFlagStopAbs(rec[i-1].m_data.flag)))
		{
			TG_SetNotAutoMakeArriveRoute(rec[i].m_data);
			TG_SetNotAutoMakeDepartRoute(rec[i].m_data);
			GpcLogTrainMessage(0, 0, "列车%s 在车站%d区间停车, 接发车进路不允许自触", ptrain->GetTrainID(), rec[i].m_data.station);
		}

		// set define pass
		if (IsBasicScheduleTrain(ptrain)				
			&& rec[i].m_data.plan_arrive == rec[i].m_data.plan_depart
			&& !TG_IsFlagStart(rec[i].m_data.flag) 
			&& !TG_IsFlagTerminal(rec[i].m_data.flag))
		{
			TG_SetDefinePass(rec[i].m_data);
		}
		else
		{
			TG_ReSetDefinePass(rec[i].m_data);
		}

		// set tech stop
		if (!IsBasicScheduleTrain(ptrain) && m_pConfig->IfSetDefaultTechStop())
		{
			TG_SetTechStopTrain(rec[i].m_data); // 设置技术停点
		}		
	}
	
	// 设置反向行车
	CTgDocument::SetTrainReverseFlag(ptrain);
	CTgDocument::SetTrainFromToConid(m_pScheduleInfo->m_nSectionID, ptrain);

	return SetTrainFlag(ptrain);
}

BOOL CTGAddTrainLogic::SetTrainFlag(CTG_SingleTrain *ptrain)
{
	int i;
	CTG_TrainRecordList &rec=*ptrain->GetRecordList();
	int    rec_count=rec.GetRecordCount();
	if(rec_count<=0)
		return FALSE;
	TG_TrainInfo *coninfo=&ptrain->GetTrainInfo()->m_data;
	for (i=0; i<rec_count; i++) 
	{
		if (i>0) 
			TG_RemoveFlagEnterStatus(rec[i].m_data.flag);
		if (i<rec_count-1)
			TG_RemoveFlagLeaveStatus(rec[i].m_data.flag);

		TG_ClearFlagPassNonMainSide(rec[i].m_data.flag);
		TG_ClearFlagStopMainSide(rec[i].m_data.flag);

		if (IS_VALID_SIDE(rec[i].m_data.arrive_side) && IS_VALID_SIDE(rec[i].m_data.depart_side))
		{
			if (TG_IsTrainPass(rec[i].m_data)) 
			{
				if (m_pBasicPara->IsStopSide(rec[i].m_data))
					TG_SetFlagPassNonMainSide(rec[i].m_data.flag);
			}
			else if (!TG_IsFlagStart(rec[i].m_data.flag) && !TG_IsFlagTerminal(rec[i].m_data.flag))
			{
				if (rec[i].m_data.arrive_side == rec[i].m_data.depart_side && m_pBasicPara->IsMainSide(rec[i].m_data))
					TG_SetFlagStopMainSide(rec[i].m_data.flag);
			}
		}

		if (TG_IsTrainPass(rec[i].m_data)) 
		{
			if (TG_IsMakeDepartRouteByPlan(rec[i].m_data))
				TG_SetMakeDepartRouteByMeet(rec[i].m_data);
		}

		if (i<rec_count-1) 
		{
			short run_time = CalculateTrainRunTime(ptrain,i,TRAIN_OUT_STATION);
			if (m_pBasicPara->VerifyEntryAndAbs(rec[i].m_data.station, rec[i].m_data.exit, rec[i+1].m_data.station, rec[i+1].m_data.entry)) 
			{
				if (!TG_IsTrainPass(rec[i].m_data)) 
					run_time += CalculateTrainStartExtra(ptrain,i, TRAIN_OUT_STATION);
				if (!TG_IsTrainPass(rec[i+1].m_data))
					run_time += CalculateTrainStopExtra(ptrain,i+1,TRAIN_INTO_STATION);
		        // 区间运缓
				TG_SetRunFastSlowTime(rec[i].m_data.run_fast_slow_time, (short)(rec[i+1].m_data.arrive-rec[i].m_data.depart-run_time));
			} 
			else 
			{
				TG_SetRunFastSlowTime(rec[i].m_data.run_fast_slow_time, 0);
			}
		}
	}

	for (i=rec_count-1; i>=0; i--) 
	{
		if (TG_IsArriveActual(rec[i].m_data) || TG_IsDepartActual(rec[i].m_data))
			break;
	}


	if (TG_GetAdjustBeginSta(coninfo) < i+1 || TG_IfAdjustBeginStaAutoSet(coninfo))
	{
		coninfo->adjust_begin_sta = i+1;
		TG_SetAdjustBeginStaAutoSet(coninfo);
	}
	if (coninfo->adjust_end_sta <= TG_GetAdjustBeginSta(coninfo) ||
		coninfo->adjust_end_sta  >= rec_count)
	{
		coninfo->adjust_end_sta = rec_count-1;
	}

	// remove error trainid NA???
	// if same trainid and original trainid NA???
	/*
	short flag=0;
	for(i=0; i<coninfo->train_id_count; i++)
	{
	if (!strncmp(coninfo->train_id[i], coninfo->original_train_id[i], sizeof(TRAIN_NO)))
	flag = 1;
	else
	flag = 0;
	}
	if (flag)
	{
	for(i=0; i<coninfo->train_id_count; i++)
	memset(coninfo->original_train_id[i], 0, sizeof(TRAIN_NO));
	}*/

	if (0xFF != m_pConfig->GetSuperSpaceTrainType() && coninfo->type == m_pConfig->GetSuperSpaceTrainType())
		if (!coninfo->space_rank) coninfo->space_rank = 1;
	return TRUE;	
}


BOOL  CTGAddTrainLogic::GenTrain_FreeDraw(CTG_SingleTrain *ptrain,BOOL qualify_runtime,BYTE run_type) // train中是界面生成的初始数据
{
	CTG_TrainRecordList &rec=*ptrain->GetRecordList();
	int    rec_count=rec.GetRecordCount();
	int    i;
	if(rec_count<=0)
		return FALSE;
	TG_TrainInfo &coninfo=ptrain->GetTrainInfo()->m_data;

	const char *train_name = rec[0].m_data.depart_train_id; 
	if(strlen(train_name)==0)
	{
		train_name=rec[0].m_data.arrive_train_id;
		if(strlen(train_name)==0)
			return FALSE;
	}
	SetTrainInfoByTrainID(ptrain,train_name);  // 对自由画线强制计算列车类型

	TG_SetTrainFromFreeDraw(coninfo);

	BYTE nOldRunType= ptrain->GetTrainInfo()->m_data.run_type;

	// 2014.05.27 沈阳 区间时分类型
	bool enable=GetTGBasicParam()->IsEnableABSTimeType();
	if(enable)
	{
		BYTE nNewRunType=ptrain->GetTrainInfo()->m_data.run_type;
		if(run_type!=255)
			nNewRunType=run_type;

		SetTrainAbsTimeType(ptrain, nNewRunType);
		ptrain->GetTrainInfo()->m_data.run_type=nNewRunType;
	}
	GpcLogTrainMessage(0, 0, "自由画线, train=%s 使用选中的run_type %d", ptrain->GetTrainID(), ptrain->GetTrainInfo()->m_data.run_type);

	coninfo.basic_train_index = 0;
	
	if (TG_IsFlagStart(rec[0].m_data.flag)) rec[0].m_data.entry = NULL_ENTRY;
	if (TG_IsFlagTerminal(rec[rec_count-1].m_data.flag)) rec[rec_count-1].m_data.exit = NULL_ENTRY;


	int run_time = (rec_count>1)?(rec[1].m_data.arrive - rec[0].m_data.depart):0;
	for (i=0; i< rec_count-1; i++) 
	{
		BOOL if_modi_abs_time=ptrain->GetTrainRecord(i+1)->m_modify_abs_time>0 || qualify_runtime;// i->i+1 应该用下一记录的标志 20090722

		int stop_time = rec[i+1].m_data.depart-rec[i+1].m_data.arrive;
		if (TG_IsFlagStopAbs(rec[i].m_data.flag) || !if_modi_abs_time)
			rec[i+1].m_data.arrive = rec[i].m_data.depart + run_time;
		else
			rec[i+1].m_data.arrive = rec[i].m_data.depart + CalculateTrainRunTime(ptrain,i,TRAIN_OUT_STATION);

		if(if_modi_abs_time) 
		{
		if ((rec[i].m_data.depart != rec[i].m_data.arrive) || ((i==0)&&TG_IsFlagStart(rec[0].m_data.flag)))
			rec[i+1].m_data.arrive += CalculateTrainStartExtra(ptrain,i,TRAIN_OUT_STATION);
		if (stop_time  || ((i==rec_count-2)&&TG_IsFlagTerminal(rec[i+1].m_data.flag)))
			rec[i+1].m_data.arrive += CalculateTrainStopExtra(ptrain,i,TRAIN_INTO_STATION);
		}
		//rec[i+1].m_data.depart = rec[i+1].m_data.arrive + stop_time;
		if (i<rec_count-2)
			run_time = rec[i+2].m_data.arrive - rec[i+1].m_data.depart;

		rec[i+1].m_data.depart = rec[i+1].m_data.arrive + stop_time;  // 20091020 修改depart必须在计算完run_time后
	}
	if(enable)
		ptrain->GetTrainInfo()->m_data.run_type=nOldRunType;
	
	for (i=0; i<rec_count; i++)
	{
		rec[i].m_data.plan_arrive = rec[i].m_data.arrive;
		rec[i].m_data.plan_depart = rec[i].m_data.depart;
	}
	ptrain->SetTrainRecordCountToDefault(); 
		
	// 保留自由画线的股道
	AdjustTrainRecordSide(ptrain,-1,-1,true,false);
	SetTrainInitFlag(ptrain); // 
	SetDefualtEntryAndExitByGpcdef(ptrain, true);
	SetDefualtEntryAndExit(ptrain);
	CTgDocument::SetTrainReverseFlag(ptrain); // 济南局,晏城--晏城北出入口设置配置,需在此计算反向行车

	SetDefaultAttrib(ptrain);
	SetDefaultByBasicSchedule(ptrain, FALSE, 0, FREE_DRAW);
	SetDefualtEntryAndExitBySide(ptrain); // 徐州东--徐州存车场间 股道决定出入口 放在股道确定之后
	if(m_pAutoAdjust != NULL)
		m_pAutoAdjust->SetTrainPassNonMainSide(ptrain);
	return TRUE;
}

// 从index开始,以index为基准向前推算运行时间
BOOL  CTGAddTrainLogic::RecacuTrainTimeForward(CTG_SingleTrain *ptrain, int index) 
{
	if (ptrain->GetTrainRecordCount() <= index || index <= 0) return FALSE;

	CTG_TrainRecordList &rec=*ptrain->GetRecordList();		
	if (rec[index].m_data.arrive==0) rec[index].m_data.arrive = rec[index].m_data.depart;
	if(rec[index].m_data.arrive==0)
		return FALSE;  // 没有时间,无法进行后续推算

	for (int i=index-1; i>=0; i--) 
	{
		int stop_extra_time=0; 
		int start_extra_time=0; 
		int stop_time=0;
		if(rec[i].m_data.arrive>0 && rec[i].m_data.depart>0 && rec[i].m_data.depart>rec[i].m_data.arrive)
		{
			stop_time=rec[i].m_data.depart-rec[i].m_data.arrive;
			start_extra_time=m_pBasicPara->CalculateTrainStartExtra(rec[i].GetStationNo(),rec[i].GetExit(),TRAIN_OUT_STATION,ptrain->GetRunType());
		}
		rec[i].m_data.depart = rec[i+1].m_data.arrive - CalculateTrainRunTime(ptrain,i,TRAIN_OUT_STATION)-start_extra_time;
		if(stop_time>0)
			stop_extra_time=m_pBasicPara->CalculateTrainStopExtra(rec[i].GetStationNo(),rec[i].GetEntry(),TRAIN_INTO_STATION,ptrain->GetRunType());
		rec[i].m_data.arrive = rec[i].m_data.depart - stop_time - stop_extra_time;
	}
	return TRUE;
}

// 从index开始,以index为基准向前推算运行时间
BOOL  CTGAddTrainLogic::RecacuTrainTimeBackward(CTG_SingleTrain *ptrain, int index) 
{
	if (ptrain->GetTrainRecordCount() <= index || index <= 0) 
		return FALSE;
	CTG_TrainRecordList &rec=*ptrain->GetRecordList();
	int    rec_count = ptrain->GetTrainRecordCount();
	if(rec_count<=0)
		return FALSE;

	// 第一个记录没有时间,无法进行后续推算
	if(rec[0].m_data.depart==0 && rec[0].m_data.arrive==0)
		return FALSE;  	

	for (int i=index; i< rec_count; i++) 
	{					
		int stop_extra_time=0; 
		int stop_time=rec[i].m_data.min_stop_time;
		if(rec[i].m_modify_abs_time<=0)
			continue;
	
		if(rec[i].m_data.arrive>0 && rec[i].m_data.depart>0 && rec[i].m_data.depart>rec[i].m_data.arrive)
			stop_time=rec[i].m_data.depart-rec[i].m_data.arrive;

		if(stop_time>0)
			stop_extra_time=m_pBasicPara->CalculateTrainStopExtra(rec[i].GetStationNo(),rec[i].GetEntry(),TRAIN_INTO_STATION,ptrain->GetRunType());
		else
		{
			if(i==rec_count-1)
			{
				if(rec[i].IsTerminalRec())
					stop_extra_time=m_pBasicPara->CalculateTrainStopExtra(rec[i].GetStationNo(),rec[i].GetEntry(),TRAIN_INTO_STATION,ptrain->GetRunType());
			}
		}
		rec[i].m_data.arrive = rec[i-1].m_data.depart + CalculateTrainRunTime(ptrain,i-1,TRAIN_OUT_STATION)+stop_extra_time;
		rec[i].m_data.depart=rec[i].m_data.arrive+stop_time;
	}
	return TRUE;
}

// 按照发车时间重新计算各站时分, only_firsttime 为是否从第一个开始推算,否则从到达时间为0的推算
BOOL  CTGAddTrainLogic::RecacuTrainTime(CTG_SingleTrain *ptrain,BOOL only_firsttime) 
{
	CTG_TrainRecordList &rec=*ptrain->GetRecordList();
	int    rec_count = ptrain->GetTrainRecordCount();
	int    i;
	if(rec_count<=0)
		return FALSE;
	if(rec[0].m_data.depart==0 && rec[0].m_data.arrive==0)
	{
		GpcLogTrainMessage(0, 0, "推算列车 %s 运行时分, 首站时间为零,推算失败.", ptrain->GetTrainID());
		return FALSE;  // 第一个记录没有时间,无法进行后续推算
	}
	BOOL has_zero_time=only_firsttime;

	for (i=1; i< rec_count; i++) 
	{	
		if(rec[i].m_data.arrive>0 && ptrain->IsPassengerTrain()) // 20100322 对于旅客列车，有点则必须保留
		{
			if (i != rec_count-1 && rec[i].m_data.depart <= 0 )
			{
				rec[i].m_data.depart = rec[i].m_data.arrive+rec[i].m_data.min_stop_time;
			}
			continue;
		}

		// 20100322 错误，此处不应该是rec[i].m_data.arrive>=0,否则导致0点没有计算
		if(rec[i].m_data.arrive>0 && !has_zero_time)
		{
			if (i != rec_count-1 && rec[i].m_data.depart <= 0 )
				rec[i].m_data.depart = rec[i].m_data.arrive;

			continue;
		}
		
		if(rec[i].m_data.arrive<=0 && !has_zero_time)
			has_zero_time=TRUE;

		int stop_extra_time=0; 
		int start_extra_time=0; 
		int stop_time=rec[i].m_data.min_stop_time;

		if(!rec[i].IsPassRec())
			stop_time=rec[i].m_data.depart-rec[i].m_data.arrive;

		if(stop_time>0)
			stop_extra_time=m_pBasicPara->CalculateTrainStopExtra(rec[i].GetStationNo(),rec[i].GetEntry(),TRAIN_INTO_STATION,ptrain->GetRunType());

		if(!rec[i-1].IsPassRec() || rec[i-1].IsStartRec())
			start_extra_time=m_pBasicPara->CalculateTrainStartExtra(rec[i-1].GetStationNo(),rec[i-1].GetExit(),TRAIN_OUT_STATION,ptrain->GetRunType());
		rec[i].m_data.arrive = rec[i-1].m_data.depart + CalculateTrainRunTime(ptrain,i-1,TRAIN_OUT_STATION)+start_extra_time+stop_extra_time;
		
		rec[i].m_data.depart=rec[i].m_data.arrive+stop_time;
	}
	return TRUE;
}

BOOL CTGAddTrainLogic::GenTrainBasic_OPMS(CTG_SingleTrain *ptrain)
{
	TG_TrainInfo &coninfo=ptrain->GetTrainInfo()->m_data;
	CTG_TrainRecordList &rec=*ptrain->GetRecordList();
	int  rec_count=rec.GetRecordCount();
	if(rec_count<=0)
		return FALSE;

	const char *train_name = rec[0].m_data.depart_train_id;
	if(strlen(train_name)==0)
	{
		train_name=rec[0].m_data.arrive_train_id;
		if(strlen(train_name)==0)
			return FALSE;
	}
	
	SetTrainInfoByTrainID(ptrain,train_name);
	GpcLogTrainMessage(0, 0, "计算基本图车次 %s 类型 %d", train_name, ptrain->GetTrainType());

	TG_SetTrainInfoFromShift(coninfo);
	coninfo.basic_train_index = 0;
	
	if (TG_IsFlagStart(rec[0].m_data.flag)) rec[0].m_data.entry = NULL_ENTRY;
	if (TG_IsFlagTerminal(rec[rec_count-1].m_data.flag)) rec[rec_count-1].m_data.exit = NULL_ENTRY;

	ptrain->SetTrainRecordCountToDefault();

	SetDefualtEntryAndExitByGpcdef(ptrain);
	SetDefualtEntryAndExit(ptrain);
	
	int iSection=m_pScheduleInfo->m_nSectionID;
	UseTDMSEntryExitByConfig(ptrain, iSection);
	// 必须在计算股道之前,确定好出入口
	BOOL bUseOPMSBasicSchdSide = GetTGFunctionConfig()->IsEnableOPMSBasicSchdSide(iSection);
	if(bUseOPMSBasicSchdSide && !ptrain->IsPassengerTrain())
	{
		if(!GetTGFunctionConfig()->IsGoodTrainUseOPMSSide(ptrain->GetTrainType()))
			bUseOPMSBasicSchdSide=FALSE;
	}
	if(bUseOPMSBasicSchdSide)
	{
		GpcLogTrainMessage(0, 0, "基本图列车GID:%u 车次:%s 使用OPMS股道", ptrain->GetShiftTrainIndex(), ptrain->GetTrainID());
	}
	else
	{
		int rec_count=ptrain->GetTrainRecordCount();
		for(int i=0; i<rec_count; i++)
		{
			CTG_TrainRecord *prec=ptrain->GetTrainRecordByRecIndex(i);
			if(prec)
			{
				prec->m_data.arrive_side=255;
				prec->m_data.depart_side=255;
			}
		}
		// 对于客车终到站,是否设置为非技术停点
		PassengerTrainEndSetNotTechStop(ptrain);
		PassengerTrainStartSetNotTechStop(ptrain);
		m_pAutoAdjust->CacuDefaultSide(ptrain); // 2010.2.26 按照站细计算股道，不考虑列车之间关系，可以提高获取速度
		for(int i=0; i<rec_count; i++)
		{
			CTG_TrainRecord *prec=ptrain->GetTrainRecordByRecIndex(i);
			if(prec)
			{
				GpcLogTrainMessage(0, 0, "列车GID:%u 车次:%s 车站%d 计算股道(entry %d exit %d) arrive_side:%d depart_side:%d", 
					ptrain->GetShiftTrainIndex(), ptrain->GetTrainID(), prec->m_data.station,
					prec->m_data.entry, prec->m_data.exit,
					prec->m_data.arrive_side, prec->m_data.depart_side);
			}
		}
	}
	
	SetTrainInitFlag(ptrain); //
	SetDefaultAttrib(ptrain);
	SetDefaultByBasicSchedule(ptrain, bUseOPMSBasicSchdSide, 0, BASIC);

	if (bUseOPMSBasicSchdSide && GetTGFunctionConfig()->IsBasicValidSideByBasicSchd())
		SetValidSideByBasicSchedule(ptrain);

	if (bUseOPMSBasicSchdSide && GetTGFunctionConfig()->IsBasicValidSideByCalculate())
		SetValidSideByCacu(ptrain);

	if (m_pConfig->SetPassengerTrainNoTechStopLockSide())
	{
		SetSideLockPassengerTrainNotTechStop(ptrain);
	}
	SetDefualtEntryAndExitBySide(ptrain); // 徐州东--徐州存车场间 股道决定出入口 放在股道确定之后
	if(m_pAutoAdjust != NULL)
		m_pAutoAdjust->SetTrainPassNonMainSide(ptrain);
	return TRUE;
}

BOOL  CTGAddTrainLogic::GenTrain_OPMS(CTG_SingleTrain *ptrain,BOOL newflag) // train中是OPMS转换来的车次
{
	TG_TrainInfo &coninfo=ptrain->GetTrainInfo()->m_data;
	CTG_TrainRecordList &rec=*ptrain->GetRecordList();
	int    rec_count=rec.GetRecordCount();
	if(rec_count<=0)
		return FALSE;

	const char *train_name = rec[0].m_data.depart_train_id;
	//const char *train_name=ptrain->GetTrainInfo()->m_data.train_name; // 空
	if(strlen(train_name)==0)
	{
		train_name=rec[0].m_data.arrive_train_id;
		if(strlen(train_name)==0)
			return FALSE;
	}
	SetTrainInfoByTrainID(ptrain,train_name);

	TG_SetTrainInfoFromShift(coninfo);
	coninfo.basic_train_index = 0;
	
	if (TG_IsFlagStart(rec[0].m_data.flag)) rec[0].m_data.entry = NULL_ENTRY;
	if (TG_IsFlagTerminal(rec[rec_count-1].m_data.flag)) rec[rec_count-1].m_data.exit = NULL_ENTRY;

	ptrain->SetTrainRecordCountToDefault();

	SetDefualtEntryAndExitByGpcdef(ptrain);
	SetDefualtEntryAndExit(ptrain);

	// 客车需要提前设置营业三角,下面计算股道的时候选择站台用到
	if(ptrain->IsPassengerTrain())
	{
		int rec_count=ptrain->GetTrainRecordCount();
		for(int i=0; i<rec_count; i++)
		{
			CTG_TrainRecord *prec=ptrain->GetTrainRecordByRecIndex(i);
			if(prec)
			{
				if(prec->m_data.min_stop_time>0) 
				{
					TG_ClearTechStopTrain(prec->m_data);
					GpcLogTrainMessage(0, 0, "[技术停点 计算]: %s (%u),根据停车时间 %d 设置 %s 为非技术停点", 
						ptrain->GetTrainID(), ptrain->GetShiftTrainIndex(), prec->m_data.min_stop_time, gpcdef.GetStationName(prec->m_data.station));
				}
			}
		}

	}
	// 对于客车终到站,是否设置为非技术停点
	PassengerTrainEndSetNotTechStop(ptrain);
	PassengerTrainStartSetNotTechStop(ptrain);

	int iSection=m_pScheduleInfo->m_nSectionID;
	UseTDMSEntryExitByConfig(ptrain, iSection);
	// 必须在计算股道之前,确定好出入口
	BOOL bUseOPMSShiftSchdSide = GetTGFunctionConfig()->IsEnableOPMSShiftSchdSide(iSection);
	if(bUseOPMSShiftSchdSide && !ptrain->IsPassengerTrain())
	{
		if(!GetTGFunctionConfig()->IsGoodTrainUseOPMSSide(ptrain->GetTrainType()))
			bUseOPMSShiftSchdSide=FALSE;
	}
	if(bUseOPMSShiftSchdSide)
	{
		GpcLogTrainMessage(0, 0, "列车GID:%u 车次:%s 使用OPMS股道", ptrain->GetShiftTrainIndex(), ptrain->GetTrainID());
	}
	else
	{
		if (newflag) // 新车重新计算所有股道，否则重新计算无效股道
			AdjustTrainRecordSide(ptrain, -1, -1, false, true);
		else
			AdjustTrainRecordSide(ptrain, -1, -1, true, true);

		int rec_count=ptrain->GetTrainRecordCount();
		for(int i=0; i<rec_count; i++)
		{
			CTG_TrainRecord *prec=ptrain->GetTrainRecordByRecIndex(i);
			if(prec)
			{
				GpcLogTrainMessage(0, 0, "列车GID:%u 车次:%s 车站%d 计算股道(entry %d exit %d) arrive_side:%d depart_side:%d", 
					ptrain->GetShiftTrainIndex(), ptrain->GetTrainID(), prec->m_data.station,
					prec->m_data.entry, prec->m_data.exit,
					prec->m_data.arrive_side, prec->m_data.depart_side);
			}
		}
	}

	SetTrainInitFlag(ptrain);
	SetDefaultAttrib(ptrain);
	SetDefaultByBasicSchedule(ptrain, bUseOPMSShiftSchdSide, 0, SHIFT);

	if (m_pConfig->SetPassengerTrainNoTechStopLockSide())
	{
		SetSideLockPassengerTrainNotTechStop(ptrain);
	}

	if(GetTGFunctionConfig()->UseOPMSTrainTimeTrainType(ptrain->GetTrainType()))
		RecacuTrainTime(ptrain,FALSE);
	else
	{
		if(newflag && !ptrain->IsPassengerTrain()) 
			RecacuTrainTime(ptrain,TRUE); // 新货车车，仅用第一个记录的时间
		else
			RecacuTrainTime(ptrain,FALSE);
	}
	SetDefualtEntryAndExitBySide(ptrain); // 徐州东--徐州存车场间 股道决定出入口 放在股道确定之后
	if(m_pAutoAdjust != NULL)
		m_pAutoAdjust->SetTrainPassNonMainSide(ptrain);
	return TRUE;
}

BOOL  CTGAddTrainLogic::SetDefaultAttrib(CTG_SingleTrain *ptrain) //对于新增车，设置默认属性
{
	BYTE run_type,prio,locotype;

	if(m_pBasicPara->GetTrainTypeDef(ptrain->GetTrainType(),&run_type,&prio,&locotype)) // 设置默认电力牵引属性
	{
		ptrain->SetDefaultAttribByDefinedFlag(locotype);
	}
	for(int i=0;i<ptrain->GetTrainRecordCount();i++) 
	{
		CTG_TrainRecord *prec=ptrain->GetTrainRecord(i);
		CTG_TrainRecord *prev_rec=NULL;
		if(i>0)
			prev_rec=ptrain->GetTrainRecord(i-1);
		if(!prec)
			continue;
		// 设置默认按计划发车
		if(m_pConfig->IsDefaultTrigByPlan())  
			TG_SetMakeDepartRouteByPlan(prec->m_data);
					
		if(prec->IsTerminalRec())
		{
			prec->m_data.depart = prec->m_data.arrive;
			TG_SetNotAutoMakeDepartRoute(prec->m_data);
			GpcLogTrainMessage(0, 0, "列车%s 在车站%d终到, 发车进路不允许自触", ptrain->GetTrainID(), prec->m_data.station);
		}

		if(TG_IsFlagStopAbs(prec->m_data.flag)
			|| (prev_rec && TG_IsFlagStopAbs(prev_rec->m_data.flag)))
		{
			TG_SetNotAutoMakeArriveRoute(prec->m_data);
			TG_SetNotAutoMakeDepartRoute(prec->m_data);

			GpcLogTrainMessage(0, 0, "列车%s 在车站%d区间停车, 接发车进路不允许自触", ptrain->GetTrainID(), prec->m_data.station);
		}

		if(m_pConfig->IsFJKStation(prec->GetStationNo()))
			TG_SetFJKStationFlag(prec->m_data);
		else
			TG_SetNoFJKStationFlag(prec->m_data);
	}

	
	// 设置本钱点
	if(m_pAutoAdjust != NULL)
	{
		m_pAutoAdjust->SetTrainFJK(ptrain); 
		m_pAutoAdjust->SetTrainPassNonMainSide(ptrain);
	}

	ptrain->SetElecFlagByEngineInfo(); // 根据机车类型设置电力标志

	TG_SetLocoPosMid(&ptrain->GetTrainInfo()->m_data);
	return TRUE;
}

int  GetTrainIDDirection(CString strTrainId)
{
	// 去掉非数字
	CString strLastNumTrainId="0";
	int len = strTrainId.GetLength();
	for(int i= len-1;i>=0;i--)
	{
		char lastchar = strTrainId.GetAt(i);
		if(0x30 <= lastchar && lastchar <= 0x39)
		{
			strLastNumTrainId = lastchar;
			break;
		}
	}

	len = atoi(strLastNumTrainId);
	if(len%2 == 0)
		return UP_DIRECT;
	else
		return DOWN_DIRECT;
}

void  CTGAddTrainLogic::SetDefualtEntryAndExitByGpcdef(CTG_SingleTrain *ptrain, bool bOnlyEntry) 
{	
	if(NULL == ptrain)
		return;

	if(ptrain->GetTrainRecordCount()<=0)
		return;

	CTG_ScheduleInfo schd;
	if(m_pAutoAdjust != NULL)
		m_pAutoAdjust->GetScheduleInfo(schd);
	else
		return;
	
	// 接入车站
	CTG_TrainRecord *prec=ptrain->GetTrainRecord(0);
	if(prec==NULL)
		return;
	
	bool bTmp=false;
	if (GetTGFunctionConfig()->IsCheckEntryValueSetDefualtEntry())		
	{
		if(!prec->IsStartRec() && !IS_VALID_ENTRY((prec->GetEntry())))
			bTmp=true;
		else
			bTmp=false;
	}
	else
	{
		if(!prec->IsStartRec())
			bTmp=true;
		else
			bTmp=false;
	}

 	if(bTmp)
	{
		BYTE up_exit;
		BYTE down_entry;
		BYTE up_entry;
		BYTE down_exit;

		CString strArrivalTrainId=prec->GetArriveTID();
		BYTE ndirect = GetTrainIDDirection(strArrivalTrainId);

		if(gpcdef.GetInterfaceStation(schd.m_nSectionID, prec->GetStationNo(), prec->GetArriveTGIndex(), up_exit, down_entry, up_entry, down_exit))
		{
			if(ndirect == DOWN_DIRECT)
			{
				if(down_entry>0)
				{
					GpcLogTrainMessage(0, 0, "[接入口设置] %s, 按照GPCDEF中[INTERFACESTATION_EXIT_ENTRY]定义, 设置车站 %d 入口 %d=>%d", 
						strArrivalTrainId, prec->GetStationNo(), prec->m_data.entry, down_entry);
					prec->m_data.entry=down_entry;
				}
			}
			else
			{
				if(up_entry>0)
				{
					GpcLogTrainMessage(0, 0, "[接入口设置] %s, 按照GPCDEF中[INTERFACESTATION_EXIT_ENTRY]定义, 设置车站 %d 入口 %d=>%d", 
						strArrivalTrainId, prec->GetStationNo(), prec->m_data.entry, up_entry);
					prec->m_data.entry=up_entry;
				}
			}
		}
	}
	// 交出车站
	prec=ptrain->GetTrainRecord(ptrain->GetTrainRecordCount()-1);
	
	if(prec==NULL)
		return;
	
	bTmp=false;
	if (GetTGFunctionConfig()->IsCheckExitValueSetDefualtExit())		
	{
		if(!prec->IsTerminalRec() && !IS_VALID_ENTRY((prec->GetExit())))
			bTmp=true;
		else
			bTmp=false;
	}
	else
	{
		if(!prec->IsTerminalRec())
			bTmp=true;
		else
			bTmp=false;
	}
	
	if(bTmp && !bOnlyEntry)
	{
		BYTE up_exit;
		BYTE down_entry;
		BYTE up_entry;
		BYTE down_exit;

		CString strDepartTrainId=prec->GetDepartTID();
		BYTE ndirect = GetTrainIDDirection(strDepartTrainId);

		if(gpcdef.GetInterfaceStation(schd.m_nSectionID, prec->GetStationNo(), prec->GetDepartTGIndex(), up_exit, down_entry, up_entry, down_exit))
		{
			if(ndirect == DOWN_DIRECT)
			{
				if(down_exit>0)
				{
					GpcLogTrainMessage(0, 0, "[交出口设置] %s, 按照GPCDEF中[INTERFACESTATION_EXIT_ENTRY]定义, 设置车站 %d 出口 %d=>%d", 
						strDepartTrainId, prec->GetStationNo(), prec->m_data.exit, down_exit);
					prec->m_data.exit=down_exit;
				}
			}
			else
			{
				if(up_exit>0)
				{
					GpcLogTrainMessage(0, 0, "[交出口设置] %s, 按照GPCDEF中[INTERFACESTATION_EXIT_ENTRY]定义, 设置车站 %d 出口 %d=>%d", 
						strDepartTrainId, prec->GetStationNo(), prec->m_data.exit, up_exit);
					prec->m_data.exit=up_exit;
				}
			}
		}
	}

	return;
}

void  CTGAddTrainLogic::SetDefualtEntryAndExit(CTG_SingleTrain *ptrain) // 2013.5.23
{	
	if(NULL == ptrain)
		return;

	if(ptrain->GetTrainRecordCount()<=0)
		return;

	CTG_ScheduleInfo schd;
	if(m_pAutoAdjust != NULL)
		m_pAutoAdjust->GetScheduleInfo(schd);
	else
		return;
	
	BOOL down_train=ptrain->IsDownDirection();

	CTG_TrainRecord *prec=ptrain->GetTrainRecord(0);
	if(prec!=NULL && !prec->IsStartRec()) // 非始发，接入
	{
		if(!IS_VALID_ENTRY((prec->GetEntry()))) // 
		{
			CTG_TrainRecord *prec2=ptrain->GetTrainRecord(1);
			if(prec2)
				prec->m_data.entry=m_pBasicPara->GetAcceptEntry(schd.m_nSectionID,prec->GetStationNo(),prec2->GetStationNo(),prec->GetExit(), prec->GetArriveTGIndex(), prec->GetArriveTID());   
			if(!IS_VALID_ENTRY((prec->GetEntry())))
				prec->m_data.entry=m_pConfig->GetDefaultEntry(prec->GetStationNo(),down_train,TRUE);
			GpcLogTrainMessage(0, 0, "train %s 在车站 %d 错误接入口,自动推算为%d",ptrain->GetTrainID(),prec->m_data.station, prec->m_data.entry);
		}
	}

	prec=ptrain->GetTrainRecord(ptrain->GetTrainRecordCount()-1);
	if(prec!=NULL && !prec->IsTerminalRec())
	{
		if(!IS_VALID_ENTRY((prec->GetExit())))
		{
			CTG_TrainRecord *prec2;
			if(ptrain->GetTrainRecordCount()>1)
			{
				prec2=ptrain->GetTrainRecord(ptrain->GetTrainRecordCount()-2);
				if(prec2)
					prec->m_data.exit=m_pBasicPara->GetGiveoutExit(schd.m_nSectionID,prec->GetStationNo(),prec2->GetStationNo(),prec->GetEntry(),prec->GetDepartTGIndex(), prec->GetDepartTID());   
			}
			if(!IS_VALID_ENTRY((prec->GetExit())))
				prec->m_data.exit=m_pConfig->GetDefaultEntry(prec->GetStationNo(),down_train,FALSE);
			GpcLogTrainMessage(0, 0, "train %s 在车站 %d 错误交出口, 自动推算为%d",ptrain->GetTrainID(),prec->m_data.station,prec->m_data.exit);
		}
	}
	
	// 济南局,晏城--晏城北出入口设置配置
	SectionStaEntry cfg_section;
	if(GetTGFunctionConfig()->GetStaEntry(schd.m_nSectionID, cfg_section))
	{
		for(int i=0; i<cfg_section.count&&i<16; i++)
		{
			StaEntry cfg_entry=cfg_section.entry[i];
			if(cfg_entry.count>0 && ptrain->GetTrainRecordCount()>0)
			{
				bool bMatch=false;
				for (int rec_index = 0; rec_index<ptrain->GetTrainRecordCount(); rec_index++)
				{
					CTG_TrainRecord *prec=ptrain->GetTrainRecordByRecIndex(rec_index);
					if (prec->m_data.station == cfg_entry.station[0])
					{
						for(int m=0; m<cfg_entry.count&&m<8; m++)
						{
							CTG_TrainRecord *prec=ptrain->GetTrainRecordByRecIndex(rec_index+m);
							if(prec==NULL)
							{
								bMatch=false;// 不匹配
								break;
							}
							if(prec->m_data.station!=cfg_entry.station[m])
							{
								bMatch=false;// 不匹配
								break;
							}
						}
			
						if (m==cfg_entry.count)
							bMatch=true;
						
						break;
					}
				}

				if(bMatch)
				{
					CString str, tmp;
					for(int k=0; k<cfg_entry.count&&k<8; k++)
					{
						tmp.Format(" %d/%d/%d", cfg_entry.station[k], cfg_entry.entry[k], cfg_entry.exit[k]);
						str+=tmp;
					}
					GpcLogTrainMessage(0,5, "列车:%s 按照[SET_STATION_ENTRY]=%d,%d,%s设置配置车站出入口.", 
						ptrain->GetTrainID(), cfg_entry.section, cfg_entry.flag, str);
					
					for(int m=0; m<cfg_entry.count&&m<8; m++)
					{
						CTG_TrainRecord *prec=ptrain->GetTrainRecordByRecIndex(rec_index+m);
						if(prec==NULL)
						{
							break;
						}
						if(prec->m_data.station==cfg_entry.station[m])
						{
							if(cfg_entry.entry[m]>0)
							{
								prec->m_data.entry=cfg_entry.entry[m];
								GpcLogTrainMessage(0, 0, "train %s 在车站 %d 入口设置为 %d", ptrain->GetTrainID(),prec->m_data.station,prec->m_data.entry);
							}				
							if(m+1==cfg_entry.count)
							{
								CTG_TrainRecord *pnext=ptrain->GetTrainRecordByRecIndex(rec_index+m+1);
								if(pnext==NULL)
								{
									if(cfg_entry.exit[m]>0)
									{
										prec->m_data.exit=cfg_entry.exit[m];
										GpcLogTrainMessage(0, 0, "train %s 在车站 %d 出口设置为 %d", ptrain->GetTrainID(),prec->m_data.station,prec->m_data.exit);
									}
								}
							}
							else
							{
								if(cfg_entry.exit[m]>0)
								{
									prec->m_data.exit=cfg_entry.exit[m];
									GpcLogTrainMessage(0, 0, "train %s 在车站 %d 出口设置为 %d", ptrain->GetTrainID(),prec->m_data.station,prec->m_data.exit);
								}
							}
						}
					}
				
					// 匹配结束跳出
					break; 
				}
			}
		}
	}
	return;
}

// 处理徐州东--徐州东车场AB线问题
void  CTGAddTrainLogic::SetDefualtEntryAndExitBySide(CTG_SingleTrain *ptrain)
{
	if(NULL == ptrain)
		return;

	if(ptrain->GetTrainRecordCount()<=0)
		return;

	CTG_ScheduleInfo schd;
	if(m_pAutoAdjust != NULL)
		m_pAutoAdjust->GetScheduleInfo(schd);
	else
		return;
	
	int section=schd.m_nSectionID;
	if(!gpcdef.IsThisSectionHaveLineNoBySide(section))
		return;

	for(int i=0; i<ptrain->GetTrainRecordCount()-1; i++)
	{
		CTG_TrainRecord *prec=ptrain->GetTrainRecordByRecIndex(i);
		CTG_TrainRecord *pnext=ptrain->GetTrainRecordByRecIndex(i+1);
		if(pnext!=NULL)
		{
			int station1=prec->m_data.station;
			int station2=pnext->m_data.station;
			int side1=prec->m_data.depart_side;
			int side2=pnext->m_data.arrive_side;
			int exit=0, entry=0;
			if(gpcdef.GetStationExitBySide(section, station1, station2, side1, exit, entry))
			{
				prec->m_data.exit=exit;
				pnext->m_data.entry=entry;

				GpcLogTrainMessage(0, 0, "[按照股道 设置出入口]:%s 按照当前站:%d的股道:%d,设置本站:%d的exit:%d,下一站:%d entry:%d.", 
					ptrain->GetTrainID(), station1, side1, station1, exit, station2, entry);
			}
			else if(gpcdef.GetStationEntryBySide(section, station1, station2, side2, exit, entry))
			{
				prec->m_data.exit=exit;
				pnext->m_data.entry=entry;
				GpcLogTrainMessage(0, 0, "[按照股道 设置出入口]:%s 按照下一站:%d的股道:%d,设置本站:%d的exit:%d,下一站:%d entry:%d.", 
					ptrain->GetTrainID(), station2, side2, station1, exit, station2, entry);
			}
		}
	}

	return;
}

void  CTGAddTrainLogic::UseTDMSEntryExitByConfig(CTG_SingleTrain* ptrain, int section)
{
	if(NULL==ptrain)
		return;
	// 本台配置了使用TDMS出入口车站对,逐个车站匹配
	if(!GetTGFunctionConfig()->IsThisSectionHasTDMSLineStation(section))
		return;

	for (int i=0; i<ptrain->GetTrainRecordCount(); i++)
	{
		CTG_TrainRecord *prec      = ptrain->GetTrainRecordByRecIndex(i);
		CTG_TrainRecord *prec_next = ptrain->GetTrainRecordByRecIndex(i+1);
		if (NULL==prec && NULL==prec_next)
		{
			break;
		}
		// 首站
		if((i==0)&&(NULL!=prec)) 
		{
			if(GetTGFunctionConfig()->IsStationUseTDMSLine(section, 0, prec->m_data.station,255,prec->GetArriveTGIndex()))
			{
				GpcLogTrainMessage(0, 0, "按照[USE_TDMS_LINE_STATION]=%d,%d,%d,%d,%d; %s(%u)保留 %s TDMS入口:%d(替换CTC计算入口:%d)",
					section, 0, prec->m_data.station, 255, prec->GetArriveTGIndex(),
					ptrain->GetTrainID(), ptrain->GetTrainIndex(),
					gpcdef.GetStationName(prec->m_data.station), 
					prec->tdms_entry, prec->m_data.entry);
				
				prec->m_data.entry=prec->tdms_entry;
			}
		}
		// 尾站
		if((i==ptrain->GetTrainRecordCount()-1)&&(NULL!=prec)) 
		{
			if(GetTGFunctionConfig()->IsStationUseTDMSLine(section, prec->m_data.station, 0, prec->GetDepartTGIndex(), 255))
			{
				GpcLogTrainMessage(0, 0, "按照[USE_TDMS_LINE_STATION]=%d,%d,%d,%d,%d; %s(%u)保留 %s OPMS出口:%d(替换CTC计算出口:%d)",
					section, prec->m_data.station, 0, prec->GetDepartTGIndex(), 255,
					ptrain->GetTrainID(), ptrain->GetTrainIndex(),
					gpcdef.GetStationName(prec->m_data.station), 
					prec->tdms_exit, prec->m_data.exit);
				
				prec->m_data.exit=prec->tdms_exit;
			}
		}
		if((NULL!=prec_next) && (NULL!=prec))
		{
			if(GetTGFunctionConfig()->IsStationUseTDMSLine(section, prec->m_data.station, prec_next->m_data.station,prec->GetDepartTGIndex(),prec_next->GetArriveTGIndex()))
			{
				GpcLogTrainMessage(0, 0, "按照[USE_TDMS_LINE_STATION]=%d,%d,%d,%d,%d; %s(%u)保留 %s OPMS出口:%d(替换CTC计算出口:%d)",
					section, prec->m_data.station, prec_next->m_data.station,prec->GetDepartTGIndex(),prec_next->GetArriveTGIndex(),
					ptrain->GetTrainID(), ptrain->GetTrainIndex(),
					gpcdef.GetStationName(prec->m_data.station), 
					prec->tdms_exit, prec->m_data.exit);
				
				prec->m_data.exit=prec->tdms_exit;

				GpcLogTrainMessage(0, 0, "按照[USE_TDMS_LINE_STATION]=%d,%d,%d,%d,%d; %s(%u)保留 %s TDMS入口:%d(替换CTC计算入口:%d)",
					section, prec->m_data.station, prec_next->m_data.station,prec->GetDepartTGIndex(),prec_next->GetArriveTGIndex(),
					ptrain->GetTrainID(), ptrain->GetTrainIndex(),
					gpcdef.GetStationName(prec_next->m_data.station), 
					prec_next->tdms_entry, prec_next->m_data.entry);
				
				prec_next->m_data.entry=prec_next->tdms_entry;
			}
		}
	}	
}

BOOL  CTGAddTrainLogic::SetDefaultAttribForNewRecords(CTG_SingleTrain *ptrain,int fromindex)// 继续画线的默认设置
{
	if(fromindex<=0)  // 仅对继续画线情况
		return FALSE;
	CTG_TrainRecord *prev_rec=ptrain->GetTrainRecordByRecIndex(fromindex-1);
	if(!prev_rec)
		return FALSE;

	// 对于客车终到站,是否设置为非技术停点
	PassengerTrainEndSetNotTechStop(ptrain);
	PassengerTrainStartSetNotTechStop(ptrain);
	for(int i=fromindex;i<ptrain->GetTrainRecordCount();i++) 
	{
		CTG_TrainRecord *prec=ptrain->GetTrainRecord(i);
		if(!prec)
			continue;
		if(m_pConfig->IsDefaultTrigByPlan())  // 设置默认按计划发车
			TG_SetMakeDepartRouteByPlan(prec->m_data);  // 按计划发车
		
		if(TG_IsElecLocoFlag(prev_rec->m_data))  // 采用前一记录的电力标志
			TG_SetElecLocoFlag(prec->m_data);
		else
			TG_SetNoElecLocoFlag(prec->m_data);

		prec->m_data.rank=prev_rec->m_data.rank;  // 特运超限属性
		BYTE nLastSpace=prev_rec->GetDepartSpace();
		prec->SetArriveSpace(nLastSpace);
		prec->SetDepartSpace(nLastSpace);

		// 计算股道
		GpcLogTrainMessage(0, 0, "[继续画线 股道分配 计算]:SetDefaultAttribForNewRecords %s(%d), %s", ptrain->GetTrainID(),ptrain->GetTrainIndex(),gpcdef.GetStationName(prec->m_data.station));
		m_pAutoAdjust->CacuDefaultSide(ptrain, i); 

		if(m_pConfig->IsFJKStation(prec->GetStationNo()))
			TG_SetFJKStationFlag(prec->m_data);
		else
			TG_SetNoFJKStationFlag(prec->m_data);

		if (i<ptrain->GetTrainRecordCount()-1 && i != 0)
		{
			TG_RemoveFlagStatus(prec->m_data.flag);
		}
	}

	SetDefaultByBasicSchedule(ptrain, FALSE, fromindex-1, FREE_DRAW);

	// 2014.05.27 沈阳 区间时分类型
	bool enable=GetTGBasicParam()->IsEnableABSTimeType();
	if(enable)
	{
        BYTE nNewRunType=GetTrainAbsTimeType(ptrain);
		// 保存
		BYTE nSaveType=ptrain->GetTrainInfo()->m_data.run_type;
		ptrain->GetTrainInfo()->m_data.run_type=nNewRunType;
		RecacuTrainTimeBackward(ptrain,fromindex);
		// 恢复
		ptrain->GetTrainInfo()->m_data.run_type=nSaveType;
	}
	else
	{
		RecacuTrainTimeBackward(ptrain,fromindex);
	}
	return TRUE;
}

BOOL  CTGAddTrainLogic::SplitTrain(CTG_SingleTrain *ptrain,CTG_SingleTrain *panother_train,int rec_index, BYTE type,int first_entry,int first_exit,int second_entry,int second_exit)
{
	int i;
	int recount=ptrain->GetTrainRecordCount();
	i=rec_index;
	if(i>=recount || i<=0)
		return FALSE;

	CTG_TrainRecordList reclist=*ptrain->GetRecordList(); 
	ptrain->GetRecordList()->PurgeRecords();
	*panother_train=*ptrain;

	TG_SetFlagGiveout(reclist[i-1].m_data.flag);   // 前半段交出
	TG_SetFlagAccept(reclist[i].m_data.flag);  // 前半段接入

	for(int j=0;j<i;j++)
	{
		reclist[j].rec_index=j; 
		ptrain->GetRecordList()->AddRecord(reclist[j]); 
	}
	ptrain->SetTrainRecordCountToDefault();
	//ptrain->GetTrainInfo()->m_data.delete_flag=i;  
	for(int j=i;j<recount;j++)
	{
		reclist[j].rec_index=j-i;
		panother_train->GetRecordList()->AddRecord(reclist[j]); 
	}
	//panother_train->GetTrainInfo()->m_data.delete_flag=recount-i;
	panother_train->SetTrainRecordCountToDefault();
	SetDefaultAttrib(ptrain);

	SetDefualtEntryAndExitByGpcdef(panother_train);
	SetDefaultAttrib(panother_train);
	
	if(IS_VALID_ENTRY(first_entry))
	{
		CTG_TrainRecord *pfirst_rec = ptrain->GetTrainFirstRecord();
		if(pfirst_rec)
		{
			pfirst_rec->m_data.entry=first_entry; 
		}
	}
	if(IS_VALID_ENTRY(first_exit))
	{
		CTG_TrainRecord *plast_rec = ptrain->GetTrainLastRecord();
		if(plast_rec)
		{
			plast_rec->m_data.exit=first_exit; 
		}
	}
	if(IS_VALID_ENTRY(second_entry))
	{
		CTG_TrainRecord *pfirst_rec = panother_train->GetTrainFirstRecord();
		if(pfirst_rec)
		{
			pfirst_rec->m_data.entry=second_entry; 
		}
	}
	if(IS_VALID_ENTRY(second_exit))
	{
		CTG_TrainRecord *plast_rec = panother_train->GetTrainLastRecord();
		if(plast_rec)
		{
			plast_rec->m_data.exit=second_exit; 
		}
	}

	if(type==SHIFT)
	{
		int iSection=m_pScheduleInfo->m_nSectionID;
		UseTDMSEntryExitByConfig(panother_train, iSection);
		// 必须在计算股道之前,确定好出入口
		BOOL bUseOPMSShiftSchdSide = GetTGFunctionConfig()->IsEnableOPMSShiftSchdSide(iSection);
		if(bUseOPMSShiftSchdSide && !panother_train->IsPassengerTrain())
		{
			if(!GetTGFunctionConfig()->IsGoodTrainUseOPMSSide(panother_train->GetTrainType()))
				bUseOPMSShiftSchdSide=FALSE;
		}

		SetDefaultByBasicSchedule(panother_train, bUseOPMSShiftSchdSide, 0, SHIFT);
	}

	if(type==BASIC)
	{
		int iSection=m_pScheduleInfo->m_nSectionID;
		UseTDMSEntryExitByConfig(panother_train, iSection);
		// 必须在计算股道之前,确定好出入口
		BOOL bUseOPMSBasicSchdSide = GetTGFunctionConfig()->IsEnableOPMSBasicSchdSide(iSection);
		if(bUseOPMSBasicSchdSide && !panother_train->IsPassengerTrain())
		{
			if(!GetTGFunctionConfig()->IsGoodTrainUseOPMSSide(panother_train->GetTrainType()))
				bUseOPMSBasicSchdSide=FALSE;
		}
		SetDefaultByBasicSchedule(panother_train, bUseOPMSBasicSchdSide, 0, BASIC);
	}
	if(m_pAutoAdjust != NULL)
		m_pAutoAdjust->SetTrainPassNonMainSide(panother_train);
	return TRUE;
}

BOOL  CTGAddTrainLogic::GenTrain_ChangeRoute(CTG_SingleTrain &srctrain,int con_id,int route_id)
{
	CSingle_Passway passway_info;

	do 
	{
		if(!m_pBasicPara->m_cPasswayInfo.GetSinglePassway(con_id,route_id,passway_info))
			break;
		int rec_count=passway_info.m_vector_Passway_Sta_info.size();
		if (rec_count<=0)
			break;

		// 定位到第一个相同车站
		int train_rec_index, passway_rec_index;
		train_rec_index = passway_rec_index = -1;
		for(int i=0; i < srctrain.GetRecordList()->GetRecordCount(); i++)
		{
			const CTG_TrainRecord& rec = *srctrain.GetTrainRecord(i);
			for(int j=0;j<rec_count;j++)
			{
				const Passway_Sta &p_sta= passway_info.m_vector_Passway_Sta_info[j];
				if (rec.GetStationNo() == p_sta.Sta_id 
					&& rec.GetEntry()  == p_sta.Entry
					&& rec.GetExit()   == p_sta.Exit)
				{
					train_rec_index = i;
					passway_rec_index = j;
				}				
			}
			if (-1 != train_rec_index) break;
		}

		// 根据Passway修改
		CTG_TrainRecordList old_rec_list=*srctrain.GetRecordList();
		srctrain.GetRecordList()->PurgeRecords();
		for(int i=0; i<rec_count; i++)
		{
			CTG_TrainRecord rec;
			if (i == 0)
				rec = old_rec_list[0];
			else
			{
				rec.m_data.arrive = rec.m_data.depart = 0;
				rec.m_data.arrive_train_id[0] = 0;
				memset(rec.m_data.arrive_train_id, 0, sizeof(TRAIN_NO));
				rec.m_data.depart_train_id[0] = 0;
				memset(rec.m_data.depart_train_id, 0, sizeof(TRAIN_NO));
			}

			const Passway_Sta &p_sta= passway_info.m_vector_Passway_Sta_info[i];

			rec.m_data.station = p_sta.Sta_id;
			rec.m_data.entry = p_sta.Entry;
			rec.m_data.exit = p_sta.Exit;	

			rec.SetArriveTGIndex(p_sta.in_sta_index);
			rec.SetDepartTGIndex(p_sta.out_sta_index);

			rec.rec_index = i;
			srctrain.GetRecordList()->AddRecord(rec);
		}
		srctrain.SetTrainRecordCountToDefault();
		srctrain.SetTrainStartEndFlag();
		CTG_TrainRecordList& new_rec_list=*srctrain.GetRecordList();

		if (-1 == train_rec_index || -1 == passway_rec_index) // 完全不一致
		{
			RecacuTrainTime(&srctrain,TRUE);
			AdjustTrainRecordSide(&srctrain);
			SetDefaultAttribForNewRecords(&srctrain,1);

			for(int i=0; i<rec_count; i++)
			{
				CTG_TrainRecord &rec = new_rec_list[i];
				strcpy(rec.m_data.arrive_train_id,srctrain.GetTrainID());   
				strcpy(rec.m_data.depart_train_id,srctrain.GetTrainID());
			}
		}
		else
		{
			int first_index = passway_rec_index, last_index = -1;
			for(int i=train_rec_index, j=first_index; 
				i<old_rec_list.GetRecordCount() && j<new_rec_list.GetRecordCount();
				i++, j++)
			{
				if (old_rec_list[i].GetStationNo() != new_rec_list[j].GetStationNo()
					|| old_rec_list[i].GetEntry() != new_rec_list[j].GetEntry()
					|| old_rec_list[i].GetExit()  != new_rec_list[j].GetExit())
					break;

				long late_reason = new_rec_list[j].m_data.late_reason; // 保存新的站序
				new_rec_list[j].m_data = old_rec_list[i].m_data;
				new_rec_list[j].m_data.late_reason = late_reason;   //恢复新站序, added 太原 20101018 变更径路前先清空旧径路节点
				new_rec_list[j].rec_index = j;
				last_index = j;
			}

			// 向前推算时间和股道
			RecacuTrainTimeForward(&srctrain, first_index);
			AdjustTrainRecordSide(&srctrain, 0, first_index);

			// 向后推算时间和股道
			RecacuTrainTime(&srctrain,FALSE);
			AdjustTrainRecordSide(&srctrain, last_index+1);

			// 向前推算车次
			CTG_TrainRecord & first_rec = new_rec_list[first_index];
			if (!first_rec.m_data.arrive_train_id[0])
				memcpy(first_rec.m_data.arrive_train_id, first_rec.m_data.depart_train_id, sizeof(first_rec.m_data.arrive_train_id));

			for(int i=0; i<passway_rec_index; i++)
			{
				CTG_TrainRecord &rec = new_rec_list[i];
				strcpy(rec.m_data.arrive_train_id, first_rec.m_data.arrive_train_id); 
				strcpy(rec.m_data.depart_train_id, first_rec.m_data.arrive_train_id);
			}

			// 向后推算车次
			if (-1 != last_index)
			{
				CTG_TrainRecord & last_rec = new_rec_list[last_index];
				if (!last_rec.m_data.depart_train_id[0])
					memcpy(last_rec.m_data.depart_train_id, last_rec.m_data.arrive_train_id, sizeof(first_rec.m_data.depart_train_id));

				for(int i=last_index+1; i<new_rec_list.GetRecordCount(); i++)
				{
					CTG_TrainRecord &rec = new_rec_list[i];
					strcpy(rec.m_data.arrive_train_id,last_rec.m_data.depart_train_id);   
					strcpy(rec.m_data.depart_train_id,last_rec.m_data.depart_train_id);
				}
			}

			// 补齐后续信息
			SetDefaultAttribForNewRecords(&srctrain,1);
		}
		
		CTG_TrainRecordList &rec=*srctrain.GetRecordList();
		rec_count = srctrain.GetTrainRecordCount();
		for(int i=0; i<rec_count; i++)
		{
			if(rec[i].m_data.plan_arrive <= 0)
				rec[i].m_data.plan_arrive = rec[i].m_data.arrive;
			if(rec[i].m_data.plan_depart <= 0)
				rec[i].m_data.plan_depart = rec[i].m_data.depart;
		}

		return TRUE;
	}while(0);	

	return FALSE;
}

BOOL CTGAddTrainLogic::GenTrain_ShiftSchedule(CTG_SingleTrain *ptrain)
{
	for(int i=0; i<ptrain->GetTrainRecordCount(); i++)
	{
		CTG_TrainRecord *prec=ptrain->GetTrainRecordByRecIndex(i);
		if(prec)
		{
			GpcLogTrainMessage(0, 0, "日班计划列车股道 train=%s(%d), %s(%d):到达股道 %d 出发股道 %d 到达车次 %s 出发车次 %s",
				ptrain->GetTrainID(), ptrain->GetTrainIndex(),
				gpcdef.GetStationName(prec->m_data.station),  prec->m_data.station, 
				prec->m_data.arrive_side, prec->m_data.depart_side,
				prec->m_data.arrive_train_id, prec->m_data.depart_train_id);
		}
	}

	// 设置列车来源于shift
	ptrain->SetFromShift(TRUE);

	SetTrainInitFlag(ptrain);
	SetDefaultAttrib(ptrain);	
	if(m_pAutoAdjust != NULL)
		m_pAutoAdjust->SetTrainPassNonMainSide(ptrain);

	return TRUE;
}

ULONG GetPrefixAndDigitalTrainId(const char *train_id, char *prefix, char *tailfix)
{
	if ((train_id == NULL) || (strlen(train_id) == 0) || (strlen(train_id) > sizeof(TRAIN_NO)))
		return 0;	
	TRAIN_NO tmp;
	//char tmp[TRAIN_ID_LEN];
	int firstDigitIdx=0, lastDigitIdx=-1;
	int trainidlen = strlen(train_id);
	for(int i=trainidlen-1; i>=0; i--)
	{	
		if((lastDigitIdx<0) && iswdigit(train_id[i])){
			lastDigitIdx = i;
		}
		if((lastDigitIdx>-1) && (!iswdigit(train_id[i])))
		{
			firstDigitIdx = i + 1;
			break;
		}
	}
	if (lastDigitIdx<0)
	{
		if (prefix != NULL)
			strncpy(prefix, train_id, sizeof(TRAIN_NO)-1);
		return 0;
	}

	if (firstDigitIdx == 0)
		*prefix = '\0';
	else
	{
		strncpy(prefix, train_id, firstDigitIdx);
		prefix[firstDigitIdx] = '\0';
	}

	if(lastDigitIdx==trainidlen-1)
		*tailfix = '\0';
	else
	{
		strncpy(tailfix, train_id+lastDigitIdx+1, trainidlen-lastDigitIdx-1);
		tailfix[trainidlen-lastDigitIdx-1] = '\0';
	}

	memset(tmp,0,sizeof(tmp));
	strncpy(tmp, train_id+firstDigitIdx, lastDigitIdx-firstDigitIdx+1);
	return atoi(tmp);
}

bool  CTGAddTrainLogic::AutoChangeTrainID(CTG_SingleTrain *ptrain, int conid, int reverse)
{
	if(ptrain->IsPassengerTrain())
		return false;

	for(int s=0; s<ptrain->GetTrainRecordCount(); s++)
	{
		CTG_TrainRecord *rec = ptrain->GetTrainRecord(s);
		rec->rec_index=s; 
	}
	
	const ConsoleChangeId* pconsole=GetTGFunctionConfig()->GetConsoleChangeId(conid);
	if(pconsole==NULL)
		return false;

	int method=0, station=0;
	CString text="";
	for(int i=0; i<pconsole->info_count; i++)
	{
		CTG_TrainRecord *prec=ptrain->GetTrainRecordByStation(pconsole->info[i].station);
		if(prec==NULL)
			continue;

		// 车站
		if(pconsole->info[i].flag==1)
		{
			CTG_TrainRecord *pprec=ptrain->GetTrainRecordByRecIndex(prec->GetIndex()-1);
			if(pprec==NULL)
				continue;
			
			CTG_TrainRecord *pnext=ptrain->GetTrainRecordByRecIndex(prec->GetIndex()+1);
			if(pnext==NULL)
				continue;

			if(pprec->GetStationNo()!=pconsole->info[i].pre_station)
				continue;

			if(pnext->GetStationNo()!=pconsole->info[i].next_station)
				continue;
			
			station=pconsole->info[i].station;
			method=pconsole->info[i].method;
			text=pconsole->info[i].text;
		}
		if(pconsole->info[i].flag==2) // 没有下一站
		{
			CTG_TrainRecord *pnext=ptrain->GetTrainRecordByRecIndex(prec->GetIndex()+1);
			if(pnext!=NULL)
				continue;

			CTG_TrainRecord *pprec=ptrain->GetTrainRecordByRecIndex(prec->GetIndex()-1);
			if(pprec==NULL)
				continue;

			if(pprec->GetStationNo()!=pconsole->info[i].pre_station)
				continue;

			for(int c=0; c<pconsole->info[i].entry_count; c++)
			{
				if(prec->GetExit()==pconsole->info[i].entry[c])
				{
					station=pconsole->info[i].station;
					method=pconsole->info[i].method;
					text=pconsole->info[i].text;
					break;
				}
			}
			if(c>=pconsole->info[i].entry_count)
			{
				continue;
			}
		}
		if(pconsole->info[i].flag==3)  // 没有上一站
		{
			CTG_TrainRecord *pprec=ptrain->GetTrainRecordByRecIndex(prec->GetIndex()-1);
			if(pprec!=NULL)
				continue;

			CTG_TrainRecord *pnext=ptrain->GetTrainRecordByRecIndex(prec->GetIndex()+1);
			if(pnext==NULL)
				continue;
			
			if(pnext->GetStationNo()!=pconsole->info[i].next_station)
				continue;

			for(int c=0; c<pconsole->info[i].entry_count; c++)
			{
				if(prec->GetEntry()==pconsole->info[i].entry[c])
				{
					station=pconsole->info[i].station;
					method=pconsole->info[i].method;
					text=pconsole->info[i].text;
					break;
				}
			}
			if(c>=pconsole->info[i].entry_count)
			{
				continue;
			}
		}

		if(method==0 || station<=0)
			continue;

		prec=ptrain->GetTrainRecordByStation(station);
		if(prec==NULL)
			continue;

		char prefix[10], tailfix[10];
		memset(prefix, 0, 10);
		memset(tailfix, 0, 10);
		ULONG num=GetPrefixAndDigitalTrainId(prec->GetArriveTID(), prefix, tailfix);
		if(num==0)
			continue;

		if(reverse==1 && pconsole->info[i].flag==3)
		{
			CString str;
			str.Format("%s%d%s", prefix, num+method*(-1), tailfix);
			
			int begin=prec->GetIndex();
			for(int a=begin; a>=0; a--)
			{
				CTG_TrainRecord *prec=ptrain->GetTrainRecordByRecIndex(a);
				if(prec==NULL)
					continue;

				if(a==begin)
				{
					prec->SetStationText(text, STATION_TXT_INFO_TYPE_SHUAIGUA);
					prec->SetOpmsSationTextState("0");
				}
				if(a<begin)
				{
					strncpy(prec->m_data.depart_train_id, str, 9);
					prec->m_data.depart_train_id[9]=0;
				}
				
				strncpy(prec->m_data.arrive_train_id, str, 9);
				prec->m_data.arrive_train_id[9]=0;
			}
		}
		else
		{
			CString str;
			str.Format("%s%d%s", prefix, num+method, tailfix);
			
			int begin=prec->GetIndex();
			for(int a=begin; a<ptrain->GetTrainRecordCount(); a++)
			{
				CTG_TrainRecord *prec=ptrain->GetTrainRecordByRecIndex(a);
				if(prec==NULL)
					continue;

				if(a==begin)
				{
					prec->SetStationText(text, STATION_TXT_INFO_TYPE_SHUAIGUA);
					prec->SetOpmsSationTextState("0");
				}

				if(a>begin)
				{
					strncpy(prec->m_data.arrive_train_id, str, 9);
					prec->m_data.arrive_train_id[9]=0;
				}

				strncpy(prec->m_data.depart_train_id, str, 9);
				prec->m_data.depart_train_id[9]=0;
			}
		}
		
		method=0;
		station=0;
		text="";
	}

	return true;
}

bool  CTGAddTrainLogic::ChangeTrainElec(CTG_SingleTrain *ptrain, int conid)
{
	int nCount=ptrain->GetTrainRecordCount();
	if(nCount<=0)
		return false;
	
	int ret=-1;
	CTG_TrainRecord *prec=ptrain->GetTrainRecord(0);
	if(prec!=NULL)
	{
		ret=GetTGFunctionConfig()->SetElecTrain(conid, prec->m_data.station, -1, prec->m_data.entry);
	}

	prec=ptrain->GetTrainRecord(nCount-1);
	if(prec!=NULL && ret==-1)
	{
		ret=GetTGFunctionConfig()->SetElecTrain(conid, prec->m_data.station, prec->m_data.exit, -1);	
	}
	
	if(1 == ret)
	{
		for(int i=0; i<nCount; i++)
		{
			CTG_TrainRecord *prec=ptrain->GetTrainRecord(i);
			if(prec!=NULL)
			{
				prec->m_data.adjust_status |= 0x8000;
			}
		}

		GpcLogTrainMessage(0, 0, "根据tg_logic.cf中配置[DEFAULT_ELEC_SET], 修改列车 %s (%u)为 电力机车", ptrain->GetTrainID(), ptrain->GetShiftTrainIndex());
	}
	else if(0 == ret)
	{
		for(int i=0; i<nCount; i++)
		{
			CTG_TrainRecord *prec=ptrain->GetTrainRecord(i);
			if(prec!=NULL)
			{
				prec->m_data.adjust_status &= ~0x8000;
			}
		}

		GpcLogTrainMessage(0, 0, "根据tg_logic.cf中配置[DEFAULT_ELEC_SET], 修改列车 %s (%u)为 内燃机车", ptrain->GetTrainID(), ptrain->GetShiftTrainIndex());
	}
	else
		return false;

	return true;
}
BOOL  CTGAddTrainLogic::SetDefaultByBasicSchedule(CTG_SingleTrain *ptrain, BOOL bUseOPMSSide, int rec_index, SOURCE source)
{
	if(NULL==ptrain)
		return FALSE;

	for(int i=rec_index;i<ptrain->GetTrainRecordCount();i++)
	{
		CTG_TrainRecord *prec=ptrain->GetTrainRecord(i);
		if(prec==NULL)
			continue;

		prec->m_data.plan_arrive_side = prec->m_data.arrive_side;
		prec->m_data.plan_depart_side = prec->m_data.depart_side;
	}

	SetNotTechStop(ptrain, rec_index, source);
	
	if(!ptrain->IsPassengerTrain())
	{
		GpcLogTrainMessage(0, 0, "%s 是货物列车, 无需匹配基本图", ptrain->GetTrainID());
		return TRUE;
	}

	// 以下匹配基本图
	if(!m_pBasicSchedule)
		return FALSE;
	
	CTG_TrainRecord *pfirst_rec=ptrain->GetTrainRecord(0);
	if(!pfirst_rec)
		return FALSE;
	
	const CTG_SingleTrain *pbasic_train=NULL;
	pbasic_train = m_pBasicSchedule->FindTrainsByStationAndTrainNo(pfirst_rec->GetStationNo(), ptrain->GetTrainID());

	if(bUseOPMSSide)
	{
		GpcLogTrainMessage(0, 0, "%s, 使用OPMS股道, 请人工检查股道合法性", ptrain->GetTrainID());
	}
	else
	{
		if(GetTGFunctionConfig()->IsTrainSideUseBasicSide()) // 上海局既有线不论日班还是基本图,均不匹配基本图股道
			SetSideByBasicSchedule(ptrain, pbasic_train, rec_index);
	}

	SetNotTechStopByBasicSchedule(ptrain, pbasic_train, rec_index, source);
	return TRUE;
}

void  CTGAddTrainLogic::SetNotTechStop(CTG_SingleTrain *ptrain, int rec_index, SOURCE source)
{
	if(NULL==ptrain)
		return;
	
	if(CTgDocument::ResetGoodsTrainStopTech(ptrain))
		return;

	bool bInitTechStop=false;
	bool bCfgNotTechStop=false;
	if(source==BASIC)
		bCfgNotTechStop=GetTGFunctionConfig()->IsBasicStopSetNoTechStop();
	else if(source==SHIFT)
		bCfgNotTechStop=GetTGFunctionConfig()->IsShiftStopSetNoTechStop();
	else if(source==FREE_DRAW)
		bCfgNotTechStop=GetTGFunctionConfig()->IsFreedrawStopSetNoTechStop();
	else
		bCfgNotTechStop=false;

	for(int i=rec_index;i<ptrain->GetTrainRecordCount();i++)
	{
		CTG_TrainRecord *prec=ptrain->GetTrainRecord(i);
		if(prec==NULL)
			continue;
		
		if(bCfgNotTechStop)
		{
			if(!prec->IsPassRec())
			{
				TG_ClearTechStopTrain(prec->m_data);
				GpcLogTrainMessage(0, 0, "[技术停点 计算]: %s (%u), 根据tg_logic.cfg配置到开 设置 %s 为非技术停点", 
					ptrain->GetTrainID(), ptrain->GetShiftTrainIndex(), gpcdef.GetStationName(prec->m_data.station));
			}
			else
			{
				TG_SetTechStopTrain(prec->m_data);
			}
			bInitTechStop=true;
		}
		else if(ptrain->GetShiftTrainIndex()!=NULL_TRAININDEX)
		{
			if(prec->m_data.min_stop_time<=0) 
			{
				TG_SetTechStopTrain(prec->m_data);
				GpcLogTrainMessage(0, 0, "[技术停点 计算]: %s (%u),根据停车时间 %d 设置 %s 为技术停点", 
					ptrain->GetTrainID(), ptrain->GetShiftTrainIndex(), prec->m_data.min_stop_time, gpcdef.GetStationName(prec->m_data.station));
			}
			else
			{
				TG_ClearTechStopTrain(prec->m_data);
				GpcLogTrainMessage(0, 0, "[技术停点 计算]: %s (%u),根据停车时间 %d 设置 %s 为非技术停点", 
					ptrain->GetTrainID(), ptrain->GetShiftTrainIndex(), prec->m_data.min_stop_time, gpcdef.GetStationName(prec->m_data.station));
			}
			bInitTechStop=true;
		}

		if(!bInitTechStop)
		{
			if(prec->m_data.min_stop_time>0)
				TG_ClearTechStopTrain(prec->m_data);
			else
				TG_SetTechStopTrain(prec->m_data);
		}
	}
	// 对于客车终到站,是否设置为非技术停点
	PassengerTrainEndSetNotTechStop(ptrain);
	PassengerTrainStartSetNotTechStop(ptrain);
	return;
}

void  CTGAddTrainLogic::SetNotTechStopByBasicSchedule(CTG_SingleTrain *ptrain, const CTG_SingleTrain *pbasic_train, int rec_index, SOURCE source)
{
	if(NULL==pbasic_train || NULL==ptrain)
		return;
	
	for(int i=rec_index;i<ptrain->GetTrainRecordCount();i++)
	{
		CTG_TrainRecord *prec=ptrain->GetTrainRecord(i);
		CTG_TrainRecord *prec_next=ptrain->GetTrainRecord(i+1);
		if(prec==NULL)
			continue;
		
		const CTG_TrainRecord *prec_basic=pbasic_train->GetTrainRecordNoDelete_Const(prec->GetIndex());
		if(!prec_basic)  // 径路和基本图不一致,终止匹配
			break;
		if(prec->GetStationNo()!=prec_basic->GetStationNo()) // 径路和基本图不一致,终止匹配
			break;

		const CTG_TrainRecord *prec_basic_next=pbasic_train->GetTrainRecordNoDelete_Const(prec_basic->GetIndex()+1);
		if(prec_next && !prec_basic_next)  // 径路和基本图不一致,终止匹配
			break;
		if(prec_next && prec_basic_next && prec_next->GetStationNo()!=prec_basic_next->GetStationNo()) // 径路和基本图不一致,终止匹配
			break;
				
		// 匹配基本图技术停点
		if(TG_IsTechStopTrain(prec_basic->m_data))
		{
			;
		}
		else
		{
			if(GetTGFunctionConfig()->IsNewSetPreBasicTechStop() && (source==BASIC))
			{
				if(FALSE==prec->IsPassRec() || prec->IsStartRec() || prec->IsTerminalRec())
				{
					TG_ClearTechStopTrain(prec->m_data);
					GpcLogTrainMessage(0, 0, "[技术停点 匹配]: %s(%u), 基本图中是非技术停点, 设置 %s 为非技术停点", 
						ptrain->GetTrainID(),ptrain->GetShiftTrainIndex(),gpcdef.GetStationName(prec->m_data.station));
				}
			}
			else
			{
				TG_ClearTechStopTrain(prec->m_data);
				GpcLogTrainMessage(0, 0, "[技术停点 匹配]: %s(%u), 基本图中是非技术停点, 设置 %s 为非技术停点", 
					ptrain->GetTrainID(),ptrain->GetShiftTrainIndex(),gpcdef.GetStationName(prec->m_data.station));
			}
		}

		if (prec->IsPassRec() 
			&& !prec->IsStartRec() 
			&& !prec->IsTerminalRec())
		{
			TG_SetDefinePass(prec->m_data);
		}
		else
		{
			TG_ReSetDefinePass(prec->m_data);
		}
	}
	
	return;
}

void  CTGAddTrainLogic::SetSideByBasicSchedule(CTG_SingleTrain *ptrain, const CTG_SingleTrain *pbasic_train, int rec_index)
{
	if(pbasic_train == NULL)
	{
		GpcLogTrainMessage(0, 0, "[股道分配 匹配基本图]:基本图中找不车次 %s, 匹配失败!", ptrain->GetTrainID());
		return;
	}
	GpcLogTrainMessage(0, 0, "[股道分配 匹配基本图]:%s", ptrain->GetTrainID());

	// 匹配股道
	for(int i=rec_index;i<ptrain->GetTrainRecordCount();i++)
	{
		CTG_TrainRecord *prec=ptrain->GetTrainRecord(i);
		CTG_TrainRecord *prec_next=ptrain->GetTrainRecord(i+1);

		if(prec==NULL)
			continue;

		const CTG_TrainRecord *prec_basic=pbasic_train->GetTrainRecordNoDelete_Const(prec->GetIndex());
		if(!prec_basic)  // 径路和基本图不一致,终止匹配
			break;
		if(prec->GetStationNo()!=prec_basic->GetStationNo()) // 径路和基本图不一致,终止匹配
			break;

		const CTG_TrainRecord *prec_basic_next=pbasic_train->GetTrainRecordNoDelete_Const(prec_basic->GetIndex()+1);
		if(prec_next && !prec_basic_next)  // 径路和基本图不一致,终止匹配
			break;
		if(prec_next && prec_basic_next && prec_next->GetStationNo()!=prec_basic_next->GetStationNo()) // 径路和基本图不一致,终止匹配
			break;

		// 只有通过到开一致时才采用基本图股道
		if(prec->IsPassRec()== prec_basic->IsPassRec())
		{
			if(IS_VALID_SIDE(prec_basic->m_data.arrive_side))
			{
				prec->m_data.arrive_side=prec_basic->m_data.arrive_side;
				prec->m_data.plan_arrive_side = prec->m_data.arrive_side;

				GpcLogTrainMessage(0, 0, "[股道分配 匹配基本图]: %s 在 %s (entry %d exit %d)股道采用基本图 接车股道 %d", 
					ptrain->GetTrainID(),  gpcdef.GetStationName(prec->m_data.station), prec->m_data.entry, prec->m_data.exit,
					prec->m_data.arrive_side);
			}
			if(IS_VALID_SIDE(prec_basic->m_data.depart_side))
			{
				prec->m_data.depart_side=prec_basic->m_data.depart_side;
				prec->m_data.plan_depart_side = prec->m_data.depart_side;

				GpcLogTrainMessage(0, 0, "[股道分配 匹配基本图]: %s 在 %s (entry %d exit %d)股道采用基本图 发车股道 %d", 
					ptrain->GetTrainID(),  gpcdef.GetStationName(prec->m_data.station), prec->m_data.entry, prec->m_data.exit,
					prec->m_data.depart_side);
			}
		}
		else
		{
			GpcLogTrainMessage(0, 0, "[股道分配 匹配基本图]: %s 在 %s (entry %d exit %d)股道不采用基本图的(到开不同),保留计算股道 %d %d", 
				ptrain->GetTrainID(), gpcdef.GetStationName(prec->m_data.station), prec->m_data.entry, prec->m_data.exit,
				prec->m_data.arrive_side, prec->m_data.depart_side);
		}
	}
	return;
}

//广州局，旅客列车办客站默认锁定股道, hjz 20120215
void  CTGAddTrainLogic::SetSideLockPassengerTrainNotTechStop(CTG_SingleTrain *ptrain)
{
	if (TG_IsPassengerTrainType(ptrain->GetTrainType()))
	{
		for (int i= 0;i<ptrain->GetTrainRecordCount();i++)
		{
			CTG_TrainRecord *prec=ptrain->GetTrainRecord(i);
			if(!TG_IsTechStopTrain(prec->m_data))
			{
				TG_SetForceSide(prec->m_data);
			}
			else 
				TG_ReSetForceSide(prec->m_data);
		}
	}
}

// 对于客车首站,是否设置为非技术停点
void  CTGAddTrainLogic::PassengerTrainStartSetNotTechStop(CTG_SingleTrain *ptrain)
{
	if(NULL==ptrain)
		return;

	if(!ptrain->IsPassengerTrain())
		return;
	if(GetTGFunctionConfig()->PassengerTrainStartTechStop(ptrain->GetTrainInfo()->m_data.type))
		return;

	if(GetTGFunctionConfig()->IsPassengerTrainStartSetNotTechStop())
	{
		CTG_TrainRecord *prec=ptrain->GetTrainRecord(0);
		if(prec)
		{
			if(prec->IsStartRec())
			{
				TG_ClearTechStopTrain(prec->m_data);
				GpcLogTrainMessage(0, 0, "[非技术停点 默认]: %s, tg_logic.cfg中配置始发客车默认办理客运,设置非技术停点", ptrain->GetTrainID());
			}
		}
	}
}

// 对于客车终到站,是否设置为非技术停点
void  CTGAddTrainLogic::PassengerTrainEndSetNotTechStop(CTG_SingleTrain *ptrain)
{
		if(GetTGFunctionConfig()->IsPassengerTrainEndSetNotTechStop()&&ptrain->IsPassengerTrain() && !GetTGFunctionConfig()->PassengerTrainEndTechStop(ptrain->GetTrainInfo()->m_data.type))
		{
			CTG_TrainRecord *prec=ptrain->GetTrainRecord(ptrain->GetTrainRecordCount()-1);
			if(prec)
			{
				if(prec->IsTerminalRec())
				{
					TG_ClearTechStopTrain(prec->m_data);
					prec->m_data.exit=0;
					GpcLogTrainMessage(0, 0, "[非技术停点 默认]: %s, tg_logic.cfg中配置终到客车默认办理客运,设置非技术停点", ptrain->GetTrainID());
				}
			}
		}
}

BOOL CTGAddTrainLogic::SetValidSideByBasicSchedule(CTG_SingleTrain *ptrain)
{
	if(!m_pBasicSchedule)
		return FALSE;
	
	const CTG_SingleTrain *pbasic_train=NULL;
	CTG_TrainRecord *pfirst_rec=ptrain->GetTrainRecord(0);
	if(!pfirst_rec)
		return FALSE;

	pbasic_train = m_pBasicSchedule->FindTrainsByStationAndTrainNo(pfirst_rec->GetStationNo(), ptrain->GetTrainID());

	if(pbasic_train == NULL)
	{
		GpcLogTrainMessage(0, 0, "[匹配基本图 计算无效股道]: %s, 在基本图中找不到该车,无法匹配", ptrain->GetTrainID());
		return FALSE;
	}

	// 匹配股道
	for(int i=0;i<ptrain->GetTrainRecordCount();i++)
	{
		CTG_TrainRecord *prec=ptrain->GetTrainRecord(i);
		CTG_TrainRecord *prec_next=ptrain->GetTrainRecord(i+1);

		if(prec==NULL)
			continue;

		const CTG_TrainRecord *prec_basic=pbasic_train->GetTrainRecordNoDelete_Const(prec->GetIndex());
		if(!prec_basic)  // 径路和基本图不一致,终止匹配
			break;
		if(prec->GetStationNo()!=prec_basic->GetStationNo()) // 径路和基本图不一致,终止匹配
			break;

		const CTG_TrainRecord *prec_basic_next=pbasic_train->GetTrainRecordNoDelete_Const(prec_basic->GetIndex()+1);
		if(prec_next && !prec_basic_next)  // 径路和基本图不一致,终止匹配
			break;
		if(prec_next && prec_basic_next && prec_next->GetStationNo()!=prec_basic_next->GetStationNo()) // 径路和基本图不一致,终止匹配
			break;

		// 基本图、日班计划 股道由OPMS传过来,当股道没有时采用基本图股道
		if(!IS_VALID_SIDE(prec->m_data.arrive_side) || !IS_VALID_SIDE(prec->m_data.depart_side))
		{
			prec->m_data.arrive_side=prec_basic->m_data.arrive_side;
			prec->m_data.depart_side=prec_basic->m_data.depart_side;

			prec->m_data.plan_arrive_side = prec->m_data.arrive_side;
			prec->m_data.plan_depart_side = prec->m_data.depart_side;

			GpcLogTrainMessage(0, 0, "[匹配基本图 计算无效股道]: %s 在 %s (entry %d exit %d)股道采用基本图 接车股道 %d 发车股道 %d", 
				ptrain->GetTrainID(),  gpcdef.GetStationName(prec->m_data.station), prec->m_data.entry, prec->m_data.exit,
				prec->m_data.arrive_side, prec->m_data.depart_side);
		}
	}

	return TRUE;
}

BOOL CTGAddTrainLogic::SetValidSideByCacu(CTG_SingleTrain *ptrain)
{
	for(int i=0;i<ptrain->GetTrainRecordCount();i++)
	{
		CTG_TrainRecord *prec=ptrain->GetTrainRecord(i);

		if(!IS_VALID_SIDE(prec->m_data.arrive_side) || !IS_VALID_SIDE(prec->m_data.depart_side))
		{
			m_pAutoAdjust->CacuDefaultSide(ptrain, i); 
			CTG_TrainRecord *brec=ptrain->GetTrainRecord(i);
			GpcLogTrainMessage(0, 0, "[股道分配 计算无效股道]: 计算列车 %s(GID %u) 在 %s(%d) 站的股道(entry %d exit %d) arrive_side:%d depart_side:%d", 
				ptrain->GetTrainID(), ptrain->GetShiftTrainIndex(), gpcdef.GetStationName(brec->GetStationNo()), brec->GetStationNo(),
				brec->m_data.entry, brec->m_data.exit,
				brec->m_data.arrive_side, brec->m_data.depart_side);
		}
	}

	return TRUE;
}