
#define EXTERN_LINKAGE_DEFINE
#include "msgid.h"
#include "msg_schd.h"

CLBTH_NAMESPACE_BEGIN


///////////////
IMPLEMENT_CLBTH(MsgTrainInfo, MSGID_MsgTrainInfo)
IMPLEMENT_SERIALIZE_OPERATOR(MsgTrainInfo)
MsgTrainInfo::MsgTrainInfo(int version) : m_Version(version)
{
	CLBTH_ZERO_INIT_OBJECT(BaseMsg);
	comment_len = 0;
	comment = NULL;
}
MsgTrainInfo::~MsgTrainInfo()
{
	delete[] comment;
}
class MsgTrainInfo& MsgTrainInfo::operator=(class MsgTrainInfo const & msg)
{
	memcpy(train_name, msg.train_name, sizeof(train_name));
	train_key            = msg.train_key;
	gpc_index            = msg.gpc_index;
	basic_index          = msg.basic_index;
	shift_index          = msg.shift_index;
	memcpy(shift_string, msg.shift_string, sizeof(shift_string));
	update_time          = msg.update_time;
	delete_flag          = msg.delete_flag;
	type                 = msg.type;       
	run_type             = msg.run_type;   
	start_plan_time      = msg.start_plan_time;     
	start_actual_time    = msg.start_actual_time;   
	sborder_plan_time    = msg.sborder_plan_time;   
	sborder_actual_time  = msg.sborder_actual_time; 
	eborder_plan_time    = msg.eborder_plan_time;   
	eborder_actual_time  = msg.eborder_actual_time; 
	sborder_sta          = msg.sborder_sta;         
	eborder_sta          = msg.eborder_sta;         
	memcpy(start_sta_name, msg.start_sta_name, sizeof(start_sta_name)); 
	memcpy(end_sta_name, msg.end_sta_name, sizeof(end_sta_name)); 
	from_conid           = msg.from_conid; 
	to_conid             = msg.to_conid; 
	priority             = msg.priority;
	train_length         = msg.train_length;
	train_weight         = msg.train_weight;
	status               = msg.status;               
	space_rank           = msg.space_rank;
	length_rank          = msg.length_rank;
	super_rank           = msg.super_rank;
	military_rank        = msg.military_rank;
	engine_utility       = msg.engine_utility;
	side_utility         = msg.side_utility;
	adjust_begin_sta     = msg.adjust_begin_sta;
	adjust_end_sta       = msg.adjust_end_sta;
	adjust_flag          = msg.adjust_flag;
	train_source         = msg.train_source;
	comment_len          = msg.comment_len;
	if (comment_len>0)
	{
		comment = new char[comment_len+1];
		memcpy(comment, msg.comment, comment_len);
		comment[comment_len] = 0;
	}
	else
		comment = NULL;

	return *this;
}

