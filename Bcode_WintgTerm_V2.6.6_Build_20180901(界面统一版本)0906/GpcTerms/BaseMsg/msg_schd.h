#ifndef _CLBTH_MSGDEF_MSG_SCHD_H_
#define _CLBTH_MSGDEF_MSG_SCHD_H_

#include "ctc_defines.h"
#include "basemsg.h"

CLBTH_NAMESPACE_BEGIN

// 运行计划消息戳
class AFX_EXT_CLASS MsgSchdStamp: public BaseMsg
{
    DECLARE_CLBTH(MsgSchdStamp)
	int m_Version;

public:
	MsgSchdStamp(int version = 0);
	virtual ~MsgSchdStamp();

	virtual HRCODE doSerialize(Archive& ar);
	virtual size_t countObjectLength();

public:
	CONID          con_id;    //调度台号
	IBYTE          schd_type; //运行线类型
	TRAIN_INDEX    index;     //索引
	I_TIME         ltime;     //报点时间
	ENTITY_ID      operatr;   //操作人
};
DECLARE_SERIALIZE_OPERATOR(MsgSchdStamp)

/* 列车基本信息 */
class AFX_EXT_CLASS MsgTrainInfo: public BaseMsg
{
	DECLARE_CLBTH(MsgTrainInfo)
	int m_Version;

public:
	MsgTrainInfo(int version = 0);
	virtual ~MsgTrainInfo();

protected:
	virtual HRCODE doSerialize(Archive& ar);
	virtual size_t countObjectLength();
public:

	/*   基础信息  */
	char          train_name[MAX_TRAIN_NAME_LEN];    //  车次号组合
	TRAIN_KEY     train_key;                         // IP跟踪的内部索引     

	TRAIN_INDEX   gpc_index;           // GPC产生的索引
	TRAIN_INDEX   basic_index;         // 根据基本图生成时, 基本图的索引
	TRAIN_INDEX   shift_index;         // 根据日班计划生成时,日班计划的索引
	char          shift_string[32];    // OPMS4.0标示
	I_TIME        update_time;         // 删除时间
	short         delete_flag;         //  -1, 无效， >=0 延迟删除记录索引,即有效数据记录个数

	IBYTE         type;                // 列车详细属性 (bit 7: 0 - passenger 1 -freight)
	IBYTE         run_type;            // 列车属性，用于计算区间运行时间和自动排路提前触发时机

	I_TIME        start_plan_time;     // 始发计划时间
	I_TIME        start_actual_time;   // 始发实际时间
	I_TIME        sborder_plan_time;   // 分界口接入计划时间
	I_TIME        sborder_actual_time; // 分界口接入实际时间
	I_TIME        eborder_plan_time;   // 分界口交出计划时间
	I_TIME        eborder_actual_time; // 分界口交出实际时间
	STATION_NO    sborder_sta;         // 分界口接入站号
	STATION_NO    eborder_sta;         // 分界口交出站号
	char          start_sta_name[MAX_STA_NAME_LEN];  // 起始车站名
	char          end_sta_name[MAX_STA_NAME_LEN];    // 终到车站名
	CONID         from_conid;          // 后方调度台号
	CONID         to_conid;            // 前方调度台号

	IBYTE         priority;             // 列车等级
	ISDWORD       train_length;         // 车长
	ISDWORD       train_weight;         // 车重
	ISDWORD       status;
	// bit 0-2:   0 - 不存在速报,       1 - 速报来于车站上报
	//            2 - 速报来于临台传递, 3 - 速报来于调度员手工
	//            4 - 速报来于无线车次号
	// bit 3: 1 - 保密列车
	// bit 4: 1-- 直接从日班计划加车 0-- 其他
	// bit 16-31: 最高两个字节存放 辆数

	/*   特殊列车信息  */
	IBYTE    space_rank;               //间隔等级,如超宽
	IBYTE    length_rank;              //长度等级
	IBYTE    super_rank;               //超限等级
	IBYTE    military_rank;            //军用等级

	/*   列车运行信息  */
	ISDWORD     engine_utility;        //机车用途
	//    0: normal
	//    1: limited_time
	//    2: no locomotive
	ISDWORD     side_utility;          //股道用途
	//    bit0:   1 :  需要高站台
	/*   自动调整信息  */
	short   adjust_begin_sta;          // 自动调整开始站    bit 7: 1 set by default,  0 set manual
	short   adjust_end_sta;            // 自动调整结束站

	ISDWORD    adjust_flag;
	// bit 0: 0 - 参于自动调整   1 - 不参于自动调整
	// bit 1:
	// bit 2: 0 - generate time_link    1 - not
	// bit 3: 0 - not in save before adjust   1 - do no use
	// bit 7-6:  0 - 分界口接入时间未设置
	//           1 - 分界口接入时间手工未设置
	//           2 - 分界口接入时间由系统自动设置
	// bit 10-8:  机车号显示位置  0-不显示   1-下 2-中  4-上
	// bit 11:   助调能够操纵的列车标志---区间作业车
	// bit 12:   区间分部运行 1:是  0：否
	// bit 13-14: 区间始发列车 停上区间 01 下区间 10
	// bit 15: 补机途中折返 1：是 0:否
	// bit 16-31: 区间始发列车 区间运行时分
	ISDWORD    train_source;           // 数据来源

