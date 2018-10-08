// CMMBar.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include <io.h>
#include <afxinet.h>
#include "mmbar.h"
#include "TGPspData.h"
#include "utility/xml_utility.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#define IDC_MMBAR_EDIT 5111
#define IDC_PLAN_TRN_GRID 5112

/////////////////////////////////////////////////////////////////////////////
// CMMBar

#define  IDC_PLAN_NOTE_GRID  5121

CMMBar::CMMBar()
{
}

CMMBar::~CMMBar()
{
}

BEGIN_MESSAGE_MAP(CMMBar, CSizingControlBar)
	//{{AFX_MSG_MAP(CMMBar)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMMBar message handlers


int CMMBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CSizingControlBar::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rect(0,0,0,0);
	m_caption.Create("施工信息",WS_CHILD|WS_VISIBLE|SS_CENTER,rect,this);//SS_GRAYFRAME
	
	CFont font;
	font.CreatePointFont(140, _T("Arial Narrow"));
	

	m_grid.CreateVerticalGrid(this,rect,IDC_PLAN_NOTE_GRID,g_mmplan_note_view_info,count_of_g_mmplan_note_view_info());
	m_grid.SetFont(&font);
	m_grid.AutoSizeColumns();
	m_grid.SetEditable(TRUE);
	return 0;
}

void CMMBar::OnSize(UINT nType, int cx, int cy) 
{
	m_caption.MoveWindow(0,0,cx,20);
	m_caption.Invalidate();
	m_grid.MoveWindow(0,20,cx,cy-20);
	m_grid.ExpandColumnsToFit(FALSE);
	m_grid.ExpandRowsToFit(FALSE); 

	CSizingControlBar::OnSize(nType, cx, cy);
}

CSize CMMBar::CalcDynamicLayout(int   nLength,   DWORD   nMode)   
{   
	CSize   size;   
	size   =   CSizingControlBar::CalcDynamicLayout(nLength,   nMode);   
	//if(size.cx   <   800)   size.cx   =   800;   
	if(size.cy   <   1000)   size.cy   =   1000; 
	return   size;   
}   


void CMMBar::OnDestroy() 
{
	CSizingControlBar::OnDestroy();
}

void CMMBar::UpdateShow(const char *xml,const char *node)
{
	m_grid.InitVerticalGrid(xml,node);
}

