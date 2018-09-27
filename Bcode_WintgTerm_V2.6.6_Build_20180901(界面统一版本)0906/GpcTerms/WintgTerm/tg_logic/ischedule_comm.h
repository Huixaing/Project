#ifndef __INCLUDE__SCHD_COMM_INT_
#define __INCLUDE__SCHD_COMM_INT_
class CTG_ScheduleInfo;
class CTG_SingleTrain;
class CTG_SingleMark;
class CTG_TrainRecordList;
class CTG_TrainEngineList;
class CTG_TrainInfo;
class CTG_TrainScheduleBase;

class  IScheduleComm{
protected:
	///////// DATA STORE
	enum{DATA_SOURCE_NUM=8};
    CTG_TrainScheduleBase  *m_pDataStore[DATA_SOURCE_NUM];
	BOOL            m_bCommReady;
	int             m_nSyncFlag;

private:
	BOOL   m_bTrainDataInited;
	BOOL   m_bMarkDataInited;

public:
	bool m_bForMMI;
public:
	enum TG_OPER_TYPE{TG_OPER_ADD,TG_OPER_UPDATE,TG_OPER_DELETE};

	void   SetDataStore(CTG_TrainScheduleBase *p)
	{
		int i;
		for(i=0;i<DATA_SOURCE_NUM;i++)
		{
			if(m_pDataStore[i]==NULL)
			{
                m_pDataStore[i]=p;
				break;
			}
		}
	}
	void   ClearDataStore(CTG_TrainScheduleBase *p)
	{
		int i;
		for(i=0;i<DATA_SOURCE_NUM;i++)
		{
			if(m_pDataStore[i]==p)
                m_pDataStore[i]=NULL;
		}
	}
	IScheduleComm()
	{
		m_bTrainDataInited=TRUE;
		m_bMarkDataInited=TRUE;
		m_bCommReady=FALSE;
		m_nSyncFlag=-1;  //SYCFLAG_NULL;
		for(int i=0;i<DATA_SOURCE_NUM;i++) m_pDataStore[i]=NULL;
		m_bForMMI=false;
	}
	////////  STATUS
	void  SetCommReady(BOOL v) {m_bCommReady=v;}
	BOOL  CommReady(){return  m_bCommReady;} // READY for UPDATING
	BOOL  DataOK()
	{
		if(TRUE==m_bTrainDataInited && TRUE==m_bMarkDataInited)
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
	
	void   SetTrainDataInited(BOOL va) 
	{
		m_bTrainDataInited=va;
	}
	void   SetMarkDataInited(BOOL va) 
	{
		m_bMarkDataInited=va;
	}
	BOOL   IsTrainDataInited() {return m_bTrainDataInited;}
	///////////////////////////////////////////////

	virtual void    Close(){}; //关闭连接
	///////// ID
	int    GetInterfaceFlag(){return m_nSyncFlag;}
	void   SetInterfaceFlag(int flag){m_nSyncFlag=flag;}
	virtual BOOL   IsAsyncMode(){return FALSE;};
	//////////////////////
	///////////LOCK
	virtual BOOL   Lock(){return TRUE;};
	virtual BOOL   UnLock(){return TRUE;};
	//////////////////////////// 输入 ////////////////////////////
	///////// UPDATE
	virtual BOOL   UpdateTrain(const CTG_ScheduleInfo & schd,const CTG_SingleTrain  &train,int source_flag)=0;
	virtual BOOL   UpdateMark(const CTG_ScheduleInfo &schd,const CTG_SingleMark  &block_info,int source_flag)=0;

	///////// DELETE
	virtual BOOL   DeleteTrain(const CTG_ScheduleInfo &schd,const CTG_SingleTrain  &train,int source_flag)=0;
	virtual BOOL   DeleteMark(const CTG_ScheduleInfo &schd,const CTG_SingleMark  &train,int source_flag)=0;
	virtual BOOL   DeleteAllData(const CTG_ScheduleInfo &schd,int source_flag)=0;
	///////// APPEND
	virtual TRAIN_INDEX   AppendTrain(const CTG_ScheduleInfo &schd,const CTG_SingleTrain  &train,int source_flag)=0;
	virtual TRAIN_INDEX   AppendMark(const CTG_ScheduleInfo &schd,const CTG_SingleMark  &mark,int source_flag)=0;

	//////  TrainIndex based function
	virtual BOOL   DeleteTrain(const CTG_ScheduleInfo &schd,TRAIN_INDEX index,int source_flag)=0;
	virtual BOOL   DeleteMark(const CTG_ScheduleInfo &schd,TRAIN_INDEX index,int source_flag)=0;

	////////  COPY
	virtual BOOL   CopyAllData(const CTG_ScheduleInfo &schd,IScheduleComm *pOut,BOOL all_flag)=0;

	///////  STATUS CHECK
	virtual void   PollingAndCheckStatus(){};

	//////
	virtual void   PollingForReqData(const CTG_ScheduleInfo& schd_info){};
	virtual void   PollingForDeleteTrain(TRAIN_INDEX index){};
	virtual void   PollingForDeleteBlock(TRAIN_INDEX index){};
	virtual void   PollingForAppendTrain(CTG_SingleTrain* ptrain){};
	virtual void   PollingForAppendBlock(CTG_SingleMark* pblock){};
	virtual void   PollingForMoveTGOCX(void){};  // 克服索引重复
};
#endif