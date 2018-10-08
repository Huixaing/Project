// LockSplitter.cpp : implementation file
//

#include "stdafx.h"
#include "LockSplitter.h"
#include "tgForm.h"
#include ".\locksplitter.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLockSplitter

CLockSplitter::CLockSplitter(BOOL bFlag)
{
	m_bLocked = bFlag;
	m_bDrawFlag = 0;
	
}

CLockSplitter::~CLockSplitter()
{
}


BEGIN_MESSAGE_MAP(CLockSplitter, CSplitterWnd)
	//{{AFX_MSG_MAP(CLockSplitter)
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_SETCURSOR()
 	//}}AFX_MSG_MAP
	ON_WM_MOUSEWHEEL()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CLockSplitter message handlers

void CLockSplitter::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	if(!m_bLocked)
		CSplitterWnd::OnLButtonDown(nFlags, point);
}

void CLockSplitter::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	if(!m_bLocked)
		CSplitterWnd::OnMouseMove(nFlags, point);
	else 
		CWnd::OnMouseMove(nFlags,point);
}

BOOL CLockSplitter::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	// TODO: Add your message handler code here and/or call default
	if(!m_bLocked)
		return CSplitterWnd::OnSetCursor(pWnd, nHitTest, message);
	else
		return CWnd::OnSetCursor(pWnd,nHitTest,message);
}

void CLockSplitter::SetLockFlag(BOOL bLockFlag)
{
	m_bLocked = bLockFlag;	
}


void CLockSplitter::SetDrawFlag(int nDrawFlag)
{
	m_bDrawFlag = nDrawFlag;
}
  
void CLockSplitter::OnDrawSplitter( CDC* pDC, ESplitType nType, const CRect& rect )
{
	if(m_bDrawFlag)
		CSplitterWnd::OnDrawSplitter(pDC,nType,rect);

	if (pDC == NULL)
	{
		RedrawWindow(rect, NULL, RDW_INVALIDATE|RDW_NOCHILDREN);
		return;
	}
	
	pDC->FillRect(rect, &CBrush(RGB(255,255,255)));	
} 

BOOL CLockSplitter::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	int row = 1;
	int col = 1;

	BOOL bHasVert = FALSE;
	CWnd* pPane = GetPane(row, col);
	CTGForm* pView = DYNAMIC_DOWNCAST(CTGForm, pPane);
	if (pView != NULL)
	{
		pView->MouseWheel(nFlags, zDelta, pt);
	}

	return TRUE;
}
