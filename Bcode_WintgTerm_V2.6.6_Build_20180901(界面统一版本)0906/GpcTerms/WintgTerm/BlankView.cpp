#include "..\..\tg\blankview.h"
// BlankView.cpp : implementation file
//

#include "stdafx.h"
#include "BlankView.h"

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
	t_cur =  CTime::GetCurrentTime();
}

CBlankView::~CBlankView()
{
}


BEGIN_MESSAGE_MAP(CBlankView, CView)
	//{{AFX_MSG_MAP(CBlankView)
	ON_WM_TIMER()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBlankView drawing

void CBlankView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	CString strText;
	strText.Format("%02d:%02d:%02d" ,
		       t_cur.GetHour(),t_cur.GetMinute(),t_cur.GetSecond());

	CFont font;
	LOGFONT stFont;
	CFont * pOldFonts;
	memset(&stFont,0,sizeof(stFont));
	stFont.lfHeight = MulDiv(16,-pDC->GetDeviceCaps(LOGPIXELSY),72);
	stFont.lfWeight = FW_BOLD;  
	stFont.lfClipPrecision = CLIP_LH_ANGLES;
    strcpy(stFont.lfFaceName,"Times New Roman"); //Times New Roman
	font.CreateFontIndirect(&stFont);

    CPen mPen,*pOldPen;
	UINT nOldAlign = pDC->SetTextAlign(TA_CENTER);
	pOldFonts = pDC->SelectObject(&font);
	mPen.CreatePen(PS_SOLID,3,RGB(0,255,0));
	pOldPen = pDC->SelectObject(&mPen);
	
	CRect rc;
	GetClientRect(&rc);
	pDC->SetTextColor(RGB(255,0,0));
	pDC->TextOut(rc.CenterPoint().x, rc.top,strText);
	
	pDC->SelectObject(pOldFonts);  
	pDC->SetTextAlign(nOldAlign);
	pDC->SelectObject(pOldPen);
	font.DeleteObject(); 


 
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

void CBlankView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
  // Invalidate();
	return;
	
}

void CBlankView::OnInitialUpdate() 
{
	CView::OnInitialUpdate();
	
	if(!SetTimer(TIMER_IDEVENT_SYSTEMTIME,1000,NULL))
	{
	;//	sysprint(0,0,"[BLANKVIEW]: create timers failed");
	} 
}

void CBlankView::OnTimer(UINT nIDEvent) 
{
   if(nIDEvent == TIMER_IDEVENT_SYSTEMTIME )
   {
	   CTime t;
	   t = CTime::GetCurrentTime();
    
	   if( t.GetSecond()%5==0)
	   {     
		     t_cur = t;
	   }
   }
	CView::OnTimer(nIDEvent);
}

void CBlankView::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);
}

