// TestTgView.cpp : CTgView 类的实现
//

#include "stdafx.h"
#include "tg.h"
#include "resource.h"
#include "TgView.h"
#include "sysshare.h"
#include "if.h"
#include "function.h"
#include "tgmsg.h"
#include "td/td_frame.h"
#include "LoginDlg.h"
#include "SuperPassDlg.h"
#include ".\tgview.h"
#include "utility\mmi_function_config.h"

#include "DlgSchduleCompareStationSet.h"
#include "DlgStplanNowarningStationSet.h"
#include "DlgSideCalTrainSet.h"

#include "basic_schd_config.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
// CTgView
extern basic_schd_config* GetBasicConfig();
extern TG_STATION_STATUS *GetStationStatusPtr(int station); // defined in tg_logic_main.cpp

extern void  RegisterDataEndEvent(HWND hWnd, const CTG_ScheduleInfo & schd);
extern void  UnRegisterDataEndEvent(HWND hWnd);

BOOL UR_CanOperateBasicSchedule();
BOOL UR_CanOperateSchedule();

CLoadGPCDef m_load;
IMPLEMENT_DYNCREATE(CTgView, CView)

BEGIN_MESSAGE_MAP(CTgView, CView)
	// 标准打印命令
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_WM_TIMER()
	////// 显示控制 /////////////
	ON_COMMAND(IDC_WIDTH, OnBig)
	ON_COMMAND(IDC_SMALL, OnSmall)
	
	ON_COMMAND_RANGE(ID_DIRECT_UP, ID_DIRECT_MIXED, OnDirectRangeChg)
	ON_UPDATE_COMMAND_UI_RANGE(ID_DIRECT_UP, ID_DIRECT_MIXED, OnUpdateDirectRange)

	ON_COMMAND_RANGE(ID_SHOW_FREIGHT, ID_SHOW_ALL_TRAIN, OnSelectTrainRange)
	ON_UPDATE_COMMAND_UI_RANGE(ID_SHOW_FREIGHT, ID_SHOW_ALL_TRAIN, OnUpdateTrainRange)

	ON_COMMAND_RANGE(ID_TRAIN_LATE_PASSENGER, ID_TRAIN_LATE_ALL, OnTrainLate)
	ON_UPDATE_COMMAND_UI_RANGE(ID_TRAIN_LATE_PASSENGER, ID_TRAIN_LATE_ALL, OnUpdateTrainLate)

//	ON_COMMAND_RANGE(ID_ACCEPT_TRAIN_LATE, ID_SHOW_DEPART_MEET, OnShowReverse)
//	ON_UPDATE_COMMAND_UI_RANGE(ID_ACCEPT_TRAIN_LATE, ID_SHOW_DEPART_MEET, OnUpdateShowReverse)

	ON_COMMAND_RANGE(ID_ACCEPT_TRAIN_LATE, ID_SHOW_PLAN_NOTETEXT, OnShowReverse)
	ON_UPDATE_COMMAND_UI_RANGE(ID_ACCEPT_TRAIN_LATE, ID_SHOW_PLAN_NOTETEXT, OnUpdateShowReverse)

	ON_MESSAGE(UM_TG_OBJECT_CHANGED,OnObjectChanged)

	ON_MESSAGE(EVENT_RECIEVE_DATA_END,OnRecievedDataEnd)
	
	ON_COMMAND(ID_FIND_TRAIN, OnFindTrain)

	ON_COMMAND(ID_PLOT24, OnPlot)
	ON_COMMAND(ID_PLOT_18_06, OnPlot18_06)
	ON_COMMAND(ID_PLOT_06_18, OnPlot06_18)

	ON_COMMAND(ID_DISP_LEAP_TRAIN_NONE, OnDispLeapTrainNone)
	ON_UPDATE_COMMAND_UI(ID_DISP_LEAP_TRAIN_NONE, OnUpdateDispLeapTrainNone)

	ON_COMMAND(ID_DISP_LEAP_TRAIN_JIERUJIAOCHU, OnDispLeapTrainJierujiaochu)
	ON_UPDATE_COMMAND_UI(ID_DISP_LEAP_TRAIN_JIERUJIAOCHU, OnUpdateDispLeapTrainJierujiaochu)

	ON_COMMAND(ID_DISP_LEAP_TRAIN_LINK, OnDispLeapTrainLink)
	ON_UPDATE_COMMAND_UI(ID_DISP_LEAP_TRAIN_LINK, OnUpdateDispLeapTrainLink)

	ON_COMMAND(ID_BEGIN_18, OnDispBegin18)
	ON_COMMAND(ID_BEGIN_12, OnDispBegin12)

	ON_COMMAND(ID_WHOLE_MOVE, OnBasicTrainMoveWhole)
	ON_UPDATE_COMMAND_UI(ID_WHOLE_MOVE, OnUpdateBasicTrainMoveWhole)
	
	
	ON_COMMAND(ID_RELOAD_TG_DATA, OnRefreshTgData)

	ON_COMMAND(IDC_WHOLE_SCREEN,OnWholeScreen)
	ON_UPDATE_COMMAND_UI(IDC_WHOLE_SCREEN, OnUpdateWholeScreen)
	
	ON_COMMAND(ID_DISPLAY_DATA_XML, OnShowDataXML)

	ON_MESSAGE(WM_USER + 2001,OnSetTrainFoucs)

	ON_COMMAND(ID_SYS_SAVE_TO_FILES,OnSaveToFiles)
	ON_COMMAND(ID_SYS_LOAD_FROM_FILES,OnLoadFromFiles)

	ON_COMMAND(ID_BASIC_GRAPHSAVETOTEXT, OnBasicGraphsavetotext)

	ON_UPDATE_COMMAND_UI(ID_SAVE_SIDE_WORK,OnUpdateSaveBasicSideWorkInfo)
	ON_COMMAND(ID_SAVE_SIDE_WORK, OnSaveBasicSideWorkInfo)

	ON_COMMAND(ID_COMPARE_STATION_SET, OnPlanCompareStationSet)
	ON_COMMAND(ID_SET_STPLAN_NOWARNING_STATION, OnSetStplanNoWarningStation)
	ON_COMMAND(ID_SIDE_CAL_TRAIN_SET, OnSetSideCalTrainType)

	ON_COMMAND_RANGE(ID_ZOOM_OUT_BEGIN, ID_ZOOM_OUT_END, OnZoomOutRange)
	ON_COMMAND_RANGE(ID_ZOOM_IN_BEGIN, ID_ZOOM_IN_END, OnZoomInRange)

	ON_UPDATE_COMMAND_UI(ID_BASIC_DELETE_ALL,OnUpdateDeleteBasic)
	ON_UPDATE_COMMAND_UI(ID_DRAW_GOON,OnUpdateOperAddWorkTrain)
	ON_UPDATE_COMMAND_UI(ID_DRAW_RETURN,OnUpdateOperAddWorkTrain)
	ON_UPDATE_COMMAND_UI(ID_DRAW_ABSTRAIN_A,OnUpdateOperAddWorkTrain)
	ON_UPDATE_COMMAND_UI(ID_DRAW_ABSTRAIN_B,OnUpdateOperAddWorkTrain)

	ON_UPDATE_COMMAND_UI(ID_SEL_TDMS_TRAIN_GEN_BASIC,OnUpdateSelTdmsTrainsGenBasic)

	ON_UPDATE_COMMAND_UI(ID_BASIC_GRAPHCHANGE, OnUpdateBasicGraphchange)
	ON_UPDATE_COMMAND_UI(ID_BASIC_DELETENAME, OnUpdateBasicDeleteName)
	ON_UPDATE_COMMAND_UI(ID_BASIC_NAMEING, OnUpdateBasicNameing)
	ON_UPDATE_COMMAND_UI(ID_MODIFY_BSCHD, OnUpdateModifyBschd)
	ON_UPDATE_COMMAND_UI(ID_GEN_ALL_RETURN_TRAINID, OnUpdateGenAllReturnTrainid)
	ON_UPDATE_COMMAND_UI(ID_EDIT_ALL_CZH, OnUpdateEditAllCzh)

	ON_UPDATE_COMMAND_UI(ID_INSERT_NEW_TRAIN,OnUpdateOperTGTrain)
	ON_UPDATE_COMMAND_UI(ID_DRAW_NEW_TRAIN_END,OnUpdateOperTGTrain)
	ON_UPDATE_COMMAND_UI(IDC_DEL_MULTI_TRAIN,OnUpdateOperTGTrain)
	ON_UPDATE_COMMAND_UI(ID_LOAD_BASIC_SIDE_WORK,OnUpdateOperTGTrain)
	ON_UPDATE_COMMAND_UI(ID_BASIC_GET_TRAIN_FROM_OPMS,OnUpdateOperTGTrain)

END_MESSAGE_MAP()

BEGIN_EVENTSINK_MAP(CTgView, CView)
	ON_EVENT(CTgView, IDC_TGCTRL_VIEW, 7, TG_LOG, VTS_BSTR)
	ON_EVENT(CTgView, IDC_TGCTRL_VIEW, 8, SELECT_TRAIN, VTS_I4 VTS_I4 VTS_I2 VTS_I2 VTS_I4 VTS_I4)
	ON_EVENT(CTgView, IDC_TGCTRL_VIEW, 13, PLOT_PARAM, VTS_BSTR VTS_I4)
END_EVENTSINK_MAP()

void CTgView::TG_LOG(LPCTSTR log)
{
	if(log)
	{
		GpcLogTrainMessage(0, 0, "TG控件日志: %s", log);
	}
}
CTgView::CTgView()
{
	::CoInitialize(0);
	
	for(int i=0;i<2;i++)
	{
		m_gpcContext.mReverseFlag[i][11] = REVERSE::SHOW_REVERSE;
		m_gpcContext.mReverseFlag[i][12] = REVERSE::SHOW_ERR_WORK_SIDE; 
	}

	m_gpcContext.mLineWidth = LINE::BOLD;

	m_gpcContext.mStatsicTime=0;
	m_bCanClose=TRUE;

	m_nPlotFlag=0;
	m_bSelTDMSTrainGenBasic=false;
}