	short   comment_len;               // 注释长度
	char*   comment;                   // 注释

	HRCODE SetComment(const char* c);
};
DECLARE_SERIALIZE_OPERATOR(MsgTrainInfo)

/*   列车机车信息  */
class AFX_EXT_CLASS MsgTrainEngineInfo: public BaseMsg
{
	DECLARE_CLBTH(MsgTrainEngineInfo)
	int m_Version;

public:
	MsgTrainEngineInfo(int version = 0);
	virtual ~MsgTrainEngineInfo();

protected:
	virtual HRCODE doSerialize(Archive& ar);
	virtual size_t countObjectLength();
public:
	TRAIN_NO     engine_id;            // 机车号
	TRAIN_NO     engine_type;          // 机车类型名称

	ISDWORD         engine_status;
	// bit 7-6:  0 - 调度员手工设置 /  1- 根据速报设置 / 2- 无线车次信息
	// bit 5-4:  0 - 牵引  1 - 附挂
	// bit 3-0:  0 - 内燃  1 - 电力

	short        attach_sta;           // 机车附挂车站   index of trainrecord
	short        unattach_sta;         // 机车甩挂车站
};
DECLARE_SERIALIZE_OPERATOR(MsgTrainEngineInfo)

/*   列车区间停车信息  */
class AFX_EXT_CLASS MsgTrainAbsStop: public BaseMsg
{
	DECLARE_CLBTH(MsgTrainAbsStop)
	int m_Version;

public:
	MsgTrainAbsStop(int version = 0);
	virtual ~MsgTrainAbsStop();

protected:
	virtual HRCODE doSerialize(Archive& ar);
	virtual size_t countObjectLength();
public:
	short   position;             // 区间停车位置,多次停车时使用
	I_TIME    start_time;         // 区间停车开始时间
	I_TIME    stop_time;          // 区间停车结束时间
};
DECLARE_SERIALIZE_OPERATOR(MsgTrainAbsStop)

/* 列车到发记录 */
class AFX_EXT_CLASS MsgTrainRecord: public BaseMsg
{
	DECLARE_CLBTH(MsgTrainRecord)
	int m_Version;

public:
	MsgTrainRecord(int version = 0);
	virtual ~MsgTrainRecord();

protected:
	virtual HRCODE doSerialize(Archive& ar);
	virtual size_t countObjectLength();
public:
	TRAIN_NO     arrive_train_id;                       // 到达车次
	TRAIN_NO     original_arrive_train_id;              // 原始到达车次，戴帽时使用

	TRAIN_NO     depart_train_id;                       // 出发车次
	TRAIN_NO     original_depart_train_id;              // 原始出发车次，戴帽时使用

	STATION_NO   station;                               // 站号
	ENTRY        entry;                                 // 入口号
	ENTRY        exit;                                  // 出口号

	SIDE     arrive_side;                               // 到达股道
	SIDE     plan_arrive_side;                          // 图定到达股道
	SIDE     depart_side;                               // 出发股道
	SIDE     plan_depart_side;                          // 图定出发股道
	short    min_stop_time;                             // 营业时间

	I_TIME    arrive;                                   // 到达时间
	I_TIME    depart;                                   // 出发时间
	I_TIME    plan_arrive;                              // 图定到达时间
	I_TIME    plan_depart;                              // 图定出发时间
	I_TIME    send_plan_time;                           // 计划下达时间    

	ISDWORD    stop_reason;                             // bit 32-16，到达接点，出发接点，当delete时记录原rec_index
	ISDWORD    late_reason;                             //                 

	ISDWORD    early_depart_time;                       // 允许提前发车时间
	// 0x0000 -> 0x7FFF, 允许提前发车的秒数
	// 0x8000 允许提前发车无限制
	// 0x0000 不允许提前发车
	ISDWORD    run_fast_slow_time;                      // 赶缓点信息
	//    bit 7:   0 - 快,   1 - 慢
	//    bit 6-0:   赶缓点时间 (分钟)

	ISDWORD    route_flag;                              // 进路办理状态   
	//    bit 10:   00/01/10/11  接车进路  no(clear)/doing/OK/OCCUPY
	//    bit 32:   00/01/10/11  发车进路  no(clear)/doing/OK/OCCUPY
	//    bit 4: 1:车已进股道
	ISDWORD    rank;                                    // 重点标记信息
	//    bit 0:  超限
	//    bit 1:  重点警卫
	//    bit 2:  毒品
	//    bit 3:  军运
	ISDWORD    speed_limit;                             // 单个列车限速信息
	//     0: 无限速, else: 最大限速值

	ISDWORD    stop_condition;                          //停车条件
	//    bit 1:    1-原定义为通过列车(技规)用于列车交会时自动排路
	//    bit 2:    1-无计划不得停车
	//    bit 3:    1-不得使用其他股道
	//    bit 4:    1-不得侧线通过
	//    bit 5:    1-不得自动办理接车进路
	//    bit 6:    1-不得自动办理发车进路
	//    bit 7:    0-按会让条件办理出发进路,  1-按计划时间办理出发进路
	//    bit 8:    1-禁止停车
	//    bit 9:    1-必须停车

