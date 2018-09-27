#pragma once

#include "tg_structdef.h"
#include "tg_schedule_base.h"
#include "tg_basicpara.h"
#include "tg_function_config.h"

class CTGAutoAdjustLogic
{
private:
	CTGAutoAdjustLogic();
	CTGAutoAdjustLogic(const CTGAutoAdjustLogic& rhs);
	CTGAutoAdjustLogic& operator=(const CTGAutoAdjustLogic& rhs);

public:
	// 南昌局，客车日班计划图办客只能使用第一默认股道
	BOOL    OnlyUseFirstStopSideForNanchang(CTG_SingleTrain *ptrain, int rec_index);
	BOOL    AdjustForStopPassChange(int con_id, CTG_SingleTrain *pnewtrain,int index,BOOL stop_to_pass);
	BOOL	ChangeSideForStopOrPass(CTG_SingleTrain *ptrain,int rec_index,int enable_passenger_side_change=-1,int only_pass_main_side=-1, CTG_ScheduleInfo::SCHEDULE_TYPE type = CTG_ScheduleInfo::SCHEDULE_TYPE_NULL); // enable_passenger_side_change 是否允许调整客车股道, only_pass_main_side: 0允许侧线通过,1:不允许侧线通过,-1用配置文件中的默认值
	
	BOOL    AdjustTrainForManualUpdate(CTG_SingleTrain *ptrain,const CMMIOperFlag &oper_flag,int con_id,CWnd* pParentWnd); 

///////////////逻辑检查
	void  CheckAndSetAlarmInfo(CTG_SingleTrain *single);
	void  CheckAndSetAlarmInfo(CTG_SingleTrain *single, long start_time, long end_time, CTG_TrainPtrGroup& group, CTG_ObjectPtrGroup& markgroup);
	BOOL  CheckStplan(CTG_SingleTrain *single, long start_time, long end_time, CTG_TrainPtrGroup& group, CTG_ObjectPtrGroup& markgroup, int rec_index);

	BOOL  CheckTrainSideInfo(CTG_SingleTrain *single, int rec_index,	unsigned long &alarm_flag,CString &strText);
	void  CheckTrainSideInfoForStplan(CTG_SingleTrain *single, int rec_index, bool bTdcsCheck=false);

	BOOL  CheckTrainABSMeetInfo(CTG_SingleTrain *single, int rec_index, ULONG &alarm_flag, CString &strText);
	BOOL  CheckTrainABSMeetInfo(CTG_SingleTrain *single, int rec_index, ULONG &alarm_flag, CString &strText, long start_time, long end_time, CTG_TrainPtrGroup& group);
	void  CheckTrainABSMeetInfoForStplan(CTG_SingleTrain *single, int rec_index, ULONG &alarm_flag, CString &strText, long start_time, long end_time, CTG_TrainPtrGroup& group);
	void  CheckTrainSideCrashForStplan(CTG_SingleTrain *single, int rec_index, long start_time, long end_time, CTG_TrainPtrGroup& group);

	BOOL  CheckTrainStaMeetInfo(CTG_SingleTrain *single,int rec_index, long start_time, long end_time, CTG_TrainPtrGroup&  trains_in_range, unsigned long &alarm_flag,CString &strText);
	//added by zhaoliang for side check
	BOOL  CheckTrainStaSideInfo(CTG_SingleTrain *single,int rec_index, long start_time, long end_time, CTG_TrainPtrGroup&  trains_in_range, unsigned long &alarm_flag,CString &strText);
	
	BOOL  CheckTrain_ABSBlock(CTG_SingleTrain *single,int rec_index, unsigned long &alarm_flag,CString &strText,TIME &etime);
	BOOL  CheckTrain_ABSBlock(CTG_SingleTrain *single,int rec_index, unsigned long &alarm_flag,CString &strText,TIME &etime, CTG_ObjectPtrGroup& markgroup);
	
