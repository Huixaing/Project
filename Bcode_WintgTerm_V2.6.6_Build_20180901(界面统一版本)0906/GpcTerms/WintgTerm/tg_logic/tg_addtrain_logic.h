#pragma once
#include "tg_schedule_base.h"
#include "tg_basicpara.h"
#include "tg_autoadjust.h"
#include "tg_function_config.h"
enum SOURCE
{
	FREE_DRAW=1,
	PASSWAY=2,
	BASIC=3,
	SHIFT=4
};

class CTGAddTrainLogic{

	CTG_TrainScheduleBase *m_pBasicSchedule;
	CTGBasicParam         *m_pBasicPara;
	CTGAutoAdjustLogic    *m_pAutoAdjust;
	const CTGFunctionConfig     *m_pConfig;
	const CTG_ScheduleInfo      *m_pScheduleInfo;
private:
	CTGAddTrainLogic();
	CTGAddTrainLogic(const CTGAddTrainLogic& rhs);
	CTGAutoAdjustLogic& operator=(const CTGAddTrainLogic& rhs);
public:
	enum  {ADDTRAIN_BASIC_SCHEDULE_MAX_BEFORE_TIME=3*3600,ADDTRAIN_BASIC_SCHEDULE_MAX_TIME_RANGE=24*3600};
	CTGAddTrainLogic(CTG_TrainScheduleBase *pBS,CTGBasicParam *pParam,CTGAutoAdjustLogic  *pAdjust,const CTGFunctionConfig  *pConfig,const CTG_ScheduleInfo  *pScheInfo)
	{
		m_pBasicSchedule=pBS;
		m_pBasicPara=pParam;
		m_pAutoAdjust=pAdjust;
		m_pScheduleInfo=pScheInfo;
		m_pConfig=pConfig;
	}

	BOOL  GenTrain_FreeDraw(CTG_SingleTrain *ptrain,BOOL qualify_runtime,BYTE run_type); // train中是界面生成的初始数据
	BOOL  GenTrainBasic_OPMS(CTG_SingleTrain *ptrain);
	BOOL  GenTrain_OPMS(CTG_SingleTrain *ptrain,BOOL newflag);
	BOOL  SplitTrain(CTG_SingleTrain *ptrain,CTG_SingleTrain *panother_train,int rec_index,BYTE type,int first_entry,int first_exit,int second_entry,int second_exit);
	BOOL  GenTrain_Passway(CTG_SingleTrain *ptrain,const TRAIN_NO train_no, int con_id,USHORT PasswayId, BYTE start_flag, BYTE end_flag, STATION_NO station, TIME atime, TIME dtime,BYTE run_type);
	BOOL  GenTrainTmp_Passway(CTG_SingleTrain *ptrain,const CString train_no, int con_id,USHORT PasswayId, BYTE start_flag, BYTE end_flag,  BYTE start_index, BYTE end_index, TIME atime, TIME dtime,BYTE run_type);
	
	BOOL  GenTrain_BasicSchedule(CTG_SingleTrain *ptrain,const CTG_SingleTrain &srctrain, STATION_NO station, TIME atime, TIME dtime, const TRAIN_NO spec_train, const TRAIN_NO spec_original_train);
	BOOL  GenTrain_ShiftSchedule(CTG_SingleTrain *ptrain);
	BOOL  SetDefaultAttribForNewRecords(CTG_SingleTrain *ptrain,int fromindex);// 继续画线的默认设置

	BOOL  RecacuTrainTimeForward(CTG_SingleTrain *ptrain, int index);
	BOOL  RecacuTrainTime(CTG_SingleTrain *ptrain,BOOL only_first_time); // 按照发车时间重新计算各站时分,only_first_time对货车仅考虑第一个记录
	BOOL  RecacuTrainTimeBackward(CTG_SingleTrain *ptrain, int index);

	BOOL  GenTrain_ChangeRoute(CTG_SingleTrain &srctrain,int con_id,int route_id);
private:

	const CTG_SingleTrain *GetBasicScheduleTrain(const char *trainid);
	const CTG_SingleTrain *GetBasicScheduleTrain(TRAIN_INDEX train_index);
	const CTG_SingleTrain *GetLongIncreaseTrain(const char *trainid);

