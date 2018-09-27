// BlankView.cpp : implementation file
//

#include "stdafx.h"
#include "BlankView.h"
#include ".\blankview.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
 
/////////////////////////////////////////////////////////////////////////////
// CBlankView

IMPLEMENT_DYNCREATE(CBlankView, CView)

CBlankView::CBlankView()
{
}

CBlankView::~CBlankView()
{
}


BEGIN_MESSAGE_MAP(CBlankView, CView)
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_WM_ERASEBKGND()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBlankView drawing

void CBlankView::OnDraw(CDC* pDC)
{
	int wide=0;
	if (m_pctrl->mConfigTg.m_context.IsWideStationView())
		wide=VIEW_PORT::STATION_VIEW_WIDTH2;
	else
		wide=VIEW_PORT::STATION_VIEW_WIDTH1;

	pDC->BitBlt(0,0, wide, VIEW_PORT::TIME_FLAG_VIEW_HEIGHT,
		&m_dcMemoryDC, 0, 0, SRCCOPY);
}

/////////////////////////////////////////////////////////////////////////////
// CBlankView diagnostics

#ifdef _DEBUG
void CBlankView::AssertValid() const
{
	CView::AssertValid();
}

void CBlankView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CBlankView message handlers

void CBlankView::OnInitialUpdate() 
{
	CView::OnInitialUpdate();
	m_fontTime.CreateFont(
				28,                        // nHeight
				0,                         // nWidth
				0,                         // nEscapement
				0,                         // nOrientation
				FW_BOLD,                 // nWeight
				FALSE,                     // bItalic
				FALSE,                     // bUnderline
				0,                         // cStrikeOut
				ANSI_CHARSET,              // nCharSet
				OUT_DEFAULT_PRECIS,        // nOutPrecision
				CLIP_DEFAULT_PRECIS,       // nClipPrecision
				DEFAULT_QUALITY,           // nQuality
				DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
				"Times New Roman");                 // lpszFacename

	m_fontDate.CreateFont(
				15,                        // nHeight
				0,                         // nWidth
				0,                         // nEscapement
				0,                         // nOrientation
				FW_BOLD,                 // nWeight
				FALSE,                     // bItalic
				FALSE,                     // bUnderline
				0,                         // cStrikeOut
				ANSI_CHARSET,              // nCharSet
				OUT_DEFAULT_PRECIS,        // nOutPrecision
				CLIP_DEFAULT_PRECIS,       // nClipPrecision
				DEFAULT_QUALITY,           // nQuality
				DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
				"Times New Roman");                 // lpszFacename

	int wide=0;
	if (m_pctrl->mConfigTg.m_context.IsWideStationView())
		wide=VIEW_PORT::STATION_VIEW_WIDTH2;
	else
		wide=VIEW_PORT::STATION_VIEW_WIDTH1;

	CClientDC dc(this);
	if(m_bmpMemBitmap.CreateCompatibleBitmap(&dc,wide, VIEW_PORT::TIME_FLAG_VIEW_HEIGHT) == 0)
	{
		AfxMessageBox("创建视图位图失败");
		return;
	}

	if(m_dcMemoryDC.CreateCompatibleDC(&dc) == 0)
	{
		m_bmpMemBitmap.DeleteObject();
		AfxMessageBox("创建兼容内存dc失败，请重启计算机后登陆！");
		return;
	}

	m_pOldBitmap = m_dcMemoryDC.SelectObject(&m_bmpMemBitmap);

	m_dcMemoryDC.SetBkMode(TRANSPARENT);
		
	SetTimeString();
			
	SetTimer(TIMER_IDEVENT_SYSTEMTIME,1000,NULL);
}

void CBlankView::SetTimeString()
{
	CTime t_cur = CTime::GetCurrentTime();
	
	CString strText;
	strText.Format("%02d:%02d:%02d" ,
		t_cur.GetHour(),t_cur.GetMinute(),t_cur.GetSecond());

	int wide=0;
	if (m_pctrl->mConfigTg.m_context.IsWideStationView())
		wide=VIEW_PORT::STATION_VIEW_WIDTH2;
	else
		wide=VIEW_PORT::STATION_VIEW_WIDTH1;

	CRect rect(0, 0, wide,	VIEW_PORT::TIME_FLAG_VIEW_HEIGHT);
	m_dcMemoryDC.FillRect(&rect, &m_pDoc->m_brushWhite);

	COLORREF nOldClr = m_dcMemoryDC.SetTextColor(RGB(255,0,0));
	UINT   nOldAlign =  m_dcMemoryDC.SetTextAlign(TA_LEFT);
	CFont* pOldFonts =  m_dcMemoryDC.SelectObject(&m_fontDate);
	int nOldBackMode =  m_dcMemoryDC.SetBkMode(TRANSPARENT);

	if(m_pDoc->GetTgType()!=TG_TYPE::BASIC_TG)
	{
		CString str;
		str.Format("%ld", m_pDoc->GetShiftId());
		CString tmp = str.Left(4);
		str = str.Right(str.GetLength() - 4);
		tmp += "年"+str.Left(2);
		str = str.Right(str.GetLength() - 2);
		tmp += "月"+str.Left(2);
		str = str.Right(str.GetLength() - 2);
		tmp += "日第"+str.Left(2)+"班";
		m_dcMemoryDC.TextOut(0, 0,tmp);
	}
	m_dcMemoryDC.SetTextAlign(TA_CENTER);
	m_dcMemoryDC.SelectObject(&m_fontTime);

	m_dcMemoryDC.TextOut(wide/2, VIEW_PORT::TIME_FLAG_VIEW_HEIGHT/2-5, strText);

	m_dcMemoryDC.SetBkMode(nOldBackMode);
	m_dcMemoryDC.SetTextColor(nOldClr);
	m_dcMemoryDC.SelectObject(pOldFonts);  
	m_dcMemoryDC.SetTextAlign(nOldAlign);

	Invalidate(TRUE);   
}
void CBlankView::OnTimer(UINT nIDEvent) 
{
   if(nIDEvent == TIMER_IDEVENT_SYSTEMTIME )
   {
		SetTimeString();
   }
   CView::OnTimer(nIDEvent);
}

void CBlankView::OnDestroy()
{
	CView::OnDestroy();

	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if(m_pOldBitmap != NULL)
	{
		m_dcMemoryDC.SelectObject(m_pOldBitmap);
		m_bmpMemBitmap.DeleteObject();
		m_dcMemoryDC.DeleteDC();
		m_pOldBitmap = NULL;
	}	

	m_fontTime.DeleteObject();
	m_fontDate.DeleteObject();
}

BOOL CBlankView::OnEraseBkgnd(CDC* pCDC) 
{
	return TRUE;	
}

void CBlankView::OnMouseMove(UINT nFlags, CPoint point) 
{	
	m_pDoc->UpdateAllViews(NULL, MSG_MOUSE_MOVE_OUTSIDE, 0);
	CView::OnMouseMove(nFlags, point);
}
