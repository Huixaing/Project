#ifndef _CLBTH_MSGDEF_MSG_EVENT_H_
#define _CLBTH_MSGDEF_MSG_EVENT_H_

#include "ctc_defines.h"
#include "basemsg.h"

CLBTH_NAMESPACE_BEGIN


//---------------------------- 事件相关 -------------------------------
#define MAX_EVENT_DATA_LEN 40960
#define MAX_EVENT_KEY_COUNT	128

enum EVENT_ID							// 事件类型ID
{
//TDCS3.0	
	NULL_EVENT_ID,						       // 陈震解加  根据实际业务替换，替换为比如	EVENT_TRAIN_ARRVIE。 每增加新的事件类型，需修改此类型定义
	EVENT_COMMON_APP_STOP,                     //程序退出时产生          
	EVENT_COMMON_APP_START,                    //程序启动后产生          
	EVENT_COMMON_USER_LOGIN,                   //当用户登录时产生        
	EVENT_COMMON_USER_LOGOUT,                  //当用户注销时产生        
	EVENT_STPC_ACK_DEPART,                     //同意邻站发车            
	EVENT_STPC_REQ_DEPART,                     //向邻站发车请求          
	EVENT_STPC_INFORM_DEPART,                  //向邻站发车预告          
	EVENT_STPC_ACK_SIDE,                       //确认接车股道            
	EVENT_STPC_REPORT_TRAIN_ARRIVE,            //确认列车到达,人工报点   
	EVENT_STPC_REPORT_TRAIN_DEPART,            //确认列车出发,人工报点   
	EVENT_STPC_ACK_SHUNT_STOP,                 //确认调车作业停止        
	EVENT_STPC_ACK_TRAIN_INT,                  //确认列车完整            
	EVENT_STPC_SIGN_PLAN,                      //签收阶段计划            
	EVENT_STPC_RECV_PLAN,                      //收到阶段计划            
	EVENT_STPC_SIGN_PLANNOTE,                  //签收阶段记事            
	EVENT_STPC_RECV_PLANNOTE,                  //收到阶段记事            
	EVENT_STPC_SIGN_DCMD,                      //签收调度命令            
	EVENT_STPC_RECV_DCMD,                      //收到调度命令            
	EVENT_STPC_SEND_DCMD_REQ,                  //请求调度命令            
	EVENT_STPC_SEND_RADIO_DCMD,                //发送无线调度命令        
	EVENT_STPC_RECV_RADIO_DCMD_ACK,            //无线调度命令接收状态    
	EVENT_STPC_REPORT_CURRENT_CAR,             //上报现在车              
	EVENT_STPC_REPORT_TRAIN_SUBAO,             //上报列车速报            
	EVENT_LIRC_REPORT_ROUTE_STATUS,            //检测到进路排列,信号开放 
	EVENT_LIRC_REPORT_TRAIN_ENTERING,          //检测到列车压过进站信号机
	EVENT_LIRC_REPORT_TRAIN_EXITING,           //检测到列车压过出站信号机
	EVENT_LIRC_REPORT_SIDE_OCC,                //检测到股道占用          
	EVENT_LIRC_REPORT_SIDE_UNOCC,              //检测到股道出清    
	EVENT_STPC_CHOOSE_SIDE,                    //选择接车股道      added by wuchunbo 
	EVENT_STPC_AGREE_OPEN_ARSIG,               //允许开放接车信号
	EVENT_STPC_AGREE_OPEN_DPTSIG,              //允许开放发车信号
	EVENT_STPC_PREPARE_OPEN_ARSIG,             //准备开放接车信号
	EVENT_STPC_PREPARE_OPEN_DPTSIG,             //准备开放发车信号
	EVENT_STPC_TRAIN_APPROACH,					//列车接近
	EVENT_STPC_DPT_CONDITION_OK,				//发车条件满足
	EVENT_STPC_OPEN_DPTSIG_DONE,                //开放信号完毕
	EVENT_STPC_TRAIN_CONTACTED,					//接送列车
	EVENT_STPC_PREPARE_CONTACT_TRAIN,			//准备接车
	EVENT_LIRC_REPORT_TRAIN_ARRIVE,            //确认列车到达,自动报点   
	EVENT_LIRC_REPORT_TRAIN_DEPART,            //确认列车出发,自动报点  
	EVENT_LIRC_AR_ROUTE_CREATE,				   //接车进路创建
	EVENT_LIRC_AR_ROUTE_CANCEL,				   //接车进路取消
	EVENT_LIRC_PS_ROUTE_CREATE,				   //通过进路创建
	EVENT_LIRC_PS_ROUTE_CANCEL,				   //通过进路取消
	EVENT_LIRC_DP_ROUTE_CREATE,				   //发车进路创建
	EVENT_LIRC_DP_ROUTE_CANCEL,				   //发车进路取消
	EVENT_STPC_AR_ROUTE_CREATING,				   //正在办理接车进路
	EVENT_STPC_DP_ROUTE_CREATING,				   //正在办理发车进路
	EVENT_STPC_PREPARE_DPT,							//准备发车
	EVENT_STPC_ACK_SIDE_CANCEL,                       //接车股道不满足接车条件
	EVENT_LIRC_REPORT_SIDE_WHOLE_OCC,			//检测到完全股道占用
	EVENT_STPC_REPORT_TRAIN_PASS,            //列车通过,人工报点   
	EVENT_LIRC_REPORT_TRAIN_PASS,            //列车通过,自动报点 
	EVENT_COMMON_TRAIN_ALARM,				//行车报警  
	EVENT_COMMON_SYS_ALARM,					//系统报警 
	EVENT_STPC_OPEN_DPTSIG_CANCEL,           //不满足发车条件
	EVENT_STPC_CHECK_LINE,                   //检查线路
	EVENT_STPC_ST_HOME_RT,					 //准备接车进路
	EVENT_STPC_HOME_TRAIN,					 //接车
	EVENT_STPC_SET_DEPART_RT,				 //准备发车进路
	EVENT_STPC_SET_TOKEN,					//办理凭证
	EVENT_STPC_ACK_DPT_CDT,					//确认发车条件
	EVENT_STPC_ACK_DPT_TRAIN,				//发车
	EVENT_STPC_NEIB_AR,						//邻站到达
	EVENT_STPC_OP_RECORD,				   //车站人工操作事件
	EVENT_STPC_FLOW_STATUS,				   //车站流程变化事件