	BOOL  supply_schedule_current_date(CTG_SingleTrain *ptrain,STATION_NO spec_sta, TIME spec_atime, TIME spec_dtime,long *time_off_set_ptr);
//////// PASSWAY //////////////////
	BOOL  SetTrainRecordListFromPartPassway(CTG_SingleTrain *ptrain, int start_idx, int end_idx, const CSingle_Passway &passway_info);
	BOOL  SetTrainRecordListFromPassway(CTG_SingleTrain *ptrain,const CSingle_Passway &passway_info);
	BOOL  SetTrainRecordListTime(CTG_SingleTrain *ptrain,STATION_NO station, TIME atime, TIME dtime);
	
	BOOL  SetTrainRecordListFlag(CTG_SingleTrain *ptrain,BYTE start_flag, BYTE end_flag);
	BOOL  SetTrainInfoByTrainID(CTG_SingleTrain *ptrain,const TRAIN_NO train_no);
	BOOL  AdjustTrainRecordSide(CTG_SingleTrain *ptrain,int begin=-1, int end=-1, bool keep_old=false, bool bShiftSchdule=false);


///////////////////////////////
	int   CalculateTrainRunTime(CTG_SingleTrain *ptrain,int rec_index,TRAIN_IN_OUT in_out);
	int   CalculateTrainStopExtra(CTG_SingleTrain *ptrain,int rec_index,TRAIN_IN_OUT in_out);
	int   CalculateTrainStartExtra(CTG_SingleTrain *ptrain,int rec_index,TRAIN_IN_OUT in_out);

	
///////////////////////////////
	BOOL  SetTrainInitFlag(CTG_SingleTrain *ptrain);  
	BOOL  SetTrainFlag(CTG_SingleTrain *ptrain);  
	BOOL  IsBasicScheduleTrain(CTG_SingleTrain *ptrain);
	
	////////
	BOOL  SetDefaultAttrib(CTG_SingleTrain *ptrain);// 新增车默认属性设置
	void  SetDefualtEntryAndExitByGpcdef(CTG_SingleTrain *ptrain, bool bOnlyEntry=false); //主要针对接入和交出
	void  SetDefualtEntryAndExit(CTG_SingleTrain *ptrain); //主要针对接入和交出
	void  SetDefualtEntryAndExitBySide(CTG_SingleTrain *ptrain); // 处理徐州东--徐州东车场AB线问题
	void  UseTDMSEntryExitByConfig(CTG_SingleTrain* ptrain, int section); // 根据配置,决定哪个车站间使用TDMS线别

	// 从rec_index向后获取默认股道设置
	BOOL  SetDefaultByBasicSchedule(CTG_SingleTrain *ptrain, BOOL bUseOPMSSide, int rec_index, SOURCE source);
	void  SetNotTechStop(CTG_SingleTrain *ptrain, int rec_index, SOURCE source);
	void  SetSideByBasicSchedule(CTG_SingleTrain *ptrain, const CTG_SingleTrain *pbasic_train, int rec_index);
	void  SetNotTechStopByBasicSchedule(CTG_SingleTrain *ptrain, const CTG_SingleTrain *pbasic_train, int rec_index, SOURCE source);
	
public:
	// 自动变更车次号
	static bool AutoChangeTrainID(CTG_SingleTrain *ptrain, int conid, int reverse=0);
	// 更改电力、内燃属性
	static bool ChangeTrainElec(CTG_SingleTrain *ptrain, int conid);
	// 旅客列车办客标志自动锁定股道
	void  SetSideLockPassengerTrainNotTechStop(CTG_SingleTrain *ptrain);
	void  PassengerTrainEndSetNotTechStop(CTG_SingleTrain *ptrain);
	void  PassengerTrainStartSetNotTechStop(CTG_SingleTrain *ptrain);
	BOOL  SetValidSideByBasicSchedule(CTG_SingleTrain *ptrain);
	BOOL  SetValidSideByCacu(CTG_SingleTrain *ptrain);
};


