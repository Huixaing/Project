#ifndef _CTC_DEFINES_H_
#define _CTC_DEFINES_H_

#include "private_defines.h"

CLBTH_NAMESPACE_BEGIN

#define MAX_STA_NAME_LEN        32

//--------------------------- 基本类型定义------- -----------------------------------
typedef	IDWORD		ENTITY_ID;				// 设备实体号
typedef IWORD		STATION_NO;			// 站号
typedef char		STAT_NAME[MAX_STA_NAME_LEN];			// 车站名
const ENTITY_ID		INVALID_ENTITY_ID = 0;		// 无效实体号
const STATION_NO	INVALID_STATION_NO = 0;	// 无效站号

typedef I_TIME		TIME;					// 时间
typedef IWORD		TIME_INTERVAL;		// 时间间隔( 秒)
const TIME          INVALID_TIME =(TIME) -1;    // 无效时间

typedef IBYTE		WORK_STATUS;				// 设备工作状态
const WORK_STATUS	ACTIVE		= 0;
const WORK_STATUS	INACTIVE	= 1;

const ENTITY_ID		SERVER_ENTITY=0;
//--------------------------- 码位相关---------------- --------------------------------
typedef IWORD		INDI_CHANGE;			// 码位变化
const IWORD			MAX_BITBLOCK_LEN = 1000;	// 码位全体的最大长度
const int				MAX_INDI_PER_MSG = 300;	// 一条消息中码位变化的最大个数

//--------------------------- 控制模式-------------------------------------------------

typedef IBYTE		STATION_CTRL_MODE;		// 车站控制模式
typedef IBYTE		ROUTE_CTRL_MODE;		// 进路控制模式
typedef IBYTE		STATION_CTRL_RESULT;	// 车站控制模式申请结果
typedef IBYTE		ROUTE_CTRL_RESULT;		// 进路控制模式申请结果
typedef IWORD	STATION_CTC_FLAG;		// 车站CTC特征标志
typedef IWORD	ZONE_ID;					// 分区号

const STATION_CTRL_MODE		SMODE_DISTRIBUTED	= 0;	// 分散自律
const STATION_CTRL_MODE		SMODE_CENTER_CTRL	= 1;	// 中心完全控制
const STATION_CTRL_MODE		SMODE_STATION_CTRL	= 2;	// 车站完全控制
const STATION_CTRL_MODE		SMODE_EMERGENCY		= 3;	// 非常站控
const STATION_CTRL_MODE		SMODE_EMG_CENTER_CTRL = 4;	// 紧急中心控制

const ROUTE_CTRL_MODE		RMODE_PLAN_CTRL		= 0;	// 计划控制
const ROUTE_CTRL_MODE		RMODE_SAVE_ROUTE	= 1;	// 存储进路
const ROUTE_CTRL_MODE		RMODE_FORBIT_AUTO	= 2;	// 禁止自动

const STATION_CTRL_RESULT	SREQUEST_SUCCESS	= 0;	// 成功
const STATION_CTRL_RESULT	SREQUEST_INVALID	= 1;	// 正在处理另一个申请
const STATION_CTRL_RESULT	SREQUEST_REFUSED	= 2;	// 被所有者拒绝
const STATION_CTRL_RESULT	SREQUEST_TIMEOUT	= 3;	// 超时失败
const STATION_CTRL_RESULT	SREQUEST_FORBIT	= 4;	// 禁止这样转换
const STATION_CTRL_RESULT	SREQUEST_WAIT		= 5;	// 向调度员申请,请等待
const STATION_CTRL_RESULT	SREQUEST_EQUAL	= 6;	// 申请的模式与现在相同
const STATION_CTRL_RESULT	SREQUEST_NO_RIGHT	= 7;	// 没有权限
const STATION_CTRL_RESULT	SREQUEST_NO_OWNER	= 8;	// 所有者没有连接
const STATION_CTRL_RESULT	SREQUEST_STATION_URGENT	= 9;	// 处于非常站控, 无法转换

const ROUTE_CTRL_RESULT		RREQUEST_SUCCESS	= 0;	// 成功
const ROUTE_CTRL_RESULT		RREQUEST_NO_RIGHT	= 1;	// 没有权限
const ROUTE_CTRL_RESULT		RREQUEST_FORBIT	= 2;	// 禁止这样转换
const ROUTE_CTRL_RESULT		RREQUEST_EQUAL	= 3;	// 申请的模式与现在相同

const STATION_CTC_FLAG		DEFAULT_CTC_FLAG	= 0;
const STATION_CTC_FLAG		CTC_FLAG_NO_PLAN_CTRL	= 0x0001;	// 是否使用计划控制澹(0为使用，1为不使用)

//---------------------------- 权限设置 -------------------------------
typedef IDWORD	USER_RIGHT;
const USER_RIGHT		UR_TRAIN_ROUTE	= 0x00000001;
const USER_RIGHT		UR_TRAIN_BUTTON	= 0x00000002;
const USER_RIGHT		UR_SHUNT_ROUTE	= 0x00000004;
const USER_RIGHT		UR_SHUNT_BUTTON	= 0x00000008;
const USER_RIGHT		UR_MAINTAIN		= 0x00000010;
const USER_RIGHT		UR_STA_NOPERSON	= 0x00000020;
const USER_RIGHT		UR_URGENCY_OPER = 0x00000040;

const USER_RIGHT		UR_NO_RIGHT		= 0;
/*	Reserved
const USER_RIGHT		UR_STMODE_CTRL	= 0x00000010;
const USER_RIGHT		UR_RTMODE_CTRL	= 0x00000020;
const USER_RIGHT		UR_TRAIN_NO_CTRL	= 0x00000040;
const USER_RIGHT		UR_SG_BUTTON		= 0x00000080;
const USER_RIGHT		UR_MANUAL_OPER	= 0x00000100;
*/

//------------------------ 进路相关类型定义----------------------------------------

typedef IBYTE	ROUTE_LIST_OPER_TYPE;	// 进路序列请求类型
const ROUTE_LIST_OPER_TYPE	RLIST_OPER_READ_ONLY	= 0;	// 只读
const ROUTE_LIST_OPER_TYPE	RLIST_OPER_MODIFY		= 1;	// 修改