CTgView::~CTgView()
{
}

// CTgView 绘制
void CTgView::OnDraw(CDC* /*pDC*/)
{
}

// CTgView 诊断
#ifdef _DEBUG
void CTgView::AssertValid() const
{
	CView::AssertValid();
}

void CTgView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CTgDocument* CTgView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CTgDocument)));
	return (CTgDocument*)m_pDocument;
}
#endif //_DEBUG

void CTgView::OnUpdateTitle()
{
	CString cs;
	GetTitle(cs);
	GetParentFrame()->SetWindowText(cs);

	return;
}

BOOL  CTgView::InitScheduleView(CTG_ScheduleInfo::SCHEDULE_TYPE type,const char *datapath,BOOL query_mode)
{
	CWorkScheduleHolder  *pholder=GetDocument()->GetTGDataHolder();
	if(!pholder)
		return FALSE;

	CTG_ScheduleInfo  schd_info;
	pholder->GetBasicScheduleInfo(schd_info);
	
	m_schdinfo =schd_info;
	m_schdinfo.m_nType = type;// view 中的TYPE可能和DOC不一样，因为BASIC
	m_tgmmi.SetScheduleInfo(m_schdinfo);
	pholder->RegisterCommInterface(type,&m_tgmmi,true);
	m_tgmmi.SetScheduleHolder(pholder);

	m_load.Load(datapath);

	mTG.SetCurrentSectionId(schd_info.m_nSectionID);

	CString xml = m_load.GetStationXML();
	mTG.SetStationInfo(xml);
	xml = m_load.GetSectionXML();
	mTG.SetSectionInfo(xml);
	mTG.SetBackgroudTimeLength(30);

	mTG.SetLeapLineFlag(m_load.m_nLeapLineFlag);
	mTG.SetDrawTenicalStopMark(m_load.m_nDrawTenicalStop);
	mTG.SetRouteOkMarkFlag(m_load.m_nDrawRouteOkFlag);
	if(GetTGFunctionConfig()->IsFormatView())
	{
		mTG.SetFormatView();
		mTG.SetBackGroud(BACKGROUD::BLOCK::FIVE); // 总公司统一界面要求5块
	}
	else
	{	
		mTG.SetBackGroud(m_load.m_backgroudNum);
	}
	if(GetTGFunctionConfig()->IsEnableCzh())
		mTG.SetTrainDisplayFlag(TRAIN_DISPLY::TRAIN_CZH, 1);
	else
		mTG.SetTrainDisplayFlag(TRAIN_DISPLY::TRAIN_CZH, 0);
	mTG.SetTrainDisplayFlag(TRAIN_DISPLY::SET_MOVE_SAME_CZ_TRAIN, 0);
	
	if(GetTGFunctionConfig()->IsEnableNewCzh())
		mTG.SetTrainDisplayFlag(TRAIN_DISPLY::TRAIN_NEW_CZH, 1);
	else
		mTG.SetTrainDisplayFlag(TRAIN_DISPLY::TRAIN_NEW_CZH, 0);

	if(GetTGFunctionConfig()->IsChangeTrainID())
		mTG.SetTrainDisplayFlag(TRAIN_DISPLY::SET_TRAIN_RETURN_ID, 1);
	else
		mTG.SetTrainDisplayFlag(TRAIN_DISPLY::SET_TRAIN_RETURN_ID, 0);

	if(gpcdef.GetTrainNoLevelCount()>3)
		mTG.SetTrainDisplayFlag(TRAIN_DISPLY::SHOW_TRAIN_NO_LEVEL_COUNT, gpcdef.GetTrainNoLevelCount());

	int list[256];
	int count=gpcdef.GetSectionSwitchStation(schd_info.m_nSectionID, list);
	for(int i=0; i<count; i++)
		mTG.SetTrainDisplayFlag(TRAIN_DISPLY::SWITCH_STATION, list[i]);

	count=gpcdef.GetNotShowSideCtcStation(schd_info.m_nSectionID, list);
	for(int i=0; i<count; i++)
		mTG.SetTrainDisplayFlag(TRAIN_DISPLY::NOT_SHOW_SIDE_CTC_STATION, list[i]);

	count=gpcdef.GetErrorSideStationList(schd_info.m_nSectionID, list);
	for(int i=0; i<count; i++)
		mTG.SetTrainDisplayFlag(TRAIN_DISPLY::ERROR_SIDE_STATION, list[i]);

	// 默认每分钟像素数
	mTG.SetTrainDisplayFlag(TRAIN_DISPLY::SET_PIXEL_PER_MINUTE, 4);
	if(CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC==type)
		mTG.SetTgType(TERM_TG_TYPE::NOW_BASIC_TG);
	else if(CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_PREV==type)
		mTG.SetTgType(TERM_TG_TYPE::PRE_BASIC_TG);
	else if(CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_BACKUP==type)
		mTG.SetTgType(TERM_TG_TYPE::READY_BASIC_TG);
	else
		return FALSE;
	
	int nRight=gpcdef.GetUserIdentify();
	mTG.SetGpcUserRight(nRight); // 设置用户权限,限制助调多车移动,删除多趟车
	GpcLogSysMessage(0,"根据配置设置用户权限:%d", nRight);
	mTG.SetShiftId(schd_info.m_nShiftID,0);
	
	if(m_gpcContext.mPixelPerMinute != 0)
	{
		mTG.SetTrainDisplayFlag(TRAIN_DISPLY::SET_PIXEL_PER_MINUTE, m_gpcContext.mPixelPerMinute);
	}
	mTG.TgStart(datapath);
	
	RIGHT right;
	int gpcid = gpcdef.GetUserGpcId();
	if(!m_load.GetUserRight(gpcid, right))
	{
		return FALSE;
	}
	mTG.SetWinTGOperateRight(right.wintg_operate);

	if(gpcdef.IsShowVirtualStation())
		mTG.SetTrainDisplayFlag(TRAIN_DISPLY::REVERSE,  REVERSE::SHOW_VIRTUAL_STATION);

	if (gpcdef.IsShowOPMSTransInfoButton())
		mTG.SetTrainDisplayFlag(TRAIN_DISPLY::REVERSE, REVERSE::SHOW_TRANS_INFO);

	if (GetTGFunctionConfig()->IsUseStringKiloMeter())
		mTG.SetTrainDisplayFlag(TRAIN_DISPLY::REVERSE,  REVERSE::USE_STRING_KILO_METER);

	if(IS_BASIC_SCHD(type))
	{
		SetTitleUpdateTimer();
		if(GetMMIFunctionConfig()->GetConfigIntegerItem(CONFIG_SHOW_ELEC_ENGINE_NO)==1)
		{
			m_gpcContext.mReverseFlag[0][6] = REVERSE::SHOW_ENGINE;
		}
		if(GetMMIFunctionConfig()->GetConfigIntegerItem(CONFIG_NO_SHOW_REVERSE)==1)
		{
			m_gpcContext.mReverseFlag[0][11] = 0;
		}
		if(GetMMIFunctionConfig()->GetConfigIntegerItem(CONFIG_SHOW_LOCONO)==1)
		{
			m_gpcContext.mReverseFlag[0][13] = REVERSE::SHOW_ELEC_LOCO;
		}
						
		// 反向行车
		//mTG.SetTrainDisplayFlag(TRAIN_DISPLY::REVERSE, m_gpcContext.mReverseFlag[0][11]);
		// 显示错误股道
		//mTG.SetTrainDisplayFlag(TRAIN_DISPLY::REVERSE, m_gpcContext.mReverseFlag[0][12]);
		// 显示行车股道
		m_gpcContext.mReverseFlag[0][16]=0;
		//mTG.SetTrainDisplayFlag(TRAIN_DISPLY::REVERSE, m_gpcContext.mReverseFlag[0][16]);

 		for(int i=0; i<16; i++)
 		{
 			mTG.SetTrainDisplayFlag(TRAIN_DISPLY::REVERSE, m_gpcContext.mReverseFlag[0][i]);
 		}
	}
	else
	{
		if(GetMMIFunctionConfig()->GetConfigIntegerItem(CONFIG_CHECK_MANUAL_OPER)==0)
		{
			GetTGFunctionConfig()->SetCheckManualOper(FALSE);
		}
		if(GetMMIFunctionConfig()->GetConfigIntegerItem(CONFIG_SHOW_ELEC_ENGINE_NO)==1)
		{
			m_gpcContext.mReverseFlag[1][6] = REVERSE::SHOW_ENGINE;
		}
		if(GetMMIFunctionConfig()->GetConfigIntegerItem(CONFIG_NO_SHOW_REVERSE)==1)
		{
			m_gpcContext.mReverseFlag[1][11] = 0;
		}
		
		if(GetMMIFunctionConfig()->GetConfigIntegerItem(CONFIG_SHOW_LOCONO)==1)
		{
			m_gpcContext.mReverseFlag[1][13] = REVERSE::SHOW_ELEC_LOCO;
		}
						
	    m_gpcContext.mReverseFlag[1][15] = 0;

		int nFlag = GetMMIFunctionConfig()->GetConfigIntegerItem(CONFIG_SHOW_EARLY_LATE_TIME);
		if(nFlag==TYPE::PASSENGER || nFlag == TYPE::FREIGHT || nFlag == TYPE::ALL)
		{
			m_gpcContext.mStatsicTime=nFlag;
		}
		mTG.SetTrainDisplayFlag(TRAIN_DISPLY::LATETRAIN, m_gpcContext.mStatsicTime);
	
		for(int i=0; i<16; i++)
		{
			mTG.SetTrainDisplayFlag(TRAIN_DISPLY::REVERSE, m_gpcContext.mReverseFlag[1][i]);
		}
	}

	int nFlag = GetMMIFunctionConfig()->GetConfigIntegerItem(CONFIG_SHOW_THIN_LINE);
	if(nFlag == LINE::BOLD || nFlag == LINE::THIN)
	{
		m_gpcContext.mLineWidth = nFlag;
	}
	mTG.SetTrainDisplayFlag(TRAIN_DISPLY::LINE, m_gpcContext.mLineWidth);
	
	// 默认选中钩连线
	if(GetMMIFunctionConfig()->GetConfigIntegerItem(CONFIG_SHOW_CZH_LINE)==1)
	{
		mTG.SetTrainDisplayFlag(TRAIN_DISPLY::REVERSE, REVERSE::SHOW_SEL_CZH_LINE);
	}
	// 内燃机车画绿线
	if(GetTGFunctionConfig()->IsDieselLocomotiveDrawGreen(schd_info.m_nSectionID))
		mTG.SetTrainDisplayFlag(TRAIN_DISPLY::GREEN_DECORATE_LINE, GREEN_DECORATE_LINE::DIESEL_LOCOMOTIVE);
	else
		mTG.SetTrainDisplayFlag(TRAIN_DISPLY::GREEN_DECORATE_LINE, GREEN_DECORATE_LINE::ELEC_LOCOMOTIVE);

	int limitMin=gpcdef.GetManualConvertActualTimeLength();
	mTG.SetTrainDisplayFlag(TRAIN_DISPLY::RECIEVE_POINT_LIMIT_TIME, limitMin);
	GpcLogSysMessage(0,"根据配置设置TGOCX允许修改实际点时间长度:%d", limitMin);
	mTG.SetTrainDisplayFlag(TRAIN_DISPLY::SET_LIMIT_MODIFY_ACT_TIME, 1);
	
	// 旅客列车非技术停点默认锁定股道
	if(GetTGFunctionConfig()->SetPassengerTrainNoTechStopLockSide())
		mTG.SetTrainDisplayFlag(TRAIN_DISPLY::LOCK_SIDE_NOT_TECH_STOP, 1);
	else
		mTG.SetTrainDisplayFlag(TRAIN_DISPLY::LOCK_SIDE_NOT_TECH_STOP, 0);

	CString cs;
	GetTitle(cs);
	GetParentFrame()->SetWindowText(cs);
	WORD para=0;
	if(query_mode || GetTGFunctionConfig()->IsDisenableSheetTab())
		para|=0x02;
	if(right.operate!=3)
		para|=0x02;
	if(IsAssistantModifyPropTreeList(right.wintg_operate))
		gpcdef.EnableAssistantModifyPropTree(TRUE);
	GetParentFrame()->SendMessage(UM_TG_PROPERTY_UPDATE,0, para); // set right para:0x02只读

	for(int i=0;i<GetDocument()->m_nStationCount;i++)
	{
		int station=GetDocument()->m_aStationList[i];
		TG_STATION_STATUS *pstat=GetStationStatusPtr(station);
		if(!pstat)
			continue;
		if(pstat->stpc_status==  TG_STATION_STATUS::ENTITY_ACTIVE) 
			mTG.SetStationConnectStatus(station,0);
		else
			mTG.SetStationConnectStatus(station,1);
		mTG.SetPlanRecieveFlag(1,station,(ULONG)RGB(255,255,255));
		mTG.SetPlanRecieveFlag(2,station,(ULONG)RGB(255,255,255));
	}
	
	CString strUrl=GetTGFunctionConfig()->GetGSoapURL();
	if(strUrl!="")
	{
		mTG.SetTrainDisplayFlag(TRAIN_DISPLY::REVERSE, REVERSE::SHOW_ENGINE_CREW_INFO);
	}
	RegisterDataEndEvent(this->m_hWnd, m_schdinfo);

	// 预览图, 默认不从服务器取数据,需要单独申请
	if(GetViewScheduleType()==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_PREV)  
	{
		GetDocument()->GetTGDataHolder()->ReloadData(GetViewScheduleType(),TRUE);
	}
	// 归档图，默认不从服务器取数据, 需要单独申请
	if(GetViewScheduleType()==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_BACKUP)  
	{
		GetDocument()->GetTGDataHolder()->ReloadData(GetViewScheduleType(),TRUE);
	}
	RefreshViewData();
	if(GetTGFunctionConfig()->IsEraseBeforeModifyTrain())
		mTG.SetTrainDisplayFlag(TRAIN_DISPLY::ERASE_BEFORE_MODIFY, 1);
	else
		mTG.SetTrainDisplayFlag(TRAIN_DISPLY::ERASE_BEFORE_MODIFY, 0);
	
	if(GetTGFunctionConfig()->IsDisableModifyPlanTm())
		mTG.SetTrainDisplayFlag(TRAIN_DISPLY::DISABLE_MODIFY_PLAN_TM, 1);
	else
		mTG.SetTrainDisplayFlag(TRAIN_DISPLY::DISABLE_MODIFY_PLAN_TM, 0);

	int nCount=gpcdef.getNotMeetInfoCount();
	for(int n=0; n<nCount; n++)
	{
		NotMeetInfo info;
		if(!gpcdef.getNotMeetInfoByNo(n, info))
		{
			break;
		}
		mTG.AddNotMeetInfo(info.nNoMeetFlag, info.sName, info.sShortName);
	}
	return TRUE;
}

