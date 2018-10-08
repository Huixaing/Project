// SysAlarmPage.cpp : implementation file
//

#include "stdafx.h"
#include "icommtask.h"
#include "windowsx.h"
#include "SysAlarmPage.h"

#include "Mmsystem.h"
#include "memshare.h"
#include <oscalls.h>

#ifdef _DEBUG
//#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


USHORT  hQueueSystemAlarm = 0;
HWND    hWndSystemAlarm = 0;

enum { UM_SYSTEM_ALARM = WM_USER + 0,
	   MAX_LIST_ITEM_COUNT=1000};

SYSALARM_ANSWER_MAP& getAlarmsgAnswerMap()
{
	static SYSALARM_ANSWER_MAP Entries;
	return Entries;
}

bool registerAlarmsg(CLBTH::MSGHEAD::msgid_t msgid,	 PFN_ALARM_ANSWER pfn)
{
	ASSERT(msgid != CLBTH::MsgAlarm::getTypeIDStatic());
	bool b = getAlarmsgAnswerMap().insert(
		SYSALARM_ANSWER_MAP::value_type(msgid, pfn)).second;
	ASSERT(b);
	return b;
}

/////////////////////////////////////////////////////////////////////////////
// CSysAlarmPage property page

IMPLEMENT_DYNCREATE(CSysAlarmPage, CPropertyPage)
CSysAlarmPage::CSysAlarmPage() : CPropertyPage(CSysAlarmPage::IDD)
{
	//{{AFX_DATA_INIT(CSysAlarmPage)
	//}}AFX_DATA_INIT
	m_imageWidth = 16;
	m_imageHeight = 16;
}

CSysAlarmPage::~CSysAlarmPage()
{
	//ALARM_DATA::iterator iter = m_alarm_data.begin();
	//for(; iter != m_alarm_data.end(); iter++)
	//{
	//	if(NULL == (*iter).pAlarm)
	//		continue;
	//	CLBTH::BaseMsgFactory::deleteObject((*iter).pAlarm);
	//	(*iter).pAlarm = NULL;
	//}
}

void CSysAlarmPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSysAlarmPage)
	DDX_Control(pDX, IDC_ALARM_LIST, m_list);
	DDX_Control(pDX, IDC_ALARM_LISTNEEDANSWER, m_listNeedAnswer);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSysAlarmPage, CPropertyPage)
	//{{AFX_MSG_MAP(CSysAlarmPage)
	//ON_NOTIFY(LVN_GETDISPINFO, IDC_ALARM_LIST, OnGetdispinfo)
	//ON_NOTIFY(LVN_GETDISPINFO, IDC_ALARM_LISTNEEDANSWER, OnGetdispinfo)
	ON_NOTIFY(NM_DBLCLK, IDC_ALARM_LISTNEEDANSWER, OnDblclkAlarmList)
	ON_WM_CREATE()
	ON_NOTIFY(HDN_ITEMCLICK, IDC_ALARM_LIST, OnItemclickAlarmList)
	ON_NOTIFY(NM_RCLICK, IDC_ALARM_LIST, OnRclickAlarmList)
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
	//}}AFX_MSG_MAP
	ON_MESSAGE(UM_UNI_MESSAGE, OnAlarm)
	ON_COMMAND(ID_ALARM_SYS, OnAlarmSys)
	ON_UPDATE_COMMAND_UI(ID_ALARM_SYS, OnUpdateAlarmSys)
	ON_COMMAND(ID_ALARM_TRAIN, OnAlarmTrain)
	ON_UPDATE_COMMAND_UI(ID_ALARM_TRAIN, OnUpdateAlarmTrain)
	ON_COMMAND(ID_ALARM_UPDATE_SCROLL, OnAlarmUpdateScroll)
	ON_UPDATE_COMMAND_UI(ID_ALARM_UPDATE_SCROLL, OnUpdateAlarmUpdateScroll)
	ON_COMMAND(ID_ALARM_SHOWLOG, OnAlarmShowlog)
	ON_COMMAND(ID_ALARM_EMPTY, OnAlarmEmpty)
	ON_COMMAND(ID_ALARM_LVL1, OnAlarmLvl1)
	ON_UPDATE_COMMAND_UI(ID_ALARM_LVL1, OnUpdateAlarmLvl1)
	ON_COMMAND(ID_ALARM_LVL2, OnAlarmLvl2)
	ON_UPDATE_COMMAND_UI(ID_ALARM_LVL2, OnUpdateAlarmLvl2)
	ON_COMMAND(ID_ALARM_LVL3, OnAlarmLvl3)
	ON_UPDATE_COMMAND_UI(ID_ALARM_LVL3, OnUpdateAlarmLvl3)
	ON_COMMAND(ID_SYSALARM_SETFILTER, OnSysalarmSetfilter)
	ON_UPDATE_COMMAND_UI(ID_SYSALARM_SETFILTER, OnUpdateSysalarmSetfilter)
	ON_COMMAND(ID_SYSALARM_UNSETFILTER, OnSysalarmUnsetfilter)
	ON_UPDATE_COMMAND_UI(ID_SYSALARM_UNSETFILTER, OnUpdateSysalarmUnsetfilter)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSysAlarmPage message handlers

int CSysAlarmPage::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CPropertyPage::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	hWndSystemAlarm = GetSafeHwnd();

	return 0;
}

void CSysAlarmPage::OnSize(UINT nType, int cx, int cy) 
{
	CPropertyPage::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	HWND hWnd = m_list.GetSafeHwnd();
	if(hWnd)
	{
		CRect rectClient,rect;
		GetClientRect(&rectClient);
		rect=rectClient;

		rect.bottom=rectClient.top+rectClient.Height()*1/2; 
		m_listNeedAnswer.MoveWindow(&rect);

		rect=rectClient;
		rect.top=rectClient.top+rectClient.Height()*1/2;
		rect.bottom=rectClient.bottom; 
		m_list.MoveWindow(&rect);
	}
}

