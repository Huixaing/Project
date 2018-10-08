//
#include "stdafx.h"
#include "Tg_TRAIN.h"

//-----------------------------------------------------------------------
bool ABS_STOP::IsAbsStopCoreSame(const ABS_STOP& rhs)
{
	if(flag != rhs.flag)
		return false;
	if(position != rhs.position)
		return false;
	if(start_time != rhs.start_time)
		return false;
	if(stop_time != rhs.stop_time)
		return false;

	return true;
}

TgTRAIN_RECORD::TgTRAIN_RECORD()
{
	Clear();
}
TgTRAIN_RECORD::TgTRAIN_RECORD(const TgTRAIN_RECORD& rhs)
{
	Copy(rhs);
}

TgTRAIN_RECORD& TgTRAIN_RECORD::operator =(const TgTRAIN_RECORD& rhs)
{
	if(this == &rhs)
		return (*this);

	Copy(rhs);

	return *this;
}
TgTRAIN_RECORD::~TgTRAIN_RECORD()
{
}

CString TgTRAIN_RECORD::GetStaAlarmText()
{
	CString strStaAndAlarmText = "";
	if(!this->szStationText.IsEmpty() && !this->szAlarmText.IsEmpty())
		strStaAndAlarmText = this->szStationText + "--" + this->szAlarmText;
	else
	{
		if(!this->szStationText.IsEmpty())
			strStaAndAlarmText = this->szStationText;
		else
		if(!this->szAlarmText.IsEmpty())
			strStaAndAlarmText = this->szAlarmText;
		else
			strStaAndAlarmText = "";
	}

	return strStaAndAlarmText;
}

void TgTRAIN_RECORD::Clear()
{
	opmsGiveStationText = 1;  //OPMS给的值 1
	szOpmsStationText = "";
	arrive_train_id.Empty();
	depart_train_id.Empty();
	original_arrive_train_id.Empty();
	original_depart_train_id.Empty();
	this->route_flag = 0;

	// 索引
	this->rec_index = 0;

	this->station = 0;
	this->entry = 0;
	this->exit = 0;

	this->arrive_side = 255;
	this->plan_arrive_side = 255;
	this->depart_side = 255;
	this->plan_depart_side = 255;

	this->min_stop_time = 0;

	arrive = 0;
	depart = 0;
	plan_arrive = 0;
	plan_depart = 0;

	this->stop_reason = 0;
	this->late_reason = 0;
	this->early_depart_time = 0;
	this->run_fast_slow_time = 0;

	this->speed_limit = 0;
	this->stop_condition = 0;
	this->adjust_status = 0;
	this->spec_run_time = 0;
	
	this->super_interval_ltime = 0;

	this->stop_abs_position = 0;
	this->stop_abs_s_time = 0;
	this->stop_abs_e_time = 0;
	this->special_train_index = 0;

	this->data_source = 0;
	this->flag = 0;
	this->no_meet_reason = 0;
	this->modify_abs_runtime = 0;

	this->in_station_index = -1;
	this->out_station_index = -1;
	this->rank = 0;
	display_status=0;
	this->in_sta_direct = -1;
	this->out_sta_direct = -1;
	
	this->static_arrive=0;
	this->static_depart=0;
	this->static_manual=0;

	abs_stop.clear();
	//------------------------------------
	str_arrive_side="";
	str_depart_side="";
	line_type=21;
	factor=255;
	schd_color=RGB(0,255,0);
	real_color=RGB(0,200,0);
	schd_vert_color=RGB(0,160,0);
	real_vert_color=RGB(0,160,0);
	schd_width=1;
	real_width=2;
	nEngineCnt=0;
	
	bShowSecond=false;   
}

void TgTRAIN_RECORD::Copy(const TgTRAIN_RECORD& rhs)
{
	//-----新增--------------------------------------
	opmsGiveStationText = rhs.opmsGiveStationText;
	szOpmsStationText = rhs.szOpmsStationText;
	szAlarmText = rhs.szAlarmText;
	szStationText = rhs.szStationText;
	szAbsText = rhs.szAbsText;
	szPlanNote = rhs.szPlanNote;

	arrive_train_id = rhs.arrive_train_id;
	depart_train_id = rhs.depart_train_id;
	original_arrive_train_id = rhs.original_arrive_train_id;
	original_depart_train_id = rhs.original_depart_train_id;
	this->route_flag = rhs.route_flag;

	// 索引
	this->rec_index = rhs.rec_index;

	this->station = rhs.station;
	this->entry = rhs.entry;
	this->exit = rhs.exit;

	this->arrive_side = rhs.arrive_side;
	this->plan_arrive_side = rhs.plan_arrive_side;
	this->depart_side = rhs.depart_side;
	this->plan_depart_side = rhs.plan_depart_side;

	this->min_stop_time = rhs.min_stop_time;

	this->arrive = rhs.arrive;
	this->depart = rhs.depart;

	this->plan_arrive = rhs.plan_arrive;
	this->plan_depart = rhs.plan_depart;

	this->stop_reason = rhs.stop_reason;
	this->late_reason = rhs.late_reason;
	this->early_depart_time = rhs.early_depart_time;
	this->run_fast_slow_time = rhs.run_fast_slow_time;

	this->speed_limit = rhs.speed_limit;
	this->stop_condition = rhs.stop_condition;
	this->adjust_status = rhs.adjust_status;
	this->spec_run_time = rhs.spec_run_time;

	this->super_interval_ltime = rhs.super_interval_ltime;

	this->stop_abs_position = rhs.stop_abs_position;
	this->stop_abs_s_time = rhs.stop_abs_s_time;
	this->stop_abs_e_time = rhs.stop_abs_e_time;
	this->special_train_index = rhs.special_train_index;

	this->data_source = rhs.data_source;
	this->flag = rhs.flag;
	this->no_meet_reason = rhs.no_meet_reason;
	this->modify_abs_runtime = rhs.modify_abs_runtime;

	this->in_station_index = rhs.in_station_index;
	this->out_station_index = rhs.out_station_index;
	this->rank = rhs.rank;
	display_status = rhs.display_status;

	this->in_sta_direct = rhs.in_sta_direct;
	this->out_sta_direct = rhs.out_sta_direct;

	this->static_arrive=rhs.static_arrive;
	this->static_depart=rhs.static_depart;
	this->static_manual=rhs.static_manual;

	abs_stop.clear();
	for(UINT idx = 0; idx < rhs.abs_stop.size(); idx++)
		abs_stop.push_back(rhs.abs_stop[idx]);
	//------------------------------------
	str_arrive_side=rhs.str_arrive_side;
	str_depart_side=rhs.str_depart_side;
	line_type=rhs.line_type;
	factor=rhs.factor;
	schd_color=rhs.schd_color;
	real_color=rhs.real_color;
	schd_vert_color=rhs.schd_vert_color;
	real_vert_color=rhs.real_vert_color;
	schd_width=rhs.schd_width;
	real_width=rhs.real_width;
	nEngineCnt=rhs.nEngineCnt;
	
}

