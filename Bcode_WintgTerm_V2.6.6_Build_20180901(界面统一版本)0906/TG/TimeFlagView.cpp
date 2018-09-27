// CFView.cpp : implementation file
//

#include "stdafx.h"
#include "IF.h"
#include "TimeFlagView.h"
#include "tgform.h"
#include ".\timeflagview.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTimeFlagView

IMPLEMENT_DYNCREATE(CTimeFlagView, CView)

CTimeFlagView::CTimeFlagView()
{
	m_pDoc = NULL;
	m_pOldBitmap = NULL;
	m_pctrl = NULL;
}

CTimeFlagView::~CTimeFlagView()
{
}

BEGIN_MESSAGE_MAP(CTimeFlagView, CView)
	ON_WM_DESTROY()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTimeFlagView drawing


/////////////////////////////////////////////////////////////////////////////
// CTimeFlagView diagnostics

#ifdef _DEBUG
void CTimeFlagView::AssertValid() const
{
	CView::AssertValid();
}

void CTimeFlagView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CTimeFlagView message handlers
void CTimeFlagView::OnInitialUpdate() 
{
	CView::OnInitialUpdate();
	ASSERT(m_pDoc != NULL);

	m_fontTimeString.CreatePointFont(120,"System"); 
    m_pDoc->sysprint(0,0,"[CTimeFlagView] OnInitialUpdate");

	ReLayoutBmpPageSize();
	ShowTimeString(&m_dcMemoryDC,CLR_LAWNGREEN);
}


void CTimeFlagView::OnDraw(CDC* pDC)
{
	CRect rectWindow;
	this->GetClientRect(&rectWindow);
	
	pDC->BitBlt(rectWindow.left,rectWindow.top,
			rectWindow.Width(),rectWindow.Height(),
			&m_dcMemoryDC, m_pDoc->m_nHorizontalPos, 0, SRCCOPY);
}

void CTimeFlagView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	if(lHint == HORIZONTAL_POSITION_CHANGE)
	{
		Invalidate(TRUE);
	}
	else if(lHint == GRAPH_ROOM_CHANGE)
	{
		ReLayoutBmpPageSize();
		ShowTimeString(&m_dcMemoryDC,CLR_LAWNGREEN);
        
		Invalidate(TRUE);
	}
	else if(lHint == GRAPH_BEGIN_TIME_CHANGE)
	{
		CRect rect(0, 0, m_pDoc->m_nPageWidth+15,	VIEW_PORT::TIME_FLAG_VIEW_HEIGHT);
		m_dcMemoryDC.FillRect(&rect, &m_pDoc->m_brushWhite);
		
		m_dcMemoryDC.SetBkMode(TRANSPARENT);
		ShowTimeString(&m_dcMemoryDC,CLR_LAWNGREEN);
		Invalidate(TRUE);
	}
}

