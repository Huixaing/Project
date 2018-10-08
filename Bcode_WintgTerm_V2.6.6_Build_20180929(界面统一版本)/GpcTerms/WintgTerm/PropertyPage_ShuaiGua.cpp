// PropertyPage_ShuaiGua.cpp : 实现文件
//

#include "stdafx.h"
#include "resource.h"
#include "PropertyPage_ShuaiGua.h"
#include "tgmsg.h"
#include "tgdocument.h"
#include "tgpspdata.h"
#include "sysshare.h"
// CPropertyPage_ShuaiGua 对话框

IMPLEMENT_DYNAMIC(CPropertyPage_ShuaiGua, CPropertyPage)
CPropertyPage_ShuaiGua::CPropertyPage_ShuaiGua(CTgDocument *pdoc)
: CPropertyPage(CPropertyPage_ShuaiGua::IDD)
{
	m_pDoc=pdoc;
	m_pTrain=NULL;
	m_bReadOnly=m_pDoc->IsQueryMode();
	m_nScheduleType=CTG_ScheduleInfo::SCHEDULE_TYPE_WORK; 
	m_nCurrentSelectRecordIndex=-1;
	m_pNotifyWindow=NULL;
}

CPropertyPage_ShuaiGua::~CPropertyPage_ShuaiGua()
{
}

void CPropertyPage_ShuaiGua::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ED_SHUAIGUA_INFO, m_cShuaiGuaInfo);
	DDX_Control(pDX, IDC_ED_YINGYE_INFO, m_cYingYeInfo);
	DDX_Control(pDX, IDC_ED_KEDIAOINFO, m_cKeDiaoInfo);
	DDX_Control(pDX, IDC_ED_JUNTEYUNINFO, m_cImportantInfo);
	DDX_Control(pDX, IDC_ED_STPINFO, m_cStopInfo);
}


BEGIN_MESSAGE_MAP(CPropertyPage_ShuaiGua, CPropertyPage)
	ON_BN_CLICKED(IDC_BTN_ACK, OnAck)
	ON_BN_CLICKED(IDC_BTN_REFRESH, OnRefresh)
	ON_MESSAGE(UM_TG_TRAIN_SELECTED,OnSchdTrainSelected)
	ON_NOTIFY(GVN_SELCHANGING,IDC_SHUAIGUA_GRID,OnGridSelectChange)
	ON_MESSAGE(UM_UNI_MESSAGE, OnCommMsg)
	ON_CBN_SELCHANGE(IDC_COMBO_STATION_NAME_SUBAO, OnCbnSelchangeComboStationNameSubao)
END_MESSAGE_MAP()


// CPropertyPage_ShuaiGua 消息处理程序
void CPropertyPage_ShuaiGua::ClearData()
{
	m_nCurrentSelectRecordIndex=-1;
	m_grid.DeleteNonFixedRows(); 
	m_cShuaiGuaInfo.SetWindowText("+/-"); 
	m_cImportantInfo.SetWindowText("");
	m_cKeDiaoInfo.SetWindowText("");
	m_cYingYeInfo.SetWindowText("");
}

CString Change0ATo0A0D(const char *s)
{
	CString tmp=s;
	tmp.Replace("\n","\r\n");
	return tmp;
}

void  CPropertyPage_ShuaiGua::RefreshData()
{
	ClearData();
	if(!m_pTrain)
		return;
	CTG_TrainRecord *prec;
	for(int i=0;i<m_pTrain->GetTrainRecordCount();i++)
	{
		prec=m_pTrain->GetTrainRecord(i);
		CString no;
		no.Format("%d",i); 
		int row=m_grid.InsertRecord(no,m_pTrain,prec->GetStationNo(),i); 
	}
	m_grid.Refresh();

	if(GetTGFunctionConfig()->IsEnableTDMS5_0()) //TD5.0
	{
		m_cKeDiaoInfo.SetWindowText(m_pTrain->GetImportInfoText(TRAIN_TXT_INFO_TYPE_KEDIAO));
		m_cYingYeInfo.SetWindowText(m_pTrain->GetImportInfoText(TRAIN_TXT_INFO_TYPE_YINGYE));
	}
	else
	{
		const char *p;

		p=m_pTrain->GetImportInfo(TRAIN_TXT_INFO_TYPE_KEDIAO);
		if(p)
			m_cKeDiaoInfo.SetWindowText(Change0ATo0A0D(p));
		else
			m_cKeDiaoInfo.SetWindowText("");

		p=m_pTrain->GetImportInfo(TRAIN_TXT_INFO_TYPE_YINGYE);
		if(p)
			m_cYingYeInfo.SetWindowText(Change0ATo0A0D(p));
		else
			m_cYingYeInfo.SetWindowText("");
	}
}	

