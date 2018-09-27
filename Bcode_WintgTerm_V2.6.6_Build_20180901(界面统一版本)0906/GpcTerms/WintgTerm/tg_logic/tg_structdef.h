#ifndef _DEF_SCHEDULE_H
#define _DEF_SCHEDULE_H

/*  this file contains schedule related data struct  */

#include "tg_defines.h"
#define  STR_CPY(x,y)   {strncpy(x,y,sizeof(x)-1);x[sizeof(x)-1]=0;}
enum TRAIN_IN_OUT 
{
	TRAIN_OUT_STATION,
	TRAIN_INTO_STATION
};
enum ARRIVE_DEPART_STATUS
{
	STATION_NULLSTATUS,
	STATION_ARRIVE,
	STATION_DEPART,
	STATION_PASS,
	MAX_ACT_TYPE_COUNT=3
};
enum OCCUPY_TYPE 
{
	OCCUPY_NULL,
	OCCUPY_ABS,
	OCCUPY_STA,
	OCCUPY_SAME_SIDE,
	OCCUPY_SIDE_AFTER_TERMINAL
};
enum GAP_TYPE 
{
	OCCUPY_SAME_SIDE_GAP=0,
	OCCUPY_SIDE_AFTER_TERMINAL_GAP=1,
	CONTINUE_DEPART_GAP=2,
	MEET_TRAIN_GAP=3,
	DIFF_ARRIVE_GAP=4,
	ABS_BLOCK_GAP=5,
	SIDE_BLOCK_GAP=6,
	TRACE_GAP=10,
	NO_GAP=19
};
enum RUN_TYPE 
{
	NULL_RUNTYPE,
	FREIGHT_TRAIN,
	PASSANGER_TRAIN,
	MAX_RUN_TYPE_COUNT=2
};
enum LINEBLOCK_TYPE 
{
	NULL_LINEBLOCK,
	AUTO_BLOCK,
	HALF_BLOCK
};

enum ELEMENT_STATUS_TYPE
{
	TG_ABS_INHIBIT=1,
	TG_ABS_FIX_MARK=2, // 综合天窗修
	TG_ABS_SLOW=3,
	TG_SIDE_SLOW=4,
	TG_SIDE_INHIBIT=5,
	TG_GRAPH_NOTE=7,
	TG_CHANGE_BLOCK_TYPE=9,
	TG_SIDE_TIANCHUANG=10,
	TG_SECTION_ELEC_FIX=11,
	TG_SIDE_OCCUPY=12,
	TG_SECTION_ELEC_FIX_LANZHOU=16
};

enum NOT_MEET_BIT_FLAG
{
	NOTMEET_CRH       =0x01,
	NOTMEET_SEPECIAL_Z=0x02,
	NOTMEET_PASSENGER =0x04,
	NOTMEET_FREIGHT   =0x08,
	NOTMEET_SPACE     =0x10,

	NOTMEET_200_orUp=0x0100,
	NOTMEET_120_200 =0x0200,
	NOTMEET_120_Low =0x0400,
	NOTMEET_ALL     =0x0800,
};

#define TG_IS_ABS_MARK(x)  (x==TG_ABS_INHIBIT || x==TG_ABS_SLOW || x==TG_SECTION_ELEC_FIX || x==TG_ABS_FIX_MARK || x==TG_SECTION_ELEC_FIX_LANZHOU)
#define TG_IS_INSTATION_MARK(x)  (x==TG_SIDE_SLOW || x==TG_SIDE_INHIBIT || x==TG_SIDE_TIANCHUANG || x==TG_SIDE_OCCUPY)

enum TRAIN_MEET_RELATION
{
	NO_MEET=0,
    DIFF_DIR_MEET=1,
    SAME_DIR_MEET=2
};

struct TG_MsgStamp{
	int	          con_id;
	int           schedule_type;
	TRAIN_INDEX   index;
	TIME          ltime;
	ENTITY_ID     operatr;
};
/*   列车机车信息  */

struct TG_TrainEngineInfo
{
	TRAIN_NO     engine_id;            // 机车号
	TRAIN_NO     engine_type;          // 机车类型
	long         engine_status;        // bit 7-6:  from dispatcher/subao
                                      // bit 5-4:  0: traction  1 - attach
									  // bit 0-3:  0:内燃 1:电力
	short       attach_sta;           // index of trainrecord
	short       unattach_sta;
};

/* 列车区间停车信息 */
struct TG_TrainAbsStop
{
	BYTE    flag;     // 1: stop out of station, 0: abs stop
	short   position;
	TIME    start_time;
	TIME    stop_time;
};

/* 列车时刻信息 */
struct TG_TrainRecord
{
	TRAIN_NO     arrive_train_id;                       // 到达车次
	TRAIN_NO     original_arrive_train_id;              // 原始到达车次，戴帽时使用

	TRAIN_NO     depart_train_id;                       // 出发车次
	TRAIN_NO     original_depart_train_id;              // 原始出发车次，戴帽时使用

	STATION_NO   station;                               // 站号
	ENTRY        entry;                                 // 入口号
	ENTRY        exit;                                  // 出口号

	BYTE     arrive_side;                               // 到达股道
	BYTE     plan_arrive_side;                          // 图定到达股道
	BYTE     depart_side;                               // 出发股道
	BYTE     plan_depart_side;                          // 图定出发股道
	short    min_stop_time;                             // 营业时间

	TIME    arrive;                                     // 到达时间
	TIME    depart;                                     // 出发时间
	TIME    plan_arrive;                                // 图定到达时间
	TIME    plan_depart;                                // 图定出发时间
	TIME    send_plan_time;                             // 计划下达时间
	long    stop_reason;
	long    late_reason;

	long    early_depart_time;                          // 允许提前发车时间
						// 0x0000 -> 0x7FFF, 允许提前发车的秒数
						// 0x8000 允许提前发车无限制
						// 0x0000 不允许提前发车
	long    run_fast_slow_time;                         // 赶缓点信息
						// bit 7:   0 - 快,   1 - 慢
						// bit 6-0:   赶缓点时间 (分钟)

	long    route_flag;                                 // 进路办理状态
						// bit 1-0:   00/01/10/11  接车进路  no(clear)/doing/OK/OCCUPY
						// bit 3-2:   00/01/10/11  发车进路  no(clear)/doing/OK/OCCUPY
	long    rank;		// 重点标记信息和TMIS对应
						// bit 0:  超限---不使用必须为0和老数据兼容,由bit16-bit21替代
						// bit 1:  重点警卫
						// bit 2:  毒品
						// bit 3:  军运
					    // bit 0-bit15该字段和TMIS对应, 不可扩充使用

						// bit16-bit18: 到达超限 0:正常 1:超级超限  2:一级超限 3:二级超限 4:三级超限
						// bit19-bit21: 出发超限 0:正常 1:超级超限  2:一级超限 3:二级超限 4:三级超限

	long    speed_limit;								// 单个列车限速信息
						// 0: 无限速, else: 最大限速值

	long    stop_condition;
						// bit 1:    1-原定义为通过列车(技规)用于列车交会时自动排路
						// bit 2:    1-无计划不得停车
						// bit 3:    1-不得使用其他股道
						// bit 4:    1-不得侧线通过
						// bit 5:    1-不得自动办理接车进路
						// bit 6:    1-不得自动办理发车进路
						// bit 7:    0-按会让条件办理出发进路,  1-按计划时间办理出发进路
						// bit 8:    1-禁止停车
						// bit 9:    1-必须停车
						// bit 10:   1-压前,车头停在股道外, 车尾完全进入股道
	                    // bit 11:   1-压后,车头完全进入股道, 车尾停在股道外
						// bit 12:	 1-人工设置进路触发模式
						// bit 13:   1-OPMS计划检查条件1:禁止通过
						// bit 14:   股道和交路折返股道冲突 1-长显示股道 0:不长显示股道
						// bit 15:   修改交路股道 1-长显示股道 0:不长显示股道
	long    adjust_status;
						// bit 0:  0 : 到达时间为计划
						//         1 : 到达时间为实际
						// bit 1:  0 : 出发时间为计划
						//         1 : 出发时间为实际
						// bit 2:  0 : 正常
						//         1 : 存在错误信息
						// bit 3:  0 : 调整成功
						//         1 : 调整失败
						// bit 4:  1 : 技术停车
						// bit 5:  1 : 
						// bit 6:  0 : arrivetime is same with the nextcon's arr
						//         1 : not same(only for inter_sta)
						// bit 7:  0 : departtime is same with the nextcon's dep
						//         1 : not same(only for inter_sta)
						// bit 8 :   到达时间锁
						// bit 9 :   出发时间锁
						// bit 10:  0  实际到达时间为人工报点
						//          1  实际到达时间为自动报点
						// bit 11:  0  实际出发时间为人工报点
						//          1  实际出发时间为自动报点
						// bit 12:  
						//          
						// bit 13,14:  0  未发送到邻台（台间站）
						//                 1  邻台拒绝
						//                 2  邻台接受
						//       
						// bit 15:  0  无电力牵引
						//          1  电力牵引
						// bit 16: 1:分界口车站
	                    // bit 17: 1:台间站
						// bit 18: 1:自动设置折返车次
	                   	// bit 19: 1:3.0不卡控基本图出入口
	                   	// bit 20: 1:3.0不卡控基本图股道

	short   spec_run_time;      // 指定运行时间
								// 用 TG_NOTMEET_xxx 判断
								// BIT 定义见 NOTMEET_xxx
	long    can_not_meet;       // 禁会车种信息
						//     bit 0: 禁会动车组
						//     bit 1: 禁会直特
						//     bit 2: 禁会客车
						//     bit 3: 禁会列车
						//     bit 4: 禁会
						//     bit 5: 禁会

