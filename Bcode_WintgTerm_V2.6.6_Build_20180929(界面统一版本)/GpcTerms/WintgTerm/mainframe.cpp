// MainFrame.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "memshare.h"
#include "bureau.h"
#include "oscalls.h"
#include "MainFrame.h"
#include "tgtype.h"
#include "distance.h"
#include "tgtime.h"
#include "tg.h"
#include "tgmsg.h"
#include "widgets.h"
#include "function.h"
//#include "sxbutton.h"
#include "alarm.h"
#include "SuperPassDlg.h"
#include "DlgMessageBox.h"
#include <baseMsg\msgid.h>
#include <baseMsg\clbth.h>
#include "DlgSelectSection.h"
#include "historyschddlg.h"
#include "QuitSystem.h"
#include "dlgabout.h"
#include "PropertyPagePassway.h"            
#include "QueryPhaseStoreTrain.h"//车站暂存车
#include "tg_schedule_base.h"
#include "AbsTimeDlg.h"
#include "db_msgproc.h"
#include "tgdocument.h"
#include "DlgWordMaintain.h"
#include "WorkTgFrame.h"
#include "alarm/SoundPlay.h"
#include "SuperPassDlg.h"
#include "UserListDlg.h"
#include "DlgBasicBackupSelect.h"
#include "DlgTCCSelect.h"
#include "basic_schd_config.h"
#include ".\mainframe.h"
#include "RightListDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern basic_schd_config* GetBasicConfig();
extern int nAllButtonStatus ;

extern void ShowErrorMsg(void);
extern int WriteMessage(PROCESSID usProcessId,ushort usQueueHandle,
					ushort count,char *buffer);
extern BOOL UR_CanOperateSchedule();

extern UINT REGIST_MSG_PROGRAMONCE;

class CStatisticsSectionInfo;
/////统计使用的配置
CStatisticsSectionInfo* g_StatisticsSecInfo = NULL;
/////统计使用的配置

// CMainFrame

#define INVALID_MSGID_CONJOIN		2
IMPLEMENT_DYNCREATE(CMainFrame, CMDIFrameWnd)

CMainFrame::CMainFrame()
{
	m_bAutoMenuEnable = FALSE;
	m_pTcc3InfoProvider=NULL;
	m_pTccInfoProvider=NULL;
	m_pTcc3AdviseSink=NULL;
	m_pTccAdviseSink=NULL;
	m_pStplanAlarm=NULL;

	nSectionNum=0;
	memset(nSectionNo, 0, sizeof(nSectionNo));
}

CMainFrame::~CMainFrame()
{
}


BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_COMMAND(ID_BUTTON_EXIT, OnButtonExit)
	ON_WM_DESTROY()
	ON_WM_CLOSE()
	ON_WM_TIMER()
	ON_WM_ACTIVATE()
	ON_WM_NCDESTROY()
	ON_WM_SIZE()
	ON_WM_MDIACTIVATE()
	ON_COMMAND(ID_HIST_WIN, OnHistWin)
	ON_UPDATE_COMMAND_UI(ID_HIST_WIN,OnUpdateHistWin)
	
	ON_COMMAND(ID_BASIC_WIN_NORMAL, OnBasicWin)
	ON_UPDATE_COMMAND_UI(ID_BASIC_WIN_NORMAL,OnUpdateBasicWin)

	ON_COMMAND(ID_BASIC_WIN_ORTHER, OnBasicPrevWin)
	ON_UPDATE_COMMAND_UI(ID_BASIC_WIN_ORTHER,OnUpdateBasicPrevWin)

	ON_COMMAND(ID_SELECT_BASIC_BACKUP, OnBasicBackupWin)
	ON_UPDATE_COMMAND_UI(ID_SELECT_BASIC_BACKUP,OnUpdateBasicBackupWin)

	ON_COMMAND(ID_PASSWAY_MANAGEMENT, OnPasswayManagement)	 

	ON_COMMAND(ID_NEIGHBER_WIN,OnNeighberWin)
	ON_UPDATE_COMMAND_UI(ID_NEIGHBER_WIN,OnUpdateNeighberWin)
	
	ON_COMMAND(ID_WORK_SCHD_WIN,OnWorkWin)

	ON_COMMAND(ID_MPLAN_OPEN,OnMplanOpen)
	ON_UPDATE_COMMAND_UI(ID_MPLAN_OPEN,OnUpdateMplanOpen)

	ON_COMMAND(ID_TYPLAN_OPEN,OnTYplanOpen)

	ON_COMMAND(ID_SHIFT_SCHD_WIN,OnShiftWin)
	ON_UPDATE_COMMAND_UI(ID_SHIFT_SCHD_WIN,OnUpdateShiftWin)
	//}}AFX_MSG_MAP
	
	ON_COMMAND(IDC_ABOUT, OnAbout)
	ON_COMMAND(ID_TCC_LIMITSPEED, OnTccLimitspeed)
	ON_UPDATE_COMMAND_UI(ID_TCC_LIMITSPEED,OnUpdateTccLimitspeed)
	ON_COMMAND(ID_SYS_COMMWND,OnSystemComWnd)
	ON_COMMAND(ID_ABS_TIME, OnAbsTime)
	ON_MESSAGE(UM_STATUS_BAR_UPDATE,OnStatusBarUpdate)

	ON_COMMAND(ID_SYS_SAVE_TO_FILES,OnSaveToFiles)
	ON_COMMAND(ID_SYS_LOAD_FROM_FILES,OnLoadFromFiles)
	ON_COMMAND(ID_WORD_MIANTAIN,OnMaintainWord)
	ON_COMMAND(ID_WIN_MEMORY,OnMemoryWnd)
	ON_MESSAGE(UM_MEMORY_NOIFY,OnMemoryNotify)
	
	ON_COMMAND(ID_ENABLE_PLAY_SOUND,OnEnablePlaySound)
	ON_UPDATE_COMMAND_UI(ID_ENABLE_PLAY_SOUND,OnUpdateEnablePlaySound)
	ON_COMMAND(ID_USER_MANAGE,OnUserManage)
	ON_UPDATE_COMMAND_UI(ID_USER_MANAGE,OnUpdateUserManage)

	ON_COMMAND_RANGE(ID_SELECT_SECTION, ID_SELECT_SECTION + 64,  OnSelectDDT)
	ON_UPDATE_COMMAND_UI_RANGE(ID_SELECT_SECTION, ID_SELECT_SECTION + 64, OnUpdateSelectDDT)

	ON_REGISTERED_MESSAGE(REGIST_MSG_PROGRAMONCE, OnProgramOnce)
	ON_COMMAND(ID_RIGHT_MANAGE, OnRightManage)
	ON_UPDATE_COMMAND_UI(ID_RIGHT_MANAGE, OnUpdateRightManage)
	ON_WM_INITMENU()
