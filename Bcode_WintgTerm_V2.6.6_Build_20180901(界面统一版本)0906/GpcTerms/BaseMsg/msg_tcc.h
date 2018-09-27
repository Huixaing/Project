#ifndef _CLBTH_MSGDEF_MSG_TCC_H_
#define _CLBTH_MSGDEF_MSG_TCC_H_

#include "ctc_defines.h"
#include "basemsg.h"

CLBTH_NAMESPACE_BEGIN

//GSM-R消息结束

// 列控中心消息定义开始
// 临时限速命令下达
class AFX_EXT_CLASS MsgStccCmdSend: public BaseMsg
{
	DECLARE_CLBTH(MsgStccCmdSend)
		
public:
	MsgStccCmdSend();
	virtual ~MsgStccCmdSend();
	
private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
public:
	enum CMD_TYPE
	{
		SPEED_LIMIT = 0,	// 临时限速命令
		SPEED_LIMIT_CANCEL,	// 取消 临时限速命令
		STATUS_REQUST,		// 临时限速状态请求
		CMD_REQUEST			// 临时限速状态请求
	};

	ENTITY_ID source;	// 发令设备
	IBYTE cmd_type;	// 命令类型
	IDWORD sys_id;		// ctc内部号, 区分取消限速命令
	IWORD cmd_no;		// 命令号(0~9999)
	ISDWORD start_tm;		// 开始时间(秒)
	ISDWORD end_tm;		// 结束时间(秒)
	IBYTE line_no;		// 线路号(具体规定)
	ISDWORD start_pos;		// 起始公里标(米)
	ISDWORD end_pos;		// 结束公里标(米)
	IBYTE speed;			// 限速值(KM/H)
	STATION_NO start_sta_no; //开始站号
	STATION_NO end_sta_no;	//结束站号
	IBYTE confirm;		// 确认次数
	
	IBYTE packet_sum;	// 本次命令包总数(1~100)
	IBYTE packet_no;		// 当前包号(1~100)
};
DECLARE_SERIALIZE_OPERATOR(MsgStccCmdSend)

// 临时限速命令执行结果
class AFX_EXT_CLASS MsgStccCmdStatus: public BaseMsg
{
	DECLARE_CLBTH(MsgStccCmdStatus)
		
public:
	MsgStccCmdStatus();
	virtual ~MsgStccCmdStatus();
	
private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
public:
	enum STATUS_TYPE
	{
		// 命令发送回执
		LIRC_RECEIVED		= 0x01,	// 自律机收到
		LIRC_SEND_FAIL		= 0x02,	// 自律机发送失败(协议帧错误)
		STCC_RECEIVED		= 0x07,	// 列控收到(协议帧正常)
		STCC_ACCEPT			= 0x08,	// 无错误

		// 命令状态
		STCC_WAITFOR_EXEC	= 0x09,	// 排队，等待执行
		STCC_EXECUTING		= 0x0a,	// 正在执行
		STCC_ZOMBIE			= 0x0b,	// 执行结束(结束时间到)
		STCC_CANCELED		= 0x0c,	// 已取消(人工或正常消亡)

		// 第三方确认状态
		STCC_CONFIRM_OK         = 0x0010,	// 被确认
		STCC_CONFIRM_CANCEL     = 0x0011,	// 被取消
		STCC_CONFIRM_TIMEOUT    = 0x0012,	// 超时，未确认

		// 有效性检测错误类型
		ERR_START_KM		= 0x20,	// 起始公里标非法
		ERR_END_KM			= 0x21,	// 结束公里标非法
		ERR_ALL_KM			= 0x22,	// 起始、结束公里标非法
		ERR_SPEED			= 0x23,	// 限速值非法
		ERR_REGION_LONG		= 0x24,	// 限速区域太长
		ERR_REGION_EXIST	= 0x25,	// 限速区域已存在
		ERR_REGION_INVALID	= 0x26,	// 限速区域非法
		ERR_DUP_CMDNO		= 0x27,	// 命令号重复
		ERR_TIME_FAIL		= 0x28,	// 时间校验失败
		ERR_UNKNOWN			= 0xff,	// 未知错误
	};

	enum ACTION_TYPE {
		COMMAND_REPLY = 0,	// 下命令的回执
		STCC_REPORT,	// 命令列表
	};

	IBYTE action_type;
	IBYTE status_type;	// 状态类型

	STATION_NO sta_no;	// 站号
	IDWORD sys_id;		// ctc内部号, 区分取消限速命令
	IWORD cmd_no;		// 命令号(0~9999)
	ISDWORD start_tm;		// 开始时间(秒)
	ISDWORD end_tm;		// 结束时间(秒)
	IBYTE line_no;		// 线路号(具体规定)
	ISDWORD start_pos;		// 起始公里标(米)
	ISDWORD end_pos;		// 结束公里标(米)
	IWORD speed;			// 限速值(KM/H)

	IBYTE packet_sum;	// 本次命令包总数(1~100)
	IBYTE packet_no;		// 当前包号(1~100)
};
DECLARE_SERIALIZE_OPERATOR(MsgStccCmdStatus)

// 列控中心状态
class AFX_EXT_CLASS MsgStccReport: public BaseMsg
{
	DECLARE_CLBTH(MsgStccReport)
		
public:
	MsgStccReport();
	virtual ~MsgStccReport();
	
private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
public:
	STATION_NO sta_no;		// 站号
	IBYTE comm_active;		// 列控通信状态	(0=断;1=单机;2=双机)
	IBYTE stcc_master[2];		// 列控主备状态
	IBYTE ilock_channel[2];	// 联锁通道状态
	IBYTE ctc_channel[2];		// CTC通道状态
	IBYTE leu_channel[4];	// LEU 通道状态
	IBYTE leu1_ans[4];	// LEU1 的应答器状态
	IBYTE leu2_ans[4];	// LEU2 的应答器状态
	IBYTE leu3_ans[4];	// LEU3 的应答器状态
	IBYTE leu4_ans[4];	// LEU4 的应答器状态
	IBYTE init_section_num;	// 初始化区间个数(最大为8)
	IBYTE init_line_no[8];		// 区间所在线路号
	IBYTE init_direction[8]; 	// 对应的线路方向
};
DECLARE_SERIALIZE_OPERATOR(MsgStccReport)

