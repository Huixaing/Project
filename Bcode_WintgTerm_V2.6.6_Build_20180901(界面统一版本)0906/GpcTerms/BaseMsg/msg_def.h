#ifndef _MSG_DEF_H_
#define  _MSG_DEF_H_

#include "private_msg_def.h"

CLBTH_NAMESPACE_BEGIN


// 待定的位标志
// DMIS_STATUS
// STATION_CTC_FLAG
// SHUNT_GOU_TYPE

// ------------------ DSS 消息-----------------------------------------------------------
// 站场ICON表示
class AFX_EXT_CLASS MsgDSSDisplay : public BaseMsg
{
	DECLARE_CLBTH(MsgDSSDisplay)

public:
	MsgDSSDisplay();
	virtual ~MsgDSSDisplay();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	STATION_NO  nStationNo; //站号
	DEVICE_TYPE nType;  //设备类型
	DMIS_ID   nID;  //dmis号
	DEVICE_STATUS nStatus;  //设备状态
	IWORD   nStrLen;            // strMoreInfo的长度
	char    strMoreInfo[MAX_DMIS_STR_LEN];  // 当nType是车次窗时, 表示车次号; 否则记录附加信息(一般为空)
};
DECLARE_SERIALIZE_OPERATOR(MsgDSSDisplay)

// 车次号操作，包括添加、删除、修改
class AFX_EXT_CLASS MsgTrainNoOper : public BaseMsg
{
	DECLARE_CLBTH(MsgTrainNoOper)

public:
	MsgTrainNoOper();
	virtual ~MsgTrainNoOper();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	STATION_NO  nStationNo;  //站号
	DMIS_ID     nWindowID;   //车次窗号
	IBYTE       nOldLen;     //老车次号长度
	TRAIN_NO    tnOldTrain;  //老车次号
	IBYTE       nNewLen;     //新车次号长度
	TRAIN_NO    tnNewTrain;  //新车次号
	IDWORD      nIPIndex;    //IP索引
};
DECLARE_SERIALIZE_OPERATOR(MsgTrainNoOper)

// 车次号人工确认消息
class AFX_EXT_CLASS MsgTrainConfirm : public BaseMsg
{
	DECLARE_CLBTH(MsgTrainConfirm)

public:
	MsgTrainConfirm();
	virtual ~MsgTrainConfirm();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	STATION_NO  nStationNo;   //站号
	DMIS_ID     nWindowID;    //车次窗号
	IBYTE       nTrainNoLen;  //车次号长度
	TRAIN_NO    tnTrainNo;    //车次号
	IDWORD      nIPIndex;     //IP索引
};
DECLARE_SERIALIZE_OPERATOR(MsgTrainConfirm)

// 车次号属性修改
class AFX_EXT_CLASS MsgSetTrainAttrib : public BaseMsg
{
	DECLARE_CLBTH(MsgSetTrainAttrib)

public:
	MsgSetTrainAttrib();
	virtual ~MsgSetTrainAttrib();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	STATION_NO  nStationNo; //站号
	DMIS_ID   nWindowID;  //车次窗号
	IBYTE   nTrainNoLen;  //车次号长度
	TRAIN_NO  tnTrainNo;  //车次号
	IDWORD    nIPIndex; //IP索引
	short   nTrainType; //列车类型
	short   nTrainRate; //列车速度
	char    nLocoNoLen; //机车号长度
	char    szLocoNo[50]; //机车号
};
DECLARE_SERIALIZE_OPERATOR(MsgSetTrainAttrib)

// 车次停稳人工设置消息
class AFX_EXT_CLASS MsgSetTrainStop : public BaseMsg
{
	DECLARE_CLBTH(MsgSetTrainStop)

public:
	MsgSetTrainStop();
	virtual ~MsgSetTrainStop();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	STATION_NO  nStationNo;       //站号
	DMIS_ID     nWindowID;        //车次窗号
	TRAIN_NO    tnTrainNo;        //车次号
	IDWORD      nIPIndex;         //IP索引
	enum { TRAIN_START=1, TRAIN_STOP=2 };
	IBYTE       nStopInfo;          // 1表示启动，2表示已停稳
};
DECLARE_SERIALIZE_OPERATOR(MsgSetTrainStop)


// 站死
class AFX_EXT_CLASS MsgStationDown : public BaseMsg
{
	DECLARE_CLBTH(MsgStationDown)

public:
	MsgStationDown();
	virtual ~MsgStationDown();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	STATION_NO  nStationNo; //站号
};
DECLARE_SERIALIZE_OPERATOR(MsgStationDown)

// 站活
class AFX_EXT_CLASS MsgStationUp : public BaseMsg
{
	DECLARE_CLBTH(MsgStationUp)

public:
	MsgStationUp();
	virtual ~MsgStationUp();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	STATION_NO  nStationNo; //站号
};
DECLARE_SERIALIZE_OPERATOR(MsgStationUp)

class AFX_EXT_CLASS MsgDssInitComplete : public BaseMsg
{
	DECLARE_CLBTH(MsgDssInitComplete)

public:
	MsgDssInitComplete();
	virtual ~MsgDssInitComplete();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	STATION_NO  nStationNo; //站号
};
DECLARE_SERIALIZE_OPERATOR(MsgDssInitComplete)

// DSS车站设备人工操作(故障区段设置等)
class AFX_EXT_CLASS MsgDSSManualDeviceOper : public BaseMsg
{
	DECLARE_CLBTH(MsgDSSManualDeviceOper)

public:
	MsgDSSManualDeviceOper();
	virtual ~MsgDSSManualDeviceOper();

protected:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	STATION_NO        nStationNo;       //站号
	MANUAL_OPER_TYPE  nOperType;        //操作类型
	DEVICE_TYPE       nDeviceType;      //设备类型
	DMIS_ID           nDeviceID;        //设备dmis号
	I_TIME            tmOperTime;       //操作时间
	char              strDeviceName[20];//设备名称
	short             nOperPara;        //操作参数
};
DECLARE_SERIALIZE_OPERATOR(MsgDSSManualDeviceOper)

// ------------------------------ 按钮操作相关消息 -----------------------------------------------

// 按钮按下
class AFX_EXT_CLASS MsgManualButtonPress : public BaseMsg
{
	DECLARE_CLBTH(MsgManualButtonPress)

public:
	MsgManualButtonPress();
	virtual ~MsgManualButtonPress();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	MANUAL_OPER_ID  nOperID;  // 人工操作的唯一ID标识
	STATION_NO    nStationNo; //站号
	DEVICE_TYPE   nDeviceType;  //设备类型
	DMIS_ID     nDmisID;  //dmis号
	char      strDeviceName[20];  //设备名称
	IBYTE     nOperPara;  //操作参数
};
DECLARE_SERIALIZE_OPERATOR(MsgManualButtonPress)

// 按钮清除
class AFX_EXT_CLASS MsgManualButtonCancel : public BaseMsg
{
	DECLARE_CLBTH(MsgManualButtonCancel)

public:
	MsgManualButtonCancel();
	virtual ~MsgManualButtonCancel();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	MANUAL_OPER_ID  nOperID;  // 人工操作的唯一ID标识
	STATION_NO    nStationNo; //站号
	DEVICE_TYPE   nDeviceType;  //设备类型
	DMIS_ID     nDmisID;  //dmis号
	char      strDeviceName[20];  //设备名称
	IBYTE     nOperPara;  //操作参数
};
DECLARE_SERIALIZE_OPERATOR(MsgManualButtonCancel)

// 进路建立
class AFX_EXT_CLASS MsgManualRouteSet : public BaseMsg
{
	DECLARE_CLBTH(MsgManualRouteSet)

public:
	MsgManualRouteSet();
	virtual ~MsgManualRouteSet();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
#ifdef _MSC_VER
	virtual string toString();
#endif

public:
	MANUAL_OPER_ID  nOperID;  // 人工操作的唯一ID标识
	STATION_NO    nStationNo; //站号
	IBYTE     nButtonCount; //按钮数量
	BUTTON_ID   nButtonID[MAX_ROUTE_BUTTON_COUNT];  //按钮ID
	BUTTON_NAME strButtonName[MAX_ROUTE_BUTTON_COUNT];  //按钮名称
	ROUTE_ID    nRouteID; //进路号
	enum ROUTE_TYPE { INVALID=0, TRAIN, SHUNT }; //0表示无效、列车、调车
	ROUTE_TYPE    nRouteType; //进路类型
	I_TIME      tmStartTime;  //进路开始时间
	TIME_INTERVAL tmExistTime;  // 进路预计存在时间
	IBYTE     bForceFlag;   // 1:强制执行(仅进行联锁逻辑检查); 0:需进行站细等检查, 下同
	TRAIN_NO    tnTrainNo;  //车次号
};
DECLARE_SERIALIZE_OPERATOR(MsgManualRouteSet)

// 设备人工操作(道岔单操、封锁解锁等)
class AFX_EXT_CLASS MsgManualDeviceOper : public BaseMsg
{
	DECLARE_CLBTH(MsgManualDeviceOper)

public:
	MsgManualDeviceOper();
	virtual ~MsgManualDeviceOper();

protected:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
#ifdef _MSC_VER
	virtual string toString();
#endif
	void SetDeviceName(const char* pstrName);

public:
	MANUAL_OPER_ID  nOperID;          //操作ID
	STATION_NO    nStationNo;         //站号
	MANUAL_OPER_TYPE  nOperType;      //操作类型
	DEVICE_TYPE   nDeviceType;      //设备类型
	DMIS_ID     nDeviceID;          //设备dmis号
	I_TIME      tmOperTime;       //操作时间
	char      strDeviceName[20];  //设备名称
	short     nOperPara;          //操作参数
	IBYTE     bForceFlag;         //1:强制执行(仅进行联锁逻辑检查); 0:需进行站细等检查
};
DECLARE_SERIALIZE_OPERATOR(MsgManualDeviceOper)

// 设备关键人工操作(包括进路取消，总人解等)
class AFX_EXT_CLASS MsgManualDeviceOperEx : public MsgManualDeviceOper
{
	DECLARE_CLBTH(MsgManualDeviceOperEx)

public:
	MsgManualDeviceOperEx();
	virtual ~MsgManualDeviceOperEx();
	void    MakeBaseDeviceOperMsg( MsgManualDeviceOper& rBaseMsg );

protected:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	IDWORD  nRandomPara;      // 随机数参数
};
DECLARE_SERIALIZE_OPERATOR(MsgManualDeviceOperEx)

// 进路试排
class AFX_EXT_CLASS MsgManualRouteTryLock : public BaseMsg
{
	DECLARE_CLBTH(MsgManualRouteTryLock)

public:
	MsgManualRouteTryLock();
	virtual ~MsgManualRouteTryLock();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
#ifdef _MSC_VER
	virtual string toString();
#endif

public:
	MANUAL_OPER_ID  nOperID;  // 人工操作的唯一ID标识
	STATION_NO    nStationNo; //站号
	IBYTE     nButtonCount; //按钮数
	BUTTON_ID   nButtonID[MAX_ROUTE_BUTTON_COUNT];  //按钮ID号
	BUTTON_NAME strButtonName[MAX_ROUTE_BUTTON_COUNT];  //按钮名称
	enum ROUTE_TYPE { INVALID=0, TRAIN, SHUNT };  //进路类型：无效、列车、调车
	ROUTE_TYPE    nRouteType; //进路类型
	ROUTE_ID    nRouteID; //进路ID
	I_TIME      tmStartTime;  //进路开始时间
	TIME_INTERVAL tmExistTime;  // 进路预计存在时间
	IBYTE     bForceFlag;   // 1:强制执行(仅进行联锁逻辑检查); 0:需进行站细等检查, 下同
};
DECLARE_SERIALIZE_OPERATOR(MsgManualRouteTryLock)

// 操作使能
class AFX_EXT_CLASS MsgManualDeviceOperEnable : public BaseMsg
{
	DECLARE_CLBTH(MsgManualDeviceOperEnable)

public:
	MsgManualDeviceOperEnable();
	virtual ~MsgManualDeviceOperEnable();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	STATION_NO      nStationNo;   // 站号
	MANUAL_OPER_TYPE  nOperType;    // 操作类别
	I_TIME        tmOperTime; // 操作开始时间
	IDWORD        nRandomPara;  // 随机数参数
};
DECLARE_SERIALIZE_OPERATOR(MsgManualDeviceOperEnable)

// 道岔单操
/*
class AFX_EXT_CLASS MsgManualSwitchOper : public BaseMsg
{
DECLARE_CLBTH(MsgManualSwitchOper)

public:
MsgManualSwitchOper();
virtual ~MsgManualSwitchOper();

private:
virtual HRCODE doSerialize(Archive& ar);
protected:
virtual size_t countObjectLength();

public:
MANUAL_OPER_ID  nOperID;
STATION_NO    nStationNo;
DMIS_ID     nSwitchID;
MANUAL_OPER_TYPE  nOperType;  // 0:单锁; 1:单解; 2:单操
IBYTE     bForceFlag;
};
DECLARE_SERIALIZE_OPERATOR(MsgManualSwitchOper)

// 区段操作
class AFX_EXT_CLASS MsgManualTrackOper : public BaseMsg
{
DECLARE_CLBTH(MsgManualTrackOper)

public:
MsgManualTrackOper();
virtual ~MsgManualTrackOper();

private:
virtual HRCODE doSerialize(Archive& ar);
protected:
virtual size_t countObjectLength();

public:
MANUAL_OPER_ID  nOperID;
STATION_NO    nStationNo;
DMIS_ID     nTrackID;
MANUAL_OPER_TYPE  nOperType;  // 0:单锁; 1:解锁; 2:封闭; 3:解封
IBYTE     bForceFlag;
};
DECLARE_SERIALIZE_OPERATOR(MsgManualTrackOper)

// 信号机操作
class AFX_EXT_CLASS MsgManualSignalOper : public BaseMsg
{
DECLARE_CLBTH(MsgManualSignalOper)

public:
MsgManualSignalOper();
virtual ~MsgManualSignalOper();

private:
virtual HRCODE doSerialize(Archive& ar);
protected:
virtual size_t countObjectLength();

public:
MANUAL_OPER_ID  nOperID;
STATION_NO    nStationNo;
DMIS_ID     nSignalID;
MANUAL_OPER_TYPE  nOperType;  // 0:紧急关闭; 1:重新开放; 2:封闭
IBYTE     bForceFlag;
};
DECLARE_SERIALIZE_OPERATOR(MsgManualSignalOper)
*/

// 人工操作的回应
class AFX_EXT_CLASS MsgManualOperResult : public BaseMsg
{
	DECLARE_CLBTH(MsgManualOperResult)

public:
	MsgManualOperResult();
	virtual ~MsgManualOperResult();
private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	bool    CanForceOper() const;
	void    SetForceOperFlag( bool b=true );
	MANUAL_OPER_RESULT  GetResult();

public:
	MANUAL_OPER_ID    nOperID;    // 针对nOperID的回应
	MANUAL_OPER_RESULT  nResult;    // 操作回应结果  0: 成功, 1:联锁冲突, 2:计划冲突, 3:无效指令
	char        strComment[MAX_RESULT_COMMENT_LEN]; // 附加说明
	IBYTE       nCommentLen;  // 附加说明的长度
};
DECLARE_SERIALIZE_OPERATOR(MsgManualOperResult)

// ------------------------------ 车站与进路控制模式转换 ----------------------------------------------

// 车站控制模式转换申请(包括GPC/STPC向LiRC申请和LiRC向GPC/STPC(所有者)申请确认)
class AFX_EXT_CLASS MsgStationModeRequest : public BaseMsg
{
	DECLARE_CLBTH(MsgStationModeRequest)

public:
	MsgStationModeRequest();
	virtual ~MsgStationModeRequest();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	ENTITY_ID     nOriginalID;    // 原始申请者
	STATION_NO    nStationNo;     // 申请的目标车站
	ZONE_ID       nZoneID;        // 分区号(目前值只取0xFFFF,表示车站控制模式)
	STATION_CTRL_MODE nMode;      // 申请的模式
};
DECLARE_SERIALIZE_OPERATOR(MsgStationModeRequest)

// 车站控制模式申请结果(包括LiRC发给GPC/STPC的申请结果和GPC/STPC发给LiRC的批准结果)
class AFX_EXT_CLASS MsgStationModeResult : public BaseMsg
{
	DECLARE_CLBTH(MsgStationModeResult)

public:
	MsgStationModeResult();
	virtual ~MsgStationModeResult();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	ENTITY_ID           nOriginalID;    // 原始申请者
	STATION_NO          nStationNo;     // 申请的目标车站
	ZONE_ID             nZoneID;        // 分区号(目前值只取0xFFFF,表示车站控制模式)
	STATION_CTRL_MODE   nMode;          // 申请的模式
	STATION_CTRL_RESULT nResult;        // 申请结果
	char        strComment[MAX_RESULT_COMMENT_LEN]; // 结果说明
	IBYTE       nCommentLen;    // 结果说明的长度
};
DECLARE_SERIALIZE_OPERATOR(MsgStationModeResult)

// 进路控制模式申请(GPC/STPC->LiRC)
class AFX_EXT_CLASS MsgRouteModeRequest : public BaseMsg
{
	DECLARE_CLBTH(MsgRouteModeRequest)

public:
	MsgRouteModeRequest();
	virtual ~MsgRouteModeRequest();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	STATION_NO        nStationNo; //站号
	ROUTE_CTRL_MODE   nMode;      //进路控制模式
};
DECLARE_SERIALIZE_OPERATOR(MsgRouteModeRequest)

// 进路控制模式申请结果(LiRC->GPC/STPC)
class AFX_EXT_CLASS MsgRouteModeResult : public BaseMsg
{
	DECLARE_CLBTH(MsgRouteModeResult)

public:
	MsgRouteModeResult();
	virtual ~MsgRouteModeResult();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	STATION_NO      nStationNo; //站号
	ROUTE_CTRL_MODE   nMode;        //申请的模式
	ROUTE_CTRL_RESULT nResult;      //申请结果
	char        strComment[MAX_RESULT_COMMENT_LEN]; //结果说明
	IBYTE       nCommentLen;    //结果说明的长度
};
DECLARE_SERIALIZE_OPERATOR(MsgRouteModeResult)

// 车站控制状态报告(LiRC->GPC/STPC/BL/SERVER)
class AFX_EXT_CLASS MsgStationModeReport : public BaseMsg
{
	DECLARE_CLBTH(MsgStationModeReport)

public:
	MsgStationModeReport();
	virtual ~MsgStationModeReport();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	STATION_NO        nStationNo;     // 站号
	ZONE_ID           nZoneID;        // 分区号
	STATION_CTRL_MODE nStationMode;   // 车站控制模式
	ROUTE_CTRL_MODE   nRouteMode;     // 进路控制模式
	USER_RIGHT        nRight;         // 用户权限
	STATION_CTC_FLAG  nCTCFlag;       // 车站CTC特征标志
};
DECLARE_SERIALIZE_OPERATOR(MsgStationModeReport)

// 车站CTC特征标志(计划控制)设置
class AFX_EXT_CLASS MsgStationCTCFlagSet : public BaseMsg
{
	DECLARE_CLBTH(MsgStationCTCFlagSet)

public:
	MsgStationCTCFlagSet();
	virtual ~MsgStationCTCFlagSet();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	STATION_NO        nStationNo; //站号
	STATION_CTC_FLAG  nFlag;      //
	STATION_CTC_FLAG  nMask;      //
};
DECLARE_SERIALIZE_OPERATOR(MsgStationCTCFlagSet)

// 查询车站控制模式
class AFX_EXT_CLASS MsgStationModeQuery : public BaseMsg
{
	DECLARE_CLBTH(MsgStationModeQuery)

public:
	MsgStationModeQuery();
	virtual ~MsgStationModeQuery();

private:
	virtual HRCODE doSerialize(Archive& ar);

public:
	STATION_NO      nStationNo; //车站
};
DECLARE_SERIALIZE_OPERATOR(MsgStationModeQuery)

// ------------------------------ 进路相关消息 ----------------------------------------------------

// 进路序列申请(GPC/STPC->LiRC)
class AFX_EXT_CLASS MsgRouteListRequest : public BaseMsg
{
	DECLARE_CLBTH(MsgRouteListRequest)

public:
	MsgRouteListRequest();
	virtual ~MsgRouteListRequest();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	STATION_NO        nStationNo; //站号
	ROUTE_LIST_OPER_TYPE  nOperType;  // 申请类型
	ROUTE_LIST_HANDLE   nHandle;  // 申请的进路序列
};
DECLARE_SERIALIZE_OPERATOR(MsgRouteListRequest)

// 进路序列释放(GPC/STPC->LiRC)
class AFX_EXT_CLASS MsgRouteListRelease : public BaseMsg
{
	DECLARE_CLBTH(MsgRouteListRelease)

public:
	MsgRouteListRelease();
	virtual ~MsgRouteListRelease();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	STATION_NO        nStationNo; //站号
	ROUTE_LIST_OPER_TYPE  nOperType;  // 申请类型
	ROUTE_LIST_HANDLE   nHandle;  // 申请的进路序列
};
DECLARE_SERIALIZE_OPERATOR(MsgRouteListRelease)

// 进路序列申请回应lirc====>gpc/stpc
class AFX_EXT_CLASS MsgRouteListResult : public BaseMsg
{
	DECLARE_CLBTH(MsgRouteListResult)

public:
	MsgRouteListResult();
	virtual ~MsgRouteListResult();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	STATION_NO        nStationNo; //站号
	ROUTE_LIST_OPER_TYPE  nOperType;  // 申请类型
	ROUTE_LIST_HANDLE   nHandle;  // 申请的进路序列
	ROUTE_REQUEST_RESULT  nResult;  //进路序列申请结果
};
DECLARE_SERIALIZE_OPERATOR(MsgRouteListResult)

// 进路更改,更改进路描述(gpc/stpc====>lirc),无用
class AFX_EXT_CLASS MsgRouteUpdate : public BaseMsg
{
	DECLARE_CLBTH(MsgRouteUpdate)

public:
	MsgRouteUpdate();
	virtual ~MsgRouteUpdate();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	STATION_NO      nStationNo; //站号
	ROUTE_LIST_HANDLE nHandle;  //更改的进路序列
	ROUTE_LIST_ID   nListID;    // 要更改的进路序列号
	IBYTE       nButtonCount; //按钮数
	BUTTON_ID     nButtonID[MAX_ROUTE_BUTTON_COUNT];  //按钮ID
	BUTTON_NAME   strButtonName[MAX_ROUTE_BUTTON_COUNT];  //按钮名称
	enum ROUTE_TYPE { INVALID=0, TRAIN, SHUNT };  //进路类型:无效、列车、调车
	ROUTE_TYPE    nRouteType; //进路类型
	ROUTE_ID      nNewRouteID;  // 新的进路
	ROUTE_LIST_ID   nGiveWayTo;   // 新的避让进路
	ROUTE_TRIG_TYPE   nTrigFlag;    // 新进路的触发类型(0:人工触发, 1:自动触发)
	SIDE_ID       nSideID;   //要更改的股道， 值为－1表示不更改
};
DECLARE_SERIALIZE_OPERATOR(MsgRouteUpdate)

// 进路股道更改gpc/stpc====>lirc
class AFX_EXT_CLASS MsgRouteSideUpdate : public BaseMsg
{
	DECLARE_CLBTH(MsgRouteSideUpdate)

public:
	MsgRouteSideUpdate();
	virtual ~MsgRouteSideUpdate();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	STATION_NO      nStationNo; //站号
	ROUTE_LIST_HANDLE nHandle;  //需更改的进路序列
	ROUTE_LIST_ID   nListID;    // 要更改的进路序列号
	SIDE_ID       nSideID;   //要更改的股道
};
DECLARE_SERIALIZE_OPERATOR(MsgRouteSideUpdate)

// 进路按钮更改gpc/stpc====>lirc
class AFX_EXT_CLASS MsgRouteButtonUpdate : public BaseMsg
{
	DECLARE_CLBTH(MsgRouteButtonUpdate)

public:
	MsgRouteButtonUpdate();
	virtual ~MsgRouteButtonUpdate();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	STATION_NO      nStationNo;   //站号
	ROUTE_LIST_HANDLE nHandle;  //需更改的进路序列
	ROUTE_LIST_ID   nListID;    // 要更改的进路序列号
	IBYTE       nButtonCount; //按钮数
	BUTTON_ID     nButtonID[MAX_ROUTE_BUTTON_COUNT];  //按钮ID
	BUTTON_NAME   strButtonName[MAX_ROUTE_BUTTON_COUNT];  //按钮名称
	enum ROUTE_TYPE { INVALID=0, TRAIN, SHUNT };  //进路类型：无效、列车、调车
	ROUTE_TYPE    nRouteType; //进路类型
	ROUTE_ID      nNewRouteID;  // 新的进路
};
DECLARE_SERIALIZE_OPERATOR(MsgRouteButtonUpdate)

// 调车进路避让列车车次更改gpc/stpc====>lirc,
class AFX_EXT_CLASS MsgRouteGiveWayUpdate : public BaseMsg
{
	DECLARE_CLBTH(MsgRouteGiveWayUpdate)

public:
	MsgRouteGiveWayUpdate();
	virtual ~MsgRouteGiveWayUpdate();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	STATION_NO      nStationNo; //站号
	ROUTE_LIST_HANDLE nHandle;  //需更改的进路序列
	ROUTE_LIST_ID   nListID;    // 要更改的进路序列号
	ROUTE_LIST_ID   nGiveWayTo;   // 新的避让进路
};
DECLARE_SERIALIZE_OPERATOR(MsgRouteGiveWayUpdate)

// 进路自动触发设置
class AFX_EXT_CLASS MsgRouteTrigFlagUpdate : public BaseMsg
{
	DECLARE_CLBTH(MsgRouteTrigFlagUpdate)

public:
	MsgRouteTrigFlagUpdate();
	virtual ~MsgRouteTrigFlagUpdate();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	STATION_NO      nStationNo; //站号
	ROUTE_LIST_HANDLE nHandle;  //需更改的进路序列
	ROUTE_LIST_ID   nListID;    // 要更改的进路序列号
	ROUTE_TRIG_TYPE   nTrigFlag;    // 新进路的触发类型(0:人工触发, 1:自动触发)
};
DECLARE_SERIALIZE_OPERATOR(MsgRouteTrigFlagUpdate)

// 进路组合更改gpc/stpc====>lirc
class AFX_EXT_CLASS MsgRouteGroupUpdate : public BaseMsg
{
	DECLARE_CLBTH(MsgRouteGroupUpdate)

public:
	MsgRouteGroupUpdate();
	virtual ~MsgRouteGroupUpdate();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	STATION_NO      nStationNo; // 站号
	ROUTE_LIST_HANDLE nHandle;    //需更改的进路序列
	ROUTE_LIST_ID   nListID;    // 要更改的进路
	ROUTE_ID      nNewRouteID;  // 新的预定义进路ID
	ROUTE_ID      nzRoutes[MAX_ROUTE_COUNT_IN_ONE_PLAN];  // 新的预定义组合包含的进路
	IWORD       nRouteCount;  // 新的预定义组合包含的进路条数
};
DECLARE_SERIALIZE_OPERATOR(MsgRouteGroupUpdate)


// 进路插入 (无用)
class AFX_EXT_CLASS MsgRouteInsert : public BaseMsg
{
	DECLARE_CLBTH(MsgRouteInsert)

public:
	MsgRouteInsert();
	virtual ~MsgRouteInsert();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	STATION_NO      nStationNo;//站号
	ROUTE_LIST_HANDLE nHandle;//需更改的进路序列
	TRAIN_NO      tnTrainNo;//车次号
	IBYTE       nButtonCount;//按钮数
	BUTTON_ID     nButtonID[MAX_ROUTE_BUTTON_COUNT];//按钮ID号
	BUTTON_NAME   strButtonName[MAX_ROUTE_BUTTON_COUNT];//按钮名称
	enum ROUTE_TYPE { INVALID=0, TRAIN, SHUNT };//0表示无效、列车、调车
	ROUTE_TYPE    nRouteType;//进路类型
	ROUTE_ID      nNewRouteID;  // 新的进路
	ROUTE_LIST_ID   nNewPos;    // 插入的相对位置(在nNewPos之后,若nNewPos为0,插入到首位)
	SHUNT_GOU_INDEX   nGouIndex;    // 调车勾序(列车进路时为0)
	ROUTE_LIST_ID   nGiveWayTo;   // 避让进路
	ROUTE_TRIG_TYPE   nTrigFlag;    // 新进路的触发类型(0:人工触发, 1:自动触发)
	IWORD       nOperID;    // 操作序号(由消息发送方生成)
};
DECLARE_SERIALIZE_OPERATOR(MsgRouteInsert)

// 进路删除gpc/stpc====>lirc
class AFX_EXT_CLASS MsgRouteDelete : public BaseMsg
{
	DECLARE_CLBTH(MsgRouteDelete)

public:
	MsgRouteDelete();
	virtual ~MsgRouteDelete();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	STATION_NO      nStationNo;//站号
	ROUTE_LIST_HANDLE nHandle;//需修改的进路序列
	ROUTE_LIST_ID   nListID;//进路列表ID
};
DECLARE_SERIALIZE_OPERATOR(MsgRouteDelete)

// 进路顺序调整（没用）
class AFX_EXT_CLASS MsgRouteMove : public BaseMsg
{
	DECLARE_CLBTH(MsgRouteMove)

public:
	MsgRouteMove();
	virtual ~MsgRouteMove();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	STATION_NO      nStationNo; //站号
	ROUTE_LIST_HANDLE nHandle;  //需修改的进路序列
	ROUTE_LIST_ID   nListID;    // 需移动的进路
	ROUTE_LIST_ID   nNewPos;    // 移动到的位置(在nNewPos之后,若nNewPos为0,移动到首位)
};
DECLARE_SERIALIZE_OPERATOR(MsgRouteMove)

// 进路人工触发gpc/stpc====>lirc
class AFX_EXT_CLASS MsgRouteTrig : public BaseMsg
{
	DECLARE_CLBTH(MsgRouteTrig)

public:
	MsgRouteTrig();
	virtual ~MsgRouteTrig();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	STATION_NO      nStationNo;//站号
	ROUTE_LIST_HANDLE nHandle;//需修改的进路序列
	ROUTE_LIST_ID   nListID;//需修改的进路序列ID
	I_TIME        tmTrigTime;//触发时间
	IBYTE       bForceFlag;   // 是否强制触发(不进行计划冲突检查)
};
DECLARE_SERIALIZE_OPERATOR(MsgRouteTrig)

// 调车进路组触发gpc/stpc====>lirc
class AFX_EXT_CLASS MsgRouteGroupTrig : public BaseMsg
{
	DECLARE_CLBTH(MsgRouteGroupTrig)

public:
	MsgRouteGroupTrig();
	virtual ~MsgRouteGroupTrig();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	STATION_NO      nStationNo;//站号
	ROUTE_LIST_HANDLE nHandle;//需修改的进路序列
	IWORD       nGroupID;//进路组ID
	I_TIME        tmTrigTime;//触发时间
	IBYTE       bForceFlag;   // 是否强制触发(不进行计划冲突检查)
};
DECLARE_SERIALIZE_OPERATOR(MsgRouteGroupTrig)