typedef IDWORD	ROUTE_LIST_HANDLE;		// 进路序列的唯一标识
const ROUTE_LIST_HANDLE		RLIST_H_TRAIN			= 0;	// 列车进路序列
const ROUTE_LIST_HANDLE		RLIST_H_TRAIN_HISTORY	= 1;	// 列车历史进路序列
const ROUTE_LIST_HANDLE		RLIST_H_SHUNT_HISTORY	= 2;	// 调车历史进路序列
const ROUTE_LIST_HANDLE		RLIST_H_MIN_SHUNT		= 10;	// 对调车作业单, 值>10

typedef IDWORD	ROUTE_LIST_ID;			// 进路序列中一条进路的序列号(由自律机产生)
typedef IWORD	ROUTE_ID;				// 特定车站中的一条固定进路的序列号
const ROUTE_LIST_ID			NULL_ROUTE_LIST_ID	= 0;
const ROUTE_ID				NULL_ROUTE_ID			= 0;

typedef IBYTE	SHUNT_GOU_INDEX;		// 调车勾序
typedef IWORD	SHUNT_GOU_TYPE;			// 调车勾类型

typedef IBYTE	ROUTE_OPER_RESULT;		// 进路人工操作回应结果
const ROUTE_OPER_RESULT		ROPER_SUCCESS			= 0;	// 操作成功
const ROUTE_OPER_RESULT		ROPER_NO_RIGHT			= 1;	// 没有权限
const ROUTE_OPER_RESULT		ROPER_UNCHANGABLE		= 2;	// 进路序列无法更改
const ROUTE_OPER_RESULT		ROPER_LOCKED			= 3;	// 进路序列已被锁定
const ROUTE_OPER_RESULT		ROPER_TRIG_FAIL			= 4;	// 进路触发失败(与联锁逻辑冲突)
const ROUTE_OPER_RESULT		ROPER_PLAN_FAIL			= 5;	// 进路触发失败(与计划冲突)
const ROUTE_OPER_RESULT		ROPER_INVALID				= 6;	// 无效申请( 进路控制模式不符)
const ROUTE_OPER_RESULT		ROPER_ROUTE_UNKNOWN		= 7;	// 通过按钮找不到进路，消息非法
const ROUTE_OPER_RESULT		ROPER_PROG_ERROR			= 8; // 自律机程序错误
const ROUTE_OPER_RESULT		ROPER_STATION_URGENT	= 9;	// 处于非常站控
const ROUTE_OPER_RESULT		ROPER_OBSOLETE_MSG	= 10;	// 过时消息
const ROUTE_OPER_RESULT		ROPER_INVALID_MSG		= 11;	// 消息数据无效
const ROUTE_OPER_RESULT		ROPER_STDETAIL_FAIL	= 12;	// 违反站细规定

typedef IBYTE	ROUTE_REQUEST_RESULT;
const ROUTE_REQUEST_RESULT	RL_REQ_SUCCESS			= 0; // 成功
const ROUTE_REQUEST_RESULT	RL_REQ_INVALID				= 1; // 无效( 对历史表申请修改)
const ROUTE_REQUEST_RESULT	RL_REQ_IN_LOCK				= 2; // 申请修改时已有修改者
const ROUTE_REQUEST_RESULT	RL_REQ_FULL				= 3; // 申请者太多
const ROUTE_REQUEST_RESULT	RL_REQ_NO_RIGHT			= 4; // 没有权限
const ROUTE_REQUEST_RESULT	RL_REQ_WRONG_STA		= 5; // 无此站号

typedef IWORD	ROUTE_TYPE;				// 进路类型标志
										/* bit 0-2: 接车/发车/通过/调车
										bit 3-5: 进路来源(计划直接产生/计划选路得到/人工插入/人工按钮排列/删除计划遗留(可能为无效进路)
										bit 6: 基本/变更进路
										bit 7: 是否计划中是通过
*/
const IWORD	ROUTE_TRAIN_TYPE_MASK	= 0x0007;	// 进路接发车标志掩码
const IWORD	ROUTE_SOURCE_MASK		= 0x0038;	// 进路来源掩码
const IWORD	ROUTE_BASIC_TYPE_MASK	= 0x0040;	// 进路基本标志掩码
const IWORD	ROUTE_PLAN_PASS_MASK		= 0x0080;	// 进路是否计划通过掩码
const IWORD	ROUTE_EXT_TYPE_MASK		= 0x0600;	// 进路扩展类型掩码
const IWORD	ROUTE_SPEC_TYPE_MASK		= 0x1800;	// 进路特殊类型他掩码
const IWORD	ROUTE_INTERSECT_MAIN_SIDE_MASK	= 0x2000;	// 调车进路穿正标志
const IWORD	ROUTE_PASSENGER_MASK		= 0x4000;	// 客车标志掩码

typedef IBYTE	ROUTE_TRAIN_TYPE;		// 进路接发车标志(在ROUTE_TYPE中)
const ROUTE_TRAIN_TYPE		RTRAIN_ARRIVE	= 0;	// 接车
const ROUTE_TRAIN_TYPE		RTRAIN_DEPART	= 1;	// 发车
const ROUTE_TRAIN_TYPE		RTRAIN_PASS		= 2;	// 通过
const ROUTE_TRAIN_TYPE		RTRAIN_SHUNT	= 3;	// 调车
const ROUTE_TRAIN_TYPE		RTRAIN_TRANS	= 4;	// 转场

typedef IBYTE	ROUTE_SOURCE;			// 进路来源(在ROUTE_TYPE中)
const ROUTE_SOURCE			RSOURCE_PLAN		= 0;	// 计划直接产生
const ROUTE_SOURCE			RSOURCE_CHOICE		= 1;	// 计划选路得到
const ROUTE_SOURCE			RSOURCE_MANUAL		= 2;	// 人工插入
const ROUTE_SOURCE			RSOURCE_BUTTON		= 3;	// 人工按钮排列
const ROUTE_SOURCE			RSOURCE_REMOVED		= 4;	// 删除计划遗留(可能为无效进路)

