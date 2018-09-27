#include "stdafx.h"
#include <baseMsg\msgid.h>
#include <baseMsg\clbth.h>
#include <baseMsg\msg_schd.h>
#include "tg_schedule_base.h"
#include "tg_function_config.h"
#include "function.h"
#include "itemname.h"
#include "icommtask.h"
#include "stplanlog.h"

cStPlanSendingLog::cStPlanSendingLog()
{
}
cStPlanSendingLog::~cStPlanSendingLog()
{
 }

void cStPlanSendingLog::LogMsg(const MsgStPlanSendOrder & msg, ENTITY_ID sender_entity)
{
	TIME ltime = time(NULL);
	
	if(GetTGFunctionConfig()->IsLocalLogSendStPlan())
	{
		CString logstr;logstr.Format( SEND_STPLAN_ORDER, 
			StringDate(ltime), StringTime(ltime), msg.station, msg.con_id, msg.st, sender_entity,
			msg.sender_name, StringDate(msg.send_time), StringTime(msg.send_time),StringDate(msg.begin_time),StringTime(msg.begin_time),StringDate(msg.end_time),StringTime(msg.end_time));

		SaveLogMessageStr("stplan",ltime,logstr);
		
	}

}
void cStPlanSendingLog::LogMsg(const MsgStPlanRequest & msg, ENTITY_ID request_entity)
{
	TIME ltime = time(NULL);
	
	if(GetTGFunctionConfig()->IsLocalLogSendStPlan())
	{
		CString logstr;logstr.Format( REQUEST_STPLAN,
			StringDate(ltime), StringTime(ltime), msg.station, msg.con_id, request_entity,
			StringDate(msg.begin_time), StringTime(msg.begin_time), StringDate(msg.end_time), StringTime(msg.end_time));

	SaveLogMessageStr("stplan",ltime,logstr);
		
	}

}

void cStPlanSendingLog::LogMsg(const MsgStPlanSendInit & msg, ENTITY_ID recv_entity)
{
	TIME ltime = time(NULL);
	
	if(GetTGFunctionConfig()->IsLocalLogSendStPlan())
	{
		CString logstr;logstr.Format( SEND_STPLAN_BEGIN,
			StringDate(ltime), StringTime(ltime), msg.station, msg.con_id, msg.gpc, recv_entity, 
			msg.sender_name, StringDate(msg.send_time), StringTime(msg.send_time));

		SaveLogMessageStr("stplan",ltime,logstr);
		
	}
}
void cStPlanSendingLog::LogMsg(const MsgStPlanInsert & msg, ENTITY_ID recv_entity)
{
	TIME ltime = time(NULL);
	
	if(GetTGFunctionConfig()->IsLocalLogSendStPlan())
	{
		CString logstr;logstr.Format( SEND_STPLAN_INSERT,
			StringDate(ltime), StringTime(ltime), msg.station, msg.con_id, msg.gpc, recv_entity,
			msg.train_index, msg.rec_index, msg.type, msg.run_type, msg.arrive_train_id, msg.depart_train_id, 
			StringDate(msg.arrive), StringTime(msg.arrive), msg.plan_arrive, StringDate(msg.depart), StringTime(msg.depart), msg.plan_depart,
			msg.entry, msg.arrive_side, msg.depart_side, msg.exit, msg.min_stop_time, msg.early_depart_time, msg.meet_count, msg.status, msg.stop_condition, msg.flag);

		for(int i=0; i<msg.meet_count; i++)
		{
			if (MsgStPlanInsert::MEET_SAME_DIR == msg.meet_status[i])
			{
				CString logstr2;
				logstr2.Format(SEND_STPLAN_MEET_SAME_DIR, msg.meet_train_index[i], msg.meet_rec_index[i]);
				logstr+=logstr2;
			}

			else if (MsgStPlanInsert::MEET_DIFF_DIR == msg.meet_status[i])
			{
				CString logstr2;
				logstr2.Format(SEND_STPLAN_MEET_DIFF_DIR, msg.meet_train_index[i], msg.meet_rec_index[i]);
				logstr+=logstr2;
			}
		}

		CString logstr3;
		logstr3.Format(DEPART_TRAIN_BY_PLAN, ((msg.stop_condition&0x80)==0x80)?1:0);
		logstr+=logstr3;
		
		logstr3.Format("³¬ÏÞ%d-%d",  msg.space_rank&0x07,  (msg.space_rank>>3)&0x07);
		logstr+=logstr3;

		SaveLogMessageStr("stplan",ltime,logstr);
		
	}
}
void cStPlanSendingLog::LogMsg(const MsgStPlanDelete & msg, ENTITY_ID recv_entity)
{
	TIME ltime = time(NULL);
	
	if(GetTGFunctionConfig()->IsLocalLogSendStPlan())
	{
		CString logstr;logstr.Format( SEND_STPLAN_DELETE,
			StringDate(ltime), StringTime(ltime), msg.station, msg.con_id, msg.gpc, recv_entity, 
			msg.train_index, msg.rec_index, msg.arrive_train_id, msg.depart_train_id, StringDate(msg.arrive), StringTime(msg.arrive), StringDate(msg.depart), StringTime(msg.depart));

		SaveLogMessageStr("stplan",ltime,logstr);
		
	}
}
void cStPlanSendingLog::LogMsg(const MsgStPlanSendComplete & msg, ENTITY_ID recv_entity)
{
	TIME ltime = time(NULL);
	
	if(GetTGFunctionConfig()->IsLocalLogSendStPlan())
	{
		CString logstr;logstr.Format( SEND_STPLAN_END,
			StringDate(ltime), StringTime(ltime), msg.station, msg.con_id, msg.gpc, recv_entity, msg.insert_cnt, msg.delete_cnt);

	SaveLogMessageStr("stplan",ltime,logstr);
		
	}
}
void cStPlanSendingLog::LogMsg(const MsgRouteStatusInform &msg, ENTITY_ID entity)
{
	TIME ltime = time(NULL);

	if(GetTGFunctionConfig()->IsLocalLogSendStPlan())
	{
		CString logstr;logstr.Format( UPDATE_STPLAN_SIDE,
			StringDate(ltime), StringTime(ltime), msg.station, msg.con_id, msg.train_index, msg.rec_index, entity);

		SaveLogMessageStr("stplan",ltime,logstr);
		
	}
}

