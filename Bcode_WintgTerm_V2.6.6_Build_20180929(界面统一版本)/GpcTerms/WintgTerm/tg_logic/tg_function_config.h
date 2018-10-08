#ifndef __INCLUDE_TG_FUNCTION_CONFIG___
#define __INCLUDE_TG_FUNCTION_CONFIG___
#include "gpcdef.h"
#include "tg_defines.h"
#include <vector>

const int NEW_SPLIT_ST_NUM=64;

class WarnIdLeval
{
public:
	COLORREF clr;
	int id_count;
	int id[128];
	int tz_id[128];
	char text[256];
public:
	WarnIdLeval()
	{
		InitWarnIdLeval();
	}
	void InitWarnIdLeval()
	{
		id_count=0;
		clr=RGB(255,255,255);
		memset(id, 0, sizeof(id));
		memset(tz_id, 0, sizeof(tz_id));
		memset(text, 0, sizeof(text));
	}
};

struct StationDefaultElec
{
	int station;
	int entry;
	int exit;
	int flag;
	StationDefaultElec()
	{
		station=0;
		entry=0;
		exit=0;
		flag=-1;
	}
};

struct SectionDefaultElec
{
	int con_id;
	int station_count;
	StationDefaultElec default_elec[20];

	SectionDefaultElec()
	{
		con_id=0;
		station_count=0;
	}
};

struct ChangeTrainId
{
	int station;
	int pre_station;
	int next_station;
	
	int flag; // 1:为车站 2:为出口 3:为入口
	int entry_count;
	int entry[5];

	int method;
	char text[20];

	ChangeTrainId()
	{
		flag=0;
		station=0;
		pre_station=0;
		next_station=0;
		entry_count=0;
		memset(entry, 0, sizeof(entry));
		method=0;
		memset(text, 0, sizeof(text));
	}
};

struct ConsoleChangeId
{
	int con_id;
	int info_count;
	ChangeTrainId info[10];

	ConsoleChangeId()
	{
		con_id=0;
		info_count=0;
		memset(info, 0, sizeof(info));
	}
};

struct NeibPlanSY
{
	int  flag;
	int  first_station;
		
	int  fjk_station;
	
	int  exit_count;
	int  exit[10];   // 出口
	
	int  station_count;
	int  station_list[10];
	int  station_index_list[20];

	int  passway;
	int  accept_side;
	int  accept_stop;
	int  accept_arrive_depart;

	NeibPlanSY()
	{
		flag=0;
		first_station=0;
		
		fjk_station=0;
		accept_side=0;

		exit_count=0;
		memset(exit, 0, sizeof(exit));
		
		station_count=0;
		memset(station_list, 0, sizeof(station_list));
		memset(station_index_list, 0, sizeof(station_index_list));

		passway=0;
		accept_stop=1;
		accept_arrive_depart=0;
	}		
};

struct NeibConsolePlanSY
{
	int  neib_section;
	int  neibplan_count;
	NeibPlanSY neib_plan[32];
	
	NeibConsolePlanSY()
	{
		neib_section=0;
		neibplan_count=0;
		memset(&neib_plan, 0, sizeof(neib_plan));
	}		
};

struct ConsoleNeibPlanSY
{
	int  this_section;
	int  neib_console_count;
	NeibConsolePlanSY neib_console[10];
	ConsoleNeibPlanSY()
	{
		this_section=0;
		neib_console_count=0;
		memset(&neib_console, 0, sizeof(neib_console));
	}
};

// 接收反向列车邻台计划配置
struct NeibPlanReverse
{
	int  flag_count;
	int  flag[2];

	int  station;		// 目标车站
	int  station_pos;   // 目标车站在本台车站序列数组中得下标

	int  station_count;     // 本台车站序列中车站数目
	int  station_list[10]; // 本台车站序列
	int  station_index_list[20];  // 本台车站在本台的出入口站序
		
	int  only_accept_depart;
	int  accept_stop;
	NeibPlanReverse()
	{
		flag_count=0;
		flag[0]=0;
		flag[1]=0;
		
		station=0;
		station_pos=0;

		station_count=0;
		memset(station_list, 0, sizeof(station_list));
		memset(station_index_list, 0, sizeof(station_index_list));
		
		only_accept_depart=1;
		accept_stop=1;
	}		
};

struct NeibConsolePlanReverse
{
	int  neib_section;
	int  neibplan_num;
	NeibPlanReverse neib_plan[32];
	
	NeibConsolePlanReverse()
	{
		neib_section=0;

		neibplan_num=0;
		memset(&neib_plan, 0, sizeof(neib_plan));
	}		
};

struct ConsoleNeibPlan
{
	int  this_section;
	int  neib_console_num;
	NeibConsolePlanReverse neib_console[10];
	ConsoleNeibPlan()
	{
		this_section=0;
		neib_console_num=0;
		memset(&neib_console, 0, sizeof(neib_console));
	}
};

struct StationSideNotChange
{
	int station;// 车站
	int flag;   // 1:客车 2:货车 3:客货车
	int side_count;
	int side[10];

	StationSideNotChange()
	{
		flag=0;
		station=0;
		side_count=0;
		memset(side, 0, sizeof(side));
	}
};

struct SectionStationNotChange
{
	int station_count;
	StationSideNotChange info[20];

	SectionStationNotChange()
	{
		station_count=0;
		memset(info, 0, sizeof(info));
	}
};
struct PlanNoteTip
{
	int conid;
	int stano;
	int nxtstano;
	PlanNoteTip()
	{
		conid = 0;
		stano = 0;
		nxtstano = 0;
	}
};
struct DcmdTypeCnt
{
	char tmisType[32];
	long cascoType;
	DcmdTypeCnt()
	{
		cascoType = 0;
		memset(tmisType, 0, sizeof(tmisType));
	}
};

struct NeibStation
{
	int nPreStation; // 被控站前一站
	int nNeibStaNo;  // 被控站
	int nStationNo;  // 被控站后一站
	int nNextNextStationNo;// 被控站后后一站
	
	NeibStation()
	{
		nPreStation = 0;
		nNeibStaNo = 0;
		nStationNo = 0;
		nNextNextStationNo=0;
	}
};

class NeibStationPlan
{
public:
	int nSectionNo;
	std::vector<NeibStation> vecNeibStation;

public:
	NeibStationPlan();
	~NeibStationPlan();
	NeibStationPlan(const NeibStationPlan& rhs);
	NeibStationPlan& operator=(const NeibStationPlan& rhs);
};

struct CheckTrainDepart
{
	int station_warning;
	int station_check;

	CheckTrainDepart()
	{
		station_warning=0;
		station_check=0;
	}
};

struct SectionCheckTrainDepart
{
	int con_id;
	int count;
	CheckTrainDepart check[50];
	SectionCheckTrainDepart()
	{
		con_id=0;
		count=0;
	}
};

struct FromNoCal
{
	int conid;
	int station;
	int entry;
	FromNoCal()
	{
		conid=0;
		station=0;
		entry=0;
	}
};

struct ToLimitCal
{
	int conid;
	int station;
	int nextstation;
	ToLimitCal()
	{
		conid=0;
		station=0;
		nextstation=0;
	}
};

struct Entry_Exit_Default
{
	int station;
	int nextstation;
	int lineno;
	Entry_Exit_Default()
	{
		station=0;
		nextstation=0;
		lineno=0;
	}
};

struct NeibSectionStation
{
	int station_count;
	int station_no[16];
	int this_section[16]; // 是否本台管辖标志 1:本台管辖车站 0:非本台管辖车站
	NeibSectionStation()
	{
		station_count=0;
		memset(station_no, 0, sizeof(station_no));
		memset(this_section, 0, sizeof(this_section));
	}
};