	ISDWORD    adjust_status;                           // 调整状态
	//     bit 0:  0 : 到达时间为计划
	//             1 : 到达时间为实际
	//     bit 1:  0 : 出发时间为计划
	//             1 : 出发时间为实际
	//     bit 2:  0 : 正常
	//             1 : 存在错误信息
	//     bit 3:  0 : 调整成功
	//             1 : 调整失败
	//     bit 4:  1 : 技术停车
	//     bit 5:  1 :
	//     bit 6:  0 : arrivetime is same with the nextcon's arr
	//             1 : not same(only for inter_sta)
	//     bit 7:  0 : departtime is same with the nextcon's dep
	//             1 : not same(only for inter_sta)
	//     bit 8 :   到达时间锁
	//     bit 9 :   出发时间锁
	//     bit 10:  0  实际到达时间为人工报点
	//              1  实际到达时间为自动报点
	//     bit 11:  0  实际出发时间为人工报点
	//              1  实际出发时间为自动报点
	//     bit 12:
	//     
	//     bit 13,14:  0  未发送到邻台（台间站）
	//                 1  邻台拒绝
	//                 2  邻台接受
	//     bit 15:  0  无电力牵引
	//              1  电力牵引
	//     bit 16:  0 非接口车站  1 分界口车站
    //     bit 17:  0 非台间站 1 台间站
	//     bit 18:  0 不自动折返车次 1 自动折返车次

	short   spec_run_time;                              // 指定运行时间

	ISDWORD    can_not_meet;                            // 禁会车种信息
	//     bit 0: 禁会动车组
	//     bit 1: 禁会直特
	//     bit 2: 禁会客车
	//     bit 3: 禁会列车
	//     bit 4: 禁会
	//     bit 5: 禁会
	// can_not_meet: 低字节： 存放老禁会标志 次低字节 存放新禁会标志

	ISDWORD    super_interval_ltime;                    // 指定列车间隔
	//        3-0: 0 使用默认配制, else: 左侧间隔 (min)
	//        7-4: 0 使用默认配制, else: 右侧间隔 (min)

	short         special_position;
	short         special_time_1;
	short         special_time_2;
	TRAIN_INDEX   special_train_index;

	ISDWORD    data_source;
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

	// bit 31:   1-车站确认股道

	ISDWORD    flag;
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

	/*   区间停车  */
	MsgTrainAbsStop   stop_out_of_signal;               // 机外停车
	short             abs_stop_count;                   // 区间停车个数
	MsgTrainAbsStop   abs_stop[MAX_ABS_STOP_COUNT];     // 区间停车

	/*   列车文字信息  0 - abs  1 - station  2 - alarm */
	char    text[3][MAX_TRAIN_TEXT_LENGTH];
};
DECLARE_SERIALIZE_OPERATOR(MsgTrainRecord)

/* 全部列车信息 */
class AFX_EXT_CLASS MsgTrain: public BaseMsg
{
	DECLARE_CLBTH(MsgTrain)
	int m_Version;

public:
	MsgTrain();
	virtual ~MsgTrain();

protected:
	virtual HRCODE doSerialize(Archive& ar);
	virtual size_t countObjectLength();

public:
	MsgTrainInfo        info;        //列车信息

	short               engine_count;//机车数
	MsgTrainEngineInfo* engine_ptr;  //机车信息

	short             rec_count;     //列车到发数
	MsgTrainRecord*   rec_ptr;       //列车到发记录
};
DECLARE_SERIALIZE_OPERATOR(MsgTrain)

/* 增加列车信息 */
class MsgTrainUpdate;
class AFX_EXT_CLASS MsgTrainAdd: public MsgTrain
{
	DECLARE_CLBTH(MsgTrainAdd)

public:
	MsgTrainAdd();
	MsgTrainAdd(const MsgTrainUpdate & update);
	virtual ~MsgTrainAdd();
private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	MsgSchdStamp stamp;
};
DECLARE_SERIALIZE_OPERATOR(MsgTrainAdd)

/* 删除列车信息 */
class AFX_EXT_CLASS MsgTrainDelete: public MsgTrain
{
	DECLARE_CLBTH(MsgTrainDelete)
public:
	MsgTrainDelete();
	MsgTrainDelete(const MsgTrainUpdate & update);
	virtual ~MsgTrainDelete();
private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	MsgSchdStamp stamp;
};
DECLARE_SERIALIZE_OPERATOR(MsgTrainDelete)

/* 更新全部列车信息 */
class AFX_EXT_CLASS MsgTrainUpdate: public MsgTrain
{
	DECLARE_CLBTH(MsgTrainUpdate)
public:
	MsgTrainUpdate();
	MsgTrainUpdate(const MsgTrainAdd& add);
	virtual ~MsgTrainUpdate();
private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	MsgSchdStamp stamp;
};
DECLARE_SERIALIZE_OPERATOR(MsgTrainUpdate)
/* 更新邻台列车信息,响应邻台申请 */
class AFX_EXT_CLASS MsgNeibTrainUpdate: public MsgTrain
{
	DECLARE_CLBTH(MsgNeibTrainUpdate)
public:
	MsgNeibTrainUpdate();
	virtual ~MsgNeibTrainUpdate();
private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
public:
	ENTITY_ID   send_entity;//发送实体
	ENTITY_ID   recv_entity;//接收实体

