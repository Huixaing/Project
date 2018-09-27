#include "tg_msgcopy_newctcmsg.h"
//#include "message.h"
//#define  STR_CPY(x,y)   strncpy(x,y,sizeof(x)-1);x[sizeof(x)-1]=0;

BOOL  TrainCopy(TG_TrainInfo& def, const MsgTrainInfo& msg)
{
	STR_CPY(def.train_name,  msg.train_name);
	def.train_key             = msg.train_key;
	def.basic_train_index    = msg.basic_index;   
	def.shift_train_index    = msg.shift_index;   
	def.gpc_train_index      = msg.gpc_index; 
	def.type                 = msg.type;
	def.run_type             = msg.run_type;

	def.start_actual_time    =msg.start_actual_time; 
	def.start_plan_time      =msg.start_plan_time;

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
	def.engine_utility       = msg.engine_utility;
	def.side_utility         = msg.side_utility;
	def.adjust_begin_sta     = msg.adjust_begin_sta;
	def.adjust_end_sta       = msg.adjust_end_sta;
	def.adjust_flag          = msg.adjust_flag;
	def.train_source         = msg.train_source;
	STR_CPY(def.shift_string,msg.shift_string); 
	def.update_time=msg.update_time;      // 删除时间
	def.delete_flag =msg.delete_flag;      //  -1, 无
	return TRUE;
}
BOOL TrainCopy(TG_TrainEngineInfo& def, const MsgTrainEngineInfo& msg)
{
	STR_CPY(def.engine_id, msg.engine_id);
	STR_CPY(def.engine_type, msg.engine_type);
	def.engine_status = msg.engine_status;
	def.attach_sta    = msg.attach_sta;
	def.unattach_sta  = msg.unattach_sta;

	return TRUE;
}
static void abs_stop_cp(TG_TrainAbsStop& rec, const MsgTrainAbsStop& msg)
{
	rec.position   = msg.position;
	rec.start_time = msg.start_time;
	rec.stop_time  = msg.stop_time;
}
BOOL TrainCopy(TG_TrainRecord& rec, const MsgTrainRecord& msg)
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
	
	

	rec.special_position   = msg.special_position;
	rec.special_time_1     = msg.special_time_1;
	rec.special_time_2     = msg.special_time_2; 
	rec.special_train_index = msg.special_train_index;
	rec.data_source         = msg.data_source; 
	rec.flag                = msg.flag;

	rec.can_not_meet        =msg.can_not_meet;
	rec.super_interval_ltime =msg.super_interval_ltime; 
	rec.send_plan_time=msg.send_plan_time; 

	rec.rank                 = msg.rank;
	return TRUE;
}
////////////////////////////////////////////////////////////
BOOL TrainCopy(MsgTrainInfo& msg, const TG_TrainInfo& def)
{
	STR_CPY(msg.train_name, def.train_name);

	msg.train_key            = def.train_key;
	msg.gpc_index      = def.gpc_train_index;
	msg.basic_index		  	 = def.basic_train_index;
	msg.shift_index		     = def.shift_train_index;
	msg.type                 = def.type;
	msg.run_type             = def.run_type;

	msg.start_actual_time    =def.start_actual_time; 
	msg.start_plan_time      =def.start_plan_time;
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
	msg.engine_utility       = def.engine_utility;
	msg.side_utility         = def.side_utility;
	msg.adjust_begin_sta     = def.adjust_begin_sta;
	msg.adjust_end_sta       = def.adjust_end_sta;
	msg.adjust_flag          = def.adjust_flag;
	msg.train_source         = def.train_source;
	
	STR_CPY(msg.shift_string,def.shift_string); 
	msg.update_time=def.update_time;      // 删除时间
	msg.delete_flag =def.delete_flag;      //  -1, 无
	return TRUE;

}
BOOL TrainCopy(MsgTrainEngineInfo& msg, const TG_TrainEngineInfo& def)
{
	STR_CPY(msg.engine_id,   def.engine_id);
	STR_CPY(msg.engine_type, def.engine_type);
	msg.engine_status = def.engine_status;
	msg.attach_sta    = def.attach_sta;
	msg.unattach_sta  = def.unattach_sta;

	return TRUE;
}
static void abs_stop_cp(MsgTrainAbsStop& msg, const TG_TrainAbsStop& rec)
{
	msg.position   = rec.position;
	msg.start_time = rec.start_time;
	msg.stop_time  = rec.stop_time;
}
BOOL TrainCopy(MsgTrainRecord& msg, const TG_TrainRecord& rec)
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

	

	msg.special_position   = rec.special_position;
	msg.special_time_1     = rec.special_time_1;
	msg.special_time_2     = rec.special_time_2; 
	msg.special_train_index = rec.special_train_index;
	msg.data_source         = rec.data_source; 
	msg.flag                = rec.flag;


	msg.can_not_meet=rec.can_not_meet;
	msg.super_interval_ltime=rec.super_interval_ltime;
	msg.send_plan_time=rec.send_plan_time; 

	msg.rank                 = rec.rank;
	return TRUE;
}


