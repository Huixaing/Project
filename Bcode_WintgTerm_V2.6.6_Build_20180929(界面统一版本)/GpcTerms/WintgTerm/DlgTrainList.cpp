// DlgTrainList.cpp : 实现文件
//

#include "stdafx.h"
#include "memshare.h"
#include "resource.h"
#include "DlgTrainList.h"
#include "tg_schedule_base.h"
#include "tg_work_schdule_holder.h"
#include <baseMsg\msgid.h>
#include <baseMsg\gpc_msgid.h>
#include <BaseMsg\clbth.h>
#include "icommtask.h"
#include "td/td_frame.h"
#include "td/td_data_def.h"
#include "tgdocument.h"
#include "tgpspdata.h"
#include "utility/xml_utility.h"
#include "dlgwait.h"
#include "schd_tgmmi.h"
#include "DlgConfirm.h"
#include ".\dlgtrainlist.h"
#define IDC_SBTX_TRAINLIST  5211
#define IDC_SBTX_MARKLIST   5212

#import  <msxml.dll> named_guids
using namespace MSXML;
// CDlgTrainList 对话框

IMPLEMENT_DYNAMIC(CDlgTrainList, CDialog)
CDlgTrainList::CDlgTrainList(const char *caption,CTgDocument *pdoc,int sche_type,int type,CWnd* pParent /*=NULL*/)
: CDialog(CDlgTrainList::IDD, pParent)
{
	m_pDoc=pdoc;
	m_pHolder=m_pDoc->GetTGDataHolder();
	m_nScheduleType= sche_type;
	m_nCurrentSel=-1;
	m_nCurrentSelPlan=-1;
	m_pData=NULL;
	m_szCaption=caption;

	if(m_nScheduleType==CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT)
		m_pSourceData=m_pHolder->GetShiftScheduleData();
	else
		if(m_nScheduleType==CTG_ScheduleInfo::SCHEDULE_TYPE_WORK)
			m_pSourceData=m_pHolder->GetWorkScheduleData();
		else
			m_pSourceData=NULL;

	if(m_pSourceData)
		m_pSourceData->GetScheduleInfo(m_schd_info);

	m_nGridType=type;
	m_nDataChanged=0;
	m_pWaitWnd=NULL;
	m_pTgMMI=NULL;
	m_bDirectlyRequestMode=FALSE;
}

CDlgTrainList::~CDlgTrainList()
{
	if(m_pData)
		delete m_pData;
}

void CDlgTrainList::AddRequestTrainGUID(DWORD tid,const char *tno,BOOL local_add)
{
	if(tid == NULL_TRAININDEX)// tid必须是有效的GTID 20101105 qiaoyt
		return;
	tid &=0x3FFFFFFF;

	int idx;
	for(idx=0;idx<m_plan_inform.COUNT;idx++)
	{
		if(m_plan_inform.trains[idx].GTID==tid)
			break;
	}
	if(idx<m_plan_inform.COUNT)
		return;
	
	if(idx<CTD_PlanInform::MAX_NFORM_TRAIN_NUM)
	{
		m_plan_inform.trains[idx].GTID=tid;
		m_plan_inform.trains[idx].TNO=tno;
		m_plan_inform.trains[idx].OPER=CTD_PlanInform::OPER_UPDATE_TRAIN; 
		m_plan_inform.trains[idx].InfoReceivedFlag=0;
		m_plan_inform.trains[idx].KDID=0;
		m_plan_inform.trains[idx].LocalAdded=local_add; // 本地添加的

		m_plan_inform.COUNT++;
	}

	return;
}

BOOL CDlgTrainList::SetPlanInformXML(const char *token,const char *xml)
{
 	if(!m_plan_inform.LoadFromXML(xml,token))
	{
		CString cap;
		cap.Format("无法识别计划通知消息:%s",token);
		GpcLogTrainMessage(0, 0, "调整计划: %s", cap);
		AfxMessageBox(cap);
		return FALSE;
	}

	if (stricmp(token, TOKEN_22_ACK_STN_PLAN) && stricmp(token, TOKEN_24_ACK_FTN_PLAN)
		&& stricmp(token, TOKEN_25_AD_PLAN_INFORM))
	{
		//////////////// 根据时间范围判断既有的运行线 ////////////
		if(m_plan_inform.STTM>0 &&  m_plan_inform.ENDTM>m_plan_inform.STTM)
		{
			CTG_TrainPtrGroup group;
			m_pSourceData->FindTrains(group,NULL_STATION, m_plan_inform.STTM,m_plan_inform.ENDTM,FALSE);
			for(int i=0;i<group.GetTrainCount();i++)
			{
				CTG_SingleTrain *ptrain=group.GetTrainPtr(i);
				if(!ptrain)
					continue;
				if(ptrain->IsPassengerTrain())
					continue;
				if(ptrain->GetShiftTrainIndex()==NULL_TRAININDEX)  // 调度员自己画的车次
					continue;
				AddRequestTrainGUID(ptrain->GetShiftTrainIndex(),ptrain->GetTrainID(),TRUE);
			}
		}
	}

	return TRUE;
}

void CDlgTrainList::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgTrainList, CDialog)
	ON_NOTIFY(GVN_SELCHANGING,IDC_SBTX_TRAINLIST,OnGridSelectChange)
	ON_NOTIFY(GVN_SELCHANGING,IDC_SBTX_MARKLIST,OnMarkGridSelectChange)
	ON_NOTIFY(GVN_SELCHANGING,IDC_GRID_PLAN_LIST,OnPlanGridSelectChange)
	ON_BN_CLICKED(ID_REQUEST_DATA, OnBnClickedReq)
	ON_MESSAGE(UM_UNI_MESSAGE, OnCommMsg)
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
END_MESSAGE_MAP()

LRESULT CDlgTrainList::OnCommMsg(WPARAM wParam, LPARAM lParam) 
{
	UNI_MESSAGE_STRUCT *pMsgStruct=( UNI_MESSAGE_STRUCT *)lParam;
	if(pMsgStruct->msg_id == MSGID_MsgStringMessage)
	{
		CLBTH::MsgStringMessage *pMsg= (CLBTH::MsgStringMessage  *)pMsgStruct->pmsg;
		const char *token=pMsg->GetToken();
		const char *content=pMsg->GetContent();
		if(token && content)
		{
			if(GetTGFunctionConfig()->IsEnableTDMS5_0()) //TD5.0
			{
				if(!stricmp(token,TOKEN_8_ACK_NEWYL))
				{
					ProcessTrainMsgFromOPMS(token,content);
				}
				else if(!stricmp(token,TOKEN_9_ACK_PLAN)
						|| !stricmp(token,TOKEN_10_ACK_ADPLAN))
				{
					ProcessReplyMsgFromOPMS(token,content);
				}
				else if(!stricmp(token,TOKEN_15_ACK_KEDIAO_INFO))
				{
					CTG_TrainKediaoInfo kd;
					if(TD_GetKediaoInfoFromXML(kd,content,token))
					{
						if(kd.msgid==3)
						{
							CTG_SingleTrain *ptrain=m_pData->GetSingleTrainByShiftIndex(kd.GetShiftIndex(), NULL);
							if(ptrain!=NULL)
							{
								ptrain->GetTrainInfoExpand()->UpdateTrainKediao(kd);
							}
						}
					}
				}
				else if(!stricmp(token,TOKEN_16_ACK_YINGYE_INFO))
				{
					CTG_TrainYingyeList yy;
					if(TD_GetYingyeInfoFromXML(yy,content,token))
					{
						if(yy.msgid==3)
						{
							CTG_SingleTrain *ptrain=m_pData->GetSingleTrainByShiftIndex(yy.GetShiftIndex(), NULL);
							if(ptrain!=NULL)
							{
								ptrain->GetTrainInfoExpand()->UpdateTrainYingye(yy);
							}
						}
					}
				}
				else if(!stricmp(token,TOKEN_20_ACK_CREW_INFO))
				{
					CTG_TrainCrewList crew;
					if(TD_GetCrewInfoFromXML(crew,content,token))
					{
						if(crew.msgid==3)
						{
							CTG_SingleTrain *ptrain=m_pData->GetSingleTrainByShiftIndex(crew.GetShiftIndex(), NULL);
							if(ptrain!=NULL)
							{
								ptrain->GetTrainInfoExpand()->UpdateTrainCrew(crew);
							}
						}
					}
				}
			}
			else
			{
				if(!stricmp(token,TOKEN_3_ACK_YL_BY_TIME)
					|| !stricmp(token,TOKEN_8_ACK_NEWYL))
				{
					ProcessTrainMsgFromOPMS(token,content);
				}
				else if(!stricmp(token,TOKEN_9_ACK_PLAN)
						|| !stricmp(token,TOKEN_10_ACK_ADPLAN))
				{
					ProcessReplyMsgFromOPMS(token,content);
				}
				else
				{
					if(!stricmp(token,TOKEN_15_ACK_KEDIAO_INFO) ||  !stricmp(token,TOKEN_16_ACK_YINGYE_INFO))
					{
						ProcessShiftInfoMsgFromOPMS(token,content);
					}
				}
			}
		}
	}
	FREE_UNI_MESSAGE_STRUCT(pMsgStruct);
	return 0;
}

PropertyInfoPara ShiftPlanInfoDef[]=
{
	{0,0,CTC_DATA_STR,"RecvTimeDisp","接收时间"},
	{0,0,CTC_DATA_STR,"SignTimeDisp","签收时间"},
	{0,0,CTC_DATA_STR,"Source","来源"},
};

