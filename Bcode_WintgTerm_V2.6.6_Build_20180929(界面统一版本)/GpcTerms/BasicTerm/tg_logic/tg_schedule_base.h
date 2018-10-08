#ifndef _DEF_TRAIN_DEF_H
#define _DEF_TRAIN_DEF_H
//#include "sysincl.h"


#include "tg_defines.h"
#include "bitstatus.h"
#include "tg_structdef.h"
#include "ischedule_comm.h"
#include <list>
#include <vector>
#include <map>
#include <string>

#define LOCK_ENABLED_WINDOWS

#ifdef LOCK_ENABLED_WINDOWS
#include "afxmt.h"
#endif

class CTG_TrainExtent
{
public:
	CTG_TrainExtent();
	virtual ~CTG_TrainExtent() {}
	CTG_TrainExtent(const CTG_TrainExtent& rhs);
	CTG_TrainExtent& operator=(const CTG_TrainExtent& rhs);

public:
	void SetTrainIndex(TRAIN_INDEX index) 
	{ 
		train_index=index;
	}
public:
	TRAIN_INDEX     train_index;
	
	char			chezu_no[32];		// 车组号
	TRAIN_INDEX		prev_shift_index;	//前趟车全局ID	 
	TRAIN_INDEX		next_shift_index;	//直通ID	 
	TRAIN_INDEX		prev_train_index;	//前趟车
	TRAIN_INDEX		prev_basic_index;	//前趟车基本图ID
	STATION_NO		prev_dest_sta;		//前一个方向站 
	STATION_NO		next_dest_sta;		//后一个方向站 
	IDWORD			halt_state;         //停运标志

	short kd_type;   //类型 1:客调 2：特调
	CString kd_content; //客调内容

	std::vector<TG_TrainYingyeInfo> yy_list;
	std::vector<TG_TrainCrewInfo> crew_list;
	std::vector<TG_TrainTicketInfo> ticket_list;
	std::vector<TG_EngineCrewInfo> engine_list;
	std::vector<TG_TrainOPMSRecordInfo> opms_rd_list;
};

enum {
	ALARM_FLAG_SIDE_ERROR=0x00000001L,
	ALARM_FLAG_MEET_ERROR=0x00000002L,
	ALARM_FLAG_ENTRY_ERROR=0x00000004L,
	ALARM_FLAG_LINE_BLOCK_ERROR=0x00000008L,
	ALARM_FLAG_NOT_STOP_ERROR=0x00000010L,
	ALARM_FLAG_NOT_PASS_ERROR=0x00000020L,
	ALARM_FLAG_SIDE_WARN=0x00010000L,
	ALARM_FLAG_MEET_WARN=0x00030000L,
	ALARM_FLAG_SERVER_NOT_ACK=0x00000008L,
	ALARM_FLAG_SIDE_CRITICAL_ERROR=0x00000040L,

};

struct CMMIOperFlag{
	enum
	{
		STAY_TO_PASS = 0x00000001L,
		PASS_TO_STAY = 0x00000002L,
		TRAIN_ATTRIB_CHANGED = 0x00000004L,
		ENGINE_INFO_CHANGED = 0x00000008L,

		CHANGE_TRAINID = 0x00000010,
		HAT_TRAINID    = 0x00000020,
		ZHEFAN_TRAINID = 0x00000040,
		MODIFY_SIDE    = 0x00000080,
		SET_ABS_TOP			= 0x00000100,
		CONTINUE_DRAW_TRAIN   = 0x00000200,
		MUTI_REC_STAY_TO_PASS = 0x00000400,
		DEPART_BY_PLAN = 0x00000800,

		DEPART_BY_MEET = 0x00001000,
		TRAIN_PLAN_PARA = 0x00002000,
		HELP_TRAIN = 0x00004000,
		TURN_REALITY = 0x00008000,

		MOUSE_TURN_REALITY = 0x00010000,
		MODIFY_TRAIN_TIME = 0x00020000,
	};
    
	unsigned long nFlag;
	int  m_nStation;
	int  m_nRecIndex;
	BYTE m_nRun_type;
	char m_trainId[20];
	CMMIOperFlag()
	{
		nFlag=0;
		m_nStation=0;
		m_nRecIndex=0;
		m_nRun_type=255;
		memset(m_trainId, 0, sizeof(m_trainId));
	}
};

class CTG_ScheduleInfo{
public:
	enum SCHEDULE_TYPE
	{
		SCHEDULE_TYPE_NULL=0,
		SCHEDULE_TYPE_SHIFT=4,
		SCHEDULE_TYPE_WORK=5,
		SCHEDULE_TYPE_BASIC=1,
		SCHEDULE_TYPE_BASIC_PREV=2,
		SCHEDULE_TYPE_HIST=9,
		SCHEDULE_TYPE_BASIC_BACKUP=10,
		SCHEDULE_TYPE_WORK_REQ=20,
	};
	SCHEDULE_TYPE  m_nType;
	int  m_nSectionID;
	int  m_nShiftID;
	TIME m_tStartTime;
	TIME m_tEndTime;
	int  m_nParentSectionID;

	BYTE m_nAutoMove;
	int m_nHistoryLen;
	int	m_nTotelLen;

public:
	CTG_ScheduleInfo();
	CTG_ScheduleInfo(SCHEDULE_TYPE type,int con_id,int shift_id);
	CTG_ScheduleInfo(const CTG_ScheduleInfo &src);
    CTG_ScheduleInfo& operator= (const CTG_ScheduleInfo &src);
private:
	BOOL  operator== (const CTG_ScheduleInfo &src);
public:	
	int   GetShiftNo();
	void  ShiftSchedule(bool bIgnore);
	bool  IsEqualScheduleInfo(const CTG_ScheduleInfo &src);
	void  GetViewScopeRange(TIME &start,TIME &end) const;
	void  GetTermAcceptSchdTimeRange(TIME& nStartTime, TIME& nEndTime) const; 	// 20171218克服索引重复

	const char  *GetShiftName();
	void  GetShiftManagerRange(TIME &start,TIME &end);

	static long Get_CurrentShiftid();
	static long Get_PreShiftid(long CurShiftid);
	static long Get_NextShiftid(long CurShiftid);
	static long Get_ShiftDate(long shift_id);
	
	static TIME GetStartShiftTime(TIME ltime);
	static CTG_ScheduleInfo::SCHEDULE_TYPE  ScheduleTypeConvert(int type);
	
		
	
};


class CTG_TrainEngineInfo{
public:
	TG_TrainEngineInfo m_data;
 public:
	CTG_TrainEngineInfo();
	CTG_TrainEngineInfo(const CTG_TrainEngineInfo &src);
    CTG_TrainEngineInfo& operator= (const CTG_TrainEngineInfo &src);
	BOOL operator== (const CTG_TrainEngineInfo &src) const;
	BYTE GetEngineType() const {return (m_data.engine_status  & 0xFF);}
	BYTE IsElecTypeEngine() const { return  ((m_data.engine_status  & 0x07)?TRUE:FALSE);}
	BYTE IsMainTractEngine() const { return  (((m_data.engine_status>>3) & 0x03)==0 ? TRUE:FALSE);}
	int  GetAttachRecord() const {return m_data.attach_sta;} 
	int  GetUnAttachRecord() const {return m_data.unattach_sta;}
	const char *GetEngineName() const;
	const char *GetEngineID() const;
};

class TG_TrainRecordExpand
{
public:
	TG_TrainRecordExpand();
	TG_TrainRecordExpand(const TG_TrainRecordExpand& rhs);
	TG_TrainRecordExpand& operator=(const TG_TrainRecordExpand& rhs);
	bool IsSame(const TG_TrainRecordExpand& rhs) const;
	void Init();
		
public:
	IDWORD	    work_flag;			// 车站作业
	IDWORD      oper_flag;			// 操作卡控
	IDWORD      important_flag;		// 重要卡控

