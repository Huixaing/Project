// QueryPhaseStoreTrain.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "QueryPhaseStoreTrain.h"
#include "messages.h"
#include "function.h" 
#include "oscalls.h"
#include "sysshare.h"
#include "QueryStationCurrentStoreTrain.h"
#include "PhaseStoreTrainSelect.h"  ////保存选择对话框

#include <baseMsg\msgid.h>
#include "db_msgproc.h"
#include "tgmsg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CString GetStationText(struct _tagStationStoreTrain *stationInfo);
#define SIDESTORECARRIAGECOLUMNS		7

int _gnSideStoreCarriageFmt2[SIDESTORECARRIAGECOLUMNS] = 
{
	LVCFMT_LEFT, LVCFMT_LEFT,LVCFMT_LEFT
};


//占内存很大!,不可在stack中
static struct _tagStationStoreTrain sectionStoreCar[64]; 


void SetStartEndtime(BYTE flag, int m_nShiftID, int& nEnd_time, int&nStart_time)
{
	int id,CurYear,CurMonth,CurDay,CurBanbie;

	id=m_nShiftID;
	CurYear  = id/1000000;
	CurMonth = id/10000-CurYear*100;
	CurDay   = id/100-(id/10000)*100;
	CurBanbie=id-(id/10)*10;	 

	long usCurrentDate;
	if(flag == CLBTH::MsgSimpleStationCarCtrl::STACAR_PHASE_CURRENT)      // 1 case 1 :现存车
	{
		nStart_time = 0;

		nEnd_time = time(NULL);

	}
	else if(flag == CLBTH::MsgSimpleStationCarCtrl::STACAR_PHASE_0)  // 0 case 2 :阶段存车：第一班
	{
		usCurrentDate = (CurYear- 1990) * 380 + (CurMonth - 1) * 31 + CurDay - 1;
		nStart_time = nEnd_time = long_time(usCurrentDate,0*3600);  //date,time;	    
	}
	else if (flag == CLBTH::MsgSimpleStationCarCtrl::STACAR_PHASE_6) //6  case 3: 阶段存车：
	{
		if(CurBanbie == 2)  //第二班
		{	
			usCurrentDate = (CurYear- 1990) * 380 + (CurMonth - 1) * 31 + CurDay - 1;
			nStart_time = nEnd_time = long_time(usCurrentDate,6*3600);  //date,time;
		}
		else                //第一班
		{
			usCurrentDate = (CurYear- 1990) * 380 + (CurMonth - 1) * 31 + CurDay - 1;
			nStart_time = nEnd_time   = long_time(usCurrentDate,6*3600);
		}	
	}
	else if (flag == CLBTH::MsgSimpleStationCarCtrl::STACAR_PHASE_12) // 12 case 4  :阶段存车：
	{
		usCurrentDate = (CurYear- 1990) * 380 + (CurMonth - 1) * 31 + CurDay - 1;
		nStart_time = nEnd_time = long_time(usCurrentDate,12*3600);  //date,time;
	}
	else if (flag == CLBTH::MsgSimpleStationCarCtrl::STACAR_PHASE_18) //18 case 5 :阶段存车：
	{
		if(CurBanbie == 2)   
		{
			usCurrentDate = (CurYear- 1990) * 380 + (CurMonth - 1) * 31 + CurDay - 1;
			nStart_time = nEnd_time   = long_time(usCurrentDate,18*3600);
		}
		else               
		{
			usCurrentDate = (CurYear- 1990) * 380 + (CurMonth - 1) * 31 + CurDay - 1;
			nStart_time = nEnd_time  = long_time(OsGetPrevDate(PMTG.ThreadId,usCurrentDate),18*3600);  //date,time;
		}	
	}
}

/////////////////////////////////////////////////////////////////////////////
// CQueryPhaseStoreTrain dialog
CQueryPhaseStoreTrain::CQueryPhaseStoreTrain(CTGBasicParam *pBasicPara,int nConID,int nShiftid,BOOL readonly,CWnd* pParent /*=NULL*/)
: CDialog(CQueryPhaseStoreTrain::IDD, pParent)
{
	// NOTE: the ClassWizard will add member initialization here
	m_pBasicPara = pBasicPara;
	m_nSectionID=nConID;
	m_nShiftID=nShiftid;
	m_bReadOnly=readonly;
	m_nCurrentSelectStation=0;
	m_nCurrentPhase = CLBTH::MsgSimpleStationCarCtrl::STACAR_PHASE_CURRENT;//1;
	nReceiveStatus = -1;
}


void CQueryPhaseStoreTrain::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CQueryPhaseStoreTrain)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CQueryPhaseStoreTrain, CDialog)
	//{{AFX_MSG_MAP(CQueryPhaseStoreTrain)
	ON_BN_CLICKED(IDC_SAVE, OnSave)
	ON_BN_CLICKED(IDC_CURRENT, OnCurrent)
	ON_BN_CLICKED(IDC_PHASE1, OnPhase1)
	ON_BN_CLICKED(IDC_PHASE2, OnPhase2)
	ON_BN_CLICKED(IDC_PHASE3, OnPhase3)
	ON_BN_CLICKED(ID_BUTTONPRINTSINGLE, OnButtonprintsingle)
	ON_BN_CLICKED(ID_BUTTONPRINTALL, OnButtonprintall)
	
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_PST_REFRESH, OnPstRefresh)
	ON_BN_CLICKED(ID_BUTTONPRINTWHOLE, OnButtonprintwhole)
	ON_BN_CLICKED(ID_BUTTON_SAVE, OnButtonSave)
	//}}AFX_MSG_MAP
	ON_MESSAGE(UM_UNI_MESSAGE, OnCommMsg)
END_MESSAGE_MAP()



/////////////////////////////////////////////////////////////////////////////
// CQueryPhaseStoreTrain message handlers
#define SECTIONSTORETRAINCOLUMNS		4
_TCHAR *_gszSectionStoreTrainLabel[SECTIONSTORETRAINCOLUMNS] =
{
	_T("车站"), _T("更新时间"),_T("现存车信息"),_T("总数")
};


int _gnSectionStoreTrainFmt[SECTIONSTORETRAINCOLUMNS] = 
{
	LVCFMT_LEFT, LVCFMT_LEFT,LVCFMT_LEFT,LVCFMT_LEFT
};

int _gnSectionStoreTrainWidth[SECTIONSTORETRAINCOLUMNS] = 
{
	110,200,750,50
};


LRESULT CQueryPhaseStoreTrain::OnCommMsg(WPARAM wParam, LPARAM lParam) 
{
	UNI_MESSAGE_STRUCT *pMsgStruct=( UNI_MESSAGE_STRUCT *)lParam;
	if(pMsgStruct->msg_id==MSGID_MsgSimpleStationCarCtrl)
	{
		ProcessMessage(( CLBTH::MsgSimpleStationCarCtrl *)pMsgStruct->pmsg);
	}
	else if(pMsgStruct->msg_id==MSGID_MsgSimpleStationCarFormat)
	{
		ProcessMessage(( CLBTH::MsgSimpleStationCarFormat *)pMsgStruct->pmsg);
	}

	FREE_UNI_MESSAGE_STRUCT(pMsgStruct);
	return 0;
}
void CQueryPhaseStoreTrain::OnOK() 
{
	SendMessage(WM_CLOSE);
}
void CQueryPhaseStoreTrain::SelectStation(int st)
{  
	//所选中的车站的序号;
	m_nCurrentSelectStation = st;
	CQueryStationCurrentStoreTrain* dlg = 
		new CQueryStationCurrentStoreTrain(m_pBasicPara, m_nSectionID,m_nShiftID,m_nCurrentSelectStation,m_nCurrentPhase,m_bReadOnly);

	if(dlg != NULL)
	{
		//直接复制站存车数据，不用重新申请
		memcpy(&dlg->info,&sectionStoreCar[m_nCurrentSelectStation],sizeof(struct _tagStationStoreTrain));
		m_pBasicPara->ResetCarCount();
		if(!m_bReadOnly)
		{
			dlg->DoModal();

			if(dlg->bChangdedFlag)
			{
				memcpy(&sectionStoreCar[m_nCurrentSelectStation],&(dlg->info),sizeof(struct _tagStationStoreTrain));
				time_t timeTmp;
				struct tm *p;   
				time(&timeTmp);   
				p=localtime(&timeTmp);

				GetStationCarsInfo(nStationList[m_nCurrentSelectStation],m_nCurrentPhase);
			}
		}		
		else if(m_bReadOnly)
		{
			dlg->DoModal();
		}

		delete dlg;
	}
}