BOOL CDlgTrainList::OnInitDialog()
{
	CDialog::OnInitDialog();
	tdms_ctc_alarm.RemoveAll();
	SetWindowText(m_szCaption);
	CRect rect;
	GetDlgItem(IDC_GRID_FRAME)->GetWindowRect(&rect);
	ScreenToClient(&rect);
	m_grid.m_bRowSelectable=TRUE;
	m_grid.CreateGrid(this,rect,IDC_SBTX_TRAINLIST);
	m_grid.SetEditable(FALSE);
	m_markgrid.m_bRowSelectable=TRUE;
	m_markgrid.CreateGrid(this,rect,IDC_SBTX_MARKLIST);
	m_markgrid.SetEditable(FALSE);

	GetDlgItem(IDC_GRID_PLAN_LIST)->GetWindowRect(&rect);
	ScreenToClient(&rect);
	m_grid_plan.m_bRowSelectable=FALSE;
	m_grid_plan.CreateGrid(this,rect,IDC_GRID_PLAN_LIST,ShiftPlanInfoDef,sizeof(ShiftPlanInfoDef)/sizeof(ShiftPlanInfoDef[0]));
	m_grid_plan.ExpandColumnsToFit();
	m_grid.SetEditable(FALSE);


	if(m_nGridType==0)
	{
		GetDlgItem(IDC_LIST_TOP_CAPTION)->SetWindowText("计划车次列表");
		GetDlgItem(IDC_INFO_TOP_CAPTION)->SetWindowText("客调命令/营业/军特调等信息");
		m_markgrid.ShowWindow(FALSE);
	}
	else
	{
		GetDlgItem(IDC_LIST_TOP_CAPTION)->SetWindowText("计划施工列表");
		GetDlgItem(IDC_INFO_TOP_CAPTION)->SetWindowText("施工相关信息");
		m_grid.ShowWindow(FALSE);
	}
	if(!m_pData)
	{
		m_pData=new CTG_TrainScheduleBase();
		m_pData->SetScheduleInfo(m_schd_info);
	}
	const char *shiftname=m_schd_info.GetShiftName();
	((CComboBox *)GetDlgItem(IDC_COMBO_SHIFT))->AddString(shiftname);
	((CComboBox *)GetDlgItem(IDC_COMBO_SHIFT))->AddString("下一班");
	((CComboBox *)GetDlgItem(IDC_COMBO_SHIFT))->SetCurSel(0);
	
	RegisterMsgProcHandleWnd(GetSafeHwnd(),MSGID_MsgStringMessage,0);
	if(!m_bDirectlyRequestMode)
	{
		RefreshPlanList();
		RequestPlanDetail();
	}
	else
	{
		RefreshTrainList_Orig();
		RequestNewYLFromOPMS();
	}
	m_bSelAll=(TRUE==m_grid.IsRowSelected(0));
	return TRUE;
}

