#ifndef __INCLUDE_MMI_COMM_INTER___
#define __INCLUDE_MMI_COMM_INTER___
#include "../tg_logic/ischedule_comm.h"
#include <list>
class CTgX;
class CWorkScheduleHolder;
#define MAX_OPER_BACKUP_NUM      128
#define MAX_BACKUP_TIMERANGE     600
class CWintgViewMMI:public IScheduleComm{
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

	virtual BOOL   DeleteTrain(const CTG_ScheduleInfo &schd,TRAIN_INDEX index,int source_flag);
	virtual BOOL   DeleteMark(const CTG_ScheduleInfo &schd,TRAIN_INDEX index,int source_flag);
	////////  LOAD
	virtual BOOL   LoadAllData(const CTG_ScheduleInfo &schd,IScheduleComm *pIn);
	//////////////////////////// 输出 ////////////////////////////
	
	////////  COPY
	virtual BOOL   CopyAllData(const CTG_ScheduleInfo &schd,IScheduleComm *pOut,BOOL all_flag);

private:
	CTgX   *m_pTGCtrl;
	CWnd   *m_pParentWnd;
	CTG_ScheduleInfo  m_schd_info;
	CWorkScheduleHolder *m_pSchdHolder;
	
	class BackupOper{
	public:
		time_t          m_opertm;
		TG_OPER_TYPE    m_oper;
		CTG_LogicObject *m_pobj; 

		BackupOper()
		{
			m_opertm=0;
			m_oper=TG_OPER_ADD;
			m_pobj=NULL;
		}
		void SetValue(const CTG_LogicObject *pobj,TG_OPER_TYPE tp,time_t t)
		{
			m_opertm=t;
			m_oper=tp;
			if(pobj->GetObjectType()==TG_OBJ_TRAIN)
			{
               m_pobj=new CTG_SingleTrain(*(CTG_SingleTrain *)pobj);

			}
			else
			if(pobj->GetObjectType()==TG_OBJ_MARK)
			{
			   m_pobj=new CTG_SingleMark(*(CTG_SingleMark *)pobj);
			}
			else
				pobj=NULL;
		}

		BackupOper(const BackupOper &src)
		{
			SetValue(src.m_pobj,src.m_oper,src.m_opertm);
		}

	    BackupOper& operator=(const BackupOper &src)
		{
			if(this == &src)
				return *this;
			
			if(m_pobj)
			{
				if(m_pobj->GetObjectType()==TG_OBJ_TRAIN)
					delete (CTG_SingleTrain *)m_pobj;
				else if(m_pobj->GetObjectType()==TG_OBJ_MARK)
					delete (CTG_SingleMark *)m_pobj;
				else
					delete m_pobj;
			}
			m_pobj=NULL;
			SetValue(src.m_pobj,src.m_oper,src.m_opertm);
			return *this;
		}
	  ~BackupOper()
	  {
		  if(m_pobj)
		  {
			  if(m_pobj->GetObjectType()==TG_OBJ_TRAIN)
				  delete (CTG_SingleTrain *)m_pobj;
			  else
			  if(m_pobj->GetObjectType()==TG_OBJ_MARK)
				  delete (CTG_SingleMark *)m_pobj;
			  else
				  delete m_pobj;
		  }
	  }
	};

	list<BackupOper>  m_listBackupOper;

	void   PushOper(const CTG_LogicObject *pobj,TG_OPER_TYPE oper);
public:
	BOOL   PopOper(const CTG_ScheduleInfo &info);
	void   OnClearTimeoutOper();
public:
	CWintgViewMMI();
	void   SetTGCtrl(CTgX *p) {m_pTGCtrl=p;if(p) SetCommReady(TRUE); else  SetCommReady(FALSE);}
	CTgX*   GetTGCtrl() {return m_pTGCtrl;}
	void   SetParentWnd(CWnd *p) {m_pParentWnd=p;}
	CTG_TrainScheduleBase *GetDataStore(const CTG_ScheduleInfo &source_info);
    void   SetScheduleInfo(const CTG_ScheduleInfo &info) {m_schd_info=info;}
	void   GetScheduleInfo(CTG_ScheduleInfo &info) {info=m_schd_info;}
	void   SetScheduleHolder(CWorkScheduleHolder *pHolder) {m_pSchdHolder=pHolder;}

	void   PollingForMoveTGOCX(void); 	// 20171218克服索引重复
	void   PollingForDeleteTrain(TRAIN_INDEX index);
	void   PollingForDeleteBlock(TRAIN_INDEX index);
	void   PollingForAppendTrain(CTG_SingleTrain* ptrain);
	void   PollingForAppendBlock(CTG_SingleMark* pblock);
	
public:
	void   UpdateTGFromData();
	void   OutprintCZHRelation();
	/////// OPERATION
	BOOL   ProcessMMIOperation(TG_OBJECT_TYPE ob_type,TG_OPER_TYPE op_type,TRAIN_INDEX index,const char *xml); 
	
