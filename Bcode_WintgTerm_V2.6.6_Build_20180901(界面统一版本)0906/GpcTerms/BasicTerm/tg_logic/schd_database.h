#ifndef __INCLUDE__DATABASE_COMM_INTERFACE__
#define __INCLUDE__DATABASE_COMM_INTERFACE__
#include "ischedule_comm.h"
class CWorkScheduleDataServer;
class  CSchduleDatabaseInterface:public IScheduleComm{
  public:
	///////// IScheduleComm 接口 实现
	//////////////////////////// 输入 ////////////////////////////
	///////// UPDATE
	virtual BOOL   UpdateTrain(const CTG_ScheduleInfo & schd,const CTG_SingleTrain  &train,int source_flag);
	virtual BOOL   UpdateTrainInfo(const CTG_ScheduleInfo &schd,const CTG_SingleTrain  &train,int source_flag);
	virtual BOOL   UpdateTrainRecords(const CTG_ScheduleInfo &schd,const CTG_SingleTrain  &train,int source_flag);
	virtual BOOL   UpdateEngineInfo(const CTG_ScheduleInfo &schd,const CTG_SingleTrain  &train,int source_flag);
	virtual BOOL   UpdateMark(const CTG_ScheduleInfo &schd,const CTG_SingleMark  &block_info,int source_flag);

	///////// DELETE
	virtual BOOL   DeleteTrain(const CTG_ScheduleInfo &schd,const CTG_SingleTrain  &train,int source_flag);
	virtual BOOL   DeleteMark(const CTG_ScheduleInfo &schd,const CTG_SingleMark  &train,int source_flag);
	virtual BOOL   DeleteAllData(const CTG_ScheduleInfo &schd,int source_flag);
	///////// APPEND
	virtual TRAIN_INDEX   AppendTrain(const CTG_ScheduleInfo &schd,const CTG_SingleTrain  &train,int source_flag);
	virtual TRAIN_INDEX   AppendMark(const CTG_ScheduleInfo &schd,const CTG_SingleMark  &mark,int source_flag);

	//////  TrainIndex based function
	virtual BOOL   UpdateTrainRecords(const CTG_ScheduleInfo &schd,TRAIN_INDEX index,const CTG_TrainRecordList &trainrecord,int source_flag); //NULL means clear!
	virtual BOOL   UpdateEngineInfo(const CTG_ScheduleInfo &schd,TRAIN_INDEX index,const CTG_TrainEngineList &trainengine,int source_flag); //NULL means clear!
	virtual BOOL   UpdateTrainInfo(const CTG_ScheduleInfo &schd,TRAIN_INDEX index,const CTG_TrainInfo  &train,int source_flag);
	virtual BOOL   DeleteTrain(const CTG_ScheduleInfo &schd,TRAIN_INDEX index,int source_flag);
	virtual BOOL   DeleteMark(const CTG_ScheduleInfo &schd,TRAIN_INDEX index,int source_flag);
	virtual BOOL   UpdateMark(const CTG_ScheduleInfo &schd,TRAIN_INDEX block_index,const CTG_SingleMark  &block_info,int source_flag);
	
	//////////////////////////// 输出 ////////////////////////////
	
	////////  COPY
	virtual BOOL   CopyAllData(const CTG_ScheduleInfo &schd,IScheduleComm *pOut,BOOL all_flag);
	virtual void   PollingAndCheckStatus();

private:
	CWorkScheduleDataServer  *m_pDatabaseServer;
	BOOL  CopyBasicScheduleData(const CTG_ScheduleInfo &schd,IScheduleComm *pOut);
	CTG_TrainScheduleBase *GetDataStore(const CTG_ScheduleInfo &source_info);
	
public:
	////////////////////////////////
	CSchduleDatabaseInterface()
	{
		m_pDatabaseServer=NULL;
	}
	BOOL   InitDatabase(const char *db_name,int db_type);
};
#endif