const char *CTgView::GetCommStatusInfo()
{
	static int nChangeFlag = 0;
	static int nStatus=-1;

	static char g_comm_status_string[128];  // status bar will read this global string! 
	CWorkScheduleHolder  *pholder=GetDocument()->GetTGDataHolder();
	if(!pholder)
		return NULL;

	nChangeFlag=0;

	IScheduleComm *pcomm=pholder->GetServerCommByScheduleType(GetViewScheduleType());
	if(pcomm && pcomm->CommReady())
	{
		sprintf(g_comm_status_string,"服务连接");
		if(nStatus == 0 || nStatus == -1)
			nChangeFlag=1;
		nStatus=1;
	}
	else
	{
		sprintf(g_comm_status_string,"服务中断");
		if(nStatus == 1 || nStatus == -1)
			nChangeFlag=1;
		nStatus=0;
	}

	if(nChangeFlag)
	{
		CString cs;
		GetTitle(cs);
		GpcLogTrainMessage(0, 0, "%s:%s", cs, g_comm_status_string);
	}

	return g_comm_status_string;
}	

const char *CTgView::GetSyncStatusInfo()
{
	static int nChangeFlag=0;
	static int nserver_nosync_count=0;
	static int nmmi_nosync_count=0;

	static char g_sync_status_string[128];  // status bar will read this global string! 
	CWorkScheduleHolder  *pholder=GetDocument()->GetTGDataHolder();
	if(!pholder)
		return NULL;

	CString strServerNoSyncedInfo="";
	CString strMmiNoSyncedInfo="";

	mServer_nosync_count=pholder->GetServerNoSyncedObjectCount(m_schdinfo.m_nType,strServerNoSyncedInfo);
	mMmi_nosync_count=pholder->GetNoSyncedObjectCount(m_schdinfo.m_nType,&m_tgmmi,strMmiNoSyncedInfo);
	sprintf(g_sync_status_string,"未同步计数:服务器:%d 界面:%d",
		mServer_nosync_count,
		mMmi_nosync_count);
	
	if(nserver_nosync_count != mServer_nosync_count || nmmi_nosync_count != mMmi_nosync_count)
	{
		nChangeFlag=1;
		nserver_nosync_count = mServer_nosync_count;
		nmmi_nosync_count = mMmi_nosync_count;
	}
	else
	{
		nChangeFlag=0;
	}
	if((mServer_nosync_count>0 || mMmi_nosync_count>0) && nChangeFlag)
	{
		CString cs;
		GetTitle(cs);
		//GpcLogTrainMessage(0, 0, "%s %s %s", cs, GetCommStatusInfo(), g_sync_status_string);
		GpcLogTrainMessage(0, 0, "%s %s 未同步计数:服务器:%d(%s) 本地:%d 界面:%d(%s)",
			cs, GetCommStatusInfo(),mServer_nosync_count,strServerNoSyncedInfo,mMmi_nosync_count,strMmiNoSyncedInfo);
	}
	return g_sync_status_string;
}

void CTgView::OnTimer(UINT event)
{
	if(event == 990)
	{
		m_timeCount++;
		OnUpdateTitle();
		if(m_schdinfo.m_nType!=CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC || m_timeCount>30)
			KillTimer(990);
	}
	else if(995 ==event)
	{
		CWorkScheduleHolder *pholder=GetDocument()->GetTGDataHolder();
		if(NULL!=pholder)
		{
			CString str;
			if(0==pholder->GetNoSyncedObjectCount(m_schdinfo.m_nType,&m_tgmmi,str))
			{
				KillTimer(995);
				GpcLogSysMessage(0,"[下载运行图] 界面同步计数完成,停止定时器,触发批量画线");
				mTG.EndBatchDraw(0, "[下载运行图] 从TDMS接收基本图或者从CTC服务器下载运行线完毕");
				GpcLogSysMessage(0,"[下载运行图] 完成批量画线");
			}
		}
	}

	if(996 ==event)
	{
		m_tgmmi.OnClearTimeoutOper();
	}
	
	CView::OnTimer(event);
}

// 时间响应
void CTgView::SELECT_TRAIN(long train_index, long nFlag,short stano1, short stano2, long  date, long time2)
{
	TIME curtm=time2;
	if(train_index!=NULL_TRAIN_INDEX)
	{
		CTG_TrainScheduleBase *pData=m_tgmmi.GetDataStore(m_schdinfo);
		if(pData)
		{
			CTG_SingleTrain *ptrain=pData->GetSingleTrain(train_index);
			int  st=0;
			if(stano1==stano2)
				st=stano1;
			if(ptrain)
			{
				int rec_index=date;
				GetDocument()->SetCurrentActiveObject(GetViewScheduleType(),ptrain,st,rec_index);
				GetParentFrame()->SendMessage(UM_TG_PROPERTY_UPDATE, 1, 1);
				GetParentFrame()->SendMessage(UM_TG_PROPERTY_UPDATE,(WPARAM)ptrain, rec_index); 
			}
		}
	}
	else
	{
		GetDocument()->SetCurrentActiveObject(GetViewScheduleType(), NULL,-1,-1);
		GetParentFrame()->SendMessage(UM_TG_PROPERTY_UPDATE, 1, 0); 
	}
}

