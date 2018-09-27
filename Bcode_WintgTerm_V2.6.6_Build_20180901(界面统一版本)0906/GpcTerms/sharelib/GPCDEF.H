/* gpcdef.h */

#ifndef _GPCDEF_H
#define _GPCDEF_H

#include "GpcDataTmpl.h"	//added by wsg

#include <vector>
#include <map>
#include <algorithm>
using namespace std;

#include "messages.h"
#include "defines.h"
#include <math.h>

const int MAX_DIRECTOR_NUM = 16;
//below is for entity define
const int GPC_DISPACHER =1;  //调度员
const int GPC_OPERATER =2;   //操作员

const int GPC_VICEDISPACHER =3; //助理调度员
const int STPC_WACHER =11;   //车站值班员
const int STPC_OPERATOR =13; //信号员
const int STPC_DISPACHER =14; //车站调度员
const int SYSTEM_ADM =21;   //系统维护员
const int EQUIP_BASELEVEL =100; //BASELEVEL
const int EQUIP_SERVER =101;  //  SERVER
const int EQUIP_LIRC = 102;//    / LIRC
const int EQUIP_HILC = 103;//HILC


enum TASK_LIST{
	    WINTG_THREAD=1,
		LAYOUT_THREAD,
		DISPATCH_THREAD,		
};
const int CTC_STATION = 1;
const int TDCS_STATION = 0;
const int NONE_SYSTEM_STATION = -1;


// Profile data
#define DATADIR				"..\\DATA"//"e:\\gpc_dmis\\data" //
#define GPCFILENAME			"GPC.DEF"
#define GPCNOFILENAME		"GPCNO.DEF"
#define TLEFILENAME			"STAT_DAT.RST"
#define ICONFILENAME	    "ICON.DAT"
#define LINEFILENAME		"LINE.CFG"
#define TGDEF       		"TG.DEF"
#define LOCOFILENAME		"LOCOTYPE.CFG"

 
#define SIMU_CONTROL		0X0010
#define VIEW_GRAPH			0X0008

// Wintg sub task
#define UPDATE_BASIC		0X0040
#define UPDATE_HISTORY		0X0080
#define USETRAIN_INFO		0X0200
#define UPDATE_WORK			0X0100
#define PLOT_TASK			0X0400
#define REPORT_TASK			0X0800
#define WINTG_TASK			0X0FC0
// Affair sub task
#define AFFAIR_INFO			0X1000
#define AFFAIR_TASK			/*0X1000*/0x0000

#define DISPATCH_TASK			0x2000   
#define DISPATCH_INFO			0X1000
// digital map sub task
#define DIGITAL_MAP_TASK	0X1000

//new define job right
///////////////////////////////////////
 
const DWORD	WINTG_EDIT = 0x01000000;
const DWORD	DRAW_MARK  = 0x02000000;
const DWORD	STATION_STORE_TRAIN = 0x04000000;
const DWORD	DISPATCH_NOTE_EDIT = 0x08000000;
const DWORD	ROUTE_OPERATE = 0x10000000;

///////////////////////////////////////

#define GPC_SCHEDULE		1   //a 计划员台
#define GPC_DISPATCHER		2   //b 调度员台
#define GPC_OPERATOR		3   //c 操作员台
#define GPC_DISPATCHERS		4   //d 调度长台
#define GPC_SCREEN_LAYOUT	5   //e 背投
#define GPC_TRAIN			6   //f 培训台
#define GPC_SERVICE			7   //g 
#define GPC_MAINTAIN		8   //h 电务维修台
#define GPC_PLOT			9   //i 绘图台
#define GPC_DISPATCHER_READONLY 9 // i 调度员切换到其他台时状态。//sunjf 5-17
#define GPC_MACHINE			10  //j 机调员台
#define GPC_SUPER_OPER	    11  //k 施工调度和助调合一  GPC_MAINTAIN_DISPATCH && GPC_OPERATOR
#define GPC_SCREEN_TG		12	//l 统计台
#define GPC_AFFAIR_SERVER	13	//m
#define GPC_SUPER_DISPATCH	14	//n
#define GPC_OTHER_DISPATCH	15	//o
#define GPC_SUPER_MAINTAIN	16	//p 系统维护台
#define GPC_TEST	        17	//q 系统调试员
#define GPC_MAINTAIN_DISPATCH	18 //综合维修调度台
#define GPC_MAP				19 //r大屏
#define GPC_USERNAME_ERROR (-1)
#define GPC_PASSWORD_ERROR (-2)

#define GPC_SECTION_DISPTACH ("[SECTION_DISPATCH]")
#define MAX_SECTION_NUM 128
#define MAX_SECTION_STA 128
// profile keyword define
#define GPC_USER			("[USER]")
#define GPC_SPSTATION		("[SPECIALSTATION]")	
#define GPC_SECTION			("[SECTION]")
#define GPC_SECNEIGHBOR		("[SECNEIGHBOR]")
#define GPC_SECNEIGHBOR_RECEIVER		("[SECNEIGHBOR_RECEIVER]")
#define GPC_SECTIONROUTE	("[SECTIONROUTE]")
#define GPC_STATIONDELTA	("[STATIONDELTA]")
#define GPC_DEFAULT			("[DEFAULT]")
#define GPC_ROUTE			("[ROUTE]")
#define GPC_PICTURESTIP		("[PICTURESTIP]")
#define GPC_STATION_SPACE	("[DELTA]")
#define GPC_SCREENINFO		("[SCREENNUM]")
#define GPC_PAGEREVERSE		("[PAGEREVERSE]")
#define GPC_BUREAUNAME		("[BUREAUNAME]")
#define GPC_DISPSECTION		("[DISPSECTION]")
#define GPC_DISPSECTION_DOMINATION		("[SECTION_DOMIN]")
#define GPC_REMOTEPATH		("[REMOTEPATH]")
#define GPC_REMOTEPATH_ROUTE		("[ROUTEREMOTEPATH]")
#define GPC_LOCALPATH       ("[LOCALPATH]")
#define GPC_SERVERADDR		("[SERVERADDR]")
#define GPC_SERVERADDR_ROUTE		("[ROUTESERVERADDR]")
#define GPC_REPLAYSVRUP		("[REPLAYSVR-UP]")
#define GPC_REPLAYSVRUP_ROUTE		("[ROUTEREPLAYSVR-UP]")
#define GPC_PBSHAREDIR      ("[PBSHAREDIR]")
#define GPC_LOGSVRADDR		("[LOGSVRADDR]")
#define GPC_LOGLOCALPATH    ("[LOGLOCALPATH]")
#define GPC_LOGSVRPATH		("[LOGSVR-PATH]")
#define GPC_LOGSVRUP		("[LOGSVR-UP]")

#define GPC_PLOTTERNAME		("[PLOTTERNAME]")
#define GPC_PRINTERNAME     ("[PRINTERNAME]")
#define GPC_PRINTERNAME_TY  ("[PRINTERNAME_TY]")
#define GPC_SECTION_NODE    ("[SECTION_NODE]")
#define GPC_SECTION_ROUTE    ("[SECTION_ROUTE]")
#define GPC_ABS_LINE         ("[LINE_DEFINE]")
#define GPC_DISPATCHERCOMMAND	("[DISPATCHERCOMMAND]") 
#define GPC_DISPATCHERCOMMANDSAVE	("[DISPATCHERCOMMANDSAVE]") 
#define GPC_TENICAL_STOP_DRAWWAY	("[TENICAL_STOP_DRAWWAY]") 

#define GPC_SWITCH_STATION    ("[SWITCH_STATION]")
#define GPC_NOT_SHOW_SIDE_CTC_STATION  ("[NOT_SHOW_SIDE_CTC_STATION]")
#define GPC_PLOTITEM            ("[PLOTITEM]")           
#define GPC_SECTIONPLOTITEM     ("[SECTIONPLOTITEM]")    
#define GPC_INTERFACESTATION    ("[INTERFACESTATION]")  
#define GPC_STATIONENTRY        ("[STATIONENTRY]")    
#define GPC_SECTION_DISPLAY     ("[SECTION_DISPLAY]")  
#define DISPLAY_TYPE_INHIBIT    ("[DISPLAY_TYPE_INHIBIT]")  
#define GPC_ROUTE_DIRECTION     ("[ROUTE_DIRECTION]")  
#define GPC_HINGE_STATION     ("[HINGE_STATION]")   
#define GPC_HINGE_ARRIVE_DEPART     ("[HINGE_ARRIVE_DEPART]") // added by qianguorong 2004-02-24,
#define GPC_HINGE_ARRIVE_DEPART_STATION     ("[HINGE_ARRIVE_DEPART_STATION]") // added by qianguorong 2004-03-18,

#define GPC_SECTION_PHASE_STORE_STATION     ("[SECTION_PHASE_STORE_STATION]")  

#define GPC_CAD_OS_SYSTEM   ("[CAD_OS_SYSTEM]")
#define BIANZHU_STATION    ("[BIANZHU_STATION]")  
#define FENJIE_STATION    ("[FENJIE_STATION]")  
#define GPC_ELECTRONIC_MODULE     ("[DISPATCHERCOMMAND_ELECTRONIC_MODULE]") //区间电网检修

