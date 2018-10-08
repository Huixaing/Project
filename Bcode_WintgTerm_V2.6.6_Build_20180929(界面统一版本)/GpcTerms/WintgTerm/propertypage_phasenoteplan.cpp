#include "stdafx.h"
#include "propertypage_phasenoteplan.h"
#include "tgdocument.h"

extern CEdit* g_curEdit;//当前常用词汇要输入的编辑框

#define ITEMNUM  12

/////////////////////////////////////////////////////////////////////////////
// CPageNotepadPlan property page
CPageNotepadPlan::CPageNotepadPlan(CTgDocument *pdoc) : CPropertyPage(CPageNotepadPlan::IDD)
{
	//{{AFX_DATA_INIT(CPageNotepadPlan)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	firstRun = TRUE;
	m_Start_station = _T("");
	m_End_station = _T("");

	m_nFixCols = 0;
	m_nFixRows = 0;
	m_nCols = 5;
	nUnitNum =0;

	m_pGridCtrl = NULL;
	bAllStationFlag = TRUE;
	m_StageNum = 0;
	m_nCurStageNo = 0;
	m_pDoc = pdoc;
	m_pNotifyWindow=NULL;
	g_curEdit=NULL;
}

CPageNotepadPlan::~CPageNotepadPlan()
{	
	if(m_pGridCtrl)
	{
		delete m_pGridCtrl;
		m_pGridCtrl=NULL;
	}
}

void CPageNotepadPlan::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPageNotepadPlan)
	DDX_Control(pDX, IDC_COMBO_WEATHER_END, m_cboWeagherEnd);
	DDX_Control(pDX, IDC_COMBO_WEATHER_START, m_cboWeagherStart);
	DDX_Control(pDX, IDC_CUR_TIME, m_timeCtrl);
	DDX_Control(pDX, IDC_CUR_DATE2, m_DateCtrl);
	DDX_Control(pDX, IDC_EDIT_CONTENT, m_Content);
	DDX_Text(pDX, IDC_START_STATION, m_Start_station);
	DDX_Text(pDX, IDC_END_STATION, m_End_station);
	DDX_Text(pDX, IDC_EDIT_COMMANDER, m_strCommander);
	DDV_MaxChars(pDX, m_strCommander, 12);
	DDX_Text(pDX, IDC_EDIT_DATE, m_strSendDate);
	DDV_MaxChars(pDX, m_strSendDate, 16);
	DDX_Text(pDX, IDC_EDIT_TIME, m_strSendTime);
	DDV_MaxChars(pDX, m_strSendTime, 16);
	DDX_Text(pDX, IDC_EDIT_SEND_TIME, m_strSendTimeEx);
	DDV_MaxChars(pDX, m_strSendTimeEx, 16);
	DDX_Radio(pDX, IDC_PHASE1, m_StageNum);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPageNotepadPlan, CPropertyPage)
	//{{AFX_MSG_MAP(CPageNotepadPlan)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_PHASE1, OnPhase1)
	ON_BN_CLICKED(IDC_PHASE2, OnPhase2)
	ON_BN_CLICKED(IDC_PHASE3, OnPhase3)
	ON_EN_SETFOCUS(IDC_EDIT_CONTENT, OnSetfocusEditContent)
	ON_EN_SETFOCUS(IDC_EDIT_COMMANDER, OnSetfocusEditCommander)
	ON_EN_SETFOCUS(IDC_EDIT_TIME, OnSetfocusEditTime)
	ON_BN_CLICKED(IDC_GRID, OnGrid)
	ON_BN_CLICKED(ID_SEND_CURRENT2, OnSendCurrent)
	ON_BN_CLICKED(IDC_SAVE, OnSave)
	ON_BN_CLICKED(IDC_LOAD, OnLoad)
	ON_EN_SETFOCUS(IDC_EDIT_SEND_TIME, OnSetfocusEditSendTime)
	//}}AFX_MSG_MAP
	ON_MESSAGE(UM_UNI_MESSAGE, OnCommMsg)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropertySheetNotepad
LRESULT CPageNotepadPlan::OnCommMsg(WPARAM wParam, LPARAM lParam) 
{
	UNI_MESSAGE_STRUCT *pMsgStruct=( UNI_MESSAGE_STRUCT *)lParam;
	if(pMsgStruct->msg_id == MSGID_MsgWordPhasePlanStatus)
	{
		ProcessMessage((MsgWordPhasePlanStatus*)pMsgStruct->pmsg);
	}
	if(pMsgStruct->msg_id == MSGID_MsgWordPhasePlanSend)
	{
		ProcessMessage((MsgWordPhasePlanSend*)pMsgStruct->pmsg);
	}
	
	FREE_UNI_MESSAGE_STRUCT(pMsgStruct);
	return 0;
}

