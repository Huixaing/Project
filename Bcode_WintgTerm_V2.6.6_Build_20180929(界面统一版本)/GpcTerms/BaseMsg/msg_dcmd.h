#ifndef _CLBTH_MSGDEF_MSG_DCMD_H_
#define _CLBTH_MSGDEF_MSG_DCMD_H_

#include "msg_tcc.h" //for import kmmark_t
#include "ctc_defines.h"
#include "basemsg.h"

CLBTH_NAMESPACE_BEGIN

class AFX_EXT_CLASS MsgDispatchCommandSend : public BaseMsg
{
	DECLARE_CLBTH(MsgDispatchCommandSend)
		
public:
	MsgDispatchCommandSend();
	virtual ~MsgDispatchCommandSend();
	
private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
public:
	
	ENTITY_ID   send_entity;   //发送实体
	ENTITY_ID   recv_entity;   //接收实体
	I_TIME        send_time;	
    I_TIME        edit_time;

	CONID		con_id;			// GPC发送时使用
	STATION_NO	source_station;	// 车站PC发送时使用
	 
	IDWORD unique_cmdindex;  //调度命令唯一识别
	short bianMa;  //命令类型编码
	short cmdNo;   //命令号码
	char cmdName[512];  //命令类型名称,可以根据类型编码来得知 // 2010.2.26 上海局调度命令标题长，导致无法发送
	char rereader[32];  //复颂人姓名
	char dispatcher[32]; //调度员姓名
	char unit[32];  //发令单位或受令单位
	char reader[32];  //阅读人姓名
 
	short       recv_count;     //发送的数目
	struct RECV_LIST{
		ENTITY_ID	recv_entity; //发送的实体
		char        chaosongMemo[64]; //抄送注释		
	}recv_list[64];
	

	short		chaosong_recv_count;     //抄送数目
	ENTITY_ID	chaosong_recv_list[128]; //抄送列表
	char        chaosongMemo[128]; //抄送注释

	short chaosong_train_list_count;    //让STPC知道抄送的列车 
	struct CHAOSONG_TRAIN_LIST{
	   TRAIN_NO    train_id; //车次，for wireless
	   TRAIN_NO    locomotive; //机车号,for wireless
	   ENTITY_ID   recv_lirc;   //接收实体
	}chaosong_train_list[32];
   
	enum CMD_TYPE {
		    DISPATCH_COMMAND = 0x01, //调度命令    
			ROUTE_TICKET,	  //路票
			GREEN_ADMISSION,     //绿色许可证
			RED_ADMISSION,       //绿色许可证
			SPY_ON_SHUNT_NOTE,   //出站跟踪调车通知单
			USE_LIGHT_CAR_ADMISSION, //轻型列车使用说明书
			FORETELL_TRAIN_ROUTE,    //列车进路预告信息
			JUST_LINE_PASS,   //正线通过，
			BY_LINE_PASS,  //侧线通过，
			JUST_LINE_SLOW_PASS,  //正线缓行通过，
			ENTER_JUST_LINE_STOP,//进站正线停车，
			ENTER_BY_LINE_STOP,  //进站侧线停车， 
			SHUNTING_JOB = 0x11, //调车作业
			SHUNTING_REQUEST_RECEIVED = 0x12, //调车请求已收到
			IN_OUT_CHECK = 0x20,      //出入库检测
			OTHER_COMMAND,    //其他命令
	}cmd_type;

	//仅用于无线调度命令
	enum   SEND_WAY {
		    SEND_INTELLIGENT,                     //智能发送
			SEND_IMMEDIATELY                      //立即发送
	}send_way;                             //   发送方式
	
	enum CMD_DESTINATION{
		GPC_2_DIRECOR_ON_DUTY,
		DIRECOR_ON_DUTY_2_GPC,			
        GPC_2_GPC,
		GPC_2_STPC,
		GPC_2_LIRC,
		GPC_2_STPC_AND_LIRC, //没有用到，实际上分两次发送的
        STPC_2_LIRC,	
		STPC_2_GPC,  //调度命令请求
	}cmd_destination;

	short ContentLength;  //长度
	char content[4000];

	TRAIN_NO    train_id; //车次，for wireless
	TRAIN_NO    locomotive; //机车号,for wireless

//列控相关
	IBYTE	bTccCmd;//
	IWORD	nCmdID;			//命令号
	IWORD    nStaID[2];		//开始、结束站
	IWORD	nTccLineID;	//线路,预留应金亮要求
	IWORD	nTccNum;//受令站个数
	struct TccLine
	{
		IWORD	nTccID;			//受令站
		IWORD	nTccLineID;		//线路
	}tccline[8];

	IBYTE	bFullLimit;		//是否全线限速
	IDWORD	nKmMark[2];		//始末公里标(以米为单位)
	IBYTE	bStartNow;		//是否立即开始
	IBYTE	bEndless;		//是否永久有效
	IDWORD	nTime[2];		//开始、结束时间

	IBYTE	bCancelLimit;	//是否取消命令

	IWORD	nTrainID;	//多少个车次
	struct {
		TRAIN_NO szTrainID;	//车次号
		IWORD	nSpeedMaxVal;	//限速值
	} train_cmd[5];
 //列控end
};

DECLARE_SERIALIZE_OPERATOR(MsgDispatchCommandSend)


class AFX_EXT_CLASS MsgDispatchCommandStatus : public BaseMsg
{
	DECLARE_CLBTH(MsgDispatchCommandStatus)
		
public:
	MsgDispatchCommandStatus();
	virtual ~MsgDispatchCommandStatus();
	
private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
public:
	