#include "AlarmSoundConfig.h"
#include ".\sysalarmpage.h"
CAlarmSoundConfig alarmSoundConfig;

enum {
	LEVEL_IDX = 0,
	CLASS_IDX,
	TIME_IDX,
	DESC_IDX,
	FROM_IDX
};

static char *label[] = { "级别", "类型", "时间", "描述(一般报警)", "来源" };
static char *labelNeedAnswer[] = { "级别", "类型", "时间", "描述(重要报警)", "来源" };
static int  width[] = {75, 45, 100, 300, 80, 0};
static int  fmt[] = {LVCFMT_LEFT, LVCFMT_LEFT, LVCFMT_LEFT, LVCFMT_LEFT, LVCFMT_LEFT, LVCFMT_LEFT};

char *typeTXT[] = { "行车", "系统"};
char *levelTXT[] = { "错误", "警告", "信息" };
BOOL CSysAlarmPage::OnInitDialog() 
{
	int i;
	CPropertyPage::OnInitDialog();
	
	// TODO: Add extra initialization here
	if(!alarmSoundConfig.loadProfile(gpcdef.GetDataPathName()))
		return FALSE;

	m_bLevelOne = TRUE;
	m_bLevelTwo = TRUE;
	m_bLevelThree = TRUE;
	if(CAlarmSoundConfig::LEVEL_INFO *pLevel_1 = alarmSoundConfig.levelInfo.getContentFromID(1))
	{
		m_bLevelOne = pLevel_1->nFlag;
	}
	if(CAlarmSoundConfig::LEVEL_INFO *pLevel_2 = alarmSoundConfig.levelInfo.getContentFromID(2))
	{
		m_bLevelTwo = pLevel_2->nFlag;
	}
	if(CAlarmSoundConfig::LEVEL_INFO *pLevel_3 = alarmSoundConfig.levelInfo.getContentFromID(3))
	{
		m_bLevelThree = pLevel_3->nFlag;
	}

	m_bClassSys = TRUE;
	m_bClassApp = TRUE;
	if(CAlarmSoundConfig::TYPE_INFO *pType_app = alarmSoundConfig.typeInfo.getContentFromID(0))
	{
		m_bClassApp = pType_app->nFlag;
	}
	if(CAlarmSoundConfig::TYPE_INFO *pType_sys = alarmSoundConfig.typeInfo.getContentFromID(1))
	{
		m_bClassSys = pType_sys->nFlag;
	}

	m_bOtherAlarmSel = TRUE;
	m_bOtherStationSel = TRUE;
	m_bUpdateScroll= TRUE;
	m_bEnableFilter = FALSE;	//缺省不过滤
	UpdateData(FALSE);

	m_SmallImageList.Create("IDB_ALARMSMALLICON", m_imageWidth, 4, RGB(255, 255, 255));
	m_StateImageList.Create("IDB_ALARMSTATEICON", m_imageWidth, 2, RGB(255, 255, 255));

	m_SmallImageListNeedAnswer.Create("IDB_ALARMSMALLICON", m_imageWidth, 4, RGB(255, 255, 255));
	m_StateImageListNeedAnswer.Create("IDB_ALARMSTATEICON", m_imageWidth, 2, RGB(255, 255, 255));

	m_SmallImageList.SetOverlayImage(3, 1);
	m_SmallImageListNeedAnswer.SetOverlayImage(3, 1);

	m_list.SetImageList(&m_SmallImageList, LVSIL_SMALL);
	m_list.SetImageList(&m_StateImageList, LVSIL_STATE);
	m_list.SetHeaderLabel(sizeof(label)/sizeof(label[0]), label, width, fmt);

	m_listNeedAnswer.SetImageList(&m_SmallImageList, LVSIL_SMALL);
	m_listNeedAnswer.SetImageList(&m_StateImageList, LVSIL_STATE);
	m_listNeedAnswer.SetHeaderLabel(sizeof(labelNeedAnswer)/sizeof(labelNeedAnswer[0]), labelNeedAnswer, width, fmt);

	vector<int> sec_list;
	gpcdef.GetSectionListOfDisp( sec_list );
	for(i=0; i<sec_list.size(); i++)
	{
		vector<int> sta_list;
		gpcdef.GetStationListofDisp(sec_list[i], sta_list);
		for(int j=0; j<sta_list.size(); j++)
		{
			m_StationFilter[sta_list[j]] = true;
		}
	}
	for(int i=0; i<alarmSoundConfig.idInfo.size(); i++)
	{
		CAlarmSoundConfig::IDINFO& id = alarmSoundConfig.idInfo[i];
		if(-1 == id.nID)
		{
			m_bOtherAlarmSel = id.nFlag;
		}
		else
		{
			if(!id.isCannotChanged())
				m_AlarmIDFilter[id.nID] = !id.isHide();
		}
	}
/*
	for(i=0; i<m_StationList.size(); i++)
	{
		CString str = gpcdef.GetStationName(m_StationList[i].nStationNo);
		int idx = m_SourceBox.AddString(str);
		if(idx != CB_ERR && idx != CB_ERRSPACE)
			m_SourceBox.SetItemData(idx, i);
	}

	int idx = m_SourceBox.AddString("全部");
	if(idx != CB_ERR && idx != CB_ERRSPACE)
		m_SourceBox.SetItemData(idx, -1);
	m_SourceBox.SetCurSel(0);
*/
	RegisterMsgProcHandleWnd(GetSafeHwnd(),MSGID_MsgAlarm,1);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

//void CSysAlarmPage::OnGetdispinfo(NMHDR* pNMHDR, LRESULT* pResult) 
//{
//	static CString txt;
//	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
//	// TODO: Add your control notification handler code here
//	int iItem = pDispInfo->item.iItem;
//	int iSubItem = pDispInfo->item.iSubItem;
//
//	ELEMENT *pEle = NULL;
//	int nClass;
//	int nLevel;
//	
//	int i=0;
//	for(ALARM_DATA::iterator iter = m_alarm_data.begin();
//		iter != m_alarm_data.end(); iter ++)
//	{
//		pEle = &(*iter);
//		switch(pEle->nLevel)
//		{
//		case 1:
//			nLevel = ERROR_LOG;		break;
//		case 2:
//			nLevel = WARNING_LOG;	break;
//		case 3:
//		default:
//			nLevel = INFORM_LOG;	break;
//		}
//		switch(pEle->nClass)
//		{
//		case 0:
//			nClass = APP_REMOTE;	break;
//		case 1:
//		default:
//			nClass = SYS_REMOTE;	break;
//		}
//
//		if(!m_bLevelOne && nLevel == ERROR_LOG
//			|| !m_bLevelTwo && nLevel == WARNING_LOG
//			|| !m_bLevelThree && nLevel == INFORM_LOG
//			|| !m_bClassSys && nClass == SYS_REMOTE
//			|| !m_bClassApp && nClass == APP_REMOTE)
//			continue;
//
//		if(i==iItem)
//			break;
//		i++;
//	}
//
//	if(pEle)
//	{
//		*pResult = 0;
//		return;
//	}
//
//	m_list.SetItemData(iItem, (DWORD)pEle);
//
//	switch(iSubItem)
//	{
//	case 0:
//		pDispInfo->item.mask = LVIF_TEXT | LVIF_IMAGE;
//		pDispInfo->item.iImage= nLevel;
//		txt.Format("%d级", pEle->nLevel);
//		pDispInfo->item.pszText = (LPSTR)(LPCSTR)txt;
//		if(pEle->bNeedRespond)
//		{
//			pDispInfo->item.mask |= LVIF_STATE;
//			pDispInfo->item.stateMask = LVIS_STATEIMAGEMASK;
//			if(pEle->bAnswered)
//			{
//				pDispInfo->item.stateMask |= LVIS_OVERLAYMASK;
//				pDispInfo->item.state = INDEXTOSTATEIMAGEMASK(2)|INDEXTOOVERLAYMASK(1);
//			}
//			else
//				pDispInfo->item.state = INDEXTOSTATEIMAGEMASK(1);
//		}
//		break;
//	case 1:
//		pDispInfo->item.mask = LVIF_TEXT;
//		pDispInfo->item.pszText = typeTXT[nClass];
//		break;
//	case 2:
//		pDispInfo->item.mask = LVIF_TEXT;
//		{
//			CTime ct(pEle->tm);
//			txt.Format("%02d-%02d %02d:%02d:%02d",
//				ct.GetMonth(), ct.GetDay(),
//				ct.GetHour(), ct.GetMinute(),ct.GetSecond());
//		}
//		pDispInfo->item.pszText = (LPSTR)(LPCSTR)txt;
//		break;
//	case 3:
//		pDispInfo->item.pszText = (LPSTR)(LPCSTR)pEle->szContent;	//描述
//		break;
//	case 4:
//		pDispInfo->item.pszText = (LPSTR)(LPCSTR)txt;	//来源
//		break;
//	};
//	*pResult = 0;
//}
//
void CSysAlarmPage::OnDblclkAlarmList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	*pResult = 0;
}

