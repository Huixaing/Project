#pragma once
#include <map>
#include <vector>
#include "afxtempl.h"
using namespace std;

// some constant for drawmode
#define DRAW_SCREEN	   0x01
#define DRAW_NORMAL    0x02

// some constant for highlow line
#define ENTRY_POSITION	    0
#define EXIT_POSITION		1
#define TOP_POSITION		2
#define BOTTOM_POSITION		3

//track  
#define TOP_TRACK	    	1
#define PRIMARY_TRACK		2
#define BOTTOM_TRACK        3

#define LINE_MAX_CNT 20


#define IsUsedByRightTrain(flag)    ((flag&0x1)== 0x1)
#define IsUsedByPessageTrain(flag)  ((flag&0x2)== 0x2)
#define IsUsedByGoodTrain(flag)     ((flag&0x4)== 0x4)


struct node_info 
{
	WORD  sub_section;
    int   index;
	char  text[32];
	WORD  station_number;
	WORD  up_line_count;
    WORD  up_line_list[LINE_MAX_CNT];
	WORD  down_line_count;
    WORD  down_line_list[LINE_MAX_CNT];
	WORD  space;
	BYTE  show_sation_sep; // 车站是否展开
	WORD  show_sides_mode; // 是否显示股道 
	BYTE  isNeedThick;  
    bool  show_statistic;               // 是否统计
    WORD  up_person_train_flag;             // 上行车统计情况选择  0: 到点  1: 发点  2：到发点  3: 不统计 4:到开车统计到点 通过车统计发点
    WORD  up_goods_train_flag;        // 下行车统计情况选择  0: 到点  1: 发点  2：到发点  3:不统计
	WORD  down_person_train_flag;                  //客车统计情况选择，通上
	WORD  down_goods_train_flag;                  // 货车统计情况选择,同上
   
    WORD  inhibit_sametime_dir;
	BYTE  electric_arms;   //0:不绘制(默认) 1:上 2:下 3:上下
	//每个车站对应的公里标，从GPC.DEF中提出
	//因为GPC中一个车站可能对应多个场，每个场公里标不同
	CString  kilo_mark;
	long  clrStationLine;
	int   m_nCzhPrintFlag; // 1. 车次折返站 2. 动车所
	node_info()
	{
		up_person_train_flag=3;
		up_goods_train_flag=3;
		down_person_train_flag=3;
		down_goods_train_flag=3;
		inhibit_sametime_dir=3;
		electric_arms = 0;
		kilo_mark = "0.0";
		clrStationLine=-1;
		m_nCzhPrintFlag=0;
	}
};

struct section_node_info 
{
	WORD  section_id;
    WORD  node_count;
    WORD  sub_section_count;
	std::vector<int> m_vectMutilSta;
	std::map<int, node_info> m_mapNodes;
	
	section_node_info();
	section_node_info(const section_node_info& rhs);
};

struct RouteDefine
{
	char routeName[100];
	DWORD routeFlag;  
    bool allSame;
	WORD station_num;
	WORD station_no[20];
	WORD in_station_index[20];
	WORD out_station_index[20];

	RouteDefine()
	{
		routeFlag=0;
		station_num = 0;
		allSame = false;
		memset(routeName, 0, sizeof(routeName));
		memset(station_no, 0, sizeof(station_no));
		memset(in_station_index, 0, sizeof(in_station_index));
		memset(out_station_index, 0, sizeof(out_station_index));
	}
};

struct section_path_define
{
	int section_id;
	std::vector<RouteDefine> m_route;
	std::map<WORD, WORD> m_station;
	section_path_define();
	~section_path_define();
	section_path_define(const section_path_define& rhs);
	section_path_define& operator=(const section_path_define& rhs);
};

struct ABS_LINE 
{
 	int nLineNo;
  	char sLineName[32];
	int nAbsNo;
	int nStartStation;
	int nEndStation;
	int nDirect;
};

struct SectionDisplay
{
	int  nSection;
	WORD nScreenTotalHeight;
	BYTE nSideHeight;
	WORD nUpSpaceHeight;
	WORD nDownSpaceHeight;