// 服务器发向STPC, GPC的确认列表
class AFX_EXT_CLASS MsgTccCommand: public BaseMsg
{
	DECLARE_CLBTH(MsgTccCommand)
public:
	MsgTccCommand();
	virtual ~MsgTccCommand();
private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	IWORD    console_no;		// 调度台 台号
	IBYTE    sys_id[16];		// 系统标识
	IBYTE    raw_sysid[16];	// 原始命令系统标识
	IWORD    cmd_no;			// 命令号（0~9999）
	IWORD    sta_no[2];		// 开始、结束站号
	IWORD    line_no;		// 线路号
	I_BOOL    bFullLimit;		// 是否全线限速
	IDWORD   km_marker[2];	// 开始、结束公里标（以m为单位）
	I_BOOL    bStartNow;		// 是否立即开始
	I_BOOL    bEndless;		// 是否永久有效
	I_TIME    time[2];		// 开始、结束时间
	IWORD    speed;			// 限速值
	I_BOOL    bCancelLimit;	// 是否 取消限速命令

	ENTITY_ID  creator;		// 原始命令草拟人
	ENTITY_ID  checkor;		// 中心校验者
	ENTITY_ID  confirmor;	// 确认分解者

	IWORD       alarm_num;   // 中心报警个数
	ENTITY_ID  alarm[4];    // 中心接受报警者

	IWORD    tcc_num;		// 受令站个数
	IWORD    tcc_no[8];		// 受令站（列控）
	IWORD    tcc_line_no[8]; // 受令站线路
	IWORD    tcc_status[8];	// 受令站命令状态
	I_TIME    tcc_time[8];	// 受令站命令生效时间
	ENTITY_ID  tcc_checkor[8]; // 受令站校验者
	ENTITY_ID  tcc_sender[8];	// 受令站发令人（车站、助调、行调）

	IWORD       tcc_alarm_num[8];   // 车站报警个数
	ENTITY_ID  tcc_alarm[8][4];    // 车站接受报警者

	IDWORD   nMarkIndex;		// 运行图区间慢行标志
	I_TIME    create_time;    // 生成时间
};
DECLARE_SERIALIZE_OPERATOR(MsgTccCommand)

// 服务器操作
class AFX_EXT_CLASS MsgTccCommandAdd: public MsgTccCommand
{
	DECLARE_CLBTH(MsgTccCommandAdd)
public:
	MsgTccCommandAdd();
	virtual ~MsgTccCommandAdd();
};
DECLARE_SERIALIZE_OPERATOR(MsgTccCommandAdd)

class AFX_EXT_CLASS MsgTccCommandDel: public BaseMsg
{
	DECLARE_CLBTH(MsgTccCommandDel)
public:
	MsgTccCommandDel();
	virtual ~MsgTccCommandDel();
private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	IWORD    console_no;		// 调度台 台号
	IBYTE    sys_id[16];		// 系统标识
	IWORD    cmd_no;			// 命令号（0~9999）

	IWORD    tcc_no;			// 受令站（0为删除相同命令号的所有受令站命令）
};
DECLARE_SERIALIZE_OPERATOR(MsgTccCommandDel)

class AFX_EXT_CLASS MsgTccCommandDump: public BaseMsg
{
	DECLARE_CLBTH(MsgTccCommandDump)
public:
	MsgTccCommandDump();
	virtual ~MsgTccCommandDump();
private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	IWORD    console_no;	// 调度台 台号
};
DECLARE_SERIALIZE_OPERATOR(MsgTccCommandDump)

class AFX_EXT_CLASS MsgTccCommandQuery: public BaseMsg
{
	DECLARE_CLBTH(MsgTccCommandQuery)
public:
	MsgTccCommandQuery();
	virtual ~MsgTccCommandQuery();
private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	IWORD    console_no;	// 调度台 台号
	IWORD    tcc_no;		// 受令站
};
DECLARE_SERIALIZE_OPERATOR(MsgTccCommandQuery)

// 临时限速命令下达
class AFX_EXT_CLASS MsgStccCommandSend: public BaseMsg
{
	DECLARE_CLBTH(MsgStccCommandSend)
public:
	MsgStccCommandSend();
	virtual ~MsgStccCommandSend();
protected:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	IWORD    console_no;		// 调度台 台号
	IBYTE    sys_id[16];		// 系统标识
	IWORD    cmd_no;			// 命令号（0~9999）
	IWORD    sta_no[2];		// 开始、结束站号
	IWORD    line_no;		// 线路号
	I_BOOL    bFullLimit;		// 是否全线限速
	IDWORD   km_marker[2];	// 开始、结束公里标（以m为单位）
	I_BOOL    bStartNow;		// 是否立即开始
	I_BOOL    bEndless;		// 是否永久有效
	I_TIME    time[2];		// 开始、结束时间
	IWORD    speed;			// 限速值

	IWORD    tcc_no;			// 受令站（列控）
	IWORD    tcc_line_no;	// 受令站线路
	ENTITY_ID  tcc_sender;	// 受令站发令人

	I_BOOL    bCancelLimit;	// 是否取消限速命令
	I_BOOL	bInitOpration;			// 是否是初始化操作
	IBYTE	section_dir;	// 区间所在相对于车站的方向（0:上行方向, 1:下行方向）
};
DECLARE_SERIALIZE_OPERATOR(MsgStccCommandSend)

