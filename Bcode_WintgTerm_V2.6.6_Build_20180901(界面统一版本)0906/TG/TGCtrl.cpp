// TGCtrl.cpp : CTGCtrl ActiveX 控件类的实现。

#include "stdafx.h"
#include "TG.h"
#include "TGCtrl.h"
#include "TGPropPage.h"
#include ".\tgctrl.h"
#include "IF.h"
#include <sys/timeb.h>
#include ".\TGFrame.h"
#include "SingleDocTemplateForActiveX.h"

#include ".\baseMsg\private_msg_def.h"

BOOL  CopyTrainFromMsg(CTG_TRAIN& train,const MsgTrainOCX& msg);
BOOL  CopyBlockFromMsg(CTg_BLOCK& block,const MsgSchdBlockOCX& msg);

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


IMPLEMENT_DYNCREATE(CTGCtrl, COleControl)

// 消息映射
BEGIN_MESSAGE_MAP(CTGCtrl, COleControl)
	ON_OLEVERB(AFX_IDS_VERB_PROPERTIES, OnProperties)
	ON_WM_SIZE()
END_MESSAGE_MAP()



// 调度映射
BEGIN_DISPATCH_MAP(CTGCtrl, COleControl)
	DISP_FUNCTION_ID(CTGCtrl, "TgStart", dispidTgStart, TgStart, VT_BOOL, VTS_BSTR)
	DISP_FUNCTION_ID(CTGCtrl, "SetTrainDisplayFlag", dispidSetTrainDisplayFlag, OnSetTrainDisplayFlag, VT_EMPTY, VTS_I2 VTS_I2)
	DISP_FUNCTION_ID(CTGCtrl, "BeginNewSingleTrain", dispidBeginNewSingleTrain, OnBeginNewSingleTrain, VT_EMPTY, VTS_BSTR VTS_BSTR VTS_UI1 VTS_UI1)	
	DISP_FUNCTION_ID(CTGCtrl, "SelectSection", dispidSelectSection, OnSelectSection, VT_I2, VTS_I2 VTS_I4)
	DISP_FUNCTION_ID(CTGCtrl, "DrawSpecialTrain", dispidDrawSpecialTrain, OnDrawSpecialTrain, VT_I2, VTS_I2)
	DISP_FUNCTION_ID(CTGCtrl, "EndNewSingleTrain", dispidEndNewSingleTrain, OnEndNewSingleTrain, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION_ID(CTGCtrl, "FindTrain", dispidFindTrain, OnFindTrain, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION_ID(CTGCtrl, "TrainsOperator", dispidTrainsOperator, OnTrainsOperator, VT_EMPTY, VTS_I2 VTS_I2)
	
	DISP_FUNCTION_ID(CTGCtrl, "NewMark", dispidNewMark, OnNewMark, VT_EMPTY, VTS_I2)
	DISP_FUNCTION_ID(CTGCtrl, "SetBackGroud", dispidSetBackGroud, OnSetBackGroud, VT_EMPTY, VTS_UI2)
	DISP_FUNCTION_ID(CTGCtrl, "SetTgType", dispidSetTgType, OnSetTgType, VT_EMPTY, VTS_UI1)
	DISP_FUNCTION_ID(CTGCtrl, "GetTrainDisplayFlag", dispidGetTrainDisplayFlag, GetTrainDisplayFlag, VT_I2, VTS_I2 VTS_I2)
	DISP_FUNCTION_ID(CTGCtrl, "ZoomIn", dispidZoomIn, OnZoomIn, VT_EMPTY, VTS_I2)
	DISP_FUNCTION_ID(CTGCtrl, "ZoomOut", dispidZoomOut, OnZoomOut, VT_EMPTY, VTS_I2)
	DISP_FUNCTION_ID(CTGCtrl, "SetStationConnectStatus", dispidSetStationConnectStatus, OnSetStationConnectStatus, VT_EMPTY, VTS_I2 VTS_UI1)
	DISP_FUNCTION_ID(CTGCtrl, "SetCtrlMode", dispidSetCtrlMode, OnSetCtrlMode, VT_EMPTY, VTS_I2 VTS_I2 VTS_I2)
	DISP_FUNCTION_ID(CTGCtrl, "SetBackgroudBeginHour", dispidSetBackgroudBeginHour, OnSetBackgroudBeginHour, VT_EMPTY, VTS_UI1)
	DISP_FUNCTION_ID(CTGCtrl, "SetBackgroudTimeLength", dispidSetBackgroudTimeLength, OnSetBackgroudTimeLength, VT_EMPTY, VTS_UI1)
	DISP_FUNCTION_ID(CTGCtrl, "GetZoomStatus", dispidGetZoomStatus, OnGetZoomStatus, VT_I2, VTS_NONE)
	DISP_FUNCTION_ID(CTGCtrl, "DeleteMutiTrain", dispidDeleteMutiTrain, OnDeleteMutiTrain, VT_EMPTY, VTS_NONE)
	
	DISP_FUNCTION_ID(CTGCtrl, "SetStationInfo", dispidSetStationInfo, OnSetStationInfo, VT_EMPTY, VTS_BSTR)
	DISP_FUNCTION_ID(CTGCtrl, "SetCurrentSectionId", dispidSetCurrentSectionId, OnSetCurrentSectionId, VT_EMPTY, VTS_I2)
	DISP_FUNCTION_ID(CTGCtrl, "SetShiftId", dispidSetShiftId, SetShiftId, VT_EMPTY, VTS_UI4 VTS_I2)
	
	DISP_FUNCTION_ID(CTGCtrl, "SaveToPicture", dispidSaveToPicture, OnSaveToPicture, VT_EMPTY, VTS_BSTR)
	DISP_FUNCTION_ID(CTGCtrl, "BeginLoadData", dispidBeginLoadData, OnBeginLoadData, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION_ID(CTGCtrl, "EndLoadData", dispidEndLoadData, OnEndLoadData, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION_ID(CTGCtrl, "DeleteMark", dispidDeleteMark, OnDeleteMark, VT_EMPTY, VTS_I4)
	DISP_FUNCTION_ID(CTGCtrl, "DeleteTrain", dispidDeleteTrain, OnDeleteTrain, VT_EMPTY, VTS_I4 VTS_I4)

	DISP_FUNCTION_ID(CTGCtrl, "TurnReality", dispidTurnReality, OnTurnReality, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION_ID(CTGCtrl, "ReceivePoint", dispidReceivePoint, OnReceivePoint, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION_ID(CTGCtrl, "SetAppCtrlWord", dispidSetAppCtrlWord, SetAppCtrlWord, VT_EMPTY, VTS_UI4)
	DISP_FUNCTION_ID(CTGCtrl, "SetLineCtrlWord", dispidSetLineCtrlWord, SetLineCtrlWord, VT_EMPTY, VTS_UI4)
	DISP_FUNCTION_ID(CTGCtrl, "SetMarkCtrlWord", dispidSetMarkCtrlWord, SetMarkCtrlWord, VT_EMPTY, VTS_UI4)
	DISP_FUNCTION_ID(CTGCtrl, "GetAppCtrlWord", dispidGetAppCtrlWord, GetAppCtrlWord, VT_UI4, VTS_NONE)
	DISP_FUNCTION_ID(CTGCtrl, "GetLineCtrlWord", dispidGetLineCtrlWord, GetLineCtrlWord, VT_UI4, VTS_NONE)
	DISP_FUNCTION_ID(CTGCtrl, "GetMarkCtrlWord", dispidGetMarkCtrlWord, GetMarkCtrlWord, VT_UI4, VTS_NONE)
	DISP_FUNCTION_ID(CTGCtrl, "SetOperateRight", dispidSetOperateRight, SetOperateRight, VT_EMPTY, VTS_UI1)
	DISP_FUNCTION_ID(CTGCtrl, "GetOperateRight", dispidGetOperateRight, GetOperateRight, VT_UI1, VTS_NONE)
	DISP_FUNCTION_ID(CTGCtrl, "SetLeapLineFlag", dispidSetLeapLineFlag, SetLeapLineFlag, VT_EMPTY, VTS_UI1)
	DISP_FUNCTION_ID(CTGCtrl, "SetDrawTenicalStopMark", dispidSetDrawTenicalStopMark, SetDrawTenicalStopMark, VT_EMPTY, VTS_I2)
	DISP_FUNCTION_ID(CTGCtrl, "SetUseBmp", dispidSetUseBmp, SetUseBmp, VT_EMPTY, VTS_UI1)
	DISP_FUNCTION_ID(CTGCtrl, "SetAllStationActive", dispidSetAllStationActive, SetAllStationActive, VT_EMPTY, VTS_NONE)

	DISP_FUNCTION_ID(CTGCtrl, "SetPlanRecieveFlag", dispidSetPlanRecieveFlag, OnSetPlanRecieveFlag, VT_EMPTY, VTS_UI1 VTS_UI4 VTS_UI4)
	DISP_FUNCTION_ID(CTGCtrl, "SetLeapTrainShow", dispidSetLeapTrainShow, SetLeapTrainShow, VT_EMPTY, VTS_UI1)
	DISP_FUNCTION_ID(CTGCtrl, "SetRouteOkMarkFlag", dispidSetRouteOkMarkFlag, SetRouteOkMarkFlag, VT_EMPTY, VTS_UI1)
	DISP_FUNCTION_ID(CTGCtrl, "AliveWindow", dispidAliveWindow, AliveWindow, VT_EMPTY, VTS_UI4)
	DISP_FUNCTION_ID(CTGCtrl, "GetCurrentSectionID", dispidGetCurrentSectionID, GetCurrentSectionId, VT_UI4, VTS_NONE)
	DISP_FUNCTION_ID(CTGCtrl, "SetTrainDefaultInfo", dispidSetTrainDefaultInfo, OnSetTrainDefaultInfo, VT_BSTR, VTS_VARIANT)
	DISP_FUNCTION_ID(CTGCtrl, "GetTgVersion", dispidGetTgVersion, OnGetTgVersion, VT_BSTR, VTS_NONE)
	DISP_FUNCTION_ID(CTGCtrl, "GenTrainExitEntryByStationIndex", dispidGenTrainExitEntryByStationIndex, GenTrainExitEntryByStationIndex, VT_BSTR, VTS_VARIANT)
	DISP_FUNCTION_ID(CTGCtrl, "ShowWholeScreen", dispidShowWholeScreen, OnShowWholeScreen, VT_EMPTY, VTS_UI1)
	DISP_FUNCTION_ID(CTGCtrl, "SetContrlRight", dispidSetContrlRight, SetContrlRight, VT_EMPTY, VTS_UI4)
	DISP_FUNCTION_ID(CTGCtrl, "GetPlotParam", dispidGetPlotParam, OnGetPlotParam, VT_EMPTY, VTS_UI4)
	DISP_FUNCTION_ID(CTGCtrl, "GetTgData", dispidGetTgData, OnGetTgData, VT_EMPTY, VTS_UI4)
	DISP_FUNCTION_ID(CTGCtrl, "SetBasicTrainWholeMove", dispidSetBasicTrainWholeMove, OnSetBasicTrainWholeMove, VT_EMPTY, VTS_UI1)
	DISP_FUNCTION_ID(CTGCtrl, "RefreshTG", dispidRefreshTG, OnRefreshTG, VT_EMPTY, VTS_UI4)
	DISP_FUNCTION_ID(CTGCtrl, "BeginNewSingleTrainXml", dispidBeginNewSingleTrainXml, OnBeginNewSingleTrainXml, VT_EMPTY, VTS_BSTR)
	DISP_FUNCTION_ID(CTGCtrl, "TGGenTrainNodeIndex", dispidTGGenTrainNodeIndex, OnGenTrainNodeIndex, VT_EMPTY, VTS_BSTR)
	DISP_FUNCTION_ID(CTGCtrl, "AppendEarlyLateInfo", dispidAppendEarlyLateInfo, OnAppendEarlyLateInfo, VT_EMPTY, VTS_BSTR VTS_I1 VTS_I4)
	DISP_FUNCTION_ID(CTGCtrl, "SetWinTGOperateRight", dispidSetWinTGOperateRight, SetWinTGOperateRight, VT_EMPTY, VTS_UI4)
	DISP_FUNCTION_ID(CTGCtrl, "SetUseVirtualMemory", dispidSetUseVirtualMemory, SetUseVirtualMemory, VT_EMPTY, VTS_UI1)
	DISP_FUNCTION_ID(CTGCtrl, "SetWidePixel", dispidSetWidePixel, SetWidePixel, VT_EMPTY, VTS_UI1)
	
	DISP_FUNCTION_ID(CTGCtrl, "OnSetBatchDrawEnv", dispidOnStopDraw, OnSetBatchDrawEnv, VT_EMPTY, VTS_I4)
	DISP_FUNCTION_ID(CTGCtrl, "OnBatchDraw", dispidOnBatchDraw, OnBatchDraw, VT_EMPTY, VTS_I4)
	DISP_FUNCTION_ID(CTGCtrl, "OnBatchSchduleData", dispidOnBatchSchduleData, OnBatchSchduleData, VT_I4, VTS_VARIANT)
	DISP_FUNCTION_ID(CTGCtrl, "IsBatchStatus", dispidIsBatchStatus, IsBatchStatus, VT_UI1, VTS_NONE)
	DISP_FUNCTION_ID(CTGCtrl, "OnSetBackGroudInfo", dispidSetBackgrandInfo, SetBackGroudInfo, VT_EMPTY, VTS_UI4 VTS_UI4 VTS_I2 VTS_I2 VTS_I2)
	DISP_FUNCTION_ID(CTGCtrl, "NewShiftBanci", dispidNewShiftBanci, NewShiftBanci, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION_ID(CTGCtrl, "GetLoginShiftId", dispidGetLoginShiftId, GetLoginShiftId, VT_I4, VTS_NONE)

	DISP_FUNCTION_ID(CTGCtrl, "GetShiftBeginTime", dispidGetBeginTime, GetShiftBeginTime, VT_I4, VTS_NONE)
	DISP_FUNCTION_ID(CTGCtrl, "GetHistoryLen", dispidGetHistoryLen, GetHistoryLen, VT_I4, VTS_NONE)
	DISP_FUNCTION_ID(CTGCtrl, "GetAutoMove", dispidGetAutoMove, GetAutoMove, VT_I4, VTS_NONE)

	DISP_FUNCTION_ID(CTGCtrl, "DataIsExistInOCX", dispidDataIsExistInOCX, DataIsExistInOCX, VT_UI1, VTS_UI4 VTS_UI4)
	DISP_FUNCTION_ID(CTGCtrl, "SetSectionInfo", dispidSetSectionInfo, OnSetSectionInfo, VT_EMPTY, VTS_BSTR)
	DISP_FUNCTION_ID(CTGCtrl, "AddBStplanStation", dispidAddBStplanStation, AddBStplanStation, VT_EMPTY, VTS_UI4)

	DISP_FUNCTION_ID(CTGCtrl, "UsingWorkGoodMode", dispidUsingWorkGoodMode, UsingWorkGoodMode, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION_ID(CTGCtrl, "MoveTGOCXOneStep", dispidMoveTGOCXOneStep, MoveTGOCXOneStep, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION_ID(CTGCtrl, "SetUserRight", dispidSetUserRight, SetUserRight, VT_EMPTY, VTS_I4)
	DISP_FUNCTION_ID(CTGCtrl, "AddNotMeetInfo", dispidSetNotMeetInfo, AddNotMeetInfo, VT_EMPTY, VTS_I2 VTS_BSTR VTS_BSTR)
	DISP_FUNCTION_ID(CTGCtrl, "OnFindTrainByTrainIndex", dispidFindTrainByTrainIndex, OnFindTrainByTrainIndex, VT_EMPTY, VTS_I4)
	DISP_FUNCTION_ID(CTGCtrl, "SetFormatView", dispidSetFormatView, SetFormatView, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION_ID(CTGCtrl, "CheckSideByRelation", dispidCheckSideByRelation, CheckSideByRelation, VT_BSTR, VTS_VARIANT)
	DISP_FUNCTION_ID(CTGCtrl, "GetTrainJiaoRelation", dispidGetTrainJiaoRelation, GetTrainJiaoRelation, VT_BSTR, VTS_I4)
	
END_DISPATCH_MAP()

// 事件映射
BEGIN_EVENT_MAP(CTGCtrl, COleControl)
	EVENT_CUSTOM_ID("TG_APPEND_MARK", eventidTG_APPEND_MARK, FIRE_TG_APPEND_MARK, VTS_I4 VTS_BSTR)
	EVENT_CUSTOM_ID("TG_UPDATE_MARK", eventidTG_UPDATE_MARK, FIRE_TG_UPDATE_MARK, VTS_I4 VTS_BSTR)
	EVENT_CUSTOM_ID("TG_DELETE_MARK", eventidTG_DELETE_MARK, FIRE_TG_DELETE_MARK, VTS_I4 VTS_BSTR)
	
	EVENT_CUSTOM_ID("TG_APPEND_TRAIN", eventidTG_APPEND_TRAIN, FIRE_TG_APPEND_TRAIN, VTS_I4 VTS_BSTR)
	EVENT_CUSTOM_ID("TG_UPDATE_TRAIN", eventidTG_UPDATE_TRAIN, FIRE_TG_UPDATE_TRAIN, VTS_I4 VTS_BSTR)
	EVENT_CUSTOM_ID("TG_DELETE_TRAIN", eventidTG_DELETE_TRAIN, FIRE_TG_DELETE_TRAIN, VTS_I4 VTS_BSTR)

	EVENT_CUSTOM_ID("TG_LOG", eventidTG_LOG, FIRE_TG_LOG, VTS_BSTR)
	EVENT_CUSTOM_ID("TG_SELECT_TRAIN", eventidTG_SELECT_TRAIN, FIRE_TG_SELECT_TRAIN, VTS_UI4 VTS_UI4 VTS_UI2 VTS_UI2 VTS_UI4 VTS_UI4)
	EVENT_CUSTOM_ID("TG_SELECT_MARK", eventidTG_SELECT_MARK, FIRE_TG_SELECT_MARK, VTS_UI4 VTS_UI1 VTS_UI1 VTS_UI2 VTS_UI2 VTS_UI4)
	EVENT_CUSTOM_ID("TG_NOTICE", eventidTG_NOTICE, TG_NOTICE, VTS_UI1 VTS_UI4 VTS_UI4 VTS_UI2)
	//EVENT_CUSTOM_ID("TG_SET_TRAIN_DEFAULT_INFO", eventidTG_SET_TRAIN_DEFAULT_INFO, FIRE_SET_TRAIN_DEFAULT_INFO, VTS_BSTR VTS_I4)
	EVENT_CUSTOM_ID("TG_GEN_BLOCK_DCMD", eventidTG_GEN_BLOCK_DCMD, TG_GEN_BLOCK_DCMD, VTS_BSTR VTS_UI4 VTS_UI4)
	EVENT_CUSTOM_ID("TG_SET_PLOT_PARAM", eventidTG_SET_PLOT_PARAM, FIRE_SET_PLOT_PARAM, VTS_BSTR VTS_I4)
	EVENT_CUSTOM_ID("FIRE_DELETE", eventidTG_FIRE_DELETE, FIRE_DELETE, VTS_BSTR)
	EVENT_CUSTOM_ID("FIRE_REAL_TIME", eventidFIRE_REAL_TIME, FIRE_REAL_TIME, VTS_UI4 VTS_I2 VTS_UI4 VTS_UI4 VTS_UI4 VTS_UI2)
	EVENT_CUSTOM_ID("AddEarlyLateInfo", eventidAddEarlyLateInfo, AddEarlyLateInfo, VTS_BSTR VTS_UI1 VTS_I4)
	EVENT_CUSTOM_ID("FIRE_SHOW_CREW", eventidFireShowCrew, FIRE_SHOW_CREW, VTS_UI4 VTS_UI4 VTS_UI4)
END_EVENT_MAP()



// 属性页

// TODO: 按需要添加更多属性页。请记住增加计数！
BEGIN_PROPPAGEIDS(CTGCtrl, 1)
	PROPPAGEID(CTGPropPage::guid)
END_PROPPAGEIDS(CTGCtrl)



// 初始化类工厂和 guid

IMPLEMENT_OLECREATE_EX(CTGCtrl, "TG.TGCtrl.1",
	0xee53802c, 0x2d94, 0x483c, 0x8c, 0x4d, 0x91, 0x6b, 0x80, 0x55, 0x28, 0x11)



// 键入库 ID 和版本

IMPLEMENT_OLETYPELIB(CTGCtrl, _tlid, _wVerMajor, _wVerMinor)



// 接口 ID

const IID BASED_CODE IID_DTG =
		{ 0x454E72C2, 0x812B, 0x43CC, { 0xAD, 0xD1, 0xEB, 0xDC, 0x2D, 0x79, 0xE5, 0xC2 } };
const IID BASED_CODE IID_DTGEvents =
		{ 0x9B8ABB96, 0x1638, 0x4E78, { 0xA3, 0x4F, 0x2D, 0x3D, 0x64, 0x79, 0xE6, 0xB1 } };



// 控件类型信息

static const DWORD BASED_CODE _dwTGOleMisc =
	OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST |
	OLEMISC_INSIDEOUT |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_RECOMPOSEONRESIZE;

IMPLEMENT_OLECTLTYPE(CTGCtrl, IDS_TG, _dwTGOleMisc)

BSTR CTGCtrl::OnGetTgVersion(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CString strResult=":2.6.5";

	// TODO: 在此添加调度处理程序代码

	return strResult.AllocSysString();
}

BOOL CTGCtrl::CTGCtrlFactory::UpdateRegistry(BOOL bRegister)
{
	if (bRegister)
		return AfxOleRegisterControlClass(
			AfxGetInstanceHandle(),
			m_clsid,
			m_lpszProgID,
			IDS_TG,
			IDB_TG,
			afxRegApartmentThreading,
			_dwTGOleMisc,
			_tlid,
			_wVerMajor,
			_wVerMinor);
	else
		return AfxOleUnregisterClass(m_clsid, m_lpszProgID);
}

// CTGCtrl::CTGCtrl - 构造函数

CTGCtrl::CTGCtrl()
{
	InitializeIIDs(&IID_DTG, &IID_DTGEvents);
	// TODO: 在此初始化控件的实例数据。
	
	m_pFrame = NULL;
	m_pDocTemplate = NULL;
	mbFirstRun = false;
	m_bNewMode=false;
	m_bWorkMode=false;
	m_nBeginTm=0;
	m_nAutoMove=FALSE;
	m_nHistoryLen=5;	
	m_bFormatView=false;
	m_nTrainNoLevelCount=3;
	m_bplanstation.clear();
	::CoInitialize(NULL);
}

// CTGCtrl::~CTGCtrl - 析构函数
CTGCtrl::~CTGCtrl()
{
	// TODO: 在此清理控件的实例数据。
	delete m_pDocTemplate;
}

// CTGCtrl::OnDraw - 绘图函数

void CTGCtrl::OnDraw(
			CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid)
{
	if (!pdc)
		return;

	// TODO: 用您自己的绘图代码替换下面的代码。
	pdc->FillRect(rcBounds, CBrush::FromHandle((HBRUSH)GetStockObject(WHITE_BRUSH)));
	pdc->Ellipse(rcBounds);
}

// CTGCtrl::DoPropExchange - 持久性支持
void CTGCtrl::DoPropExchange(CPropExchange* pPX)
{
	ExchangeVersion(pPX, MAKELONG(_wVerMinor, _wVerMajor));
	COleControl::DoPropExchange(pPX);

	// TODO: 为每个持久的自定义属性调用 PX_ 函数。
}

// CTGCtrl::OnResetState - 将控件重置为默认状态

void CTGCtrl::OnResetState()
{
	COleControl::OnResetState();  // 重置 DoPropExchange 中找到的默认值

	// TODO: 在此重置任意其他控件状态。
}
void CTGCtrl::OnSize(UINT nType, int cx, int cy)
{
	COleControl::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	if(m_pFrame)
	{
		m_pFrame->SetWindowPos(NULL, 0, 0, cx, cy, SWP_NOMOVE|SWP_SHOWWINDOW);
	}
}



// -------------------------------------------------
// 下面是TG属性设置
//--------------------------------------------------
void CTGCtrl::OnSetBackGroud(USHORT flag)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	mConfigTg.m_context.SetBackgroudBlock(flag);
}

void CTGCtrl::OnSetTgType(BYTE type)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	m_nTermSchdType=type;
	if(TERM_TG_TYPE::WORK_TG == type || TERM_TG_TYPE::SHIFT_TG == type)
	{
		m_nTgSchdType = TG_TYPE::WORK_TG;
	}
	else if(TERM_TG_TYPE::NOW_BASIC_TG == type || TERM_TG_TYPE::PRE_BASIC_TG == type  || TERM_TG_TYPE::READY_BASIC_TG == type)
	{
		m_nTgSchdType = TG_TYPE::BASIC_TG;
	}
	else
	{
		m_nTgSchdType = TG_TYPE::QUERY_TG;
	}
}

void CTGCtrl::UsingWorkGoodMode(void)
{
	m_bWorkMode=true;
}

void CTGCtrl::OnSetBackgroudBeginHour(BYTE begin_hour)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CTGFrame* pMainFrme = (CTGFrame*)m_pFrame;
	if(pMainFrme != NULL)
	{
		CTGForm* form = pMainFrme->GetFormView();
		if(form)
		{
			if(begin_hour == BACKGROUD::TIME::BEGIN::TOWNTY)
				form->OnShowBeginTimePoint12();
			else if(begin_hour == BACKGROUD::TIME::BEGIN::SIX)
				form->OnShowBeginTimePoint6();
		}
	}
}

void CTGCtrl::SetShiftId(ULONG nshiftid, SHORT nCurValue)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	m_nShiftid = nshiftid;
}

void CTGCtrl::OnSetBackgroudTimeLength(BYTE nHours)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	m_nTimeLength = nHours;
}

void CTGCtrl::SetFormatView(void)
{
	m_bFormatView=true;
}

// ------------------------动态-------------------------------
void CTGCtrl::SetTgCfg(CString dataPath)
{
	CString tmp = "[StartTG] data path:" + dataPath;
	this->FIRE_TG_LOG(tmp);
    
	// path
	this->mConfigTg.m_context.SetDataPath(dataPath);

	// load tg cfg
	this->mConfigTg.LoadTGFile(this);

	// backgrand
	if(m_bNewMode)
	{
		return;
	}

	if(TG_TYPE::BASIC_TG == m_nTgSchdType)
	{
		this->mConfigTg.m_context.SetDrawCurrTimeBar(false);
		SetBeginHour(18);
		m_nTimeLength=BACKGROUD::TIME::LENGTH::TOWNTYFOUR;
		this->mConfigTg.m_context.SetBackgroudBlock(BACKGROUD::BLOCK::ONE);
		this->mConfigTg.m_context.SetBasicSchdCanModify(0);
		this->mConfigTg.m_bDrawCurrentFlag = false;
	}
	else if(TG_TYPE::QUERY_TG == m_nTgSchdType)
	{
		this->mConfigTg.m_context.SetDrawCurrTimeBar(false);
		m_nTimeLength=BACKGROUD::TIME::LENGTH::TOWNTYFOUR;
		this->mConfigTg.m_context.SetBackgroudBlock(BACKGROUD::BLOCK::ONE);
		this->mConfigTg.m_context.SetBasicSchdCanModify(0);
		this->mConfigTg.m_bDrawCurrentFlag = false;
	}
	else
	{
		m_nTimeLength=BACKGROUD::TIME::LENGTH::THIRTY;
		this->mConfigTg.m_context.SetDrawCurrTimeBar(true);
		this->mConfigTg.m_context.SetBasicSchdCanModify(1);
		this->mConfigTg.m_bDrawCurrentFlag = true;
	}
}

void CTGCtrl::SetIsDrawCurrentFlag(bool bDraw)
{
	// backgrand
	if(TG_TYPE::BASIC_TG == m_nTgSchdType)
	{
		this->mConfigTg.m_bDrawCurrentFlag = false;
	}
	else if(TG_TYPE::QUERY_TG == m_nTgSchdType)
	{
		this->mConfigTg.m_bDrawCurrentFlag = false;
	}
	else
	{
		this->mConfigTg.m_bDrawCurrentFlag = bDraw;
	}
}

void CTGCtrl::TgStart(LPCSTR filename)
{
	SetTgCfg(filename);
	// Create a Document View Framework
	if(NULL == m_pDocTemplate)
	{
		m_pDocTemplate = new CSingleDocTemplateForActiveX(
			IDR_RBUTTON_POPMENU,
			RUNTIME_CLASS(CTGDoc),
			RUNTIME_CLASS(CTGFrame),       // main SDI frame window
			RUNTIME_CLASS(CTGForm));
	}
	if(NULL == m_pFrame)
	{
		m_pFrame = (CFrameWnd*)((CSingleDocTemplateForActiveX*)m_pDocTemplate)->NewDVF(this);
		ASSERT(m_pFrame);
	}

	if(NULL == m_pFrame)
		return;
	
	CRect rectWnd;
	GetWindowRect(&rectWnd);
	OnSize(0, rectWnd.Width(), rectWnd.Height());

	if(m_pFrame)
	{
		m_pFrame->SetWindowPos(NULL, 0, 0, rectWnd.Width(), rectWnd.Height(), SWP_NOMOVE|SWP_SHOWWINDOW);
	}
	m_pFrame->ShowWindow(SW_SHOW);
	m_pFrame->UpdateWindow();

	if (mbFirstRun)
	{
		CTGFrame* pMainFrme = (CTGFrame*)m_pFrame;
		CTGForm* form = pMainFrme->GetFormView();
		if(form)
			form->SendMessage(MSG_SELECT_TG_SECTION, 0, 1); 
	}
	mbFirstRun = true;

	timeb cur;
	ftime(&cur);
	m_timePress = ((short)cur.time)*1000 + cur.millitm;
}

//--------------------------------------------------------------------
void CTGCtrl::OnBeginNewSingleTrain(LPCTSTR strTrainId, LPCTSTR strChangeTrainId, BYTE inMode, BYTE outMode)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: 在此添加调度处理程序代码
	CTGFrame* pMainFrme = (CTGFrame*)m_pFrame;
	if(pMainFrme != NULL)
	{
		DrawTrainInfo info;
		info.strTrainId = strTrainId;
		info.strArrivalTrainId = strChangeTrainId;
		info.nStartMode = (inMode&~0x80);
		info.nEndMode = outMode;
		info.nSecretTrain=(0x80&inMode);
		
		CTGForm* form = pMainFrme->GetFormView();
		if(form)
		{
			form->SendMessage(MSG_ADDNEW_TRAIN_GRAPH, 0, (LPARAM)&info); 
		}
	}
}

void CTGCtrl::OnBeginNewSingleTrainXml(LPCTSTR xml)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: 在此添加调度处理程序代码
	CTGFrame* pMainFrme = (CTGFrame*)m_pFrame;
	if(pMainFrme != NULL)
	{
		CTGForm* form = pMainFrme->GetFormView();
		if(form)
		{
			form->SendMessage(MSG_ADDNEW_TRAIN_GRAPH_XML, 0, (LPARAM)xml); 
		}
	}
}

void CTGCtrl::OnEndNewSingleTrain(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: 在此添加调度处理程序代码
	m_pFrame->SendMessage(MSG_ADDNEW_TRAIN_GRAPH_END, 0, 0);
}

void CTGCtrl::OnSetTrainDisplayFlag(SHORT type, SHORT val)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: 在此添加调度处理程序代码
	if(type == TRAIN_DISPLY::LATETRAIN)
		mConfigTg.m_context.SetLateEarlyTime(val);
	else if(type == TRAIN_DISPLY::LINE)
	{
		if(mConfigTg.m_context.GetLineWidth()==val)
			return;
		mConfigTg.m_context.SetLineWidth(val);
	}
	else if(type == TRAIN_DISPLY::TYPE)
		mConfigTg.m_context.SetShowTrainType(val);
	else if(type == TRAIN_DISPLY::DIRECT)
		mConfigTg.m_context.SetShowTrainDirect(val);
	else if(type == TRAIN_DISPLY::BSCHD_OPER)
	{
		if(mConfigTg.m_context.GetBasicSchdCanModify())
			mConfigTg.m_context.SetBasicSchdCanModify(0);
		else
			mConfigTg.m_context.SetBasicSchdCanModify(1);
	}
	else if(type == TRAIN_DISPLY::LOCK_SIDE)
		mConfigTg.m_context.SetEnableLockSide(val);
	else if(type == TRAIN_DISPLY::GREEN_DECORATE_LINE)
		mConfigTg.m_context.SetElecDrawGreenDecorate(val);
	else if(type == TRAIN_DISPLY::TRAIN_CZH)
		mConfigTg.m_context.EnableCZH(val);
	else if(type == TRAIN_DISPLY::SWITCH_STATION)
		mConfigTg.m_context.AddSwtichStation(val);
	else if(type == TRAIN_DISPLY::NOT_SHOW_SIDE_CTC_STATION)
		mConfigTg.m_context.AddNotShowSideCtcStation(val);

	else if(type == TRAIN_DISPLY::RECIEVE_POINT_LIMIT_TIME)
		mConfigTg.m_context.SetLimitModifyTimeLen(val);

	else if(type == TRAIN_DISPLY::LOCK_SIDE_NOT_TECH_STOP)
		mConfigTg.m_context.LockSideNotTechStop(val);

	else if(type == TRAIN_DISPLY::SET_MOVE_SAME_CZ_TRAIN)
		mConfigTg.m_context.SetMoveSameCZTrain(val>0);
	
	else if(type == TRAIN_DISPLY::SET_NOT_THIS_SECTION_STATION)
		mConfigTg.AddNotThisSectionStation(val);

	else if (type == TRAIN_DISPLY::NOT_DELETE_REAL_TIME)
		mConfigTg.m_context.SetProhibitDeleteRealTimeFlag(val);
	
	else if (type == TRAIN_DISPLY::SET_NO_LIMIT_STATION)
		mConfigTg.AddNoLimitStation(val);
	
	else if (type == TRAIN_DISPLY::SET_LIMIT_MODIFY_ACT_TIME)
		mConfigTg.m_context.SetLimitModifyActTimeFlag(val);

	else if(type == TRAIN_DISPLY::INTER_CITY_STPLAN)
		mConfigTg.m_context.EnableInterCityStplan(val);

	else if(type == TRAIN_DISPLY::ERROR_SIDE_STATION)
		mConfigTg.m_context.AddErrorSideStation(val);

	else if(type == TRAIN_DISPLY::ENABLE_SHOW_RUNTIME_WND)
		mConfigTg.SetShowRuntimeWnd();

	else if(type == TRAIN_DISPLY::DISENABLE_SHEET_TAB)
		mConfigTg.SetDisenableSheetTab(val);

	else if(type == TRAIN_DISPLY::TRAIN_NEW_CZH)
		mConfigTg.m_context.EnableNewCZH(val);

	else if(type == TRAIN_DISPLY::SET_TRAIN_RETURN_ID)
		mConfigTg.m_context.EnableSetCZHReturnTrainID(val);
	
	else if(type == TRAIN_DISPLY::SHOW_TRAIN_NO_LEVEL_COUNT)
	{
		if(val>m_nTrainNoLevelCount)
			m_nTrainNoLevelCount=val;
	}
	else if(type == TRAIN_DISPLY::BASIC_MOVE_NULTI_STATION)
		mConfigTg.SetIsCanMoveBasciTrainWhole(val==1);

	else if(type == TRAIN_DISPLY::ERASE_BEFORE_MODIFY)
		mConfigTg.SetEraseBeforeModifyTrain(val);

	else if(type == TRAIN_DISPLY::DISABLE_MODIFY_PLAN_TM)
		mConfigTg.SetDisableModifyPlanTm(val);

	else if(type == TRAIN_DISPLY::RUN_FAST_SLOW_WITH_BASIC)
		mConfigTg.SetRunFastSlowCompareWithBase(val);
	
	else if(type == TRAIN_DISPLY::SET_PIXEL_PER_MINUTE)
		mConfigTg.m_context.SetPixelPerMinute(val);

	CTGFrame* pMainFrme = (CTGFrame*)m_pFrame;
	if(NULL != pMainFrme)
	{
		CTGForm* form = pMainFrme->GetFormView();
		if(type == TRAIN_DISPLY::REVERSE)
			pMainFrme->ShowTrainReverse(val);
		else if(type == TRAIN_DISPLY::MUTI_SEL)
		{
			pMainFrme->SetMutiSelFlag(val);
		}
		else if(type == TRAIN_DISPLY::LATETRAIN)
		{
			pMainFrme->TrainReDraw(type);
		}
		else if(type == TRAIN_DISPLY::LINE)
		{
			pMainFrme->TrainReDraw(type);
		}
		else if(type == TRAIN_DISPLY::TYPE)
		{
			// 客货车
			if(NULL != form)
			{
				form->ResetFocusTrain();
				form->Redraw();
			}
		}
		else if(type == TRAIN_DISPLY::DIRECT)
		{
			// 上下行方向
			if(NULL != form)
			{
				form->ResetFocusTrain();
				if(this->IsWorkMode())
					form->DisplyTrainByDirection();
				else
					form->Redraw();//ok
			}
		}
		else if(type == TRAIN_DISPLY::KEY_DOWN)
		{
			pMainFrme->KeyDown(val);
		}
		else if(type == TRAIN_DISPLY::EDIT_REAL_TIME)
		{
			pMainFrme->ProhibitEditRealTime(val);
		}
		else if(type == TRAIN_DISPLY::GEN_TRAINS_RELATION)
		{
			pMainFrme->OutputTrainsRelation();
		}
	}
}

SHORT CTGCtrl::GetTrainDisplayFlag(SHORT type, SHORT val)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: 在此添加调度处理程序代码
	if(type == TRAIN_DISPLY::LATETRAIN)
		return this->mConfigTg.m_context.GetLateEarlyTime();
	else if(type == TRAIN_DISPLY::LINE)
		return this->mConfigTg.m_context.GetLineWidth();
	else if(type == TRAIN_DISPLY::TYPE)
		return this->mConfigTg.m_context.GetShowTrainType();
	else if(type == TRAIN_DISPLY::DIRECT)
		return this->mConfigTg.m_context.GetShowTrainDirect();
	else if(type == TRAIN_DISPLY::BSCHD_OPER)
		return this->mConfigTg.m_context.GetBasicSchdCanModify();
	else if(type == TRAIN_DISPLY::BASIC_MOVE_NULTI_STATION)
		return mConfigTg.IsCanMoveBasciTrainWhole() ? TRUE:FALSE;
	
	CTGFrame* pMainFrme = (CTGFrame*)m_pFrame;
	if(NULL != pMainFrme)
	{
		if(type == TRAIN_DISPLY::REVERSE)
			return pMainFrme->GetTrainReverse(val);
	}
	return -1;
}
SHORT CTGCtrl::OnDrawSpecialTrain(SHORT trainType)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: 在此添加调度处理程序代码
	CTGFrame* pMainFrme = (CTGFrame*)m_pFrame;
	pMainFrme->drawSpecialTrain(trainType);
	return 0;
}