	BYTE nSpaceSubSectionCnt;
	WORD nSpaceBetweenSubSection[15];

	SectionDisplay()
	{
		nSpaceSubSectionCnt=0;
		memset(nSpaceBetweenSubSection, 0, sizeof(nSpaceBetweenSubSection));
	}
};

struct LocoNoDisplay
{
	int nSection;
	int nStation;
	int nArrivalIndex;
	int nDepartIndex;
	int nDirect;
	BYTE nAddShow;
	BYTE nSubShow;
	
	LocoNoDisplay()
	{
		nSection = 0;
		nStation = 0;
		nArrivalIndex = 0;
		nDepartIndex = 0;
		nDirect = 0;
		nAddShow = 0;
		nSubShow = 0;
	}
};

struct INHIBIT_TRAIN
{
	INHIBIT_TRAIN();
	BYTE nTypeCount;
	WORD trainType[100];
};

struct EntryDefine
{
	int station;
	int entry;
	int exit;
	EntryDefine()
	{
		station = 0;
		entry = 0;
		exit = 0;
	}
};

struct SingleEntryDefine
{
	EntryDefine currentSta;
	int beforeNum;
	EntryDefine before[5];
	SingleEntryDefine()
	{
		beforeNum = 0;
	}
};

struct SectionEntryDefine
{
	WORD section;
	int defineNum;
	SingleEntryDefine define[64];

	SectionEntryDefine()
	{
		section = 0;
		defineNum = 0;
	}
};

//;区段号, 区段名称, 车站, 站序1, 站序2,  绘制停车方法(1: 列车在站序1画停车  2: 列车在站序2画停车  3:列车在站序1、2都画停车)
//[STATION_STOP_LINE] = 22,  武广二台, 357,  1, 3, 1
struct StationStopLine
{
	int station;
	int   in_index;
	int   out_index;
	int   pass_stop_flag;
	
	StationStopLine()
	{
		station = 0;
		in_index = 0;
		out_index = 0;
		pass_stop_flag = 0;
	}
};

struct SectionStopLine
{
	int       section;
	char    name[50];
	vector<StationStopLine> station_stop_line;
	SectionStopLine()
	{
		section = 0;
		memset(name, 0, sizeof(name));
		station_stop_line.clear();
	}
};

struct ABS_DIRECT
{
	int conid;
	int station1;
	int station2;
	int index1;
	int index2;
	int lineno;
	ABS_DIRECT()
	{
		conid = 0;
		station1 = 0;
		station2 = 0;
		index1 = 0;
		index2 = 0;
		lineno = 0;
	}
};

// 特别出入口配置
struct SPECIAL_EXIT_ENTRY_INFO
{
	int conid;
	int station1;
	int station2;
	int index1;
	int index2;
	int lineno_count;
	int lineno[5];
	SPECIAL_EXIT_ENTRY_INFO()
	{
		conid = 0;
		station1 = 0;
		station2 = 0;
		index1 = 0;
		lineno_count = 0;
		memset(lineno, 0, sizeof(lineno));
	}
};

struct STATION_COLOR
{
	int index;
	COLORREF clr;
	STATION_COLOR()
	{
		index=-1;
		clr=99999999;
	}
};

struct CON_STATION_COLOR
{
	WORD con_id;
	STATION_COLOR station_clr[255];
	BYTE station_num;
	CON_STATION_COLOR()
	{
		con_id=0;
		station_num=0;
		memset(station_clr, 0, sizeof(station_clr));
	}
};

class DIRECT_DEFINE
{
public:
	int begin_station;
	int end_station;
	int begin_index;
	int end_index;
};

class TRAIN_DIRECT_DEFINE
{
public:
	int  con_id;
	int  direct_num;
	DIRECT_DEFINE direct[50];

	TRAIN_DIRECT_DEFINE()
	{
		con_id=0;
		direct_num=0;
		memset(direct, 0, sizeof(direct));
	}
};

struct STA_KILO_MK
{
	int  index;
	int  station;
	char kilo[64];
	STA_KILO_MK()
	{
		index=0;
		station=0;
		memset(kilo, 0, sizeof(kilo));
	}
};