void TgTRAIN_RECORD::InitRecordBasic()
{
	opmsGiveStationText = 1;
	szOpmsStationText = "";
	route_flag = 0;
	min_stop_time = 0;
	early_depart_time = 0;
	run_fast_slow_time = 0;

	speed_limit = 0;
	stop_condition = 0;
	adjust_status = 16;
	spec_run_time = 0;

	stop_abs_position = 0;
	stop_abs_s_time = 0;
	stop_abs_e_time = 0;
	special_train_index = 0;

	data_source = 0;
	no_meet_reason = 0;
	modify_abs_runtime = 0;

	static_arrive=0;
	static_depart=0;
	static_manual=0;
	abs_stop.clear();
}
BYTE TgTRAIN_RECORD::GetArriveSpace()  const
{  
	return TG_GetArriveSpace(rank);
}

BYTE TgTRAIN_RECORD::GetDepartSpace() const
{  
	return TG_GetDepartSpace(rank);
}

bool TgTRAIN_RECORD::IsRecordCoreSame(const TgTRAIN_RECORD& rhs)
{
	if(abs_stop.size() != rhs.abs_stop.size())
		return false;

	for(int i=0; i<abs_stop.size(); i++)
	{
		if(!abs_stop[i].IsAbsStopCoreSame(rhs.abs_stop[i]))
			return false;
	}

	if(opmsGiveStationText != rhs.opmsGiveStationText)
		return false;
	if(szOpmsStationText != rhs.szOpmsStationText)
		return false;
	if(szAlarmText != rhs.szAlarmText)
		return false;
	if(szStationText != rhs.szStationText)
		return false;
	if(szAbsText != rhs.szAbsText)
		return false;
	if(szPlanNote != rhs.szPlanNote)
		return false;
	
	if(arrive_train_id != rhs.arrive_train_id)
		return false;
	if(depart_train_id != rhs.depart_train_id)
		return false;
	if(original_arrive_train_id != rhs.original_arrive_train_id)
		return false;
	if(original_depart_train_id != rhs.original_depart_train_id)
		return false;
	if(route_flag != rhs.route_flag)
		return false;

	if(rec_index != rhs.rec_index)
		return false;
	if(station != rhs.station)
		return false;
	if(entry != rhs.entry)
		return false;
	if(exit != rhs.exit)
		return false;

	if(arrive_side != rhs.arrive_side)
		return false;
	if(plan_arrive_side != rhs.plan_arrive_side)
		return false;
	if(depart_side != rhs.depart_side)
		return false;
	if(plan_depart_side != rhs.plan_depart_side)
		return false;

	if(min_stop_time != rhs.min_stop_time)
		return false;

	if(arrive != rhs.arrive)
		return false;
	if(depart != rhs.depart)
		return false;

	if(plan_arrive != rhs.plan_arrive)
		return false;
	if(plan_depart != rhs.plan_depart)
		return false;

	if(stop_reason != rhs.stop_reason)
		return false;
	if(late_reason != rhs.late_reason)
		return false;
	if(early_depart_time != rhs.early_depart_time)
		return false;
	if(run_fast_slow_time != rhs.run_fast_slow_time)
		return false;

	
	if(speed_limit != rhs.speed_limit)
		return false;
	if(stop_condition != rhs.stop_condition)
		return false;
	if(adjust_status != rhs.adjust_status)
		return false;
	if(spec_run_time != rhs.spec_run_time)
		return false;

	if(stop_abs_position != rhs.stop_abs_position)
		return false;
	if(stop_abs_s_time != rhs.stop_abs_s_time)
		return false;
	if(stop_abs_e_time != rhs.stop_abs_e_time)
		return false;
	if(special_train_index != rhs.special_train_index)
		return false;

	if(data_source != rhs.data_source)
		return false;
	if(flag != rhs.flag)
		return false;
	if(no_meet_reason != rhs.no_meet_reason)
		return false;
	if(in_station_index != rhs.in_station_index)
		return false;

	if(out_station_index != rhs.out_station_index)
		return false;
	if(rank != rhs.rank)
		return false;
	if(display_status != rhs.display_status)
		return false;
	if (super_interval_ltime != rhs.super_interval_ltime)
		return false;


	return true;
}
CString TgTRAIN_RECORD::toXML()
{	
	CString xml,tmp;

	xml = "<RECORD";
	
	tmp.Format(" rec_index = \"%d\"", rec_index);
	xml += tmp;

	tmp.Format(" opms_give_station_text = \"%d\"", opmsGiveStationText);
	xml += tmp;

	tmp.Format(" opms_station_text = \"%s\"", GetValidStr(szOpmsStationText));
	xml += tmp;

	tmp.Format(" station_text = \"%s\"", GetValidStr(szStationText));
	xml += tmp;

	tmp.Format(" abs_text = \"%s\"", GetValidStr(szAbsText));
	xml += tmp;

	tmp.Format(" alarm_text = \"%s\"", GetValidStr(szAlarmText));
	xml += tmp;
	
	tmp.Format(" note_text = \"%s\"", GetValidStr(szPlanNote));
	xml += tmp;
	
	tmp.Format(" arrive_train_id = \"%s\"", GetValidStr(arrive_train_id));
	xml += tmp;

	tmp.Format(" original_arrive_train_id = \"%s\"", GetValidStr(original_arrive_train_id));
	xml += tmp;

	tmp.Format(" depart_train_id = \"%s\"", GetValidStr(depart_train_id));
	xml += tmp;

	tmp.Format(" original_depart_train_id = \"%s\"", GetValidStr(original_depart_train_id));
	xml += tmp;

	tmp.Format(" station = \"%d\"", station);
	xml += tmp;

	tmp.Format(" entry = \"%d\"", entry);
	xml += tmp;

	tmp.Format(" exit = \"%d\"", exit);
	xml += tmp;

	tmp.Format(" arrive_side = \"%d\"", arrive_side);
	xml += tmp;

	tmp.Format(" plan_arrive_side = \"%d\"", plan_arrive_side);
	xml += tmp;

	tmp.Format(" depart_side = \"%d\"", depart_side);
	xml += tmp;

	tmp.Format(" plan_depart_side = \"%d\"", plan_depart_side);
	xml += tmp;
	   
	tmp.Format(" min_stop_time = \"%d\"", min_stop_time);
	xml += tmp;

	tmp.Format(" arrive = \"%d\"", arrive);
	xml += tmp;

	tmp.Format(" depart = \"%d\"", depart);
	xml += tmp;

	tmp.Format(" plan_arrive = \"%d\"", plan_arrive);
	xml += tmp;

	tmp.Format(" plan_depart = \"%d\"", plan_depart);
	xml += tmp;
	
	tmp.Format(" stop_reason = \"%d\"", stop_reason);
	xml += tmp;

	tmp.Format(" late_reason = \"%d\"", late_reason);
	xml += tmp;

	tmp.Format(" early_depart_time = \"%d\"", early_depart_time);
	xml += tmp;

	tmp.Format(" run_fast_slow_time = \"%d\"", run_fast_slow_time);
	xml += tmp;

	tmp.Format(" route_flag = \"%d\"", route_flag);
	xml += tmp;

	tmp.Format(" speed_limit = \"%d\"", speed_limit);
	xml += tmp;

	tmp.Format(" stop_condition = \"%d\"", stop_condition);
	xml += tmp;

	tmp.Format(" adjust_status = \"%d\"", adjust_status);
	xml += tmp;

	tmp.Format(" super_interval_ltime = \"%d\"", super_interval_ltime);
	xml += tmp;

	tmp.Format(" spec_run_time = \"%d\"", spec_run_time);
	xml += tmp;

	tmp.Format(" special_position = \"%d\"", stop_abs_position);
	xml += tmp;

	if(stop_abs_s_time>0)
		tmp.Format(" special_time_1 = \"%d\"", SetAbsStopTime(stop_abs_s_time));
	else
		tmp.Format(" special_time_1 = \"%d\"", stop_abs_s_time);
	xml += tmp;

	if(stop_abs_e_time>0)
		tmp.Format(" special_time_2 = \"%d\"", SetAbsStopTime(stop_abs_e_time));
	else
		tmp.Format(" special_time_2 = \"%d\"", stop_abs_e_time);
	xml += tmp;    

	tmp.Format(" special_train_index = \"%d\"", special_train_index);
	xml += tmp;

	tmp.Format(" data_source = \"%d\"", data_source);
	xml += tmp;

	tmp.Format(" flag = \"%d\"", flag);
	xml += tmp;

	tmp.Format(" can_not_meet = \"%d\"", no_meet_reason);
	xml += tmp;
	
	tmp.Format(" modify_abs_runtime = \"%d\"", modify_abs_runtime);
	xml += tmp;

	tmp.Format(" in_station_index = \"%d\"", in_station_index);
	xml += tmp;

	tmp.Format(" out_station_index = \"%d\"", out_station_index);
	xml += tmp;
	
	tmp.Format(" rank = \"%d\"", rank);
	xml += tmp;

	xml += ">";

	for(UINT idx = 0; idx < this->abs_stop.size(); idx++)
	{
		xml += "<ABS_STOP";
		
		tmp.Format(" flag = \"%d\"", abs_stop[idx].flag);
		xml += tmp;

		tmp.Format(" position = \"%d\"", abs_stop[idx].position);
		xml += tmp;

		tmp.Format(" start_time = \"%d\"", abs_stop[idx].start_time);
		xml += tmp;

		tmp.Format(" stop_time = \"%d\"", abs_stop[idx].stop_time);
		xml += tmp;
			
		xml += "/>";	   
	}
	
	xml += "</RECORD>";

	return xml;
}