void CDlgTrainList::OnOK()
{
	if(tdms_ctc_alarm.GetCount()>0)
	{
		CString text="请注意,接收调整计划存在如下问题,请确认:";
		CDlgConfirm dlg(text, tdms_ctc_alarm, this);
		dlg.DoModal();
	}

	int nSelectCount=0;
	for(int i=1;i<m_grid.GetRowCount();i++)
	{
		if(m_grid.IsRowSelected(i))
			nSelectCount++;
	}

	if(0 == nSelectCount)
	{
		if(MessageBox("没有选择接收列车, 是否继续?", "警告", MB_YESNO|MB_ICONWARNING) == IDNO)
			return;
	}
	if(!m_pData)
		return;
	
	CGridCellBase* pCell=m_grid_plan.GetCell(m_nCurrentSelPlan,1);
	if(pCell==NULL)
		return;

	bool bUpdateTrainRelation=false;
	int nAccpetMode=GetTGFunctionConfig()->GetAdjustAcceptRelationMode();
	if(nAccpetMode==1)
	{
		bUpdateTrainRelation=true;
		GpcLogTrainMessage(0, 0, "调整计划:更新交路");
	}
	else
	{
		bUpdateTrainRelation=false;
		GpcLogTrainMessage(0, 0, "调整计划:不更新交路");
	}

	DWORD id=pCell->GetData();
	CPlanInformListItem *pitem=m_pDoc->GetPlanInformByID(id);
	if(!pitem)
		return;
	if (stricmp(pitem->m_token, TOKEN_22_ACK_STN_PLAN) && stricmp(pitem->m_token, TOKEN_24_ACK_FTN_PLAN)
		&& stricmp(pitem->m_token, TOKEN_25_AD_PLAN_INFORM))
	{
		CString token,dest;
		TD_GenRequestMessage_9_SEND_PLAN(token,dest,m_schd_info.m_nSectionID,m_plan_inform.JHTNM,m_plan_inform.JHID,"");
		if(!token.IsEmpty())
		{
			long  eid=GetSpecialEntityID(SERVER_TYPE_TDCOMM); 
			TGSendStringMessage(token,dest,eid);
		}
	}

	// 当前时间3小时内列车只更新甩挂、停运
	CTime tm = CTime::GetCurrentTime();
	long nBeginTm = tm.GetTime();
	long nEndTm = nBeginTm + 3*3600;
	
	if (GetTGFunctionConfig()->IsEnableRecvAllAdjustPlan())
		nEndTm = nBeginTm + 2*3600;  // 上海局, 两小时

	if(GetTGFunctionConfig()->IsGiveChoiceWhenPathChange())
	{
		for(int i=1;i<m_grid.GetRowCount();i++)
		{
			TRAIN_INDEX trainindex=m_grid.GetCell(i,1)->GetData();
			CTG_SingleTrain *ptrain=m_pData->GetSingleTrain(trainindex);
			if(ptrain)
			{
				if(ptrain->m_nTipToDdy==1) // 1:表示径路发生变化,需要逐个提示调度员
				{
					CString tmp;
					CString oper=m_grid.GetCell(i,2)->GetText();
					tmp.Format("%s: %s, 你确认签收吗？\n签收点击‘是’, 不签收点击‘否’\n", ptrain->GetTrainID(), oper);

					if(MessageBox(tmp, "提示", MB_YESNO)==IDNO)
					{
						m_grid.SetRowSelected(i,FALSE);
					}
					else
					{
						m_grid.SetRowSelected(i,TRUE);
					}
				}
			}
		}
	}

	// 为了回执信息显示.失败数量和失败车次号
	int fail_cnt=0; 
	CString fail_text="";
	CString tmptext;

	for(int i=1;i<m_grid.GetRowCount();i++)
	{
		if(!m_grid.IsRowSelected(i))
		{
			int col=m_grid.GetColumnByID(ID_PD_TRAIN_ID);
			CString tno=m_grid.GetCell(i,col)->GetText();
			tmptext.Format("调度员拒绝接收 车次 %s 的调整!\n", tno);
			if (!stricmp(pitem->m_token, TOKEN_22_ACK_STN_PLAN) )
				m_pDoc->SendGpcAckMsg(TD_GPC_ACK_STN,false, m_plan_inform.JHID, m_plan_inform.MSGID, m_plan_inform.JHTNM,tmptext);
			else if (!stricmp(pitem->m_token, TOKEN_24_ACK_FTN_PLAN))
			{
				fail_cnt++;
				fail_text+=tmptext;
			}
			else if (!stricmp(pitem->m_token, TOKEN_25_AD_PLAN_INFORM))
			{
				fail_cnt++;
				fail_text+=tmptext;
			}
			
			continue;
		}

		// 以下应可以通过 m_pTgMMI调用，以后修改
		if(m_grid.GetCell(i,2)->GetData()==0)
		{
			DWORD  gtid=m_grid.GetCell(i,1)->GetData();
			CTG_TrainPtrGroup group;
			m_pSourceData->FindTrainsByShiftIndex(group,gtid);
			for(int x=0;x<group.GetTrainCount();x++)
			{
				CTG_SingleTrain *ptrainx=group.GetTrainPtr(x);
				if(ptrainx->HasActualRecord()>=0)
				{
					GpcLogTrainMessage(0, 0, "调整计划:拒绝删除 车次 %s ID=%u OPMSID=%u,已经有实际点",ptrainx->GetTrainID(),ptrainx->GetTrainIndex(),ptrainx->GetShiftTrainIndex());
					tmptext.Format("车次 %s 有实际点, 拒绝删除!\n", ptrainx->GetTrainID());
					if (!stricmp(pitem->m_token, TOKEN_22_ACK_STN_PLAN) )
						m_pDoc->SendGpcAckMsg(TD_GPC_ACK_STN,false, m_plan_inform.JHID, m_plan_inform.MSGID, m_plan_inform.JHTNM,tmptext);
					else if (!stricmp(pitem->m_token, TOKEN_24_ACK_FTN_PLAN))
					{
						fail_cnt++;
						fail_text+=tmptext;
					}
					else if (!stricmp(pitem->m_token, TOKEN_25_AD_PLAN_INFORM))
					{
						fail_cnt++;
						fail_text+=tmptext;
					}
					
					continue;
				}
				
				CString text;
				text.Format("调整计划删除列车 %s ,确认?",ptrainx->GetTrainID());
				if (AfxMessageBox(text,MB_YESNO)==IDYES)
				{
					GpcLogTrainMessage(0, 0, "调整计划:删除 车次 %s ID=%u OPMSID=%u",ptrainx->GetTrainID(),ptrainx->GetTrainIndex(),ptrainx->GetShiftTrainIndex());
					m_pTgMMI->DeleteTrain_Directly(ptrainx->GetTrainIndex());
					if (!stricmp(pitem->m_token, TOKEN_22_ACK_STN_PLAN))
						m_pDoc->SendGpcAckMsg(TD_GPC_ACK_STN,true,m_plan_inform.JHID, m_plan_inform.MSGID, m_plan_inform.JHTNM);
				}
			}
		}
		else
		{
			TRAIN_INDEX trainindex=m_grid.GetCell(i,1)->GetData();
			CTG_SingleTrain *ptrain=m_pData->GetSingleTrain(trainindex);
			if(ptrain)
			{	
				CTG_SingleTrain newtrain;
				newtrain=*ptrain;
				
				// train_name 用于临时存放操作,现在恢复
				CString trainId = newtrain.GetTrainID();
				memset(newtrain.GetTrainInfo()->m_data.train_name, 0, MAX_TRAIN_NAME_LEN);
				strncpy(newtrain.GetTrainInfo()->m_data.train_name, trainId.GetBuffer(), MAX_TRAIN_NAME_LEN-1);
				newtrain.GetTrainInfo()->m_data.train_name[MAX_TRAIN_NAME_LEN-1]=0;
				
				CTG_SingleTrain *prev=m_pSourceData->GetSingleTrainByShiftIndex(newtrain.GetTrainIndex(),NULL_STATION);
				if(NULL == prev)
					GpcLogTrainMessage(0, 0, "按照shiftindex %u 未找到修改列车 %s", newtrain.GetTrainIndex(), trainId);
				else
					GpcLogTrainMessage(0, 0, "按照shiftindex %u 找到修改列车 %s", newtrain.GetTrainIndex(), prev->GetTrainID());

				int update_or_add = 0; // 1:update 0:add
				if (prev)
				{
					const CTG_TrainRecordList * old = prev->GetRecordList_Const();
					const CTG_TrainRecordList * newr = newtrain.GetRecordList_Const();

					if (prev->GetTrainRecordCount() == newtrain.GetTrainRecordCount())
					{
						update_or_add = 1;
						for (int j=0; j<prev->GetTrainRecordCount(); j++)
						{
							if (old->m_listRecords[j].GetStationNo() != newr->m_listRecords[j].GetStationNo())
							{
								update_or_add = 0;
								break;
							}
						}
					}
				}
				GpcLogTrainMessage(0, 0, "调整计划:update_or_add=%d", update_or_add); 
				// update
				if (update_or_add)
				{
					GpcLogTrainMessage(0, 0, "调整计划:更新 调整前 车次 %s ID=%u OPMSID=%u %d stalist %s", 
						prev->GetTrainID(), prev->GetTrainIndex(), prev->GetShiftTrainIndex(),
						prev->GetRecordList()->m_listRecords[0].m_data.depart, prev->toString());

					newtrain.SetTrainIndex(prev->GetTrainIndex()); //否则导致重复！20090616

					if(GetTGFunctionConfig()->IsEnableRecvAdjustPlanTime())
					{
						GpcLogTrainMessage(0, 0, "调整计划:配置完全接收调整计划时间");
						bool bInHistory = prev->IfTrainTimeLessTime(nBeginTm);
						if(bInHistory)
						{
							GpcLogTrainMessage(0, 0, "调整计划:老车次 %s ID=%u OPMSID=%u, 在历史图区域,不予更新",
										prev->GetTrainID(), prev->GetTrainIndex(), prev->GetShiftTrainIndex());
						}
						else
						{
							GpcLogTrainMessage(0, 0, "调整计划:老车次 %s ID=%u OPMSID=%u, 在当前时间以后区域,更新",
										prev->GetTrainID(), prev->GetTrainIndex(), prev->GetShiftTrainIndex());

							m_pTgMMI->UpdateTrain_Directly(newtrain, bUpdateTrainRelation);
							GpcLogTrainMessage(0, 0, "调整计划:更新后 车次 %s ID=%u OPMSID=%u %d stalist %s",
								newtrain.GetTrainID(), newtrain.GetTrainIndex(), newtrain.GetShiftTrainIndex(),
								newtrain.GetRecordList()->m_listRecords[0].m_data.depart, newtrain.toString());
							if (!stricmp(pitem->m_token, TOKEN_22_ACK_STN_PLAN))
								m_pDoc->SendGpcAckMsg(TD_GPC_ACK_STN,true,m_plan_inform.JHID, m_plan_inform.MSGID, m_plan_inform.JHTNM);
						}
					}
					else
					{
						GpcLogTrainMessage(0, 0, "调整计划:配置不完全接收调整计划时间");
						if(prev->IfTrainInTimeScope(nBeginTm, nEndTm))
						{
							GpcLogTrainMessage(0, 0, "调整计划:老车次 %s ID=%u OPMSID=%u, 时间%d-%d范围内,更新甩挂和车次",
											prev->GetTrainID(), prev->GetTrainIndex(), prev->GetShiftTrainIndex(), nBeginTm, nEndTm);

							CTG_SingleTrain newtrain2 = *prev;
							CTG_TrainRecordList * new2 = newtrain2.GetRecordList();
							CTG_TrainRecordList * new1 = newtrain.GetRecordList();
							
							for(int i=0; i<newtrain2.GetTrainRecordCount(); i++)
							{
								if (new2->m_listRecords[i].GetStationNo() == new1->m_listRecords[i].GetStationNo())
								{
									// 甩挂
									CString newopmsinfo="";
									const char* opms = new1->m_listRecords[i].GetStationText(STATION_TXT_INFO_TYPE_OPMS);
									if(NULL != opms)
										newopmsinfo = opms;	
									new2->m_listRecords[i].SetStationText(newopmsinfo, STATION_TXT_INFO_TYPE_OPMS);
									if(new2->m_listRecords[i].IsOpmsSetSationText())
									{
										new2->m_listRecords[i].SetStationText(newopmsinfo, STATION_TXT_INFO_TYPE_SHUAIGUA);
										new2->m_listRecords[i].SetOpmsSationTextState("1");
									}
									new2->m_listRecords[i].m_strNoteText=new1->m_listRecords[i].m_strNoteText;
									if(GetTGFunctionConfig()->IsAcceptTrainID())
									{
										GpcLogTrainMessage(0, 0, "调整计划:老车次 %s-%s 新车次 %s-%s",
											new2->m_listRecords[i].m_data.arrive_train_id,
											new2->m_listRecords[i].m_data.depart_train_id,
											new1->m_listRecords[i].m_data.arrive_train_id,
											new1->m_listRecords[i].m_data.depart_train_id);

										strncpy(new2->m_listRecords[i].m_data.arrive_train_id,
											new1->m_listRecords[i].m_data.arrive_train_id, sizeof(TRAIN_NO)-1);
										new2->m_listRecords[i].m_data.arrive_train_id[sizeof(TRAIN_NO)-1]=0;

										strncpy(new2->m_listRecords[i].m_data.depart_train_id,
											new1->m_listRecords[i].m_data.depart_train_id, sizeof(TRAIN_NO)-1);
										new2->m_listRecords[i].m_data.depart_train_id[sizeof(TRAIN_NO)-1]=0;
									}

									// 最后一个点
									if(i == newtrain2.GetTrainRecordCount()-1)
									{
										if(new2->m_listRecords[i].m_data.arrive > nEndTm)
										{
											if(TG_IsFlagGiveout(new1->m_listRecords[i].m_data.flag))
											{
												TG_SetFlagGiveout(new2->m_listRecords[i].m_data.flag);
												TG_SetAutoMakeDepartRoute(new2->m_listRecords[i].m_data);
											}
											else
											{
												TG_SetFlagTerminal(new2->m_listRecords[i].m_data.flag);
												TG_SetNotAutoMakeDepartRoute(new2->m_listRecords[i].m_data);
											}
										}
									}
								}
							}
							m_pTgMMI->UpdateTrain_Directly(newtrain2, bUpdateTrainRelation);
							GpcLogTrainMessage(0, 0, "调整计划:更新后 车次 %s ID=%u OPMSID=%u %d stalist %s",
									newtrain2.GetTrainID(), newtrain2.GetTrainIndex(), newtrain2.GetShiftTrainIndex(),
									newtrain2.GetRecordList()->m_listRecords[0].m_data.depart, newtrain2.toString());
							if (!stricmp(pitem->m_token, TOKEN_22_ACK_STN_PLAN))
								m_pDoc->SendGpcAckMsg(TD_GPC_ACK_STN,true,m_plan_inform.JHID, m_plan_inform.MSGID, m_plan_inform.JHTNM);
						}
						else
						{
							bool bInHistory = prev->IfTrainTimeLessTime(nBeginTm);
							if(bInHistory)
							{
								GpcLogTrainMessage(0, 0, "调整计划:老车次 %s ID=%u OPMSID=%u, 在历史图区域,不予更新",
											prev->GetTrainID(), prev->GetTrainIndex(), prev->GetShiftTrainIndex());
							}
							else
							{
								GpcLogTrainMessage(0, 0, "调整计划:老车次 %s ID=%u OPMSID=%u, 在当前时间%d小时以后区域",
											prev->GetTrainID(), prev->GetTrainIndex(), prev->GetShiftTrainIndex(), (nEndTm-nBeginTm)/3600);

								if(newtrain.HasActualRecord()>=0)
								{
									GpcLogTrainMessage(0, 0, "调整计划:老车次 %s ID=%u OPMSID=%u, 存在实际点,不予更新",
											prev->GetTrainID(), prev->GetTrainIndex(), prev->GetShiftTrainIndex());
								}
								else
								{
									m_pTgMMI->UpdateTrain_Directly(newtrain, bUpdateTrainRelation);

									GpcLogTrainMessage(0, 0, "调整计划:更新后 车次 %s ID=%u OPMSID=%u %d stalist %s",
										newtrain.GetTrainID(), newtrain.GetTrainIndex(), newtrain.GetShiftTrainIndex(),
										newtrain.GetRecordList()->m_listRecords[0].m_data.depart, newtrain.toString());

									if (!stricmp(pitem->m_token, TOKEN_22_ACK_STN_PLAN))
										m_pDoc->SendGpcAckMsg(TD_GPC_ACK_STN,true,m_plan_inform.JHID, m_plan_inform.MSGID, m_plan_inform.JHTNM);
								}
							}
						}
					}
				}
				else
				{
					//太原 20101016 解决由于调整计划中变更径路引起的冲实际线问题
					if(prev)
					{  
						GpcLogTrainMessage(0, 0, "找到老车次");

						if (prev->HasActualRecord()<0)
						{
							GpcLogTrainMessage(0, 0, "调整计划:更新删除 车次 %s ID=%u OPMSID=%u %d staList %s",
								prev->GetTrainID(), prev->GetTrainIndex(), prev->GetShiftTrainIndex(), 
								prev->GetRecordList()->m_listRecords[0].m_data.depart, prev->toString());
							m_pTgMMI->DeleteTrain_Directly(prev->GetTrainIndex());		
							if (!stricmp(pitem->m_token, TOKEN_22_ACK_STN_PLAN))
								m_pDoc->SendGpcAckMsg(TD_GPC_ACK_STN,true,m_plan_inform.JHID, m_plan_inform.MSGID, m_plan_inform.JHTNM);
						}
						
						if((prev->GetShiftTrainIndex() != newtrain.GetShiftTrainIndex()) || (prev->HasActualRecord()<0))
						{
							m_pTgMMI->AddTrain_Directly(newtrain);
							GpcLogTrainMessage(0, 0, "调整计划:增加 车次 %s 原有列车index %u OPMSID=%u %d staList %s",
									newtrain.GetTrainID(),prev->GetTrainIndex(),newtrain.GetShiftTrainIndex(), 
									newtrain.GetRecordList()->m_listRecords[0].m_data.depart, newtrain.toString());
							if (!stricmp(pitem->m_token, TOKEN_22_ACK_STN_PLAN))
								m_pDoc->SendGpcAckMsg(TD_GPC_ACK_STN,true,m_plan_inform.JHID, m_plan_inform.MSGID, m_plan_inform.JHTNM);
						}
						else 
						{	
							// 全局ID相等且有实际点
							if(GetTGFunctionConfig()->IsModifyTrainActAndGTIDSame())
							{
								GpcLogTrainMessage(0, 0, "调整计划:全局ID相等且有实际点,比较两个车次:");
								
								GpcLogTrainMessage(0, 0, "调整计划:老车次 %s(%u) TDMSID=%u staList %s",
										prev->GetTrainID(), prev->GetTrainIndex(), prev->GetShiftTrainIndex(), 
										prev->toString());
								GpcLogTrainMessage(0, 0, "调整计划:新车次 %s(%u) TDMSID=%u staList %s",
										newtrain.GetTrainID(), newtrain.GetTrainIndex(), newtrain.GetShiftTrainIndex(), 
										newtrain.toString());

								const CTG_TrainRecordList * old = prev->GetRecordList_Const();
								const CTG_TrainRecordList * newr = newtrain.GetRecordList_Const();
								
								int nflag=0;
								for (int n=0; n < prev->GetTrainRecordCount(); n++)
								{
									if(old->m_listRecords[n].IsActualArriveTimeRecord())
									{
										if(n<newtrain.GetTrainRecordCount())
										{
											if(!newr->m_listRecords[n].IsActualArriveTimeRecord())
											{
												nflag=1;
												break;
											}

											if(old->m_listRecords[n].GetStationNo() != newr->m_listRecords[n].GetStationNo())
											{
												nflag=2;
												break;
											}
										}
										else
										{
											nflag=1;
											break;
										}
									}
									if(old->m_listRecords[n].IsActualDepartTimeRecord())
									{
										if(n<newtrain.GetTrainRecordCount())
										{
											if(!newr->m_listRecords[n].IsActualDepartTimeRecord())
											{
												nflag=1;
												break;
											}

											if(old->m_listRecords[n].GetStationNo() != newr->m_listRecords[n].GetStationNo())
											{
												nflag=2;
												break;
											}
										}
										else
										{
											nflag=1;
											break;
										}
									}
								}

								if(nflag==0)
								{
									GpcLogTrainMessage(0, 0, "调整计划:删除车次 %s(%u) TDMSID=%u staList %s",
										prev->GetTrainID(), prev->GetTrainIndex(), prev->GetShiftTrainIndex(), 
										prev->toString());
									m_pTgMMI->DeleteTrain_Directly(prev->GetTrainIndex());

									m_pTgMMI->AddTrain_Directly(newtrain);
									GpcLogTrainMessage(0, 0, "调整计划:增加车次 %s(%u) TDMSID=%u stalist %s",
										newtrain.GetTrainID(), newtrain.GetTrainIndex(), newtrain.GetShiftTrainIndex(),
										newtrain.toString());
								
									if (!stricmp(pitem->m_token, TOKEN_22_ACK_STN_PLAN))
										m_pDoc->SendGpcAckMsg(TD_GPC_ACK_STN,true,m_plan_inform.JHID, m_plan_inform.MSGID, m_plan_inform.JHTNM);
								}
								else
								{
									tmptext.Format("车次 %s 有实际点,更新后将丢失实际点 拒绝更新!\n", prev->GetTrainID());

									GpcLogTrainMessage(0, 0, "调整计划:%s", tmptext);
									
									if (!stricmp(pitem->m_token, TOKEN_24_ACK_FTN_PLAN))
									{
										fail_cnt++;
										fail_text+=tmptext;
									}
									else if (!stricmp(pitem->m_token, TOKEN_25_AD_PLAN_INFORM))
									{
										fail_cnt++;
										fail_text+=tmptext;
									}

									if (!stricmp(pitem->m_token, TOKEN_22_ACK_STN_PLAN) )
										m_pDoc->SendGpcAckMsg(TD_GPC_ACK_STN,false, m_plan_inform.JHID, m_plan_inform.MSGID, m_plan_inform.JHTNM,tmptext);
								}
							}
						}
						prev = NULL;
					}
					else
					{
						m_pTgMMI->AddTrain_Directly(newtrain);
						GpcLogTrainMessage(0, 0, "调整计划:增加 车次 %s OPMSID=%u %d staList %s",
							newtrain.GetTrainID(),newtrain.GetShiftTrainIndex(), 
							newtrain.GetRecordList()->m_listRecords[0].m_data.depart, newtrain.toString());
						if (!stricmp(pitem->m_token, TOKEN_22_ACK_STN_PLAN))
							m_pDoc->SendGpcAckMsg(TD_GPC_ACK_STN,true,m_plan_inform.JHID, m_plan_inform.MSGID, m_plan_inform.JHTNM);
					}
					//太原 20101016 end 
				}
			}
		}
	}

	if (!stricmp(pitem->m_token, TOKEN_24_ACK_FTN_PLAN))
	{
		if(fail_cnt>0)
			m_pDoc->SendGpcAckMsg(TD_GPC_ACK_FTN,false,m_plan_inform.JHID, m_plan_inform.MSGID, m_plan_inform.JHTNM, fail_text);
		else
			m_pDoc->SendGpcAckMsg(TD_GPC_ACK_FTN,true,m_plan_inform.JHID, m_plan_inform.MSGID, m_plan_inform.JHTNM);
	}

	if (!stricmp(pitem->m_token, TOKEN_25_AD_PLAN_INFORM))
	{
		if(fail_cnt>0)
			m_pDoc->SendGpcAckMsg(TD_GPC_ACK_PLAN_INFORM,false,m_plan_inform.JHID, m_plan_inform.MSGID, m_plan_inform.JHTNM, fail_text);
		else
			m_pDoc->SendGpcAckMsg(TD_GPC_ACK_PLAN_INFORM,true,m_plan_inform.JHID, m_plan_inform.MSGID, m_plan_inform.JHTNM);
	}

	if(m_nCurrentSelPlan>0 && !m_bDirectlyRequestMode)
	{
		DWORD id=m_grid_plan.GetCell(m_nCurrentSelPlan,1)->GetData();
		CPlanInformListItem *pitem=m_pDoc->GetPlanInformByID(id);
		pitem->m_signtime=time(NULL);
		RefreshPlanList();
		m_pDoc->SavePhasePlanToFile(pitem);
	}
}
// CDlgTrainList 消息处理程序
void CDlgTrainList::OnMarkGridSelectChange(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_GRIDVIEW *pv=(NM_GRIDVIEW *)pNMHDR;
	if(pv->iRow>=1)
	{
		if(m_nCurrentSel!=pv->iRow)
		{
			GetDlgItem(IDC_ED_NOTE)->EnableWindow(TRUE); 
			m_nCurrentSel=pv->iRow;
			ShowMarkInfo();
		}
	}
}
void CDlgTrainList::OnGridSelectChange(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_GRIDVIEW *pv=(NM_GRIDVIEW *)pNMHDR;
	if(pv->iRow==0 && pv->iColumn==0)
	{
		if(m_bSelAll)
		{
			m_bSelAll=false;
			m_grid.SetAllRowSelected(FALSE); 
		}
		else
		{
			m_bSelAll=true;
			m_grid.SetAllRowSelected(TRUE); 
		}

		return;
	}
	if(pv->iRow>=1)
	{
		if(m_nCurrentSel!=pv->iRow)
		{
			GetDlgItem(IDC_ED_NOTE)->EnableWindow(TRUE); 
			m_nCurrentSel=pv->iRow;
			ShowTrainInfo();
		}
	}
}