	CString	    oper_info;
	CString     note_text;
};

#define  STATION_TXT_INFO_TYPE_SHUAIGUA  0
#define  STATION_TXT_INFO_TYPE_OPMS      5

class CTG_TrainRecord{
public:
	int rec_index;
	TG_TrainRecord  m_data;
	ENTRY tdms_entry,tdms_exit;
	TG_TrainRecordExpand m_record_expand;
	TG_TrainAbsStop m_stop_out_of_signal;
	vector<TG_TrainAbsStop> m_listAbsStop;
	unsigned long  m_nAlarmStatus; // ALARM_FLAG_XXX

	string    m_strStationText;       // 车站信息XML文档

	string    m_strAbsText;
	string    m_strAlarmText;
	string    m_strNoteText;
	BOOL      m_modify_abs_time;

private:
	CStringArray m_stplan_warning;
	CWordArray m_stplan_warning_id;
 public:
	CTG_TrainRecord();
	CTG_TrainRecord(const CTG_TrainRecord &src);
    CTG_TrainRecord& operator= (const CTG_TrainRecord &src);
	BOOL operator== (const CTG_TrainRecord &src0) const;
	void Init();
public:
	bool HaveWarnInfo();
	void ClearStplanWarnInfo();
	void AddStplanWarnInfo(WORD id, CString& text);
	bool GetStplanWarnInfo(int index, WORD& id, CString& text);

	ENTRY_ID  GetEntryNumber(TRAIN_IN_OUT in_out) const;
	STATION_NO GetStationNo() const {return m_data.station;}
	BOOL   IfInTimeScope(TIME begin_time, TIME end_time) const;
	bool   BasicTrainIfInTimeScope(int banbie) const;
	int    GetIndex() const {return rec_index;}
	bool   IsOpmsSetSationText();
	void   SetOpmsSationTextState(const char *str);
	const  char *GetOpmsSationTextState() const;
	const  char *GetStationText(int flag=STATION_TXT_INFO_TYPE_SHUAIGUA) const;
	void   SetStationText(const char *str,int flag=STATION_TXT_INFO_TYPE_SHUAIGUA);
	const  char *GetAbsText() const {return m_strAbsText.c_str();}
	const  char *GetAlarmText() const {return m_strAlarmText.c_str();}
	int    GetAbsStopCount() const {return m_listAbsStop.size();}
	TG_TrainAbsStop *GetAbsStop(int index)
	{
		if(index>=0 && index<=GetAbsStopCount())
			return &m_listAbsStop[index];
		else
			return NULL;
	}
	BOOL      IsSideConfirmed() const {return TG_IsSideConfirmed(m_data)?TRUE:FALSE;}
	ENTRY_ID  GetEntry() const {return m_data.entry;}
	ENTRY_ID  GetExit() const {return m_data.exit;}
	TIME      GetArriveTime() const {return m_data.arrive;}
	TIME      GetDepartTime() const {return m_data.depart;}
	BOOL      IsStartRec() const {return TG_IsFlagStart(m_data.flag)?TRUE:FALSE;}
	BOOL      IsTerminalRec() const {return TG_IsFlagTerminal(m_data.flag)?TRUE:FALSE;}
	void      SetGiveoutRec() { TG_SetFlagGiveout(m_data.flag); }
	void      SetAcceptRec() { TG_SetFlagAccept(m_data.flag); }
	const char *GetArriveTID() const {return m_data.arrive_train_id;}
	const char *GetDepartTID() const {return m_data.depart_train_id;}
	BOOL      IsPassRec()  const {return (m_data.arrive==m_data.depart)?TRUE:FALSE;}
	BOOL      IsActualArriveTimeRecord() const {return TG_IsArriveActual(m_data)?TRUE:FALSE;}
	BOOL      IsActualDepartTimeRecord() const {return TG_IsDepartActual(m_data)?TRUE:FALSE;}
	BOOL      HasWarningAlarm() const {return (m_nAlarmStatus & 0xFFFF0000)?TRUE:FALSE;}
	BOOL      HasErrorAlarm() const {return (m_nAlarmStatus & 0x0000FFFF)?TRUE:FALSE;}
	TIME      GetPlanSendTime() const {return m_data.send_plan_time;} 
	void      SetPlanSendTime(TIME x) {m_data.send_plan_time=x;} 
	int       GetArriveTGIndex() const;
	int       GetDepartTGIndex() const;
	void      SetArriveTGIndex(int va);
	void      SetDepartTGIndex(int va);

	int       GetMilitaryRank() const { return TG_IsMilitaryRank(m_data)?1:0;}
	void      SetMilitaryRank(int va) { if(va) TG_SetMilitaryRank(m_data);else TG_SetNoMilitaryRank(m_data);}
	BOOL      HasImportantFlag() const;

	BYTE      GetArriveSpace()  const { return TG_GetArriveSpace(m_data);}
	BYTE      GetDepartSpace() const { return TG_GetDepartSpace(m_data);}
	void      SetArriveSpace(BYTE space) { TG_SetArriveSpace(m_data,  space) ;}
	void      SetDepartSpace(BYTE space) { TG_SetDepartSpace(m_data,  space) ;}
	BYTE     IsStationSpace() const { return TG_IsSpace(m_data);}
	void      SetRecordKK(const TG_TrainOPMSRecordInfo& opms);
	bool      IsLockSide() const { return TG_IsForceSide(m_data)? true:false;}
	void      LockSide()  {  TG_SetForceSide(m_data);}
	void      UnLockSide() {  TG_ReSetForceSide(m_data);}
};	


// 注意,目前暂时仅支持总共不超过32个同步对象,包括MMI,SERVER,DATABSE,MEMORY 4个固定的
#define  SYCFLAG_NULL           -1
#define  SYCFLAG_MAX            31
#define  SYCFLAG_ALL            SYCFLAG_MAX+1
#define  SYCFLAG_MEMORY         SYCFLAG_MAX+2
//#define  SYCFLAG_MMI            SYCFLAG_MAX	
//#define  SYCFLAG_SERVER         SYCFLAG_MAX-1	
//#define  SYCFLAG_DATABASE        SYCFLAG_MAX-2	
//#define  SYCFLAG_MEMORY         SYCFLAG_MAX-3

#define CHECK_VALID_SYNCFLAG(x)  (x>=-1 && x<=SYCFLAG_MAX+1)
#define IS_READ_ONLY_BASIC_SCHD(x)  ((CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC==x) || (CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_BACKUP==x))
#define IS_BASIC_SCHD(x)  ((CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC==x) || (CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_PREV==x) || (CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_BACKUP==x))
// 0 - 31 表示其他32个接收者的 NOSYNC查询
enum TG_OBJECT_TYPE{TG_OBJ_NULL,TG_OBJ_SCHD,TG_OBJ_TRAIN,TG_OBJ_MARK,TG_OBJ_TRAININFO,TG_OBJ_TRAINRECORDS,TG_OBJ_ENGINEINFO};