END_MESSAGE_MAP()

static UINT indicators[] =
{
	IDS_OPERATOR_IS_WHO,
	IDS_SCROLL_TEXT,
	IDS_SYNC_STATUS,
	ID_ADJUST_STATUS,
	IDS_TRAINSSELECTED     
};

// CMainFrame message handlers
BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs) 
{
	// TODO: Add your specialized code here and/or call the base class
	cs.lpszClass = AfxRegisterWndClass(CS_DBLCLKS,
		AfxGetApp()->LoadStandardCursor(IDC_ARROW), (HBRUSH)(COLOR_WINDOW+1),
		AfxGetApp()->LoadIcon(IDR_TGMAIN));

	cs.style &= ~FWS_ADDTOTITLE;
	cs.style &= ~WS_MAXIMIZE;

	int screenNum = gpcdef.GetScreenNum();
	int screenx = GetSystemMetrics(SM_CXSCREEN);
	int screeny = GetSystemMetrics(SM_CYSCREEN);
	if(screenNum == 1)
	{
		cs.cx = screenx;
		cs.x = 0;
	}
	else if(screenNum == 2 && gpcdef.GetUserIdentify() == GPC_DISPATCHER)
	{
		cs.cx = screenx;
		cs.x = 0;
	}
#ifdef BUREAU_JINAN
	else if(screenNum == 5)
	{
		cs.cx = screenx;
		cs.x = 0;
	}
	else if(screenNum == 6)
	{
		cs.cx = screenx/3;
		cs.x = 0;

	}
#endif
	else
	{
		cs.cx = screenx/screenNum;
		cs.x = 0;
	}

	cs.y = 0;
	cs.cy = GetSystemMetrics(SM_CYMAXIMIZED);
	return CMDIFrameWnd::PreCreateWindow(cs);
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	SetProp(GetSafeHwnd( ),"programname",(HANDLE)3);

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	//设置车次黑体
	UINT nID, nStyle;
	int cxWidth;
	m_wndStatusBar.GetPaneInfo(0, nID, nStyle, cxWidth);
	m_wndStatusBar.SetPaneInfo(0, nID, nStyle, cxWidth*0.8);

	m_wndStatusBar.GetPaneInfo(1, nID, nStyle, cxWidth);
	m_wndStatusBar.SetPaneInfo(1, nID, nStyle | SBT_OWNERDRAW, cxWidth*1.2);

	m_wndStatusBar.GetPaneInfo(2, nID, nStyle, cxWidth);
	m_wndStatusBar.SetPaneInfo(2, nID, nStyle | SBT_OWNERDRAW, 250);

	m_wndStatusBar.GetPaneInfo(3, nID, nStyle, cxWidth);
	m_wndStatusBar.SetPaneInfo(3, nID, nStyle | SBT_OWNERDRAW, 280);

	m_wndStatusBar.GetPaneInfo(4, nID, nStyle, cxWidth);
	m_wndStatusBar.SetPanelBoldText(4,TRUE);
	m_wndStatusBar.SetPaneInfo(4, nID, nStyle | SBT_OWNERDRAW, 550);

	m_wndStatusBar.SetOpeator();

	m_DlgMemory.CreateMe(this);			

	DWORD tabstyle=TCS_MULTILINE|TCS_VERTICAL|TCS_HOTTRACK|TCS_SCROLLOPPOSITE|TCS_RIGHTJUSTIFY;
	VERIFY(m_MTIClientWnd.Attach(this,tabstyle));
	
	AddXdtMenus();
	return 0;
}

void CMainFrame::OnButtonExit() 
{
	// TODO: Add your command handler code here
	SendMessage(WM_CLOSE);
	return;   
}

BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext) 
{
	return CMDIFrameWnd::OnCreateClient(lpcs, pContext);
}

void CMainFrame::OnDestroy() 
{
	theWintgApp.bWinTGExit=true;
	
	RemoveProp(GetSafeHwnd( ),"programname");	
	
	DB_DeinitData();
	EndComTask(); //放在此处调用，可以防止窗口提前退出后，若干消息处理异常
	if(m_pTcc3InfoProvider)
		delete m_pTcc3InfoProvider;
	if(m_pTccInfoProvider)
		delete m_pTccInfoProvider;
	if(m_pTcc3AdviseSink)
		delete m_pTcc3AdviseSink;
	if(m_pTccAdviseSink)
		delete m_pTccAdviseSink;
	
	CMDIFrameWnd::OnDestroy();
}

