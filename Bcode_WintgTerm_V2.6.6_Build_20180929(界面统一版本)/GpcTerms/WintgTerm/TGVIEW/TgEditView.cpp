// TestTgView.cpp : CTgEditView 类的实现
//

#include "stdafx.h"
#include "tg.h"
#include "resource.h"
#include "TgEditView.h"
#include "sysshare.h"
#include "if.h"
#include "NeibPlanFuncChoiceDlg.h"
#include "SelectScheduleTrain.h"
#include "SelectScheduleMark.h"

#include "function.h"
#include "CPropertysheetNotepad.h"
#include "ManualReportHistory.h"
#include "DlgTrainList.h"
#include "DlgSelectShift.h"
#include "AddNewTrainDlg.h"
#include ".\tgeditview.h"
#include "propertypage_phasenoteplan.h"
#include "InputHatTrain.h"
#include "ChangeTrainRoute.h"
#include "DlgNeibTrainList.h"
#include "DlgRestoreTrainList.h"
#include "tg_binxml.h"
#include "..\SuperPassDlg.h"
#include "utility\mmi_function_config.h"
#include "InsertTrainDlg.h"
#include "InsertTrainDlgLanzhou.h"
#include "DlgDividePortLateTime.h"
#include "DlgTrainRecord.h"
#include "dlgwait.h"
#include "DlgDirectSetTrigFlag.h"
#include "DlgBasicChange.h"
#include "DlgBasicParam.h"
#include "DlgDeleteTrainByIndex.h"
#include "DlgBasicDeleteName.h"
#include "..\Alarm\SoundPlay.h"

#include ".\TrainCrewTicket.h"
#include "DlgMultiAdjust.h"
#include "DlgTDMSBasicTip.h"
#include "DlgTDMSBasicGen.h"
#include "DlgConfirm.h"
#include "DlgStationReqChangeSide.h"
#include "DlgBasicTrainDeleteChoice.h"
#include "DlgBatchSideModify.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern CLoadGPCDef m_load;
extern int DB_GetEntityStatus(long eid);
// CTgEditView
BOOL UR_CanOperateBasicSchedule();
BOOL UR_CanOperateSchedule();
IMPLEMENT_DYNCREATE(CTgEditView, CTgView)

BEGIN_EVENTSINK_MAP(CTgEditView, CTgView)
	ON_EVENT(CTgEditView, IDC_TGCTRL_VIEW, 1, APPEND_MARK, VTS_I4 VTS_BSTR)
	ON_EVENT(CTgEditView, IDC_TGCTRL_VIEW, 2, UPDATE_MARK, VTS_I4 VTS_BSTR)
	ON_EVENT(CTgEditView, IDC_TGCTRL_VIEW, 3, DELETE_MARK, VTS_I4 VTS_BSTR)
	ON_EVENT(CTgEditView, IDC_TGCTRL_VIEW, 4, APPEND_TRAIN, VTS_I4 VTS_BSTR)
	ON_EVENT(CTgEditView, IDC_TGCTRL_VIEW, 5, UPDATE_TRAIN, VTS_I4 VTS_BSTR)
	ON_EVENT(CTgEditView, IDC_TGCTRL_VIEW, 6, TG_DELETE_TRAIN, VTS_I4 VTS_BSTR)
	ON_EVENT(CTgEditView, IDC_TGCTRL_VIEW, 7, TG_LOG, VTS_BSTR)
	ON_EVENT(CTgEditView, IDC_TGCTRL_VIEW, 8, SELECT_TRAIN, VTS_I4 VTS_I4 VTS_I2 VTS_I2 VTS_I4 VTS_I4)
	ON_EVENT(CTgEditView, IDC_TGCTRL_VIEW, 9, SELECT_MARK, VTS_I4 VTS_I1 VTS_I1 VTS_I2 VTS_I2 VTS_I4)
	ON_EVENT(CTgEditView, IDC_TGCTRL_VIEW, 10, TG_NOTICE, VTS_I1 VTS_I4 VTS_I4 VTS_I2)
	
	ON_EVENT(CTgEditView, IDC_TGCTRL_VIEW, 14, TG_FIRE_DELETE, VTS_BSTR)
	ON_EVENT(CTgEditView, IDC_TGCTRL_VIEW, 16, SetLateEarlyTimeRd, VTS_BSTR VTS_I1 VTS_I4)
	ON_EVENT(CTgEditView, IDC_TGCTRL_VIEW, 18, MutiTrainOper, VTS_BSTR VTS_I4) 
END_EVENTSINK_MAP()
// CTgEditView 构造/析构
BEGIN_MESSAGE_MAP(CTgEditView, CTgView)
	ON_COMMAND(ID_TEMP_ADD_TRAIN,OnAddTmpTrain)
	ON_COMMAND(ID_INSERT_NEW_TRAIN, OnInsertNewTrain)
	ON_COMMAND(ID_DRAW_NEW_TRAIN_END, OnEndInsertNewTrain)
	ON_COMMAND(ID_MANUAL_ADD_NEW_TRAIN, OnManualAddNewTrain)
	ON_COMMAND(IDC_DEL_MULTI_TRAIN, OnDelMultiTrain)
	ON_COMMAND(ID_ABS_INHIBIT_G, OnAbsInhibitG)
	ON_COMMAND(ID_ABS_SLOW_G, OnAbsSlowG)
	ON_COMMAND(ID_ABS_ELE_FIX_G, OnAbsEleFixG)
	ON_COMMAND(ID_ABS_FIXED, OnAbsFixG)
	ON_COMMAND(ID_SELF_CLOSE_G, OnSelfCloseG)
	ON_COMMAND(ID_DRAW_GOON, OnDrawGoon)
	ON_COMMAND(ID_DRAW_RETURN, OnDrawReturn)
	ON_COMMAND(ID_DRAW_ABSTRAIN_A, OnDrawAbstrainA)
	ON_COMMAND(ID_DRAW_ABSTRAIN_B, OnDrawAbstrainB)

	ON_COMMAND(ID_SIDE_INHIBIT_G, OnSideInhibitG)
	ON_COMMAND(ID_STATION_SLOW_G, OnStationSlowG)
	ON_COMMAND(ID_SIDE_TIANCHUANG_G, OnSideTianchuangG)
	ON_COMMAND(ID_SIDE_OCCUPY, OnSideOccupy)
	ON_COMMAND(ID_SIDE_LOST_ELE, OnSideLostEle)
	ON_COMMAND(ID_TEXT_NOTE, OnTextNote)
	ON_COMMAND(ID_JISHI_ZHUJIE, OnJishiZhujie)
	ON_COMMAND(ID_UNITE_TRAIN, OnUniteTrain)
	ON_COMMAND(ID_RESCUE_TRAIN, OnRescueTrain)
	ON_COMMAND(ID_SPLIT_TRAIN, OnSplitTrain)
	ON_COMMAND(ID_COMBINE_TRAIN, OnCombineTrain)
	ON_COMMAND(ID_ABS_PART_RUN, OnAbsPartRun)
	ON_COMMAND(ID_ABS_WHOLE_HELP_RUN, OnAbsWholeHelpRun)
	
	ON_COMMAND(ID_ABS_RETURN_RUN, OnAttainEngineReturn)

	ON_COMMAND(ID_MANUAL_SELECT_BASIC_TRAIN, OnManualSelectBasicTrain)
	ON_COMMAND(ID_MANUAL_SELECT_SHIFT_TRAIN, OnManualSelectShiftTrain)
	ON_COMMAND(ID_MANUAL_SELECT_TRAIN, OnManualSelectTrain)

	ON_COMMAND(ID_MANUAL_SELECT_SHIFT_MARK,OnManualSelectShiftMark)
	ON_COMMAND(ID_MANUAL_SELECT_BASIC_MARK,OnManualSelectBasicMark)
	ON_COMMAND(IDC_SEND_STATION_PLAN, OnSendStationPlan)

	ON_COMMAND(ID_MANUAL_REPORT_HISTORY, OnManualReportHistory)
	ON_COMMAND(ID_SHIFT_GET_TRAIN_FROM_OPMS,OnGetTrainList)
	ON_COMMAND(ID_BASIC_GET_TRAIN_FROM_OPMS,OnGetBasicTrainFromOpms)
	ON_COMMAND(ID_SHIFT_GET_MARK_FROM_OPMS,OnGetMarkList)
	ON_COMMAND(ID_WORK_GET_TRAIN_FROM_OPMS,OnRecvTrainList)
	ON_COMMAND(ID_WORK_GET_MARK_FROM_OPMS,OnRecvMarkList)
	ON_COMMAND(ID_DELETE_TRAIN, OnDeleteTrain)
	ON_WM_DESTROY()
	ON_COMMAND(ID_GET_ENGINE_TICKET, OnGetEngineTicketInfo)
	ON_UPDATE_COMMAND_UI(ID_GET_ENGINE_TICKET, OnUpdateGetEngineTicketInfo)
	ON_COMMAND(ID_SHIFT_GET_SHIFTINFO, OnShiftGetShiftinfo)
	ON_UPDATE_COMMAND_UI(ID_SHIFT_GET_SHIFTINFO, OnUpdateShiftGetShiftinfo)
	ON_COMMAND(ID_HAT_TRAIN, OnHatTrain)
	ON_COMMAND(ID_ELEC_ARMS_SET, OnElecArmsSet)
	ON_UPDATE_COMMAND_UI(ID_ELEC_ARMS_SET, OnUpdateElecArmsSet)
	ON_COMMAND(ID_MANUAL_OPER_CHECK, OnManualOperCheck)
	ON_UPDATE_COMMAND_UI(ID_MANUAL_OPER_CHECK, OnUpdateManualOperCheck)


	ON_UPDATE_COMMAND_UI(ID_BASIC_GET_TRAIN_FROM_OPMS,OnUpdateOperTGTrain)

	ON_UPDATE_COMMAND_UI(ID_INSERT_NEW_TRAIN,OnUpdateOperTGTrain)
	ON_UPDATE_COMMAND_UI(ID_DRAW_NEW_TRAIN_END,OnUpdateOperTGTrain)
	ON_UPDATE_COMMAND_UI(ID_MANUAL_ADD_NEW_TRAIN,OnUpdateOperTGTrain)
	ON_UPDATE_COMMAND_UI(IDC_DEL_MULTI_TRAIN,OnUpdateOperTGTrain)
	ON_UPDATE_COMMAND_UI(ID_UNITE_TRAIN,OnUpdateOperTGTrain)
	ON_UPDATE_COMMAND_UI(ID_RESCUE_TRAIN,OnUpdateOperTGTrain)
	ON_UPDATE_COMMAND_UI(ID_SPLIT_TRAIN,OnUpdateOperTGTrain)
	ON_UPDATE_COMMAND_UI(ID_MANUAL_SELECT_BASIC_TRAIN,OnUpdateOperTGTrain)
	ON_UPDATE_COMMAND_UI(ID_MANUAL_SELECT_SHIFT_TRAIN,OnUpdateOperTGTrain)
	ON_UPDATE_COMMAND_UI(ID_MANUAL_SELECT_TRAIN, OnUpdateOperTGTrain)

	ON_UPDATE_COMMAND_UI(ID_MANUAL_REPORT_HISTORY,OnUpdateOperTGTrain)
	ON_UPDATE_COMMAND_UI(ID_HAT_TRAIN,OnUpdateOperTGTrain)
	ON_UPDATE_COMMAND_UI(ID_COMBINE_TRAIN,OnUpdateOperTGTrain)
	ON_UPDATE_COMMAND_UI(IDC_SEND_STATION_PLAN,OnUpdateSendPlan)
	ON_UPDATE_COMMAND_UI(IDC_SEND_NEIGHBER,OnUpdateSendPlan)

	ON_COMMAND(IDC_RICEIVE_PLAN_1, OnNeighberPlan)
	ON_UPDATE_COMMAND_UI(IDC_RICEIVE_PLAN_1,OnUpdateOperTGTrain)

	ON_COMMAND(IDC_SEND_NEIGHBER, OnSendNeighber)

	ON_COMMAND(ID_SEND_NEIGBOUR_PLAN, OnSendNeigbourPlan)
	ON_UPDATE_COMMAND_UI(ID_SEND_NEIGBOUR_PLAN,OnUpdateOperTGTrain)

	ON_COMMAND(IDC_RICEIVE_NEIGB_PLAN, OnRecvNeigbourPlan)
	ON_UPDATE_COMMAND_UI(IDC_RICEIVE_NEIGB_PLAN,OnUpdateOperTGTrain)

	ON_COMMAND(ID_TURN_REALITY, OnTurnReality)
	ON_UPDATE_COMMAND_UI(ID_TURN_REALITY,OnUpdateOperTGTrain)
	ON_UPDATE_COMMAND_UI(ID_DELETE_TRAIN,OnUpdateOperTGTrain)
	ON_UPDATE_COMMAND_UI(ID_MUTI_TRAIN_MOVE,OnUpdateOperTGTrain)

	ON_UPDATE_COMMAND_UI(ID_ABS_INHIBIT_G,OnUpdateOperTGMark)
	ON_UPDATE_COMMAND_UI(ID_ABS_SLOW_G,OnUpdateOperTGMark)
	ON_UPDATE_COMMAND_UI(ID_SELF_CLOSE_G,OnUpdateOperTGMark)
	ON_UPDATE_COMMAND_UI(ID_ABS_ELE_FIX_G,OnUpdateOperTGMark)
	ON_UPDATE_COMMAND_UI(ID_ABS_FIXED,OnUpdateOperTGMark)
	ON_UPDATE_COMMAND_UI(ID_DRAW_GOON,OnUpdateOperAddWorkTrain)
	ON_UPDATE_COMMAND_UI(ID_DRAW_RETURN,OnUpdateOperAddWorkTrain)
	ON_UPDATE_COMMAND_UI(ID_DRAW_ABSTRAIN_A,OnUpdateOperAddWorkTrain)
	ON_UPDATE_COMMAND_UI(ID_DRAW_ABSTRAIN_B,OnUpdateOperAddWorkTrain)

	ON_UPDATE_COMMAND_UI(ID_SIDE_INHIBIT_G,OnUpdateOperTGMark)
	ON_UPDATE_COMMAND_UI(ID_STATION_SLOW_G,OnUpdateOperTGMark)
	ON_UPDATE_COMMAND_UI(ID_SIDE_TIANCHUANG_G,OnUpdateOperTGMark)
	ON_UPDATE_COMMAND_UI(ID_SIDE_OCCUPY,OnUpdateOperTGMark)
	ON_UPDATE_COMMAND_UI(ID_SIDE_LOST_ELE,OnUpdateOperTGMark)
	ON_UPDATE_COMMAND_UI(ID_TEXT_NOTE,OnUpdateOperTGMark)
	ON_UPDATE_COMMAND_UI(ID_JISHI_ZHUJIE,OnUpdateOperTGMark)
	ON_UPDATE_COMMAND_UI(ID_MANUAL_SELECT_SHIFT_MARK,OnUpdateOperTGMark)
	ON_UPDATE_COMMAND_UI(ID_MANUAL_SELECT_BASIC_MARK,OnUpdateOperTGMark)

	ON_UPDATE_COMMAND_UI(ID_SHIFT_GET_MARK_FROM_OPMS,OnUpdateOperTGOPMS)
	ON_UPDATE_COMMAND_UI(ID_SHIFT_GET_TRAIN_FROM_OPMS,OnUpdateOperTGOPMS)
	ON_UPDATE_COMMAND_UI(ID_WORK_GET_TRAIN_FROM_OPMS,OnUpdateOperTGOPMS)
	ON_UPDATE_COMMAND_UI(ID_WORK_GET_MARK_FROM_OPMS,OnUpdateOperTGOPMS)
	ON_UPDATE_COMMAND_UI(ID_SHIFT_GET_SHIFTINFO,OnUpdateOperTGOPMS)
	ON_UPDATE_COMMAND_UI(ID_SHIFT_DELETE_ALL_TRAIN,OnUpdateOperTGOPMS)
	ON_UPDATE_COMMAND_UI(ID_SHIFT_DELETE_ALL_MARK,OnUpdateOperTGOPMS)

	ON_COMMAND(ID_SHIFT_DELETE_ALL_TRAIN, OnDeleteAllShiftTrain)
	ON_COMMAND(ID_SHIFT_DELETE_ALL_MARK, OnDeleteAllShiftMark)

	ON_COMMAND(ID_BACK_OPER, OnBackOper)
	ON_UPDATE_COMMAND_UI(ID_BACK_OPER,OnUpdateBackOper)

	ON_COMMAND(ID_CHANGE_TRAINROUTE, OnChangeTrainRoute)
	ON_UPDATE_COMMAND_UI(ID_CHANGE_TRAINROUTE,OnUpdateOperTGTrain)
	ON_COMMAND(ID_SYS_SAVE_TO_FILES,OnSaveToFiles)
	ON_COMMAND(ID_SYS_LOAD_FROM_FILES,OnLoadFromFiles)	

	ON_UPDATE_COMMAND_UI(ID_SYS_LOAD_FROM_FILES,OnUpdateOperRestore)

	ON_MESSAGE(UM_UNI_MESSAGE, OnCommMsg)

	ON_UPDATE_COMMAND_UI(ID_BASIC_DELETE_ALL,OnUpdateDeleteBasic)
	ON_COMMAND(ID_BASIC_DELETE_ALL, OnDeleteAllBasic)

	ON_UPDATE_COMMAND_UI(ID_SAVE_SIDE_WORK,OnUpdateSaveBasicSideWorkInfo)
	ON_COMMAND(ID_SAVE_SIDE_WORK, OnSaveBasicSideWorkInfo)

	ON_UPDATE_COMMAND_UI(ID_LOAD_BASIC_SIDE_WORK,OnUpdateOperTGTrain)
	ON_COMMAND(ID_LOAD_BASIC_SIDE_WORK, OnLoadBasicSideWorkInfo)


	ON_UPDATE_COMMAND_UI(ID_BASIC_GRAPHSAVETOTEXT,OnUpdateOperTGTrain)
	ON_COMMAND(ID_BASIC_GRAPHSAVETOTEXT, OnBasicGraphsavetotext)

	ON_COMMAND(ID_ENABLE_FREE_ADDTRAIN,OnEnableFreeAddTrain)
	ON_UPDATE_COMMAND_UI(ID_ENABLE_FREE_ADDTRAIN,OnUpdateEnableFreeAddTrain)

	ON_COMMAND(ID_GEN_TG_NODE_INDEX,OnGenNodeIndex)
	ON_UPDATE_COMMAND_UI(ID_GEN_TG_NODE_INDEX,OnUpdateGenNodeIndex)

	ON_COMMAND(ID_DIRECT_UPDATE_FROM_TDMS,OnDirectUpdateFromOPMS)
	ON_UPDATE_COMMAND_UI(ID_DIRECT_UPDATE_FROM_TDMS,OnUpdateOperTGTrain)

	ON_COMMAND(ID_MUTI_TRAIN_MOVE, OnMutiTrainMove)

	ON_COMMAND(ID_BASIC_NAMEING, OnBasicNameing)
	ON_UPDATE_COMMAND_UI(ID_BASIC_NAMEING, OnUpdateBasicNameing)

	ON_COMMAND(ID_BASIC_DELETENAME, OnBasicDeleteName)
	ON_UPDATE_COMMAND_UI(ID_BASIC_DELETENAME, OnUpdateBasicDeleteName)

    ON_COMMAND(ID_MODIFY_BSCHD, OnModifyBschd)
	ON_UPDATE_COMMAND_UI(ID_MODIFY_BSCHD, OnUpdateModifyBschd)

	ON_COMMAND(ID_TRAINPLAN_SIDE_MAN, OnTrainPlanSideMan)
	ON_UPDATE_COMMAND_UI(ID_TRAINPLAN_SIDE_MAN, OnUpdateTrainPlanSideMan)

	ON_COMMAND(ID_TRAINPLAN_ROUTE_MAN, OnTrainPlanRouteMan)
	ON_UPDATE_COMMAND_UI(ID_TRAINPLAN_ROUTE_MAN, OnUpdateTrainPlanRouteMan)

	ON_COMMAND(ID_BASIC_GRAPHCHANGE, OnBasicGraphchange)
	ON_UPDATE_COMMAND_UI(ID_BASIC_GRAPHCHANGE, OnUpdateBasicGraphchange)

	ON_COMMAND(ID_DIVIDEPORT_LATETIME, OnDivideportLatetime)
	ON_UPDATE_COMMAND_UI(ID_DIVIDEPORT_LATETIME,OnUpdateOperTGTrain)


	ON_COMMAND(ID_DELETE_MARK, OnDeleteMark)
	ON_COMMAND(ID_DELETE_TRAIN_BY_INDEX, OnDeleteTrainByIndex)
	ON_COMMAND(ID_DELETE_INVALID_TRAIN, OnDeleteInvalidTrain)

	ON_COMMAND(ID_AUTO_ADJUST_COL, OnAutoAdjustCol)
	ON_UPDATE_COMMAND_UI(ID_AUTO_ADJUST_COL,OnUpdateOperTGTrain)
	ON_COMMAND(ID_AUTO_ADJUST_PASS, OnAutoAdjustPass)
	ON_UPDATE_COMMAND_UI(ID_AUTO_ADJUST_PASS, OnUpdateOperTGTrain)
	ON_COMMAND(ID_AUTO_ADJUST_MULTI, OnAutoAdjustMulti)
	ON_UPDATE_COMMAND_UI(ID_AUTO_ADJUST_MULTI, OnUpdateOperTGTrain)
	ON_COMMAND(ID_OPT_AUTO_ADJUST_COLLISION, OnOptAutoAdjustCollision)
	ON_UPDATE_COMMAND_UI(ID_OPT_AUTO_ADJUST_COLLISION, OnUpdateOptAutoAdjustCollision)

	ON_WM_KEYDOWN()
	ON_COMMAND(ID_DIRECT_SET_AUTOTRIG, OnDirectSetAutotrig)
	ON_UPDATE_COMMAND_UI(ID_DIRECT_SET_AUTOTRIG, OnUpdateOperTGTrain)

	ON_COMMAND(ID_PREDICT_STPC, OnPredictStpc)
	ON_WM_TIMER()

	ON_COMMAND(IDC_RECEIVE_POINT, OnReceivePoint)
	ON_UPDATE_COMMAND_UI(IDC_RECEIVE_POINT, OnUpdateReceivePoint)

	ON_COMMAND(ID_EDIT_ALL_CZH, OnEditAllCzh)
	ON_UPDATE_COMMAND_UI(ID_EDIT_ALL_CZH, OnUpdateEditAllCzh)

	ON_COMMAND(ID_GEN_ALL_RETURN_TRAINID, OnGenAllReturnTrainid)
	ON_UPDATE_COMMAND_UI(ID_GEN_ALL_RETURN_TRAINID, OnUpdateGenAllReturnTrainid)

	ON_COMMAND(ID_MODIFY_SHIFT_SCHD, OnModifyShiftschd)
	ON_UPDATE_COMMAND_UI(ID_MODIFY_SHIFT_SCHD, OnUpdateModifyShiftschd)

	ON_COMMAND(ID_SET_MULTI_SIDE_LOCK, OnSetMultiSideLock)
	ON_UPDATE_COMMAND_UI(ID_SET_MULTI_SIDE_LOCK, OnUpdateSetMultiSideLock)

	ON_COMMAND(ID_QUICK_MODIFY_MULTI_TRAINS, OnQuickModifyMultiTrains)
	ON_UPDATE_COMMAND_UI(ID_QUICK_MODIFY_MULTI_TRAINS,OnUpdateOperTGTrain)

	ON_COMMAND(ID_SEL_TDMS_TRAIN_GEN_BASIC, OnSelTdmsTrainsGenBasic)
	ON_UPDATE_COMMAND_UI(ID_SEL_TDMS_TRAIN_GEN_BASIC,OnUpdateSelTdmsTrainsGenBasic)
	
	ON_COMMAND(ID_SHIFT_GENTRAIN_TO_WORK, OnShiftTrainToWork)
	ON_UPDATE_COMMAND_UI(ID_SHIFT_GENTRAIN_TO_WORK,OnUpdateShiftTrainToWork)

	ON_COMMAND(ID_BASIC_GENTRAIN_TO_WORK, OnBasicTrainToWork)
	ON_UPDATE_COMMAND_UI(ID_BASIC_GENTRAIN_TO_WORK,OnUpdateBasicTrainToWork)

	ON_COMMAND(ID_TRAINPLAN_COMPARE, OnTrainPlanCompareMan)
	ON_UPDATE_COMMAND_UI(ID_TRAINPLAN_COMPARE, OnUpdateTrainPlanCompareMan)
	
	ON_COMMAND(ID_BATCH_ALLOCATION_SIDE, OnBatchAllocationSide)
	ON_UPDATE_COMMAND_UI(ID_BATCH_ALLOCATION_SIDE, OnUpdateBatchAllocationSide)
END_MESSAGE_MAP()

CTG_TrainScheduleBase* CTgEditView::GetTrainScheduleBase()
{
	CWorkScheduleHolder *pHolder=GetDocument()->GetTGDataHolder();
	CTG_TrainScheduleBase *pData=NULL;
	if(GetViewScheduleType()==CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT)
	{
		pData=pHolder->GetShiftScheduleData(); 
		GpcLogTrainMessage(0,0,"更新日班图TrainExpand");
	}
	else if(GetViewScheduleType()==CTG_ScheduleInfo::SCHEDULE_TYPE_WORK)
	{
		pData=pHolder->GetWorkScheduleData(); 
		GpcLogTrainMessage(0,0,"更新调整图TrainExpand");
	}

	return pData;
}

BOOL  CTgEditView::ProcessXML_Kediao(const char *xml, const char *node)
{
	if(!GetTGFunctionConfig()->IsEnableTDMS5_0())
		return TRUE;

	if(GetViewScheduleType()!=CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT 
		&& GetViewScheduleType()!=CTG_ScheduleInfo::SCHEDULE_TYPE_WORK)
		return FALSE;
	
	CTG_TrainKediaoInfo kd;
	if(!TD_GetKediaoInfoFromXML(kd,xml,node))
		return FALSE;
	GpcLogTrainMessage(0,0,"CTgEditView type %d 收到TD返回 客调信息 node:%s msgid:%d", GetViewScheduleType(), node, kd.msgid);
	if((GetViewScheduleType()==CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT && 1==kd.msgid)
		||(GetViewScheduleType()==CTG_ScheduleInfo::SCHEDULE_TYPE_WORK && 2==kd.msgid))
	{
		CTG_TrainScheduleBase* pData=GetTrainScheduleBase();
		if(pData)
		{
			CTG_SingleTrain* ptrain=pData->GetTrainForUpdateExtent(kd.gtid);
			if(ptrain)
			{
				if(!ptrain->GetTrainInfoExpand_Const()->IsTrainKediaoSame(kd))
				{
					CTG_SingleTrain train=*ptrain;
					train.GetTrainInfoExpand()->UpdateTrainKediao(kd);
					
					CTG_ScheduleInfo schdinfo;	
					pData->GetScheduleInfo(schdinfo); 
					pData->UpdateTrain(schdinfo, train, SYCFLAG_NULL);
				}
			}
		}
	}		
	return TRUE;
}