bool TgTRAIN_RECORD::IfInTimeScope(TIME stime, TIME etime) const
{
	if(arrive>=stime && arrive<=etime)
		return true;
	if(depart>=stime && depart<=etime)
		return true;
	if(arrive<=stime && depart>=etime)
		return true;
	return false;
}

TgTRAIN_MORE_INFO::TgTRAIN_MORE_INFO()
{
	Clear();
}
TgTRAIN_MORE_INFO::TgTRAIN_MORE_INFO(const TgTRAIN_MORE_INFO& rhs)
{
	Copy(rhs);
}
TgTRAIN_MORE_INFO& TgTRAIN_MORE_INFO::operator=(const TgTRAIN_MORE_INFO& rhs)
{
	if(this == &rhs)
		return (*this);

	Copy(rhs);
	return *this;
}

TgTRAIN_MORE_INFO::~TgTRAIN_MORE_INFO()
{
	Clear();
}

void TgTRAIN_MORE_INFO::Clear()
{
	this->train_name="";
	this->start_sta_name="";
	this->end_sta_name="";

	this->train_key = 0;
	this->basic_train_index = 0;
	this->shift_index_string="";

	this->type = 0;
	this->run_type = 0;

	start_actual_time=0; 
	start_plan_time=0;
	
	this->sborder_plan_time = 0;
	this->sborder_actual_time = 0;
	this->eborder_plan_time = 0;
	this->eborder_actual_time = 0;

	this->sborder_sta = 0;
	this->eborder_sta = 0;
	
	this->from_conid = 255;
	this->to_conid = 255;
	
	this->priority = 0;
	this->train_length = 0;
	this->train_weight = 0;
	this->status = 0;
	
	this->space_rank = 0;
	this->length_rank = 0;
	this->super_rank = 0;
	this->military_rank = 0;

	this->engine_utility = 0;
	this->side_utility = 0;

	this->no_meet_flag = 0;
		
	this->adjust_begin_sta = 0;
	this->adjust_end_sta = 0;
	this->adjust_flag = 0;
	this->train_source = 0;
	this->stop_abs_time = 0;

	// add your code here
	this->whole_static=0;
	this->b_elec_rank = false;
	this->b_self_update = true;
}
void TgTRAIN_MORE_INFO::Copy(const TgTRAIN_MORE_INFO& rhs)
{
	this->train_name = rhs.train_name;
	this->start_sta_name = rhs.start_sta_name;
	this->end_sta_name = rhs.end_sta_name;

	this->train_key = rhs.train_key;
	this->basic_train_index = rhs.basic_train_index;

	this->type = rhs.type;
	this->run_type = rhs.run_type;

	start_actual_time=rhs.start_actual_time;
	start_plan_time=rhs.start_plan_time;

	this->sborder_plan_time = rhs.sborder_plan_time;
	this->sborder_actual_time = rhs.sborder_actual_time;

	this->eborder_plan_time = rhs.eborder_plan_time;
	this->eborder_actual_time = rhs.eborder_actual_time;

	this->sborder_sta = rhs.sborder_sta;
	this->eborder_sta = rhs.eborder_sta;
		
	this->from_conid = rhs.from_conid;
	this->to_conid = rhs.to_conid;

	this->priority = rhs.priority;
	this->train_length = rhs.train_length;
	this->train_weight = rhs.train_weight;
	this->status = rhs.status;
	
	this->space_rank = rhs.space_rank;
	this->length_rank = rhs.length_rank;
	this->super_rank = rhs.super_rank;
	this->military_rank = rhs.military_rank;
	
	this->engine_utility = rhs.engine_utility;
	this->side_utility = rhs.side_utility;

	this->no_meet_flag = rhs.no_meet_flag;
	
	
	this->adjust_begin_sta = rhs.adjust_begin_sta;
	this->adjust_end_sta = rhs.adjust_end_sta;

	this->adjust_flag = rhs.adjust_flag;
	this->train_source = rhs.train_source;
	this->stop_abs_time = rhs.stop_abs_time;

	this->shift_index_string = rhs.shift_index_string;

	// add your code here
	this->whole_static=rhs.whole_static;
	this->b_elec_rank = rhs.b_elec_rank;
	this->b_self_update = rhs.b_self_update;
}