BOOL  CPageNotepadPlan::ProcessMessage(const MsgWordPhasePlanSend *pBuf) 
{
	if(pBuf->con_id!=m_pDoc->GetCurrentSectionNo())
	{
		GpcLogTrainMessage(0, 0, "收到阶段记事, 非本台, 不处理 con_id %d msg_con_id %d doc %d", m_pDoc->GetCurrentSectionNo(), pBuf->con_id, this);
		return FALSE;
	}
	if(pBuf->shift_id!=m_pDoc->GetCurrentShiftID())
	{
		GpcLogTrainMessage(0, 0, "收到阶段记事, 非本班, 不处理 shift_id %d msg_shift_id %d doc %d", m_pDoc->GetCurrentShiftID(), pBuf->shift_id, this);
		return FALSE;
	}
	if(pBuf->schd_type!=m_pDoc->GetScheduleType())
	{
		GpcLogTrainMessage(0, 0, "收到阶段记事, 非本班类型, 不处理 schd_type %d msg_schd_type %d doc %d", m_pDoc->GetScheduleType(), pBuf->schd_type, this);
		return FALSE;
	}

	int phase_id = pBuf->phase_id;
	m_pDoc->m_StationPlanInfo_ImportantNew.start_st_weather[phase_id]  
				= pBuf->start_sta_weather;      
	m_pDoc->m_StationPlanInfo_ImportantNew.end_st_weather[phase_id]
				= pBuf->end_sta_weather;

	GpcLogTrainMessage(0, 0, "收到阶段记事 阶段 %d, start_st_weather %d end_sta_weather %d", 
		pBuf->phase_id, pBuf->start_sta_weather, pBuf->end_sta_weather);

	int position = 0;
	CString strTemp,strContent,strCommander,strCommandTime,strSendTime;
	strTemp = pBuf->phase_content;

	do
	{
		if(strTemp.IsEmpty())
		{
			strContent = strSendTime = strCommandTime = strCommander = " ";
			break;
		}
		position = strTemp.Find('&');
		if(position <0)
			break;
		strCommander = strTemp.Left(position);
		strTemp = strTemp.Right(strTemp.GetLength() - position -1);
		position = strTemp.Find('&');

		if(position < 0)
			break;
		strCommandTime = strTemp.Left(position);
		strTemp = strTemp.Right(strTemp.GetLength() - position -1);
		position = strTemp.Find('&');

		if(position < 0)
			break;
		strSendTime = strTemp.Left(position);
		strTemp = strTemp.Right(strTemp.GetLength() - position -1);
	}while(0);
	strContent = strTemp;

	m_pDoc->m_StationPlanInfo_ImportantNew.text_content[phase_id].Format("%s" ,strContent);
	m_pDoc->m_StationPlanInfo_ImportantNew.text_send_time[phase_id].Format("%s" , strSendTime);
	m_pDoc->m_StationPlanInfo_ImportantNew.text_command_time[phase_id].Format("%s" , strCommandTime);
	m_pDoc->m_StationPlanInfo_ImportantNew.text_commander[phase_id].Format("%s" ,strCommander);

	m_pDoc->m_StationPlanInfo_ImportantNew.edit_time[phase_id] = pBuf->edit_time;
	m_pDoc->m_StationPlanInfo_ImportantNew.send_time[phase_id] = pBuf->send_time;

	for(int j = 0; j <  m_pDoc->m_StationPlanInfo_ImportantNew.nEntityNum; j++)
	{
		m_pDoc->m_StationPlanInfo_ImportantNew.m_StationPlan[j].m_StageInfo[phase_id].bReceiveFlag = CLBTH::MsgWordPhasePlanStatus::WORDPHASE_INIT; 
		m_pDoc->m_StationPlanInfo_ImportantNew.m_StationPlan[j].m_StageInfo[phase_id].bSignInFlag = CLBTH::MsgWordPhasePlanStatus::WORDPHASE_INIT;							
	}

	UpdateShow(phase_id);

	return TRUE;
}

BOOL  CPageNotepadPlan::ProcessMessage(const MsgWordPhasePlanStatus *pBuf) 
{
	if(pBuf->con_id!=m_pDoc->GetCurrentSectionNo())
	{
		GpcLogTrainMessage(0, 0, "收到阶段记事状态, 非本台, 不处理 con_id %d msg_con_id %d doc %d", m_pDoc->GetCurrentSectionNo(), pBuf->con_id, this);
		return FALSE;
	}
	if(pBuf->shift_id!=m_pDoc->GetCurrentShiftID())
	{
		GpcLogTrainMessage(0, 0, "收到阶段记事状态, 非本班, 不处理 shift_id %d msg_shift_id %d doc %d", m_pDoc->GetCurrentShiftID(), pBuf->shift_id, this);
		return FALSE;
	}
	//if(pBuf->schd_type!=m_pDoc->GetScheduleType())
	//{
	//	GpcLogTrainMessage(0, 0, "收到阶段记事状态, 非本班类型, 不处理 schd_type %d msg_schd_type %d doc %d", m_pDoc->GetScheduleType(), pBuf->schd_type, this);
	//	return FALSE;
	//}

	do
	{			
		int phase_id = pBuf->phase_id;
			
		//find suited entity
		for(int i=0;i<  m_pDoc->m_StationPlanInfo_ImportantNew.nEntityNum;i++)
		{
			if(pBuf->recv_entity ==  m_pDoc->m_StationPlanInfo_ImportantNew.m_StationPlan[i].Entityid)
				break;
		}
		//not found one ,then return;
		if(i>= m_pDoc->m_StationPlanInfo_ImportantNew.nEntityNum)
			break;
		//int nStatus = m_StationPlanInfo_ImportantNew.m_StationPlan[row-1].m_StageInfo[m_StageNum].bReceiveFlag;

		//found one entity		
		m_pDoc->m_StationPlanInfo_ImportantNew.m_StationPlan[i].m_StageInfo[phase_id].bReceiveFlag = (short)pBuf->recv_status;
		m_pDoc->m_StationPlanInfo_ImportantNew.m_StationPlan[i].m_StageInfo[phase_id].bSignInFlag = (short)pBuf->recv_status;

		GpcLogTrainMessage(0, 0, "收到阶段记事状态entity %d recv_status %d bSignInFlag %d", pBuf->recv_entity, pBuf->recv_status, pBuf->recv_status);

		memset( m_pDoc->m_StationPlanInfo_ImportantNew.m_StationPlan[i].m_StageInfo[phase_id].strSignName,0x00,16);//permit_name
		strncpy( m_pDoc->m_StationPlanInfo_ImportantNew.m_StationPlan[i].m_StageInfo[phase_id].strSignName,pBuf->recv_name,sizeof(m_pDoc->m_StationPlanInfo_ImportantNew.m_StationPlan[i].m_StageInfo[phase_id].strSignName));

		memset( m_pDoc->m_StationPlanInfo_ImportantNew.m_StationPlan[i].m_StageInfo[phase_id].strRecvName,0x00,16);
		strncpy( m_pDoc->m_StationPlanInfo_ImportantNew.m_StationPlan[i].m_StageInfo[phase_id].strRecvName,pBuf->recv_name,sizeof(m_pDoc->m_StationPlanInfo_ImportantNew.m_StationPlan[i].m_StageInfo[phase_id].strRecvName));

		m_pDoc->m_StationPlanInfo_ImportantNew.m_StationPlan[i].m_StageInfo[phase_id].send_time = pBuf->send_time;
		m_pDoc->m_StationPlanInfo_ImportantNew.m_StationPlan[i].m_StageInfo[phase_id].recv_time = pBuf->recv_time;
		m_pDoc->m_StationPlanInfo_ImportantNew.m_StationPlan[i].m_StageInfo[phase_id].edit_time = pBuf->edit_time;

		UpdateShowSendStatus(phase_id, pBuf->recv_entity); 
	}while(0);

	return TRUE;
}

