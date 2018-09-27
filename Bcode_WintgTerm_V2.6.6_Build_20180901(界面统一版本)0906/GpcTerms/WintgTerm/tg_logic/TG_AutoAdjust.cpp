#include "stdafx.h"
#include "tg_work_schdule_holder.h"
#include "tg_defines.h"
#include "tg_autoadjust.h"
#include "gpcdef.h"
#include "sysshare.h"
extern BYTE  GetTrainAbsTimeType(const CTG_SingleTrain* ptrain);
extern void CaculateTrainSlowTime(CTG_SingleTrain *single);
const short  NEAR_TRAIN_TIME_RANGE=1800;

CTGAutoAdjustLogic::CTGAutoAdjustLogic(const CTGFunctionConfig  *pConfig,CTGBasicParam *pparam,CTG_TrainScheduleBase *pbasic_schd,CTG_TrainScheduleBase *pwork_schd)
{
	if(NULL!=pwork_schd)
	{
		CTG_ScheduleInfo info;
		pwork_schd->GetScheduleInfo(info);
		m_nAdjustSchdType=info.m_nType;
	}
	else
	{
		m_nAdjustSchdType=CTG_ScheduleInfo::SCHEDULE_TYPE_WORK;
	}

	m_pBasicSchedule=pbasic_schd;
	m_pWorkSchedule=pwork_schd;
	m_pBasicPara=pparam;
	m_pConfig=pConfig;

}
CTGAutoAdjustLogic::~CTGAutoAdjustLogic(void)
{
}

TIME CTGAutoAdjustLogic::CacuTrainDepartTime(const CTG_SingleTrain *psourcetrain,int rec_index, TIME ltime)
{
	const CTG_TrainRecord *ori_rec=psourcetrain->GetTrainRecordNoDelete_Const(rec_index);
	if(!ori_rec)
		return NULL_TIME;

	TIME default_return = ori_rec->GetDepartTime();
	if (ori_rec->IsStartRec() || ori_rec->IsTerminalRec())
		return default_return;

	if (NULL_TIME == ltime)
		ltime = TG_GetValidADTime(ori_rec->m_data);

	STATION_NO station = ori_rec->GetStationNo();
	ENTRY_ID   exit_id = ori_rec->GetExit();

	TIME near_time=ltime+NEAR_TRAIN_TIME_RANGE;

	short flag=0;

	ARRIVE_DEPART_STATUS status;

	RUN_TYPE type;


	CTG_TrainPtrGroup group;
	m_pWorkSchedule->FindTrains(group);
	for(int i=0;i<group.GetTrainCount();i++)
	{
		CTG_SingleTrain *ptrain=group.GetTrainPtr(i);
		if (ptrain->GetTrainIndex()==psourcetrain->GetTrainIndex())
			continue;

		CTG_TrainRecord *prec=ptrain->GetTrainRecord(ptrain->GetTrainRecordCount()-1);
		if(!prec)
			continue;

		if(prec->GetArriveTime()< ltime)   // 之前的列车忽略 
			continue;
		prec=ptrain->GetTrainRecord(0);
		if(!prec)
			continue;
		if(prec->GetArriveTime() > ltime && (prec->GetDepartTime() - ltime)> NEAR_TRAIN_TIME_RANGE) // 之后的列车忽略
			continue;

		if (ptrain->IsPassengerTrain())
			type = PASSANGER_TRAIN;
		else 
			type = FREIGHT_TRAIN;

		prec=ptrain->GetTrainRecordByStation(station);
		if(!prec)
			continue;
		if(prec->GetArriveTime() >= ltime && prec->GetEntry()== exit_id) 
		{ // diff dir
			if(prec->GetArriveTime()< near_time)
			{
				near_time = prec->GetArriveTime();
				flag = 1;
				status = prec->IsPassRec() ? STATION_PASS : STATION_ARRIVE;
			}
		} 
		else if (prec->GetDepartTime() > ltime && prec->GetExit() == exit_id) 
		{ // same dir
			if (prec->GetDepartTime()< near_time)
			{
				near_time = prec->GetDepartTime();
				flag = 0;
				status = prec->IsPassRec() ? STATION_PASS : STATION_DEPART;
			}
		}
	}

	if (near_time >= ltime+NEAR_TRAIN_TIME_RANGE || flag == 0)

		//next train too far,or nearest train is same dir

		return default_return;

	//get time gap

	ARRIVE_DEPART_STATUS ori_status=STATION_DEPART;
	RUN_TYPE ori_type=PASSANGER_TRAIN;
	if (ori_rec->IsPassRec())  status = STATION_PASS;
	if (!psourcetrain->IsPassengerTrain()) ori_type = FREIGHT_TRAIN;   
	return near_time + m_pBasicPara->GetTrainGapTime(station, MEET_TRAIN_GAP, ori_status, status, ori_type, type);
}

BOOL  CTGAutoAdjustLogic::CacuDefaultSide(CTG_SingleTrain *ptrain)
{
	int rec_count=ptrain->GetTrainRecordCount();
	for(int i=0;i<rec_count;i++)
	{
		CacuDefaultSide(ptrain,i);
	}
	return TRUE;
}

BOOL  CTGAutoAdjustLogic::CacuDefaultSide(CTG_SingleTrain *ptrain,int rec_index)
{
	CTG_TrainRecord *pRec=ptrain->GetTrainRecord(rec_index);
	if(!pRec)
		return FALSE;
	TG_TrainRecord  &ori_rec=pRec->m_data;
	TG_TrainInfo &traininfo=ptrain->GetTrainInfo()->m_data;
	
	// side cannot be changed
	if (TG_IsForceSide(ori_rec))
	{
		GpcLogTrainMessage(0, 0, "[股道分配 计算]:CacuDefaultSide %s(%d) 在车站%d(entry %d exit %d)股道不可改变", 
				ptrain->GetTrainID(),ptrain->GetTrainIndex(),ori_rec.station, ori_rec.entry, ori_rec.exit);
		return FALSE;
	}
	

	do 
	{
		STATION_NO station = ori_rec.station;
		if (!m_pBasicPara->VerifyStationNumber(station)) 
		{
			GpcLogTrainMessage(0, 0, "[股道分配 计算]:CacuDefaultSide %s(%d) 车站 %d 没有配置", ptrain->GetTrainID(),ptrain->GetTrainIndex(),station);
			break;
		}

		BOOL if_pass = FALSE;
		if (pRec->IsPassRec() && !pRec->IsStartRec() && !pRec->IsTerminalRec()) //本车站通过，既不是始发站也不是终到站
		{
			if_pass = TRUE;
			SIDE_ID side = m_pBasicPara->GetMainSide(ori_rec);
			if (NULL_SIDE == side) 
			{
				break;
			}
			else if (m_pConfig->IfChoseMainSideWhilePass())
			{
				// if train passed, always find the main side, even which is occupied, force the lirc to trig main side
				if (ori_rec.arrive_side == side && ori_rec.depart_side == side)
				{
					return FALSE;
				}
				else
				{
					ori_rec.arrive_side = side;
					ori_rec.depart_side = side;
					return TRUE;
				}
			}
			else
			{
				ori_rec.arrive_side = side;
				ori_rec.depart_side = side;
			}
		}

		// reset main side
		SIDE_ID main_side = m_pBasicPara->GetMainSide(ori_rec);
		
		// find one stop side
		SIDE_ID order_side[MAX_SIDE_COUNT];
		short selected_side_count = MAX_SIDE_COUNT;
		if (!m_pBasicPara->GetStopOrderSide(traininfo, ori_rec, order_side, &selected_side_count))
		{
			GpcLogTrainMessage(0, 0, "[股道分配 计算]:CacuDefaultSide %s(%d) 在车站%d(entry %d exit %d)的StationSide.def中没有配置可选股道", 
				ptrain->GetTrainID(),ptrain->GetTrainIndex(),station, ori_rec.entry, ori_rec.exit);
			break;
		}
		if(selected_side_count<=0)
		{
			GpcLogTrainMessage(0, 0, "[股道分配 计算]:CacuDefaultSide %s(%d) 在车站%d(entry %d exit %d)的StationSide.def中配置可选股道为0", 
				ptrain->GetTrainID(),ptrain->GetTrainIndex(),station, ori_rec.entry, ori_rec.exit);
			break;
		}
		short if_force_stop_main_side = FALSE;
		if (1 == selected_side_count && order_side[0] == main_side)
			if_force_stop_main_side = TRUE;

		
		ori_rec.arrive_side = order_side[0];
		ori_rec.depart_side = order_side[0];
		

		// cannot find side to use, force to use the selected side rather than the main side
		//   otherwise, if lirc trigs the main side, which maybe pause all trains passby it
		if ((main_side == ori_rec.arrive_side && !if_force_stop_main_side)
			|| !IS_VALID_SIDE(ori_rec.arrive_side))
		{
			if (!m_pConfig->IfdChoseSideWhileAllFull())
			{
				ori_rec.arrive_side = 255;
				ori_rec.depart_side = 255;

				GpcLogTrainMessage(0, 0, "[股道分配 计算]:CacuDefaultSide %s(%d) 在车站%d(entry %d exit %d)股道占用已满,不可分配", 
				ptrain->GetTrainID(),ptrain->GetTrainIndex(),station, ori_rec.entry, ori_rec.exit);
			}
		}

		return TRUE;

	} while(0);

	// failed to get one side, reset to NULL for safe
	ori_rec.arrive_side = 255;
	ori_rec.depart_side = 255;
	return FALSE;
}

BOOL  CTGAutoAdjustLogic::OnlyUseFirstStopSideForNanchang(CTG_SingleTrain *ptrain, int rec_index)
{
	CTG_TrainRecord *pRec=ptrain->GetTrainRecord(rec_index);
	if(!pRec)
		return FALSE;
	TG_TrainRecord  &ori_rec=pRec->m_data;
	TG_TrainInfo &traininfo=ptrain->GetTrainInfo()->m_data;
	
	// side cannot be changed
	if(IS_VALID_SIDE(pRec->m_data.arrive_side) || IS_VALID_SIDE(pRec->m_data.depart_side))
	{
		if (TG_IsForceSide(ori_rec))
		{
			GpcLogTrainMessage(0, 0, "[股道分配 计算]: %s(%d) 在车站%d(entry %d exit %d)该股道不可改变", 
					ptrain->GetTrainID(),ptrain->GetTrainIndex(),ori_rec.station, ori_rec.entry, ori_rec.exit);
			return FALSE;
		}
	}
	// if route is making or occuped, we do not change side automaticaly
	CStringArray list;
	if (TG_IsArriveRouteDoing(ori_rec))
	{
		list.Add("接车进路正在触发");
	}
	if (TG_IsArriveRouteOccupy(ori_rec))
	{
		list.Add("接车进路占用");
	}
	if (TG_IsArriveRouteOk(ori_rec))
	{
		list.Add("接车进路触发完成");
	}
	if (TG_IsDepartRouteDoing(ori_rec))
	{
		list.Add("发车进路正在触发");
	}
	if (TG_IsDepartRouteOccupy(ori_rec))
	{
		list.Add("发车进路占用");
	}
	if (TG_IsDepartRouteOk(ori_rec))
	{
		list.Add("发车进路触发完成");
	}

	if(list.GetCount()>0)
	{
		GpcLogTrainMessage(0, 0, "[股道分配 计算]: %s(%d) 在车站%d(entry %d exit %d)该股道不可改变,原因如下:", 
				ptrain->GetTrainID(),ptrain->GetTrainIndex(),ori_rec.station, ori_rec.entry, ori_rec.exit);

		for(int n=0; n<list.GetCount(); n++)
		{
			GpcLogTrainMessage(0, 0, "%s", list.GetAt(n));
		}
		return FALSE;
	}

	if (TG_IsTrainIntoSide(ori_rec))
	{
		GpcLogTrainMessage(0, 0, "[股道分配 计算]: %s(%d) 在车站%d(entry %d exit %d)该股道不可改变,列车已经进站", 
				ptrain->GetTrainID(),ptrain->GetTrainIndex(),ori_rec.station, ori_rec.entry, ori_rec.exit);
		return FALSE;
	}
	
	do 
	{
		STATION_NO station = ori_rec.station;
		if (!m_pBasicPara->VerifyStationNumber(station)) 
		{
			GpcLogTrainMessage(0, 0, "[股道分配 计算]:OnlyUseFirstStopSideForNanchang %s(%d) 车站 %d 没有配置", ptrain->GetTrainID(),ptrain->GetTrainIndex(), station);
			break;
		}

		BOOL if_pass = FALSE;
		if (pRec->IsPassRec() && !pRec->IsStartRec() && !pRec->IsTerminalRec()) //本车站通过，既不是始发站也不是终到站
		{
			if_pass = TRUE;
			SIDE_ID side = m_pBasicPara->GetMainSide(ori_rec);
			if (NULL_SIDE == side) 
			{
				break;
			}
			else if (m_pConfig->IfChoseMainSideWhilePass())
			{
				// if train passed, always find the main side, even which is occupied, force the lirc to trig main side
				if (ori_rec.arrive_side == side && ori_rec.depart_side == side)
				{
					return FALSE;
				}
				else
				{
					ori_rec.arrive_side = side;
					ori_rec.depart_side = side;
					return TRUE;
				}
			}
			else
			{
				ori_rec.arrive_side = side;
				ori_rec.depart_side = side;
			}
		}

		// reset main side
		SIDE_ID main_side = m_pBasicPara->GetMainSide(ori_rec);
		
		// find one stop side
		SIDE_ID order_side[MAX_SIDE_COUNT];
		short selected_side_count = MAX_SIDE_COUNT;
		if (!m_pBasicPara->GetStopOrderSide(traininfo, ori_rec, order_side, &selected_side_count))
		{
			GpcLogTrainMessage(0, 0, "[股道分配 计算]:CacuDefaultSide %s(%d) 在车站%d(entry %d exit %d)的StationSide.def中没有配置可选股道", 
				ptrain->GetTrainID(),ptrain->GetTrainIndex(),station, ori_rec.entry, ori_rec.exit);
			break;
		}
		if(selected_side_count<=0)
		{
			GpcLogTrainMessage(0, 0, "[股道分配 计算]:CacuDefaultSide %s(%d) 在车站%d(entry %d exit %d)的StationSide.def中配置可选股道为0", 
				ptrain->GetTrainID(),ptrain->GetTrainIndex(),station, ori_rec.entry, ori_rec.exit);
			break;
		}
		short if_force_stop_main_side = FALSE;
		if (1 == selected_side_count && order_side[0] == main_side)
			if_force_stop_main_side = TRUE;

		
		ori_rec.arrive_side = order_side[0];
		ori_rec.depart_side = order_side[0];
		

		// cannot find side to use, force to use the selected side rather than the main side
		//   otherwise, if lirc trigs the main side, which maybe pause all trains passby it
		if ((main_side == ori_rec.arrive_side && !if_force_stop_main_side)
			|| !IS_VALID_SIDE(ori_rec.arrive_side))
		{
			if (!m_pConfig->IfdChoseSideWhileAllFull())
			{
				ori_rec.arrive_side = 255;
				ori_rec.depart_side = 255;

				GpcLogTrainMessage(0, 0, "[股道分配 计算]:CacuDefaultSide %s(%d) 在车站%d(entry %d exit %d)股道占用已满,不可分配", 
				ptrain->GetTrainID(),ptrain->GetTrainIndex(),station, ori_rec.entry, ori_rec.exit);
			}
		}

		return TRUE;

	} while(0);

	// failed to get one side, reset to NULL for safe
	ori_rec.arrive_side = 255;
	ori_rec.depart_side = 255;
	return FALSE;
}