// LiRC、GPC发向服务器的命令状态改变(???)
class AFX_EXT_CLASS MsgTccCommandStatusChanged: public BaseMsg
{
	DECLARE_CLBTH(MsgTccCommandStatusChanged)
public:
	MsgTccCommandStatusChanged();
	virtual ~MsgTccCommandStatusChanged();
private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	IWORD    console_no;		// 调度台 台号
	IBYTE    sys_id[16];		// 系统标识
	IWORD    cmd_no;			// 命令号（0~9999）

	IWORD    tcc_no;			// 受令站（列控）

	I_BOOL    b_status_changed;
	IWORD    tcc_status;		// 受令站命令状态(发送中、执行、失败)
	I_BOOL       b_sender_set;
	ENTITY_ID  tcc_sender;	// 受令站发令人
	I_BOOL    b_time_set;
	I_TIME    tcc_time;
};
DECLARE_SERIALIZE_OPERATOR(MsgTccCommandStatusChanged)


// LiRC对下命令操作的返回
class AFX_EXT_CLASS MsgStccCommandSendResult: public MsgStccCommandSend
{
	DECLARE_CLBTH(MsgStccCommandSendResult)
public:
	MsgStccCommandSendResult();
	virtual ~MsgStccCommandSendResult();
private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	IDWORD   result;			// 命令执行结果

	void SetComment(const char* pComment, INT len);
	void GetComment(char buf[], INT len);
private:
	IWORD    comment_length;	// 描述文字长度
	char    *strComment;	// 描述
};
DECLARE_SERIALIZE_OPERATOR(MsgStccCommandSendResult)

// 向LiRC申请实时限速列表
class AFX_EXT_CLASS MsgStccCommandListQuery: public BaseMsg
{
	DECLARE_CLBTH(MsgStccCommandListQuery)
public:
	MsgStccCommandListQuery();
	virtual ~MsgStccCommandListQuery();
private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	IWORD    tcc_no;			// 受令站（列控）
	ENTITY_ID  inquirer;	// 申请者
};
DECLARE_SERIALIZE_OPERATOR(MsgStccCommandListQuery)

// LiRC报告的实时限速列表
class AFX_EXT_CLASS MsgStccCommandList: public BaseMsg
{
	DECLARE_CLBTH(MsgStccCommandList)
public:
	MsgStccCommandList();
	virtual ~MsgStccCommandList();
private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	IWORD    console_no;		// 调度台 台号
	IBYTE    sys_id[16];		// 系统标识
	IWORD    cmd_no;			// 命令号（0~9999）
	IWORD    sta_no[2];		// 开始、结束站号
	IWORD    line_no;		// 线路号
	I_BOOL    bFullLimit;		// 是否全线限速
	IDWORD   km_marker[2];	// 开始、结束公里标（以m为单位）
	I_BOOL    bStartNow;		// 是否立即开始
	I_BOOL    bEndless;		// 是否永久有效
	IDWORD   time[2];		// 开始、结束时间
	IWORD    speed;			// 限速值

	IWORD    tcc_no;			// 受令站（列控）
	IWORD    tcc_line_no;	// 受令站线路
	ENTITY_ID  tcc_sender;	// 受令站发令人
};
DECLARE_SERIALIZE_OPERATOR(MsgStccCommandList)

// LiRC报告的实时限速列表(含设备元素列表)
class AFX_EXT_CLASS MsgStccCommandDetail: public BaseMsg
{
	DECLARE_CLBTH(MsgStccCommandDetail)
public:
	MsgStccCommandDetail();
	virtual ~MsgStccCommandDetail();
private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	IWORD    console_no;		// 调度台 台号
	IBYTE    sys_id[16];		// 系统标识
	IWORD    cmd_no;			// 命令号（0~9999）
	IWORD    sta_no[2];		// 开始、结束站号
	IWORD    line_no;		// 线路号
	I_BOOL    bFullLimit;		// 是否全线限速
	IDWORD   km_marker[2];	// 开始、结束公里标（以m为单位）
	I_BOOL    bStartNow;		// 是否立即开始
	I_BOOL    bEndless;		// 是否永久有效
	IDWORD   time[2];		// 开始、结束时间
	IWORD    speed;			// 限速值

	IWORD    tcc_no;			// 受令站（列控）
	IWORD    tcc_line_no;	// 受令站线路
	ENTITY_ID  tcc_sender;	// 受令站发令人

	I_TIME	work_time;	// 命令执行时间

	enum { MAIN_LINE_LMT, SIDE_LINE_LMT };
	IBYTE	line_type;

	enum { MAX_CMD_ELEM_COUNT = 128 };
	struct
	{
		STATION_NO	sta_no;
		DEVICE_TYPE	elem_type;
		DMIS_ID		elem_id;
	}	elements[MAX_CMD_ELEM_COUNT];
	IWORD	elem_count;
};
DECLARE_SERIALIZE_OPERATOR(MsgStccCommandDetail)

// 列控中心消息定义结束
// 车站列控中心三点检查消息
// CTC向TCC发送操作请求消息
class AFX_EXT_CLASS MsgTccThreePointCheckRequest: public BaseMsg
{
	DECLARE_CLBTH(MsgTccThreePointCheckRequest)
public:
	MsgTccThreePointCheckRequest();
	virtual ~MsgTccThreePointCheckRequest();
private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
	
public:
	IBYTE    ctc_request[20];	//三点检查CTC请求信息包
	//	IDWORD	 tcc_no;			//TCC站编号
	//	IBYTE	 command_type;		//命令类型
	//	IWORD	 deviece_id;		//闭塞分区编号
	//	IBYTE	 train_id;			//列车编号
	//	IWORD	 ctc_sender;		//发令人实体号	 
	//	IDWORD	 crc1;				//CRC32校验值1
	//	IDWORD	 crc2;				//CRC32校验值2
	IDWORD	 tcc_data_vebose;	//列控数据版本号
};
DECLARE_SERIALIZE_OPERATOR(MsgTccThreePointCheckRequest)


