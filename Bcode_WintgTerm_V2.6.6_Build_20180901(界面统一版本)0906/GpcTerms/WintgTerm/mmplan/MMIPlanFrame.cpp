// MMIPlanFrame.cpp : 实现文件
//

#include "stdafx.h"
#include "resource.h"
#include "MMIPlanFrame.h"
#include ".\mmiplanframe.h"
#include "td\td_data_def.h"
#include "MMPlanView.h"
// CMMIPlanFrame

IMPLEMENT_DYNCREATE(CMMIPlanFrame, CMDIChildWnd)

CMMIPlanFrame::CMMIPlanFrame()
{
}

CMMIPlanFrame::~CMMIPlanFrame()
{
}


BEGIN_MESSAGE_MAP(CMMIPlanFrame, CMDIChildWnd)
	ON_WM_CREATE()
	ON_WM_CLOSE()
	ON_COMMAND(ID_MPLAN_VIEW_INFO, OnMplanViewInfo)
	ON_UPDATE_COMMAND_UI(ID_MPLAN_VIEW_INFO, OnUpdateMplanViewInfo)
	ON_COMMAND(ID_MPLAN_VIEW_DCMD, OnMplanViewDcmd)
	ON_UPDATE_COMMAND_UI(ID_MPLAN_VIEW_DCMD, OnUpdateMplanViewDcmd)
	ON_COMMAND(ID_MPLAN_VIEW_TRAIN_INFO, OnMplanViewTrainInfo)
	ON_UPDATE_COMMAND_UI(ID_MPLAN_VIEW_TRAIN_INFO, OnUpdateMplanViewTrainInfo)
	ON_COMMAND(ID_MPLAN_VIEW_JIESHI, OnMplanViewJieshi)
	ON_UPDATE_COMMAND_UI(ID_MPLAN_VIEW_JIESHI, OnUpdateMplanViewJieshi)
END_MESSAGE_MAP()

// CMMIPlanFrame 消息处理程序
BOOL CMMIPlanFrame::PreCreateWindow(CREATESTRUCT& cs) 
{
	cs.style &=~FWS_ADDTOTITLE;	
	return CMDIChildWnd::PreCreateWindow(cs);
}

BOOL CMMIPlanFrame::AddButtonToBar(CButton& btn,const CString& str,UINT id,CRect &rect)
{
	if (!btn.Create(str,WS_CHILD | WS_VISIBLE ,rect, &m_wndToolBar, id))	 
		return FALSE;
	btn.SetFont(m_wndToolBar.GetFont());
	btn.SetWindowPos(&btn, rect.left,rect.top , 0, 0,SWP_NOZORDER|SWP_NOSIZE|SWP_NOACTIVATE);
	return TRUE;
}
void CMMIPlanFrame::DockControlBarLeftOf(CSizingControlBar *dest_bar,CSizingControlBar *ref_bar,DWORD dw)
{
	CRect rect;
	RecalcLayout();
	ref_bar->GetWindowRect(&rect);
	if(dw==AFX_IDW_DOCKBAR_RIGHT || dw==AFX_IDW_DOCKBAR_LEFT)
		rect.OffsetRect(0,1);
	else
		rect.OffsetRect(1,0);

	DockControlBar(dest_bar,dw,&rect);
}

void CMMIPlanFrame::ReDockBars()
{
	CSizingControlBar *pbars[10];
	int cnt=0;
	if(m_bar.GetSafeHwnd())
		pbars[cnt++]=&m_bar;
	if(m_trnbar.GetSafeHwnd())
		pbars[cnt++]=&m_trnbar;
	if(m_jieshibar.GetSafeHwnd())
		pbars[cnt++]=&m_jieshibar;
	if(m_dcmdbar.GetSafeHwnd())
		pbars[cnt++]=&m_dcmdbar;
	if(cnt<=0)
		return;
	DockControlBar(pbars[0],AFX_IDW_DOCKBAR_RIGHT);
	for(int i=1;i<cnt;i++)
		DockControlBarLeftOf(pbars[i],pbars[i-1],AFX_IDW_DOCKBAR_RIGHT);
}

void CMMIPlanFrame::Create1Bar(CSizingControlBar *pbar,UINT id)
{
	if(pbar->GetSafeHwnd())
	{
		pbar->ShowWindow(SW_SHOW);
		return;
	}
	if(!pbar->Create("",this, CSize(600,200),TRUE,id))
		return;
	pbar->SetBarStyle(pbar->GetBarStyle()|CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);
	pbar->EnableDocking(CBRS_ALIGN_ANY);
}

