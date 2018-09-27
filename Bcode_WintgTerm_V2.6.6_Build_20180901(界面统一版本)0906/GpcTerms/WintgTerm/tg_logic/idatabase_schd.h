
#define IDSCHD_RC_OK        1
#define IDSCHD_RC_NO_DATA   0
#define IDSCHD_RC_ERROR     -1

class IScheduleDatabase{
   public:
 
    int        IDSCHD_SetDatabasePara(int flag)=0;

////////////////////////////////  READ & QUERY
    //  循环读取
	int        IDSCHD_LoadWorkSchedule(int con_id,int shift_id,long tmStart,long tmEnd)=0;
	int        IDSCHD_ReadWorkScheduleSingleTrain(CTG_SingleTrain &train)=0;
	int        IDSCHD_ReadWorkScheduleMark(CTG_BlockInfo &blockinfo)=0;

	// 按INDEX单独读取
	int        IDSCHD_ReadWorkScheduleSingleTrainByIndex(int con_id,unsigned long train_index,CTG_SingleTrain &train)=0;
	int        IDSCHD_ReadWorkScheduleMarkByIndex(int con_id,unsigned long train_index,CTG_BlockInfo &blockinfo)=0;

////////////////////////////////  WRITE & UPDATE
	int  IDSCHD_DeleteTrains(int con_id,int shift_id,long tmStart,long tmEnd)=0;
	int  IDSCHD_DeleteMarks(int con_id,int shift_id,long tmStart,long tmEnd)=0;
	int  IDSCHD_DeleteSingleTrain(int con_id,long train_index)=0;
	int  IDSCHD_InsertSingleTrain(int con_id,const CTG_SingleTrain &train)=0;
	int  IDSCHD_UpdateTrainInfo(int con_id,const CTG_SingleTrain &train)=0; // 仅仅更新TRAIN_INFO
	int  IDSCHD_UpdateTrainRecord(int con_id,const CTG_SingleTrain &train)=0; // 仅仅更新TRAIN_RECORD
	int  IDSCHD_DeleteMark(int con_id,unsigned long mark_index)=0;
	int  IDSCHD_InsertMark(int con_id,const CTG_BlockInfo &Item)=0;	
	int  IDSCHD_UpdateMark(int con_id,const CTG_BlockInfo &Item)=0;

};