// 进路操作回应
class AFX_EXT_CLASS MsgRouteOperResult : public BaseMsg
{
	DECLARE_CLBTH(MsgRouteOperResult)

public:
	MsgRouteOperResult();
	virtual ~MsgRouteOperResult();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
public:
	bool    CanForceOper() const;
	void    SetForceOperFlag( bool b=true );
	ROUTE_OPER_RESULT GetResult();

public:
	STATION_NO      nStationNo;//站号
	ROUTE_LIST_HANDLE nHandle;//操作的进路序列
	ROUTE_LIST_ID   nListID;//进路序列ID
	enum OPER_TYPE
	{
		ROUTE_INSERT=0, //进路插入
		ROUTE_DELETE, //进路删除
		ROUTE_UPDATE, //进路更新
		ROUTE_MOVE, //进路移动
		ROUTE_TRIG  //进路触发
	};
	OPER_TYPE     nOperType;//操作类型
	ROUTE_OPER_RESULT nResult;//进路操作结果
	char        strComment[MAX_RESULT_COMMENT_LEN]; // 结果说明
	IBYTE       nCommentLen;    // 结果说明的长度
};
DECLARE_SERIALIZE_OPERATOR(MsgRouteOperResult)

// 进路组操作回应
class AFX_EXT_CLASS MsgRouteGroupOperResult : public BaseMsg
{
	DECLARE_CLBTH(MsgRouteGroupOperResult)

public:
	MsgRouteGroupOperResult();
	virtual ~MsgRouteGroupOperResult();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	STATION_NO      nStationNo; //站号
	ROUTE_LIST_HANDLE nHandle;  //操作的进路
	IWORD       nGroupID; //进路组ID
	enum OPER_TYPE
	{
		ROUTE_INSERT=0, //进路插入
		ROUTE_DELETE, //进路删除
		ROUTE_UPDATE, //进路更新
		ROUTE_MOVE, //进路移动
		ROUTE_TRIG  //进路触发
	};
	OPER_TYPE     nOperType;  //操作类型
	ROUTE_OPER_RESULT nResult;  //进路操作结果
	char        strComment[MAX_RESULT_COMMENT_LEN]; // 结果说明
	IBYTE       nCommentLen;    // 结果说明的长度
	IBYTE       nCanForceOper;    // 是否可以强制
};
DECLARE_SERIALIZE_OPERATOR(MsgRouteGroupOperResult)

// 进路长表示信息
class AFX_EXT_CLASS MsgRouteLongInfo : public BaseMsg
{
	DECLARE_CLBTH(MsgRouteLongInfo)

public:
	MsgRouteLongInfo();
	virtual ~MsgRouteLongInfo();

protected:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	STATION_NO        nStationNo;          //站号
	ROUTE_LIST_HANDLE nHandle;             //需修改的进路
	ROUTE_LIST_ID     nListID;             //进路ID
	ROUTE_LIST_ID     nPos;                // 位置,在nPos之后(若nPos为0, 则在首位)
	TRAIN_NO          tnTrainNo;           //车次号
	IBYTE             nButtonCount;        //按钮数
	BUTTON_ID         nButtonID[MAX_ROUTE_BUTTON_COUNT];  //按钮ID
	ROUTE_ID          nRouteID;            //进路ID
	ROUTE_TYPE        nType;               //进路类型
	SIDE_ID           nSideID;             //股道ID
	ENTRY_ID          nEntryID;            //入口号
	ENTRY_ID          nExitID;             //出口号
	SHUNT_GOU_INDEX   nGouIndex;           //调车组索引
	SHUNT_GOU_TYPE    nGouType;            //调车组类型
	ROUTE_LIST_ID     nGiveWayTo;          //避让进路
	ROUTE_STATUS      nStatus;             //进路状态
	I_TIME            tmStart;             //开始时间
	TIME_INTERVAL     tOccupy;             //占用时间
	IWORD             nGroupID;            //组ID
};
DECLARE_SERIALIZE_OPERATOR(MsgRouteLongInfo)

// 进路长表示信息新lirc====>gpc/stpc
class AFX_EXT_CLASS MsgRouteLongInfoNew : public MsgRouteLongInfo
{
	DECLARE_CLBTH(MsgRouteLongInfoNew)

public:
	MsgRouteLongInfoNew();
	virtual ~MsgRouteLongInfoNew();

protected:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	I_TIME        tmPlan;       // 计划执行时间
	TRAIN_INDEX   nTrainIndex;  // 列车的train_index
	IBYTE         nRecIndex;    // 列车的rec_index
	char          strDesc[MAX_DMIS_STR_LEN];    // 进路文字描述
};
DECLARE_SERIALIZE_OPERATOR(MsgRouteLongInfoNew)


// 进路短表示信息(当只有ROUTE_STATUS改变时发送短表示)lirc====>gpc/stpc
class AFX_EXT_CLASS MsgRouteShortInfo : public BaseMsg
{
	DECLARE_CLBTH(MsgRouteShortInfo)

public:
	MsgRouteShortInfo();
	virtual ~MsgRouteShortInfo();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	STATION_NO      nStationNo;   //站号
	ROUTE_LIST_HANDLE nHandle;    //进路
	ROUTE_LIST_ID   nListID;      //进路ID
	ROUTE_STATUS    nStatus;      //进路状态
};
DECLARE_SERIALIZE_OPERATOR(MsgRouteShortInfo)

// 进路表示删除lirc====>gpc/stpc
class AFX_EXT_CLASS MsgRouteInfoDel : public BaseMsg
{
	DECLARE_CLBTH(MsgRouteInfoDel)

public:
	MsgRouteInfoDel();
	virtual ~MsgRouteInfoDel();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	STATION_NO      nStationNo;   //站号
	ROUTE_LIST_HANDLE nHandle;    //需删除的进路
	ROUTE_LIST_ID   nListID;      //进路ID
};
DECLARE_SERIALIZE_OPERATOR(MsgRouteInfoDel)

//没用
class AFX_EXT_CLASS MsgRouteInfoMove : public BaseMsg
{
	DECLARE_CLBTH(MsgRouteInfoMove)

public:
	MsgRouteInfoMove();
	virtual ~MsgRouteInfoMove();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	STATION_NO      nStationNo;  //站号
	ROUTE_LIST_HANDLE nHandle;   //需移动的进路
	ROUTE_LIST_ID   nMoveID;     //移动后的进路ID
	ROUTE_LIST_ID   nPrevID;     //移动前的进路ID
};
DECLARE_SERIALIZE_OPERATOR(MsgRouteInfoMove)

// 进路表示刷新lirc====>gpc/stpc
class AFX_EXT_CLASS MsgRouteInfoRefresh : public BaseMsg
{
	DECLARE_CLBTH(MsgRouteInfoRefresh)
public:
	MsgRouteInfoRefresh();
	virtual ~MsgRouteInfoRefresh();

private:
	virtual HRCODE doSerialize(Archive& ar);

public:
	STATION_NO      nStationNo;
	ROUTE_LIST_HANDLE nHandle;
};
DECLARE_SERIALIZE_OPERATOR(MsgRouteInfoRefresh)

// 进路优先级比较 （没用）lirc====>gpc/stpc
class AFX_EXT_CLASS MsgRouteCompare : public BaseMsg
{
	DECLARE_CLBTH(MsgRouteCompare)
public:
	MsgRouteCompare();
	virtual ~MsgRouteCompare();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	STATION_NO  nStationNo;         // 站号
	enum { MAX_ROUTE_COUNT = 16 };
	struct
	{
		TRAIN_NO tnTrain;                           // 车次号
		ROUTE_TRAIN_TYPE nRunType;                  // 接发车类型
		ROUTE_LIST_ID nListID;                      // 进路序号
		char strDesc[64];                           // 进路说明
	}       Routes[MAX_ROUTE_COUNT];
	IBYTE         nRouteCount;                    // Routes数组元素个数
	TIME_INTERVAL nTimeout;                       // 超时时间
	IBYTE         nResult;                        // 人工指示结果
	IBYTE         nzPriority[MAX_ROUTE_COUNT];    // 优先顺序

	enum
	{
		RS_NOW,         // 现在办理
		RS_DELAY,       // 延迟办理
		RS_MANUAL,      // 手工办理
		RS_SYS_DEFAULT  // 按系统默认
	};
};
DECLARE_SERIALIZE_OPERATOR(MsgRouteCompare)


// ----------------------- 自律机之间、自律机与Baselevel之间的消息 ------------------------------------------------------------

// 码位全体
class AFX_EXT_CLASS MsgBitblock : public BaseMsg
{
	DECLARE_CLBTH(MsgBitblock)

public:
	MsgBitblock();
	virtual ~MsgBitblock();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	STATION_NO    nStationNo;               //站号
	IWORD     nBitblockLen;                 //码位长度
	IBYTE     szBitblock[MAX_BITBLOCK_LEN]; //码位
};
DECLARE_SERIALIZE_OPERATOR(MsgBitblock)

// 码位变化
class AFX_EXT_CLASS MsgIndiChange : public BaseMsg
{
	DECLARE_CLBTH(MsgIndiChange)

public:
	MsgIndiChange();
	virtual ~MsgIndiChange();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	STATION_NO    nStationNo;//站号
	IWORD     nIndiChangeCount;//码位变化数
	INDI_CHANGE   szIndiChange[MAX_INDI_PER_MSG];//码位变化
};
DECLARE_SERIALIZE_OPERATOR(MsgIndiChange)

// 主备状态报告
class AFX_EXT_CLASS MsgMasterStatus : public BaseMsg
{
	DECLARE_CLBTH(MsgMasterStatus)

public:
	MsgMasterStatus();
	virtual ~MsgMasterStatus();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	STATION_NO    nStationNo; //站号
	IBYTE     nMasterFlag;    // 主备标志(0:主机, 1:备机)
	IBYTE     nABFlag;      // AB机标志(0:A机, 1:B机)
};
DECLARE_SERIALIZE_OPERATOR(MsgMasterStatus)

// 自律机状态报告(发送MMI监控程序)
class AFX_EXT_CLASS MsgLiRCStatusReport : public BaseMsg
{
	DECLARE_CLBTH(MsgLiRCStatusReport)

public:
	MsgLiRCStatusReport();
	virtual ~MsgLiRCStatusReport();

private:
	virtual HRCODE doSerialize(Archive& ar);

public:
	STATION_NO    nStationNo;//站号
	WORK_STATUS nNetStatus;//网络状态
	IBYTE     bHasCISConn;//联锁是否连接
	IBYTE     bHasBoardConn;//comm板是否连接
	IBYTE     bHasSimuConn;//模拟是否连接
	WORK_STATUS nCISStatus[2];//联锁通信状态
	IDWORD      nDIBStatus[2];//DIB通信状态
	IDWORD      nDOBStatus[2];//DOB通信状态
	IWORD     nDIBStart;//DIB开始板数
	IWORD     nDIBCount;//DIB板数
	IWORD     nDOBStart;//DOB开始板数
	IWORD     nDOBCount;//DOB板数
	WORK_STATUS nSimuStatus;//模拟状态
	WORK_STATUS nWirelessStatus;//无线状态
};
DECLARE_SERIALIZE_OPERATOR(MsgLiRCStatusReport)

// 自律机状态报告(发送电务维修机)
class AFX_EXT_CLASS MsgLiRCStatusReportNew : public BaseMsg
{
	DECLARE_CLBTH(MsgLiRCStatusReportNew)

public:
	MsgLiRCStatusReportNew();
	virtual ~MsgLiRCStatusReportNew();

private:
	virtual HRCODE doSerialize(Archive& ar);

public:
	STATION_NO    nStationNo;
	enum
	{
		ST_MASTER = 0x01,
		ST_SLAVE  = 0x10
	};
	IBYTE     nMasterStatus;    // 主备状态

	enum
	{
		ST_MCA    = 0x01,
		ST_MCB    = 0x10
	};
	IBYTE     nABFlag;      // AB机标志

	enum
	{
		WORK_ON = 0x01,
		WORK_OFF  = 0x10,
		NO_CONN = 0x11
	};
	IBYTE     nNetStatus;     // 网络状态
	IBYTE     nStbyStatus;      // 热备串口状态
	IBYTE     nInternalStatus;    // 内部进程状态
	IBYTE     nCISStatus[2];    // 联锁通信状态
	IBYTE     nStccStatus[2];   // 列控通信状态
	IBYTE     nWirelessStatus;    // 无线通信状态
	IWORD     nDIBCount;      // 板数
	IDWORD      nDIBStatus[2];    // 板状态
	IBYTE     nOtherStatus[8];    // 预留
};
DECLARE_SERIALIZE_OPERATOR(MsgLiRCStatusReportNew)


class AFX_EXT_CLASS MsgCTCOutput : public BaseMsg
{
	DECLARE_CLBTH(MsgCTCOutput)

public:
	MsgCTCOutput();
	virtual ~MsgCTCOutput();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	STATION_NO    nStationNo;//站号
	I_TIME      tm;//
	IBYTE     nCtrlID;//
	IBYTE     nHandleCount;//
	short     nOutputHandles[MAX_CTC_OUTPUT_HANDLE_COUNT];//
	short     nInputHandles[MAX_CTC_OUTPUT_HANDLE_COUNT];//
};
DECLARE_SERIALIZE_OPERATOR(MsgCTCOutput)

// 预计出发时间
class AFX_EXT_CLASS MsgTrainDepartTime : public BaseMsg
{
	DECLARE_CLBTH(MsgTrainDepartTime)
public:
	MsgTrainDepartTime();
	virtual ~MsgTrainDepartTime();

private:
	virtual HRCODE doSerialize(Archive& ar);

public:
	STATION_NO    nStationNo;//站号
	TRAIN_INDEX   nTrainIndex;//车次索引
	TRAIN_NO    tnTrainNo;//车次号
	I_TIME      tTime;  //出发时间
	IBYTE     nTimeSource;//
};
DECLARE_SERIALIZE_OPERATOR(MsgTrainDepartTime)

// 车次号操作
class AFX_EXT_CLASS MsgIPTrainOper : public BaseMsg
{
	DECLARE_CLBTH(MsgIPTrainOper)
public:
	MsgIPTrainOper();
	virtual ~MsgIPTrainOper();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	enum TOP_TYPE { INIT, COMMON, SYNC, EXTERNAL };
	TOP_TYPE  nType;
	IBYTE nzInfo[128];
	IWORD nInfoLen;
};
DECLARE_SERIALIZE_OPERATOR(MsgIPTrainOper)

// 邻站传递元素CTC状态
class AFX_EXT_CLASS MsgIconCTCStatus : public BaseMsg
{
	DECLARE_CLBTH(MsgIconCTCStatus)
public:
	MsgIconCTCStatus();
	virtual ~MsgIconCTCStatus();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	IBYTE nzInfo[128];//状态内容
	IWORD nInfoLen;//内容长度
};
DECLARE_SERIALIZE_OPERATOR(MsgIconCTCStatus)

// --------------------------- 报警消息 --------------------------------------------------------------------

class AFX_EXT_CLASS MsgAlarm : public BaseMsg
{
	DECLARE_CLBTH(MsgAlarm)

public:
	MsgAlarm();
	virtual ~MsgAlarm();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	virtual I_BOOL NeedRespond();

public:
	ALARM_TYPE    nType;     // 系统报警或行车报警
	IBYTE         nVerbose;  // 报警级别(1-3级)
	ALARM_ID      nAlarmID;  // 报警类别
	I_TIME        nTime;     // 报警时间
	char          szContent[MAX_ALARM_CONTENT_LEN]; // 报警文字内容
	IWORD         nLen;                             // 文字内容长度
};
DECLARE_SERIALIZE_OPERATOR(MsgAlarm)

class AFX_EXT_CLASS MsgAlarmEx : public MsgAlarm
{
	DECLARE_CLBTH(MsgAlarmEx)

public:
	MsgAlarmEx();
	virtual ~MsgAlarmEx();

public:
	virtual I_BOOL NeedRespond();

public:
};
DECLARE_SERIALIZE_OPERATOR(MsgAlarmEx)

class AFX_EXT_CLASS MsgAlarmExd: public MsgAlarm
{
	DECLARE_CLBTH(MsgAlarmExd)

public:
	MsgAlarmExd();
	MsgAlarmExd(const MsgAlarm &msg, ENTITY_ID src);
	virtual ~MsgAlarmExd();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	virtual I_BOOL NeedRespond();

public:
	ENTITY_ID   source;

};
DECLARE_SERIALIZE_OPERATOR(MsgAlarmExd)

// 报警注册消息
class AFX_EXT_CLASS MsgAlarmReg : public BaseMsg
{
	DECLARE_CLBTH(MsgAlarmReg)

public:
	MsgAlarmReg();
	virtual ~MsgAlarmReg();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	ENTITY_ID   nSrcEntityID;   // 报警源实体号
	enum ReqStatus
	{
		LOAD,
		CLOSE
	};
	ReqStatus  nStatus;         // 注册或关闭注册
};
DECLARE_SERIALIZE_OPERATOR(MsgAlarmReg)

// 报警查询消息
class AFX_EXT_CLASS MsgAlarmReq : public BaseMsg
{
	DECLARE_CLBTH(MsgAlarmReq)

public:
	MsgAlarmReq();
	virtual ~MsgAlarmReq();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	ENTITY_ID   nSrcEntityID;   // 报警源实体号 NULL_ENTITY--全体
	//  ALARM_TYPE    nType;        // 系统报警或行车报警 -1--全体
	//  IBYTE     nVerbose;     // 报警级别(1-3级) -1--全体
	//  ALARM_ID    nAlarmID;     // 报警类别 -1--全体
	I_TIME      nStartTime;     // 查询时间范围开始
	I_TIME      nEndTime;     // 查询时间范围结束
};
DECLARE_SERIALIZE_OPERATOR(MsgAlarmReq)

// --------------------------- 登录消息 --------------------------------------------------------------------
class AFX_EXT_CLASS MsgLogin : public BaseMsg
{
	DECLARE_CLBTH(MsgLogin)

public:
	MsgLogin();
	virtual ~MsgLogin();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	IWORD        nStationCount;                      // 站号数
	STATION_NO   Stations[LOGIN_MAX_STATION_COUNT];  // 站号列表
	IWORD        nEntityCount;                       // 实体数
	ENTITY_ID    Entities[LOGIN_MAX_ENTITY_COUNT];   // 实体号列表
	IBYTE        nMasterFlag;        // 主备标志(0:主机, 1:备机)
	IBYTE        nABFlag;            // AB机标志(0:A机, 1:B机)
};                              
DECLARE_SERIALIZE_OPERATOR(MsgLogin)

class AFX_EXT_CLASS MsgLoginReply : public BaseMsg
{
	DECLARE_CLBTH(MsgLoginReply)

public:
	MsgLoginReply();
	virtual ~MsgLoginReply();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	LOG_REPLY_TYPE  nReplyType;     // 登录回应号
};
DECLARE_SERIALIZE_OPERATOR(MsgLoginReply)

//车务终端数据请求信息
class AFX_EXT_CLASS MsgDataStyleReq:public BaseMsg
{
	DECLARE_CLBTH(MsgDataStyleReq)

public:
	MsgDataStyleReq();
	virtual ~MsgDataStyleReq();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	enum DataStyle
	{
		DISPLAY     = 0x01,
		RENTID      = 0x02,
		REPORTTIME  = 0x04,
		BITBLOCK    = 0x08,
		LIRCSTATUS  = 0x10,
		ABSBLOCK    = 0x20,
		ABSCIRCUIT  = 0x40,
		IP_EVENT    = 0x80
	};
	IBYTE      nStatCount;                       //状态数
	STATION_NO nStno[LOGIN_MAX_STATION_COUNT];   //站号
	IDWORD     nStyle[LOGIN_MAX_STATION_COUNT];  //请求类型
};
DECLARE_SERIALIZE_OPERATOR(MsgDataStyleReq)


// inform connect status of two entities 通信连接状态
class AFX_EXT_CLASS MsgEntityConnStatus: public BaseMsg
{    
	DECLARE_CLBTH(MsgEntityConnStatus)

public:
	MsgEntityConnStatus();
	virtual ~MsgEntityConnStatus();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	ENTITY_ID s_server;        //服务端实体号
	ENTITY_ID s_client;        //客户端实体号
	IWORD     board_no;        //板子数
	IBYTE     nABFlag;         //A/B机标示
	enum ConnectStatus {       //联接状态
		ENTITY_CONNECT,          //联接建立
		ENTITY_CLOSE,            //联接关闭
		ENTITY_DATA_LOST         //丢包
	} status;
};
DECLARE_SERIALIZE_OPERATOR(MsgEntityConnStatus)

class AFX_EXT_CLASS MsgGpcAccessInfo: public BaseMsg
{    //  step 0: GPC send usercode and job_id...
	DECLARE_CLBTH(MsgGpcAccessInfo)

public:
	MsgGpcAccessInfo();
	virtual ~MsgGpcAccessInfo();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	USERNAME  user_code;   //用户号码
	JOB_ID    job_id;      //工作id
	GPC_ID    gpc_id;      //gpc实体号
	CONID     con_id;      //调度台号
	enum ACCESS_STEP{
		WANT_PWD,            //输入密码
		PWD_OK               //密码确认
	} step;
};
DECLARE_SERIALIZE_OPERATOR(MsgGpcAccessInfo)

class AFX_EXT_CLASS MsgGpcAccessReply: public BaseMsg
{    // cad reply with name/pwd and status
	DECLARE_CLBTH(MsgGpcAccessReply)

public:
	MsgGpcAccessReply();
	virtual ~MsgGpcAccessReply();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
public:
	USERNAME   user_code;          //用户号码
	JOB_ID     job_id;             //工作id
	GPC_ID     gpc_id;             //gpc实体号
	CONID      con_id;             //调度台号
	USERNAME   user_name;          //用户名
	PASSWORD   pwd;                //密码
	char       os_identify[4];
	char       sys_version[16];

	enum ACCESS_STATUS {                   //登陆状态
		GPC_ACCESS_USERID_OK,                //用户号正确
		GPC_ACCESS_SERVER_READY,             //服务器拒绝
		GPC_ACCESS_USERID_NOT_EXIST,         //用户号不存在
		GPC_ACCESS_INVALID_GPC_ID,           //无效的gpc号
		GPC_ACCESS_JOBID_NOT_EXIST,          //工作id不存在
		GPC_ACCESS_CONID_NOT_EXIST,          //调度台号不存在
		GPC_ACCESS_GPCID_ALREADY_ONLINE,     //gpc号已经登陆
		GPC_ACCESS_USERID_CONID_DISMATCH,    //用户号和调度台号不匹配
		GPC_ACCESS_USERID_JOBID_DISMATCH,    //用户号和工作id不匹配
		GPC_LOGIN_CHECK,                     //登陆确认
		GPC_LOGIN_CHECK_OK,                  //登陆确认成功
		GPC_LOGIN_CHECK_FAIL,                //登陆确认失败
		GPC_ACCESS_UNKNOWN_MESSAGE  }        //未知消息
	status;
};
DECLARE_SERIALIZE_OPERATOR(MsgGpcAccessReply)

class AFX_EXT_CLASS MsgGpcLogin : public BaseMsg
{
	DECLARE_CLBTH(MsgGpcLogin)

public:
	MsgGpcLogin();
	virtual ~MsgGpcLogin();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	enum  USER_RIGHT {//用户权限
		READ_ONLY,//只读
		READ_WRITE//读写
	};
	IWORD     sta_count;//用户有权限的车站数
	STATION_NO    sta_list[LOGIN_MAX_STATION_COUNT];//用户有权限的车站列表

	IWORD     entity_count;//用户有权限的实体数
	ENTITY_ID   entity_list[LOGIN_MAX_ENTITY_COUNT];//用户有权限的实体列表
	USER_RIGHT    user_right[LOGIN_MAX_ENTITY_COUNT];//用户的权限

	CONID     con_id; //调度台号
	IBYTE     job_id;//工作id
	USERNAME    user_name;//用户名

};
DECLARE_SERIALIZE_OPERATOR(MsgGpcLogin)

// report time and report echo
class AFX_EXT_CLASS MsgReportTime: public BaseMsg
{
	DECLARE_CLBTH(MsgReportTime)

public:
	MsgReportTime();
	virtual ~MsgReportTime();

	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	CONID         con_id;       //调度台号
	TRAIN_NO      train_id;     //车次号
	TRAIN_INDEX   train_index;  //车次索引
	IBYTE         rec_index;    //记录索引
	TRAIN_KEY     train_key;    //车次关键字
	enum OPER{
		STPC_REPORT_TIME,         //车站报点
		STPC_UPDATE_TIME,         //车站更新报点
		AUTO_REPORT_TIME,         //自动报点
		GPC_CONFIRM_REPORT,       //gpc确认报点
		REPORT_ECHO }  oper;
		enum RUN_ATTRIB {           //运行方式描述
			ARRIVE,               //到达
			DEPART,               //出发
			PASS }    run_attrib; //通过
			enum TRAIN_ATTRIB {        //车次描述
				FAKE_TRAIN,           //假车次
				TRUE_TRAIN            //真车次
			}     train_attrib;

			STATION_NO station;       //站号
			STATION_NO from_st;       //上一站
			STATION_NO to_st;         //下一站
			ENTRY_ID   entry;         //入口 -1 not used
			ENTRY_ID   exit;          //出口
			SIDE_ID    side;          //股道
			I_TIME     ltime;         // report time (second)
			I_TIME     delay_time;    // delay time (second)
};
DECLARE_SERIALIZE_OPERATOR(MsgReportTime)

// change train_id reported from LIRC to cad
class AFX_EXT_CLASS MsgReportTrainId: public BaseMsg
{
	DECLARE_CLBTH(MsgReportTrainId)

public:
	MsgReportTrainId();
	virtual ~MsgReportTrainId();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	TRAIN_NO    old_train_id;//老车次号
	TRAIN_NO    new_train_id;//新车次号
	TRAIN_KEY   train_key;   //车次关键字
	STATION_NO  station;     //车站

	enum OPER{
		MSG_CHANGE_TRAIN_ID,//变更车次消息
		MSG_REMOVE_TRAIN_ID,//删除车次消息
		MSG_INSERT_TRAIN_ID} oper;//插入车次消息
};
DECLARE_SERIALIZE_OPERATOR(MsgReportTrainId)

// send work schedule
class AFX_EXT_CLASS MsgStPlanSendOrder: public BaseMsg
{  // GPC orders CAD to send plan
	DECLARE_CLBTH(MsgStPlanSendOrder)

public:
	MsgStPlanSendOrder();
	virtual ~MsgStPlanSendOrder();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	CONID       con_id;//调度台号

	USERNAME    sender_name;//发送者
	I_TIME        begin_time;//计划开始时间
	I_TIME        end_time;//计划结束时间
	I_TIME        send_time;//发送时间

	ENTITY_ID   st;
	short       prev_sta_count;
	short       next_sta_count;
	enum StPlanEchoMode {
		PLAN_NO_ECHO,
		PLAN_EACH_ECHO,
		PLAN_TOTAL_ECHO
	}     st_echo;

	STATION_NO  station;//站号
	STATION_NO  station_dmis; //在dmis系统中的站号
	short   count;//
	IBYTE   entry[MAX_STPLAN_DSTN_CNT]; //入口 -1 both
	IBYTE   exit[MAX_STPLAN_DSTN_CNT];  //出口 -1 both
	IBYTE   yard[MAX_STPLAN_DSTN_CNT];  // -1 both
	IBYTE   train_type[MAX_STPLAN_DSTN_CNT];  //车次类型 -1 both, 0 passenger 1 freight
};
DECLARE_SERIALIZE_OPERATOR(MsgStPlanSendOrder)

class AFX_EXT_CLASS MsgStPlanStatusRequest: public BaseMsg
{ // GPC requests all plans' recv status from CAD
	DECLARE_CLBTH(MsgStPlanStatusRequest)

public:
	MsgStPlanStatusRequest();
	virtual ~MsgStPlanStatusRequest();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	STATION_NO  station;//站号
	CONID       con_id;//调度台号
	IBYTE        st_cnt;//请求的目标数
	ENTITY_ID   st[MAX_STPLAN_DSTN_CNT];          // destination
};
DECLARE_SERIALIZE_OPERATOR(MsgStPlanStatusRequest)

class AFX_EXT_CLASS MsgStPlanStatusAnswer: public BaseMsg
{   //  CAD answers MsgStPlanStatusRequest to GPC
	DECLARE_CLBTH(MsgStPlanStatusAnswer)

public:
	MsgStPlanStatusAnswer();
	virtual ~MsgStPlanStatusAnswer();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	CONID       con_id;//调度台号
	STATION_NO  station;//站号
	ENTITY_ID   dest;//目标实体号
	enum PLAN_RECV_STATUS {
		PLAN_RECV,//计划接收
		PLAN_SIGN,//计划签收
		PLAN_SENDING,//计划正在发送
		PLAN_NO_CHANGE,//计划没变化
		PLAN_NO_DATA,//计划无数据
		PLAN_ENTITY_DEAD,//实体无响应
		PLAN_CONFIG_WRONG,//配置错误
		PLAN_INIT//计划初始化
	} recv_status;
	I_TIME        begin_time;//计划开始时间
	I_TIME        end_time;//计划结束时间
	I_TIME    send_time;//计划发送时间
	I_TIME    recv_time;//计划接收时间
	USERNAME  sender_name;//发送者名字
	USERNAME    recver_name;//接收者名字
};
DECLARE_SERIALIZE_OPERATOR(MsgStPlanStatusAnswer)

class AFX_EXT_CLASS MsgStPlanRequest: public BaseMsg
{    // STPC/LIRC request stplan
	DECLARE_CLBTH(MsgStPlanRequest)

public:
	MsgStPlanRequest();
	virtual ~MsgStPlanRequest();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	STATION_NO  station;//站号
	CONID       con_id;//调度台号
	I_TIME        begin_time;//请求计划开始时间
	I_TIME        end_time;//请求计划结束时间
	enum StPlanRequestMode {
		PLAN_FROM_TIME,
		PLAN_FROM_ORDER
	} mode;
	enum StPlanRequestType{
		PLAN_SENDED,
		PLAN_RECVED,
		PLAN_ALL
	} data_type;
	enum StPlanRequestConId{
		PLAN_ALL_CON,//请求所有调度台计划
		PLAN_ONE_CON//请求1个调度台计划
	} request_con;
};
DECLARE_SERIALIZE_OPERATOR(MsgStPlanRequest)

