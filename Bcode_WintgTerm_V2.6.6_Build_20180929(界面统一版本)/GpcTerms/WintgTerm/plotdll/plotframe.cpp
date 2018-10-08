// PlotFrame.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "PlotFrame.h"
#include "oscalls.h"
#include "sysshare.h"
#include "widgets.h"
#include "Plot24Doc.h"
#include ".\plotframe.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPlotFrame

IMPLEMENT_DYNCREATE(CPlotFrame, CFrameWnd)

CPlotFrame::CPlotFrame()
{
	m_bAutoMenuEnable = FALSE;
}

CPlotFrame::~CPlotFrame()
{
}


BEGIN_MESSAGE_MAP(CPlotFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CPlotFrame)
	ON_COMMAND(ID_PLOT_EXIT, OnPlotExit)
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_LINE_DIRECT, OnLineDirect) 
	ON_UPDATE_COMMAND_UI(ID_LINE_DIRECT, OnUpdateLineDirect)

	ON_COMMAND(ID_ARL_AND_DEPART, OnArlAndDepart)
	ON_UPDATE_COMMAND_UI(ID_ARL_AND_DEPART, OnUpdateArlAndDepart)

	ON_COMMAND(ID_DIRECT_ARL_AND_DEPART, OnDirectArlAndDepart)
	ON_UPDATE_COMMAND_UI(ID_DIRECT_ARL_AND_DEPART, OnUpdateDirectArlAndDepart)
	
	ON_COMMAND(ID_WAVE_PASS, OnWavePass)
	ON_UPDATE_COMMAND_UI(ID_WAVE_PASS,OnUpdateWavePass)

	ON_COMMAND(ID_SHOW_VIRTUAL_STATION_PLOT, OnShowVirtualStationPlot)
	ON_UPDATE_COMMAND_UI(ID_SHOW_VIRTUAL_STATION_PLOT,OnUpdateVirtualStationPlot)

	ON_COMMAND(ID_PASSENGER, OnPassenger)
	ON_UPDATE_COMMAND_UI(ID_PASSENGER, OnUpdatePassenger)

	ON_COMMAND(ID_GOODS, OnGoods)
	ON_UPDATE_COMMAND_UI(ID_GOODS, OnUpdateGoods)

	ON_COMMAND(ID_PESSAGEGOODS, OnPessagegoods)
	ON_UPDATE_COMMAND_UI(ID_PESSAGEGOODS, OnUpdatePessagegoods)

	ON_COMMAND(ID_THIN_LINE, OnThinLine)
	ON_UPDATE_COMMAND_UI(ID_THIN_LINE, OnUpdateThinLine)

	ON_COMMAND_RANGE(ID_PESSAHER_TRAIN, ID_GOODANDPESSAGER, OnTrainType)
	ON_UPDATE_COMMAND_UI_RANGE(ID_PESSAHER_TRAIN, ID_GOODANDPESSAGER, OnUpdateTrainType)

	ON_COMMAND_RANGE(ID_UPTRAIN, ID_UPANDDOWNTRAIN, OnTrainDirect)
	ON_UPDATE_COMMAND_UI_RANGE(ID_UPTRAIN, ID_UPANDDOWNTRAIN, OnUpdateTrainDirect)

	ON_COMMAND(ID_SHOW_CZH_CONNECT_LINE, OnShowCzhConnectLine)
	ON_UPDATE_COMMAND_UI(ID_SHOW_CZH_CONNECT_LINE,OnUpdateCzhConnectLine)

	ON_COMMAND(ID_SHOW_NOT_TECH_STOP_MARK, OnNotShowTechStopMark)
	ON_UPDATE_COMMAND_UI(ID_SHOW_NOT_TECH_STOP_MARK,OnUpdateNotShowTechStopMark)
	
	ON_WM_INITMENU()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPlotFrame message handlers