////////////////////////////////////////////////////
BOOL BlockCopy(MsgSchdBlock& msg, const TG_ScheduleBlock& def)
{
	msg.basic_index =def.basic_index;
	msg.shift_index = def.shift_index;
	msg.gpc_index =def.gpc_index; 
	msg.start_time = def.start_time;
	msg.end_time   = def.end_time;
	msg.type       = def.type;
	msg.flag       = def.flag;
	msg.dir        = def.line;
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
	msg.adjust_status =def.adjust_status;
	STR_CPY(msg.shift_string,def.shift_string);  
	return TRUE;
}
BOOL BlockCopy(TG_ScheduleBlock& def, const MsgSchdBlock& msg)
{
	def.basic_index=msg.basic_index;
	def.shift_index=msg.shift_index;
	def.gpc_index=msg.gpc_index;

	def.start_time = msg.start_time;
	def.end_time   = msg.end_time;
	def.type       = msg.type;
	def.flag       = msg.flag;
	def.line       = msg.dir;
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
	def.adjust_status =msg.adjust_status;
	STR_CPY(def.shift_string,msg.shift_string);  
	return TRUE;
}


BOOL  BlockCopy(MsgSchdBlock& msg,          const CTG_SingleMark& def)
{
	if(!BlockCopy(msg,def.m_data))
		return FALSE;
	int slen=def.m_info.length();
	if(slen>0)
	{
		msg.comment= new char[slen+1];
		strncpy(msg.comment,def.m_info.c_str(),slen);   
		msg.comment[slen]=0;
		msg.comment_len=slen; 
	}
	else
		msg.comment_len=0; 
	return TRUE;
}

BOOL  TrainCopy(MsgTrainInfo& msg,          const CTG_TrainInfo& def)
{
	if(!TrainCopy(msg,def.m_data))
		return FALSE;
	int slen=def.m_info.length();
	if(slen>0)
	{
		{
			msg.comment= new char[slen+1];
			strncpy(msg.comment,def.m_info.c_str(),slen);   
			msg.comment[slen]=0;
			msg.comment_len=slen; 
		}
	}
	else
		msg.comment_len=0; 
	return TRUE;
}

BOOL  TrainCopy(MsgTrainEngineInfo& msg,    const CTG_TrainEngineInfo& def)
{
	return TrainCopy(msg,def.m_data);
}

BOOL  TrainCopy(MsgTrainRecord& msg,        const CTG_TrainRecord& def)
{
	TrainCopy(msg,def.m_data);
	abs_stop_cp(msg.stop_out_of_signal, def.m_stop_out_of_signal);
	msg.abs_stop_count=def.m_listAbsStop.size();
	if ( msg.abs_stop_count> sizeof(msg.abs_stop)/sizeof(msg.abs_stop[0]))
		msg.abs_stop_count=sizeof(msg.abs_stop)/sizeof(msg.abs_stop[0]);
	     
	for (int i=0; i<msg.abs_stop_count; i++)
		abs_stop_cp(msg.abs_stop[i], def.m_listAbsStop[i]); 

	/*   列车文字信息  0 - abs  1 - station  2 - alarm */
	const char *p=def.m_strAbsText.c_str();
	if(p)
	{
		strncpy(msg.text[0],p,MAX_TRAIN_TEXT_LENGTH-1);  
		msg.text[0][MAX_TRAIN_TEXT_LENGTH-1]=0;
	}
	else
		msg.text[0][0]=0;
	p=def.m_strStationText.c_str();
	if(p)
	{
		strncpy(msg.text[1],p,MAX_TRAIN_TEXT_LENGTH-1);  
		msg.text[1][MAX_TRAIN_TEXT_LENGTH-1]=0;
	}
	else
		msg.text[1][0]=0;

	// 重点标注
	p=def.m_strNoteText.c_str();
	if(p)
	{
		strncpy(msg.text[2],p,MAX_TRAIN_TEXT_LENGTH-1);  
		msg.text[2][MAX_TRAIN_TEXT_LENGTH-1]=0;
	}
	else
		msg.text[2][0]=0;
	
	return TRUE;
}

BOOL  BlockCopy(CTG_SingleMark& def,     const MsgSchdBlock& msg)
{
	if(!BlockCopy(def.m_data,msg))
		return FALSE;
	if(msg.comment_len>0)
		{
			def.m_info=msg.comment;  
		}
		else
			def.m_info.clear();
		return TRUE;
}

BOOL  TrainCopy(CTG_TrainInfo& def,         const MsgTrainInfo& msg)
{
		if(!TrainCopy(def.m_data,msg))
			return FALSE;
		if(msg.comment_len>0)
		{
			def.m_info=msg.comment;  
		}
		else
			def.m_info.clear();
		return TRUE;
}