#define GPC_ERRORSIDESTATION	("[ERROR_SIDE_STATION]")  
#define GPC_NOT_THIS_STATION	("[NOT_THIS_SECTION_STATION]")

#define GPCNO_ONLY_ONE_TG ("[ONLY_ONE_TG]")
#define GPCNO_TG_CTRL ("[TG_CTRL]")
#define GPCNO_ROUTELIST_FONTSIZE ("[RouteListFontSize]")
#define GPCNO_ALARM_SOURCE ("[ALARMFROMSERVER]")
#define GPCNO_ROUTE_SOURCE ("[ROUTEFROMSERVER]")

#define GPCNO_RUNTIME_COMPARE_WITH_BASIC ("[RUNTIME_COMPARE_WITH_BASIC]")

#define GPCNO_WINTG_NOSYN_TIME ("[WINTG_REPORT_NOSYN_INTERVAL]")
#define GPCNO_WINTG_SET_MSS_PORT ("[WINTG_SET_MSS_PORT]")
#define GPCNO_LAYOUT_SET_MSS_PORT ("[LAYOUT_SET_MSS_PORT]")

#define GPCNO_FORMAT_VIEW 	("[FORMAT_VIEW]")   // 是否显示为总公司统一界面
#define GPCNO_TRAIN_NO_LEVEL_COUNT 	("[TRAIN_NO_LEVEL_COUNT]")   // 列车车次显示层数
#define GPC_SECTION_BASIC_PLAN      ("[SECTION_BASIC_PLAN]") //基本图阶段计划接受
#define GPC_PLAN_RECEIVER ("[PLAN_RECERVER]") //阶段计划接受；
#define GPC_JIDIAO_SERVERINFO     ("[JIDIAOSERVER]") //机调文件服务器地址信息
#define GPC_AUTO_CHANGE_TRAINID  ("[AUTOCHANGE_TRAINID_STATION]") // added by wjh 

#define GPC_PRINTFORMAT		("[PRINTFORMAT]")
#define GPC_TRAINIDFORMAT	("[TRAINIDFORMAT]")
 
#define GPC_TRAIN_LEAP_LINE_FLAG  ("[LEAPLINEFLAG]")// 运行线跳跃时，画接入交出还是画直连线  sunjf 2005 6 17
#define GPC_LINE_WIDTH  ("[LINEWIDTH]") // 粗线条或细线条 sunjf 2005 6 17
#define GPC_LAYOUT_POS  ("[LAYOUTPOS]") // Layout的显示区域, Zhoujb added, 2008-01-16
#define GPC_LAYOUT_POS_STA  ("[LAYOUTPOS_STA]") // Layout的显示区域, Zhoujb added, 2008-01-16

#define LOCO_TYPE ("LOCOTYPE")
#define GPCDEF_TRAINTYPE ("TRAINTYPE")
#define GPC_ROUTE_LIST_VIEW_CHANGE_BY_STATION	("[ROUTELISTVIEW]") 

//进路序列强制执行
#define GPC_ROUTELIST_EXECUTE_FORCE_ENABLE ("[ROUTELIST_EXECUTE_FORCE_ENABLE]")

// 列控命令操作权限设置
#define GPC_TCC_DCMD_EXECUTE_RIGHT ("[TCC_DCMD_RIGHT]")

//值班主任授权命令下发时机---1: 立即下发 2: 默认择机下发
#define GPC_DISPATCHERCOMMAND_SEND_MODE ("[GPC_DISPATCHERCOMMAND_SEND_MODE]") 
//值班主任授权类型选择
#define GPC_DISPATCHERCOMMAND_AUTHORITY_TYPE ("[DISPATCHERCOMMAND_AUTHORITY_TYPE]") 
//调度命令授权配置信息
#define GPC_DISPATCHERCOMMAND_AUTHORITY     ("[DISPATCHERCOMMAND_AUTHORITY]") 
#define GPC_RECV_UNIT_SYTLE    ("[DISPATCHRECVUNITSHOWSTYLE]")   // 调度命令受令单位分表显示配置 qiaoyt 2008 4 29
#define GPC_SEND_DCMD_NAME     ("[DISPATCHCOMMAND_SENDER_NAME]") // 调度命令发送者姓名


#define GPC_SPECIAL_STPLAN			("[SPECIAL_STPLAN_GPC]")
#define GPC_SPECIAL_STPLAN_GROUP	("[SPECIAL_STPLAN_STATION_GROUP]")

#define TGDEF_ABSELE_COLOR                  "[ABSELE_COLOR]"
#define TGDEF_TRAINID_FONT                  "[TRAINID_FONT]"
#define TGDEF_TIMESTRING_FONT               "[TIMESTRING_FONT]"
#define TGDEF_LINESIDE_FONT                 "[LINESIDE_FONT]"

#define GPCNO_CHEZUHAO_ENABLE               "[CHEZUHAO_ENABLE]"
#define GPCNO_LIMIT_MODIFY_ACTUAL_TIME		"[LIMIT_MODIFY_ACTUAL_TIME]"
#define GPCNO_ENABLE_MMPLAN_SVR				"[ENABLE_MMPLAN_SVR]"
#define GPCNO_PREDICT                       "[PREDICT_ENABLE]"
#define GPC_YUGAO_STATION					"[YUGAO_STATION]"
#define GPCNO_BASICSCHEDULEPRIORITY         "[BASICSCHEDULEPRIORITY]"
#define GPCNO_BASIC_SCHEDULE_DUMP			"[BASIC_SCHEDULE_DUMP]" 
#define GPCNO_SHOW_SIDE_NOT_CONFIRM			"[SHOW_BASIC_SCHEDULE_SIDE_NOT_CONFIRM]" 
#define GPCNO_BASIC_SCHEDULE_DUMP_TIME		"[BASIC_SCHEDULE_DUMP_TIME]" 

#define GPCNO_BMPBIT_DEEP				    "[BMPBIT_DEEP]"
#define GPCNO_BASICSCHEDULESYN				"[BASIC_SYN_DATA]"

#define GPCNO_PRESCHDNUM					"[PRESCHDNUM]"

#define GPCNO_WIDE_PIXEL					"[WIDE_PIXEL]"
#define GPCNO_USE_VIRTUAL_MEMORY			"[USE_VIRTUAL_MEMORY]"
#define GPCNO_MODIFY_TSR_STATUS				"[MODIFY_TSR_STATUS]"
#define GPCNO_AUTO_MOVE_MODE				"[AUTO_MOVE_MODE]"

#define GPCNO_TGMSG_TO_GATEWAY				"[WINTG_TG_MSG_TO_GATEWAY]"
#define GPCNO_OTHERMSG_TO_GATEWAY			"[WINTG_OTHER_MSG_TO_GATEWAY]"

#define GPC_PRINT_ELEC_WIDGET              "[PRINT_ELEC_WIDGET]" // 是否打印电力机车方框
#define GPC_DCMD_NO_REQ_MSG_TYPE           "[DCMDID_REQ_MSG_TYPE]" // 调度命令号码请求使用的消息类型
#define GPC_TG_PIC_SAVE						"[TG_PIC_SAVE]"

#define GPC_STI_NAME						"[STI_NAME]"
#define GPC_STI_PATH						"[STI_PATH]"

#define GPC_DRAGWINDOW_SHOW			    "[DRAGWINDOW_SHOW]"
#define GPC_DRAGWINDOW_DEFAULT		    "[DRAGWINDOW_DEFAULT]"
#define GPC_ROUTE_TRIG_LAST				"[ROUTE_TRIG_LAST]"

#define GPC_HILC_MODE					"[HILCMODE]"



//定时提醒下发阶段计划功能
#define GPC_STPLAN_ALARM					"[STPLAN_ALARM]"

const char USER_TYPES[18][20]={"计划员", "调度员","操作员","调度长","背投","培训","服务","电务维修","绘图","机调员","货调台","统计","AFFAIR_SERVER","SUPER_DISPATCH","OTHER_DISPATCH","系统维护","系统调试","综合维修调度"};

//列控命令号是否需要服务器确认
#define TCC_CMDID_REQUIRE_SERVER_CONFIRM	"[TCC-CMDID-REQUIRE-SERVER-CONFIRM]"
#define TCC_VERSION							"[TCC-VERSION]"
#define TCC_DEFINE                          "[TCC_DEFINE]"
#define INTERFACESTATION_EXIT_ENTRY			"[INTERFACESTATION_EXIT_ENTRY]"

#define ALARMFILTER_PASSWORD				"[ALARMFILTER_PASSWORD]"
#define ALARMFILTER_BYSTA					"[ALARMFILTER_BYSTA]"

struct GPCUSER {
    char    guserid[10];            /// 当前用户ID ,如gpc1,gpc10
    char    gCurrentUserName[20];  /// 当前用户名字，如李主任等
	unsigned long  gLoginDate;     //登陆日期
	int     gLoginShiftid;         //登陆班别
 	int     nHistoryShiftid;       
	int     gTmisLoginShiftid;     //TMIS日班计划
	char	ConsoleName[32];
	int     nGpcId;
	char	UserName[32];
	char    UserRealName[32];//lyc
	char	Password[32];
	long     bSetup;//lyc
	int		bIdentify;		
	int		bPriority;