	long    super_interval_ltime; // 指定列车间隔
					// 3-0: 0 使用默认配制, else: 左侧间隔 (min)
					// 7-4: 0 使用默认配制, else: 右侧间隔 (min)

	short         special_position;
	WORD         special_time_1;
	WORD         special_time_2;
	TRAIN_INDEX   special_train_index;

	long    data_source;
						// bit 03-00:  define side infomation's source(0-7)
						//   0 - basic schedule defined       1 - addition schedule defined
						//   2 - static side                  3 - dispatcher changed
						//   4 - auto adjusted                5 - sta watcher changed plan
						//   6 - sta watcher reported actual  7 - lirc changed plan
						//   8 - auto report                  9 - pathway defined

						// bit 07-04:  define arrive information's source(0-6)
						// bit 11-08:  define depart information's source(0-6)
						//   0 - default(create, dispatcher change)    1 - auto adjust
						//   2 - lirc changed plan                     3 - dispatcher turn actual
						//   4 - sta watcher changed plan              5 - sta watcher report 
						//   6 - auto report                           7 - auto report match
						//   8 - simple adjust(calculate by runtime)

						// bit 14-12:  define plan arrive information's source(0-2)
						// bit 17-15:  define plan depart information's source(0-2)
						//   0 - basic schedule               1 - addition schedule
						//   2 - auto create (the time while non-basic schedule is created )
						//   3 - manual set 

	long    flag;
						// bit  2-0:
						//            001 - 交出
						//            010 - 终到
						//            011 - 停运
						//            100 - 保留
						//            101 - 续开
						//            110 - 列车合并运行
						//            111 - 列车区间内分部运行
						// bit  4-3:    01 - 接入
						//              10 - 始发
						// bit  5:  1 - 反向行车
						// bit  7:  1 - 机外停车
						// bit  8:  1 - 区间停车
						// bit  9:  1 - 补机途中折返
						// bit 10:  1 - 侧线通过
						// bit 11:  1 - 救援其他列车， 其他列车为special_train_index
						// bit 12:  1 - 被其他列车救援， 其他列车为special_train_index
						// bit 13:  1 - 上行区间停车, 0 下行区间停车
						// bit 14:  1 - 组合其他列车， 其他列车为special_train_index
						// bit 15:  1 - 被其他列车组合， 其他列车为special_train_index
						// bit 16:  1 - 区间终到
						// bit 17:  1 - 区间始发
};

// 列车折返位置 站内 站后
//bit1:0 通用版本, bit2无意义
//bit1:1 珠三角城际折返,bit2有意义,并且bit为1是站后折返 bit2为0是原地折返
#define IsNewReturn(side_utility)		   (((side_utility)&0x2) == 0x2)	//bit1:1城际折返 0:无意义
#define IsStationOutReturn(side_utility)    (((side_utility)&0x6) == 0x6)	//bit2
#define SetStationOutReturn(side_utility)   ((side_utility) &= ~0x6, (side_utility) |= 0x6)	//bit2
#define SetStationReturn(side_utility)   ((side_utility) &= ~0x6, (side_utility) |= 0x2)	//bit2
#define NotStationReturn(side_utility)   ((side_utility) &= ~0x6)	//bit2

/* 列车基本信息 */
struct TG_TrainInfo 
{
	TG_TrainInfo()
	{
		memset(this, 0, sizeof(TG_TrainInfo));
	}
/*   基础信息  */
	char          train_name[MAX_TRAIN_NAME_LEN];   //  车次号组合

	TRAIN_KEY     train_key;

	TRAIN_INDEX   gpc_train_index;      // GPC产生的索引
	TRAIN_INDEX   basic_train_index;    // 根据基本图生成时, 基本图的索引
	TRAIN_INDEX   shift_train_index;    // 根据日班计划生成时,日班计划的索引
	char          shift_string[32]; // OPMS4.0标示
	TIME          update_time;      // 删除时间
	short         delete_flag;      //  -1, 无
	BYTE          type;        // 列车详细属性 (bit 7: 0 - passenger 1 -freight)
	BYTE          run_type;    // 列车属性，用于计算区间运行时间和自动排路提前触发时机

	TIME          start_plan_time;     // 始发计划时间
	TIME          start_actual_time;   // 始发实际时间
	TIME          sborder_plan_time;   // 分界口接入计划时间
	TIME          sborder_actual_time; // 分界口接入实际时间
	TIME          eborder_plan_time;   // 分界口交出计划时间
	TIME          eborder_actual_time; // 分界口交出实际时间
	STATION_NO    sborder_sta;         // 分界口接入站号
	STATION_NO    eborder_sta;         // 分界口交出站号
	char          start_sta_name[MAX_STA_NAME_LEN];  // 起始车站名
	char          end_sta_name[MAX_STA_NAME_LEN];    // 终到车站名
	CONID         from_conid;          // 后方调度台号
	CONID         to_conid;            // 前方调度台号

	BYTE         priority;             // 列车等级
	long         train_length;         // 车长
	long         train_weight;         // 车重
	long         status;               
						// bit 0-2:   0 - 不存在速报,       1 - 速报来于车站上报 
						//            2 - 速报来于临台传递, 3 - 速报来于调度员手工
						//            4 - 速报来于无线车次号
	                    
						// bit 3: 1 - 保密列车
						// bit 4: 1 - TDMS列车
	                    // bit 5: 1 - 强制正点
						// bit 16-31: 最高两个字节存放 辆数  乔永涛 20100715 应兰州局需求
/*   特殊列车信息  */
	BYTE    space_rank;       // 0: no out of space, else: rank of space
	BYTE    length_rank;      // 0: no out of length, else: rank of length
	BYTE    super_rank;       // 0: not super_train, else: rank of super_train
	BYTE    military_rank;    // 0: not military_train, else: rank of military_train
	

/*   列车运行信息  */
	long     engine_utility;  // 0: normal
								// 1: limited_time
								// 2: no locomotive
	long     side_utility;    // bit0:   1 :   need high platform
							  // bit 1-2
							  // bit 1: 1:bit2有效 0:bit2无效
							  // bit 2: 1:站后折返 0:原地折返	
							  // bit 4-5
							  // bit 4: 1:bit5有效 0:bit5无效
							  // bit 5: 1:站后折返 0:原地折返	

/*   自动调整信息  */
	short   adjust_begin_sta;   // bit 7: 1 set by default,  0 set manual
	short   adjust_end_sta;     // keep it for future use

	long    adjust_flag;        // bit 0: 0 - join adjust           1 - not
								// bit 1: 0 - exist in shift schedule
								// bit 2: 0 - generate time_link    1 - not
								// bit 3: 0 - not in save before adjust   1 - do no use
								// bit 7-6:  0 - sborder_actual time is not set
								//           1 - sborder_actual time is set by manual
								//           2 - sborder_actual time is set by system
								// bit 10-8:  机车号显示位置  0-不显示   1-下 2-中  4-上
								// bit 11:   助调能够操纵的列车标志---区间作业车
								// bit 12:   区间分部运行 1:是  0：否
								// bit 13-14: 区间始发列车 停上区间 01 下区间 10
								// bit 15: 补机途中折返 1：是 0:否
								// bit 16-31: 区间始发列车 区间运行时分

	long    train_source;
};


struct TG_ScheduleBlock 
{
  TRAIN_INDEX   gpc_index;
  TRAIN_INDEX   basic_index;
  TRAIN_INDEX   shift_index;

  char          shift_string[32]; // OPMS4.0标示
  TIME          update_time;      // 删除时间
  short         delete_flag;      //  -1, 无效， >=0 延迟删除记录索引

  TIME     start_time;
  TIME     end_time;

  BYTE       type;           //  ABS SLOW,      ABS_INHIBIT,  ELECTRO_FIX,  SIDE_SLOW,    SIDE_INHIBIT,  GRAPH_NOTE, ABS_ELEC_FIX, REGIST_BLOCK_TYPE
  BYTE       flag;           //  limit_speed    flag          0             limit_speed   type           0,                        block_type(0-half,1-auto,2-tel)
  USHORT     line;            // used by gpc
  STATION_NO station;        //  0              0             0             station       station        top_position
  USHORT     index;          // used by gpc
  USHORT     position;       //  limit_length   position      position      limit_length  position       bottom_position
                             // bit 0:  1 -- inhibit first sta, 0 -- not;  bit 1: 1 -- inhibit second sta, 0 -- not ;  when type=SECTION_ELEC_FIX
  USHORT     direction;  // 仅station & index==0xFF 时，下述三个字段才有效
  USHORT     station1;  
  USHORT     station2;
  USHORT     ele_count;
  USHORT     ele_number[MAX_SCHD_BLOCK_ELE_COUNT];
  long      adjust_status;   // bit 1-0:  0 计划、1 执行、2 实际
  char      text[MAX_SCHD_BLOCK_TEXT_LEN];
  USHORT    s_meter;  //first sta num ,when type = SECTION_ELEC_FIX
  USHORT    e_meter;  //second sta num ,when type = SECTION_ELEC_FIX
  USHORT    e_kilo;
  USHORT    slow_time;
} ;

struct TG_TrainYingyeInfo
{
	STATION_NO	station_no;				// 站号
	I_TIME		arrive_time;			// 到达时间
	I_TIME		depart_time;			// 出发时间
	char		station_name[MAX_STA_NAME_LEN];			// 站名
	TG_TrainYingyeInfo()
	{
		station_no=0;
		arrive_time=0;
		depart_time=0;
		memset(station_name, 0, sizeof(station_name));
	}
};

struct TG_TrainCrewInfo
{
	short	crew_type;			//0:未知 1：司机	2：机械师	3：乘警	4：保洁	5：餐饮
	char	crew_info[64];		//姓名及电话
	char	crew_bureau[32];	//归属路局
	TG_TrainCrewInfo()
	{
		crew_type=0;
		memset(crew_info, 0, sizeof(crew_info));
		memset(crew_bureau, 0, sizeof(crew_bureau));
	}
};

