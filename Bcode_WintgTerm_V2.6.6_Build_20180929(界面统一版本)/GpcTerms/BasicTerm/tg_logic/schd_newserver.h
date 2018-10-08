#ifndef __INCLUDE_NEWSERVER_COMM_INTER___
#define __INCLUDE_NEWSERVER_COMM_INTER___
#include "ischedule_comm.h"

class CWintgNewServer:public IScheduleComm{
public:
	///////// IScheduleComm 接口 实现
	
	virtual BOOL   IsAsyncMode(){return TRUE;};
	//////////////////////////// 输入 ////////////////////////////
	///////// UPDATE
	virtual BOOL   UpdateTrain(const CTG_ScheduleInfo & schd,const CTG_SingleTrain  &train,int source_flag);
	virtual BOOL   UpdateMark(const CTG_ScheduleInfo &schd,const CTG_SingleMark  &block_info,int source_flag);

	///////// DELETE
	virtual BOOL   DeleteTrain(const CTG_ScheduleInfo &schd,const CTG_SingleTrain  &train,int source_flag);
	virtual BOOL   DeleteMark(const CTG_ScheduleInfo &schd,const CTG_SingleMark  &train,int source_flag);
	virtual BOOL   DeleteAllData(const CTG_ScheduleInfo &schd,int source_flag);
	///////// APPEND
	virtual TRAIN_INDEX   AppendTrain(const CTG_ScheduleInfo &schd,const CTG_SingleTrain  &train,int source_flag);
	virtual TRAIN_INDEX   AppendMark(const CTG_ScheduleInfo &schd,const CTG_SingleMark  &mark,int source_flag);

	//////  TrainIndex based function
	virtual BOOL   DeleteTrain(const CTG_ScheduleInfo &schd,TRAIN_INDEX index,int source_flag);
	virtual BOOL   DeleteMark(const CTG_ScheduleInfo &schd,TRAIN_INDEX index,int source_flag);

	//////////////////////////// 输出 ////////////////////////////
	
	////////  COPY
	virtual BOOL   CopyAllData(const CTG_ScheduleInfo &schd,IScheduleComm *pOut,BOOL all_flag);
	virtual void   PollingAndCheckStatus();

	virtual void    Close(); //关闭连接
	virtual void   PollingForReqData(const CTG_ScheduleInfo& schd_info);
private:
	CTG_TrainScheduleBase *GetDataStore(const CTG_ScheduleInfo &source_info);
	long  m_nOperEntityID;
	long  m_nServerEntityID;
	BOOL  m_bQueryMode;
public:
	BOOL  SendMessageToServer(const CTG_ScheduleInfo &schd,TG_OPER_TYPE op_type,const CTG_SingleTrain  &train);
	BOOL  SendMessageToServer(const CTG_ScheduleInfo &schd,TG_OPER_TYPE op_type,const CTG_SingleMark  &mark);	
	
    /// Process Server message Input 
	BOOL  UpdateData(const TG_MsgStamp &stamp,TG_OPER_TYPE op_type,const CTG_SingleTrain  &train);
	BOOL  UpdateData(const TG_MsgStamp &stamp,TG_OPER_TYPE op_type,const CTG_SingleMark  &mark);	
    BOOL  UpdateTrainExtent(const TG_MsgStamp& stamp, const CTG_TrainExtent& trainExtent);

	BOOL  InitAppendData(const CTG_ScheduleInfo &schd,const CTG_SingleMark  &mark);	
	BOOL  InitAppendData(const CTG_ScheduleInfo &schd,const CTG_SingleTrain  &train);

	BOOL  SendScheduleLoadMessage_ToServer(const CTG_ScheduleInfo &schd_info,BOOL unload);
	BOOL  Init(int  con_id,int server_type);
	BOOL  ProcessServerErrorEcho(const CTG_ScheduleInfo &schd,TG_OBJECT_TYPE obj_type,TRAIN_INDEX train_index,long err_code);
	BOOL  SendScheduleLoadMessage_ToServer(const CTG_ScheduleInfo &schd_info,int stations[],int statcnt);
public:
	CWintgNewServer();
	~CWintgNewServer();
	void   SetQueryMode(BOOL va){m_bQueryMode=va;}
	BOOL   IsQueryMode() {return m_bQueryMode;}
public:
	// static stuff
	static BOOL  ProcessServerCommStatus(long server_id,BOOL  active_flag);   // STATIC!!!
	static BOOL  InitSetvers();
	static BOOL  GlobalUpdateData(const TG_MsgStamp &stamp,TG_OPER_TYPE op_type,const CTG_SingleTrain  &train);
	static BOOL  GlobalUpdateData(const TG_MsgStamp &stamp,TG_OPER_TYPE op_type,const CTG_SingleMark  &mark);	
	static BOOL  GlobalInitAppendData(const TG_MsgStamp &stamp,const CTG_SingleMark  &mark);	
	static BOOL  GlobalInitAppendData(const TG_MsgStamp &stamp,const CTG_SingleTrain  &train);
	static int   GetServerEntityIDs(long entitys[],int limit);
	static BOOL  GlobalProcessServerErrorEcho(const TG_MsgStamp &stamp,TG_OBJECT_TYPE obj_type,long err_code);

	static BOOL  GlobalSetInitStatus(const TG_MsgStamp &stamp,TG_OBJECT_TYPE obj_type,BOOL va,int obj_count);
	static void  CheckStayPassRelation(const CTG_SingleTrain  &srctrain);
	static void  GlobalTrainExtent(const TG_MsgStamp& stamp, const CTG_TrainExtent& trainExtent);
};
#endif