BOOL  CTgEditView::ProcessXML_Yingye(const char *xml, const char *node)
{
	if(!GetTGFunctionConfig()->IsEnableTDMS5_0())
		return TRUE;

	if(GetViewScheduleType()!=CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT 
		&& GetViewScheduleType()!=CTG_ScheduleInfo::SCHEDULE_TYPE_WORK)
		return FALSE;
	
	CTG_TrainYingyeList yy;
	if(!TD_GetYingyeInfoFromXML(yy,xml,node))
		return FALSE;

	GpcLogTrainMessage(0,0,"CTgEditView type %d 收到TD返回 营业信息 node:%s msgid:%d", GetViewScheduleType(), node, yy.msgid);

	if((GetViewScheduleType()==CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT && 1==yy.msgid)
		||(GetViewScheduleType()==CTG_ScheduleInfo::SCHEDULE_TYPE_WORK && 2==yy.msgid))
	{
		CTG_TrainScheduleBase* pData=GetTrainScheduleBase();
		if(pData)
		{
			CTG_SingleTrain* ptrain=pData->GetTrainForUpdateExtent(yy.gtid);
			if(ptrain)
			{
				if(!ptrain->GetTrainInfoExpand_Const()->IsTrainYingyeSame(yy))
				{
					CTG_SingleTrain train=*ptrain;
					train.GetTrainInfoExpand()->UpdateTrainYingye(yy);
					
					CTG_ScheduleInfo schdinfo;	
					pData->GetScheduleInfo(schdinfo); 
					pData->UpdateTrain(schdinfo, train, SYCFLAG_NULL);
				}
			}
		}
	}
	return TRUE;
}

BOOL  CTgEditView::ProcessXML_Crew(const char *xml,const char *node)
{
	if(!GetTGFunctionConfig()->IsEnableTDMS5_0())
		return TRUE;

	if(GetViewScheduleType()!=CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT 
		&& GetViewScheduleType()!=CTG_ScheduleInfo::SCHEDULE_TYPE_WORK)
		return FALSE;

	CTG_TrainCrewList crew;
	if(!TD_GetCrewInfoFromXML(crew,xml,node))
		return FALSE;

	GpcLogTrainMessage(0,0,"CTgEditView type %d 收到TD返回 乘务信息 node:%s msgid:%d", GetViewScheduleType(), node, crew.msgid);

	if((GetViewScheduleType()==CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT && 1==crew.msgid)
		||(GetViewScheduleType()==CTG_ScheduleInfo::SCHEDULE_TYPE_WORK && 2==crew.msgid))
	{
		CTG_TrainScheduleBase* pData=GetTrainScheduleBase();
		if(pData)
		{
			CTG_SingleTrain* ptrain=pData->GetTrainForUpdateExtent(crew.gtid);
			if(ptrain)
			{
				if(!ptrain->GetTrainInfoExpand_Const()->IsTrainCrewSame(crew))
				{
					CTG_SingleTrain train=*ptrain;
					train.GetTrainInfoExpand()->UpdateTrainCrew(crew);
					
					CTG_ScheduleInfo schdinfo;	
					pData->GetScheduleInfo(schdinfo); 
					pData->UpdateTrain(schdinfo, train, SYCFLAG_NULL);
				}
			}
		}
	}
	return TRUE;
}

BOOL  CTgEditView::ProcessXML_Ticket(const char *xml,const char *node)
{
	if(!GetTGFunctionConfig()->IsEnableTDMS5_0())
		return TRUE;

	if(GetViewScheduleType()!=CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT 
		&& GetViewScheduleType()!=CTG_ScheduleInfo::SCHEDULE_TYPE_WORK)
		return FALSE;

	CTG_TrainTicketList tk;
	if(!TD_GetTicketInfoFromXML(tk,xml,node))
		return FALSE;
	
	GpcLogTrainMessage(0,0,"CTgEditView type %d 收到TD返回 客票信息 node:%s msgid:%d", GetViewScheduleType(), node, tk.msgid);
	if((GetViewScheduleType()==CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT && 1==tk.msgid)
		||(GetViewScheduleType()==CTG_ScheduleInfo::SCHEDULE_TYPE_WORK && 2==tk.msgid))
	{
		CTG_TrainScheduleBase* pData=GetTrainScheduleBase();
		if(pData)
		{
			CTG_SingleTrain* ptrain=pData->GetTrainForUpdateExtent(tk.gtid);
			if(ptrain)
			{
				if(!ptrain->GetTrainInfoExpand_Const()->IsTrainTicketSame(tk))
				{
					CTG_SingleTrain train=*ptrain;
					train.GetTrainInfoExpand()->UpdateTrainTicket(tk);
					
					CTG_ScheduleInfo schdinfo;	
					pData->GetScheduleInfo(schdinfo); 
					pData->UpdateTrain(schdinfo, train, SYCFLAG_NULL);
				}
			}
		}
	}
	return TRUE;
}

BOOL  CTgEditView::ProcessXML_TrainEngineCrew(const char *xml,const char *node)
{
	if(!GetTGFunctionConfig()->IsEnableTDMS5_0())
		return TRUE;

	if(GetViewScheduleType()!=CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT 
		&& GetViewScheduleType()!=CTG_ScheduleInfo::SCHEDULE_TYPE_WORK)
		return FALSE;
	
	// 车次信息
	CTG_EngineCrewList engine_crew;
	int opms_seq=0,opms_count=0;
	if(!TD_LoadEngineCrewInfoFromXML(engine_crew,xml,node,&opms_seq,&opms_count))
		return FALSE;
	
	CString waitstr;
	waitstr.Format("正在接收TDMS日班计划车次 %s(%u), %d/%d, msgid:%d",engine_crew.trainid,engine_crew.gtid,
		opms_seq,opms_count, engine_crew.msgid); 
	GpcLogTrainMessage(0,0,"%s",waitstr);
	
	if((GetViewScheduleType()==CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT && 1==engine_crew.msgid)
		||(GetViewScheduleType()==CTG_ScheduleInfo::SCHEDULE_TYPE_WORK && 2==engine_crew.msgid))
	{
		CTG_TrainScheduleBase* pData=GetTrainScheduleBase();
		if(pData)
		{
			CTG_SingleTrain* ptrain=pData->GetTrainForUpdateExtent(engine_crew.gtid);
			if(ptrain)
			{
				if(!ptrain->GetTrainInfoExpand_Const()->IsTrainEngineCrewSame(engine_crew))
				{
					CTG_SingleTrain train=*ptrain;
					train.GetTrainInfoExpand()->UpdateTrainEngineCrew(engine_crew);
					
					CTG_ScheduleInfo schdinfo;	
					pData->GetScheduleInfo(schdinfo); 
					pData->UpdateTrain(schdinfo, train, SYCFLAG_NULL);
				}
			}
		}
	}
	return TRUE;
}

LRESULT CTgEditView::OnCommMsg(WPARAM wParam, LPARAM lParam) 
{
	UNI_MESSAGE_STRUCT *pMsgStruct=( UNI_MESSAGE_STRUCT *)lParam;
	CLBTH::BaseMsg  *pBase =  ( CLBTH::BaseMsg *)pMsgStruct->pmsg;
	if(pMsgStruct->msg_id == MSGID_MsgStringMessage)
	{
		CLBTH::MsgStringMessage *pMsg= (CLBTH::MsgStringMessage  *)pBase;

		const char *token=pMsg->GetToken();
		const char *content=pMsg->GetContent();
		if(token && content)
		{
			if(!stricmp(token,TOKEN_19_ACK_BASICSCHEDULE))
			{
				ProcessXML_BasicScheduleTrain(content,token);
			}
			if(	!stricmp(token,TOKEN_3_ACK_YL_BY_TIME))
			{
				ProcessXML_ShiftTrain(content,token);
			}
			else if(!stricmp(token,TOKEN_15_ACK_KEDIAO_INFO))
			{
				if(GetTGFunctionConfig()->IsEnableTDMS5_0()) //TD5.0
					ProcessXML_Kediao(content,token);
				else
					ProcessXML_YingyeKediao(content,token);
			}
			else if(!stricmp(token,TOKEN_16_ACK_YINGYE_INFO))
			{
				if(GetTGFunctionConfig()->IsEnableTDMS5_0()) //TD5.0
					ProcessXML_Yingye(content,token);
				else
					ProcessXML_YingyeKediao(content,token);
			}
			else if(!stricmp(token,TOKEN_20_ACK_CREW_INFO))
			{
				if(GetTGFunctionConfig()->IsEnableTDMS5_0()) //TD5.0
					ProcessXML_Crew(content,token);
				else
					ProcessXML_TrainCrew(content,token);
			}
			else if(!stricmp(token,TOKEN_30_ACK_PASSENGER_TICKET_INFO))
			{
				if(GetTGFunctionConfig()->IsEnableTDMS5_0()) //TD5.0
					ProcessXML_Ticket(content,token);
			}
			else if(!stricmp(token,TOKEN_31_ACK_ENGINE_CREW_INFO))
			{
				if(GetTGFunctionConfig()->IsEnableTDMS5_0()) //TD5.0
					ProcessXML_TrainEngineCrew(content,token);
			}
			else if(!stricmp(token,"ALERT"))
			{
				ProcessXML_Alert(content, token);
			}
		}
	}
	else if (pMsgStruct->msg_id == MSGID_MsgParamentReply || pMsgStruct->msg_id == MSGID_MsgParamentOperation)
	{
		long this_entity=gpcdef.GetThisEntity();
		CLBTH::MsgParamentReply *pServerReply = NULL;
		if (pMsgStruct->msg_id == MSGID_MsgParamentReply)
			pServerReply = ( CLBTH::MsgParamentReply *)pMsgStruct->pmsg;
		else 
		{
			CLBTH::MsgParamentOperation* tmp = (CLBTH::MsgParamentOperation*) pMsgStruct->pmsg;
			if (tmp->oper == CLBTH::MsgParamentOperation::PARAOPER_INSERT_UPDATE)
				pServerReply = tmp;
		}

		if(NULL != pServerReply && this_entity==pServerReply->entity)
		{
			char *token,*ptr;
			char seps[]=" ,\t\\/=\n";
			GpcLogSysMessage(0,"服务器返回权限设置:");
			for (int i=0; i<pServerReply->type_count; i++)
			{
				if (pServerReply->type[i] == (int)CLBTH::MsgParamentReply::WINTG && pServerReply->scope==gpcdef.GetDefaultSection())
				{
					if (pServerReply->int_parament[i] & 0x0020)
					{ // 服务器有权限修改实际图－－－运行图无限制
						gpcdef.SetManualConvertActualTime(false);
						sysprint(0,PMTG.ThreadId,"MsgParamentReply, 服务器返回有修改实际点权限, 运行图不限制调度员修改实际点");
					}
					else
					{ // 服务器无权限修改实际图－－－运行图 限制+时间
						gpcdef.SetManualConvertActualTime(true);
						sysprint(0,PMTG.ThreadId,"MsgParamentReply, 服务器返回没有修改实际点权限,运行图 限制+时间");
					}
					
					int modify_time=0;
					ptr=pServerReply->str_parament[i];
					token = strtok(ptr, seps);
					if(token!=NULL)
					{
						modify_time=(int)atoi(token); 
					}

				
					GpcLogSysMessage(0,"设置允许修改实际点时间长度:%d", modify_time);
					mTG.SetTrainDisplayFlag(TRAIN_DISPLY::RECIEVE_POINT_LIMIT_TIME, modify_time);
					gpcdef.SetManualConvertActualTimeLength(modify_time);

					if(gpcdef.IsInhibitManualConvertActualTime())
					{
						mTG.SetTrainDisplayFlag(TRAIN_DISPLY::EDIT_REAL_TIME, 1);
						mTG.SetTrainDisplayFlag(TRAIN_DISPLY::SET_LIMIT_MODIFY_ACT_TIME, 1);

						m_gpcContext.mReverseFlag[1][15]=0;
						GpcLogSysMessage(0,"限制修改实际点");
					}
					else
					{
						mTG.SetTrainDisplayFlag(TRAIN_DISPLY::EDIT_REAL_TIME, 0);
						mTG.SetTrainDisplayFlag(TRAIN_DISPLY::SET_LIMIT_MODIFY_ACT_TIME, 0);
						GpcLogSysMessage(0,"允许修改实际点");
					}
					if (pMsgStruct->msg_id == MSGID_MsgParamentOperation)
					{
						AfxMessageBox("用户操作权限变更!");
					}

				}
				else if (pServerReply->scope==0 && pServerReply->entity==0)
				{	
					;
				}
				if (pServerReply->type[i] == (int)CLBTH::MsgParamentReply::MAINTN)
				{	
					if (pServerReply->int_parament[i] == 0x0001)
					{
						gpcdef.SetRightManangRight();
					}
				}
			}
		}
	}
	else if(pMsgStruct->msg_id == MSGID_MsgChangeSideReq)
	{
		long source_id=pMsgStruct->source_id;
		MsgChangeSideAck ackmsg;
		ackmsg.result = 1;
		SendBMsg(&ackmsg, source_id);
		GpcLogTrainMessage(0,0, "收到车站股道修改请求,车站实体 %u", source_id);
		CLBTH::MsgChangeSideReq *pChangeSideReq = ( CLBTH::MsgChangeSideReq *)pMsgStruct->pmsg;
		if(pChangeSideReq)
		{
			int station=pChangeSideReq->station;
			GpcLogTrainMessage(0,0, "开始解析车站 %d 股道修改请求：", station);
			OnDisplaySideReqChangeDlg(pChangeSideReq, station);
		}
		else
		{
			GpcLogTrainMessage(0,0, "解析车站股道修改请求,失败");
		}
	}
	else if(pMsgStruct->msg_id == MSGID_MsgElecArmSet)
	{
		CLBTH::MsgElecArmSet* pArm=(CLBTH::MsgElecArmSet *)pMsgStruct->pmsg;
		GpcLogTrainMessage(0,0,"接收%d发来的供电设置: arm_id %d sta %d set_entity %d set_time %d status %X",
				pMsgStruct->source_id,pArm->elec_arm_id, pArm->elec_sta, pArm->set_entity, pArm->set_time, pArm->status);
		if(GetTGFunctionConfig()->IsEnableScadaElecArm())
		{
			if(pArm->status==CLBTH::MsgElecArmSet::PREP_ENABLE
				|| pArm->status==CLBTH::MsgElecArmSet::PREP_DISABLE)
			{
				OnElecArmsSet();
				m_DlgArms.ReceiveMsgElecArmSet(pArm);
			}
		}
	}

	FREE_UNI_MESSAGE_STRUCT(pMsgStruct);
	return 0;
}

void CTgEditView::ReqParament()
{
	long this_entity=gpcdef.GetThisEntity();
	CLBTH::MsgParamentRequest msg;

	msg.type_count = 2;
	msg.type[0] = CLBTH::MsgParamentRequest::MAINTN;
	msg.type[1] = CLBTH::MsgParamentRequest::WINTG;
	msg.entity = this_entity;
	msg.scope = gpcdef.GetDefaultSection();

	long  server_id=GetSpecialEntityID(SERVER_TYPE_PARA);
	if(server_id<=0)  
	{
		GpcLogTrainMessage(0,0,"[REQ_PARAMENT]: err can't get svr SERVER_TYPE_PARA entity");
	}
	else
	{
		GpcLogTrainMessage(0,0,"[REQ_PARAMENT]:svr_entity:%d entity:%d section:%d", server_id, msg.entity, msg.scope);
		for(int i=0; i<msg.type_count; i++)
		{
			GpcLogTrainMessage(0,0,"[REQ_PARAMENT]:type[%d]:%d", i, msg.type[i]);
		}
		SendBMsg(&msg, server_id);
	}
}


extern BOOL  CheckAndLaunchMemoryNotify();
extern DWORD AddToMemoryNotify(const char *desc,time_t begintime,int repeat,int type,DWORD objindex, int flag);
#define  GET_NODE_STRVALUE(dest,pXMLDoc,node) \
{MSXML::IXMLDOMNodePtr pNode = pXMLDoc->selectSingleNode(_bstr_t(node)); \
	if(pNode) \
{ \
	_variant_t v; \
	v = pNode->GetnodeTypedValue(); \
	dest = CString((v.bstrVal)); \
	SysFreeString(v.bstrVal); \
	v.Clear(); \
} \
			else dest.Empty();};
BOOL CTgEditView::ProcessXML_Alert(const char *xml,const char *node)
{
	if(GetViewScheduleType()!=CTG_ScheduleInfo::SCHEDULE_TYPE_WORK)
		return FALSE;
	GpcLogTrainMessage(0, 0, "报警:%s", xml);

	MSXML::IXMLDOMDocumentPtr pXMLDoc;
	HRESULT hr = pXMLDoc.CreateInstance(MSXML::CLSID_DOMDocument);
	if(FAILED(hr))
	{
		_com_error er(hr);
		AfxMessageBox(er.ErrorMessage());
		return FALSE;
	}

	if(VARIANT_TRUE != pXMLDoc->loadXML(_bstr_t( xml )))
	{
		return FALSE;
	}
	
	MSXML::IXMLDOMNodePtr pRoot1 = pXMLDoc->selectSingleNode( "ALERT");
	if(pRoot1)
	{
		MSXML::IXMLDOMNodePtr pAlert = pRoot1->selectSingleNode( "OVER_WORK_ALERT");
		if(pAlert)
		{
			CString str;
			GET_NODE_STRVALUE(str, pAlert, "@conid");
			int conid=atoi(str);
			GET_NODE_STRVALUE(str, pAlert, "@train_index");
			long train_index=atol(str);
			GET_NODE_STRVALUE(str, pAlert, "@flag");
			long flag=atol(str);
			if(conid==m_schdinfo.m_nSectionID)
			{
				CString strStartTm, strWorkTm, strTrainId;
				GET_NODE_STRVALUE(strStartTm, pAlert, "@start_time");
				GET_NODE_STRVALUE(strWorkTm, pAlert, "@standard_work_time");
				GET_NODE_STRVALUE(strTrainId, pAlert, "@train_id");
				
				//1. 处理报警
				CWorkScheduleHolder *pHolder=GetDocument()->GetTGDataHolder();
				CTG_TrainScheduleBase *pData=pHolder->GetWorkScheduleData(); 
				if(pData)
				{
					CTG_SingleTrain* ptrain=pData->GetSingleTrain(train_index);
					if(ptrain)
					{
						int nCount=ptrain->GetTrainRecordCount();
						CTG_TrainRecord* prec=ptrain->GetTrainRecordNoDelete(nCount-1);
						if(prec)
						{
							if(prec->IsActualArriveTimeRecord() || prec->IsActualDepartTimeRecord())
							{
								GpcLogTrainMessage(0,0,"获取尾站, 存在实际点,忽略报警");
							}
							else
							{
								long tt=atol(strWorkTm);
								CString desc;
								desc.Format("超劳 %d 小时报警,车次:%s\t上岗时间:%s 标准工作时长:%d时%d分", 
										flag, ptrain->GetTrainID(),strStartTm, tt/60, tt%60);
							
								CTime tm=CTime::GetCurrentTime();
								AddToMemoryNotify(desc,tm.GetTime(),1,0,0,flag); // flag=1 or 3
								CheckAndLaunchMemoryNotify();
							}
						}
						else
						{
							GpcLogTrainMessage(0,0,"找到列车,但是获取尾站失败,忽略报警");
						}
					}
					else
					{
						GpcLogTrainMessage(0,0,"按照ALERT索引 %d 找不到列车, 忽略报警", train_index);
					}
				}
				
				//2. 回执报警---2015-01-16陈振解提出不发回执
				//CString token, xml;
				//token="ACK_ALERT";
				//xml.Format("<ACK_ALERT> <OVER_WORK_ALERT conid=\"%d\" train_index=\"%d\" flag=\"%d\" /> </ACK_ALERT>", 
				//	conid, train_index, flag);
				
				// 获取报警服务entity
				//long  eid=GetSpecialEntityID(SERVER_TYPE_MGATE, conid);
				//if(eid>0)
				//{
				//	GpcLogTrainMessage(0,0,"向报警服务e%d 发送回执token:%s XML:%s", eid, token, xml);
				//	TGSendStringMessage(token,xml,eid);
				//}
				//else
				//{
				//	GpcLogTrainMessage(0,0,"[Error]向报警服务发送回执失败,找不到报警服务实体 token:%s XML:%s", token, xml);
				//}
			}
		}
	}
	return TRUE;
}

CTgEditView::CTgEditView()
{
	m_pSendPlanDlg=NULL;
	m_pNotepadDlg=NULL;
	m_predictDlg=NULL;
	m_pTrainSideDlg=NULL;
	m_pTrainRouteDlg=NULL;
	m_pWorkSchdCompareDlg=NULL;
	m_pShiftSchdCompareDlg=NULL;
	m_pBasicSchdCompareDlg=NULL;
	m_pPredictServer = NULL;

	m_currentOper.nOperStatus=OPER_STATUS_IDLE;
	m_bUserMemDC=TRUE;
	m_bEnableProcessTdmsBasicInfo=false;
	m_bEnableProcessTdmsShiftInfo=false;
	m_bSelTDMSTrainGenBasic=false;
	m_bEnableFreeAddTrain=TRUE;
	m_earlylatetimelog.clear();
}

CTgEditView::~CTgEditView()
{
	if(m_pTrainSideDlg)
		delete m_pTrainSideDlg;
	m_pTrainSideDlg=NULL;

	if(m_pTrainRouteDlg)
		delete m_pTrainRouteDlg;
	m_pTrainRouteDlg=NULL;

	if(m_pWorkSchdCompareDlg)
		delete m_pWorkSchdCompareDlg;
	m_pWorkSchdCompareDlg=NULL;
	
	if(m_pShiftSchdCompareDlg)
		delete m_pShiftSchdCompareDlg;
	m_pShiftSchdCompareDlg=NULL;

	if(m_pBasicSchdCompareDlg)
		delete m_pBasicSchdCompareDlg;
	m_pBasicSchdCompareDlg=NULL;
}


void CTgEditView::OnDestroy() 
{
	UnRegisterMsgProcHandleWnd(GetSafeHwnd(),MSGID_MsgStringMessage);

	if(m_pNotepadDlg)  // 阶段记事和阶段计划合并为一个窗口
	{
		delete m_pNotepadDlg;
		m_pNotepadDlg=NULL;
		m_pSendPlanDlg=NULL;  
	}
	if(m_pSendPlanDlg)
	{
		delete m_pSendPlanDlg;
		m_pSendPlanDlg=NULL;
		GetDocument()->GetSTPLAN()->SetPlanMMIWindow(NULL);
	}
	if(m_predictDlg)
	{
		delete m_predictDlg;
		m_predictDlg=NULL;
	}

	if(GetViewScheduleType()==CTG_ScheduleInfo::SCHEDULE_TYPE_WORK) // 只有在调整图中才需要下述对话框
	{
		m_DlgArms.DestroyWindow(); 
	}
}
void CTgEditView::OnInitialUpdate()
{
	CTgView::OnInitialUpdate();
}