BOOL  CTGAutoAdjustLogic::ChangeSideForStopOrPass(CTG_SingleTrain *ptrain,int rec_index,int enable_passenger_side_change,int only_pass_main_side,CTG_ScheduleInfo::SCHEDULE_TYPE type)
{
	CTG_TrainRecord *pRec=ptrain->GetTrainRecord(rec_index);
	if(!pRec)
		return FALSE;
	TG_TrainRecord  &ori_rec=pRec->m_data;
	TG_TrainInfo &traininfo=ptrain->GetTrainInfo()->m_data;
	TIME stime,etime;
	if(enable_passenger_side_change<0)
		enable_passenger_side_change=GetTGFunctionConfig()->IsChangeSideWhileStopPass(traininfo.type)?1:0;
	if(only_pass_main_side<0)
		only_pass_main_side=GetTGFunctionConfig()->IfChoseMainSideWhilePass()?1:0;
	
	// side cannot be changed
	if(IS_VALID_SIDE(pRec->m_data.arrive_side) || IS_VALID_SIDE(pRec->m_data.depart_side))
	{
		if (TG_IsForceSide(ori_rec))
		{
			GpcLogTrainMessage(0, 0, "[股道分配 计算]: %s(%d) 在车站%d(entry %d exit %d)该股道不可改变", 
					ptrain->GetTrainID(),ptrain->GetTrainIndex(),ori_rec.station, ori_rec.entry, ori_rec.exit);
			return FALSE;
		}
		if(enable_passenger_side_change!=1 && ptrain->IsPassengerTrain())
		{
			GpcLogTrainMessage(0, 0, "[股道分配 计算]: %s(%d) 在车站%d(entry %d exit %d)该股道不可改变,因为未被允许", 
					ptrain->GetTrainID(),ptrain->GetTrainIndex(), ori_rec.station, ori_rec.entry, ori_rec.exit);
			return FALSE;
		}
	}
	// if route is making or occuped, we do not change side automaticaly
	CStringArray list;
	if (TG_IsArriveRouteDoing(ori_rec))
	{
		list.Add("接车进路正在触发");
	}
	if (TG_IsArriveRouteOccupy(ori_rec))
	{
		list.Add("接车进路占用");
	}
	if (TG_IsArriveRouteOk(ori_rec))
	{
		list.Add("接车进路触发完成");
	}
	if (TG_IsDepartRouteDoing(ori_rec))
	{
		list.Add("发车进路正在触发");
	}
	if (TG_IsDepartRouteOccupy(ori_rec))
	{
		list.Add("发车进路占用");
	}
	if (TG_IsDepartRouteOk(ori_rec))
	{
		list.Add("发车进路触发完成");
	}

	if(list.GetCount()>0)
	{
		GpcLogTrainMessage(0, 0, "[股道分配 计算]: %s(%d) 在车站%d(entry %d exit %d)该股道不可改变,原因如下:", 
				ptrain->GetTrainID(),ptrain->GetTrainIndex(),ori_rec.station, ori_rec.entry, ori_rec.exit);

		for(int n=0; n<list.GetCount(); n++)
		{
			GpcLogTrainMessage(0, 0, "%s", list.GetAt(n));
		}
		return FALSE;
	}
	
	if (TG_IsTrainIntoSide(ori_rec))
	{
		GpcLogTrainMessage(0, 0, "[股道分配 计算]: %s(%d) 在车站%d(entry %d exit %d)该股道不可改变,列车已经进站", 
				ptrain->GetTrainID(),ptrain->GetTrainIndex(),ori_rec.station, ori_rec.entry, ori_rec.exit);
		return FALSE;
	}
	
	do 
	{
		STATION_NO station = ori_rec.station;
		if (!m_pBasicPara->VerifyStationNumber(station)) 
		{
			GpcLogTrainMessage(0, 0, "[股道分配 计算]: %s(%d) 车站%d(entry %d exit %d), 在是个非法车站", 
				ptrain->GetTrainID(),ptrain->GetTrainIndex(),ori_rec.station, ori_rec.entry, ori_rec.exit);
			break;
		}

		stime = ori_rec.arrive;
		etime = ori_rec.depart;

		if (NULL_TIME == stime) stime = etime;
		if (NULL_TIME == etime) etime = stime;

		BOOL if_pass = FALSE;
		if (stime == etime && !TG_IsFlagTerminal(ori_rec.flag) && !TG_IsFlagStart(ori_rec.flag)) //本车站通过，既不是始发站也不是终到站
		{
			if_pass = TRUE;
			SIDE_ID side = m_pBasicPara->GetMainSide(ori_rec);
			if (NULL_SIDE == side) 
			{
				GpcLogTrainMessage(0, 0, "[股道分配 计算]: %s(%d) 车站%d(entry %d exit %d), 在StationSide.def中找不到正线股道", 
				ptrain->GetTrainID(),ptrain->GetTrainIndex(),ori_rec.station, ori_rec.entry, ori_rec.exit);
				break;
			}
			else if (only_pass_main_side)
			{
				// if train passed, always find the main side, even which is occupied, force the lirc to trig main side
				if (ori_rec.arrive_side == side && ori_rec.depart_side == side)
				{
					return FALSE;
				}
				else
				{
					ori_rec.arrive_side = side;
					ori_rec.depart_side = side;
					return TRUE;
				}
			}
			else
			{
				ori_rec.arrive_side = side;
				ori_rec.depart_side = side;
			}
		}

		// use reg_side to record the max time of all side utility less than needed (etime) for every side
		TIME reg_time[MAX_SIDE_COUNT+1];
		memset(&reg_time, 0x00, sizeof(reg_time));
		CTG_TrainPtrGroup group;
		m_pWorkSchedule->FindTrains(group);
		for(int train_iter=0;train_iter<group.GetTrainCount();train_iter++)
		{
			CTG_SingleTrain *iter_train=group.GetTrainPtr(train_iter);
			if (ptrain->GetTrainIndex() == iter_train->GetTrainIndex())
			{
				continue;
			}
			
			if(type==CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT) // 20140929 修改日班计划 股道避让
 			{
				if (ptrain->GetShiftTrainIndex() == iter_train->GetShiftTrainIndex() && ptrain->GetShiftTrainIndex()!=NULL_TRAININDEX)
					continue;
			}

			short nRecCount = iter_train->GetTrainRecordCount();
			for (int i=0; i<nRecCount; i++)
			{
				const TG_TrainRecord & rec = iter_train->GetTrainRecord(i)->m_data;
				if (rec.station != station)
					continue;

				TIME ltime = TG_GetValidADTime(rec);
				if (NULL_TIME == ltime)
					continue;

				if (ltime > etime) // donot care about later train
					continue;

				if (rec.arrive_side <0 || rec.arrive_side >= sizeof(reg_time)/sizeof(reg_time[0]))
					continue;

				if (rec.depart_side <0 || rec.depart_side >= sizeof(reg_time)/sizeof(reg_time[0]))
					continue;

				ltime = rec.depart;
				if (NULL_TIME == ltime) ltime = rec.arrive;
				if (reg_time[rec.arrive_side] < ltime)
					reg_time[rec.arrive_side] = ltime;

				break;
			}
		} 

		CTG_ObjectPtrGroup mark_group;
		m_pWorkSchedule->FindMarks(mark_group);
		for(int mark_iter=0;mark_iter<mark_group.GetCount();mark_iter++)
		{
			CTG_SingleMark *iter_mark=(CTG_SingleMark *)mark_group.GetPtr(mark_iter);
			if(!iter_mark) continue;
			const TG_ScheduleBlock &mark = iter_mark->m_data;
			if (TG_SIDE_INHIBIT != mark.type && TG_SIDE_TIANCHUANG != mark.type && TG_SIDE_OCCUPY != mark.type)
			{
				continue;
			}

			if (mark.station != station)
			{
				continue;
			}

			if (mark.start_time > etime)
			{
				continue;
			}

			for(int j=0; j<mark.ele_count; j++)
			{
				SIDE_ID side = (SIDE_ID) mark.ele_number[j];
				if (!m_pBasicPara->VerifyStationSide(station, side))
					continue;

				if (reg_time[side] < mark.end_time)
					reg_time[side] = mark.end_time;			
			}
		}

		// do not need to change
		if (TG_IsFlagStart(ori_rec.flag) && IS_VALID_SIDE(ori_rec.depart_side))
		{
			if (m_pBasicPara->IsStopSide(ori_rec))
				if (reg_time[ori_rec.depart_side] < stime) 
					return FALSE;
		}
		else if (TG_IsFlagTerminal(ori_rec.flag) && IS_VALID_SIDE(ori_rec.arrive_side)) 
		{
			if (m_pBasicPara->IsStopSide(ori_rec))
				if (reg_time[ori_rec.arrive_side] < stime) 
					return FALSE;
		}
		else if (ori_rec.arrive_side == ori_rec.depart_side && IS_VALID_SIDE(ori_rec.depart_side)) 
		{
			if (if_pass)
			{
				if (m_pBasicPara->IsMainSide(ori_rec))
					if (reg_time[ori_rec.depart_side] < stime) 
						return FALSE;
                if(only_pass_main_side) // 2011.01.01 在某种情况下,比如封锁股道后的调整,应该允许侧线通过
					break;
			}
			else
			{
				if (m_pBasicPara->IsStopSide(ori_rec))
					if (reg_time[ori_rec.depart_side] < stime) 
						return FALSE;
			}
		}
		else if (IS_VALID_SIDE(ori_rec.arrive_side) && IS_VALID_SIDE(ori_rec.depart_side)) 
		{
			if (m_pBasicPara->IsStopSide(ori_rec))
				if ((reg_time[ori_rec.arrive_side] < stime && reg_time[ori_rec.arrive_side] < stime) 
					&& (reg_time[ori_rec.depart_side] < stime && reg_time[ori_rec.depart_side] < stime))
					return FALSE;
		}

		// reset main side
		SIDE_ID main_side = m_pBasicPara->GetMainSide(ori_rec);
		ori_rec.arrive_side = main_side;
		ori_rec.depart_side = main_side;

		// find one stop side
		SIDE_ID order_side[MAX_SIDE_COUNT];
		short selected_side_count = MAX_SIDE_COUNT;
		if (!m_pBasicPara->GetStopOrderSide(traininfo, ori_rec, order_side, &selected_side_count))
		{
			if(GetTGFunctionConfig()->IsTrainPassToStopUseMainSide())
			{
				GpcLogTrainMessage(0, 0, "[股道分配 计算]: %s(%d) 车站%d(entry %d exit %d), 在StationSide.def中没有配置可选股道, 使用正线股道", 
					ptrain->GetTrainID(),ptrain->GetTrainIndex(), station, ori_rec.entry, ori_rec.exit);
				return TRUE;
			}
			else
			{
				GpcLogTrainMessage(0, 0, "[股道分配 计算]: %s(%d) 车站%d(entry %d exit %d), ChangeSideForStopOrPass 在StationSide.def中没有配置可选股道", 
					ptrain->GetTrainID(),ptrain->GetTrainIndex(), station, ori_rec.entry, ori_rec.exit);
			}
			break;
		}

		if(selected_side_count<=0)
		{
			if(GetTGFunctionConfig()->IsTrainPassToStopUseMainSide())
			{
				GpcLogTrainMessage(0, 0, "[股道分配 计算]: %s(%d) 车站%d(entry %d exit %d), 在StationSide.def中配置可选股道为0个, 使用正线股道", 
					ptrain->GetTrainID(),ptrain->GetTrainIndex(), station, ori_rec.entry, ori_rec.exit);
				return TRUE;
			}
			else
			{
				GpcLogTrainMessage(0, 0, "[股道分配 计算]: %s(%d) 车站%d(entry %d exit %d), 在StationSide.def中配置可选股道为0个", 
					ptrain->GetTrainID(),ptrain->GetTrainIndex(), station, ori_rec.entry, ori_rec.exit);
			}
		}
		short if_force_stop_main_side = FALSE;
		if (1 == selected_side_count && order_side[0] == main_side)
			if_force_stop_main_side = TRUE;

		for(int i=0; i<selected_side_count; i++)
		{
			SIDE_ID side = order_side[i];
			if (side < 0 || side >= sizeof(reg_time)/sizeof(reg_time[0]))
			{
				continue;
			}

			if (reg_time[side] < stime) 
			{
				ori_rec.arrive_side = side; // 重点测试
				ori_rec.depart_side = side;
				break;
			}
		}

		// cannot find side to use, force to use the selected side rather than the main side
		//   otherwise, if lirc trigs the main side, which maybe pause all trains passby it
		if ((main_side == ori_rec.arrive_side && !if_force_stop_main_side)
			|| !IS_VALID_SIDE(ori_rec.arrive_side))
		{
			if (m_pConfig->IfdChoseSideWhileAllFull())
			{
				ori_rec.arrive_side = order_side[0];
				ori_rec.depart_side = order_side[0];
			}
			else
			{
				ori_rec.arrive_side = 255;
				ori_rec.depart_side = 255;
				GpcLogTrainMessage(0, 0, "[股道分配 计算]: %s(%d) 车站%d(entry %d exit %d), 股道占满,配置不能分配股道", 
				ptrain->GetTrainID(),ptrain->GetTrainIndex(),station, ori_rec.entry, ori_rec.exit);
			}
		}

		return TRUE;

	} while(0);

	// failed to get one side, reset to NULL for safe
	ori_rec.arrive_side = 255;
	ori_rec.depart_side = 255;
	return FALSE;
}

BOOL  CTGAutoAdjustLogic::TrainHasActualTimeRecord(const CTG_SingleTrain *ptrain,int index)
{
	for(int k=index; k<ptrain->GetTrainRecordCount(); k++) 
	{
		const CTG_TrainRecord *prec=ptrain->GetTrainRecordNoDelete_Const(k);
		if(!prec) continue; //NOT possible
		if(prec->IsActualArriveTimeRecord() || prec->IsActualDepartTimeRecord())
			return TRUE;
	}
	return FALSE;
}

BOOL  CTGAutoAdjustLogic::AdjustForStopPassChange(int con_id, CTG_SingleTrain *pnewtrain,int index,BOOL stop_to_pass)
{
	bool bSideNotChange=false;
	
	CTG_TrainRecord* prec = pnewtrain->GetTrainRecordNoDelete(index);
	if(!prec)
		return FALSE;
	
	BYTE side=prec->m_data.arrive_side; // 保存原股道
	if(pnewtrain->IsPassengerTrain())
		bSideNotChange=GetTGFunctionConfig()->IsSideNotChange(con_id, prec->m_data.station, prec->m_data.arrive_side, 1);
	else
		bSideNotChange=GetTGFunctionConfig()->IsSideNotChange(con_id, prec->m_data.station, prec->m_data.arrive_side, 2);
	
	BOOL ret=AdjustForStopPassChange(pnewtrain,index,stop_to_pass);

	if(bSideNotChange && IS_VALID_SIDE(side))
	{
		CTG_TrainRecord* prec = pnewtrain->GetTrainRecordNoDelete(index);
		if(prec)
		{
			prec->m_data.arrive_side=side;  // 恢复原股道
			prec->m_data.depart_side=side;

			if(stop_to_pass)
				GpcLogTrainMessage(0, 0, "根据tg_logic.cfg中[STATION_SIDE_NOT_CHANGE]配置 %s (%d) 在车站 %d 股道到开-->通过不变%d %d",
					pnewtrain->GetTrainID(), pnewtrain->GetTrainIndex(), prec->m_data.station,  
					prec->m_data.arrive_side, prec->m_data.depart_side);
			else
				GpcLogTrainMessage(0, 0, "根据tg_logic.cfg中[STATION_SIDE_NOT_CHANGE]配置 %s (%d) 在车站 %d 股道通过-->到开不变%d %d",
						pnewtrain->GetTrainID(), pnewtrain->GetTrainIndex(), prec->m_data.station,  
						prec->m_data.arrive_side, prec->m_data.depart_side);
		}
	}

	return ret;
}