// 计划下达开始消息  CAD sends plan init to STPC/LIRC
class AFX_EXT_CLASS MsgStPlanSendInit: public BaseMsg
{    
	DECLARE_CLBTH(MsgStPlanSendInit)

public:
	MsgStPlanSendInit();
	virtual ~MsgStPlanSendInit();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	STATION_NO  station;      //站号
	CONID       con_id;       //调度台号
	IBYTE       flag;         // keep for future
	ENTITY_ID   gpc;          //发送者编码
	USERNAME    sender_name;  //发送者名字
	I_TIME      send_time;    //发送时间
	enum StPlanEchoMode {
		PLAN_NO_ECHO,
		PLAN_EACH_ECHO,
		PLAN_TOTAL_ECHO
	}     send_echo;          //应答回馈要求
};
DECLARE_SERIALIZE_OPERATOR(MsgStPlanSendInit)

class AFX_EXT_CLASS MsgStPlanInsert: public BaseMsg
{    // CAD send plan data(insert or change) to STPC/LIRC
	DECLARE_CLBTH(MsgStPlanInsert)

public:
	MsgStPlanInsert();
	virtual ~MsgStPlanInsert();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	CONID          con_id;                   // 调度台号码
	ENTITY_ID      gpc;                      // 发送者编码
	TRAIN_INDEX    train_index;              // 列车索引
	IBYTE          rec_index;                // 记录索引
	STATION_NO     station;                  // 车站站号
	I_TIME         arrive;                   // 本站到达  0 为未下达
	I_TIME         depart;                   // 出发时间  0 为未下达
	I_TIME         plan_arrive;              // 图定到达
	I_TIME         plan_depart;              // 图定出发
	ENTRY_ID       entry;                    // 入口
	ENTRY_ID       exit;                     // 出口
	SIDE_ID        arrive_side;              // 股道
	SIDE_ID        depart_side;              // 股道
	IWORD          min_stop_time;            // 最小停站时间

	short          prev_sta_count;           //前方站数
	STATION_NO     prev_sta[8];              //前方站站号
	ENTRY_ID       prev_entry[8];            //前方站入口
	ENTRY_ID       prev_exit[8];             //前方站出口
	SIDE_ID        prev_side[8];             //前方站股道
	I_TIME         prev_arrive[8];           //前方站到达时间
	I_TIME         prev_depart[8];           //前方站离开时间
	IBYTE          prev_status[8];           //前方站状态
	IBYTE          prev_flag[8];             //

	short          next_sta_count;           //后方站数
	STATION_NO     next_sta[8];              //后方站站号
	ENTRY_ID       next_entry[8];            //后方站入口
	ENTRY_ID       next_exit[8];             //后方站出口
	SIDE_ID        next_side[8];             //后方站股道
	I_TIME         next_arrive[8];           //后方站到达时间
	I_TIME         next_depart[8];           //后方站离开时间
	IBYTE          next_status[8];           //后方站状态
	IBYTE          next_flag[8];             //

	TRAIN_NO       arrive_train_id;                     //到达车次
	TRAIN_NO       depart_train_id;                     //出发车次
	IBYTE          engine_count;                        //机车个数
	TRAIN_NO       engine_id[MAX_ATTACH_ENGINE_COUNT];  //机车号码
	IBYTE          engine_type[MAX_ATTACH_ENGINE_COUNT];
	// 机车类型
	// bit 7-6:  from dispatcher/subao
	// bit 5-4:  0 - traction  1 - attach
	// bit 3-0:  0 - diesel    1 - elec
	IWORD          early_depart_time;      // 是否可以提前发车
	// 0: can not early depart
	// else: the time span could be taken early
	//   -1: no limit
	IBYTE           status;                //   是否计划或者实际点
	// bit 0:  0 : arrival time is plan
	//         1 : arrival time is actual
	// bit 1:  0 : depart time is plan
	//         1 : depart time is actual
	// bit 2:  0  arrive time is actual by manul
	//          1  arrive time is actual by auto
	// bit 3:  0  arrive time is actual by manul
	//          1  arrive time is actual by auto
	// bit 4:  0  without tech stop
	//          1  tech stop 技术停车
	// bit 5:  0  无电力牵引
	//          1  电力牵引
	// bit 6:  1: 压前 0: 正常
	// bit 7:  1: 压后 0: 正常

	IBYTE           stop_condition;        // 停车条件；
	// bit 0:    3.0不卡控基本图股道
	// bit 1:    defined pass                 原规定通过
	// bit 2:    cannot stop without plan     无计划不得停车
	// bit 3 :   can not use other side       不得变更股道
	// bit 4 :   cannot pass by non-main line 不得侧线通过
	// bit 5 :   cannot auto make arrive route
	// bit 6 :   cannot auto make depart route
	// bit 7 :   0 make depart route by meet,  1 make depart route as plan

	IBYTE           run_fast_slow_time;    // 区间运缓时间
	// bit 7:   0   run too fast
	//          1   run too slow
	// bit 6-0: time (minutes)
	IBYTE           flag;                  // 本站交出、接入、始发、终到
	// bit  2-0:
	//     001 - give out this train 交出
	//     010 - terminal this train终到
	//     011 - end this train, temporily terminal
	//     100 - keep this train
	//     101 - relay this train
	//     110 - unite this train
	//     111 - divide this train
	// bit  4-3:    01 - accept this train接入
	//              10 - start this train始发
	// bit 5-6:  珠三角城际折返类型 原地折返 站后折返 
	// bit 7:    3.0不卡控基本图出入口
	IBYTE           type;                  //  列车类型
	//  detailed train type  (bit 7: 0 - passenger 1 -freight)
	IBYTE           run_type;              //  main type : EXPRESS_P_TRAIN, FAST_P_TRAIN,
	//         NORMAL_P_TRAIN, PACK_F_TRAIN,
	//         FIVE_F_TRAIN,  NORMAL_F_TRAIN,
	IBYTE           space_rank;            // 列车超限等级；
	// 0: no out of space, else: rank of space
	IBYTE           length_rank;           // 列车超长等级；
	// 0: no out of length, else: rank of length
	IBYTE           super_rank;            // 重点列车等级；
	// 0: not super_train, else: rank of super_train
	IBYTE           military_rank;         // 军运列车等级；
	// 0: not military_train, else: rank of military_train
	IBYTE           disconn_count;         // 甩车个数；
	IBYTE           conn_count;            // 挂车个数；

	IWORD           train_length;          //车长
	IWORD           train_weight;          //车重

	char            notes[64];             //文字记事

	IBYTE           meet_count;            //会让列车个数
	TRAIN_INDEX     meet_train_index[16];  //会让列车编码
	IBYTE           meet_rec_index[16];    //会让列车顺序
	enum            MEET_STATUS {
		MEET_SAME_DIR,
		MEET_DIFF_DIR
	}              meet_status[16];
};
DECLARE_SERIALIZE_OPERATOR(MsgStPlanInsert)

// 计划下达删车消息  CAD send plan data(delete) to STPC/LIRC
class AFX_EXT_CLASS MsgStPlanDelete: public BaseMsg
{       
	DECLARE_CLBTH(MsgStPlanDelete)

public:
	MsgStPlanDelete();
	virtual ~MsgStPlanDelete();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	ENTITY_ID     gpc;                   // 发送者实体号
	CONID         con_id;                // 调度台号
	STATION_NO    station;               // 站号                                        
	TRAIN_INDEX   train_index;           // 车次索引
	IBYTE         rec_index;             // 记录索引
	TRAIN_NO      arrive_train_id;       // 到达车次
	TRAIN_NO      depart_train_id;       // 出发车次
	I_TIME        arrive;                // 本站到达  0 为未下达
	I_TIME        depart;                // 出发时间  0 为未下达
};
DECLARE_SERIALIZE_OPERATOR(MsgStPlanDelete)

// 计划下达结束消息  CAD send plan complete to STPC/LIRC
class AFX_EXT_CLASS MsgStPlanSendComplete: public BaseMsg
{ 
	DECLARE_CLBTH(MsgStPlanSendComplete)

public:
	MsgStPlanSendComplete();
	virtual ~MsgStPlanSendComplete();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	CONID         con_id;            //调度台号
	ENTITY_ID     gpc;               //发送者实体号
	STATION_NO    station;           //站号
	IBYTE         insert_cnt;        //插入车次数
	IBYTE         delete_cnt;        //删除车次数
};
DECLARE_SERIALIZE_OPERATOR(MsgStPlanSendComplete)

// STPC/LIRC send CAD recv status of each plan
class AFX_EXT_CLASS MsgStPlanTrainRecv: public BaseMsg
{   
	DECLARE_CLBTH(MsgStPlanTrainRecv)

public:
	MsgStPlanTrainRecv();
	virtual ~MsgStPlanTrainRecv();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	STATION_NO     station;//站号
	CONID          con_id;//调度台号
	TRAIN_INDEX    train_index;//车次索引
	IBYTE           rec_index;//记录索引
	enum RUN_ATTRIB {//运行方式描述
		ARRIVE,//到达
		DEPART,//离开
		PASS   //通过 
	}    attrib;
};
DECLARE_SERIALIZE_OPERATOR(MsgStPlanTrainRecv)

// 计划完整下达签收消息 STPC/LIRC send recv all msg to CAD, then CAD relays to GPC
class AFX_EXT_CLASS MsgStPlanRecvALL: public BaseMsg
{     
	DECLARE_CLBTH(MsgStPlanRecvALL)

public:
	MsgStPlanRecvALL();
	virtual ~MsgStPlanRecvALL();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	ENTITY_ID    gpc;          //计划发送者实体号
	ENTITY_ID    st;           //计划接受者实体号
	STATION_NO   station;      //站号
	CONID        con_id;       //调度台号
	enum RECV_STATUS{          //接收状态
		RECV,                    //接收
		SIGN,                    //签收
		WRONG                    //错误
	}           status;      
	I_TIME        send_time;   //计划发送时间
	I_TIME        recv_time;   //计划接收时间
	USERNAME      recver_name; //计划接收者名字
};
DECLARE_SERIALIZE_OPERATOR(MsgStPlanRecvALL)

// 列车基本图下达
class AFX_EXT_CLASS MsgBaseTrainPlan: public BaseMsg
{     
  DECLARE_CLBTH(MsgBaseTrainPlan)

public:
  MsgBaseTrainPlan();
  virtual ~MsgBaseTrainPlan();

private:
  virtual HRCODE doSerialize(Archive& ar);
protected:
  virtual size_t countObjectLength();

public:
    STATION_NO station;
    enum { MAX_PLAN_COUNT = 100 };
    struct base_plan
    {
    	TRAIN_NO			arrive_train;
    	TRAIN_NO			depart_train;
    	SIDE_ID				side;
    	ENTRY_ID			entry;
    	ENTRY_ID			exit;
    	STATION_NO			prev_station;
    	STATION_NO			next_station;
    	int					arrive_time; //seconds since 0:00:00 in a day
    	int					depart_time; //same as above
		char                reserve[8];
		base_plan()
		{
			memset(arrive_train, 0, sizeof(arrive_train));
			memset(depart_train, 0, sizeof(depart_train));
			side=0;entry=0;exit=0;prev_station=0;next_station=0;
			arrive_time=0;
			depart_time=0;
			memset(reserve, 0, sizeof(reserve));
		}
    } base_plan_record[MAX_PLAN_COUNT];
    IBYTE plan_count;
};
DECLARE_SERIALIZE_OPERATOR(MsgBaseTrainPlan)

class AFX_EXT_CLASS MsgShuntNoteInit: public BaseMsg
{        //  begin sending one note
	DECLARE_CLBTH(MsgShuntNoteInit)

public:
	MsgShuntNoteInit();
	virtual ~MsgShuntNoteInit();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	enum SOURCE_TYPE{ INVALID=0, SERVER, LIRC };//来源：无效为0,服务器,自律机
	SOURCE_TYPE   nSourceType;//来源
	ENTITY_ID    source_entity;//源实体
	SHUNT_INDEX  shunt_index;//调车单索引
	STATION_NO   station;//站号
	I_TIME         send_time;//发送时间
	USERNAME     sender_name;//发送者名字
};
DECLARE_SERIALIZE_OPERATOR(MsgShuntNoteInit)

class AFX_EXT_CLASS MsgShuntNoteComplete: public BaseMsg
{    //  fishish sending one note
	DECLARE_CLBTH(MsgShuntNoteComplete)

public:
	MsgShuntNoteComplete();
	virtual ~MsgShuntNoteComplete();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	enum SOURCE_TYPE{ INVALID=0, SERVER, LIRC };//消息来源：无效为0,服务器,自律机
	SOURCE_TYPE   nSourceType;//消息来源
	ENTITY_ID    source_entity;//源实体号
	SHUNT_INDEX  shunt_index;// 调车单索引
	STATION_NO   station;//站号
	I_TIME         send_time;//发送时间
	USERNAME     sender_name; //发送者名字
};
DECLARE_SERIALIZE_OPERATOR(MsgShuntNoteComplete)

class AFX_EXT_CLASS MsgShuntNoteSingleGouInit: public BaseMsg
{        //  begin sending one note
	DECLARE_CLBTH(MsgShuntNoteSingleGouInit)

public:
	MsgShuntNoteSingleGouInit();
	virtual ~MsgShuntNoteSingleGouInit();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	enum SOURCE_TYPE{ INVALID=0, SERVER, LIRC };//消息来源:无效为0,服务器,自律机
	SOURCE_TYPE   nSourceType;//消息来源
	ENTITY_ID    source_entity;//源实体
	SHUNT_INDEX  shunt_index;//调车单索引
	IWORD   shunt_gou_index;//调车勾索引
	STATION_NO   station;//站号
	I_TIME         send_time;//发送时间
	USERNAME     sender_name;//发送者名字
};
DECLARE_SERIALIZE_OPERATOR(MsgShuntNoteSingleGouInit)

class AFX_EXT_CLASS MsgShuntNoteSingleGouComplete: public BaseMsg
{    //  fishish sending one note
	DECLARE_CLBTH(MsgShuntNoteSingleGouComplete)

public:
	MsgShuntNoteSingleGouComplete();
	virtual ~MsgShuntNoteSingleGouComplete();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	enum SOURCE_TYPE{ INVALID=0, SERVER, LIRC };//消息来源:无效为0,服务器,自律机
	SOURCE_TYPE   nSourceType;//消息来源
	ENTITY_ID    source_entity;//源实体
	SHUNT_INDEX  shunt_index; //调车单索引
	IWORD   shunt_gou_index;//调车勾索引
	STATION_NO   station;//站号
	I_TIME         send_time;//发送时间
	USERNAME     sender_name; //发送者名字
};
DECLARE_SERIALIZE_OPERATOR(MsgShuntNoteSingleGouComplete)

class AFX_EXT_CLASS MsgShuntNoteGpcOperEcho: public BaseMsg
{    //  fishish sending one note
	DECLARE_CLBTH(MsgShuntNoteGpcOperEcho)

public:
	MsgShuntNoteGpcOperEcho();
	virtual ~MsgShuntNoteGpcOperEcho();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	SHUNT_INDEX  shunt_index;//调车单索引
	STATION_NO   station;//站号
	enum OPER_TYPE {
		OPER_INSERT_ONE,

		OPER_INSERT_HEAD,
		OPER_UPDATE_HEAD,
		OPER_DELETE_HEAD,

		OPER_INSERT_GOU,//插入勾
		OPER_UPDATE_GOU,//更新勾
		OPER_DELETE_GOU,//删除勾

		OPER_INSERT_ROUTE,//插入进路
		OPER_UPDATE_ROUTE,//更新进路
		OPER_DELETE_ROUTE,//删除进路

		OPER_UPDATE_STORECAR,
		OPER_DELETE_STORECAR,

		OPER_UPDATE_PICTURE,
		OPER_DELETE_PICTURE,

		OPER_INSERT_STATUS,
		OPER_UPDATE_STATUS,
	}            oper;
	enum OPER_ECHO {
		OPER_SUCCESS,//操作成功
		OPER_FAILED//操作失败
	}            echo;
};
DECLARE_SERIALIZE_OPERATOR(MsgShuntNoteGpcOperEcho)

// shuntnote data
class AFX_EXT_CLASS MsgShuntNoteSendOrder: public BaseMsg
{     // GPC orders CAD to send ShuntNote
	DECLARE_CLBTH(MsgShuntNoteSendOrder)

public:
	MsgShuntNoteSendOrder();
	virtual ~MsgShuntNoteSendOrder();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	SHUNT_INDEX    shunt_index;//调车单索引
	STATION_NO     station;//站号
	I_TIME           send_time;//发送时间
	USERNAME       sender_name;   //发送者名字
	IBYTE           entity_cnt;//实体数
	ENTITY_ID      st_entity[MAX_SHUNTNOTE_DSTN_CNT];       // destination
};
DECLARE_SERIALIZE_OPERATOR(MsgShuntNoteSendOrder)

class AFX_EXT_CLASS MsgShuntNoteRecvALL: public BaseMsg
{    // STPC/LIRC send CAD recv status of shuntnote
	DECLARE_CLBTH(MsgShuntNoteRecvALL)

public:
	MsgShuntNoteRecvALL();
	virtual ~MsgShuntNoteRecvALL();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	SHUNT_INDEX     shunt_index;//调车单索引
	enum SHUNTNOTE_SEND_STATUS {//调车记录发送状态
		SHUNT_RECEIVE,//接收
		SHUNT_SIGNATURE,//签收
		SHUNT_SENDING,//发送中
		SHUNT_ENTITY_DEAD,//实体无响应
		SHUNT_NOT_SEND,//未发送
		SHUNT_RECV_OUTDATE,//接收过期
		SHUNT_INIT//初始化
	} status;

	I_TIME            send_time;//发送时间
	I_TIME            recv_time;//接收时间
	USERNAME    sender_name;//发送者名字
	USERNAME        recver_name;  //接收者名字
	STATION_NO      station;//站号
	ENTITY_ID       gpc;//gpc实体号
	ENTITY_ID       st;//车站实体号
};
DECLARE_SERIALIZE_OPERATOR(MsgShuntNoteRecvALL)

class AFX_EXT_CLASS MsgShuntNoteSingleGouRecvALL: public BaseMsg
{    // STPC/LIRC send CAD recv status of shuntnote
	DECLARE_CLBTH(MsgShuntNoteSingleGouRecvALL)

public:
	MsgShuntNoteSingleGouRecvALL();
	virtual ~MsgShuntNoteSingleGouRecvALL();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	SHUNT_INDEX     shunt_index;//调车单索引
	IWORD     shunt_gou_index;//调车勾索引
	enum GOU_SEND_STATUS {//勾计划发送状态
		GOU_RECEIVE,//接收
		GOU_SIGNATURE,//签收
		GOU_SENDING,//发送中
		GOU_ENTITY_DEAD,//实体无响应
		GOU_NOT_SEND,//未发送
		GOU_RECV_OUTDATE,//接收过期
		GOU_INIT//初始化
	} status;

	I_TIME            send_time;//发送时间
	I_TIME            recv_time;//接收时间
	USERNAME    sender_name;//发送者名字
	USERNAME        recver_name;//接收者名字
	STATION_NO      station;//站号
	ENTITY_ID       gpc;//gpc实体号
	ENTITY_ID       st;//车站实体号
};
DECLARE_SERIALIZE_OPERATOR(MsgShuntNoteSingleGouRecvALL)

class AFX_EXT_CLASS MsgShuntNoteDelete: public BaseMsg
{        // GPC orders CAD to delete one note
	DECLARE_CLBTH(MsgShuntNoteDelete)

public:
	MsgShuntNoteDelete();
	virtual ~MsgShuntNoteDelete();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	SHUNT_INDEX     shunt_index;         //调车单索引
	STATION_NO      station;             //站号
	enum DELETE_ITEMS{                   //删除子项
		SHUNTNOTE_ALL,                     //删除所有
		SHUNTNOTEGOU,                      //删除勾
		SHUNTNOTEROUTELIST,                //删除进路
		SHUNTNOTESTORECAR}  delete_items;  //删除存储车
};
DECLARE_SERIALIZE_OPERATOR(MsgShuntNoteDelete)

class AFX_EXT_CLASS MsgShuntNoteInsert: public BaseMsg
{        // GPC orders CAD to Insert one note
	DECLARE_CLBTH(MsgShuntNoteInsert)

public:
	MsgShuntNoteInsert();
	virtual ~MsgShuntNoteInsert();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	SHUNT_INDEX     shunt_index;//调车单索引
	STATION_NO      station;  //站号
};
DECLARE_SERIALIZE_OPERATOR(MsgShuntNoteInsert)

class AFX_EXT_CLASS MsgShuntNoteUpdate: public BaseMsg
{        // GPC orders CAD to Update one note
	DECLARE_CLBTH(MsgShuntNoteUpdate)

public:
	MsgShuntNoteUpdate();
	virtual ~MsgShuntNoteUpdate();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	SHUNT_INDEX     shunt_index;//调车单索引
	STATION_NO      station;  //站号
	enum UPDATE_ITEMS{//更新子项
		SHUNTNOTEHEAD,//调车记录头
		SHUNTNOTEGOU,//调车记录勾
		SHUNTNOTEROUTELIST,//调车记录进路
		SHUNTNOTESTORECAR,//调车记录存储车
		SHUNTNOTESTATUS//调车记录状态
	} update_items;
};
DECLARE_SERIALIZE_OPERATOR(MsgShuntNoteUpdate)

class AFX_EXT_CLASS MsgShuntNoteListRequest: public BaseMsg
{  //  GPC request shuntnote list from cad
	DECLARE_CLBTH(MsgShuntNoteListRequest)

public:
	MsgShuntNoteListRequest();
	virtual ~MsgShuntNoteListRequest();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	STATION_NO   station;//站号
	I_TIME         begin_time;//调车记录请求开始时间
	I_TIME         end_time;//调车记录请求接收时间
};
DECLARE_SERIALIZE_OPERATOR(MsgShuntNoteListRequest)

class AFX_EXT_CLASS MsgShuntNoteListInit: public BaseMsg
{        //  CAD begin sending note list
	DECLARE_CLBTH(MsgShuntNoteListInit)

public:
	MsgShuntNoteListInit();
	virtual ~MsgShuntNoteListInit();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	enum SOURCE_TYPE{ INVALID=0, SERVER, LIRC };//消息来源：无效为0,服务器,自律机
	SOURCE_TYPE   nSourceType;//消息来源
	STATION_NO   station;//站号
};
DECLARE_SERIALIZE_OPERATOR(MsgShuntNoteListInit)

class AFX_EXT_CLASS MsgShuntNoteListComplete: public BaseMsg
{    //  CAD fishish sending note list
	DECLARE_CLBTH(MsgShuntNoteListComplete)

public:
	MsgShuntNoteListComplete();
	virtual ~MsgShuntNoteListComplete();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	enum SOURCE_TYPE{ INVALID=0, SERVER, LIRC };//消息来源：无效为0,服务器,自律机
	SOURCE_TYPE   nSourceType;//消息来源
	STATION_NO   station;//站号
	IWORD   count;//调车记录数
};
DECLARE_SERIALIZE_OPERATOR(MsgShuntNoteListComplete)

class AFX_EXT_CLASS MsgShuntNoteRequest: public BaseMsg
{     //  GPC request one shuntnote from cad
	DECLARE_CLBTH(MsgShuntNoteRequest)

public:
	MsgShuntNoteRequest();
	virtual ~MsgShuntNoteRequest();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:

	STATION_NO   station;//站号
	SHUNT_INDEX    shunt_index;//调车单索引
};
DECLARE_SERIALIZE_OPERATOR(MsgShuntNoteRequest)

class AFX_EXT_CLASS MsgShuntNoteStatus: public BaseMsg
{    // send MsgShuntNoteStatus between cad and gpc
	DECLARE_CLBTH(MsgShuntNoteStatus)

public:
	MsgShuntNoteStatus();
	virtual ~MsgShuntNoteStatus();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	SHUNT_INDEX     shunt_index;//调车单索引
	STATION_NO    station;//站号
	ENTITY_ID        st_entity;//车站实体号
	enum SHUNTNOTE_SEND_STATUS{//调车记录发送状态
		SHUNT_RECEIVE,//接收
		SHUNT_SIGNATURE,//签收
		SHUNT_SENDING,//发送中
		SHUNT_ENTITY_DEAD,//实体无响应
		SHUNT_NOT_SEND,//未发送
		SHUNT_INIT//初始化
	} status;
	I_TIME            send_time;//发送时间
	I_TIME            recv_time;//接收时间
	USERNAME    sender_name;//发送者名字
	USERNAME        recver_name;//接收者名字
};
DECLARE_SERIALIZE_OPERATOR(MsgShuntNoteStatus)

class AFX_EXT_CLASS MsgShuntNoteHead: public BaseMsg
{           //  调车作业通知单表头信息
	DECLARE_CLBTH(MsgShuntNoteHead)

public:
	MsgShuntNoteHead();
	virtual ~MsgShuntNoteHead();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	enum SOURCE_TYPE{ INVALID=0, SERVER, LIRC };//消息来源：无效为0,服务器,自律机
	SOURCE_TYPE   nSourceType;                //消息来源
	SHUNT_INDEX   shunt_index;             //  调车单唯一索引
	char          shunt_note_name[24];     //  第___调车单
	STATION_NO    station;                 //  编制车站
	IBYTE          shunt_gou_count;         //  调车钩个数
	IBYTE          flag;                    //  bit 0:  0 - 无图，1- 有图(本务机车作业)
	I_TIME          start_time;              //  调车作业通知单作业起始时间
	I_TIME          end_time;                //  调车作业通知单作业结束时间
	TRAIN_NO      train_id;                //  被作业车次号
	ENGINE_NO     engine_id;               //  作业机车号或者车次号
	TRAIN_INDEX   train_index;             //  预留
	I_TIME          edit_time;               //  编写时间
	USERNAME      shunt_group_name;        //  调车组姓名
	USERNAME      shunt_leader_name;       //  调车领导者姓名
	char          shunt_comment[64];       //  备注
	enum   SHUNTNOTE_STATUS {
		SHUNTNOTE_EDIT,
		SHUNTNOTE_SENDED,
		SHUNTNOTE_COMPLETE
	}     shunt_status;
	enum OPER_STATUS {
		OPER_NOT_START = 0,
		OPER_START,
		OPER_FINISH
	}     oper_status;
};
DECLARE_SERIALIZE_OPERATOR(MsgShuntNoteHead)

class AFX_EXT_CLASS MsgShuntNoteGou: public BaseMsg
{      //  调车作业通知单钩信息
	DECLARE_CLBTH(MsgShuntNoteGou)

public:
	MsgShuntNoteGou();
	virtual ~MsgShuntNoteGou();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	enum SOURCE_TYPE{ INVALID=0, SERVER, LIRC };
	SOURCE_TYPE   nSourceType;
	SHUNT_INDEX  shunt_index;           //  调车单唯一索引
	STATION_NO station;                    //  车站
	IWORD  shunt_gou_index;            //  调车钩唯一索引
	IWORD  shunt_gou_type;             //  调车勾类型
	//    BIT7-0: 0 挂,1 甩, 2本务机挂头,3 本务机摘头
	//    BIT10,8: 牵出线编号,7表示用区间调车,
	//              0表示不用牵出线
	//    BIT11: 是否影响正线
	//    BIT12~13:勾执行状态: 0 未开始执行, 1 已开始执行, 2 执行完成
	SIDE_ID   side;                     //  作业股道
	IBYTE      direction;                //  作业方向
	IWORD    shunt_car_count;          //  作业车辆数
	IWORD    shunt_note_route_count;   //  进路个数
	IWORD    shunt_work_time;          //  调车进路作业时间
	char      gou_comment[30];          //  备注
	SIDE_NAME szTrackQCX;   //  牵出线
};
DECLARE_SERIALIZE_OPERATOR(MsgShuntNoteGou)

class AFX_EXT_CLASS MsgShuntNoteRouteList: public BaseMsg
{    //  调车作业通知单进路信息
	DECLARE_CLBTH(MsgShuntNoteRouteList)

public:
	MsgShuntNoteRouteList();
	virtual ~MsgShuntNoteRouteList();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	SHUNT_INDEX   shunt_index;           //  调车单唯一索引
	STATION_NO  station;                    //  车站
	IWORD   shunt_gou_index;            //  调车钩索引
	IWORD   shunt_route_index;          //  调车钩进路顺序（索引）
	IWORD   route_id;                   //  进路标识
	IWORD   shunt_route_occ_time;       //  调车进路占用时间
	IWORD   flag;                       //  预留
	I_TIME     start_time;                 //  调车进路办理时刻
	IWORD   group_id;     // 进路分组ID
};
DECLARE_SERIALIZE_OPERATOR(MsgShuntNoteRouteList)

class AFX_EXT_CLASS MsgShuntNoteStoreCar: public BaseMsg
{  // 调车作业通知单图片内车站存车的结构，可供编辑
	DECLARE_CLBTH(MsgShuntNoteStoreCar)

public:
	MsgShuntNoteStoreCar();
	virtual ~MsgShuntNoteStoreCar();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	SHUNT_INDEX    shunt_index;//调车单索引
	STATION_NO     station;//站号
	IWORD   count;//站存车数
	IWORD   len[MAX_SHUNTNOTE_STORECAR_CNT];//
	IBYTE     info[MAX_SHUNTNOTE_STORECAR_CNT][MAX_SHUNTNOTE_STORECAR_LEN];//站存车信息
};
DECLARE_SERIALIZE_OPERATOR(MsgShuntNoteStoreCar)

class AFX_EXT_CLASS MsgShuntNotePicture: public BaseMsg
{  // 调车作业通知单图片下达的结构，不可编辑
	DECLARE_CLBTH(MsgShuntNotePicture)

public:
	MsgShuntNotePicture();
	virtual ~MsgShuntNotePicture();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	SHUNT_INDEX    shunt_index;//调车单索引
	STATION_NO     station;//站号
	IWORD   pic_len;
	IWORD   index;
	IWORD   count;
	IBYTE     pic[MAX_SHUNTNOTE_PICPACK_LEN];
};
DECLARE_SERIALIZE_OPERATOR(MsgShuntNotePicture)

// subao info
class AFX_EXT_CLASS MsgSubaoFormat: public BaseMsg
{
	DECLARE_CLBTH(MsgSubaoFormat)

public:
	MsgSubaoFormat();
	virtual ~MsgSubaoFormat();

private:
	virtual HRCODE doSerialize(Archive& ar);
	void init();

protected:
	virtual size_t countObjectLength();

public:
	enum   SUBAO_OPER {//速报操作
		SUBAO_STPC_REPORT,//车务终端上传速报
		SUBAO_GPC_INSERT,//gpc插入速报
		SUBAO_GPC_CHANGE_MISC,//
		SUBAO_GPC_DELETE, //gpc删除速报
		SUBAO_GPC_CHANGE_KEY,//gpc改变关键字
		SUBAO_SERVER_RELAY,//服务器传递速报
		SUBAO_FROM_RADIO//来自无线的速报
	} oper;
	I_TIME          input_time;//输入时间
	SHIFT_ID      shift_id;//日班计划id
	SHIFT_ID      old_shift_id;//老日班计划id
	TRAIN_INDEX   train_index;//车次索引
	TRAIN_INDEX   old_train_index;//老车次索引
	TRAIN_NO      train_id;//车次号
	TRAIN_NO      old_train_id;//老车次号
	STATION_NO    report_sta;                     //上报车站
	CONID         con_id;//调度台号
	IBYTE          schd_type;//运行线类型