	ENTITY_ID   send_entity;
	ENTITY_ID   recv_entity;
	I_TIME        send_time;	
    I_TIME        recv_time;
	
	IDWORD unique_cmdindex;  //调度命令唯一识别
	short bianMa;  //命令类型编码
	short cmdNo;   //命令号码
 
	enum CMD_TYPE {
		    DISPATCH_COMMAND = 0x01, //调度命令    
			ROUTE_TICKET,	  //路票
			GREEN_ADMISSION,     //绿色许可证
			RED_ADMISSION,       //绿色许可证
			SPY_ON_SHUNT_NOTE,   //出站跟踪调车通知单
			USE_LIGHT_CAR_ADMISSION, //轻型列车使用说明书
			FORETELL_TRAIN_ROUTE,    //列车进路预告信息
			JUST_LINE_PASS,   //正线通过，
			BY_LINE_PASS,  //侧线通过，
			JUST_LINE_SLOW_PASS,  //正线缓行通过，
			ENTER_JUST_LINE_STOP,//进站正线停车，
			ENTER_BY_LINE_STOP,  //进站侧线停车， 
			SHUNTING_JOB = 0x11, //调车作业
			SHUNTING_REQUEST_RECEIVED = 0x12, //调车请求已收到
			IN_OUT_CHECK = 0x20,      //出入库检测
			OTHER_COMMAND,    //其他命令
	}cmd_type;
	
 
    TRAIN_NO    train_id; //车次，for wireless
	TRAIN_NO    locomotive; //机车号,for wireless
	
	enum RECV_DESTIN{
		 STATUS_GPC_DIRECOR_ON_DUTY,
		 STATUS_GPC,
	     STATUS_STPC,
		 STATUS_LIRC,
		 STATUS_LOCOMOTIVE,
	}recv_destin;
	enum RECV_STATUS {
		    COMMAND_RECV,
			COMMAND_SIGN,
			COMMAND_LOCOMOTIVE_RECV,
			COMMAND_LOCOMOTIVE_SIGN,	
			COMMAND_SENDED,            //自律机已发送
			COMMAND_ENTITY_DEAD,
			COMMAND_ERROR,
			COMMAND_DENY, //拒绝
			COMMAND_INIT,
			COMMAND_DEV_RECV, 	// 地面转接器回执
			COMMAND_AGREE = 21,			//同意
			COMMAND_DISAGREE = 23		//不同意			
    } recv_status;
	
    char    sign_name[32];
	I_TIME    sign_time;
	
	
};

DECLARE_SERIALIZE_OPERATOR(MsgDispatchCommandStatus)

class AFX_EXT_CLASS MsgDCmdTcc : public BaseMsg
{
	DECLARE_CLBTH(MsgDCmdTcc)
		
public:
	MsgDCmdTcc();
	virtual ~MsgDCmdTcc();
	
protected:
	virtual HRCODE doSerialize(Archive& ar);
	virtual size_t countObjectLength();
public:
	//这两项仅对STPC校验列控命令时有效
	IBYTE    nCmdType;
	IBYTE    nDirection;		//限速方向	1下行  2上行  3双向

	IWORD    nConsoleID;		//调度台 台号
	IBYTE    raw_sysid[16];	//原始命令标识

	IWORD	nCmdID;			//命令号
	IWORD    nStaID[2];		//开始、结束站
	IWORD	nLineID;	    //线路,预留

	I_BOOL	bFullLimit;		//是否全线限速
	kmmark_t nKmMark[2];		//始末公里标(以米为单位)

	I_BOOL	bStartNow;		//是否立即开始
	I_BOOL	bEndless;		//是否永久有效
	IDWORD	nTime[2];		//开始、结束时间
	I_BOOL	bCancelLimit;	//是否取消命令

	I_BOOL	bChecked;		//是否已经校验，不看nCheckorID
	IDWORD   nCreatorID;		//草拟者
	IDWORD	nCheckorID;		//中心校验者
	IDWORD   nConfirmorID;	//确认分解者
	IDWORD   nCreateTime;	//命令创建时间

	IWORD	nTccNum;		//受令站个数
	struct TCC 
	{
		IWORD	nTccID;		//受令站
		IWORD	nTccLineID;	//受令站线路
		IDWORD   nCheckorID;	//车站校验者(无人站为确认分解者)
		kmmark_t   nKmMark[2];	//受令站公里标范围
		I_BOOL    bNeedCheck;	//是否需要校验
	} tcc[128];

	IWORD	nTrainNum;	    //多少个车次
	struct TRAIN_CMD
	{
		TRAIN_NO szTrainID;	    //车次号
		IWORD	 nSpeedMaxVal;	//限速值, 发给列控的值
		IWORD     nSpeed;		//限速值, 人工选择的值, 低于45按45发给列控
	} train_cmd[5];
};
DECLARE_SERIALIZE_OPERATOR(MsgDCmdTcc)



class AFX_EXT_CLASS MsgDCmdTcc2 : public MsgDCmdTcc
{
	DECLARE_CLBTH(MsgDCmdTcc2)
		
public:
	MsgDCmdTcc2();
	virtual ~MsgDCmdTcc2();
	
protected:
	virtual HRCODE doSerialize(Archive& ar);
	virtual size_t countObjectLength();
public:
	IBYTE reason;	// 限速原因(1:施工; 2:风雨雪; 3:突发灾害)
	
	// 预留
	IWORD nReserveLen;
};
DECLARE_SERIALIZE_OPERATOR(MsgDCmdTcc2)



CLBTH_NAMESPACE_END

#endif