void CMainFrame::OnClose() 
{
	CQuitSystem *dlg = new CQuitSystem;	
	int nCurrentDate = OsGetCurrDate(PMTG.ThreadId);
	int nCurrentTime = OsGetCurrTime(PMTG.ThreadId);
	MessageBeep(MB_ICONASTERISK);
	sysprint(0,PMTG.ThreadId,"[PMTG] 用户选择 退出 at date %s, time %s",
		OsConvertDatetoString(nCurrentDate),
		OsConvertTimetoString(nCurrentTime));
	if(dlg == NULL)
	{
		sysprint(0,PMTG.ThreadId,"[PMTG] memory alloc failed in file %s at line %d",
			__FILE__,__LINE__);
		return;
	}

	if(dlg->DoModal( ) == IDOK)
	{
		KillTimer(1901);
		int nCurrentDate = OsGetCurrDate(PMTG.ThreadId);
		int nCurrentTime = OsGetCurrTime(PMTG.ThreadId);

		sysprint(0,PMTG.ThreadId,"[PMTG] user press WM_CLOSE at date %s, time %s",
			OsConvertDatetoString(nCurrentDate),
			OsConvertTimetoString(nCurrentTime));

		GpcLogSysMessage(0,"WintgTerm 用户选择确认退出程序.");

		if(m_pStplanAlarm)
		{
			delete m_pStplanAlarm;
			m_pStplanAlarm=NULL;
		}
		CMDIFrameWnd::OnClose();
	}
	delete dlg;
	dlg = NULL;
	
}

void CMainFrame::OnDropDown(NMHDR* pNotifyStruct, LRESULT* pResult)
{
	*pResult = TBDDRET_DEFAULT;
}

BOOL CMainFrame::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	NMHDR* pDr = (NMHDR *)lParam;

	if(wParam == AFX_IDW_TOOLBAR)
		if(pDr->code == TBN_DROPDOWN)
			OnDropDown((NMHDR *)lParam, pResult);

	return CMDIFrameWnd::OnNotify(wParam, lParam, pResult);
}

bool CMainFrame::CreateStplanAlarmWnd()
{
	if(m_pStplanAlarm)
		return true;
	m_pStplanAlarm=new CDlgStplanAlarm();
	m_pStplanAlarm->Create(CDlgStplanAlarm::IDD, this);
	m_pStplanAlarm->ShowWindow(SW_SHOW);
	m_pStplanAlarm->UpdateWindow();
	m_pStplanAlarm->ShowWindow(SW_HIDE);
	if(!m_pStplanAlarm->LoadStplanAlarmXML(m_gpcContext.mSectionId))
	{
		AfxMessageBox("读取计划下达提醒时间配置失败");
		return false;
	}
	SetTimer(1901, 20*1000, NULL);
	return true;
}

void CMainFrame::OnTimer(UINT nIDEvent) 
{
	if(1901==nIDEvent && m_pStplanAlarm)
	{
		if(::IsWindow(m_pStplanAlarm->m_hWnd))
		{
			if(m_pStplanAlarm->IsAlarm())			
			{
				if(!::IsWindowVisible(m_pStplanAlarm->m_hWnd))
					m_pStplanAlarm->ShowWindow(SW_SHOW);
			}

			if(::IsWindowVisible(m_pStplanAlarm->m_hWnd))
			{
				m_pStplanAlarm->Alarm();
			}
		}
	}
	CMDIFrameWnd::OnTimer(nIDEvent);
}

void CMainFrame::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized) 
{
	CMDIFrameWnd::OnActivate(nState, pWndOther, bMinimized);
	CMDIChildWnd *pMDIActive = MDIGetActive();
	if(pMDIActive == NULL) return;

	CString title = gpcdef.GetBureauName();
	SetWindowText(title);
	AddXdtMenus();
}

BOOL CMainFrame::DestroyWindow() 
{
	CSizingControlBar::GlobalSaveState(_T("BarState"));
	SaveBarState(_T("BarState"));

	return CMDIFrameWnd::DestroyWindow(); 
}

LRESULT CMainFrame::OnStatusBarUpdate(WPARAM wParam, LPARAM lParam)
{	
	m_wndStatusBar._SetPaneText((UINT)wParam, (const char *)lParam);
	return TRUE;
}	

void CMainFrame::OnNcDestroy() 
{
	CMDIFrameWnd::OnNcDestroy();

	// TODO: Add your message handler code here
}

void CMainFrame::OnSize(UINT nType, int cx, int cy) 
{
	CMDIFrameWnd::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
}

void CMainFrame::OnMDIActivate(BOOL bActivate, CWnd* pActivateWnd, CWnd* pDeactivateWnd) 
{
	CMDIFrameWnd::OnMDIActivate(bActivate, pActivateWnd, pDeactivateWnd);

	// TODO: Add your message handler code here	
}

void CMainFrame::SetStatusText(UINT index, LPCTSTR text)
{
	m_wndStatusBar._SetPaneText(index, text);
	return;
}

void CMainFrame::OnUpdateSelectDDT(CCmdUI* pCmdUI)
{
	int idx=pCmdUI->m_nID-ID_SELECT_SECTION;
	for(int i=0; i<nSectionNum; i++)
	{
		if(nSectionNo[i] == m_gpcContext.mSectionId)
			break;		
	}
	pCmdUI->SetCheck(idx==i);
	//pCmdUI->Enable(idx!=i);
}

int FindMenuItem(CMenu* Menu, LPCTSTR MenuString)
{
   ASSERT(Menu);
   ASSERT(::IsMenu(Menu->GetSafeHmenu()));

   int count = Menu->GetMenuItemCount();
   for (int i = 0; i < count; i++)
   {
      CString str;
      if (Menu->GetMenuString(i, str, MF_BYPOSITION) &&
         (strcmp(str, MenuString) == 0))
         return i;
   }

   return -1;
}