BOOL  CTgEditView::InitScheduleView(CTG_ScheduleInfo::SCHEDULE_TYPE type,const char *datapath,BOOL query_mode)
{
	if(type==CTG_ScheduleInfo::SCHEDULE_TYPE_WORK)
	{	
		RegisterMsgProcHandleWnd(GetSafeHwnd(),MSGID_MsgParamentReply,0);
		RegisterMsgProcHandleWnd(GetSafeHwnd(),MSGID_MsgParamentOperation,0);

		RegisterMsgProcHandleWnd(GetSafeHwnd(),MSGID_MsgChangeSideReq,0);
		if(GetTGFunctionConfig()->IsEnableScadaElecArm())
			RegisterMsgProcHandleWnd(GetSafeHwnd(),MSGID_MsgElecArmSet,0);
		
		ReqParament();
	}
	if(!CTgView::InitScheduleView(type,datapath,query_mode))
		return FALSE;

	if(GetTGFunctionConfig()->IsGoodTrainNotTechStopNeedPwd())
		mTG.SetTrainDisplayFlag(TRAIN_DISPLY::REVERSE, REVERSE::GOOD_TRAIN_STAY_TO_PASS_NEED_PWD);
	
	// 根据配置设置禁止修改实际点
	if(gpcdef.IsInhibitManualConvertActualTime())
	{
		mTG.SetTrainDisplayFlag(TRAIN_DISPLY::EDIT_REAL_TIME, 1);
		mTG.SetTrainDisplayFlag(TRAIN_DISPLY::SET_LIMIT_MODIFY_ACT_TIME, 1);
	}
	else
	{
		mTG.SetTrainDisplayFlag(TRAIN_DISPLY::SET_LIMIT_MODIFY_ACT_TIME, 0);
	}

	// 设置TG.OCX 中的权限，从TView中转移出来
	LoadEarlylatetimeXML();

	RIGHT right;
	int gpcid = gpcdef.GetUserGpcId();
	if(!m_load.GetUserRight(gpcid, right))
	{
		return FALSE;
	}

	if(!CheckUserTGRight())
	{
		right.train=0;
		right.mark=0;
		right.app=0;
		right.operate=0;
	}
	mTG.SetLineCtrlWord(right.train);
	mTG.SetMarkCtrlWord(right.mark);
	mTG.SetAppCtrlWord(right.app);
	mTG.SetOperateRight(right.operate);

	if(GetViewScheduleType()==CTG_ScheduleInfo::SCHEDULE_TYPE_WORK) // 只有在调整图中才需要下述对话框
	{
		if(GetTGFunctionConfig()->IsSupportInterCityStPlan())
		{
			if(IsSendPlanToStation(right.wintg_operate))	
				mTG.SetTrainDisplayFlag(TRAIN_DISPLY::INTER_CITY_STPLAN, 1);
			else
				mTG.SetTrainDisplayFlag(TRAIN_DISPLY::INTER_CITY_STPLAN, 0);
		}
		m_pNotepadDlg=new CPropertySheetNotepad("阶段计划下达",GetDocument(),m_schdinfo.m_nType);
		m_pNotepadDlg->m_bEnableStPlanWaring=TRUE;
		m_pNotepadDlg->m_bModaless=TRUE;
		m_pNotepadDlg->AddUserPage(STPLAN_PAGE); 
		m_pNotepadDlg->AddUserPage(PHASENOTE_SEND_PAGE); 
		m_pNotepadDlg->Create(this,WS_SYSMENU | WS_POPUP | WS_CAPTION | DS_MODALFRAME | DS_CONTEXTHELP);

		m_pSendPlanDlg=(CSendStationPlanDlgEx *)m_pNotepadDlg->GetUserPage(STPLAN_PAGE);
		GetDocument()->GetSTPLAN()->SetTgMMI(&m_tgmmi); 
		GetDocument()->GetSTPLAN()->SetPlanMMIWindow(m_pSendPlanDlg->GetSafeHwnd());   

		m_DlgArms.Create(CDlgElecArms::IDD, this);

		if(gpcdef.IsEnablePredict())
		{
			m_predictDlg=new CDepartPredictDlg();
			m_predictDlg->SetPredictInfo(GetDocument(), m_schdinfo.m_nShiftID, m_schdinfo.m_nSectionID);
			m_predictDlg->Create(CDepartPredictDlg::IDD, this);

			m_pPredictServer = GetDocument()->GetTrainDepartPredictHold();
			SetTimer(198311, 60*1000, NULL);
		}
		// 2018.7.23将指针置到tgmmi中
		m_tgmmi.SetPredictHolder(m_pPredictServer);

		int bstation_list[129];
		int count=gpcdef.GetBasicPlanStation(m_schdinfo.m_nSectionID, bstation_list);
		for(int n=0; n<count; n++)
		{
			mTG.AddBStplanStation(bstation_list[n]);
		}
	}

	if(GetViewScheduleType()==CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT) // 获取车次
		RegisterMsgProcHandleWnd(GetSafeHwnd(),MSGID_MsgStringMessage,0);
	if(GetViewScheduleType()==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_PREV) // 获取车次
		RegisterMsgProcHandleWnd(GetSafeHwnd(),MSGID_MsgStringMessage,0);
	if(GetViewScheduleType()==CTG_ScheduleInfo::SCHEDULE_TYPE_WORK) // 获取乘务信息
		RegisterMsgProcHandleWnd(GetSafeHwnd(),MSGID_MsgStringMessage,0);

	m_bEnableFreeAddTrain=GetMMIFunctionConfig()->GetConfigIntegerItem(CONFIG_FREE_ADDTRAIN)>0?TRUE:FALSE;
	m_tgmmi.EnableFreeTrainOper(m_bEnableFreeAddTrain);
	if(m_bEnableFreeAddTrain)
		mTG.SetContrlRight(0);
	else
		mTG.SetContrlRight(1);

	if(GetMMIFunctionConfig()->GetConfigIntegerItem(CONFIG_MOVE_SAME_CZ_AFTER_TRAIN)>0)
		mTG.SetTrainDisplayFlag(TRAIN_DISPLY::SET_MOVE_SAME_CZ_TRAIN, 1);
	else
		mTG.SetTrainDisplayFlag(TRAIN_DISPLY::SET_MOVE_SAME_CZ_TRAIN, 0);

	if(GetTGFunctionConfig()->IsDisenableSheetTab())
		mTG.SetTrainDisplayFlag(TRAIN_DISPLY::DISENABLE_SHEET_TAB, 1);
	else
		mTG.SetTrainDisplayFlag(TRAIN_DISPLY::DISENABLE_SHEET_TAB, 0);
	
	if(!IS_BASIC_SCHD(GetViewScheduleType()))
	{
		int stationlist[32];
		int nNum=GetTGFunctionConfig()->GetNotThisSectionStation(m_schdinfo.m_nSectionID, stationlist, sizeof(stationlist)/sizeof(stationlist[0]));
		for(int i=0; i<nNum; i++)
			mTG.SetTrainDisplayFlag(TRAIN_DISPLY::SET_NOT_THIS_SECTION_STATION,  stationlist[i]);
	}

	if(!IS_BASIC_SCHD(GetViewScheduleType()) && GetTGFunctionConfig()->HasNoLimitStation())
	{
		int stationlist[64];
		int nNum=GetTGFunctionConfig()->GetNoLimitStationList(m_schdinfo.m_nSectionID, stationlist, sizeof(stationlist)/sizeof(stationlist[0]));
		for(int i=0; i<nNum; i++)
			mTG.SetTrainDisplayFlag(TRAIN_DISPLY::SET_NO_LIMIT_STATION, stationlist[i]);
	}
	
	if(gpcdef.GetOperatorPrior4Basic() && CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_PREV==GetViewScheduleType() && gpcdef.GetUserIdentify() == GPC_TEST)
		m_pTrainSideDlg=new CDlgTrainRecord(CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_PREV, this);
	else if(CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT==GetViewScheduleType())
		m_pTrainSideDlg=new CDlgTrainRecord(CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT, this);
	else
		m_pTrainSideDlg=NULL;

	if(m_pTrainSideDlg)
	{
		m_pTrainSideDlg->Create(CDlgTrainRecord::IDD, this);
	}

	if(gpcdef.GetOperatorPrior4Basic() && CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_PREV==GetViewScheduleType() && gpcdef.GetUserIdentify() == GPC_TEST)
		m_pTrainRouteDlg=new CDlgTrainRouteCheck(this);
	else if(CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT==GetViewScheduleType())
		m_pTrainRouteDlg=new CDlgTrainRouteCheck(this);
	else
		m_pTrainRouteDlg=NULL;

	if(m_pTrainRouteDlg)
	{
		m_pTrainRouteDlg->Create(CDlgTrainRouteCheck::IDD, this);
	}

	if(gpcdef.GetOperatorPrior4Basic() && CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_PREV==GetViewScheduleType() && gpcdef.GetUserIdentify() == GPC_TEST)
	{
		m_pBasicSchdCompareDlg=new CDlgBasicSchduleCompare(GetDocument()->GetTGDataHolder(), this);
		if(m_pBasicSchdCompareDlg)
		{
			m_pBasicSchdCompareDlg->Create(CDlgBasicSchduleCompare::IDD, this);
		}
	}
	else if(CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT==GetViewScheduleType())
	{
		m_pShiftSchdCompareDlg=new CDlgShiftSchduleCompare(GetDocument()->GetTGDataHolder(), this);
		if(m_pShiftSchdCompareDlg)
		{
			m_pShiftSchdCompareDlg->Create(CDlgShiftSchduleCompare::IDD, this);
		}
	}
	else if(CTG_ScheduleInfo::SCHEDULE_TYPE_WORK==GetViewScheduleType())
	{
		m_pWorkSchdCompareDlg=new CDlgWorkSchduleCompare(GetDocument()->GetTGDataHolder(), this);
		if(m_pWorkSchdCompareDlg)
		{
			m_pWorkSchdCompareDlg->Create(CDlgWorkSchduleCompare::IDD, this);
		}
	}


	// 允许锁定股道
	if(GetTGFunctionConfig()->IsEnableLockWholeTrainSide())
		mTG.SetTrainDisplayFlag(TRAIN_DISPLY::LOCK_SIDE, 1);
	else
		mTG.SetTrainDisplayFlag(TRAIN_DISPLY::LOCK_SIDE, 0);

	// 封闭运行图,禁止删除运行线
	if(GetTGFunctionConfig()->ProhibitDeleteRecordWhileLimitModifyActual())
		mTG.SetTrainDisplayFlag(TRAIN_DISPLY::NOT_DELETE_REAL_TIME, 1);
	else
		mTG.SetTrainDisplayFlag(TRAIN_DISPLY::NOT_DELETE_REAL_TIME, 0);

	return TRUE;
}
// CTgEditView 诊断
#ifdef _DEBUG
void CTgEditView::AssertValid() const
{
	CView::AssertValid();
}

void CTgEditView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

#endif //_DEBUG

// 下面是事件响应
void CTgEditView::APPEND_MARK(long nIndex, LPCTSTR xml)
{
	m_tgmmi.ProcessMMIOperation(TG_OBJ_MARK,IScheduleComm::TG_OPER_ADD,nIndex,xml);
}

void CTgEditView::UPDATE_MARK(long nIndex, LPCTSTR xml)
{
	m_tgmmi.ProcessMMIOperation(TG_OBJ_MARK,IScheduleComm::TG_OPER_UPDATE,nIndex,xml);
}

void CTgEditView::DELETE_MARK(long nIndex, LPCTSTR xml)
{
	m_tgmmi.ProcessMMIOperation(TG_OBJ_MARK,IScheduleComm::TG_OPER_DELETE,nIndex,xml);
}

void CTgEditView::APPEND_TRAIN(long nIndex, LPCTSTR xml)
{
	m_tgmmi.ProcessMMIOperation(TG_OBJ_TRAIN,IScheduleComm::TG_OPER_ADD,nIndex,xml);
}

void CTgEditView::UPDATE_TRAIN(long nIndex, LPCTSTR xml)
{
	m_tgmmi.ProcessMMIOperation(TG_OBJ_TRAIN,IScheduleComm::TG_OPER_UPDATE,nIndex,xml);
}

void CTgEditView::TG_DELETE_TRAIN(long nIndex, LPCTSTR xml)
{
	m_tgmmi.ProcessMMIOperation(TG_OBJ_TRAIN,IScheduleComm::TG_OPER_DELETE,nIndex,xml);
}
void CTgEditView::SELECT_TRAIN(long train_index, long nFlag,short stano1, short stano2, long  date, long time2)
{
	TIME curtm=time2;

	TIME cctm=time(NULL);
	if(m_currentOper.nOperStatus==OPER_STATUS_PASSWAY)
	{
		if(IS_BASIC_SCHD(GetViewScheduleType()))
		{
			MessageBox("基本图不支持此操作","提示");
			m_currentOper.nOperStatus=OPER_STATUS_IDLE;
			return;
		}

		if(GetTGFunctionConfig()->IsEnableInputEngineId())
		{
			CDWordArray  aryStatus;
			CStringArray aryEngineId;
			aryEngineId.RemoveAll();
			for(int i=0; i<m_currentOper.nEngineCount; i++)
			{
				aryEngineId.Add(m_currentOper.engineId[i]);
				aryStatus.Add(m_currentOper.status[i]);
			}
			m_tgmmi.AddTrain_Passway(m_currentOper.bSelSecret, m_currentOper.szTrainNo,aryEngineId, aryStatus, m_currentOper.nRouteNo,m_currentOper.nStartMode,m_currentOper.nEndMode,curtm,m_currentOper.nRunType);		
		}
		else
			m_tgmmi.AddTrain_Passway(m_currentOper.bSelSecret, m_currentOper.szTrainNo,m_currentOper.szArriveTrainNo, m_currentOper.nRouteNo,m_currentOper.nStartMode,m_currentOper.nEndMode,curtm,m_currentOper.nRunType);		
		m_currentOper.nOperStatus=OPER_STATUS_IDLE;
	}
	else
	{
		CTgView::SELECT_TRAIN(train_index,nFlag,stano1,stano2,date,time2);
	}
}
void CTgEditView::SELECT_MARK(long mark_index, char type, char flag, short  stano1, short  stano2,long param)
{
	CTgView::SELECT_MARK(mark_index,type,flag,stano1,stano2,param);
}

void CTgEditView::TG_LOG(LPCTSTR log)
{
	if(log)
	{
		GpcLogTrainMessage(0, 0, "TG控件操作日志: %s", log);
	}
}

void CTgEditView::TG_NOTICE(BYTE type, long nFlag, long nIndex,short station)
{
	CTgView::TG_NOTICE(type,nFlag,nIndex,station);
	if(type == 1)
	{
		long flag = nFlag & 0xFF;
		if(flag & 0x08)  // 避让调整
		{
			CTG_TrainScheduleBase *pData=GetDocument()->GetScheduleData(GetViewScheduleType());
			if(!pData)
				return;
			CTG_SingleMark *pselectedMark=pData->GetSingleMark(nIndex);
			if(!pselectedMark)
				return;
			m_tgmmi.AdjustTrainsForMark(pselectedMark);
		}
	}
	else if(type == 2)
	{
		long flag = nFlag & 0xFF;
		int rec_index = ((nFlag & 0xFFFFFF00) >> 24);
		if(flag & 0x4)
		{
			OnChangeTrainRoute();
		}
		if(flag & 0x8)
		{
			OnDivideportLatetime();
		}
	}
	else if(type == 3)  // 调整
	{
		CTG_TrainScheduleBase *pData=GetDocument()->GetScheduleData(GetViewScheduleType());
		if(!pData)
			return;
		CTG_SingleTrain *pselectedTrain=pData->GetSingleTrain(nIndex);
		if(!pselectedTrain)
			return;
		CTG_SingleTrain train=*pselectedTrain;
		int rec_index=(nFlag>>24)&0xFF;
		int cmd_index=nFlag & 0xFF;
		if(cmd_index==1) // 按图调整
		{
			m_tgmmi.AdjustTrainForBasicSchedule(train,rec_index);
		}
		else if(cmd_index==2)  // 避让调整
		{

			m_tgmmi.AdjustTrainForCollision(train,rec_index);
		}
		else if(cmd_index==3) // 多点不避让调整
		{
			m_tgmmi.AdjustTrainForMutiPass(train,rec_index);
		}
	}
	else if(type == 4)
	{
		CTG_TrainScheduleBase *pData=GetDocument()->GetScheduleData(GetViewScheduleType());
		if(!pData)
			return;
		CTG_SingleTrain *pselectedTrain=pData->GetSingleTrain(nIndex);
		if(!pselectedTrain)
			return;
		CTG_SingleTrain train=*pselectedTrain;
		int rec_index=(nFlag>>24)&0xFF;
		int cmd_index=nFlag & 0xFF;
		if(cmd_index==1) // 设置进路自触
		{
			DirectSetAutotrig(train,rec_index);
		}
	}
	else if(type == 5)
	{
		BatchManualModifySide(nIndex);
	}
}


void CTgEditView::TG_FIRE_DELETE(LPCTSTR xml)
{
	std::vector<ULONG> list_index;
	CStdioFile file;
	if(file.Open("c:\\delete.xml", CFile::modeCreate|CFile::modeReadWrite))
	{
		file.WriteString(xml);
		file.Close();
	}
	int showdlg=0;
	int nFlag = fromDeleteXml(showdlg,list_index, xml);
	if(list_index.size() == 0)
		return;


	CTG_TrainScheduleBase *pData=GetDocument()->GetScheduleData(GetViewScheduleType());
	if(NULL == pData)
		return;

	// TRAIN
	if(nFlag == 1)
	{
		CTG_SingleTrain *ptrain=NULL;

		if(list_index.size() == 1)
		{
			ptrain = pData->GetSingleTrain(list_index[0]);
			if(ptrain==0)
			{
				return;
			}
			//不允许删除班计划列车
			if(!m_bEnableFreeAddTrain && !GetTGFunctionConfig()->IsFreeAddTrain(ptrain->GetTrainType()) && ptrain->GetShiftTrainIndex()!=NULL_TRAININDEX)
			{
				return;
			}
			
			if (gpcdef.IsInhibitManualConvertActualTime() && GetTGFunctionConfig()->ProhibitDeleteRecordWhileLimitModifyActual())
			{
				if (ptrain->HasActualRecord()>=0)
				{
					CString text;
					text.Format("列车 %s  中存在实际点,不得进行删除操作!", ptrain->GetTrainID());
					AfxMessageBox(text);
					return;
				}
			}

			CString cap;
			CMMIOperFlag operflag;
			cap.Format("确认删除车次 %s 吗", ptrain->GetTrainID());  
			if(MessageBox(cap,"警告",MB_YESNO)==IDYES)
			{
				GpcLogTrainMessage(0, 0, "车次操作 %s 删除",ptrain->GetTrainID());
				m_tgmmi.ProcessMMIOperation(IScheduleComm::TG_OPER_DELETE,*ptrain,operflag);	
			}
		}
		else
		{
			CDlgMultiSelOperate dlg(list_index, this);
			dlg.m_pSchdBase = pData;
			if(dlg.DoModal() == IDOK)
			{
				for(int i=0; i<dlg.sel.size(); i++)
				{
					ptrain = pData->GetSingleTrain(dlg.sel[i]);
					if(ptrain == NULL)
						continue;

					//不允许删除班计划列车
					if(!m_bEnableFreeAddTrain && !GetTGFunctionConfig()->IsFreeAddTrain(ptrain->GetTrainType()) && ptrain->GetShiftTrainIndex()!=NULL_TRAININDEX)
					{
						continue;
					}
					
					if (gpcdef.IsInhibitManualConvertActualTime() && GetTGFunctionConfig()->ProhibitDeleteRecordWhileLimitModifyActual())
					{
						if (ptrain->HasActualRecord()>=0)
						{
							CString text;
							text.Format("列车 %s  中存在实际点,不得进行删除操作!", ptrain->GetTrainID());
							AfxMessageBox(text);
							continue;
						}
					}

					GpcLogTrainMessage(0, 0, "车次操作 %s 删除",ptrain->GetTrainID());
					CMMIOperFlag operflag;
					m_tgmmi.ProcessMMIOperation(IScheduleComm::TG_OPER_DELETE,*ptrain,operflag);		
				}
			}
		}	
	}

	// BLOCK
	if(nFlag == 2)
	{
		CTG_SingleMark *pmark=NULL;

		if(list_index.size() == 1)
		{
			pmark = pData->GetSingleMark(list_index[0]);
			if(pmark==0)
			{
				return;
			}
			CString str;
			int m_type = pmark->GetMarkType();
			switch(m_type)
			{
			case ABS_SLOW_MARK:
				str.Format ("区间慢行[%s]?", pmark->m_data.text);	
				break;
			case SELF_CLOSE_MARK:
				str.Format ("区间变更闭塞[%s]?", pmark->m_data.text);
				break;
			case ABS_INHIBIT_MARK:
				str.Format ("区间封锁[%s]?", pmark->m_data.text);
				break;
			case ABS_FIX_MARK:
				str.Format ("区间综合天窗修[%s]?", pmark->m_data.text);
				break;
			case ABS_ELE_FIX_MARK:
				str.Format ("区间电网检修[%s]?", pmark->m_data.text);
				break;
			case NEW_ABS_ELE_FIX_MARK:
				str.Format ("区间电网检修[%s]?", pmark->m_data.text);
				break;
			case SIDE_SLOW_MARK:
				str.Format ("站内慢行[%s]?", pmark->m_data.text);
				break;
			case SIDE_INHIBIT_MARK:
				str.Format ("股道禁用[%s]?", pmark->m_data.text);
				break;
			case SIDE_TIANCHUANG_MARK:
				str.Format ("股道天窗修[%s]?", pmark->m_data.text);
				break;
			case GRAPH_NOTE_MARK:
				str.Format ("图表注解[%s]?", pmark->m_data.text);
				break;
			case SIDE_OCCUPY_MARK:
				str.Format("股道占用[%s]?", pmark->m_data.text);
				break;
			case NEW_GRAPH_NOTE_MARK:
				str.Format ("图表注解[%s]?", pmark->m_data.text);
				break;

			default:
				str.Format ("该施工符号?");
				break;
			}
			str = "你确认删除 " + str;
			if(showdlg==0)
			{
				if(MessageBox(str, "警告", MB_YESNO|MB_ICONWARNING) != IDYES)
					return; 
			}
			CString log;
			log.Format("用户删除施工符号 index %d gpc_index %d: ", pmark->GetBlockIndex(), pmark->GetGPCIndex());
			GpcLogTrainMessage(0, 0, log);
			pmark->m_data.text[255]=0;
			log.Format("%s",  pmark->m_data.text);
			GpcLogTrainMessage(0, 0, log);

			CMMIOperFlag operflag;
			m_tgmmi.ProcessMMIOperation(IScheduleComm::TG_OPER_DELETE,*pmark,pmark->GetBlockIndex());	
		}
	}
}
////////////////////////////////////// 以下是运行图的操作
// 计划线

void CTgEditView::OnEndInsertNewTrain()
{
	mTG.EndNewSingleTrain();
}

void CTgEditView::OnDelMultiTrain()
{
	mTG.DeleteMutiTrain();
}

// 标记
void CTgEditView::OnAbsInhibitG()
{
	mTG.NewMark(MARK::IF_ABS_INHIBIT);
}

void CTgEditView::OnAbsSlowG()
{
	mTG.NewMark(MARK::IF_ABS_SLOW);
}

void CTgEditView::OnAbsFixG()
{
	mTG.NewMark(MARK::IF_ABS_TOTAL_FIX);
}

void CTgEditView::OnAbsEleFixG()
{
	mTG.NewMark(MARK::IF_ABS_ELEM_FIX);
}

void CTgEditView::OnSelfCloseG()
{
	mTG.NewMark(MARK::IF_ABS_SELF_CLOSE);
}

void CTgEditView::OnSideInhibitG()
{
	mTG.NewMark(MARK::STATION_INHIBIT);
}
void CTgEditView::OnStationSlowG()
{
	mTG.NewMark(MARK::STATION_SLOW);
}
void CTgEditView::OnSideTianchuangG()
{
	mTG.NewMark(MARK::STATION_FIX);
}
void CTgEditView::OnSideOccupy()
{
	mTG.NewMark(MARK::STATION_SIDE_OCCUPY);
}
void CTgEditView::OnSideLostEle()
{
	mTG.NewMark(MARK::STATION_SIDE_LOST_ELE);
}
void CTgEditView::OnTextNote()
{
	mTG.NewMark(MARK::TEXT_NOTE);
}
void CTgEditView::OnJishiZhujie()
{
	mTG.NewMark(MARK::PE_NOTE);
}

// 特别列车
void CTgEditView::OnDrawGoon()
{
	mTG.DrawSpecialTrain(TRAIN::GOON);
}

void CTgEditView::OnDrawReturn()
{
	mTG.DrawSpecialTrain(TRAIN::RETURN);
}

void CTgEditView::OnDrawAbstrainA()
{
	mTG.DrawSpecialTrain(TRAIN::TO_ABS_TRAIN);
}

void CTgEditView::OnDrawAbstrainB()
{
	mTG.DrawSpecialTrain(TRAIN::FROM_ABS_TRAIN);
}

// 两车操作
void CTgEditView::OnUniteTrain()
{
	mTG.TrainsOperator(TWO_TRAIN_OPERATOR::UNITE_TRAIN, 1);
}

void CTgEditView::OnRescueTrain()
{
	//由新的救援列车取代 武汉局提出
	//mTG.TrainsOperator(TWO_TRAIN_OPERATOR::RESCUE_TRAIN, 1);
}

void CTgEditView::OnSplitTrain()
{
	mTG.TrainsOperator(TWO_TRAIN_OPERATOR::SPLIT_TRAIN, 1);
}

void CTgEditView::OnCombineTrain()
{
	mTG.TrainsOperator(TWO_TRAIN_OPERATOR::COMBINE_TRAIN, 1);
}

void CTgEditView::OnAbsPartRun()
{
	if(MessageBox("你确实要修改列车区间分部运行吗?", "提示", MB_YESNO)==IDYES)
	{
		mTG.TrainsOperator(TWO_TRAIN_OPERATOR::IN_ABS_PART_RUN, 1);
	}
}
	
void CTgEditView::OnAbsWholeHelpRun()
{
	if(MessageBox("你确实要修改列车区间整车救援吗?", "提示", MB_YESNO)==IDYES)
	{
		mTG.TrainsOperator(TWO_TRAIN_OPERATOR::IN_ABS_WHOLE_HELP_RUN, 1);
	}
}

void CTgEditView::OnAttainEngineReturn()
{
	if(MessageBox("你确实要修改列车补机途中折返吗?", "提示", MB_YESNO)==IDYES)
	{
		mTG.TrainsOperator(TWO_TRAIN_OPERATOR::ATT_ENGINE_RETURN, 1);
	}
}

void CTgEditView::OnDeleteTrain()
{
	if(!CheckUserTGRight()) 
	{
		return;
	}
	
	CTG_LogicObject *pobj=GetDocument()->GetCurrentActiveObject();
	if(!pobj)
		return;
	if(pobj->GetObjectType()==TG_OBJ_TRAIN)
	{
		CTG_SingleTrain *ptrain=(CTG_SingleTrain *)pobj;
		if(!m_bEnableFreeAddTrain && !GetTGFunctionConfig()->IsFreeAddTrain(ptrain->GetTrainType())&&ptrain->GetShiftTrainIndex()!=NULL_TRAININDEX)//不允许删除班计划列车
		{
			return;
		}

		if (gpcdef.IsInhibitManualConvertActualTime() && GetTGFunctionConfig()->ProhibitDeleteRecordWhileLimitModifyActual())
		{
			if (ptrain->HasActualRecord()>=0)
			{
				AfxMessageBox("运行线中存在实际点,不得进行删除操作!");
				return;
			}
		}

		CString cap;
		CMMIOperFlag operflag;
		cap.Format("确认删除车次 %s 吗",ptrain->GetTrainID());  
		if(MessageBox(cap,"警告",MB_YESNO)==IDYES)
		{
			GpcLogTrainMessage(0, 0, "车次操作 %s,快捷删除",ptrain->GetTrainID());
			m_tgmmi.ProcessMMIOperation(IScheduleComm::TG_OPER_DELETE,*ptrain,operflag);		
		}
	}
	else
		if(pobj->GetObjectType()==TG_OBJ_MARK)
		{
			CTG_SingleMark *pmark=(CTG_SingleMark *)pobj;
			CString cap;
			CMMIOperFlag operflag;
			cap.Format("确认删除标记吗 %s 吗",pmark->GetMarkInfo(MARK_TXT_INFO_TYPE_INSIDE));  
			if(MessageBox(cap,"警告",MB_YESNO)==IDYES)
			{
				m_tgmmi.ProcessMMIOperation(IScheduleComm::TG_OPER_DELETE,*pmark,pmark->GetBlockIndex());		
			}
			GpcLogTrainMessage(0, 0, "标记操作 %d,快捷删除",pmark->GetBlockIndex());
		}
}

void CTgEditView::OnAddTmpTrain()
{
	// G12
	if(!CheckUserTGRight())
		return;

	if(IS_BASIC_SCHD(GetViewScheduleType()))
	{
		MessageBox("基本图不支持此操作","提示");
		return;
	}

	CTempAddTrains* dlg = new CTempAddTrains(GetDocument(), m_schdinfo.m_nSectionID, this);
	if(dlg != NULL)
	{
		dlg->Create(IDD_TEMP_ADD_TRAINS, this);
		dlg->ShowWindow(SW_SHOW);
	}
}