	CONID       send_con_id;//发送调度台号
	CONID       recv_con_id;//接收调度台号

	I_TIME        begin_time;//邻台信息开始时间
	I_TIME        end_time;//邻台信息结束时间
	I_TIME        send_time;  //发送时间
	USERNAME    sender_name;//发送者名字
};
DECLARE_SERIALIZE_OPERATOR(MsgNeibTrainUpdate)

/* 返回列车信息 */
class AFX_EXT_CLASS MsgTrainData: public MsgTrain
{
	DECLARE_CLBTH(MsgTrainData)

public:
	MsgTrainData();
	virtual ~MsgTrainData();
private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	MsgSchdStamp  stamp;
};
DECLARE_SERIALIZE_OPERATOR(MsgTrainData)

/* 更新列车基本信息 */
class AFX_EXT_CLASS MsgTrainUpdateInfo: public MsgTrainInfo
{
	DECLARE_CLBTH(MsgTrainUpdateInfo)
public:
	MsgTrainUpdateInfo();
	virtual ~MsgTrainUpdateInfo();
private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	MsgSchdStamp stamp;
};
DECLARE_SERIALIZE_OPERATOR(MsgTrainUpdateInfo)

/* 更新列车机车号信息 */
class AFX_EXT_CLASS MsgTrainUpdateEngine: public BaseMsg
{
	DECLARE_CLBTH(MsgTrainUpdateEngine)
public:
	MsgTrainUpdateEngine();
	virtual ~MsgTrainUpdateEngine();
private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	short               engine_count;//机车数
	MsgTrainEngineInfo* engine_ptr;//机车信息

	MsgSchdStamp stamp;

	HRCODE SetEngine(short count, const MsgTrainEngineInfo* engine);
};
DECLARE_SERIALIZE_OPERATOR(MsgTrainUpdateEngine)

/* 更新列车到发点信息 */
class AFX_EXT_CLASS MsgTrainUpdateRecord: public BaseMsg
{
	DECLARE_CLBTH(MsgTrainUpdateRecord)
public:
	MsgTrainUpdateRecord();
	virtual ~MsgTrainUpdateRecord();
protected:
	virtual HRCODE doSerialize(Archive& ar);
	virtual size_t countObjectLength();

public:
	short            rec_cnt;//
	MsgTrainRecord   *rec_ptr;//列车到发记录
	short            *rec_index;   // 记录下标

	MsgSchdStamp     stamp;//运行线特征
};
DECLARE_SERIALIZE_OPERATOR(MsgTrainUpdateRecord)

/* 封锁信息 */
class AFX_EXT_CLASS MsgSchdBlock: public BaseMsg
{
	DECLARE_CLBTH(MsgSchdBlock)
	int m_Version;

public:
	MsgSchdBlock(int version = 0);
	virtual ~MsgSchdBlock();

protected:
	virtual HRCODE doSerialize(Archive& ar);
	virtual size_t countObjectLength();

public:
	TRAIN_INDEX   gpc_index;         //gpc产生的索引
	TRAIN_INDEX   basic_index;       //根据基本图产生的索引
	TRAIN_INDEX   shift_index;       //根据日班计划产生的索引
	char          shift_string[32];  // OPMS4.0标示
	I_TIME        update_time;       // 删除时间
	short         delete_flag;       //  -1, 无效， >=0 延迟删除记录索引

	I_TIME        start_time;        //封锁开始时间
	I_TIME        end_time;          //封锁结束时间

	IBYTE         type;           //  ABS SLOW,      ABS_INHIBIT,  ELECTRO_FIX,  SIDE_SLOW,    SIDE_INHIBIT,  GRAPH_NOTE, ABS_ELEC_FIX, REGIST_BLOCK_TYPE
	IBYTE         flag;           //  limit_speed    flag          0             limit_speed   type           0,                        block_type(0-half,1-auto,2-tel)
	IWORD         dir;            // used by gpc
	STATION_NO    station;        //  0              0             0             station       station        top_position
	IWORD         index;          // used by gpc
	IWORD         position;       //  limit_length   position      position      limit_length  position       bottom_position
	//     bit 0:  1 -- inhibit first sta, 0 -- not;  bit 1: 1 -- inhibit second sta, 0 -- not ;  when type=SECTION_ELEC_FIX
	IWORD     ele_count;                                 //  元素个数
	IWORD     ele_number[MAX_SCHD_BLOCK_ELE_COUNT];      //  元素列表

	short     text_len;
	char      text[MAX_SCHD_BLOCK_TEXT_LEN];
	IWORD     s_meter;   //first sta num ,when type = SECTION_ELEC_FIX
	IWORD     e_meter;   //second sta num ,when type = SECTION_ELEC_FIX
	IWORD     e_kilo;
	IWORD     slow_time;
	ISDWORD   adjust_status;   // bit 1-0:  0 计划、1 执行、2 实际

	short     comment_len;    //注释长度
	char*     comment;        //注释