typedef IBYTE	ROUTE_BASIC_TYPE;		// 进路基本标志(在ROUTE_TYPE中)
const ROUTE_BASIC_TYPE		BASIC_ROUTE			= 0;	// 基本进路
const ROUTE_BASIC_TYPE		REVISED_ROUTE		= 1;	// 变更进路

typedef IBYTE	ROUTE_EXT_TYPE;			// 进路扩展类型
const ROUTE_EXT_TYPE			REXT_NO_EXT			= 0;	// 一般进路
const ROUTE_EXT_TYPE			REXT_LONG_CONT		= 1;	// 长延续进路
const ROUTE_EXT_TYPE			REXT_LONG_TRAIN		= 2;	// 长列车进路
const ROUTE_EXT_TYPE			REXT_LONG_SHUNT		= 3;	// 长调车进路

typedef IBYTE	ROUTE_SPEC_TYPE;
const ROUTE_SPEC_TYPE		RSPEC_NONE				= 0;	// 一般进路
const ROUTE_SPEC_TYPE		RSPEC_VIRTUAL_PLAN	= 1;	// 虚拟计划进路
const ROUTE_SPEC_TYPE		RSPEC_VIRTUAL_COMP	= 2;	// 虚拟组合进路
const ROUTE_SPEC_TYPE		RSPEC_COMP				= 3;	// 组合进路

// 列控命令 状态定义
const IWORD TCC_COMMAND_STATUS_NONE       = 0;	//初始值
const IWORD TCC_COMMAND_STATUS_CONFIRM    = 1;	//已确认
const IWORD TCC_COMMAND_STATUS_EXECUTING  = 2;	//执行中
const IWORD TCC_COMMAND_STATUS_CANCELED   = 3;	//已取消
const IWORD TCC_COMMAND_STATUS_FAILED   = 4;	//已失败

inline ROUTE_TRAIN_TYPE GET_ROUTE_TRAIN_TYPE(ROUTE_TYPE nType)
{
	return (ROUTE_TRAIN_TYPE)(nType & ROUTE_TRAIN_TYPE_MASK);
}
inline void SET_ROUTE_TRAIN_TYPE(ROUTE_TRAIN_TYPE nTrainType, ROUTE_TYPE& nType)
{
	nType &= ~ROUTE_TRAIN_TYPE_MASK;
	nType |= ((IWORD)nTrainType) & ROUTE_TRAIN_TYPE_MASK;
}

inline ROUTE_SOURCE GET_ROUTE_SOURCE(ROUTE_TYPE nType)
{
	return (ROUTE_SOURCE)((nType & ROUTE_SOURCE_MASK)>>3);
}
inline void SET_ROUTE_SOURCE(ROUTE_SOURCE nSource, ROUTE_TYPE& nType)
{
	nType &= ~ROUTE_SOURCE_MASK;
	nType |= (((IWORD)nSource)<<3) & ROUTE_SOURCE_MASK;
}

inline ROUTE_BASIC_TYPE GET_ROUTE_BASIC_TYPE(ROUTE_TYPE nType)
{
	return (ROUTE_BASIC_TYPE)((nType & ROUTE_BASIC_TYPE_MASK)>>6);
}
inline void SET_ROUTE_BASIC_TYPE(ROUTE_BASIC_TYPE nBasic, ROUTE_TYPE& nType)
{
	nType &= ~ROUTE_BASIC_TYPE_MASK;
	nType |= (((IWORD)nBasic)<<6) & ROUTE_BASIC_TYPE_MASK;
}

inline bool	IS_PASS_ROUTE_IN_PLAN(ROUTE_TYPE nType)
{
	return (nType & ROUTE_PLAN_PASS_MASK) != 0;
}

inline void	SET_PLAN_PASS_ROUTE_FLAG(bool b, ROUTE_TYPE& nType)
{
	if ( b )
		nType |= ROUTE_PLAN_PASS_MASK;
	else
		nType &= ~ROUTE_PLAN_PASS_MASK;
}

inline ROUTE_EXT_TYPE GET_ROUTE_EXT_TYPE(ROUTE_TYPE nType)
{
	return (ROUTE_EXT_TYPE)((nType & ROUTE_EXT_TYPE_MASK)>>9);
}
inline void SET_ROUTE_EXT_TYPE(ROUTE_EXT_TYPE nExtType, ROUTE_TYPE& nType)
{
	nType &= ~ROUTE_EXT_TYPE_MASK;
	nType |= (((IWORD)nExtType)<<9) & ROUTE_EXT_TYPE_MASK;
}

inline ROUTE_SPEC_TYPE GET_ROUTE_SPEC_TYPE(ROUTE_TYPE nType)
{
	return (ROUTE_SPEC_TYPE)((nType & ROUTE_SPEC_TYPE_MASK)>>11);
}
inline void SET_ROUTE_SPEC_TYPE(ROUTE_SPEC_TYPE nSpecType, ROUTE_TYPE& nType)
{
	nType &= ~ROUTE_SPEC_TYPE_MASK;
	nType |= (((IWORD)nSpecType)<<11) & ROUTE_SPEC_TYPE_MASK;
}

inline bool	IS_SHUNT_INTERSECT_MAIN_SIDE(ROUTE_TYPE nType)
{
	return (nType & ROUTE_INTERSECT_MAIN_SIDE_MASK) != 0;
}
inline void	SET_INTERSECT_MAIN_SIDE_FLAG(bool b, ROUTE_TYPE& nType)
{
	if ( b )
		nType |= ROUTE_INTERSECT_MAIN_SIDE_MASK;
	else
		nType &= ~ROUTE_INTERSECT_MAIN_SIDE_MASK;
}

inline bool	IS_PASSENGER_PLAN_ROUTE(ROUTE_TYPE nType)
{
	return (nType & ROUTE_PASSENGER_MASK) != 0;
}
inline void	SET_PASSENGER_PLAN_ROUTE_FLAG(bool b, ROUTE_TYPE& nType)
{
	if ( b )
		nType |= ROUTE_PASSENGER_MASK;
	else
		nType &= ~ROUTE_PASSENGER_MASK;
}

