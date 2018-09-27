// DlgTrainList.cpp : 实现文件
//

#include "stdafx.h"
#include "resource.h"
#include "DlgNeibTrainList.h"
#include "TgEditView.h"
#include "tg_binxml.h"
#include "tg_schedule_base.h"
#include "tg_work_schdule_holder.h"
#include <baseMsg\msgid.h>
#include <baseMsg\gpc_msgid.h>
#include <BaseMsg\clbth.h>
#include "icommtask.h"
#include "tgdocument.h"
#include "tgpspdata.h"
#include "utility/xml_utility.h"
#include "dlgwait.h"
#include "sysshare.h"
#include ".\dlgneibtrainlist.h"
#include "ChangeTrainRoute.h"
#include "tg_addtrain_logic.h"
#include "schd_tgmmi.h"
#include "DlgConfirm.h"

#define IDC_SBTX_TRAINLIST  5211
#define IDC_SBTX_MARKLIST   5212
// CDlgNeibTrainList 对话框
extern int DB_GetEntityStatus(long eid);
IMPLEMENT_DYNAMIC(CDlgNeibTrainList, CDialog)
CDlgNeibTrainList::CDlgNeibTrainList(const char *caption,CTgDocument *pdoc,
									 int sche_type,int type, 
									 int hour, int nSection, CWnd* pParent)  // 标准构造函数
: CDialog(CDlgNeibTrainList::IDD, pParent)
{
	m_pDoc=pdoc;
	m_pHolder=m_pDoc->GetTGDataHolder();
	m_nScheduleType= sche_type;
	m_pData=NULL;
	m_pOtherData=NULL;
	m_szCaption="接收[";
	m_szCaption += caption;
	m_szCaption += "]邻台计划";

	if(m_nScheduleType==CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT)
		m_pSourceData=m_pHolder->GetShiftScheduleData();
	else if(m_nScheduleType==CTG_ScheduleInfo::SCHEDULE_TYPE_WORK)
		m_pSourceData=m_pHolder->GetWorkScheduleData();
	else
		m_pSourceData=NULL;

	if(m_pSourceData)
		m_pSourceData->GetScheduleInfo(m_schd_info);

	m_nDataChanged=0;
	m_pWaitWnd=NULL;
	m_nCurrentSel=-1;
	m_pWintgNewServerWork=NULL;
	m_nStatus=0;
	m_pTgMMI=NULL;

	m_nHour = hour;
	m_nSectionNo=nSection;
}

CDlgNeibTrainList::CDlgNeibTrainList(const char *caption,CTgDocument *pdoc,int sche_type,int type,CWnd* pParent /*=NULL*/)
: CDialog(CDlgNeibTrainList::IDD, pParent)
{
	m_pDoc=pdoc;
	m_pHolder=m_pDoc->GetTGDataHolder();
	m_nScheduleType= sche_type;
	m_pData=NULL;
	m_pOtherData=NULL;
	m_szCaption=caption;

	if(m_nScheduleType==CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT)
		m_pSourceData=m_pHolder->GetShiftScheduleData();
	else if(m_nScheduleType==CTG_ScheduleInfo::SCHEDULE_TYPE_WORK)
		m_pSourceData=m_pHolder->GetWorkScheduleData();
	else
		m_pSourceData=NULL;

	if(m_pSourceData)
		m_pSourceData->GetScheduleInfo(m_schd_info);

	m_nDataChanged=0;
	m_pWaitWnd=NULL;
	m_nCurrentSel=-1;
	m_pWintgNewServerWork=NULL;
	m_nStatus=0;
	m_pTgMMI=NULL;
	m_nHour = 0;
	m_nSectionNo = 0;
}

CDlgNeibTrainList::~CDlgNeibTrainList()
{
	if(m_pData)
		delete m_pData;
	if(m_pOtherData)
		delete m_pOtherData;
	if(m_pWintgNewServerWork)
		delete m_pWintgNewServerWork;
}

BOOL    CDlgNeibTrainList::AddTrainIDToPlanInform(const CTG_SingleTrain *ptrain)
{
	TRAIN_INDEX shift_index = ptrain->GetShiftTrainIndex();
	if(shift_index == NULL_TRAININDEX)
		return FALSE;

	shift_index &=0x3FFFFFFF;

	int idx;
	for(idx=0;idx<m_plan_inform.COUNT;idx++)
	{
		if(m_plan_inform.trains[idx].GTID== shift_index)
			break;
	}
	if(idx<m_plan_inform.COUNT)
		return FALSE;
	
	if(idx<CTD_PlanInform::MAX_NFORM_TRAIN_NUM)
	{
		m_plan_inform.trains[idx].GTID=shift_index;
		m_plan_inform.trains[idx].TNO=ptrain->GetTrainID();
		m_plan_inform.trains[idx].OPER=CTD_PlanInform::OPER_ADD_TRAIN; 
		m_plan_inform.trains[idx].InfoReceivedFlag=0;
		m_plan_inform.trains[idx].KDID=0;
		m_plan_inform.trains[idx].LocalAdded=TRUE; // 本地添加的
		m_plan_inform.COUNT++;
	}
	
	return TRUE;
}


void CDlgNeibTrainList::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgNeibTrainList, CDialog)
	ON_NOTIFY(GVN_SELCHANGING,IDC_SBTX_TRAINLIST,OnGridSelectChange)
	ON_NOTIFY(WM_USER+5050,IDC_SBTX_TRAINLIST,OnGridSelectChangeCheck)
	ON_BN_CLICKED(ID_REQUEST_DATA, OnBnClickedReq)
	ON_MESSAGE(UM_UNI_MESSAGE, OnCommMsg)
	ON_WM_TIMER()
	ON_WM_DESTROY()
END_MESSAGE_MAP()

// || !stricmp(token,TOKEN_4_ACK_YL_BY_TRAINNUM)
//			 || !stricmp(token,TOKEN_5_ACK_YL_BY_TRAINNUM_TIME))
LRESULT CDlgNeibTrainList::OnCommMsg(WPARAM wParam, LPARAM lParam) 
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
					if(GetTGFunctionConfig()->IsNewNeibModeAddTrain())
						ProcessTrainMsgFromOPMS_NEW(token,content);
					else
						ProcessTrainMsgFromOPMS(token,content);
				}
				else if(!stricmp(token,TOKEN_15_ACK_KEDIAO_INFO))	
				{
					CTG_TrainKediaoInfo kd;
					if(TD_GetKediaoInfoFromXML(kd,content,token))
					{
						if(kd.msgid==4)
						{
							CTG_TrainScheduleBase *pTmpData=NULL;
							CTG_SingleTrain *ptrain=GetSingleTrainByShiftIndex(kd.GetShiftIndex(), NULL, &pTmpData);
							if(pTmpData!=NULL && ptrain!=NULL)
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
						if(yy.msgid==4)
						{
							CTG_TrainScheduleBase *pTmpData=NULL;
							CTG_SingleTrain *ptrain=GetSingleTrainByShiftIndex(yy.GetShiftIndex(), NULL, &pTmpData);
							if(pTmpData!=NULL && ptrain!=NULL)
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
						if(crew.msgid==4)
						{
							CTG_TrainScheduleBase *pTmpData=NULL;
							CTG_SingleTrain *ptrain=GetSingleTrainByShiftIndex(crew.GetShiftIndex(), NULL, &pTmpData);
							if(pTmpData!=NULL && ptrain!=NULL)
							{
								ptrain->GetTrainInfoExpand()->UpdateTrainCrew(crew);
							}
						}
					}
				}
			}
			else
			{
				if(!stricmp(token,TOKEN_3_ACK_YL_BY_TIME) || !stricmp(token,TOKEN_8_ACK_NEWYL))
				{
					if(GetTGFunctionConfig()->IsNewNeibModeAddTrain())
						ProcessTrainMsgFromOPMS_NEW(token,content);
					else
						ProcessTrainMsgFromOPMS(token,content);
				}
				else if(!stricmp(token,TOKEN_15_ACK_KEDIAO_INFO) ||  !stricmp(token,TOKEN_16_ACK_YINGYE_INFO))
				{
					ProcessShiftInfoMsgFromOPMS(token,content);
				}
			}
		}
	}
	FREE_UNI_MESSAGE_STRUCT(pMsgStruct);
	return 0;
}

BOOL CDlgNeibTrainList::OnInitDialog()
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

	if(!m_pData)
	{
		m_pData=new CTG_TrainScheduleBase();
		m_pData->SetScheduleInfo(m_schd_info);
	}
	if(!m_pOtherData)
	{
		m_pOtherData=new CTG_TrainScheduleBase();
		m_pOtherData->SetScheduleInfo(m_schd_info);
	}
	
	// 初始化邻台列表
	int nSectionNum = 0;
	int nSectionList[MAX_SECTION_NUM];
	memset(nSectionList, 0, sizeof(nSectionList));
	gpcdef.GetNeighberSectionList(m_schd_info.m_nSectionID,nSectionNum,nSectionList);
	
	
	CComboBox *pComboNeibBox=(CComboBox *)GetDlgItem(IDC_CBO_NEIB_LIST);
	if(NULL != pComboNeibBox)
	{
		for (int i = 0; i < nSectionNum; i++)//int
		{
			CString strSection = gpcdef.GetSectionName(nSectionList[i]);
			int sel=pComboNeibBox->AddString(strSection);
			if(sel>=0)
			{
				pComboNeibBox->SetItemData(sel,nSectionList[i]); 
			}
		}

		int cursel=0;
		for (int i = 0; i < nSectionNum; i++)//int
		{
			if(m_nSectionNo == nSectionList[i])
			{
				cursel = i;
				break;
			}
		}
		pComboNeibBox->SetCurSel(cursel);

		if(m_nSectionNo != 0)
		{
			pComboNeibBox->EnableWindow(FALSE);
		}
	}

	/// 初始化时间范围
	CComboBox *pComboBox=(CComboBox *)GetDlgItem(IDC_CBO_STARTTIME2);
	if(NULL != pComboBox)
	{
		CString text;
		for(int i=1;i<=6;i++)
		{
			text.Format("%02d",i);
			pComboBox->AddString(text);
		}

		if(m_nHour == 0)
		{	
			int hour=GetTGFunctionConfig()->GetDefaultNeibPlanTRange();
			if(hour>=1 && hour<=6)
				pComboBox->SetCurSel(hour-1);
			else
				pComboBox->SetCurSel(2);
		}
		else 
		{
			CWnd* pWnd = GetDlgItem(ID_REQUEST_DATA);
			pWnd->SetWindowText("接收计划");
			if(m_nHour>=1 && m_nHour<=6)
				pComboBox->SetCurSel(m_nHour-1);
			else
				pComboBox->SetCurSel(2);
		}
	}

	((CButton *)GetDlgItem(IDC_CHK_PASSENGER))->SetCheck(1);
	((CButton *)GetDlgItem(IDC_CHK_FREIGHT))->SetCheck(1);
	RegisterMsgProcHandleWnd(GetSafeHwnd(),MSGID_MsgStringMessage,0);
	RequestPlanDetail();

	m_bSelAll=m_grid.IsRowSelected(0)>0;
	SetTimer(2002,100,NULL); // 时间间隔要短,每个站都向邻台请求一次
	return TRUE;
}

void CDlgNeibTrainList::RequestPlanDetail()
{
	GetDlgItem(IDOK)->EnableWindow(FALSE);
}

CTG_SingleTrain *CDlgNeibTrainList::GetFromTmpMemoryTrain(TRAIN_INDEX trainindex)
{
	if(m_pData==NULL)
		return NULL;

	CTG_SingleTrain *ptrain=m_pData->GetSingleTrain(trainindex);
	if(ptrain==NULL && m_pOtherData!=NULL)
	{
		ptrain=m_pOtherData->GetSingleTrain(trainindex);
	}

	return ptrain;
}

CTG_SingleTrain *CDlgNeibTrainList::GetSingleTrainByShiftIndex(TRAIN_INDEX shift_index, STATION_NO station, CTG_TrainScheduleBase** pData)
{
	*pData=NULL;
	if(m_pData==NULL)
		return NULL;
	CTG_SingleTrain *ptrain=m_pData->GetSingleTrainByShiftIndex(shift_index, station);
	if(NULL==ptrain)
	{
		*pData=NULL;
		if(m_pOtherData!=NULL)
		{
			ptrain=m_pOtherData->GetSingleTrainByShiftIndex(shift_index, station);
		}

		if(NULL!=ptrain)
		{
			*pData=m_pOtherData;
		}
	}
	else
	{
		*pData=m_pData;
	}

	return ptrain;
}

