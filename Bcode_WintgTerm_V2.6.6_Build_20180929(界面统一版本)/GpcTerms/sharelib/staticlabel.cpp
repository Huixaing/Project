// StaticLabel.cpp : implementation file
//

#include "stdafx.h"
#include "StaticLabel.h"
#include "messages.h"
#include "clrdef.h"
#ifdef STPC_SYSTEM
#include <clbth.h>
#endif
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CStaticLabel
static int nStaticLabelFlag = 0;
static CString m_strStaticLabelClassName;

static void Draw3DRect(CDC * pCDC, CRect &rect, COLORREF clrTopLeft, COLORREF clrBottomRight,int nWidth)
{
#ifndef DRAW_WIDEN_LINE_BY_POINT	
	//method of drawing width line 
	CPen  mPen,*pOldPen;
	int nOldMode;

	if(nWidth > 6)
		nWidth = 6;
	mPen.CreatePen(PS_INSIDEFRAME,nWidth,clrTopLeft);
	pOldPen = pCDC->SelectObject(&mPen);
	nOldMode = pCDC->SetROP2(R2_COPYPEN);
	pCDC->MoveTo(rect.left,rect.bottom);
	pCDC->LineTo(rect.left,rect.top);// left line
	pCDC->LineTo(rect.right,rect.top);// top line
	pCDC->SelectObject(pOldPen);
	mPen.DeleteObject( );
	mPen.CreatePen(PS_INSIDEFRAME,nWidth,clrBottomRight);
	pOldPen = pCDC->SelectObject(&mPen);
	pCDC->MoveTo(rect.right,rect.top);
	pCDC->LineTo(rect.right,rect.bottom);// right line
	pCDC->LineTo(rect.left,rect.bottom);// bottom line
	pCDC->SelectObject(pOldPen);
	mPen.DeleteObject( );
	pCDC->SetROP2(nOldMode);
#else
	// method of drawing color point
	register int i,j,count;
	if(nWidth > 6)
		nWidth = 6;
	if(nWidth / 2 == 1)
		nWidth = nWidth / 2 + 1;
	else nWidth /= 2;
	// drawing top line
	count = 0;
	for(j = rect.top - nWidth;j <= rect.top + nWidth;j++) 
	{
		for(i = rect.left - nWidth;i <= rect.right + nWidth - count;i++)
			pCDC->SetPixel(i,j,clrTopLeft);
		count += 1;
	}
	// drawing left line
	count = 0;
	for(i = rect.left - nWidth;i <= rect.left + nWidth;i++)
	{
		for(j = rect.top + nWidth;j <= rect.bottom + nWidth - count;j++)
			pCDC->SetPixel(i,j,clrTopLeft);
		count += 1;
	}
	// drawing bottom line
	count = 0;
	for(j = rect.bottom + nWidth;j >= rect.bottom - nWidth;j--) 
	{
		for(i = rect.left - nWidth + count;i <= rect.right + nWidth;i++)
			pCDC->SetPixel(i,j,clrBottomRight);
		count += 1;
	}
	// drawing right line
	count = 0;
	for(i = rect.right + nWidth;i >= rect.right - nWidth;i--)
	{
		for(j = rect.top - nWidth + count;j <= rect.bottom - nWidth;j++)
			pCDC->SetPixel(i,j,clrBottomRight);
		count += 1;
	}
#endif	
}