	//调度命令事件处理 占用预留600~799
	EVENT_DCMD_CREATE_COMMOND=600,	//调度命令创建		
	EVENT_DCMD_SAVE_COMMOND,	//调度命令缓存		
	EVENT_DCMD_SEND_COMMOND,	//调度命令下发		
	EVENT_DCMD_RECV_COMMOND,	//调度命令接受		
	EVENT_DCMD_SIGN_COMMOND,//调度命令签收		
	EVENT_DCMD_LOGIN,//调度命令登录
	EVENT_DCMD_FEEDBACK_COMMOND,//调度命令收到接受回执	
	
	//TSRTERM事件处理 占用预留800~899
	EVENT_TSRTERM_CMD_OPER=800, //限速命令操作
	EVENT_TSRTERM_CMD_RESULT,	//限速命令操作结果
	
	//自律机事件处理 占用预留900~999
	EVENT_LIRC_ROUTE_STATUS=900, //进路状态变化	
	
	//站场图事件处理 占用预留1000~1999
	EVENT_LAYOUTTERM_SM_REQUEST=1000,     //	车站控制模式申请
	EVENT_LAYOUTTERM_RM_REQUEST,     //	车站进路模式申请
	EVENT_LAYOUTTERM_TRAININDEX_OPER,//	车次号操作
	EVENT_LAYOUTTERM_TRAINSTOP,//	设置列车停稳、启动      
	EVENT_LAYOUTTERM_TRAINATTR_OPER, //	修改列车属性
	EVENT_LAYOUTTERM_MANUAL_OPER,//人工操作命令    
	EVENT_LAYOUTTERM_RBCTRAIN_OPER, //车次号操作 
	EVENT_LAYOUTTERM_STATIONCTCFLAG, //车站CTC特征标志设置		
	EVENT_LAYOUTTERM_ROUTETRIG,//进路人工触发
	EVENT_LAYOUTTERM_TRIGFLAGUPDATE,//修改自触标记
	
