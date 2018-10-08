#include "../message/message.h"
#include "traintextdef.h"
#include "blockdef.h"

short TrainCopy(TrainInfo& def, const MsgTrainInfo& msg)
{
	STR_CPY(def.train_name,  msg.train_name);
	def.train_key             = msg.train_key;
	def.basic_train_index    = msg.basic_train_index;   
	def.type                 = msg.type;
	def.run_type             = msg.run_type;
	def.sborder_plan_time    = msg.sborder_plan_time;
	def.sborder_actual_time  = msg.sborder_actual_time;
	def.eborder_plan_time    = msg.eborder_plan_time;
	def.eborder_actual_time  = msg.eborder_actual_time;
	def.sborder_sta          = msg.sborder_sta;
	def.eborder_sta          = msg.eborder_sta;
	STR_CPY(def.start_sta_name, msg.start_sta_name);
	STR_CPY(def.end_sta_name, msg.end_sta_name);

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
	def.speed_limit          = msg.speed_limit;
	def.engine_utility       = msg.engine_utility;
	def.side_utility         = msg.side_utility;
	def.can_not_meet         = msg.can_not_meet;
	def.super_interval_ltime = msg.super_interval_ltime;
	def.adjust_begin_sta     = msg.adjust_begin_sta;
	def.adjust_end_sta       = msg.adjust_end_sta;
	def.adjust_flag          = msg.adjust_flag;
	def.train_source         = msg.train_source;
	
	return SUCCESS;
}
short TrainCopy(TrainEngineInfo& def, const MsgTrainEngineInfo& msg)
{
	if (msg.engine_count > sizeof(def.engine_status)/sizeof(def.engine_status[0]))
		return FAILED;

	def.engine_count = msg.engine_count;
	for (int i=0; i<def.engine_count; i++)
	{
		STR_CPY(def.engine_id[i], msg.engine_id[i]);
		STR_CPY(def.engine_type[i], msg.engine_type[i]);
		def.engine_status[i] = msg.engine_status[i];
		def.attach_sta[i]    = msg.attach_sta[i];
		def.unattach_sta[i]  = msg.unattach_sta[i];
	}

	return SUCCESS;
}
static void abs_stop_cp(TrainAbsStop& rec, const MsgTrainAbsStop& msg)
{
	rec.position   = msg.position;
	rec.start_time = msg.start_time;
	rec.stop_time  = msg.stop_time;
}
short TrainCopy(TrainRecord& rec, const MsgTrainRecord& msg)
{
	STR_CPY(rec.arrive_train_id,          msg.arrive_train_id);
	STR_CPY(rec.original_arrive_train_id, msg.original_arrive_train_id);
	STR_CPY(rec.depart_train_id,          msg.depart_train_id);
	STR_CPY(rec.original_depart_train_id, msg.original_depart_train_id);

	rec.station            = msg.station;
	rec.entry              = msg.entry;
	rec.exit               = msg.exit;
	rec.arrive_side        = msg.arrive_side;
	rec.plan_arrive_side   = msg.plan_arrive_side;
	rec.depart_side        = msg.depart_side;
	rec.plan_depart_side   = msg.plan_depart_side;
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
	rec.spec_run_time      = msg.spec_run_time;
	
	abs_stop_cp(rec.stop_out_of_signal, msg.stop_out_of_signal);
	if (msg.abs_stop_count > sizeof(rec.abs_stop)/sizeof(rec.abs_stop[0]))
		return FAILED;

	rec.abs_stop_count = msg.abs_stop_count;
	for (int i=0; i<rec.abs_stop_count; i++)
		abs_stop_cp(rec.abs_stop[i], msg.abs_stop[i]);

	rec.special_position   = msg.special_position;
	rec.special_time_1     = msg.special_time_1;
	rec.special_time_2     = msg.special_time_2; 
	rec.special_train_index = msg.special_train_index;
	rec.data_source         = msg.data_source; 
	rec.flag                = msg.flag;

	rec.station_text = -1;
	rec.abs_text     = -1;

	return SUCCESS;
}
static short record_text_cp(cTrainTextDef& def, short rec_count, const MsgTrainRecord* rec_ptr, const short* rec_index)
{
	def.m_RecCount = rec_count;
	for (int i=0; i<def.m_RecCount; i++)
	{
		const MsgTrainRecord& rec = rec_ptr[i];
		if (FAILED == TrainCopy(def.m_Record[i], rec))
			return FAILED;

		if (NULL == rec_index)
			def.m_pIndexList[i] = i;
		else
			def.m_pIndexList[i] = rec_index[i];

		char * text = def.GetStationText(i);
		if (!rec.station_text_len)
			text[0]=0;
		else
		{
			int len = rec.station_text_len;
			if (len >= def.m_MaxTextLength) len = def.m_MaxTextLength-1;
			memcpy(text, rec.station_text, len);
			text[len] = 0;
		}

		text = def.GetAbsText(i);
		if (!rec.abs_text_len)
			text[0]=0;
		else
		{
			int len = rec.abs_text_len;
			if (len >= def.m_MaxTextLength) len = def.m_MaxTextLength-1;
			memcpy(text, rec.abs_text, len);
			text[len] = 0;
		}
	}
	return SUCCESS;
}
short TrainCopy(cTrainTextDef& def, const MsgTrain& msg)
{
	if (FAILED == TrainCopy(def.m_TrainInfo,msg.info))
		return FAILED;

	if (FAILED == TrainCopy(def.m_Engine,msg.engine))
		return FAILED;

	if (FAILED == record_text_cp(def, msg.rec_count, msg.rec_ptr, NULL))
		return FAILED;

	return SUCCESS;
}
short TrainCopy(cTrainTextDef& def, const MsgTrainUpdateRecord& msg)
{
	if (FAILED == record_text_cp(def, msg.rec_cnt, msg.rec_ptr, msg.rec_index))
		return FAILED;

	return SUCCESS;
}

