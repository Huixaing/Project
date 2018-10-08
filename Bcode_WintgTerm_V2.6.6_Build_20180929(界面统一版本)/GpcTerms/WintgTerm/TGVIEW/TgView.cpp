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
#include "DlgQueBaoList.h"
#include "CPropertysheetNotepad.h"
#include "QueryPhaseStoreTrain.h"//车站暂存车
#include "LoginDlg.h"
#include ".\tgview.h"
#include "utility\mmi_function_config.h"
#include "JiaojiecheStatisticDlg.h"
#include "TrainSubaoDlg.h"
#include "DlgDDYNameQuery.h"
#include ".\predict\PredictHistoryDlg.h"
#include "ChangePasswordDlg.h"
#include "TrainEngineInfoDlg.h"
#include "basic_schd_config.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
// CTgView
extern int DB_GetEntityStatus(long eid);
extern basic_schd_config* GetBasicConfig();
extern TG_STATION_STATUS *GetStationStatusPtr(int station); // defined in tg_logic_main.cpp

extern void  RegisterDataEndEvent(HWND hWnd, const CTG_ScheduleInfo & schd);
extern void  UnRegisterDataEndEvent(HWND hWnd);

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
	
	ON_COMMAND(ID_SELFNEXTSTD, OnSelfNextSTD)
	ON_UPDATE_COMMAND_UI(ID_SELFNEXTSTD, OnUpdateSelfNextSTD)

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
	ON_COMMAND(IDC_NEW_DC_COMMAND,OnDCMD)
	
	ON_COMMAND(ID_QUEBAO_QUERY_INFO,OnQueBao)
	ON_COMMAND(ID_BUTTON_LOGIN, OnButtonLogin)
	ON_COMMAND(ID_CHANGE_PASSWORD, OnChangePassword)

	ON_COMMAND(ID_LINE_EDIT_SELECT, OnModifyAtcual)
	ON_UPDATE_COMMAND_UI(ID_LINE_EDIT_SELECT, OnUpdateModifyAtcual)

	ON_COMMAND(ID_SEND_JIAOBAN_PLAN,OnJiaoBan)
	
	ON_COMMAND(ID_SHIGU_OUTLINE,OnShigu)
	ON_COMMAND(ID_PHASE_NOTEPAD,OnNote)
	ON_COMMAND(ID_STATION_CURRENT_TRAINS, OnStationCurrentTrains)
	ON_COMMAND(ID_SCHD_SUBAO, OnSUBAO)
	ON_COMMAND(ID_TRAIN_WORK_PLAN, OnShuaiGua)
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

	ON_COMMAND(ID_JIAOJIECHESTATIC, OnJiaojiechestatistic)
	ON_COMMAND(ID_DDYNAME_QUERY, OnQueryDDYName)
	ON_COMMAND(ID_PREDICT_QUERY, OnPredictHistroyView)
	
	ON_COMMAND(ID_DISPLAY_DATA_XML, OnShowDataXML)

	ON_MESSAGE(WM_USER + 2001,OnSetTrainFoucs)

	ON_COMMAND_RANGE(ID_ZOOM_OUT_BEGIN, ID_ZOOM_OUT_END, OnZoomOutRange)
	ON_COMMAND_RANGE(ID_ZOOM_IN_BEGIN, ID_ZOOM_IN_END, OnZoomInRange)
	
END_MESSAGE_MAP()

BEGIN_EVENTSINK_MAP(CTgView, CView)
	ON_EVENT(CTgView, IDC_TGCTRL_VIEW, 7, TG_LOG, VTS_BSTR)
	ON_EVENT(CTgView, IDC_TGCTRL_VIEW, 8, SELECT_TRAIN, VTS_I4 VTS_I4 VTS_I2 VTS_I2 VTS_I4 VTS_I4)
	ON_EVENT(CTgView, IDC_TGCTRL_VIEW, 9, SELECT_MARK, VTS_I4 VTS_I1 VTS_I1 VTS_I2 VTS_I2 VTS_I4)
	ON_EVENT(CTgView, IDC_TGCTRL_VIEW, 10, TG_NOTICE, VTS_I1 VTS_I4 VTS_I4 VTS_I2)
	ON_EVENT(CTgView, IDC_TGCTRL_VIEW, 12, GEN_BLOCK_DCMD, VTS_BSTR VTS_I4 VTS_I4)
	ON_EVENT(CTgView, IDC_TGCTRL_VIEW, 13, PLOT_PARAM, VTS_BSTR VTS_I4)
	ON_EVENT(CTgView, IDC_TGCTRL_VIEW, 17, ShowCrewInfo, VTS_UI4 VTS_UI4 VTS_UI4)
END_EVENTSINK_MAP()