typedef IWORD	ROUTE_STATUS;			// 进路状态标志
										/*	bit 0-2: 进路触发标志
										bit 3-7: 触发失败次数
										bit 8: 进路触发类型(自动/人工)
*/
const IWORD	ROUTE_TRIG_STATUS_MASK	= 0x000F;	// 进路触发标志掩码
const IWORD	ROUTE_FAIL_TIMES_MASK	= 0x00F0;	// 进路触发失败次数掩码
const IWORD	ROUTE_TRIG_TYPE_MASK	= 0x0100;	// 进路触发类型掩码
const IWORD	ROUTE_AUTO_SET_TRIG_TYPE_MASK	= 0x0200;	// 是否自动设置进路触发类型掩码
const IWORD	ROUTE_MANUAL_LOCK_MASK	= 0x0400;	// 进路人工更改进路标志
const IWORD	ROUTE_ALARM_MASK		= 0x8000;	// 是否已报警标志掩码
const IWORD	ROUTE_PREDICT_MASK		= 0x6000;	// 是否已发送进路预告标志掩码
const IWORD	ROUTE_DEPART_PREDICT_MASK	= 0x1800;	// 是否已发送发车预告标志掩码
const IWORD	ROUTE_SHUNTREQ_MASK	= 0x2000;	// 是否已收到调车请求
const IWORD	ROUTE_SHUNTTRIG_MASK	= 0x4000;	// 是否已准备办理调车

typedef IBYTE	ROUTE_TRIG_STATUS;		// 进路触发标志(在ROUTE_STATUS中)
const ROUTE_TRIG_STATUS		RTSTATUS_WAIT		= 0;		// 等待触发
const ROUTE_TRIG_STATUS		RTSTATUS_DOING		= 1;		// 正在进行触发
const ROUTE_TRIG_STATUS		RTSTATUS_OK			= 2;		// 触发完成
const ROUTE_TRIG_STATUS		RTSTATUS_OCCUPY		= 3;		// 进路已占用
const ROUTE_TRIG_STATUS		RTSTATUS_CLEAR		= 4;		// 进路已出清
const ROUTE_TRIG_STATUS		RTSTATUS_TIMEOUT	= 5;		// 进路触发超时
const ROUTE_TRIG_STATUS		RTSTATUS_WAIT_PLAN_CHECK	= 6;		// 正在进行计划冲突检查
const ROUTE_TRIG_STATUS		RTSTATUS_CANCEL	= 7;		// 被人工取消
const ROUTE_TRIG_STATUS		RTSTATUS_STADOWN	= 8;	// 站死

typedef IBYTE	ROUTE_TRIG_TYPE;		// 进路触发类型(在ROUTE_STATUS中)
const ROUTE_TRIG_TYPE		RTRIG_MANUAL		= 0;		// 人工触发
const ROUTE_TRIG_TYPE		RTRIG_AUTO			= 1;		// 自动触发

const IWORD	RPD_NOT_SEND	= 0;		// 未发送进路预告
const IWORD	RPD_SENDING	= 1;		// 已发送进路预告，等待回执
const IWORD	RPD_SEND_OK	= 2;		// 进路预告已收到回执
const IWORD	RPD_SEND_FAIL	= 3;		// 进路预告发送失败

const IWORD	DP_NOT_SEND	= 0;		// 未发送发车预告
const IWORD	DP_SEND		= 1;		// 已发送发车预告
const IWORD	DP_RECV		= 2;		// 发车预告已收到
const IWORD	DP_ACK			= 3;		// 预告已同意

inline ROUTE_TRIG_STATUS GET_TRIG_STATUS(ROUTE_STATUS nStatus)
{
	return (ROUTE_TRIG_STATUS)(nStatus & ROUTE_TRIG_STATUS_MASK);
}
inline void SET_TRIG_STATUS(ROUTE_TRIG_STATUS nTrigStatus, ROUTE_STATUS& nStatus)
{
	nStatus &= ~ROUTE_TRIG_STATUS_MASK;
	nStatus |= ((IWORD)nTrigStatus) & ROUTE_TRIG_STATUS_MASK;
}

inline IBYTE GET_TRIG_FAIL_TIMES(ROUTE_STATUS nStatus)
{
	return (IBYTE)((nStatus & ROUTE_FAIL_TIMES_MASK)>>4);
}
inline void SET_TRIG_FAIL_TIMES(IBYTE nTimes, ROUTE_STATUS& nStatus)
{
	nStatus &= ~ROUTE_FAIL_TIMES_MASK;
	if ( nTimes > 15 )
		nTimes = 15;
	nStatus |= (((IWORD)nTimes)<<4) & ROUTE_FAIL_TIMES_MASK;
}

inline ROUTE_TRIG_TYPE GET_ROUTE_TRIG_TYPE(ROUTE_STATUS nStatus)
{
	return (ROUTE_TRIG_TYPE)((nStatus & ROUTE_TRIG_TYPE_MASK)>>8);
}
inline void SET_ROUTE_TRIG_TYPE(ROUTE_TRIG_TYPE nType, ROUTE_STATUS& nStatus)
{
	nStatus &= ~ROUTE_TRIG_TYPE_MASK;
	nStatus |= (((IWORD)nType)<<8) & ROUTE_TRIG_TYPE_MASK;
}

inline bool IS_ROUTE_REPORTED_ALARM(ROUTE_STATUS nStatus)
{
	return (nStatus & ROUTE_ALARM_MASK) != 0;
}
inline void SET_ROUTE_ALARM_FLAG(bool b, ROUTE_STATUS& nStatus)
{
	if ( b ) 
		nStatus |= ROUTE_ALARM_MASK;
	else
		nStatus &= ~ROUTE_ALARM_MASK;
}

inline IWORD GET_ROUTE_PREDICT_FLAG(ROUTE_STATUS nStatus)
{
	return (IWORD)((nStatus & ROUTE_PREDICT_MASK)>>13);
}
inline void SET_ROUTE_PREDICT_FLAG(IWORD nFlag, ROUTE_STATUS& nStatus)
{
	nStatus &= ~ROUTE_PREDICT_MASK;
	nStatus |= (nFlag<<13) & ROUTE_PREDICT_MASK;
}

