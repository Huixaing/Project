// MyToolBar.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "MyToolBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMyToolBar

CMyToolBar::CMyToolBar()
{
	bSize = false;
	m_Brush.CreateSolidBrush(RGB(255,0,0));
}

CMyToolBar::~CMyToolBar()
{
}


BEGIN_MESSAGE_MAP(CMyToolBar, CToolBar)
	//{{AFX_MSG_MAP(CMyToolBar)
	ON_WM_CTLCOLOR()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMyToolBar message handlers
HBRUSH CMyToolBar::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CToolBar::OnCtlColor(pDC, pWnd, nCtlColor);
	return hbr;
}

BOOL CMyToolBar::CreateLogo(CString fileName)
{
	CRect  wndRect;
	GetParentFrame()->GetWindowRect(&wndRect);
	int nScreenX = wndRect.right;
	
	// 459:128 = x:40
	CRect rect(nScreenX - 139,0, nScreenX + 10,36);
    m_cascoLogo.SetPicSize(rect, IDB_BITMAP_CASCO);
	if( !m_cascoLogo.Create("logo", WS_VISIBLE|SS_RIGHT, rect, this, -1) )
		return FALSE;
    bSize = true;
	return TRUE;
}

void CMyToolBar::OnSize(UINT nType, int cx, int cy)
{
	if(bSize)
	{
		CRect rect(cx-139, 0,  cx + 10, 36);
		m_cascoLogo.SetWindowPos(NULL, rect.left, rect.top, 0, 0, SWP_NOSIZE);
	}
}