BOOL  CTGAutoAdjustLogic::AdjustForStopPassChange(CTG_SingleTrain *pnewtrain,int index,BOOL stop_to_pass) 
{
	int rec_count=pnewtrain->GetTrainRecordCount();
	CTG_TrainRecord *prec=pnewtrain->GetTrainRecord(index);
	if(!prec)
		return FALSE;
	int station=prec->GetStationNo();
	int stop_extra_time = m_pBasicPara->CalculateTrainStopExtra(station,prec->GetEntry(),TRAIN_INTO_STATION,pnewtrain->GetRunType());
	int start_extra_time =m_pBasicPara->CalculateTrainStartExtra(station,prec->GetExit(),TRAIN_OUT_STATION,pnewtrain->GetRunType());
	if(!GetTGFunctionConfig()->IsEnablePasstoStayModifyArivalTime())
	{
		stop_extra_time=0;
	}
	int offset=0;
	if (stop_to_pass)   // stop -> pass
	{ 
		offset = -1 * (stop_extra_time + start_extra_time+prec->m_data.depart-prec->m_data.arrive);
		prec->m_data.arrive -= stop_extra_time;
		prec->m_data.depart=prec->m_data.arrive;

	}
	else // pass ->stop
	{						
		//offset=300;
		TIME prev_depart_time=prec->m_data.depart;

		prec->m_data.arrive += stop_extra_time;
		if(m_nAdjustSchdType==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_PREV)
			prec->m_data.depart += stop_extra_time+120;
		else
			prec->m_data.depart += stop_extra_time+300; // 默认停5分钟
		TIME depart_time=CacuTrainDepartTime(pnewtrain,index);
		prec->m_data.depart=depart_time;
		offset = prec->m_data.depart-prev_depart_time+start_extra_time;
		long t = prec->m_data.min_stop_time - (prec->m_data.depart - prec->m_data.arrive);
		if (prec->m_data.depart + t > prec->m_data.arrive)
		{
			prec->m_data.depart += t;
			offset += t;
		}
	}

	if(TrainHasActualTimeRecord(pnewtrain,index)) // added by wx 0718
		return TRUE;

	ChangeSideForStopOrPass(pnewtrain,index);
	if(m_nAdjustSchdType==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_PREV)
		return TRUE; // 基本图不能修改后续点时间
	for(int i=index+1;i<rec_count;i++)
	{
		prec=pnewtrain->GetTrainRecord(i);
		if(!prec)
			continue; 
		prec->m_data.arrive+=offset;
		prec->m_data.depart+=offset;
	}
	return TRUE;
}

const char *CTGAutoAdjustLogic::CheckTrainManualUpdateOper(CTG_SingleTrain *pnewtrain,const CMMIOperFlag &oper_flag,DWORD& status)
{
	static char reason[2048];
	if(oper_flag.nFlag==CMMIOperFlag::MUTI_REC_STAY_TO_PASS)
	{
		;
	}//通过变到开，到开变通过的逻辑全部放入ocx实现
	else if((oper_flag.nFlag&3)==CMMIOperFlag::STAY_TO_PASS)
	{
		CTG_TrainRecord *prec=pnewtrain->GetTrainRecord(oper_flag.m_nRecIndex);
		if(prec)
		{
			if(TG_IsMustStop(prec->m_data))
			{
				sprintf(reason,"日班计划规定必须停车,不能办理通过,你确认要通过吗?");
				status=112;
				return reason;
			}

			if(TG_IsMustNoPass(prec->m_data))
			{
				sprintf(reason,"计划规定禁止通过,不能办理通过,你确认要通过吗?");
				status=112;
				return reason;
			}

			if(prec->m_data.min_stop_time>0)
			{
				int minute=prec->m_data.min_stop_time/60;
				CString strStationName=gpcdef.GetStationName(prec->m_data.station);
				if(GetTGFunctionConfig()->IsGoodTrainNotTechStopNeedPwd() && !pnewtrain->IsPassengerTrain())
				{
					sprintf(reason,"%s 日班计划要求有%d分钟 营业时分,不能办理通过,你确实要办理通过吗?", strStationName, minute);
					status=112;
					return reason;
				}
				else
				{
					sprintf(reason,"%s 日班计划要求有%d分钟 营业时分,不能办理通过,你确实要办理通过吗?", strStationName, minute);
					status=112;
					return reason;
				}
			}

			const char *pshuaigua=prec->GetStationText(STATION_TXT_INFO_TYPE_SHUAIGUA);
			if(pshuaigua && strlen(pshuaigua)>0)
			{
				CString strStationName=gpcdef.GetStationName(prec->m_data.station);
				sprintf(reason,"%s 有车站作业:%s,你确实要办理通过吗?",strStationName,pshuaigua);
				status=112;
				return reason;
			}			
		}
	}
	else if((oper_flag.nFlag&3)==CMMIOperFlag::PASS_TO_STAY)
	{
		CTG_TrainRecord *prec=pnewtrain->GetTrainRecord(oper_flag.m_nRecIndex);
		if(prec)
		{
			if(TG_IsMustNotStop(prec->m_data))
			{
				sprintf(reason,"日班计划规定禁止停车,你确认要到开吗?");
				status=112;
				return reason;
			}
		}
	}
	else
	{
		if(!GetTGFunctionConfig()->IfCheckManualOper() || !GetTGFunctionConfig()->NeedToCheckTGOper(pnewtrain->GetTrainType()))  // 290090913增加条件，路用列车不判断
		{
			return NULL;
		}
		if(GetTGFunctionConfig()->GetAutoAdjustOperFlag()==1)  // 自动避让调整时无需判断以下冲突
			return NULL;
		int i=0;
		int rec_count=pnewtrain->GetTrainRecordCount();
		CTG_TrainRecord *prec;
		for(i=rec_count-1;i>=0;i--)  //找到最后一个非实际点的记录
		{
			prec=pnewtrain->GetTrainRecord(i);
			if(!prec)
				continue;
			if(prec->IsActualArriveTimeRecord() || prec->IsActualDepartTimeRecord())
				break;
		}

		
		if(i<0) // no need to do this
			i=0;
		
		if(i < oper_flag.m_nRecIndex)
			i = oper_flag.m_nRecIndex;
		
		for(;i<=rec_count-1;i++)
		{
			TIME departtm=0;
			CString err_info;
			unsigned long alarm_flag=0;
			
			if(CheckTrain_ABSBlock(pnewtrain,i,alarm_flag,err_info,departtm))
			{
				if(alarm_flag & ALARM_FLAG_LINE_BLOCK_ERROR)
				{
					strcpy(reason,err_info);
					status=101;
					return reason;
				}
			}

			if(CheckTrainABSMeetInfo(pnewtrain,i,alarm_flag,err_info))
			{
				if(alarm_flag & ALARM_FLAG_MEET_ERROR)
				{
					strcpy(reason,err_info);
					status=102;
					return reason;
				}
			}

			CTG_TrainRecord *precord=pnewtrain->GetTrainRecordNoDelete(i);
			if(precord)
			{
				precord->ClearStplanWarnInfo();
				CheckTrainSideInfoForStplan(pnewtrain,i,true);
				if(precord->HaveWarnInfo())
				{
					GpcLogTrainMessage(0, 0,"人工修改产生报警:");

					CString strText;
					WORD id=0;
					CString text;
                    for(int index=0; index<1024; index++)
					{
						if(precord->GetStplanWarnInfo(index, id, text))
						{
							GpcLogTrainMessage(0, 0,"%s", text);
							strText += text;
							strText += "\n";
						}
						else
						{
							break;
						}
					}
					precord->ClearStplanWarnInfo();

					strncpy(reason,strText,sizeof(reason)-1);  
					reason[sizeof(reason)-1]=0;
					status=103;
					return reason;
				}
			}
		}
	}
	status=0;
	return NULL;
}

BOOL CTGAutoAdjustLogic::AdjustTrainForManualUpdate(CTG_SingleTrain *pnewtrain,const CMMIOperFlag &oper_flag,int con_id,CWnd* pParentWnd)
{
	int rec_index = oper_flag.m_nRecIndex;
	CTG_TrainRecord* rec = pnewtrain->GetTrainRecord(rec_index);
	if (NULL == rec)
		return FALSE;

	CTG_TrainRecord old = *rec;

	if(oper_flag.nFlag & CMMIOperFlag::MUTI_REC_STAY_TO_PASS)
	{
		for(int i=oper_flag.m_nRecIndex; i<pnewtrain->GetTrainRecordCount(); i++)
		{
			CTG_TrainRecord* rec = pnewtrain->GetTrainRecord(i);
			if (NULL == rec)
				continue;
			if(rec->IsPassRec())
				continue;
			// 有作业时分
			if(rec->m_data.min_stop_time>0)
			{
				GpcLogTrainMessage(0, 0, "多点调整, 列车%s在%s站 最小作业时分为%d, 不能变为通过",
					pnewtrain->GetTrainID(), gpcdef.GetStationName(rec->GetStationNo()), rec->m_data.min_stop_time);
				continue;
			}
			if(TG_IsMustStop(rec->m_data))
			{
				GpcLogTrainMessage(0, 0, "多点调整, 列车%s在%s站 设置为必须停车, 不能变为通过",
					pnewtrain->GetTrainID(), gpcdef.GetStationName(rec->GetStationNo()));
				continue;
			}
			if(TG_IsMustNoPass(rec->m_data))
			{
				GpcLogTrainMessage(0, 0, "多点调整, 列车%s在%s站 设置为禁止通过, 不能变为通过",
					pnewtrain->GetTrainID(), gpcdef.GetStationName(rec->GetStationNo()));
				continue;
			}
			// 实际点
			if(rec->IsActualArriveTimeRecord() || rec->IsActualDepartTimeRecord())
				continue;
			if (TG_IsArriveRouteDoing(rec->m_data) || 
				TG_IsArriveRouteOk(rec->m_data) || 
				TG_IsArriveRouteOccupy(rec->m_data))
			{
				GpcLogTrainMessage(0, 0, "多点调整, 列车%s在%s站 接车进路正在排出或者已排出或者占用, 不能变为通过",
					pnewtrain->GetTrainID(), gpcdef.GetStationName(rec->GetStationNo()));

				continue;
			}
			if (TG_IsDepartRouteDoing(rec->m_data) || 
				TG_IsDepartRouteOk(rec->m_data) || 
				TG_IsDepartRouteOccupy(rec->m_data))
			{
				GpcLogTrainMessage(0, 0, "多点调整, 列车%s在%s站 发车进路正在排出或者已排出或者占用, 不能变为通过",
					pnewtrain->GetTrainID(), gpcdef.GetStationName(rec->GetStationNo()));

				continue;
			}

			if (TG_IsTrainIntoSide(rec->m_data))
			{
				GpcLogTrainMessage(0, 0, "多点调整, 列车%s在%s站 已经进站, 不能变为通过",
					pnewtrain->GetTrainID(), gpcdef.GetStationName(rec->GetStationNo()));

				continue;
			}
			
			// 非技术停点
			if( !TG_IsTechStopTrain(rec->m_data))
			{
				GpcLogTrainMessage(0, 0, "多点调整, 列车%s在%s站 有客运作业, 不能变为通过",
					pnewtrain->GetTrainID(), gpcdef.GetStationName(rec->GetStationNo()));
				continue;
			}
			
			if(GetTGFunctionConfig()->IsNotThisSectionStation(con_id,  rec->m_data.station))
			{
				GpcLogTrainMessage(0, 0, "请注意, %s 不属于本台管辖,不能修改到开方式", gpcdef.GetStationName(rec->m_data.station));
				continue;
			}
			
			AdjustForStopPassChange(con_id, pnewtrain, i, TRUE);
		}
	}
	else if(oper_flag.nFlag & CMMIOperFlag::STAY_TO_PASS)
	{
		AdjustForStopPassChange(con_id, pnewtrain,oper_flag.m_nRecIndex,TRUE);
	}
	else if(oper_flag.nFlag & CMMIOperFlag::PASS_TO_STAY)
	{
		AdjustForStopPassChange(con_id, pnewtrain,oper_flag.m_nRecIndex,FALSE);
	}
	
	SetTrainPassNonMainSide(pnewtrain);
	//SetDefualtEntryAndExit(pnewtrain);  // 对于-1的entry exist设置
	
	// 设置本钱点
	this->SetTrainFJK(pnewtrain);

	if(oper_flag.nFlag & CMMIOperFlag::STAY_TO_PASS
		|| oper_flag.nFlag & CMMIOperFlag::PASS_TO_STAY)
	{	
		// 20150121沈阳局要求加入 通过变到开修改股道时 对驶入非电力区段的电力机车进行报警
		if (TG_IsElecLocoFlag(rec->m_data))
		{
			bool bCheck=false;
			int nFlag=GetTGFunctionConfig()->GetCheckElecSideWhenPassToStopFlag();
			if(nFlag==2) // 不论TDCS\CTC均检查
			{
				bCheck=true;
			}
			else if(nFlag==1) // 仅CTC车站检查
			{
				int station=rec->GetStationNo();
				if(gpcdef.IsCtcStation(station))
				{
					bCheck=CWorkScheduleHolder::IsNeedCheckCtcModePlan(station);
				}
				else
				{
					bCheck=false;
				}
			}
			else
			{
				bCheck=false;
			}

			if(bCheck)
			{
				const CStationDef *pStationDef=m_pBasicPara->GetStationDef(rec->GetStationNo());
				if(pStationDef==NULL)
					return FALSE;

				if (!pStationDef->isCatenary(rec->m_data.arrive_side))
				{
					SIDE_NAME arrive_side_name;
					memset(arrive_side_name, 0, sizeof(arrive_side_name));
					pStationDef->GetSideName(rec->m_data.arrive_side, arrive_side_name);
					
					CString str_arrive_side_name=arrive_side_name;
					if(str_arrive_side_name!="")
					{
						CString strText;
						strText.Format("由于人工操作导致, 车次 %s 在 %s 接入无电力股道 %s", 
							pnewtrain->GetTrainID(), 
							gpcdef.GetStationName(rec->GetStationNo()), 
							arrive_side_name);
						
						if(pParentWnd)
						{
							pParentWnd->MessageBox("请注意:\n"+strText, "错误提示", MB_OK|MB_ICONHAND);
						}
						strText = "报警("+strText+")";
						GpcLogTrainMessage(0,0,strText.GetBuffer());
					}
				}
				
				if ((rec->m_data.depart_side!=rec->m_data.arrive_side) && !pStationDef->isCatenary(rec->m_data.depart_side))
				{
					SIDE_NAME depart_side_name;
					memset(depart_side_name, 0, sizeof(depart_side_name));
					pStationDef->GetSideName(rec->m_data.depart_side, depart_side_name);
					
					CString str_depart_side_name=depart_side_name;
					if(str_depart_side_name!="")
					{
						CString strText;
						strText.Format("由于人工操作导致, 车次 %s 在 %s 无电力股道 %s 发车", 
							pnewtrain->GetTrainID(), 
							gpcdef.GetStationName(rec->GetStationNo()), 
							depart_side_name);

						if(pParentWnd)
						{
							pParentWnd->MessageBox("请注意:\n"+strText,"错误提示", MB_OK|MB_ICONHAND);
						}
						
						strText = "报警("+strText+")";
						GpcLogTrainMessage(0,0,strText.GetBuffer());
					}
				}
			}
		}

		GpcLogTrainMessage(0, 0, "股道变更train %s %u sta %d 股道:%d %d -> %d %d, 出口:%d 入口:%d",
			pnewtrain->GetTrainID(), pnewtrain->GetTrainIndex(), rec->GetStationNo(), 
			old.m_data.arrive_side, old.m_data.depart_side, 
			rec->m_data.arrive_side, rec->m_data.depart_side,
			rec->m_data.exit, rec->m_data.entry);
	}
	else
	{
		// 人工操作--重新计算非法股道  20131204 挪线调整非法股道 不允许
		//for(int i=0; i<pnewtrain->GetTrainRecordCount(); i++)
		//{
		//	CTG_TrainRecord* rec = pnewtrain->GetTrainRecord(i);
		//	if (NULL == rec)
		//		continue;

		//	if(!IS_VALID_SIDE(rec->m_data.depart_side) || !IS_VALID_SIDE(rec->m_data.arrive_side))
		//	{
		//		ChangeSideForStopOrPass(pnewtrain, i);
		//	}
		//}
	}

	return TRUE;
}

