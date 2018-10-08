#pragma once
#include <vector>
#include "tg_schedule_base.h"

#define MAX_MPLAN_NODE_NUM  256
#define MAX_MPLAN_MARK_NUM  512
#define MAX_MPLAN_DCMD_NUM  64

/////////////////////////////////////////////////////////////
class CTD_TrainInfo
{
public:
	CTD_TrainInfo();
	CTD_TrainInfo(const CTD_TrainInfo &src);
	CTD_TrainInfo& operator= (const CTD_TrainInfo &src);

private:
	void SetValue(const CTD_TrainInfo &src);

public:
	int     SCHTYP; // 所属运行图类型
	int     YLFG;  //0: 不存在 1: 存在
	int     LYLFG; //0: 不在本台 1: 本台
	int     TRAINTYPE;
	DWORD   GTID;  // *
	CString TNO; // *
	int     IFLAG; //1：始发/0：转入
	int     OFLAG; //0：转出/1：运休
	CString STST; // 始发站 *
	CString ENDST; //终到站 *
	CString FROMST; //接入前一站
	CString TOST;   // 交出后一站
	time_t LSTTM; //本台开始时间
	time_t LEDTM; //本台结束时间
	time_t STTM;  //始发时间 *
	time_t FJTM; // 分界口接入时间 *
	time_t FJTDTM; // 分界口接入时间 *
	int    m_nStationCount;
	int    SEQ;  // 当前序号
	int    COUNT; // 总数

	// 上海局新增 2014.4.12
	DWORD   RTID;
	CString EMU;
	DWORD   DGID;
	int		HALT;
};

class CTD_TrainRecordInfo
{
public:
	CTD_TrainRecordInfo();
	CTD_TrainRecordInfo(const CTD_TrainRecordInfo &src);
	CTD_TrainRecordInfo& operator= (const CTD_TrainRecordInfo &src);

private:
	void SetValue(const CTD_TrainRecordInfo &src);

public:
	int     STID;
	int     NODESEQ; // 节点顺序
	CString STNM; //站名
	int     ARND; //到达节点
	int     DPND; //出发节点
	int     ENTRY_LINE;//入口线别
	int     EXIT_LINE;//出口线别
	int     SIDENO;//股道号
	CString ARTNO; //到达车次
	CString DPTNO; //出发车次
	time_t  ARTM;  //到达时间
	time_t  DPTM;  //出发时间
	time_t  SARTM; //图定到达时间
	time_t  SDPTM; //图定出发时间
	int     BSTM;  //营业时间
	WORD    IMPINFO; //1 超限 2 重点警卫 4 毒品 8 军运
	int     STPINFO; //无/禁停/必须停 0/1/2
	int     METINFO[20]; //禁会车种
	int     m_nNotMeetCount;
	CString SGINFO;
	int     SGFLAG;
	//added by zhaoliang 20100529
    int     ARR_ENGINE_NUM;//机车数量
	CString ARR_ENGINE_NAME;//机车名称=机车类型+机车号
	int     DEP_ENGINE_NUM;
	CString DEP_ENGINE_NAME;

	// 上海局新增 2014.4.12
	int		OPERFLAG;
	CString OPERINFO;
	int     WORKFLAG;
	time_t  CWCL;
	// 济南局要求增加 重点事项 2017.01.07
	CString NOTE_TEXT; 
};

class CTD_Train
{
public:
	CTD_Train();
	CTD_Train(const CTD_Train &src);
	CTD_Train& operator= (const CTD_Train &src);

public:
	TRAIN_INDEX GetTrainShiftIndex() {return m_TDInfo.GTID;}
	void SetTrainShiftIndex(TRAIN_INDEX index) { m_TDInfo.GTID=index;}
	CTD_TrainRecordInfo* GetTrainTDRecord(WORD idx);
	int GetTrainTDRecordCount() { return m_TDRecordList.size(); }
	void PurgeTrainRecord() { m_TDRecordList.clear();}
public:
	CTD_TrainInfo                m_TDInfo;
	CTG_TrainEngineList          m_cEngineList;
	vector<CTD_TrainRecordInfo>  m_TDRecordList;
};


void SumZhongChe(const char *dir,int ld);
void SumQingChe(const char *type);
BOOL TD_BZGenHeaderXML(CString &hdr,const char *xml,const char *node);

