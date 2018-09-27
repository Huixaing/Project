// TYPlanFrame.cpp : 实现文件
//

#include "stdafx.h"
#include "resource.h"
#include "TYPlanFrame.h"
#include ".\typlanframe.h"


// CTYPlanFrame

IMPLEMENT_DYNCREATE(CTYPlanFrame, CMDIChildWnd)

CTYPlanFrame::CTYPlanFrame()
{
}

CTYPlanFrame::~CTYPlanFrame()
{
}


BEGIN_MESSAGE_MAP(CTYPlanFrame, CMDIChildWnd)
	ON_WM_CREATE()
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CTYPlanFrame 消息处理程序
BOOL CTYPlanFrame::PreCreateWindow(CREATESTRUCT& cs) 
{
	cs.style &=~FWS_ADDTOTITLE;	
	return CMDIChildWnd::PreCreateWindow(cs);
}

int  CTYPlanFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	//IDR_TY_BUTTON
	if(!m_wndToolBar.Create(this,   WS_BORDER |WS_VISIBLE | CBRS_BOTTOM ) ||
		! m_wndToolBar.LoadToolBar(IDR_TY_BUTTON))
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

void CTYPlanFrame::OnClose()
{

	CMDIChildWnd::OnClose();
}
