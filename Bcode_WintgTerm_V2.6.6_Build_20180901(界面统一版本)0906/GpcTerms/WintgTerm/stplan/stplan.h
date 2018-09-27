#ifndef _DEF_STPLAN_
#define _DEF_STPLAN_

/*  send schedule function implement class  */
#include <baseMsg\msgid.h>
#include <baseMsg\clbth.h>
#include <baseMsg\msg_schd.h>
#include "tg_schedule_base.h"
#include "stplanstatus.h"
#include "stplanlog.h"
//#include "macroswitch.h"
#include ".\predict\TrainDepartPredictHolder.h"

typedef Pstruct _tagStationPlan {
	int nStation;// entry no
 	long usStartDate;
	long usStartTime;
	long usEndDate;
	long usEndTime;
	long usSendDate;
	long usSendTime;
	bool bSignInFlag;
	short bReceiveFlag;
	char strSignName[16];
	time_t ussignTime;
} STATIONPLAN;
//new defined 
typedef Pstruct _tagStationPlanNew 
{
	time_t send_time;
	time_t begin_time;
	time_t end_time;
	time_t recv_time;
	int nStation;
	int nEntityIndex;
	DWORD Entityid;
	char  EntityName[32];
	//bool bSignInFlag;
	short bReceiveFlag;
	char strSignName[20];
	char sSendName[20];
	
}STATIONPLAN_NEW;

typedef Pstruct _tagStationPlanInfo
{
    int nStationNum;
    Pstruct _tagStationPlan m_StationPlan[MAX_STATIONS_PER_SECTION];
}STATIONPLANINFO;

//new defined
typedef Pstruct _tagStationPlanInfoNew{
    int nEntityNum;
    Pstruct _tagStationPlanNew m_StationPlan[MAX_STATIONS_PER_SECTION];
}STATIONPLANINFONEW;
class CTgDocument;

struct RetInfo
{
	int entity;
	int station;
	int rec;
	int sign;
	RetInfo()
	{
		entity=0;
		station=0;
		rec=0;
		sign =0;
	}
};

struct Train_Relation // 20150623 珠三角城际,该结构保存同车组列车索引,来之车组关系文件
{
	char cCzh[32];
	int  nTrainCount;
	long nTrainIndex[64];
	Train_Relation()
	{
		memset(cCzh,0,sizeof(cCzh));
		nTrainCount=0;
		memset(nTrainIndex,0,sizeof(nTrainIndex));
	}
};

class CWintgViewMMI;

class cStPlan 
{
	CTG_TrainScheduleBase    *m_pWorkSchedule;
	CTG_TrainScheduleBase    *m_pBasicSchedule;
	CTGBasicParam            *m_pBasicPara;
	CTgDocument              *m_pDoc;

	cSendStPlanStatus*       m_pSendStPlanStatus;

	cStPlanSendingLog*       m_pSendLog;
	HWND                     m_hPlanMMIWindow;
	
	short get_current_con_id();
	TIME  GetAdjustStartTime();
	short proc_msg(ENTITY_ID entity, const MsgStPlanTrainRecv &msg);
	short proc_msg(ENTITY_ID entity, const MsgStPlanRecvALL &msg);
	short proc_msg(ENTITY_ID entity, const MsgRouteStatusInform &msg);
	short get_meet_store_pos( MsgStPlanInsert &msg, const CTG_TrainRecord *prec, TRAIN_MEET_RELATION flag, int *need_plus_count);
	BOOL  SetMeetRelationInfo(const CTG_SingleTrain &desttrain, MsgStPlanInsert &insertmsg, const CTG_TrainRecord& rec, CTG_TrainPtrGroup &trains_in_range);
	
	short OrganizeMsg(MsgStPlanInsert &msg, STATION_NO recv_sta, bool bEnable3Function, const CTG_SingleTrain *singletrain,BYTE rec_index,  short prev_count, short next_count, CTG_TrainPtrGroup &trains_in_range, CString &report_early, DWORD dest);
//////////// 以下是WINTGDOC中移植
	STATIONPLANINFONEW m_StationPlanInfoNew;//new defined
	BOOL  ProcessPlanAnswer(const MsgStPlanStatusAnswer *pBuf);
	BOOL  InitStationPlanReceiveInfo(int section_id);
public:
	void ClearPlanFlag();
	void SetPlanFlag(DWORD entity, int station, int index);
	cStPlan(CTgDocument *pdoc,CTGBasicParam *pParam);
	~cStPlan();
	BOOL RequestPlanStatus(STATION_NO plan_sta,ENTITY_ID eid, MsgStPlanStatusAnswer &answer);
	BOOL SendPlan(const MsgStPlanSendOrder &order, bool bSecretSendToSta, std::vector<stplannotice>& predict_notice, CString &report_early, bool bEnable3Function, int match);
	BOOL ProcessMessage(long eid,  BaseMsg *pmsg);
	void SetPlanMMIWindow(HWND h) {m_hPlanMMIWindow=h;}
	STATIONPLANINFONEW *GetPlanReceiveInfo() {return &m_StationPlanInfoNew;}
    static BOOL GlobalProcessMessage(long eid, BaseMsg *pmsg);	

public:
	void  OutprintCZHRelation();
	void  LoadTrainRelation();

private:
	bool  SetTrigeRouteByPlan(int con_id, CTG_SingleTrain *desttrain, int rec_index, CTG_TrainPtrGroup &trains_in_range, TIME begin_time);
	long  GetNextTrainDepart(CString czh, int station, long curr_train_index,CString& trainid);
	long  GetBeforeTrainArrive(CString czh, int station, long curr_train_index,long&side_utility);

	bool  GetStartTrainDepartNo(int station, long curr_train_index, TRAIN_NO trainid);
public:
	void  SetTgMMI(CWintgViewMMI *pmmi) {m_pTgMMI=pmmi;}
	BOOL SendBasicPlan(const MsgStPlanSendOrder &order, bool bSecretSendToSta, int match);
	void SendBasicPlan(STATION_NO station, int con_id, ENTITY_ID sender, ENTITY_ID dest);
	void SendBasicPlan(CTG_TrainPtrGroup& result, STATION_NO plan_sta, STATION_NO recv_sta, ENTITY_ID dest, CString strArTid, CString strDpTid);
private:
	CWintgViewMMI *m_pTgMMI;
	std::vector<Train_Relation> m_relation;
private:
	int m_nbPlanCount;
	MsgBaseTrainPlan m_bplan;
	RetInfo gRetInfo[256];
};

#endif /** THE END **/