class CTG_LogicObject{

protected:
	BOOL       m_flagDelete;      // 被删除的对象
	time_t     m_tSyncStartTime[SYCFLAG_MAX+1];     // 非0表示正在同步中，启动同步的时间
	CBitStatus m_flagNeedToSync;  // 32 BIT 表示 0-31个接受者的同步请求,是否应该同步
	CBitStatus m_flagSyncStatus;  // 32 BIT 表示 0-31个接受者的同步状态
	unsigned long  m_nAlarmStatus; // ALARM_FLAG_XXX, 可能存在的报警状态
public:
	CTG_LogicObject();
	CTG_LogicObject& operator= (const CTG_LogicObject &src);
	virtual TG_OBJECT_TYPE GetObjectType() const{return TG_OBJ_NULL;}
	virtual void  StartSyncing(int sync_flag);
	virtual void  StopSyncing(int sync_flag);
	virtual BOOL  IsSyncing(int sync_flag);
	virtual BOOL    CheckSyncStatus(int sync_flag) const;
	virtual void    SetSyncStatus(int sync_flag,BOOL setva);
	virtual void    SetNeedSyncFlag(int sync_flag,BOOL setva);
	virtual void    SetSyncStatusX(int sync_flag,BOOL setva); // 设置指定，其他设置为相反
	virtual BOOL    IsDeletedObject() const { return m_flagDelete;}
	virtual void    SetDeletedFlag(BOOL flag=TRUE) {  m_flagDelete=flag;}
	virtual unsigned long GetSyncStatus() const { return m_flagSyncStatus.GetLongValue();}
	virtual BOOL    CheckMultiSyncStatus(unsigned  long multi_sync_flag) const;
	void   SetAlarmStatus(unsigned long statusbit,BOOL value);
};

#define  MARK_TXT_INFO_TYPE_INSIDE   0
class CTG_SingleMark:public CTG_LogicObject{
  public:
	 TRAIN_INDEX        m_nBlockIndex;
	 struct TG_ScheduleBlock m_data;
	string       m_info;
 	CTG_SingleMark();
	CTG_SingleMark(const CTG_SingleMark &src);
    CTG_SingleMark& operator= (const CTG_SingleMark &src);
	BOOL operator== (const CTG_SingleMark &src) const;

	TRAIN_INDEX  GetBlockIndex() const {return m_nBlockIndex;}
	TRAIN_INDEX  GetBasicIndex() const {return m_data.basic_index;}
	TRAIN_INDEX  GetGPCIndex() const {return  m_data.gpc_index;}
	void   SetGPCIndex(TRAIN_INDEX index) {  m_data.gpc_index=index;}
	TRAIN_INDEX  GetShiftIndex() const {return m_data.shift_index;}  
	void  SetBlockIndex(TRAIN_INDEX index) {m_nBlockIndex=index;}
	BOOL IsLocalCreated()const ;
	BOOL   IfInTimeScope(TIME stime, TIME etime) const;
	virtual TG_OBJECT_TYPE GetObjectType() const{return TG_OBJ_MARK;}
	int  GetMarkDirection() const {return (m_data.line & 0x03);}
	BOOL  MatchStationSide(int station1,int side) const ;
	BOOL  MatchStation(int station1,int station2) const ;
	BOOL  MatchAbsNumber(int absnum) const ;
	int   GetMarkType() const  {return m_data.type;}; 
	BOOL  IsStationMarkType() const;
	BOOL  IsAbsMarkType() const;
	TIME  GetStartTime() const {return m_data.start_time;}
	TIME  GetEndTime() const {return m_data.end_time;} 
	const char *GetMarkID() const;
	const char *GetMarkInfo(int flag=MARK_TXT_INFO_TYPE_INSIDE) const; 
	void  SetMarkInfo(const char *info,int flag=MARK_TXT_INFO_TYPE_INSIDE); 
	TIME  GetUpdateTime() const {return m_data.update_time;} 
	void  SetUpdateTime(TIME x) {m_data.update_time=x;} 
	const char *GetShiftIndexString() const {return m_data.shift_string;} 
	int   GetStation() const;
	int   GetStation2() const;
protected:
	BOOL   m_bSelfUpdate;
public:
	void   SetSelfUpdate(BOOL va) {m_bSelfUpdate=va;}
	BOOL   IsSelfUpdate() const  {return m_bSelfUpdate;}
};

class CTG_TrainKediaoInfo
{
public:
	short type;   //类型 1:客调 2：特调
	CString content; //客调内容
public:
	TRAIN_INDEX gtid; 
	int  msgid;
public:
	CTG_TrainKediaoInfo();
	CTG_TrainKediaoInfo(const CTG_TrainKediaoInfo &src);
    CTG_TrainKediaoInfo& operator= (const CTG_TrainKediaoInfo &src);
	bool IsCoreSame(const CTG_TrainKediaoInfo &src) const;

	void Init() {type=0;content="";gtid=0;msgid=0;}
	TRAIN_INDEX GetShiftIndex() { return gtid;}
};

class CTG_TrainYingyeList
{
public:
	TRAIN_INDEX gtid; 
	int  msgid;
	vector<TG_TrainYingyeInfo> m_listRecords;
public:
	CTG_TrainYingyeList();
	CTG_TrainYingyeList(const CTG_TrainYingyeList &src);
    CTG_TrainYingyeList& operator= (const CTG_TrainYingyeList &src);
	bool IsCoreSame(const CTG_TrainYingyeList &src) const;

	TRAIN_INDEX GetShiftIndex() { return gtid;}
public:
	void Init() {m_listRecords.clear();}
	int GetCount() const {return m_listRecords.size();}
	TG_TrainYingyeInfo* GetRecord(int index);
	void AddRecord(const TG_TrainYingyeInfo& rd);
	const TG_TrainYingyeInfo* GetRecord_Const(int index) const;
};

class CTG_TrainCrewList
{
public:
	TRAIN_INDEX gtid; 
	int  msgid;
public:
	CString trainid;
	CString emuno;
	vector<TG_TrainCrewInfo> m_listRecords;
public:
	CTG_TrainCrewList();
	CTG_TrainCrewList(const CTG_TrainCrewList &src);
    CTG_TrainCrewList& operator= (const CTG_TrainCrewList &src);
	bool IsCoreSame(const CTG_TrainCrewList &src) const;
	TRAIN_INDEX GetShiftIndex() { return gtid;}
public:
	void Init() {m_listRecords.clear();}
	int GetCount() const {return m_listRecords.size();}
	TG_TrainCrewInfo* GetRecord(int index);
	const TG_TrainCrewInfo* GetRecord_Const(int index) const;
	void AddRecord(const TG_TrainCrewInfo& rd);
	CString toString() const;
	CString toString2() const;
};
class CTG_TrainTicketList
{
public:
	TRAIN_INDEX gtid; 
	int  msgid;
public: 
	CString trainid;
	vector<TG_TrainTicketInfo> m_listRecords;
public:
	CTG_TrainTicketList();
	CTG_TrainTicketList(const CTG_TrainTicketList &src);
    CTG_TrainTicketList& operator= (const CTG_TrainTicketList &src);
	bool IsCoreSame(const CTG_TrainTicketList &src) const;
	TRAIN_INDEX GetShiftIndex() { return gtid;}
public:
	void Init() {m_listRecords.clear();}
	int GetCount() const{return m_listRecords.size();}
	TG_TrainTicketInfo* GetRecord(int index);
	const TG_TrainTicketInfo* GetRecord_Const(int index) const;
	void AddRecord(const TG_TrainTicketInfo& rd);
};
class CTG_EngineCrewList
{
public:
	TRAIN_INDEX gtid; 
	int  msgid;
public:
	CString trainid;
	vector<TG_EngineCrewInfo> m_listRecords;
public:
	CTG_EngineCrewList();
	CTG_EngineCrewList(const CTG_EngineCrewList &src);
    CTG_EngineCrewList& operator= (const CTG_EngineCrewList &src);
	bool IsCoreSame(const CTG_EngineCrewList &src) const;
	TRAIN_INDEX GetShiftIndex() { return gtid;}
public:
	void Init() {m_listRecords.clear();}
	int GetCount() const{return m_listRecords.size();}
    TG_EngineCrewInfo* GetRecord(int index);
	const TG_EngineCrewInfo* GetRecord_Const(int index) const;
	void AddRecord(const TG_EngineCrewInfo& rd);
};
class CTG_TrainRelation
{
public:
	TRAIN_INDEX		   prev_train_index;	//前趟车索引
	TRAIN_INDEX		   prev_basic_index;    //前趟车基本图ID
	TRAIN_INDEX		   prev_shift_index;	//前趟车全局ID
	TRAIN_INDEX		   next_shift_index;	//后趟车全局ID
	STATION_NO		   prev_dest_sta;		//前一个方向站 
	STATION_NO		   next_dest_sta;		//后一个方向站 
	CString            czh_no_str;
public:
	CTG_TrainRelation();
	CTG_TrainRelation(const CTG_TrainRelation &src);
    CTG_TrainRelation& operator= (const CTG_TrainRelation &src);
	bool IsCoreSame(const CTG_TrainRelation &src) const;
	void Init();
};

