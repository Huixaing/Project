#ifndef __INCLUDE__WORK_SCHD___
#define __INCLUDE__WORK_SCHD___
#include "tg_schedule_base.h"
#include "ischedule_comm.h"
#include "tg_basicpara.h"
const int NEIBPLAN_ADJUST_NUM=32;

class CTgX;
class CWorkScheduleHolder{
	enum{MAX_COMM_INTF_NUM =32};
 // data
	CTG_TrainScheduleBase m_cWorkSchduleData;
	CTG_TrainScheduleBase m_cBasicSchduleData;
	CTG_TrainScheduleBase m_cShiftSchduleData;
	CTG_TrainScheduleBase m_cBasicPrevSchduleData;
	CTG_TrainScheduleBase m_cBasicBackupSchduleData;

 // comm_interfaces
	struct{
		CTG_ScheduleInfo::SCHEDULE_TYPE nSchType;
        int            nSectionID;
		IScheduleComm *pCommInterface;
		BOOL           bActive;
	}m_aCommArray[MAX_COMM_INTF_NUM];
	
	
	// para
	IScheduleComm  *m_pMasterServerCommWork;
	IScheduleComm  *m_pMasterServerCommShift;
	IScheduleComm  *m_pMasterServerCommBasic;
	IScheduleComm  *m_pMasterServerCommBasicPrev;
	IScheduleComm  *m_pMasterServerCommBasicBackup;
	IScheduleComm  *m_pMasterDatabaseComm;

	BOOL    m_bQueryMode;

	int mNeibPlanCount;
	NeibAdjustExtern mNeibPlanStation[NEIBPLAN_ADJUST_NUM];

	int mNeibAllStationCount;
	NeibStationList m_ns[20];
public:
	enum DATA_RUN_MODE{LOCAL_MODE=1,SERVER_MODE=2};  // 本地数据库模式和服务器模式

	void   SetQueryMode(BOOL va);
	BOOL   IsQueryMode() {return m_bQueryMode;}

	// 2010.2.26 外部调用
	BOOL  PollingForUpdate(CTG_TrainScheduleBase *pData,IScheduleComm *pcomm,BOOL all_flag);
	void  PollingAutoMove(TIME ctm);

	void  PollingDataForAutoMove();
	void  PollingForReqData();
	BOOL  InitScheduleData_NEW(int section,CTG_ScheduleInfo::SCHEDULE_TYPE type); 
	BOOL  ShiftWorkSchedule_NEW(const CTG_ScheduleInfo &info);
private:
	TIME m_nPollingMiniute;
	TIME m_nPollingHour;

    DATA_RUN_MODE  m_nMode;

	BOOL  WriteSchedule(CTG_TrainScheduleBase *pData,IScheduleComm *pcomm,BOOL allwrite);
	BOOL  ReadSchedule(CTG_TrainScheduleBase *pData,IScheduleComm *pcomm);
	

	BOOL  TrainAlreadyExsitInWorkSchedule(const TRAIN_NO trainno);
	
public:
	bool  TGOCXSetTrainExitEntry(CTG_SingleTrain& train, CTgX* pTG);
	bool  TGOCXSetTrainExitEntry(CTG_SingleTrain& train, CTgX* pTG, int first_entry, int last_exit);
	void  CheckTrainNo(CTG_SingleTrain& newtrain, CStringArray& train_alarm);
	
	BOOL  SwitchToLocalMode();
	BOOL  SwitchToServerMode();
	void  SetServerCommWork(IScheduleComm *p) {m_pMasterServerCommWork=p;}
	void  SetServerCommShift(IScheduleComm *p) {m_pMasterServerCommShift=p;}
	void  SetServerCommBasic(IScheduleComm *p) {m_pMasterServerCommBasic=p;}
	void  SetServerCommBasicPrev(IScheduleComm *p) {m_pMasterServerCommBasicPrev=p;}
	void  SetServerCommBasicBackup(IScheduleComm *p) {m_pMasterServerCommBasicBackup=p;}
	void  SetDatabaseComm(IScheduleComm *p) {m_pMasterDatabaseComm=p;}
	IScheduleComm *GetDatabaseComm() {return m_pMasterDatabaseComm;}
	void  PostCommStatus(IScheduleComm *pcomm,BOOL active);
public:
	///////// INIT /////////////
	CWorkScheduleHolder(DATA_RUN_MODE mode);
	~CWorkScheduleHolder();
	BOOL  InitScheduleData(int section,CTG_ScheduleInfo::SCHEDULE_TYPE type);   // 初始化内部数据结构
	BOOL  ShiftWorkSchedule(const CTG_ScheduleInfo &info);  // 换班,读取应用数据
	BOOL  ReloadData(CTG_ScheduleInfo::SCHEDULE_TYPE  tp=CTG_ScheduleInfo::SCHEDULE_TYPE_NULL,BOOL delfirst=FALSE);
	void  GetWorkScheduleInfo(CTG_ScheduleInfo &info){m_cWorkSchduleData.GetScheduleInfo(info);}
	void  GetShiftScheduleInfo(CTG_ScheduleInfo &info){m_cShiftSchduleData.GetScheduleInfo(info);}
	void  GetBasicScheduleInfo(CTG_ScheduleInfo &info){m_cBasicSchduleData.GetScheduleInfo(info);}
	void  GetBasicPrevScheduleInfo(CTG_ScheduleInfo &info){m_cBasicPrevSchduleData.GetScheduleInfo(info);}
	
