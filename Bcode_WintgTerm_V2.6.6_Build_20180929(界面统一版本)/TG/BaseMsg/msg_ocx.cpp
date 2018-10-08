
#define EXTERN_LINKAGE_DEFINE
#include "msgid.h"
#include "msg_ocx.h"

CLBTH_NAMESPACE_BEGIN
//////////////////////////////////////////////////////////////////////
IMPLEMENT_CLBTH(MsgTrainRecordOCX, MSGID_MsgTrainRecordOCX)
IMPLEMENT_SERIALIZE_OPERATOR(MsgTrainRecordOCX)
MsgTrainRecordOCX::MsgTrainRecordOCX(int version) : m_Version(version)
{
	opms_station_text_state=0;
	abs_text[0] = alarm_text[0] = station_text[0] = 0;
	opms_station_text[0] = 0;
	plan_note[0]=0;

	arrive_train_id[0] = original_arrive_train_id[0] = 0;
	depart_train_id[0] = original_depart_train_id[0] = 0;
	abs_stop_count = 0;	
	arrive = depart = 0;

	in_station_index=0;
	out_station_index=0;

	late_reason=0;
	stop_reason=0;
	display_status=0;
}
MsgTrainRecordOCX::~MsgTrainRecordOCX()
{
}

HRCODE MsgTrainRecordOCX::doSerialize(Archive& ar)
{
	short len;
	if(ar.isStoring())
	{
		ar << m_Version;

		ar << in_station_index;
		ar << out_station_index;

		ar << opms_station_text_state;
		
		AR_WRITE_STRING(opms_station_text, len);
		AR_WRITE_STRING(station_text, len);
		AR_WRITE_STRING(abs_text, len);
		AR_WRITE_STRING(alarm_text, len);
		AR_WRITE_STRING(plan_note, len);

		AR_WRITE_STRING(arrive_train_id, len);
		AR_WRITE_STRING(original_arrive_train_id, len);
		AR_WRITE_STRING(depart_train_id, len);
		AR_WRITE_STRING(original_depart_train_id, len);

		ar << station;
		ar << entry;
		ar << exit;
		ar << arrive_side;
		ar << plan_arrive_side;
		ar << depart_side;
		ar << plan_depart_side;
		ar << min_stop_time;

		ar << arrive;
		ar << depart;
		ar << plan_arrive;
		ar << plan_depart;
		ar << send_plan_time;

		ar << stop_reason;
		ar << late_reason;

		ar << early_depart_time;
		ar << run_fast_slow_time;
		ar << route_flag;
		ar << rank;
		ar << speed_limit;
		ar << stop_condition;
		ar << adjust_status;
		ar << spec_run_time;
		ar << can_not_meet;
		ar << super_interval_ltime;

		ar << special_position;

		ar << special_time_1;
		ar << special_time_2;
		ar << special_train_index;

		ar << data_source;
		ar << flag;
	
		//
		ar << abs_stop_count;
		for (int i=0; i<sizeof(abs_stop)/sizeof(abs_stop[0]); i++)
		{
			ar << abs_stop[i].flag;
			ar << abs_stop[i].position;
			ar << abs_stop[i].start_time;
			ar << abs_stop[i].stop_time;
		}
		ar << display_status;
	}
	else
	{
		ar >> m_Version;

		ar >> in_station_index;
		ar >> out_station_index;
		
		ar >> opms_station_text_state;
		
		AR_READ_STRING(opms_station_text, len);
		AR_READ_STRING(station_text, len);
		AR_READ_STRING(abs_text, len);
		AR_READ_STRING(alarm_text, len);
		AR_READ_STRING(plan_note, len);

		AR_READ_STRING(arrive_train_id, len);
		AR_READ_STRING(original_arrive_train_id, len);
		AR_READ_STRING(depart_train_id, len);
		AR_READ_STRING(original_depart_train_id, len);

		ar >> station;
		ar >> entry;
		ar >> exit;
		ar >> arrive_side;
		ar >> plan_arrive_side;
		ar >> depart_side;
		ar >> plan_depart_side;
		ar >> min_stop_time;

		ar >> arrive;
		ar >> depart;
		ar >> plan_arrive;
		ar >> plan_depart;
		ar >> send_plan_time;

		ar >> stop_reason;
		ar >> late_reason;

		ar >> early_depart_time;
		ar >> run_fast_slow_time;
		ar >> route_flag;
		ar >> rank;
		ar >> speed_limit;
		ar >> stop_condition;
		ar >> adjust_status;
		ar >> spec_run_time;
		ar >> can_not_meet;
		ar >> super_interval_ltime;

		ar >> special_position;

		ar >> special_time_1;
		ar >> special_time_2;
		ar >> special_train_index;

		ar >> data_source;
		ar >> flag;
	
		//
		ar >> abs_stop_count;
		for (int i=0; i<sizeof(abs_stop)/sizeof(abs_stop[0]); i++)
		{
			ar >> abs_stop[i].flag;
			ar >> abs_stop[i].position;
			ar >> abs_stop[i].start_time;
			ar >> abs_stop[i].stop_time;
		}
		ar >> display_status;
	}

	return HRCODE(HRCODE::HC_SUCCESS);
}