bool TgTRAIN_MORE_INFO::IsInfoCoreSame(const TgTRAIN_MORE_INFO& rhs)
{
	if(train_name != rhs.train_name)
		return false;
	if(train_key != rhs.train_key)
		return false;
	if(basic_train_index != rhs.basic_train_index)
		return false;
	if(type != rhs.type)
		return false;
	if(run_type != rhs.run_type)
		return false;

	if(sborder_plan_time != rhs.sborder_plan_time)
		return false;
	//if(sborder_actual_time != rhs.sborder_actual_time)
	//	return false;
	if(eborder_plan_time != rhs.eborder_plan_time)
		return false;
	if(eborder_actual_time != rhs.eborder_actual_time)
		return false;

	if(sborder_sta != rhs.sborder_sta)
		return false;
	if(eborder_sta != rhs.eborder_sta)
		return false;
	if(start_sta_name != rhs.start_sta_name)
		return false;
	if(end_sta_name != rhs.end_sta_name)
		return false;

	if(from_conid != rhs.from_conid)
		return false;
	if(to_conid != rhs.to_conid)
		return false;
	if(priority != rhs.priority)
		return false;
	if(train_length != rhs.train_length)
		return false;
	
	if(train_weight != rhs.train_weight)
		return false;
	if(status != rhs.status)
		return false;
	if(space_rank != rhs.space_rank)
		return false;
	if(length_rank != rhs.length_rank)
		return false;

	if(super_rank != rhs.super_rank)
		return false;
	if(military_rank != rhs.military_rank)
		return false;
	if(engine_utility != rhs.engine_utility)
		return false;

	if(side_utility != rhs.side_utility)
		return false;

	if(start_actual_time != rhs.start_actual_time)
		return false;
	if(start_plan_time != rhs.start_plan_time)
		return false;
	if(adjust_begin_sta != rhs.adjust_begin_sta)
		return false;

	if(adjust_end_sta != rhs.adjust_end_sta)
		return false;
	if(adjust_flag != rhs.adjust_flag)
		return false;
	if(train_source != rhs.train_source)
		return false;
	//if(b_self_update != rhs.b_self_update)
	//	return false;
		
	if(shift_index_string != rhs.shift_index_string)
		return false;
	
	return true;
}

CString TgTRAIN_MORE_INFO::toXML()
{
	CString xml,tmp;

	xml = "<INFO";

	tmp.Format(" train_name = \"%s\"", GetValidStr(train_name));
	xml += tmp;

	tmp.Format(" shift_index_string = \"%s\"", GetValidStr(shift_index_string));
	xml += tmp;

	tmp.Format(" train_key = \"%d\"", train_key);
	xml += tmp;

	tmp.Format(" basic_train_index = \"%d\"", basic_train_index);
	xml += tmp;

	tmp.Format(" type = \"%d\"", type);
	xml += tmp;

	tmp.Format(" run_type = \"%d\"", run_type);
	xml += tmp;

	tmp.Format(" start_actual_time = \"%d\"", start_actual_time);
	xml += tmp;

	tmp.Format(" start_plan_time = \"%d\"", start_plan_time);
	xml += tmp;

	tmp.Format(" sborder_plan_time = \"%d\"", sborder_plan_time);
	xml += tmp;

	tmp.Format(" sborder_actual_time = \"%d\"", sborder_actual_time);
	xml += tmp;

	tmp.Format(" eborder_plan_time = \"%d\"", eborder_plan_time);
	xml += tmp;

	tmp.Format(" eborder_actual_time = \"%d\"", eborder_actual_time);
	xml += tmp;
	       
	tmp.Format(" sborder_sta = \"%d\"", sborder_sta);
	xml += tmp;

	tmp.Format(" eborder_sta = \"%d\"", eborder_sta);
	xml += tmp;
		  
	tmp.Format(" start_sta_name = \"%s\"", GetValidStr(start_sta_name));
	xml += tmp;

	tmp.Format(" end_sta_name = \"%s\"", GetValidStr(end_sta_name));
	xml += tmp;
		 
	tmp.Format(" from_conid = \"%d\"", from_conid);
	xml += tmp;

	tmp.Format(" to_conid = \"%d\"", to_conid);
	xml += tmp;

	tmp.Format(" priority = \"%d\"", priority);
	xml += tmp;

	tmp.Format(" train_length = \"%d\"", train_length);
	xml += tmp;
		
	tmp.Format(" train_weight = \"%d\"", train_weight);
	xml += tmp;

	tmp.Format(" status = \"%d\"", status);
	xml += tmp;
		
	tmp.Format(" space_rank = \"%d\"", space_rank);
	xml += tmp;

	tmp.Format(" length_rank = \"%d\"", length_rank);
	xml += tmp;

	tmp.Format(" super_rank = \"%d\"", super_rank);
	xml += tmp;

	tmp.Format(" military_rank = \"%d\"", military_rank);
	xml += tmp;
		
	tmp.Format(" engine_utility = \"%d\"", engine_utility);
	xml += tmp;

	tmp.Format(" side_utility = \"%d\"", side_utility);
	xml += tmp;

	tmp.Format(" adjust_begin_sta = \"%d\"", adjust_begin_sta);
	xml += tmp;

	tmp.Format(" adjust_end_sta = \"%d\"", adjust_end_sta);
	xml += tmp;

	adjust_flag = adjust_flag & 0xffff;
	adjust_flag += stop_abs_time<<16;
	tmp.Format(" adjust_flag = \"%d\"", adjust_flag);
	xml += tmp;

	tmp.Format(" train_source = \"%d\"", train_source);
	xml += tmp;
	
	xml += "/>";

	return xml;
}