SHORT CTGCtrl::OnSelectSection(SHORT nSection, LONG nFlag)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: 在此添加调度处理程序代码
	if(nFlag==1)
		SetIsDrawCurrentFlag(true);
	else
		SetIsDrawCurrentFlag(false);

	mConfigTg.SetCurrentSectionID(nSection);
	CTGFrame* pMainFrme = (CTGFrame*)m_pFrame;
	if(pMainFrme != NULL)
	{
		pMainFrme->SendMessage(MSG_SELECT_TG_SECTION, nSection, 0); 
	}
	
	return 0; 
}

void CTGCtrl::OnFindTrain(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: 在此添加调度处理程序代码
	CTGFrame* pMainFrme = (CTGFrame*)m_pFrame;
	pMainFrme->FindTrain();
}

void CTGCtrl::OnFindTrainByTrainIndex(ULONG train_index)
{
	CTGFrame* pMainFrme = (CTGFrame*)m_pFrame;
	pMainFrme->FindTrain(train_index);
}

void CTGCtrl::OnTrainsOperator(SHORT oper, SHORT rst)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: 在此添加调度处理程序代码
	CTGFrame* pMainFrme = (CTGFrame*)m_pFrame;
	if(pMainFrme != NULL)
	{
		CTGForm* form = pMainFrme->GetFormView();
		if(form)
		   form->SendMessage(MSG_TRAINS_OPERATOR, oper, rst); 
	}
}