BOOL CQueryPhaseStoreTrain::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_ctrlSectionStoreTrainList.SubclassDlgItem(IDC_LIST,this);

	//head of side list  
	m_ctrlSectionStoreTrainList.SetHeaderLabel(SECTIONSTORETRAINCOLUMNS,_gszSectionStoreTrainLabel,
		_gnSectionStoreTrainWidth,_gnSectionStoreTrainFmt);

	CButton *radioWnd = (CButton *)GetDlgItem(IDC_CURRENT);
	if(m_bReadOnly)
	{
		CWnd *pWnd = GetDlgItem(IDC_SAVE);
		pWnd->EnableWindow(FALSE);
	}

	radioWnd->SetCheck(1);

	// initiate login shiftid
	int id,CurBanbie;
	id=m_nShiftID;


	CurBanbie=id-(id/10)*10;	 

	CWnd *pWnd1 = GetDlgItem(IDC_PHASE1);
	CWnd *pWnd2 = GetDlgItem(IDC_PHASE2);
	CWnd *pWnd3 = GetDlgItem(IDC_PHASE3);
	

	if(CurBanbie ==2)
	{   
		banbie = 2;
		pWnd1->SetWindowText("6点");
		pWnd2->SetWindowText("12点");
		pWnd3->SetWindowText("18点");
	}
	else
	{   
		banbie = 1;
		pWnd1->SetWindowText("18点");
		pWnd2->SetWindowText("0点");
		pWnd3->SetWindowText("6点");
	}
	

	CString text,text2;
	text = "11"; text2 = "22";

	nStationNum=sizeof(nStationList)/sizeof(nStationList[0]);  
	gpcdef.GetSectionPhaseStoreStationList(m_nSectionID,nStationNum,nStationList);

	LV_ITEM lvi;
	lvi.mask = LVIF_TEXT;
	lvi.iSubItem = 0;

	for(int i=0;i<nStationNum;i++)
	{
		lvi.iItem = i;
		lvi.pszText = (char *)(LPCTSTR)gpcdef.GetStationName(nStationList[i]);
		m_ctrlSectionStoreTrainList.InsertItem(&lvi); 
	}

	//初始化起始行号，索引为0
	for(i=0;i<nStationNum;i++)
		m_StdLineNo[i] = i;

	// 初始化股道信息
	for(int i=0;i<nStationNum&&i<64;i++)
	{
		memset(&sectionStoreCar[i],0x00,sizeof(struct _tagStationStoreTrain));
	}

	RegisterMsgProcHandleWnd(GetSafeHwnd(),MSGID_MsgSimpleStationCarCtrl,0);
	RegisterMsgProcHandleWnd(GetSafeHwnd(),MSGID_MsgSimpleStationCarFormat,0);
	GetStationCarsInfo(0,m_nCurrentPhase);

	InitDlgPosition(this); 

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CQueryPhaseStoreTrain::PreTranslateMessage(MSG* pMsg) 
{
	bool flag = FALSE;
	if(pMsg->message == WM_LBUTTONDBLCLK)
	{
		do 
		{
			if(pMsg->hwnd != m_ctrlSectionStoreTrainList.GetSafeHwnd( ))
				break;

			CPoint point;
			point.x = (pMsg->lParam) & 0x0000ffff;
			point.y = ((pMsg->lParam) & 0xffff0000) >> 16;
			UINT uFlags = 0;
			int nHitItem = m_ctrlSectionStoreTrainList.HitTest(point, &uFlags);
			int stdno = ConvertSelIndex2StationNO(nHitItem);

			if(stdno > 0)
			{
				m_nCurrentSelStaIndex = GetStationidInList(stdno);
				SelectStation(m_nCurrentSelStaIndex);
				flag = true;
			}
			else
			{
				AfxMessageBox("请选中一个车站后双击");
			}

		}while(0);
	}
	if(pMsg->message == WM_LBUTTONDOWN  )
	{
		do 
		{
			if(pMsg->hwnd != m_ctrlSectionStoreTrainList.GetSafeHwnd( ))
				break;

			CPoint point;
			point.x = (pMsg->lParam) & 0x0000ffff;
			point.y = ((pMsg->lParam) & 0xffff0000) >> 16;
			UINT uFlags = 0;
			int nHitItem = m_ctrlSectionStoreTrainList.HitTest(point, &uFlags);
			int stdno = ConvertSelIndex2StationNO(nHitItem);
			m_nCurrentSelStaIndex = GetStationidInList(stdno);


		}while(0);
	}

	if(flag) 
		return TRUE;

	return CDialog::PreTranslateMessage(pMsg);
}

void CQueryPhaseStoreTrain::SaveAllStationCar(WORD new_phase)
{
	GpcLogSysMessage(0,"Begin 令存为全部站站存车记录");


	SetStartEndtime(new_phase, m_nShiftID, nEnd_time, nStart_time);

	CLBTH::MsgSimpleStationCarConvert  mMsgSimpleStationCarConvert;
	mMsgSimpleStationCarConvert.from_phase =(CLBTH::MsgSimpleStationCarConvert::STACAR_PHASE) m_nCurrentPhase;
	mMsgSimpleStationCarConvert.to_phase = (CLBTH::MsgSimpleStationCarConvert::STACAR_PHASE)new_phase;


	mMsgSimpleStationCarConvert.to_input_time = nStart_time;

	mMsgSimpleStationCarConvert.sta_count = 1;
	long eid=GetSpecialEntityID(SERVER_TYPE_DB,m_nSectionID);
	for(int i=0;i<nStationNum&&i<64; i++)
	{
		mMsgSimpleStationCarConvert.from_input_time = sectionStoreCar[i].EditTime;	
		mMsgSimpleStationCarConvert.sta_list[0] = nStationList[i];	 

		GpcLogSysMessage(0,"from_phase: %d, to_phase: %d, from_input_time: %d, to_input_time: %d, sta: %d", 
			mMsgSimpleStationCarConvert.from_phase,
			mMsgSimpleStationCarConvert.to_phase,
			mMsgSimpleStationCarConvert.from_input_time,
			mMsgSimpleStationCarConvert.to_input_time,
			mMsgSimpleStationCarConvert.sta_list[0]);
		SendTGBMsg(m_nSectionID,&mMsgSimpleStationCarConvert,eid);
	}

	GpcLogSysMessage(0,"End 令存为全部站站存车记录, m_nSectionID %d, Send to Entity %d", m_nSectionID, eid);
}

// save current data to phase data 
void CQueryPhaseStoreTrain::OnSave() 
{
	CLBTH::MsgSimpleStationCarConvert  mMsgSimpleStationCarConvert;
	int nNum; time_t old_time,new_time;
	nNum = 1;
	CPhaseStoreTrainSelect dlg(banbie);
	if(dlg.DoModal() == IDCANCEL)
		return;
	if(dlg.m_nPhase == -1 || dlg.m_nPhase == CLBTH::MsgSimpleStationCarConvert::STACAR_PHASE_CURRENT)
		return;
	ushort new_phase = dlg.m_nPhase;

	if(m_ctrlSectionStoreTrainList.GetCurSel() <0)
	{
		SaveAllStationCar(new_phase);
		return;
	}

	mMsgSimpleStationCarConvert.from_phase =(CLBTH::MsgSimpleStationCarConvert::STACAR_PHASE)m_nCurrentPhase;
	mMsgSimpleStationCarConvert.to_phase = (CLBTH::MsgSimpleStationCarConvert::STACAR_PHASE)new_phase;

	int nSel =  m_nCurrentSelStaIndex;
	if(nSel < 0)
		return;
	old_time = sectionStoreCar[nSel].EditTime;	

	mMsgSimpleStationCarConvert.from_input_time = old_time;	

	SetStartEndtime(new_phase, m_nShiftID, nEnd_time, nStart_time);
	new_time = nStart_time;
	mMsgSimpleStationCarConvert.to_input_time = new_time;

	mMsgSimpleStationCarConvert.sta_count = nNum;
	GpcLogSysMessage(0,"Begin 令存为单一站站存车记录");
	for(int i=0;i<nNum; i++)
	{
		int m = ConvertSelIndex2StationNO(m_ctrlSectionStoreTrainList.GetCurSel());
		TRACE("Current Selected station no:%d \n",m);
		mMsgSimpleStationCarConvert.sta_list[i] =  (ushort)ConvertSelIndex2StationNO(m_ctrlSectionStoreTrainList.GetCurSel());

		GpcLogSysMessage(0,"from_phase: %d, to_phase: %d, from_input_time: %d, to_input_time: %d, sta: %d", 
			mMsgSimpleStationCarConvert.from_phase,
			mMsgSimpleStationCarConvert.to_phase,
			mMsgSimpleStationCarConvert.from_input_time,
			mMsgSimpleStationCarConvert.to_input_time,
			mMsgSimpleStationCarConvert.sta_list[i]);
	}


	long eid=GetSpecialEntityID(SERVER_TYPE_DB,m_nSectionID);
	SendTGBMsg(m_nSectionID,&mMsgSimpleStationCarConvert,eid);	
	GpcLogSysMessage(0,"End 令存为单一站站存车记录, m_nSectionID %d, Send to Entity %d", m_nSectionID, eid);

}
void CQueryPhaseStoreTrain::GetStationCarsInfo(ushort StationIndex,byte type)
{
	CLBTH::MsgSimpleStationCarQuery mMsgSimpleStationCarQuery;

	SetStartEndtime(type, m_nShiftID, nEnd_time, nStart_time);
	mMsgSimpleStationCarQuery.begin_time = nStart_time;
	mMsgSimpleStationCarQuery.end_time = nEnd_time;
	mMsgSimpleStationCarQuery.phase = (CLBTH::MsgSimpleStationCarQuery::STACAR_PHASE)type;
	//only one station
	if(StationIndex != 0)
	{
		mMsgSimpleStationCarQuery.sta_count = 1;
		mMsgSimpleStationCarQuery.sta_list[0] = StationIndex;
	}
	else
	{
		mMsgSimpleStationCarQuery.sta_count = nStationNum;
		for(int i=0;i<nStationNum;i++)
			mMsgSimpleStationCarQuery.sta_list[i] = nStationList[i];
	}

	long eid=GetSpecialEntityID(SERVER_TYPE_DB,m_nSectionID);
	SendTGBMsg(m_nSectionID,&mMsgSimpleStationCarQuery,eid);

	GpcLogSysMessage(0,"向服务器entity=%d, 申请phase %d 站存车", eid, mMsgSimpleStationCarQuery.phase);
}
//数据开始，结束标志
BOOL CQueryPhaseStoreTrain::ProcessMessage(CLBTH::MsgSimpleStationCarCtrl *pBuf)
{
	do{	
		pBuf->input_time;
		pBuf->station;
		if(m_nCurrentPhase !=pBuf->phase)
			break;
		switch(pBuf->phase)
		{
		case CLBTH::MsgSimpleStationCarCtrl::STACAR_PHASE_0:
			break;
		case CLBTH::MsgSimpleStationCarCtrl::STACAR_PHASE_6:
			break;
		case CLBTH::MsgSimpleStationCarCtrl::STACAR_PHASE_12:
			break;
		case CLBTH::MsgSimpleStationCarCtrl::STACAR_PHASE_18:
			break;
		case CLBTH::MsgSimpleStationCarCtrl::STACAR_PHASE_CURRENT:
			break;			
		}
		if(pBuf->status == CLBTH::MsgSimpleStationCarCtrl::MSG_BEGIN)
		{
			int id = GetStationidInList(pBuf->station);	
			if(id < 0 || id >=64) return FALSE;
			memset(&sectionStoreCar[id],0x00,sizeof(struct _tagStationStoreTrain));		 
			sectionStoreCar[id].usStationNo = pBuf->station;  
			nReceiveStatus = 0;
		}
		else if(pBuf->status == CLBTH::MsgSimpleStationCarCtrl::MSG_END)
		{
			nReceiveStatus = 1;
			ShowStationInfoMultiLines(pBuf->station);
		}

	}while(0);
	return TRUE;
}
//接收存车数据
BOOL CQueryPhaseStoreTrain::ProcessMessage(CLBTH::MsgSimpleStationCarFormat *pBuf)
{
	//站存车信息均发送给该窗口，如果双击了某个站，那么记得要把该消息转发给当前弹出的对话框
	if(pBuf == NULL)
		return TRUE;
	
	//1 phase
	if(m_nCurrentPhase !=pBuf->phase)
	{
		GpcLogTrainMessage(0,0,"Receive data, but type is not same m_nCurrentPhase=%d type=%d",m_nCurrentPhase,pBuf->phase);
		return TRUE;
	}        
	//2 station
	ushort usStationNo = pBuf->station;
	int id = GetStationidInList(usStationNo);	
	if(id < 0 || id >=64) return FALSE;

	sectionStoreCar[id].usStationNo = usStationNo;     
	//3 input time
	sectionStoreCar[id].EditTime = pBuf->input_time;
	//find if this side data exists 
	int nValue = IsSideInfoExist(usStationNo,pBuf->side_name);
	int nSideIndex=0;
	//不存在该股道的信息
	if(nValue < 0)
	{
		nSideIndex = sectionStoreCar[id].usSideNum;
		sectionStoreCar[id].usSideNum++;
	}
	else
		nSideIndex = nValue;

	//4 发送序号
	_tagSideStoredTrain *sideinfo;
	sideinfo = &sectionStoreCar[id].sideinfo[nSideIndex];
	//5 side number
	sideinfo->usSideNo = pBuf->side;
	//6 side name
	memset(sideinfo->pszSideName, 0, sizeof(sideinfo->pszSideName));
	strncpy(sideinfo->pszSideName,pBuf->side_name, sizeof(pBuf->side_name)-1);
	sideinfo->usRecNum ++ ;            //该股道车组的纪录数。一条记录可能有多个车厢

	int j = sideinfo->usRecNum -1;
	if(j>=64) return FALSE;
	//7,8,9,10
	sideinfo->rec[j].usCarriageExplain = pBuf->car_flag;          //说明信息
	sideinfo->rec[j].usCarriageType = pBuf->car_type;       //车厢类型
	sideinfo->rec[j].usCarriageNum = pBuf->car_count;        //车厢数目，
	CString remark ;
	remark.Format("%s",pBuf->destine);
	//根据^拆分
	int pos = remark.Find("^");
	if(pos < 0)
	{
		strncpy(sideinfo->rec[j].lpszCarriageRemark, pBuf->destine,sizeof(sideinfo->rec[j].lpszCarriageRemark));  //车厢去向，如上海	
		sideinfo->rec[j].lpszCarriageApply[0] = '\0';
	}
	else
	{			
		CString apply;
		apply = remark.Left(pos);
		strcpy(sideinfo->rec[j].lpszCarriageApply,(char*)(LPCTSTR)apply);

		apply = remark.Mid(pos+1);
		strcpy(sideinfo->rec[j].lpszCarriageRemark,(char*)(LPCTSTR)apply);     //车厢去向，如上海	 					
	}

	return TRUE;
}