	vector<int> nSectionList;
	//vector<int> pagelist;
	vector<int> stationlist;
	vector<int> layout_section_list;

	int nCurDispathNo;
	int nOldDispathNo;
	int nLoginType;
	int nAckGpcNo;                       //需要确认的GPC号
	int nTDGpcNo;                     	// 列车索引生成号
	int	getID() { return 0; }
	GPCUSER()
	{
		memset(guserid, 0, sizeof(guserid));
		memset(gCurrentUserName, 0, sizeof(gCurrentUserName));
		memset(ConsoleName, 0, sizeof(ConsoleName));
		memset(UserName, 0, sizeof(UserName));
		memset(UserRealName, 0, sizeof(UserRealName));
		memset(Password, 0, sizeof(Password));
		
		gLoginDate = 0;     //登陆日期
		gLoginShiftid = 0;         //登陆班别
 		nHistoryShiftid = 0;       
		gTmisLoginShiftid = 0;     //TMIS日班计划
		
		nGpcId=0;
		
		bSetup=0;
		bIdentify=0;		
		bPriority=0;

		nCurDispathNo=0;
		nOldDispathNo=0;		
		nLoginType=0;
		nAckGpcNo=0;
		nTDGpcNo=0;
	}
};
 
//编组打印栏目列表
struct GPCPLOTITEM 
{           
   int  nPlotItemNum;
   char sPlotItemName[36];
   GPCPLOTITEM()
	{
		memset(this, 0, sizeof(GPCPLOTITEM));
	}
};

//区段的编组打印配置
struct GPCSECTIONPLOTITEM 
{   
    int nSectionNo;                       //区段号
	int nSectionPlotItemNum;              //区段编组打印个数
	int nSectionPlotItemList[10];         //区段编组列表

	GPCSECTIONPLOTITEM()
	{
		memset(this, 0, sizeof(GPCSECTIONPLOTITEM));
	}
};

 
//区段暂存车站的配置
typedef struct GPCSECTIONPHASESTORETRAIN
{
    int nSectionNo;
	int nStationNum ;
	int nStationList[128];

	float nStationPrintRate[128];//运行图打印比例

	GPCSECTIONPHASESTORETRAIN()
	{
		memset(this, 0, sizeof(GPCSECTIONPHASESTORETRAIN));
	}
}GPCSECTIONPHASESTORETRAIN;
 
struct GPCINTERFACESTATION
{
    int nSectionNo;
	int nStationNum;
	int nStationList[64];

	GPCINTERFACESTATION()
	{
		memset(this, 0, sizeof(GPCINTERFACESTATION));
	}
};

//车站某方向的出入口
struct GPCDIRECTIONENTRY
{    
	char sDirectionName[50];
    int nInNum;
	int nInList[30];
	int nOutNum;
	int nOutList[30];
};

//车站所有方向的出入口
struct GPCSTATIONENTRY
{     
    int nStationNo;
	int nDirectionCount;
	struct GPCDIRECTIONENTRY directionentryinfo[20];
	GPCSTATIONENTRY()
	{
		memset(this, 0, sizeof(GPCSTATIONENTRY));
	}
};
 

typedef struct GPCSPSTATION {
	int nId;//lyc
	int nStation;
	int nStationNum;
	int nStationList[16];
	char sStationName[32]; // MAX_STA_NAME_LEN=32
	GPCSPSTATION()
	{
		memset(this, 0, sizeof(GPCSPSTATION));
	}
} GPCSPSTATION;


typedef struct GPCENTRYEXIT{    
	int entry;
	int exit;
}GPCENTRYEXIT;

struct STATION_SIDES_INFO 
{                                                   
	bool fHigh;                                                     
	int  entryNum;                                                   
	bool entries[64];                                               
	int  sideNum;                                                    
	int  sideno[64];                                                 
	bool sides[64];                                                 
	char sidename[64][10];   
	STATION_SIDES_INFO()
	{
		memset(this, 0, sizeof(STATION_SIDES_INFO));
		fHigh = false;
	}
}; 

typedef struct GPCPLANRECEIVER{
	int nindex;
	int nstationNo;
	int match;
    char sentity[16];
	//char strain_type[16];
    //char sdir[16];	
	int nEntryExit;
	int ntraintye;
    struct GPCENTRYEXIT entry_exit[64];

	GPCPLANRECEIVER()
	{
		memset(this, 0, sizeof(GPCPLANRECEIVER));
		nindex=0;
		nstationNo=0;
		match=0;
		memset(sentity,0,sizeof(sentity));
		nEntryExit=0;
		ntraintye=-1;
		memset(entry_exit,0,sizeof(entry_exit));
	}
}GPCPLANRECEIVER;

typedef struct  GPCNEIGHBERENTRYEXIT{
	int nStationNoMiddle;      //台间站号
    int nStationNoMiddle_neighber;      //台间站 在邻台的站号

	int nEntryExitNum;         //出入口数目
	int nEntryExit_List[6];    //出入口列表
	int nSendStartOrTerminateTrain;
	int nAcceptStartOrTerminateTrainSide;
	int nAcceptThroughTrainSide;
	int nPasswayNo;
	GPCNEIGHBERENTRYEXIT()
	{
		memset(this, 0, sizeof(GPCNEIGHBERENTRYEXIT));
	}
}GPCNEIGHBERENTRYEXIT;

typedef struct NEIGHBERDATA{
     
	int nSectionFrom;     //本台号
	int nSectionTo;       //发送目标台号
	int nGpcNo;           //发送目标gpc号

	int nStationNum;      //台间站个数
    struct GPCNEIGHBERENTRYEXIT Station[6];

	NEIGHBERDATA()
	{
		memset(this, 0, sizeof(NEIGHBERDATA));
	}

}NEIGHBERDATA;

struct GPCERRORSIDESTATION
{
	int nSectionNo;
	int nStationNum;
	int nStationList[64];

	GPCERRORSIDESTATION()
	{
		memset(this, 0, sizeof(GPCERRORSIDESTATION));
	}
};

struct GPCSECTION {
	int		nSection;
	char	szSectionName[32]; // MAX_STA_NAME_LEN=32
	int		nStationNum;
	int		nStationList[64];

	//dmis capture
	int     nNeighberCount;
	int     nNeighberList[32];
	BYTE    nNeighberPropertyList[32];//邻台的属性，0－本系统的DMIS台，1－其他系统的DMIS台

	//neighber gpc
 	int     nNeighberGpcCount;
	int     nNeighberGpcList[32];

	//new added 关于直接发邻台数据的配置
	int     nNeighberSendDataNum;       //配置的记录个数
	struct  NEIGHBERDATA  NeighberSendData[32];
	//

	//view plan capture
	int     nViewPlanNeighberCount; // for view plan use 
	int     nViewPlanNeighberList[32];
	//view plan neighber gpc
 	int     nViewPlanNeighberGpcCount;
	int     nViewPlanNeighberGpcList[32];

    //no use
	int     nRouteCount;
	int     nRouteStationCount[10];
	int     nRouteStationList[10][30];
	int     nRouteStationDelta[10][30];
	
	//new added 2004, for nCTC use;
	//plan receiver 
	int     nPlanReceiverCount;
	int     nPlanReceiverList[100];

	//selfstd & nextstd schedule
	int		nSelfStdFlag;
	int		nNextStdFlag;

	int		nStationNumPhase;//下阶段计划的车站个数
	int		nStationListPhase[64];//下阶段计划的车列表
	
	int	getID(){ return nSection; }
	GPCSECTION()
	{
		memset(this, 0, sizeof(GPCSECTION));
		nSection=0;
		szSectionName[0]=0;
		nStationNum=0;
		nNeighberCount=0;
		nNeighberGpcCount=0;
		nNeighberSendDataNum=0;
		nViewPlanNeighberCount=0;
		nViewPlanNeighberGpcCount=0;
		nRouteCount=0;
		nPlanReceiverCount=0;
		nSelfStdFlag=0;
		nNextStdFlag=0;
		nStationNumPhase=0;
	}
};

struct GPCDISPSECTINFO{
	ushort nSectionNo;
	char lpszSectionName[64];
	vector<int> stationlist;
	
	SIZE	nOriginalSize;	//	Section's original dimension in TLE tools
	SIZE	nDispSize;		//	Section's display dimension on Screen

	int getID() { return nSectionNo; }
} ;


typedef struct ROUTETURNPOINT {
	int nPrevDirect;
	int nStationNo;
	int nNextDirect;
} ROUTETURNPOINT;

typedef struct GPCROUTE {
	int nRouteSetNo;
	int nRouteNo;
	int nStationNum;
	int nStationList[64];
	//lyd 2001/9/20
	int nStationSpaceList[64];
	int nRouteDirect;
	ROUTETURNPOINT spStation;

	int getID(){ return nRouteNo; }
} GPCROUTE;

typedef struct GPCSECTIONROUTE {
 	int nRouteNo;
	int nStationNum;
	int nStationList[32];
 	char sRouteName[32];
	int nRouteDirect;
	GPCSECTIONROUTE()
	{
		memset(this, 0, sizeof(GPCSECTIONROUTE));
	}
} GPCSECTIONROUTE;

