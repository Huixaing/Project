// PlotStyle.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "PlotStyle.h"
#include "graphmark.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPlotStyle dialog

#define TRAINTYPENAMENUMBER	20
_TCHAR *_gszTrainTypeName[] =
{
	_T("旅客列车（包括国际特快列车）"), 
	_T("旅客列车（包括混合列车）"),
	_T("快运货物列车，直达列车"),
	_T("重载列车"),
	_T("直通、区段、小运转列车"),
	_T("保温列车"),
	_T("超限货物列车"),
	_T("摘挂列车"),
	_T("临时旅客快车"),
	_T("临时旅客列车"),
	_T("回送的旅客列车"),
	_T("货物列车附挂补机、双机牵引或附挂有火车机的列车"),
	_T("单机"),
	_T("重挂单机"),
	_T("高级专列及先驱列车"),
	_T("军用列车"),
	_T("回送的军用列车"),
	_T("重型轨道车、轻油列车"),
	_T("救援列车、除雪列车"),
	_T("路用列车（包括工程、材料检修车备用车、回送空守车、试运转列车等）"),
};


CPlotStyle::CPlotStyle(CWnd* pParent /*=NULL*/)
	: CDialog(CPlotStyle::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPlotStyle)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_ack.LoadBitmaps("acku","ackd","ackf","ackl");
	m_cancel.LoadBitmaps("retrieveu","retrieved","retrievef");
	m_modify.LoadBitmaps("acceptu","acceptd","acceptf","acceptl");
}


void CPlotStyle::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPlotStyle)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPlotStyle, CDialog)
	//{{AFX_MSG_MAP(CPlotStyle)
	ON_BN_CLICKED(IDC_STYLE_ACK, OnStyleAck)
	ON_BN_CLICKED(IDC_STYLE_CANCEL, OnStyleCancel)
	ON_BN_CLICKED(IDC_STYLE_CHANGE, OnStyleChange)
	ON_WM_LBUTTONDOWN()
	ON_WM_DRAWITEM()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPlotStyle message handlers

void CPlotStyle::OnStyleAck() 
{
	// TODO: Add your control notification handler code here
	CDialog::OnOK( );
}

void CPlotStyle::OnStyleCancel() 
{
	// TODO: Add your control notification handler code here
	CDialog::OnCancel( );
}

void CPlotStyle::OnStyleChange() 
{
	// TODO: Add your control notification handler code here
	
}

BOOL CPlotStyle::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CDialog::PreTranslateMessage(pMsg);
}

void CPlotStyle::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
	CDialog::OnLButtonDown(nFlags, point);
}

BOOL CPlotStyle::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	VERIFY(m_ack.SubclassDlgItem(IDC_STYLE_ACK,this));
	VERIFY(m_cancel.SubclassDlgItem(IDC_STYLE_CANCEL,this));
	VERIFY(m_modify.SubclassDlgItem(IDC_STYLE_CHANGE,this));

	m_ack.SizeToContent( );
	m_cancel.SizeToContent( );
	m_modify.SizeToContent( );
	// TODO: Add extra initialization here

	CWnd *pWnd = GetDlgItem(IDC_STYLE_SAMPLE);
	CRect rclRect;
	CGraphConnect *pFigure;

	pWnd->GetWindowRect(&rclRect);
	ScreenToClient(&rclRect);
	m_sample.Create("sample",0,rclRect,this,IDC_STYLE_SAMPLE + 30);

	CPoint ptStart,ptEnd;
	ptStart.x = 3;
	ptEnd.x = rclRect.Width( ) - 3;
	ptStart.y = 3;
