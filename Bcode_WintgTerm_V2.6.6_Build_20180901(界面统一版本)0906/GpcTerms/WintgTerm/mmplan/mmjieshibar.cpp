// CMMJieshiBar.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include <io.h>
#include <afxinet.h>
#include "mmjieshibar.h"
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
// CMMJieshiBar

#define  IDC_PLAN_NOTE_GRID  5121

CMMJieshiBar::CMMJieshiBar()
{
}

CMMJieshiBar::~CMMJieshiBar()
{
}

BEGIN_MESSAGE_MAP(CMMJieshiBar, CSizingControlBar)
	//{{AFX_MSG_MAP(CMMJieshiBar)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMMJieshiBar message handlers


int CMMJieshiBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CSizingControlBar::OnCreate(lpCreateStruct) == -1)
		return -1;
	CRect rect(0,0,0,0);
	m_jieshicap.Create(" ©π§Ω“ æ√¸¡Ó",WS_CHILD|WS_VISIBLE|SS_CENTER,rect,this);//SS_GRAYFRAME
	m_edit.Create(WS_CHILD|WS_VISIBLE|ES_MULTILINE|ES_AUTOVSCROLL|ES_READONLY|WS_BORDER,rect,this,IDC_MMBAR_EDIT);


	return 0;
}

void CMMJieshiBar::OnSize(UINT nType, int cx, int cy) 
{
	m_jieshicap.MoveWindow(0,0,cx,20);
	m_jieshicap.Invalidate();
	m_edit.MoveWindow(0,20,cx,cy-20);
	CSizingControlBar::OnSize(nType, cx, cy);
}

CSize CMMJieshiBar::CalcDynamicLayout(int   nLength,   DWORD   nMode)   
{   
	CSize   size;   
	size   =   CSizingControlBar::CalcDynamicLayout(nLength,   nMode);   
	//if(size.cx   <   600)   size.cx   =   800;   

	return   size;   

}   


void CMMJieshiBar::OnDestroy() 
{
	CSizingControlBar::OnDestroy();
}


void CMMJieshiBar::UpdateShowJieShi(const char *xml,const char *node)
{
	if(!xml || !node)
	{
		m_edit.SetWindowText(NULL);
		return;
	}
	CString cap;
	GetXMLNodeValue(cap,xml,node);
	m_edit.SetWindowText(cap); 
}