	BOOL  AddTrain_BasicSchedule(bool secret, const TRAIN_NO orig_trainno,const TRAIN_NO hat_trainno,STATION_NO station=NULL_STATION,TIME bm=NULL_TIME,TIME em=NULL_TIME);
	BOOL  AddTrain_BasicSchedule(TRAIN_INDEX basic_index,STATION_NO station=NULL_STATION,TIME bm=NULL_TIME,TIME em=NULL_TIME);
	BOOL  AddTrain_ShiftSchedule(bool secret, TRAIN_INDEX shift_index,STATION_NO station=NULL_STATION,TIME bm=NULL_TIME,TIME em=NULL_TIME);
	BOOL  AddTrain_ShiftSchedule(const char *train_no,STATION_NO station=NULL_STATION,TIME bm=NULL_TIME,TIME em=NULL_TIME);
	BOOL  ProcessMMIOperation(TG_OPER_TYPE op_type,CTG_SingleMark &mark,TRAIN_INDEX mark_index=NULL_TRAININDEX);
	BOOL  ProcessMMIOperation(TG_OPER_TYPE op_type,CTG_SingleTrain &train,CMMIOperFlag &oper_flag,TRAIN_INDEX train_index=NULL_TRAININDEX);
	BOOL  AddMark_ShiftSchedule(TRAIN_INDEX shift_index);
	BOOL  AddMark_BasicSchedule(TRAIN_INDEX shift_index);
	BOOL  CanBackOper();

	void  SetEngineIds(CTG_SingleTrain &srctrain, const CDWordArray& status, const CStringArray& engineId, int conid);
	BOOL  AddTrain_BasicSchedule(bool secret, const TRAIN_NO orig_trainno, const CStringArray& engineId, const CDWordArray& status);
	BOOL  AddTrain_ShiftSchedule(bool secret, TRAIN_INDEX shift_index,const CStringArray& engineId, const CDWordArray& status, STATION_NO station,TIME bm,TIME em);
	
	// 按照经路加车
	BOOL  AddTrain_Passway(bool secret, const TRAIN_NO train, const CStringArray& engineId, const CDWordArray& status,int  pathway, BYTE start_flag, BYTE end_flag ,TIME ltime,BYTE run_type);
	BOOL  AddTrainTmp_Passway(bool secret, CStringArray& aryEngineId, CDWordArray& aryStatus, CString trainId, int  pathway, BYTE start_flag, BYTE end_flag, BYTE start_index, BYTE end_index, TIME ltime,BYTE run_type);
	BOOL  AddTrain_Passway(bool secret, const TRAIN_NO train,const TRAIN_NO chg_train,int  pathway, BYTE start_flag, BYTE end_flag ,TIME ltime,BYTE run_type);
	
	BOOL   UpdateObject_ByProperty(const CTG_LogicObject *pobj);
	BOOL   UpdateTrain_ByBasicInfo(CTG_SingleTrain& train);

	// 以下用于邻台计划或者调整计划等其他方式的修改操作，非界面直接操作
	BOOL   AddTrain_Directly(CTG_SingleTrain &train);
	BOOL   UpdateTrain_Directly(CTG_SingleTrain &train, bool bCheckRelation=false);
	BOOL   DeleteTrain_Directly(TRAIN_INDEX index);
	BOOL   UpdateMMITrain_Directly(ULONG train_index);
private:
	// 用于通过非图定加车,给车设置基本图计划时间
	BOOL   SetFreedrawTrainPlanTime(CTG_SingleTrain& train, int rec_index=0); 
	void   RemoveTrainRelations(CTG_SingleTrain& train, CTG_TrainScheduleBase *pData);
	bool   IsTrainHasRelation(TRAIN_INDEX train_index, TRAIN_INDEX prev_train_index, CTG_TrainScheduleBase *pData);
private:
	BOOL   m_bEnableFreeTrainOper;
	int    m_nTGUpdateFlag; // 2010.2.26  当为1时，tg不立即刷新界面

public:
	void   EnableFreeTrainOper(BOOL va) {m_bEnableFreeTrainOper=va;}
	BOOL   GenTrainNodeIndex(TRAIN_INDEX train_index=NULL_TRAININDEX);
	void   SetTGUpdateFlag(int flag){m_nTGUpdateFlag=flag;}

public:
	BOOL   AdjustTrainForCollision(CTG_SingleTrain &train,int recIndex);
	BOOL   AdjustTrainForBasicSchedule(CTG_SingleTrain &train,int recIndex);
	BOOL   AdjustTrainForMutiPass(CTG_SingleTrain &train,int recIndex);
	BOOL   AdjustTrainsForMark(const CTG_SingleMark *pmark); // 根据mark 调整相关车次
	
};
#endif