void CPageNotepadPlan::OnDestroy()
{
	UnRegisterMsgProcHandleWnd(GetSafeHwnd(), MSGID_MsgWordPhasePlanSend);
	UnRegisterMsgProcHandleWnd(GetSafeHwnd(), MSGID_MsgWordPhasePlanStatus);

	if(m_pGridCtrl)
	{
		delete m_pGridCtrl;
		m_pGridCtrl=NULL;
	}
	CPropertyPage::OnDestroy();
}

BOOL CPageNotepadPlan::OnInitDialog() 
{
	char *  weather[]={"晴","阴","小雨","中雨","大雨","小雾","中雾","大雾"
		,"小雪","中雪","大雪"};

	this->InitIdentify();

	CPropertyPage::OnInitDialog();


	m_edittemp1.SubclassDlgItem( IDC_EDIT_COMMANDER, this );
	m_edittemp2.SubclassDlgItem( IDC_EDIT_TIME, this );
	m_edittemp3.SubclassDlgItem( IDC_EDIT_SEND_TIME, this );



	//*** CEditEx and CStaticEx Subclass our controls ***
	//
	m_Content.setFont( -17 );

	if(firstRun)
	{
		firstRun = FALSE;
		m_Content.SetLimitText(900);

		CleanLocal();
		InitialGridCtrl();

		FillGridCtrl();
		int num=128, stationNo[128];
		memset(stationNo, 0, sizeof(stationNo));

		num=gpcdef.GetStationList(m_pDoc->GetCurrentSectionNo(), num, stationNo);

		m_Start_station.Format("%s", gpcdef.GetStationName(stationNo[0]));
		if(num > 0)
			m_End_station.Format("%s", gpcdef.GetStationName(stationNo[num-1]));
		else
			m_End_station.Format("%s", gpcdef.GetStationName(stationNo[0]));

		m_Content.SetWindowText(m_pDoc->m_StationPlanInfo_ImportantNew.text_content[m_StageNum]);

		for(int i=0; i<sizeof weather / sizeof weather[0]; i++)
		{
			m_cboWeagherStart.InsertString (i,_T(weather[i]) );   
			m_cboWeagherEnd.InsertString(i,_T(weather[i]) );
		}
		m_cboWeagherStart.SetCurSel(0);
		m_cboWeagherEnd.SetCurSel(0);
	}
	InformParent();
	
	RegisterMsgProcHandleWnd(GetSafeHwnd(), MSGID_MsgWordPhasePlanSend, 0);
	RegisterMsgProcHandleWnd(GetSafeHwnd(), MSGID_MsgWordPhasePlanStatus, 0);
	UpdateData(FALSE);

	OnLoad();
	return TRUE;  
}

void CPageNotepadPlan::InformParent()
{
	TRAIN_SELECT_PARA para;
	CTG_ScheduleInfo schd;
	m_pDoc->GetScheduleInfo( schd);
	TIME shift_start,shift_end;
	schd.GetShiftManagerRange(shift_start,shift_end); 
	para.starttime= shift_start+m_nCurStageNo*4*3600;
	para.endtime= para.starttime+4*3600;
	if(m_pNotifyWindow)
		m_pNotifyWindow->SendMessage(UM_TG_TRAIN_SELECTED,(WPARAM)0,(LPARAM)&para);

}

void CPageNotepadPlan::OnPhase1() 
{
	UpdateData(TRUE);
	CString strContent;
	m_Content.GetWindowText(strContent);
	m_pDoc->m_StationPlanInfo_ImportantNew.text_content[m_nCurStageNo] = strContent;
	m_pDoc->m_StationPlanInfo_ImportantNew.text_commander[m_nCurStageNo] = m_strCommander;
	m_pDoc->m_StationPlanInfo_ImportantNew.text_command_time[m_nCurStageNo] = m_strSendTime;
	m_pDoc->m_StationPlanInfo_ImportantNew.text_send_time[m_nCurStageNo] = m_strSendTimeEx;
	m_nCurStageNo = 0;
	commonControl();
	InformParent();
}