	char          car_des[50];                    //列车描述 $
	char          orig[20];                       //来源   @
	char          dest[20];                       //到站   @

	IBYTE          engine_count;                   //bit 7-4 附挂机车个数  3-0 牵引机车个数
	struct         {
		char          engine_type;                //机车型号 $
		ENGINE_NO     engine_id;                  //机车号   @
	} subao_engine [8];

	USERNAME      driver_name;                    //司机 @
	USERNAME      train_leader_name;              //车长 @
	char          cover[10];                      //蓬布 $
	char          tail_no[10];                    //尾号 $

	IBYTE          space_rank;                     //超限级别 $
	IBYTE          length_rank;                    //超长级别 $
	IBYTE          super_rank;                     //专运级别 $
	IBYTE          military_rank;                  //军运级别 $
	IBYTE          speed_limit;                    //限速 $
	IBYTE          e_engine;                       //电力标志  @
	IBYTE          print_flag;                     //是否打印  $
	short         pass_cnt;                       //客车辆数  @
	short         load_cnt;                       //重车辆数  @
	short         pk_count;                       //篷车（空）辆数   $
	short         pz_count;                       //篷车（重）辆数   $
	short         ck_count;                       //敞车（空）辆数   $
	short         cz_count;                       //敞车（重）辆数   $
	short         pik_count;                      //平车（空）辆数   $
	short         piz_count;                      //平车（重）辆数   $
	short         gk_count;                       //罐车（空）辆数   $
	short         gz_count;                       //罐车（重）辆数   $
	short         o_count;                        //其它辆数辆数     $
	short         s_count;                        //守车辆数         $
	short         total_count;                    //合计辆数         @
	short         van_count;                      //蓬布数           $
	short         subao_type;                     //编组类型         $
	short         speed;                          //速度             $
	short         noutil_cnt;                     //运用车数         @
	short         cal_len;                        //记长             @
	ISDWORD          total_weight;                   //全重             @

};
DECLARE_SERIALIZE_OPERATOR(MsgSubaoFormat)

// query subao info
class AFX_EXT_CLASS MsgSubaoQuery: public BaseMsg
{
	DECLARE_CLBTH(MsgSubaoQuery)

public:
	MsgSubaoQuery();
	virtual ~MsgSubaoQuery();

private:
	virtual HRCODE doSerialize(Archive& ar);
	void init();

protected:
	virtual size_t countObjectLength();

public:
	CONID con_id;//调度台号
	IBYTE  schd_type;//运行线类型

	TRAIN_INDEX train_index;//车次索引
	SHIFT_ID    shift_id;//日班计划id
	enum SUBAO_QUERY_FILTER {//速报查询过滤器
		SUBAO_QUERY_BY_TRAININDEX,//按车次索引查询速报
		SUBAO_QUERY_BY_SHIFTID//按日班计划id查询速报
	} query_filter;
};
DECLARE_SERIALIZE_OPERATOR(MsgSubaoQuery)

// gpc消息处理错误反馈
class AFX_EXT_CLASS MsgGpcMsgEcho: public BaseMsg
{
	DECLARE_CLBTH(MsgGpcMsgEcho)
	int m_Version;

public:
	MsgGpcMsgEcho(int version = 0);
	virtual ~MsgGpcMsgEcho();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	ISDWORD msgid;//消息id
	ISDWORD ecode;//
};
DECLARE_SERIALIZE_OPERATOR(MsgGpcMsgEcho)

// 分包数据控制消息
class AFX_EXT_CLASS MsgHugeDataCtrl: public BaseMsg
{
	DECLARE_CLBTH(MsgHugeDataCtrl)

public:
	MsgHugeDataCtrl();
	virtual ~MsgHugeDataCtrl();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	MSGHEAD::msgid_t msgid;                           //消息id
	enum STATUS {MSG_BEGIN, MSG_END, MSG_FAIL} status;//状态：消息开始、消息结束、消息失败
	IDWORD msgpara;                                   //消息参数
};
DECLARE_SERIALIZE_OPERATOR(MsgHugeDataCtrl)

class AFX_EXT_CLASS MsgSimButton: public BaseMsg
{
	DECLARE_CLBTH(MsgSimButton)

public:
	MsgSimButton();
	virtual ~MsgSimButton();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	IWORD nStationNo;//站号
	IBYTE nButtonCount;//按钮数
	IDWORD sButtonInput[10];//按钮
};
DECLARE_SERIALIZE_OPERATOR(MsgSimButton)


class AFX_EXT_CLASS MsgBasicTrainListInit: public BaseMsg
{
	DECLARE_CLBTH(MsgBasicTrainListInit)

public:
	MsgBasicTrainListInit();
	virtual ~MsgBasicTrainListInit();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	ENTITY_ID gpc;//gpc实体号
	CONID   con_id;//调度台号
	STATION_NO  station;//站号
	I_TIME    stime;//
	I_TIME    etime;//
	I_TIME    shift_begin_time;//日班计划中的开始时间
	I_TIME    shift_end_time; //日班计划中的结束时间
	enum  INIT_TRAINTYPE {//车次类型
		INIT_PASSENGER,//客车
		INIT_FREIGHT,//货车
		INIT_ALL_TYPE//客货车
	} train_type;
	enum  INIT_DIRECTION {//运行方向
		INIT_UP,//上行
		INIT_DOWN,//下行
		INIT_ALL_DIR//上下行
	} direction;
	enum  INIT_START_ACCEPT {//
		INIT_START,//
		INIT_ACCEPT,//
		INIT_ALL_SA//
	} flag;
	enum  INIT_HAT_FLAG {//带帽标记
		INIT_NORMAL_TRAIN,//普通车次
		INIT_HAT_TRAIN//带帽车次
	} hat_flag;
};
DECLARE_SERIALIZE_OPERATOR(MsgBasicTrainListInit)

//////////////
class AFX_EXT_CLASS MsgBasicTrainListReply: public BaseMsg
{
	DECLARE_CLBTH(MsgBasicTrainListReply)

public:
	MsgBasicTrainListReply();
	virtual ~MsgBasicTrainListReply();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	TRAIN_INDEX train_index;//车次索引
	TRAIN_NO  train_id;//车次号
	I_TIME    arrive;//到达时间
	I_TIME    depart;//离开时间
	enum BASIC_TRAIN_LIST_TYPE {//基本图车次类型
		BT_PASSENGER,//客车
		BT_FREIGHT,//货车
		BT_ALL_TYPE//客货车
	} type;
	enum BASIC_TRAIN_LIST_DIR {//基本图车次运行方向
		BT_UP,//上行
		BT_DOWN,//下行
		BT_ALL_DIR//上下行
	} dir;
	enum BASIC_TRAIN_LIST_HAT {//基本图中是否带帽
		BT_HAT_TRAIN,//带帽车次
		BT_NORMAL_TRAIN//普通车次
	} hat_train;
	enum BASIC_TRAIN_LIST_EXIST {//基本图是否存在车次
		BT_EXIST,//存在
		BT_NONE//不存在
	} exist_train;
	enum BASIC_TRAIN_LIST_ADD {
		BT_NORMAL_SCHEDULE,
		BT_LONG_STOP,
		BT_LONG_INCREASE
	} add_train;
};
DECLARE_SERIALIZE_OPERATOR(MsgBasicTrainListReply)

// 中心表示请求信息（对Rt的请求）
class AFX_EXT_CLASS MsgDssDisplayOrder: public BaseMsg
{
	DECLARE_CLBTH(MsgDssDisplayOrder)

public:
	MsgDssDisplayOrder();
	virtual ~MsgDssDisplayOrder();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	short sta_count;                             //车站数
	STATION_NO sta_list[LOGIN_MAX_STATION_COUNT];//车站
	enum DSS_ORDER {DSS_INIT, DSS_CLOSE} flag;   //标示：表示初始化,表示关闭
};
DECLARE_SERIALIZE_OPERATOR(MsgDssDisplayOrder)

//////////////
class AFX_EXT_CLASS MsgEntityConnStatusOrder: public BaseMsg
{
	DECLARE_CLBTH(MsgEntityConnStatusOrder)

public:
	MsgEntityConnStatusOrder();
	virtual ~MsgEntityConnStatusOrder();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	short      entity_count;                        //实体数
	short      msg_dispatch[LOGIN_MAX_ENTITY_COUNT];//
	ENTITY_ID  entity_list[LOGIN_MAX_ENTITY_COUNT]; //实体号
	enum CONN_ORDER {CONN_INIT, CONN_CLOSE} flag;   //连接标示：连接初始化、连接关闭
};            
DECLARE_SERIALIZE_OPERATOR(MsgEntityConnStatusOrder)

//////////////   
class AFX_EXT_CLASS MsgTimeSync: public BaseMsg
{
	DECLARE_CLBTH(MsgTimeSync)

public:
	MsgTimeSync();
	virtual ~MsgTimeSync();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	I_TIME ltime;//同步时间
};
DECLARE_SERIALIZE_OPERATOR(MsgTimeSync)
////////////////////////////////////////////////////
// message for trian form: get from tmis
////////////////////////////////////////////////////
// request
class AFX_EXT_CLASS MsgLoadTrainFormRequest: public BaseMsg
{
	DECLARE_CLBTH(MsgLoadTrainFormRequest)

public:
	MsgLoadTrainFormRequest();
	virtual ~MsgLoadTrainFormRequest();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	CONID   con_id;         //调度台号
	GPC_ID    gpc_id;//gpc号
	JOB_ID    job_id;//工作号
	STATION_NO  station_id;       //站号
	ENTITY_ID entily_id;//实体号
	USERNAME  user_code;//用户号
	TRAIN_INDEX train_index;      //车次索引
	TRAIN_NO  train_id;
	enum TRAIN_FORM_LOAD_REQUEST{
		LOAD_ASM_TRAIN_FORM_HEAD = 1,
		LOAD_ASM_TRAIN_FORM = 2,
		LOAD_ASM = 3,
	}request_flag;
};
DECLARE_SERIALIZE_OPERATOR(MsgLoadTrainFormRequest)

////////////////////////////////////////////////////
// train form head information
class AFX_EXT_CLASS MsgLoadAsmTrainFormHead: public BaseMsg
{
	DECLARE_CLBTH(MsgLoadAsmTrainFormHead)

public:
	MsgLoadAsmTrainFormHead();
	virtual ~MsgLoadAsmTrainFormHead();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	CONID   con_id;       //调度台号
	char    con_name[19];//调度台名字
	STATION_NO  station_id;     //站号
	TRAIN_INDEX train_index;      //车次索引
	TRAIN_NO  train_id;//车次号
	TRAIN_INDEX TMIS_train_id;      // tmis train_id
	char    marshing_train_id[20];    // 编组车次
	char    runing_train_id[20];    // 运行车次
	char    marshing_station_code[4]; // 编组站略码
	STATION_NO  marshing_station_id;    // 编组站号
	STATION_NO  shunting_station_id;    // 解体站号
	char    shunting_station_code[4]; // 解体站略码
	I_TIME    plan_time;      // 计划开车时间（14）
	I_TIME    actl_time;      // 实际开车时间（14）
	I_TIME    send_time;      // 下达时间（14）
	short   toal_cnt;     // 车数（3）
	short   cal_len;      // 换长（5）
	short   weight;             // 总重（5）
	short   self_weight;            // 自重（5）
	short   van_cnt;      // 蓬布数合计
	char    reporter[10];     // 发报人（10）
	char    assigner[10];     // 签收人（10）
	char    sg_station_name[10];    // 甩挂站名
	STATION_NO  sg_station_id;      // 甩挂站号
	char    sg_station_code[6];   // 甩挂站代码
	short   weight_1;     // 列车载重1
	short   weight_2;     // 列车载重2
	short   marshing_cnt;     // 编组条数
	IBYTE   ht_flag;      // 首尾标志（1）
	IBYTE   no_use_flag;      // 列非运用标志(1)
	IBYTE   car_no_count_flag;    // 车辆不计数标志(1)
	IBYTE   car_count_flag;     // 车辆计数标志(1)
	IBYTE   self_car_flag;      // 自备车标记(1)
	IBYTE   received_tmis_flag;   // 从TMIS读取flag(1)
	I_TIME    received_tmis_time;   // 从TMIS读取时间
};
DECLARE_SERIALIZE_OPERATOR(MsgLoadAsmTrainFormHead)

////////////////////////////////////////////////////
// train form carriage information
class AFX_EXT_CLASS MsgLoadAsmTrainForm: public BaseMsg
{
	DECLARE_CLBTH(MsgLoadAsmTrainForm)

public:
	MsgLoadAsmTrainForm();
	virtual ~MsgLoadAsmTrainForm();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	CONID   con_id;       //调度台号
	TRAIN_INDEX train_index;      //车次索引
	TRAIN_INDEX TMIS_train_id;//tmis中的车次索引
	short   car_pos;      // 顺序号
	short   car_type_id;      // 车种id号
	CARRIAGE_NO car_no;       // 车号(8)
	IBYTE   tanker_oil_type;    // 罐车油种
	IBYTE   load_flag;      // 载重标志
	IBYTE   car_empty_weight_flag;  //空重标志
	short   car_suttle;     // 自重(4,1)
	short   car_len_cnt;      // 换长(2,1)
	short   car_load;     // 载重
	char    car_type[4];//车种
	char    fisrt_station_code[4];    // 第一到站略码
	char    last_station_code[4];   // 最后到站略码
	STATION_NO  first_station_id;//第一站站号
	STATION_NO  last_station_id;//最后站站号
	char    freight_name[40];   // 货物名称
	char    freight_code[12];   // 货物品名代码
	char    receiver[40];     // 收货人
	char    depart_station_code[4];   // 发站略码
	STATION_NO  depart_station_id;    // 发站id
	short   car_van_cnt;      // 蓬布数
	char    car_note[40];     // 记事
	char    car_type_code[4];   // 车种代码
	char    container_type_1_cnt[4];  // 集装箱类型1
	char    weight_container_1_cnt[4];  // 重集装箱数1
	char    empty_container_1_cnt[4]; // 重集装箱数1
	char    container_type_2_cnt[4];  // 集装箱类型1
	char    weight_container_2_cnt[4];  // 重集装箱数1
	char    empty_container_2_cnt[4]; // 重集装箱数1
	char    first_station_name[12];   // 第一到站名称
	char    last_station_name[12];    // 最后到站名称
	char    depart_station_name[12];  // 发站名称
	char    aim_bureau[4];      // 到局
	char    aim_subbureau[4];   // 到分局
	char    direction_no[4];    // 方向号
	char    frieght_no_use_code[4];   // 品名非运用代码
	char    railway_company_flag[4];  // 路企标志
	char    no_count_flag[4];   // 不计数标志
};
DECLARE_SERIALIZE_OPERATOR(MsgLoadAsmTrainForm)

class AFX_EXT_CLASS MsgLoadTrainFormResponse: public BaseMsg
{
	DECLARE_CLBTH(MsgLoadTrainFormResponse)

public:
	MsgLoadTrainFormResponse();
	virtual ~MsgLoadTrainFormResponse();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	CONID   con_id;         //调度台号
	GPC_ID    gpc_id;       //gpc号
	STATION_NO  station_id;       //站号
	ENTITY_ID entily_id;          //实体号
	TRAIN_INDEX train_index;      //车次索引
	enum TRAIN_FORM_LOAD_RESULT{
		LOAD_ASM_INIT = 0,
		LOAD_ASM_TRAIN_FORM_HEAD_SUCCESS = 1,
		LOAD_ASM_HEAD_SUCC_CAR_FAILED = 2,
		LOAD_ASM_HEAD_FAILED_CAR_SUCC = 3,
		LOAD_ASM_TRAIN_FORM_SUCCESS = 4,
		LOAD_ASM_SUCCESSED = 5,
		LOAD_ASM_TRAIN_FORM_HEAD_FAILED = -1,
		LOAD_ASM_HEAD_FAILD_CAR_SUCCESS = -2,
		LOAD_ASM_HEAD_SUCC_CAR_FALID = -3,
		LOAD_ASM_TRAIN_FORM_FAILED = -4,
		LOAD_ASM_FAILD = -5,
	}load_flag;
};
DECLARE_SERIALIZE_OPERATOR(MsgLoadTrainFormResponse)

////////////////////////////////////////////////////
// message for station store carriage
////////////////////////////////////////////////////
// request
class AFX_EXT_CLASS MsgLoadSscRequest: public BaseMsg
{
	DECLARE_CLBTH(MsgLoadSscRequest)

public:
	MsgLoadSscRequest();
	virtual ~MsgLoadSscRequest();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	CONID   con_id;         //调度台号
	GPC_ID    gpc_id;       //gpc号
	JOB_ID    job_id;     //工作号
	STATION_NO  station_no;       //站号
	ENTITY_ID entily_id;      //实体号
	USERNAME  user_code;      //用户号
	enum SSC_LOAD_REQUEST{
		LOAD_SSC_BLOCK = 1,
		LOAD_SSC_CARRIAGE = 2,
		LOAD_SSC_ALL = 3,
	}request_flag;
};
DECLARE_SERIALIZE_OPERATOR(MsgLoadSscRequest)
// response
class AFX_EXT_CLASS MsgLoadSscResponse: public BaseMsg
{
	DECLARE_CLBTH(MsgLoadSscResponse)

public:
	MsgLoadSscResponse();
	virtual ~MsgLoadSscResponse();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	CONID   con_id;         //调度台号
	GPC_ID    gpc_id;       //gpc号
	STATION_NO  station_no;       //站号
	ENTITY_ID entily_id;          //实体号
	short   block_count;          //
	short   carriage_count;     //
	short   side_count; //股道数
	enum SSC_LOAD_RESPONSE{
		LOAD_SSC_NO_DATA = 0,
		LOAD_SSC_BLOCK_SUCCESS = 1,
		LOAD_SSC_CARRIAGE_SUCCESS = 2,
		LOAD_SSC_ALL_SUCCESS = 3,
		LOAD_SSC_BLOCK_FAILED = -1,
		LOAD_SSC_BLOCK_FAILED_CAR_SUCC = -2,
		LOAD_SSC_BLOCK_SUCC_CAR_FAILED = -3,
		LOAD_SSC_CARRIAGE_FAILED = -4,
		LOAD_SSC_ALL_FAILED = -5,
	}load_flag;
};
DECLARE_SERIALIZE_OPERATOR(MsgLoadSscResponse)

class AFX_EXT_CLASS MsgUpdateSscRequest: public BaseMsg
{
	DECLARE_CLBTH(MsgUpdateSscRequest)

public:
	MsgUpdateSscRequest();
	virtual ~MsgUpdateSscRequest();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	CONID   con_id;         //调度台号
	GPC_ID    gpc_id;       //gpc号
	STATION_NO  station_id;       //站号
	ENTITY_ID entily_id;          //实体号
	short   block_cnt;
	short   car_cnt;  //车数
	enum SSC_UPDATE_FLAG{
		UPDATE_BLOCK = 1,
		UPDATE_CARRIAGE = 2,
		UPDATE_ALL = 3,
	}request_flag;
};
DECLARE_SERIALIZE_OPERATOR(MsgUpdateSscRequest)

class AFX_EXT_CLASS MsgUpdateSscRequestEnd: public BaseMsg
{
	DECLARE_CLBTH(MsgUpdateSscRequestEnd)

public:
	MsgUpdateSscRequestEnd();
	virtual ~MsgUpdateSscRequestEnd();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	CONID   con_id;         //调度台号
	GPC_ID    gpc_id;       //gpc号
	STATION_NO  station_id;       //站号
	ENTITY_ID entily_id;  //实体号
};
DECLARE_SERIALIZE_OPERATOR(MsgUpdateSscRequestEnd)

class AFX_EXT_CLASS MsgUpdateSscResponse: public BaseMsg
{
	DECLARE_CLBTH(MsgUpdateSscResponse)

public:
	MsgUpdateSscResponse();
	virtual ~MsgUpdateSscResponse();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	CONID   con_id;         //调度台号
	GPC_ID    gpc_id;       //gpc号
	STATION_NO  station_id;       //站号
	ENTITY_ID entily_id;    //实体号
};
DECLARE_SERIALIZE_OPERATOR(MsgUpdateSscResponse)

class AFX_EXT_CLASS MsgUpdateSscResponseEnd: public BaseMsg
{
	DECLARE_CLBTH(MsgUpdateSscResponseEnd)

public:
	MsgUpdateSscResponseEnd();
	virtual ~MsgUpdateSscResponseEnd();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	CONID   con_id;         //调度台号
	GPC_ID    gpc_id;     //gpc号
	STATION_NO  station_id;       //站号
	ENTITY_ID entily_id;      //实体号
	short   succ_block_cnt;
	short   succ_carriage_cnt;
	short   failed_block_cnt;
	short   failed_carriage_cnt;
};
DECLARE_SERIALIZE_OPERATOR(MsgUpdateSscResponseEnd)

//////////////////////////////////////////////////////////////
// block
//////////////////////////////////////////////////////////////
// load
class AFX_EXT_CLASS MsgLoadSscBlock: public BaseMsg
{
	DECLARE_CLBTH(MsgLoadSscBlock)

public:
	MsgLoadSscBlock();
	virtual ~MsgLoadSscBlock();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	CONID   con_id;         //调度台号
	GPC_ID    gpc_id; //gpc号
	STATION_NO  station_no;       //站号
	ENTITY_ID entily_id;  //实体号
	short   side_no;  //股道号
	short   index_in_side;//股道索引
	short   car_cnt;  //车数
	IBYTE   ht_flag;  //首尾标志（1）
	IBYTE   oil_type;//油车类型
	IBYTE   car_empty_flag; //是否空车标示
	IBYTE   flag; //标志
	IBYTE   car_type; //车种
	char    block_desc[9];
	char    freight_flag[8];  //货运标示
	CARRIAGE_NO first_car_no;//第一个车号
	CARRIAGE_NO last_car_no;//最后一个车号
	I_TIME    last_edit_time;//最后编辑时间
	char    direction_no[4];//方向号
	char    end_station_code[4];//结束站略码
	STATION_NO  end_station_no;//结束站站号
	short   positon;  //位置
	char    status[4];  //状态
	char    work_status[4]; //工作状态
	char    work_flag[4]; //工作标志
	char    note[8];  //注释
};
DECLARE_SERIALIZE_OPERATOR(MsgLoadSscBlock)
// load
class AFX_EXT_CLASS MsgInsertSscBlock: public BaseMsg
{
	DECLARE_CLBTH(MsgInsertSscBlock)

public:
	MsgInsertSscBlock();
	virtual ~MsgInsertSscBlock();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	CONID   con_id;         //调度台号
	GPC_ID    gpc_id;       //gpc号
	STATION_NO  station_no;       //站号
	ENTITY_ID entily_id;  //实体号
	short   side_no;  //股道号
	short   index_in_side;  //股道索引
	short   car_cnt;  //车数
	IBYTE   ht_flag;  //首尾标志（1）
	IBYTE   oil_type; //油车类型
	IBYTE   car_empty_flag; //是否空车标示
	IBYTE   flag; //标志
	IBYTE   car_type; //车种
	char    block_desc[9];
	char    freight_flag[8];  //货运标示
	CARRIAGE_NO first_car_no; //第一个车号
	CARRIAGE_NO last_car_no;  //最后一个车号
	I_TIME    last_edit_time; //最后编辑时间
	char    direction_no[4];//方向号
	char    end_station_code[4];  //结束站略码
	STATION_NO  end_station_no; //结束站站号
	short   positon;  //位置
	char    status[4];  //状态
	char    work_status[4]; //工作状态
	char    work_flag[4]; //工作标志
	char    note[8];  //注释
};
DECLARE_SERIALIZE_OPERATOR(MsgInsertSscBlock)
// update block detail information
class AFX_EXT_CLASS MsgUpdateSscBlock: public BaseMsg
{
	DECLARE_CLBTH(MsgUpdateSscBlock)

public:
	MsgUpdateSscBlock();
	virtual ~MsgUpdateSscBlock();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	CONID   con_id;         //调度台号
	GPC_ID    gpc_id; //gpc号
	STATION_NO  station_no;       //站号
	ENTITY_ID entily_id;  //实体号
	short   side_no;  //股道号
	short   index_in_side;  //股道索引
	short   car_cnt;  //车数
	IBYTE   ht_flag;  //首尾标志（1）
	IBYTE   oil_type; //油车类型
	IBYTE   car_empty_flag; //是否空车标示
	IBYTE   flag;
	IBYTE   car_type; //车种
	char    block_desc[9];
	char    freight_flag[8];  //货运标示
	CARRIAGE_NO first_car_no; //第一个车号
	CARRIAGE_NO last_car_no;  //最后一个车号
	I_TIME    last_edit_time; //最后编辑时间
	char    direction_no[4];//方向号
	char    end_station_code[4];  //结束站略码
	STATION_NO  end_station_no; //结束站站号
	short   positon;  //位置
	char    status[4];  //状态
	char    work_status[4]; //工作状态
	char    work_flag[4]; //工作标志
	char    note[8];  //注释
};
DECLARE_SERIALIZE_OPERATOR(MsgUpdateSscBlock)
// update block index or porsition
class AFX_EXT_CLASS MsgUpdateSscBlockPos: public BaseMsg
{
	DECLARE_CLBTH(MsgUpdateSscBlockPos)

public:
	MsgUpdateSscBlockPos();
	virtual ~MsgUpdateSscBlockPos();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	CONID   con_id;         //调度台号
	STATION_NO  station_no;       //站号
	short   side_no;  //股道号
	short   index_in_side;  //股道索引
	short   positon;  //位置
};
DECLARE_SERIALIZE_OPERATOR(MsgUpdateSscBlockPos)
// update block status、work_flag or work_status
class AFX_EXT_CLASS MsgUpdateSscBlockStatus: public BaseMsg
{
	DECLARE_CLBTH(MsgUpdateSscBlockStatus)

public:
	MsgUpdateSscBlockStatus();
	virtual ~MsgUpdateSscBlockStatus();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	CONID   con_id;         //调度台号
	STATION_NO  station_no;       //站号
	short   side_no;  //股道号
	short   index_in_side;  //股道索引
	IBYTE   ht_flag;  //首尾标志（1）
	IBYTE   oil_type; //油车类型
	IBYTE   car_empty_flag; //是否空车标示
	IBYTE   flag; //标志
	char    status[4];  //状态
	char    work_status[4]; //工作状态
	char    work_flag[4]; //工作标志
};
DECLARE_SERIALIZE_OPERATOR(MsgUpdateSscBlockStatus)
// update block note or freight_flag
class AFX_EXT_CLASS MsgUpdateSscBlockNote: public BaseMsg
{
	DECLARE_CLBTH(MsgUpdateSscBlockNote)

public:
	MsgUpdateSscBlockNote();
	virtual ~MsgUpdateSscBlockNote();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	CONID   con_id;         //调度台号
	STATION_NO  station_no;       //站号
	short   side_no;//股道号
	short   index_in_side;//股道索引
	char    freight_flag[8];//货运标示
	char    note[8];//注释
};
DECLARE_SERIALIZE_OPERATOR(MsgUpdateSscBlockNote)
// DELETE BLOCK
class AFX_EXT_CLASS MsgDeleteSscBlock: public BaseMsg
{
	DECLARE_CLBTH(MsgDeleteSscBlock)

public:
	MsgDeleteSscBlock();
	virtual ~MsgDeleteSscBlock();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	CONID   con_id;         //调度台号
	STATION_NO  station_no;       //站号
	short   side_no;  //股道号
	short   index_in_side;  //股道索引
};
DECLARE_SERIALIZE_OPERATOR(MsgDeleteSscBlock)
// OPER RESULT
class AFX_EXT_CLASS MsgOperBlockReaponse: public BaseMsg
{
	DECLARE_CLBTH(MsgOperBlockReaponse)

public:
	MsgOperBlockReaponse();
	virtual ~MsgOperBlockReaponse();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	CONID   con_id;         //调度台号
	STATION_NO  station_no;       //站号
	short   side_no;  //股道号
	short   index_in_side;  //股道索引
	enum DEL_UPDATE_SSC_BLOCK_RESULT{//删除更新封锁结果
		NO_FOUND = 0,//没有发现
		UPDATE_DETAIL_SUCCESS = 1,//更新细节成功
		UPDATE_POS_SUCCESS = 2,//更新位置成功
		UPDATE_STATUS_SUCCESS = 3,//更新状态成功
		UPDATE_NOTE_SUCCESS = 4,//更新注释成功
		DELETE_SUCCESS = 5,//删除成功
		INSERT_SUCCESS = 6,//插入成功
		UPDATE_DETAIL_FAILED = -1,//更新细节失败
		UPDATE_POS_FAILED = -2,//更新位置失败
		UPDATE_STATUS_FAILED = -3,//更新状态失败
		UPDATE_NOTE_FAILED = -4,//更新注释失败
		DELETE_BLOCK_FAILED = -5,//删除封锁失败
		INSERT_FAILED = -6,//插入失败
	}del_update_block_rs;
};
DECLARE_SERIALIZE_OPERATOR(MsgOperBlockReaponse)

