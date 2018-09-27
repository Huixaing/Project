// DlgElecArms.cpp : 实现文件
//

#include "stdafx.h"
#include "resource.h"
#include "DlgElecArms.h"
#include "DlgElecArmsConfirm.h"
#include "inifile.h"
#include ".\dlgelecarms.h"
#include <memshare.h>
#include "..\BaseMsg\msg_def.h"
#include <BaseMsg\clbth.h>
#include "widgets.h"

extern bool SendTGBMsg(int section_id,CLBTH::BaseMsg* pOneMsg,long dest_entity,short flag=0);

int CMyListBox::CompareItem(LPCOMPAREITEMSTRUCT lpCompareItemStruct) 
{
   ASSERT(lpCompareItemStruct->CtlType == ODT_LISTBOX);
   LPCTSTR lpszText1 = (LPCTSTR) lpCompareItemStruct->itemData1;
   ASSERT(lpszText1 != NULL);
   LPCTSTR lpszText2 = (LPCTSTR) lpCompareItemStruct->itemData2;
   ASSERT(lpszText2 != NULL);

   return strcmp(lpszText2, lpszText1);
}

void CMyListBox::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct) 
{
   ASSERT(lpMeasureItemStruct->CtlType == ODT_LISTBOX);
   LPCTSTR lpszText = (LPCTSTR) lpMeasureItemStruct->itemData;
   ASSERT(lpszText != NULL);
   CSize   sz;
   CDC*    pDC = GetDC();

   sz = pDC->GetTextExtent(lpszText);

   ReleaseDC(pDC);

   lpMeasureItemStruct->itemHeight = 2*sz.cy;
}

void CMyListBox::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
   ASSERT(lpDrawItemStruct->CtlType == ODT_LISTBOX);
   LPCTSTR lpszText = (LPCTSTR) lpDrawItemStruct->itemData;
   ASSERT(lpszText != NULL);
   CDC dc;

   
   dc.Attach(lpDrawItemStruct->hDC);

   // Save these value to restore them when done drawing.
   COLORREF crOldTextColor = dc.GetTextColor();
   COLORREF crOldBkColor = dc.GetBkColor();

   COLORREF crBack,crBackSelect;
   crBack=crOldBkColor;
   crBackSelect=::GetSysColor(COLOR_HIGHLIGHT);
   if(*lpszText=='+')
   {
      crBack=RGB(0,255,0);
      crBackSelect=RGB(128,0,0);
   }
   
   // If this item is selected, set the background color 
   // and the text color to appropriate values. Also, erase
   // rect by filling it with the background color.
   if ((lpDrawItemStruct->itemAction | ODA_SELECT) &&
      (lpDrawItemStruct->itemState & ODS_SELECTED))
   {
      dc.SetTextColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));
      dc.SetBkColor(::GetSysColor(COLOR_HIGHLIGHT)); // crBackSelect);
      dc.FillSolidRect(&lpDrawItemStruct->rcItem,crBackSelect);
         // ::GetSysColor(COLOR_HIGHLIGHT));
   }
   else
      dc.FillSolidRect(&lpDrawItemStruct->rcItem, crBack);

   // If this item has the focus, draw a red frame around the
   // item's rect.
   if ((lpDrawItemStruct->itemAction | ODA_FOCUS) &&
      (lpDrawItemStruct->itemState & ODS_FOCUS))
   {
      CBrush br(RGB(255, 0, 0));
      dc.FrameRect(&lpDrawItemStruct->rcItem, &br);
   }

   // Draw the text.
   dc.DrawText(
      lpszText,
      strlen(lpszText),
      &lpDrawItemStruct->rcItem,
      DT_LEFT|DT_SINGLELINE|DT_VCENTER);

   // Reset the background color and the text color back to their
   // original values.
   dc.SetTextColor(crOldTextColor);
   dc.SetBkColor(crOldBkColor);

   dc.Detach();
}


// CDlgElecArms 对话框
#define IDC_LIST_ARMS    101

IMPLEMENT_DYNAMIC(CDlgElecArms, CDialog)
CDlgElecArms::CDlgElecArms(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgElecArms::IDD, pParent)
	, m_bSendToStation(FALSE)
	, m_nStationIndex(0)
{
	m_nArmCount=0;
	m_nSectionID=0;
	m_szTreeRoot[0]=0;
}

CDlgElecArms::~CDlgElecArms()
{

}

void CDlgElecArms::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//DDX_Control(pDX, IDC_CBO_STAT_LIST, m_cboStationList);
	DDX_Check(pDX, IDC_CHK_SEND_TO_STATION, m_bSendToStation);
	//DDX_CBIndex(pDX, IDC_CBO_STAT_LIST, m_nStationIndex);
}