void CPageNotepadPlan::OnPhase2() 
{
	UpdateData(TRUE);
	CString strContent;
	m_Content.GetWindowText(strContent);
	m_pDoc->m_StationPlanInfo_ImportantNew.text_content[m_nCurStageNo] = strContent;
	m_pDoc->m_StationPlanInfo_ImportantNew.text_commander[m_nCurStageNo] = m_strCommander;
	m_pDoc->m_StationPlanInfo_ImportantNew.text_command_time[m_nCurStageNo] = m_strSendTime;
	m_pDoc->m_StationPlanInfo_ImportantNew.text_send_time[m_nCurStageNo] = m_strSendTimeEx;
	m_nCurStageNo = 1;
	commonControl();
	InformParent();
}

void CPageNotepadPlan::OnPhase3() 
{
	UpdateData(TRUE);
	CString strContent;
	m_Content.GetWindowText(strContent);
	m_pDoc->m_StationPlanInfo_ImportantNew.text_content[m_nCurStageNo] = strContent;
	m_pDoc->m_StationPlanInfo_ImportantNew.text_commander[m_nCurStageNo] = m_strCommander;
	m_pDoc->m_StationPlanInfo_ImportantNew.text_command_time[m_nCurStageNo] = m_strSendTime;
	m_pDoc->m_StationPlanInfo_ImportantNew.text_send_time[m_nCurStageNo] = m_strSendTimeEx;
	m_nCurStageNo = 2;
	commonControl();
	InformParent();
}


BOOL CPageNotepadPlan::PreTranslateMessage(MSG* pMsg) 
{
	return CPropertyPage::PreTranslateMessage(pMsg);
}

BOOL CPageNotepadPlan::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	SetModified(TRUE);	
	return CPropertyPage::OnCommand(wParam, lParam);
}
void CPageNotepadPlan::InitIdentify()
{
	if(m_pDoc->IsQueryMode())
	{
		CWnd *pWnd;
		pWnd = GetDlgItem(IDC_COMBO_WEATHER_START);
		pWnd->EnableWindow(FALSE);

		pWnd = GetDlgItem(IDC_COMBO_WEATHER_END);
		pWnd->EnableWindow(FALSE);

		CButton *pButton;
		pButton= (CButton *)GetDlgItem(ID_SEND_CURRENT2);
		pButton->EnableWindow(FALSE);

		pButton= (CButton *)GetDlgItem(IDC_SAVE);
		pButton->EnableWindow(FALSE);

		pWnd = GetDlgItem(IDC_COMBO_WEATHER_START);
		pWnd->EnableWindow(FALSE);

		pWnd = GetDlgItem(IDC_COMBO_WEATHER_END);
		pWnd->EnableWindow(FALSE);
	}
}

void CPageNotepadPlan::UpdateShowSendStatus(int phase_id,int entity_id)
{
	if(phase_id !=m_StageNum )
		return;
	for (int row = 0; row < m_pDoc->m_StationPlanInfo_ImportantNew.nEntityNum; row++)
	{
		if(m_pDoc->m_StationPlanInfo_ImportantNew.m_StationPlan[row].Entityid==entity_id)
		{
			RefreshSendStatusRow(row+1);
			break;
		}
	}
	m_pGridCtrl->Refresh();
}

void CPageNotepadPlan::UpdateShow(int phase_id)
{
	if(m_pGridCtrl == NULL)
	{
		return;
	}
	//1 下发状态消息
	m_pGridCtrl->DeleteAllItems();
	FillGridCtrl();
	m_pGridCtrl->UpdateData ();

	//2 接收的消息是阶段记事的,此时的nFlag是phase_id;
	if(phase_id !=m_StageNum )
		return;
	//显示当前变化的阶段记事
	m_strCommander = m_pDoc->m_StationPlanInfo_ImportantNew.text_commander[m_StageNum];
	m_strSendTime = m_pDoc->m_StationPlanInfo_ImportantNew.text_command_time[m_StageNum];
	m_strSendTimeEx = m_pDoc->m_StationPlanInfo_ImportantNew.text_send_time[m_StageNum];// = m_strSendTimeEx;

	m_cboWeagherStart.SetCurSel(m_pDoc->m_StationPlanInfo_ImportantNew.start_st_weather[m_StageNum]);
	m_cboWeagherEnd.SetCurSel(m_pDoc->m_StationPlanInfo_ImportantNew.end_st_weather[m_StageNum]);
	m_Content.SetWindowText(m_pDoc->m_StationPlanInfo_ImportantNew.text_content[m_StageNum]);
	UpdateData(FALSE);
}

void CPageNotepadPlan::OnGrid()
{
}

