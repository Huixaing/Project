// CTGFrame.cpp : implementation file
//

#include "stdafx.h"
#include "afxpriv.h"
#include "IF.h"

#include "resource.h"

#include "tg_message_define.h"
#include "tg.h"


#include ".\timeflagview.h"
#include ".\blankview.h"
#include ".\tgform.h"
#include ".\stationview.h"
#include ".\tgframe.h"

#include ".\dlg\DlgMessageBox.h"
#include ".\dlg\FindTrainDlg.h"
#include ".\dlg\DlgFindTrainResult.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTGFrame
IMPLEMENT_DYNCREATE(CTGFrame, CFrameWnd)

CTGFrame::CTGFrame()
{
	m_bAutoMenuEnable = FALSE;
	myPCtrl = NULL;
	m_pWintgForm = NULL;
	m_waitDlg = NULL;
	m_bLoad = true;
}

CTGFrame::~CTGFrame()
{
}


BEGIN_MESSAGE_MAP(CTGFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CTGFrame)
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_WM_SYSCOMMAND()
	ON_MESSAGE(MSG_ADDNEW_TRAIN_GRAPH_END, OnAddNewTrainGraphEnd)
	ON_MESSAGE(MSG_LOAD_DATA, OnSchdRecieveStatus)
	
	

	ON_MESSAGE(MSG_SET_STATION_STATUS,OnStationStatus) 
	ON_MESSAGE(MSG_SET_CTRL_MODE, OnSetCtrlMode)
	ON_MESSAGE(MSG_SET_PLAN_STATUS, OnSetPlanStatus) 
	
	ON_MESSAGE(MSG_REFRESH_TG, OnRefreshTG)
    ON_MESSAGE(MSG_SELECT_TG_SECTION,	OnSelectSection)
	
	ON_MESSAGE(MSG_GET_PLOT_PARAM, OnGetPlotParam)
	ON_MESSAGE(MSG_GET_TG_DATA, OnGetTGData)
	
	ON_MESSAGE(MSG_ALIVE_WINDOW, OnLiveWindow)
	ON_MESSAGE(MSG_GEN_TRAIN_INDEX, OnGenTrainIndex)

	ON_MESSAGE(MSG_SET_TRAIN_FOCUS, OnMoveTrainFocus)
	
	//}}AFX_MSG_MAP

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
void CTGFrame::SetMyCtrl(CTGCtrl* ctrl)
{
	myPCtrl = ctrl;
}
CTGForm* CTGFrame::GetFormView(void)
{
	ASSERT(m_pWintgForm != NULL);
	return m_pWintgForm;
}


BOOL CTGFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext) 
{
  	int theStatus = 1;
	CRect rt;
	GetWindowRect(&rt);
	do 
	{
		CTimeFlagView* TimeView = NULL;
		CStationView* StationView = NULL;
		CBlankView* BlankView = NULL;
		if(myPCtrl->mConfigTg.m_context.IsUpAndDownTimeView())
		{
			if (!m_wndSplitterHorz.CreateStatic(this,3,2))
				break;

			if(!m_wndSplitterHorz.CreateView(0,0,RUNTIME_CLASS(CBlankView),CSize(0,0),pContext))
				break;

			if(!m_wndSplitterHorz.CreateView(0,1,RUNTIME_CLASS(CTimeFlagView),CSize(0,0),pContext))
				break;

			if(!m_wndSplitterHorz.CreateView(1,0,RUNTIME_CLASS(CStationView),CSize(0,0),pContext))//CStationView
				break;
			
			if(!m_wndSplitterHorz.CreateView(1,1,RUNTIME_CLASS(CTGForm),CSize(0,0),pContext))//CTGForm
				break;

			if(!m_wndSplitterHorz.CreateView(2,0,RUNTIME_CLASS(CBlankView),CSize(0,0),pContext))
				break;

			if(!m_wndSplitterHorz.CreateView(2,1,RUNTIME_CLASS(CTimeFlagView),CSize(0,0),pContext))
				break;
			TimeView = (CTimeFlagView*)m_wndSplitterHorz.GetPane(2, 1);
			TimeView->SetMyCtrl(myPCtrl);
			TimeView = (CTimeFlagView*)m_wndSplitterHorz.GetPane(0, 1);
			TimeView->SetMyCtrl(myPCtrl);
			StationView = (CStationView*)m_wndSplitterHorz.GetPane(1, 0);
			StationView->SetMyCtrl(myPCtrl);
			BlankView = (CBlankView*)m_wndSplitterHorz.GetPane(0, 0);
			BlankView->SetMyCtrl(myPCtrl);
			BlankView = (CBlankView*)m_wndSplitterHorz.GetPane(2, 0);
			BlankView->SetMyCtrl(myPCtrl);


			if (myPCtrl->mConfigTg.m_context.IsWideStationView())
				m_wndSplitterHorz.SetColumnInfo(0,VIEW_PORT::STATION_VIEW_WIDTH2,100);
			else
				m_wndSplitterHorz.SetColumnInfo(0,VIEW_PORT::STATION_VIEW_WIDTH1,100);
			m_wndSplitterHorz.SetRowInfo(0,VIEW_PORT::TIME_FLAG_VIEW_HEIGHT,20); 
			m_wndSplitterHorz.SetRowInfo(1,10*VIEW_PORT::TIME_FLAG_VIEW_HEIGHT,20);
			m_wndSplitterHorz.SetRowInfo(2,VIEW_PORT::TIME_FLAG_VIEW_HEIGHT,20); 
		}
		else
		{
			if (!m_wndSplitterHorz.CreateStatic(this,2,2))
				break;

			if(!m_wndSplitterHorz.CreateView(0,0,RUNTIME_CLASS(CBlankView),CSize(0,0),pContext))
				break;

			if(!m_wndSplitterHorz.CreateView(0,1,RUNTIME_CLASS(CTimeFlagView),CSize(0,0),pContext))
				break;

			if(!m_wndSplitterHorz.CreateView(1,0,RUNTIME_CLASS(CStationView),CSize(0,0),pContext))//CStationView
				break;
			
			if(!m_wndSplitterHorz.CreateView(1,1,RUNTIME_CLASS(CTGForm),CSize(0,0),pContext))//CTGForm
				break;
 
			TimeView = (CTimeFlagView*)m_wndSplitterHorz.GetPane(0, 1);
			TimeView->SetMyCtrl(myPCtrl);
			StationView = (CStationView*)m_wndSplitterHorz.GetPane(1, 0);
			StationView->SetMyCtrl(myPCtrl);
			BlankView = (CBlankView*)m_wndSplitterHorz.GetPane(0, 0);
			BlankView->SetMyCtrl(myPCtrl);

			if (myPCtrl->mConfigTg.m_context.IsWideStationView())
				m_wndSplitterHorz.SetColumnInfo(0,VIEW_PORT::STATION_VIEW_WIDTH2,100);
			else
				m_wndSplitterHorz.SetColumnInfo(0,VIEW_PORT::STATION_VIEW_WIDTH1,100);
			m_wndSplitterHorz.SetRowInfo(0,VIEW_PORT::TIME_FLAG_VIEW_HEIGHT,20);
		}

		theStatus = 0;
		
	}while(0);

	if(theStatus)
		return FALSE;
    
	m_pWintgForm = (CTGForm*)m_wndSplitterHorz.GetPane(1, 1);
	ASSERT(myPCtrl != NULL);
	m_pWintgForm->SetMyCtrl(myPCtrl);
	
	m_pWintgForm->SetMyDataManage(&myDataManage);
	myDataManage.SetMyForm(m_pWintgForm);

	return TRUE;
}

