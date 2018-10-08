// ConnectStatus.cpp : implementation file
//

#include "stdafx.h"
#include "Logo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
CLogo::CLogo()
{
	flag = true;
}

CLogo::~CLogo()
{
}

BEGIN_MESSAGE_MAP(CLogo, CWnd)
	//{{AFX_MSG_MAP(CLogo)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CLogo::SetPicSize(const CRect& rt, UINT id, bool bSize)
{
	this->rect.top = 0;
	this->rect.left = 0;
	this->rect.right = rt.Width();
	this->rect.bottom = rt.Height();
	this->bRealSize = bSize;
	this->bResourceId = id;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CLogo::Create(LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID) 
{
	if(flag)
	{
		strClassName = AfxRegisterWndClass(CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW,
			AfxGetApp()->LoadStandardCursor(IDC_ARROW), (HBRUSH)(COLOR_WINDOW+1),
			NULL);
		flag = false;
	}
	dwStyle = WS_VISIBLE | WS_CHILD;

	if(! CWnd::CreateEx(0,
		(LPCTSTR)strClassName,lpszWindowName,dwStyle,
		rect,pParentWnd,nID))
		return FALSE;
	return TRUE;
}

BOOL CLogo::OnEraseBkgnd(CDC* pCDC) 
{
	return TRUE;
}

void CLogo::OnDestroy() 
{
	CWnd::OnDestroy();
}

void CLogo::OnPaint() 
{
	CPaintDC dc(this); 
	DrawBitmapFile(&dc, 0, 0, rect.Width(), rect.Height());     
}

void CLogo::DrawBitmapFile(CPaintDC *pCDC,int left,int top,int w,int h)
{
	BITMAP bm;
	int iWidth,iHeight;
	CBitmap *pOldBitmap;
	CBitmap  bmp;
	CDC hdcMem;

	if(!hdcMem.CreateCompatibleDC(pCDC))
		return;

	if(!bmp.LoadBitmap(bResourceId))
	{
		hdcMem.DeleteDC();
		return;
	}
	pOldBitmap = (CBitmap *)hdcMem.SelectObject(&bmp);
	bmp.GetBitmap(&bm);
	iWidth = bm.bmWidth;
	iHeight = bm.bmHeight;

	if(h*iWidth < w*iHeight)
	{
		w = (iWidth*h)/iHeight;
	}
	else
	{
		h = (iHeight*w)/iWidth;
	}
	if (bRealSize)
	{
		w = iWidth;
		h = iHeight;
	}
	pCDC->FillSolidRect(left,top,w,h,0x00ffffff);
	pCDC->TransparentBlt(left,top,
		w,h,
		&hdcMem,
		0, 0,
		iWidth, iHeight,
		RGB(255,255,255));
	hdcMem.SelectObject(pOldBitmap);
	hdcMem.DeleteDC( );
}