void CPageNotepadPlan::OnSendCurrent()
{
	UpdateData(TRUE);
	nSelectedNum = 0;

	CTime time;	
	long usDateValue,usTimeValue;
	CString str;		

	usTimeValue = m_pDoc->GetCurrentTGTime();
	usDateValue = m_pDoc->GetCurrentTGDate();;
	sendTime = usTimeValue / 60;
	sendDate = usDateValue;
	//下发对象列表
	for (int row = 1; row <= m_pDoc->m_StationPlanInfo_ImportantNew.nEntityNum; row++)
	{
		nSelectedList[nSelectedNum++] = m_pDoc->m_StationPlanInfo_ImportantNew.m_StationPlan[row-1].nStation;
		m_pDoc->m_StationPlanInfo_ImportantNew.m_StationPlan[row-1].m_StageInfo[m_StageNum].usSendDate = usDateValue;
		m_pDoc->m_StationPlanInfo_ImportantNew.m_StationPlan[row-1].m_StageInfo[m_StageNum].usSendTime = usTimeValue;

		m_pDoc->m_StationPlanInfo_ImportantNew.m_StationPlan[row-1].m_StageInfo[m_StageNum].send_time =  long_time(usDateValue,usTimeValue);
		m_pDoc->m_StationPlanInfo_ImportantNew.m_StationPlan[row-1].m_StageInfo[m_StageNum].recv_time =  0;
		m_pDoc->m_StationPlanInfo_ImportantNew.m_StationPlan[row-1].m_StageInfo[m_StageNum].edit_time =  long_time(usDateValue,usTimeValue);

		m_pDoc->m_StationPlanInfo_ImportantNew.m_StationPlan[row-1].m_StageInfo[m_StageNum].bSignInFlag = CLBTH::MsgWordPhasePlanStatus::WORDPHASE_INIT;
		m_pDoc->m_StationPlanInfo_ImportantNew.m_StationPlan[row-1].m_StageInfo[m_StageNum].bReceiveFlag = CLBTH::MsgWordPhasePlanStatus::WORDPHASE_INIT ;
		m_pDoc->m_StationPlanInfo_ImportantNew.m_StationPlan[row-1].m_StageInfo[m_StageNum].strSignName[0] = 0x00;

		str = OsConvertTimetoString(m_pDoc->m_StationPlanInfo_ImportantNew.m_StationPlan[row-1].m_StageInfo[m_StageNum].usSendTime);
		str = str.Left(5);
		m_pGridCtrl->SetItemText (row,1,str);

		str = "否";
		m_pGridCtrl->SetItemFgColour (row,2,CLR_INDIANRED);
		m_pGridCtrl->SetItemText (row,2,str);
		str = "否";
		m_pGridCtrl->SetItemFgColour (row,3,CLR_INDIANRED);
		m_pGridCtrl->SetItemText (row,3,str);
		str = "";
		m_pGridCtrl->SetItemText (row,4,str);
	}	
	m_Content.GetWindowText(m_pDoc->m_StationPlanInfo_ImportantNew.text_content[m_StageNum]);

	m_pGridCtrl->UpdateData();
	//下发计划
	SendMessageToCad(0);

	CString strTime;
	CTime t = CTime::GetCurrentTime( );
	long m_usCurrentTime = (t.GetHour( ) * 60 + t.GetMinute( )) * 60 + t.GetSecond( );
	strTime = OsConvertTimetoString(m_usCurrentTime);   
	m_strSendTimeEx = strTime;

	Invalidate(FALSE);
}


void CPageNotepadPlan::RefreshSendStatusRow(int row)
{
	CRect rect;	
	DWORD dwTextStyle = DT_RIGHT|DT_VCENTER|DT_SINGLELINE|DT_NOPREFIX;	
	int col;
	CTime t_Recv; time_t t_long;
	TCHAR rgtsz[ITEMNUM][20] = {_T("车站"),_T("下达时间"),_T("是否接收"),_T("是否签收"),_T("签收人")};	
	for ( col = 0; col < m_pGridCtrl->GetColumnCount(); col++)
	{ 
		GV_ITEM Item;
		Item.mask = GVIF_TEXT|GVIF_FORMAT;
		Item.row = row;
		Item.col = col;
		if (row == 0)
		{
			Item.nFormat = DT_RIGHT|DT_WORDBREAK|DT_NOPREFIX;
			Item.strText.Format(rgtsz[col]);
			m_pGridCtrl->SetItemBkColour(row,col,CLR_GRAY);
		}
		else
		{  
			CString sRecvStatus;
			int nStatus = m_pDoc->m_StationPlanInfo_ImportantNew.m_StationPlan[row-1].m_StageInfo[m_StageNum].bReceiveFlag;
			if(nStatus == (ushort)CLBTH::MsgWordPhasePlanStatus::WORDPHASE_CONFIG_WRONG )
				sRecvStatus ="配置错误";
			else if (nStatus == (ushort)CLBTH::MsgWordPhasePlanStatus::WORDPHASE_ENTITY_DEAD )
				sRecvStatus ="断开";  ////站死
			else if (nStatus == (ushort)CLBTH::MsgWordPhasePlanStatus::WORDPHASE_ERROR )
				sRecvStatus ="错误";
			else if (nStatus == (ushort)CLBTH::MsgWordPhasePlanStatus::WORDPHASE_INIT )
				sRecvStatus = " "; //初始化
			else if (nStatus == (ushort)CLBTH::MsgWordPhasePlanStatus::WORDPHASE_RECV )
				sRecvStatus ="是";
			else if (nStatus == (ushort)CLBTH::MsgWordPhasePlanStatus::WORDPHASE_SENDING )
				sRecvStatus ="否";
			else if (nStatus == (ushort)CLBTH::MsgWordPhasePlanStatus::WORDPHASE_SIGN )
				sRecvStatus ="是";

			switch(col)
			{
				case 0://实体名称
					Item.nFormat = dwTextStyle;
					Item.strText.Format(_T("%s"),
						m_pDoc->m_StationPlanInfo_ImportantNew.m_StationPlan[row-1].EntityName);			 
					m_pGridCtrl->SetItemFgColour (row,col,CLR_BLUE);
					break;
				case 1://下发时间
					t_long = m_pDoc->m_StationPlanInfo_ImportantNew.m_StationPlan[row-1].m_StageInfo[m_StageNum].send_time;
					if(!t_long)
						break;
					t_Recv = CTime(t_long);
					Item.strText.Format(_T("%2d:%2d"),t_Recv.GetHour(),t_Recv.GetMinute());
					break;
				case 2://接收状态
					Item.nFormat = dwTextStyle;
					if( nStatus == (ushort)CLBTH::MsgWordPhasePlanStatus::WORDPHASE_RECV ||
						nStatus == (ushort)CLBTH::MsgWordPhasePlanStatus::WORDPHASE_SIGN)
					{
						Item.strText = sRecvStatus;
						m_pGridCtrl->SetItemFgColour (row,col,CLR_GREEN);
					}
					else
					{
						Item.strText = sRecvStatus;
						m_pGridCtrl->SetItemFgColour (row,col,CLR_INDIANRED);
					}
					break;
				case 3://签收状态
					Item.nFormat = dwTextStyle;
					if(nStatus == (ushort)CLBTH::MsgWordPhasePlanStatus::WORDPHASE_SIGN)
					{
						Item.strText = sRecvStatus;
						m_pGridCtrl->SetItemFgColour (row,col,CLR_GREEN);
					}
					else
					{
						Item.strText = "--  ";
						m_pGridCtrl->SetItemFgColour (row,col,CLR_INDIANRED);
					}
					break;
				case 4://签收人
					Item.nFormat = dwTextStyle;
					Item.strText.Format("%s",m_pDoc->m_StationPlanInfo_ImportantNew.m_StationPlan[row-1].m_StageInfo[m_StageNum].strSignName);
					break;					
				default:
					Item.nFormat = dwTextStyle;
					break;
			}			
		}
		m_pGridCtrl->SetItem(&Item);			
	}
}

