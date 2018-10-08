// CTgBar.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include <io.h>
#include <afxinet.h>
#include "tgbar.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTgBar

CTgBar::CTgBar()
{
}

CTgBar::~CTgBar()
{
}

BEGIN_MESSAGE_MAP(CTgBar, CSizingControlBar)
	//{{AFX_MSG_MAP(CTgBar)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTgBar message handlers

int CTgBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CSizingControlBar::OnCreate(lpCreateStruct) == -1)
		return -1;
	CRect rect(0,0,0,0);
	m_sheet.CreateMe(rect,this);
	return 0;
}

void CTgBar::OnSize(UINT nType, int cx, int cy) 
{
	m_sheet.MoveWindow(0,0,cx,cy);
	CSizingControlBar::OnSize(nType, cx, cy);
	
}

void CTgBar::OnDestroy() 
{
	CSizingControlBar::OnDestroy();
}

void CTgBar::UpdateShowData(CTgView *pview,const CTG_LogicObject *pobj,int para,BOOL switchobject, int conid)
{
	m_sheet.UpdateShowThis(pview,pobj,para,switchobject, conid);
}

void CTgBar::SetCurPage(int page_index)
{
	m_sheet.SetCurPage(page_index);
}