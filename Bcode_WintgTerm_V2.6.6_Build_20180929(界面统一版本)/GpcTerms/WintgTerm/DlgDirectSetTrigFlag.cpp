// DlgDirectSetTrigFlag.cpp : 实现文件
//

#include "stdafx.h"
#include "resource.h"
#include "DlgDirectSetTrigFlag.h"
#include <BaseMsg\clbth.h> 
#include "memshare.h"
#include "schd_tgmmi.h"


// CDlgDirectSetTrigFlag 对话框

IMPLEMENT_DYNAMIC(CDlgDirectSetTrigFlag, CDialog)
CDlgDirectSetTrigFlag::CDlgDirectSetTrigFlag(CWnd* pParent /*=NULL*/)
: CDialog(CDlgDirectSetTrigFlag::IDD, pParent)
{
	m_nModifyArriveOk=0;
	m_nModifyDepartOk=0;
	m_nTryCount=0;
}

CDlgDirectSetTrigFlag::~CDlgDirectSetTrigFlag()
{
}

void CDlgDirectSetTrigFlag::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgDirectSetTrigFlag, CDialog)
	ON_BN_CLICKED(IDC_BTN_SET_ARRIVE, OnBnClickedBtnSetArrive)
	ON_BN_CLICKED(IDC_BTN_SET_DEPART, OnBnClickedBtnSetDepart)
	ON_WM_DESTROY()
	ON_MESSAGE(UM_UNI_MESSAGE, OnCommMsg)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_WM_CLOSE()
END_MESSAGE_MAP()

LRESULT CDlgDirectSetTrigFlag::OnCommMsg(WPARAM wParam, LPARAM lParam) 
{
	UNI_MESSAGE_STRUCT *pMsgStruct=( UNI_MESSAGE_STRUCT *)lParam;
	CLBTH::MsgStPlanAutoTrigUpdateResult  *pMsg =  ( CLBTH::MsgStPlanAutoTrigUpdateResult *)pMsgStruct->pmsg;
	CTG_TrainRecord *prec=m_pTrain->GetTrainRecord(m_nRecIndex);
	if(pMsg->train_index==m_pTrain->GetTrainIndex() && pMsg->rec_index==m_nRecIndex)
	{
		if(pMsg->run_type==CLBTH::MsgStPlanAutoTrigUpdateResult::DEPART)
		{
			if(pMsg->trig_flag==1)
			{
				TG_SetAutoMakeDepartRoute(prec->m_data);
				sysprint(4,0,"根据自律机自触返回结果,修改 %s 次列车 %d 站发车进路允许自触",m_pTrain->GetTrainID(),prec->GetStationNo());
			}
			else
			{
				TG_SetNotAutoMakeDepartRoute(prec->m_data);
				sysprint(4,0,"根据自律机自触返回结果,修改 %s 次列车 %d 站发车进路不允许自触",m_pTrain->GetTrainID(),prec->GetStationNo());
			}
			m_nModifyDepartOk=2;
		}
		else if(pMsg->run_type==CLBTH::MsgStPlanAutoTrigUpdateResult::ARRIVE)
		{
			if(pMsg->trig_flag==1)
			{ 
				TG_SetAutoMakeArriveRoute(prec->m_data);
				sysprint(4,0,"根据自律机自触返回结果,修改 %s 次列车 %d 站接车进路允许自触",m_pTrain->GetTrainID(),prec->GetStationNo());
			}
			else
			{
				TG_SetNotAutoMakeArriveRoute(prec->m_data);	
				sysprint(4,0,"根据自律机自触返回结果,修改 %s 次列车 %d 站接车进路不允许自触",m_pTrain->GetTrainID(),prec->GetStationNo());
			}
			m_nModifyArriveOk=2;
		}	
		CString plan_res_str,route_res_str;
		if(pMsg->plan_res==CLBTH::MsgStPlanAutoTrigUpdateResult::PLAN_UPDATED)
			plan_res_str="计划更新成功";
		else
			plan_res_str="计划更新失败";

		if(pMsg->route_res==CLBTH::MsgStPlanAutoTrigUpdateResult::ROUTE_UPDATED)
			route_res_str="进路更新成功";
		else
			route_res_str="进路更新失败";

		m_szCommStatusStr=plan_res_str;
		m_szCommStatusStr+=";";
		m_szCommStatusStr+=route_res_str;
		RefreshData();
	}
	FREE_UNI_MESSAGE_STRUCT(pMsgStruct);
	if((m_nModifyArriveOk==0) || (m_nModifyArriveOk==2) || (m_nModifyDepartOk==0) || (m_nModifyDepartOk==2))
		OnOK();
	return 0;
}