// allocate sample 
	ptEnd.y = ptStart.y + 20;
	pFigure = new CGraphDoubleLine(ptStart,ptEnd,CLR_RED,CLR_RED);
	ptStart.y = ptEnd.y + 4;
	m_sample.AddObject(pFigure);

	ptEnd.y = ptStart.y + 20;
	pFigure = new CGraphSingleLine(ptStart,ptEnd,CLR_RED);
	ptStart.y = ptEnd.y + 4;
	m_sample.AddObject(pFigure);

	ptEnd.y = ptStart.y + 20;
	pFigure = new CGraphSingleLine(ptStart,ptEnd,CLR_BLUE);
	ptStart.y = ptEnd.y + 4;
	m_sample.AddObject(pFigure);

	ptEnd.y = ptStart.y + 20;
	pFigure = new CGraphDoubleLine(ptStart,ptEnd,CLR_BLUE,CLR_BLUE);
	ptStart.y = ptEnd.y + 4;
	m_sample.AddObject(pFigure);

	ptEnd.y = ptStart.y + 20;
	pFigure = new CGraphSingleLine(ptStart,ptEnd,CLR_BLACK);
	ptStart.y = ptEnd.y + 4;
	m_sample.AddObject(pFigure);

	ptEnd.y = ptStart.y + 20;
	pFigure = new CGraphCircleLine(ptStart,ptEnd,CLR_RED,CLR_BLACK);
	ptStart.y = ptEnd.y + 4;
	m_sample.AddObject(pFigure);

	ptEnd.y = ptStart.y + 20;
	pFigure = new CGraphRectLine(ptStart,ptEnd,CLR_BLUE,CLR_BLACK);
	ptStart.y = ptEnd.y + 4;
	m_sample.AddObject(pFigure);

	ptEnd.y = ptStart.y + 20;
	pFigure = new CGraphADLine(ptStart,ptEnd,CLR_BLACK,CLR_BLACK);
	ptStart.y = ptEnd.y + 4;
	m_sample.AddObject(pFigure);

	ptEnd.y = ptStart.y + 20;
	pFigure = new CGraphVertDoubleLine(ptStart,ptEnd,CLR_RED,CLR_RED);
	ptStart.y = ptEnd.y + 4;
	m_sample.AddObject(pFigure);

	ptEnd.y = ptStart.y + 20;
	pFigure = new CGraphVertSingleLine(ptStart,ptEnd,CLR_RED,CLR_RED);
	ptStart.y = ptEnd.y + 4;
	m_sample.AddObject(pFigure);

	ptEnd.y = ptStart.y + 20;
	pFigure = new CGraphRectLine(ptStart,ptEnd,CLR_RED,CLR_RED);
	ptStart.y = ptEnd.y + 4;
	m_sample.AddObject(pFigure);

	ptEnd.y = ptStart.y + 20;
	pFigure = new CGraphSolidDashLine(ptStart,ptEnd,CLR_BLUE,CLR_BLACK);
	ptStart.y = ptEnd.y + 4;
	m_sample.AddObject(pFigure);

	ptEnd.y = ptStart.y + 20;
	pFigure = new CGraphTriangleLine(ptStart,ptEnd,CLR_BLACK,CLR_BLACK);
	ptStart.y = ptEnd.y + 4;
	m_sample.AddObject(pFigure);

	ptEnd.y = ptStart.y + 20;
	pFigure = new CGraphTriangleLine(ptStart,ptEnd,CLR_BLACK,CLR_BLACK);
	ptStart.y = ptEnd.y + 4;
	m_sample.AddObject(pFigure);

	ptEnd.y = ptStart.y + 20;
	pFigure = new CGraphArrowLine(ptStart,ptEnd,CLR_RED,CLR_RED);
	ptStart.y = ptEnd.y + 4;
	m_sample.AddObject(pFigure);

	ptEnd.y = ptStart.y + 20;
	pFigure = new CGraphDashLine(ptStart,ptEnd,CLR_RED);
	ptStart.y = ptEnd.y + 4;
	m_sample.AddObject(pFigure);

	ptEnd.y = ptStart.y + 20;
	pFigure = new CGraphRectDashLine(ptStart,ptEnd,CLR_RED,CLR_RED);
	ptStart.y = ptEnd.y + 4;
	m_sample.AddObject(pFigure);

	ptEnd.y = ptStart.y + 20;
	pFigure = new CGraphVertSingleLine(ptStart,ptEnd,CLR_BLACK,CLR_BLACK);
	ptStart.y = ptEnd.y + 4;
	m_sample.AddObject(pFigure);

	ptEnd.y = ptStart.y + 20;
	pFigure = new CGraphXSignalLine(ptStart,ptEnd,CLR_RED,CLR_RED);
	ptStart.y = ptEnd.y + 4;
	m_sample.AddObject(pFigure);	

	ptEnd.y = ptStart.y + 20;
	pFigure = new CGraphCircleLine(ptStart,ptEnd,CLR_BLUE,CLR_BLACK);
	ptStart.y = ptEnd.y + 4;
	m_sample.AddObject(pFigure);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPlotStyle::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	// TODO: Add your message handler code here and/or call default
	
	CDialog::OnDrawItem(nIDCtl, lpDrawItemStruct);
/*	if(nIDCtl == IDC_STYLE_CURRENT)
	{
		CWnd *pWnd = GetDlgItem(IDC_STYLE_CURRENT);
//		CDC *pCDC = pWnd->GetDC( );
		
	} else if(nIDCtl == IDC_STYLE_SAMPLE + 30){ */
		CWnd *pWnd = GetDlgItem(IDC_STYLE_SAMPLE + 30);
		CDC *pCDC = pWnd->GetDC( );
		m_sample.OnDraw(pCDC);
		pWnd->ReleaseDC(pCDC);
	
}

void CPlotStyle::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	CRect rclClipBox,rclWindow,rclSect;

	dc.GetClipBox(&rclClipBox);
	m_sample.GetWindowRect(&rclWindow);
	ScreenToClient(&rclWindow);
	if(rclSect.IntersectRect(&rclClipBox,&rclWindow))
		m_sample.OnDraw(&dc);
	// Do not call CDialog::OnPaint() for painting messages
}
