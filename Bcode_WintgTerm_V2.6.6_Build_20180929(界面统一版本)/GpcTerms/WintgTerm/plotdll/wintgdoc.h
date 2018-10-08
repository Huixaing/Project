#if !defined(AFX_WINTGDOC_H__27E02CFA_7B29_11D1_9CEF_000021000CE6__INCLUDED_)
#define AFX_WINTGDOC_H__27E02CFA_7B29_11D1_9CEF_000021000CE6__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// WintgDoc.h : header file
//
#include "tgmsg.h"

#define IsTrain(train_info)            (!(((train_info.type)>>7) &0x01))
#define IsTruck(train_info)            (((train_info.type)>>7) &0x01)
#define IsCrhTrain(train_info)         (((train_info.type)>=17 && (train_info.type)<=19) || (train_info.run_type)==5)
#define IsFlagPassNonMainSide(flag) ((flag & 0x0400) ==  0x0400)     //modify by wg 0809, 0 -- no, 1 -- yes
#define IsFlagStopAbsUp(flag)       (flag & 0x2000)

#define ERR_NOT_FIND -1
#define FIND_ONCE	1
#define FIND_TWICE 2
#define SHOW_MUTI_TRAINID_SEPARATE  //多车次显示是否分散

#define MINIMUM_UNIT	25
#define WHOLE_DAY_SECOND	86400
#define WHOLE_DAY_MINUTE	1440
#define MAX_MULTI_OPTION_NUM  10

#define HISTORY_TRAIN_TYPE		1
#define SCHEDULE_TRAIN_TYPE		4
#define WAITING_TRAIN_TYPE		2
#define ADJUST_TRAIN_TYPE		3
#define BASIC_TRAIN_TYPE		5
#define NEIGHBER_TRAIN_TYPE		6

#define SCHEDULE_TRAIN_BASE 0
#define WAIT_TRAIN_BASE		2000
#define ADJUST_TRAIN_BASE	3000

#define MAX_WAIT_TIME   100 //add by cf after 7-18


// some constant for drawmode
//#define DRAW_SCREEN	0x10000000
#define DRAW_SCREEN	   0x01
#define DRAW_NORMAL    0x02
#define DRAW_ERASE	   0x04

// some constant for highlow line
#define ENTRY_POSITION	    0
#define EXIT_POSITION		1
#define TOP_POSITION		2
#define BOTTOM_POSITION   3

//track  
#define TOP_TRACK	    	1
#define PRIMARY_TRACK		2
#define BOTTOM_TRACK        3

//机车绘制位置,0表示不显示
#define TRACTION_NOSHOW 0
#define TRACTION_TOP 1
#define TRACTION_MIDDLE 2
#define TRACTION_BOTTOM 3

#define X_TIME_OFFSET   0

#define MAX_DIRECTION_NUM 20


#define IsDefinePass(rec)                 ((rec).stop_condition & 0x02)
#define IsForcePass(rec)                  ((rec).stop_condition & 0x04)
#define IsForceSide(rec)                  ((rec).stop_condition & 0x08)
#define IsForcePassByMainSide(rec)        ((rec).stop_condition & 0x10)

#define SetDefinePass(rec)                ((rec).stop_condition |= 0x02)
#define SetForcePass(rec)                 ((rec).stop_condition |= 0x04)
#define SetForceSide(rec)                 ((rec).stop_condition |= 0x08)
#define SetForcePassByMainSide(rec)       ((rec).stop_condition |= 0x10)

#define ReSetDefinePass(rec)              ((rec).stop_condition &= ~0x02)
#define ReSetForcePass(rec)               ((rec).stop_condition &= ~0x04)
#define ReSetForceSide(rec)               ((rec).stop_condition &= ~0x08)
#define ReSetForcePassByMainSide(rec)     ((rec).stop_condition &= ~0x10)