// 动态生成调度台菜单表
void CMainFrame::AddXdtMenus()
{
	nSectionNum = sizeof(nSectionNo)/sizeof(nSectionNo[0]);
	gpcdef.GetSectionList(nSectionNum, nSectionNo);

	CMenu* mmenu = GetMenu();

	// 区段选择
    int pos = FindMenuItem(mmenu, "区段选择(&S)");
    if(pos >= 0)
	{
		CMenu* submenu = mmenu->GetSubMenu(pos);
		if(submenu)
		{
			if(FindMenuItem(submenu, "占位区段")>=0)
			{
				// 占位区段
				submenu->DeleteMenu(0,MF_BYPOSITION);				
				for(int i=0; i <nSectionNum; i++)
				{
					CString ddtName = gpcdef.GetSectionName(nSectionNo[i]);

					submenu->AppendMenu(MF_STRING | MF_ENABLED| MF_UNCHECKED,	ID_SELECT_SECTION+i, ddtName);
				}
			}
		}
	}
	
	// 功能
	int npos = FindMenuItem(mmenu, "功能(&F)");
	if(npos >= 0)
	{
		if(!gpcdef.IsFuncMenuViewTgSchd(SCHD_TYPE::BASIC))
		{
			CMenu* submenu = mmenu->GetSubMenu(npos);
			if(submenu)
			{
				int pos = FindMenuItem(submenu, "基本图");
				if (pos >= 0)
					submenu->DeleteMenu(pos,MF_BYPOSITION);	
			}
		}

		if(!gpcdef.IsFuncMenuViewTgSchd(SCHD_TYPE::BACK_UP_BASIC) || !gpcdef.IsNewBasicDump())
		{
			CMenu* submenu = mmenu->GetSubMenu(npos);
			if(submenu)
			{
				int pos = FindMenuItem(submenu, "基本图");
				if (pos >= 0)
				{
					CMenu* subsubmenu = submenu->GetSubMenu(pos);
					if(subsubmenu && !GetTGFunctionConfig()->IsFormatView())
					{
						int nnpos = FindMenuItem(subsubmenu, "归档基本图");
						if(nnpos<0)
							return;
						subsubmenu->DeleteMenu(nnpos,MF_BYPOSITION);
					}
				}
			}
		}

		if(!gpcdef.IsFuncMenuViewTgSchd(SCHD_TYPE::SHIFT))
		{
			CMenu* submenu = mmenu->GetSubMenu(npos);
			if(submenu)
			{
				int pos = FindMenuItem(submenu, "日班计划图");
				if (pos >= 0)
					submenu->DeleteMenu(pos,MF_BYPOSITION);	
			}
		}

		if(!gpcdef.IsFuncMenuViewTgSchd(SCHD_TYPE::MMPLAN))
		{
			CMenu* submenu = mmenu->GetSubMenu(npos);
			if(submenu)
			{
				int pos = FindMenuItem(submenu, "施工计划");
				if (pos >= 0)
					submenu->DeleteMenu(pos,MF_BYPOSITION);	
			}
		}
		if(!gpcdef.IsFuncMenuViewTgSchd(SCHD_TYPE::NEIGHBOUR))
		{
			CMenu* submenu = mmenu->GetSubMenu(npos);
			if(submenu)
			{
				int pos = FindMenuItem(submenu, "邻台运行图");
				if (pos >= 0)
					submenu->DeleteMenu(pos,MF_BYPOSITION);	
			}
		}
		if(!gpcdef.IsFuncMenuViewTgSchd(SCHD_TYPE::HISTORY))
		{
			CMenu* submenu = mmenu->GetSubMenu(npos);
			if(submenu)
			{
				int pos = FindMenuItem(submenu, "历史运行图");
				if (pos >= 0)
					submenu->DeleteMenu(pos,MF_BYPOSITION);	
			}
		}
		if(!gpcdef.IsFuncMenuViewTgSchd(SCHD_TYPE::TCC_MNG))
		{
			CMenu* submenu = mmenu->GetSubMenu(npos);
			if(submenu)
			{
				int pos = FindMenuItem(submenu, "列控命令管理");
				if (pos >= 0)
					submenu->DeleteMenu(pos,MF_BYPOSITION);	
			}
		}
		if(!gpcdef.IsFuncMenuViewTgSchd(SCHD_TYPE::ELE_ARMS))
		{
			CMenu* submenu = mmenu->GetSubMenu(npos);
			if(submenu && !GetTGFunctionConfig()->IsFormatView())
			{
				int pos = FindMenuItem(submenu, "电力臂管理");
				if (pos >= 0)
					submenu->DeleteMenu(pos,MF_BYPOSITION);	
			}
		}
		if(!gpcdef.IsEnablePredict())
		{
			CMenu* submenu = mmenu->GetSubMenu(npos);
			if(submenu)
			{
				int pos = FindMenuItem(submenu, "车站预告");
				if (pos >= 0)
					submenu->DeleteMenu(pos,MF_BYPOSITION);	
			}
		}
	}

	npos = FindMenuItem(mmenu, "编辑(&E)");
	if(npos >= 0)
	{
		if(!gpcdef.IsShowAutoAdjust())
		{
			CMenu* submenu = mmenu->GetSubMenu(npos);
			if(submenu)
			{
				int pos = FindMenuItem(submenu, "自动调整");
				if (pos >= 0)
					submenu->DeleteMenu(pos,MF_BYPOSITION);	
			}
		}

		if (!GetTGFunctionConfig()->IsEnableLockWholeTrainSide())
		{
			CMenu* submenu = mmenu->GetSubMenu(npos);
			if(submenu)
			{
				int pos = FindMenuItem(submenu, "设置多趟车股道锁定");
				if (pos >= 0)
					submenu->DeleteMenu(pos,MF_BYPOSITION);	
			}
		}
	}

	// 选项
	npos = FindMenuItem(mmenu, "选项(&A)");
	if(npos >= 0)
	{
		if(!GetTGFunctionConfig()->IsEnableCzh() && !GetTGFunctionConfig()->IsEnableNewCzh())
		{
			CMenu* submenu = mmenu->GetSubMenu(npos);
			if(submenu)
			{
				int pos = FindMenuItem(submenu, "显示选中车组勾连线");
				if (pos >= 0)
					submenu->DeleteMenu(pos,MF_BYPOSITION);	
			}
			if(submenu)
			{
				int pos = FindMenuItem(submenu, "显示所有车组勾连线");
				if (pos >= 0)
					submenu->DeleteMenu(pos,MF_BYPOSITION);	
			}
			submenu = mmenu->GetSubMenu(npos);
			if(submenu)
			{
				int pos = FindMenuItem(submenu, "显示折返车次");
				if (pos >= 0)
					submenu->DeleteMenu(pos,MF_BYPOSITION);	
			}
			submenu = mmenu->GetSubMenu(npos);
			if(submenu)
			{
				int pos = FindMenuItem(submenu, "平移后续同交路列车");
				if (pos >= 0)
					submenu->DeleteMenu(pos,MF_BYPOSITION);	
			}
		}

		if(!gpcdef.IsShowRouteOkMark())
		{
			CMenu* submenu = mmenu->GetSubMenu(npos);
			if(submenu)
			{
				int pos = FindMenuItem(submenu, "显示进路已排出表示");
				if (pos >= 0)
					submenu->DeleteMenu(pos,MF_BYPOSITION);	
			}
		}

		if(!gpcdef.IsShowRouteTrig())
		{
			CMenu* submenu = mmenu->GetSubMenu(npos);
			if(submenu)
			{
				int pos = FindMenuItem(submenu, "显示非自触双圆圈");
				if (pos >= 0)
					submenu->DeleteMenu(pos,MF_BYPOSITION);	
			}
		}

		if(!gpcdef.IsShowVirtualStation())
		{
			CMenu* submenu = mmenu->GetSubMenu(npos);
			if(submenu)
			{
				int pos = FindMenuItem(submenu, "显示虚拟车站");
				if (pos >= 0)
					submenu->DeleteMenu(pos,MF_BYPOSITION);	
			}
		}

		if(!gpcdef.IsShowOPMSTransInfoButton())
		{
			CMenu* submenu = mmenu->GetSubMenu(npos);
			if(submenu)
			{
				int pos = FindMenuItem(submenu, "显示计划运输信息");
				if (pos >= 0)
					submenu->DeleteMenu(pos,MF_BYPOSITION);	
			}
		}

		if(!gpcdef.IsShowKiloMeterButton())
		{
			CMenu* submenu = mmenu->GetSubMenu(npos);
			if(submenu)
			{
				int pos = FindMenuItem(submenu, "显示里程标(&L)");
				if (pos >= 0)
					submenu->DeleteMenu(pos,MF_BYPOSITION);	
			}
		}

		if(!gpcdef.IsShowAutoAdjust())
		{
			CMenu* submenu = mmenu->GetSubMenu(npos);
			if(submenu)
			{
				int pos = FindMenuItem(submenu, "自动调整");
				if (pos >= 0)
					submenu->DeleteMenu(pos,MF_BYPOSITION);	
			}
		}

		if(!GetTGFunctionConfig()->IsAcceptImportNote())
		{
			CMenu* submenu = mmenu->GetSubMenu(npos);
			if(submenu)
			{
				int pos = FindMenuItem(submenu, "显示重点事项");
				if (pos >= 0)
					submenu->DeleteMenu(pos,MF_BYPOSITION);	
			}
		}
	}

	return;
}