	//GPC事件处理 占用预留2000~2999
	EVENT_GPC_LOGIN=2000,					//运行图登陆
	EVENT_GPC_SHIFT_BANBIE,					//运行图交接班
	EVENT_GPC_MODIFY_SIDE,					//修改股道
	EVENT_GPC_SEND_STPLAN,					//下发阶段计划
	EVENT_GPC_PASS_TO_STAY,					//通过变到开
	EVENT_GPC_STAY_TO_PASS,					//到开变通过
	EVENT_GPC_STOP_ABS,						//区间停车
	EVENT_GPC_STOP_OUT_SIGNED,				//机外停车
	EVENT_GPC_CHANG_TRAINID,				//修改车次	
	EVENT_GPC_MODIFY_DEPART,				//修改发车时机
	EVENT_GPC_TRAIN_PLAN_PARA,				//修改计划线参数
	EVENT_GPC_RECV_NEIBPLAN,				//收邻台计划
	EVENT_GPC_SEND_NEIBPLAN,				//发邻台计划
	EVENT_GPC_CHANGE_TRAIN_REAL,			//转实际
	EVENT_GPC_MOUSE_REAL,					//鼠标收点
	EVENT_GPC_NEW_TRAIN,					//新增列车
	EVENT_GPC_DELETE_TRAIN,					//删除列车
	EVENT_GPC_ELE_ARM,						//修改供电臂状态
	EVENT_GPC_HELP_TRAIN,					//救援列车
	EVENT_GPC_MODIFY_ABSLINE,				//修改区间线别
	EVENT_GPC_NEW_MARK,						//新增施工
	EVENT_GPC_DELETE_MARK,					//删除施工
	EVENT_GPC_MODIFY_MARK,					//修改施工
	EVENT_GPC_TRAIN_ATTR_SPACE,				//修改列车超限
	EVENT_GPC_INHIBIT_TRAINS_MEET,			//修改列车禁会
	EVENT_GPC_MODIFY_TRAIN_TIME,			//列车时刻修改
	EVENT_GPC_ZHEFAN_TRAIN,					//修改折返车次
	EVENT_GPC_HAT_TRAIN,					//修改带帽车次
		
};

enum EVENT_KEY_ID
{
	NULL_KEY_ID,
	KeyTrainIndex,            //计划INDEX      
	KeyTrainKey,				//列车KEY                      
	KeyStationId,             //车站站号                      
	KeyConId,                 //调度区段号                        
	KeySideId,                //股道编号,和STATIONSIDE配置一致
	KeyEntryId,               //入口号                        
	KeyExitId,                //出口号                      
	KeyArriveTime,            //到达时刻                      
	KeyDepartTime,            //出发时刻                      
	KeyPhaseId,               //阶段点                        
	KeyRouteStatus,           //进路状态
	KeyShiftId,               //班别    
	KeySourceEntity,          //相关来源
	KeyDestEntity,            //相关去向
	KeyTrainNo,               //车次号
	KeyLocomotiveNo,          //机车号
	KeyUserName,              //用户真实姓名
	KeyUserId,	               //用户代码    
	KeyDcmdNo,                //调度命令号
	KeyDcmdKey,               //调度命令关键字
	KeyRecvStatus,            //接受状态
	KeySourceType,			 //来源，GPC/自律机/stpc/manual
	KeyRecindex,					
	KeyStationName,			  //站名
	KeyArrTrainNo,			  //到达车次
	KeyDptTrainNo,			  //出发车次
	KeyRouteType,             //进路类型（1:接车2:发车3:通过）
	KeyRouteFlag,				// 进路特殊标志(bit0:列车电力标志, bit1:进路电力标志, bit2:进路分路不良标志)
	KeyMinSwitch,				// 进路途径最小辙叉号	
	KeyMinDypassSwitch,		// 进路途径侧向最小辙叉号
	KeyFromStationId,             //前方车站站号                      
	KeyToStationId,             //后方车站站号                        
	KeyDelayTime,             //早晚点   
	KeyAlarmId,             //报警id
	KeyFlowId,				//流程id
	KeyFlowName,			//流程名
	KeyFlowTskId,          //任务id
	KeyFlowTskName,        //任务名
	KeyFlowTskStatus,     //任务状态
	KeyVerbose,                   //详细级别
	