void CTGCtrl::OnNewMark(SHORT mark_type)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: 在此添加调度处理程序代码
	CTGFrame* pMainFrme = (CTGFrame*)m_pFrame;
	if(pMainFrme != NULL)
	{
		CTGForm* form = pMainFrme->GetFormView();
		if(form)
		   form->SendMessage(MSG_MENUADD_MARK_GRAPH, mark_type, 0); 
	}
}

SHORT CTGCtrl::OnZoomIn(SHORT param)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: 在此添加调度处理程序代码
	SHORT nPixel=0;
	CTGFrame* pMainFrme = (CTGFrame*)m_pFrame;
	CTGForm* form = pMainFrme->GetFormView();
	if(form)
		nPixel = form->OnZoomGraph(GRAPH_ZOOM::SMALL, param); 
	return nPixel;
}

SHORT CTGCtrl::OnZoomOut(SHORT param)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: 在此添加调度处理程序代码
	SHORT nPixel=0;
	CTGFrame* pMainFrme = (CTGFrame*)m_pFrame;
	CTGForm* form = pMainFrme->GetFormView();
	if(form)
		nPixel = form->OnZoomGraph(GRAPH_ZOOM::BIG, param);
	return nPixel;
}

void CTGCtrl::OnSetStationConnectStatus(SHORT nStaId, BYTE active)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: 在此添加调度处理程序代码
	CTGFrame* pMainFrme = (CTGFrame*)m_pFrame;
	if(pMainFrme != NULL)
	{
		pMainFrme->SendMessage(MSG_SET_STATION_STATUS, nStaId, active); 
	}	
}