BOOL  TrainCopy(CTG_TrainEngineInfo& def,   const MsgTrainEngineInfo& msg)
{
	return TrainCopy(def.m_data,msg);
}

BOOL  TrainCopy(CTG_TrainRecord& def,       const MsgTrainRecord& msg)
{
	TrainCopy(def.m_data,msg);
	abs_stop_cp(def.m_stop_out_of_signal, msg.stop_out_of_signal);
	for (int i=0; i<msg.abs_stop_count; i++)
	{
		TG_TrainAbsStop rec;
		abs_stop_cp(rec, msg.abs_stop[i]);
		def.m_listAbsStop.push_back(rec); 
	}
	def.m_strAbsText=msg.text[0];
	def.m_strStationText=msg.text[1];
	def.m_strAlarmText="";
	def.m_strNoteText=msg.text[2];

	return TRUE;
}

void CopyStamp(MsgSchdStamp &dest,const TG_MsgStamp &src)
{
				dest.con_id=src.con_id;
				dest.index=src.index;
				dest.ltime=src.ltime; // NA???
				dest.operatr=src.operatr;
				dest.schd_type = src.schedule_type; 
}
void CopyStamp(TG_MsgStamp &dest,const  MsgSchdStamp&src)
{
				dest.con_id=src.con_id;
				dest.index=src.index;
				dest.ltime=src.ltime; // NA???
				dest.operatr=src.operatr;
				dest.schedule_type = src.schd_type;
}

BOOL  TrainCopy(MsgTrain& msg,const CTG_SingleTrain& train)
{
	MsgTrainRecord *pRecs;
	TrainCopy(msg.info,*train.GetTrainInfo_Const());
	int rec_count=train.GetTrainRecordCountWithDeleted() ;
	if(rec_count>0)
	{	
		pRecs=new MsgTrainRecord[rec_count];
		ASSERT(pRecs!=NULL);
		for(int i=0;i<rec_count;i++)
			TrainCopy(pRecs[i],*train.GetTrainRecord_Const(i));
		msg.rec_ptr=pRecs;
		msg.rec_count=rec_count;
	}
	else
	{
		msg.rec_count=0;
		msg.rec_ptr=NULL;
	}
	rec_count=train.GetEngineCount();
	MsgTrainEngineInfo *pEngRecs;
	if(rec_count>0)
	{	
		pEngRecs=new MsgTrainEngineInfo[rec_count];
		ASSERT(pEngRecs!=NULL);
		for(int i=0;i<rec_count;i++)
			TrainCopy(pEngRecs[i],*train.GetEngine_Const(i));
		msg.engine_ptr=pEngRecs;
		msg.engine_count=rec_count;
	}
	else
	{
		msg.engine_ptr=NULL;
		msg.engine_count=0;
	}

	return TRUE;
}

BOOL  TrainCopy(CTG_SingleTrain& train,const MsgTrain& msg)
{
	int i;
				TrainCopy(*train.GetTrainInfo(),msg.info);
				
				for(i=0;i<msg.rec_count;i++)
				{
					CTG_TrainRecord rec;
					TrainCopy(rec,msg.rec_ptr[i]);
					rec.rec_index = i;
					train.GetRecordList()->AddRecord(rec);
				}
				for(i=0;i<msg.engine_count;i++)
				{
					CTG_TrainEngineInfo rec;
					TrainCopy(rec,msg.engine_ptr[i]);
					train.GetEngineList()->AddRecord(rec);
				}
			return TRUE;
}

BOOL  TrainCopy(MsgTrainUpdateEngine &msg,const CTG_SingleTrain& train)
{
	int rec_count=train.GetEngineCount();
				MsgTrainEngineInfo *pEngRecs;
				if(rec_count>0)
				{	
					pEngRecs=new MsgTrainEngineInfo[rec_count];
					ASSERT(pEngRecs!=NULL);
					for(int i=0;i<rec_count;i++)
						TrainCopy(pEngRecs[i],*train.GetEngine_Const(i));
					msg.engine_ptr=pEngRecs;
					msg.engine_count=rec_count;
				}
				else
				{
					msg.engine_ptr=NULL;
					msg.engine_count=0;
				}
				
			return TRUE;

}
BOOL  TrainCopy(CTG_SingleTrain& train,const MsgTrainUpdateEngine &msg)
{
	int i;
				for(i=0;i<msg.engine_count;i++)
				{
					CTG_TrainEngineInfo rec;
					TrainCopy(rec,msg.engine_ptr[i]);
					train.GetEngineList()->AddRecord(rec);
				}
				
	return TRUE;
}

