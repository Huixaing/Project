#define EXTERN_LINKAGE_DEFINE
#include <stdio.h>
#include "msg_event.h"

CLBTH_NAMESPACE_BEGIN

#define IMPLEMENT_EVENT_DEFINE_INT(name)              \
void EventSet##name(MsgEventData& msg, int data)      \
{                                                     \
	msg.SetKey(name, data);                         \
}                                                     \
int EventGet##name(MsgEventData& msg)                 \
{                                                     \
	return atol(msg.GetKey(name));                  \
}


#define IMPLEMENT_EVENT_DEFINE_STR(name)                   \
void EventSet##name(MsgEventData& msg, const char* data)   \
{                                                          \
	msg.SetKey(name, data);                              \
}                                                          \
const char* EventGet##name(MsgEventData& msg)              \
{                                                          \
	return msg.GetKey(name);                             \
}
	                              

IMPLEMENT_EVENT_DEFINE_INT(KeyTrainIndex);
IMPLEMENT_EVENT_DEFINE_INT(KeyTrainKey);
IMPLEMENT_EVENT_DEFINE_INT(KeyStationId);
IMPLEMENT_EVENT_DEFINE_INT(KeyConId);
IMPLEMENT_EVENT_DEFINE_INT(KeySideId);
IMPLEMENT_EVENT_DEFINE_INT(KeyEntryId);
IMPLEMENT_EVENT_DEFINE_INT(KeyExitId);
IMPLEMENT_EVENT_DEFINE_INT(KeyArriveTime);
IMPLEMENT_EVENT_DEFINE_INT(KeyDepartTime);
IMPLEMENT_EVENT_DEFINE_INT(KeyPhaseId);
IMPLEMENT_EVENT_DEFINE_INT(KeyRouteStatus);
IMPLEMENT_EVENT_DEFINE_INT(KeyShiftId);
IMPLEMENT_EVENT_DEFINE_INT(KeySourceEntity);
IMPLEMENT_EVENT_DEFINE_INT(KeyDestEntity);
IMPLEMENT_EVENT_DEFINE_INT(KeySourceType);
IMPLEMENT_EVENT_DEFINE_INT(KeyRecindex);
//GPC
IMPLEMENT_EVENT_DEFINE_INT(KeyLoginTime);
IMPLEMENT_EVENT_DEFINE_INT(KeyOrlSideId);
IMPLEMENT_EVENT_DEFINE_INT(KeyOrlBusinessTime);
IMPLEMENT_EVENT_DEFINE_INT(KeyBusinessTime);
IMPLEMENT_EVENT_DEFINE_INT(KeyLocoStopStation);
IMPLEMENT_EVENT_DEFINE_INT(KeyRouteFlag);
IMPLEMENT_EVENT_DEFINE_INT(KeyEleArmStatus);
IMPLEMENT_EVENT_DEFINE_INT(KeyNeibConId);
IMPLEMENT_EVENT_DEFINE_INT(KeyOrlLineNo);
IMPLEMENT_EVENT_DEFINE_INT(KeyLineNo);
IMPLEMENT_EVENT_DEFINE_INT(KeyFromStationId);
IMPLEMENT_EVENT_DEFINE_INT(KeyToStationId);