void CTGCtrl::OnSetCtrlMode(SHORT nStaId, SHORT sta_ctrl_mode, SHORT route_ctrl_mode)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: 在此添加调度处理程序代码
	msgCtrlMode mode;
	mode.staId = nStaId;
	mode.route_mode = (BYTE)route_ctrl_mode;
	mode.sta_mode = sta_ctrl_mode;

	CTGFrame* pMainFrme = (CTGFrame*)m_pFrame;
	if(pMainFrme != NULL)
	{
		pMainFrme->SendMessage(MSG_SET_CTRL_MODE, 0, (LPARAM)&mode); 
	}	
}

void CTGCtrl::OnSetPlanRecieveFlag(BYTE nType, ULONG id, ULONG color)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: 在此添加调度处理程序代码
	msgPlanParam mode;
	mode.staId = id;
	mode.type = nType;
	mode.color = color;
	CTGFrame* pMainFrme = (CTGFrame*)m_pFrame;
	if(pMainFrme != NULL)
	{
		pMainFrme->SendMessage(MSG_SET_PLAN_STATUS, 0, (LPARAM)&mode); 
	}	
}
SHORT CTGCtrl::OnGetZoomStatus(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: 在此添加调度处理程序代码
	CTGFrame* pMainFrme = (CTGFrame*)m_pFrame;
	if(pMainFrme != NULL)
	{
		return pMainFrme->GetZoomStatus();
	}
	return -1;
}