void CMainFrame::OnSelectDDT(UINT nID)
{
	int nSelSectionId = nSectionNo[0];
	int index = nID - ID_SELECT_SECTION;
	if(index < nSectionNum && index >= 0)
		nSelSectionId = nSectionNo[index];
	else
		return;
	
	if(nSelSectionId != m_gpcContext.mSectionId)
	{
		// 广州局A权限,切换调度台不关闭已打开的图
		if(gpcdef.GetUserIdentify()==GPC_SCHEDULE)
		{
			if(GetTGFunctionConfig()->IsQuerySchdClose())
			{
				((CWintg *)AfxGetApp())->CloseSectionIdFrame(m_gpcContext.mSectionId);
			}
		}
		else
		{
			((CWintg *)AfxGetApp())->CloseSectionIdFrame(m_gpcContext.mSectionId);
		}
		m_gpcContext.mSectionId = nSelSectionId;
	}
	if(UR_CanOperateSchedule())
		OnWorkWin();
	else // 如果查询权限打开 ，则...
	{
		((CWintg *)AfxGetApp())->CreateScheduleWindowDync(
						0,
						m_gpcContext.mSectionId,
						CTG_ScheduleInfo::SCHEDULE_TYPE_WORK, 
						m_gpcContext.mShiftID,
						TRUE);	
	}
}

void CMainFrame::OnWorkWin() 
{
	((CWintg *)AfxGetApp())->CreateScheduleWindow(
		m_gpcContext.mSectionId,
		CTG_ScheduleInfo::SCHEDULE_TYPE_WORK, 
		m_gpcContext.mShiftID);	
}

void CMainFrame::OnUpdateShiftWin(CCmdUI* pCmdUI)
{
	if(!gpcdef.IsFuncMenuViewTgSchd(SCHD_TYPE::SHIFT))
	{
		pCmdUI->Enable(FALSE);
	}
}

void CMainFrame::OnShiftWin()
{
	if(m_gpcContext.mSectionId > 0)
	{
		if(UR_CanOperateSchedule())
		{
			((CWintg *)AfxGetApp())->CreateScheduleWindow(
				m_gpcContext.mSectionId,
				CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT, 
				m_gpcContext.mShiftID);
		}
		else
		{
			((CWintg *)AfxGetApp())->CreateScheduleWindowDync(
						0,
						m_gpcContext.mSectionId,
						CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT, 
						m_gpcContext.mShiftID,
						TRUE);	
		}
	}
}

void CMainFrame::OnUpdateBasicWin(CCmdUI* pCmdUI)
{
	if(!gpcdef.IsFuncMenuViewTgSchd(SCHD_TYPE::BASIC))
	{
		pCmdUI->Enable(FALSE);
	}
}

void CMainFrame::OnBasicWin( ) 
{
	if(m_gpcContext.mSectionId > 0)
	{
		if(UR_CanOperateSchedule())
		{
			((CWintg *)AfxGetApp())->CreateScheduleWindow(
				m_gpcContext.mSectionId,
				CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC, 
				m_gpcContext.mShiftID);
		}
		else
		{
			((CWintg *)AfxGetApp())->CreateScheduleWindowDync(
						0,
						m_gpcContext.mSectionId,
						CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC, 
						m_gpcContext.mShiftID,
						TRUE);	
		}
	}
}