BEGIN_MESSAGE_MAP(CDlgElecArms, CDialog)
	ON_BN_CLICKED(ID_SET_ENABLE, OnBnClickedSetEnable)
	ON_BN_CLICKED(ID_SET_DISABLE, OnBnClickedSetDisable)
	ON_BN_CLICKED(IDC_CANCEL, OnBnClickedCancel)
	ON_WM_NCDESTROY()
	ON_BN_CLICKED(IDC_REFRESH_REMOTE, OnBnClickedRefreshRemote)
	ON_NOTIFY(TVN_SELCHANGED, IDC_LIST_ARMS, OnTvnSelchangedListArms)
	ON_WM_DESTROY()
	ON_MESSAGE(UM_UNI_MESSAGE, OnCommMsg)
END_MESSAGE_MAP()

LRESULT CDlgElecArms::OnCommMsg(WPARAM wParam, LPARAM lParam) 
{
	 UNI_MESSAGE_STRUCT *pMsgStruct=( UNI_MESSAGE_STRUCT *)lParam;

	 if (pMsgStruct->msg_id == MSGID_MsgElecArmAnswer)
	 {
	 	CLBTH::MsgElecArmAnswer  *pArm =  ( CLBTH::MsgElecArmAnswer *)pMsgStruct->pmsg;
	 
	 	if(pArm->status==CLBTH::MsgElecArmAnswer::ELEC_ENABLE)
			m_treeArms.SetObjStateByRelaValue(pArm->elec_arm_id*10000+pArm->elec_sta,3);
		else
			m_treeArms.SetObjStateByRelaValue(pArm->elec_arm_id*10000+pArm->elec_sta,1);
	 }
	 FREE_UNI_MESSAGE_STRUCT(pMsgStruct);
	 return 0;
}


BOOL  CDlgElecArms::LoadArmConfig(const char *file)
{
	CIniFile ini;
	char  key[32];
	char  content[256];
	char  section[32];
	char seps[]=" ,\t\\/=\n";
	memset(m_aArmConfigs, 0, sizeof(m_aArmConfigs));
	do
	{
	m_nArmCount=0;
	memset(m_szTreeRoot,0,sizeof(m_szTreeRoot));
	ini.OpenFile(file);
	if(!ini.IsOK())
		break;
	if(m_nSectionID==0)
		strcpy(section,"ARMS");
	else
		sprintf(section,"ARMS_%d",m_nSectionID);
	if(!ini.GotoSection(section))
		return FALSE;

	while(ini.GetContent(key,content,sizeof(content)) && m_nArmCount<MAX_ARM_NUM)
	{
		if(!stricmp(key,"TREEROOT"))
		{
			strncpy(m_szTreeRoot,content,sizeof(m_szTreeRoot)-1);
		}
		else
		{
			m_aArmConfigs[m_nArmCount].nArmNo=atoi(key);
			m_aArmConfigs[m_nArmCount].nStatus=0;
			m_aArmConfigs[m_nArmCount].nbAgree=0;
			char* token = strtok(content, seps);
			if(token!=NULL) 
			{
				char* token = strtok(NULL, seps);
				if(token!=NULL)
					m_aArmConfigs[m_nArmCount].nbAgree=atoi(token);
				else
					m_aArmConfigs[m_nArmCount].nbAgree=0;
			}
			m_nArmCount++;
		}
	}
	}while(0);
	ini.CloseFile(); 
	m_treeArms.LoadFromFile(file,m_szTreeRoot); 
	if(m_nArmCount>0)
		return TRUE;
	else
		return FALSE;
	return TRUE;
}

void CDlgElecArms::RefreshList()
{  

}


