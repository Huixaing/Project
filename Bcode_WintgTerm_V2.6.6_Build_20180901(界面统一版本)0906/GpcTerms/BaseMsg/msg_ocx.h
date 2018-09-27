#ifndef _CLBTH_MSGDEF_OCX_SCHD_H_
#define _CLBTH_MSGDEF_OCX_SCHD_H_

#include "msg_schd.h"
#include "basemsg.h"

CLBTH_NAMESPACE_BEGIN

class MsgTrainRecordOCX: public BaseMsg
{
	DECLARE_CLBTH_OCX(MsgTrainRecordOCX)
	int m_Version;

public:
	MsgTrainRecordOCX(int version = 0);
	virtual ~MsgTrainRecordOCX();

private:
	MsgTrainRecordOCX(const MsgTrainRecordOCX& rhs);                       
	MsgTrainRecordOCX& operator=(const MsgTrainRecordOCX& rhs); 

protected:
	virtual HRCODE doSerialize(Archive& ar);
	virtual size_t countObjectLength();
public:
	short		in_station_index;
	short		out_station_index;

	short    opms_station_text_state;
	char    opms_station_text[256];
	char    station_text[256];
	char	abs_text[256];
	char	alarm_text[256];
	char    plan_note[256];

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

	I_TIME    arrive;                                   // 到达时间
	I_TIME    depart;                                   // 出发时间
	I_TIME    plan_arrive;                              // 图定到达时间
	I_TIME    plan_depart;                              // 图定出发时间
	I_TIME    send_plan_time;                           // 计划下达时间    

	ISDWORD    stop_reason;                             // bit 32-16，到达接点，出发接点，当delete时记录原rec_index
	ISDWORD    late_reason;                             //                 

	ISDWORD    early_depart_time;                       // 允许提前发车时间
	ISDWORD    run_fast_slow_time;                      // 赶缓点信息
	
	ISDWORD    route_flag;                              // 进路办理状态   
	ISDWORD    rank;                                    // 重点标记信息
	
	ISDWORD    speed_limit;                             // 单个列车限速信息
	ISDWORD    stop_condition;                          //停车条件
	
	ISDWORD    adjust_status;                           // 调整状态
	short   spec_run_time;                              // 指定运行时间

	ISDWORD    can_not_meet;                            // 禁会车种信息
	ISDWORD    super_interval_ltime;                    // 指定列车间隔
	
	short         special_position;
	short         special_time_1;
	short         special_time_2;
	TRAIN_INDEX   special_train_index;

	ISDWORD    data_source;
	ISDWORD    flag;
    
	/*   区间停车  */
	struct ABS_STOP
	{
		BYTE    flag;				// 区间or站外
		short   position;           // 停车位置,多次停车时使用
		I_TIME  start_time;         // 停车开始时间
		I_TIME  stop_time;          // 停车结束时间
	};
	short      abs_stop_count;	// 停车个数
	ABS_STOP   abs_stop[8];		// 停车信息
	ISDWORD    display_status;
};
DECLARE_SERIALIZE_OPERATOR(MsgTrainRecordOCX)

/* 全部列车信息 */
class MsgTrainOCX: public BaseMsg
{
	DECLARE_CLBTH_OCX(MsgTrainOCX)
	int m_Version;

public:
	MsgTrainOCX();
	virtual ~MsgTrainOCX();

private:
	MsgTrainOCX(const MsgTrainOCX& rhs);                       
	MsgTrainOCX& operator=(const MsgTrainOCX& rhs); 

protected:
	virtual HRCODE doSerialize(Archive& ar);
	virtual size_t countObjectLength();

public:
	MsgTrainInfo        info;        //列车信息

	short               engine_count;//机车数
	MsgTrainEngineInfo* engine_ptr;  //机车信息

	short				rec_count;     //列车到发数
	MsgTrainRecordOCX*   rec_ptr;       //列车到发记录


	TRAIN_INDEX train_index;
	TRAIN_INDEX prev_train_index;
	IBYTE		self_update;
};
DECLARE_SERIALIZE_OPERATOR(MsgTrainOCX)

/* 封锁信息 */
class MsgSchdBlockOCX: public MsgSchdBlock
{
	DECLARE_CLBTH_OCX(MsgSchdBlockOCX)
	int m_Version;

public:
	MsgSchdBlockOCX(int version = 0);
	virtual ~MsgSchdBlockOCX();

private:
	MsgSchdBlockOCX(const MsgSchdBlockOCX& rhs);                       
	MsgSchdBlockOCX& operator=(const MsgSchdBlockOCX& rhs); 

public:
	IBYTE self_update;
	TRAIN_INDEX mark_index;
	USHORT station1;
	USHORT station2;
	IBYTE  direction;

protected:
	virtual HRCODE doSerialize(Archive& ar);
	virtual size_t countObjectLength();
};
DECLARE_SERIALIZE_OPERATOR(MsgSchdBlockOCX)

CLBTH_NAMESPACE_END

#endif
