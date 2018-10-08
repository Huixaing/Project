// MyTreeView.cpp : implementation file
//

#include "stdafx.h"
//#include "wintg.h"
#include "MyTreeView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMyTreeView

IMPLEMENT_DYNCREATE(CMyTreeView, CTreeView)

CMyTreeView::CMyTreeView()
{
}

CMyTreeView::~CMyTreeView()
{
}


BEGIN_MESSAGE_MAP(CMyTreeView, CTreeView)
	//{{AFX_MSG_MAP(CMyTreeView)
	ON_NOTIFY_REFLECT(TVN_SELCHANGED, OnSelchanged)
	ON_NOTIFY_REFLECT(NM_CLICK, OnClick)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMyTreeView drawing

void CMyTreeView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}

/////////////////////////////////////////////////////////////////////////////
// CMyTreeView diagnostics

#ifdef _DEBUG
void CMyTreeView::AssertValid() const
{
	CTreeView::AssertValid();
}

void CMyTreeView::Dump(CDumpContext& dc) const
{
	CTreeView::Dump(dc);
}
#endif //_DEBUG
CWintgDoc* CMyTreeView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CWintgDoc)));
	return (CWintgDoc*)m_pDocument;
}
/////////////////////////////////////////////////////////////////////////////
// CMyTreeView message handlers

void CMyTreeView::OnInitialUpdate() 
{
	CTreeView::OnInitialUpdate();
	
	CTreeCtrl& ctlTree = (CTreeCtrl&) GetTreeCtrl();

	ctlTree.DeleteAllItems();
	UpdateWindow();
  
	tSchedule = ctlTree.InsertItem ("中国",ctlTree.GetRootItem());
	tSchedule = ctlTree.InsertItem ("美国",ctlTree.GetRootItem());

//	tSchedule = ctlTree.GetRootItem().AddTail("计划列车",IID_DATABASE);
//	tSchedule = ctlTree.GetRootItem().AddTail("计划列车",IID_DATABASE);
 
//	tUpdir = tSchedule.AddTail(_T("上行"),IID_TABLES);
//	tDowndir = tSchedule.AddTail(_T("下行"),IID_RELATIONS);	
}

void CMyTreeView::OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	
	*pResult = 0;
}

BOOL CMyTreeView::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
 	return CTreeView::PreTranslateMessage(pMsg);
}

BOOL CMyTreeView::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CTreeView::OnNotify(wParam, lParam, pResult);
}

void CMyTreeView::OnClick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	
	*pResult = 0;
}