// TCC向CTC发送确认回执消息
class AFX_EXT_CLASS MsgTccThreePointCheckConfirm: public BaseMsg
{
	DECLARE_CLBTH(MsgTccThreePointCheckConfirm)
public:
	MsgTccThreePointCheckConfirm();
	virtual ~MsgTccThreePointCheckConfirm();
private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
	
public:
	IBYTE    tcc_confrim[20];    //tcc三点检查确认信息包
	//	IDWORD	 tcc_no;			//TCC站编号
	//	IBYTE	 command_type;		//命令类型
	//	IWORD	 deviece_id;		//闭塞分区编号
	//	IBYTE	 result_id;			//执行结果
	//	IWORD	 ctc_sender;		//发令人实体号	 
	//	IDWORD	 crc1;				//CRC32校验值1
	//	IDWORD	 crc2;				//CRC32校验值2
	IDWORD	 tcc_data_vebose;	//列控数据版本号
	
};
DECLARE_SERIALIZE_OPERATOR(MsgTccThreePointCheckConfirm)
//-----------------------------------------------------------------------------
//
//C2列控消息
//
//-----------------------------------------------------------------------------

typedef struct KM_MARK_STRUCT {
		IWORD  nKmsysCode;	//公里标系
		IWORD  k_cl;	//长短链
		IWORD  m_cl;
		IDWORD val;	//公里标0-9999 999
	} kmmark_t;
	EXTERN Archive& operator<<(Archive& ar, kmmark_t& m); 
	EXTERN Archive& operator>>(Archive& ar, kmmark_t& m); 


// 客专列控限速初始化完成
class AFX_EXT_CLASS MsgStccInitComplete: public BaseMsg
{
	DECLARE_CLBTH(MsgStccInitComplete)
public:
	MsgStccInitComplete();
	virtual ~MsgStccInitComplete();
protected:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	IWORD    console_no;		// 调度台 台号
	IBYTE    sys_id[16];		// 系统标识
	IWORD    cmd_no;			// 命令号（0~9999）
	IWORD    sta_no[2];		// 开始、结束站号
	IWORD    line_no;		// 线路号
	kmmark_t   km_marker[2];	// 开始、结束公里标（以m为单位）

	IWORD    tcc_no;			// 受令站（列控）
	IWORD    tcc_line_no;	// 受令站线路
	ENTITY_ID  tcc_sender;	// 受令站发令人

	kmmark_t   tcc_km_marker[2];	// 受令站公里标范围
};
DECLARE_SERIALIZE_OPERATOR(MsgStccInitComplete)

// 客专列控中心状态
class AFX_EXT_CLASS MsgStccReport2: public BaseMsg
{
	DECLARE_CLBTH(MsgStccReport2)
		
public:
	MsgStccReport2();
	virtual ~MsgStccReport2();
	
private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
public:
	IBYTE init_status;	//初始化状态: 未初始化, 初始化进行中, 初始化已完成
    STATION_NO sta_no;      //发送站号
    IBYTE comm_active;	// 列控通信状态	(0=断;1=单机;2=双机)
	IBYTE nStaNum;// 站数
	struct {
    		STATION_NO nStaID;// 站号
        IBYTE stby_status;       // 1: 单机， 2：双机
		IBYTE master_status;     // 1: a主 2  b: 主	
		IBYTE ilock_channel[2];  // 与联锁连接通道状态 //=10无连接 =00故障 =01正常
		IBYTE ctc_channel[2];    // 与CTC连接通道状态  //=10无连接 =00故障 =01正常 
		IBYTE switch_channel[2];  //与交换机连接通道状态 // =00故障 =03正常
		struct {
			I_BOOL bValid;	//是否有效  2: 有效  1：无效
			IBYTE leu_channel[2];// LEU 通道状态 0 故障 1 正常
			IBYTE leu_ans[4];//LEU应答器状态, 一个leu有4个应答器 0 故障 1 正常
		} leu[16];
		IBYTE nZCNum;
		struct {
	    	STATION_NO nStaID;// 中继站 站号
			IBYTE channel[2];//只有第一个主站才有 与中继站通道的连接状态
		} ZC[8];
	} sta[32];
};
DECLARE_SERIALIZE_OPERATOR(MsgStccReport2)


// 客专 LiRC报告的实时限速列表
class AFX_EXT_CLASS MsgStccCommandList2: public BaseMsg
{
	DECLARE_CLBTH(MsgStccCommandList2)
public:
	MsgStccCommandList2();
	virtual ~MsgStccCommandList2();
private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	IWORD    console_no;		// 调度台 台号
	IBYTE    sys_id[16];		// 系统标识
	IWORD    cmd_no;			// 命令号（0~9999）
	IWORD    sta_no[2];		// 开始、结束站号
	IWORD    line_no;		// 线路号
	I_BOOL    bFullLimit;		// 是否全线限速
	kmmark_t   km_marker[2];	// 开始、结束公里标（以m为单位）
	I_BOOL    bStartNow;		// 是否立即开始
	I_BOOL    bEndless;		// 是否永久有效
	IDWORD   time[2];		// 开始、结束时间
	IWORD    speed;			// 限速值
    IDWORD   set_time;               // 命令生效时刻   time_t, 0表示无法确定时间(自律机重启动)

	IWORD    tcc_no;			// 受令站（列控）
	IWORD    tcc_line_no;	// 受令站线路
	ENTITY_ID  tcc_sender;	// 受令站发令人
	kmmark_t   tcc_km_marker[2];	// 受令站公里标范围
};
DECLARE_SERIALIZE_OPERATOR(MsgStccCommandList2)


// 临时限速命令下达
class AFX_EXT_CLASS MsgStccCommandSend2: public BaseMsg
{
	DECLARE_CLBTH(MsgStccCommandSend2)
public:
	MsgStccCommandSend2();
	virtual ~MsgStccCommandSend2();
protected:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	IWORD    console_no;		// 调度台 台号
	IBYTE    sys_id[16];		// 系统标识
	IWORD    cmd_no;			// 命令号（0~9999）
	IWORD    sta_no[2];		// 开始、结束站号
	IWORD    line_no;		// 线路号
	I_BOOL    bFullLimit;		// 是否全线限速
	kmmark_t   km_marker[2];	// 开始、结束公里标（以m为单位）
	I_BOOL    bStartNow;		// 是否立即开始
	I_BOOL    bEndless;		// 是否永久有效
	I_TIME    time[2];		// 开始、结束时间
	IWORD    speed;			// 限速值