BOOL CPlotFrame::PreCreateWindow(CREATESTRUCT& cs) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	cs.lpszClass = AfxRegisterWndClass(CS_DBLCLKS,
		AfxGetApp()->LoadStandardCursor(IDC_ARROW), (HBRUSH)(COLOR_WINDOW+1),
		AfxGetApp()->LoadIcon(IDR_PLOT));

//	cs.style = WS_VISIBLE | WS_POPUP;
    cs.style &= ~FWS_ADDTOTITLE;
	cs.style &= ~WS_MAXIMIZE;
 	cs.style &= ~WS_CAPTION;

	// Size the window to full screen size and center it 
	int screenNum = gpcdef.GetScreenNum();
	int screenx = GetSystemMetrics(SM_CXSCREEN);
	int screeny = GetSystemMetrics(SM_CYSCREEN);

	if(screenNum == 1)
	{
		cs.cx = screenx;
		cs.x = 0;
	}
	else
    {
		if(screenNum == 5)
		{
			cs.cx = screenx/4;
			cs.x = 0;
		}
		else if(screenNum == 6)
		{
			cs.cx = screenx/2;
			cs.x = 0;
		}
		else
		{
			cs.cx = screenx/screenNum;
			cs.x = 0;
		}
		//end by cf
	}
	
	cs.y = 0;
	cs.cy = GetSystemMetrics(SM_CYMAXIMIZED);//screeny;


    // Size the window to full screen size and center it 
	if(gpcdef.GetUserPriority() == GPC_SCREEN_TG)
		cs.hMenu = NULL;

	return CFrameWnd::PreCreateWindow(cs);
}

void CPlotFrame::OnPlotExit() 
{
	// TODO: Add your command handler code here 
	//	PostMessage(UM_SMALL_GRAPH);
 	SendMessage(WM_CLOSE);
}


void CPlotFrame::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	
	CPlot24Doc* pDoc = (CPlot24Doc*)this->GetActiveDocument();
	if(NULL != pDoc)
	{
		pDoc->m_vecSchdTrains.clear();
		pDoc->m_nAbsList.DeleteContents();
	}
	CFrameWnd::OnClose();
}

void CPlotFrame::OnLineDirect()
{
	// TODO: 在此添加命令处理程序代码
	CPlot24Doc* pDoc = (CPlot24Doc*)this->GetActiveDocument();
	if(NULL != pDoc)
	{
		pDoc->m_nTrainLeapLineFlag = 3;
		pDoc->PlotReDraw(this->GetActiveView());
	}
}

void CPlotFrame::OnUpdateLineDirect(CCmdUI *pCmdUI)
{
	CPlot24Doc* pDoc = (CPlot24Doc*)this->GetActiveDocument();
	if(NULL != pDoc)
	{
		if(pDoc->m_nTrainLeapLineFlag == 3)
			pCmdUI->SetCheck(TRUE); 
		else
			pCmdUI->SetCheck(FALSE);  
	}
}

void CPlotFrame::OnArlAndDepart()
{
	// TODO: 在此添加命令处理程序代码
	CPlot24Doc* pDoc = (CPlot24Doc*)this->GetActiveDocument();
	if(NULL != pDoc)
	{
		pDoc->m_nTrainLeapLineFlag = 2;
		pDoc->PlotReDraw(this->GetActiveView());
	}
}

void CPlotFrame::OnUpdateArlAndDepart(CCmdUI *pCmdUI)
{
	CPlot24Doc* pDoc = (CPlot24Doc*)this->GetActiveDocument();
	if(NULL != pDoc)
	{
		if(pDoc->m_nTrainLeapLineFlag == 2)
			pCmdUI->SetCheck(TRUE); 
		else
			pCmdUI->SetCheck(FALSE);  
	}
}

void CPlotFrame::OnDirectArlAndDepart()
{
	// TODO: 在此添加命令处理程序代码
	CPlot24Doc* pDoc = (CPlot24Doc*)this->GetActiveDocument();
	if(NULL != pDoc)
	{
		pDoc->m_nTrainLeapLineFlag = 1; // 1：无标示 2 接入交出 3:直接连线
		pDoc->PlotReDraw(this->GetActiveView());
	}
}