//自动触发标记的判断和设置
#define IsAutoMakeArriveRoute(rec)        (!((rec).stop_condition & 0x20))
#define IsAutoMakeDepartRoute(rec)        (!((rec).stop_condition & 0x40))
#define SetAutoMakeArriveRoute(rec)       ((rec).stop_condition &= ~0x20)
#define SetAutoMakeDepartRoute(rec)       ((rec).stop_condition &= ~0x40)
#define SetNotAutoMakeArriveRoute(rec)    ((rec).stop_condition |= 0x20)
#define SetNotAutoMakeDepartRoute(rec)    ((rec).stop_condition |= 0x40)

//发车时机：根据 1 避让 2 计划
#define IsMakeDepartRouteByMeet(rec)      (((rec).stop_condition & 0x80) == 0x00)  //是否根据避让关系
#define SetMakeDepartRouteByMeet(rec)     ((rec).stop_condition &= ~0x80)
#define IsMakeDepartRouteByPlan(rec)      (((rec).stop_condition & 0x80) == 0x80)  //是否根据计划
#define SetMakeDepartRouteByPlan(rec)     ((rec).stop_condition |= 0x80) 

//技术停点相关
#define IsTechStopTrain(rec)        (((rec).adjust_status & 0x0010))
#define SetTechStopTrain(rec)       ((rec).adjust_status |= 0x0010)
#define ClearTechStopTrain(rec)     ((rec).adjust_status &= ~0x0010)

#define EXCEPTION_MARK 0
#define BAD_MARK       1
#define REVERSE_MARK   2
#define STOPREASTON_MARK 3

#define PI	3.14159265359
#define EMIN  1E-10
#define EMAX 

#define PRIO_DEF ("prio_def.def")

#define TRAIN_TYPE  ("TRAIN_TYPE")
#define ABS_TYPE    ("ABS_TYPE")
#define PRIO_DATA  ("RUN_PRIORITY")
//#define STEP_OF_SIDES   8//10
/////////////////////////////////////////////////////////////////////////////
// CWintgDoc document
#include "oscalls.h"
#include "defines.h"
#include "memshare.h"

#include "tgtype.h"
#include "distance.h"
#include "tgtime.h"
#include "dbfunc.h"
#include "widgets.h"
#include "resource.h"
#include "shapeobj.h"
#include "tgmsg.h"
//#include "MacroSwitch.h"
//#include "NewDispatcherCommand.h"
#include "stationDef.h"

#include "function.h"
#include <vector>  


class CMultiAbsMark;
class CAbsMark;


//zgj for compute train id output layer
struct PTofTrainID{
	int stationno;
	BYTE layer;
	CPoint ptActual;
};

using namespace std ; 
 
typedef struct _tagStatusStation{
	int station_index;
	byte station_status;
	byte lirc_status;
} STATUS_STATION;


Pstruct _tagTimestamp {
	int nCurrentDate;
	int nCurrentTime;
	ushort usMemoryFlag;
	ushort usOperateFlag;
	int nDialogShowFlag;
};

typedef Pstruct _tagMasterRoute {
	int nRouteSetNo;
	int nRouteNo;// current route info subscript
	int nAllRouteNo;// all route info subscript
} MASTERROUTE;

struct DISPDISTANCE
{
	int nRouteNo;
	int nRouteSetNo;
	
	int nSecondRouteSetNo;
	int nStationNo;
	int nStationYPos;
	int nSecondStationYPos;
	//bool fHigh;
	
 	int nSideYPos[MAX_SIDES];
	
 	Pstruct _distance distance;
};

typedef Pstruct _TrainPerPointInfo{
    ushort record_index;
	ushort station;
    ushort start_depart_flag;//start,depart ,start and depart
	bool  select_flag;
    CRect rect;
	byte nStatus;   // route status
}TRAINPERPOINTINFO;
typedef Pstruct _tagTerminatePoint {
	int nStartIndex;// index of struct dispdistance
	int nEndIndex;
} TERMINATEPOINT;
//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WINTGDOC_H__27E02CFA_7B29_11D1_9CEF_000021000CE6__INCLUDED_)
