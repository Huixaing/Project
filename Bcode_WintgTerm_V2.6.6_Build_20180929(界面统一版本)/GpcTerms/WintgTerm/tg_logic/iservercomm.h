#ifndef __INCLUDE__SCHD_SERVERCOMM__
#define __INCLUDE__SCHD_SERVERCOMM__
clas  IScheduleServerComm{
public:
	BOOL  SendUpdate(const CTG_TrainInfo &train_info)=0;
	BOOL  SendUpdate(const CTG_TrainRecordList &train_record_list)=0;
	BOOL  SendUpdate(const CTG_TrainEngineList &train_engine_list)=0;
	BOOL  SendUpdate(const CTG_SingleTrain &train)=0;
	
	BOOL  SendUpdate(const CTG_SingleMark &mark)=0;

	BOOL  SendDeleteTrain(TRAIN_INDEX train_index)=0;
	BOOL  SendDeleteMark(TRAIN_INDEX mark_index)=0;
	
};
#endif