void CDlgNeibTrainList::OnOK()
{
	if(!m_pData || !m_pOtherData)
		return;

	if (GetTGFunctionConfig()->IsNeibplanOnlyUseOPMSSchd())
	{
		CStringArray err_trainid;
		for(int i=1;i<m_grid.GetRowCount();i++)
		{
			if(!m_grid.IsRowSelected(i))
				continue;

			CString strOper=m_grid.GetCell(i,0)->GetText();
			CString strTrainId=m_grid.GetCell(i,2)->GetText();
			CString strSource=m_grid.GetCell(i,5)->GetText();
			long GTId = atol(m_grid.GetCell(i,1)->GetText());

			if (strOper!="新增")
				continue;

			if (strSource!="调整计划" && strSource!="日班计划" && GTId !=0)
			{
				CString str;
				str.Format("新增车次 %s 全局ID %u 来源不是计划台", strTrainId, GTId);
				err_trainid.Add(str);
			}
		}

		if (err_trainid.GetCount()>0)
		{
			CString text;
			text.Format("存在 %d 趟新增列车来源不是计划台,请继续等待：", err_trainid.GetCount());
			
			CDlgConfirm dlg(text, err_trainid, this);
			dlg.DoModal();
			
			return;
		}
	}

	GpcLogTrainMessage(0, 0, "==================本台接收 邻台计划 开始======================");

	if(tdms_ctc_alarm.GetCount()>0)
	{
		CString text="请注意,接收邻台计划存在如下问题,请确认:";
		CDlgConfirm dlg(text, tdms_ctc_alarm, this);
		dlg.DoModal();
	}

	CStringArray list_train, list_source;
	CWordArray list_type;

	// D事件处理12: 收邻台计划
	GPCENTITY eventEntity;
	if(gpcdef.GetEntityInfo(136,0,0,eventEntity))
	{
		MsgEventData data;
		data.nSrcEntityID = GetLocalEntityID();
		data.tmEventTime = time(NULL);
		data.nEventRank = NORMAL_EVENT;

		data.nEventID =  EVENT_GPC_RECV_NEIBPLAN;
		data.SetKey(KeyConId, gpcdef.GetCurrentSectNo());
		int sel=((CComboBox *)GetDlgItem(IDC_CBO_NEIB_LIST))->GetCurSel();
		int NeibSection=((CComboBox *)GetDlgItem(IDC_CBO_NEIB_LIST))->GetItemData(sel);
		data.SetKey(KeyNeibConId, NeibSection);

		SendBMsg(&data, eventEntity.nEntityId);
	}

	CStringArray not_add_trainid;
	for(int i=1;i<m_grid.GetRowCount();i++)
	{
		if(!m_grid.IsRowSelected(i))
			continue;

		CString strOper=m_grid.GetCell(i,0)->GetText();
		CString strTrainId=m_grid.GetCell(i,2)->GetText();
		GpcLogTrainMessage(0, 0, "邻台处理:%s %s", strOper, strTrainId);
		if(strOper=="忽略")
			continue;

		if(strOper=="新增")
		{
			if(GetTGFunctionConfig()->IsNeibPlanNotAddNewTrain())
			{
				not_add_trainid.Add(strTrainId);
				continue;
			}
			else if(GetTGFunctionConfig()->IsNeibPlanNotAddNewTrainIFSourceNotShift())
			{
				CString strSource=m_grid.GetCell(i,5)->GetText();
				if(strSource!="班计划" && strSource!="日班计划")
				{
					not_add_trainid.Add(strTrainId);
					continue;
				}
			}
		}

		if(m_grid.GetCell(i,2)->GetData()!=0) 
		{
			TRAIN_INDEX trainindex=m_grid.GetCell(i,1)->GetData();
			CTG_SingleTrain *ptrain=GetFromTmpMemoryTrain(trainindex);
			if(ptrain)
			{
				if((((CButton *)GetDlgItem(IDC_CHK_PASSENGER))->GetCheck()==BST_UNCHECKED)
					&& ptrain->IsPassengerTrain())
					continue;
				if((((CButton *)GetDlgItem(IDC_CHK_FREIGHT))->GetCheck()==BST_UNCHECKED)
					&& !ptrain->IsPassengerTrain())
					continue;

				CTG_SingleTrain newtrain;
				newtrain=*ptrain;
				CTG_TrainRecord *firstrec=newtrain.GetTrainRecord(0);
				strncpy(newtrain.GetTrainInfo()->m_data.train_name, firstrec->GetDepartTID(), sizeof(TRAIN_NO)); // 用于临时存放操作,现在恢复
				STATION_NO firstation=NULL_STATION;
				
				CTG_SingleTrain *prev=NULL;
				if (newtrain.m_nCurrConTrainIndex>0)
				{
					prev=m_pSourceData->GetSingleTrain(newtrain.m_nCurrConTrainIndex);
					newtrain.SetTrainIndex(newtrain.m_nCurrConTrainIndex);
				}
				if(prev)
				{
					GpcLogTrainMessage(0, 0, "邻台计划:找到本台车次 %s ID=%u OPMSID=%u depart %d stalist %s", 
							prev->GetTrainID(), prev->GetTrainIndex(), prev->GetShiftTrainIndex(),
							prev->GetRecordList()->m_listRecords[0].m_data.depart, prev->toString());
				}

				if(ptrain->IsDeletedObject())
				{
					continue;
				}
				else
				{
					if(strOper=="新增")
					{
						CString strSource=m_grid.GetCell(i,5)->GetText();
						list_train.Add(strTrainId);
						list_source.Add(strSource);
						if(ptrain->IsPassengerTrain())
							list_type.Add(1);
						else
							list_type.Add(0);
					}
					GpcLogTrainMessage(0, 0, "邻台计划:判断车次 %s 在本台, 是增加还是更新", newtrain.GetTrainID());

					int update_or_add = 0;
					BOOL IsDelOld = FALSE;  //yuzhu
					if (prev)
					{
						GpcLogTrainMessage(0, 0, "邻台计划:老车次 %s ID=%u OPMSID=%u depart %d stalist %s", 
							prev->GetTrainID(), prev->GetTrainIndex(), prev->GetShiftTrainIndex(),
							prev->GetRecordList()->m_listRecords[0].m_data.depart, prev->toString());
						
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
									if(j>0){										
										IsDelOld = TRUE;//yuzhu 20100916
									}
									break;
								}
							}
						}
					
						GpcLogTrainMessage(0, 0, "邻台计划:比较本台列车 %s ID=%d 结果IsDelOld=%d update_or_add=%d ",
							prev->GetTrainID(),prev->GetTrainIndex(), IsDelOld, update_or_add);
					}

					if (update_or_add)
					{
						bool bUpdateTrainRelation=false;
						int nAccpetMode=GetTGFunctionConfig()->GetNeibplanAcceptRelationMode();
						if(nAccpetMode==1)
						{
							bUpdateTrainRelation=true;
							GpcLogTrainMessage(0, 0, "邻台计划:更新交路");
						}
						else
						{
							bUpdateTrainRelation=false;
							GpcLogTrainMessage(0, 0, "邻台计划:不更新交路");
						}
						newtrain.SetTrainIndex(prev->GetTrainIndex());
						if(prev->HasActualRecord()>=0)
						{
							GpcLogTrainMessage(0, 0, "车次 %s(%d) 存在实际点,更新失败",
												newtrain.GetTrainID(), 
												newtrain.GetTrainIndex());
						}
						else
						{
							CTG_TrainRecordList * old = prev->GetRecordList();
							CTG_TrainRecordList * newr = newtrain.GetRecordList();
							if (prev->GetTrainRecordCount() == newtrain.GetTrainRecordCount())
							{
								for (int j=0; j<prev->GetTrainRecordCount(); j++)
								{
									if(old->m_listRecords[j].m_data.send_plan_time>0)
										newr->m_listRecords[j].m_data.send_plan_time = old->m_listRecords[j].m_data.send_plan_time;
								}
							}
							
							bool bMoveNextTrain=false;
							if(m_grid.IsColSetCheck(i, 6))
								bMoveNextTrain=true;
							else
								bMoveNextTrain=false;
							
							m_pTgMMI->UpdateTrain_Directly(newtrain, bUpdateTrainRelation, bMoveNextTrain);

							GpcLogTrainMessage(0, 0, "更新车次 %s ID=%u OPMSID=%u depart %d stalist %s",
												newtrain.GetTrainID(), 
												newtrain.GetTrainIndex(), 
												newtrain.GetShiftTrainIndex(),
												newtrain.GetRecordList()->m_listRecords[0].m_data.depart, 
												newtrain.toString());
						}
					}
					else
					{
						if (prev)
						{
							TRAIN_INDEX temindex1 = newtrain.GetShiftTrainIndex();
							TRAIN_INDEX temindex2 = prev->GetShiftTrainIndex();
							if ((temindex1>0)&&(temindex2 >0)&&(temindex1 == temindex2))
								IsDelOld =TRUE;

							if(prev->HasActualRecord()>=0)
							{
								IsDelOld = FALSE;

								GpcLogTrainMessage(0, 0, "邻台计划:拒绝删除车次 %s 因为该车有实际点 OPMSID=%u depart %d staList %s",
										prev->GetTrainID(),
										prev->GetShiftTrainIndex(), 
										prev->GetRecordList()->m_listRecords[0].m_data.depart, 
										prev->toString());
							}
							
							for (int i=0;i<prev->GetTrainRecordCount();i++)
							{
								CTG_TrainRecord *rec = prev->GetTrainRecordByRecIndex(i);
								if (rec->m_data.send_plan_time>0)
								{
									IsDelOld = FALSE;
									GpcLogTrainMessage(0, 0, "邻台计划:拒绝删除车次 %s 因为该车有下计划的点 OPMSID=%u depart %d staList %s",
										prev->GetTrainID(),
										prev->GetShiftTrainIndex(), 
										prev->GetRecordList()->m_listRecords[0].m_data.depart, 
										prev->toString());
									break;
								}
							}

							if(IsDelOld == TRUE)
							{					
								GpcLogTrainMessage(0, 0, "邻台计划:删除车次 %s OPMSID=%u depart %d staList %s",
									prev->GetTrainID(),
									prev->GetShiftTrainIndex(), 
									prev->GetRecordList()->m_listRecords[0].m_data.depart, 
									prev->toString());

								m_pTgMMI->DeleteTrain_Directly(prev->GetTrainIndex());
								prev = NULL;

								m_pTgMMI->AddTrain_Directly(newtrain);

								GpcLogTrainMessage(0, 0, "邻台计划:增加车次 %s index %u OPMSID=%u depart %d staList %s",
									newtrain.GetTrainID(), 
									newtrain.GetTrainIndex(), 
									newtrain.GetShiftTrainIndex(), 
									newtrain.GetRecordList()->m_listRecords[0].m_data.depart, 
									newtrain.toString());
							}
						}
						else
						{
							m_pTgMMI->AddTrain_Directly(newtrain);

							GpcLogTrainMessage(0, 0, "邻台计划:增加车次 %s index %u OPMSID=%u depart %d staList %s",
								newtrain.GetTrainID(), newtrain.GetTrainIndex(), newtrain.GetShiftTrainIndex(), 
								newtrain.GetRecordList()->m_listRecords[0].m_data.depart, newtrain.toString());
						}
					}
				}
			}
		}
	}
	m_pTgMMI->UpdateTGFromData();   // 2010-9-27济南增加--立即更新界面
	GpcLogTrainMessage(0, 0, "==================本台接收 邻台计划 结束=====================");
	m_pDoc->OpenWaitWindow(NULL);
	
	CStringArray list;
	for(int i=0; i<list_train.GetCount(); i++)
	{
		CString tmp;
		tmp.Format("按照 %s 新增列车 %s", list_source.GetAt(i), list_train.GetAt(i));
		GpcLogTrainMessage(0, 0, "邻台计划:%s", tmp);
		
		// 2017-8-25该模式下,默认只能从“班计划”加车或者“单点车”,"班计划"其实也是“日班计划”,不需要报警
		if(GetTGFunctionConfig()->IsNeibPlanAddTrainOnlyByShift())
		{
			if(list_source.GetAt(i)=="班计划")
			{
				continue;
			}
		}

		if((list_source.GetAt(i)!="日班计划") && (list_type.GetAt(i)==1))
		{
			list.Add(tmp);
		}
	}
	if(list.GetCount()>0)
	{
		CString text="请注意,如下客车不是按照日班计划新增,请确认股道和营业:";
		CDlgConfirm dlg(text, list, this);
		dlg.DoModal();
	}

	if(GetTGFunctionConfig()->IsNeibPlanNotAddNewTrain())
	{
		if(not_add_trainid.GetCount()>0)
		{
			CDlgConfirm dlg("请注意,如下车次没有通过邻台新增,请人工处理:", not_add_trainid, this);
			dlg.DoModal();
		}
	}
	else if(GetTGFunctionConfig()->IsNeibPlanNotAddNewTrainIFSourceNotShift())
	{
		if(not_add_trainid.GetCount()>0)
		{
			CDlgConfirm dlg("请注意,如下车次来源不是日班计划,没有新增,请人工处理:", not_add_trainid, this);
			dlg.DoModal();
		}
	}
	
	
	CDialog::OnOK();
}