////////////////////////////////////////////////////////////
short TrainCopy(MsgTrainInfo& msg, const TrainInfo& def)
{
	STR_CPY(msg.train_name, def.train_name);

	msg.train_key             = def.train_key;
	msg.basic_train_index    = def.basic_train_index;
	msg.type                 = def.type;
	msg.run_type             = def.run_type;
	msg.sborder_plan_time    = def.sborder_plan_time;
	msg.sborder_actual_time  = def.sborder_actual_time;
	msg.eborder_plan_time    = def.eborder_plan_time;
	msg.eborder_actual_time  = def.eborder_actual_time;
	msg.sborder_sta          = def.sborder_sta;
	msg.eborder_sta          = def.eborder_sta;
	STR_CPY(msg.start_sta_name, def.start_sta_name) 
	STR_CPY(msg.end_sta_name,   def.end_sta_name) 
	
	msg.from_conid           = def.from_conid;
	msg.to_conid             = def.to_conid;
	msg.priority             = def.priority;
	msg.train_length         = def.train_length;
	msg.train_weight         = def.train_weight;
	msg.status               = def.status;
	msg.space_rank           = def.space_rank;
	msg.length_rank          = def.length_rank;
	msg.super_rank           = def.super_rank;
	msg.military_rank        = def.military_rank;
	msg.speed_limit          = def.speed_limit;
	msg.engine_utility       = def.engine_utility;
	msg.side_utility         = def.side_utility;
	msg.can_not_meet         = def.can_not_meet;
	msg.super_interval_ltime = def.super_interval_ltime;
	msg.adjust_begin_sta     = def.adjust_begin_sta;
	msg.adjust_end_sta       = def.adjust_end_sta;
	msg.adjust_flag          = def.adjust_flag;
	msg.train_source         = def.train_source;
	
	return SUCCESS;

}
short TrainCopy(MsgTrainEngineInfo& msg, const TrainEngineInfo& def)
{
	msg.engine_count = def.engine_count;

	for (int i=0; i<def.engine_count; i++)
	{
		STR_CPY(msg.engine_id[i],   def.engine_id[i]);
		STR_CPY(msg.engine_type[i], def.engine_type[i]);
		msg.engine_status[i] = def.engine_status[i];
		msg.attach_sta[i]    = def.attach_sta[i];
		msg.unattach_sta[i]  = def.unattach_sta[i];
	}

	return SUCCESS;
}
static void abs_stop_cp(MsgTrainAbsStop& msg, const TrainAbsStop& rec)
{
	msg.position   = rec.position;
	msg.start_time = rec.start_time;
	msg.stop_time  = rec.stop_time;
}
short TrainCopy(MsgTrainRecord& msg, const TrainRecord& rec)
{
	STR_CPY(msg.arrive_train_id,          rec.arrive_train_id);
	STR_CPY(msg.original_arrive_train_id, rec.original_arrive_train_id);
	STR_CPY(msg.depart_train_id,          rec.depart_train_id);
	STR_CPY(msg.original_depart_train_id, rec.original_depart_train_id);

	msg.station            = rec.station;
	msg.entry              = rec.entry;
	msg.exit               = rec.exit;
	msg.arrive_side        = rec.arrive_side;
	msg.plan_arrive_side   = rec.plan_arrive_side;
	msg.depart_side        = rec.depart_side;
	msg.plan_depart_side   = rec.plan_depart_side;
	msg.min_stop_time      = rec.min_stop_time;
	msg.arrive             = rec.arrive;
	msg.depart             = rec.depart;
	msg.plan_arrive        = rec.plan_arrive;
	msg.plan_depart        = rec.plan_depart;
	msg.stop_reason        = rec.stop_reason;
	msg.late_reason        = rec.late_reason;
	msg.early_depart_time  = rec.early_depart_time;
	msg.run_fast_slow_time = rec.run_fast_slow_time;
	msg.route_flag         = rec.route_flag;
	msg.speed_limit        = rec.speed_limit;
	msg.stop_condition     = rec.stop_condition;
	msg.adjust_status      = rec.adjust_status;
	msg.spec_run_time      = rec.spec_run_time;

	abs_stop_cp(msg.stop_out_of_signal, rec.stop_out_of_signal);
	if (rec.abs_stop_count > sizeof(msg.abs_stop)/sizeof(msg.abs_stop[0]))
		return FAILED;
	msg.abs_stop_count     = rec.abs_stop_count;
	for (int i=0; i<msg.abs_stop_count; i++)
		abs_stop_cp(msg.abs_stop[i], rec.abs_stop[i]);

	msg.special_position   = rec.special_position;
	msg.special_time_1     = rec.special_time_1;
	msg.special_time_2     = rec.special_time_2; 
	msg.special_train_index = rec.special_train_index;
	msg.data_source         = rec.data_source; 
	msg.flag                = rec.flag;

	return SUCCESS;
}