BOOL  TrainCopy(MsgTrainUpdateRecord &msg,const CTG_SingleTrain& train)
{
				MsgTrainRecord *pRecs;
				short *pRecIndex;
				int rec_count=train.GetTrainRecordCountWithDeleted(); 
				if(rec_count>0)
				{	
					pRecs=new MsgTrainRecord[rec_count];
					pRecIndex = new short[rec_count];
					ASSERT(pRecs!=NULL && pRecIndex!=NULL);
					for(int i=0;i<rec_count;i++)
					{
						TrainCopy(pRecs[i],*train.GetTrainRecord_Const(i));
						pRecIndex[i]=i;
					}
					msg.rec_ptr=pRecs;
					msg.rec_index= pRecIndex;
					msg.rec_cnt=rec_count;
				}
				else
				{
					msg.rec_cnt=0;
					msg.rec_ptr=NULL;
				}
				
				return TRUE;
}
// NA??? 是否存在更新特定记录？
BOOL  TrainCopy(CTG_SingleTrain& train,const MsgTrainUpdateRecord &msg )
{
	int i;
	for(i=0;i<msg.rec_cnt;i++)
	{
		CTG_TrainRecord rec;
		TrainCopy(rec,msg.rec_ptr[i]);
		rec.rec_index = i;
		train.GetRecordList()->AddRecord(rec);
	}
	return TRUE;
}

BOOL  TrainCopy(MsgTrainUpdateInfo &msg,const CTG_SingleTrain& train)
{
	return TrainCopy(msg,*train.GetTrainInfo_Const());
}

BOOL  TrainCopy(CTG_SingleTrain& train,const MsgTrainUpdateInfo &msg )
{
	return TrainCopy(*train.GetTrainInfo(),msg);
}

BOOL  TrainExtentCopy(CTG_TrainExtent& trainExtent,const MsgTrainExtent& msg)
{
	trainExtent.prev_train_index=msg.prev_train_index;
	trainExtent.prev_basic_index=msg.prev_basic_index;
	trainExtent.prev_shift_index=msg.prev_shift_index;
	trainExtent.next_shift_index=msg.next_shift_index;
	trainExtent.prev_dest_sta=msg.prev_dest_sta; 
	trainExtent.next_dest_sta=msg.next_dest_sta;
	STR_CPY(trainExtent.chezu_no,msg.chezu_no);

	if(msg.kd_ptr!=NULL)
	{
		trainExtent.kd_type=msg.kd_ptr->type;
		if(msg.kd_ptr->text_len>0)
			trainExtent.kd_content=msg.kd_ptr->text;
		else
			trainExtent.kd_content="";
	}
	else
	{
		trainExtent.kd_type=0;
		trainExtent.kd_content="";
	}
	
	for(int i=0; i<msg.yingye_count; i++)
	{
		TG_TrainYingyeInfo yy;
		yy.station_no=msg.yy_ptr[i].station_no;
		yy.arrive_time=msg.yy_ptr[i].arrive_time;
		yy.depart_time=msg.yy_ptr[i].depart_time;
		STR_CPY(yy.station_name,msg.yy_ptr[i].station_name);
		trainExtent.yy_list.push_back(yy);
	}

	for(int i=0; i<msg.crew_count; i++)
	{
		TG_TrainCrewInfo cr;
		cr.crew_type=msg.crew_ptr[i].crew_type;
		STR_CPY(cr.crew_info,msg.crew_ptr[i].crew_info);
		STR_CPY(cr.crew_bureau,msg.crew_ptr[i].crew_bureau);

		trainExtent.crew_list.push_back(cr);
	}

	for(int i=0; i<msg.ticket_count; i++)
	{
		TG_TrainTicketInfo tk;
		tk.on_station_index=msg.ticket_ptr[i].on_station_index;
		tk.on_station=msg.ticket_ptr[i].on_station;
		tk.down_station_index=msg.ticket_ptr[i].down_station_index;
		tk.down_station=msg.ticket_ptr[i].down_station;
		tk.sale=msg.ticket_ptr[i].sale;

		STR_CPY(tk.seat,msg.ticket_ptr[i].seat);
		STR_CPY(tk.on_station_name,msg.ticket_ptr[i].on_station_name);
		STR_CPY(tk.down_station_name,msg.ticket_ptr[i].down_station_name);

		trainExtent.ticket_list.push_back(tk);
	}

	for(int i=0; i<msg.engine_crew_count; i++)
	{
		TG_EngineCrewInfo ec;
		ec.station_no=msg.engine_crew_ptr[i].station_no;
		ec.overwork_time=msg.engine_crew_ptr[i].overwork_time;	
		STR_CPY(ec.station_name,msg.engine_crew_ptr[i].station_name);
	
		trainExtent.engine_list.push_back(ec);
	}

	for(int i=0; i<msg.opms_count; i++)
	{
		TG_TrainOPMSRecordInfo rd;
		rd.station_no=msg.opms_ptr[i].station_no;
		rd.work_flag=msg.opms_ptr[i].work_flag;
		rd.oper_flag=msg.opms_ptr[i].oper_flag;
		rd.important_flag=msg.opms_ptr[i].important_flag;	
	
		STR_CPY(rd.oper_info,msg.opms_ptr[i].oper_info);
		STR_CPY(rd.note_text,msg.opms_ptr[i].note_text);
	
		trainExtent.opms_rd_list.push_back(rd);
	}

	return TRUE;
}

