#pragma once
//#include "defines.h"

// display flag define
struct TRAIN_DISPLY
{
	enum
	{
		DIRECT = 1,
		TYPE,
		LATETRAIN,
		LINE,
		REVERSE,
		TRACTION,
	};

	struct DIRECT
	{
		enum 
		{
			UP =	1,
			DOWN	= 2,
			DUAL	= 3,
		};
	};
	struct TYPE
	{
		enum{
			FREIGHT = 1,
			PASSENGER	= 2,
			ALL	= 3,
		};
	};
	struct LATETRAIN
	{
		enum{
			LATE_FREIGHT_TRAIN = 1,
			LATE_PASSENGER_TRAIN = 2,
			LATE_ALL_TRAIN	 = 3,
		};
	};
	struct LINE
	{
		enum{
		THIN =	1,
		BOLD =	2,
		};
	};
	// draw traction position
	struct TRACTION
	{
		enum{
		NOSHOW	=	0, 
		TOP=	1,
		MIDDLE=	2,
		BOTTOM=	3,
		};
	};
	struct REVERSE
	{
		enum{
		SHOW_WAVE = 1,  // 待避波浪线
		SHOW_REVERSE	=		2,  // 显示反向行车
		SHOW_ENGINE		=		3, // 显示机车号
		EDIT_REAL_LINE	=		4,  // 实际线是否可以修改
		SHOW_KILO_MARK	=		5,  // 是否显示公里标
		BY_PASS_FLAG	=		6,  // 侧线通过标志
		CONSIDER_SIDE	=		7,
		ADJUST_AFFIRM	=		8,
		ADJUST_PART_RESULT	=	9,
		SHOW_TRAIN_ID		=	10,
		SHOW_EARLY_LATE_TIME=	11, // 显示早晚点
		SHOW_CONFLICT		=	12, // 显示冲突
		SHOW_ADJUST_ERROR	=	13,
		SHOW_ERROR			=	14,
		SHOW_ELEC_LOCO		=	15,
		ACCEPT_TRUCK_LATE	=	16,
		SHOW_ACCEPT_TRAIN_LATE	=17,
		SHOW_FAST_SLOW_TIME		=18,
		SHOW_TRUCK_ON_TIME	=	19, 
		SHOW_FALSE_TRAIN	=	20, 
		SHOW_TRAIN_ON_TIME	=	21,
		SHOW_WORK_SIDE		=	22,
		SHOW_ERR_WORK_SIDE	=	23,
		SHOW_WHOLE_SCREEN	=	24,
		SET_SELF_NEXT_STD	=	25,
		SHOW_BASIC_SCHD_SIDE =  26,
		};
	};
};

struct STATION
{
	enum{
		DISTRIBUTED,	// 分散自律
		CENTER_CTRL,	// 中心完全控制
		STATION_CTRL,	// 车站完全控制
		EMERGENCY,	// 非常站控
	};
};

struct ROUTE
{
	enum{
		PLAN_CTRL,	 // 按图排路
		SAVE_ROUTE,  // 存储进路
		FORBIT_AUTO, // 人工排路
	};
};


struct STATION_STATUS
{
	enum{
		ACTIVE = 0,	   // 站活
		INACTIVE,  // 站死
	};
};
// trains operator
struct TWO_TRAIN_OPERATOR
{
	enum  OPER
	{
		SPLIT_TRAIN	= 1,
		COMBINE_TRAIN = 2,
		UNITE_TRAIN = 3,
		RESCUE_TRAIN = 4,
	};
	struct RESULT
	{
		enum 
		{
			OPER_OK = 1,
			OPER_CANCEL = 2,
		};
	};
};

// draw special train flag
struct DRAW
{
	struct TRAIN
	{
		enum{
		RETURN			=1,	// 区间折返
		GOON			=2,	// 区间前行
		TO_ABS_TRAIN	=3,   // 区间作业 至区间
		FROM_ABS_TRAIN	=4,   // 区间作业 区间返回
		};
	};
	struct MARK
	{
		enum{
		IF_ABS_INHIBIT		=1,	// 区间封锁
		IF_ABS_SLOW			=2,	// 区间慢行
		IF_ABS_ELEM_FIX		=3,   // 区间电网检修
		IF_ABS_SELF_CLOSE	=4,   // 区间变更闭塞方式
		STATION_INHIBIT		=5,	// 站内封锁
		STATION_SLOW		=6,	// 站内慢行
		STATION_FIX			=7,	// 站内天窗修
		STATION_SIDE_OCCUPY	=8,	// 站内股道占用
		TEXT_NOTE			=9,	// 文字注释
		GRAH_NOTE			=10,	// 图解标注
		PE_NOTE				=11,	// 记事标注
		OTHER_MARK			=20,	// 其它
		};
	};
};
struct GRAPH_ZOOM
{
	enum{
		SMALL = 1,
		NORMAL = 2,
		BIG = 3,
	};
};

struct TG_TYPE
{
	enum 
	{
		WORK_TG  = 1,
		BASIC_TG = 2,
		QUERY_TG = 3,
	};
};
// 背景
struct BACKGROUD
{
	struct BLOCK
	{
		enum {
		ONE  = 1,
		TWO  = 2,
		FIVE = 5,
		};
	};
	
	struct TIME
	{
		struct BEGIN
		{
			enum {
			ZERO  = 0,
			SIX   = 6,
			TOWNTY = 12,
			};
		};
		struct LENGTH
		{
			enum {
			TOWNTYFOUR  = 24,
			THIRTY		= 30,
			THIRTYSIX	= 36,
			};
		};
	};
};

struct RECOVER
{
	enum{
		SINGLT_TRAIN = 1,
		MULTI_TRAIN
	};
};

struct PS_LINE
{
	enum 
	{
		SOLID = 1,
		DASH  = 2,
	};
};

struct OPER_RIGHT
{
	enum
	{
	NONE_EDIT=	0,
	TG_EDIT	=	1, 
	MARK_EDIT=	2,
	TG_AND_MARK_EDIT = 3,
	};
};

//struct TRAIN_INFO_TYPE
//{
//	enum{
//		WHOLE_TRAIN_INFO = 1,
//		STATION_ABS_LOAD_TEXT = 2,
//		WHOLE_CONSOLE_TRAIN_INFO = 3,
//		ENGINE_ADJUST_BEGIN_STATION = 4,
//		CHANGE_TRAIN_ID = 5,
//		CHANGE_MISC_TRAIN_INFO = 6,
//		CHANGE_BASIC_TRAIN_INFO = 7,
//		WHOLE_TRAIN_INFO_INSERT = 8,
//	};
//};
//
//struct RECORD_CHANGE_TYPE
//{
//	enum{
//		SCHD_TIME_STATUS_FLAG = 10,
//		SCHD_CHANGE_ONE_TRAINRECORD = 12,
//		SCHD_CHANGE_SIDE = 13,
//		SCHD_ADJUST_SUB = 14,
//		SCHD_REPORT_POINT = 15,
//		SCHD_CHANGE_TRAIN = 16,
//	};
//};