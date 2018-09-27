// WinPrint.cpp : implementation file
//

#include "stdafx.h"
#include <oscalls.h>
#include "WinPrint.h"
#include "dlglogfilter.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "function.h"

/////////////////////////////////////////////////////////////////////////////
// CPrintFrame
IMPLEMENT_DYNCREATE(CPrintFrame, CFrameWnd)

CPrintFrame::CPrintFrame()
{
	m_bAlwaysTopMost = TRUE;
}

CPrintFrame::~CPrintFrame()
{
}


BEGIN_MESSAGE_MAP(CPrintFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CPrintFrame)
	ON_WM_CLOSE()
	ON_COMMAND(ID_ALWAYS_TOPMOST, OnAlwaysTopmost)
	ON_UPDATE_COMMAND_UI(ID_ALWAYS_TOPMOST, OnAlwaysTopmost_UpdateUI)
	ON_MESSAGE(ID_ALWAYS_TOPMOST, OnShowTopmost)
	ON_WM_SIZE()
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPrintFrame message handlers

BOOL CPrintFrame::PreCreateWindow(CREATESTRUCT& cs) 
{
	cs.style = WS_VISIBLE|WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX;
	cs.hMenu = NULL;
	//cs.style = WS_OVERLAPPED & ~WS_CAPTION;
	//cs.style = WS_OVERLAPPEDWINDOW & ~WS_SYSMENU;
	//cs.style = WS_POPUP | WS_BORDER;
	
    // Size the window to full screen size and center it
	/*if(::GetSystemMetrics(SM_CXSCREEN) == 1024)
	{
		cs.cy = 210;
		cs.cx = ::GetSystemMetrics(SM_CXSCREEN) * 2 / 3; 
		cs.y = ::GetSystemMetrics(SM_CYSCREEN) - cs.cy; 
		cs.x = cs.cx / 4; 
	} else {
		cs.cy = 210;
		cs.cx = ::GetSystemMetrics(SM_CXSCREEN) / 3; 
		cs.y = ::GetSystemMetrics(SM_CYSCREEN) - cs.cy; 
		cs.x = cs.cx; 
	}*/
	//cs.hMenu = NULL;
	return CFrameWnd::PreCreateWindow(cs);
}

int CPrintFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	if(!m_tb.CreateEx(this) || !m_tb.LoadToolBar(IDR_TB_LOG))
		return -1;
	EnableDocking(CBRS_ALIGN_ANY);
	m_tb.EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_tb, AFX_IDW_DOCKBAR_LEFT, NULL);

	//CMenu* pMenu = GetSystemMenu(FALSE);
	//ASSERT(pMenu != NULL);
	//pMenu->EnableMenuItem(SC_CLOSE,MF_BYCOMMAND | MF_DISABLED|MF_GRAYED);
	
	if(m_bAlwaysTopMost)
		SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE);

	return 0;
}

BOOL CPrintFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext) 
{
	CCreateContext cs;
	cs.m_pCurrentFrame = this;
	cs.m_pNewViewClass = RUNTIME_CLASS(CInfoForm);

	return CFrameWnd::OnCreateClient(lpcs, &cs);
}


void CPrintFrame::OnAlwaysTopmost() 
{
	m_bAlwaysTopMost = !m_bAlwaysTopMost;
	if(m_bAlwaysTopMost)
		SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE);
	else
		SetWindowPos(&wndNoTopMost, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE);
}

void CPrintFrame::OnAlwaysTopmost_UpdateUI(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_bAlwaysTopMost);
}

LRESULT CPrintFrame::OnShowTopmost(WPARAM wParam, LPARAM lParam) 
{
	ShowWindow(SW_SHOWNORMAL);
	if(!m_bAlwaysTopMost)
		OnAlwaysTopmost();
	return TRUE;
}

void CPrintFrame::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	//ShowWindow(SW_HIDE);
}

void CPrintFrame::OnSize(UINT nType, int cx, int cy) 
{
	CFrameWnd::OnSize(nType, cx, cy);
}




#define UM_LIST_STRING_APPEND  WM_USER+1

/////////////////////////////////////////////////////////////////////////////
// CInfoForm
IMPLEMENT_DYNCREATE(CInfoForm, CFormView)