void CDlgTrainList::GetRecvCountRate(int &recved,int &tatol)
{
	tatol=0;
	recved=0;
	for(int i=0;i<m_plan_inform.COUNT;i++)
	{

		if(m_plan_inform.trains[i].InfoReceivedFlag>0)
			recved++;
		tatol++;
	}
}

void CDlgTrainList::RequestNewYLFromOPMS()
{
	CString token,content;
	DWORD  ids[512];
	int    idcnt=0;
	for(int i=0;i<m_plan_inform.COUNT;i++)
	{
		int j;
		for( j=0;j<idcnt;j++)
		{
			if(ids[j]==m_plan_inform.trains[i].GTID)
				break;
		}
		if(j<idcnt || m_plan_inform.trains[i].GTID==0)
			continue; // already exsit
		ids[idcnt++]=m_plan_inform.trains[i].GTID;
		m_plan_inform.trains[i].InfoReceivedFlag=0;
	}
	if(idcnt>0)
	{
		TD_GenRequestMessage_8_NEWYL(token,content,m_schd_info.m_nSectionID,idcnt,ids);
		long  eid=GetSpecialEntityID(SERVER_TYPE_TDCOMM); 
		TGSendStringMessage(token,content,eid);

		GetDlgItem(IDOK)->EnableWindow(FALSE);
		m_pDoc->OpenWaitWindow("请稍候,正在获取车次详细信息..."); 
	}
}

void CDlgTrainList::OnDestroy()
{
	UnRegisterMsgProcHandleWnd(GetSafeHwnd(),MSGID_MsgStringMessage);
	m_pDoc->OpenWaitWindow(NULL); 
	CDialog::OnDestroy();
}

void CDlgTrainList::OnBnClickedReq()
{
	tdms_ctc_alarm.RemoveAll();
	GpcLogTrainMessage(0, 0, "调整计划:重新申请OPMS信息");
	m_pData->DeleteAllData(m_schd_info,SYCFLAG_NULL);
	RefreshTrainList_Orig();
	RequestNewYLFromOPMS();
}

void CDlgTrainList::ShowMarkInfo()
{
}

void CDlgTrainList::ShowTrainInfo()
{
	if(m_nCurrentSel>=0)
	{
		TRAIN_INDEX train_index=m_grid.GetCell(m_nCurrentSel,1)->GetData(); 
		CTG_SingleTrain *ptrain=m_pData->GetSingleTrain(train_index);
		CString txtstr;
		if(ptrain)
		{
			txtstr=GetObjectAllInfo(ptrain);
		}
		GetDlgItem(IDC_ED_NOTE)->SetWindowText(txtstr);
	}
}


BOOL CDlgTrainList::ProcessReplyMsgFromOPMS(const char *token,const char *content)
{
	CTD_Ack x;
	x.LoadFromXML(content,token);
	if(x.ACKL==0)
	{
		MessageBox("签收成功","提示");
		m_grid.DeleteAllItems();  
		CDialog::OnOK();
	}
	else
		MessageBox("签收处理失败","提示");

	return TRUE;	
}

