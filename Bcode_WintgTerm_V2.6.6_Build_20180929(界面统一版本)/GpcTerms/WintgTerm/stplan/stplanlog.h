#ifndef _DEF_STPLAN_LOG_
#define _DEF_STPLAN_LOG_
/**/



// Record sended and recved message of stplan
class cStPlanSendingLog
{
	
public:
	cStPlanSendingLog();
	~cStPlanSendingLog();

	void LogMsg(const MsgStPlanSendOrder & msg, ENTITY_ID sender_entity);
	void LogMsg(const MsgStPlanRequest & msg, ENTITY_ID request_entity);

	void LogMsg(const MsgStPlanSendInit & msg, ENTITY_ID recv_entity);
	void LogMsg(const MsgStPlanInsert & msg, ENTITY_ID recv_entity);
	void LogMsg(const MsgStPlanDelete & msg, ENTITY_ID recv_entity);
	void LogMsg(const MsgStPlanSendComplete & msg, ENTITY_ID recv_entity);
	void LogMsg(const MsgRouteStatusInform &msg, ENTITY_ID entity);

	void LogMsg(const MsgStPlanStatusAnswer & msg);

	void LogMsg(const MsgStPlanTrainRecv & msg);
	void LogMsg(const MsgStPlanRecvALL & msg);

	void LogMsg(const MsgBaseTrainPlan & msg, ENTITY_ID recv_entity);
};

#endif /** THE END **/