	//layoutterm
	KeyZoneID,//区域号
	KeyStationMode,//站控模式0分散自律1中心完全控制2车站完全控制3非常站控4紧急中心控制
	KeyRouteMode,//进路控制模式,0计划控制;1存储进路;2禁止自动
	KeyDmisID,//dims号
	KeyIconName,//设备名称
	KeyOperType,//0添加，1修改，2删除
	KeyTrainType,//机车类型，0列车，1调车，3货车
	KeyTrainPos,//0站内，1区间
	KeyWindowID,//车次窗id
	KeyWindowName,//车次窗名称
	KeyStopInfo,//停稳标记，1启动,2停稳，
	KeyTrainRate,//列车速率
	KeyManualOperID,//人工操作序号
	KeyManualOperType,//人工操作类型, 参加ctc_defines.h中MANUAL_OPER_TYPE类型定义
	KeyDeviceType,//设备类型
	KeyOperTime,//操作时间
	KeyOperPara,//操作参数
	KeyForceFlag,//1:强制执行(仅进行联锁逻辑检查); 0:需进行站细等检查,
	KeyUniqueID,//RBC提供的列车唯一识别号，
	KeyRBCOper,//操作类型，0查询，1删除
	KeyCTCFlag ,//0计划控制，1非计划控制
	KeyRouteHandle,//进路句柄DWORD类型
	KeyListID,//进路序列号DWORD类型
	KeyTrigTime,//触发时间
	KeyRouteForceFlag,//是否强制触发 0:强制， 1：不强制
	KeyTrigFlag,//触发标记(0:人工触发, 1:自动触发)

	//tsrterm
	KeyTsrCmdNo,//tsr命令号    
	KeyTsrCmdOperID,//操作类型,初始化1,激活2,设置3,撤销4
	KeyReason,// 限速原因(1:施工; 2:风雨雪; 3:突发灾害);
	KeyTsrNum,//TSR命令号
	KeyTsrLineNo,//Tsr线路号
	KeyStartNow,//是否立即开始
	KeyEndless,//是否永久有效
	KeyStartTime,//
	KeyEndTime,//
	KeySpeed,//限速值
	KeyTsrResult,//1:TSR通信机已收到,2：TSR通信机已发送至TSR服务器,3：TSR服务器已成功处理,4：TSR服务器返回错误,5：TSR服务器响应超时,6：TSR服务器无响应,7：TSR通信机内部错误不接收命令
	KeyKmsysCode,//公里标系
	KeyKCL,//长短链
	KeyKmValue,//公里标值
	//dcmd
	KeyDcmdID,//调度命令唯一id
	KeyDcmdCreateTime,//创建时间
	KeyDcmdCreateEntityID,//创建实体号
	KeyDcmdType,//调度命令类型 0普通命令，1列控命令，2施工命令，3取消命令。
	KeySaveType,//调度命令保存类型 1只存储本地,2只存储数据库，3存储本地和数据库
	KeyConName,//调度区段名称
	KeyDcmdSendResult,//发送结果
	KeySendTime,//发送时间
	KeyReceiveTime,//接收时间
	KeySourceEntityID,//源实体号
	KeySourceName,//源名称
	KeySignTime,//签收时间
	KeySignName,//签收人名称
	KeySignEntityID,//签收实体号
	KeySignResult,//签收结果1签收，2拒签，3代签
	KeyFeedBackEntityID,//回执来源实体号
	KeyFeedBackName,//回执来源名称
	KeyFeedBackType,//发送回执消息的终端类型，0车站，1调度台，2值班主任，3机车，4接口机。
	KeyFeedBackResult,//0接收，1签收，2拒签   
		
	KeyLoginTime,				//登陆时间
	KeyOrlSideId,				//原股道
	KeyOrlBusinessTime,			//原营业时间
	KeyBusinessTime,			//营业时间
	KeyOrlLineNo,				//原区间线别
	KeyLineNo,					//区间线别
	KeyLocoStopStation,			//机外停车车站
	KeyOldTrainNo,				//原列车车次号
	KeyNeibConId,				//邻台调度区间
	KeyEleArm,					//电力臂描述
	KeyEleArmStatus,			//电力臂有无电状态
	KeyHelpTrain,				//救援列车
	KeyOrlDepartMoment,			//原发车时机
	KeyDepartMoment,			//发车时机
	KeyMarkType,				//施工类型
	KeyMarkStartTime,			//施工起始时间
	KeyMarkEndTime,				//施工结束时间
	KeyMarkAbsNo,				//封锁区间
	KeyMarkSideNo,				//封锁股道
	KeyTrainSpaceInfo,			//超限信息
	KeyTrainMeetInfo,			//禁会信息
	KeyRankType,				//列车特殊属性 1、超限 2、超长 3、重点 4、军运
	KeyArrOrlSideId,
	KeyDptOrlSideId,
	KeyArrSideId,
	KeyDptSideId,