void CDlgDirectSetTrigFlag::OnDestroy()
{
	UnRegisterMsgProcHandleWnd(GetSafeHwnd(),MSGID_MsgStPlanAutoTrigUpdateResult);
	CDialog::OnDestroy();
}

BOOL CDlgDirectSetTrigFlag::OnInitDialog()
{
	CDialog::OnInitDialog();
	RegisterMsgProcHandleWnd(GetSafeHwnd(),MSGID_MsgStPlanAutoTrigUpdateResult,0);

	RefreshData();
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CDlgDirectSetTrigFlag::SetTrain(CWintgViewMMI   *pmmi,CTG_SingleTrain *ptrain,int rec_index)
{
	m_pTGMMI=pmmi;
	m_pTrain=ptrain;
	m_nRecIndex=rec_index;
}
// CDlgDirectSetTrigFlag 消息处理程序

void CDlgDirectSetTrigFlag::RefreshData()
{
	GetDlgItem(IDC_ED_TRAINID)->SetWindowText("");
	GetDlgItem(IDC_ED_STATION)->SetWindowText("");
	GetDlgItem(IDC_ED_ARRIVE_TRIG)->SetWindowText("");
	GetDlgItem(IDC_ED_DEPART_TRIG)->SetWindowText("");
	if(!m_pTrain)
		return;
	CTG_TrainRecord *prec=m_pTrain->GetTrainRecord(m_nRecIndex);
	if(!prec)
		return;
	CString cap;
	cap.Format("%s/%s",prec->GetArriveTID(),prec->GetDepartTID());
	GetDlgItem(IDC_ED_TRAINID)->SetWindowText(cap);
	cap.Format("%s",gpcdef.GetStationName(prec->GetStationNo()));
	GetDlgItem(IDC_ED_STATION)->SetWindowText(cap);
	if(TG_IsAutoMakeArriveRoute(prec->m_data))
	{
		GetDlgItem(IDC_ED_ARRIVE_TRIG)->SetWindowText("允许自触");
		GetDlgItem(IDC_BTN_SET_ARRIVE)->SetWindowText("设置接车不允许自触");
	}
	else
	{
		GetDlgItem(IDC_ED_ARRIVE_TRIG)->SetWindowText("不允许自触");
		GetDlgItem(IDC_BTN_SET_ARRIVE)->SetWindowText("设置接车允许自触");
	}
	if(TG_IsAutoMakeDepartRoute(prec->m_data))
	{
		GetDlgItem(IDC_ED_DEPART_TRIG)->SetWindowText("允许自触");
		GetDlgItem(IDC_BTN_SET_DEPART)->SetWindowText("设置发车不允许自触");
	}
	else
	{
		GetDlgItem(IDC_ED_DEPART_TRIG)->SetWindowText("不允许自触");
		GetDlgItem(IDC_BTN_SET_DEPART)->SetWindowText("设置发车允许自触");
	}
	GetDlgItem(IDC_ED_COMM_STATUS)->SetWindowText(m_szCommStatusStr);
}


void CDlgDirectSetTrigFlag::OnBnClickedBtnSetArrive()
{
	CTime tm=CTime::GetCurrentTime();

	CLBTH::MsgStPlanAutoTrigUpdate msg;
	CTG_ScheduleInfo  schd;
	m_pTGMMI->GetScheduleInfo(schd);
	CTG_TrainRecord *prec=m_pTrain->GetTrainRecord(m_nRecIndex);
	if(!prec)
		return;

	TIME curTime=tm.GetTime();
	TIME arlTime=prec->GetArriveTime();
	if(arlTime>curTime && arlTime > curTime+3*3600)
	{
		CString str;
		if(TG_IsAutoMakeArriveRoute(prec->m_data))
		{
			TG_SetNotAutoMakeArriveRoute(prec->m_data);
			GetDlgItem(IDC_ED_COMM_STATUS)->SetWindowText("超过3小时不下发自律机, 直接修改计划为 接车进路不允许自触,请及时下计划!");
			
			str.Format("车次 %s %s 在当前时间3小时外,不下发自律机,直接修改计划为接车进路不允许自触,请及时下达阶段计划!", 
				m_pTrain->GetTrainID(),gpcdef.GetStationName(prec->GetStationNo()));
		}
		else
		{
			TG_SetAutoMakeArriveRoute(prec->m_data);
			GetDlgItem(IDC_ED_COMM_STATUS)->SetWindowText("超过3小时不下发自律机, 直接修改计划为 接车进路允许自触,请及时下计划!");
			
			str.Format("车次 %s %s 在当前时间3小时外,不下发自律机,直接修改计划为接车进路允许自触,请及时下达阶段计划!", 
				m_pTrain->GetTrainID(),gpcdef.GetStationName(prec->GetStationNo()));

		}

		sysprint(4,0,"%s", str);
		MessageBox(str, "警告", MB_OK);
		OnOK();
	}
	else
	{
		GPCENTITY entityInfo;
		if(!gpcdef.GetEntityInfo(EQUIP_LIRC, prec->GetStationNo(), entityInfo))
		{
			CString stmp;
			stmp.Format("配置错误, can not get entityID for sta %d!", prec->GetStationNo());
			MessageBox(stmp);
			return;
		}
		msg.con_id=schd.m_nSectionID;
		msg.gpc=GetLocalEntityID();
		msg.train_index=m_pTrain->GetTrainIndex();
		msg.rec_index=m_nRecIndex;                       // 记录索引
		msg.station=prec->GetStationNo();                // 车站站号
		msg.run_type=CLBTH::MsgStPlanAutoTrigUpdate::ARRIVE;
		if(TG_IsAutoMakeArriveRoute(prec->m_data))
		{
			msg.trig_flag=0;
			sysprint(4,0,"调度员发送申请，申请修改 %s次列车在 %s 站接车进路不允许自触",m_pTrain->GetTrainID(),gpcdef.GetStationName(prec->GetStationNo()));
		}
		else
		{
			msg.trig_flag=1;
			sysprint(4,0,"调度员发送申请，申请修改 %s次列车在 %s 站接车进路允许自触",m_pTrain->GetTrainID(),gpcdef.GetStationName(prec->GetStationNo()));
		}
		SendBMsg(&msg,entityInfo.nEntityId,0);

		GetDlgItem(IDC_ED_COMM_STATUS)->SetWindowText("正在与自律机通信,请稍候!");
		m_nModifyArriveOk=1;
	}
}

void CDlgDirectSetTrigFlag::OnBnClickedBtnSetDepart()
{
	CTime tm=CTime::GetCurrentTime();

	CLBTH::MsgStPlanAutoTrigUpdate msg;
	CTG_ScheduleInfo  schd;
	m_pTGMMI->GetScheduleInfo(schd);
	CTG_TrainRecord *prec=m_pTrain->GetTrainRecord(m_nRecIndex);
	if(!prec)
		return;

	TIME curTime=tm.GetTime();
	TIME depTime=prec->GetDepartTime();
	if(depTime>curTime && depTime > curTime+3*3600)
	{
		CString str;
		if(TG_IsAutoMakeDepartRoute(prec->m_data))
		{
			TG_SetNotAutoMakeDepartRoute(prec->m_data);
			GetDlgItem(IDC_ED_COMM_STATUS)->SetWindowText("超过3小时不下发自律机, 直接修改计划为 发车进路不允许自触!");
			
			str.Format("车次 %s %s 在当前时间3小时外,不下发自律机,直接修改计划为发车进路不允许自触,请及时下达阶段计划!", 
				m_pTrain->GetTrainID(),gpcdef.GetStationName(prec->GetStationNo()));
		}
		else
		{
			TG_SetAutoMakeDepartRoute(prec->m_data);
			GetDlgItem(IDC_ED_COMM_STATUS)->SetWindowText("超过3小时不下发自律机, 直接修改计划为 发车进路允许自触!");
			
			str.Format("车次 %s %s 在当前时间3小时外,不下发自律机,直接修改计划为发车进路允许自触,请及时下达阶段计划!", 
				m_pTrain->GetTrainID(),gpcdef.GetStationName(prec->GetStationNo()));
		}

		sysprint(4,0,"%s", str);
		MessageBox(str, "警告", MB_OK);
		OnOK();
	}
	else
	{
		GPCENTITY entityInfo;
		if(!gpcdef.GetEntityInfo(EQUIP_LIRC, prec->GetStationNo(), entityInfo))
		{
			CString stmp;
			stmp.Format("配置错误, can not get entityID for sta %d!", prec->GetStationNo());
			MessageBox(stmp);
			return;
		}
		msg.con_id=schd.m_nSectionID;
		msg.gpc=GetLocalEntityID();
		msg.train_index=m_pTrain->GetTrainIndex();
		msg.rec_index=m_nRecIndex;                       // 记录索引
		msg.station=prec->GetStationNo();                // 车站站号
		msg.run_type=CLBTH::MsgStPlanAutoTrigUpdate::DEPART;
		if(TG_IsAutoMakeDepartRoute(prec->m_data))
		{
			msg.trig_flag=0;
			sysprint(4,0,"调度员发送申请，申请修改 %s次列车在 %s 站发车进路不允许自触",m_pTrain->GetTrainID(),gpcdef.GetStationName(prec->GetStationNo()));
		}
		else
		{
			msg.trig_flag=1;
			sysprint(4,0,"调度员发送申请，申请修改 %s次列车在 %s 站发车进路允许自触",m_pTrain->GetTrainID(),gpcdef.GetStationName(prec->GetStationNo()));
		}
		SendBMsg(&msg,entityInfo.nEntityId,0);
		
		GetDlgItem(IDC_ED_COMM_STATUS)->SetWindowText("正在与自律机通信,请稍候!");
		m_nModifyDepartOk=1;
	}
}

void CDlgDirectSetTrigFlag::OnBnClickedCancel()
{
	OnClose();
}

void  CDlgDirectSetTrigFlag::OnClose()
{
	if(m_nModifyArriveOk==1 || m_nModifyDepartOk==1)
	{
		m_nTryCount++;
		if(m_nTryCount>3)
		{
			CString str;
			str.Format("请注意:你已经重试了3次,但仍未收到自触设置回执,请联系维护人员!");
			sysprint(4,0,"%s", str);
			MessageBox(str);
			if(m_nModifyArriveOk==2 || m_nModifyDepartOk==2)
				OnOK();
			else
				OnCancel();
		}
		else
		{
			CString str,str1,str2;
			if(m_nModifyArriveOk==1)
				str1="接车";
			if(m_nModifyDepartOk==1)
				str2="发车";
			str.Format("尚未收到自律机:%s %s 自触设置回执,请稍候!", str1, str2);
			sysprint(4,0,"%s", str);
			MessageBox(str);
		}
	}
	else
	{
		if(m_nModifyArriveOk==2 || m_nModifyDepartOk==2)
			OnOK();
		else
			OnCancel();
	}
}