void CTGCtrl::OnDeleteMutiTrain(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: 在此添加调度处理程序代码
	CTGFrame* pMainFrme = (CTGFrame*)m_pFrame;
	pMainFrme->SetDeleteMultiTrainFlag();
}


void CTGCtrl::OnSetStationInfo(LPCTSTR xml)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: 在此添加调度处理程序代码
	CStdioFile file;
	if(file.Open("c:\\StationInfo.xml", CFile::modeCreate | CFile::modeWrite))
	{
		file.WriteString(xml);
		file.Close();
	}
	this->mConfigTg.SetStationInfo(xml, this);
}

void CTGCtrl::OnSetSectionInfo(LPCTSTR xml)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	this->mConfigTg.SetSectionInfo(xml, this);
}

void CTGCtrl::OnSetCurrentSectionId(SHORT flag)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	this->mConfigTg.SetCurrentSectionID(flag);
}

void CTGCtrl::OnSaveToPicture(LPCTSTR pathname)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
}

void CTGCtrl::OnTurnReality(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	timeb cur;
	ftime(&cur);
	DWORD curPress = ((short)cur.time)*1000 + cur.millitm;

	if(curPress - m_timePress > 1000)
	{
		m_timePress = curPress;
		CTGFrame* pMainFrme = (CTGFrame*)m_pFrame;
		if(pMainFrme != NULL)
		{
			CTGForm* form = pMainFrme->GetFormView();
			if(form)
			{
				form->OnTurnReality(); 
				this->FIRE_TG_LOG("调度员通过点击按钮进行转实际");
			}
		}
	}
}