typedef struct GPCSECTIONROUTEINFO {
 	int nSectionNo;
	int nRouteCount;
	GPCSECTIONROUTE route_info[32];
} GPCSECTIONROUTEINFO;
typedef struct tag_NEIGHBER_SECTION_INFO{
	int nNeighberSectionCount;
	int nNeighberSectionList[32];
	
}NEIGHBER_SECTION_INFO;
typedef struct tag_GPCSECTIONDISPLAYINFO{
	int nSectionNo;
	int nScreenTotalHeight;
	int nDistanceBetweenSubSection;
	int nSideHeight;
    NEIGHBER_SECTION_INFO nNeighberInfo;
} GPCSECTIONDISPLAYINFO;

typedef struct GPCABSLINEINFO 
{
 	int nLineNo;
  	char sLineName[32];
	int nAbsNo;
	int nStartStation;
	int nEndStation;
	int nDirect;

	GPCABSLINEINFO()
	{
		memset(this, 0, sizeof(GPCABSLINEINFO));
	}
} GPCABSLINENUMINFO;

struct PLAYBACKINFO {
	char abLocalPlaybackPath[64];
	char abRemotePlaybackPath[64];
	int nServerAddressNum,nActiveServerAddressIndex;
	char abServerAddress[4][64];
	char abAffairBackupPath[64];
	char userName[32];
	char passwd[32];
	char mPbShareDir[64];

	PLAYBACKINFO()
	{ 
		memset(this, 0x0, sizeof(*this)); 
		
	}
} ;

struct LOGSVRINFO {
	char abLocalLogPath[64];
	char abRemoteLogSvrPath[64];
	int  nLogSvrAddrNum, nActiveLogSvrAddrIndex;	
	char abLogSvrAddress[4][64];
	char userName[32];
	char passwd[32];

	LOGSVRINFO() {	memset(this, 0x0, sizeof(*this)); }
} ;


struct GPCDCINFO{
	ushort no;//编号
	char dispatchercommandname[32];//调度台名称
	ushort StartCommandNo;//开始命令编号
	ushort EndCommandNo;//终结命令编号

	ushort RcvStaitionNum;//受令车站数
	ushort StartCommandNoWorking;//开始命令编号，施工命令
	ushort EndCommandNoWorking;//终结命令编号，施工命令
	
	ushort RcvStationList[128];//受令车站列表
    ushort nOnDutyLeader;         //对应的值班主任台
	
	ushort RcvGpcNum;         //受令调度台数
	ushort RcvGpcList[128];   //受令调度台列表
    
	ushort RcvGpcProperty[128]; //受令台属性， 0 tdcs, 1 ctc 

	char sStartCommandNo[16];//开始命令编号
	char sEndCommandNo[16];//终结命令编号

	char sStartCommandNoWorking[16];//开始命令编号，施工命令
	char sEndCommandNoWorking[16];//终结命令编号，施工命令
	int getID() { return no; }

	GPCDCINFO() { memset(this, 0, sizeof(GPCDCINFO)); }
};

struct DispatchCommandSave{

	BOOL bSavetoLocalDrive;    //调度命令是否保存在本地
	char sDriveName[32];       //网络驱动器名称
	DispatchCommandSave() { memset(this, 0, sizeof(*this)); } 
};


typedef struct _tagRouteViewChange{
   BOOL bChangeByStationSel;  //车站画面下的车站选择
   BOOL bChangeBySectionSel;  //区段画面下的车站选择
   int  nStationRouteViewPos;  // 单站画面进路窗位置0: 无, 1: 下 2: 左
   int  nSectionRouteViewPos;  // 区段画面进路窗位置0: 无, 1: 下 2: 左
}RouteViewChange;

struct GPCELECTRONICMODULE{	//调度命令区间电网检修模板
	int		nSection;				//区间编号
	char	szSectionName[32];		//区间名字
	int		nElectronicHandNum;		//电力臂个数
	
	int		nStationStartNo[32];	//电力臂起始车站编号
	int		nStationEndNo[32];		//终止车站编号

	int		nStartKilo[32];			//起始公里
	int		nStartMeter[32];		//起始米

	int		nEndKilo[32];			//终止公里
	int		nEndMeter[32];			//终止米	
	GPCELECTRONICMODULE() { memset(this, 0x00, sizeof(*this)); }
};

struct GPCGROUP{  // group_id 的定义
	//; 格式:  GROUP_ID = 标示符  名称  
	//  e.g. 1, GPC_DISPACHER, 调度员  
	int  nGroupId;
	char sExpression[32];
	char sName[32];

	GPCGROUP()
	{
		memset(this, 0, sizeof(GPCGROUP));
	}
};
struct GPCENTITY{ //group entity 的定义
	//; 格式: ENTITY_ID = 标示符  名称  GROUP_ID  STBY_COUNT  特征值2  特征值3 
    // 12346 = stpc1_wchr  1号站值班员  11   2	  1    0
	
	int nEntityId;
	char sExpression[32];
	char sName[32];
	int nGroupId;
	ushort nStby;
	ushort nSpecial1;
	ushort nSpecial2;
	
	GPCENTITY(){ memset(this, 0x00, sizeof(*this)); }
};
 
struct DCAuthority
{
	// 模板号码
	CStringArray aryCmdTempleteNo;
	
	// gpc列表
	CDWordArray aryGPC;

	// 受令人描述
	CString desp;
	
	DCAuthority()
	{
		aryCmdTempleteNo.RemoveAll();
		aryGPC.RemoveAll();
		desp.Empty();
	}
	//char cmd_no[8];//类型编号
	//char cmd_type_name[256];//命令类型名称
	//int  priorityid;
	//char priorityname[32];
	//int  gpccount;
	//int  gpclist[16];
};

typedef struct {
	int   id;
	char type[10];
	char name[10];

}LOCOTYPE;

struct TRAINTYPEINFO
{
	char strPrefix[10];
	int nStart;
	int nEnd;
	int nType;
	int nRunType;
	int nPriority;

	int	nID;
	int getID() {	return nID;	}

};

//; 列控配置
//; 区段号, 区段名称, 列控级别(C2默认: 2 C3: 3), 列控命令号确认, 列控配置目录(TCC+台号)
//[TCC_DEFINE] = 1,  宁武台,     3,		1,      TCC1
//[TCC_DEFINE] = 7,  京沪一台,   2, 		1,      TCC7
//[TCC_DEFINE] = 28, 西陇海台,   2,		1,      TCC28
struct TCC_CFG
{
	int  tcc_type;
	char tcc_dir[10];
	
	bool confirm_tcc_cmdid;
    int  sta_num;
	int  stano[100];
	bool bReInitTcc;  // 是否可以多次初始化列控中心
	bool bIsCanCancelDirect; // 是否可以直接取消限速命令
	int  neib_tcc_type;
	char neib_tcc_dir[10];
	TCC_CFG() 
	{	
		tcc_type=0;
		
		confirm_tcc_cmdid = false;
		memset(tcc_dir, 0, sizeof(tcc_dir));
		sta_num = 0;
		memset(stano, 0, sizeof(stano));
		bReInitTcc = false;
		bIsCanCancelDirect = true;

		neib_tcc_type=0;
		memset(neib_tcc_dir, 0, sizeof(neib_tcc_dir));
	}
};

struct TCC_CFG_NEW
{
	int  con_id;
	int  tdcs_or_ctc; // CTC:1 TDCS:0
	char tcc_type[5];
	TCC_CFG tcc_cfg[2];

	char con_name[50];
	
	TCC_CFG_NEW()
	{
		con_id = 0;
		tdcs_or_ctc = 0;
		memset(con_name, 0, sizeof(con_name));
		memset(tcc_type, 0, sizeof(tcc_type));
		memset(tcc_cfg, 0, sizeof(tcc_cfg));
	}
};

struct SCHD_TYPE
{
	enum
	{
		SHIFT=0,
		MMPLAN=1,
		BASIC=2,
		NEIGHBOUR=3,
		HISTORY=4,
		TCC_MNG=5,
		ELE_ARMS=6,
		BACK_UP_BASIC=7,
		TYPLAN=8,
	};
};

struct StationExitEntry
{
	WORD station;
	BYTE index;

	BYTE up_exit;
	BYTE down_entry;
	BYTE up_entry;
	BYTE down_exit;
};

struct InterfaceStationExitEntry
{
	int con_id;
	BYTE num;
	StationExitEntry station_exit_entry[12];
	InterfaceStationExitEntry()
	{
		con_id=0;
		num=0;
		memset(station_exit_entry, 0, sizeof(station_exit_entry));
	}
};

struct FtpInfo
{
	BYTE init;
	int  port;
	char ip[20];
	char user[20];
	char pwd[20];
	char dirName[100];
};

struct SwitchStation
{
	int num;
	int station[128];
};

struct TGSectionStation
{
	int num;
	int station[128];
};

struct YugaoStation
{
	int num;
	int station[128];
	int neib_station[128];
	YugaoStation()
	{
		memset(this, 0, sizeof(YugaoStation));
	}
};