struct TG_TrainTicketInfo
{
	int			on_station_index;		// 上车站序
	STATION_NO	on_station;			// 上车站号
	int			down_station_index;		// 下车站序
	STATION_NO  down_station;		// 下车站号
	int			sale;					// 已售张数
	char		seat[32];				// 席别
	char	on_station_name[MAX_STA_NAME_LEN];			// 上车站名
	char	down_station_name[MAX_STA_NAME_LEN];			// 下车站名
	TG_TrainTicketInfo()
	{
		on_station_index=0;
		on_station=0;
		down_station_index=0;
		down_station=0;
		sale=0;
		memset(seat, 0, sizeof(seat));
		memset(on_station_name, 0, sizeof(on_station_name));
		memset(down_station_name, 0, sizeof(down_station_name));
	}
};

struct TG_EngineCrewInfo
{
	STATION_NO	station_no;				// 站号
	I_TIME		overwork_time;			// 超劳时间
	char	station_name[MAX_STA_NAME_LEN];			// 站名
	TG_EngineCrewInfo()
	{
		station_no=0;
		overwork_time=0;
		memset(station_name, 0, sizeof(station_name));
	}
};

struct TG_TrainOPMSRecordInfo
{
	STATION_NO	station_no;			// 车站站号
	IDWORD	    work_flag;			// 车站作业
	IDWORD      oper_flag;			// 操作卡控
	IDWORD      important_flag;		// 重要卡控

	char	    oper_info[256];
	char        note_text[256];
	TG_TrainOPMSRecordInfo()
	{
		station_no=0;
		work_flag=0;
		oper_flag=0;
		important_flag=0;
		memset(oper_info, 0, sizeof(oper_info));
		memset(note_text, 0, sizeof(note_text));
	}
};

/* */
#define TG_GetAnotherShiftId(shift_id)           (3 - shift_id)

#define TG_SetTccStatusNone(block)      ((block).dir = (0xCFFF&((block).dir))|0x0000)
#define TG_SetTccStatusConfirm(block)   ((block).dir = (0xCFFF&((block).dir))|0x1000)
#define TG_SetTccStatusExec(block)      ((block).dir = (0xCFFF&((block).dir))|0x2000)
#define TG_SetTccStatusCancel(block)    ((block).dir = (0xCFFF&((block).dir))|0x3000)

#define TG_IsTccStatusNone(block)	     ((0x3000&((block).dir)) == 0x0000)
#define TG_IsTccStatusConfirm(block)	 ((0x3000&((block).dir)) == 0x1000)
#define TG_IsTccStatusExec(block)	     ((0x3000&((block).dir)) == 0x2000)
#define TG_IsTccStatusCancel(block)	 ((0x3000&((block).dir)) == 0x3000)

#define TG_IsFlagGiveout(flag)         ((flag & 0x0007) ==  0x0001)
#define TG_IsFlagTerminal(flag)        (((flag & 0x0007) ==  0x0002)  || ((flag & 0x0007) ==  0x0003))
#define TG_IsFlagEnd(flag)             ((flag & 0x0007) ==  0x0003)
#define TG_IsFlagKeep(flag)            ((flag & 0x0007) ==  0x0004)
#define TG_IsFlagRelay(flag)           ((flag & 0x0007) ==  0x0005)
#define TG_IsFlagEndInAbs(flag)        ((flag & 0x0007) ==  0x0006)
#define TG_IsFlagDivide(flag)          ((flag & 0x0007) ==  0x0007)
#define TG_IsFlagAccept(flag)          ((flag & 0x0018) ==  0x0008)
#define TG_IsFlagStart(flag)           ((flag & 0x0018) ==  0x0010)
#define TG_IsFlagReverse(flag)         (flag & 0x0020)
#define TG_IsFlagCorner(flag)          (flag & 0x0040)
#define TG_IsFlagStopSignal(flag)      ((flag & 0x0080) ==  0x0080)
#define TG_IsFlagStopAbs(flag)         ((flag & 0x0100) ==  0x0100)
#define TG_IsFlagAnLocReturn(flag)     (flag & 0x0200)
#define TG_IsFlagPassNonMainSide(flag) ((flag & 0x0400) ==  0x0000)
#define TG_IsFlagStopAbsUp(flag)       (flag & 0x2000)
#define TG_IsFlagStopAbsDown(flag)     (!(flag & 0x2000))
#define TG_IsFlagUnitOtherTrain(flag)  (flag & 0x4000)   //bit 14
#define TG_IsFalgBeUnitedByOther(flag) (flag & 0x8000)   //bit 15
#define TG_IsFlagStopMainSide(flag)    (0)
#define TG_IsFlagAbsTerminal(flag)     (flag & 0x00010000)
#define TG_IsFlagAbsStart(flag)        (flag & 0x00020000)

#define TG_SetFlagGiveout(flag)         (flag &= 0xFFFFFFF8,flag|= 0x0001)
#define TG_SetFlagTerminal(flag)        (flag &= 0xFFFFFFF8,flag|= 0x0002)
#define TG_SetFlagEnd(flag)             (flag &= 0xFFFFFFF8,flag|= 0x0003)
#define TG_SetFlagKeep(flag)            (flag &= 0xFFFFFFF8,flag|= 0x0004)
#define TG_SetFlagRelay(flag)           (flag &= 0xFFFFFFF8,flag|= 0x0005)
#define TG_SetFlagEndInAbs(flag)        (flag &= 0xFFFFFFF8,flag|= 0x0006)
#define TG_SetFlagDivide(flag)          (flag &= 0xFFFFFFF8,flag|= 0x0007)
#define TG_SetFlagAccept(flag)          (flag &= 0xFFFFFFE7,flag|= 0x0008)
#define TG_SetFlagStart(flag)           (flag &= 0xFFFFFFE7,flag|= 0x0010)
#define TG_SetFlagReverse(flag)         (flag |= 0x0020)
#define TG_SetFlagCorner(flag)          (flag |= 0x0040)
#define TG_SetFlagStopSignal(flag)      (flag |= 0x0080)
#define TG_SetFlagStopAbs(flag)         (flag |= 0x0100)
#define TG_SetFlagAnLocReturn(flag)     (flag |= 0x0200)
#define TG_SetFlagPassNonMainSide(flag) (flag |= 0x0400)
#define TG_SetFlagStopAbsUp(flag)       (flag |= 0x2000)
#define TG_SetFlagStopAbsDown(flag)     (flag &= 0xFFFFDFFF)
#define TG_SetFlagUnitOtherTrain(flag)  (flag |= 0x4000)
#define TG_SetFlagBeUnitedByOther(flag) (flag |= 0x8000)
#define TG_SetFlagStopMainSide(flag)    (flag)
#define TG_SetFlagAbsTerminal(flag)     (flag |= 0x00010000)
#define TG_SetFlagAbsStart(flag)        (flag |= 0x00020000)

#define TG_ClearFlagGiveoutTerminal(flag) (flag &= ~0x3)
#define TG_ClearFlagStartAccept(flag)     (flag &= ~0x18)

#define TG_ClearFlagReverse(flag)         (flag &= ~0x0020)
#define TG_ClearFlagCorner(flag)          (flag &= ~0x0040)
#define TG_ClearFlagStopSignal(flag)      (flag &= ~0x0080)
#define TG_ClearFlagStopAbs(flag)         (flag &= ~0x0100)
#define TG_ClearFlagAnLocReturn(flag)     (flag &= ~0x0200)
#define TG_ClearFlagPassNonMainSide(flag) (flag &= ~0x0400)
#define TG_ClearFlagStopMainSide(flag)    (flag)
#define TG_RemoveFlagStatus(flag)         (flag &= ~0x1F)

#define TG_RemoveFlagLeaveStatus(flag)    (flag &= ((TG_IsFlagEndInAbs(flag) || TG_IsFlagDivide(flag)) ? 0xFFFFFFFF:0xFFFFFFF8))
#define TG_RemoveFlagEnterStatus(flag)    (flag &= 0xFFFFFFE7)

#define TG_ClearFlagUnitOtherTrain(flag)  (flag &= 0xFFFFBFFF)
#define TG_ClearFlagBeUnitedByOther(flag) (flag &= 0xFFFF7FFF)
#define TG_ClearFlagAbsTerminal(flag)     (flag &= ~0x00010000)
#define TG_ClearFlagAbsStart(flag)        (flag &= ~0x00020000)

#define TG_IsTrainPass(rec)                  ((rec).arrive == (rec).depart && (rec).arrive_side == (rec).depart_side && (!TG_IsFlagStart((rec).flag) && !TG_IsFlagTerminal((rec).flag)))

#define TG_IsDefinePass(rec)                 ((rec).stop_condition & 0x02)
#define TG_IsForcePass(rec)                  ((rec).stop_condition & 0x04)
#define TG_IsForceSide(rec)                  ((rec).stop_condition & 0x08)
#define TG_IsForcePassByMainSide(rec)        ((rec).stop_condition & 0x10)

#define TG_SetDefinePass(rec)                ((rec).stop_condition |= 0x02)
#define TG_SetForcePass(rec)                 ((rec).stop_condition |= 0x04)
#define TG_SetForceSide(rec)                 ((rec).stop_condition |= 0x08)
#define TG_SetForcePassByMainSide(rec)       ((rec).stop_condition |= 0x10)

#define TG_ReSetDefinePass(rec)              ((rec).stop_condition &= ~0x02)
#define TG_ReSetForcePass(rec)               ((rec).stop_condition &= ~0x04)
#define TG_ReSetForceSide(rec)               ((rec).stop_condition &= ~0x08)
#define TG_ReSetForcePassByMainSide(rec)     ((rec).stop_condition &= ~0x10)