void CPageNotepadPlan::FillGridCtrl()
{
	m_nRows =  m_pDoc->m_StationPlanInfo_ImportantNew.nEntityNum +1;	
	try 
	{
		m_pGridCtrl->SetRowCount(m_nRows);
		m_pGridCtrl->SetColumnCount(m_nCols);
		m_pGridCtrl->SetFixedRowCount(m_nFixRows);
		m_pGridCtrl->SetFixedColumnCount(m_nFixCols);
	}
	catch (CMemoryException* e)
	{
		e->ReportError();
		e->Delete();
		return;
	}		

	// fill rows/cols with text

	for (int row = 0; row <= m_pDoc->m_StationPlanInfo_ImportantNew.nEntityNum; row++)
		RefreshSendStatusRow(row);
	m_pGridCtrl->ExpandColumnsToFit();		
}

BOOL CPageNotepadPlan::InitialGridCtrl()
{
	if (m_pGridCtrl == NULL)
	{
		// Create the Gridctrl object
		m_pGridCtrl = new CGridCtrl;
		if (!m_pGridCtrl) return FALSE;

		CStatic *pWnd;
		pWnd = (CStatic *)GetDlgItem(IDC_GRID);
		CRect rect;
		pWnd->GetWindowRect(rect);
		ScreenToClient(&rect); 

		m_pGridCtrl->Create(rect, this, IDC_GRID);

		// fill it up with stuff
		m_pGridCtrl->SetEditable(FALSE);
		m_pGridCtrl->EnableDragAndDrop(FALSE);
	}
	return TRUE;
}

void CPageNotepadPlan::commonControl()
{
	if(m_pGridCtrl == NULL)
	{
		return;
	}

	m_pGridCtrl->DeleteAllItems();
	FillGridCtrl();
	m_pGridCtrl->Refresh();
	m_Content.Clear();
	m_strCommander.Empty();
	m_strSendTime.Empty();
	m_strSendTimeEx.Empty();

	m_Content.SetWindowText(m_pDoc->m_StationPlanInfo_ImportantNew.text_content[m_StageNum]);
	m_strCommander = m_pDoc->m_StationPlanInfo_ImportantNew.text_commander[m_StageNum];
	m_strSendTime = m_pDoc->m_StationPlanInfo_ImportantNew.text_command_time[m_StageNum];
	m_strSendTimeEx = m_pDoc->m_StationPlanInfo_ImportantNew.text_send_time[m_StageNum];
	m_cboWeagherStart.SetCurSel(m_pDoc->m_StationPlanInfo_ImportantNew.start_st_weather[m_StageNum]);
	m_cboWeagherEnd.SetCurSel(m_pDoc->m_StationPlanInfo_ImportantNew.end_st_weather[m_StageNum]);
	UpdateData(FALSE);
}