struct SECTION_STA_KILO
{
	int con_id;
	int num;
	STA_KILO_MK sta_kilo[32];
	SECTION_STA_KILO()
	{
		con_id=0;
		num=0;
		memset(sta_kilo, 0, sizeof(sta_kilo));
	}
};


struct TGVirtualStation
{
	int perStationIndex;
	int nextStationIndex;
	int nStation;
	int nPercent;
	char strStatinName[64];
	TGVirtualStation()
	{
		perStationIndex=0;
		nextStationIndex=0;
		nStation=0;
		nPercent=0;
		memset(strStatinName, 0, sizeof(strStatinName));
	}
};

struct SectionVirtualStation
{
	int conid;
	int count;
	TGVirtualStation m_virtualStation[128];
};

struct TrainIDShowInfo // 2013.5.27 宁杭台 宁杭场折角车次显示
{
	int con_id;
	int station;
	int before_station;
	int after_station;
	TrainIDShowInfo()
	{
		con_id=0;
		station=0;
		before_station=0;
		after_station=0;
	}
};

//[SECTION_NODE_ENTRY]=105, 1, 2, 361, 7, 2
struct NodeEntryDefine
{
	int conid;
	int sub_table;
	int index;
	int station;
	int entry;
	int exit;
};

struct Entry_Exit_Default
{
	int section;
	int station;
	int nextstation;
	int line_num;
	Entry_Exit_Default()
	{
		section=0;
		station=0;
		nextstation=0;
		line_num=0;
	}
};

struct Station_Direction_Tip
{
	int section;
	int station;
	int next_station;
	char tip[32];
	Station_Direction_Tip()
	{
		section=0;
		station=0;
		next_station=0;
		memset(tip, 0, sizeof(tip));
	}
};

struct StationGiveoutTip
{
	int section;
	int station;
	int exit;
	char tip[32];
	StationGiveoutTip()
	{
		section=0;
		station=0;
		exit=0;
		memset(tip, 0, sizeof(tip));
	}
};

struct StationAcceptTip
{
	int section;
	int station;
	int entry;
	char tip[32];
	StationAcceptTip()
	{
		section=0;
		station=0;
		entry=0;
		memset(tip, 0, sizeof(tip));
	}
};

class CTGDef
{
public:
	CTGDef(void);
	~CTGDef(void);

public:
	void Clear();
	bool LoadTGProfile(CString pathname);
public:
	WORD  GetSectionIDs(WORD id[], const WORD nMax);
	int   GetSectionNodeCount(int sectionid);
	bool  IsMutilStation(int sectionid, int sta);
	const node_info* GetNodeByIndex(int sectionid, int index);
	const SectionDisplay* GetSectionDisplay(int sectionid);
	
	int   GetLineDirection(int lineNo);
	const ABS_LINE* GetLineDefine(int lineno);
	int   GetErrLineNo(){return m_nTgErrorline;}

	bool  IsInhibitTrainType(int sectionid, WORD type);
	bool  IsBianzuStation(int sta);
	bool  IsInterfaceStation(int sta);

	int  GetSectionHeight(int nSectionNo);

	bool GetRouteDefine(WORD section_id, section_path_define& route_define);
	bool IsDefinePathStation(WORD section_id, WORD sta_no);
	bool IsCalcInOutStaSection(int section);
    
	int GetStationStopLine(int section, int station, int in_index, int out_index);
	int GetInitAbsLineNo(int conid, int sta1, int sta2, int index1, int index2);

	int GetSpecialExitEntryLineNo(int conid, int sta1, int sta2, int index1, int index2, int absno[]);
public:
	const SectionEntryDefine* GetSectionEntryDefine(const int nSection);
public:
	void GetStationList(WORD nSection, int& stationNum, int* stationList);
	int  GetWholeScreenHeight() { return m_ScreenHeight;}
	void SetWholeScreenHeight(int h) {m_ScreenHeight = h;}
	bool IsCanDrawABSStart(int con_id);
	