inline bool CAN_AUTO_SET_TRIG_TYPE(ROUTE_STATUS nStatus)
{
	return (nStatus & ROUTE_AUTO_SET_TRIG_TYPE_MASK) == 0;
}
inline void SET_AUTO_TRIG_TYPE_FLAG(bool b, ROUTE_STATUS& nStatus)
{
	if ( b )
		nStatus &= ~ROUTE_AUTO_SET_TRIG_TYPE_MASK;
	else
		nStatus |= ROUTE_AUTO_SET_TRIG_TYPE_MASK;
}

inline bool IS_MANUAL_LOCKED(ROUTE_STATUS nStatus)
{
	return (nStatus & ROUTE_MANUAL_LOCK_MASK) != 0;
}
inline void SET_MANUAL_LOCK_FLAG(bool b, ROUTE_STATUS& nStatus)
{
	if ( b )
		nStatus |= ROUTE_MANUAL_LOCK_MASK;
	else
		nStatus &= ~ROUTE_MANUAL_LOCK_MASK;
}

inline IWORD GET_DEPART_PREDICT_FLAG(ROUTE_STATUS nStatus)
{
	return (IWORD)((nStatus & ROUTE_DEPART_PREDICT_MASK)>>11);
}
inline void SET_DEPART_PREDICT_FLAG(IWORD nFlag, ROUTE_STATUS& nStatus)
{
	nStatus &= ~ROUTE_DEPART_PREDICT_MASK;
	nStatus |= (nFlag<<11) & ROUTE_DEPART_PREDICT_MASK;
}

inline bool IS_SHUNT_REQUEST(ROUTE_STATUS nStatus)
{
	return (nStatus & ROUTE_SHUNTREQ_MASK) != 0;
}
inline void SET_SHUNT_REQUEST_FLAG(bool b, ROUTE_STATUS& nStatus)
{
	if ( b )
		nStatus |= ROUTE_SHUNTREQ_MASK;
	else
		nStatus &= ~ROUTE_SHUNTREQ_MASK;
}

inline bool IS_SHUNT_WAIT_TRIG(ROUTE_STATUS nStatus)
{
	return (nStatus & ROUTE_SHUNTTRIG_MASK) != 0;
}
inline void SET_SHUNT_WAIT_TRIG_FLAG(bool b, ROUTE_STATUS& nStatus)
{
	if ( b )
		nStatus |= ROUTE_SHUNTTRIG_MASK;
	else
		nStatus &= ~ROUTE_SHUNTTRIG_MASK;
}


const IBYTE	MAX_ROUTE_BUTTON_COUNT = 6;		// 表示一条进路包含的按钮最多个数
const IBYTE	MAX_SIDE_COUNT_IN_ROUTE	= 8;		// 一条进路途径股道的最大数
const IBYTE	MAX_ROUTE_COUNT_IN_ONE_PLAN = 16;	// 由一条计划生成的组合进路包含的进路最大个数

//------------------------ CTC按钮操作相关-------------------------------------

typedef IDWORD	MANUAL_OPER_ID;				// 人工操作的序列号
typedef IBYTE	MANUAL_OPER_TYPE;			// 按钮、道岔、区段、信号机等的操作类型
typedef IBYTE	MANUAL_OPER_RESULT;			// 操作回应类型

const MANUAL_OPER_TYPE		ROUTE_OPER_SET		= 1;	// 人工排列进路
const MANUAL_OPER_TYPE		ROUTE_OPER_CANCEL	= 2;	// 人工取消进路
const MANUAL_OPER_TYPE		ROUTE_OPER_YINDAO	= 3;	// 人工引导进路
const MANUAL_OPER_TYPE		ROUTE_OPER_TRY_LOCK	= 4;	// 人工试排进路
const MANUAL_OPER_TYPE		ROUTE_OPER_SHACKLE  = 5;	// 进路单锁

const MANUAL_OPER_TYPE		SWITCH_OPER_LOCK		= 10;	// 道岔单锁
const MANUAL_OPER_TYPE		SWITCH_OPER_UNLOCK	= 11;	// 道岔单解
const MANUAL_OPER_TYPE		SWITCH_OPER_NORMAL	= 12;	// 道岔定操
const MANUAL_OPER_TYPE		SWITCH_OPER_REVERSE	= 13;	// 道岔反操
const MANUAL_OPER_TYPE		SWITCH_OPER_CEN_FAULT	= 14;	// 道岔心轨故障
const MANUAL_OPER_TYPE		SWITCH_OPER_TIP_FAULT	= 15;	// 道岔尖轨故障

const MANUAL_OPER_TYPE		SIGNAL_OPER_REOPEN	= 21;	// 信号机重新开放
const MANUAL_OPER_TYPE		MANUAL_OPER_CALLON_LOCK	= 22;	// 引导总锁
const MANUAL_OPER_TYPE		MANUAL_OPER_UNLOCK	= 23;	// 总人解
const MANUAL_OPER_TYPE		MANUAL_OPER_BLOCK	= 24;	// 封锁
const MANUAL_OPER_TYPE		MANUAL_OPER_UNLOCK_STA	= 25;	// 全站解锁
const MANUAL_OPER_TYPE		MANUAL_OPER_SLOPE_UNLOCK	= 26;	// 坡道解锁
const MANUAL_OPER_TYPE		MANUAL_OPER_CIRCUIT_BAD	= 27;	// 分路不良
const MANUAL_OPER_TYPE		SIGNAL_OPER_AUTOPASS		= 28;	// 自通设置
const MANUAL_OPER_TYPE		MANUAL_OPER_CIRCUIT_FAULT	= 29;	// 故障区段
const MANUAL_OPER_TYPE		MANUAL_OPER_NO_ELEC	= 30; 	// 人工停电
const MANUAL_OPER_TYPE		MANUAL_OPER_LOCO_OCCUPY	= 36; 	// 机占
const MANUAL_OPER_TYPE		SIGNAL_OPER_OPENSIG	= 37; 	// 信号机开灯
const MANUAL_OPER_TYPE		SIGNAL_OPER_CLOSESIG	= 38; 	// 信号机关灯

