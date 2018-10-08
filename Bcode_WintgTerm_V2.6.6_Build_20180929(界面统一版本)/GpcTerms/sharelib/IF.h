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
		BSCHD_OPER,
		MUTI_SEL,
		KEY_DOWN,
		EDIT_REAL_TIME,
		LOCK_SIDE,
		GREEN_DECORATE_LINE,
		TRAIN_CZH,
		SWITCH_STATION,
		NOT_SHOW_SIDE_CTC_STATION,
		RECIEVE_POINT_LIMIT_TIME,
		LOCK_SIDE_NOT_TECH_STOP,
		SET_MOVE_SAME_CZ_TRAIN,
		SET_NOT_THIS_SECTION_STATION,
		NOT_DELETE_REAL_TIME,
		CHANGE_TRAINID_TIME_POS_VIEW, // 宁杭折返车次特殊显示--不用了
		SET_NO_LIMIT_STATION, // 设置封闭运行图中不封闭的车站
		SET_LIMIT_MODIFY_ACT_TIME, // 是否限制修改实际点
		GEN_TRAINS_RELATION, // 生成当前列车车组关系文件czh_relation.xml 20150623珠三角城际
		INTER_CITY_STPLAN, // 支持城际计划开关 --珠三角城际20150623
		ERROR_SIDE_STATION, // 设置问股车站,济南局
		ENABLE_SHOW_RUNTIME_WND, // 是否显示区间运行时分窗口
		DRAW_FOUCS_TRAIN_ACCEPT_GIVEOUT_MARK, // 是否绘制焦点车接入交出符号
		DISENABLE_SHEET_TAB,	// 是否屏蔽右侧属性窗口， 需要打开对话框中进行电力属性恢复
		TRAIN_NEW_CZH, // 新车组关系
		SET_TRAIN_RETURN_ID, // 设置车组是否修改运行线数据
		SHOW_TRAIN_NO_LEVEL_COUNT,	// 设置车次号显示层数
		BASIC_MOVE_NULTI_STATION, // 是否基本图可以整线平移
		ERASE_BEFORE_MODIFY, // 拖线时,不保留老线
		DISABLE_MODIFY_PLAN_TM, // 禁止修改图定时间
		RUN_FAST_SLOW_WITH_BASIC, // 区间运缓同基本图比较显示
		SET_PIXEL_PER_MINUTE, // 设置每分钟像素数,用于放大缩小使用
	};
};

struct GREEN_DECORATE_LINE
{
	enum 
	{
		DIESEL_LOCOMOTIVE = 0,
		ELEC_LOCOMOTIVE	= 1,
	};
};

struct MUTI_SEL
{
	enum 
	{
		TRAIN			= 1,
		MARK			= 2,
		TRAIN_AND_MARK	= 3,
	};
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
	SHOW_EARLY_LATE_TIME=	11, // 显示早晚点
	SHOW_CONFLICT		=	12, // 显示冲突
	SHOW_ADJUST_ERROR	=	13,
	SHOW_ERROR			=	14,
	SHOW_ELEC_LOCO		=	15,
	ACCEPT_TRUCK_LATE	=	16,
	ACCEPT_TRAIN_LATE	=	17,
	SHOW_FAST_SLOW_TIME	=	18,
	SHOW_ERR_WORK_SIDE	=	23,
	
	SHOW_BASIC_SCHD_SIDE =  26,
	SHOW_ABS_DESP =  27,
	SHOW_TECH_STOP_MARK=28,  // 是否显示技术停点三角符号
	//ENABLE_LOCK_STATION_SIDE=29,// 允许锁定股道
	SHOW_ROUTE_NOT_AUTO_TRIGER_ROUND=30, // 是否显示进路非自触双圆圈
	SHOW_ROUTE_TRIGER_OK_FLAG=31, // 是否显示进路已触发标志
	SHOW_SEL_CZH_LINE=32, // 是否显示选中车组勾连线
	RECEIVE_POINT=33, // 鼠标收点
	ENABLE_CZH_OPER=34, // 允许车组号
	SHIFT_SCHD_OPER=35, // 日班计划开关
	SHOW_RETURN_TRAINID=36, //显示折返车次
	SHOW_DEPART_MEET_MARK=37,  // 是否显示发车时机星号
	SHOW_VIRTUAL_STATION=38, //显示虚拟车站
	EDIT_TRIGER_LINE_TIME=39, // 暂时不用,编辑进路已触发到发时间
	MOVE_SAME_CZ_TRAIN=40, // 是否平移同车组后续列车
	USE_STRING_KILO_METER=41, // 是否使用字符串公里标
	SHOW_TRANS_INFO=42, // 是否显示运输信息
	SHOW_ALL_CZH_LINE=43, // 是否显示所有车组勾连线
	SHOW_ENGINE_CREW_INFO=44, // 是否显示沈阳机务信息菜单
	FIRE_REAL_TIME_EVENT=45, // 是否发出实际点事件,不再使用了
	GOOD_TRAIN_STAY_TO_PASS_NEED_PWD=46, // 货物列车非技术停点,到开变通过输密码
	SHOW_PLAN_NOTE_TEXT=47,	// 是否显示计划台下发的重点事项
	WHOLE_SCREEN_STATE=48, // 全屏显示状态
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
		IN_ABS_PART_RUN = 5,
		ATT_ENGINE_RETURN = 6,
		IN_ABS_WHOLE_HELP_RUN = 7,
	};
};

// draw special train flag
struct TRAIN
{
	enum{
		RETURN			=1,	// 区间折返
		GOON			=2,	// 区间前行
		TO_ABS_TRAIN	=3,   // 区间作业 至区间作业
		FROM_ABS_TRAIN	=4,   // 区间作业 区间作业返回
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
	IF_ABS_TOTAL_FIX	=10,	// 区间综合天窗检修
	PE_NOTE				=11,	// 记事标注
	STATION_SIDE_LOST_ELE	=12,	// 站内股道停电
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

struct TERM_TG_TYPE
{
	enum 
	{
		NOW_BASIC_TG = 1,   // 在用基本图
		PRE_BASIC_TG = 2,   // 预览基本图
		READY_BASIC_TG = 3, // 归档基本图
		SHIFT_TG = 4,
		WORK_TG  = 5,
		QUERY_TG = 6,
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
