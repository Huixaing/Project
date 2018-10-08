// MainFrm.cpp : implementation of the CDlgMainFrame class
//

#include "stdafx.h"
#include "dlgMainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgMainFrame

IMPLEMENT_DYNAMIC(CDlgMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CDlgMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CDlgMainFrame)
	ON_WM_CREATE()	
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// CDlgMainFrame construction/destruction

CDlgMainFrame::CDlgMainFrame(DRAWFUN pDraw,CWnd*pOldW,CWnd* pCallW,BOOL bDirect,LPCTSTR stTitle,CString &plotName,int screenNum)
{
	// TODO: add member initialization code here	
	Draw=pDraw;
	pOldWnd=pOldW;
	pCallWnd=pCallW;
	bDirectPrint=bDirect;
	CString ss;
	if(stTitle!=NULL)
		ss=_T(stTitle);
	else
	{		
		pOldW->GetWindowText(ss);
		if(!ss.IsEmpty())
			ss=ss+_T(" (预览)");
		else
			ss=ss+_T("打印预览");
	}
	m_sPlotName = plotName;
	m_nScreenNum = screenNum;
	if(!Create(NULL,ss,WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE,CRect(200,200,500,500)))
		TRACE0("Failed to create view window\n");

}


CDlgMainFrame::~CDlgMainFrame()
{   
	//重新显示对话框
	pOldWnd->ShowWindow(SW_SHOW);
	
}

int CDlgMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;	
	CCreateContext context;
	context.m_pNewViewClass = RUNTIME_CLASS(CWrapperView);
	context.m_pCurrentFrame = this;
	context.m_pCurrentDoc = NULL;
	context.m_pLastView = NULL;

	m_pView = STATIC_DOWNCAST(CWrapperView, CreateView(&context)); //CreateView(&context);
	m_pView->m_sPlotName = m_sPlotName;
	
	if(m_pView != NULL)
	{
		if(!bDirectPrint)
			//显示此view
			m_pView->ShowWindow(SW_SHOW);
		//设置为活动的view
		SetActiveView(m_pView);
	}	
	SetIcon(pOldWnd->GetIcon(FALSE),FALSE);
	SetIcon(pOldWnd->GetIcon(TRUE),TRUE);
	//最大化
//	ShowWindow(SW_MAXIMIZE);
	///////////////////获得主应用程序
	//CWinApp *pApp=AfxGetApp();
	//CWnd* pMainWnd = AfxGetThread()->m_pMainWnd;
	CWinThread *pThread = AfxGetThread( );
	///////////////////将frame赋值给应用程序的主窗口！
	pThread->m_pMainWnd=this;
	if(bDirectPrint)
	{
		m_pView->SendMessage(WM_COMMAND, ID_FILE_PRINT);		
	}
	else
	{//	CWrapperView  *m_pView;
		//注意此指针是view的指针
		m_pView->OnFilePrintPreview(this);	
		pOldWnd->ShowWindow(SW_HIDE);
	}
	return 0;
}

BOOL CDlgMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	cs.lpszClass = AfxRegisterWndClass(0);

 	int screenNum = m_nScreenNum;
	int screenx = GetSystemMetrics(SM_CXSCREEN);
	int screeny = GetSystemMetrics(SM_CYSCREEN);
	if(screenNum == 1)
	{
		cs.cx = screenx;
		cs.x = 0;
	}
	else
    {
		//	cs.x = screenx/screenNum;
		//modify by cf at liuzhou
		cs.cx = screenx/screenNum;
		cs.x = 0;
		//end by cf
	}
	cs.y = 0;
	cs.cy = screeny;


	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CDlgMainFrame diagnostics

#ifdef _DEBUG
void CDlgMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CDlgMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CDlgMainFrame message handlers

BOOL CDlgMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// let the view have first crack at the command
	if (m_pView->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		return TRUE;

	// otherwise, do default handling
	return CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}