struct NoLimitSectionStation
{
	int station_count;
	int station_no[64];
	NoLimitSectionStation()
	{
		station_count=0;
		memset(station_no, 0, sizeof(station_no));
	}
};

struct NeibStationList // 邻台完全接收配置
{
	int nThisSection;
	int nNeibSection;
	int entry;
	int exit;
	int station;
	int n_PreStation;
	int n_NextStation;
	NeibStationList()
	{
		nThisSection=0;
		nNeibSection=0;
		entry=0;
		exit=0;
		station=0;
		n_PreStation=0;
		n_NextStation=0;
	}
};

struct NeibAdjustExtern // 邻台交互配置
{
	int nThisSection;
	int nNeibSection;
	
	int nStation;
	int exit_count;
	int exit[8];
	int nNextStation;
	int nPassway;
	int nNodeIndex;

	int neib_count;
	int neib_station[8];
	int passway_list[8];
	int index_list[8];

	NeibAdjustExtern()
	{
		Init();
	}
	void Init()
	{
		nThisSection=0;
		nNeibSection=0;
		nStation=0;
		exit_count=0;
		memset(exit, 0, sizeof(exit));
		nNextStation=0;
		
		nPassway=0;
		nNodeIndex=0;
		neib_count=0;
		memset(neib_station, 0, sizeof(neib_station));
		memset(passway_list, 0, sizeof(passway_list));
		memset(index_list, 0, sizeof(index_list));
	}
};

struct SplitTrainExtern // 分割列车
{
	int nThisSection;
	int position_station;

	int station_pair_count;
	int station[8];
	int station_exit[8];
	int next_station[8];
	int next_station_entry[8];
};

struct StaEntry  // 济南局,晏城--晏城北出入口设置配置
{
	int section;
	int flag; // 匹配运行线位置 1:首站 2:中间站
	int count;
	int station[8];
	int entry[8];
	int exit[8];
	StaEntry()
	{
		count=0;
		flag=0;
		for(int i=0; i<8; i++)
		{
			station[i]=0;
			entry[i]=0;
			exit[i]=0;
		}
	}
};

struct SectionStaEntry  // 济南局,晏城--晏城北出入口设置配置
{
	int section;
	int count;
	StaEntry entry[16];
	SectionStaEntry()
	{
		section=0;
		count=0;
	}
};

class NeibPlanParam
{
public:
	int first_station_index;
	int first_station;
	int next_station;
	int next_next_station;
	int neib_station;
	int consider_start_train; // 本台始发车,是否去邻台查找, 0:不去邻台查找, 1:去邻台查找
	NeibPlanParam()
	{
		first_station_index=0;
		first_station=0;
		next_station=0;
		next_next_station=0;
		neib_station=0;
		consider_start_train=0;
	}
};

struct StationCheckNode
{
	int station;
	int setbcheck;
	int setscheck;
	int setwcheck;
};

class CheckStationsOfSection
{
public:
	int count;
	StationCheckNode station_list[256];
public:
	CheckStationsOfSection()
	{
		Init();
	}
	void Init()
	{
		count=0;
		memset(station_list, 0, sizeof(station_list));
	}
};

class NodeDefine
{
public:
	NodeDefine()
	{
		Init();
	}
	void Init()
	{
		index=-1;
		station=0;
		entry_line_num=0;
		memset(entry_line, 0, sizeof(entry_line));
		exit_line_num=0;
		memset(exit_line, 0, sizeof(exit_line));
	}
public:
	int index;
	int station;
	int entry_line_num;
	int entry_line[32];
	int exit_line_num;
	int exit_line[32];
};

class SectionDefine
{
public:
	SectionDefine()
	{
		Init();
	}
	void Init()
	{
		con_id=0;
		stdef_num=0;
		for(int n=0; n<128; n++)
			stinfo[n].Init();
	}
   
public:
	int con_id;
	int stdef_num;
	NodeDefine stinfo[128];
};

class StationStplanNotWarning
{
public:
	StationStplanNotWarning()
	{
		Init();
	}
	void Init()
	{
		station_count=0;
		memset(station_lists, 0, sizeof(station_lists));
	}
public:
	int station_count;
	int station_lists[128];
};

class CalSideTrainType
{
public:
	int nSection;
	int nCount;
	int nTrainType[256];
public:
	CalSideTrainType()
	{
		Init();
	}
	void Init()
	{
		nSection=0;
		nCount=0;
		memset(nTrainType, 0, sizeof(nTrainType));
	}
};

class CTGFunctionConfig
{
public:
	enum{MAX_LOGIC_SECTION_NUM=128,
		MAX_SPLIT_DEF_NUM=64,
		MAX_NOTIFY_STRLEN=128,
		MAX_NOTIY_COUNT=24,
		MAX_FJK_STAT_NUM=64,
		MAX_NEIB_FJK_STATION_COUNT=5};

	enum{ADJUST_FLAGBIT_BLOCK=0,
		 ADJUST_FLAGBIT_TRAIN=1,
		};

	struct
	{
		short section_id;
		short tmis_id;
		char  disid[10]; // 简码
		char  jhdisid[10]; // 计划台简码
		char  kddisid[10]; // 客调台简码
	}m_aSectionList[MAX_LOGIC_SECTION_NUM];
	int   m_nSectionCount;

	struct
	{
		short section_id;
		int   station1;
		int   station2;
		int   station1next;   // station1下一站
		int   station2pre;    // station2上一站

		int   first_entry;  // 前段列车入口
		int   first_exit;   // 前段列车出口
		int   second_entry;  // 后段列车入口
		int   second_exit;  // 后段列车出口
	}m_aSplitTrainDef[MAX_SPLIT_DEF_NUM];
	int m_nSplitDefCount;

	struct
	{
		int   day_time; //0点开始的分钟数
		char  notify_str[MAX_NOTIFY_STRLEN];
	}m_aAutoNotifyDef[MAX_NOTIY_COUNT];
	int m_nAutoNotifyDefCount;

	int   m_aFJKStations[MAX_FJK_STAT_NUM];
	int   m_nFJKStationCount;

	struct NeibPlanStruct
	{
		int  source_section;
		int  dest_section;
		int  prev_station;
		int  first_station;
		int  next_station;
		int  next_next_station;
		int  passway;
		int  node_index;
		
		int  nWholeMatch;
		int  accept_ternimal_train;
		int  neib_station; // 台间站
		int	 control_prev_abs; // 是否管台间站上一区间
		int  consider_start_train; // 本台始发车,是否去邻台查找, 0:不去邻台查找, 1:去邻台查找
		NeibPlanStruct()
		{
			source_section=0;
			dest_section=0;
			prev_station=0;
			first_station=0;
			next_station=0;
			next_next_station=0;
			passway=0;
			node_index=255;
			nWholeMatch=1;
			accept_ternimal_train=0;
			neib_station=0;
			control_prev_abs=0;
			consider_start_train=0;
		}		
	};
   vector<NeibPlanStruct>  m_aNeibPlanConfig;