BOOL CDlgElecArms::OnInitDialog()
{
	CDialog::OnInitDialog();
	m_nStationCount=0;
	CRect rect;
	GetDlgItem(IDC_ARMS_RECT)->GetWindowRect(&rect); 
	ScreenToClient(&rect);
	rect.DeflateRect(2,2,2,2); 
	m_treeArms.Create(WS_VISIBLE|TVS_HASLINES|TVS_LINESATROOT|TVS_HASBUTTONS|TVS_SHOWSELALWAYS,rect,this,IDC_LIST_ARMS);

	CFont * pFont; 
	pFont = new CFont; 
	pFont->CreateFont(18, // nHeight 
		0, // nWidth 
		0, // nEscapement 
		0, // nOrientation 
		FW_BOLD,//FW_MEDIUM, // nWeight 
		FALSE, // bItalic 
		FALSE, // bUnderline 
		0, // cStrikeOut 
		ANSI_CHARSET, // nCharSet 
		OUT_DEFAULT_PRECIS, // nOutPrecision 
		CLIP_DEFAULT_PRECIS, // nClipPrecision 
		DEFAULT_QUALITY, // nQuality 
		DEFAULT_PITCH | FF_SWISS, // nPitchAndFamily 
		_T("宋体")); // lpszFac 

	m_treeArms.SetFont(pFont);   
	

	m_treeArms.Startup();
		 
	CButton *pButSetEnable = (CButton *)GetDlgItem(ID_SET_ENABLE);
	CButton *pButSetDisEnable = (CButton *)GetDlgItem(ID_SET_DISABLE);
	if(gpcdef.GetUserIdentify() == GPC_OPERATOR  || gpcdef.GetUserIdentify() == GPC_DISPATCHER || gpcdef.GetUserIdentify() ==GPC_TEST 
		||gpcdef.GetUserIdentify() == GPC_MAINTAIN_DISPATCH || gpcdef.GetUserIdentify() ==GPC_SUPER_OPER)
	{
		pButSetEnable->EnableWindow(TRUE);
		pButSetDisEnable->EnableWindow(TRUE);
	}
	else
	{
		pButSetEnable->EnableWindow(FALSE);
		pButSetDisEnable->EnableWindow(FALSE);

	}
 	RegisterMsgProcHandleWnd(GetSafeHwnd(),MSGID_MsgElecArmAnswer,0);

	delete pFont;
	pFont = NULL;
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CDlgElecArms::ChangeArmStatus(BOOL enable)
{
	UpdateData(TRUE);
	int sel; //=m_treeArms.GetSelectedRelaValue();
	int node_array[256]; // 最多256个节点
	int node_count=m_treeArms.GetSelectedSubTreeRelaValues(node_array,256); 
	if(node_count<=0)
	{
		MessageBox("没有选中设置对象!");
		return;
	}
	sel=node_array[0]; // 当前选中节点
	int stno,armno;
	stno=(sel % 10000);
	armno=(sel/10000);
	CString armname,cap;
	armname=m_treeArms.GetSelectedCaption();
	CString dest;
	GPCENTITY entityInfo;
	if(m_bSendToStation)
	{
		if(stno==0)
		{
			MessageBox("没有选中发送车站!");
			return;
		}
		char *strname=gpcdef.GetStationName(stno);
		dest=strname;
		if(!gpcdef.GetEntityInfo(EQUIP_LIRC, stno, entityInfo))
		{
			CString stmp;
			stmp.Format("配置错误, can not get entityID for sta %d!", stno);
			MessageBox(stmp);
			return;
		}
		node_count=1; // 直接发往车站只对当前节点(不发子节点)
	}
	else
	{
		dest="服务器";
		entityInfo.nEntityId =0; 
	}

	if(enable)
	{
		for(int i=0;i<node_count;i++)
		{
			int stationno=(node_array[i] % 10000);
			int elecarmno=(node_array[i]/10000);
			if(stationno==0)
				continue;
			if(elecarmno>0)
			{
				for(int k=0; k<MAX_ARM_NUM; k++)
				{
					if((elecarmno == m_aArmConfigs[k].nArmNo) 
						&& ((m_aArmConfigs[k].nbAgree==2) || (m_aArmConfigs[k].nbAgree==1 && node_count>1)))
					{
						CString text;
						text = "安全提示: 请与供电调度确认　"+armname;
						text += "　已恢复供电";

						CDlgElecArmsConfirm dlg(text, this);
						if(dlg.DoModal() == IDCANCEL)
						{
							GpcLogTrainMessage(0,0,"供电臂操作: %s, 调度员选择　取消", text);
							return;
						}
						GpcLogTrainMessage(0,0,"供电臂操作: %s, 调度员选择　确定", text);
					}
				}
				break;
			}
		}
	}
	
	if(enable)
		cap.Format("确认把供电臂< %s >设置为有电状态吗, 目标: %s",armname,dest);
	else
		cap.Format("确认把供电臂< %s >设置为无电状态吗, 目标: %s",armname,dest);

	if(MessageBox(cap,"提示",MB_YESNOCANCEL)==IDYES)
	{
		for(int i=0;i<node_count;i++)
		{
			CLBTH::MsgElecArmSet  msg;
			stno=(node_array[i] % 10000);
			armno=(node_array[i]/10000);
			msg.elec_arm_id= armno;
			msg.elec_sta= stno;
			if(stno==0)
				continue;
			if(enable)
				msg.status = CLBTH::MsgElecArmSet::ELEC_ENABLE;
			else
				msg.status = CLBTH::MsgElecArmSet::ELEC_DISABLE;

			// 事件处理18: 设置电力臂状态
			GPCENTITY eventEntity;
			if(gpcdef.GetEntityInfo(136,0,0,eventEntity))
			{
				CLBTH::MsgEventData data;
				data.nSrcEntityID = GetLocalEntityID();
				data.tmEventTime = time(NULL);
				data.nEventRank = CLBTH::NORMAL_EVENT;
				data.nEventID =  CLBTH::EVENT_GPC_ELE_ARM;
				data.SetKey(CLBTH::KeyEleArm, armno);
				data.SetKey(CLBTH::KeyEleArmStatus, msg.status);

				SendBMsg(&data, eventEntity.nEntityId);
			}

			CLBTH::Archive ar;
			if(msg.serialize(ar))
			{
				if (0 == entityInfo.nEntityId)
				{
					int dest_entity = GetSpecialEntityID(SERVER_TYPE_DATASTORE, 0);
					SendBMsg(&msg, dest_entity, 0);
					//SendTGBMsg(m_nSectionID, &msg, 0);
				}
				else
					SendCommMessage(entityInfo.nEntityId,0,(BYTE *)ar.getBuffer(), ar.getCount());

				OnBnClickedRefreshRemote();
			}
			if(enable)
				GpcLogTrainMessage(0,0,"供电臂操作: < %s >设置为有电状态,目标:%s",armname,dest);
			else
				GpcLogTrainMessage(0,0,"供电臂操作: < %s >设置为无电状态,目标:%s",armname,dest);
		}
	}
}

void CDlgElecArms::OnBnClickedSetEnable()
{
	ChangeArmStatus(TRUE);
}

void CDlgElecArms::OnBnClickedSetDisable()
{
	ChangeArmStatus(FALSE);
}

void CDlgElecArms::OnBnClickedCancel()
{
     ShowWindow(SW_HIDE);
}

void CDlgElecArms::OnNcDestroy()
{
	CDialog::OnNcDestroy();

}

void CDlgElecArms::OnBnClickedRefreshRemote()
{
	CLBTH::MsgElecArmQuery	 msg;
	msg.query_source=CLBTH::MsgElecArmQuery::GPC; 
	msg.elec_arm_count= m_nArmCount;
	msg.elec_sta=0;
	for(int i=0;i<m_nArmCount;i++)
	{
		msg.elec_arm_list[i]=m_aArmConfigs[i].nArmNo;
	}

	int dest_entity = GetSpecialEntityID(SERVER_TYPE_DATASTORE, 0);
	SendBMsg(&msg, dest_entity, 0);
	GpcLogTrainMessage(0,0,"向entity %d 请求电力臂状态", dest_entity);
	//SendTGBMsg(m_nSectionID, &msg, 0);
}

void CDlgElecArms::SetSectionID(int nSectionId,BOOL readonly)
{
	m_bReadOnly=readonly;
	m_nStationIndex=-1;
	m_bSendToStation=FALSE;
	if(m_bReadOnly)
	{
		GetDlgItem(ID_SET_ENABLE)->EnableWindow(FALSE);
		GetDlgItem(ID_SET_DISABLE)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHK_SEND_TO_STATION)->EnableWindow(FALSE);
	}
	else
	{
		GetDlgItem(ID_SET_ENABLE)->EnableWindow(TRUE);
		GetDlgItem(ID_SET_DISABLE)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHK_SEND_TO_STATION)->EnableWindow(TRUE);
	}
	UpdateData(FALSE);
	if(m_nSectionID!= nSectionId)
	{
	  m_nSectionID=nSectionId;
  	  m_nStationCount=sizeof(m_aStationList) / sizeof(m_aStationList[0]); 
      gpcdef.GetStationList(nSectionId,m_nStationCount,m_aStationList);    
	  
	   char filename[128];
	   sprintf(filename,"%s\\%s",gpcdef.GetDataDir(),"elecarms.cfg");
	   LoadArmConfig(filename);
	   RefreshList();
	}
}