void  CTgEditView::OnAddTmpTrainPassway(TmpTrainInfo* info)
{
	// G12
	if(!GetTGFunctionConfig()->IsEnableTempAddTrain())
		return;
	if(!CheckUserTGRight()) 
		return;
	if(IS_BASIC_SCHD(GetViewScheduleType()))
	{
		MessageBox("基本图不支持按照径路时加车","提示");
		return;
	}
	if(info == NULL)
		return;

	CString trainId = info->trainid; 
	CStringArray aryEngineId;
	CDWordArray aryEngineStatus;
	ULONG beginTm = info->beginTime;
	BYTE nStartMode = info->start_mode;
	BYTE nEndMode = info->end_mode;
	BYTE nStartStaIndex = info->nBeginStaIndex;
	BYTE nEndStaIndex = info->nEndStaIndex;
	BYTE nRunType = info->nRunType;
	int nPasswayNo = info->nSelPasswayId;
	for(int i=0; i<info->nEngineCount&&i<5; i++)
	{
		aryEngineId.Add(info->engineId[i]);
		aryEngineStatus.Add(info->status[i]);
	}

	CTG_TrainScheduleBase *pData=GetDocument()->GetScheduleData(GetViewScheduleType());
	if(!pData)
		return;

    // 判断是否加车
	if(!IsAddTrain(trainId))
	{
		return;
	}

	m_tgmmi.AddTrainTmp_Passway(info->bSecret, aryEngineId, aryEngineStatus, trainId, nPasswayNo, nStartMode, nEndMode, nStartStaIndex, nEndStaIndex, beginTm, nRunType);
	GpcLogTrainMessage(0, 0, "按径路临时加车, train=%s bt=%d", trainId, beginTm);
	GpcLogTrainMessage(0, 0, "按径路临时加车, pasway=%d bs=%d es=%d",nPasswayNo,nStartStaIndex,nEndStaIndex);
	GpcLogTrainMessage(0, 0, "按径路临时加车, StartMode=%d EndMode=%d",nStartMode,nEndMode);

	return;
}


void CTgEditView::OnInsertNewTrain() 
{
	if(!CheckUserTGRight()) 
	{
		return;
	}

	if(GetTGFunctionConfig()->IsEnableInputEngineId())
		InsertNewTrainWithEngineNo();
	else
		InsertNewTrain();
	return;
}

void CTgEditView::InsertNewTrainWithEngineNo()
{
	CInsertTrainLanzhou dlg(GetDocument(), m_schdinfo.m_nSectionID);
	if(m_bEnableFreeAddTrain || (GetViewScheduleType() == CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_PREV))
		dlg.m_bAutoSelectShiftAddTrain=FALSE;
	else
		dlg.m_bAutoSelectShiftAddTrain=TRUE;

	if(dlg.DoModal()==IDOK)
	{
		CTG_TrainScheduleBase *pData=GetDocument()->GetScheduleData(GetViewScheduleType());
		if(!pData)
			return;

		// 判断是否重复加车
		if(!IsAddTrain(dlg.m_strInsertTrainId))
		{
			return;
		}
				
		// 20091011 XIAN 卡控任意加车
		if(GetViewScheduleType()==CTG_ScheduleInfo::SCHEDULE_TYPE_WORK
			&& !dlg.m_bAddShift)  
		{
			BYTE run_prio=255,train_type=255,run_type=255;
			BOOL can_free_add=TRUE;
			if(GetTGBasicParam()->GetTrainPrio(dlg.m_strInsertTrainId,&train_type,&run_type,&run_prio))
			{
				if(!m_bEnableFreeAddTrain && !GetTGFunctionConfig()->IsFreeAddTrain(train_type))
					can_free_add=FALSE;
			}

			if(!can_free_add)
			{
				MessageBox("请使用日班计划方式加车","提示");
				return;
			}
		}
		/////////////////////////////////////////////////
		// 图定加车
		if(dlg.m_bAddGraphic == TRUE)  
		{
			if(IS_BASIC_SCHD(GetViewScheduleType()))
			{
				MessageBox("基本图不支持此操作","提示");
				return;
			}

			m_tgmmi.AddTrain_BasicSchedule(dlg.m_bSecret, dlg.m_strInsertTrainId, dlg.m_aryEngineId, dlg.m_aryStatus);
			GpcLogTrainMessage(0, 0, "按基本图加单一车次, train=%s",dlg.m_strInsertTrainId);
		}
		// 日班计划加车
		else if(dlg.m_bAddShift == TRUE)  
		{
			CTG_TrainScheduleBase *pData=GetDocument()->GetScheduleData(CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT);
			BOOL  oper_rc=FALSE;
			if(pData)
			{
				TIME bm,em;
				CWorkScheduleHolder *pHolder=GetDocument()->GetTGDataHolder();
				CTG_ScheduleInfo info;
				if(pHolder==NULL)
					return;
				pHolder->GetShiftScheduleInfo(info);
				info.GetShiftManagerRange(bm,em);

				CTG_TrainPtrGroup prev_group;
				pData->FindTrains(prev_group,NULL_STATION,bm, em,FALSE,TRUE);
				for(int idx=0;idx<prev_group.GetTrainCount();idx++)
				{
					CTG_SingleTrain *prev_tmp=prev_group.GetTrainPtr(idx);
					if(!prev_tmp->MatchTrainID(dlg.m_strInsertTrainId)) // 原来方式添加的车次
						continue;

					oper_rc=m_tgmmi.AddTrain_ShiftSchedule(dlg.m_bSecret, prev_tmp->GetTrainIndex(), dlg.m_aryEngineId, dlg.m_aryStatus, NULL_STATION,bm,em);
					if(oper_rc)
					{
						GpcLogTrainMessage(0, 0, "按日班计划加单一车次, train=%s",dlg.m_strInsertTrainId);
					}
				}
			}
			if(oper_rc==FALSE)
				MessageBox("找不到指定车次");
		}
		// 自由画线
		else if(dlg.m_nCurSel == 0)  
		{
			BYTE train_type,run_type,prio;
			if(!GetTGBasicParam()->GetTrainPrio(dlg.m_strInsertTrainId, &train_type, &run_type, &prio))
				train_type=255;
			CString xml, tmp;
			xml = "<NEWTRAIN>";
			tmp.Format(" <TRAINID>%s</TRAINID>", dlg.m_strInsertTrainId);
			xml+=tmp;
			tmp.Format(" <ARLTRAINID>%s</ARLTRAINID>", dlg.m_strArrivalTrainId);
			xml+=tmp;
			tmp.Format(" <START>%d</START>", dlg.m_nStartMode);
			xml+=tmp;
			tmp.Format(" <END>%d</END>", dlg.m_nEndMode);
			xml+=tmp;
			tmp.Format(" <SECRET>%d</SECRET>", dlg.m_bSecret);
			xml+=tmp;
			tmp.Format(" <RUN_TYPE>%d</RUN_TYPE>", dlg.m_nRunType);
			xml+=tmp;
			tmp.Format(" <TRAIN_TYPE>%d</TRAIN_TYPE>", train_type);
			xml+=tmp;
			xml+="<ENGINE>";
			for(int i=0; i<dlg.m_aryEngineId.GetCount(); i++)
			{
				tmp.Format("<ID id=\"%s\" status=\"%d\"/>", 
					dlg.m_aryEngineId.GetAt(i),
					dlg.m_aryStatus.GetAt(i));
				xml+=tmp;
			}
			xml+="</ENGINE>";
			xml += "</NEWTRAIN>";
			mTG.BeginNewSingleTrainXml(xml);

			GpcLogTrainMessage(0, 0, "自由画线增加单一车次, train=%s",dlg.m_strInsertTrainId);
		}
		// 按照径路加车
		else
		{
			CWorkScheduleHolder  *pholder=GetDocument()->GetTGDataHolder();
			if(pholder)
			{
				m_currentOper.nOperStatus=OPER_STATUS_PASSWAY;
				memset(m_currentOper.szTrainNo, 0, sizeof(m_currentOper.szTrainNo));
				TG_CopyTrainId(m_currentOper.szTrainNo,dlg.m_strInsertTrainId);

				m_currentOper.nRouteNo=dlg.m_nRouteNo;
				m_currentOper.nStartMode=dlg.m_nStartMode;
				m_currentOper.nEndMode=dlg.m_nEndMode;
				m_currentOper.nEngineCount=0;
				memset(m_currentOper.engineId, 0, sizeof(m_currentOper.engineId));
				m_currentOper.bSelSecret=dlg.m_bSecret;
				m_currentOper.nRunType=dlg.m_nRunType;
				for(int i=0; i<dlg.m_aryEngineId.GetCount()&&i<5; i++)
				{
					strncpy(m_currentOper.engineId[i],  dlg.m_aryEngineId.GetAt(i), sizeof(m_currentOper.engineId[i])-1);
					m_currentOper.status[i] = dlg.m_aryStatus.GetAt(i);
					m_currentOper.nEngineCount++;
				}
			}
			GpcLogTrainMessage(0, 0, "按径路增加单一车次, train=%s route=%d abs_run_type=%d",dlg.m_strInsertTrainId,dlg.m_nRouteNo,m_currentOper.nRunType);
		}
	}
}

void CTgEditView::InsertNewTrain()
{
	CInsertTrain dlg(m_schdinfo.m_nSectionID);
	if(m_bEnableFreeAddTrain || (GetViewScheduleType() == CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_PREV))
		dlg.m_bAutoSelectShiftAddTrain=FALSE;
	else
		dlg.m_bAutoSelectShiftAddTrain=TRUE;

	if(dlg.DoModal()==IDOK)
	{
		CTG_TrainScheduleBase *pData=GetDocument()->GetScheduleData(GetViewScheduleType());
		if(!pData)
			return;

		// 判断是否加车
		if(!IsAddTrain(dlg.m_strInsertTrainId))
		{
			return;
		}
		
		// 20091011 XIAN 卡控任意加车
		if(GetViewScheduleType()==CTG_ScheduleInfo::SCHEDULE_TYPE_WORK
			&& !dlg.m_bAddShift)  
		{
			BYTE run_prio=255,train_type=255,run_type=255;
			BOOL can_free_add=TRUE;
			if(GetTGBasicParam()->GetTrainPrio(dlg.m_strInsertTrainId,&train_type,&run_type,&run_prio))
			{
				if(!m_bEnableFreeAddTrain && !GetTGFunctionConfig()->IsFreeAddTrain(train_type))
					can_free_add=FALSE;
			}

			if(!can_free_add)
			{
				MessageBox("请使用日班计划方式加车","提示");
				return;
			}
		}
		/////////////////////////////////////////////////

		if(dlg.m_bAddGraphic == TRUE)  // 图定加车
		{
			if(IS_BASIC_SCHD(GetViewScheduleType()))
			{
				MessageBox("基本图不支持此操作","提示");
				return;
			}

			if(m_tgmmi.AddTrain_BasicSchedule(dlg.m_bSecret, dlg.m_strInsertTrainId,""))
			{
				GpcLogTrainMessage(0, 0, "按基本图加单一车次, train=%s",dlg.m_strInsertTrainId);
			}
		}
		else if(dlg.m_bAddShift == TRUE)  // 日班计划加车
		{
			CTG_TrainScheduleBase *pData=GetDocument()->GetScheduleData(CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT);
			BOOL  oper_rc=FALSE;
			if(pData)
			{
				TIME bm,em;
				CWorkScheduleHolder *pHolder=GetDocument()->GetTGDataHolder();
				CTG_ScheduleInfo info;
				if(pHolder==NULL)
					return;
				pHolder->GetShiftScheduleInfo(info);
				info.GetShiftManagerRange(bm,em);
				em += 6*3600;

				CTG_TrainPtrGroup prev_group;
				pData->FindTrains(prev_group,NULL_STATION,bm, em,FALSE,TRUE);
				for(int idx=0;idx<prev_group.GetTrainCount();idx++)
				{
					CTG_SingleTrain *prev_tmp=prev_group.GetTrainPtr(idx);
					if(!prev_tmp->MatchTrainID(dlg.m_strInsertTrainId)) // 原来方式添加的车次
						continue;
					oper_rc=m_tgmmi.AddTrain_ShiftSchedule(dlg.m_bSecret, prev_tmp->GetTrainIndex(),NULL_STATION,bm,em);
					if(oper_rc)
					{
						GpcLogTrainMessage(0, 0, "按日班计划加单一车次, train=%s",dlg.m_strInsertTrainId);
					}
				}
			}
			if(oper_rc==FALSE)
			{
				if(GetTGFunctionConfig()->IsFreedrawAddTrainShiftAndBasic())
				{
					if(IS_BASIC_SCHD(GetViewScheduleType()))
					{
						MessageBox("基本图不支持此操作","提示");
						return;
					}

					if(m_tgmmi.AddTrain_BasicSchedule(dlg.m_bSecret, dlg.m_strInsertTrainId,""))
					{
						GpcLogTrainMessage(0, 0, "日班计划中没有该车, 按基本图加单一车次, train=%s",dlg.m_strInsertTrainId);
					}
					else
					{
						CString text;
						text.Format("日班计划没有 基本图中也没有车次 %s, 加车失败", dlg.m_strInsertTrainId);
						AfxMessageBox(text);
					}
				}
				else
				{
					CString text;
					text.Format("日班计划中没有车次 %s, 按照日班计划加车失败", dlg.m_strInsertTrainId);
					AfxMessageBox(text);
				}
			}
		}
		// 自由画线
		else if(dlg.m_nCurSel == 0)
		{
			//int nStartMode = dlg.m_nStartMode;
			//int nEndMode = dlg.m_nEndMode;
			//if(dlg.m_bSecret)
			//	nStartMode |= 0x80;
			//mTG.BeginNewSingleTrain(dlg.m_strInsertTrainId,dlg.m_strArrivalTrainId,nStartMode,nEndMode);
			BYTE train_type,run_type,prio;
			if(!GetTGBasicParam()->GetTrainPrio(dlg.m_strInsertTrainId, &train_type, &run_type, &prio))
				train_type=255;

			CString xml, tmp;
			xml = "<NEWTRAIN>";
			tmp.Format(" <TRAINID>%s</TRAINID>", dlg.m_strInsertTrainId);
			xml+=tmp;
			tmp.Format(" <ARLTRAINID>%s</ARLTRAINID>", dlg.m_strArrivalTrainId);
			xml+=tmp;
			tmp.Format(" <START>%d</START>", dlg.m_nStartMode);
			xml+=tmp;
			tmp.Format(" <END>%d</END>", dlg.m_nEndMode);
			xml+=tmp;
			tmp.Format(" <SECRET>%d</SECRET>", dlg.m_bSecret);
			xml+=tmp;
			tmp.Format(" <RUN_TYPE>%d</RUN_TYPE>", dlg.m_nRunType);
			xml+=tmp;
			tmp.Format(" <TRAIN_TYPE>%d</TRAIN_TYPE>", train_type);
			xml+=tmp;
			xml += "</NEWTRAIN>";
			mTG.BeginNewSingleTrainXml(xml);

			GpcLogTrainMessage(0, 0, "自由画线增加单一车次, train=%s xml=%s",dlg.m_strInsertTrainId, xml);
		}
		// 按照径路加车
		else
		{
			CWorkScheduleHolder  *pholder=GetDocument()->GetTGDataHolder();
			if(pholder)
			{
				m_currentOper.nOperStatus=OPER_STATUS_PASSWAY;
				memset(m_currentOper.szTrainNo, 0, sizeof(m_currentOper.szTrainNo));
				TG_CopyTrainId(m_currentOper.szTrainNo,dlg.m_strInsertTrainId);

				memset(m_currentOper.szArriveTrainNo, 0, sizeof(m_currentOper.szArriveTrainNo));
				TG_CopyTrainId(m_currentOper.szArriveTrainNo,dlg.m_strArrivalTrainId);

				m_currentOper.nRouteNo=dlg.m_nRouteNo;
				m_currentOper.nStartMode=dlg.m_nStartMode;
				m_currentOper.nEndMode=dlg.m_nEndMode;
				m_currentOper.nEngineCount=0;
				memset(m_currentOper.engineId, 0, sizeof(m_currentOper.engineId));
				m_currentOper.bSelSecret=dlg.m_bSecret;
				m_currentOper.nRunType=dlg.m_nRunType;
				GpcLogTrainMessage(0, 0, "按径路增加单一车次, train=%s route=%d abs_run_type=%d",dlg.m_strInsertTrainId,dlg.m_nRouteNo,m_currentOper.nRunType);
			}
		}
	}
}

void CTgEditView::OnManualSelectTrain()
{
	if(1 != GetMMIFunctionConfig()->GetConfigIntegerItem(CONFIG_ADD_TRAIN_BY_BASIC))
		OnManualSelectShiftTrain();
	else
		OnManualSelectBasicTrain();
}

void CTgEditView::OnManualSelectBasicTrain() 
{//选择图定列车
	if(!m_bEnableFreeAddTrain)
	{
		MessageBox("请使用日班计划方式加车","提示");
		return;
	}
	CWorkScheduleHolder  *pholder=GetDocument()->GetTGDataHolder();		
	CSelectScheduleTrain dlg(pholder);
	dlg.m_nSelectFromSchdType=CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC;
	dlg.m_bEnableChoiceSchedule=TRUE;

	if(dlg.DoModal () == IDOK)
	{
		OnRecievedDataEnd(0,1);
		for(int i=0;i<dlg.m_nCount;i++)
		{
			if(dlg.info[i].exist_train==TGSelectedBasicTrain::BT_NONE || dlg.m_check)
			{
				if(dlg.m_nSelectFromSchdType==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC)
					m_tgmmi.AddTrain_BasicSchedule(dlg.info[i].train_index);
				else if(dlg.m_nSelectFromSchdType==CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT)
					m_tgmmi.AddTrain_ShiftSchedule(false, dlg.info[i].train_index);
			}
		}
		OnRecievedDataEnd(0,0);
	}
}

void CTgEditView::OnManualSelectShiftTrain() 
{//选择日班计划列车
	CWorkScheduleHolder  *pholder=GetDocument()->GetTGDataHolder();		
	CSelectScheduleTrain dlg(pholder);

	dlg.m_nSelectFromSchdType=CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT;
	if(!m_bEnableFreeAddTrain)
		dlg.m_bEnableChoiceSchedule=FALSE;
	else
		dlg.m_bEnableChoiceSchedule=TRUE;

	if(m_schdinfo.m_nType==CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT)
	{
		MessageBox("不能在日班计划中使用该功能!","错误");
		return;
	}
	if(dlg.DoModal () == IDOK)
	{
		OnRecievedDataEnd(0,1);
		for(int i=0;i<dlg.m_nCount;i++)
		{
			if(dlg.info[i].exist_train==TGSelectedBasicTrain::BT_NONE || dlg.m_check)
			{
				if(dlg.m_nSelectFromSchdType==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC)
					m_tgmmi.AddTrain_BasicSchedule(dlg.info[i].train_index);
				else if(dlg.m_nSelectFromSchdType==CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT)
					m_tgmmi.AddTrain_ShiftSchedule(false, dlg.info[i].train_index);
			}
		}
		OnRecievedDataEnd(0,0);
	}
}

void CTgEditView::SingleTrainToWork()
{
	if(m_schdinfo.m_nType!=CTG_ScheduleInfo::SCHEDULE_TYPE_WORK 
		|| m_schdinfo.m_nSectionID!=m_singleTrainToWork.section)
	{
		return;
	}

	CTG_TrainScheduleBase *pData=GetDocument()->GetScheduleData(GetViewScheduleType());
	if(!pData)
		return;

	// 判断是否加车
	if(!IsAddTrain(m_singleTrainToWork.train_id))
	{
		return;
	}
	
	if(m_singleTrainToWork.source_schd_type==CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT)
	{
		BOOL oper_rc=m_tgmmi.AddTrain_ShiftSchedule(false, m_singleTrainToWork.train_index);
		if(oper_rc)
		{
			GpcLogTrainMessage(0, 0, "日班计划,单趟车加单一车次, train=%s(%u)",
							m_singleTrainToWork.train_id, m_singleTrainToWork.train_index);
		}
	}
	else if(m_singleTrainToWork.source_schd_type==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC)
	{
		BOOL oper_rc=m_tgmmi.AddTrain_BasicSchedule(m_singleTrainToWork.train_index);
		if(oper_rc)
		{
			GpcLogTrainMessage(0, 0, "基本图,单趟车加单一车次, train=%s(%u)",
							m_singleTrainToWork.train_id,m_singleTrainToWork.train_index);
		}
	}
}

void CTgEditView::OnShiftTrainToWork()
{
	TRAIN_INDEX nTrainIndex=0;
	CString train_name="";
	CTG_SingleTrain *ptrain=GetDocument()->GetCurrentActiveTrain(CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT);
	if(ptrain)
	{
		nTrainIndex=ptrain->GetTrainIndex();
		train_name=ptrain->GetTrainID();
	}
	if(0==nTrainIndex)
	{
		AfxMessageBox("请先选中日班计划列车");
		return;
	}
	CString str;
	str.Format("你确认日班计划列车 %s 上调度员当前调整图吗?", train_name);
	if(IDYES!=MessageBox(str, "提示", MB_YESNO))
		return;
	m_singleTrainToWork.section=m_schdinfo.m_nSectionID;
	m_singleTrainToWork.train_index=nTrainIndex;
	m_singleTrainToWork.source_schd_type=m_schdinfo.m_nType;
	memset(m_singleTrainToWork.train_id, 0, sizeof(m_singleTrainToWork.train_id));
	strncpy(m_singleTrainToWork.train_id, train_name,31);
	m_singleTrainToWork.train_id[31]=0;
	GetDocument()->UpdateAllViews(this, TGSHIFT_TRAIN_GEN_TO_WORK, (CObject*)&m_singleTrainToWork);
}

void CTgEditView::OnUpdateShiftTrainToWork(CCmdUI* pCmdUI)
{
	if(GetViewScheduleType()!=CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT) 
	{
		pCmdUI->Enable(FALSE); 
		return;
	}

	if(gpcdef.GetUserIdentify()==GPC_DISPATCHER)
		pCmdUI->Enable(TRUE);
	else
		pCmdUI->Enable(FALSE);
}

void CTgEditView::OnBasicTrainToWork()
{
	TRAIN_INDEX nTrainIndex=0;
	CString train_name="";
	CTG_SingleTrain *ptrain=GetDocument()->GetCurrentActiveTrain(CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC);
	if(ptrain)
	{
		nTrainIndex=ptrain->GetTrainIndex();
		train_name=ptrain->GetTrainID();
	}
	if(0==nTrainIndex)
	{
		AfxMessageBox("请先选中基本图列车");
		return;
	}

	CString str;
	str.Format("你确认基本图列车 %s 上调度员当前调整图吗?", train_name);
	if(IDYES!=MessageBox(str, "提示", MB_YESNO))
		return;

	m_singleTrainToWork.section=m_schdinfo.m_nSectionID;
	m_singleTrainToWork.train_index=nTrainIndex;
	m_singleTrainToWork.source_schd_type=m_schdinfo.m_nType;
	memset(m_singleTrainToWork.train_id, 0, sizeof(m_singleTrainToWork.train_id));
	strncpy(m_singleTrainToWork.train_id, train_name,31);
	m_singleTrainToWork.train_id[31]=0;
	GetDocument()->UpdateAllViews(this, TGSHIFT_TRAIN_GEN_TO_WORK, (CObject*)&m_singleTrainToWork);
}

void CTgEditView::OnUpdateBasicTrainToWork(CCmdUI* pCmdUI)
{
	if(GetViewScheduleType()!=CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC) 
	{
		pCmdUI->Enable(FALSE); 
		return;
	}
	
	if(gpcdef.GetUserIdentify()==GPC_DISPATCHER)
		pCmdUI->Enable(TRUE);
	else
		pCmdUI->Enable(FALSE);
}

void CTgEditView::OnManualSelectBasicMark() 
{//选择日班计划列车
	CWorkScheduleHolder  *pholder=GetDocument()->GetTGDataHolder();		
	CSelectScheduleMark dlg(pholder);
	dlg.m_nSelectFromSchdType=CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC;
	if(m_schdinfo.m_nType==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC)
	{
		MessageBox("不能在基本图中使用该功能!","错误");
		return;
	}
	if(dlg.DoModal () == IDOK)
	{
		for(int i=0;i<dlg.m_nCount;i++)
		{
			if(!dlg.info[i].checked )
				continue;
			if(!dlg.info[i].exist || dlg.m_check) // 20090702 避免重复添加
		 {
			 if(dlg.m_nSelectFromSchdType==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC)
			 {
				 m_tgmmi.AddMark_BasicSchedule(dlg.info[i].mark_index);
				 GpcLogTrainMessage(0, 0, "从基本图中导入施工符号,id=%d",dlg.info[i].mark_index);
			 }
		 }
		}
	}
}

void CTgEditView::OnManualSelectShiftMark() 
{//选择日班计划列车
	CWorkScheduleHolder  *pholder=GetDocument()->GetTGDataHolder();		
	CSelectScheduleMark dlg(pholder);
	dlg.m_nSelectFromSchdType=CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT;
	if(m_schdinfo.m_nType==CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT)
	{
		MessageBox("不能在日班计划中使用该功能!","错误");
		return;
	}
	if(dlg.DoModal () == IDOK)
	{
		for(int i=0;i<dlg.m_nCount;i++)
		{
			if(!dlg.info[i].checked )
				continue;
			if(!dlg.info[i].exist || dlg.m_check) // 20090702 避免重复添加
		 {
			 if(dlg.m_nSelectFromSchdType==CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT)
			 {
				 m_tgmmi.AddMark_ShiftSchedule(dlg.info[i].mark_index);
				 GpcLogTrainMessage(0, 0, "从日班计划中导入施工符号,id=%d",dlg.info[i].mark_index);
			 }
		 }
		}

	}
}

void CTgEditView::OnSendStationPlan() 
{
	if(GetViewScheduleType()!=CTG_ScheduleInfo::SCHEDULE_TYPE_WORK)
		return;
	if(m_pNotepadDlg != NULL)
	{
		m_pNotepadDlg->SetBasicSchduleInfo();  // 弹出下计划框之前,生成基本图信息
		m_pNotepadDlg->ShowWindow(SW_SHOW);
		m_pNotepadDlg->SetActiveWindow();
		m_pNotepadDlg->m_HideWnd=false;
		if(GetTGFunctionConfig()->IsStplanOnlySelectLirc())
			m_pNotepadDlg->SendStplanOnlySelLIRC();
		else
			m_pNotepadDlg->SelAllStplanRecEntity();

		if(GetTGFunctionConfig()->IsEnableSendPlanNoticeDCmd())
		{
			m_pNotepadDlg->SendNoticeToDCmd();
		}
	}
}   
void CTgEditView::OnSendNeighber() 
{
	OnSendNeigbourPlan();
}