struct MSSvrTgEntity
{
	int AttributNo;
	int SectionNo;
	long EntityID;
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

struct EntryBySide
{
	int station1;
	int station2;
	int exit;
	int entry;
	int side_num;
	int side[64];
};

struct EntryBySideSection
{
	int section;
	int count;
	EntryBySide info[16];
};

struct SpecialGPCStplanInfo
{
	int 	nSection;
	char    sGpcId[10];
	int     nPlanGroupCount;
	int     nPlanGroupList[100];
};

struct SpecialGPCStplanGroup
{
	int		nIndex;
	char	sDiscription[100];
	int     nPlanReceiverCount;
	int     nPlanReceiverList[100];
};

class NotMeetInfo
{
public:
	WORD	nNoMeetFlag;
	BYTE	nTrainTypeCount;
	CString	sName;
	CString sShortName;
	int		nTrainTypeList[256];

	NotMeetInfo()
	{
		Init();
	}
	void SetNotMeetInfo(WORD nNotMeetFlag, CString name, CString shortname)
	{
		nNoMeetFlag=nNotMeetFlag;
		sName=name;
		sShortName=shortname;
		nTrainTypeCount=0;
		memset(nTrainTypeList, 0, sizeof(nTrainTypeList));
	}
	void Init()
	{
		nNoMeetFlag=0;
		sName="";
		sShortName="";
		nTrainTypeCount=0;
		memset(nTrainTypeList, 0, sizeof(nTrainTypeList));
	}
};

struct BasicPlanReciever
{
	int section;
	int count;
	int reciever[256];
	BasicPlanReciever()
	{
		section=0;
		count=0;
	}
};

struct UDP_ENTITY  // 综合监控端口，实体对应
{
	int udp_port;
	int entity;
};

class  CGpcdef: public CObject
{
public: 
	CGpcdef();
	virtual ~CGpcdef( );

public:
	CPureTextStation	IsPureTextStation;
	CIsHighByteFirst	IsHighByteFirst;
	CDapingServer		GetDapingServerAddr;
	CDapingServer		GetTmisServerName;
	CDapingServer		GetWinvncOptions;
	
	GpcDataTmpl<GPCSTATION>	stainfo;
protected:
	BYTE m_nInterfaceExitEntryNum;
	InterfaceStationExitEntry nInterfaceExitEntry[64];

	int nGroupCnt;
	int nEntityCnt;
	GPCGROUP  groupIdInfo[128];  
	GPCENTITY entity[4096];  
	
	int		nUserIndex;

	GpcDataTmpl<GPCUSER> userinfo;	// added by wsg, Sep 19, 2002
	GpcDataTmpl<GPCSECTION> sectinfo;	//added by wsg for SdLayout.dll
	GpcDataTmpl<GPCDISPSECTINFO>  dispsectinfo;//站场图显示区段
	

    GpcDataTmpl<GPCSECTION> sectinfo_dispatch; ////for ctc only进路序列显示区段
	GpcDataTmpl<GPCROUTE> routeinfo_wsg;	// adde by wsg
	GpcDataTmpl<GPCSECTION> dispdominsectinfo;//站场图管辖范围

	std::vector<SpecialGPCStplanInfo> special_stplanInfo_list;
	std::vector<SpecialGPCStplanGroup> special_stplanGroup_list;

	PLAYBACKINFO pbinfo;//站场回放相关数据
	PLAYBACKINFO pbinforoute;//进路回放相关数据

	LOGSVRINFO   logSvrInfo;
	
	gpcINT	signalRadius;
	gpcINT	lineThickness;
	gpcDIRECTIONSTRING	dirStringInfo;	//wsg Sep 19, 2002


	gpcPAGE	pageinfo;

	GpcDataTmpl<GPCDCINFO> dcinfo_wsg;

	DispatchCommandSave dcSaveinfo;
	RouteViewChange     routeviewchangeinfo;
	int  m_nManualConvertActualTimeLength; // 时间参数
	bool m_bLimitModifyActualTime; // true:限制修改 false:不限制，可以修改
	long m_thisGpcEntity; // 本终端实体
	bool m_bHaveRightManangRight; // 是否有权限管理 用户权限
	
	int  nTenicalStop; //
	int  m_nBasicSchedulePrio;
	int  m_nUpdateTGDataMemory;
	int  m_nDcmdVisualHours;
	int  m_nDcmdReqHours;
	
	int  m_nPrintElecTrainWidget;
	int  m_nDCmdNoReqMsgType;
	int  m_nDCmdMenuStytle;
	int  m_nDcmdAuthorType;
	int  m_nDcmdSendMode;
	int  m_nPreSchdNum;
	int  m_nTgOnce;
	int  m_nViewSchdTG[15];
	// other information
	int		m_nCurrentSectNo;//当前用户在运行图上正在使用的区段，也就是台号console id
    int		m_nScreenNum;
	int		m_nPicturesTip;
	int		m_printFormat;

    int     m_nTrainLeapLineFlag ;  //车站跳跃连线设置  2005 6 17 sunjf 
	int		m_nLineWidth;           //运行线的线宽，粗线条或细线条  2005 6 17 sunjf
    CRect   m_rectLayoutPos, m_rectLayoutPosForSingleStation;

	int		m_TrainIDFormat;//分层打印算法

	char	pathname[512];	// global vars
	char	datapathname[512];	// global vars
	char	m_szGPCDataPath[512];	// global vars
	char	m_szGPCWorkPath[512];	// global vars
	char	szServerAddress[32];
	char    szLocalPathName[512];
	char	szRemotePathName[512];
	char	szRunPathName[128];
	char	szBureauName[128];
	char	szDefaultUser[64];
	char    szPlotterName[64];
	char    szPrinterName[64];
	DECLARE_SERIAL(CGpcdef);

	// 默认列控只读权限
	int  m_nTCCReadOnly; // 0: 只读(查询) 1: 可读写(查询\编辑\删除)
    int  m_nTCCExecute;  // 0: 不能执行   1: 可以执行
	int  m_nDirectExecute;
	int  m_nNeedSynDataToGpc;
	int  m_bmpbitdeep; // 位图位深度
public :
	int  GetBitDeep() {return m_bmpbitdeep; }
	int  IsEnableTgSynDataToGpc() { return m_nNeedSynDataToGpc; }
	bool IsTCCReadOnly(){ return (m_nTCCReadOnly==0);}
	bool IsTCCExecute(){ return (m_nTCCExecute!=0);}
	bool IsDirectExecuteTCC() {return (m_nDirectExecute==0);}
    int  LoadTCCRight(char* ptr);
	bool IsCTCGpc();
	bool IsReInitTCC(int type);
	bool IsCanCancelLimitDcmdDirect(int type);
	BOOL IsTccDefineInit(int con_id);
	BOOL GetOperatorPrior4Basic();

	void SetManualConvertActualTimeLength(int tm);
	int  GetManualConvertActualTimeLength();
	void SetManualConvertActualTime(bool b);
	bool IsInhibitManualConvertActualTime();
	void SetRightManangRight();
	BOOL IsHaveRightManangRight() const;

	int GetDCmdVisualHours(int& reqtimelength);
	
	int IsPrintElecTrainWidget() { return m_nPrintElecTrainWidget;}
	int GetDCmdNoReqMsgType() { return m_nDCmdNoReqMsgType;}
	int GetDCmdMenuStytle() { return m_nDCmdMenuStytle;}
	int GetDcmdAuthorType() { return m_nDcmdAuthorType;}
	int GetAuthorCmdSendMode()  { return m_nDcmdSendMode;}

	int GetPreSchdNum()  { return m_nPreSchdNum;}
	
	BOOL GetbShowElectricTrain();
	int GetDCAuthorityList(CString templeteno,int& gpccount, int* gpclist);
	int GetPrintFormat();
	
	
	BOOL GetOperatorRight(DWORD right);
	int GetPicturesTip();
	int GetSectionPhaseStorePrintRate(int secNo,int &printNum,float* prinRate);
	CString GetPicFilePath(int secNo);
	BOOL CheckPicExist(int secNo);
	char* GetJDServerPWD(char* pwd);
	char* GetJDServerUser(char* user);
	char* GetJDServerAddr(char* servAddr);
	BOOL SetSectionNextStdFlag(int nSection, int flag);
	int LoadJDInfo(char *ptr);
	int GetSectionNextStdFlag(int nSection);
	int GetSectionSelfStdFlag(int nSection);
	int GetStationListofPhase(int nSection, int& stationNum, int* stationList);

	ushort GetDispatcherCommandOndutyGpcNo(int nGpcNo);
	bool GetEntityInfobyName(char *sEntityName, GPCENTITY &entityInfo);
	bool GetEntityInfo(int nGroupid, int nStNo,GPCENTITY &entityInfo);
	bool GetGroupInfo(int nGroupId, GPCGROUP &groupInfo);
	bool GetEntityInfo(const char *sEntityExpress, GPCENTITY &entityInfo);
	bool GetEntityInfo(GPCENTITY &entityInfo, char *sEntityName);// new added 2009 qyt
	bool GetEntityInfo(int nEntityId, GPCENTITY &entityInfo); ////new added 2004
	bool GetGpcEntityInfo(int gpcid, GPCENTITY &entityInfo);
	// 根据特征查找ENTITY, 主要用于查找合适的服务器进程 
    bool GetEntityInfo(int nGroupid, int special1,int special2, GPCENTITY &entityInfo);
	bool LoadEntity(char* entitycfg); // 沈阳 2016.10.12 
	int GetSectionPlanReceivers(int nSectionNo,int *nRecList); ////new 2004
    int GetStationPlanReceivers(int nStationNo,int *nRecList); ////new 2004
    int GetReceiverInfo(int nRecIndex,struct GPCPLANRECEIVER &rec);
	int GetLoginType();

	
	//获取所有自律机实体
	void GetLircEnityList(vector<int>& vec);
	//获取所有rt实体号
	void GetRtEntityList(vector<int>& vec);