//////////// 逻辑冲突检查 /////////////
void  CTGAutoAdjustLogic::CheckAndSetAlarmInfo(CTG_SingleTrain *single)
{
	if(!GetTGFunctionConfig()->IfCheckAndSetAlarm())
	{
		return;
	}
	CTime tm = CTime::GetCurrentTime();
	long start_time=tm.GetTime();
	long end_time=start_time+3*60*60;
		
	CTG_TrainPtrGroup group;
	m_pWorkSchedule->FindTrains(group,NULL_STATION,start_time,end_time);

	CTG_ObjectPtrGroup markgroup;
	m_pWorkSchedule->FindMarks(markgroup,start_time,end_time);

	CheckAndSetAlarmInfo(single, start_time, end_time, group, markgroup);
}

void  CTGAutoAdjustLogic::CheckAndSetAlarmInfo(CTG_SingleTrain *single, long start_time, long end_time, CTG_TrainPtrGroup& group, CTG_ObjectPtrGroup& markgroup)
{
	unsigned long alarm_flag=0;
	CString  strText;
	int alarm_count=0;
	for(int rec_index=0;rec_index<single->GetTrainRecordCount();rec_index++)
	{
		CTG_TrainRecord *precord=single->GetTrainRecord(rec_index);
		if(!precord) continue;
		do
		{
			alarm_flag=0;
			strText="";
			
			precord->ClearStplanWarnInfo();
			CheckTrainPassAndStop(single, rec_index);
			if(precord->HaveWarnInfo())
			{
				WORD id;
				CString text;
				if(precord->GetStplanWarnInfo(0, id, text))
				{
					strText=text;
					break;
				}
			}
			
			// 检查列车与施工在区间冲突
			TIME etime=0;
			if(CheckTrain_ABSBlock(single,rec_index,alarm_flag,strText,etime,markgroup))
				break;

			// 检查列车区间冲突
			if(CheckTrainABSMeetInfo(single,rec_index,alarm_flag,strText,start_time,end_time,group))
			    break;
			
			// 检查列车站内冲突
			if(CheckTrainStaMeetInfo(single,rec_index, start_time, end_time,group,alarm_flag,strText))
				break;

			CTG_TrainRecord *precord=single->GetTrainRecordNoDelete(rec_index);
			if(precord)
			{
				precord->ClearStplanWarnInfo();
				CheckTrainSideInfoForStplan(single,rec_index,true);
				if(precord->HaveWarnInfo())
				{
					WORD id;
					CString text;
					if(precord->GetStplanWarnInfo(0, id, text))
					{
						strText=text;
						break;
					}
				}
			}
		}while(0);

		precord->m_strAlarmText.clear();
		precord->ClearStplanWarnInfo();
		if(alarm_flag>0 || strText!="")
		{
			alarm_count++;
			precord->m_nAlarmStatus|=alarm_flag;
			precord->m_strAlarmText+=" ";
			precord->m_strAlarmText+=strText;
			precord->m_strAlarmText+=" ";
		}
		else
		{
			precord->m_nAlarmStatus=0;
			precord->m_strAlarmText="";
		}
	}
}

static BOOL CheckNotToMeetRecordFlag(const CTG_TrainRecord *prec,const CTG_SingleTrain *train)
{
	WORD nNotMeetFlag=(prec->m_data.can_not_meet&0xFFFF);
	if(TG_NOTMEET_CRH(prec->m_data.can_not_meet) && train->IsCRHTrain())
		return TRUE;
	if(TG_NOTMEET_PASSENGER(prec->m_data.can_not_meet) && train->IsPassengerTrain())
		return TRUE;
	if(TG_NOTMEET_FREIGHT(prec->m_data.can_not_meet))
		return TRUE;
	if(TG_NOTMEET_SEPECIAL_Z(prec->m_data.can_not_meet) && train->IsSpecialZ())
		return TRUE;
	if(TG_NOTMEET_SPACE(prec->m_data.can_not_meet) && (train->GetSpaceRank()>0))
		return TRUE;
	
	NotMeetInfo info;
	bool bFind=gpcdef.getNotMeetInfoByNoMeetFlag(nNotMeetFlag, info);
	if(bFind)
	{
		int train_type = train->GetTrainType();
		for (int n=0; n<info.nTrainTypeCount; n++)
		{
			if (info.nTrainTypeList[n]==train_type || info.nTrainTypeList[n]==-1)
			{
				return TRUE;
			}
		}
	}
	return FALSE;
}

BOOL CheckExtNotMeet(const CTG_SingleTrain *train1,const CTG_SingleTrain *train2)
{
		if(train1->IsCRHTrain() && train2->GetSpaceRank()>0)
			return TRUE;
		if(train2->IsCRHTrain() && train1->GetSpaceRank()>0)
			return TRUE;
        return FALSE;
}

// 注意: 下面函数没有考虑列车间隔时分 2010-11-20 cuihu
// 本函数只判断 train1的发车和train2的接车\发车是否冲突
static int CheckNotToMeetWithTime(const CTG_SingleTrain *train1,const CTG_SingleTrain *train2,int recindex1,int recindex2,TIME  rec_from_depart=0,TIME  rec_to_arrival=0) //后两个参数是train1的时间,可以指定非record中的时间
{
	bool bCheckSpaceMeet=false;
	if(gpcdef.IsFormatView())
	{
		if (train1->GetSpaceRank() != 0)
			bCheckSpaceMeet=true;
	}
	else
	{
		if (GetTGFunctionConfig()->IsAlarmNotCheckSpackTrainMeetInfo())
		{
			bCheckSpaceMeet=false;
		}
		else
		{
			if (train1->GetSpaceRank() != 0)
				bCheckSpaceMeet=true;
		}
	}

	const CTG_TrainRecord *pthis_rec=train1->GetTrainRecordNoDelete_Const(recindex1);
	const CTG_TrainRecord *pthis_nextrec=train1->GetTrainRecordNoDelete_Const(recindex1+1);
	const CTG_TrainRecord *prela_rec=train2->GetTrainRecordNoDelete_Const(recindex2);
	const CTG_TrainRecord *prela_nextrec=NULL;
	const CTG_TrainRecord *prela_fromrec=NULL;
	if(!pthis_rec ||  !pthis_nextrec || !prela_rec)
		return 0;
	if(pthis_rec->GetStationNo()!=prela_rec->GetStationNo())
		return 0;
	ushort abs_num_in1=0,abs_num_in2=0,dir1,dir2,abs_num_out1=0,abs_num_out2=0;
	GetTGBasicParam()->ConvertStationEntryToAbsnum(pthis_rec->GetStationNo(),pthis_rec->GetExit(),&abs_num_out1,&dir1); //本车次的发车口

	GetTGBasicParam()->ConvertStationEntryToAbsnum(prela_rec->GetStationNo(),prela_rec->GetEntry(),&abs_num_in2,&dir2); // 其他车次的接车口
	GetTGBasicParam()->ConvertStationEntryToAbsnum(prela_rec->GetStationNo(),prela_rec->GetExit(),&abs_num_out2,&dir2); // 其他车次的发车口

	int meet_type=0;
	if((abs_num_out1==abs_num_out2) && (abs_num_out1!=0))  //bug
	{
		prela_nextrec =train2->GetTrainRecordNoDelete_Const(recindex2+1);
		prela_fromrec =prela_rec;
		meet_type=1;   // 同向争用区间
	}
	else if((abs_num_out1==abs_num_in2) && (abs_num_out1!=0))  //bug
	{
		prela_nextrec=prela_rec;
		if(recindex2>0)
		{
			prela_fromrec=train2->GetTrainRecordNoDelete_Const(recindex2-1);
			meet_type=2; // 对向争用区间
		}
	}
	else
	{
		prela_nextrec=prela_rec;
		if(recindex2>0)
		{
			prela_fromrec=train2->GetTrainRecordNoDelete_Const(recindex2-1);
			if(prela_fromrec!=NULL && prela_nextrec!=NULL)
			{
				if(pthis_rec->GetStationNo()==prela_nextrec->GetStationNo() && pthis_nextrec->GetStationNo()==prela_fromrec->GetStationNo())
				{
					if(CheckNotToMeetRecordFlag(pthis_rec,train2)|| CheckNotToMeetRecordFlag(prela_fromrec,train1) || CheckExtNotMeet(train1,train2))	
						meet_type=3; // 区间交汇
					else if(bCheckSpaceMeet)
						meet_type=3; // 区间交汇
				}
			}
		}
	}

	int nResult=0;
	if((meet_type!=0) && (prela_nextrec!=NULL) && (prela_fromrec!=NULL))
	{
		if(!rec_from_depart)
			rec_from_depart= pthis_rec->GetDepartTime(); 
		if(!rec_to_arrival)
			rec_to_arrival = pthis_nextrec->GetArriveTime(); 
		
		// 对向
		if ((meet_type==2 || meet_type==3) && prela_nextrec->GetArriveTime() >= rec_from_depart	&& rec_to_arrival >= prela_fromrec->GetDepartTime())
		{
			nResult = 1;
			if(bCheckSpaceMeet)
				nResult = 2;
		}
		// 同向
		if (meet_type==1 && (prela_nextrec->GetArriveTime() <= rec_to_arrival && prela_fromrec->GetDepartTime() >=rec_from_depart
			|| prela_nextrec->GetArriveTime() >= rec_to_arrival && prela_fromrec->GetDepartTime() <=rec_from_depart))  // 原来缺少一种情况
		{
			nResult = 1;
			if(bCheckSpaceMeet)
				nResult = 2;
		}
	}
	return nResult;	
}

BOOL  CTGAutoAdjustLogic::CheckTrainABSMeetInfo(CTG_SingleTrain *single, int rec_index, ULONG &alarm_flag, CString &strText)
{
	CTime tm = CTime::GetCurrentTime();
	long start_time=tm.GetTime();
	long end_time=start_time+3*60*60;
		
	CTG_TrainPtrGroup group;
	m_pWorkSchedule->FindTrains(group,NULL_STATION,start_time,end_time);

	return CheckTrainABSMeetInfo(single, rec_index, alarm_flag, strText, start_time, end_time, group);
}

BOOL CTGAutoAdjustLogic::CheckTrainABSMeetInfo(CTG_SingleTrain *single,int rec_index,ULONG &alarm_flag,CString &strText, long start_time, long end_time, CTG_TrainPtrGroup& group)
{
	alarm_flag=0;
	CTG_TrainRecord *precord_from=single->GetTrainRecord(rec_index);
	CTG_TrainRecord *precord_to=single->GetTrainRecord(rec_index+1);

	if(!precord_from || !precord_to)
		return FALSE;
	TIME  rec_from_depart = precord_from->GetDepartTime(); 
	TIME  rec_to_arrival = precord_to->GetArriveTime(); 

	if(end_time<rec_from_depart || rec_to_arrival<start_time)
		return FALSE;

	const char* strDepartStaName = gpcdef.GetStationName(precord_from->GetStationNo());
	const char* strArrivalStaName = gpcdef.GetStationName(precord_to->GetStationNo());
	if(!strDepartStaName || !strArrivalStaName)
		return FALSE;
		
	const CTG_TrainRecord *rec=NULL;
	for(int j=0; j<group.GetTrainCount(); j++)
	{
		CTG_SingleTrain *pTrain = group.GetTrainPtr(j); 
		if(!pTrain || pTrain->GetTrainIndex()==single->GetTrainIndex())
			continue;

		for(int i=0;i<pTrain->GetTrainRecordCount();i++)  // 乔永涛 2011-1-17, 不要-1
		{
			// 判断禁会条件和区间共用冲突
			if (CheckNotToMeetWithTime(single,pTrain,rec_index,i))
			{
					strText.Format("列车%s与%s在%s至%s区间冲突", single->GetTrainID(), pTrain->GetTrainID(), strDepartStaName, strArrivalStaName);
					alarm_flag=ALARM_FLAG_MEET_ERROR;
					break;
			}
		}
		if(alarm_flag>0)
			break;
	}

	if(alarm_flag>0)
		return TRUE;
	else
		return FALSE;
	return TRUE;
}