   struct NeibPlanEntryStruct
	{
		int  source_section;
		int  dest_section;
		int  fjk_station;
		int  passway;
		int  node_index;
		int  exit;  // 邻台fjk_station出口
		int  bqd_entry; // 邻台fjk_station入口,用于本钱点
		int	 entry;  // 邻台fjk_station入口,用于筛选邻台计划
		NeibPlanEntryStruct()
		{
			source_section=0;
			dest_section=0;
			
			fjk_station=0;
			passway=0;
			exit=-1;
			bqd_entry=-1; //20140911
			node_index=255;
			entry=-1; 
		}		
	};
   vector<NeibPlanEntryStruct>  m_aNeibPlanEntryConfig;
	struct NeibPlanMode
	{
		int  this_section;
		int  neib_section;
		int  mode; // 1:按照出入口 2:按照前后站
		NeibPlanMode()
		{
			this_section=0;
			neib_section=0;
			mode=0;
		}
	};
	vector<NeibPlanMode>  m_aNeibPlanMode;

   // 接收反向列车邻台计划配置  
   std::vector<ConsoleNeibPlan> console_reverse_neibplan;

   // 沈阳正向邻台计划配置
   std::vector<ConsoleNeibPlanSY> console_neibplan;
   std::vector<int> m_notUseSyNeibPlan; // 配置不受沈阳模式邻台配置文件(neibplan.xml)控制的邻台计划,比如哈大使用通用邻台处理

   // 变更车次
   std::map<int, ConsoleChangeId> console_change_trainid;

   // 车站发车检查
   std::map<int, SectionCheckTrainDepart> section_check_depart;
   struct StationDefaultEntry
   {
	   short  station;
	   short  in_down_entry;  // 默认接入下行车口子
	   short  in_up_entry; // 默认接入上行车口子
	   short  out_down_entry; // 默认交出下行车口子
	   short  out_up_entry; // 默认交出上行车口子
	   StationDefaultEntry()
	   {
		   station=0;
		   in_down_entry=0;
		   in_up_entry=0;
		   out_down_entry=0;
		   out_up_entry=0;
      }

   };
   vector<StationDefaultEntry>  m_aStationDefaultEntryConfig;

   struct AdjustGapTime
   {
	   short              station;
	   short               act1;
	   short          act2;
	   short              type1;
	   short                  type2;
	   long                    gap_time;
	   AdjustGapTime()
	   {
		   station=0;
		   act1=-1;
		   act2=-1;
		   type1=-1;
		   type2=-1;
		   gap_time=0;
      }
   };
   vector<AdjustGapTime>  m_aAdjustGapTimeConfig;
	
   vector<int>  m_aInsideUsedTrainTypes;  // 路用车不检查卡控
   vector<int>  m_aFreeAddTrainTypes;  // 可以直接增加的列车类型
   vector<int>  m_vecCheckPlanSection; // 下计划默认检查站细调度台
   vector<int>  m_vecSectionThirtyByPlan;  // 30分钟按计划发车调度区段号
   
   vector<DcmdTypeCnt> m_vecDCMDType;

   map<int, NeibStationPlan> m_neigStationPlan; // 台间站列车计划发送配置
   struct FJK_LATE_TIME
   {
	   int station;
	   int method; // 0: 到点 1:发点
	   FJK_LATE_TIME()
	   {
		   station=0;
		   method=0;
	   }
   };
   vector<FJK_LATE_TIME> m_vecFjkStatisticLateTime;

   vector<SplitTrainExtern> m_NewSplitList;
   vector<NeibAdjustExtern> m_neibAdjustStation;
   vector<NeibStationList> m_neibRecvAllStation;

   vector<long> m_GoodTrainType; //  南昌局,允许有非技术停点的货车配置,比如 小运转等
   vector<long> m_PessengerTrainEndTechType; // 南昌局,允许客车终到是技术停点的客车配置,比如 动检车等
   vector<long> m_PessengerTrainStartTechType; // 允许客车始发是技术停点的客车配置
   vector<int> m_UseOPMSTrainType; //   南昌局,允许货车完全使用OPMS运行时间的列车类型

   struct USE_TDMS_LINE_STATION
   {
		int con_id;
		int station_id1;
		int station_id2;
		int station_id1_nd;
		int station_id2_nd;
		USE_TDMS_LINE_STATION()
		{
			con_id=0;
			station_id1=0;
			station_id2=0;
			station_id1_nd=255;
			station_id2_nd=255;
		}
   };
   vector<USE_TDMS_LINE_STATION> m_UserTDMSLineStation; // 广州局, 允许部分车站完全使用TDMS的线别

   map<int, SectionDefine> m_tg_define;
   
   WarnIdLeval m_stplanLeval[5];
private:
	char  m_szDataPath[512];
	char  m_szTrainBZCXurl[256];
	char  m_szWsDCMDurl1[128];
	char  m_szWsDCMDurl2[128];

private:
	char m_szGSoapAuthName[256];
	char m_szGSoapUrl[256];
	char m_szGSoapUrl2[256];

public:
	void GetWarnIdLeval(int level, WarnIdLeval& cfg);
	bool GetStaEntry(int section, SectionStaEntry& entry); // 济南局,晏城--晏城北出入口设置配置
	int  GetNewSplitStation(int nThisSection, int station_list[], int entry_list[], int max_size) const;
	int  GetNeibEqualStation(int nThisSection, int nNeibSection, NeibAdjustExtern neibPlanStation[], int max_size) const;

	int  GetNeibStationList(int nThisSection, int nNeibSection, NeibStationList ns[], int max_size) const;

	bool UseOPMSTrainTimeTrainType(int type);
	bool IsOtherBureaSection(int sectionno); // 判断是否为其他局区段，用于邻台计划。 true:外局区段 false:本局区段
	int GetNeibScheduleMinStopTime_Shanghai(BOOL type); // 获取最小停站时分 用于邻台计划
	void GetNeibStructs(int this_section,int neib_section,std::vector<NeibPlanParam>& neibplan_param_vect);
	bool IsEnableExactNeibPlanMode() { return m_bEnableExactNeibPlanMode;}
	bool IsNeibPlanShowAcceptInfoOrder() { return m_bNeibPlanShowAcceptInfoOrder;}
	bool IsQuerySchdClose() { return m_bQuerySchdClose;}
	bool IsShowColorNeibPlanTip() { return m_bShowColorNeibPlanTip;}
	int  GetFjkStatisticMethod(int station);
	int  GetNeibStations(int nSection, int station, std::vector<NeibStation>& vecNeibStaPlan);
	
	CGpcdef *GetGPCDEFx();
	const char *GetDataPath() const;
	BOOL  IfCalculateBasicTrainType() const;
	BOOL  NeiConsiderPassenger()  const;
	BOOL  IfSetDefaultTechStop() const;
	int   GetSuperSpaceTrainType() const;
	BOOL  IfdChoseSideWhileAllFull() const;
	BOOL  IfChoseMainSideWhilePass() const;
	
	int   GetDefaultRunTime() const;
	int   GetDefaultStopExtra() const;
	int   GetDefaultStartExtra() const;

	BOOL  IsDefaultAccept() const;  // 人工加车默认
	BOOL  IsDefaultTerminate() const;// 人工加车默认

	BOOL  IsLocalLogSendStPlan() const;
	int   GetSendWholePlanCronTimes() const;

	
	int   GetMaxPlanReceverCount() const {return 64;}
	int   GetMaxPlanTrainCount()  const {return 512;}

	int    GetDefaultSendPlanTRange() const {return m_nDefaultSendPlanTRange;}
	int    GetDefaultNeibPlanTRange() const {return m_nDefaultNeibPlanTRange;}
	BOOL  IsDefaultSendPlanClose() const;
	WORD  GetFileSaveFlag() const;
	int   GetFileKeepTimeHour() const;

	const char *GetBZCXurl() const {return m_szTrainBZCXurl;}
	const char *GetWSDCMDurl1() const {return m_szWsDCMDurl1;}
	const char *GetWSDCMDurl2() const {return m_szWsDCMDurl2;}

