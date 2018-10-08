#include <baseMsg\msgid.h>
#include <baseMsg\clbth.h>
#include <baseMsg\msg_schd.h>
#include "tg_schedule_base.h"
#include "stplanstatus.h"

cSendStPlanStatus::cSendStPlanStatus(long max_count)
:m_Count(max_count>0?max_count:8)
{
	m_pStatus=new SendStPlanStatus[m_Count];
	for(long i=0; i<m_Count; i++) 
	{
		memset(&m_pStatus[i], 0x00, sizeof(SendStPlanStatus));
		m_pStatus[i].status = PLAN_INIT;
	}
}
cSendStPlanStatus::~cSendStPlanStatus()
{
	delete[] m_pStatus;
}
MsgStPlanStatusAnswer::PLAN_RECV_STATUS cSendStPlanStatus::convert_status(STATUS s)
{
	switch (s){
	case PLAN_RECV:
		return MsgStPlanStatusAnswer::PLAN_RECV;
	case PLAN_SIGN:
		return MsgStPlanStatusAnswer::PLAN_SIGN;
	case PLAN_SENDING:
		return MsgStPlanStatusAnswer::PLAN_SENDING;
	case PLAN_NO_CHANGE:
		return MsgStPlanStatusAnswer::PLAN_NO_CHANGE;
	case PLAN_NO_DATA:
		return MsgStPlanStatusAnswer::PLAN_NO_DATA;
	case PLAN_ENTITY_DEAD:
		return MsgStPlanStatusAnswer::PLAN_ENTITY_DEAD;
	case PLAN_CONFIG_WRONG:
		return MsgStPlanStatusAnswer::PLAN_CONFIG_WRONG;
	case PLAN_INIT:
		return MsgStPlanStatusAnswer::PLAN_INIT;
	}

	return MsgStPlanStatusAnswer::PLAN_INIT;
}

#define GetStatusName(s)                                  \
	PLAN_RECV == (int)(s) ? "recv plan" :                 \
	PLAN_SIGN == (int)(s) ? "sign plan" :                 \
	PLAN_SENDING == (int)(s) ? "sending plan" :           \
	PLAN_NO_CHANGE == (int)(s) ? "change plan" :          \
	PLAN_NO_DATA == (int)(s) ? "no plan" :                \
	PLAN_ENTITY_DEAD == (int)(s) ? "entity dead" :        \
	PLAN_CONFIG_WRONG == (int)(s) ? "wrong config" :      \
	PLAN_INIT == (int)(s) ? "init" : "unknown"

short cSendStPlanStatus::SetSendStPlanStatus(STATION_NO plan_sta, STATION_NO recv_sta, ENTITY_ID e, STATUS s, TIME btime, TIME etime, TIME sendtime, const USERNAME sender)
{
	long index=-1;
	for(long i=0; i<m_Count; i++) 
	{
		if (m_pStatus[i].plan_sta == plan_sta && m_pStatus[i].recv_sta == recv_sta && m_pStatus[i].dest == e)
		{
			index = i;
			break;
		}
		if (m_pStatus[i].status == PLAN_INIT)
			index = i;
	}
	if (index == -1) 
	{
//		m_Task.ErrLog("SetSendStPlanStatus overflow max %d", m_Count);
		return TG_FAILED;
	}

	m_pStatus[index].plan_sta = plan_sta;
	m_pStatus[index].recv_sta = recv_sta;
	m_pStatus[index].dest = e;
	m_pStatus[index].status = s;
	m_pStatus[index].btime  = btime;
	m_pStatus[index].etime  = etime;
	m_pStatus[index].sendtime = sendtime;
	m_pStatus[index].recvtime = NULL_TIME;
	memcpy(m_pStatus[index].sender, sender, sizeof(USERNAME));
	memset(m_pStatus[index].recver, 0x00, sizeof(USERNAME));

	//DebugLog("SetSendStPlanStatus sta %d %d e%d status %d %s", plan_sta, recv_sta, e, s, GetStatusName(s));
	return TG_SUCCESS;
}
short cSendStPlanStatus::SetSendStPlanStatus(STATION_NO recv_sta, ENTITY_ID e, STATUS s, TIME sendtime, TIME recvtime, const USERNAME recver)
{
	for(long i=0; i<m_Count; i++) 
	{
		if (m_pStatus[i].recv_sta == recv_sta && m_pStatus[i].dest == e && m_pStatus[i].sendtime == sendtime) 
		{
			if (s == PLAN_RECV && m_pStatus[i].status == PLAN_SENDING)
				m_pStatus[i].status = PLAN_RECV;
			
			else if (s == PLAN_SIGN && m_pStatus[i].status == PLAN_RECV)
				m_pStatus[i].status = PLAN_SIGN;
			
			else if (s == PLAN_CONFIG_WRONG)
				m_pStatus[i].status = PLAN_CONFIG_WRONG;

			else if ((s == PLAN_RECV && m_pStatus[i].status == PLAN_RECV) || (s == PLAN_SIGN && m_pStatus[i].status == PLAN_SIGN))
			{
				// if double stpc recv/sign plan, we got two echo
				return TG_SUCCESS;
			}
		
			m_pStatus[i].recvtime = recvtime;
			memcpy(m_pStatus[i].recver, recver, sizeof(USERNAME));

//			m_Task.SysLog("SetSendStPlanStatus sta %d e%d status %d %s", recv_sta, e, s, GetStatusName(s));
			return TG_SUCCESS;
		}
	}
//	m_Task.ErrLog("can not set SendStPlanStatus sta %d e%d sendtime %d", recv_sta, e, sendtime);
	return TG_FAILED;
}
short cSendStPlanStatus::QuerySendStPlanStatus(STATION_NO plan_sta, STATION_NO recv_sta, ENTITY_ID e, MsgStPlanStatusAnswer::PLAN_RECV_STATUS *s, TIME *btime, TIME *etime, TIME *sendtime, TIME *recvtime, USERNAME sender, USERNAME recver)
{
	for(long i=0; i<m_Count; i++) 
	{
		if (m_pStatus[i].dest != e) continue;

		if ((NULL_STATION != plan_sta && m_pStatus[i].plan_sta == plan_sta) 
		  ||(NULL_STATION != recv_sta && m_pStatus[i].recv_sta == recv_sta))
		{
			*s = convert_status(m_pStatus[i].status);

			*btime = m_pStatus[i].btime;
			*etime = m_pStatus[i].etime;
			*sendtime = m_pStatus[i].sendtime;
			*recvtime = m_pStatus[i].recvtime;
			memcpy(sender, m_pStatus[i].sender, sizeof(USERNAME));
			memcpy(recver, m_pStatus[i].recver, sizeof(USERNAME));

			return TG_SUCCESS;
		}
	}

	*s = convert_status(PLAN_INIT);
	*btime = NULL_TIME;
	*etime = NULL_TIME;
	*sendtime = NULL_TIME;
	*recvtime = NULL_TIME;
	memset(sender, 0x00, sizeof(USERNAME));
	memset(recver, 0x00, sizeof(USERNAME));

	return TG_SUCCESS;
}