BOOL  CTGAutoAdjustLogic::CheckTrainStaMeetInfo(CTG_SingleTrain *single,int rec_index, long start_time, long end_time, CTG_TrainPtrGroup& trains_in_range, unsigned long &alarm_flag,CString &strText)
{
	alarm_flag=0;
	strText="";

	if(!gpcdef.IsFormatView())
	{
		if (GetTGFunctionConfig()->IsAlarmNotCheckSpackTrainMeetInfo())
			return FALSE;
	}
		
	if (single->GetSpaceRank() == 0)
		return FALSE;

	CTG_TrainRecord *precord=single->GetTrainRecord(rec_index);
	if(!precord)
		return FALSE;

	const char* strStaName = gpcdef.GetStationName(precord->GetStationNo());
	if(!strStaName)
		return FALSE;

	TIME  rec_depart = precord->GetDepartTime();
	TIME rec_arrival = precord->GetArriveTime();

	if(start_time>rec_depart || rec_arrival>end_time)
		return FALSE;

	int j=0;
	for(j=0; j<trains_in_range.GetTrainCount(); j++)
	{
		CTG_SingleTrain *pTrain = trains_in_range.GetTrainPtr(j); 
		if(!pTrain || pTrain->GetTrainIndex()==single->GetTrainIndex())
			continue;
		if ( pTrain->GetSpaceRank() == 0 )
			continue;
		CTG_TrainRecord *thisrec = pTrain->GetTrainRecordByStation(precord->GetStationNo());
		if(!thisrec)
			continue;

		TIME this_depart =  thisrec->GetDepartTime();
		TIME this_arrival = thisrec->GetArriveTime();
		if (thisrec->IsStartRec())
			this_arrival = this_depart;
		else if (thisrec->IsTerminalRec() )
			this_depart = this_arrival;

		if ( this_depart >= rec_arrival && rec_depart >= this_arrival )
		{
			if (GetTGBasicParam()->EnableStaSideLimitCfg())
			{
				if (GetTGBasicParam()->IsTwoStationSideLimit(precord->GetStationNo(), precord->m_data.arrive_side, thisrec->m_data.arrive_side))
				{
					strText.Format("超限列车%s次与超限列车%s次在%s站接入不能交会股道", 
								single->GetTrainID(), pTrain->GetTrainID(), strStaName);
					alarm_flag=ALARM_FLAG_MEET_ERROR;
					break;
				}
			}
			else
			{
				strText.Format("超限列车%s次和超限列车%s次在%s站接入不能交会股道", 
								single->GetTrainID(), pTrain->GetTrainID(), strStaName); // stplan warn 20161019
				alarm_flag=ALARM_FLAG_MEET_ERROR;
				break;
			}
		}
	}

	if(strText!="")
	{
		precord->AddStplanWarnInfo(1, strText);
		return TRUE;
	}
	else
		return FALSE;
}

//added by zhaoliang for same side check
//目前只检测到发股道一致的情况
BOOL  CTGAutoAdjustLogic::CheckTrainStaSideInfo(CTG_SingleTrain *single,int rec_index, long start_time, long end_time, CTG_TrainPtrGroup& trains_in_range, unsigned long &alarm_flag,CString &strText)
{
	alarm_flag=0;
	strText="";

	CTG_TrainRecord *precord=single->GetTrainRecord(rec_index);
	if(!precord)
		return FALSE;

	const char* strStaName = gpcdef.GetStationName(precord->GetStationNo());
	if(!strStaName)
		return FALSE;

	if ( precord->IsStartRec() || precord->IsTerminalRec() )
		return FALSE;

	TIME rec_depart = precord->GetDepartTime();
	TIME rec_arrival = precord->GetArriveTime();
	BYTE rec_side_depart  = precord->m_data.depart_side;
    BYTE rec_side_arrival = precord->m_data.arrive_side;

	rec_arrival=rec_arrival-rec_arrival%60; // 去掉秒
	rec_depart=rec_depart-rec_depart%60; // 去掉秒

	if(end_time<rec_depart || rec_arrival<start_time)
		return FALSE;
		
	int j;
	const CTG_TrainRecord *rec=NULL;
	for(j=0; j<trains_in_range.GetTrainCount(); j++)
	{
		CTG_SingleTrain *pTrain = trains_in_range.GetTrainPtr(j); 
		if(!pTrain || pTrain->GetTrainIndex()==single->GetTrainIndex())
			continue;
		CTG_TrainRecord *thisrec = pTrain->GetTrainRecordByStation(precord->GetStationNo());
		if(!thisrec)
			continue;

		TIME this_depart =  thisrec->GetDepartTime();
		TIME this_arrival = thisrec->GetArriveTime();
		BYTE this_side_depart  = thisrec->m_data.depart_side;
		BYTE this_side_arrival = thisrec->m_data.arrive_side;

		this_arrival=this_arrival-this_arrival%60; // 去掉秒
		this_depart=this_depart-this_depart%60; // 去掉秒

		if (thisrec->IsStartRec())
			this_arrival = this_depart;
		else if (thisrec->IsTerminalRec() )
			this_depart = this_arrival;

		if ( this_depart >= rec_arrival && rec_depart >= this_arrival )
		{
			if ((rec_side_depart==rec_side_arrival)&&(this_side_depart==this_side_arrival)&&(rec_side_depart==this_side_depart))
			{
				strText.Format("%s次与%s次在%s站相同股道交会", single->GetTrainID(), pTrain->GetTrainID(), strStaName );
				alarm_flag=ALARM_FLAG_MEET_ERROR;
				
				break;
			}
		}
	}
	if(alarm_flag>0)
		return TRUE;
	else
		return FALSE;
}
////////// 施工封锁限制条件

BOOL CTGAutoAdjustLogic::CheckTrain_SideBlock(STATION_NO  station,SIDE_ID side_id,TIME &etime,TIME &stime)
{
	TIME reg_time=etime;
	CTG_ObjectPtrGroup group;
	m_pWorkSchedule->FindMarks(group);
	for(int i=0;i<group.GetCount();i++)
	{
		const CTG_SingleMark *iter_mark=(const CTG_SingleMark *)group.GetPtr(i);
		const TG_ScheduleBlock &mark = iter_mark->m_data;
		if (TG_SIDE_INHIBIT != mark.type && TG_SIDE_TIANCHUANG != mark.type && TG_SIDE_OCCUPY != mark.type)
		{
			continue;
		}

		if (mark.station != station)
		{
			continue;
		}

		if (stime > 0)
		{
			if (stime < mark.start_time || mark.end_time < etime)
			{
				continue;
			}
		}
		else 
		{
			if (mark.start_time > etime)
			{
				continue;
			}
		}
		if(GetTGFunctionConfig()->IsStationBlockNotCheckSide()) //不检查具体股道 20091011 XIAN
		{ 
			if(reg_time< mark.end_time)
				reg_time = mark.end_time;	
		}
		else
			for(int j=0; j<mark.ele_count; j++)
			{
				SIDE_ID tmpside = (SIDE_ID) mark.ele_number[j];
				if(tmpside==side_id)
				{
					if (m_pBasicPara->VerifyStationSide(station, tmpside))
					{
						if (reg_time< mark.end_time)
							reg_time = mark.end_time;	
					}
					break;
				}
			}

	}
	if(reg_time!=etime)  
	{	
		etime=reg_time;
		return TRUE;
	}
	return FALSE;
}

BOOL  CTGAutoAdjustLogic::CheckTrain_ABSBlock(CTG_SingleTrain *single,int rec_index, unsigned long &alarm_flag,CString &strText,TIME &etime)
{
	CTime tm = CTime::GetCurrentTime();
	long start_time=tm.GetTime();
	long end_time=start_time+3*60*60;
		
	CTG_ObjectPtrGroup markgroup;
	m_pWorkSchedule->FindMarks(markgroup,start_time,end_time);
	
	return CheckTrain_ABSBlock(single,rec_index,alarm_flag,strText,etime,markgroup);
}

BOOL  CTGAutoAdjustLogic::CheckTrain_ABSBlock(CTG_SingleTrain *single,int rec_index, unsigned long &alarm_flag,CString &strText,TIME &etime, CTG_ObjectPtrGroup& markgroup)
{
	TIME reg_time;

	int absnum=-1;
	int absdir=-1;
	if(rec_index<0 || rec_index>=(single->GetTrainRecordCount()-1))
		return FALSE;
	CTG_TrainRecord *prec=single->GetTrainRecord(rec_index);
	CTG_TrainRecord *pnextrec=single->GetTrainRecord(rec_index+1);
	if(!prec || !pnextrec)
		return FALSE;
	int abs_run_time=pnextrec->GetArriveTime()-prec->GetDepartTime();
	alarm_flag=0;
	STATION_NO  station1,station2;
	ENTRY_ID exit_id,entry_id;
	station1=prec->GetStationNo();
	station2=pnextrec->GetStationNo();
	exit_id=prec->GetExit();
	entry_id=pnextrec->GetEntry();
	if(!m_pBasicPara->GetAbsNameBetweenStations(station1,station2,exit_id,entry_id ,&absnum,&absdir))
		return FALSE;
	if(etime==0)
		etime=prec->GetDepartTime();
	reg_time=etime;
	//CTG_ObjectPtrGroup group;
	//m_pWorkSchedule->FindMarks(group);
	for(int i=0;i<markgroup.GetCount();i++)
	{
		const CTG_SingleMark *iter_mark=(const CTG_SingleMark *)markgroup.GetPtr(i);
		const CTG_SingleMark  &mark = *iter_mark;
		if ( mark.GetMarkType()==TG_ABS_INHIBIT ||
			mark.GetMarkType()==TG_ABS_FIX_MARK  ||
			(mark.GetMarkType()==TG_SECTION_ELEC_FIX && single->IsElectricTrain())|| 
			(mark.GetMarkType()==TG_SECTION_ELEC_FIX_LANZHOU && single->IsElectricTrain()))
		{
			if(mark.MatchAbsNumber(absnum))
			{
				if (reg_time>= mark.GetStartTime() && reg_time<=mark.GetEndTime())
					reg_time = mark.GetEndTime();
				if ((reg_time+abs_run_time)>= mark.GetStartTime() && (reg_time+abs_run_time)<=mark.GetEndTime())
					reg_time = mark.GetEndTime();
			}

		}

	}
	if(reg_time!=etime)  
	{	
		etime=reg_time;
		alarm_flag=ALARM_FLAG_LINE_BLOCK_ERROR;
		strText.Format("%s次通过封锁、停电、检修或者停用线路!",single->GetTrainID());
		return TRUE;
	}
	return FALSE;
}

void  CTGAutoAdjustLogic::SetTrainPassNonMainSide(CTG_SingleTrain *ptrain)
{
	if(NULL==ptrain)
		return;

	int rec_count=ptrain->GetTrainRecordCount(); 
	for(int i=0;i<rec_count;i++)
	{
		CTG_TrainRecord *prec=ptrain->GetTrainRecord(i);
		
		TG_ClearFlagPassNonMainSide(prec->m_data.flag);
		if(!prec->IsTerminalRec() && !prec->IsStartRec() && prec->IsPassRec())
		{
			SIDE_ID main_side = m_pBasicPara->GetMainSide(prec->m_data);
			SIDE_ID side = prec->m_data.arrive_side;
			if(main_side!=side)  //重点测试 
				TG_SetFlagPassNonMainSide(prec->m_data.flag);
		}
	}

	return;
}

void  CTGAutoAdjustLogic::CheckTrainPassAndStop(CTG_SingleTrain *single, int rec_index)
{
	if(single==NULL)
		return;

	CTG_TrainRecord *prec=single->GetTrainRecordNoDelete(rec_index);
	if(prec==NULL)
		return;
	
	if(rec_index==0 && prec->IsStartRec())
		return;

	if(rec_index+1==single->GetTrainRecordCount() && prec->IsTerminalRec())
		return;

	//检查车站禁停,必停和禁止通过
	if(TG_IsMustNotStop(prec->m_data))  // "禁止停车";
	{
		if(!prec->IsPassRec())
		{
			CString strText;
			strText.Format("%s次禁止在%s站停车",single->GetTrainID(),gpcdef.GetStationName(prec->GetStationNo()));
			prec->AddStplanWarnInfo(5, strText);
		}
	}
	if(TG_IsMustStop(prec->m_data) || prec->m_data.min_stop_time>0) // "必须停车";
	{
		if(prec->IsPassRec())
		{
			CString strText;
			strText.Format("%s次必须在%s站停车",single->GetTrainID(),gpcdef.GetStationName(prec->GetStationNo()));
			prec->AddStplanWarnInfo(6, strText);
		}
	}
	if(TG_IsMustNoPass(prec->m_data))  // "禁止通过";
	{
		if(prec->IsPassRec())
		{
			CString strText;
			strText.Format("%s次禁止在%s站通过",single->GetTrainID(),gpcdef.GetStationName(prec->GetStationNo()));
			prec->AddStplanWarnInfo(7, strText); // 新增报警类型
		}
	}
}

void CTGAutoAdjustLogic::CacuTrainSlowTime(CTG_SingleTrain *single)
{
	CaculateTrainSlowTime(single);
}

void  CTGAutoAdjustLogic::GetScheduleInfo(CTG_ScheduleInfo& schd)
{
	m_pWorkSchedule->GetScheduleInfo(schd);
}

// 设置本钱点
void  CTGAutoAdjustLogic::SetTrainFJK(CTG_SingleTrain *ptrain)
{
	int flag = 0;
	int rec_count=ptrain->GetTrainRecordCount(); 
	for(int i=rec_count-2;i>=0;i--)
	{
		CTG_TrainRecord *prec=ptrain->GetTrainRecord(i);
		if (TG_IsFJKStationFlag(prec->m_data))
		{
			int method = GetTGFunctionConfig()->GetFjkStatisticMethod(prec->m_data.station);
			ptrain->SetFJKTime(*prec, method);
			flag = 1;	
		}
	}
	if (!flag)
	{
		CTG_TrainRecord *prec=ptrain->GetTrainRecord(0);
		if (prec->IsStartRec())
		{
			int method = GetTGFunctionConfig()->GetFjkStatisticMethod(prec->m_data.station);
			ptrain->SetFJKTime(*prec, method);
		}
	}
}
void  CTGAutoAdjustLogic::CheckTrainABSMeetInfoForStplan(CTG_SingleTrain *single,int rec_index,ULONG &alarm_flag,CString &strText, long start_time, long end_time, CTG_TrainPtrGroup& group)
{
	if(rec_index<0 || rec_index>=(single->GetTrainRecordCount()-1))
		return;

	CTG_TrainRecord *precord_from=single->GetTrainRecord(rec_index);
	CTG_TrainRecord *precord_to=single->GetTrainRecord(rec_index+1);

	if(!precord_from || !precord_to)
		return;
	TIME  rec_from_depart = precord_from->GetDepartTime(); 
	TIME  rec_to_arrival = precord_to->GetArriveTime(); 

	if(end_time<rec_from_depart || rec_to_arrival<start_time)
		return;

	const char* strDepartStaName = gpcdef.GetStationName(precord_from->GetStationNo());
	const char* strArrivalStaName = gpcdef.GetStationName(precord_to->GetStationNo());
	if(!strDepartStaName || !strArrivalStaName)
		return;
			
	const CTG_TrainRecord *rec=NULL;
	for(int j=0; j<group.GetTrainCount(); j++)
	{
		CTG_SingleTrain *pTrain = group.GetTrainPtr(j); 
		if(!pTrain || pTrain->GetTrainIndex()==single->GetTrainIndex())
			continue;

		for(int i=0;i<pTrain->GetTrainRecordCount();i++)  // 乔永涛 2011-1-17, 不要-1
		{
			// 判断禁会条件和区间共用冲突
			int nResult = CheckNotToMeetWithTime(single,pTrain,rec_index,i);
			if (nResult==1)
			{
				strText.Format("%s次与%s次在%s站至%s站区间冲突", single->GetTrainID(), pTrain->GetTrainID(), strDepartStaName, strArrivalStaName);
				precord_from->AddStplanWarnInfo(8, strText);
			}
			else if (nResult==2)
			{
				strText.Format("超限列车%s次与%s次在%s站(至%s站间)不符合交会条件", 
						single->GetTrainID(), pTrain->GetTrainID(), strDepartStaName, strArrivalStaName);
				
				precord_from->AddStplanWarnInfo(36, strText);
			}
		}
		if(strText != "")
			break;
	}

	return;
}