	const char *CheckTrainManualUpdateOper(CTG_SingleTrain *pnewtrain,const CMMIOperFlag &oper_flag,DWORD& flag);// return NULL ok,or ERROR info
	
	void  SetTrainPassNonMainSide(CTG_SingleTrain *ptrain);

	
	
	BOOL  CheckTrain_SideBlock(STATION_NO  station,SIDE_ID side_id,TIME &etime, TIME &stime);
	BOOL  CheckTrainPassAndStop(CTG_SingleTrain *single,int rec_index, unsigned long &alarm_flag,CString &strText);
	BOOL  AdjustForStopPassChange(CTG_SingleTrain *pnewtrain,int index,BOOL stop_to_pass);
	void  CacuTrainSlowTime(CTG_SingleTrain *single);
	
	BOOL  CacuDefaultSide(CTG_SingleTrain *ptrain,int rec_index);  // 2010.2.26: 只根据站细推算股道，不考虑其他车次影响
	BOOL  CacuDefaultSide(CTG_SingleTrain *ptrain);// 2010.2.26: 只根据站细推算股道，不考虑其他车次影响
// CUIHU 2010.11.1 自动调整相关
	BOOL  AdjustTrainForCollision(CTG_SingleTrain *ptrain,int rec_index); //调整以避开冲突情况
	BOOL  CacuAvailableSideForPlanStop(CTG_SingleTrain *ptrain, int recIndex,TIME &arriveTime,int &avaSide,BOOL forceSide);
	BOOL  AdjustTrainForMutiQuickPass(CTG_SingleTrain *ptrain,int rec_index, int conid); //调整以通过,除非有最小停时分
	BOOL  GetRelaTrainsOfMark(const CTG_SingleMark *pmark,CTG_ObjectPtrGroup &rela_trains); //查找与mark关联的车次
	BOOL  AdjustSingleTrainForMark(CTG_SingleTrain &train,int recIndex,const CTG_SingleMark *pmark);
	// 检查在指定时间的停站股道，arriveTime初始值是预计到达时刻，avaSide初始值是默认股道
	// 如果 forceSide 为TRUE,则arriveTime返回 avaSide可用的时刻
	// 如果 forceSide 为FALSE,则avaSide返回最早可用的股道，arriveTime返回可用的时刻
	// 函数返回 TRUE, 表明上述返回值有效，否则无效

public:
	void   SetTrainFJK(CTG_SingleTrain *ptrain);
	void   GetScheduleInfo(CTG_ScheduleInfo& schd);

private:
	CTG_TrainScheduleBase *m_pBasicSchedule;
	CTG_TrainScheduleBase *m_pWorkSchedule;
	CTGBasicParam         *m_pBasicPara;
	const CTGFunctionConfig     *m_pConfig;
	int   m_nAdjustSchdType;  // 被调整图类型
	BOOL  TrainHasActualTimeRecord(const CTG_SingleTrain *pnewtrain,int index=0);
	BOOL  CheckBlockInfo_SideAvailable(STATION_NO  station,SIDE_ID side_id,TIME &etime);
	TIME  CacuTrainDepartTime(const CTG_SingleTrain *ptrain,int rec_index, TIME ltime=NULL_TIME);

	BOOL  __AdjustTrainForCollision__(CTG_SingleTrain *ptrain,int rec_index); //调整以避开冲突情况,被调用的底层
	BOOL  __AdjustTrainDepartTimeByTrainCollision(CTG_SingleTrain *ptrain,int rec_index,int abs_run_time,CTG_SingleTrain *&initRelaTrain,TIME  &cau_depart,TIME &cau_next_arrive);

	BOOL  GetAbsMarkStation(const CTG_SingleMark *pmark,int &station1,int &station2);
public:
	CTGAutoAdjustLogic(const CTGFunctionConfig *pConfig,CTGBasicParam *pparam,CTG_TrainScheduleBase *pbasic_schd,CTG_TrainScheduleBase *pwork_schd);
	~CTGAutoAdjustLogic(void);
};