void CPlotFrame::OnUpdateDirectArlAndDepart(CCmdUI *pCmdUI)
{
	CPlot24Doc* pDoc = (CPlot24Doc*)this->GetActiveDocument();
	if(NULL != pDoc)
	{
		if(pDoc->m_nTrainLeapLineFlag == 1)
			pCmdUI->SetCheck(TRUE); 
		else
			pCmdUI->SetCheck(FALSE);  
	}
}

void CPlotFrame::OnWavePass()
{
	// TODO: 在此添加命令处理程序代码
	CPlot24Doc* pDoc = (CPlot24Doc*)this->GetActiveDocument();
	if(NULL != pDoc)
	{
		if(pDoc->m_bStopStationWaveLine == TRUE)
			pDoc->m_bStopStationWaveLine = FALSE;
		else
			pDoc->m_bStopStationWaveLine = TRUE;
		pDoc->PlotReDraw(this->GetActiveView());
	}
}

void CPlotFrame::OnUpdateWavePass(CCmdUI *pCmdUI)
{
	CPlot24Doc* pDoc = (CPlot24Doc*)this->GetActiveDocument();
	if(NULL != pDoc)
	{
		if(pDoc->m_bStopStationWaveLine == TRUE)
			pCmdUI->SetCheck(TRUE); 
		else
			pCmdUI->SetCheck(FALSE);  
	}
}

void CPlotFrame::OnThinLine()
{
	CPlot24Doc* pDoc = (CPlot24Doc*)this->GetActiveDocument();
	if(NULL != pDoc)
	{
		if(pDoc->m_nLineWidthRate == 1)
			pDoc->m_nLineWidthRate = 2;
		else
			pDoc->m_nLineWidthRate = 1;
		pDoc->PlotReDraw(this->GetActiveView());
	}
}

void CPlotFrame::OnUpdateThinLine(CCmdUI *pCmdUI)
{
	CPlot24Doc* pDoc = (CPlot24Doc*)this->GetActiveDocument();
	if(NULL != pDoc)
	{
		if(pDoc->m_nLineWidthRate == 1)
			pCmdUI->SetCheck(1);
		else
			pCmdUI->SetCheck(0);
	}
}

void CPlotFrame::OnPassenger()
{
	CPlot24Doc* pDoc = (CPlot24Doc*)this->GetActiveDocument();
	if(NULL != pDoc)
	{
		if(pDoc->m_nShowTrainEarlyLateFlag == 1)
			return;
		pDoc->m_nShowTrainEarlyLateFlag = 1;
		pDoc->PlotReDraw(this->GetActiveView());
	}
}

void CPlotFrame::OnGoods()
{
	CPlot24Doc* pDoc = (CPlot24Doc*)this->GetActiveDocument();
	if(NULL != pDoc)
	{
		if(pDoc->m_nShowTrainEarlyLateFlag == 2)
			return;
		pDoc->m_nShowTrainEarlyLateFlag = 2;
		pDoc->PlotReDraw(this->GetActiveView());
	}
}