	bool GetTrainDirectDefine(int con_id, TRAIN_DIRECT_DEFINE& define);
	CString GetKiloMk(int conid, int index, int station);
	
	bool GetEntryExit(int conid, int sub, int index, int station, int& entry, int& exit);
	bool IsLocoNoDisplay(int conid, int station, int arrival_index, int depart_index, int direction, int add_or_sub);

	bool IsNoCheckMinLateSection(int section);
	int GetLineNo(int section, int from_st[32], int to_st[32], int lineno[32]);
	void GetSectionDirectTip(int section, vector<Station_Direction_Tip>& direction_tip);
	void GetStationGiveoutTip(int section, vector<StationGiveoutTip>& giveout_list);
	void GetStationAcceptTip(int section, vector<StationAcceptTip>& accept_list);
private:
	bool ReadCzhConfig(char* data,char* seps);
	bool ReadSection(char* data,char* seps);
	bool ReadSectionNodeEntry(char* data,char* seps);
	bool ReadVirtualStation(char* ptr,char* seps);
	bool ReadDefinePath(char* data,char* seps);
	bool ReadABSLine(char* ptr,char* seps);
	bool ReadSectionDisplay(char* ptr,char* seps);
	bool ReadDisplayTypeInhibit(char* ptr,char* seps);
	bool ReadABSTrainNO(char* ptr,char* seps);

	bool ReadLocoNoDisplay(char* ptr,char* seps);


	bool ReadDrawAbsStart(char* data,char* seps);
	bool ReadInOutStation(char* data,char* seps);
	bool ReadEntryDefine(char* data);
	bool ReadScreenHeight(char* ptr,char* seps);
	bool ReadSectionStopLine(char* ptr,char* seps);
	bool ReadAbsDirect(char* ptr,char* seps);
	
	bool ReadTrainDisplay(char* ptr,char* seps);
	bool ReadStationLineColorDisplay(char* ptr,char* seps);
	bool ReadStationKiloMk(char* ptr,char* seps);

	bool ReadSpecialExitEntry(char* ptr,char* seps);

	bool ReadFromToLineNo(char* ptr,char* seps);
	bool ReadDirectionTip(char* ptr,char* seps);
	bool ReadGiveoutTip(char* ptr,char* seps);
	bool ReadAcceptTip(char* ptr,char* seps);
private:
	vector<Entry_Exit_Default> m_entryset;
	vector<Station_Direction_Tip> m_direction_tip;
	vector<StationGiveoutTip> m_giveout_tip;
	vector<StationAcceptTip> m_accept_tip;
	
	std::map<WORD, INHIBIT_TRAIN> m_mapInhibitTrainType;
	std::map<WORD, section_node_info> m_mapSection;
	std::map<WORD, section_path_define> m_mapRoute;

	std::map<int, TRAIN_DIRECT_DEFINE> m_mapShowDirect;
	std::map<int, ABS_LINE> m_mapAbsLine;  
	std::vector<SectionDisplay> m_section_display_info;
    std::map<int, SectionEntryDefine> m_mapEntryDefine;  

	std::vector<LocoNoDisplay> m_loco_no_display_list;

	std::vector<SectionStopLine> m_vecSectionStopLine;
	std::vector<ABS_DIRECT> m_vecAbsDirect;
	std::map<int, int> m_mapCanDrawABSStart;
	std::map<int, CON_STATION_COLOR> m_mapStationLineColor;
	std::map<int, SECTION_STA_KILO> m_mapStationKilo;

	std::vector<SPECIAL_EXIT_ENTRY_INFO> m_vecSpecialExitEntry;
	std::vector<NodeEntryDefine> m_vecNodeEntry;
public:
	std::vector<int> m_infteraceStation;
	std::vector<int> m_bianzuStation;
	std::vector<int> m_inOutStationIndexSection;

public:
	std::map<int , SectionVirtualStation> m_TGVirtualStation;
	
private:
	char *token;
	int m_nTgErrorline;
	int m_ScreenHeight;
    
public:
	int m_nCheckMinLate; // 运行图最近原则
	std::vector<int> m_vecNoCheckMinLateSectionList; // 运行图就近原则的调度台
};