	HRCODE SetComment(const char* c);
	void Set(const MsgSchdBlock & msg);
};
DECLARE_SERIALIZE_OPERATOR(MsgSchdBlock)

/* 增加封锁信息 */
class MsgSchdBlockUpdate;
class AFX_EXT_CLASS MsgSchdBlockAdd: public MsgSchdBlock
{
	DECLARE_CLBTH(MsgSchdBlockAdd)
public:
	MsgSchdBlockAdd();
	MsgSchdBlockAdd(const MsgSchdBlockUpdate & msg);
	virtual ~MsgSchdBlockAdd();
private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	MsgSchdStamp stamp;
};
DECLARE_SERIALIZE_OPERATOR(MsgSchdBlockAdd)

/* 更改封锁信息 */
class AFX_EXT_CLASS MsgSchdBlockUpdate: public MsgSchdBlock
{
	DECLARE_CLBTH(MsgSchdBlockUpdate)
public:
	MsgSchdBlockUpdate();
	MsgSchdBlockUpdate(const MsgSchdBlockAdd & msg);
	virtual ~MsgSchdBlockUpdate();
private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	MsgSchdStamp stamp;
};
DECLARE_SERIALIZE_OPERATOR(MsgSchdBlockUpdate)

/* 删除封锁信息 */
class AFX_EXT_CLASS MsgSchdBlockDelete: public MsgSchdBlock
{
	DECLARE_CLBTH(MsgSchdBlockDelete)
public:
	MsgSchdBlockDelete();
	virtual ~MsgSchdBlockDelete();
private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	MsgSchdStamp stamp;
};
DECLARE_SERIALIZE_OPERATOR(MsgSchdBlockDelete)

/* 返回封锁信息 */
class AFX_EXT_CLASS MsgSchdBlockData: public MsgSchdBlock
{
	DECLARE_CLBTH(MsgSchdBlockData)
public:
	MsgSchdBlockData();
	virtual ~MsgSchdBlockData();
private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	MsgSchdStamp    stamp;
};
DECLARE_SERIALIZE_OPERATOR(MsgSchdBlockData)

// gpc消息处理错误反馈
class AFX_EXT_CLASS MsgSchdMsgEcho: public BaseMsg
{
	DECLARE_CLBTH(MsgSchdMsgEcho)
	int m_Version;

public:
	MsgSchdMsgEcho(int version = 0);
	virtual ~MsgSchdMsgEcho();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	short msgid;//消息号
	ISDWORD  ecode;//错误码

	MsgSchdStamp stamp;

	void SetSchdStamp(const MsgSchdStamp& s);
};
DECLARE_SERIALIZE_OPERATOR(MsgSchdMsgEcho)

// 基本图更换消息和反馈
class AFX_EXT_CLASS MsgConvertBasicSchd: public BaseMsg
{
	DECLARE_CLBTH(MsgConvertBasicSchd)
	int m_Version;

public:
	MsgConvertBasicSchd(int version = 0);
	virtual ~MsgConvertBasicSchd();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	CONID conid;//区段号
	enum FLAG
	{
		CONVERT,
		RESTORE,
		ECHO_OK,
		ECHO_FAIL,
	}  flag;
};
DECLARE_SERIALIZE_OPERATOR(MsgConvertBasicSchd)


class AFX_EXT_CLASS MsgSchdLoad: public BaseMsg
{
	DECLARE_CLBTH(MsgSchdLoad)
	int m_Version;

public:
	MsgSchdLoad(int version = 0);
	virtual ~MsgSchdLoad();

	virtual HRCODE doSerialize(Archive& ar);
	virtual size_t countObjectLength();

public:
	CONID         con_id;//调度台号
	IBYTE          schd_type; //运行线类型

	I_TIME          start_time;//开始时间
	I_TIME          stop_time;//结束时间

	int           sta_count;//包含车站数
	STATION_NO    station[128];//站号
};
DECLARE_SERIALIZE_OPERATOR(MsgSchdLoad)

class AFX_EXT_CLASS MsgNoticeNeigbourReceiveSchd: public BaseMsg
{
	DECLARE_CLBTH(MsgNoticeNeigbourReceiveSchd)
	int m_Version;

public:
	MsgNoticeNeigbourReceiveSchd(int version = 0);
	virtual ~MsgNoticeNeigbourReceiveSchd();

	virtual HRCODE doSerialize(Archive& ar);
	virtual size_t countObjectLength();

public:
	ISDWORD         send_id;//发送实体号
	ISDWORD         receive_id;//接收实体号
	IBYTE         schd_type;  //运行线类型
	ISDWORD         nShiftId;//根据日班计划生成的id号
	int          hour;//
	ISDWORD         con_id;//调度台号
};
DECLARE_SERIALIZE_OPERATOR(MsgNoticeNeigbourReceiveSchd)

/* 部分站数据 */
class AFX_EXT_CLASS MsgTrainRecordData: public MsgTrainUpdateRecord
{
	DECLARE_CLBTH(MsgTrainRecordData)
public:
	MsgTrainRecordData();
	virtual ~MsgTrainRecordData();
private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	MsgTrainInfo info;
};
DECLARE_SERIALIZE_OPERATOR(MsgTrainRecordData)