	IWORD    tcc_no;			// 受令站（列控）
	IWORD    tcc_line_no;	// 受令站线路
	ENTITY_ID  tcc_sender;	// 受令站发令人

	I_BOOL    bCancelLimit;	// 是否取消限速命令
	I_BOOL	bInitOpration;			// 是否是初始化操作
	IBYTE	section_dir;	// 区间所在相对于车站的方向（0:上行方向, 1:下行方向）

	kmmark_t   tcc_km_marker[2];	// 受令站公里标范围
	IBYTE    op;				// 操作参数 1:校验  2:执行
};
DECLARE_SERIALIZE_OPERATOR(MsgStccCommandSend2)


// 客专 服务器发向STPC, GPC的确认列表
class AFX_EXT_CLASS MsgTccCommand2: public BaseMsg
{
	DECLARE_CLBTH(MsgTccCommand2)
public:
	MsgTccCommand2();
	virtual ~MsgTccCommand2();
protected:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	IWORD    console_no;		// 调度台 台号
	IBYTE    dcmd_no[16];    // 调度命令号码
	ENTITY_ID  dcmd_entity; // 调度命令索引
	I_TIME       dcmd_time;   // 调度命令索引

	IBYTE    sys_id[16];		// 系统标识
	IBYTE    raw_sysid[16];	// 原始命令系统标识
	IWORD    cmd_no;			// 命令号（0~9999）
	IWORD    sta_no[2];		// 开始、结束站号
	IWORD    line_no;		// 线路号
	I_BOOL    bFullLimit;		// 是否全线限速
	kmmark_t   km_marker[2];	// 开始、结束公里标（以m为单位）
	I_BOOL    bStartNow;		// 是否立即开始
	I_BOOL    bEndless;		// 是否永久有效
	I_TIME    time[2];		// 开始、结束时间
	IWORD    speed;			// 限速值
	I_BOOL    bCancelLimit;	// 是否 取消限速命令

	ENTITY_ID  creator;		// 原始命令草拟人
	ENTITY_ID  checkor;		// 中心校验者
	ENTITY_ID  confirmor;	// 确认分解者

	IWORD       alarm_num;   // 中心报警个数
	ENTITY_ID  alarm[4];    // 中心接受报警者

	IWORD    tcc_num;		      // 受令站个数
	IWORD    tcc_no[128];	          // 受令站（列控）
	IWORD    tcc_line_no[128];      // 受令站线路
	IWORD    tcc_status[128];	      // 受令站命令状态
	I_TIME    tcc_time[128];	      // 受令站命令生效时间
	ENTITY_ID  tcc_checkor[128];   // 受令站校验者
	ENTITY_ID  tcc_sender[128];	  // 受令站发令人（车站、助调、行调）
	kmmark_t   tcc_km_marker[128][2]; // 受令站公里标范围

	IWORD       tcc_alarm_num[128];   // 车站报警个数
	ENTITY_ID  tcc_alarm[128][4];    // 车站接受报警者

	IDWORD   nMarkIndex;		// 运行图区间慢行标志
	I_TIME    create_time;		// 命令创建时间
};
DECLARE_SERIALIZE_OPERATOR(MsgTccCommand2)

class AFX_EXT_CLASS MsgTccCommandAdd2: public MsgTccCommand2
{
	DECLARE_CLBTH(MsgTccCommandAdd2)
public:
	MsgTccCommandAdd2();
	virtual ~MsgTccCommandAdd2();
};
DECLARE_SERIALIZE_OPERATOR(MsgTccCommandAdd2)

class AFX_EXT_CLASS MsgTccCommandData2: public MsgTccCommand2
{
	DECLARE_CLBTH(MsgTccCommandData2)
public:
	MsgTccCommandData2();
	virtual ~MsgTccCommandData2();
private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	enum QUERY_TYPE
	{
		QUERY_CURRENT,
		QUERY_HISTORY
	} type;

};
DECLARE_SERIALIZE_OPERATOR(MsgTccCommandData2)

class AFX_EXT_CLASS MsgTccCommandQuery2: public BaseMsg
{
	DECLARE_CLBTH(MsgTccCommandQuery2)
public:
	MsgTccCommandQuery2();
	virtual ~MsgTccCommandQuery2();
private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	IWORD    console_no;	  // 调度台台号
	IWORD    tcc_no;		  // 受令站
	IBYTE    dcmd_no[16];  // 调度命令号码
	
	I_TIME    time_scope[2];	//时间段范围, 匹配命令创建时间, 相等则不考虑时间, time_scope[0]==0表示所有time_scope[1]之前的, time_scope[1]==0表示所有time_cope[0]之后的

	enum QUERY_MODE 
	{
		QUERY_ALL,	//查询全部
		QUERY_BY_CONID,//按调度台id查询
		QUERY_BY_TCCNO,//按受令站查询
		QUERY_BY_DCMDID//安调度命令号查询
	} flag;

	enum QUERY_TYPE
	{
		QUERY_CURRENT,//查询当前
		QUERY_HISTORY//查询历史
	} type;
};
DECLARE_SERIALIZE_OPERATOR(MsgTccCommandQuery2)

class AFX_EXT_CLASS MsgTccCommandSet: public BaseMsg
{
	DECLARE_CLBTH(MsgTccCommandSet)
public:
	MsgTccCommandSet();
	virtual ~MsgTccCommandSet();
private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	IWORD    console_no;		// 调度台 台号
	IBYTE    sys_id[16];		// 系统标识
	IWORD    cmd_no;			// 命令号（0~9999）
	IWORD    tcc_no;			// 受令站（0为相同命令号的所有受令站命令）