    int GetLeapLineFlag();         //sunjf 2005 6 17
	int GetLineWidth();            // sunjf 2005 6 17

	bool m_bSendedLoginOk;
	GPCELECTRONICMODULE* GetEleModule(int sectionNo);
	ushort m_usCadOsSystemFlag;
   	BOOL GetOperatorPrior();
	unsigned long GetCurTimeShiftId(int nYear,int nMonth,int nDay,int nHour,int uProcessId);
	unsigned long GetCurTimeShiftId(ushort uProcessId);

	
	int GetPageInform(int &nPageNum,int *nPageList);
	GPCPAGE * GetPageInform(int lPageNo);
	int GetDefaultSection();
	int GetGpcSectionNum();

    CRect GetLayoutPos();
	CRect GetLayoutPosForSingleStation();
	int  GetLineThickness();
	int	 GetSignalRadius();
	BOOL ShowDirectionString( const int iSectionID, const int iStationID, const char *dirStr );
	CSize GetPageSize(int iPageID = 0 );
	void GetSectionSizeOfDisp( SIZE* originalSize, SIZE* dispSize, int nSectionID=1 );
	char *GetDefaultUserName();

	int GetPBConfig(char * abLocalPath, char * abRemotePath,char *abServerAddress,char *iGetFileWay=NULL,bool dss = true);
	int GetNextServerAddress(char *abServerAddress,int nIndex,bool dss = true);
	int GetNextLogServerAddress(char *abServerAddress, int nIndex);
	int GetServerAddressNum( ) ;
	int GetRouteServerAddressNum( ) ;
	int GetLogServerAddressNum();
	int SetServerAddr(char *abServerAddress);
	int GetCurDispathNo();
	int RestoreOdDispathNo();
	void ChangeUserPassword();
	char* GetDataDir();
	const char *GetXMLDir();
	const char* GetLogDir();  // CUIHU 2008.11.24
	BOOL IsDDMLLocalDrive();
	BOOL IsDDMLSaveToServer();
	char* GetDDMLDataDir(BOOL bDefalutFlag = TRUE, int nSectionNo = 0);
	char* GetShareDataDir(); //共享调度命令，只用来储存申请值班主任， 调度台间互相传送的调度命令
    BOOL IsRouteViewChangeByStation();
	BOOL IsRouteViewChangeBySection();
    int  GetRouteViewStationPos();
	int  GetRouteViewSectionPos();

	struct	GPCSPSTATION	*spstationinfo[512];

	std::vector<GPCPLANRECEIVER> m_vctPlanReceicer;
 
	std::vector<GPCPLOTITEM> plotiteminfo;
	std::vector<GPCSECTIONPLOTITEM> sectionplotinfo;

	std::vector<GPCSTATIONENTRY> m_vctStationEntryInfo;//交接口车站的接入,交出口
	std::vector<GPCINTERFACESTATION> m_vctIfStation;//区段的交接口车站

	std::vector<GPCERRORSIDESTATION> m_vctErrorSideStation; //设置错误股道的车站
	std::vector<GPCERRORSIDESTATION> m_vctNotThisSectionStation; //设置非本台车站
		
	GPCELECTRONICMODULE * elemodule[128];	//区间电网检修
	int nEleModuleNum;

	std::vector<GPCSECTIONPHASESTORETRAIN> m_vctSectionPhaseStation;

    bool  m_bPasswordAndSetupChanged;
	int		nSPStationNum;	// station info
	BOOL bGpcTrain;
    int  m_nTGLineType;
    void SetUserIdentifyAndPriotiry(int nID);
	// export function
	int LoadSetup();
	int SaveSetup();
	void SetGPCWorkDir(const char *path); // must call before LoadProfile or WintgTermLoadProfile887()
	int LoadProfile(CString entitycfg);  // 沈阳 2016.10.12
	int WintgTermLoadProfile(CString entitycfg);  // 沈阳 2016.10.12
	int GetScreenNum();
	int GetStationNum();
	int GetSectionNum();
	int GetUserIdentify();
	int GetUserPriority();
	int GetUserPriority(int);	// only for StdLayout, added by wsg, Sep 15, 2002
	int GetUserGpcId();
	long GetUserSetup();//lyc
	int SetUserSetup(long lSetup);//lyc
	int GetUserIndex();
	int SetUserIndex(int useridx);
	int SetDefaultUser(const char *gpcname=NULL);

	int GetLineSpeed();
	int GetFirstSection();
		////for ctc
	int GetDispatchStationNum(int nSection);
    int GetDispatchStationList(int nSection, int& stationNum, int* stationList);
	
	int GetStationNum(int nSection);
	int GetStationList(int& stationNum, int* stationList);

	bool LoadTGDefProfile(CString pathname);
	int  GetTGStationListForCheck(int con_id, int max_station_list, int stationList[]);
	
	int GetStationListofDisp(int nSection, vector<int>& sta_list);
	int GetStationListofDispDomin(int nSection, vector<int>& sta_list);
	
	int GetSectionCount();
	int GetSectionList(int& sectionNum, int* sectionList);
	int GetNeighberSectionList(int nCurrSectionNo,int& nNeighberSectionNum, int* nNeighberSectionList);
 	int GetNeighberGpcList(int nCurrSectionNo,int& nNeighberGpcNum, int* nNeighberGpcList);
	int GetStationList(int nSection, int& stationNum, int* stationList);
	int * GetSubstation(int nStationNo,int & count);

	int GetNeighberReceiverStaList(int nFromSectionNo,int nToSectionNo,int& nNeighberStaNum, int* nNeighberStaList, int* nNeighberStaList_nei );
	int GetNeighberReceiverStaEntryList(int nFromSectionNo,int nToSectionNo, int nMiddleStaNo,int& nEntryNum, int* nEntryList);
	BOOL IfSendNeighberStartOrTerminateTrain(int nFromSectionNo,int nToSectionNo, int nMiddleStaNo);
	BOOL IfAcceptNeighberStartOrTerminateTrainSide(int nFromSectionNo,int nToSectionNo, int nMiddleStaNo);
	BOOL IfAcceptThroughTrainSide(int nFromSectionNo,int nToSectionNo, int nMiddleStaNo);
    int GetDefaultPassway(int nFromSectionNo,int nToSectionNo, int nMiddleStaNo);

	//int GetPageList(int nSection, int&pageNum, int* pageList);
	// Get layout section list which are controlled by current user
	int GetSectionListOfDisp(vector<int>& sec_list);
	// Get layout section's name
	LPCSTR GetSectionNameOfDisp(int nSectionID);
	LPCSTR GetSectionNameOfDispDomain(int nSectionID);
	
	
		
	int GetNeighberSectionPropertyList(int nCurrSectionNo,int& nNeighberSectionNum, int* nNeighberSectionPropertyList);
   	int GetNeighberViewPlanSectionList(int nCurrSectionNo,int& nNeighberSectionNum, int* nNeighberSectionList);
 	int GetNeighberViewPlanGpcList(int nCurrSectionNo,int& nNeighberGpcNum, int* nNeighberGpcList);
 
	int  GetUserDestineGpcNo(int nSourceGpcNo);
	int  GetTdIdByUserID(int nSourceGpcNo);
	char* GetCurUserName();
	char* GetCurUserRealName();
	char* GetUserName(int useridx);
	char* GetUserPassword(char* username);
	char* SetUserPassword(char* password);
    int GetSectionPhaseStoreStationList(int sectionNo,int& stationNum, int* stationList);  ////
	 
	int	 SetCurrentSectNo(int sect_no);
	int  GetCurrentSectNo();
	int  SetCurrentUserid(const char* useridx);
	int  SetCurrentUsername(const char* username);
	int  SetCurrentUserpwd(const char* pwd);
    char* GetCurrentUserid();
	char* GetCurrentUsername();
	char* GetCurrentUserpwd();
    char* GetPassword();
    int GetConsoleid();
	char* GetCurUserConsoleName();   

	unsigned long GetLoginDate();               //登陆日期
	void SetLoginDate(unsigned long itime);
	int GetLoginShiftid();                     //登陆班别
	void SetLoginShiftid(int inumber);       
	int GetHistoryShiftid();                     
	void SetHistoryShiftid(int inumber);       
        void SetTmisLoginShiftid(int inumber); 
	int  GetTmisLoginShiftid();                 //TMIS日班计划
	char* GetPlotItemName(int inumber);        //获取打印栏目名称
    int GetSectionPlotItemList(int sectionNum, int* sectionplotitemList, int max);
    int GetInterfaceStationList(int sectionNo,int *StationList, int maxSize);