	KeyRouteAttribute,			//进路类型，参见msg_def.h中ROUTE_ATTRIBUTE类型定义
	KeyRouteTrigStatus,			//进路触发状态，参见ctc_defines.h中ROUTE_TRIG_STATUS类型定义
};

enum EVENT_RANK
{
	IMPORTANT_EVENT,
	NORMAL_EVENT,
	MISC_EVENT
};

#define EXTERN_EVENT_DEFINE_INT(name)       extern void EventSet##name(MsgEventData&, int );           extern int EventGet##name(MsgEventData& );
#define EXTERN_EVENT_DEFINE_STR(name)       extern void EventSet##name(MsgEventData&, const char* );   extern const char* EventGet##name(MsgEventData& );

struct EVENT_KEY_DATA					// 关键字
{
	EVENT_KEY_ID    sKeyId;		        //  事件关键字
	char			sKeyWord[64];		// 事件关键字内容
};

// 事件注册消息
class AFX_EXT_CLASS MsgEventReg : public BaseMsg
{
	DECLARE_CLBTH(MsgEventReg)

public:
	MsgEventReg();
	virtual ~MsgEventReg();
	
private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
	
public:
	ENTITY_ID		nSrcEntityID;		// 事件源实体号
	EVENT_ID		nEventID;			// 事件类型ID
	enum ReqStatus
	{
		LOAD,
		CLOSE
	};
	ReqStatus  nStatus;					// 注册或关闭注册
};
DECLARE_SERIALIZE_OPERATOR(MsgEventReg)

// 事件内容消息
class AFX_EXT_CLASS MsgEventData : public BaseMsg
{
	DECLARE_CLBTH(MsgEventData)
		
public:
	MsgEventData();
	MsgEventData(EVENT_ID e);
	virtual ~MsgEventData();
	
private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
	
public:
	EVENT_ID		nEventID;			// 事件类型ID

	ENTITY_ID		nSrcEntityID;		// 事件源实体号
	char			sSrcName[32];		// 事件源名称
	I_TIME			tmEventTime;		// 事件发生时间
	char			sEventName[32];		// 事件类型名称
	int				nEventIdx;			// 事件序号
	int             nDataLen;
	char			*nzDataBuffer;		// 事件数据内容	
	EVENT_RANK		nEventRank	;		// 事件级别
	short			nKeyCount	;		// 事件关键字个数
	IBYTE			nKeySpace	;		// 事件关键字分配大小
	char			sEventDesc[256];	// 事件描述

	EVENT_KEY_DATA	*struKey;		// 事件关键字

	I_BOOL SetData(const char* buffer);
	const char* GetData() const;

	I_BOOL IsKeyHeld(const EVENT_KEY_DATA ek) const;
	short SetKey(EVENT_KEY_ID key, const char* ek);		//成功返回现有KEY个数，失败-0
	short SetKey(EVENT_KEY_ID key, int ek);		//成功返回现有KEY个数，失败-0
	const char* GetKey(EVENT_KEY_ID key) const;	//成功返回关键字值， 失败返回NULL
	const char* GetEventName();	//成功返回事件名称， 失败返回NULL
	void SetEventName();//设置事件名 by envent id.
};
DECLARE_SERIALIZE_OPERATOR(MsgEventData)

// 事件查询消息
class AFX_EXT_CLASS MsgEventReq : public BaseMsg
{
	DECLARE_CLBTH(MsgEventReq)
		
public:
	MsgEventReq();
	virtual ~MsgEventReq();
	short SetKey(EVENT_KEY_ID key, const char* ek);		//成功返回现有KEY个数，失败-0
	short SetKey(EVENT_KEY_ID key, int ek);		//成功返回现有KEY个数，失败-0
	
private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
	
public:
	ENTITY_ID		nSrcEntityID;		// 事件源实体号
	EVENT_ID		nEventID;			// 事件类型ID
	I_TIME			nStartTime;			// 查询时间范围开始
	I_TIME			nEndTime;			// 查询时间范围结束
	EVENT_RANK		nEventRank	;		// 事件级别(返回同级别或高级别的）
	IBYTE			nKeyCount	;		// 事件关键字个数
	IBYTE			nKeySpace	;		// 事件关键字分配大小
	EVENT_KEY_DATA	*struKey;		// 事件关键字
};
DECLARE_SERIALIZE_OPERATOR(MsgEventReq)