LRESULT CTgView::OnRecievedDataEnd(WPARAM wp,LPARAM lp)
{
	if(1==(int)lp)
	{
		mTG.BeginBatchDraw(1, "[下载运行图] 开始从服务器下载运行图数据"); 
	}
	else
	{
		GpcLogSysMessage(0,"[下载运行图] 设置界面未同步检查定时器");
		SetTimer(995,200,NULL);
	}
	return 0;
}

extern void  RemoveMarkMemoryNotify(DWORD markindex);
extern DWORD  AddMarkToMemoryNotify(const CTG_SingleMark *pmark);

LRESULT CTgView::OnObjectChanged(WPARAM wp,LPARAM lp)
{
	CTG_TrainScheduleBase *pData=m_tgmmi.GetDataStore(m_schdinfo);
	if(!pData)
		return 0;
	TG_OBJECT_TYPE  objtype;
	TRAIN_INDEX train_index;
	CTG_ScheduleInfo::SCHEDULE_TYPE schdtype;
	GetDocument()->GetCurrentActiveObject(schdtype,objtype,train_index);
	if(objtype!=wp || schdtype!=GetViewScheduleType() || train_index!=(TRAIN_INDEX)lp)
		return 0;
	
	int st=0, rec_index=0;
	GetDocument()->GetCurrentActiveObject(&st, &rec_index);
	if(wp==TG_OBJ_TRAIN)
	{
		CTG_SingleTrain *pobj=pData->GetSingleTrain((TRAIN_INDEX)lp);
		if(pobj)
		{
			if(pobj->GetTrainRecordCount()>0)
				GetParentFrame()->SendMessage(UM_TG_PROPERTY_UPDATE,(WPARAM)pobj, rec_index);
			else
				GetParentFrame()->SendMessage(UM_TG_PROPERTY_UPDATE, 0, 0x7FFF); // 删除
		}
		else
		{
			GetParentFrame()->SendMessage(UM_TG_PROPERTY_UPDATE, 0, 0x7FFF); // 删除
		}
	}
	else if(wp==TG_OBJ_MARK)
	{
		CTG_SingleMark *pobj=pData->GetSingleMark((TRAIN_INDEX)lp);
		if(pobj)
		{
			GetParentFrame()->SendMessage(UM_TG_PROPERTY_UPDATE,(WPARAM)pobj,0);
		}
		else
		{
			GetParentFrame()->SendMessage(UM_TG_PROPERTY_UPDATE, 0, 0x7FFF);
		}
		
		if(pobj)
		{
			if(!GetDocument()->IsQueryMode()) 
				AddMarkToMemoryNotify(pobj);
		}
		else
		{
			if(!GetDocument()->IsQueryMode()) 
				RemoveMarkMemoryNotify((TRAIN_INDEX)lp);
		}
	}
	return 0;
}

// CTgView 消息处理程序
void CTgView::OnShowDataXML()
{
	CTG_LogicObject *pobj=GetDocument()->GetCurrentActiveObject();
	if(!pobj)
		return;

	if(pobj->GetObjectType()==TG_OBJ_TRAIN || pobj->GetObjectType()==TG_OBJ_MARK)
	{
		CDlgDataXMLShow dlg(pobj, this);
		dlg.DoModal();
	}
}

int CTgView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	if(!mTG.Create(NULL, WS_CHILD | WS_VISIBLE, CRect(0,-1,100,100), this, IDC_TGCTRL_VIEW, NULL))
		return -1;

	m_tgmmi.SetTGCtrl(&mTG); 
	m_tgmmi.SetParentWnd(this);
	
	return 0;
}


void CTgView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	mTG.MoveWindow(0,0, cx, cy);
}
BOOL CTgView::CloseView() 
{
	if(!m_bCanClose)
		return FALSE;

	KillTimer(990);
	mTG.AliveWindow(4); // 关闭TGOCX
	Sleep(500);
	UnRegisterDataEndEvent(this->m_hWnd);
	CWorkScheduleHolder  *pholder=GetDocument()->GetTGDataHolder();
	if(pholder)
		pholder->UnRegisterCommInterface(&m_tgmmi);

	return TRUE;
}
void CTgView::OnDestroy() 
{
	CView::OnDestroy();
}

void CTgView::OnInitialUpdate()
{
	CView::OnInitialUpdate();
	SetTimer(996,30000,NULL);
}

void CTgView::RefreshViewData()
{
	mTG.BeginBatchDraw(0, "开始将Term内存中运行图转递到OCX"); 
	CWorkScheduleHolder  *pholder=GetDocument()->GetTGDataHolder();

	if(pholder->WriteSchedule(m_schdinfo.m_nType ,&m_tgmmi,TRUE))
	{
		m_tgmmi.SetCommReady(TRUE); // 允许更新
	}
	mTG.EndBatchDraw(0,"将Term内存中运行图转递到OCX完毕"); // 
}

void CTgView::OnSavePicture()
{
	// TODO: 在此添加命令处理程序代码
	mTG.SaveToPicture("");
}


void CTgView::GetTitle(CString &title)
{
	CString sdate=m_schdinfo.GetShiftName();
	
	CString temp;
	title=(LPCTSTR)gpcdef.GetSectionName(m_schdinfo.m_nSectionID);
	temp="]["+sdate;

	CString basicName=GetBasicConfig()->GetBasicNameByType(m_schdinfo.m_nSectionID, m_schdinfo.m_nType);
	CString strtm=GetBasicConfig()->GetBasicNameTime(m_schdinfo.m_nSectionID, m_schdinfo.m_nType);
	
	if(m_schdinfo.m_nType ==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC )
	{
		title += "--常用基本图";
		if(!basicName.IsEmpty())
		{
			title += "("+basicName+")";
		}
	}
	else if(m_schdinfo.m_nType ==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_PREV)
	{
		title += "--预览基本图";
		if(!basicName.IsEmpty())
		{
			title += "("+basicName+")";
		}
		if(gpcdef.IsEnableBasicDumpTime())
		{
			title += strtm;
		}
	}
	else if(m_schdinfo.m_nType ==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_BACKUP)
	{
		title += "--归档基本图";
		if(!basicName.IsEmpty())
		{
			title += "("+basicName+")";
		}
	}
}

void CTgView::OnDirectRangeChg(UINT nID)
{
	short nShowDirectFlag=0;
	if(nID==ID_DIRECT_UP)
		nShowDirectFlag=DIRECT::UP;  
	else if(nID==ID_DIRECT_DOWN)
		nShowDirectFlag=DIRECT::DOWN;  
	else
		nShowDirectFlag=DIRECT::DUAL; 
	mTG.SetTrainDisplayFlag(TRAIN_DISPLY::DIRECT,nShowDirectFlag); 
}

void CTgView::OnUpdateDirectRange(CCmdUI* pCmdUI)
{
	int nShowDirectFlag=mTG.GetTrainDisplayFlag(TRAIN_DISPLY::DIRECT,0);
	int nID=pCmdUI->m_nID; 
	if(nID==ID_DIRECT_UP)
	{
		pCmdUI->SetCheck(nShowDirectFlag==DIRECT::UP);  
		pCmdUI->Enable(nShowDirectFlag!=DIRECT::UP);  
	}
	else if(nID==ID_DIRECT_DOWN)
	{
		pCmdUI->SetCheck(nShowDirectFlag==DIRECT::DOWN);  
		pCmdUI->Enable(nShowDirectFlag!=DIRECT::DOWN);  
	}
	else
	{
		pCmdUI->SetCheck(nShowDirectFlag==DIRECT::DUAL);  
		pCmdUI->Enable(nShowDirectFlag!=DIRECT::DUAL);  
	}
}

void CTgView::OnSelectTrainRange(UINT nID)
{
	short nShowTrainFlag=0;
	if(nID==ID_SHOW_FREIGHT)
		nShowTrainFlag = TYPE::PASSENGER;	
	else if(nID==ID_SHOW_PASSENGER)
		nShowTrainFlag =TYPE::FREIGHT;  
	else
		nShowTrainFlag = TYPE::ALL; 

	mTG.SetTrainDisplayFlag(TRAIN_DISPLY::TYPE,nShowTrainFlag); 
}

void CTgView::OnUpdateTrainRange(CCmdUI* pCmdUI)
{
	int nShowTrainFlag=mTG.GetTrainDisplayFlag(TRAIN_DISPLY::TYPE,0);
	int nID=pCmdUI->m_nID;
	if(nID==ID_SHOW_PASSENGER)
	{
		pCmdUI->SetCheck(nShowTrainFlag==TYPE::FREIGHT);  
		pCmdUI->Enable(nShowTrainFlag!=TYPE::FREIGHT);  
	}
	else if(nID==ID_SHOW_FREIGHT)
	{
		pCmdUI->SetCheck(nShowTrainFlag==TYPE::PASSENGER);  
		pCmdUI->Enable(nShowTrainFlag!=TYPE::PASSENGER);  
	}
	else
	{
		pCmdUI->SetCheck(nShowTrainFlag==TYPE::ALL);  
		pCmdUI->Enable(nShowTrainFlag!=TYPE::ALL);  
	}
}

void CTgView::OnTrainLate(UINT nID)
{
	if(nID==ID_TRAIN_LATE_PASSENGER)
	{
		if(m_gpcContext.mStatsicTime == TYPE::PASSENGER)
			m_gpcContext.mStatsicTime = 0;
		else
			m_gpcContext.mStatsicTime = TYPE::PASSENGER;
	}
	else if(nID==ID_TRAIN_LATE_FREIGHT)
	{
		if(m_gpcContext.mStatsicTime == TYPE::FREIGHT)
			m_gpcContext.mStatsicTime = 0;
		else
			m_gpcContext.mStatsicTime = TYPE::FREIGHT;
	}
	else
	{
		if(m_gpcContext.mStatsicTime == TYPE::ALL)
			m_gpcContext.mStatsicTime = 0;
		else
			m_gpcContext.mStatsicTime = TYPE::ALL;
	}

	mTG.SetTrainDisplayFlag(TRAIN_DISPLY::LATETRAIN, m_gpcContext.mStatsicTime);
}