int CQueryPhaseStoreTrain::GetStationidInList(int nStationNo)
{ 
	if(nStationNo <=0) return -1;
	int i=0;
	for(i=0;i<nStationNum;i++)
	{
		if(nStationList[i] == nStationNo)
			return i;
	}
	//not found this station in list;
	if(i == nStationNum)
		return -1;

	return i;
}


void CQueryPhaseStoreTrain::OnPstRefresh() 
{
	int type =-1;
	CButton*pBtn=NULL;
	pBtn=(CButton*)GetDlgItem(IDC_CURRENT);
	if(pBtn && type<0)
	{
		if(pBtn->GetCheck()==1)
			type=0;
	}

	pBtn=(CButton*)GetDlgItem(IDC_PHASE1);
	if(pBtn && type<0)
	{
		if(pBtn->GetCheck()==1)
			type=1;
	}

	pBtn=(CButton*)GetDlgItem(IDC_PHASE2);
	if(pBtn && type<0)
	{
		if(pBtn->GetCheck()==1)
			type=2;
	}

	pBtn=(CButton*)GetDlgItem(IDC_PHASE3);
	if(pBtn && type<0)
	{
		if(pBtn->GetCheck()==1)
			type=3;
	}

	if(type>=0 && type<=3)
		SetCurrentPhase(type);

	return;
}

void CQueryPhaseStoreTrain::OnCurrent() 
{
	SetCurrentPhase(0);
}

void CQueryPhaseStoreTrain::OnPhase1() 
{
	SetCurrentPhase(1);
}

void CQueryPhaseStoreTrain::OnPhase2() 
{
	SetCurrentPhase(2);
}

void CQueryPhaseStoreTrain::OnPhase3() 
{
	SetCurrentPhase(3);	
}
void CQueryPhaseStoreTrain::SetCurrentPhase(int index ) 
{
	//clear all items;
	int itemcount,j;
	itemcount = m_ctrlSectionStoreTrainList.GetItemCount();
	for(j=itemcount-1;j>=nStationNum; j--)
	{
		m_ctrlSectionStoreTrainList.DeleteItem(j);
	}

	for(j=0; j<nStationNum; j++)
	{
		m_StdLineNo[j] = j;
		m_ctrlSectionStoreTrainList.SetItemText(j,0,gpcdef.GetStationName(nStationList[j]));
		m_ctrlSectionStoreTrainList.SetItemText(j,1,"");
		m_ctrlSectionStoreTrainList.SetItemText(j,2,"");
		m_ctrlSectionStoreTrainList.SetItemText(j,3,"");

		memset(&sectionStoreCar[j],0x00,sizeof(struct _tagStationStoreTrain));
	}

	// 1
	if(index ==0)
	{ 
		m_nCurrentPhase = CLBTH::MsgSimpleStationCarCtrl::STACAR_PHASE_CURRENT;//1;	

		GetStationCarsInfo(0,m_nCurrentPhase);
		return;
	}

	CTime t= CTime::GetCurrentTime(); //////
	int nHour = t.GetHour();

	int id,CurBanbie;
	id=m_nShiftID;
	CurBanbie=id-(id/10)*10;	 

	if(CurBanbie ==2)
	{   
		if(index == 1 )
			m_nCurrentPhase = CLBTH::MsgSimpleStationCarCtrl::STACAR_PHASE_6;//6;
		else if(index == 2 )
			m_nCurrentPhase = CLBTH::MsgSimpleStationCarCtrl::STACAR_PHASE_12;//12;
		else if(index == 3 )
			m_nCurrentPhase = CLBTH::MsgSimpleStationCarCtrl::STACAR_PHASE_18;//18;
	}
	else 
	{   
		if(index == 1 )
			m_nCurrentPhase = CLBTH::MsgSimpleStationCarCtrl::STACAR_PHASE_18;//18;
		else if(index == 2 )
			m_nCurrentPhase = CLBTH::MsgSimpleStationCarCtrl::STACAR_PHASE_0;//0;
		else if(index == 3 )
			m_nCurrentPhase = CLBTH::MsgSimpleStationCarCtrl::STACAR_PHASE_6;//6;
	}
	//send request info 

	GetStationCarsInfo(0,m_nCurrentPhase);

}

unsigned long CQueryPhaseStoreTrain::GetTimeIdx()
{
	CTime t=CTime::GetCurrentTime();
	CString result;
	result.Format("%d",t);
	return  atoi(result);
}

void CQueryPhaseStoreTrain::OnButtonprintsingle() 
{
	int curSel = m_ctrlSectionStoreTrainList.GetCurSel();

	if(curSel<0)
	{
		AfxMessageBox("请先选择一个车站!");
		return;
	}
	int stdno = ConvertSelIndex2StationNO(curSel);
	CString stdname;
	stdname.Format("%s",gpcdef.GetStationName(stdno));

	CString title;
	title = "打印"+stdname+"车站站存车？";
	if(AfxMessageBox(title,MB_YESNO | MB_DEFBUTTON1 |MB_ICONQUESTION) != IDYES)
		return; 

	PrintSingleStation(stdno);
}

void CQueryPhaseStoreTrain::OnButtonprintall() 
{
	// TODO: Add your control notification handler code here
	if(AfxMessageBox("打印所有车站站存车？",MB_YESNO | MB_DEFBUTTON1 |MB_ICONQUESTION) != IDYES)
		return; 

	for(int i=0; i<nStationNum; i++)
	{
		if(!PrintSingleStation(nStationList[i]))
			return;
	}

}

/***********************************************************************
*Purpose:在发现要显示的股道信息宽度大于列的宽度，我们要在把数据分行显示
*
*input:int stationno 要显示的车站站号
*
*output:none
*
*return:BOOL TRUE成功/FALSE失败
*
***********************************************************************/