EXTERN_EVENT_DEFINE_INT(KeyTrainIndex);
EXTERN_EVENT_DEFINE_INT(KeyStationId);
EXTERN_EVENT_DEFINE_INT(KeyConId);
EXTERN_EVENT_DEFINE_INT(KeySideId);
EXTERN_EVENT_DEFINE_INT(KeyEntryId);
EXTERN_EVENT_DEFINE_INT(KeyExitId);
EXTERN_EVENT_DEFINE_INT(KeyArriveTime);
EXTERN_EVENT_DEFINE_INT(KeyDepartTime);
EXTERN_EVENT_DEFINE_INT(KeyPhaseId);
EXTERN_EVENT_DEFINE_INT(KeyRouteStatus);
EXTERN_EVENT_DEFINE_INT(KeyShiftId);
EXTERN_EVENT_DEFINE_INT(KeySourceEntity);
EXTERN_EVENT_DEFINE_INT(KeyDestEntity);
EXTERN_EVENT_DEFINE_INT(KeySourceType);
EXTERN_EVENT_DEFINE_INT(KeyRecindex);
//GPC
EXTERN_EVENT_DEFINE_INT(KeyLoginTime);
EXTERN_EVENT_DEFINE_INT(KeyOrlSideId);
EXTERN_EVENT_DEFINE_INT(KeyOrlBusinessTime);
EXTERN_EVENT_DEFINE_INT(KeyBusinessTime);
EXTERN_EVENT_DEFINE_INT(KeyLocoStopStation);
EXTERN_EVENT_DEFINE_INT(KeyRouteFlag);
EXTERN_EVENT_DEFINE_INT(KeyEleArmStatus);
EXTERN_EVENT_DEFINE_INT(KeyNeibConId);
EXTERN_EVENT_DEFINE_INT(KeyOrlLineNo);
EXTERN_EVENT_DEFINE_INT(KeyLineNo);
EXTERN_EVENT_DEFINE_INT(KeyFromStationId);
EXTERN_EVENT_DEFINE_INT(KeyToStationId);

EXTERN_EVENT_DEFINE_STR(KeyTrainNo);
EXTERN_EVENT_DEFINE_STR(KeyLocomotiveNo);
EXTERN_EVENT_DEFINE_STR(KeyUserName);
EXTERN_EVENT_DEFINE_STR(KeyUserId);	
EXTERN_EVENT_DEFINE_STR(KeyDcmdNo);
EXTERN_EVENT_DEFINE_STR(KeyDcmdKey);
EXTERN_EVENT_DEFINE_STR(KeyRecvStatus);
EXTERN_EVENT_DEFINE_STR(KeyStationName);
EXTERN_EVENT_DEFINE_STR(KeyArrTrainNo);
EXTERN_EVENT_DEFINE_STR(KeyDptTrainNo);
//GPC
EXTERN_EVENT_DEFINE_STR(KeyHelpTrain);
EXTERN_EVENT_DEFINE_STR(KeyOrlTrainNo);
EXTERN_EVENT_DEFINE_STR(KeyEleArm);
EXTERN_EVENT_DEFINE_STR(KeyOrlDepartMoment);
EXTERN_EVENT_DEFINE_STR(KeyDepartMoment);
EXTERN_EVENT_DEFINE_STR(KeyMarkType);
EXTERN_EVENT_DEFINE_STR(KeyTrainSpaceInfo);
EXTERN_EVENT_DEFINE_STR(KeyTrainMeetInfo);
EXTERN_EVENT_DEFINE_STR(KeyMarkStartTime);
EXTERN_EVENT_DEFINE_STR(KeyMarkEndTime);
EXTERN_EVENT_DEFINE_STR(KeyMarkAbsNo);
EXTERN_EVENT_DEFINE_STR(KeyMarkSideNo);


CLBTH_NAMESPACE_END

#endif