IMPLEMENT_EVENT_DEFINE_STR(KeyTrainNo);
IMPLEMENT_EVENT_DEFINE_STR(KeyLocomotiveNo);
IMPLEMENT_EVENT_DEFINE_STR(KeyUserName);
IMPLEMENT_EVENT_DEFINE_STR(KeyUserId);	
IMPLEMENT_EVENT_DEFINE_STR(KeyDcmdNo);
IMPLEMENT_EVENT_DEFINE_STR(KeyDcmdKey);
IMPLEMENT_EVENT_DEFINE_STR(KeyRecvStatus);
IMPLEMENT_EVENT_DEFINE_STR(KeyStationName);
IMPLEMENT_EVENT_DEFINE_STR(KeyArrTrainNo);
IMPLEMENT_EVENT_DEFINE_STR(KeyDptTrainNo);
//GPC
IMPLEMENT_EVENT_DEFINE_STR(KeyHelpTrain);
IMPLEMENT_EVENT_DEFINE_STR(KeyEleArm);
IMPLEMENT_EVENT_DEFINE_STR(KeyOrlDepartMoment);
IMPLEMENT_EVENT_DEFINE_STR(KeyDepartMoment);
IMPLEMENT_EVENT_DEFINE_STR(KeyMarkType);
IMPLEMENT_EVENT_DEFINE_STR(KeyTrainSpaceInfo);
IMPLEMENT_EVENT_DEFINE_STR(KeyTrainMeetInfo);
IMPLEMENT_EVENT_DEFINE_STR(KeyOldTrainNo);
IMPLEMENT_EVENT_DEFINE_STR(KeyMarkStartTime);
IMPLEMENT_EVENT_DEFINE_STR(KeyMarkEndTime);
IMPLEMENT_EVENT_DEFINE_STR(KeyMarkAbsNo);
IMPLEMENT_EVENT_DEFINE_STR(KeyMarkSideNo);