const int MAX_SEARCH_TRAIN_STA = 24;
/* 重复车次判断 */
class AFX_EXT_CLASS MsgCheckDupilicateTrainID: public BaseMsg
{
	DECLARE_CLBTH(MsgCheckDupilicateTrainID)
public:
	MsgCheckDupilicateTrainID();
	virtual ~MsgCheckDupilicateTrainID();
private:
protected:
	virtual size_t countObjectLength();
	virtual HRCODE doSerialize(Archive& ar);

public:
	TRAIN_NO  szTrainNo;//车次号
	CONID   nCon_Id;  // 如果为0，则表示查找所有台数据
	I_TIME    tBeginTm;
	I_TIME    tEndTm;
	int     nStatCount;// 如果为0，则表示查找所有站数据，预留
	STATION_NO  nStno[MAX_SEARCH_TRAIN_STA];
};
DECLARE_SERIALIZE_OPERATOR(MsgCheckDupilicateTrainID)

/* 不重复车次推荐 */
class AFX_EXT_CLASS MsgFindUniqeTrainID: public BaseMsg
{
	DECLARE_CLBTH(MsgFindUniqeTrainID)
public:
	MsgFindUniqeTrainID();
	virtual ~MsgFindUniqeTrainID();
private:
protected:
	virtual size_t countObjectLength();
	virtual HRCODE doSerialize(Archive& ar);

public:
	char    szPrefix[8];   // 比如如果是特快，则此处为 “T”
	CONID   nCon_Id;  // 如果为0，则表示查找所有台数据
	IDWORD    nTrainIDFrom;  // 车次号数值部分最小值
	IDWORD    nTrainIDTo;    // 车次号数值部分最大值
	I_TIME    tBeginTm;
	I_TIME    tEndTm;
	int     nStatCount;// 如果为0，则表示查找所有站数据，预留
	STATION_NO  nStno[MAX_SEARCH_TRAIN_STA];
};
DECLARE_SERIALIZE_OPERATOR(MsgFindUniqeTrainID)

/* 重复车次判断结果 */
class AFX_EXT_CLASS MsgCheckDupilicateTrainIDResult: public MsgCheckDupilicateTrainID
{
	DECLARE_CLBTH(MsgCheckDupilicateTrainIDResult)
public:
	MsgCheckDupilicateTrainIDResult();
	virtual ~MsgCheckDupilicateTrainIDResult();
	MsgCheckDupilicateTrainIDResult(const MsgCheckDupilicateTrainID & msg);
private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	I_BOOL           bResult;  // 此处返回是否重复
	I_TIME           tResultTime; // 此处返回上次重复车次的时间
	CONID nResultConID;
};
DECLARE_SERIALIZE_OPERATOR(MsgCheckDupilicateTrainIDResult)

/* 不重复车次推荐结果 */
class AFX_EXT_CLASS MsgFindUniqeTrainIDResult: public MsgFindUniqeTrainID
{
	DECLARE_CLBTH(MsgFindUniqeTrainIDResult)
public:
	MsgFindUniqeTrainIDResult();
	virtual ~MsgFindUniqeTrainIDResult();
	MsgFindUniqeTrainIDResult(const MsgFindUniqeTrainID & msg);
private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	IDWORD          nResultTrainID;   // 在指定范围内不重复的车次数值，如果找不到，则为0
};
DECLARE_SERIALIZE_OPERATOR(MsgFindUniqeTrainID)

/* 查询车次号列表 */
const int MAX_TRAIN_ID_BUFFER_LEN_PER_PACK=2048;
class AFX_EXT_CLASS MsgGetTrainIDList: public BaseMsg
{
	DECLARE_CLBTH(MsgGetTrainIDList)
public:
	MsgGetTrainIDList();
	virtual ~MsgGetTrainIDList();
private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	CONID   nCon_Id;  // 如果为0，则表示查找所有台数据
	I_TIME    tBeginTm;
	I_TIME    tEndTm;
	int     nStatCount;// 如果为0，则表示查找所有站数据，预留
	STATION_NO  nStno[MAX_SEARCH_TRAIN_STA];//站号
	int     nTrainIDBufferLen;    //申请时为0，返回时为结果数量
	char  szTrainNoBuffer[MAX_TRAIN_ID_BUFFER_LEN_PER_PACK];
};
DECLARE_SERIALIZE_OPERATOR(MsgGetTrainIDList)

// 基本图转储消息和反馈
class AFX_EXT_CLASS MsgConvertBasicSchdNew: public BaseMsg
{
	DECLARE_CLBTH(MsgConvertBasicSchdNew)

public:
	MsgConvertBasicSchdNew(int version = 0);
	virtual ~MsgConvertBasicSchdNew();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	int m_Version;
	CONID conid;
	SCHEDULE_ID from_schd_type, to_schd_type;
	enum FLAG
	{
		CONVERT,
		ECHO_OK,
		ECHO_FAIL,
	}  flag;
};
DECLARE_SERIALIZE_OPERATOR(MsgConvertBasicSchdNew)