void  CTGAutoAdjustLogic::CheckTrainSideCrashForStplan(CTG_SingleTrain *single,int rec_index, long start_time, long end_time, CTG_TrainPtrGroup& group)
{
	CString strText;
	CTG_TrainRecord *precord=single->GetTrainRecord(rec_index);
	
	if(!precord)
		return;

	TIME  arrival = precord->GetArriveTime();
	TIME  depart = precord->GetDepartTime(); 

	if(end_time<arrival || depart<start_time)
		return;

	const char* strStaName = gpcdef.GetStationName(precord->GetStationNo());
	if(!strStaName)
		return;

	if(!IS_VALID_SIDE(precord->m_data.arrive_side)) // 未来冲突,只检查接车股道
		return;
		
	for(int j=0; j<group.GetTrainCount(); j++)
	{
		CTG_SingleTrain *pTrain = group.GetTrainPtr(j); 
		if(pTrain==NULL)
			continue;
		if(pTrain->GetTrainIndex()==single->GetTrainIndex())
			continue;

		CTG_TrainRecord *pobj = pTrain->GetTrainRecordByStation(precord->GetStationNo());
		if(pobj)
		{
			TIME  obj_arrival = pobj->GetArriveTime();
			TIME  obj_depart = pobj->GetDepartTime(); 

			if(end_time<obj_arrival || obj_depart<start_time)
				continue;

			if(!IS_VALID_SIDE(pobj->m_data.arrive_side))
				continue;

			if(precord->m_data.arrive_side != pobj->m_data.arrive_side)
				continue;

			if(obj_depart >= arrival && depart>=obj_arrival)
			{
				strText.Format("%s次和%s次在%s站股道冲突", single->GetTrainID(), pTrain->GetTrainID(), strStaName);
				precord->AddStplanWarnInfo(10, strText);
			}
		}
		if(strText != "")
			break;
	}

	return;
}

BOOL  CTGAutoAdjustLogic::CheckStplan(CTG_SingleTrain *single, long start_time, long end_time, CTG_TrainPtrGroup& group, CTG_ObjectPtrGroup& markgroup, int rec_index)
{
	CTG_TrainRecord *precord=single->GetTrainRecord(rec_index);
	if(NULL == precord) 
		return FALSE;

	precord->ClearStplanWarnInfo();

	ULONG alarm_flag=0;
	CString strText="";
	
	//检查车站禁停,必停和禁止通过
	CheckTrainPassAndStop(single, rec_index);
	
	// 检查列车与施工在区间冲突
	TIME etime=0;
	if(CheckTrain_ABSBlock(single,rec_index,alarm_flag,strText,etime,markgroup))
	{
		if(!strText.IsEmpty())
		{
			precord->AddStplanWarnInfo(4, strText);
		}
	}
	
	// 检查列车区间冲突
	CheckTrainABSMeetInfoForStplan(single,rec_index,alarm_flag,strText,start_time,end_time,group);
	// 检查超限列车站内相邻股道不能接车
	CheckTrainStaMeetInfo(single,rec_index, start_time, end_time,group,alarm_flag,strText);
		
	if(gpcdef.IsCtcStation(precord->m_data.station))  // 沈阳局TDCS车站不需要往下面检查计划 2014.05.19
	{
		if (!CWorkScheduleHolder::IsNeedCheckCtcModePlan(precord->m_data.station)) // 非常站控 和 车站控制
		{
			if(precord->HaveWarnInfo())
				return TRUE;
			else
				return FALSE;
		}
	}
	else
	{
		if(precord->HaveWarnInfo())  // TDCS 车站
			return TRUE;
		else
			return FALSE;
	} 
	// 分散自律 和 中心控制模式 需要检查站内
	
	//检查股道冲突
	if(CheckTrainStaSideInfo(single,rec_index, start_time, end_time,group,alarm_flag,strText))
	{
		if(!strText.IsEmpty())
		{
			precord->AddStplanWarnInfo(2, strText);
		}
	}
	CheckTrainSideInfoForStplan(single,rec_index,false);
	CheckTrainSideCrashForStplan(single,rec_index,start_time,end_time,group);
	
	if(precord->HaveWarnInfo())
		return TRUE;
	else
		return FALSE;
}

void CTGAutoAdjustLogic::CheckTrainSideInfoForStplan(CTG_SingleTrain *single, int rec_index, bool bTdcsCheck)
{
	CTG_TrainRecord *precord=single->GetTrainRecord(rec_index);
	if(!precord)
		return;
	TG_TrainRecord *rec=&precord->m_data; 
	TG_TrainInfo   *traininfo=&single->GetTrainInfo()->m_data;  

	const char* strStaName = gpcdef.GetStationName(precord->GetStationNo());
	const char* strTrainName = single->GetTrainID();
	// 股道封锁
	if(!strStaName || !strTrainName)
		return;
	
	CString strText;
	TIME sideusedstarttm=precord->GetArriveTime(); 
	TIME sideuseendtm = precord->GetDepartTime();
	if(CheckTrain_SideBlock(precord->GetStationNo(),precord->m_data.arrive_side,sideusedstarttm, sideuseendtm))
	{
		strText.Format("%s次在%s站接入封锁股道", strTrainName,strStaName);
		precord->AddStplanWarnInfo(11, strText);
	}

	if(bTdcsCheck && !gpcdef.IsCtcStation(precord->m_data.station))  // 沈阳局TDCS车站,在人工操作时不需要往下面检查计划 2014.05.19
	{
		return;
	}

	if(!CWorkScheduleHolder::IsCheckStationSide(precord->GetStationNo())) // TDCS车站仅检查封锁
		return;

	const CStationDef* pSideDef = m_pBasicPara->GetStationDef(rec->station);
	if(!pSideDef)
	{
		strText.Format("获取 %s 站细失败,请核对 %s 是否满足站细", strStaName, strTrainName);
		precord->AddStplanWarnInfo(12, strText);
		return;
	}

	SIDE_NAME arrive_side_name, depart_side_name;
	memset(arrive_side_name, 0, sizeof(arrive_side_name));
	memset(depart_side_name, 0, sizeof(depart_side_name));

	bool bStart=false, bTerminal=false;
	bool bArriveSideOK=true, bDepartSideOK=true;

	if(precord->IsStartRec() && (0==rec_index)) // 始发车
	{
		bStart=true;
	}
	if(precord->IsTerminalRec() && (single->GetTrainRecordCount()==rec_index+1)) // 终到车
	{
		bTerminal=true;
	}
	CStationDef::SIDE_T side;
	if(STA_FAILED == pSideDef->GetSideInfo(rec->arrive_side, side))
	{
		strcpy(arrive_side_name, "?G");
		bArriveSideOK=false;
		if(!bStart)
		{
			strText.Format("%s次在%s站未分配接车股道", strTrainName, strStaName);
			precord->AddStplanWarnInfo(15, strText);
		}
	}
	else
	{
		pSideDef->GetSideName(rec->arrive_side, arrive_side_name);
	}

	if(STA_FAILED == pSideDef->GetSideInfo(rec->depart_side, side))
	{
		strcpy(depart_side_name, "?G");
		bDepartSideOK=false;
		if(!bTerminal)
		{
			strText.Format("%s次在%s站未分配发车股道", strTrainName, strStaName);
			precord->AddStplanWarnInfo(16, strText);
		}
	}
	else
	{
		pSideDef->GetSideName(rec->depart_side, depart_side_name);
	}

	if(bArriveSideOK && bDepartSideOK && !bStart && !bTerminal)
	{
		if(rec->arrive_side != rec->depart_side)// 到达出发股道是否一致
		{
			if(single->IsCRHTrain())
			{
				strText.Format("旅客列车%s次在%s站到发股道不一致", strTrainName, strStaName); 
				precord->AddStplanWarnInfo(23, strText);
			}
			else
			{
				strText.Format("列车%s次在%s站到发股道不一致", strTrainName, strStaName); 
				precord->AddStplanWarnInfo(9, strText);
			}
		}
	}

	if(bStart) // 始发检查发车股道
	{
		if(bDepartSideOK)
		{
			// 检查超限车固定股道
			if (STA_FAILED==pSideDef->isReceiveExceedTrain(rec->depart_side, single->GetSpaceRank(rec_index)) )
			{
				strText.Format("超限列车%s次在%s站禁止由%s发车", strTrainName, strStaName, depart_side_name);
				precord->AddStplanWarnInfo(18, strText);
			}
			
			// 检查股道电力网
			if (single->IsElectricTrain())
			{
				if (STA_FAILED==pSideDef->isCatenary(rec->depart_side) )
				{
					strText.Format("%s次在%s站禁止由无接触网的%s发车", strTrainName, strStaName, depart_side_name);
					precord->AddStplanWarnInfo(20, strText);
				}
			}
			// 检查股道接发车性质
			BYTE nTrainType = single->IsPassengerTrain() ? CStationDef::RECV_PASSENGER : CStationDef::RECV_CARGO;
			if (STA_FAILED==pSideDef->isReceiveTrain(rec->depart_side, nTrainType) )
			{
				strText.Format("%s次在%s站禁止由%s发车,违反客货列车接发条件", strTrainName, strStaName, depart_side_name);
				precord->AddStplanWarnInfo(22, strText);
			}
			// 检查客车
			if ( single->IsPassengerTrain())
			{
				// 客运作业
				if (!TG_IsTechStopTrain(*rec))
				{
					if ( !pSideDef->isPlatform(rec->depart_side) )
					{
						strText.Format("%s站%s无站台,%s次不能办理客运作业", strStaName, depart_side_name, strTrainName);
						precord->AddStplanWarnInfo(25, strText);
					}
					else if (single->IsCRHTrain() && !pSideDef->isHighPlatform(rec->depart_side) )
					{
						strText.Format("动车组%s次在%s站接入非高站台股道", strTrainName, strStaName);
						precord->AddStplanWarnInfo(26, strText);
					}
				}
			}
		}
	}
	else if(bTerminal)
	{
		if(bArriveSideOK)
		{
			// 检查超限车固定股道
			if (STA_FAILED==pSideDef->isReceiveExceedTrain(rec->arrive_side, single->GetSpaceRank(rec_index)) )
			{
				strText.Format("超限列车%s次在%s站禁止接入%s", strTrainName, strStaName, arrive_side_name);
				precord->AddStplanWarnInfo(17, strText);
			}

			// 检查股道电力网
			if (single->IsElectricTrain())
			{
				if (STA_FAILED==pSideDef->isCatenary(rec->arrive_side) )
				{
					strText.Format("%s次在%s站禁止接入无接触网的%s", strTrainName, strStaName, arrive_side_name);
					precord->AddStplanWarnInfo(19, strText);
				}
			}

			// 检查股道接发车性质
			BYTE nTrainType = single->IsPassengerTrain() ? CStationDef::RECV_PASSENGER : CStationDef::RECV_CARGO;
			if (STA_FAILED==pSideDef->isReceiveTrain(rec->arrive_side, nTrainType) )
			{
				strText.Format("%s次在%s站禁止接入%s,违反客货列车接发条件", strTrainName, strStaName, arrive_side_name);
				precord->AddStplanWarnInfo(21, strText);
			}
			// 检查客车
			if ( single->IsPassengerTrain())
			{
				// 客运作业
				if (!TG_IsTechStopTrain(*rec))
				{
					if ( !pSideDef->isPlatform(rec->arrive_side) )
					{
						strText.Format("%s站%s无站台,%s次不能办理客运作业", strStaName, arrive_side_name, strTrainName);
						precord->AddStplanWarnInfo(25, strText);
					}
					else if (single->IsCRHTrain() && !pSideDef->isHighPlatform(rec->arrive_side) )
					{
						strText.Format("动车组%s次在%s站接入非高站台股道", strTrainName, strStaName);
						precord->AddStplanWarnInfo(26, strText);
					}
				}
			}
		}
	}
	else
	{
		// 检查超限车固定股道
		if (bArriveSideOK && STA_FAILED==pSideDef->isReceiveExceedTrain(rec->arrive_side, single->GetSpaceRank(rec_index)) )
		{
			strText.Format("超限列车%s次在%s站禁止接入%s", strTrainName, strStaName, arrive_side_name);
			precord->AddStplanWarnInfo(17, strText);
		}
		if (bDepartSideOK && STA_FAILED==pSideDef->isReceiveExceedTrain(rec->depart_side, single->GetSpaceRank(rec_index)) )
		{
			strText.Format("超限列车%s次在%s站禁止由%s发车", strTrainName, strStaName, depart_side_name);
			precord->AddStplanWarnInfo(18, strText);
		}

		// 检查股道电力网
		if (single->IsElectricTrain())
		{
			if (bArriveSideOK && STA_FAILED==pSideDef->isCatenary(rec->arrive_side) )
			{
				strText.Format("%s次在%s站禁止接入无接触网的%s", strTrainName, strStaName, arrive_side_name);
				precord->AddStplanWarnInfo(19, strText);
			}
			if (bDepartSideOK && STA_FAILED==pSideDef->isCatenary(rec->depart_side) )
			{
				strText.Format("%s次在%s站禁止由无接触网的%s发车", strTrainName, strStaName, depart_side_name);
				precord->AddStplanWarnInfo(20, strText);
			}
		}

		// 检查股道接发车性质
		BYTE nTrainType = single->IsPassengerTrain() ? CStationDef::RECV_PASSENGER : CStationDef::RECV_CARGO;
		if (bArriveSideOK && STA_FAILED==pSideDef->isReceiveTrain(rec->arrive_side, nTrainType) )
		{
			strText.Format("%s次在%s站禁止接入%s,违反客货列车接发条件", strTrainName, strStaName, arrive_side_name);
			precord->AddStplanWarnInfo(21, strText);
		}
		if (bDepartSideOK && (rec->arrive_side != rec->depart_side) && STA_FAILED==pSideDef->isReceiveTrain(rec->depart_side, nTrainType) )
		{
			strText.Format("%s次在%s站禁止由%s发车,违反客货列车接发条件", strTrainName, strStaName, depart_side_name);
			precord->AddStplanWarnInfo(22, strText);
		}

		// 检查客车
		if ( single->IsPassengerTrain())
		{
			// 侧线通过
			if (bArriveSideOK && bDepartSideOK && GetTGBasicParam()->IsTrainPassNotUseMainSide(precord->m_data))
			{
				strText.Format("%s次在%s站%s侧线通过", strTrainName, strStaName, arrive_side_name);
				precord->AddStplanWarnInfo(24, strText);
			}
			// 客运作业
			if (!precord->IsPassRec() && !TG_IsTechStopTrain(*rec))
			{
				if (bArriveSideOK)
				{
					if (!pSideDef->isPlatform(rec->arrive_side) )
					{
						strText.Format("%s站%s无站台,%s次不能办理客运作业", strStaName, arrive_side_name, strTrainName);
						precord->AddStplanWarnInfo(25, strText);
					}
					else if (single->IsCRHTrain() && !pSideDef->isHighPlatform(rec->arrive_side) )
					{
						strText.Format("动车组%s次在%s站接入非高站台股道", strTrainName, strStaName);
						precord->AddStplanWarnInfo(26, strText);
					}
				}
				if (bDepartSideOK && (rec->arrive_side != rec->depart_side))
				{
					if (!pSideDef->isPlatform(rec->depart_side) )
					{
						strText.Format("%s %s 无站台,客车 %s 不能办理客运作业", strStaName, depart_side_name, strTrainName);
						precord->AddStplanWarnInfo(25, strText);
					}
					else if (single->IsCRHTrain() && !pSideDef->isHighPlatform(rec->depart_side) )
					{
						strText.Format("动车组%s次在%s站接入非高站台股道", strTrainName, strStaName);
						precord->AddStplanWarnInfo(26, strText);
					}
				}
			}
		}
	}

	// 检查股道和出入口是否连通
	if(!bStart)
	{
		CStationDef::ENTRY_T entry;
		if (STA_FAILED == pSideDef->GetEntryInfo(rec->entry, entry))
		{
			strText.Format("%s次在%s站入口线别错误",strTrainName,strStaName); // stplan warn 20161019
			precord->AddStplanWarnInfo(13, strText);
		}
		else
		{
			if (bArriveSideOK && STA_FAILED==pSideDef->isEntrySideConnect(rec->arrive_side, rec->entry))
			{
				strText.Format("%s次在%s站不能经由%d口接入%s",strTrainName, strStaName, rec->entry, arrive_side_name);
				precord->AddStplanWarnInfo(27, strText);
			}
		}
	}

	if(!bTerminal)
	{
		CStationDef::ENTRY_T entry;
		if (STA_FAILED == pSideDef->GetEntryInfo(rec->exit, entry))
		{
			strText.Format("%s次在%s站出口线别错误",strTrainName,strStaName); // stplan warn 20161019
			precord->AddStplanWarnInfo(14, strText);
		}
		else
		{
			if (bDepartSideOK && STA_FAILED == pSideDef->isEntrySideConnect(rec->depart_side, rec->exit))
			{
				strText.Format("%s次在%s站不能经由%s发往%d口",strTrainName, strStaName, depart_side_name, rec->exit);
				precord->AddStplanWarnInfo(28, strText);
			}
		}
	}

	CTG_TrainRecord *pnextrec=single->GetTrainRecordNoDelete(rec_index+1);
	if(NULL != pnextrec)
	{
		ushort absnum1=1,absnum2=2;
		ushort dir1=0xffff,dir2=0xffff;

		if(m_pBasicPara->ConvertStationEntryToAbsnum(precord->GetStationNo(),precord->GetExit(),&absnum1,&dir1)
			&& m_pBasicPara->ConvertStationEntryToAbsnum(pnextrec->GetStationNo(),pnextrec->GetEntry(),&absnum2,&dir2))
		{
			if(absnum1!=absnum2)
			{
				STATION_NO station1=precord->GetStationNo();
				STATION_NO station2=pnextrec->GetStationNo();
				strText.Format("%s次无法从%s开到%s",strTrainName, gpcdef.GetStationName(station1), gpcdef.GetStationName(station2)); // stplan warn 20161019
				precord->AddStplanWarnInfo(3, strText);
			}
		}
	}

	return;
}