static void  SaveAlarmMessage(int source_id,CLBTH::MsgAlarm *pAlarm)
{

	SaveLogMessage("ctcinfo",pAlarm->nTime,"级别%d,类别%d,来源%d,%s",pAlarm->nVerbose,	pAlarm->nType,source_id,pAlarm->szContent);
}


bool CSysAlarmPage::IsAlarmHide(int nAlarmID, int nLevel, int nType, int nStaID)
{
	CAlarmSoundConfig::IDINFO *pid = alarmSoundConfig.idInfo.getContentFromID(nAlarmID);

	//不可变更的报警，依据配置
	if(pid && pid->isCannotChanged())
	{
		return pid->isHide();
	}

	//没有过滤条件时，全显示
	if(!m_bEnableFilter)
		return false;

	//判断当前记录所属的报警有无被查询条件选中
	if(m_AlarmIDFilter.end() != m_AlarmIDFilter.find(nAlarmID))
	{
		if(!m_AlarmIDFilter[nAlarmID])	//报警被过滤
			return true;
	}
	else
	{	//其它报警
		if(!m_bOtherAlarmSel)
			return true;
	}

	if(!m_bLevelOne && nLevel == 1//ERROR_LOG
		|| !m_bLevelTwo && nLevel == 2//WARNING_LOG
		|| !m_bLevelThree && nLevel >= 3)//INFORM_LOG
	{
		return true;
	}
	if(!m_bClassSys && nType == 1//SYS_REMOTE
		|| !m_bClassApp && nType != 1)//APP_REMOTE
	{
		return true;
	}
	//|| m_SourceEntityID != -1 && ele.SourceEntityID != m_SourceEntityID

	//判断当前记录所属的站号有无被查询条件选中
	if(m_StationFilter.end() != m_StationFilter.find(nStaID))
	{
		if(!m_StationFilter[nStaID])
			return true;
	}
	else
	{
		if(!m_bOtherStationSel)
			return true;
	}

	return false;
}