void CTgView::OnUpdateTrainLate(CCmdUI* pCmdUI)
{
	int nID=pCmdUI->m_nID;
	if(nID==ID_TRAIN_LATE_PASSENGER)
	{
		pCmdUI->SetCheck(m_gpcContext.mStatsicTime==TYPE::PASSENGER);  
	}
	else if(nID==ID_TRAIN_LATE_FREIGHT)
	{
		pCmdUI->SetCheck(m_gpcContext.mStatsicTime==TYPE::FREIGHT);  
	}
	else
	{
		pCmdUI->SetCheck(m_gpcContext.mStatsicTime==TYPE::ALL);  
	}
}

void CTgView::OnShowReverse(UINT nID)
{
	int ScheduleType=-1;
	CTG_ScheduleInfo::SCHEDULE_TYPE type;
	type = GetViewScheduleType();
	if (IS_BASIC_SCHD(type))
		ScheduleType=0;
	else
		ScheduleType=1;

	if (ScheduleType<0)
		return;

	if(nID==ID_ACCEPT_TRAIN_LATE)
	{
		if(m_gpcContext.mReverseFlag[ScheduleType][0] == REVERSE::ACCEPT_TRAIN_LATE)
			m_gpcContext.mReverseFlag[ScheduleType][0] = 0;  
		else
			m_gpcContext.mReverseFlag[ScheduleType][0] = REVERSE::ACCEPT_TRAIN_LATE;

		mTG.SetTrainDisplayFlag(TRAIN_DISPLY::REVERSE, REVERSE::ACCEPT_TRAIN_LATE); 
	}
	else if(nID==ID_ACCEPT_TRUCK_LATE)
	{
		if(m_gpcContext.mReverseFlag[ScheduleType][1] == REVERSE::ACCEPT_TRUCK_LATE)
			m_gpcContext.mReverseFlag[ScheduleType][1] = 0; 
		else
			m_gpcContext.mReverseFlag[ScheduleType][1] = REVERSE::ACCEPT_TRUCK_LATE; 

		mTG.SetTrainDisplayFlag(TRAIN_DISPLY::REVERSE, REVERSE::ACCEPT_TRUCK_LATE);
	}	
	else if(nID==IDC_BYPASS_FLAG)
	{
		if(m_gpcContext.mReverseFlag[ScheduleType][2] == REVERSE::BY_PASS_FLAG)
			m_gpcContext.mReverseFlag[ScheduleType][2] = 0; 
		else
			m_gpcContext.mReverseFlag[ScheduleType][2] = REVERSE::BY_PASS_FLAG; 

		mTG.SetTrainDisplayFlag(TRAIN_DISPLY::REVERSE, REVERSE::BY_PASS_FLAG);
	}	
	else if(nID==ID_SHOW_WAVE_LINE)
	{
		if(m_gpcContext.mReverseFlag[ScheduleType][3] == REVERSE::SHOW_WAVE)
			m_gpcContext.mReverseFlag[ScheduleType][3] = 0; 
		else
			m_gpcContext.mReverseFlag[ScheduleType][3] = REVERSE::SHOW_WAVE; 

		mTG.SetTrainDisplayFlag(TRAIN_DISPLY::REVERSE, REVERSE::SHOW_WAVE);
	}	
	else if(nID==ID_SHOW_ERROR_TAG)
	{
		if(m_gpcContext.mReverseFlag[ScheduleType][4] == REVERSE::SHOW_ERROR)
			m_gpcContext.mReverseFlag[ScheduleType][4] = 0; 
		else
			m_gpcContext.mReverseFlag[ScheduleType][4] = REVERSE::SHOW_ERROR; 

		mTG.SetTrainDisplayFlag(TRAIN_DISPLY::REVERSE, REVERSE::SHOW_ERROR);
	}	
	/*
	else if(nID==ID_SHOW_NOT_ADJUST_ERROR)
	{
		if(m_gpcContext.mReverseFlag[ScheduleType][5] == REVERSE::SHOW_ADJUST_ERROR)
			m_gpcContext.mReverseFlag[ScheduleType][5] = 0; 
		else
			m_gpcContext.mReverseFlag[ScheduleType][5] = REVERSE::SHOW_ADJUST_ERROR; 

		mTG.SetTrainDisplayFlag(TRAIN_DISPLY::REVERSE, REVERSE::SHOW_ADJUST_ERROR);
	}*/	
	else if(nID==ID_SHOW_TRACTIONENGINE)
	{
		if(m_gpcContext.mReverseFlag[ScheduleType][6] == REVERSE::SHOW_ENGINE)
			m_gpcContext.mReverseFlag[ScheduleType][6] = 0; 
		else
			m_gpcContext.mReverseFlag[ScheduleType][6] = REVERSE::SHOW_ENGINE; 

		mTG.SetTrainDisplayFlag(TRAIN_DISPLY::REVERSE, REVERSE::SHOW_ENGINE);
	}	
	else if(nID==ID_SHOW_EARLYLATE_TIME)
	{
		if(m_gpcContext.mReverseFlag[ScheduleType][7] == REVERSE::SHOW_EARLY_LATE_TIME)
			m_gpcContext.mReverseFlag[ScheduleType][7] = 0; 
		else
			m_gpcContext.mReverseFlag[ScheduleType][7] = REVERSE::SHOW_EARLY_LATE_TIME; 

		mTG.SetTrainDisplayFlag(TRAIN_DISPLY::REVERSE, REVERSE::SHOW_EARLY_LATE_TIME);
	}	
	else if(nID==ID_SHOW_TIME)
	{
		if(m_gpcContext.mReverseFlag[ScheduleType][8] == REVERSE::SHOW_FAST_SLOW_TIME)
			m_gpcContext.mReverseFlag[ScheduleType][8] = 0; 
		else
			m_gpcContext.mReverseFlag[ScheduleType][8] = REVERSE::SHOW_FAST_SLOW_TIME; 

		mTG.SetTrainDisplayFlag(TRAIN_DISPLY::REVERSE, REVERSE::SHOW_FAST_SLOW_TIME);
	}	
	else if(nID==IDC_SHOW_DISTANCE_STONE)
	{
		if(m_gpcContext.mReverseFlag[ScheduleType][9] == REVERSE::SHOW_KILO_MARK)
			m_gpcContext.mReverseFlag[ScheduleType][9] = 0; 
		else
			m_gpcContext.mReverseFlag[ScheduleType][9] = REVERSE::SHOW_KILO_MARK; 

		mTG.SetTrainDisplayFlag(TRAIN_DISPLY::REVERSE, REVERSE::SHOW_KILO_MARK);
	}	
	else if(nID==ID_SHOW_CONFLICT)
	{
		if(m_gpcContext.mReverseFlag[ScheduleType][10] == REVERSE::SHOW_CONFLICT)
			m_gpcContext.mReverseFlag[ScheduleType][10] = 0; 
		else
			m_gpcContext.mReverseFlag[ScheduleType][10] = REVERSE::SHOW_CONFLICT; 
		
		mTG.SetTrainDisplayFlag(TRAIN_DISPLY::REVERSE, REVERSE::SHOW_CONFLICT);
	}	
	else if(nID==ID_SHOW_REVERSE)
	{
		if(m_gpcContext.mReverseFlag[ScheduleType][11] == REVERSE::SHOW_REVERSE)
			m_gpcContext.mReverseFlag[ScheduleType][11] = 0; 
		else
			m_gpcContext.mReverseFlag[ScheduleType][11] = REVERSE::SHOW_REVERSE; 

		mTG.SetTrainDisplayFlag(TRAIN_DISPLY::REVERSE, REVERSE::SHOW_REVERSE);
	}	
	else if(nID==ID_SHOW_WORKSIDE_ERR)
	{
		if(m_gpcContext.mReverseFlag[ScheduleType][12] == REVERSE::SHOW_ERR_WORK_SIDE)
			m_gpcContext.mReverseFlag[ScheduleType][12] = 0; 
		else
			m_gpcContext.mReverseFlag[ScheduleType][12] = REVERSE::SHOW_ERR_WORK_SIDE; 

		mTG.SetTrainDisplayFlag(TRAIN_DISPLY::REVERSE, REVERSE::SHOW_ERR_WORK_SIDE);
	}	
	else if(nID==ID_SHOW_ELECLOCOMOTIVE)
	{
		if(m_gpcContext.mReverseFlag[ScheduleType][13] == REVERSE::SHOW_ELEC_LOCO)
			m_gpcContext.mReverseFlag[ScheduleType][13] = 0; 
		else
			m_gpcContext.mReverseFlag[ScheduleType][13] = REVERSE::SHOW_ELEC_LOCO; 

		mTG.SetTrainDisplayFlag(TRAIN_DISPLY::REVERSE, REVERSE::SHOW_ELEC_LOCO);
	}
	else if(nID==ID_THIN_LINE)
	{
		if(m_gpcContext.mLineWidth == LINE::THIN)
			m_gpcContext.mLineWidth = LINE::BOLD; 
		else
			m_gpcContext.mLineWidth = LINE::THIN; 

		mTG.SetTrainDisplayFlag(TRAIN_DISPLY::LINE, m_gpcContext.mLineWidth);
	}
	else if(nID==ID_SHOW_YINGYE_INFO)
	{
		mTG.SetTrainDisplayFlag(TRAIN_DISPLY::REVERSE, REVERSE::SHOW_TECH_STOP_MARK);
	}
	else if(nID==ID_SHOW_ROUTE_TRIGER_OK)
	{
		mTG.SetTrainDisplayFlag(TRAIN_DISPLY::REVERSE, REVERSE::SHOW_ROUTE_TRIGER_OK_FLAG);
	}
	else if(nID==ID_SHOW_ROUTE_NOTAUTO_TRIGER)
	{
		mTG.SetTrainDisplayFlag(TRAIN_DISPLY::REVERSE, REVERSE::SHOW_ROUTE_NOT_AUTO_TRIGER_ROUND);
	}
	else if(nID==ID_SHOW_SEL_CZH_LINE)
	{
		mTG.SetTrainDisplayFlag(TRAIN_DISPLY::REVERSE, REVERSE::SHOW_SEL_CZH_LINE);
	}
	else if(nID==ID_SHOW_WORKSIDE)
	{
		mTG.SetTrainDisplayFlag(TRAIN_DISPLY::REVERSE, REVERSE::SHOW_BASIC_SCHD_SIDE);
	}
	else if(nID==ID_SHOW_END_START_TRAINID)
	{
		mTG.SetTrainDisplayFlag(TRAIN_DISPLY::REVERSE, REVERSE::SHOW_RETURN_TRAINID);
	}
	else if(nID==ID_SHOW_DEPART_MEET)
	{
		mTG.SetTrainDisplayFlag(TRAIN_DISPLY::REVERSE, REVERSE::SHOW_DEPART_MEET_MARK);
	}
	else if(nID==ID_SHOW_VIRTUAL_STATION)
	{
		mTG.SetTrainDisplayFlag(TRAIN_DISPLY::REVERSE, REVERSE::SHOW_VIRTUAL_STATION);
	}
	else if(nID==ID_MOVE_SAME_CZ_TRAIN)
	{
		mTG.SetTrainDisplayFlag(TRAIN_DISPLY::REVERSE, REVERSE::MOVE_SAME_CZ_TRAIN);
	}
	else if (nID==ID_SHOW_TRANS_INFO)
	{
		mTG.SetTrainDisplayFlag(TRAIN_DISPLY::REVERSE, REVERSE::SHOW_TRANS_INFO);
	}
	else if (nID==ID_SHOW_ALL_CZH_LINE)
	{
		mTG.SetTrainDisplayFlag(TRAIN_DISPLY::REVERSE, REVERSE::SHOW_ALL_CZH_LINE);
	}
	else if (nID==ID_SHOW_PLAN_NOTETEXT)
	{
		mTG.SetTrainDisplayFlag(TRAIN_DISPLY::REVERSE, REVERSE::SHOW_PLAN_NOTE_TEXT);
	}
	
	return;
}