BOOL CQueryPhaseStoreTrain::ShowStationInfoMultiLines(int stationno)
{
	m_criticalShowStdInfo.Lock();//避免多个消息同时进入下面代码，可能导致显示不正确
	int stdindex;
	stdindex = GetStationidInList(stationno);//这个id可以用来获取在显示框中的行号和对应数组中的数据
	
	if(stdindex<0 || stdindex>=nStationNum)
	{
		m_criticalShowStdInfo.Unlock();
		return FALSE;
	}

	struct  _tagStationStoreTrain* cur_std_train;
	struct _tagSideStoredTrain *cur_side_train;
	struct _tagSideStoreCarriage *cur_carriage;

	//重新显示该站的显示股道信息
	CString text,tmp;		
	CString strStyle;
	int carriagecount;

	//从当前站开始，重新排列其和其后的所有车站的信息
	for(int stdid = stdindex; stdid < nStationNum; stdid++)
	{
		cur_std_train = &sectionStoreCar[stdid];

		text  = "";

		carriagecount = 0;
		//获取要显示的股道信息，和成要显示的字符串
		for(int j=0;j < cur_std_train->usSideNum;j++)
		{  
			int nSideCarriageCount=0;
			cur_side_train = &cur_std_train->sideinfo[j]; 

			for(int k=0; k < cur_side_train->usRecNum; k++)
			{   
				cur_carriage = &cur_side_train->rec[k];
				carriagecount+=cur_carriage->usCarriageNum;
				if(cur_carriage->usCarriageNum ==0)
					continue;
				strStyle = "";
				BYTE nSel = 0;
				BYTE nSubSel = 0;
				m_pBasicPara->GetCarTypeAndKindIndex(cur_carriage->usCarriageType, nSel, nSubSel);

				if(nSel<m_pBasicPara->m_carKindType.size())	 
				{
					strStyle = m_pBasicPara->m_carKindType[nSel].shortName;
					if(strStyle.IsEmpty())
						strStyle = m_pBasicPara->m_carKindType[nSel].name;

					text += strStyle;

					if(cur_carriage->usCarriageExplain>=m_pBasicPara->m_aryWorkExplain.GetCount())
						cur_carriage->usCarriageExplain = 0;

					CString strCarriageRemark = cur_carriage->lpszCarriageRemark;
					CString strCarriageApply = cur_carriage->lpszCarriageApply;
					strCarriageRemark.Trim();
					strCarriageApply.Trim();

					if(strCarriageRemark.IsEmpty() && strCarriageApply.IsEmpty()) 
					{
						tmp.Format("%s%s%d, ",
							m_pBasicPara->m_aryWorkExplain[cur_carriage->usCarriageExplain],
							m_pBasicPara->m_carKindType[nSel].carKind[nSubSel].shortName,
							cur_carriage->usCarriageNum);
					}
					else
					{
						tmp.Format("%s%s%d(%s %s), ",
							m_pBasicPara->m_aryWorkExplain[cur_carriage->usCarriageExplain],
							m_pBasicPara->m_carKindType[nSel].carKind[nSubSel].shortName,
							cur_carriage->usCarriageNum,
							strCarriageRemark,
							strCarriageApply);
					}
				}

				text += tmp;
			}	 
		}
		//合成股道显示字符串结束

		//计算文本宽度，获得行数
		int cur_std_lineno,col_width,text_width,line_num;
		cur_std_lineno = m_StdLineNo[stdid];
		//col_width = m_ctrlSectionStoreTrainList.GetColumnWidth(2);
		col_width = _gnSectionStoreTrainWidth[2] - 200;
		text_width = m_ctrlSectionStoreTrainList.GetDC()->GetTextExtent(text).cx;

		if(text_width%col_width == 0)
			line_num = text_width/col_width;
		else
			line_num = text_width/col_width + 1;

		if(line_num == 0)//没有股道数据的情况，显示只有车站名字的一行
			line_num = 1;


		//排列本车站的内容
		CString stdstr,timestr,sidestr,carriagestr;
		int col_char_num;//每行的字符数，最后一行的字符数可能会有差别
		int i = 0;

		col_char_num = text.GetLength()/line_num;

		//加入空白行用来分割车站
		line_num++;


		//判断是否需要增加行数
		int row_dif = 0;
		if(m_ctrlSectionStoreTrainList.GetItemCount() < cur_std_lineno + line_num)//行数不够
		{
			row_dif = (cur_std_lineno + line_num) - m_ctrlSectionStoreTrainList.GetItemCount();
		}

		//增加行数
		int cur_rows_count = m_ctrlSectionStoreTrainList.GetItemCount();
		for(i = cur_rows_count; i < cur_rows_count + row_dif; i++)
		{
			LV_ITEM lvi;
			lvi.mask = LVIF_TEXT;
			lvi.iItem = i;
			lvi.iSubItem = 0;
			lvi.pszText = "";
			m_ctrlSectionStoreTrainList.InsertItem(&lvi);
		}

		//插入数据	
		int pre_chinese_splited = 0;//上一行汉字被分割
		int  cur_chinese_splited = 0;//当前行汉字被分割
		for(i=0; i<line_num; i++)
		{
			if(i==0 && text.IsEmpty())//只有一个空行的情况，不显示时间和股道信息
			{				
				stdstr.Format("%s",gpcdef.GetStationName(nStationList[stdid]));
				timestr = "";
				carriagestr = "";
			}
			else if(i==0 && !text.IsEmpty())//股道有车，且是第一行，显示车站名称和更新时间
			{
				stdstr.Format("%s",gpcdef.GetStationName(nStationList[stdid]));

				CTime t=cur_std_train->EditTime;
				timestr = t.Format( "%Y-%m-%d, %H:%M" );			

				carriagestr.Format("%d",carriagecount);
			}
			else//有多行数据，其他行不显示车站名称和更新时间
			{
				stdstr = "";
				timestr = "";
				carriagestr = "";
			}

			if(pre_chinese_splited)
				sidestr = text.Mid(i*col_char_num-1,col_char_num+1);
			else
				sidestr = text.Mid(i*col_char_num,col_char_num);

			cur_chinese_splited = GetChineseSplitFlag(sidestr);

			if(cur_chinese_splited)
			{
				sidestr = sidestr.Left(sidestr.GetLength()-1);
			}

			pre_chinese_splited = cur_chinese_splited;

			//最后一行为加的多余行
			if(i == line_num -1)
			{
				sidestr.Empty();
			}
			m_ctrlSectionStoreTrainList.SetItemText(cur_std_lineno+i,0,stdstr);
			m_ctrlSectionStoreTrainList.SetItemText(cur_std_lineno+i,1,timestr);
			m_ctrlSectionStoreTrainList.SetItemText(cur_std_lineno+i,2,sidestr);
			m_ctrlSectionStoreTrainList.SetItemText(cur_std_lineno+i,3,carriagestr);

		}

		//判断当前车站所占的行数是否发生了改变
		//如果发生了改变，那么要重新排列后面的所有车站
		//否则，排列自己这个车站即可

		int next_std_line_no,line_dif;
		BOOL rowschanged = FALSE;

		if(stdid >= nStationNum)//最后一个车站，不必进行重新排列
			rowschanged = FALSE;
		else//查找下一个车站原来的行号，进行比较
		{
			next_std_line_no = cur_std_lineno + line_num;
			if(next_std_line_no != m_StdLineNo[stdid+1])
			{
				rowschanged = TRUE;
				line_dif = next_std_line_no - m_StdLineNo[stdid+1];
			}
		}

		//发生了改变，那么要重新排列后面的所有车站
		if(rowschanged)
		{
			//先改变保存行号的那个数组
			for(i=stdid+1; i<nStationNum; i++)
			{
				m_StdLineNo[i] += line_dif;
			}
		}
		else
		{
			//第一个要改变的车站行数没有发生变化，那么后面数据可以不必改变
			if(stdid == stdindex)
				break;
		}
	}

	m_ctrlSectionStoreTrainList.Invalidate();
		
	m_criticalShowStdInfo.Unlock();
	return TRUE;

}

/***********************************************************************
*
*Purpose:把选中的列表框中的索引号转换成车站号
*
*intput:int selIndex，在列表框中的索引号
*
*output:none
*
*return:int 车站号
*
***********************************************************************/
int CQueryPhaseStoreTrain::ConvertSelIndex2StationNO(int selIndex)
{
	if(selIndex < 0)
		return -1;

	int staionindex = 0;
	for(int i=0; i<nStationNum; i++)
	{
		if(m_StdLineNo[i] == selIndex)
		{
			staionindex = i;
			break;
		}		
		else if(m_StdLineNo[i] > selIndex)
		{
			staionindex = i - 1 ;
			break;
		}
		else
			;
	}

	if(i>=nStationNum)
		return -1;
	return nStationList[staionindex];
}

//获取打印设备函数
extern BOOL GetPrinterDevice(LPTSTR pszPrinterName, HGLOBAL* phDevNames, HGLOBAL* phDevMode);

BOOL CQueryPhaseStoreTrain::PrintSingleStation(int stationNo)
{
	CWinApp* cApp=AfxGetApp();
	ASSERT_VALID(cApp);

	HGLOBAL hDevMode;
	HGLOBAL hDevNames;

	CString csPlotterName;
	csPlotterName.Format("%s",gpcdef.GetPrinterName());
	if (!GetPrinterDevice((char *)(LPCTSTR)csPlotterName, &hDevNames, &hDevMode))
	{
		AfxMessageBox(_T("无法获取指定的打印机设备\n"));
		return FALSE;
	}

	CPrintInfo pInfo;
	HDC prnDc;

	pInfo.m_pPD->m_pd.hDevMode = hDevMode;
	pInfo.m_pPD->m_pd.hDevNames = hDevNames;
	prnDc = pInfo.m_pPD->CreatePrinterDC();

	CDC printDC;
	printDC.Attach(prnDc);

	DOCINFO printDocumentName;
	CString sPrintName;
	sPrintName.Format("%s",gpcdef.GetStationName(stationNo));
	sPrintName = sPrintName + "车站现存车打印";

	char sPrintDocName[32];
	memcpy(sPrintDocName,sPrintName,32);

	printDocumentName.cbSize = sizeof(DOCINFO);
	printDocumentName.lpszDocName = sPrintDocName;
	printDocumentName.lpszOutput = NULL;
	printDC.StartDoc(&printDocumentName);
	printDC.StartPage( );

	OnPrint(&printDC,&pInfo,stationNo);

	printDC.EndPage( );

	printDC.EndDoc( );
	printDC.Detach( );
	DeleteDC(prnDc);

	GlobalFree(hDevNames);
	GlobalFree(hDevMode);
	return TRUE;
}