//-------------------------------------------------------
//
//				TgTRAIN_ENGINE_INFO
//
//-------------------------------------------------------
TgTRAIN_ENGINE_INFO::TgTRAIN_ENGINE_INFO()
{
	Clear();
}

TgTRAIN_ENGINE_INFO::TgTRAIN_ENGINE_INFO(const TgTRAIN_ENGINE_INFO& rhs)
{
	Copy(rhs);
}

TgTRAIN_ENGINE_INFO& TgTRAIN_ENGINE_INFO::operator=(const TgTRAIN_ENGINE_INFO& rhs)
{
	if(this == &rhs)
		return (*this);

	Copy(rhs);
	return *this;
}

TgTRAIN_ENGINE_INFO::~TgTRAIN_ENGINE_INFO()
{
	Clear();
}

void TgTRAIN_ENGINE_INFO::Copy(const TgTRAIN_ENGINE_INFO& rhs)
{
	engine_id = rhs.engine_id;
	engine_name = rhs.engine_name;
	engine_status = rhs.engine_status;
	attach_sta = rhs.attach_sta;
	unattach_sta = rhs.unattach_sta;
}

void TgTRAIN_ENGINE_INFO::Clear()
{
	engine_id.Empty();
	engine_name.Empty();
	engine_status = 0;
	attach_sta = 0;
	unattach_sta = 0;
}

bool TgTRAIN_ENGINE_INFO::IsEngineCoreSame(const TgTRAIN_ENGINE_INFO& rhs)
{
	if(engine_id != rhs.engine_id)
		return false;
	if(engine_name != rhs.engine_name)
		return false;
	if(engine_status != rhs.engine_status)
		return false;
	if(attach_sta != rhs.attach_sta)
		return false;
	if(unattach_sta != rhs.unattach_sta)
		return false;

	return true;
}

CString TgTRAIN_ENGINE_INFO::toXML()
{
	CString xml,tmp;

	xml = "<ENGINE ";

	tmp.Format(" engine_id = \"%s\"", GetValidStr(engine_id));
	xml += tmp;

	tmp.Format(" engine_type = \"%s\"", GetValidStr(engine_name));
	xml += tmp;

	tmp.Format(" engine_status = \"%d\"", engine_status);
	xml += tmp;

	tmp.Format(" attach_sta = \"%d\"", attach_sta);
	xml += tmp;

	tmp.Format(" unattach_sta = \"%d\"", unattach_sta);
	xml += tmp;
	
	xml += "/>";

	return xml;
}

//-------------------------------------------------------
//
//				CTG_TRAIN
//
//-------------------------------------------------------
CTG_TRAIN::CTG_TRAIN()
{
	dwLogicFlag=0;
	dwFlag = 0;
	mbDoubleDirect=false;
	m_nTrainIndex = 0;
	prev_train_index = 0;
	mOtherTrainIndex = 0;
	m_flashTimes=0;
	myRecord.clear();
	bWholeTrainSideLock=false;
	m_train_rect.SetRectEmpty();
}
CTG_TRAIN::~CTG_TRAIN()
{
	dwFlag = 0;
	dwLogicFlag = 0;
	mbDoubleDirect = false;
	myRecord.clear();
	myEngine.clear();
	myTrainInfo.Clear();
	m_nTrainIndex = 0;
	prev_train_index = 0;
	mOtherTrainIndex = 0;
	m_flashTimes=0;
	bWholeTrainSideLock=false;
	m_train_rect.SetRectEmpty();
}
CTG_TRAIN::CTG_TRAIN(const CTG_TRAIN& rhs): CBaseTrain(rhs)
{
	dwFlag = 0;
	dwLogicFlag = 0;

	m_train_rect = rhs.m_train_rect;
	mbDoubleDirect = rhs.mbDoubleDirect;
	m_nTrainIndex = rhs.m_nTrainIndex;
	prev_train_index = rhs.prev_train_index;
	myTrainInfo = rhs.myTrainInfo;
    mOtherTrainIndex = rhs.mOtherTrainIndex;
	bWholeTrainSideLock=rhs.bWholeTrainSideLock;
	myRecord.clear();
	myEngine.clear();

	vector<TgTRAIN_RECORD>::const_iterator itrd = rhs.myRecord.begin();
	while(itrd != rhs.myRecord.end())
	{
		myRecord.push_back(*itrd);
		itrd++;
	}

	vector<TgTRAIN_ENGINE_INFO>::const_iterator iteg = rhs.myEngine.begin();
	while(iteg != rhs.myEngine.end())
	{
		myEngine.push_back(*iteg);
		iteg++;
	}
}
CTG_TRAIN& CTG_TRAIN::operator=(const CTG_TRAIN& rhs)
{
	if(this == &rhs)
		return *this;
	
	dwFlag = 0;
	dwLogicFlag=0;
	m_nTrainIndex = rhs.m_nTrainIndex;
	prev_train_index = rhs.prev_train_index;
	CBaseTrain::operator =(rhs);

	myTrainInfo = rhs.myTrainInfo;
	m_train_rect = rhs.m_train_rect;
	mbDoubleDirect = rhs.mbDoubleDirect;
	mOtherTrainIndex = rhs.mOtherTrainIndex;
	bWholeTrainSideLock=rhs.bWholeTrainSideLock;

	myRecord.clear();
	myEngine.clear();
	
	vector<TgTRAIN_RECORD>::const_iterator itrd = rhs.myRecord.begin();
	while(itrd != rhs.myRecord.end())
	{
		myRecord.push_back(*itrd);
		itrd++;
	}

	vector<TgTRAIN_ENGINE_INFO>::const_iterator iteg = rhs.myEngine.begin();
	while(iteg != rhs.myEngine.end())
	{
		myEngine.push_back(*iteg);
		iteg++;
	}

	return *this;
}