const MANUAL_OPER_TYPE		BUTTON_OPER_DOWN		= 31;	// 非自复按下
const MANUAL_OPER_TYPE		BUTTON_OPER_UP		= 32;	// 非自复恢复
const MANUAL_OPER_TYPE		BUTTON_OPER_PRESS	= 33;	// 自复一次操作
const MANUAL_OPER_TYPE		BUTTON_OPER_COUNT	= 34; 	// 按钮计数
const MANUAL_OPER_TYPE		BUTTON_OPER_FUNC		= 35; 	// 功能按钮

const MANUAL_OPER_RESULT	MOPER_SUCCESS		= 0;	// 操作成功(已发往联锁系统)
const MANUAL_OPER_RESULT	MOPER_INTERLOCK_FAIL	= 1;	// 与联锁冲突
const MANUAL_OPER_RESULT	MOPER_PLAN_FAIL		= 2;	// 与计划冲突
const MANUAL_OPER_RESULT	MOPER_LOCKED		= 3;	// 操作设备被锁定
const MANUAL_OPER_RESULT	MOPER_NO_RIGHT	= 4;	// 没有权限
const MANUAL_OPER_RESULT	MOPER_TIMEOUT		= 5;	// 超时
const MANUAL_OPER_RESULT	MOPER_ROUTE_UNKNOWN = 6;	// 通过按钮找不到进路，消息非法
const MANUAL_OPER_RESULT	MOPER_STATION_URGENT	= 7;	// 处于非常站控
const MANUAL_OPER_RESULT	MOPER_PROG_ERROR		= 8; // 自律机程序错误
const MANUAL_OPER_RESULT	MOPER_OBSOLETE_MSG	= 9;	// 过时消息
const MANUAL_OPER_RESULT	MOPER_INVALID_MSG	= 10;	// 消息数据无效
const MANUAL_OPER_RESULT	MOPER_STDETAIL_FAIL	= 11;	// 违反站细规定

const	IWORD	MAX_CTC_OUTPUT_HANDLE_COUNT	= 64;	// 一条CTC输出命令中的输出码位最大个数

const	short	MOPER_PARA_CBAD_COMMON		= 0;		// 分路不良参数: 通用区段, 岔前
const	short	MOPER_PARA_CBAD_SWITCH_N	= 1;		// 分路不良参数: 定位
const	short	MOPER_PARA_CBAD_SWITCH_R	= 2;		// 分路不良参数: 反位
const	short	MOPER_PARA_CBAD_ACK_IDLE	= 3;		// 分路不良参数: 确认空闲
const   short   MOPER_PARA_CBAD_SWITCH_C	= 4;		// 分路不良参数: 曲轨

inline bool	IS_CRITICAL_MANUAL_OPER( MANUAL_OPER_TYPE nType )
{
	return (	nType != ROUTE_OPER_SET && nType != MANUAL_OPER_NO_ELEC );
}

//----------------------------- 站场表示信息相关--------------------------------
typedef IBYTE		DEVICE_TYPE;		// 设备的DMIS类型
typedef IDWORD	DEVICE_STATUS;		// 设备状态
typedef short		DMIS_ID;			// 设备的DMIS号
typedef short		SIDE_ID;			// 股道号
typedef short		ENTRY_ID;			// 出入口号
typedef short		BUTTON_ID;		// 按钮号
typedef char		BUTTON_NAME[20];	// 按钮名称
typedef char		TRAIN_NO[10];		// 车次号
const IBYTE		MAX_TRAIN_NO_LEN	= 10;	// 车次号字符串的最大长度
const IWORD		MAX_DMIS_STR_LEN	= 256;	// DMIS表示信息中附加字符串的最大长度(必须不小于车次号最大长度)
const IBYTE		MAX_RESULT_COMMENT_LEN	= 200;	// 操作回应附加说明的最大长度
const DMIS_ID	INVALID_DMIS_ID	= -1;

//------------------------------ 列车计划相关--------------------------------------

typedef IDWORD	TRAIN_INDEX;
const TRAIN_INDEX	NULL_TRAIN_INDEX = 0;
typedef IDWORD	RECE_TRAIN_TYPE;
typedef short	SIDE_REGION_FLAG;

typedef IDWORD	TRAIN_KEY;

typedef IBYTE		SCHEDULE_ID;		// 基本图ID，BASIC_SCHEDULE_TYPE(1) PREV_SCHEDULE_TYPE(2) ，11--11+MAX_BASIC_ACHIVE_SCHEDULE_COUNT-1为归档基本图ID，填入SCHD_TYPE字段中
typedef IBYTE		CONID;				// GPC台号  by wuxiang at 0219
typedef IWORD		GPC_ID;
typedef IBYTE		JOB_ID;
typedef char		USERNAME[20];		// 用户名
typedef char		PASSWORD[20];		// 口令
typedef IDWORD		SHIFT_ID;

const IWORD	MAX_STPLAN_DSTN_CNT	= 100;
const IWORD	MAX_ATTACH_ENGINE_COUNT	= 4;
const IWORD  MAX_TRAIN_ID_CHG_COUNT  = 4;
const IWORD  MAX_ABS_STOP_COUNT      = 4;
const IWORD	MAX_STPLAN_DEL_CNT	= 10;
const CONID	INVALID_CONID	= 0xFF;
typedef IBYTE     ENTRY;
typedef IBYTE     SIDE;
typedef IWORD     ABS;
const ENTRY  INVALID_ENTRY = (ENTRY)-1;
const SIDE   INVALID_SIDE  = (SIDE)-1;

//const IWORD  MAX_STA_NAME_LEN   = 32;
const IWORD  MAX_TRAIN_NAME_LEN = 24;
const IWORD  MAX_BLOCK_ELE_COUNT = 512;
const IWORD  MAX_BLOCK_TEXT_LEN		  = 256;
const IWORD  MAX_TRAIN_TEXT_LENGTH    = 256;
const IWORD  MAX_COMMENT_LENGTH       = 2048;

