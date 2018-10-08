// CMMTrainInfoBar.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include <io.h>
#include <afxinet.h>
#include "mmtrnbar.h"
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
// CMMTrainInfoBar

#define  IDC_PLAN_NOTE_GRID  5121

CMMTrainInfoBar::CMMTrainInfoBar()
{
}

CMMTrainInfoBar::~CMMTrainInfoBar()
{
}

BEGIN_MESSAGE_MAP(CMMTrainInfoBar, CSizingControlBar)
	//{{AFX_MSG_MAP(CMMTrainInfoBar)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMMTrainInfoBar message handlers


int CMMTrainInfoBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CSizingControlBar::OnCreate(lpCreateStruct) == -1)
		return -1;
	CRect rect(0,0,0,0);
	m_trncap.Create("施工路用车信息",WS_CHILD|WS_VISIBLE|SS_CENTER,rect,this);//SS_GRAYFRAME
	m_trngrid.CreateGrid(this,rect,IDC_PLAN_TRN_GRID,g_mplan_trninfo_def,count_of_mplan_trninfo_def());
	m_trngrid.SetEditable(FALSE);
	return 0;
}

void CMMTrainInfoBar::OnSize(UINT nType, int cx, int cy) 
{
	//m_edit.MoveWindow(0,0,cx,cy);

	m_trncap.MoveWindow(0,0,cx,20);
	m_trncap.Invalidate();
	m_trngrid.MoveWindow(0,20,cx,cy-20);
	m_trngrid.ExpandColumnsToFit(FALSE);
	m_trngrid.ExpandRowsToFit(FALSE); 

	CSizingControlBar::OnSize(nType, cx, cy);
}

CSize CMMTrainInfoBar::CalcDynamicLayout(int   nLength,   DWORD   nMode)   
  {   
  CSize   size;   
  size   =   CSizingControlBar::CalcDynamicLayout(nLength,   nMode);   
  //if(size.cx   <   800)   size.cx   =   800;   
  return   size;   
    
  }   


void CMMTrainInfoBar::OnDestroy() 
{
	CSizingControlBar::OnDestroy();
}

void CMMTrainInfoBar::UpdateShow(const char *xml,const char *node)
{
	
}

void CMMTrainInfoBar::UpdateShowTrnInfo(const char *xml,const char *node)
{
	m_trngrid.DeleteNonFixedRows();
	if(!xml || !node)
	{
		m_trngrid.Refresh(); 
		return;
	}
	CString subnode;
	subnode.Format("%s/RECORD",node); 
	m_trngrid.InsertRows(xml,subnode);
	m_trngrid.ExpandColumnsToFit(); 
	m_trngrid.Refresh();
}