// 本函数仅用于自动调整的内部调用，功能是找出在指定节点的避让列车和最近的可用出发时刻
BOOL  CTGAutoAdjustLogic::__AdjustTrainDepartTimeByTrainCollision(CTG_SingleTrain *ptrain,int rec_index,int abs_run_time,CTG_SingleTrain *&initRelaTrain,TIME  &cau_depart,TIME &cau_next_arrive)
{
	TIME  begin_time,end_time;
    CTG_ObjectPtrGroup rela_trains;
	const CTG_TrainRecord *precord=ptrain->GetTrainRecordNoDelete_Const(rec_index);
	const CTG_TrainRecord *pnextrec=ptrain->GetTrainRecordNoDelete_Const(rec_index+1);
	if(!pnextrec || !precord)  
		return FALSE;
	if(abs_run_time==0)
		abs_run_time=pnextrec->m_data.arrive-precord->m_data.depart;
    if(cau_depart==0)
       cau_depart=precord->m_data.depart;  // 默认发车时刻
	if(cau_next_arrive==0)
	   cau_next_arrive=pnextrec->m_data.arrive; // 默认邻站到达

	begin_time=precord->GetDepartTime()-1800;
	end_time=NULL_TIME;
	m_pWorkSchedule->FindTrainsByStation(rela_trains,precord->GetStationNo(),begin_time,end_time,TRUE,TRUE);
	CTG_StationTrainStruct *pSpecRelaTrain=NULL; // 同向避让列车
		
	BOOL bHasCollision=FALSE;
	RUN_TYPE this_train_type=ptrain->IsPassengerTrain()?PASSANGER_TRAIN:FREIGHT_TRAIN;
	ARRIVE_DEPART_STATUS this_train_act=precord->IsPassRec()? STATION_PASS:STATION_DEPART;
	for(int i=0;i<rela_trains.GetCount();i++)
		{
			CTG_StationTrainStruct *pRelaTrain=(CTG_StationTrainStruct *)rela_trains.GetPtr(i);
           
			if(pRelaTrain->nTrainIndex == ptrain->GetTrainIndex())  // 忽略本列车
				continue;
			if(pRelaTrain->pTrain->GetPriority()>ptrain->GetPriority()) //忽略不比本列车优先级高的列车
				continue;

			if((pRelaTrain->pTrain->GetPriority()==ptrain->GetPriority())
				&& pRelaTrain->pTrain->IsAdjusting()) //忽略同等优先级但还正在调整中的列车（对于批量情况)
				continue;
			CTG_TrainRecord *prela_record=pRelaTrain->pTrain->GetTrainRecord(pRelaTrain->nRecIndex);
			if(!prela_record)
				continue;
			CTG_TrainRecord *pnextrecRela=pRelaTrain->pTrain->GetTrainRecord(pRelaTrain->nRecIndex+1);

			    // 得到追踪列车间隔
		 
		    ARRIVE_DEPART_STATUS rela_train_act=prela_record->IsPassRec()? STATION_PASS:STATION_DEPART;
	        RUN_TYPE rela_train_type=pRelaTrain->pTrain->IsPassengerTrain()?PASSANGER_TRAIN:FREIGHT_TRAIN;
	      
		   	int colli=-1;

			if(pRelaTrain->nEntry==precord->GetExit())   // 单线情况
			{
				int meet_gap_time=m_pBasicPara->GetTrainGapTime(precord->GetStationNo(), MEET_TRAIN_GAP, this_train_act, rela_train_act, this_train_type, rela_train_type);
                int stop_extra_time=m_pBasicPara->CalculateTrainStopExtra(pnextrec->GetStationNo(),pnextrec->GetEntry(),TRAIN_INTO_STATION,ptrain->GetRunType());

				if(cau_depart>=(pRelaTrain->tArriveTime-meet_gap_time) && cau_depart<=(pRelaTrain->tArriveTime+meet_gap_time)
					|| CheckNotToMeetWithTime(ptrain,pRelaTrain->pTrain,rec_index,pRelaTrain->nRecIndex,cau_depart,cau_next_arrive+stop_extra_time+meet_gap_time)) 
				{
					 if(cau_depart<pRelaTrain->tArriveTime+meet_gap_time)
					 {
						cau_depart=pRelaTrain->tArriveTime+meet_gap_time;
						initRelaTrain=pRelaTrain->pTrain;
					}
					 if(cau_next_arrive<cau_depart+abs_run_time)
						cau_next_arrive=cau_depart+abs_run_time;
					 colli+=2;
				}
				else
					 colli=0;
			}

			if(pRelaTrain->nExit==precord->GetExit())  // 同向发车情况
			{
				// 得到配置的同向发车间隔
				int depart_before_gap_time=m_pBasicPara->GetTrainGapTime(precord->GetStationNo(), CONTINUE_DEPART_GAP, this_train_act, rela_train_act, this_train_type, rela_train_type);
			    int depart_after_gap_time=m_pBasicPara->GetTrainGapTime(precord->GetStationNo(), CONTINUE_DEPART_GAP, this_train_act, rela_train_act, this_train_type, rela_train_type);
				int arrive_gap_time=0; // 邻站接车间隔
				if(pnextrecRela && pnextrecRela->GetStationNo()==pnextrec->GetStationNo())
					arrive_gap_time=m_pBasicPara->GetTrainGapTime(pnextrec->GetStationNo(), DIFF_ARRIVE_GAP,
						pnextrecRela->IsPassRec()? STATION_PASS:STATION_DEPART,
						pnextrec->IsPassRec()? STATION_PASS:STATION_DEPART,
						pRelaTrain->pTrain->IsPassengerTrain()?PASSANGER_TRAIN:FREIGHT_TRAIN,
						ptrain->IsPassengerTrain()?PASSANGER_TRAIN:FREIGHT_TRAIN
					);

				if(cau_depart<=(prela_record->m_data.depart+depart_after_gap_time)     
					&& cau_depart>=(prela_record->m_data.depart-depart_before_gap_time)  // 同向发车间隔参数
					|| arrive_gap_time>0 && cau_next_arrive>=(pnextrecRela->m_data.arrive-arrive_gap_time)     // 同向接车间隔
					&& cau_next_arrive<=(pnextrecRela->m_data.arrive+arrive_gap_time)
					||	CheckNotToMeetWithTime(ptrain,pRelaTrain->pTrain,rec_index,pRelaTrain->nRecIndex,cau_depart,cau_next_arrive)) 

				{
				     if(cau_depart<pRelaTrain->tDepartTime+depart_after_gap_time)
					 {
						cau_depart=pRelaTrain->tDepartTime+depart_after_gap_time;
						initRelaTrain=pRelaTrain->pTrain;
					 }
					 colli+=2;
					 if(cau_next_arrive<pnextrecRela->m_data.arrive+arrive_gap_time) // 同向接车间隔
						  cau_next_arrive=pnextrecRela->m_data.arrive+arrive_gap_time;	
				}
				else
				if(colli==-1) colli=0;
			}
			if(colli==0 && pRelaTrain->tDepartTime>precord->GetDepartTime())  // 遇到比本身列车发车晚，但无冲突则停止
				break;
			
			if(colli>0)
				bHasCollision=TRUE;
		}
	 return TRUE;
}

// 如果min_stop_time为0, 则默认通过,
// 从 begin_index记录的出发开始调整
BOOL  CTGAutoAdjustLogic::__AdjustTrainForCollision__(CTG_SingleTrain *ptrain,int begin_index) 
{
	if(ptrain==NULL)
		return false;
	if(ptrain->IsPassengerTrain())  // 2012.05.02 不能自动调整旅客列车
		return false;

	CTG_SingleTrain train=*ptrain;

	// 1. 调整列车避让时间
	int rec_index=begin_index;
	CTG_SingleTrain *pRelaTrain=NULL;
	int depart_delay_for_block_abs=GetTGFunctionConfig()->GetDepartDelayTimeOfBlockedABS();
	BOOL bAdjusted=FALSE;
	while(rec_index<ptrain->GetTrainRecordCount()-1)
	{
		CTG_TrainRecord *precord=ptrain->GetTrainRecord(rec_index);
		TIME cau_depart;
		TIME cau_next_arrive;

		// 得到下一邻站记录，以及默认区间时分
		CTG_TrainRecord *pnextrec=ptrain->GetTrainRecord(rec_index+1);
		if(!pnextrec)  
			break;
		int abs_run_time = pnextrec->m_data.arrive-precord->m_data.depart;
        	
    
	    BOOL back_trace=FALSE;

		//此处应该初始化,否则当下面循环未执行时,异常
		cau_depart=precord->m_data.depart;
	    cau_next_arrive=pnextrec->m_data.arrive; 

		for(int step=0;step<2;step++)
		{
			if(!GetTGFunctionConfig()->GetAutoAdjustStepFlag(step))  //根据配置决定
				continue;
			cau_depart=precord->m_data.depart;
		    cau_next_arrive=pnextrec->m_data.arrive; 
			if(step==CTGFunctionConfig::ADJUST_FLAGBIT_BLOCK) // 首先考虑封锁区间情况
			{
			  CTG_ObjectPtrGroup markgroup;
			  m_pWorkSchedule->FindMarks(markgroup);
		      unsigned long alarm_flag=0; // dumy
		      CString       strText;  // dumy
		      CheckTrain_ABSBlock(ptrain,rec_index,alarm_flag,strText,cau_depart,markgroup);
			  if(cau_depart>precord->m_data.depart)
				 cau_depart+=depart_delay_for_block_abs;
			}
			else 
				continue;

			 CTG_ObjectPtrGroup meettrains;
			if(cau_depart>precord->m_data.arrive && precord->m_data.min_stop_time==0 &&
				(cau_depart-precord->m_data.arrive<300) && // 停车小于5分钟
				!m_pWorkSchedule->GetStationTrainList()->FindMeetTrains(meettrains,rec_index,ptrain)) // 停站没有避让列车,并且也没有最小停站时间,那么通过
			 {
				precord->m_data.arrive=cau_depart;
				back_trace=TRUE;   
		 	 }
			else if(precord->IsPassRec() && cau_depart>precord->m_data.depart)  // 由于避让，导致通过变到开
		     {
			    precord->m_data.arrive+=m_pBasicPara->CalculateTrainStopExtra(precord->GetStationNo(),precord->GetEntry(),TRAIN_INTO_STATION,ptrain->GetRunType());
				back_trace=TRUE;   
			 }
			 
			int offset=cau_depart-precord->m_data.depart;
			if(offset>0)
			{
				ptrain->OffsetScheduleTime(rec_index,offset,FALSE,0);
				bAdjusted=TRUE;
			}
		}

		int offset=cau_next_arrive-pnextrec->m_data.arrive;
		if((offset+pnextrec->m_data.arrive-precord->m_data.depart)>(abs_run_time+180))  // 如果偏差过大(超过原区间时分3分钟），则应该在前面记录中增加停站时间
		{
			int tmp_index=rec_index;
			while(tmp_index>begin_index)
			{
               CTG_TrainRecord *ptmp_rec=ptrain->GetTrainRecord(tmp_index);
			   if(!ptmp_rec->IsPassRec())
			       break;
			   tmp_index--;
			}
			ptrain->OffsetScheduleTime(tmp_index,offset,FALSE,0);
			rec_index=tmp_index;
			continue;  // 重新返回调整
		}
		
		if(offset>0)
		{
			ptrain->OffsetScheduleTime(rec_index+1,offset,FALSE,1);
		}
		rec_index++;
	}

	
	// 2. 根据列车避让后到发关系变化分配股道
	for(int i=begin_index; i<train.GetTrainRecordCount(); i++)
	{
		CTG_TrainRecord *poldrecord=train.GetTrainRecord(i);
		CTG_TrainRecord *pnewrecord=ptrain->GetTrainRecord(i);

		if(poldrecord==NULL || pnewrecord==NULL)
			break;
		
		if(poldrecord->GetStationNo() != pnewrecord->GetStationNo())
			break;

		if(poldrecord->IsPassRec())
		{
			if(!pnewrecord->IsPassRec())
			{
				GpcLogTrainMessage(0, 0, "使用自动调整功能, 列车 %s (%d)在 %d  站由通过->到开,股道自动调整",
									ptrain->GetTrainID(), ptrain->GetTrainIndex(), pnewrecord->GetStationNo());
			
				ChangeSideForStopOrPass(ptrain,i);  // 股道自动调整
			}
		}
		else
		{
			if(pnewrecord->IsPassRec())
			{
				GpcLogTrainMessage(0, 0, "使用自动调整功能, 列车 %s (%d)在 %d   站由到开->通过, 股道自动调整",
									ptrain->GetTrainID(), ptrain->GetTrainIndex(), pnewrecord->GetStationNo());
			
				ChangeSideForStopOrPass(ptrain,i); // 股道自动调整
			}
		}
	}

	if(rec_index>=ptrain->GetTrainRecordCount()-1)
		return TRUE;
	else
		return FALSE;
}