void CQueryPhaseStoreTrain::OnPrint(CDC *pDC, CPrintInfo *pInfo, int StationNo)
{
	CString disp_msg,station_name;
	station_name.Format("%s",gpcdef.GetStationName(StationNo));

	if(station_name.IsEmpty())
		return;

	CPen pen;
	pen.CreatePen(PS_SOLID,2,CLR_BLACK);
	CPen *pOldPen=pDC->SelectObject(&pen);

	CFont font,*oldfont;
	CPoint point;
	//打印标题
	CString ss=_T(station_name + "站现存车数统计");
	int nWidth=pDC->GetDeviceCaps(HORZRES);
	int nHeight=pDC->GetDeviceCaps(VERTRES);
	int ghdfx = pDC->GetDeviceCaps(LOGPIXELSX);
	int ghdfy = pDC->GetDeviceCaps(LOGPIXELSY);

	CRect DrawRect(0,0,nWidth,nHeight),rect,tempRect;		
	DrawRect.InflateRect(-100,-50,-100,-20);
	//pDC->Rectangle(&DrawRect);		
	//-MulDiv(21,-pDC->GetDeviceCaps(LOGPIXELSY),72),设置字体高度为21磅，一磅为1/72英寸
	//pDC->GetDeviceCaps(LOGPIXELSY)为每逻辑英寸所占像素
	font.CreateFont(-MulDiv(21,-pDC->GetDeviceCaps(LOGPIXELSY),72),
		0,0,0,FW_NORMAL,0,0,0,GB2312_CHARSET,
		OUT_STROKE_PRECIS,CLIP_STROKE_PRECIS,DRAFT_QUALITY,
		VARIABLE_PITCH|FF_SWISS,_T("黑体"));
	oldfont=pDC->SelectObject(&font);
	tempRect=DrawRect;
	tempRect.InflateRect(0,-5,0,0);

	rect=tempRect;
	rect.InflateRect(10,0);
	point=CPoint(DrawRect.left+(DrawRect.Width()-rect.Width())/2,rect.bottom+2);
	pDC->MoveTo(point);
	pDC->LineTo(point.x+rect.Width(),point.y);
	point.Offset(0,2);
	pDC->MoveTo(point);
	pDC->LineTo(point.x+rect.Width(),point.y);
	tempRect.right=DrawRect.right;
	pDC->DrawText(ss,&tempRect,DT_CENTER|DT_TOP|DT_SINGLELINE);	
	pDC->SelectObject(oldfont);
	font.DeleteObject();
	//显示统计的参数


	CString strStation="统计车站：";
	strStation +=station_name;

	long loginshiftid =  gpcdef.GetLoginShiftid();

	CString strDate ="班别: " + ConvertShiftIdToString(loginshiftid);
	CString temp;

	font.CreateFont(-MulDiv(12,-pDC->GetDeviceCaps(LOGPIXELSY),72),
		0,0,0,FW_BOLD,0,0,0,GB2312_CHARSET,
		OUT_STROKE_PRECIS,CLIP_STROKE_PRECIS,DRAFT_QUALITY,
		VARIABLE_PITCH|FF_SWISS,_T("宋体"));
	oldfont = pDC->SelectObject(&font);

	pDC->TextOut(rect.left+100,rect.top+250,strDate);
	pDC->TextOut(rect.left+100,rect.top+350,strStation);

	pDC->SelectObject(oldfont);

	CFont font_title;
	font_title.CreateFont(-MulDiv(11,-pDC->GetDeviceCaps(LOGPIXELSY),72),
		0,0,0,FW_NORMAL,0,0,0,GB2312_CHARSET,
		OUT_STROKE_PRECIS,CLIP_STROKE_PRECIS,DRAFT_QUALITY,
		VARIABLE_PITCH|FF_SWISS,_T("宋体"));
	CFont font_detail;
	font_detail.CreateFont(-MulDiv(8,-pDC->GetDeviceCaps(LOGPIXELSY),72),
		0,0,0,FW_NORMAL,0,0,0,GB2312_CHARSET,
		OUT_STROKE_PRECIS,CLIP_STROKE_PRECIS,DRAFT_QUALITY,
		VARIABLE_PITCH|FF_SWISS,_T("宋体"));

	//rect.InflateRect(-100,-630,-100,-200);
	rect.InflateRect(-100,-500,-100,-200);
	pDC->Rectangle(&rect);	     


	//列数为三列，设置列宽
	//第一列股道名字，第二列股道信息，第三列股道车辆数
	double first_col,second_col,third_col;
	int total_col_width;
	first_col = 0.1;
	second_col = 0.8;
	third_col = 0.1;
	total_col_width = abs(rect.right - rect.left);

	//计算该车站的股道所有信息所占的行数
	int stdindex;
	stdindex = GetStationidInList(StationNo);//这个id可以用来获取在对应数组中的数据

	struct  _tagStationStoreTrain* cur_std_train;
	struct _tagSideStoredTrain *cur_side_train;
	struct _tagSideStoreCarriage *cur_carriage;

	int jzCount,jzCountK,szCount,szCountK,axCount,axCountK;
	jzCount = jzCountK = szCount = szCountK = axCount = axCountK = 0;

	//定义存放股道车辆信息的变量
	CString sideArrayStr[64][3]; //3列，第一列股道名字，第二列股道信息，第三列股道车辆数
	int sideArrayNum = 0;
//	int nSel;
	CString strStyle,text,tmp;
//	BOOL bStyleFlag;

	cur_std_train = &sectionStoreCar[stdindex];

	//获取要显示的股道信息，和成要显示的字符串
	int i,j,k;
	for(j=0;j < cur_std_train->usSideNum;j++)
	{  
		cur_side_train = &cur_std_train->sideinfo[j];

		int nSideCarriageCount=0;
		text = "";
		tmp = "";

		for(k=0; k < cur_side_train->usRecNum; k++)
		{   
			cur_carriage = &cur_side_train->rec[k];
			if(cur_carriage->usCarriageNum <=0)
				continue;

			strStyle = "";

			BYTE nSel = 0;
			BYTE nSubSel = 0;
			m_pBasicPara->GetCarTypeAndKindIndex(cur_carriage->usCarriageType, nSel, nSubSel);

			if(nSel<m_pBasicPara->m_carKindType.size())	 
			{
				strStyle = m_pBasicPara->m_carKindType[nSel].shortName;
				if(strStyle.IsEmpty())
					strStyle = m_pBasicPara->m_carKindType[nSel].name;


				text += strStyle;

				if(CString(cur_carriage->lpszCarriageRemark) != ""){
					tmp.Format("%s%s%d(%s) ",
						m_pBasicPara->m_aryWorkExplain[cur_carriage->usCarriageExplain],
						m_pBasicPara->m_carKindType[nSel].carKind[nSubSel].shortName,
						cur_carriage->usCarriageNum,		
						cur_carriage->lpszCarriageRemark);
				}
				else
				{
					tmp.Format("%s%s%d ",
						m_pBasicPara->m_aryWorkExplain[cur_carriage->usCarriageExplain],
						m_pBasicPara->m_carKindType[nSel].carKind[nSubSel].shortName,
						cur_carriage->usCarriageNum);
				}
				text += tmp;
			}

			nSideCarriageCount +=  cur_carriage->usCarriageNum;
		}

		cur_side_train->CarriageCounts = nSideCarriageCount;   ///该股道的车厢数目
		//totalCount += cur_side_train->CarriageCounts;

		int side_row_num = 1;
		//该行长度超出了本列的宽度
		if(pDC->GetTextExtent(text).cx > total_col_width*second_col)
		{			
			if(pDC->GetTextExtent(text).cx % LONG(total_col_width*second_col) == 0)
				side_row_num = pDC->GetTextExtent(text).cx / LONG(total_col_width*second_col);
			else 
				side_row_num = pDC->GetTextExtent(text).cx / LONG(total_col_width*second_col) + 1;
		}
		if(side_row_num<=0)
			side_row_num = 1;

		int single_col_char;
		single_col_char = text.GetLength()/side_row_num;

		int pre_chinese_splited = 0;//上一行汉字被分割
		int  cur_chinese_splited = 0;//当前行汉字被分割
		CString splited_text;

		for(i = 0; i < side_row_num; i++)
		{
			if(i == 0)
			{
				sideArrayStr[sideArrayNum+i][0].Format("%s",cur_side_train->pszSideName);
				sideArrayStr[sideArrayNum+i][2].Format("%d",cur_side_train->CarriageCounts);
			}
			else
			{
				sideArrayStr[sideArrayNum+i][0].Empty();
				sideArrayStr[sideArrayNum+i][2].Empty();
			}

			splited_text = text.Mid(single_col_char*i,single_col_char);
			if(pre_chinese_splited)
				splited_text = text.Mid(i*single_col_char-1,single_col_char+1);
			else
				splited_text = text.Mid(i*single_col_char,single_col_char);

			cur_chinese_splited = GetChineseSplitFlag(splited_text);

			if(cur_chinese_splited)
			{
				splited_text = splited_text.Left(splited_text.GetLength()-1);
			}

			pre_chinese_splited = cur_chinese_splited;		

			sideArrayStr[sideArrayNum+i][1] = splited_text;
		}

		sideArrayNum += side_row_num;

	}



	//要打印的行数
	int row_count = sideArrayNum+1;
	CString side_name,side_name_next;
	side_name = "";
	side_name_next = "";

	int yStep =	150;//(rect.bottom-rect.top)/2/row_count;

	//自动调整行距
	while((row_count+8)*yStep > (rect.bottom - rect.top))
		yStep-=5;

	//实际行数还要加上标题拦这一行
	for(i=0; i<=row_count; i++)  //行数的绘制
	{			
		if(i>=2)//正式画股道的线，如果发现下面一行股道号为空，那么不画线
		{
			side_name = sideArrayStr[i-2][0];

			if(i < row_count)
				side_name_next = side_name = sideArrayStr[i-1][0];
			else
				side_name_next = "temp";

			if( side_name_next == "")  //该行股道号为空，表示为上面股道的信息太长而追加的行，不画线
				;
			else
			{
				pDC->MoveTo(rect.left,rect.top + yStep*i);
				pDC->LineTo(rect.right,rect.top + yStep*i);
			}
		}
		else //画标题拦的底线
		{
			pDC->MoveTo(rect.left,rect.top + yStep*i);
			pDC->LineTo(rect.right,rect.top + yStep*i);
		}


	}


	for(j=1; j<=4; j++)
	{
		if(j==1)	//可以不画，和矩形左边框重叠的那一条线
		{
			pDC->MoveTo(rect.left, rect.top);
			pDC->LineTo(rect.left, rect.top + yStep*row_count);
		}
		else if(j==2)
		{
			pDC->MoveTo(rect.left + first_col*(rect.right-rect.left), rect.top);
			pDC->LineTo(rect.left + first_col*(rect.right-rect.left), rect.top + yStep*row_count);
		}
		else if(j==3)
		{
			pDC->MoveTo(rect.left + (first_col+second_col)*(rect.right-rect.left), rect.top);
			pDC->LineTo(rect.left + (first_col+second_col)*(rect.right-rect.left), rect.top + yStep*row_count);
		}
		else
		{
			//	pDC->MoveTo(rect.left + (rect.right-rect.left), rect.top);
			//	pDC->LineTo(rect.left + (rect.right-rect.left), rect.top + (rect.bottom-rect.top)/2);
		}

	}

	//

	//	_TCHAR *_gszStationStoreTrainLabel[STATIONSTORETRAINCOLUMNS] =
	//{
	//	_T("股道"), _T("现在存车信息"),_T("数目"),
	//};

	//绘制标题栏文字

	CRect cellrect;
	int cell_left,cell_top,cell_right,cell_bottom;

	cell_left = rect.left;
	cell_top = rect.top + 15;
	cell_right = cell_left + first_col*(rect.right-rect.left);
	cell_bottom = rect.bottom + yStep;

	cellrect.SetRect(cell_left,cell_top,cell_right,cell_bottom);

	oldfont = pDC->SelectObject(&font_title);
	pDC->DrawText("股道",cellrect,DT_CENTER);

	cell_left = cell_right;
	//cell_top = rect.top + 15;
	cell_right = cell_left + second_col*(rect.right-rect.left);
	//cell_bottom = rect.bottom + yStep;

	cellrect.SetRect(cell_left,cell_top,cell_right,cell_bottom);
	pDC->DrawText("现存车信息",cellrect,DT_CENTER);

	cell_left = cell_right;
	//cell_top = rect.top + 15;
	cell_right = cell_left + third_col*(rect.right-rect.left);
	//cell_bottom = rect.bottom + yStep;

	cellrect.SetRect(cell_left,cell_top,cell_right,cell_bottom);
	pDC->DrawText("数目",cellrect,DT_CENTER);


	pDC->SelectObject(oldfont);



	int first_col_xpos,second_col_xpos,third_col_xpos;

	//显示内容
	row_count = row_count - 1; //数据的行数,row_count中以前包括了标题栏这一行

	first_col_xpos = rect.left + 20;
	second_col_xpos = first_col_xpos + first_col*(rect.right - rect.left);
	third_col_xpos = second_col_xpos + second_col*(rect.right - rect.left);

	CString side_content,side_cars_num;

	for(i=1; i<=row_count; i++)  
	{

		side_name = sideArrayStr[i-1][0];
		side_content = sideArrayStr[i-1][1];
		side_cars_num = sideArrayStr[i-1][2];

		pDC->TextOut(first_col_xpos, rect.top+yStep*i+15,side_name);
		pDC->TextOut(second_col_xpos, rect.top+yStep*i+15,side_content);
		pDC->TextOut(third_col_xpos, rect.top+yStep*i+15,side_cars_num);

	}

	//判断下面统计数据的起始位置

	int static_rect_top; 
	//原定的起始位置在股道信息这些内容里面，我们必须把起始位置往下面挪
	if((row_count+1)*yStep > (rect.bottom-rect.top)/2)	
		static_rect_top = rect.top + (row_count+1)*yStep + 50 ;
	else
		static_rect_top = rect.top + (rect.bottom-rect.top)/2 +100;

	//重新建立一个RECT，从原来矩形的中间部分开始
	CRect static_rect(rect.left,static_rect_top,rect.right,rect.bottom);

	//画行，yStep同上面的一样
	yStep = (static_rect.top + yStep*8 > static_rect.bottom)?((static_rect.bottom-static_rect.top)/8):yStep;


	for(i=0; i<=6; i++)  //行数的绘制，共6行7条横线
	{
		pDC->MoveTo(static_rect.left,static_rect.top + yStep*i);
		pDC->LineTo(static_rect.right,static_rect.top + yStep*i);

	}

	int xStep = (static_rect.right - static_rect.left)/7;
	//画列，xStep自己重新计算
	for(i=0; i<7; i++)  //列数的绘制，共6列7条竖线，最后一条术线已经画过，这儿不重画
	{
		pDC->MoveTo(static_rect.left + xStep*i,static_rect.top);
		pDC->LineTo(static_rect.left + xStep*i,static_rect.top + 6*yStep);
	}



	//绘制标题


	/*
	|------|-------|-------|-------|-------|--------|------|	<----标题栏
	|------|-------|-------|-------|-------|--------|------|
	|------|-------|-------|-------|-------|--------|------|
	|------|-------|-------|-------|-------|--------|------|	<----标题栏
	|------|-------|-------|-------|-------|--------|------|
	|------|-------|-------|-------|-------|--------|------|
	|------|-------|-------|-------|-------|--------|------|

	第一列为重车空车信息
	^	
	|
	|
	|
	*/



	for(i=1; i<=6; i++)
	{
		cellrect.SetRect(static_rect.left + xStep*i,static_rect.top+15,static_rect.left + xStep*(i+1),static_rect.top+yStep);
		//		pDC->DrawText(_gszCarriageType[i-1].lpszCarriageLongName ,cellrect,DT_CENTER);
	}

	for(i=1; i<=6; i++)
	{
		cellrect.SetRect(static_rect.left + xStep*i,static_rect.top+15+yStep*3,static_rect.left + xStep*(i+1),static_rect.top+yStep*4);
		//		pDC->DrawText(_gszCarriageType[i-1+6].lpszCarriageLongName,cellrect,DT_CENTER);
	}

	//_TCHAR *_gszCarriageStyle[ ] = {_T("空车"),_T("重车")};
	//绘制重车空车

	cell_left = static_rect.left;
	cell_top = static_rect.top + yStep + 15;
	cell_right = cell_left + xStep;
	cell_bottom = cell_top + yStep;
	cellrect.SetRect(cell_left,cell_top,cell_right,cell_bottom);

	pDC->DrawText("重车",cellrect,DT_CENTER);

	cell_top = cell_top + yStep;
	cell_bottom = cell_top + yStep;
	cellrect.SetRect(cell_left,cell_top,cell_right,cell_bottom);

	pDC->DrawText("空车",cellrect,DT_CENTER);

	cell_top = cell_top + 2*yStep;
	cell_bottom = cell_top + yStep;
	cellrect.SetRect(cell_left,cell_top,cell_right,cell_bottom);

	pDC->DrawText("重车",cellrect,DT_CENTER);

	cell_top = cell_top + yStep;
	cell_bottom = cell_top + yStep;
	cellrect.SetRect(cell_left,cell_top,cell_right,cell_bottom);

	pDC->DrawText("空车",cellrect,DT_CENTER);

	//填空车辆数	

	cell_left = static_rect.left;
	cell_top = static_rect.top + yStep + 15;
	cell_right = cell_left + xStep;
	cell_bottom = cell_top + yStep;



	CString tempstr;
	//for(i=1; i<=6; i++)
	//{
	//	cell_left = cell_left + xStep;
	//	cell_right = cell_left + xStep;

	//	cellrect.SetRect(cell_left,cell_top,cell_right,cell_bottom);

	//	switch(i)
	//	{
	//	case 1://篷车，
	//		tempstr.Format("%d",pCount);
	//		pDC->DrawText(tempstr,cellrect,DT_CENTER);
	//		break;;
	//	case 2://敞车
	//		tempstr.Format("%d",cCount);
	//		pDC->DrawText(tempstr,cellrect,DT_CENTER);
	//		break;
	//	case 3://平板车
	//		tempstr.Format("%d",pbCount);
	//		pDC->DrawText(tempstr,cellrect,DT_CENTER);
	//		break;
	//	case 4://油罐车
	//		tempstr.Format("%d",yCount);
	//		pDC->DrawText(tempstr,cellrect,DT_CENTER);
	//		break;
	//	case 5://水泥车
	//		tempstr.Format("%d",sCount);
	//		pDC->DrawText(tempstr,cellrect,DT_CENTER);
	//		break;
	//	case 6://保温车
	//		tempstr.Format("%d",bCount);
	//		pDC->DrawText(tempstr,cellrect,DT_CENTER);
	//		break;
	//	default:
	//		break;

	//	}

	//}


	//第一排空车
	//	m_pCount = m_cCount = m_pbCount = m_yCount = m_sCount = m_bCount = 0;
	cell_left = static_rect.left;
	cell_top = static_rect.top + 2*yStep + 15;
	cell_right = cell_left + xStep;
	cell_bottom = cell_top + yStep;
	for(i=1; i<=6; i++)
	{
		cell_left = cell_left + xStep;
		cell_right = cell_left + xStep;

		cellrect.SetRect(cell_left,cell_top,cell_right,cell_bottom);

		//switch(i)
		//{
		//case 1://篷车，
		//	tempstr.Format("%d",pCountK);
		//	pDC->DrawText(tempstr,cellrect,DT_CENTER);
		//	break;;
		//case 2://敞车
		//	tempstr.Format("%d",cCountK);
		//	pDC->DrawText(tempstr,cellrect,DT_CENTER);
		//	break;
		//case 3://平板车
		//	tempstr.Format("%d",pbCountK);
		//	pDC->DrawText(tempstr,cellrect,DT_CENTER);
		//	break;
		//case 4://油罐车
		//	tempstr.Format("%d",yCountK);
		//	pDC->DrawText(tempstr,cellrect,DT_CENTER);
		//	break;
		//case 5://水泥车
		//	tempstr.Format("%d",sCountK);
		//	pDC->DrawText(tempstr,cellrect,DT_CENTER);
		//	break;
		//case 6://保温车
		//	tempstr.Format("%d",bCountK);
		//	pDC->DrawText(tempstr,cellrect,DT_CENTER);
		//	break;
		//default:
		//	break;

		//}

	}
	/*
	{"毒品车","W"},
	{"集装箱车","X"},
	{"石渣车","K"},
	{"客车底","YZ"},
	{"凹型平板","D"},
	{"其他车种","O"}	

	*/
	/*
	m_totalCount = 0;

	m_dCount = m_jzCount = m_szCount = m_kCount =  m_axCount = m_qCount = 0;

	m_dCountK = m_jzCountK = m_szCountK = m_kCountK = m_axCountK = m_qCountK = 0;
	*/


	//第二排重车 	
	cell_left = static_rect.left;
	cell_top = static_rect.top + 4*yStep + 15;
	cell_right = cell_left + xStep;
	cell_bottom = cell_top + yStep;
	for(i=1; i<=6; i++)
	{
		cell_left = cell_left + xStep;
		cell_right = cell_left + xStep;

		cellrect.SetRect(cell_left,cell_top,cell_right,cell_bottom);

		//switch(i)
		//{
		//case 1://毒品车
		//	tempstr.Format("%d",dCount);
		//	pDC->DrawText(tempstr,cellrect,DT_CENTER);
		//	break;;
		//case 2://集装箱车
		//	tempstr.Format("%d",jzCount);
		//	pDC->DrawText(tempstr,cellrect,DT_CENTER);
		//	break;
		//case 3://石渣车
		//	tempstr.Format("%d",szCount);
		//	pDC->DrawText(tempstr,cellrect,DT_CENTER);
		//	break;
		//case 4://客车底
		//	tempstr.Format("%d",kCount);
		//	pDC->DrawText(tempstr,cellrect,DT_CENTER);
		//	break;
		//case 5://凹型平板
		//	tempstr.Format("%d",axCount);
		//	pDC->DrawText(tempstr,cellrect,DT_CENTER);
		//	break;
		//case 6://其他车种
		//	tempstr.Format("%d",qCount);
		//	pDC->DrawText(tempstr,cellrect,DT_CENTER);
		//	break;
		//default:
		//	break;

		//}

	}


	//第二排空车 	
	cell_left = static_rect.left;
	cell_top = static_rect.top + 5*yStep + 15;
	cell_right = cell_left + xStep;
	cell_bottom = cell_top + yStep;
	for(i=1; i<=6; i++)
	{
		cell_left = cell_left + xStep;
		cell_right = cell_left + xStep;

		cellrect.SetRect(cell_left,cell_top,cell_right,cell_bottom);

		//switch(i)
		//{
		//case 1://毒品车
		//	tempstr.Format("%d",dCountK);
		//	pDC->DrawText(tempstr,cellrect,DT_CENTER);
		//	break;;
		//case 2://集装箱车
		//	tempstr.Format("%d",jzCountK);
		//	pDC->DrawText(tempstr,cellrect,DT_CENTER);
		//	break;
		//case 3://石渣车
		//	tempstr.Format("%d",szCountK);
		//	pDC->DrawText(tempstr,cellrect,DT_CENTER);
		//	break;
		//case 4://客车底
		//	tempstr.Format("%d",kCountK);
		//	pDC->DrawText(tempstr,cellrect,DT_CENTER);
		//	break;
		//case 5://凹型平板
		//	tempstr.Format("%d",axCountK);
		//	pDC->DrawText(tempstr,cellrect,DT_CENTER);
		//	break;
		//case 6://其他车种
		//	tempstr.Format("%d",qCountK);
		//	pDC->DrawText(tempstr,cellrect,DT_CENTER);
		//	break;
		//default:
		//	break;

		//}

	}
	//打印总数
	cell_left = static_rect.left + 15;
	cell_top = static_rect.top + yStep*7;
	cell_right = cell_left + 2*xStep;
	cell_bottom = cell_top + 3*yStep;

	cellrect.SetRect(cell_left,cell_top,cell_right,cell_bottom);

	//tempstr.Format("总数：%d",totalCount);

	pDC->DrawText(tempstr,cellrect,DT_LEFT|DT_VCENTER);

	pDC->SelectObject(pOldPen);

	pen.DeleteObject();
	font.DeleteObject();
	font_title.DeleteObject();
	font_detail.DeleteObject();

}//58874620