class TG_TrainInfoExpand
{
private:
	// 交路,客调,营业,乘务,客票和机务
	CTG_TrainRelation   m_cRelation;
	CTG_TrainKediaoInfo m_cKediao;
	CTG_TrainYingyeList m_cYingyeList;
	CTG_TrainCrewList   m_cCrewList;
	CTG_TrainTicketList m_cTicketList;
	CTG_EngineCrewList  m_cEngineCrewList;

public:
	TG_TrainInfoExpand();
	TG_TrainInfoExpand(const TG_TrainInfoExpand &src);
	TG_TrainInfoExpand& operator= (const TG_TrainInfoExpand &src);
	
public:
	void Init();
	bool IsSame(const TG_TrainInfoExpand &src) const;

public:
	void UpdateTrainRelation(const CTG_TrainRelation& relation);
	void UpdateTrainKediao(const CTG_TrainKediaoInfo& kd);
	void UpdateTrainYingye(const CTG_TrainYingyeList& yingye);
	void UpdateTrainCrew(const CTG_TrainCrewList& crew);
	void UpdateTrainTicket(const CTG_TrainTicketList& ticket);
	void UpdateTrainEngineCrew(const CTG_EngineCrewList& engine);

public:
	void  SetPrevTrainIndex(TRAIN_INDEX index) {m_cRelation.prev_train_index=index;}
	void  SetPrevShiftTrainIndex(TRAIN_INDEX index) {m_cRelation.prev_shift_index=index;}
	void  SetPrevBasicTrainIndex(TRAIN_INDEX index) {m_cRelation.prev_basic_index=index;}

public:
	const CTG_TrainRelation* GetTrainRelation_Const() const { return &m_cRelation;}
	const CTG_TrainKediaoInfo* GetTrainKediao_Const() const { return &m_cKediao;}
	const CTG_TrainCrewList* GetTrainCrewList_Const() const { return &m_cCrewList;}
	const CTG_TrainYingyeList* GetTrainYYList_Const() const { return &m_cYingyeList;}
	const CTG_EngineCrewList* GetTrainEngineCrewList_Const() const { return &m_cEngineCrewList;}
	const CTG_TrainTicketList* GetTrainTicketList_Const() const { return &m_cTicketList;}

public:
	bool IsTrainKediaoSame(const CTG_TrainKediaoInfo& kd) const;
	bool IsTrainYingyeSame(const CTG_TrainYingyeList& yingye) const;
	bool IsTrainCrewSame(const CTG_TrainCrewList& crew) const;
	bool IsTrainTicketSame(const CTG_TrainTicketList& ticket) const;
	bool IsTrainEngineCrewSame(const CTG_EngineCrewList& engine) const;
};

class CTG_TrainInfo:public CTG_LogicObject{
 public:
	TG_TrainInfo m_data;
	TG_TrainInfoExpand m_info_expand;
	string       m_info;

public:
	CTG_TrainInfo();
	CTG_TrainInfo(const CTG_TrainInfo &src);
    CTG_TrainInfo& operator= (const CTG_TrainInfo &src);
	BOOL operator== (const CTG_TrainInfo &src) const; 
	BOOL IsSecretTrain() const {return IsConsoleTrainSecret(m_data)?TRUE:FALSE;}
	virtual TG_OBJECT_TYPE GetObjectType() const {return TG_OBJ_TRAININFO;}

	bool IsTrainInfoExpandSame(const CTG_TrainInfo& expand) const;
};

class CTG_TrainRecordList:public CTG_LogicObject{
public:
	vector<CTG_TrainRecord>   m_listRecords;
 public:
	CTG_TrainRecordList();
	CTG_TrainRecordList(const CTG_TrainRecordList &src);
    CTG_TrainRecordList& operator= (const CTG_TrainRecordList &src);
	BOOL operator== (const CTG_TrainRecordList &src) const;
	int  GetRecordCount() const  {return m_listRecords.size();}
	CTG_TrainRecord& operator[](int index) {return m_listRecords[index];};
	const CTG_TrainRecord& operator[](int index) const {return m_listRecords[index];};
	void  PurgeRecords() {m_listRecords.clear();}  
	void  AddRecord(const CTG_TrainRecord &rec) {m_listRecords.push_back(rec);} 
	void  DeleteRecord(int index);
	int   GetRecordByStation(int station) const;
	virtual TG_OBJECT_TYPE GetObjectType() const {return TG_OBJ_TRAINRECORDS;}
};

class CTG_TrainEngineList:public CTG_LogicObject{
public:
	vector<CTG_TrainEngineInfo>   m_listRecords;
 public:
	CTG_TrainEngineList();
	CTG_TrainEngineList(const CTG_TrainEngineList &src);
    CTG_TrainEngineList& operator= (const CTG_TrainEngineList &src);
	BOOL operator== (const CTG_TrainEngineList &src) const;
	virtual TG_OBJECT_TYPE GetObjectType() const {return TG_OBJ_ENGINEINFO;}
	int  GetRecordCount() const {return m_listRecords.size();} 
	CTG_TrainEngineInfo& operator[](int index) {return m_listRecords[index];};
	const CTG_TrainEngineInfo& operator[](int index) const {return m_listRecords[index];};
	
	void  PurgeRecords() {m_listRecords.clear();}
	void  AddRecord(const CTG_TrainEngineInfo &rec) {m_listRecords.push_back(rec);}

};

#define  TRAIN_TXT_INFO_TYPE_IMPORTANT   0
#define  TRAIN_TXT_INFO_TYPE_SHUAIGUA    1
#define  TRAIN_TXT_INFO_TYPE_KEDIAO      2
#define  TRAIN_TXT_INFO_TYPE_YINGYE      3
#define  TRAIN_TXT_INFO_TYPE_JUTEYUN     4
#define  TRAIN_TXT_INFO_TYPE_CREW        6

#define  STATION_TXT_INFO_TYPE_SHUAIGUA  0
#define  STATION_TXT_INFO_TYPE_OPMS      5

class CTG_SingleTrain:public CTG_LogicObject{
	friend class CTG_TrainScheduleBase;
public:
	enum {
		TRAIN_DIFF_RECORD_COUNT=0x0001,
		TRAIN_DIFF_STOP_PASS=0x0002,
		TRAIN_DIFF_RECORD_TIME=0x0004,
		TRAIN_DIFF_TRAIN_INFO=0x0008,
		TRAIN_DIFF_ENGINE_INFO=0x0010,
		TRAIN_DIFF_ONE_RECORD=0x0020,
		TRAIN_DIFF_RECORD_SIDE=0x0040
	};
protected:
	TRAIN_INDEX        m_nTrainIndex;