LRESULT CSysAlarmPage::OnAlarm(WPARAM wParam, LPARAM lParam)
{
	UNI_MESSAGE_STRUCT *pMsgStruct=( UNI_MESSAGE_STRUCT *)lParam;
    CLBTH::MsgAlarm *pAlarmPara= ( CLBTH::MsgAlarm *)pMsgStruct->pmsg;
	CLBTH::MsgAlarm *pAlarm=pAlarmPara;

	ELEMENT ele;
	ele.SourceEntityID = pMsgStruct->source_id;

	do {
		if(NULL == pAlarm)
		{
			sysprint(0, 0, "[SYSALARM]: 报警消息内容不正确 ");
			break;
		}

		if(pAlarm->nTime == 0)
		{
			time_t t = time_t(pAlarm->nTime);
			time(&t);
		}

		CAlarmSoundConfig::IDINFO *pid = alarmSoundConfig.idInfo.getContentFromID(pAlarm->nAlarmID);
		if(pid)
		{	//重新定义报警参数
			if(pid->nLevel > 0)
				pAlarm->nVerbose = pid->nLevel;
			if(pid->nType > 0)
				pAlarm->nType = pid->nType -1;
		}

		SaveAlarmMessage(pMsgStruct->source_id,pAlarm);

				ele.tm = pAlarm->nTime;
				ele.nLevel = pAlarm->nVerbose;
				ele.nClass = pAlarm->nType;
				ele.szContent = pAlarm->szContent;
				ele.nAlarmID = pAlarm->nAlarmID;
				ele.bNeedRespond = pAlarm->NeedRespond() ? 1 : 0;

				int nStaID = 0;
				CString szStaSndFile;
				GPCENTITY entity;
				if(gpcdef.GetEntityInfo(ele.SourceEntityID, entity))
				{
					ele.szSourceTitle = entity.sName;
					if(entity.nGroupId == 102)
						szStaSndFile.Format("Sta%d.wav", entity.nSpecial1);

					nStaID = entity.nSpecial1;
				}
				else
				{
					ele.szSourceTitle.Format("%d", ele.SourceEntityID);
				}

				if(IsAlarmHide(ele.nAlarmID, ele.nLevel, ele.nClass, nStaID))
					break;

				if(ele.nLevel>1)
					AddElementToListBox(&m_list,&ele,TRUE,m_bUpdateScroll);
				else
					AddElementToListBox(&m_listNeedAnswer,&ele);

				CString szTypeSndFile = alarmSoundConfig.GetSoundFileName(ele.nAlarmID);
				if( !szTypeSndFile.IsEmpty() && !szStaSndFile.IsEmpty() )
				{
					CString path;
					path.Format("%s\\Sound\\alarm\\", gpcdef.GetDataPathName());

					PlaySound(path + szStaSndFile, NULL, SND_FILENAME|SND_NOSTOP);
					PlaySound(path + szTypeSndFile, NULL, SND_FILENAME|SND_ASYNC);
				}
	}while(0);

	FREE_UNI_MESSAGE_STRUCT(pMsgStruct);
	return TRUE;
}


void CSysAlarmPage::OnItemclickAlarmList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	HD_NOTIFY *phdn = (HD_NOTIFY *) pNMHDR;
	*pResult = 0;
}

BOOL CSysAlarmPage::AddElementToListBox(CHeaderList *plist,ELEMENT *pele, BOOL bEnableFilter,BOOL bScroll)
{
	int nClass;
	int nLevel;
	switch(pele->nLevel)
	{
	case 1:
		nLevel = ERROR_LOG;		break;
	case 2:
		nLevel = WARNING_LOG;	break;
	case 3:
		nLevel = INFORM_LOG;	break;
	default:
		nLevel = UNKNOWN_LOG;	break;
	}

	switch(pele->nClass)
	{
	case 0:
		nClass = APP_REMOTE;	break;
	case 1:
	default:
		nClass = SYS_REMOTE;	break;
	}

	while(plist->GetItemCount()>MAX_LIST_ITEM_COUNT)
		plist->DeleteItem(0);

	CString txt;
	LVITEM lvi;
	lvi.mask = LVIF_TEXT | LVIF_IMAGE;
	lvi.iItem = plist->GetItemCount();

	lvi.mask = LVIF_TEXT | LVIF_IMAGE;
	lvi.iSubItem = 0;
	lvi.iImage= nLevel;
	txt.Format("%d级", pele->nLevel);
	lvi.pszText = (LPSTR)(LPCSTR)txt;
	if(pele->bNeedRespond)
	{
		lvi.mask |= LVIF_STATE;
		lvi.stateMask = LVIS_STATEIMAGEMASK;
		if(pele->bAnswered)
		{
			lvi.stateMask |= LVIS_OVERLAYMASK;
			lvi.state = INDEXTOSTATEIMAGEMASK(2)|INDEXTOOVERLAYMASK(1);
		}
		else
			lvi.state = INDEXTOSTATEIMAGEMASK(1);
	}
	lvi.iItem = plist->InsertItem(&lvi);

	lvi.mask = LVIF_TEXT;
	lvi.iSubItem = 1;
	lvi.pszText = typeTXT[nClass];
	plist->SetItem(&lvi);

	lvi.mask = LVIF_TEXT;
	lvi.iSubItem = 2;
	CTime ct(pele->tm);
	txt.Format("%02d:%02d:%02d",
		ct.GetHour(), ct.GetMinute(),ct.GetSecond());
	lvi.pszText = (LPSTR)(LPCSTR)txt;
	plist->SetItem(&lvi);

	lvi.iSubItem = 3;
	lvi.pszText = (LPSTR)(LPCSTR)pele->szContent;	//描述
	plist->SetItem(&lvi);

	lvi.iSubItem = 4;
	lvi.pszText = (LPSTR)(LPCSTR)pele->szSourceTitle;	//来源
	plist->SetItem(&lvi);
	
	//plist->SetItemData(lvi.iItem, (DWORD)pele);//注意: 只有listNeedAnswer 的pele才是可以访问的

	//m_list.EnsureVisible(i, FALSE);

    if(bScroll)
	{
		CRect rect;
		if(plist->GetItemRect(lvi.iItem, &rect, LVIR_BOUNDS))
			plist->Scroll(CSize(0, rect.Height() * (plist->GetItemCount() - plist->GetTopIndex())));
		plist->SetCurSel(plist->GetItemCount()-1);
	}

	return TRUE;
}


void CSysAlarmPage::OnCtcalarmTb() 
{
}

void CSysAlarmPage::OnCtcalarmTb_UpdateUI(CCmdUI* pCmdUI) 
{
}


void CSysAlarmPage::OnRclickAlarmList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	*pResult = 0;
}