void CDlgElecArms::OnTvnSelchangedListArms(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
}

void CDlgElecArms::OnDestroy()
{
	CDialog::OnDestroy();
}

// 外部SCADA设置电力供电情况
void CDlgElecArms::ReceiveMsgElecArmSet(CLBTH::MsgElecArmSet  *pArm)
{		
	CIniFile ini;
	char  key[32];
	char  content[256];
	char  section[32];
	char filename[128];
	sprintf(filename,"%s\\%s",gpcdef.GetDataDir(),"elecarms.cfg");

	int status = 1;
	CString name = "停止供电";
	if (pArm->status==CLBTH::MsgElecArmSet::PREP_ENABLE) 
	{
		status = 3;
		name = "恢复供电";
	}
	else if (pArm->status==CLBTH::MsgElecArmSet::PREP_DISABLE) 
	{
		status = 1;
		name = "停止供电";
	}
	else
	{
		return;
	}

	CString strArmName;
	ini.OpenFile(filename);
	if(ini.IsOK())
	{
		if(m_nSectionID==0)
			strcpy(section,"ARMS");
		else
			sprintf(section,"ARMS_%d",m_nSectionID);
		if(ini.GotoSection(section))
		{
			while(ini.GetContent(key,content,sizeof(content)))
			{
				if(!stricmp(key,"TREEROOT"))
				{
					;
				}
				else
				{
					int nArmNo=atoi(key);
					if(pArm->elec_arm_id==nArmNo)
					{
						strArmName = content;
						break;
					}
				}
			}
		}
	}
	ini.CloseFile();

	if(strArmName=="")
	{
		CString text;
		text.Format("接收到未知供电臂号%d 站号%d 状态%d", pArm->elec_arm_id, pArm->elec_sta, pArm->status);
		AfxMessageBox(text);
	}
	else
	{
		CString text = "安全提示: 接受到自动通知 <"+ strArmName + "> " + name +  ", 请确认!";
		CDlgElecArmsConfirm dlg(text, this);			
		if(dlg.DoModal() == IDOK) 
		{
			GpcLogTrainMessage(0,0,"供电臂操作: %s, 调度员选择　确定", text);
			// 发给服务器,由服务器分发到各站
			if(status==3)
				SendArmSetMsg(pArm->elec_arm_id, strArmName, pArm->elec_sta, TRUE);
			else
				SendArmSetMsg(pArm->elec_arm_id, strArmName, pArm->elec_sta, FALSE);
		}
		else
		{
			GpcLogTrainMessage(0,0,"供电臂操作: %s, 调度员选择　取消", text);
		}
	}
}

