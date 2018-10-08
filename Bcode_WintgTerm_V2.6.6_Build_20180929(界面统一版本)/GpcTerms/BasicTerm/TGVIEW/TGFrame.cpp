// TGFrame.cpp : 实现文件
//

#include "stdafx.h"
#include "resource.h"
#include "TGFrame.h"
#include "sysshare.h"
#include "tgview.h"
#include "function.h"
#include "tgmsg.h"
// CTGFrame

IMPLEMENT_DYNCREATE(CTgFrame, CMDIChildWnd)

CTgFrame::CTgFrame()
{
	for(int i=0;i<TG_BUTTON_NUM;i++)
		m_pButtons[i]=NULL; 
	m_nFrameType=0;

	
}

CTgFrame::~CTgFrame()
{
}


BEGIN_MESSAGE_MAP(CTgFrame, CMDIChildWnd)
	ON_WM_CLOSE()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_COMMAND(ID_WINDOW_PROPERTY,OnShowPropertyWindow)
	ON_UPDATE_COMMAND_UI(ID_WINDOW_PROPERTY,OnUpdateShowPropertyWindow)
	ON_MESSAGE(UM_TG_PROPERTY_UPDATE,OnPropertyUpdate)
	ON_MESSAGE(UM_GET_FRAME_TYPE,OnGetFrameType)
	ON_MESSAGE(UM_SET_FRAME_ACTIVE,OnUserSelectFrame)
END_MESSAGE_MAP()

CTgView  *CTgFrame::GetTgView()
{
	CView *pv=GetActiveView();
	if(pv && pv->IsKindOf(RUNTIME_CLASS(CTgView)))
		return (CTgView *)pv;
	else
		return NULL;
}
// CTGFrame 消息处理程序
void CTgFrame::OnDestroy()
{
	for(int i=0;i<TG_BUTTON_NUM;i++)
	{
		if(m_pButtons[i])
			delete m_pButtons[i];
	}

	CMDIChildWnd::OnDestroy();
}

void CTgFrame::OnClose() 
{
	int m_nCurrentSectionNo=-1;
	CTgView *pv=GetTgView();

	if(pv->CloseView())
		DestroyWindow();

	//CMDIChildWnd::OnClose();
}
BOOL CTgFrame::AddButtonToBar(int i,const char *str,UINT id,UINT style,BOOL enable)
{
	if(i<0 || i>=TG_BUTTON_NUM)
		return FALSE;

	if(!m_pButtons[i])
	{
		m_pButtons[i]=new CColorButton();
		m_pButtons[i]->Attach(0,NULL,RGB(192,192,192));
	}
	else	
		m_pButtons[i]->DestroyWindow();
	CRect	rect;
	m_wndToolBar.GetItemRect(i, &rect);

	if (!m_pButtons[i]->Create(str,WS_CHILD | WS_VISIBLE|style ,rect, &m_wndToolBar, id))	 
	{
		delete m_pButtons[i];
		m_pButtons[i]=NULL;
		return FALSE;
	}
	m_pButtons[i]->SetFont(m_wndToolBar.GetFont());
	m_pButtons[i]->SetWindowPos(m_pButtons[i], rect.left,rect.top , 0, 0,SWP_NOZORDER|SWP_NOSIZE|SWP_NOACTIVATE);
	m_pButtons[i]->EnableWindow(enable);
	return TRUE;
}

BOOL CTgFrame::PreCreateWindow(CREATESTRUCT& cs) 
{
	cs.style &=~FWS_ADDTOTITLE;	
	return CMDIChildWnd::PreCreateWindow(cs);
}

int CTgFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (!m_wndToolBar.Create(this,   WS_BORDER |WS_VISIBLE | CBRS_BOTTOM  )
		|| !m_wndToolBar.LoadToolBar(IDR_WINTG_BOTTOM))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}
	CRect temp;
	int screenx = GetSystemMetrics(SM_CXSCREEN);
	int screenNum = gpcdef.GetScreenNum();
	screenx = screenx/screenNum;
	m_wndToolBar.GetItemRect(0,&temp);
	int width=(screenx-20)/(TG_BUTTON_NUM+2);
	if((width-8)>0&&(temp.Height()-7)>0)
		m_wndToolBar.SetSizes(CSize(width,temp.Height()),CSize(width-8,temp.Height()-7));
	for(int i=0;i<TG_BUTTON_NUM;i++)
	{
		m_wndToolBar.SetButtonInfo(i,0,TBBS_BUTTON,-1);
	}

	if(!m_tgBar.Create("",this, CSize(200,200),TRUE,123))
	{
		TRACE0("Failed to create mybar\n");
		return -1;
		// fail to create
	}

	AddButtonToBar(0,"放大",IDC_WIDTH,0);
	AddButtonToBar(1,"缩小",IDC_SMALL,0);

	AddButtonToBar(2,"上行",ID_DIRECT_UP,BS_RADIOBUTTON);
	AddButtonToBar(3,"下行",ID_DIRECT_DOWN,BS_RADIOBUTTON);
	AddButtonToBar(4,"上下行",ID_DIRECT_MIXED,BS_RADIOBUTTON);

	AddButtonToBar(5,"货车",ID_SHOW_FREIGHT,BS_RADIOBUTTON);
	AddButtonToBar(6,"客车",ID_SHOW_PASSENGER,BS_RADIOBUTTON);
	AddButtonToBar(7,"客货车",ID_SHOW_ALL_TRAIN,BS_RADIOBUTTON);

	m_tgBar.SetBarStyle(m_tgBar.GetBarStyle()|CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);
	m_tgBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_tgBar,AFX_IDW_DOCKBAR_RIGHT);
	m_tgBar.ShowWindow(SW_HIDE);
	
	// 绘制公司标志
	m_wndToolBar.CreateLogo(".\\logo\\logo.bmp");

	return CMDIChildWnd::OnCreate(lpCreateStruct);		
}

void CTgFrame::ShowPropertyWindow(BOOL show)
{
	if(show)
		m_tgBar.ShowWindow(SW_SHOW);
	else
		m_tgBar.ShowWindow(SW_HIDE);
	RecalcLayout();
}

void CTgFrame::OnShowPropertyWindow()
{
	ShowControlBar(&m_tgBar, !m_tgBar.IsWindowVisible(), TRUE);
}

void CTgFrame::OnUpdateShowPropertyWindow(CCmdUI* pCmdUI)
{
	if(m_tgBar.IsWindowVisible())
		pCmdUI->SetCheck(TRUE);
	else
		pCmdUI->SetCheck(FALSE);
}

void CTgFrame::OnShowToolbar() 
{
	if(!m_wndToolBar.GetSafeHwnd())
		return;
	if(!m_wndToolBar.IsWindowVisible())
	{
		m_wndToolBar.ShowWindow(SW_SHOW);
		m_wndToolBar.EnableDocking(CBRS_ALIGN_BOTTOM); 
		EnableDocking(CBRS_ALIGN_BOTTOM); 
		DockControlBar(&m_wndToolBar);	

	}

	MDIMaximize();
}

void CTgFrame::ActivateFrame(int nCmdShow) 
{
	CTgView *pv=GetTgView();
	if(pv)
		m_tgBar.SetDocument(pv->GetDocument());
	nCmdShow = SW_MAXIMIZE;
    
	CMDIChildWnd::ActivateFrame(nCmdShow);
}

LRESULT CTgFrame::OnPropertyUpdate(WPARAM wParam, LPARAM lParam)
{
	int nSectionID=GetTgView()->GetViewSectionNo();
	if(wParam==0)
	{
		if(lParam==0x7FFF) // current object deleted!
		{
			m_tgBar.UpdateShowData(GetTgView(),NULL,0x7FFF,TRUE,nSectionID);
			m_tgBar.SetCurPage(0);
		}
		else
		{
			if(lParam&0x01)
				ShowPropertyWindow(TRUE);
			else
				ShowPropertyWindow(FALSE);
			if(lParam&0x02)
				SetPropertyBarReadOnly(TRUE);
			else
				SetPropertyBarReadOnly(FALSE);
		}
	}
	else if((int)wParam==1)
	{
		m_tgBar.SetCurPage((int)lParam);
	}
	else
	{
		const CTG_LogicObject *pobj=(const CTG_LogicObject *)wParam;
		if(m_tgBar.GetSafeHwnd())
		{
			m_tgBar.UpdateShowData(GetTgView(),pobj,(int)lParam,FALSE,nSectionID);
		}
	}
	return TRUE;
}

LRESULT CTgFrame::OnGetFrameType(WPARAM wParam, LPARAM lParam)
{
	return m_nFrameType;

}

LRESULT CTgFrame::OnUserSelectFrame(WPARAM wParam, LPARAM lParam)
{
	// 当用户用左侧TAB选择不同frame时如此
	return 0;
}