	CTG_TrainInfo                m_cTrainInfo;
	CTG_TrainEngineList          m_cEngineList;
    CTG_TrainRecordList          m_cRecordList;

	BOOL              m_bAdjusting;  // 临时标志，用于指示是否正在调整中

public:
	CString 		  m_strSource; // 临时标志, 用于邻台记录列车来源
	TRAIN_INDEX		  m_nCurrConTrainIndex; // 临时标志，用于邻台计划时存储本台列车的索引号
public:
	char			   m_cNextTrainName[24];  // 后后车次 --- 打印同车组勾连线
	char               m_cBeforeTrainName[24]; // 前车车次 --- 打印同车组勾连线
	TRAIN_INDEX		   m_nNextTrainIndex;  // 后后索引 --- 打印同车组勾连线
	TRAIN_INDEX		   m_nBeforeTrainIndex; // 前车索引 --- 打印同车组勾连线
	int				   m_nAddOrUpdate;	// 用于标记邻台计划交互时，列车是新增还是更新
	WORD			   m_nTipToDdy; // 用于提醒调度员
public:
/////////////// CTG_LogicObject Overload
	virtual BOOL    CheckSyncStatus(int sync_flag) const;
	virtual void    SetSyncStatus(int sync_flag,BOOL setva);
	virtual void    SetNeedSyncFlag(int sync_flag,BOOL setva);
	virtual void    SetSyncStatusX(int sync_flag,BOOL setva); // 设置指定，其他设置为相反
	virtual unsigned long GetSyncStatus() const;
	virtual BOOL    CheckMultiSyncStatus(unsigned  long multi_sync_flag) const;
	CString toString() const;
protected:
	BOOL           m_bSelfUpdate;
	int               m_curSelRecIdx;

public:
	void   SetSelfUpdate(BOOL va) {m_bSelfUpdate=va;}
	BOOL   IsSelfUpdate() const  {return m_bSelfUpdate;}
public:
	CTG_SingleTrain();
	CTG_SingleTrain(const CTG_SingleTrain &src);
	CTG_SingleTrain& operator= (const CTG_SingleTrain &src);
	BOOL operator== (const CTG_SingleTrain &src);
	virtual TG_OBJECT_TYPE GetObjectType() const{return TG_OBJ_TRAIN;}
	BOOL CompareCoreData(const CTG_SingleTrain &src);
	BOOL CompareCoreData(const CTG_TrainInfo &src0);  // 比较重要和运行图界面显示的成员
	BOOL CompareCoreData(const CTG_TrainRecordList &src0);  // 比较重要和运行图界面显示的成员
	~CTG_SingleTrain();

	CTG_TrainInfo *GetTrainInfo() {return &m_cTrainInfo;}
	CTG_TrainRecordList *GetRecordList() {return &m_cRecordList;}
	CTG_TrainEngineList *GetEngineList() {return &m_cEngineList;}
	
	const CTG_TrainInfo *GetTrainInfo_Const()  const {return &m_cTrainInfo;}
	const CTG_TrainRecordList *GetRecordList_Const()  const {return &m_cRecordList;}

	void  InitTrainExpand()  { return m_cTrainInfo.m_info_expand.Init();}
	TG_TrainInfoExpand* GetTrainInfoExpand() { return &(m_cTrainInfo.m_info_expand);}
	const TG_TrainInfoExpand* GetTrainInfoExpand_Const() const { return &(m_cTrainInfo.m_info_expand);}

	const CTG_TrainRelation* GetTrainRelation_Const() const { return m_cTrainInfo.m_info_expand.GetTrainRelation_Const();}
	
	const char *GetImportInfoText(int flag) const;

	TRAIN_INDEX GetTrainIndex() const { return m_nTrainIndex; }
	void SetTrainIndex(TRAIN_INDEX x) {  m_nTrainIndex = x;   }
	BOOL IsLocalCreated() const ;
	const char *GetTrainID() const;
	const char *GetEndTrainID() const;//yuzhu
	const char *GetAlarmInfo(int flag=0) const; // flag=0 ALL alarm
	const char *GetImportInfo(int flag=TRAIN_TXT_INFO_TYPE_IMPORTANT) const;
	void   SetImportInfo(const char *info,int flag=TRAIN_TXT_INFO_TYPE_IMPORTANT);
	void   SetTrainIDEx(const char *tid,BOOL orig_tid,int from_rec,BOOL from_arrive,int to_rec,BOOL to_arrive);
	void   SetTrainID(const char *tid);
	void   SetOrigTrainID(const char *tid);
	BOOL   MatchTrainID(const char *tid);
	BOOL   IsDownDirection() const;
	BOOL   IsEndDownDirection() const;//yuzhu
	BOOL   IsUpDirection() const;
	BOOL   IsEndUpDirection() const;//yuzhu
	BOOL   IsCRHTrain() const;
	BOOL   IsSpecialZ() const;
	BOOL   IsSecretTrain() const;
	TRAIN_INDEX  GetBasicTrainIndex() const {return m_cTrainInfo.m_data.basic_train_index;}
	TRAIN_INDEX  GetGPCTrainIndex() const {return m_cTrainInfo.m_data.gpc_train_index;}
	TRAIN_INDEX  GetShiftTrainIndex() const {return m_cTrainInfo.m_data.shift_train_index;}  

	TRAIN_INDEX GetPrevTrainIndex() const {return GetTrainRelation_Const()->prev_train_index;}
	TRAIN_INDEX GetPrevTrainShiftTrainIndex() const {return GetTrainRelation_Const()->prev_shift_index;}
	TRAIN_INDEX GetPrevTrainBasicTrainIndex() const {return GetTrainRelation_Const()->prev_basic_index;}

	void   SetGPCTrainIndex(TRAIN_INDEX index) {m_cTrainInfo.m_data.gpc_train_index=index;}
	void   SetShiftTrainIndex(TRAIN_INDEX index) {m_cTrainInfo.m_data.shift_train_index=index;}

	void   SetPrevTrainIndex(TRAIN_INDEX index) {m_cTrainInfo.m_info_expand.SetPrevTrainIndex(index);}
	void   SetPrevShiftTrainIndex(TRAIN_INDEX index) {m_cTrainInfo.m_info_expand.SetPrevShiftTrainIndex(index);}
	void   SetPrevBasicTrainIndex(TRAIN_INDEX index) {m_cTrainInfo.m_info_expand.SetPrevBasicTrainIndex(index);}
	
	BOOL   UpdateTrain(const CTG_SingleTrain &src,int source_flag);
	BOOL   UpdateTrainInfo(const CTG_TrainInfo &traininfo,int source_flag);
	BOOL   UpdateTrainRecords(const CTG_TrainRecordList &trainrecord,int source_flag); //NULL means clear!
	BOOL   UpdateEngineInfo(const CTG_TrainEngineList &trainengine,int source_flag); //NULL means clear!
	BOOL   ComparePrevAndUpateFlags(const CTG_SingleTrain &prev);  // 用于人工修改后，设置DELETE_FLAG等重要标志
	BOOL   IsValid() const;