LRESULT CPropertyPage_ShuaiGua::OnSchdTrainSelected(WPARAM wp,LPARAM lp)
{
	CTG_TrainScheduleBase *pdata;
	CWorkScheduleHolder *pholder=m_pDoc->GetTGDataHolder();
	if(m_nScheduleType==CTG_ScheduleInfo::SCHEDULE_TYPE_WORK) 
		pdata=pholder->GetWorkScheduleData();
	else
		if(m_nScheduleType==CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT) 
			pdata=pholder->GetShiftScheduleData();
		else
			return 0;
	m_pTrain=pdata->GetSingleTrain(wp);
	RefreshData();
	return 0;
}


BOOL CPropertyPage_ShuaiGua::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	CRect rc;
	GetDlgItem(IDC_SHUAIGUA_GRID)->GetWindowRect(rc);
	ScreenToClient(rc);
	m_grid.CreateGrid(this,rc,IDC_SHUAIGUA_GRID);
	m_grid.SetEditable(FALSE);
	if(m_bReadOnly)
	{
		m_cShuaiGuaInfo.EnableWindow(FALSE);
		GetDlgItem(IDC_BTN_ACK)->EnableWindow(FALSE);
		m_cImportantInfo.EnableWindow(FALSE);
		m_cStopInfo.EnableWindow(FALSE);
		GetDlgItem(IDC_BTN_ACK)->EnableWindow(FALSE); 
	}
	else
		RegisterMsgProcHandleWnd(GetSafeHwnd(),MSGID_MsgStringMessage,0);
	CTG_LogicObject  *pobj=m_pDoc->GetCurrentActiveObject();
	if(pobj && pobj->GetObjectType()==TG_OBJ_TRAIN)
	{
		m_pTrain=(CTG_SingleTrain *)pobj;
		RefreshData();

		// G7
		m_nCurrentSelectRecordIndex = m_pTrain->GetCurSelRecIndex();
		CComboBox* pWnd = (CComboBox*)GetDlgItem(IDC_COMBO_STATION_NAME_SUBAO);
		if(NULL != pWnd)
		{
			pWnd->ResetContent();

			CTG_TrainRecord *prec;
			for(int i=0;i<m_pTrain->GetTrainRecordCount();i++)
			{
				prec=m_pTrain->GetTrainRecord(i);
				CString staName = gpcdef.GetStationName(prec->GetStationNo());
				pWnd->AddString(staName);
			}

			if(m_nCurrentSelectRecordIndex>=0 && m_nCurrentSelectRecordIndex<m_pTrain->GetTrainRecordCount())
				pWnd->SetCurSel(m_nCurrentSelectRecordIndex);
		}
		
		if(GetTGFunctionConfig()->IsEnableTDMS5_0()) //TD5.0
		{
			RequestShiftInfoFromOPMS(m_pTrain->GetShiftTrainIndex());
		}
	}
	InformParent();
	return TRUE;
}

void CPropertyPage_ShuaiGua::OnGridSelectChange(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_GRIDVIEW *pv=(NM_GRIDVIEW *)pNMHDR;
	if(pv->iRow>=1)
	{
		if(!m_pTrain)
			return;
		const CTG_TrainInfo *pTrainInfo=m_pTrain->GetTrainInfo_Const();
		if(!m_bReadOnly) m_cShuaiGuaInfo.EnableWindow(TRUE); 
		m_nCurrentSelectRecordIndex=pv->iRow-1;
		CTG_TrainRecord *prec=m_pTrain->GetTrainRecordByRecIndex(m_nCurrentSelectRecordIndex); 
		if(prec)
		{
			CComboBox* pWnd = (CComboBox*)GetDlgItem(IDC_COMBO_STATION_NAME_SUBAO);
			pWnd->SetCurSel(m_nCurrentSelectRecordIndex);
			UpdateData(FALSE);

			const char *p;
			p=prec->GetStationText(STATION_TXT_INFO_TYPE_SHUAIGUA); 
			if(p)
				m_cShuaiGuaInfo.SetWindowText(Change0ATo0A0D(p)); 	
			else
				m_cShuaiGuaInfo.SetWindowText("+/-"); 	
			p=GetTrainShortImportantInfo(prec, pTrainInfo->m_data.space_rank);
			if(p)
				m_cImportantInfo.SetWindowText(p);
			else
				m_cImportantInfo.SetWindowText("");
			p=GetTrainStopInfo(prec);
			if(p)
				m_cStopInfo.SetWindowText(p);
			else
				m_cStopInfo.SetWindowText("");

			if(!m_bReadOnly)
			{
				m_cShuaiGuaInfo.EnableWindow(TRUE);
				m_cImportantInfo.EnableWindow(TRUE);
				m_cStopInfo.EnableWindow(TRUE);
			}

		}
	}
}