void CPageNotepadPlan::SendMessageToCad(int flag)
{
	static unsigned long pre_tickcount2 = 0;
	unsigned long cur_tickcount = GetTickCount();
	if(cur_tickcount - pre_tickcount2<10*1000 )
	{
		AfxMessageBox("两次发送阶段记事时间不能小于10秒");
		return;
	}

	CString phaseWordPlanLog;

	CLBTH::MsgWordPhasePlanSend mMsgWordPhasePlanSend;

	mMsgWordPhasePlanSend.shift_id =m_pDoc->GetCurrentShiftID();
	mMsgWordPhasePlanSend.con_id = m_pDoc->m_nCurrentSectionNo;
	mMsgWordPhasePlanSend.schd_type = CTG_ScheduleInfo::SCHEDULE_TYPE_WORK; 
	mMsgWordPhasePlanSend.phase_id = (byte)m_StageNum;
	mMsgWordPhasePlanSend.recv_count = nSelectedNum;

	GPCENTITY entityInfo;
	gpcdef.GetEntityInfo(gpcdef.GetCurUserName(),entityInfo);	
	mMsgWordPhasePlanSend.send_entity = entityInfo.nEntityId;

	phaseWordPlanLog.Format("下发阶段记事 send_entity %d shift_id %d con_id %d schd_type %d phase_id %d rec_entity_count %d",
		mMsgWordPhasePlanSend.send_entity, mMsgWordPhasePlanSend.shift_id, mMsgWordPhasePlanSend.con_id, mMsgWordPhasePlanSend.schd_type,
		mMsgWordPhasePlanSend.phase_id, mMsgWordPhasePlanSend.recv_count);

	GpcLogTrainMessage(0, 0, phaseWordPlanLog);

	for(int j = 0;j<nSelectedNum&&j<128;j++)
	{	 			
		mMsgWordPhasePlanSend.recv_list[j] = m_pDoc->m_StationPlanInfo_ImportantNew.m_StationPlan[j].Entityid;	

		phaseWordPlanLog.Format("接收实体 %d", mMsgWordPhasePlanSend.recv_list[j]);
		GpcLogTrainMessage(0, 0, phaseWordPlanLog);
	}
	mMsgWordPhasePlanSend.send_time = m_pDoc->GetCurrentTGDLongTime();
	mMsgWordPhasePlanSend.edit_time = m_pDoc->GetCurrentTGDLongTime();

	int len_of_name = sizeof(mMsgWordPhasePlanSend.send_name)/sizeof(mMsgWordPhasePlanSend.send_name[0]);
	memset(mMsgWordPhasePlanSend.send_name, 0, len_of_name);
	memset(mMsgWordPhasePlanSend.permit_name, 0, len_of_name); 


	strncpy(mMsgWordPhasePlanSend.send_name, gpcdef.GetCurrentUsername(), len_of_name);
	mMsgWordPhasePlanSend.send_name[len_of_name-1] = 0;
	strncpy(mMsgWordPhasePlanSend.permit_name, m_strCommander, len_of_name); 
	mMsgWordPhasePlanSend.permit_name[len_of_name-1] = 0;

	phaseWordPlanLog.Format("send_time %d edit_time %d, send_name %s permit_name %s", 
		mMsgWordPhasePlanSend.send_time, mMsgWordPhasePlanSend.edit_time,
		mMsgWordPhasePlanSend.send_name, mMsgWordPhasePlanSend.permit_name);

	GpcLogTrainMessage(0, 0, phaseWordPlanLog);

	mMsgWordPhasePlanSend.start_sta_weather = (ushort)m_cboWeagherStart.GetCurSel();
	mMsgWordPhasePlanSend.end_sta_weather = (ushort)m_cboWeagherEnd.GetCurSel();

	phaseWordPlanLog.Format("保存,阶段记事天气 phase_id %d start_sta_weather %d end_sta_weather %d", 
		mMsgWordPhasePlanSend.phase_id, 
		mMsgWordPhasePlanSend.start_sta_weather, mMsgWordPhasePlanSend.end_sta_weather);

	GpcLogTrainMessage(0, 0, phaseWordPlanLog);

	CString strContent;
	m_Content.GetWindowText(strContent);	

	int max_len = sizeof(mMsgWordPhasePlanSend.phase_content)/sizeof(mMsgWordPhasePlanSend.phase_content[0]);
	memset(mMsgWordPhasePlanSend.phase_content, 0, max_len);

	if((3+ m_strCommander.GetLength() 
		+ m_strSendTime.GetLength() 
		+ m_strSendTimeEx.GetLength() 
		+ strContent.GetLength())	>= max_len)
	{
		CString log;
		log.Format("阶段记事最大长度不能超过 %d字节,请适当缩减内容", max_len);
		AfxMessageBox(log);
		return;
	}

	char  buff[2096],*ptr;
	ptr = buff;
	strncpy(ptr,(char*)(LPCTSTR)m_strCommander,m_strCommander.GetLength());
	ptr += m_strCommander.GetLength();
	*ptr = '&';
	ptr ++;

	phaseWordPlanLog.Format("批准人: %s", m_strCommander);
	GpcLogTrainMessage(0, 0, phaseWordPlanLog);

	strncpy(ptr,(char*)(LPCTSTR)m_strSendTime,m_strSendTime.GetLength());
	ptr += m_strSendTime.GetLength();
	*ptr = '&';
	ptr ++;

	strncpy(ptr,(char*)(LPCTSTR)m_strSendTimeEx,m_strSendTimeEx.GetLength());
	ptr += m_strSendTimeEx.GetLength();
	*ptr = '&';
	ptr ++;

	int len = strContent.GetLength();
	strncpy(ptr,strContent,len);
	ptr += len;

	phaseWordPlanLog.Format("内容: %s", strContent);
	if(phaseWordPlanLog.GetLength() > 256)
		phaseWordPlanLog = phaseWordPlanLog.Left(250);
	GpcLogTrainMessage(0, 0, phaseWordPlanLog);

	mMsgWordPhasePlanSend.phase_len =  ptr-buff; 
	strncpy(mMsgWordPhasePlanSend.phase_content,buff,ptr-buff);
	mMsgWordPhasePlanSend.phase_content[max_len-1]=0;


	if (flag)  //0 下发 1 保存
		mMsgWordPhasePlanSend.phase_oper = CLBTH:: MsgWordPhasePlanSend::WORDPHASE_SAVE;
	else
		mMsgWordPhasePlanSend.phase_oper = CLBTH:: MsgWordPhasePlanSend::WORDPHASE_SEND;

	long eid=GetSpecialEntityID(SERVER_TYPE_DB, mMsgWordPhasePlanSend.con_id);
	SendTGBMsg(mMsgWordPhasePlanSend.con_id,&mMsgWordPhasePlanSend,eid);

	if(flag)
	{
		GpcLogTrainMessage(0, 0, "提交 %d 保存", eid);
	}
	else
	{
		GpcLogTrainMessage(0, 0, "提交 %d 下发", eid);
	}

	pre_tickcount2 = cur_tickcount;

	CWnd *pWnd = GetDlgItem(IDC_EDIT_SEND_TIME);
	pWnd->SetWindowText(m_strSendTimeEx);
}