HRCODE MsgTrainInfo::doSerialize(Archive& ar)
{
	char len ;
	if(ar.isStoring())
	{
		ar << m_Version;
		AR_WRITE_STRING(train_name, len);
		ar << train_key << gpc_index << basic_index << shift_index;
		AR_WRITE_STRING(shift_string, len);
		ar << update_time << delete_flag;

		ar << type << run_type 
			<< start_plan_time << start_actual_time 
			<< sborder_plan_time << sborder_actual_time 
			<< eborder_plan_time << eborder_actual_time 
			<< sborder_sta << eborder_sta;
		AR_WRITE_STRING(start_sta_name, len);
		AR_WRITE_STRING(end_sta_name,   len);

		ar << from_conid << to_conid <<	priority 
			<< train_length <<	train_weight <<	status;

		ar << space_rank << length_rank << super_rank << military_rank;
		ar << engine_utility << side_utility;

		ar << adjust_begin_sta << adjust_end_sta << adjust_flag << train_source;
		ar << comment_len;
		ar.write(comment, comment_len);
	}
	else
	{
		ar >> m_Version;
		AR_READ_STRING(train_name, len);
		ar >> train_key >> gpc_index >> basic_index >> shift_index;
		AR_READ_STRING(shift_string, len);
		ar >> update_time >> delete_flag;

		ar >> type >> run_type 
			>> start_plan_time >> start_actual_time 
			>> sborder_plan_time >> sborder_actual_time 
			>> eborder_plan_time >> eborder_actual_time 
			>> sborder_sta >> eborder_sta;
		AR_READ_STRING(start_sta_name, len);
		AR_READ_STRING(end_sta_name,   len);

		ar >> from_conid >> to_conid >>	priority 
			>> train_length >>	train_weight >>	status;

		ar >> space_rank >> length_rank >> super_rank >> military_rank;
		ar >> engine_utility >> side_utility;

		ar >> adjust_begin_sta >> adjust_end_sta >> adjust_flag >> train_source;
		if (comment_len || comment)
			return HRCODE(HRCODE::HC_FAIL);

		ar >> comment_len;
		if (comment_len+1 <= 0)
			return HRCODE(HRCODE::HC_FAIL);

		comment = new char[comment_len+1];
		if (NULL == comment)
			return HRCODE(HRCODE::HC_FAIL);

		ar.read(comment, comment_len);
		comment[comment_len] =0 ;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgTrainInfo::countObjectLength()
{
	size_t t;
	t = sizeof(m_Version)+strlen(train_name)+sizeof(char);
	t += sizeof(train_key)+sizeof(gpc_index)+sizeof(basic_index)+sizeof(shift_index);
	t += strlen(shift_string)+sizeof(char);
	t += sizeof(update_time)+sizeof(delete_flag)
			+sizeof(type)+sizeof(run_type)
			+sizeof(start_plan_time)+sizeof(start_actual_time)
			+sizeof(sborder_plan_time)+sizeof(sborder_actual_time)
			+sizeof(eborder_plan_time)+sizeof(eborder_actual_time) 
			+sizeof(sborder_sta)+sizeof(eborder_sta);
	t+=strlen(start_sta_name)+sizeof(char);
	t+=strlen(end_sta_name)+sizeof(char);

	t+=sizeof(from_conid)+sizeof(to_conid)+sizeof(priority);
	t+=sizeof(train_length)+sizeof(train_weight)+sizeof(status);
	t+=sizeof(space_rank)+sizeof(length_rank)+sizeof(super_rank)+sizeof(military_rank);
	t+=sizeof(engine_utility)+sizeof(side_utility);

	t+=sizeof(adjust_begin_sta)+sizeof(adjust_end_sta)+sizeof(adjust_flag)+sizeof(train_source);

	t+=sizeof(comment_len)+comment_len;
	return t;
}
HRCODE MsgTrainInfo::SetComment(const char* c)
{
	if (NULL != comment)
	{
		delete[] comment;
		comment = NULL;
	}

	if (NULL == c)	return HRCODE(HRCODE::HC_SUCCESS);
	comment_len = strlen(c)+1;
	comment = new char[comment_len];
	if (NULL == comment)
		return HRCODE(HRCODE::HC_FAIL);

	memcpy(comment, c, comment_len-1);
	comment[comment_len-1] = 0;

	return HRCODE(HRCODE::HC_SUCCESS);
}

///////////////
IMPLEMENT_CLBTH(MsgTrainEngineInfo, MSGID_MsgTrainEngineInfo)
IMPLEMENT_SERIALIZE_OPERATOR(MsgTrainEngineInfo)
MsgTrainEngineInfo::MsgTrainEngineInfo(int version) : m_Version(version)
{
	CLBTH_ZERO_INIT_OBJECT(BaseMsg);
}
MsgTrainEngineInfo::~MsgTrainEngineInfo()
{
}
class MsgTrainEngineInfo& MsgTrainEngineInfo::operator=(class MsgTrainEngineInfo const & msg)
{
	memcpy(engine_id, msg.engine_id, sizeof(engine_id));
	memcpy(engine_type, msg.engine_type, sizeof(engine_type));
	engine_status = msg.engine_status;
	attach_sta    = msg.attach_sta;
	unattach_sta  = msg.unattach_sta;

	return *this;
}

HRCODE MsgTrainEngineInfo::doSerialize(Archive& ar)
{
	char len;

	if(ar.isStoring())
	{
		ar << m_Version;

		AR_WRITE_STRING(engine_id, len)
		AR_WRITE_STRING(engine_type, len)

		ar << engine_status << attach_sta << unattach_sta;
	}
	else
	{
		ar >> m_Version;

		AR_READ_STRING(engine_id, len)
		AR_READ_STRING(engine_type, len)

		ar >> engine_status >> attach_sta >> unattach_sta;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgTrainEngineInfo::countObjectLength()
{
	return sizeof(m_Version)
		+ strlen(engine_id)+sizeof(char)
		+ strlen(engine_type)+sizeof(char)
		+ sizeof(engine_status)+sizeof(attach_sta)+sizeof(unattach_sta);
}

///////////////
IMPLEMENT_CLBTH(MsgTrainAbsStop, MSGID_MsgTrainAbsStop)
IMPLEMENT_SERIALIZE_OPERATOR(MsgTrainAbsStop)
MsgTrainAbsStop::MsgTrainAbsStop(int version) : m_Version(version)
{
	CLBTH_ZERO_INIT_OBJECT(BaseMsg);
}
MsgTrainAbsStop::~MsgTrainAbsStop()
{
}
class MsgTrainAbsStop& MsgTrainAbsStop::operator=(class MsgTrainAbsStop const & msg)
{
	position   = msg.position;
	start_time = msg.start_time;
	stop_time  = msg.stop_time;

	return *this;
}

HRCODE MsgTrainAbsStop::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << m_Version;
		ar << position << start_time << stop_time;
	}
	else
	{
		ar >> m_Version;

		ar >> position >> start_time >> stop_time;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgTrainAbsStop::countObjectLength()
{
	return sizeof(m_Version)+sizeof(position)+sizeof(start_time)+sizeof(stop_time);
}

///////////////
IMPLEMENT_CLBTH(MsgTrainRecord, MSGID_MsgTrainRecord)
IMPLEMENT_SERIALIZE_OPERATOR(MsgTrainRecord)
MsgTrainRecord::MsgTrainRecord(int version) : m_Version(version)
{
	arrive_train_id[0] = original_arrive_train_id[0] = depart_train_id[0] = original_depart_train_id[0] = 0;
	abs_stop_count = 0;	
	arrive = depart = 0;
	for (int i=0; i<3; i++)
		text[i][0]=0;
}
MsgTrainRecord::~MsgTrainRecord()
{
}
class MsgTrainRecord& MsgTrainRecord::operator=(class MsgTrainRecord const & msg)
{
	memcpy(arrive_train_id         ,msg.arrive_train_id         , sizeof(arrive_train_id         ));
	memcpy(original_arrive_train_id,msg.original_arrive_train_id, sizeof(original_arrive_train_id));
	memcpy(depart_train_id         ,msg.depart_train_id         , sizeof(depart_train_id         ));
	memcpy(original_depart_train_id,msg.original_depart_train_id, sizeof(original_depart_train_id));
	station              = msg.station             ;
	entry                = msg.entry               ;
	exit                 = msg.exit                ;
	arrive_side          = msg.arrive_side         ;
	plan_arrive_side     = msg.plan_arrive_side    ;
	depart_side          = msg.depart_side         ;
	plan_depart_side     = msg.plan_depart_side    ;
	min_stop_time        = msg.min_stop_time       ;
	arrive               = msg.arrive              ;
	depart               = msg.depart              ;
	plan_arrive          = msg.plan_arrive         ;
	plan_depart          = msg.plan_depart         ;
	send_plan_time       = msg.send_plan_time      ;
	stop_reason          = msg.stop_reason         ;
	late_reason          = msg.late_reason         ;
	early_depart_time    = msg.early_depart_time   ;
	run_fast_slow_time   = msg.run_fast_slow_time  ;
	route_flag           = msg.route_flag          ;
	rank                 = msg.rank                ;
	speed_limit          = msg.speed_limit         ;
	stop_condition       = msg.stop_condition      ;
	adjust_status        = msg.adjust_status       ;
	spec_run_time        = msg.spec_run_time       ;
	can_not_meet         = msg.can_not_meet        ;
	super_interval_ltime = msg.super_interval_ltime;
	special_position     = msg.special_position    ;
	special_time_1       = msg.special_time_1      ;
	special_time_2       = msg.special_time_2      ;
	special_train_index  = msg.special_train_index ;
	data_source          = msg.data_source         ;
	flag                 = msg.flag                ;
	
	stop_out_of_signal = msg.stop_out_of_signal;
	abs_stop_count = msg.abs_stop_count;

	int i = 0;
	for (i=0; i<abs_stop_count; i++)
		abs_stop[i] = msg.abs_stop[i];

	for (i=0; i<3; i++)
		memcpy(text[i], msg.text[i], sizeof(text[i]));

	return *this;
}
HRCODE MsgTrainRecord::doSerialize(Archive& ar)
{
	short len;
	if(ar.isStoring())
	{
		ar << m_Version;
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
	
		if (HRCODE(HRCODE::HC_SUCCESS) != stop_out_of_signal.serialize(ar))
			return HRCODE(HRCODE::HC_FAIL);

		AR_WRITE_COUNT(abs_stop_count, abs_stop);

		int i = 0;
		for (i=0; i<abs_stop_count; i++)
			if (HRCODE(HRCODE::HC_SUCCESS) != abs_stop[i].serialize(ar))
			return HRCODE(HRCODE::HC_FAIL);

		for (i=0; i<sizeof(text)/sizeof(text[0]); i++)
		{
			AR_WRITE_STRING(text[i], len);
		}
	}
	else
	{
		ar >> m_Version;

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
	
		if (HRCODE(HRCODE::HC_SUCCESS) != stop_out_of_signal.serialize(ar))
			return HRCODE(HRCODE::HC_FAIL);

		AR_READ_COUNT(abs_stop_count, abs_stop);

		int i = 0;
		for (i=0; i<abs_stop_count; i++)
			if (HRCODE(HRCODE::HC_SUCCESS) != abs_stop[i].serialize(ar))
			return HRCODE(HRCODE::HC_FAIL);


		for (i=0; i<sizeof(text)/sizeof(text[0]); i++)
		{
			AR_READ_STRING(text[i], len);
		}
	}

	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgTrainRecord::countObjectLength()
{
	size_t t=0;
	for (int i=0; i<sizeof(text)/sizeof(text[0]); i++)
	{
		t += strlen(text[i]);
		t += sizeof(short);
	}

	return t+sizeof(short)+strlen(arrive_train_id)
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
		+sizeof(flag);
}

///////////////
IMPLEMENT_CLBTH(MsgTrain, MSGID_MsgTrain)
IMPLEMENT_SERIALIZE_OPERATOR(MsgTrain)
MsgTrain::MsgTrain()
{
	rec_ptr = NULL;
	engine_ptr = NULL;
	engine_count = rec_count = 0;
}
MsgTrain::~MsgTrain()
{
	delete[] rec_ptr;
	delete[] engine_ptr;
}
class MsgTrain& MsgTrain::operator=(class MsgTrain const & msg)
{
	info = msg.info;

	engine_count = msg.engine_count;
	if (engine_count >0)
		engine_ptr = new MsgTrainEngineInfo[engine_count];
	int i = 0;
	for (i=0; i<engine_count; i++)
		engine_ptr[i] = msg.engine_ptr[i];

	rec_count = msg.rec_count;
	if (rec_count >0)
		rec_ptr = new MsgTrainRecord[rec_count];
	for (i=0; i<rec_count; i++)
		rec_ptr[i] = msg.rec_ptr[i];

	return *this;
}
HRCODE MsgTrain::doSerialize(Archive& ar)
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
			rec_ptr = new MsgTrainRecord[rec_count];

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

	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgTrain::countObjectLength()
{
	size_t t = info.getObjectLength();
	t += sizeof(rec_count);
	int i = 0;
	for (i=0; i<rec_count; i++)
		t+=rec_ptr[i].getObjectLength();

	t += sizeof(engine_count);
	for (i=0; i<engine_count; i++)
		t+=engine_ptr[i].getObjectLength();

	return t;
}

///////////////
IMPLEMENT_CLBTH(MsgSchdStamp, MSGID_MsgSchdStamp)
IMPLEMENT_SERIALIZE_OPERATOR(MsgSchdStamp)
MsgSchdStamp::MsgSchdStamp(int version) : m_Version(version)
{
}
MsgSchdStamp::~MsgSchdStamp()
{
}
class MsgSchdStamp& MsgSchdStamp::operator=(class MsgSchdStamp const & msg)
{
	con_id    = msg.con_id;
	schd_type = msg.schd_type;
	index     = msg.index;
	
	ltime   = msg.ltime;
	operatr = msg.operatr;

	return *this;
}

HRCODE MsgSchdStamp::doSerialize(Archive& ar)
{	
	if(ar.isStoring())
	{
		ar << m_Version << schd_type << con_id << index << ltime << operatr;
	}
	else
	{
		ar >> m_Version >> schd_type >> con_id >> index >> ltime >> operatr;
	}

	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgSchdStamp::countObjectLength()
{
	return sizeof(m_Version)+sizeof(schd_type)+sizeof(con_id)+sizeof(index)+sizeof(ltime)+sizeof(operatr);
}

///////////////
IMPLEMENT_CLBTH(MsgTrainAdd, MSGID_MsgTrainAdd)
IMPLEMENT_SERIALIZE_OPERATOR(MsgTrainAdd)
MsgTrainAdd::MsgTrainAdd()
{
}
MsgTrainAdd::~MsgTrainAdd()
{
}
MsgTrainAdd::MsgTrainAdd(const MsgTrainUpdate & msg)
{
	stamp = msg.stamp;

	*(MsgTrain*)this = (const MsgTrain&) msg;
}
HRCODE MsgTrainAdd::doSerialize(Archive& ar)
{
	if (HRCODE(HRCODE::HC_SUCCESS) != MsgTrain::doSerialize(ar)
	 || HRCODE(HRCODE::HC_SUCCESS) != stamp.serialize(ar))
		return HRCODE(HRCODE::HC_FAIL);
	
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgTrainAdd::countObjectLength()
{
	return MsgTrain::countObjectLength()+stamp.getObjectLength();
}

///////////////
IMPLEMENT_CLBTH(MsgTrainDelete, MSGID_MsgTrainDelete)
IMPLEMENT_SERIALIZE_OPERATOR(MsgTrainDelete)
MsgTrainDelete::MsgTrainDelete()
{
}
MsgTrainDelete::~MsgTrainDelete()
{
	
}
HRCODE MsgTrainDelete::doSerialize(Archive& ar)
{
	if (HRCODE(HRCODE::HC_SUCCESS) != MsgTrain::doSerialize(ar)
	 || HRCODE(HRCODE::HC_SUCCESS) != stamp.serialize(ar))
		return HRCODE(HRCODE::HC_FAIL);
	
	return HRCODE(HRCODE::HC_SUCCESS);
}
MsgTrainDelete::MsgTrainDelete(const MsgTrainUpdate & msg)
{
	stamp = msg.stamp;

	*(MsgTrain*)this = (const MsgTrain&) msg;
}
size_t MsgTrainDelete::countObjectLength()
{
	return MsgTrain::countObjectLength()+stamp.getObjectLength();
}


///////////////
IMPLEMENT_CLBTH(MsgTrainUpdate, MSGID_MsgTrainUpdate)
IMPLEMENT_SERIALIZE_OPERATOR(MsgTrainUpdate)
MsgTrainUpdate::MsgTrainUpdate()
{
}
MsgTrainUpdate::MsgTrainUpdate(const MsgTrainAdd& msg)
{
	stamp = msg.stamp;

	*(MsgTrain*)this = (const MsgTrain&) msg;
}

MsgTrainUpdate::~MsgTrainUpdate()
{
	
}
HRCODE MsgTrainUpdate::doSerialize(Archive& ar)
{
	if (HRCODE(HRCODE::HC_SUCCESS) != MsgTrain::doSerialize(ar)
	 || HRCODE(HRCODE::HC_SUCCESS) != stamp.serialize(ar))
		return HRCODE(HRCODE::HC_FAIL);
	
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgTrainUpdate::countObjectLength()
{
	return MsgTrain::countObjectLength()+stamp.getObjectLength();
}

///////////////
IMPLEMENT_CLBTH(MsgNeibTrainUpdate, MSGID_MsgNeibTrainUpdate)
IMPLEMENT_SERIALIZE_OPERATOR(MsgNeibTrainUpdate)
MsgNeibTrainUpdate::MsgNeibTrainUpdate()
{
    send_entity = 0;
	recv_entity = 0;
	
	send_con_id = 0;
	recv_con_id = 0;
	begin_time = 0;
	end_time = 0;
	send_time = 0;	
	memset(sender_name, 0x00, sizeof(USERNAME)) ;
}
MsgNeibTrainUpdate::~MsgNeibTrainUpdate()
{
	
}
HRCODE MsgNeibTrainUpdate::doSerialize(Archive& ar)
{
	if (HRCODE(HRCODE::HC_SUCCESS) != MsgTrain::doSerialize(ar))
		return HRCODE(HRCODE::HC_FAIL);
	
	short len;
	if(ar.isStoring())
	{
		ar << send_entity;
		ar<< recv_entity;
		ar<< send_con_id;
		ar<< recv_con_id;
		ar<< begin_time;
		ar<< end_time;
		AR_WRITE_STRING(sender_name, len);
	}
	else
	{
		ar >> send_entity;
		ar >> recv_entity;
		ar >> send_con_id;
		ar >> recv_con_id;
		ar >> begin_time;
		ar >> end_time;
		AR_READ_STRING(sender_name,len);
	}

	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgNeibTrainUpdate::countObjectLength()
{
	size_t t=0;
	t=MsgTrain::countObjectLength()+sizeof(send_entity)
		+sizeof(recv_entity)+sizeof(send_con_id)+sizeof(recv_con_id)
		+sizeof(begin_time)+sizeof(end_time)+strlen(sender_name);
	return t;
}

///////////////
IMPLEMENT_CLBTH(MsgTrainData, MSGID_MsgTrainData)
IMPLEMENT_SERIALIZE_OPERATOR(MsgTrainData)
MsgTrainData::MsgTrainData()
{
}
MsgTrainData::~MsgTrainData()
{
	
}
HRCODE MsgTrainData::doSerialize(Archive& ar)
{
	if (HRCODE(HRCODE::HC_SUCCESS) != MsgTrain::doSerialize(ar)
	 || HRCODE(HRCODE::HC_SUCCESS) != stamp.serialize(ar))
		return HRCODE(HRCODE::HC_FAIL);

	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgTrainData::countObjectLength()
{
	return MsgTrain::countObjectLength()+stamp.getObjectLength();
}

///////////////

IMPLEMENT_CLBTH(MsgTrainUpdateInfo, MSGID_MsgTrainUpdateInfo)
IMPLEMENT_SERIALIZE_OPERATOR(MsgTrainUpdateInfo)
MsgTrainUpdateInfo::MsgTrainUpdateInfo()
{
}
MsgTrainUpdateInfo::~MsgTrainUpdateInfo()
{
	
}
HRCODE MsgTrainUpdateInfo::doSerialize(Archive& ar)
{
	if (HRCODE(HRCODE::HC_SUCCESS) != MsgTrainInfo::doSerialize(ar)
	 || HRCODE(HRCODE::HC_SUCCESS) != stamp.serialize(ar))
		return HRCODE(HRCODE::HC_FAIL);

	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgTrainUpdateInfo::countObjectLength()
{
	return MsgTrainInfo::countObjectLength()+stamp.getObjectLength();
}

///////////////
IMPLEMENT_CLBTH(MsgTrainUpdateEngine, MSGID_MsgTrainUpdateEngine)
IMPLEMENT_SERIALIZE_OPERATOR(MsgTrainUpdateEngine)
MsgTrainUpdateEngine::MsgTrainUpdateEngine()
{
	engine_ptr = NULL;
}
MsgTrainUpdateEngine::~MsgTrainUpdateEngine()
{
	delete[] engine_ptr;
}
HRCODE MsgTrainUpdateEngine::doSerialize(Archive& ar)
{
	if (HRCODE(HRCODE::HC_SUCCESS) != stamp.serialize(ar))
		return HRCODE(HRCODE::HC_FAIL);

	if(ar.isStoring())
	{
		ar << engine_count;
	}
	else
	{
		ar >> engine_count;

		if (engine_count>0)
		{
			engine_ptr = new MsgTrainEngineInfo[engine_count];
			if (NULL == engine_ptr)
				return HRCODE(HRCODE::HC_FAIL);
		}
	}

	for (int i=0; i<engine_count; i++)
		if (HRCODE(HRCODE::HC_SUCCESS) != engine_ptr[i].serialize(ar) )
			return HRCODE(HRCODE::HC_FAIL);

	return HRCODE(HRCODE::HC_SUCCESS);
}

size_t MsgTrainUpdateEngine::countObjectLength()
{
	size_t t = stamp.getObjectLength()+sizeof(engine_count);
	for (int i=0; i<engine_count; i++)
		t+=engine_ptr[i].getObjectLength();

	return t;
}
HRCODE MsgTrainUpdateEngine::SetEngine(short count, const MsgTrainEngineInfo* engine)
{
	engine_count = 0;
	if (count <= 0)
		return HRCODE(HRCODE::HC_SUCCESS);

	engine_ptr = new MsgTrainEngineInfo[count];
	if (NULL == engine_ptr)
		return HRCODE(HRCODE::HC_FAIL);
	engine_count = count;
	for (int i=0; i<engine_count; i++)
	{
		strncpy(engine_ptr[i].engine_id, engine[i].engine_id, sizeof(engine_ptr[i].engine_id)-1);
		engine_ptr[i].engine_id[sizeof(engine_ptr[i].engine_id)-1] = 0;
		
		strncpy(engine_ptr[i].engine_type, engine[i].engine_type, sizeof(engine_ptr[i].engine_type)-1);
		engine_ptr[i].engine_type[sizeof(engine_ptr[i].engine_type)-1] = 0;

		engine_ptr[i].engine_status = engine[i].engine_status; 
		engine_ptr[i].attach_sta    = engine[i].attach_sta;
		engine_ptr[i].unattach_sta  = engine[i].unattach_sta;
	}

	return HRCODE(HRCODE::HC_SUCCESS);
}

///////////////
IMPLEMENT_CLBTH(MsgTrainUpdateRecord, MSGID_MsgTrainUpdateRecord)
IMPLEMENT_SERIALIZE_OPERATOR(MsgTrainUpdateRecord)
MsgTrainUpdateRecord::MsgTrainUpdateRecord()
{
	rec_ptr   = NULL;
	rec_index = NULL;
}
MsgTrainUpdateRecord::~MsgTrainUpdateRecord()
{
	delete[] rec_ptr;
	delete[] rec_index;
}
HRCODE MsgTrainUpdateRecord::doSerialize(Archive& ar)
{
	if (HRCODE(HRCODE::HC_SUCCESS) != stamp.serialize(ar))
		return HRCODE(HRCODE::HC_FAIL);

	if(ar.isStoring())
	{
		ar << rec_cnt;
	}
	else
	{
		ar >> rec_cnt;
		if(rec_cnt >0)
		{
			rec_ptr   = new MsgTrainRecord[rec_cnt];
			rec_index = new short[rec_cnt];
			if (NULL == rec_ptr || NULL == rec_index)
				return HRCODE(HRCODE::HC_FAIL);
		}
	}

	for (int i=0; i<rec_cnt; i++)
	{
		if (HRCODE(HRCODE::HC_SUCCESS) != rec_ptr[i].serialize(ar))
			return HRCODE(HRCODE::HC_FAIL);

		if(ar.isStoring())
			ar << rec_index[i];
		else
			ar >> rec_index[i];
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgTrainUpdateRecord::countObjectLength()
{
	size_t t=stamp.getObjectLength()+sizeof(rec_cnt);
	for (int i=0; i<rec_cnt; i++)
	{
		t += rec_ptr[i].getObjectLength();
		t += sizeof(rec_index[i]);
	}

	return t;
}

///////////////
IMPLEMENT_CLBTH(MsgSchdBlock, MSGID_MsgSchdBlock)
IMPLEMENT_SERIALIZE_OPERATOR(MsgSchdBlock)
MsgSchdBlock::MsgSchdBlock(int version) : m_Version(version)
{
	CLBTH_ZERO_INIT_OBJECT(BaseMsg);
	comment_len = 0;
	comment = NULL;
}
MsgSchdBlock::~MsgSchdBlock()
{
	delete[] comment;
}
void MsgSchdBlock::Set(const MsgSchdBlock & msg)
{
	gpc_index     = msg.gpc_index    ;
	basic_index   = msg.basic_index  ;
	shift_index   = msg.shift_index  ;
	memcpy(shift_string, msg.shift_string, sizeof(shift_string));
	update_time   = msg.update_time  ;
	delete_flag   = msg.delete_flag  ;
	start_time    = msg.start_time   ;
	end_time      = msg.end_time     ;
	type          = msg.type         ;
	flag          = msg.flag         ;
	dir           = msg.dir          ;
	station       = msg.station      ;
	index         = msg.index        ;
	position      = msg.position     ;
	ele_count     = msg.ele_count    ;
	memcpy(ele_number, msg.ele_number, sizeof(ele_number));
	text_len      = msg.text_len     ;
	memcpy(text, msg.text, sizeof(text));
	s_meter       = msg.s_meter      ;
	e_meter       = msg.e_meter      ;
	e_kilo        = msg.e_kilo       ;
	slow_time     = msg.slow_time    ;
	adjust_status = msg.adjust_status;
	comment_len   = msg.comment_len  ;
	if (comment_len>0)
	{
		comment = new char[comment_len];
		memcpy(comment, msg.comment, comment_len);
	}
}
HRCODE MsgSchdBlock::doSerialize(Archive& ar)
{
	char len ;
	int i;
	if(ar.isStoring())
	{
		ar << m_Version;
		ar << gpc_index << basic_index << shift_index;
		AR_WRITE_STRING(shift_string, len);
		ar << update_time << delete_flag;

		ar << start_time << end_time
			<< type << flag << dir << station << index << position;
		
		AR_WRITE_COUNT(ele_count, ele_number);
		for (i=0; i<ele_count; i++)
			ar << ele_number[i];
		
		AR_WRITE_COUNT(text_len, text);
		ar.write(text, text_len);

		ar << s_meter << e_meter << e_kilo << slow_time << adjust_status;

		ar << comment_len;
		ar.write(comment, comment_len);
	}
	else
	{
		ar >> m_Version;
		ar >> gpc_index >> basic_index >> shift_index;
		AR_READ_STRING(shift_string, len);
		ar >> update_time >> delete_flag;

		ar >> start_time >> end_time 
			>> type >> flag >> dir >> station >> index >> position;

		AR_READ_COUNT(ele_count, ele_number);
		for (i=0; i<ele_count; i++)
			ar >> ele_number[i];

		AR_READ_COUNT(text_len, text);
		ar.read(text, text_len);

		ar >> s_meter >> e_meter >> e_kilo >> slow_time >> adjust_status;

		if (comment_len || comment)
			return HRCODE(HRCODE::HC_FAIL);

		if (comment_len+1 <= 0)
			return HRCODE(HRCODE::HC_FAIL);

		ar >> comment_len;
		comment = new char[comment_len+1];
		ar.read(comment, comment_len);
		comment[comment_len] = 0;
	}

	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgSchdBlock::countObjectLength()
{
	return sizeof(m_Version)
		+sizeof(gpc_index)+sizeof(basic_index)+sizeof(shift_index)
		+strlen(shift_string)+sizeof(char)
		+sizeof(update_time)+sizeof(delete_flag)
		+sizeof(start_time)+sizeof(end_time)
		+sizeof(type)+sizeof(flag)+sizeof(dir)+sizeof(station)+sizeof(index)+sizeof(position)
		+sizeof(ele_count)+ele_count*sizeof(IWORD)
		+sizeof(text_len)+text_len*sizeof(char)
		+sizeof(s_meter)+sizeof(e_meter)+sizeof(e_kilo)+sizeof(slow_time)+sizeof(adjust_status)
		+sizeof(comment_len)+comment_len;
}
HRCODE MsgSchdBlock::SetComment(const char* c)
{
	if (NULL != comment)
	{
		delete[] comment;
		comment = NULL;
	}
	
	if (NULL == c)	return HRCODE(HRCODE::HC_SUCCESS);
	comment_len = strlen(c)+1;
	comment = new char[comment_len];
	if (NULL == comment)
		return HRCODE(HRCODE::HC_FAIL);

	memcpy(comment, c, comment_len-1);
	comment[comment_len-1] = 0;

	return HRCODE(HRCODE::HC_SUCCESS);
}

///////////////
IMPLEMENT_CLBTH(MsgSchdBlockAdd, MSGID_MsgSchdBlockAdd)
IMPLEMENT_SERIALIZE_OPERATOR(MsgSchdBlockAdd)
MsgSchdBlockAdd::MsgSchdBlockAdd()
{
}
MsgSchdBlockAdd::~MsgSchdBlockAdd()
{
}
MsgSchdBlockAdd::MsgSchdBlockAdd(const MsgSchdBlockUpdate & msg)
{
	stamp = msg.stamp;
	Set(msg);
}
HRCODE MsgSchdBlockAdd::doSerialize(Archive& ar)
{
	if (HRCODE(HRCODE::HC_SUCCESS) != MsgSchdBlock::doSerialize(ar) ||
		HRCODE(HRCODE::HC_SUCCESS) != stamp.serialize(ar))
		return HRCODE(HRCODE::HC_FAIL);

	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgSchdBlockAdd::countObjectLength()
{
	return MsgSchdBlock::countObjectLength()+stamp.getObjectLength();
}

///////////////
IMPLEMENT_CLBTH(MsgSchdBlockUpdate, MSGID_MsgSchdBlockUpdate)
IMPLEMENT_SERIALIZE_OPERATOR(MsgSchdBlockUpdate)
MsgSchdBlockUpdate::MsgSchdBlockUpdate()
{
}
MsgSchdBlockUpdate::~MsgSchdBlockUpdate()
{
}
MsgSchdBlockUpdate::MsgSchdBlockUpdate(const MsgSchdBlockAdd & msg)
{
	stamp = msg.stamp;
	Set(msg);
}
HRCODE MsgSchdBlockUpdate::doSerialize(Archive& ar)
{
	if (HRCODE(HRCODE::HC_SUCCESS) != MsgSchdBlock::doSerialize(ar) ||
		HRCODE(HRCODE::HC_SUCCESS) != stamp.serialize(ar))
		return HRCODE(HRCODE::HC_FAIL);

	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgSchdBlockUpdate::countObjectLength()
{
	return MsgSchdBlock::countObjectLength()+stamp.getObjectLength();
}

///////////////
IMPLEMENT_CLBTH(MsgSchdBlockDelete, MSGID_MsgSchdBlockDelete)
IMPLEMENT_SERIALIZE_OPERATOR(MsgSchdBlockDelete)
MsgSchdBlockDelete::MsgSchdBlockDelete()
{
}
MsgSchdBlockDelete::~MsgSchdBlockDelete()
{
	
}
HRCODE MsgSchdBlockDelete::doSerialize(Archive& ar)
{
	if (HRCODE(HRCODE::HC_SUCCESS) != MsgSchdBlock::doSerialize(ar) ||
		HRCODE(HRCODE::HC_SUCCESS) != stamp.serialize(ar))
		return HRCODE(HRCODE::HC_FAIL);

	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgSchdBlockDelete::countObjectLength()
{
	return MsgSchdBlock::countObjectLength()+stamp.getObjectLength();
}


///////////////

IMPLEMENT_CLBTH(MsgSchdBlockData, MSGID_MsgSchdBlockData)
IMPLEMENT_SERIALIZE_OPERATOR(MsgSchdBlockData)
MsgSchdBlockData::MsgSchdBlockData()
{
}
MsgSchdBlockData::~MsgSchdBlockData()
{
	
}
HRCODE MsgSchdBlockData::doSerialize(Archive& ar)
{
	if (HRCODE(HRCODE::HC_SUCCESS) != MsgSchdBlock::doSerialize(ar) ||
		HRCODE(HRCODE::HC_SUCCESS) != stamp.serialize(ar))
		return HRCODE(HRCODE::HC_FAIL);

	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgSchdBlockData::countObjectLength()
{
	return MsgSchdBlock::countObjectLength()+stamp.getObjectLength();
}


IMPLEMENT_CLBTH(MsgSchdMsgEcho, MSGID_MsgSchdMsgEcho)
IMPLEMENT_SERIALIZE_OPERATOR(MsgSchdMsgEcho)
MsgSchdMsgEcho::MsgSchdMsgEcho(int version) : m_Version(version)
{
	m_Version = msgid = ecode = 0;
}
MsgSchdMsgEcho::~MsgSchdMsgEcho()
{
	
}
HRCODE MsgSchdMsgEcho::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << m_Version << msgid << ecode;
	}
	else
	{
		ar >> m_Version >> msgid >> ecode;
	}

	if (HRCODE(HRCODE::HC_SUCCESS) != stamp.serialize(ar))
		return HRCODE(HRCODE::HC_FAIL);

	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgSchdMsgEcho::countObjectLength()
{
	return sizeof(m_Version)+sizeof(msgid)+sizeof(ecode)+stamp.getObjectLength();
}

void MsgSchdMsgEcho::SetSchdStamp(const MsgSchdStamp& s)
{
	stamp.con_id    = s.con_id;
	stamp.schd_type = s.schd_type;
	stamp.index     = s.index;
	stamp.ltime     = s.ltime;
	stamp.operatr   = s.operatr;
}


IMPLEMENT_CLBTH(MsgConvertBasicSchd, MSGID_MsgConvertBasicSchd)
IMPLEMENT_SERIALIZE_OPERATOR(MsgConvertBasicSchd)
MsgConvertBasicSchd::MsgConvertBasicSchd(int version) : m_Version(version)
{
}
MsgConvertBasicSchd::~MsgConvertBasicSchd()
{
	
}
HRCODE MsgConvertBasicSchd::doSerialize(Archive& ar)
{
	short t;
	if(ar.isStoring())
	{
		ar << m_Version << conid << (short)flag;
	}
	else
	{
		ar >> m_Version >> conid >> t;
		flag = (MsgConvertBasicSchd::FLAG)t;
	}

	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgConvertBasicSchd::countObjectLength()
{
	return sizeof(m_Version)+sizeof(conid)+sizeof(short);
}


IMPLEMENT_CLBTH(MsgSchdLoad, MSGID_MsgSchdLoad)
IMPLEMENT_SERIALIZE_OPERATOR(MsgSchdLoad)
MsgSchdLoad::MsgSchdLoad(int version) : m_Version(version)
{
}
MsgSchdLoad::~MsgSchdLoad()
{
	
}
HRCODE MsgSchdLoad::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << con_id << schd_type << start_time << stop_time;

		AR_WRITE_COUNT(sta_count, station);
		for (int i=0; i<sta_count; i++)	
			ar << station[i];

	}
	else
	{
		ar >> con_id >> schd_type >> start_time >> stop_time;

		AR_READ_COUNT(sta_count, station);
		for (int i=0; i<sta_count; i++)	
			ar >> station[i];
	}

	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgSchdLoad::countObjectLength()
{
	return sizeof(con_id)+sizeof(schd_type)+sizeof(start_time)+sizeof(start_time)+sizeof(sta_count)+sta_count*sizeof(station[0]);
}

IMPLEMENT_CLBTH(MsgNoticeNeigbourReceiveSchd, MSGID_MsgNoticeNeigbourReceiveSchd)
IMPLEMENT_SERIALIZE_OPERATOR(MsgNoticeNeigbourReceiveSchd)
MsgNoticeNeigbourReceiveSchd::MsgNoticeNeigbourReceiveSchd(int version) : m_Version(version)
{
	con_id=0;
}
MsgNoticeNeigbourReceiveSchd::~MsgNoticeNeigbourReceiveSchd()
{
	
}
HRCODE MsgNoticeNeigbourReceiveSchd::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << send_id << receive_id << nShiftId << schd_type << hour << con_id;
	}
	else
	{
		ar >> send_id >> receive_id >> nShiftId >> schd_type >> hour >> con_id;
	}

	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgNoticeNeigbourReceiveSchd::countObjectLength()
{
	return sizeof(send_id)+sizeof(receive_id)+sizeof(nShiftId)+sizeof(schd_type)+sizeof(hour)+sizeof(con_id);
}
///////////////
IMPLEMENT_CLBTH(MsgTrainRecordData, MSGID_MsgTrainRecordData)
IMPLEMENT_SERIALIZE_OPERATOR(MsgTrainRecordData)
MsgTrainRecordData::MsgTrainRecordData()
{
}
MsgTrainRecordData::~MsgTrainRecordData()
{
}
HRCODE MsgTrainRecordData::doSerialize(Archive& ar)
{
	if (HRCODE(HRCODE::HC_SUCCESS) != MsgTrainUpdateRecord::doSerialize(ar)
	 || HRCODE(HRCODE::HC_SUCCESS) != info.serialize(ar))
		return HRCODE(HRCODE::HC_FAIL);

	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgTrainRecordData::countObjectLength()
{
	return MsgTrainUpdateRecord::countObjectLength()+info.getObjectLength();
}

///////////////
IMPLEMENT_CLBTH(MsgCheckDupilicateTrainID, MSGID_CheckDupilicateTrainID)
IMPLEMENT_SERIALIZE_OPERATOR(MsgCheckDupilicateTrainID)
MsgCheckDupilicateTrainID::MsgCheckDupilicateTrainID()
{
	CLBTH_ZERO_INIT_OBJECT(BaseMsg);
}
MsgCheckDupilicateTrainID::~MsgCheckDupilicateTrainID()
{
}
HRCODE MsgCheckDupilicateTrainID::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{	
		ar.write(szTrainNo, sizeof(szTrainNo));
		ar << nCon_Id << tBeginTm << tEndTm << nStatCount;
		if ( nStatCount > MAX_SEARCH_TRAIN_STA )
			return HRCODE(HRCODE::HC_FAIL);
		for (int i=0; i<nStatCount; i++)
			ar << nStno[i];
	}
	else
	{
		ar.read(szTrainNo, sizeof(szTrainNo));
		ar >> nCon_Id >> tBeginTm >> tEndTm >> nStatCount;
		if ( nStatCount > MAX_SEARCH_TRAIN_STA )
			return HRCODE(HRCODE::HC_FAIL);
		for (int i=0; i<nStatCount; i++)
			ar >> nStno[i];
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgCheckDupilicateTrainID::countObjectLength()
{
	return sizeof(szTrainNo)+sizeof(nCon_Id)+sizeof(tBeginTm)+sizeof(tEndTm)+sizeof(nStatCount)+sizeof(nStno[0])*nStatCount;
}
class MsgCheckDupilicateTrainID& MsgCheckDupilicateTrainID::operator=(class MsgCheckDupilicateTrainID const & msg)
{
	memcpy(szTrainNo, msg.szTrainNo, sizeof(szTrainNo));
	nCon_Id = msg.nCon_Id;
	tBeginTm = msg.tBeginTm;
	tEndTm = msg.tEndTm;
	nStatCount = msg.nStatCount;
	for (int i=0; i<nStatCount; i++)
		nStno[i] = msg.nStno[i];	
	return *this;
}
///////////////
IMPLEMENT_CLBTH(MsgFindUniqeTrainID, MSGID_FindUniqeTrainID)
IMPLEMENT_SERIALIZE_OPERATOR(MsgFindUniqeTrainID)
MsgFindUniqeTrainID::MsgFindUniqeTrainID()
{
	CLBTH_ZERO_INIT_OBJECT(BaseMsg);
}
MsgFindUniqeTrainID::~MsgFindUniqeTrainID()
{
}
HRCODE MsgFindUniqeTrainID::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{	
		ar.write(szPrefix, sizeof(szPrefix));
		ar << nCon_Id << nTrainIDFrom << nTrainIDTo << tBeginTm << tEndTm << nStatCount;
		if ( nStatCount > MAX_SEARCH_TRAIN_STA )
			return HRCODE(HRCODE::HC_FAIL);
		for (int i=0; i<nStatCount; i++)
			ar << nStno[i];
	}
	else
	{
		ar.read(szPrefix, sizeof(szPrefix));
		ar >> nCon_Id >> nTrainIDFrom >> nTrainIDTo >> tBeginTm >> tEndTm >> nStatCount;
		if ( nStatCount > MAX_SEARCH_TRAIN_STA )
			return HRCODE(HRCODE::HC_FAIL);
		for (int i=0; i<nStatCount; i++)
			ar >> nStno[i];
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgFindUniqeTrainID::countObjectLength()
{
	return sizeof(szPrefix)+sizeof(nCon_Id)+sizeof(nTrainIDFrom)+sizeof(nTrainIDTo)+sizeof(tBeginTm)+sizeof(tEndTm)+sizeof(nStatCount)+sizeof(nStno[0])*nStatCount;
}

class MsgFindUniqeTrainID& MsgFindUniqeTrainID::operator=(class MsgFindUniqeTrainID const & msg)
{
	memcpy(szPrefix, msg.szPrefix, sizeof(szPrefix));
	nCon_Id = msg.nCon_Id;
	nTrainIDFrom = msg.nTrainIDFrom;
	nTrainIDTo = msg.nTrainIDTo;
	tBeginTm = msg.tBeginTm;
	tEndTm = msg.tEndTm;
	nStatCount = msg.nStatCount;
	for (int i=0; i<nStatCount; i++)
		nStno[i] = msg.nStno[i];	
	return *this;
}
///////////////
IMPLEMENT_CLBTH(MsgCheckDupilicateTrainIDResult, MSGID_CheckDupilicateTrainIDResult)
IMPLEMENT_SERIALIZE_OPERATOR(MsgCheckDupilicateTrainIDResult)
MsgCheckDupilicateTrainIDResult::MsgCheckDupilicateTrainIDResult()
{
}
MsgCheckDupilicateTrainIDResult::~MsgCheckDupilicateTrainIDResult()
{
}
HRCODE MsgCheckDupilicateTrainIDResult::doSerialize(Archive& ar)
{
	if (HRCODE(HRCODE::HC_SUCCESS) != MsgCheckDupilicateTrainID::doSerialize(ar))
		return HRCODE(HRCODE::HC_FAIL);
	if(ar.isStoring())
	{	
		ar << bResult << tResultTime << nResultConID;
	}
	else
	{
		ar >> bResult >> tResultTime >> nResultConID;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgCheckDupilicateTrainIDResult::countObjectLength()
{
	return MsgCheckDupilicateTrainID::countObjectLength()+sizeof(bResult)+sizeof(tResultTime)+sizeof(nResultConID);
}

MsgCheckDupilicateTrainIDResult::MsgCheckDupilicateTrainIDResult(const MsgCheckDupilicateTrainID & msg)
{
	(MsgCheckDupilicateTrainID&)*this = msg;
	this->bResult = IFALSE;
}
///////////////
IMPLEMENT_CLBTH(MsgFindUniqeTrainIDResult, MSGID_FindUniqeTrainIDResult)
IMPLEMENT_SERIALIZE_OPERATOR(MsgFindUniqeTrainIDResult)
MsgFindUniqeTrainIDResult::MsgFindUniqeTrainIDResult()
{
}
MsgFindUniqeTrainIDResult::~MsgFindUniqeTrainIDResult()
{
}
HRCODE MsgFindUniqeTrainIDResult::doSerialize(Archive& ar)
{
	if (HRCODE(HRCODE::HC_SUCCESS) != MsgFindUniqeTrainID::doSerialize(ar))
		return HRCODE(HRCODE::HC_FAIL);
	if(ar.isStoring())
	{	
		ar << nResultTrainID;
	}
	else
	{
		ar >> nResultTrainID;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgFindUniqeTrainIDResult::countObjectLength()
{
	return MsgFindUniqeTrainID::countObjectLength()+sizeof(nResultTrainID);
}

MsgFindUniqeTrainIDResult::MsgFindUniqeTrainIDResult(const MsgFindUniqeTrainID & msg)
{
	(MsgFindUniqeTrainID&)*this = msg;
	this->nResultTrainID = 0;
}
///////////////
IMPLEMENT_CLBTH(MsgGetTrainIDList, MSGID_GetTrainIDList)
IMPLEMENT_SERIALIZE_OPERATOR(MsgGetTrainIDList)
MsgGetTrainIDList::MsgGetTrainIDList()
{
	CLBTH_ZERO_INIT_OBJECT(BaseMsg);
}
MsgGetTrainIDList::~MsgGetTrainIDList()
{
}
HRCODE MsgGetTrainIDList::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{	
		ar << nCon_Id << tBeginTm << tEndTm << nStatCount << nTrainIDBufferLen;
		if ( nStatCount > MAX_SEARCH_TRAIN_STA )
			return HRCODE(HRCODE::HC_FAIL);
		if ( nTrainIDBufferLen > MAX_TRAIN_ID_BUFFER_LEN_PER_PACK )
			return HRCODE(HRCODE::HC_FAIL);
		for (int i=0; i<nStatCount; i++)
			ar << nStno[i];
		ar.write(szTrainNoBuffer, nTrainIDBufferLen);
	}
	else
	{
		ar >> nCon_Id >> tBeginTm >> tEndTm >> nStatCount >> nTrainIDBufferLen;
		if ( nStatCount > MAX_SEARCH_TRAIN_STA )
			return HRCODE(HRCODE::HC_FAIL);
		if ( nTrainIDBufferLen > MAX_TRAIN_ID_BUFFER_LEN_PER_PACK )
			return HRCODE(HRCODE::HC_FAIL);
		for (int i=0; i<nStatCount; i++)
			ar >> nStno[i];
		ar.read(szTrainNoBuffer, nTrainIDBufferLen);
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgGetTrainIDList::countObjectLength()
{
	return sizeof(nCon_Id)+sizeof(tBeginTm)+sizeof(tEndTm)+sizeof(nStatCount)+sizeof(nTrainIDBufferLen)+sizeof(nStno[0])*nStatCount+nTrainIDBufferLen;
}
class MsgGetTrainIDList& MsgGetTrainIDList::operator=(class MsgGetTrainIDList const & msg)
{
	nCon_Id = msg.nCon_Id;
	tBeginTm = msg.tBeginTm;
	tEndTm = msg.tEndTm;
	nStatCount = msg.nStatCount;
	nTrainIDBufferLen = msg.nTrainIDBufferLen;
	for (int i=0; i<nStatCount; i++)
		nStno[i] = msg.nStno[i];	
	memcpy(szTrainNoBuffer, msg.szTrainNoBuffer, sizeof(nTrainIDBufferLen));	
	return *this;
}
MsgGetTrainIDList::MsgGetTrainIDList(const MsgGetTrainIDList & msg)
{
	(MsgGetTrainIDList&)*this = msg;
}
////////////////////

IMPLEMENT_CLBTH(MsgConvertBasicSchdNew, MSGID_MsgConvertBasicSchdNew)
IMPLEMENT_SERIALIZE_OPERATOR(MsgConvertBasicSchdNew)
MsgConvertBasicSchdNew::MsgConvertBasicSchdNew(int version) : m_Version(version)
{
}
MsgConvertBasicSchdNew::~MsgConvertBasicSchdNew()
{
	
}
HRCODE MsgConvertBasicSchdNew::doSerialize(Archive& ar)
{
	short t;
	if(ar.isStoring())
	{
		ar << m_Version << conid << from_schd_type << to_schd_type << (short)flag;
	}
	else
	{
		ar >> m_Version >> conid >> from_schd_type >> to_schd_type >> t;
		flag = (MsgConvertBasicSchdNew::FLAG)t;
	}

	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgConvertBasicSchdNew::countObjectLength()
{
	return sizeof(m_Version)+sizeof(conid)+sizeof(from_schd_type)+sizeof(to_schd_type)+sizeof(short);
}

MsgConvertBasicSchdNew::MsgConvertBasicSchdNew(const MsgConvertBasicSchdNew & msg)
{
	m_Version = msg.m_Version;
	conid = msg.conid;
	from_schd_type = msg.from_schd_type;
	to_schd_type = msg.to_schd_type;
	flag = msg.flag;
}


///////////////
IMPLEMENT_CLBTH(MsgQueryTrainByID, MSGID_QueryTrainByID)
IMPLEMENT_SERIALIZE_OPERATOR(MsgQueryTrainByID)
MsgQueryTrainByID::MsgQueryTrainByID()
{
	CLBTH_ZERO_INIT_OBJECT(BaseMsg);
}
MsgQueryTrainByID::~MsgQueryTrainByID()
{
}
HRCODE MsgQueryTrainByID::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{	
		ar.write(szTrainNo, sizeof(szTrainNo));
		ar  << tBeginTm << tEndTm;
	}
	else
	{
		ar.read(szTrainNo, sizeof(szTrainNo));
		ar >> tBeginTm >> tEndTm;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgQueryTrainByID::countObjectLength()
{
	return sizeof(szTrainNo)+sizeof(tBeginTm)+sizeof(tEndTm);
}
/*
///////////////
IMPLEMENT_CLBTH(MsgBasicScheduleInfoUpdate, MSGID_MsgBasicScheduleInfoUpdate)
IMPLEMENT_SERIALIZE_OPERATOR(MsgBasicScheduleInfoUpdate)
MsgBasicScheduleInfoUpdate::MsgBasicScheduleInfoUpdate(int version) : m_Version(version)
{
	CLBTH_ZERO_INIT_OBJECT(BaseMsg);
}
MsgBasicScheduleInfoUpdate::~MsgBasicScheduleInfoUpdate()
{
}
HRCODE MsgBasicScheduleInfoUpdate::doSerialize(Archive& ar)
{
	char len=0;
	if(ar.isStoring())
	{
		ar << m_Version << con_id << flag << set_time; 
		AR_WRITE_STRING(ver_info, len);
		AR_WRITE_STRING(set_name, len);
	}
	else
	{
		ar >> m_Version >> con_id >> flag >> set_time; 
		AR_READ_STRING(ver_info, len);
		AR_READ_STRING(set_name, len);
	}

	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgBasicScheduleInfoUpdate::countObjectLength()
{
	return sizeof(m_Version)+sizeof(con_id)+sizeof(flag)+sizeof(set_time)+sizeof(char)*2+strlen(ver_info)+strlen(set_name);
}

///////////////
IMPLEMENT_CLBTH(MsgBasicScheduleInfoInform, MSGID_MsgBasicScheduleInfoInform)
IMPLEMENT_SERIALIZE_OPERATOR(MsgBasicScheduleInfoInform)

MsgBasicScheduleInfoInform::MsgBasicScheduleInfoInform(int version) 
: m_Version(version),
  MsgBasicScheduleInfoUpdate(version)
{
	
}
MsgBasicScheduleInfoInform::~MsgBasicScheduleInfoInform()
{
}

///////////////
IMPLEMENT_CLBTH(MsgBasicScheduleInfoQuery, MSGID_MsgBasicScheduleInfoQuery)
IMPLEMENT_SERIALIZE_OPERATOR(MsgBasicScheduleInfoQuery)
MsgBasicScheduleInfoQuery::MsgBasicScheduleInfoQuery(int version) : m_Version(version)
{
	CLBTH_ZERO_INIT_OBJECT(BaseMsg);
}
MsgBasicScheduleInfoQuery::~MsgBasicScheduleInfoQuery()
{
}
HRCODE MsgBasicScheduleInfoQuery::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << m_Version << con_id << flag ;
	}
	else
	{
		ar >> m_Version >> con_id >> flag ;
	}

	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgBasicScheduleInfoQuery::countObjectLength()
{
	return sizeof(m_Version)+sizeof(con_id)+sizeof(flag);
}
*/

////////////////////

IMPLEMENT_CLBTH(MsgTrainKediaoInfo, MSGID_MsgTrainKediaoInfo)
IMPLEMENT_SERIALIZE_OPERATOR(MsgTrainKediaoInfo)
MsgTrainKediaoInfo::MsgTrainKediaoInfo()
{
	CLBTH_ZERO_INIT_OBJECT(BaseMsg);
}
MsgTrainKediaoInfo::~MsgTrainKediaoInfo()
{
	delete[] text;
}
HRCODE MsgTrainKediaoInfo::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << type << text_len;
		ar.write(text, text_len);
	}
	else
	{
		ar >> type >> text_len;

		if (text_len+1 <= 0)
			return HRCODE(HRCODE::HC_FAIL);

		text = new char[text_len+1];
		if (NULL == text)
			return HRCODE(HRCODE::HC_FAIL);

		ar.read(text, text_len);
		text[text_len]=0;
	}

	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgTrainKediaoInfo::countObjectLength()
{
	return sizeof(type)+sizeof(text_len)+text_len;
}
class MsgTrainKediaoInfo& MsgTrainKediaoInfo::operator=(class MsgTrainKediaoInfo const & msg)
{
	type = msg.type;
	text_len = msg.text_len;

	if (text_len>0)
	{
		text = new char[text_len+1];
		memcpy(text, msg.text, text_len);
		text[text_len] = 0;
	}
	else
		text = NULL;

	return *this;
}

HRCODE MsgTrainKediaoInfo::SetKediao(const char* c)
{
	if (NULL != text)
	{
		delete[] text;
		text = NULL;
	}

	if (NULL == c)	return HRCODE(HRCODE::HC_SUCCESS);
	text_len = strlen(c);
	text = new char[text_len+1];
	if (NULL == text)
		return HRCODE(HRCODE::HC_FAIL);

	memcpy(text, c, text_len);
	text[text_len] = 0;

	return HRCODE(HRCODE::HC_SUCCESS);
}

////////////////////

IMPLEMENT_CLBTH(MsgTrainYingyeInfo, MSGID_MsgTrainYingyeInfo)
IMPLEMENT_SERIALIZE_OPERATOR(MsgTrainYingyeInfo)
MsgTrainYingyeInfo::MsgTrainYingyeInfo()
{
	CLBTH_ZERO_INIT_OBJECT(BaseMsg);	
}
MsgTrainYingyeInfo::~MsgTrainYingyeInfo()
{

}
HRCODE MsgTrainYingyeInfo::doSerialize(Archive& ar)
{	
	char len;
	if(ar.isStoring())
	{
		ar << station_no << arrive_time << depart_time;
		AR_WRITE_STRING(station_name, len);
	}
	else
	{
		ar >> station_no >> arrive_time >> depart_time;
		AR_READ_STRING(station_name, len);
	}

	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgTrainYingyeInfo::countObjectLength()
{
	return sizeof(station_no)+sizeof(arrive_time)+sizeof(depart_time)+strlen(station_name)+sizeof(char);
}

class MsgTrainYingyeInfo& MsgTrainYingyeInfo::operator=(class MsgTrainYingyeInfo const & msg)
{
	station_no = msg.station_no;
	arrive_time = msg.arrive_time;
	depart_time = msg.depart_time;
	memcpy(station_name, msg.station_name, sizeof(station_name));
	return *this;
}

////////////////////

IMPLEMENT_CLBTH(MsgTrainCrewInfo, MSGID_MsgTrainCrewInfo)
IMPLEMENT_SERIALIZE_OPERATOR(MsgTrainCrewInfo)
MsgTrainCrewInfo::MsgTrainCrewInfo()
{
	CLBTH_ZERO_INIT_OBJECT(BaseMsg);	
}
MsgTrainCrewInfo::~MsgTrainCrewInfo()
{
}
HRCODE MsgTrainCrewInfo::doSerialize(Archive& ar)
{
	char len;
	if(ar.isStoring())
	{
		ar << crew_type;
		AR_WRITE_STRING(crew_info, len);
		AR_WRITE_STRING(crew_bureau, len);
	}
	else
	{
		ar >> crew_type;
		AR_READ_STRING(crew_info, len);
		AR_READ_STRING(crew_bureau, len);
	}

	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgTrainCrewInfo::countObjectLength()
{
	return sizeof(crew_type)+strlen(crew_info)+strlen(crew_bureau)+2*sizeof(char);
}

class MsgTrainCrewInfo& MsgTrainCrewInfo::operator=(class MsgTrainCrewInfo const & msg)
{
	crew_type = msg.crew_type;
	memcpy(crew_info, msg.crew_info, sizeof(crew_info));
	memcpy(crew_bureau, msg.crew_bureau, sizeof(crew_bureau));
	return *this;
}

////////////////////

IMPLEMENT_CLBTH(MsgEngineCrewInfo, MSGID_MsgEngineCrewInfo)
IMPLEMENT_SERIALIZE_OPERATOR(MsgEngineCrewInfo)
MsgEngineCrewInfo::MsgEngineCrewInfo()
{
	CLBTH_ZERO_INIT_OBJECT(BaseMsg);	
}
MsgEngineCrewInfo::~MsgEngineCrewInfo()
{
}
HRCODE MsgEngineCrewInfo::doSerialize(Archive& ar)
{
	char len;
	if(ar.isStoring())
	{
		ar << station_no << overwork_time;
		AR_WRITE_STRING(station_name, len);
	}
	else
	{
		ar >> station_no >> overwork_time;
		AR_READ_STRING(station_name, len);
	}

	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgEngineCrewInfo::countObjectLength()
{
	return sizeof(station_no)+sizeof(overwork_time)+strlen(station_name)+sizeof(char);
}

class MsgEngineCrewInfo& MsgEngineCrewInfo::operator=(class MsgEngineCrewInfo const & msg)
{
	station_no = msg.station_no;
	overwork_time = msg.overwork_time;
	memcpy(station_name, msg.station_name, sizeof(station_name));
	return *this;
}
////////////////////
IMPLEMENT_CLBTH(MsgTrainPassgenerTicketInfo, MSGID_MsgTrainPassgenerTicketInfo)
IMPLEMENT_SERIALIZE_OPERATOR(MsgTrainPassgenerTicketInfo)
MsgTrainPassgenerTicketInfo::MsgTrainPassgenerTicketInfo()
{
	CLBTH_ZERO_INIT_OBJECT(BaseMsg);	
}
MsgTrainPassgenerTicketInfo::~MsgTrainPassgenerTicketInfo()
{

}
HRCODE MsgTrainPassgenerTicketInfo::doSerialize(Archive& ar)
{
	char len;
	if(ar.isStoring())
	{
		ar << on_station_index << on_station << down_station_index << down_station << sale;
		AR_WRITE_STRING(seat, len);
		AR_WRITE_STRING(on_station_name, len);
		AR_WRITE_STRING(down_station_name, len);
	}
	else
	{
		ar >> on_station_index >> on_station >> down_station_index >> down_station >> sale;
		AR_READ_STRING(seat, len);
		AR_READ_STRING(on_station_name, len);
		AR_READ_STRING(down_station_name, len);
	}

	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgTrainPassgenerTicketInfo::countObjectLength()
{
	return sizeof(on_station_index)+sizeof(on_station)+sizeof(down_station_index)+sizeof(down_station)+sizeof(sale)
					+strlen(seat)+strlen(on_station_name)+strlen(down_station_name)+3*sizeof(char);
}

class MsgTrainPassgenerTicketInfo& MsgTrainPassgenerTicketInfo::operator=(class MsgTrainPassgenerTicketInfo const & msg)
{
	on_station_index = msg.on_station_index;
	on_station = msg.on_station;
	down_station_index = msg.down_station_index;
	down_station = msg.down_station;
	sale = msg.sale;
	memcpy(seat, msg.seat, sizeof(seat));
	memcpy(on_station_name, msg.on_station_name, sizeof(on_station_name));
	memcpy(down_station_name, msg.down_station_name, sizeof(down_station_name));

	return *this;
}

////////////////////

IMPLEMENT_CLBTH(MsgTrainOPMSInfo, MSGID_MsgTrainOPMSInfo)
IMPLEMENT_SERIALIZE_OPERATOR(MsgTrainOPMSInfo)
MsgTrainOPMSInfo::MsgTrainOPMSInfo()
{
	station_no=0;			// 车站站号
	work_flag=0;			// 车站作业
	oper_flag=0;			// 操作卡控
	important_flag=0;		// 重要卡控

	memset(oper_info, 0, sizeof(oper_info));
	memset(note_text, 0, sizeof(note_text));
}
MsgTrainOPMSInfo::~MsgTrainOPMSInfo()
{
}
HRCODE MsgTrainOPMSInfo::doSerialize(Archive& ar)
{
	short len;
	if(ar.isStoring())
	{
		ar << station_no << work_flag << oper_flag << important_flag;
		AR_WRITE_STRING(oper_info, len);
		AR_WRITE_STRING(note_text, len);
	}
	else
	{
		ar >> station_no >> work_flag >> oper_flag >> important_flag;
		AR_READ_STRING(oper_info, len);
		AR_READ_STRING(note_text, len);
	}

	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgTrainOPMSInfo::countObjectLength()
{
	return sizeof(station_no)+sizeof(work_flag)+sizeof(oper_flag)+sizeof(important_flag)
		+2*sizeof(short)+strlen(oper_info)+strlen(note_text);
}

class MsgTrainOPMSInfo& MsgTrainOPMSInfo::operator=(class MsgTrainOPMSInfo const & msg)
{
	station_no = msg.station_no;
	work_flag = msg.work_flag;
	oper_flag = msg.oper_flag;
	important_flag = msg.important_flag;
	strncpy(oper_info, msg.oper_info, sizeof(oper_info)-1);
	oper_info[sizeof(oper_info)-1]=0;
	strncpy(note_text, msg.note_text, sizeof(note_text)-1);
	note_text[sizeof(note_text)-1]=0;
	return *this;
}
////////////////////

IMPLEMENT_CLBTH(MsgTrainExpand, MSGID_MsgTrainExpand)
IMPLEMENT_SERIALIZE_OPERATOR(MsgTrainExpand)
MsgTrainExpand::MsgTrainExpand()
{
	m_Version = chezu_no[0] = 0;
	yingye_count = crew_count = ticket_count = engine_crew_count = opms_count = 0;
	prev_shift_index = next_shift_index = prev_train_index = prev_basic_index = NULL_TRAIN_INDEX;
	kd_ptr = NULL;
	yy_ptr = NULL;
	crew_ptr = NULL;
	ticket_ptr = NULL;
	prev_dest_sta=0;		//前一个方向站 
	next_dest_sta=0;		//后一个方向站 
	halt_state=0;
	engine_crew_ptr = NULL;
	opms_ptr=NULL;
}
MsgTrainExpand::~MsgTrainExpand()
{
	delete		kd_ptr;
	delete[]	yy_ptr;
	delete[]	crew_ptr;
	delete[]	ticket_ptr;
	delete[]	engine_crew_ptr;
	delete[]	opms_ptr;
}
HRCODE MsgTrainExpand::doSerialize(Archive& ar)
{
	if (HRCODE(HRCODE::HC_SUCCESS) != stamp.serialize(ar))
		return HRCODE(HRCODE::HC_FAIL);
	
	char len, kd_null;
	if(ar.isStoring())
	{
		if (NULL == kd_ptr)
		{
			kd_null = 1;
		}
		else
		{
			kd_null = 0;
		}
		ar << m_Version << prev_train_index << prev_basic_index << prev_shift_index << next_shift_index << prev_dest_sta  << next_dest_sta << kd_null << yingye_count << crew_count << ticket_count << engine_crew_count << opms_count << halt_state;
		AR_WRITE_STRING(chezu_no, len);
	}
	else
	{
		ar >> m_Version >> prev_train_index >> prev_basic_index >> prev_shift_index >> next_shift_index >> prev_dest_sta  >> next_dest_sta >> kd_null >> yingye_count >> crew_count >> ticket_count >> engine_crew_count >> opms_count >> halt_state;		
		AR_READ_STRING(chezu_no, len);

		if (0 == kd_null)
			kd_ptr = new MsgTrainKediaoInfo;
		if (0 == kd_null && NULL == kd_ptr)
			return HRCODE(HRCODE::HC_FAIL);
		
		if (yingye_count > 0)
			yy_ptr = new MsgTrainYingyeInfo[yingye_count];
		if (yingye_count > 0 && NULL == yy_ptr)
				return HRCODE(HRCODE::HC_FAIL);
		
		if (crew_count > 0)
			crew_ptr = new MsgTrainCrewInfo[crew_count];
		if (crew_count > 0 && NULL == crew_ptr)
				return HRCODE(HRCODE::HC_FAIL);
		
		if (ticket_count > 0)
			ticket_ptr = new MsgTrainPassgenerTicketInfo[ticket_count];
		if (ticket_count > 0 && NULL == ticket_ptr)
				return HRCODE(HRCODE::HC_FAIL);
		
		if (engine_crew_count > 0)
			engine_crew_ptr = new MsgEngineCrewInfo[engine_crew_count];
		if (engine_crew_count > 0 && NULL == engine_crew_ptr)
				return HRCODE(HRCODE::HC_FAIL);
		
		if (opms_count > 0)
			opms_ptr = new MsgTrainOPMSInfo[opms_count];
		if (opms_count > 0 && NULL == opms_ptr)
				return HRCODE(HRCODE::HC_FAIL);
	}

	if (NULL != kd_ptr && HRCODE(HRCODE::HC_SUCCESS) != kd_ptr->serialize(ar))
		return HRCODE(HRCODE::HC_FAIL);

	int i = 0;

	for (i=0; i<yingye_count; i++)
		if (HRCODE(HRCODE::HC_SUCCESS) != yy_ptr[i].serialize(ar) )
			return HRCODE(HRCODE::HC_FAIL);
	
	for (i=0; i<crew_count; i++)
		if (HRCODE(HRCODE::HC_SUCCESS) != crew_ptr[i].serialize(ar) )
			return HRCODE(HRCODE::HC_FAIL);
	
	for (i=0; i<ticket_count; i++)
		if (HRCODE(HRCODE::HC_SUCCESS) != ticket_ptr[i].serialize(ar) )
			return HRCODE(HRCODE::HC_FAIL);
	
	for (i=0; i<engine_crew_count; i++)
		if (HRCODE(HRCODE::HC_SUCCESS) != engine_crew_ptr[i].serialize(ar) )
			return HRCODE(HRCODE::HC_FAIL);
	
	for (i=0; i<opms_count; i++)
		if (HRCODE(HRCODE::HC_SUCCESS) != opms_ptr[i].serialize(ar) )
			return HRCODE(HRCODE::HC_FAIL);

	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgTrainExpand::countObjectLength()
{
	size_t t = stamp.getObjectLength() + sizeof(m_Version) + sizeof(prev_train_index) + sizeof(prev_basic_index) + sizeof(prev_shift_index) + sizeof(next_shift_index) + 
		sizeof(prev_dest_sta) + sizeof(next_dest_sta) + strlen(chezu_no) + sizeof(halt_state) + sizeof(char);
	
	t += sizeof(char);
	if (NULL != kd_ptr)
		t += kd_ptr->getObjectLength();

	int i = 0;

	t += sizeof(yingye_count);
	for (i=0; i<yingye_count; i++)
		t += yy_ptr[i].getObjectLength();

	t += sizeof(crew_count);
	for (i=0; i<crew_count; i++)
		t += crew_ptr[i].getObjectLength();

	t += sizeof(ticket_count);
	for (i=0; i<ticket_count; i++)
		t += ticket_ptr[i].getObjectLength();

	t += sizeof(engine_crew_count);
	for (i=0; i<engine_crew_count; i++)
		t += engine_crew_ptr[i].getObjectLength();
	
	t += sizeof(opms_count);
	for (i=0; i<opms_count; i++)
		t += opms_ptr[i].getObjectLength();
	return t;
}

MsgTrainExpand::MsgTrainExpand(const MsgTrainExpand & msg)
{
	m_Version = msg.m_Version;
	stamp = msg.stamp;
	memcpy(chezu_no, msg.chezu_no, sizeof(chezu_no));
	prev_basic_index = msg.prev_basic_index;
	prev_shift_index = msg.prev_shift_index;
	next_shift_index = msg.next_shift_index;
	prev_train_index = msg.prev_train_index;
	prev_dest_sta = msg.prev_dest_sta;
	next_dest_sta = msg.next_dest_sta;
	halt_state  = msg.halt_state;
	if (NULL == msg.kd_ptr)
	{
		kd_ptr = NULL;
	}
	else
	{
		kd_ptr = new MsgTrainKediaoInfo;
		*kd_ptr = *(msg.kd_ptr);
	}

	yingye_count = msg.yingye_count;
	if (yingye_count >0)
		yy_ptr = new MsgTrainYingyeInfo[yingye_count];
	int i = 0;
	for (i=0; i<yingye_count; i++)
		yy_ptr[i] = msg.yy_ptr[i];

	crew_count = msg.crew_count;
	if (crew_count >0)
		crew_ptr = new MsgTrainCrewInfo[crew_count];
	for (i=0; i<crew_count; i++)
		crew_ptr[i] = msg.crew_ptr[i];
	
	ticket_count = msg.ticket_count;
	if (ticket_count >0)
		ticket_ptr = new MsgTrainPassgenerTicketInfo[ticket_count];
	for (i=0; i<ticket_count; i++)
		ticket_ptr[i] = msg.ticket_ptr[i];

	engine_crew_count = msg.engine_crew_count;
	if (engine_crew_count >0)
		engine_crew_ptr = new MsgEngineCrewInfo[engine_crew_count];
	for (i=0; i<engine_crew_count; i++)
		engine_crew_ptr[i] = msg.engine_crew_ptr[i];

	opms_count = msg.opms_count;
	if (opms_count >0)
		opms_ptr = new MsgTrainOPMSInfo[opms_count];
	for (i=0; i<opms_count; i++)
		opms_ptr[i] = msg.opms_ptr[i];
}

/////////////////////
IMPLEMENT_CLBTH(MsgTrainCmdInfo, MSGID_MsgTrainCmdInfo)
IMPLEMENT_SERIALIZE_OPERATOR(MsgTrainCmdInfo)
MsgTrainCmdInfo::MsgTrainCmdInfo(int version):m_Version(version)
{
	station_no = rec_index = send_entity = create_time = sign_status = sign_time =0;
	memset(dcmd_id, 0, sizeof(dcmd_id));
}
MsgTrainCmdInfo::~MsgTrainCmdInfo()
{
}

HRCODE MsgTrainCmdInfo::doSerialize(Archive& ar)
{
	IDWORD msg_start_pos = ar.getCurrentPos();
	IDWORD whole_len = 0;
	char len;

	if(ar.isStoring())
	{
		ar << whole_len << m_Version;	//whole_len to be corrected later
	}
	else
	{
		ar >> whole_len >> m_Version;
	}
	
	//data of version 0
	if(ar.isStoring())
	{
		ar << station_no << rec_index << send_entity << create_time << sign_status << sign_time;
		AR_WRITE_STRING(dcmd_id, len);
	}
	else
	{
		ar >> station_no >> rec_index >> send_entity >> create_time >> sign_status >> sign_time;
		AR_READ_STRING(dcmd_id, len);
	}	
	
/*	
	//data of version 1
	if(ar.isStoring())
	{
		ar << tcc_cmd_count;
		for (int i=0; i<tcc_cmd_count; i++)
			if (HRCODE(HRCODE::HC_SUCCESS) != tcc_cmd_ptr[i].serialize(ar))
				return HRCODE(HRCODE::HC_FAIL);		
	}
	else if (m_Version >= 1)		
	{
		ar >> tcc_cmd_count;

		if (tcc_cmd_count > 0)
			tcc_cmd_ptr = new MsgTrainTccCmdInfo[tcc_cmd_count];
		if (tcc_cmd_count > 0 && NULL == tcc_cmd_ptr)
				return HRCODE(HRCODE::HC_FAIL);	
		for (int i=0; i<tcc_cmd_count; i++)
			if (HRCODE(HRCODE::HC_SUCCESS) != tcc_cmd_ptr[i].serialize(ar))
				return HRCODE(HRCODE::HC_FAIL);		
	}
	else
		return HRCODE(HRCODE::HC_SUCCESS);	
*/

	if (ar.isStoring())		
	{
		whole_len = ar.getCurrentPos() - msg_start_pos;
		ar.seek(Archive::BEGIN, msg_start_pos);
		ar << whole_len;
		ar.seek(Archive::BEGIN, msg_start_pos + whole_len);
	}
	else
	{
		IDWORD offset = ar.getCurrentPos() - msg_start_pos;
		if (offset > whole_len)
			return HRCODE(HRCODE::HC_FAIL);
		ar.seek(Archive::CURRENT, whole_len-offset);
		return HRCODE(HRCODE::HC_SUCCESS);
	}
	
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgTrainCmdInfo::countObjectLength()
{
	size_t t = sizeof(m_Version) + sizeof(IDWORD);
	//len of version 0 data
	t += sizeof(station_no)+sizeof(rec_index)+sizeof(send_entity)+sizeof(create_time)
		+sizeof(sign_status)+sizeof(sign_time);
	t += strlen(dcmd_id)+sizeof(char);
	
	//len of version 1 data

	return t;
}

class MsgTrainCmdInfo& MsgTrainCmdInfo::operator=(class MsgTrainCmdInfo const & msg)
{
	m_Version	= msg.m_Version		;
	station_no	= msg.station_no	;
	rec_index	= msg.rec_index		;
	send_entity	= msg.send_entity	;
	create_time	= msg.create_time	;
	sign_status	= msg.sign_status	;
	sign_time	= msg.sign_time		;
	
	memcpy(dcmd_id, msg.dcmd_id, sizeof(dcmd_id));
	return *this;
}
////////////////////

IMPLEMENT_CLBTH(MsgTrainExtent, MSGID_MsgTrainExtent)
IMPLEMENT_SERIALIZE_OPERATOR(MsgTrainExtent)
MsgTrainExtent::MsgTrainExtent(int version):m_Version(version)
{
	chezu_no[0] = 0;
	prev_shift_index = next_shift_index = prev_train_index = prev_basic_index = NULL_TRAIN_INDEX;
	prev_dest_sta=0;		//前一个方向站 
	next_dest_sta=0;		//后一个方向站 
	halt_state=0;
	yingye_count = crew_count = ticket_count = engine_crew_count = opms_count = cmd_count = 0;
	kd_ptr = NULL;
	yy_ptr = NULL;
	crew_ptr = NULL;
	ticket_ptr = NULL;
	engine_crew_ptr = NULL;
	opms_ptr=NULL;
	cmd_ptr = NULL;
}
MsgTrainExtent::~MsgTrainExtent()
{
	delete		kd_ptr;
	delete[]	yy_ptr;
	delete[]	crew_ptr;
	delete[]	ticket_ptr;
	delete[]	engine_crew_ptr;
	delete[]	opms_ptr;
	delete[]	cmd_ptr;
}

//serialize to be like [ver0_data]ver0_right_len(short)[ver1_data]ver1_right_len(short)......
//ver0_right_len, include ver1+ver2+ver3 .... ; ver1_right_len, include ver2+ver3 .... ; etc.
HRCODE MsgTrainExtent::doSerialize(Archive& ar)
{
	IDWORD msg_start_pos = ar.getCurrentPos();
	IDWORD whole_len = 0;
	if(ar.isStoring())
	{
		ar << whole_len << m_Version;
	}
	else
	{
		ar >> whole_len >> m_Version;
	}
	
	if (HRCODE(HRCODE::HC_SUCCESS) != stamp.serialize(ar))
		return HRCODE(HRCODE::HC_FAIL);
	
	
	//data of version 0
	char len, kd_null;
	if(ar.isStoring())
	{
		if (NULL == kd_ptr)
		{
			kd_null = 1;
		}
		else
		{
			kd_null = 0;
		}
		ar << prev_train_index << prev_basic_index << prev_shift_index << next_shift_index << prev_dest_sta  << next_dest_sta << kd_null << yingye_count << crew_count << ticket_count << engine_crew_count << opms_count << halt_state << cmd_count;;
		AR_WRITE_STRING(chezu_no, len);
	}
	else
	{
		ar >> prev_train_index >> prev_basic_index >> prev_shift_index >> next_shift_index >> prev_dest_sta  >> next_dest_sta >> kd_null >> yingye_count >> crew_count >> ticket_count >> engine_crew_count >> opms_count >> halt_state >> cmd_count;		
		AR_READ_STRING(chezu_no, len);

		if (0 == kd_null)
			kd_ptr = new MsgTrainKediaoInfo;
		if (0 == kd_null && NULL == kd_ptr)
			return HRCODE(HRCODE::HC_FAIL);
		
		if (yingye_count > 0)
			yy_ptr = new MsgTrainYingyeInfo[yingye_count];
		if (yingye_count > 0 && NULL == yy_ptr)
				return HRCODE(HRCODE::HC_FAIL);
		
		if (crew_count > 0)
			crew_ptr = new MsgTrainCrewInfo[crew_count];
		if (crew_count > 0 && NULL == crew_ptr)
				return HRCODE(HRCODE::HC_FAIL);
		
		if (ticket_count > 0)
			ticket_ptr = new MsgTrainPassgenerTicketInfo[ticket_count];
		if (ticket_count > 0 && NULL == ticket_ptr)
				return HRCODE(HRCODE::HC_FAIL);
		
		if (engine_crew_count > 0)
			engine_crew_ptr = new MsgEngineCrewInfo[engine_crew_count];
		if (engine_crew_count > 0 && NULL == engine_crew_ptr)
				return HRCODE(HRCODE::HC_FAIL);
		
		if (opms_count > 0)
			opms_ptr = new MsgTrainOPMSInfo[opms_count];
		if (opms_count > 0 && NULL == opms_ptr)
				return HRCODE(HRCODE::HC_FAIL);
		
		if (cmd_count > 0)
			cmd_ptr = new MsgTrainCmdInfo[cmd_count];
		if (cmd_count > 0 && NULL == cmd_ptr)
			return HRCODE(HRCODE::HC_FAIL);		
	}

	if (NULL != kd_ptr && HRCODE(HRCODE::HC_SUCCESS) != kd_ptr->serialize(ar))
		return HRCODE(HRCODE::HC_FAIL);

	int i = 0;

	for (i=0; i<yingye_count; i++)
		if (HRCODE(HRCODE::HC_SUCCESS) != yy_ptr[i].serialize(ar) )
			return HRCODE(HRCODE::HC_FAIL);
	
	for (i=0; i<crew_count; i++)
		if (HRCODE(HRCODE::HC_SUCCESS) != crew_ptr[i].serialize(ar) )
			return HRCODE(HRCODE::HC_FAIL);
	
	for (i=0; i<ticket_count; i++)
		if (HRCODE(HRCODE::HC_SUCCESS) != ticket_ptr[i].serialize(ar) )
			return HRCODE(HRCODE::HC_FAIL);
	
	for (i=0; i<engine_crew_count; i++)
		if (HRCODE(HRCODE::HC_SUCCESS) != engine_crew_ptr[i].serialize(ar) )
			return HRCODE(HRCODE::HC_FAIL);
	
	for (i=0; i<opms_count; i++)
	{
		if (HRCODE(HRCODE::HC_SUCCESS) != opms_ptr[i].serialize(ar) )
			return HRCODE(HRCODE::HC_FAIL);
	}

	for (int i=0; i<cmd_count; i++)
		if (HRCODE(HRCODE::HC_SUCCESS) != cmd_ptr[i].serialize(ar))
			return HRCODE(HRCODE::HC_FAIL);
	
/*	
	//data of version 1
	if(ar.isStoring())
	{
		ar << tcc_cmd_count;
		for (int i=0; i<tcc_cmd_count; i++)
			if (HRCODE(HRCODE::HC_SUCCESS) != tcc_cmd_ptr[i].serialize(ar))
				return HRCODE(HRCODE::HC_FAIL);		
	}
	else if (m_Version >= 1)		
	{
		ar >> tcc_cmd_count;

		if (tcc_cmd_count > 0)
			tcc_cmd_ptr = new MsgTrainTccCmdInfo[tcc_cmd_count];
		if (tcc_cmd_count > 0 && NULL == tcc_cmd_ptr)
				return HRCODE(HRCODE::HC_FAIL);	
		for (int i=0; i<tcc_cmd_count; i++)
			if (HRCODE(HRCODE::HC_SUCCESS) != tcc_cmd_ptr[i].serialize(ar))
				return HRCODE(HRCODE::HC_FAIL);		
	}
	else
		return HRCODE(HRCODE::HC_SUCCESS);	
*/
	
	if (ar.isStoring())		
	{
		whole_len = ar.getCurrentPos() - msg_start_pos;
		ar.seek(Archive::BEGIN, msg_start_pos);
		ar << whole_len;
		ar.seek(Archive::BEGIN, msg_start_pos + whole_len);
	}
	else	
	{
		IDWORD offset = ar.getCurrentPos() - msg_start_pos;
		if (offset > whole_len)
			return HRCODE(HRCODE::HC_FAIL);
		ar.seek(Archive::CURRENT, whole_len-offset);
		return HRCODE(HRCODE::HC_SUCCESS);
	}
	
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgTrainExtent::countObjectLength()
{
	size_t t = sizeof(IDWORD) + sizeof(m_Version) + stamp.getObjectLength();
		
	//len of version 0 data
	t += sizeof(prev_train_index) + sizeof(prev_basic_index) + sizeof(prev_shift_index) + sizeof(next_shift_index) + 
		sizeof(prev_dest_sta) + sizeof(next_dest_sta) + strlen(chezu_no) + sizeof(halt_state) + sizeof(char);
	
	t += sizeof(char);
	if (NULL != kd_ptr)
		t += kd_ptr->getObjectLength();

	int i = 0;

	t += sizeof(yingye_count);
	for (i=0; i<yingye_count; i++)
		t += yy_ptr[i].getObjectLength();

	t += sizeof(crew_count);
	for (i=0; i<crew_count; i++)
		t += crew_ptr[i].getObjectLength();

	t += sizeof(ticket_count);
	for (i=0; i<ticket_count; i++)
		t += ticket_ptr[i].getObjectLength();

	t += sizeof(engine_crew_count);
	for (i=0; i<engine_crew_count; i++)
		t += engine_crew_ptr[i].getObjectLength();
	
	t += sizeof(opms_count);
	for (i=0; i<opms_count; i++)
		t += opms_ptr[i].getObjectLength();			
		
	t += sizeof(cmd_count);
	
	for (int i=0; i<cmd_count; i++)
		t += cmd_ptr[i].getObjectLength();
	
	//len of version 1 data
	//t += 

	return t;
}

MsgTrainExtent::MsgTrainExtent(const MsgTrainExtent & msg)
{
	m_Version = msg.m_Version;
	stamp = msg.stamp;
	
	memcpy(chezu_no, msg.chezu_no, sizeof(chezu_no));
	prev_basic_index = msg.prev_basic_index;
	prev_shift_index = msg.prev_shift_index;
	next_shift_index = msg.next_shift_index;
	prev_train_index = msg.prev_train_index;
	prev_dest_sta = msg.prev_dest_sta;
	next_dest_sta = msg.next_dest_sta;
	halt_state  = msg.halt_state;
	if (NULL == msg.kd_ptr)
	{
		kd_ptr = NULL;
	}
	else
	{
		kd_ptr = new MsgTrainKediaoInfo;
		*kd_ptr = *(msg.kd_ptr);
	}

	yingye_count = msg.yingye_count;
	if (yingye_count >0)
		yy_ptr = new MsgTrainYingyeInfo[yingye_count];
	int i = 0;
	for (i=0; i<yingye_count; i++)
		yy_ptr[i] = msg.yy_ptr[i];

	crew_count = msg.crew_count;
	if (crew_count >0)
		crew_ptr = new MsgTrainCrewInfo[crew_count];
	for (i=0; i<crew_count; i++)
		crew_ptr[i] = msg.crew_ptr[i];
	
	ticket_count = msg.ticket_count;
	if (ticket_count >0)
		ticket_ptr = new MsgTrainPassgenerTicketInfo[ticket_count];
	for (i=0; i<ticket_count; i++)
		ticket_ptr[i] = msg.ticket_ptr[i];

	engine_crew_count = msg.engine_crew_count;
	if (engine_crew_count >0)
		engine_crew_ptr = new MsgEngineCrewInfo[engine_crew_count];
	for (i=0; i<engine_crew_count; i++)
		engine_crew_ptr[i] = msg.engine_crew_ptr[i];

	opms_count = msg.opms_count;
	if (opms_count >0)
		opms_ptr = new MsgTrainOPMSInfo[opms_count];
	for (i=0; i<opms_count; i++)
		opms_ptr[i] = msg.opms_ptr[i];

	cmd_count = msg.cmd_count;
	if (cmd_count >0)
		cmd_ptr = new MsgTrainCmdInfo[cmd_count];
	for (int i=0; i<cmd_count; i++)
		cmd_ptr[i] = msg.cmd_ptr[i];
}

class MsgTrainExtent& MsgTrainExtent::operator=(class MsgTrainExtent const & msg)
{
	m_Version = msg.m_Version;
	stamp = msg.stamp;
	
	memcpy(chezu_no, msg.chezu_no, sizeof(chezu_no));
	prev_basic_index = msg.prev_basic_index;
	prev_shift_index = msg.prev_shift_index;
	next_shift_index = msg.next_shift_index;
	prev_train_index = msg.prev_train_index;
	prev_dest_sta = msg.prev_dest_sta;
	next_dest_sta = msg.next_dest_sta;
	halt_state  = msg.halt_state;
	if (NULL == msg.kd_ptr)
	{
		kd_ptr = NULL;
	}
	else
	{
		kd_ptr = new MsgTrainKediaoInfo;
		*kd_ptr = *(msg.kd_ptr);
	}

	yingye_count = msg.yingye_count;
	if (yingye_count >0)
		yy_ptr = new MsgTrainYingyeInfo[yingye_count];
	int i = 0;
	for (i=0; i<yingye_count; i++)
		yy_ptr[i] = msg.yy_ptr[i];

	crew_count = msg.crew_count;
	if (crew_count >0)
		crew_ptr = new MsgTrainCrewInfo[crew_count];
	for (i=0; i<crew_count; i++)
		crew_ptr[i] = msg.crew_ptr[i];
	
	ticket_count = msg.ticket_count;
	if (ticket_count >0)
		ticket_ptr = new MsgTrainPassgenerTicketInfo[ticket_count];
	for (i=0; i<ticket_count; i++)
		ticket_ptr[i] = msg.ticket_ptr[i];

	engine_crew_count = msg.engine_crew_count;
	if (engine_crew_count >0)
		engine_crew_ptr = new MsgEngineCrewInfo[engine_crew_count];
	for (i=0; i<engine_crew_count; i++)
		engine_crew_ptr[i] = msg.engine_crew_ptr[i];

	opms_count = msg.opms_count;
	if (opms_count >0)
		opms_ptr = new MsgTrainOPMSInfo[opms_count];
	for (i=0; i<opms_count; i++)
		opms_ptr[i] = msg.opms_ptr[i];

	cmd_count = msg.cmd_count;
	if (cmd_count >0)
		cmd_ptr = new MsgTrainCmdInfo[cmd_count];
	for (int i=0; i<cmd_count; i++)
		cmd_ptr[i] = msg.cmd_ptr[i];

	return *this;
}

///////////// END /////////////
CLBTH_NAMESPACE_END