BOOL TD_LoadTrainFromXML(const CTG_ScheduleInfo& schdinfo,CTG_TrainScheduleBase *ptrainbase,CTG_SingleTrain &newtrain,const char *xml,const char *node,int *curindex,int *totalcount,int *pflag,time_t sttm,time_t endtm,int mode, CStringArray &train_alarm);
CTG_SingleTrain *TD_GetTrainInfoFromXML(CTG_TrainScheduleBase *ptrainbase,const char *xml,const char *node);
void TD_SetTrainTicketInfoFromXML(CTG_TrainScheduleBase *ptrainbase,const char *xml,const char *node);
BOOL TD_LoadBasicScheduleTrainFromXML(CTG_SingleTrain &newtrain,const char *xml,const char *node,int *curindex,int *totalcount,int *ptype, CStringArray& train_alarm); // 读取基本图车次信息
BOOL TD_LoadShiftScheduleTrainFromXML(const CTG_ScheduleInfo& schdinfo,CTG_SingleTrain &newtrain,const char *xml,const char *node,int *curindex,int *totalcount, int mode, CStringArray &train_alarm);

BOOL TD_LoadTrainFromXMLx(const CTG_ScheduleInfo& schdinfo,CTD_Train &tdtrain,const char *xml,const char *node,int *pflag);

BOOL TD_ImportRecordInfoFromTD(CTG_TrainRecord &ctcrecord,CTD_TrainRecordInfo &tdrecord,CTD_TrainInfo &info, bool bAcceptSide,CStringArray& list);
BOOL TD_ImportTrainInfoFromTD(CTG_SingleTrain &ctctrain,CTD_TrainInfo &tdtrain,int mode);

void SetTrainFirstStationEntry(CTD_TrainInfo& traininfo, CTG_SingleTrain &newtrain, int schd_type); //schd_type:1日班计划、调整图 2基本图 主要针对接入和交出
void SetTrainLastStationExit(CTD_TrainInfo& traininfo, CTG_SingleTrain &newtrain, int schd_type); //schd_type:1日班计划、调整图 2基本图 主要针对接入和交出

bool TD_GetKediaoInfoFromXML(CTG_TrainKediaoInfo& kd,const char *xml,const char *node);
bool TD_GetYingyeInfoFromXML(CTG_TrainYingyeList& yy,const char *xml,const char *node);
bool TD_GetTicketInfoFromXML(CTG_TrainTicketList& tk,const char *xml,const char *node);
bool TD_GetCrewInfoFromXML(CTG_TrainCrewList& tk,const char *xml,const char *node);
bool TD_LoadEngineCrewInfoFromXML(CTG_EngineCrewList& tk,const char *xml,const char *node, int* seq, int*count);

// 3. 施工符号
class CTD_MPLANMARK
{
public:
	CTD_MPLANMARK();
	CTD_MPLANMARK &operator=(const CTD_MPLANMARK &other);
	BOOL operator== (const CTD_MPLANMARK &src) const;
	
public:
	BOOL  SaveToXML(CString &dest,const char *node);
	BOOL  GenListInfoXML(CString &dest,const char *node);
	const char  *GetListTitle();
	
	BOOL     GenMark(CTG_SingleMark &mark) const;

public:
	DWORD    m_nGMID;
	int      m_nCTCType;
	int      m_nTDType;
	time_t   m_tStartTime,m_tEndTime;
	
	CString  PLANID,STST,EDST,OTHER,PLANCD;
	CString  REASON;
	int      STKM,STM,ETKM,ETM;
	int      LIM,DIR;
	CString  CONTENT;
	int      STTB, EDTB;
};

// 4. 施工维修
class CTD_MPLAN_SG_WX
{
public:
	CTD_MPLAN_SG_WX();
public:
	int m_con_id;
	int m_count, m_flag;
	CString DISID;
	CString PLANID,TM,DBH,MID,WORKPOS,LINENM,DIR,WORKKM;
	CString WORKTP,PJNM,UNITNM,LEADER,RANK,WKCON,SPLIM,LIMINFO;
	CString STTM,EDTM,TELID;
public:
	BOOL  LoadSG_WXFromTDXML(const char *xml,const char *node);
	BOOL  LoadSG_WXFromCADXML(const char *xml,const char *node, int conid);
	BOOL  GenMplan_SG_WX(const char *xml,const char *node);
	BOOL  GenViewInfoXML(CString &dest,const char *node);
	BOOL  SaveSG_WXToXML(CString &dest,const char *node);
};