	char *GetGSoapAuthority()  {return m_szGSoapAuthName;}
	const char *GetGSoapURL() const {return m_szGSoapUrl;}
	const char *GetGSoapURL2() const {return m_szGSoapUrl2;}

	int   GetAutoShift2Time() const;
	int   GetAutoShift1Time() const;
	BOOL  IsMMPlanWSEnabled() const;
    BOOL  IsMMPlanEditable() const;
	int   GetSectionIDByTmisID(int tmis_id,char *dispid);
    int   GetTmisSectionID(int con_id,char *dispid=NULL,char *jhdispid=NULL,char *kddispid=NULL);
	BOOL  IsDefaultTrigByPlan() const;

	BOOL MatchSplitTrain(int con_id,int station1,int station2, int *station1next, int *station2pre, int* first_entry, int* first_exit, int* second_entry, int* second_exit);
	void  SetCheckManualOper(BOOL va);
	BOOL  IfCheckManualOper() const;
	BOOL  IsDebugTDMsg() const {return m_bDebugTDMsg;}
	void  SetDebugTDMsg(BOOL va) {m_bDebugTDMsg=va;}
	bool IfCheckAndSetAlarm() const {return m_bCheckAndSetAlarm;}
	
	BOOL IsNeibPlanConsiderSameTrain() const;

	BOOL IsStplanTrainIndexSaveInList() const;
	BOOL IsCheckExitValueSetDefualtExit() const;
	BOOL IsCheckEntryValueSetDefualtEntry() const;
	int  GetLineNo(int station, int nextstation);
private:
	int  m_nDefaultNeibPlanTRange;
	int  m_nDefaultSendPlanTRange;
	BOOL m_IfChoseMainSideWhilePass,m_IfCalculateBasicTrainType,m_NeiConsiderPassenger;
	BOOL m_IfSetDefaultTechStop,m_IfdChoseSideWhileAllFull;
	BOOL m_bLocalLogSendStPlan;
	int  m_nSuperSpaceTrainType;
	int  m_nDefaultRunTime;
	int  m_nDefaultStopExtra;
	int  m_nDefaultStartExtra;
	int  m_SendWholePlanCronTimes;
	BOOL m_bDefaultAccept;
	BOOL m_bDefaultTerminate;
	BOOL m_bDefaultSendPlanClose;
	BOOL m_bMMPlanWSEnabled;
	BOOL m_bMMPlanEditable;
	WORD m_nFileSaveFlag;
	int m_nFileKeepTimeHour;
	BOOL m_bDefaultTrigByPlan;
	BOOL m_bCheckManualOper;
	BOOL m_bDebugTDMsg;
	bool m_bCheckAndSetAlarm;
	
	bool m_bCheckStPlanWithBasicSchd;
	bool m_bNewNeibModeAddTrain;
	bool m_bEnableOPMSNodeIndex;
	BOOL m_bStationBlockNotCheckSide;

	BOOL m_bEnablePasstoStayModifyArivalTime;
	BOOL m_bEnableTempAddTrain;
	BOOL m_bEnableSelPasswayByDirectTrain;
	BOOL m_bEnablePlanNoticeDCmd; // 是否下计划通知调度命令终端

	BOOL m_bEnableOPMSBasicSchdSide; // 是否使用OPMS基本图股道
	BOOL m_bEnableOPMSShiftSchdSide; // 是否使用OPMS日班计划股道
	bool m_bIsGoodTrainUseOPMSSide;  // 货车是否使用opms股道
	std::vector<int> m_vConfigGoodTrainTypesOfUseOPMSSide; // 可以使用TDMS股道的货物列车类型

	bool m_bIsBasicValidSideByBasicSchd;	// 在使用OPMS基本图股道模式下，对于OPMS未给的股道，是否使用常用基本图股道
	bool m_bIsBasicValidSideByCalculate;	// 在使用OPMS基本图股道模式下，对于OPMS未给的股道，是否计算默认股道

	bool m_bIsCalBasicSchdExitEntry; // 预览基本图是否计算出入口, 默认计算
	bool m_bIsCalShiftSchdExitEntry; // 日班计划图是否计算出入口, 默认计算

	std::vector<int> m_vNotUseBasicSideSection;
	std::vector<int> m_vNotUseShiftSideSection;

	BOOL m_bDisEnableWSSaveDCmd;
	BOOL m_bEnablePrintNoteTrainStopOneHour; // 列车停车超过一小时,在记事中打印
	BOOL m_bEnableSelectTrainByLanzhou;
	BOOL m_bEnableInputEngineId;
	BOOL m_bEnableLanzhouEnginePlot;
	bool m_bSecretTrainToSta;
	bool m_bEnableCheckPlatform; // 检查站台
	
	BOOL m_bTempAddTrain;// 临时加车
	BOOL m_bEnableFreeDrawSetBasicTrainPlanTime; // 自由画线列车是否按照图定列车设置图定计划时分
	BOOL m_bUnknowTrainStPlanRunType;  //未知列车下发阶段是否运行等级是否下达为255
	bool m_bCheckCtcModeStaCtrlStPlan; //是否检查CTC模式车站完全控制计划
	BOOL m_bCheckAllStationStPlan; // 是否检查所有车站计划
	BOOL m_bCheckTrainMeetByNoMeet; // 按照列车编辑禁会列车检查禁会
	BOOL m_bDisenableSheetTab;  // 是否只读属性页
	BOOL m_bDisenableSheetTabExit;  // 属性页中下一区间是否只读

	bool m_bStplanSelectAllRecEntity; // true:每次打开阶段计划框选中所有接受计划单位 false:记忆上次
	bool m_bRemoveChineseCharSetTrainType; // 判断列车属性是否去掉前缀汉字
	bool m_bEngineChangeGiveoutTip; // 机车变化是否提示
	bool m_bTrainPassToStopUseMainSide; // true: 没有侧线到开车使用正线 false: 没有侧线到开车禁止使用正线

	bool m_bStplanDefaultSelLirc; // true:下发阶段计划只选中自律机 false:下发阶段计划选中所有设备
	bool m_bGetEngineFromPreConID; // true: 完全按照上个台传机车 false: 不按照上一个站传机车
	bool m_bSendStplanUseName; // true: 下发阶段计划发调度员姓名 false: 下发阶段计划发调度员代号
	bool m_bEnableDeleteFromTmisBlock; // true: 允许删除符号上图的施工 false:不允许删除符号上图的施工 
	bool m_bTrainIdUseArriveId; // true: 列车使用第一个站接入车次 false: 列车使用第一个站发车车次
	
	bool m_bTaiYuanNeigbourPlan; // true: 按照太原模式处理
	bool m_bChangeRouteTip;      // 动车和重点车的径路变更是否提示,默认不提示
	bool m_bGoodTrainUseBasicSide; // 基本图加车货车股道和基本图保持一致 true: 保持一致 false: 重新计算
	bool m_bSTIQueryBianzu; // 列车编组查询 false:不是使用STI; true:使用STI查询
	
	bool m_bAddTrainShiftAndBasic; // true:自由画线，选中按照日班加车，首先检查日班、再检查基本图
	bool m_bEnableThirtyByPlan; // 按照避让模式下,下发阶段计划是否修改进路触发为按照计划 true: 到开车前30分钟有车，设置为按计划发车 false:不修改
	
	bool m_bNeibPlanConsiderSameTrain; //2012.04.25 判断是否处理邻台计划中的重车次号的车 true:处理 false:不处理
	