static void Draw3DRect(CDC * pCDC, CRect &rect, int nWidth, int nFlag)
{
#ifndef DRAW_WIDEN_LINE_BY_POINT	
// method of drawing width line
	CPen  mPen,*pOldPen;
	int nOldMode;

	if(nFlag)
	{// up mode button
		mPen.CreatePen(PS_INSIDEFRAME,nWidth,CLR_PALEGRAY);
		pOldPen = pCDC->SelectObject(&mPen);
		nOldMode = pCDC->SetROP2(R2_COPYPEN);
		pCDC->MoveTo(rect.left,rect.bottom);
		pCDC->LineTo(rect.left,rect.top);// left line
		pCDC->LineTo(rect.right,rect.top);// top line
		pCDC->SelectObject(pOldPen);
		mPen.DeleteObject( );
		mPen.CreatePen(PS_INSIDEFRAME,nWidth,CLR_GRAY);
		pOldPen = pCDC->SelectObject(&mPen);
		pCDC->MoveTo(rect.right,rect.top);
		pCDC->LineTo(rect.right,rect.bottom);// right line
		pCDC->LineTo(rect.left,rect.bottom);// bottom line
	} else {// down mode button
		mPen.CreatePen(PS_INSIDEFRAME,nWidth,CLR_GRAY);
		pOldPen = pCDC->SelectObject(&mPen);
		nOldMode = pCDC->SetROP2(R2_COPYPEN);
		pCDC->MoveTo(rect.left,rect.bottom);
		pCDC->LineTo(rect.left,rect.top);
		pCDC->LineTo(rect.right,rect.top);
		pCDC->SelectObject(pOldPen);
		mPen.DeleteObject( );
		mPen.CreatePen(PS_INSIDEFRAME,nWidth,CLR_PALEGRAY);
		pOldPen = pCDC->SelectObject(&mPen);
		pCDC->MoveTo(rect.right,rect.top);
		pCDC->LineTo(rect.right,rect.bottom);
		pCDC->LineTo(rect.left,rect.bottom);
	}
	pCDC->SelectObject(pOldPen);
	mPen.DeleteObject( );
	pCDC->SetROP2(nOldMode);
#else
	// method of drawing color point
	register int i,j,count;
	COLORREF clrTopLeft,clrBottomRight;
	if(nFlag)
	{// draw up mode 3Drect
		clrTopLeft = CLR_PALEGRAY;
		clrBottomRight = CLR_GRAY;
	} else {// draw down mode 3Drect
		clrTopLeft = CLR_GRAY;
		clrBottomRight = CLR_PALEGRAY;
	}
	if(nWidth > 6)
		nWidth = 6;
	if(nWidth / 2 == 1)
		nWidth = nWidth / 2 + 1;
	else nWidth /= 2;
	// drawing top line
	count = 0;
	for(j = rect.top - nWidth;j <= rect.top + nWidth;j++) 
	{
		for(i = rect.left - nWidth;i <= rect.right + nWidth - count;i++)
			pCDC->SetPixel(i,j,clrTopLeft);
		count += 1;
	}
	// drawing left line
	count = 0;
	for(i = rect.left - nWidth;i <= rect.left + nWidth;i++)
	{
		for(j = rect.top + nWidth;j <= rect.bottom + nWidth - count;j++)
			pCDC->SetPixel(i,j,clrTopLeft);
		count += 1;
	}
	// drawing bottom line
	count = 0;
	for(j = rect.bottom + nWidth;j >= rect.bottom - nWidth;j--) 
	{
		for(i = rect.left - nWidth + count;i <= rect.right + nWidth;i++)
			pCDC->SetPixel(i,j,clrBottomRight);
		count += 1;
	}
	// drawing right line
	count = 0;
	for(i = rect.right + nWidth;i >= rect.right - nWidth;i--)
	{
		for(j = rect.top - nWidth + count;j <= rect.bottom - nWidth;j++)
			pCDC->SetPixel(i,j,clrBottomRight);
		count += 1;
	}
#endif	
}

CStaticLabel::CStaticLabel()
{
	m_clrText = ::GetSysColor(COLOR_WINDOWTEXT);
	m_clrTextBk = CLR_PALEGRAY;
	m_clrBkgnd = CLR_DARKGRAY;
	
	m_font.CreatePointFont(120,"创艺简魏碑");
	
	m_brushRed.CreateSolidBrush(CLR_RED);
	m_brushNormal.CreateSolidBrush(m_clrTextBk);
	m_nStatus = STATUS_ACTIVE;// dead status
	m_nTextStyle = DT_LEFT;
	m_nLabel3DStyle = UP_STYLE;
}

CStaticLabel::~CStaticLabel()
{

}


BEGIN_MESSAGE_MAP(CStaticLabel, CWnd)
	//{{AFX_MSG_MAP(CStaticLabel)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CStaticLabel message handlers

void CStaticLabel::ResetStatus(int nStatus)
{
	m_nStatus = nStatus;
	Invalidate(TRUE);
}

void CStaticLabel::SetStatus(int nStatus)
{
	if(m_nStatus != nStatus)
	{
		if(nStatus != 0 && nStatus != 1)
			Sleep(0);
		m_nStatus = nStatus;
		Invalidate(TRUE);
	}
}

int CStaticLabel::GetStatus()
{
	return m_nStatus;
}