	///////// COMM INTERFACE ///////
	BOOL  RegisterCommInterface(CTG_ScheduleInfo::SCHEDULE_TYPE sch_type,IScheduleComm *pcomm, bool bMMI=false);
	BOOL  UnRegisterCommInterface(IScheduleComm *pcomm);
    ///////// DATABSE FUNCTION ///////
	BOOL  WriteSchedule(CTG_ScheduleInfo::SCHEDULE_TYPE  type,IScheduleComm *pcomm,BOOL allwrite);
	void  DeleteAllData();
	///////// POLLING
	BOOL  PollingData(TIME curtm);
	void RefreshCommStatus();

public:
	static int IsCheckStationSide(int station);
	static bool IsNeedCheckCtcModePlan(int station);
	static BOOL GlobalSetCommInterfaceStatus(IScheduleComm *p,BOOL va);
	static void GlobalRefreshCommStatus();
public:
	static BOOL  Polling(TIME curtm);
	static BOOL  InitTGHolders();
	
private:
	BOOL  GenTrainBasicSchedule_OPMS(CTG_SingleTrain &srctrain,CTG_SingleTrain &othertrain,const CTG_ScheduleInfo &schd);
public:
	BOOL  GenTrain_OPMS(CTG_SingleTrain &srctrain,CTG_SingleTrain &othertrain,const CTG_ScheduleInfo &schd,BOOL newflag);
	BOOL  GenTrain_OPMS(CTG_SingleTrain &srctrain,const CTG_ScheduleInfo &schd,BOOL newflag);
public:
	///////////// 加车 //////////////////
	BOOL  GenTrain_ChangeRoute(CTG_SingleTrain &srctrain,const CTG_ScheduleInfo &schd,int route_no);
	BOOL  GenTrain_FreeDraw(CTG_SingleTrain &srctrain,const CTG_ScheduleInfo &schd,BOOL qualify_runtime,BYTE run_type); // train中是界面生成的初始数据
	
	void  SetTrainFirstStationZeroTMToArriveTm(CTG_SingleTrain &train, int arrivetm);
	int   CalcTrainLastSt_NextStTime(CTG_SingleTrain &train);
	BOOL  GenTrain_OPMSx(CTG_SingleTrain &srctrain, std::vector<CTG_SingleTrain>& train_list,const CTG_ScheduleInfo &schd,BOOL newflag,bool& bOldSplitTrain);
	BOOL  GenTrainBasicSchedule_OPMSx(CTG_SingleTrain &srctrain, std::vector<CTG_SingleTrain>& train_list,const CTG_ScheduleInfo &schd, bool& bOldSplitTrain);
	
	BOOL  GenTrain_Passway(CTG_SingleTrain &srctrain,const CTG_ScheduleInfo &schd,const TRAIN_NO train,int  pathway, BYTE start_flag, BYTE end_flag ,STATION_NO station,TIME ltime,BYTE run_type);
	BOOL  GenTrainTmp_Passway(CTG_SingleTrain &srctrain,const CTG_ScheduleInfo &schd,const CString train,int  pathway, BYTE start_flag, BYTE end_flag ,BYTE start_index, BYTE end_index,TIME ltime,BYTE run_type);
	BOOL  GenTrain_BasicSchedule(CTG_SingleTrain &srctrain,const CTG_ScheduleInfo &schd,TRAIN_INDEX index,STATION_NO station,TIME bm,TIME em);
	BOOL  GenTrain_BasicSchedule(CTG_SingleTrain &srctrain,const CTG_ScheduleInfo &schd,const TRAIN_NO trainno,STATION_NO station,TIME bm,TIME em);
	BOOL  GenMark_ShiftSchedule(CTG_SingleMark &mark,const CTG_ScheduleInfo &schd,TRAIN_INDEX index);
	BOOL  GenMark_BasicSchedule(CTG_SingleMark &mark,const CTG_ScheduleInfo &schd,TRAIN_INDEX index);
	// 基本图或日班计划上图时，列车数目超出最大数目判断处理：-1：列车超出最大容量 0：没有读取到列车 1：读取成功
	int   SelectSchdTrain(const TGBasicTrainListRequest &msg,TGSelectedBasicTrain info[],int *limit,CTG_ScheduleInfo::SCHEDULE_TYPE type,BOOL IsStart = FALSE);
	BOOL  GenTrain_AutoAdjustManualUpdate(CTG_SingleTrain &srctrain,const CTG_ScheduleInfo &schd,const CMMIOperFlag &oper_flag,CWnd* pParentWnd=NULL);
	