//////////////////////////////////////////////////////////////
// carriage
//////////////////////////////////////////////////////////////
// load
class AFX_EXT_CLASS MsgLoadSscCarriage: public BaseMsg
{
	DECLARE_CLBTH(MsgLoadSscCarriage)

public:
	MsgLoadSscCarriage();
	virtual ~MsgLoadSscCarriage();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	CONID   con_id;         //调度台号
	STATION_NO  station_no;       //站号
	short   side_no;  //股道号
	short   block_index_in_side;
	short   car_index_in_block;
	CARRIAGE_NO car_no;//车号
	short   car_type_id;//车种id号
	char    car_type[4];//车种
	IBYTE   oil_type; //油车类型
	IBYTE   load_flag;//载重标志
	short   car_suttle;//自重
	short   car_len_cnt;//换长
	short   car_load;//载重
	STATION_NO  first_station_no;//第一到站站号
	char    first_station_code[4];//第一到站略码
	STATION_NO  last_station_no;//最后到站站号
	char    last_station_code[4];//最后到站略码
	char    freight_name[38];//货物名称
	char    freight_code[8];//货物品名代码
	char    receiver[40];//收货人
	char    depart_station_code[4];//发站略码
	STATION_NO  depart_station_no;//发站站号
	short   car_van_cnt;//篷布数
	char    car_note[40];//记事
	char    car_type_code[4];//车种代码
	char    container_type_1_cnt[4];//集装箱类型1
	char    weight_container_1_cnt[4];//重集装箱数1
	char    empty_contrainer_1_cnt[4];//空集装箱数1
	char    container_type_2_cnt[4];//集装箱类型2
	char    weight_container_2_cnt[4];//重集装箱数2
	char    empty_contrainer_2_cnt[4];//空集装箱数2
	char    car_empty_weight_flag[4];//空重标志
	char    first_station_name[12];//第一到站名称
	char    last_station_name[12];//最后到站名称
	char    depart_station_name[12];//发站名称
	char    aim_bureau[4];//到局
	char    aim_subbereau[4];//到分局
	char    direction_no[4];//方向号
	char    freight_no_use_code[4];//货物无用品名代码
	char    rail_company_flag[4];//铁路公司标示
	char    no_count_flag[4];//不计数标志
	char    current_status[4];//当前状态
	I_TIME    arrive_time;//到达时间
	TRAIN_INDEX arrive_train_index;//到达车次索引
	TRAIN_NO  arrive_train_no;//到达车次号
	IBYTE   arrive_flag;//到达标示
	IBYTE   arrive_reason;//到达原因
	I_TIME    work_complete_time;//工作完成时间
	I_TIME    depart_time;//出发时间
	TRAIN_INDEX depart_train_index;//出发车次索引
	IBYTE   depart_flag;//出发标示
};
DECLARE_SERIALIZE_OPERATOR(MsgLoadSscCarriage)
// insert detail carriage information
class AFX_EXT_CLASS MsgInsertSscCarriage: public BaseMsg
{
	DECLARE_CLBTH(MsgInsertSscCarriage)

public:
	MsgInsertSscCarriage();
	virtual ~MsgInsertSscCarriage();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	CONID   con_id;         //调度台号
	STATION_NO  station_no;       //站号
	short   side_no;  //股道号
	short   block_index_in_side;  //
	short   car_index_in_block;   //
	CARRIAGE_NO car_no; //车号
	short   car_type_id;  //车种id号
	char    car_type[4];  //车种
	IBYTE   oil_type; //油车类型
	IBYTE   load_flag;  //载重标志
	short   car_suttle; //自重
	short   car_len_cnt;  //换长
	short   car_load; //载重
	STATION_NO  first_station_no; //第一到站站号
	char    first_station_code[4];  //第一到站略码
	STATION_NO  last_station_no;  //最后到站站号
	char    last_station_code[4]; //最后到站略码
	char    freight_name[38]; //货物名称
	char    freight_code[8];  // 货物品名代码
	char    receiver[40]; //收货人
	char    depart_station_code[4]; //发站略码
	STATION_NO  depart_station_no;  //发站站号
	short   car_van_cnt;  //篷布数
	char    car_note[40]; //记事
	char    car_type_code[4]; //车种代码
	char    container_type_1_cnt[4];  //集装箱类型1
	char    weight_container_1_cnt[4];  //重集装箱数1
	char    empty_contrainer_1_cnt[4];  //空集装箱数1
	char    container_type_2_cnt[4];  //集装箱类型2
	char    weight_container_2_cnt[4];  //重集装箱数2
	char    empty_contrainer_2_cnt[4];  //空集装箱数2
	char    car_empty_weight_flag[4]; //空重标志
	char    first_station_name[12]; //第一到站名称
	char    last_station_name[12];  //最后到站名称
	char    depart_station_name[12];  //发站名称
	char    aim_bureau[4];  //到局
	char    aim_subbereau[4]; //到分局
	char    direction_no[4];//方向号
	char    freight_no_use_code[4]; //货物无用品名代码
	char    rail_company_flag[4]; //铁路公司标示
	char    no_count_flag[4]; //不计数标志
	char    current_status[4];  //当前状态
	I_TIME    arrive_time;  //到达时间
	TRAIN_INDEX arrive_train_index; //到达车次索引
	TRAIN_NO  arrive_train_no;  //到达车次号
	IBYTE   arrive_flag;  //到达标示
	IBYTE   arrive_reason;  //到达原因
	I_TIME    work_complete_time; //工作完成时间
	I_TIME    depart_time;  //出发时间
	TRAIN_INDEX depart_train_index; //出发车次索引
	IBYTE   depart_flag;  //出发标示
};
DECLARE_SERIALIZE_OPERATOR(MsgInsertSscCarriage)
// update detail information
class AFX_EXT_CLASS MsgUpdateSscCarriage: public BaseMsg
{
	DECLARE_CLBTH(MsgUpdateSscCarriage)

public:
	MsgUpdateSscCarriage();
	virtual ~MsgUpdateSscCarriage();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	CONID   con_id;         //调度台号
	STATION_NO  station_no;       //站号
	short   side_no;  //股道号
	short   block_index_in_side;
	short   car_index_in_block;
	CARRIAGE_NO car_no;   //车号(8)
	short   car_type_id;  //车种id号
	char    car_type[4];  //车种
	IBYTE   oil_type; //油车类型
	IBYTE   load_flag;//载重标志
	short   car_suttle;//自重
	short   car_len_cnt;//换长
	short   car_load;//载重
	STATION_NO  first_station_no;//第一到站站号
	char    first_station_code[4];  //第一到站略码
	STATION_NO  last_station_no;  //最后到站站号
	char    last_station_code[4]; //最后到站略码
	char    freight_name[38]; //货物名称
	char    freight_code[8];  // 货物品名代码
	char    receiver[40]; //收货人
	char    depart_station_code[4]; //发站略码
	STATION_NO  depart_station_no;  //发站站号
	short   car_van_cnt;  //篷布数
	char    car_note[40]; //记事
	char    car_type_code[4]; //车种代码
	char    container_type_1_cnt[4];  //集装箱类型1
	char    weight_container_1_cnt[4];  //重集装箱数1
	char    empty_contrainer_1_cnt[4];  //空集装箱数1
	char    container_type_2_cnt[4];  //集装箱类型2
	char    weight_container_2_cnt[4];  //重集装箱数2
	char    empty_contrainer_2_cnt[4];  //空集装箱数2
	char    car_empty_weight_flag[4]; //空重标志
	char    first_station_name[12]; //第一到站名称
	char    last_station_name[12];  //最后到站名称
	char    depart_station_name[12];  //发站名称
	char    aim_bureau[4];  //到局
	char    aim_subbereau[4]; //到分居
	char    direction_no[4];//方向号
	char    freight_no_use_code[4]; //货物无用品名代码
	char    rail_company_flag[4]; //铁路公司标示
	char    no_count_flag[4]; //不计数标志
	char    current_status[4];  //当前状态
	I_TIME    arrive_time;  //到达时间
	TRAIN_INDEX arrive_train_index; //到达车次索引
	TRAIN_NO  arrive_train_no;  //到达车次号
	IBYTE   arrive_flag;  //到达标示
	IBYTE   arrive_reason;  //到达原因
	I_TIME    work_complete_time; //工作完成时间
	I_TIME    depart_time;  //出发时间
	TRAIN_INDEX depart_train_index; //出发车次索引
	IBYTE   depart_flag;  //出发标示
};
DECLARE_SERIALIZE_OPERATOR(MsgUpdateSscCarriage)
// update detail information
class AFX_EXT_CLASS MsgDeleteSscCarriage: public BaseMsg
{
	DECLARE_CLBTH(MsgDeleteSscCarriage)

public:
	MsgDeleteSscCarriage();
	virtual ~MsgDeleteSscCarriage();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	CONID   con_id;         //调度台号
	STATION_NO  station_no;       //站号
	short   side_no;  //股道号
	short   block_index_in_side;  //
	short   car_index_in_block; //
	CARRIAGE_NO car_no; //车号
	//  enum SSC_DELETE_FLAG
};
DECLARE_SERIALIZE_OPERATOR(MsgDeleteSscCarriage)
// OPER RESULT
class AFX_EXT_CLASS MsgOperSscCarriageReaponse: public BaseMsg
{
	DECLARE_CLBTH(MsgOperSscCarriageReaponse)

public:
	MsgOperSscCarriageReaponse();
	virtual ~MsgOperSscCarriageReaponse();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	CONID   con_id;         //调度台号
	STATION_NO  station_no;       //站号
	short   side_no;//股道号
	short   block_index_in_side;//
	short   car_index_in_block;//
	CARRIAGE_NO car_no;//车号
	enum DEL_UPDATE_SSC_CARRIAGE_RESULT{//删除更新机车结果
		NO_FOUND = 0,//没发现
		UPDATE_DETAIL_SUCCESS = 1,//更新细节成功
		UPDATE_POS_SUCCESS = 2,//更新位置成功
		UPDATE_STATUS_SUCCESS = 3,//更新状态成功
		UPDATE_NOTE_SUCCESS = 4,//更新注释成功
		DELETE_SUCCESS = 5,//删除成功
		INSERT_SUCCESS = 6,//插入成功
		UPDATE_DETAIL_FAILED = -1,//更新细节失败
		UPDATE_POS_FAILED = -2,//更新位置失败
		UPDATE_STATUS_FAILED = -3,//更新状态失败
		UPDATE_NOTE_FAILED = -4,//更新注释失败
		DELETE_BLOCK_FAILED = -5,//
		INSERT_FAILED = -6,//插入失败
	}del_update_carriage_rs;
};
DECLARE_SERIALIZE_OPERATOR(MsgOperSscCarriageReaponse)

// send next console schedule
class AFX_EXT_CLASS MsgNextPlanSendOrder: public BaseMsg
{  // GPC orders CAD to send plan
	DECLARE_CLBTH(MsgNextPlanSendOrder)

public:
	MsgNextPlanSendOrder();
	virtual ~MsgNextPlanSendOrder();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	ENTITY_ID   send_entity;//发送实体号
	ENTITY_ID   recv_entity;//接收实体号

	CONID       send_con_id;//发送调度台号
	CONID       recv_con_id;//接收调度台号

	USERNAME    sender_name;//发送用户名
	I_TIME        begin_time;//开始时间
	I_TIME        end_time;//结束时间
	I_TIME        send_time;//发送时间

	short       next_count;//
	struct NextPlanSendOrder{
		STATION_NO  station;//站号

		short   count;//
		IBYTE   entry[MAX_STPLAN_DSTN_CNT]; // 入口 -1 both
		IBYTE   exit[MAX_STPLAN_DSTN_CNT];  // 出口 -1 both
		IBYTE   yard[MAX_STPLAN_DSTN_CNT];  // -1 both
		IBYTE   train_type[MAX_STPLAN_DSTN_CNT];  //车次类型 -1 both, 0 passenger 1 freight
	}     next_order[MAX_STPLAN_DSTN_CNT];
};
DECLARE_SERIALIZE_OPERATOR(MsgNextPlanSendOrder)

class AFX_EXT_CLASS MsgNextPlanRequest: public BaseMsg
{    // request next console plan
	DECLARE_CLBTH(MsgNextPlanRequest)

public:
	MsgNextPlanRequest();
	virtual ~MsgNextPlanRequest();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	ENTITY_ID   requester_entity;//请求实体号
	ENTITY_ID   master_entity;//主实体号

	CONID       requester_con_id;//请求调度台号
	CONID       master_con_id;//主调度台号

	I_TIME        begin_time;//开始时间
	I_TIME        end_time;//结束时间

	USERNAME    requester_name;//请求用户名
};
DECLARE_SERIALIZE_OPERATOR(MsgNextPlanRequest)

class AFX_EXT_CLASS MsgNextPlanSendInit: public BaseMsg
{    // CAD sends plan init to next console
	DECLARE_CLBTH(MsgNextPlanSendInit)

public:
	MsgNextPlanSendInit();
	virtual ~MsgNextPlanSendInit();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	ENTITY_ID   send_entity;//发送实体号
	ENTITY_ID   recv_entity;//接收实体号

	CONID       send_con_id;//发送调度台号
	CONID       recv_con_id;//接收调度台号

	I_TIME        begin_time;//开始时间
	I_TIME        end_time;//结束时间
	I_TIME        send_time;//发送时间
	USERNAME    sender_name;//发送用户名
};
DECLARE_SERIALIZE_OPERATOR(MsgNextPlanSendInit)

class AFX_EXT_CLASS MsgNextPlanSendComplete: public BaseMsg
{ // CAD send plan complete to next console
	DECLARE_CLBTH(MsgNextPlanSendComplete)

public:
	MsgNextPlanSendComplete();
	virtual ~MsgNextPlanSendComplete();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	ENTITY_ID   send_entity;//发送实体号
	ENTITY_ID   recv_entity;//接收实体号

	CONID       send_con_id;//发送调度台号
	CONID       recv_con_id;//接收调度台号

	I_TIME        begin_time;//开始时间
	I_TIME        end_time;//结束时间
	I_TIME        send_time;  //发送时间

	short       insert_cnt;//插入数
	USERNAME    sender_name;//发送用户名
};
DECLARE_SERIALIZE_OPERATOR(MsgNextPlanSendComplete)

class AFX_EXT_CLASS MsgNextPlanInsert: public BaseMsg
{    // CAD send plan data(insert or change) to next console
	DECLARE_CLBTH(MsgNextPlanInsert)

public:
	MsgNextPlanInsert();
	virtual ~MsgNextPlanInsert();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	ENTITY_ID   send_entity;//发送实体号
	ENTITY_ID   recv_entity;//接收实体号

	CONID       send_con_id;//发送调度台号
	CONID       recv_con_id;//接收调度台号

	I_TIME        begin_time;//开始时间
	I_TIME        end_time;//结束时间
	I_TIME        send_time;  //发送时间
	USERNAME    sender_name;  //发送用户名

	TRAIN_NO    arrive_train_id;//到达车次号
	TRAIN_NO    depart_train_id;//出发车次号
	TRAIN_INDEX train_index;//车次索引
	STATION_NO  station;//站号
	I_TIME        arrive;//到达时间
	I_TIME        depart;//出发时间
	IBYTE        entry;//入口
	IBYTE        exit;//出口
	IBYTE        arrive_side;//到达股道
	IBYTE        depart_side;//出发股道
	IWORD   early_depart_time;//提前出发时间
	IBYTE     run_fast_slow_time;//运行早晚点
	IBYTE     speed_limit;//速度限制
	IBYTE     stop_condition;//停车条件
	IWORD   adjust_status;//调整状态
	IWORD   flag; //标示
};
DECLARE_SERIALIZE_OPERATOR(MsgNextPlanInsert)

class AFX_EXT_CLASS MsgNextPlanTrainRecv: public BaseMsg
{    // gpc send CAD recv each plan
	DECLARE_CLBTH(MsgNextPlanTrainRecv)

public:
	MsgNextPlanTrainRecv();
	virtual ~MsgNextPlanTrainRecv();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	ENTITY_ID   send_entity;//发送实体号
	ENTITY_ID   recv_entity;//接收实体号

	CONID       send_con_id;//发送调度台号
	CONID       recv_con_id;//接收调度台号

	I_TIME        begin_time;//开始时间
	I_TIME        end_time;//结束时间
	I_TIME        send_time;  //发送时间

	TRAIN_NO    arrive_train_id;//到达车次号
	TRAIN_NO    depart_train_id;//出发车次号
	TRAIN_INDEX train_index;//车次索引
	STATION_NO  station;//站号
	I_TIME        arrive;//到达时间
	I_TIME        depart;//出发时间
	IBYTE        entry;//入口
	IBYTE        exit;//出口
	IBYTE        side;//股道
	IWORD      early_depart_time;//提前出发时间
	IBYTE        run_fast_slow_time;//运行早晚点
	IBYTE        speed_limit;//速度限制
	IBYTE        stop_condition;//停车条件
	IWORD      adjust_status;//调整状态
	IWORD      flag;//标示

	IWORD      passway;//径路

	enum RECV_STATUS {//接收状态
		RECV_ACCEPT,//接收
		RECV_REJECT//拒绝
	}       recv_status;
};
DECLARE_SERIALIZE_OPERATOR(MsgNextPlanTrainRecv)

class AFX_EXT_CLASS MsgNextPlanRecvStatus: public BaseMsg
{     // next console send recv all msg to CAD, then CAD relays to GPC
	DECLARE_CLBTH(MsgNextPlanRecvStatus)

public:
	MsgNextPlanRecvStatus();
	virtual ~MsgNextPlanRecvStatus();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	ENTITY_ID   send_entity;//发送实体号
	ENTITY_ID   recv_entity;//接收实体号

	CONID       send_con_id;//发送调度台号
	CONID       recv_con_id;//接收调度台号

	I_TIME        begin_time;//计划开始时间
	I_TIME        end_time;//计划结束时间
	I_TIME        send_time;//计划发送时间
	I_TIME    recv_time;//计划接收时间

	enum PLAN_RECV_STATUS {//计划接收状态
		PLAN_RECV,//计划接收
		PLAN_SENDING,//计划发送中
		PLAN_NO_CHANGE,//计划没有变化
		PLAN_NO_DATA,//计划无数据
		PLAN_ENTITY_DEAD,//实体无响应
		PLAN_CONFIG_WRONG,//配置错误
		PLAN_INIT//计划初始化
	} recv_status;

	USERNAME  sender_name;//发送用户名
	USERNAME    recver_name;//接收用户名
};
DECLARE_SERIALIZE_OPERATOR(MsgNextPlanRecvStatus)

class AFX_EXT_CLASS MsgNextPlanSignStatus: public BaseMsg
{     // next console send recv all msg to CAD, then CAD relays to GPC
	DECLARE_CLBTH(MsgNextPlanSignStatus)

public:
	MsgNextPlanSignStatus();
	virtual ~MsgNextPlanSignStatus();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	ENTITY_ID   send_entity;//发送实体号
	ENTITY_ID   recv_entity;//接收实体号

	CONID       send_con_id;//发送调度台号
	CONID       recv_con_id;//接收调度台号

	I_TIME        begin_time;//计划开始时间
	I_TIME        end_time;//计划结束时间
	I_TIME        send_time;//计划发送时间
	I_TIME    recv_time;//计划接收时间

	USERNAME  sender_name;//发送用户名
	USERNAME    recver_name;//接收用户名

	short       accept_count;//接收数
	short       reject_count;//拒绝数

	TRAIN_NO    accept_train[100];//接收车次
	TRAIN_NO    reject_train[100];//拒绝车次
};
DECLARE_SERIALIZE_OPERATOR(MsgNextPlanSignStatus)

class AFX_EXT_CLASS MsgWordPhasePlanSend: public BaseMsg
{    // gpc to cad, cad to gpc and cad to stpc
	DECLARE_CLBTH(MsgWordPhasePlanSend)

public:
	MsgWordPhasePlanSend();
	virtual ~MsgWordPhasePlanSend();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	SHIFT_ID  shift_id;//日班计划id号
	CONID   con_id;//调度台号
	IBYTE   schd_type;//运行线类型
	IBYTE   phase_id;//阶段计划id
	short   recv_count;//接收阶段计划实体数
	ENTITY_ID recv_list[128];//接收阶段计划实体
	ENTITY_ID send_entity;//发送实体号
	I_TIME    edit_time;  //编辑时间
	I_TIME    send_time;  //发送时间
	USERNAME  send_name;  //发送用户名
	USERNAME  permit_name;  //许可用户名
	IWORD   start_sta_weather;  //开始站天气
	IWORD   end_sta_weather;  //结束站天气
	short   phase_len;  //阶段计划长度
	char    phase_content[2048];  //阶段计划内容
	enum    WORDPHASE_OPER {
		WORDPHASE_SERVER_ECHO,  //
		WORDPHASE_SAVE, //计划保存
		WORDPHASE_SEND  //计划发送
	}   phase_oper;
};
DECLARE_SERIALIZE_OPERATOR(MsgWordPhasePlanSend)

class AFX_EXT_CLASS MsgWordPhasePlanStatus: public BaseMsg
{
	DECLARE_CLBTH(MsgWordPhasePlanStatus)

public:
	MsgWordPhasePlanStatus();
	virtual ~MsgWordPhasePlanStatus();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	SHIFT_ID  shift_id;//日班计划id号
	CONID   con_id;//调度台号
	IBYTE   schd_type;//运行线类型
	IBYTE   phase_id;//阶段计划id号
	ENTITY_ID send_entity;//发送实体号
	ENTITY_ID recv_entity;  //接收实体号
	I_TIME    send_time;//发送时间
	I_TIME    recv_time;//接收时间
	I_TIME    edit_time;  //编辑时间
	USERNAME  send_name;//发送用户名
	USERNAME  permit_name;//许可用户名
	USERNAME  recv_name;//接收用户名

	enum WORDPHASE_RECV_STATUS {//接收状态
		WORDPHASE_RECV,//接收
		WORDPHASE_SIGN, //签收
		WORDPHASE_SENDING,//发送中
		WORDPHASE_ENTITY_DEAD,//实体无响应
		WORDPHASE_CONFIG_WRONG,//配置错误
		WORDPHASE_ERROR,//错误
		WORDPHASE_INIT//初始化
	} recv_status;

};
DECLARE_SERIALIZE_OPERATOR(MsgWordPhasePlanStatus)

class AFX_EXT_CLASS MsgWordPhasePlanQuery: public BaseMsg
{
	DECLARE_CLBTH(MsgWordPhasePlanQuery)

public:
	MsgWordPhasePlanQuery();
	virtual ~MsgWordPhasePlanQuery();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	SHIFT_ID  shift_id;//日班计划id号
	CONID   con_id;//调度台号
	IBYTE   schd_type;//运行线类型
	IBYTE   phase_count;//阶段计划数
	IBYTE   phase_id[4];//阶段计划号

};
DECLARE_SERIALIZE_OPERATOR(MsgWordPhasePlanQuery)

class AFX_EXT_CLASS MsgSimpleStationCarFormat : public BaseMsg
{
	DECLARE_CLBTH(MsgSimpleStationCarFormat)

public:
	MsgSimpleStationCarFormat();
	virtual ~MsgSimpleStationCarFormat();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	I_TIME          input_time;//输入时间
	STATION_NO    station;//站号
	enum  STACAR_PHASE {
		STACAR_PHASE_0,
		STACAR_PHASE_6,
		STACAR_PHASE_12,
		STACAR_PHASE_18,
		STACAR_PHASE_CURRENT
	}         phase;

	short         index;//索引
	char          side_name[20];//股道名
	IBYTE          side;//股道
	/* 与station_now_car.xml的对应关系   */
	IBYTE            car_flag;   /* work_explain的序号                */
	IBYTE            car_type;   /* (重-0x80,空-0x00) & common1的序号 */
	short           car_count;//车数
	char            destine[512];//
};
DECLARE_SERIALIZE_OPERATOR(MsgSimpleStationCarFormat)

class AFX_EXT_CLASS MsgSimpleStationCarCtrl : public BaseMsg
{
	DECLARE_CLBTH(MsgSimpleStationCarCtrl)

public:
	MsgSimpleStationCarCtrl();
	virtual ~MsgSimpleStationCarCtrl();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	I_TIME          input_time;//输入时间
	STATION_NO    station;//站号
	enum  STACAR_PHASE {
		STACAR_PHASE_0,
		STACAR_PHASE_6,
		STACAR_PHASE_12,
		STACAR_PHASE_18,
		STACAR_PHASE_CURRENT
	}         phase;
	enum MSG_STATUS {MSG_BEGIN, MSG_END} status;//消息状态:消息开始,消息结束
};
DECLARE_SERIALIZE_OPERATOR(MsgSimpleStationCarCtrl)

class AFX_EXT_CLASS MsgSimpleStationCarQuery : public BaseMsg
{
	DECLARE_CLBTH(MsgSimpleStationCarQuery)

public:
	MsgSimpleStationCarQuery();
	virtual ~MsgSimpleStationCarQuery();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	I_TIME     begin_time;//查询开始时间
	I_TIME     end_time;//查询结束时间
	enum  STACAR_PHASE {
		STACAR_PHASE_0,
		STACAR_PHASE_6,
		STACAR_PHASE_12,
		STACAR_PHASE_18,
		STACAR_PHASE_CURRENT
	}         phase;
	short      sta_count;//站数
	STATION_NO sta_list[64];//站号
};
DECLARE_SERIALIZE_OPERATOR(MsgSimpleStationCarQuery)

class AFX_EXT_CLASS MsgSimpleStationCarConvert : public BaseMsg
{
	DECLARE_CLBTH(MsgSimpleStationCarConvert)

public:
	MsgSimpleStationCarConvert();
	virtual ~MsgSimpleStationCarConvert();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	enum  STACAR_PHASE {
		STACAR_PHASE_0,
		STACAR_PHASE_6,
		STACAR_PHASE_12,
		STACAR_PHASE_18,
		STACAR_PHASE_CURRENT
	};
	STACAR_PHASE  from_phase;//来自哪个阶段
	STACAR_PHASE  to_phase;//转换到哪个极端
	I_TIME            from_input_time;//
	I_TIME            to_input_time;//

	short           sta_count;//站数
	STATION_NO      sta_list[64];//站号
};
DECLARE_SERIALIZE_OPERATOR(MsgSimpleStationCarConvert)

class AFX_EXT_CLASS MsgNotePadFormat : public BaseMsg
{
	DECLARE_CLBTH(MsgNotePadFormat)

public:
	MsgNotePadFormat();
	virtual ~MsgNotePadFormat();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	SHIFT_ID         shift_id;//日班计划id号

	CONID            con_id;//调度台号
	IBYTE             schd_type;//运行线类型

	short            data_len;//数据长度
	char             data[4000];//数据
};
DECLARE_SERIALIZE_OPERATOR(MsgNotePadFormat)

class AFX_EXT_CLASS MsgNotePadQuery : public BaseMsg
{
	DECLARE_CLBTH(MsgNotePadQuery)

public:
	MsgNotePadQuery();
	virtual ~MsgNotePadQuery();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	SHIFT_ID         shift_id;//日班计划id号

	CONID            con_id;//调度台号
	IBYTE             schd_type;//运行线类型
};
DECLARE_SERIALIZE_OPERATOR(MsgNotePadQuery)

class AFX_EXT_CLASS MsgAccidentOper : public BaseMsg
{
	DECLARE_CLBTH(MsgAccidentOper)

public:
	MsgAccidentOper();
	virtual ~MsgAccidentOper();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	CONID            con_id;//调度台号
	IBYTE             schd_type;//运行线类型
	SHIFT_ID         shift_id;//日班计划id号
	short            index;//索引
	short            note_len;//注释长度
	char             note[2400];//注释
	char             subject[40];//主题

	enum AccidentOper {
		ACCIDENT_SERVER_ECHO,//
		ACCIDENT_INSERT_UPDATE,//
		ACCIDENT_DELETE//
	}                 oper;
};
DECLARE_SERIALIZE_OPERATOR(MsgAccidentOper)

class AFX_EXT_CLASS MsgAccidentQuery : public BaseMsg
{
	DECLARE_CLBTH(MsgAccidentQuery)

public:
	MsgAccidentQuery();
	virtual ~MsgAccidentQuery();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	CONID            con_id;//调度台号
	IBYTE             schd_type;//运行线类型
	SHIFT_ID         shift_id;  //日班计划id号
};
DECLARE_SERIALIZE_OPERATOR(MsgAccidentQuery)

class AFX_EXT_CLASS MsgUserManager : public BaseMsg
{
	DECLARE_CLBTH(MsgUserManager)

public:
	MsgUserManager();
	virtual ~MsgUserManager();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	short count;//数量
	struct {
		USERNAME  user_code;//用户号
		USERNAME  user_name;//用户名
		PASSWORD  user_pwd;//用户密码
		JOB_ID    job_id;//工作号
		IBYTE   console_count;//调度台数
		CONID   con_list[20];//调度台号
	} data[30];

	enum USER_OPER{//用户操作
		USERMGR_INSERT,//插入
		USERMGR_DELETE,//删除
		USERMGR_CHANGE,//改变
		USERMGR_CHANGE_PROP,//
		USERMGR_SERVER_ECHO//
	} oper;
};
DECLARE_SERIALIZE_OPERATOR(MsgUserManager)

class AFX_EXT_CLASS MsgUserManagerQuery : public BaseMsg
{
	DECLARE_CLBTH(MsgUserManagerQuery)

public:
	MsgUserManagerQuery();
	virtual ~MsgUserManagerQuery();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
public:
};
DECLARE_SERIALIZE_OPERATOR(MsgUserManagerQuery)

class AFX_EXT_CLASS MsgRouteStatusInform : public BaseMsg
{
	DECLARE_CLBTH(MsgRouteStatusInform)

public:
	MsgRouteStatusInform();
	virtual ~MsgRouteStatusInform();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
public:
	STATION_NO      station;//站号
	TRAIN_NO      train_id;//车次号
	TRAIN_INDEX     train_index;//车次索引
	IBYTE       rec_index;//接收索引
	CONID       con_id;//调度台号

	ENTRY_ID      entry;//入口
	ENTRY_ID      exit;//出口
	SIDE_ID       side;//股道

	enum ROUTE_ATTRIBUTE {//进路描述
		PASS_ROUTE,//通过进路
		ARRIVE_ROUTE,//到达进路
		DEPART_ROUTE//出发进路
	}                   attrib;
	ROUTE_TRIG_STATUS   status;//进路触发状态
};
DECLARE_SERIALIZE_OPERATOR(MsgRouteStatusInform)

class AFX_EXT_CLASS MsgGroupTrainWorkFormat : public BaseMsg
{
	DECLARE_CLBTH(MsgGroupTrainWorkFormat)

public:
	MsgGroupTrainWorkFormat();
	virtual ~MsgGroupTrainWorkFormat();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
public:
	CONID       con_id;//调度台号
	IBYTE       schd_type;  //运行线类型
	short       rec_count;  //接收数
	struct {
		I_TIME      input_time; //输入时间
		TRAIN_INDEX   train_index;  //列车索引
		TRAIN_NO    train_id; //车次号
		STATION_NO    station;  //站号
		IBYTE     work_type;  //工作类型
		IBYTE     side; //股道
		char      work_content[20]; //工作内容
	}         data[40];
	enum GROUP_TRAIN_WORK_OPER {
		GPC_SET,  //gpc设置
		GPC_DELETE, //gpc删除
		SERVER_ECHO   //
	}         oper;
};
DECLARE_SERIALIZE_OPERATOR(MsgGroupTrainWorkFormat)

class AFX_EXT_CLASS MsgGroupTrainWorkDel : public BaseMsg
{
	DECLARE_CLBTH(MsgGroupTrainWorkDel)

public:
	MsgGroupTrainWorkDel();
	virtual ~MsgGroupTrainWorkDel();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
public:
	CONID       con_id;//调度台号
	IBYTE       schd_type;//运行线类型
	short       rec_count;//接收数

	I_TIME        input_time[40];
};
DECLARE_SERIALIZE_OPERATOR(MsgGroupTrainWorkDel)

class AFX_EXT_CLASS MsgGroupTrainWorkQuery : public BaseMsg
{
	DECLARE_CLBTH(MsgGroupTrainWorkQuery)

public:
	MsgGroupTrainWorkQuery();
	virtual ~MsgGroupTrainWorkQuery();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
public:
	SHIFT_ID            shift_id;//日班计划中的id
	CONID       con_id;//调度台号
	IBYTE       schd_type;//运行线类型
	short       rec_count;//

	TRAIN_INDEX     train_index[256];//车次索引
};
DECLARE_SERIALIZE_OPERATOR(MsgGroupTrainWorkQuery)

class AFX_EXT_CLASS MsgAdjustSequenceLoad : public BaseMsg
{
	DECLARE_CLBTH(MsgAdjustSequenceLoad)

public:
	MsgAdjustSequenceLoad();
	virtual ~MsgAdjustSequenceLoad();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
public:

	CONID con_id;
};
DECLARE_SERIALIZE_OPERATOR(MsgAdjustSequenceLoad)

class AFX_EXT_CLASS MsgAdjustSequenceData : public BaseMsg
{
	DECLARE_CLBTH(MsgAdjustSequenceData)

public:
	MsgAdjustSequenceData();
	virtual ~MsgAdjustSequenceData();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
public:

	CONID   con_id;//调度台号
	short   count;//
	enum  OPER {
		ADJUSTSEQ_SERVER_ECHO,
		ADJUSTSEQ_GPC_SET,
		ADJUSTSEQ_GPC_REFRESH
	}     oper;
	struct {
		TRAIN_INDEX train_index;//车次索引
		IWORD   prio;
	}     data[600];
};
DECLARE_SERIALIZE_OPERATOR(MsgAdjustSequenceData)

class AFX_EXT_CLASS MsgAutoAdjustInform : public BaseMsg
{
	DECLARE_CLBTH(MsgAutoAdjustInform)

public:
	MsgAutoAdjustInform();
	virtual ~MsgAutoAdjustInform();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
public:

	CONID   con_id;//调度台号
	I_TIME    start_time;//开始时间
	I_TIME    end_time;//结束时间
	enum  OPER {
		AUTOADJUST_SERVER_ECHO_BEGIN,
		AUTOADJUST_SERVER_ECHO_END,
		AUTOADJUST_SERVER_ECHO_FAILED,
		AUTOADJUST_GPC_SET,
	}     oper;
};
DECLARE_SERIALIZE_OPERATOR(MsgAutoAdjustInform)

class AFX_EXT_CLASS MsgAutoAdjustSingleInform : public BaseMsg
{
	DECLARE_CLBTH(MsgAutoAdjustSingleInform)

public:
	MsgAutoAdjustSingleInform();
	virtual ~MsgAutoAdjustSingleInform();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
public:

	CONID        con_id;//调度台号
	TRAIN_INDEX  train_index;//车次索引
	short        adjust_begin_sta_index;//调整开始的车站索引
	short        adjust_end_sta_index;//调整结束的车站索引
	enum    SINGLE_ADJUST_PRIO {
		SINGLE_HIGH_CHANGE_FOLLOWING,
		SINGLE_HIGH_CHANGE_SELF_ONLY,
		SINGLE_NORMAL_CHANGE_FOLLOWING,
		SINGLE_NORMAL_CHANGE_SELF_ONLY,
		SINGLE_LOW,
	}     prio;
};
DECLARE_SERIALIZE_OPERATOR(MsgAutoAdjustSingleInform)

class AFX_EXT_CLASS MsgAutoAdjustUndo : public BaseMsg
{
	DECLARE_CLBTH(MsgAutoAdjustUndo)

public:
	MsgAutoAdjustUndo();
	virtual ~MsgAutoAdjustUndo();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
public:

	CONID        con_id;//调度台号
};
DECLARE_SERIALIZE_OPERATOR(MsgAutoAdjustUndo)

// 自律机根据GPCID向GPC发送消息
class AFX_EXT_CLASS MsgLiRCToGpc : public BaseMsg
{
	DECLARE_CLBTH(MsgLiRCToGpc)
public:
	MsgLiRCToGpc();
	virtual ~MsgLiRCToGpc();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	GPC_ID      nGpcID;//gpc号
	IWORD     nMsgLen;//消息长度
	enum { BUFFER_SIZE = 2048 };
	IBYTE     Msg[BUFFER_SIZE];//消息
};
DECLARE_SERIALIZE_OPERATOR(MsgLiRCToGpc)

// shuntnote data
class AFX_EXT_CLASS MsgShuntNoteSend2LocoMotive: public BaseMsg
{     // GPC orders LIRC to send ShuntNote to locomotive
	DECLARE_CLBTH(MsgShuntNoteSend2LocoMotive)

public:
	MsgShuntNoteSend2LocoMotive();
	virtual ~MsgShuntNoteSend2LocoMotive();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	SHUNT_INDEX   shunt_index;             //  作业单
	STATION_NO    nstation;                //  站号
	TRAIN_NO    train_id;                //  车次号
	ENGINE_NO     engine_id;               //  作业机车号或者车次号
	I_TIME          send_time;               //  下发时间

	enum   SEND_WAY {
		SEND_INTELLIGENT,                         //智能发送
		SEND_IMMEDIATELY                          //立即发送
	}send_way;                             //   发送方式

	enum   OPER_TYPE {
		SEND_2_LOCOMOTIVE,                        //发送给机车
		DELETE_FROM_LIRC                          //从自律机中删除
	}oper_type;                             //   操作类型
};
DECLARE_SERIALIZE_OPERATOR(MsgShuntNoteSend2LocoMotive)


class AFX_EXT_CLASS MsgShuntNote2LocoMotiveStatus: public BaseMsg
{    // send MsgShuntNoteStatus from lirc to gpc
	DECLARE_CLBTH(MsgShuntNote2LocoMotiveStatus)

public:
	MsgShuntNote2LocoMotiveStatus();
	virtual ~MsgShuntNote2LocoMotiveStatus();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	SHUNT_INDEX     shunt_index;//调车单索引
	STATION_NO      nstation;                //  站号
	enum STATUS{
		SHUNT_RECEIVE,//作业单接收
		SHUNT_SIGNATURE,//作业单签收
		SHUNT_SENDING,//作业单发送中
		SHUNT_NOT_SEND,//作业单未发送
		SHUNT_INIT,//作业单初始化
		SHUNT_DELETED     //作业单已被删除
	} status;
	I_TIME            send_time;//发送时间
	I_TIME            sign_time;//签收时间
	USERNAME    sign_name;//签收名字

};
DECLARE_SERIALIZE_OPERATOR(MsgShuntNote2LocoMotiveStatus)

class AFX_EXT_CLASS MsgDataRequest: public BaseMsg
{
	DECLARE_CLBTH(MsgDataRequest)

public:
	MsgDataRequest();
	virtual ~MsgDataRequest();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	MSGHEAD::msgid_t msgid;//消息id
};
DECLARE_SERIALIZE_OPERATOR(MsgDataRequest)

class AFX_EXT_CLASS MsgServerProcessEcho: public BaseMsg
{
	DECLARE_CLBTH(MsgServerProcessEcho)

public:
	MsgServerProcessEcho();
	virtual ~MsgServerProcessEcho();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	MSGHEAD::msgid_t msgid; //消息id
	I_TIME   recv_time;//接收时间
};
DECLARE_SERIALIZE_OPERATOR(MsgServerProcessEcho)

class AFX_EXT_CLASS MsgQueryTrainForRoute: public BaseMsg
{
	DECLARE_CLBTH(MsgQueryTrainForRoute)

public:
	MsgQueryTrainForRoute();
	virtual ~MsgQueryTrainForRoute();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	MANUAL_OPER_ID  nOperID;  // 人工操作的唯一ID标识
	STATION_NO    nStationNo;//站号
	IBYTE     nButtonCount;//按钮数
	BUTTON_ID   nButtonID[MAX_ROUTE_BUTTON_COUNT];//按钮号
	BUTTON_NAME strButtonName[MAX_ROUTE_BUTTON_COUNT];//按钮名称
	ROUTE_ID    nRouteID;//进路号
	enum ROUTE_TYPE { INVALID=0, TRAIN, SHUNT };//无效、列车、调车
	ROUTE_TYPE    nRouteType;//进路类型
};
DECLARE_SERIALIZE_OPERATOR(MsgQueryTrainForRoute)

class AFX_EXT_CLASS MsgAnswerTrainForRoute: public BaseMsg
{
	DECLARE_CLBTH(MsgAnswerTrainForRoute)

public:
	MsgAnswerTrainForRoute();
	virtual ~MsgAnswerTrainForRoute();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	MANUAL_OPER_ID  nOperID;//人工操作id
	STATION_NO    nStationNo;//站号
	ROUTE_ID    nRouteID;//进路号
	TRAIN_NO    tnTrainNo;//车次号
};
DECLARE_SERIALIZE_OPERATOR(MsgAnswerTrainForRoute)

// 通信状态报告
class AFX_EXT_CLASS MsgCommStatusReport: public BaseMsg
{
	DECLARE_CLBTH(MsgCommStatusReport)

public:
	MsgCommStatusReport();
	virtual ~MsgCommStatusReport();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	enum
	{
		CSR_ID_LIRC = 1,
		CSR_ID_STPC = 2,
		CSR_ID_BL   = 3,
		CSR_ID_CAD  = 4,
		CSR_ID_GPC  = 5,

		CSR_DATA_LOST   = 1,    // 检测到数据丢失
		CSR_REMOTE_DOWN     = 2,    // 检测到对方中断
		CSR_LOCAL_UP    = 3       // 本机复位通告
	};

	IBYTE   nStatus;//状态
	IBYTE        nIdentify;
};
DECLARE_SERIALIZE_OPERATOR(MsgCommStatusReport)

// 探测消息
class AFX_EXT_CLASS MsgPeerDetect: public BaseMsg
{
	DECLARE_CLBTH(MsgPeerDetect)

public:
	MsgPeerDetect();
	virtual ~MsgPeerDetect();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:

};
DECLARE_SERIALIZE_OPERATOR(MsgPeerDetect)

//此处为GSM-R特殊消息
//无线车次号消息，BASELEVEL收到后发送到LIRC和SERVER
class AFX_EXT_CLASS MsgRadioTrainInfo : public BaseMsg
{
	DECLARE_CLBTH(MsgRadioTrainInfo)

public:
	MsgRadioTrainInfo();
	virtual ~MsgRadioTrainInfo();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
public:

	TRAIN_NO    train_id; //车次，for wireless
	TRAIN_NO    locomotive; //机车号,for wireless
	IBYTE        motivetype; //机车类型
	STATION_NO  station_no; //该车当前所在车站
	IDWORD       distance;   //公里标（米）
	short       speed;      //速度（0.1 KM/h）
	short       car_cnt;     //车数
	short       car_len;        //记长   (0.1)
	ISDWORD        total_weight;   //全重   (0.1吨)
	IBYTE        train_status;   //1启动,2停稳,默认0
	IBYTE        train_com;      //列车完整性 0x55 列车完整, 0x77 列车不完整 否则不确定
	USERNAME    driver;         //司机姓名
	TRAIN_NO    conf_trainno;   //CTC确认车次号
	char        othermsg[22];   //其他消息

};
DECLARE_SERIALIZE_OPERATOR(MsgRadioTrainInfo)

class AFX_EXT_CLASS MsgGsmrTrainInfo : public BaseMsg
{
	DECLARE_CLBTH(MsgGsmrTrainInfo)

public:
	MsgGsmrTrainInfo();
	virtual ~MsgGsmrTrainInfo();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
public:
	IWORD	token; //标志MSG_GSMR_TRAININFO 1
	IWORD	nLen; //内容长度
	char	content[4096];
};
DECLARE_SERIALIZE_OPERATOR(MsgGsmrTrainInfo)

///机车申请站场图消息
///BASELEVEL收到机车发送机车申请站场图消息（或BL判断应向该车发送站场图）后向Lirc发送此消息，
//Lirc收到此消息后向该车发送表示消息。
class AFX_EXT_CLASS MsgMotiveRequestDisplay : public BaseMsg
{
	DECLARE_CLBTH(MsgMotiveRequestDisplay)

public:
	MsgMotiveRequestDisplay();
	virtual ~MsgMotiveRequestDisplay();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
public:

	TRAIN_NO    train_id; //车次
	TRAIN_NO    locomotive; //机车号
	IBYTE        motivetype; //机车类型
	STATION_NO  station_no; //该车请求表示的车站

};
DECLARE_SERIALIZE_OPERATOR(MsgMotiveRequestDisplay)

//该消息目前预留，暂无此应用

class AFX_EXT_CLASS MsgMotiveDeviceAlarm : public BaseMsg
{
	DECLARE_CLBTH(MsgMotiveDeviceAlarm)

public:
	MsgMotiveDeviceAlarm();
	virtual ~MsgMotiveDeviceAlarm();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
public:

	TRAIN_NO    train_id; //车次
	TRAIN_NO    locomotive; //机车号
	IBYTE        motivetype; //机车类型
	STATION_NO  station_no; //报警列车所在的车站
	IWORD        alarm_status; //报警状态
	IBYTE        alarm_length;
	char        alarm_dis[256]; //报警描述
};
DECLARE_SERIALIZE_OPERATOR(MsgMotiveDeviceAlarm)
//列车进路预告
class AFX_EXT_CLASS MsgRouteForetell : public BaseMsg
{
	DECLARE_CLBTH(MsgRouteForetell)

public:
	MsgRouteForetell();
	virtual ~MsgRouteForetell();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
public:

	TRAIN_NO    train_id; //车次
	TRAIN_NO    locomotive; //机车号
	IBYTE        motivetype; //机车类型
	STATION_NO  station_no; //要预告的车站
	IBYTE        flag;       //接发车标志1:正线通过 2:侧线通过  3:进站停车
	IBYTE        side;       //股道号
	IBYTE        describe_len;
	char        describe[256];  //描述
};
DECLARE_SERIALIZE_OPERATOR(MsgRouteForetell)
//无线表示信息
class AFX_EXT_CLASS MsgRadioMotiveDisplay : public BaseMsg
{
	DECLARE_CLBTH(MsgRadioMotiveDisplay)

public:
	MsgRadioMotiveDisplay();
	virtual ~MsgRadioMotiveDisplay();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
public:

	TRAIN_NO    train_id; //车次
	TRAIN_NO    locomotive; //机车号
	IBYTE        motivetype; //机车类型
	STATION_NO  station_no; //要发送的车站
	enum { MAX_CONTENT_SIZE = 600 };
	IWORD        elementlen; //元素类型的个数
	IBYTE        elementdata[MAX_CONTENT_SIZE]; //元素状态区域
};
DECLARE_SERIALIZE_OPERATOR(MsgRadioMotiveDisplay)

class AFX_EXT_CLASS MsgTrainIDUpdate : public BaseMsg
{
	DECLARE_CLBTH(MsgTrainIDUpdate)

public:
	MsgTrainIDUpdate();
	virtual ~MsgTrainIDUpdate();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
public:

	TRAIN_NO    org_train_id; //原车次
	TRAIN_NO    locomotive; //机车号
	IBYTE        motivetype; //机车类型
	TRAIN_NO    update_train_id;  //更改后的车次号
	char        describe[22];  //其他数据
};
DECLARE_SERIALIZE_OPERATOR(MsgTrainIDUpdate)

//列车行驶出管界是自律机发送给BL
class AFX_EXT_CLASS MsgTrainOutofControl : public BaseMsg
{
	DECLARE_CLBTH(MsgTrainOutofControl)

public:
	MsgTrainOutofControl();
	virtual ~MsgTrainOutofControl();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
public:

	TRAIN_NO    train_id; //车次
	TRAIN_NO    locomotive; //机车号
	IBYTE        motivetype; //机车类型
};
DECLARE_SERIALIZE_OPERATOR(MsgTrainOutofControl)

class AFX_EXT_CLASS MsgRadioShuntNoteRequest : public BaseMsg
{
	DECLARE_CLBTH(MsgRadioShuntNoteRequest)

public:
	MsgRadioShuntNoteRequest();
	virtual ~MsgRadioShuntNoteRequest();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
public:

	TRAIN_NO    train_id; //车次
	TRAIN_NO    locomotive; //机车号
	IBYTE        motivetype; //机车类型
	STATION_NO  stationno;  //作业的站号
	IWORD        shunt_index; //作业单号
	IBYTE        gou_index;   //钩序号
};
DECLARE_SERIALIZE_OPERATOR(MsgRadioShuntNoteRequest)

class AFX_EXT_CLASS MsgRadioShuntNote : public BaseMsg
{
	DECLARE_CLBTH(MsgRadioShuntNote)

public:
	MsgRadioShuntNote();
	virtual ~MsgRadioShuntNote();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
public:

	TRAIN_NO    train_id; //车次
	TRAIN_NO    locomotive; //机车号
	IBYTE        motivetype; //机车类型
	STATION_NO  stationno;  //作业的站号
	IWORD        shunt_index; //作业单号
	I_TIME        send_time;   //作业单下达时刻(time_t)
	I_TIME        start_time;  //作业起始时刻(time_t)
	I_TIME        occ_time;    //作业预计时间(time_t)
	USERNAME    shunt_group_name;//调车组名称
	USERNAME    shunt_leader_name;//调车区长名称
	IBYTE        oper_len;         //作业长度
	char        operation_describe[64];//作业说明
	IBYTE        gou_count; //总勾数

	struct GOU_LIST{
		IWORD  shunt_gou_index;            //  调车钩唯一索引
		IWORD  shunt_gou_type;             //  调车勾类型
		//    BIT7-0: 0 挂,1 甩, 2本务机挂头,3 本务机摘头
		//    BIT10,8: 牵出线编号,7表示用区间调车,
		//              0表示不用牵出线
		//    BIT11: 是否影响正线
		SIDE_ID   side;                     //  作业股道
		IWORD    shunt_car_count;          //  作业车辆数
		IWORD    shunt_note_route_count;   //  进路个数
		IWORD    shunt_work_time;          //  调车进路作业时间
		IBYTE   route_describe_len;
		char   route_describe[256];          //  本勾进路的简述
		IBYTE   gou_describe_len;
		char   gou_describe[256];            //  本勾作业的备注
	}gou_list[MAX_GOU_PER_SHUNT];
};
DECLARE_SERIALIZE_OPERATOR(MsgRadioShuntNote)

//GPC、STPC发送无线调度命令时，先发送此消息查询车次位置
class AFX_EXT_CLASS MsgQueryTrainPosition : public BaseMsg
{
	DECLARE_CLBTH(MsgQueryTrainPosition)

public:
	MsgQueryTrainPosition();
	virtual ~MsgQueryTrainPosition();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
public:
	TRAIN_NO    train_id; //车次
};
DECLARE_SERIALIZE_OPERATOR(MsgQueryTrainPosition)

class AFX_EXT_CLASS MsgReplyTrainPosition : public BaseMsg
{
	DECLARE_CLBTH(MsgReplyTrainPosition)

public:
	MsgReplyTrainPosition();
	virtual ~MsgReplyTrainPosition();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
public:
	TRAIN_NO    train_id; //车次
	IBYTE        train_count;
	struct TRAIN_INFO{
		TRAIN_NO    locomotive; //机车号
		IBYTE        motivetype; //机车类型
		STATION_NO  stationno;  //目前所在车站的站号
	}train_info[MAX_TRAIN_POSITION];
};
DECLARE_SERIALIZE_OPERATOR(MsgReplyTrainPosition)

//BL判断出该向哪个机车发送站场图消息，发送此消息给服务器查询该车的前方站
class AFX_EXT_CLASS MsgQueryPreStationList : public BaseMsg
{
	DECLARE_CLBTH(MsgQueryPreStationList)

public:
	MsgQueryPreStationList();
	virtual ~MsgQueryPreStationList();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
public:
	TRAIN_NO    train_id;   //车次
	STATION_NO  cur_stno;   //当前所在车站
	IBYTE        requestcnt; //请求车站数
};
DECLARE_SERIALIZE_OPERATOR(MsgQueryPreStationList)

//服务器发送查询结果给BL
class AFX_EXT_CLASS MsgReplyPreStationList : public BaseMsg
{
	DECLARE_CLBTH(MsgReplyPreStationList)

public:
	MsgReplyPreStationList();
	virtual ~MsgReplyPreStationList();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
public:
	TRAIN_NO    train_id;   //车次
	STATION_NO  cur_stno;   //当前所在车站
	IBYTE        requestcnt; //请求车站数
	STATION_NO  st_list[MAX_STATION_LIST]; //查询车站的列表
};
DECLARE_SERIALIZE_OPERATOR(MsgReplyPreStationList)

class AFX_EXT_CLASS MsgGsmrLocalMsg: public BaseMsg
{
	DECLARE_CLBTH(MsgGsmrLocalMsg)

public:
	MsgGsmrLocalMsg();
	virtual ~MsgGsmrLocalMsg();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
public:
	char DestIpaddress[24];  //目标IP地址
	IWORD DestPort;           //目标端口
	IBYTE MsgId;              //消息ID
	IBYTE CommandId;          //命令号
	IWORD Msglen;             //消息长度
	IBYTE MsgContent[4096];         //消息内容
};
DECLARE_SERIALIZE_OPERATOR(MsgGsmrLocalMsg)



//根据车次号查询机车信息->
class AFX_EXT_CLASS MsgQueryLocomotiveInfo : public BaseMsg
{
	DECLARE_CLBTH(MsgQueryLocomotiveInfo)

public:
	MsgQueryLocomotiveInfo();
	virtual ~MsgQueryLocomotiveInfo();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
public:

	IWORD        console_no;    // 调度台 台号
	TRAIN_NO    train_id;       // 车次

};
DECLARE_SERIALIZE_OPERATOR(MsgQueryLocomotiveInfo)

//根据车次号查询机车信息->返回
class AFX_EXT_CLASS MsgLocomotiveInfo : public BaseMsg
{
	DECLARE_CLBTH(MsgLocomotiveInfo)

public:
	MsgLocomotiveInfo();
	virtual ~MsgLocomotiveInfo();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
public:

	IWORD        console_no;    // 调度台 台号
	TRAIN_NO    train_id;       // 车次
	short loco_count;    //让STPC知道抄送的列车
	struct LOCO_LIST{
		TRAIN_NO    locomotive;     // 机车号
		IDWORD       kilosignal;     // 公里标
	}loco_list[6];
};
DECLARE_SERIALIZE_OPERATOR(MsgLocomotiveInfo)

//设置电力臂状态
class AFX_EXT_CLASS MsgElecArmSet : public BaseMsg
{
	DECLARE_CLBTH(MsgElecArmSet)

public:
	MsgElecArmSet();
	virtual ~MsgElecArmSet();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
public:

	short      elec_arm_id;           // 电力臂号
	STATION_NO elec_sta;              // 车站号

	ENTITY_ID  set_entity;            // 设置者
	I_TIME       set_time;              // 设置时间

	ISDWORD       status;                // 供电状态
	char       remark[128];           // 注释

	enum { ELEC_ENABLE = 0xAA, ELEC_DISABLE = 0x55, PREP_ENABLE=0x5BEC, PREP_DISABLE=0x5BCE  };

};
DECLARE_SERIALIZE_OPERATOR(MsgElecArmSet)

//请求电力臂状态
class AFX_EXT_CLASS MsgElecArmQuery : public BaseMsg
{
	DECLARE_CLBTH(MsgElecArmQuery)

public:
	MsgElecArmQuery();
	virtual ~MsgElecArmQuery();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
public:

	short    elec_arm_list[128];      // 请求列表
	short    elec_arm_count;          // 请求个数

	STATION_NO   elec_sta;
	enum SOURCE { LIRC, GPC} query_source;
};
DECLARE_SERIALIZE_OPERATOR(MsgElecArmQuery)

//回复电力臂状态
class AFX_EXT_CLASS MsgElecArmAnswer : public MsgElecArmSet
{
	DECLARE_CLBTH(MsgElecArmAnswer)
public:
	MsgElecArmAnswer();
	virtual ~MsgElecArmAnswer();
private:
protected:

};
DECLARE_SERIALIZE_OPERATOR(MsgElecArmAnswer)

//早晚点传递
class AFX_EXT_CLASS MsgTrainDelayTime : public BaseMsg
{
	DECLARE_CLBTH(MsgTrainDelayTime)

public:
	MsgTrainDelayTime();
	virtual ~MsgTrainDelayTime();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
public:
	STATION_NO  nStationNo;//站号
	TRAIN_NO  tnTrain;//车次号
	ISDWORD     nDelayTime;//延迟时间
};
DECLARE_SERIALIZE_OPERATOR(MsgTrainDelayTime)

class AFX_EXT_CLASS MsgPackRequest: public BaseMsg
{
	DECLARE_CLBTH(MsgPackRequest)
public:
	MsgPackRequest();
	virtual ~MsgPackRequest();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	short        flag;   // set 0

	short        entity_count;//实体数
	ENTITY_ID    sender[16];//发送者
	ENTITY_ID    recver[16];//接收者
};
DECLARE_SERIALIZE_OPERATOR(MsgPackRequest)

class AFX_EXT_CLASS MsgCollector: public BaseMsg
{
	DECLARE_CLBTH(MsgCollector)
public:
	MsgCollector();
	virtual ~MsgCollector();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	ENTITY_ID    sender;//发送者
	ENTITY_ID    recver;//接收者

	MSGHEAD::msgid_t id;//消息id
	char         para[16];//参数
};
DECLARE_SERIALIZE_OPERATOR(MsgCollector)

class AFX_EXT_CLASS MsgPackAnswer: public BaseMsg
{
	DECLARE_CLBTH(MsgPackAnswer)
public:
	MsgPackAnswer();
	virtual ~MsgPackAnswer();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	short     msg_attrib;//消息描述
	ENTITY_ID dest_id;//目的实体
	ENTITY_ID source_id;//源实体
	I_TIME      tm;//
	I_TIME      subtm;//
	ISDWORD      label;//标识
	short     data_len;//
	short     check_sum;//校验数
	ISDWORD      spare;//

	short pack_len;//包长
	char  package[4000];//包内容
};

DECLARE_SERIALIZE_OPERATOR(MsgPackAnswer)


// 新调度消息定义

class AFX_EXT_CLASS MsgDCmdTitle : public BaseMsg
{
	DECLARE_CLBTH(MsgDCmdTitle)

public:
	MsgDCmdTitle();
	virtual ~MsgDCmdTitle();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
public:

	// 调度命令唯一识别 send_entity+create_time
	ENTITY_ID     send_entity;     // 发送实体
	I_TIME        create_time;     // 生成时间

	I_TIME        send_time;       // 发送时间
	I_TIME        edit_time;       // 编辑时间

	CONID         con_id;          // GPC发送时使用，调度台号码
	STATION_NO    source_station;  // STPC发送时使用，站号

	short         bian_ma;         // 命令类型编码
	char          cmd_no[16];      // 命令号码
	char          cmd_name[512];   // 命令类型名称,可以根据类型编码来得知
	char          rereader[32];    // 复颂人姓名
	char          dispatcher[32];  // 调度员姓名
	char          unit[32];        // 发令单位
	char          reader[32];      // 值班主任姓名
	I_TIME        read_time;       // 值班主任签收时间
	char          recver[128];     // 收令单位简表

	enum Status
	{
		EDIT,          //  编辑
		SEND,          //  发送
		RECV,          //  全部接收
		SIGN,          //  全部签收
		RE_SEND,       //  转发邻台命令标志
	}   status;

	enum CMD_DESTINATION
	{
		GPC_2_DIRECOR_ON_DUTY,
		DIRECOR_ON_DUTY_2_GPC,
		GPC_2_GPC,
		GPC_2_STPC,
		GPC_2_LIRC,
		GPC_2_STPC_AND_LIRC, //没有用到，实际上分两次发送的
		STPC_2_LIRC,
		STPC_2_GPC,          //调度命令请求
	} cmd_destination;

	enum CMD_OPER
	{
		NORMAL_CMD,          // 正常调度命令
		WORK_CMD,            // 施工调度命令
		TRANSFER_CMD,    // 转发调度命令，仅在号码不变时使用
		REQUEST_CMD          // 请求调度命令
	} cmd_oper;

	friend class MsgDCmdData;
};
DECLARE_SERIALIZE_OPERATOR(MsgDCmdTitle)

class AFX_EXT_CLASS MsgDCmdSendList : public BaseMsg
{
	DECLARE_CLBTH(MsgDCmdSendList)

public:
	MsgDCmdSendList();
	virtual ~MsgDCmdSendList();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
public:
	// 调度命令唯一识别
	ENTITY_ID     send_entity;    // 发送实体
	I_TIME        create_time;    // 生成时间

	//此项对无线调度命令
	enum CMD_TYPE
	{
		DISPATCH_COMMAND = 0x01,          // 调度命令
		ROUTE_TICKET,                     // 路票
		GREEN_ADMISSION,                  // 绿色许可证
		RED_ADMISSION,                    // 绿色许可证
		SPY_ON_SHUNT_NOTE,                // 出站跟踪调车通知单
		USE_LIGHT_CAR_ADMISSION,          // 轻型列车使用说明书
		FORETELL_TRAIN_ROUTE,             // 列车进路预告信息
		JUST_LINE_PASS,                   // 正线通过，
		BY_LINE_PASS,                     // 侧线通过，
		JUST_LINE_SLOW_PASS,              // 正线缓行通过，
		ENTER_JUST_LINE_STOP,             // 进站正线停车，
		ENTER_BY_LINE_STOP,               // 进站侧线停车，
		SHUNTING_JOB = 0x11,              // 调车作业
		SHUNTING_REQUEST_RECEIVED = 0x12, // 调车请求已收到
		IN_OUT_CHECK = 0x20,              // 出入库检测
		OTHER_COMMAND,                    // 其他命令
	} cmd_type;


	// 正常调度命令
	enum RECV_STATUS
	{
		COMMAND_RECV,                //命令接收
		COMMAND_SIGN,                //命令签收
		COMMAND_LOCOMOTIVE_RECV,     //机车接收
		COMMAND_LOCOMOTIVE_SIGN,     //司机签收
		COMMAND_SENDED,              // 自律机已发送
		COMMAND_ENTITY_DEAD,         //实体无响应
		COMMAND_ERROR,               //错误
		COMMAND_DENY,                // 拒绝
		COMMAND_INIT                 //初始化
	};

	//普通命令（对stpc,gpc）
	struct RECV_LIST
	{
		ENTITY_ID    recv_entity;         // 接受实体
		STATION_NO   station;             // 站号或gpc号
		RECV_STATUS  recv_status;         // 发送状态
		I_TIME       recv_time;           // 接受/签收时间
		char         sign_name[32];

		char         chaosong_memo[128];   // 抄送注释
	};

	short       recv_count;              // 发送数目,stpc or GPC
	RECV_LIST   recv_list[128];          // 发送列表,stpc or GPC

	short       chaosong_recv_count;     // 抄送数目,只对GPC有用
	RECV_LIST   chaosong_list[128];      // 抄送列表

	enum   SEND_WAY                      // 发送方式
	{
		SEND_INTELLIGENT,                  // 智能发送
		SEND_IMMEDIATELY                   // 立即发送
	} send_way;

	short  train_list_count;             // 无线调度命令的目标个数
	short  index;                        // 对自律机有用，当前收到的无线调度命令列表的索引，从0开始编号
	struct TRAIN_LIST
	{
		ENTITY_ID   recv_lirc;            // 接收实体
		RECV_STATUS recv_status;          // 发送状态
		I_TIME      recv_time;            // 接受/签收时间
		char        sign_name[32];        // 签收人

		TRAIN_NO    train_id;             // 车次， for wireless
		TRAIN_NO    locomotive;           // 机车号,for wireless
		STATION_NO  station;              // 接收站号
	}train_list[32];

	friend class MsgDCmdData;
};

DECLARE_SERIALIZE_OPERATOR(MsgDCmdSendList)