int CTG_TRAIN::OffsetWorkScheduleTime(int nTimeOffset)
{
	for(UINT idx = 0; idx < myRecord.size(); idx++)
	{
		myRecord[idx].arrive += nTimeOffset;
		myRecord[idx].depart += nTimeOffset;
	}

	return 0;
}

/*
*	nRecordNo: 移动起点 序号
*	nTimeOffset: 时间变化
*   nFlag: 1 发点变 2 到发点变
**/
int CTG_TRAIN::OffsetWorkScheduleTime(int nRecordNo, int nTimeOffset, int nFlag)
{
	if(nRecordNo >= myRecord.size() || nRecordNo < 0)
		return -1;

	if(nFlag != 1 && nFlag != 2)
		return 0;

	TgTRAIN_RECORD* rec = GetRecordByRecIndex(nRecordNo);
	if(rec==NULL)
		return 0;

	if(nFlag == 2)
	{
		rec->arrive += nTimeOffset;
	}
	if(nFlag == 1 || nFlag == 2)
	{
		rec->depart += nTimeOffset;
	}

	// 选中车站发点是实际点, 后续车站不偏移
	if(IsActualDepartTime(rec->adjust_status))
		return 0;

	for(UINT idx = nRecordNo+1; idx < myRecord.size(); idx++)
	{
		TgTRAIN_RECORD* rec = GetRecordByRecIndex(idx);
		if(rec==NULL)
			break;
		if(IsActualArrivalTime(rec->adjust_status) || IsActualDepartTime(rec->adjust_status))
			break;

		rec->arrive += nTimeOffset;
		rec->depart += nTimeOffset;
	}
	
	return 0;
}

/*
*	nRecordNo: 移动起点 序号
*	nTimeOffset: 时间变化
*	bMoveWhole: 是否只移动第一个点
*   nFlag: 0 到点变 1 发点变 2 到发点变
**/
int CTG_TRAIN::OffsetBasicScheduleTime(int nRecordNo,int nTimeOffset,bool bMoveWhole, int nFlag)
{
	if(nRecordNo >= myRecord.size() || nRecordNo < 0)
		return -1;

	TgTRAIN_RECORD* rec = GetRecordByRecIndex(nRecordNo);
	if(rec==NULL)
		return 0;

	if(nFlag == 0 || nFlag == 2)
	{
		rec->arrive += nTimeOffset;
	}
	if(nFlag == 1 || nFlag == 2)
	{
		rec->depart += nTimeOffset;
	}

	if(bMoveWhole)
	{
		for(UINT idx = nRecordNo+1; idx < myRecord.size(); idx++)
		{
			TgTRAIN_RECORD* rec = GetRecordByRecIndex(idx);
			if(rec==NULL)
				break;

			rec->arrive += nTimeOffset;
			rec->depart += nTimeOffset;
		}
	}
	
	return 0;
}
UINT CTG_TRAIN::GetIndexOfRecord(int nStationNo)
{
	UINT uIndex = 0;
	while(uIndex < myRecord.size())
	{
		if(myRecord[uIndex].station == nStationNo)
			return uIndex;
		uIndex++;
	}
	return -1;
}

TgTRAIN_RECORD* CTG_TRAIN::GetLastRecord()
{
	if (myRecord.empty())
	{
		return NULL;
	}
	return &myRecord[myRecord.size()-1];
}
TgTRAIN_RECORD* CTG_TRAIN::GetFirstRecord()
{
	if (myRecord.empty())
	{
		return NULL;
	}
	return &myRecord[0];
}
TgTRAIN_RECORD* CTG_TRAIN::GetRecordByPosition(int pos)
{
	if(pos >= 0 && pos < myRecord.size())
		return &myRecord[pos];
	return NULL;
}

TgTRAIN_RECORD* CTG_TRAIN::GetRecordByRecIndex(int uIndex)
{
	if(uIndex >= 0 && uIndex < myRecord.size())
		return &myRecord[uIndex];
	return NULL;
}

TgTRAIN_RECORD* CTG_TRAIN::GetStationRecord(int nStationNo)
{
	for(UINT uIndex = 0; uIndex < myRecord.size(); uIndex++)
	{
		if(myRecord[uIndex].station == nStationNo) 
		{
			return &myRecord[uIndex];
		}

	}
	return NULL;
}

TgTRAIN_RECORD* CTG_TRAIN::GetPrevStationRecord(const TgTRAIN_RECORD* currec)
{
	if(currec == NULL)
		return NULL;

	for(UINT uIndex = 1; uIndex < myRecord.size(); uIndex++)
	{
		if(myRecord[uIndex].rec_index == currec->rec_index)
		{
			return &myRecord[uIndex - 1];
		}
	}

	return NULL;
}

TgTRAIN_RECORD* CTG_TRAIN::GetNextStationRecord(const TgTRAIN_RECORD* currec)
{
	if(currec == NULL)
		return NULL;

	for(UINT uIndex = 0; uIndex < myRecord.size()-1; uIndex++)
	{
		if(myRecord[uIndex].rec_index == currec->rec_index) 
		{
			return &myRecord[uIndex + 1];
		}
	}

	return NULL;
}

void CTG_TRAIN::DeleteTailRecord()
{
	myRecord.pop_back();
}

void CTG_TRAIN::DeleteHeadRecord()
{
	myRecord.erase(myRecord.begin());
}

UINT CTG_TRAIN::GetRecordNum() const
{
	return myRecord.size();
}

int  CTG_TRAIN::AddRecord(TgTRAIN_RECORD& curr, int nType)
{
	int rc = -1;
	if(TG_TYPE::BASIC_TG == nType)
	{
		curr.arrive = curr.arrive%86400;
		curr.depart = curr.depart%86400;
		curr.plan_arrive = curr.arrive;
		curr.plan_depart = curr.depart;
		curr.plan_arrive_side = curr.arrive_side;
		curr.plan_depart_side = curr.depart_side;

		myRecord.push_back(curr);
		rc = 0;
	}
	else
	{
		if(myRecord.empty())
		{
			myRecord.push_back(curr);
			rc = 0;
		}
		else
		{
			TgTRAIN_RECORD& rec = myRecord.back();
			if((curr.arrive - rec.depart) > 0)
			{
				curr.min_stop_time = (curr.depart - curr.arrive) / 60;

				myRecord.push_back(curr);
				rc = 0;
			} 
		}
	}

	return rc;
}