void CDlgNeibTrainList::OnGridSelectChangeCheck(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_GRIDVIEW *pv=(NM_GRIDVIEW *)pNMHDR;
	if(pv->iColumn!=0)
		return;

	if (pv->iRow==0)
		return;

	BOOL b=m_grid.IsRowSelected(pv->iRow);
	if(b)
		GpcLogTrainMessage(0, 0, "[调度员操作] 选中列车:%s (index=%s)", 
			m_grid.GetCell(pv->iRow,2)->GetText(),
			m_grid.GetCell(pv->iRow,1)->GetText());
	else
		GpcLogTrainMessage(0, 0, "[调度员操作] 未选中列车:%s (index=%s)", 
			m_grid.GetCell(pv->iRow,2)->GetText(),
			m_grid.GetCell(pv->iRow,1)->GetText());
}

void CDlgNeibTrainList::OnGridSelectChange(NMHDR* pNMHDR, LRESULT* pResult) 
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

	BOOL b=m_grid.IsRowSelected(pv->iRow);
	if(b)
		GpcLogTrainMessage(0, 0, "调度员操作前状态 列车已选中:%s (index=%s)", 
		m_grid.GetCell(pv->iRow,2)->GetText(),
		m_grid.GetCell(pv->iRow,1)->GetText());
	else
		GpcLogTrainMessage(0, 0, "调度员操作前状态 列车未选中:%s (index=%s)", 
		m_grid.GetCell(pv->iRow,2)->GetText(),
		m_grid.GetCell(pv->iRow,1)->GetText());

	if(pv->iRow>=1)
	{
		if(m_nCurrentSel!=pv->iRow)
		{
			m_nCurrentSel=pv->iRow;
		}
		if(m_grid.GetColumnByID(500)==pv->iColumn)
		{
			// 变更径路
			TRAIN_INDEX trainindex=m_grid.GetCell(pv->iRow,1)->GetData();
			CTG_SingleTrain *ptrain=GetFromTmpMemoryTrain(trainindex);
			if(!ptrain)
				return;

			bool bChangeRoute=true;
			if(ptrain->IsPassengerTrain())
				bChangeRoute=false;

			//太原 added 
			if(GetTGFunctionConfig()->IsCheckRouteforCrhAndImp())
			{
				TG_TrainInfo  traininfo=ptrain->GetTrainInfo()->m_data; 
				if(ptrain->IsCRHTrain() || traininfo.super_rank>0 )
				{
					if(AfxMessageBox("确认对此动车/重点列车进行径路变更么？",MB_YESNO | MB_DEFBUTTON1 |MB_ICONQUESTION) != IDYES  )
						return;
					bChangeRoute=true;
				}
			}
			//end
			if(bChangeRoute)
			{
				CTG_ScheduleInfo schdinfo;
				m_pData->GetScheduleInfo(schdinfo); 
				CChangeTrainRoute dlg(schdinfo.m_nSectionID,ptrain->GetTrainRecord(0)->GetStationNo(),1,this);  
				if(dlg.DoModal()==IDOK)
				{
					if(dlg.m_nRouteNo>0)
					{
						CTG_SingleTrain newtrain=*ptrain;
						if(m_pHolder->GenTrain_ChangeRoute(newtrain,schdinfo,dlg.m_nRouteNo))
						{
							if(GetTGFunctionConfig()->IsEnableNewCzh())
							{
								newtrain.SetShiftTrainIndex(0);// 变更径路，去掉shift_index
								newtrain.SetFromShift(FALSE);
								newtrain.SetPrevTrainIndex(0);
								newtrain.SetPrevShiftTrainIndex(0);
								newtrain.SetPrevBasicTrainIndex(0);
							}
							CTGAddTrainLogic::AutoChangeTrainID(&newtrain, schdinfo.m_nSectionID);
							GpcLogTrainMessage(0, 0, "调度员修改列车:%s 运行径路按照 passway_id %d", newtrain.GetTrainID(), dlg.m_nRouteNo);
							
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
							// 邻台传递机车号
							m_pHolder->PassTrainEngineNo(newtrain, ptrain);
							newtrain.SetTrainStartEndFlag();
							m_pData->UpdateTrain(schdinfo,newtrain,SYCFLAG_NULL); 
							RefreshTrain(newtrain.GetTrainIndex());
						}
					}
				}
			}
			else
			{
				CString str;
				str.Format("请注意, 列车 %s 是客车,系统不支持修改径路!", ptrain->GetTrainID());
				AfxMessageBox(str);
			}
		}
	}

}

void  CDlgNeibTrainList::GetRecvCountRate(int &recved,int &tatol)
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

void CDlgNeibTrainList::RequestNewYLFromOPMS()
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
		GpcLogTrainMessage(0, 0, "向OPMS获取: shift_index %d 列车详细信息", m_plan_inform.trains[i].GTID);
	}
	if(idcnt>0)
	{
		long eid=GetSpecialEntityID(SERVER_TYPE_TDCOMM); 
		if(DB_GetEntityStatus(eid)!=1)
		{
			MessageBox("请注意:\n\t本调度终端和TD通信服务器连接中断, 不能从TDMS获取列车详细信息, 请确认新增列车营业信息!!", "提示", MB_OK|MB_ICONWARNING);
			m_pDoc->OpenWaitWindow(NULL);
			return;
		}

		TD_GenRequestMessage_8_NEWYL(token,content,m_schd_info.m_nSectionID,idcnt,ids);
		
		TGSendStringMessage(token,content,eid);

		//GetDlgItem(IDOK)->EnableWindow(FALSE);
		m_pDoc->OpenWaitWindow("请稍候,正在获取车次详细信息...",NULL); 
	}

}

void CDlgNeibTrainList::OnDestroy()
{
	UnRegisterMsgProcHandleWnd(GetSafeHwnd(),MSGID_MsgStringMessage);
	m_pDoc->OpenWaitWindow(NULL); 
	CDialog::OnDestroy();
}

void CDlgNeibTrainList::OnBnClickedReq()
{
	tdms_ctc_alarm.RemoveAll();
	if((((CButton *)GetDlgItem(IDC_CHK_PASSENGER))->GetCheck()==BST_UNCHECKED)
	  && (((CButton *)GetDlgItem(IDC_CHK_FREIGHT))->GetCheck()==BST_UNCHECKED))
	{
		MessageBox("你没有选中客车或者货车,请选中客车或者货车","提示");
		return;
	}

	int sel=((CComboBox *)GetDlgItem(IDC_CBO_NEIB_LIST))->GetCurSel();
	if(sel < 0)
		return;

	int section=((CComboBox *)GetDlgItem(IDC_CBO_NEIB_LIST))->GetItemData(sel);

	GPCENTITY entityInfo;
	if(!gpcdef.GetEntityInfo(101, SERVER_TYPE_TG, section, entityInfo)) 
	{
		CString text;
		text.Format("无法获取 %d号 srv_tg 的实体号", section);
		AfxMessageBox(text);
		return; 
	}

	if(DB_GetEntityStatus(entityInfo.nEntityId)!=1)
	{
		if(IDYES==MessageBox("请注意:\n\t邻台tg_svr服务器和本调度终端连接中断, 不能进行邻台交互,你确认退出吗", "提示", MB_YESNO|MB_ICONWARNING))
		{
			return;
		}
	}

	//台间站数目,台间站列表
	nMiddleStaNum=sizeof(nMiddleStaList)/sizeof(nMiddleStaList[0]);
	nMiddleStaNum=GetTGFunctionConfig()->GetNeibFJKStations(m_schd_info.m_nSectionID,section,nMiddleStaList,nMiddleStaNum);
	
	if (!nMiddleStaNum)
	{
		MessageBox("无法获取台间站列表,请检查配置","提示");
		return;
	}
	CString str, stationlist;
	for(int s=0; s<nMiddleStaNum; s++)
	{
		if(s==0)
			stationlist.Format("%d", nMiddleStaList[0]);
		else
		{
			str.Format(",%d", nMiddleStaList[s]);
			stationlist+=str;
		}
	}

	CComboBox *pComboBox=(CComboBox *)GetDlgItem(IDC_CBO_STARTTIME2);
	CString tmStart;
	pComboBox->GetLBText(((CComboBox *)GetDlgItem(IDC_CBO_STARTTIME2))->GetCurSel(),tmStart);
	int range=atoi(tmStart)*3600;

	m_pData->DeleteAllData(m_schd_info,SYCFLAG_NULL);
	m_pOtherData->DeleteAllData(m_schd_info,SYCFLAG_NULL);

	CTG_ScheduleInfo info;
	info.m_nType=CTG_ScheduleInfo::SCHEDULE_TYPE_WORK;
	info.m_nSectionID=section;
	info.m_tStartTime=time(NULL);
	info.m_tEndTime=info.m_tStartTime+range;

	m_plan_inform.COUNT=0;
	m_plan_inform.STTM=info.m_tStartTime;
	m_plan_inform.ENDTM=info.m_tEndTime;
	if(m_pWintgNewServerWork==NULL)
	{
		m_pWintgNewServerWork=new CWintgNewServer;
	}
	m_pWintgNewServerWork->Init(section,SERVER_TYPE_TG);
	info.m_nType=CTG_ScheduleInfo::SCHEDULE_TYPE_WORK_REQ; 
	
	CTime btm(info.m_tStartTime), etm(info.m_tEndTime);
	GpcLogTrainMessage(0, 0, "邻台计划:向邻台(SECTION=%d)服务器请求 %s至%s 包含车站:%s的运行线",
		section, 
		btm.Format("%Y年%m月%d日%H:%M:%S"),
		etm.Format("%Y年%m月%d日%H:%M:%S"),
		stationlist);

	m_mapTrains.clear();
	
	m_nStatus=0;
	m_LocalScheduleData.SetScheduleInfo(info);
	int neib_station[2]={nMiddleStaList[m_nStatus], 0};
	m_LocalScheduleData.SetStationList(neib_station, 1);
	m_LocalScheduleData.SetCommInterface(m_pWintgNewServerWork);
	m_LocalScheduleData.ReloadData();
	GpcLogTrainMessage(0, 0, "邻台计划:向服务器请求 包含车站:%d的运行线", nMiddleStaList[0]);
	
	m_pDoc->OpenWaitWindow("请稍候，正在获取邻台计划..."); 
	m_nStatus=1;
}

BOOL CDlgNeibTrainList::ProcessShiftInfoMsgFromOPMS(const char *token,const char *content)
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