CString CQueryPhaseStoreTrain::ConvertShiftIdToString(unsigned long id)
{
	CString date,banbie;

	int CurYear  = id/1000000;
	int CurMonth = id/10000-CurYear*100;
	int CurDay   = id/100-(id/10000)*100;
	date.Format("%4d年%2d月%2d日",CurYear,CurMonth,CurDay);
	int CurBanbie=id-(id/10)*10;
	banbie=(CurBanbie==1) ? "第一班" : "第二班";

	CString shift_id_string;
	shift_id_string = date + banbie;
	return shift_id_string;
}


void CQueryPhaseStoreTrain::OnClose() 
{
	CDialog::OnClose();
}


int CQueryPhaseStoreTrain::GetChineseSplitFlag(CString &chinesestr)
{
	int len = chinesestr.GetLength();
	char curChar;
	int chineseCount = 0;

	for(int i=0; i<len; i++)
	{
		curChar = chinesestr.GetAt(i);

		if(curChar<0)
		{
			chineseCount++;
		}
	}

	return chineseCount%2;
}

void CQueryPhaseStoreTrain::OnButtonprintwhole() 
{
	// TODO: Add your control notification handler code here
	CString title;
	title = "打印站存车汇总？";
	if(AfxMessageBox(title,MB_YESNO | MB_DEFBUTTON1 |MB_ICONQUESTION) != IDYES)
		return; 

	PrintWholeStation();
}