	bool m_bStplanTrainIndexSaveInList; //2013.02.22 判断对于下过计划的列车运行线进行额外的记录 true:处理 false:不处理
	bool m_bCheckExitValueSetDefualtExit; //2013.02.28 赵亮修改,兰州局增加配置项目 true:检查出口有效性 false:不检查
	bool m_bCheckEntryValueSetDefualtEntry;//2013.05.23 乔永涛修改,增加配置项目 true:检查入口有效性 false:不检查
	// 自动换班时间
	int  m_nAutoShift1Time; // 早换班时间, 从0:00起分钟数
	int  m_nAutoShift2Time; // 晚换班时间, 从0:00起分钟数

	char m_szTrainId[25];
	int  m_dcmd_prefix;
	std::vector<PlanNoteTip> m_vctPlanTip;

	int  m_AutoAdjustOperFlag; // Bit 0: 不自动调整  1: 人工操作-自动避让调整 
	int  m_AutoAdjustStepFlag; // Bit 0: 调整考虑施工符号 Bit 1:调整考虑列车冲突

	int  m_nDepartDelayTimeOfBlockedABS;  // 封锁区间结束后，发车延时秒，自动调整用

	bool m_nSetPassengerTrainNoTechStopLockSide;//旅客列车办客站默认锁定股道
	bool m_bIsSubaoShowSpace; // 是否速报显示超限
	bool m_bSetStplanSpackRankByStation;//列车本站无超限时,下发阶段计划是否按照车站超限等级下发 0:默认值 按照整车属性下发 1:按照车站等级下发

	// 当列车通过变到开或者到开变通过时, 不改变股道列车类型
	std::vector<int> m_vecNoChangeSideWhileStopPass;
	
	bool m_bEnableLockWholeTrainSide; // true: 启用锁定整列车股道 false:禁用锁定整列车股道

	bool m_bEnableCzh; // true:启用车组管理 false:禁用车组管理
	bool m_bEnableNewCzh; // true:启用新车组管理功能  false:禁用新车组管理功能
	bool m_bChangeTrainID; // 老车组模式下 true:修改运行线数据 false:不修改运行线数据, 新车组模式 固定为false:不修改运行线数据
	bool m_bEnableTDMS5; // true:TDMS5.0 false:TDMS4.0
	int m_nAdjustAcceptRelation;  // 调整计划上调整图交路关系更新定义 0:不接收 1:接收
	int m_nNeibplanAcceptRelation;  // 邻台计划上调整图交路关系更新定义 0:不接收 1:接收
	// 内燃机车绘制绿线的台
	std::vector<int> m_vecConDieselLocomotiveDrawGreen;

	bool m_bDeleteRepeatDeleteStplan;//true:删除多余删除计划 false:不检查

	bool m_bUseStringKiloMeter; // true:使用字符串的公里标 false:不使用 （20140328针对太原局需要使用不以K开头的公里标指定）
	bool m_bIsNeibplanOnlyUseOPMSSchd; // true:邻台计划加车时，仅通过OPMS的运行线最新表现形式添加 false:默认,不启用该功能
	bool m_bShowMMPlanSkyLightButton; // true:施工计划界面增加两个按钮"施工"和"天窗",分别进行过滤显示  false:默认,不启用该功能
	bool m_bShowMMPlanMaintainButton; // true:施工计划界面增加两个按钮"施工"和"维修",分别进行过滤显示施工和维修计划  false:默认,不启用该功能

	bool m_bPlaySoundCrew;
	bool m_bPlaySoundNeiPlan;
	bool m_bBasicStopSetNoTechStop;
	bool m_bShiftStopSetNoTechStop;
	bool m_bFreedrawStopSetNoTechStop;
	
	bool m_bPassengerTrainStartSetNotTechStop; // 上海局, 始发客车,首站默认设置成非技术停点
	bool m_bPassengerTrainEndSetNotTechStop; // 南昌局, 终到客车,最后一个站默认设置成非技术停点
	bool m_bNeibPlanPassengerTrainAddNotByBasic; // 南昌局, 客车邻台交换不按照基本图加车
	bool m_bNeibPlanTrainAddOnlyByShift; // 济南局, 邻台交换不论客货车均首先按照日班加车,再向北洋申请,前者失败为单点
	int  m_nNeibPlanAddNewTrainMode;  // 邻台交互式0:新增列车 1:不新增列车(南宁) 2:列车来源不是日班计划时不新增列车(武汉)
	bool m_bSelBackupSideByCfg; // 南昌局, 被选股道按照配置顺序选择
	bool m_bShiftTrainSelFirstBackupSide; // 南昌局, 日班计划图客车计算股道,选第一备用股道,必须和m_bSelBackupSideByCfg搭配使用
	bool m_bGoodTrainNotTechStopNeedPwd; // 南昌局货物列车,非技术停点到开变通过需要密码提示 true:需要 false:缺省值,不需要

	bool m_bQuebaoQueryHaveChinese; // 查询确保 保留车次号中汉字
	bool m_bShowArriveTrainid; // 显示到达车次

	bool m_bDisEnablDeleteRecordWhileLimitModifyActual;
	bool m_bClosePollingCommStatus;
	bool m_bCalExitEntryByTrainId; // opms传过来列车，首站入口，末站出口计算, false:默认值,按照本站车次方向选择出入口 true:按照整车车次选择出入口 
	bool m_bHasNoLimitStation; // 是否存在不受封闭运行图限制的车站

	bool m_bTrainSideUseBasicSide; // 生成列车股道使用基本图股道, 上海局 false 其它局 true
	
	bool m_bEnableRecvAllAdjustPlan; // 上海局配置是否完全接收调整计划, true  上海局   false  其他局(默认值)
	bool m_bIsAlarmOnlyCheckCRHTrainSide;  // 下发阶段计划时检查五固定股道信息并报警  true 仅检查动车组列车  false 检查所有客车(默认值)
	bool m_bIsAlarmNotCheckSpackTrainMeetInfo;  // 下发阶段计划时不检查 超限车在同一车站内的交互 true 不检查 false检查(默认值)
	
	int m_nCheckElecSideWhenPassToStop; // 沈阳局要求 通过变到开时 增加检查电力股道报警配置 0:默认 不报警 1 仅检查中心控制的CTC车站 2 检查所有车站
	bool m_bEnablePlotMainSide; // 北京局要求 增加打印运行图时 加粗正线股道
	bool m_bQuestionStationPlanSelState; // 下发阶段计划,是否提示未选接收计划单位报警
	bool m_bQuestionStationPlanPassengerSta; // 下发阶段计划, 提示是否下发旅服系统
	bool m_bEnableNeibPlanFreightSameToPassenger; // 广州局要求 邻台计划中货车与客车相同方式进行处理
	
	bool m_bEnableLogTDMessage; // true;默认,日志记录TD接口传过来的消息信息 false:不启用该功能
	bool mbSupportInterCityStPlan; // 20150623是否支持珠三角城际折返计划 true:珠三角 false:通用路局计划
	bool m_bEnableRecvAdjustPlanTime; // 沈阳局配置 接收所有调整计划的时间点 true 沈阳局 false 其他局(默认值)
	bool m_bModifyTrainActAndGTIDSame; // 调整计划在全局ID相同且有实际点时是否更新列车
	bool m_bNewModeLoginRefresh; // 新模式(前12后12)每次都刷新数据
	int m_nReportEarlyTime; //济南局下发阶段计划时，到开旅客列车早于图定点N分钟进行提示
	bool m_bConsiderStplanSide; // 阶段计划中前后站股道是否设置-1,false:不设置 true:设置
	std::map<int, SectionDefaultElec> default_elec;