	int GetErrorSideStationList(int sectionNo,int *StationList);

	char *GetEntryStationDirectName(int stationNo,int DirecNo);
	int  GetEntryStationDirectNum(int stationNo);

	int GetEntryStationInListDirec(int stationNo,int DirectNo,int *InList);
	int GetEntryStationOutListDirec(int stationNo,int DirectNo,int *OutList);
    ////for ctc
	char* GetDispatchSectionName(int nSection);
	
	char* GetPlotterName();
	char* GetPrinterName();
	char* GetBureauName();
	char* GetDataPathName();
	char* GetWorkPathName();
	char* GetRunningPathName();
	char* GetAffairDataPathName();
	char* GetDispatchDataPathName(); //////WJH
	char* GetLayoutDataPathName();

	int   GetStationNoByName(const char *name,bool bTGName=TRUE);
    char* GetStationName(int nStation,bool bTGName = TRUE);
	char* GetSectionName(int nSection);
	
	char* GetReplayUserName(bool dss=true);
	char* GetReplayPasswd(bool dss = true);
	char* GetRouteReplayPasswd();
	char* GetRouteReplayUserName();
	char* GetLogSvrUserName();
	char* GetLogSvrPasswd();
	char* GetLogSvrPath();
	char* GetLogLocalPath();
	bool  GetStationProperty(int nStation);
	bool  IsCtcStation(int nStation);
	bool  GetFlagDrawSides(int nStation);
	bool  SetFlagDrawSides(int nStation,bool flag = TRUE);
	bool  GetFlagDrawTrainTime(int nStation);
	
	int FindUserName(const char *username);
	int GetRemoteInfo(char *remotepath, char *serveraddr);
	// perserve version 0.0
	int GetStationSideNum(int nStation);
	int FindRouteInform(int nStationNum, 
			int * nStationList,  GPCROUTE * * RouteInfo);
	int GetSectionFromStationNo(int lStationNo);
	int GetStationParameter(int nFirstStationNo, int nSecondStationNo,
	char* strStationName, CString &fDistance,float &fDeltaDistance,int flag=0);
	CString GetStationKiloMeter(int nStationNo);
	int GetAbsRuntime(int usTrainType, int usSrcStationNo,
		int usDstStationNo, int usDirect, int usRunFlag);

	ushort GetRcvDCStationNum(int no);//返回受令车站数目,参数是指哪个区段
	ushort GetRcvDCStationList(int no,int i);//返回值是受令处所的列表,参数是顺号,i表示第几个受令车站

	ushort GetDispatchNoFromDefaultUser();//根据当前用户号来得到调度台号

	ushort GetAllStationNum();
	ushort GetAllStationList(int i);//得到所有车站的站号列表，i从0开始
	int LoadSectionPhaseStoreStation(char *ptr); //区间暂存车站信息
	int GetTrainIDFormat(){return m_TrainIDFormat;};//获得列车车次号分层配置

    int LoadLocoConfigFile();//读locotype.cfg
	int GetLocoTypeCount() { return m_locolist.size(); }
	BOOL GetLocoTypeByIndex(int index, char *type, char *name, int &id);
	BOOL GetLocoTypeById(int id, char *type, char *name);
	CString GetLocoNameByType(CString no);
	CString GetEngineDesp(CString loco_id);

    BOOL loadFromTrainTypeInfo();
	BOOL IsFreightTrain(char  *strTrainNo);
    int GetTenicalStopDrawWay();

	// 界面控制
	BOOL IsTgOnlyOne();
	BOOL IsFuncMenuViewTgSchd(int type);
	bool IsFormatView();  // 是否显示为总公司统一界面
	
	// 列控命令号是否需要服务器确认
	bool IsTccCmdIDRequireServerConfirm(int type);
	CString  GetTccVersionString();
	CString  GetTccCfgDir(int type);
	bool     GetStaList(int type, int& num, int stano[]);
	
	// 邻台列控级别
	int     GetNeibTCCType(int type);
	CString GetNeibTccCfgDir(int type);
	bool GetInterfaceStation(int con_id, WORD station, BYTE index, BYTE& up_exit, BYTE& down_entry, BYTE& up_entry, BYTE& down_exit);

	FtpInfo mFtpInfo;
	int  LoadFtpInfo(char *ptr);
	bool GetFtpLoginInfo(FtpInfo& ftpInfo);

	char *GetAlarmFilterPassword(){return m_szAlarmFilterPassword;}
	int   GetAlarmFilterBySta()const{return m_nAlarmFilterBySta;}
	BOOL IsDragWindowEnable()const{return m_bDragWindowEnable;}
	BOOL IsDragWindowShow()const{return m_bDragWindowShow;}
	BOOL IsRouteTrigLast()const{return 1==m_nRouteTrigLast;}
protected:
	TCC_CFG_NEW *GetNewTCCCfg();
	const TCC_CFG_NEW *GetTCCCfg_Const(int conid);
	TCC_CFG_NEW *GetTCCCfg(int conid);
	TCC_CFG_NEW m_tcc_cfg[64];

	vector<TRAINTYPEINFO> m_train_type;//列车类型
	BOOL LoadSingleProfile(char* szFileName);
	BOOL LoadPublicProfile(char* szFileName, char* EntityName);
	int LoadTccDefine(char *ptr);
	int LoadEleModule(char *ptr);
 	
	int LoadPlotItemInfo(char * ptr);     //打印的编组信息
    int LoadSectionPlotInfo(char * ptr);  //区段的编组打印信息
    
	int LoadInterfaceStationInfo(char *ptr); //分界口车站列表 
    int LoadStationEntryInfo(char *ptr);     //车站的接入,交出口列表

	int LoadErrorSideStationInfo(char *ptr); //设置问股车站列表
	int LoadNotThisSectionStationInfo(char *ptr); //设置非本台车站

   	int LoadDispatchSectInfo(char *ptr);
	int LoadUserInfo(char *ptr);
	int LoadSectInfo(char *ptr);
	int LoadSecNeighborInfo(char *ptr);
	int LoadSecNeighborReceiverInfo(char *ptr);
	// plan receiver 
	int LoadPlanReceiverInfo(char *ptr);
	int LoadBasicPlanReceiverInfo(char *ptr);

	int LoadSectRouteInfo(char *ptr);
	int LoadStationDeltaInfo(char *ptr);
	int LoadDispSectInfo(char *ptr);
	int LoadDispSectDominInfo(char *ptr);	

	int LoadRouteInfo(char *ptr);
 	int LoadStationSpaceInfo(char *ptr);
	
	int LoadPageInfo(char *ptr);
	int LoadDefaultInfo(char *ptr);
	int LoadBureauName(char *ptr);
	
	int LoadViewSchdTG(char *ptr);

	int LoadSPStationInfo(char *ptr);
	int LoadServerInfo(char *ptr);
	int LoadRouteServerInfo(char *ptr);
	int LoadReplayUserAndPasswd(char *ptr);
	int LoadRouteReplayUserAndPasswd(char *ptr);
	int LoadLogSvrAddr(char *ptr);
	int LoadLogSvrUp(char *ptr);
	
	int LoadLimitModifyActualTime(char *ptr); 
	int LoadDispatcherCommandInfo(char *ptr); 
	int LoadDispatcherCommandSaveInfo(char *ptr); 
	int LoadRouteViewIfChangebyStation(char *ptr);
	int LoadDCAuthorityInfo(char *ptr);
    int LoadLayoutPos(char *ptr);
	int LoadLayoutPosForSingleStation(char *ptr);
    
	int LoadIntValue(char *ptr, short& value);
	int LoadIntValue(char *ptr, int &value);             // 读取整数值
	int LoadIntValue(char *ptr, int& value, int& value2);
	
	int LoadAutoMoveConfig(char *ptr);
	int LoadInterfaceExitEntry(char *ptr);
	//列控
	int LoadTccCmdIDRequireServerConfirm(char *ptr);
	int LoadTccVersion(char *ptr);

	// 沈阳特殊模式
	int LoadSpecialGPCStplanInfo(char *ptr);
	int LoadSpecialGPCStplanGroup(char *ptr);
	
	int SetNeedAuthorityDCNum(int num);
	int GetNeedAuthorityDCNum();
    
	//机调服务器信息
	char m_JDServerAdd[MAX_PATH+1];
	char m_JDServerUser[32];
	char m_JDServerPWD[32];

	//调度命令授权信息
	DCAuthority m_dcAuthority[MAX_DIRECTOR_NUM];
	int m_dcAuthorityNum;
	int m_nNeedAuthorityDCNum;
	vector<LOCOTYPE> m_locolist;//locotype类型
	
	// 调度命令受令单位显示样式
	int m_cmdRecvUnitSytle;
	int m_cmdSendByName;
	
	bool m_bTccCmdIDRequireServerConfirm;
	CString m_szTccVersionString;

	char m_strSTIName[100];
	char m_strSTIPath[300];
private:
	int m_current_con_id;
	int m_nEnablePredict;
	int m_nEnableMMPlanFromCAD;
	int m_nNewBasicDump;
	int m_nShowSideNotConfirmed; // 1:显示 0:不显示
	int m_nEnableBasicDumpTime;