void CTGCtrl::OnReceivePoint(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: 在此添加调度处理程序代码
	((CTGFrame*)m_pFrame)->OnReceivePoint();
}
//-------------------------数据处理-----------------------------
void CTGCtrl::OnBeginLoadData(void)
{
	((CTGFrame*)m_pFrame)->SendMessage(MSG_LOAD_DATA, SCHD_RECIEVE_STATUS::BEGIN, 0); 
}

void CTGCtrl::OnEndLoadData(void)
{
	((CTGFrame*)m_pFrame)->SendMessage(MSG_LOAD_DATA, SCHD_RECIEVE_STATUS::END, 0); 
}

void CTGCtrl::OnDeleteMark(LONG nIndex)
{
	((CTGFrame*)m_pFrame)->myDataManage.DeleteMark(nIndex);
}

void CTGCtrl::OnDeleteTrain(ULONG train_index, ULONG dwLogicFlag)
{
	((CTGFrame*)m_pFrame)->myDataManage.DeleteTrain(train_index, dwLogicFlag);
}

void CTGCtrl::SetAppCtrlWord(ULONG word)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
}

void CTGCtrl::SetLineCtrlWord(ULONG word)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	this->mConfigTg.m_context.SetTrainFuncCtrlWord(word);
}

void CTGCtrl::SetMarkCtrlWord(ULONG word)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	this->mConfigTg.m_context.SetMarkFuncCtrlWord(word);
}

ULONG CTGCtrl::GetAppCtrlWord(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return this->mConfigTg.m_context.GetAppFuncCtrlWord();
}

ULONG CTGCtrl::GetLineCtrlWord(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return this->mConfigTg.m_context.GetTrainFuncCtrlWord();
}

ULONG CTGCtrl::GetMarkCtrlWord(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return this->mConfigTg.m_context.GetMarkFuncCtrlWord();
}

void CTGCtrl::SetOperateRight(BYTE right)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: 在此添加调度处理程序代码
	return this->mConfigTg.m_context.SetOperateRight(right);
}

BYTE CTGCtrl::GetOperateRight(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: 在此添加调度处理程序代码
	return this->mConfigTg.m_context.GetOperateRight();
}

void CTGCtrl::SetLeapLineFlag(BYTE flag)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: 在此添加调度处理程序代码
	return this->mConfigTg.m_context.SetLeapLineFlag(flag);
}

void CTGCtrl::SetDrawTenicalStopMark(SHORT flag)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: 在此添加调度处理程序代码
	return this->mConfigTg.m_context.SetDrawTenicalStopMark(flag);
}

void CTGCtrl::SetUseBmp(BYTE flag)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: 在此添加调度处理程序代码
}

void CTGCtrl::SetAllStationActive(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: 在此添加调度处理程序代码
	this->mConfigTg.bAllStationActive = true;
}


