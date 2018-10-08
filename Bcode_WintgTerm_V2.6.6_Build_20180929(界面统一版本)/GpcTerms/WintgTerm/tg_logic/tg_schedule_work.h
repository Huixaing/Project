#ifndef __INCLUDE_TRAIN_SCHEDULE_WORK
#define __INCLUDE_TRAIN_SCHEDULE_WORK
class CTG_TrainScheduleWork: public CTG_TrainScheduleBase{

public:
	//////////  数据库操作接口
    BOOL  OpenDatabase(ICTCDatabase *pInterface,BOOL readonly);
	BOOL  CloseDatabase();
	BOOL  ReadDataFromDatabase(BOOL purge_first);
	BOOL  WriteDataToDatabase(BOOL nosyncd_only);
	//////////  服务器操作接口
	BOOL  ProcessServerMessage();
	//BOOL  RequestDataFromServer(BOOL purge_first);
	BOOL  ProcessOldCTCMessage();
    
	//////////  计划发送操作接口
	BOOL  SetScheduleReceiverRange(int index,TIME from,TIME to);
	///////////////////////////////////////////////
};

#endif