void cStPlanSendingLog::LogMsg(const MsgStPlanStatusAnswer & msg)
{
	TIME ltime = time(NULL);
	
	if(GetTGFunctionConfig()->IsLocalLogSendStPlan())
	{
		CString logstr;
		if (MsgStPlanStatusAnswer::PLAN_RECV == msg.recv_status)
		{
			logstr.Format( SEND_STPLAN_ANSWER_PLAN_RECV, StringDate(ltime), StringTime(ltime), msg.station, msg.con_id, msg.dest);
			//snprintf(log.inform, sizeof(log.inform)-1, SEND_STPLAN_ANSWER_PLAN_RECV, StringDate(ltime), StringTime(ltime), msg.station, msg.con_id, msg.dest);
		}
		else if (MsgStPlanStatusAnswer::PLAN_SIGN == msg.recv_status)
		{
			logstr.Format( SEND_STPLAN_ANSWER_PLAN_SIGN, StringDate(ltime), StringTime(ltime), msg.station, msg.con_id, msg.dest);
			//snprintf(log.inform, sizeof(log.inform)-1, SEND_STPLAN_ANSWER_PLAN_SIGN, StringDate(ltime), StringTime(ltime), msg.station, msg.con_id, msg.dest);
		}
		else if (MsgStPlanStatusAnswer::PLAN_SENDING == msg.recv_status)
		{
			logstr.Format( SEND_STPLAN_ANSWER_PLAN_SENDING, StringDate(ltime), StringTime(ltime), msg.station, msg.con_id, msg.dest);
			//snprintf(log.inform, sizeof(log.inform)-1, SEND_STPLAN_ANSWER_PLAN_SENDING, StringDate(ltime), StringTime(ltime), msg.station, msg.con_id, msg.dest);
		}
		else if (MsgStPlanStatusAnswer::PLAN_NO_CHANGE == msg.recv_status)
		{
			logstr.Format( SEND_STPLAN_ANSWER_PLAN_NO_CHANGE, StringDate(ltime), StringTime(ltime), msg.station, msg.con_id, msg.dest);
			//snprintf(log.inform, sizeof(log.inform)-1, SEND_STPLAN_ANSWER_PLAN_NO_CHANGE, StringDate(ltime), StringTime(ltime), msg.station, msg.con_id, msg.dest);
		}
		else if (MsgStPlanStatusAnswer::PLAN_NO_DATA == msg.recv_status)
		{
			logstr.Format( SEND_STPLAN_ANSWER_PLAN_NO_DATA, StringDate(ltime), StringTime(ltime), msg.station, msg.con_id, msg.dest);
			//snprintf(log.inform, sizeof(log.inform)-1, SEND_STPLAN_ANSWER_PLAN_NO_DATA, StringDate(ltime), StringTime(ltime), msg.station, msg.con_id, msg.dest);
		}
		else if (MsgStPlanStatusAnswer::PLAN_ENTITY_DEAD == msg.recv_status)
		{
			logstr.Format( SEND_STPLAN_ANSWER_PLAN_ENTITY_DEAD, StringDate(ltime), StringTime(ltime), msg.station, msg.con_id, msg.dest);
			//snprintf(log.inform, sizeof(log.inform)-1, SEND_STPLAN_ANSWER_PLAN_ENTITY_DEAD, StringDate(ltime), StringTime(ltime), msg.station, msg.con_id, msg.dest);
		}
		else if (MsgStPlanStatusAnswer::PLAN_CONFIG_WRONG == msg.recv_status)
		{
			logstr.Format( SEND_STPLAN_ANSWER_PLAN_CONFIG_WRONG, StringDate(ltime), StringTime(ltime), msg.station, msg.con_id, msg.dest);
		//	snprintf(log.inform, sizeof(log.inform)-1, SEND_STPLAN_ANSWER_PLAN_CONFIG_WRONG, StringDate(ltime), StringTime(ltime), msg.station, msg.con_id, msg.dest);
		}
		else 
		{
			logstr.Format( SEND_STPLAN_ANSWER_ERR, StringDate(ltime), StringTime(ltime), msg.station, msg.con_id, msg.dest);
			//snprintf(log.inform, sizeof(log.inform)-1, SEND_STPLAN_ANSWER_ERR, StringDate(ltime), StringTime(ltime), msg.station, msg.con_id, msg.dest);			
		}

		SaveLogMessageStr("stplan",ltime,logstr);
		
	}
}
void cStPlanSendingLog::LogMsg(const MsgStPlanTrainRecv & msg)
{
	TIME ltime = time(NULL);
	if(GetTGFunctionConfig()->IsLocalLogSendStPlan())
	{
		CString logstr;logstr.Format( SEND_STPLAN_RECV,
			StringDate(ltime), StringTime(ltime), msg.station, msg.con_id);

		SaveLogMessageStr("stplan",ltime,logstr);
		
	}
}
void cStPlanSendingLog::LogMsg(const MsgStPlanRecvALL & msg)
{
	TIME ltime = time(NULL);
	CString logstr;
	if(GetTGFunctionConfig()->IsLocalLogSendStPlan())
	{
		if (MsgStPlanRecvALL::RECV == msg.status)
		{
			logstr.Format( SEND_STPLAN_RECV_ALL,
				StringDate(ltime), StringTime(ltime), msg.station, msg.con_id);

		}
		else if (MsgStPlanRecvALL::SIGN == msg.status)
		{
			logstr.Format( SEND_STPLAN_SIGN_ALL,
				StringDate(ltime), StringTime(ltime), msg.station, msg.con_id);
		}
		else 
		{
			logstr.Format( SEND_STPLAN_RECV_WRONG,
				StringDate(ltime), StringTime(ltime), msg.station, msg.con_id);
		}

	SaveLogMessageStr("stplan",ltime,logstr);
	
	}

}

void cStPlanSendingLog::LogMsg(const MsgBaseTrainPlan & msg, ENTITY_ID recv_entity)
{
	TIME ltime = time(NULL);
	if(GetTGFunctionConfig()->IsLocalLogSendStPlan())
	{
		CString logstr, strArt, strDpt;
		for(BYTE i=0; i<msg.plan_count; i++)
		{
			strArt=BasicStringTime(msg.base_plan_record[i].arrive_time);
			strDpt=BasicStringTime(msg.base_plan_record[i].depart_time);

			logstr.Format( SEND_BASICSTPLAN_INSERT,
					msg.station, recv_entity,
					msg.base_plan_record[i].arrive_train, msg.base_plan_record[i].depart_train, 
					strArt, msg.base_plan_record[i].arrive_time, 
					strDpt, msg.base_plan_record[i].depart_time, 
					msg.base_plan_record[i].entry, msg.base_plan_record[i].side, msg.base_plan_record[i].exit, 
					msg.base_plan_record[i].prev_station, msg.base_plan_record[i].next_station);

			SaveLogMessageStr("stplan",ltime,logstr);
		}	
	}
}