void CMainFrame::OnUpdateBasicPrevWin(CCmdUI* pCmdUI)
{
	if(!gpcdef.IsFuncMenuViewTgSchd(SCHD_TYPE::BASIC))
	{
		pCmdUI->Enable(FALSE);
	}
}

void CMainFrame::OnBasicPrevWin( ) 
{
	if(m_gpcContext.mSectionId > 0)
	{
		((CWintg *)AfxGetApp())->CreateScheduleWindow(
			m_gpcContext.mSectionId,
			CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_PREV, 
			m_gpcContext.mShiftID);
	}
}

void CMainFrame::OnUpdateBasicBackupWin(CCmdUI* pCmdUI)
{
	if(gpcdef.IsNewBasicDump())
		pCmdUI->Enable(TRUE);
	else
		pCmdUI->Enable(FALSE);
}

void CMainFrame::OnBasicBackupWin()
{
	if(m_gpcContext.mSectionId > 0)
	{
		CDlgBasicBackupSelect dlg(m_gpcContext.mSectionId, this);

		if(dlg.DoModal()==IDOK)
		{
			int nSelSchdType = dlg.m_nSelSchdType;
			if(nSelSchdType<=0)
				return;

			int nCurrSelSchdType=GetBasicConfig()->GetCurSelBackupBasicType(m_gpcContext.mSectionId);
			if(nSelSchdType != nCurrSelSchdType && nCurrSelSchdType>0)
			{
				((CWintg *)AfxGetApp())->CloseBackupBasicFrame(m_gpcContext.mSectionId);
			}

			GetBasicConfig()->SetCurSelBackupBasicTypeIndex(m_gpcContext.mSectionId, nSelSchdType);
				
			((CWintg *)AfxGetApp())->CreateScheduleWindow(
					m_gpcContext.mSectionId,
					CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_BACKUP, 
					m_gpcContext.mShiftID);
		}
	}
}

void CMainFrame::OnMplanOpen() 
{
	if(!gpcdef.IsFuncMenuViewTgSchd(SCHD_TYPE::MMPLAN))
	{
		return;
	}

	if(m_gpcContext.mSectionId > 0)
	{
		((CWintg *)AfxGetApp())->CreateMMIPlanWindow(
			m_gpcContext.mSectionId,
			m_gpcContext.mShiftID,
			FALSE);
	}
}

void CMainFrame::OnUpdateMplanOpen(CCmdUI* pCmdUI)
{
	if(!gpcdef.IsFuncMenuViewTgSchd(SCHD_TYPE::MMPLAN))
	{
		pCmdUI->Enable(FALSE);
	}
}

// 特运计划管理
void CMainFrame::OnTYplanOpen() 
{
	if(!gpcdef.IsFuncMenuViewTgSchd(SCHD_TYPE::TYPLAN))
	{
		return;
	}

	if(m_gpcContext.mSectionId > 0)
	{
		((CWintg *)AfxGetApp())->CreateTYPlanWindow(
			m_gpcContext.mSectionId,
			m_gpcContext.mShiftID,
			FALSE);
	}
}

// 历史图
void CMainFrame::OnUpdateHistWin(CCmdUI* pCmdUI)
{
	if(!gpcdef.IsFuncMenuViewTgSchd(SCHD_TYPE::HISTORY))
	{
		pCmdUI->Enable(FALSE);
	}
}

void CMainFrame::OnHistWin( ) 
{
	if(m_gpcContext.mSectionId > 0)
	{	
		// 允许调度员向前登录的次数
		CHistorySchdDlg dlg2;
		dlg2.m_bCtrlBanci = false;
		dlg2.m_nPreSchdMaxNum = 99999;
		if(dlg2.DoModal() == IDOK)
		{
			ULONG ShiftID = dlg2.id;
			((CWintg *)AfxGetApp())->CreateScheduleWindowDync(
				0,
				m_gpcContext.mSectionId,
				CTG_ScheduleInfo::SCHEDULE_TYPE_HIST, 
				ShiftID,
				TRUE);
		}
	}
}

// 邻台图
void CMainFrame::OnUpdateNeighberWin(CCmdUI* pCmdUI)
{
	if(!gpcdef.IsFuncMenuViewTgSchd(SCHD_TYPE::NEIGHBOUR))
	{
		pCmdUI->Enable(FALSE);
	}
}

void CMainFrame::OnNeighberWin() 
{
	if(m_gpcContext.mSectionId > 0)
	{
		// 选择邻台
		CDlgSelectSection dlg;
		dlg.m_nNeighborOfSection= m_gpcContext.mSectionId;
		if(dlg.DoModal()==IDOK && dlg.m_nSectionNo>0)
		{
			CHistorySchdDlg dlg2;
			dlg2.m_bCtrlBanci = true;
			dlg2.m_nPreSchdMaxNum = gpcdef.GetPreSchdNum();
			if(dlg2.DoModal() == IDOK)
			{
				((CWintg *)AfxGetApp())->CreateScheduleWindowDync(
					m_gpcContext.mSectionId,
					dlg.m_nSectionNo,
					CTG_ScheduleInfo::SCHEDULE_TYPE_WORK, 
					dlg2.id,
					TRUE);	
			}
		}
	}
}

void CMainFrame::OnUpdateTccLimitspeed(CCmdUI* pCmdUI)
{
	if(!gpcdef.IsFuncMenuViewTgSchd(SCHD_TYPE::TCC_MNG))
	{
		pCmdUI->Enable(FALSE);
		return;
	}

	int SectionNo = m_gpcContext.mSectionId;
	if(SectionNo<=0)
		return;
	pCmdUI->Enable(gpcdef.IsTccDefineInit(SectionNo));
}

void CMainFrame::LoadRelateTccLine(int type)
{
	int nRelateTccType=gpcdef.GetNeibTCCType(type);
	CString strRelateTccDir=gpcdef.GetNeibTccCfgDir(type);
	if(!strRelateTccDir.IsEmpty())
	{
		CString path = gpcdef.GetDataDir();
		path += "\\"+strRelateTccDir+"\\tccline.cfg";
		if(nRelateTccType==2)
		{
			TCC::CTccGpcInfoProvider::LoadTccLineNameConfig(path);
		}
		else if(nRelateTccType==3)
		{
			TCC_C3::CTccGpcInfoProvider::LoadTccLineNameConfig(path);
		}
	}

	return;
}