void CPageNotepadPlan::OnSave()
{
	UpdateData(TRUE);

	nSelectedNum = 0;	
	CTime time;	
	long usDateValue,usTimeValue;
	CString str;

	usTimeValue = 0;
	usDateValue = 0;	
	sendTime = usTimeValue / 60;
	sendDate = usDateValue;

	for (int row = 1; row <= m_pDoc->m_StationPlanInfo_ImportantNew.nEntityNum; row++)
	{
		nSelectedList[nSelectedNum++] = m_pDoc->m_StationPlanInfo_ImportantNew.m_StationPlan[row-1].nStation;
		m_pDoc->m_StationPlanInfo_ImportantNew.m_StationPlan[row-1].m_StageInfo[m_StageNum].usSendDate = usDateValue;
		m_pDoc->m_StationPlanInfo_ImportantNew.m_StationPlan[row-1].m_StageInfo[m_StageNum].usSendTime = usTimeValue;

		m_pDoc->m_StationPlanInfo_ImportantNew.m_StationPlan[row-1].m_StageInfo[m_StageNum].bSignInFlag = (ushort) CLBTH::MsgWordPhasePlanStatus::WORDPHASE_INIT;
		m_pDoc->m_StationPlanInfo_ImportantNew.m_StationPlan[row-1].m_StageInfo[m_StageNum].bReceiveFlag = (ushort) CLBTH::MsgWordPhasePlanStatus::WORDPHASE_INIT;
		m_pDoc->m_StationPlanInfo_ImportantNew.m_StationPlan[row-1].m_StageInfo[m_StageNum].strSignName[0] = 0x00;

		str = OsConvertTimetoString(m_pDoc->m_StationPlanInfo_ImportantNew.m_StationPlan[row-1].m_StageInfo[m_StageNum].usSendTime);
		str = str.Left(5);
		m_pGridCtrl->SetItemText (row,1,str);

		str = "否";
		m_pGridCtrl->SetItemFgColour (row,2,CLR_INDIANRED);
		m_pGridCtrl->SetItemText (row,2,str);
		str = "否";
		m_pGridCtrl->SetItemFgColour (row,3,CLR_INDIANRED);
		m_pGridCtrl->SetItemText (row,3,str);
		str = "";
		m_pGridCtrl->SetItemText (row,4,str);
	}

	m_Content.GetWindowText(m_pDoc->m_StationPlanInfo_ImportantNew.text_content[m_StageNum]);


	m_pGridCtrl->UpdateData();

	SendMessageToCad(1);

	Invalidate(FALSE);
}

BOOL CPageNotepadPlan::CleanLocal()
{
	m_pDoc->ClearPhasePlanInfo();
	m_Content.SetWindowText("");
	return TRUE;
}

void CPageNotepadPlan::OnLoad()
{
	CleanLocal();
	FillGridCtrl();
	LoadWorkPhasePlan();
}

void CPageNotepadPlan::LoadWorkPhasePlan()
{ 
	// 请求
	CLBTH::MsgWordPhasePlanQuery mMsgWordPhasePlanQuery;

	mMsgWordPhasePlanQuery.shift_id = m_pDoc->GetCurrentShiftID();
	mMsgWordPhasePlanQuery.con_id = m_pDoc->m_nCurrentSectionNo;
	mMsgWordPhasePlanQuery.schd_type = (byte)m_pDoc->GetScheduleType();
	mMsgWordPhasePlanQuery.phase_count = 4;

	mMsgWordPhasePlanQuery.phase_id[0] = 3;
	mMsgWordPhasePlanQuery.phase_id[1] = 0;
	mMsgWordPhasePlanQuery.phase_id[2] = 1;
	mMsgWordPhasePlanQuery.phase_id[3] = 2;


	long eid=GetSpecialEntityID(SERVER_TYPE_DB,mMsgWordPhasePlanQuery.con_id);
	SendTGBMsg(mMsgWordPhasePlanQuery.con_id,&mMsgWordPhasePlanQuery,eid);

	CString noteText;
	noteText.Format("向服务器申请阶段记事: svr_entity %d shift_id %d con_id %d schd_type %d",
		eid, 
		mMsgWordPhasePlanQuery.shift_id, 
		mMsgWordPhasePlanQuery.con_id, 
		mMsgWordPhasePlanQuery.schd_type);

	GpcLogTrainMessage(0, 0, noteText);

	return;
}

void CPageNotepadPlan::OnSetfocusEditContent() 
{
	g_curEdit =(CEdit*)GetDlgItem(IDC_EDIT_CONTENT);
}


void CPageNotepadPlan::OnSetfocusEditCommander() 
{
	g_curEdit =(CEdit*)GetDlgItem(IDC_EDIT_COMMANDER);
}

void CPageNotepadPlan::OnSetfocusEditTime() 
{
	g_curEdit =NULL;
}

void CPageNotepadPlan::OnSetfocusEditSendTime() 
{
	g_curEdit =NULL;
}