void CTGCtrl::SetLeapTrainShow(BYTE nFlag)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: 在此添加调度处理程序代码
	//1: 无标记 2: 接入交出 3: 连线
	if(nFlag == this->mConfigTg.m_context.GetLeapLineFlag())
		return;
	this->mConfigTg.m_context.SetLeapLineFlag(nFlag);
	CTGFrame* pMainFrme = (CTGFrame*)m_pFrame;
	if(pMainFrme != NULL)
	{
		pMainFrme->SendMessage(MSG_REFRESH_TG, nFlag, 0);
	}
}

void CTGCtrl::SetRouteOkMarkFlag(BYTE flag)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: 在此添加调度处理程序代码
	// 进路准备好标志----0: 不绘制 1: 紫色线 2: 圆圈
	return this->mConfigTg.m_context.SetDrawRouteOkFlag(flag);
}

void CTGCtrl::AliveWindow(ULONG nFlag)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: 在此添加调度处理程序代码
	CTGFrame* pMainFrme = (CTGFrame*)m_pFrame;
	if(pMainFrme != NULL)
	{
		pMainFrme->SendMessage(MSG_ALIVE_WINDOW, nFlag, 0);
	}
}

ULONG CTGCtrl::GetCurrentSectionId(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: 在此添加调度处理程序代码
	short nSectionId = mConfigTg.GetCurrentSectionID();
	return nSectionId;
}

BSTR CTGCtrl::OnSetTrainDefaultInfo(const VARIANT& msg)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: 在此添加调度处理程序代码
	CString strResult="";
	COleSafeArray varRcv;
	BYTE *pdata;
	varRcv=msg;
    varRcv.AccessData((LPVOID*)&pdata);
	int datalen=varRcv.GetOneDimSize();  
	if(pdata)
	{
		CLBTH::Archive lar( CLBTH::Archive::load, datalen,pdata);
		CLBTH::MSGHEAD msghead;
		msghead.length=0;
		if(!lar.eof()) 
		{
			msghead = CLBTH::BaseMsg::getMsgHead(lar);
			if(msghead.length < CLBTH::MSGHEAD::SIZEOF || msghead.length > lar.getCount()) 
			{
				;
			}
			else
			{
				switch(msghead.id)
				{
				case MSGID_MsgTrainOCX:
					{
						CLBTH::MsgTrainOCX  msg;
						CLBTH::HRCODE hrcode=msg.serialize(lar);
						if(hrcode.failed()) 
							break;

						CTG_TRAIN train;
						if(TRUE==CopyTrainFromMsg(train, msg))
						{
							strResult = ((CTGFrame*)m_pFrame)->OnSetTrainDefault(train);
						}
					} 
					break;
				default:
					break;
				}
			}
		}
	}

	varRcv.UnaccessData();

	return strResult.AllocSysString();
}

BSTR CTGCtrl::GenTrainExitEntryByStationIndex(const VARIANT& msg)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	
	CString strResult="";
	COleSafeArray varRcv;
	BYTE *pdata;
	varRcv=msg;
    varRcv.AccessData((LPVOID*)&pdata);
	int datalen=varRcv.GetOneDimSize();  
	if(pdata)
	{
		CLBTH::Archive lar( CLBTH::Archive::load, datalen,pdata);
		CLBTH::MSGHEAD msghead;
		msghead.length=0;
		if(!lar.eof()) 
		{
			msghead = CLBTH::BaseMsg::getMsgHead(lar);
			if(msghead.length < CLBTH::MSGHEAD::SIZEOF || msghead.length > lar.getCount()) 
			{
				;
			}
			else
			{
				switch(msghead.id)
				{
				case MSGID_MsgTrainOCX:
					{
						CLBTH::MsgTrainOCX  msg;
						CLBTH::HRCODE hrcode=msg.serialize(lar);
						if(hrcode.failed()) 
							break;

						CTG_TRAIN train;
						if(TRUE==CopyTrainFromMsg(train, msg))
						{
							strResult = ((CTGFrame*)m_pFrame)->SetTrainExitEntryByStationIndex(train);
						}
					} 
					break;
				default:
					break;
				}
			}
		}
	}

	varRcv.UnaccessData();

	return strResult.AllocSysString();
}

void CTGCtrl::OnShowWholeScreen(BYTE nFlag)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: 在此添加调度处理程序代码
	CTGFrame* pMainFrme = (CTGFrame*)m_pFrame;
	CTGForm* form = pMainFrme->GetFormView();
	if(NULL != form)
	{
		form->OnShowWholeScreen(nFlag);
	}
}

void CTGCtrl::SetContrlRight(unsigned long nLFlag)
{
	this->mConfigTg.m_context.SetKaCtrlFlag(nLFlag);
}

void CTGCtrl::OnGetPlotParam(ULONG nFlag)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: 在此添加调度处理程序代码
	((CTGFrame*)m_pFrame)->SendMessage(MSG_GET_PLOT_PARAM, nFlag, 0); 
}

void CTGCtrl::OnGetTgData(ULONG flag)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: 在此添加调度处理程序代码
	((CTGFrame*)m_pFrame)->SendMessage(MSG_GET_TG_DATA, flag, 0);
}

void CTGCtrl::OnSetBasicTrainWholeMove(BYTE nFlag)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: 在此添加调度处理程序代码
	this->mConfigTg.SetIsCanMoveBasciTrainWhole((bool)(nFlag > 0));
}

void CTGCtrl::OnRefreshTG(ULONG nFlag)
{
	CTGFrame* pMainFrme = (CTGFrame*)m_pFrame;
	if(pMainFrme != NULL)
	{
		pMainFrme->SendMessage(MSG_REFRESH_TG, nFlag, 1);
	}
}

void CTGCtrl::OnGenTrainNodeIndex(LPCTSTR xml)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CTGFrame* pMainFrme = (CTGFrame*)m_pFrame;
	if(pMainFrme != NULL)
	{
		pMainFrme->SendMessage(MSG_GEN_TRAIN_INDEX, (WPARAM)xml, 0);
	}
}

void CTGCtrl::SetWinTGOperateRight(DWORD right)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: 在此添加调度处理程序代码
	return this->mConfigTg.m_context.SetWinTGOperateRight(right);
}

void CTGCtrl::SetWidePixel(short val)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
}

void CTGCtrl::SetUseVirtualMemory(short val)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
}

void CTGCtrl::OnAppendEarlyLateInfo(LPCTSTR xml, BYTE val, LONG shiftid)
{
	LateInfo info;
	info.flag=val;
	info.shiftid=shiftid;

	this->mConfigTg.m_earlylatetimelog[xml]= info;
}

ULONG CTGCtrl::GetLoginShiftId()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return m_nShiftid;
}

ULONG CTGCtrl::GetShiftBeginTime(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	ULONG btm=0;
	CTGFrame* pMainFrme = (CTGFrame*)m_pFrame;
	if(pMainFrme != NULL)
	{
		CTGDoc *pDoc  = (CTGDoc*)pMainFrme->GetActiveDocument();
		if(pDoc == NULL) 
			return 0;
		btm=pDoc->GetBeginTime();
	}
	return btm;
}
	
ULONG CTGCtrl::GetHistoryLen(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return m_nHistoryLen;
}
	
ULONG CTGCtrl::GetAutoMove(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return m_nAutoMove;
}


void CTGCtrl::OnSetBatchDrawEnv(LONG nFlag)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: 在此添加调度处理程序代码
	((CTGFrame*)m_pFrame)->myDataManage.SetBatchDrawEnv(nFlag);
}