void CTGFrame::SetMyDocument(CTGDoc* pDocument)
{
	if(myPCtrl->mConfigTg.m_context.IsUpAndDownTimeView())
	{
		CBlankView* pBlankForm = (CBlankView*)m_wndSplitterHorz.GetPane(2, 0);
		pBlankForm->m_pDoc = pDocument;
		CTimeFlagView* m_pTimeForm = (CTimeFlagView*)m_wndSplitterHorz.GetPane(2, 1);
		m_pTimeForm->m_pDoc = pDocument;
	}
	
	CBlankView* pBlankForm = (CBlankView*)m_wndSplitterHorz.GetPane(0, 0);
	pBlankForm->m_pDoc = pDocument;
	CTimeFlagView* m_pTimeForm = (CTimeFlagView*)m_wndSplitterHorz.GetPane(0, 1);
	m_pTimeForm->m_pDoc = pDocument;
	CStationView* pStationForm = (CStationView*)m_wndSplitterHorz.GetPane(1, 0);
	pStationForm->m_pDoc = pDocument;
	CTGForm* pTgForm = (CTGForm*)m_wndSplitterHorz.GetPane(1, 1);
	pTgForm->m_pDoc = pDocument;
	pDocument->SetFormView(pTgForm);

	ASSERT(myPCtrl != NULL);
	pDocument->SetTgCtrlPtr(myPCtrl);
	this->myDataManage.SetMyDoc(pDocument);
}

void CTGFrame::OnSize(UINT nType, int cx, int cy)
{
	CRect rt;
	GetClientRect(&rt);
	if(rt.Height() <= 2*VIEW_PORT::TIME_FLAG_VIEW_HEIGHT+20)
		return;

	CTimeFlagView* TimeView = NULL;
	if(myPCtrl->mConfigTg.m_context.IsUpAndDownTimeView())
	{
		if (myPCtrl->mConfigTg.m_context.IsWideStationView())
			m_wndSplitterHorz.SetColumnInfo(0,VIEW_PORT::STATION_VIEW_WIDTH2,100);
		else
			m_wndSplitterHorz.SetColumnInfo(0,VIEW_PORT::STATION_VIEW_WIDTH1,100);

		m_wndSplitterHorz.SetRowInfo(0,VIEW_PORT::TIME_FLAG_VIEW_HEIGHT,20); 
		m_wndSplitterHorz.SetRowInfo(1,rt.Height()-2*VIEW_PORT::TIME_FLAG_VIEW_HEIGHT-20,20);
		m_wndSplitterHorz.SetRowInfo(2,VIEW_PORT::TIME_FLAG_VIEW_HEIGHT,20); 
	}
		
	CFrameWnd::OnSize(nType, cx, cy);
}

LRESULT CTGFrame::OnAddNewTrainGraphEnd(WPARAM wParam,LPARAM lParam)
{
	m_pWintgForm->OnDrawNewTrainEnd();
	return 0;
}

