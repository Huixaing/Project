
#define EXTERN_LINKAGE_DEFINE
#include "msgid.h"
#include "msg_def.h"
#include <time.h>

CLBTH_NAMESPACE_BEGIN

// ------------------ DSS 消息-----------------------------------------------------------

// 站场ICON表示
IMPLEMENT_CLBTH(MsgDSSDisplay, MSGID_MsgDSSDisplay)
IMPLEMENT_SERIALIZE_OPERATOR(MsgDSSDisplay)
MsgDSSDisplay::MsgDSSDisplay()
:	nStationNo(INVALID_STATION_NO), nID(INVALID_DMIS_ID), nStrLen(0)
{
	strMoreInfo[0] = '\0';
}
MsgDSSDisplay::~MsgDSSDisplay()
{

}
HRCODE MsgDSSDisplay::doSerialize(Archive& ar)
{if(ar.isStoring())
	{
		if(nStrLen >= MAX_DMIS_STR_LEN)
			return HRCODE(HRCODE::HC_FAIL);

		ar << nStationNo << nType << nID << nStatus << nStrLen;
		if ( nStrLen > 0 )
			ar.write(strMoreInfo, nStrLen);
	}
	else
	{
		ar >> nStationNo >> nType >> nID >> nStatus >> nStrLen;
		if(nStrLen >= MAX_DMIS_STR_LEN)
			return HRCODE(HRCODE::HC_FAIL);

		if ( nStrLen > 0 )
		{
			ar.read(strMoreInfo, nStrLen);
			if ( nStrLen < MAX_DMIS_STR_LEN )
				strMoreInfo[nStrLen] = 0;
		}
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgDSSDisplay::countObjectLength()
{
	return sizeof(nStationNo) + sizeof(nType) + sizeof(nID) + sizeof(nStatus) + sizeof(nStrLen) + nStrLen;
}

// 车次号操作，包括添加、删除、修改
IMPLEMENT_CLBTH(MsgTrainNoOper, MSGID_MsgTrainNoOper)
IMPLEMENT_SERIALIZE_OPERATOR(MsgTrainNoOper)
MsgTrainNoOper::MsgTrainNoOper()
:	nStationNo(INVALID_STATION_NO), nWindowID(INVALID_DMIS_ID), nOldLen(0), nNewLen(0)
{
	tnOldTrain[0] = '\0';
	tnNewTrain[0] = '\0';
	nIPIndex = 0;
}

MsgTrainNoOper::~MsgTrainNoOper()
{

}
HRCODE MsgTrainNoOper::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << nStationNo << nWindowID << nOldLen;
		if ( nOldLen >= MAX_TRAIN_NO_LEN )
			return HRCODE(HRCODE::HC_FAIL);
		ar.write(tnOldTrain, nOldLen);
		ar << nNewLen;
		if ( nNewLen >= MAX_TRAIN_NO_LEN )
			return HRCODE(HRCODE::HC_FAIL);
		ar.write(tnNewTrain, nNewLen);
		ar << nIPIndex;
	}
	else
	{
		ar >> nStationNo >> nWindowID >> nOldLen;
		if ( nOldLen >= MAX_TRAIN_NO_LEN )
			return HRCODE(HRCODE::HC_FAIL);
		ar.read(tnOldTrain, nOldLen);
		ar >> nNewLen;
		if ( nNewLen >= MAX_TRAIN_NO_LEN )
			return HRCODE(HRCODE::HC_FAIL);
		ar.read(tnNewTrain, nNewLen);
		ar >> nIPIndex;
		tnOldTrain[nOldLen] = 0;
		tnNewTrain[nNewLen] = 0;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgTrainNoOper::countObjectLength()
{
	return sizeof(nStationNo) + sizeof(nWindowID) + sizeof(nOldLen) + sizeof(nNewLen) + nOldLen + nNewLen + sizeof(nIPIndex);
}

// 车次号确认
IMPLEMENT_CLBTH(MsgTrainConfirm, MSGID_MsgTrainConfirm)
IMPLEMENT_SERIALIZE_OPERATOR(MsgTrainConfirm)
MsgTrainConfirm::MsgTrainConfirm()
:	nStationNo(INVALID_STATION_NO), nWindowID(INVALID_DMIS_ID), nTrainNoLen(0)
{
	tnTrainNo[0] = '\0';
	nIPIndex = 0;
}	
MsgTrainConfirm::~MsgTrainConfirm()
{

}
HRCODE MsgTrainConfirm::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << nStationNo << nWindowID << nTrainNoLen;
		if ( nTrainNoLen >= MAX_TRAIN_NO_LEN )
			return HRCODE(HRCODE::HC_FAIL);
		ar.write(tnTrainNo, nTrainNoLen);
		ar << nIPIndex;
	}
	else
	{
		ar >> nStationNo >> nWindowID >> nTrainNoLen;
		if ( nTrainNoLen >= MAX_TRAIN_NO_LEN )
			return HRCODE(HRCODE::HC_FAIL);
		ar.read(tnTrainNo, nTrainNoLen);
		ar >> nIPIndex;
		tnTrainNo[nTrainNoLen] = 0;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgTrainConfirm::countObjectLength()
{
	return sizeof(nStationNo) + sizeof(nWindowID) + sizeof(nTrainNoLen) + nTrainNoLen + sizeof(nIPIndex);
}

// 车次号属性修改
IMPLEMENT_CLBTH(MsgSetTrainAttrib, MSGID_MsgSetTrainAttrib)
IMPLEMENT_SERIALIZE_OPERATOR(MsgSetTrainAttrib)
MsgSetTrainAttrib::MsgSetTrainAttrib()
:	nStationNo(INVALID_STATION_NO), nWindowID(INVALID_DMIS_ID), nTrainNoLen(0)
{
	tnTrainNo[0] = '\0';
	nIPIndex = 0;
	nTrainType = -1;
	nTrainRate = -1;
	nLocoNoLen = -1;
	szLocoNo[0] = 0;
}	
MsgSetTrainAttrib::~MsgSetTrainAttrib()
{
	
}
HRCODE MsgSetTrainAttrib::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << nStationNo << nWindowID << nTrainNoLen;
		if ( nTrainNoLen >= MAX_TRAIN_NO_LEN )
			return HRCODE(HRCODE::HC_FAIL);
		ar.write(tnTrainNo, nTrainNoLen);
		ar << nIPIndex << nTrainType << nTrainRate << nLocoNoLen;
		if ( nLocoNoLen >= sizeof(szLocoNo) )
			return HRCODE(HRCODE::HC_FAIL);
		if ( nLocoNoLen > 0 )
			ar.write(szLocoNo, nLocoNoLen);
	}
	else
	{
		ar >> nStationNo >> nWindowID >> nTrainNoLen;
		if ( nTrainNoLen >= MAX_TRAIN_NO_LEN )
			return HRCODE(HRCODE::HC_FAIL);
		ar.read(tnTrainNo, nTrainNoLen);
		tnTrainNo[nTrainNoLen] = 0;
		ar >> nIPIndex >> nTrainType >> nTrainRate >> nLocoNoLen;
		if ( nLocoNoLen >= sizeof(szLocoNo) )
			return HRCODE(HRCODE::HC_FAIL);
		if ( nLocoNoLen > 0 )
		{
			ar.read(szLocoNo, nLocoNoLen);
			szLocoNo[nLocoNoLen] = 0;
		}
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgSetTrainAttrib::countObjectLength()
{
	return sizeof(nStationNo) + sizeof(nWindowID) + sizeof(nTrainNoLen) + nTrainNoLen + sizeof(nIPIndex)
		+ sizeof(nTrainType) + sizeof(nTrainRate) + sizeof(nLocoNoLen) + (nLocoNoLen>=0 ? nLocoNoLen : 0);
}

// 车次停稳设置
IMPLEMENT_CLBTH(MsgSetTrainStop, MSGID_MsgSetTrainStop)
IMPLEMENT_SERIALIZE_OPERATOR(MsgSetTrainStop)
MsgSetTrainStop::MsgSetTrainStop()
:	nStationNo(INVALID_STATION_NO), nWindowID(INVALID_DMIS_ID)
{
	tnTrainNo[0] = '\0';
	nIPIndex = 0;
}	
MsgSetTrainStop::~MsgSetTrainStop()
{
}
HRCODE MsgSetTrainStop::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << nStationNo << nWindowID << nIPIndex << nStopInfo;
		tnTrainNo[MAX_TRAIN_NO_LEN-1] = '\0';
		short len = strlen(tnTrainNo);
		if ( len >= MAX_TRAIN_NO_LEN )
			return HRCODE(HRCODE::HC_FAIL);
		ar << len;
		ar.write(tnTrainNo, len);
	}
	else
	{
		short len = 0;
		ar >> nStationNo >> nWindowID >> nIPIndex >> nStopInfo >> len;
		if ( len >= MAX_TRAIN_NO_LEN )
			return HRCODE(HRCODE::HC_FAIL);
		ar.read(tnTrainNo, len);
		tnTrainNo[len] = '\0';
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgSetTrainStop::countObjectLength()
{
	return sizeof(nStationNo) + sizeof(nWindowID) + sizeof(nIPIndex) + sizeof(nStopInfo) + sizeof(short) + strlen(tnTrainNo);
}


// 站死
IMPLEMENT_CLBTH(MsgStationDown, MSGID_MsgStationDown)
IMPLEMENT_SERIALIZE_OPERATOR(MsgStationDown)
MsgStationDown::MsgStationDown()
:	nStationNo(INVALID_STATION_NO)
{

}
MsgStationDown::~MsgStationDown()
{

}
HRCODE MsgStationDown::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << nStationNo;
	}
	else
	{
		ar >> nStationNo;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgStationDown::countObjectLength()
{
	return sizeof(nStationNo);
}

// 站活
IMPLEMENT_CLBTH(MsgStationUp, MSGID_MsgStationUp)
IMPLEMENT_SERIALIZE_OPERATOR(MsgStationUp)
MsgStationUp::MsgStationUp()
:	nStationNo(INVALID_STATION_NO)
{

}
MsgStationUp::~MsgStationUp()
{

}
HRCODE MsgStationUp::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << nStationNo;
	}
	else
	{
		ar >> nStationNo;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgStationUp::countObjectLength()
{
	return sizeof(nStationNo);
}

//
IMPLEMENT_CLBTH(MsgDssInitComplete, MSGID_MsgDssInitComplete)
IMPLEMENT_SERIALIZE_OPERATOR(MsgDssInitComplete)
MsgDssInitComplete::MsgDssInitComplete()
:	nStationNo(INVALID_STATION_NO)
{

}
MsgDssInitComplete::~MsgDssInitComplete()
{

}
HRCODE MsgDssInitComplete::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << nStationNo;
	}
	else
	{
		ar >> nStationNo;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgDssInitComplete::countObjectLength()
{
	return sizeof(MsgDssInitComplete);
}

// DSS车站设备人工操作(故障区段设置等)
IMPLEMENT_CLBTH(MsgDSSManualDeviceOper, MSGID_MsgDSSManualDeviceOper)
IMPLEMENT_SERIALIZE_OPERATOR(MsgDSSManualDeviceOper)
MsgDSSManualDeviceOper::MsgDSSManualDeviceOper()
:	nStationNo(INVALID_STATION_NO)
{
}
MsgDSSManualDeviceOper::~MsgDSSManualDeviceOper()
{
}
HRCODE MsgDSSManualDeviceOper::doSerialize(Archive& ar)
{
	if ( ar.isStoring() )
	{
		ar << nStationNo << nOperType << nDeviceType << nDeviceID << tmOperTime << nOperPara;
		ar.write( strDeviceName, sizeof(strDeviceName) );
	}
	else
	{
		ar >> nStationNo >> nOperType >> nDeviceType >> nDeviceID >> tmOperTime >> nOperPara;
		ar.read( strDeviceName, sizeof(strDeviceName) );
		strDeviceName[sizeof(strDeviceName)-1] = 0;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgDSSManualDeviceOper::countObjectLength()
{
	return sizeof(MsgDSSManualDeviceOper);
}


// ------------------------------ 按钮操作相关消息 -----------------------------------------------

// 按钮按下
IMPLEMENT_CLBTH(MsgManualButtonPress, MSGID_MsgManualButtonPress)
IMPLEMENT_SERIALIZE_OPERATOR(MsgManualButtonPress)
MsgManualButtonPress::MsgManualButtonPress()
:	nStationNo(INVALID_STATION_NO), nDmisID(INVALID_DMIS_ID), nOperPara(0)
{
	strDeviceName[0] = 0;
}
MsgManualButtonPress::~MsgManualButtonPress()
{

}
HRCODE MsgManualButtonPress::doSerialize(Archive& ar)
{
	if ( ar.isStoring() )
	{
		ar << nOperID << nStationNo << nDeviceType << nDmisID;
		ar.write( strDeviceName, 20 );
		ar << nOperPara;
	}
	else
	{
		ar >> nOperID >> nStationNo >> nDeviceType >> nDmisID;
		ar.read( strDeviceName, 20 );
		strDeviceName[19] = 0;
		ar >> nOperPara;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgManualButtonPress::countObjectLength()
{
	return sizeof(nOperID) + sizeof(nStationNo) + sizeof(nDeviceType) + sizeof(nDmisID) + sizeof(nOperPara) + 20;
}

// 按钮清除
IMPLEMENT_CLBTH(MsgManualButtonCancel, MSGID_MsgManualButtonCancel)
IMPLEMENT_SERIALIZE_OPERATOR(MsgManualButtonCancel)
MsgManualButtonCancel::MsgManualButtonCancel()
:	nStationNo(INVALID_STATION_NO), nDmisID(INVALID_DMIS_ID), nOperPara(0)
{
	strDeviceName[0] = 0;
}
MsgManualButtonCancel::~MsgManualButtonCancel()
{

}
HRCODE MsgManualButtonCancel::doSerialize(Archive& ar)
{
	if ( ar.isStoring() )
	{
		ar << nOperID << nStationNo << nDeviceType << nDmisID;
		ar.write( strDeviceName, 20 );
		ar << nOperPara;
	}
	else
	{
		ar >> nOperID >> nStationNo >> nDeviceType >> nDmisID;
		ar.read( strDeviceName, 20 );
		strDeviceName[19] = 0;
		ar >> nOperPara;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgManualButtonCancel::countObjectLength()
{
	return sizeof(nOperID) + sizeof(nStationNo) + sizeof(nDeviceType) + sizeof(nDmisID) + sizeof(nOperPara) + 20;
}

// 进路建立
IMPLEMENT_CLBTH(MsgManualRouteSet, MSGID_MsgManualRouteSet)
IMPLEMENT_SERIALIZE_OPERATOR(MsgManualRouteSet)
MsgManualRouteSet::MsgManualRouteSet()
:	nStationNo(INVALID_STATION_NO), nButtonCount(0), nRouteID(NULL_ROUTE_ID), tmStartTime(0), tmExistTime(0), nRouteType(INVALID)
{
	for ( int i = 0; i < MAX_ROUTE_BUTTON_COUNT; i++ )
		strButtonName[i][0] = '\0';
	bForceFlag = 0;
	tnTrainNo[0] = 0;
}
MsgManualRouteSet::~MsgManualRouteSet()
{

}
HRCODE MsgManualRouteSet::doSerialize(Archive& ar)
{
	if ( ar.isStoring() )
	{
		ar << nOperID << nStationNo << nButtonCount;
		if ( nButtonCount > MAX_ROUTE_BUTTON_COUNT )
			return HRCODE(HRCODE::HC_FAIL);
		for ( int i = 0; i < nButtonCount; i++ )
		{
			ar << nButtonID[i];
			ar.write( strButtonName[i], sizeof(BUTTON_NAME) );
		}
		ar << (IBYTE)nRouteType << nRouteID << tmStartTime << tmExistTime << bForceFlag;
		ar.write( tnTrainNo, MAX_TRAIN_NO_LEN );
	}
	else
	{
		ar >> nOperID >> nStationNo >> nButtonCount;
		if ( nButtonCount > MAX_ROUTE_BUTTON_COUNT )
			return HRCODE(HRCODE::HC_FAIL);
		for ( int i = 0; i < nButtonCount; i++ )
		{
			ar >> nButtonID[i];
			ar.read( strButtonName[i], sizeof(BUTTON_NAME) );
			strButtonName[i][sizeof(BUTTON_NAME)-1] = 0;
		}
		IBYTE nTemp;
		ar >> nTemp >> nRouteID >> tmStartTime >> tmExistTime >> bForceFlag;
		nRouteType = (ROUTE_TYPE)nTemp;
		ar.read( tnTrainNo, MAX_TRAIN_NO_LEN );
		tnTrainNo[MAX_TRAIN_NO_LEN-1] = '\0';
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgManualRouteSet::countObjectLength()
{
	return sizeof(nOperID) + sizeof(nStationNo) + sizeof(nButtonCount) + sizeof(nRouteID)
		+ sizeof(tmStartTime) + sizeof(tmExistTime) + sizeof(bForceFlag)
		+ MAX_ROUTE_BUTTON_COUNT*(sizeof(BUTTON_ID)+sizeof(BUTTON_NAME)) + sizeof(IBYTE) + MAX_TRAIN_NO_LEN;
}

#ifdef _MSC_VER
string MsgManualRouteSet::toString()
{
	string prompt;
	char sz[512];

	sprintf(sz, "%d号令: 建立进路[%d]", nOperID, nRouteID);
	prompt = sz;

	for(int i=0; i<nButtonCount; i++)
	{
		sprintf(sz, " %s[%d]", strButtonName[i], nButtonID[i]);
		prompt = prompt + sz;
	}
	if(bForceFlag)
		prompt = prompt + " 强制执行";
	return prompt;
}
#endif

// 进路取消
/*
IMPLEMENT_CLBTH(MsgManualRouteCancel, MSGID_MsgManualRouteCancel)
IMPLEMENT_SERIALIZE_OPERATOR(MsgManualRouteCancel)
MsgManualRouteCancel::MsgManualRouteCancel()
:	nStationNo(INVALID_STATION_NO), nButtonID(INVALID_DMIS_ID)
{
	strButtonName[0] = '\0';
	bForceFlag = 0;
}
MsgManualRouteCancel::~MsgManualRouteCancel()
{

}
HRCODE MsgManualRouteCancel::doSerialize(Archive& ar)
{
	if ( ar.isStoring() )
	{
		ar << nOperID << nStationNo << nButtonID;
		ar.write( strButtonName, sizeof(BUTTON_NAME) );
		ar << (IBYTE)nRouteType << nRouteID << bForceFlag;
	}
	else
	{
		ar >> nOperID >> nStationNo >> nButtonID;
		ar.read( strButtonName, sizeof(BUTTON_NAME) );
		strButtonName[sizeof(BUTTON_NAME)-1] = 0;
		IBYTE nTemp;
		ar >> nTemp >> nRouteID >> bForceFlag;
		nRouteType = (ROUTE_TYPE)nTemp;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgManualRouteCancel::countObjectLength()
{
	return sizeof(nOperID) + sizeof(nStationNo) + sizeof(nButtonID) + sizeof(nRouteID)
		+ sizeof(bForceFlag) + sizeof(BUTTON_NAME) + sizeof(IBYTE);
}

// 引导进路办理
IMPLEMENT_CLBTH(MsgManualYinDaoRoute, MSGID_MsgManualYinDaoRoute)
IMPLEMENT_SERIALIZE_OPERATOR(MsgManualYinDaoRoute)
MsgManualYinDaoRoute::MsgManualYinDaoRoute()
:	nStationNo(INVALID_STATION_NO), nRouteID(NULL_ROUTE_ID), nYinDaoButton(INVALID_DMIS_ID)
{

}
MsgManualYinDaoRoute::~MsgManualYinDaoRoute()
{

}
HRCODE MsgManualYinDaoRoute::doSerialize(Archive& ar)
{
	if ( ar.isStoring() )
	{
		ar << nOperID << nStationNo << nRouteID << nYinDaoButton;
		ar.write( strButtonName, sizeof(BUTTON_NAME) );
		ar << bForceFlag;
	}
	else
	{
		ar >> nOperID >> nStationNo >> nRouteID >> nYinDaoButton;
		ar.read( strButtonName, sizeof(BUTTON_NAME) );
		strButtonName[sizeof(BUTTON_NAME)-1] = 0;
		ar >> bForceFlag;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgManualYinDaoRoute::countObjectLength()
{
	return sizeof(nOperID) + sizeof(nStationNo) + sizeof(nRouteID) + sizeof(nYinDaoButton) + sizeof(BUTTON_NAME) + sizeof(bForceFlag);
}
*/
// 设备人工操作
IMPLEMENT_CLBTH(MsgManualDeviceOper, MSGID_MsgManualDeviceOper)
IMPLEMENT_SERIALIZE_OPERATOR(MsgManualDeviceOper)
MsgManualDeviceOper::MsgManualDeviceOper()
:	nOperID(0), nStationNo(INVALID_STATION_NO), nDeviceID(INVALID_DMIS_ID), nOperPara(0), bForceFlag(0)
{
	strDeviceName[0] = 0;
}
MsgManualDeviceOper::~MsgManualDeviceOper()
{

}

HRCODE MsgManualDeviceOper::doSerialize(Archive& ar)
{
	if ( ar.isStoring() )
	{
		ar << nOperID << nStationNo << nOperType << nDeviceType << nDeviceID << tmOperTime;
		ar.write( strDeviceName, sizeof(strDeviceName) );
		ar << nOperPara << bForceFlag;
	}
	else
	{
		ar >> nOperID >> nStationNo >> nOperType >> nDeviceType >> nDeviceID >> tmOperTime;
		ar.read( strDeviceName, sizeof(strDeviceName) );
		strDeviceName[sizeof(strDeviceName)-1] = 0;
		ar >> nOperPara >> bForceFlag;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgManualDeviceOper::countObjectLength()
{
	return sizeof(nOperID) + sizeof(nStationNo) + sizeof(nDeviceID) + sizeof(nDeviceType) + sizeof(nOperType) + sizeof(nOperPara) + sizeof(bForceFlag) + sizeof(strDeviceName) + sizeof(tmOperTime);
}

#ifdef _MSC_VER
string MsgManualDeviceOper::toString()
{
	static struct OPER_DESC {
		MANUAL_OPER_TYPE oper_t;
		char *szCommand;
	} desc[] =
	{
		//{ROUTE_OPER_SET,		"人工排列进路"},
		{ROUTE_OPER_CANCEL,		"取消进路"},
		{ROUTE_OPER_YINDAO,		"引导进路"},
		//{ROUTE_OPER_TRY_LOCK,	"人工试排进路"},
		{ROUTE_OPER_SHACKLE,	"进路单锁"},

		{SWITCH_OPER_LOCK,		"道岔单锁"},
		{SWITCH_OPER_UNLOCK,	"道岔单解"},
		{SWITCH_OPER_NORMAL,	"道岔定操"},
		{SWITCH_OPER_REVERSE,	"道岔反操"},
		{SWITCH_OPER_CEN_FAULT,	"道岔心轨故障"},
		{SWITCH_OPER_TIP_FAULT,	"道岔尖轨故障"},

		{SIGNAL_OPER_REOPEN,		"信号重开"},
		{MANUAL_OPER_CALLON_LOCK,	"引导总锁"},
		{MANUAL_OPER_UNLOCK,		"总人解"},
		{MANUAL_OPER_BLOCK,			"封锁"},
		{MANUAL_OPER_UNLOCK_STA,	"全站解锁"},
		{MANUAL_OPER_SLOPE_UNLOCK,	"坡道解锁"},
		{MANUAL_OPER_CIRCUIT_BAD,	"分路不良"},
		{SIGNAL_OPER_AUTOPASS,		"自通设置"},
		{MANUAL_OPER_CIRCUIT_FAULT, "故障区段"},
		{MANUAL_OPER_NO_ELEC,		"人工停电"},
		{BUTTON_OPER_FUNC,			"功能按钮"},
		{MANUAL_OPER_LOCO_OCCUPY,	"机占"},
		{SIGNAL_OPER_OPENSIG,		"信号机开灯"},
		{SIGNAL_OPER_CLOSESIG,		"信号机关灯"},
	};

	string prompt = "未知命令";
	for(int i=0; i<sizeof(desc)/sizeof(desc[0]); i++)
	{
		if(nOperType == desc[i].oper_t)
		{
			prompt = desc[i].szCommand;
			break;
		}
	}
	char sz[512];
	sprintf(sz, "%d号令: %s %s[Sta-%d, Type-%d, ID-%d]", 
		nOperID, prompt.c_str(), strDeviceName, nStationNo, nDeviceType, nDeviceID);
	prompt = sz;
	if(bForceFlag)
		prompt = prompt + " 强制执行";
	return prompt;
}
#endif

void MsgManualDeviceOper::SetDeviceName(const char* pstrName)
{
	int maxlen = sizeof(strDeviceName)/sizeof(strDeviceName[0]);
	if(NULL == pstrName)
		memset(strDeviceName, 0, maxlen);
	else if(strlen(pstrName) < maxlen)
		strcpy(strDeviceName, pstrName);
	else
		strncpy(strDeviceName, pstrName, maxlen);
}

// 设备关键人工操作(包括进路取消，总人解等)
IMPLEMENT_CLBTH(MsgManualDeviceOperEx, MSGID_MsgManualDeviceOperEx)
IMPLEMENT_SERIALIZE_OPERATOR(MsgManualDeviceOperEx)
MsgManualDeviceOperEx::MsgManualDeviceOperEx()
{
}
MsgManualDeviceOperEx::~MsgManualDeviceOperEx()
{
}
HRCODE MsgManualDeviceOperEx::doSerialize(Archive& ar)
{
	HRCODE rs = MsgManualDeviceOper::doSerialize(ar);
	if ( rs != HRCODE(HRCODE::HC_SUCCESS) )
		return rs;

	if ( ar.isStoring() )
	{
		ar << nRandomPara;
	}
	else
	{
		ar >> nRandomPara;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgManualDeviceOperEx::countObjectLength()
{
	return MsgManualDeviceOper::countObjectLength() + sizeof(nRandomPara);
}
void	MsgManualDeviceOperEx::MakeBaseDeviceOperMsg( MsgManualDeviceOper& rBaseMsg )
{
	rBaseMsg.nOperID		= nOperID;
	rBaseMsg.nStationNo		= nStationNo;
	rBaseMsg.nOperType		= nOperType;
	rBaseMsg.nDeviceType	= nDeviceType;
	rBaseMsg.nDeviceID		= nDeviceID;
	rBaseMsg.tmOperTime		= tmOperTime;
	rBaseMsg.nOperPara		= nOperPara;
	rBaseMsg.bForceFlag		= bForceFlag;
	memcpy( rBaseMsg.strDeviceName, strDeviceName, sizeof(strDeviceName) );
}

// 进路试排
IMPLEMENT_CLBTH(MsgManualRouteTryLock, MSGID_MsgManualRouteTryLock)
IMPLEMENT_SERIALIZE_OPERATOR(MsgManualRouteTryLock)
MsgManualRouteTryLock::MsgManualRouteTryLock()
:	nStationNo(INVALID_STATION_NO), nButtonCount(0), nRouteID(NULL_ROUTE_ID), tmStartTime(0), tmExistTime(0), nRouteType(INVALID)
{
	for ( int i = 0; i < MAX_ROUTE_BUTTON_COUNT; i++ )
		strButtonName[i][0] = '\0';
	bForceFlag = 0;
}
MsgManualRouteTryLock::~MsgManualRouteTryLock()
{

}
HRCODE MsgManualRouteTryLock::doSerialize(Archive& ar)
{
	if ( ar.isStoring() )
	{
		ar << nOperID << nStationNo << nButtonCount;
		if ( nButtonCount > MAX_ROUTE_BUTTON_COUNT )
			return HRCODE(HRCODE::HC_FAIL);
		for ( int i = 0; i < nButtonCount; i++ )
		{
			ar << nButtonID[i];
			ar.write( strButtonName[i], sizeof(BUTTON_NAME) );
		}
		ar << (IBYTE)nRouteType << nRouteID << tmStartTime << tmExistTime << bForceFlag;
	}
	else
	{
		ar >> nOperID >> nStationNo >> nButtonCount;
		if ( nButtonCount > MAX_ROUTE_BUTTON_COUNT )
			return HRCODE(HRCODE::HC_FAIL);
		for ( int i = 0; i < nButtonCount; i++ )
		{
			ar >> nButtonID[i];
			ar.read( strButtonName[i], sizeof(BUTTON_NAME) );
			strButtonName[i][sizeof(BUTTON_NAME)-1] = 0;
		}
		IBYTE nTemp;
		ar >> nTemp >> nRouteID >> tmStartTime >> tmExistTime >> bForceFlag;
		nRouteType = (ROUTE_TYPE)nTemp;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgManualRouteTryLock::countObjectLength()
{
	return sizeof(nOperID) + sizeof(nStationNo) + sizeof(nButtonCount) + sizeof(nRouteID)
		+ sizeof(tmStartTime) + sizeof(tmExistTime) + sizeof(bForceFlag)
		+ MAX_ROUTE_BUTTON_COUNT*(sizeof(BUTTON_ID)+sizeof(BUTTON_NAME)) + sizeof(IBYTE);
}

#ifdef _MSC_VER
string MsgManualRouteTryLock::toString()
{
	string prompt;
	char sz[512];

	sprintf(sz, "%d号令: 试排进路[%d]", nOperID, nRouteID);
	prompt = sz;

	for(int i=0; i<nButtonCount; i++)
	{
		sprintf(sz, " %s[%d]", strButtonName[i], nButtonID[i]);
		prompt = prompt + sz;
	}
	if(bForceFlag)
		prompt = prompt + " 强制执行";
	return prompt;
}
#endif

// 操作使能
IMPLEMENT_CLBTH(MsgManualDeviceOperEnable, MSGID_MsgManualDeviceOperEnable)
IMPLEMENT_SERIALIZE_OPERATOR(MsgManualDeviceOperEnable)
MsgManualDeviceOperEnable::MsgManualDeviceOperEnable()
	: nStationNo(INVALID_STATION_NO), tmOperTime(0)
{
}
MsgManualDeviceOperEnable::~MsgManualDeviceOperEnable()
{
}
HRCODE MsgManualDeviceOperEnable::doSerialize(Archive& ar)
{
	if ( ar.isStoring() )
	{
		ar << nStationNo << nOperType << tmOperTime << nRandomPara;
	}
	else
	{
		ar >> nStationNo >> nOperType >> tmOperTime >> nRandomPara;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgManualDeviceOperEnable::countObjectLength()
{
	return sizeof(nStationNo) + sizeof(tmOperTime) + sizeof(nRandomPara) + sizeof(nOperType);
}

// 道岔单操
/*
IMPLEMENT_CLBTH(MsgManualSwitchOper, MSGID_MsgManualSwitchOper)
IMPLEMENT_SERIALIZE_OPERATOR(MsgManualSwitchOper)
MsgManualSwitchOper::MsgManualSwitchOper()
:	nStationNo(INVALID_STATION_NO), nSwitchID(INVALID_DMIS_ID)
{

}
MsgManualSwitchOper::~MsgManualSwitchOper()
{

}
HRCODE MsgManualSwitchOper::doSerialize(Archive& ar)
{
	if ( ar.isStoring() )
	{
		ar << nOperID << nStationNo << nSwitchID << nOperType << bForceFlag;
	}
	else
	{
		ar >> nOperID >> nStationNo >> nSwitchID >> nOperType >> bForceFlag;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgManualSwitchOper::countObjectLength()
{
	return sizeof(nOperID) + sizeof(nStationNo) + sizeof(nSwitchID) + sizeof(nOperType) + sizeof(bForceFlag);
}

// 区段操作
IMPLEMENT_CLBTH(MsgManualTrackOper, MSGID_MsgManualTrackOper)
IMPLEMENT_SERIALIZE_OPERATOR(MsgManualTrackOper)
MsgManualTrackOper::MsgManualTrackOper()
:	nStationNo(INVALID_STATION_NO), nTrackID(INVALID_DMIS_ID)
{

}
MsgManualTrackOper::~MsgManualTrackOper()
{

}
HRCODE MsgManualTrackOper::doSerialize(Archive& ar)
{
	if ( ar.isStoring() )
	{
		ar << nOperID << nStationNo << nTrackID << nOperType << bForceFlag;
	}
	else
	{
		ar >> nOperID >> nStationNo >> nTrackID >> nOperType >> bForceFlag;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgManualTrackOper::countObjectLength()
{
	return sizeof(nOperID) + sizeof(nStationNo) + sizeof(nTrackID) + sizeof(nOperType) + sizeof(bForceFlag);
}

// 信号机操作
IMPLEMENT_CLBTH(MsgManualSignalOper, MSGID_MsgManualSignalOper)
IMPLEMENT_SERIALIZE_OPERATOR(MsgManualSignalOper)
MsgManualSignalOper::MsgManualSignalOper()
:	nStationNo(INVALID_STATION_NO), nSignalID(INVALID_DMIS_ID)
{

}
MsgManualSignalOper::~MsgManualSignalOper()
{

}
HRCODE MsgManualSignalOper::doSerialize(Archive& ar)
{
	if ( ar.isStoring() )
	{
		ar << nOperID << nStationNo << nSignalID << nOperType << bForceFlag;
	}
	else
	{
		ar >> nOperID >> nStationNo >> nSignalID >> nOperType >> bForceFlag;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgManualSignalOper::countObjectLength()
{
	return sizeof(nOperID) + sizeof(nStationNo) + sizeof(nSignalID) + sizeof(nOperType) + sizeof(bForceFlag);
}
*/

// 人工操作的回应
IMPLEMENT_CLBTH(MsgManualOperResult, MSGID_MsgManualOperResult)
IMPLEMENT_SERIALIZE_OPERATOR(MsgManualOperResult)
MsgManualOperResult::MsgManualOperResult()
:	nCommentLen(0)
{
	strComment[0] = '\0';
}
MsgManualOperResult::~MsgManualOperResult()
{

}
HRCODE MsgManualOperResult::doSerialize(Archive& ar)
{
	if ( ar.isStoring() )
	{
		ar << nOperID << nResult << nCommentLen;
		if ( nCommentLen > MAX_RESULT_COMMENT_LEN )
			return HRCODE(HRCODE::HC_FAIL);
		if ( nCommentLen > 0 )
			ar.write( strComment, nCommentLen );
	}
	else
	{
		ar >> nOperID >> nResult >> nCommentLen;
		if ( nCommentLen > MAX_RESULT_COMMENT_LEN )
			return HRCODE(HRCODE::HC_FAIL);
		if ( nCommentLen > 0 )
		{
			ar.read( strComment, nCommentLen );
			if ( nCommentLen < MAX_RESULT_COMMENT_LEN )
				strComment[nCommentLen] = 0;
		}
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgManualOperResult::countObjectLength()
{
	return sizeof(nOperID) + sizeof(nResult) + sizeof(nCommentLen) + nCommentLen;
}
bool	MsgManualOperResult::CanForceOper() const
{
	return ( nResult & 0x80 ) != 0;
}
void	MsgManualOperResult::SetForceOperFlag( bool b )
{
	if ( b )
		nResult |= 0x80;
	else
		nResult &= 0x7F;
}
MANUAL_OPER_RESULT	MsgManualOperResult::GetResult()
{
	return nResult & 0x7F;
}


// ------------------------------ 车站与进路控制模式转换 ----------------------------------------------

// 车站控制模式转换申请(包括GPC/STPC向LiRC申请和LiRC向GPC/STPC(所有者)申请确认)
IMPLEMENT_CLBTH(MsgStationModeRequest, MSGID_MsgStationModeRequest)
IMPLEMENT_SERIALIZE_OPERATOR(MsgStationModeRequest)
MsgStationModeRequest::MsgStationModeRequest()
:	nStationNo(INVALID_STATION_NO), nOriginalID(INVALID_ENTITY_ID)
{

}
MsgStationModeRequest::~MsgStationModeRequest()
{

}
HRCODE MsgStationModeRequest::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << nOriginalID << nStationNo << nZoneID << nMode;
	}
	else
	{
		ar >> nOriginalID >> nStationNo >> nZoneID >> nMode;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgStationModeRequest::countObjectLength()
{
	return sizeof(nOriginalID) + sizeof(nStationNo) + sizeof(nZoneID) + sizeof(nMode);
}

// 车站控制模式申请结果(包括LiRC发给GPC/STPC的申请结果和GPC/STPC发给LiRC的批准结果)
IMPLEMENT_CLBTH(MsgStationModeResult, MSGID_MsgStationModeResult)
IMPLEMENT_SERIALIZE_OPERATOR(MsgStationModeResult)
MsgStationModeResult::MsgStationModeResult()
:	nStationNo(INVALID_STATION_NO), nOriginalID(INVALID_ENTITY_ID), nCommentLen(0)
{
	strComment[0] = '\0';
}
MsgStationModeResult::~MsgStationModeResult()
{

}
HRCODE MsgStationModeResult::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << nOriginalID << nStationNo << nZoneID << nMode << nResult << nCommentLen;
		if ( nCommentLen > MAX_RESULT_COMMENT_LEN )
			return HRCODE(HRCODE::HC_FAIL);
		if ( nCommentLen > 0 )
			ar.write( strComment, nCommentLen );
	}
	else
	{
		ar >> nOriginalID >> nStationNo >> nZoneID >> nMode >> nResult >> nCommentLen;
		if ( nCommentLen > MAX_RESULT_COMMENT_LEN )
			return HRCODE(HRCODE::HC_FAIL);
		if ( nCommentLen > 0 )
		{
			ar.read( strComment, nCommentLen );
			if ( nCommentLen < MAX_RESULT_COMMENT_LEN )
				strComment[nCommentLen] = 0;
		}
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgStationModeResult::countObjectLength()
{
	return sizeof(nOriginalID) + sizeof(nStationNo) + sizeof(nZoneID) + sizeof(nMode) + sizeof(nResult)
		+ sizeof(nCommentLen) + MAX_RESULT_COMMENT_LEN;
}


// 进路控制模式申请(GPC/STPC->LiRC)
IMPLEMENT_CLBTH(MsgRouteModeRequest, MSGID_MsgRouteModeRequest)
IMPLEMENT_SERIALIZE_OPERATOR(MsgRouteModeRequest)
MsgRouteModeRequest::MsgRouteModeRequest()
:	nStationNo(INVALID_STATION_NO)
{

}
MsgRouteModeRequest::~MsgRouteModeRequest()
{

}
HRCODE MsgRouteModeRequest::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << nStationNo << nMode;
	}
	else
	{
		ar >> nStationNo >> nMode;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgRouteModeRequest::countObjectLength()
{
	return sizeof(nStationNo) + sizeof(nMode);
}


// 进路控制模式申请结果(LiRC->GPC/STPC)
IMPLEMENT_CLBTH(MsgRouteModeResult, MSGID_MsgRouteModeResult)
IMPLEMENT_SERIALIZE_OPERATOR(MsgRouteModeResult)
MsgRouteModeResult::MsgRouteModeResult()
:	nStationNo(INVALID_STATION_NO), nCommentLen(0)
{
	strComment[0] = '\0';
}
MsgRouteModeResult::~MsgRouteModeResult()
{

}
HRCODE MsgRouteModeResult::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << nStationNo << nMode << nResult << nCommentLen;
		if ( nCommentLen > MAX_RESULT_COMMENT_LEN )
			return HRCODE(HRCODE::HC_FAIL);
		if ( nCommentLen > 0 )
			ar.write( strComment, nCommentLen );
	}
	else
	{
		ar >> nStationNo >> nMode >> nResult >> nCommentLen;
		if ( nCommentLen > MAX_RESULT_COMMENT_LEN )
			return HRCODE(HRCODE::HC_FAIL);
		if ( nCommentLen > 0 )
		{
			ar.read( strComment, nCommentLen );
			if ( nCommentLen < MAX_RESULT_COMMENT_LEN )
				strComment[nCommentLen] = 0;
		}
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgRouteModeResult::countObjectLength()
{
	return sizeof(nStationNo) + sizeof(nMode) + sizeof(nResult) + sizeof(nCommentLen) + nCommentLen;
}


// 车站控制状态报告(LiRC->GPC/STPC/BL/SERVER)
IMPLEMENT_CLBTH(MsgStationModeReport, MSGID_MsgStationModeReport)
IMPLEMENT_SERIALIZE_OPERATOR(MsgStationModeReport)
MsgStationModeReport::MsgStationModeReport()
:	nStationNo(INVALID_STATION_NO)
{

}
MsgStationModeReport::~MsgStationModeReport()
{

}
HRCODE MsgStationModeReport::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << nStationNo << nZoneID << nStationMode << nRouteMode << nRight << nCTCFlag;
	}
	else
	{
		ar >> nStationNo >> nZoneID >> nStationMode >> nRouteMode >> nRight >> nCTCFlag;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgStationModeReport::countObjectLength()
{
	return sizeof(nStationNo) + sizeof(nZoneID) + sizeof(nStationMode) + sizeof(nRouteMode) + sizeof(nCTCFlag) + sizeof(nRight);
}


// 车站CTC特征标志设置(GPC->LiRC)
IMPLEMENT_CLBTH(MsgStationCTCFlagSet, MSGID_MsgStationCTCFlagSet)
IMPLEMENT_SERIALIZE_OPERATOR(MsgStationCTCFlagSet)
MsgStationCTCFlagSet::MsgStationCTCFlagSet()
:	nStationNo(INVALID_STATION_NO)
{

}
MsgStationCTCFlagSet::~MsgStationCTCFlagSet()
{

}
HRCODE MsgStationCTCFlagSet::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << nStationNo << nFlag << nMask;
	}
	else
	{
		ar >> nStationNo >> nFlag >> nMask;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgStationCTCFlagSet::countObjectLength()
{
	return sizeof(nStationNo) + sizeof(nFlag) + sizeof(nMask);
}

// 查询车站控制模式
IMPLEMENT_CLBTH(MsgStationModeQuery, MSGID_MsgStationModeQuery)
IMPLEMENT_SERIALIZE_OPERATOR(MsgStationModeQuery)
MsgStationModeQuery::MsgStationModeQuery()
{
}
MsgStationModeQuery::~MsgStationModeQuery()
{
}
HRCODE MsgStationModeQuery::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << nStationNo;
	}
	else
	{
		ar >> nStationNo;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}

// ------------------------------ 进路相关消息 ----------------------------------------------------

// 进路序列申请(GPC/STPC->LiRC)
IMPLEMENT_CLBTH(MsgRouteListRequest, MSGID_MsgRouteListRequest)
IMPLEMENT_SERIALIZE_OPERATOR(MsgRouteListRequest)
MsgRouteListRequest::MsgRouteListRequest()
:	nStationNo(INVALID_STATION_NO)
{

}
MsgRouteListRequest::~MsgRouteListRequest()
{

}
HRCODE MsgRouteListRequest::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << nStationNo << nOperType << nHandle;
	}
	else
	{
		ar >> nStationNo >> nOperType >> nHandle;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgRouteListRequest::countObjectLength()
{
	return sizeof(nStationNo) + sizeof(nOperType) + sizeof(nHandle);
}

// 进路序列释放(GPC/STPC->LiRC)
IMPLEMENT_CLBTH(MsgRouteListRelease, MSGID_MsgRouteListRelease)
IMPLEMENT_SERIALIZE_OPERATOR(MsgRouteListRelease)
MsgRouteListRelease::MsgRouteListRelease()
:	nStationNo(INVALID_STATION_NO)
{

}
MsgRouteListRelease::~MsgRouteListRelease()
{

}
HRCODE MsgRouteListRelease::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << nStationNo << nOperType << nHandle;
	}
	else
	{
		ar >> nStationNo >> nOperType >> nHandle;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgRouteListRelease::countObjectLength()
{
	return sizeof(nStationNo) + sizeof(nOperType) + sizeof(nHandle);
}

// 进路序列申请回应
IMPLEMENT_CLBTH(MsgRouteListResult, MSGID_MsgRouteListResult)
IMPLEMENT_SERIALIZE_OPERATOR(MsgRouteListResult)
MsgRouteListResult::MsgRouteListResult()
:	nStationNo(INVALID_STATION_NO)
{

}
MsgRouteListResult::~MsgRouteListResult()
{

}
HRCODE MsgRouteListResult::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << nStationNo << nOperType << nHandle << nResult;
	}
	else
	{
		ar >> nStationNo >> nOperType >> nHandle >> nResult;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgRouteListResult::countObjectLength()
{
	return sizeof(nStationNo) + sizeof(nOperType) + sizeof(nHandle) + sizeof(nResult);
}

// 进路更改
IMPLEMENT_CLBTH(MsgRouteUpdate, MSGID_MsgRouteUpdate)
IMPLEMENT_SERIALIZE_OPERATOR(MsgRouteUpdate)
MsgRouteUpdate::MsgRouteUpdate()
:	nStationNo(INVALID_STATION_NO), nButtonCount(0), nListID(NULL_ROUTE_LIST_ID), nNewRouteID(NULL_ROUTE_ID)
{
	for ( int i = 0; i < MAX_ROUTE_BUTTON_COUNT; i++ )
		strButtonName[i][0] = '\0';
}
MsgRouteUpdate::~MsgRouteUpdate()
{

}
HRCODE MsgRouteUpdate::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << nStationNo << nHandle << nListID << nButtonCount;
		if ( nButtonCount > MAX_ROUTE_BUTTON_COUNT )
			return HRCODE(HRCODE::HC_FAIL);
		for ( int i = 0; i < nButtonCount; i++ )
		{
			ar << nButtonID[i];
			ar.write( strButtonName[i], sizeof(BUTTON_NAME) );
		}
		ar << (IBYTE)nRouteType << nNewRouteID << nGiveWayTo << nTrigFlag << nSideID;
	}
	else
	{
		ar >> nStationNo >> nHandle >> nListID >> nButtonCount;
		if ( nButtonCount > MAX_ROUTE_BUTTON_COUNT )
			return HRCODE(HRCODE::HC_FAIL);
		for ( int i = 0; i < nButtonCount; i++ )
		{
			ar >> nButtonID[i];
			ar.read( strButtonName[i], sizeof(BUTTON_NAME) );
			strButtonName[i][sizeof(BUTTON_NAME)-1] = 0;
		}
		IBYTE nTemp;
		ar >> nTemp >> nNewRouteID >> nGiveWayTo >> nTrigFlag >> nSideID;
		nRouteType = (ROUTE_TYPE)nTemp;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgRouteUpdate::countObjectLength()
{
	return sizeof(nStationNo) + sizeof(nHandle) + sizeof(nListID) + sizeof(nButtonCount)
		+ sizeof(nNewRouteID) + sizeof(nGiveWayTo) + sizeof(nTrigFlag)
		+ MAX_ROUTE_BUTTON_COUNT*(sizeof(BUTTON_ID)+sizeof(BUTTON_NAME)) + sizeof(IBYTE) + sizeof(short);
}

// 进路股道更改
IMPLEMENT_CLBTH(MsgRouteSideUpdate, MSGID_MsgRouteSideUpdate)
IMPLEMENT_SERIALIZE_OPERATOR(MsgRouteSideUpdate)
MsgRouteSideUpdate::MsgRouteSideUpdate()
:	nStationNo(INVALID_STATION_NO), nListID(NULL_ROUTE_LIST_ID), nSideID(INVALID_DMIS_ID)
{
}
MsgRouteSideUpdate::~MsgRouteSideUpdate()
{
}
HRCODE MsgRouteSideUpdate::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << nStationNo << nHandle << nListID << nSideID;
	}
	else
	{
		ar >> nStationNo >> nHandle >> nListID >> nSideID;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgRouteSideUpdate::countObjectLength()
{
	return sizeof(nStationNo) + sizeof(nHandle) + sizeof(nListID) + sizeof(nSideID);
}

// 进路按钮更改
IMPLEMENT_CLBTH(MsgRouteButtonUpdate, MSGID_MsgRouteButtonUpdate)
IMPLEMENT_SERIALIZE_OPERATOR(MsgRouteButtonUpdate)
MsgRouteButtonUpdate::MsgRouteButtonUpdate()
:	nStationNo(INVALID_STATION_NO), nButtonCount(0), nListID(NULL_ROUTE_LIST_ID), nNewRouteID(NULL_ROUTE_ID)
{
	for ( int i = 0; i < MAX_ROUTE_BUTTON_COUNT; i++ )
		strButtonName[i][0] = '\0';
}
MsgRouteButtonUpdate::~MsgRouteButtonUpdate()
{
}
HRCODE MsgRouteButtonUpdate::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << nStationNo << nHandle << nListID << nButtonCount;
		if ( nButtonCount > MAX_ROUTE_BUTTON_COUNT )
			return HRCODE(HRCODE::HC_FAIL);
		for ( int i = 0; i < nButtonCount; i++ )
		{
			ar << nButtonID[i];
			ar.write( strButtonName[i], sizeof(BUTTON_NAME) );
		}
		ar << (IBYTE)nRouteType << nNewRouteID;
	}
	else
	{
		ar >> nStationNo >> nHandle >> nListID >> nButtonCount;
		if ( nButtonCount > MAX_ROUTE_BUTTON_COUNT )
			return HRCODE(HRCODE::HC_FAIL);
		for ( int i = 0; i < nButtonCount; i++ )
		{
			ar >> nButtonID[i];
			ar.read( strButtonName[i], sizeof(BUTTON_NAME) );
			strButtonName[i][sizeof(BUTTON_NAME)-1] = 0;
		}
		IBYTE nTemp;
		ar >> nTemp >> nNewRouteID;
		nRouteType = (ROUTE_TYPE)nTemp;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgRouteButtonUpdate::countObjectLength()
{
	return sizeof(nStationNo) + sizeof(nHandle) + sizeof(nListID) + sizeof(nButtonCount)
		+ sizeof(nNewRouteID) + MAX_ROUTE_BUTTON_COUNT*(sizeof(BUTTON_ID)+sizeof(BUTTON_NAME)) + sizeof(IBYTE);
}

// 进路避让更改
IMPLEMENT_CLBTH(MsgRouteGiveWayUpdate, MSGID_MsgRouteGiveWayUpdate)
IMPLEMENT_SERIALIZE_OPERATOR(MsgRouteGiveWayUpdate)
MsgRouteGiveWayUpdate::MsgRouteGiveWayUpdate()
:	nStationNo(INVALID_STATION_NO), nListID(NULL_ROUTE_LIST_ID)
{
}
MsgRouteGiveWayUpdate::~MsgRouteGiveWayUpdate()
{
}
HRCODE MsgRouteGiveWayUpdate::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << nStationNo << nHandle << nListID << nGiveWayTo;
	}
	else
	{
		ar >> nStationNo >> nHandle >> nListID >> nGiveWayTo;
	}
	return HRCODE(HRCODE::HC_SUCCESS);	
}
size_t MsgRouteGiveWayUpdate::countObjectLength()
{
	return sizeof(nStationNo) + sizeof(nHandle) + sizeof(nListID) + sizeof(nGiveWayTo);
}

// 进路自动触发设置
IMPLEMENT_CLBTH(MsgRouteTrigFlagUpdate, MSGID_MsgRouteTrigFlagUpdate)
IMPLEMENT_SERIALIZE_OPERATOR(MsgRouteTrigFlagUpdate)
MsgRouteTrigFlagUpdate::MsgRouteTrigFlagUpdate()
:	nStationNo(INVALID_STATION_NO), nListID(NULL_ROUTE_LIST_ID)
{
}
MsgRouteTrigFlagUpdate::~MsgRouteTrigFlagUpdate()
{
}
HRCODE MsgRouteTrigFlagUpdate::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << nStationNo << nHandle << nListID << nTrigFlag;
	}
	else
	{
		ar >> nStationNo >> nHandle >> nListID >> nTrigFlag;
	}
	return HRCODE(HRCODE::HC_SUCCESS);	
}
size_t MsgRouteTrigFlagUpdate::countObjectLength()
{
	return sizeof(nStationNo) + sizeof(nHandle) + sizeof(nListID) + sizeof(nTrigFlag);
}

// 进路组合更改
IMPLEMENT_CLBTH(MsgRouteGroupUpdate, MSGID_MsgRouteGroupUpdate)
IMPLEMENT_SERIALIZE_OPERATOR(MsgRouteGroupUpdate)
MsgRouteGroupUpdate::MsgRouteGroupUpdate()
{
}
MsgRouteGroupUpdate::~MsgRouteGroupUpdate()
{
}
HRCODE MsgRouteGroupUpdate::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << nStationNo << nHandle << nListID << nNewRouteID << nRouteCount;
		if ( nRouteCount > MAX_ROUTE_COUNT_IN_ONE_PLAN )
			return HRCODE(HRCODE::HC_FAIL);
		for ( int i = 0; i < nRouteCount; i++ )
			ar << nzRoutes[i];
	}
	else
	{
		ar >> nStationNo >> nHandle >> nListID >> nNewRouteID >> nRouteCount;
		if ( nRouteCount > MAX_ROUTE_COUNT_IN_ONE_PLAN )
			return HRCODE(HRCODE::HC_FAIL);
		for ( int i = 0; i < nRouteCount; i++ )
			ar >> nzRoutes[i];
	}
	return HRCODE(HRCODE::HC_SUCCESS);	
}
size_t MsgRouteGroupUpdate::countObjectLength()
{
	return sizeof(nStationNo) + sizeof(nHandle) + sizeof(nListID) + sizeof(nNewRouteID) + sizeof(nRouteCount) + nRouteCount*sizeof(ROUTE_ID);
}

// 进路插入
IMPLEMENT_CLBTH(MsgRouteInsert, MSGID_MsgRouteInsert)
IMPLEMENT_SERIALIZE_OPERATOR(MsgRouteInsert)
MsgRouteInsert::MsgRouteInsert()
:	nStationNo(INVALID_STATION_NO), nButtonCount(0), nNewRouteID(NULL_ROUTE_ID)
{
	for ( int i = 0; i < MAX_ROUTE_BUTTON_COUNT; i++ )
		strButtonName[i][0] = '\0';
}
MsgRouteInsert::~MsgRouteInsert()
{

}
HRCODE MsgRouteInsert::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << nStationNo << nHandle;
		ar.write( tnTrainNo, MAX_TRAIN_NO_LEN );
		ar << nButtonCount;
		if ( nButtonCount > MAX_ROUTE_BUTTON_COUNT )
			return HRCODE(HRCODE::HC_FAIL);
		for ( int i = 0; i < nButtonCount; i++ )
		{
			ar << nButtonID[i];
			ar.write( strButtonName[i], sizeof(BUTTON_NAME) );
		}
		ar << (IBYTE)nRouteType << nNewRouteID << nNewPos << nGouIndex << nGiveWayTo << nTrigFlag << nOperID;
	}
	else
	{
		ar >> nStationNo >> nHandle;
		ar.read( tnTrainNo, MAX_TRAIN_NO_LEN );
		tnTrainNo[MAX_TRAIN_NO_LEN-1] = 0;
		ar >> nButtonCount;
		if ( nButtonCount > MAX_ROUTE_BUTTON_COUNT )
			return HRCODE(HRCODE::HC_FAIL);
		for ( int i = 0; i < nButtonCount; i++ )
		{
			ar >> nButtonID[i];
			ar.read( strButtonName[i], sizeof(BUTTON_NAME) );
			strButtonName[i][sizeof(BUTTON_NAME)-1] = 0;
		}
		IBYTE nTemp;
		ar >> nTemp >> nNewRouteID >> nNewPos >> nGouIndex >> nGiveWayTo >> nTrigFlag >> nOperID;
		nRouteType = (ROUTE_TYPE)nTemp;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgRouteInsert::countObjectLength()
{
	return sizeof(nStationNo) + sizeof(nHandle) + sizeof(nButtonCount) + sizeof(nNewRouteID)
		+ sizeof(nNewPos) + sizeof(nGouIndex) + sizeof(nGiveWayTo) + sizeof(nTrigFlag)
		+ MAX_ROUTE_BUTTON_COUNT*(sizeof(BUTTON_ID)+sizeof(BUTTON_NAME)) + MAX_TRAIN_NO_LEN + sizeof(nOperID) + sizeof(IBYTE);
}

// 进路删除
IMPLEMENT_CLBTH(MsgRouteDelete, MSGID_MsgRouteDelete)
IMPLEMENT_SERIALIZE_OPERATOR(MsgRouteDelete)
MsgRouteDelete::MsgRouteDelete()
:	nStationNo(INVALID_STATION_NO), nListID(NULL_ROUTE_LIST_ID)
{

}
MsgRouteDelete::~MsgRouteDelete()
{

}
HRCODE MsgRouteDelete::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << nStationNo << nHandle << nListID;
	}
	else
	{
		ar >> nStationNo >> nHandle >> nListID;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgRouteDelete::countObjectLength()
{
	return sizeof(nStationNo) + sizeof(nHandle) + sizeof(nListID);
}

// 进路顺序调整
IMPLEMENT_CLBTH(MsgRouteMove, MSGID_MsgRouteMove)
IMPLEMENT_SERIALIZE_OPERATOR(MsgRouteMove)
MsgRouteMove::MsgRouteMove()
:	nStationNo(INVALID_STATION_NO), nListID(NULL_ROUTE_LIST_ID)
{

}
MsgRouteMove::~MsgRouteMove()
{

}
HRCODE MsgRouteMove::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << nStationNo << nHandle << nListID << nNewPos;
	}
	else
	{
		ar >> nStationNo >> nHandle >> nListID >> nNewPos;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgRouteMove::countObjectLength()
{
	return sizeof(nStationNo) + sizeof(nHandle) + sizeof(nListID) + sizeof(nNewPos);
}

// 进路人工触发
IMPLEMENT_CLBTH(MsgRouteTrig, MSGID_MsgRouteTrig)
IMPLEMENT_SERIALIZE_OPERATOR(MsgRouteTrig)
MsgRouteTrig::MsgRouteTrig()
:	nStationNo(INVALID_STATION_NO), nListID(NULL_ROUTE_LIST_ID)
{

}
MsgRouteTrig::~MsgRouteTrig()
{

}
HRCODE MsgRouteTrig::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << nStationNo << nHandle << nListID << tmTrigTime << bForceFlag;
	}
	else
	{
		ar >> nStationNo >> nHandle >> nListID >> tmTrigTime >> bForceFlag;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgRouteTrig::countObjectLength()
{
	return sizeof(nStationNo) + sizeof(nHandle) + sizeof(nListID) + sizeof(bForceFlag) + sizeof(tmTrigTime);
}

// 进路组触发
IMPLEMENT_CLBTH(MsgRouteGroupTrig, MSGID_MsgRouteGroupTrig)
IMPLEMENT_SERIALIZE_OPERATOR(MsgRouteGroupTrig)
MsgRouteGroupTrig::MsgRouteGroupTrig()
:	nStationNo(INVALID_STATION_NO)
{
}
MsgRouteGroupTrig::~MsgRouteGroupTrig()
{
}
HRCODE MsgRouteGroupTrig::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << nStationNo << nHandle << nGroupID << tmTrigTime << bForceFlag;
	}
	else
	{
		ar >> nStationNo >> nHandle >> nGroupID >> tmTrigTime >> bForceFlag;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgRouteGroupTrig::countObjectLength()
{
	return sizeof(nStationNo) + sizeof(nHandle) + sizeof(nGroupID) + sizeof(bForceFlag) + sizeof(tmTrigTime);
}

// 进路操作回应
IMPLEMENT_CLBTH(MsgRouteOperResult, MSGID_MsgRouteOperResult)
IMPLEMENT_SERIALIZE_OPERATOR(MsgRouteOperResult)
MsgRouteOperResult::MsgRouteOperResult()
:	nStationNo(INVALID_STATION_NO), nCommentLen(0)
{
	strComment[0] = '\0';
}
MsgRouteOperResult::~MsgRouteOperResult()
{

}
HRCODE MsgRouteOperResult::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << nStationNo << nHandle << nListID << (IBYTE)nOperType << nResult << nCommentLen;
		if ( nCommentLen > MAX_RESULT_COMMENT_LEN )
			return HRCODE(HRCODE::HC_FAIL);
		if ( nCommentLen > 0 )
			ar.write( strComment, nCommentLen );
	}
	else
	{
		IBYTE nTemp;
		ar >> nStationNo >> nHandle >> nListID >> nTemp >> nResult >> nCommentLen;
		nOperType = (OPER_TYPE)nTemp;
		if ( nCommentLen > MAX_RESULT_COMMENT_LEN )
		{
			nCommentLen =0;
			return HRCODE(HRCODE::HC_FAIL);
		}
		if ( nCommentLen > 0 )
		{
			ar.read( strComment, nCommentLen );
			if ( nCommentLen < MAX_RESULT_COMMENT_LEN )
				strComment[nCommentLen] = 0;
		}
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgRouteOperResult::countObjectLength()
{
	return sizeof(nStationNo) + sizeof(nHandle) + sizeof(nListID) + sizeof(nResult) + sizeof(IBYTE) + sizeof(nCommentLen) + nCommentLen;
}
bool	MsgRouteOperResult::CanForceOper() const
{
	return ( nResult & 0x80 ) != 0;
}
void	MsgRouteOperResult::SetForceOperFlag( bool b )
{
	if ( b )
		nResult |= 0x80;
	else
		nResult &= 0x7F;
}
ROUTE_OPER_RESULT	MsgRouteOperResult::GetResult()
{
	return nResult & 0x7F;
}

// 进路组操作回应
IMPLEMENT_CLBTH(MsgRouteGroupOperResult, MSGID_MsgRouteGroupOperResult)
IMPLEMENT_SERIALIZE_OPERATOR(MsgRouteGroupOperResult)
MsgRouteGroupOperResult::MsgRouteGroupOperResult()
:	nStationNo(INVALID_STATION_NO), nCommentLen(0)
{
}
MsgRouteGroupOperResult::~MsgRouteGroupOperResult()
{
}
HRCODE MsgRouteGroupOperResult::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << nStationNo << nHandle << nGroupID << (IBYTE)nOperType << nResult << nCanForceOper << nCommentLen;
		if ( nCommentLen > MAX_RESULT_COMMENT_LEN )
			return HRCODE(HRCODE::HC_FAIL);
		if ( nCommentLen > 0 )
			ar.write( strComment, nCommentLen );
	}
	else
	{
		IBYTE nTemp;
		ar >> nStationNo >> nHandle >> nGroupID >> nTemp >> nResult >> nCanForceOper >> nCommentLen;
		nOperType = (OPER_TYPE)nTemp;
		if ( nCommentLen > MAX_RESULT_COMMENT_LEN )
			return HRCODE(HRCODE::HC_FAIL);
		if ( nCommentLen > 0 )
		{
			ar.read( strComment, nCommentLen );
			if ( nCommentLen < MAX_RESULT_COMMENT_LEN )
				strComment[nCommentLen] = 0;
		}
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgRouteGroupOperResult::countObjectLength()
{
	return sizeof(nStationNo) + sizeof(nHandle) + sizeof(nGroupID) + sizeof(nResult) + sizeof(IBYTE) + sizeof(nCanForceOper) + sizeof(nCommentLen) + nCommentLen;
}

// 进路长表示信息
IMPLEMENT_CLBTH(MsgRouteLongInfo, MSGID_MsgRouteLongInfo)
IMPLEMENT_SERIALIZE_OPERATOR(MsgRouteLongInfo)
MsgRouteLongInfo::MsgRouteLongInfo()
:	nStationNo(INVALID_STATION_NO), nListID(NULL_ROUTE_LIST_ID), nButtonCount(0)
{

}
MsgRouteLongInfo::~MsgRouteLongInfo()
{

}
HRCODE MsgRouteLongInfo::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << nStationNo << nHandle << nListID << nPos;
		ar.write( tnTrainNo, MAX_TRAIN_NO_LEN );
		ar << nButtonCount;
		if ( nButtonCount > MAX_ROUTE_BUTTON_COUNT )
			return HRCODE(HRCODE::HC_FAIL);
		for ( int i = 0; i < nButtonCount; i++ )
			ar << nButtonID[i];
		ar << nRouteID << nType << nSideID << nEntryID << nExitID << nGouIndex << nGouType
			<< nGiveWayTo << nStatus << tmStart << tOccupy << nGroupID;
	}
	else
	{
		ar >> nStationNo >> nHandle >> nListID >> nPos;
		ar.read( tnTrainNo, MAX_TRAIN_NO_LEN );
		tnTrainNo[MAX_TRAIN_NO_LEN-1] = '\0';
		ar >> nButtonCount;
		if ( nButtonCount > MAX_ROUTE_BUTTON_COUNT )
			return HRCODE(HRCODE::HC_FAIL);
		for ( int i = 0; i < nButtonCount; i++ )
			ar >> nButtonID[i];
		ar >> nRouteID >> nType >> nSideID >> nEntryID >> nExitID >> nGouIndex >> nGouType
			>> nGiveWayTo >> nStatus >> tmStart >> tOccupy >> nGroupID;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgRouteLongInfo::countObjectLength()
{
	return sizeof(nStationNo) + sizeof(nHandle) + sizeof(nListID) + sizeof(nPos) + MAX_TRAIN_NO_LEN
		+ sizeof(nButtonCount) + MAX_ROUTE_BUTTON_COUNT*sizeof(BUTTON_ID) +sizeof(nRouteID)
		+ sizeof(nType) + sizeof(nSideID) + sizeof(nEntryID) + sizeof(nExitID) + sizeof(nGouIndex)
		+ sizeof(nGouType) + sizeof(nGiveWayTo) + sizeof(nStatus) + sizeof(tmStart) + sizeof(tOccupy) +sizeof(nGroupID);
}

// 进路长表示信息新
IMPLEMENT_CLBTH(MsgRouteLongInfoNew, MSGID_MsgRouteLongInfoNew)
IMPLEMENT_SERIALIZE_OPERATOR(MsgRouteLongInfoNew)
MsgRouteLongInfoNew::	MsgRouteLongInfoNew()
:	tmPlan(0), MsgRouteLongInfo()
{
	strDesc[0] = '\0';
}
MsgRouteLongInfoNew::~MsgRouteLongInfoNew()
{
}
MsgRouteLongInfoNew::	MsgRouteLongInfoNew(class MsgRouteLongInfoNew const & msg)
{
	*this = msg;
}
class MsgRouteLongInfoNew& MsgRouteLongInfoNew::operator=(class MsgRouteLongInfoNew const & msg)
{
	nStationNo   = msg.nStationNo;
	nHandle      = msg.nHandle;
	nListID      = msg.nListID;
	nPos		 = msg.nPos;				
	memcpy(tnTrainNo, msg.tnTrainNo, sizeof(tnTrainNo));
	nButtonCount = msg.nButtonCount;
	memcpy(nButtonID, msg.nButtonID, sizeof(nButtonID));
	nRouteID     = msg.nRouteID;
	nType        = msg.nType;
	nSideID      = msg.nSideID;
	nEntryID     = msg.nEntryID;
	nExitID      = msg.nExitID;
	nGouIndex    = msg.nGouIndex;
	nGouType     = msg.nGouType;
	nGiveWayTo   = msg.nGiveWayTo;
	nStatus      = msg.nStatus;
	tmStart      = msg.tmStart;
	tOccupy      = msg.tOccupy;
	nGroupID     = msg.nGroupID;
	tmPlan            = msg.tmPlan;
	nTrainIndex       = msg.nTrainIndex;
	nRecIndex         = msg.nRecIndex;
	memcpy(strDesc, msg.strDesc, sizeof(strDesc));
	return *this;
}
HRCODE MsgRouteLongInfoNew::doSerialize(Archive& ar)
{
	HRCODE rs = MsgRouteLongInfo::doSerialize(ar);
	if ( rs != HRCODE(HRCODE::HC_SUCCESS) )
		return rs;

	if ( ar.isStoring() )
	{
		ar << tmPlan << nTrainIndex << nRecIndex;
		strDesc[MAX_DMIS_STR_LEN-1] = '\0';
		short len = strlen(strDesc);
		if ( len >= MAX_DMIS_STR_LEN )
			return HRCODE(HRCODE::HC_FAIL);
		ar << len;
		ar.write(strDesc, len);
	}
	else
	{
		short len = 0;
		ar >> tmPlan >> nTrainIndex >> nRecIndex >> len;
		if ( len >= MAX_DMIS_STR_LEN )
			return HRCODE(HRCODE::HC_FAIL);
		ar.read(strDesc, len);
		strDesc[len] = '\0';
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgRouteLongInfoNew::countObjectLength()
{
	return sizeof(tmPlan) + sizeof(nTrainIndex) + sizeof(nRecIndex) + sizeof(short) + strlen(strDesc) + MsgRouteLongInfo::countObjectLength();
}

// 进路短表示信息(当只有ROUTE_STATUS改变时发送短表示)
IMPLEMENT_CLBTH(MsgRouteShortInfo, MSGID_MsgRouteShortInfo)
IMPLEMENT_SERIALIZE_OPERATOR(MsgRouteShortInfo)
MsgRouteShortInfo::MsgRouteShortInfo()
:	nStationNo(INVALID_STATION_NO), nListID(NULL_ROUTE_LIST_ID)
{

}
MsgRouteShortInfo::~MsgRouteShortInfo()
{

}
HRCODE MsgRouteShortInfo::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << nStationNo << nHandle << nListID << nStatus;
	}
	else
	{
		ar >> nStationNo >> nHandle >> nListID >> nStatus;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgRouteShortInfo::countObjectLength()
{
	return sizeof(nStationNo) + sizeof(nHandle) + sizeof(nListID) + sizeof(nStatus);
}

// 进路表示删除
IMPLEMENT_CLBTH(MsgRouteInfoDel, MSGID_MsgRouteInfoDel)
IMPLEMENT_SERIALIZE_OPERATOR(MsgRouteInfoDel)
MsgRouteInfoDel::MsgRouteInfoDel()
:	nStationNo(INVALID_STATION_NO), nListID(NULL_ROUTE_LIST_ID)
{

}
MsgRouteInfoDel::~MsgRouteInfoDel()
{

}
HRCODE MsgRouteInfoDel::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << nStationNo << nHandle << nListID;
	}
	else
	{
		ar >> nStationNo >> nHandle >> nListID;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgRouteInfoDel::countObjectLength()
{
	return sizeof(nStationNo) + sizeof(nHandle) + sizeof(nListID);
}

// 进路表示移动位置
IMPLEMENT_CLBTH(MsgRouteInfoMove, MSGID_MsgRouteInfoMove)
IMPLEMENT_SERIALIZE_OPERATOR(MsgRouteInfoMove)
MsgRouteInfoMove::MsgRouteInfoMove()
:	nStationNo(INVALID_STATION_NO), nMoveID(NULL_ROUTE_LIST_ID)
{

}
MsgRouteInfoMove::~MsgRouteInfoMove()
{

}
HRCODE MsgRouteInfoMove::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << nStationNo << nHandle << nMoveID << nPrevID;
	}
	else
	{
		ar >> nStationNo >> nHandle >> nMoveID >> nPrevID;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgRouteInfoMove::countObjectLength()
{
	return sizeof(nStationNo) + sizeof(nHandle) + sizeof(nMoveID) + sizeof(nPrevID);
}

// 进路表示刷新
IMPLEMENT_CLBTH(MsgRouteInfoRefresh, MSGID_MsgRouteInfoRefresh)
IMPLEMENT_SERIALIZE_OPERATOR(MsgRouteInfoRefresh)
MsgRouteInfoRefresh::MsgRouteInfoRefresh()
{
}
MsgRouteInfoRefresh::~MsgRouteInfoRefresh()
{
}
HRCODE MsgRouteInfoRefresh::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << nStationNo << nHandle;
	}
	else
	{
		ar >> nStationNo >> nHandle;
	}
	return HRCODE(HRCODE::HC_SUCCESS);	
}

// 进路优先级比较
IMPLEMENT_CLBTH(MsgRouteCompare, MSGID_MsgRouteCompare)
IMPLEMENT_SERIALIZE_OPERATOR(MsgRouteCompare)
MsgRouteCompare::MsgRouteCompare()
	: nStationNo(INVALID_STATION_NO),
	  nRouteCount(0)
{
}
MsgRouteCompare::~MsgRouteCompare()
{
}
HRCODE MsgRouteCompare::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << nStationNo << nRouteCount << nTimeout;
		if ( nRouteCount > MAX_ROUTE_COUNT )
			return HRCODE(HRCODE::HC_FAIL);
		for ( int i = 0; i < nRouteCount; i++ )
		{
			ar.write( Routes[i].tnTrain, MAX_TRAIN_NO_LEN );
			ar << Routes[i].nRunType << Routes[i].nListID;
			Routes[i].strDesc[sizeof(Routes[i].strDesc)-1] = '\0';
			short nLen = strlen(Routes[i].strDesc);
			ar << nLen;
			ar.write( Routes[i].strDesc, nLen );
			ar << nzPriority[i];
		}
		ar << nResult;
	}
	else
	{
		ar >> nStationNo >> nRouteCount >> nTimeout;
		if ( nRouteCount > MAX_ROUTE_COUNT )
			return HRCODE(HRCODE::HC_FAIL);
		for ( int i = 0; i < nRouteCount; i++ )
		{
			ar.read( Routes[i].tnTrain, MAX_TRAIN_NO_LEN );
			Routes[i].tnTrain[MAX_TRAIN_NO_LEN-1] = '\0';
			short nLen = 0;
			ar >> Routes[i].nRunType >> Routes[i].nListID >> nLen;
			if ( nLen < 0 || nLen >= sizeof(Routes[i].strDesc) )
				return HRCODE(HRCODE::HC_FAIL);
			ar.read( Routes[i].strDesc, nLen );
			Routes[i].strDesc[nLen] = '\0';
			ar >> nzPriority[i];
		}
		ar >> nResult;
	}
	return HRCODE(HRCODE::HC_SUCCESS);

}
size_t MsgRouteCompare::countObjectLength()
{
	size_t nLen = sizeof(nStationNo) + sizeof(nRouteCount) + sizeof(nResult) + sizeof(nTimeout);
	for ( int i = 0; i < nRouteCount; i++ )
	{
		nLen += MAX_TRAIN_NO_LEN + sizeof(Routes[i].nRunType) + sizeof(Routes[i].nListID) + sizeof(nzPriority[i]);
		Routes[i].strDesc[sizeof(Routes[i].strDesc)-1] = '\0';
		nLen += sizeof(short) + strlen(Routes[i].strDesc);
	}
	return nLen;
}

// ----------------------- 自律机之间、自律机与Baselevel之间的消息 ------------------------------------------------------------

// 码位全体
IMPLEMENT_CLBTH(MsgBitblock, MSGID_MsgBitblock)
IMPLEMENT_SERIALIZE_OPERATOR(MsgBitblock)
MsgBitblock::MsgBitblock()
:	nStationNo(INVALID_STATION_NO), nBitblockLen(0)
{

}
MsgBitblock::~MsgBitblock()
{

}
HRCODE MsgBitblock::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << nStationNo << nBitblockLen;
		if ( nBitblockLen > MAX_BITBLOCK_LEN )
			return HRCODE(HRCODE::HC_FAIL);
		for ( int i = 0; i < nBitblockLen; i++ )
			ar << szBitblock[i];
	}
	else
	{
		ar >> nStationNo >> nBitblockLen;
		if ( nBitblockLen > MAX_BITBLOCK_LEN )
			return HRCODE(HRCODE::HC_FAIL);
		for ( int i = 0; i < nBitblockLen; i++ )
			ar >> szBitblock[i];
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgBitblock::countObjectLength()
{
	return sizeof(nStationNo) + sizeof(nBitblockLen) + nBitblockLen;
}

// 码位变化
IMPLEMENT_CLBTH(MsgIndiChange, MSGID_MsgIndiChange)
IMPLEMENT_SERIALIZE_OPERATOR(MsgIndiChange)
MsgIndiChange::MsgIndiChange()
:	nStationNo(INVALID_STATION_NO), nIndiChangeCount(0)
{

}
MsgIndiChange::~MsgIndiChange()
{

}
HRCODE MsgIndiChange::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << nStationNo << nIndiChangeCount;
		if ( nIndiChangeCount > MAX_INDI_PER_MSG )
			return HRCODE(HRCODE::HC_FAIL);
		for ( int i = 0; i < nIndiChangeCount; i++ )
			ar << szIndiChange[i];
	}
	else
	{
		ar >> nStationNo >> nIndiChangeCount;
		if ( nIndiChangeCount > MAX_INDI_PER_MSG )
			return HRCODE(HRCODE::HC_FAIL);
		for ( int i = 0; i < nIndiChangeCount; i++ )
			ar >> szIndiChange[i];
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgIndiChange::countObjectLength()
{
	return sizeof(nStationNo) + sizeof(nIndiChangeCount) + nIndiChangeCount*sizeof(INDI_CHANGE);
}

// 主备状态报告
IMPLEMENT_CLBTH(MsgMasterStatus, MSGID_MsgMasterStatus)
IMPLEMENT_SERIALIZE_OPERATOR(MsgMasterStatus)
MsgMasterStatus::MsgMasterStatus()
{

}
MsgMasterStatus::~MsgMasterStatus()
{

}
HRCODE MsgMasterStatus::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << nStationNo << nMasterFlag << nABFlag;
	}
	else
	{
		ar >> nStationNo >> nMasterFlag >> nABFlag;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgMasterStatus::countObjectLength()
{
	return sizeof(nMasterFlag) + sizeof(nABFlag) + sizeof(nStationNo);
}

// 自律机状态报告
IMPLEMENT_CLBTH(MsgLiRCStatusReport, MSGID_MsgLiRCStatusReport)
IMPLEMENT_SERIALIZE_OPERATOR(MsgLiRCStatusReport)
MsgLiRCStatusReport::MsgLiRCStatusReport()
:	nNetStatus(INACTIVE), bHasCISConn(false), bHasBoardConn(false), bHasSimuConn(false),
nDIBStart(0), nDOBStart(0), nDIBCount(0), nDOBCount(0), nWirelessStatus(INACTIVE)
{
}
MsgLiRCStatusReport::~MsgLiRCStatusReport()
{
}
HRCODE MsgLiRCStatusReport::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << nStationNo << nNetStatus << bHasCISConn << bHasBoardConn << bHasSimuConn \
			<< nCISStatus[0] << nCISStatus[1] << nDIBStatus[0] << nDIBStatus[1] << nDOBStatus[0] << nDOBStatus[1] \
			<< nDIBStart << nDIBCount << nDOBStart << nDOBCount << nSimuStatus << nWirelessStatus;
	}
	else
	{
		ar >> nStationNo >> nNetStatus >> bHasCISConn >> bHasBoardConn >> bHasSimuConn \
			>> nCISStatus[0] >> nCISStatus[1] >> nDIBStatus[0] >> nDIBStatus[1] >> nDOBStatus[0] >> nDOBStatus[1] \
			>> nDIBStart >> nDIBCount >> nDOBStart >> nDOBCount >> nSimuStatus >> nWirelessStatus;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}

// 自律机状态报告(发送电务维修机)
IMPLEMENT_CLBTH(MsgLiRCStatusReportNew, MSGID_MsgLiRCStatusReportNew)
IMPLEMENT_SERIALIZE_OPERATOR(MsgLiRCStatusReportNew)
MsgLiRCStatusReportNew::MsgLiRCStatusReportNew()
{
}
MsgLiRCStatusReportNew::~MsgLiRCStatusReportNew()
{
}
HRCODE MsgLiRCStatusReportNew::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << nStationNo << nMasterStatus << nABFlag << nNetStatus << nStbyStatus << nInternalStatus;
		int i = 0;
		for ( i = 0; i < 2; i++ )
			ar << nCISStatus[i];
		for ( i = 0; i < 2; i++ )
			ar << nStccStatus[i];
		ar << nWirelessStatus << nDIBCount;
		for ( i = 0; i < 2; i++ )
			ar << nDIBStatus[i];
		for ( i = 0; i < 8; i++ )
			ar << nOtherStatus[i];
	}
	else
	{
		ar >> nStationNo >> nMasterStatus >> nABFlag >> nNetStatus >> nStbyStatus >> nInternalStatus;
		int i = 0;
		for ( i = 0; i < 2; i++ )
			ar >> nCISStatus[i];
		for ( i = 0; i < 2; i++ )
			ar >> nStccStatus[i];
		ar >> nWirelessStatus >> nDIBCount;
		for ( i = 0; i < 2; i++ )
			ar >> nDIBStatus[i];
		for ( i = 0; i < 8; i++ )
			ar >> nOtherStatus[i];
	}
	return HRCODE(HRCODE::HC_SUCCESS);

}

// 模拟CTC输出
IMPLEMENT_CLBTH(MsgCTCOutput, MSGID_MsgCTCOutput)
IMPLEMENT_SERIALIZE_OPERATOR(MsgCTCOutput)
MsgCTCOutput::MsgCTCOutput()
	:	nStationNo(INVALID_STATION_NO), nCtrlID(0), nHandleCount(0)
{
	for ( int i = 0; i < MAX_CTC_OUTPUT_HANDLE_COUNT; i++ )
	{
		nOutputHandles[i] = -1;
		nInputHandles[i] = -1;
	}
}
MsgCTCOutput::~MsgCTCOutput()
{

}
HRCODE MsgCTCOutput::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << nStationNo << tm << nCtrlID << nHandleCount;
		if ( nHandleCount > MAX_CTC_OUTPUT_HANDLE_COUNT )
			return HRCODE(HRCODE::HC_FAIL);
		for ( int i = 0; i < nHandleCount; i++ )
			ar << nOutputHandles[i] << nInputHandles[i];
	}
	else
	{
		ar >> nStationNo >> tm >> nCtrlID >> nHandleCount;
		if ( nHandleCount > MAX_CTC_OUTPUT_HANDLE_COUNT )
			return HRCODE(HRCODE::HC_FAIL);
		for ( int i = 0; i < nHandleCount; i++ )
			ar >> nOutputHandles[i] >> nInputHandles[i];
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgCTCOutput::countObjectLength()
{
	return sizeof(nCtrlID) + sizeof(nHandleCount) + sizeof(nStationNo) + sizeof(tm)
		+ 2*nHandleCount*sizeof(short);
}

// 预计出发时间
IMPLEMENT_CLBTH(MsgTrainDepartTime, MSGID_MsgTrainDepartTime)
IMPLEMENT_SERIALIZE_OPERATOR(MsgTrainDepartTime)
MsgTrainDepartTime::MsgTrainDepartTime()
:	nStationNo(INVALID_STATION_NO), nTrainIndex(NULL_TRAIN_INDEX)
{
}
MsgTrainDepartTime::~MsgTrainDepartTime()
{
}
HRCODE MsgTrainDepartTime::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << nStationNo << nTrainIndex;
		ar.write( tnTrainNo, MAX_TRAIN_NO_LEN );
		ar << tTime << nTimeSource;
	}
	else
	{
		ar >> nStationNo >> nTrainIndex;
		ar.read( tnTrainNo, MAX_TRAIN_NO_LEN );
		tnTrainNo[MAX_TRAIN_NO_LEN-1] = '\0';
		ar >> tTime >> nTimeSource;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}

// 车次号操作
IMPLEMENT_CLBTH(MsgIPTrainOper, MSGID_MsgIPTrainOper)
IMPLEMENT_SERIALIZE_OPERATOR(MsgIPTrainOper)
MsgIPTrainOper::MsgIPTrainOper()
:	nInfoLen(0)
{
}
MsgIPTrainOper::~MsgIPTrainOper()
{
}
HRCODE MsgIPTrainOper::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << (IBYTE)nType << nInfoLen;
		if ( nInfoLen > sizeof(nzInfo) )
			return HRCODE(HRCODE::HC_FAIL);
		ar.write( nzInfo, nInfoLen );
	}
	else
	{
		IBYTE nTemp;
		ar >> nTemp >> nInfoLen;
		nType = (TOP_TYPE)nTemp;
		if ( nInfoLen > sizeof(nzInfo) )
			return HRCODE(HRCODE::HC_FAIL);
		ar.read( nzInfo, nInfoLen );
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgIPTrainOper::countObjectLength()
{
	return sizeof(IBYTE) + sizeof(nInfoLen) + sizeof(IBYTE)*nInfoLen;
}

// 邻站传递元素CTC状态
IMPLEMENT_CLBTH(MsgIconCTCStatus, MSGID_MsgIconCTCStatus)
IMPLEMENT_SERIALIZE_OPERATOR(MsgIconCTCStatus)
MsgIconCTCStatus::MsgIconCTCStatus()
	:	nInfoLen(0)
{
}
MsgIconCTCStatus::~MsgIconCTCStatus()
{
}
HRCODE MsgIconCTCStatus::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << nInfoLen;
		if ( nInfoLen > sizeof(nzInfo) )
			return HRCODE(HRCODE::HC_FAIL);
		ar.write( nzInfo, nInfoLen );
	}
	else
	{
		ar >> nInfoLen;
		if ( nInfoLen > sizeof(nzInfo) )
			return HRCODE(HRCODE::HC_FAIL);
		ar.read( nzInfo, nInfoLen );
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgIconCTCStatus::countObjectLength()
{
	return sizeof(nInfoLen) + sizeof(IBYTE)*nInfoLen;
}

// --------------------------- 报警消息 --------------------------------------------------------------------
IMPLEMENT_CLBTH(MsgAlarm, MSGID_MsgAlarm)
IMPLEMENT_SERIALIZE_OPERATOR(MsgAlarm)
MsgAlarm::MsgAlarm()
{
	CLBTH_ZERO_INIT_OBJECT(BaseMsg)
}
MsgAlarm::~MsgAlarm()
{

}
HRCODE MsgAlarm::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << nType << nVerbose << nAlarmID << nTime << nLen;
		if ( nLen > MAX_ALARM_CONTENT_LEN )
			return HRCODE(HRCODE::HC_FAIL);
		if ( nLen > 0 )
			ar.write( szContent, nLen );
	}
	else
	{
		ar >> nType >> nVerbose >> nAlarmID >> nTime >> nLen;
		if ( nLen > MAX_ALARM_CONTENT_LEN )
			return HRCODE(HRCODE::HC_FAIL);
		if ( nLen > 0 )
		{
			ar.read( szContent, nLen );
			if ( nLen < MAX_ALARM_CONTENT_LEN )
				szContent[nLen] = 0;
		}
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgAlarm::countObjectLength()
{
	return sizeof(nType) + sizeof(nVerbose) + sizeof(nAlarmID)
		+ sizeof(nTime) + sizeof(nLen) + nLen;
}

I_BOOL MsgAlarm::NeedRespond()
{
	return IFALSE;
}


IMPLEMENT_CLBTH(MsgAlarmEx, MSGID_MsgAlarmEx)
IMPLEMENT_SERIALIZE_OPERATOR(MsgAlarmEx)
MsgAlarmEx::MsgAlarmEx()
{
	CLBTH_ZERO_INIT_OBJECT(MsgAlarm)
}
MsgAlarmEx::~MsgAlarmEx()
{
}

I_BOOL MsgAlarmEx::NeedRespond()
{
	return ITRUE;
}


IMPLEMENT_CLBTH(MsgAlarmExd, MSGID_MsgAlarmExd)
IMPLEMENT_SERIALIZE_OPERATOR(MsgAlarmExd)
MsgAlarmExd::MsgAlarmExd()
{
	CLBTH_ZERO_INIT_OBJECT(MsgAlarmExd)
}
MsgAlarmExd::MsgAlarmExd(const MsgAlarm &msg, ENTITY_ID src)
{
	source = src;
	nType = msg.nType;
	nVerbose = msg.nVerbose;
	nAlarmID = msg.nAlarmID;
	nTime = msg.nTime;
	nLen = msg.nLen;
	strcpy(szContent, msg.szContent);
}
MsgAlarmExd::~MsgAlarmExd()
{
}

HRCODE MsgAlarmExd::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << source;
		ar << nType << nVerbose << nAlarmID << nTime << nLen;
		if ( nLen > MAX_ALARM_CONTENT_LEN )
			return HRCODE(HRCODE::HC_FAIL);
		if ( nLen > 0 )
			ar.write( szContent, nLen );
	}
	else
	{
		ar >> source;
		ar >> nType >> nVerbose >> nAlarmID >> nTime >> nLen;
		if ( nLen > MAX_ALARM_CONTENT_LEN )
			return HRCODE(HRCODE::HC_FAIL);
		if ( nLen > 0 )
		{
			ar.read( szContent, nLen );
			if ( nLen < MAX_ALARM_CONTENT_LEN )
				szContent[nLen] = 0;
		}
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgAlarmExd::countObjectLength()
{
	return sizeof(source)+MsgAlarm::countObjectLength();
}

I_BOOL MsgAlarmExd::NeedRespond()
{
	return IFALSE;
}

/////////////// 告警注册消息
IMPLEMENT_CLBTH(MsgAlarmReg, MSGID_MsgAlarmReg)
IMPLEMENT_SERIALIZE_OPERATOR(MsgAlarmReg)
MsgAlarmReg::MsgAlarmReg()
{
	CLBTH_ZERO_INIT_OBJECT(BaseMsg);
}
MsgAlarmReg::~MsgAlarmReg()
{

}
HRCODE MsgAlarmReg::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << nSrcEntityID << nStatus;		
	}
	else
	{
		int t;
		ar >> nSrcEntityID >> t;							nStatus = (ReqStatus)t;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgAlarmReg::countObjectLength()
{
	return sizeof(nSrcEntityID)+sizeof(nStatus);
}

/////////////// 告警查询消息
IMPLEMENT_CLBTH(MsgAlarmReq, MSGID_MsgAlarmReq)
IMPLEMENT_SERIALIZE_OPERATOR(MsgAlarmReq)
MsgAlarmReq::MsgAlarmReq()
{
	CLBTH_ZERO_INIT_OBJECT(BaseMsg);
}
MsgAlarmReq::~MsgAlarmReq()
{	
}
HRCODE MsgAlarmReq::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << nSrcEntityID << nStartTime << nEndTime;	
	}
	else
	{
		ar >> nSrcEntityID >> nStartTime >> nEndTime;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgAlarmReq::countObjectLength()
{
	return sizeof(nSrcEntityID)+sizeof(nStartTime)+sizeof(nEndTime);
}

// --------------------------- 登录消息 --------------------------------------------------------------------
IMPLEMENT_CLBTH(MsgLogin, MSGID_MsgLogin)
IMPLEMENT_SERIALIZE_OPERATOR(MsgLogin)
MsgLogin::MsgLogin()
:	nStationCount(0), nEntityCount(0)
{

}
MsgLogin::~MsgLogin()
{

}
HRCODE MsgLogin::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << nStationCount;
		if ( nStationCount > LOGIN_MAX_STATION_COUNT )
			return HRCODE(HRCODE::HC_FAIL);
		int i;
		for ( i = 0; i < nStationCount; i++ )
			ar << Stations[i];
		ar << nEntityCount;
		if ( nEntityCount > LOGIN_MAX_ENTITY_COUNT )
			return HRCODE(HRCODE::HC_FAIL);
		for ( i = 0; i < nEntityCount; i++ )
			ar << Entities[i];
		ar << nMasterFlag << nABFlag;
	}
	else
	{
		ar >> nStationCount;
		if ( nStationCount > LOGIN_MAX_STATION_COUNT )
			return HRCODE(HRCODE::HC_FAIL);
		int i;
		for ( i = 0; i < nStationCount; i++ )
			ar >> Stations[i];
		ar >> nEntityCount;
		if ( nEntityCount > LOGIN_MAX_ENTITY_COUNT )
			return HRCODE(HRCODE::HC_FAIL);
		for ( i = 0; i < nEntityCount; i++ )
			ar >> Entities[i];
		ar >> nMasterFlag >> nABFlag;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgLogin::countObjectLength()
{
	return sizeof(nStationCount) + LOGIN_MAX_STATION_COUNT*sizeof(STATION_NO)
		+ sizeof(nEntityCount) + LOGIN_MAX_ENTITY_COUNT*sizeof(ENTITY_ID)
		+ sizeof(nMasterFlag) + sizeof(nABFlag);
}

///////////////
IMPLEMENT_CLBTH(MsgLoginReply, MSGID_MsgLoginReply)
IMPLEMENT_SERIALIZE_OPERATOR(MsgLoginReply)
MsgLoginReply::MsgLoginReply()
{

}
MsgLoginReply::~MsgLoginReply()
{

}
HRCODE MsgLoginReply::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << nReplyType;
	}
	else
	{
		ar >> nReplyType;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgLoginReply::countObjectLength()
{
	return sizeof(nReplyType);
}

///////////////
IMPLEMENT_CLBTH(MsgDataStyleReq, MSGID_MsgDataStyleReq)
IMPLEMENT_SERIALIZE_OPERATOR(MsgDataStyleReq)
MsgDataStyleReq::MsgDataStyleReq()
{
	nStatCount = 0;
	memset(nStno,0,sizeof(nStno));
	memset(nStyle,0,sizeof(nStyle));
}
MsgDataStyleReq::~MsgDataStyleReq()
{

}
HRCODE MsgDataStyleReq::doSerialize(Archive& ar)
{
	int i = 0;
	if(ar.isStoring())
	{
		ar << nStatCount;
		for(i = 0;i<nStatCount;i++)
			ar<<nStno[i]<<nStyle[i];
	}
	else
	{
		ar >> nStatCount;
		if(nStatCount>LOGIN_MAX_STATION_COUNT)
			return HRCODE(HRCODE::HC_FAIL);
		for(i = 0;i<nStatCount;i++)
			ar>>nStno[i]>>nStyle[i];
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgDataStyleReq::countObjectLength()
{
	return sizeof(nStatCount)+(sizeof(nStyle[0])+sizeof(nStno[0]))*nStatCount;
}

///////////////
IMPLEMENT_CLBTH(MsgEntityConnStatus, MSGID_MsgEntityConnStatus)
IMPLEMENT_SERIALIZE_OPERATOR(MsgEntityConnStatus)
MsgEntityConnStatus::MsgEntityConnStatus()
{

}
MsgEntityConnStatus::~MsgEntityConnStatus()
{

}
HRCODE MsgEntityConnStatus::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << s_server << s_client << board_no << nABFlag << (short)status;
	}
	else
	{
		short t;
		ar >> s_server >> s_client >> board_no >> nABFlag >> t;
		status = (ConnectStatus)t;
	}

	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgEntityConnStatus::countObjectLength()
{
	return sizeof(s_server) + sizeof(s_client) +
		sizeof(board_no) +sizeof(nABFlag) + sizeof(short);
}

///////////////
IMPLEMENT_CLBTH(MsgGpcAccessInfo, MSGID_MsgGpcAccessInfo)
IMPLEMENT_SERIALIZE_OPERATOR(MsgGpcAccessInfo)
MsgGpcAccessInfo::MsgGpcAccessInfo()
{

}
MsgGpcAccessInfo::~MsgGpcAccessInfo()
{

}
HRCODE MsgGpcAccessInfo::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		user_code[sizeof(user_code)-1]='\0';
		ar.write(user_code, sizeof(user_code));
		ar<< job_id << gpc_id << con_id << (short)step;
	}
	else
	{
		ar.read(user_code, sizeof(user_code));
		short t;
		ar>>job_id >> gpc_id >> con_id >> t;
		step=(ACCESS_STEP)t;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgGpcAccessInfo::countObjectLength()
{
	return sizeof(user_code)+sizeof(job_id)+sizeof(gpc_id) +
		sizeof(con_id) + sizeof(short);
}

///////////////
IMPLEMENT_CLBTH(MsgGpcAccessReply, MSGID_MsgGpcAccessReply)
IMPLEMENT_SERIALIZE_OPERATOR(MsgGpcAccessReply)
MsgGpcAccessReply::MsgGpcAccessReply()
{

}
MsgGpcAccessReply::~MsgGpcAccessReply()
{

}
HRCODE MsgGpcAccessReply::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		user_code[sizeof(user_code)-1]='\0';
		ar.write(user_code, sizeof(user_code));
		ar<< job_id << gpc_id << con_id;
		user_name[sizeof(user_name)-1]='\0';
		ar.write(user_name, sizeof(user_name));
		pwd[sizeof(pwd)-1]='\0';
		ar.write(pwd, sizeof(pwd));
		ar << (short)status;
		ar.write(os_identify, 4);
		ar.write(sys_version, 16);
	}
	else
	{
		ar.read(user_code, sizeof(user_code));
		ar >> job_id >> gpc_id >> con_id;
		ar.read(user_name, sizeof(user_name));
		ar.read(pwd, sizeof(pwd));
		short t;
		ar >> t;
		status=(ACCESS_STATUS)t;
		ar.read(os_identify, 4);
		ar.read(sys_version, 16);
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgGpcAccessReply::countObjectLength()
{
	return sizeof(user_code)+sizeof(job_id)+sizeof(gpc_id) +
		sizeof(con_id) + sizeof(user_name) +sizeof(pwd) +
		sizeof(short) + sizeof(os_identify) + sizeof(sys_version);
}

///////////////
IMPLEMENT_CLBTH(MsgGpcLogin, MSGID_MsgGpcLogin)
IMPLEMENT_SERIALIZE_OPERATOR(MsgGpcLogin)
MsgGpcLogin::MsgGpcLogin()
{

}
MsgGpcLogin::~MsgGpcLogin()
{

}
HRCODE MsgGpcLogin::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		if(sta_count > sizeof(sta_list)/sizeof(sta_list[0]) ||
			entity_count > sizeof(entity_list)/sizeof(entity_list[0]))
			return HRCODE(HRCODE::HC_FAIL);

		user_name[sizeof(user_name)-1]='\0';
		ar.write(user_name, sizeof(user_name));
		ar << con_id << job_id << sta_count << entity_count;
		short i;
		for(i=0; i<sta_count; i++)
			ar << sta_list[i];
		for(i=0; i<entity_count; i++)
			ar << entity_list[i] << (short)user_right[i];
	}
	else
	{
		ar.read(user_name, sizeof(user_name));
		user_name[sizeof(user_name)-1]='\0';
		ar >> con_id >> job_id >> sta_count >> entity_count;
		if(sta_count > sizeof(sta_list)/sizeof(sta_list[0]) ||
			entity_count > sizeof(entity_list)/sizeof(entity_list[0]))
			return HRCODE(HRCODE::HC_FAIL);

		short i, t;
		for(i=0; i<sta_count; i++)
			ar >> sta_list[i];
		for(i=0; i<entity_count; i++) {
			ar >> entity_list[i];
			ar >> t;
			user_right[i]=(USER_RIGHT)t;
		}
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgGpcLogin::countObjectLength()
{
	return sizeof(user_name)+sizeof(con_id)+sizeof(job_id)+
		sizeof(sta_count) + sizeof(entity_count)+
		sizeof(sta_list[0])*sta_count+
		(sizeof(entity_list[0])+sizeof(short))*entity_count;
}

///////////////
IMPLEMENT_CLBTH(MsgReportTime, MSGID_MsgReportTime)
IMPLEMENT_SERIALIZE_OPERATOR(MsgReportTime)
MsgReportTime::MsgReportTime()
{

}
MsgReportTime::~MsgReportTime()
{

}
HRCODE MsgReportTime::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		train_id[sizeof(train_id)-1]='\0';
		ar.write(train_id, sizeof(train_id));
		ar << train_index << rec_index << train_key << (short)oper << (short)run_attrib
			<< (short)train_attrib << station << from_st << to_st << entry << exit
			<< side << ltime << delay_time << con_id ;
	}
	else
	{
		ar.read(train_id, sizeof(train_id));
		train_id[sizeof(train_id)-1]='\0';
		short t1, t2, t3;
		ar >> train_index >> rec_index >> train_key >> t1 >> t2
			>> t3 >> station >> from_st >> to_st >> entry >> exit
			>> side >> ltime >> delay_time >> con_id ;
		oper = (OPER)t1;
		run_attrib = (RUN_ATTRIB) t2;
		train_attrib = (TRAIN_ATTRIB)t3;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgReportTime::countObjectLength()
{
	return sizeof(train_id) + sizeof(train_index) + sizeof(rec_index) +
		sizeof(train_key) + sizeof(short) + sizeof(short) + sizeof(short) +
		sizeof(station) + sizeof(entry) + sizeof(exit) + sizeof(side) +
		sizeof(ltime) + sizeof(delay_time) + sizeof(from_st) + sizeof(to_st) + sizeof(con_id);
}
///////////////
IMPLEMENT_CLBTH(MsgReportTrainId, MSGID_MsgReportTrainId)
IMPLEMENT_SERIALIZE_OPERATOR(MsgReportTrainId)
MsgReportTrainId::MsgReportTrainId()
{

}
MsgReportTrainId::~MsgReportTrainId()
{

}
HRCODE MsgReportTrainId::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		old_train_id[sizeof(old_train_id)-1]='\0';
		new_train_id[sizeof(new_train_id)-1]='\0';
		ar.write(old_train_id, sizeof(old_train_id));
		ar.write(new_train_id, sizeof(new_train_id));
		ar << train_key << station << (short)oper;
	}
	else
	{
		ar.read(old_train_id, sizeof(old_train_id));
		ar.read(new_train_id, sizeof(new_train_id));
		old_train_id[sizeof(old_train_id)-1]='\0';
		new_train_id[sizeof(new_train_id)-1]='\0';
		short t;
		ar >> train_key >> station >> t;
		oper=(OPER)t;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgReportTrainId::countObjectLength()
{
	return sizeof(old_train_id) + sizeof(new_train_id) +
		sizeof(train_key) + sizeof(station) + sizeof(short);
}
///////////////
IMPLEMENT_CLBTH(MsgStPlanSendOrder, MSGID_MsgStPlanSendOrder)
IMPLEMENT_SERIALIZE_OPERATOR(MsgStPlanSendOrder)
MsgStPlanSendOrder::MsgStPlanSendOrder()
{

}
MsgStPlanSendOrder::~MsgStPlanSendOrder()
{

}
HRCODE MsgStPlanSendOrder::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		if(count > sizeof(entry) / sizeof(entry[0]))
			return HRCODE(HRCODE::HC_FAIL);

		ar << con_id;
		sender_name[sizeof(sender_name)-1]='\0';
		ar.write(sender_name, sizeof(sender_name));

		ar << begin_time << end_time << send_time <<
			st << prev_sta_count << next_sta_count << 
			(short)st_echo << station << station_dmis << count;

		for(int i=0; i< count; i++)
			ar << entry[i] << exit[i] <<
				yard[i] << train_type[i];
	}
	else
	{
		ar >> con_id;
		ar.read(sender_name, sizeof(sender_name));
		sender_name[sizeof(sender_name)-1]='\0';

		short t;
		ar >> begin_time >> end_time >> send_time >>
			st >> prev_sta_count >> next_sta_count >> 
			t >> station >> station_dmis >> count;
		if(count > sizeof(entry) / sizeof(entry[0])) {
			count = 0;
			return HRCODE(HRCODE::HC_FAIL);
		}
		st_echo= (StPlanEchoMode)t;

		for(int i=0; i< count; i++)
			ar >> entry[i] >> exit[i] >>
				yard[i] >> train_type[i];
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgStPlanSendOrder::countObjectLength()
{
	return sizeof(con_id)+sizeof(sender_name)+
		sizeof(begin_time)+sizeof(end_time)+sizeof(send_time)+
		sizeof(st)+sizeof(prev_sta_count)+sizeof(next_sta_count)+
		sizeof(short)+sizeof(station) +sizeof(station_dmis)+sizeof(count)+
		count*(sizeof(entry[0])+sizeof(exit[0])+sizeof(yard[0])+
		sizeof(train_type[0]));
}
///////////////
IMPLEMENT_CLBTH(MsgStPlanStatusRequest, MSGID_MsgStPlanStatusRequest)
IMPLEMENT_SERIALIZE_OPERATOR(MsgStPlanStatusRequest)
MsgStPlanStatusRequest::MsgStPlanStatusRequest()
{

}
MsgStPlanStatusRequest::~MsgStPlanStatusRequest()
{

}
HRCODE MsgStPlanStatusRequest::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		if(st_cnt > sizeof(st) / sizeof(st[0]))
			return HRCODE(HRCODE::HC_FAIL);

		ar << station << con_id << st_cnt;
		for(int i=0; i<st_cnt; i++)
			ar << st[i];
	}
	else
	{
		ar >> station >> con_id >> st_cnt;
		if(st_cnt > sizeof(st) / sizeof(st[0])) {
			st_cnt=0;
			return HRCODE(HRCODE::HC_FAIL);
		}

		for(int i=0; i<st_cnt; i++)
			ar >> st[i];

	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgStPlanStatusRequest::countObjectLength()
{
	return sizeof(station)+sizeof(con_id)+sizeof(st_cnt)+
		st_cnt*sizeof(st[0]);
}
///////////////
IMPLEMENT_CLBTH(MsgStPlanStatusAnswer, MSGID_MsgStPlanStatusAnswer)
IMPLEMENT_SERIALIZE_OPERATOR(MsgStPlanStatusAnswer)
MsgStPlanStatusAnswer::MsgStPlanStatusAnswer()
{

}
MsgStPlanStatusAnswer::~MsgStPlanStatusAnswer()
{

}
HRCODE MsgStPlanStatusAnswer::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << con_id << station << dest <<
			(short)recv_status << begin_time <<
			end_time << send_time << recv_time;
		recver_name[sizeof(recver_name)-1]='\0';
		ar.write(recver_name, sizeof(recver_name));
		sender_name[sizeof(sender_name)-1]='\0';
		ar.write(sender_name, sizeof(sender_name));
	}
	else
	{
		short t;
		ar >> con_id >> station >> dest >>
			t >> begin_time >> end_time >>
			send_time >> recv_time;
		recv_status = (PLAN_RECV_STATUS)t;
		ar.read(recver_name, sizeof(recver_name));
		recver_name[sizeof(recver_name)-1]='\0';
		ar.read(sender_name, sizeof(sender_name));
		sender_name[sizeof(sender_name)-1]='\0';
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgStPlanStatusAnswer::countObjectLength()
{
	return sizeof(con_id)+sizeof(station)+
		sizeof(dest)+ sizeof(short) +
		sizeof(begin_time)+sizeof(end_time)+sizeof(send_time)+
		sizeof(recv_time)+sizeof(recver_name)+sizeof(sender_name);
}

///////////////
IMPLEMENT_CLBTH(MsgStPlanRequest, MSGID_MsgStPlanRequest)
IMPLEMENT_SERIALIZE_OPERATOR(MsgStPlanRequest)
MsgStPlanRequest::MsgStPlanRequest()
{

}
MsgStPlanRequest::~MsgStPlanRequest()
{

}
HRCODE MsgStPlanRequest::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << station << con_id << begin_time << end_time;
		ar << (short)mode << (short)data_type <<
			(short)request_con;
	}
	else
	{
		ar >> station >> con_id >> begin_time >> end_time;
		short t;
		ar >> t;
		mode = (StPlanRequestMode)t;

		ar >> t;
		data_type = (StPlanRequestType)t;

		ar >> t;
		request_con = (StPlanRequestConId)t;
	}

	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgStPlanRequest::countObjectLength()
{
	return sizeof(station)+sizeof(con_id)+sizeof(begin_time)+
		sizeof(end_time)+sizeof(short)+sizeof(short)+
		+sizeof(short);
}

///////////////
IMPLEMENT_CLBTH(MsgStPlanSendInit, MSGID_MsgStPlanSendInit)
IMPLEMENT_SERIALIZE_OPERATOR(MsgStPlanSendInit)
MsgStPlanSendInit::MsgStPlanSendInit()
{
	sender_name[0] = 0;
	send_echo = PLAN_TOTAL_ECHO;
}
MsgStPlanSendInit::~MsgStPlanSendInit()
{

}
HRCODE MsgStPlanSendInit::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << station << con_id << flag << gpc << send_time << (short) send_echo;
		sender_name[sizeof(sender_name)-1]='\0';
		ar.write(sender_name, sizeof(sender_name));
	}
	else
	{
		short t;
		ar >> station >> con_id >> flag >> gpc >> send_time >> t;
		ar.read(sender_name, sizeof(sender_name));
		sender_name[sizeof(sender_name)-1]='\0';
		send_echo=(StPlanEchoMode)t;
	}

	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgStPlanSendInit::countObjectLength()
{
	return sizeof(station)+sizeof(con_id)+sizeof(flag)+
		sizeof(flag)+sizeof(sender_name)+sizeof(send_time);
}

///////////////
IMPLEMENT_CLBTH(MsgStPlanInsert, MSGID_MsgStPlanInsert)
IMPLEMENT_SERIALIZE_OPERATOR(MsgStPlanInsert)
MsgStPlanInsert::MsgStPlanInsert()
{
	engine_count = 0;
	meet_count   = 0;
	prev_sta_count = 0;
	next_sta_count = 0;
	arrive_train_id[0] = 0;
	depart_train_id[0] = 0;
	notes[0] = 0;
	train_index = 0;
	rec_index = 0;
}
MsgStPlanInsert::~MsgStPlanInsert()
{

}
HRCODE MsgStPlanInsert::doSerialize(Archive& ar)
{
	short i;
	if(ar.isStoring())
	{
		if(engine_count > sizeof(engine_type)/sizeof(engine_type[0]))
			return HRCODE(HRCODE::HC_FAIL);

		if (meet_count > sizeof(meet_train_index)/sizeof(meet_train_index[0]))
			return HRCODE(HRCODE::HC_FAIL);

		ar << gpc << train_index << rec_index << station << arrive << depart <<
			plan_arrive << plan_depart << entry << exit << arrive_side << depart_side << 
			min_stop_time << con_id << prev_sta_count << next_sta_count;

		for(i=0; i<prev_sta_count; i++)
			ar << prev_sta[i] << prev_entry[i] << prev_exit[i] << prev_side[i] << prev_arrive[i] << prev_depart[i] << prev_status[i] << prev_flag[i];

		for(i=0; i<next_sta_count; i++)
			ar << next_sta[i] << next_entry[i] << next_exit[i] << next_side[i] << next_arrive[i] << next_depart[i] << next_status[i] << next_flag[i];

		arrive_train_id[sizeof(arrive_train_id)-1]='\0';
		depart_train_id[sizeof(depart_train_id)-1]='\0';
		IBYTE len=(IBYTE)strlen(arrive_train_id);
		ar << len;
		ar.write(arrive_train_id, len);
		IBYTE t=1;
		if(!strcmp(arrive_train_id, depart_train_id)) {
			t=0;
			ar << t;
		} else {
			ar << t;
			len=(IBYTE)strlen(depart_train_id);
			ar << len;
			ar.write(depart_train_id, len);
		}
		ar << engine_count;
		for(i=0; i< engine_count; i++) {
			engine_id[i][sizeof(engine_id[0])-1]='\0';
			len=(IBYTE)strlen(engine_id[i]);
			ar << len;
			ar.write(engine_id[i], len);
			ar << engine_type[i];
		}
		ar << early_depart_time << status << stop_condition <<
			run_fast_slow_time << flag << type << run_type <<
			space_rank << length_rank << super_rank <<
			military_rank << disconn_count << conn_count <<
			train_length << train_weight;
		notes[sizeof(notes)-1]='\0';
		short note_len=strlen(notes);
		ar << note_len;
		ar.write(notes, note_len);
		ar << meet_count;
		for(i=0; i<meet_count; i++)
			ar << meet_train_index[i] << meet_rec_index[i] << (short)meet_status[i];

	} else {
		ar >> gpc >> train_index >> rec_index >> station >> arrive >> depart >>
			plan_arrive >> plan_depart >> entry >> exit >> arrive_side >> depart_side >> 
			min_stop_time >> con_id >> prev_sta_count >> next_sta_count; 

		for(i=0; i<prev_sta_count; i++)
			ar >> prev_sta[i] >> prev_entry[i] >> prev_exit[i] >> prev_side[i] >> prev_arrive[i] >> prev_depart[i] >> prev_status[i] >> prev_flag[i];

		for(i=0; i<next_sta_count; i++)
			ar >> next_sta[i] >> next_entry[i] >> next_exit[i] >> next_side[i] >> next_arrive[i] >> next_depart[i] >> next_status[i] >> next_flag[i];		

		IBYTE len;
		ar >> len;
		if(len>=sizeof(arrive_train_id))
			return HRCODE(HRCODE::HC_FAIL);
		ar.read(arrive_train_id, len);
		arrive_train_id[len]='\0';
		IBYTE t=0;
		ar >> t;
		if(!t) {
			memcpy(depart_train_id, arrive_train_id, sizeof(arrive_train_id));
		}else {
			ar >> len;
			if(len>=sizeof(arrive_train_id))
				return HRCODE(HRCODE::HC_FAIL);
			ar.read(depart_train_id, len);
			depart_train_id[len]='\0';
		}
		arrive_train_id[sizeof(arrive_train_id)-1]='\0';
		depart_train_id[sizeof(depart_train_id)-1]='\0';

		ar >> engine_count;
		if(engine_count > sizeof(engine_type)/sizeof(engine_type[0])) {
			engine_count =0;
			return HRCODE(HRCODE::HC_FAIL);
		}

		for(i=0; i< engine_count; i++) {
			ar >> len;
			if(len >= sizeof(engine_id[0]))
				return HRCODE(HRCODE::HC_FAIL);

			ar.read(engine_id[i], len);
			engine_id[i][len]='\0';
			ar >> engine_type[i];
		}
		ar >> early_depart_time >> status >> stop_condition >>
			run_fast_slow_time >> flag >> type >> run_type >>
			space_rank >> length_rank >> super_rank >>
			military_rank >> disconn_count >> conn_count >>
			train_length >> train_weight;

		short note_len=0;
		ar >> note_len;
		if(note_len >=sizeof(notes) || note_len<0)
			return HRCODE(HRCODE::HC_FAIL);

		ar.read(notes, note_len);
		notes[note_len]='\0';

		ar >> meet_count;
		if (meet_count > sizeof(meet_train_index)/sizeof(meet_train_index[0]))
			return HRCODE(HRCODE::HC_FAIL);		
		for(i=0; i<meet_count; i++) {
			short t;
			ar >> meet_train_index[i] >> meet_rec_index[i] >> t;
			meet_status[i]=(MEET_STATUS)t;
		}

	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgStPlanInsert::countObjectLength()
{
	short len=sizeof(gpc)+sizeof(train_index)+sizeof(rec_index)+sizeof(station)+sizeof(arrive)+sizeof(depart)+
		sizeof(plan_arrive)+sizeof(plan_depart)+sizeof(entry)+sizeof(exit)+sizeof(arrive_side)+sizeof(depart_side)+
		sizeof(min_stop_time)+sizeof(con_id)+sizeof(prev_sta_count)+sizeof(next_sta_count);

	len+=prev_sta_count*(sizeof(prev_sta[0])+sizeof(prev_entry[0])+sizeof(prev_exit[0])+sizeof(prev_side[0])+sizeof(prev_arrive[0])+sizeof(prev_depart[0])+sizeof(prev_status[0])+sizeof(prev_flag[0]));
	len+=next_sta_count*(sizeof(next_sta[0])+sizeof(next_entry[0])+sizeof(next_exit[0])+sizeof(next_side[0])+sizeof(next_arrive[0])+sizeof(next_depart[0])+sizeof(next_status[0])+sizeof(next_flag[0]));

	arrive_train_id[sizeof(arrive_train_id)-1]='\0';
	depart_train_id[sizeof(depart_train_id)-1]='\0';
	len+=sizeof(IBYTE)+strlen(arrive_train_id);
	len+=sizeof(IBYTE);	
	if(strcmp(arrive_train_id, depart_train_id))
		len +=sizeof(IBYTE)+strlen(depart_train_id);

	len+=sizeof(engine_count);
	for(int i=0; i<engine_count; i++) {
		engine_id[i][sizeof(engine_id[0])-1]='\0';
		len+=sizeof(IBYTE)+(strlen(engine_id[i])+sizeof(engine_type[i]));
	}	

	len+=sizeof(early_depart_time)+sizeof(status)+sizeof(stop_condition)+
		sizeof(run_fast_slow_time)+sizeof(flag)+sizeof(type)+sizeof(run_type)+
		sizeof(space_rank)+sizeof(length_rank)+sizeof(super_rank)+
		sizeof(military_rank)+sizeof(disconn_count)+sizeof(conn_count)+
		sizeof(train_length)+sizeof(train_weight);

	notes[sizeof(notes)-1]='\0';
	len+=sizeof(short)+strlen(notes);
	len+=sizeof(meet_count)+meet_count*(sizeof(meet_train_index[0])+sizeof(meet_rec_index[0])+sizeof(short));
	return len;
}


///////////////
IMPLEMENT_CLBTH(MsgStPlanDelete, MSGID_MsgStPlanDelete)
IMPLEMENT_SERIALIZE_OPERATOR(MsgStPlanDelete)
MsgStPlanDelete::MsgStPlanDelete()
{
	arrive_train_id[0] = 0;
	depart_train_id[0] = 0;

	train_index = 0;
	rec_index = 0;
}
MsgStPlanDelete::~MsgStPlanDelete()
{

}
HRCODE MsgStPlanDelete::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << gpc << con_id << station << train_index <<
			rec_index << arrive << depart;

		arrive_train_id[sizeof(arrive_train_id)-1]='\0';
		depart_train_id[sizeof(depart_train_id)-1]='\0';
		IBYTE len=(IBYTE)strlen(arrive_train_id);
		ar << len;
		ar.write(arrive_train_id, len);
		IBYTE t=1;
		if(!strcmp(arrive_train_id, depart_train_id)) {
			t=0;
			ar << t;
		}else {
			ar << t;
			len=(IBYTE)strlen(depart_train_id);
			ar << len; // BUG！！！！
			ar.write(depart_train_id, len);
		}
	}
	else
	{
		ar >> gpc >> con_id >> station >> train_index >>
			rec_index >> arrive >> depart;

		IBYTE len,t;
		ar >> len;
		if(len >=sizeof(arrive_train_id))
			return HRCODE(HRCODE::HC_FAIL);

		ar.read(arrive_train_id, len);
		arrive_train_id[len]='\0';

		ar >> t;
		if(!t) {
			memcpy(depart_train_id, arrive_train_id, sizeof(arrive_train_id));
		} else {
			ar >> len;
			if(len >=sizeof(depart_train_id))
				return HRCODE(HRCODE::HC_FAIL);
			ar.read(depart_train_id, len);
			depart_train_id[len]='\0';
		}
		arrive_train_id[sizeof(arrive_train_id)-1]='\0';
		depart_train_id[sizeof(depart_train_id)-1]='\0';
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgStPlanDelete::countObjectLength()
{
	return sizeof(gpc)+sizeof(con_id)+sizeof(station)+
		sizeof(train_index)+sizeof(rec_index)+sizeof(arrive)+
		sizeof(depart)+sizeof(IBYTE)+strlen(arrive_train_id)+1+
		(strncmp(arrive_train_id, depart_train_id, sizeof(arrive_train_id))?
		strlen(depart_train_id):0);
}
///////////////
IMPLEMENT_CLBTH(MsgStPlanSendComplete, MSGID_MsgStPlanSendComplete)
IMPLEMENT_SERIALIZE_OPERATOR(MsgStPlanSendComplete)
MsgStPlanSendComplete::MsgStPlanSendComplete()
{

}
MsgStPlanSendComplete::~MsgStPlanSendComplete()
{

}
HRCODE MsgStPlanSendComplete::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << gpc << con_id << station << insert_cnt << delete_cnt;
	}
	else
	{
		ar >> gpc >> con_id >> station >> insert_cnt >> delete_cnt;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgStPlanSendComplete::countObjectLength()
{
	return sizeof(gpc)+sizeof(con_id)+sizeof(station)+
		sizeof(insert_cnt)+sizeof(delete_cnt);
}

///////////////
IMPLEMENT_CLBTH(MsgStPlanTrainRecv, MSGID_MsgStPlanTrainRecv)
IMPLEMENT_SERIALIZE_OPERATOR(MsgStPlanTrainRecv)
MsgStPlanTrainRecv::MsgStPlanTrainRecv()
{

}
MsgStPlanTrainRecv::~MsgStPlanTrainRecv()
{

}
HRCODE MsgStPlanTrainRecv::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << station << con_id << train_index << rec_index << (short)attrib;
	}
	else
	{
		short t;
		ar >> station >> con_id >> train_index >> rec_index >> t;
		attrib=(RUN_ATTRIB)t;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgStPlanTrainRecv::countObjectLength()
{
	return sizeof(station)+sizeof(con_id)+sizeof(train_index)+
		sizeof(rec_index)+sizeof(short);
}

///////////////
IMPLEMENT_CLBTH(MsgStPlanRecvALL, MSGID_MsgStPlanRecvALL)
IMPLEMENT_SERIALIZE_OPERATOR(MsgStPlanRecvALL)
MsgStPlanRecvALL::MsgStPlanRecvALL()
{

}
MsgStPlanRecvALL::~MsgStPlanRecvALL()
{

}
HRCODE MsgStPlanRecvALL::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << gpc << st << station << con_id <<
			(short) status << send_time << recv_time;
		recver_name[sizeof(recver_name)-1]='\0';
		ar.write(recver_name, sizeof(recver_name));
	}
	else
	{
		short t;
		ar >> gpc >> st >> station >> con_id >>
			t >> send_time >> recv_time;
		status=(RECV_STATUS)t;
		ar.read(recver_name, sizeof(recver_name));
		recver_name[sizeof(recver_name)-1]='\0';
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgStPlanRecvALL::countObjectLength()
{
	return sizeof(gpc)+sizeof(st)+sizeof(station)+sizeof(con_id)+
		sizeof(short)+sizeof(send_time)+sizeof(recv_time)+sizeof(recver_name);
}

// 列车基本图下达
IMPLEMENT_CLBTH(MsgBaseTrainPlan, MSGID_MsgBaseTrainPlan)
IMPLEMENT_SERIALIZE_OPERATOR(MsgBaseTrainPlan)
MsgBaseTrainPlan::MsgBaseTrainPlan() :
    station(INVALID_STATION_NO), plan_count(0)
{
}
MsgBaseTrainPlan::~MsgBaseTrainPlan()
{
}
HRCODE MsgBaseTrainPlan::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << station << plan_count;
        if (plan_count > MAX_PLAN_COUNT)
            return HRCODE(HRCODE::HC_FAIL);
        for (IBYTE i = 0; i < plan_count; i++)
        {
            IBYTE str_len = 0;
            AR_WRITE_STRING(base_plan_record[i].arrive_train, str_len);
            AR_WRITE_STRING(base_plan_record[i].depart_train, str_len);
            ar << base_plan_record[i].side << base_plan_record[i].entry << base_plan_record[i].exit
                << base_plan_record[i].prev_station << base_plan_record[i].next_station 
                << base_plan_record[i].arrive_time << base_plan_record[i].depart_time;
			for(IBYTE n=0; n<8; n++)
			{
				ar << base_plan_record[i].reserve[n];
			}
        }
	}
	else
	{
		ar >> station >> plan_count;
        if (plan_count > MAX_PLAN_COUNT)
            return HRCODE(HRCODE::HC_FAIL);
        for (IBYTE i = 0; i < plan_count; i++)
        {
            IBYTE str_len = 0;
            AR_READ_STRING(base_plan_record[i].arrive_train, str_len);
            AR_READ_STRING(base_plan_record[i].depart_train, str_len);
            ar >> base_plan_record[i].side >> base_plan_record[i].entry >> base_plan_record[i].exit
                >> base_plan_record[i].prev_station >> base_plan_record[i].next_station 
                >> base_plan_record[i].arrive_time >> base_plan_record[i].depart_time;
			for(IBYTE n=0; n<8; n++)
			{
				ar >> base_plan_record[i].reserve[n];
			}
        }
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgBaseTrainPlan::countObjectLength()
{
    return sizeof(station) + sizeof(plan_count) + plan_count*sizeof(base_plan);
}

///////////////
IMPLEMENT_CLBTH(MsgShuntNoteInit, MSGID_MsgShuntNoteInit)
IMPLEMENT_SERIALIZE_OPERATOR(MsgShuntNoteInit)
MsgShuntNoteInit::MsgShuntNoteInit()
{

}
MsgShuntNoteInit::~MsgShuntNoteInit()
{

}
HRCODE MsgShuntNoteInit::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << (IBYTE)nSourceType << source_entity << shunt_index << station <<
			send_time;
		sender_name[sizeof(sender_name)-1]='\0';
		IBYTE len = (IBYTE)strlen(sender_name);
		ar << len;
		ar.write(sender_name, len);
	}
	else
	{
		IBYTE nTemp;
		IBYTE len;
		ar >> nTemp >> source_entity >> shunt_index >> station >>
			send_time >> len;
		nSourceType= (SOURCE_TYPE )nTemp;

		if(len > sizeof(sender_name)-1)
			return HRCODE(HRCODE::HC_FAIL);

		ar.read(sender_name, len);
		sender_name[len]='\0';
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgShuntNoteInit::countObjectLength()
{
	sender_name[sizeof(sender_name)-1]='\0';
	return sizeof(source_entity)+sizeof(shunt_index)+sizeof(station)+
		sizeof(send_time)+sizeof(IBYTE)+strlen(sender_name) + sizeof(IBYTE);
} 

///////////////
IMPLEMENT_CLBTH(MsgShuntNoteSingleGouInit, MSGID_MsgShuntNoteSingleGouInit)
IMPLEMENT_SERIALIZE_OPERATOR(MsgShuntNoteSingleGouInit)
MsgShuntNoteSingleGouInit::MsgShuntNoteSingleGouInit()
{

}
MsgShuntNoteSingleGouInit::~MsgShuntNoteSingleGouInit()
{

}
HRCODE MsgShuntNoteSingleGouInit::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << (IBYTE)nSourceType << source_entity << shunt_index << shunt_gou_index << station <<
			send_time;
		sender_name[sizeof(sender_name)-1]='\0';
		IBYTE len = (IBYTE)strlen(sender_name);
		ar << len;
		ar.write(sender_name, len);
	}
	else
	{
		IBYTE nTemp;
		IBYTE len;
		ar >> nTemp >> source_entity >> shunt_index >> shunt_gou_index >> station >>
			send_time >> len;
		nSourceType= (SOURCE_TYPE )nTemp;

		if(len > sizeof(sender_name)-1)
			return HRCODE(HRCODE::HC_FAIL);

		ar.read(sender_name, len);
		sender_name[len]='\0';
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgShuntNoteSingleGouInit::countObjectLength()
{
	sender_name[sizeof(sender_name)-1]='\0';
	return sizeof(source_entity)+sizeof(shunt_index)+sizeof(shunt_gou_index)+sizeof(station)+
		sizeof(send_time)+sizeof(IBYTE)+strlen(sender_name) + sizeof(IBYTE);
} 

///////////////
IMPLEMENT_CLBTH(MsgShuntNoteComplete, MSGID_MsgShuntNoteComplete)
IMPLEMENT_SERIALIZE_OPERATOR(MsgShuntNoteComplete)
MsgShuntNoteComplete::MsgShuntNoteComplete()
{

}
MsgShuntNoteComplete::~MsgShuntNoteComplete()
{

}
HRCODE MsgShuntNoteComplete::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << (IBYTE)nSourceType << source_entity << shunt_index << station << send_time;
		sender_name[sizeof(sender_name)-1]='\0';
		IBYTE len = (IBYTE)strlen(sender_name);
		ar << len;
		ar.write(sender_name, len);
	}
	else
	{
		IBYTE nTemp;
		IBYTE len;
		ar  >> nTemp >> source_entity >> shunt_index >> station >>
			send_time >> len;
		nSourceType= (SOURCE_TYPE )nTemp;
		if(len > sizeof(sender_name)-1)
			return HRCODE(HRCODE::HC_FAIL);

		ar.read(sender_name, len);
		sender_name[len]='\0';
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgShuntNoteComplete::countObjectLength()
{
	sender_name[sizeof(sender_name)-1]='\0';
	return sizeof(source_entity)+sizeof(shunt_index)+sizeof(station)+
		sizeof(send_time)+sizeof(IBYTE)+strlen(sender_name) + sizeof(IBYTE);
}

///////////////
IMPLEMENT_CLBTH(MsgShuntNoteSingleGouComplete, MSGID_MsgShuntNoteSingleGouComplete)
IMPLEMENT_SERIALIZE_OPERATOR(MsgShuntNoteSingleGouComplete)
MsgShuntNoteSingleGouComplete::MsgShuntNoteSingleGouComplete()
{

}
MsgShuntNoteSingleGouComplete::~MsgShuntNoteSingleGouComplete()
{

}
HRCODE MsgShuntNoteSingleGouComplete::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << (IBYTE)nSourceType << source_entity << shunt_index << shunt_gou_index << station << send_time;
		sender_name[sizeof(sender_name)-1]='\0';
		IBYTE len = (IBYTE)strlen(sender_name);
		ar << len;
		ar.write(sender_name, len);
	}
	else
	{
		IBYTE nTemp;
		IBYTE len;
		ar  >> nTemp >> source_entity >> shunt_index >> shunt_gou_index >> station >>
			send_time >> len;
		nSourceType= (SOURCE_TYPE )nTemp;
		if(len > sizeof(sender_name)-1)
			return HRCODE(HRCODE::HC_FAIL);

		ar.read(sender_name, len);
		sender_name[len]='\0';
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgShuntNoteSingleGouComplete::countObjectLength()
{
	sender_name[sizeof(sender_name)-1]='\0';
	return sizeof(source_entity)+sizeof(shunt_index)+sizeof(shunt_gou_index)+sizeof(station)+
		sizeof(send_time)+sizeof(IBYTE)+strlen(sender_name) + sizeof(IBYTE);
}

///////////////
IMPLEMENT_CLBTH(MsgShuntNoteGpcOperEcho, MSGID_MsgShuntNoteGpcOperEcho)
IMPLEMENT_SERIALIZE_OPERATOR(MsgShuntNoteGpcOperEcho)
MsgShuntNoteGpcOperEcho::MsgShuntNoteGpcOperEcho()
{

}
MsgShuntNoteGpcOperEcho::~MsgShuntNoteGpcOperEcho()
{

}
HRCODE MsgShuntNoteGpcOperEcho::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << shunt_index << station << (short)oper << (short)echo;
	}
	else
	{
		short t;
		ar >> shunt_index >> station >> t;
		oper = (OPER_TYPE)t;
		ar >> t;
		echo = (OPER_ECHO)t;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgShuntNoteGpcOperEcho::countObjectLength()
{
	return sizeof(shunt_index)+sizeof(station)+
		sizeof(short)+sizeof(short);
}

///////////////
IMPLEMENT_CLBTH(MsgShuntNoteSendOrder, MSGID_MsgShuntNoteSendOrder)
IMPLEMENT_SERIALIZE_OPERATOR(MsgShuntNoteSendOrder)
MsgShuntNoteSendOrder::MsgShuntNoteSendOrder()
{

}
MsgShuntNoteSendOrder::~MsgShuntNoteSendOrder()
{

}
HRCODE MsgShuntNoteSendOrder::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		if(entity_cnt > MAX_SHUNTNOTE_DSTN_CNT)
			return HRCODE(HRCODE::HC_FAIL);

		ar <<  shunt_index << station << send_time << entity_cnt;
		sender_name[sizeof(sender_name)-1]='\0';
		ar.write(sender_name, sizeof(sender_name));
		for(int i=0; i<entity_cnt; i++)
			ar << st_entity[i];
	}
	else
	{
		ar >>  shunt_index >> station >> send_time >> entity_cnt;
		ar.read(sender_name, sizeof(sender_name));
		sender_name[sizeof(sender_name)-1]='\0';
		if(entity_cnt > MAX_SHUNTNOTE_DSTN_CNT) {
			entity_cnt = 0;
			return HRCODE(HRCODE::HC_FAIL);
		}

		for(int i=0; i<entity_cnt; i++)
			ar >> st_entity[i];

	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgShuntNoteSendOrder::countObjectLength()
{
	return sizeof(shunt_index)+sizeof(send_time)+
		sizeof(entity_cnt)+sizeof(sender_name)+
		entity_cnt*sizeof(st_entity[0]);
}

///////////////
IMPLEMENT_CLBTH(MsgShuntNoteRecvALL, MSGID_MsgShuntNoteRecvALL)
IMPLEMENT_SERIALIZE_OPERATOR(MsgShuntNoteRecvALL)
MsgShuntNoteRecvALL::MsgShuntNoteRecvALL()
{

}
MsgShuntNoteRecvALL::~MsgShuntNoteRecvALL()
{

}
HRCODE MsgShuntNoteRecvALL::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << shunt_index << (short)status << send_time << recv_time <<
			gpc << st << station;

		sender_name[sizeof(sender_name)-1]='\0';
		IBYTE len=(IBYTE)strlen(sender_name);
		ar << len;
		ar.write(sender_name, len);

		recver_name[sizeof(recver_name)-1]='\0';
		len=(IBYTE)strlen(recver_name);
		ar << len;
		ar.write(recver_name, len);
	}
	else
	{
		short t;
		ar >> shunt_index >> t >> send_time >> recv_time >>
			gpc >> st >> station;
		status = (SHUNTNOTE_SEND_STATUS)t;

		IBYTE len;
		ar >> len;
		if(len >sizeof(sender_name)-1)
			return HRCODE(HRCODE::HC_FAIL);
		ar.read(sender_name, len);

		ar >> len;
		if(len >sizeof(recver_name)-1)
			return HRCODE(HRCODE::HC_FAIL);
		ar.read(recver_name, len);

		sender_name[len]='\0';
		recver_name[len]='\0';
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgShuntNoteRecvALL::countObjectLength()
{
	return sizeof(shunt_index)+sizeof(short)+
		sizeof(send_time)+sizeof(recv_time)+sizeof(gpc)+
		sizeof(st)+sizeof(station)+sizeof(IBYTE)
		+strlen(sender_name)+sizeof(IBYTE)
		+strlen(recver_name);
}

///////////////
IMPLEMENT_CLBTH(MsgShuntNoteSingleGouRecvALL, MSGID_MsgShuntNoteSingleGouRecvALL)
IMPLEMENT_SERIALIZE_OPERATOR(MsgShuntNoteSingleGouRecvALL)
MsgShuntNoteSingleGouRecvALL::MsgShuntNoteSingleGouRecvALL()
{

}
MsgShuntNoteSingleGouRecvALL::~MsgShuntNoteSingleGouRecvALL()
{

}
HRCODE MsgShuntNoteSingleGouRecvALL::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << shunt_index << shunt_gou_index << (short)status << send_time << recv_time <<
			gpc << st << station;

		sender_name[sizeof(sender_name)-1]='\0';
		IBYTE len=(IBYTE)strlen(sender_name);
		ar << len;
		ar.write(sender_name, len);

		recver_name[sizeof(recver_name)-1]='\0';
		len=(IBYTE)strlen(recver_name);
		ar << len;
		ar.write(recver_name, len);
	}
	else
	{
		short t;
		ar >> shunt_index >> shunt_gou_index >> t >> send_time >> recv_time >>
			gpc >> st >> station;
		status = (GOU_SEND_STATUS)t;

		IBYTE len;
		ar >> len;
		if(len >sizeof(sender_name)-1)
			return HRCODE(HRCODE::HC_FAIL);
		ar.read(sender_name, len);

		ar >> len;
		if(len >sizeof(recver_name)-1)
			return HRCODE(HRCODE::HC_FAIL);
		ar.read(recver_name, len);

		sender_name[len]='\0';
		recver_name[len]='\0';
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgShuntNoteSingleGouRecvALL::countObjectLength()
{
	return sizeof(shunt_index)+sizeof(shunt_gou_index)+sizeof(short)+
		sizeof(send_time)+sizeof(recv_time)+sizeof(gpc)+
		sizeof(st)+sizeof(station)+sizeof(IBYTE)
		+strlen(sender_name)+sizeof(IBYTE)
		+strlen(recver_name);
}

///////////////
IMPLEMENT_CLBTH(MsgShuntNoteDelete, MSGID_MsgShuntNoteDelete)
IMPLEMENT_SERIALIZE_OPERATOR(MsgShuntNoteDelete)
MsgShuntNoteDelete::MsgShuntNoteDelete()
{

}
MsgShuntNoteDelete::~MsgShuntNoteDelete()
{

}
HRCODE MsgShuntNoteDelete::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << shunt_index << station << (short)delete_items;
	}
	else
	{
		short t;
		ar >> shunt_index >> station >> t;
		delete_items = (DELETE_ITEMS)t;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgShuntNoteDelete::countObjectLength()
{
	return sizeof(shunt_index)+sizeof(station)+sizeof(short);
}

///////////////
IMPLEMENT_CLBTH(MsgShuntNoteInsert, MSGID_MsgShuntNoteInsert)
IMPLEMENT_SERIALIZE_OPERATOR(MsgShuntNoteInsert)
MsgShuntNoteInsert::MsgShuntNoteInsert()
{

}
MsgShuntNoteInsert::~MsgShuntNoteInsert()
{

}
HRCODE MsgShuntNoteInsert::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << shunt_index << station;
	}
	else
	{
		ar >> shunt_index >> station;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgShuntNoteInsert::countObjectLength()
{
	return sizeof(shunt_index)+sizeof(station);
}

///////////////
IMPLEMENT_CLBTH(MsgShuntNoteUpdate, MSGID_MsgShuntNoteUpdate)
IMPLEMENT_SERIALIZE_OPERATOR(MsgShuntNoteUpdate)
MsgShuntNoteUpdate::MsgShuntNoteUpdate()
{

}
MsgShuntNoteUpdate::~MsgShuntNoteUpdate()
{

}
HRCODE MsgShuntNoteUpdate::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << shunt_index << station << (short)update_items;
	}
	else
	{
		short t;
		ar >> shunt_index >> station >> t;
		update_items=(UPDATE_ITEMS)t;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgShuntNoteUpdate::countObjectLength()
{
	return sizeof(shunt_index)+sizeof(station)+sizeof(short);
}

///////////////
IMPLEMENT_CLBTH(MsgShuntNoteListRequest, MSGID_MsgShuntNoteListRequest)
IMPLEMENT_SERIALIZE_OPERATOR(MsgShuntNoteListRequest)
MsgShuntNoteListRequest::MsgShuntNoteListRequest()
{

}
MsgShuntNoteListRequest::~MsgShuntNoteListRequest()
{

}
HRCODE MsgShuntNoteListRequest::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << station << begin_time << end_time;
	}
	else
	{
		ar >> station >> begin_time >> end_time;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgShuntNoteListRequest::countObjectLength()
{
	return sizeof(station)+sizeof(begin_time)+sizeof(end_time);
}

///////////////
IMPLEMENT_CLBTH(MsgShuntNoteListInit, MSGID_MsgShuntNoteListInit)
IMPLEMENT_SERIALIZE_OPERATOR(MsgShuntNoteListInit)
MsgShuntNoteListInit::MsgShuntNoteListInit()
{

}
MsgShuntNoteListInit::~MsgShuntNoteListInit()
{

}
HRCODE MsgShuntNoteListInit::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << (IBYTE)nSourceType << station;
	}
	else
	{
		IBYTE nTemp;
		ar >> nTemp >> station;
		nSourceType= (SOURCE_TYPE )nTemp;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgShuntNoteListInit::countObjectLength()
{
	return sizeof(station) + sizeof(IBYTE);
}

///////////////
IMPLEMENT_CLBTH(MsgShuntNoteListComplete, MSGID_MsgShuntNoteListComplete)
IMPLEMENT_SERIALIZE_OPERATOR(MsgShuntNoteListComplete)
MsgShuntNoteListComplete::MsgShuntNoteListComplete()
{

}
MsgShuntNoteListComplete::~MsgShuntNoteListComplete()
{

}
HRCODE MsgShuntNoteListComplete::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << (IBYTE)nSourceType << station << count;
	}
	else
	{
		IBYTE nTemp;
		ar  >> nTemp >> station >> count;
		nSourceType= (SOURCE_TYPE )nTemp;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgShuntNoteListComplete::countObjectLength()
{
	return sizeof(station)+sizeof(count) + sizeof(IBYTE);
}

///////////////
IMPLEMENT_CLBTH(MsgShuntNoteRequest, MSGID_MsgShuntNoteRequest)
IMPLEMENT_SERIALIZE_OPERATOR(MsgShuntNoteRequest)
MsgShuntNoteRequest::MsgShuntNoteRequest()
{

}
MsgShuntNoteRequest::~MsgShuntNoteRequest()
{

}
HRCODE MsgShuntNoteRequest::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << station << shunt_index;
	}
	else
	{
		ar >> station >> shunt_index;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgShuntNoteRequest::countObjectLength()
{
	return sizeof(station)+sizeof(shunt_index);
}

///////////////
IMPLEMENT_CLBTH(MsgShuntNoteStatus, MSGID_MsgShuntNoteStatus)
IMPLEMENT_SERIALIZE_OPERATOR(MsgShuntNoteStatus)
MsgShuntNoteStatus::MsgShuntNoteStatus()
{

}
MsgShuntNoteStatus::~MsgShuntNoteStatus()
{

}
HRCODE MsgShuntNoteStatus::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << shunt_index << station << st_entity << (short)status <<
			send_time << recv_time;

		sender_name[sizeof(sender_name)-1]='\0';
		recver_name[sizeof(recver_name)-1]='\0';
		IBYTE len=(IBYTE)strlen(sender_name);
		ar << len;
		ar.write(sender_name, len);
		len=(IBYTE)strlen(recver_name);
		ar << len;
		ar.write(recver_name, len);
	}
	else
	{
		short t;
		ar >>  shunt_index >> station >> st_entity >> t >>
			send_time >> recv_time;
		status=(SHUNTNOTE_SEND_STATUS)t;
		IBYTE len;
		ar >> len;
		if(len > sizeof(sender_name)-1)
			return HRCODE(HRCODE::HC_FAIL);

		ar.read(sender_name, len);
		sender_name[len]='\0';
		ar >> len;
		if(len > sizeof(recver_name)-1)
			return HRCODE(HRCODE::HC_FAIL);
		ar.read(recver_name, len);
		recver_name[len]='\0';
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgShuntNoteStatus::countObjectLength()
{
	sender_name[sizeof(sender_name)-1]='\0';
	recver_name[sizeof(recver_name)-1]='\0';
	return sizeof(shunt_index)+sizeof(station)+sizeof(st_entity)+
		sizeof(short)+sizeof(send_time)+sizeof(recv_time)+
		sizeof(IBYTE)+strlen(sender_name)+sizeof(IBYTE)+
		strlen(recver_name);
}

///////////////
IMPLEMENT_CLBTH(MsgShuntNoteHead, MSGID_MsgShuntNoteHead)
IMPLEMENT_SERIALIZE_OPERATOR(MsgShuntNoteHead)
MsgShuntNoteHead::MsgShuntNoteHead()
{

}
MsgShuntNoteHead::~MsgShuntNoteHead()
{

}
HRCODE MsgShuntNoteHead::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		IBYTE len=0;
		ar << (IBYTE)nSourceType << shunt_index << station << shunt_gou_count <<
			flag << start_time << end_time <<
			train_index << edit_time << (IBYTE)shunt_status << (IBYTE)oper_status;

		train_id[sizeof(train_id)-1]='\0';
		len=(IBYTE)strlen(train_id);
		ar << len;
		ar.write(train_id, len);

		engine_id[sizeof(engine_id)-1]='\0';
		len=(IBYTE)strlen(engine_id);
		ar << len;
		ar.write(engine_id, len);

		shunt_group_name[sizeof(shunt_group_name)-1]='\0';
		len=(IBYTE)strlen(shunt_group_name);
		ar << len;
		ar.write(shunt_group_name, len);

		shunt_leader_name[sizeof(shunt_leader_name)-1]='\0';
		len=(IBYTE)strlen(shunt_leader_name);
		ar << len;
		ar.write(shunt_leader_name, len);

		shunt_note_name[sizeof(shunt_note_name)-1]='\0';
		len=(IBYTE)strlen(shunt_note_name);
		ar << len;
		ar.write(shunt_note_name, len);

		shunt_comment[sizeof(shunt_comment)-1]='\0';
		len=(IBYTE)strlen(shunt_comment);
		ar << len;
		ar.write(shunt_comment, len);
	}
	else
	{
		IBYTE nTemp;
		IBYTE nShuntStatus;
		IBYTE nOperStatus;
		ar  >> nTemp >> shunt_index >> station >> shunt_gou_count >>
			flag >> start_time >> end_time >>
			train_index >> edit_time >> nShuntStatus >> nOperStatus;
		nSourceType= (SOURCE_TYPE )nTemp;
		shunt_status=(SHUNTNOTE_STATUS)nShuntStatus;
		oper_status = (OPER_STATUS)nOperStatus;
		IBYTE len;

		ar >> len;
		if(len > sizeof(train_id)-1)
			return HRCODE(HRCODE::HC_FAIL);
		ar.read(train_id, len);
		train_id[len]='\0';

		ar >> len;
		if(len > sizeof(engine_id)-1)
			return HRCODE(HRCODE::HC_FAIL);
		ar.read(engine_id, len);
		engine_id[len]='\0';

		ar >> len;
		if(len > sizeof(shunt_group_name)-1)
			return HRCODE(HRCODE::HC_FAIL);
		ar.read(shunt_group_name, len);
		shunt_group_name[len]='\0';

		ar >> len;
		if(len > sizeof(shunt_leader_name)-1)
			return HRCODE(HRCODE::HC_FAIL);
		ar.read(shunt_leader_name, len);
		shunt_leader_name[len]='\0';

		ar >> len;
		if(len > sizeof(shunt_note_name)-1)
			return HRCODE(HRCODE::HC_FAIL);
		ar.read(shunt_note_name, len);
		shunt_note_name[len]='\0';

		ar >> len;
		if(len > sizeof(shunt_comment)-1)
			return HRCODE(HRCODE::HC_FAIL);
		ar.read(shunt_comment, len);
		shunt_comment[len]='\0';
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgShuntNoteHead::countObjectLength()
{
	train_id[sizeof(train_id)-1]='\0';
	engine_id[sizeof(engine_id)-1]='\0';
	shunt_group_name[sizeof(shunt_group_name)-1]='\0';
	shunt_leader_name[sizeof(shunt_leader_name)-1]='\0';
	shunt_note_name[sizeof(shunt_note_name)-1]='\0';
	shunt_comment[sizeof(shunt_comment)-1]='\0';
	return sizeof(shunt_index)+sizeof(station)+sizeof(shunt_gou_count)+
		sizeof(flag)+sizeof(start_time)+sizeof(end_time)+
		sizeof(train_index)+sizeof(edit_time)+sizeof(short)+
		sizeof(IBYTE)+strlen(train_id)+
		sizeof(IBYTE)+strlen(engine_id)+
		sizeof(IBYTE)+strlen(shunt_group_name)+
		sizeof(IBYTE)+strlen(shunt_leader_name)+
		sizeof(IBYTE)+strlen(shunt_note_name)+
		sizeof(IBYTE)+strlen(shunt_comment) +
		sizeof(IBYTE);
}

///////////////
IMPLEMENT_CLBTH(MsgShuntNoteGou, MSGID_MsgShuntNoteGou)
IMPLEMENT_SERIALIZE_OPERATOR(MsgShuntNoteGou)
MsgShuntNoteGou::MsgShuntNoteGou()
{

}
MsgShuntNoteGou::~MsgShuntNoteGou()
{

}
HRCODE MsgShuntNoteGou::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << (IBYTE)nSourceType << shunt_index << station << shunt_gou_index <<
			shunt_gou_type << side << direction <<
			shunt_car_count << shunt_note_route_count <<
			shunt_work_time;

		gou_comment[sizeof(gou_comment)-1]='\0';
		IBYTE len=(IBYTE)strlen(gou_comment);
		ar << len;
		ar.write(gou_comment, len);

		szTrackQCX[sizeof(szTrackQCX)-1] = '\0';
		len = (IBYTE) strlen(szTrackQCX);
		ar<< len;
		ar.write(szTrackQCX, len);
	}
	else
	{
		IBYTE nTemp;
		ar  >> nTemp >> shunt_index >> station >> shunt_gou_index >>
			shunt_gou_type >> side >> direction >>
			shunt_car_count >> shunt_note_route_count >>
			shunt_work_time;
		nSourceType= (SOURCE_TYPE )nTemp;

		IBYTE len=0;
		ar >> len;
		if(len > sizeof(gou_comment)-1)
			return HRCODE(HRCODE::HC_FAIL);

		ar.read(gou_comment, len);
		gou_comment[len]='\0';

		ar >> len;
		if(len > sizeof(szTrackQCX)-1)
			return HRCODE(HRCODE::HC_FAIL);

		ar.read(szTrackQCX, len);
		szTrackQCX[len] = '\0';
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgShuntNoteGou::countObjectLength()
{
	gou_comment[sizeof(gou_comment)-1]='\0';
	szTrackQCX[sizeof(szTrackQCX)-1] = '\0';	
	return sizeof(shunt_index)+sizeof(station)+sizeof(shunt_gou_index)+
		sizeof(shunt_gou_type)+sizeof(side)+sizeof(direction)+
		sizeof(shunt_car_count)+sizeof(shunt_note_route_count)+
		sizeof(shunt_work_time)+sizeof(IBYTE)+strlen(gou_comment)+sizeof(IBYTE)+
		sizeof(IBYTE)+strlen(szTrackQCX);
}

///////////////
IMPLEMENT_CLBTH(MsgShuntNoteRouteList, MSGID_MsgShuntNoteRouteList)
IMPLEMENT_SERIALIZE_OPERATOR(MsgShuntNoteRouteList)
MsgShuntNoteRouteList::MsgShuntNoteRouteList()
{

}
MsgShuntNoteRouteList::~MsgShuntNoteRouteList()
{

}
HRCODE MsgShuntNoteRouteList::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar <<  shunt_index << station << shunt_gou_index <<
			shunt_route_index << route_id <<
			shunt_route_occ_time << flag << start_time << group_id;
	}
	else
	{
		ar >>  shunt_index >> station >> shunt_gou_index >>
			shunt_route_index >> route_id >>
			shunt_route_occ_time >> flag >> start_time >> group_id;

	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgShuntNoteRouteList::countObjectLength()
{
	return sizeof(shunt_index)+sizeof(station)+sizeof(shunt_gou_index)+
		sizeof(shunt_route_index)+sizeof(route_id)+
		sizeof(shunt_route_occ_time)+sizeof(flag)+sizeof(start_time) + sizeof(group_id);
}

///////////////
IMPLEMENT_CLBTH(MsgShuntNoteStoreCar, MSGID_MsgShuntNoteStoreCar)
IMPLEMENT_SERIALIZE_OPERATOR(MsgShuntNoteStoreCar)
MsgShuntNoteStoreCar::MsgShuntNoteStoreCar()
{

}
MsgShuntNoteStoreCar::~MsgShuntNoteStoreCar()
{

}
HRCODE MsgShuntNoteStoreCar::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		if(count > sizeof(len)/sizeof(len[0]))
			return HRCODE(HRCODE::HC_FAIL);

		ar << shunt_index << station << count;
		for(int i=0; i< count; i++) {
			if(len[i] > sizeof(info)/sizeof(info[0]))
				return HRCODE(HRCODE::HC_FAIL);

			ar << len[i];
			ar.write(info[i], len[i]);
		}
	}
	else
	{
		ar >> shunt_index >> station >> count;
		if(count > sizeof(len)/sizeof(len[0]))
			return HRCODE(HRCODE::HC_FAIL);

		for(int i=0; i< count; i++) {
			ar >> len[i];
			if(len[i] > sizeof(info)/sizeof(info[0]))
				return HRCODE(HRCODE::HC_FAIL);

			ar.read(info[i], len[i]);
			info[i][len[i]]='\0';
		}
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgShuntNoteStoreCar::countObjectLength()
{
	size_t length=sizeof(shunt_index)+sizeof(station)+sizeof(count);
	for(int i=0; i<count; i++) {
		length+=(sizeof(len[i])+len[i]);
	}

	return length;
}

///////////////
IMPLEMENT_CLBTH(MsgShuntNotePicture, MSGID_MsgShuntNotePicture)
IMPLEMENT_SERIALIZE_OPERATOR(MsgShuntNotePicture)
MsgShuntNotePicture::MsgShuntNotePicture()
{

}
MsgShuntNotePicture::~MsgShuntNotePicture()
{

}
HRCODE MsgShuntNotePicture::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		if(pic_len >sizeof(pic))
			return HRCODE(HRCODE::HC_FAIL);

		ar << shunt_index << station << pic_len << index << count;
		ar.write(pic, pic_len);
	}
	else
	{
		ar >> shunt_index >> station >> pic_len >> index >> count;
		if(pic_len >sizeof(pic))
			return HRCODE(HRCODE::HC_FAIL);

		ar.read(pic, pic_len);
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgShuntNotePicture::countObjectLength()
{
	return sizeof(shunt_index)+sizeof(station)+sizeof(pic_len)+
		sizeof(index)+sizeof(count)+pic_len;
}

///////////////
IMPLEMENT_CLBTH(MsgSubaoFormat, MSGID_MsgSubaoFormat)
IMPLEMENT_SERIALIZE_OPERATOR(MsgSubaoFormat)
MsgSubaoFormat::MsgSubaoFormat()
{
	init();
}
MsgSubaoFormat::~MsgSubaoFormat()
{

}
void MsgSubaoFormat::init()
{
	CLBTH_ZERO_INIT_OBJECT(BaseMsg)
		
	train_index = old_train_index = (TRAIN_INDEX)-1;
	train_id[0]=0;
	old_train_id[0]=0;	

	report_sta = (STATION_NO)-1;
	con_id = (CONID)-1;
	schd_type = (CONID)-1;
	car_des[0]=0;
	orig[0]=0;
	dest[0]=0;
	engine_count=0;
	for(int i=0; i<sizeof(subao_engine)/sizeof(subao_engine[0]); i++) 
	{
		subao_engine[i].engine_id[0]=0;
		subao_engine[i].engine_type=0;
	}
	driver_name[0]=0;
	train_leader_name[0]=0;
	cover[0]=0;
	tail_no[0]=0;

	space_rank=0xFF;
	length_rank=0xFF;
	super_rank=0xFF;
	military_rank=0xFF;
	speed_limit=0xFF;
	e_engine=0xFF;
	print_flag=0xFF;

	pass_cnt=-1;
	load_cnt=-1;
	pk_count=-1;
	pz_count=-1;
	ck_count=-1;
	cz_count=-1;
	pik_count=-1;
	piz_count=-1;
	gk_count=-1;
	gz_count=-1;
	o_count=-1;
	s_count=-1;
	total_count=-1;
	van_count=-1;
	cal_len=-1;
	total_weight=-1;
	subao_type=-1;
	speed=-1;
	noutil_cnt=-1;	
}
HRCODE MsgSubaoFormat::doSerialize(Archive& ar)
{
	IBYTE len;
	if(ar.isStoring())
	{
		ar << (short)oper << input_time << shift_id << train_index ;
		train_id[sizeof(train_id)-1]=0;
		len=(IBYTE)strlen(train_id);
		ar << len;
		ar.write(train_id, len);
		if(oper == SUBAO_GPC_CHANGE_KEY) 
		{
			ar << old_shift_id << old_train_index;
			old_train_id[sizeof(old_train_id)-1]=0;
			len=(IBYTE)strlen(old_train_id);
			ar << len;
			ar.write(old_train_id, len);			
		}
		ar << report_sta << con_id << schd_type;

		car_des[sizeof(car_des)-1]=0;
		len=(IBYTE)strlen(car_des);
		ar << len;
		ar.write(car_des, len);

		orig[sizeof(orig)-1]=0;
		len=(IBYTE)strlen(orig);
		ar << len;
		ar.write(orig, len);

		dest[sizeof(dest)-1]=0;
		len=(IBYTE)strlen(dest);
		ar << len;
		ar.write(dest, len);

		IBYTE attach_cnt = (engine_count >> 4) & 0x0F;
		IBYTE traction_cnt = engine_count & 0x0F;
		if (attach_cnt+traction_cnt > sizeof(subao_engine)/sizeof(subao_engine[0]))
			return HRCODE(HRCODE::HC_FAIL);

		ar << engine_count;
		for(int i=0; i<attach_cnt+traction_cnt; i++) 
		{
			ar << subao_engine[i].engine_type;
			subao_engine[i].engine_id[sizeof(subao_engine[i].engine_id)-1]=0;
			len=(IBYTE)strlen(subao_engine[i].engine_id);
			ar << len;
			ar.write(subao_engine[i].engine_id, len);
		}

		driver_name[sizeof(driver_name)-1]=0;
		len=(IBYTE)strlen(driver_name);
		ar << len;
		ar.write(driver_name, len);

		train_leader_name[sizeof(train_leader_name)-1]=0;
		len=(IBYTE)strlen(train_leader_name);
		ar << len;
		ar.write(train_leader_name, len);

		cover[sizeof(cover)-1]=0;
		len=(IBYTE)strlen(cover);
		ar << len;
		ar.write(cover, len);

		tail_no[sizeof(tail_no)-1]=0;
		len=(IBYTE)strlen(tail_no);
		ar << len;
		ar.write(tail_no, len);

		IBYTE data_ctrl=0;
		if (0xFF != space_rank) data_ctrl |=0x01;
		if (0xFF != length_rank) data_ctrl |= 0x02;
		if (0xFF != super_rank) data_ctrl |= 0x04;
		if (0xFF != military_rank) data_ctrl |= 0x08;
		if (0xFF != speed_limit) data_ctrl |= 0x10;
		if (0xFF != e_engine) data_ctrl |= 0x20;
		if (0xFF != print_flag) data_ctrl |= 0x40;
		ar << data_ctrl;
		if (0xFF != space_rank) ar << space_rank;
		if (0xFF != length_rank) ar << length_rank;
		if (0xFF != super_rank) ar << super_rank;
		if (0xFF != military_rank) ar << military_rank;
		if (0xFF != speed_limit) ar << speed_limit;
		if (0xFF != e_engine) ar << e_engine;
		if (0xFF != print_flag) ar << print_flag;

		data_ctrl=0;
		if (-1 != pass_cnt) data_ctrl |= 0x01;
		if (-1 != load_cnt) data_ctrl |= 0x02;
		if (-1 != pk_count) data_ctrl |= 0x04;
		if (-1 != pz_count) data_ctrl |= 0x08;
		if (-1 != ck_count) data_ctrl |= 0x10;
		if (-1 != cz_count) data_ctrl |= 0x20;
		if (-1 != pik_count) data_ctrl |= 0x40;
		if (-1 != piz_count) data_ctrl |= 0x80;
		ar << data_ctrl;
		if (-1 != pass_cnt) ar << pass_cnt;
		if (-1 != load_cnt) ar << load_cnt;
		if (-1 != pk_count) ar << pk_count;
		if (-1 != pz_count) ar << pz_count;
		if (-1 != ck_count) ar << ck_count;
		if (-1 != cz_count) ar << cz_count;
		if (-1 != pik_count) ar << pik_count;
		if (-1 != piz_count) ar << piz_count;

		data_ctrl=0;
		if (-1 != gk_count) data_ctrl |= 0x01;
		if (-1 != gz_count) data_ctrl |= 0x02;
		if (-1 != o_count)  data_ctrl |= 0x04;
		if (-1 != s_count)  data_ctrl |= 0x08;
		if (-1 != total_count) data_ctrl |= 0x10;
		if (-1 != van_count)   data_ctrl |= 0x20;
		if (-1 != subao_type)  data_ctrl |= 0x40;
		if (-1 != speed)       data_ctrl |= 0x80;
		ar << data_ctrl;
		if (-1 != gk_count) ar << gk_count;
		if (-1 != gz_count) ar << gz_count;
		if (-1 != o_count)  ar << o_count;
		if (-1 != s_count)  ar << s_count;
		if (-1 != total_count) ar << total_count;
		if (-1 != van_count)   ar << van_count;
		if (-1 != subao_type)  ar << subao_type;
		if (-1 != speed)       ar << speed;

		data_ctrl=0;
		if (-1 != noutil_cnt)   data_ctrl |= 0x01;
		if (-1 != cal_len)      data_ctrl |= 0x02;
		if (-1 != total_weight) data_ctrl |= 0x04;
		ar << data_ctrl;
		if (-1 != noutil_cnt)   ar << noutil_cnt;		
		if (-1 != cal_len)      ar << cal_len;
		if (-1 != total_weight) ar << total_weight;
	}
	else
	{
		init();

		short t;
		ar >> t >> input_time >> shift_id >> train_index ;
		oper = (SUBAO_OPER)t;
		ar >> len;
		if(len > sizeof(train_id)-1)
			return HRCODE(HRCODE::HC_FAIL);
		ar.read(train_id, len);
		train_id[len]=0;

		if(oper == SUBAO_GPC_CHANGE_KEY) 
		{
			ar >> old_shift_id >> old_train_index;
			ar >> len;
			if(len > sizeof(old_train_id)-1)
				return HRCODE(HRCODE::HC_FAIL);
			ar.read(old_train_id, len);
			old_train_id[len]=0;			
		}

		ar >> report_sta >> con_id >> schd_type;

		ar >> len;
		if(len > sizeof(car_des)-1)
			return HRCODE(HRCODE::HC_FAIL);
		ar.read(car_des, len);
		car_des[len]=0;

		ar >> len;
		if(len > sizeof(orig)-1)
			return HRCODE(HRCODE::HC_FAIL);
		ar.read(orig, len);
		orig[len]=0;

		ar >> len;
		if(len > sizeof(dest)-1)
			return HRCODE(HRCODE::HC_FAIL);
		ar.read(dest, len);
		dest[len]=0;

		ar >> engine_count;
		IBYTE attach_cnt = (engine_count >> 4) & 0x0F;
		IBYTE traction_cnt = engine_count & 0x0F;
		if (attach_cnt+traction_cnt > sizeof(subao_engine)/sizeof(subao_engine[0]))
			return HRCODE(HRCODE::HC_FAIL);

		for(int i=0; i<attach_cnt+traction_cnt; i++) 
		{
			ar >> subao_engine[i].engine_type;
			ar >> len;
			if(len > sizeof(subao_engine[i].engine_id)-1)
				return HRCODE(HRCODE::HC_FAIL);
			ar.read(subao_engine[i].engine_id, len);
			subao_engine[i].engine_id[len]=0;
		}

		ar >> len;
		if(len > sizeof(driver_name)-1)
			return HRCODE(HRCODE::HC_FAIL);
		ar.read(driver_name, len);
		driver_name[len]=0;

		ar >> len;
		if(len > sizeof(train_leader_name)-1)
			return HRCODE(HRCODE::HC_FAIL);
		ar.read(train_leader_name, len);
		train_leader_name[len]=0;

		ar >> len;
		if(len > sizeof(cover)-1)
			return HRCODE(HRCODE::HC_FAIL);
		ar.read(cover, len);
		cover[len]=0;

		ar >> len;
		if(len > sizeof(tail_no)-1)
			return HRCODE(HRCODE::HC_FAIL);
		ar.read(tail_no, len);
		tail_no[len]=0;

		IBYTE data_ctrl;
		ar >> data_ctrl;
		if (data_ctrl & 0x01) ar >> space_rank;
		if (data_ctrl & 0x02) ar >> length_rank;
		if (data_ctrl & 0x04) ar >> super_rank;
		if (data_ctrl & 0x08) ar >> military_rank;
		if (data_ctrl & 0x10) ar >> speed_limit;
		if (data_ctrl & 0x20) ar >> e_engine;
		if (data_ctrl & 0x40) ar >> print_flag;

		ar >> data_ctrl;
		if (data_ctrl & 0x01) ar >> pass_cnt;
		if (data_ctrl & 0x02) ar >> load_cnt;
		if (data_ctrl & 0x04) ar >> pk_count;
		if (data_ctrl & 0x08) ar >> pz_count;
		if (data_ctrl & 0x10) ar >> ck_count;
		if (data_ctrl & 0x20) ar >> cz_count;
		if (data_ctrl & 0x40) ar >> pik_count;
		if (data_ctrl & 0x80) ar >> piz_count;

		ar >> data_ctrl;
		if (data_ctrl & 0x01) ar >> gk_count;
		if (data_ctrl & 0x02) ar >> gz_count;
		if (data_ctrl & 0x04) ar >> o_count;
		if (data_ctrl & 0x08) ar >> s_count;
		if (data_ctrl & 0x10) ar >> total_count;
		if (data_ctrl & 0x20) ar >> van_count;
		if (data_ctrl & 0x40) ar >> subao_type;
		if (data_ctrl & 0x80) ar >> speed;

		ar >> data_ctrl;
		if (data_ctrl & 0x01) ar >> noutil_cnt;
		if (data_ctrl & 0x02) ar >> cal_len;
		if (data_ctrl & 0x04) ar >> total_weight;		

	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgSubaoFormat::countObjectLength()
{
	size_t len=sizeof(short)+sizeof(input_time)+sizeof(shift_id)+sizeof(train_index);

	train_id[sizeof(train_id)-1]=0;
	len += (sizeof(IBYTE)+strlen(train_id));

	if(oper == SUBAO_GPC_CHANGE_KEY) 
	{
		len += sizeof(old_shift_id)+ sizeof(old_train_index);
		old_train_id[sizeof(old_train_id)-1]=0;
		len += sizeof(IBYTE)+strlen(old_train_id);
	}

	len += sizeof(report_sta)+sizeof(con_id)+sizeof(schd_type);

	car_des[sizeof(car_des)-1]=0;
	len += (sizeof(IBYTE)+strlen(car_des));
	orig[sizeof(orig)-1]=0;
	len += (sizeof(IBYTE)+strlen(orig));
	dest[sizeof(dest)-1]=0;
	len += (sizeof(IBYTE)+strlen(dest));

	IBYTE attach_cnt = (engine_count >> 4) & 0x0F;
	IBYTE traction_cnt = engine_count & 0x0F;
	len += sizeof(engine_count);
	for(int i=0; i<attach_cnt+traction_cnt; i++) 
	{
		len += sizeof(subao_engine[i].engine_type);
		subao_engine[i].engine_id[sizeof(subao_engine[i].engine_id)-1]=0;
		len +=(sizeof(IBYTE)+strlen(subao_engine[i].engine_id));		
	}

	driver_name[sizeof(driver_name)-1]=0;
	len += (sizeof(IBYTE)+strlen(driver_name));
	train_leader_name[sizeof(train_leader_name)-1]=0;
	len += (sizeof(IBYTE)+strlen(train_leader_name));
	cover[sizeof(cover)-1]=0;
	len += (sizeof(IBYTE)+strlen(cover));
	tail_no[sizeof(tail_no)-1]=0;
	len += (sizeof(IBYTE)+strlen(tail_no));

	len += sizeof(IBYTE);
	if (0xFF != space_rank)    len+=sizeof(space_rank);
	if (0xFF != length_rank)   len+=sizeof(length_rank);
	if (0xFF != super_rank)    len+=sizeof(super_rank);
	if (0xFF != military_rank) len+=sizeof(military_rank);
	if (0xFF != speed_limit)   len+=sizeof(speed_limit);
	if (0xFF != e_engine)      len+=sizeof(e_engine);
	if (0xFF != print_flag)    len+=sizeof(print_flag);

	len += sizeof(IBYTE);
	if (-1 != pass_cnt) len+=sizeof(pass_cnt);
	if (-1 != load_cnt) len+=sizeof(load_cnt);
	if (-1 != pk_count) len+=sizeof(pk_count);
	if (-1 != pz_count) len+=sizeof(pz_count);
	if (-1 != ck_count) len+=sizeof(ck_count);
	if (-1 != cz_count) len+=sizeof(cz_count);
	if (-1 != pik_count) len+=sizeof(pik_count);
	if (-1 != piz_count) len+=sizeof(piz_count);

	len += sizeof(IBYTE);
	if (-1 != gk_count) len+=sizeof(gk_count);
	if (-1 != gz_count) len+=sizeof(gz_count);
	if (-1 != o_count)  len+=sizeof(o_count);
	if (-1 != s_count)  len+=sizeof(s_count);
	if (-1 != total_count) len+=sizeof(total_count);
	if (-1 != van_count)   len+=sizeof(van_count);
	if (-1 != subao_type)  len+=sizeof(subao_type);
	if (-1 != speed)       len+=sizeof(speed);

	len += sizeof(IBYTE);
	if (-1 != noutil_cnt)   len+=sizeof(noutil_cnt);
	if (-1 != cal_len)      len+=sizeof(cal_len);
	if (-1 != total_weight) len+=sizeof(total_weight);

	return len;
}

IMPLEMENT_CLBTH(MsgSubaoQuery, MSGID_MsgSubaoQuery)
IMPLEMENT_SERIALIZE_OPERATOR(MsgSubaoQuery)
MsgSubaoQuery::MsgSubaoQuery()
{
	con_id = (CONID)-1;
	schd_type = 0;
	train_index = NULL_TRAIN_INDEX;
	shift_id = (SHIFT_ID)-1;
}
MsgSubaoQuery::~MsgSubaoQuery()
{

}
HRCODE MsgSubaoQuery::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << con_id << schd_type << train_index << shift_id << (short)query_filter;
	}
	else
	{
		short t;
		ar >> con_id >> schd_type >> train_index >> shift_id >> t;
		query_filter = (SUBAO_QUERY_FILTER)t;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgSubaoQuery::countObjectLength()
{
	return sizeof(con_id)+sizeof(schd_type)+sizeof(train_index)+sizeof(shift_id)+sizeof(short);
}

///////////////
IMPLEMENT_CLBTH(MsgGpcMsgEcho, MSGID_MsgGpcMsgEcho)
IMPLEMENT_SERIALIZE_OPERATOR(MsgGpcMsgEcho)
MsgGpcMsgEcho::MsgGpcMsgEcho(int version):m_Version(version)
{

}
MsgGpcMsgEcho::~MsgGpcMsgEcho()
{

}
HRCODE MsgGpcMsgEcho::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << msgid << ecode;
	}
	else
	{
		ar >> msgid >> ecode;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgGpcMsgEcho::countObjectLength()
{
	return sizeof(msgid)+sizeof(ecode);
}

///////////////
IMPLEMENT_CLBTH(MsgHugeDataCtrl, MSGID_MsgHugeDataCtrl)
IMPLEMENT_SERIALIZE_OPERATOR(MsgHugeDataCtrl)
MsgHugeDataCtrl::MsgHugeDataCtrl()
{

}
MsgHugeDataCtrl::~MsgHugeDataCtrl()
{

}
HRCODE MsgHugeDataCtrl::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << msgid << (short)status << msgpara;
	}
	else
	{
		short t;
		ar >> msgid >> t >> msgpara;
		status = (STATUS)t;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgHugeDataCtrl::countObjectLength()
{
	return sizeof(msgid)+sizeof(short)+sizeof(msgpara);
}

///////////////
IMPLEMENT_CLBTH(MsgSimButton, MSGID_MsgSimButton)
IMPLEMENT_SERIALIZE_OPERATOR(MsgSimButton)
MsgSimButton::MsgSimButton()
{

}
MsgSimButton::~MsgSimButton()
{

}
HRCODE MsgSimButton::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << nStationNo<<nButtonCount;
		for(int i = 0;i<nButtonCount;i++)
			ar << sButtonInput[i];
	}
	else
	{
		ar >> nStationNo >> nButtonCount;
		for(int i = 0;i<nButtonCount;i++)
			ar >> sButtonInput[i];
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgSimButton::countObjectLength()
{
	return sizeof(nStationNo)+sizeof(nButtonCount)+
		nStationNo*sizeof(sButtonInput[0]);
}

///////////////
IMPLEMENT_CLBTH(MsgBasicTrainListInit, MSGID_MsgBasicTrainListInit)
IMPLEMENT_SERIALIZE_OPERATOR(MsgBasicTrainListInit)
MsgBasicTrainListInit::MsgBasicTrainListInit()
{

}
MsgBasicTrainListInit::~MsgBasicTrainListInit()
{

}
HRCODE MsgBasicTrainListInit::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << gpc << con_id << station << stime << etime <<
			shift_begin_time << shift_end_time <<
			(short)train_type << (short)direction <<
			(short)flag << (short)hat_flag;
	}
	else
	{
		ar >> gpc >> con_id >> station >> stime >> etime >>
			shift_begin_time >> shift_end_time;
		short t;
		ar >> t;	train_type=(INIT_TRAINTYPE)t;
		ar >> t;	direction=(INIT_DIRECTION)t;
		ar >> t;	flag=(INIT_START_ACCEPT)t;
		ar >> t;	hat_flag=(INIT_HAT_FLAG)t;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgBasicTrainListInit::countObjectLength()
{
	return sizeof(gpc)+sizeof(con_id)+sizeof(station)+sizeof(stime)+
		sizeof(etime)+sizeof(shift_begin_time)+sizeof(shift_end_time)+sizeof(short)*4;
}

///////////////
IMPLEMENT_CLBTH(MsgBasicTrainListReply, MSGID_MsgBasicTrainListReply)
IMPLEMENT_SERIALIZE_OPERATOR(MsgBasicTrainListReply)
MsgBasicTrainListReply::MsgBasicTrainListReply()
{

}
MsgBasicTrainListReply::~MsgBasicTrainListReply()
{

}
HRCODE MsgBasicTrainListReply::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		train_id[sizeof(train_id)-1]='\0';
		ar.write(train_id, sizeof(train_id));
		ar << train_index << arrive << depart <<
			(short)type << (short)dir << (short)hat_train <<
			(short)exist_train << (short)add_train;
	}
	else
	{
		ar.read(train_id, sizeof(train_id));
		train_id[sizeof(train_id)-1]='\0';
		ar >> train_index >> arrive >> depart;
		short t;
		ar >> t;	type=(BASIC_TRAIN_LIST_TYPE)t;
		ar >> t;	dir=(BASIC_TRAIN_LIST_DIR)t;
		ar >> t;	hat_train=(BASIC_TRAIN_LIST_HAT)t;
		ar >> t;	exist_train=(BASIC_TRAIN_LIST_EXIST)t;
		ar >> t;	add_train=(BASIC_TRAIN_LIST_ADD)t;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgBasicTrainListReply::countObjectLength()
{
	return sizeof(train_id)+sizeof(train_index)+sizeof(arrive)+sizeof(depart)+
		sizeof(short)*5;
}

///////////////
IMPLEMENT_CLBTH(MsgDssDisplayOrder, MSGID_MsgDssDisplayOrder)
IMPLEMENT_SERIALIZE_OPERATOR(MsgDssDisplayOrder)
MsgDssDisplayOrder::MsgDssDisplayOrder()
{
	CLBTH_ZERO_INIT_OBJECT(BaseMsg)
}
MsgDssDisplayOrder::~MsgDssDisplayOrder()
{

}
HRCODE MsgDssDisplayOrder::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		if(sta_count > LOGIN_MAX_STATION_COUNT)
			return HRCODE(HRCODE::HC_FAIL);
		ar << sta_count;
		for(int i=0; i<sta_count; i++)
			ar << sta_list[i];
		ar << (short) flag;
	}
	else
	{
		ar >> sta_count;
		if(sta_count > LOGIN_MAX_STATION_COUNT)
			return HRCODE(HRCODE::HC_FAIL);
		for(int i=0; i<sta_count; i++)
			ar >> sta_list[i];
		short t;
		ar >> t;
		flag=(DSS_ORDER)t;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgDssDisplayOrder::countObjectLength()
{
	return sizeof(sta_count)+sizeof(STATION_NO)*sta_count+
		sizeof(short);
}


///////////////
IMPLEMENT_CLBTH(MsgEntityConnStatusOrder, MSGID_MsgEntityConnStatusOrder)
IMPLEMENT_SERIALIZE_OPERATOR(MsgEntityConnStatusOrder)
MsgEntityConnStatusOrder::MsgEntityConnStatusOrder()
{
	CLBTH_ZERO_INIT_OBJECT(BaseMsg)
}
MsgEntityConnStatusOrder::~MsgEntityConnStatusOrder()
{

}
HRCODE MsgEntityConnStatusOrder::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		if(entity_count > LOGIN_MAX_ENTITY_COUNT)
			return HRCODE(HRCODE::HC_FAIL);
		ar << entity_count;
		for(int i=0; i<entity_count; i++)
			ar << msg_dispatch[i] << entity_list[i];
		ar << (short)flag;
	}
	else
	{
		ar >> entity_count;
		if(entity_count > LOGIN_MAX_ENTITY_COUNT)
			return HRCODE(HRCODE::HC_FAIL);
		for(int i=0; i<entity_count; i++)
			ar >> msg_dispatch[i] >> entity_list[i];
		short t;
		ar >> t;
		flag = (CONN_ORDER)t;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgEntityConnStatusOrder::countObjectLength()
{
	return sizeof(entity_count)+sizeof(short)+
		(sizeof(entity_list[0])+sizeof(msg_dispatch[0]))*entity_count;
}

///////////////
IMPLEMENT_CLBTH(MsgTimeSync, MSGID_MsgTimeSync)
IMPLEMENT_SERIALIZE_OPERATOR(MsgTimeSync)
MsgTimeSync::MsgTimeSync()
{
	CLBTH_ZERO_INIT_OBJECT(BaseMsg)
}
MsgTimeSync::~MsgTimeSync()
{

}
HRCODE MsgTimeSync::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << ltime;
	}
	else
	{
		ar >> ltime;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgTimeSync::countObjectLength()
{
	return sizeof(ltime);
}

////////////////////////////////////////////////////
IMPLEMENT_CLBTH(MsgLoadTrainFormRequest, MSGID_MsgLoadTrainFormRequest)
IMPLEMENT_SERIALIZE_OPERATOR(MsgLoadTrainFormRequest)
MsgLoadTrainFormRequest::MsgLoadTrainFormRequest()
{
	CLBTH_ZERO_INIT_OBJECT(BaseMsg)
}

MsgLoadTrainFormRequest::~MsgLoadTrainFormRequest()
{
}

HRCODE MsgLoadTrainFormRequest::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << con_id << gpc_id << job_id << station_id << entily_id;
		user_code[sizeof(user_code)-1]='\0';
		ar.write(user_code, sizeof(user_code));
		ar << train_index;
		train_id[sizeof(train_id)-1]='\0';
		ar.write(train_id, sizeof(train_id));
		ar << (short)request_flag;
	}
	else
	{
		ar >> con_id >> gpc_id >> job_id >> station_id >> entily_id;
		ar.read(user_code, sizeof(user_code));
		ar >> train_index;
		ar.read(train_id, sizeof(train_id));
		short flag;
		ar >> flag;
		request_flag = (TRAIN_FORM_LOAD_REQUEST)flag;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgLoadTrainFormRequest::countObjectLength()
{
	return sizeof(con_id) + sizeof(gpc_id) + sizeof(job_id) + sizeof(station_id) + sizeof(entily_id)
		+ sizeof(user_code) + sizeof(train_index) + sizeof(train_id) + sizeof(short);
}

////////////////////////////////////////////////////
IMPLEMENT_CLBTH(MsgLoadAsmTrainFormHead, MSGID_MsgLoadAsmTrainFormHead)
IMPLEMENT_SERIALIZE_OPERATOR(MsgLoadAsmTrainFormHead)
MsgLoadAsmTrainFormHead::MsgLoadAsmTrainFormHead()
{
	CLBTH_ZERO_INIT_OBJECT(BaseMsg)
}

MsgLoadAsmTrainFormHead::~MsgLoadAsmTrainFormHead()
{
}

HRCODE MsgLoadAsmTrainFormHead::doSerialize(Archive& ar)
{
	IBYTE len=0;
	if(ar.isStoring())
	{
		ar << con_id;
		con_name[sizeof(con_name)-1]='\0';
		len=(IBYTE)strlen(con_name);
		ar << len;
		ar.write(con_name, len);
		ar << station_id << train_index;
		train_id[sizeof(train_id)-1]='\0';
		ar.write(train_id, sizeof(train_id));
		ar << TMIS_train_id;
		marshing_train_id[sizeof(marshing_train_id)-1]='\0';
		len=(IBYTE)strlen(marshing_train_id);
		ar << len;
		ar.write(marshing_train_id, len);
		runing_train_id[sizeof(runing_train_id)-1] = '\0';
		len=(IBYTE)strlen(runing_train_id);
		ar << len;
		ar.write(runing_train_id, len);
		marshing_station_code[sizeof(marshing_station_code)-1]='\0';
		len=(IBYTE)strlen(marshing_station_code);
		ar << len;
		ar.write(marshing_station_code, len);
		ar << marshing_station_id << shunting_station_id;
		shunting_station_code[sizeof(shunting_station_code)-1] = '\0';
		len=(IBYTE)strlen(shunting_station_code);
		ar << len;
		ar.write(shunting_station_code, len);
		ar << plan_time << actl_time << send_time << toal_cnt << cal_len
			<< weight << self_weight << van_cnt;
		reporter[sizeof(reporter)-1] = '\0';
		len=(IBYTE)strlen(reporter);
		ar << len;
		ar.write(reporter,len);
		assigner[sizeof(assigner)-1]='\0';
		len=(IBYTE)strlen(assigner);
		ar << len;
		ar.write(assigner, len);
		sg_station_name[sizeof(sg_station_name)-1]='\0';
		len=(IBYTE)strlen(sg_station_name);
		ar << len;
		ar.write(sg_station_name, len);
		ar << sg_station_id;
		sg_station_code[sizeof(sg_station_code)-1] = '\0';
		len=(IBYTE)strlen(sg_station_code);
		ar << len;
		ar.write(sg_station_code, len);
		ar << weight_1 << weight_2 << marshing_cnt << ht_flag << no_use_flag
			<< car_no_count_flag << car_count_flag << self_car_flag
			<< received_tmis_flag << received_tmis_time;
	}
	else
	{
		ar >> con_id;
		ar >> len;
		ar.read(con_name, len);
		con_name[len]='\0';
		ar >> station_id >> train_index;
		ar.read(train_id, sizeof(train_id));
		train_id[sizeof(train_id)-1]='\0';
		ar >> TMIS_train_id;
		ar >> len;
		ar.read(marshing_train_id, len);
		marshing_train_id[len] = '\0';
		ar >> len;
		ar.read(runing_train_id, len);
		runing_train_id[len]='\0';
		ar >> len;
		ar.read(marshing_station_code, len);
		marshing_station_code[len] = '\0';
		ar >> marshing_station_id >> shunting_station_id;
		ar >> len;
		ar.read(shunting_station_code, len);
		shunting_station_code[len] = '\0';
		ar >> plan_time >> actl_time >> send_time >> toal_cnt >> cal_len
			>> weight >> self_weight >> van_cnt;
		ar >> len;
		ar.read(reporter,len);
		reporter[len] = '\0';
		ar >> len;
		ar.read(assigner, len);
		assigner[len]='\0';
		ar >> len;
		ar.read(sg_station_name, len);
		sg_station_name[len]='\0';
		ar >> sg_station_id;
		ar >> len;
		ar.read(sg_station_code, len);
		sg_station_code[len] = '\0';
		ar >> weight_1 >> weight_2 >> marshing_cnt >> ht_flag >> no_use_flag
			>> car_no_count_flag >> car_count_flag >> self_car_flag
			>> received_tmis_flag >> received_tmis_time;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgLoadAsmTrainFormHead::countObjectLength()
{
	con_name[sizeof(con_name)-1] = '\0';
	marshing_train_id[sizeof(marshing_train_id)-1] = '\0';
	runing_train_id[sizeof(runing_train_id)-1] = '\0';
	marshing_station_code[sizeof(marshing_station_code)-1] = '\0';
	shunting_station_code[sizeof(shunting_station_code)-1] = '\0';
	reporter[sizeof(reporter)-1] = '\0';
	assigner[sizeof(assigner)-1]='\0';
	sg_station_name[sizeof(sg_station_name)-1]='\0';
	sg_station_code[sizeof(sg_station_code)-1] = '\0';
	return sizeof(con_id) + sizeof(IBYTE) + strlen(con_name) + sizeof(station_id)
		+ sizeof(train_index) + sizeof(train_id) + sizeof(TMIS_train_id)
		+ sizeof(IBYTE) + strlen(marshing_train_id) + sizeof(IBYTE) + strlen(runing_train_id)
		+ sizeof(IBYTE) + strlen(marshing_station_code) + sizeof(marshing_station_id)
		+ sizeof(shunting_station_id) + sizeof(IBYTE) + strlen(shunting_station_code)
		+ sizeof(plan_time) + sizeof(actl_time) + sizeof(send_time) + sizeof(toal_cnt)
		+ sizeof(cal_len) + sizeof(weight) + sizeof(self_weight) + sizeof(van_cnt)
		+ sizeof(IBYTE) + strlen(reporter) + sizeof(IBYTE) + strlen(assigner)
		+ sizeof(IBYTE) + strlen(sg_station_name) + sizeof(sg_station_id) + sizeof(IBYTE)
		+ strlen(sg_station_code) + sizeof(weight_1) + sizeof(weight_2) + sizeof(marshing_cnt)
		+ sizeof(ht_flag) + sizeof(no_use_flag) + sizeof(car_no_count_flag) + sizeof(car_count_flag)
		+ sizeof(self_car_flag)	+ sizeof(received_tmis_flag) + sizeof(received_tmis_time);
}


/////////////////////////////////////////////////////
IMPLEMENT_CLBTH(MsgLoadAsmTrainForm, MSGID_MsgLoadAsmTrainForm)
IMPLEMENT_SERIALIZE_OPERATOR(MsgLoadAsmTrainForm)
MsgLoadAsmTrainForm::MsgLoadAsmTrainForm()
{
	CLBTH_ZERO_INIT_OBJECT(BaseMsg)
}

MsgLoadAsmTrainForm::~MsgLoadAsmTrainForm()
{
}

HRCODE MsgLoadAsmTrainForm::doSerialize(Archive& ar)
{
	IBYTE len=0;
	if(ar.isStoring())
	{
		ar << con_id << train_index << TMIS_train_id << car_pos << car_type_id;
		car_type[sizeof(car_type)-1]='\0';
		len = (IBYTE)strlen(car_type);
		ar << len;
		ar.write(car_type, len);
		ar << tanker_oil_type;
		car_no[sizeof(car_no)-1]='\0';
		len = (IBYTE)strlen(car_no);
		ar << len;
		ar.write(car_no, len);
		ar << car_suttle << car_len_cnt << car_load << load_flag;
		fisrt_station_code[sizeof(fisrt_station_code)-1] = '\0';
		len = (IBYTE)strlen(fisrt_station_code);
		ar << len;
		ar.write(fisrt_station_code, len);
		ar << first_station_id;
		last_station_code[sizeof(last_station_code)-1] = '\0';
		len = (IBYTE)strlen(last_station_code);
		ar << len;
		ar.write(last_station_code, len);
		ar << last_station_id;
		freight_name[sizeof(freight_name)-1] = '\0';
		len = (IBYTE)strlen(freight_name);
		ar << len;
		ar.write(freight_name, len);
		freight_code[sizeof(freight_code)-1] = '\0';
		len = (IBYTE)strlen(freight_code);
		ar << len;
		ar.write(freight_code, len);
		receiver[sizeof(receiver)-1] = '\0';
		len = (IBYTE)strlen(receiver);
		ar << len;
		ar.write(receiver, len);
		depart_station_code[sizeof(depart_station_code)-1] = '\0';
		len = (IBYTE)strlen(depart_station_code);
		ar << len;
		ar.write(depart_station_code, len);
		ar << depart_station_id << car_van_cnt;
		car_note[sizeof(car_note)-1] = '\0';
		len = (IBYTE)strlen(car_note);
		ar << len;
		ar.write(car_note, len);
		car_type_code[sizeof(car_type_code)-1]='\0';
		len = (IBYTE)strlen(car_note);
		ar << len;
		ar.write(car_type_code, len);
		container_type_1_cnt[sizeof(container_type_1_cnt)-1]='\0';
		len = (IBYTE)strlen(container_type_1_cnt);
		ar << len;
		ar.write(container_type_1_cnt, len);
		weight_container_1_cnt[sizeof(weight_container_1_cnt)-1]='\0';
		len = (IBYTE)strlen(weight_container_1_cnt);
		ar << len;
		ar.write(weight_container_1_cnt, len);
		empty_container_1_cnt[sizeof(empty_container_1_cnt)-1]='\0';
		len = (IBYTE)strlen(empty_container_1_cnt);
		ar << len;
		ar.write(empty_container_1_cnt, len);
		container_type_2_cnt[sizeof(container_type_2_cnt)-1]='\0';
		len = (IBYTE)strlen(container_type_2_cnt);
		ar << len;
		ar.write(container_type_2_cnt, len);
		weight_container_2_cnt[sizeof(weight_container_2_cnt)-1]='\0';
		len = (IBYTE)strlen(weight_container_2_cnt);
		ar << len;
		ar.write(weight_container_2_cnt, len);
		empty_container_2_cnt[sizeof(empty_container_2_cnt)-1]='\0';
		len = (IBYTE)strlen(empty_container_2_cnt);
		ar << len;
		ar.write(empty_container_2_cnt, len);
		ar << car_empty_weight_flag;
		first_station_name[sizeof(first_station_name)-1]='\0';
		len = (IBYTE)strlen(first_station_name);
		ar << len;
		ar.write(first_station_name, len);
		last_station_name[sizeof(last_station_name)-1]='\0';
		len = (IBYTE)strlen(last_station_name);
		ar << len;
		ar.write(last_station_name, len);
		depart_station_name[sizeof(depart_station_name)-1]='\0';
		len = (IBYTE)strlen(depart_station_name);
		ar << len;
		ar.write(depart_station_name, len);
		aim_bureau[sizeof(aim_bureau)-1]='\0';
		len = (IBYTE)strlen(aim_bureau);
		ar << len;
		ar.write(aim_bureau, len);
		aim_subbureau[sizeof(aim_subbureau)-1]='\0';
		len = (IBYTE)strlen(aim_subbureau);
		ar << len;
		ar.write(aim_subbureau, len);
		direction_no[sizeof(direction_no)-1]='\0';
		len = (IBYTE)strlen(direction_no);
		ar << len;
		ar.write(direction_no, len);
		frieght_no_use_code[sizeof(frieght_no_use_code)-1]='\0';
		len = (IBYTE)strlen(frieght_no_use_code);
		ar << len;
		ar.write(frieght_no_use_code, len);
		railway_company_flag[sizeof(railway_company_flag)-1] = '\0';
		len = (IBYTE)strlen(railway_company_flag);
		ar << len;
		ar.write(railway_company_flag, len);
		no_count_flag[sizeof(no_count_flag)-1] = '\0';
		len = (IBYTE)strlen(no_count_flag);
		ar << len;
		ar.write(no_count_flag, len);
	}
	else
	{
		ar >> con_id >> train_index >> TMIS_train_id >> car_pos >> car_type_id;
		ar >> len;
		ar.read(car_type, len);
		car_type[len]='\0';
		ar >> tanker_oil_type;
		ar >> len;
		ar.read(car_no, len);
		car_no[len]='\0';
		ar >> car_suttle >> car_len_cnt >> car_load >> load_flag;
		ar >> len;
		ar.read(fisrt_station_code, len);
		fisrt_station_code[len] = '\0';
		ar >> first_station_id;
		ar >> len;
		ar.read(last_station_code, len);
		last_station_code[len] = '\0';
		ar >> last_station_id;
		ar >> len;
		ar.read(freight_name, len);
		freight_name[len] = '\0';
		ar >> len;
		ar.read(freight_code, len);
		freight_code[len] = '\0';
		ar >> len;
		ar.read(receiver, len);
		receiver[len] = '\0';
		ar >> len;
		ar.read(depart_station_code, len);
		depart_station_code[len] = '\0';
		ar >> depart_station_id >> car_van_cnt;
		ar >> len;
		ar.read(car_note, len);
		car_note[len] = '\0';
		ar >> len;
		ar.read(car_type_code, len);
		car_type_code[len]='\0';
		ar >> len;
		ar.read(container_type_1_cnt, len);
		container_type_1_cnt[len]='\0';
		ar >> len;
		ar.read(weight_container_1_cnt, len);
		weight_container_1_cnt[len]='\0';
		ar >> len;
		ar.read(empty_container_1_cnt, len);
		empty_container_1_cnt[len]='\0';
		ar >> len;
		ar.read(container_type_2_cnt, len);
		container_type_2_cnt[len]='\0';
		ar >> len;
		ar.read(weight_container_2_cnt, len);
		weight_container_2_cnt[len]='\0';
		ar >> len;
		ar.read(empty_container_2_cnt, len);
		empty_container_2_cnt[len]='\0';
		ar >> car_empty_weight_flag;
		ar >> len;
		ar.read(first_station_name, len);
		first_station_name[len]='\0';
		ar >> len;
		ar.read(last_station_name, len);
		last_station_name[len]='\0';
		ar >> len;
		ar.read(depart_station_name, len);
		depart_station_name[len]='\0';
		ar >> len;
		ar.read(aim_bureau, len);
		aim_bureau[len]='\0';
		ar >> len;
		ar.read(aim_subbureau, len);
		aim_subbureau[len]='\0';
		ar >> len;
		ar.read(direction_no, len);
		direction_no[len]='\0';
		ar >> len;
		ar.read(frieght_no_use_code, len);
		frieght_no_use_code[len]='\0';
		ar >> len;
		ar.read(railway_company_flag, len);
		railway_company_flag[len]='\0';
		ar >> len;
		ar.read(no_count_flag, len);
		no_count_flag[len] = '\0';
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}

size_t MsgLoadAsmTrainForm::countObjectLength()
{
	car_type[sizeof(car_type)-1]='\0';
	car_no[sizeof(car_no)-1]='\0';
	fisrt_station_code[sizeof(fisrt_station_code)-1] = '\0';
	last_station_code[sizeof(last_station_code)-1] = '\0';
	freight_name[sizeof(freight_name)-1] = '\0';
	freight_code[sizeof(freight_code)-1] = '\0';
	receiver[sizeof(receiver)-1] = '\0';
	depart_station_code[sizeof(depart_station_code)-1] = '\0';
	car_note[sizeof(car_note)-1] = '\0';
	car_type_code[sizeof(car_type_code)-1]='\0';
	container_type_1_cnt[sizeof(container_type_1_cnt)-1]='\0';
	weight_container_1_cnt[sizeof(weight_container_1_cnt)-1]='\0';
	empty_container_1_cnt[sizeof(empty_container_1_cnt)-1]='\0';
	container_type_2_cnt[sizeof(container_type_2_cnt)-1]='\0';
	weight_container_2_cnt[sizeof(weight_container_2_cnt)-1]='\0';
	empty_container_2_cnt[sizeof(empty_container_2_cnt)-1]='\0';
	first_station_name[sizeof(first_station_name)-1]='\0';
	last_station_name[sizeof(last_station_name)-1]='\0';
	depart_station_name[sizeof(depart_station_name)-1]='\0';
	aim_bureau[sizeof(aim_bureau)-1]='\0';
	aim_subbureau[sizeof(aim_subbureau)-1]='\0';
	direction_no[sizeof(direction_no)-1]='\0';
	frieght_no_use_code[sizeof(frieght_no_use_code)-1]='\0';
	railway_company_flag[sizeof(railway_company_flag)-1] = '\0';
	no_count_flag[sizeof(no_count_flag)-1] = '\0';
	return sizeof(con_id) + sizeof(train_index) + sizeof(TMIS_train_id) + sizeof(car_pos)
		+ sizeof(car_type_id)	+ sizeof(IBYTE) + strlen(car_type) + sizeof(tanker_oil_type)
		+ sizeof(IBYTE) + strlen(car_no) + sizeof(car_suttle) + sizeof(car_len_cnt) + sizeof(car_load)
		+ sizeof(load_flag) + sizeof(IBYTE) + strlen(fisrt_station_code) + sizeof(first_station_id)
		+ sizeof(IBYTE) + strlen(last_station_code) + sizeof(last_station_id)
		+ sizeof(IBYTE) + strlen(freight_name) + sizeof(IBYTE) + strlen(freight_code)
		+ sizeof(IBYTE) + strlen(receiver) + sizeof(IBYTE) + strlen(depart_station_code)
		+ sizeof(depart_station_id) + sizeof(car_van_cnt) + sizeof(IBYTE) + strlen(car_note)
		+ sizeof(IBYTE) + strlen(car_type_code) + sizeof(IBYTE) + strlen(container_type_1_cnt)
		+ sizeof(IBYTE) + strlen(weight_container_1_cnt) + sizeof(IBYTE) + strlen(empty_container_1_cnt)
		+ sizeof(IBYTE) + strlen(container_type_2_cnt) + sizeof(IBYTE) + strlen(weight_container_2_cnt)
		+ sizeof(IBYTE) + strlen(empty_container_2_cnt) + sizeof(car_empty_weight_flag)
		+ sizeof(IBYTE) + strlen(first_station_name) + sizeof(IBYTE) + strlen(last_station_name)
		+ sizeof(IBYTE) + strlen(depart_station_name) + sizeof(IBYTE) + strlen(aim_bureau)
		+ sizeof(IBYTE) + strlen(aim_subbureau) + sizeof(IBYTE) + strlen(direction_no)
		+ sizeof(IBYTE) + strlen(frieght_no_use_code) + sizeof(IBYTE) + strlen(railway_company_flag)
		+ sizeof(IBYTE) + strlen(no_count_flag);
}

///////////////////////////////////////////////////////////
IMPLEMENT_CLBTH(MsgLoadTrainFormResponse, MSGID_MsgLoadTrainFormResponse)
IMPLEMENT_SERIALIZE_OPERATOR(MsgLoadTrainFormResponse)
MsgLoadTrainFormResponse::MsgLoadTrainFormResponse()
{
	CLBTH_ZERO_INIT_OBJECT(BaseMsg)
}

MsgLoadTrainFormResponse::~MsgLoadTrainFormResponse()
{
}

HRCODE MsgLoadTrainFormResponse::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << con_id << gpc_id << station_id << entily_id << train_index << (short)load_flag;
	}
	else
	{
		ar >> con_id >> gpc_id >> station_id >> entily_id >> train_index;
		short flag;
		ar >> flag;
		load_flag = (TRAIN_FORM_LOAD_RESULT)flag;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgLoadTrainFormResponse::countObjectLength()
{
	return sizeof(con_id) + sizeof(gpc_id) + sizeof(station_id)
		+ sizeof(entily_id) + sizeof(train_index) + sizeof(short);
}

////////////////////////////////////////////////////
// message for station store carriage
////////////////////////////////////////////////////
// requst
IMPLEMENT_CLBTH(MsgLoadSscRequest, MSGID_MsgLoadSscRequest)
IMPLEMENT_SERIALIZE_OPERATOR(MsgLoadSscRequest)
MsgLoadSscRequest::MsgLoadSscRequest()
{
	CLBTH_ZERO_INIT_OBJECT(BaseMsg)
}

MsgLoadSscRequest::~MsgLoadSscRequest()
{
}

HRCODE MsgLoadSscRequest::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << con_id << gpc_id << job_id << station_no << entily_id;
		user_code[sizeof(user_code)-1]='\0';
		ar.write(user_code, sizeof(user_code));
		ar << (short)request_flag;
	}
	else
	{
		ar >> con_id >> gpc_id >> job_id >> station_no >> entily_id;
		ar.read(user_code, sizeof(user_code));
		short flag;
		ar >> flag;
		request_flag = (SSC_LOAD_REQUEST)flag;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgLoadSscRequest::countObjectLength()
{
	user_code[sizeof(user_code)-1]='\0';
	return sizeof(con_id) + sizeof(gpc_id) + sizeof(job_id) + sizeof(station_no)
		+ sizeof(entily_id) + sizeof(user_code) + sizeof(short);
}

///////////////////////////////////////////////////////////
IMPLEMENT_CLBTH(MsgLoadSscResponse, MSGID_MsgLoadSscResponse)
IMPLEMENT_SERIALIZE_OPERATOR(MsgLoadSscResponse)
MsgLoadSscResponse::MsgLoadSscResponse()
{
	CLBTH_ZERO_INIT_OBJECT(BaseMsg)
}

MsgLoadSscResponse::~MsgLoadSscResponse()
{
}

HRCODE MsgLoadSscResponse::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << con_id << gpc_id << station_no << entily_id << block_count << carriage_count
			<< side_count << (short)load_flag;
	}
	else
	{
		ar >> con_id >> gpc_id >> station_no >> entily_id >> block_count >> carriage_count >> side_count;
		short flag;
		ar >> flag;
		load_flag = (SSC_LOAD_RESPONSE)flag;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgLoadSscResponse::countObjectLength()
{
	return sizeof(con_id) + sizeof(gpc_id) + sizeof(station_no) + sizeof(entily_id) + sizeof(block_count)
		+ sizeof(carriage_count) + sizeof(side_count) + sizeof(short);
}

///////////////////////////////////////////////////////////
IMPLEMENT_CLBTH(MsgUpdateSscRequest, MSGID_MsgUpdateSscRequest)
IMPLEMENT_SERIALIZE_OPERATOR(MsgUpdateSscRequest)
MsgUpdateSscRequest::MsgUpdateSscRequest()
{
	CLBTH_ZERO_INIT_OBJECT(BaseMsg)
}

MsgUpdateSscRequest::~MsgUpdateSscRequest()
{
}

HRCODE MsgUpdateSscRequest::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << con_id << gpc_id << station_id << entily_id << block_cnt << car_cnt << (short)request_flag;
	}
	else
	{
		ar >> con_id >> gpc_id >> station_id >> entily_id >> block_cnt >> car_cnt;
		short flag;
		ar >> flag;
		request_flag = (SSC_UPDATE_FLAG)flag;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgUpdateSscRequest::countObjectLength()
{
	return sizeof(con_id) + sizeof(gpc_id) + sizeof(station_id) + sizeof(entily_id) + sizeof(block_cnt) + sizeof(car_cnt) + sizeof(short);
}

///////////////////////////////////////////////////////////
IMPLEMENT_CLBTH(MsgUpdateSscRequestEnd, MSGID_MsgUpdateSscRequestEnd)
IMPLEMENT_SERIALIZE_OPERATOR(MsgUpdateSscRequestEnd)
MsgUpdateSscRequestEnd::MsgUpdateSscRequestEnd()
{
	CLBTH_ZERO_INIT_OBJECT(BaseMsg)
}

MsgUpdateSscRequestEnd::~MsgUpdateSscRequestEnd()
{
}

HRCODE MsgUpdateSscRequestEnd::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << con_id << gpc_id << station_id << entily_id;
	}
	else
	{
		ar >> con_id >> gpc_id >> station_id >> entily_id;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgUpdateSscRequestEnd::countObjectLength()
{
	return sizeof(con_id) + sizeof(gpc_id) + sizeof(station_id) + sizeof(entily_id);
}

///////////////////////////////////////////////////////////
IMPLEMENT_CLBTH(MsgUpdateSscResponse, MSGID_MsgUpdateSscResponse)
IMPLEMENT_SERIALIZE_OPERATOR(MsgUpdateSscResponse)
MsgUpdateSscResponse::MsgUpdateSscResponse()
{
	CLBTH_ZERO_INIT_OBJECT(BaseMsg)
}

MsgUpdateSscResponse::~MsgUpdateSscResponse()
{
}

HRCODE MsgUpdateSscResponse::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << con_id << gpc_id << station_id << entily_id ;
	}
	else
	{
		ar >> con_id >> gpc_id >> station_id >> entily_id;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgUpdateSscResponse::countObjectLength()
{
	return sizeof(con_id) + sizeof(gpc_id) + sizeof(station_id) + sizeof(entily_id);
}

///////////////////////////////////////////////////////////
IMPLEMENT_CLBTH(MsgUpdateSscResponseEnd, MSGID_MsgUpdateSscResponseEnd)
IMPLEMENT_SERIALIZE_OPERATOR(MsgUpdateSscResponseEnd)
MsgUpdateSscResponseEnd::MsgUpdateSscResponseEnd()
{
	CLBTH_ZERO_INIT_OBJECT(BaseMsg)
}

MsgUpdateSscResponseEnd::~MsgUpdateSscResponseEnd()
{
}

HRCODE MsgUpdateSscResponseEnd::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << con_id << gpc_id << station_id << entily_id << succ_block_cnt << succ_carriage_cnt << failed_block_cnt << failed_carriage_cnt;
	}
	else
	{
		ar >> con_id >> gpc_id >> station_id >> entily_id >> succ_block_cnt >> succ_carriage_cnt >> failed_block_cnt >> failed_carriage_cnt;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgUpdateSscResponseEnd::countObjectLength()
{
	return sizeof(con_id) + sizeof(gpc_id) + sizeof(station_id) + sizeof(entily_id) + sizeof(succ_block_cnt) + sizeof(succ_carriage_cnt)
	+ sizeof(failed_block_cnt) + sizeof(failed_carriage_cnt);
}


///////////////////////////////////////////////////////////
// load ssc block
IMPLEMENT_CLBTH(MsgLoadSscBlock, MSGID_MsgLoadSscBlock)
IMPLEMENT_SERIALIZE_OPERATOR(MsgLoadSscBlock)
MsgLoadSscBlock::MsgLoadSscBlock()
{
	CLBTH_ZERO_INIT_OBJECT(BaseMsg)
}

MsgLoadSscBlock::~MsgLoadSscBlock()
{
}

HRCODE MsgLoadSscBlock::doSerialize(Archive& ar)
{
	IBYTE len;
	if(ar.isStoring())
	{
		ar << con_id << gpc_id << station_no << entily_id << side_no << index_in_side
			<< car_cnt << ht_flag << oil_type << car_empty_flag << flag << car_type;
		block_desc[sizeof(block_desc)-1]='\0';
		len = (IBYTE)strlen(block_desc);
		ar << len;
		ar.write(block_desc, len);
		freight_flag[sizeof(freight_flag)-1]='\0';
		len = (IBYTE)strlen(freight_flag);
		ar << len;
		ar.write(freight_flag, len);
		first_car_no[sizeof(first_car_no)-1]='\0';
		len = (IBYTE)strlen(first_car_no);
		ar << len;
		ar.write(first_car_no, len);
		last_car_no[sizeof(last_car_no)-1]='\0';
		len = (IBYTE)strlen(last_car_no);
		ar << len;
		ar.write(last_car_no, len);
		ar << last_edit_time;
		direction_no[sizeof(direction_no)-1]='\0';
		len = (IBYTE)strlen(direction_no);
		ar << len;
		ar.write(direction_no, len);
		end_station_code[sizeof(end_station_code)-1]='\0';
		len = (IBYTE)strlen(end_station_code);
		ar << len;
		ar.write(end_station_code, len);
		ar << end_station_no << positon;
		status[sizeof(status)-1]='\0';
		len = (IBYTE)strlen(status);
		ar << len;
		ar.write(status, len);
		work_status[sizeof(work_status)-1]='\0';
		len = (IBYTE)strlen(work_status);
		ar << len;
		ar.write(work_status, len);
		work_flag[sizeof(work_flag)-1]='\0';
		len = (IBYTE)strlen(work_flag);
		ar << len;
		ar.write(work_flag, len);
		note[sizeof(note)-1]='\0';
		len = (IBYTE)strlen(note);
		ar << len;
		ar.write(note, len);
	}
	else
	{
		ar >> con_id >> gpc_id >> station_no >> entily_id >> side_no >> index_in_side
			>> car_cnt >> ht_flag >> oil_type >> car_empty_flag >> flag >> car_type;
		ar >> len;
		ar.read(block_desc, len);
		block_desc[len]='\0';
		ar >> len;
		ar.read(freight_flag, len);
		freight_flag[len]='\0';
		ar >> len;
		ar.read(first_car_no, len);
		first_car_no[len]='\0';
		ar >> len;
		ar.read(last_car_no, len);
		last_car_no[len]='\0';
		ar >> last_edit_time;
		ar >> len;
		ar.read(direction_no, len);
		direction_no[len]='\0';
		ar >> len;
		ar.read(end_station_code, len);
		end_station_code[len]='\0';
		ar >> end_station_no >> positon;
		ar >> len;
		ar.read(status, len);
		status[len]='\0';
		ar >> len;
		ar.read(work_status, len);
		work_status[len]='\0';
		ar >> len;
		ar.read(work_flag, len);
		work_flag[len]='\0';
		ar >> len;
		ar.read(note, len);
		note[len]='\0';
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgLoadSscBlock::countObjectLength()
{
	block_desc[sizeof(block_desc)-1]='\0';
	freight_flag[sizeof(freight_flag)-1]='\0';
	first_car_no[sizeof(first_car_no)-1]='\0';
	last_car_no[sizeof(last_car_no)-1]='\0';
	direction_no[sizeof(direction_no)-1]='\0';
	end_station_code[sizeof(end_station_code)-1]='\0';
	status[sizeof(status)-1]='\0';
	work_status[sizeof(work_status)-1]='\0';
	work_flag[sizeof(work_flag)-1]='\0';
	note[sizeof(note)-1]='\0';
	return sizeof(con_id) + sizeof(gpc_id) + sizeof(station_no) + sizeof(entily_id)
		+ sizeof(side_no) + sizeof(index_in_side) + sizeof(car_cnt) + sizeof(ht_flag)
		+ sizeof(oil_type) + sizeof(car_empty_flag) + sizeof(flag) + sizeof(car_type)
		+ sizeof(IBYTE) + strlen(block_desc) + sizeof(IBYTE) + strlen(freight_flag)
		+ sizeof(IBYTE) + strlen(first_car_no) + sizeof(IBYTE) + strlen(last_car_no)
		+ sizeof(last_edit_time) + sizeof(IBYTE) + strlen(direction_no) + sizeof(IBYTE)
		+ strlen(end_station_code) + sizeof(end_station_no) + sizeof(positon)
		+ sizeof(IBYTE) + strlen(status) + sizeof(IBYTE) + strlen(work_status)
		+ sizeof(IBYTE) + strlen(work_flag) + sizeof(IBYTE) + strlen(note);
}
// Insert ssc block
IMPLEMENT_CLBTH(MsgInsertSscBlock, MSGID_MsgInsertSscBlock)
IMPLEMENT_SERIALIZE_OPERATOR(MsgInsertSscBlock)
MsgInsertSscBlock::MsgInsertSscBlock()
{
	CLBTH_ZERO_INIT_OBJECT(BaseMsg)
}

MsgInsertSscBlock::~MsgInsertSscBlock()
{
}

HRCODE MsgInsertSscBlock::doSerialize(Archive& ar)
{
	IBYTE len;
	if(ar.isStoring())
	{
		ar << con_id << gpc_id << station_no << entily_id << side_no << index_in_side
			<< car_cnt << ht_flag << oil_type << car_empty_flag << flag << car_type;
		block_desc[sizeof(block_desc)-1]='\0';
		len = (IBYTE)strlen(block_desc);
		ar << len;
		ar.write(block_desc, len);
		freight_flag[sizeof(freight_flag)-1]='\0';
		len = (IBYTE)strlen(freight_flag);
		ar << len;
		ar.write(freight_flag, len);
		first_car_no[sizeof(first_car_no)-1]='\0';
		len = (IBYTE)strlen(first_car_no);
		ar << len;
		ar.write(first_car_no, len);
		last_car_no[sizeof(last_car_no)-1]='\0';
		len = (IBYTE)strlen(last_car_no);
		ar << len;
		ar.write(last_car_no, len);
		ar << last_edit_time;
		direction_no[sizeof(direction_no)-1]='\0';
		len = (IBYTE)strlen(direction_no);
		ar << len;
		ar.write(direction_no, len);
		end_station_code[sizeof(end_station_code)-1]='\0';
		len = (IBYTE)strlen(end_station_code);
		ar << len;
		ar.write(end_station_code, len);
		ar << end_station_no << positon;
		status[sizeof(status)-1]='\0';
		len = (IBYTE)strlen(status);
		ar << len;
		ar.write(status, len);
		work_status[sizeof(work_status)-1]='\0';
		len = (IBYTE)strlen(work_status);
		ar << len;
		ar.write(work_status, len);
		work_flag[sizeof(work_flag)-1]='\0';
		len = (IBYTE)strlen(work_flag);
		ar << len;
		ar.write(work_flag, len);
		note[sizeof(note)-1]='\0';
		len = (IBYTE)strlen(note);
		ar << len;
		ar.write(note, len);
	}
	else
	{
		ar >> con_id >> gpc_id >> station_no >> entily_id >> side_no >> index_in_side
			>> car_cnt >> ht_flag >> oil_type >> car_empty_flag >> flag >> car_type;
		ar >> len;
		ar.read(block_desc, len);
		block_desc[len]='\0';
		ar >> len;
		ar.read(freight_flag, len);
		freight_flag[len]='\0';
		ar >> len;
		ar.read(first_car_no, len);
		first_car_no[len]='\0';
		ar >> len;
		ar.read(last_car_no, len);
		last_car_no[len]='\0';
		ar >> last_edit_time;
		ar >> len;
		ar.read(direction_no, len);
		direction_no[len]='\0';
		ar >> len;
		ar.read(end_station_code, len);
		end_station_code[len]='\0';
		ar >> end_station_no >> positon;
		ar >> len;
		ar.read(status, len);
		status[len]='\0';
		ar >> len;
		ar.read(work_status, len);
		work_status[len]='\0';
		ar >> len;
		ar.read(work_flag, len);
		work_flag[len]='\0';
		ar >> len;
		ar.read(note, len);
		note[len]='\0';
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgInsertSscBlock::countObjectLength()
{
	block_desc[sizeof(block_desc)-1]='\0';
	freight_flag[sizeof(freight_flag)-1]='\0';
	first_car_no[sizeof(first_car_no)-1]='\0';
	last_car_no[sizeof(last_car_no)-1]='\0';
	direction_no[sizeof(direction_no)-1]='\0';
	end_station_code[sizeof(end_station_code)-1]='\0';
	status[sizeof(status)-1]='\0';
	work_status[sizeof(work_status)-1]='\0';
	work_flag[sizeof(work_flag)-1]='\0';
	note[sizeof(note)-1]='\0';
	return sizeof(con_id) + sizeof(gpc_id) + sizeof(station_no) + sizeof(entily_id)
		+ sizeof(side_no) + sizeof(index_in_side) + sizeof(car_cnt) + sizeof(ht_flag)
		+ sizeof(oil_type) + sizeof(car_empty_flag) + sizeof(flag) + sizeof(car_type)
		+ sizeof(IBYTE) + strlen(block_desc) + sizeof(IBYTE) + strlen(freight_flag)
		+ sizeof(IBYTE) + strlen(first_car_no) + sizeof(IBYTE) + strlen(last_car_no)
		+ sizeof(last_edit_time) + sizeof(IBYTE) + strlen(direction_no) + sizeof(IBYTE)
		+ strlen(end_station_code) + sizeof(end_station_no) + sizeof(positon)
		+ sizeof(IBYTE) + strlen(status) + sizeof(IBYTE) + strlen(work_status)
		+ sizeof(IBYTE) + strlen(work_flag) + sizeof(IBYTE) + strlen(note);
}
// update ssc block
IMPLEMENT_CLBTH(MsgUpdateSscBlock, MSGID_MsgUpdateSscBlock)
IMPLEMENT_SERIALIZE_OPERATOR(MsgUpdateSscBlock)
MsgUpdateSscBlock::MsgUpdateSscBlock()
{
	CLBTH_ZERO_INIT_OBJECT(BaseMsg)
}

MsgUpdateSscBlock::~MsgUpdateSscBlock()
{
}

HRCODE MsgUpdateSscBlock::doSerialize(Archive& ar)
{
	IBYTE len;
	if(ar.isStoring())
	{
		ar << con_id << gpc_id << station_no << entily_id << side_no << index_in_side
			<< car_cnt << ht_flag << oil_type << car_empty_flag << flag << car_type;
		block_desc[sizeof(block_desc)-1]='\0';
		len = (IBYTE)strlen(block_desc);
		ar << len;
		ar.write(block_desc, len);
		freight_flag[sizeof(freight_flag)-1]='\0';
		len = (IBYTE)strlen(freight_flag);
		ar << len;
		ar.write(freight_flag, len);
		first_car_no[sizeof(first_car_no)-1]='\0';
		len = (IBYTE)strlen(first_car_no);
		ar << len;
		ar.write(first_car_no, len);
		last_car_no[sizeof(last_car_no)-1]='\0';
		len = (IBYTE)strlen(last_car_no);
		ar << len;
		ar.write(last_car_no, len);
		ar << last_edit_time;
		direction_no[sizeof(direction_no)-1]='\0';
		len = (IBYTE)strlen(direction_no);
		ar << len;
		ar.write(direction_no, len);
		end_station_code[sizeof(end_station_code)-1]='\0';
		len = (IBYTE)strlen(end_station_code);
		ar << len;
		ar.write(end_station_code, len);
		ar << end_station_no << positon;
		status[sizeof(status)-1]='\0';
		len = (IBYTE)strlen(status);
		ar << len;
		ar.write(status, len);
		work_status[sizeof(work_status)-1]='\0';
		len = (IBYTE)strlen(work_status);
		ar << len;
		ar.write(work_status, len);
		work_flag[sizeof(work_flag)-1]='\0';
		len = (IBYTE)strlen(work_flag);
		ar << len;
		ar.write(work_flag, len);
		note[sizeof(note)-1]='\0';
		len = (IBYTE)strlen(note);
		ar << len;
		ar.write(note, len);
	}
	else
	{
		ar >> con_id >> gpc_id >> station_no >> entily_id >> side_no >> index_in_side
			>> car_cnt >> ht_flag >> oil_type >> car_empty_flag >> flag >> car_type;
		ar >> len;
		ar.read(block_desc, len);
		block_desc[len]='\0';
		ar >> len;
		ar.read(freight_flag, len);
		freight_flag[len]='\0';
		ar >> len;
		ar.read(first_car_no, len);
		first_car_no[len]='\0';
		ar >> len;
		ar.read(last_car_no, len);
		last_car_no[len]='\0';
		ar >> last_edit_time;
		ar >> len;
		ar.read(direction_no, len);
		direction_no[len]='\0';
		ar >> len;
		ar.read(end_station_code, len);
		end_station_code[len]='\0';
		ar >> end_station_no >> positon;
		ar >> len;
		ar.read(status, len);
		status[len]='\0';
		ar >> len;
		ar.read(work_status, len);
		work_status[len]='\0';
		ar >> len;
		ar.read(work_flag, len);
		work_flag[len]='\0';
		ar >> len;
		ar.read(note, len);
		note[len]='\0';
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgUpdateSscBlock::countObjectLength()
{
	block_desc[sizeof(block_desc)-1]='\0';
	freight_flag[sizeof(freight_flag)-1]='\0';
	first_car_no[sizeof(first_car_no)-1]='\0';
	last_car_no[sizeof(last_car_no)-1]='\0';
	direction_no[sizeof(direction_no)-1]='\0';
	end_station_code[sizeof(end_station_code)-1]='\0';
	status[sizeof(status)-1]='\0';
	work_status[sizeof(work_status)-1]='\0';
	work_flag[sizeof(work_flag)-1]='\0';
	note[sizeof(note)-1]='\0';
	return sizeof(con_id) + sizeof(gpc_id) + sizeof(station_no) + sizeof(entily_id)
		+ sizeof(side_no) + sizeof(index_in_side) + sizeof(car_cnt) + sizeof(ht_flag)
		+ sizeof(oil_type) + sizeof(car_empty_flag) + sizeof(flag) + sizeof(car_type)
		+ sizeof(IBYTE) + strlen(block_desc) + sizeof(IBYTE) + strlen(freight_flag)
		+ sizeof(IBYTE) + strlen(first_car_no) + sizeof(IBYTE) + strlen(last_car_no)
		+ sizeof(last_edit_time) + sizeof(IBYTE) + strlen(direction_no) + sizeof(IBYTE)
		+ strlen(end_station_code) + sizeof(end_station_no) + sizeof(positon)
		+ sizeof(IBYTE) + strlen(status) + sizeof(IBYTE) + strlen(work_status)
		+ sizeof(IBYTE) + strlen(work_flag) + sizeof(IBYTE) + strlen(note);
}
// update ssc block pos
IMPLEMENT_CLBTH(MsgUpdateSscBlockPos, MSGID_MsgUpdateSscBlockPos)
IMPLEMENT_SERIALIZE_OPERATOR(MsgUpdateSscBlockPos)
MsgUpdateSscBlockPos::MsgUpdateSscBlockPos()
{
	CLBTH_ZERO_INIT_OBJECT(BaseMsg)
}

MsgUpdateSscBlockPos::~MsgUpdateSscBlockPos()
{
}

HRCODE MsgUpdateSscBlockPos::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << con_id << station_no << side_no << index_in_side << positon;
	}
	else
	{
		ar >> con_id >> station_no >> side_no >> index_in_side >> positon;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgUpdateSscBlockPos::countObjectLength()
{
	return sizeof(con_id) + sizeof(station_no) + sizeof(side_no) + sizeof(index_in_side) + sizeof(positon);
}

// update ssc block status
IMPLEMENT_CLBTH(MsgUpdateSscBlockStatus, MSGID_MsgUpdateSscBlockStatus)
IMPLEMENT_SERIALIZE_OPERATOR(MsgUpdateSscBlockStatus)
MsgUpdateSscBlockStatus::MsgUpdateSscBlockStatus()
{
	CLBTH_ZERO_INIT_OBJECT(BaseMsg)
}

MsgUpdateSscBlockStatus::~MsgUpdateSscBlockStatus()
{
}

HRCODE MsgUpdateSscBlockStatus::doSerialize(Archive& ar)
{
	IBYTE len;
	if(ar.isStoring())
	{
		ar << con_id << station_no << side_no << index_in_side
			<< ht_flag << oil_type << car_empty_flag << flag;
		status[sizeof(status)-1]='\0';
		len = (IBYTE)strlen(status);
		ar << len;
		ar.write(status, len);
		work_status[sizeof(work_status)-1]='\0';
		len = (IBYTE)strlen(work_status);
		ar << len;
		ar.write(work_status, len);
		work_flag[sizeof(work_flag)-1]='\0';
		len = (IBYTE)strlen(work_flag);
		ar << len;
		ar.write(work_flag, len);
	}
	else
	{
		ar >> con_id >> station_no >> side_no >> index_in_side
			>> ht_flag >> oil_type >> car_empty_flag >> flag;
		ar >> len;
		ar.read(status, len);
		status[len]='\0';
		ar >> len;
		ar.read(work_status, len);
		work_status[len]='\0';
		ar >> len;
		ar.read(work_flag, len);
		work_flag[len]='\0';
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgUpdateSscBlockStatus::countObjectLength()
{
	status[sizeof(status)-1]='\0';
	work_status[sizeof(work_status)-1]='\0';
	work_flag[sizeof(work_flag)-1]='\0';
	return sizeof(con_id) + sizeof(station_no) + sizeof(side_no) + sizeof(index_in_side) + sizeof(ht_flag)
		+ sizeof(oil_type) + sizeof(car_empty_flag) + sizeof(flag) + sizeof(IBYTE) + strlen(status)
		+ sizeof(IBYTE) + strlen(work_status) + sizeof(IBYTE) + strlen(work_flag);
}
// update ssc block
IMPLEMENT_CLBTH(MsgUpdateSscBlockNote, MSGID_MsgUpdateSscBlockNote)
IMPLEMENT_SERIALIZE_OPERATOR(MsgUpdateSscBlockNote)
MsgUpdateSscBlockNote::MsgUpdateSscBlockNote()
{
	CLBTH_ZERO_INIT_OBJECT(BaseMsg)
}

MsgUpdateSscBlockNote::~MsgUpdateSscBlockNote()
{
}

HRCODE MsgUpdateSscBlockNote::doSerialize(Archive& ar)
{
	IBYTE len;
	if(ar.isStoring())
	{
		ar << con_id << station_no << side_no << index_in_side;
		freight_flag[sizeof(freight_flag)-1]='\0';
		len = (IBYTE)strlen(freight_flag);
		ar << len;
		ar.write(freight_flag, len);
		note[sizeof(note)-1]='\0';
		len = (IBYTE)strlen(note);
		ar << len;
		ar.write(note, len);
	}
	else
	{
		ar >> con_id >> station_no >> side_no >> index_in_side;
		ar >> len;
		ar.read(freight_flag, len);
		freight_flag[len]='\0';
		ar >> len;
		ar.read(note, len);
		note[len]='\0';
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgUpdateSscBlockNote::countObjectLength()
{
	freight_flag[sizeof(freight_flag)-1]='\0';
	note[sizeof(note)-1]='\0';
	return sizeof(con_id) + sizeof(station_no) + sizeof(side_no) + sizeof(index_in_side)
		+ sizeof(IBYTE) + strlen(freight_flag) + sizeof(IBYTE) + strlen(note);
}
// delete ssc block
IMPLEMENT_CLBTH(MsgDeleteSscBlock, MSGID_MsgDeleteSscBlock)
IMPLEMENT_SERIALIZE_OPERATOR(MsgDeleteSscBlock)
MsgDeleteSscBlock::MsgDeleteSscBlock()
{
	CLBTH_ZERO_INIT_OBJECT(BaseMsg)
}

MsgDeleteSscBlock::~MsgDeleteSscBlock()
{
}

HRCODE MsgDeleteSscBlock::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << con_id << station_no << side_no << index_in_side;
	}
	else
	{
		ar >> con_id >> station_no >> side_no >> index_in_side;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgDeleteSscBlock::countObjectLength()
{
	return sizeof(con_id) + sizeof(station_no) + sizeof(side_no) + sizeof(index_in_side);
}
// carriage oper result
IMPLEMENT_CLBTH(MsgOperBlockReaponse, MSGID_MsgOperBlockReaponse)
IMPLEMENT_SERIALIZE_OPERATOR(MsgOperBlockReaponse)
MsgOperBlockReaponse::MsgOperBlockReaponse()
{
	CLBTH_ZERO_INIT_OBJECT(BaseMsg)
}

MsgOperBlockReaponse::~MsgOperBlockReaponse()
{
}

HRCODE MsgOperBlockReaponse::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << con_id << station_no << side_no << index_in_side << (short)del_update_block_rs;
	}
	else
	{
		ar >> con_id >> station_no >> side_no >> index_in_side;
		short flag;
		ar >> flag;
		del_update_block_rs = (DEL_UPDATE_SSC_BLOCK_RESULT)flag;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgOperBlockReaponse::countObjectLength()
{
	return sizeof(con_id) + sizeof(station_no) + sizeof(side_no) + sizeof(index_in_side) + sizeof(short);
}

///////////////////////////////////////////////////////////
// load ssc carriage
IMPLEMENT_CLBTH(MsgLoadSscCarriage, MSGID_MsgLoadSscCarriage)
IMPLEMENT_SERIALIZE_OPERATOR(MsgLoadSscCarriage)
MsgLoadSscCarriage::MsgLoadSscCarriage()
{
	CLBTH_ZERO_INIT_OBJECT(BaseMsg)
}

MsgLoadSscCarriage::~MsgLoadSscCarriage()
{
}

HRCODE MsgLoadSscCarriage::doSerialize(Archive& ar)
{
	IBYTE len;
	if(ar.isStoring())
	{
		ar << con_id << station_no << side_no << block_index_in_side << car_index_in_block;
		car_no[sizeof(car_no)-1] = '\0';
		len = (IBYTE)strlen(car_no);
		ar << len;
		ar.write(car_no, len);
		ar << car_type_id;
		car_type[sizeof(car_type)-1] = '\0';
		len = (IBYTE)strlen(car_type);
		ar << len;
		ar.write(car_type, len);
		car_type_code[sizeof(car_type_code)-1] = '\0';
		len = (IBYTE)strlen(car_type_code);
		ar << len;
		ar.write(car_type_code, len);
		ar << oil_type << load_flag << car_suttle << car_len_cnt << car_load << first_station_no;
		first_station_code[sizeof(first_station_code)-1]='\0';
		len = (IBYTE)strlen(first_station_code);
		ar << len;
		ar.write(first_station_code, len);
		ar << last_station_no;
		last_station_code[sizeof(last_station_code)-1]='\0';
		len = (IBYTE)strlen(last_station_code);
		ar << len;
		ar.write(last_station_code, len);
		freight_name[sizeof(freight_name)-1]='\0';
		len = (IBYTE)strlen(freight_name);
		ar << len;
		ar.write(freight_name, len);
		freight_code[sizeof(freight_code)-1]='\0';
		len = (IBYTE)strlen(freight_code);
		ar << len;
		ar.write(freight_code, len);
		receiver[sizeof(receiver)-1]='\0';
		len = (IBYTE)strlen(receiver);
		ar << len;
		ar.write(receiver, len);
		depart_station_code[sizeof(depart_station_code)-1]='\0';
		len = (IBYTE)strlen(depart_station_code);
		ar << len;
		ar.write(depart_station_code, len);
		ar << depart_station_no << car_van_cnt;
		car_note[sizeof(car_note)-1]='\0';
		len = (IBYTE)strlen(car_note);
		ar << len;
		ar.write(car_note, len);
		container_type_1_cnt[sizeof(container_type_1_cnt)-1]='\0';
		len = (IBYTE)strlen(container_type_1_cnt);
		ar << len;
		ar.write(container_type_1_cnt, len);
		weight_container_1_cnt[sizeof(weight_container_1_cnt)-1]='\0';
		len = (IBYTE)strlen(weight_container_1_cnt);
		ar << len;
		ar.write(weight_container_1_cnt, len);
		empty_contrainer_1_cnt[sizeof(empty_contrainer_1_cnt)-1]='\0';
		len = (IBYTE)strlen(empty_contrainer_1_cnt);
		ar << len;
		ar.write(empty_contrainer_1_cnt, len);
		container_type_2_cnt[sizeof(container_type_2_cnt)-1]='\0';
		len = (IBYTE)strlen(container_type_2_cnt);
		ar << len;
		ar.write(container_type_2_cnt, len);
		weight_container_2_cnt[sizeof(weight_container_2_cnt)-1]='\0';
		len = (IBYTE)strlen(weight_container_2_cnt);
		ar << len;
		ar.write(weight_container_2_cnt, len);
		empty_contrainer_2_cnt[sizeof(empty_contrainer_2_cnt)-1]='\0';
		len = (IBYTE)strlen(empty_contrainer_2_cnt);
		ar << len;
		ar.write(empty_contrainer_2_cnt, len);
		car_empty_weight_flag[sizeof(car_empty_weight_flag)-1] = '\0';
		len = (IBYTE)strlen(car_empty_weight_flag);
		ar << len;
		ar.write(car_empty_weight_flag, len);
		first_station_name[sizeof(first_station_name)-1] = '\0';
		len = (IBYTE)strlen(first_station_name);
		ar << len;
		ar.write(first_station_name, len);
		last_station_name[sizeof(last_station_name)-1] = '\0';
		len = (IBYTE)strlen(last_station_name);
		ar << len;
		ar.write(last_station_name, len);
		depart_station_name[sizeof(depart_station_name)-1] = '\0';
		len = (IBYTE)strlen(depart_station_name);
		ar << len;
		ar.write(depart_station_name, len);
		aim_bureau[sizeof(aim_bureau)-1] = '\0';
		len = (IBYTE)strlen(aim_bureau);
		ar << len;
		ar.write(aim_bureau, len);
		aim_subbereau[sizeof(aim_subbereau)-1] = '\0';
		len = (IBYTE)strlen(aim_subbereau);
		ar << len;
		ar.write(aim_subbereau, len);
		direction_no[sizeof(direction_no)-1] = '\0';
		len = (IBYTE)strlen(direction_no);
		ar << len;
		ar.write(direction_no, len);
		freight_no_use_code[sizeof(freight_no_use_code)-1] = '\0';
		len = (IBYTE)strlen(freight_no_use_code);
		ar << len;
		ar.write(freight_no_use_code, len);
		rail_company_flag[sizeof(rail_company_flag)-1] = '\0';
		len = (IBYTE)strlen(rail_company_flag);
		ar << len;
		ar.write(rail_company_flag, len);
		no_count_flag[sizeof(no_count_flag)-1] = '\0';
		len = (IBYTE)strlen(no_count_flag);
		ar << len;
		ar.write(no_count_flag, len);
		current_status[sizeof(current_status)-1] = '\0';
		len = (IBYTE)strlen(current_status);
		ar << len;
		ar.write(current_status, len);
		ar << arrive_time << arrive_train_index;
		arrive_train_no[sizeof(arrive_train_no)-1] = '\0';
		len = (IBYTE)strlen(arrive_train_no);
		ar << len;
		ar.write(arrive_train_no, len);
		ar << arrive_flag << arrive_reason << work_complete_time << depart_time << depart_train_index << depart_flag;
	}
	else
	{
		ar >> con_id >> station_no >> side_no >> block_index_in_side >> car_index_in_block;
		ar >> len;
		ar.read(car_no, len);
		car_no[len] = '\0';
		ar >> car_type_id;
		ar >> len;
		ar.read(car_type, len);
		car_type[len]='\0';
		ar >> len;
		ar.read(car_type_code, len);
		car_type_code[len]='\0';
		ar >> oil_type >> load_flag >> car_suttle >> car_len_cnt >> car_load >> first_station_no;
		ar >> len;
		ar.read(first_station_code, len);
		first_station_code[len]='\0';
		ar >> last_station_no;
		ar >> len;
		ar.read(last_station_code, len);
		last_station_code[len]='\0';
		ar >> len;
		ar.read(freight_name, len);
		freight_name[len]='\0';
		ar >> len;
		ar.read(freight_code, len);
		freight_code[len]='\0';
		ar >> len;
		ar.read(receiver, len);
		receiver[len]='\0';
		ar >> len;
		ar.read(depart_station_code, len);
		depart_station_code[len]='\0';
		ar >> depart_station_no >> car_van_cnt;
		ar >> len;
		ar.read(car_note, len);
		car_note[len]='\0';
		ar >> len;
		ar.read(container_type_1_cnt, len);
		container_type_1_cnt[len]='\0';
		ar >> len;
		ar.read(weight_container_1_cnt, len);
		weight_container_1_cnt[len]='\0';
		ar >> len;
		ar.read(empty_contrainer_1_cnt, len);
		empty_contrainer_1_cnt[len]='\0';
		ar >> len;
		ar.read(container_type_2_cnt, len);
		container_type_2_cnt[len]='\0';
		ar >> len;
		ar.read(weight_container_2_cnt, len);
		weight_container_2_cnt[len]='\0';
		ar >> len;
		ar.read(empty_contrainer_2_cnt, len);
		empty_contrainer_2_cnt[len]='\0';
		ar >> len;
		ar.read(car_empty_weight_flag, len);
		car_empty_weight_flag[len]='\0';
		ar >> len;
		ar.read(first_station_name, len);
		first_station_name[len] = '\0';
		ar >> len;
		ar.read(last_station_name, len);
		last_station_name[len] = '\0';
		ar >> len;
		ar.read(depart_station_name, len);
		depart_station_name[len] = '\0';
		ar >> len;
		ar.read(aim_bureau, len);
		aim_bureau[len] = '\0';
		ar >> len;
		ar.read(aim_subbereau, len);
		aim_subbereau[len] = '\0';
		ar >> len;
		ar.read(direction_no, len);
		direction_no[len] = '\0';
		ar >> len;
		ar.read(freight_no_use_code, len);
		freight_no_use_code[len] = '\0';
		ar >> len;
		ar.read(rail_company_flag, len);
		rail_company_flag[len] = '\0';
		ar >> len;
		ar.read(no_count_flag, len);
		no_count_flag[len] = '\0';
		ar >> len;
		ar.read(current_status, len);
		current_status[len] = '\0';
		ar >> arrive_time >> arrive_train_index;
		ar >> len;
		ar.read(arrive_train_no, len);
		arrive_train_no[len] = '\0';
		ar >> arrive_flag >> arrive_reason >> work_complete_time
			>> depart_time >> depart_train_index >> depart_flag;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgLoadSscCarriage::countObjectLength()
{
	car_no[sizeof(car_no)-1] = '\0';
	car_type[sizeof(car_type)-1]='\0';
	car_type_code[sizeof(car_type_code)-1]='\0';
	first_station_code[sizeof(first_station_code)-1]='\0';
	last_station_code[sizeof(last_station_code)-1]='\0';
	freight_name[sizeof(freight_name)-1]='\0';
	freight_code[sizeof(freight_code)-1]='\0';
	receiver[sizeof(receiver)-1]='\0';
	depart_station_code[sizeof(depart_station_code)-1]='\0';
	car_note[sizeof(car_note)-1]='\0';
	container_type_1_cnt[sizeof(container_type_1_cnt)-1]='\0';
	weight_container_1_cnt[sizeof(weight_container_1_cnt)-1]='\0';
	empty_contrainer_1_cnt[sizeof(empty_contrainer_1_cnt)-1]='\0';
	container_type_2_cnt[sizeof(container_type_2_cnt)-1]='\0';
	weight_container_2_cnt[sizeof(weight_container_2_cnt)-1]='\0';
	empty_contrainer_2_cnt[sizeof(empty_contrainer_2_cnt)-1]='\0';
	car_empty_weight_flag[sizeof(car_empty_weight_flag)-1] = '\0';
	first_station_name[sizeof(first_station_name)-1] = '\0';
	last_station_name[sizeof(last_station_name)-1] = '\0';
	depart_station_name[sizeof(depart_station_name)-1] = '\0';
	aim_bureau[sizeof(aim_bureau)-1] = '\0';
	aim_subbereau[sizeof(aim_subbereau)-1] = '\0';
	direction_no[sizeof(direction_no)-1] = '\0';
	freight_no_use_code[sizeof(freight_no_use_code)-1] = '\0';
	rail_company_flag[sizeof(rail_company_flag)-1] = '\0';
	no_count_flag[sizeof(no_count_flag)-1] = '\0';
	current_status[sizeof(current_status)-1] = '\0';
	arrive_train_no[sizeof(arrive_train_no)-1] = '\0';
	return sizeof(con_id) + sizeof(station_no) + sizeof(side_no) + sizeof(block_index_in_side)
		+ sizeof(car_index_in_block) + sizeof(IBYTE) + strlen(car_no) + sizeof(car_type_id)
		+ sizeof(IBYTE) + strlen(car_type) + sizeof(IBYTE) + strlen(car_type_code)
		+ sizeof(oil_type) + sizeof(load_flag) + sizeof(car_suttle) + sizeof(car_len_cnt)
		+ sizeof(car_load) + sizeof(first_station_no) + sizeof(IBYTE) + strlen(first_station_code)
		+ sizeof(last_station_no) + sizeof(IBYTE) + strlen(last_station_code)
		+ sizeof(IBYTE) + strlen(freight_name) + sizeof(IBYTE) + strlen(freight_code)
		+ sizeof(IBYTE) + strlen(receiver) + sizeof(IBYTE) + strlen(depart_station_code)
		+ sizeof(depart_station_no) + sizeof(car_van_cnt) + sizeof(IBYTE) + strlen(car_note)
		+ sizeof(IBYTE) + strlen(container_type_1_cnt) + sizeof(IBYTE) + strlen(weight_container_1_cnt)
		+ sizeof(IBYTE) + strlen(empty_contrainer_1_cnt) + sizeof(IBYTE) + strlen(container_type_2_cnt)
		+ sizeof(IBYTE) + strlen(weight_container_2_cnt) + sizeof(IBYTE) + strlen(empty_contrainer_2_cnt)
		+ sizeof(IBYTE) + strlen(car_empty_weight_flag) + sizeof(IBYTE) + strlen(first_station_name)
		+ sizeof(IBYTE) + strlen(last_station_name) + sizeof(IBYTE) + strlen(depart_station_name)
		+ sizeof(IBYTE) + strlen(aim_bureau) + sizeof(IBYTE) + strlen(aim_subbereau)
		+ sizeof(IBYTE) + strlen(direction_no) + sizeof(IBYTE) + strlen(freight_no_use_code)
		+ sizeof(IBYTE) + strlen(rail_company_flag) + sizeof(IBYTE) + strlen(no_count_flag)
		+ sizeof(IBYTE) + strlen(current_status) + sizeof(arrive_time) + sizeof(arrive_train_index)
		+ sizeof(IBYTE) + sizeof(arrive_train_no) + sizeof(arrive_flag) + sizeof(arrive_reason)
		+ sizeof(work_complete_time) + sizeof(depart_time) + sizeof(depart_train_index) + sizeof(depart_flag);
}
// insert ssc carriage
IMPLEMENT_CLBTH(MsgInsertSscCarriage, MSGID_MsgInsertSscCarriage)
IMPLEMENT_SERIALIZE_OPERATOR(MsgInsertSscCarriage)
MsgInsertSscCarriage::MsgInsertSscCarriage()
{
	CLBTH_ZERO_INIT_OBJECT(BaseMsg)
}

MsgInsertSscCarriage::~MsgInsertSscCarriage()
{
}

HRCODE MsgInsertSscCarriage::doSerialize(Archive& ar)
{
	IBYTE len;
	if(ar.isStoring())
	{
		ar << con_id << station_no << side_no << block_index_in_side << car_index_in_block;
		car_no[sizeof(car_no)-1] = '\0';
		len = (IBYTE)strlen(car_no);
		ar << len;
		ar.write(car_no, len);
		ar << car_type_id;
		car_type[sizeof(car_type)-1] = '\0';
		len = (IBYTE)strlen(car_type);
		ar << len;
		ar.write(car_type, len);
		car_type_code[sizeof(car_type_code)-1] = '\0';
		len = (IBYTE)strlen(car_type_code);
		ar << len;
		ar.write(car_type_code, len);
		ar << oil_type << load_flag << car_suttle << car_len_cnt << car_load << first_station_no;
		first_station_code[sizeof(first_station_code)-1]='\0';
		len = (IBYTE)strlen(first_station_code);
		ar << len;
		ar.write(first_station_code, len);
		ar << last_station_no;
		last_station_code[sizeof(last_station_code)-1]='\0';
		len = (IBYTE)strlen(last_station_code);
		ar << len;
		ar.write(last_station_code, len);
		freight_name[sizeof(freight_name)-1]='\0';
		len = (IBYTE)strlen(freight_name);
		ar << len;
		ar.write(freight_name, len);
		freight_code[sizeof(freight_code)-1]='\0';
		len = (IBYTE)strlen(freight_code);
		ar << len;
		ar.write(freight_code, len);
		receiver[sizeof(receiver)-1]='\0';
		len = (IBYTE)strlen(receiver);
		ar << len;
		ar.write(receiver, len);
		depart_station_code[sizeof(depart_station_code)-1]='\0';
		len = (IBYTE)strlen(depart_station_code);
		ar << len;
		ar.write(depart_station_code, len);
		ar << depart_station_no << car_van_cnt;
		car_note[sizeof(car_note)-1]='\0';
		len = (IBYTE)strlen(car_note);
		ar << len;
		ar.write(car_note, len);
		container_type_1_cnt[sizeof(container_type_1_cnt)-1]='\0';
		len = (IBYTE)strlen(container_type_1_cnt);
		ar << len;
		ar.write(container_type_1_cnt, len);
		weight_container_1_cnt[sizeof(weight_container_1_cnt)-1]='\0';
		len = (IBYTE)strlen(weight_container_1_cnt);
		ar << len;
		ar.write(weight_container_1_cnt, len);
		empty_contrainer_1_cnt[sizeof(empty_contrainer_1_cnt)-1]='\0';
		len = (IBYTE)strlen(empty_contrainer_1_cnt);
		ar << len;
		ar.write(empty_contrainer_1_cnt, len);
		container_type_2_cnt[sizeof(container_type_2_cnt)-1]='\0';
		len = (IBYTE)strlen(container_type_2_cnt);
		ar << len;
		ar.write(container_type_2_cnt, len);
		weight_container_2_cnt[sizeof(weight_container_2_cnt)-1]='\0';
		len = (IBYTE)strlen(weight_container_2_cnt);
		ar << len;
		ar.write(weight_container_2_cnt, len);
		empty_contrainer_2_cnt[sizeof(empty_contrainer_2_cnt)-1]='\0';
		len = (IBYTE)strlen(empty_contrainer_2_cnt);
		ar << len;
		ar.write(empty_contrainer_2_cnt, len);
		car_empty_weight_flag[sizeof(car_empty_weight_flag)-1] = '\0';
		len = (IBYTE)strlen(car_empty_weight_flag);
		ar << len;
		ar.write(car_empty_weight_flag, len);
		first_station_name[sizeof(first_station_name)-1] = '\0';
		len = (IBYTE)strlen(first_station_name);
		ar << len;
		ar.write(first_station_name, len);
		last_station_name[sizeof(last_station_name)-1] = '\0';
		len = (IBYTE)strlen(last_station_name);
		ar << len;
		ar.write(last_station_name, len);
		depart_station_name[sizeof(depart_station_name)-1] = '\0';
		len = (IBYTE)strlen(depart_station_name);
		ar << len;
		ar.write(depart_station_name, len);
		aim_bureau[sizeof(aim_bureau)-1] = '\0';
		len = (IBYTE)strlen(aim_bureau);
		ar << len;
		ar.write(aim_bureau, len);
		aim_subbereau[sizeof(aim_subbereau)-1] = '\0';
		len = (IBYTE)strlen(aim_subbereau);
		ar << len;
		ar.write(aim_subbereau, len);
		direction_no[sizeof(direction_no)-1] = '\0';
		len = (IBYTE)strlen(direction_no);
		ar << len;
		ar.write(direction_no, len);
		freight_no_use_code[sizeof(freight_no_use_code)-1] = '\0';
		len = (IBYTE)strlen(freight_no_use_code);
		ar << len;
		ar.write(freight_no_use_code, len);
		rail_company_flag[sizeof(rail_company_flag)-1] = '\0';
		len = (IBYTE)strlen(rail_company_flag);
		ar << len;
		ar.write(rail_company_flag, len);
		no_count_flag[sizeof(no_count_flag)-1] = '\0';
		len = (IBYTE)strlen(no_count_flag);
		ar << len;
		ar.write(no_count_flag, len);
		current_status[sizeof(current_status)-1] = '\0';
		len = (IBYTE)strlen(current_status);
		ar << len;
		ar.write(current_status, len);
		ar << arrive_time << arrive_train_index;
		arrive_train_no[sizeof(arrive_train_no)-1] = '\0';
		len = (IBYTE)strlen(arrive_train_no);
		ar << len;
		ar.write(arrive_train_no, len);
		ar << arrive_flag << arrive_reason << work_complete_time << depart_time << depart_train_index << depart_flag;
	}
	else
	{
		ar >> con_id >> station_no >> side_no >> block_index_in_side >> car_index_in_block;
		ar >> len;
		ar.read(car_no, len);
		car_no[len] = '\0';
		ar >> car_type_id;
		ar >> len;
		ar.read(car_type, len);
		car_type[len]='\0';
		ar >> len;
		ar.read(car_type_code, len);
		car_type_code[len]='\0';
		ar >> oil_type >> load_flag >> car_suttle >> car_len_cnt >> car_load >> first_station_no;
		ar >> len;
		ar.read(first_station_code, len);
		first_station_code[len]='\0';
		ar >> last_station_no;
		ar >> len;
		ar.read(last_station_code, len);
		last_station_code[len]='\0';
		ar >> len;
		ar.read(freight_name, len);
		freight_name[len]='\0';
		ar >> len;
		ar.read(freight_code, len);
		freight_code[len]='\0';
		ar >> len;
		ar.read(receiver, len);
		receiver[len]='\0';
		ar >> len;
		ar.read(depart_station_code, len);
		depart_station_code[len]='\0';
		ar >> depart_station_no >> car_van_cnt;
		ar >> len;
		ar.read(car_note, len);
		car_note[len]='\0';
		ar >> len;
		ar.read(container_type_1_cnt, len);
		container_type_1_cnt[len]='\0';
		ar >> len;
		ar.read(weight_container_1_cnt, len);
		weight_container_1_cnt[len]='\0';
		ar >> len;
		ar.read(empty_contrainer_1_cnt, len);
		empty_contrainer_1_cnt[len]='\0';
		ar >> len;
		ar.read(container_type_2_cnt, len);
		container_type_2_cnt[len]='\0';
		ar >> len;
		ar.read(weight_container_2_cnt, len);
		weight_container_2_cnt[len]='\0';
		ar >> len;
		ar.read(empty_contrainer_2_cnt, len);
		empty_contrainer_2_cnt[len]='\0';
		ar >> len;
		ar.read(car_empty_weight_flag, len);
		car_empty_weight_flag[len]='\0';
		ar >> len;
		ar.read(first_station_name, len);
		first_station_name[len] = '\0';
		ar >> len;
		ar.read(last_station_name, len);
		last_station_name[len] = '\0';
		ar >> len;
		ar.read(depart_station_name, len);
		depart_station_name[len] = '\0';
		ar >> len;
		ar.read(aim_bureau, len);
		aim_bureau[len] = '\0';
		ar >> len;
		ar.read(aim_subbereau, len);
		aim_subbereau[len] = '\0';
		ar >> len;
		ar.read(direction_no, len);
		direction_no[len] = '\0';
		ar >> len;
		ar.read(freight_no_use_code, len);
		freight_no_use_code[len] = '\0';
		ar >> len;
		ar.read(rail_company_flag, len);
		rail_company_flag[len] = '\0';
		ar >> len;
		ar.read(no_count_flag, len);
		no_count_flag[len] = '\0';
		ar >> len;
		ar.read(current_status, len);
		current_status[len] = '\0';
		ar >> arrive_time >> arrive_train_index;
		ar >> len;
		ar.read(arrive_train_no, len);
		arrive_train_no[len] = '\0';
		ar >> arrive_flag >> arrive_reason >> work_complete_time
			>> depart_time >> depart_train_index >> depart_flag;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgInsertSscCarriage::countObjectLength()
{
	car_no[sizeof(car_no)-1] = '\0';
	car_type[sizeof(car_type)-1]='\0';
	car_type_code[sizeof(car_type_code)-1]='\0';
	first_station_code[sizeof(first_station_code)-1]='\0';
	last_station_code[sizeof(last_station_code)-1]='\0';
	freight_name[sizeof(freight_name)-1]='\0';
	freight_code[sizeof(freight_code)-1]='\0';
	receiver[sizeof(receiver)-1]='\0';
	depart_station_code[sizeof(depart_station_code)-1]='\0';
	car_note[sizeof(car_note)-1]='\0';
	container_type_1_cnt[sizeof(container_type_1_cnt)-1]='\0';
	weight_container_1_cnt[sizeof(weight_container_1_cnt)-1]='\0';
	empty_contrainer_1_cnt[sizeof(empty_contrainer_1_cnt)-1]='\0';
	container_type_2_cnt[sizeof(container_type_2_cnt)-1]='\0';
	weight_container_2_cnt[sizeof(weight_container_2_cnt)-1]='\0';
	empty_contrainer_2_cnt[sizeof(empty_contrainer_2_cnt)-1]='\0';
	car_empty_weight_flag[sizeof(car_empty_weight_flag)-1] = '\0';
	first_station_name[sizeof(first_station_name)-1] = '\0';
	last_station_name[sizeof(last_station_name)-1] = '\0';
	depart_station_name[sizeof(depart_station_name)-1] = '\0';
	aim_bureau[sizeof(aim_bureau)-1] = '\0';
	aim_subbereau[sizeof(aim_subbereau)-1] = '\0';
	direction_no[sizeof(direction_no)-1] = '\0';
	freight_no_use_code[sizeof(freight_no_use_code)-1] = '\0';
	rail_company_flag[sizeof(rail_company_flag)-1] = '\0';
	no_count_flag[sizeof(no_count_flag)-1] = '\0';
	current_status[sizeof(current_status)-1] = '\0';
	arrive_train_no[sizeof(arrive_train_no)-1] = '\0';
	return sizeof(con_id) + sizeof(station_no) + sizeof(side_no) + sizeof(block_index_in_side)
		+ sizeof(car_index_in_block) + sizeof(IBYTE) + strlen(car_no) + sizeof(car_type_id)
		+ sizeof(IBYTE) + strlen(car_type) + sizeof(IBYTE) + strlen(car_type_code)
		+ sizeof(oil_type) + sizeof(load_flag) + sizeof(car_suttle) + sizeof(car_len_cnt)
		+ sizeof(car_load) + sizeof(first_station_no) + sizeof(IBYTE) + strlen(first_station_code)
		+ sizeof(last_station_no) + sizeof(IBYTE) + strlen(last_station_code)
		+ sizeof(IBYTE) + strlen(freight_name) + sizeof(IBYTE) + strlen(freight_code)
		+ sizeof(IBYTE) + strlen(receiver) + sizeof(IBYTE) + strlen(depart_station_code)
		+ sizeof(depart_station_no) + sizeof(car_van_cnt) + sizeof(IBYTE) + strlen(car_note)
		+ sizeof(IBYTE) + strlen(container_type_1_cnt) + sizeof(IBYTE) + strlen(weight_container_1_cnt)
		+ sizeof(IBYTE) + strlen(empty_contrainer_1_cnt) + sizeof(IBYTE) + strlen(container_type_2_cnt)
		+ sizeof(IBYTE) + strlen(weight_container_2_cnt) + sizeof(IBYTE) + strlen(empty_contrainer_2_cnt)
		+ sizeof(IBYTE) + strlen(car_empty_weight_flag) + sizeof(IBYTE) + strlen(first_station_name)
		+ sizeof(IBYTE) + strlen(last_station_name) + sizeof(IBYTE) + strlen(depart_station_name)
		+ sizeof(IBYTE) + strlen(aim_bureau) + sizeof(IBYTE) + strlen(aim_subbereau)
		+ sizeof(IBYTE) + strlen(direction_no) + sizeof(IBYTE) + strlen(freight_no_use_code)
		+ sizeof(IBYTE) + strlen(rail_company_flag) + sizeof(IBYTE) + strlen(no_count_flag)
		+ sizeof(IBYTE) + strlen(current_status) + sizeof(arrive_time) + sizeof(arrive_train_index)
		+ sizeof(IBYTE) + sizeof(arrive_train_no) + sizeof(arrive_flag) + sizeof(arrive_reason)
		+ sizeof(work_complete_time) + sizeof(depart_time) + sizeof(depart_train_index) + sizeof(depart_flag);
}
// update ssc carriage
IMPLEMENT_CLBTH(MsgUpdateSscCarriage, MSGID_MsgUpdateSscCarriage)
IMPLEMENT_SERIALIZE_OPERATOR(MsgUpdateSscCarriage)
MsgUpdateSscCarriage::MsgUpdateSscCarriage()
{
	CLBTH_ZERO_INIT_OBJECT(BaseMsg)
}

MsgUpdateSscCarriage::~MsgUpdateSscCarriage()
{
}

HRCODE MsgUpdateSscCarriage::doSerialize(Archive& ar)
{
	IBYTE len;
	if(ar.isStoring())
	{
		ar << con_id << station_no << side_no << block_index_in_side << car_index_in_block;
		car_no[sizeof(car_no)-1] = '\0';
		len = (IBYTE)strlen(car_no);
		ar << len;
		ar.write(car_no, len);
		ar << car_type_id;
		car_type[sizeof(car_type)-1] = '\0';
		len = (IBYTE)strlen(car_type);
		ar << len;
		ar.write(car_type, len);
		car_type_code[sizeof(car_type_code)-1] = '\0';
		len = (IBYTE)strlen(car_type_code);
		ar << len;
		ar.write(car_type_code, len);
		ar << oil_type << load_flag << car_suttle << car_len_cnt << car_load << first_station_no;
		first_station_code[sizeof(first_station_code)-1]='\0';
		len = (IBYTE)strlen(first_station_code);
		ar << len;
		ar.write(first_station_code, len);
		ar << last_station_no;
		last_station_code[sizeof(last_station_code)-1]='\0';
		len = (IBYTE)strlen(last_station_code);
		ar << len;
		ar.write(last_station_code, len);
		freight_name[sizeof(freight_name)-1]='\0';
		len = (IBYTE)strlen(freight_name);
		ar << len;
		ar.write(freight_name, len);
		freight_code[sizeof(freight_code)-1]='\0';
		len = (IBYTE)strlen(freight_code);
		ar << len;
		ar.write(freight_code, len);
		receiver[sizeof(receiver)-1]='\0';
		len = (IBYTE)strlen(receiver);
		ar << len;
		ar.write(receiver, len);
		depart_station_code[sizeof(depart_station_code)-1]='\0';
		len = (IBYTE)strlen(depart_station_code);
		ar << len;
		ar.write(depart_station_code, len);
		ar << depart_station_no << car_van_cnt;
		car_note[sizeof(car_note)-1]='\0';
		len = (IBYTE)strlen(car_note);
		ar << len;
		ar.write(car_note, len);
		container_type_1_cnt[sizeof(container_type_1_cnt)-1]='\0';
		len = (IBYTE)strlen(container_type_1_cnt);
		ar << len;
		ar.write(container_type_1_cnt, len);
		weight_container_1_cnt[sizeof(weight_container_1_cnt)-1]='\0';
		len = (IBYTE)strlen(weight_container_1_cnt);
		ar << len;
		ar.write(weight_container_1_cnt, len);
		empty_contrainer_1_cnt[sizeof(empty_contrainer_1_cnt)-1]='\0';
		len = (IBYTE)strlen(empty_contrainer_1_cnt);
		ar << len;
		ar.write(empty_contrainer_1_cnt, len);
		container_type_2_cnt[sizeof(container_type_2_cnt)-1]='\0';
		len = (IBYTE)strlen(container_type_2_cnt);
		ar << len;
		ar.write(container_type_2_cnt, len);
		weight_container_2_cnt[sizeof(weight_container_2_cnt)-1]='\0';
		len = (IBYTE)strlen(weight_container_2_cnt);
		ar << len;
		ar.write(weight_container_2_cnt, len);
		empty_contrainer_2_cnt[sizeof(empty_contrainer_2_cnt)-1]='\0';
		len = (IBYTE)strlen(empty_contrainer_2_cnt);
		ar << len;
		ar.write(empty_contrainer_2_cnt, len);
		car_empty_weight_flag[sizeof(car_empty_weight_flag)-1] = '\0';
		len = (IBYTE)strlen(car_empty_weight_flag);
		ar << len;
		ar.write(car_empty_weight_flag, len);
		first_station_name[sizeof(first_station_name)-1] = '\0';
		len = (IBYTE)strlen(first_station_name);
		ar << len;
		ar.write(first_station_name, len);
		last_station_name[sizeof(last_station_name)-1] = '\0';
		len = (IBYTE)strlen(last_station_name);
		ar << len;
		ar.write(last_station_name, len);
		depart_station_name[sizeof(depart_station_name)-1] = '\0';
		len = (IBYTE)strlen(depart_station_name);
		ar << len;
		ar.write(depart_station_name, len);
		aim_bureau[sizeof(aim_bureau)-1] = '\0';
		len = (IBYTE)strlen(aim_bureau);
		ar << len;
		ar.write(aim_bureau, len);
		aim_subbereau[sizeof(aim_subbereau)-1] = '\0';
		len = (IBYTE)strlen(aim_subbereau);
		ar << len;
		ar.write(aim_subbereau, len);
		direction_no[sizeof(direction_no)-1] = '\0';
		len = (IBYTE)strlen(direction_no);
		ar << len;
		ar.write(direction_no, len);
		freight_no_use_code[sizeof(freight_no_use_code)-1] = '\0';
		len = (IBYTE)strlen(freight_no_use_code);
		ar << len;
		ar.write(freight_no_use_code, len);
		rail_company_flag[sizeof(rail_company_flag)-1] = '\0';
		len = (IBYTE)strlen(rail_company_flag);
		ar << len;
		ar.write(rail_company_flag, len);
		no_count_flag[sizeof(no_count_flag)-1] = '\0';
		len = (IBYTE)strlen(no_count_flag);
		ar << len;
		ar.write(no_count_flag, len);
		current_status[sizeof(current_status)-1] = '\0';
		len = (IBYTE)strlen(current_status);
		ar << len;
		ar.write(current_status, len);
		ar << arrive_time << arrive_train_index;
		arrive_train_no[sizeof(arrive_train_no)-1] = '\0';
		len = (IBYTE)strlen(arrive_train_no);
		ar << len;
		ar.write(arrive_train_no, len);
		ar << arrive_flag << arrive_reason << work_complete_time << depart_time << depart_train_index << depart_flag;
	}
	else
	{
		ar >> con_id >> station_no >> side_no >> block_index_in_side >> car_index_in_block;
		ar >> len;
		ar.read(car_no, len);
		car_no[len] = '\0';
		ar >> car_type_id;
		ar >> len;
		ar.read(car_type, len);
		car_type[len]='\0';
		ar >> len;
		ar.read(car_type_code, len);
		car_type_code[len]='\0';
		ar >> oil_type >> load_flag >> car_suttle >> car_len_cnt >> car_load >> first_station_no;
		ar >> len;
		ar.read(first_station_code, len);
		first_station_code[len]='\0';
		ar >> last_station_no;
		ar >> len;
		ar.read(last_station_code, len);
		last_station_code[len]='\0';
		ar >> len;
		ar.read(freight_name, len);
		freight_name[len]='\0';
		ar >> len;
		ar.read(freight_code, len);
		freight_code[len]='\0';
		ar >> len;
		ar.read(receiver, len);
		receiver[len]='\0';
		ar >> len;
		ar.read(depart_station_code, len);
		depart_station_code[len]='\0';
		ar >> depart_station_no >> car_van_cnt;
		ar >> len;
		ar.read(car_note, len);
		car_note[len]='\0';
		ar >> len;
		ar.read(container_type_1_cnt, len);
		container_type_1_cnt[len]='\0';
		ar >> len;
		ar.read(weight_container_1_cnt, len);
		weight_container_1_cnt[len]='\0';
		ar >> len;
		ar.read(empty_contrainer_1_cnt, len);
		empty_contrainer_1_cnt[len]='\0';
		ar >> len;
		ar.read(container_type_2_cnt, len);
		container_type_2_cnt[len]='\0';
		ar >> len;
		ar.read(weight_container_2_cnt, len);
		weight_container_2_cnt[len]='\0';
		ar >> len;
		ar.read(empty_contrainer_2_cnt, len);
		empty_contrainer_2_cnt[len]='\0';
		ar >> len;
		ar.read(car_empty_weight_flag, len);
		car_empty_weight_flag[len]='\0';
		ar >> len;
		ar.read(first_station_name, len);
		first_station_name[len] = '\0';
		ar >> len;
		ar.read(last_station_name, len);
		last_station_name[len] = '\0';
		ar >> len;
		ar.read(depart_station_name, len);
		depart_station_name[len] = '\0';
		ar >> len;
		ar.read(aim_bureau, len);
		aim_bureau[len] = '\0';
		ar >> len;
		ar.read(aim_subbereau, len);
		aim_subbereau[len] = '\0';
		ar >> len;
		ar.read(direction_no, len);
		direction_no[len] = '\0';
		ar >> len;
		ar.read(freight_no_use_code, len);
		freight_no_use_code[len] = '\0';
		ar >> len;
		ar.read(rail_company_flag, len);
		rail_company_flag[len] = '\0';
		ar >> len;
		ar.read(no_count_flag, len);
		no_count_flag[len] = '\0';
		ar >> len;
		ar.read(current_status, len);
		current_status[len] = '\0';
		ar >> arrive_time >> arrive_train_index;
		ar >> len;
		ar.read(arrive_train_no, len);
		arrive_train_no[len] = '\0';
		ar >> arrive_flag >> arrive_reason >> work_complete_time
			>> depart_time >> depart_train_index >> depart_flag;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgUpdateSscCarriage::countObjectLength()
{
	car_no[sizeof(car_no)-1] = '\0';
	car_type[sizeof(car_type)-1]='\0';
	car_type_code[sizeof(car_type_code)-1]='\0';
	first_station_code[sizeof(first_station_code)-1]='\0';
	last_station_code[sizeof(last_station_code)-1]='\0';
	freight_name[sizeof(freight_name)-1]='\0';
	freight_code[sizeof(freight_code)-1]='\0';
	receiver[sizeof(receiver)-1]='\0';
	depart_station_code[sizeof(depart_station_code)-1]='\0';
	car_note[sizeof(car_note)-1]='\0';
	container_type_1_cnt[sizeof(container_type_1_cnt)-1]='\0';
	weight_container_1_cnt[sizeof(weight_container_1_cnt)-1]='\0';
	empty_contrainer_1_cnt[sizeof(empty_contrainer_1_cnt)-1]='\0';
	container_type_2_cnt[sizeof(container_type_2_cnt)-1]='\0';
	weight_container_2_cnt[sizeof(weight_container_2_cnt)-1]='\0';
	empty_contrainer_2_cnt[sizeof(empty_contrainer_2_cnt)-1]='\0';
	car_empty_weight_flag[sizeof(car_empty_weight_flag)-1] = '\0';
	first_station_name[sizeof(first_station_name)-1] = '\0';
	last_station_name[sizeof(last_station_name)-1] = '\0';
	depart_station_name[sizeof(depart_station_name)-1] = '\0';
	aim_bureau[sizeof(aim_bureau)-1] = '\0';
	aim_subbereau[sizeof(aim_subbereau)-1] = '\0';
	direction_no[sizeof(direction_no)-1] = '\0';
	freight_no_use_code[sizeof(freight_no_use_code)-1] = '\0';
	rail_company_flag[sizeof(rail_company_flag)-1] = '\0';
	no_count_flag[sizeof(no_count_flag)-1] = '\0';
	current_status[sizeof(current_status)-1] = '\0';
	arrive_train_no[sizeof(arrive_train_no)-1] = '\0';
	return sizeof(con_id) + sizeof(station_no) + sizeof(side_no) + sizeof(block_index_in_side)
		+ sizeof(car_index_in_block) + sizeof(IBYTE) + strlen(car_no) + sizeof(car_type_id)
		+ sizeof(IBYTE) + strlen(car_type) + sizeof(IBYTE) + strlen(car_type_code)
		+ sizeof(oil_type) + sizeof(load_flag) + sizeof(car_suttle) + sizeof(car_len_cnt)
		+ sizeof(car_load) + sizeof(first_station_no) + sizeof(IBYTE) + strlen(first_station_code)
		+ sizeof(last_station_no) + sizeof(IBYTE) + strlen(last_station_code)
		+ sizeof(IBYTE) + strlen(freight_name) + sizeof(IBYTE) + strlen(freight_code)
		+ sizeof(IBYTE) + strlen(receiver) + sizeof(IBYTE) + strlen(depart_station_code)
		+ sizeof(depart_station_no) + sizeof(car_van_cnt) + sizeof(IBYTE) + strlen(car_note)
		+ sizeof(IBYTE) + strlen(container_type_1_cnt) + sizeof(IBYTE) + strlen(weight_container_1_cnt)
		+ sizeof(IBYTE) + strlen(empty_contrainer_1_cnt) + sizeof(IBYTE) + strlen(container_type_2_cnt)
		+ sizeof(IBYTE) + strlen(weight_container_2_cnt) + sizeof(IBYTE) + strlen(empty_contrainer_2_cnt)
		+ sizeof(IBYTE) + strlen(car_empty_weight_flag) + sizeof(IBYTE) + strlen(first_station_name)
		+ sizeof(IBYTE) + strlen(last_station_name) + sizeof(IBYTE) + strlen(depart_station_name)
		+ sizeof(IBYTE) + strlen(aim_bureau) + sizeof(IBYTE) + strlen(aim_subbereau)
		+ sizeof(IBYTE) + strlen(direction_no) + sizeof(IBYTE) + strlen(freight_no_use_code)
		+ sizeof(IBYTE) + strlen(rail_company_flag) + sizeof(IBYTE) + strlen(no_count_flag)
		+ sizeof(IBYTE) + strlen(current_status) + sizeof(arrive_time) + sizeof(arrive_train_index)
		+ sizeof(IBYTE) + sizeof(arrive_train_no) + sizeof(arrive_flag) + sizeof(arrive_reason)
		+ sizeof(work_complete_time) + sizeof(depart_time) + sizeof(depart_train_index) + sizeof(depart_flag);
}
// delete ssc CARRIAGE
IMPLEMENT_CLBTH(MsgDeleteSscCarriage, MSGID_MsgDeleteSscCarriage)
IMPLEMENT_SERIALIZE_OPERATOR(MsgDeleteSscCarriage)
MsgDeleteSscCarriage::MsgDeleteSscCarriage()
{
	CLBTH_ZERO_INIT_OBJECT(BaseMsg)
}

MsgDeleteSscCarriage::~MsgDeleteSscCarriage()
{
}

HRCODE MsgDeleteSscCarriage::doSerialize(Archive& ar)
{
	IBYTE len;
	if(ar.isStoring())
	{
		ar << con_id << station_no << side_no << block_index_in_side << car_index_in_block;
		car_no[sizeof(car_no)-1] = '\0';
		len = (IBYTE)strlen(car_no);
		ar << len;
		ar.write(car_no, len);
	}
	else
	{
		ar >> con_id >> station_no >> side_no >> block_index_in_side >> car_index_in_block;
		ar >> len;
		ar.read(car_no, len);
		car_no[len] = '\0';
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgDeleteSscCarriage::countObjectLength()
{
	return sizeof(con_id) + sizeof(station_no) + sizeof(side_no) + sizeof(block_index_in_side)
		+ sizeof(car_index_in_block) + sizeof(IBYTE) + strlen(car_no);
}
// block oper result
IMPLEMENT_CLBTH(MsgOperSscCarriageReaponse, MSGID_MsgOperSscCarriageReaponse)
IMPLEMENT_SERIALIZE_OPERATOR(MsgOperSscCarriageReaponse)
MsgOperSscCarriageReaponse::MsgOperSscCarriageReaponse()
{
	CLBTH_ZERO_INIT_OBJECT(BaseMsg)
}

MsgOperSscCarriageReaponse::~MsgOperSscCarriageReaponse()
{
}

HRCODE MsgOperSscCarriageReaponse::doSerialize(Archive& ar)
{
	IBYTE len;
	if(ar.isStoring())
	{
		ar << con_id << station_no << side_no << block_index_in_side << car_index_in_block;
		car_no[sizeof(car_no)-1] = '\0';
		len = (IBYTE)strlen(car_no);
		ar << len;
		ar.write(car_no, len);
		ar << (short)del_update_carriage_rs;
	}
	else
	{
		ar >> con_id >> station_no >> side_no >> block_index_in_side >> car_index_in_block;
		ar >> len;
		ar.read(car_no, len);
		car_no[len] = '\0';
		short flag;
		ar >> flag;
		del_update_carriage_rs = (DEL_UPDATE_SSC_CARRIAGE_RESULT)flag;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgOperSscCarriageReaponse::countObjectLength()
{
	return sizeof(con_id) + sizeof(station_no) + sizeof(side_no) + sizeof(block_index_in_side)
		+ sizeof(car_index_in_block) + sizeof(IBYTE) + strlen(car_no) + sizeof(short);
}


///////////////////////////////////////////////
IMPLEMENT_CLBTH(MsgNextPlanSendOrder, MSGID_MsgNextPlanSendOrder)
IMPLEMENT_SERIALIZE_OPERATOR(MsgNextPlanSendOrder)
MsgNextPlanSendOrder::MsgNextPlanSendOrder()
{
	CLBTH_ZERO_INIT_OBJECT(BaseMsg)
}

MsgNextPlanSendOrder::~MsgNextPlanSendOrder()
{
}

HRCODE MsgNextPlanSendOrder::doSerialize(Archive& ar)
{
	IBYTE len;
	if(ar.isStoring())
	{
		sender_name[sizeof(sender_name)-1]=0;
		len=(IBYTE)strlen(sender_name);
		ar << send_entity << recv_entity << send_con_id << recv_con_id <<
			begin_time << end_time << send_time << len;
		ar.write(sender_name, len);	
		ar << next_count;
		for(int i=0; i<next_count; i++) {
			ar << next_order[i].station << next_order[i].count;
			for(int j=0; j<next_order[i].count; j++) {
				ar << next_order[i].entry[j] <<
					next_order[i].exit[j] <<
					next_order[i].yard[j] <<
					next_order[i].train_type[j];
			}
		}		
	}
	else
	{
		ar >> send_entity >> recv_entity >> send_con_id >> recv_con_id >>
			begin_time >> end_time >> send_time >> len;
		if(len+1 > sizeof(sender_name))
			return HRCODE(HRCODE::HC_FAIL);
		ar.read(sender_name, len);	
		sender_name[len]=0;
		ar >> next_count;
		for(int i=0; i<next_count; i++) {
			ar >> next_order[i].station >> next_order[i].count;
			for(int j=0; j<next_order[i].count; j++) {
				ar >> next_order[i].entry[j] >>
					next_order[i].exit[j] >>
					next_order[i].yard[j] >>
					next_order[i].train_type[j];
			}
		}		

	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgNextPlanSendOrder::countObjectLength()
{
	return 4096;
}

IMPLEMENT_CLBTH(MsgNextPlanRequest, MSGID_MsgNextPlanRequest)
IMPLEMENT_SERIALIZE_OPERATOR(MsgNextPlanRequest)
MsgNextPlanRequest::MsgNextPlanRequest()
{
	CLBTH_ZERO_INIT_OBJECT(BaseMsg)
}

MsgNextPlanRequest::~MsgNextPlanRequest()
{
}

HRCODE MsgNextPlanRequest::doSerialize(Archive& ar)
{
	IBYTE len;
	if(ar.isStoring())
	{
		requester_name[sizeof(requester_name)-1]=0;
		len=(IBYTE)strlen(requester_name);
		ar << requester_entity << master_entity << 
			requester_con_id << master_con_id <<
			begin_time << end_time << len;
		ar.write(requester_name, len);		
	}
	else
	{
		ar >> requester_entity >> master_entity >>
			requester_con_id >> master_con_id >>
			begin_time >> end_time >> len;
		if(len+1 > sizeof(requester_name))
			return HRCODE(HRCODE::HC_FAIL);
		ar.read(requester_name, len);
		requester_name[len]=0;		
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgNextPlanRequest::countObjectLength()
{
	requester_name[sizeof(requester_name)-1]=0;
	return sizeof(requester_entity)+sizeof(master_entity)+
		sizeof(requester_con_id)+sizeof(master_con_id)+
		sizeof(begin_time)+sizeof(end_time)+sizeof(IBYTE)+
		strlen(requester_name);	
}

IMPLEMENT_CLBTH(MsgNextPlanSendInit, MSGID_MsgNextPlanSendInit)
IMPLEMENT_SERIALIZE_OPERATOR(MsgNextPlanSendInit)
MsgNextPlanSendInit::MsgNextPlanSendInit()
{
	CLBTH_ZERO_INIT_OBJECT(BaseMsg)
}

MsgNextPlanSendInit::~MsgNextPlanSendInit()
{
}

HRCODE MsgNextPlanSendInit::doSerialize(Archive& ar)
{
	IBYTE len;
	if(ar.isStoring())
	{
		sender_name[sizeof(sender_name)-1]=0;
		len=(IBYTE)strlen(sender_name);
		ar << send_entity << recv_entity << send_con_id << recv_con_id <<
			begin_time << end_time << send_time << len;
		ar.write(sender_name, len);
	}
	else
	{
		ar >> send_entity >> recv_entity >> send_con_id >> recv_con_id >>
			begin_time >> end_time >> send_time >> len;
		if(len+1 > sizeof(sender_name))
			return HRCODE(HRCODE::HC_FAIL);
		ar.read(sender_name, len);	
		sender_name[len]=0;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgNextPlanSendInit::countObjectLength()
{
	sender_name[sizeof(sender_name)-1]=0;
	return sizeof(send_entity)+sizeof(recv_entity)+sizeof(send_con_id)+sizeof(recv_con_id)+
		sizeof(begin_time)+sizeof(end_time)+sizeof(send_time)+sizeof(IBYTE)+strlen(sender_name);	
}

IMPLEMENT_CLBTH(MsgNextPlanSendComplete, MSGID_MsgNextPlanSendComplete)
IMPLEMENT_SERIALIZE_OPERATOR(MsgNextPlanSendComplete)
MsgNextPlanSendComplete::MsgNextPlanSendComplete()
{
	CLBTH_ZERO_INIT_OBJECT(BaseMsg)
}

MsgNextPlanSendComplete::~MsgNextPlanSendComplete()
{
}

HRCODE MsgNextPlanSendComplete::doSerialize(Archive& ar)
{
	IBYTE len;
	if(ar.isStoring())
	{
		sender_name[sizeof(sender_name)-1]=0;
		len=(IBYTE)strlen(sender_name);
		ar << send_entity << recv_entity << send_con_id << recv_con_id <<
			begin_time << end_time << send_time << len;
		ar.write(sender_name, len);
		ar << insert_cnt;
	}
	else
	{
		ar >> send_entity >> recv_entity >> send_con_id >> recv_con_id >>
			begin_time >> end_time >> send_time >> len;
		if(len+1 > sizeof(sender_name))
			return HRCODE(HRCODE::HC_FAIL);
		ar.read(sender_name, len);	
		sender_name[len]=0;
		ar >> insert_cnt;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgNextPlanSendComplete::countObjectLength()
{
	sender_name[sizeof(sender_name)-1]=0;
	return sizeof(send_entity)+sizeof(recv_entity)+sizeof(send_con_id)+sizeof(recv_con_id)+
		sizeof(begin_time)+sizeof(end_time)+sizeof(send_time)+sizeof(IBYTE)+strlen(sender_name)+
		sizeof(insert_cnt);	
}

IMPLEMENT_CLBTH(MsgNextPlanInsert, MSGID_MsgNextPlanInsert)
IMPLEMENT_SERIALIZE_OPERATOR(MsgNextPlanInsert)
MsgNextPlanInsert::MsgNextPlanInsert()
{
	CLBTH_ZERO_INIT_OBJECT(BaseMsg)
}

MsgNextPlanInsert::~MsgNextPlanInsert()
{
}

HRCODE MsgNextPlanInsert::doSerialize(Archive& ar)
{
	IBYTE len;
	if(ar.isStoring())
	{
		sender_name[sizeof(sender_name)-1]=0;
		len=(IBYTE)strlen(sender_name);
		ar << send_entity << recv_entity << send_con_id << recv_con_id <<
			begin_time << end_time << send_time << len;
		ar.write(sender_name, len);

		arrive_train_id[sizeof(arrive_train_id)-1]=0;
		len=(IBYTE)strlen(arrive_train_id);
		ar << len;
		ar.write(arrive_train_id, len);
		depart_train_id[sizeof(depart_train_id)-1]=0;
		len=(IBYTE)strlen(depart_train_id);
		ar << len;
		ar.write(depart_train_id, len);
		ar << train_index << station << arrive << depart << entry <<
			exit << arrive_side << depart_side << early_depart_time << run_fast_slow_time << 
			speed_limit << stop_condition << adjust_status << flag;
	}
	else
	{
		ar >> send_entity >> recv_entity >> send_con_id >> recv_con_id >>
			begin_time >> end_time >> send_time >> len;
		if(len+1 > sizeof(sender_name))
			return HRCODE(HRCODE::HC_FAIL);
		ar.read(sender_name, len);	
		sender_name[len]=0;

		ar >> len;
		if(len+1 > sizeof(arrive_train_id))
			return HRCODE(HRCODE::HC_FAIL);
		ar.read(arrive_train_id, len);	
		arrive_train_id[len]=0;
		ar >> len;
		if(len+1 > sizeof(depart_train_id))
			return HRCODE(HRCODE::HC_FAIL);
		ar.read(depart_train_id, len);	
		depart_train_id[len]=0;
		ar >> train_index >> station >> arrive >> depart >> entry >>
			exit >> arrive_side >> depart_side >> early_depart_time >> run_fast_slow_time >> 
			speed_limit >> stop_condition >> adjust_status >> flag;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgNextPlanInsert::countObjectLength()
{
	sender_name[sizeof(sender_name)-1]=0;
	arrive_train_id[sizeof(arrive_train_id)-1]=0;
	depart_train_id[sizeof(depart_train_id)-1]=0;
	return sizeof(send_entity)+sizeof(recv_entity)+sizeof(send_con_id)+sizeof(recv_con_id)+
		sizeof(begin_time)+sizeof(end_time)+sizeof(send_time)+sizeof(IBYTE)+strlen(sender_name)+
		sizeof(IBYTE)+strlen(arrive_train_id)+strlen(depart_train_id)+sizeof(train_index)+
		sizeof(station)+sizeof(arrive)+sizeof(depart)+sizeof(entry)+sizeof(exit)+sizeof(arrive_side)+sizeof(depart_side)+
		sizeof(early_depart_time)+sizeof(run_fast_slow_time)+sizeof(speed_limit)+
		sizeof(stop_condition)+sizeof(adjust_status)+sizeof(flag);
}

IMPLEMENT_CLBTH(MsgNextPlanTrainRecv, MSGID_MsgNextPlanTrainRecv)
IMPLEMENT_SERIALIZE_OPERATOR(MsgNextPlanTrainRecv)
MsgNextPlanTrainRecv::MsgNextPlanTrainRecv()
{
	CLBTH_ZERO_INIT_OBJECT(BaseMsg)
}

MsgNextPlanTrainRecv::~MsgNextPlanTrainRecv()
{
}

HRCODE MsgNextPlanTrainRecv::doSerialize(Archive& ar)
{
	IBYTE len;
	if(ar.isStoring())
	{
		ar << send_entity << recv_entity << send_con_id << recv_con_id <<
			begin_time << end_time << send_time;

		arrive_train_id[sizeof(arrive_train_id)-1]=0;
		len=(IBYTE)strlen(arrive_train_id);
		ar << len;
		ar.write(arrive_train_id, len);
		depart_train_id[sizeof(depart_train_id)-1]=0;
		len=(IBYTE)strlen(depart_train_id);
		ar << len;
		ar.write(depart_train_id, len);
		ar << train_index << station << arrive << depart << entry <<
			exit << side << early_depart_time << run_fast_slow_time << 
			speed_limit << stop_condition << adjust_status << flag << passway;

		ar << (short)recv_status;		
	}
	else
	{
		ar >> send_entity >> recv_entity >> send_con_id >> recv_con_id >>
			begin_time >> end_time >> send_time;

		ar >> len;
		if(len+1 > sizeof(arrive_train_id))
			return HRCODE(HRCODE::HC_FAIL);
		ar.read(arrive_train_id, len);	
		arrive_train_id[len]=0;
		ar >> len;
		if(len+1 > sizeof(depart_train_id))
			return HRCODE(HRCODE::HC_FAIL);
		ar.read(depart_train_id, len);	
		depart_train_id[len]=0;

		ar >> train_index >> station >> arrive >> depart >> entry >>
			exit >> side >> early_depart_time >> run_fast_slow_time >> 
			speed_limit >> stop_condition >> adjust_status >> flag >> passway;

		short temp;
		ar >> temp;
		recv_status = (RECV_STATUS)temp;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgNextPlanTrainRecv::countObjectLength()
{
	arrive_train_id[sizeof(arrive_train_id)-1]=0;
	depart_train_id[sizeof(depart_train_id)-1]=0;
	return sizeof(send_entity)+sizeof(recv_entity)+sizeof(send_con_id)+sizeof(recv_con_id)+
		sizeof(begin_time)+sizeof(end_time)+sizeof(send_time)+sizeof(IBYTE)+strlen(arrive_train_id)+
		sizeof(IBYTE)+strlen(depart_train_id)+sizeof(train_index)+sizeof(station)+
		sizeof(arrive)+sizeof(depart)+sizeof(entry)+sizeof(exit)+sizeof(side)+
		sizeof(early_depart_time)+sizeof(run_fast_slow_time)+sizeof(speed_limit)+
		sizeof(stop_condition)+sizeof(adjust_status)+sizeof(flag)+sizeof(passway)+
		sizeof(short);
}

IMPLEMENT_CLBTH(MsgNextPlanRecvStatus, MSGID_MsgNextPlanRecvStatus)
IMPLEMENT_SERIALIZE_OPERATOR(MsgNextPlanRecvStatus)
MsgNextPlanRecvStatus::MsgNextPlanRecvStatus()
{
	CLBTH_ZERO_INIT_OBJECT(BaseMsg)
}

MsgNextPlanRecvStatus::~MsgNextPlanRecvStatus()
{
}

HRCODE MsgNextPlanRecvStatus::doSerialize(Archive& ar)
{
	IBYTE len;
	if(ar.isStoring())
	{
		sender_name[sizeof(sender_name)-1]=0;
		recver_name[sizeof(recver_name)-1]=0;
		len=(IBYTE)strlen(sender_name);
		ar << send_entity << recv_entity << send_con_id << recv_con_id <<
			begin_time << end_time << send_time << recv_time << len;
		ar.write(sender_name, len);
		len=(IBYTE)strlen(recver_name);
		ar << len;
		ar.write(recver_name, len);
		ar << (short)recv_status;
	}
	else
	{
		ar >> send_entity >> recv_entity >> send_con_id >> recv_con_id >>
			begin_time >> end_time >> send_time >> recv_time >> len;
		if(len+1 > sizeof(sender_name))
			return HRCODE(HRCODE::HC_FAIL);
		ar.read(sender_name, len);	
		sender_name[len]=0;
		ar >> len;
		if(len+1 > sizeof(recver_name))
			return HRCODE(HRCODE::HC_FAIL);
		ar.read(recver_name, len);	
		recver_name[len]=0;

		short temp;
		ar >> temp;
		recv_status = (PLAN_RECV_STATUS)temp;

	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgNextPlanRecvStatus::countObjectLength()
{
	sender_name[sizeof(sender_name)-1]=0;
	recver_name[sizeof(recver_name)-1]=0;
	return sizeof(send_entity)+sizeof(recv_entity)+sizeof(send_con_id)+sizeof(recv_con_id)+
		sizeof(begin_time)+sizeof(end_time)+sizeof(send_time)+sizeof(recv_time)+
		sizeof(IBYTE)+strlen(sender_name)+sizeof(IBYTE)+strlen(recver_name)+sizeof(short);
}

IMPLEMENT_CLBTH(MsgNextPlanSignStatus, MSGID_MsgNextPlanSignStatus)
IMPLEMENT_SERIALIZE_OPERATOR(MsgNextPlanSignStatus)
MsgNextPlanSignStatus::MsgNextPlanSignStatus()
{
	CLBTH_ZERO_INIT_OBJECT(BaseMsg)
}

MsgNextPlanSignStatus::~MsgNextPlanSignStatus()
{
}

HRCODE MsgNextPlanSignStatus::doSerialize(Archive& ar)
{
	IBYTE len;
	if(ar.isStoring())
	{
		sender_name[sizeof(sender_name)-1]=0;
		recver_name[sizeof(recver_name)-1]=0;
		len=(IBYTE)strlen(sender_name);
		ar << send_entity << recv_entity << send_con_id << recv_con_id <<
			begin_time << end_time << send_time << recv_time << len;
		ar.write(sender_name, len);
		len=(IBYTE)strlen(recver_name);
		ar << len;
		ar.write(recver_name, len);
		ar << accept_count << reject_count;
		int i;
		for(i=0; i<accept_count; i++) {
			accept_train[i][sizeof(accept_train[0])-1]=0;
			len=(IBYTE)strlen(accept_train[i]);
			ar << len;
			ar.write(accept_train[i], len);
		}
		for(i=0; i<reject_count; i++) {
			reject_train[i][sizeof(reject_train[0])-1]=0;
			len=(IBYTE)strlen(reject_train[i]);
			ar << len;
			ar.write(reject_train[i], len);
		}		
	}
	else
	{
		ar >> send_entity >> recv_entity >> send_con_id >> recv_con_id >>
			begin_time >> end_time >> send_time >> recv_time >> len;
		if(len+1 > sizeof(sender_name))
			return HRCODE(HRCODE::HC_FAIL);
		ar.read(sender_name, len);	
		sender_name[len]=0;
		ar >> len;
		if(len+1 > sizeof(recver_name))
			return HRCODE(HRCODE::HC_FAIL);
		ar.read(recver_name, len);	
		recver_name[len]=0;

		ar >> accept_count >> reject_count;
		int i;
		for(i=0; i<accept_count; i++) {
			len=(IBYTE)strlen(accept_train[i]);
			ar >> len;
			ar.read(accept_train[i], len);
			accept_train[i][len]=0;
		}
		for(i=0; i<reject_count; i++) {
			len=(IBYTE)strlen(reject_train[i]);
			ar >> len;
			ar.read(reject_train[i], len);
			reject_train[i][len]=0;
		}

	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgNextPlanSignStatus::countObjectLength()
{
	return 4096;
}

IMPLEMENT_CLBTH(MsgWordPhasePlanSend, MSGID_MsgWordPhasePlanSend)
IMPLEMENT_SERIALIZE_OPERATOR(MsgWordPhasePlanSend)
MsgWordPhasePlanSend::MsgWordPhasePlanSend()
{
	CLBTH_ZERO_INIT_OBJECT(BaseMsg)
}

MsgWordPhasePlanSend::~MsgWordPhasePlanSend()
{
}

HRCODE MsgWordPhasePlanSend::doSerialize(Archive& ar)
{
	IBYTE len;
	if(ar.isStoring())
	{
		ar << shift_id << con_id << schd_type << phase_id << recv_count;
		for(int i=0; i<recv_count; i++)
			ar << recv_list[i];

		ar << send_entity << edit_time << send_time;
		ar << start_sta_weather << end_sta_weather;

		send_name[sizeof(send_name)-1]=0;
		len=(IBYTE)strlen(send_name);
		ar << len;
		ar.write(send_name, len);

		permit_name[sizeof(permit_name)-1]=0;
		len=(IBYTE)strlen(permit_name);
		ar << len;
		ar.write(permit_name, len);

		if(phase_len > sizeof(phase_content) || phase_len <0)
			return HRCODE(HRCODE::HC_FAIL);
		ar << phase_len;
		ar.write(phase_content, phase_len);

		ar << (short)phase_oper;
	}
	else
	{
		ar >> shift_id >> con_id >> schd_type >> phase_id >> recv_count;
		for(int i=0; i<recv_count; i++)
			ar >> recv_list[i];

		ar >> send_entity >> edit_time >> send_time;
		ar >> start_sta_weather >> end_sta_weather;

		ar >> len;
		if(len > sizeof(send_name)-1) 
			return HRCODE(HRCODE::HC_FAIL);
		ar.read(send_name, len);
		send_name[len]=0;

		ar >> len;
		if(len > sizeof(permit_name)-1) 
			return HRCODE(HRCODE::HC_FAIL);
		ar.read(permit_name, len);
		permit_name[len]=0;

		ar >> phase_len;
		if(phase_len > sizeof(phase_content) || phase_len < 0)
			return HRCODE(HRCODE::HC_FAIL);		
		ar.read(phase_content, phase_len);

		short t;
		ar >> t;
		phase_oper = (WORDPHASE_OPER)t;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgWordPhasePlanSend::countObjectLength()
{
	send_name[sizeof(send_name)-1]=0;
	phase_content[sizeof(phase_content)-1]=0;
	return sizeof(shift_id)+sizeof(con_id)+sizeof(schd_type)+sizeof(phase_id)+
		sizeof(recv_count)+sizeof(recv_list[0])*recv_count+
		sizeof(send_entity)+sizeof(edit_time)+sizeof(send_time)+
		sizeof(start_sta_weather)+sizeof(end_sta_weather)+
		sizeof(IBYTE)+strlen(send_name)+sizeof(IBYTE)+strlen(permit_name)+
		sizeof(short)+phase_len+sizeof(short);
}

IMPLEMENT_CLBTH(MsgWordPhasePlanStatus, MSGID_MsgWordPhasePlanStatus)
IMPLEMENT_SERIALIZE_OPERATOR(MsgWordPhasePlanStatus)
MsgWordPhasePlanStatus::MsgWordPhasePlanStatus()
{
	CLBTH_ZERO_INIT_OBJECT(BaseMsg)
}

MsgWordPhasePlanStatus::~MsgWordPhasePlanStatus()
{
}

HRCODE MsgWordPhasePlanStatus::doSerialize(Archive& ar)
{
	IBYTE len;
	if(ar.isStoring())
	{
		ar << shift_id << con_id << schd_type << phase_id << 
			send_entity << recv_entity << send_time << recv_time << edit_time;

		send_name[sizeof(send_name)-1]=0;
		len=(IBYTE)strlen(send_name);
		ar << len;
		ar.write(send_name, len);

		permit_name[sizeof(permit_name)-1]=0;
		len=(IBYTE)strlen(permit_name);
		ar << len;
		ar.write(permit_name, len);

		recv_name[sizeof(recv_name)-1]=0;
		len=(IBYTE)strlen(recv_name);
		ar << len;
		ar.write(recv_name, len);

		ar << (short)recv_status;
	}
	else
	{
		ar >> shift_id >> con_id >> schd_type >> phase_id >> 
			send_entity >> recv_entity >> send_time >> recv_time >> edit_time;

		ar >> len;
		if(len > sizeof(send_name)-1) 
			return HRCODE(HRCODE::HC_FAIL);
		ar.read(send_name, len);
		send_name[len]=0;

		ar >> len;
		if(len > sizeof(permit_name)-1) 
			return HRCODE(HRCODE::HC_FAIL);
		ar.read(permit_name, len);
		permit_name[len]=0;

		ar >> len;
		if(len > sizeof(recv_name)-1) 
			return HRCODE(HRCODE::HC_FAIL);
		ar.read(recv_name, len);
		recv_name[len]=0;

		short t;
		ar >> t;
		recv_status = (WORDPHASE_RECV_STATUS)t;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgWordPhasePlanStatus::countObjectLength()
{
	send_name[sizeof(send_name)-1]=0;
	permit_name[sizeof(permit_name)-1]=0;
	recv_name[sizeof(recv_name)-1]=0;
	return sizeof(shift_id)+sizeof(con_id)+sizeof(schd_type)+sizeof(phase_id)+
		sizeof(send_entity)+sizeof(recv_entity)+sizeof(send_time)+sizeof(recv_time)+sizeof(edit_time)+
		sizeof(IBYTE)+strlen(send_name)+sizeof(IBYTE)+strlen(permit_name)+sizeof(IBYTE)+strlen(recv_name)+sizeof(short);
}

IMPLEMENT_CLBTH(MsgWordPhasePlanQuery, MSGID_MsgWordPhasePlanQuery)
IMPLEMENT_SERIALIZE_OPERATOR(MsgWordPhasePlanQuery)
MsgWordPhasePlanQuery::MsgWordPhasePlanQuery()
{
	CLBTH_ZERO_INIT_OBJECT(BaseMsg)
}

MsgWordPhasePlanQuery::~MsgWordPhasePlanQuery()
{
}

HRCODE MsgWordPhasePlanQuery::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << shift_id << con_id << schd_type << phase_count;
		for(int i=0; i<phase_count; i++)
			ar << phase_id[i];
	}
	else
	{
		ar >> shift_id >> con_id >> schd_type >> phase_count;
		for(int i=0; i<phase_count; i++)
			ar >> phase_id[i];
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgWordPhasePlanQuery::countObjectLength()
{
	return sizeof(shift_id)+sizeof(con_id)+sizeof(schd_type)+sizeof(phase_count)+phase_count*(sizeof(phase_id[0]));
}

IMPLEMENT_CLBTH(MsgSimpleStationCarFormat, MSGID_MsgSimpleStationCarFormat)
IMPLEMENT_SERIALIZE_OPERATOR(MsgSimpleStationCarFormat)
MsgSimpleStationCarFormat::MsgSimpleStationCarFormat()
{
	CLBTH_ZERO_INIT_OBJECT(BaseMsg)
}

MsgSimpleStationCarFormat::~MsgSimpleStationCarFormat()
{
}

HRCODE MsgSimpleStationCarFormat::doSerialize(Archive& ar)
{
	IBYTE len;
	if(ar.isStoring())
	{
		ar << input_time << station << index ;
		ar << (short)phase;
		side_name[sizeof(side_name)-1]=0;
		len=strlen(side_name);
		ar << len;
		ar.write(side_name, len);

		ar << side << car_flag << car_type << car_count;
		destine[sizeof(destine)-1]=0;
		len=strlen(destine);
		ar << len;
		ar.write(destine, len);
	}
	else
	{
		ar >> input_time >> station >> index;

		short t;
		ar >> t;
		phase = (STACAR_PHASE)t;
		ar >> len;
		if (len > sizeof(side_name)-1)
			return HRCODE(HRCODE::HC_FAIL);
		ar.read(side_name, len);
		side_name[len]=0;
		ar >> side >> car_flag >> car_type >> car_count;
		ar >> len;
		if(len > sizeof(destine)-1)
			return HRCODE(HRCODE::HC_FAIL);

		ar.read(destine, len);
		destine[len]=0;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgSimpleStationCarFormat::countObjectLength()
{
	size_t len=0;	
	side_name[sizeof(side_name)-1]=0;	
	len=sizeof(input_time)+sizeof(station)+sizeof(short)+sizeof(index)+
		sizeof(IBYTE)+strlen(side_name);

	len+= sizeof(side)+sizeof(car_flag)+sizeof(car_type)+sizeof(car_count);
	destine[sizeof(destine)-1]=0;
	len+=sizeof(IBYTE)+strlen(destine);
	return len;
}

IMPLEMENT_CLBTH(MsgSimpleStationCarCtrl, MSGID_MsgSimpleStationCarCtrl)
IMPLEMENT_SERIALIZE_OPERATOR(MsgSimpleStationCarCtrl)
MsgSimpleStationCarCtrl::MsgSimpleStationCarCtrl()
{
	CLBTH_ZERO_INIT_OBJECT(BaseMsg)
}

MsgSimpleStationCarCtrl::~MsgSimpleStationCarCtrl()
{
}

HRCODE MsgSimpleStationCarCtrl::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << input_time << station << (short)phase << (short)status;
	}
	else
	{
		ar >> input_time >> station;
		short t;
		ar >> t;
		phase = (STACAR_PHASE)t;
		ar >> t;
		status = (MSG_STATUS)t;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgSimpleStationCarCtrl::countObjectLength()
{	
	return sizeof(input_time)+sizeof(station)+sizeof(short)+sizeof(short);
}

IMPLEMENT_CLBTH(MsgSimpleStationCarQuery, MSGID_MsgSimpleStationCarQuery)
IMPLEMENT_SERIALIZE_OPERATOR(MsgSimpleStationCarQuery)
MsgSimpleStationCarQuery::MsgSimpleStationCarQuery()
{
	CLBTH_ZERO_INIT_OBJECT(BaseMsg)
}

MsgSimpleStationCarQuery::~MsgSimpleStationCarQuery()
{
}

HRCODE MsgSimpleStationCarQuery::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << begin_time << end_time << sta_count;
		for(int i=0; i<sta_count; i++) {
			ar << sta_list[i];
		}
		ar << (short)phase;
	}
	else
	{
		ar >> begin_time >> end_time >> sta_count;
		if(sta_count > sizeof(sta_list)/sizeof(sta_list[0])) {
			sta_count=0;
			return HRCODE(HRCODE::HC_FAIL);
		}

		for(int i=0; i<sta_count; i++) {
			ar >> sta_list[i];
		}
		short t;
		ar >> t;
		phase = (STACAR_PHASE)t;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgSimpleStationCarQuery::countObjectLength()
{	
	return sizeof(begin_time)+sizeof(end_time)+sizeof(sta_count)+sta_count*sizeof(STATION_NO)+sizeof(short);
}

IMPLEMENT_CLBTH(MsgSimpleStationCarConvert, MSGID_MsgSimpleStationCarConvert)
IMPLEMENT_SERIALIZE_OPERATOR(MsgSimpleStationCarConvert)
MsgSimpleStationCarConvert::MsgSimpleStationCarConvert()
{
}

MsgSimpleStationCarConvert::~MsgSimpleStationCarConvert()
{
}

HRCODE MsgSimpleStationCarConvert::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		if(sta_count > sizeof(sta_list)/sizeof(sta_list[0]))
			return HRCODE(HRCODE::HC_FAIL);

		ar << (short)from_phase <<(short)to_phase << from_input_time << to_input_time;
		ar << sta_count;
		for(int i=0; i<sta_count; i++)
			ar << sta_list[i];
	}
	else
	{
		short t1, t2;		
		ar >> t1 >> t2 >> from_input_time >> to_input_time;
		from_phase = STACAR_PHASE(t1);
		to_phase = STACAR_PHASE(t2);
		ar >> sta_count;

		if(sta_count > sizeof(sta_list)/sizeof(sta_list[0]))
			return HRCODE(HRCODE::HC_FAIL);

		for(int i=0; i<sta_count; i++)
			ar >> sta_list[i];
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgSimpleStationCarConvert::countObjectLength()
{	
	return sizeof(short)*2+sizeof(from_input_time)+sizeof(to_input_time)+sizeof(sta_count)+sta_count*sizeof(sta_list[0]);
}

IMPLEMENT_CLBTH(MsgNotePadFormat, MSGID_MsgNotePadFormat)
IMPLEMENT_SERIALIZE_OPERATOR(MsgNotePadFormat)
MsgNotePadFormat::MsgNotePadFormat()
{
}

MsgNotePadFormat::~MsgNotePadFormat()
{
}

HRCODE MsgNotePadFormat::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		if(data_len > sizeof(data) || data_len <0 )
			return HRCODE(HRCODE::HC_FAIL);
		ar << shift_id << con_id << schd_type << data_len;

		ar.write(data, data_len);
	}
	else
	{
		ar >> shift_id >> con_id >> schd_type >> data_len;
		if(data_len > sizeof(data) || data_len <0 )
			return HRCODE(HRCODE::HC_FAIL);

		ar.read(data, data_len);		
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgNotePadFormat::countObjectLength()
{	
	return sizeof(shift_id)+sizeof(con_id)+sizeof(schd_type)+sizeof(data_len)+data_len;
}

IMPLEMENT_CLBTH(MsgNotePadQuery, MSGID_MsgNotePadQuery)
IMPLEMENT_SERIALIZE_OPERATOR(MsgNotePadQuery)
MsgNotePadQuery::MsgNotePadQuery()
{
}

MsgNotePadQuery::~MsgNotePadQuery()
{
}

HRCODE MsgNotePadQuery::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << shift_id << con_id << schd_type;
	}
	else
	{
		ar >> shift_id >> con_id >> schd_type;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgNotePadQuery::countObjectLength()
{	
	return sizeof(shift_id)+sizeof(con_id)+sizeof(schd_type);
}

IMPLEMENT_CLBTH(MsgAccidentOper, MSGID_MsgAccidentOper)
IMPLEMENT_SERIALIZE_OPERATOR(MsgAccidentOper)
MsgAccidentOper::MsgAccidentOper()
{
	CLBTH_ZERO_INIT_OBJECT(BaseMsg)
}

MsgAccidentOper::~MsgAccidentOper()
{
}

HRCODE MsgAccidentOper::doSerialize(Archive& ar)
{
	IBYTE len;
	if(ar.isStoring())
	{
		if(note_len > sizeof(note))
			return HRCODE(HRCODE::HC_FAIL);

		ar << con_id << schd_type << shift_id << index << note_len;
		ar.write(note, note_len);

		subject[sizeof(subject)-1]=0;
		len=strlen(subject);
		ar << len;
		ar.write(subject, len);
		ar << (short)oper;		
	}
	else
	{
		ar >> con_id >> schd_type >> shift_id >> index >> note_len;
		if(note_len > sizeof(note))
			return HRCODE(HRCODE::HC_FAIL);
		ar.read(note, note_len);

		ar >> len;
		if(len > sizeof(note))
			return HRCODE(HRCODE::HC_FAIL);		
		ar.read(subject, len);
		subject[len]=0;

		short t;
		ar >> t;
		oper=(AccidentOper)t;

	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgAccidentOper::countObjectLength()
{	
	subject[sizeof(subject)-1]=0;
	return  sizeof(con_id)+sizeof(schd_type)+sizeof(shift_id)+sizeof(index)+sizeof(note_len)+
		note_len+sizeof(IBYTE)+strlen(subject)+sizeof(short);
}

IMPLEMENT_CLBTH(MsgAccidentQuery, MSGID_MsgAccidentQuery)
IMPLEMENT_SERIALIZE_OPERATOR(MsgAccidentQuery)
MsgAccidentQuery::MsgAccidentQuery()
{
	CLBTH_ZERO_INIT_OBJECT(BaseMsg)
}

MsgAccidentQuery::~MsgAccidentQuery()
{
}

HRCODE MsgAccidentQuery::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << con_id << schd_type << shift_id;
	}
	else
	{
		ar >> con_id >> schd_type >> shift_id;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgAccidentQuery::countObjectLength()
{	
	return  sizeof(con_id)+sizeof(schd_type)+sizeof(shift_id);
}

IMPLEMENT_CLBTH(MsgUserManager, MSGID_MsgUserManager)
IMPLEMENT_SERIALIZE_OPERATOR(MsgUserManager)
MsgUserManager::MsgUserManager()
{
	CLBTH_ZERO_INIT_OBJECT(BaseMsg)
}

MsgUserManager::~MsgUserManager()
{
}

HRCODE MsgUserManager::doSerialize(Archive& ar)
{
	IBYTE len;

	if(ar.isStoring())
	{
		ar << count;
		for(int i=0; i<count; i++) {
			data[i].user_code[sizeof(data[i].user_code)-1]=0;
			len=strlen(data[i].user_code);
			ar << len;
			ar.write(data[i].user_code, len);

			data[i].user_name[sizeof(data[i].user_name)-1]=0;
			len=strlen(data[i].user_name);
			ar << len;
			ar.write(data[i].user_name, len);

			data[i].user_pwd[sizeof(data[i].user_pwd)-1]=0;
			len=strlen(data[i].user_pwd);
			ar << len;
			ar.write(data[i].user_pwd, len);

			ar << data[i].job_id;
			ar << data[i].console_count;

			for(int j=0; j<data[i].console_count; j++)
				ar << data[i].con_list[j];
		}
		ar << (short)oper;
	}
	else
	{
		ar >> count;
		if(count > sizeof(data)/sizeof(data[0]))
			return HRCODE(HRCODE::HC_FAIL);
		for(int i=0; i<count; i++) {
			ar >> len;
			if(len > sizeof(data[i].user_code)-1)
				return HRCODE(HRCODE::HC_FAIL);
			ar.read(data[i].user_code, len);
			data[i].user_code[len]=0;

			ar >> len;
			if(len > sizeof(data[i].user_name)-1)
				return HRCODE(HRCODE::HC_FAIL);
			ar.read(data[i].user_name, len);
			data[i].user_name[len]=0;

			ar >> len;
			if(len > sizeof(data[i].user_pwd)-1)
				return HRCODE(HRCODE::HC_FAIL);
			ar.read(data[i].user_pwd, len);
			data[i].user_pwd[len]=0;

			ar >> data[i].job_id;
			ar >> data[i].console_count;
			if(data[i].console_count > sizeof(data[i].con_list)/sizeof(data[i].con_list[0]))
				return HRCODE(HRCODE::HC_FAIL);

			for(int j=0; j<data[i].console_count; j++)
				ar >> data[i].con_list[j];
		}
		short t;
		ar >> t;
		oper = (USER_OPER)t;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgUserManager::countObjectLength()
{	
	size_t len=sizeof(count);
	for(int i=0; i<count; i++) {
		len+=strlen(data[i].user_code)+sizeof(IBYTE);
		len+=strlen(data[i].user_name)+sizeof(IBYTE);
		len+=strlen(data[i].user_pwd)+sizeof(IBYTE);
		len+=sizeof(JOB_ID)+sizeof(IBYTE)+data[i].console_count*sizeof(data[i].con_list[0]);
	}
	len+=sizeof(short);
	return len;
}

IMPLEMENT_CLBTH(MsgUserManagerQuery, MSGID_MsgUserManagerQuery)
IMPLEMENT_SERIALIZE_OPERATOR(MsgUserManagerQuery)
MsgUserManagerQuery::MsgUserManagerQuery()
{

}

MsgUserManagerQuery::~MsgUserManagerQuery()
{
}

HRCODE MsgUserManagerQuery::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
	}
	else
	{
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgUserManagerQuery::countObjectLength()
{	
	return 0;
}

IMPLEMENT_CLBTH(MsgRouteStatusInform, MSGID_MsgRouteStatusInform)
IMPLEMENT_SERIALIZE_OPERATOR(MsgRouteStatusInform)
MsgRouteStatusInform::MsgRouteStatusInform()
{

}

MsgRouteStatusInform::~MsgRouteStatusInform()
{
}

HRCODE MsgRouteStatusInform::doSerialize(Archive& ar)
{
	IBYTE len;
	if(ar.isStoring())
	{
		train_id[sizeof(train_id)-1]=0;
		ar << station << train_index << rec_index << con_id << 
			entry << exit << side << (short)attrib << status;
		len=strlen(train_id);
		ar << len;
		ar.write(train_id, len);
	}
	else
	{
		short t;
		ar >> station >> train_index >> rec_index >> con_id >> 
			entry >>exit >> side >> t >> status >> len;
		if (len>sizeof(train_id)-1) 
			return HRCODE(HRCODE::HC_FAIL);

		ar.read(train_id, len);
		train_id[len]=0;
		attrib=(ROUTE_ATTRIBUTE)t;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgRouteStatusInform::countObjectLength()
{	
	train_id[sizeof(train_id)-1]=0;
	return sizeof(station)+sizeof(train_index)+sizeof(rec_index)+sizeof(con_id)+
		sizeof(entry)+sizeof(exit)+sizeof(side)+sizeof(short)+sizeof(status)+
		sizeof(IBYTE)+strlen(train_id);

}

IMPLEMENT_CLBTH(MsgGroupTrainWorkFormat, MSGID_MsgGroupTrainWorkFormat)
IMPLEMENT_SERIALIZE_OPERATOR(MsgGroupTrainWorkFormat)
MsgGroupTrainWorkFormat::MsgGroupTrainWorkFormat()
{
	CLBTH_ZERO_INIT_OBJECT(BaseMsg)	
}

MsgGroupTrainWorkFormat::~MsgGroupTrainWorkFormat()
{
}

HRCODE MsgGroupTrainWorkFormat::doSerialize(Archive& ar)
{
	IBYTE len;
	if(ar.isStoring())
	{
		if(rec_count >=sizeof(data)/sizeof(data[0]))
			return HRCODE(HRCODE::HC_FAIL);

		ar << con_id << schd_type << (short)oper << rec_count;
		for(int i=0; i<rec_count; i++) {
			ar << data[i].input_time << data[i].train_index << data[i].station 
				<< data[i].work_type << data[i].side;

			data[i].train_id[sizeof(data[i].train_id)-1]=0;			
			len=strlen(data[i].train_id);
			ar<<len;
			ar.write(data[i].train_id, len);

			data[i].work_content[sizeof(data[i].work_content)-1]=0;			
			len=strlen(data[i].work_content);
			ar<<len;
			ar.write(data[i].work_content, len);		
		}
	}
	else
	{
		short t;
		ar >> con_id >> schd_type >> t >> rec_count;
		oper=(GROUP_TRAIN_WORK_OPER)t;
		if(rec_count >=sizeof(data)/sizeof(data[0]))
			return HRCODE(HRCODE::HC_FAIL);

		for(int i=0; i<rec_count; i++) {
			ar >> data[i].input_time >> data[i].train_index >> data[i].station 
				>> data[i].work_type >> data[i].side;

			ar >> len;
			if(len > sizeof(data[i].train_id)-1)
				return HRCODE(HRCODE::HC_FAIL);
			ar.read(data[i].train_id, len);
			data[i].train_id[len]=0;

			ar >> len;
			if(len > sizeof(data[i].work_content)-1)
				return HRCODE(HRCODE::HC_FAIL);
			ar.read(data[i].work_content, len);
			data[i].work_content[len]=0;			
		}
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgGroupTrainWorkFormat::countObjectLength()
{	
	size_t len=sizeof(con_id)+sizeof(schd_type)+sizeof(short)+sizeof(rec_count);
	for(int i=0; i<rec_count; i++) {
		len+=sizeof(data[i].input_time)+sizeof(data[i].train_index)+sizeof(data[i].station)+ 
			sizeof(data[i].work_type)+sizeof(data[i].side);
		data[i].train_id[sizeof(data[i].train_id)-1]=0;
		data[i].work_content[sizeof(data[i].work_content)-1]=0;
		len+=strlen(data[i].train_id)+sizeof(IBYTE)+strlen(data[i].work_content)+sizeof(IBYTE);
	}
	return len;
}

IMPLEMENT_CLBTH(MsgGroupTrainWorkDel, MSGID_MsgGroupTrainWorkDel)
IMPLEMENT_SERIALIZE_OPERATOR(MsgGroupTrainWorkDel)
MsgGroupTrainWorkDel::MsgGroupTrainWorkDel()
{
	CLBTH_ZERO_INIT_OBJECT(BaseMsg)	
}

MsgGroupTrainWorkDel::~MsgGroupTrainWorkDel()
{
}

HRCODE MsgGroupTrainWorkDel::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		if(rec_count >=sizeof(input_time)/sizeof(input_time[0]))
			return HRCODE(HRCODE::HC_FAIL);

		ar << con_id << schd_type << rec_count;
		for(int i=0; i<rec_count; i++) {
			ar << input_time[i];
		}
	}
	else
	{
		ar >> con_id >> schd_type >> rec_count;
		if(rec_count >=sizeof(input_time)/sizeof(input_time[0]))
			return HRCODE(HRCODE::HC_FAIL);

		for(int i=0; i<rec_count; i++) {
			ar >> input_time[i];
		}
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgGroupTrainWorkDel::countObjectLength()
{	
	return sizeof(con_id)+sizeof(schd_type)+sizeof(rec_count)+rec_count*sizeof(input_time[0]);
}

IMPLEMENT_CLBTH(MsgGroupTrainWorkQuery, MSGID_MsgGroupTrainWorkQuery)
IMPLEMENT_SERIALIZE_OPERATOR(MsgGroupTrainWorkQuery)
MsgGroupTrainWorkQuery::MsgGroupTrainWorkQuery()
{
	CLBTH_ZERO_INIT_OBJECT(BaseMsg)	
}

MsgGroupTrainWorkQuery::~MsgGroupTrainWorkQuery()
{
}

HRCODE MsgGroupTrainWorkQuery::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		if(rec_count > sizeof(train_index)/sizeof(train_index[0]))
			return HRCODE(HRCODE::HC_FAIL);

		ar << shift_id << con_id << schd_type << rec_count;
		for(int i=0; i<rec_count; i++) {
			ar << train_index[i];
		}
	}
	else
	{
		ar >> shift_id >> con_id >> schd_type >> rec_count;
		if(rec_count > sizeof(train_index)/sizeof(train_index[0]))
			return HRCODE(HRCODE::HC_FAIL);

		for(int i=0; i<rec_count; i++) {
			ar >> train_index[i];
		}
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgGroupTrainWorkQuery::countObjectLength()
{	
	return sizeof(shift_id)+sizeof(con_id)+sizeof(schd_type)+sizeof(rec_count)+rec_count*sizeof(train_index[0]);
}

IMPLEMENT_CLBTH(MsgAdjustSequenceLoad, MSGID_MsgAdjustSequenceLoad)
IMPLEMENT_SERIALIZE_OPERATOR(MsgAdjustSequenceLoad)
MsgAdjustSequenceLoad::MsgAdjustSequenceLoad()
{
}
MsgAdjustSequenceLoad::~MsgAdjustSequenceLoad()
{
}
HRCODE MsgAdjustSequenceLoad::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << con_id;
	}
	else
	{
		ar >> con_id; 
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgAdjustSequenceLoad::countObjectLength()
{	
	return sizeof(con_id);
}

IMPLEMENT_CLBTH(MsgAdjustSequenceData, MSGID_MsgAdjustSequenceData)
IMPLEMENT_SERIALIZE_OPERATOR(MsgAdjustSequenceData)
MsgAdjustSequenceData::MsgAdjustSequenceData()
{
}
MsgAdjustSequenceData::~MsgAdjustSequenceData()
{
}
HRCODE MsgAdjustSequenceData::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		if(count > sizeof(data)/sizeof(data[0]))
			return HRCODE(HRCODE::HC_FAIL);
		ar << con_id << count << (short)oper;
		for(int i=0; i<count; i++) 
			ar << data[i].train_index << data[i].prio;
	}
	else
	{
		short t;
		ar >> con_id >> count >> t; 
		oper = (OPER)t;
		if(count > sizeof(data)/sizeof(data[0]))
			return HRCODE(HRCODE::HC_FAIL);
		for(int i=0; i<count; i++) 
			ar >> data[i].train_index >> data[i].prio;			
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgAdjustSequenceData::countObjectLength()
{	
	return sizeof(con_id)+sizeof(count)+sizeof(short)+count*sizeof(data[0]);
}

IMPLEMENT_CLBTH(MsgAutoAdjustInform, MSGID_MsgAutoAdjustInform)
IMPLEMENT_SERIALIZE_OPERATOR(MsgAutoAdjustInform)
MsgAutoAdjustInform::MsgAutoAdjustInform()
{
}
MsgAutoAdjustInform::~MsgAutoAdjustInform()
{
}
HRCODE MsgAutoAdjustInform::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << con_id << start_time << end_time << (short)oper;
	}
	else
	{
		short t;
		ar >> con_id >> start_time >> end_time >> t;
		oper = (OPER)t;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgAutoAdjustInform::countObjectLength()
{	
	return sizeof(con_id)+sizeof(start_time)+sizeof(end_time)+sizeof(short);
}

IMPLEMENT_CLBTH(MsgAutoAdjustSingleInform, MSGID_MsgAutoAdjustSingleInform)
IMPLEMENT_SERIALIZE_OPERATOR(MsgAutoAdjustSingleInform)
MsgAutoAdjustSingleInform::MsgAutoAdjustSingleInform()
{
}
MsgAutoAdjustSingleInform::~MsgAutoAdjustSingleInform()
{
}
HRCODE MsgAutoAdjustSingleInform::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << con_id << train_index << adjust_begin_sta_index << 
			adjust_end_sta_index << (short)prio;
	}
	else
	{
		short t;
		ar >> con_id >> train_index >> adjust_begin_sta_index >>
			adjust_end_sta_index >> t;
		prio = (SINGLE_ADJUST_PRIO)t;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgAutoAdjustSingleInform::countObjectLength()
{	
	return sizeof(con_id)+sizeof(train_index)+sizeof(adjust_begin_sta_index)+sizeof(adjust_end_sta_index)+sizeof(short);
}

IMPLEMENT_CLBTH(MsgAutoAdjustUndo, MSGID_MsgAutoAdjustUndo)
IMPLEMENT_SERIALIZE_OPERATOR(MsgAutoAdjustUndo)
MsgAutoAdjustUndo::MsgAutoAdjustUndo()
{
}
MsgAutoAdjustUndo::~MsgAutoAdjustUndo()
{
}
HRCODE MsgAutoAdjustUndo::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << con_id;
	}
	else
	{
		ar >> con_id;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgAutoAdjustUndo::countObjectLength()
{	
	return sizeof(con_id);
}

IMPLEMENT_CLBTH(MsgLiRCToGpc, MSGID_MsgLiRCToGpc)
IMPLEMENT_SERIALIZE_OPERATOR(MsgLiRCToGpc)
MsgLiRCToGpc::MsgLiRCToGpc()
:	nGpcID(0), nMsgLen(0)
{
}
MsgLiRCToGpc::~MsgLiRCToGpc()
{
}
HRCODE MsgLiRCToGpc::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << nGpcID << nMsgLen;
		if ( nMsgLen > BUFFER_SIZE )
			return HRCODE(HRCODE::HC_FAIL);
		if ( nMsgLen > 0 )
			ar.write( Msg, nMsgLen );
	}
	else
	{
		ar >> nGpcID >> nMsgLen;
		if ( nMsgLen > BUFFER_SIZE )
			return HRCODE(HRCODE::HC_FAIL);
		if ( nMsgLen > 0 )
			ar.read( Msg, nMsgLen );
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgLiRCToGpc::countObjectLength()
{
	return sizeof(nGpcID) + sizeof(nMsgLen) + nMsgLen;
}


IMPLEMENT_CLBTH(MsgShuntNoteSend2LocoMotive, MSGID_MsgShuntNoteSend2Lirc)
IMPLEMENT_SERIALIZE_OPERATOR(MsgShuntNoteSend2LocoMotive)
MsgShuntNoteSend2LocoMotive::MsgShuntNoteSend2LocoMotive()
{
}
MsgShuntNoteSend2LocoMotive::~MsgShuntNoteSend2LocoMotive()
{
}


HRCODE MsgShuntNoteSend2LocoMotive::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << shunt_index ;
        ar << nstation;	               //  站号
		train_id[sizeof(train_id)-1]='\0';
		ar.write(train_id, sizeof(train_id));

		engine_id[sizeof(engine_id)-1]='\0';
		ar.write(engine_id, sizeof(engine_id));
		
		ar << send_time;
	    ar << (short)send_way;
		ar << (short)oper_type;
		
	}
	else
	{
		ar >> shunt_index ;
        ar >> nstation;
		ar.read(train_id, sizeof(train_id));
		train_id[sizeof(train_id)-1]='\0';

		ar.read(engine_id, sizeof(engine_id));
		engine_id[sizeof(engine_id)-1]='\0';

		short temp1,temp2;
		ar >> send_time >> temp1 >> temp2;
        send_way = (SEND_WAY)temp1;
		oper_type = (OPER_TYPE)temp2;
	    
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgShuntNoteSend2LocoMotive::countObjectLength()
{
	return sizeof(shunt_index) + sizeof(nstation) + sizeof(train_id) + sizeof(engine_id) 
		 + sizeof(send_time)  + sizeof(send_way)  + sizeof(oper_type);
}



IMPLEMENT_CLBTH(MsgShuntNote2LocoMotiveStatus, MSGID_MsgShuntNote2LocoMotiveStatus)
IMPLEMENT_SERIALIZE_OPERATOR(MsgShuntNote2LocoMotiveStatus)
MsgShuntNote2LocoMotiveStatus::MsgShuntNote2LocoMotiveStatus()
{

}
MsgShuntNote2LocoMotiveStatus::~MsgShuntNote2LocoMotiveStatus()
{

}


HRCODE MsgShuntNote2LocoMotiveStatus::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << shunt_index << nstation << (short)status <<
			send_time << sign_time;

		sign_name[sizeof(sign_name)-1]='\0';
		IBYTE len=(IBYTE)strlen(sign_name);
		ar << len;
		ar.write(sign_name, len);

	}
	else
	{
		short t;
		ar >>  shunt_index  >> nstation >> t >>
			send_time >> sign_time;
		status=(STATUS)t;
		IBYTE len;
		ar >> len;
		if(len > sizeof(sign_name)-1)
			return HRCODE(HRCODE::HC_FAIL);

		ar.read(sign_name, len);
		sign_name[len]='\0';

	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgShuntNote2LocoMotiveStatus::countObjectLength()
{
	sign_name[sizeof(sign_name)-1]='\0';

	return sizeof(shunt_index)+ sizeof(nstation) + sizeof(short)+
		sizeof(send_time)+sizeof(sign_time)+
		sizeof(IBYTE)+strlen(sign_name);
}

IMPLEMENT_CLBTH(MsgDataRequest, MSGID_MsgDataRequest)
IMPLEMENT_SERIALIZE_OPERATOR(MsgDataRequest)
MsgDataRequest::MsgDataRequest()
{

}
MsgDataRequest::~MsgDataRequest()
{

}


HRCODE MsgDataRequest::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << msgid;
	}
	else
	{
		ar >> msgid;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgDataRequest::countObjectLength()
{
	return sizeof(msgid);
}

IMPLEMENT_CLBTH(MsgServerProcessEcho, MSGID_MsgServerProcessEcho)
IMPLEMENT_SERIALIZE_OPERATOR(MsgServerProcessEcho)
MsgServerProcessEcho::MsgServerProcessEcho()
{

}
MsgServerProcessEcho::~MsgServerProcessEcho()
{

}


HRCODE MsgServerProcessEcho::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << msgid << recv_time;
	}
	else
	{
		ar >> msgid >> recv_time;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgServerProcessEcho::countObjectLength()
{
	return sizeof(msgid)+sizeof(recv_time);
}

IMPLEMENT_CLBTH(MsgQueryTrainForRoute, MSGID_MsgQueryTrainForRoute)
IMPLEMENT_SERIALIZE_OPERATOR(MsgQueryTrainForRoute)
MsgQueryTrainForRoute::MsgQueryTrainForRoute()
:	nStationNo(INVALID_STATION_NO), nButtonCount(0), nRouteID(NULL_ROUTE_ID), nRouteType(INVALID)
{
	for ( int i = 0; i < MAX_ROUTE_BUTTON_COUNT; i++ )
		strButtonName[i][0] = '\0';
}
MsgQueryTrainForRoute::~MsgQueryTrainForRoute()
{
}

HRCODE MsgQueryTrainForRoute::doSerialize(Archive& ar)
{
	if ( ar.isStoring() )
	{
		ar << nOperID << nStationNo << nButtonCount;
		if ( nButtonCount > MAX_ROUTE_BUTTON_COUNT )
			return HRCODE(HRCODE::HC_FAIL);
		for ( int i = 0; i < nButtonCount; i++ )
		{
			ar << nButtonID[i];
			ar.write( strButtonName[i], sizeof(BUTTON_NAME) );
		}
		ar << (IBYTE)nRouteType << nRouteID;
	}
	else
	{
		ar >> nOperID >> nStationNo >> nButtonCount;
		if ( nButtonCount > MAX_ROUTE_BUTTON_COUNT )
			return HRCODE(HRCODE::HC_FAIL);
		for ( int i = 0; i < nButtonCount; i++ )
		{
			ar >> nButtonID[i];
			ar.read( strButtonName[i], sizeof(BUTTON_NAME) );
			strButtonName[i][sizeof(BUTTON_NAME)-1] = 0;
		}
		IBYTE nTemp;
		ar >> nTemp >> nRouteID;
		nRouteType = (ROUTE_TYPE)nTemp;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgQueryTrainForRoute::countObjectLength()
{
	return sizeof(nOperID) + sizeof(nStationNo) + sizeof(nButtonCount) + sizeof(nRouteID)
		+ MAX_ROUTE_BUTTON_COUNT*(sizeof(BUTTON_ID)+sizeof(BUTTON_NAME)) + sizeof(IBYTE);
}

IMPLEMENT_CLBTH(MsgAnswerTrainForRoute, MSGID_MsgAnswerTrainForRoute)
IMPLEMENT_SERIALIZE_OPERATOR(MsgAnswerTrainForRoute)
MsgAnswerTrainForRoute::MsgAnswerTrainForRoute()
	: nStationNo(INVALID_STATION_NO), nRouteID(NULL_ROUTE_ID)
{
	tnTrainNo[0] = '\0';
}
MsgAnswerTrainForRoute::~MsgAnswerTrainForRoute()
{
}
HRCODE MsgAnswerTrainForRoute::doSerialize(Archive& ar)
{
	if ( ar.isStoring() )
	{
		ar << nOperID << nStationNo << nRouteID;
		ar.write( tnTrainNo, MAX_TRAIN_NO_LEN );
	}
	else
	{
		ar >> nOperID >> nStationNo >> nRouteID;
		ar.read( tnTrainNo, MAX_TRAIN_NO_LEN );
		tnTrainNo[MAX_TRAIN_NO_LEN-1] = '\0';
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgAnswerTrainForRoute::countObjectLength()
{
	return sizeof(nOperID) + sizeof(nStationNo) + sizeof(nRouteID) + MAX_TRAIN_NO_LEN;
}

// 通信状态报告
IMPLEMENT_CLBTH(MsgCommStatusReport, MSGID_MsgCommStatusReport)
IMPLEMENT_SERIALIZE_OPERATOR(MsgCommStatusReport)
MsgCommStatusReport::MsgCommStatusReport()
{
}
MsgCommStatusReport::~MsgCommStatusReport()
{
}
HRCODE MsgCommStatusReport::doSerialize(Archive& ar)
{
	if ( ar.isStoring() )
	{
		ar << nStatus << nIdentify;
	}
	else
	{
		ar >> nStatus >> nIdentify;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgCommStatusReport::countObjectLength()
{
	return sizeof(nStatus)+sizeof(nIdentify);
}

// 探测消息
IMPLEMENT_CLBTH(MsgPeerDetect, MSGID_MsgPeerDetect)
IMPLEMENT_SERIALIZE_OPERATOR(MsgPeerDetect)
MsgPeerDetect::MsgPeerDetect()
{
}
MsgPeerDetect::~MsgPeerDetect()
{
}
HRCODE MsgPeerDetect::doSerialize(Archive& ar)
{
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgPeerDetect::countObjectLength()
{
	return 0;
}
//GSMR特殊消息
IMPLEMENT_CLBTH(MsgRadioTrainInfo, MSGID_GSMR_RADIOTRAINID)
IMPLEMENT_SERIALIZE_OPERATOR(MsgRadioTrainInfo)
MsgRadioTrainInfo::MsgRadioTrainInfo()
{
	train_id[0]='\0';
	locomotive[0]='\0';
	driver[0]='\0';
	conf_trainno[0]='\0';
}
MsgRadioTrainInfo::~MsgRadioTrainInfo()
{
}
HRCODE MsgRadioTrainInfo::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar.write(train_id,MAX_TRAIN_NO_LEN);
		ar.write(locomotive,MAX_TRAIN_NO_LEN);
		ar << motivetype << station_no << distance << speed << car_cnt << car_len;
		ar << total_weight << train_status << train_com;
		ar.write(driver,sizeof(USERNAME));
		ar.write(conf_trainno,MAX_TRAIN_NO_LEN);
		ar.write(othermsg,sizeof(othermsg));
	}
	else
	{
		ar.read(train_id,MAX_TRAIN_NO_LEN);
		train_id[MAX_TRAIN_NO_LEN-1] = '\0';
		ar.read(locomotive,MAX_TRAIN_NO_LEN);
		locomotive[MAX_TRAIN_NO_LEN-1] = '\0';
		ar >> motivetype >> station_no >> distance >> speed >> car_cnt >> car_len;
		ar >> total_weight >> train_status >> train_com;
		ar.read(driver,sizeof(USERNAME));
		driver[sizeof(USERNAME)-1] = '\0';
		ar.read(conf_trainno,MAX_TRAIN_NO_LEN);
		conf_trainno[MAX_TRAIN_NO_LEN-1] = '\0';
		ar.read(othermsg,sizeof(othermsg));
		othermsg[sizeof(othermsg)-1] = '\0';
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgRadioTrainInfo::countObjectLength()
{
	return MAX_TRAIN_NO_LEN+MAX_TRAIN_NO_LEN+sizeof(motivetype)+sizeof(station_no)+sizeof(distance)
		+sizeof(speed)+sizeof(car_cnt)+sizeof(car_len)+sizeof(total_weight)+sizeof(train_status)
		+sizeof(train_com)+sizeof(USERNAME)+MAX_TRAIN_NO_LEN+sizeof(othermsg);
}

IMPLEMENT_CLBTH(MsgMotiveRequestDisplay, MSGID_GSMR_REQTDISPLAY)
IMPLEMENT_SERIALIZE_OPERATOR(MsgMotiveRequestDisplay)
MsgMotiveRequestDisplay::MsgMotiveRequestDisplay()
{
	train_id[0]='\0';
	locomotive[0]='\0';
}
MsgMotiveRequestDisplay::~MsgMotiveRequestDisplay()
{
}
HRCODE MsgMotiveRequestDisplay::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar.write(train_id,MAX_TRAIN_NO_LEN);
		ar.write(locomotive,MAX_TRAIN_NO_LEN);
		ar << motivetype << station_no;
	}
	else
	{
		ar.read(train_id,MAX_TRAIN_NO_LEN);
		ar.read(locomotive,MAX_TRAIN_NO_LEN);
		train_id[MAX_TRAIN_NO_LEN-1] = '\0';
		locomotive[MAX_TRAIN_NO_LEN-1] = '\0';
		ar >> motivetype >> station_no ;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgMotiveRequestDisplay::countObjectLength()
{
	return MAX_TRAIN_NO_LEN+MAX_TRAIN_NO_LEN+sizeof(motivetype)+sizeof(station_no);
}

IMPLEMENT_CLBTH(MsgMotiveDeviceAlarm, MSGID_GSMR_MOTIVEDEVALARM)
IMPLEMENT_SERIALIZE_OPERATOR(MsgMotiveDeviceAlarm)
MsgMotiveDeviceAlarm::MsgMotiveDeviceAlarm()
{
	train_id[0]='\0';
	locomotive[0]='\0';
}
MsgMotiveDeviceAlarm::~MsgMotiveDeviceAlarm()
{
}
HRCODE MsgMotiveDeviceAlarm::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar.write(train_id,MAX_TRAIN_NO_LEN);
		ar.write(locomotive,MAX_TRAIN_NO_LEN);
		ar << motivetype << station_no << alarm_status << alarm_length;
		if ( alarm_length >= sizeof(alarm_dis) )
			return HRCODE(HRCODE::HC_FAIL);
		if(alarm_length>0)
		{
			ar.write(alarm_dis,alarm_length);
		}
	}
	else
	{
		ar.read(train_id,MAX_TRAIN_NO_LEN);
		ar.read(locomotive,MAX_TRAIN_NO_LEN);
		train_id[MAX_TRAIN_NO_LEN-1] = '\0';
		locomotive[MAX_TRAIN_NO_LEN-1] = '\0';
		ar >> motivetype >> station_no >> alarm_status >> alarm_length;
		if ( alarm_length >= sizeof(alarm_dis) )
			return HRCODE(HRCODE::HC_FAIL);
		if(alarm_length>0)
		{
			ar.read(alarm_dis,alarm_length);
			alarm_dis[alarm_length]='\0';
		}
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgMotiveDeviceAlarm::countObjectLength()
{
	return MAX_TRAIN_NO_LEN+MAX_TRAIN_NO_LEN+sizeof(motivetype)+sizeof(station_no)
		+sizeof(alarm_status)+sizeof(alarm_length)+alarm_length;
}

IMPLEMENT_CLBTH(MsgRouteForetell, MSGID_GSMR_ROUTEFORTELL)
IMPLEMENT_SERIALIZE_OPERATOR(MsgRouteForetell)
MsgRouteForetell::MsgRouteForetell()
{
	train_id[0]='\0';
	locomotive[0]='\0';
}
MsgRouteForetell::~MsgRouteForetell()
{
}
HRCODE MsgRouteForetell::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar.write(train_id,MAX_TRAIN_NO_LEN);
		ar.write(locomotive,MAX_TRAIN_NO_LEN);
		ar << motivetype << station_no;
		ar << flag << side << describe_len;
		if(describe_len >= sizeof(describe) )
			return HRCODE(HRCODE::HC_FAIL);
		if(describe_len>0)
			ar.write(describe,describe_len);
	}
	else
	{
		ar.read(train_id,MAX_TRAIN_NO_LEN);
		ar.read(locomotive,MAX_TRAIN_NO_LEN);
		train_id[MAX_TRAIN_NO_LEN-1] = '\0';
		locomotive[MAX_TRAIN_NO_LEN-1] = '\0';
		ar >> motivetype >> station_no ;
		ar >> flag >> side >> describe_len;
		if(describe_len >= sizeof(describe) )
			return HRCODE(HRCODE::HC_FAIL);
		if(describe_len>0)
		{
			ar.read(describe,describe_len);
			describe[describe_len] = '\0';
		}

	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgRouteForetell::countObjectLength()
{
	return MAX_TRAIN_NO_LEN+MAX_TRAIN_NO_LEN+sizeof(motivetype)+sizeof(station_no)
		+sizeof(flag)+sizeof(side)+describe_len;
}

//来自ctci的无线车次信息
IMPLEMENT_CLBTH(MsgGsmrTrainInfo, MSGID_MsgGsmrTrainInfo)
IMPLEMENT_SERIALIZE_OPERATOR(MsgGsmrTrainInfo)
MsgGsmrTrainInfo::MsgGsmrTrainInfo()
{
	CLBTH_ZERO_INIT_OBJECT(BaseMsg)
}
MsgGsmrTrainInfo::~MsgGsmrTrainInfo()
{

}
HRCODE MsgGsmrTrainInfo::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar<<token<<nLen;
		if(nLen>MAX_GSMR_CONTENT_LEN)
			return HRCODE(HRCODE::HC_FAIL);
		if(nLen>0) 
			ar.write(content,nLen);
	}
	else
	{
		ar>>token>>nLen;
		if(nLen>MAX_GSMR_CONTENT_LEN)
			return HRCODE(HRCODE::HC_FAIL);
		if(nLen>0)
		{
			ar.read(content,nLen);
			if(nLen<MAX_GSMR_CONTENT_LEN)
				content[nLen]=0;
		}
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgGsmrTrainInfo::countObjectLength()
{
	return sizeof(token)+sizeof(nLen)+nLen;
}

IMPLEMENT_CLBTH(MsgRadioMotiveDisplay, MSGID_GSMR_MOTIVEDISPLAY)
IMPLEMENT_SERIALIZE_OPERATOR(MsgRadioMotiveDisplay)
MsgRadioMotiveDisplay::MsgRadioMotiveDisplay()
{
	train_id[0]='\0';
	locomotive[0]='\0';
}
MsgRadioMotiveDisplay::~MsgRadioMotiveDisplay()
{
}
HRCODE MsgRadioMotiveDisplay::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar.write(train_id,MAX_TRAIN_NO_LEN);
		ar.write(locomotive,MAX_TRAIN_NO_LEN);
		ar << motivetype << station_no << elementlen;
		if ( elementlen > MAX_CONTENT_SIZE )
			return HRCODE(HRCODE::HC_FAIL);
		ar.write(elementdata,elementlen);
	}
	else
	{
		ar.read(train_id,MAX_TRAIN_NO_LEN);
		ar.read(locomotive,MAX_TRAIN_NO_LEN);
		train_id[MAX_TRAIN_NO_LEN-1] = '\0';
		locomotive[MAX_TRAIN_NO_LEN-1] = '\0';
		ar >> motivetype >> station_no >> elementlen;
		if ( elementlen > MAX_CONTENT_SIZE )
			return HRCODE(HRCODE::HC_FAIL);
		ar.read(elementdata,elementlen);
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgRadioMotiveDisplay::countObjectLength()
{
	return MAX_TRAIN_NO_LEN+MAX_TRAIN_NO_LEN+sizeof(motivetype)+sizeof(station_no)+sizeof(elementlen)+elementlen;
}


IMPLEMENT_CLBTH(MsgTrainIDUpdate, MSGID_GSMR_TRAINIDUPDATE)
IMPLEMENT_SERIALIZE_OPERATOR(MsgTrainIDUpdate)
MsgTrainIDUpdate::MsgTrainIDUpdate()
{
	org_train_id[0]='\0';
	locomotive[0]='\0';
	update_train_id[0]='\0';
}
MsgTrainIDUpdate::~MsgTrainIDUpdate()
{
}
HRCODE MsgTrainIDUpdate::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar.write(org_train_id,MAX_TRAIN_NO_LEN);
		ar.write(locomotive,MAX_TRAIN_NO_LEN);
		ar << motivetype;
		ar.write(update_train_id,MAX_TRAIN_NO_LEN);
		ar.write(describe,sizeof(describe));
	}
	else
	{
		ar.read(org_train_id,MAX_TRAIN_NO_LEN);
		ar.read(locomotive,MAX_TRAIN_NO_LEN);
		org_train_id[MAX_TRAIN_NO_LEN-1] = '\0';
		locomotive[MAX_TRAIN_NO_LEN-1] = '\0';
		ar >> motivetype;
		ar.read(update_train_id,MAX_TRAIN_NO_LEN);
		update_train_id[MAX_TRAIN_NO_LEN-1] = '\0';
		ar.read(describe,sizeof(describe));
		describe[sizeof(describe)-1] = '\0';
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgTrainIDUpdate::countObjectLength()
{
	return MAX_TRAIN_NO_LEN+MAX_TRAIN_NO_LEN+sizeof(motivetype)+MAX_TRAIN_NO_LEN+sizeof(describe);
}

IMPLEMENT_CLBTH(MsgTrainOutofControl, MSGID_GSMR_TRAINOUTOFCONTROL)
IMPLEMENT_SERIALIZE_OPERATOR(MsgTrainOutofControl)
MsgTrainOutofControl::MsgTrainOutofControl()
{
	train_id[0]='\0';
	locomotive[0]='\0';
}
MsgTrainOutofControl::~MsgTrainOutofControl()
{
}
HRCODE MsgTrainOutofControl::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar.write(train_id,MAX_TRAIN_NO_LEN);
		ar.write(locomotive,MAX_TRAIN_NO_LEN);
		ar << motivetype ;
	}
	else
	{
		ar.read(train_id,MAX_TRAIN_NO_LEN);
		ar.read(locomotive,MAX_TRAIN_NO_LEN);
		train_id[MAX_TRAIN_NO_LEN-1] = '\0';
		locomotive[MAX_TRAIN_NO_LEN-1] = '\0';
		ar >> motivetype;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgTrainOutofControl::countObjectLength()
{
	return MAX_TRAIN_NO_LEN+MAX_TRAIN_NO_LEN+sizeof(motivetype);
}

IMPLEMENT_CLBTH(MsgRadioShuntNoteRequest, MSGID_GSMR_REQSHUNTNOTE)
IMPLEMENT_SERIALIZE_OPERATOR(MsgRadioShuntNoteRequest)
MsgRadioShuntNoteRequest::MsgRadioShuntNoteRequest()
{
	train_id[0]='\0';
	locomotive[0]='\0';
}
MsgRadioShuntNoteRequest::~MsgRadioShuntNoteRequest()
{
}
HRCODE MsgRadioShuntNoteRequest::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar.write(train_id,MAX_TRAIN_NO_LEN);
		ar.write(locomotive,MAX_TRAIN_NO_LEN);
		ar << motivetype << stationno << shunt_index << gou_index;
	}
	else
	{
		ar.read(train_id,MAX_TRAIN_NO_LEN);
		ar.read(locomotive,MAX_TRAIN_NO_LEN);
		train_id[MAX_TRAIN_NO_LEN-1] = '\0';
		locomotive[MAX_TRAIN_NO_LEN-1] = '\0';
		ar >> motivetype >> stationno >> shunt_index >> gou_index;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgRadioShuntNoteRequest::countObjectLength()
{
	return MAX_TRAIN_NO_LEN+MAX_TRAIN_NO_LEN+sizeof(motivetype)+sizeof(stationno)+sizeof(shunt_index)+sizeof(gou_index);
}

IMPLEMENT_CLBTH(MsgRadioShuntNote, MSGID_GSMR_REPSHUNTNOTE)
IMPLEMENT_SERIALIZE_OPERATOR(MsgRadioShuntNote)
MsgRadioShuntNote::MsgRadioShuntNote()
{
	train_id[0]='\0';
	locomotive[0]='\0';
}
MsgRadioShuntNote::~MsgRadioShuntNote()
{
}
HRCODE MsgRadioShuntNote::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar.write(train_id,MAX_TRAIN_NO_LEN);
		ar.write(locomotive,MAX_TRAIN_NO_LEN);
		ar << motivetype << stationno;
		ar << shunt_index << send_time << start_time << occ_time;
		ar.write(shunt_group_name,sizeof(shunt_group_name));
		ar.write(shunt_leader_name,sizeof(shunt_leader_name));
		ar << oper_len;
		if(oper_len>sizeof(operation_describe))
			return HRCODE(HRCODE::HC_FAIL);
		if(oper_len>0)
			ar.write(operation_describe,oper_len);
		ar << gou_count;
		if(gou_count>MAX_GOU_PER_SHUNT)
			return HRCODE(HRCODE::HC_FAIL);
		for(int i=0;i<gou_count;i++)
		{
			ar << gou_list[i].shunt_gou_index << gou_list[i].shunt_gou_type << gou_list[i].side << gou_list[i].shunt_car_count << gou_list[i].shunt_note_route_count;
			ar << gou_list[i].shunt_work_time << gou_list[i].route_describe_len;
			if(gou_list[i].route_describe_len>0)
				ar.write(gou_list[i].route_describe,gou_list[i].route_describe_len);
			ar << gou_list[i].gou_describe_len;
			if(gou_list[i].gou_describe_len>0)
				ar.write(gou_list[i].gou_describe,gou_list[i].gou_describe_len);
		}	
	}
	else
	{
		ar.read(train_id,MAX_TRAIN_NO_LEN);
		ar.read(locomotive,MAX_TRAIN_NO_LEN);
		train_id[MAX_TRAIN_NO_LEN-1] = '\0';
		locomotive[MAX_TRAIN_NO_LEN-1] = '\0';
		ar >> motivetype >> stationno;
		ar >> shunt_index >> send_time >> start_time >> occ_time;
		ar.read(shunt_group_name,sizeof(shunt_group_name));
		ar.read(shunt_leader_name,sizeof(shunt_leader_name));
		shunt_group_name[sizeof(shunt_group_name)-1] = '\0';
		shunt_leader_name[sizeof(shunt_leader_name)-1] = '\0';
		ar >> oper_len;
		if(oper_len>sizeof(operation_describe))
			return HRCODE(HRCODE::HC_FAIL);
		if(oper_len>0)
		{
			ar.read(operation_describe,oper_len);
			if(oper_len<sizeof(operation_describe))
				operation_describe[oper_len]='\0';
		}
		ar >> gou_count;
		if(gou_count>MAX_GOU_PER_SHUNT)
			return HRCODE(HRCODE::HC_FAIL);
		for(int i=0;i<gou_count;i++)
		{
			ar >> gou_list[i].shunt_gou_index >> gou_list[i].shunt_gou_type >> gou_list[i].side >> gou_list[i].shunt_car_count >> gou_list[i].shunt_note_route_count;
			ar >> gou_list[i].shunt_work_time >> gou_list[i].route_describe_len;
			if(gou_list[i].route_describe_len>0)
			{
				ar.read(gou_list[i].route_describe,gou_list[i].route_describe_len);
				if(gou_list[i].route_describe_len<sizeof(gou_list[i].route_describe))
					gou_list[i].route_describe[0]='\0';
			}
			ar >> gou_list[i].gou_describe_len;
			if(gou_list[i].gou_describe_len>0)
			{
				ar.read(gou_list[i].gou_describe,gou_list[i].gou_describe_len);
				if(gou_list[i].gou_describe_len<sizeof(gou_list[i].gou_describe))
					gou_list[i].gou_describe[gou_list[i].gou_describe_len]='\0';
			}
		}	
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}

size_t MsgRadioShuntNote::countObjectLength()
{
	int len;
	len =  MAX_TRAIN_NO_LEN+MAX_TRAIN_NO_LEN+sizeof(motivetype)+sizeof(stationno)
		+sizeof(shunt_index)+sizeof(send_time)+sizeof(start_time)+sizeof(occ_time)
		+sizeof(shunt_group_name)+sizeof(shunt_leader_name)+sizeof(oper_len)+oper_len
		+sizeof(gou_count);
	for(int i = 0;i<gou_count;i++)
	{
		len += sizeof(gou_list[i].shunt_gou_index);
		len	+= sizeof(gou_list[i].shunt_gou_type);
		len += sizeof(gou_list[i].side);
		len += sizeof(gou_list[i].shunt_car_count);
		len += sizeof(gou_list[i].shunt_note_route_count);
		len += sizeof(gou_list[i].shunt_work_time);
		len += sizeof(gou_list[i].route_describe_len);
		len	+= gou_list[i].route_describe_len;
		len += sizeof(gou_list[i].gou_describe_len);
		len += gou_list[i].gou_describe_len;
	}
	return len;
}

IMPLEMENT_CLBTH(MsgQueryTrainPosition, MSGID_GSMR_QUERYTRAINPOS)
IMPLEMENT_SERIALIZE_OPERATOR(MsgQueryTrainPosition)
MsgQueryTrainPosition::MsgQueryTrainPosition()
{
	train_id[0]='\0';
}
MsgQueryTrainPosition::~MsgQueryTrainPosition()
{
}
HRCODE MsgQueryTrainPosition::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar.write(train_id,MAX_TRAIN_NO_LEN);
	}
	else
	{
		ar.read(train_id,MAX_TRAIN_NO_LEN);
		train_id[MAX_TRAIN_NO_LEN-1] = '\0';
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgQueryTrainPosition::countObjectLength()
{
	return MAX_TRAIN_NO_LEN;
}

IMPLEMENT_CLBTH(MsgReplyTrainPosition, MSGID_GSMR_REPLYTRAINPOS)
IMPLEMENT_SERIALIZE_OPERATOR(MsgReplyTrainPosition)
MsgReplyTrainPosition::MsgReplyTrainPosition()
{
	train_id[0]='\0';
	for(int i = 0;i<MAX_TRAIN_POSITION;i++)
	{
		train_info[i].locomotive[0]='\0';
	}
}
MsgReplyTrainPosition::~MsgReplyTrainPosition()
{
}
HRCODE MsgReplyTrainPosition::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar.write(train_id,MAX_TRAIN_NO_LEN);
		ar << train_count;
		if(train_count>MAX_TRAIN_POSITION)
			return HRCODE(HRCODE::HC_FAIL);
		for(int i = 0;i<train_count;i++)
		{
			ar.write(train_info[i].locomotive,MAX_TRAIN_NO_LEN);
			ar << train_info[i].motivetype << train_info[i].stationno;
		}
	}
	else
	{
		ar.read(train_id,MAX_TRAIN_NO_LEN);
		train_id[MAX_TRAIN_NO_LEN-1] = '\0';
		ar >> train_count;
		if(train_count>MAX_TRAIN_POSITION)
			return HRCODE(HRCODE::HC_FAIL); 
		for(int i = 0;i<train_count;i++)
		{
			ar.read(train_info[i].locomotive,MAX_TRAIN_NO_LEN);
			train_info[i].locomotive[MAX_TRAIN_NO_LEN-1] = '\0';
			ar >> train_info[i].motivetype >> train_info[i].stationno;
		}
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgReplyTrainPosition::countObjectLength()
{
	int len=0;
	len = MAX_TRAIN_NO_LEN+sizeof(train_count);
	for(int i = 0;i<train_count;i++)
	{
		len += MAX_TRAIN_NO_LEN;
		len += sizeof(train_info[i].motivetype);
		len	+= sizeof(train_info[i].stationno);
	}
	return len;
}

IMPLEMENT_CLBTH(MsgQueryPreStationList, MSGID_GSMR_QUEPRESTATION)
IMPLEMENT_SERIALIZE_OPERATOR(MsgQueryPreStationList)
MsgQueryPreStationList::MsgQueryPreStationList()
{
	train_id[0]='\0';
}
MsgQueryPreStationList::~MsgQueryPreStationList()
{
}
HRCODE MsgQueryPreStationList::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar.write(train_id,MAX_TRAIN_NO_LEN);
		ar << cur_stno << requestcnt;
	}
	else
	{
		ar.read(train_id,MAX_TRAIN_NO_LEN);
		train_id[MAX_TRAIN_NO_LEN-1] = '\0';
		ar >> cur_stno >> requestcnt;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgQueryPreStationList::countObjectLength()
{
	return MAX_TRAIN_NO_LEN+sizeof(cur_stno)+sizeof(requestcnt);
}

IMPLEMENT_CLBTH(MsgReplyPreStationList, MSGID_GSMR_REPPRESTATION)
IMPLEMENT_SERIALIZE_OPERATOR(MsgReplyPreStationList)
MsgReplyPreStationList::MsgReplyPreStationList()
{
	train_id[0]='\0';
}
MsgReplyPreStationList::~MsgReplyPreStationList()
{
}

HRCODE MsgReplyPreStationList::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar.write(train_id,MAX_TRAIN_NO_LEN);
		ar << cur_stno << requestcnt;
		if(requestcnt>MAX_STATION_LIST)
			return HRCODE(HRCODE::HC_FAIL);
		if(requestcnt>0)
		{
			for(int i = 0;i<requestcnt;i++)
				ar << st_list[i];
		}
	}
	else
	{
		ar.read(train_id,MAX_TRAIN_NO_LEN);
		train_id[MAX_TRAIN_NO_LEN-1] = '\0';
		ar >> cur_stno >> requestcnt;
		if(requestcnt>MAX_STATION_LIST)
			return HRCODE(HRCODE::HC_FAIL);
		if(requestcnt>0)
		{
			for(int i = 0;i<requestcnt;i++)
				ar >> st_list[i];
		}
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgReplyPreStationList::countObjectLength()
{
	return MAX_TRAIN_NO_LEN+sizeof(cur_stno)+sizeof(requestcnt)+requestcnt*sizeof(STATION_NO);
}

IMPLEMENT_CLBTH(MsgGsmrLocalMsg, MSGID_GSMR_LOCALMSG)
IMPLEMENT_SERIALIZE_OPERATOR(MsgGsmrLocalMsg)
MsgGsmrLocalMsg::MsgGsmrLocalMsg()
{
	memset(DestIpaddress,0,sizeof(DestIpaddress));
	memset(MsgContent,0,sizeof(MsgContent));
}
MsgGsmrLocalMsg::~MsgGsmrLocalMsg()
{
}

HRCODE MsgGsmrLocalMsg::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar.write(DestIpaddress,sizeof(DestIpaddress));
		ar << DestPort << MsgId << CommandId << Msglen;
		if(Msglen>sizeof(MsgContent))
			return HRCODE(HRCODE::HC_FAIL);
		if(Msglen>0)
		{
			for(int i = 0;i<Msglen;i++)
				ar << MsgContent[i];
		}
	}
	else
	{
		ar.read(DestIpaddress,sizeof(DestIpaddress));
		ar >> DestPort >> MsgId >> CommandId >> Msglen;
		if(Msglen>sizeof(MsgContent))
			return HRCODE(HRCODE::HC_FAIL);
		if(Msglen>0)
		{
			for(int i = 0;i<Msglen;i++)
				ar >> MsgContent[i];
		}
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgGsmrLocalMsg::countObjectLength()
{
	return sizeof(DestIpaddress)+sizeof(DestPort)+sizeof(MsgId)+sizeof(CommandId)+sizeof(Msglen)+Msglen;
}

// 向baselevel查询机车信息
IMPLEMENT_CLBTH(MsgQueryLocomotiveInfo, MSGID_MsgQueryLocomotiveInfo)
IMPLEMENT_SERIALIZE_OPERATOR(MsgQueryLocomotiveInfo)
MsgQueryLocomotiveInfo::MsgQueryLocomotiveInfo()
{
	CLBTH_ZERO_INIT_OBJECT(BaseMsg);
}
MsgQueryLocomotiveInfo::~MsgQueryLocomotiveInfo()
{
}
HRCODE MsgQueryLocomotiveInfo::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << console_no ;

		ar.write(train_id, MAX_TRAIN_NO_LEN );					
	}
	else
	{
		ar >> console_no ;

		ar.read(train_id,MAX_TRAIN_NO_LEN);
		train_id[MAX_TRAIN_NO_LEN-1] = '\0';

	}

	return HRCODE(HRCODE::HC_SUCCESS);
}

size_t MsgQueryLocomotiveInfo::countObjectLength()
{
	return sizeof(console_no) + sizeof(train_id);
}

// 向baselevel查询机车信息(返回信息)
IMPLEMENT_CLBTH(MsgLocomotiveInfo, MSGID_MsgLocomotiveInfo)
IMPLEMENT_SERIALIZE_OPERATOR(MsgLocomotiveInfo)
MsgLocomotiveInfo::MsgLocomotiveInfo()
{
	CLBTH_ZERO_INIT_OBJECT(BaseMsg);
}
MsgLocomotiveInfo::~MsgLocomotiveInfo()
{

}
HRCODE MsgLocomotiveInfo::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << console_no ;

		ar.write(train_id, MAX_TRAIN_NO_LEN );					

		ar<< loco_count;
		int i;
		for( i=0;i<loco_count;i++)
		{   

			ar.write( loco_list[i].locomotive, MAX_TRAIN_NO_LEN );
			ar <<  loco_list[i].kilosignal;

		}
	}
	else
	{

		ar >> console_no ;

		ar.read(train_id,MAX_TRAIN_NO_LEN);

		ar >> loco_count;
		int i;
		for( i=0;i<loco_count;i++)
		{   					
			ar.read( loco_list[i].locomotive, MAX_TRAIN_NO_LEN );	
			loco_list[i].locomotive[MAX_TRAIN_NO_LEN-1] = '\0';
			ar >>  loco_list[i].kilosignal;

		}

	}

	return HRCODE(HRCODE::HC_SUCCESS);
}

size_t MsgLocomotiveInfo::countObjectLength()
{
	return sizeof(console_no) + sizeof(train_id)+ loco_count*sizeof(loco_list[0]) ;
}


IMPLEMENT_CLBTH(MsgElecArmSet, MSGID_MsgElecArmSet)
IMPLEMENT_SERIALIZE_OPERATOR(MsgElecArmSet)
MsgElecArmSet::MsgElecArmSet()
{
	CLBTH_ZERO_INIT_OBJECT(BaseMsg);
}
MsgElecArmSet::~MsgElecArmSet()
{

}
HRCODE MsgElecArmSet::doSerialize(Archive& ar)
{
	short len;
	if(ar.isStoring())
	{
		len = strlen(remark);
		if ( len >= sizeof(remark))
			return HRCODE(HRCODE::HC_FAIL);

		ar << elec_arm_id << elec_sta << set_entity << set_time << status << len;
		if ( len > 0 )
			ar.write(remark, len);
	}
	else
	{
		ar >> elec_arm_id >> elec_sta >> set_entity >> set_time >> status >> len;
		if (len >= sizeof(remark))
			return HRCODE(HRCODE::HC_FAIL);

		if ( len > 0 )
		{
			ar.read(remark, len);			
			remark[len] = 0;
		}
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgElecArmSet::countObjectLength()
{
	return sizeof(elec_arm_id) + sizeof(elec_sta) + sizeof(set_entity) + sizeof(set_time) + sizeof(status) + sizeof(short) + strlen(remark);
}


IMPLEMENT_CLBTH(MsgElecArmQuery, MSGID_MsgElecArmQuery)
IMPLEMENT_SERIALIZE_OPERATOR(MsgElecArmQuery)
MsgElecArmQuery::MsgElecArmQuery()
{
	CLBTH_ZERO_INIT_OBJECT(BaseMsg);
}
MsgElecArmQuery::~MsgElecArmQuery()
{

}
HRCODE MsgElecArmQuery::doSerialize(Archive& ar)
{
	short t;
	if(ar.isStoring())
	{
		if ( elec_arm_count >= sizeof(elec_arm_list)/sizeof(elec_arm_list[0]))
			return HRCODE(HRCODE::HC_FAIL);

		t = query_source;
		ar << t << elec_sta << elec_arm_count;
		for(int i=0; i<elec_arm_count; i++)
			ar << elec_arm_list[i];
	}
	else
	{
		ar >> t >> elec_sta >> elec_arm_count;
		query_source = (SOURCE)t;
		if ( elec_arm_count >= sizeof(elec_arm_list)/sizeof(elec_arm_list[0]))
			return HRCODE(HRCODE::HC_FAIL);

		for(int i=0; i<elec_arm_count; i++)
			ar >> elec_arm_list[i];
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgElecArmQuery::countObjectLength()
{
	return sizeof(short) + sizeof(elec_sta) + sizeof(elec_arm_count) + elec_arm_count*sizeof(elec_arm_list[0]);
}


IMPLEMENT_CLBTH(MsgElecArmAnswer, MSGID_MsgElecArmAnswer)
IMPLEMENT_SERIALIZE_OPERATOR(MsgElecArmAnswer)
MsgElecArmAnswer::MsgElecArmAnswer()
{
}
MsgElecArmAnswer::~MsgElecArmAnswer()
{
}

//早晚点传递
IMPLEMENT_CLBTH(MsgTrainDelayTime, MSGID_MsgTrainDelayTime)
IMPLEMENT_SERIALIZE_OPERATOR(MsgTrainDelayTime)
MsgTrainDelayTime::MsgTrainDelayTime()
{
	tnTrain[0] = '\0';
	nDelayTime = 0;
}
MsgTrainDelayTime::~MsgTrainDelayTime()
{
}
HRCODE MsgTrainDelayTime::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << nStationNo << nDelayTime;
		ar.write( tnTrain, MAX_TRAIN_NO_LEN );
	}
	else
	{
		ar >> nStationNo >> nDelayTime;
		ar.read( tnTrain, MAX_TRAIN_NO_LEN );
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgTrainDelayTime::countObjectLength()
{
	return sizeof(nStationNo) + sizeof(nDelayTime) + MAX_TRAIN_NO_LEN;
}


IMPLEMENT_CLBTH(MsgPackRequest, MSGID_MsgPackRequest)
IMPLEMENT_SERIALIZE_OPERATOR(MsgPackRequest)
MsgPackRequest::MsgPackRequest()
{
}
MsgPackRequest::~MsgPackRequest()
{

}
HRCODE MsgPackRequest::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		if(entity_count >= sizeof(recver)/sizeof(recver[0]))
			return HRCODE(HRCODE::HC_FAIL);

		ar << flag << entity_count;
		for (int i=0; i<entity_count; i++)
			ar << sender[i] << recver[i];
	}
	else
	{
		ar >> flag >> entity_count;
		if(entity_count >= sizeof(recver)/sizeof(recver[0]))
			return HRCODE(HRCODE::HC_FAIL);

		for (int i=0; i<entity_count; i++)
			ar >> sender[i] >> recver[i];
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgPackRequest::countObjectLength()
{
	return sizeof(flag) + sizeof(entity_count) + entity_count*(sizeof(recver[0]) + sizeof(sender[0]));
}


IMPLEMENT_CLBTH(MsgCollector, MSGID_MsgCollector)
IMPLEMENT_SERIALIZE_OPERATOR(MsgCollector)
MsgCollector::MsgCollector()
{
	sender = recver = INVALID_ENTITY_ID;
	id = 0;
	memset(para, 0x00, sizeof(para));
}
MsgCollector::~MsgCollector()
{

}
HRCODE MsgCollector::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << sender << recver << id;
		ar.write(para, 16);
	}
	else
	{
		ar >> sender >> recver >> id;
		ar.read(para, 16);
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgCollector::countObjectLength()
{
	return sizeof(sender) + sizeof(recver) + sizeof(id) + 16;
}

IMPLEMENT_CLBTH(MsgPackAnswer, MSGID_MsgPackAnswer)
IMPLEMENT_SERIALIZE_OPERATOR(MsgPackAnswer)
MsgPackAnswer::MsgPackAnswer()
{
}
MsgPackAnswer::~MsgPackAnswer()
{

}
HRCODE MsgPackAnswer::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		if (pack_len >= sizeof(package)/sizeof(package[0]))
			return HRCODE(HRCODE::HC_FAIL);

		ar << msg_attrib << dest_id << source_id << tm << subtm 
			<< label << data_len << check_sum << spare;

		ar << pack_len;
		ar.write(package, pack_len);

	}
	else
	{
		ar >> msg_attrib >> dest_id >> source_id >> tm >> subtm 
			>> label >> data_len >> check_sum >> spare;

		ar >> pack_len;
		if (pack_len >= sizeof(package)/sizeof(package[0]))
			return HRCODE(HRCODE::HC_FAIL);

		ar.read(package, pack_len);
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgPackAnswer::countObjectLength()
{
	return sizeof(msg_attrib)+sizeof(dest_id)+sizeof(source_id)+sizeof(tm)+sizeof(subtm)
		+sizeof(label)+sizeof(data_len)+sizeof(check_sum)+sizeof(spare)
		+sizeof(pack_len) + pack_len;
}

IMPLEMENT_CLBTH(MsgDCmdTitle, MSGID_MsgDCmdTitle)
IMPLEMENT_SERIALIZE_OPERATOR(MsgDCmdTitle)
MsgDCmdTitle::MsgDCmdTitle()
{
	CLBTH_ZERO_INIT_OBJECT(BaseMsg)
}
MsgDCmdTitle::~MsgDCmdTitle()
{

}
HRCODE MsgDCmdTitle::doSerialize(Archive& ar)
{
	short len;

	if(ar.isStoring())
	{
		ar << send_entity << create_time << send_time << edit_time;
		ar << con_id << source_station << bian_ma;

		AR_WRITE_STRING(cmd_no, len);

		AR_WRITE_STRING(cmd_name, len);

		AR_WRITE_STRING(rereader, len);

		AR_WRITE_STRING(dispatcher, len);

		AR_WRITE_STRING(unit, len);

		AR_WRITE_STRING(reader, len);

		ar << read_time;
		AR_WRITE_STRING(recver, len);

		ar << (short)status;
		ar << (short)cmd_destination;
		ar << (short)cmd_oper;
	}
	else
	{
		ar >> send_entity >> create_time >> send_time >> edit_time;
		ar >> con_id >> source_station >> bian_ma;

		AR_READ_STRING(cmd_no, len);

		AR_READ_STRING(cmd_name, len);

		AR_READ_STRING(rereader, len);

		AR_READ_STRING(dispatcher, len);

		AR_READ_STRING(unit, len);

		AR_READ_STRING(reader, len);
		ar >> read_time;

		AR_READ_STRING(recver, len);

		short t;
		ar >> t;
		status = (Status)t;

		ar >> t;
		cmd_destination =(CMD_DESTINATION)t;	

		ar >> t;
		cmd_oper =(CMD_OPER)t;	

	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgDCmdTitle::countObjectLength()
{
	return sizeof(send_entity)+sizeof(create_time)+sizeof(send_time)+sizeof(edit_time)
		+sizeof(con_id)+sizeof(source_station)+sizeof(bian_ma)
		+sizeof(short)*7
		+strlen(cmd_no)
		+strlen(cmd_name)
		+strlen(rereader)
		+sizeof(read_time)
		+strlen(dispatcher)
		+strlen(unit)
		+strlen(reader)
		+strlen(recver)
		+sizeof(short)
		+sizeof(short)
		+sizeof(short);
}

IMPLEMENT_CLBTH(MsgDCmdSendList, MSGID_MsgDCmdSendList)
IMPLEMENT_SERIALIZE_OPERATOR(MsgDCmdSendList)
MsgDCmdSendList::MsgDCmdSendList()
{
	CLBTH_ZERO_INIT_OBJECT(BaseMsg)
}
MsgDCmdSendList::~MsgDCmdSendList()
{

}
HRCODE MsgDCmdSendList::doSerialize(Archive& ar)
{
	short len;

	if(ar.isStoring())
	{
		ar << send_entity << create_time << (short)cmd_type ;

		AR_WRITE_COUNT(recv_count, recv_list);
		int i = 0;
		for (i=0; i<recv_count; i++)
		{
			ar << recv_list[i].recv_entity << recv_list[i].station << (short)recv_list[i].recv_status << recv_list[i].recv_time; 
			AR_WRITE_STRING(recv_list[i].sign_name, len);
			AR_WRITE_STRING(recv_list[i].chaosong_memo, len);
		}

		AR_WRITE_COUNT(chaosong_recv_count, chaosong_list);
		for (i=0; i<chaosong_recv_count; i++)
		{
			ar << chaosong_list[i].recv_entity<< chaosong_list[i].station  << (short)chaosong_list[i].recv_status << chaosong_list[i].recv_time; 
			AR_WRITE_STRING(chaosong_list[i].sign_name, len);
			AR_WRITE_STRING(chaosong_list[i].chaosong_memo, len);
		}

		ar << (short)send_way;
		ar << index;

		AR_WRITE_COUNT(train_list_count, train_list);
		for (i=0; i<train_list_count; i++)
		{
			ar << train_list[i].recv_lirc <<(short)train_list[i].recv_status<< train_list[i].recv_time << train_list[i].station;

			AR_WRITE_STRING(train_list[i].train_id, len);
			AR_WRITE_STRING(train_list[i].locomotive, len);
			AR_WRITE_STRING(train_list[i].sign_name, len);
		}
	}
	else
	{
		short t;
		ar >> send_entity >> create_time >> t;
		cmd_type = (CMD_TYPE)t;


		AR_READ_COUNT(recv_count, recv_list);
		int i = 0;
		for (i=0; i<recv_count; i++)
		{
			ar >> recv_list[i].recv_entity >> recv_list[i].station >> t >> recv_list[i].recv_time; 
			recv_list[i].recv_status = (RECV_STATUS) t;
			AR_READ_STRING(recv_list[i].sign_name, len);
			AR_READ_STRING(recv_list[i].chaosong_memo, len);
		}

		AR_READ_COUNT(chaosong_recv_count, chaosong_list);
		for (i=0; i<chaosong_recv_count; i++)
		{
			ar >> chaosong_list[i].recv_entity >> chaosong_list[i].station >> t >> chaosong_list[i].recv_time; 
			chaosong_list[i].recv_status = (RECV_STATUS) t;
			AR_READ_STRING(chaosong_list[i].sign_name, len);
			AR_READ_STRING(chaosong_list[i].chaosong_memo, len);
		}

		ar >> t;
        send_way = (SEND_WAY)t;
		ar >> index;

		AR_READ_COUNT(train_list_count,  train_list);
		for (i=0; i< train_list_count; i++)
		{
			ar >> train_list[i].recv_lirc >> t >> train_list[i].recv_time >> train_list[i].station;
            train_list[i].recv_status = (RECV_STATUS)t;

			AR_READ_STRING( train_list[i].train_id, len);
			AR_READ_STRING( train_list[i].locomotive, len);
			AR_READ_STRING( train_list[i].sign_name, len);
		}

	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgDCmdSendList::countObjectLength()
{
	size_t len = sizeof(send_entity)+sizeof(create_time)+sizeof(short);

	len += sizeof(short);
	int i = 0;
	for (i=0; i<recv_count; i++)
	{
		len +=  sizeof(recv_list[i].recv_entity)+sizeof(recv_list[i].station)+sizeof(short)+sizeof(recv_list[i].recv_time);
		len +=  sizeof(short) + strlen(recv_list[i].sign_name); 
		len +=  sizeof(short) + strlen(recv_list[i].chaosong_memo); 
	}

	len += sizeof(short);
	for (i=0; i<chaosong_recv_count; i++)
	{
		len +=  sizeof(chaosong_list[i].recv_entity)+sizeof(chaosong_list[i].station)+sizeof(short)+sizeof(chaosong_list[i].recv_time);
		len +=  sizeof(short) + strlen(chaosong_list[i].sign_name); 
		len +=  sizeof(short) + strlen(chaosong_list[i].chaosong_memo); 
	}

	len += sizeof(short)+sizeof(index);

	len += sizeof(short);
	for (i=0; i<train_list_count; i++)
	{
		len += sizeof(train_list[i].recv_lirc)+sizeof(short)+sizeof(train_list[i].recv_time)+sizeof(train_list[i].station);
		len += sizeof(short) + strlen(train_list[i].train_id); 
		len += sizeof(short) + strlen(train_list[i].locomotive); 
		len += sizeof(short) + strlen(train_list[i].sign_name); 
	}

	return len;
}

IMPLEMENT_CLBTH(MsgDCmdContent, MSGID_MsgDCmdContent)
IMPLEMENT_SERIALIZE_OPERATOR(MsgDCmdContent)
MsgDCmdContent::MsgDCmdContent()
{
	CLBTH_ZERO_INIT_OBJECT(BaseMsg)
}
MsgDCmdContent::~MsgDCmdContent()
{

}
HRCODE MsgDCmdContent::doSerialize(Archive& ar)
{
	short len;
	if(ar.isStoring())
	{
		ar << send_entity << create_time << content_seq;
		AR_WRITE_STRING(content, len);
	}
	else
	{
		ar >> send_entity >> create_time >> content_seq;
		AR_READ_STRING(content, len);
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgDCmdContent::countObjectLength()
{
	return sizeof(send_entity)+sizeof(create_time)+sizeof(content_seq)+sizeof(short)+strlen(content);
}


IMPLEMENT_CLBTH(MsgDCmdData, MSGID_MsgDCmdData)
IMPLEMENT_SERIALIZE_OPERATOR(MsgDCmdData)
MsgDCmdData::MsgDCmdData()
{
	
}
MsgDCmdData::~MsgDCmdData()
{

}
HRCODE MsgDCmdData::doSerialize(Archive& ar)
{
	if (HRCODE(HRCODE::HC_SUCCESS) != title.serialize(ar) 
	 || HRCODE(HRCODE::HC_SUCCESS) != sendlist.serialize(ar)
	 || HRCODE(HRCODE::HC_SUCCESS) != first_package.serialize(ar) )
		return HRCODE(HRCODE::HC_FAIL);

	if(ar.isStoring())
	{
		ar << tcc_len;
		ar.write(tcc_total, tcc_len);
		ar << content_len << content_count;
	}
	else
	{
		ar >> tcc_len;
		if (tcc_len > sizeof(tcc_total))
			return HRCODE(HRCODE::HC_FAIL);

		ar.read(tcc_total, tcc_len);
		ar >> content_len >> content_count;

	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgDCmdData::countObjectLength()
{
	return title.getObjectLength()+sendlist.getObjectLength()+first_package.getObjectLength()+
		sizeof(tcc_len)+tcc_len+sizeof(content_len)+sizeof(content_count);
}


IMPLEMENT_CLBTH(MsgDCmdInsert, MSGID_MsgDCmdInsert)
IMPLEMENT_SERIALIZE_OPERATOR(MsgDCmdInsert)
MsgDCmdInsert::MsgDCmdInsert()
{

}
MsgDCmdInsert::~MsgDCmdInsert()
{

}
HRCODE MsgDCmdInsert::doSerialize(Archive& ar)
{
	return MsgDCmdData::doSerialize(ar);
}
size_t MsgDCmdInsert::countObjectLength()
{
	return MsgDCmdData::countObjectLength();
}


IMPLEMENT_CLBTH(MsgDCmdDelete, MSGID_MsgDCmdDelete)
IMPLEMENT_SERIALIZE_OPERATOR(MsgDCmdDelete)
MsgDCmdDelete::MsgDCmdDelete()
{
	CLBTH_ZERO_INIT_OBJECT(BaseMsg)
}
MsgDCmdDelete::~MsgDCmdDelete()
{

}
HRCODE MsgDCmdDelete::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << con_id << send_entity << create_time;
	}
	else
	{
		ar >> con_id >> send_entity >> create_time;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgDCmdDelete::countObjectLength()
{
	return sizeof(con_id)+sizeof(send_entity)+sizeof(create_time);
}

IMPLEMENT_CLBTH(MsgDCmdTitleLoad, MSGID_MsgDCmdTitleLoad)
IMPLEMENT_SERIALIZE_OPERATOR(MsgDCmdTitleLoad)
MsgDCmdTitleLoad::MsgDCmdTitleLoad()
{
	CLBTH_ZERO_INIT_OBJECT(BaseMsg)
}
MsgDCmdTitleLoad::~MsgDCmdTitleLoad()
{

}
HRCODE MsgDCmdTitleLoad::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << con_id << begin_time << end_time;
	}
	else
	{
		ar >> con_id >> begin_time >> end_time;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgDCmdTitleLoad::countObjectLength()
{
	return sizeof(con_id)+sizeof(begin_time)+sizeof(end_time);
}

IMPLEMENT_CLBTH(MsgDCmdTitleClose, MSGID_MsgDCmdTitleClose)
IMPLEMENT_SERIALIZE_OPERATOR(MsgDCmdTitleClose)
MsgDCmdTitleClose::MsgDCmdTitleClose()
{
	CLBTH_ZERO_INIT_OBJECT(BaseMsg)
}
MsgDCmdTitleClose::~MsgDCmdTitleClose()
{
}


IMPLEMENT_CLBTH(MsgDCmdLoad, MSGID_MsgDCmdLoad)
IMPLEMENT_SERIALIZE_OPERATOR(MsgDCmdLoad)
MsgDCmdLoad::MsgDCmdLoad()
{
	CLBTH_ZERO_INIT_OBJECT(BaseMsg)
}
MsgDCmdLoad::~MsgDCmdLoad()
{

}
HRCODE MsgDCmdLoad::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << send_entity << create_time;
	}
	else
	{
		ar >> send_entity >> create_time;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgDCmdLoad::countObjectLength()
{
	return sizeof(send_entity)+sizeof(create_time);
}

IMPLEMENT_CLBTH(MsgDCmdLoad2, MSGID_MsgDCmdLoad2)
IMPLEMENT_SERIALIZE_OPERATOR(MsgDCmdLoad2)
MsgDCmdLoad2::MsgDCmdLoad2()
{
	CLBTH_ZERO_INIT_OBJECT(BaseMsg)
}
MsgDCmdLoad2::~MsgDCmdLoad2()
{

}
HRCODE MsgDCmdLoad2::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << send_entity << create_time;
	}
	else
	{
		ar >> send_entity >> create_time;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgDCmdLoad2::countObjectLength()
{
	return sizeof(send_entity)+sizeof(create_time);
}

IMPLEMENT_CLBTH(MsgDCmdSend, MSGID_MsgDCmdSend)
IMPLEMENT_SERIALIZE_OPERATOR(MsgDCmdSend)
MsgDCmdSend::MsgDCmdSend()
{

}
MsgDCmdSend::~MsgDCmdSend()
{

}
HRCODE MsgDCmdSend::doSerialize(Archive& ar)
{
	return MsgDCmdData::doSerialize(ar);
}
size_t MsgDCmdSend::countObjectLength()
{
	return MsgDCmdData::countObjectLength();
}

IMPLEMENT_CLBTH(MsgDCmdRecv, MSGID_MsgDCmdRecv)
IMPLEMENT_SERIALIZE_OPERATOR(MsgDCmdRecv)
MsgDCmdRecv::MsgDCmdRecv()
{
	CLBTH_ZERO_INIT_OBJECT(BaseMsg)
}
MsgDCmdRecv::~MsgDCmdRecv()
{

}
HRCODE MsgDCmdRecv::doSerialize(Archive& ar)
{
	char len;

	if(ar.isStoring())
	{
		ar << send_entity << create_time << station << recv_time << recv_entity << (short)recv_status;

		AR_WRITE_STRING(train_id, len);

		AR_WRITE_STRING(locomotive, len);

		AR_WRITE_STRING(sign_name, len);

		AR_WRITE_STRING(cmdno, len);

	}
	else
	{
		short t;
		ar >> send_entity >> create_time >> station >> recv_time >> recv_entity >> t;
		recv_status = RECV_STATUS(t);

		AR_READ_STRING(train_id, len);

		AR_READ_STRING(locomotive, len);

		AR_READ_STRING(sign_name, len);

		AR_READ_STRING(cmdno, len);
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgDCmdRecv::countObjectLength()
{
	return sizeof(send_entity)+sizeof(create_time)+sizeof(station)+sizeof(recv_time)
		+sizeof(recv_entity)+sizeof(short)
		+sizeof(char)*3+strlen(train_id)+strlen(locomotive)+strlen(sign_name)+strlen(cmdno);
}


IMPLEMENT_CLBTH(MsgDCmdStatus, MSGID_MsgDCmdStatus)
IMPLEMENT_SERIALIZE_OPERATOR(MsgDCmdStatus)
MsgDCmdStatus::MsgDCmdStatus()
{
	
}
MsgDCmdStatus::~MsgDCmdStatus()
{

}
HRCODE MsgDCmdStatus::doSerialize(Archive& ar)
{
	if (HRCODE(HRCODE::HC_SUCCESS) != title.serialize(ar) 
	 || HRCODE(HRCODE::HC_SUCCESS) != sendlist.serialize(ar))
		return HRCODE(HRCODE::HC_FAIL);

	if(ar.isStoring())
	{
		ar << tcc_len;
		ar.write(tcc_total, tcc_len);
	}
	else
	{
		ar >> tcc_len;
		if (tcc_len > sizeof(tcc_total))
			return HRCODE(HRCODE::HC_FAIL);

		ar.read(tcc_total, tcc_len);
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgDCmdStatus::countObjectLength()
{
	return title.getObjectLength()+sendlist.getObjectLength()+sizeof(tcc_len)+tcc_len;
}

IMPLEMENT_CLBTH(MsgDcmd_WirelessBroadcast, MSGID_MsgDcmd_WirelessBroadcast)
IMPLEMENT_SERIALIZE_OPERATOR(MsgDcmd_WirelessBroadcast)
MsgDcmd_WirelessBroadcast::MsgDcmd_WirelessBroadcast()
{
	memset(broadcast_id,0,sizeof(broadcast_id));
	memset(dcmd_id,0,sizeof(dcmd_id));
	gen_time = 0;
	is_startnow = 0;
	is_neverstop = 0;
	plan_start_time = 0;
	plan_end_time = 0;

	station_id = 0;
//	memset(station_name,0,sizeof(station_name));
	real_start_time = 0;
	real_end_time = 0;
}
MsgDcmd_WirelessBroadcast::~MsgDcmd_WirelessBroadcast()
{
}
HRCODE MsgDcmd_WirelessBroadcast::doSerialize(Archive& ar)
{
	IWORD sublen = 0;

	if(ar.isStoring())
	{
		AR_WRITE_STRING(broadcast_id,sublen);
		AR_WRITE_STRING(dcmd_id,sublen);
		ar<<gen_time<<is_startnow<<is_neverstop<<plan_start_time<<plan_end_time<<station_id;
//		AR_WRITE_STRING(station_name,sublen);
		ar<<(short)direction<<(short)status<<(short)train_type<<real_start_time<<real_end_time;
	}
	else
	{
		AR_READ_STRING(broadcast_id,sublen);
		AR_READ_STRING(dcmd_id,sublen);
		ar>>gen_time>>is_startnow>>is_neverstop>>plan_start_time>>plan_end_time>>station_id;
//		AR_READ_STRING(station_name,sublen);
		short t;
		ar>>t;
		direction = (DIRECTION)t;
		ar>>t;
		status = (STATUS)t;
		ar>>t;
		train_type = (TRAINTYPE)t;
		ar>>real_start_time>>real_end_time;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgDcmd_WirelessBroadcast::countObjectLength()
{
	return strlen(broadcast_id)+sizeof(short)
		+strlen(dcmd_id)+sizeof(short)
		+sizeof(gen_time)
		+sizeof(is_startnow)+sizeof(is_neverstop)
		+sizeof(plan_start_time)+sizeof(plan_end_time)
		+sizeof(station_id)
//		+strlen(station_name)+sizeof(short)
		+sizeof(short)
		+sizeof(short)
		+sizeof(short)
		+sizeof(real_start_time)
		+sizeof(real_end_time);
}

IMPLEMENT_CLBTH(MsgDcmd_BroadCast_Send, MSGID_MsgDcmd_BroadCast_Send)
IMPLEMENT_SERIALIZE_OPERATOR(MsgDcmd_BroadCast_Send)
MsgDcmd_BroadCast_Send::MsgDcmd_BroadCast_Send()
{
	memset(dcmdcontent.dcmd_id,0,sizeof(dcmdcontent.dcmd_id));
	memset(dcmdcontent.dcmd_style_title,0,sizeof(dcmdcontent.dcmd_style_title));
	memset(dcmdcontent.dcmd_num,0,sizeof(dcmdcontent.dcmd_num));
	memset(dcmdcontent.org_cmd_num,0,sizeof(dcmdcontent.org_cmd_num));

	dcmdcontent.send_entity = 0;
	dcmdcontent.create_time = 0;
	memset(dcmdcontent.creater_name,0,sizeof(dcmdcontent.creater_name));
	dcmdcontent.send_time = 0;
	dcmdcontent.edit_time = 0;

	dcmdcontent.sender_type = 0;
	dcmdcontent.sender_id = 0;
	memset(dcmdcontent.sender_name,0,sizeof(dcmdcontent.sender_name));
	memset(dcmdcontent.sender_unit_name,0,sizeof(dcmdcontent.sender_unit_name));
	memset(dcmdcontent.rereader_name,0,sizeof(dcmdcontent.rereader_name));
	memset(dcmdcontent.dutychief_name,0,sizeof(dcmdcontent.dutychief_name));
	dcmdcontent.author_time = 0;
	dcmdcontent.author_status = 0;
	memset(dcmdcontent.recver,0,sizeof(dcmdcontent.recver));

	dcmdcontent.need_author = 0;
	dcmdcontent.tcc_len = 0;

	dcmdcontent.status = 0;
	memset(dcmdcontent.jhh,0,sizeof(dcmdcontent.jhh));
	memset(dcmdcontent.content,0,sizeof(dcmdcontent.content));

	nSendDestCount = 0;
	nUsedIndex = 0;
}
MsgDcmd_BroadCast_Send::~MsgDcmd_BroadCast_Send()
{

}
HRCODE MsgDcmd_BroadCast_Send::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << nSendDestCount << nUsedIndex;
	}
	else
	{
		ar >> nSendDestCount >> nUsedIndex;
	}
	if ( nSendDestCount > MAX_SEND_COUNT )
		return HRCODE(HRCODE::HC_FAIL);
	for ( int n = 0; n < nSendDestCount; n++ )
	{
		if (HRCODE(HRCODE::HC_SUCCESS) != wbInfo[n].doSerialize(ar))
			return HRCODE(HRCODE::HC_FAIL);
	}
	IWORD sublen = 0;
	if(ar.isStoring())
	{
		AR_WRITE_STRING(dcmdcontent.dcmd_id,sublen);
		AR_WRITE_STRING(dcmdcontent.dcmd_style_title,sublen);
		AR_WRITE_STRING(dcmdcontent.dcmd_num,sublen);
		AR_WRITE_STRING(dcmdcontent.org_cmd_num,sublen);

		ar<<dcmdcontent.send_entity<<dcmdcontent.create_time;

		AR_WRITE_STRING(dcmdcontent.creater_name,sublen);
		
		ar<<dcmdcontent.send_time<<dcmdcontent.edit_time<<dcmdcontent.sender_type<<dcmdcontent.sender_id;

		AR_WRITE_STRING(dcmdcontent.sender_name,sublen);
		AR_WRITE_STRING(dcmdcontent.sender_unit_name,sublen);
		AR_WRITE_STRING(dcmdcontent.rereader_name,sublen);
		AR_WRITE_STRING(dcmdcontent.dutychief_name,sublen);
		
		ar<<dcmdcontent.author_time<<dcmdcontent.author_status;

        AR_WRITE_STRING(dcmdcontent.recver,sublen);
		//此项对无线调度命令
		ar<<(short)dcmdcontent.cmd_type<<(short)dcmdcontent.cmd_destination<<(short)dcmdcontent.cmd_oper;
		ar<<dcmdcontent.need_author<<dcmdcontent.tcc_len<<(short)dcmdcontent.send_way<<dcmdcontent.status;

		AR_WRITE_STRING(dcmdcontent.jhh,sublen);
		AR_WRITE_STRING(dcmdcontent.content,sublen);
	}
	else
	{
		AR_READ_STRING(dcmdcontent.dcmd_id,sublen);
		AR_READ_STRING(dcmdcontent.dcmd_style_title,sublen);
		AR_READ_STRING(dcmdcontent.dcmd_num,sublen);
		AR_READ_STRING(dcmdcontent.org_cmd_num,sublen);

		ar>>dcmdcontent.send_entity>>dcmdcontent.create_time;

		AR_READ_STRING(dcmdcontent.creater_name,sublen);
		
		ar>>dcmdcontent.send_time>>dcmdcontent.edit_time>>dcmdcontent.sender_type>>dcmdcontent.sender_id;

		AR_READ_STRING(dcmdcontent.sender_name,sublen);
		AR_READ_STRING(dcmdcontent.sender_unit_name,sublen);
		AR_READ_STRING(dcmdcontent.rereader_name,sublen);
		AR_READ_STRING(dcmdcontent.dutychief_name,sublen);
		
		ar>>dcmdcontent.author_time>>dcmdcontent.author_status;

        AR_READ_STRING(dcmdcontent.recver,sublen);
		//此项对无线调度命令
		short t;
		ar>>t;
		dcmdcontent.cmd_type = DCMD::CMD_TYPE(t);
		ar>>t;
		dcmdcontent.cmd_destination = DCMD::CMD_DESTINATION(t);
		ar>>t;
		dcmdcontent.cmd_oper = DCMD::CMD_OPER(t);
		ar>>dcmdcontent.need_author>>dcmdcontent.tcc_len;
		ar>>t;
		dcmdcontent.send_way = DCMD::SEND_WAY(t);
		ar>>dcmdcontent.status;

		AR_READ_STRING(dcmdcontent.jhh,sublen);
		AR_READ_STRING(dcmdcontent.content,sublen);
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgDcmd_BroadCast_Send::countObjectLength()
{
	size_t info_len = 0;
	for ( int i = 0; i < nSendDestCount; i++ )
		info_len += wbInfo[i].countObjectLength();
	return sizeof(nSendDestCount) + sizeof(nUsedIndex) + info_len
		+ strlen(dcmdcontent.dcmd_id)+sizeof(short)
		+ strlen(dcmdcontent.dcmd_style_title)+sizeof(short)
		+ strlen(dcmdcontent.dcmd_num)+sizeof(short)
		+ strlen(dcmdcontent.org_cmd_num)+sizeof(short)
		+sizeof(dcmdcontent.send_entity)+sizeof(dcmdcontent.create_time)
		+strlen(dcmdcontent.creater_name)+sizeof(short)		
		+sizeof(dcmdcontent.send_time)+sizeof(dcmdcontent.edit_time)+sizeof(dcmdcontent.sender_type)+sizeof(dcmdcontent.sender_id)
		+ strlen(dcmdcontent.sender_name)+sizeof(short)
		+ strlen(dcmdcontent.sender_unit_name)+sizeof(short)
		+ strlen(dcmdcontent.rereader_name)+sizeof(short)
		+ strlen(dcmdcontent.dutychief_name)+sizeof(short)		
		+sizeof(dcmdcontent.author_time)+sizeof(dcmdcontent.author_status)
        +strlen(dcmdcontent.recver)+sizeof(short)
		+sizeof(short)+sizeof(short)+sizeof(short)
		+sizeof(dcmdcontent.need_author)+sizeof(dcmdcontent.tcc_len)
		+sizeof(short)+sizeof(dcmdcontent.status)
		+strlen(dcmdcontent.jhh)+sizeof(short)
		+strlen(dcmdcontent.content)+sizeof(short);
}

IMPLEMENT_CLBTH(MsgDcmd_BroadCast_Status, MSGID_MsgDcmd_BroadCast_Status)
IMPLEMENT_SERIALIZE_OPERATOR(MsgDcmd_BroadCast_Status)
MsgDcmd_BroadCast_Status::MsgDcmd_BroadCast_Status()
{
	memset(broadcast_id,0,sizeof(broadcast_id));
	memset(dcmd_id,0,sizeof(dcmd_id));
	station_entity = 0;
	memset(station_name,0,sizeof(station_name));
	status = 0;
	oper_time = 0;	
}
MsgDcmd_BroadCast_Status::~MsgDcmd_BroadCast_Status()
{

}
HRCODE MsgDcmd_BroadCast_Status::doSerialize(Archive& ar)
{
	IWORD sublen = 0;
	if(ar.isStoring())
	{
		AR_WRITE_STRING(broadcast_id,sublen);
		AR_WRITE_STRING(dcmd_id,sublen);
		ar<<station_entity;
		AR_WRITE_STRING(station_name,sublen);
		ar<<(short)direction<<status<<oper_time;
	}
	else
	{
		AR_READ_STRING(broadcast_id,sublen);
		AR_READ_STRING(dcmd_id,sublen);
		ar>>station_entity;
		AR_READ_STRING(station_name,sublen);
		short t;
		ar>>t;
		direction = (DIRECTION)t;
		ar>>status>>oper_time;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgDcmd_BroadCast_Status::countObjectLength()
{
	return strlen(broadcast_id)+sizeof(short)
			+strlen(dcmd_id)+sizeof(short)
			+sizeof(station_entity)
			+strlen(station_name)+sizeof(short)
			+sizeof(short)
			+sizeof(status)+sizeof(oper_time);
}

IMPLEMENT_CLBTH(MsgDcmd_BroadCast_Command, MSGID_MsgDcmd_BroadCast_Command)
IMPLEMENT_SERIALIZE_OPERATOR(MsgDcmd_BroadCast_Command)
MsgDcmd_BroadCast_Command::MsgDcmd_BroadCast_Command()
{
	commandid = 0;
}
MsgDcmd_BroadCast_Command::~MsgDcmd_BroadCast_Command()
{

}
HRCODE MsgDcmd_BroadCast_Command::doSerialize(Archive& ar)
{
	IWORD sublen = 0;
	if(ar.isStoring())
	{
		AR_WRITE_STRING(broadcast_id,sublen);
		AR_WRITE_STRING(dcmd_id,sublen);
		ar<<commandid;
	}
	else
	{
		AR_READ_STRING(broadcast_id,sublen);
		AR_READ_STRING(dcmd_id,sublen);
		ar>>commandid;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgDcmd_BroadCast_Command::countObjectLength()
{
	return strlen(broadcast_id)+sizeof(short)+strlen(dcmd_id)+sizeof(short)+sizeof(commandid);
}

IMPLEMENT_CLBTH(MsgDCmdCurrentNoRequest, MSGID_MsgDCmdCurrentNoRequest)
IMPLEMENT_SERIALIZE_OPERATOR(MsgDCmdCurrentNoRequest)
MsgDCmdCurrentNoRequest::MsgDCmdCurrentNoRequest()
{
	CLBTH_ZERO_INIT_OBJECT(BaseMsg)
}
MsgDCmdCurrentNoRequest::~MsgDCmdCurrentNoRequest()
{

}
HRCODE MsgDCmdCurrentNoRequest::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << conid << send_entity << create_time;
		ar << (short)cmd_oper;
		ar << (short)flag;
	}
	else
	{

		ar >> conid >> send_entity >> create_time;
		short t;
		ar >> t;
		cmd_oper = (CMD_OPER)t;

		ar >> t;
		flag = (FLAG)t;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgDCmdCurrentNoRequest::countObjectLength()
{
	return sizeof(conid)+sizeof(send_entity)+sizeof(create_time)+sizeof(short)*2;
}

IMPLEMENT_CLBTH(MsgDCmdCurrentNoRequest2, MSGID_MsgDCmdCurrentNoRequest2)
IMPLEMENT_SERIALIZE_OPERATOR(MsgDCmdCurrentNoRequest2)
MsgDCmdCurrentNoRequest2::MsgDCmdCurrentNoRequest2()
{
	CLBTH_ZERO_INIT_OBJECT(BaseMsg)
}
MsgDCmdCurrentNoRequest2::~MsgDCmdCurrentNoRequest2()
{

}
HRCODE MsgDCmdCurrentNoRequest2::doSerialize(Archive& ar)
{
	char len;

	if(ar.isStoring())
	{
		ar << conid << send_entity << create_time;
		ar << (short)cmd_oper;
		ar << (short)flag;

		AR_WRITE_STRING(begin_cmdno, len);
		AR_WRITE_STRING(end_cmdno, len);
	}
	else
	{
		ar >> conid >> send_entity >> create_time;
		short t;
		ar >> t;
		cmd_oper = (CMD_OPER)t;

		ar >> t;
		flag = (FLAG)t;

		AR_READ_STRING(begin_cmdno, len);
		AR_READ_STRING(end_cmdno, len);
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgDCmdCurrentNoRequest2::countObjectLength()
{
	return sizeof(conid)+sizeof(send_entity)+sizeof(create_time)+sizeof(short)*2+sizeof(char)+strlen(begin_cmdno)+sizeof(char)+strlen(end_cmdno);
}

IMPLEMENT_CLBTH(MsgDCmdCurrentNo, MSGID_MsgDCmdCurrentNo)
IMPLEMENT_SERIALIZE_OPERATOR(MsgDCmdCurrentNo)
MsgDCmdCurrentNo::MsgDCmdCurrentNo()
{
	CLBTH_ZERO_INIT_OBJECT(BaseMsg)
}
MsgDCmdCurrentNo::~MsgDCmdCurrentNo()
{

}
HRCODE MsgDCmdCurrentNo::doSerialize(Archive& ar)
{
	if (HRCODE(HRCODE::HC_SUCCESS) != MsgDCmdCurrentNoRequest::doSerialize(ar))
		return HRCODE(HRCODE::HC_FAIL);

	char len;

	if(ar.isStoring())
	{
		AR_WRITE_STRING(cmdno, len);
		ar << send_time;
	}
	else
	{
		AR_READ_STRING(cmdno, len);
		ar >> send_time;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgDCmdCurrentNo::countObjectLength()
{
	return MsgDCmdCurrentNoRequest::countObjectLength()+sizeof(char)+strlen(cmdno)+sizeof(send_time);
}



IMPLEMENT_CLBTH(MsgDCmdRefresh, MSGID_MsgDCmdRefresh)
IMPLEMENT_SERIALIZE_OPERATOR(MsgDCmdRefresh)
MsgDCmdRefresh::MsgDCmdRefresh()
{
	CLBTH_ZERO_INIT_OBJECT(BaseMsg)
}
MsgDCmdRefresh::~MsgDCmdRefresh()
{

}
HRCODE MsgDCmdRefresh::doSerialize(Archive& ar)
{
//	char len;

	if(ar.isStoring())
	{
		ar << conid << ltime << send_entity << create_time << source;
	}
	else
	{
		ar >> conid >> ltime >> send_entity >> create_time >> source;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgDCmdRefresh::countObjectLength()
{
	return sizeof(conid)+sizeof(ltime)+sizeof(send_entity)+sizeof(create_time)+sizeof(source);
}



IMPLEMENT_CLBTH(MsgDCmdRequest, MSGID_MsgDCmdRequest)
IMPLEMENT_SERIALIZE_OPERATOR(MsgDCmdRequest)
MsgDCmdRequest::MsgDCmdRequest()
{
	CLBTH_ZERO_INIT_OBJECT(BaseMsg)
}
MsgDCmdRequest::~MsgDCmdRequest()
{
}
HRCODE MsgDCmdRequest::doSerialize(Archive& ar)
{
	char len;

	if(ar.isStoring())
	{
		AR_WRITE_STRING(cmdno, len);
	}
	else
	{
		AR_READ_STRING(cmdno, len);
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgDCmdRequest::countObjectLength()
{
	return sizeof(char)+strlen(cmdno);
}


IMPLEMENT_CLBTH(MsgDCmdAnswer, MSGID_MsgDCmdAnswer)
IMPLEMENT_SERIALIZE_OPERATOR(MsgDCmdAnswer)
MsgDCmdAnswer::MsgDCmdAnswer()
{
	CLBTH_ZERO_INIT_OBJECT(BaseMsg)
}
MsgDCmdAnswer::~MsgDCmdAnswer()
{
}
HRCODE MsgDCmdAnswer::doSerialize(Archive& ar)
{
	char len;

	if(ar.isStoring())
	{
		AR_WRITE_STRING(cmdno, len);
		ar << status;
	}
	else
	{
		AR_READ_STRING(cmdno, len);
		ar >> status;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgDCmdAnswer::countObjectLength()
{
	return sizeof(char)+strlen(cmdno)+sizeof(status);
}

IMPLEMENT_CLBTH(MsgGpcMessage, MSGID_MsgGpcMessage)
IMPLEMENT_SERIALIZE_OPERATOR(MsgGpcMessage)
MsgGpcMessage::MsgGpcMessage()
{
}
MsgGpcMessage::~MsgGpcMessage()
{

}
HRCODE MsgGpcMessage::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		if (pack_len >= sizeof(package)/sizeof(package[0]))
			return HRCODE(HRCODE::HC_FAIL);

		ar << label << source_id << dest_id << cmd_type << answer_type << tm_request 
			<< tm_reply << spare ;

		ar << pack_len;
		ar.write(package, pack_len);

	}
	else
	{
		ar >> label >> source_id >> dest_id >> cmd_type >> answer_type >> tm_request 
			>> tm_reply >> spare ;

		ar >> pack_len;
		if (pack_len >= sizeof(package)/sizeof(package[0]))
			return HRCODE(HRCODE::HC_FAIL);

		ar.read(package, pack_len);
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgGpcMessage::countObjectLength()
{
	return sizeof(label)+sizeof(source_id)+sizeof(dest_id) 
		+sizeof(cmd_type) + sizeof(answer_type) +sizeof(tm_request)+sizeof(tm_reply)
		+sizeof(spare)
		+sizeof(pack_len) + pack_len;
}


IMPLEMENT_CLBTH(MsgDirectOutput, MSGID_MsgDirectOutput)
IMPLEMENT_SERIALIZE_OPERATOR(MsgDirectOutput)
MsgDirectOutput::MsgDirectOutput()
{
	CLBTH_ZERO_INIT_OBJECT(BaseMsg);
	nOutputHandleCount=0;
	for(int i=0;i<16;i++)
	{
	  nOutputHandles[i]=0xFFFF;
	  nOutputHandleChecks[i]=0xFFFF;
	}
}
MsgDirectOutput::~MsgDirectOutput()
{}

size_t MsgDirectOutput::countObjectLength()
{
	return sizeof(nOutputHandleCount)+sizeof(nOutputHandles)+sizeof(nOutputHandleChecks);
}

bool MsgDirectOutput::AddOutput(IWORD handle,IWORD flag)
{
    if(nOutputHandleCount>=16)
		return false;
    nOutputHandles[nOutputHandleCount] = flag ? (handle|0x8000):handle;
	nOutputHandleChecks[nOutputHandleCount] = ~nOutputHandles[nOutputHandleCount];
	nOutputHandleCount++;
	return true;
}

int MsgDirectOutput::GetCount()
{
	int i;
	for(i=0;i<nOutputHandleCount;i++)
	{
		if(nOutputHandleChecks[i] != ~nOutputHandles[i])
			break;
	}
	if(i<nOutputHandleCount) // error!
		return 0; 
	return nOutputHandleCount;
}

bool MsgDirectOutput::GetOutput(int index,IWORD &handle,IWORD &flag)
{
	if(index<0 || index>=nOutputHandleCount)
		return false;
    if(nOutputHandleChecks[index] != ~nOutputHandles[index])
		return false;
	handle=nOutputHandles[index] & 0x7FFF;
	flag=(nOutputHandles[index]>>15)&0x1;
	return true;
}


HRCODE MsgDirectOutput::doSerialize(Archive& ar)
{
	int i;
	if(ar.isStoring())
	{
		ar<<nOutputHandleCount;
		for(i=0;i<16;i++)
			ar<<nOutputHandles[i];
		for(i=0;i<16;i++)
			ar<<nOutputHandleChecks[i];
	}
	else
	{
		ar>>nOutputHandleCount;
		for(i=0;i<16;i++)
			ar>>nOutputHandles[i];
		for(i=0;i<16;i++)
			ar>>nOutputHandleChecks[i];
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}

//---------------------------- RBC 消息----------------------------------------------------------------

// RBC列车信息
IMPLEMENT_CLBTH(MsgRBCTrainInfo, MSGID_MsgRBCTrainInfo)
IMPLEMENT_SERIALIZE_OPERATOR(MsgRBCTrainInfo)
MsgRBCTrainInfo::MsgRBCTrainInfo()
{
}
MsgRBCTrainInfo::~MsgRBCTrainInfo()
{
}
HRCODE MsgRBCTrainInfo::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		strRBCName[sizeof(strRBCName)-1] = '\0';
		IBYTE nLen = strlen(strRBCName);
		ar << nUniqueID << nRBCID << nLen;
		ar.write(strRBCName, nLen);
		ar.write(tnTrainNo, MAX_TRAIN_NO_LEN);
		ar << nTrainLen << nMaxSpeed << nTrainMode << nTrainLevel << nSpeed << nPositionStatus;
		ar << nEmergencyStop << nRequestAct << nDir << nTrainOffset;
		ar << sTrainPos.nID << sTrainPos.nSta;
		ar.write( sTrainPos.strName, DEV_NAME_LEN );
		ar << sEOA.nID << sEOA.nSta;
		ar.write( sEOA.strName, DEV_NAME_LEN );
	}
	else
	{
		IBYTE nLen = 0;
		ar >> nUniqueID >> nRBCID >> nLen;
		if ( nLen >= sizeof(strRBCName) )
			return HRCODE(HRCODE::HC_FAIL);
		ar.read(strRBCName, nLen);
		strRBCName[nLen] = '\0';
		ar.read(tnTrainNo, MAX_TRAIN_NO_LEN);
		tnTrainNo[MAX_TRAIN_NO_LEN-1] = '\0';
		ar >> nTrainLen >> nMaxSpeed >> nTrainMode >> nTrainLevel >> nSpeed >> nPositionStatus;
		ar >> nEmergencyStop >> nRequestAct >> nDir >> nTrainOffset;
 		ar >> sTrainPos.nID >> sTrainPos.nSta;
		ar.read( sTrainPos.strName, DEV_NAME_LEN );
		sTrainPos.strName[DEV_NAME_LEN-1] = '\0';
		ar >> sEOA.nID >> sEOA.nSta;
		ar.read( sEOA.strName, DEV_NAME_LEN );
		sEOA.strName[DEV_NAME_LEN-1] = '\0';
	}
	return HRCODE(HRCODE::HC_SUCCESS);

}
size_t MsgRBCTrainInfo::countObjectLength()
{
	return sizeof(nUniqueID) + sizeof(nRBCID) + sizeof(IBYTE) + strlen(strRBCName) + MAX_TRAIN_NO_LEN
		+ sizeof(nTrainLen) + sizeof(nMaxSpeed) + sizeof(nTrainMode) + sizeof(nTrainLevel) + sizeof(nSpeed) + sizeof(nPositionStatus)
		+ sizeof(nEmergencyStop) + sizeof(nRequestAct) + sizeof(nDir) + sizeof(nTrainOffset)
		+ (sizeof(sTrainPos.nID)+sizeof(sTrainPos.nSta)+DEV_NAME_LEN) * 2;
}

// 列车操作(检索与删除)
IMPLEMENT_CLBTH(MsgRBCTrainOperation, MSGID_MsgRBCTrainOperation)
IMPLEMENT_SERIALIZE_OPERATOR(MsgRBCTrainOperation)
MsgRBCTrainOperation::MsgRBCTrainOperation()
{
}
MsgRBCTrainOperation::~MsgRBCTrainOperation()
{
}
HRCODE MsgRBCTrainOperation::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << nUniqueID << nOper;
	}
	else
	{
		ar >> nUniqueID >> nOper;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}

// RBC报警信息
IMPLEMENT_CLBTH(MsgRBCAlarm, MSGID_MsgRBCAlarm)
IMPLEMENT_SERIALIZE_OPERATOR(MsgRBCAlarm)
MsgRBCAlarm::MsgRBCAlarm()
{
}
MsgRBCAlarm::~MsgRBCAlarm()
{
}
HRCODE MsgRBCAlarm::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << nAlarmGroup << nAlarmCode << nAlarmTime << nMilliSeconds << nPriority << nAlarmState;
		strSite[sizeof(strSite)-1] = '\0';
		strObject[sizeof(strObject)-1] = '\0';
		strContent[sizeof(strContent)-1] = '\0';
		IWORD nLen = strlen(strSite);
		ar << nLen;
		ar.write(strSite, nLen);
		nLen = strlen(strObject);
		ar << nLen;
		ar.write(strObject, nLen);
		nLen = strlen(strContent);
		ar << nLen;
		ar.write(strContent, nLen);
	}
	else
	{
		ar >> nAlarmGroup >> nAlarmCode >> nAlarmTime >> nMilliSeconds >> nPriority >> nAlarmState;
		IWORD nLen = 0;
		ar >> nLen;
		if ( nLen >= sizeof(strSite) )
			return HRCODE(HRCODE::HC_FAIL);
		ar.read(strSite, nLen);
		strSite[nLen] = '\0';
		ar >> nLen;
		if ( nLen >= sizeof(strObject) )
			return HRCODE(HRCODE::HC_FAIL);
		ar.read(strObject, nLen);
		strObject[nLen] = '\0';
		ar >> nLen;
		if ( nLen >= sizeof(strContent) )
			return HRCODE(HRCODE::HC_FAIL);
		ar.read(strContent, nLen);
		strContent[nLen] = '\0';
	}
	return HRCODE(HRCODE::HC_SUCCESS);

}
size_t MsgRBCAlarm::countObjectLength()
{
	return sizeof(nAlarmGroup) + sizeof(nAlarmCode) + sizeof(nAlarmTime) + sizeof(nMilliSeconds) + sizeof(nPriority) + sizeof(nAlarmState)
			+ 3*sizeof(IWORD) + strlen(strSite) + strlen(strObject) + strlen(strContent);
}

// RBC连接状态报告
IMPLEMENT_CLBTH(MsgRBCStatusReport, MSGID_MsgRBCStatusReport)
IMPLEMENT_SERIALIZE_OPERATOR(MsgRBCStatusReport)
MsgRBCStatusReport::MsgRBCStatusReport()
	: nCount(0) 
{
}
MsgRBCStatusReport::~MsgRBCStatusReport()
{
}
HRCODE MsgRBCStatusReport::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		if ( nCount > MAX_RBC_COUNT )
			return HRCODE(HRCODE::HC_FAIL);
		ar << nCount;
		for ( int i = 0; i < nCount; i++ )
			ar << szStatus[i].nRBCID << szStatus[i].nStatus;
	}
	else
	{
		ar >> nCount;
		if ( nCount > MAX_RBC_COUNT )
			return HRCODE(HRCODE::HC_FAIL);
		for ( int i = 0; i < nCount; i++ )
			ar >> szStatus[i].nRBCID >> szStatus[i].nStatus;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgRBCStatusReport::countObjectLength()
{
	return sizeof(nCount) + nCount*(sizeof(IWORD)+sizeof(WORK_STATUS));
}

//文本信息
IMPLEMENT_CLBTH(MsgRBCTxt, MSGID_MsgRBCTxt)
IMPLEMENT_SERIALIZE_OPERATOR(MsgRBCTxt)
MsgRBCTxt::MsgRBCTxt()
{
}
MsgRBCTxt::~MsgRBCTxt()
{
}
HRCODE MsgRBCTxt::doSerialize(Archive& ar)
{
	if (ar.isStoring())
	{
		ar << nOperID << nSender;
		ar.write(tnTrain, sizeof(tnTrain));
		ar << nNIDEngine;
		ar.write(strTxt, sizeof(strTxt));
		//YB.P add for text display @20110712
		ar << nConfirm << nDispTime;
	} 
	else
	{
		ar >> nOperID >> nSender;
		ar.read(tnTrain, sizeof(tnTrain));
		tnTrain[sizeof(tnTrain)-1] = 0;
		ar >> nNIDEngine;
		ar.read(strTxt, sizeof(strTxt));
		strTxt[sizeof(strTxt)-1] = 0;
		//YB.P add for text display @20110712
		ar >> nConfirm >> nDispTime;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}

// 紧急停车命令发送
IMPLEMENT_CLBTH(MsgRBCEmergencySend, MSGID_MsgRBCEmergencySend)
IMPLEMENT_SERIALIZE_OPERATOR(MsgRBCEmergencySend)
MsgRBCEmergencySend::MsgRBCEmergencySend()
{
}
MsgRBCEmergencySend::~MsgRBCEmergencySend()
{
}
HRCODE MsgRBCEmergencySend::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << nOperID << nSender;
		ar.write(tnTrain, sizeof(tnTrain));
		ar << nNIDEngine << tmSend << nFlag;
	}
	else
	{
		ar >> nOperID >> nSender;
		ar.read(tnTrain, sizeof(tnTrain));
		tnTrain[sizeof(tnTrain)-1] = '\0';
		ar >> nNIDEngine >> tmSend >> nFlag;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}

// 紧急停车命令发送回执
IMPLEMENT_CLBTH(MsgRBCEmergencyResult, MSGID_MsgRBCEmergencyResult)
IMPLEMENT_SERIALIZE_OPERATOR(MsgRBCEmergencyResult)
MsgRBCEmergencyResult::MsgRBCEmergencyResult()
{
}
MsgRBCEmergencyResult::~MsgRBCEmergencyResult()
{
}
HRCODE MsgRBCEmergencyResult::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		strResult[sizeof(strResult)-1] = '\0';
		short nRsLen = strlen(strResult);
		ar << nOperID << nSender;
		ar.write(tnTrain, sizeof(tnTrain));
		ar << nNIDEngine << tmSend << nFlag << nStatus << nRsLen;
		ar.write(strResult, nRsLen);
	}
	else
	{
		ar >> nOperID >> nSender;
		ar.read(tnTrain, sizeof(tnTrain));
		tnTrain[sizeof(tnTrain)-1] = '\0';
		short nRsLen = 0;
		ar >> nNIDEngine >> tmSend >> nFlag >> nStatus >> nRsLen;
		if ( nRsLen < 0 || nRsLen >= sizeof(strResult) )
			return HRCODE(HRCODE::HC_FAIL);
		ar.read(strResult, nRsLen);
		strResult[nRsLen] = '\0';
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgRBCEmergencyResult::countObjectLength()
{
	return sizeof(nOperID) + sizeof(nSender) + sizeof(tnTrain) + sizeof(nNIDEngine) + sizeof(tmSend)
		+ sizeof(nFlag) + sizeof(nStatus) + sizeof(short) + strlen(strResult);
}

///////////////
IMPLEMENT_CLBTH(MsgDataStyleReq2, MSGID_MsgDataStyleReq2)
IMPLEMENT_SERIALIZE_OPERATOR(MsgDataStyleReq2)
MsgDataStyleReq2::MsgDataStyleReq2()
{
	CLBTH_ZERO_INIT_OBJECT(BaseMsg);
}
MsgDataStyleReq2::~MsgDataStyleReq2()
{

}
HRCODE MsgDataStyleReq2::doSerialize(Archive& ar)
{
	int i = 0;
	if(ar.isStoring())
	{
		ar << nStatCount;
		for(i = 0;i<nStatCount;i++)
			ar<<nStno[i]<<nStyle[i]<<nStatus[i];
		ar << nStartTime << nEndTime;
	}
	else
	{
		ar >> nStatCount;
		if(nStatCount>LOGIN_MAX_STATION_COUNT)
			return HRCODE(HRCODE::HC_FAIL);
		for(i = 0;i<nStatCount;i++)
		{
			ar >> nStno[i];
			int t;
			ar >> t;  nStyle[i]  = (MsgDataStyleReq2::DataStyle)t;
			ar >> t;  nStatus[i] = (MsgDataStyleReq2::ReqStatus)t;
		}
		ar >> nStartTime >> nEndTime;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgDataStyleReq2::countObjectLength()
{
	return sizeof(nStatCount)+(sizeof(nStyle[0])+sizeof(nStno[0])+sizeof(nStatus[0]))*nStatCount+sizeof(nStartTime)+sizeof(nEndTime);
}

///////////////
IMPLEMENT_CLBTH(MsgParamentRequest, MSGID_MsgParamentRequest)
IMPLEMENT_SERIALIZE_OPERATOR(MsgParamentRequest)
MsgParamentRequest::MsgParamentRequest()
{
	CLBTH_ZERO_INIT_OBJECT(BaseMsg);
}
MsgParamentRequest::~MsgParamentRequest()
{
}
HRCODE MsgParamentRequest::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << entity << scope << type_count;
		for (int i=0; i<type_count; i++)
			ar << type[i];
	}
	else
	{
		ar >> entity >> scope >> type_count;
		if ( type_count > sizeof(type)/sizeof(type[0]) )
			return HRCODE(HRCODE::HC_FAIL);

		for (int i=0; i<type_count; i++)
			ar >> type[i];
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgParamentRequest::countObjectLength()
{
	return sizeof(scope)+sizeof(entity)+sizeof(type_count)+type_count*sizeof(type[0]);
}

IMPLEMENT_CLBTH(MsgParamentReply, MSGID_MsgParamentReply)
IMPLEMENT_SERIALIZE_OPERATOR(MsgParamentReply)
MsgParamentReply::MsgParamentReply()
{
	memset(&int_parament, 0x00, sizeof(int_parament));
	memset(&str_parament, 0x00, sizeof(str_parament));
}
MsgParamentReply::~MsgParamentReply()
{

}
HRCODE MsgParamentReply::doSerialize(Archive& ar)
{
	if (HRCODE(HRCODE::HC_SUCCESS) != MsgParamentRequest::doSerialize(ar))
		return HRCODE(HRCODE::HC_FAIL);

	short len;
	if(ar.isStoring())
	{
		for (int i=0; i<type_count; i++)
		{
			ar << int_parament[i];
			AR_WRITE_STRING(str_parament[i], len);
		}
	}
	else
	{
		for (int i=0; i<type_count; i++)
		{
			ar >> int_parament[i];
			AR_READ_STRING(str_parament[i], len);
		}
	}

	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgParamentReply::countObjectLength()
{
	size_t t = MsgParamentRequest::countObjectLength();
	for (int i=0; i<type_count; i++)
		t+= (sizeof(int_parament[i])+sizeof(short)+strlen(str_parament[i]));
	return t;
}

IMPLEMENT_CLBTH(MsgParamentOperation, MSGID_MsgParamentOperation)
IMPLEMENT_SERIALIZE_OPERATOR(MsgParamentOperation)
MsgParamentOperation::MsgParamentOperation()
{
	oper = (Operation)-1;
}
MsgParamentOperation::~MsgParamentOperation()
{
}
HRCODE MsgParamentOperation::doSerialize(Archive& ar)
{
	if (HRCODE(HRCODE::HC_SUCCESS) != MsgParamentReply::doSerialize(ar))
		return HRCODE(HRCODE::HC_FAIL);

	if(ar.isStoring())
	{
		ar << oper;
	}
	else
	{
		int t;
		ar >> t;
		oper = (Operation)t;
	}

	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgParamentOperation::countObjectLength()
{
	size_t t = MsgParamentReply::countObjectLength()+sizeof(oper);
	return t;
}


IMPLEMENT_CLBTH(MsgSetDDyName, MSGID_MsgSetDDyName)
IMPLEMENT_SERIALIZE_OPERATOR(MsgSetDDyName)
MsgSetDDyName::MsgSetDDyName()
{
	CLBTH_ZERO_INIT_OBJECT(BaseMsg);
}
MsgSetDDyName::~MsgSetDDyName()
{

}
HRCODE MsgSetDDyName::doSerialize(Archive& ar)
{
	char len;
	if(ar.isStoring())
	{
		AR_WRITE_STRING(user_name, len);
		ar << conid << shift_id;
	}
	else
	{
		AR_READ_STRING(user_name, len);
		ar >> conid >> shift_id;
	}

	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgSetDDyName::countObjectLength()
{
	return sizeof(char)+strlen(user_name)+sizeof(CONID)+sizeof(shift_id);
}

IMPLEMENT_CLBTH(MsgGetDDyName, MSGID_MsgGetDDyName)
IMPLEMENT_SERIALIZE_OPERATOR(MsgGetDDyName)
MsgGetDDyName::MsgGetDDyName()
{
	CLBTH_ZERO_INIT_OBJECT(BaseMsg);
}
MsgGetDDyName::~MsgGetDDyName()
{

}
HRCODE MsgGetDDyName::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << conid << shift_id;
	}
	else
	{
		ar >> conid >> shift_id;
	}

	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgGetDDyName::countObjectLength()
{
	return sizeof(CONID)+sizeof(shift_id);
}

IMPLEMENT_CLBTH(MsgGiveDDyName, MSGID_MsgGiveDDyName)
IMPLEMENT_SERIALIZE_OPERATOR(MsgGiveDDyName)
MsgGiveDDyName::MsgGiveDDyName()
{
	CLBTH_ZERO_INIT_OBJECT(BaseMsg);
}
MsgGiveDDyName::~MsgGiveDDyName()
{

}
HRCODE MsgGiveDDyName::doSerialize(Archive& ar)
{
	char len;
	if(ar.isStoring())
	{
		AR_WRITE_STRING(user_name1, len);
		AR_WRITE_STRING(user_name2, len);
		ar << conid << shift_id;
	}
	else
	{
		AR_READ_STRING(user_name1, len);
		AR_READ_STRING(user_name2, len);

		ar >> conid >> shift_id;
	}

	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgGiveDDyName::countObjectLength()
{
	return sizeof(CONID)+sizeof(shift_id)+sizeof(char*)*2+strlen(user_name1)+strlen(user_name2);
}

IMPLEMENT_CLBTH(MsgReportTimeTransfer, MSGID_MsgReportTimeTransfer)
IMPLEMENT_SERIALIZE_OPERATOR(MsgReportTimeTransfer)
MsgReportTimeTransfer::MsgReportTimeTransfer()
{
	CLBTH_ZERO_INIT_OBJECT(BaseMsg)
}
MsgReportTimeTransfer::MsgReportTimeTransfer(const MsgReportTime& msg)
{
	con_id = msg.con_id;
	memcpy(train_id, msg.train_id, sizeof(train_id));
	train_index = msg.train_index;
	rec_index   = msg.rec_index;
	train_key   = msg.train_key;
	oper        = msg.oper;
	run_attrib  = msg.run_attrib;	
	train_attrib = msg.train_attrib;			
	station      = msg.station;
	from_st      = msg.from_st;
	to_st        = msg.to_st;
	entry        = msg.entry;
	exit         = msg.exit;
	side         = msg.side;
	ltime        = msg.ltime;
	delay_time   = msg.delay_time;

	transfer_sta = INVALID_STATION_NO;
}
MsgReportTimeTransfer::~MsgReportTimeTransfer()
{
}
HRCODE MsgReportTimeTransfer::doSerialize(Archive& ar)
{
	if (HRCODE(HRCODE::HC_SUCCESS) != MsgReportTime::doSerialize(ar))
		return HRCODE(HRCODE::HC_FAIL);

	if(ar.isStoring())
	{
		ar << transfer_sta;
	}
	else
	{
		ar >> transfer_sta;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgReportTimeTransfer::countObjectLength()
{
	return MsgReportTime::countObjectLength()+sizeof(transfer_sta);
}

IMPLEMENT_CLBTH(MsgStringMessage, MSGID_MsgStringMessage)
IMPLEMENT_SERIALIZE_OPERATOR(MsgStringMessage)
MsgStringMessage::MsgStringMessage()
{
	memset(token,0,sizeof(token));
	memset(content,0,sizeof(content));
	pcontent = content;
	len = 0;
}
MsgStringMessage::~MsgStringMessage()
{
	if((pcontent != NULL) && (pcontent != content))
	{
		delete []pcontent;
	}
}
void MsgStringMessage::expand(IDWORD new_len)
{
	if((pcontent != NULL) && (pcontent != content))
	{
		delete []pcontent;
	}		
	if(new_len < sizeof(content))
	{
		pcontent = content;
	}
	else
	{
		pcontent = new char[new_len+1];
	}
}
HRCODE MsgStringMessage::doSerialize(Archive& ar)
{
	IDWORD token_len;

	if(ar.isStoring())
	{
		AR_WRITE_STRING(token,token_len);

		ar << len;
		ar.write(pcontent, len);

	}
	else
	{
		AR_READ_STRING(token,token_len);

		ar>>len;

		expand(len);
		if(NULL == pcontent)//分配失败
		{
			return HRCODE(HRCODE::HC_FAIL);
		}
		ar.read(pcontent,len);
		pcontent[len] = 0;
	}

	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgStringMessage::countObjectLength()
{
	if(NULL == pcontent)
	{
		return strlen(token)+sizeof(IDWORD)+len+sizeof(IDWORD);
	}
	else
	{
		return strlen(token)+sizeof(IDWORD)+len+sizeof(IDWORD);
	}
}

I_BOOL MsgStringMessage::SetTokenContent(const char *sToken,const char* sText, int nTextLength)
{
	IDWORD token_len = 0;
	if (NULL != sToken)
	{
		token_len = strlen(sToken);
		if(token_len >= sizeof(token))
		{
			return IFALSE;
		}
		memcpy(token,sToken,token_len);
		token[token_len] = '\0';
	}

	if (NULL != sText)
	{
		if (nTextLength == -1)
			len = strlen(sText);
		else
			len = nTextLength;

		if (len > sizeof(MSGHEAD::msgid_t)-token_len - 12)
			return IFALSE;

		expand(len);
		if(NULL == pcontent)
		{
			return IFALSE;
		}
		memcpy(pcontent,sText,len);
		pcontent[len] = '\0';
	}
	return ITRUE;
}
const char* MsgStringMessage::GetContent() const
{
	return pcontent;
}
IDWORD MsgStringMessage::GetContentLength() const
{
	return len;
}
const char* MsgStringMessage::GetToken() const
{
	return token;
}

IMPLEMENT_CLBTH(MsgStringMessage2, MSGID_MsgStringMessage2)
IMPLEMENT_SERIALIZE_OPERATOR(MsgStringMessage2)
MsgStringMessage2::MsgStringMessage2()
{
	memset(sysid,0,sizeof(sysid));
	memset(token,0,sizeof(token));
	memset(content,0,sizeof(content));
	package_index = 0;
	package_count = 0;
	pcontent = NULL;
	currentlength = 0;
}
MsgStringMessage2::~MsgStringMessage2()
{
	if((pcontent != NULL) && (pcontent != content))
	{
		delete pcontent;
	}
}
HRCODE MsgStringMessage2::doSerialize(Archive& ar)
{
	IDWORD len;
	if(ar.isStoring())
	{
		AR_WRITE_STRING(sysid,len);
		AR_WRITE_STRING(token,len);
		AR_WRITE_STRING(content,len);
		ar<<package_index<<package_count<<totallength;
	}
	else
	{
		AR_READ_STRING(sysid,len);
		AR_READ_STRING(token,len);
		AR_READ_STRING(content,len);
		ar>>package_index>>package_count>>totallength;
	}

	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgStringMessage2::countObjectLength()
{
	return strlen(sysid) + sizeof(IDWORD)+strlen(token)+sizeof(IDWORD)+strlen(content)+sizeof(IDWORD)
		+sizeof(package_index)+sizeof(package_count)+sizeof(totallength);
}

I_BOOL MsgStringMessage2::IsSamePackage(const MsgStringMessage2* msgSM2) const
{
	if(strlen(sysid) == 0)
	{
		return IFALSE;
	}
	if(strcmp(msgSM2->sysid,sysid) == 0)
	{
		return ITRUE;
	}
	return IFALSE;
}

I_TIME MsgStringMessage2::GetPackageTime() const
{
	return packagetime;
}

const char* MsgStringMessage2::GetSysid() const
{
	return sysid;
}

IDWORD MsgStringMessage2::GetPackageIndex() const
{
	return package_index;
}

IDWORD MsgStringMessage2::GetPackageCount() const
{
	return package_count;
}

IDWORD MsgStringMessage2::GetTotalLength() const
{
	return totallength;
}

ISDWORD MsgStringMessage2::SelfPackage()
{
	if(strlen(sysid) == 0)
	{
		return -1;//无数据
	}
	if(package_index != 0)
	{
		return 2;//不是第一帧
	}
	if((pcontent != NULL) && (pcontent != content))
	{
		delete pcontent;
	}
	currentlength = 0;
	if(totallength >= sizeof(content))
	{
		pcontent = new char[totallength+1];
		if(NULL == pcontent)
		{
			return -101;//申请内存失败
		}
		memset(pcontent,0,totallength+1);		
		if(strlen(content) + currentlength <= totallength)
		{
			strcpy(pcontent,content);
		}
		else
		{
			return -100;//数据溢出
		}
	}
	else
	{
		pcontent = content; 
	}
	currentlength = strlen(pcontent);		
	packagetime = time(0);
	package_index++;

	if(package_count == package_index)
	{
		return 0;
	}
	return 1;
}

ISDWORD MsgStringMessage2::Package(const MsgStringMessage2* msgSM2)
{
	if(IsSamePackage(msgSM2))//同一包数据
	{
		if(package_index == msgSM2->package_index)//当前包序相同
		{
			if(strlen(msgSM2->content) + currentlength <= totallength)
			{
				strcat(pcontent,msgSM2->content);
				currentlength = strlen(pcontent);
				package_index++;
			}
			else
			{
				return -100;//数据溢出
			}
		}
		else if(package_index > msgSM2->package_index)
		{
			return -2;//重复帧丢弃
		}
		else if(package_index < msgSM2->package_index)
		{
			return -3;//丢失数据包
		}
	}
	else
	{	
		return -4;//非相同数据
	}
	if(package_count == package_index)
	{
		return 0;
	}
	return 1;
}

I_BOOL MsgStringMessage2::SetTokenContent(const char *sToken,const char* sText)
{
	IDWORD len = strlen(sToken);
	if(len >= sizeof(token))
	{
		return IFALSE;
	}
	memcpy(token,sToken,len);
	token[len] = '\0';

	len = strlen(sText);
	if(len >= sizeof(content))
	{
		return IFALSE;
	}
	memcpy(content,sText,len);
	content[len] = '\0';
	return ITRUE;
}
I_BOOL MsgStringMessage2::SetPackageInfo(const char *sSysid,IDWORD nCount,IDWORD nIndex,IDWORD nLength)
{
	IDWORD len = strlen(sSysid);
	if(len >= sizeof(sysid))
	{
		return IFALSE;
	}
	memcpy(sysid,sSysid,len);
	sysid[len] = '\0';

	package_index = nIndex;
	package_count = nCount;
	totallength = nLength;
	return ITRUE;
}

const char* MsgStringMessage2::GetContent() const
{
	if(NULL == pcontent)
	{
		return content;
	}
	return pcontent;
}
const char* MsgStringMessage2::GetToken() const
{
	return token;
}

///////////////
IMPLEMENT_CLBTH(MsgDataStyleReq3, MSGID_MsgDataStyleReq3)
IMPLEMENT_SERIALIZE_OPERATOR(MsgDataStyleReq3)
MsgDataStyleReq3::MsgDataStyleReq3()
{

}
MsgDataStyleReq3::~MsgDataStyleReq3()
{

}
HRCODE MsgDataStyleReq3::doSerialize(Archive& ar)
{
	if (HRCODE(HRCODE::HC_SUCCESS) != MsgDataStyleReq2::doSerialize(ar))
		return HRCODE(HRCODE::HC_FAIL);

	if(ar.isStoring())
	{
		for (int i=0; i<nStatCount; i++) ar << nConId[i];
	}
	else
	{
		for (int i=0; i<nStatCount; i++) ar >> nConId[i];
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgDataStyleReq3::countObjectLength()
{
	return MsgDataStyleReq2::countObjectLength()+sizeof(nConId[0])*nStatCount;
}



///////////////
IMPLEMENT_CLBTH(MsgDataStyleReq4, MSGID_MsgDataStyleReq4)
IMPLEMENT_SERIALIZE_OPERATOR(MsgDataStyleReq4)
MsgDataStyleReq4::MsgDataStyleReq4()
{

}
MsgDataStyleReq4::~MsgDataStyleReq4()
{

}
HRCODE MsgDataStyleReq4::doSerialize(Archive& ar)
{
	if (HRCODE(HRCODE::HC_SUCCESS) != MsgDataStyleReq2::doSerialize(ar))
		return HRCODE(HRCODE::HC_FAIL);

	if(ar.isStoring())
	{
		for (int i=0; i<nStatCount; i++) ar << nScheduleId[i];
	}
	else
	{
		for (int i=0; i<nStatCount; i++) ar >> nScheduleId[i];
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgDataStyleReq4::countObjectLength()
{
	return MsgDataStyleReq2::countObjectLength()+sizeof(nScheduleId[0])*nStatCount;
}

///////////////
IMPLEMENT_CLBTH(MsgTrainDepartPredict, MSGID_MsgTrainDepartPredict)
IMPLEMENT_SERIALIZE_OPERATOR(MsgTrainDepartPredict)
MsgTrainDepartPredict::MsgTrainDepartPredict()
{
	CLBTH_ZERO_INIT_OBJECT(BaseMsg);
}
MsgTrainDepartPredict::~MsgTrainDepartPredict()
{

}
HRCODE MsgTrainDepartPredict::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << nSourceStno << nDestStno << nDestEntry << nTrainIndex << nRecIndex;
		ar.write(strTid, sizeof(strTid));
		ar << nSrcEntity << nSrcType << nOperType << nOperTime;
	}
	else
	{
		ar >> nSourceStno >> nDestStno >> nDestEntry >> nTrainIndex >> nRecIndex;
		ar.read(strTid, sizeof(strTid));
		ar >> nSrcEntity >> nSrcType >> nOperType >> nOperTime;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgTrainDepartPredict::countObjectLength()
{
	return sizeof(nSourceStno)+sizeof(nDestStno)+sizeof(nDestEntry)+sizeof(nTrainIndex)+sizeof(nRecIndex)+sizeof(strTid)
		+sizeof(nSrcEntity)+sizeof(nSrcType)+sizeof(nOperType)+sizeof(nOperTime);
}

//////////////////
IMPLEMENT_CLBTH(MsgCISFIRAlarm, MSGID_MsgCISFIRAlarm)
IMPLEMENT_SERIALIZE_OPERATOR(MsgCISFIRAlarm)
MsgCISFIRAlarm::MsgCISFIRAlarm()
	: nStno(INVALID_STATION_NO), nFIRLen(0)
{
	memset(nzFIRInfo, 0, sizeof(nzFIRInfo));
}
MsgCISFIRAlarm::~MsgCISFIRAlarm()
{
}
HRCODE MsgCISFIRAlarm::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << nStno << nFIRLen;
		if ( nFIRLen > MAX_FIR_LEN )
			return HRCODE(HRCODE::HC_FAIL);
		ar.write(nzFIRInfo, nFIRLen);
	}
	else
	{
		ar >> nStno >> nFIRLen;
		if ( nFIRLen > MAX_FIR_LEN )
			return HRCODE(HRCODE::HC_FAIL);
		ar.read(nzFIRInfo, nFIRLen);
	}
	return HRCODE(HRCODE::HC_SUCCESS);

}
size_t MsgCISFIRAlarm::countObjectLength()
{
	return sizeof(nStno) + sizeof(nFIRLen) + nFIRLen;
}

////////////轨道电路信息//////////////
IMPLEMENT_CLBTH(MsgTrackCircuit, MSGID_MsgTrackCircuit)
IMPLEMENT_SERIALIZE_OPERATOR(MsgTrackCircuit)
MsgTrackCircuit::MsgTrackCircuit()
: nTrackcircuitLen(0)
{
	memset(nzTrackCircuitInfo, 0, sizeof(nzTrackCircuitInfo));
}
MsgTrackCircuit::~MsgTrackCircuit()
{

}
HRCODE MsgTrackCircuit::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar <<nTrackcircuitLen;
		if ( nTrackcircuitLen > TRACKCIRCUIT_COUNT )
			return HRCODE(HRCODE::HC_FAIL);
		ar.write(nzTrackCircuitInfo, nTrackcircuitLen);
	}
	else
	{
		ar >>nTrackcircuitLen;
		if ( nTrackcircuitLen > TRACKCIRCUIT_COUNT )
			return HRCODE(HRCODE::HC_FAIL);
		ar.read(nzTrackCircuitInfo, nTrackcircuitLen);
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgTrackCircuit::countObjectLength()
{
	return sizeof(nTrackcircuitLen) + nTrackcircuitLen;
}

IMPLEMENT_CLBTH(MsgStPlanAutoTrigUpdate, MSGID_MsgStPlanAutoTrigUpdate)
IMPLEMENT_SERIALIZE_OPERATOR(MsgStPlanAutoTrigUpdate)
MsgStPlanAutoTrigUpdate::MsgStPlanAutoTrigUpdate()
	: train_index(0), rec_index(0), station(INVALID_STATION_NO)
{
}
MsgStPlanAutoTrigUpdate::~MsgStPlanAutoTrigUpdate()
{
}
HRCODE MsgStPlanAutoTrigUpdate::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << con_id << gpc << train_index << rec_index << station << run_type << trig_flag;
	}
	else
	{
		ar >> con_id >> gpc >> train_index >> rec_index >> station >> run_type >> trig_flag;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}

IMPLEMENT_CLBTH(MsgStPlanAutoTrigUpdateResult, MSGID_MsgStPlanAutoTrigUpdateResult)
IMPLEMENT_SERIALIZE_OPERATOR(MsgStPlanAutoTrigUpdateResult)
MsgStPlanAutoTrigUpdateResult::MsgStPlanAutoTrigUpdateResult()
	: train_index(0), rec_index(0), station(INVALID_STATION_NO)
{
}
MsgStPlanAutoTrigUpdateResult::~MsgStPlanAutoTrigUpdateResult()
{
}
HRCODE MsgStPlanAutoTrigUpdateResult::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << con_id << train_index << rec_index << station << run_type << trig_flag << plan_res << route_res;
	}
	else
	{
		ar >> con_id >> train_index >> rec_index >> station >> run_type >> trig_flag >> plan_res >> route_res;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}

// 调车状态变化
IMPLEMENT_CLBTH(MsgShuntStatusUpdate, MSGID_MsgShuntStatusUpdate)
IMPLEMENT_SERIALIZE_OPERATOR(MsgShuntStatusUpdate)
MsgShuntStatusUpdate::MsgShuntStatusUpdate()
	: nStationNo(INVALID_STATION_NO)
{
}
MsgShuntStatusUpdate::~MsgShuntStatusUpdate()
{
}
HRCODE MsgShuntStatusUpdate::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << nStationNo << nShuntIndex << nGouIndex << nStatus;
	}
	else
	{
		ar >> nStationNo >> nShuntIndex >> nGouIndex >> nStatus;
	}
	return HRCODE(HRCODE::HC_SUCCESS);

}

// 调车报点
IMPLEMENT_CLBTH(MsgShuntTimeReport, MSGID_MsgShuntTimeReport)
IMPLEMENT_SERIALIZE_OPERATOR(MsgShuntTimeReport)
MsgShuntTimeReport::MsgShuntTimeReport()
	: nStationNo(INVALID_STATION_NO)
{
}
MsgShuntTimeReport::~MsgShuntTimeReport()
{
}
HRCODE MsgShuntTimeReport::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << nStationNo << nShuntIndex << nGouIndex << nTimeType << nOperType << tmRptTime;
	}
	else
	{
		ar >> nStationNo >> nShuntIndex >> nGouIndex >> nTimeType >> nOperType >> tmRptTime;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}

// 调车请求
IMPLEMENT_CLBTH(MsgShuntRouteGroupRequest, MSGID_MsgShuntRouteGroupRequest)
IMPLEMENT_SERIALIZE_OPERATOR(MsgShuntRouteGroupRequest)
MsgShuntRouteGroupRequest::MsgShuntRouteGroupRequest()
	: nStationNo(INVALID_STATION_NO)
{
}
MsgShuntRouteGroupRequest::~MsgShuntRouteGroupRequest()
{
}
HRCODE MsgShuntRouteGroupRequest::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << nStationNo << nHandle << nGouIndex << tmReqTime;
	}
	else
	{
		ar >> nStationNo >> nHandle >> nGouIndex >> tmReqTime;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}


// 调机号操作，包括添加、删除、修改
IMPLEMENT_CLBTH(MsgShuntLocoNoOper, MSGID_MsgShuntLocoNoOper)
IMPLEMENT_SERIALIZE_OPERATOR(MsgShuntLocoNoOper)
MsgShuntLocoNoOper::MsgShuntLocoNoOper()
:	nStationNo(INVALID_STATION_NO), nDmisType(0), nDmisNo(INVALID_DMIS_ID), nOldLen(0), nNewLen(0)
{
	tnOldTrain[0] = '\0';
	tnNewTrain[0] = '\0';
	nIPIndex = 0;
	memset(reserve, 0, sizeof(reserve));

	pszLocoDesc = NULL;
	nLocoDescBufLen= 0;
}

MsgShuntLocoNoOper::~MsgShuntLocoNoOper()
{

}
HRCODE MsgShuntLocoNoOper::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << nStationNo << nDmisType << nDmisNo << nOldLen;
		if ( nOldLen >= MAX_TRAIN_NO_LEN )
			return HRCODE(HRCODE::HC_FAIL);
		ar.write(tnOldTrain, nOldLen);
		ar << nNewLen;
		if ( nNewLen >= MAX_TRAIN_NO_LEN )
			return HRCODE(HRCODE::HC_FAIL);
		ar.write(tnNewTrain, nNewLen);
		ar << nIPIndex;
		ar << nLocoType;
		ar.write(reserve, sizeof(reserve));

		unsigned short len = 0;
		if(pszLocoDesc)
		{
			len = strlen(pszLocoDesc);
			if(len >= nLocoDescBufLen)
				len = nLocoDescBufLen;
		}
		ar << len;
		if(len)
			ar.write(pszLocoDesc, len);
	}
	else
	{
		ar >> nStationNo >> nDmisType >> nDmisNo >> nOldLen;
		if ( nOldLen >= MAX_TRAIN_NO_LEN )
			return HRCODE(HRCODE::HC_FAIL);
		ar.read(tnOldTrain, nOldLen);
		ar >> nNewLen;
		if ( nNewLen >= MAX_TRAIN_NO_LEN )
			return HRCODE(HRCODE::HC_FAIL);
		ar.read(tnNewTrain, nNewLen);
		ar >> nIPIndex;
		tnOldTrain[nOldLen] = 0;
		tnNewTrain[nNewLen] = 0;
		ar >> nLocoType;
		ar.read(reserve, sizeof(reserve));

		unsigned short len = 0;
		ar >> len;
		if( NULL != SetLocoDesc(NULL, len) )
			ar.read(pszLocoDesc, len);
		else
			return HRCODE(HRCODE::HC_FAIL, HRCODE::E_NOENOUGH_MEMORY, HRCODE::SYS);
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgShuntLocoNoOper::countObjectLength()
{
	size_t obj_len = sizeof(nStationNo) + sizeof(nDmisType) + sizeof(nDmisNo) + sizeof(nOldLen) + sizeof(nNewLen) + nOldLen + nNewLen + sizeof(nIPIndex) + sizeof(reserve)
		+ sizeof(nLocoType) + sizeof(nLocoDescBufLen);
	if(pszLocoDesc)
	{
		obj_len += strlen(pszLocoDesc);
	}

	return obj_len;
}

char*	MsgShuntLocoNoOper::GetLocoDesc()
{
	return pszLocoDesc;
}
char*	MsgShuntLocoNoOper::SetLocoDesc(const char *psz, unsigned short nLen)
{
	if(nLen >= nLocoDescBufLen)
	{
		if(pszLocoDesc)
			delete pszLocoDesc;
		nLocoDescBufLen = nLen + 1;
		pszLocoDesc = new char[nLocoDescBufLen];
		if(pszLocoDesc)
			memset(pszLocoDesc, 0, nLocoDescBufLen);
	}

	if(psz)
	{
		strncpy(pszLocoDesc, psz, nLen);
		pszLocoDesc[nLen] = '\0';
	}
	else
	{
		pszLocoDesc[0] = '\0';
	}

	return pszLocoDesc;
}


//----------------------------权限申请-----------------------------------

IMPLEMENT_CLBTH(MsgRMRequest,MSGID_MsgRMRequest)
IMPLEMENT_SERIALIZE_OPERATOR(MsgRMRequest)
MsgRMRequest::MsgRMRequest()
	:nDestID(INVALID_ENTITY_ID),nSrcID(INVALID_ENTITY_ID)
{
	strPassword[0]='\0';
	strSrcName[0]='\0';
	strUserName[0]='\0';
}
MsgRMRequest::~MsgRMRequest()
{
}
HRCODE MsgRMRequest::doSerialize(Archive &ar)
{
	if(ar.isStoring())
	{
		ar<<nDestID<<nSrcID;
		ar.write(strSrcName,sizeof(strSrcName));
		ar.write(strUserName,sizeof(strUserName));
		ar.write(strPassword,sizeof(strPassword));
	}
	else
	{
		ar>>nDestID>>nSrcID;
		ar.read(strSrcName,sizeof(strSrcName));
		ar.read(strUserName,sizeof(strUserName));
		ar.read(strPassword,sizeof(strPassword));
		strSrcName[sizeof(strSrcName)-1] = '\0';
		strUserName[sizeof(strUserName)-1] = '\0';
		strPassword[sizeof(strPassword)-1] = '\0';
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgRMRequest::countObjectLength()
{
	return sizeof(nDestID)+sizeof(nSrcID)+sizeof(strSrcName)+sizeof(strUserName)+sizeof(strPassword);
}

/******************************权限申请结果********************************/
IMPLEMENT_CLBTH(MsgRMResult,MSGID_MsgRMResult)
IMPLEMENT_SERIALIZE_OPERATOR(MsgRMResult)
MsgRMResult::MsgRMResult()
	:nDestID(INVALID_ENTITY_ID),nSrcID(INVALID_ENTITY_ID)
{
}
MsgRMResult::~MsgRMResult()
{
}
HRCODE MsgRMResult::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar<<nDestID<<nSrcID<<nResult;
	}
	else
	{
		ar>>nDestID>>nSrcID>>nResult;
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgRMResult::countObjectLength()
{
	return sizeof(nDestID)+sizeof(nSrcID)+sizeof(nResult);
}


/********************************权限通知**********************************/
IMPLEMENT_CLBTH(MsgRMReport,MSGID_MsgRMReport)
IMPLEMENT_SERIALIZE_OPERATOR(MsgRMReport)
MsgRMReport::MsgRMReport()
	:nDestID(INVALID_ENTITY_ID),nSrcID(INVALID_ENTITY_ID)
{
	strSrcName[0]='\0';
	strUserName[0]='\0';
}
MsgRMReport::~MsgRMReport()
{
}
HRCODE MsgRMReport::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar<<nDestID<<nSrcID;
		ar.write(strSrcName,sizeof(strSrcName));
		ar.write(strUserName,sizeof(strUserName));
		ar<<tmRight<<nResult;
	}
	else
	{
		ar>>nDestID>>nSrcID;
		ar.read(strSrcName,sizeof(strSrcName));
		ar.read(strUserName,sizeof(strUserName));
		ar>>tmRight>>nResult;
		strSrcName[sizeof(strSrcName)-1] = '\0';
		strUserName[sizeof(strUserName)-1] = '\0';
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgRMReport::countObjectLength()
{
	return sizeof(nDestID)+sizeof(nSrcID)+sizeof(strSrcName)+sizeof(strUserName)
		+sizeof(tmRight)+sizeof(nResult);
}


/******************************权限管理注册********************************/
IMPLEMENT_CLBTH(MsgRMRegister,MSGID_MsgRMRegister)
IMPLEMENT_SERIALIZE_OPERATOR(MsgRMRegister)
MsgRMRegister::MsgRMRegister()
	:nDestCount(0),nControllerID(INVALID_ENTITY_ID)
{
	memset(nzDestIDs,0,sizeof(nzDestIDs));
}
MsgRMRegister::~MsgRMRegister()
{
}
HRCODE MsgRMRegister::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar<<nDestCount<<nControllerID;
		for(int i=0;i<nDestCount;i++)
		{
			ar<<nzDestIDs[i];
		}
	}
	else
	{
		ar>>nDestCount>>nControllerID;
		if(nDestCount>256)
			return HRCODE(HRCODE::HC_FAIL);
		for(int i=0;i<nDestCount;i++)
		{
			ar>>nzDestIDs[i];
		}
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgRMRegister::countObjectLength()
{
	return sizeof(nDestCount)+sizeof(nControllerID)+nDestCount*sizeof(ENTITY_ID);
}


/********************************包装消息**********************************/
IMPLEMENT_CLBTH(MsgRMOperPack,MSGID_MsgRMOperPack)
IMPLEMENT_SERIALIZE_OPERATOR(MsgRMOperPack)
MsgRMOperPack::MsgRMOperPack()
	:nMsgLen(0),nDestID(INVALID_ENTITY_ID)
{
	memset(nzMsgBuf,0,sizeof(nzMsgBuf));
}
MsgRMOperPack::~MsgRMOperPack()
{
}
HRCODE MsgRMOperPack::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar<<nMsgLen<<nDestID;
		ar.write(nzMsgBuf,nMsgLen);
	}
	else
	{
		ar>>nMsgLen>>nDestID;
		if(nMsgLen>4096)
			return HRCODE(HRCODE::HC_FAIL);
		ar.read(nzMsgBuf,nMsgLen);
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgRMOperPack::countObjectLength()
{
	return sizeof(nMsgLen) + nMsgLen + sizeof(nDestID);
}



/********************************实体更新**********************************/
IMPLEMENT_CLBTH(MsgDestEntityUpdate,MSGID_MsgDestEntityUpdate)
IMPLEMENT_SERIALIZE_OPERATOR(MsgDestEntityUpdate)
MsgDestEntityUpdate::MsgDestEntityUpdate()
	:nEntityAddCount(0),nEntityRemovedCount(0)
{
	memset(nzEntityAdd,0,sizeof(nzEntityAdd));
	memset(nzEntityRemoved,0,sizeof(nzEntityRemoved));
}
MsgDestEntityUpdate::~MsgDestEntityUpdate()
{
}
HRCODE MsgDestEntityUpdate::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar<<nEntityAddCount<<nEntityRemovedCount;
		int i;
		for(i=0;i<nEntityAddCount;i++)
			ar<<nzEntityAdd[i];
		for(i=0;i<nEntityRemovedCount;i++)
			ar<<nzEntityRemoved[i];
	}
	else
	{
		ar>>nEntityAddCount>>nEntityRemovedCount;
		if(nEntityAddCount>256||nEntityRemovedCount>256)
			return HRCODE(HRCODE::HC_FAIL);
		int i;
		for(i=0;i<nEntityAddCount;i++)
			ar>>nzEntityAdd[i];
		for(i=0;i<nEntityRemovedCount;i++)
			ar>>nzEntityRemoved[i];
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgDestEntityUpdate::countObjectLength()
{
	return sizeof(nEntityAddCount)+nEntityAddCount*sizeof(ENTITY_ID)
		+sizeof(nEntityRemovedCount)+nEntityRemovedCount*sizeof(ENTITY_ID);
}
///////////////
IMPLEMENT_CLBTH(MsgBasicScheduleSet, MSGID_MsgBasicScheduleSet)
IMPLEMENT_SERIALIZE_OPERATOR(MsgBasicScheduleSet)
MsgBasicScheduleSet::MsgBasicScheduleSet()
{
	CLBTH_ZERO_INIT_OBJECT(BaseMsg);
}
MsgBasicScheduleSet::~MsgBasicScheduleSet()
{

}
HRCODE MsgBasicScheduleSet::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << nConId << nScheduleId << nStartTime << nEndTime << nSetNameTime << nEstimateStartTime << nSetNameEntity;		
		ar.write(nScheduleName, sizeof(nScheduleName));
	}
	else
	{
		ar >> nConId >> nScheduleId >> nStartTime >> nEndTime >> nSetNameTime >> nEstimateStartTime >> nSetNameEntity;	
		ar.read(nScheduleName, sizeof(nScheduleName));		
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgBasicScheduleSet::countObjectLength()
{
	return sizeof(nConId)+sizeof(nScheduleId)+sizeof(nStartTime)+sizeof(nEndTime)+sizeof(nScheduleName)+sizeof(nSetNameTime)+sizeof(nEstimateStartTime)+sizeof(nSetNameEntity);
}

// ----------------------- IP公布状态消息 -----------------------------------------------
IMPLEMENT_CLBTH(IPMsgTrainMove, MSGID_IPMsgTrainMove)
IMPLEMENT_SERIALIZE_OPERATOR(IPMsgTrainMove)
IPMsgTrainMove::IPMsgTrainMove()
{
	CLBTH_ZERO_INIT_OBJECT(BaseMsg);
}
IPMsgTrainMove::~IPMsgTrainMove()
{	
}
HRCODE IPMsgTrainMove::doSerialize(Archive& ar)
{
	char len;
	if(ar.isStoring())
	{
		ar << nTime << nPrevIndex << nPrevSt << nPrevWin << nNextIndex << nNextSt << nNextWin << nOperLvl << nTrainLvl << nFlag;	
		AR_WRITE_STRING(szPrevTrain, len);
		AR_WRITE_STRING(szNextTrain, len);
	}
	else
	{
		ar >> nTime >> nPrevIndex >> nPrevSt >> nPrevWin >> nNextIndex >> nNextSt >> nNextWin >> nOperLvl >> nTrainLvl >> nFlag;	
		AR_READ_STRING(szPrevTrain, len);
		AR_READ_STRING(szNextTrain, len);
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t IPMsgTrainMove::countObjectLength()
{
	return sizeof(nTime)+sizeof(nPrevIndex)+sizeof(nPrevSt)+sizeof(nPrevWin)+sizeof(nNextIndex)+sizeof(nNextSt)+
		sizeof(nNextWin)+sizeof(nOperLvl)+sizeof(nTrainLvl)+sizeof(nFlag)+sizeof(char)*2+strlen(szPrevTrain)+strlen(szNextTrain);
}

IMPLEMENT_CLBTH(IPMsgRouteStatus, MSGID_IPMsgRouteStatus)
IMPLEMENT_SERIALIZE_OPERATOR(IPMsgRouteStatus)
IPMsgRouteStatus::IPMsgRouteStatus()
{
	CLBTH_ZERO_INIT_OBJECT(BaseMsg);
}
IPMsgRouteStatus::~IPMsgRouteStatus()
{	
}
HRCODE IPMsgRouteStatus::doSerialize(Archive& ar)
{
	char len;
	if(ar.isStoring())
	{
		ar << nTime << nStation << nTrainIndex << nRouteType << nEntry << nExit << nRouteFlag << nMinSwitch << nMinDypassSwitch << nRouteStatus;	
		AR_WRITE_STRING(szTrain, len);

		AR_WRITE_COUNT(nSideCount, nzSides);
		for (int i=0; i< nSideCount; i++) ar << nzSides[i];
	}
	else
	{
		ar >> nTime >> nStation >> nTrainIndex >> nRouteType >> nEntry >> nExit >> nRouteFlag >> nMinSwitch >> nMinDypassSwitch >> nRouteStatus;	
		AR_READ_STRING(szTrain, len);

		AR_READ_COUNT(nSideCount, nzSides);
		for (int i=0; i< nSideCount; i++) ar >> nzSides[i];
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t IPMsgRouteStatus::countObjectLength()
{
	return sizeof(nTime)+sizeof(nStation)+sizeof(nTrainIndex)+sizeof(nRouteType)+sizeof(nEntry)+sizeof(nExit)+sizeof(nRouteFlag)+sizeof(nMinSwitch)+sizeof(nMinDypassSwitch)+sizeof(nRouteStatus)
		+sizeof(nSideCount)+sizeof(SIDE_ID)*nSideCount
		+sizeof(char)+strlen(szTrain);
}

IMPLEMENT_CLBTH(IPMsgSideStatus, MSGID_IPMsgSideStatus)
IMPLEMENT_SERIALIZE_OPERATOR(IPMsgSideStatus)
IPMsgSideStatus::IPMsgSideStatus()
{
	CLBTH_ZERO_INIT_OBJECT(BaseMsg);
}
IPMsgSideStatus::~IPMsgSideStatus()
{	
}
HRCODE IPMsgSideStatus::doSerialize(Archive& ar)
{
	char len;
	if(ar.isStoring())
	{
		ar << nTime << nStation << nTrainIndex << nSide << nSideStatus;	
		AR_WRITE_STRING(szTrain, len);
	}
	else
	{
		ar >> nTime >> nStation >> nTrainIndex >> nSide >> nSideStatus;
		AR_READ_STRING(szTrain, len);
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t IPMsgSideStatus::countObjectLength()
{
	return sizeof(nTime)+sizeof(nStation)+sizeof(nTrainIndex)+sizeof(nSide)+sizeof(nSideStatus)+sizeof(char)+strlen(szTrain);
}

IMPLEMENT_CLBTH(IPMsgTrainStatus, MSGID_IPMsgTrainStatus)
IMPLEMENT_SERIALIZE_OPERATOR(IPMsgTrainStatus)
IPMsgTrainStatus::IPMsgTrainStatus()
{
	CLBTH_ZERO_INIT_OBJECT(BaseMsg);
}
IPMsgTrainStatus::~IPMsgTrainStatus()
{	
}
HRCODE IPMsgTrainStatus::doSerialize(Archive& ar)
{
	char len;
	if(ar.isStoring())
	{
		ar << nTime << nStation << nTrainIndex << nTrainStatus << nEntry << nSide;	
		AR_WRITE_STRING(szTrain, len);
	}
	else
	{
		ar >> nTime >> nStation >> nTrainIndex >> nTrainStatus >> nEntry >> nSide;
		AR_READ_STRING(szTrain, len);
	}
	return HRCODE(HRCODE::HC_SUCCESS);

}
size_t IPMsgTrainStatus::countObjectLength()
{
	return sizeof(nTime)+sizeof(nStation)+sizeof(nTrainIndex)+sizeof(nEntry)+sizeof(nSide)+sizeof(nTrainStatus)+sizeof(char)+strlen(szTrain);
}

// ----------------------- 综合维护消息 -----------------------------------------------

// 属性查询
IMPLEMENT_CLBTH(CmsMsgAttrRequest, MSGID_CmsMsgAttrRequest)
IMPLEMENT_SERIALIZE_OPERATOR(CmsMsgAttrRequest)
CmsMsgAttrRequest::CmsMsgAttrRequest()
{
}
CmsMsgAttrRequest::~CmsMsgAttrRequest()
{
}
HRCODE CmsMsgAttrRequest::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << nDetectID << nDetectDev << nDestID << nABFlag << nAttrType << nAttrGroupID << nAttrIndex << nReplyPort << nPeerID << nPeerABFlag;
		strAttrName[sizeof(strAttrName)-1] = '\0';
		IBYTE len = strlen(strAttrName);
		ar << len;
		if ( len > 0 )
			ar.write(strAttrName, len);
		strPara[sizeof(strPara)-1] = '\0';
		len = strlen(strPara);
		ar << len;
		if ( len > 0 )
			ar.write(strPara, len);
	}
	else
	{
		ar >> nDetectID >> nDetectDev >> nDestID >> nABFlag >> nAttrType >> nAttrGroupID >> nAttrIndex >> nReplyPort >> nPeerID >> nPeerABFlag;
		IBYTE len = 0;
		ar >> len;
		if (len >= sizeof(strAttrName))
			return HRCODE(HRCODE::HC_FAIL);
		if (len > 0)
			ar.read(strAttrName, len);
		strAttrName[len] = '\0';
		ar >> len;
		if (len >= sizeof(strPara))
			return HRCODE(HRCODE::HC_FAIL);
		if (len > 0)
			ar.read(strPara, len);
		strPara[len] = '\0';
	}
	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t CmsMsgAttrRequest::countObjectLength()
{
	return sizeof(nDetectID) + sizeof(nDetectDev) + sizeof(nDestID) + sizeof(nABFlag) + sizeof(nAttrType) + sizeof(nAttrGroupID) + sizeof(nAttrIndex)
		+ sizeof(nReplyPort) + sizeof(nPeerID) + sizeof(nPeerABFlag) + 2*sizeof(IBYTE) + strlen(strAttrName) + strlen(strPara);
}


//cmss向目标设备注册消息
IMPLEMENT_CLBTH(CmsMsgSvrRegister,MSGID_CmsMsgSvrRegister)
IMPLEMENT_SERIALIZE_OPERATOR(CmsMsgSvrRegister)
CmsMsgSvrRegister::CmsMsgSvrRegister()
{
}
CmsMsgSvrRegister::~CmsMsgSvrRegister()
{
}
HRCODE CmsMsgSvrRegister::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar<<nSvrID<<nDestID<<nABFlag<<nReplyPort;
	}
	else
	{
		ar>>nSvrID>>nDestID>>nABFlag>>nReplyPort;
	}

	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t CmsMsgSvrRegister::countObjectLength()
{
	return sizeof(nSvrID) + sizeof(nDestID) + sizeof(nABFlag) + sizeof(nReplyPort);
}

//单属性应答消息

IMPLEMENT_CLBTH(CmsMsgSingleAttrReply,MSGID_CmsMsgSingleAttrReply)
IMPLEMENT_SERIALIZE_OPERATOR(CmsMsgSingleAttrReply)
CmsMsgSingleAttrReply::CmsMsgSingleAttrReply()
{
}
CmsMsgSingleAttrReply::~CmsMsgSingleAttrReply()
{
}
HRCODE CmsMsgSingleAttrReply::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar<<nDetectID<<nDetectDev<<nSrcID<<nABFlag<<nAttrType<<nAttrGroupID<<nAttrIndex<<nAttrSingleID<<nValue<<nMaxValue<<nUserValueLen;
		if(nUserValueLen>0)
			ar.write(nzUserValue,nUserValueLen);

	}
	else
	{
		ar>>nDetectID>>nDetectDev>>nSrcID>>nABFlag>>nAttrType>>nAttrGroupID>>nAttrIndex>>nAttrSingleID>>nValue>>nMaxValue>>nUserValueLen;
		if (nUserValueLen > sizeof(nzUserValue))
			return HRCODE(HRCODE::HC_FAIL);
		if (nUserValueLen > 0)
			ar.read(nzUserValue, nUserValueLen);
	}

	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t CmsMsgSingleAttrReply::countObjectLength()
{
	return sizeof(nDetectID) + sizeof(nDetectDev) + sizeof(nSrcID) + sizeof(nABFlag)+sizeof(nAttrType)+sizeof(nAttrGroupID)+sizeof(nAttrIndex)+sizeof(nAttrSingleID)+sizeof(nValue)+sizeof(nMaxValue)+sizeof(nUserValueLen)+nUserValueLen;
}

//组属性应答

IMPLEMENT_CLBTH(CmsMsgGroupAttrReply,MSGID_CmsMsgGroupAttrReply)
IMPLEMENT_SERIALIZE_OPERATOR(CmsMsgGroupAttrReply)
CmsMsgGroupAttrReply::CmsMsgGroupAttrReply()
{
}
CmsMsgGroupAttrReply::~CmsMsgGroupAttrReply()
{
}
HRCODE CmsMsgGroupAttrReply::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar<<nDetectID<<nDetectDev<<nSrcID<<nABFlag<<nAttrType<<nAttrGroupID<<nValueCount<<nValueStart;
		int i = 0 ;
		for(;i<nValueCount;i++)
		{
			ar<<nValue[i];		
		}
	}
	else
	{
		ar>>nDetectID>>nDetectDev>>nSrcID>>nABFlag>>nAttrType>>nAttrGroupID>>nValueCount>>nValueStart;
		if ( nValueCount > sizeof(nValue)/sizeof(nValue[0]) )
			return HRCODE(HRCODE::HC_FAIL);
		int i = 0;
		for(;i<nValueCount;i++)
		{
			ar>>nValue[i];
		}
	}

	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t CmsMsgGroupAttrReply::countObjectLength()
{
	return sizeof(nDetectID) + sizeof(nDetectDev) + sizeof(nSrcID) + sizeof(nABFlag)+sizeof(nAttrType)+sizeof(nAttrGroupID)+sizeof(nValueCount)+sizeof(nValueStart)+sizeof(nValue[0])*nValueCount;
}

//容量组属性应答

IMPLEMENT_CLBTH(CmsMsgCapaAttrReply,MSGID_CmsMsgCapaAttrReply)
IMPLEMENT_SERIALIZE_OPERATOR(CmsMsgCapaAttrReply)
CmsMsgCapaAttrReply::CmsMsgCapaAttrReply()
{
}
CmsMsgCapaAttrReply::~CmsMsgCapaAttrReply()
{
}
HRCODE CmsMsgCapaAttrReply::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar<<nDetectID<<nDetectDev<<nSrcID<<nABFlag<<nAttrGroupID<<nValueCount<<nValueStart;
		int i = 0 ;
		for(;i<nValueCount;i++)
		{
			ar<<nValue[i]<<nMaxValue[i];		
		}
	}
	else
	{
		ar>>nDetectID>>nDetectDev>>nSrcID>>nABFlag>>nAttrGroupID>>nValueCount>>nValueStart;
		if ( nValueCount > sizeof(nValue)/sizeof(nValue[0]) )
			return HRCODE(HRCODE::HC_FAIL);
		int i = 0;
		for(;i<nValueCount;i++)
		{
			ar>>nValue[i]>>nMaxValue[i];
		}
	}

	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t CmsMsgCapaAttrReply::countObjectLength()
{
	return sizeof(nDetectID) + sizeof(nDetectDev) + sizeof(nSrcID) + sizeof(nABFlag)+sizeof(nAttrGroupID)+sizeof(nValueCount)+sizeof(nValueStart)+(sizeof(nValue[0])+sizeof(nMaxValue[0]))*nValueCount;
}

// 动态队列属性应答
IMPLEMENT_CLBTH(CmsMsgQueueAttrReply,MSGID_CmsMsgQueueAttrReply)
IMPLEMENT_SERIALIZE_OPERATOR(CmsMsgQueueAttrReply)
CmsMsgQueueAttrReply::CmsMsgQueueAttrReply()
{
}
CmsMsgQueueAttrReply::~CmsMsgQueueAttrReply()
{
}
HRCODE CmsMsgQueueAttrReply::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar<<nDetectID<<nDetectDev<<nSrcID<<nABFlag<<nAttrGroupID<<nAttrSingleID<<nQueueCount;

		int i = 0 ;
		for(;i<nQueueCount;i++)
		{
			ar<<szQueue[i].nPeerABFlag;
			ar<<szQueue[i].nPeerID;
			ar<<szQueue[i].nValue;
			ar<<szQueue[i].nMaxValue;
			ar<<szQueue[i].nIndex;
		}

	}
	else
	{
		ar>>nDetectID>>nDetectDev>>nSrcID>>nABFlag>>nAttrGroupID>>nAttrSingleID>>nQueueCount;

		if(nQueueCount<MAX_QUEUE_COUNT+1)
		{
			int i = 0 ;
			for(;i<nQueueCount;i++)
			{
				ar>>szQueue[i].nPeerABFlag;
				ar>>szQueue[i].nPeerID;
				ar>>szQueue[i].nValue;	
				ar>>szQueue[i].nMaxValue;
				ar>>szQueue[i].nIndex;
			}
		}
		else
			return HRCODE(HRCODE::HC_FAIL);
	}

	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t CmsMsgQueueAttrReply::countObjectLength()
{
	return sizeof(nDetectID) + sizeof(nDetectDev) + sizeof(nSrcID) + sizeof(nABFlag)+sizeof(nAttrGroupID)+sizeof(nAttrSingleID)+sizeof(nQueueCount)+sizeof(szQueue[0])*nQueueCount;
}


//报警消息
IMPLEMENT_CLBTH(CmsMsgAlarm,MSGID_CmsMsgAlarm)
IMPLEMENT_SERIALIZE_OPERATOR(CmsMsgAlarm)
CmsMsgAlarm::CmsMsgAlarm()
{
}
CmsMsgAlarm::~CmsMsgAlarm()
{
}
HRCODE CmsMsgAlarm::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar<<nSrcID<<nABFlag<<tmAlarm<<nLevel<<nType<<nAttrType<<nAttrGroupID<<nAttrIndex;

		strAlarm[sizeof(strAlarm)-1] = '\0';		
		IBYTE len = strlen(strAlarm);
		ar<<len;
		ar.write(strAlarm,len);

	}
	else
	{
		ar>>nSrcID>>nABFlag>>tmAlarm>>nLevel>>nType>>nAttrType>>nAttrGroupID>>nAttrIndex;

		IBYTE len  = 0 ;
		ar>>len ;
		if (len >= sizeof(strAlarm))
			return HRCODE(HRCODE::HC_FAIL);
		if(len>0)
			ar.read(strAlarm,len);
		strAlarm[len] = '\0';
	}

	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t CmsMsgAlarm::countObjectLength()
{
	return sizeof(nSrcID) + sizeof(nABFlag) + sizeof(tmAlarm) + sizeof(nLevel)+sizeof(nType)+sizeof(nAttrType)+sizeof(nAttrGroupID)+sizeof(nAttrIndex)+sizeof(IBYTE)+strlen(strAlarm);
}

// 客户端监控设备注册

IMPLEMENT_CLBTH(CmsMsgClientDevRegister,MSGID_CmsMsgClientDevRegister)
IMPLEMENT_SERIALIZE_OPERATOR(CmsMsgClientDevRegister)
CmsMsgClientDevRegister::CmsMsgClientDevRegister()
{
}
CmsMsgClientDevRegister::~CmsMsgClientDevRegister()
{
}
HRCODE CmsMsgClientDevRegister::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar<<nClientID<<nReplyPort<<nDevCount;		
		int i = 0 ;
		for(;i<nDevCount;i++)
		{
			ar<<szDevices[i].nABFlag<<szDevices[i].nDevID;
		}

	}
	else
	{
		ar>>nClientID>>nReplyPort>>nDevCount;
		if(nDevCount<MAX_DEV_COUNT+1)
		{
			int i = 0;
			for(;i<nDevCount;i++)
			{
				ar>>szDevices[i].nABFlag>>szDevices[i].nDevID ;
			}
		}
		else
			return HRCODE(HRCODE::HC_FAIL);
	}

	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t CmsMsgClientDevRegister::countObjectLength()
{
	return sizeof(nClientID) + sizeof(nReplyPort) + sizeof(nDevCount) + sizeof(szDevices[0])*nDevCount;
}

IMPLEMENT_CLBTH(MsgHILCTransmit, MSGID_MsgHILCTransmit)
IMPLEMENT_SERIALIZE_OPERATOR(MsgHILCTransmit)
MsgHILCTransmit::MsgHILCTransmit()
{
	memset(token,0,sizeof(token));
	memset(hilc_content,0,sizeof(hilc_content));
	hilc_len = 0;
}
MsgHILCTransmit::~MsgHILCTransmit()
{
}
HRCODE MsgHILCTransmit::doSerialize(Archive& ar)
{
	IDWORD token_len;

	if(ar.isStoring())
	{
		AR_WRITE_STRING(token,token_len);

		ar << hilc_len;
		ar.write(hilc_content, hilc_len);

	}
	else
	{
		AR_READ_STRING(token,token_len);

		ar>>hilc_len;
		if(hilc_len >= sizeof(hilc_content))
		{
			return HRCODE(HRCODE::HC_FAIL);
		}

		ar.read(hilc_content,hilc_len);
		hilc_content[hilc_len] = 0;
	}

	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgHILCTransmit::countObjectLength()
{
	return strlen(token)+sizeof(IDWORD)+hilc_len+sizeof(hilc_len);
}

IMPLEMENT_CLBTH(MsgChangeSideReq, MSGID_MsgChangeSideReq)
IMPLEMENT_SERIALIZE_OPERATOR(MsgChangeSideReq)
MsgChangeSideReq::MsgChangeSideReq()
{
	station=0;
	count=0;
	memset(train_index,0,sizeof(train_index));
	memset(rec_index,0,sizeof(rec_index));
	memset(arrive_side,0,sizeof(arrive_side));
	memset(depart_side,0,sizeof(depart_side));
}
MsgChangeSideReq::~MsgChangeSideReq()
{
}
HRCODE MsgChangeSideReq::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << station;
		ar << count;
		for(int i=0; i<count; i++)
		{
			ar << train_index[i];
			ar << rec_index[i];
			ar << arrive_side[i];
			ar << depart_side[i];
		}
	}
	else
	{
		ar>>station;
		ar>>count;
		for(int i=0; i<count; i++)
		{
			ar >> train_index[i];
			ar >> rec_index[i];
			ar >> arrive_side[i];
			ar >> depart_side[i];
		}
	}

	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgChangeSideReq::countObjectLength()
{
	return sizeof(STATION_NO)+sizeof(IWORD)+count*(sizeof(TRAIN_KEY)+sizeof(IBYTE)+sizeof(SIDE)+sizeof(SIDE));
}

IMPLEMENT_CLBTH(MsgChangeSideAck, MSGID_MsgChangeSideAck)
IMPLEMENT_SERIALIZE_OPERATOR(MsgChangeSideAck)
MsgChangeSideAck::MsgChangeSideAck()
{
	result=0;
}
MsgChangeSideAck::~MsgChangeSideAck()
{
}
HRCODE MsgChangeSideAck::doSerialize(Archive& ar)
{
	if(ar.isStoring())
	{
		ar << result;
	}
	else
	{
		ar>>result;
	}

	return HRCODE(HRCODE::HC_SUCCESS);
}
size_t MsgChangeSideAck::countObjectLength()
{
	return sizeof(IWORD);
}
///////////// END /////////////
CLBTH_NAMESPACE_END