BOOL CDlgNeibTrainList::ProcessTrainMsgFromOPMS(const char *token,const char *content)
{
	if(!m_pData)
		return FALSE;

	CTG_ScheduleInfo schdinfo;
	m_pData->GetScheduleInfo(schdinfo);

	//GpcLogTrainMessage(0, 0, "%s", content);
	int flag=0;
	// 修改原来“出错直接返回FALSE" 
	CString  errinfo;
	do
	{
		CTG_SingleTrain train;
		int mode=GetTGFunctionConfig()->GetNeibplanAcceptRelationMode();
		if(!TD_LoadTrainFromXML(schdinfo,m_pData,train,content,token,NULL,NULL,&flag,0,0,mode,tdms_ctc_alarm))
		{
			errinfo="接收到的XML格式有误!";
			break;
		}
		if(train.GetTrainIndex()==NULL_TRAININDEX) // 不存在，不能添加，只能更新
		{
			errinfo.Format("%s:接收到的车次不是申请的范围! GTID=%u,找不到对应车次!",train.GetTrainID(),train.GetShiftTrainIndex());
			break;
		}
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
				CTG_ScheduleInfo schdinfo;
				m_pData->GetScheduleInfo(schdinfo);
				//m_pData->DeleteTrain(schdinfo,train.GetTrainIndex(),SYCFLAG_NULL);//yuzhu OPMS发送删除消息
			}
		}
		else
		{
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
			{
				errinfo.Format("%s:接收到的车次不是申请的范围! GTID=%u",train.GetTrainID(),train.GetShiftTrainIndex());
				break;
			}
			CTG_SingleTrain *pold=m_pData->GetSingleTrain(train.GetTrainIndex());
			if(!pold)
			{
				errinfo.Format("%s:内部错误,找不到对应车次!",train.GetTrainID());
				break;
			}
			// 用原来车次的首发时间取代日班计划中的
			CTG_TrainRecord *pfirstrec_old=pold->GetTrainRecord(0);
			CTG_TrainRecord *pfirstrec_opms=train.GetTrainRecord(0);
			if(!pfirstrec_old || !pfirstrec_opms)
			{
				errinfo.Format("%s:接收到的数据错误,记录数为0!",train.GetTrainID());
				break;
			}
			if(pfirstrec_old->GetStationNo()!= pfirstrec_opms->GetStationNo())
			{
				/*errinfo.Format("%s:接收到的数据首站不一致，原站=%d,接收站=%d!",train.GetTrainID(),pfirstrec_old->GetStationNo(),pfirstrec_opms->GetStationNo());
				break;*/
				GpcLogTrainMessage(0, 0,"%s:接收到的数据首站不一致，原站=%d,接收站=%d!",train.GetTrainID(),pfirstrec_old->GetStationNo(),pfirstrec_opms->GetStationNo());
			}

			/////////////////////////////////////////
			CTG_SingleTrain othertrain;
			BOOL  newflag=TRUE;
			if(m_pSourceData->GetSingleTrain(train.GetTrainIndex())) // 原来已经在实际图中存在
				newflag=FALSE;
			if(!m_pHolder->GenTrain_OPMS(train,othertrain,m_schd_info,newflag))
			{
				errinfo.Format("%s:无法生成车次,GenTrain_OPMS失败!",train.GetTrainID());
				break;
			}

			if(GetTGFunctionConfig()->IsEnableOPMSNodeIndex() || GetTGFunctionConfig()->IsCalShiftSchdExitEntry())
			{
				CTG_TrainRecord *pfirst_rec=train.GetTrainRecord(0);
				CTG_TrainRecord *plast_rec=train.GetTrainRecord(train.GetTrainRecordCount()-1);
				
				// 保存第一个节点接入口、最后一个节点的交出口
				int prev_entry=255,prev_exit=255;
				if(pfirst_rec)
					prev_entry=pfirst_rec->GetEntry();
				if(plast_rec)
					prev_exit=plast_rec->GetExit();

				// 保存字段 speed_limit，用于判断是否使用TG生成的第一个节点入口、最后一个节点出口
				long first_speed_limit=pfirst_rec->m_data.speed_limit;
				long last_speed_limit=plast_rec->m_data.speed_limit;
				pfirst_rec->m_data.speed_limit=0;
				plast_rec->m_data.speed_limit=0;

				if(m_pTgMMI==NULL)
					continue;
				CTgX* pTG=m_pTgMMI->GetTGCtrl();
				if(pTG == NULL)
					continue;
				GpcLogTrainMessage(0, 0, "ProcessTrainMsgFromOPMS 画图控件开始计算出入口");
				if(!m_pHolder->TGOCXSetTrainExitEntry(train, pTG, 255, 255))
				{
					CString str;
					str.Format("车次:%s 径路计算失败,请确认列车径路", othertrain.GetTrainID());
					tdms_ctc_alarm.Add(str);
				}
				pfirst_rec=train.GetTrainRecord(0);
				plast_rec=train.GetTrainRecord(train.GetTrainRecordCount()-1);
				
				if(pfirst_rec->m_data.speed_limit==1)
				{
					GpcLogTrainMessage(0,0,"使用TG运算的入口 %d", pfirst_rec->m_data.entry);
				}
				else
				{
					if(IS_VALID_ENTRY(prev_entry) && pfirst_rec)
						pfirst_rec->m_data.entry=prev_entry;
				}
				if(plast_rec->m_data.speed_limit==1)
				{
					GpcLogTrainMessage(0,0,"使用TG运算的出口 %d", plast_rec->m_data.exit);
				}
				else
				{
					if(IS_VALID_ENTRY(prev_exit) && plast_rec)
						plast_rec->m_data.exit=prev_exit;
				}

				// 恢复该字段 speed_limit
				pfirst_rec->m_data.speed_limit=first_speed_limit;
				plast_rec->m_data.speed_limit=last_speed_limit;
			}

			CTG_TrainRecord *pfirstrec = train.GetTrainRecord(0);
			// 根据原有时间平移
			long time_off = pfirstrec_old->m_data.depart -  pfirstrec->m_data.depart;
			//if (abs(time_off) > 8*3600) 
			//		return FALSE;

			pfirstrec->m_data.arrive=pfirstrec_old->m_data.arrive;
			pfirstrec->m_data.depart=pfirstrec_old->m_data.depart;

			train.OffsetScheduleTime(1, time_off, FALSE, 1);
			//
			m_nDataChanged=1;
			if(flag!=0)
				RequestShiftInfoFromOPMS(train.GetShiftTrainIndex()); 
			train.SetTrainIndex(pold->GetTrainIndex());

			CTG_ScheduleInfo schd;
			m_pData->GetScheduleInfo(schd);

			STATION_NO firstation = NULL_STATION;
			if (pfirstrec)
                firstation = pfirstrec->GetStationNo();

			m_pHolder->CheckTrainNo(train,tdms_ctc_alarm);
			CTgDocument::SetTrainReverseFlag(&train);
			CTgDocument::SetTrainFromToConid(schdinfo.m_nSectionID, &train);

			GpcLogTrainMessage(0, 0, "train %s train_index %u shift_index %u 邻台计划 日班计划更新临时生成邻台列车", train.GetTrainID(), train.GetTrainIndex(), train.GetShiftTrainIndex());
			if(m_pData->UpdateTrain(schd,train,SYCFLAG_NULL)) 
			{
				m_pData->SetTrainFromOPMS(schd,train);
			}
			// 此处未考虑拆分车次，邻台接入只更新第一段
		}
	}while(0);

	int recved,total;
	GetRecvCountRate(recved,total);
	if(!errinfo.IsEmpty())
	{
		GpcLogTrainMessage(0,0,"接收邻台计划OPMS信息出错,%s",errinfo);
	}
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
		RefreshTrainList();

		m_pDoc->OpenWaitWindow(waitcap); 
	}
	return TRUE;
}

void CDlgNeibTrainList::ProcessTrainMsgFromOPMS_NEW(const char *token,const char *content)
{
	if(!m_pData || !m_pOtherData)
		return;

	CTG_ScheduleInfo schdinfo;
	m_pData->GetScheduleInfo(schdinfo);
	int mode=GetTGFunctionConfig()->GetNeibplanAcceptRelationMode();
	GpcLogTrainMessage(0, 0, "============================处理邻台列车===============================");
	CString errinfo;
	CTG_SingleTrain newtrain;
	if(!TD_LoadShiftScheduleTrainFromXML(schdinfo,newtrain,content,token,NULL,NULL,mode,tdms_ctc_alarm))
	{
		errinfo="接收到的XML格式有误!";
		GpcLogTrainMessage(0, 0, "邻台计划处理OPMS列车:%s", errinfo);
		return;
	}
	
	int i=0;
	for(;i<m_plan_inform.COUNT;i++)
	{
		if(m_plan_inform.trains[i].GTID==newtrain.GetShiftTrainIndex())
		{
			m_plan_inform.trains[i].InfoReceivedFlag=1;
			break;
		}
	}
	if(i>=m_plan_inform.COUNT)
	{
		errinfo.Format("接收到的车次 %s GTID=%u 不是申请的!",newtrain.GetTrainID(),newtrain.GetShiftTrainIndex());
		GpcLogTrainMessage(0, 0, "邻台计划处理OPMS列车:%s", errinfo);
		return;
	}
	
	CTG_TrainRecord *prec_opms=newtrain.GetTrainRecord(0);
	if(NULL==prec_opms)
	{
		errinfo.Format("%s:接收到的数据错误,记录数为0!",newtrain.GetTrainID());
		GpcLogTrainMessage(0, 0, "邻台计划处理OPMS列车:%s", errinfo);
		return;
	}

	/////////////////////////////////////////
	CTgX* pTgX=m_pTgMMI->GetTGCtrl();
	if(pTgX!=NULL)
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
	bool bOldSplitTrain=false;
	std::vector<CTG_SingleTrain> othertrain_list;
	if(!m_pHolder->GenTrain_OPMSx(newtrain, othertrain_list, m_schd_info, TRUE, bOldSplitTrain))
	{
		errinfo.Format("%s:无法生成车次失败!",newtrain.GetTrainID());
		GpcLogTrainMessage(0, 0, "邻台计划处理OPMS列车:%s", errinfo);
		return;
	}

	// 分割后续列车,仅有金丽温模式需要,老的分割无法加车成功不需要计算后续出入后
	//if(bOldSplitTrain)  
	//{
	//	for(int i=0; i<othertrain_list.size(); i++) // 对于分拆的列车
	//	{
	//		CTG_SingleTrain& othertrain=othertrain_list[i];
	//		if(othertrain.GetTrainRecordCount()>0)
	//		{
	//			if(GetTGFunctionConfig()->IsCalShiftSchdExitEntry())
	//			{
	//				GpcLogTrainMessage(0, 0, "ProcessTrainMsgFromOPMS_NEW 画图控件开始计算出入口");
	//			
	//				int first_entry=255, last_exit=255;
	//				// 保存首站接入口、未站交出口
	//				CTG_TrainRecord *pfirst_rec=othertrain.GetTrainFirstRecord();
	//				CTG_TrainRecord *plast_rec=othertrain.GetTrainLastRecord();
	//				if(pfirst_rec)
	//					first_entry=pfirst_rec->GetEntry();
	//				if(plast_rec)
	//					last_exit=plast_rec->GetExit();
	//				CWorkScheduleHolder::TGOCXSetEntryExit(othertrain, pTgX, first_entry, last_exit);
	//			}
	//		}
	//	}
	//}

	othertrain_list.push_back(newtrain);
	for(int i=0; i<othertrain_list.size(); i++)
	{
		CTG_SingleTrain train=othertrain_list[i];
		CTG_TrainRecord *prec=train.GetTrainRecord(0);
		if(prec==NULL)
		{
			continue;
		}
		m_pHolder->CheckTrainNo(train,tdms_ctc_alarm);
		CTG_TrainScheduleBase *pTmpData=NULL;
		CTG_SingleTrain *pold=GetSingleTrainByShiftIndex(train.GetShiftTrainIndex(), prec->m_data.station, &pTmpData);
		if(pTmpData==NULL || pold==NULL)
		{
			errinfo.Format("接收到的车次 %s GTID=%u,在邻台处理临时内存找不到对应车次!",train.GetTrainID(),train.GetShiftTrainIndex());
			GpcLogTrainMessage(0, 0, "邻台计划处理OPMS列车:%s", errinfo);
			continue;
		}
		
		train.SetTrainIndex(pold->GetTrainIndex());

		// 用原来车次的首发时间取代日班计划中的
		CTG_TrainRecord *pfirstrec_old=pold->GetTrainRecord(0);
		CTG_TrainRecord *pfirstrec_opms=train.GetTrainRecord(0);
		if(!pfirstrec_old || !pfirstrec_opms)
		{
			errinfo.Format("%s:接收到的数据错误,记录数为0!",train.GetTrainID());
			GpcLogTrainMessage(0, 0, "邻台计划处理OPMS列车:%s", errinfo);
			break; // return 的话等待对话框不消失
		}
		if(pfirstrec_old->GetStationNo()!= pfirstrec_opms->GetStationNo())
		{
			GpcLogTrainMessage(0, 0,"邻台计划处理OPMS列车:%s接收到的数据首站不一致,原站=%d,接收站=%d!",
				train.GetTrainID(),pfirstrec_old->GetStationNo(),pfirstrec_opms->GetStationNo());
			break; // return 的话等待对话框不消失
		}

		// 根据原有时间平移
		long time_off = pfirstrec_old->m_data.arrive -  pfirstrec_opms->m_data.arrive;
		pfirstrec_opms->m_data.arrive=pfirstrec_old->m_data.arrive;
		
		// 偏移本站发点及后续个站到发点 time_off
		train.OffsetScheduleTime(0, time_off, FALSE, 0);
		
		RequestShiftInfoFromOPMS(train.GetShiftTrainIndex()); 
		train.SetTrainIndex(pold->GetTrainIndex());

		CTG_ScheduleInfo schd;
		pTmpData->GetScheduleInfo(schd);

		CTgDocument::SetTrainReverseFlag(&train);
		CTgDocument::SetTrainFromToConid(schdinfo.m_nSectionID, &train);

		GpcLogTrainMessage(0, 0, "train %s train_index %u shift_index %u 邻台计划 日班计划更新临时生成邻台列车", 
			train.GetTrainID(), train.GetTrainIndex(), train.GetShiftTrainIndex());
		if(pTmpData->UpdateTrain(schd,train,SYCFLAG_NULL)) 
		{
			pTmpData->SetTrainFromOPMS(schd,train);
		}
	}
	
	int recved,total;
	GetRecvCountRate(recved,total);
	
	if(recved==total)
	{
		GetDlgItem(IDOK)->EnableWindow(TRUE);
		m_pDoc->OpenWaitWindow(NULL); 
		RefreshTrainList();
	}
	else
	{
		CString waitcap;
		waitcap.Format("正在获取车次详细信息,进度 %d/%d",recved, total);
		m_pDoc->OpenWaitWindow(waitcap);
		RefreshTrainList();
	}
	return;
}