class AFX_EXT_CLASS MsgDCmdContent : public BaseMsg
{
	DECLARE_CLBTH(MsgDCmdContent)

public:
	MsgDCmdContent();
	virtual ~MsgDCmdContent();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
public:
	// 调度命令唯一识别
	ENTITY_ID   send_entity;    // 发送实体
	I_TIME      create_time;    // 生成时间

	short       content_seq;    // 正文数据包顺序, 从1开始
	char        content[2048];  // 正文数据包内容

	friend class MsgDCmdData;
};
DECLARE_SERIALIZE_OPERATOR(MsgDCmdContent)

class AFX_EXT_CLASS MsgDCmdData : public BaseMsg
{
	DECLARE_CLBTH(MsgDCmdData)

public:
	MsgDCmdData();
	virtual ~MsgDCmdData();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
public:
	MsgDCmdTitle       title;            // 命令标题
	MsgDCmdSendList    sendlist;         // 命令发送列表
	MsgDCmdContent     first_package;    // 第一包正文内容, content_seq设为0

	short  tcc_len;
	char   tcc_total[512];    // 由MsgDCmdTcc Serialize 写入

	ISDWORD   content_len;       // 除第一包的正文长度
	short     content_count;     // 除第一包的正文分包个数

	friend class MsgDCmdInsert;
	friend class MsgDCmdSend;
};
DECLARE_SERIALIZE_OPERATOR(MsgDCmdData)

class AFX_EXT_CLASS MsgDCmdInsert : public MsgDCmdData
{
	DECLARE_CLBTH(MsgDCmdInsert)

public:
	MsgDCmdInsert();
	virtual ~MsgDCmdInsert();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
public:


};
DECLARE_SERIALIZE_OPERATOR(MsgDCmdInsert)

class AFX_EXT_CLASS MsgDCmdDelete : public BaseMsg
{
	DECLARE_CLBTH(MsgDCmdDelete)

public:
	MsgDCmdDelete();
	virtual ~MsgDCmdDelete();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
public:
	CONID   con_id;//调度台号
	ENTITY_ID   send_entity;//命令发送实体
	I_TIME        create_time;//命令创建时间
};
DECLARE_SERIALIZE_OPERATOR(MsgDCmdDelete)


class AFX_EXT_CLASS MsgDCmdTitleLoad : public BaseMsg
{
	DECLARE_CLBTH(MsgDCmdTitleLoad)

public:
	MsgDCmdTitleLoad();
	virtual ~MsgDCmdTitleLoad();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
public:
	CONID       con_id;//调度台号
	I_TIME        begin_time;//开始时间
	I_TIME        end_time;//结束时间
};
DECLARE_SERIALIZE_OPERATOR(MsgDCmdTitleLoad)

class AFX_EXT_CLASS MsgDCmdLoad : public BaseMsg
{
	DECLARE_CLBTH(MsgDCmdLoad)

public:
	MsgDCmdLoad();
	virtual ~MsgDCmdLoad();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
public:
	ENTITY_ID   send_entity;//发送实体
	I_TIME        create_time;//创建时间
};
DECLARE_SERIALIZE_OPERATOR(MsgDCmdLoad)

class AFX_EXT_CLASS MsgDCmdLoad2 : public BaseMsg
{
	DECLARE_CLBTH(MsgDCmdLoad2)

public:
	MsgDCmdLoad2();
	virtual ~MsgDCmdLoad2();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
public:
	ENTITY_ID   send_entity;//命令发送实体
	I_TIME        create_time;//命令创建时间
	I_TIME        send_time;//命令发送时间
};
DECLARE_SERIALIZE_OPERATOR(MsgDCmdLoad2)
class AFX_EXT_CLASS MsgDCmdSend : public MsgDCmdData
{
	DECLARE_CLBTH(MsgDCmdSend)

public:
	MsgDCmdSend();
	virtual ~MsgDCmdSend();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
public:


};
DECLARE_SERIALIZE_OPERATOR(MsgDCmdSend)

class AFX_EXT_CLASS MsgDCmdRecv : public BaseMsg
{
	DECLARE_CLBTH(MsgDCmdRecv)

public:
	MsgDCmdRecv();
	virtual ~MsgDCmdRecv();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
public:
	// 调度命令唯一识别
	ENTITY_ID   send_entity;    // 发送实体
	I_TIME      create_time;    // 生成时间

	// 无线调度命令
	TRAIN_NO    train_id;       // 车次
	TRAIN_NO    locomotive;     // 机车号
	STATION_NO  station;        // 站号

	I_TIME      recv_time;      // 接收或签收时间
	ENTITY_ID   recv_entity;    // 接收或签收实体

	enum RECV_STATUS
	{
		COMMAND_RECV              = 0,// 命令接收
		COMMAND_SIGN              = 1,// 命令签收
		COMMAND_LOCOMOTIVE_RECV   = 2,// 机车接收
		COMMAND_LOCOMOTIVE_SIGN   = 3,// 司机签收
		COMMAND_SENDED            = 4,// 自律机已发送
		COMMAND_ENTITY_DEAD       = 5,// 实体无响应
		COMMAND_ERROR             = 6,// 错误
		COMMAND_DENY              = 7,// 拒绝
		COMMAND_INIT              = 8,// 初始化
		COMMAND_DEV_RECV          = 9,// 地面转接器回执

		COMMAND_ACK               = 21, // 同意
		COMMAND_READ              = 22, // 已阅读
		COMMAND_NACK              = 23, // 不同意
		COMMAND_SEND_FAIL         = 24, // 命令发送失败
		COMMAND_SEND_FAIL_LOCAL   = 25, // 本站发送失败
		COMMAND_WAIT_SEND         = 26  // 等待发送机车
	} recv_status;

	char    sign_name[32];  //签收姓名
	char    cmdno[16];      //命令号码
};
DECLARE_SERIALIZE_OPERATOR(MsgDCmdRecv)

class AFX_EXT_CLASS MsgDCmdStatus : public BaseMsg
{
	DECLARE_CLBTH(MsgDCmdStatus)

public:
	MsgDCmdStatus();
	virtual ~MsgDCmdStatus();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
public:
	MsgDCmdTitle       title;//命令标题
	MsgDCmdSendList    sendlist;//命令发送列表

	short  tcc_len;//tcc长度
	char   tcc_total[512];    // 由MsgDCmdTcc Serialize 写入

};
DECLARE_SERIALIZE_OPERATOR(MsgDCmdStatus)

class AFX_EXT_CLASS MsgDcmd_WirelessBroadcast:public BaseMsg
{
	DECLARE_CLBTH(MsgDcmd_WirelessBroadcast)

public:
	MsgDcmd_WirelessBroadcast();
	virtual ~MsgDcmd_WirelessBroadcast();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
public:
	char broadcast_id[128];   //这个是本次群发对应的调度命令Id，在返回消息中也要返回来，客户端用来查找对应调度命令，以更新其状态
	char dcmd_id[128];//uuid
	I_TIME gen_time;
	IBYTE is_startnow;      //是否“立即开始”
	IBYTE is_neverstop;     //是否“永久有效”
	I_TIME plan_start_time;   //计划开始时间，当“立即开始”为1时，此处为0
	I_TIME plan_end_time;     //计划结束时间，当“永久有效”为1时，此处为0

	IWORD station_id;

	//  char station_name[128];
	enum DIRECTION
	{
		DOWN_DIRECTION  = 1,  //1是下行
		UP_DIRECTION,     //2是上行
		ALL_DIRECTION,      //3是上下行
	}
	direction;
	enum STATUS
	{
		EDIT,       //  0：未下发
		SEND,       //  1：己发到自律机，但处于等待状态
		BEGIN_BROADCAST,    //  2：己开始群发
		END_BROADCAST,    //  3：己中止群发
		CANCEL        //  4：命令已取消
	}
	status;
	enum TRAINTYPE
	{
		PASSENGER_CAR = 1,  //1:客车
		FREIGHT_WAGGON,     //2:货车
		ALL_TRAIN,        //3:客货车
	}
	train_type;

	I_TIME real_start_time;//真正的开始时间
	I_TIME real_end_time;//真正的结束时间

	friend class MsgDcmd_BroadCast_Send;
	friend class MsgDcmd_BroadCast_Command;
};

class AFX_EXT_CLASS MsgDcmd_BroadCast_Send : public BaseMsg
{
	DECLARE_CLBTH(MsgDcmd_BroadCast_Send)

public:
	MsgDcmd_BroadCast_Send();
	virtual ~MsgDcmd_BroadCast_Send();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
public:
	enum { MAX_SEND_COUNT = 16 };
	MsgDcmd_WirelessBroadcast wbInfo[MAX_SEND_COUNT];
	IBYTE  nSendDestCount;
	IBYTE  nUsedIndex;

	struct DCMD
	{
		char dcmd_id[128];//uuid
		char dcmd_style_title[128];//title
		char dcmd_num[16];//cmdno
		char org_cmd_num[16];//

		ENTITY_ID send_entity;//发送实体
		I_TIME      create_time;//命令创建时间
		char    creater_name[32];//命令创建者名字
		I_TIME    send_time;//命令发送时间
		I_TIME    edit_time;//命令编辑时间

		IBYTE   sender_type;//发送者类型
		IDWORD    sender_id;//发送者id
		char    sender_name[32];//发送者姓名
		char    sender_unit_name[32];//
		char    rereader_name[32];//
		char    dutychief_name[32];//
		I_TIME    author_time;//
		IBYTE   author_status;//
		char    recver[512];//接收者

		//此项对无线调度命令
		enum CMD_TYPE
		{
			DISPATCH_COMMAND = 0x01, // 调度命令
			ROUTE_TICKET,          // 路票
			GREEN_ADMISSION,         // 绿色许可证
			RED_ADMISSION,           // 绿色许可证
			SPY_ON_SHUNT_NOTE,       // 出站跟踪调车通知单
			USE_LIGHT_CAR_ADMISSION, // 轻型列车使用说明书
			FORETELL_TRAIN_ROUTE,    // 列车进路预告信息
			JUST_LINE_PASS,          // 正线通过，
			BY_LINE_PASS,            // 侧线通过，
			JUST_LINE_SLOW_PASS,     // 正线缓行通过，
			ENTER_JUST_LINE_STOP,    // 进站正线停车，
			ENTER_BY_LINE_STOP,      // 进站侧线停车，
			SHUNTING_JOB = 0x11,     // 调车作业
			SHUNTING_REQUEST_RECEIVED = 0x12, // 调车请求已收到
			IN_OUT_CHECK = 0x20,              // 出入库检测
			OTHER_COMMAND,                    // 其他命令
		}
		cmd_type;

		enum CMD_DESTINATION
		{
			GPC_2_DIRECOR_ON_DUTY,
			DIRECOR_ON_DUTY_2_GPC,
			GPC_2_GPC,
			GPC_2_STPC,
			GPC_2_LIRC,
			GPC_2_STPC_AND_LIRC, //没有用到，实际上分两次发送的
			STPC_2_LIRC,
			STPC_2_GPC,          //调度命令请求
		}
		cmd_destination;

		enum CMD_OPER
		{
			NORMAL_CMD,          // 正常调度命令
			WORK_CMD,            // 施工调度命令
			TRANSFER_CMD,    // 转发调度命令，仅在号码不变时使用
			REQUEST_CMD          // 请求调度命令
		}
		cmd_oper;

		IBYTE need_author;
		int     tcc_len;

		enum   SEND_WAY                        // 发送方式
		{
			SEND_INTELLIGENT,                  // 智能发送
			SEND_IMMEDIATELY                   // 立即发送
		}
		send_way;

		IWORD status;
		char    jhh[64];    // 施工计划号
		char        content[2048];  // 正文数据包内容
	}
	dcmdcontent;
};
DECLARE_SERIALIZE_OPERATOR(MsgDcmd_BroadCast_Send)

class AFX_EXT_CLASS MsgDcmd_BroadCast_Status : public BaseMsg
{
	DECLARE_CLBTH(MsgDcmd_BroadCast_Status)

public:
	MsgDcmd_BroadCast_Status();
	virtual ~MsgDcmd_BroadCast_Status();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
public:
	char broadcast_id[128];   //这个是本次群发对应的调度命令Id，在返回消息中也要返回来，客户端用来查找对应调度命令，以更新其界面显示状态
	char dcmd_id[128];      //命令号
	ENTITY_ID station_entity;   //车站 通信实体号
	char station_name[128];   //站名，可有可无。
	enum DIRECTION
	{
		DOWN_DIRECTION  = 1,  //1是下行
		UP_DIRECTION,     //2是上行
		ALL_DIRECTION,      //3是上下行
	}
	direction;  //方向
	IWORD status;       //执行状态，将来要根据自律机返回的结果来刷新的，0：未下发 1：
	I_TIME oper_time;//操作时间
};
DECLARE_SERIALIZE_OPERATOR(MsgDcmd_BroadCast_Status)

class AFX_EXT_CLASS MsgDcmd_BroadCast_Command : public BaseMsg
{
	DECLARE_CLBTH(MsgDcmd_BroadCast_Command)

public:
	MsgDcmd_BroadCast_Command();
	virtual ~MsgDcmd_BroadCast_Command();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
public:
	//  MsgDcmd_WirelessBroadcast wbInfo;
	char broadcast_id[128];   //这个是本次群发对应的调度命令Id，在返回消息中也要返回来，客户端用来查找对应调度命令，以更新其界面显示状态
	char dcmd_id[128];
	IWORD commandid;              //0为查询当前执行状态，1为立即开始 ，2为立即中止
};
DECLARE_SERIALIZE_OPERATOR(MsgDcmd_BroadCast_Command)

class AFX_EXT_CLASS MsgDCmdCurrentNoRequest : public BaseMsg
{
	DECLARE_CLBTH(MsgDCmdCurrentNoRequest)

public:
	MsgDCmdCurrentNoRequest();
	virtual ~MsgDCmdCurrentNoRequest();

protected:
	virtual HRCODE doSerialize(Archive& ar);
	virtual size_t countObjectLength();
public:
	// 调度命令唯一识别
	ENTITY_ID   send_entity;    // 发送实体
	I_TIME        create_time;    // 生成时间

	enum CMD_OPER
	{
		NORMAL_CMD,          // 正常调度命令
		WORK_CMD             // 施工调度命令
	} cmd_oper;

	enum FLAG
	{
		EDIT,//编辑
		SEND//发送
	} flag;

	CONID       conid;//调度台号

	friend class MsgDCmdCurrentNo;
};
DECLARE_SERIALIZE_OPERATOR(MsgDCmdCurrentNoRequest)

class AFX_EXT_CLASS MsgDCmdCurrentNoRequest2 : public BaseMsg
{
	DECLARE_CLBTH(MsgDCmdCurrentNoRequest2)

public:
	MsgDCmdCurrentNoRequest2();
	virtual ~MsgDCmdCurrentNoRequest2();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
public:
	// 调度命令唯一识别
	ENTITY_ID   send_entity;    // 发送实体
	I_TIME        create_time;    // 生成时间

	enum CMD_OPER
	{
		NORMAL_CMD,          // 正常调度命令
		WORK_CMD             // 施工调度命令
	} cmd_oper;

	enum FLAG
	{
		EDIT,//编辑
		SEND//发送
	} flag;

	CONID       conid;//调度台号

	char   begin_cmdno[16];//开始命令号
	char   end_cmdno[16];//结束命令号

	friend class MsgDCmdCurrentNo;
};
DECLARE_SERIALIZE_OPERATOR(MsgDCmdCurrentNoRequest2)

class AFX_EXT_CLASS MsgDCmdCurrentNo : public MsgDCmdCurrentNoRequest
{
	DECLARE_CLBTH(MsgDCmdCurrentNo)

public:
	MsgDCmdCurrentNo();
	virtual ~MsgDCmdCurrentNo();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
public:
	char   cmdno[16];//命令号
	I_TIME   send_time;//发送时间
};
DECLARE_SERIALIZE_OPERATOR(MsgDCmdCurrentNo)

class AFX_EXT_CLASS MsgDCmdRefresh : public BaseMsg
{
	DECLARE_CLBTH(MsgDCmdRefresh)

public:
	MsgDCmdRefresh();
	virtual ~MsgDCmdRefresh();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
public:
	CONID      conid;//调度台号

	ENTITY_ID  send_entity;//发送实体号
	I_TIME       create_time;//创建命令时间

	I_TIME       ltime;//
	ENTITY_ID  source;//源实体号
};
DECLARE_SERIALIZE_OPERATOR(MsgDCmdRefresh)

class AFX_EXT_CLASS MsgDCmdTitleClose : public MsgDCmdTitleLoad
{
	DECLARE_CLBTH(MsgDCmdTitleClose)
public:
	MsgDCmdTitleClose();
	virtual ~MsgDCmdTitleClose();
};
DECLARE_SERIALIZE_OPERATOR(MsgDCmdTitleClose)

class AFX_EXT_CLASS MsgDCmdRequest : public BaseMsg
{
	DECLARE_CLBTH(MsgDCmdRequest)

public:
	MsgDCmdRequest();
	virtual ~MsgDCmdRequest();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
public:
	char   cmdno[16];//命令号
};
DECLARE_SERIALIZE_OPERATOR(MsgDCmdRequest)

class AFX_EXT_CLASS MsgDCmdAnswer : public BaseMsg
{
	DECLARE_CLBTH(MsgDCmdAnswer)

public:
	MsgDCmdAnswer();
	virtual ~MsgDCmdAnswer();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
public:

	char   cmdno[16];//命令号
	short  status;   //命令回执状态
};
DECLARE_SERIALIZE_OPERATOR(MsgDCmdAnswer)


//需要应答的消息
class AFX_EXT_CLASS MsgGpcMessage: public BaseMsg
{
	DECLARE_CLBTH(MsgGpcMessage)
public:
	MsgGpcMessage();
	virtual ~MsgGpcMessage();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	ISDWORD      label;     //消息标示符

	ENTITY_ID source_id;//源实体号
	ENTITY_ID dest_id;//目的实体号

	enum ACTION_TYPE {
		Msg_REQUEST = 0,   //请求消息
		Msg_REPLY          //回应消息
	};
	enum ANSWER_TYPE {
		ANSWER_OK = 0,     //确认
		ANSWER_DENAY       //拒绝
	};
	IBYTE cmd_type;//命令类型
	IBYTE answer_type;//答复类型

	I_TIME      tm_request; //请求时间
	I_TIME      tm_reply;   //响应时间

	ISDWORD      spare;

	short pack_len;       //消息长度
	char  package[1024];  //消息内容
};
DECLARE_SERIALIZE_OPERATOR(MsgGpcMessage)

// 直接输出控制
class AFX_EXT_CLASS MsgDirectOutput: public BaseMsg
{
	DECLARE_CLBTH(MsgDirectOutput)
public:
	MsgDirectOutput();
	virtual ~MsgDirectOutput();
private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	bool AddOutput(IWORD handle,IWORD flag);
	int  GetCount();
	bool GetOutput(int index,IWORD &handle,IWORD &flag);
private:
	int   nOutputHandleCount;
	int   nOutputHandles[16];
	int   nOutputHandleChecks[16];  // 用于校验
};
DECLARE_SERIALIZE_OPERATOR(MsgStccCommandSendResult2)

//数据请求2
class AFX_EXT_CLASS MsgDataStyleReq2:public BaseMsg
{
	DECLARE_CLBTH(MsgDataStyleReq2)

public:
	MsgDataStyleReq2();
	virtual ~MsgDataStyleReq2();

protected:
	virtual HRCODE doSerialize(Archive& ar);
	virtual size_t countObjectLength();

public:
	enum DataStyle
	{
		DISPLAY   = 0x0001,
		RENTID    = 0x0002,
		REPORTTIME  = 0x0004,
		BITBLOCK  = 0x0008,
		LIRCSTATUS  = 0x0010,
		SCHD_TRAIN  = 0x0020,
		SCHD_BLOCK  = 0x0040,
		HSTY_TRAIN  = 0x0080,
		HSTY_BLOCK  = 0x0100,
		PREV_TRAIN  = 0x0200,
		PREV_BLOCK  = 0x0400,
		BASC_TRAIN  = 0x0800,
		BASC_BLOCK  = 0x1000,
		SHIFT_TRAIN = 0x2000,
		SHIFT_BLOCK = 0x4000,

		SHUNT         = 0x00008000,
		MMPLAN      = 0x00010000,
		BASIC_ARCHIVE = 0x00020000,
		SCHEDULE_NAME = 0x00040000,
		TRAINLOG      = 0x00080000,

		//    WHOLE_TRAIN  =  0x0020,
		//    WHOLE_BLOCK  =  0x0040,

		WHOLE_TRAIN  =  0x00100000,
		WHOLE_BLOCK  =  0x00200000,
		WBASIC_TRAIN =  0x00400000,
	};
	enum ReqStatus
	{
		LOAD,
		CLOSE
	};

	IBYTE nStatCount;//状态数
	STATION_NO nStno[LOGIN_MAX_STATION_COUNT];//站号
	IDWORD      nStyle[LOGIN_MAX_STATION_COUNT];//
	ReqStatus  nStatus[LOGIN_MAX_STATION_COUNT];//状态

	I_TIME  nStartTime;
	I_TIME  nEndTime;
};
DECLARE_SERIALIZE_OPERATOR(MsgDataStyleReq2)

//---------------------------- RBC 消息----------------------------------------------------------------

// RBC列车信息
class AFX_EXT_CLASS MsgRBCTrainInfo: public BaseMsg
{
	DECLARE_CLBTH(MsgRBCTrainInfo)
public:
	MsgRBCTrainInfo();
	virtual ~MsgRBCTrainInfo();
private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	IDWORD    nUniqueID;      // RBC提供的列车唯一识别号
	IWORD   nRBCID;       // 列车所属RBC编号
	char      strRBCName[32];   // RBC名称
	TRAIN_NO  tnTrainNo;      // 车次号
	IWORD   nTrainLen;      // 列车长度
	IWORD   nMaxSpeed;      // 最大速度(km/h)

	IWORD   nTrainMode;     // 车载运营模式
	// 0:完全监控; 1:目视; 2:员工负责; 3:调车; 4:休眠; 5:待机; 6:隔离
	// 7:冒进8:冒进后13:STM  15:不适用
	IBYTE   nTrainLevel;      // CTCS级别
	// 0:0级; 1:STM级; 2:1级; 3:2级; 4:3级; 0xFF:无数据

	IWORD   nSpeed;       // 速度(km/h)

	IWORD   nPositionStatus;    // 位置有效性
	// 0:未知; 1:无效; 2:近似; 3:有效; 4:ERTMS区域之外

	IWORD   nEmergencyStop; // 紧急停车
	// 0:否;  1:是
	IDWORD    nRequestAct;    // 预留
	IBYTE   nDir;       // 列车运行方向，0:未知 1:正向, 2:反向

	enum { DEV_NAME_LEN = 16 };
	struct DEVINFO
	{
		IWORD   nID;
		STATION_NO  nSta;
		char      strName[DEV_NAME_LEN];
	};

	DEVINFO   sTrainPos;      // 列车位置

	IDWORD    nTrainOffset;   // 武广为距前方信号机距离
	// 郑西为最大前端公里标
	DEVINFO   sEOA;       // 移动授权终点，
};
DECLARE_SERIALIZE_OPERATOR(MsgRBCTrainInfo)

// 列车操作(检索与删除)
class AFX_EXT_CLASS MsgRBCTrainOperation: public BaseMsg
{
	DECLARE_CLBTH(MsgRBCTrainOperation)
public:
	MsgRBCTrainOperation();
	virtual ~MsgRBCTrainOperation();
private:
	virtual HRCODE doSerialize(Archive& ar);

public:
	IDWORD    nUniqueID;  // RBC提供的列车唯一识别号
	enum { TRN_QUERY, TRN_REMOVE };
	IBYTE   nOper;    // 查询或删除
};
DECLARE_SERIALIZE_OPERATOR(MsgRBCTrainOperation)

// RBC报警信息
class AFX_EXT_CLASS MsgRBCAlarm: public BaseMsg
{
	DECLARE_CLBTH(MsgRBCAlarm)
public:
	MsgRBCAlarm();
	virtual ~MsgRBCAlarm();
private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	IWORD   nAlarmGroup;        // 报警组别
	IWORD   nAlarmCode;     // 报警代码
	I_TIME    nAlarmTime;     // 时间
	IDWORD    nMilliSeconds;      // 时间(毫秒)
	IWORD   nPriority;        // 等级
	IWORD   nAlarmState;      // 状态
	char      strSite[64];        // 报警发生地
	char      strObject[64];      // 报警对象
	char      strContent[256];      // 报警内容
};
DECLARE_SERIALIZE_OPERATOR(MsgRBCAlarm)

// RBC连接状态报告
class AFX_EXT_CLASS MsgRBCStatusReport : public BaseMsg
{
	DECLARE_CLBTH(MsgRBCStatusReport)
public:
	MsgRBCStatusReport();
	virtual ~MsgRBCStatusReport();
private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	int nCount; // RBC个数
	enum { MAX_RBC_COUNT = 64 };
	struct
	{
		IWORD nRBCID;//RBC号
		WORK_STATUS nStatus;//状态
	} szStatus[MAX_RBC_COUNT];
};
DECLARE_SERIALIZE_OPERATOR(MsgRBCStatusReport)

//文本信息
class AFX_EXT_CLASS MsgRBCTxt : public BaseMsg
{
	DECLARE_CLBTH(MsgRBCTxt)
public:
	MsgRBCTxt();
	virtual ~MsgRBCTxt();
private:
	virtual HRCODE doSerialize(Archive& ar);

public:
	IWORD   nOperID;    // 操作号
	ENTITY_ID nSender;    // 命令发送者的entity
	TRAIN_NO  tnTrain;    // 车次号
	IDWORD    nNIDEngine;   // 列车ID（C3唯一标识）
	char        strTxt[128];    //文本信息

	//YB.P add @2011-07-11
	int nConfirm;
	int nDispTime;
};
DECLARE_SERIALIZE_OPERATOR(MsgRBCTxt)

// 紧急停车命令发送
class AFX_EXT_CLASS MsgRBCEmergencySend : public BaseMsg
{
	DECLARE_CLBTH(MsgRBCEmergencySend)
public:
	MsgRBCEmergencySend();
	virtual ~MsgRBCEmergencySend();
private:
	virtual HRCODE doSerialize(Archive& ar);

public:
	IWORD   nOperID;    // 操作号
	ENTITY_ID nSender;    // 命令发送者的entity
	TRAIN_NO  tnTrain;    // 车次号
	IDWORD    nNIDEngine; // 列车ID（C3唯一标识）
	I_TIME    tmSend;   // 命令发送时间
	IBYTE   nFlag;    // 操作标记（0x55：设置紧急，0xAA：取消紧急）
};
DECLARE_SERIALIZE_OPERATOR(MsgRBCEmergencySend)

// 紧急停车命令发送回执
class AFX_EXT_CLASS MsgRBCEmergencyResult : public BaseMsg
{
	DECLARE_CLBTH(MsgRBCEmergencyResult)
public:
	MsgRBCEmergencyResult();
	virtual ~MsgRBCEmergencyResult();
private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	IWORD   nOperID;    // 操作号
	ENTITY_ID nSender;    // 命令发送者的entity
	TRAIN_NO  tnTrain;    // 车次号
	IDWORD    nNIDEngine; // 列车ID（C3唯一标识）
	I_TIME    tmSend;   // 命令发送时间
	IBYTE   nFlag;    // 操作标记（0x55：设置紧急，0xAA：取消紧急）
	IBYTE   nStatus;    // 回执（定义如下）
	enum
	{
		CTC_RECV=0,     // RBC通信机收到命令
		CTC_SEND,     // RBC通信机已发出命令
		RBC_RECV,     // RBC已接收命令（收到成功的反馈）
		RBC_REJECT,   // RBC不接收命令（收到不成功反馈）
		CMD_OK,     // 命令已执行，列车状态改变
		CMD_TIMEOUT,  // 命令超时，列车状态未改变
		CMD_NOTSEND   // 条件不满足，不能发送
	};
	char  strResult[256];   // 注释字符串，用于显示武广协议命令不成功时的反馈信息
};
DECLARE_SERIALIZE_OPERATOR(MsgRBCEmergencyResult)

//-------------------------------------------------------------------------------------------------------------------

class AFX_EXT_CLASS MsgParamentRequest:public BaseMsg
{
	DECLARE_CLBTH(MsgParamentRequest)

public:
	MsgParamentRequest();
	virtual ~MsgParamentRequest();

protected:
	virtual HRCODE doSerialize(Archive& ar);
	virtual size_t countObjectLength();

public:
	ENTITY_ID  entity;     // INVALID_ENTITY_ID for ALL
	int        scope;
	short      type_count;
	int        type[4];
	enum PARA_TYPE
	{ 
		MAINTN,
		WINTG , 
		LAYOUT,
		DCMD  
	} ;
};
DECLARE_SERIALIZE_OPERATOR(MsgParamentRequest)

class AFX_EXT_CLASS MsgParamentReply:public MsgParamentRequest
{
	DECLARE_CLBTH(MsgParamentReply)

public:
	MsgParamentReply();
	virtual ~MsgParamentReply();

protected:
	virtual HRCODE doSerialize(Archive& ar);
	virtual size_t countObjectLength();

public:
	int        int_parament[4];
	char       str_parament[4][2048];

	/* 
	TYPE: MAINTN
	int_parament:
	bit0 :  权限分配权限

	TYPE: WINTG 
	int_parament:
	bit0: 运行图查询权限
	bit1: 运行线编辑权限
	bit2: 施工符号编辑权限
	bit3: 基本图修改权限
	bit4: 基本图更换权限
	bit5: 实际图修改权限*
	bit6: 接收日班计划权限
	bit7: 下计划权限

	*/

};
DECLARE_SERIALIZE_OPERATOR(MsgParamentReply)

class AFX_EXT_CLASS MsgParamentOperation: public MsgParamentReply
{
	DECLARE_CLBTH(MsgParamentOperation)

protected:
	virtual HRCODE doSerialize(Archive& ar);
	virtual size_t countObjectLength();

public:
	MsgParamentOperation();
	virtual ~MsgParamentOperation();

public:
	enum Operation {
		PARAOPER_SERVER_ECHO_OK,
		PARAOPER_SERVER_ECHO_FAIL,
		PARAOPER_INSERT_UPDATE,
		PARAOPER_DELETE
	}                 oper;
};
DECLARE_SERIALIZE_OPERATOR(MsgParamentOperation)

// 设置当班调度员（主调）姓名消息
class AFX_EXT_CLASS MsgSetDDyName: public BaseMsg
{
	DECLARE_CLBTH(MsgSetDDyName)

public:
	MsgSetDDyName();
	virtual ~MsgSetDDyName();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	CONID      conid;    //调度台id
	SHIFT_ID   shift_id; //日班计划id
	USERNAME   user_name;//用户名
};
DECLARE_SERIALIZE_OPERATOR(MsgSetDDyName)


