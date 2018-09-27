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

#include "SuperPassDlg.h"
#include "DlgMessageBox.h"
#include <baseMsg\msgid.h>
#include <baseMsg\clbth.h>
#include "DlgSelectSection.h"
#include "QuitSystem.h"
#include "dlgabout.h"
#include "tg_schedule_base.h"
#include "BasicTGFrame.h"
#include "db_msgproc.h"
#include "tgdocument.h"

#include "SuperPassDlg.h"

#include "DlgBasicBackupSelect.h"
#include "basic_schd_config.h"
#include ".\mainframe.h"

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

// CMainFrame

#define INVALID_MSGID_CONJOIN		2
IMPLEMENT_DYNCREATE(CMainFrame, CMDIFrameWnd)

CMainFrame::CMainFrame()
{
	m_bAutoMenuEnable = FALSE;
	
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
	ON_COMMAND(ID_BASIC_WIN_NORMAL, OnBasicWin)
	ON_COMMAND(ID_BASIC_WIN_ORTHER, OnBasicPrevWin)
	ON_COMMAND(ID_SELECT_BASIC_BACKUP, OnBasicBackupWin)
	//}}AFX_MSG_MAP
	
	ON_COMMAND(IDC_ABOUT, OnAbout)
	ON_COMMAND(ID_SYS_COMMWND,OnSystemComWnd)
	ON_MESSAGE(UM_STATUS_BAR_UPDATE,OnStatusBarUpdate)

	
	
	ON_COMMAND_RANGE(ID_SELECT_SECTION, ID_SELECT_SECTION + 64,  OnSelectDDT)
	ON_UPDATE_COMMAND_UI_RANGE(ID_SELECT_SECTION, ID_SELECT_SECTION + 64, OnUpdateSelectDDT)

	ON_REGISTERED_MESSAGE(REGIST_MSG_PROGRAMONCE, OnProgramOnce)
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
		
	CMDIFrameWnd::OnDestroy();
}

void CMainFrame::OnClose() 
{
	CQuitSystem *dlg = new CQuitSystem;	
	MessageBeep(MB_ICONASTERISK);
	if(dlg == NULL)
	{
		sysprint(0,PMTG.ThreadId,"[PMTG] memory alloc failed in file %s at line %d",
			__FILE__,__LINE__);
		return;
	}

	if(dlg->DoModal( ) == IDOK)
	{
		GpcLogSysMessage(0,"用户选择确认退出程序.");

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

void CMainFrame::OnTimer(UINT nIDEvent) 
{
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
		((CWintg *)AfxGetApp())->CloseSectionIdFrame(m_gpcContext.mSectionId);
		m_gpcContext.mSectionId = nSelSectionId;
	}
	OnBasicWin();
}

void CMainFrame::OnBasicWin( ) 
{
	if(m_gpcContext.mSectionId > 0)
	{
		((CWintg *)AfxGetApp())->CreateScheduleWindow(
				m_gpcContext.mSectionId,
				CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC);
	}
}

void CMainFrame::OnBasicPrevWin( ) 
{
	if(m_gpcContext.mSectionId > 0)
	{
		((CWintg *)AfxGetApp())->CreateScheduleWindow(
			m_gpcContext.mSectionId,
			CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_PREV);
	}
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
					CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_BACKUP);
		}
	}
}

void CMainFrame::OnAbout()
{
	CDlgAbout dlg;
	dlg.DoModal();
}

void CMainFrame::OnSystemComWnd()
{
	ShowCommWindow(true);
}

void CMainFrame::OnMemoryWnd()
{
	m_DlgMemory.RefreshShow();
	m_DlgMemory.ShowWindow(SW_SHOW);
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

void CMainFrame::OnInitMenu(CMenu* pMenu)
{
	CMDIFrameWnd::OnInitMenu(pMenu);

	// TODO: 在此处添加消息处理程序代码
	AddXdtMenus();
}