	TIME   GetStartTime() const;
	TIME   GetEndTime() const;
	BOOL   IfInTimeScope(TIME stime, TIME etime, BOOL IsStart = FALSE) const;
 
///////////////////////// GET ////////////////////////
	int    GetTrainType() const {return m_cTrainInfo.m_data.type;}
	int    GetRunType() const {return m_cTrainInfo.m_data.run_type;}
	int    GetPriority() const {return m_cTrainInfo.m_data.priority;}
	int    GetTrainRecordCountWithDeleted() const { return m_cRecordList.GetRecordCount();} //// 512计划优化处理 
	int    GetTrainRecordCount() const;
	int    GetEngineCount() const {return m_cEngineList.GetRecordCount();}
	BOOL   IsPassengerTrain() const {return TG_IsPassengerTrainType(GetTrainType());}
	int    GetMilitaryRank(int station=-1) const;
	BOOL   IsElectricTrain(int station=-1) const; // 如果station==-1,则只要任一车站记录有电力标志，则为TRUE
	int    GetSpaceRank(int rec_index=-1) const; // 如果rec_index==-1,查找记录中最大的超限值
////////////////////////SET///////////////////////////////////////////////////
	void   SetElectricFlag(BOOL va,int fromstation=-1) const;  //如果station==-1,则整车
	void   SetSpaceRank(int  va, int fromindex,  bool include_arrive, bool bmodifyinfo) ; 
	void   SetTrainStSpace();
	void   SetMilitaryRank(int  va,int fromstation=-1) ; //如果station==-1,则整车,否则该站之后,含该站
	
	int  GetCurSelRecIndex()
	{
		return m_curSelRecIdx;
	} 
	void  SetCurSelRecIndex(int idx)
	{
		m_curSelRecIdx = idx;
	}

	void   SetFromToConId(CONID from_conid, CONID to_conid)
	{
		m_cTrainInfo.m_data.from_conid = from_conid;
		m_cTrainInfo.m_data.to_conid = to_conid;
	}

	void SetFromShift(BOOL va)
	{
		if(va)
			TG_SetTrainFromShift(m_cTrainInfo.m_data.status);
		else
			TG_ClearTrainFromShift(m_cTrainInfo.m_data.status);
	}

	

	void SetFJKTime(const CTG_TrainRecord& rec, int method)
	{
		m_cTrainInfo.m_data.sborder_sta = rec.GetStationNo();
		if (rec.IsStartRec())
		{
			m_cTrainInfo.m_data.sborder_actual_time = rec.m_data.depart;
			m_cTrainInfo.m_data.sborder_plan_time   = rec.m_data.plan_depart;
		}
		else
		{
			if(1 == method)
			{
				m_cTrainInfo.m_data.sborder_actual_time = rec.m_data.depart;
				m_cTrainInfo.m_data.sborder_plan_time   = rec.m_data.plan_depart;
			}
			else
			{
				m_cTrainInfo.m_data.sborder_actual_time = rec.m_data.arrive;
				m_cTrainInfo.m_data.sborder_plan_time   = rec.m_data.plan_arrive;
			}
		}
	}
	CTG_TrainRecord *GetTrainRecord(int index)
	{
		if(index<0 || index>=m_cRecordList.GetRecordCount())
			return NULL;
		return &m_cRecordList[index];
	}
	CTG_TrainEngineInfo *GetEngine(int index)
	{
		if(index<0 || index>=m_cEngineList.GetRecordCount())
			return NULL;
		return &m_cEngineList[index];
	}
	const CTG_TrainRecord *GetTrainRecordNoDelete_Const(int index) const
	{
		if(index<0 || index>=GetTrainRecordCount())
			return NULL;
		return &m_cRecordList[index];
	}

	CTG_TrainRecord *GetTrainFirstRecord()
	{
		int index=GetTrainRecordCount();
		if(index<=0)
			return NULL;
		return &m_cRecordList[0];
	}

	CTG_TrainRecord *GetTrainLastRecord()
	{
		int index=GetTrainRecordCount();
		if(index<=0)
			return NULL;
		return &m_cRecordList[index-1];
	}

	CTG_TrainRecord *GetTrainRecordNoDelete(int index)
	{
		if(index<0 || index>=GetTrainRecordCount())
			return NULL;
		return &m_cRecordList[index];
	}
	const CTG_TrainRecord *GetTrainRecord_Const(int index) const
	{
		if(index<0 || index>=m_cRecordList.GetRecordCount())
			return NULL;
		return &m_cRecordList[index];
	}
	const CTG_TrainEngineInfo *GetEngine_Const(int index) const
	{
		if(index<0 || index>=m_cEngineList.GetRecordCount())
			return NULL;
		return &m_cEngineList[index];
	}
	
	int    GetFirstPlanRecordIndex();

	CTG_TrainRecord *GetTrainRecordByStation(int stno);
	const CTG_TrainRecord *GetTrainRecordByStation_Const(int stno) const;
	CTG_TrainRecord *GetTrainRecordByStation_Reverse(int stno);
	const CTG_TrainRecord *GetTrainRecordByStation_Reverse_Const(int stno) const;

	CTG_TrainRecord *GetTrainRecordByRecIndex(int rec_index);
	const CTG_TrainRecord *GetTrainRecordByRecIndex_Const(int rec_index) const;

	const CTG_TrainRecord *GetTrainPreRecordByStation_Const(int stno) const; 
	TRAIN_MEET_RELATION GetMeetRelation(const CTG_TrainRecord& thisrec, const CTG_SingleTrain &another_train, int another_train_recindex) const;

	BOOL  OffsetScheduleTime(int nRecordNo, int nTimeOffset,BOOL  bActual, int nFlag);
	TIME  GetUpdateTime() const {return m_cTrainInfo.m_data.update_time;} 
	void  SetUpdateTime(TIME x) {m_cTrainInfo.m_data.update_time=x;} 
	const char *GetShiftIndexString() const {return m_cTrainInfo.m_data.shift_string;} 
	void   SetTrainRecordCount(int x);
	void   SetTrainRecordCountToDefault();
	BOOL  DirectChangeTrainLocoTypeFlag(BOOL elec,int fromstation=-1); // fromstation=-1
	int   GetStationElecLocoTypeByFlag(int station) const; // 根据flag判断是否电力牵引 ,返回1:电力 0:非电力, -1无信息
	int   GetStationElecLocoTypeByEngineInfo(int station) const; // 根据engine info判断是否电力牵引 ,返回1:电力 0:非电力, -1无信息
	int   GetRecordElecLocoTypeByEngineInfo(int index) const; // 根据engine info判断是否电力牵引 ,返回1:电力 0:非电力, -1无信息

	void  SetElecFlagByEngineInfo(); // 跟据engine info调整elec FLAG
	BOOL  HasSentPlan() const; // 是否已经下过计划

	int   HasActualRecord(int fromindex=0) const; // 是否包含实际点,return Actual Record index
	void  SetDefaultAttribByDefinedFlag(unsigned short  locotype); // 注意: locotype 是在train_type.xml中指定的值 0x01 电力 0x04 超限 0x08 军运 

	BOOL  OnlyNodeIndexWithoutExtryExit() const; // 只有节点没有出入口，来自TMIS基本图
	void  SetTrainStartEndFlag();

	void  SetAdjustingFlag(BOOL va) {m_bAdjusting=va;}
	BOOL  IsAdjusting() {return m_bAdjusting;}

	void  ModifyTrainRunTimeByMinute(int begin_sta_index, int end_sta_index, DWORD begin_time, DWORD end_time, int minute);
	void  ModifyTrainRunTimeByPercent(int begin_sta_index, int end_sta_index, DWORD begin_time, DWORD end_time, int percent);
};

struct TrainResultTmp
{
	int tm;
	CTG_SingleTrain* p;
	TrainResultTmp()
	{
		tm=0;
		p=NULL;
	}
};
class CTG_TrainPtrGroup{
	int m_trainptrs_cnt;
	CTG_SingleTrain* m_trainptrs[4096];
	std::vector<TrainResultTmp> m_trainpter_tmp_list;
public:
	CTG_TrainPtrGroup() { Clear();}
	void Clear()
	{
		m_trainpter_tmp_list.clear();
		m_trainptrs_cnt=0;
		memset(m_trainptrs, 0, sizeof(m_trainptrs));
	}
	void AddTrainPtr(CTG_SingleTrain *p);
	void AddTrainPtrNotOrder(CTG_SingleTrain *p);
	void AddTrainPtrByEndTmOrder(CTG_SingleTrain *p);
	int  GetTrainCount() {return  m_trainptrs_cnt;}
	CTG_SingleTrain *GetTrainPtr(int i) 
	{
		if(i<0 || i>=m_trainptrs_cnt)
			return NULL;
		return m_trainptrs[i];
	}
	void SetTrainsPtr();
};