class AFX_EXT_CLASS MsgGetDDyName: public BaseMsg
{
	DECLARE_CLBTH(MsgGetDDyName)

public:
	MsgGetDDyName();
	virtual ~MsgGetDDyName();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	CONID      conid;//调度台id
	SHIFT_ID   shift_id;//日班计划id
};
DECLARE_SERIALIZE_OPERATOR(MsgGetDDyName)

class AFX_EXT_CLASS MsgGiveDDyName: public BaseMsg
{
	DECLARE_CLBTH(MsgGiveDDyName)

public:
	MsgGiveDDyName();
	virtual ~MsgGiveDDyName();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	CONID      conid;//调度台id
	SHIFT_ID   shift_id;//日班计划id
	USERNAME   user_name1;//用户名1
	USERNAME   user_name2;//用户名2
};
DECLARE_SERIALIZE_OPERATOR(MsgGiveDDyName)

class AFX_EXT_CLASS MsgReportTimeTransfer :public MsgReportTime
{
	DECLARE_CLBTH(MsgReportTimeTransfer)
public:
	MsgReportTimeTransfer();
	MsgReportTimeTransfer(const MsgReportTime& msg);
	virtual ~MsgReportTimeTransfer();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	STATION_NO transfer_sta;//转换车站
};
DECLARE_SERIALIZE_OPERATOR(MsgReportTimeTransfer)

class AFX_EXT_CLASS MsgStringMessage:public BaseMsg
{
	DECLARE_CLBTH(MsgStringMessage)

public:
	MsgStringMessage();
	virtual ~MsgStringMessage();

protected:
	virtual HRCODE doSerialize(Archive& ar);
	virtual size_t countObjectLength();

	IDWORD  len;//消息长度
	char * pcontent;//

private:
	char token[128];//标识
	char content[4096];//内容

	void expand(IDWORD new_len);

public:
	I_BOOL SetTokenContent(const char *sToken,const char* sText, int nTextLength=-1);
	const char* GetToken()  const;
	const char* GetContent() const;
	IDWORD GetContentLength() const;
};
DECLARE_SERIALIZE_OPERATOR(MsgStringMessage)

class AFX_EXT_CLASS MsgStringMessage2 : public BaseMsg
{
	DECLARE_CLBTH(MsgStringMessage2)

public:
	MsgStringMessage2();
	virtual ~MsgStringMessage2();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	char token[128];//标示
	char content[4096];//内容

	char sysid[128];//
	IDWORD package_index;//包序
	IDWORD package_count;//包数
	IDWORD totallength;//总长
	//非消息内容
	IDWORD currentlength;//当前长度
	char *pcontent; //
	I_TIME packagetime;//
public:
	I_BOOL SetTokenContent(const char *sToken,const char* sText);
	I_BOOL SetPackageInfo(const char *sSysid,IDWORD nCount,IDWORD nIndex,IDWORD nLength);
	const char* GetToken() const;
	const char* GetContent() const;

	const char* GetSysid() const;
	IDWORD GetPackageIndex() const;
	IDWORD GetPackageCount() const;
	IDWORD GetTotalLength() const;

	I_BOOL IsSamePackage(const MsgStringMessage2* msgSM2) const;
	I_TIME GetPackageTime() const;
	ISDWORD SelfPackage();
	ISDWORD Package(const MsgStringMessage2* msgSM2);
};
DECLARE_SERIALIZE_OPERATOR(MsgStringMessage2)


//车务终端数据请求
class AFX_EXT_CLASS MsgDataStyleReq3:public MsgDataStyleReq2
{
	DECLARE_CLBTH(MsgDataStyleReq3)

public:
	MsgDataStyleReq3();
	virtual ~MsgDataStyleReq3();

	CONID nConId[LOGIN_MAX_STATION_COUNT];

protected:
	virtual HRCODE doSerialize(Archive& ar);
	virtual size_t countObjectLength();

};
DECLARE_SERIALIZE_OPERATOR(MsgDataStyleReq3)

//归档基本图数据请求
class AFX_EXT_CLASS MsgDataStyleReq4:public MsgDataStyleReq2
{
	DECLARE_CLBTH(MsgDataStyleReq4)

public:
	MsgDataStyleReq4();
	virtual ~MsgDataStyleReq4();

	SCHEDULE_ID nScheduleId[LOGIN_MAX_STATION_COUNT];

protected:
	virtual HRCODE doSerialize(Archive& ar);
	virtual size_t countObjectLength();

};
DECLARE_SERIALIZE_OPERATOR(MsgDataStyleReq4)
// 列车预告信息
class AFX_EXT_CLASS MsgTrainDepartPredict : public BaseMsg
{
	DECLARE_CLBTH(MsgTrainDepartPredict)

public:
	MsgTrainDepartPredict();
	virtual ~MsgTrainDepartPredict();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	STATION_NO nSourceStno;   // 发车车站
	STATION_NO nDestStno;   // 发车目标车站
	ENTRY_ID nDestEntry;    // 目标车站接车入口
	TRAIN_NO strTid;      // 车次号
	TRAIN_INDEX nTrainIndex;  //车次索引
	IBYTE nRecIndex;//接收索引

	ENTITY_ID nSrcEntity;   // 发送预告的实体号

	enum { LIRC, STPC, GPC };
	IBYTE nSrcType;       // 发送预告的实体类型

	enum
	{
		YUGAO,    // 发车预告
		SHOUDAO,    // 收到发车预告
		TONGYI,   // 同意发车
		QUXIAO    // 取消发车
	};
	IBYTE nOperType;        // 预告类型

	I_TIME nOperTime;       // 预告时间
};
DECLARE_SERIALIZE_OPERATOR(MsgTrainDepartPredict)

// 联锁设备报警
class AFX_EXT_CLASS MsgCISFIRAlarm : public BaseMsg
{
	DECLARE_CLBTH(MsgCISFIRAlarm)

public:
	MsgCISFIRAlarm();
	virtual ~MsgCISFIRAlarm();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	STATION_NO nStno;                // 车站号
	enum { MAX_FIR_LEN = 512 };
	IBYTE nzFIRInfo[MAX_FIR_LEN];    // 报警信息
	IWORD nFIRLen;                   // 报警信息长度
};
DECLARE_SERIALIZE_OPERATOR(MsgCISFIRAlarm)


//轨道电路消息，自律机发来
class AFX_EXT_CLASS MsgTrackCircuit : public BaseMsg
{
	DECLARE_CLBTH(MsgTrackCircuit)

public:
	MsgTrackCircuit();
	virtual ~MsgTrackCircuit();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	enum { TRACKCIRCUIT_COUNT = 30 };
	IBYTE nzTrackCircuitInfo[TRACKCIRCUIT_COUNT];//轨道电路信息
	IWORD nTrackcircuitLen;//轨道电路长度
};
DECLARE_SERIALIZE_OPERATOR(MsgTrackCircuit)

// 运行图修改自触
class AFX_EXT_CLASS MsgStPlanAutoTrigUpdate : public BaseMsg
{
	DECLARE_CLBTH(MsgStPlanAutoTrigUpdate)

public:
	MsgStPlanAutoTrigUpdate();
	virtual ~MsgStPlanAutoTrigUpdate();

private:
	virtual HRCODE doSerialize(Archive& ar);

public:
	CONID          con_id;
	ENTITY_ID      gpc;           //gpc实体号
	TRAIN_INDEX    train_index;   // 列车索引
	IBYTE          rec_index;     // 记录索引
	STATION_NO     station;       // 车站站号

	enum { ARRIVE, DEPART };      //到达、出发
	IBYTE run_type;               //运行类型
	IBYTE trig_flag;              // 0:不自触, 1: 自触
};
DECLARE_SERIALIZE_OPERATOR(MsgStPlanAutoTrigUpdate)

// 运行图修改自触回执
class AFX_EXT_CLASS MsgStPlanAutoTrigUpdateResult : public BaseMsg
{
	DECLARE_CLBTH(MsgStPlanAutoTrigUpdateResult)

public:
	MsgStPlanAutoTrigUpdateResult();
	virtual ~MsgStPlanAutoTrigUpdateResult();

private:
	virtual HRCODE doSerialize(Archive& ar);

public:
	CONID          con_id;
	TRAIN_INDEX    train_index;                     // 列车索引
	IBYTE           rec_index;                       // 记录索引
	STATION_NO     station;                         // 车站站号

	enum { ARRIVE, DEPART };  //到、开
	IBYTE run_type;
	IBYTE trig_flag;    // 0:不自触, 1: 自触

	enum { PLAN_UPDATED, PLAN_NOT_FOUND };//计划更新、计划不更新
	IBYTE plan_res;

	enum { ROUTE_UPDATED, ROUTE_NOT_UPDATE }; //进路更新,进路不更新
	IBYTE route_res;
};
DECLARE_SERIALIZE_OPERATOR(MsgStPlanAutoTrigUpdateResult)

// 调车作业单新增消息
// 调车状态变化
class AFX_EXT_CLASS MsgShuntStatusUpdate : public BaseMsg
{
	DECLARE_CLBTH(MsgShuntStatusUpdate)

public:
	MsgShuntStatusUpdate();
	virtual ~MsgShuntStatusUpdate();

private:
	virtual HRCODE doSerialize(Archive& ar);

public:
	STATION_NO nStationNo; // 站号
	SHUNT_INDEX nShuntIndex; // 调车作业单唯一编号
	IWORD nGouIndex; // 勾序号
	IBYTE nStatus; // 状态
	enum
	{
		NOTE_NOT_START,
		NOTE_START,
		NOTE_FINISH,
		GOU_NOT_START,
		GOU_START,
		GOU_FINISH
	};
};
DECLARE_SERIALIZE_OPERATOR(MsgShuntStatusUpdate)

// 调车报点
class AFX_EXT_CLASS MsgShuntTimeReport : public BaseMsg
{
	DECLARE_CLBTH(MsgShuntTimeReport)

public:
	MsgShuntTimeReport();
	virtual ~MsgShuntTimeReport();

private:
	virtual HRCODE doSerialize(Archive& ar);

public:
	STATION_NO nStationNo; // 站号
	SHUNT_INDEX nShuntIndex; // 调车作业单唯一编号
	IWORD nGouIndex; // 勾序号
	IBYTE nTimeType; // 报点类型
	enum
	{
		NOTE_FINISH,
		GOU_FINISH
	};
	IBYTE nOperType; // 操作类型
	enum
	{
		MANUAL_REPORT,  //手工报点
		AUTO_REPORT //自动报点
	};
	I_TIME tmRptTime; // 报点时间
};
DECLARE_SERIALIZE_OPERATOR(MsgShuntTimeReport)

// 调车请求
class AFX_EXT_CLASS MsgShuntRouteGroupRequest : public BaseMsg
{
	DECLARE_CLBTH(MsgShuntRouteGroupRequest)

public:
	MsgShuntRouteGroupRequest();
	virtual ~MsgShuntRouteGroupRequest();

private:
	virtual HRCODE doSerialize(Archive& ar);

public:
	STATION_NO nStationNo; // 站号
	ROUTE_LIST_HANDLE nHandle; // 调车作业单唯一编号
	IWORD nGouIndex; // 勾序号
	I_TIME tmReqTime; // 请求时间
};
DECLARE_SERIALIZE_OPERATOR(MsgShuntRouteGroupRequest)


// 调机号操作，包括添加、删除、修改
class AFX_EXT_CLASS MsgShuntLocoNoOper : public BaseMsg
{
	DECLARE_CLBTH(MsgShuntLocoNoOper)

public:
	MsgShuntLocoNoOper();
	virtual ~MsgShuntLocoNoOper();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	STATION_NO  nStationNo; //站号
	DEVICE_TYPE nDmisType;  //主要是道岔和股道
	DMIS_ID   nDmisNo;  //dmis号
	IBYTE   nOldLen;  //老调机号长度
	TRAIN_NO  tnOldTrain; //老调机号
	IBYTE   nNewLen;  //新调机号长度
	TRAIN_NO  tnNewTrain; //新调机号
	IDWORD    nIPIndex; //IP索引
	IBYTE   nLocoType;  //调机号类型
	IBYTE   reserve[11];  //预留

	char*   GetLocoDesc();
	char*   SetLocoDesc(const char *psz, unsigned short nLen);
private:
	char    *pszLocoDesc;//调机号描述
	unsigned short    nLocoDescBufLen;//调机号描述长度
};
DECLARE_SERIALIZE_OPERATOR(MsgShuntLocoNoOper)


/**************************************************************************/
/*-------------------------------移动授权消息-----------------------------*/
/**************************************************************************/


/********************************权限申请**********************************/
class AFX_EXT_CLASS MsgRMRequest:public BaseMsg
{
	DECLARE_CLBTH(MsgRMRequest)
public:
	MsgRMRequest();
	virtual ~MsgRMRequest();
private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
public:
	ENTITY_ID nDestID;  //申请的目的实体号
	ENTITY_ID nSrcID;   //申请者的实体号
	char strSrcName[32];  //申请者终端设备描述
	char strUserName[32]; //申请者用户名
	char strPassword[32]; //申请者密码
};
DECLARE_SERIALIZE_OPERATOR(MsgRMRequest)

/******************************权限申请结果********************************/
class AFX_EXT_CLASS MsgRMResult:public BaseMsg
{
	DECLARE_CLBTH(MsgRMResult)
public:
	MsgRMResult();
	virtual ~MsgRMResult();
private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
public:
	ENTITY_ID nDestID;  //申请的目标实体
	ENTITY_ID nSrcID; //申请者的实体号
	IBYTE nResult;  //申请结果
	enum  //申请列表
	{
		RQ_OK,  //申请成功
		RQ_REJECT,  //管理者拒绝
		RQ_NO_OWNER,  //无管理者
		RQ_LOCKED,  //申请的实体已被另一终端申请
		RQ_INVALID, //用户名密码校验错误
		RQ_NO_ENTITY  //申请实体无效
	};
};
DECLARE_SERIALIZE_OPERATOR(MsgRMResult)

/********************************权限通知**********************************/
class AFX_EXT_CLASS MsgRMReport:public BaseMsg
{
	DECLARE_CLBTH(MsgRMReport)
public:
	MsgRMReport();
	virtual ~MsgRMReport();
private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
public:
	ENTITY_ID nDestID;  //权限的目标实体号
	ENTITY_ID nSrcID; //拥有权限者的实际实体号
	char strSrcName[32]; //权限拥有者终端设备描述
	char strUserName[32]; //权限拥有者用户名
	I_TIME tmRight; //权限分配或取消时间
	IBYTE nResult; //权限使用中/取消标志
	enum
	{
		RT_INIT, //权限初始化通告
		RT_REQUEST,  //权限申请中
		RT_USING,  //权限使用中
		RT_RELEASE //权限取消
	};
};
DECLARE_SERIALIZE_OPERATOR(MsgRMReport)

/******************************权限管理注册********************************/
class AFX_EXT_CLASS MsgRMRegister:public BaseMsg
{
	DECLARE_CLBTH(MsgRMRegister)
public:
	MsgRMRegister();
	virtual ~MsgRMRegister();
private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
public:
	ENTITY_ID nzDestIDs[256]; //被管理的目标实体号组
	int nDestCount;   //被管理的目标实体个数
	ENTITY_ID nControllerID; //管理终端实体号
};
DECLARE_SERIALIZE_OPERATOR(MsgRMRegister)

/********************************包装消息**********************************/
class AFX_EXT_CLASS MsgRMOperPack:public BaseMsg
{
	DECLARE_CLBTH(MsgRMOperPack)
public:
	MsgRMOperPack();
	virtual ~MsgRMOperPack();
private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
public:
	IBYTE nzMsgBuf[4096]; //消息字节流
	int nMsgLen;  //消息长度
	ENTITY_ID nDestID; //目标实体
};
DECLARE_SERIALIZE_OPERATOR(MsgRMOperPack)

/********************************实体更新**********************************/
class AFX_EXT_CLASS MsgDestEntityUpdate:public BaseMsg
{
	DECLARE_CLBTH(MsgDestEntityUpdate)
public:
	MsgDestEntityUpdate();
	virtual ~MsgDestEntityUpdate();
private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();
public:
	ENTITY_ID nzEntityAdd[256]; //新增管理实体列表
	int nEntityAddCount;   //新增管理实体个数
	ENTITY_ID nzEntityRemoved[256]; //删除管理实体列表
	int nEntityRemovedCount; //删除管理实体个数
};
DECLARE_SERIALIZE_OPERATOR(MsgDestEntityUpdate)

// 基本图名称设置
class AFX_EXT_CLASS MsgBasicScheduleSet : public BaseMsg
{
	DECLARE_CLBTH(MsgBasicScheduleSet)

public:
	MsgBasicScheduleSet();
	virtual ~MsgBasicScheduleSet();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	CONID nConId;//调度台区段号
	SCHEDULE_ID nScheduleId;//基本图id
	I_TIME        nStartTime, nEndTime;//开始时间,结束时间
	I_TIME  nSetNameTime, nEstimateStartTime;//基本图名称设置时间、估计开始时间
	ENTITY_ID nSetNameEntity;//基本图名称设置实体号
	char nScheduleName[MAX_SCHEDULE_NAME_LEN];//基本图名称
};
DECLARE_SERIALIZE_OPERATOR(MsgBasicScheduleSet)


// ----------------------- IP公布状态消息 -----------------------------------------------

// 列车位置移动
class AFX_EXT_CLASS IPMsgTrainMove : public BaseMsg
{
	DECLARE_CLBTH(IPMsgTrainMove)

public:
	IPMsgTrainMove();
	virtual ~IPMsgTrainMove();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	I_TIME        nTime;              // 发生时间
	TRAIN_NO  szPrevTrain;    // 原车次号
	IDWORD    nPrevIndex;       // 原列车IP索引
	STATION_NO  nPrevSt;      // 原列车车次窗位置
	DMIS_ID   nPrevWin;       //前一个车次窗号
	TRAIN_NO  szNextTrain;    // 现车次号
	IDWORD    nNextIndex;       // 现列车IP索引
	STATION_NO  nNextSt;      // 现列车车次窗位置
	DMIS_ID   nNextWin; //下一个车次窗号
	IBYTE   nOperLvl;   // 列车操作等级(0:TMP, 1:Radio, 2:STPC, 4:Dispatch)
	IBYTE   nTrainLvl;    // 列车运行等级(1:特快,2:普通客车,3:货车)
	IWORD   nFlag;      // 列车特殊标志(bit5: 电力机车标志)
};
DECLARE_SERIALIZE_OPERATOR(IPMsgTrainMove)

// 进路建立
class AFX_EXT_CLASS IPMsgRouteStatus : public BaseMsg
{
	DECLARE_CLBTH(IPMsgRouteStatus)

public:
	IPMsgRouteStatus();
	virtual ~IPMsgRouteStatus();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	I_TIME        nTime;                      // 发生时间
	STATION_NO  nStation;         // 站号
	TRAIN_NO  szTrain;            // 车次号
	IDWORD    nTrainIndex;        // 车次IP索引
	IBYTE   nRouteType;       // 进路类型（1:接车2:发车3:通过）
	ENTRY_ID  nEntry;         // 接车口
	ENTRY_ID  nExit;          // 发车口
	SIDE_ID   nzSides[MAX_SIDE_COUNT_IN_ROUTE]; // 途径股道
	IBYTE   nSideCount;       // 股道数量
	IDWORD    nRouteFlag;       // 进路特殊标志(bit0:列车电力标志, bit1:进路电力标志, bit2:进路分路不良标志)
	IBYTE   nMinSwitch;       // 进路途径最小辙叉号
	IBYTE   nMinDypassSwitch;   // 进路途径侧向最小辙叉号
	enum { ROUTE_SET, ROUTE_CANCEL01 };//进路建立,进路取消
	IBYTE   nRouteStatus;     // 进路建立取消标志
};
DECLARE_SERIALIZE_OPERATOR(IPMsgRouteStatus)

// 股道使用状态(占用/出清/完全进入股道)
class AFX_EXT_CLASS IPMsgSideStatus : public BaseMsg
{
	DECLARE_CLBTH(IPMsgSideStatus)

public:
	IPMsgSideStatus();
	virtual ~IPMsgSideStatus();

private:
	virtual HRCODE doSerialize(Archive& ar);

protected:
	virtual size_t countObjectLength();

public:
	I_TIME        nTime;                      // 发生时间
	STATION_NO  nStation;         // 站号
	TRAIN_NO  szTrain;            // 车次号
	IDWORD    nTrainIndex;        // 车次IP索引
	SIDE_ID   nSide;              // 股道号
	enum { SIDE_OCC, SIDE_CLEAR, SIDE_WHOLE_OCC };
	IBYTE   nSideStatus;  //股道状态
};
DECLARE_SERIALIZE_OPERATOR(IPMsgSideStatus)

// 列车出站
class AFX_EXT_CLASS IPMsgTrainStatus : public BaseMsg
{
	DECLARE_CLBTH(IPMsgTrainStatus)

public:
	IPMsgTrainStatus();
	virtual ~IPMsgTrainStatus();

private:
	virtual HRCODE doSerialize(Archive& ar);

protected:
	virtual size_t countObjectLength();

public:
	I_TIME        nTime;                      // 发生时间
	STATION_NO  nStation;         // 站号
	TRAIN_NO  szTrain;            // 车次号
	IDWORD    nTrainIndex;        // 车次IP索引
	enum { TRAIN_ENTER, TRAIN_EXIT };
	IBYTE nTrainStatus;   // 进出站标志
	ENTRY_ID  nEntry;             // 出站口
	SIDE_ID   nSide;          // 目标股道
};
DECLARE_SERIALIZE_OPERATOR(IPMsgTrainStatus)


// ----------------------- 综合维护消息 -----------------------------------------------

// 属性查询
class AFX_EXT_CLASS CmsMsgAttrRequest : public BaseMsg
{
	DECLARE_CLBTH(CmsMsgAttrRequest)

public:
	CmsMsgAttrRequest();
	virtual ~CmsMsgAttrRequest();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	IDWORD nDetectID; // 探测ID
	IDWORD nDetectDev;  // 探测发起者ID
	ENTITY_ID nDestID;  // 目标设备实体号
	IBYTE nABFlag;    // 目标设备AB机标志
	IBYTE nAttrType;    // 属性类别
	IDWORD nAttrGroupID;  // 属性数据库ID（对应于attributes表主键）
	short nAttrIndex; // 属性索引
	IWORD nReplyPort; // 反馈发送的UDP端口
	ENTITY_ID nPeerID;  // 通信对方实体（用于动态队列查询）
	IBYTE nPeerABFlag;  // 通信对方AB机标志（用于动态队列查询）
	char strAttrName[20]; // 属性名（用于自定义属性）
	char strPara[128];    // 查询参数（用于自定义属性）
};
DECLARE_SERIALIZE_OPERATOR(CmsMsgAttrRequest)

// 服务注册
class AFX_EXT_CLASS CmsMsgSvrRegister : public BaseMsg
{
	DECLARE_CLBTH(CmsMsgSvrRegister)

public:
	CmsMsgSvrRegister();
	virtual ~CmsMsgSvrRegister();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	ENTITY_ID nSvrID; // CMSS实体号
	ENTITY_ID nDestID;  // 目标设备实体号
	IBYTE nABFlag;    // 目标设备AB机标志
	IWORD nReplyPort; // 反馈发送的UDP端口
};
DECLARE_SERIALIZE_OPERATOR(CmsMsgSvrRegister)

// 单属性应答
class AFX_EXT_CLASS CmsMsgSingleAttrReply : public BaseMsg
{
	DECLARE_CLBTH(CmsMsgSingleAttrReply)

public:
	CmsMsgSingleAttrReply();
	virtual ~CmsMsgSingleAttrReply();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	IDWORD nDetectID; // 探测ID（复制查询消息中对应字段）
	IDWORD nDetectDev;  // 探测发起者ID（复制查询消息中对应字段）
	ENTITY_ID nSrcID; // 设备实体号
	IBYTE nABFlag;    // 设备AB机标志
	IBYTE nAttrType;    // 属性类别
	IDWORD nAttrGroupID;  // 属性组数据库ID（对应于attributes表主键）
	short nAttrIndex; // 属性索引
	IDWORD nAttrSingleID;// 单属性ID（对应于attr_conn/attr_capacity表主键）
	IDWORD nValue;    // 属性值（主备（0：主；1：备；2：未知/中断）/连接/容量/时间）
	IDWORD nMaxValue; // 最大属性值（仅用于容量属性）
	IBYTE nzUserValue[800]; // 用户自定义属性值
	short nUserValueLen;  // 用户自定义属性值长度
};
DECLARE_SERIALIZE_OPERATOR(CmsMsgSingleAttrReply)

// 组属性应答
class AFX_EXT_CLASS CmsMsgGroupAttrReply : public BaseMsg
{
	DECLARE_CLBTH(CmsMsgGroupAttrReply)

public:
	CmsMsgGroupAttrReply();
	virtual ~CmsMsgGroupAttrReply();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	IDWORD nDetectID; // 探测ID（复制查询消息中对应字段）
	IDWORD nDetectDev;  // 探测发起者ID（复制查询消息中对应字段）
	ENTITY_ID nSrcID; // 设备实体号
	IBYTE nABFlag;    // 设备AB机标志
	IBYTE nAttrType;    // 属性类别
	IDWORD nAttrGroupID;  // 属性数据库ID（对应于attributes表主键）
	IDWORD nValue[200]; // 属性值（连接/容量）
	short nValueStart;  // 属性值起始索引
	short nValueCount;  // 属性值数组成员数
};
DECLARE_SERIALIZE_OPERATOR(CmsMsgGroupAttrReply)

// 容量组属性应答
class AFX_EXT_CLASS CmsMsgCapaAttrReply : public BaseMsg
{
	DECLARE_CLBTH(CmsMsgCapaAttrReply)

public:
	CmsMsgCapaAttrReply();
	virtual ~CmsMsgCapaAttrReply();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	IDWORD nDetectID; // 探测ID（复制查询消息中对应字段）
	IDWORD nDetectDev;  // 探测发起者ID（复制查询消息中对应字段）
	ENTITY_ID nSrcID; // 设备实体号
	IBYTE nABFlag;    // 设备AB机标志
	IDWORD nAttrGroupID;  // 属性数据库ID（对应于attributes表主键）
	IDWORD nValue[100]; // 属性值
	IDWORD nMaxValue[100];  // 最大属性值
	short nValueStart;  // 属性值起始索引
	short nValueCount;  // 属性值数组成员数
};
DECLARE_SERIALIZE_OPERATOR(CmsMsgCapaAttrReply)

// 动态队列属性应答
class AFX_EXT_CLASS CmsMsgQueueAttrReply : public BaseMsg
{
	DECLARE_CLBTH(CmsMsgQueueAttrReply)

public:
	CmsMsgQueueAttrReply();
	virtual ~CmsMsgQueueAttrReply();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	IDWORD nDetectID; // 探测ID（复制查询消息中对应字段）
	IDWORD nDetectDev;  // 探测发起者ID（复制查询消息中对应字段）
	ENTITY_ID nSrcID; // 设备实体号
	IBYTE nABFlag;    // 设备AB机标志
	IDWORD nAttrGroupID;  // 属性数据库ID（对应于attributes表主键）
	IDWORD nAttrSingleID;// 单属性ID（对应于attr_capacity表主键）
	enum { MAX_QUEUE_COUNT = 60 };
	struct
	{
		ENTITY_ID nPeerID;  // 队列通信对方实体号
		IBYTE nPeerABFlag;  // 对方AB机标志
		short nIndex;   // 队列Indedx
		IDWORD nValue;    // 队列使用量
		IDWORD nMaxValue; // 队列容量最大值
	} szQueue[MAX_QUEUE_COUNT];
	IBYTE nQueueCount;    // 队列数组个数
};
DECLARE_SERIALIZE_OPERATOR(CmsMsgQueueAttrReply)

// 报警
class AFX_EXT_CLASS CmsMsgAlarm : public BaseMsg
{
	DECLARE_CLBTH(CmsMsgAlarm)

public:
	CmsMsgAlarm();
	virtual ~CmsMsgAlarm();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	ENTITY_ID nSrcID; // 设备实体号
	IBYTE nABFlag;    // 设备AB机标志
	I_TIME tmAlarm;   // 报警生成时间
	IBYTE nLevel;   // 报警级别
	IBYTE nType;      // 报警类别
	IBYTE nAttrType;    // 关联的属性类型
	IDWORD nAttrGroupID;  // 关联的属性数据库ID
	short nAttrIndex; // 关联是的属性索引
	char strAlarm[256]; // 报警内容
};
DECLARE_SERIALIZE_OPERATOR(CmsMsgAlarm)

// 客户端监控设备注册
class AFX_EXT_CLASS CmsMsgClientDevRegister : public BaseMsg
{
	DECLARE_CLBTH(CmsMsgClientDevRegister)

public:
	CmsMsgClientDevRegister();
	virtual ~CmsMsgClientDevRegister();

private:
	virtual HRCODE doSerialize(Archive& ar);
protected:
	virtual size_t countObjectLength();

public:
	ENTITY_ID nClientID;  // CMSC实体号
	IWORD nReplyPort;   // 发送属性状态的UDP端口号
	enum { MAX_DEV_COUNT = 200 };
	struct
	{
		ENTITY_ID nDevID; // 监控设备实体号
		IBYTE nABFlag;    // 监控设备AB机标志
	} szDevices[MAX_DEV_COUNT];
	IBYTE nDevCount;      // 注册设备个数
};
DECLARE_SERIALIZE_OPERATOR(CmsMsgClientDevRegister)


class AFX_EXT_CLASS MsgHILCTransmit:public BaseMsg
{
	DECLARE_CLBTH(MsgHILCTransmit)

public:
	MsgHILCTransmit();
	virtual ~MsgHILCTransmit();

protected:
	virtual HRCODE doSerialize(Archive& ar);
	virtual size_t countObjectLength();

public:
	char token[32];     //标识

	IDWORD  hilc_len;    //HILC内容长度
	char    hilc_content[256];//HILC内容
};
DECLARE_SERIALIZE_OPERATOR(MsgHILCTransmit)

class AFX_EXT_CLASS MsgChangeSideReq:public BaseMsg
{
	DECLARE_CLBTH(MsgChangeSideReq)

public:
	MsgChangeSideReq();
	virtual ~MsgChangeSideReq();

protected:
	virtual HRCODE doSerialize(Archive& ar);
	virtual size_t countObjectLength();

public:
	STATION_NO station;
	IWORD count;
	TRAIN_KEY train_index[256];
    IBYTE rec_index[256];
	SIDE  arrive_side[256];
	SIDE  depart_side[256];
};
DECLARE_SERIALIZE_OPERATOR(MsgChangeSideReq)

class AFX_EXT_CLASS MsgChangeSideAck:public BaseMsg
{
	DECLARE_CLBTH(MsgChangeSideAck)

public:
	MsgChangeSideAck();
	virtual ~MsgChangeSideAck();

protected:
	virtual HRCODE doSerialize(Archive& ar);
	virtual size_t countObjectLength();
public:
	IWORD result;
};
DECLARE_SERIALIZE_OPERATOR(MsgChangeSideAck)

CLBTH_NAMESPACE_END

#endif