void CDlgElecArms::SendArmSetMsg(int armno, CString& armname, short stno, BOOL enable)
{
	CString dest = "服务器";
	int dest_entity = GetSpecialEntityID(SERVER_TYPE_DATASTORE, 0);
	CString cap;
	if(enable)
		cap.Format("确认把供电臂< %s >设置为有电状态吗, 目标: 服务器",armname,dest);
	else
		cap.Format("确认把供电臂< %s >设置为无电状态吗, 目标: 服务器",armname,dest);

	GpcLogTrainMessage(0,0,"%s", cap);
	if(MessageBox(cap,"提示",MB_YESNO)==IDYES)
	{
		GpcLogTrainMessage(0,0,"调度员选择确定");

		CLBTH::MsgElecArmSet  msg;
		msg.elec_arm_id= armno;
		msg.elec_sta= stno;
		if(enable)
			msg.status = CLBTH::MsgElecArmSet::ELEC_ENABLE;
		else
			msg.status = CLBTH::MsgElecArmSet::ELEC_DISABLE;

		// 事件处理18: 设置电力臂状态
		GPCENTITY eventEntity;
		if(gpcdef.GetEntityInfo(136,0,0,eventEntity))
		{
			CLBTH::MsgEventData data;
			data.nSrcEntityID = GetLocalEntityID();
			data.tmEventTime = time(NULL);
			data.nEventRank = CLBTH::NORMAL_EVENT;
			data.nEventID =  CLBTH::EVENT_GPC_ELE_ARM;
			data.SetKey(CLBTH::KeyEleArm, armno);
			data.SetKey(CLBTH::KeyEleArmStatus, msg.status);

			SendBMsg(&data, eventEntity.nEntityId);
		}

		CLBTH::Archive ar;
		if(msg.serialize(ar))
		{
			if (0 == dest_entity)
			{
				dest_entity = GetSpecialEntityID(SERVER_TYPE_DATASTORE, 0);
				SendBMsg(&msg, dest_entity, 0);
			}
			else
				SendCommMessage(dest_entity, 0, (BYTE *)ar.getBuffer(), ar.getCount());

			OnBnClickedRefreshRemote();
		}
		if(enable)
			GpcLogTrainMessage(0,0,"供电臂操作: < %s >设置为有电状态,目标:%s(%d)",armname,dest, dest_entity);
		else
			GpcLogTrainMessage(0,0,"供电臂操作: < %s >设置为无电状态,目标:%s(%d)",armname,dest, dest_entity);
	}
	else
	{
		GpcLogTrainMessage(0,0,"调度员选择取消");
	}
}