BOOL  CTGAutoAdjustLogic::AdjustTrainForCollision(CTG_SingleTrain *ptrain,int begin_index) //调整以避开冲突情况
{
	int rec_index=ptrain->GetTrainRecordCount()-1;
	if(rec_index<=begin_index) // 最后一个节点无需调整
		return FALSE; 
	// 有实际点的记录不参与调整
	CTG_TrainRecord *precord=NULL;
	while(rec_index>begin_index)
	{
		precord=ptrain->GetTrainRecord(rec_index);
		if(precord->IsActualDepartTimeRecord() || precord->IsActualArriveTimeRecord())
			break;
		rec_index--;
	}
	if(precord==NULL)
		return FALSE;

	if(precord->IsActualDepartTimeRecord())
		rec_index++;
	
	return __AdjustTrainForCollision__(ptrain,rec_index);
}

    // 本函数对
	// 检查在指定时间的停站股道，arriveTime初始值是预计到达时刻，avaSide初始值是默认股道
	// 如果 forceSide 为TRUE,则arriveTime返回 avaSide可用的时刻
	// 如果 forceSide 为FALSE,则avaSide返回最早可用的股道，arriveTime返回可用的时刻
	// 函数返回 TRUE, 表明上述返回值有效，否则无效

BOOL  CTGAutoAdjustLogic::CacuAvailableSideForPlanStop(CTG_SingleTrain *ptrain, int recIndex,TIME &arriveTime,int &avaSide,BOOL forceSide)
{
	int trainType,station,entry,exit,priority;
	SIDE_ID ava_sides[MAX_SIDE_COUNT];
	TIME    ava_time[MAX_SIDE_COUNT];

	short  side_hotel_count=MAX_SIDE_COUNT;

	CTG_TrainRecord *precord=ptrain->GetTrainRecord(recIndex);
	if(!precord)
		return FALSE;
    CTG_TrainInfo train_info=*ptrain->GetTrainInfo();
	if(!m_pBasicPara->GetStopOrderSide(train_info.m_data,precord->m_data,ava_sides,&side_hotel_count)) //查找符合基本条件的股道
		return FALSE;
    if(side_hotel_count<=0)
		return FALSE;
	for(int i=0;i<side_hotel_count;i++)   // 假定都可用
		ava_time[i]=arriveTime;

	station=precord->GetStationNo();
	trainType=ptrain->GetTrainType();
	entry=precord->GetEntry();
	exit=precord->GetExit();
	priority=ptrain->GetPriority();

	TIME begin_time,end_time;
	begin_time=arriveTime;
	end_time=begin_time+3600*3;  // 假定查找3小时后的列车无意义
	CTG_ObjectPtrGroup rela_trains;
	m_pWorkSchedule->FindTrainsByStation(rela_trains,station,begin_time,end_time,FALSE,TRUE);
	for(int i=0;i<rela_trains.GetCount();i++)
		{
			CTG_StationTrainStruct *pRelaTrain=(CTG_StationTrainStruct *)rela_trains.GetPtr(i);
           
			if(pRelaTrain->nTrainIndex == ptrain->GetTrainIndex())  // 忽略本列车
				continue;
			if(pRelaTrain->pTrain->GetPriority()>priority)  // 或略优先级低的列车
				continue;
			const CTG_TrainRecord *prela_rec=pRelaTrain->pTrain->GetTrainRecordNoDelete_Const(pRelaTrain->nRecIndex);
			if(!prela_rec)
				continue;
			if(prela_rec->GetArriveTime()>arriveTime)  // 不在此车站交会，或略
				continue;
			int x=0;
			for(x=0;x<side_hotel_count;x++)
			{
				if(ava_sides[x]==prela_rec->m_data.arrive_side)
                     break;
			}
			if(x>=side_hotel_count)
				continue;
			int depart_gap_time=m_pBasicPara->GetTrainGapTime(precord->GetStationNo(), OCCUPY_SAME_SIDE_GAP,
					prela_rec->IsPassRec()? STATION_PASS:STATION_DEPART,
					precord->IsPassRec()? STATION_PASS:STATION_DEPART,
					pRelaTrain->pTrain->IsPassengerTrain()?PASSANGER_TRAIN:FREIGHT_TRAIN,
					ptrain->IsPassengerTrain()?PASSANGER_TRAIN:FREIGHT_TRAIN);
			ava_time[x]=prela_rec->m_data.depart+depart_gap_time; // 出发间隔？
		}
	arriveTime=NULL_TIME;
	for(int x=0;x<side_hotel_count;x++)
	{
		if(forceSide && ava_sides[x]!=avaSide)
			continue;
        if(ava_time[x]<arriveTime || arriveTime==NULL_TIME)
		{
			avaSide=ava_sides[x];
			arriveTime=ava_time[x];
		}
	}
	if(	arriveTime==NULL_TIME)
		return FALSE;
	else
		return TRUE;
}

BOOL  CTGAutoAdjustLogic::AdjustTrainForMutiQuickPass(CTG_SingleTrain *ptrain, int begin_index, int con_id)
{
	for(int i=begin_index; i<ptrain->GetTrainRecordCount(); i++)
	{
		CTG_TrainRecord* rec = ptrain->GetTrainRecord(i);
		if (NULL == rec)
			continue;
		
		// 实际点
		if(rec->IsActualArriveTimeRecord() || rec->IsActualDepartTimeRecord())
			continue;

		if (TG_IsArriveRouteDoing(rec->m_data) || 
			TG_IsArriveRouteOk(rec->m_data) || 
			TG_IsArriveRouteOccupy(rec->m_data))
		{
			GpcLogTrainMessage(0, 0, "多点调整, 列车%s在%s站 接车进路正在排出或者已排出或者占用, 不能变为通过",
				ptrain->GetTrainID(), gpcdef.GetStationName(rec->GetStationNo()));

			continue;
		}
		if (TG_IsDepartRouteDoing(rec->m_data) || 
			TG_IsDepartRouteOk(rec->m_data) || 
			TG_IsDepartRouteOccupy(rec->m_data))
		{
			GpcLogTrainMessage(0, 0, "多点调整, 列车%s在%s站 发车进路正在排出或者已排出或者占用, 不能变为通过",
				ptrain->GetTrainID(), gpcdef.GetStationName(rec->GetStationNo()));

			continue;
		}
		if (TG_IsTrainIntoSide(rec->m_data))
		{
			GpcLogTrainMessage(0, 0, "多点调整, 列车%s在%s站 已经进站, 不能变为通过",
				ptrain->GetTrainID(), gpcdef.GetStationName(rec->GetStationNo()));

			continue;
		}

		bool bMustStop=false;
		if(TG_IsMustStop(rec->m_data))
		{
			GpcLogTrainMessage(0, 0, "多点调整, 列车%s在%s站 设置为必须停车, 不能变为通过",
				ptrain->GetTrainID(), gpcdef.GetStationName(rec->GetStationNo()));
			bMustStop=true;
		}

		if(TG_IsMustNoPass(rec->m_data))
		{
			GpcLogTrainMessage(0, 0, "多点调整, 列车%s在%s站 设置为禁止通过, 不能变为通过",
				ptrain->GetTrainID(), gpcdef.GetStationName(rec->GetStationNo()));
			bMustStop=true;
		}

		// 有作业时分
		if(rec->m_data.min_stop_time>0)
		{
			GpcLogTrainMessage(0, 0, "多点调整, 列车%s在%s站 最小作业时分为%d, 不能变为通过",
				ptrain->GetTrainID(), gpcdef.GetStationName(rec->GetStationNo()), rec->m_data.min_stop_time);
			bMustStop=true;
		}

		// 非技术停点
		if( !TG_IsTechStopTrain(rec->m_data))
		{
			GpcLogTrainMessage(0, 0, "多点调整, 列车%s在%s站 有客运作业, 不能变为通过",
				ptrain->GetTrainID(), gpcdef.GetStationName(rec->GetStationNo()));
			bMustStop=true;
		}
		
		if(GetTGFunctionConfig()->IsNotThisSectionStation(con_id,  rec->m_data.station))
		{
			GpcLogTrainMessage(0, 0, "请注意, %s 不属于本台管辖,不能修改到开方式", gpcdef.GetStationName(rec->m_data.station));
			bMustStop=true;
		}

		CTG_TrainRecord *pprecrec=ptrain->GetTrainRecord(i-1);
		if(NULL==pprecrec)
			continue;

		int abs_run_time = m_pBasicPara->CalculateTrainRunTime(pprecrec->GetStationNo(),pprecrec->GetExit(),TRAIN_OUT_STATION,ptrain->GetRunType());
		if(rec->IsPassRec())
		{
			if(bMustStop)
			{
				AdjustForStopPassChange(ptrain, i, FALSE);
			}
			else
			{
				int start_extra_time=0;
				if(!pprecrec->IsPassRec() || ((i==0)&&pprecrec->IsStartRec()))
					start_extra_time=m_pBasicPara->CalculateTrainStartExtra(pprecrec->GetStationNo(),pprecrec->GetExit(),TRAIN_OUT_STATION,ptrain->GetRunType());

				rec->m_data.arrive = pprecrec->m_data.depart + start_extra_time + abs_run_time;
				rec->m_data.depart = rec->m_data.arrive;
			}
		}
		else
		{
			if(bMustStop)
			{
				int start_extra_time=0;
				if(!pprecrec->IsPassRec() || ((i==0)&&pprecrec->IsStartRec()))
					start_extra_time=m_pBasicPara->CalculateTrainStartExtra(pprecrec->GetStationNo(),pprecrec->GetExit(),TRAIN_OUT_STATION,ptrain->GetRunType());

				int stop_extra_time = m_pBasicPara->CalculateTrainStopExtra(rec->GetStationNo(),rec->GetEntry(),TRAIN_INTO_STATION,ptrain->GetRunType());
		
				TIME stop_time = rec->m_data.depart - rec->m_data.arrive;
				rec->m_data.arrive = pprecrec->m_data.depart + start_extra_time + abs_run_time + stop_extra_time;
				rec->m_data.depart = rec->m_data.arrive+stop_time;
			}
			else
			{
				AdjustForStopPassChange(ptrain, i, TRUE);
			}
		}
	}

	return TRUE;
}

BOOL  CTGAutoAdjustLogic::GetAbsMarkStation(const CTG_SingleMark *pmark,int &station1,int &station2)
{
	if(pmark->IsStationMarkType())
		return FALSE;
	if(pmark->m_data.ele_count<=0)
		return FALSE;
	int absnum = pmark->m_data.ele_number[0]; // 此处选择第一个区间，假定所有区间对应车站是相同的

	ushort stat1=0,stat2=0,entry1=0,entry2=0;
	if(!m_pBasicPara->ConvertAbsnumToStationEntry(&stat1,&entry1,&stat2,&entry2,absnum))
		return FALSE;
	station1=stat1;
	station2=stat2;
	return TRUE;
    
}
//查找与mark关联的车次,注意返回的是 CTG_StationTrainStruct类型的指针数组，处理时注意为空的要跳过
BOOL  CTGAutoAdjustLogic::GetRelaTrainsOfMark(const CTG_SingleMark *pmark,CTG_ObjectPtrGroup &rela_trains) 
{
	TIME  begin_time=pmark->GetStartTime();
	TIME  end_time=pmark->GetEndTime();

	int mark_type=pmark->GetMarkType();
	if(mark_type==TG_SIDE_INHIBIT || mark_type==TG_SIDE_TIANCHUANG || mark_type==TG_SIDE_OCCUPY) //股道限制
	{
	   	m_pWorkSchedule->FindTrainsByStation(rela_trains,pmark->GetStation(),begin_time,end_time,TRUE,FALSE); //只是查找，在范围内到达时间
       for(int i=0;i<rela_trains.GetCount();)
	   {
		   CTG_StationTrainStruct *pRelaTrain=(CTG_StationTrainStruct *)rela_trains.GetPtr(i);
		   CTG_TrainRecord *precord=pRelaTrain->pTrain->GetTrainRecord(pRelaTrain->nRecIndex);
		   if(!precord || (IS_VALID_SIDE(precord->m_data.arrive_side) && !pmark->MatchStationSide(precord->GetStationNo(),precord->m_data.arrive_side)))
				rela_trains.RemovePtr(i);
		   else
			    i++;
	   }
	}
	else
	if(mark_type==TG_ABS_INHIBIT || mark_type==TG_ABS_FIX_MARK  || mark_type==TG_SECTION_ELEC_FIX) // 区间限制
	{
	  int  station,station1,station2;
	  if(!GetAbsMarkStation(pmark,station1,station2))
		  return FALSE;
	  for(int step=0;step<2;step++)
	  {
	   if(step==0)
	      station=station1;
	   else
		  station=station2;
       m_pWorkSchedule->FindTrainsByStation(rela_trains,station,begin_time,end_time,FALSE,TRUE); //只是查找，在范围内出发时间
      }
	   ushort abs_num=0;
	   ushort dir=0xffff;
	   for(int i=0;i<rela_trains.GetCount();)
	   {
	      CTG_StationTrainStruct *pRelaTrain=(CTG_StationTrainStruct *)rela_trains.GetPtr(i);
		   CTG_TrainRecord *precord=pRelaTrain->pTrain->GetTrainRecord(pRelaTrain->nRecIndex);
	      if(!precord 
			  || !m_pBasicPara->ConvertStationEntryToAbsnum(precord->GetStationNo(),precord->GetExit(),&abs_num,&dir)
			  || !pmark->MatchAbsNumber(abs_num))
			    rela_trains.RemovePtr(i);
		  else
			    i++;
	   }
	}
	if(rela_trains.GetCount()>0)
		return TRUE;
	else
		return FALSE;
}

BOOL  CTGAutoAdjustLogic::AdjustSingleTrainForMark(CTG_SingleTrain &train,int recIndex,const CTG_SingleMark *pmark)
{
	int mark_type=pmark->GetMarkType();
	if(mark_type==TG_SIDE_INHIBIT || mark_type==TG_SIDE_TIANCHUANG || mark_type==TG_SIDE_OCCUPY) //股道限制
	{
		ChangeSideForStopOrPass(&train,recIndex,1,0); // 重新计算股道，应该还要比较一下是否确实变过了
		CTG_TrainRecord* prec=train.GetTrainRecordByRecIndex(recIndex);
		if(prec)
		{
			GpcLogTrainMessage(0, 0, "[股道分配 计算]: %s(%d)车站%d(entry %d exit %d), %d %d", 
				train.GetTrainID(),train.GetTrainIndex(),prec->m_data.station, prec->m_data.entry, prec->m_data.exit, prec->m_data.arrive_side, prec->m_data.depart_side);
		}
	}
	else
	if(mark_type==TG_ABS_INHIBIT || mark_type==TG_ABS_FIX_MARK  || mark_type==TG_SECTION_ELEC_FIX) // 区间限制
	{
	   AdjustTrainForCollision(&train,recIndex);
	}
	return TRUE;
}