BOOL CDlgTrainList::ProcessShiftInfoMsgFromOPMS(const char *token,const char *content)
{
	if(!stricmp(token,TOKEN_15_ACK_KEDIAO_INFO) || !stricmp(token,TOKEN_16_ACK_YINGYE_INFO))
	{
		CTG_SingleTrain *ptrain=TD_GetTrainInfoFromXML(m_pData,content,token);
		if(ptrain)
			return TRUE;
	}
	if(!stricmp(token,TOKEN_20_ACK_CREW_INFO))
	{
		TD_SetTrainTicketInfoFromXML(m_pData,content,token);
		return TRUE;
	}
	return FALSE;
}

void CDlgTrainList::CheckAndSetTipToDdyFlag(CTG_SingleTrain& train)
{
	for(int i=0;i<train.GetTrainRecordCount();i++)
	{
		const CTG_TrainRecord *prec=train.GetTrainRecordNoDelete_Const(i);
		if(prec!=NULL)
		{
			if(!prec->m_strNoteText.empty())
			{ 
				train.m_nTipToDdy=2; // 2:存在重点事项
				break;
			}
		}
	}
}

BOOL CDlgTrainList::ProcessTrainMsgFromOPMS(const char *token,const char *content)
{
	if(!m_pData)
		return FALSE;

	CTG_ScheduleInfo schdinfo;
	m_pData->GetScheduleInfo(schdinfo);

	memset(m_cSplitStationList, 0, sizeof(m_cSplitStationList));
	memset(m_cSplitEntryList, 0, sizeof(m_cSplitEntryList));
	m_nSplitNum=GetTGFunctionConfig()->GetNewSplitStation(schdinfo.m_nSectionID, m_cSplitStationList, m_cSplitEntryList, NEW_SPLIT_ST_NUM);
	if(m_nSplitNum>0) // 需要采用新列车分割处理
	{
		return ProcessTrainMsgFromOPMS_NEW(token,content);
	}
	
	// 下面采用老的列车分割处理
	CTG_SingleTrain train;
	
	int flag=0;
	int nMode=GetTGFunctionConfig()->GetAdjustAcceptRelationMode();
	if(!TD_LoadTrainFromXML(schdinfo,m_pSourceData,train,content,token,NULL,NULL,&flag,m_plan_inform.STTM,m_plan_inform.ENDTM,nMode,tdms_ctc_alarm))
		return FALSE;
	
	// 超限,重点警卫,军运处理
	CTgDocument::SetTrainSpaceGaurdMilitaryRank(train);

	train.SetTrainIndex(NULL_TRAININDEX); //m_pSourceData的trainindex 
	if(flag==0)
	{
		for(int i=0;i<m_plan_inform.COUNT;i++)
		{
			if(m_plan_inform.trains[i].GTID==train.GetShiftTrainIndex())
			{
				m_plan_inform.trains[i].OPER=CTD_PlanInform::OPER_NOEXSIT; // 不存在	
				m_plan_inform.trains[i].TNO=train.GetTrainInfo()->m_data.train_name;   
				m_plan_inform.trains[i].InfoReceivedFlag=1;
			}
		}
	}
	else
	{
		CTG_SingleTrain othertrain;
		BOOL  newflag=FALSE;
		if(train.GetTrainIndex() == NULL_TRAININDEX && train.HasActualRecord() < 0) 
			newflag=TRUE;
		
		CTgX* pTgX=m_pTgMMI->GetTGCtrl();
		if(pTgX!=NULL)
		{
			if(GetTGFunctionConfig()->IsCalShiftSchdExitEntry())
			{
				if(!m_pHolder->TGOCXSetTrainExitEntry(train, pTgX))
				{
					CString str;
					str.Format("车次:%s 径路计算失败,请确认列车径路", train.GetTrainID());
					tdms_ctc_alarm.Add(str);
				}
			}
		}
	
		if(!m_pHolder->GenTrain_OPMS(train,othertrain,m_schd_info,newflag))
			return FALSE;
		int i;
		for(i=0;i<m_plan_inform.COUNT;i++)
		{
			if(m_plan_inform.trains[i].GTID==train.GetShiftTrainIndex())
			{
				m_plan_inform.trains[i].InfoReceivedFlag=1;
				break;
			}
		}
		if(i>=m_plan_inform.COUNT)
			return FALSE;
		CTG_SingleTrain *pold=m_pData->GetSingleTrain(train.GetTrainIndex()) ;
		if(!pold)
		{
			m_pHolder->CheckTrainNo(train,tdms_ctc_alarm);
			DWORD diffflag=0;
			CTG_SingleTrain *prev_train=m_pSourceData->GetSingleTrainByShiftIndex(train.GetShiftTrainIndex(),NULL_STATION);
			CTG_TrainRecord *pfirst_rec=train.GetTrainRecord(0);
			if(!pfirst_rec)
				return FALSE;

			train.m_nTipToDdy=0; // 初始化为0
			CheckAndSetTipToDdyFlag(train);
			
			if(prev_train)
			{
				WORD nTipToDdy=0;
				const char *pdiffstr=Compare2Trains(prev_train,&train,diffflag,0,nTipToDdy);
				if(1==nTipToDdy)
				{
					train.m_nTipToDdy=nTipToDdy;
				}

				STR_CPY(train.GetTrainInfo()->m_data.train_name,"无");
				if(diffflag)
				{
					if(pdiffstr)
					{
						STR_CPY(train.GetTrainInfo()->m_data.train_name,pdiffstr);
					}
					else
					{
						STR_CPY(train.GetTrainInfo()->m_data.train_name,"修改");
					}
				}
			}
			// 对于非局界站，且非始发的置位"无",默认不选中新增
			else if(!GetTGFunctionConfig()->IsFJKStation(pfirst_rec->GetStationNo()) &&  !pfirst_rec->IsStartRec())  
			{
				STR_CPY(train.GetTrainInfo()->m_data.train_name,"无");  
			}
			else
			{
				STR_CPY(train.GetTrainInfo()->m_data.train_name,"新增");
			}

			if(diffflag!=0 || (m_plan_inform.trains[i].LocalAdded==FALSE))
				m_pData->AppendTrain(m_schd_info,train,SYCFLAG_NULL); 
		}
		if(othertrain.GetTrainRecordCount()>0)
		{
			m_pHolder->CheckTrainNo(othertrain,tdms_ctc_alarm);
			pold=m_pData->GetSingleTrain(othertrain.GetTrainIndex()) ;
			if(!pold)
			{
				CTG_TrainRecord *pfirst_rec=othertrain.GetTrainRecord(0);  // bug 此处应合分割后列车othertrain比较，而不是train
				ASSERT(pfirst_rec!=NULL);
				if(!pfirst_rec)
					return FALSE;

				othertrain.m_nTipToDdy=0; // 初始化为0
				CheckAndSetTipToDdyFlag(othertrain);
								
				DWORD diffflag=0;
				CTG_SingleTrain *prev_train=m_pSourceData->GetSingleTrainByShiftIndex(othertrain.GetShiftTrainIndex(),pfirst_rec->GetStationNo());

				if(prev_train)
				{
					WORD nTipToDdy=0;
					const char *pdiffstr=Compare2Trains(prev_train,&othertrain,diffflag,0,nTipToDdy);
					if(1==nTipToDdy)
					{
						othertrain.m_nTipToDdy=nTipToDdy;
					}

					STR_CPY(othertrain.GetTrainInfo()->m_data.train_name,"无");  
					if(diffflag)
					{
						if(pdiffstr)
						{
							STR_CPY(othertrain.GetTrainInfo()->m_data.train_name,pdiffstr);
						}
						else
						{
							STR_CPY(othertrain.GetTrainInfo()->m_data.train_name,"修改");
						}
					}
				}
				else
				{
					STR_CPY(othertrain.GetTrainInfo()->m_data.train_name,"新增");
				}

				if(diffflag!=0 || (m_plan_inform.trains[i].LocalAdded==FALSE))
					m_pData->AppendTrain(m_schd_info,othertrain,SYCFLAG_NULL); 
			}
		}

		m_nDataChanged=1;
		if(flag!=0 && stricmp(token, TOKEN_22_ACK_STN_PLAN) && stricmp(token, TOKEN_24_ACK_FTN_PLAN)
			&& stricmp(token, TOKEN_25_AD_PLAN_INFORM))
		{
			RequestShiftInfoFromOPMS(train.GetShiftTrainIndex()); 
		}
	}
	int recved,total;
	GetRecvCountRate(recved,total);
	if(recved==total)
	{
		GetDlgItem(IDOK)->EnableWindow(TRUE);
		m_pDoc->OpenWaitWindow(NULL); 
		RefreshTrainList();  // 避免过多刷屏
		m_nDataChanged=0;
	}
	else
	{
		CString waitcap;
		waitcap.Format("正在获取车次信息,进度 %d/%d",recved, total);
		m_pDoc->OpenWaitWindow(waitcap); 
	}
	return TRUE;
}
void CDlgTrainList::RequestShiftInfoFromOPMS(DWORD gtid)
{
	if(GetTGFunctionConfig()->IsEnableTDMS5_0()) //TD5.0
	{
		m_pDoc->RequestTrainExpand_KD_YL_CREW_FromOPMS(gtid, 3);
		return;
	}

	CString token,dest;
	long  eid=GetSpecialEntityID(SERVER_TYPE_TDCOMM); 
	TD_GenRequestMessage_15_KEDIAO_INFO(token,dest,gtid);
	TGSendStringMessage(token,dest,eid);
	TD_GenRequestMessage_16_YINGYE_INFO(token,dest,gtid);
	TGSendStringMessage(token,dest,eid);
	TD_GenRequestMessage_20_CREW_INFO(token,dest,gtid);
	TGSendStringMessage(token,dest,eid); 
}