void CPlotFrame::OnPessagegoods()
{
	CPlot24Doc* pDoc = (CPlot24Doc*)this->GetActiveDocument();
	if(NULL != pDoc)
	{
		if(pDoc->m_nShowTrainEarlyLateFlag == 3)
			return;
		pDoc->m_nShowTrainEarlyLateFlag = 3;
		pDoc->PlotReDraw(this->GetActiveView());
	}
}
void CPlotFrame::OnUpdatePassenger(CCmdUI *pCmdUI)
{
	CPlot24Doc* pDoc = (CPlot24Doc*)this->GetActiveDocument();
	if(NULL != pDoc)
	{
		if(pDoc->m_nShowTrainEarlyLateFlag == 1)
			pCmdUI->SetCheck(TRUE); 
		else
			pCmdUI->SetCheck(FALSE);  
	}
}
void CPlotFrame::OnUpdateGoods(CCmdUI *pCmdUI)
{
	CPlot24Doc* pDoc = (CPlot24Doc*)this->GetActiveDocument();
	if(NULL != pDoc)
	{
		if(pDoc->m_nShowTrainEarlyLateFlag == 2)
			pCmdUI->SetCheck(TRUE); 
		else
			pCmdUI->SetCheck(FALSE);  
	}
}
void CPlotFrame::OnUpdatePessagegoods(CCmdUI *pCmdUI)
{
	CPlot24Doc* pDoc = (CPlot24Doc*)this->GetActiveDocument();
	if(NULL != pDoc)
	{
		if(pDoc->m_nShowTrainEarlyLateFlag == 3)
			pCmdUI->SetCheck(TRUE); 
		else
			pCmdUI->SetCheck(FALSE);  
	}
}

void CPlotFrame::OnTrainType(UINT nID)
{
	CPlot24Doc* pDoc = (CPlot24Doc*)this->GetActiveDocument();
	if(NULL != pDoc)
	{
		pDoc->m_nShowTrainFlag = nID - ID_PESSAHER_TRAIN;
		pDoc->PlotReDraw(this->GetActiveView());
	}
}

void CPlotFrame::OnUpdateTrainType(CCmdUI *pCmdUI)
{
	CPlot24Doc* pDoc = (CPlot24Doc*)this->GetActiveDocument();
	if(NULL != pDoc)
	{
		int nID=pCmdUI->m_nID; 
		if(nID==ID_PESSAHER_TRAIN)
		{
			pCmdUI->SetCheck(pDoc->m_nShowTrainFlag==0);  
			pCmdUI->Enable(pDoc->m_nShowTrainFlag!=0);  
		}
		else	if(nID==ID_GOODANDPESSAGER)
		{
			pCmdUI->SetCheck(pDoc->m_nShowTrainFlag==2);  
			pCmdUI->Enable(pDoc->m_nShowTrainFlag!=2);  
		}
		else
		{
			pCmdUI->SetCheck(pDoc->m_nShowTrainFlag==1);  
			pCmdUI->Enable(pDoc->m_nShowTrainFlag!=1);   
		}
	}
}

void CPlotFrame::OnTrainDirect(UINT nID)
{
	CPlot24Doc* pDoc = (CPlot24Doc*)this->GetActiveDocument();
	if(NULL != pDoc)
	{
		pDoc->m_nShowDirectFlag = nID - ID_UPTRAIN;
		pDoc->PlotReDraw(this->GetActiveView());
	}
}

void CPlotFrame::OnUpdateTrainDirect(CCmdUI *pCmdUI)
{
	CPlot24Doc* pDoc = (CPlot24Doc*)this->GetActiveDocument();
	if(NULL != pDoc)
	{
		int nID=pCmdUI->m_nID; 
		if(nID==ID_UPTRAIN)
		{
			pCmdUI->SetCheck(pDoc->m_nShowDirectFlag==0);  
			pCmdUI->Enable(pDoc->m_nShowDirectFlag!=0);  
		}
		else	if(nID==ID_UPANDDOWNTRAIN)
		{
			pCmdUI->SetCheck(pDoc->m_nShowDirectFlag==2);  
			pCmdUI->Enable(pDoc->m_nShowDirectFlag!=2);  
		}
		else
		{
			pCmdUI->SetCheck(pDoc->m_nShowDirectFlag==1);  
			pCmdUI->Enable(pDoc->m_nShowDirectFlag!=1);   
		}
	}
}

void CPlotFrame::OnShowVirtualStationPlot()
{
	// TODO: 在此添加命令处理程序代码
	CPlot24Doc* pDoc = (CPlot24Doc*)this->GetActiveDocument();
	if(NULL != pDoc)
	{
		if(pDoc->m_bShowVirtualStationOnPlot == TRUE)
			pDoc->m_bShowVirtualStationOnPlot = FALSE;
		else
			pDoc->m_bShowVirtualStationOnPlot = TRUE;
		pDoc->PlotReDraw(this->GetActiveView());
	}
}