void CPropertyPage_ShuaiGua::OnAck()
{
	if(!m_pTrain)
		return;
	CString cs;
	if(m_nCurrentSelectRecordIndex>=0)
	{
		CTG_TrainRecord *prec=m_pTrain->GetTrainRecordByRecIndex(m_nCurrentSelectRecordIndex); 
		if(!prec)
			return;
		m_cShuaiGuaInfo.GetWindowText(cs);
		if(cs == "+/-")
			cs="";
		prec->SetStationText(cs,STATION_TXT_INFO_TYPE_SHUAIGUA);
		prec->SetOpmsSationTextState("0"); // 2011-8-23 调度员确认标志
		m_pTrain->GetRecordList()->SetSyncStatusX(SYCFLAG_NULL,TRUE);  
		m_cShuaiGuaInfo.EnableWindow(FALSE);
		m_grid.RefreshStationRecord(m_pTrain,prec->GetStationNo(),m_nCurrentSelectRecordIndex);
		m_grid.Refresh();
	}
	m_cYingYeInfo.GetWindowText(cs);
	m_pTrain->SetImportInfo(cs,TRAIN_TXT_INFO_TYPE_YINGYE);

	m_cKeDiaoInfo.GetWindowText(cs);
	m_pTrain->SetImportInfo(cs,TRAIN_TXT_INFO_TYPE_KEDIAO);

	m_pTrain->GetTrainInfo()->SetSyncStatusX(SYCFLAG_NULL,TRUE);  
}

void CPropertyPage_ShuaiGua::InformParent()
{
	TRAIN_SELECT_PARA para;
	CTG_ScheduleInfo schd;
	m_pDoc->GetScheduleInfo( schd);
	TIME shift_start,shift_end;
	schd.GetShiftManagerRange(shift_start,shift_end); 
	para.starttime= shift_start;
	para.endtime= shift_end;
	para.list_type=0; 
	if(m_pNotifyWindow)
		m_pNotifyWindow->SendMessage(UM_TG_TRAIN_SELECTED,(WPARAM)0,(LPARAM)&para);
}

void CPropertyPage_ShuaiGua::RequestShiftInfoFromOPMS(DWORD gtid)
{
	if(GetTGFunctionConfig()->IsEnableTDMS5_0())
	{
		m_pDoc->RequestTrainExpand_KD_YL_FromOPMS(gtid, 5);
		return;
	}

	CString token,dest;
	long  eid=GetSpecialEntityID(SERVER_TYPE_TDCOMM); 
	TD_GenRequestMessage_15_KEDIAO_INFO(token,dest,gtid);
	TGSendStringMessage(token,dest,eid);
	GpcLogTrainMessage(0, 0, "向OPMS客调信息:eid:%d token:%s xml:%s", eid, token, dest);
	TD_GenRequestMessage_16_YINGYE_INFO(token,dest,gtid);
	TGSendStringMessage(token,dest,eid);
	GpcLogTrainMessage(0, 0, "向OPMS营业信息:eid:%d token:%s xml:%s", eid, token, dest);
}