void CTgView::OnUpdateShowReverse(CCmdUI *pCmdUI)
{
	int nID=pCmdUI->m_nID;

	int ScheduleType=-1;
	CTG_ScheduleInfo::SCHEDULE_TYPE type;
	type = GetViewScheduleType();
	if (IS_BASIC_SCHD(type))
		ScheduleType=0;
	else
		ScheduleType=1;

	if (ScheduleType<0)
		return;

	if(nID==ID_ACCEPT_TRAIN_LATE)
	{
		pCmdUI->SetCheck(m_gpcContext.mReverseFlag[ScheduleType][0]==REVERSE::ACCEPT_TRAIN_LATE);   
	}
	else if(nID==ID_ACCEPT_TRUCK_LATE)
	{
		pCmdUI->SetCheck(m_gpcContext.mReverseFlag[ScheduleType][1]==REVERSE::ACCEPT_TRUCK_LATE);  
	}	
	else if(nID==IDC_BYPASS_FLAG)
	{
		pCmdUI->SetCheck(m_gpcContext.mReverseFlag[ScheduleType][2]==REVERSE::BY_PASS_FLAG);  
	}	
	else if(nID==ID_SHOW_WAVE_LINE)
	{
		pCmdUI->SetCheck(m_gpcContext.mReverseFlag[ScheduleType][3]==REVERSE::SHOW_WAVE); 
	}	
	else if(nID==ID_SHOW_ERROR_TAG)
	{
		pCmdUI->SetCheck(m_gpcContext.mReverseFlag[ScheduleType][4]==REVERSE::SHOW_ERROR); 
	}	
	/*
	else if(nID==ID_SHOW_NOT_ADJUST_ERROR)
	{
		pCmdUI->SetCheck(m_gpcContext.mReverseFlag[ScheduleType][5]==REVERSE::SHOW_ADJUST_ERROR); 
	}*/	
	else if(nID==ID_SHOW_TRACTIONENGINE)
	{
		pCmdUI->SetCheck(m_gpcContext.mReverseFlag[ScheduleType][6]==REVERSE::SHOW_ENGINE); 
	}	
	else if(nID==ID_SHOW_EARLYLATE_TIME)
	{
		pCmdUI->SetCheck(m_gpcContext.mReverseFlag[ScheduleType][7]==REVERSE::SHOW_EARLY_LATE_TIME); 
	}	
	else if(nID==ID_SHOW_TIME)
	{
		pCmdUI->SetCheck(m_gpcContext.mReverseFlag[ScheduleType][8]==REVERSE::SHOW_FAST_SLOW_TIME); 
	}	
	else if(nID==IDC_SHOW_DISTANCE_STONE)
	{
		pCmdUI->SetCheck(m_gpcContext.mReverseFlag[ScheduleType][9]==REVERSE::SHOW_KILO_MARK); 
	}	
	else if(nID==ID_SHOW_CONFLICT)
	{
		pCmdUI->SetCheck(m_gpcContext.mReverseFlag[ScheduleType][10]==REVERSE::SHOW_CONFLICT); 
	}	
	else if(nID==ID_SHOW_REVERSE)
	{
		pCmdUI->SetCheck(m_gpcContext.mReverseFlag[ScheduleType][11]==REVERSE::SHOW_REVERSE); 
	}	
	else if(nID==ID_SHOW_WORKSIDE_ERR)
	{
		pCmdUI->SetCheck(m_gpcContext.mReverseFlag[ScheduleType][12]==REVERSE::SHOW_ERR_WORK_SIDE); 
	}	
	else if(nID==ID_SHOW_ELECLOCOMOTIVE)
	{
		pCmdUI->SetCheck(m_gpcContext.mReverseFlag[ScheduleType][13]==REVERSE::SHOW_ELEC_LOCO); 
	}
	else if(nID==ID_THIN_LINE)
	{
		pCmdUI->SetCheck(m_gpcContext.mLineWidth==LINE::THIN); 
	}	
	else if(nID==ID_SHOW_YINGYE_INFO)
	{
		pCmdUI->SetCheck(mTG.GetTrainDisplayFlag(TRAIN_DISPLY::REVERSE, REVERSE::SHOW_TECH_STOP_MARK));
	}
	else if(nID==ID_SHOW_ROUTE_TRIGER_OK)
	{
		pCmdUI->SetCheck(mTG.GetTrainDisplayFlag(TRAIN_DISPLY::REVERSE, REVERSE::SHOW_ROUTE_TRIGER_OK_FLAG));
	}
	else if(nID==ID_SHOW_ROUTE_NOTAUTO_TRIGER)
	{
		pCmdUI->SetCheck(mTG.GetTrainDisplayFlag(TRAIN_DISPLY::REVERSE, REVERSE::SHOW_ROUTE_NOT_AUTO_TRIGER_ROUND));
	}
	else if(nID==ID_SHOW_SEL_CZH_LINE)
	{
		pCmdUI->SetCheck(mTG.GetTrainDisplayFlag(TRAIN_DISPLY::REVERSE, REVERSE::SHOW_SEL_CZH_LINE));
	}
	else if(nID==ID_SHOW_WORKSIDE)
	{
		pCmdUI->SetCheck(mTG.GetTrainDisplayFlag(TRAIN_DISPLY::REVERSE, REVERSE::SHOW_BASIC_SCHD_SIDE));
	}
	else if(nID==ID_SHOW_END_START_TRAINID)
	{
		pCmdUI->SetCheck(mTG.GetTrainDisplayFlag(TRAIN_DISPLY::REVERSE, REVERSE::SHOW_RETURN_TRAINID));
	}
	else if(nID==ID_SHOW_DEPART_MEET)
	{
		pCmdUI->SetCheck(mTG.GetTrainDisplayFlag(TRAIN_DISPLY::REVERSE, REVERSE::SHOW_DEPART_MEET_MARK));
	}
	else if(nID==ID_SHOW_VIRTUAL_STATION)
	{
		pCmdUI->SetCheck(mTG.GetTrainDisplayFlag(TRAIN_DISPLY::REVERSE, REVERSE::SHOW_VIRTUAL_STATION));
	}
	else if(nID==ID_MOVE_SAME_CZ_TRAIN)
	{
		pCmdUI->SetCheck(mTG.GetTrainDisplayFlag(TRAIN_DISPLY::REVERSE, REVERSE::MOVE_SAME_CZ_TRAIN));
	}
	else if(nID==ID_SHOW_TRANS_INFO)
	{
		pCmdUI->SetCheck(mTG.GetTrainDisplayFlag(TRAIN_DISPLY::REVERSE, REVERSE::SHOW_TRANS_INFO));
	}
	else if(nID==ID_SHOW_ALL_CZH_LINE)
	{
		pCmdUI->SetCheck(mTG.GetTrainDisplayFlag(TRAIN_DISPLY::REVERSE, REVERSE::SHOW_ALL_CZH_LINE));
	}
	else if (nID==ID_SHOW_PLAN_NOTETEXT)
	{
		pCmdUI->SetCheck(mTG.GetTrainDisplayFlag(TRAIN_DISPLY::REVERSE, REVERSE::SHOW_PLAN_NOTE_TEXT));
	}
	return;
}

void CTgView::OnBig()
{
	CMenu menu;
	menu.CreatePopupMenu();

	menu.AppendMenu(MF_BYPOSITION|MF_STRING, ID_ZOOM_OUT_BEGIN+0, "放大%150");
	menu.AppendMenu(MF_BYPOSITION|MF_STRING, ID_ZOOM_OUT_BEGIN+1, "放大%100");
	menu.AppendMenu(MF_BYPOSITION|MF_STRING, ID_ZOOM_OUT_BEGIN+2, "放大%50");
	menu.AppendMenu(MF_BYPOSITION|MF_STRING, ID_ZOOM_OUT_BEGIN+3, "放大%25");
	
	CPoint point;
	::GetCursorPos(&point);
	
	menu.TrackPopupMenu(
		TPM_LEFTALIGN | TPM_RIGHTBUTTON, 
		point.x, point.y,this);
}