class CTG_ObjectPtrGroup{
	vector<CTG_LogicObject *> m_ptrs;
public:
	
	void Clear(){m_ptrs.clear();}
	void AddPtr(CTG_LogicObject *p) {m_ptrs.push_back(p);}
	void RemovePtr(CTG_LogicObject *p)
	{
		vector<CTG_LogicObject *>::iterator iter=m_ptrs.begin();
		while(iter!=m_ptrs.end())
		{
			if(*iter==p)
				iter=m_ptrs.erase(iter);
			else
				iter++;
		}

	}
	void RemovePtr(int index)
	{
		vector<CTG_LogicObject *>::iterator iter=m_ptrs.begin();
		for(int i=0;i<index && iter!=m_ptrs.end();i++) iter++;
		if(iter!=m_ptrs.end())
		  m_ptrs.erase(iter);
	}

	int  GetCount() {return  m_ptrs.size();}
	CTG_LogicObject *GetPtr(int i) 
	{
		if(i<0 || i>=m_ptrs.size())
			return NULL;
		return m_ptrs[i];
	}

};

class  CTG_StationTrainStruct:public CTG_LogicObject{
  public:
		CTG_SingleTrain   *pTrain;
		TRAIN_INDEX       nTrainIndex;
		int               nRecIndex;
        TIME              tArriveTime,tDepartTime;
		ENTRY_ID          nEntry,nExit;
		CTG_StationTrainStruct()
		{
			pTrain=NULL;
			nTrainIndex=NULL_TRAIN_INDEX;
			nRecIndex=-1;
		    tArriveTime=NULL_TIME;
			tDepartTime=NULL_TIME;
			nEntry=NULL_ENTRY;
			nExit=NULL_ENTRY;
		}
	};

class CTG_StationTrainList{
public:
	
protected:
	typedef std::list<CTG_StationTrainStruct>  CListStationTrain;
	
	std::map<STATION_NO,CListStationTrain>  m_listStationTrains;
public:
	CListStationTrain *GetStationList(STATION_NO nStation);
	CListStationTrain *AddStationList(STATION_NO nStation);
	BOOL UpdateTrain(CTG_SingleTrain *pTrain);
	BOOL DeleteTrain(TRAIN_INDEX nTrainIndex);
	BOOL DeleteObsoleteTrains(TIME before_time);
	BOOL DeleteAllTrains();
	BOOL FindTrains(CTG_ObjectPtrGroup &result,STATION_NO station,TIME begin_time,TIME end_time,BOOL arrive,BOOL depart);
	BOOL FindMeetTrains(CTG_ObjectPtrGroup &result,int recIndex,const CTG_SingleTrain *pTrain);
};

#define MATCH_FLAG_TRAIN_ID  0x00000001
#define MATCH_FLAG_LOCAL_CREATED  0x00000002

class CTG_TrainScheduleBase:public IScheduleComm,public CTG_LogicObject{
protected:
	typedef std::map<TRAIN_INDEX, CTG_SingleTrain> CMapTrain;
	typedef std::map<TRAIN_INDEX, CTG_SingleMark> CMapMark;
	typedef CMapTrain::iterator IterMapTrain;
	typedef CMapMark::iterator IterMapMark;

	std::map<TRAIN_INDEX, ULONG> CHasStPlanTrainList;
	CTG_StationTrainList  m_staList; // 用于按照车站并按照时间顺序存储列车信息,加快搜索速度 

	CMapTrain   	m_mapTrains;
	CMapMark		m_mapMarks;

	CBitStatus       m_flagNeedToSync;  // 32 BIT 表示 0-31个接受者的同步请求,是否应该同步
	CTG_ScheduleInfo m_cScheduleInfo;
	
	WORD             m_nFileSaveFlag; 
private:
	TIME         m_nPollingBeginAddTime;
	TIME         m_nPollingBeginDelTime;
	///////// LOCK
#ifdef LOCK_ENABLED_WINDOWS
	CMutex        m_csCriticalSection;
#endif
	int        m_nUserSpecial; // 用于生成TRAININDEX的最高8位，如为0则默认为台号
public:
    CTG_TrainScheduleBase();
	void  SetUserSpeical(int v) {m_nUserSpecial=v;}
	virtual TG_OBJECT_TYPE GetObjectType() const{return TG_OBJ_SCHD;}
	///////// INIT ///////////
	void  SetScheduleInfo(const CTG_ScheduleInfo &info);
	void  GetScheduleInfo(CTG_ScheduleInfo &info) const;
	///////// GET
    CTG_SingleTrain *GetSingleTrain(TRAIN_INDEX train_index,BOOL with_deleted=FALSE,BOOL IsStart=FALSE);
	CTG_SingleTrain *GetSingleTrain(const TRAIN_NO train_no);
	/////////
	CTG_SingleTrain *GetSingleTrain(const TRAIN_NO trainno,TIME sttm,TIME endtm,BOOL null_shiftindex=TRUE,BOOL IsStart=FALSE);

	CTG_SingleMark  *GetSingleMark(TRAIN_INDEX block_index,BOOL with_deleted=FALSE);
	// 按照全局ID查找列车，车站不要缺省值
	CTG_SingleTrain *GetSingleTrainByShiftIndex(TRAIN_INDEX train_index,STATION_NO station);

	CTG_SingleTrain *GetPrevTrainByShiftIndex(TRAIN_INDEX train_index,STATION_NO station); // 勾连车组关系使用
	CTG_SingleTrain *GetNextTrainByShiftIndex(TRAIN_INDEX train_index,STATION_NO station); // 勾连车组关系使用

	CTG_SingleTrain *GetPrevTrainByBasicIndex(TRAIN_INDEX train_index,STATION_NO station,TIME tm, CString trainid); // 勾连车组关系使用
	CTG_SingleTrain *GetNextTrainByBasicIndex(TRAIN_INDEX train_index,STATION_NO station,TIME tm, CString trainid); // 勾连车组关系使用

	CTG_SingleTrain *GetNextTrainByPrevTrainIndex(TRAIN_INDEX train_index); // 勾连车组关系使用