#define TG_IsAutoMakeArriveRoute(rec)        (!((rec).stop_condition & 0x20))
#define TG_IsAutoMakeDepartRoute(rec)        (!((rec).stop_condition & 0x40))
#define TG_SetAutoMakeArriveRoute(rec)       ((rec).stop_condition &= ~0x20)
#define TG_SetAutoMakeDepartRoute(rec)       ((rec).stop_condition &= ~0x40)
#define TG_SetNotAutoMakeArriveRoute(rec)    ((rec).stop_condition |= 0x20)
#define TG_SetNotAutoMakeDepartRoute(rec)    ((rec).stop_condition |= 0x40)

#define TG_IsMakeDepartRouteByMeet(rec)      (((rec).stop_condition & 0x80) == 0x00)
#define TG_SetMakeDepartRouteByMeet(rec)     ((rec).stop_condition &= ~0x80)
#define TG_IsMakeDepartRouteByPlan(rec)      (((rec).stop_condition & 0x80) == 0x80)
#define TG_SetMakeDepartRouteByPlan(rec)     ((rec).stop_condition |= 0x80) 

// TDMS要求:禁止停车
#define TG_IsMustNotStop(rec)     ((rec).stop_condition & 0x100) 
#define TG_SetFlagMustNotStop(rec)       ((rec).stop_condition |= 0x100) 
#define TG_ClearFlagMustNotStop(rec)     ((rec).stop_condition &= ~0x100) 

// TDMS要求:必须停车
#define TG_IsMustStop(rec)     ((rec).stop_condition & 0x200) 
#define TG_SetFlagMustStop(rec)       ((rec).stop_condition |= 0x200) 
#define TG_ClearFlagMustStop(rec)     ((rec).stop_condition &= ~0x200) 

// 压前
#define TG_IsBeforePressure(rec)         (((rec).stop_condition & 0x400) == 0x400)

// 压前
#define TG_IsAfterPressure(rec)         (((rec).stop_condition & 0x800) == 0x800)

// TDMS要求:停运
#define TG_IsManualSetTrigeRoute(rec)   (((rec).stop_condition & 0x1000) == 0x1000)
#define TG_ManualSetTrigeRoute(rec)   ((rec).stop_condition |= 0x1000)

// TDMS要求:禁止通过
#define TG_IsMustNoPass(rec)     ((rec).stop_condition & 0x2000) 
#define TG_SetFlagMustNoPass(rec)       ((rec).stop_condition |= 0x2000) 
#define TG_ClearFlagMustNoPass(rec)     ((rec).stop_condition &= ~0x2000) 

// 股道和交路折返股道冲突,是否长显示股道,重点提示用
#define TG_IsDispalySide(rec)     ((rec).stop_condition & 0x4000) 
#define TG_SetFlagDispalySide(rec)       ((rec).stop_condition |= 0x4000) 
#define TG_ClearFlagDispalySide(rec)     ((rec).stop_condition &= ~0x4000) 

//  修改交路股道, 是否长显示股道,重点提示用
#define TG_IsDispalySide2(rec)     ((rec).stop_condition & 0x8000) 
#define TG_SetFlagDispalySide2(rec)       ((rec).stop_condition |= 0x8000) 
#define TG_ClearFlagDispalySide2(rec)     ((rec).stop_condition &= ~0x8000) 

#define TG_IsArrivePlan(rec)           (!((rec).adjust_status & 0x0001))
#define TG_IsArriveActual(rec)         ((rec).adjust_status & 0x0001)
#define TG_IsDepartPlan(rec)           (!((rec).adjust_status & 0X0002))
#define TG_IsDepartActual(rec)         ((rec).adjust_status & 0x0002)
#define TG_IsStatusRight(rec)          (!((rec).adjust_status & 0x0004))
#define TG_IsStatusError(rec)          ((rec).adjust_status & 0x0004)
#define TG_IsStatusCanAdjust(rec)      (!((rec).adjust_status & 0x0008))
#define TG_IsStatusCannotAdjust(rec)   ((rec).adjust_status & 0x0008)
#define TG_SetArrivePlan(rec)          ((rec).adjust_status &= 0xFFFE)
#define TG_SetArriveActual(rec)        ((rec).adjust_status |= 0x0001)
#define TG_SetDepartPlan(rec)          ((rec).adjust_status &= 0xFFFD)
#define TG_SetDepartActual(rec)        ((rec).adjust_status |= 0x0002)
#define TG_SetStatusRight(rec)         ((rec).adjust_status &= ~0x0004)
#define TG_SetStatusError(rec)         ((rec).adjust_status |= 0x0004)
#define TG_SetStatusCanAdjust(rec)     ((rec).adjust_status &= ~0x0008)
#define TG_SetStatusCannotAdjust(rec)  ((rec).adjust_status |= 0x0008)

/// 
#define TG_IsActualArrivalTime(adjust_status)  (adjust_status & 0x0001)
#define TG_IsActualDepartTime(adjust_status)  (adjust_status & 0x0002)
#define TG_SetActualArrivalTime(adjust_status)  adjust_status|=0x0001
#define TG_SetActualDepartTime(adjust_status)   adjust_status|=0x0002

#define TG_IsTechStopTrain(rec)        (((rec).adjust_status & 0x0010))
#define TG_SetTechStopTrain(rec)       ((rec).adjust_status |= 0x0010)
#define TG_ClearTechStopTrain(rec)     ((rec).adjust_status &= ~0x0010)

#define TG_IsArriveLock(rec)           (((rec).adjust_status & 0x0100) == 0x0100)
#define TG_IsDepartLock(rec)           (((rec).adjust_status & 0x0200) == 0x0200)
#define TG_SetArriveLock(rec)          ((rec).adjust_status |= 0x0100)
#define TG_SetDepartLock(rec)          ((rec).adjust_status |= 0x0200)
#define TG_ReSetArriveLock(rec)        ((rec).adjust_status &= ~0x0100)
#define TG_ReSetDepartLock(rec)        ((rec).adjust_status &= ~0x0200)

#define TG_IsInterArriveSame (rec)     (!((rec).adjust_status & 0x0040))
#define TG_IsInterArriveDiffer (rec)   ((rec).adjust_status & 0x0040)
#define TG_SetInterArriveSame (rec)    ((rec).adjust_status &= ~0x0040))
#define TG_SetInterArriveDiffer (rec)  ((rec).adjust_status |= 0x0040))
#define TG_IsInterDepartSame (rec)     (!((rec).adjust_status & 0x0080))
#define TG_IsInterDepartDiffer (rec)   ((rec).adjust_status & 0x0080)
#define TG_SetInterDepartSame (rec)    ((rec).adjust_status &= ~0x0080))
#define TG_SetInterDepartDiffer (rec)  ((rec).adjust_status |= 0x0080)

#define TG_IsArriveActualByManual(rec) (!((rec).adjust_status & 0x0400) && ((rec).adjust_status & 0x0001))
#define TG_IsArriveActualByAuto(rec)   (((rec).adjust_status & 0x0400) && ((rec).adjust_status & 0x0001))
#define TG_IsDepartActualByManual(rec) (!((rec).adjust_status & 0x0800) && ((rec).adjust_status & 0x0002))
#define TG_IsDepartActualByAuto(rec)   (((rec).adjust_status & 0x0800) && ((rec).adjust_status & 0x0002))
#define TG_SetArriveActualByManual(rec) ((rec).adjust_status|=0x0001,(rec).adjust_status&= ~0x0400)
#define TG_SetArriveActualByAuto(rec)   ((rec).adjust_status|=0x0001,(rec).adjust_status|=0x0400)
#define TG_SetDepartActualByManual(rec) ((rec).adjust_status|=0x0002,(rec).adjust_status&= ~0x0800)
#define TG_SetDepartActualByAuto(rec)   ((rec).adjust_status|=0x0002,(rec).adjust_status|=0x0800)

#define TG_SetAdjustBeginSta(rec)          ((rec).adjust_status |= 0x1000)              //bit 12
#define TG_SetAdjustNormalSta(rec)         ((rec).adjust_status &= ~0x1000)
#define TG_IsAdjustBeginSta(rec)           ((rec).adjust_status &  0x1000)
#define TG_SetNotSendPlanToNextCon(rec)    ((rec).adjust_status &= 0x9FFF)          //bit 13,14: 00
#define TG_SetBeRejectedByNextCon(rec)     ((rec).adjust_status &= 0x9FFF, (rec).adjust_status |= 0x2000)   //bit 13,14: 01
#define TG_SetBeReceivedByNextCon(rec)     ((rec).adjust_status &= 0x9FFF, (rec).adjust_status |= 0x4000)   //bit 13,14: 10
#define TG_IsRejectedByNextCon(rec)        (((rec).adjust_status & 0x6000) == 0x2000)
#define TG_IsReceivedByNextCon(rec)        (((rec).adjust_status & 0x6000) == 0x4000)
#define TG_IsSendPlanToNextCon(rec)        (((rec).adjust_status & 0x6000) == 0x6000)

/// 电力牵引
#define TG_SetElecLocoFlag(rec)          ((rec).adjust_status |= 0x8000)              //bit 15
#define TG_SetNoElecLocoFlag(rec)        ((rec).adjust_status &= ~0x8000)              //bit 15
#define TG_IsElecLocoFlag(rec)           ((rec).adjust_status & 0x8000)              //bit 15

// 分界口车站
#define TG_SetFJKStationFlag(rec)        ((rec).adjust_status |= 0x10000)              //bit 16
#define TG_SetNoFJKStationFlag(rec)        ((rec).adjust_status &= ~0x10000)              //bit 16
#define TG_IsFJKStationFlag(rec)           ((rec).adjust_status & 0x10000)              //bit 16