class CTD_MPLANDCMD
{
public:
	CTD_MPLANDCMD();

public:
	BOOL SaveToXML(CString &dest,const char *node, long create_time, CString dcmdtype, CString dcmdsubtype);
	BOOL SaveDCmdToXML(CString &dest,const char *node);
public:
	int     m_nSectionID;

	CString PLANID; //计划ID
	CString CMDINDEX; // 内部标示
	CString CMDTP; // 命令类型

	CString  RECVUNIT; // 受令机构
	
	CString TOPIC; // 命令标题
	CString CONTENT; //命令正文
	CString PRNM;  // 项目名称

	CString SUBTP; // 命令子类型
	CString RECVER;
};

// 6. 施工兑现
class CMPLAN_STATE
{
public:
	CMPLAN_STATE()
	{
		m_nSectionID=0;
		m_nStatus=0;
	}
	CString DISID;
	int     m_nSectionID;
	CString PLANID;
	int     m_nStatus;
	CString STTM;
	CString EDTM;
};

// 7. 施工揭示
class CMPLAN_WKDES
{
public:
	CMPLAN_WKDES();

public:
	BOOL  LoadWKDESFromTDXML(const char *xml,const char *node);
	BOOL  LoadWKDESFromCADXML(const char *xml,const char *node, int conid);
	BOOL  SaveWKDESToXML(CString &dest,const char *node);
public:
	CString strDisID;
	int     m_nSectionID;

	CString PLANID;
	CString CMDID;
	CString CONTENT;
};

// 8. 施工总表
class CMPLAN_RSINFO
{
public:
	CMPLAN_RSINFO();
	
public:
	BOOL SaveRSINFOToXML(CString& dest, const char* node);

public:
	CString strDisID;
	int     m_nSectionID;

	CString PLANID;
	time_t  TM;
	CString TRNINFO;
	int     FLAG;

	CString PNM;
	CString WKDES;
};

class CTD_MPlanMidList
{
public:
	CTD_MPlanMidList();

public:
	BOOL  LoadRSINFOFromTDXML(const char *xml,const char *node, int conid);
	BOOL  LoadRSINFOFromCADXML(const char *xml,const char *node, int conid);
	BOOL  SaveRSINFOToXML(CString&xml, const char* token);
public:

	int m_nCount;
	CMPLAN_RSINFO m_list[MAX_MPLAN_NODE_NUM];
};

class CMPlanListState
{
public:
	CMPlanListState();
	
public:
	BOOL LoadMPlanStateFromCADXML(const char *xml,const char *node, int con_id);

public:
	int m_nCount;
	CMPLAN_STATE m_list[MAX_MPLAN_NODE_NUM];
};

class CTD_MPlanMarkList
{
public:
	CTD_MPlanMarkList();

public:
	BOOL  LoadMarkFromTDXML(const char *xml,const char *node);
	BOOL  LoadMarkFromCADXML(const char *xml,const char *node, int conid);
	BOOL  SaveXMLToString(CString&xml, const char* token);
public:
	long m_nSrccode;
	long m_nMsgid;

	int mark_con_id;
	CString strDisID;
	int m_nCount;
	CTD_MPLANMARK m_list[MAX_MPLAN_MARK_NUM];
};

class CTD_MPlanDCMDList
{
public:
	CTD_MPlanDCMDList();

public:
	BOOL  LoadDcmdFromTDXML(const char *xml,const char *node, int conid);
	BOOL  LoadDcmdFromCADXML(const char *xml,const char *node, int conid);
	BOOL  SaveDCmdsToXML(CString&xml, const char* token);
public:
	CString strDisID;
	int     m_nSectionID;

public:
	int m_nCount;
	CTD_MPLANDCMD m_list[MAX_MPLAN_DCMD_NUM];
};

class CTD_TrainCrew
{
public:
	CTD_TrainCrew();
	
public:
	CString GetCrewInfo();
public:
	DWORD GID;
	CString TICKET;
	CString CREW;
	CString TNO;
};