	std::map<int, SectionStationNotChange> side_not_change;
	std::vector<ToLimitCal> limit_section;
	std::vector<FromNoCal> notcalfromconid;
	std::map<int, NeibSectionStation> m_neibSectionStation;
	std::map<int, NoLimitSectionStation> m_noLimitStationList;
	int m_nSectionStaEntryCount;  // 济南局,晏城--晏城北出入口设置配置
	SectionStaEntry m_specialEntry[64]; // 济南局,晏城--晏城北出入口设置配置
	std::vector<int> m_otherBureaSection; // 存放其他局区段号，用于邻台计划
	std::vector<Entry_Exit_Default> default_entry_vet;

	// 上海局 精确邻台计划 模式
	bool m_bEnableExactNeibPlanMode;
	int m_nExactNeibPlanModeCrhStopTime;  // 动车组停车时分
	int m_nExactNeibPlanModeOtherStopTime; // 普速列车停车时分
	bool m_bNeibPlanShowAcceptInfoOrder; // 排序显示台间站时间
	bool m_bShowColorNeibPlanTip; // 上海局,邻台交换显示彩色大字体

	// 济南局需求
	bool m_bAcceptImportNote; // 是否接收处理OPMS传送的重点事项 true:显示 false:不显示
	bool m_bGiveChoicePathChange; // 接收调整计划后，当列车径路发生变化，是否提示调度员签收
	
    int  m_nBasicPreHour, m_nBasicBackHour; // 勾连关系基本图,默认查找前车时范围：（当前车始发时间，向前6小时）查找后车时范围：（当前车终到时间，向后6小时）
	bool m_bSetPreBasicTechStop; // false:无条件,只要径路一致就匹配 true:预览基本图列车为到开时才匹配常用基本图中营业三角
	bool m_bNeedCompareTrainExpand; // true:默认值,比较列车扩充部分 false：比较列车忽略列车扩充部分
	bool m_bNeedWaitMarkEnd; // true:默认值,批量加车需等待符号结束再画线 false：不等待
	bool m_bGpcSendReqOtherMsg; // 日班计划列车的其它：客调，营业和乘务信息怎么请求 true:运行图请求 false:TD自己请求,运行图不在发送请求加快速度
	bool m_bSendLogToLogsvr; // 是否将日志发送给logsvr true:发送 false：默认值,不发送
	bool m_bBasicDeletePassengerFreight; // 济南局需求,预览基本图中可以单独删除客车或者货车
	bool m_bGenAllMarkByDeleteExist;	// 青藏线需求,全体施工符号上图时,应先提醒删除已经存在的相同符号
	bool m_bConsiderTwicePassSameStation;	// 阶段计划处理列车两次通过同一车站比较基本图是否考虑该情况 false:不考虑 true:考虑
	bool m_bCompatibleOldStPlan; // 是否兼容老的计划
	bool m_bEnableModifyGID; // 是否允许更改全局ID
	bool m_bCheckShiftTrainAddTime; // 日班计划界面下，车次检查时，是否比较时刻
	bool m_NotSelAdjustPlan; // 调整计划,广州提出默认不选中
	bool m_bUsingWorkGoodMode; // 多个位图优化模式
	std::vector<int> m_NotCheckSection; // 接收邻台计划时,不检查本台是否存在比邻台多出运行线的调度区段
	bool m_bBasicShowCompareDlg; // 接收基本图完成,是否显示比较对话框
	bool m_bShiftShowCompareDlg; // 接收日班计划完成,是否显示比较对话框
	bool m_bEnableSelGenBasicTrain; // 广州局,是否允许选择基本图列车生成基本图
	bool m_bQuerySchdClose; // 广州局,查询台切换时不关闭已经打开的图
	bool m_bDisableAutoTrainNo; // 广州局,下计划可以选择是否禁止自动变更车次
	bool m_bTrainNoAutoChgChoice; // 下计划时是否可以选择不自动变更车次
	bool m_bEraseBeforeModifyTrain; // 去掉运行线影子
	bool m_bNotMeetBySpeed; // true:按照速度禁会 false:按照类型禁会
	bool m_bDisableModifyPlanTm; // 是否禁止修改图定时间
	CalSideTrainType mCalSideTrainType; // 本台不使用TDMS股道列车种类
	bool m_bCheckSideFull;
	bool m_bEnableScadElecArm; // 是否允许pscad
	bool m_bCheckTDMSSide; // 是否检查TDMS股道合法性
	bool m_bAcceptTrainID; // 调整计划是否接收当前时间3小时内车次变化
	bool m_bZeroGIDPasswayChange; // 太原局提出,当列车径路变化时,去掉全局ID
	bool m_bFormatView; // 是否显示为总公司统一界面
	bool m_bIsFindTrainByViewRange; // 济南局,如何报警重复车次, false:按照所在班次范围, true:可见范围
	bool m_bBatchAddTrainFindExistTrain; // 济南局,批量加车时 true:检查重复车次 false:不检查重复车次
	bool m_bCheckRelationSideCrash; // 广州局，是否检查交路股道冲突
public:
	bool IsCheckTDMSSide() { return m_bCheckTDMSSide;}
	bool IsEnableScadaElecArm() { return m_bEnableScadElecArm;}
	bool IsCheckSideFull() { return m_bCheckSideFull;}
	bool IsTrainNoAutoChgEnable() { return m_bTrainNoAutoChgChoice;}
	bool IsNotMeetBySpeed() { return m_bNotMeetBySpeed;}
	bool IsDisableModifyPlanTm() { return m_bDisableModifyPlanTm;}
	bool IsEraseBeforeModifyTrain() { return m_bEraseBeforeModifyTrain;}
	bool IsNewSetPreBasicTechStop() {return m_bSetPreBasicTechStop;}
	int GetFindBasicTrainBeforeTime() { return m_nBasicPreHour*3600;}
	int GetFindBasicTrainAfterTime() { return m_nBasicBackHour*3600;}
	
    ///////////////////////////////////
	bool   IsLimitSectionCalToConid(int conid, int station, int nextstation);
	bool   IsNotCalFromConid(int conid, int station, int entry);    

    ///////////////////////////////////
	bool   IsAcceptImportNote() {return m_bAcceptImportNote;}
	bool   IsGiveChoiceWhenPathChange() {return m_bGiveChoicePathChange;}
	bool   IsSubaoShowSpace() {return m_bIsSubaoShowSpace;}
	bool   IsStplanSpaceRankByStation() {return m_bSetStplanSpackRankByStation; }
	bool   IsShowArriveTrainid() {return m_bShowArriveTrainid;}
	bool   IsQuebaoQueryHaveChinese() {return m_bQuebaoQueryHaveChinese;}
	bool   IsBasicStopSetNoTechStop() {return m_bBasicStopSetNoTechStop;}
	bool   IsShiftStopSetNoTechStop() {return m_bShiftStopSetNoTechStop;}
	bool   IsFreedrawStopSetNoTechStop() {return m_bFreedrawStopSetNoTechStop;}

	CONID  GetTrainRunFromConid(int src_conid, int sta, int exit, int entry);
	CONID  GetTrainRunToConid(int src_conid, int sta, int exit);