void CQueryPhaseStoreTrain::PrintWholeStation()
{
	CWinApp* cApp=AfxGetApp();
	ASSERT_VALID(cApp);

	HGLOBAL hDevMode;
	HGLOBAL hDevNames;

	CString csPlotterName;
	csPlotterName.Format("%s",gpcdef.GetPrinterName());
	if (!GetPrinterDevice((char *)(LPCTSTR)csPlotterName, &hDevNames, &hDevMode))
	{
		AfxMessageBox(_T("无法获取指定的打印机设备\n"));
		return ;
	}

	CPrintInfo pInfo;
	HDC prnDc;

	pInfo.m_pPD->m_pd.hDevMode = hDevMode;
	pInfo.m_pPD->m_pd.hDevNames = hDevNames;
	prnDc = pInfo.m_pPD->CreatePrinterDC();

	CDC printDC;
	printDC.Attach(prnDc);

	DOCINFO printDocumentName;
	CString sPrintName;	
	sPrintName = "现存车打印";

	char sPrintDocName[32];
	memcpy(sPrintDocName,sPrintName,32);

	printDocumentName.cbSize = sizeof(DOCINFO);
	printDocumentName.lpszDocName = sPrintDocName;
	printDocumentName.lpszOutput = NULL;
	printDC.StartDoc(&printDocumentName);
	printDC.StartPage( );

	PrintWhole(&printDC,&pInfo);


	printDC.EndPage( );	
	printDC.EndDoc( );
	printDC.Detach( );
	DeleteDC(prnDc);

	GlobalFree(hDevNames);
	GlobalFree(hDevMode);
	return ;

}

void CQueryPhaseStoreTrain::PrintWhole(CDC *pDC, CPrintInfo *pInfo)
{
	CString disp_msg;	

	CPen pen;
	pen.CreatePen(PS_SOLID,2,CLR_BLACK);
	CPen *pOldPen=pDC->SelectObject(&pen);

	CFont font,*oldfont;
	CPoint point;
	//打印标题
	CString ss=_T("现存车数统计");
	int nWidth=pDC->GetDeviceCaps(HORZRES);
	int nHeight=pDC->GetDeviceCaps(VERTRES);
	int ghdfx = pDC->GetDeviceCaps(LOGPIXELSX);
	int ghdfy = pDC->GetDeviceCaps(LOGPIXELSY);

	CRect DrawRect(0,0,nWidth,nHeight),rect,tempRect;		
	DrawRect.InflateRect(-100,-50,-100,-20);
	//pDC->Rectangle(&DrawRect);		
	//-MulDiv(21,-pDC->GetDeviceCaps(LOGPIXELSY),72),设置字体高度为21磅，一磅为1/72英寸
	//pDC->GetDeviceCaps(LOGPIXELSY)为每逻辑英寸所占像素
	font.CreateFont(-MulDiv(21,-pDC->GetDeviceCaps(LOGPIXELSY),72),
		0,0,0,FW_NORMAL,0,0,0,GB2312_CHARSET,
		OUT_STROKE_PRECIS,CLIP_STROKE_PRECIS,DRAFT_QUALITY,
		VARIABLE_PITCH|FF_SWISS,_T("黑体"));
	oldfont=pDC->SelectObject(&font);
	tempRect=DrawRect;
	tempRect.InflateRect(0,-5,0,0);

	rect=tempRect;
	rect.InflateRect(10,0);
	point=CPoint(DrawRect.left+(DrawRect.Width()-rect.Width())/2,rect.bottom+2);
	pDC->MoveTo(point);
	pDC->LineTo(point.x+rect.Width(),point.y);
	point.Offset(0,2);
	pDC->MoveTo(point);
	pDC->LineTo(point.x+rect.Width(),point.y);
	tempRect.right=DrawRect.right;
	pDC->DrawText(ss,&tempRect,DT_CENTER|DT_TOP|DT_SINGLELINE);	
	pDC->SelectObject(oldfont);
	font.DeleteObject();
	//显示统计的参数


	long loginshiftid =  gpcdef.GetLoginShiftid();

	CString strDate =ConvertShiftIdToString(loginshiftid) + "    " + gpcdef.GetSectionName(m_nSectionID);
	CString temp;

	font.CreateFont(-MulDiv(12,-pDC->GetDeviceCaps(LOGPIXELSY),72),
		0,0,0,FW_BOLD,0,0,0,GB2312_CHARSET,
		OUT_STROKE_PRECIS,CLIP_STROKE_PRECIS,DRAFT_QUALITY,
		VARIABLE_PITCH|FF_SWISS,_T("宋体"));
	oldfont = pDC->SelectObject(&font);

	pDC->TextOut(rect.left+100,rect.top+250,strDate);

	pDC->SelectObject(oldfont);
	font.DeleteObject();

	CFont font_title;
	font_title.CreateFont(-MulDiv(11,-pDC->GetDeviceCaps(LOGPIXELSY),72),
		0,0,0,FW_NORMAL,0,0,0,GB2312_CHARSET,
		OUT_STROKE_PRECIS,CLIP_STROKE_PRECIS,DRAFT_QUALITY,
		VARIABLE_PITCH|FF_SWISS,_T("宋体"));
	CFont font_detail;
	font_detail.CreateFont(-MulDiv(8,-pDC->GetDeviceCaps(LOGPIXELSY),72),
		0,0,0,FW_NORMAL,0,0,0,GB2312_CHARSET,
		OUT_STROKE_PRECIS,CLIP_STROKE_PRECIS,DRAFT_QUALITY,
		VARIABLE_PITCH|FF_SWISS,_T("宋体"));

	//rect.InflateRect(-100,-630,-100,-200);
	rect.InflateRect(-100,-400,-100,-200);
	pDC->Rectangle(&rect);	     


	//列数为三列，设置列宽
	//第一列车站名字，第二列现存车信息，第三列现存车辆数
	double first_col,second_col,third_col;
	int total_col_width;
	first_col = 0.1;
	second_col = 0.8;
	third_col = 0.1;
	total_col_width = abs(rect.right - rect.left);


	struct  _tagStationStoreTrain* cur_std_train;
	struct _tagSideStoredTrain *cur_side_train;
	struct _tagSideStoreCarriage *cur_carriage;

	//定义存放股道车辆信息的变量
	CString sideArrayStr[64][3]; //3列，//第一列车站名字，第二列现存车信息，第三列现存车辆数
	int sideArrayNum = 0;

	CString strStyle,text,tmp;

	//获取要显示的股道信息，和成要显示的字符串
	int i,j,k;

	for(i = 0; i<nStationNum; i++)
	{
		cur_std_train = &sectionStoreCar[i];

		text = "";
		int totalCount = 0;
		for(j=0;j < cur_std_train->usSideNum;j++)
		{  
			cur_side_train = &cur_std_train->sideinfo[j];

			int nSideCarriageCount=0;

			tmp = "";

			for(k=0; k < cur_side_train->usRecNum; k++)
			{   
				cur_carriage = &cur_side_train->rec[k];
				if(cur_carriage->usCarriageNum <=0)
					continue;

				strStyle = "";

				BYTE nSel = 0;
				BYTE nSubSel = 0;
				m_pBasicPara->GetCarTypeAndKindIndex(cur_carriage->usCarriageType, nSel, nSubSel);

				if(nSel<m_pBasicPara->m_carKindType.size())	 
				{
					strStyle = m_pBasicPara->m_carKindType[nSel].shortName;
					if(strStyle.IsEmpty())
						strStyle = m_pBasicPara->m_carKindType[nSel].name;


					text += strStyle;

					if(CString(cur_carriage->lpszCarriageRemark) != "")
					{
						tmp.Format("%s%s%d(%s) ",
							m_pBasicPara->m_aryWorkExplain[cur_carriage->usCarriageExplain],
							m_pBasicPara->m_carKindType[nSel].carKind[nSubSel].shortName,
							cur_carriage->usCarriageNum,		
							cur_carriage->lpszCarriageRemark);
					}
					else
					{
						tmp.Format("%s%s%d ",
							m_pBasicPara->m_aryWorkExplain[cur_carriage->usCarriageExplain],
							m_pBasicPara->m_carKindType[nSel].carKind[nSubSel].shortName,
							cur_carriage->usCarriageNum);
					}
					text += tmp;
				}

				nSideCarriageCount +=  cur_carriage->usCarriageNum;
			}

			cur_side_train->CarriageCounts = nSideCarriageCount;   ///该股道的车厢数目
			totalCount += cur_side_train->CarriageCounts;			

		}

		int line_num = 1;
		//该行长度超出了本列的宽度
		if(pDC->GetTextExtent(text).cx > total_col_width*second_col)
		{			
			if(pDC->GetTextExtent(text).cx % LONG(total_col_width*second_col) == 0)
				line_num = pDC->GetTextExtent(text).cx / LONG(total_col_width*second_col);
			else 
				line_num = pDC->GetTextExtent(text).cx / LONG(total_col_width*second_col) + 1;
		}
		if(line_num<=0)
			line_num = 1;

		if(sideArrayNum+line_num>=64)//超出数组长度
			break;

		int single_col_char;
		single_col_char = text.GetLength()/line_num;		

		//插入数据	
		int pre_chinese_splited = 0;//上一行汉字被分割
		int  cur_chinese_splited = 0;//当前行汉字被分割
		CString splited_text;
		for(k=0; k<line_num; k++)
		{
			if(k == 0)
			{
				sideArrayStr[sideArrayNum+k][0].Format("%s",gpcdef.GetStationName(nStationList[i]));
				sideArrayStr[sideArrayNum+k][2].Format("%d",totalCount);
			}
			else
			{
				sideArrayStr[sideArrayNum+k][0].Empty();
				sideArrayStr[sideArrayNum+k][2].Empty();
			}

			if(pre_chinese_splited)
				splited_text = text.Mid(k*single_col_char-1,single_col_char+1);
			else
				splited_text = text.Mid(k*single_col_char,single_col_char);

			cur_chinese_splited = GetChineseSplitFlag(splited_text);

			if(cur_chinese_splited)
			{
				splited_text = splited_text.Left(splited_text.GetLength()-1);
			}

			pre_chinese_splited = cur_chinese_splited;		

			sideArrayStr[sideArrayNum+k][1].Format("%s",splited_text);
		}	

		sideArrayNum += line_num;
	}

	//要打印的行数
	int row_count = sideArrayNum+1;
	CString side_name,side_name_next;
	side_name = "";
	side_name_next = "";

	int yStep =	150;//(rect.bottom-rect.top)/2/row_count;

	//自动调整行距
	while((row_count)*yStep > (rect.bottom - rect.top))
		yStep-=5;

	//实际行数还要加上标题拦这一行
	for(k=0; k<=row_count; k++)  //行数的绘制
	{			
		if(k>=2)//正式画股道的线，如果发现下面一行股道号为空，那么不画线
		{
			side_name = sideArrayStr[k-2][0];

			if(k < row_count)
				side_name_next = side_name = sideArrayStr[k-1][0];
			else
				side_name_next = "temp";

			if( side_name_next == "")  //该行股道号为空，表示为上面股道的信息太长而追加的行，不画线
				;
			else
			{
				pDC->MoveTo(rect.left,rect.top + yStep*k);
				pDC->LineTo(rect.right,rect.top + yStep*k);
			}
		}
		else //画标题拦的底线
		{
			pDC->MoveTo(rect.left,rect.top + yStep*k);
			pDC->LineTo(rect.right,rect.top + yStep*k);
		}


	}

	for(j=1; j<=4; j++)
	{
		if(j==1)	//可以不画，和矩形左边框重叠的那一条线
		{
			pDC->MoveTo(rect.left, rect.top);
			pDC->LineTo(rect.left, rect.top + yStep*row_count);
		}
		else if(j==2)
		{
			pDC->MoveTo(rect.left + first_col*(rect.right-rect.left), rect.top);
			pDC->LineTo(rect.left + first_col*(rect.right-rect.left), rect.top + yStep*row_count);
		}
		else if(j==3)
		{
			pDC->MoveTo(rect.left + (first_col+second_col)*(rect.right-rect.left), rect.top);
			pDC->LineTo(rect.left + (first_col+second_col)*(rect.right-rect.left), rect.top + yStep*row_count);
		}
		else
		{
			//	pDC->MoveTo(rect.left + (rect.right-rect.left), rect.top);
			//	pDC->LineTo(rect.left + (rect.right-rect.left), rect.top + (rect.bottom-rect.top)/2);
		}

	}

	CRect cellrect;
	int cell_left,cell_top,cell_right,cell_bottom;

	cell_left = rect.left;
	cell_top = rect.top + 15;
	cell_right = cell_left + first_col*(rect.right-rect.left);
	cell_bottom = rect.bottom + yStep;

	cellrect.SetRect(cell_left,cell_top,cell_right,cell_bottom);

	oldfont = pDC->SelectObject(&font_title);
	pDC->DrawText("车站",cellrect,DT_CENTER);

	cell_left = cell_right;
	cell_right = cell_left + second_col*(rect.right-rect.left);

	cellrect.SetRect(cell_left,cell_top,cell_right,cell_bottom);
	pDC->DrawText("现存车信息",cellrect,DT_CENTER);

	cell_left = cell_right;
	cell_right = cell_left + third_col*(rect.right-rect.left);

	cellrect.SetRect(cell_left,cell_top,cell_right,cell_bottom);
	pDC->DrawText("总数",cellrect,DT_CENTER);


	pDC->SelectObject(oldfont);



	int first_col_xpos,second_col_xpos,third_col_xpos;

	//显示内容
	row_count = row_count - 1; //数据的行数,row_count中以前包括了标题栏这一行

	first_col_xpos = rect.left + 20;
	second_col_xpos = first_col_xpos + first_col*(rect.right - rect.left);
	third_col_xpos = second_col_xpos + second_col*(rect.right - rect.left);

	CString side_content,side_cars_num;

	for(i=1; i<=row_count; i++)  
	{

		side_name = sideArrayStr[i-1][0];
		side_content = sideArrayStr[i-1][1];
		side_cars_num = sideArrayStr[i-1][2];

		pDC->TextOut(first_col_xpos, rect.top+yStep*i+15,side_name);
		pDC->TextOut(second_col_xpos, rect.top+yStep*i+15,side_content);
		pDC->TextOut(third_col_xpos, rect.top+yStep*i+15,side_cars_num);

	}

	pDC->SelectObject(pOldPen);

	pen.DeleteObject();
	font_title.DeleteObject();
	font_detail.DeleteObject();

}