// 通知邻台接收计划
void CTgEditView::OnSendNeigbourPlan()
{
	GPCENTITY entityInfo;
	char sEntityExpress[15];
	sprintf(sEntityExpress, "gpc%d", gpcdef.GetDispatchNoFromDefaultUser());
	if(!gpcdef.GetEntityInfo(sEntityExpress, entityInfo))
	{
		MessageBox("找不到本台entity, 不能发送邻台计划", "提示", MB_OK|MB_ICONWARNING);
		return;
	}

	CTgDocument *pDoc = GetDocument();
	if(pDoc == NULL)
		return;

	CWorkScheduleHolder *pholder=pDoc->GetTGDataHolder();
	if(!pholder)
		return;

	CTG_ScheduleInfo  schd_info;
	pholder->GetWorkScheduleInfo(schd_info);

	pDoc->NoticeNeighbourReceivePlan(schd_info, entityInfo.nEntityId);
}

void CTgEditView::OnNeighberPlan() 
{	
	if(GetViewScheduleType()!=CTG_ScheduleInfo::SCHEDULE_TYPE_WORK)
		return;

	CTgDocument *pDoc = GetDocument();
	if(pDoc != NULL && pDoc->m_vectNeigPlan.size()>0)
	{
		RecvNeigPlan plan = pDoc->m_vectNeigPlan.back();
		pDoc->m_vectNeigPlan.pop_back();

		GPCENTITY entityInfo;
		if(!gpcdef.GetEntityInfo(101, SERVER_TYPE_TG, plan.nSendSectionNo, entityInfo)) 
		{
			CString text;
			text.Format("无法获取 %d号 srv_tg 的实体号", plan.nSendSectionNo);
			AfxMessageBox(text);
			return; 
		}

		if(DB_GetEntityStatus(entityInfo.nEntityId)!=1)
		{
			if(IDYES==MessageBox("请注意:\n\t邻台tg_svr服务器和本调度终端连接中断, 不能进行邻台交互,你确认退出吗?", "提示", MB_YESNO|MB_ICONWARNING))
			{
				return;
			}
		}

		GpcLogTrainMessage(0, 0, "本台被动接收邻台计划con_name %s con_id %d  hour %d", plan.strSendSectionName, plan.nSendSectionNo, plan.nHour);
		CDlgNeibTrainList dlg(  plan.strSendSectionName,
			GetDocument(),
			GetViewScheduleType(),
			0,
			plan.nHour, 
			plan.nSendSectionNo, 
			this);
		dlg.SetTgMMI(&m_tgmmi);
		dlg.DoModal();
	}
	else
	{
		CNeibPlanFuncChoiceDlg dlg;
		dlg.DoModal();
		if(dlg.nRet == IDOK)
		{
			OnSendNeigbourPlan();
		}
		else if(dlg.nRet == IDCANCEL)
		{
			GpcLogTrainMessage(0, 0, "本台主动获取邻台计划");
			CDlgNeibTrainList dlg("接收邻台计划",GetDocument(),GetViewScheduleType(),0,this);
			dlg.SetTgMMI(&m_tgmmi);
			dlg.DoModal();
		}
	}
}

void CTgEditView::OnRecvNeigbourPlan() 
{	
	if(GetViewScheduleType()!=CTG_ScheduleInfo::SCHEDULE_TYPE_WORK)
		return;

	CTgDocument *pDoc = GetDocument();
	if(pDoc != NULL && pDoc->m_vectNeigPlan.size()>0)
	{
		RecvNeigPlan plan = pDoc->m_vectNeigPlan.back();
		pDoc->m_vectNeigPlan.pop_back();

		GpcLogTrainMessage(0, 0, "本台被动接收邻台计划con_name %s con_id %d  hour %d", plan.strSendSectionName, plan.nSendSectionNo, plan.nHour);
		CDlgNeibTrainList dlg(  plan.strSendSectionName,
			GetDocument(),
			GetViewScheduleType(),
			0,
			plan.nHour, 
			plan.nSendSectionNo, 
			this);
		dlg.SetTgMMI(&m_tgmmi);
		dlg.DoModal();
	}
	else
	{
		GpcLogTrainMessage(0, 0, "本台主动获取邻台计划");
		CDlgNeibTrainList dlg("接收邻台计划",GetDocument(),GetViewScheduleType(),0,this);
		dlg.SetTgMMI(&m_tgmmi);
		dlg.DoModal();
	}
}

void CTgEditView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{

	CTgDocument *pDoc=GetDocument();
	if(lHint==TGDATA_XML_INFORM)
	{
		XMLInform *pinform=(XMLInform *)pHint;
		m_xmlinform.content =pinform->content; 
		m_xmlinform.token =pinform->token;
		BOOL do_recv=FALSE;
		if(m_schdinfo.m_nType==CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT)
		{
			if(pinform->token==TOKEN_SS_PLAN_INFORM)
			{
				GpcLogTrainMessage(0, 0, "收到日班计划下达通知");
				if(MessageBox("收到日班计划下达通知,现在进行接收吗?","提示",MB_YESNO)==IDYES)
				{
					OnGetTrainList();
					return;
				}
			}
		}
		else
			if(m_schdinfo.m_nType==CTG_ScheduleInfo::SCHEDULE_TYPE_WORK)
			{
				GpcLogTrainMessage(0, 0, "收到调整计划下达通知");

				GetParentFrame()->SendMessage(UM_MEMORY_NOIFY,3,0);
			}
	}
	else if(lHint==TGSHIFT_TRAIN_GEN_TO_WORK)
	{
		m_singleTrainToWork=*(SINGLE_TRAIN_GEN_TO_WORK*)pHint;
		SingleTrainToWork();
	}
	else
		CTgView::OnUpdate(pSender,lHint,pHint);
}

void CTgEditView::OnManualReportHistory()
{
	GpcLogTrainMessage(0, 0, "手工报点");
	CManualReportHistory* pReportDlg=new CManualReportHistory(&m_tgmmi,m_schdinfo.m_nSectionID,m_schdinfo.m_nType, this);
	if(pReportDlg)
	{
		pReportDlg->Create(IDD_DLG_MANUAL_REPORT);
		pReportDlg->ShowWindow(SW_SHOW);
	}
}

void CTgEditView::OnDeleteAllShiftTrain()
{
	CDlgSelectShift dlg(this, m_schdinfo.m_nShiftID);

	GetDocument()->OpenWaitWindow(NULL);
	if(dlg.DoModal()==IDOK)
	{
		if(MessageBox("确认删除日班计划列车吗?","警告",MB_YESNO)==IDYES)
		{
			TIME starttm,endtm;
			CTG_ScheduleInfo schd(m_schdinfo.m_nType, m_schdinfo.m_nSectionID, dlg.m_nShiftID); 
			schd.GetShiftManagerRange(starttm,endtm);

			CWorkScheduleHolder  *pholder=GetDocument()->GetTGDataHolder();
			CTG_TrainScheduleBase *pData=pholder->GetShiftScheduleData();
			if(pData)
				pData->DirectDeleteTrainsInRange(starttm,endtm,TRUE,FALSE);  // 20090628 接收日班计划时删除原车次,收到第一条再清除
			GpcLogTrainMessage(0, 0, "删除日班计划列车， %d-%d",starttm,endtm);
		}
	}
}

void CTgEditView::OnDeleteAllShiftMark()
{
	CDlgSelectShift dlg(this, m_schdinfo.m_nShiftID);

	GetDocument()->OpenWaitWindow(NULL);
	if(dlg.DoModal()==IDOK)
	{
		if(MessageBox("确认删除日班计划施工吗?","警告",MB_YESNO)==IDYES)
		{
			TIME starttm,endtm;
			CTG_ScheduleInfo schd(m_schdinfo.m_nType, m_schdinfo.m_nSectionID, dlg.m_nShiftID); 
			schd.GetShiftManagerRange(starttm,endtm);

			CWorkScheduleHolder  *pholder=GetDocument()->GetTGDataHolder();
			CTG_TrainScheduleBase *pData=pholder->GetShiftScheduleData();
			if(pData)
				pData->DirectDeleteMarksInRange(starttm,endtm,TRUE,FALSE);  // 20090628 接收日班计划时删除原车次,收到第一条再清除
			GpcLogTrainMessage(0, 0, "删除日班计划施工， %d-%d",starttm,endtm);
		}
	}
}

void CTgEditView::OnGetBasicTrainFromOpms()
{
	if(mServer_nosync_count > 0 || mMmi_nosync_count > 0)
	{
		CString msg;
		msg.Format("存在未同步数据, 服务器 %d  界面%d, 请等待同步个数为0再获取", mServer_nosync_count, mMmi_nosync_count);
		AfxMessageBox(msg);
		return;
	}

	m_bEnableProcessTdmsBasicInfo=false;
	tdms_ctc_alarm.RemoveAll();
	if(GetViewScheduleType()==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_PREV)
	{
		if(!UR_CanOperateBasicSchedule())
		{
			return;
		}
		long eid=GetSpecialEntityID(SERVER_TYPE_TDCOMM); 
		if(DB_GetEntityStatus(eid)!=1)
		{
			MessageBox("请注意:\n\t本调度终端和TD通信服务器连接中断, 不能获取基本图计划!!", "提示", MB_OK|MB_ICONWARNING);
			return;
		}

		GetDocument()->OpenWaitWindow(NULL);
		m_bSelTDMSTrainGenBasic=false;
		CDlgTDMSBasicTip dlg;
		if(dlg.DoModal()==IDOK)
		{
			m_bSelTDMSTrainGenBasic=dlg.m_bSelTDMSTrainGenBasic;

			CTG_ScheduleInfo schdinfo;
			CWorkScheduleHolder *pHolder=GetDocument()->GetTGDataHolder();
			CTG_TrainScheduleBase *pData=pHolder->GetBasicPrevScheduleData();
			pData->GetScheduleInfo(schdinfo); 
			m_BaseOfBasicTmp.SetScheduleInfo(schdinfo);
			m_BaseOfBasicTmp.DeleteAllData(schdinfo, 0);

			recieved_tdms_trains.clear();

			if(RequestBasicScheduleFromOPMS(0))
			{
				GetDocument()->OpenWaitWindow("正在接收TDMS基本图数据,请稍侯...");
				m_bEnableProcessTdmsBasicInfo=true;
			}
			else
			{
				AfxMessageBox("获取TDMS预览基本图失败!");
			}
		}
	}
}

BOOL CTgEditView::RequestShiftScheduleFromOPMS(TIME starttm,TIME endtm)
{
	CString token,content;
	if(TD_GenRequestMessage_3_YL_BY_TIME(token,content,GetDocument()->GetCurrentSectionNo(),starttm,endtm,FALSE,TRUE,TRUE,""))
	{
		long  eid=GetSpecialEntityID(SERVER_TYPE_TDCOMM); 
		TGSendStringMessage(token,content,eid);
		GpcLogTrainMessage(0,0,"向TD请求日班计划 %d %s %s", eid, token, content);
		return TRUE;
	}
	
	return FALSE;
}

void CTgEditView::OnGetTrainList()
{
	long eid=GetSpecialEntityID(SERVER_TYPE_TDCOMM); 
	if(DB_GetEntityStatus(eid)!=1)
	{
		MessageBox("请注意:\n\t本调度终端和TD通信服务器连接中断, 不能获取日班计划!!", "提示", MB_OK|MB_ICONWARNING);
		return;
	}

	m_bEnableProcessTdmsShiftInfo=false;
	tdms_ctc_alarm.RemoveAll();
	if(GetViewScheduleType()==CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT)
	{
		CDlgSelectShift dlg(this, m_schdinfo.m_nShiftID);

		GetDocument()->OpenWaitWindow(NULL);

		if(dlg.DoModal()==IDOK)
		{
			recieved_tdms_trains.clear();

			TIME starttm,endtm;
			CTG_ScheduleInfo schd(m_schdinfo.m_nType, m_schdinfo.m_nSectionID, dlg.m_nShiftID); 
			schd.GetShiftManagerRange(starttm,endtm);
			if(RequestShiftScheduleFromOPMS(starttm,endtm))
			{
				GpcLogTrainMessage(0, 0, "获取班计划车次，区段号 %d 班别 %d", m_schdinfo.m_nSectionID,dlg.m_nShiftID);
				GetDocument()->OpenWaitWindow("正在接收TDMS日班计划数据,请稍侯...");
				m_bEnableProcessTdmsShiftInfo=true;
			}
			else
			{
				AfxMessageBox("获取班计划车次失败!");
			}
		}
	}
}

void CTgEditView::OnGetMarkList()
{
	long  eid=0; 
	if(gpcdef.IsEnableMMPlanFromCAD())
	{
		eid=GetSpecialEntityID(SERVER_TYPE_MMPLAN); 
		if(DB_GetEntityStatus(eid)!=1)
		{
			AfxMessageBox("请注意:\n\t本调度终端 和 施工服务器 连接中断, 不能获取施工计划!!", MB_OK|MB_ICONWARNING);
			return;
		}
	}
	else
	{
		eid=GetSpecialEntityID(SERVER_TYPE_TDCOMM);
		if(DB_GetEntityStatus(eid)!=1)
		{
			AfxMessageBox("请注意:\n\t本调度终端 和 TD服务器 连接中断, 不能获取施工计划!!", MB_OK|MB_ICONWARNING);
			return;
		}
	}

	CDlgSelectShift dlg(this, m_schdinfo.m_nShiftID);

	GetDocument()->OpenWaitWindow(NULL);
	if(dlg.DoModal()==IDOK)
	{
		CTime begintm(dlg.m_ntStartTime);
		begintm = CTime(begintm.GetYear(),begintm.GetMonth(),begintm.GetDay(),0,0,0);

		GetDocument()->RequestMMPlanFromOPMS(begintm.GetTime(),dlg.m_ntEndTime,this,eid);
	} 
}
void CTgEditView::OnRecvTrainList()
{
	GetParentFrame()->SendMessage(UM_MEMORY_NOIFY,2,0);
	CDlgTrainList dlg("接收调整计划",GetDocument(),m_schdinfo.m_nType,0,NULL);
	dlg.SetTgMMI(&m_tgmmi);
	dlg.DoModal();
	GpcLogTrainMessage(0, 0, "接收调整计划");
}


void CTgEditView::OnRecvMarkList()
{
	CDlgTrainList dlg("接收调整施工计划",GetDocument(),m_schdinfo.m_nType,1,NULL);
	dlg.DoModal();
}



void CTgEditView::OnManualAddNewTrain()
{
	if(!m_bEnableFreeAddTrain)
	{
		MessageBox("请使用日班计划方式加车","提示");
		return;
	}

	if(IS_BASIC_SCHD(GetViewScheduleType())) // 预览基本图,不可以绘制施工符号
	{
		MessageBox("基本图不支持该种方式加车","提示");
		return;
	}

	CAddNewTrainDlg dlg(GetDocument(),m_schdinfo.m_nType, this);
	if(dlg.DoModal () != IDOK)
		return;

	for (int i=0;i<dlg.m_nTrainCount ;i++)
	{
		int station=dlg.m_nReportTrainInfo [i].nStation;
		int entry=-1;
		int exit=-1; 
		TRAIN_NO trainid, chgtrainid;
		memset(trainid, 0, sizeof(trainid));
		memset(chgtrainid, 0, sizeof(chgtrainid));
		strcpy(trainid,(LPCTSTR)dlg.m_nReportTrainInfo [i].strTrainId);
		int route=dlg.m_nReportTrainInfo [i].nRoute;
		TIME tm=long_time(dlg.m_nReportTrainInfo [i].usReceiveDate ,dlg.m_nReportTrainInfo [i].usReceiveTime);
		int iflag,oflag;
		if(dlg.m_nReportTrainInfo[i].fetch)
		{
			iflag= (byte)SCHD_ACCEPT;
		}
		else
		{
			iflag = (byte)SCHD_START;
		}

		if(dlg.m_nReportTrainInfo[i].giveout)
		{
			oflag = (byte)SCHD_GIVEOUT;
		}
		else
		{
			oflag = (byte)SCHD_TERMINAL;
		}
		int flag= dlg.m_nReportTrainInfo [i].nFlag ; // NA??? 无用
		BYTE nRunType=dlg.m_nReportTrainInfo [i].nRunType;
		m_tgmmi.AddTrain_Passway(0, trainid, chgtrainid, route,iflag,oflag,tm,nRunType);		
		GpcLogTrainMessage(0, 0, "列车操作 %s, run_type=%d批量添加",trainid,nRunType);

	}

}


void CTgEditView::OnManualOperCheck()
{
	if(GetTGFunctionConfig()->IfCheckManualOper())
	{
		GpcLogTrainMessage(0, 0, "[调度员操作] 通过点击选中运行图操作卡控按钮, 使用<运行图操作不卡控模式>进行操作");
		GetTGFunctionConfig()->SetCheckManualOper(FALSE);
	}
	else
	{
		GpcLogTrainMessage(0, 0, "[调度员操作] 通过点击选中运行图操作卡控按钮, 使用<运行图操作卡控模式>进行操作");
		GetTGFunctionConfig()->SetCheckManualOper(TRUE);
	}
}

void CTgEditView::OnUpdateManualOperCheck(CCmdUI *pCmdUI)
{
	if(GetTGFunctionConfig()->IfCheckManualOper())
		pCmdUI->SetCheck(1);
	else
		pCmdUI->SetCheck(0);
}

void CTgEditView::OnGetEngineTicketInfo()
{
	CTG_LogicObject *pobj=GetDocument()->GetCurrentActiveObject();
	if(!pobj)
		return;
	if(pobj->GetObjectType()!=TG_OBJ_TRAIN)
		return;

	CTG_SingleTrain *ptrain=(CTG_SingleTrain *)pobj;
	if(ptrain==NULL)
		return;

	TRAIN_INDEX shift_index=0;
	if(CTG_ScheduleInfo::SCHEDULE_TYPE_WORK==GetViewScheduleType())
	{
		shift_index=ptrain->GetShiftTrainIndex();
	}
	else
	{
		return;
	}
	if(shift_index!=0)
	{
		CTrainCrewTicket *pdlg = new CTrainCrewTicket(this, GetDocument(), shift_index, ptrain->GetTrainID()); 
		if(pdlg != NULL)
		{
			pdlg->Create(IDD_DIALOG_CREW_TICKET); 
			pdlg->ShowWindow(SW_SHOW);
		}
	}
}