// 台间车站
#define TG_SetNeighbourStationFlag(rec)        ((rec).adjust_status |= 0x20000)              //bit 17
#define TG_SetNoNeighbourStationFlag(rec)        ((rec).adjust_status &= ~0x20000)        //bit 17
#define TG_IsNeighbourStationFlag(rec)           ((rec).adjust_status & 0x20000)              //bit 17

// 自动设置折返车次
#define IsAutoSetReturnTrainId(rec)     (((rec).adjust_status&0x00040000) == 0x00040000)	//bit 18
#define SetAutoReturnTrainId(rec)       ((rec).adjust_status |= 0x00040000)					//bit 18
#define ResetAutoReturnTrainId(rec)     ((rec).adjust_status &= ~0x00040000)				//bit 18

// 3.0卡控基本图出入口
#define TG_IsNotCheckTrainExitEntry(rec)   (((rec).adjust_status&0x80000) == 0x80000)	//bit 19
#define TG_NotCheckTrainExitEntry(rec)	   ((rec).adjust_status |= 0x80000)					//bit 19
#define TG_CheckTrainExitEntry(rec)		   ((rec).adjust_status &= ~0x80000)				//bit 19

// 3.0不卡控基本图股道
#define TG_IsNotCheckTrainSide(rec)          (((rec).adjust_status & 0x100000) == 0x100000)  // bit 20
#define TG_NotCheckTrainSide(rec)            ((rec).adjust_status |= 0x100000)
#define TG_CheckTrainSide(rec)			     ((rec).adjust_status &= ~0x100000)
//TDMS要求 解析rank
						// bit 0:  超限
						// bit 1:  重点警卫
						// bit 2:  毒品
						// bit 3:  军运
#define TG_IsSpaceRank(rec)               ((rec).rank & 0x01) //超限标志---超限等级保存在bit16-bit21
#define TG_IsSpecialGaurdRank(rec)        ((rec).rank & 0x02)
#define TG_IsGoustRank(rec)               ((rec).rank & 0x04)
#define TG_IsMilitaryRank(rec)            ((rec).rank & 0x08)

#define TG_IsSpace(rec)					  (((rec).rank & 0x3F0000)>>16)
#define TG_GetArriveSpace(rec)            (((rec).rank & 0x070000)>>16) // 接车超限等级值
#define TG_GetDepartSpace(rec)            (((rec).rank & 0x380000)>>19) // 发车超限等级值

#define TG_SetArriveSpace(rec,  space)    ((rec).rank &= ~0x070000,  (rec).rank |= (space)<<16)
#define TG_SetDepartSpace(rec, space)     ((rec).rank &= ~0x380000,  (rec).rank |= (space)<<19)

#define TG_SetSpaceRank(rec)			   ((rec).rank |= 0x01)
#define TG_SetSpecialGaurdRank(rec)        ((rec).rank |= 0x02)
#define TG_SetGoustRank(rec)               ((rec).rank |= 0x04)
#define TG_SetMilitaryRank(rec)            ((rec).rank |=0x08)

#define TG_SetNoSpaceRank(rec)             ((rec).rank &=~0x01)
#define TG_SetNoMilitaryRank(rec)          ((rec).rank &=~0x08)

#define TG_IsSpecifyAbsRunTime(rec)              ((rec)->spec_run_time)
#define TG_SetSpecifyAbsRunTime(rec, ltime)      ((rec)->spec_run_time = ltime)

#define TG_SetRunFastSlowTime(rfstime, t)  (rfstime = (((short)(t/60)>127 ||(short)(t/60)<-127)? 0 :(((short)(t/60)<=0)?(short)(t/60)*(-1):(t/60+128))))
#define TG_InitRunFastSlowTIme(rfstime)    (rfstime = 0)

#define TG_IsPermitEarlyDepart(rec)           ((short)(rec).early_depart_time != (short)0x0000)
#define TG_SetCanNotEarlyDepart(rec)          ((rec).early_depart_time = 0x0000)
#define TG_IsNoLimitEarlyDepart(rec)          ((rec).early_depart_time == (short)0x8000)
#define TG_SetNoLimitEarlyDepart(rec)         ((rec).early_depart_time =  (short)0x8000)
#define TG_SetPermitEarlyDepart(rec, ltime)   ((rec).early_depart_time = (ltime - (rec).plan_depart)/60)
#define TG_GetEarlyDepartLimit(rec)           (!IsPermitEarlyDepart(rec)?(rec).plan_depart:IsNoLimitEarlyDepart(rec)?NULL_TIME:(rec).early_depart_time*60+(rec).plan_depart)

#define TG_SetNoConflictReason(rec)          ((rec).stop_reason &= 0x00)
#define TG_SetEntryConflictPre(rec)          ((rec).stop_reason &= 0x3F,(rec).stop_reason |= 0x40)           
#define TG_SetEntryConflictNext(rec)         ((rec).stop_reason &= 0x3F,(rec).stop_reason |= 0x80)           
#define TG_SetEntryConflictDevice(rec)       ((rec).stop_reason |= 0xC0)
#define TG_SetEntryNoConflict(rec)           ((rec).stop_reason &= 0x3F)           
#define TG_SetSideConflictPre(rec)           ((rec).stop_reason &= 0xCF,(rec).stop_reason |= 0x10)           
#define TG_SetSideConflictNext(rec)          ((rec).stop_reason &= 0xCF,(rec).stop_reason |= 0x20)           
#define TG_SetSideConflictDevice(rec)        ((rec).stop_reason |= 0x30)
#define TG_SetSideNoConflict(rec)            ((rec).stop_reason &= 0xCF)           
#define TG_SetExitConflictPre(rec)           ((rec).stop_reason &= 0xF3,(rec).stop_reason |= 0x04)           
#define TG_SetExitConflictNext(rec)          ((rec).stop_reason &= 0xF3,(rec).stop_reason |= 0x08)           
#define TG_SetExitConflictDevice(rec)        ((rec).stop_reason |= 0x0C)
#define TG_SetExitNoConflict(rec)            ((rec).stop_reason &= 0xF3)           
#define TG_SetAbsConflictPre(rec)            ((rec).stop_reason &= 0xFC,(rec).stop_reason |= 0x01)           
#define TG_SetAbsConflictNext(rec)           ((rec).stop_reason &= 0xFC,(rec).stop_reason |= 0x02)           
#define TG_SetAbsConflictDevice(rec)         ((rec).stop_reason |= 0x03)
#define TG_SetAbsNoConflict(rec)             ((rec).stop_reason &= 0xFC)           

#define TG_SetNoErrorReason(rec)             ((rec).stop_reason = 0x00)
#define TG_SetErrorStationNumber(rec)        ((rec).stop_reason = 0x01)
#define TG_SetErrorEntryNumber(rec)          ((rec).stop_reason = 0x02)
#define TG_SetErrorExitNumber(rec)           ((rec).stop_reason = 0x03)
#define TG_SetErrorSideNumber(rec)           ((rec).stop_reason = 0x04)
#define TG_SetErrorArriveDepartTime(rec)     ((rec).stop_reason = 0x05)
#define TG_SetErrorRunFlag(rec)              ((rec).stop_reason = 0x06)
#define TG_SetArriveTimeTooSmall(rec)        ((rec).stop_reason = 0x07)
#define TG_SetAbsRunTimeTooSmall(rec)        ((rec).stop_reason = 0x08)
#define TG_SetEntrySideNotConnect(rec)       ((rec).stop_reason = 0x09)
#define TG_SetSideExitNotConnect(rec)        ((rec).stop_reason = 0x0A)
#define TG_SetPrevAbsEntryNotConnect(rec)    ((rec).stop_reason = 0x0B)
#define TG_SetExitNextAbsNotConnect(rec)     ((rec).stop_reason = 0x0C)
#define TG_SetWrongSideArriveDirection(rec)  ((rec).stop_reason = 0x0D)
#define TG_SetWrongSideDepartDirection(rec)  ((rec).stop_reason = 0x0E)
#define TG_SetWrongSideArriveTrainType(rec)  ((rec).stop_reason = 0x0F)
#define TG_SetWrongSideDepartTrainType(rec)  ((rec).stop_reason = 0x10)
#define TG_SetWrongSideLength(rec)           ((rec).stop_reason = 0x11)
#define TG_SetWrongSideSpace(rec)            ((rec).stop_reason = 0x12)
#define TG_SetWrongSideWithouElec(rec)       ((rec).stop_reason = 0x13)
#define TG_SetWrongSideWithoutPlatform(rec)  ((rec).stop_reason = 0x14)
#define TG_SetWrongSideStartDifficult(rec)   ((rec).stop_reason = 0x15)
#define TG_SetStopAbsEntryError(rec)         ((rec).stop_reason = 0x16)
#define TG_SetMustStopTrain(rec)             ((rec).stop_reason = 0x17)
#define TG_SetCannotDepartEarly(rec)         ((rec).stop_reason = 0x18)
#define TG_SetSideUtilityConflict(rec)       ((rec).stop_reason = 0x19)
#define TG_SetAbsRunTimeConflict(rec)        ((rec).stop_reason = 0x1A)

#define TG_IsArriveRouteClear(rec)      (((rec).route_flag & 0x03) == 0x00)
#define TG_IsArriveRouteDoing(rec)      (((rec).route_flag & 0x03) == 0x01)
#define TG_IsArriveRouteOk(rec)         (((rec).route_flag & 0x03) == 0x02)
#define TG_IsArriveRouteOccupy(rec)     (((rec).route_flag & 0x03) == 0x03)
#define TG_IsDepartRouteClear(rec)      (((rec).route_flag & 0x0C) == 0x00)
#define TG_IsDepartRouteDoing(rec)      (((rec).route_flag & 0x0C) == 0x04)
#define TG_IsDepartRouteOk(rec)         (((rec).route_flag & 0x0C) == 0x08)
#define TG_IsDepartRouteOccupy(rec)     (((rec).route_flag & 0x0C) == 0x0C)
#define TG_IsRouteClear(rec)            (IsArriveRouteClear(rec) && IsDepartRouteClear(rec))
#define TG_IsTrainIntoSide(rec)         (((rec).route_flag & 0x10) == 0x10)