	long  GetDcmdTempleteNo(CString type, CString subType);
	CString GetTdmsTrainId() { return m_szTrainId; }
	CTGFunctionConfig();
	BOOL  InitData(CString strPath);
	int   GetNoifyCount() {return m_nAutoNotifyDefCount;}
	BOOL  GetNotifyDesc(int index,int *daytime,char *desc,int desclen);
	BOOL  IsFJKStation(int station) const;
	int   MatchNeibPlan(int source_section,int dest_section,int stationlist[],int stationcount,int& matched_rec_count, bool& bHavePreStation,int& nodeindex,int& passway,bool& bWholeMatch, bool& bAcceptTerminalTrain, int &neib_station, int &control_prev_abs);
	
	int   GetNeibFJKStations(int dest_section,int source_section,int stationlist[],int limit);
	BOOL  GetDefaultEntry(int station,StationDefaultEntry &default_entry) const;
	int   GetDefaultEntry(int station, BOOL down_direction,BOOL accept_in) const;
	BOOL  NeedToCheckTGOper(int train_type) const;
	BOOL  GoodsTrainHaveTechStop(int train_type) const;
	BOOL  PassengerTrainEndTechStop(int train_type) const;
	BOOL  PassengerTrainStartTechStop(int train_type) const;
	bool  IsEnableOPMSNodeIndex() const;

	bool  IsCheckStPlanWithBasicSchd() const;
	bool  IsNewNeibModeAddTrain() const;
	bool  IsCalBasicSchdExitEntry() const;
	bool  IsCalShiftSchdExitEntry() const;

	BOOL  IsEnableOPMSBasicSchdSide(int nSection) const;
	BOOL  IsEnableOPMSShiftSchdSide(int nSection) const;
	bool  IsGoodTrainUseOPMSSide(int train_type) const;
	bool  IsPessagerTrainNotUseOPMSSide(int section, int train_type);

	bool  IsBasicValidSideByBasicSchd()	const;
	bool  IsBasicValidSideByCalculate() const;

	BOOL  IsEnableTempAddTrain() const;
	
	BOOL  IsEnableSelPasswayByDirectTrain() const;
	BOOL  IsEnablePasstoStayModifyArivalTime() const;
	BOOL  IsEnableSendPlanNoticeDCmd() const;
	BOOL  IsDisEnableWSSaveDCmd() const;
	BOOL  IsEnablePrintNoteTrainStopOneHour() const;
	BOOL  IsEnableSelectTrainByLanzhou() const;
	BOOL  MatchNeibPlanByEntry(int source_section,int dest_section,int station,int exit,int *nodeinex,int *passway, int entry); // 返回值成功失败
	bool IsSyNeighbourPlan(int conid);
	
	
	const NeibPlanSY* GetNeibPlanSY(int this_section, int neib_section, int id);
	const NeibConsolePlanSY*  GetNeibConsolePlan(int this_section,int neib_section);
	int   GetFJKStationPreList_SY(int this_section, int neib_section,int id, int station[], int limit, int& fjk_accept, int& accept_stop, int& accept_arrive_depart);

    // 接收反向列车邻台计划配置
	const NeibPlanReverse*  GetStationList_Reverse(int this_section, int neib_section, int id);
	const NeibConsolePlanReverse*  GetNeibPlan_Reverse(int this_section,int neib_section);
	
	void  LoadStplanWarnCfg(const CString pathname);
	bool  LoadNeibPlanReverseXML(const CString pathname);
	bool  LoadNeibPlanSYXML(const CString pathname);
	bool  LoadChangeTrainidXML(const CString pathname);
	bool  LoadTrainDepartCheckXML(const CString pathname);

	const ConsoleChangeId* GetConsoleChangeId(int con_id);

	BOOL  IsFreeAddTrain(int train_type) const;
	BOOL  IsStationBlockNotCheckSide() const;
	BOOL  IsEnableInputEngineId() const;
	BOOL  IsEnableLanzhouEnginePlot() const;
	BOOL  IsSendPlanTip(int con_id, int sta, int nxtsta) const;
	BOOL  IsFreeDrawSetBasicTrainPlanTime() const;
	BOOL  IsEnableUnknowTrainStPlanRunType() const;
	bool  IsCheckCtcModeStaCtrlStPlan() const;
	BOOL  IsCheckAllStationStPlan() const;
	BOOL  IsCheckTrainMeetByNoMeet() const;

	bool  IsSecretTrainToStation() { return m_bSecretTrainToSta;}
	bool  IsCheckPlanValid(int con_id);
	bool  IsEnableCheckPlatform() { return m_bEnableCheckPlatform;}
	
	BOOL IsDisenableSheetTab() const;
	BOOL IsDisenableSheetTabExit() const;
	
	bool IsStplanOnlySelectLirc() {return m_bStplanDefaultSelLirc;}
	bool IsStplanSelectAllEntity() {return m_bStplanSelectAllRecEntity;}
	bool IsRemoveTrainIDChineseChar() {return m_bRemoveChineseCharSetTrainType;}
	bool IsEngineChangeGiveOutTip() {return m_bEngineChangeGiveoutTip;}

	bool IsGetEngineFromPreConID() {return m_bGetEngineFromPreConID;}
	bool IsSendStplanUseName() {return m_bSendStplanUseName;}
	bool IsEnableDeleteFromTmisBlock() {return m_bEnableDeleteFromTmisBlock;}
	bool IsTrainPassToStopUseMainSide() {return m_bTrainPassToStopUseMainSide;}
	bool IsTrainIdUseArriveId() {return m_bTrainIdUseArriveId;}
	
	bool IsTaiYuanNeigbourPlan() {return m_bTaiYuanNeigbourPlan;}
	bool IsCheckRouteforCrhAndImp() {return m_bChangeRouteTip;}
	
	bool IsGoodTrainUseBasicSide() {return m_bGoodTrainUseBasicSide;}

	bool IsSTIQueryTrainBianzu() {return m_bSTIQueryBianzu; }
	int  GetAutoAdjustOperFlag() const;
	void SetAutoAdjustOperFlag(int va);

	bool IsEnableThirtyByPlan(const int con_id);
	bool IsFreedrawAddTrainShiftAndBasic() {return m_bAddTrainShiftAndBasic;}
	int  GetAutoAdjustStepFlag(int bit) const;
	void SetAutoAdjustStepFlag(int bit,int va);

	int  GetDepartDelayTimeOfBlockedABS() {return  m_nDepartDelayTimeOfBlockedABS;}
	bool IsChangeSideWhileStopPass(int train_type);

	bool SetPassengerTrainNoTechStopLockSide() const {return m_nSetPassengerTrainNoTechStopLockSide;}

	bool IsEnableLockWholeTrainSide() {return m_bEnableLockWholeTrainSide;}
	
	bool IsDeleteRepeatDeleteStplan() {return m_bDeleteRepeatDeleteStplan;}

	bool IsDieselLocomotiveDrawGreen(int con_id);
	bool IsNeibPlanPlaySound() {return m_bPlaySoundNeiPlan;}
	bool IsCrewPlaySound() {return m_bPlaySoundCrew;}

	bool IsEnableCzh() {return m_bEnableCzh;}
	bool IsEnableNewCzh() {return m_bEnableNewCzh;}
	int  GetAdjustAcceptRelationMode() {return m_nAdjustAcceptRelation; }
	int  GetNeibplanAcceptRelationMode() { return m_nNeibplanAcceptRelation; }
	bool IsChangeTrainID() {return m_bChangeTrainID;}
	bool IsEnableTDMS5_0() {return m_bEnableTDMS5;}
	bool IsGoodTrainNotTechStopNeedPwd() { return m_bGoodTrainNotTechStopNeedPwd;}
	bool IsUseStringKiloMeter() {return m_bUseStringKiloMeter;}
	bool IsNeibplanOnlyUseOPMSSchd() {return m_bIsNeibplanOnlyUseOPMSSchd;}
	bool IsShowMMPlanSkyLightButton() {return m_bShowMMPlanSkyLightButton;}
	bool IsShowMMPlanMaintainButton() {return m_bShowMMPlanMaintainButton;}
	