void CStaticLabel::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
 

	CString text;
	COLORREF clrTextSave;
	CFont *pOldFont;
	CRect rect,rcLabel;

	GetClientRect(&rect);
	GetWindowText(text);
	rcLabel.left = rect.left + 2;
	rcLabel.right = rect.right - 2;
	rcLabel.top = rect.top + 2;
	rcLabel.bottom = rect.bottom - 2; 

	pOldFont = dc.SelectObject(&m_font);
	int nOldBackMode;
	if(m_nLabel3DStyle == UP_STYLE)
		Draw3DRect(&dc,rect,CLR_WHITE,m_clrBkgnd,4);
	else if(m_nLabel3DStyle == DOWN_STYLE)
		Draw3DRect(&dc,rect,m_clrBkgnd,CLR_WHITE,4);
	else 
		rcLabel = rect;
	
	if(m_nStatus == STATUS_ACTIVE)
	{
		dc.FillRect(rcLabel,&m_brushNormal);
		clrTextSave = dc.SetTextColor(m_clrText);
		nOldBackMode = dc.SetBkMode(TRANSPARENT);
		dc.DrawText(text,rcLabel,m_nTextStyle | DT_SINGLELINE | DT_VCENTER);
	}else if(m_nStatus == STATUS_DEAD) {
		dc.FillRect(rcLabel,&m_brushNormal);
		clrTextSave = dc.SetTextColor(CLR_RED);
		nOldBackMode = dc.SetBkMode(TRANSPARENT);
		dc.DrawText(text,rcLabel,m_nTextStyle | DT_SINGLELINE | DT_VCENTER);
	} else {

		dc.FillRect(rcLabel,&m_brushRed);
		clrTextSave = dc.SetTextColor(CLR_GRAY);//CLR_BLACK
		nOldBackMode = dc.SetBkMode(TRANSPARENT);
		dc.DrawText(text,rcLabel,m_nTextStyle | DT_SINGLELINE | DT_VCENTER);
	}
	// TODO: Add your message handler code here
    dc.SetTextColor(clrTextSave);
	dc.SelectObject(pOldFont);
	dc.SetBkMode(nOldBackMode);
}


BOOL CStaticLabel::Create(LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	if(nStaticLabelFlag == 0)
	{
		m_strStaticLabelClassName = AfxRegisterWndClass(CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW,
			AfxGetApp()->LoadStandardCursor(IDC_ARROW), (HBRUSH)(COLOR_WINDOW+1),
			NULL);
		nStaticLabelFlag = 1;
	}

	dwStyle = WS_VISIBLE | WS_CHILD;

	if(! CWnd::CreateEx(0,
		(LPCTSTR)m_strStaticLabelClassName,lpszWindowName,dwStyle,
		rect,pParentWnd,nID))
		return FALSE;
	return TRUE;
}

void CStaticLabel::SetFont(int nPoint, LPCTSTR sFontName)
{
	m_font.DeleteObject( );

	if(sFontName == NULL)
		m_font.CreatePointFont(nPoint,"创艺简魏碑");
	else 
		m_font.CreatePointFont(nPoint,sFontName);
	Invalidate(TRUE);
}

void CStaticLabel::SetFont(LOGFONT* lpLogFont)
{
	m_font.DeleteObject( );

	if(lpLogFont == NULL)
		m_font.CreatePointFont(120,"创艺简魏碑");
	else 
		m_font.CreateFontIndirect(lpLogFont);
	Invalidate(TRUE);
}

void CStaticLabel::SetTextStyle(int nStyle)
{
	if(m_nTextStyle != nStyle)
	{
		m_nTextStyle = nStyle;
		Invalidate(TRUE);
	}
}

void CStaticLabel::SetLabel3DStyle(int n3DStyle)
{
	if(m_nLabel3DStyle != n3DStyle)
	{
		m_nLabel3DStyle = n3DStyle;
		Invalidate( );
	}
}

void CStaticLabel::SetWindowText(LPCTSTR text,int nFlag)
{
	CWnd::SetWindowText(text);
	if(nFlag)
		Invalidate(TRUE);
}

BOOL CStaticLabel::OnEraseBkgnd(CDC* pCDC) 
{
	// TODO: Add your message handler code here and/or call default
	CRect ClipRect;
	pCDC->GetClipBox(&ClipRect);
	if(m_nStatus == STATUS_ACTIVE || m_nStatus == STATUS_DEAD)
		pCDC->FillRect(&ClipRect,&m_brushNormal);
	else 
		pCDC->FillRect(&ClipRect,&m_brushRed);
	return TRUE;	
}