void CPlotFrame::OnUpdateVirtualStationPlot(CCmdUI *pCmdUI)
{
	CPlot24Doc* pDoc = (CPlot24Doc*)this->GetActiveDocument();
	if(NULL != pDoc)
	{
		if(pDoc->m_bShowVirtualStationOnPlot == TRUE)
			pCmdUI->SetCheck(TRUE); 
		else
			pCmdUI->SetCheck(FALSE);  
	}
}

void CPlotFrame::OnShowCzhConnectLine()
{
	// TODO: 在此添加命令处理程序代码
	CPlot24Doc* pDoc = (CPlot24Doc*)this->GetActiveDocument();
	if(NULL != pDoc)
	{
		if(pDoc->m_bShowCzhConnectLineOnPlot)
			pDoc->m_bShowCzhConnectLineOnPlot = false;
		else
			pDoc->m_bShowCzhConnectLineOnPlot = true;
		pDoc->PlotReDraw(this->GetActiveView());
	}
}

void CPlotFrame::OnUpdateCzhConnectLine(CCmdUI *pCmdUI)
{
	CPlot24Doc* pDoc = (CPlot24Doc*)this->GetActiveDocument();
	if(NULL != pDoc)
	{
		if(pDoc->m_bShowCzhConnectLineOnPlot)
			pCmdUI->SetCheck(TRUE); 
		else
			pCmdUI->SetCheck(FALSE);  
	}
}

void CPlotFrame::OnNotShowTechStopMark()
{
	// TODO: 在此添加命令处理程序代码
	CPlot24Doc* pDoc = (CPlot24Doc*)this->GetActiveDocument();
	if(NULL != pDoc)
	{
		if(pDoc->m_bShowNotTechStopFlag)
			pDoc->m_bShowNotTechStopFlag = false;
		else
			pDoc->m_bShowNotTechStopFlag = true;
		pDoc->PlotReDraw(this->GetActiveView());
	}
}

void CPlotFrame::OnUpdateNotShowTechStopMark(CCmdUI *pCmdUI)
{
	CPlot24Doc* pDoc = (CPlot24Doc*)this->GetActiveDocument();
	if(NULL != pDoc)
	{
		if(pDoc->m_bShowNotTechStopFlag)
			pCmdUI->SetCheck(FALSE); 
		else
			pCmdUI->SetCheck(TRUE);  
	}
}

int CPlotFrame::FindMenuItem(CMenu* Menu, LPCTSTR MenuString)
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

void CPlotFrame::OnInitMenu(CMenu* pMenu)
{
	CFrameWnd::OnInitMenu(pMenu);

	if (!gpcdef.IsShowVirtualStation())
	{
		CMenu* mmenu=GetMenu();
		if (mmenu==NULL)
			return ;

		int npos = FindMenuItem(mmenu, "选项");
		CMenu* submenu = mmenu->GetSubMenu(npos);
		if(submenu)
		{
			int pos = FindMenuItem(submenu, "显示虚拟车站");
			if (pos >= 0)
				submenu->DeleteMenu(pos,MF_BYPOSITION);	
		}
	}

	if (!GetTGFunctionConfig()->IsEnableCzh() && !GetTGFunctionConfig()->IsEnableNewCzh())
	{
		CMenu* mmenu=GetMenu();
		if (mmenu==NULL)
			return ;

		int npos = FindMenuItem(mmenu, "选项");
		CMenu* submenu = mmenu->GetSubMenu(npos);
		if(submenu)
		{
			int pos = FindMenuItem(submenu, "显示车组勾连线");
			if (pos >= 0)
				submenu->DeleteMenu(pos,MF_BYPOSITION);	
		}
	}
}