void UpdateCommandUI(CWnd *pWnd, CMenu* pMenu) 
{
	if(NULL == pMenu)
		return ;

	CCmdUI state;
	state.m_pMenu = pMenu;
	ASSERT(state.m_pOther == NULL);
	ASSERT(state.m_pParentMenu == NULL);

	state.m_nIndexMax = state.m_pMenu->GetMenuItemCount();
	for (state.m_nIndex = 0; state.m_nIndex < state.m_nIndexMax;
		state.m_nIndex++)
	{
		state.m_nID = state.m_pMenu->GetMenuItemID(state.m_nIndex);
		if (state.m_nID == 0)
			continue; // menu separator or invalid cmd - ignore it

		ASSERT(state.m_pOther == NULL);
		ASSERT(state.m_pMenu != NULL);
		if (state.m_nID == (UINT)-1)
		{
			// possibly a popup menu, route to first item of that popup
			state.m_pSubMenu = state.m_pMenu->GetSubMenu(state.m_nIndex);
			if (state.m_pSubMenu == NULL ||
				(state.m_nID = state.m_pSubMenu->GetMenuItemID(0)) == 0 ||
				state.m_nID == (UINT)-1)
			{
				continue;       // first item of popup can't be routed to
			}
			state.DoUpdate(pWnd, FALSE);    // popups are never auto disabled
			UpdateCommandUI(pWnd, state.m_pSubMenu);
		}
		else
		{
			// normal menu item
			// Auto enable/disable if frame window has 'm_bAutoMenuEnable'
			//    set and command is _not_ a system command.
			state.m_pSubMenu = NULL;
			state.DoUpdate(pWnd, /*m_bAutoMenuEnable &&*/ state.m_nID < 0xF000);
		}

		// adjust for menu deletions and additions
		UINT nCount = state.m_pMenu->GetMenuItemCount();
		if (nCount < state.m_nIndexMax)
		{
			state.m_nIndex -= (state.m_nIndexMax - nCount);
			while (state.m_nIndex < nCount &&
				state.m_pMenu->GetMenuItemID(state.m_nIndex) == state.m_nID)
			{
				state.m_nIndex++;
			}
		}
		state.m_nIndexMax = nCount;
	}
}

void CSysAlarmPage::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	CMenu menu, *pPopupMenu = NULL;
	menu.LoadMenu(IDR_CTCALARM_POPUP);

	pPopupMenu = menu.GetSubMenu(0);
	ASSERT(pPopupMenu);

	UpdateCommandUI(this, pPopupMenu);
	pPopupMenu->TrackPopupMenu(
		TPM_LEFTALIGN | TPM_RIGHTBUTTON,
		point.x, point.y, this,NULL);
}

void CSysAlarmPage::SetAlarmFilter()
{
	CAlarmSoundConfig::IDINFO *pid;
	for(map<int, bool>::iterator iter = m_AlarmIDFilter.begin(); iter != m_AlarmIDFilter.end(); iter ++)
	{
		pid = alarmSoundConfig.idInfo.getContentFromID(iter->first);
		pid->nFlag = iter->second;
	}
	if(pid = alarmSoundConfig.idInfo.getContentFromID(-1))
	{
		pid->nFlag = m_bOtherAlarmSel;
	}

	CAlarmSoundConfig::LEVEL_INFO *pLevel;
	if(pLevel = alarmSoundConfig.levelInfo.getContentFromID(1))
	{
		pLevel->nFlag = m_bLevelOne;
	}
	if(pLevel = alarmSoundConfig.levelInfo.getContentFromID(2))
	{
		pLevel->nFlag = m_bLevelTwo;
	}
	if(pLevel = alarmSoundConfig.levelInfo.getContentFromID(3))
	{
		pLevel->nFlag = m_bLevelThree;
	}

	CAlarmSoundConfig::TYPE_INFO *pType;
	if(pType = alarmSoundConfig.typeInfo.getContentFromID(0))
	{
		pType->nFlag = m_bClassApp;
	}
	if(pType = alarmSoundConfig.typeInfo.getContentFromID(1))
	{
		pType->nFlag = m_bClassSys;
	}

	alarmSoundConfig.saveProfile();

	if(m_bEnableFilter)
	{
		CString sz, str;
		sysprint(5, 0, "[ALARM] 设置报警过滤条件");

		if(!m_bClassApp)
			sysprint(5, 0, "[ALARM] 行车报警 -- 不显示");
		if(!m_bClassSys)
			sysprint(5, 0, "[ALARM] 系统报警 -- 不显示");
		if(!m_bLevelOne)
			sysprint(5, 0, "[ALARM] 1级报警 -- 不显示");
		if(!m_bLevelTwo)
			sysprint(5, 0, "[ALARM] 2级报警 -- 不显示");
		if(!m_bLevelThree)
			sysprint(5, 0, "[ALARM] 3级报警 -- 不显示");
		
		sz.Empty();
		for(map<int, bool>::iterator iter = m_StationFilter.begin(); iter != m_StationFilter.end(); iter ++)
		{
			if(iter->second)
			{
				str.Format("%d, ", iter->first);
				sz += str;
			}
		}
		if(!sz.IsEmpty())
			sysprint(5, 0, "[ALARM] 报警车站 -- %s", sz);
		
		sz.Empty();
		for(map<int, bool>::iterator iter = m_StationFilter.begin(); iter != m_StationFilter.end(); iter ++)
		{
			if(!iter->second)
			{
				str.Format("%d, ", iter->first);
				sz += str;
			}
		}
		if(!sz.IsEmpty())
			sysprint(5, 0, "[ALARM] 不报警车站 -- %s", sz);
		sysprint(5, 0, "[ALARM] 其它车站 -- %s", m_bOtherStationSel ? "报警" : "不报警");
		
		sz.Empty();
		for(map<int, bool>::iterator iter = m_AlarmIDFilter.begin(); iter != m_AlarmIDFilter.end(); iter ++)
		{
			if(iter->second)
			{
				str.Format("%d, ", iter->first);
				sz += str;
			}
		}
		if(!sz.IsEmpty())
			sysprint(5, 0, "[ALARM] 显示的报警 -- %s", sz);
		
		sz.Empty();
		for(map<int, bool>::iterator iter = m_AlarmIDFilter.begin(); iter != m_AlarmIDFilter.end(); iter ++)
		{
			if(!iter->second)
			{
				str.Format("%d, ", iter->first);
				sz += str;
			}
		}
		if(!sz.IsEmpty())
			sysprint(5, 0, "[ALARM] 不显示的报警 -- %s", sz);
		sysprint(5, 0, "[ALARM] 其它报警 -- %s", m_bOtherAlarmSel ? "显示" : "不显示");
	}
	else
	{
		sysprint(0, 0, "[ALARM] 报警不过滤");
	}
}