void CDlgTrainList::RefreshTrainList_Orig()
{
	if(m_plan_inform.STTM>0 &&  m_plan_inform.ENDTM>0)
	{
		CString tmrange;
		CTime  sttm,endtm;
		sttm=m_plan_inform.STTM;
		endtm=m_plan_inform.ENDTM;
		tmrange.Format("%s - %s",sttm.Format("%H:%M"),endtm.Format("%H:%M")); 
		GetDlgItem(IDC_ED_JH_TIMERANGE)->SetWindowText(tmrange);
	}
	else
		GetDlgItem(IDC_ED_JH_TIMERANGE)->SetWindowText("");
	
	m_grid.DeleteNonFixedRows();
	for(int i=0;i<m_plan_inform.COUNT;i++)
	{
		if(m_plan_inform.trains[i].LocalAdded)
			continue;
		CString cap,operstr;
		cap.Format("%d", m_plan_inform.trains[i].GTID); 
		if(m_plan_inform.trains[i].OPER==CTD_PlanInform::OPER_ADD_TRAIN)
			operstr="新增";
		else if(m_plan_inform.trains[i].OPER==CTD_PlanInform::OPER_UPDATE_TRAIN)
			operstr="修改";
		else if(m_plan_inform.trains[i].OPER==CTD_PlanInform::OPER_DELETE_TRAIN)
			operstr="删除";
		else
			operstr="未知";
		int row,col;
		row=m_grid.Insert1EmptyRow(cap);
		
		col=m_grid.GetColumnByID(ID_PD_TRAIN_ID);
		if(col>=0)
			m_grid.GetCell(row,col)->SetText(m_plan_inform.trains[i].TNO);
		
		col=m_grid.GetColumnByID(ID_PD_TRAIN_OPER);
		if(col>=0)
			m_grid.GetCell(row,col)->SetText(operstr);
		
		m_grid.SetRowSelected(row,FALSE);
		m_grid.GetCell(row,1)->SetData(0); 
		m_grid.GetCell(row,2)->SetData(0); 
	}
	m_grid.AutoSizeColumns();
	m_grid.Refresh();
}

void CDlgTrainList::RefreshTrainList()
{
	if(m_plan_inform.STTM>0 &&  m_plan_inform.ENDTM>0)
	{
		CString tmrange;
		CTime  sttm,endtm;
		sttm=m_plan_inform.STTM;
		endtm=m_plan_inform.ENDTM;
		tmrange.Format("%s - %s",sttm.Format("%H:%M"),endtm.Format("%H:%M")); 
		GetDlgItem(IDC_ED_JH_TIMERANGE)->SetWindowText(tmrange);
	}
	else
		GetDlgItem(IDC_ED_JH_TIMERANGE)->SetWindowText("");
	
	m_grid.DeleteNonFixedRows();
	CTG_TrainPtrGroup group;
	m_pData->FindTrains(group);

	for(int i=0;i<group.GetTrainCount();i++)
	{
		CString cap;
		int row;
		CTG_SingleTrain *ptrain=group.GetTrainPtr(i);
		cap.Format("%u", ptrain->GetTrainIndex()); 
		CString stroper=ptrain->GetTrainInfo()->m_data.train_name;
		row=m_grid.InsertTrain(cap,stroper,ptrain);   // 用train_name 暂存操作表示

		if(GetTGFunctionConfig()->IsNotSelAdjustPlan())  	// 20171218满足广州局调整计划，默认不选
		{
			m_grid.SetRowSelected(row,FALSE);
		}
		else
		{
			if (GetTGFunctionConfig()->IsEnableRecvAllAdjustPlan())
			{
				m_grid.SetRowSelected(row,TRUE);
			}
			else
			{
				if(stricmp(ptrain->GetTrainInfo()->m_data.train_name,"无"))
					m_grid.SetRowSelected(row,TRUE); 
				else
					m_grid.SetRowSelected(row,FALSE); 		
			}
		}
		
		m_grid.GetCell(row,1)->SetData(ptrain->GetTrainIndex()); 
		m_grid.GetCell(row,2)->SetData(1);
	}
	
	for(int i=0;i<m_plan_inform.COUNT;i++)
	{
		CString cap;
		cap.Format("%d", m_plan_inform.trains[i].GTID); 
		if(m_plan_inform.trains[i].OPER==CTD_PlanInform::OPER_ADD_TRAIN ||
			m_plan_inform.trains[i].OPER==CTD_PlanInform::OPER_UPDATE_TRAIN)
			continue;

		int row=-1,col=-1;
		CTG_SingleTrain *ptrain=m_pSourceData->GetSingleTrainByShiftIndex(m_plan_inform.trains[i].GTID,NULL_STATION);	
		row=m_grid.Insert1EmptyRow(cap);
		col=m_grid.GetColumnByID(ID_PD_TRAIN_ID);
		if(col>=0)
		{
			if(ptrain)
				m_grid.GetCell(row,col)->SetText(ptrain->GetTrainID());
			else
				m_grid.GetCell(row,col)->SetText(m_plan_inform.trains[i].TNO);
		}
		col=m_grid.GetColumnByID(ID_PD_TRAIN_OPER);
		if(col>=0)
			m_grid.GetCell(row,col)->SetText("删除");
		
		if(GetTGFunctionConfig()->IsNotSelAdjustPlan())  // 20171218满足广州局调整计划，默认不选
		{
			m_grid.SetRowSelected(row,FALSE);
		}
		else
		{
			if(ptrain)
				m_grid.SetRowSelected(row,TRUE);
			else
				m_grid.SetRowSelected(row,FALSE);
		}
		m_grid.GetCell(row,1)->SetData(m_plan_inform.trains[i].GTID); 
		m_grid.GetCell(row,2)->SetData(0); 
	}
	m_grid.AutoSizeColumns();
	m_grid.Refresh();
}

void CDlgTrainList::RefreshPlanList()
{
	m_grid_plan.DeleteNonFixedRows();
	list<CPlanInformListItem>::iterator iter=m_pDoc->m_listPlanInform.begin();
	CString xml;
	xml="<InformPlan>";
	while(iter!=m_pDoc->m_listPlanInform.end())
	{
		xml+="<RECORD>";
		if(!stricmp(iter->m_token,TOKEN_KD_PLAN_INFORM))
			InsertXMLNodeValue(xml,"Source","客调");
		else if(!stricmp(iter->m_token,TOKEN_JH_PLAN_INFORM))
			InsertXMLNodeValue(xml,"Source","计划");
		else if(!stricmp(iter->m_token,TOKEN_SS_PLAN_INFORM))
			InsertXMLNodeValue(xml,"Source","通知");
		else if(!stricmp(iter->m_token,TOKEN_22_ACK_STN_PLAN))
			InsertXMLNodeValue(xml,"Source","调整");
		else if(!stricmp(iter->m_token,TOKEN_24_ACK_FTN_PLAN)) 
			InsertXMLNodeValue(xml,"Source","货运调整");
		else if(!stricmp(iter->m_token,TOKEN_25_AD_PLAN_INFORM)) 
			InsertXMLNodeValue(xml,"Source","客运调整");

		InsertXMLNodeValue(xml,"RecvTimeDisp",GetCTCValueDispString(CTC_DATA_DATETIME,iter->m_recvtime,0)); 
		if(iter->m_signtime>0)
		{
			InsertXMLNodeValue(xml,"SignTimeDisp",GetCTCValueDispString(CTC_DATA_DATETIME,iter->m_signtime,0)); 
			InsertXMLNodeValue(xml,"COLOR","GRAY");
		}
		else
			InsertXMLNodeValue(xml,"COLOR","RED");
		InsertXMLNodeValue(xml,"ITEMDATA1",iter->m_id);
		xml+="</RECORD>";
		iter++;
		if(m_nCurrentSelPlan==-1)
			m_nCurrentSelPlan=1;
	}
	xml+="</InformPlan>";
	m_grid_plan.InsertRows(xml,"InformPlan/RECORD");
}

void CDlgTrainList::RequestPlanDetail()
{
	if(m_nCurrentSelPlan<=0)
		return;
	DWORD id=m_grid_plan.GetCell(m_nCurrentSelPlan,1)->GetData();
	CPlanInformListItem *pitem=m_pDoc->GetPlanInformByID(id);
	if(pitem)
	{
		SetPlanInformXML(pitem->m_token,pitem->m_content);
		m_pData->DeleteAllData(m_schd_info,SYCFLAG_NULL);
		RefreshTrainList_Orig();
		if(pitem->m_signtime==0 && stricmp(pitem->m_token,TOKEN_SS_PLAN_INFORM) 
			&& stricmp(pitem->m_token,TOKEN_22_ACK_STN_PLAN)
			&& stricmp(pitem->m_token,TOKEN_24_ACK_FTN_PLAN)
			&& stricmp(pitem->m_token,TOKEN_25_AD_PLAN_INFORM))  // 未签收过的计划,非日班计划通知 
		{
			GetDlgItem(ID_REQUEST_DATA)->EnableWindow(TRUE);
			RequestNewYLFromOPMS();
		}
		else if(!stricmp(pitem->m_token,TOKEN_22_ACK_STN_PLAN) ) //针对单趟车
		{
			GetDlgItem(ID_REQUEST_DATA)->EnableWindow(FALSE);
			if (m_plan_inform.trains[0].OPER != CTD_PlanInform::OPER_DELETE_TRAIN)
			{
				BOOL cor;
 				cor=ProcessTrainMsgFromOPMS(pitem->m_token,pitem->m_content);
 				if(!cor)
 					m_pDoc->SendGpcAckMsg(TD_GPC_ACK_STN,false,m_plan_inform.JHID,m_plan_inform.MSGID,m_plan_inform.JHTNM,"数据解析失败!");
			}
			else
				RefreshTrainList();
		}
		else if (!stricmp(pitem->m_token,TOKEN_24_ACK_FTN_PLAN)) // 货运调整计划
		{ 
			GetDlgItem(ID_REQUEST_DATA)->EnableWindow(FALSE);
			
			MSXML::IXMLDOMNodePtr pNode;
			MSXML::IXMLDOMDocumentPtr pXMLDoc;
			HRESULT hr = pXMLDoc.CreateInstance(MSXML::CLSID_DOMDocument);
			if(FAILED(hr))
			{
				_com_error er(hr);
				AfxMessageBox(er.ErrorMessage());
				return;
			}
			if(VARIANT_TRUE != pXMLDoc->loadXML( _bstr_t( pitem->m_content)))
				return;

			pNode =pXMLDoc->selectSingleNode(_bstr_t(pitem->m_token));
			if(pNode==NULL)
				return;

			MSXML::IXMLDOMNodeListPtr pTrainList = pNode->selectNodes("ST_TRAIN");
			int cnt=0;
			for (int i=0;i<pTrainList->Getlength();i++)
			{
				BOOL cor;
				cor=ProcessTrainMsgFromOPMS("ST_TRAIN",pTrainList->Getitem(i)->Getxml());
				if(!cor)
					cnt++;
			}
			if (cnt>0)
				m_pDoc->SendGpcAckMsg(TD_GPC_ACK_FTN,false,m_plan_inform.JHID,m_plan_inform.MSGID,m_plan_inform.JHTNM,"数据解析失败!");
		}
		else if (!stricmp(pitem->m_token,TOKEN_25_AD_PLAN_INFORM)) // 客运调整计划
		{ 
			GetDlgItem(ID_REQUEST_DATA)->EnableWindow(FALSE);
			
			MSXML::IXMLDOMNodePtr pNode;
			MSXML::IXMLDOMDocumentPtr pXMLDoc;
			HRESULT hr = pXMLDoc.CreateInstance(MSXML::CLSID_DOMDocument);
			if(FAILED(hr))
			{
				_com_error er(hr);
				AfxMessageBox(er.ErrorMessage());
				return;
			}
			if(VARIANT_TRUE != pXMLDoc->loadXML( _bstr_t( pitem->m_content)))
				return;

			pNode =pXMLDoc->selectSingleNode(_bstr_t(pitem->m_token));
			if(pNode==NULL)
				return;

			MSXML::IXMLDOMNodeListPtr pTrainList = pNode->selectNodes("ST_TRAIN");
			int cnt=0;
			for (int i=0;i<pTrainList->Getlength();i++)
			{
				BOOL cor;
				cor=ProcessTrainMsgFromOPMS("ST_TRAIN",pTrainList->Getitem(i)->Getxml());
				if(!cor)
					cnt++;
			}
			if (cnt>0)
				m_pDoc->SendGpcAckMsg(TD_GPC_ACK_PLAN_INFORM,false,m_plan_inform.JHID,m_plan_inform.MSGID,m_plan_inform.JHTNM,"数据解析失败!");
		}
		else 
		{
			GetDlgItem(IDOK)->EnableWindow(FALSE);
			//GetDlgItem(ID_REQUEST_DATA)->EnableWindow(FALSE);
		}
	}
}