#define TG_SetRecordNormal(rec)             ((rec).db_flag &= 0xFC, (rec).db_flag |= 0x00)
#define TG_SetRecordChange(rec)             ((rec).db_flag &= 0xFC, (rec).db_flag |= 0x01)
#define TG_SetRecordInsert(rec)             ((rec).db_flag &= 0xFC, (rec).db_flag |= 0x02)
#define TG_SetRecordSendBack(rec)           ((rec).db_flag &= 0xFC, (rec).db_flag |= 0x03)
#define TG_IsRecordNormal(rec)              (!((rec).db_flag & 0x03))
#define TG_IsRecordChange(rec)              (((rec).db_flag & 0x03) == 1)
#define TG_IsRecordInsert(rec)              (((rec).db_flag & 0x03) == 2)
#define TG_IsRecordSendBack(rec)            (((rec).db_flag & 0x03) == 3)

#define TG_SetRecordSaveIntoHo(rec)         ((rec).db_flag |= 0x04)
#define TG_SetRecordNotSaveIntoHo(rec)      ((rec).db_flag &= ~0x04)
#define TG_IsRecordSaveIntoHo(rec)          ((rec).db_flag & 0x04)

#define TG_SetSideFromBasicSchedule(rec)              ((rec).data_source &= 0xFFFFFFF0,(rec).data_source |= 0x00)
#define TG_SetSideFromAdditionSchedule(rec)           ((rec).data_source &= 0xFFFFFFF0,(rec).data_source |= 0x01)
#define TG_SetSideFromStaticDefine(rec)               ((rec).data_source &= 0xFFFFFFF0,(rec).data_source |= 0x02)
#define TG_SetSideFromDispatcher(rec)                 ((rec).data_source &= 0xFFFFFFF0,(rec).data_source |= 0x03)
#define TG_SetSideFromAutoAdjust(rec)                 ((rec).data_source &= 0xFFFFFFF0,(rec).data_source |= 0x04)
#define TG_SetSideFromStaWatcherChangePlan(rec)       ((rec).data_source &= 0xFFFFFFF0,(rec).data_source |= 0x05)
#define TG_SetSideFromStaWatcherReportActual(rec)     ((rec).data_source &= 0xFFFFFFF0,(rec).data_source |= 0x06)
#define TG_SetSideFromLircChangePlan(rec)             ((rec).data_source &= 0xFFFFFFF0,(rec).data_source |= 0x07)
#define TG_SetSideFromLircReportActual(rec)           ((rec).data_source &= 0xFFFFFFF0,(rec).data_source |= 0x08)
#define TG_SetSideFromPathWay(rec)                    ((rec).data_source &= 0xFFFFFFF0,(rec).data_source |= 0x09)

#define TG_IsSideFromBasicSchedule(rec)               (((rec).data_source & 0x0F) == 0)
#define TG_IsSideFromAdditionSchedule(rec)            (((rec).data_source & 0x0F) == 1)
#define TG_IsSideFromStaticDefine(rec)                (((rec).data_source & 0x0F) == 2)
#define TG_IsSideFromDispatcher(rec)                  (((rec).data_source & 0x0F) == 3)
#define TG_IsSideFromAutoAdjust(rec)                  (((rec).data_source & 0x0F) == 4)
#define TG_IsSideFromStaWatcherChangePlan(rec)        (((rec).data_source & 0x0F) == 5)
#define TG_IsSideFromStaWatcherReportActual(rec)      (((rec).data_source & 0x0F) == 6)
#define TG_IsSideFromLircChangePlan(rec)              (((rec).data_source & 0x0F) == 7)
#define TG_IsSideFromLircReportActual(rec)            (((rec).data_source & 0x0F) == 8)
#define TG_IsSideFromPathWay(rec)                     (((rec).data_source & 0x0F) == 9)

#define TG_SetRecTimeFromDefault(rec, ad)              ((rec).data_source &= (TRAIN_ARRIVE == ad ? 0xFFFFFF0F:0xFFFFF0FF), (rec).data_source |= (TRAIN_ARRIVE == ad ? 0x0000:0x0000))
#define TG_SetRecTimeFromAutoAdjust(rec, ad)           ((rec).data_source &= (TRAIN_ARRIVE == ad ? 0xFFFFFF0F:0xFFFFF0FF), (rec).data_source |= (TRAIN_ARRIVE == ad ? 0x0010:0x0100))
#define TG_SetRecTimeFromLircChangePlan(rec, ad)       ((rec).data_source &= (TRAIN_ARRIVE == ad ? 0xFFFFFF0F:0xFFFFF0FF), (rec).data_source |= (TRAIN_ARRIVE == ad ? 0x0020:0x0200))
#define TG_SetRecTimeFromDispatcherTurnActual(rec, ad) ((rec).data_source &= (TRAIN_ARRIVE == ad ? 0xFFFFFF0F:0xFFFFF0FF), (rec).data_source |= (TRAIN_ARRIVE == ad ? 0x0030:0x0300))
#define TG_SetRecTimeFromStaWatcherChangePlan(rec, ad) ((rec).data_source &= (TRAIN_ARRIVE == ad ? 0xFFFFFF0F:0xFFFFF0FF), (rec).data_source |= (TRAIN_ARRIVE == ad ? 0x0040:0x0400))
#define TG_SetRecTimeFromStaWatcherReport(rec, ad)     ((rec).data_source &= (TRAIN_ARRIVE == ad ? 0xFFFFFF0F:0xFFFFF0FF), (rec).data_source |= (TRAIN_ARRIVE == ad ? 0x0050:0x0500))
#define TG_SetRecTimeFromLircReportActual(rec, ad)     ((rec).data_source &= (TRAIN_ARRIVE == ad ? 0xFFFFFF0F:0xFFFFF0FF), (rec).data_source |= (TRAIN_ARRIVE == ad ? 0x0060:0x0600))
#define TG_SetRecTimeFromLircReportMatch(rec, ad)      ((rec).data_source &= (TRAIN_ARRIVE == ad ? 0xFFFFFF0F:0xFFFFF0FF), (rec).data_source |= (TRAIN_ARRIVE == ad ? 0x0070:0x0700))
#define TG_SetRecTimeFromSimpleAdjust(rec, ad)         ((rec).data_source &= (TRAIN_ARRIVE == ad ? 0xFFFFFF0F:0xFFFFF0FF), (rec).data_source |= (TRAIN_ARRIVE == ad ? 0x0080:0x0800))

#define TG_IsRecTimeFromDefault(rec, ad)               (((rec).data_source & (TRAIN_ARRIVE == ad ? 0x00F0:0x0F00)) == (TRAIN_ARRIVE == ad ? 0x0000:0x0000))
#define TG_IsRecTimeFromAutoAdjust(rec, ad)            (((rec).data_source & (TRAIN_ARRIVE == ad ? 0x00F0:0x0F00)) == (TRAIN_ARRIVE == ad ? 0x0010:0x0100))
#define TG_IsRecTimeFromLircChangePlan(rec, ad)        (((rec).data_source & (TRAIN_ARRIVE == ad ? 0x00F0:0x0F00)) == (TRAIN_ARRIVE == ad ? 0x0020:0x0200))
#define TG_IsRecTimeFromDispatcherTurnActual(rec, ad)  (((rec).data_source & (TRAIN_ARRIVE == ad ? 0x00F0:0x0F00)) == (TRAIN_ARRIVE == ad ? 0x0030:0x0300))
#define TG_IsRecTimeFromStaWatcherChangePlan(rec, ad)  (((rec).data_source & (TRAIN_ARRIVE == ad ? 0x00F0:0x0F00)) == (TRAIN_ARRIVE == ad ? 0x0040:0x0400))
#define TG_IsRecTimeFromStaWatcherReport(rec, ad)      (((rec).data_source & (TRAIN_ARRIVE == ad ? 0x00F0:0x0F00)) == (TRAIN_ARRIVE == ad ? 0x0050:0x0500))
#define TG_IsRecTimeFromLircReportActual(rec, ad)      (((rec).data_source & (TRAIN_ARRIVE == ad ? 0x00F0:0x0F00)) == (TRAIN_ARRIVE == ad ? 0x0060:0x0600))
#define TG_IsRecTimeFromLircReportMatch(rec, ad)       (((rec).data_source & (TRAIN_ARRIVE == ad ? 0x00F0:0x0F00)) == (TRAIN_ARRIVE == ad ? 0x0070:0x0700))
#define TG_IsRecTimeFromSimpleAdjust(rec, ad)          (((rec).data_source & (TRAIN_ARRIVE == ad ? 0x00F0:0x0F00)) == (TRAIN_ARRIVE == ad ? 0x0080:0x0800))

#define TG_SetPlanTimeFromBasicSchedule(rec, ad)       ((rec).data_source &= (TRAIN_ARRIVE == ad ? 0xFFFF8FFF:0xFFFC7FFF), (rec).data_source |= (TRAIN_ARRIVE == ad ? 0x00000:0x00000))
#define TG_SetPlanTimeFromAdditionSchedule(rec, ad)    ((rec).data_source &= (TRAIN_ARRIVE == ad ? 0xFFFF8FFF:0xFFFC7FFF), (rec).data_source |= (TRAIN_ARRIVE == ad ? 0x01000:0x08000))
#define TG_SetPlanTimeFromAutoCreate(rec, ad)          ((rec).data_source &= (TRAIN_ARRIVE == ad ? 0xFFFF8FFF:0xFFFC7FFF), (rec).data_source |= (TRAIN_ARRIVE == ad ? 0x02000:0x10000))
#define TG_SetPlanTimeFromManualSet(rec, ad)           ((rec).data_source &= (TRAIN_ARRIVE == ad ? 0xFFFF8FFF:0xFFFC7FFF), (rec).data_source |= (TRAIN_ARRIVE == ad ? 0x03000:0x18000))