//------------------------------调车通知单相关------------------------------------

typedef IDWORD		SHUNT_INDEX;
typedef char		SHUNT_NOTE_NAME[30];
typedef IWORD		SHUNT_NOTE_FLAG;
typedef TRAIN_NO	ENGINE_NO;

typedef char		CARRIAGE_NO[10];		// 车号
typedef short		STATION_PROP;			// 车站性质
typedef short		STATION_GRADE;			// 车站级别
typedef short		STATION_TECHNICAL;		// 车站技术性质
typedef short		STATION_OPERATION;		// 车站业务性质
typedef short		BUREAU_NO;			// 路局、分局号
typedef char		BUREAU_CODE[8];			// 路局、分局、车站代码
typedef char		BUREAU_NAME[32];		// 路局、分局名称
typedef short		SIDE_NO;			// 股道号
typedef short		SIDE_TONRANK;			// 万吨等级
typedef	char		SIDE_NAME[20];			// 股道名
typedef short		SHUNT_TIME;
typedef char		SIDE_REMARK[20];

const IWORD	MAX_SHUNTNOTE_DSTN_CNT	= 100;
const IWORD	MAX_SHUNTNOTE_STATION_CNT	= 10;
const IWORD	MAX_SHUNTNOTE_STORECAR_CNT	= 10;
const IWORD	MAX_SHUNTNOTE_STORECAR_LEN	= 10;
const IWORD	MAX_SHUNTNOTE_PICPACK_LEN	= 10;

// ----------------------------- 报警相关----------------------------------------------
typedef IBYTE	ALARM_TYPE;			// 报警消息的类型
const ALARM_TYPE	TRAIN_ALARM	= 0;	// 行车报警
const ALARM_TYPE	SYS_ALARM		= 1;	// 系统报警

typedef IWORD	ALARM_ID;		// 报警类型ID
// 自律机报警
const ALARM_ID	ALARM_LIRC_COMMON_INFO				= 0; // 一般通告消息
const ALARM_ID	ALARM_LIRC_SYS_ERROR					= 1; // 系统逻辑错误
const ALARM_ID	ALARM_LIRC_CIS_COMM_FAIL				= 2; // 微机联锁通信故障
const ALARM_ID	ALARM_LIRC_CIS_SYS_INFO				= 3; // 微机联锁系统报警
const ALARM_ID	ALARM_LIRC_DOB_SEND_FAIL				= 4; // DOB输出失败
const ALARM_ID	ALARM_LIRC_DIB_COMM_FAIL				= 5; // DIB通信故障
const ALARM_ID	ALARM_LIRC_MS_COMM_FAIL				= 6; // 网络通信故障
const ALARM_ID	ALARM_LIRC_MASTER_SWITCH			= 7; // 自律机主备切换
const ALARM_ID	ALARM_LIRC_CTRL_MODE_SWITCH			= 8; // 车站控制模式转换
const ALARM_ID	ALARM_LIRC_ROUTE_MODE_SWITCH		= 9; // 进路控制模式转换
const ALARM_ID	ALARM_LIRC_SHUNT_COLLISION			= 10; // 调车与列车计划冲突
const ALARM_ID	ALARM_LIRC_TRAIN_COLLISION			= 11; // 列车与列车计划冲突
const ALARM_ID	ALARM_LIRC_MANUAL_INTERLOCK_FAIL	= 12; // 人工办理进路时联锁冲突
const ALARM_ID	ALARM_LIRC_AUTO_INTERLOCK_FAIL		= 13; // 自动办理进路时联锁冲突
const ALARM_ID	ALARM_LIRC_SIDE_COLLISION				= 14; // 列车计划股道类型冲突
const ALARM_ID	ALARM_LIRC_TRAINPLAN_INVALID			= 15; // 列车计划信息无效
const ALARM_ID	ALARM_LIRC_TRAINROUTE_MISMATCH		= 16; // 列车进路指令表与站场实际不符
const ALARM_ID	ALARM_LIRC_INPUT_TRAINID_MISMATCH	= 17; // 列车进路指令表与人工输入的车次号不符
const ALARM_ID	ALARM_LIRC_TRAINPLAN_NOTPROC		= 18; // 列车计划无法自动处理
const ALARM_ID	ALARM_LIRC_CTC_TIMEOUT				= 19; // 设备操作超时
const ALARM_ID	ALARM_LIRC_FORCE_CREATE_ROUTE		= 20; // 人工强制办理冲突进路
const ALARM_ID	ALARM_LIRC_MANUAL_TRIG				= 21; // 自动触发转为人工触发
const ALARM_ID	ALARM_LIRC_AUTO_RETRIG				= 22; // 自动重新办理进路
const ALARM_ID	ALARM_LIRC_SHUNT_NOT_CLEAR			= 23; // 调车进路没有及时出清
const ALARM_ID	ALARM_LIRC_DUP_SHUNTNOTE			= 24; // 调车作业单号重复
const ALARM_ID	ALARM_LIRC_WIRELESS					= 25; // 无线通信报警
const ALARM_ID	ALARM_LIRC_CTC_CHECK					= 26; // 站场检测报警
const ALARM_ID	ALARM_LIRC_CENTER_DOWN				= 27; // 自律机与中心中断
const ALARM_ID	ALARM_LIRC_STA_DOWN					= 28; // 自律机与车站中断
const ALARM_ID	ALARM_LIRC_TRAIN_ROUTE_LATE			= 29; // 列车进路未及时办理
const ALARM_ID	ALARM_LIRC_LINE_COLLISION				= 30; // 列车计划线路类型冲突
const ALARM_ID	ALARM_LIRC_ELEC_MISMATCH				= 31; // 电力标志不一致
const ALARM_ID	ALARM_LIRC_STCC_INACTIVE				= 32; // 列控通信中断
const ALARM_ID	ALARM_LIRC_STCC_UNINITED				= 33; // 列控未初始化
const ALARM_ID	ALARM_LIRC_STCC_INITED				= 34; // 列控通信恢复
const ALARM_ID	ALARM_LIRC_TRAINPLAN_TIME_MISMATCH	= 35; // 计划发车顺序与预计不一致
const ALARM_ID	ALARM_LIRC_TCC_INPUT_MISMATCH		= 36; // 列控驱采不一致
const ALARM_ID	ALARM_LIRC_RES_DOWN		                     = 37; // 自律机与有源应答器监控设备中断
const ALARM_ID	ALARM_LIRC_RES_UNDESTECT		              = 38; //有源应答器未检测
const ALARM_ID	ALARM_LIRC_RES_SYS_FAIL		              = 39; //有源应答器系统故障
const ALARM_ID	ALARM_LIRC_RES_TCC_LEU_FAIL		       = 40;//列控至LEU故障
const ALARM_ID	ALARM_LIRC_RES_LEU_RES_FAIL		       = 41;//LEU至应答器故障
const ALARM_ID	ALARM_LIRC_RES_RES_CHECK_FAIL		= 42;//有源应答器限速校核错误
const ALARM_ID	ALARM_LIRC_RES_DEV_FAIL		 		= 43;//应答器监测设备故障
const ALARM_ID	ALARM_LIRC_IFS_SIGNALCLOSE		 	= 44;//信号非正常关闭
const ALARM_ID	ALARM_LIRC_TRAIN_PREDICT_NOTACK		= 45;//邻站未同意发车
const ALARM_ID	ALARM_LIRC_TRAIN_PREDICT_MISMATCH	= 46;//邻站预告信息不匹配
const ALARM_ID	ALARM_LIRC_ROUTE_SET_OK				= 47;	// 进路已办理提示
const ALARM_ID	ALARM_LIRC_ROUTE_SET_ERR				= 48;	// 进路错办报警
const ALARM_ID	ALARM_LIRC_TRAINPLAN_SYS_ERROR		= 49; // 列车计划处理系统逻辑错误