void CDlgTrainList::OnPlanGridSelectChange(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_GRIDVIEW *pv=(NM_GRIDVIEW *)pNMHDR;
	if(pv->iRow>=1)
	{
		if(m_nCurrentSelPlan!=pv->iRow)
		{
			m_nCurrentSelPlan=pv->iRow;
			RequestPlanDetail();
		}
	}
}

void CDlgTrainList::OnTimer(UINT_PTR IDEvent)
{
	if(m_nDataChanged>0)
	{
		if(m_nDataChanged++>3)
		{
			RefreshTrainList();  // 避免过多刷屏

			m_nDataChanged=0;
		}
	}
}
void CDlgTrainList::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	m_nCurrentSelPlan=-1;
	m_pDoc->OpenWaitWindow(NULL);
}

void CDlgTrainList::SplitTDTrain(CTD_Train& train, CTD_Train& other_train,int rec_index) const
{
	CTD_Train srctrain=train;
	other_train=srctrain;

	train.PurgeTrainRecord();
	for(int n=0; n<rec_index; n++)
	{
		train.m_TDRecordList.push_back(srctrain.m_TDRecordList[n]);
	}

	other_train.PurgeTrainRecord();
	for(int n=rec_index; n<srctrain.GetTrainTDRecordCount(); n++)
	{
		other_train.m_TDRecordList.push_back(srctrain.m_TDRecordList[n]);
	}

	// 有后续列车,则清除前半部分列车为交出参数,后半部分列车为接入
	if(other_train.GetTrainTDRecordCount()>0)
	{
		//设置前半部分列车为交出参数
		train.m_TDInfo.TOST="";
		train.m_TDInfo.OFLAG=0;
		//设置前半部分列车为接入参数
		other_train.m_TDInfo.FROMST="";
		other_train.m_TDInfo.IFLAG=0;
	}
		
	return;
}

// 用于切割TD类型列车
BOOL CDlgTrainList::SplitTDTrainx(CTD_Train &tdtrain, std::vector<CTD_Train>& train_list,const CTG_ScheduleInfo &schd, int entry_list[16], BOOL newflag) const
{
	train_list.clear();

	int entry_count=0;
	entry_list[entry_count]=0;
	entry_count++;

	CTD_Train othertrain;
	
	TRAIN_INDEX shift_index=tdtrain.GetTrainShiftIndex();
	CTD_Train newtrain=tdtrain;
	// 一次分割
	for(int m=1; m<m_nSplitNum; m++)
	{
		for(int i=0;i<newtrain.GetTrainTDRecordCount()-1;i++)
		{
			CTD_TrainRecordInfo *prec=newtrain.GetTrainTDRecord(i);
			CTD_TrainRecordInfo *nextprec=newtrain.GetTrainTDRecord(i+1);

			if((prec!=NULL)&&(nextprec!=NULL)
				&&(prec->STID==m_cSplitStationList[m-1]) 
				&& (nextprec->STID==m_cSplitStationList[m]))
			{
				entry_list[entry_count]=m_cSplitEntryList[m-1];
				entry_count++;
				entry_list[entry_count]=m_cSplitEntryList[m];
				entry_count++;
				SplitTDTrain(newtrain,othertrain,i+1);
				tdtrain=newtrain;
				tdtrain.SetTrainShiftIndex(shift_index); //20141204 车次分割 30bit为1,解决南宁全局ID分割重复
								
				othertrain.SetTrainShiftIndex(shift_index | 0x40000000L); //20141204 车次分割 30bit为1,解决南宁全局ID分割重复
			
				m=m_nSplitNum;
				break;
			}
		}
	}

	// 二次分割
	newtrain=othertrain;
	for(int m=1; m<m_nSplitNum; m++)
	{
		for(int i=0;i<newtrain.GetTrainTDRecordCount()-1;i++)
		{
			CTD_TrainRecordInfo *prec=newtrain.GetTrainTDRecord(i);
			CTD_TrainRecordInfo *nextprec=newtrain.GetTrainTDRecord(i+1);

			if((prec!=NULL)&&(nextprec!=NULL)
				&&(prec->STID==m_cSplitStationList[m-1]) 
				&& (nextprec->STID==m_cSplitStationList[m]))
			{
				entry_list[entry_count]=m_cSplitEntryList[m-1];
				entry_count++;
				entry_list[entry_count]=m_cSplitEntryList[m];
				entry_count++;
				SplitTDTrain(newtrain,othertrain,i+1);
				
				newtrain.SetTrainShiftIndex(shift_index | 0x40000000L); //20141204 车次分割 30bit为1,解决南宁全局ID分割重复
				if(newtrain.GetTrainTDRecordCount()>0)
				{
					train_list.push_back(newtrain);
				}

				othertrain.SetTrainShiftIndex(shift_index | 0x80000000L); //20141204 车次分割 30bit为1,解决南宁全局ID分割重复
				
				m=m_nSplitNum;
				break;
			}
		}
	}
	entry_list[entry_count]=0;
	entry_count++;

	if(othertrain.GetTrainTDRecordCount()>0)
	{
		train_list.push_back(othertrain);
	}

	return TRUE;
}

int CDlgTrainList::SetNewTrainByOldTrain(int nSection, const SetInfo& info, CTD_Train& tdtrain, CTG_SingleTrain& newtrain, CTG_SingleTrain *ptrain) 
{
	CTG_ScheduleInfo schdinfo;
	m_pData->GetScheduleInfo(schdinfo);

	time_t sttm=m_plan_inform.STTM;
	time_t edtm=m_plan_inform.ENDTM;
	if(NULL != ptrain)
	{
		newtrain=*ptrain;
	}
	
	CTG_TrainRecordList reclist;
	if(newtrain.GetTrainRecordCount()>0)
		reclist=*newtrain.GetRecordList();  // 保留原有信息

	newtrain.GetRecordList()->PurgeRecords(); 
	BOOL ign_time=FALSE;
	if(newtrain.HasActualRecord() >= 0)
		ign_time=TRUE;

	BOOL bUseOPMSShiftSchdSide = GetTGFunctionConfig()->IsEnableOPMSShiftSchdSide(nSection);

	CTD_TrainInfo traininfo=tdtrain.m_TDInfo;
	CTD_TrainRecordInfo recordinfo;
	int rec_count=tdtrain.GetTrainTDRecordCount();
	for(WORD idx = 0; idx < rec_count; idx++)
	{
		recordinfo=tdtrain.m_TDRecordList[idx];

		GpcLogTrainMessage(0, 0, "OPMS传来的股道,TID=%s,ST=%s(%d) SIDENO=%d",
					traininfo.TNO,recordinfo.STNM,recordinfo.STID,recordinfo.SIDENO);

		if( !bUseOPMSShiftSchdSide)
		{
			recordinfo.SIDENO=255;
		}

		int stop_time;
		if(recordinfo.DPTM>recordinfo.ARTM)
			stop_time=recordinfo.DPTM - recordinfo.ARTM;
		else
			stop_time=0;

		bool bAcceptSide=true;
		CTG_TrainRecord  newrecord;
		if(idx<reclist.GetRecordCount())
		{
			if(reclist[idx].GetStationNo() == recordinfo.STID)
			{
				newrecord=reclist[idx];
				bAcceptSide=false;

				if((idx==0) && info.bFirst)
				{
					if(sttm>0)
					{
						if(recordinfo.DPTM<sttm || newrecord.m_data.depart<sttm)
							ign_time=TRUE;
					}
					// 非分界站,且非始发---局内接入列车
					if(!GetTGFunctionConfig()->IsFJKStation(recordinfo.STID) && traininfo.IFLAG!=1)  // 非分界站,且非始发
						ign_time=TRUE;
				}
			}
		}
	
		if(ign_time && !GetTGFunctionConfig()->IsEnableRecvAdjustPlanTime())
		{
			recordinfo.ARTM=0;
			recordinfo.DPTM=0;
		}
		TD_ImportRecordInfoFromTD(newrecord, recordinfo, traininfo, bAcceptSide,tdms_ctc_alarm);
					
		if(newrecord.m_data.entry==255|| newrecord.m_data.exit==255)
		{
			GpcLogTrainMessage(0, 0, "OPMS传来线别转换为出入口错误,TID=%s,ST=%s (%d,%d)=>(%d,%d)",
				traininfo.TNO,recordinfo.STNM,recordinfo.ENTRY_LINE,recordinfo.EXIT_LINE,
				newrecord.m_data.entry, newrecord.m_data.exit);
		}

		int prev_stop_time=newrecord.m_data.depart-newrecord.m_data.arrive;
		if(prev_stop_time<stop_time)
		{
			if(!newrecord.IsActualDepartTimeRecord())  // 非实际点才更新 20090811
			{
				newrecord.m_data.depart=newrecord.m_data.arrive+stop_time;
				for(int x=idx+1;x<reclist.GetRecordCount();x++)
				{
					reclist[x].m_data.arrive+=stop_time-prev_stop_time;
					if(!reclist[x].IsTerminalRec())
						reclist[x].m_data.depart+=stop_time-prev_stop_time;
				}
			}
		}
		newrecord.rec_index=idx;
		newtrain.GetRecordList()->AddRecord(newrecord);
	}

	// 机车号
	newtrain.UpdateEngineInfo(tdtrain.m_cEngineList,1);
	//
	newtrain.GetTrainInfo()->m_data.delete_flag=newtrain.GetTrainRecordCountWithDeleted();     
	int nMode=GetTGFunctionConfig()->GetAdjustAcceptRelationMode();
	TD_ImportTrainInfoFromTD(newtrain,traininfo,nMode);
	// 用shift_index作为train_index
	newtrain.SetTrainIndex(newtrain.GetShiftTrainIndex());

	SetTrainFirstStationEntry(traininfo, newtrain, 2);
	SetTrainLastStationExit(traininfo, newtrain, 2);
	
	CTgX* pTgX=m_pTgMMI->GetTGCtrl();
	if(pTgX!=NULL && m_pHolder!=NULL)
	{
		if(GetTGFunctionConfig()->IsCalShiftSchdExitEntry())
		{
			if(!m_pHolder->TGOCXSetTrainExitEntry(newtrain, pTgX))
			{
				CString str;
				str.Format("车次:%s 径路计算失败,请确认列车径路", newtrain.GetTrainID());
				tdms_ctc_alarm.Add(str);
			}
		}
	}

	BOOL  newflag=FALSE;
	if(newtrain.HasActualRecord() < 0) 
		newflag=TRUE;
	
	const char *train_name = newtrain.GetTrainID();
	BYTE type, run_type, prio;
	if(GetTGBasicParam()->GetTrainTypeAndPrio(train_name, &type, &run_type, &prio))
	{
		newtrain.GetTrainInfo()->m_data.type = type;
		newtrain.GetTrainInfo()->m_data.run_type = run_type;
		newtrain.GetTrainInfo()->m_data.priority = prio;
	}

	bool bGoodsTrain=false; // 计算列车类型,下面需要判断客货列车
	if(newtrain.IsPassengerTrain())
	{
		bGoodsTrain=false;
	}
	else
	{
		bGoodsTrain=true;
	}
	
	if(bGoodsTrain && (info.arrive>0)) // 货物列车,检查首站到发时间是否为0,如果是零则设置为合适时间info.tmarrive
	{
		m_pHolder->SetTrainFirstStationZeroTMToArriveTm(newtrain, info.arrive);
	}
	m_pHolder->GenTrain_OPMS(newtrain, schdinfo, newflag);

	if(info.entry>0) // 设置分割后列车入口
	{
		CTG_TrainRecord * prec=newtrain.GetTrainFirstRecord();
		if(prec!=NULL)
		{
			prec->m_data.entry=info.entry;
		}
	}

	if(info.exit>0) // 设置分割后列车出口
	{
		CTG_TrainRecord * prec=newtrain.GetTrainLastRecord();
		if(prec!=NULL)
		{
			prec->m_data.exit=info.exit;
		}
	}
	int tmlast=0;
	if(bGoodsTrain) // 货物列车推算下一站接入时间
	{
		tmlast=m_pHolder->CalcTrainLastSt_NextStTime(newtrain);
	}

	return tmlast;
}