size_t MsgTrainRecordOCX::countObjectLength()
{
	size_t t= sizeof(abs_stop_count)+sizeof(abs_stop);
	
	return t+sizeof(in_station_index)
		+sizeof(out_station_index)
		+sizeof(opms_station_text_state)
		+sizeof(short)+strlen(opms_station_text)
		+sizeof(short)+strlen(station_text)
		+sizeof(short)+strlen(abs_text)
		+sizeof(short)+strlen(alarm_text)
		+sizeof(short)+strlen(plan_note)
		
		+sizeof(short)+strlen(arrive_train_id)
		+sizeof(short)+strlen(original_arrive_train_id)
		+sizeof(short)+strlen(depart_train_id)
		+sizeof(short)+strlen(original_depart_train_id)

		+sizeof(station)
		+sizeof(entry)
		+sizeof(exit)

		+sizeof(arrive_side)
		+sizeof(plan_arrive_side)
		+sizeof(depart_side)
		+sizeof(plan_depart_side)
		+sizeof(min_stop_time)

		+sizeof(arrive)
		+sizeof(depart)
		+sizeof(plan_arrive)
		+sizeof(plan_depart)
		+sizeof(send_plan_time)

		+sizeof(stop_reason)
		+sizeof(late_reason)

		+sizeof(early_depart_time)
		+sizeof(run_fast_slow_time)
		+sizeof(route_flag)
		+sizeof(rank)
		+sizeof(speed_limit)
		+sizeof(stop_condition)
		+sizeof(adjust_status)
		+sizeof(spec_run_time)
		+sizeof(can_not_meet)
		+sizeof(super_interval_ltime)

		+sizeof(special_position)

		+sizeof(special_time_1)
		+sizeof(special_time_2)
		+sizeof(special_train_index)

		+sizeof(data_source)
		+sizeof(flag)
		+sizeof(display_status);
}

///////////////
IMPLEMENT_CLBTH(MsgTrainOCX, MSGID_MsgTrainOCX)
IMPLEMENT_SERIALIZE_OPERATOR(MsgTrainOCX)
MsgTrainOCX::MsgTrainOCX()
{
	rec_ptr = NULL;
	engine_ptr = NULL;

	engine_count = rec_count = 0;
		
	self_update=0;
	train_index=0;
	prev_train_index=0;
}
MsgTrainOCX::~MsgTrainOCX()
{
	delete[] rec_ptr;
	delete[] engine_ptr;
}

HRCODE MsgTrainOCX::doSerialize(Archive& ar)
{
	if (HRCODE(HRCODE::HC_SUCCESS) != info.serialize(ar)) 
		return HRCODE(HRCODE::HC_FAIL);

	if(ar.isStoring())
	{
		ar << rec_count << engine_count;
	}
	else
	{
		ar >> rec_count >> engine_count;
		if (rec_count > 0)
			rec_ptr = new MsgTrainRecordOCX[rec_count];

		if (rec_count <=0 || NULL == rec_ptr)
				return HRCODE(HRCODE::HC_FAIL);
		
		if (engine_count > 0)
			engine_ptr = new MsgTrainEngineInfo[engine_count];


		if (engine_count <0 || (engine_count>0 && NULL == engine_ptr))
			return HRCODE(HRCODE::HC_FAIL);
		
	}

	int i = 0;
	for (i=0; i<rec_count; i++)
		if (HRCODE(HRCODE::HC_SUCCESS) != rec_ptr[i].serialize(ar) )
			return HRCODE(HRCODE::HC_FAIL);
	
	for (i=0; i<engine_count; i++)
		if (HRCODE(HRCODE::HC_SUCCESS) != engine_ptr[i].serialize(ar) )
			return HRCODE(HRCODE::HC_FAIL);

	if(ar.isStoring())
	{
		ar << train_index;
		ar << prev_train_index;
		ar << self_update;
	}
	else
	{
		ar >> train_index;
		ar >> prev_train_index;
		ar >> self_update;
	}

	return HRCODE(HRCODE::HC_SUCCESS);
}

size_t MsgTrainOCX::countObjectLength()
{
	size_t t = info.getObjectLength();
	t += sizeof(rec_count);
	int i = 0;
	for (i=0; i<rec_count; i++)
		t+=rec_ptr[i].getObjectLength();

	t += sizeof(engine_count);
	for (i=0; i<engine_count; i++)
		t+=engine_ptr[i].getObjectLength();

	t+=sizeof(self_update)+sizeof(train_index)+sizeof(prev_train_index);
	return t;
}

///////////////
IMPLEMENT_CLBTH(MsgSchdBlockOCX, MSGID_MsgSchdBlockOCX)
IMPLEMENT_SERIALIZE_OPERATOR(MsgSchdBlockOCX)
MsgSchdBlockOCX::MsgSchdBlockOCX(int version) : m_Version(version)
{
	self_update=0;
	mark_index=0;

	station1=0;
	station2=0;
	direction=0;
}
MsgSchdBlockOCX::~MsgSchdBlockOCX()
{
}

HRCODE MsgSchdBlockOCX::doSerialize(Archive& ar)
{
	if (HRCODE(HRCODE::HC_SUCCESS) != MsgSchdBlock::doSerialize(ar))
		return HRCODE(HRCODE::HC_FAIL);
	
	if(ar.isStoring())
	{
		ar << self_update;
		ar << mark_index;

		ar << station1;
		ar << station2;
		ar << direction;
	}
	else
	{
		ar >> self_update;
		ar >> mark_index;

		ar >> station1;
		ar >> station2;
		ar >> direction;
	}

	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgSchdBlockOCX::countObjectLength()
{
	return MsgSchdBlock::countObjectLength()
		+sizeof(self_update)+sizeof(mark_index)
		+sizeof(station1)+sizeof(station2)+sizeof(direction);
}

///////////// END /////////////
CLBTH_NAMESPACE_END