void CQueryPhaseStoreTrain::OnButtonSave() 
{
	// TODO: Add your control notification handler code here
	char strFileName[1024];
	CString strFile;


	//确定要存储的文件名称
	CString szFileName,filePath;

	filePath += "站存车.txt";
	CFileDialog cfiledialog(FALSE,"txt",NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"文本文件 (*.txt)|*.txt",NULL);

	cfiledialog.DoModal();
	strFile = cfiledialog.GetFileName();
	strcpy(strFileName,LPCTSTR (strFile));
	if(strFileName[0]=='\0')return;

	CStdioFile fStdFile;
	if(!fStdFile.Open(strFileName,CFile::modeCreate | CFile::modeWrite | CFile::typeText ))
	{
		AfxMessageBox("保存站存车文本失败!");
		return;
	}

	/*CString stationName;
	CString stationText;
	struct _tagStationStoreTrain *stationInfo;

	for(int i=0; i<nStationNum; i++)
	{
		stationInfo = &sectionStoreCar[i];
		stationText = GetStationText(stationInfo);
		if(gpcdef.GetStationName(nStationList[i]) != NULL)
			stationName = CString(gpcdef.GetStationName(nStationList[i]));
		fStdFile.WriteString(stationName + "\t" + stationText + "\n");
	}*/

	

	CString stationName, timeName, numName, context, line;
	for(int i=0; i<m_ctrlSectionStoreTrainList.GetItemCount(); i++)
	{
		stationName=m_ctrlSectionStoreTrainList.GetItemText(i, 0);
		timeName=m_ctrlSectionStoreTrainList.GetItemText(i, 1);
		context=m_ctrlSectionStoreTrainList.GetItemText(i, 2);
		numName=m_ctrlSectionStoreTrainList.GetItemText(i, 3);
		fStdFile.WriteString(stationName+"\n");
		line.Format("%s\t%s\t%s\n", timeName, numName, context);
		fStdFile.WriteString(line);
	}
	fStdFile.Close();
	
	AfxMessageBox("保存站存车文本成功!");
}

CString GetStationText(struct _tagStationStoreTrain *stationInfo)
{
	CString stationText;
	CString strStyle;
	CString recText;
	struct _tagSideStoredTrain* cur_side_train = NULL;
	struct _tagSideStoreCarriage* cur_carriage = NULL;

	//获取要显示的股道信息，和成要显示的字符串
	stationText.Empty();
	for(int j=0;j < stationInfo->usSideNum;j++)
	{  
		cur_side_train = &stationInfo->sideinfo[j];

		for(int k=0; k < cur_side_train->usRecNum; k++)
		{ 	
			recText.Empty();
			strStyle.Empty();

			cur_carriage = &cur_side_train->rec[k];

			if(cur_carriage->usCarriageNum ==0)
				continue;		

			BYTE nSel = 0;
			BYTE nSubSel = 0;
			GetTGBasicParam()->GetCarTypeAndKindIndex(cur_carriage->usCarriageType, nSel, nSubSel);

			if(nSel<GetTGBasicParam()->m_carKindType.size())	 
			{
				strStyle = GetTGBasicParam()->m_carKindType[nSel].shortName;
				if(strStyle.IsEmpty())
					strStyle = GetTGBasicParam()->m_carKindType[nSel].name;

				if(CString(cur_carriage->lpszCarriageRemark) != "")
				{
					recText.Format("%s%s%d(%s) ",
						GetTGBasicParam()->m_aryWorkExplain[cur_carriage->usCarriageExplain],
						GetTGBasicParam()->m_carKindType[nSel].carKind[nSubSel].shortName,
						cur_carriage->usCarriageNum,		
						cur_carriage->lpszCarriageRemark);
				}
				else
				{
					recText.Format("%s%s%d ",
						GetTGBasicParam()->m_aryWorkExplain[cur_carriage->usCarriageExplain],
						GetTGBasicParam()->m_carKindType[nSel].carKind[nSubSel].shortName,
						cur_carriage->usCarriageNum);
				}
			}

			recText = strStyle + recText;
			stationText += recText;
		}	 

	}
	//合成股道显示字符串结束
	return stationText;
}

int CQueryPhaseStoreTrain::IsSideInfoExist(ushort nStationNo,char *sSideName)
{	
	_tagSideStoredTrain *sideinfo;
	for(int i=0;i<nStationNum;i++)
	{
		if (sectionStoreCar[i].usStationNo != nStationNo) continue;

		for(int j=0;j<sectionStoreCar[i].usSideNum;j++)
		{
			sideinfo = &sectionStoreCar[i].sideinfo[j];
			if(!strcmp(sideinfo->pszSideName,sSideName))
				return j;		   
		}
	}
	return -1;

}