short TrainCopy(MsgTrain& msg, const cTrainTextDef& def)
{
	if (FAILED == TrainCopy(msg.info,def.m_TrainInfo))
		return FAILED;

	if (FAILED == TrainCopy(msg.engine,def.m_Engine))
		return FAILED;

	msg.rec_count = def.m_RecCount;
	msg.rec_ptr  = new MsgTrainRecord[def.m_RecCount];
	if (NULL == msg.rec_ptr) return FAILED;

	for (int i=0; i<def.m_RecCount; i++)
	{
		MsgTrainRecord& rec = msg.rec_ptr[i];

		if (FAILED == TrainCopy(rec, def.m_Record[i]))
			return FAILED;

		char* text = def.GetStationText(i);
		if (!text[0])
			rec.station_text_len = 0;
		else
		{
			rec.station_text_len = strlen(text);
			if (rec.station_text_len > MAX_TRAIN_TEXT_LENGTH)
				rec.station_text_len = MAX_TRAIN_TEXT_LENGTH;
			memcpy(rec.station_text, text, rec.station_text_len);
		}

		text = def.GetAbsText(i);
		if (!text[0])
			rec.abs_text_len = 0;
		else
		{
			rec.abs_text_len = strlen(text);
			if (rec.abs_text_len > MAX_TRAIN_TEXT_LENGTH)
				rec.abs_text_len = MAX_TRAIN_TEXT_LENGTH;
			memcpy(rec.abs_text, text, rec.abs_text_len);
		}
	}
	return SUCCESS;
}

////////////////////////////////////////////////////
short BlockCopy(MsgSchdBlock& msg, const cBlockDef& def)
{
	msg.start_time = def.start_time;
	msg.end_time   = def.end_time;
	msg.type       = def.type;
	msg.flag       = def.flag;
	msg.dir        = def.dir;
	msg.station    = def.station;
	msg.index      = def.index;
	msg.position   = def.position;
	msg.ele_count  = def.ele_count;
	for (int i=0; i<msg.ele_count; i++)
		msg.ele_number[i] = def.ele_number[i];

	msg.text_len =  strlen(def.text);
	memcpy(msg.text, def.text, msg.text_len);

	msg.s_meter   = def.s_meter;
	msg.e_meter   = def.e_meter;
	msg.e_kilo    = def.e_kilo;
	msg.slow_time = def.slow_time;

	return SUCCESS;
}
short BlockCopy(cBlockDef& def, const MsgSchdBlock& msg)
{
	def.start_time = msg.start_time;
	def.end_time   = msg.end_time;
	def.type       = msg.type;
	def.flag       = msg.flag;
	def.dir        = msg.dir;
	def.station    = msg.station;
	def.index      = msg.index;
	def.position   = msg.position;
	def.ele_count  = msg.ele_count;
	for (int i=0; i<msg.ele_count; i++)
		def.ele_number[i] = msg.ele_number[i];

	STR_CPY(def.text, msg.text);

	def.s_meter   = msg.s_meter;
	def.e_meter   = msg.e_meter;
	def.e_kilo    = msg.e_kilo;
	def.slow_time = msg.slow_time;

	return SUCCESS;
}