	CTG_TrainScheduleBase *GetWorkScheduleData() {return &m_cWorkSchduleData;}
	CTG_TrainScheduleBase *GetBasicScheduleData() {return &m_cBasicSchduleData;}
	CTG_TrainScheduleBase *GetShiftScheduleData() {return &m_cShiftSchduleData;}
	CTG_TrainScheduleBase *GetBasicPrevScheduleData() {return &m_cBasicPrevSchduleData;}
	CTG_TrainScheduleBase *GetBasicBackupScheduleData() {return &m_cBasicBackupSchduleData;}

	BOOL  CheckStplan(CTG_SingleTrain* ptrain, long start_time, long end_time, CTG_TrainPtrGroup& group, CTG_ObjectPtrGroup& markgroup, int rec_index);
	
	BOOL  IsSyncedWithServer(CTG_ScheduleInfo::SCHEDULE_TYPE  schd_type,const CTG_LogicObject *pobj);
	BOOL  IsSyncedWithDatabase(const CTG_LogicObject *pobj);
	int   GetServerNoSyncedObjectCount(CTG_ScheduleInfo::SCHEDULE_TYPE  type,CString &Info);
	int   GetDatabaseNoSyncedObjectCount(CTG_ScheduleInfo::SCHEDULE_TYPE  type);
	int   GetNoSyncedObjectCount(CTG_ScheduleInfo::SCHEDULE_TYPE  type,IScheduleComm *pcomm,CString &Info);

	BOOL  LoadDataFromLocalFiles(CTG_ScheduleInfo::SCHEDULE_TYPE type=CTG_ScheduleInfo::SCHEDULE_TYPE_NULL);
	BOOL  SaveDataToLocalFiles(CTG_ScheduleInfo::SCHEDULE_TYPE  type=CTG_ScheduleInfo::SCHEDULE_TYPE_NULL);
	BOOL  GenTrain_ShiftSchedule(CTG_SingleTrain &srctrain,const CTG_ScheduleInfo &schd,TRAIN_INDEX index,STATION_NO station,TIME bm,TIME em);
	BOOL  GenTrain_ShiftSchedule(CTG_SingleTrain &srctrain,const CTG_ScheduleInfo &schd,const char *train_no,STATION_NO station,TIME bm,TIME em);
	void  SetFileSaveFlag(WORD va);

	const char *CheckTrainManualUpdateOper(CTG_SingleTrain *pnewtrain,const CTG_ScheduleInfo &schd,const CMMIOperFlag &oper_flag,DWORD& flag);// return NULL ok,or ERROR info

	IScheduleComm  *GetServerCommByScheduleType(CTG_ScheduleInfo::SCHEDULE_TYPE type);
	void  InitCommInterfaceStatus(IScheduleComm *pcomm,BOOL va);

	BOOL  GenTrain_NeibSchedule_ConsiderTrainID(CTG_SingleTrain &newtrain,vector<CTG_SingleTrain>& othertrain_list, const CTG_SingleTrain &neibtrain, int& nNeibStaNo, int neib_section, int *add_or_update,CTG_TrainScheduleBase* pData, CStringArray& m_warning);

	void  GetNeibPlanEqualStation(int nThisSection, int nNeibSection);
	BOOL  AdjustNeibPlanByEqualStation(CTG_TrainScheduleBase* pData, 
													   int *add_or_update, 
													   CTG_SingleTrain &newtrain, 
													   const CTG_SingleTrain &neibtrain, 
													   vector<CTG_SingleTrain>& othertrain_list,
													   CStringArray& m_warning);

	BOOL  UpdateOrAddTrain(CTG_TrainScheduleBase* pData, int *add_or_update,
										   CTG_SingleTrain& srctrain, 
										   const CTG_SingleTrain &neibtrain, 
										   const CTG_TrainRecord *prec,
										   int passway, int nodeindex,
										   CStringArray& m_warning);