int  CMMIPlanFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	Create1Bar(&m_bar,123);
	Create1Bar(&m_dcmdbar,124);
	Create1Bar(&m_trnbar,125);
	Create1Bar(&m_jieshibar,126);
	EnableDocking(CBRS_ALIGN_ANY);
	ReDockBars();
	//IDR_MPLAN_BOTTOM
	if(!m_wndToolBar.Create(this,   WS_BORDER |WS_VISIBLE | CBRS_BOTTOM ) ||
		! m_wndToolBar.LoadToolBar(IDR_MPLAN_BOTTOM))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}
	m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle() 
		| CBRS_TOOLTIPS |CBRS_BORDER_3D |CBRS_BORDER_LEFT);
	m_wndToolBar.ShowWindow(SW_SHOW);
	m_wndToolBar.EnableDocking(CBRS_ALIGN_BOTTOM ); //CBRS_ALIGN_ANY

	// 绘制公司标志
	m_wndToolBar.CreateLogo(".\\logo\\logo.bmp");

	return CMDIChildWnd::OnCreate(lpCreateStruct);	
}

void CMMIPlanFrame::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	DestroyWindow();
	//CMDIChildWnd::OnClose();
}

void CMMIPlanFrame::ShowInfoInBar(const char *str,const char *node)
{
	if(m_bar.GetSafeHwnd())
		m_bar.UpdateShow(str,node);
}

void CMMIPlanFrame::ShowJieShiInBar(const char *str,const char *node)
{
	if(m_jieshibar.GetSafeHwnd())
		m_jieshibar.UpdateShowJieShi(str,node);
}

void CMMIPlanFrame::ShowDCMDInfoInBar(const char *str,const char *node)
{
	if(m_dcmdbar.GetSafeHwnd())
		m_dcmdbar.UpdateShow(str,node);
}

void CMMIPlanFrame::OnGetMinMaxInfo(MINMAXINFO *lpMMI)
{
	lpMMI->ptMaxSize.x=800; 
	lpMMI->ptMaxSize.y=400;
}

void CMMIPlanFrame::ShowTrnInfoInBar(const char *xml,const char *node)
{
	if(m_trnbar.GetSafeHwnd())
		m_trnbar.UpdateShowTrnInfo(xml,node);
}
void CMMIPlanFrame::OnMplanViewInfo()
{
	if(m_bar.IsVisible())
		m_bar.ShowWindow(SW_HIDE);
	else
		m_bar.ShowWindow(SW_SHOW);
	
	RecalcLayout();
}

void CMMIPlanFrame::OnUpdateMplanViewInfo(CCmdUI *pCmdUI)
{
	if(m_bar.IsVisible())
		pCmdUI->SetCheck(1);
	else
		pCmdUI->SetCheck(0);
}

void CMMIPlanFrame::OnMplanViewDcmd()
{
	if(m_dcmdbar.IsVisible())
		m_dcmdbar.ShowWindow(SW_HIDE);
	else
		m_dcmdbar.ShowWindow(SW_SHOW);
	
	RecalcLayout();
}

void CMMIPlanFrame::OnUpdateMplanViewDcmd(CCmdUI *pCmdUI)
{
	if(m_dcmdbar.IsVisible())
		pCmdUI->SetCheck(1);
	else
		pCmdUI->SetCheck(0);
}

void CMMIPlanFrame::OnMplanViewTrainInfo()
{
	if(m_trnbar.IsVisible())
		m_trnbar.ShowWindow(SW_HIDE);
	else
		m_trnbar.ShowWindow(SW_SHOW);
	
	RecalcLayout();
}

void CMMIPlanFrame::OnUpdateMplanViewTrainInfo(CCmdUI *pCmdUI)
{
	if(m_trnbar.IsVisible())
		pCmdUI->SetCheck(1);
	else
		pCmdUI->SetCheck(0);
}
void CMMIPlanFrame::OnMplanViewJieshi()
{
	if(m_jieshibar.IsVisible())
		m_jieshibar.ShowWindow(SW_HIDE);
	else
		m_jieshibar.ShowWindow(SW_SHOW);
	
	RecalcLayout();
}

void CMMIPlanFrame::OnUpdateMplanViewJieshi(CCmdUI *pCmdUI)
{
	if(m_jieshibar.IsVisible())
		pCmdUI->SetCheck(1);
	else
		pCmdUI->SetCheck(0);
}