	IWORD    tcc_status;         // 定义和MsgTccCommand中tcc_status相同
	I_TIME    tcc_time;	    // 受令站命令生效时间

	ENTITY_ID  tcc_checkor; // 受令站校验者
	ENTITY_ID  tcc_sender;	// 受令站发令人（车站、助调、行调）
};
DECLARE_SERIALIZE_OPERATOR(MsgTccCommandSet)

class AFX_EXT_CLASS MsgTccCommandOperEcho: public BaseMsg
{
	DECLARE_CLBTH(MsgTccCommandOperEcho)
public:
	MsgTccCommandOperEcho();
	virtual ~MsgTccCommandOperEcho();
private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	IWORD    console_no;		// 调度台 台号
	IBYTE    sys_id[16];		// 系统标识
	IWORD    cmd_no;			// 命令号（0~9999）

	IWORD code;        //  0(不返回),    1,             2,           3,          4,
	enum TccOper
	{
		ADD,          // OK，保存数据太多,  数据已存在,   命令号重复
		DEL,          // OK，数据不存在
		SET           // OK，数据不存在
	} oper;

	IWORD para;
};
DECLARE_SERIALIZE_OPERATOR(MsgTccCommandOperEcho)


// LiRC对下命令操作的返回
class AFX_EXT_CLASS MsgStccCommandSendResult2: public MsgStccCommandSend2
{
	DECLARE_CLBTH(MsgStccCommandSendResult2)
public:
	MsgStccCommandSendResult2();
	virtual ~MsgStccCommandSendResult2();
private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	IDWORD   result;			// 命令执行结果  1: 自律机收到 2:列控校验成功 3:列控校验失败 4:列控执行成功 5:列控执行失败 6:列控系统异常 7:自律机检查失败 

	void SetComment(const char* pComment, INT len);
	void GetComment(char buf[], INT len);
private:
	IWORD    comment_length;	// 描述文字长度
	char    *strComment;	// 描述
};
DECLARE_SERIALIZE_OPERATOR(MsgStccCommandSendResult2)

// LiRC报告的实时限速列表(含设备元素列表)
class AFX_EXT_CLASS MsgStccCommandDetail2: public BaseMsg
{
	DECLARE_CLBTH(MsgStccCommandDetail2)
public:
	MsgStccCommandDetail2();
	virtual ~MsgStccCommandDetail2();
private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	IWORD    console_no;		// 调度台 台号
	IBYTE    sys_id[16];		// 系统标识
	IWORD    cmd_no;			// 命令号（0~9999）
	IWORD    sta_no[2];		// 开始、结束站号
	IWORD    line_no;		// 线路号
	I_BOOL    bFullLimit;		// 是否全线限速
	kmmark_t   km_marker[2];	// 开始、结束公里标（以m为单位）
	I_BOOL    bStartNow;		// 是否立即开始
	I_BOOL    bEndless;		// 是否永久有效
	IDWORD   time[2];		// 开始、结束时间
	IWORD    speed;			// 限速值

	IWORD    tcc_no;			// 受令站（列控）
	IWORD    tcc_line_no;	// 受令站线路
	ENTITY_ID  tcc_sender;	// 受令站发令人

	I_TIME	work_time;	// 命令执行时间

	enum { MAIN_LINE_LMT, SIDE_LINE_LMT };
	IBYTE	line_type;

	enum { MAX_CMD_ELEM_COUNT = 128 };
	struct
	{
		STATION_NO	sta_no;//站号
		DEVICE_TYPE	elem_type;//设备类型
		DMIS_ID		elem_id;//设备id
	}	elements[MAX_CMD_ELEM_COUNT];
	IWORD	elem_count;
};
DECLARE_SERIALIZE_OPERATOR(MsgStccCommandDetail2)

// 列控中心消息定义结束


// 限速命令号 申请
class AFX_EXT_CLASS MsgStccCommandIDRequest: public BaseMsg
{
	DECLARE_CLBTH(MsgStccCommandIDRequest)
public:
	MsgStccCommandIDRequest();
	virtual ~MsgStccCommandIDRequest();
private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	enum {	GET_CMDID = 1,	SET_CMDID = 2,	};
	IBYTE    oper_mode;		// GET申请命令号，SET申请命令号的返回

	IWORD    console_no;		// 调度台 台号
	IWORD    cmd_no;			// 当前命令号（0~9999）
	IWORD    min_cmd_no;		// 最小命令号
	IWORD    max_cmd_no;		// 最大命令号
	IDWORD   dwCookie;		// 申请者保留的校核数据

	enum {
		TCC_CMDID_REQUEST_SUCCESS = 1,
		TCC_CMDID_REQUEST_FAIL = 2,
	};
	IWORD    nStatus;
};
DECLARE_SERIALIZE_OPERATOR(MsgStccCommandIDRequest)

//-----------------------------------------------------------------------------
//
//TSR 列控消息
//
//-----------------------------------------------------------------------------

struct tsr_cmd_t
{
	IBYTE	reason;			// 限速原因(1:施工; 2:风雨雪; 3:突发灾害)
	char		cmd_no[16];		// 命令号（0~9999）
	IWORD	sta_no;			// 站号(仅对侧线限速有效)
	IWORD	line_no;			// 线路号
	kmmark_t   km_marker[2];	// 开始、结束公里标（以m为单位）
	I_BOOL	bStartNow;		// 是否立即开始
	I_BOOL	bEndless;		// 是否永久有效
	I_TIME	time[2];			// 开始、结束时间
	IWORD	speed;			// 限速值

	size_t	countObjectLength();
};
EXTERN Archive& operator<<(Archive& ar, tsr_cmd_t& m); 
EXTERN Archive& operator>>(Archive& ar, tsr_cmd_t& m); 