void CTgView::OnZoomOutRange(UINT nID)
{
	short val=0;
	if(nID==ID_ZOOM_OUT_BEGIN)
		val=150;
	else if(nID==ID_ZOOM_OUT_BEGIN+1)
		val=100;
	else if(nID==ID_ZOOM_OUT_BEGIN+2)
		val=50;
	else if(nID==ID_ZOOM_OUT_BEGIN+3)
		val=25;
	if(val>0 && val<=150)
	{
		short nPixelPerMinute = mTG.ZoomOut(val);
		if(nPixelPerMinute!=0)
			m_gpcContext.mPixelPerMinute = nPixelPerMinute;
	}
}

void CTgView::OnSmall()
{
	CMenu menu;
	menu.CreatePopupMenu();

	menu.AppendMenu(MF_BYPOSITION|MF_STRING, ID_ZOOM_IN_BEGIN+0, "缩小%25");
	menu.AppendMenu(MF_BYPOSITION|MF_STRING, ID_ZOOM_IN_BEGIN+1, "缩小%50");
	menu.AppendMenu(MF_BYPOSITION|MF_STRING, ID_ZOOM_IN_BEGIN+2, "缩小%75");
		
	CPoint point;
	::GetCursorPos(&point);
	
	menu.TrackPopupMenu(
		TPM_LEFTALIGN | TPM_RIGHTBUTTON, 
		point.x, point.y,this);
}

void CTgView::OnZoomInRange(UINT nID)
{
	short val=0;
	if(nID==ID_ZOOM_IN_BEGIN)
		val=25;
	else if(nID==ID_ZOOM_IN_BEGIN+1)
		val=50;
	else if(nID==ID_ZOOM_IN_BEGIN+2)
		val=75;
	if(val>0 && val<100)
	{
		short nPixelPerMinute = mTG.ZoomIn(val);
		if(nPixelPerMinute!=0)
			m_gpcContext.mPixelPerMinute = nPixelPerMinute;
	}
}

void CTgView::OnFindTrain()
{
	mTG.FindTrain();
}

void CTgView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	// 不能调用默认,否则白屏
	//else
	//return CView::OnUpdate(pSender,lHint,pHint); 
}

void CTgView::OnDispLeapTrainNone()
{
	m_load.m_nLeapLineFlag = 1;
	mTG.SetLeapTrainShow(m_load.m_nLeapLineFlag);
}

void CTgView::OnUpdateDispLeapTrainNone(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_load.m_nLeapLineFlag == 1);
}

void CTgView::OnDispLeapTrainJierujiaochu()
{
	m_load.m_nLeapLineFlag = 2;
	mTG.SetLeapTrainShow(m_load.m_nLeapLineFlag);
}

void CTgView::OnUpdateDispLeapTrainJierujiaochu(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_load.m_nLeapLineFlag == 2);
}

void CTgView::OnDispLeapTrainLink()
{
	m_load.m_nLeapLineFlag = 3;
	mTG.SetLeapTrainShow(m_load.m_nLeapLineFlag);
}

void CTgView::OnUpdateDispLeapTrainLink(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_load.m_nLeapLineFlag == 3);
}

void CTgView::OnDispBegin18()
{
	mTG.SetBackgroudBeginHour(BACKGROUD::TIME::BEGIN::SIX);
}
void CTgView::OnDispBegin12()
{
	mTG.SetBackgroudBeginHour(BACKGROUD::TIME::BEGIN::TOWNTY);
}

void CTgView::OnBasicTrainMoveWhole()
{
	BOOL val=mTG.GetTrainDisplayFlag(TRAIN_DISPLY::BASIC_MOVE_NULTI_STATION, 0);
	if(val==TRUE)
	    mTG.SetTrainDisplayFlag(TRAIN_DISPLY::BASIC_MOVE_NULTI_STATION, FALSE);
	else
		mTG.SetTrainDisplayFlag(TRAIN_DISPLY::BASIC_MOVE_NULTI_STATION, TRUE);
}

void CTgView::OnUpdateBasicTrainMoveWhole(CCmdUI* pCmdUI)
{
	BOOL val=mTG.GetTrainDisplayFlag(TRAIN_DISPLY::BASIC_MOVE_NULTI_STATION, 0);
	pCmdUI->SetCheck(val);
}

void CTgView::OnRefreshTgData()
{
	if(MessageBox("该操作将删除当前运行图所有数据，然后重新从服务器获取,确认吗?","警告",MB_YESNO)!=IDYES)
		return ;

	GpcLogTrainMessage(0, 0, "[调度员操作] 通过点击界面 刷新数据 按钮, 重新从服务器获取运行图数据!");
	GetDocument()->GetTGDataHolder()->ReloadData(GetViewScheduleType(),TRUE);
	
	RefreshViewData();
}

void CTgView::OnWholeScreen()
{
	if(mTG.GetTrainDisplayFlag(TRAIN_DISPLY::REVERSE, REVERSE::WHOLE_SCREEN_STATE)==TRUE)
	{
		mTG.ShowWholeScreen(1);
	}
	else
	{
		mTG.ShowWholeScreen(0);
	}

}

CString CTgView::GetSchdName()
{
	CString str;
	if(GetViewScheduleType()==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC)
	{
		str="常用基本图";
	}
	else if(GetViewScheduleType()==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_BACKUP)
	{
		str="归档基本图";
	}
	else if(GetViewScheduleType()==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_PREV)
	{
		str="预览基本图";
	}
	else
	{
		str="未知图";
	}

	return str;
}

void CTgView::OnSaveToFiles()
{
	CString str;
	str.Format("你确实要保存[%s]到本地文件吗?", GetSchdName());

	if(MessageBox(str,"警告",MB_YESNO)==IDYES)
	{
		CString str;
		if(GetDocument()->GetTGDataHolder()->SaveDataToLocalFiles(GetViewScheduleType(), str))
			AfxMessageBox("文件成功保存在目录中"+str);
		else
			AfxMessageBox("保存失败");
	}
}

void CTgView::OnLoadFromFiles()
{
	CTG_TrainScheduleBase *pbase=GetDocument()->GetScheduleData(GetViewScheduleType());
	CDlgRestoreTrainList dlg(pbase);
	OnRecievedDataEnd(0,1);
	dlg.DoModal();
	OnRecievedDataEnd(0,0);
}

void CTgView::OnUpdateWholeScreen(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(mTG.GetTrainDisplayFlag(TRAIN_DISPLY::REVERSE, REVERSE::WHOLE_SCREEN_STATE));
}

void CTgView::OnPlot()
{
	sysprint(5,0,"调度员申请打印运行图");
	m_nPlotFlag=0;
	mTG.GetPlotParam(0);
	sysprint(5,0,"显示打印预览运行图");
}
void CTgView::OnPlot18_06()
{
	sysprint(5,0,"调度员申请打印运行图");
	m_nPlotFlag=1;
	mTG.GetPlotParam(0);
	sysprint(5,0,"显示打印预览运行图");
}
void CTgView::OnPlot06_18()
{
	sysprint(5,0,"调度员申请打印运行图");
	m_nPlotFlag=2;
	mTG.GetPlotParam(0);
	sysprint(5,0,"显示打印预览运行图");
}
extern void PlotSchedule(CTgDocument *pdoc,int sche_type,const CString& plot_param, const int plotflag);
void CTgView::PLOT_PARAM(LPCTSTR param_xml, long nFlag)
{
	sysprint(5,0,"TG返回打印参数");
	((CWintg *)AfxGetApp())->PlotSchedule(GetDocument(),GetViewScheduleType(),param_xml, m_nPlotFlag);
}

BOOL  CTgView::CheckUserTGRight()
{
	if(GetViewScheduleType()==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC)
	{
		return FALSE;
	}
	else if(GetViewScheduleType()==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_BACKUP)
	{
		return FALSE;
	}
	else if(GetViewScheduleType()==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_PREV)
	{
		if(UR_CanOperateBasicSchedule())
			return TRUE;
		else
			return FALSE;
	}
	return FALSE;
}

void CTgView::OnUpdateOperTGTrain(CCmdUI *pCmdUI)
{
	if(!CheckUserTGRight()) 
	{
		pCmdUI->Enable(FALSE); 
		return;
	}
	int gpcid = gpcdef.GetUserGpcId();
	RIGHT right;
	if(!m_load.GetUserRight(gpcid, right))
	{
		pCmdUI->Enable(FALSE); 
	}
	if(right.operate==1 || right.operate==3)
		pCmdUI->Enable(TRUE);
	else
		pCmdUI->Enable(FALSE); 
}

void CTgView::OnBasicGraphsavetotext() 
{
	CTgDocument  *pDoc  = GetDocument();
	CString fileDir;
	fileDir.Format("%s\\xml",gpcdef.GetWorkPathName());

	CString infoName, recordsName, xmlfileName, tnfilename;
	infoName.Format("bsinfo%02d.txt",pDoc->GetCurrentSectionNo());
	recordsName.Format("tntime%02d.txt",pDoc->GetCurrentSectionNo());
	xmlfileName.Format("section%02d.xml",pDoc->GetCurrentSectionNo());
	tnfilename.Format("运行图到发点文件%02d.txt",pDoc->GetCurrentSectionNo());

	bool bFindInfoFile=false, bFindRecordFile=false, 
		bFindTnFile=false, bFindXmlFile=false;

	CFileFind  fileFind;
	BOOL flag = fileFind.FindFile(fileDir+"\\*.*");
	while(flag)
	{
		flag = fileFind.FindNextFile();

		if(fileFind.IsDirectory())
			continue;

		CString fileName = fileFind.GetFileName();
		if(fileName == infoName)
		{
			bFindInfoFile=true;
		}
		if(fileName == recordsName)
		{
			bFindRecordFile=true;
		}
		if(fileName == tnfilename)
		{
			bFindTnFile=true;
		}
		if(fileName == xmlfileName)
		{
			bFindXmlFile=true;
		}
	}

	//获得文件名
	if(bFindInfoFile || bFindRecordFile || bFindTnFile || bFindXmlFile)
	{
		CString cap;
		cap.Format("在保存目录%s下,存在你要保存的同名文件, 你确认覆盖吗?",fileDir);
		if(MessageBox(cap, "提示", MB_YESNO|MB_ICONQUESTION)==IDNO)
		{
			AfxMessageBox("文件保存失败!!");
			return;
		}
	}

	CString path = fileDir + "\\" + infoName;
	pDoc->olSaveTrainInfo(GetViewScheduleType(),path);

	path = fileDir + "\\" + recordsName;
	pDoc->olSaveTrainRecords(GetViewScheduleType(),path);

	path = fileDir + "\\" + tnfilename;
	pDoc->olSaveTrainRecords(GetViewScheduleType(),path);

	path = fileDir + "\\" + xmlfileName;
	pDoc->olSaveTrainToFile(GetViewScheduleType(),path);

	CString cap;
	cap.Format("文件保存在 %s 目录下,共有四个文件 %s %s %s %s",fileDir, infoName, recordsName, tnfilename, xmlfileName);
	MessageBox(cap);

	return;
}

