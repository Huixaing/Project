#ifndef _DEF_STPLAN_STATUS_
#define _DEF_STPLAN_STATUS_
/* Save schedule send status */

class cSendStPlanStatus 
{
public:
	enum STATUS 
	{
		PLAN_RECV,
		PLAN_SIGN,
		PLAN_SENDING,
		PLAN_NO_CHANGE,
		PLAN_NO_DATA,
		PLAN_ENTITY_DEAD,
		PLAN_CONFIG_WRONG,
		PLAN_INIT
	};
private:
	struct SendStPlanStatus 
	{
		ENTITY_ID   dest;
		STATUS      status;
		TIME        btime;
		TIME        etime;
		TIME		sendtime;
		TIME		recvtime;
		USERNAME	sender;
		USERNAME    recver;
		STATION_NO	plan_sta;
		STATION_NO	recv_sta;
	};

	cSendStPlanStatus();

	const long	      m_Count;
	SendStPlanStatus *m_pStatus;

	MsgStPlanStatusAnswer::PLAN_RECV_STATUS convert_status(STATUS s);

public:
	cSendStPlanStatus(long max_count);
	~cSendStPlanStatus();

	short SetSendStPlanStatus(STATION_NO plan_sta, STATION_NO recv_sta, ENTITY_ID e, STATUS s, TIME btime, TIME etime, TIME sendtime, const USERNAME sender);
	short SetSendStPlanStatus(STATION_NO recv_sta, ENTITY_ID e, STATUS s, TIME sendtime, TIME recvtime, const USERNAME recver);

	short QuerySendStPlanStatus(STATION_NO plan_sta, STATION_NO recv_sta, ENTITY_ID e, MsgStPlanStatusAnswer::PLAN_RECV_STATUS *s, TIME *btime, TIME *etime, TIME *sendtime, TIME *recvtime, USERNAME sender, USERNAME recver);
};


#endif /** THE END **/