void CSysAlarmPage::OnSysalarmSetfilter() 
{
	CSysAlarmFilterDlg dlg(m_StationFilter, m_AlarmIDFilter, this);
	dlg.m_bType[0] = m_bClassApp;
	dlg.m_bType[1] = m_bClassSys;
	dlg.m_bLevel[0] = m_bLevelOne;
	dlg.m_bLevel[1] = m_bLevelTwo;
	dlg.m_bLevel[2] = m_bLevelThree;
	dlg.m_bOtherStationSel = m_bOtherStationSel;
	dlg.m_bOtherAlarmSel = m_bOtherAlarmSel;
	if(IDOK == dlg.DoModal())
	{
		m_bClassApp = dlg.m_bType[0];
		m_bClassSys = dlg.m_bType[1];
		m_bLevelOne = dlg.m_bLevel[0];
		m_bLevelTwo = dlg.m_bLevel[1];
		m_bLevelThree = dlg.m_bLevel[2];
		m_bOtherStationSel = dlg.m_bOtherStationSel;
		m_StationFilter = dlg.m_StationFilter;
		m_bOtherAlarmSel = dlg.m_bOtherAlarmSel;
		m_AlarmIDFilter = dlg.m_AlarmIDFilter;
		
		m_bEnableFilter = TRUE;

		SetAlarmFilter();
	}
}

void CSysAlarmPage::OnUpdateSysalarmSetfilter(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_bEnableFilter);
}

void CSysAlarmPage::OnSysalarmUnsetfilter() 
{
	m_bEnableFilter = !m_bEnableFilter;
}

void CSysAlarmPage::OnUpdateSysalarmUnsetfilter(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(!m_bEnableFilter);
}

void CSysAlarmPage::OnAlarmSys()
{
	m_bEnableFilter = TRUE;

	m_bClassSys = !m_bClassSys;
	if(!m_bClassSys && !m_bClassApp)
		m_bClassApp = TRUE;

	SetAlarmFilter();
}

void CSysAlarmPage::OnUpdateAlarmSys(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(!m_bClassSys.bCannotChanged);
	pCmdUI->SetCheck(m_bClassSys);
}

void CSysAlarmPage::OnAlarmTrain()
{
	m_bEnableFilter = TRUE;

	m_bClassApp = !m_bClassApp;
	if(!m_bClassSys && !m_bClassApp)
		m_bClassSys = TRUE;

	SetAlarmFilter();
}

void CSysAlarmPage::OnUpdateAlarmTrain(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(!m_bClassApp.bCannotChanged);
	pCmdUI->SetCheck(m_bClassApp);
}

void CSysAlarmPage::OnAlarmUpdateScroll()
{
	m_bUpdateScroll=!m_bUpdateScroll;
}

void CSysAlarmPage::OnUpdateAlarmUpdateScroll(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_bUpdateScroll);
}



void CSysAlarmPage::OnAlarmShowlog()
{
	ShowLogFiles("ctcinfo");
}


void CSysAlarmPage::OnAlarmEmpty()
{
	m_list.DeleteAllItems();
	m_listNeedAnswer.DeleteAllItems();
}

void CSysAlarmPage::OnAlarmLvl1()
{
	m_bEnableFilter = TRUE;
	m_bLevelOne = !m_bLevelOne;

	SetAlarmFilter();
}

void CSysAlarmPage::OnUpdateAlarmLvl1(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(!m_bLevelOne.bCannotChanged);
	pCmdUI->SetCheck(m_bLevelOne);
}

void CSysAlarmPage::OnAlarmLvl2()
{
	m_bEnableFilter = TRUE;
	m_bLevelTwo = !m_bLevelTwo;

	SetAlarmFilter();
}

void CSysAlarmPage::OnUpdateAlarmLvl2(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(!m_bLevelTwo.bCannotChanged);
	pCmdUI->SetCheck(m_bLevelTwo);
}

void CSysAlarmPage::OnAlarmLvl3()
{
	m_bEnableFilter = TRUE;
	m_bLevelThree = !m_bLevelThree;

	SetAlarmFilter();
}

void CSysAlarmPage::OnUpdateAlarmLvl3(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(!m_bLevelThree.bCannotChanged);
	pCmdUI->SetCheck(m_bLevelThree);
}



/////////////////////////////////////////////////////////////////////////////
// CSysAlarmFilterDlg dialog


CSysAlarmFilterDlg::CSysAlarmFilterDlg(
	map<int, bool>& stationList, map<int, bool>& alarmList, CWnd* pParent /*=NULL*/)
	: CDialog(CSysAlarmFilterDlg::IDD, pParent),
	  m_StationFilter(stationList), m_AlarmIDFilter(alarmList)
{
	//{{AFX_DATA_INIT(CSysAlarmFilterDlg)
	m_bAllStationSel = FALSE;
	m_bAllAlarmSel = TRUE;
	//}}AFX_DATA_INIT
}


void CSysAlarmFilterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSysAlarmFilterDlg)
	DDX_Control(pDX, IDC_LIST1, m_list);
	DDX_Control(pDX, IDC_LIST2, m_listAlarm);
	DDX_Control(pDX, IDC_LEVEL_ONE, m_Level[0]);
	DDX_Control(pDX, IDC_LEVEL_TWO, m_Level[1]);
	DDX_Control(pDX, IDC_LEVEL_THREE, m_Level[2]);
	DDX_Control(pDX, IDC_CLASS_APP, m_Type[0]);
	DDX_Control(pDX, IDC_CLASS_SYS, m_Type[1]);

	int nCheck;
	if (pDX->m_bSaveAndValidate)
	{
		DDX_Check(pDX, IDC_LEVEL_ONE, nCheck);
		m_bLevel[0].bVisible = nCheck;
		DDX_Check(pDX, IDC_LEVEL_TWO, nCheck);
		m_bLevel[1].bVisible = nCheck;
		DDX_Check(pDX, IDC_LEVEL_THREE, nCheck);
		m_bLevel[2].bVisible = nCheck;

		DDX_Check(pDX, IDC_CLASS_APP, nCheck);
		m_bType[0].bVisible = nCheck;
		DDX_Check(pDX, IDC_CLASS_SYS, nCheck);
		m_bType[1].bVisible = nCheck;
	}
	else
	{
		nCheck = m_bLevel[0].bVisible;
		DDX_Check(pDX, IDC_LEVEL_ONE, nCheck);
		nCheck = m_bLevel[1].bVisible;
		DDX_Check(pDX, IDC_LEVEL_TWO, nCheck);
		nCheck = m_bLevel[2].bVisible;
		DDX_Check(pDX, IDC_LEVEL_THREE, nCheck);

		nCheck = m_bType[0].bVisible;
		DDX_Check(pDX, IDC_CLASS_APP, nCheck);
		nCheck = m_bType[1].bVisible;
		DDX_Check(pDX, IDC_CLASS_SYS, nCheck);
	}
	DDX_Check(pDX, IDC_SYSALARM_ALLSTATION, m_bAllStationSel);
	DDX_Check(pDX, IDC_SYSALARM_ALLALARM, m_bAllAlarmSel);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSysAlarmFilterDlg, CDialog)
	//{{AFX_MSG_MAP(CSysAlarmFilterDlg)
	ON_NOTIFY(NM_CLICK, IDC_LIST1, OnClickStationList)
	ON_NOTIFY(NM_CLICK, IDC_LIST2, OnClickAlarmList)
	ON_BN_CLICKED(IDC_CLASS_APP, OnClassApp)
	ON_BN_CLICKED(IDC_CLASS_SYS, OnClassSys)
	ON_BN_CLICKED(IDC_LEVEL_ONE, OnLevelOne)
	ON_BN_CLICKED(IDC_LEVEL_TWO, OnLevelTwo)
	ON_BN_CLICKED(IDC_LEVEL_THREE, OnLevelThree)
	ON_BN_CLICKED(IDC_SYSALARM_ALLSTATION, OnSysalarmAllstation)
	ON_BN_CLICKED(IDC_SYSALARM_ALLALARM, OnSysalarmAllAlarm)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSysAlarmFilterDlg message handlers


BOOL CSysAlarmFilterDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	if(m_bLevel[0].bCannotChanged)
		m_Level[0].EnableWindow(FALSE);
	if(m_bLevel[1].bCannotChanged)
		m_Level[1].EnableWindow(FALSE);
	if(m_bLevel[2].bCannotChanged)
		m_Level[2].EnableWindow(FALSE);
	
	if(m_bType[0].bCannotChanged)
		m_Type[0].EnableWindow(FALSE);
	if(m_bType[1].bCannotChanged)
		m_Type[1].EnableWindow(FALSE);

	// TODO: Add extra initialization here
	//m_SmallImageList.Create("IDB_ALARMSMALLICON", 16, 4, RGB(255, 255, 255));
	//m_SmallImageList.SetBkColor(RGB(255,255,255));
	m_StateImageList.Create("IDB_ALARMSTATEICON", 16, 2, RGB(255, 255, 255));
	//m_StateImageList.SetBkColor(RGB(255,255,255));

	//m_SmallImageList.SetOverlayImage(3, 1);

	//m_list.SetImageList(&m_SmallImageList, LVSIL_SMALL);
	m_list.SetImageList(&m_StateImageList, LVSIL_STATE);
	
	char *label[] = { "车站" };
	int  width[] = {300};
	int  fmt[] = {LVCFMT_LEFT};
	m_list.SetHeaderLabel(sizeof(label)/sizeof(label[0]), label, width, fmt);

	m_bAllStationSel = TRUE;
	map<int, bool>::iterator iter;
	for(iter = m_StationFilter.begin(); iter != m_StationFilter.end(); iter++)
	{
		m_list.AddItem(iter->second, iter->first, gpcdef.GetStationName(iter->first));
		if(!iter->second)
			m_bAllStationSel = FALSE;
	}
	m_list.AddItem(iter->second, -1, "其它车站");
	if(!m_bOtherStationSel)
		m_bAllStationSel = FALSE;
	
	{
		m_listAlarm.SetImageList(&m_StateImageList, LVSIL_STATE);

		char *label[] = { "报警" };
		int  width[] = {300};
		int  fmt[] = {LVCFMT_LEFT};
		m_listAlarm.SetHeaderLabel(sizeof(label)/sizeof(label[0]), label, width, fmt);

		m_bAllAlarmSel = TRUE;
		for(map<int, bool>::iterator iter = m_AlarmIDFilter.begin(); 
			iter != m_AlarmIDFilter.end(); iter++)
		{
			CAlarmSoundConfig::IDINFO* pid = alarmSoundConfig.idInfo.getContentFromID(iter->first);
			if(NULL == pid)
				continue;
			m_listAlarm.AddItem(iter->second, iter->first, pid->szAlarmTitle.c_str());
			if(!iter->second)
				m_bAllAlarmSel = FALSE;
		}
		if(!m_bOtherAlarmSel.bCannotChanged)
		{
			m_listAlarm.AddItem(m_bOtherAlarmSel, -1, "其它报警");
			if(!m_bOtherAlarmSel)
				m_bAllAlarmSel = FALSE;
		}
	}

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSysAlarmFilterDlg::OnClickStationList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	LPNMLISTVIEW pNMLISTVIEW = (NMLISTVIEW*)pNMHDR;
	int nItem = pNMLISTVIEW->iItem;
	if(nItem < 0)
		return ;

	UpdateData(TRUE);
	m_list.ToggleItemSelected(nItem);
	m_bAllStationSel = m_list.GetItemSelectedCount() == m_list.GetItemCount();
	UpdateData(FALSE);

	*pResult = 0;
}