void CMainFrame::OnTccLimitspeed()
{
	int SectionNo = m_gpcContext.mSectionId;
	if(SectionNo<=0)
		return;

	int nOldSection = gpcdef.GetTccCurrentSectNo();
	gpcdef.SetTccCurrentSectNo(SectionNo);

	bool bTCCExecute  = gpcdef.IsTCCExecute();   // 执行权限
	bool bTCCReadOnly = gpcdef.IsTCCReadOnly();  // 编辑权限
	bool bCTCGpc      = gpcdef.IsCTCGpc(); // GPC类别
    
	CString szTCCDir = gpcdef.GetDataDir();
	szTCCDir += "\\";
	
	CString szTccVersion = gpcdef.GetTccVersionString();
	if(szTccVersion=="C3")
	{
		CString szFile = szTCCDir+gpcdef.GetTccCfgDir(3) + "\\command-id.xml";
		CFileFind fFind;
		if(!fFind.FindFile(szFile))
		{
			CString str;
			str.Format("%s 不存在!", szFile);
			AfxMessageBox(str);

			return;
		}

		if(m_pTcc3InfoProvider == NULL && m_pTcc3AdviseSink == NULL)
		{
			m_pTcc3InfoProvider = new TCC_C3::CTccGpcInfoProvider(); // wintgdoc
			m_pTcc3AdviseSink = new TCC_C3::CTccGpcAdviseSink(m_pTcc3InfoProvider, this);
		}

		if(nOldSection != SectionNo)
		{
			m_pTcc3InfoProvider->SetTCCRight(SectionNo,bTCCReadOnly,bTCCExecute,bCTCGpc);
			m_pTcc3InfoProvider->LoadTccLineConfig();

			LoadRelateTccLine(3);
		}

		TCC::TccDialogHide();
		TCC::UnRegisterTCCMsgProcHandleWnd();
		if(NULL == TCC_C3::TccDialogOpen())
			TCC_C3::TccDialog(m_pTcc3InfoProvider, m_pTcc3AdviseSink, this, "临时限速命令");
		TCC_C3::RegisterTCCMsgProcHandleWnd();
	}
	else if(szTccVersion=="C2")
	{
		CString szFile = szTCCDir+gpcdef.GetTccCfgDir(2) + "\\command-id.xml";
		CFileFind fFind;
		if(!fFind.FindFile(szFile))
		{
			CString str;
			str.Format("%s 不存在!", szFile);
			AfxMessageBox(str);

			return;
		}

		if(m_pTccInfoProvider == NULL && m_pTccAdviseSink == NULL)
		{
			m_pTccInfoProvider = new TCC::CTccGpcInfoProvider();
			m_pTccAdviseSink = new TCC::CTccGpcAdviseSink(m_pTccInfoProvider,this);
		}

		if(nOldSection != SectionNo)
		{
			m_pTccInfoProvider->SetTCCRight(SectionNo,bTCCReadOnly,bTCCExecute,bCTCGpc);
			m_pTccInfoProvider->LoadTccLineConfig();

			LoadRelateTccLine(2);
		}
		
		TCC_C3::TccDialogHide();
		TCC_C3::UnRegisterTCCMsgProcHandleWnd();
		if(NULL == TCC::TccDialogOpen())
			TCC::TccDialog(m_pTccInfoProvider, m_pTccAdviseSink, this, "临时限速命令");
		TCC::RegisterTCCMsgProcHandleWnd();
	}
	else if(szTccVersion=="C4")
	{
		CString szFile = szTCCDir+gpcdef.GetTccCfgDir(3) + "\\command-id.xml";
		CFileFind fFind;
		if(!fFind.FindFile(szFile))
		{
			CString str;
			str.Format("%s 不存在!", szFile);
			AfxMessageBox(str);

			return;
		}

		if(m_pTcc3InfoProvider == NULL && m_pTcc3AdviseSink == NULL)
		{
			m_pTcc3InfoProvider = new TCC_C3::CTccGpcInfoProvider(); // wintgdoc
			m_pTcc3AdviseSink = new TCC_C3::CTccGpcAdviseSink(m_pTcc3InfoProvider, this);
		}

		if(nOldSection != SectionNo)
		{
			m_pTcc3InfoProvider->SetTCCRight(SectionNo,bTCCReadOnly,bTCCExecute,bCTCGpc);
			m_pTcc3InfoProvider->LoadTccLineConfig();

			LoadRelateTccLine(2);
		}

		szFile = szTCCDir+gpcdef.GetTccCfgDir(2) + "\\command-id.xml";
		if(!fFind.FindFile(szFile))
		{
			CString str;
			str.Format("%s 不存在!", szFile);
			AfxMessageBox(str);

			return;
		}
		if(m_pTccInfoProvider == NULL && m_pTccAdviseSink == NULL)
		{
			m_pTccInfoProvider = new TCC::CTccGpcInfoProvider();
			m_pTccAdviseSink = new TCC::CTccGpcAdviseSink(m_pTccInfoProvider,this);
		}

		if(nOldSection != SectionNo)
		{
			m_pTccInfoProvider->SetTCCRight(SectionNo,bTCCReadOnly,bTCCExecute,bCTCGpc);
			m_pTccInfoProvider->LoadTccLineConfig();

			LoadRelateTccLine(3);
		}
	
		CDlgTCCSelect dlg;
		if(dlg.DoModal()==IDOK)
		{
			if(dlg.m_nSelSchdType==2)
			{
				TCC_C3::TccDialogHide();
				TCC_C3::UnRegisterTCCMsgProcHandleWnd();
				if(NULL == TCC::TccDialogOpen())
					TCC::TccDialog(m_pTccInfoProvider, m_pTccAdviseSink, this, "临时限速命令");

				TCC::RegisterTCCMsgProcHandleWnd();
			}
			if(dlg.m_nSelSchdType==3)
			{
				TCC::TccDialogHide();
				TCC::UnRegisterTCCMsgProcHandleWnd();
				if(NULL == TCC_C3::TccDialogOpen())
					TCC_C3::TccDialog(m_pTcc3InfoProvider, m_pTcc3AdviseSink, this, "临时限速命令");

				TCC_C3::RegisterTCCMsgProcHandleWnd();
			}
		}
	}
}