// 根据车次号返回多台运行线
class AFX_EXT_CLASS MsgQueryTrainByID: public BaseMsg
{
	DECLARE_CLBTH(MsgQueryTrainByID)

public:
	MsgQueryTrainByID();
	virtual ~MsgQueryTrainByID();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	TRAIN_NO  szTrainNo;//车次号
	I_TIME    tBeginTm;//
	I_TIME    tEndTm;//
};
DECLARE_SERIALIZE_OPERATOR(MsgQueryTrainByID)
/*
class AFX_EXT_CLASS MsgBasicScheduleInfoUpdate: public BaseMsg
{
DECLARE_CLBTH(MsgBasicScheduleInfoUpdate)
int m_Version;

public:
MsgBasicScheduleInfoUpdate(int version = 0);
virtual ~MsgBasicScheduleInfoUpdate();

virtual HRCODE doSerialize(Archive& ar);
virtual size_t countObjectLength();

public:
CONID       con_id;             // 区段号
short       flag;               // 1: 更新预览图版本，2：更新基本图版本，3：正在更新预览图；4：正在更新基本图
char        ver_info[256];      // 版本信息
I_TIME        set_time;           // 设置时间
USERNAME    set_name;           // 设置姓名

};
DECLARE_SERIALIZE_OPERATOR(MsgBasicScheduleInfoUpdate)

class AFX_EXT_CLASS MsgBasicScheduleInfoInform : public MsgBasicScheduleInfoUpdate
{
DECLARE_CLBTH(MsgBasicScheduleInfoInform)
int m_Version;

public:
MsgBasicScheduleInfoInform(int version = 0);
virtual ~MsgBasicScheduleInfoInform();

};
DECLARE_SERIALIZE_OPERATOR(MsgBasicScheduleInfoInform)

class AFX_EXT_CLASS MsgBasicScheduleInfoQuery: public BaseMsg
{
DECLARE_CLBTH(MsgBasicScheduleInfoQuery)
int m_Version;

public:
MsgBasicScheduleInfoQuery(int version = 0);
virtual ~MsgBasicScheduleInfoQuery();

virtual HRCODE doSerialize(Archive& ar);
virtual size_t countObjectLength();

public:
CONID   con_id;            // 区段号
short   flag;               // 1: 更新预览图版本，2：更新基本图版本
};
DECLARE_SERIALIZE_OPERATOR(MsgBasicScheduleInfoQuery)
*/

// 列车运行线客调属性
class AFX_EXT_CLASS MsgTrainKediaoInfo: public BaseMsg
{
	DECLARE_CLBTH(MsgTrainKediaoInfo)
public:
	MsgTrainKediaoInfo();
	virtual ~MsgTrainKediaoInfo();
private:
	virtual HRCODE doSerialize(Archive& ar);
	virtual size_t countObjectLength();
public:
	short	type;			// 类型 1:客调 2：特调

	short   text_len;       // 信息长度
	char*   text;           // 信息内容, 最大长度为 MAX_KEDIAO_LENGTH
	
	HRCODE SetKediao(const char* c);

};
DECLARE_SERIALIZE_OPERATOR(MsgTrainKediaoInfo)

// 列车运行线营业属性
class AFX_EXT_CLASS MsgTrainYingyeInfo: public BaseMsg
{
	DECLARE_CLBTH(MsgTrainYingyeInfo)
public:
	MsgTrainYingyeInfo();
	virtual ~MsgTrainYingyeInfo();
private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
public:
	STATION_NO	station_no;				// 站号
	I_TIME		arrive_time;			// 到达时间
	I_TIME		depart_time;			// 出发时间
	STAT_NAME	station_name;			// 站名
};
DECLARE_SERIALIZE_OPERATOR(MsgTrainYingyeInfo)

// 列车运行线乘务属性
class AFX_EXT_CLASS MsgTrainCrewInfo: public BaseMsg
{
	DECLARE_CLBTH(MsgTrainCrewInfo)
public:
	MsgTrainCrewInfo();
	virtual ~MsgTrainCrewInfo();
private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
public:
	short	crew_type;			//0:未知 1：司机	2：机械师	3：乘警	4：保洁	5：餐饮
	char	crew_info[64];		//姓名及电话
	char	crew_bureau[32];	//归属路局
};
DECLARE_SERIALIZE_OPERATOR(MsgTrainCrewInfo)

// 列车运行线机务属性
class AFX_EXT_CLASS MsgEngineCrewInfo: public BaseMsg
{
	DECLARE_CLBTH(MsgEngineCrewInfo)
public:
	MsgEngineCrewInfo();
	virtual ~MsgEngineCrewInfo();
private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
public:
	STATION_NO	station_no;				// 站号
	STAT_NAME	station_name;			// 站名
	I_TIME		overwork_time;			// 超劳时间
};
DECLARE_SERIALIZE_OPERATOR(MsgEngineCrewInfo)

// 列车运行线客票属性
class AFX_EXT_CLASS MsgTrainPassgenerTicketInfo: public BaseMsg
{
	DECLARE_CLBTH(MsgTrainPassgenerTicketInfo)
public:
	MsgTrainPassgenerTicketInfo();
	virtual ~MsgTrainPassgenerTicketInfo();
private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
public:
	short		on_station_index;		// 上车站序
	STATION_NO	on_station;				// 上车站号
	short		down_station_index;		// 下车站序
	STATION_NO  down_station;			// 下车站号
	int			sale;					// 已售张数
	char		seat[32];				// 席别
	STAT_NAME	on_station_name;		// 上车站名
	STAT_NAME	down_station_name;		// 下车站名
};
DECLARE_SERIALIZE_OPERATOR(MsgTrainPassgenerTicketInfo)