// TSR限速命令发送，拟定，DCMD->TSRi
class AFX_EXT_CLASS MsgTSRCmdSend: public BaseMsg
{
	DECLARE_CLBTH(MsgTSRCmdSend)
public:
	MsgTSRCmdSend();
	virtual ~MsgTSRCmdSend();
private:
	virtual HRCODE doSerialize(Archive& ar);
	virtual size_t countObjectLength();

public:
	char	szGUID[64];
	char	dcmd_no[16];		// 调度命令号
	char	m_cContent[2048];//调度命令内容
	IWORD	m_nContentLen;//调度命令内容长度
};
DECLARE_SERIALIZE_OPERATOR(MsgTSRCmdSend)

// TSR限速命令发送状态的回执，拟定的回执，TSRi->DCMD
class AFX_EXT_CLASS MsgTSRCmdSendResult: public BaseMsg
{
	DECLARE_CLBTH(MsgTSRCmdSendResult)
public:
	MsgTSRCmdSendResult();
	virtual ~MsgTSRCmdSendResult();
private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	char			szGUID[64];//限速命令id
	tsr_cmd_t	sTsrCmd;//限速命令
	IBYTE		nResult;	//限速命令发送结果
	enum	// 状态定义
	{
		CMD_CTC_RECEIVED = 11,	// TSR通信机已收到
		CMD_CTC_SEND,			// TSR通信机已发送至TSR服务器
		CMD_TSR_CHECKOK,		// TSR服务器已校验通过
		CMD_TSR_CHECKFAIL,	// TSR服务器校验失败
		CMD_TSR_TIMEOUT,		// TSR服务器响应超时
		CMD_TSR_DEAD,			// TSR服务器无响应
		CMD_CTC_REJECTED		// TSR通信机内部错误不接收命令
	};
	char			strResult[128];	// 结果描述字符串
};
DECLARE_SERIALIZE_OPERATOR(MsgTSRCmdSendResult)

// 限速激活、设置、撤销、初始化，TSR命令的人工操作，TSRTerm->TSRi
class AFX_EXT_CLASS MsgTSRCmdOperation : public BaseMsg
{
	DECLARE_CLBTH(MsgTSRCmdOperation)
public:
	MsgTSRCmdOperation();
	virtual ~MsgTSRCmdOperation();
private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	tsr_cmd_t	sTsrCmd;	// 限速命令
	CONID		nConID;		// 区段号
	IBYTE		nOper;		// 操作类型
	enum
	{
		TSR_INIT	= 1,	// 初始化
		TSR_ACT		= 2,	// 激活
		TSR_SET		= 3,	// 设置
		TSR_REVOKE	= 4		// 撤销
	};
};
DECLARE_SERIALIZE_OPERATOR(MsgTSRCmdOperation)

// 限速操作反馈，TSR命令操作的回执，TSRi->TSRTerm
class AFX_EXT_CLASS MsgTSRCmdOperResult : public BaseMsg
{
	DECLARE_CLBTH(MsgTSRCmdOperResult)
public:
	MsgTSRCmdOperResult();
	virtual ~MsgTSRCmdOperResult();
private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	tsr_cmd_t	sTsrCmd; 	// 限速命令
	CONID		nConID;		// 区段号
	IBYTE		nOper; 		// 操作类型
	enum
	{
		TSR_INIT	= 1,	// 初始化
		TSR_ACT		= 2,	// 激活
		TSR_SET		= 3,	// 设置
		TSR_REVOKE	= 4		// 撤销
	};
	IBYTE		nResult;	// 操作结果
	enum
	{
		CMD_CTC_RECEIVED = 1,	// TSR通信机已收到
		CMD_CTC_SEND,			// TSR通信机已发送至TSR服务器
		CMD_TSR_OK,				// TSR服务器已成功处理
		CMD_TSR_FAIL,			// TSR服务器返回错误
		CMD_TSR_TIMEOUT,		// TSR服务器响应超时
		CMD_TSR_DEAD,			// TSR服务器无响应
		CMD_CTC_REJECTED		// TSR通信机内部错误不接收命令
	};
	char		strResult[128];		// 结果描述字符串	
};
DECLARE_SERIALIZE_OPERATOR(MsgTSRCmdOperResult)

// 限速提示,TSRi->TSRTerm，用于TSRTerm界面右下list显示报警提示
class AFX_EXT_CLASS MsgTSRCmdAlarm : public BaseMsg
{
	DECLARE_CLBTH(MsgTSRCmdAlarm)
public:
	MsgTSRCmdAlarm();
	virtual ~MsgTSRCmdAlarm();
private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	tsr_cmd_t	sTsrCmd;		// 限速命令
	CONID		nConID;			// 区段号
	IBYTE		nAlarmLevel;	// 提示等级
	enum
	{
		ALARM_EMERGENCY	= 1,	// 紧急提示（需明确提示人工处理）
		ALARM_NORMAL = 2		// 一般提示
	};
	IBYTE		nAlarmType;		// 提示类型
	enum
	{
		ST_ALARM_STOP		= 0,	// 停止提示
		ST_CANCEL			= 2,	// 执行取消成功
		ST_REVOKE			= 4,	// 执行撤销成功
		ST_ALL_SET			= 6,	// 执行设置成功
		ST_PART_SET			= 14,	// 提示部分执行
		ST_NOT_SET_ALARM	= 16,	// 提示超时未设置
		ST_SET_ALARM		= 18,	// 提示可设置
		ST_ACTIVE_ALARM		= 20	// 提示可激活
	};
	IWORD		nValidTime;		// 提示有效时间（单位秒）
};
DECLARE_SERIALIZE_OPERATOR(MsgTSRCmdAlarm)

