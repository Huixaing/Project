#include "stdafx.h"
#include "Tg_TRAIN.h"
#include "Tg_BLOCK.h"
#include <baseMsg\msgid.h>
#include <baseMsg\clbth.h>

void CopyTrainInfo(TgTRAIN_MORE_INFO& def, const MsgTrainInfo& msg)
{
	def.shift_index_string   = msg.shift_string;
	def.train_name			 = msg.train_name;
	def.train_key            = msg.train_key;
	def.basic_train_index    = msg.basic_index;  
		
	def.type                 = msg.type;
	def.run_type             = msg.run_type;

	def.start_plan_time      =msg.start_plan_time;
	def.start_actual_time    =msg.start_actual_time; 
	
	def.sborder_plan_time    = msg.sborder_plan_time;
	def.sborder_actual_time  = msg.sborder_actual_time;
	def.eborder_plan_time    = msg.eborder_plan_time;
	def.eborder_actual_time  = msg.eborder_actual_time;
	def.sborder_sta          = msg.sborder_sta;
	def.eborder_sta          = msg.eborder_sta;
	def.start_sta_name	= msg.start_sta_name;
	def.end_sta_name	= msg.end_sta_name;

	def.from_conid           = msg.from_conid;
	def.to_conid             = msg.to_conid;

	def.priority             = msg.priority;
	def.train_length         = msg.train_length;
	def.train_weight         = msg.train_weight;
	def.status               = msg.status;
	def.space_rank           = msg.space_rank;
	def.length_rank          = msg.length_rank;
	def.super_rank           = msg.super_rank;
	def.military_rank        = msg.military_rank;
	def.engine_utility       = msg.engine_utility;
	def.side_utility         = msg.side_utility;
	
	def.no_meet_flag		 = 0;
	def.adjust_begin_sta     = msg.adjust_begin_sta;
	def.adjust_end_sta       = msg.adjust_end_sta;
	def.adjust_flag          = msg.adjust_flag;

	def.stop_abs_time = msg.adjust_flag>>16;
	def.adjust_flag = msg.adjust_flag & 0xffff;

	def.train_source         = msg.train_source;

	def.b_elec_rank			 = false;
}

void CopyTrainRecord(TgTRAIN_RECORD& rec, const MsgTrainRecordOCX& msg)
{
	rec.opmsGiveStationText = msg.opms_station_text_state;
	rec.szOpmsStationText = msg.opms_station_text;
	rec.szStationText = msg.station_text;
	rec.szAbsText	=	msg.abs_text;
	rec.szAlarmText = msg.alarm_text;
	rec.szPlanNote = msg.plan_note;

	rec.arrive_train_id=          msg.arrive_train_id;
	rec.original_arrive_train_id= msg.original_arrive_train_id;
	rec.depart_train_id=          msg.depart_train_id;
	rec.original_depart_train_id= msg.original_depart_train_id;
	
	rec.station            = msg.station;
	rec.entry              = msg.entry;
	if(255==rec.entry)
		rec.entry=0;
	rec.exit               = msg.exit;
	if(255==rec.exit)
		rec.exit=0;

	rec.arrive_side        = msg.arrive_side;
	rec.plan_arrive_side   = msg.plan_arrive_side;
	rec.depart_side        = msg.depart_side;
	rec.plan_depart_side   = msg.plan_depart_side;
	if(!IS_VALID_SIDE(rec.arrive_side)) // 20161105 非法股道固定设置为255
		rec.arrive_side = 255;
	if(!IS_VALID_SIDE(rec.plan_arrive_side))
		rec.plan_arrive_side = 255;
	if(!IS_VALID_SIDE(rec.depart_side))
		rec.depart_side = 255;
	if(!IS_VALID_SIDE(rec.plan_depart_side))
		rec.plan_depart_side = 255;

	rec.min_stop_time      = msg.min_stop_time;
	rec.arrive             = msg.arrive;
	rec.depart             = msg.depart;

	
	rec.plan_arrive        = msg.plan_arrive;
	rec.plan_depart        = msg.plan_depart;
	rec.stop_reason        = msg.stop_reason;
	rec.late_reason        = msg.late_reason;
	rec.early_depart_time  = msg.early_depart_time;
	rec.run_fast_slow_time = msg.run_fast_slow_time;
	rec.route_flag         = msg.route_flag;
	rec.speed_limit        = msg.speed_limit;
	rec.stop_condition     = msg.stop_condition;
	rec.adjust_status      = msg.adjust_status;

	rec.super_interval_ltime = msg.super_interval_ltime;
	rec.spec_run_time      = msg.spec_run_time;
	
	rec.stop_abs_position   = msg.special_position;
	rec.stop_abs_s_time= msg.special_time_1;
	if(rec.stop_abs_s_time&0x8000)
	{
		rec.stop_abs_s_time = rec.stop_abs_s_time&0x7FFF;
	}
	else
	{
		rec.stop_abs_s_time = rec.stop_abs_s_time/60;
	}
	if(rec.stop_abs_s_time>2880)
		rec.stop_abs_s_time=2880;
   
	rec.stop_abs_e_time     = msg.special_time_2; 
	if(rec.stop_abs_e_time&0x8000)
	{
		rec.stop_abs_e_time = rec.stop_abs_e_time&0x7FFF;
	}
	else
	{
		rec.stop_abs_e_time = rec.stop_abs_e_time/60;
	}

	if(rec.stop_abs_e_time>2880)
		rec.stop_abs_e_time=2880;

	rec.special_train_index = msg.special_train_index;
	rec.data_source         = msg.data_source; 

	rec.flag                = msg.flag;
	rec.no_meet_reason        =msg.can_not_meet;
	rec.rank                 = msg.rank;

	rec.in_station_index = msg.in_station_index;
	rec.out_station_index = msg.out_station_index;
	

	///////////////////////////////////////////
	for (int i=0; i<msg.abs_stop_count; i++)
	{
		ABS_STOP stop;
		stop.flag		= msg.abs_stop[i].flag;
		stop.position   = msg.abs_stop[i].position;
		stop.start_time = msg.abs_stop[i].start_time;
		stop.stop_time  = msg.abs_stop[i].stop_time;
		
		rec.abs_stop.push_back(stop); 
	}
	rec.display_status = msg.display_status;
}