#define TG_IsPlanTimeFromBasicSchedule(rec, ad)        (((rec).data_source & (TRAIN_ARRIVE == ad ? 0xFFFF8FFF:0xFFFC7FFF)) == (TRAIN_ARRIVE == ad ? 0x00000:0x00000))
#define TG_IsPlanTimeFromAdditionSchedule(rec, ad)     (((rec).data_source & (TRAIN_ARRIVE == ad ? 0xFFFF8FFF:0xFFFC7FFF)) == (TRAIN_ARRIVE == ad ? 0x01000:0x08000))
#define TG_IsPlanTimeFromAutoCreate(rec, ad)           (((rec).data_source & (TRAIN_ARRIVE == ad ? 0xFFFF8FFF:0xFFFC7FFF)) == (TRAIN_ARRIVE == ad ? 0x02000:0x10000))
#define TG_IsPlanTimeFromManualSet(rec, ad)            (((rec).data_source & (TRAIN_ARRIVE == ad ? 0xFFFF8FFF:0xFFFC7FFF)) == (TRAIN_ARRIVE == ad ? 0x03000:0x18000))

#define TG_IsSideConfirmed(rec)     ((rec).data_source & 0x80000000)
#define TG_SetSideConfirmed(rec)       ((rec).data_source |= 0x80000000)
#define TG_ClearSideConfirmed(rec)     ((rec).data_source &= ~0x80000000)

#define TG_SetTrainFromBasicSchedule(coninfo)            ((coninfo).train_source |= 0)
#define TG_SetTrainFromAdditionSchedule(coninfo)         ((coninfo).train_source |= 1)
#define TG_SetTrainFromReportTrue(coninfo)               ((coninfo).train_source |= 2)
#define TG_SetTrainFromReportFake(coninfo)               ((coninfo).train_source |= 3)
#define TG_SetTrainFromFreeDraw(coninfo)                 ((coninfo).train_source |= 4)
#define TG_SetTrainFromPathWay(coninfo)                  ((coninfo).train_source |= 5)
#define TG_SetTrainInfoFromShift(coninfo)                ((coninfo).train_source |= 6)
#define TG_SetTrainFromInformalCreate(coninfo)           ((coninfo).train_source |= 7)

#define TG_IsBasicScheduleTrain(coninfo)                 (0 == (0xffff&&(coninfo).train_source))
#define TG_IsAdditionScheduleTrain(coninfo)              (1 == (0xffff&&(coninfo).train_source))
#define TG_IsReportTrueTrain(coninfo)                    (2 == (0xffff&&(coninfo).train_source))
#define TG_IsReportFakeTrain(coninfo)                    (3 == (0xffff&&(coninfo).train_source))
#define TG_IsFreeDrawTrain(coninfo)                      (4 == (0xffff&&(coninfo).train_source))
#define TG_IsPathWayCreateTrain(coninfo)                 (5 == (0xffff&&(coninfo).train_source))
#define TG_IsProbeCreateTrain(coninfo)                   (6 == (0xffff&&(coninfo).train_source))
#define TG_IsInformalCreateTrain(coninfo)                (7 == (0xffff&&(coninfo).train_source))

#define TG_IsFreightTrainType(type)           ((type) & 0x80)
#define TG_IsPassengerTrainType(type)         (!((type) & 0x80))
#define TG_IsFreightTrain(coninfo)            (TG_IsFreightTrainType((coninfo)->type))
#define TG_IsPassengerTrain(coninfo)          (TG_IsPassengerTrainType((coninfo)->type))
#define TG_IsNullTrainType(type)     (type==255)   // type ==0 是特快!

#define TG_GetAdjustBeginSta(conptr)             ((conptr)->adjust_begin_sta & ~0x0080)
#define TG_IfAdjustBeginStaManualSet(conptr)     (!((conptr)->adjust_begin_sta & 0x0080))
#define TG_IfAdjustBeginStaAutoSet(conptr)       ((conptr)->adjust_begin_sta & 0x0080)
#define TG_SetAdjustBeginStaAutoSet(conptr)      ((conptr)->adjust_begin_sta |= 0x0080)
#define TG_GetAdjustEndSta(conptr)               ((conptr)->adjust_end_sta)

#define TG_SetConsoleTrainNotJoinAdjust(conptr)  ((conptr)->adjust_flag |= 0x01)
#define TG_SetConsoleTrainJoinAdjust(conptr)     ((conptr)->adjust_flag &= ~0x01)
#define TG_IsConsoleTrainNotJoinAdjust(conptr)   ((conptr)->adjust_flag & 0x01)
#define TG_IsConsoleTrainJoinAdjust(conptr)      (!((conptr)->adjust_flag & 0x01))

#define TG_SetConsoleTrainLink(conptr)          ((conptr)->adjust_flag |= 0x04)
#define TG_SetConsoleTrainNoLink(conptr)        ((conptr)->adjust_flag &= ~0x04)
#define TG_IsConsoleTrainLink(conptr)           ((conptr)->adjust_flag & 0x04)
#define TG_IsConsoleTrainNoLink(conptr)         (!((conptr)->adjust_flag & 0x04))

#define TG_ClearBorderTimeAuto(conptr)          ((conptr)->adjust_flag &= 0x3F)
#define TG_SetBorderTimeAuto(conptr)            ((conptr)->adjust_flag &= 0x3F, (conptr)->adjust_flag |= 0x40)
#define TG_SetBorderTimeManual(conptr)          ((conptr)->adjust_flag &= 0x3F, (conptr)->adjust_flag |= 0x80)
#define TG_IsBorderTimeNotSet(conptr)           (((conptr)->adjust_flag & 0xC0) == 0x00 ? TRUE : FALSE)
#define TG_IsBorderTimeAuto(conptr)             (((conptr)->adjust_flag & 0xC0) == 0x40 ? TRUE : FALSE)
#define TG_IsBorderTimeManual(conptr)           (((conptr)->adjust_flag & 0xC0) == 0x80 ? TRUE : FALSE)

#define TG_SetLocoPosMid(conptr)                ((conptr)->adjust_flag |= 0x200)    //bit 8

#define IsAbsStartStopUp(conptr)			(((conptr).adjust_flag&0x6000)==0x2000) // 区间始发列车
#define IsAbsStartStopDown(conptr)			(((conptr).adjust_flag&0x6000)==0x4000) // 区间始发列车

#define TG_SetConsoleTrainNotExistSubao(conptr)  ((conptr)->status &= 0xFFF8)   //bit 0-2
#define TG_IsConsoleTrainExistSubao(conptr)      ((conptr)->status & 0x0007) 

#define TG_SetTrainFromShift(status)  ( (status)|= 0x10)
#define TG_IsTrainFromShift(status) ( ((status)&0x10) == 0x10)
#define TG_ClearTrainFromShift(status)  ( (status)&= ~0x10)

#define TG_SetSubaoFromStation(conptr)           ((conptr)->status &= ~0x7,(conptr)->status |= 0x1)
#define TG_SetSubaoFromPreConsole(conptr)        ((conptr)->status &= ~0x7,(conptr)->status |= 0x2)
#define TG_SetSubaoFromDispatcher(conptr)        ((conptr)->status &= ~0x7,(conptr)->status |= 0x3)
#define TG_SetSubaoFromWireless(conptr)          ((conptr)->status &= ~0x7,(conptr)->status |= 0x4)
#define TG_IsSubaoFromStation(conptr)            (((conptr)->status & 0x7) == 0x1)
#define TG_IsSubaoFromPreConsole(conptr)         (((conptr)->status & 0x7) == 0x2)
#define TG_IsSubaoFromDispatcher(conptr)         (((conptr)->status & 0x7) == 0x3)
#define TG_IsSubaoFromWireless(conptr)           (((conptr)->status & 0x7) == 0x4)

#define SetConsoleTrainSecret(conptr)          ((conptr).status |= 0x8)
#define SetConsoleTrainNotSecret(conptr)       ((conptr).status &= ~0x8)
#define IsConsoleTrainSecret(conptr)           ((conptr).status & 0x8)

#define SetTrainFromShift(conptr)		((conptr).status |= 0x10)
#define ResetTrainFromShift(conptr)		((conptr).status &= ~0x10)
#define IsTrainFromShift(conptr)		(((conptr).status & 0x10) == 0x10)


#define IsConTrainUpDir(conptr)          ((conptr)->direction == 0)
#define IsConTrainDownDir(conptr)        ((conptr)->direction == 1)
#define IsTrainUpDir(dir)          		((dir) == 0)
#define IsTrainDownDir(dir)        		((dir) == 1)

#define TG_SetDieselEngineExt(enginetype)		    ((enginetype) &= 0xF0)                        //bit 0-3 
#define TG_SetElecEngineExt(enginetype)		    ((enginetype) &= 0xF0, (enginetype)|= 0x01) 
#define TG_IsDieselEngineExt(enginetype)		    (((enginetype) & 0x0F) == 0x00) 
#define TG_IsElecEngineExt(enginetype)		    (((enginetype) & 0x0F) == 0x01) 
#define TG_SetTractionEngineExt(enginetype)	    ((enginetype) &= 0xCF)                        //bit 4,5 
#define TG_SetAttachEngineExt(enginetype)		    ((enginetype) &= 0xCF, (enginetype)|= 0x10)
#define TG_IsTractionEngineExt(enginetype)	    (((enginetype) & 0x30) == 0x00) 
#define TG_IsAttachEngineExt(enginetype)		    (((enginetype) & 0x30) == 0x10) 
#define TG_SetEngineFromDispatcher(enginetype)    ((enginetype) &= 0x3F)                        //bit 6,7 
#define TG_SetEngineFromSubao(enginetype)         ((enginetype) &= 0x3F, (enginetype)|= 0x40) 
#define TG_IsEngineFromDispatcher(enginetype)     (((enginetype) & 0xC0) == 0x00) 
#define TG_TG_IsEngineFromSubao(enginetype)          (((enginetype) & 0xC0) == 0x40) 