void CDlgNeibTrainList::RequestShiftInfoFromOPMS(DWORD gtid)
{
	if(GetTGFunctionConfig()->IsEnableTDMS5_0()) //TD5.0
	{
		m_pDoc->RequestTrainExpand_KD_YL_CREW_FromOPMS(gtid, 4);
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

void CDlgNeibTrainList::RefreshTrain(int train_index)
{
	CTG_SingleTrain *ptrain=GetFromTmpMemoryTrain(train_index);
	if(!ptrain)
		return;

	for(int row=0; row<m_grid.GetRowCount(); row++)
	{
		if(train_index == m_grid.GetCell(row,1)->GetData())
		{
			break;
		}
	}
	if(row>=m_grid.GetRowCount())
		return;

	CString xml;
	BOOL selected=GetGridXml(ptrain, xml);
	m_grid.UpdateTrainInfo(row, xml);
	
	m_grid.SetRowSelected(row,selected); 
	m_grid.GetCell(row,1)->SetData(ptrain->GetTrainIndex()); 
	m_grid.GetCell(row,2)->SetData(1); 

	m_grid.AutoSizeColumns();
	m_grid.Refresh();	
}

void CDlgNeibTrainList::RefreshTrainList()
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

	// 按照台间站顺序排序显示
	if(GetTGFunctionConfig()->IsNeibPlanShowAcceptInfoOrder())
	{
		DisplayTrainAcceptOrder();
		return; 
	}

	// 保留原有邻台代码显示
	m_grid.DeleteNonFixedRows();
	CTG_TrainPtrGroup group;
	m_pData->FindAllTrains(group);

	for(int i=0;i<group.GetTrainCount();i++)
	{
		CTG_SingleTrain *ptrain=group.GetTrainPtr(i);
		if(!ptrain)
			continue;
			
		CString xml;
		BOOL selected=GetGridXml(ptrain, xml);
		
		CString cap;
		cap.Format("%u", ptrain->GetShiftTrainIndex()); 
		int row=m_grid.InsertTrainInfo(cap, xml);
		if(row>=0)
		{
			m_grid.SetRowSelected(row,selected); 
			m_grid.GetCell(row,1)->SetData(ptrain->GetTrainIndex());
			m_grid.GetCell(row,2)->SetData(1);
		}
		else
		{
			GpcLogTrainMessage(0,0,"邻台计划 显示错误 %s", xml);
			AfxMessageBox("邻台计划显示不完整清重新获取!");
			m_grid.DeleteNonFixedRows();
			break;
		}
	}

	CTG_TrainPtrGroup group1;
	m_pOtherData->FindAllTrains(group1);
	for(int i=0;i<group1.GetTrainCount();i++)
	{
		CTG_SingleTrain *ptrain=group1.GetTrainPtr(i);
		if(!ptrain)
			continue;
			
		CString xml;
		BOOL selected=GetGridXml(ptrain, xml);
		
		CString cap;
		cap.Format("%u", ptrain->GetShiftTrainIndex()); 
		int row=m_grid.InsertTrainInfo(cap, xml);
		if(row>=0)
		{
			m_grid.SetRowSelected(row,selected); 
			m_grid.GetCell(row,1)->SetData(ptrain->GetTrainIndex());
			m_grid.GetCell(row,2)->SetData(1);
		}
		else
		{
			GpcLogTrainMessage(0,0,"邻台计划 显示错误 %s", xml);
			AfxMessageBox("邻台计划显示不完整清重新获取!");
			m_grid.DeleteNonFixedRows();
			break;
		}
	}

	m_grid.AutoSizeColumns();
	m_grid.Refresh();
}


void CDlgNeibTrainList::OnTimer(UINT_PTR IDEvent)
{
	if(2002 == IDEvent)
	{
		if(m_nStatus>0)
		{
			if(m_LocalScheduleData.IsDataOK())
			{
				CTG_TrainPtrGroup group;
				m_LocalScheduleData.GetScheduleData()->FindTrainsByEndTm(group);
				for(int i=0;i<group.GetTrainCount();i++)
				{
					CTG_SingleTrain *ptrain=group.GetTrainPtr(i);
					if(ptrain)
					{
						TRAIN_INDEX train_index=ptrain->GetTrainIndex();
						std::map<TRAIN_INDEX, CTG_SingleTrain>::iterator iter = m_mapTrains.find(train_index);
						if (iter == m_mapTrains.end())
						{
							m_mapTrains[train_index] = *ptrain;
						}
					}
				}
				// 数据接收完毕,邻台处理
				if(m_nStatus>=nMiddleStaNum)
				{
					m_warning.RemoveAll();
					m_pDoc->OpenWaitWindow(NULL); 
					m_nStatus=-1;

					GenTrainsFromNeib();

					RefreshTrainList();   // 刷新到界面
					GetDlgItem(IDOK)->EnableWindow(TRUE);

					// 比较本台比邻台多出列车报警信息
					CStringArray m_warning_ex;
					m_warning_ex.RemoveAll();
					if (GetTGFunctionConfig()->IsEnableExactNeibPlanMode())
					{
						if(GetTGFunctionConfig()->IsCheckNeibTrains(m_schd_info.m_nSectionID))
						{
							FindNeibNotExistTrains(m_warning_ex);
						}
					}

					for(int i=0; i<m_warning.GetCount(); i++)
					{
						tdms_ctc_alarm.Add(m_warning.GetAt(i));
					}

					for(int i=0; i<m_warning_ex.GetCount(); i++)
					{
						tdms_ctc_alarm.Add(m_warning_ex.GetAt(i));
					}

					if (GetTGFunctionConfig()->IsShowColorNeibPlanTip())
					{
						CRichEditCtrl* pEdit=(CRichEditCtrl*)GetDlgItem(IDC_EDIT_WARNING);
						if(pEdit)
						{
							pEdit->SetReadOnly(TRUE);
							pEdit->SetWindowText("");
							CFont * pFont = new CFont; 
							if(pFont)
							{
								pFont->CreateFont(20, 0,0,0, FW_BOLD, 0,0,0, DEFAULT_CHARSET,
								OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY,
								DEFAULT_PITCH | FF_DONTCARE, "黑体");
								pEdit->SetFont(pFont);
								pFont->DeleteObject();
							}

							CString str_ex="";
							for(int i=0; i<m_warning_ex.GetCount(); i++)
							{
								str_ex+=m_warning_ex.GetAt(i);
								str_ex+="\r\n";
							}

							if(str_ex!="")
							{
								CHARFORMAT cf;
								cf.cbSize = sizeof(cf);
								pEdit->GetDefaultCharFormat(cf);  
								cf.cbSize		= sizeof(CHARFORMAT);
								cf.dwEffects	= 0;	// To disable CFE_AUTOCOLOR
								cf.crTextColor = RGB(255,0,0); 
								cf.dwMask = CFM_BOLD | CFM_COLOR; 
								long lineStart = pEdit->LineIndex(m_warning.GetCount());
								pEdit->SetSel(lineStart,-1);
								pEdit->SetSelectionCharFormat(cf);//设置颜色
								pEdit->ReplaceSel(str_ex);
							}
						
							CString str="";
							for(int i=0; i<m_warning.GetCount(); i++)
							{
								str+=m_warning.GetAt(i);
								str+="\r\n";
							}
							if(str!="")
							{
								CHARFORMAT cf;
								cf.cbSize = sizeof(cf);
								pEdit->GetDefaultCharFormat(cf);  
								cf.cbSize		= sizeof(CHARFORMAT);
								cf.dwEffects	= 0;	// To disable CFE_AUTOCOLOR
								cf.crTextColor = RGB(0,0,250); 
								cf.dwMask = CFM_BOLD | CFM_COLOR; 
								long lineStart = pEdit->LineIndex(pEdit->GetLineCount());
								pEdit->SetSel(lineStart,-1);
								pEdit->SetSelectionCharFormat(cf);//设置颜色

								pEdit->ReplaceSel(str);
							}
						}
					}
					else
					{
						CStringArray list;
						for(int i=0; i<m_warning.GetCount(); i++)
						{
							list.Add(m_warning.GetAt(i));
						}

						for(int i=0; i<m_warning_ex.GetCount(); i++)
						{
							list.Add(m_warning_ex.GetAt(i));
						}

						CString str;
						CEdit* pEdit=(CEdit*)GetDlgItem(IDC_EDIT_WARNING);
						for(int i=0; i<list.GetCount(); i++)
						{
							str+=list.GetAt(i);
							str+="\r\n";
						}
						if(pEdit)
							pEdit->SetWindowText(str);
					}

					RequestNewYLFromOPMS();   // 向OPMS请求
				}
				else
				{
					int neib_station[2]={nMiddleStaList[m_nStatus], 0};
					m_LocalScheduleData.SetStationList(neib_station, 1);
					m_LocalScheduleData.ReloadData();
					//m_nStatus=m_nStatus+1;
					GpcLogTrainMessage(0, 0, "邻台计划:向服务器请求 包含车站:%d的运行线", nMiddleStaList[m_nStatus]);
					m_nStatus=m_nStatus+1;
				}
			}
		}
	}
}

void CDlgNeibTrainList::SetTrainStartOrAccept(const CTG_SingleTrain* ptrain, CTG_SingleTrain& newtrain, int sectionid,bool accept_start_flag)
{
	if(ptrain==NULL)
		return;

	const CTG_TrainRecord*pOldFirst = ptrain->GetTrainRecordNoDelete_Const(0);
	CTG_TrainRecord*pNewFirst = newtrain.GetTrainRecordNoDelete(0);
	if(pOldFirst==NULL || pNewFirst==NULL)
		return;
	
	if(pOldFirst->m_data.station != pNewFirst->m_data.station)
		return;
	
	if(GetTGFunctionConfig()->IsNotThisSectionStation(sectionid,  pNewFirst->m_data.station) || accept_start_flag)
	{
		if(pOldFirst->IsStartRec())
		{
			TG_SetFlagStart(pNewFirst->m_data.flag);
			GpcLogTrainMessage(0,0,"邻台计划: 车站 %d 不属于本台 %d 管辖, %s 在邻台始发,修改本台为始发", 
				pNewFirst->m_data.station, sectionid, ptrain->GetTrainID());
		}
		else
		{
			TG_SetFlagAccept(pNewFirst->m_data.flag);
			GpcLogTrainMessage(0,0,"邻台计划: 车站 %d 不属于本台 %d 管辖, %s 在邻台接入,修改本台为接入", 
				pNewFirst->m_data.station, sectionid, ptrain->GetTrainID());
		}
	}
		
	return;
}

void CDlgNeibTrainList::GenTrainsFromNeib()
{
	if(!m_pData)
		return;
	CTG_ScheduleInfo info;
	m_pData->GetScheduleInfo(info);
	m_pData->DeleteAllData(info,SYCFLAG_NULL);

	m_LocalScheduleData.GetScheduleInfo(info);
	m_pHolder->GetNeibPlanEqualStation(m_schd_info.m_nSectionID, info.m_nSectionID);
	for(int m=0;m<2;m++)
	{
		std::map<TRAIN_INDEX, CTG_SingleTrain>::iterator it = m_mapTrains.begin();
		for(int i=0; it != m_mapTrains.end(); it++, i++)
		{
			CTG_SingleTrain *ptrain=&it->second;
			if(!ptrain)
				continue;

			if((((CButton *)GetDlgItem(IDC_CHK_PASSENGER))->GetCheck()==BST_UNCHECKED)
				&& ptrain->IsPassengerTrain())
				continue;
			if((((CButton *)GetDlgItem(IDC_CHK_FREIGHT))->GetCheck()==BST_UNCHECKED)
				&& !ptrain->IsPassengerTrain())
				continue;

			if(m==0 && ptrain->GetShiftTrainIndex()==NULL_TRAININDEX)
				continue;
			if(m==1 && ptrain->GetShiftTrainIndex()!=NULL_TRAININDEX)
				continue;
		
			int nNeibStaNo=0;
			int add_or_update = -1;
			CTG_SingleTrain newtrain;
			m_LocalScheduleData.GetScheduleInfo(info);
			// 沈阳模式			
			if(GetTGFunctionConfig()->IsSyNeighbourPlan(m_schd_info.m_nSectionID))
			{
				bool bFindNeibTrain=false;
				if(m_pHolder->GenTrain_NeibSchedule_SY(newtrain,
														*ptrain,
														nNeibStaNo,
														info.m_nSectionID, 
														&add_or_update,
														m_pData,
														m_warning))
				{
					bFindNeibTrain=true;
					
					SetTrainStartOrAccept(ptrain, newtrain, m_schd_info.m_nSectionID, true);
				}
				else if(m_pHolder->GenTrain_NeibSchedule_Reverse(newtrain,
																	*ptrain,
																	nNeibStaNo,
																	info.m_nSectionID, 
																	&add_or_update))
				{
					bFindNeibTrain=true;
				}

				if(bFindNeibTrain)
				{
					m_pData->GetScheduleInfo(info);

					m_pData->UpdateTrain(info,newtrain,SYCFLAG_NULL);
					GpcLogTrainMessage(0,0,"邻台计划 沈阳模式: 列车 %s,index=%d 符合邻台计划要求, 台间站 %d, 修改", 
						ptrain->GetTrainID(), ptrain->GetTrainIndex(), nNeibStaNo);

					if (0 == add_or_update)
						AddTrainIDToPlanInform(&newtrain);
				}
				else
				{
					GpcLogTrainMessage(0,0,"邻台计划 沈阳模式: 列车 %s,index=%d 不符合邻台计划要求, 忽略", ptrain->GetTrainID(), ptrain->GetTrainIndex());
				}
			}
			else
			{
				std::vector<CTG_SingleTrain> othertrain_list;
				if(m_pHolder->GenTrain_NeibSchedule_ConsiderTrainID(newtrain, othertrain_list,
																*ptrain,
																nNeibStaNo,
																info.m_nSectionID, 
																&add_or_update,
																m_pData,
																m_warning))
				{ 
					SetTrainStartOrAccept(ptrain, newtrain, m_schd_info.m_nSectionID, false);
					m_pData->GetScheduleInfo(info);
					m_pData->UpdateTrain(info,newtrain,SYCFLAG_NULL);

					GpcLogTrainMessage(0,0,"邻台计划:列车 %s,index=%d (邻台index=%d) 符合邻台计划要求, 台间站 %d, 修改", 
						ptrain->GetTrainID(), ptrain->m_nCurrConTrainIndex, ptrain->GetTrainIndex(), nNeibStaNo);

					if (0 == add_or_update && newtrain.m_strSource!="日班计划")
						AddTrainIDToPlanInform(&newtrain);

					for(int i=0; i<othertrain_list.size(); i++)
					{
						CTG_SingleTrain othertrain=othertrain_list[i];
						if(othertrain.m_nAddOrUpdate==0)
							AddTrainIDToPlanInform(&othertrain);
						othertrain.SetTrainIndex(NULL_TRAININDEX);
						m_pOtherData->GetScheduleInfo(info);
						m_pOtherData->AppendTrain(info,othertrain,SYCFLAG_NULL);
					}
				}
				else
				{
					GpcLogTrainMessage(0,0,"邻台计划,列车 %s,index=%d 不符合邻台计划要求, 忽略", ptrain->GetTrainID(), ptrain->GetTrainIndex());
				}
			}
		}
	}
}

BOOL CDlgNeibTrainList::GetGridXml(CTG_SingleTrain *ptrain, CString& xml)
{
	BOOL selected=TRUE;
	
	CTG_SingleTrain *prev_train=NULL;
	if (ptrain->m_nCurrConTrainIndex!=0)
		prev_train=m_pSourceData->GetSingleTrain(ptrain->m_nCurrConTrainIndex);

	const CTG_TrainRecord* prec=ptrain->GetTrainRecordByRecIndex(0);
	const CTG_TrainRecord* plast=ptrain->GetTrainRecordByRecIndex(ptrain->GetTrainRecordCount()-1);
	
	CString strPath;
	strPath.Format("<PATH>%s--%s</PATH>", gpcdef.GetStationName(prec->GetStationNo()), 
			gpcdef.GetStationName(plast->GetStationNo()));

	if(!prev_train || prev_train->GetTrainRecordCount()<=0)
	{
		CString tmp;
		xml="<ROOT>";
		tmp.Format("<GTID>%u</GTID>", ptrain->GetShiftTrainIndex());   // 按照无符号数,输出索引
		xml += tmp;
		tmp.Format("<TNO>%s</TNO>", ptrain->GetTrainID());
		xml += tmp;

		if(ptrain->m_strSource=="日班计划" || ptrain->m_strSource=="调整计划")
			tmp.Format("<SOURCE clr=\"1\">%s</SOURCE>", ptrain->m_strSource);
		else
			tmp.Format("<SOURCE clr=\"2\">%s</SOURCE>", ptrain->m_strSource);
		xml += tmp;

		CString strDate="错误", strStationName="未知";
		if(prec)
		{
			if(prec->IsStartRec())
			{
				CTime ctm(prec->GetDepartTime());
				strDate = ctm.Format("%d日 %H:%M:%S");
			}
			else
			{
				CTime ctm(prec->GetArriveTime());
				strDate = ctm.Format("%d日 %H:%M:%S");
			}
			strStationName=gpcdef.GetStationName(prec->GetStationNo());
		}
		tmp.Format("<STATION>%s</STATION>", strStationName);
		xml += tmp;
		tmp.Format("<DATE>%s</DATE>", strDate);
		xml += tmp;

		xml += strPath;
		
		tmp.Format("<STATION_LIST>--</STATION_LIST>");
		xml += tmp;
		
		if(prec->IsStartRec())
			tmp.Format("<ARRIVE_FLAG clr=\"1\">始发</ARRIVE_FLAG>");
		else
			tmp.Format("<ARRIVE_FLAG clr=\"1\">接入</ARRIVE_FLAG>");
		xml += tmp;

		if(plast->IsTerminalRec())
			tmp.Format("<DEPART_FLAG clr=\"1\">终到</DEPART_FLAG>");
		else
			tmp.Format("<DEPART_FLAG clr=\"1\">交出</DEPART_FLAG>");
		xml += tmp;
	
		tmp.Format("<STOP_PASS>--</STOP_PASS>");
		xml += tmp;
		
		tmp.Format("<WORK_TM>--</WORK_TM>");
		xml += tmp;

		tmp.Format("<ENGINE>--</ENGINE>");
		xml += tmp;

		tmp.Format("<WORK_SIDE>--</WORK_SIDE>");
		xml += tmp;
		
		tmp.Format("<TIME>--</TIME>");
		xml += tmp;
		
		tmp.Format("<GTID>%u</GTID>", ptrain->GetShiftTrainIndex());
		xml += tmp;

		tmp.Format("<OPER>%s</OPER>", "新增");
		xml += tmp;
		xml+="</ROOT>";
		
		selected=TRUE;
	}
	else
	{
		if(IsTrainChange(prev_train, ptrain, strPath, xml))
			selected=TRUE;
		else
			selected=FALSE;
	}

	return selected;
}

/*
<ROOT>
	<STATION_LIST></STATION_LIST>
	<TIME></TIME>
	<PATH></PATH>
	<ARRIVE_FLAG></ARRIVE_FLAG>
	<DEPART_FLAG></DEPART_FLAG>
	<STOP_PASS></STOP_PASS>
	<WORK_TM></WORK_TM>
	<WORK_SIDE></WORK_SIDE>
</ROOT>
*/
bool CDlgNeibTrainList::IsTrainChange(CTG_SingleTrain *srctrain, CTG_SingleTrain *desttrain, CString& strFirstEnd, CString& xml)
{
	CString tmp;
	
	CString strPath="";
	CString strStationList="";
	CString strArriveFlag="";
	CString strDepartFlag="";
	CString strStopFlag="";
	CString strWorkTime="";
	CString strWorkSide="";
	CString strStationTime="";
	CString strDate="错误";
	CString strFirstStName="未知";
	CString strEngine="";
	// 比较机车号
	CTG_TrainEngineList newEngineList=*srctrain->GetEngineList();
	CTG_TrainEngineList oldEngineList=*desttrain->GetEngineList();

	if(oldEngineList == newEngineList)
		strEngine="";
	else
		strEngine="变化";

	const int src_rec_count=srctrain->GetTrainRecordCount();
	const int des_rec_count=desttrain->GetTrainRecordCount();
	const int rec_count=(src_rec_count>=des_rec_count)?src_rec_count:des_rec_count;

	for(int rec_index=0; rec_index<rec_count; rec_index++)
	{
		const CTG_TrainRecord *prec=srctrain->GetTrainRecordByRecIndex_Const(rec_index);
		const CTG_TrainRecord *prec_dest=desttrain->GetTrainRecordByRecIndex_Const(rec_index);
		
		if(prec==NULL && prec_dest!=NULL)
		{
			strPath.Format("径路延伸");  
			break; // 不用再比较了
		}
		else if(prec!=NULL && prec_dest==NULL)
		{
			strPath.Format("径路缩短");  
			break; // 不用再比较了
		}
		if(prec==NULL && prec_dest==NULL)
			break; // 不用再比较了

		if(prec->GetStationNo()!=prec_dest->GetStationNo())
		{
			strStationList.Format("经由车站变化");  
			continue;
		}

		CString strStName=gpcdef.GetStationName(prec_dest->GetStationNo());
		if(rec_index==0)
		{
			if(prec->IsStartRec())
			{
				if(!prec_dest->IsStartRec())
					strArriveFlag="始发->接入";
			}
			else
			{
				if(prec_dest->IsStartRec())
					strArriveFlag="接入->始发";
			}

			if(prec_dest->IsStartRec())
			{
				CTime ctm(prec_dest->GetDepartTime());
				strDate = ctm.Format("%d日 %H:%M:%S");
			}
			else
			{
				CTime ctm(prec_dest->GetArriveTime());
				strDate = ctm.Format("%d日 %H:%M:%S");
			}

			strFirstStName=strStName;
		}
		if(rec_index==(src_rec_count-1) && src_rec_count==des_rec_count)
		{
			if(prec->IsTerminalRec())
			{
				if(!prec_dest->IsTerminalRec())
					strDepartFlag="终到->交出";
			}
			else
			{
				if(prec_dest->IsTerminalRec())
					strDepartFlag="交出->终到";
			}
		}
		
		if(prec->IsPassRec())
		{
			if(!prec_dest->IsPassRec())
			{
				tmp.Format("%s 通过->到开", strStName);
				strStopFlag += tmp;
			}
		}
		else
		{
			if(prec_dest->IsPassRec())
			{
				tmp.Format("%s 到开->通过", strStName);
				strStopFlag += tmp;
			}
		}

		
		if(prec->m_data.min_stop_time!=prec_dest->m_data.min_stop_time)
		{
			tmp.Format("%s 作业%d分->%d分", strStName,
					prec->m_data.min_stop_time/60, prec_dest->m_data.min_stop_time/60);  
			strWorkTime += tmp;
		}
		
		if((prec->m_data.arrive_side!=prec_dest->m_data.arrive_side)
			|| (prec->m_data.depart_side!=prec_dest->m_data.depart_side))
		{
			CString stra="";
			{
				const char* psidename=GetTGBasicParam()->GetSideNameByID(prec->m_data.station,prec->m_data.arrive_side);
				const char* psidename2=GetTGBasicParam()->GetSideNameByID(prec_dest->m_data.station,prec_dest->m_data.arrive_side);
				CString str,str1;
				if(psidename==NULL)
					str="?G";
				else
					str=psidename;
				if(psidename2==NULL)
					str1="?G";
				else
					str1=psidename2;

				stra.Format("%s->%s", str, str1); 
			}
			CString strd="";
			{
				const char* psidename=GetTGBasicParam()->GetSideNameByID(prec->m_data.station,prec->m_data.depart_side);
				const char* psidename2=GetTGBasicParam()->GetSideNameByID(prec_dest->m_data.station,prec_dest->m_data.depart_side);
				CString str,str1;
				if(psidename==NULL)
					str="?G";
				else
					str=psidename;
				if(psidename2==NULL)
					str1="?G";
				else
					str1=psidename2;

				strd.Format("%s->%s", str, str1);
			}

			if(strd==stra)
			{
				tmp.Format("%s:%s ", strStName, stra);
			}
			else
			{
				tmp.Format("%s:J %s F %s ", strStName, stra, strd);
			}
			strWorkSide += tmp;
		}

		if(strStationTime=="")
		{
			if(prec->GetArriveTime()!=prec_dest->GetArriveTime())
			{
				strFirstStName = gpcdef.GetStationName(prec_dest->GetStationNo());
				strStationTime.Format("到点变化");
			}
			else if(prec->GetDepartTime()!=prec_dest->GetDepartTime())
			{
				strFirstStName = gpcdef.GetStationName(prec_dest->GetStationNo());
				strStationTime.Format("发点变化");
			}
		}
	}

	
	
	xml="<ROOT>";
	tmp.Format("<GTID>%u</GTID>", desttrain->GetShiftTrainIndex());  // 按照无符号数,输出索引
	xml += tmp;
	tmp.Format("<TNO>%s</TNO>", GetValidStr(srctrain->GetTrainID()));
	xml += tmp;
	tmp.Format("<STATION>%s</STATION>", GetValidStr(strFirstStName));
	xml += tmp;
	tmp.Format("<DATE>%s</DATE>", GetValidStr(strDate));
	xml += tmp;

	xml += "<SOURCE></SOURCE>";

	CString oper="忽略";
	
	tmp="";
	if(strStationList!="")
		tmp.Format("<STATION_LIST clr=\"1\">%s</STATION_LIST>", GetValidStr(strStationList));
	else if(strPath!="")
		tmp.Format("<STATION_LIST clr=\"1\">%s</STATION_LIST>", GetValidStr(strPath));
	if(tmp!="")	
	{
		xml += tmp;
		oper="修改";
	}
	if(strArriveFlag!="")
	{
		tmp.Format("<ARRIVE_FLAG clr=\"1\">%s</ARRIVE_FLAG>", GetValidStr(strArriveFlag));
		xml += tmp;
		oper="修改";
	}
	if(strDepartFlag!="")
	{
		tmp.Format("<DEPART_FLAG clr=\"1\">%s</DEPART_FLAG>", GetValidStr(strDepartFlag));
		xml += tmp;
		oper="修改";
	}
	if(strStopFlag!="")
	{
		tmp.Format("<STOP_PASS clr=\"1\">%s</STOP_PASS>", GetValidStr(strStopFlag));
		xml += tmp;
		oper="修改";
	}
	if(strWorkTime!="")
	{
		tmp.Format("<WORK_TM clr=\"1\">%s</WORK_TM>", GetValidStr(strWorkTime));
		xml += tmp;
		oper="修改";
	}
	if(strWorkSide!="")
	{
		tmp.Format("<WORK_SIDE clr=\"1\">%s</WORK_SIDE>", GetValidStr(strWorkSide));
		xml += tmp;
		oper="修改";
	}

	if(strStationTime!="")
	{
		tmp.Format("<TIME clr=\"1\">%s</TIME>", GetValidStr(strStationTime));
		xml += tmp;
		oper="修改";
	}

	if(strEngine!="")
	{
		tmp.Format("<ENGINE clr=\"1\">变化</ENGINE>");
		xml += tmp;
		oper="修改";
	}
	
	tmp.Format("<GTID>%d</GTID>", desttrain->GetShiftTrainIndex());
	xml += tmp;
	tmp.Format("<OPER>%s</OPER>", oper);
	xml += tmp;
	xml += strFirstEnd;
	xml+="</ROOT>";

	if(oper=="修改")
		return true;
	else
		return false;
}

// 上海局邻台要求，在提示框中显示本台列车比邻台列车多的情况
void CDlgNeibTrainList::FindNeibNotExistTrains(CStringArray& m_warning_ex)
{
	CTG_ScheduleInfo info;
	m_LocalScheduleData.GetScheduleInfo(info);
	int nThisSectionNo = m_schd_info.m_nSectionID;
	int nNeibSectionNo = info.m_nSectionID;
	
	int nMode=GetTGFunctionConfig()->GetNeibPlanMode(nThisSectionNo, nNeibSectionNo);
	if(nMode!=2 || m_pSourceData==NULL)
		return;

	CTime btm(info.m_tStartTime), etm(info.m_tEndTime);
	CString strBtm=btm.Format("%d日%H:%M");
	CString strEtm=etm.Format("%d日%H:%M");
	
	// 存放比较中间结果
	std::vector<TRAIN_INDEX> neibHaveTrain, neibNotHasTrain;

	std::vector<NeibPlanParam> neib_param_vect;
	GetTGFunctionConfig()->GetNeibStructs(nThisSectionNo, nNeibSectionNo, neib_param_vect);
	for (int n=0; n<neib_param_vect.size(); n++)
	{
		int rec_index			= neib_param_vect[n].first_station_index;
		int first_station		= neib_param_vect[n].first_station;
		int next_station		= neib_param_vect[n].next_station;
		int next_next_station	= neib_param_vect[n].next_next_station;
		int neib_station		= neib_param_vect[n].neib_station;
		int consider_start_train= neib_param_vect[n].consider_start_train;		
		
		GpcLogTrainMessage(0,0,"first_station=%d(%d) next_station=%d next_next_station=%d neib_station=%d consider_start_train=%d",
			first_station, rec_index, next_station, next_next_station, neib_station, consider_start_train);
		if (neib_station==0)
			continue;
	
		CTG_TrainPtrGroup thisTrainGroup;
		m_pSourceData->FindTrains(thisTrainGroup, neib_station, info.m_tStartTime,info.m_tEndTime);
		for (size_t i =0; i<thisTrainGroup.GetTrainCount(); i++)
		{
			CTG_SingleTrain *pThisTrain = thisTrainGroup.GetTrainPtr(i);
			if(pThisTrain==NULL)
				continue;

			if((((CButton *)GetDlgItem(IDC_CHK_PASSENGER))->GetCheck()==BST_UNCHECKED)
				&& pThisTrain->IsPassengerTrain())
				continue;
			if((((CButton *)GetDlgItem(IDC_CHK_FREIGHT))->GetCheck()==BST_UNCHECKED)
				&& !pThisTrain->IsPassengerTrain())
				continue;

			CTG_TrainRecord *prec = pThisTrain->GetTrainRecordByStation(neib_station);
			if(prec==NULL)
				continue;
					
			const CTG_TrainRecord *pFirstRec=NULL, *pNextRec=NULL, *pNextNextRec=NULL;
			pFirstRec=pThisTrain->GetTrainRecordByRecIndex_Const(0);
			if(pFirstRec==NULL)
				continue;

			if(pFirstRec->m_data.station!=first_station)
				continue;

			if(pFirstRec->GetArriveTGIndex()!=rec_index)
			{
				GpcLogTrainMessage(0,0,"列车:%s(%u) shift_index=%u 站序不同(列车接入站序%d-邻台配置站序%d),不比较邻台列车", 
						pThisTrain->GetTrainID(), pThisTrain->GetTrainIndex(), pThisTrain->GetShiftTrainIndex(),
						pFirstRec->GetArriveTGIndex(), rec_index);
				continue;
			}

			if(next_station!=0)
			{
				pNextRec=pThisTrain->GetTrainRecordByRecIndex_Const(1);
				if(pNextRec==NULL)
					continue;
				if(pNextRec->m_data.station!=next_station)
					continue;
			}
			if(next_next_station!=0)
			{
				pNextNextRec=pThisTrain->GetTrainRecordByRecIndex_Const(2);
				if(pNextNextRec==NULL)
					continue;
				if(pNextNextRec->m_data.station!=next_next_station)
					continue;
			}

			// 本台始发车,不去邻台查找
			if(consider_start_train==0)
			{
				if(pFirstRec->IsStartRec())
				{
					GpcLogTrainMessage(0,0,"列车:%s(%u) shift_index=%u 在本台始发,不比较邻台列车", 
						pThisTrain->GetTrainID(), pThisTrain->GetTrainIndex(), pThisTrain->GetShiftTrainIndex());
					continue;
				}
			}

			bool bNeibHasTrain = false;
			std::map<TRAIN_INDEX, CTG_SingleTrain>::iterator it = m_mapTrains.begin();
			for(int i=0; it != m_mapTrains.end(); it++, i++)
			{
				CTG_SingleTrain *pNeibTrain=&it->second;
				if(NULL==pNeibTrain)
					continue;
				
				// 邻台必需包含台间站
				CTG_TrainRecord *pneibrec = pNeibTrain->GetTrainRecordByStation_Reverse(neib_station);
				if(pneibrec==NULL)
					continue;

				if (pNeibTrain->MatchTrainID(prec->GetArriveTID()) || pNeibTrain->MatchTrainID(prec->GetDepartTID()))
				{
					bNeibHasTrain = true;
					GpcLogTrainMessage(0,0,"邻台存在列车 %s,index=%u shift_index=%u.径路:%s", 
						pNeibTrain->GetTrainID(), pNeibTrain->GetTrainIndex(), pNeibTrain->GetShiftTrainIndex(),
						pNeibTrain->toString());
					break;
				}
			}
			
			TRAIN_INDEX train_index=pThisTrain->GetTrainIndex();
			if (bNeibHasTrain)
			{
				if(std::find(neibHaveTrain.begin(), neibHaveTrain.end(), train_index)
					 == neibHaveTrain.end())
				{
					neibHaveTrain.push_back(train_index);
				}
			}
			else
			{
				if(std::find(neibNotHasTrain.begin(), neibNotHasTrain.end(), train_index)
					 == neibNotHasTrain.end())
				{
					neibNotHasTrain.push_back(train_index);
				}
			}
		}
	}

	for(int m=0; m<neibNotHasTrain.size(); m++)
	{
		TRAIN_INDEX train_index=neibNotHasTrain[m];

		bool bHaveTrain=false;
		for(int n=0; n<neibHaveTrain.size(); n++)
		{
			if(train_index==neibHaveTrain[n])
			{
				bHaveTrain=true;
				break;
			}
		}

		if(!bHaveTrain)
		{
			CTG_SingleTrain *ptrain=m_pSourceData->GetSingleTrain(train_index);
			if(ptrain)
			{
				CString str;
				str.Format("车次:%s 在 %s %s到%s时间范围内不存在, 请人工核对并确认",
					ptrain->GetTrainID(),gpcdef.GetSectionName(nNeibSectionNo),
					strBtm, strEtm);
				m_warning_ex.Add(str); // 暂时不判断重复报警
			}
		}
	}
}

// 排序显示
extern bool sortbytm(const TrainResultTmp& n1, const TrainResultTmp& n2);
void CDlgNeibTrainList::DisplayTrainAcceptOrder()
{
	int m_trainptrs_cnt;
	CTG_SingleTrain* m_trainptrs[4096];
	std::vector<TrainResultTmp> m_trainpter_list;

	m_grid.DeleteNonFixedRows();
	std::vector<TRAIN_INDEX> train_list1, train_list2;
	for(int n=0; n<nMiddleStaNum&&n<64; n++)
	{
		int station=nMiddleStaList[n];
		CTG_TrainPtrGroup result;
		m_pData->FindTrainsNotOrder(result, station);
		for(int m=0; m<result.GetTrainCount(); m++)
		{
			CTG_SingleTrain *ptrain=result.GetTrainPtr(m);
			if(!ptrain)
				continue;
			
			const CTG_TrainRecord *prec=ptrain->GetTrainRecordByStation_Const(station);
			if(NULL==prec)
				continue;

			if(std::find(train_list1.begin(), train_list1.end(), ptrain->GetTrainIndex())==train_list1.end())
			{
				train_list1.push_back(ptrain->GetTrainIndex());
			}
			else
			{
				continue;
			}
			TrainResultTmp s;
			s.tm=prec->GetArriveTime();
			s.p=ptrain;
			m_trainpter_list.push_back(s);
		}
		result.Clear();
		m_pOtherData->FindTrainsNotOrder(result, station);
		for(int m=0; m<result.GetTrainCount(); m++)
		{
			CTG_SingleTrain *ptrain=result.GetTrainPtr(m);
			if(!ptrain)
				continue;
			
			const CTG_TrainRecord *prec=ptrain->GetTrainRecordByStation_Const(station);
			if(NULL==prec)
				continue;
			
			if(std::find(train_list2.begin(), train_list2.end(), ptrain->GetTrainIndex())==train_list2.end())
			{
				train_list2.push_back(ptrain->GetTrainIndex());
			}
			else
			{
				continue;
			}
			TrainResultTmp s;
			s.tm=prec->GetArriveTime();
			s.p=ptrain;
			m_trainpter_list.push_back(s);
		}

		if(m_trainpter_list.size()==0)
			continue;
			
		m_trainptrs_cnt=0;
		memset(m_trainptrs, 0, sizeof(m_trainptrs));
		std::sort(m_trainpter_list.begin(), m_trainpter_list.end(), sortbytm);
		for(int m=0; m<m_trainpter_list.size(); m++)
		{
			m_trainptrs[m_trainptrs_cnt]=m_trainpter_list[m].p;
			m_trainptrs_cnt++;
		}
		m_trainpter_list.clear();
	
		for(int m=0;m<m_trainptrs_cnt;m++)
		{
			CTG_SingleTrain *ptrain=m_trainptrs[m];
			if(!ptrain)
				continue;
			
			CTG_TrainRecord *pneibrec=ptrain->GetTrainRecordByStation(station);
			if(NULL==pneibrec)
				continue;

			CString xml;
			BOOL selected=GetGridXmlShowNeibStation(ptrain, xml, pneibrec);
			
			CString cap;
			cap.Format("%u", ptrain->GetShiftTrainIndex()); 
			int row=m_grid.InsertTrainInfo(cap, xml);
			if(row>=0)
			{
				m_grid.SetRowSelected(row,selected); 
				m_grid.GetCell(row,1)->SetData(ptrain->GetTrainIndex());
				m_grid.GetCell(row,2)->SetData(1);
			}
			else
			{
				GpcLogTrainMessage(0,0,"邻台计划 显示错误 %s", xml);
				AfxMessageBox("邻台计划显示不完整清重新获取!");
				m_grid.DeleteNonFixedRows();
				break;
			}
		}
	}

	CTG_TrainPtrGroup group;
	m_pData->FindAllTrains(group);

	for(int i=0;i<group.GetTrainCount();i++)
	{
		CTG_SingleTrain *ptrain=group.GetTrainPtr(i);
		if(!ptrain)
			continue;
		
		if(std::find(train_list1.begin(), train_list1.end(), ptrain->GetTrainIndex())!=train_list1.end())
		{
			continue;
		}

		CString xml;
		BOOL selected=GetGridXml(ptrain, xml);
		
		CString cap;
		cap.Format("%u", ptrain->GetShiftTrainIndex()); 
		int row=m_grid.InsertTrainInfo(cap, xml);
		if(row>=0)
		{
			m_grid.SetRowSelected(row,selected); 
			m_grid.GetCell(row,1)->SetData(ptrain->GetTrainIndex());
			m_grid.GetCell(row,2)->SetData(1);
		}
		else
		{
			GpcLogTrainMessage(0,0,"邻台计划 显示错误 %s", xml);
			AfxMessageBox("邻台计划显示不完整清重新获取!");
			m_grid.DeleteNonFixedRows();
			break;
		}
	}

	CTG_TrainPtrGroup group1;
	m_pOtherData->FindAllTrains(group1);
	for(int i=0;i<group1.GetTrainCount();i++)
	{
		CTG_SingleTrain *ptrain=group1.GetTrainPtr(i);
		if(!ptrain)
			continue;
		
		if(std::find(train_list2.begin(), train_list2.end(), ptrain->GetTrainIndex())!=train_list2.end())
		{
			continue;
		}

		CString xml;
		BOOL selected=GetGridXml(ptrain, xml);
		
		CString cap;
		cap.Format("%u", ptrain->GetShiftTrainIndex()); 
		int row=m_grid.InsertTrainInfo(cap, xml);
		if(row>=0)
		{
			m_grid.SetRowSelected(row,selected); 
			m_grid.GetCell(row,1)->SetData(ptrain->GetTrainIndex());
			m_grid.GetCell(row,2)->SetData(1);
		}
		else
		{
			GpcLogTrainMessage(0,0,"邻台计划 显示错误 %s", xml);
			AfxMessageBox("邻台计划显示不完整清重新获取!");
			m_grid.DeleteNonFixedRows();
			break;
		}
	}

	m_grid.AutoSizeColumns();
	m_grid.Refresh();
}

BOOL CDlgNeibTrainList::GetGridXmlShowNeibStation(CTG_SingleTrain *ptrain, CString& xml, CTG_TrainRecord *pneibrec)
{
	BOOL selected=TRUE;
	
	CTG_SingleTrain *prev_train=NULL;
	if (ptrain->m_nCurrConTrainIndex!=0)
		prev_train=m_pSourceData->GetSingleTrain(ptrain->m_nCurrConTrainIndex);

	CTG_TrainRecord* pfirst=ptrain->GetTrainRecordByRecIndex(0);
	CTG_TrainRecord* plast=ptrain->GetTrainRecordByRecIndex(ptrain->GetTrainRecordCount()-1);
	
	CString strDate="错误", strStationName="未知";
	int neib_station=pfirst->GetStationNo();
	if(pneibrec)
	{
		if(pneibrec->IsStartRec())
		{
			CTime ctm(pneibrec->GetDepartTime());
			strDate = ctm.Format("%d日 %H:%M:%S");
		}
		else
		{
			CTime ctm(pneibrec->GetArriveTime());
			strDate = ctm.Format("%d日 %H:%M:%S");
		}
		neib_station=pneibrec->GetStationNo();
		strStationName=gpcdef.GetStationName(neib_station);
	}

	CString tmp;

	xml="<ROOT>";
	tmp.Format("<GTID>%u</GTID>", ptrain->GetShiftTrainIndex());   // 按照无符号数,输出索引
	xml += tmp;
	tmp.Format("<TNO>%s</TNO>", ptrain->GetTrainID());
	xml += tmp;
	tmp.Format("<STATION>%s</STATION>", strStationName);
	xml += tmp;
	tmp.Format("<DATE>%s</DATE>", strDate);
	xml += tmp;
	tmp.Format("<PATH>%s--%s</PATH>", gpcdef.GetStationName(pfirst->GetStationNo()), 
			gpcdef.GetStationName(plast->GetStationNo()));
	xml += tmp;
	
	if(!prev_train || prev_train->GetTrainRecordCount()<=0)
	{
		if(ptrain->m_strSource=="日班计划" || ptrain->m_strSource=="调整计划")
			tmp.Format("<SOURCE clr=\"1\">%s</SOURCE>", ptrain->m_strSource);
		else
			tmp.Format("<SOURCE clr=\"2\">%s</SOURCE>", ptrain->m_strSource);
		xml += tmp;

		tmp.Format("<STATION_LIST>--</STATION_LIST>");
		xml += tmp;
		
		if(pfirst->IsStartRec())
			tmp.Format("<ARRIVE_FLAG clr=\"1\">始发</ARRIVE_FLAG>");
		else
			tmp.Format("<ARRIVE_FLAG clr=\"1\">接入</ARRIVE_FLAG>");
		xml += tmp;

		if(plast->IsTerminalRec())
			tmp.Format("<DEPART_FLAG clr=\"1\">终到</DEPART_FLAG>");
		else
			tmp.Format("<DEPART_FLAG clr=\"1\">交出</DEPART_FLAG>");
		xml += tmp;
	
		tmp.Format("<STOP_PASS>--</STOP_PASS>");
		xml += tmp;
		
		tmp.Format("<WORK_TM>--</WORK_TM>");
		xml += tmp;

		tmp.Format("<ENGINE>--</ENGINE>");
		xml += tmp;

		tmp.Format("<WORK_SIDE>--</WORK_SIDE>");
		xml += tmp;
		
		tmp.Format("<TIME>--</TIME>");
		xml += tmp;
		
		tmp.Format("<OPER>%s</OPER>", "新增");
		xml += tmp;
		xml+="</ROOT>";
		
		selected=TRUE;
	}
	else
	{
		selected=GetTrainCompareResult(prev_train, ptrain, neib_station, xml);
	}

	return selected;
}

BOOL CDlgNeibTrainList::GetTrainCompareResult(CTG_SingleTrain *srctrain, CTG_SingleTrain *desttrain, int neib_station, CString& xml)
{
	CString tmp;
	
	CString strPath="";
	CString strStationList="";
	CString strArriveFlag="";
	CString strDepartFlag="";
	CString strStopFlag="";
	CString strWorkTime="";
	CString strWorkSide="";
	CString strTime="";
	CString strEngine="";
	// 比较机车号
	CTG_TrainEngineList newEngineList=*srctrain->GetEngineList();
	CTG_TrainEngineList oldEngineList=*desttrain->GetEngineList();

	if(oldEngineList == newEngineList)
		strEngine="";
	else
		strEngine="变化";

	const int src_rec_count=srctrain->GetTrainRecordCount();
	const int des_rec_count=desttrain->GetTrainRecordCount();
	const int rec_count=(src_rec_count>=des_rec_count)?src_rec_count:des_rec_count;
	bool bFindNeib=false;
	for(int rec_index=0; rec_index<rec_count; rec_index++)
	{
		const CTG_TrainRecord *prec=srctrain->GetTrainRecordByRecIndex_Const(rec_index);
		const CTG_TrainRecord *prec_dest=desttrain->GetTrainRecordByRecIndex_Const(rec_index);
		
		if(prec==NULL && prec_dest!=NULL)
		{
			strPath.Format("径路延伸");  
			break; // 不用再比较了
		}
		else if(prec!=NULL && prec_dest==NULL)
		{
			strPath.Format("径路缩短");  
			break; // 不用再比较了
		}
		if(prec==NULL && prec_dest==NULL)
			break; // 不用再比较了

		if(prec->GetStationNo()!=prec_dest->GetStationNo())
		{
			strStationList.Format("经由车站变化");  
			break;
		}

		if(rec_index==0)
		{
			if(prec->IsStartRec())
			{
				if(!prec_dest->IsStartRec())
					strArriveFlag="始发->接入";
			}
			else
			{
				if(prec_dest->IsStartRec())
					strArriveFlag="接入->始发";
			}
		}

		CString strStName=gpcdef.GetStationName(prec_dest->GetStationNo());
		if(rec_index==(src_rec_count-1) && src_rec_count==des_rec_count)
		{
			if(prec->IsTerminalRec())
			{
				if(!prec_dest->IsTerminalRec())
					strDepartFlag="终到->交出";
			}
			else
			{
				if(prec_dest->IsTerminalRec())
					strDepartFlag="交出->终到";
			}
		}
		
		if(prec->IsPassRec())
		{
			if(!prec_dest->IsPassRec())
			{
				tmp.Format("%s 通过->到开", strStName);
				strStopFlag += tmp;
			}
		}
		else
		{
			if(prec_dest->IsPassRec())
			{
				tmp.Format("%s 到开->通过", strStName);
				strStopFlag += tmp;
			}
		}

		if(prec->m_data.min_stop_time!=prec_dest->m_data.min_stop_time)
		{
			tmp.Format("%s 作业%d分->%d分", strStName,
					prec->m_data.min_stop_time/60, prec_dest->m_data.min_stop_time/60);  
			strWorkTime += tmp;
		}
		
		if((prec->m_data.arrive_side!=prec_dest->m_data.arrive_side)
			|| (prec->m_data.depart_side!=prec_dest->m_data.depart_side))
		{
			CString stra="";
			{
				const char* psidename=GetTGBasicParam()->GetSideNameByID(prec->m_data.station,prec->m_data.arrive_side);
				const char* psidename2=GetTGBasicParam()->GetSideNameByID(prec_dest->m_data.station,prec_dest->m_data.arrive_side);
				CString str,str1;
				if(psidename==NULL)
					str="?G";
				else
					str=psidename;
				if(psidename2==NULL)
					str1="?G";
				else
					str1=psidename2;

				stra.Format("%s->%s", str, str1); 
			}
			CString strd="";
			{
				const char* psidename=GetTGBasicParam()->GetSideNameByID(prec->m_data.station,prec->m_data.depart_side);
				const char* psidename2=GetTGBasicParam()->GetSideNameByID(prec_dest->m_data.station,prec_dest->m_data.depart_side);
				CString str,str1;
				if(psidename==NULL)
					str="?G";
				else
					str=psidename;
				if(psidename2==NULL)
					str1="?G";
				else
					str1=psidename2;

				strd.Format("%s->%s", str, str1);
			}

			if(strd==stra)
			{
				tmp.Format("%s:%s ", strStName, stra);
			}
			else
			{
				tmp.Format("%s:J %s F %s ", strStName, stra, strd);
			}
			strWorkSide += tmp;
		}
		
		if(!bFindNeib)
		{
			if(prec->GetArriveTime()!=prec_dest->GetArriveTime())
			{
				tmp.Format("%s 到点变化", strStName);
				strTime = tmp;
			}
			else if(prec->GetDepartTime()!=prec_dest->GetDepartTime())
			{
				tmp.Format("%s 发点变化", strStName);
				strTime = tmp;
			}

			if(prec->GetStationNo()==neib_station && strStName!="")
			{
				bFindNeib=true;
			}
		}
	}

	xml += "<SOURCE></SOURCE>";

	CString oper="忽略";
	
	tmp="";
	if(strStationList!="")
		tmp.Format("<STATION_LIST clr=\"1\">%s</STATION_LIST>", GetValidStr(strStationList));
	else if(strPath!="")
		tmp.Format("<STATION_LIST clr=\"1\">%s</STATION_LIST>", GetValidStr(strPath));
	if(tmp!="")	
	{
		xml += tmp;
		oper="修改";
	}
	if(strArriveFlag!="")
	{
		tmp.Format("<ARRIVE_FLAG clr=\"1\">%s</ARRIVE_FLAG>", GetValidStr(strArriveFlag));
		xml += tmp;
		oper="修改";
	}
	if(strDepartFlag!="")
	{
		tmp.Format("<DEPART_FLAG clr=\"1\">%s</DEPART_FLAG>", GetValidStr(strDepartFlag));
		xml += tmp;
		oper="修改";
	}
	if(strStopFlag!="")
	{
		tmp.Format("<STOP_PASS clr=\"1\">%s</STOP_PASS>", GetValidStr(strStopFlag));
		xml += tmp;
		oper="修改";
	}
	if(strWorkTime!="")
	{
		tmp.Format("<WORK_TM clr=\"1\">%s</WORK_TM>", GetValidStr(strWorkTime));
		xml += tmp;
		oper="修改";
	}
	if(strWorkSide!="")
	{
		tmp.Format("<WORK_SIDE clr=\"1\">%s</WORK_SIDE>", GetValidStr(strWorkSide));
		xml += tmp;
		oper="修改";
	}

	if(strTime!="")
	{
		tmp.Format("<TIME clr=\"1\">%s</TIME>", GetValidStr(strTime));
		xml += tmp;
		oper="修改";
	}

	if(strEngine!="")
	{
		tmp.Format("<ENGINE clr=\"1\">变化</ENGINE>");
		xml += tmp;
		oper="修改";
	}
	
	tmp.Format("<OPER>%s</OPER>", oper);
	xml += tmp;
	xml+="</ROOT>";

	if(oper=="修改")
		return TRUE;
	else
		return FALSE;
}