void CTimeFlagView::ShowTimeString(CDC * pDC, long lColor)
{
	if(TRUE==m_pDoc->m_nAutoMove)
	{
		ShowTimeStringAutoMove(pDC, lColor);
		return;
	}

	int nOldAlign = pDC->SetTextAlign(TA_LEFT | TA_TOP);

	char timestr[6];
	POINT ptlLeft;
	CSize textSize;
	ptlLeft.x = 0;

	UINT OldTextColor = pDC->SetTextColor(lColor);
	UINT nOldBackMode = pDC->SetBkMode(TRANSPARENT);
	CFont *pOldFont = pDC->SelectObject(&m_fontTimeString);	
		
	int baseHour  = m_pDoc->m_ptgctrl->GetBeginHour();
	int nWidth    = m_pDoc->m_nPageWidth -  2*X_TIME_OFFSET;
	int timeRange = m_pctrl->GetTimeLength();

	CPen mThinPen,mDashPen,mBoldPen,*pOldPen;

	mThinPen.CreatePen(PS_SOLID, 1, lColor);
	mDashPen.CreatePen(PS_DASH,  1, lColor);
	mBoldPen.CreatePen(PS_SOLID, 2, lColor);
	pOldPen = pDC->SelectObject(&mBoldPen);
	ptlLeft.x = X_TIME_OFFSET;
	ptlLeft.y = 25 ;

	pDC->MoveTo(ptlLeft.x,ptlLeft.y);
	pDC->LineTo(nWidth + ptlLeft.x,ptlLeft.y);

	for(int k=0; k<= timeRange; k++)
	{
		sprintf(timestr, "%d", (k+baseHour) % 24);

		textSize = pDC->GetTextExtent(timestr);

		if(k == 0)
		{
			pDC->TextOut(ptlLeft.x, 
			ptlLeft.y - 3*textSize.cy/2 + 3  , timestr);
		}
		else if(k == timeRange)
		{
			pDC->TextOut(ptlLeft.x - textSize.cx, 
			ptlLeft.y - 3*textSize.cy/2 + 3  , timestr);
		}
		else
		{
			pDC->TextOut(ptlLeft.x - textSize.cx/2 , 
			ptlLeft.y - 3*textSize.cy/2 + 3  , timestr);
		}
		int nDis = (int)m_pDoc->m_fPixelPerHour/6;

		for(int i = 0 ;i< 6;i++)
		{
			int nYOffset,nHorPos;

			nHorPos = ptlLeft.x + nDis*i ;

			if(i == 0) 
				nYOffset = 6;
			else if(i == 3)
				nYOffset = 4;
			else
				nYOffset = 3;
			pDC->MoveTo (ptlLeft.x + nDis*i  ,ptlLeft.y - nYOffset );
			if(nYOffset == 3)
			{
				pDC->SelectObject(pOldPen);
				pOldPen = pDC->SelectObject(&mThinPen);
				pDC->LineTo (nHorPos, ptlLeft.y );
			}
			else
			{
				pDC->SelectObject(pOldPen);
				pOldPen = pDC->SelectObject(&mBoldPen);
				pDC->LineTo (nHorPos , ptlLeft.y );
			}
		}

		ptlLeft.x += (long)m_pDoc->m_fPixelPerHour;
	}

	pDC->SetTextAlign(nOldAlign);
	pDC->SetTextColor(OldTextColor);
	pDC->SelectObject(pOldPen);
	pDC->SetBkMode(nOldBackMode);
	pDC->SelectObject(pOldFont);

	BOOL ret =mThinPen.DeleteObject();
	ret = mDashPen.DeleteObject();
	ret = mBoldPen.DeleteObject();
}

void CTimeFlagView::ShowTimeStringAutoMove(CDC * pDC, long lColor)
{
	int nOldAlign = pDC->SetTextAlign(TA_LEFT | TA_TOP);

	char timestr[6];
	POINT ptlLeft;
	CSize textSize;
	ptlLeft.x = 0;

	UINT OldTextColor = pDC->SetTextColor(lColor);
	UINT nOldBackMode = pDC->SetBkMode(TRANSPARENT);
	CFont *pOldFont = pDC->SelectObject(&m_fontTimeString);	
	
	int nWidth    = m_pDoc->m_nPageWidth -  2*X_TIME_OFFSET;
	int timeRange = m_pctrl->GetTimeLength();

	CPen mThinPen,mDashPen,mBoldPen,*pOldPen;

	mThinPen.CreatePen(PS_SOLID, 1, lColor);
	mDashPen.CreatePen(PS_DASH,  1, lColor);
	mBoldPen.CreatePen(PS_SOLID, 2, lColor);
	pOldPen = pDC->SelectObject(&mBoldPen);
	
	ptlLeft.y = 25;
	ptlLeft.x = X_TIME_OFFSET ;

	pDC->MoveTo(ptlLeft.x,ptlLeft.y);
	pDC->LineTo(nWidth + ptlLeft.x,ptlLeft.y);

	int nYOffset=0;
	CTime tm(m_pDoc->GetBeginTime());
	int nMiniute  = tm.GetMinute();
	int nHour     = tm.GetHour();
	
	for(int k=0; k<= timeRange*60; k++)
	{
		if(nMiniute%10==0)
		{
			if(nMiniute%60 == 0) 
				nYOffset = 6;
			else if(nMiniute%30 == 0)
				nYOffset = 4;
			else
				nYOffset = 3;
			pDC->MoveTo (ptlLeft.x, ptlLeft.y - nYOffset);
			if(nYOffset == 3)
			{
				pDC->SelectObject(pOldPen);
				pOldPen = pDC->SelectObject(&mThinPen);
				pDC->LineTo (ptlLeft.x, ptlLeft.y );
			}
			else
			{
				pDC->SelectObject(pOldPen);
				pOldPen = pDC->SelectObject(&mBoldPen);
				pDC->LineTo (ptlLeft.x , ptlLeft.y );
			}
		}
		
		if(nMiniute%60==0)
		{
			sprintf(timestr, "%d", (nHour+nMiniute/60)%24);
		
			textSize = pDC->GetTextExtent(timestr);

			if(k == 0)
			{
				pDC->TextOut(ptlLeft.x, 
				ptlLeft.y - 3*textSize.cy/2 + 3  , timestr);
			}
			else if(k == timeRange*6)
			{
				pDC->TextOut(ptlLeft.x - textSize.cx, 
				ptlLeft.y - 3*textSize.cy/2 + 3  , timestr);
			}
			else
			{
				pDC->TextOut(ptlLeft.x - textSize.cx/2 , 
				ptlLeft.y - 3*textSize.cy/2 + 3  , timestr);
			}
		}

		
		nMiniute += 1;

		ptlLeft.x += m_pDoc->m_fPixelPerMinute;
	}
	
	pDC->SetTextAlign(nOldAlign);
	pDC->SetTextColor(OldTextColor);
	pDC->SelectObject(pOldPen);
	pDC->SetBkMode(nOldBackMode);
	pDC->SelectObject(pOldFont);

	BOOL ret =mThinPen.DeleteObject();
	ret = mDashPen.DeleteObject();
	ret = mBoldPen.DeleteObject();
}