CInfoForm::CInfoForm()
	: CFormView(CInfoForm::IDD)
{
	//{{AFX_DATA_INIT(CInfoForm)
	//}}AFX_DATA_INIT
}

CInfoForm::~CInfoForm()
{
}

void CInfoForm::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInfoForm)
	DDX_Control(pDX, IDC_LIST_INFO, m_listGeneralLog);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CInfoForm, CFormView)
	//{{AFX_MSG_MAP(CInfoForm)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_COMMAND(ID_GENERALLOG, OnGenerallog)
	ON_UPDATE_COMMAND_UI(ID_GENERALLOG, OnGenerallog_UpdateUI)
	ON_COMMAND(ID_CTCALARMLOG, OnCtcalarmlog)
	ON_UPDATE_COMMAND_UI(ID_CTCALARMLOG, OnCtcalarmlog_UpdateUI)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInfoForm diagnostics

#ifdef _DEBUG
void CInfoForm::AssertValid() const
{
	CFormView::AssertValid();
}

void CInfoForm::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CInfoForm message handlers

int CInfoForm::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CFormView::OnCreate(lpCreateStruct) == -1)
		return -1;

	CCreateContext *pcs = (CCreateContext*)lpCreateStruct->lpCreateParams;
	pcs->m_pCurrentFrame->SetActiveView(this);

	mSysAlarmPage.Create(mSysAlarmPage.IDD, this);
	mSysAlarmPage.ShowWindow(SW_SHOW);
	mSysAlarmPage.UpdateWindow();

	return 0;
}

void CInfoForm::OnInitialUpdate() 
{
	UpdateData(FALSE);

	OnCtcalarmlog();

	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();
	
	CRect rc;
	GetClientRect(&rc);
	mSysAlarmPage.MoveWindow(&rc);
	SetScrollSizes(MM_TEXT, CSize(1, 1));

	CFormView::OnInitialUpdate();
}

BOOL CInfoForm::PreCreateWindow(CREATESTRUCT& cs) 
{
	return CFormView::PreCreateWindow(cs);
}


void CInfoForm::OnSize(UINT nType, int cx, int cy) 
{
	CFormView::OnSize(nType, cx, cy);
	if(m_listGeneralLog.GetSafeHwnd())
		m_listGeneralLog.MoveWindow(0, 0, cx, cy);
	if(mSysAlarmPage.GetSafeHwnd())
		mSysAlarmPage.MoveWindow(0, 0, cx, cy);
}

extern void ShowLogFiles(const char *key,CListBox *plist,const char *filter,int max_line,int days);


void CInfoForm::OnGenerallog() 
{
	CRect rectClient;
	GetClientRect(&rectClient);
	m_listGeneralLog.ShowWindow(SW_SHOWNOACTIVATE);
	m_listGeneralLog.MoveWindow(rectClient);
	m_listGeneralLog.UpdateWindow();
	GetParentFrame()->SetWindowText("日志文件查询");
	mSysAlarmPage.ShowWindow(SW_HIDE);
	CDlgLogFilter dlg;
	dlg.m_szFilterKey=m_szFilterKey;
	if(dlg.DoModal()==IDOK)
	{
		m_szFilterKey=dlg.m_szFilterKey;
		m_listGeneralLog.ResetContent();
		CString filter="";
		ShowLogFiles("ctcinfo",&m_listGeneralLog,m_szFilterKey,2000,2); //暂时最多显示2000行,最近2天的记录
	}
}

void CInfoForm::OnGenerallog_UpdateUI(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_listGeneralLog.IsWindowVisible());
}

void CInfoForm::OnCtcalarmlog() 
{
	CRect rectClient;
	GetClientRect(&rectClient);
	mSysAlarmPage.ShowWindow(SW_SHOWNOACTIVATE);
	mSysAlarmPage.MoveWindow(rectClient);
	mSysAlarmPage.UpdateWindow();
	GetParentFrame()->SetWindowText("CTC报警实时日志");

	m_listGeneralLog.ShowWindow(SW_HIDE);
}

void CInfoForm::OnCtcalarmlog_UpdateUI(CCmdUI* pCmdUI) 
{
 	pCmdUI->SetCheck(mSysAlarmPage.IsWindowVisible());
}