const ALARM_ID  ALARM_LIRC_TDCS3_ALARM_BASE			= 65;//TDCS3.0报警 < ALARM_LIRC_IP_ALARM_BASE 


const ALARM_ID	ALARM_LIRC_IP_ALARM_BASE				= 100; // 自律机IP报警

const ALARM_ID	ALARM_LIRC_TDCS3_EXT_ALARM_BASE			= 150;

const ALARM_ID	ALARM_TSRI_ALARM_BASE				= 10000; // TSR报警

const ALARM_ID	ALARM_CAD_ALARM_BASE				= 20000; // CAD报警
const ALARM_ID	ALARM_CAD_TASK_DEAD					= 20001; // CAD线程死
const ALARM_ID	ALARM_CAD_CONN_OK					= 20002; // CAD程序连接COMMSVR
const ALARM_ID	ALARM_CAD_DB_CONN					= 20003; // CAD程序连接数据库
const ALARM_ID	ALARM_CAD_DB_DISCONN				= 20004; // CAD线程与数据库断开
const ALARM_ID	ALARM_CAD_CONN_FAIL					= 20005; // CAD程序断开COMMSVR

const ALARM_ID	ALARM_STPC_NOT_SATISFY_ARR			= 30000; // 车务终端程序报警，不满足接车条件
const ALARM_ID	ALARM_STPC_NOT_SATISFY_DPT			= 30001; // 车务终端程序报警，不满足发车条件
const ALARM_ID	ALARM_STPC_ROUTE_MISMATCH_PLAN		= 30002; // 车务终端程序报警，接车计划不一致
const ALARM_ID	ALARM_STPC_FLOW_ERR			        = 30003; // 车务终端程序报警，接发车流程异常
const ALARM_ID	ALARM_STPC_NO_PLAN			        = 30004; // 车务终端程序报警，行车日志中无此车


const IWORD		MAX_ALARM_CONTENT_LEN	= 256;	// 报警内容最大长度

// ----------------------------- 登录相关----------------------------------------------
const IWORD		LOGIN_MAX_STATION_COUNT	= 1024;	// 登录消息中包括的站数最大值
const IWORD		LOGIN_MAX_ENTITY_COUNT	= 2048;	// 登录消息中包括的实体数最大值
const short		MAX_SCHEDULE_NAME_LEN	= 256;	// 基本图名称最大长度

typedef IBYTE	LOG_REPLY_TYPE;		// 登录回应类型
const LOG_REPLY_TYPE	LOGIN_SUCCESS	= 0;	// 登录成功
const LOG_REPLY_TYPE	LOGIN_RELOG		= 1;	// 重复登录
const LOG_REPLY_TYPE	LOGIN_INVALID	= 2;	// 登录消息非法
// ------------------------------GSMR相关消息------------------------------------------
const IBYTE     MAX_GOU_PER_SHUNT   =16;
const IBYTE     MAX_TRAIN_POSITION  =8;
const IBYTE     MAX_STATION_LIST    =8;

// -----------------------------综合维护属性-------------------------------------------
// 属性类别
const IBYTE	ATTR_RUN	= 1;	// 主备属性
const IBYTE	ATTR_CONN	= 2;	// 连接属性
const IBYTE	ATTR_CAPA	= 3;	// 容量属性
const IBYTE	ATTR_QUE	= 4;	// 动态队列属性
const IBYTE	ATTR_TIME	= 5;	// 时间属性
const IBYTE	ATTR_EVENT	= 6;	// 事件计数属性
const IBYTE	ATTR_USER	= 7;	// 用户自定义属性

// 报警类别
const IBYTE	CMSALARM_RUN		= 1;	// 主备状态异常
const IBYTE	CMSALARM_CONN		= 2;	// 连接状态异常
const IBYTE	CMSALARM_CAPA		= 3;	// 容量超限
const IBYTE	CMSALARM_TIME		= 4;	// 时间不同步
const IBYTE	CMSALARM_EVENT		= 5;	// 事件报警

//来自ctci的无线消息类别
const IWORD MSG_GSMR_TRAININFO=1; //无线车次消息
const IWORD MAX_GSMR_CONTENT_LEN=139; //消息内容最大长度
CLBTH_NAMESPACE_END

#endif