void CTgView::OnUpdateSaveBasicSideWorkInfo(CCmdUI *pCmdUI)
{
	if(GetViewScheduleType()==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC)
		pCmdUI->Enable(TRUE);
	else if(GetViewScheduleType()==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_BACKUP)
		pCmdUI->Enable(TRUE);
	else
		pCmdUI->Enable(FALSE);
}

void CTgView::OnSaveBasicSideWorkInfo()
{
	if(GetViewScheduleType()!=CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC)
	{
		AfxMessageBox("请注意, 只能保存常用基本图信息"); 
		return;
	}

	if(!gpcdef.GetOperatorPrior4Basic())
	{
		AfxMessageBox("你没有保存基本图信息的权限"); 
		return;
	}
	for(int i=0; i<3; i++)
	{
		CSuperPassDlg pdlg("输入密码");
		if (pdlg.DoModal() == IDOK)
		{
			if (!pdlg.PasswordIsOk())
			{
				AfxMessageBox("密码错误！！！"); 
				continue;
			}
			else
			{
				break;
			}
		}
		else
		{
			return;
		}
	}
	if(i>=3)
		return;

	CTgDocument  *pDoc  = GetDocument();

	CString fileDir, fileName;
	fileDir.Format("%s\\xml",gpcdef.GetWorkPathName());
	fileName.Format("section_basic%02d.xml",pDoc->GetCurrentSectionNo());

	bool bFindInfoFile=false;
	CFileFind  fileFind;
	BOOL flag = fileFind.FindFile(fileDir+"\\*.xml");
	while(flag)
	{
		flag = fileFind.FindNextFile();

		if(fileFind.IsDirectory())
			continue;

		CString strName = fileFind.GetFileName();
		if(strName == fileName)
		{
			bFindInfoFile=true;
			break;
		}
	}

	//获得文件名
	if(bFindInfoFile)
	{
		CString cap;
		cap.Format("在保存目录%s下,存在你要保存的同名文件 %s, 你确认覆盖吗?",fileDir, fileName);
		if(MessageBox(cap, "提示", MB_YESNO|MB_ICONQUESTION)==IDNO)
		{
			AfxMessageBox("文件保存失败!!");
			return;
		}
	}

	if(MessageBox("请注意, 你确认保存常用基本图信息吗?","警告",MB_YESNO)==IDYES)
	{
		fileName = fileDir + "\\" + fileName;
		if(!pDoc->SaveTrainSideTechToFile(GetViewScheduleType(),fileName))
			return;
		AfxMessageBox("文件保存在 "+fileName);
	}
}

LRESULT CTgView::OnSetTrainFoucs(WPARAM wParam, LPARAM lParam)
{
	mTG.FindTrainByTrainIndex(wParam);
	return 0;
}

void CTgView::OnPlanCompareStationSet()
{
	CDlgSchdCompareStationSet dlg(m_schdinfo.m_nSectionID,this);
	dlg.DoModal();
}

void CTgView::OnSetStplanNoWarningStation()
{
	CDlgStplanNowarningStationSet dlg(m_schdinfo.m_nSectionID, 
		GetDocument()->m_nStplanStationCount, GetDocument()->m_nStplanStation, this);
	dlg.DoModal();
}

void CTgView::OnSetSideCalTrainType()
{
	CDlgSideCalTrainSet dlg(m_schdinfo.m_nSectionID,this);
	dlg.DoModal();
}

void CTgView::OnUpdateEditAllCzh(CCmdUI* pCmdUI)
{
	if(GetViewScheduleType()==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC || GetViewScheduleType()==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_BACKUP)
	{
		pCmdUI->Enable(FALSE);
	}
	else if(GetViewScheduleType()==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_PREV)
	{
		if(GetTGFunctionConfig()->IsEnableCzh() || GetTGFunctionConfig()->IsEnableNewCzh())
		{
			short nCanModify = mTG.GetTrainDisplayFlag(TRAIN_DISPLY::BSCHD_OPER, 0);
			if(nCanModify>0)
				pCmdUI->Enable(TRUE);
			else
				pCmdUI->Enable(FALSE);
		}
		else
		{
			pCmdUI->Enable(FALSE);
		}
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}
void CTgView::OnUpdateGenAllReturnTrainid(CCmdUI *pCmdUI)
{
	if(GetTGFunctionConfig()->IsChangeTrainID())
	{
		if(GetViewScheduleType()==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC || GetViewScheduleType()==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_BACKUP)
		{
			pCmdUI->Enable(FALSE);
		}
		else if(GetViewScheduleType()==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_PREV)
		{
			if(GetTGFunctionConfig()->IsEnableCzh())
			{
				short nCanModify = mTG.GetTrainDisplayFlag(TRAIN_DISPLY::BSCHD_OPER, 0);
				if(nCanModify>0)
					pCmdUI->Enable(TRUE);
				else
					pCmdUI->Enable(FALSE);
			}
			else
			{
				pCmdUI->Enable(FALSE);
			}
		}
		else
		{
			pCmdUI->Enable(FALSE);
		}
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}

void CTgView::OnUpdateDeleteBasic(CCmdUI *pCmdUI)
{
	if(GetViewScheduleType()!=CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_PREV) 
	{
		pCmdUI->Enable(FALSE);
	}
	else
	{
		if(gpcdef.GetOperatorPrior4Basic() && (gpcdef.GetUserIdentify() == GPC_TEST))
			pCmdUI->Enable(TRUE);
		else
			pCmdUI->Enable(FALSE);
	}
}

// 单独定义助调对区间作业车的权限---一般情况下禁止助调绘制区间作业运行线，在某种可能下会造成本台列车索引和其他调度台重复
void CTgView::OnUpdateOperAddWorkTrain(CCmdUI *pCmdUI)
{
	if(!CheckUserTGRight()) 
	{
		pCmdUI->Enable(FALSE); 
		return;
	}
	int gpcid = gpcdef.GetUserGpcId();
	RIGHT right;
	if(!m_load.GetUserRight(gpcid, right))
	{
		pCmdUI->Enable(FALSE); 
	}

	if(right.operate==1 || right.operate==3)
		pCmdUI->Enable(TRUE); 
	else if(right.operate==2)
	{
		if(IsInhbitAddWorkTrain(right.wintg_operate))
			pCmdUI->Enable(FALSE);
		else
			pCmdUI->Enable(TRUE);
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}

void CTgView::OnUpdateSelTdmsTrainsGenBasic(CCmdUI *pCmdUI)
{
	if(m_bSelTDMSTrainGenBasic)
		pCmdUI->Enable(TRUE);
	else
		pCmdUI->Enable(FALSE); 
}

void CTgView::OnUpdateBasicNameing(CCmdUI* pCmdUI)
{
	if(gpcdef.IsNewBasicDump())
	{
		if(gpcdef.GetUserIdentify() == GPC_TEST && gpcdef.GetOperatorPrior4Basic())
		{
			if(GetViewScheduleType()==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_PREV)
				pCmdUI->Enable(TRUE);
			else
				pCmdUI->Enable(FALSE);
		}
		else
			pCmdUI->Enable(FALSE);
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}

void CTgView::OnUpdateBasicDeleteName(CCmdUI* pCmdUI)
{
	if(gpcdef.IsNewBasicDump())
	{
		if(gpcdef.GetUserIdentify() == GPC_TEST && gpcdef.GetOperatorPrior4Basic())
		{
			if(GetViewScheduleType()==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_PREV)
				pCmdUI->Enable(TRUE);
			else
				pCmdUI->Enable(FALSE);
		}
		else
			pCmdUI->Enable(FALSE);
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}

void CTgView::OnUpdateBasicGraphchange(CCmdUI* pCmdUI)
{
	if(gpcdef.IsNewBasicDump())
	{
		if(gpcdef.GetUserIdentify() == GPC_DISPATCHER && GetViewScheduleType()==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_BACKUP)
		{
			if(gpcdef.GetOperatorPrior4Basic())
				pCmdUI->Enable(TRUE);
			else
				pCmdUI->Enable(FALSE);
		}
		else
		{
			if(gpcdef.GetUserIdentify() == GPC_TEST && gpcdef.GetOperatorPrior4Basic())
				pCmdUI->Enable(TRUE);
			else
				pCmdUI->Enable(FALSE);
		}
	}
	else
	{
		if(gpcdef.GetUserIdentify() == GPC_TEST && gpcdef.GetOperatorPrior4Basic())
		{
			if(GetViewScheduleType()==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_PREV)
				pCmdUI->Enable(TRUE);
			else
				pCmdUI->Enable(FALSE);
		}
		else
			pCmdUI->Enable(FALSE);
	}
}

void CTgView::OnUpdateModifyBschd(CCmdUI* pCmdUI)
{
	if(gpcdef.GetOperatorPrior4Basic() && CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_PREV==GetViewScheduleType() && gpcdef.GetUserIdentify() == GPC_TEST)
	{
		pCmdUI->Enable(TRUE);
	}
	else
	{
		pCmdUI->Enable(FALSE);
		return;
	}

	short nCanModify = mTG.GetTrainDisplayFlag(TRAIN_DISPLY::BSCHD_OPER, 0);
	if(nCanModify > 0)
		pCmdUI->SetCheck(1);
	else
		pCmdUI->SetCheck(0);
}