#define TG_IsCheckNCWayStop(rec)          ((rec.flag & 0x03) == 0x01)
#define TG_IsCheckNCWayPass(rec)          ((rec.flag & 0x03) == 0x02)
#define TG_IsNoCheckNCWayStopPass(rec)    ((rec.flag & 0x03) == 0x00)
#define TG_IsCheckNCWayEntry(rec)         (rec.entry  != 0xFF)
#define TG_IsCheckNCWayExit(rec)          (rec.exit  != 0xFF)
#define TG_IsCheckNCWaySide(rec)          (rec.side  != 0xFF)

#define TG_IsCheckNCWayStart(recs)              (((recs).flag & 0x0003) == 0x0001)
#define TG_IsCheckNCWayAccept(recs)             (((recs).flag & 0x0003) == 0x0002)
#define TG_IsNoCheckNCWayStartAccept(recs)      (((recs).flag & 0x0003) == 0x0000)
#define TG_IsCheckNCWayTerminal(recs)           (((recs).flag & 0x000c) == 0x0001)
#define TG_IsCheckNCWayGiveout(recs)            (((recs).flag & 0x000c) == 0x0002)
#define TG_IsNoCheckNCWayTerminalGiveout(recs)  (((recs).flag & 0x000c) == 0x0000)
#define TG_IsCheckNCWayActual(recs)             (((recs).flag & 0x0030) == 0x0010)
#define TG_IsCheckNCWayPlan(recs)               (((recs).flag & 0x0030) == 0x0020)
#define TG_IsNoCheckNCWayActualPlan(recs)       (((recs).flag & 0x0030) == 0x0000)

#define TG_IsCheckNCWaySame(recs)               (((recs).flag & 0x00C0) == 0x0000 || ((recs).flag & 0x00C0) == 0x0040)
#define TG_ISCheckNCWayEmbed(recs)              (((recs).flag & 0x00C0) == 0x0000 || ((recs).flag & 0x00C0) == 0x0080)


#define TG_GetTrainRecordDir(rec)            ((rec).entry != 0xFF ? TrainDirection((rec).station, (rec).entry, TRAIN_INTO_STATION) : TrainDirection((rec).station, (rec).exit, TRAIN_OUT_STATION))

#define TG_GetValidADTime(rec)           ((!(rec).arrive || NULL_TIME == (rec).arrive) ? (rec).depart:(rec).arrive)
#define TG_GetValidDATime(rec)           ((!(rec).depart || NULL_TIME == (rec).depart) ? (rec).arrive:(rec).depart)
#define TG_SetArriveTimeOff(rec, toff)   ((rec).arrive += (NULL_TIME == (rec).arrive ? 0 : (int)(toff)))
#define TG_SetDepartTimeOff(rec, toff)   ((rec).depart += (NULL_TIME == (rec).depart ? 0 : (int)(toff)))
#define TG_SetTrainRecordTimeOff(rec, toff)   (SetArriveTimeOff(rec, toff), SetDepartTimeOff(rec, toff))

#define TG_GetLogoTrainId(coninfo)       (NULL == (coninfo) ? "NULL" : 0 == (coninfo)->train_id[0][0] ? "" : (coninfo)->train_id[0])
#define TG_CopyTrainId(new_t, old_t)     {strncpy((new_t), (old_t), sizeof(TRAIN_NO)-1); new_t[sizeof(TRAIN_NO)-1]=0;}

#define TG_IsInhibitStation(schdblock)                ((schdblock).position & 0x03)
#define TG_IsInhibitFirstStation(schdblock)           (((schdblock).position & 0x01) == 0x01)
#define TG_IsInhibitSecondStation(schdblock)          (((schdblock).position & 0x02) == 0x02)

#define TG_QUALIFY_TRAININDEX(tindex)    if (0 == (tindex) || (TRAIN_INDEX)-1 == (tindex)) (tindex) = NULL_TRAININDEX;
#define TG_QUALIFY_STATION(sta)          if (0 == (sta)    || (STATION_NO)-1  == (sta))    (sta)    = NULL_STATION; 
#define TG_QUALIFY_ENTRY(entry)          if (0 == (entry)  || (ENTRY)-1 == (entry))        (entry)  = NULL_ENTRY; 
#define TG_QUALIFY_SIDE(side)            if (0 == (side)   || (SIDE)-1  == (side))         (side)   = NULL_SIDE; 

// record 中的not_meet 定义
#define TG_NOTMEET_CRH(x)          ((x & NOTMEET_CRH)==NOTMEET_CRH)
#define TG_NOTMEET_SEPECIAL_Z(x)   ((x & NOTMEET_SEPECIAL_Z)==NOTMEET_SEPECIAL_Z)
#define TG_NOTMEET_PASSENGER(x)    ((x & NOTMEET_PASSENGER)==NOTMEET_PASSENGER)
#define TG_NOTMEET_FREIGHT(x)      ((x & NOTMEET_FREIGHT)==NOTMEET_FREIGHT)
#define TG_NOTMEET_SPACE(x)          ((x & NOTMEET_SPACE)==NOTMEET_SPACE)

#define TG_NOTMEET_200OrUp(x)   ((x & NOTMEET_200_orUp)==NOTMEET_200_orUp)
#define TG_NOTMEET_120to200(x)    ((x & NOTMEET_120_200)==NOTMEET_120_200)
#define TG_NOTMEET_120Low(x)      ((x & NOTMEET_120_Low)==NOTMEET_120_Low)
#define TG_NOTMEET_All(x)      ((x & NOTMEET_ALL)==NOTMEET_ALL)

// shoud be same with gpc
#define TG_QUALIFY_TIME(ltime)           (ltime /60 *60)





struct TG_PassWayInfo{
	 short Pass_way_id;          //经路号
	 char  Pass_way_name[40];    //经路名称
     short Console_id;           //调度台号码
	 short Begin_sta;            //起始车站
	 short End_sta;              //终止车站
	 TG_PassWayInfo()
	 {
		 memset(this, 0, sizeof(TG_PassWayInfo));
	 }
};

//          2 : 站信息 
struct TG_PassWaySta
{
	 short Pass_way_id;          //经路号码
	 short in_sta_idx;           //入口站序号
	 short out_sta_idx;          //出口站序号
	 short Entry;                //入口
	 short Station;              //站号
	 short Exit;                 //出口
	 TG_PassWaySta()
	 {
		 memset(this, 0, sizeof(TG_PassWaySta));
	 }
};

struct TG_PassWayUtility{
	 char PREFIX[10];                  //车次前缀
	 unsigned long START_TRAIN_NUM;    //起始车次
	 unsigned long END_TRAIN_NUM;      //终止车次
	 short Station;                    //站号
	 short Entry;                       //入口
	 short Exit;                        //出口
     short DIRECTIONS;                  //车次方向
	 short CONSOLE_ID;                  //调度台号码
	 short PASS_WAY_ID;                //径路号
     BYTE  ACPT_FLAG;                   //接入，始发
	 BYTE  GVT_FLAG;                    //终点，交出
	  TG_PassWayUtility()
	 {
		 memset(this, 0, sizeof(TG_PassWayUtility));
	 }
};

struct TGBasicTrainListRequest
{
	ENTITY_ID	gpc;
	short		con_id;
	STATION_NO	station;
	TIME		stime;
	TIME		etime;
	TIME		shift_begin_time;
	TIME		shift_end_time;	
	enum	INIT_TRAINTYPE {
		INIT_PASSENGER,
		INIT_FREIGHT,
		INIT_ALL_TYPE
	} train_type;
	enum	INIT_DIRECTION {
		INIT_UP,
		INIT_DOWN,
		INIT_ALL_DIR
	} direction;
	enum	INIT_START_ACCEPT {
		INIT_START,
		INIT_ACCEPT,
		INIT_ALL_SA
	} flag;
	enum	INIT_HAT_FLAG {
		INIT_NORMAL_TRAIN,
		INIT_HAT_TRAIN
	} hat_flag;
};

struct TGSelectedBasicTrain{
	TRAIN_NO train_id;
	TRAIN_INDEX train_index;
 	TIME  start_date;
	TIME  start_time;
	TIME  depart_date;
	TIME  depart_time;
	BOOL  exist;//add by cf after 9-4
	enum BASIC_TRAIN_LIST_EXIST {
		BT_EXIST,
		BT_NONE
	} exist_train;
	enum BASIC_TRAIN_LIST_DIR {
		BT_UP,
		BT_DOWN,
		BT_ALL_DIR
	}  dir;
	enum BASIC_TRAIN_LIST_TYPE {
		BT_PASSENGER,
		BT_FREIGHT,
		BT_ALL_TYPE
	} type;
	enum BASIC_TRAIN_LIST_HAT {
		BT_HAT_TRAIN,
		BT_NORMAL_TRAIN
	}  hat_train;
	enum BASIC_TRAIN_LIST_ADD {
		BT_NORMAL_SCHEDULE,
		BT_LONG_STOP,
		BT_LONG_INCREASE
	} add_train;
};

struct TGSelectedBasicMark{
	TRAIN_INDEX mark_index;
	BOOL  exist;//add by cf after 9-4
	BOOL  checked;
	int   mark_type;
	int   row_index;
};

struct TGMarkSelector{
	TIME  start_time;
	TIME  end_time;
	int   station;
};
#endif /** THE END **/