CString CTG_TRAIN::GetTrainId()
{
	if(myRecord.size() == 0)
		return "无车次";
    return myRecord[0].arrive_train_id;
}

void CTG_TRAIN::SetTrainType()
{
	m_nTrainType = TYPE::FREIGHT;
	if(IsTrain(myTrainInfo))
	{
		m_nTrainType = TYPE::PASSENGER;
	}

	bWholeTrainSideLock=true;
	TgTRAIN_RECORD *rec;
	for(int i=0; i<GetRecordNum(); i++)
	{
		rec = GetRecordByRecIndex(i);
		if(NULL != rec)
		{
			if(!IsForceSide(*rec))
			{
				bWholeTrainSideLock=false;
				break;
			}
		}
	}
}

void CTG_TRAIN::SetEleLocoTrain()
{
	myTrainInfo.b_elec_rank = false;
	for(UINT idx=0; idx<this->myRecord.size(); idx++)
	{
		if(IsElectricEngineTrain(this->myRecord[idx]))
		{
			myTrainInfo.b_elec_rank = true;
			break;
		}
	}
}

//获得牵引机车号
CString CTG_TRAIN::GetTractionIds()
{
	CString tractionStr = "";

	for(UINT i = 0; i < myEngine.size(); i++)
	{
		if(i==0)
		{
			tractionStr += myEngine[i].engine_id;
		}
		else
		{
			tractionStr += ",";
			tractionStr += myEngine[i].engine_id;
		}
	}
	return tractionStr;
}

void CTG_TRAIN::ClearEngine()
{
	myEngine.clear();
}

void CTG_TRAIN::AppendTailEngine(const TgTRAIN_ENGINE_INFO& eng)
{
	myEngine.push_back(eng);
}

//-----------------------------------------------------------
ULONG CTG_TRAIN::GetTrainIndex()
{ 
	return this->m_nTrainIndex;
}
CString CTG_TRAIN::toXML(DWORD flag, WORD station, WORD rec_index, CString trainid, BYTE nRunType)
{
	CString xml, tmp;
	tmp.Format("<TRAIN train_index = \"%u\" prev_train_index = \"%u\" >", 
		this->m_nTrainIndex, prev_train_index);
	xml = tmp;
	// 扩充操作
	xml += "<LOGIC ";
		tmp.Format(" flag = \"%d\"", flag);
		xml += tmp;
		tmp.Format(" station = \"%d\"", station);
		xml += tmp;
		tmp.Format(" rec_index = \"%d\"", rec_index);
		xml += tmp;
		tmp.Format(" train_id = \"%s\"", GetValidStr(trainid));
		xml += tmp;
		tmp.Format(" run_type = \"%d\"/>", nRunType);
		xml += tmp;
	
	// INFO
	xml += this->myTrainInfo.toXML();
	
	xml += "<RECORDS>";
	for(UINT idx = 0; idx < this->myRecord.size(); idx++)
	{
		xml += myRecord[idx].toXML();
	}
    xml += "</RECORDS>";

	xml += "<ENGINES>";
	for(UINT idx = 0; idx < this->myEngine.size(); idx++)
	{
		xml += myEngine[idx].toXML();
	}
    xml += "</ENGINES>";

	xml += "</TRAIN>";
   
	return xml;
}

CString CTG_TRAIN::toString()
{
	CString ret, tmp;
	ret.Format("prev_train_index %u shift_string %s stanum %d:", prev_train_index, this->myTrainInfo.shift_index_string, this->GetRecordNum());
	for (UINT i=0; i<this->GetRecordNum();i++) 
	{
		tmp.Format("%d(%d-%d)(%d/%d-%d/%d %d-%d) ", myRecord[i].station, 
			myRecord[i].entry,myRecord[i].exit,
			myRecord[i].plan_arrive_side, myRecord[i].plan_depart_side, 
			myRecord[i].arrive_side, myRecord[i].depart_side, 
			myRecord[i].arrive, myRecord[i].depart);
		ret += tmp;
	}
	return ret;
}

CString CTG_TRAIN::toPassway()
{
	CString xml, tmp;
	xml = "\n<PASSWAY>";
	for(UINT idx = 0; idx < this->myRecord.size(); idx++)
	{
		CString tmp;
		tmp.Format("\n\t<PASSWAY_ELE sta_id=\"%d\" entry=\"%d\" exit=\"%d\" in_sta_index=\"%d\" out_sta_index=\"%d\" arrive_side=\"%d\" depart_side=\"%d\"/>",
			myRecord[idx].station,  
			myRecord[idx].entry, 
			myRecord[idx].exit, 
			myRecord[idx].in_station_index, 
			myRecord[idx].out_station_index, 
			myRecord[idx].arrive_side, 
			myRecord[idx].depart_side); 
		xml += tmp;
	}
    xml += "\n</PASSWAY>";

	return xml;
}

CString CTG_TRAIN::toXMLForExitEntry()
{
	CString ret, tmp;
	ret="<TRAIN>";
	for (UINT i=0; i<this->GetRecordNum();i++) 
	{
		tmp.Format("<RECORD station=\"%d\" entry=\"%d\" exit=\"%d\" limit_speed=\"%d\" in_index=\"%d\" out_index=\"%d\"/>", 
			myRecord[i].station, 
			myRecord[i].entry, 
			myRecord[i].exit,
			myRecord[i].speed_limit,
			myRecord[i].in_station_index, 
			myRecord[i].out_station_index);
		ret += tmp;
	}
	ret+="</TRAIN>";
	return ret;
}