void CTgView::GEN_BLOCK_DCMD(LPCTSTR content, long nIndex, long type)
{
	if (type == 3) //发布调度命令提示
	{//nIndex == 1
		if (nIndex ==1)
		{
			CString xml;
			xml = content;
			long nEntityOfDCMD=GetSpecialEntityID(GPCTERM_TYPE_DCMD,m_schdinfo.m_nSectionID);

			CString token,content;
			GenXMLMessage_InformDCMDToFront(token,content);
			TGSendStringMessage(token,content,nEntityOfDCMD);
			sysprint(5,0,"向调度命令终端 %d 发送最前端XML\n %s", nEntityOfDCMD, content);

			token="REQ_SIDE_DCMD";
			TGSendStringMessage(token, xml, nEntityOfDCMD);

			sysprint(5,0,"向调度命令终端 %d 发送生成命令XML\n %s", nEntityOfDCMD, content);
		}
	}
	else if(nIndex>0)
	{ 
		CTG_TrainScheduleBase *pData=m_tgmmi.GetDataStore(m_schdinfo);
		if(pData)
		{
			CTG_SingleMark *pmark=pData->GetSingleMark(nIndex);
			if(pmark)
			{
				GetDocument()->GenDCmd(pmark->GetShiftIndexString(), pmark->GetBlockIndex(), content, m_schdinfo.m_nSectionID,type);
			}
		}
	}
}

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
	m_bSelfNextSTDState = false; // 调整标志

	m_bUserMemDC=FALSE;
	
	for(int i=0;i<2;i++)
	{
		m_gpcContext.mReverseFlag[i][11] = REVERSE::SHOW_REVERSE;
		m_gpcContext.mReverseFlag[i][12] = REVERSE::SHOW_ERR_WORK_SIDE; 
	}

	m_gpcContext.mLineWidth = LINE::BOLD;

	m_gpcContext.mStatsicTime=0;
	m_bCanClose=TRUE;

	m_nPlotFlag=0;
	m_bDyncView=false;
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
	if(gpcdef.IsNewAutoMoveMode())
	{
		if(CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC==type)
			pholder->GetBasicScheduleInfo(schd_info);
		else if(CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_PREV==type)
			pholder->GetBasicPrevScheduleInfo(schd_info);
		else if(CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_BACKUP==type)
			pholder->GetBasicScheduleInfo(schd_info);
		else if(CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT==type)
			pholder->GetShiftScheduleInfo(schd_info);
		else
			pholder->GetWorkScheduleInfo(schd_info);
	}
	else
	{
		pholder->GetWorkScheduleInfo(schd_info);
	}
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
	if(type==CTG_ScheduleInfo::SCHEDULE_TYPE_WORK)
	{
		int mode=gpcdef.GetWidePixel();
		if( !m_bDyncView && (mode>0)) // 非动态VIEW,排出邻台图
		{
			if(mode==1)
				mTG.SetTrainDisplayFlag(TRAIN_DISPLY::SET_PIXEL_PER_MINUTE, 5);
			if(mode==2)
				mTG.SetTrainDisplayFlag(TRAIN_DISPLY::SET_PIXEL_PER_MINUTE, 12);
		}
		else if(mode>0)
		{
			mTG.SetTrainDisplayFlag(TRAIN_DISPLY::SET_PIXEL_PER_MINUTE, 5);
		}

		if( !m_bDyncView) // 非动态VIEW,排出邻台图
		{
			if(GetTGFunctionConfig()->IsUsingWorkGoodMode())
			{
				GpcLogSysMessage(0,"使用上下行优化模式");
				mTG.UsingWorkGoodMode();
			}
		}
		mTG.SetTgType(TERM_TG_TYPE::WORK_TG);
	}
	else if(type==CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT)
	{
		mTG.SetTgType(TERM_TG_TYPE::SHIFT_TG);
		mTG.SetAllStationActive(); // 避免日班计划出现状态 断字
	}
	else
	{
		if(CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC==type)
			mTG.SetTgType(TERM_TG_TYPE::NOW_BASIC_TG);
		else if(CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_PREV==type)
			mTG.SetTgType(TERM_TG_TYPE::PRE_BASIC_TG);
		else if(CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_BACKUP==type)
			mTG.SetTgType(TERM_TG_TYPE::READY_BASIC_TG);
		else
			mTG.SetTgType(TERM_TG_TYPE::QUERY_TG);
	}

	int nRight=gpcdef.GetUserIdentify();
	mTG.SetGpcUserRight(nRight); // 设置用户权限,限制助调多车移动,删除多趟车
	GpcLogSysMessage(0,"根据配置设置用户权限:%d", nRight);
	if(gpcdef.IsNewAutoMoveMode())
	{
		TIME start=0,end=0;
		m_schdinfo.GetViewScopeRange(start,end);
	
		CTime btm(start);
		GpcLogTrainMessage(0, 0, "[InitScheduleView] shiftid %d btime %d(%s) auto_move %d view_hour %d history_hour %d",
			m_schdinfo.m_nShiftID, start, btm.Format("%Y-%m-%d %H:%M:%S"), m_schdinfo.m_nAutoMove, m_schdinfo.m_nTotelLen, m_schdinfo.m_nHistoryLen);

		mTG.SetBackgrandInfo(m_schdinfo.m_nShiftID, start, m_schdinfo.m_nAutoMove, m_schdinfo.m_nTotelLen, m_schdinfo.m_nHistoryLen);
	}
	else
	{
		mTG.SetShiftId(schd_info.m_nShiftID,0);
	}

	if(gpcdef.IsRunFastTimeCompareWithBasic())
		mTG.SetTrainDisplayFlag(TRAIN_DISPLY::RUN_FAST_SLOW_WITH_BASIC, 1);
	else
		mTG.SetTrainDisplayFlag(TRAIN_DISPLY::RUN_FAST_SLOW_WITH_BASIC, 0);

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
		SetTimer(993,1000,NULL);

		GetDocument()->OpenWaitWindow("请稍候正在初始化基本图...");
	}
	// 归档图，默认不从服务器取数据, 需要单独申请
	if(GetViewScheduleType()==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_BACKUP)  
	{
		GetDocument()->GetTGDataHolder()->ReloadData(GetViewScheduleType(),TRUE);
		SetTimer(993,1000,NULL);

		GetDocument()->OpenWaitWindow("请稍候正在初始化基本图...");
	}
	RefreshViewData();

	// 设置TG控件挪动线条时是否保留原有线条
	if(GetTGFunctionConfig()->IsEraseBeforeModifyTrain())
		mTG.SetTrainDisplayFlag(TRAIN_DISPLY::ERASE_BEFORE_MODIFY, 1);
	else
		mTG.SetTrainDisplayFlag(TRAIN_DISPLY::ERASE_BEFORE_MODIFY, 0);
	
	// 设置TG控件是否可以修改图定点
	if(GetTGFunctionConfig()->IsDisableModifyPlanTm())
		mTG.SetTrainDisplayFlag(TRAIN_DISPLY::DISABLE_MODIFY_PLAN_TM, 1);
	else
		mTG.SetTrainDisplayFlag(TRAIN_DISPLY::DISABLE_MODIFY_PLAN_TM, 0);

	// 将禁会信息传递到TG控件
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
	m_nTimeoutSecond=gpcdef.GetWintgNosynTimeoutTime();
	nNosync_with_svr_LastTime=CTime::GetCurrentTime();
	nNosync_with_ocx_LastTime=CTime::GetCurrentTime();

	if (gpcdef.IsMSSAlarmEnable())
	{
		SetTimer(991,5000,NULL); //启动未同步计数检查定时器
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

void CTgView::CheckSyncAlarm()
{
	CTime now=CTime::GetCurrentTime();
	if (m_schdinfo.m_nType == CTG_ScheduleInfo::SCHEDULE_TYPE_WORK)
	{
		if (mServer_nosync_count >0)
		{
			if(now.GetTime() >= nNosync_with_svr_LastTime.GetTime()+m_nTimeoutSecond)
			{
				nNosync_with_svr_LastTime=now;
				
				sprintf(nosync_buf, "调整图和服务器存在 %d 个未同步", mServer_nosync_count);
				SetMSCapacityAlarm(10, nosync_buf,0);
				GpcLogSysMessage(0,"%s", nosync_buf);
			}
		}
		else
		{
			nNosync_with_svr_LastTime=now;
		}
		if (mMmi_nosync_count>0)
		{
			if(now.GetTime() >= nNosync_with_ocx_LastTime.GetTime()+m_nTimeoutSecond)
			{
				nNosync_with_ocx_LastTime=now;
				
				sprintf(nosync_buf, "调整图界面和调整图存在 %d 个未同步", mMmi_nosync_count);
				SetMSCapacityAlarm(11, nosync_buf,0);
				GpcLogSysMessage(0,"%s", nosync_buf);
			}
		}
		else
		{
			nNosync_with_ocx_LastTime=now;
		}
	}
	else if (m_schdinfo.m_nType == CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT)
	{
		if (mServer_nosync_count >0)
		{
			if(now.GetTime() >= nNosync_with_svr_LastTime.GetTime()+m_nTimeoutSecond)
			{
				nNosync_with_svr_LastTime=now;
				
				sprintf(nosync_buf, "日班图和服务器存在 %d 个未同步", mServer_nosync_count);
				SetMSCapacityAlarm(12, nosync_buf,0);
				GpcLogSysMessage(0,"%s", nosync_buf);
			}
		}
		else
		{
			nNosync_with_svr_LastTime=now;
		}
		if (mMmi_nosync_count>0)
		{
			if(now.GetTime() >= nNosync_with_ocx_LastTime.GetTime()+m_nTimeoutSecond)
			{
				nNosync_with_ocx_LastTime=now;
				
				sprintf(nosync_buf, "日班图界面和日班图存在 %d 个未同步", mMmi_nosync_count);
				SetMSCapacityAlarm(13, nosync_buf,0);
				GpcLogSysMessage(0,"%s", nosync_buf);
			}
		}
		else
		{
			nNosync_with_ocx_LastTime=now;
		}
	}
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
	mDatabase_nosync_count=pholder->GetDatabaseNoSyncedObjectCount(m_schdinfo.m_nType);
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
			cs, GetCommStatusInfo(),mServer_nosync_count,strServerNoSyncedInfo,mDatabase_nosync_count,mMmi_nosync_count,strMmiNoSyncedInfo);
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
	else if(991 == event) //未同步计数检查定时器
	{
		CheckSyncAlarm();
	}
	else if(992 == event)
	{
		;
	}
	else if(993 == event) //预览基本图和归档基本图 的定时间隔事件,等待2秒
	{
		KillTimer(993);
		SetTimer(994,100,NULL);
	}
	else if (994 ==event) //预览基本图和归档基本图 的定时刷新事件,在term与ocx同步之后进行刷新
	{
		if (mMmi_nosync_count == 0)
		{
			mTG.RefreshTG(0);
			KillTimer(994);

			GetDocument()->OpenWaitWindow(NULL);
		}
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
				mTG.EndBatchDraw(0, "[下载运行图] 从TDMS接收基本图,日班计划完毕或者从CTC服务器下载运行线完毕");
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

void CTgView::SELECT_MARK(long mark_index, char type, char flag, short  stano1, short  stano2,long param)
{
	if(mark_index>0)
	{
		CTG_TrainScheduleBase *pData=m_tgmmi.GetDataStore(m_schdinfo);
		if(pData)
		{
			CTG_SingleMark *pmark=pData->GetSingleMark(mark_index);
			if(pmark)
			{
				GetDocument()->SetCurrentActiveObject(GetViewScheduleType(),pmark,-1,-1);
				GetParentFrame()->SendMessage(UM_TG_PROPERTY_UPDATE, 1, 1); 
				GetParentFrame()->SendMessage(UM_TG_PROPERTY_UPDATE,(WPARAM)pmark, 0x8000); 
				/*
				long nEntityOfDCMD=GetSpecialEntityID(GPCTERM_TYPE_DCMD,0);
				if(nEntityOfDCMD>0)
				{
				CString token,content;
				TIME st,et;
				GetDocument()->GetMMPlanRange(st,et);
				GenXMLMessage_SelectDCMD(token,content,st,et,pmark->GetShiftIndexString());
				TGSendStringMessage(token,content,nEntityOfDCMD);
				}
				*/
			}
		}
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
	m_pCfWnd.Create(this, 180, 18);
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

	else if(m_schdinfo.m_nType ==CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT )
	{
		title+=temp;
		title += "--班计划图";
	}
	else if(m_schdinfo.m_nType ==CTG_ScheduleInfo::SCHEDULE_TYPE_HIST)
	{
		title+=temp;
		title += "--历史图";
	}
	else 
	{
		title+=temp;
		title += "--实际运行图";
		if (NULL != gpcdef.GetCurrentUsername())
		{
			title += "-调度员: ";
			title += gpcdef.GetCurrentUsername();  
		}
		/*if (NULL != gpcdef.GetCurrentUserid())
		{
		title += "(";
		title += gpcdef.GetCurrentUserid();
		title += ")";
		}*/
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

void CTgView::OnSelfNextSTD()
{
	//	m_bSelfNextSTDState = !m_bSelfNextSTDState;
	//	mTG.Selfnextstd();
}

void CTgView::OnUpdateSelfNextSTD(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_bSelfNextSTDState);
}

void CTgView::OnModifyAtcual()
{
	if(m_gpcContext.mReverseFlag[1][15] == REVERSE::EDIT_REAL_LINE)
	{
		GpcLogTrainMessage(0, 0, "[调度员操作] 通过点击选中修改实际点按钮, 使用<不修改实际点模式>进行操作");
		m_gpcContext.mReverseFlag[1][15] = 0; 
	}
	else
	{
		GpcLogTrainMessage(0, 0, "[调度员操作] 通过点击选中修改实际点按钮, 使用<修改实际点模式>进行操作");
		m_gpcContext.mReverseFlag[1][15] = REVERSE::EDIT_REAL_LINE; 
	}
	mTG.SetTrainDisplayFlag(TRAIN_DISPLY::REVERSE, REVERSE::EDIT_REAL_LINE);
}

void CTgView::OnUpdateModifyAtcual(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_gpcContext.mReverseFlag[1][15]==REVERSE::EDIT_REAL_LINE);
	if(gpcdef.IsInhibitManualConvertActualTime())
	{
		pCmdUI->Enable(0);
	}
	else
	{
		pCmdUI->Enable(1);
		
	}
}

void CTgView::OnFindTrain()
{
	mTG.FindTrain();
}

void CTgView::ShiftBanCiNew()
{
	CWorkScheduleHolder  *pholder=GetDocument()->GetTGDataHolder();
	if(!pholder)
		return;

	CTG_ScheduleInfo  schd_info;
	if(m_schdinfo.m_nType==CTG_ScheduleInfo::SCHEDULE_TYPE_WORK)
		pholder->GetWorkScheduleInfo(schd_info);
	else if(m_schdinfo.m_nType==CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT)
		pholder->GetShiftScheduleInfo(schd_info);
	else if(m_schdinfo.m_nType==CTG_ScheduleInfo::SCHEDULE_TYPE_HIST)
		pholder->GetWorkScheduleInfo(schd_info);
	else
		return;
	m_schdinfo.m_nShiftID = schd_info.m_nShiftID; 
	m_schdinfo.m_nAutoMove = schd_info.m_nAutoMove; 
	m_schdinfo.m_nTotelLen = schd_info.m_nTotelLen; 
	m_schdinfo.m_nHistoryLen = schd_info.m_nHistoryLen; 

	m_schdinfo.m_tStartTime  =schd_info.m_tStartTime; 
	m_schdinfo.m_tEndTime  =schd_info.m_tEndTime;

	CString cs;
	GetTitle(cs);
	GetParentFrame()->SetWindowText(cs);

	mTG.SetShiftId(m_schdinfo.m_nShiftID, 0);
	
	TIME start=0,end=0;
	m_schdinfo.GetViewScopeRange(start,end);

	if(GetTGFunctionConfig()->IsNewModeLoginRefresh())
	{
		if(m_schdinfo.m_nType==CTG_ScheduleInfo::SCHEDULE_TYPE_WORK || m_schdinfo.m_nType==CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT)
		{
			if(start==mTG.GetShiftBeginTime() && m_schdinfo.m_nAutoMove==mTG.GetAutoMove() && m_schdinfo.m_nHistoryLen==mTG.GetHistoryLen())
			{
				return;
			}
		}
	}
	mTG.SetBackgrandInfo(m_schdinfo.m_nShiftID, start, m_schdinfo.m_nAutoMove, m_schdinfo.m_nTotelLen, m_schdinfo.m_nHistoryLen);
	
	mTG.NewShiftBanci();


	mTG.BeginBatchDraw(0, "开始将Term内存中运行图转递到OCX"); 
	if(pholder->WriteSchedule(m_schdinfo.m_nType ,&m_tgmmi,TRUE))
	{
		m_tgmmi.SetCommReady(TRUE); // 允许更新
	}
	
	IScheduleComm  *pNewServer=pholder->GetServerCommByScheduleType(m_schdinfo.m_nType);
	if(NULL != pNewServer)
	{
		if(pNewServer->CommReady())
		{
			GpcLogTrainMessage(0, 0, "交接班 section=%d schdtype=%d 通信正常,由服务器返回结束驱动批量绘制", m_schdinfo.m_nSectionID, m_schdinfo.m_nType);
			//mTG.EndBatchDraw(0,"将Term内存中运行图转递到OCX完毕"); // 如果通信正常由 服务器返回结束驱动批量绘制
		}
		else
		{
			GpcLogTrainMessage(0, 0, "交接班 section=%d schdtype=%d 通信中断", m_schdinfo.m_nSectionID, m_schdinfo.m_nType);
			mTG.EndBatchDraw(0,"将Term内存中运行图转递到OCX完毕"); //
		}
	}
}

void CTgView::ShiftBanCi()
{
	CWorkScheduleHolder  *pholder=GetDocument()->GetTGDataHolder();
	if(!pholder)
		return;

	CTG_ScheduleInfo  schd_info;
	pholder->GetWorkScheduleInfo(schd_info);
	m_schdinfo.m_nShiftID = schd_info.m_nShiftID; 
	m_schdinfo.m_tStartTime  =schd_info.m_tStartTime; 
	m_schdinfo.m_tEndTime  =schd_info.m_tEndTime;

	CString cs;
	GetTitle(cs);
	GetParentFrame()->SetWindowText(cs);

	mTG.SetShiftId(m_schdinfo.m_nShiftID,0);
	short con_id = (short)mTG.GetCurrentSectionID();

	CTime bTime(m_schdinfo.m_tStartTime), eTime(m_schdinfo.m_tEndTime);
	GpcLogTrainMessage(0, 0, "交接班 section=%d schdtype=%d sttm=%s edtm=%s", 
		m_schdinfo.m_nSectionID, m_schdinfo.m_nType, bTime.Format("%Y-%m-%d %H:%M:%S"), eTime.Format("%Y-%m-%d %H:%M:%S"));

	mTG.SelectSection(con_id, 1);

	mTG.BeginBatchDraw(0, "开始将Term内存中运行图转递到OCX"); 
	if(pholder->WriteSchedule(m_schdinfo.m_nType ,&m_tgmmi,TRUE))
	{
		m_tgmmi.SetCommReady(TRUE); // 允许更新
	}
	
	IScheduleComm  *pNewServer=pholder->GetServerCommByScheduleType(m_schdinfo.m_nType);
	if(NULL!=pNewServer)
	{
		if(pNewServer->CommReady())
		{
			GpcLogTrainMessage(0, 0, "交接班 section=%d schdtype=%d 通信正常,由服务器返回结束驱动批量绘制", m_schdinfo.m_nSectionID, m_schdinfo.m_nType);
			//mTG.EndBatchDraw(0,"将Term内存中运行图转递到OCX完毕"); // 如果通信正常由 服务器返回结束驱动批量绘制
		}
		else
		{
			GpcLogTrainMessage(0, 0, "交接班 section=%d schdtype=%d 通信中断", m_schdinfo.m_nSectionID, m_schdinfo.m_nType);
			mTG.EndBatchDraw(0,"将Term内存中运行图转递到OCX完毕"); //
		}
	}
}
void CTgView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	if(lHint==TGDATA_SHIFT_SCHEDULE)
	{
		if(IS_BASIC_SCHD(m_schdinfo.m_nType))
		{
			;//基本图不交接班
		}
		else
		{
			if(gpcdef.IsNewAutoMoveMode())
				ShiftBanCiNew();
			else
				ShiftBanCi();
		}
	}
	else if(lHint==TGDATA_STATION_STATUS_CHANGE)
	{
		StationStatusInform *pinform=(StationStatusInform *)pHint;
		TG_STATION_STATUS *pstat=GetStationStatusPtr(pinform->station_no);
		if(!pstat)
			return;

		if(this->m_schdinfo.m_nType == CTG_ScheduleInfo::SCHEDULE_TYPE_WORK  || this->m_schdinfo.m_nType == CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT)
		{
			if(pstat->stpc_status==  TG_STATION_STATUS::ENTITY_ACTIVE) 
				mTG.SetStationConnectStatus(pinform->station_no,0);
			else
				mTG.SetStationConnectStatus(pinform->station_no,1);

			//mTG.SetPlanRecieveFlag(BYTE nType, ULONG id, ULONG color)
			//  nType 1: STPC 2: LIRC
			// id:  车站站号
			// color: 显示颜色
			if(pstat->lirc_status ==  TG_STATION_STATUS::ENTITY_ACTIVE) 
				mTG.SetCtrlMode(pstat->station_no,pstat->ctrl_mode,pstat->route_mode);
			else
				mTG.SetCtrlMode(pstat->station_no,255,255);
		}

		if(this->m_schdinfo.m_nType == CTG_ScheduleInfo::SCHEDULE_TYPE_WORK)
		{
			if(pstat->lirc_plan_status ==  TG_STATION_STATUS::NEED_RESEND) 
				mTG.SetPlanRecieveFlag(2,pstat->station_no,(ULONG)RGB(192,0,0));
			else if(pstat->lirc_plan_status ==  TG_STATION_STATUS::SENDING) 
				mTG.SetPlanRecieveFlag(2,pstat->station_no,(ULONG)RGB(192,0,0));
			else if(pstat->lirc_plan_status ==  TG_STATION_STATUS::SENDOK) 
				mTG.SetPlanRecieveFlag(2,pstat->station_no,(ULONG)RGB(0,255,50));
			else
				mTG.SetPlanRecieveFlag(2,pstat->station_no,(ULONG)RGB(255,255,255)); // 默认白色,不可见

			if(pstat->stpc_plan_status ==  TG_STATION_STATUS::NEED_RESEND) 
				mTG.SetPlanRecieveFlag(1,pstat->station_no,(ULONG)RGB(192,0,0));
			else if(pstat->stpc_plan_status ==  TG_STATION_STATUS::SENDING) 
				mTG.SetPlanRecieveFlag(1,pstat->station_no,(ULONG)RGB(192,0,0));
			else if(pstat->stpc_plan_status ==  TG_STATION_STATUS::SENDOK) 
				mTG.SetPlanRecieveFlag(1,pstat->station_no,(ULONG)RGB(0,255,50));
			else
				mTG.SetPlanRecieveFlag(1,pstat->station_no,(ULONG)RGB(255,255,255)); // 默认白色,不可见
		}

	}

	// 不能调用默认,否则白屏
	//else
	//return CView::OnUpdate(pSender,lHint,pHint); 
}

void CTgView::OnDCMD()
{
	long eid=GetSpecialEntityID(GPCTERM_TYPE_DCMD,0);
	if(eid<0)
	{
		MessageBox("无法查找对应的调度命令实体","提示");
		return;
	}
	CString token,content;
	GenXMLMessage_InformDCMDToFront(token,content);
	TGSendStringMessage(token,content,eid);
}

void CTgView::OnQueBao()
{
	int station;
	CTG_LogicObject *pobj=GetDocument()->GetCurrentActiveObject(&station);
	if(pobj && pobj->GetObjectType()==TG_OBJ_TRAIN)
	{
		CTG_SingleTrain *ptrain=(CTG_SingleTrain *)pobj;
		CDlgQueBaoList dlg(station,ptrain->GetTrainID(),ptrain->GetShiftTrainIndex()); 
		dlg.DoModal();
	}
}
void CTgView::OnButtonLogin()
{
	CLoginDlg dlg(this);

	CTG_ScheduleInfo::SCHEDULE_TYPE type = GetViewScheduleType();
	if(type == CTG_ScheduleInfo::SCHEDULE_TYPE_HIST)
		dlg.m_bCtrlLoginBanci = false;
	else
		dlg.m_bCtrlLoginBanci = true;
	dlg.m_nMaxPreNum= gpcdef.GetPreSchdNum(); // 允许调度员向前登录的次数

	if(dlg.DoModal() == IDOK)
	{
		m_gpcContext.mShiftID = dlg.m_nShiftID;
		GetDocument()->ShiftSchedule(dlg.m_nShiftID, dlg.m_bIgnoreLoginTm);


		// 事件处理2: 交接班
		GPCENTITY eventEntity;
		if(gpcdef.GetEntityInfo(136,0,0,eventEntity))
		{
			MsgEventData data;
			data.nSrcEntityID = GetLocalEntityID();
			data.tmEventTime = time(NULL);
			data.nEventRank = NORMAL_EVENT;

			data.nEventID =  EVENT_GPC_SHIFT_BANBIE;
			data.SetKey(KeyConId, gpcdef.GetCurrentSectNo());
			data.SetKey(KeyConName, gpcdef.GetCurUserConsoleName());
			data.SetKey(KeyUserName, gpcdef.GetCurUserName());
			data.SetKey(KeyUserId, dlg.m_userid);
			data.SetKey(KeyLoginTime, time(NULL));

			SendBMsg(&data, eventEntity.nEntityId);
		}

		GpcLogSysMessage(0,"WintgTerm 交接班登录成功. shiftId %d", dlg.m_nShiftID);
	}
	else
	{
		GpcLogSysMessage(0,"WintgTerm 调度员取消 交接班登录.");
	}

	GpcLogSysMessage(0,"WintgTerm 当前调度员 %s", gpcdef.GetCurrentUsername());
}

void CTgView::OnChangePassword()
{
	CChangePasswordDlg dlg(this);
	dlg.DoModal();
}

void CTgView::ShowCrewInfo(ULONG nTrainIndex, long x, long y)
{
	do
	{
		if(0==nTrainIndex)
			break;

		CTG_TrainScheduleBase *pData=m_tgmmi.GetDataStore(m_schdinfo);
		if(NULL==pData)
			break;

		CTG_SingleTrain *ptrain=pData->GetSingleTrain(nTrainIndex);
		if(NULL==ptrain)
			break;

		CString crew="";
		if(GetTGFunctionConfig()->IsEnableTDMS5_0())
		{
			const CTG_TrainCrewList* cw=ptrain->GetTrainInfoExpand_Const()->GetTrainCrewList_Const();
			crew=cw->toString2();
		}
		else
		{
			crew=ptrain->GetImportInfo(TRAIN_TXT_INFO_TYPE_CREW);
		}
		if(m_pCfWnd.m_strText != crew && !m_pCfWnd.m_strText.IsEmpty())
		{
			m_pCfWnd.HideTipWindow();
		}
		if(crew.IsEmpty())
		{
			m_pCfWnd.HideTipWindow();
		}
		else
		{
			m_pCfWnd.ShowTipWindow(x, y, crew);
		}
		return;
	}while(0);

	m_pCfWnd.HideTipWindow();
}

// 定义 nFlag
// 标记: bit1为1表示关联调度命令
// 运行线: bit1为1表示查询确保
void CTgView::TG_NOTICE(BYTE type, long nFlag, long nIndex,short station)
{
	CTG_TrainScheduleBase *pData=m_tgmmi.GetDataStore(m_schdinfo);
	if(!pData)
		return;

	// 标记
	if(type == 1)
	{
		CTG_SingleMark *pmark=pData->GetSingleMark(nIndex);
		if(!pmark)
			return;
		if(nFlag & 0x1)
		{
			long nEntityOfDCMD=GetSpecialEntityID(GPCTERM_TYPE_DCMD,0);
			CString token,content;
			GenXMLMessage_InformDCMDToFront(token,content);
			TGSendStringMessage(token,content,nEntityOfDCMD);
			sysprint(5,0,"向调度命令终端 %d 发送最前端XML\n %s", nEntityOfDCMD, content);

			TIME st,et;
			st=m_schdinfo.m_tStartTime;
			et=m_schdinfo.m_tEndTime;
			CString plan_id = pmark->GetShiftIndexString();
			if(plan_id == "")
				plan_id.Format("%d", nIndex);

			GenXMLMessage_SelectDCMD(token,content,st,et, plan_id.GetBuffer());
			TGSendStringMessage(token,content,nEntityOfDCMD);
		}
		else if(nFlag & 0x2)
		{
			int mplanindex=GetDocument()->FindMMPlanByMainID(pmark->GetShiftIndexString());
			if(mplanindex<0)
			{
				MessageBox("找不到对应的施工计划,需要从TDMS获取","提示");
				return;
			}
			MMplanInform inform;
			inform.id=pmark->GetShiftIndexString();
			inform.index=mplanindex;
			inform.change_flag=MMplanInform::MMPLAN_SELECT_CHANGE;

			GetDocument()->UpdateAllViews(this,TGDATA_MPLAN_UPDATED,&inform);
		}
		else if(nFlag & 0x4)
		{
			int mplanindex=GetDocument()->FindMMPlanByMainID(pmark->GetShiftIndexString());
			if(mplanindex<0)
			{
				MessageBox("找不到对应的施工计划, 确认该施工符号是否手工绘置，手工绘制无法取消施工","提示");
				return;
			}
			MMplanInform inform;
			inform.id=pmark->GetShiftIndexString();
			inform.index=mplanindex;
			inform.change_flag=MMplanInform::MMPLAN_CANCEL_WORK;

			GetDocument()->UpdateAllViews(this,TGDATA_MPLAN_UPDATED,&inform);
		}
	}
	// 运行线
	else if(type == 2)
	{
		long rec_index = (nFlag&0xFF000000)>>24;
		CTG_LogicObject  *pobj=GetDocument()->GetCurrentActiveObject();
		if(pobj && pobj->GetObjectType()==TG_OBJ_TRAIN)
		{
			CTG_SingleTrain* m_pTrain=(CTG_SingleTrain *)pobj;
			m_pTrain->SetCurSelRecIndex(rec_index);
		}
		nFlag &= 0x00FFFFFF;
		if(nFlag & 0x1)
		{
			if(station<=0)
				return;
			CTG_SingleTrain *ptrain=pData->GetSingleTrain(nIndex);
			CDlgQueBaoList dlg(station,ptrain->GetTrainID(),ptrain->GetShiftTrainIndex()); 
			dlg.DoModal();
		}
		else if(nFlag & 0x2)
		{
			CPropertySheetNotepad dlg(SHUAIGUA_PAGE,"列车作业",GetDocument(),m_schdinfo.m_nType);
			dlg.DoModal();
		}
		// 速报
		else if(nFlag & 0x10)
		{
			OnTrainSubao();
		}
		// 列车编组
		else if(nFlag & 0x20)
		{
			OnTrainBianzudan(rec_index);
		}
		// 查看机车/机组信息
		else if(nFlag & 0x40)
		{
			OnTrainEngineInfo(rec_index);
		}
	}
}

void CTgView::OnTrainEngineInfo(int rec_index)
{
	CTG_LogicObject *pobj=GetDocument()->GetCurrentActiveObject();
	if(!pobj)
		return;
	if(pobj->GetObjectType()!=TG_OBJ_TRAIN)
		return;
	
	CString strUrl=GetTGFunctionConfig()->GetGSoapURL();
	if(strUrl=="")
		return;
	CTG_SingleTrain *ptrain=(CTG_SingleTrain *)pobj;
	
	CTrainEngineInfoDlg *pdlg = new CTrainEngineInfoDlg(this); 
	if(pdlg != NULL)
	{
		//1. 设置查询参数
		pdlg->mQueryParam.entity=GetLocalEntityID();
		pdlg->mQueryParam.train_index=ptrain->GetTrainIndex();
		pdlg->mQueryParam.section=m_schdinfo.m_nSectionID;
		pdlg->mQueryParam.shift_start_time=m_schdinfo.m_tStartTime;

		//2. 显示对话框
		pdlg->Create(IDD_DIALOG_ENGINE); 
		pdlg->ShowWindow(SW_SHOW);	
	} 	
}

void CTgView::OnTrainSubao()
{
	CTG_LogicObject *pobj=GetDocument()->GetCurrentActiveObject();
	if(!pobj)
		return;
	if(pobj->GetObjectType()==TG_OBJ_TRAIN)
	{
		CTG_SingleTrain *ptrain=(CTG_SingleTrain *)pobj;

		GpcLogTrainMessage(0, 0, "调度员查看列车 %s 速报,该列车超限等级为:%d",ptrain->GetTrainID(),ptrain->GetTrainInfo()->m_data.space_rank);

		CSuBaoDlg dlg(this, ptrain->GetTrainIndex(), GetDocument(), ptrain->GetTrainID(), ptrain->GetTrainInfo()->m_data.space_rank);
		dlg.DoModal();		
	}
}

struct Gpc_QB_Data
{
	char  m_sTrain[32];
	char  m_sCurSta[32];
};

void CTgView::OnTrainBianzudan(int rec_index)
{
	CTG_LogicObject *pobj=GetDocument()->GetCurrentActiveObject();
	if(!pobj)
		return;
	if(pobj->GetObjectType()!=TG_OBJ_TRAIN)
		return;
	
	struct Gpc_QB_Data QB_data;
	memset(&QB_data,0,sizeof(QB_data));

	CTG_SingleTrain *ptrain=(CTG_SingleTrain *)pobj;
	const CTG_TrainRecord* prec = ptrain->GetTrainRecordNoDelete_Const(rec_index);
	if(prec == NULL)
		return;

	//train id
	CString strTrainId;
	strTrainId=prec->GetArriveTID();
	if(strTrainId.IsEmpty())
		strTrainId=prec->GetDepartTID();

	if(GetTGFunctionConfig()->IsQuebaoQueryHaveChinese())
	{
		// 保留汉字
	}
	else
	{
		for(int x=0;x<strTrainId.GetLength();x++)
			if(!IsCharAlphaNumeric(strTrainId.GetAt(x)))
				strTrainId.SetAt(x,0x20);
		strTrainId.Remove(0x20);
	}
	
	strncpy(QB_data.m_sTrain, strTrainId, 31);
	
	//current station
	CString stationName = gpcdef.GetStationName(prec->GetStationNo());
	strncpy(QB_data.m_sCurSta, stationName, 31);

	CFile pReadFile;	
	if(pReadFile.Open("C:\\QB_Train.txt", CFile::modeWrite|CFile::modeCreate|CFile::shareDenyNone))
	{
		pReadFile.Write(&QB_data, sizeof(QB_data));
		pReadFile.Close();
	}

	CString STI="列车编组查询系统";
	CString STIEXE=gpcdef.GetSTIPathName();
	
	GpcLogTrainMessage(0, 0, "列车编组查询 STIWinName: %s, STIExePath: %s",
		STI, STIEXE);

	CWnd *pWnd = CWnd::FindWindow(NULL,STI);
	if(NULL == pWnd)
	{
		UINT ret = ::WinExec(STIEXE,SW_SHOW);
		if(ret == ERROR_FILE_NOT_FOUND || ret == ERROR_PATH_NOT_FOUND)
		{
			CString msg;
			msg.Format("请确认STI径路是否正确 %s",STIEXE);
			AfxMessageBox(msg);
		}
	}

	CWnd *pWndPark =CWnd::FindWindow(NULL,STI);
	if(pWndPark)
	{
		HWND HWindow = pWndPark -> m_hWnd;	
		::PostMessage(HWindow,WM_USER+201,strlen(QB_data.m_sTrain),(LPARAM)0);
	}
}

void CTgView::OnNote()
{
	CPropertySheetNotepad *pdlg=NULL;
	if(GetMMIFunctionConfig()->GetConfigIntegerItem(CONFIG_COMB_NOTEPAD)==1) // 组合方式，TDCS
	{
		pdlg=new CPropertySheetNotepad("记事栏",GetDocument(),m_schdinfo.m_nType);
		pdlg->AddUserPage(PHASENOTE_PAGE);
		pdlg->AddUserPage(SUBAO_PAGE);
		pdlg->AddUserPage(PHASENOTE_SEND_PAGE);
	}
	else
	{
		pdlg=new  CPropertySheetNotepad(PHASENOTE_PAGE,"记事",GetDocument(),m_schdinfo.m_nType);
	}
	pdlg->DoModal();
	delete pdlg;
}
void CTgView::OnJiaoBan()
{
	if(GetMMIFunctionConfig()->GetConfigIntegerItem(CONFIG_JIAOBAN)!=1) // 组合方式，TDCS
	{
		MessageBox("本功能暂时不可用!","提示");
		return;
	}
	CPropertySheetNotepad dlg(JIAOBAN_PAGE,"叫班计划",GetDocument(),m_schdinfo.m_nType);
	dlg.DoModal();
}

void CTgView::OnSUBAO()
{
	if(GetMMIFunctionConfig()->GetConfigIntegerItem(CONFIG_COMB_NOTEPAD)==1) // 组合方式，TDCS
	{
		CPropertySheetNotepad *pdlg=NULL;
		if(GetMMIFunctionConfig()->GetConfigIntegerItem(CONFIG_COMB_NOTEPAD)==1) // 组合方式，TDCS
		{
			pdlg=new CPropertySheetNotepad("列车编组",GetDocument(),m_schdinfo.m_nType);
			pdlg->AddUserPage(SUBAO_PAGE);
			pdlg->AddUserPage(PHASENOTE_PAGE);
			pdlg->AddUserPage(PHASENOTE_SEND_PAGE);
		}
		else
		{
			pdlg=new  CPropertySheetNotepad(PHASENOTE_PAGE,"记事",GetDocument(),m_schdinfo.m_nType);
		}
		pdlg->DoModal();
		delete pdlg;
	}
	else
	{
		CPropertySheetNotepad dlg(SUBAO_PAGE,"列车速报",GetDocument(),m_schdinfo.m_nType);
		dlg.m_bEnableWordSelected=FALSE;
		dlg.DoModal();
	}
}
void CTgView::OnShuaiGua()
{
	CPropertySheetNotepad dlg(SHUAIGUA_PAGE,"列车作业",GetDocument(),m_schdinfo.m_nType);

	BOOL bReadOnly=FALSE;
	if(GetViewScheduleType()!=CTG_ScheduleInfo::SCHEDULE_TYPE_WORK)
		bReadOnly=TRUE;
	else
	{
		int gpcid = gpcdef.GetUserGpcId();
		RIGHT right;
		if(!m_load.GetUserRight(gpcid, right))
		{
			bReadOnly=TRUE;
		}
		if(right.operate==1 || right.operate==3)
			bReadOnly=FALSE;
		else
			bReadOnly=TRUE;
	}
	((CPropertyPage_ShuaiGua *)dlg.GetUserPage(SHUAIGUA_PAGE))->m_bReadOnly=bReadOnly;
	
	dlg.DoModal();
}
void CTgView::OnSendPhaseNotePlan()
{
	CPropertySheetNotepad dlg(PHASENOTE_SEND_PAGE,"阶段计划(记事)",GetDocument(),m_schdinfo.m_nType);
	dlg.DoModal();
}
void CTgView::OnShigu()
{
	CPropertySheetNotepad dlg(SHIGU_PAGE,"行车事故概况",GetDocument(),m_schdinfo.m_nType);
	dlg.DoModal();

}
void CTgView::OnStationCurrentTrains()
{
	CTgDocument *pdoc=GetDocument();
	CQueryPhaseStoreTrain dlg(GetTGBasicParam(), pdoc->GetCurrentSectionNo(),pdoc->GetCurrentShiftID(),pdoc->IsQueryMode() ,this);
	dlg.DoModal();
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

void CTgView::OnActivateFrame(UINT nState,CFrameWnd *pwnd)
{
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

void CTgView::OnJiaojiechestatistic()
{
	CWorkScheduleHolder  *pholder=GetDocument()->GetTGDataHolder();
	if(!pholder)
		return;

	CTG_ScheduleInfo  schd_info;
	pholder->GetWorkScheduleInfo(schd_info);
	CJiaojiecheStatisticDlg* pdlg = new CJiaojiecheStatisticDlg(GetDocument(), schd_info.m_nSectionID);
	pdlg->Create(IDD_DLG_JIAOJIECHE_STATISTIC, this);
	pdlg->ShowWindow(SW_SHOW);
}

void CTgView::OnQueryDDYName()
{
	CDlgDDYNameQuery* pdlg = new CDlgDDYNameQuery(GetDocument(), m_schdinfo.m_nSectionID, m_schdinfo.m_nShiftID, this);
	pdlg->Create(IDD_DLG_REQ_DDYNAME, this);
	pdlg->ShowWindow(SW_SHOW);
}

void CTgView::OnPredictHistroyView()
{
	CPredictHistoryDlg* pdlg=new CPredictHistoryDlg(m_schdinfo.m_nSectionID,this);
	if(pdlg)
	{
		pdlg->Create(IDD_DLG_PREDICT_HIS);
		pdlg->ShowWindow(SW_SHOW);
	}
	
	return;
}

LRESULT CTgView::OnSetTrainFoucs(WPARAM wParam, LPARAM lParam)
{
	mTG.FindTrainByTrainIndex(wParam);
	return 0;
}