	char m_szAlarmFilterPassword[32];
	BYTE m_nAlarmFilterBySta;
public:
	bool  IsShowStManualSetSide();
	bool  IsNewBasicDump();
	bool  IsEnablePredict();
	bool  IsEnableMMPlanFromCAD();
	bool  IsEnableBasicDumpTime();
	char* GetSTIWinName() {return m_strSTIName;}
	char* GetSTIPathName() {return m_strSTIPath;}
	int GetTccCurrentSectNo();
	int CompleteWintgTermUserInfo();
	void SetTccCurrentSectNo(int con_id);
	const GPCSTATION* GetStationInfo(short nStaID);
	GPCSECTION GetSectionInfo(short nNum);
	short GetRouteCnt();
	GPCROUTE GetRouteInfo(short nNum);

	int GetPhaseNoteReceiverList(int sectionno,int recvlist[],int limit); // 获取阶段记事列表

	int GetSpecialGPCList(int section, int *RecerverList);
	SpecialGPCStplanGroup* GetSpcialGroupByIndex(int index);
	int GetSpecialGPCGroupList(int section, int *GroupList);
	CString GetSpecialGroupNameByIndex(int index);

public: // 车站股道信息 -- 来之StationSide.def 取代了line.cfg
	void SetSidesOfStation(int stano, STATION_SIDES_INFO& info);
	bool GetSidesOfStation(int stano, STATION_SIDES_INFO& info);
	void ClearSides(){mapStationSideInfo.clear();};
private:
	std::map<int, STATION_SIDES_INFO> mapStationSideInfo;
	std::map<int, TGSectionStation> mapTGSectionStation;


public:
	void AddC3TccLineName(long, CString& lineName);
	CString GetC3TccLineName(long);
    void ClearC3TccLineName();

private:
	std::map<long, CString> mapC3TccLineName;

public:
	void AddC2TccLineName(long, CString& lineName);
	CString GetC2TccLineName(long);
    void ClearC2TccLineName();

private:
	std::map<long, CString> mapC2TccLineName;

public:
	int GetYugaoStation(int con_id, int capity, unsigned short list_station[], unsigned short list_neib_station[]);
	int LoadYugaoStation(char* ptr);
private:
	std::map<int, YugaoStation> m_mapYugaoStation;

public:
	void SetShowRouteTrigFlag(int set);
	void SetShowVirtualStation(bool set);
	void SetShowAutoAdjust(bool set);
	void SetShowRouteOkMark(int set);
	bool IsShowRouteTrig(){return m_bShowShowRouteTrig;}
	bool IsShowRouteOkMark(){return m_bRouteOkMark;}
	bool IsShowVirtualStation(){return m_bShowVirtualStation;}
	bool IsShowAutoAdjust() {return m_bShowAutoAdjust;}

	void SetShowTransInfoButton(bool set);
	bool IsShowOPMSTransInfoButton() {return m_bShowTransInfoButton;}

	void SetShowKiloMeterButton(bool set);
	bool IsShowKiloMeterButton() {return m_bShowKiloMeterButton;}
	
private:
	bool m_bShowShowRouteTrig;
	bool m_bRouteOkMark;
	bool m_bShowVirtualStation;
	bool m_bShowAutoAdjust;

	bool m_bShowKiloMeterButton;
	bool m_bShowTransInfoButton;

public:
	int  LoadSectionSwitchStation(char *ptr);
	int  GetSectionSwitchStation(int con_id, int list[]);
private:
	std::map<int,SwitchStation> m_mapSwitchStation;

public:
	int  LoadNotShowSideCtcStation(char *ptr);
	int  GetNotShowSideCtcStation(int con_id, int list[]);
private:
	std::map<int,SwitchStation> m_mapNotShowSideCtcStation;

public:
	bool IsRouteListExecuteForceEnable();
private:
	bool m_bRouteListExecuteForceEnable;

public:
	bool IsAlarmFromSVR(){return m_AlarmFromSVR>0?true:false; }
	bool IsRouteFromSVR(){return m_RouteFromSVR>0?true:false; }

	bool IsHilcMode(){return m_HilcMode>0?true:false;}
	bool IsRunFastTimeCompareWithBasic(){return m_nRunFastTimeCompareWithBasic>0?true:false; }
private:
	int m_AlarmFromSVR;//报警是否从报警服务器获取
	int m_RouteFromSVR;//进路序列是否从进路序列服务器获取
	BOOL m_bDragWindowEnable, m_bDragWindowShow;
	int m_HilcMode;//HILC模式
	int m_nRouteTrigLast;
	int m_nRunFastTimeCompareWithBasic;
	int m_nFomatView; // 是否显示为总公司统一界面
public:
	void LoadMmproUdpEntity(void);  // 加载综合监控端口，实体对应
	void SetThisMMPortByEntity(long eid); // 根据实体设置综合监控端口
	int  GetMSSAlarmPort() {return m_nMSSAlarm;}
	void SetDisableMSSAlarmPort() {m_nMSSAlarm=0;m_nMSSLayoutAlarm=0;}
	bool IsMSSAlarmEnable() { return m_nMSSAlarm>0?true:false;}
	bool IsMSSLayoutAlarmEnable() { return m_nMSSLayoutAlarm>0?true:false;}
	int  GetMSSLayoutAlarmPort() {return m_nMSSLayoutAlarm;}
	int  GetWintgNosynTimeoutTime() {return m_nNosynTimeoutTime;}
private:
	int m_nMSSAlarm; // wintg 综合监控端口
	int m_nMSSLayoutAlarm; // layout 综合监控端口
	int m_nNosynTimeoutTime; // 运行图未同步计数,报警间隔
private:
	int m_RouteListFontsize;
	int m_nTrainNoLevelCount;
	std::vector<BasicPlanReciever> m_basicPlanReciever;

public:
	int GetRouteListFontsize(){return m_RouteListFontsize;}

//检测TG服务器状态
public:
	bool AddSvrTgEntityIntoMap(MSSvrTgEntity svrtg);
	int  GetSvrTgEntityCnt() { return m_mapMSSvrTgEntity.size();}
	long GetSvrTgEntityIDByIndex(int index);
	long GetSvrTgAttributNoByIndex(int index);
private:
	std::vector<MSSvrTgEntity> m_mapMSSvrTgEntity;

public:
	bool IsUseVirtualMemory() { return m_bUseVirtualMemory;}
	bool IsEnableModifyTsrStatus() { return m_bModifyTsrStatus; }
	int  GetWidePixel() { return m_nWidePixel; }

	bool IsTgMsgToGateway() { return m_bTgMsgToGateway; }
	bool IsOtherMsgToGateway() { return m_bOtherMsgToGateway; }

	void SetThisEntity(long e) { m_thisGpcEntity=e;}
	long GetThisEntity() { return m_thisGpcEntity;}
	bool IsNewAutoMoveMode() { return m_bAutoMoveTime;}
	int  GetHistoryHourLen() { return m_nHistoryHour; }
private:
	int  m_nWidePixel;
	bool m_bUseVirtualMemory;
	bool m_bModifyTsrStatus;
	bool m_bTgMsgToGateway;
	bool m_bOtherMsgToGateway;
	bool m_bAutoMoveTime; // true 自动平移模式--类似东方宇业 false 老模式,需要调度员交接班
	int  m_nHistoryHour; // 自动平移模式,调整图历史小时数
public:
	void EnableAssistantModifyPropTree(BOOL va);
	bool IsEnableAssistantModifyPropTree() { return m_bEnableAssistModifyPropTree;}
private:
	bool m_bEnableAssistModifyPropTree;

	// 兰州局打印机车号特殊配置
public:
	std::vector<LocoNoDisplay> m_loco_no_display_list;
	bool IsLocoNoDisplay(int conid, int station, int arrival_index, int depart_index, int direction, int add_or_sub);

public:
	void LoadEntryBySide();
	bool IsThisSectionHaveLineNoBySide(int section);
	bool GetStationExitBySide(int section, int station1, int station2, int side1, int& exit, int& entry);
	bool GetStationEntryBySide(int section, int station1, int station2, int side2, int& exit, int& entry);

private:
	int m_nEntryBySideSectionCount;
	EntryBySideSection mEntryBySideSection[64];
	std::vector<UDP_ENTITY> m_listUdpEntity; // 保存监控端口和实体对应
	
private:
	//区间禁会信息
	int m_nNotMeetInfoCount;
	NotMeetInfo mNotMeetInfo[8];

public:
	bool LoadNotMeetBySpeed();
	void LoadNotMeetByType();
	int  getNotMeetInfoCount() { return m_nNotMeetInfoCount;}
	bool getNotMeetInfoByNo(BYTE idx, NotMeetInfo& info) const;
	bool getNotMeetInfoByNoMeetFlag(WORD no_meet_flag, NotMeetInfo& info) const;
	int  GetTrainNoLevelCount() { return m_nTrainNoLevelCount; }
	int  GetBasicPlanStation(int section, int station_list[]);
	bool IsBasicPlanIndex(int section, int index_rec);
	bool IsNotThisSectionStation(int sectionNo, int stationNo);
};

#endif