void CSysAlarmFilterDlg::OnClickAlarmList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	LPNMLISTVIEW pNMLISTVIEW = (NMLISTVIEW*)pNMHDR;
	int nItem = pNMLISTVIEW->iItem;
	if(nItem < 0)
		return ;

	UpdateData(TRUE);
	m_listAlarm.ToggleItemSelected(nItem);
	m_bAllAlarmSel = m_listAlarm.GetItemSelectedCount() == m_listAlarm.GetItemCount();
	UpdateData(FALSE);

	*pResult = 0;
}

void CSysAlarmFilterDlg::OnClassApp() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	if(!m_bType[1] && !m_bType[0])
		m_bType[1] = TRUE;
	UpdateData(FALSE);
}

void CSysAlarmFilterDlg::OnClassSys() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	if(!m_bType[1] && !m_bType[0])
		m_bType[0] = TRUE;
	UpdateData(FALSE);
}

void CSysAlarmFilterDlg::OnLevelOne() 
{
	// TODO: Add your control notification handler code here
}


void CSysAlarmFilterDlg::OnLevelTwo() 
{
	// TODO: Add your control notification handler code here
}


void CSysAlarmFilterDlg::OnLevelThree() 
{
	// TODO: Add your control notification handler code here
}

void CSysAlarmFilterDlg::OnSysalarmAllstation() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	m_list.SetAllItemSelected(m_bAllStationSel);
}

void CSysAlarmFilterDlg::OnSysalarmAllAlarm() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	m_listAlarm.SetAllItemSelected(m_bAllAlarmSel);
}

void CSysAlarmFilterDlg::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类
	UpdateData();

	m_StationFilter.clear();
	for(int i=0; i<m_list.GetItemCount(); i++)
	{
		ITEMDATA dw = m_list.GetItemData(i);
		if(dw.nID == 0x7FFFFFFF)
			m_bOtherStationSel = dw.bSel;
		else
			m_StationFilter[dw.nID] = dw.bSel;
	}

	m_AlarmIDFilter.clear();
	for(int i=0; i<m_listAlarm.GetItemCount(); i++)
	{
		ITEMDATA dw = m_listAlarm.GetItemData(i);
		if(dw.nID == 0x7FFFFFFF)
			m_bOtherAlarmSel = dw.bSel;
		else
			m_AlarmIDFilter[dw.nID] = dw.bSel;
	}

	CDialog::OnOK();
}




void CFilterList::AddItem(bool bSelected, int nID, LPCSTR szTitle)
{
	ITEMDATA dw;
	dw.nID = nID;
	dw.bSel = bSelected;

		CString txt;
		LVITEM lvi;
		lvi.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
		//item.state = 0;
		//item.stateMask = LVIS_SELECTED;
		//item.iImage = 0;
		lvi.lParam = dw;
		lvi.iItem = GetItemCount();

		lvi.mask = LVIF_TEXT | LVIF_STATE;
		lvi.stateMask = LVIS_STATEIMAGEMASK;
		lvi.iSubItem = 0;
		txt.Format("%s(%d)", szTitle, nID);
		lvi.pszText = (LPSTR)(LPCSTR)txt;

		if(dw.bSel)
		{
			lvi.stateMask |= LVIS_OVERLAYMASK;
			lvi.state = INDEXTOSTATEIMAGEMASK(2)|INDEXTOOVERLAYMASK(1);
		}
		else
		{
			lvi.state = INDEXTOSTATEIMAGEMASK(1);
		}
		lvi.iItem = InsertItem(&lvi);

		SetItemData(lvi.iItem, dw);
}

bool CFilterList::SetItemSelected(int nItem, bool bSelected)
{
	ASSERT(nItem <= GetItemCount() && nItem >= 0);

	ITEMDATA dw = GetItemData(nItem);
	
	bool bRet = dw.bSel;
	
	dw.bSel = bSelected;
	SetItemData(nItem, dw);
	
	if(dw.bSel)
	{
		SetItemState(nItem, 
			INDEXTOSTATEIMAGEMASK(2)|INDEXTOOVERLAYMASK(1),
			LVIS_STATEIMAGEMASK|LVIS_OVERLAYMASK);
	}
	else
	{
		SetItemState(nItem, 
			INDEXTOSTATEIMAGEMASK(1),
			LVIS_STATEIMAGEMASK );	
	}

	return bRet;
}

void CFilterList::SetAllItemSelected(bool bSelected)
{
	for(int i=0; i<GetItemCount(); i++)
	{
		SetItemSelected(i, bSelected);
	}
}

void CFilterList::ToggleItemSelected(int nItem)
{
	ITEMDATA dw = GetItemData(nItem);
	
	if(dw.bSel = !dw.bSel)
	{
		SetItemState(nItem, 
			INDEXTOSTATEIMAGEMASK(2)|INDEXTOOVERLAYMASK(1),
			LVIS_STATEIMAGEMASK|LVIS_OVERLAYMASK);
	}
	else
	{
		SetItemState(nItem, 
			INDEXTOSTATEIMAGEMASK(1),
			LVIS_STATEIMAGEMASK );	
	}
	
	SetItemData(nItem, dw);
}

DWORD CFilterList::GetItemSelectedCount()
{
	DWORD count = 0;

	for(int i=0; i<GetItemCount(); i++)
	{
		ITEMDATA dw = GetItemData(i);
		if(dw.bSel)
			count ++;
	}

	return count;
}