void CMainFrame::OnAbout()
{
	CDlgAbout dlg;
	dlg.DoModal();
}

void CMainFrame::OnPasswayManagement() 
{		
	if(m_gpcContext.mSectionId > 0)
	{
		sysprint(0,PMTG.ThreadId,"[PMTG] WintgFrame::OnPasswayManagement");		
		CPropertyPagePasswayManagement dlg(m_gpcContext.mSectionId, this);
		dlg.DoModal();
	}
}

void CMainFrame::OnSystemComWnd()
{
	ShowCommWindow(true);
}


void CMainFrame::OnAbsTime() 
{
	int SectionNo;
	SectionNo=m_gpcContext.mSectionId;
	if(SectionNo<0)
		return;
	CAbsTimeDlg dlg(SectionNo,this); //add by cf after 9-9
	dlg.DoModal();
}

void CMainFrame::OnSaveToFiles()
{
	CTgDocument::GlobalSaveToLocalFiles();
}

void CMainFrame::OnLoadFromFiles()
{
	if(MessageBox("确认将本地保存的运行图数据恢复吗?","警告",MB_YESNO)==IDYES)
		CTgDocument::GlobalLoadFromLocalFiles();
}

void CMainFrame::OnMaintainWord()
{
	CDlgWordMaintain dlg;
	dlg.DoModal(); 
}
void CMainFrame::OnMemoryWnd()
{
	m_DlgMemory.RefreshShow();
	m_DlgMemory.ShowWindow(SW_SHOW);
}
LRESULT CMainFrame::OnMemoryNotify(WPARAM wParam, LPARAM lParam)
{
	if(lParam == 0)
	{
		if(wParam==0) // diable alert
			m_DlgMemory.ShowWindow(SW_HIDE);
		else
		{
			m_DlgMemory.RefreshShow();
			m_DlgMemory.ShowWindow(SW_SHOW);
		}
	}
	else if(lParam == 1)
	{
		CWnd* pActiveChildWnd=MDIGetActive(NULL);
		if(pActiveChildWnd!=NULL)
		{
			CMDIChildWnd* pChildWnd = (CMDIChildWnd*)pActiveChildWnd->GetWindow(GW_HWNDFIRST);
			while(pChildWnd!=NULL)
			{
				if(pChildWnd->IsKindOf(RUNTIME_CLASS(CWorkTgFrame)))
				{
					if(wParam == 0)
						pChildWnd->SendMessage(UM_MEMORY_NOIFY, 0);
					else
						pChildWnd->SendMessage(UM_MEMORY_NOIFY, 1);
				}
				pChildWnd=(CMDIChildWnd*)pChildWnd->GetWindow(GW_HWNDNEXT);
			}
		}
	}
	return 0;
}

void CMainFrame::OnEnablePlaySound() 
{
	if(SoundPlayEnabled())
		EnableSoundPlay(FALSE);
	else
		EnableSoundPlay(TRUE);
}

void CMainFrame::OnUpdateEnablePlaySound(CCmdUI* pCmdUI) 
{
	if(SoundPlayEnabled())
		pCmdUI->SetCheck(1);
	else
		pCmdUI->SetCheck(0);
}
void CMainFrame::OnUserManage()
{
	CSuperPassDlg pdlg;
	if (pdlg.DoModal() == IDOK)
	{
		if(!pdlg.PasswordIsOk())
		{
			CString str;
			str.Format("%s","密码错误!");
			CDlgMessageBox dlg(str);
			dlg.DoModal();
			return;
		}
	}
	else
		return;

	CUserListDlg *dlg = new CUserListDlg; 
	if(dlg != NULL)
	{
		dlg->Create(IDD_USER_LIST); 
		dlg->ShowWindow(SW_SHOW);	
	} 			
}

void CMainFrame::OnUpdateUserManage(CCmdUI* pCmdUI) 
{
	if(gpcdef.GetUserIdentify() == GPC_TEST)
		pCmdUI->Enable(TRUE);
	else
		pCmdUI->Enable(FALSE);
}

LRESULT CMainFrame::OnProgramOnce(WPARAM wParam, LPARAM lParam)
{
	switch(wParam)
	{
	case 0:
		ShowWindow(SW_SHOWNORMAL);
		BringWindowToTop();
		break;
	case 1:
		SendMessage(WM_CLOSE);
		break;
	default:
		break;
	}

	return TRUE;
}

void CMainFrame::OnRightManage()
{
	// TODO: 在此添加命令处理程序代码
	CSuperPassDlg pdlg;
	if (pdlg.DoModal() == IDOK)
	{
		if(!pdlg.PasswordIsOk())
		{
			CString str;
			str.Format("%s","密码错误!");
			CDlgMessageBox dlg(str);
			dlg.DoModal();
			return;
		}
	}
	else
		return;

	CRightListDlg *dlg = new CRightListDlg; 
	if(dlg != NULL)
	{
		dlg->Create(IDD_RIGHT_LIST); 
		dlg->ShowWindow(SW_SHOW);	
	} 		
}

void CMainFrame::OnUpdateRightManage(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	if(gpcdef.IsHaveRightManangRight()==TRUE)
		pCmdUI->Enable(TRUE);
	else
		pCmdUI->Enable(FALSE);
}

void CMainFrame::OnInitMenu(CMenu* pMenu)
{
	CMDIFrameWnd::OnInitMenu(pMenu);

	// TODO: 在此处添加消息处理程序代码
	AddXdtMenus();
}