	bool  IsMatchConfigure(const CTG_TrainRecord *prec_before, const CTG_TrainRecord *prec_next, int index, bool bFlag, int& station1, int& station2);
	BOOL  RecvNeibPlanAll(CTG_TrainScheduleBase* pData, int *add_or_update, 
													   CTG_SingleTrain &newtrain, 
													   const CTG_SingleTrain &neibtrain, 
													   vector<CTG_SingleTrain>& othertrain_list,
													   CStringArray& m_warning);

	bool  UpdateOrAddTrainForNeibPlan6(CTG_TrainScheduleBase* pData, int *add_or_update,
										   CTG_SingleTrain& srctrain, 
										   const CTG_SingleTrain &neibtrain, 
										   int rec_index,
										   int index, 
										   CStringArray& m_warning);

	int   CheckNeedSendToNeibSection(const CTG_SingleTrain *single,int source_section,int dest_section,int& matched_rec_count, bool& bHavePreStation,int& nodeindex,int& passway, bool& bWholeMatch, bool& bAcceptTerminalTrain, int &neib_station, int &control_prev_abs);
	
	CTG_SingleTrain* GetTrainByShiftIDFromShiftSchedule(const DWORD nShiftTrainIndex, const int nStation);
	CTG_SingleTrain* GetTrainByTrainNoFromShiftSchedule(const CTG_SingleTrain &neibtrain, int neib_station, CStringArray& m_warning, bool& bFind);
	BOOL  NeibScheduleUpdate_Shanghai(CTG_SingleTrain &newtrain, const CTG_SingleTrain &neibtrain, const int nNeibStaNo, const int control_prev_abs);
	BOOL  NeibScheduleAppend_Shanghai(CTG_SingleTrain &newtrain, const CTG_SingleTrain &neibtrain, const int first_station, int neib_station, int neib_section, CStringArray& m_warning, int nodeindex, int control_prev_abs);
	
	CTG_SingleTrain* GetTrainByNeibTrainID(const CTG_SingleTrain &neibtrain, const CTG_TrainRecord *prec, CTG_TrainScheduleBase* pData, CStringArray& m_warning, bool& bFind);
	CTG_SingleTrain* GetTrainByNeibTrainShiftID(const CTG_SingleTrain& neibtrain, const CTG_TrainRecord* pneib_rec);

	int   CheckNeedSendToNeibSectionByEntry(const CTG_SingleTrain *single,int source_section,int dest_section,int *matched_rec_count,int *nodeindex,int *passway, int* exit_entry);

	void  SetUserID(int id);
	int   CheckSidePlatform(const CTG_SingleTrain& train, CString& strErrText);

	BOOL  GenTrain_NeibSchedule_SY(CTG_SingleTrain &newtrain,const CTG_SingleTrain &neibtrain, int& nNeibStaNo, int neib_section, int *add_or_update, CTG_TrainScheduleBase* pData, CStringArray& m_warning);
	int   CheckNeedSendToNeibSectionByEntry_SY(const CTG_SingleTrain *single,int neib_section,int this_section,int& nodeindex,int& passway,int& id, int& flag);

	void  PassTrainEngineNo(CTG_SingleTrain& newtrain, const CTG_SingleTrain* train); // 传递机车号

// AutoAdjust
	BOOL   AdjustTrainForCollision(CTG_SingleTrain &train,int recIndex,const CTG_ScheduleInfo &schd);
	BOOL   AdjustTrainForBasicSchedule(CTG_SingleTrain &train,int recIndex,const CTG_ScheduleInfo &schd);
	BOOL   AdjustTrainForMutiPass(CTG_SingleTrain &train,int recIndex,const CTG_ScheduleInfo &schd);
	BOOL   GetRelaTrainsOfMark(const CTG_SingleMark *pmark,CTG_ObjectPtrGroup &rela_trains,const CTG_ScheduleInfo &schd); //查找与mark关联的车次
	BOOL   AdjustSingleTrainForMark(CTG_SingleTrain &train,int recIndex,const CTG_SingleMark *pmark,const CTG_ScheduleInfo &schd);

	BOOL  GenTrain_NeibSchedule_Reverse(CTG_SingleTrain &newtrain,const CTG_SingleTrain &neibtrain, int& nNeibStaNo, int neib_section, int *add_or_update);
	int   CheckNeedSendToNeibSectionByReverse(const CTG_SingleTrain *single,int this_section,int neib_section,int& matched_rec_count,int& nodeindex,int& id);

	BOOL  CheckTrainDepartStplan(int con_id, CTG_SingleTrain &srctrain, CTG_ObjectPtrGroup& markgroup, int rec_index, CString& str);

	void  ReqBasicScheduleName();
};

#endif