	IterMapTrain   FirstTrain() {return m_mapTrains.begin();} 
	IterMapTrain   LastTrain() {return m_mapTrains.end();}
	IterMapMark    FirstMark() {return m_mapMarks.begin();} 
	IterMapMark    LastMark() {return m_mapMarks.end();}
    BOOL           Polling();
	void           SetNeedSyncFlag(int flag, BOOL va);
	BOOL           CheckAllSynced(const CTG_LogicObject *p) const;
public: /// ISCHEDULE_COMM INTERFACE ////////////////////////////
	virtual BOOL   Lock();
	virtual BOOL   UnLock();
	//////////////////////////
	virtual int    GetNoSyncTrainCount(int sync_flag,CString &info);
	virtual int    GetNoSyncMarkCount(int sync_flag,CString &info);
	virtual int    GetInterfaceFlag()
	{
		return SYCFLAG_MEMORY;
	}
	TRAIN_INDEX    GetMaxTrainIndex();
	TRAIN_INDEX    GetMaxBlockIndex();
	TRAIN_INDEX    GetAvaGPCIndex();
	TRAIN_INDEX    GetAvaTrainIndex();
    //////////////////////////// 输入 ////////////////////////////
	BOOL   SaveExpandTrain(const CTG_ScheduleInfo &schd,const CTG_SingleTrain*  ptrain);
	///////// UPDATE
	virtual BOOL   UpdateTrain(const CTG_ScheduleInfo &schd,const CTG_SingleTrain  &train,int source_flag);
	virtual BOOL   UpdateTrainInfo(const CTG_ScheduleInfo &schd,const CTG_SingleTrain  &train,int source_flag);
	virtual BOOL   UpdateTrainRecords(const CTG_ScheduleInfo &schd,const CTG_SingleTrain  &train,int source_flag);
	virtual BOOL   UpdateMark(const CTG_ScheduleInfo &schd,const CTG_SingleMark  &block_info,int source_flag);
	virtual BOOL   UpdateEngineInfo(const CTG_ScheduleInfo &schd,const CTG_SingleTrain  &train,int source_flag);
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
	BOOL CopyAllDataNew(const CTG_ScheduleInfo &schd,IScheduleComm *pOut,BOOL all_flag);
	virtual BOOL   CopyAllData(const CTG_ScheduleInfo &schd,IScheduleComm *pOut,BOOL all_flag);
	virtual CTG_SingleTrain *Find1NoSyncTrain(CTG_ScheduleInfo *schd,int source_flag,BOOL include_syncing=TRUE);
	virtual CTG_SingleMark *Find1NoSyncMark(CTG_ScheduleInfo *schd,int source_flag,BOOL include_syncing=TRUE);

	/////////////////////////// 数据更新整理/////////////
	BOOL   PollingForUpdate(BOOL all_flag,TIME curtm=0);
	
	/////////////////////////// FIND ////////////////////
	IterMapTrain FindMatchedTrain(const CTG_SingleTrain &srctrain, int flag,const IterMapTrain &prev);  // DEFINED AS MATCH_TRAIN_XXXXXX
	BOOL   FindTrainsForStplan(CTG_TrainPtrGroup &result,STATION_NO station,TIME begin_time,TIME end_time);
	BOOL   FindTrains(CTG_TrainPtrGroup &result,STATION_NO station=NULL_STATION,TIME begin_time=NULL_TIME,TIME end_time=NULL_TIME,BOOL include_deleted=FALSE,BOOL IsStart = FALSE);
	BOOL   FindTrainsNotOrder(CTG_TrainPtrGroup &result,STATION_NO station=NULL_STATION,TIME begin_time=NULL_TIME,TIME end_time=NULL_TIME,BOOL include_deleted=FALSE,BOOL IsStart = FALSE);	
	BOOL   FindTrainsByEndTm(CTG_TrainPtrGroup &result,STATION_NO station=NULL_STATION,TIME begin_time=NULL_TIME,TIME end_time=NULL_TIME,BOOL include_deleted=FALSE,BOOL IsStart = FALSE);
	BOOL   FindAllTrains(CTG_TrainPtrGroup &result);
	BOOL   FindMarks(CTG_ObjectPtrGroup &result,TIME begin_time=NULL_TIME,TIME end_time=NULL_TIME);
	BOOL   FindTrainsHasStation(CTG_TrainPtrGroup &result,STATION_NO station);
	BOOL   FindMarksByShiftString(CTG_ObjectPtrGroup &result,const char *planid);
	CTG_SingleMark  *GetSingleMarkByShiftString(const char *planid);
	////////////////// 统计 ///////////
	int    GetTrainCount1(int *passcnt, int *frecnt) const;
	int    GetMarkCount() const;
	int    GetTrainAndMarkCount() const;
	/////////////////////////////
	void   SetFileSaveFlag(WORD va){ m_nFileSaveFlag=va;}

	BOOL   FindTrainsByShiftIndex(CTG_TrainPtrGroup &result,TRAIN_INDEX shiftindex);
	BOOL   DirectDeleteTrainsInRange(TIME starttime,TIME endtime,BOOL onlystarttime,BOOL onlyendtime, int type=0);
	BOOL   DirectDeleteMarksInRange(TIME starttime,TIME endtime,BOOL onlystarttime,BOOL onlyendtime); // 目前用于接收日班计划时删除原有记录
	BOOL   DeleteDataInMemory(TIME starttime,TIME endtime,BOOL onlystarttime,BOOL onlyendtime); //删除内存中记录，不更新服务器和界面，目前用于换班
	
	void   DirectDeleteTrain(TRAIN_INDEX index);
	void   DirectDeleteBlock(TRAIN_INDEX index);
	void   GetHistoryData(TIME nBeginTm, std::vector<TRAIN_INDEX>& train_vec, std::vector<TRAIN_INDEX>& block_vec);
	void   GetNewlyData(TIME nEndTm, std::vector<TRAIN_INDEX>& train_vec, std::vector<TRAIN_INDEX>& block_vec);
	
	CTG_SingleTrain* GetTrainForUpdateExtent(TRAIN_INDEX train_index);
public:
	CTG_SingleTrain * GetSingleTrainForNeib(TRAIN_INDEX train_index);  //邻台计划模式查找列车
	void   SetTrainFromOPMS(const CTG_ScheduleInfo &schd,const CTG_SingleTrain  &train); //邻台计划设置列车来源于日班计划
	BOOL   FindPessangerTrains(CTG_TrainPtrGroup &result);

public:
	void  SaveTrainExtent(const CTG_TrainExtent& trainExtent);
	bool  SetTrainExtent(CTG_SingleTrain  &train) const;
	void  FinishSetTrainExtent();
private:
	CTG_TrainExtent mTrainExtent;

// 以下接口和m_staList相关，即按照车站索引的列车数据
public:
	BOOL    FindTrainsByStation(CTG_ObjectPtrGroup &result,STATION_NO station,TIME begin_time,TIME end_time,BOOL arrive,BOOL depart);
	CTG_StationTrainList *GetStationTrainList() {return &m_staList;}

	BOOL   IsTrainInSendPlanList(CTG_SingleTrain *pTrain);
	void   AddTrainInSendPlanList(TRAIN_INDEX trainindex);
	void   RemoveTrainInSendPlanList();

	int GetAllTrainCount();
	int GetAllMarkCount();

	BOOL   FindTrainsForBasicStplan(CTG_TrainPtrGroup &result,STATION_NO station);
	CTG_SingleTrain   *FindTrainsByStationAndTrainNo(STATION_NO station, const char *train_no);
};

struct TmpRecord
{
	int station;
	int entry;
	int exit;
	int limit_speed;
	int in_index;
	int out_index;
	TmpRecord()
	{
		station=0;
		entry=0;
		exit=0;
		limit_speed=0;
		in_index=-1;
		out_index=-1;
	}
};
// 下面定义临时列车为了出入口计算
class TrainForExitEntry
{
private:
	std::vector<TmpRecord> rec_list;

private:
	TrainForExitEntry(const TrainForExitEntry &src);
	TrainForExitEntry& operator= (const TrainForExitEntry &src);
	
public:
	TrainForExitEntry(){rec_list.clear();}
	~TrainForExitEntry(){rec_list.clear();}
	
	const TmpRecord *GetTrainRecord(int index) const
	{
		if(index<0 || index>=rec_list.size())
			return NULL;
		return &rec_list[index];
	}

	int GetTrainRecordCount() const
	{
		return rec_list.size();
	}

	void AddRecord(TmpRecord& rd)
	{
		rec_list.push_back(rd);
	}
};

bool   SetTrainExitEntry(CTG_SingleTrain &srctrain, const TrainForExitEntry& train);

#endif /** THE END **/