void CopyTrainEngine(TgTRAIN_ENGINE_INFO& def, const MsgTrainEngineInfo& msg)
{
	def.engine_id	  = msg.engine_id;
	def.engine_name	  = msg.engine_type;
	def.engine_status = msg.engine_status;
	def.attach_sta    = msg.attach_sta;
	def.unattach_sta  = msg.unattach_sta;
}

BOOL  CopyTrainFromMsg(CTG_TRAIN& train,const MsgTrainOCX& msg)
{
	// Ext
	train.m_nTrainIndex=msg.train_index;
	train.prev_train_index=msg.prev_train_index;
	if(msg.self_update==TRUE)
		train.myTrainInfo.b_self_update=true;
	else
		train.myTrainInfo.b_self_update=false;

	// Info
	CopyTrainInfo(train.myTrainInfo,msg.info);
		
	// Records
	for(int i=0;i<msg.rec_count;i++)
	{
		TgTRAIN_RECORD rec;
		CopyTrainRecord(rec,msg.rec_ptr[i]);
		rec.rec_index = i;
		train.myRecord.push_back(rec);
	}

	// Engineids
	for(int i=0;i<msg.engine_count;i++)
	{
		TgTRAIN_ENGINE_INFO rec;
		CopyTrainEngine(rec,msg.engine_ptr[i]);
		train.myEngine.push_back(rec);
	}
	
	return true;
}

BOOL  CopyBlockFromMsg(CTg_BLOCK& block,const MsgSchdBlockOCX& msg)
{
	block.Clear();
	// Ext
	block.mark_index=msg.mark_index;
	block.station1=msg.station1;
	block.station2=msg.station2;
	block.direction=msg.direction;

	// msg
	if(msg.text_len>=MAX_SCHD_BLOCK_TEXT_LEN)
	{
		strncpy(block.text, msg.text, MAX_SCHD_BLOCK_TEXT_LEN-1);
		block.text[MAX_SCHD_BLOCK_TEXT_LEN-1]=0;
	}
	else
	{
		strncpy(block.text, msg.text, msg.text_len);
		block.text[msg.text_len]=0;
	}
	block.start_time=msg.start_time;
	block.end_time=msg.end_time;
	block.flag=msg.flag;
	block.type=msg.type;
	block.line=msg.dir;
	block.station=msg.station;
	block.index=msg.index;
	block.position=msg.position;
	block.s_meter=msg.s_meter;
	block.e_meter=msg.e_meter;

	block.e_kilo=msg.e_kilo;

	block.slow_time=msg.slow_time;
	block.adjust_status=msg.adjust_status;
	block.combstation=msg.basic_index;

	int count=0;
	for(int i=0; i<msg.ele_count && i<MAX_BLOCK_ELE_COUNT; i++)
	{
		block.ele_number[i]=msg.ele_number[i];
		count++;
	}
	block.count=count;
		
	return true;
}