/////////////// 事件注册消息
IMPLEMENT_CLBTH(MsgEventReg, MSGID_MsgEventReg)
IMPLEMENT_SERIALIZE_OPERATOR(MsgEventReg)
MsgEventReg::MsgEventReg()
{
	CLBTH_ZERO_INIT_OBJECT(BaseMsg);
}
MsgEventReg::~MsgEventReg()
{
	
}
HRCODE MsgEventReg::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << nSrcEntityID << nEventID << nStatus;		
	}
	else
	{
		int t;
		ar >> nSrcEntityID >> t;			nEventID = (EVENT_ID)t;	
		ar >> t;							nStatus = (ReqStatus)t;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgEventReg::countObjectLength()
{
	return sizeof(nSrcEntityID)+sizeof(nEventID)+sizeof(nStatus);
}

/////////////// 事件内容消息
IMPLEMENT_CLBTH(MsgEventData, MSGID_MsgEventData)
IMPLEMENT_SERIALIZE_OPERATOR(MsgEventData)
MsgEventData::MsgEventData(EVENT_ID e)
{
	CLBTH_ZERO_INIT_OBJECT(BaseMsg);
	nDataLen = 0;
	nzDataBuffer = NULL;
	struKey = NULL;
	sSrcName[0] = sEventName[0] = sEventDesc[0] = 0;
	nEventRank = NORMAL_EVENT;
	nEventID = e;
}
MsgEventData::MsgEventData()
{
	CLBTH_ZERO_INIT_OBJECT(BaseMsg);
	nDataLen = 0;
	nzDataBuffer = NULL;
	nKeyCount = nKeySpace = 0;
	struKey = NULL;
	sSrcName[0] = sEventName[0] = sEventDesc[0] = 0;
	nEventRank = NORMAL_EVENT;
	nEventID = NULL_EVENT_ID;
}
MsgEventData::~MsgEventData()
{	
	if(nzDataBuffer != NULL)
		delete []nzDataBuffer;
	if(struKey != NULL)
		delete []struKey;
}
HRCODE MsgEventData::doSerialize(Archive& ar)
{
	int t;
	if(ar.isStoring())
	{
		if (NULL_EVENT_ID == nEventID)	return HRCODE(HRCODE::HC_FAIL);
		if (0 == sEventName[0])	SetEventName();
		if (0 == sEventName[0])	return HRCODE(HRCODE::HC_FAIL);
		if (0 == sEventDesc[0])
			strcpy(sEventDesc, sEventName);
		ar << nSrcEntityID << nEventID << tmEventTime << nEventIdx << nEventRank;	
		AR_WRITE_STRING(sSrcName, t);	
		AR_WRITE_STRING(sEventName, t);	
		AR_WRITE_STRING(sEventDesc, t);	
		if(nDataLen>=MAX_EVENT_DATA_LEN) return HRCODE(HRCODE::HC_FAIL);
		ar << nDataLen;
		ar.write(nzDataBuffer, nDataLen);

		ar << nKeyCount;
		for (int i=0; i<nKeyCount; i++)
		{
			ar << struKey[i].sKeyId;
			AR_WRITE_STRING(struKey[i].sKeyWord, t);
		}
	}
	else
	{
		ar >> nSrcEntityID >> t >> tmEventTime >> nEventIdx;		
		nEventID = (EVENT_ID)t;			
		if (NULL_EVENT_ID == nEventID)	return HRCODE(HRCODE::HC_FAIL);
		ar >> t;
		nEventRank = (EVENT_RANK)t;	
		AR_READ_STRING(sSrcName, t);
		AR_READ_STRING(sEventName, t);
		AR_READ_STRING(sEventDesc, t);	

		ar >> t;
		if(t>=MAX_EVENT_DATA_LEN) return HRCODE(HRCODE::HC_FAIL);
		if(nzDataBuffer != NULL) delete []nzDataBuffer;
		nzDataBuffer = new char[t+1];
		ar.read(nzDataBuffer, t);
		nzDataBuffer[t] = 0;
		nDataLen = t;
		
		ar >> nKeyCount;
		if(nKeyCount>MAX_EVENT_KEY_COUNT)
			return HRCODE(HRCODE::HC_FAIL);		
		nKeySpace = nKeyCount;
		struKey = new EVENT_KEY_DATA[nKeySpace];
		for (int i=0; i<nKeyCount; i++)
		{
			ar >> t;
			struKey[i].sKeyId = (EVENT_KEY_ID)t;
			AR_READ_STRING(struKey[i].sKeyWord, t);
		}
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgEventData::countObjectLength()
{
	//return 0;
	return sizeof(nSrcEntityID)+sizeof(nEventID)+sizeof(tmEventTime)+sizeof(nEventIdx)+sizeof(nEventRank)+sizeof(nKeyCount)+sizeof(sSrcName)+sizeof(sEventName)+sizeof(sEventDesc)+nKeyCount*(sizeof(struKey[0].sKeyId)+sizeof(struKey[0].sKeyWord))+sizeof(nDataLen)+nDataLen;
}

I_BOOL MsgEventData::SetData(const char* buffer)
{
	if ( NULL == buffer || strlen(buffer) >= MAX_EVENT_DATA_LEN)
	{
		nDataLen = 0;
		return IFALSE;
	}
	else 
	{
		if(nzDataBuffer != NULL) delete []nzDataBuffer;
		nDataLen = strlen(buffer);

		nzDataBuffer = new char[nDataLen+1];
		memcpy(nzDataBuffer, buffer, nDataLen);
		nzDataBuffer[nDataLen] = 0;
		return ITRUE;
	}
}
const char* MsgEventData::GetData() const
{
	return nzDataBuffer;
}

I_BOOL MsgEventData::IsKeyHeld(const EVENT_KEY_DATA ek) const
{
	for (int i=0; i<nKeyCount; i++)
	{
		if (ek.sKeyId == struKey[i].sKeyId
			&& strcmp(ek.sKeyWord, struKey[i].sKeyWord)==0)
			return ITRUE;
	}
	return IFALSE;
}
short MsgEventData::SetKey(EVENT_KEY_ID key, const char* ek)
{
	if (nKeyCount >= MAX_EVENT_KEY_COUNT)
		return 0;
	
	if (nKeyCount == nKeySpace)
	{
		nKeySpace += 16;
		EVENT_KEY_DATA* tmp_key = new EVENT_KEY_DATA[nKeySpace];
		for (int i=0; i<nKeyCount; i++)
			tmp_key[i] = struKey[i];
		delete[] struKey;
		struKey = tmp_key;
	}

	struKey[nKeyCount].sKeyId = key;
	memset(struKey[nKeyCount].sKeyWord, 0, sizeof(struKey[nKeyCount].sKeyWord));
	strncpy(struKey[nKeyCount].sKeyWord, ek, sizeof(struKey[nKeyCount].sKeyWord)-1);
	return ++nKeyCount;
}
short MsgEventData::SetKey(EVENT_KEY_ID key, int ek)
{
	char x[64]; sprintf(x, "%d", ek);
	return SetKey(key, x);
}
const char* MsgEventData::GetKey(EVENT_KEY_ID key) const
{
	for (int i=0; i<nKeyCount; i++)
	{
		if (key == struKey[i].sKeyId)
			return struKey[i].sKeyWord;
	}
	return NULL;
}
const char* MsgEventData::GetEventName()
{
	return sEventName;

}
void MsgEventData::SetEventName()
{
	switch (nEventID)
	{
	case EVENT_COMMON_APP_STOP: //程序退出后产生
		strcpy(sEventName,"程序退出");
		break;
	case EVENT_COMMON_APP_START:                    //程序启动后产生    
		strcpy(sEventName,"程序启动");
		break;
	case EVENT_COMMON_USER_LOGIN:                   //当用户登录时产生        
		strcpy(sEventName,"用户登录");
		break;
	case EVENT_COMMON_USER_LOGOUT:                  //当用户注销时产生   
		strcpy(sEventName,"用户注销");
		break;
	case EVENT_STPC_ACK_DEPART:                     //同意邻站发车   
		strcpy(sEventName,"同意邻站发车");
		break;
	case EVENT_STPC_REQ_DEPART:                     //向邻站发车请求      
		strcpy(sEventName,"向邻站发车请求");
		break;
	case EVENT_STPC_INFORM_DEPART:                  //向邻站发车预告  
		strcpy(sEventName,"向邻站发车预告");
		break;
	case EVENT_STPC_ACK_SIDE:                       //确认接车股道      
		strcpy(sEventName,"确认接车股道");
		break;
	case EVENT_STPC_REPORT_TRAIN_ARRIVE:            //确认列车到达,人工报点 
		strcpy(sEventName,"列车到达");
		break;
	case EVENT_STPC_REPORT_TRAIN_DEPART:            //确认列车出发,人工报点   
		strcpy(sEventName,"列车出发");
		break;
	case EVENT_STPC_ACK_SHUNT_STOP:                 //确认调车作业停止     
		strcpy(sEventName,"确认调车作业停止");
		break;
	case EVENT_STPC_ACK_TRAIN_INT:                  //确认列车完整       
		strcpy(sEventName,"确认列车完整");
		break;
	case EVENT_STPC_SIGN_PLAN:                      //签收阶段计划    
		strcpy(sEventName,"签收阶段计划");
		break;
	case EVENT_STPC_RECV_PLAN:                      //收到阶段计划    
		strcpy(sEventName,"收到阶段计划");
		break;
	case EVENT_STPC_SIGN_PLANNOTE:                  //签收阶段记事  
		strcpy(sEventName,"签收阶段记事");
		break;
	case EVENT_STPC_RECV_PLANNOTE:                  //收到阶段记事   
		strcpy(sEventName,"收到阶段记事");
		break;
	case EVENT_STPC_SIGN_DCMD:                      //签收调度命令  
		strcpy(sEventName,"签收调度命令");
		break;
	case EVENT_STPC_RECV_DCMD:                      //收到调度命令   
		strcpy(sEventName,"收到调度命令");
		break;
	case EVENT_STPC_SEND_DCMD_REQ:                  //请求调度命令       
		strcpy(sEventName,"请求调度命令");
		break;
	case EVENT_STPC_SEND_RADIO_DCMD:                //发送无线调度命令
		strcpy(sEventName,"发送无线调度命令");
		break;
	case EVENT_STPC_RECV_RADIO_DCMD_ACK:            //无线调度命令接收状态   
		strcpy(sEventName,"无线调度命令接收状态");
		break;
	case EVENT_STPC_REPORT_CURRENT_CAR:             //上报现在车   
		strcpy(sEventName,"上报现在车");
		break;
	case EVENT_STPC_REPORT_TRAIN_SUBAO:             //上报列车速报   
		strcpy(sEventName,"上报列车速报");
		break;
	case EVENT_LIRC_REPORT_ROUTE_STATUS:            //检测到进路排列,信号开放 
		strcpy(sEventName,"检测到进路排列,信号开放");
		break;
	case EVENT_LIRC_REPORT_TRAIN_ENTERING:          //检测到列车压过进站信号机
		strcpy(sEventName,"检测到列车压过进站信号机");
		break;
	case EVENT_LIRC_REPORT_TRAIN_EXITING:           //检测到列车压过出站信号机
		strcpy(sEventName,"检测到列车压过出站信号机");
		break;
	case EVENT_LIRC_REPORT_SIDE_OCC:                //检测到股道占用   
		strcpy(sEventName,"检测到股道占用");
		break;
	case EVENT_LIRC_REPORT_SIDE_UNOCC:              //检测到股道出清  
		strcpy(sEventName,"检测到股道出清");
		break;
	case EVENT_STPC_CHOOSE_SIDE:                    //选择接车股道      added by wuchunbo 
		strcpy(sEventName,"选择接车股道");
		break;
	case EVENT_STPC_AGREE_OPEN_ARSIG:               //允许开放接车信号
		strcpy(sEventName,"允许开放接车信号");
		break;
	case EVENT_STPC_AGREE_OPEN_DPTSIG:              //允许开放发车信号
		strcpy(sEventName,"允许开放发车信号");
		break;
	case EVENT_STPC_PREPARE_OPEN_ARSIG:             //准备开放接车信号
		strcpy(sEventName,"准备开放接车信号");
		break;
	case EVENT_STPC_PREPARE_OPEN_DPTSIG:             //准备开放发车信号
		strcpy(sEventName,"准备开放发车信号");
		break;
	case EVENT_STPC_TRAIN_APPROACH:					//列车接近
		strcpy(sEventName,"列车接近");
		break;
	case EVENT_STPC_DPT_CONDITION_OK:				//发车条件满足
		strcpy(sEventName,"发车条件满足");
		break;
	case EVENT_STPC_OPEN_DPTSIG_DONE:                //开放信号完毕
		strcpy(sEventName,"开放信号完毕");
		break;
	case EVENT_STPC_TRAIN_CONTACTED:					//接送列车
		strcpy(sEventName,"接送列车");
		break;
	case EVENT_STPC_PREPARE_CONTACT_TRAIN:			//准备接车
		strcpy(sEventName,"准备接车");
		break;
	case EVENT_LIRC_REPORT_TRAIN_ARRIVE:			//列车到达(自动报点
		strcpy(sEventName,"列车到达(自动报点)");
		break;
	case EVENT_LIRC_REPORT_TRAIN_DEPART:			//列车出发(自动报点
		strcpy(sEventName,"列车出发(自动报点)");
		break;
	case EVENT_LIRC_AR_ROUTE_CREATE:			//接车进路创建
		strcpy(sEventName,"接车进路创建");
		break;
	case EVENT_LIRC_AR_ROUTE_CANCEL:			//接车进路取消
		strcpy(sEventName,"接车进路取消");
		break;
	case EVENT_LIRC_PS_ROUTE_CREATE:			//通过进路创建
		strcpy(sEventName,"通过进路创建");
		break;
	case EVENT_LIRC_PS_ROUTE_CANCEL:			//通过进路取消
		strcpy(sEventName,"通过进路取消");
		break;
	case EVENT_LIRC_DP_ROUTE_CREATE:			//发车进路创建
		strcpy(sEventName,"发车进路创建");
		break;
	case EVENT_LIRC_DP_ROUTE_CANCEL:			//发车进路取消
		strcpy(sEventName,"发车进路取消");
		break;
	case EVENT_STPC_AR_ROUTE_CREATING:
		strcpy(sEventName,"办理接车进路");
		break;
	case EVENT_STPC_DP_ROUTE_CREATING:
		strcpy(sEventName,"办理发车进路");
		break;
	case EVENT_STPC_PREPARE_DPT:							//准备发车
		strcpy(sEventName,"准备发车");
		break;
	case EVENT_STPC_ACK_SIDE_CANCEL:                     //接车股道不满足接车条件
		strcpy(sEventName,"接车股道不满足接车条件");
		break;
	case EVENT_LIRC_REPORT_SIDE_WHOLE_OCC:							//检测到完全股道占用
		strcpy(sEventName,"检测到完全股道占用");
		break;
	case EVENT_STPC_REPORT_TRAIN_PASS:							//列车通过,人工报点
		strcpy(sEventName,"列车通过(人工报点)");
		break;
	case EVENT_LIRC_REPORT_TRAIN_PASS:							//列车通过,自动报点
		strcpy(sEventName,"列车通过(自动报点)");
		break;
	case EVENT_STPC_OPEN_DPTSIG_CANCEL:          //不满足发车条件
		strcpy(sEventName,"不满足开放信号条件");
		break;
	case EVENT_COMMON_TRAIN_ALARM:				//行车报警  
		strcpy(sEventName,"行车报警");
		break;
	case EVENT_COMMON_SYS_ALARM:				//系统报警  
		strcpy(sEventName,"系统报警");
		break;
	case EVENT_STPC_CHECK_LINE:                  //检查线路
		strcpy(sEventName,"检查线路");
		break;
	case EVENT_STPC_ST_HOME_RT:					 //准备接车进路
		strcpy(sEventName,"准备接车进路");
		break;
	case EVENT_STPC_HOME_TRAIN:					 //接车
		strcpy(sEventName,"接车");
		break;
	case EVENT_STPC_SET_DEPART_RT:				 //准备发车进路
		strcpy(sEventName,"准备发车进路");
		break;
	case EVENT_STPC_SET_TOKEN:					 //办理凭证
		strcpy(sEventName,"办理凭证");
		break;
	case EVENT_STPC_ACK_DPT_CDT:				 //确认发车条件
		strcpy(sEventName,"确认发车条件");
		break;
	case EVENT_STPC_ACK_DPT_TRAIN:				 //发车
		strcpy(sEventName,"发车");
		break;
	case EVENT_STPC_NEIB_AR:					 //邻站到达
		strcpy(sEventName,"邻站到达");
		break;
	case EVENT_STPC_OP_RECORD:				   //车站人工操作事件
		strcpy(sEventName,"人工操作");
		break;
	case EVENT_STPC_FLOW_STATUS:				   //车站流程变化事件
		strcpy(sEventName,"流程变化");
		break;
		
	case EVENT_GPC_LOGIN:							//运行图登陆
		strcpy(sEventName,"运行图登陆");
		break;
	case EVENT_GPC_SHIFT_BANBIE:					 //运行图交接班
		strcpy(sEventName,"运行图交接班");
		break;
	case EVENT_GPC_MODIFY_SIDE:						 //修改股道
		strcpy(sEventName,"修改股道");
		break;
	case EVENT_GPC_SEND_STPLAN:						 //下发阶段计划
		strcpy(sEventName,"下发阶段计划");
		break;
	case EVENT_GPC_PASS_TO_STAY:					 //通过变到开
		strcpy(sEventName,"通过变到开");
		break;
	case EVENT_GPC_STAY_TO_PASS:					 //到开变通过
		strcpy(sEventName,"到开变通过");
		break;
	case EVENT_GPC_STOP_ABS:						 //区间停车
		strcpy(sEventName,"区间停车");
		break;
	case EVENT_GPC_STOP_OUT_SIGNED:					 //机外停车
		strcpy(sEventName,"机外停车");
		break;
	case EVENT_GPC_CHANG_TRAINID:					 //修改车次
		strcpy(sEventName,"修改车次");
		break;
	case EVENT_GPC_MODIFY_DEPART:					 //修改发车时机
		strcpy(sEventName,"修改发车时机");
		break;
	case EVENT_GPC_TRAIN_PLAN_PARA:					 //修改计划线参数
		strcpy(sEventName,"修改计划线参数");
		break;
	case EVENT_GPC_RECV_NEIBPLAN:					 //收邻台计划
		strcpy(sEventName,"收邻台计划");
		break;
	case EVENT_GPC_SEND_NEIBPLAN:					 //发邻台计划
		strcpy(sEventName,"发邻台计划");
		break;
	case EVENT_GPC_CHANGE_TRAIN_REAL:				 //转实际
		strcpy(sEventName,"转实际");
		break;
	case EVENT_GPC_MOUSE_REAL:						 //鼠标收点
		strcpy(sEventName,"鼠标收点");
		break;
	case EVENT_GPC_NEW_TRAIN:						 //新增列车
		strcpy(sEventName,"新增列车");
		break;
	case EVENT_GPC_DELETE_TRAIN:					 //删除列车
		strcpy(sEventName,"删除列车");
		break;	
	case EVENT_GPC_ELE_ARM:							 //修改供电臂状态
		strcpy(sEventName,"修改供电臂状态");
		break;
	case EVENT_GPC_MODIFY_ABSLINE:					 //修改区间线别
		strcpy(sEventName,"修改区间线别");
		break;
	case EVENT_GPC_NEW_MARK:						 //新增施工
		strcpy(sEventName,"新增施工");
		break;
	case EVENT_GPC_DELETE_MARK:						 //删除施工
		strcpy(sEventName,"删除施工");
		break;
	case EVENT_GPC_MODIFY_MARK:						 //修改施工
		strcpy(sEventName,"修改施工");
		break;
	case EVENT_GPC_TRAIN_ATTR_SPACE:				 //修改列车超限
		strcpy(sEventName,"修改列车超限");
		break;
	case EVENT_GPC_INHIBIT_TRAINS_MEET:				 //修改列车禁会
		strcpy(sEventName,"修改列车禁会");
		break;
	case EVENT_GPC_MODIFY_TRAIN_TIME:				 //列车时刻修改
		strcpy(sEventName,"列车时刻修改");
		break;
	case	EVENT_DCMD_CREATE_COMMOND://	调度命令创建
		strcpy(sEventName,"调度命令创建");
		break;		
	case	EVENT_DCMD_SAVE_COMMOND://	调度命令缓存
		strcpy(sEventName,"调度命令缓存");
		break;			
	case	EVENT_DCMD_SEND_COMMOND://调度命令下发
		strcpy(sEventName,"调度命令下发");
		break;			
	case	EVENT_DCMD_RECV_COMMOND://	调度命令接受
		strcpy(sEventName,"调度命令接受");
		break;			
	case	EVENT_DCMD_SIGN_COMMOND://调度命令签收
		strcpy(sEventName,"调度命令签收");
		break;			
	case	EVENT_DCMD_LOGIN://调度命令登录
		strcpy(sEventName,"调度命令登录");
		break;	
	case	EVENT_DCMD_FEEDBACK_COMMOND://调度命令收到接受回执
		strcpy(sEventName,"调度命令收到接受回执");
		break;	
	case	EVENT_LAYOUTTERM_SM_REQUEST://车站控制模式申请
		strcpy(sEventName,"车站控制模式申请");
		break;	
	case	EVENT_LAYOUTTERM_RM_REQUEST:// 车站进路模式申请
		strcpy(sEventName,"车站进路模式申请");
		break;
	case	EVENT_LAYOUTTERM_TRAININDEX_OPER://车次号操作
		strcpy(sEventName,"车次号操作");
		break;	
	case	EVENT_LAYOUTTERM_TRAINSTOP://设置列车停稳、启动
		strcpy(sEventName,"设置列车停稳、启动");
		break;	     
	case	EVENT_LAYOUTTERM_TRAINATTR_OPER:// 修改列车属性
		strcpy(sEventName,"修改列车属性");
		break;	
	case	EVENT_LAYOUTTERM_MANUAL_OPER://人工操作命令
		strcpy(sEventName,"人工操作命令");
		break;	    
	case	EVENT_LAYOUTTERM_RBCTRAIN_OPER://车次号操作
		 strcpy(sEventName,"车次号操作");
		 break;	 
	case	EVENT_LAYOUTTERM_STATIONCTCFLAG:// 车站CTC特征标志设置
		strcpy(sEventName,"车站CTC特征标志设置");
		break;
	case	EVENT_LAYOUTTERM_ROUTETRIG:// 进路人工触发
		strcpy(sEventName,"进路人工触发");
		break;
	case	EVENT_LAYOUTTERM_TRIGFLAGUPDATE:// 修改自触标记
		strcpy(sEventName,"修改自触标记");
		break;
	case	EVENT_TSRTERM_CMD_OPER://限速命令操作
		strcpy(sEventName,"限速命令操作");
		break;
	case	EVENT_TSRTERM_CMD_RESULT://限速命令操作结果
		strcpy(sEventName,"限速命令操作结果");
		break;
	case	EVENT_LIRC_ROUTE_STATUS://进路状态变化
		strcpy(sEventName,"进路状态变化");
		break;
	default:
		strcpy(sEventName,"");
		break;
	}	
}
/////////////// 事件查询消息
IMPLEMENT_CLBTH(MsgEventReq, MSGID_MsgEventReq)
IMPLEMENT_SERIALIZE_OPERATOR(MsgEventReq)
MsgEventReq::MsgEventReq()
{
	CLBTH_ZERO_INIT_OBJECT(BaseMsg);
	nKeyCount = nKeySpace = 0;
	struKey = NULL;
	nEventRank = NORMAL_EVENT;
}
MsgEventReq::~MsgEventReq()
{	
	if(struKey != NULL)
		delete []struKey;
}
HRCODE MsgEventReq::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << nSrcEntityID << nEventID << nStartTime << nEndTime << nEventRank << nKeyCount;	
		for (int i=0; i<nKeyCount; i++)
		{
			ar << struKey[i].sKeyId;
			ar.write(struKey[i].sKeyWord, sizeof(struKey[i].sKeyWord));
		}
	}
	else
	{
		int t;
		ar >> nSrcEntityID >> t >> nStartTime >> nEndTime;	
		nEventID = (EVENT_ID)t;		
		ar >> t >> nKeyCount;
		nEventRank = (EVENT_RANK)t;	
		if(nKeyCount>MAX_EVENT_KEY_COUNT)
			return HRCODE(HRCODE::HC_FAIL);
		nKeySpace = nKeyCount;
		struKey = new EVENT_KEY_DATA[nKeySpace];
		for (int i=0; i<nKeyCount; i++)
		{
			ar >> t;
			struKey[i].sKeyId = (EVENT_KEY_ID)t;
			ar.read(struKey[i].sKeyWord, sizeof(struKey[i].sKeyWord));
		}	
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgEventReq::countObjectLength()
{
	return sizeof(nSrcEntityID)+sizeof(nEventID)+sizeof(nStartTime)+sizeof(nEndTime)+sizeof(nEventRank)+sizeof(nKeyCount)+nKeyCount*(sizeof(struKey[0].sKeyId)+sizeof(struKey[0].sKeyWord));
}
short MsgEventReq::SetKey(EVENT_KEY_ID key, const char* ek)
{
	if (nKeyCount >= MAX_EVENT_KEY_COUNT)
		return 0;

	if (nKeyCount == nKeySpace)
	{
		nKeySpace += 16;
		EVENT_KEY_DATA* tmp_key = new EVENT_KEY_DATA[nKeySpace];
		for (int i=0; i<nKeyCount; i++)
			tmp_key[i] = struKey[i];
		delete[] struKey;
		struKey = tmp_key;
	}

	struKey[nKeyCount].sKeyId = key;
	memset(struKey[nKeyCount].sKeyWord, 0, sizeof(struKey[nKeyCount].sKeyWord));
	strncpy(struKey[nKeyCount].sKeyWord, ek, sizeof(struKey[nKeyCount].sKeyWord)-1);
	return ++nKeyCount;
}
short MsgEventReq::SetKey(EVENT_KEY_ID key, int ek)
{
	char x[64]; sprintf(x, "%d", ek);
	return SetKey(key, x);
}

///////////// END /////////////
CLBTH_NAMESPACE_END