bool CTG_TRAIN::GetLogicInfo(CString xml)
{
	this->m_nTrainIndex = 0;
	this->dwLogicFlag = 0;

	MSXML::IXMLDOMDocumentPtr pXMLDoc;
	HRESULT hr = pXMLDoc.CreateInstance(MSXML::CLSID_DOMDocument);
	if(FAILED(hr))
	{
		_com_error er(hr);
		AfxMessageBox(er.ErrorMessage());
		return false;
	}
	
	if(VARIANT_TRUE != pXMLDoc->loadXML(_bstr_t( xml )))
	{
		return false;
	}
	
	MSXML::IXMLDOMNodePtr pRoot = pXMLDoc->selectSingleNode( "TRAIN");
	if(pRoot == NULL)
	{
		return false;
	}
	 
	_variant_t v;
	MSXML::IXMLDOMNodePtr pNode, pChildNode;
	
	pNode = pRoot->selectSingleNode( "@train_index" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		this->m_nTrainIndex = atol(CString((v.bstrVal)));
		SysFreeString(v.bstrVal);
		v.Clear();
	}

	// 逻辑---在formXML中也有该段
	pNode = pRoot->selectSingleNode( "LOGIC" );
	if(pNode)
	{
		pChildNode = pNode->selectSingleNode( "@flag" );
		if(pChildNode)
		{
			v = pChildNode->GetnodeTypedValue();
			this->dwLogicFlag = atol(CString((v.bstrVal)));
			SysFreeString(v.bstrVal);
			v.Clear();
		}
	}
		
	return true;
}

bool CTG_TRAIN::IsTrainIDChange(const CTG_TRAIN& rhs)
{
	if(myRecord.size() != rhs.myRecord.size())
		return true;

	for(int i=0; i<myRecord.size(); i++)
	{
		if((myRecord[i].arrive_train_id!=rhs.myRecord[i].arrive_train_id)||
		   (myRecord[i].depart_train_id!=rhs.myRecord[i].depart_train_id))
			return true;
	}

	return false;
}

bool CTG_TRAIN::IsTrainCoreSame(const CTG_TRAIN& rhs)
{
	if(prev_train_index != rhs.prev_train_index)
		return false;
	if(myRecord.size() != rhs.myRecord.size())
		return false;

	if(myEngine.size() != rhs.myEngine.size())
		return false;

	if(!myTrainInfo.IsInfoCoreSame(rhs.myTrainInfo))
		return false;

	for(int i=0; i<myEngine.size(); i++)
	{
		if(!myEngine[i].IsEngineCoreSame(rhs.myEngine[i]))
			return false;
	}

	for(int i=0; i<myRecord.size(); i++)
	{
		if(!myRecord[i].IsRecordCoreSame(rhs.myRecord[i]))
			return false;
	}

	return true;
}

void CTG_TRAIN::AddPerPointInfo(WORD nRecIndex, WORD nFlag, const CPoint& point, int nSection)
{
	if(m_nPointCount==0)
	{
		for(int i=0; i<MAX_REC; i++)
		{
			this->m_pPerPointInfo[i].init();
		}
	}

	for(int i=0; i<m_nPointCount; i++)
	{
		if(nRecIndex == this->m_pPerPointInfo[i].record_index)
		{
			if(this->m_pPerPointInfo[i].start_depart_flag == nFlag &&
				this->m_pPerPointInfo[i].nSubSection == nSection &&
				this->m_pPerPointInfo[i].point == point)
			{
				return;
			}
		}
	}

	this->m_pPerPointInfo[m_nPointCount].record_index = nRecIndex;
	this->m_pPerPointInfo[m_nPointCount].start_depart_flag = nFlag;
	this->m_pPerPointInfo[m_nPointCount].nSubSection = nSection;
	this->m_pPerPointInfo[m_nPointCount].point = point;
	m_nPointCount++;
	
	// 修正错误的到发合并标志
	if(m_nPointCount < 2)
		return;
	if(this->m_pPerPointInfo[m_nPointCount-2].record_index != nRecIndex)
		return;

	if(nFlag == UNION_POSITION)
	{
		if(this->m_pPerPointInfo[m_nPointCount-2].point.x == point.x)
		{
			this->m_pPerPointInfo[m_nPointCount-1].start_depart_flag 
				= this->m_pPerPointInfo[m_nPointCount-2].start_depart_flag;
		}
	}
	else
	{
		for(int i=m_nPointCount-2; i>=0; i--)
		{
			if((this->m_pPerPointInfo[i].nSubSection == nSection) || (this->m_pPerPointInfo[i].point.x != point.x))
				break;

			this->m_pPerPointInfo[i].start_depart_flag = nFlag;
		}
	}
}

bool CTG_TRAIN::IsCHRTrain()
{
	if(myTrainInfo.run_type==5)
		return true;

	// 普通D车
	if(myTrainInfo.type==17 || myTrainInfo.type==18 || myTrainInfo.type==19)
		return true;
	// 高速D车
	if(myTrainInfo.type==22 || myTrainInfo.type==23)
		return true;
	// 城际D车
	if(myTrainInfo.type==24 || myTrainInfo.type==25)
		return true;

	return false;
}

bool CTG_TRAIN::IsWholeTrainSideLock()
{
	return bWholeTrainSideLock;
}

void CTG_TRAIN::InitBasicTM()
{
	m_flashTimes=21;
	m_nTrainIndex=1;
	this->myTrainInfo.train_key=1;
	for(int i=0; i<myRecord.size(); i++)
	{
		long flag=0;
		if(i==0)
		{
			if(IsFlagStart(myRecord[i].flag))
				SetFlagStart(flag);
			else
				SetFlagAccept(flag);
		}
		if(i==myRecord.size()-1)
		{
			if(IsFlagTerminal(myRecord[i].flag))
				SetFlagTerminal(flag);
			else
				SetFlagGiveout(flag);
		}
		myRecord[i].flag=flag;
		myRecord[i].InitRecordBasic();
		myRecord[i].arrive = myRecord[i].plan_arrive;
		myRecord[i].depart = myRecord[i].plan_depart;
		myRecord[i].arrive_side = myRecord[i].plan_arrive_side;
		myRecord[i].depart_side = myRecord[i].plan_depart_side;
	}
}

bool CTG_TRAIN::IsFlash(int& times)
{
	if(m_flashTimes>0)
	{
		times=m_flashTimes;
		m_flashTimes--;
		return true;
	}
	return false;
}

bool CTG_TRAIN::IsExistActualTime()
{
	for (UINT i=0; i<this->GetRecordNum();i++) 
	{
		if(myRecord[i].adjust_status & 0x0003)
			return true;
	}

	return false;
}

bool CTG_TRAIN::IfInTimeScope(TIME stime, TIME etime) const
{
	int rec_count=this->GetRecordNum();
	if(rec_count<=0)
		return false;
	int i;
	for( i=0;i<rec_count;i++)
	{
		if(myRecord[i].IfInTimeScope(stime,etime))
			break;			
	}
	if(i<rec_count)
		return true;
	else
		return false;
}