void CPropertyPage_ShuaiGua::OnRefresh()
{
	if(m_pTrain)
	{
		RequestShiftInfoFromOPMS(m_pTrain->GetShiftTrainIndex());
		RefreshData();
	}
}
LRESULT CPropertyPage_ShuaiGua::OnCommMsg(WPARAM wParam, LPARAM lParam) 
{
	UNI_MESSAGE_STRUCT *pMsgStruct=( UNI_MESSAGE_STRUCT *)lParam;
	CLBTH::MsgStringMessage *pMsg= (CLBTH::MsgStringMessage  *)pMsgStruct->pmsg;
	const char *token=pMsg->GetToken();
	const char *content=pMsg->GetContent();
	if(token && content)
	{
		if(GetTGFunctionConfig()->IsEnableTDMS5_0()) //TD5.0
		{
			CTG_TrainScheduleBase *pdata=NULL;
			CWorkScheduleHolder *pholder=m_pDoc->GetTGDataHolder();
			if(!stricmp(token,TOKEN_15_ACK_KEDIAO_INFO) && (pholder!=NULL))
			{
				CTG_TrainKediaoInfo kd;
				if(TD_GetKediaoInfoFromXML(kd,content,token))
				{
					if(kd.msgid==5)
					{
						CTG_SingleTrain* ptrain=NULL;
						if(CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT==m_nScheduleType)
						{
							pdata=pholder->GetShiftScheduleData();
							if(pdata)
								ptrain=pdata->GetSingleTrain(kd.gtid);
						}
						if(CTG_ScheduleInfo::SCHEDULE_TYPE_WORK==m_nScheduleType)
						{
							pdata=pholder->GetWorkScheduleData();
							if(pdata)
								ptrain=pdata->GetSingleTrainByShiftIndex(kd.gtid, NULL);
						}
						if(ptrain!=NULL && pdata!=NULL)
						{
							if(!ptrain->GetTrainInfoExpand_Const()->IsTrainKediaoSame(kd))
							{
								CTG_SingleTrain train=*ptrain;
								train.GetTrainInfoExpand()->UpdateTrainKediao(kd);

								CTG_ScheduleInfo schdinfo;	
								pdata->GetScheduleInfo(schdinfo); 
								pdata->UpdateTrain(schdinfo, train, SYCFLAG_NULL);
							}
							RefreshData();
						}
					}
				}
			}
			if(!stricmp(token,TOKEN_16_ACK_YINGYE_INFO) && (pholder!=NULL))
			{
				CTG_TrainYingyeList yy;
				if(TD_GetYingyeInfoFromXML(yy,content,token))
				{
					if(yy.msgid==5)
					{
						CTG_SingleTrain* ptrain=NULL;
						if(CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT==m_nScheduleType)
						{
							pdata=pholder->GetShiftScheduleData();
							if(pdata)
								ptrain=pdata->GetSingleTrain(yy.gtid);
						}
						if(CTG_ScheduleInfo::SCHEDULE_TYPE_WORK==m_nScheduleType)
						{
							pdata=pholder->GetWorkScheduleData();
							if(pdata)
								ptrain=pdata->GetSingleTrainByShiftIndex(yy.gtid, NULL);
						}
						if(ptrain!=NULL)
						{
							if(!ptrain->GetTrainInfoExpand_Const()->IsTrainYingyeSame(yy))
							{
								CTG_SingleTrain train=*ptrain;
								train.GetTrainInfoExpand()->UpdateTrainYingye(yy);
								
								CTG_ScheduleInfo schdinfo;	
								pdata->GetScheduleInfo(schdinfo); 
								pdata->UpdateTrain(schdinfo, train, SYCFLAG_NULL);
							}
							RefreshData();
						}
					}
				}
			}
		}
		else
		{
			if(!stricmp(token,TOKEN_15_ACK_KEDIAO_INFO) ||  !stricmp(token,TOKEN_16_ACK_YINGYE_INFO))
			{
				CTG_TrainScheduleBase *pdata=NULL;
				CWorkScheduleHolder *pholder=m_pDoc->GetTGDataHolder();
				if(m_nScheduleType==CTG_ScheduleInfo::SCHEDULE_TYPE_WORK) 
					pdata=pholder->GetWorkScheduleData();
				else
					if(m_nScheduleType==CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT) 
						pdata=pholder->GetShiftScheduleData();

				if(pdata)
				{
					CTG_SingleTrain *ptrain=TD_GetTrainInfoFromXML(pdata,content,token);
					if(ptrain) //发给服务器,但不更新界面
						ptrain->SetSyncStatusX(m_pDoc->GetServerCommInterface()->GetInterfaceFlag(),FALSE);
					RefreshData();
				}
			}
		}
	}
	FREE_UNI_MESSAGE_STRUCT(pMsgStruct);
	return 0;
}
void CPropertyPage_ShuaiGua::OnCbnSelchangeComboStationNameSubao()
{
	// TODO: 在此添加控件通知处理程序代码
	CComboBox* pWnd = (CComboBox*)GetDlgItem(IDC_COMBO_STATION_NAME_SUBAO);
	UpdateData(TRUE);
	m_nCurrentSelectRecordIndex=pWnd->GetCurSel();
}