// TSR限速命令实时状态，非表示，TSRi->TSRTerm，
class AFX_EXT_CLASS MsgTSRCmdStatus: public BaseMsg
{
	DECLARE_CLBTH(MsgTSRCmdStatus)
public:
	MsgTSRCmdStatus();
	virtual ~MsgTSRCmdStatus();
private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	tsr_cmd_t	sTsrCmd;	//限速命令
	CONID		nConID;	//区段号
	IBYTE		nStatus;		// TSR命令状态
	enum	// 命令状态定义
	{
		CMD_CHECKOK = 1,	// 限速已拟定
		CMD_ACTIVE,			// 限速已激活
		CMD_SETALLOK,		// 限速已执行
		CMD_CANCEL,			// 取消限速命令已激活（已执行）
		CMD_SETPARTOK		// 部分设置（已执行）
	};
	IBYTE		nDetailStatus;
	enum	// 状态详细定义
	{
		ST_PROCESS			= 0,	// 尚在处理中
		ST_CANCEL			= 2,	// 执行取消成功
		ST_REVOKE			= 4,	// 执行撤销成功
		ST_ALL_SET			= 6,	// 执行设置成功
		ST_PREPARE_CANCEL	= 8,	// 验证取消成功
		ST_PREPARE_SET		= 10,	// 验证设置成功
		ST_NOT_ACTIVE		= 12,	// 尚未激活
		ST_PART_SET			= 14,	// 提示部分执行
		ST_NOT_SET_ALARM	= 16,	// 提示超时未设置
		ST_SET_ALARM		= 18,	// 提示可设置
		ST_ACTIVE_ALARM		= 20	// 提示可激活
	};

	friend class MsgTSRCmdDisplay;
};
DECLARE_SERIALIZE_OPERATOR(MsgTSRCmdStatus)

// TSR限速命令查询,1: 请求表示LiRC->TSRi,nQueryType==QUERY_DISP    = 0x0200,	2:请求命令TSRTerm->TSRi，nQueryType==QUERY_CMD    = 0x0400   
class AFX_EXT_CLASS MsgTSRCmdQuery: public BaseMsg
{
	DECLARE_CLBTH(MsgTSRCmdQuery)
public:
	MsgTSRCmdQuery();
	virtual ~MsgTSRCmdQuery();
private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	IWORD		nQueryType;
	enum
	{
		QUERY_CHECKOK = 0x0001,	// 限速已校验通过
		QUERY_ACTIVE  = 0x0002,	// 限速已激活
		QUERY_SETOK   = 0x0004,	// 限速已设置
		QUERY_CANCEL  = 0x0008,	// 取消限速命令已激活
		QUERY_ALL     = 0x000F,	// 请求全部
		QUERY_COUNT	  = 0x0100,	// 请求限速个数
		QUERY_DISP    = 0x0200,	// 请求表示
		QUERY_CMD    = 0x0400    // 请求命令
	};
	STATION_NO	nzStations[256];
	int			nStaCount;
	CONID		nConID;			// 申请的区段号
};
DECLARE_SERIALIZE_OPERATOR(MsgTSRCmdQuery)

// TSRS状态查询，TSRTerm->TSRi，
class AFX_EXT_CLASS MsgTSRServerRequest: public BaseMsg
{
	DECLARE_CLBTH(MsgTSRServerRequest)
public:
	MsgTSRServerRequest();
	virtual ~MsgTSRServerRequest();
private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	enum { MAX_REQ_CON_COUNT = 32 };
	CONID	nzConIDs[MAX_REQ_CON_COUNT];	// 区段号列表
	IBYTE	nConCount;
};
DECLARE_SERIALIZE_OPERATOR(MsgTSRServerRequest)

// TSRS状态报告，TSRi->TSRTerm,根据MsgTSRServerRequest发送
class AFX_EXT_CLASS MsgTSRServerStatus: public BaseMsg
{
	DECLARE_CLBTH(MsgTSRServerStatus)
public:
	MsgTSRServerStatus();
	virtual ~MsgTSRServerStatus();
private:
	virtual HRCODE doSerialize(Archive& ar);

public:
	IDWORD	nTsrsID;		// TSRS编号
	CONID	nConID;			// 区段号
	IBYTE	nStatus;		// 状态
	enum
	{
		TSRS_DOWN	= 0,	// 中断
		TSRS_UNINIT,		// 未初始化
		TSRS_INIT			// 已初始化
	};
	IBYTE	nTimeFlag;		// 时间同步标志（1：TSRS与CTC同步，0：不同步）
};
DECLARE_SERIALIZE_OPERATOR(MsgTSRServerStatus)

// TSR限速命令数量，TSRi->TSRTerm,发送MsgTSRCmdStatus时，发之前和发之后发送，作为开始和结束标识
class AFX_EXT_CLASS MsgTSRCmdCount: public BaseMsg
{
	DECLARE_CLBTH(MsgTSRCmdCount)
public:
	MsgTSRCmdCount();
	virtual ~MsgTSRCmdCount();
private:
	virtual HRCODE doSerialize(Archive& ar);

public:
	CONID	nConID;	      //区段号
	IWORD	nCheckCount;  //限速命令校核数
	IWORD	nActiveCount; //限速命令激活数
	IWORD	nSetCount;    //限速命令设置数
	IWORD	nCancelCount; //限速命令取消数
	IBYTE	nReplyStatus; //限速命令回执状态
	enum { RPL_BEGIN, RPL_END }; //
};
DECLARE_SERIALIZE_OPERATOR(MsgTSRCmdCount)

// TSR限速命令实时状态表示，TSRi->LiRC,
class AFX_EXT_CLASS MsgTSRCmdDisplay: public MsgTSRCmdStatus
{
	DECLARE_CLBTH(MsgTSRCmdDisplay)
public:
	MsgTSRCmdDisplay();
	virtual ~MsgTSRCmdDisplay();
private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	enum { MAX_ELEM_COUNT = 256 };
	struct
	{
		short nStation;    //车站
		short nDmisType;   //dmis类型
		short nDmisID;     //dmis号
	}		szElems[MAX_ELEM_COUNT];
	int		nElemCount;     //元素数
	char	szComment[256]; //注释
	IBYTE	nRemoveFlag;//
};
DECLARE_SERIALIZE_OPERATOR(MsgTSRCmdDisplay)

CLBTH_NAMESPACE_END

#endif