// 对于登陆到非当前班次 nflag=0
LRESULT CTGFrame::OnSelectSection(WPARAM wParam,LPARAM lParam)
{	
	int conid = (int)wParam;
	myDataManage.RemoveAllMark();
	myDataManage.RemoveAllTrain();
	m_pWintgForm->OnSelectSection(conid);
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
// CTGFrame diagnostics

#ifdef _DEBUG
void CTGFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CTGFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

void CTGFrame::OnSysCommand(UINT nID, LPARAM lParam) 
{
	if(nID == SC_CLOSE || nID == SC_TASKLIST)
	{
		return;
	}

	CFrameWnd::OnSysCommand(nID, lParam);
}

LRESULT CTGFrame::OnStationStatus(WPARAM wParam, LPARAM lParam) 
{
	CTGDoc *pDoc  = (CTGDoc*)GetActiveDocument();
	if(pDoc == NULL) 
		return TRUE;

	BOOL bNeedInvalidate = FALSE;
	for(int j=0;j < pDoc->m_nDispDistanceNum;j++)
	{
		if(pDoc->m_sDispDistance[j].station_number == (int)wParam)
		{		 	 			 				
			if(pDoc->m_sDispDistance[j].usStationStatus != (BYTE)lParam) 
				bNeedInvalidate = TRUE;
			
			if(bNeedInvalidate)
				pDoc->m_sDispDistance[j].usStationStatus = (UINT)lParam;  
		}
	}

	if(bNeedInvalidate)
	{
		pDoc->UpdateAllViews(NULL,UM_CHANGE_STATION_STATUS,NULL);
		//pDoc->sysprint(4,0, "车站:(%d)%s 状态:%d", 
		//		(int)wParam, pDoc->GetStationName((int)wParam), (UINT)lParam);
	}
	return TRUE;
}

LRESULT CTGFrame::OnSetCtrlMode(WPARAM wParam, LPARAM lParam)
{
	CTGDoc *pDoc  = (CTGDoc*)GetActiveDocument();
	if(pDoc == NULL) 
		return TRUE;
	if(!pDoc->m_bAcceptCadData)
		return TRUE;


	msgCtrlMode* ctrl_mode = (msgCtrlMode*)lParam;

	BOOL bNeedInvalidate = FALSE;
	for(int j=0;j < pDoc->m_nDispDistanceNum;j++)
	{
		if(pDoc->m_sDispDistance[j].station_number  == ctrl_mode->staId)
		{		 	 			 				
			if((pDoc->m_sDispDistance[j].nStationCtrlMode != ctrl_mode->sta_mode) 
				||(pDoc->m_sDispDistance[j].nRouteCtrlMode != ctrl_mode->route_mode))
				bNeedInvalidate = TRUE;
			
			if(bNeedInvalidate)
			{
				pDoc->m_sDispDistance[j].usLircStatus = 0;
				pDoc->m_sDispDistance[j].nRouteCtrlMode = ctrl_mode->route_mode;
				pDoc->m_sDispDistance[j].nStationCtrlMode = ctrl_mode->sta_mode;
			}
		}
	}

	if(bNeedInvalidate)
	{
		pDoc->UpdateAllViews(NULL,UM_CHANGE_STATION_STATUS,NULL);
		//pDoc->sysprint(4,0, "车站:%s 排路模式(0:图 1:存 2:人):%d 控制模式(0:分 1:中 2:站 3:非):%d", 
		//		pDoc->GetStationName(ctrl_mode->staId), ctrl_mode->route_mode, ctrl_mode->sta_mode);
	}

	return TRUE;
}

LRESULT CTGFrame::OnSetPlanStatus(WPARAM wParam, LPARAM lParam)
{
	CTGDoc *pDoc  = (CTGDoc*)GetActiveDocument();
	if(pDoc == NULL) 
		return TRUE;
	if(!pDoc->m_bAcceptCadData)
		return TRUE;


	msgPlanParam* mode = (msgPlanParam*)lParam;

	BOOL bNeedInvalidate = FALSE;
	for(int j=0;j < pDoc->m_nDispDistanceNum;j++)
	{
		if(pDoc->m_sDispDistance[j].station_number  == mode->staId)
		{		 	 	
			// STPC
			if(mode->type == 1)
			{
				if(pDoc->m_sDispDistance[j].clStpcFlag != mode->color) 
				    bNeedInvalidate = TRUE;

				pDoc->m_sDispDistance[j].clStpcFlag = mode->color;
			}
			else
			if(mode->type == 2)
			{
				if(pDoc->m_sDispDistance[j].clLircFlag != mode->color) 
				    bNeedInvalidate = TRUE;

				pDoc->m_sDispDistance[j].clLircFlag = mode->color;
			}
		}
	}

	if(bNeedInvalidate)
		pDoc->UpdateAllViews(NULL,UM_CHANGE_STATION_STATUS,NULL);

	return TRUE;
}

void  CTGFrame::SetMutiSelFlag(short flag)
{
	CTGDoc *pDoc  = (CTGDoc*)GetActiveDocument();
	if(pDoc == NULL || TG_TYPE::WORK_TG != pDoc->GetTgType()) 
		return;

	if(flag == MUTI_SEL::TRAIN && m_pWintgForm)
	{
		m_pWintgForm->OnSetSelMultiTrain();
	}
}

// set train bool show value
void CTGFrame::ShowTrainReverse(short flag)
{
    CTGDoc *pDoc  = (CTGDoc*)GetActiveDocument();
	if(pDoc == NULL) 
		return;
	if(!pDoc->m_bAcceptCadData)
		return;


	if(flag == REVERSE::SHOW_WAVE)
        pDoc->m_bStopStationWaveLine = !pDoc->m_bStopStationWaveLine;
	else if(flag == REVERSE::SHOW_REVERSE)
        pDoc->m_bShowReverse = !pDoc->m_bShowReverse;
	else if(flag == REVERSE::SHOW_ENGINE)
		pDoc->m_ptgctrl->mConfigTg.m_context.SetShowLocoNo();
	else if(flag == REVERSE::EDIT_REAL_LINE)
		pDoc->m_bEditRealLine = !pDoc->m_bEditRealLine;
	else if(flag == REVERSE::SHOW_KILO_MARK)
	{
		pDoc->m_bShowdistanceStone = !pDoc->m_bShowdistanceStone;
		pDoc->UpdateAllViews(NULL,UM_CHANGE_STATION_STATUS,NULL);
		return;
	}
	else if(flag == REVERSE::BY_PASS_FLAG)
		pDoc->m_bByPassFlag  = !pDoc->m_bByPassFlag;
	
	else if(flag == REVERSE::SHOW_CONFLICT)
	{
		pDoc->m_bShowConflict  = !pDoc->m_bShowConflict;
	}
	else if(flag == REVERSE::SHOW_ADJUST_ERROR)
	{
		pDoc->m_bShowAdjustErrorStatus  = !pDoc->m_bShowAdjustErrorStatus;
	}
	else if(flag == REVERSE::SHOW_ERROR)
	{
		pDoc->m_bShowErrorStatus  = !pDoc->m_bShowErrorStatus;
	}
	else if(flag == REVERSE::SHOW_ELEC_LOCO)
	{
		pDoc->m_nShowElecLocoFlag = !pDoc->m_nShowElecLocoFlag;
	}

	else if(flag == REVERSE::ACCEPT_TRUCK_LATE)
	{
		pDoc->m_bShowAcceptTruckLate = !pDoc->m_bShowAcceptTruckLate;
	}
	else if(flag == REVERSE::SHOW_ACCEPT_TRAIN_LATE)
	{
		pDoc->m_bShowAcceptTrainLate = !pDoc->m_bShowAcceptTrainLate;
	}
	else if(flag == REVERSE::SHOW_FAST_SLOW_TIME)
	{
		pDoc->m_bShowFastOrSlowTime = !pDoc->m_bShowFastOrSlowTime;
	}
	
	else if(flag == REVERSE::SHOW_ERR_WORK_SIDE)
	{
		pDoc->m_nShowErrWorkSideFlag = !pDoc->m_nShowErrWorkSideFlag;
	}
	else if(flag == REVERSE::SHOW_BASIC_SCHD_SIDE)
	{
		pDoc->m_nShowWorkSideFlag = !pDoc->m_nShowWorkSideFlag;
	}
	else if(flag == REVERSE::SHOW_TECH_STOP_MARK)
	{
		pDoc->m_nShowWorkTechFlag = !pDoc->m_nShowWorkTechFlag;
	}
	else if(flag == REVERSE::SHOW_ROUTE_NOT_AUTO_TRIGER_ROUND)
	{
		pDoc->m_bShowRouteNotAutoTrigerFlag=!pDoc->m_bShowRouteNotAutoTrigerFlag;
	}
	else if(flag == REVERSE::SHOW_ROUTE_TRIGER_OK_FLAG)
	{
		pDoc->m_bShowRouteTrigerOkFlag=!pDoc->m_bShowRouteTrigerOkFlag;
	}
	else if(flag == REVERSE::SHOW_SEL_CZH_LINE)
	{
		pDoc->m_bShowSelCZHLine=!pDoc->m_bShowSelCZHLine;
		if(pDoc->m_bShowSelCZHLine)
		{
			pDoc->m_bShowAllCZHLine=false;
			if(myPCtrl->mConfigTg.m_context.IsEnableNewCZH())
			{
				pDoc->sysprint(0,0, "[ShowSelCZHLine]重新生成车组关系,根据列车前序索引");
				pDoc->ReGenTrainJiaoluRelation();
			}
		}
	}
	else if(flag == REVERSE::SHIFT_SCHD_OPER)
	{
		if(myPCtrl->mConfigTg.m_context.GetShiftSchdCanModify()>0)
			myPCtrl->mConfigTg.m_context.SetShiftSchdCanModify(0);
		else
			myPCtrl->mConfigTg.m_context.SetShiftSchdCanModify(1);
	}
	else if(flag == REVERSE::SHOW_RETURN_TRAINID)
	{
		pDoc->m_bShowReturnTrainid=!pDoc->m_bShowReturnTrainid;
		if(pDoc->m_bShowReturnTrainid)
		{
			if(myPCtrl->mConfigTg.m_context.IsEnableNewCZH())
			{
				pDoc->sysprint(0,0, "[ShowAllCZHLine]重新生成车组关系,根据列车前序索引");
				pDoc->ReGenTrainJiaoluRelation();
			}
		}
	}
	else if(flag == REVERSE::SHOW_DEPART_MEET_MARK)
	{
		pDoc->m_bShowDepartFlag=!pDoc->m_bShowDepartFlag;
	}
	else if (flag == REVERSE::SHOW_VIRTUAL_STATION)
	{
		pDoc->m_bShowVirtualStation=!pDoc->m_bShowVirtualStation;
		pDoc->UpdateAllViews(NULL,UM_CHANGE_STATION_STATUS,NULL);
	}
	else if (flag == REVERSE::USE_STRING_KILO_METER)
	{
		pDoc->m_bUseStringKiloMeter=!pDoc->m_bUseStringKiloMeter;
	}
	else if(flag == REVERSE::MOVE_SAME_CZ_TRAIN)
	{
		if(pDoc->m_ptgctrl->mConfigTg.m_context.IsMoveSameCZTrain())
			pDoc->m_ptgctrl->mConfigTg.m_context.SetMoveSameCZTrain(false);
		else
			pDoc->m_ptgctrl->mConfigTg.m_context.SetMoveSameCZTrain(true);
	}
	else if (flag == REVERSE::SHOW_TRANS_INFO)
	{
		pDoc->m_bShowOPMSTrainsInfo=!pDoc->m_bShowOPMSTrainsInfo;
	}
	else if (flag == REVERSE::SHOW_ALL_CZH_LINE)
	{
		pDoc->m_bShowAllCZHLine=!pDoc->m_bShowAllCZHLine;
		if(pDoc->m_bShowAllCZHLine)
		{
			pDoc->m_bShowSelCZHLine=false;
			if(myPCtrl->mConfigTg.m_context.IsEnableNewCZH())
			{
				pDoc->sysprint(0,0, "[ShowAllCZHLine]重新生成车组关系,根据列车前序索引");
				pDoc->ReGenTrainJiaoluRelation();
			}
		}
			
		if(myPCtrl->IsWorkMode())
			m_pWintgForm->DisplyTrainByDirection();
		else
			m_pWintgForm->Redraw();//ok
		return;
	}
	else if (flag == REVERSE::SHOW_ENGINE_CREW_INFO)
	{
		pDoc->m_bShowEngineCrewMenu=true;
		return;
	}
	else if (flag == REVERSE::GOOD_TRAIN_STAY_TO_PASS_NEED_PWD)
	{
		pDoc->m_bGoodTrainNotTechStopNeedPwd=TRUE;
		return;
	}
	else if (flag == REVERSE::SHOW_PLAN_NOTE_TEXT)
	{
		pDoc->m_bShowPlanNoteText=!pDoc->m_bShowPlanNoteText;
	}
	else 
	{
		return;
	}
	
	m_pWintgForm->Redraw();
}

short CTGFrame::GetTrainReverse(short flag)
{
	CTGDoc *pDoc  = (CTGDoc*)GetActiveDocument();
	if(pDoc == NULL) 
		return -1;

	if(flag == REVERSE::SHOW_WAVE)
        return pDoc->m_bStopStationWaveLine;

	else if(flag == REVERSE::SHOW_REVERSE)
		return pDoc->m_bShowReverse;
       
	else if(flag == REVERSE::SHOW_ENGINE)
		return pDoc->m_ptgctrl->mConfigTg.m_context.GetShowLocoNo()? 1:0;

	else if(flag == REVERSE::EDIT_REAL_LINE)
		return pDoc->m_bEditRealLine;

	else if(flag == REVERSE::SHOW_KILO_MARK)
		return pDoc->m_bShowdistanceStone;
	
	else if(flag == REVERSE::BY_PASS_FLAG)
		return pDoc->m_bByPassFlag;
	
	else if(flag == REVERSE::SHOW_CONFLICT)
		return pDoc->m_bShowConflict;
	
	else if(flag == REVERSE::SHOW_ADJUST_ERROR)
		return pDoc->m_bShowAdjustErrorStatus;
	
	else if(flag == REVERSE::SHOW_ERROR)
		return pDoc->m_bShowErrorStatus;
	
	else if(flag == REVERSE::SHOW_ELEC_LOCO)
		return pDoc->m_nShowElecLocoFlag;
	
	else if(flag == REVERSE::ACCEPT_TRUCK_LATE)
		return pDoc->m_bShowAcceptTruckLate?TRUE:FALSE;
	
	else if(flag == REVERSE::SHOW_ACCEPT_TRAIN_LATE)
		return pDoc->m_bShowAcceptTrainLate?TRUE:FALSE;
	
	else if(flag == REVERSE::SHOW_FAST_SLOW_TIME)
		return pDoc->m_bShowFastOrSlowTime;
	
	else if(flag == REVERSE::SHOW_ERR_WORK_SIDE)
		return pDoc->m_nShowErrWorkSideFlag?TRUE:FALSE;
	
	else if(flag == REVERSE::SHOW_BASIC_SCHD_SIDE)
		return pDoc->m_nShowWorkSideFlag?TRUE:FALSE;
	
	else if(flag == REVERSE::SHOW_TECH_STOP_MARK)
		return pDoc->m_nShowWorkTechFlag;

	else if(flag == REVERSE::SHOW_ROUTE_NOT_AUTO_TRIGER_ROUND)
		return pDoc->m_bShowRouteNotAutoTrigerFlag;

	else if(flag == REVERSE::SHOW_ROUTE_TRIGER_OK_FLAG)
		return pDoc->m_bShowRouteTrigerOkFlag;

	else if(flag == REVERSE::SHOW_SEL_CZH_LINE)
		return pDoc->m_bShowSelCZHLine?TRUE:FALSE;

	else if(flag == REVERSE::RECEIVE_POINT)
		return (pDoc->m_nOperatorStatus == RECEIVE_POINT_STATUS)?TRUE:FALSE;
		
	else if(flag == REVERSE::ENABLE_CZH_OPER)
	{
		if(pDoc->GetOperatorRight(OPER_RIGHT::TG_EDIT))	
			return myPCtrl->mConfigTg.m_context.IsEnableCZH()? TRUE:FALSE;
		else
			return 0;
	}

	else if(flag == REVERSE::SHIFT_SCHD_OPER)
	{
		return (myPCtrl->mConfigTg.m_context.GetShiftSchdCanModify()>0)? TRUE:FALSE;
	}

	else if(flag == REVERSE::SHOW_RETURN_TRAINID)
		return pDoc->m_bShowReturnTrainid?TRUE:FALSE;
	
	else if(flag == REVERSE::SHOW_DEPART_MEET_MARK)
		return pDoc->m_bShowDepartFlag?TRUE:FALSE;

	else if(flag == REVERSE::SHOW_VIRTUAL_STATION)
		return pDoc->m_bShowVirtualStation?TRUE:FALSE;

	else if(flag == REVERSE::USE_STRING_KILO_METER)
		return pDoc->m_bUseStringKiloMeter?TRUE:FALSE;
	
	else if(flag == REVERSE::MOVE_SAME_CZ_TRAIN)
	{
		return pDoc->m_ptgctrl->mConfigTg.m_context.IsMoveSameCZTrain()?TRUE:FALSE;
	}
	else if (flag == REVERSE::SHOW_TRANS_INFO)
		return pDoc->m_bShowOPMSTrainsInfo?TRUE:FALSE;
	else if (flag == REVERSE::SHOW_ALL_CZH_LINE)
		return pDoc->m_bShowAllCZHLine?TRUE:FALSE;
	else if (flag == REVERSE::GOOD_TRAIN_STAY_TO_PASS_NEED_PWD)
		return pDoc->m_bGoodTrainNotTechStopNeedPwd;
	else if (flag == REVERSE::SHOW_PLAN_NOTE_TEXT)
		return pDoc->m_bShowPlanNoteText ? TRUE:FALSE;
	else if (flag == REVERSE::WHOLE_SCREEN_STATE)
		return pDoc->m_bShowWholeScreen ? TRUE:FALSE;
	return FALSE;
}

void CTGFrame::TrainReDraw(int nFlag)
{
	CTGDoc *pDoc = (CTGDoc *)GetActiveDocument();
	if(pDoc)
	{
		if(!pDoc->m_bAcceptCadData)
			return;


		if(nFlag == TRAIN_DISPLY::LATETRAIN)
		{
			pDoc->SetStaticTimeByCfg();
		}
		if(nFlag == TRAIN_DISPLY::LINE)
		{
			pDoc->ResetLineWidth();
		}
		m_pWintgForm->Redraw();
	}
}

void CTGFrame::KeyDown(short type)
{
	m_pWintgForm->OnKeyDown(type, 0, 0);
}

void CTGFrame::OutputTrainsRelation()
{
	CTGDoc *pDoc = (CTGDoc *)GetActiveDocument();
	if(pDoc)
	{
		if(myPCtrl->mConfigTg.m_context.IsEnableNewCZH())
		{
			pDoc->ReGenTrainJiaoluRelation();
			pDoc->myChezuhao.OutputCZRelation(); // 输出车组关系到文件
		}
		else
		{
			pDoc->myChezuhao.OutputCZRelation(); // 输出车组关系到文件
		}
	}
}

void CTGFrame::ProhibitEditRealTime(short type)
{
	CTGDoc *pDoc = (CTGDoc *)GetActiveDocument();
	if(pDoc)
	{
		if(1==type)
		{
			pDoc->m_bInhibitEditRealTime=TRUE;
		}
		else
		{
			pDoc->m_bInhibitEditRealTime=FALSE;
		}

		if(pDoc->m_bInhibitEditRealTime)
			pDoc->m_bEditRealLine=FALSE;
	}
}

// 查找列车
void CTGFrame::FindTrain()
{
	CFindTrainDlg dlg;
	if(dlg.DoModal() == IDOK)
	{
		CString strFindTrainid = dlg.m_train_id;
		DWORD train_index = dlg.m_train_index;
		CTGDoc *pDoc = (CTGDoc *)GetActiveDocument();
		if(pDoc && train_index)
		{
			long position=-1;
			// 单趟车模式查找列车
			bool bFindTrain = false;
			if(pDoc->m_bShowSingleTrain && m_pWintgForm != NULL)
			{
				myDataManage.ClearSingleTrain();
				m_pWintgForm->Redraw();
				bFindTrain = pDoc->FindTrainByTrainIndexForSingleTrain(position, train_index, strFindTrainid);
				m_pWintgForm->Redraw();
			}
			else
			{
				bFindTrain = pDoc->FindTrainByTrainIndex(position, train_index, strFindTrainid);
			}
			position = position*pDoc->m_fPixelPerMinute;
			if(bFindTrain && m_pWintgForm != NULL)
			{
				m_pWintgForm->SetScrollPosition(position, train_index);
				myPCtrl->FIRE_TG_SELECT_TRAIN(train_index, 0, 0, 0 ,0 ,0);
				m_pWintgForm->SetFocus();
			}
			else
			{
				AfxMessageBox("没有找到查询列车");
			}
		}
		else if (pDoc)
		{
			long position_list[MAX_SAME_TRAIN_ID_NUM];
			long train_list[MAX_SAME_TRAIN_ID_NUM];
			long begin_time_list[MAX_SAME_TRAIN_ID_NUM];
			int train_cnt = 0;
			// 单趟车模式查找列车
			if(pDoc->m_bShowSingleTrain && m_pWintgForm != NULL)
			{
				myDataManage.ClearSingleTrain();
				m_pWintgForm->Redraw();
				train_cnt = pDoc->FindTrainByTrainIdForSingleTrain(strFindTrainid, position_list, train_list);
				m_pWintgForm->Redraw();
			}
			else
			{
				train_cnt = pDoc->FindTrainByTrainId(strFindTrainid, position_list, train_list);
			}
			if (train_cnt == 0)
				AfxMessageBox("没有找到查询列车");
			else if (train_cnt == 1)
			{
				position_list[0] = position_list[0]*pDoc->m_fPixelPerMinute;
				MoveTrainFocusFromFindTrainResult(position_list[0], train_list[0]);
			}
			else if (train_cnt > 1)
			{
				for (int i=0;i<train_cnt;i++)
				{
					position_list[i] = position_list[i]*pDoc->m_fPixelPerMinute;		

					CTG_TRAIN* train = myDataManage.GetTrain(train_list[i]);
					TgTRAIN_RECORD* rec = train->GetFirstRecord();
					if (rec==NULL)
						return;

					if (IsFlagStart(rec->flag))
						begin_time_list[i] = rec->depart;
					else
						begin_time_list[i] = rec->arrive;
				}

				// 查找列车，基本图显示日期错误				
				CDlgFindTrainResult DlgResult(myPCtrl->GetTGType(),train_cnt, strFindTrainid, position_list, train_list, begin_time_list,this);
				DlgResult.DoModal();
			}
		}
	}
}

void CTGFrame::FindTrain(DWORD train_index)
{
	CTGDoc *pDoc = (CTGDoc *)GetActiveDocument();
	if(train_index!=0)
	{
		CString strFindTrainid;
		long position=-1;
		// 单趟车模式查找列车
		bool bFindTrain = false;
		if(pDoc->m_bShowSingleTrain && m_pWintgForm != NULL)
		{
			myDataManage.ClearSingleTrain();
			m_pWintgForm->Redraw();
			bFindTrain = pDoc->FindTrainByTrainIndexForSingleTrain(position, train_index, strFindTrainid);
			m_pWintgForm->Redraw();
		}
		else
		{
			bFindTrain = pDoc->FindTrainByTrainIndex(position, train_index, strFindTrainid);
		}
		position = position*pDoc->m_fPixelPerMinute;
		if(bFindTrain && m_pWintgForm != NULL)
		{
			m_pWintgForm->SetScrollPosition(position, train_index);
			myPCtrl->FIRE_TG_SELECT_TRAIN(train_index, 0, 0, 0 ,0 ,0);
			m_pWintgForm->SetFocus();
		}
	}
}

LRESULT CTGFrame::OnMoveTrainFocus(WPARAM wParam, LPARAM lParam)
{
	long position = (long)wParam;
	ULONG train_index = (ULONG)lParam;

	MoveTrainFocusFromFindTrainResult(position, train_index);

	return 1;
}

void CTGFrame::MoveTrainFocusFromFindTrainResult(long position, long train_index)
{
	CTGDoc *pDoc = (CTGDoc*)GetActiveDocument();
	if(pDoc)
	{
		if(m_pWintgForm != NULL)
		{
			m_pWintgForm->SetScrollPosition(position, train_index);
			myPCtrl->FIRE_TG_SELECT_TRAIN(train_index, 0, 0, 0 ,0 ,0);
			m_pWintgForm->SetFocus();
		}
	}
}

void CTGFrame::SetDeleteMultiTrainFlag()
{
	CTGDoc *pDoc  = (CTGDoc*)GetActiveDocument();
	if(pDoc == NULL) 
		return;
	if(m_pWintgForm != NULL)
	{
		m_pWintgForm->OnSetDeleteMultiTrain();
	}
}

void CTGFrame::OnReceivePoint() 
{
	CTGDoc *pDoc = (CTGDoc *)GetActiveDocument();
	if(pDoc == NULL)
		return;
    if(pDoc->m_nOperatorStatus == RECEIVE_POINT_STATUS)
	{
        pDoc->m_nOperatorStatus = NORMAL_STATUS;
		m_pWintgForm->m_bMouseDown = FALSE;
	}
	else
	{
        pDoc->m_nOperatorStatus = RECEIVE_POINT_STATUS;
	}
}

// draw special train
void CTGFrame::drawSpecialTrain(short trainType) 
{
	CTGDoc *pDoc  = (CTGDoc*)GetActiveDocument();
	if(pDoc == NULL) 
		return;
	if(!pDoc->m_bAcceptCadData)
		return;

	
	if((m_pWintgForm != NULL) && (::IsWindow(m_pWintgForm->GetSafeHwnd())))
	{
		if(TRAIN::RETURN == trainType)
		   m_pWintgForm->OnDrawReturn();
		else if(TRAIN::GOON == trainType)
		   m_pWintgForm->OnDrawGoon();
		
		else if(TRAIN::TO_ABS_TRAIN == trainType)
		   m_pWintgForm->OnDrawAbstrainA();
		else if(TRAIN::FROM_ABS_TRAIN == trainType)
		   m_pWintgForm->OnDrawAbstrainB();
	}
}

short CTGFrame::GetZoomStatus()
{
	CTGDoc *pDoc  = (CTGDoc*)GetActiveDocument();
	if(pDoc == NULL) 
		return -1;
	if (pDoc->m_fPixelPerHour == 120)
	{
		return GRAPH_ZOOM::SMALL;
	}
	if (pDoc->m_fPixelPerHour == 180)
	{
		return GRAPH_ZOOM::NORMAL;
	}
	if (pDoc->m_fPixelPerHour == 270)
	{
		return GRAPH_ZOOM::BIG;
	}
	return -1;
}

LRESULT CTGFrame::OnRefreshTG(WPARAM wParam,LPARAM lParam)
{
	CTGDoc *pDoc  = (CTGDoc*)GetActiveDocument();
	if(pDoc == NULL) 
		return TRUE;
	pDoc->sysprint(5,0,"Refresh GRAPH");

	m_pWintgForm->Redraw(); //???
	return TRUE;
}

LRESULT CTGFrame::OnSchdRecieveStatus(WPARAM wParam,LPARAM lParam)
{
	int nStatus = (int)wParam;
	CTGDoc *pDoc  = (CTGDoc*)GetActiveDocument();
	if(pDoc == NULL) 
		return TRUE;
	if(!pDoc->m_bAcceptCadData)
		return TRUE;

	
	if(nStatus == SCHD_RECIEVE_STATUS::BEGIN)
	{
		myDataManage.BeginLoad();
		myDataManage.EndLoad();
		pDoc->m_nOperatorStatus = WAIT_STATUS;
		if(m_waitDlg == NULL)
		{
			m_bLoad = true;

			m_waitDlg = new CDlgWait("正在接收数据,请稍候...", this);
		}
	}
	else if(nStatus == SCHD_RECIEVE_STATUS::END && pDoc->m_nOperatorStatus == WAIT_STATUS)
	{
		if(m_waitDlg)
		{
			delete m_waitDlg;
			m_waitDlg = NULL;

			m_bLoad = false;
		}
		pDoc->m_nOperatorStatus = NORMAL_STATUS;
		myDataManage.EndLoad();
	}
	return TRUE;
}

CString CTGFrame::OnSetTrainDefault(CTG_TRAIN& train)
{
	CTGDoc *pDoc  = (CTGDoc*)GetActiveDocument();
	if(NULL != pDoc && pDoc->m_bAcceptCadData)
	{
		pDoc->SetStationInOutIndex(train);
		return train.toXMLForExitEntry();
	}
	return "";
}

CString CTGFrame::SetTrainExitEntryByStationIndex(CTG_TRAIN& train)
{
	CTGDoc *pDoc  = (CTGDoc*)GetActiveDocument();
	if(pDoc != NULL) 
	{
		if(pDoc->m_bAcceptCadData && pDoc->CheckStationInOutIndex(train))
		{
			pDoc->GenTrainExitEntryByStationIndex(train);
			return train.toXMLForExitEntry();
		}
	}
	return "";
}

// 获取打印参数
LRESULT CTGFrame::OnGetPlotParam(WPARAM wParam, LPARAM lParam)
{
	CTGDoc *pDoc  = (CTGDoc*)GetActiveDocument();
	if(pDoc == NULL) 
		return TRUE;
	//1. 车站展开车站整理
	pDoc->GetPlotParamXml((ULONG)wParam);
	return TRUE;
}

// 用于将TG内存中的数据更新到数据库中
LRESULT CTGFrame::OnGetTGData(WPARAM wParam, LPARAM lParam)
{
	//BYTE nFlag = (BYTE)wParam;
	//CTGForm* form = GetFormView();
	//if(NULL != form)
	//{
	//	form->GetTGMemoryData(nFlag);
	//}

	return TRUE;
}

// 用于将TG内存中的数据更新到数据库中
LRESULT CTGFrame::OnLiveWindow(WPARAM wParam, LPARAM lParam)
{
	ULONG nFlag = (ULONG)wParam;
	if(nFlag == 4) // 不接收cad数据
	{
		CTGDoc *pDoc  = (CTGDoc*)GetActiveDocument();
		if(pDoc == NULL) 
			return TRUE;
		pDoc->m_bAcceptCadData=false;
	}
	else
	{
		CTGForm* form = GetFormView();
		if(NULL != form)
		{
			form->OnLiveDlgWindow(nFlag);
		}
	}
    
	return TRUE;
}

LRESULT CTGFrame::OnGenTrainIndex(WPARAM wParam, LPARAM lParam)
{
	CTGDoc *pDoc  = (CTGDoc*)GetActiveDocument();
	if(NULL == pDoc)
		return TRUE;

	if(!pDoc->m_bAcceptCadData)
		return TRUE;

	myDataManage.GenTrainIndex();
	return TRUE;
}

void CTGFrame::OnNewShiftBanci()
{
	myDataManage.RemoveAllMark();
	myDataManage.RemoveAllTrain();
	m_pWintgForm->OnNewShiftBanci();
}

CString CTGFrame::GetTrainJiaoRelation(TRAIN_INDEX train_index)
{
	TRAIN_INDEX prev_train_index=0, next_train_index=0;
	myDataManage.GetPrevAndAfterTrainIndex(train_index, prev_train_index, next_train_index);
	CString xml,str;
	str.Format("<prev_train_index>%u</prev_train_index>", prev_train_index);
	xml += str;
	str.Format("<next_train_index>%u</next_train_index>", next_train_index);
	xml += str; 
	return xml;
}