BOOL  TrainExtentCopy(MsgTrainExtent& msg, const CTG_SingleTrain& train)
{
	const TG_TrainInfoExpand* pTrainExpand=train.GetTrainInfoExpand_Const();

	const CTG_TrainRelation* pRL=pTrainExpand->GetTrainRelation_Const();
	msg.prev_train_index=pRL->prev_train_index;
	msg.prev_basic_index=pRL->prev_basic_index;
	msg.prev_shift_index=pRL->prev_shift_index;
	msg.next_shift_index=pRL->next_shift_index;
	msg.prev_dest_sta=pRL->prev_dest_sta; 
	msg.next_dest_sta=pRL->next_dest_sta;
	CString str=pRL->czh_no_str;
	if(str.GetLength()>0 && str.GetLength()<32)
	{
		STR_CPY(msg.chezu_no, str);
	}
	else
	{
		msg.chezu_no[0]=0;
	}
	
	const CTG_TrainKediaoInfo* pKD=pTrainExpand->GetTrainKediao_Const();
	CString kd_content=pKD->content;
	msg.kd_ptr=new MsgTrainKediaoInfo();
	if(msg.kd_ptr==NULL)  return FALSE;
	msg.kd_ptr->type=pKD->type;
	msg.kd_ptr->SetKediao(kd_content); 
		
	const CTG_TrainYingyeList* yyList=pTrainExpand->GetTrainYYList_Const();
	int count=yyList->GetCount();
	if(count<=0)
		msg.yy_ptr=NULL;
	else
	{
		msg.yy_ptr=new MsgTrainYingyeInfo[count];
		if(msg.yy_ptr==NULL)  return FALSE;
		msg.yingye_count=count;
		for(int i=0; i<msg.yingye_count; i++)
		{
			const TG_TrainYingyeInfo* p=yyList->GetRecord_Const(i);
			if(p==NULL)
				return FALSE;

			msg.yy_ptr[i].station_no=p->station_no;
			msg.yy_ptr[i].arrive_time=p->arrive_time;
			msg.yy_ptr[i].depart_time=p->depart_time;
			STR_CPY(msg.yy_ptr[i].station_name,p->station_name);
		}
	}

	const CTG_TrainCrewList* crList=pTrainExpand->GetTrainCrewList_Const();
	count=crList->GetCount();
	if(count<=0)
		msg.crew_ptr=NULL;
	else
	{
		msg.crew_ptr=new MsgTrainCrewInfo[count];
		if(msg.crew_ptr==NULL)  return FALSE;
		msg.crew_count=count;
		for(int i=0; i<msg.crew_count; i++)
		{
			const TG_TrainCrewInfo* p=crList->GetRecord_Const(i);
			if(p==NULL)
				return FALSE;

			msg.crew_ptr[i].crew_type=p->crew_type;
			STR_CPY(msg.crew_ptr[i].crew_info,p->crew_info);
			STR_CPY(msg.crew_ptr[i].crew_bureau,p->crew_bureau);
		}
	}

	const CTG_TrainTicketList* tkList=pTrainExpand->GetTrainTicketList_Const();
	count=tkList->GetCount();
	if(count<=0)
		msg.ticket_ptr=NULL;
	else
	{
		msg.ticket_ptr=new MsgTrainPassgenerTicketInfo[count];
		if(msg.ticket_ptr==NULL)  return FALSE;
		msg.ticket_count=count;
		for(int i=0; i<msg.ticket_count; i++)
		{
			const TG_TrainTicketInfo* p=tkList->GetRecord_Const(i);
			if(p==NULL)
				return FALSE;

			msg.ticket_ptr[i].on_station_index=p->on_station_index;
			msg.ticket_ptr[i].on_station=p->on_station;
			msg.ticket_ptr[i].down_station_index=p->down_station_index;
			msg.ticket_ptr[i].down_station=p->down_station;
			msg.ticket_ptr[i].sale=p->sale;
			STR_CPY(msg.ticket_ptr[i].seat,p->seat);
			STR_CPY(msg.ticket_ptr[i].on_station_name,p->on_station_name);
			STR_CPY(msg.ticket_ptr[i].down_station_name,p->down_station_name);
		}
	}

	const CTG_EngineCrewList* ecList=pTrainExpand->GetTrainEngineCrewList_Const();
	count=ecList->GetCount();
	if(count<=0)
		msg.engine_crew_ptr=NULL;
	else
	{
		msg.engine_crew_ptr=new MsgEngineCrewInfo[count];
		if(msg.engine_crew_ptr==NULL)  return FALSE;
		msg.engine_crew_count=count;
		for(int i=0; i<msg.engine_crew_count; i++)
		{
			const TG_EngineCrewInfo* p=ecList->GetRecord_Const(i);
			if(p==NULL)
				return FALSE;

			msg.engine_crew_ptr[i].station_no=p->station_no;
			msg.engine_crew_ptr[i].overwork_time=p->overwork_time;
			
			STR_CPY(msg.engine_crew_ptr[i].station_name,p->station_name);
		}
	}

	count = train.GetTrainRecordCount();
	if(count<=0)
		msg.opms_ptr=NULL;
	else
	{
		msg.opms_ptr=new MsgTrainOPMSInfo[count];
		if(msg.opms_ptr==NULL)  return FALSE;
		msg.opms_count=0;
		for(int i=0; i<count; i++)
		{
			const CTG_TrainRecord* p=train.GetTrainRecordByRecIndex_Const(i);
			if(p==NULL)
				return FALSE;

			msg.opms_ptr[i].station_no=p->m_data.station;
			msg.opms_ptr[i].work_flag=p->m_record_expand.work_flag;
			msg.opms_ptr[i].oper_flag=p->m_record_expand.oper_flag;
			msg.opms_ptr[i].important_flag=p->m_record_expand.important_flag;

			CString str=p->m_record_expand.oper_info;
			if(str.GetLength()>=255 || str.GetLength()==0)
			{
				msg.opms_ptr[i].oper_info[0]=0;
			}
			else
			{
				STR_CPY(msg.opms_ptr[i].oper_info, str);
			}

			str=p->m_record_expand.note_text;
			if(str.GetLength()>=255 || str.GetLength()==0)
			{
				msg.opms_ptr[i].note_text[0]=0;
			}
			else
			{
				STR_CPY(msg.opms_ptr[i].note_text, str);
			}
			msg.opms_count++;
		}
	}

	msg.cmd_count=0;
	msg.cmd_ptr=NULL;
	
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
BOOL  TrainCopyForOCX(MsgTrainOCX& msg,const CTG_SingleTrain& train)
{
	// train_index
	msg.train_index=train.GetTrainIndex();
	msg.prev_train_index=train.GetPrevTrainIndex();
	// INFO
	{
		const TG_TrainInfo& def=train.GetTrainInfo_Const()->m_data;

		msg.self_update				  = train.IsSelfUpdate();
		STR_CPY(msg.info.train_name, def.train_name);

		msg.info.train_key            = def.train_key;
		msg.info.basic_index		  = def.basic_train_index;
		msg.info.gpc_index			  = def.gpc_train_index;
		msg.info.shift_index		  = def.shift_train_index;
		STR_CPY(msg.info.shift_string,def.shift_string); 
		msg.info.update_time		  = def.update_time; 
		msg.info.type                 = def.type;
		msg.info.run_type             = def.run_type;

		msg.info.start_plan_time      =def.start_plan_time;
		msg.info.start_actual_time    =def.start_actual_time; 
		
		msg.info.sborder_plan_time    = def.sborder_plan_time;
		msg.info.sborder_actual_time  = def.sborder_actual_time;
		msg.info.eborder_plan_time    = def.eborder_plan_time;
		msg.info.eborder_actual_time  = def.eborder_actual_time;
		msg.info.sborder_sta          = def.sborder_sta;
		msg.info.eborder_sta          = def.eborder_sta;

		STR_CPY(msg.info.start_sta_name, def.start_sta_name) 
		STR_CPY(msg.info.end_sta_name,   def.end_sta_name) 
		
		msg.info.from_conid           = def.from_conid;
		msg.info.to_conid             = def.to_conid;
		msg.info.priority             = def.priority;
		msg.info.train_length         = def.train_length;
		msg.info.train_weight         = def.train_weight;
		msg.info.status               = def.status;
		msg.info.space_rank           = def.space_rank;
		msg.info.length_rank          = def.length_rank;
		msg.info.super_rank           = def.super_rank;
		msg.info.military_rank        = def.military_rank;
		msg.info.engine_utility       = def.engine_utility;
		msg.info.side_utility         = def.side_utility;
		msg.info.adjust_begin_sta     = def.adjust_begin_sta;
		msg.info.adjust_end_sta       = def.adjust_end_sta;
		msg.info.adjust_flag          = def.adjust_flag;
		msg.info.train_source         = def.train_source;
		
		msg.info.delete_flag		  = def.delete_flag;      //  -1, 无
	}
	
	// RECORDS
	{
		msg.rec_count=train.GetTrainRecordCount();
		if(msg.rec_count<=0)
		{
			return FALSE; // 没有有效记录无需传递到OCX显示
		}
		msg.rec_ptr=new MsgTrainRecordOCX[msg.rec_count];
		if(msg.rec_ptr==NULL)
		{
			msg.rec_count=0;
			return FALSE; // memory fail
		}
		int len = 0;
		const char* ptmpstr = NULL;
		for(int i=0;i<msg.rec_count;i++)
		{
			const CTG_TrainRecord *prec=train.GetTrainRecord_Const(i);
	
			ptmpstr=prec->GetOpmsSationTextState();
			if(ptmpstr)
				msg.rec_ptr[i].opms_station_text_state = atoi(ptmpstr);
			else
				msg.rec_ptr[i].opms_station_text_state = 1;
			
			ptmpstr=prec->GetStationText(STATION_TXT_INFO_TYPE_OPMS);
			if(ptmpstr)
			{
				len = sizeof(msg.rec_ptr[i].opms_station_text)-1;
				strncpy(msg.rec_ptr[i].opms_station_text, ptmpstr, len);
				msg.rec_ptr[i].opms_station_text[len]=0;
			}
			else
				msg.rec_ptr[i].opms_station_text[0]=0;
						
			ptmpstr=prec->GetStationText(STATION_TXT_INFO_TYPE_SHUAIGUA);
			if(ptmpstr)
			{
				len = sizeof(msg.rec_ptr[i].station_text)-1;
				strncpy(msg.rec_ptr[i].station_text, ptmpstr, len);
				msg.rec_ptr[i].station_text[len]=0;
			}
			else
				msg.rec_ptr[i].station_text[0]=0;
			
			if(prec->m_strAbsText.empty())
				msg.rec_ptr[i].abs_text[0]=0;
			else
			{
				len = sizeof(msg.rec_ptr[i].abs_text)-1;
				strncpy(msg.rec_ptr[i].abs_text, prec->m_strAbsText.c_str(), len);
				msg.rec_ptr[i].abs_text[len]=0;
			}

			if(prec->m_strAlarmText.empty())
				msg.rec_ptr[i].alarm_text[0]=0;
			else
			{
				len = sizeof(msg.rec_ptr[i].alarm_text)-1;
				strncpy(msg.rec_ptr[i].alarm_text, prec->m_strAlarmText.c_str(), len);
				msg.rec_ptr[i].alarm_text[len]=0;
			}

			if(prec->m_strNoteText.empty())
				msg.rec_ptr[i].plan_note[0]=0;
			else
			{
				len = sizeof(msg.rec_ptr[i].plan_note)-1;
				strncpy(msg.rec_ptr[i].plan_note, prec->m_strNoteText.c_str(), len);
				msg.rec_ptr[i].plan_note[len]=0;
			}

			STR_CPY(msg.rec_ptr[i].arrive_train_id,          prec->m_data.arrive_train_id);
			STR_CPY(msg.rec_ptr[i].original_arrive_train_id, prec->m_data.original_arrive_train_id);
			STR_CPY(msg.rec_ptr[i].depart_train_id,          prec->m_data.depart_train_id);
			STR_CPY(msg.rec_ptr[i].original_depart_train_id, prec->m_data.original_depart_train_id);

			msg.rec_ptr[i].station            = prec->m_data.station;

			msg.rec_ptr[i].entry              = prec->m_data.entry;
			msg.rec_ptr[i].exit               = prec->m_data.exit;

			msg.rec_ptr[i].arrive_side        = prec->m_data.arrive_side;
			msg.rec_ptr[i].plan_arrive_side   = prec->m_data.plan_arrive_side;
			msg.rec_ptr[i].depart_side        = prec->m_data.depart_side;
			msg.rec_ptr[i].plan_depart_side   = prec->m_data.plan_depart_side;

			msg.rec_ptr[i].min_stop_time      = prec->m_data.min_stop_time;

			msg.rec_ptr[i].arrive             = prec->m_data.arrive;
			msg.rec_ptr[i].depart             = prec->m_data.depart;
			msg.rec_ptr[i].plan_arrive        = prec->m_data.plan_arrive;
			msg.rec_ptr[i].plan_depart        = prec->m_data.plan_depart;

			msg.rec_ptr[i].stop_reason        = prec->m_data.stop_reason;
			msg.rec_ptr[i].late_reason        = prec->m_data.late_reason;
			msg.rec_ptr[i].early_depart_time  = prec->m_data.early_depart_time;
			msg.rec_ptr[i].run_fast_slow_time = prec->m_data.run_fast_slow_time;
			msg.rec_ptr[i].route_flag         = prec->m_data.route_flag;
			msg.rec_ptr[i].speed_limit        = prec->m_data.speed_limit;
			msg.rec_ptr[i].stop_condition     = prec->m_data.stop_condition;
			msg.rec_ptr[i].adjust_status      = prec->m_data.adjust_status;
			msg.rec_ptr[i].spec_run_time      = prec->m_data.spec_run_time;

			msg.rec_ptr[i].special_position   = prec->m_data.special_position;
			msg.rec_ptr[i].special_time_1     = prec->m_data.special_time_1;
			msg.rec_ptr[i].special_time_2     = prec->m_data.special_time_2; 
			msg.rec_ptr[i].special_train_index = prec->m_data.special_train_index;
			msg.rec_ptr[i].data_source         = prec->m_data.data_source; 
			msg.rec_ptr[i].flag                = prec->m_data.flag;
			msg.rec_ptr[i].super_interval_ltime= prec->m_data.super_interval_ltime;
			
			msg.rec_ptr[i].in_station_index	 = prec->GetArriveTGIndex();
			msg.rec_ptr[i].out_station_index   = prec->GetDepartTGIndex();

			msg.rec_ptr[i].can_not_meet		 = prec->m_data.can_not_meet;
			msg.rec_ptr[i].rank                = prec->m_data.rank;
			msg.rec_ptr[i].send_plan_time      = prec->m_data.send_plan_time; 

			msg.rec_ptr[i].abs_stop_count		 = prec->m_listAbsStop.size();	// 停车个数
			if(msg.rec_ptr[i].abs_stop_count>8)
				msg.rec_ptr[i].abs_stop_count = 8;
			
			for(int n=0; n<sizeof(msg.rec_ptr[i].abs_stop)/sizeof(msg.rec_ptr[i].abs_stop[0]); n++)
			{
				if(n<prec->m_listAbsStop.size())
				{
					msg.rec_ptr[i].abs_stop[n].flag = prec->m_listAbsStop[n].flag;
					msg.rec_ptr[i].abs_stop[n].position = prec->m_listAbsStop[n].position;
					msg.rec_ptr[i].abs_stop[n].start_time = prec->m_listAbsStop[n].start_time;
					msg.rec_ptr[i].abs_stop[n].stop_time = prec->m_listAbsStop[n].stop_time;
				}
				else
				{
					msg.rec_ptr[i].abs_stop[n].flag = 0;
					msg.rec_ptr[i].abs_stop[n].position = 0;
					msg.rec_ptr[i].abs_stop[n].start_time = 0;
					msg.rec_ptr[i].abs_stop[n].stop_time = 0;
				}
			}
			
			ISDWORD display_status=0; // bit0,bit1被用于命令签收
			// 预留将prec->m_data_ex.important_flag翻译到display_status中
			msg.rec_ptr[i].display_status = display_status;
		}
	}
	
	// ENGINES
	{
		msg.engine_count   = train.GetEngineCount();
		msg.engine_ptr  = NULL;
		if(msg.engine_count>0)
		{	
			msg.engine_ptr=new MsgTrainEngineInfo[msg.engine_count];
			if(msg.engine_ptr == NULL)
			{
				msg.engine_count = 0;
				return FALSE;
			}
			for(int i=0;i<msg.engine_count;i++)
			{
				const TG_TrainEngineInfo& Info = train.GetEngine_Const(i)->m_data;

				STR_CPY(msg.engine_ptr[i].engine_id, Info.engine_id);
				STR_CPY(msg.engine_ptr[i].engine_type, Info.engine_type);
				msg.engine_ptr[i].engine_status	= Info.engine_status;
				msg.engine_ptr[i].attach_sta	= Info.attach_sta;
				msg.engine_ptr[i].unattach_sta	= Info.unattach_sta;
			}
		}
	}
	
	return TRUE;
}

BOOL  BlockCopyForOCX(MsgSchdBlockOCX& msg,  const CTG_SingleMark& def)
{
	msg.station1=def.m_data.station1;
	msg.station2=def.m_data.station2;
	msg.direction =def.m_data.direction;
	msg.mark_index=def.m_nBlockIndex;
	msg.self_update=def.IsSelfUpdate();

	msg.basic_index =def.m_data.basic_index;
	msg.shift_index = def.m_data.shift_index;
	msg.gpc_index =def.m_data.gpc_index; 
	msg.start_time = def.m_data.start_time;
	msg.end_time   = def.m_data.end_time;
	msg.type       = def.m_data.type;
	msg.flag       = def.m_data.flag;
	msg.dir        = def.m_data.line;
	msg.station    = def.m_data.station;
	msg.index      = def.m_data.index;
	msg.position   = def.m_data.position;
	msg.ele_count  = def.m_data.ele_count;
	for (int i=0; i<msg.ele_count; i++)
		msg.ele_number[i] = def.m_data.ele_number[i];

	msg.text_len =  strlen(def.m_data.text);
	memcpy(msg.text, def.m_data.text, msg.text_len);

	msg.s_meter   = def.m_data.s_meter;
	msg.e_meter   = def.m_data.e_meter;
	msg.e_kilo    = def.m_data.e_kilo;
	msg.slow_time = def.m_data.slow_time;
	msg.adjust_status =def.m_data.adjust_status;
	STR_CPY(msg.shift_string,def.m_data.shift_string);  
	
	int slen=def.m_info.length();
	if(slen>0)
	{
		msg.comment= new char[slen+1];
		strncpy(msg.comment,def.m_info.c_str(),slen);   
		msg.comment[slen]=0;
		msg.comment_len=slen; 
	}
	else
		msg.comment_len=0; 
	return TRUE;
}
	