// 列车运行线OPMS信息
class AFX_EXT_CLASS MsgTrainOPMSInfo: public BaseMsg
{
	DECLARE_CLBTH(MsgTrainOPMSInfo)
public:
	MsgTrainOPMSInfo();
	virtual ~MsgTrainOPMSInfo();
private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	STATION_NO	station_no;			// 车站站号
	IDWORD	    work_flag;			// 车站作业
	IDWORD      oper_flag;			// 操作卡控
	IDWORD      important_flag;		// 重要卡控

	char	    oper_info[256];
	char        note_text[256];
};
DECLARE_SERIALIZE_OPERATOR(MsgTrainOPMSInfo)

// 列车运行线额外属性（营业、乘务、客调、客票、机务）
class AFX_EXT_CLASS MsgTrainExpand: public BaseMsg
{
	DECLARE_CLBTH(MsgTrainExpand)

public:
	MsgTrainExpand();
	virtual ~MsgTrainExpand();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	MsgSchdStamp	stamp;
	int				m_Version;			//版本
	char			chezu_no[32];		// 车组号
	TRAIN_INDEX		prev_shift_index;	//前趟车全局ID	 
	TRAIN_INDEX		next_shift_index;	//直通ID	 
	TRAIN_INDEX		prev_train_index;	//前趟车
	TRAIN_INDEX		prev_basic_index;	//前趟车基本图ID
	STATION_NO		prev_dest_sta;		//前一个方向站 
	STATION_NO		next_dest_sta;		//后一个方向站 
	IDWORD			halt_state;         //停运标志
	MsgTrainKediaoInfo*        kd_ptr;        //客调信息

	int		yingye_count;
	MsgTrainYingyeInfo*		  yy_ptr;		  //营业信息

	int		crew_count;					
	MsgTrainCrewInfo*		crew_ptr;		  //乘务信息
	
	int		engine_crew_count;					
	MsgEngineCrewInfo*	  engine_crew_ptr;		  //机务信息	

	int		ticket_count;
	MsgTrainPassgenerTicketInfo*	  ticket_ptr;		  //客票信息

	int opms_count;
	MsgTrainOPMSInfo* opms_ptr;
};
DECLARE_SERIALIZE_OPERATOR(MsgTrainExpand)


// 列车运行线限速命令信息
#define MsgTrainCmdInfo_Version 0
class AFX_EXT_CLASS MsgTrainCmdInfo: public BaseMsg
{
	DECLARE_CLBTH(MsgTrainCmdInfo)
public:
	MsgTrainCmdInfo(int version = 0);
	virtual ~MsgTrainCmdInfo();
private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	int			m_Version;			//版本
	STATION_NO	station_no			;	// 车站站号
	int		    rec_index			;	// 车站序号
	ENTITY_ID	send_entity;
	TIME		create_time;
	int			sign_status;	//0: not sent	1:sent	2:locomotive received	3:signed
	TIME		sign_time	;
	char		dcmd_id[64];
};
DECLARE_SERIALIZE_OPERATOR(MsgTrainCmdInfo)

// 列车运行线附加属性（限速命令）
#define MsgTrainExtent_Version 0
class AFX_EXT_CLASS MsgTrainExtent: public BaseMsg
{
	DECLARE_CLBTH(MsgTrainExtent)

public:
	MsgTrainExtent(int version = MsgTrainExtent_Version);
	virtual ~MsgTrainExtent();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	MsgSchdStamp	stamp;
	int				m_Version;			//版本
	
	char			chezu_no[32];		// 车组号
	TRAIN_INDEX		prev_shift_index;	//前趟车全局ID	 
	TRAIN_INDEX		next_shift_index;	//直通ID	 
	TRAIN_INDEX		prev_train_index;	//前趟车
	TRAIN_INDEX		prev_basic_index;	//前趟车基本图ID
	STATION_NO		prev_dest_sta;		//前一个方向站 
	STATION_NO		next_dest_sta;		//后一个方向站 
	IDWORD			halt_state;         //停运标志
	MsgTrainKediaoInfo*        kd_ptr;        //客调信息

	int		yingye_count;
	MsgTrainYingyeInfo*		  yy_ptr;		  //营业信息

	int		crew_count;					
	MsgTrainCrewInfo*		crew_ptr;		  //乘务信息
	
	int		engine_crew_count;					
	MsgEngineCrewInfo*	  engine_crew_ptr;		  //机务信息	

	int		ticket_count;
	MsgTrainPassgenerTicketInfo*	  ticket_ptr;		  //客票信息

	int opms_count;
	MsgTrainOPMSInfo* opms_ptr;

	int		cmd_count;
	MsgTrainCmdInfo*	  cmd_ptr;		  //限速命令信息
};
DECLARE_SERIALIZE_OPERATOR(MsgTrainExtent)

CLBTH_NAMESPACE_END

#endif
