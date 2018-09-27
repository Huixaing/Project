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
	CTG_TrainScheduleBase m_cBasicSchduleData;
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
	IScheduleComm  *m_pMasterServerCommBasic;
	IScheduleComm  *m_pMasterServerCommBasicPrev;
	IScheduleComm  *m_pMasterServerCommBasicBackup;
	
	BOOL    m_bQueryMode;

public:
	void   SetQueryMode(BOOL va);
	BOOL   IsQueryMode() {return m_bQueryMode;}

	// 2010.2.26 外部调用
	BOOL  PollingForUpdate(CTG_TrainScheduleBase *pData,IScheduleComm *pcomm,BOOL all_flag);
private:
	BOOL  WriteSchedule(CTG_TrainScheduleBase *pData,IScheduleComm *pcomm,BOOL allwrite);
	BOOL  ReadSchedule(CTG_TrainScheduleBase *pData,IScheduleComm *pcomm);

public:
	bool  TGOCXSetTrainExitEntry(CTG_SingleTrain& train, CTgX* pTG);
	bool  TGOCXSetTrainExitEntry(CTG_SingleTrain& train, CTgX* pTG, int first_entry, int last_exit);
	void  CheckTrainNo(CTG_SingleTrain& newtrain, CStringArray& train_alarm);
	
	void  SetServerCommBasic(IScheduleComm *p) {m_pMasterServerCommBasic=p;}
	void  SetServerCommBasicPrev(IScheduleComm *p) {m_pMasterServerCommBasicPrev=p;}
	void  SetServerCommBasicBackup(IScheduleComm *p) {m_pMasterServerCommBasicBackup=p;}
	
	void  PostCommStatus(IScheduleComm *pcomm,BOOL active);
public:
	///////// INIT /////////////
	CWorkScheduleHolder();
	~CWorkScheduleHolder();
	BOOL  InitScheduleData(int section);   // 初始化内部数据结构
	
	BOOL  ReloadData(CTG_ScheduleInfo::SCHEDULE_TYPE  tp=CTG_ScheduleInfo::SCHEDULE_TYPE_NULL,BOOL delfirst=FALSE);
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
	///////////// 加车 //////////////////
	
	BOOL  GenTrain_FreeDraw(CTG_SingleTrain &srctrain,const CTG_ScheduleInfo &schd,BOOL qualify_runtime,BYTE run_type); // train中是界面生成的初始数据
	
	void  SetTrainFirstStationZeroTMToArriveTm(CTG_SingleTrain &train, int arrivetm);
	int   CalcTrainLastSt_NextStTime(CTG_SingleTrain &train);
	
	BOOL  GenTrainBasicSchedule_OPMSx(CTG_SingleTrain &srctrain, std::vector<CTG_SingleTrain>& train_list,const CTG_ScheduleInfo &schd, bool& bOldSplitTrain);
	
	
	BOOL  GenMark_BasicSchedule(CTG_SingleMark &mark,const CTG_ScheduleInfo &schd,TRAIN_INDEX index);
	BOOL  GenTrain_AutoAdjustManualUpdate(CTG_SingleTrain &srctrain,const CTG_ScheduleInfo &schd,const CMMIOperFlag &oper_flag,CWnd* pParentWnd=NULL);
	
	CTG_TrainScheduleBase *GetBasicScheduleData() {return &m_cBasicSchduleData;}
	CTG_TrainScheduleBase *GetBasicPrevScheduleData() {return &m_cBasicPrevSchduleData;}
	CTG_TrainScheduleBase *GetBasicBackupScheduleData() {return &m_cBasicBackupSchduleData;}

	
	BOOL  IsSyncedWithServer(CTG_ScheduleInfo::SCHEDULE_TYPE  schd_type,const CTG_LogicObject *pobj);

	int   GetServerNoSyncedObjectCount(CTG_ScheduleInfo::SCHEDULE_TYPE  type,CString &Info);
	
	int   GetNoSyncedObjectCount(CTG_ScheduleInfo::SCHEDULE_TYPE  type,IScheduleComm *pcomm,CString &Info);

	BOOL  LoadDataFromLocalFiles(CTG_ScheduleInfo::SCHEDULE_TYPE type);
	BOOL  SaveDataToLocalFiles(CTG_ScheduleInfo::SCHEDULE_TYPE  type, CString& str);
	BOOL  GenTrain_ShiftSchedule(CTG_SingleTrain &srctrain,const CTG_ScheduleInfo &schd,TRAIN_INDEX index,STATION_NO station,TIME bm,TIME em);
	BOOL  GenTrain_ShiftSchedule(CTG_SingleTrain &srctrain,const CTG_ScheduleInfo &schd,const char *train_no,STATION_NO station,TIME bm,TIME em);
	void  SetFileSaveFlag();

	IScheduleComm  *GetServerCommByScheduleType(CTG_ScheduleInfo::SCHEDULE_TYPE type);
	void  InitCommInterfaceStatus(IScheduleComm *pcomm,BOOL va);
	
	void  SetUserID(int id);
	
	void  ReqBasicScheduleName();
};

#endif