	bool IsNeibPlanAddTrainOnlyByShift() { return m_bNeibPlanTrainAddOnlyByShift;}
	bool IsNeibPlanPassengerTrainAddNotByBasic(){	return m_bNeibPlanPassengerTrainAddNotByBasic;}
	bool IsPassengerTrainEndSetNotTechStop(){	return m_bPassengerTrainEndSetNotTechStop;}
	bool IsPassengerTrainStartSetNotTechStop(){	return m_bPassengerTrainStartSetNotTechStop;}
	bool IsSelBackupSideByCfgQueue() {return m_bSelBackupSideByCfg;}
	bool IsShiftTrainSelFirstBackupSide() {return m_bShiftTrainSelFirstBackupSide;}
	bool IsEnableNeibPlanFreightSameToPassenger(){return m_bEnableNeibPlanFreightSameToPassenger;}
	
	bool IsSupportInterCityStPlan() { return mbSupportInterCityStPlan;}

	int SetElecTrain(int con_id, int station, int exit, int entry);
	bool IsSideNotChange(int con_id, int station, int side, int flag);

	bool GetTrainDepartCheckInfo(int con_id, int station, std::vector<int>& liststation); 

	bool IsNotThisSectionStation(int this_section, int station);
	bool IsThisSectionStation(int this_section, int station);
	UINT GetNotThisSectionStation(int this_section, int station[], int nMax);

	//封闭运行图通过配置增加不封闭车站配置，配置文件no_limit_station.xml
	bool LoadNoLimitStitonXml(CString filename);
	bool IsNoLimitStation(int this_section, int station);
	UINT GetNoLimitStationList(int this_section, int station[], int nMax);
	bool HasNoLimitStation() { return m_bHasNoLimitStation; }

	bool ProhibitDeleteRecordWhileLimitModifyActual() { return m_bDisEnablDeleteRecordWhileLimitModifyActual;}
	bool IsClosePollingCommStatus() { return m_bClosePollingCommStatus;}

	bool IsCalExitEntryByTrainId() { return m_bCalExitEntryByTrainId;}

	int  GetTrainDirection(CString strTrainId);
	
	int  GetNeibPlanMode(int this_section, int neib_section);

	bool IsTrainSideUseBasicSide() { return m_bTrainSideUseBasicSide;}

	bool IsEnableRecvAllAdjustPlan() { return m_bEnableRecvAllAdjustPlan;}
	bool IsAlarmOnlyCheckCRHTrainSide() {return m_bIsAlarmOnlyCheckCRHTrainSide;}
	bool IsAlarmNotCheckSpackTrainMeetInfo() {return m_bIsAlarmNotCheckSpackTrainMeetInfo;}


	int GetCheckElecSideWhenPassToStopFlag() { return m_nCheckElecSideWhenPassToStop; }
	bool IsEnablePlotMainSide() {return m_bEnablePlotMainSide;}
	bool IsQuestionStplanSelState() {return m_bQuestionStationPlanSelState;}
	bool IsQuestionStplanSelPassengerSta() {return m_bQuestionStationPlanPassengerSta;}

	bool IsEnableLogTDMessage() {return m_bEnableLogTDMessage;}
	bool IsThisSectionHasTDMSLineStation(int section);
	bool IsStationUseTDMSLine(int section, int station1, int station2, int station1_nd, int station2_nd);

	bool IsEnableRecvAdjustPlanTime() { return m_bEnableRecvAdjustPlanTime;}	
	bool IsNeibPlanNotAddNewTrainIFSourceNotShift() const;
	bool IsNeibPlanNotAddNewTrain() const;
	bool IsModifyTrainActAndGTIDSame() { return m_bModifyTrainActAndGTIDSame;}
	bool IsAcceptTrainID() { return m_bAcceptTrainID;}
	// 太原局，变更列车径路是否设置全局ID为0
	bool IsZeroGIDPasswayChange() { return m_bZeroGIDPasswayChange;}
	// 是否显示为总公司统一界面
	void SetFormatView(bool b){ m_bFormatView=b;}
	bool IsFormatView(){ return m_bFormatView;	}
	// 济南局,重复车次查找范围
	bool IsFindTrainByViewRange() { return m_bIsFindTrainByViewRange;}
	bool IsBatchAddTrainFindExistTrain() { return m_bBatchAddTrainFindExistTrain;}
	// 广州局，是否检查交路股道冲突
	bool IsCheckRelationSideCrash() { return m_bCheckRelationSideCrash;}
	int GetStPlanReportEarlyTimeRange() { return m_nReportEarlyTime;}
	bool IsNewModeLoginRefresh() { return m_bNewModeLoginRefresh;}
	bool IsConsiderStplanSide() { return m_bConsiderStplanSide;}
	bool IsNeedCompareTrainExpand() { return m_bNeedCompareTrainExpand;}
	bool IsNeedWaitMarkEnd() { return m_bNeedWaitMarkEnd;}
	bool IsGpcSendReqOtherMsg() { return m_bGpcSendReqOtherMsg;}
	bool IsSendLogToLogsvr() { return m_bSendLogToLogsvr;}
	bool IsBasicDeletePassengerFreight() { return m_bBasicDeletePassengerFreight;}
	bool IsGenAllMarkByDeleteExist() { return m_bGenAllMarkByDeleteExist;}
	bool IsConsiderTwicePassSameStation() { return m_bConsiderTwicePassSameStation;}
	bool IsCompatibleOldStPlan() { return m_bCompatibleOldStPlan; }
	bool IsEnableModifyGID() {return m_bEnableModifyGID; }
	bool IsShiftCompareWithBasicAddTime() { return m_bCheckShiftTrainAddTime;}
	bool IsNotSelAdjustPlan() const { return m_NotSelAdjustPlan;}
	bool IsUsingWorkGoodMode() const { return m_bUsingWorkGoodMode;}
	bool IsBasicShowCompareDlg() const { return m_bBasicShowCompareDlg;}
	bool IsShiftShowCompareDlg() const { return m_bShiftShowCompareDlg;}
	bool IsEnableSelGenBasicTrain() { return m_bEnableSelGenBasicTrain;} 
	bool checkConfig();
	BOOL LoadTgdef();
	bool VerifyStationWithIndex(int section, int station, int index);
	bool IsCheckNeibTrains(int section);
	void GetNeibPlanConfig(int neib_section,int this_section, vector<NeibPlanStruct>& neibPlanConfig);

	bool LoadCompareStationList(int section, CheckStationsOfSection& compare_stations);
	bool IsDisableAutoTrainNo() { return m_bDisableAutoTrainNo;}
	void SetDisableAutoTrainNo(bool b) { m_bDisableAutoTrainNo=b;}
	int  GetThisSectionStation(int con_id, int station_list[128], int total); // 获取该区段所有车站

	bool GetSectionConfig(int section, SectionDefine& node_section);
	bool LoadStplanNoWarningStation(int section, StationStplanNotWarning& stations);
	void LoadCalSideTrainType(int section);
};

CTGFunctionConfig *GetTGFunctionConfig();
// 原来定义在icommtask.h 
void GpcLogTrainMessage(short process_id, long id, const char *fmt, ...);
void GpcLogSysMessage(short process_id, const char *fmt, ...);
void GpcLogDebugMessage(short process_id, const char *fmt, ...);
#endif