BOOL CDlgTrainList::ProcessTrainMsgFromOPMS_NEW(const char *token,const char *content)
{
	if(!m_pData)
		return FALSE;

	CTG_ScheduleInfo schdinfo;
	m_pData->GetScheduleInfo(schdinfo);

	int flag=0;
	CTD_Train tdtrain;
	if(!TD_LoadTrainFromXMLx(schdinfo, tdtrain, content, token, &flag))
		return FALSE;
	
	TRAIN_INDEX nShiftIndex=tdtrain.GetTrainShiftIndex();
	if(flag==0)
	{
		for(int i=0;i<m_plan_inform.COUNT;i++)
		{
			if(m_plan_inform.trains[i].GTID==nShiftIndex)
			{
				m_plan_inform.trains[i].OPER=CTD_PlanInform::OPER_NOEXSIT; // 不存在	
				m_plan_inform.trains[i].TNO=tdtrain.m_TDInfo.TNO;   
				m_plan_inform.trains[i].InfoReceivedFlag=1;
			}
		}
	}
	else
	{
		int  entry_list[16];
		memset(entry_list, 0, sizeof(entry_list));
		std::vector<CTD_Train> othertrain_list;
		if(!SplitTDTrainx(tdtrain, othertrain_list, m_schd_info, entry_list, TRUE))
			return FALSE;

		int i;
		for(i=0;i<m_plan_inform.COUNT;i++)
		{
			if(m_plan_inform.trains[i].GTID==nShiftIndex)
			{
				m_plan_inform.trains[i].InfoReceivedFlag=1;
				break;
			}
		}
		if(i>=m_plan_inform.COUNT)
			return FALSE;
		
		CTD_TrainInfo traintdinfo=tdtrain.m_TDInfo;
		
		SetInfo info;
		// 第一段或未分割列车
		info.arrive=0;  // 首段时间设置为0
		info.bFirst=true; // 首段列车
		CTG_SingleTrain *pold=m_pData->GetSingleTrain(nShiftIndex) ;
		if(NULL==pold)
		{
			CTD_TrainRecordInfo *pfirst_rec=tdtrain.GetTrainTDRecord(0);
			if(!pfirst_rec)
				return FALSE;
				
			int station=pfirst_rec->STID;
			DWORD diffflag=0;
			CTG_SingleTrain *prev_train=m_pSourceData->GetSingleTrainByShiftIndex(nShiftIndex, station);
			CTG_SingleTrain train;
			info.entry=entry_list[0];
			info.exit=entry_list[1];
			
			info.arrive=SetNewTrainByOldTrain(schdinfo.m_nSectionID, info, tdtrain, train, prev_train);
			if(prev_train)
			{
				WORD nTipToDdy=0;
				const char *pdiffstr=Compare2Trains(prev_train,&train,diffflag,0,nTipToDdy);
				if(1==nTipToDdy)
				{
					train.m_nTipToDdy=nTipToDdy;
				}

				STR_CPY(train.GetTrainInfo()->m_data.train_name,"无");
				if(diffflag)
				{
					if(pdiffstr)
					{
						STR_CPY(train.GetTrainInfo()->m_data.train_name,pdiffstr);
					}
					else
					{
						STR_CPY(train.GetTrainInfo()->m_data.train_name,"修改");
					}
				}
			}
			// 对于非局界站，且非始发的置位"无",默认不选中新增
			else if(!GetTGFunctionConfig()->IsFJKStation(station) && tdtrain.m_TDInfo.IFLAG!=1)  
			{
				STR_CPY(train.GetTrainInfo()->m_data.train_name,"无");  
			}
			else
			{
				STR_CPY(train.GetTrainInfo()->m_data.train_name,"新增");
			}
			
			if(diffflag!=0 || (m_plan_inform.trains[i].LocalAdded==FALSE))
			{
				m_pData->AppendTrain(m_schd_info,train,SYCFLAG_NULL); 
			}
		}
		// 分割的后续列车
		info.bFirst=false; // 后续分割列车
		for(int i=0; i<othertrain_list.size(); i++)
		{
			CTD_Train tdothertrain=othertrain_list[i];
			
			TRAIN_INDEX nOtherShiftIndex=tdothertrain.GetTrainShiftIndex();
			pold=m_pData->GetSingleTrain(nOtherShiftIndex) ;
			if(NULL!=pold)
			{
				info.arrive=0;
			}
			else
			{
				CTD_TrainRecordInfo *pfirst_rec=tdothertrain.GetTrainTDRecord(0); 
				if(!pfirst_rec)
					return FALSE;

				info.entry=entry_list[2+2*i];
				info.exit=entry_list[2+2*i+1];

				DWORD diffflag=0;
				int station=pfirst_rec->STID;
				CTG_SingleTrain *prev_train=m_pSourceData->GetSingleTrainByShiftIndex(nOtherShiftIndex, station);
				CTG_SingleTrain othertrain;
				info.arrive=SetNewTrainByOldTrain(schdinfo.m_nSectionID, info, tdothertrain, othertrain, prev_train);
				if(prev_train)
				{
					WORD nTipToDdy=0;
					const char *pdiffstr=Compare2Trains(prev_train,&othertrain,diffflag,0,nTipToDdy);
					if(1==nTipToDdy)
					{
						othertrain.m_nTipToDdy=nTipToDdy;
					}

					STR_CPY(othertrain.GetTrainInfo()->m_data.train_name,"无");  
					if(diffflag)
					{
						if(pdiffstr)
						{
							STR_CPY(othertrain.GetTrainInfo()->m_data.train_name,pdiffstr);
						}
						else
						{
							STR_CPY(othertrain.GetTrainInfo()->m_data.train_name,"修改");
						}
					}
				}
				else
				{
					STR_CPY(othertrain.GetTrainInfo()->m_data.train_name,"新增");
				}

				if(diffflag!=0 || (m_plan_inform.trains[i].LocalAdded==FALSE))
				{
					m_pData->AppendTrain(m_schd_info,othertrain,SYCFLAG_NULL);
				}
			}
		}		
		m_nDataChanged=1;
		if(flag!=0 && stricmp(token, TOKEN_22_ACK_STN_PLAN) && stricmp(token, TOKEN_24_ACK_FTN_PLAN)
			&& stricmp(token, TOKEN_25_AD_PLAN_INFORM))
		{
			RequestShiftInfoFromOPMS(nShiftIndex);
		}
	}

	int recved,total;
	GetRecvCountRate(recved,total);
	if(recved==total)
	{
		GetDlgItem(IDOK)->EnableWindow(TRUE);
		m_pDoc->OpenWaitWindow(NULL); 
		RefreshTrainList();  // 避免过多刷屏
		m_nDataChanged=0;
	}
	else
	{
		CString waitcap;
		waitcap.Format("正在获取车次信息,进度 %d/%d",recved, total);
		m_pDoc->OpenWaitWindow(waitcap); 
	}
	return TRUE;
}