LONG CTGCtrl::OnBatchSchduleData(const VARIANT& msg)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: 在此添加调度处理程序代码
	LONG result=0;
	COleSafeArray varRcv;
	BYTE *pdata;
	varRcv=msg;
    varRcv.AccessData((LPVOID*)&pdata);
	int datalen=varRcv.GetOneDimSize();  
	if(pdata)
	{
		CLBTH::Archive lar( CLBTH::Archive::load, datalen,pdata);
		CLBTH::MSGHEAD msghead;
		msghead.length=0;
		if(!lar.eof()) 
		{
			msghead = CLBTH::BaseMsg::getMsgHead(lar);
			if(msghead.length < CLBTH::MSGHEAD::SIZEOF || msghead.length > lar.getCount()) 
			{
				;
			}
			else
			{
				switch(msghead.id)
				{
				case MSGID_MsgTrainOCX:
					{
						CLBTH::MsgTrainOCX  msg;
						CLBTH::HRCODE hrcode=msg.serialize(lar);
						if(hrcode.failed()) 
							break;

						CTG_TRAIN train;
						if(TRUE==CopyTrainFromMsg(train, msg))
						{
							if(IsBatchStatus()==TRUE)
								((CTGFrame*)m_pFrame)->myDataManage.BatchAddTrain(train);
							else
								((CTGFrame*)m_pFrame)->myDataManage.UpdateTrain(train);
						}
					} 
					break;
				case MSGID_MsgSchdBlockOCX:
					{
						CLBTH::MsgSchdBlockOCX  msg;
						CLBTH::HRCODE hrcode=msg.serialize(lar);
						if(hrcode.failed()) 
							break;

						CTg_BLOCK block;
						if(TRUE==CopyBlockFromMsg(block, msg))
						{
							if(IsBatchStatus()==TRUE)
								((CTGFrame*)m_pFrame)->myDataManage.BatchAddBlock(block);
							else
								((CTGFrame*)m_pFrame)->myDataManage.UpdateMark(block);
						}
					} 
					break;
				default:
					break;
				}
			}
		}
	}

	varRcv.UnaccessData();

	return result;
}

void CTGCtrl::OnBatchDraw(LONG nFlag)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: 在此添加调度处理程序代码
	((CTGFrame*)m_pFrame)->myDataManage.BatchDraw(nFlag);
}

BYTE CTGCtrl::IsBatchStatus(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: 在此添加调度处理程序代码
	return ((CTGFrame*)m_pFrame)->myDataManage.IsBatchStatus();
}

void CTGCtrl::NewShiftBanci()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CTGFrame* pMainFrme = (CTGFrame*)m_pFrame;
	if(pMainFrme != NULL)
	{
		pMainFrme->OnNewShiftBanci(); 
	}
}

void CTGCtrl::SetBackGroudInfo(ULONG nShiftid, ULONG nBeginTm, SHORT nAutoMove, SHORT nTotalLen, SHORT nHistoryLen)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	m_bNewMode=true;
	m_nShiftid = nShiftid;
	m_nBeginTm=nBeginTm;
	m_nAutoMove=nAutoMove;
	m_nHistoryLen=nHistoryLen;	

	// backgrand
	m_nTimeLength = nTotalLen;

	if(TG_TYPE::BASIC_TG == m_nTgSchdType)
	{
		this->mConfigTg.m_context.SetDrawCurrTimeBar(false);
		SetBeginHour(18);
		
		this->mConfigTg.m_context.SetBackgroudBlock(BACKGROUD::BLOCK::ONE);
		this->mConfigTg.m_context.SetBasicSchdCanModify(0);
		this->mConfigTg.m_bDrawCurrentFlag = false;
	}
	else if(TG_TYPE::QUERY_TG == m_nTgSchdType)
	{
		this->mConfigTg.m_context.SetDrawCurrTimeBar(false);
	
		this->mConfigTg.m_context.SetBackgroudBlock(BACKGROUD::BLOCK::ONE);
		this->mConfigTg.m_context.SetBasicSchdCanModify(0);
		this->mConfigTg.m_bDrawCurrentFlag = false;
	}
	else
	{
		this->mConfigTg.m_context.SetDrawCurrTimeBar(true);
		this->mConfigTg.m_context.SetBasicSchdCanModify(1);
		this->mConfigTg.m_bDrawCurrentFlag = true;
	}

	CTime btm(m_nBeginTm);
	CString str;
	str.Format("[设置OCX时间范围]shiftid %d btime %s auto_move %d view_hour %d history_hour %d",
		m_nShiftid, btm.Format("%Y-%m-%d %H:%M:%S"), m_nAutoMove, m_nTimeLength, m_nHistoryLen);

	FIRE_TG_LOG(str);
}

BYTE CTGCtrl::DataIsExistInOCX(ULONG nTrainIndex, ULONG data_type)
{
	BYTE ret=0;
	CTGFrame* pMainFrme = (CTGFrame*)m_pFrame;
	if(pMainFrme != NULL)
	{
		if(pMainFrme->myDataManage.DataIsExistInOCX(nTrainIndex, data_type))
		{
			ret=1;
		}
	}

	return ret;
}

CString CTGCtrl::GetTermTGTypeName()
{
	CString str;
	if(m_nTermSchdType==TERM_TG_TYPE::NOW_BASIC_TG)
		str="常用基本图";
	else if(m_nTermSchdType==TERM_TG_TYPE::READY_BASIC_TG)
		str="归档基本图";
	else if(m_nTermSchdType==TERM_TG_TYPE::PRE_BASIC_TG)
		str="预览基本图";
	else if(m_nTermSchdType==TERM_TG_TYPE::WORK_TG)
		str="调整图";
	else if(m_nTermSchdType==TERM_TG_TYPE::SHIFT_TG)
		str="日班图";
	else if(m_nTermSchdType==TERM_TG_TYPE::QUERY_TG)
		str="查询图";
	else
		str="未知";
	return str;
}

void CTGCtrl::AddBStplanStation(ULONG station)
{
	m_bplanstation.push_back(station);
}

bool CTGCtrl::IsBPlanStation(int station) const
{
	for(int i=0; i<m_bplanstation.size(); i++)
	{
		if(m_bplanstation[i]==station)
			return true;
	}
	return false;
}

void CTGCtrl::MoveTGOCXOneStep(void)
{
	CTGFrame* pMainFrme = (CTGFrame*)m_pFrame;
	if(pMainFrme != NULL)
	{
		CTGForm* form = pMainFrme->GetFormView();
		if(NULL != form)
		{
			form->MoveTGOCXOneStep();
		}
	}
}

void CTGCtrl::SetUserRight(LONG nRight)
{
	if(nRight==2 || nRight==17)
		mConfigTg.m_bCanMutiMoveOrDel=true;
	else
		mConfigTg.m_bCanMutiMoveOrDel=false;
}

void CTGCtrl::AddNotMeetInfo(WORD no_meet_flag, LPCTSTR name, LPCTSTR short_name)
{
	mConfigTg.AddNotMeetInfo(no_meet_flag, name, short_name);
}

BSTR CTGCtrl::CheckSideByRelation(const VARIANT& msg)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: 在此添加调度处理程序代码
	CString str="<root></root>";
	COleSafeArray varRcv;
	BYTE *pdata;
	varRcv=msg;
    varRcv.AccessData((LPVOID*)&pdata);
	int datalen=varRcv.GetOneDimSize();  
	if(pdata)
	{
		CLBTH::Archive lar( CLBTH::Archive::load, datalen,pdata);
		CLBTH::MSGHEAD msghead;
		msghead.length=0;
		if(!lar.eof()) 
		{
			msghead = CLBTH::BaseMsg::getMsgHead(lar);
			if(msghead.length >= CLBTH::MSGHEAD::SIZEOF && msghead.length <= lar.getCount()) 
			{
				CTGFrame* pMainFrme = (CTGFrame*)m_pFrame;
				CTGForm* form = pMainFrme->GetFormView();
				if((MSGID_MsgTrainOCX == msghead.id) && (NULL != form))
				{
					CLBTH::MsgTrainOCX  msg;
					CLBTH::HRCODE hrcode=msg.serialize(lar);
					if(hrcode.failed()) 
						;
					else
					{
						CTG_TRAIN train;
						if(TRUE==CopyTrainFromMsg(train, msg))
						{
							str=form->CheckTrainSideByRelation(train);
						}
					}
				}
			}
		}
	}

	varRcv.UnaccessData();
	
	return str.AllocSysString();
}

BSTR CTGCtrl::GetTrainJiaoRelation(ULONG nTrainIndex)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: 在此添加调度处理程序代码
	CString str="<root>";
	
	CTGFrame* pMainFrme = (CTGFrame*)m_pFrame;
	if(pMainFrme != NULL)
	{
		str+=pMainFrme->GetTrainJiaoRelation(nTrainIndex);
	}
	str+="</root>";
	
	return str.AllocSysString();
}