void CTgEditView::OnUpdateGetEngineTicketInfo(CCmdUI *pCmdUI)
{
	if(GetDocument()->IsQueryMode()) 
	{
		pCmdUI->Enable(FALSE); 
		return;
	}
	if(CTG_ScheduleInfo::SCHEDULE_TYPE_WORK!=GetViewScheduleType())
	{
		pCmdUI->Enable(FALSE); 
		return;
	}

	if(GetTGFunctionConfig()->IsEnableTDMS5_0())
	{
		CTG_LogicObject *pobj=GetDocument()->GetCurrentActiveObject();
		if(pobj && pobj->GetObjectType()==TG_OBJ_TRAIN)
			pCmdUI->Enable(TRUE);
		else
			pCmdUI->Enable(FALSE);
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}

void CTgEditView::OnShiftGetShiftinfo()
{
	CTG_LogicObject *pobj=GetDocument()->GetCurrentActiveObject();
	if(pobj && pobj->GetObjectType()==TG_OBJ_TRAIN)
	{
		if(CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT==GetViewScheduleType())
		{
			CTG_SingleTrain* ptrain=(CTG_SingleTrain *)pobj;
			GetDocument()->RequestTrainExpand_KD_YL_CREW_FromOPMS(ptrain->GetTrainIndex(), 1);
		}
		if(CTG_ScheduleInfo::SCHEDULE_TYPE_WORK==GetViewScheduleType())
		{
			CTG_SingleTrain* ptrain=(CTG_SingleTrain *)pobj;
			TRAIN_INDEX shift_index=ptrain->GetShiftTrainIndex();
			GetDocument()->RequestTrainExpand_KD_YL_CREW_FromOPMS(shift_index, 2);
		}
	}
}

void CTgEditView::OnUpdateShiftGetShiftinfo(CCmdUI *pCmdUI)
{
	CTG_LogicObject *pobj=GetDocument()->GetCurrentActiveObject();
	if(pobj && pobj->GetObjectType()==TG_OBJ_TRAIN)
		pCmdUI->Enable(TRUE);
	else
		pCmdUI->Enable(FALSE);
}


void CTgEditView::OnHatTrain() 
{
	CInputHatTrain dlg(GetDocument());
	if(dlg.DoModal()==IDOK)
	{
		m_tgmmi.AddTrain_BasicSchedule(false, dlg.m_GraphicTrainId,dlg.m_HatTrainId);		
		GpcLogTrainMessage(0, 0, "列车操作 %s,戴帽车次 %s",dlg.m_GraphicTrainId,dlg.m_HatTrainId);
	}
}

void CTgEditView::OnUpdateElecArmsSet(CCmdUI *pCmdUI)
{
	if(!gpcdef.IsFuncMenuViewTgSchd(SCHD_TYPE::ELE_ARMS))
	{
		pCmdUI->Enable(FALSE);
	}
}

void CTgEditView::OnElecArmsSet()
{
	if(m_DlgArms.GetSafeHwnd() == NULL)
		return;
	CTgDocument *pdoc=GetDocument();
	m_DlgArms.SetSectionID(pdoc->GetCurrentSectionNo(),pdoc->IsQueryMode());
	m_DlgArms.OnBnClickedRefreshRemote(); // 窗口显示时,刷新下状态
	m_DlgArms.ShowWindow(SW_SHOW); 
}

void CTgEditView::OnUpdateOperRestore(CCmdUI *pCmdUI)
{
	if(GetDocument()->IsQueryMode()) 
	{
		pCmdUI->Enable(FALSE); 
	}
	else
		if(!UR_CanOperateSchedule())
		{
			pCmdUI->Enable(FALSE); 
		}
		else
			pCmdUI->Enable(TRUE); 
}

void CTgEditView::OnUpdateSaveBasicSideWorkInfo(CCmdUI *pCmdUI)
{
	if(GetViewScheduleType()==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC)
		pCmdUI->Enable(TRUE);
	else if(GetViewScheduleType()==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_BACKUP)
		pCmdUI->Enable(TRUE);
	else
		pCmdUI->Enable(FALSE);
}

void CTgEditView::OnUpdateDeleteBasic(CCmdUI *pCmdUI)
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

void CTgEditView::OnUpdateOperTGTrain(CCmdUI *pCmdUI)
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

// 单独定义助调对区间作业车的权限---一般情况下禁止助调绘制区间作业运行线，在某种可能下会造成本台列车索引和其他调度台重复
void CTgEditView::OnUpdateOperAddWorkTrain(CCmdUI *pCmdUI)
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

void CTgEditView::OnUpdateOperTGMark(CCmdUI *pCmdUI)
{
	if(!CheckUserTGRight()) 
	{
		pCmdUI->Enable(FALSE); 
		return;
	}

	if(IS_BASIC_SCHD(GetViewScheduleType())) // 预览基本图,不可以绘制施工符号
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
	if(right.operate==2 || right.operate==3 )
		pCmdUI->Enable(TRUE); 
	else
		pCmdUI->Enable(FALSE); 
}

void CTgEditView::OnUpdateOperTGOPMS(CCmdUI *pCmdUI)
{
	if(GetDocument()->IsQueryMode()) 
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
	if(IsReceiveTMISPlan(right.wintg_operate))
		pCmdUI->Enable(TRUE); 
	else
		pCmdUI->Enable(FALSE); 
}

void CTgEditView::OnUpdateSendPlan(CCmdUI *pCmdUI)
{
	if(GetViewScheduleType()!=CTG_ScheduleInfo::SCHEDULE_TYPE_WORK
		|| GetDocument()->IsQueryMode()) 
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
	if(IsSendPlanToStation(right.wintg_operate))
		pCmdUI->Enable(TRUE); 
	else
		pCmdUI->Enable(FALSE); 
}

void CTgEditView::OnBackOper()
{
	m_tgmmi.PopOper(m_schdinfo);
}

void CTgEditView::OnUpdateBackOper(CCmdUI *pCmdUI)
{
	if(m_tgmmi.CanBackOper())
		pCmdUI->Enable(TRUE); 
	else
		pCmdUI->Enable(FALSE); 
}

void CTgEditView::OnChangeTrainRoute()
{
	CTG_LogicObject *pobj=GetDocument()->GetCurrentActiveObject();
	if(!pobj)
		return;
	if(pobj->GetObjectType()==TG_OBJ_TRAIN)
	{
		CTG_SingleTrain *ptrain=(CTG_SingleTrain *)pobj;
		if (gpcdef.IsInhibitManualConvertActualTime())
		{
			if(ptrain->HasActualRecord()>=0)
			{
				AfxMessageBox("你要变更径路的列车存在实际点,不允许进行径路变更操作!\n但是你可以通过继续画线修改最后一个实际点后的运行线.");
				return;
			}
		}

		//太原 added 
		if(GetTGFunctionConfig()->IsCheckRouteforCrhAndImp())
		{
			TG_TrainInfo  traininfo=ptrain->GetTrainInfo()->m_data;
			if(ptrain->IsCRHTrain() || traininfo.super_rank>0 )
			{
				if(AfxMessageBox("确认对此动车/重点列车进行径路变更么？",MB_YESNO | MB_DEFBUTTON1 |MB_ICONQUESTION) != IDYES  )
					return;
			}
		}
		//end

		CChangeTrainRoute dlg(GetDocument()->GetCurrentSectionNo(),ptrain->GetTrainRecord(0)->GetStationNo(),0,this);  
		if(dlg.DoModal()==IDOK)
		{
			if(dlg.m_nRouteNo>0)
			{
				CWorkScheduleHolder  *pholder=GetDocument()->GetTGDataHolder();
				CTG_SingleTrain newtrain=*ptrain;
				CTG_ScheduleInfo schd;
				GetViewScheduleInfo(schd);
				if(pholder->GenTrain_ChangeRoute(newtrain,schd,dlg.m_nRouteNo))
				{
					GpcLogTrainMessage(0, 0, "列车操作 %s,更改列车径路,径路号:%d",ptrain->GetTrainID(),dlg.m_nRouteNo);
					// 每次人操作之后，判断径路是否相同
					if(GetTGFunctionConfig()->IsZeroGIDPasswayChange() && (newtrain.GetShiftTrainIndex()!=0))
					{
						if(!ptrain->IsSamePathway(newtrain))
						{
							GpcLogTrainMessage(0, 0, "列车%s(%u)的径路和原车不同清除全局ID.", newtrain.GetTrainID(), newtrain.GetTrainIndex());
							newtrain.SetShiftTrainIndex(0);
							newtrain.SetFromShift(FALSE);
						}
					}
					m_tgmmi.DeleteTrain_Directly(ptrain->GetTrainIndex());

					newtrain.SetTrainStartEndFlag();
					CTgDocument::SetTrainFromToConid(schd.m_nSectionID, &newtrain);

					m_tgmmi.AddTrain_Directly(newtrain);
				}
			}
		}
	}
}

BOOL  CTgEditView::CheckUserTGRight()
{
	if(GetDocument()->IsQueryMode()) 
	{
		return FALSE;
	}
	if(GetViewScheduleType()==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC)
	{
		return FALSE;
	}
	if(GetViewScheduleType()==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_BACKUP)
	{
		return FALSE;
	}
	if(GetViewScheduleType()==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_PREV && !UR_CanOperateBasicSchedule())
	{
		return FALSE;
	}
	if(GetViewScheduleType()==CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT)
	{
		return FALSE;
	}
	if(!UR_CanOperateSchedule())
	{
		return FALSE;
	}
	return TRUE;
}

void CTgEditView::OnSaveToFiles()
{
	GetDocument()->GetTGDataHolder()->SaveDataToLocalFiles(GetViewScheduleType());
}

void CTgEditView::OnLoadFromFiles()
{
	/*
	if(MessageBox("确认将本地保存的运行图数据恢复吗?","警告",MB_YESNO)==IDYES)
	{
	GetDocument()->GetTGDataHolder()->LoadDataFromLocalFiles(GetViewScheduleType());
	}*/
	CTG_TrainScheduleBase *pbase=GetDocument()->GetScheduleData(GetViewScheduleType());
	CDlgRestoreTrainList dlg(pbase);
	OnRecievedDataEnd(0,1);
	dlg.DoModal();
	OnRecievedDataEnd(0,0);
}

BOOL CTgEditView::RequestBasicScheduleFromOPMS(int type) // type==1 常用基本图   type==0 预览其本图
{
	CString token,content;
	if(TD_GenRequestMessage_19_BASICSCHEDULE(token,content,GetDocument()->GetCurrentSectionNo(),type))
	{ 
		long  eid=GetSpecialEntityID(SERVER_TYPE_TDCOMM); 
		TGSendStringMessage(token,content,eid);
		GpcLogTrainMessage(0,0,"向TD请求基本图 %d %s %s", eid, token, content);
		return TRUE;
	}
	return FALSE;
}

BOOL CTgEditView::ProcessXML_BasicScheduleTrain(const char *xml,const char *node)
{
	if(!m_bEnableProcessTdmsBasicInfo)
		return FALSE;

	if(GetViewScheduleType()!=CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_PREV)
		return FALSE;

	CTG_ScheduleInfo schdinfo;
	CWorkScheduleHolder *pHolder=GetDocument()->GetTGDataHolder();
	CTG_TrainScheduleBase *pData=pHolder->GetBasicPrevScheduleData();
	
	pData->GetScheduleInfo(schdinfo); 

	CTG_SingleTrain train;
	int opms_seq=0,opms_count=0;
	int schd_type=0;
	if(!TD_LoadBasicScheduleTrainFromXML(train,xml,node,&opms_seq,&opms_count,&schd_type,tdms_ctc_alarm))
	{
		GpcLogTrainMessage(0,0, "解析TDMS预览基本图失败");
		return FALSE;
	}
	
	if(opms_seq==1)
	{
		mTG.BeginBatchDraw(1,"开始从TDMS接收基本图"); // 
	}

	DWORD guid = train.GetShiftTrainIndex();
	AddTdmsTrainGuid(guid);

	CString waitstr;
	waitstr.Format("接收预览计划:%d/%d,当前计数:%d 车次 %s(%u)", opms_seq,opms_count,recieved_tdms_trains.size(),train.GetTrainID(),guid); 
	GpcLogTrainMessage(0,0, waitstr);
	if(opms_seq==opms_count)
	{
		if(recieved_tdms_trains.size()==opms_count)
			GetDocument()->OpenWaitWindow(NULL);
		else
		{
			CString waitstr;
			waitstr.Format("接收基本图完成,有车次丢失,总%d根 收到%d根,请重新获取", opms_count,recieved_tdms_trains.size()); 
			GetDocument()->OpenWaitWindow(waitstr);
			tdms_ctc_alarm.Add(waitstr);
			GpcLogTrainMessage(0,0, waitstr);
		}
		m_bEnableProcessTdmsBasicInfo=false;

		GpcLogSysMessage(0,"TDMS基本图接收完毕,设置界面未同步检查定时器");
		SetTimer(995,500,NULL);
	}
	else
	{
		CString waitstr;
		waitstr.Format("正在接收TDMS基本图车次 %s, %d/%d",train.GetTrainID(),opms_seq,opms_count); 
		GetDocument()->OpenWaitWindow(waitstr);
	}

	if(GetTGFunctionConfig()->IsCalBasicSchdExitEntry())
	{
		if(!pHolder->TGOCXSetTrainExitEntry(train, &mTG))
		{
			CString str;
			str.Format("车次:%s 径路计算失败,请确认列车径路", train.GetTrainID());
			tdms_ctc_alarm.Add(str);
		}
	}

	bool bOldSplitTrain=false;
	std::vector<CTG_SingleTrain> othertrain_list;
	if(pHolder->GenTrainBasicSchedule_OPMSx(train, othertrain_list, schdinfo, bOldSplitTrain))
	{
		pHolder->CheckTrainNo(train,tdms_ctc_alarm);
		CTgDocument::SetTrainReverseFlag(&train);
		CTgDocument::SetTrainFromToConid(schdinfo.m_nSectionID, &train);

		if(m_bSelTDMSTrainGenBasic)
		{
			m_BaseOfBasicTmp.AppendTrain(schdinfo,train,SYCFLAG_NULL); 
		}
		else
		{
			CTG_SingleTrain *pold=pData->GetSingleTrain(train.GetTrainIndex()); // OPMS计划中trainindex==shift_index
			if(pold)
			{
				if (GetTGFunctionConfig()->IsBasicDeletePassengerFreight())
					GpcLogTrainMessage(0,0,"获取TDMS基本图时,列车已经存在, 不再重复生成!");
				else
					pData->UpdateTrain(schdinfo,train,SYCFLAG_NULL); 
			}
			else
				pData->AppendTrain(schdinfo,train,SYCFLAG_NULL); 
		}
	}
	for(int i=0; i<othertrain_list.size(); i++)
	{
		CTG_SingleTrain othertrain=othertrain_list[i];
		if(othertrain.GetTrainRecordCount()>0)
		{
			if(GetTGFunctionConfig()->IsCalBasicSchdExitEntry())
			{
				GpcLogTrainMessage(0, 0, "ProcessXML_BasicScheduleTrain 画图控件开始计算出入口");

				int first_entry=255, last_exit=255;
				if(bOldSplitTrain)
				{	
					// 保存首站接入口、未站交出口
					CTG_TrainRecord *pfirst_rec=othertrain.GetTrainFirstRecord();
					CTG_TrainRecord *plast_rec=othertrain.GetTrainLastRecord();
					if(pfirst_rec)
						first_entry=pfirst_rec->GetEntry();
					if(plast_rec)
						last_exit=plast_rec->GetExit();
				}
				if(!pHolder->TGOCXSetTrainExitEntry(othertrain, &mTG, first_entry, last_exit))
				{
					CString str;
					str.Format("车次:%s 径路计算失败,请确认列车径路", othertrain.GetTrainID());
					tdms_ctc_alarm.Add(str);
				}
			}
			pHolder->CheckTrainNo(othertrain,tdms_ctc_alarm);
			CTgDocument::SetTrainReverseFlag(&othertrain);
			CTgDocument::SetTrainFromToConid(schdinfo.m_nSectionID, &othertrain);

			if(m_bSelTDMSTrainGenBasic)
			{
				m_BaseOfBasicTmp.AppendTrain(schdinfo,train,SYCFLAG_NULL); 
			}
			else
			{
			CTG_SingleTrain *pold=pData->GetSingleTrain(othertrain.GetTrainIndex()); // OPMS计划中trainindex==shift_index
			if(pold)
			{
				if (GetTGFunctionConfig()->IsBasicDeletePassengerFreight())
					GpcLogTrainMessage(0,0,"获取TDMS基本图时,列车 %d 已经存在, 不再重复生成!");
				else
					pData->UpdateTrain(schdinfo,othertrain,SYCFLAG_NULL); 
			}
			else
				pData->AppendTrain(schdinfo,othertrain,SYCFLAG_NULL); 
			}
		}
	}

	if(opms_seq==opms_count)
	{
		if(GetTGFunctionConfig()->IsBasicShowCompareDlg())
		{
			if(m_pBasicSchdCompareDlg!=NULL)
			{
				m_pBasicSchdCompareDlg->InitDlgCtrlItem();
				m_pBasicSchdCompareDlg->ShowWindow(SW_SHOW);
				GpcLogSysMessage(0,"TDMS基本图接收完毕,配置显示接收结果对话框");
			}
		}
		
		if(tdms_ctc_alarm.GetCount()>0)
		{
			CString text="请注意,基本图接收完成,请确认如下问题:";
			CDlgConfirm dlg(text, tdms_ctc_alarm, this);
			dlg.DoModal();
		
			GpcLogSysMessage(0,"TDMS基本图接收完毕,存在如下问题:");
			for(int n=0; n<tdms_ctc_alarm.GetCount(); n++)
			{
				GpcLogSysMessage(0, "%s", tdms_ctc_alarm.GetAt(n));
			}
		}
	}

	return TRUE;
}

void CTgEditView::OnSelTdmsTrainsGenBasic()
{
	CWorkScheduleHolder *pHolder=GetDocument()->GetTGDataHolder();
	if(pHolder==NULL)
		return;

	CDlgSelTDMSBasicGen* pdlg=new CDlgSelTDMSBasicGen(this, &m_BaseOfBasicTmp, pHolder);
	if(pdlg)
	{
		pdlg->Create(CDlgSelTDMSBasicGen::IDD, this);
		pdlg->ShowWindow(SW_SHOW);
	}
}

void CTgEditView::OnUpdateSelTdmsTrainsGenBasic(CCmdUI *pCmdUI)
{
	if(m_bSelTDMSTrainGenBasic)
		pCmdUI->Enable(TRUE);
	else
		pCmdUI->Enable(FALSE); 
}

BOOL  CTgEditView::ProcessXML_TrainCrew(const char *xml,const char *node)
{
	if(0!=stricmp(node,TOKEN_20_ACK_CREW_INFO))
		return FALSE;

	GpcLogTrainMessage(0,0,"CTgEditView type %d 收到TD返回 乘务信息 node:%s xml:%s", GetViewScheduleType(), node, xml);
	CWorkScheduleHolder *pHolder=GetDocument()->GetTGDataHolder();
	CTG_TrainScheduleBase *pData=NULL; 

	if(GetViewScheduleType()==CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT)
	{
		pData=pHolder->GetShiftScheduleData(); 
		GpcLogTrainMessage(0,0,"更新日班图计划乘务信息");
	}
	else if(GetViewScheduleType()==CTG_ScheduleInfo::SCHEDULE_TYPE_WORK)
	{
		pData=pHolder->GetWorkScheduleData(); 
		GpcLogTrainMessage(0,0,"更新调整图计划乘务信息");
	}
	if(!pData)
		return FALSE;

	CTG_ScheduleInfo schdinfo;
	pData->GetScheduleInfo(schdinfo); 

	CTD_TrainTicketList ticketList;
	if(!ticketList.LoadCrewFromTDXML(xml, node))
		return FALSE;

	DWORD gtid;
	CString info;
	for(int i=0; i<ticketList.m_nCount; i++)
	{
		gtid=ticketList.m_list[i].GID;
		info=ticketList.m_list[i].GetCrewInfo();
		if(info.IsEmpty() || gtid<=0)
			continue;

		//////////////////////////
		CTG_SingleTrain *ptrain=pData->GetSingleTrainByShiftIndex(gtid,NULL_STATION);
		if(!ptrain)
		{
			GpcLogTrainMessage(0,0,"找不到 GTID %d 对应的列车,乘务信息更新失败", gtid);
			continue;
		}
		const char *prevstr=ptrain->GetImportInfo(TRAIN_TXT_INFO_TYPE_CREW);
		if(prevstr && !stricmp(prevstr,info))  // 重复!
			continue;
		ptrain->SetImportInfo(info,TRAIN_TXT_INFO_TYPE_CREW);
		//发给服务器,但不更新界面
		ptrain->SetSyncStatus(GetDocument()->GetServerCommInterface()->GetInterfaceFlag(),FALSE);
	}

	return TRUE;
}

BOOL  CTgEditView::ProcessXML_YingyeKediao(const char *xml, const char *node)
{
	if(GetViewScheduleType()!=CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT && GetViewScheduleType()!=CTG_ScheduleInfo::SCHEDULE_TYPE_WORK)
		return FALSE;
	
	GpcLogTrainMessage(0,0,"CTgEditView type %d 收到TD返回 客调或营业信息 node:%s xml:%s", GetViewScheduleType(), node, xml);

	CWorkScheduleHolder *pHolder=GetDocument()->GetTGDataHolder();
	CTG_TrainScheduleBase *pData=NULL; 

	if(GetViewScheduleType()==CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT)
	{
		pData=pHolder->GetShiftScheduleData(); 
		GpcLogTrainMessage(0,0,"更新日班图计划客调营业信息");
	}
	else if(GetViewScheduleType()==CTG_ScheduleInfo::SCHEDULE_TYPE_WORK)
	{
		pData=pHolder->GetWorkScheduleData(); 
		GpcLogTrainMessage(0,0,"更新调整图计划客调营业信息");
	}
	if(!pData)
		return FALSE;

	CTG_ScheduleInfo schdinfo;
	pData->GetScheduleInfo(schdinfo); 

	CTG_SingleTrain *ptrain=TD_GetTrainInfoFromXML(pData,xml,node);
	if(ptrain) //发给服务器,但不更新界面
		ptrain->SetSyncStatus(GetDocument()->GetServerCommInterface()->GetInterfaceFlag(),FALSE);
	
	return TRUE;
}
BOOL  CTgEditView::ProcessXML_ShiftTrain(const char *xml,const char *node)
{
	if(!m_bEnableProcessTdmsShiftInfo)  
		return FALSE;

	if(GetViewScheduleType()!=CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT)
		return FALSE;
	CWorkScheduleHolder *pHolder=GetDocument()->GetTGDataHolder();
	CTG_TrainScheduleBase *pData=pHolder->GetShiftScheduleData(); 

	CTG_ScheduleInfo schdinfo;
	pData->GetScheduleInfo(schdinfo); 

	// 车次信息
	CTG_SingleTrain train;
	int opms_seq=0,opms_count=0;
	if(!TD_LoadShiftScheduleTrainFromXML(schdinfo,train,xml,node,&opms_seq,&opms_count,1, tdms_ctc_alarm))
	{
		GpcLogTrainMessage(0,0, "解析日班计划失败");
		return FALSE;
	}

	if(opms_count==0)
	{
		m_bEnableProcessTdmsShiftInfo=false;
		
		CString waitstr;
		waitstr.Format("接收日班计划完成,没有收到日班车次"); 
		GetDocument()->OpenWaitWindow(waitstr);
		GetDocument()->OpenWaitWindow(NULL);
		GpcLogTrainMessage(0,0, waitstr);
		return TRUE;
	}

	if(!GetTGFunctionConfig()->IsEnableNewCzh())
	{
		train.InitTrainExpand();
	}
	
	if(opms_seq==1)
	{
		mTG.BeginBatchDraw(1, "开始从TDMS接收日班计划"); // 
	}
	
	DWORD guid = train.GetShiftTrainIndex();
	AddTdmsTrainGuid(guid);
	
	CString waitstr;
	waitstr.Format("接收日班计划:%d/%d,当前计数:%d 车次 %s(%u)", opms_seq,opms_count,recieved_tdms_trains.size(),train.GetTrainID(),guid); 
	GpcLogTrainMessage(0,0, waitstr);

	if(opms_seq==opms_count)
	{
		m_bEnableProcessTdmsShiftInfo=false;
		if(recieved_tdms_trains.size()==opms_count)
			GetDocument()->OpenWaitWindow(NULL);
		else
		{
			CString waitstr;
			waitstr.Format("接收日班计划完成,有车次丢失,总%d根,收到%d根,请重新获取", opms_count,recieved_tdms_trains.size()); 
			GetDocument()->OpenWaitWindow(waitstr);
			tdms_ctc_alarm.Add(waitstr);
			GpcLogTrainMessage(0,0, waitstr);
		}
		GpcLogSysMessage(0,"TDMS日班计划接收完毕,设置界面未同步检查定时器");
		SetTimer(995,500,NULL);
	}
	else
	{
		CString waitstr;
		waitstr.Format("正在接收TDMS日班计划车次 %s, %d/%d",train.GetTrainID(),opms_seq,opms_count); 
		GetDocument()->OpenWaitWindow(waitstr);
	}

	// 超限,重点警卫,军运处理
	CTgDocument::SetTrainSpaceGaurdMilitaryRank(train);
	
	if(GetTGFunctionConfig()->IsCalShiftSchdExitEntry())
	{
		if(!pHolder->TGOCXSetTrainExitEntry(train, &mTG))
		{
			CString str;
			str.Format("车次:%s 径路计算失败,请确认列车径路", train.GetTrainID());
			tdms_ctc_alarm.Add(str);
		}
	}
	
	bool bOldSplitTrain=false;
	std::vector<CTG_SingleTrain> othertrain_list;
	if(pHolder->GenTrain_OPMSx(train,othertrain_list,schdinfo,TRUE,bOldSplitTrain))
	{
		pHolder->CheckTrainNo(train,tdms_ctc_alarm);
		CTgDocument::SetTrainReverseFlag(&train);
		CTgDocument::SetTrainFromToConid(schdinfo.m_nSectionID, &train);
		CTgDocument::CheckTdmsSideByStationside(schdinfo.m_nSectionID, train);
		
		CTG_SingleTrain *pold=pData->GetSingleTrain(train.GetTrainIndex());
		if(pold)  // 更新
		{
			if(train.GetTrainRecordCount()<=0)
			{
				GpcLogTrainMessage(0,0,"程序处理OPMS车次时,reccount==0");
			}

			if(!pold->CompareCoreData(train))  // 核心数据无变化
				pData->UpdateTrain(schdinfo,train,SYCFLAG_NULL); 
		}
		else   // 新增
		{
			pData->AppendTrain(schdinfo,train,SYCFLAG_NULL); 
		} 

		if(GetTGFunctionConfig()->IsGpcSendReqOtherMsg()) //TD自己请求train外的其它：客调，营业和乘务，运行图不在发送请求
		{
			GetDocument()->RequestTrainExpand_KD_YL_CREW_FromOPMS(train.GetTrainIndex(), 1);
		}
	}
	
	for(int i=0; i<othertrain_list.size(); i++) // 对于特殊分拆的列车后段更新
	{
		CTG_SingleTrain othertrain=othertrain_list[i];
		if(othertrain.GetTrainRecordCount()>0)
		{
			if(GetTGFunctionConfig()->IsCalShiftSchdExitEntry())
			{
				GpcLogTrainMessage(0, 0, "ProcessXML_ShiftTrain 画图控件开始计算出入口");
				
				int first_entry=255, last_exit=255;
				if(bOldSplitTrain)
				{
					// 保存首站接入口、未站交出口
					CTG_TrainRecord *pfirst_rec=othertrain.GetTrainFirstRecord();
					CTG_TrainRecord *plast_rec=othertrain.GetTrainLastRecord();
					if(pfirst_rec)
						first_entry=pfirst_rec->GetEntry();
					if(plast_rec)
						last_exit=plast_rec->GetExit();
				}
				if(!pHolder->TGOCXSetTrainExitEntry(othertrain, &mTG, first_entry, last_exit))
				{
					CString str;
					str.Format("车次:%s 径路计算失败,请确认列车径路", othertrain.GetTrainID());
					tdms_ctc_alarm.Add(str);
				}
			}
			pHolder->CheckTrainNo(othertrain,tdms_ctc_alarm);

			CTgDocument::SetTrainReverseFlag(&othertrain);
			CTgDocument::SetTrainFromToConid(schdinfo.m_nSectionID, &othertrain);
			CTgDocument::CheckTdmsSideByStationside(schdinfo.m_nSectionID, othertrain);
			
			CTG_SingleTrain *pold=pData->GetSingleTrain(othertrain.GetShiftTrainIndex()); // 班计划中trainindex==shift_index
			if(pold)
			{  
				if(!pold->CompareCoreData(othertrain))  // 核心数据无变化
					pData->UpdateTrain(schdinfo,othertrain,SYCFLAG_NULL); 
			}
			else
			{
				pData->AppendTrain(schdinfo,othertrain,SYCFLAG_NULL); 
			}
		}
	}
	
	if(opms_seq==opms_count)
	{		
		if(GetTGFunctionConfig()->IsShiftShowCompareDlg())
		{
			if(m_pShiftSchdCompareDlg!=NULL)
			{
				m_pShiftSchdCompareDlg->InitDlgCtrlItem();
				m_pShiftSchdCompareDlg->ShowWindow(SW_SHOW);
				GpcLogSysMessage(0,"TDMS日班计划接收完毕,配置显示接收结果对话框");
			}
		}

		if(tdms_ctc_alarm.GetCount()>0)
		{
			CString text="请注意,日班计划接收完成,请确认如下问题:";
			CDlgConfirm dlg(text, tdms_ctc_alarm, this);
			dlg.DoModal();
		
			GpcLogSysMessage(0,"TDMS日班计划接收完毕,存在如下问题:");
			for(int n=0; n<tdms_ctc_alarm.GetCount(); n++)
			{
				GpcLogSysMessage(0, "%s", tdms_ctc_alarm.GetAt(n));
			}
		}
	}

	return TRUE;
}


void CTgEditView::OnDeleteAllBasic()
{
	if (GetTGFunctionConfig()->IsBasicDeletePassengerFreight())
	{
		CDlgBasicTrainDeleteChoice dlg(1, this);
		if(dlg.DoModal()==IDOK)
		{
			int nResult = dlg.m_nChoice;
			if(nResult==1)
			{
				OnDeleteBasicTrains(0);
			}
			else if(nResult==2)
			{
				OnDeleteBasicTrains(1);
			}
			else if(nResult==3)
			{
				OnDeleteBasicTrains(2);
			}
		}
	}
	else
	{
		OnDeleteBasicTrains(0);
	}
}

void CTgEditView::OnDeleteBasicTrains(int type)
{
	if(GetViewScheduleType()==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC)
	{
		MessageBox("常用基本图不能删除!","提示");
		return;
	}
	else if(GetViewScheduleType()==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_BACKUP)
	{
		MessageBox("归档基本图不能删除!","提示");
		return;
	}

	if(GetViewScheduleType()!=CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_PREV)
		return;
	
	CString str;
	if(type==0)
		str="请注意,确认删除预览基本图中车次数据吗?";
	else if(type==1)
		str="请注意,确认删除预览基本图中客车车次数据吗?";
	else if(type==2)
		str="请注意,确认删除预览基本图中货车车次数据吗?";
	else
		return;
	if(MessageBox(str,"警告",MB_YESNO)==IDYES)
	{
		CTG_TrainScheduleBase *pData=GetDocument()->GetScheduleData(GetViewScheduleType());
		if(pData)
			pData->DirectDeleteTrainsInRange(0,0,TRUE,FALSE,type);  // 
		if (type==0)
			GpcLogTrainMessage(0, 0, "删除基本图车次，schetype=%d",GetViewScheduleType());
		else if (type==1)
			GpcLogTrainMessage(0, 0, "删除基本图客车车次，schetype=%d",GetViewScheduleType());
		else if (type==2)
			GpcLogTrainMessage(0, 0, "删除基本图货车车次，schetype=%d",GetViewScheduleType());
		////////// 加快删除显示更新速度 /////////
		m_tgmmi.SetTGUpdateFlag(1);
		GetDocument()->GetTGDataHolder()->PollingForUpdate(pData,&m_tgmmi,TRUE);
		m_tgmmi.SetTGUpdateFlag(0);
		mTG.RefreshTG(0);
	}
}


void CTgEditView::OnBasicGraphsavetotext() 
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

void CTgEditView::OnEnableFreeAddTrain()
{
	CSuperPassDlg pdlg("输入密码");
	if (pdlg.DoModal() == IDOK)
	{
		if (!pdlg.PasswordIsOk())
		{
			AfxMessageBox("密码错误！！！"); 
			return;
		}
	}
	else
	{
		return;
	}

	if(m_bEnableFreeAddTrain)
	{
		m_bEnableFreeAddTrain=FALSE;
		mTG.SetContrlRight(1);
		GpcLogTrainMessage(0, 0, "[调度员操作] 通过点击选中TDMS模式按钮, 使用<不允许自由画线模式>进行操作");
	}
	else
	{
		m_bEnableFreeAddTrain=TRUE;
		mTG.SetContrlRight(0);
		GpcLogTrainMessage(0, 0, "[调度员操作] 通过点击选中TDMS模式按钮, 使用<允许自由画线模式>进行操作");
	}
	m_tgmmi.EnableFreeTrainOper(m_bEnableFreeAddTrain);

}

void CTgEditView::OnUpdateEnableFreeAddTrain(CCmdUI *pCmdUI)
{
	if(m_bEnableFreeAddTrain)
		pCmdUI->SetCheck(0);
	else
		pCmdUI->SetCheck(1);		
}

void CTgEditView::OnGenNodeIndex()
{
	CSuperPassDlg pdlg("输入密码");
	if (pdlg.DoModal() == IDOK)
	{
		if ( !pdlg.PasswordIsOk())
		{
			AfxMessageBox("密码错误！！！"); 
			return;
		}
	}
	else
	{
		return;
	}

	m_tgmmi.GenTrainNodeIndex();
}

void CTgEditView::OnUpdateGenNodeIndex(CCmdUI *pCmdUI)
{
	if(UR_CanOperateBasicSchedule())
		pCmdUI->Enable(TRUE);
	else
		pCmdUI->Enable(FALSE);
}

void CTgEditView::OnDirectUpdateFromOPMS()
{
	CTG_LogicObject *pobj=GetDocument()->GetCurrentActiveObject();
	if(!pobj)
	{
		MessageBox("请选中运行线操作");
		return;
	}
	if(pobj->GetObjectType()!=TG_OBJ_TRAIN)
	{
		MessageBox("请选中运行线操作");
		return;
	}

	CTG_SingleTrain *ptrain=(CTG_SingleTrain *)pobj;
	if(ptrain->GetShiftTrainIndex()==NULL_TRAININDEX)
	{
		MessageBox("选中的运行线非日班计划车次");
		return;
	}
	CDlgTrainList dlg("接收TDMS更新",GetDocument(),m_schdinfo.m_nType,0,NULL);
	dlg.m_bDirectlyRequestMode=TRUE;
	dlg.AddRequestTrainGUID(ptrain->GetShiftTrainIndex(),ptrain->GetTrainID(),FALSE);
	dlg.SetTgMMI(&m_tgmmi);
	dlg.DoModal();

	CString tmp;
	tmp.Format("从TDMS更新车次 %s(index: %u ,shiftindex: %u) ",
		ptrain->GetTrainID(),ptrain->GetTrainIndex(),ptrain->GetShiftTrainIndex());
	GpcLogTrainMessage(0, 0, tmp);
}

void CTgEditView::OnMutiTrainMove()
{
	// TODO: 在此添加命令处理程序代码
	mTG.SetTrainDisplayFlag(TRAIN_DISPLY::MUTI_SEL, MUTI_SEL::TRAIN);
}

// 命名基本图
extern BOOL  ReqBasicName_ToServer(int con_id, bool bload);
void CTgEditView::OnBasicNameing()
{
	if(!gpcdef.GetOperatorPrior4Basic())
		return;

	CDlgBasicParam dlg(GetDocument(), GetViewScheduleType(), this); 
	dlg.DoModal();
	ReqBasicName_ToServer(m_schdinfo.m_nSectionID, true);
}

void CTgEditView::OnUpdateBasicNameing(CCmdUI* pCmdUI)
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

void CTgEditView::OnBasicDeleteName()
{
	if(!gpcdef.GetOperatorPrior4Basic())
		return;

	CDlgBasicDeleteName dlg(GetViewSectionNo(),GetDocument(),this); 
	dlg.DoModal();
}

void CTgEditView::OnUpdateBasicDeleteName(CCmdUI* pCmdUI)
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

void CTgEditView::OnUpdateBasicGraphchange(CCmdUI* pCmdUI)
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

void CTgEditView::OnBasicGraphchange()
{
	if(gpcdef.GetUserIdentify() == GPC_DISPATCHER)
	{
		if(GetViewScheduleType()!=CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_BACKUP)
			return;
		if(!gpcdef.GetOperatorPrior4Basic())
			return;
	}
	else if(gpcdef.GetUserIdentify() == GPC_TEST)
	{
		if(!gpcdef.GetOperatorPrior4Basic())
			return;
	}
	else
	{
		return;
	}

	if(mServer_nosync_count > 0 || mMmi_nosync_count > 0)
	{
		CString msg;
		msg.Format("存在未同步数据 服务器:%d  界面:%d, 请等待同步个数为0再换图", 
			mServer_nosync_count, mMmi_nosync_count);
		AfxMessageBox(msg);
		return;
	}

	CDlgBasicChange dlg(GetDocument(), GetViewScheduleType()); 
	dlg.DoModal();
	if(dlg.m_bSucceed)
	{
		CWintg* pApp=(CWintg *)AfxGetApp();
		if(dlg.m_nObjSchdType == CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC)
		{
			CWorkScheduleHolder *pholder=GetDocument()->GetTGDataHolder();
			pApp->ClearTGOCXSchedule(m_schdinfo.m_nSectionID, CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC);

			pholder->ReloadData(CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC,TRUE); 
		}
		else if(dlg.m_nObjSchdType == CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_PREV)
		{
			CWorkScheduleHolder *pholder=GetDocument()->GetTGDataHolder();
			pApp->ClearTGOCXSchedule(m_schdinfo.m_nSectionID, CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_PREV);

			pholder->ReloadData(CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_PREV,TRUE); 
		}

	}
}

// 基本图修改开关
void CTgEditView::OnModifyBschd()
{
	// TODO: 在此添加命令处理程序代码
	mTG.SetTrainDisplayFlag(TRAIN_DISPLY::BSCHD_OPER, 0);
}

void CTgEditView::OnUpdateModifyBschd(CCmdUI* pCmdUI)
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
void CTgEditView::OnDivideportLatetime()
{
	CTG_LogicObject *pobj=GetDocument()->GetCurrentActiveObject();
	if(!pobj)
		return;
	if(pobj->GetObjectType()==TG_OBJ_TRAIN)
	{
		CTG_SingleTrain srctrain=*(CTG_SingleTrain *)pobj;

		CDlgDividePortLateTime dlg(&srctrain, this);

		if(dlg.DoModal()==IDOK)
		{
			m_tgmmi.UpdateTrain_Directly(srctrain);
		}
	}
}

void CTgEditView::OnUpdateTrainPlanSideMan(CCmdUI* pCmdUI)
{
	if(gpcdef.GetOperatorPrior4Basic() && CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_PREV==GetViewScheduleType() && gpcdef.GetUserIdentify() == GPC_TEST)
	{
		pCmdUI->Enable(TRUE);
	}
	else if(CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT==GetViewScheduleType())
	{
		pCmdUI->Enable(TRUE);
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}
void CTgEditView::OnTrainPlanSideMan()
{
	if(m_pTrainSideDlg)
	{
		m_pTrainSideDlg->SetDocument(GetDocument(), 0);
		m_pTrainSideDlg->InitDlgCtrlItem();
		m_pTrainSideDlg->ShowWindow(SW_SHOW);
	}
}
void CTgEditView::OnTrainPlanRouteMan()
{
	if(m_pTrainRouteDlg)
	{
		m_pTrainRouteDlg->SetDocument(GetDocument(), GetViewScheduleType(), 0);
		m_pTrainRouteDlg->InitDlgCtrlItem();
		m_pTrainRouteDlg->ShowWindow(SW_SHOW);
	}
}

void CTgEditView::OnUpdateTrainPlanRouteMan(CCmdUI* pCmdUI)
{
	if(gpcdef.GetOperatorPrior4Basic() && CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_PREV==GetViewScheduleType() && gpcdef.GetUserIdentify() == GPC_TEST)
	{
		pCmdUI->Enable(TRUE);
	}
	else if(CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT==GetViewScheduleType())
	{
		pCmdUI->Enable(TRUE);
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}

void CTgEditView::OnTrainPlanCompareMan()
{
	if(CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_PREV==GetViewScheduleType())
	{
		if(m_pBasicSchdCompareDlg!=NULL)
		{
			m_pBasicSchdCompareDlg->InitDlgCtrlItem();
			m_pBasicSchdCompareDlg->ShowWindow(SW_SHOW);
		}
	}
	else if(CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT==GetViewScheduleType())
	{
		if(m_pShiftSchdCompareDlg!=NULL)
		{
			m_pShiftSchdCompareDlg->InitDlgCtrlItem();
			m_pShiftSchdCompareDlg->ShowWindow(SW_SHOW);
		}
	}
	else if(CTG_ScheduleInfo::SCHEDULE_TYPE_WORK==GetViewScheduleType())
	{
		if(m_pWorkSchdCompareDlg!=NULL)
		{
			m_pWorkSchdCompareDlg->InitDlgCtrlItem();
			m_pWorkSchdCompareDlg->ShowWindow(SW_SHOW);
		}
	}
}

void CTgEditView::OnUpdateTrainPlanCompareMan(CCmdUI* pCmdUI)
{
	if(gpcdef.GetOperatorPrior4Basic() && CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_PREV==GetViewScheduleType() && gpcdef.GetUserIdentify() == GPC_TEST)
	{
		pCmdUI->Enable(TRUE);
	}
	else if(CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT==GetViewScheduleType())
	{
		pCmdUI->Enable(TRUE);
	}
	else if(CTG_ScheduleInfo::SCHEDULE_TYPE_WORK==GetViewScheduleType())
	{
		pCmdUI->Enable(TRUE);
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}

void CTgEditView::OnDeleteTrainByIndex() //20130222hjz 增加索引号删除列车功能
{
	if (GetDocument()->IsQueryMode() || !UR_CanOperateSchedule())
	{
		AfxMessageBox("没有操作权限!");
		return;
	}
	/////////////////////////////按照索引号删除列车//////////////////////////////////
	CDlgDeleteTrainByIndex dlg = new CDlgDeleteTrainByIndex();
	if (dlg.DoModal() == IDOK)
	{
		if(dlg.m_nTrainIndex!=0)
		{
			//////////////////////////////////////////////////////
			CWorkScheduleHolder *pholder=GetDocument()->GetTGDataHolder();
			CTG_TrainScheduleBase *pdata=pholder->GetWorkScheduleData();

			CTG_SingleTrain *pTrain;
			pTrain = pdata->GetSingleTrain(dlg.m_nTrainIndex);
			if (!pTrain)
			{
				AfxMessageBox("不存在此趟列车,请重新检查列车索引号后再进行删除操作!");
				return;
			}
			else
			{
				m_tgmmi.DeleteTrain_Directly(dlg.m_nTrainIndex);
				return;
			}
		}
		else
			AfxMessageBox("未输入车次索引号!");
	}
}
void CTgEditView::OnDeleteMark()
{
	// TODO: 在此添加命令处理程序代码
	mTG.AliveWindow(2);
}

void CTgEditView::OnDeleteInvalidTrain()
{
	// TODO: 在此添加命令处理程序代码
	mTG.AliveWindow(8);
}

void CTgEditView::OnSaveBasicSideWorkInfo()
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

void CTgEditView::OnLoadBasicSideWorkInfo()
{
	if(GetViewScheduleType()!=CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_PREV)
		return;

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

	if(MessageBox("请注意, 你确认匹配预览基本图信息吗?","警告",MB_YESNO)!=IDYES)
		return;

	CDlgWait *pw=new CDlgWait("正在加载基本图信息,可能需要1-2分钟时间,请稍侯...");

	CTgDocument  *pDoc  = GetDocument();

	CString fileDir, fileName;
	fileDir.Format("%s\\xml",gpcdef.GetWorkPathName());
	fileName.Format("section_basic%02d.xml",pDoc->GetCurrentSectionNo());
	fileName = fileDir + "\\" + fileName;

	pDoc->LoadTrainSideTechToFile(fileName);

	// 开始匹配
	CString strMsg="匹配列车: ";
	CTG_TrainScheduleBase *pData=pDoc->GetScheduleData(GetViewScheduleType());
	if(pData)
	{
		CTG_TrainPtrGroup  group;
		pData->FindTrains(group);

		for(int i=0; i<group.GetTrainCount(); i++)
		{
			const CTG_SingleTrain* oldtrain = group.GetTrainPtr(i);
			if(NULL == oldtrain)
				continue;

			CTG_SingleTrain train = *oldtrain;
			pw->UpdateDisplay(strMsg+train.GetTrainID());

			if(pDoc->InitBasicInfo(&train))
			{
				m_tgmmi.UpdateTrain_ByBasicInfo(train);
			}
			else
			{
				GpcLogTrainMessage(0, 0, "匹配基本图信息, %s 失败", train.GetTrainID());
			}
		}
	}

	pDoc->ReleaseBasicTrainInfo();
	delete pw;
}

void CTgEditView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	short type = nChar;
	if(type == nChar) // 数据丢失
		mTG.SetTrainDisplayFlag(TRAIN_DISPLY::KEY_DOWN, type);

	CTgView::OnKeyDown(nChar, nRepCnt, nFlags);
}
void CTgEditView::OnAutoAdjustCol()
{
	int station, rec_index;
	CTG_LogicObject *pobj=GetDocument()->GetCurrentActiveObject(&station, &rec_index);
	if(!pobj)
		return;
	if(pobj->GetObjectType()==TG_OBJ_TRAIN)
	{
		CTG_SingleTrain train=*(CTG_SingleTrain *)pobj;
		CTG_TrainRecord *prec=train.GetTrainRecordByRecIndex(rec_index);
		if(!prec)
			return;
		if(prec->GetStationNo()==station)
			m_tgmmi.AdjustTrainForCollision(train, rec_index);
	}
	else if(pobj->GetObjectType()==TG_OBJ_MARK) // 自动根据标记调整列车
	{
		CTG_SingleMark *pmark=(CTG_SingleMark *)pobj;
		m_tgmmi.AdjustTrainsForMark(pmark);
	}
}

void CTgEditView::OnAutoAdjustPass()
{
	int station, rec_index;
	CTG_LogicObject *pobj=GetDocument()->GetCurrentActiveObject(&station, &rec_index);
	if(!pobj)
		return;
	if(pobj->GetObjectType()==TG_OBJ_TRAIN)
	{
		CTG_SingleTrain train=*(CTG_SingleTrain *)pobj;
		CTG_TrainRecord *prec=train.GetTrainRecordByRecIndex(rec_index);
		if(!prec)
			return;
		if(prec->GetStationNo()==station)
			m_tgmmi.AdjustTrainForMutiPass(train, rec_index);
	}
}
void CTgEditView::OnAutoAdjustMulti()
{
	CTG_TrainScheduleBase *pData=GetDocument()->GetScheduleData(GetViewScheduleType());
	CDlgMultiAdjust dlg(pData,this);
	if(dlg.DoModal()==IDOK)
	{
		TRAIN_INDEX  trainidxs[100];
		int traincount=dlg.GetSelectedTrains(trainidxs,100);
		for(int i=0;i<traincount;i++)
		{
			CTG_SingleTrain train,*ptrain;
			ptrain=pData->GetSingleTrain(trainidxs[i]);
			if(ptrain)
				ptrain->SetAdjustingFlag(TRUE);  // 批量调整时设置标志，以便于同等级列车避让
		}

		for(int pri=0;pri<=MAX_PRIORITY_VALUE;pri++)  // 优先高优先级列车,priority值小的列车
		{
			for(int i=0;i<traincount;i++)
			{
				if(trainidxs[i]==NULL_TRAIN_INDEX)
					continue;

				CTG_SingleTrain train,*ptrain;
				ptrain=pData->GetSingleTrain(trainidxs[i]);
				if(!ptrain)
					continue;
				if(ptrain->GetPriority()!=pri)
					continue;
				train=*ptrain;
				m_tgmmi.AdjustTrainForCollision(train,0);

				ptrain=pData->GetSingleTrain(trainidxs[i]);
				if(ptrain)
					ptrain->SetAdjustingFlag(FALSE);  // 批量调整时设置标志，以便于同等级列车避让

				trainidxs[i]=NULL_TRAIN_INDEX;
			}
		}
	}
}

void CTgEditView::OnOptAutoAdjustCollision()
{
	if(GetTGFunctionConfig()->GetAutoAdjustOperFlag()==1)
	{
		CString tmp;
		tmp.Format("该操作将进入 <不自动调整模式> 模式进行操作,确认继续吗?");

		if(MessageBox(tmp,"警告",MB_YESNO)!=IDYES)
			return;

		GpcLogTrainMessage(0, 0, "[调度员操作] 通过点击选中自动调整按钮, 使用<不自动调整模式>进行操作");
		GetTGFunctionConfig()->SetAutoAdjustOperFlag(0);
	}
	else
	{
		CString tmp;
		tmp.Format("该操作将进入 <自动调整模式> 模式进行操作,可能修改列车运行线的到发时刻和股道,调整运行线后应检查核对,确认继续吗?");

		if(MessageBox(tmp,"警告",MB_YESNO)!=IDYES)
			return;

		GpcLogTrainMessage(0, 0, "[调度员操作] 通过点击选中自动调整按钮, 使用<自动调整模式>进行操作");
		GetTGFunctionConfig()->SetAutoAdjustOperFlag(1);
	}
}

void CTgEditView::OnUpdateOptAutoAdjustCollision(CCmdUI *pCmdUI)
{
	if(GetTGFunctionConfig()->GetAutoAdjustOperFlag()==1)
		pCmdUI->SetCheck(1);
	else
		pCmdUI->SetCheck(0);
}

void CTgEditView::OnUpdateDirectSetAutotrig(CCmdUI *pCmdUI)
{

	// TODO: 在此添加命令更新用户界面处理程序代码
}

void CTgEditView::OnDirectSetAutotrig()
{
	int station, rec_index;
	CTG_LogicObject *pobj=GetDocument()->GetCurrentActiveObject(&station, &rec_index);
	do
	{
		if(!pobj)
			break;
		if(pobj->GetObjectType()!=TG_OBJ_TRAIN)
			break;
		CTG_SingleTrain train=*(CTG_SingleTrain *)pobj;
		CTG_TrainRecord *prec=train.GetTrainRecordByRecIndex(rec_index);
		if(!prec)
			break;
		if(prec->GetStationNo()!=station)
			break;
		
		if(GetTGFunctionConfig()->IsNotThisSectionStation(m_schdinfo.m_nSectionID, prec->m_data.station))
		{
			CString str;
			str.Format("请注意, %s 不属于本台管辖,不能修改自触", gpcdef.GetStationName(prec->m_data.station));
			MessageBox(str, "提示", MB_OK);
			GpcLogTrainMessage(0, 0, "%s",  str);
			return;
		}
		DirectSetAutotrig(train, rec_index);
		return;
	}while(0);
	MessageBox("请选择车次上的车站节点,然后再操作.","提示");
}

void CTgEditView::DirectSetAutotrig(CTG_SingleTrain& train, const int rec_index)
{
	CDlgDirectSetTrigFlag  dlg(this);
	dlg.SetTrain(&m_tgmmi,&train,rec_index);
	if(dlg.DoModal()==IDOK)
	{
		m_tgmmi.UpdateTrain_Directly(train);
	}
}

void CTgEditView::OnPredictStpc()
{
	if(!UR_CanOperateSchedule())
	{
		MessageBox("你没有预告权限", "提示", MB_OK|MB_ICONINFORMATION);
		return;
	}

	if(m_predictDlg)
	{
		m_predictDlg->ShowWindow(SW_SHOW);
	}

	return;
}

void CTgEditView::OnTimer(UINT nIDEvent)
{
	//if(nIDEvent == 198302)
	//{
	//	// 只检查界面，单机版可用
	//	CWorkScheduleHolder  *pholder=GetDocument()->GetTGDataHolder();
	//	if(!pholder)
	//	{
	//		GetDocument()->OpenWaitWindow(NULL);
	//		KillTimer(198302);
	//	}
	//	else
	//	{
	//		int m_count=pholder->GetNoSyncedObjectCount(m_schdinfo.m_nType,&m_tgmmi);
	//		if(m_count<=0)
	//		{
	//			mTG.AliveWindow(16);
	//			GetDocument()->OpenWaitWindow(NULL);

	//			KillTimer(198302);
	//		}
	//	}
	//}
	if(nIDEvent == 198311)
	{
		if(m_pPredictServer!=NULL)
			m_pPredictServer->SaveStplanToHistory();
	}
	
	CTgView::OnTimer(nIDEvent);
}

void CTgEditView::OnTurnReality()
{
	if(gpcdef.IsInhibitManualConvertActualTime())
	{
		AfxMessageBox("禁止修改实际点,不能转实际");
		GpcLogTrainMessage(0, 0, "禁止修改实际点,不能转实际");
	}
	else
	{
		mTG.TurnReality();
	}
}

void CTgEditView::OnReceivePoint()
{
	mTG.ReceivePoint();
}

void CTgEditView::OnUpdateReceivePoint(CCmdUI* pCmdUI)
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
		return;
	}
	if(right.operate==1 || right.operate==3)
		pCmdUI->Enable(TRUE); 
	else
		pCmdUI->Enable(FALSE); 

	pCmdUI->SetCheck(mTG.GetTrainDisplayFlag(TRAIN_DISPLY::REVERSE, REVERSE::RECEIVE_POINT));
}

void CTgEditView::OnGenAllReturnTrainid()
{
	// TODO: 在此添加命令处理程序代码
	GpcLogTrainMessage(0, 0, "[调度员操作] 进行 批量生成折返车次 功能");
	mTG.AliveWindow(16);
}

void CTgEditView::OnEditAllCzh()
{
	// TODO: 在此添加命令处理程序代码
	GpcLogTrainMessage(0, 0, "[调度员操作] 进行 批量车组编辑 功能");
	mTG.AliveWindow(32);
}

// 日班计划修改开关
void CTgEditView::OnModifyShiftschd()
{
	mTG.SetTrainDisplayFlag(TRAIN_DISPLY::REVERSE, REVERSE::SHIFT_SCHD_OPER);
}

void CTgEditView::OnUpdateModifyShiftschd(CCmdUI* pCmdUI)
{
	if(CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT==GetViewScheduleType() && 
		(gpcdef.GetUserIdentify() == GPC_TEST || 
		gpcdef.GetUserIdentify() == GPC_DISPATCHER || 
		gpcdef.GetUserIdentify() == GPC_OPERATOR) && GetTGFunctionConfig()->IsEnableCzh())
	{
		pCmdUI->Enable(TRUE);
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}

	short nCanModify = mTG.GetTrainDisplayFlag(TRAIN_DISPLY::REVERSE, REVERSE::SHIFT_SCHD_OPER);
	if(nCanModify > 0)
		pCmdUI->SetCheck(1);
	else
		pCmdUI->SetCheck(0);
}

void CTgEditView::OnUpdateEditAllCzh(CCmdUI* pCmdUI)
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
	else if(GetViewScheduleType()==CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT)
	{
		if(GetTGFunctionConfig()->IsEnableCzh() || GetTGFunctionConfig()->IsEnableNewCzh())
		{
			short nCanModify = mTG.GetTrainDisplayFlag(TRAIN_DISPLY::REVERSE, REVERSE::SHIFT_SCHD_OPER);
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
	else if(GetViewScheduleType()==CTG_ScheduleInfo::SCHEDULE_TYPE_WORK)
	{
		if(GetTGFunctionConfig()->IsEnableCzh() || GetTGFunctionConfig()->IsEnableNewCzh())
			pCmdUI->Enable(TRUE);
		else
			pCmdUI->Enable(FALSE);
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}

void CTgEditView::OnUpdateGenAllReturnTrainid(CCmdUI *pCmdUI)
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
		else if(GetViewScheduleType()==CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT)
		{
			if(GetTGFunctionConfig()->IsEnableCzh())
			{
				short nCanModify = mTG.GetTrainDisplayFlag(TRAIN_DISPLY::REVERSE, REVERSE::SHIFT_SCHD_OPER);
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
		else if(GetViewScheduleType()==CTG_ScheduleInfo::SCHEDULE_TYPE_WORK)
		{
			if(GetTGFunctionConfig()->IsEnableCzh())
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

void CTgEditView::OnSetMultiSideLock()
{
	mTG.AliveWindow(64);
}

void CTgEditView::OnUpdateSetMultiSideLock(CCmdUI* pCmdUI)
{
	if(CTG_ScheduleInfo::SCHEDULE_TYPE_WORK==GetViewScheduleType() && 
		(gpcdef.GetUserIdentify() == GPC_TEST || 
		gpcdef.GetUserIdentify() == GPC_DISPATCHER || 
		gpcdef.GetUserIdentify() == GPC_OPERATOR))
	{
		pCmdUI->Enable(TRUE);
	}
	else
		pCmdUI->Enable(FALSE);
}

void CTgEditView::OnQuickModifyMultiTrains()
{
	if(CTG_ScheduleInfo::SCHEDULE_TYPE_WORK==GetViewScheduleType() && 
		(gpcdef.GetUserIdentify() == GPC_TEST || 
		gpcdef.GetUserIdentify() == GPC_DISPATCHER || 
		gpcdef.GetUserIdentify() == GPC_OPERATOR))
	{
		mTG.AliveWindow(128);
	}
}

void CTgEditView::SetLateEarlyTimeRd(LPCTSTR xml, BYTE flag, ULONG shiftid)
{
	CString str= xml;
	std::map<CString, LateInfo>::iterator it = m_earlylatetimelog.find(str);

	if(it != m_earlylatetimelog.end())
	{
		it->second.flag=flag;
	}
	else
	{
		LateInfo info;
		info.flag=flag;
		info.shiftid=shiftid;

		m_earlylatetimelog[str]= info;
	}
	mTG.AppendEarlyLateInfo(str, flag, shiftid);

	if(flag&0x3)
		sysprint(10,0, "调度员设置列车 %s 正晚点统计:%s %s", str, (flag&0x01)?"到点":"",  (flag&0x02)?"发点":"");
	else if(flag&0x4)
		sysprint(10,0, "调度员设置列车 %s 不统计正晚点", str);

	SaveEarlylatetimeFile();
}


extern const char *TG_GetScheduleBaseFilePath(const CTG_ScheduleInfo  &schdinfo);
bool CTgEditView::LoadEarlylatetimeXML()
{
	ULONG shiftids[5];
	shiftids[0]=Get_CurrentShiftid();
	shiftids[1]=Get_PreShiftid(shiftids[0]);
	shiftids[2]=Get_PreShiftid(shiftids[1]);
	shiftids[3]=Get_NextShiftid(shiftids[0]);
	shiftids[4]=Get_NextShiftid(shiftids[3]);

	CString strFilename;
	strFilename.Format("%s\\early_late_time_set.xml", TG_GetScheduleBaseFilePath(m_schdinfo));

	MSXML::IXMLDOMDocumentPtr pXMLDoc;
	HRESULT hr = pXMLDoc.CreateInstance(MSXML::CLSID_DOMDocument);
	if(FAILED(hr))
	{
		return false;
	}

	if(VARIANT_TRUE != pXMLDoc->load( _bstr_t( strFilename) ) )
	{
		return false; 
	}

	_variant_t v;
	MSXML::IXMLDOMNodePtr pNode,pChildNode, pRoot;
	pRoot=pXMLDoc->selectSingleNode("root");

	MSXML::IXMLDOMNodeListPtr pList = pRoot->selectNodes( "record");
	if(pList)
	{
		for(int idx = 0; idx < pList->Getlength(); idx++)
		{
			pNode = pList->Getitem(idx);
			if(pNode)
			{
				CString key;
				BYTE val=0;
				ULONG shiftid=0;
				pChildNode=pNode->selectSingleNode("key");
				if(pChildNode)
				{
					v = pChildNode->GetnodeTypedValue();
					key = v.bstrVal;
					SysFreeString(v.bstrVal);
					v.Clear();
				}
				pChildNode=pNode->selectSingleNode("tm");
				if(pChildNode)
				{
					v = pChildNode->GetnodeTypedValue();
					shiftid = atol(CString(v.bstrVal));
					SysFreeString(v.bstrVal);
					v.Clear();
				}
				pChildNode=pNode->selectSingleNode("val");
				if(pChildNode)
				{
					v = pChildNode->GetnodeTypedValue();
					val = atoi(CString(v.bstrVal));
					SysFreeString(v.bstrVal);
					v.Clear();
				}

				for(int i=0; i<5; i++)
				{
					if(shiftids[i]==shiftid)
						break;
				}
				if( i>=5)
				{
					continue;
				}

				if(key!="" && val>0 && shiftid>0)
				{
					mTG.AppendEarlyLateInfo(key, val, shiftid);

					LateInfo info;
					info.flag=val;
					info.shiftid=shiftid;
					m_earlylatetimelog.insert(make_pair(key, info));
				}
			}
		}
	}

	return true;
}

void CTgEditView::SaveEarlylatetimeFile()
{
	ULONG shiftids[5];
	shiftids[0]=Get_CurrentShiftid();
	shiftids[1]=Get_PreShiftid(shiftids[0]);
	shiftids[2]=Get_PreShiftid(shiftids[1]);
	shiftids[3]=Get_NextShiftid(shiftids[0]);
	shiftids[4]=Get_NextShiftid(shiftids[3]);

	CString strFilename;
	strFilename.Format("%s\\early_late_time_set.xml",  TG_GetScheduleBaseFilePath(m_schdinfo));
	CStdioFile file;
	if(file.Open(strFilename, CFile::modeCreate|CFile::modeWrite))
	{
		file.WriteString("<?xml version=\"1.0\" encoding=\"GB2312\"?>\n");
		file.WriteString("<root>\n");
		std::map<CString, LateInfo>::iterator it = m_earlylatetimelog.begin();
		for(; it!=m_earlylatetimelog.end(); it++)
		{
			for(int i=0; i<5; i++)
			{
				if(shiftids[i]==it->second.shiftid)
					break;
			}
			if( i>=5)
			{
				continue;
			}

			CString xml;
			file.WriteString("\t<record>\n");	
			xml.Format("\t\t<key>%s</key>\n", it->first);
			file.WriteString(xml);
			xml.Format("\t\t<tm>%d</tm>\n", it->second.shiftid);
			file.WriteString(xml);
			xml.Format("\t\t<val>%d</val>\n", it->second.flag);
			file.WriteString(xml);
			file.WriteString("\t</record>\n");
		}
		file.WriteString("</root>\n");
		file.Close();
	}
}

extern void CaculateTrainSlowTime(CTG_SingleTrain *single);
void CTgEditView::MutiTrainOper(LPCTSTR xml, ULONG nOperFlag)
{
	// 只能批量处理调整图
	if(GetViewScheduleType()!=CTG_ScheduleInfo::SCHEDULE_TYPE_WORK)
		return;
	CWorkScheduleHolder *pHolder=GetDocument()->GetTGDataHolder();
	if(NULL==pHolder)
		return;
	CTG_TrainScheduleBase *pData=pHolder->GetWorkScheduleData();
	if(NULL==pData)
		return;

	MSXML::IXMLDOMDocumentPtr pXMLDoc;
	HRESULT hr = pXMLDoc.CreateInstance(MSXML::CLSID_DOMDocument);
	if(FAILED(hr))
	{
		_com_error er(hr);
		AfxMessageBox(er.ErrorMessage());
		return;
	}
	
	if(VARIANT_TRUE != pXMLDoc->loadXML( _bstr_t( xml ) ) )
	{
		return;
	}
	
	_variant_t v;
	
    MSXML::IXMLDOMNodePtr pNode, pChildNode;
	MSXML::IXMLDOMNodePtr pRootNode = pXMLDoc->selectSingleNode( "root");
	if(pRootNode == NULL)
		return;
	
	std::vector<DWORD> list_index;
	MSXML::IXMLDOMNodeListPtr pRecAbsList = pRootNode->selectNodes( "TRAIN_INDEX" );
	for(int idx = 0; idx < pRecAbsList->Getlength(); idx++)
	{
		pNode = pRecAbsList->Getitem(idx);
		if(pNode)
		{
			v = pNode->GetnodeTypedValue();
			DWORD train_index = atol(CString(v.bstrVal));
			list_index.push_back(train_index);
			SysFreeString(v.bstrVal);
			v.Clear();
		}
	}
	if(list_index.size() == 0)
		return;

	CTG_ScheduleInfo schdinfo;	
	pData->GetScheduleInfo(schdinfo); 
	
	if(nOperFlag==0x00000001) // 批量锁定股道
	{
		OnRecievedDataEnd(0,1);
		for(int i=0; i<list_index.size(); i++)
		{
			CTG_SingleTrain *pTrain1=pData->GetSingleTrain(list_index[i]);
			if(NULL!=pTrain1)
			{
				bool bSend=false;
				CTG_SingleTrain train=*pTrain1;
				for(int i=0; i<train.GetTrainRecordCount(); i++)
				{
					CTG_TrainRecord *prec = train.GetTrainRecordNoDelete(i);
					if(NULL != prec)
					{
						if(prec->IsLockSide())
							continue;
						if(prec->IsActualArriveTimeRecord() || prec->IsActualDepartTimeRecord())
						{
							sysprint(4,0,"[批量股道锁定] 调度员设置列车:%s 在车站:%s 存在实际点, 股道不锁定, 股道:%d %d", 
								train.GetTrainID(), gpcdef.GetStationName(prec->m_data.station),prec->m_data.arrive_side, prec->m_data.depart_side);
						}
						else
						{
							prec->LockSide();
							bSend=true;
							sysprint(4,0,"[批量股道锁定] 调度员设置列车:%s 在车站:%s 股道锁定, 股道:%d %d", 
								train.GetTrainID(), gpcdef.GetStationName(prec->m_data.station),prec->m_data.arrive_side, prec->m_data.depart_side);
						}
					}
				}
				if(bSend)
				{
					pData->UpdateTrain(schdinfo,train,SYCFLAG_NULL); 
				}
			}
		}
		OnRecievedDataEnd(0,0);
	}
	else if(nOperFlag==0x00000002) // 批量解锁股道
	{
		OnRecievedDataEnd(0,1);
		for(int i=0; i<list_index.size(); i++)
		{
			CTG_SingleTrain *pTrain1=pData->GetSingleTrain(list_index[i]);
			if(NULL!=pTrain1)
			{
				bool bSend=false;
				CTG_SingleTrain train=*pTrain1;
				for(int i=0; i<train.GetTrainRecordCount(); i++)
				{
					CTG_TrainRecord *prec = train.GetTrainRecordNoDelete(i);
					if(NULL != prec)
					{
						if(!prec->IsLockSide())
							continue;
						if(prec->IsActualArriveTimeRecord() || prec->IsActualDepartTimeRecord())
						{
							sysprint(4,0,"[批量股道解锁] 调度员设置列车:%s 在车站:%s 解锁, 存在实际点, 股道不解锁, 股道:%d %d", 
								train.GetTrainID(), gpcdef.GetStationName(prec->m_data.station),prec->m_data.arrive_side, prec->m_data.depart_side);
						}
						else
						{
							prec->UnLockSide();
							bSend=true;
							sysprint(4,0,"[批量股道解锁] 调度员设置列车:%s 在车站:%s 股道解锁, 股道:%d %d", 
								train.GetTrainID(), gpcdef.GetStationName(prec->m_data.station),prec->m_data.arrive_side, prec->m_data.depart_side);
						}
					}
				}
				if(bSend)
				{
					pData->UpdateTrain(schdinfo,train,SYCFLAG_NULL); 
				}
			}
		}
		OnRecievedDataEnd(0,0);
	}
	else if(nOperFlag==0x00000003) // 批量列车偏移
	{
		int moveMin=0,begin_sta_index=-1, end_sta_idx=-1, begin_tm=0, end_tm=0;
		pNode = pRootNode->selectSingleNode("movetm");
		if(pNode)
		{
			v = pNode->GetnodeTypedValue();
			moveMin = atoi(CString(v.bstrVal));
			SysFreeString(v.bstrVal);
			v.Clear();

			pChildNode=pNode->selectSingleNode("@begin_sta_index");
			if(pChildNode)
			{
				v = pChildNode->GetnodeTypedValue();
				begin_sta_index = atoi(CString(v.bstrVal));
				SysFreeString(v.bstrVal);
				v.Clear();
			}
			pChildNode=pNode->selectSingleNode("@end_sta_idx");
			if(pChildNode)
			{
				v = pChildNode->GetnodeTypedValue();
				end_sta_idx = atoi(CString(v.bstrVal));
				SysFreeString(v.bstrVal);
				v.Clear();
			}
			pChildNode=pNode->selectSingleNode("@begin_tm");
			if(pChildNode)
			{
				v = pChildNode->GetnodeTypedValue();
				begin_tm = atoi(CString(v.bstrVal));
				SysFreeString(v.bstrVal);
				v.Clear();
			}
			pChildNode=pNode->selectSingleNode("@end_tm");
			if(pChildNode)
			{
				v = pChildNode->GetnodeTypedValue();
				end_tm = atoi(CString(v.bstrVal));
				SysFreeString(v.bstrVal);
				v.Clear();
			}
		}
		if(moveMin==0 || begin_sta_index==-1 || end_sta_idx==-1 || begin_tm==0 || end_tm==0)
			return;

		OnRecievedDataEnd(0,1);
		for(int i=0; i<list_index.size(); i++)
		{
			CTG_SingleTrain *pTrain1=pData->GetSingleTrain(list_index[i]);
			if(NULL!=pTrain1)
			{
				CTG_SingleTrain train=*pTrain1;
				train.ModifyTrainRunTimeByMinute(begin_sta_index, end_sta_idx, begin_tm, end_tm,moveMin);
				CaculateTrainSlowTime(&train);
				pData->UpdateTrain(schdinfo,train,SYCFLAG_NULL); 
			}
		}
		OnRecievedDataEnd(0,0);
	}
	else if(nOperFlag==0x00000004) // 批量缓行列车
	{
		int percent=0,begin_sta_index=-1, end_sta_idx=-1, begin_tm=0, end_tm=0;
		pNode = pRootNode->selectSingleNode("percent");
		if(pNode)
		{
			v = pNode->GetnodeTypedValue();
			percent = atoi(CString(v.bstrVal));
			SysFreeString(v.bstrVal);
			v.Clear();

			pChildNode=pNode->selectSingleNode("@begin_sta_index");
			if(pChildNode)
			{
				v = pChildNode->GetnodeTypedValue();
				begin_sta_index = atoi(CString(v.bstrVal));
				SysFreeString(v.bstrVal);
				v.Clear();
			}
			pChildNode=pNode->selectSingleNode("@end_sta_idx");
			if(pChildNode)
			{
				v = pChildNode->GetnodeTypedValue();
				end_sta_idx = atoi(CString(v.bstrVal));
				SysFreeString(v.bstrVal);
				v.Clear();
			}
			pChildNode=pNode->selectSingleNode("@begin_tm");
			if(pChildNode)
			{
				v = pChildNode->GetnodeTypedValue();
				begin_tm = atoi(CString(v.bstrVal));
				SysFreeString(v.bstrVal);
				v.Clear();
			}
			pChildNode=pNode->selectSingleNode("@end_tm");
			if(pChildNode)
			{
				v = pChildNode->GetnodeTypedValue();
				end_tm = atoi(CString(v.bstrVal));
				SysFreeString(v.bstrVal);
				v.Clear();
			}
		}
		if(percent==0 || begin_sta_index==-1 || end_sta_idx==-1 || begin_tm==0 || end_tm==0)
			return;

		OnRecievedDataEnd(0,1);
		for(int i=0; i<list_index.size(); i++)
		{
			CTG_SingleTrain *pTrain1=pData->GetSingleTrain(list_index[i]);
			if(NULL!=pTrain1)
			{
				CTG_SingleTrain train=*pTrain1;
				train.ModifyTrainRunTimeByPercent(begin_sta_index, end_sta_idx, begin_tm, end_tm,percent);
				CaculateTrainSlowTime(&train);
				pData->UpdateTrain(schdinfo,train,SYCFLAG_NULL); 
			}
		}
		OnRecievedDataEnd(0,0);
	}
	else if(nOperFlag==0x00000005) // 批量平移列车
	{
		int offset=0;
		pNode = pRootNode->selectSingleNode("offset");
		if(pNode)
		{
			v = pNode->GetnodeTypedValue();
			offset = atoi(CString(v.bstrVal));
			SysFreeString(v.bstrVal);
			v.Clear();
		}
		if(offset==0)
			return;
		OnRecievedDataEnd(0,1);
		for(int i=0; i<list_index.size(); i++)
		{
			CTG_SingleTrain *pTrain1=pData->GetSingleTrain(list_index[i]);
			if(NULL!=pTrain1)
			{
				CTG_SingleTrain train=*pTrain1;
				train.OffsetScheduleTime(0,offset,FALSE,1);
				CaculateTrainSlowTime(&train);
				pData->UpdateTrain(schdinfo,train,SYCFLAG_NULL); 
			}
		}
		OnRecievedDataEnd(0,0);
	}
}

// 保存shift id,滤去重复
void CTgEditView::AddTdmsTrainGuid(DWORD guid)
{
	for(int n=0; n<recieved_tdms_trains.size(); n++)
	{
		if(recieved_tdms_trains[n]==guid)
		{
			CString waitstr;
			if(m_schdinfo.m_nType==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_PREV)
				waitstr.Format("接收tdms基本图时发现 GTID %u 重复", guid);
			else
				waitstr.Format("接收tdms日班计划时发现 GTID %u 重复", guid);
			GpcLogTrainMessage(0,0, waitstr);
			break;
		}
	}
	if(n>=recieved_tdms_trains.size())
	{
		recieved_tdms_trains.push_back(guid);
	}
}

void CTgEditView::OnDisplaySideReqChangeDlg(CLBTH::MsgChangeSideReq *pChangeSideReq, int station)
{
	CWorkScheduleHolder *pHolder=GetDocument()->GetTGDataHolder();
	if(NULL==pHolder)
		return;
	CTG_TrainScheduleBase *pData=pHolder->GetWorkScheduleData();
	if(NULL==pData)
		return;
	CTG_ScheduleInfo schd_info;
	pData->GetScheduleInfo(schd_info);
	CDlgStationReqChangeSide dlg(station);
	for(int i=0; i<pChangeSideReq->count&&i<256; i++)
	{
		NodeSideInfo info;
		info.train_index = pChangeSideReq->train_index[i];
		info.rec_index = pChangeSideReq->rec_index[i];
		info.a_side = pChangeSideReq->arrive_side[i];
		info.d_side = pChangeSideReq->depart_side[i];
		GpcLogTrainMessage(0,0, "第%d条: train_index=%u, rec_index=%d, arrive_side=%d, depart_side=%d",
			i+1, info.train_index, info.rec_index, info.a_side, info.d_side);

		CTG_SingleTrain *ptrain=pData->GetSingleTrain(info.train_index);
		if(ptrain==NULL)
		{
			GpcLogTrainMessage(0,0, "根据train_index=%u 找不到运行线, 无法修改", info.train_index);
			continue;
		}
		CTG_TrainRecord *prec=ptrain->GetTrainRecordByRecIndex(info.rec_index);
		if(prec==NULL)
		{
			GpcLogTrainMessage(0,0, "根据rec_index=%d 找不到运行线车站, 无法修改", info.rec_index);
			continue;
		}
		if(prec->GetStationNo()!=station)
		{
			GpcLogTrainMessage(0,0, "根据rec_index=%d 找到车站和实际请求车站不同, 无法修改", info.rec_index);
			continue;
		}

		info.trainid=ptrain->GetTrainID();
		const char* pstr=GetTGBasicParam()->GetSideNameByID(station, info.a_side);
		if(pstr==NULL)
			info.s_aside="?G";
		else
			info.s_aside=pstr;

		pstr=GetTGBasicParam()->GetSideNameByID(station, info.d_side);
		if(pstr==NULL)
			info.s_dside="?G";
		else
			info.s_dside=pstr;

		GpcLogTrainMessage(0,0, "成功解析出 train_name=%s %s-%s", 
			info.trainid, info.s_aside, info.s_dside);
		dlg.AddTrainSideRecord(info);
	}

	if(dlg.DoModal()==IDOK)
	{
		GpcLogTrainMessage(0,0, "开始修改股道");
		for(int i=0; i<dlg.m_count; i++)
		{
			TRAIN_INDEX train_index=dlg.siderecord[i].train_index;
			int rec_index = dlg.siderecord[i].rec_index;

			CTG_SingleTrain *ptrain=pData->GetSingleTrain(train_index);
			if(ptrain==NULL)
			{
				GpcLogTrainMessage(0,0, "根据 train_index=%u 找不到运行线, 无法修改", train_index);
				continue;
			}
			CTG_SingleTrain train=*ptrain;
			CTG_TrainRecord *prec=train.GetTrainRecordByRecIndex(rec_index);
			if(prec==NULL)
			{
				GpcLogTrainMessage(0,0, "根据 rec_index=%d 找不到运行线车站, 无法修改", rec_index);
				continue;
			}
			if(prec->GetStationNo()!=station)
			{
				GpcLogTrainMessage(0,0, "根据 rec_index=%d 找到车站 %d 和实际请求车站 %d 不同, 无法修改", 
					rec_index,prec->GetStationNo()!=station);
				continue;
			}

			prec->m_data.arrive_side=dlg.siderecord[i].a_side;
			prec->m_data.depart_side=dlg.siderecord[i].d_side;
			GpcLogTrainMessage(0,0, "修改车次 %s 在车站 %d 股道 %d-%d", 
				train.GetTrainID(), prec->GetStationNo(), prec->m_data.arrive_side, prec->m_data.depart_side);

			pData->UpdateTrain(schd_info, train, SYCFLAG_NULL);
		}
		GpcLogTrainMessage(0,0, "结束修改股道");
	}
}

// 批量修改股道
void CTgEditView::BatchManualModifySide(TRAIN_INDEX train_index)
{
	// TODO: 在此添加命令处理程序代码
	CWorkScheduleHolder *pHolder=GetDocument()->GetTGDataHolder();
	if(NULL==pHolder)
		return;

	CTG_TrainScheduleBase *pData = NULL;
	if(CTG_ScheduleInfo::SCHEDULE_TYPE_WORK==GetViewScheduleType())
		pData=pHolder->GetWorkScheduleData();
	else if(CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_PREV==GetViewScheduleType())
		pData=pHolder->GetBasicPrevScheduleData();
	else
		pData = NULL;

	if(NULL==pData)
		return;
	CTG_ScheduleInfo schd_info;
	pData->GetScheduleInfo(schd_info);

	CTG_SingleTrain *pTrain=pData->GetSingleTrain(train_index);
	if(!pTrain)
		return;
	
	CTG_SingleTrain train=*pTrain;

	GpcLogTrainMessage(0,0, "调度员开始批量修改车次%s(%u)的股道", train.GetTrainID(), train_index);
	GpcLogTrainMessage(0,0, "修改前%s经过的车站股道如下:", train.GetTrainID());

	for(int n=0; n<train.GetTrainRecordCount(); n++)
	{
		const CTG_TrainRecord *prec=train.GetTrainRecordByRecIndex_Const(n);
		if(prec==NULL)
			return;

		GpcLogTrainMessage(0,0, "车站 %d 股道 %d-%d", prec->GetStationNo(), prec->m_data.arrive_side, prec->m_data.depart_side);
	}

	// 根据计划下发情况决定哪些车站为本台管辖
	std::vector<int> station_list;
	GetDocument()->GetSTPLAN()->GetSendPlanStation(station_list);
	
	CString strCaption;
	strCaption.Format("批量修改股道——%s", train.GetTrainID());
	DlgBatchSideModify dlg(train, strCaption, pHolder->GetBasicScheduleData(), pData, this);
	dlg.SetThisStation(station_list); // 设置本台管辖车站
	if(dlg.DoModal()==IDOK)
	{
		GpcLogTrainMessage(0,0, "修改后%s经过的车站股道如下:", dlg.m_train.GetTrainID());
		for(int n=0; n<dlg.m_train.GetTrainRecordCount(); n++)
		{
			const CTG_TrainRecord *prec=dlg.m_train.GetTrainRecordByRecIndex_Const(n);
			if(prec==NULL)
				return;
			GpcLogTrainMessage(0,0, "车站 %d 股道 %d-%d", prec->GetStationNo(), prec->m_data.arrive_side, prec->m_data.depart_side);
		}

		pData->UpdateTrain(schd_info, dlg.m_train, SYCFLAG_NULL);
	}
	else
	{
		GpcLogTrainMessage(0,0, "调度员取消车次 %s 的股道批量修改", train.GetTrainID());
	}
}

// 批量分配未知股道
void CTgEditView::OnBatchAllocationSide()
{
	// TODO: 在此添加命令处理程序代码
	CTG_LogicObject *pobj=GetDocument()->GetCurrentActiveObject();
	if(!pobj)
	{
		MessageBox("请选中运行线操作");
		return;
	}
	if(pobj->GetObjectType()!=TG_OBJ_TRAIN)
	{
		MessageBox("请选中运行线操作");
		return;
	}

	CWorkScheduleHolder *pHolder=GetDocument()->GetTGDataHolder();
	if(NULL==pHolder)
		return;

	CTG_TrainScheduleBase *pData = NULL;
	if(CTG_ScheduleInfo::SCHEDULE_TYPE_WORK==GetViewScheduleType())
		pData=pHolder->GetWorkScheduleData();
	else if(CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_PREV==GetViewScheduleType())
		pData=pHolder->GetBasicPrevScheduleData();
	else
		pData = NULL;

	if(NULL==pData)
		return;
	CTG_ScheduleInfo schd_info;
	pData->GetScheduleInfo(schd_info);

	CTG_SingleTrain train=*((CTG_SingleTrain*)pobj);

	GpcLogTrainMessage(0,0, "调度员开始批量生成车次%s(%u)的股道", train.GetTrainID(), train.GetTrainIndex());
	GpcLogTrainMessage(0,0, "生成前%s经过的车站股道如下:", train.GetTrainID());

	for(int n=0; n<train.GetTrainRecordCount(); n++)
	{
		const CTG_TrainRecord *prec=train.GetTrainRecordByRecIndex_Const(n);
		if(prec==NULL)
			return;

		GpcLogTrainMessage(0,0, "车站 %d 股道 %d-%d", prec->GetStationNo(), prec->m_data.arrive_side, prec->m_data.depart_side);
	}

	CTGAutoAdjustLogic adjust_logic(GetTGFunctionConfig(),GetTGBasicParam(),pHolder->GetBasicScheduleData(), pData);
	bool bAlloctionSide=false;
	for(int n=0; n<train.GetTrainRecordCount(); n++)
	{
		const CTG_TrainRecord *prec=train.GetTrainRecordByRecIndex_Const(n);
		if(prec==NULL)
			return;

		int station=prec->GetStationNo();
		int sideno=prec->m_data.arrive_side;
		int sideno2=prec->m_data.depart_side;
		
        const char* p=GetTGBasicParam()->GetSideNameByID(station, sideno);
		const char* p2=GetTGBasicParam()->GetSideNameByID(station, sideno2);
		if(n==0)
		{
			if(prec->IsStartRec())
			{	
				if(NULL == p2)
				{
					bAlloctionSide=true;
					adjust_logic.ChangeSideForStopOrPass(&train, n);
				}
			}
			else
			{
				if(NULL==p || NULL==p2)
				{
					bAlloctionSide=true;
					adjust_logic.ChangeSideForStopOrPass(&train, n);
				}
			}
		}
		if(n+1==train.GetTrainRecordCount())
		{
			if(prec->IsTerminalRec())
			{
				if(NULL == p)
				{
					bAlloctionSide=true;
					adjust_logic.ChangeSideForStopOrPass(&train, n);
				}
			}
			else
			{
				if(NULL==p || NULL==p2)
				{
					bAlloctionSide=true;
					adjust_logic.ChangeSideForStopOrPass(&train, n);
				}
			}
		}

		if(n>0 && n<train.GetTrainRecordCount())
		{
			if(NULL==p || NULL==p2)
			{
				bAlloctionSide=true;
				adjust_logic.ChangeSideForStopOrPass(&train, n);
			}
		}
	}
	
	// 如果自动分配了,显示给用户
	if(bAlloctionSide)
	{
		CString strCaption;
		strCaption.Format("批量未知股道分配股道——%s", train.GetTrainID());
		DlgBatchSideModify dlg(train, strCaption, pHolder->GetBasicScheduleData(), pData, this);
		if(dlg.DoModal()==IDOK)
		{
			GpcLogTrainMessage(0,0, "最终调度员确认%s经过的车站股道如下:", dlg.m_train.GetTrainID());
			for(int n=0; n<dlg.m_train.GetTrainRecordCount(); n++)
			{
				const CTG_TrainRecord *prec=dlg.m_train.GetTrainRecordByRecIndex_Const(n);
				if(prec==NULL)
					return;
				GpcLogTrainMessage(0,0, "车站 %d 股道 %d-%d", prec->GetStationNo(), prec->m_data.arrive_side, prec->m_data.depart_side);
			}

			pData->UpdateTrain(schd_info, dlg.m_train, SYCFLAG_NULL);
		}
		else
		{
			GpcLogTrainMessage(0,0, "调度员取消车次 %s 的批量股道生成", train.GetTrainID());
		}
	}
	else
	{
		GpcLogTrainMessage(0,0, "%s(%u)经过车站没有未分配的股道,不需要批量分配股道.", train.GetTrainID(), train.GetTrainIndex());
		CString str;
		str.Format("%s经过车站没有未分配的股道,不需要批量分配股道",train.GetTrainID());
		MessageBox(str, "错误");
	}
}

void CTgEditView::OnUpdateBatchAllocationSide(CCmdUI* pCmdUI)
{
	if(CTG_ScheduleInfo::SCHEDULE_TYPE_WORK==GetViewScheduleType())
	{
		if(!CheckUserTGRight()) 
		{
			pCmdUI->Enable(FALSE);
		}
		else
		{
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
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}

bool CTgEditView::IsAddTrain(CString trainId)
{
	CString strText, strTitle;
	CWorkScheduleHolder *pHolder=GetDocument()->GetTGDataHolder();
	if(pHolder)
	{
		strText = pHolder->NewTrainNoIsExist(trainId, strTitle);
	}

	if(strText != "")
	{
		strText = "车次"+strText+"已经存在，是否重复加车?";
		if(MessageBox(strText,strTitle,MB_YESNO)!=IDYES)
			return false; // 不加车
	}

	return true; // 加车
}