void CTimeFlagView::OnDestroy()
{
	CView::OnDestroy();
 
	// TODO: 在此处添加消息处理程序代码
	if(m_pOldBitmap != NULL)
	{
		m_dcMemoryDC.SelectObject(m_pOldBitmap);
		m_bmpMemBitmap.DeleteObject();
		m_dcMemoryDC.DeleteDC();
		m_pOldBitmap = NULL;
	}
	m_fontTimeString.DeleteObject();
}

void CTimeFlagView::OnMouseMove(UINT nFlags, CPoint point) 
{	
	m_pDoc->UpdateAllViews(NULL, MSG_MOUSE_MOVE_OUTSIDE, 0);
	CView::OnMouseMove(nFlags, point);
}

void CTimeFlagView::ReLayoutBmpPageSize()
{
	if(m_pOldBitmap != NULL)
	{
		m_dcMemoryDC.SelectObject(m_pOldBitmap);
		m_bmpMemBitmap.DeleteObject();
		m_dcMemoryDC.DeleteDC();
		m_pOldBitmap = NULL;
	}

	CClientDC dc(this);
	dc.SetMapMode(MM_TEXT);

	BITMAPINFOHEADER bmih;
	memset(&bmih,0,sizeof(BITMAPINFOHEADER));
	bmih.biSize = sizeof(BITMAPINFOHEADER);
	DEVMODE md;
	md.dmSize = sizeof(md);
	if (EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &md))
		bmih.biBitCount = md.dmBitsPerPel;
	else
		bmih.biBitCount =16;
	bmih.biCompression=BI_RGB;
	bmih.biPlanes=1;
	bmih.biWidth = m_pDoc->m_nPageWidth+15;
	bmih.biHeight= VIEW_PORT::TIME_FLAG_VIEW_HEIGHT;
	BITMAPINFO bmi;
	memset(&bmi,0,sizeof(BITMAPINFO));
	bmi.bmiHeader = bmih;
	void *p;
	HBITMAP bitMap = ::CreateDIBSection(dc.m_hDC, &bmi, DIB_RGB_COLORS, &p, NULL, 0);
	if(!bitMap)
	{
		m_pDoc->sysprint(0,0,"[TG]: create memory bitmap error");				
		AfxMessageBox("创建时间标视图位图失败，请重启计算机后登陆！");
		return;
	}
	m_bmpMemBitmap.Attach(bitMap);
	if(m_dcMemoryDC.CreateCompatibleDC(&dc) == 0)
	{
		m_bmpMemBitmap.DeleteObject();
		AfxMessageBox("创建时间标视图内存兼容dc失败，请重启计算机后登陆！");
		m_pDoc->sysprint(0,0,"[TG]: create  memory DC error");
		return;
	}

	m_pOldBitmap = m_dcMemoryDC.SelectObject(&m_bmpMemBitmap);

	m_dcMemoryDC.SetBkMode(TRANSPARENT);

	CRect rect(0, 0, m_pDoc->m_nPageWidth+15,	VIEW_PORT::TIME_FLAG_VIEW_HEIGHT);
	m_dcMemoryDC.FillRect(&rect, &m_pDoc->m_brushWhite);
}