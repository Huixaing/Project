// CMMDcmdBar.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include <io.h>
#include <afxinet.h>
#include "mmdcmdbar.h"
#include "TGPspData.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#define IDC_MMBAR_EDIT 5111
/////////////////////////////////////////////////////////////////////////////
// CMMDcmdBar

#define  IDC_PLAN_DCMD_GRID  5122

CMMDcmdBar::CMMDcmdBar()
{
}

CMMDcmdBar::~CMMDcmdBar()
{
}

BEGIN_MESSAGE_MAP(CMMDcmdBar, CSizingControlBar)
	//{{AFX_MSG_MAP(CMMDcmdBar)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_NOTIFY(GVN_SELCHANGING,IDC_PLAN_DCMD_GRID,OnGridSelectChange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMMDcmdBar message handlers


int CMMDcmdBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CSizingControlBar::OnCreate(lpCreateStruct) == -1)
		return -1;
	CRect rect(0,0,0,0);
	m_edit.Create(WS_CHILD|WS_VISIBLE|ES_MULTILINE|ES_AUTOVSCROLL|ES_READONLY|WS_BORDER,rect,this,IDC_MMBAR_EDIT);
	m_grid.CreateGrid(this,rect,IDC_PLAN_DCMD_GRID,g_dcmd_viewlist_def,count_of_viewlist_def());
	m_caption.Create("施工调度命令",WS_CHILD|WS_VISIBLE|SS_CENTER,rect,this);//SS_GRAYFRAME

	return 0;
}

void CMMDcmdBar::OnSize(UINT nType, int cx, int cy) 
{
	m_caption.MoveWindow(0,0,cx,20);
	m_caption.Invalidate();
	m_grid.MoveWindow(0,20,cx*2/3,cy-20);
	m_grid.ExpandColumnsToFit(FALSE);
	m_edit.MoveWindow(cx*2/3,20,cx/3,cy-20);

	m_grid.SetColumnWidth(0,cx*2/12);
	m_grid.SetColumnWidth(1,cx*2/12);
	m_grid.SetColumnWidth(2,cx*2/24);
	m_grid.SetColumnWidth(3,cx*2/8);
	m_grid.SetColumnWidth(4,cx*2/48);
	m_grid.SetEditable(TRUE);
	
	CSizingControlBar::OnSize(nType, cx, cy);
	
}

void CMMDcmdBar::OnDestroy() 
{
	CSizingControlBar::OnDestroy();
}
BOOL IsDCMDSaved(const char *planid,const char *cmdindex);

void CMMDcmdBar::UpdateShow(const char *xml,const char *rootnode)
{
	//m_edit.SetWindowText(str);
	if(!xml)
	{
		m_grid.DeleteNonFixedRows();
		m_grid.Refresh();
		m_edit.SetWindowText("");//2010.3.20
		return;
	}
	m_grid.InsertRows(xml,rootnode); 
	for(int i=1;i<m_grid.GetRowCount();i++)
	{
		//m_grid.SetRowHeight(i,40);
		int col=m_grid.GetColumnByID(ID_PD_MPLAN_DCMD_INDEX);
		if(col>=0)
		{
			CString cmdindex=m_grid.GetCell(i,col)->GetText();
			if(IsDCMDSaved(NULL,cmdindex))
				m_grid.GetCell(i,col)->SetBackClr(RGB(0,255,0));
		}
	}
	m_grid.ExpandColumnsToFit(FALSE);
}

void CMMDcmdBar::OnGridSelectChange(NMHDR* pNMHDR, LRESULT* pResult) 
 {
	 NM_GRIDVIEW *pv=(NM_GRIDVIEW *)pNMHDR;
	 if(pv->iRow>0)
	 {
			int col=m_grid.GetColumnByID(ID_PD_MPLAN_DCMD_CONTENT);
			if(col>=0)
			{
				m_edit.SetWindowText(m_grid.GetCell(pv->iRow,col)->GetText());
			}
	 }
 }