class CTD_TrainTicketList
{
public:
	CTD_TrainTicketList();

public:
	BOOL  LoadCrewFromTDXML(const char *xml,const char *node);

public:
	int m_nCount;
	CTD_TrainCrew m_list[512];
};

class CTD_CrewNotice
{
public:
	CTD_CrewNotice();

public:
	BOOL  LoadCrewNoticeFromTDXML(const char *xml,const char *node);

public:
	UINT m_sendcount;
	UINT m_count;
	ULONG m_gtid[512];
};

class CTD_BZInfo
{
public:
	CTD_BZInfo();
public:
	CString TYPE;
	CString CNO;
	int SWT;
	int LEN;
	int LD;
	int PBCNT;
	CString FIRSTNM;
	CString OBJNM;
};

class CTD_PlanInform
{
public:
   enum{MAX_NFORM_TRAIN_NUM=512};
   enum{SHIFT_PLAN_INFORM,KD_PLAN_INFORM,JH_PLAN_INFORM,STN_PLAN_INFORM,FTN_PLAN_INFORM, AD_PLAN_INFO} inform_type;
   enum{OPER_ADD_TRAIN=1,OPER_UPDATE_TRAIN=2,OPER_DELETE_TRAIN=3,OPER_NOEXSIT=4};
	DWORD         JHID;
	CString       JHTNM;
	CString		  MSGID;
	struct{
		BYTE      OPER; //0x01 增加  0x02 修改 0x03 删除
		DWORD     KDID;
		DWORD     GTID;
		CString   TNO;
		BOOL      LocalAdded;
		short     InfoReceivedFlag;
	}trains[MAX_NFORM_TRAIN_NUM];
	int COUNT;
	time_t  STTM,ENDTM;
	CTD_PlanInform()
	{
		JHID=0;
		COUNT=0;
		for(int i=0;i<MAX_NFORM_TRAIN_NUM;i++)
		{
			trains[i].KDID=0;
			trains[i].GTID=0;
			trains[i].InfoReceivedFlag=0;
			trains[i].LocalAdded=FALSE;
		}
		STTM=0;
		ENDTM=0;

	}
	BOOL  LoadFromXML(const char *xml,const char *node);
	BOOL  GenACKXML(CString &dest,const char *node);
};

class CTD_Ack
{
public:
	int ACKL;
	BOOL  LoadFromXML(const char *xml,const char *node);
};

#define MAX_TY_DCMD_NUM 64
#define MAX_TY_NOTIFY_NUM 64

class CTD_TYDCMD
{
public:
	CTD_TYDCMD();
	CTD_TYDCMD(const CTD_TYDCMD& rhs);

	BOOL LoadTYDcmd(const char *xml,const char *node);
	BOOL SaveTYDcmdToXML(CString &dest,const char *node);
public:
	CString CID;
	int CTYPE;

	CString CRECV;
	CString CTITLE;
	CString CCONTENT;
	CString CPNM;
	CString CSID;
	int CRCNT;
	CString CRPT;
};

class CTD_TYNOTIFY
{
public:
	CTD_TYNOTIFY();
	CTD_TYNOTIFY(const CTD_TYNOTIFY& rhs);

	BOOL LoadTYNOTIFY(const char *xml,const char *node);
	BOOL LoadTYNOTIFYX(const char *xml,const char *node);
	BOOL SaveTYNOTIFYToXML(CString &dest,const char *node);

public:
	CString NTITLE;
	CString NCONTENT;
	time_t NTM;
	DWORD NGID;
};

class CTD_SPECPLAN
{
public:
	CTD_SPECPLAN();

	BOOL LoadSPECPLAN(const char *xml,const char *node);
	BOOL LoadSPECPLANFromFile(const char *xml,const char *node);
	BOOL SaveSPECPLANToFile(CString &dest,const char *node);

public:
	long SRCCODE;
	CString MSGID;
	CString JHTNM;
	time_t DATE;
	int COUNT;
	int SEQ;
	int TRACKID;
	CString CONTENT;
	CString TNO;
	time_t  STTM;
	time_t  EDTM;
	CString STST;
	CString EDST;

	int NOTECNT;
	std::vector<CTD_TYNOTIFY> TYNOTIFY;
	int CMDCNT;
	std::vector<CTD_TYDCMD> TYDCMDLIST;
};

