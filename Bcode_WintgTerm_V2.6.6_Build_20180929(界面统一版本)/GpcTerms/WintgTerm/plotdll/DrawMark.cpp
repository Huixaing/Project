#include "stdafx.h"
#include "plotDoc.h"
#include <math.h>
CString CPlotAbsMark::GetKiloMDesp()
{
	CString ret="";
	
	if(rec.position != 0 || rec.s_meter != 0 || rec.e_meter != 0 || rec.e_kilo != 0)
	{
		ret.Format("%dkm%dm至%dkm%dm",rec.position,rec.s_meter,rec.e_kilo,rec.e_meter);
	}
	
	return ret;
}
void CPlotAbsMark::Draw(CDC *pCDC,int nPageWidth)
{
	switch(rec.type)
	{
	//----------------------ABS-----------------------
	case ABS_INHIBIT_MARK:
		DrawAbsInhitBlock(pCDC, nPageWidth);
		break;
	case ABS_SLOW_MARK:
		DrawAbsSlowBlock(pCDC, nPageWidth);
		break;
	case SELF_CLOSE_MARK:
		DrawAbsSelfCloseBlock(pCDC, nPageWidth);
		break;
	case ABS_ELE_FIX_MARK:
		DrawAbsEleFix(pCDC, nPageWidth);
		break;
	case NEW_ABS_ELE_FIX_MARK:
		DrawLanzhouAbsEleFix(pCDC, nPageWidth);
		break;
	case ABS_FIX_MARK:
		DrawAbsZhFix(pCDC, nPageWidth);
		break;
	//---------------------Side-----------------------
	case SIDE_INHIBIT_MARK:
	case SIDE_SLOW_MARK:
	case SIDE_OCCUPY_MARK:
	case SIDE_TIANCHUANG_MARK:
		DrawSationBlock(pCDC, nPageWidth);
		break;
	//---------------------Text-----------------------
	case GRAPH_NOTE_MARK:
		DrawNoteTextBlock(pCDC, nPageWidth);
		break;
	case NEW_GRAPH_NOTE_MARK:
		DrawNewNoteTextBlock(pCDC, nPageWidth);
		break;
	default:
		;
	}
	
	return;
}

void CPlotAbsMark::DrawNoteTextBlock(CDC *pCDC,int nPageWidth)
{
	if(rec.type != GRAPH_NOTE_MARK )
		return;

	int scalerate = min(m_pDoc->yMultiple,5);

	int nMaxIndex,nMinIndex;
	nMaxIndex = nMinIndex = -1;
	nPageWidth = m_pDoc->m_fPixelPerMinute * m_pDoc->m_nViewTimeRange*60; 


	m_drawRect.top    = rec.station*scalerate + m_pDoc->y_offset_print;
	m_drawRect.bottom = rec.position*scalerate + m_pDoc->y_offset_print;
	nMinIndex = rec.index;
	nMaxIndex = rec.index;

	//added by zhaoliang 20100812 for BLOCK_ELE print erro
	if(nMinIndex < 0 || nMinIndex >= m_pDoc->m_nDispDistanceNum)
		return;
	if(nMaxIndex < 0 || nMaxIndex >= m_pDoc->m_nDispDistanceNum)
		return;

	mColor = CLR_RED;

	int y1=0,y2=0;
	if(m_pDoc->m_sDispDistance[nMinIndex].distance.node_info.show_sides)
		y1 = m_pDoc->m_sDispDistance[nMinIndex].nSecondStationYPos;
	else
		y1 = m_pDoc->m_sDispDistance[nMinIndex].nStationYPos;
	y2 = m_pDoc->m_sDispDistance[nMaxIndex].nStationYPos;

	//////////////////////////////////////////////////////////////////////
	int x1,x2;
	x1 = m_pDoc->ConvertTimetoXPos(rec.start_time,rec.start_date,m_pDoc->m_nSchdStatus);
	x2 = m_pDoc->ConvertTimetoXPos(rec.end_time, rec.end_date,m_pDoc->m_nSchdStatus);

	if(x1<= 0 && x2<=0)
		return;
	if(x1<= 0 && x2>0)
		x1 = 0;

	if(x1 >= nPageWidth && x2 > nPageWidth)
		return;
	if(x1 < nPageWidth && x2 >= nPageWidth)
		x2 = nPageWidth;

	m_drawRect.left  = x1;
	m_drawRect.right = x2;

	int ntop = m_drawRect.bottom - m_drawRect.top;
	if(rec.e_kilo == 1)
	{
		m_drawRect.top    = y1 - rec.line - ntop/2;
		m_drawRect.bottom = y1 - rec.line + ntop/2;
	}
	else
	{
		m_drawRect.top    = y1 + rec.line - ntop/2;
		m_drawRect.bottom = y1 + rec.line + ntop/2;
	}
	if(m_drawRect.Height () < 10)
		m_drawRect.bottom = m_drawRect.top + 10;

	if(rec.flag == 5)
	{
		//保留车，画确定大小
		//设置保留车的圈直径为0.5英寸
		//1cm = 0.3937inch
		//1inch = 2.54cm
		int timeRectWidth = pCDC->GetDeviceCaps(LOGPIXELSX)*0.3937;//定义实际直径为1cm
		int timeRectHeight = pCDC->GetDeviceCaps(LOGPIXELSY);

		CPoint cp = m_drawRect.CenterPoint();
		m_drawRect.left = cp.x - timeRectWidth/2;
		m_drawRect.right = cp.x + timeRectWidth/2;
		m_drawRect.top = cp.y - timeRectWidth/2;
		m_drawRect.bottom = cp.y + timeRectWidth/2;
	}
	m_drawRect.OffsetRect (m_pDoc->x_offset_print, m_pDoc->y_offset_print); 

	CPen mPen,*pOldPen;
	CRect rect;
	UINT nPreviousMode = pCDC->SetROP2(R2_COPYPEN); 
	UINT nOldMode = pCDC->SetBkMode(TRANSPARENT);
	UINT nOldColor = pCDC->SetTextColor(RGB(255,0,0));
	UINT nOldAlign = pCDC->SetTextAlign(TA_CENTER|TA_BOTTOM);
	
		
	CPoint LinePos = m_drawRect.CenterPoint(); //中间横线的Y轴坐标，用来确定文字输出位置

	mPen.CreatePen(PS_SOLID,scalerate,RGB(255,0,0));
	pOldPen = pCDC->SelectObject(&mPen);
	
	int m_nLogPixelSy = pCDC->GetDeviceCaps(LOGPIXELSY);
	if(m_nLogPixelSy >300)
		m_nLogPixelSy = 400;

	LOGFONT stFont;
	CFont mFont;
	CFont *pOldFont;
	memset(&stFont,0,sizeof(stFont));
	stFont.lfHeight = MulDiv(8,-m_nLogPixelSy,72);	  //12
	stFont.lfWeight = FW_THIN;//FW_NORMAL;
	stFont.lfClipPrecision = CLIP_LH_ANGLES;
	mFont.CreateFontIndirect(&stFont);
	pOldFont = pCDC->SelectObject(&mFont);

	CString strName,strDistance,strDirection; 

	if(IsLineDirectionUp(rec.line))
		strDirection = "上行";
	else if(IsLineDirectionDown(rec.line))
		strDirection = "下行";
	else if(IsLineDirectionDual(rec.line))
		strDirection = "上下行";
	else
		strDirection = "";

	CRect graph_note_rect = m_drawRect;
	CString s_kilo,s_meter,e_kilo,e_meter,start_time;

	CString strDCmdno, strReason;
	rec.text[255] = 0;
	m_pDoc->ParseMarkReasonText(CString(rec.text), strDCmdno, strReason);

	strName = strReason;

	CRect drawRect = m_drawRect;
	CPoint cp = drawRect.CenterPoint();
	drawRect.left = cp.x - drawRect.Height()/2;
	drawRect.right = cp.x + drawRect.Height()/2;

	CPoint pStart;
	pStart.x = drawRect.left;
	pStart.y = drawRect.top+ drawRect.Height()/2;

	if(IsFrameBlue(rec.slow_time))
	{
		CPen bluePen;
		bluePen.CreatePen(PS_INSIDEFRAME,1,CLR_BLUE);
		CPen* oldPen = pCDC->SelectObject(&bluePen);

		FrameRectExtend(pCDC,m_drawRect);

		pCDC->SelectObject(oldPen);
		bluePen.DeleteObject();
	}
	else if(IsFrameRed(rec.slow_time))
	{
		CPen bluePen;
		bluePen.CreatePen(PS_INSIDEFRAME,1,CLR_RED);
		CPen* oldPen = pCDC->SelectObject(&bluePen);

		FrameRectExtend(pCDC,m_drawRect);

		pCDC->SelectObject(oldPen);
		bluePen.DeleteObject();
	}
	else if(IsCircleRed(rec.slow_time))
	{
		CPen bluePen;
		bluePen.CreatePen(PS_INSIDEFRAME,1,CLR_RED);
		CPen* oldPen = pCDC->SelectObject(&bluePen);

		CRect rect;
		long radius=sqrt((double)m_drawRect.Width() * m_drawRect.Width() + m_drawRect.Height() * m_drawRect.Height())/3;
		rect.left=m_drawRect.CenterPoint().x-radius;
		rect.right=m_drawRect.CenterPoint().x+radius;
		rect.top=m_drawRect.CenterPoint().y-radius;
		rect.bottom=m_drawRect.CenterPoint().y+radius;
		pCDC->Arc(&rect,pStart,pStart);

		pCDC->SelectObject(oldPen);
		bluePen.DeleteObject();
	}
	else if(IsCircleBlue(rec.slow_time))
	{
		CPen bluePen;
		bluePen.CreatePen(PS_INSIDEFRAME,1,CLR_BLUE);
		CPen* oldPen = pCDC->SelectObject(&bluePen);

		CRect rect;
		long radius=sqrt((double)m_drawRect.Width() * m_drawRect.Width() + m_drawRect.Height() * m_drawRect.Height())/3;
		rect.left=m_drawRect.CenterPoint().x-radius;
		rect.right=m_drawRect.CenterPoint().x+radius;
		rect.top=m_drawRect.CenterPoint().y-radius;
		rect.bottom=m_drawRect.CenterPoint().y+radius;
		pCDC->Arc(&rect,pStart,pStart);

		pCDC->SelectObject(oldPen);
		bluePen.DeleteObject();
	}				

	if(!strDCmdno.IsEmpty())
		strName = "揭示命令" + strDCmdno +", "+ strName;


	CSize textSize = pCDC->GetTextExtent(strName);


	CPoint myPoint = m_drawRect.CenterPoint();


	myPoint = graph_note_rect.CenterPoint();
	CFont font;		
	CString strFontName = "Arial";
	font.CreateFont(graph_note_rect.Height(), 0,0,0, FW_NORMAL, 0,0,0, DEFAULT_CHARSET,
		OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_DONTCARE, strFontName);
	CFont* poldFont;
	poldFont = pCDC->SelectObject(&font);
	textSize = pCDC->GetTextExtent(strName);
	pCDC->TextOut (myPoint.x,myPoint.y +textSize.cy/2,strName);
	pCDC->SelectObject(poldFont);
	font.DeleteObject();



	pCDC->SetTextAlign(nOldAlign);
	pCDC->SetBkMode(nOldMode);
	pCDC->SetTextColor(nOldColor);
	pCDC->SelectObject(pOldPen);
	pCDC->SetROP2(nPreviousMode); 
	pCDC->SelectObject (pOldFont);

	mFont.DeleteObject();
	mPen.DeleteObject();
}
void CPlotAbsMark::DrawNewNoteTextBlock(CDC *pCDC,int nPageWidth)
{
	if(rec.type != NEW_GRAPH_NOTE_MARK)
		return;

	int scalerate = min(m_pDoc->yMultiple,5);

	nPageWidth = m_pDoc->m_fPixelPerMinute * m_pDoc->m_nViewTimeRange*60; 

	int	nMinIndex   = rec.s_meter&0x00FF; 
	int nMaxIndex = (rec.s_meter&0xFF00)>>8;

	int nMidRate   = rec.e_meter;
	if(nMidRate > 100)
		return;

	int Y1, Y2;
	// 位于最上站平线上方---最上站平线和底图最上边界确定位置
	if(nMinIndex == 0xFF)
	{
		Y1 = 0; // 底图上边时间位置
		if(nMaxIndex < 0 || nMaxIndex >= m_pDoc->m_nDispDistanceNum)
			return;

		Y2 = m_pDoc->m_sDispDistance[nMaxIndex].nStationYPos; 
	}
	// 位于最下站平线下方---最下站平线和底图最下边界确定位置
	else if(nMaxIndex == 0xFF)
	{
		if(nMinIndex < 0 || nMinIndex >= m_pDoc->m_nDispDistanceNum)
			return;
		Y1 = m_pDoc->m_sDispDistance[nMinIndex].nStationYPos;			
		Y2 = m_pDoc->m_sDispDistance[m_pDoc->m_nDispDistanceNum-1].nSecondStationYPos+100; // 底图下边时间位置
	}
	// 位于两个站平线间---两个站平线确定位置
	else
	{
		if(nMinIndex < 0 || nMinIndex >= m_pDoc->m_nDispDistanceNum)
			return;
		if(nMaxIndex < 0 || nMaxIndex >= m_pDoc->m_nDispDistanceNum)
			return;

		Y1 = m_pDoc->m_sDispDistance[nMinIndex].nSecondStationYPos; // 上站平线位置
		Y2 = m_pDoc->m_sDispDistance[nMaxIndex].nStationYPos; // 下站平线位置
	}

	int MidY = Y1 + ((Y2-Y1)*nMidRate)/100;

	int height = rec.e_kilo*scalerate;
	m_drawRect.top    = MidY - height/2;
	m_drawRect.bottom = MidY + height/2;

	int y1 = m_drawRect.top;
	int y2 = m_drawRect.bottom;

	//////////////////////////////////////////////////////////////////////
	int x1 = m_pDoc->ConvertTimetoXPos(rec.start_time,rec.start_date,m_pDoc->m_nSchdStatus);
	int x2 = m_pDoc->ConvertTimetoXPos(rec.end_time, rec.end_date,m_pDoc->m_nSchdStatus);

	if(x1<= 0 && x2<=0)
		return;
	if(x1<= 0 && x2>0)
		x1 = 0;

	if(x1 >= nPageWidth && x2 > nPageWidth)
		return;
	if(x1 < nPageWidth && x2 >= nPageWidth)
		x2 = nPageWidth;

	m_drawRect.left  = x1;
	m_drawRect.right = x2;

	m_drawRect.OffsetRect (m_pDoc->x_offset_print, m_pDoc->y_offset_print); 

	CPen mPen,*pOldPen;
	CRect rect;
	UINT nPreviousMode = pCDC->SetROP2(R2_COPYPEN); 
	UINT nOldMode = pCDC->SetBkMode(TRANSPARENT);
	UINT nOldColor = pCDC->SetTextColor(RGB(255,0,0));
	UINT nOldAlign = pCDC->SetTextAlign(TA_CENTER|TA_BOTTOM);
	

	CPoint LinePos = m_drawRect.CenterPoint(); //中间横线的Y轴坐标，用来确定文字输出位置
	mPen.CreatePen(PS_SOLID,scalerate,RGB(255,0,0));
	pOldPen = pCDC->SelectObject(&mPen);

	int m_nLogPixelSy = pCDC->GetDeviceCaps(LOGPIXELSY);
	if(m_nLogPixelSy >300)
		m_nLogPixelSy = 400;

	LOGFONT stFont;
	CFont mFont;
	CFont *pOldFont;
	memset(&stFont,0,sizeof(stFont));
	stFont.lfHeight = MulDiv(8,-m_nLogPixelSy,72);	  //12
	stFont.lfWeight = FW_THIN;//FW_NORMAL;
	stFont.lfClipPrecision = CLIP_LH_ANGLES;
	mFont.CreateFontIndirect(&stFont);
	pOldFont = pCDC->SelectObject(&mFont);

	CString strName,strDistance; 
	CRect graph_note_rect = m_drawRect;
	CString s_kilo,s_meter,e_kilo,e_meter,start_time;

	CString strDCmdno, strReason;
	rec.text[255] = 0;
	m_pDoc->ParseMarkReasonText(CString(rec.text), strDCmdno, strReason);

	strName = strReason;

	CRect drawRect = m_drawRect;
	CPoint cp = drawRect.CenterPoint();
	drawRect.left = cp.x - drawRect.Height()/2;
	drawRect.right = cp.x + drawRect.Height()/2;

	CPoint pStart;
	pStart.x = drawRect.left;
	pStart.y = drawRect.top+ drawRect.Height()/2;

	if(IsFrameBlue(rec.slow_time))
	{
		CPen bluePen;
		bluePen.CreatePen(PS_INSIDEFRAME,1,CLR_BLUE);
		CPen* oldPen = pCDC->SelectObject(&bluePen);

		FrameRectExtend(pCDC,m_drawRect);

		pCDC->SelectObject(oldPen);
		bluePen.DeleteObject();
	}
	else if(IsFrameRed(rec.slow_time))
	{
		CPen bluePen;
		bluePen.CreatePen(PS_INSIDEFRAME,1,CLR_RED);
		CPen* oldPen = pCDC->SelectObject(&bluePen);

		FrameRectExtend(pCDC,m_drawRect);

		pCDC->SelectObject(oldPen);
		bluePen.DeleteObject();
	}
	else if(IsCircleRed(rec.slow_time))
	{
		CPen bluePen;
		bluePen.CreatePen(PS_INSIDEFRAME,1,CLR_RED);
		CPen* oldPen = pCDC->SelectObject(&bluePen);

		CRect rect;
		long radius=sqrt((double)m_drawRect.Width() * m_drawRect.Width() + m_drawRect.Height() * m_drawRect.Height())/3;
		rect.left=m_drawRect.CenterPoint().x-radius;
		rect.right=m_drawRect.CenterPoint().x+radius;
		rect.top=m_drawRect.CenterPoint().y-radius;
		rect.bottom=m_drawRect.CenterPoint().y+radius;
		pCDC->Arc(&rect,pStart,pStart);

		pCDC->SelectObject(oldPen);
		bluePen.DeleteObject();
	}
	else if(IsCircleBlue(rec.slow_time))
	{
		CPen bluePen;
		bluePen.CreatePen(PS_INSIDEFRAME,1,CLR_BLUE);
		CPen* oldPen = pCDC->SelectObject(&bluePen);

		CRect rect;
		long radius=sqrt((double)m_drawRect.Width() * m_drawRect.Width() + m_drawRect.Height() * m_drawRect.Height())/3;
		rect.left=m_drawRect.CenterPoint().x-radius;
		rect.right=m_drawRect.CenterPoint().x+radius;
		rect.top=m_drawRect.CenterPoint().y-radius;
		rect.bottom=m_drawRect.CenterPoint().y+radius;
		pCDC->Arc(&rect,pStart,pStart);

		pCDC->SelectObject(oldPen);
		bluePen.DeleteObject();
	}				

	if(!strDCmdno.IsEmpty())
		strName = "揭示命令" + strDCmdno +", "+ strName;

	CSize textSize = pCDC->GetTextExtent(strName);

	CPoint myPoint = graph_note_rect.CenterPoint();
	CFont font;		
	CString strFontName = "Arial";
	font.CreateFont(graph_note_rect.Height(), 0,0,0, FW_NORMAL, 0,0,0, DEFAULT_CHARSET,
		OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_DONTCARE, strFontName);
	CFont* poldFont;
	poldFont = pCDC->SelectObject(&font);
	textSize = pCDC->GetTextExtent(strName);
	pCDC->TextOut (myPoint.x,myPoint.y +textSize.cy/2,strName);
	pCDC->SelectObject(poldFont);
	font.DeleteObject();

	pCDC->SetTextAlign(nOldAlign);
	pCDC->SetBkMode(nOldMode);
	pCDC->SetTextColor(nOldColor);
	pCDC->SelectObject(pOldPen);
	pCDC->SetROP2(nPreviousMode); 
	pCDC->SelectObject (pOldFont);

	mFont.DeleteObject();
	mPen.DeleteObject();
}

void CPlotAbsMark::DrawSationBlock(CDC *pCDC,int nPageWidth)
{
	if(rec.type != SIDE_INHIBIT_MARK &&
		rec.type != SIDE_TIANCHUANG_MARK &&
		rec.type != SIDE_SLOW_MARK &&
		rec.type != SIDE_OCCUPY_MARK)
		return;

	int ndraw=0;
	COLORREF nColor=RGB(255,0,0);
	COLORREF nTextColor=RGB(255,0,0);
	switch(rec.type)
	{
	case SIDE_INHIBIT_MARK:
		nColor = m_pDoc->m_SideInhibitMarkCfg.color1;
		nTextColor = m_pDoc->m_SideInhibitMarkCfg.color3;
		break;
	case SIDE_TIANCHUANG_MARK:
		nColor = m_pDoc->m_SideTianchuangMarkCfg.color1;
		nTextColor = m_pDoc->m_SideTianchuangMarkCfg.color3;
		break;
	case SIDE_SLOW_MARK:
		nColor = m_pDoc->m_SideSlowMarkCfg.color1;
		nTextColor = m_pDoc->m_SideSlowMarkCfg.color3;
		break;
	case SIDE_OCCUPY_MARK:
		nColor = m_pDoc->m_SideOccupyMarkCfg.color1;
		nTextColor = m_pDoc->m_SideOccupyMarkCfg.color3;
		break;
	default:
		return;
	}
	
	int scalerate = min(m_pDoc->yMultiple,5);
	nPageWidth = m_pDoc->m_fPixelPerMinute * m_pDoc->m_nViewTimeRange*60; 

	int nMaxIndex = rec.index;
	int nMinIndex = rec.index;

	//added by zhaoliang 20100812 for BLOCK_ELE print erro
	if(nMinIndex < 0 || nMinIndex >= m_pDoc->m_nDispDistanceNum)
		return;
	if(nMaxIndex < 0 || nMaxIndex >= m_pDoc->m_nDispDistanceNum)
		return;
	

	int y1=0,y2=0;
	if(m_pDoc->m_sDispDistance[nMinIndex].distance.node_info.show_sides)
		y1 = m_pDoc->m_sDispDistance[nMinIndex].nSecondStationYPos;
	else
		y1 = m_pDoc->m_sDispDistance[nMinIndex].nStationYPos;
	y2 = m_pDoc->m_sDispDistance[nMaxIndex].nStationYPos;
	
	//////////////////////////////////////////////////////////////////////
	int x1,x2;
	x1 = m_pDoc->ConvertTimetoXPos(rec.start_time,rec.start_date,m_pDoc->m_nSchdStatus);
	x2 = m_pDoc->ConvertTimetoXPos(rec.end_time, rec.end_date,m_pDoc->m_nSchdStatus);

	if(x1<= 0 && x2<=0)
		return;
	if(x1<= 0 && x2>0)
		x1 = 0;

	if(x1 >= nPageWidth && x2 > nPageWidth)
		return;
	if(x1 < nPageWidth && x2 >= nPageWidth)
		x2 = nPageWidth;

	m_drawRect.SetRect(x1, y1-4, x2, y1+4);  
	m_drawRect.OffsetRect (m_pDoc->x_offset_print,m_pDoc->y_offset_print);
	
	CPen mPen,*pOldPen,dashPen;
	
	UINT nPreviousMode = pCDC->SetROP2(R2_COPYPEN); 
	UINT nOldMode = pCDC->SetBkMode(TRANSPARENT);
	UINT nOldAlign = pCDC->SetTextAlign(TA_CENTER|TA_BOTTOM);
	
	int dashStyleCount = 2;
	DWORD dashStyleArray[2];
	dashStyleArray[0] = 20* scalerate;
	dashStyleArray[1] = 10* scalerate;
	LOGBRUSH dashlogBrush;
	dashlogBrush.lbStyle = BS_SOLID;

	dashlogBrush.lbColor = nColor;
	dashlogBrush.lbHatch = 0;
	dashPen.CreatePen(PS_GEOMETRIC|PS_USERSTYLE, scalerate,&dashlogBrush,dashStyleCount,dashStyleArray); 
	mPen.CreatePen(PS_SOLID,scalerate, nColor);
			
	int m_nLogPixelSy = pCDC->GetDeviceCaps(LOGPIXELSY);
	if(m_nLogPixelSy >300)
		m_nLogPixelSy = 400;

	LOGFONT stFont;
	CFont mFont;
	CFont *pOldFont;
	memset(&stFont,0,sizeof(stFont));
	stFont.lfHeight = MulDiv(8,-m_nLogPixelSy,72);	  //12
	stFont.lfWeight = FW_THIN;//FW_NORMAL;
	stFont.lfClipPrecision = CLIP_LH_ANGLES;
	mFont.CreateFontIndirect(&stFont);
	
	pOldFont = pCDC->SelectObject(&mFont);
	pOldPen = pCDC->SelectObject(&mPen);
	
	pCDC->MoveTo(m_drawRect.left, m_drawRect.top);
	pCDC->LineTo(m_drawRect.left ,m_drawRect.bottom);

	pCDC->MoveTo(m_drawRect.right, m_drawRect.top);
	pCDC->LineTo(m_drawRect.right ,m_drawRect.bottom);

	if(rec.type == SIDE_SLOW_MARK)
		pCDC->SelectObject(&dashPen);
			
	pCDC->MoveTo(m_drawRect.left,  m_drawRect.CenterPoint().y);
	pCDC->LineTo(m_drawRect.right, m_drawRect.CenterPoint().y);
	
	if(rec.type == SIDE_SLOW_MARK)
		pCDC->SelectObject(&mPen);

	char time_char_left[2];
	char time_char_right[2];

	itoa(rec.start_time/60 % 10, time_char_left, 10);
	itoa(rec.end_time/60 % 10, time_char_right, 10);
	
	if(x1 == 0)
		strcpy(time_char_left,"0");
	if(x2 == nPageWidth)
		strcpy(time_char_right,"0");

	UINT nOldColor = pCDC->SetTextColor(nColor);
	pCDC->TextOut(m_drawRect.left, m_drawRect.top, time_char_left, 1);
	pCDC->TextOut(m_drawRect.right, m_drawRect.top, time_char_right, 1);

	pCDC->TextOut(m_drawRect.left, m_drawRect.bottom + (pCDC->GetTextExtent(time_char_left)).cy, time_char_left, 1);
	pCDC->TextOut(m_drawRect.right, m_drawRect.bottom + (pCDC->GetTextExtent(time_char_right)).cy, time_char_right, 1);
	

	CString strName, strDCmdno, strReason;
	rec.text[255] = 0;
	m_pDoc->ParseMarkReasonText(CString(rec.text), strDCmdno, strReason);

	CString strKm = GetKiloMDesp();
	
	switch(rec.type)
	{
	case SIDE_INHIBIT_MARK:
	case SIDE_TIANCHUANG_MARK:   
		{
			CString strScope,str;
			if(m_pDoc->IsBlockAllSide(rec.station, rec.count))
			{
				strScope="";
			}
			else
			{
				for(int i = 0;i< rec.count ;i ++)
				{
					if(i == rec.count -1)
						str.Format("%s  ", m_pDoc->GetSideName(rec.station, rec.ele_number[i]));
					else
						str.Format("%s,", m_pDoc->GetSideName(rec.station, rec.ele_number[i]));

					strScope = strScope + str;
				}
			}

			strName = strScope+ strKm + strReason;		
		}
		break;
	case SIDE_SLOW_MARK:
	case SIDE_OCCUPY_MARK:
		{
			CString strScope,str;
			if(m_pDoc->IsBlockAllSide(rec.station, rec.count))
			{
				strScope="";
			}
			else
			{
				for(int i = 0;i< rec.count ;i ++)
				{
					if(i == rec.count -1)
						str.Format("%s  ", m_pDoc->GetSideName(rec.station, rec.ele_number[i]));
					else
						str.Format("%s,", m_pDoc->GetSideName(rec.station, rec.ele_number[i]));

					strScope = strScope + str;
				}
			}

			if(rec.type == SIDE_SLOW_MARK)
				strName.Format("%s %s %s 限速 %dkm/h ",strScope, strKm, strReason, rec.flag*5);
			else
				strName.Format("%s %s %s",strScope, strKm, strReason);
		}
		break;
	default:
		return;
	};

	if(!strDCmdno.IsEmpty())
		strName = "揭示命令" + strDCmdno +", "+ strName;

	pCDC->SetTextColor(nTextColor);
	CSize textSize = pCDC->GetTextExtent(strName);
	CPoint LinePos = m_drawRect.CenterPoint();
	if(IsLineTextPosBottom(rec.line))
		pCDC->TextOut (LinePos.x,LinePos.y + textSize.cy,strName);
	else
		pCDC->TextOut (LinePos.x,LinePos.y,strName);
	

	pCDC->SetTextAlign(nOldAlign);
	pCDC->SetBkMode(nOldMode);
	pCDC->SetTextColor(nOldColor);
	pCDC->SelectObject(pOldPen);
	pCDC->SetROP2(nPreviousMode); 
	pCDC->SelectObject (pOldFont);

	mFont.DeleteObject();
	mPen.DeleteObject();
	dashPen.DeleteObject();
}

void CPlotAbsMark::DrawAbsInhitBlock(CDC *pCDC,int nPageWidth)
{
	int scalerate = min(m_pDoc->yMultiple,5);
	nPageWidth = m_pDoc->m_fPixelPerMinute * m_pDoc->m_nViewTimeRange*60; 

	if(rec.type != ABS_INHIBIT_MARK) 
		return;

	int nMinIndex = rec.station;
	int nMaxIndex = rec.index;
	if(nMinIndex>nMaxIndex)
	{
		int tmp = nMinIndex;
		nMinIndex=nMaxIndex;
		nMaxIndex=tmp;
	}

	//added by zhaoliang 20100812 for BLOCK_ELE print erro
	if(nMinIndex < 0 || nMinIndex >= m_pDoc->m_nDispDistanceNum)
		return;
	if(nMaxIndex < 0 || nMaxIndex >= m_pDoc->m_nDispDistanceNum)
		return;
	
	int y1=0,y2=0;
	if(m_pDoc->m_sDispDistance[nMinIndex].distance.node_info.show_sides)
		y1 = m_pDoc->m_sDispDistance[nMinIndex].nSecondStationYPos;
	else
		y1 = m_pDoc->m_sDispDistance[nMinIndex].nStationYPos;
	y2 = m_pDoc->m_sDispDistance[nMaxIndex].nStationYPos;
	
	//////////////////////////////////////////////////////////////////////
	int x1,x2;
	x1 = m_pDoc->ConvertTimetoXPos(rec.start_time,rec.start_date,m_pDoc->m_nSchdStatus);
	x2 = m_pDoc->ConvertTimetoXPos(rec.end_time, rec.end_date,m_pDoc->m_nSchdStatus);

	if(x1<= 0 && x2<=0)
		return;
	if(x1<= 0 && x2>0)
		x1 = 0;

	if(x1 >= nPageWidth && x2 > nPageWidth)
		return;
	if(x1 < nPageWidth && x2 >= nPageWidth)
		x2 = nPageWidth;

	int dlt = (m_pDoc->m_AbsInhibitMarkCfg.dlt*(y2-y1))/100;
	m_drawRect.SetRect(x1, y1+dlt, x2, y2-dlt);
	if(IsLineLinePosTop(rec.line))
		m_drawRect.OffsetRect(0, -dlt+2);
	else if(IsLineLinePosBottom(rec.line))
		m_drawRect.OffsetRect(0, dlt-2);
	
	m_drawRect.OffsetRect (m_pDoc->x_offset_print,m_pDoc->y_offset_print);
	
	CPen mPen,*pOldPen;
	CRect rect;
	UINT nPreviousMode = pCDC->SetROP2(R2_COPYPEN); 
	UINT nOldMode = pCDC->SetBkMode(TRANSPARENT);
	UINT nOldAlign = pCDC->SetTextAlign(TA_CENTER|TA_BOTTOM);
	

	mPen.CreatePen(PS_SOLID,scalerate,m_pDoc->m_AbsInhibitMarkCfg.color1);
	pOldPen = pCDC->SelectObject(&mPen);
			
	int m_nLogPixelSy = pCDC->GetDeviceCaps(LOGPIXELSY);
	if(m_nLogPixelSy >300)
		m_nLogPixelSy = 400;

	LOGFONT stFont;
	CFont mFont;
	CFont *pOldFont;
	memset(&stFont,0,sizeof(stFont));
	stFont.lfHeight = MulDiv(8,-m_nLogPixelSy,72);
	stFont.lfWeight = FW_THIN;
	stFont.lfClipPrecision = CLIP_LH_ANGLES;
	mFont.CreateFontIndirect(&stFont);
	pOldFont = pCDC->SelectObject(&mFont);
	
	pCDC->MoveTo(m_drawRect.left, m_drawRect.top);
	pCDC->LineTo(m_drawRect.left ,m_drawRect.bottom);

	pCDC->MoveTo(m_drawRect.left, m_drawRect.CenterPoint().y);
	pCDC->LineTo(m_drawRect.right, m_drawRect.CenterPoint().y);
		
	pCDC->MoveTo(m_drawRect.right, m_drawRect.top);
	pCDC->LineTo(m_drawRect.right ,m_drawRect.bottom);

	char time_char_left[2];
	char time_char_right[2];

	itoa(rec.start_time/60 % 10, time_char_left, 10);
	itoa(rec.end_time/60 % 10, time_char_right, 10);
	
	if(x1 == 0)
		strcpy(time_char_left,"0");
	if(x2 == nPageWidth)
		strcpy(time_char_right,"0");

	UINT nOldColor = pCDC->SetTextColor(m_pDoc->m_AbsInhibitMarkCfg.color1);
	pCDC->TextOut(m_drawRect.left, m_drawRect.top, time_char_left, 1);
	pCDC->TextOut(m_drawRect.right, m_drawRect.top, time_char_right, 1);

	pCDC->TextOut(m_drawRect.left, m_drawRect.bottom + (pCDC->GetTextExtent(time_char_left)).cy, time_char_left, 1);
	pCDC->TextOut(m_drawRect.right, m_drawRect.bottom + (pCDC->GetTextExtent(time_char_right)).cy, time_char_right, 1);
	

	CString strName,strDistance,strDirection; 

	if(IsLineDirectionUp(rec.line))
		strDirection = "上行";
	else if(IsLineDirectionDown(rec.line))
		strDirection = "下行";
	else if(IsLineDirectionDual(rec.line))
		strDirection = "上下行";
	else
		strDirection = "";

	CRect graph_note_rect = m_drawRect;
	
	CString strDCmdno, strReason;
	rec.text[255] = 0;
	m_pDoc->ParseMarkReasonText(CString(rec.text), strDCmdno, strReason);

	CString strKm = GetKiloMDesp();
	if(strKm.IsEmpty())
	{
		strName = strDirection + strReason;		
	}
	else
	{
		strName = strDirection + strKm + strReason;
	}
	
	if(!strDCmdno.IsEmpty())
		strName = "揭示命令" + strDCmdno +", "+ strName;

	pCDC->SetTextColor(m_pDoc->m_AbsInhibitMarkCfg.color3);
	CSize textSize = pCDC->GetTextExtent(strName);
	CPoint myPoint = m_drawRect.CenterPoint();

	CPoint LinePos = m_drawRect.CenterPoint();
	if(IsLineTextPosBottom(rec.line))
		pCDC->TextOut (LinePos.x,LinePos.y + textSize.cy,strName);
	else
		pCDC->TextOut (LinePos.x,LinePos.y,strName);

	if(m_pDoc->m_AbsInhibitMarkCfg.draw) // 绘制方向横线 CUIHU 2011.2.26
	{
		int nStyleCount = 2;
		DWORD StyleArray[] = {15,10};

		LOGBRUSH logBrush;
		logBrush.lbStyle = BS_SOLID;
		logBrush.lbColor = m_pDoc->m_AbsInhibitMarkCfg.color2; 
		logBrush.lbHatch = 0;

		CPen newPen;
		newPen.CreatePen(PS_GEOMETRIC|PS_USERSTYLE, 1,&logBrush,nStyleCount,StyleArray);  

		pOldPen = pCDC->SelectObject(&newPen);

		if(IsLineDirectionDown(rec.line))
		{	
			pCDC->MoveTo(m_drawRect.left, m_drawRect.CenterPoint().y + 6);
			pCDC->LineTo(m_drawRect.right, m_drawRect.CenterPoint().y + 6);
		}
		else
		if(IsLineDirectionUp(rec.line))
		{	
			pCDC->MoveTo(m_drawRect.left, m_drawRect.CenterPoint().y - 6);
			pCDC->LineTo(m_drawRect.right, m_drawRect.CenterPoint().y - 6);
		}

		pCDC->SelectObject(&pOldPen);
		newPen.DeleteObject();
	}


	pCDC->SetTextAlign(nOldAlign);
	pCDC->SetBkMode(nOldMode);
	pCDC->SetTextColor(nOldColor);
	pCDC->SelectObject(pOldPen);
	pCDC->SetROP2(nPreviousMode); 
	pCDC->SelectObject (pOldFont);

	mFont.DeleteObject();
	mPen.DeleteObject();
}

void CPlotAbsMark::DrawAbsSlowBlock(CDC *pCDC,int nPageWidth)
{
	int scalerate = min(m_pDoc->yMultiple,5);
	nPageWidth = m_pDoc->m_fPixelPerMinute * m_pDoc->m_nViewTimeRange*60; 

	if(rec.type != ABS_SLOW_MARK) 
		return;

	int nMinIndex = rec.station;
	int nMaxIndex = rec.index;
	if(nMinIndex>nMaxIndex)
	{
		int tmp = nMinIndex;
		nMinIndex=nMaxIndex;
		nMaxIndex=tmp;
	}
	//added by zhaoliang 20100812 for BLOCK_ELE print erro
	if(nMinIndex < 0 || nMinIndex >= m_pDoc->m_nDispDistanceNum)
		return;
	if(nMaxIndex < 0 || nMaxIndex >= m_pDoc->m_nDispDistanceNum)
		return;

	int y1=0,y2=0;
	if(m_pDoc->m_sDispDistance[nMinIndex].distance.node_info.show_sides)
		y1 = m_pDoc->m_sDispDistance[nMinIndex].nSecondStationYPos;
	else
		y1 = m_pDoc->m_sDispDistance[nMinIndex].nStationYPos;
	y2 = m_pDoc->m_sDispDistance[nMaxIndex].nStationYPos;
	
	//////////////////////////////////////////////////////////////////////
	int x1,x2;
	x1 = m_pDoc->ConvertTimetoXPos(rec.start_time,rec.start_date,m_pDoc->m_nSchdStatus);
	x2 = m_pDoc->ConvertTimetoXPos(rec.end_time, rec.end_date,m_pDoc->m_nSchdStatus);

	if(x1<= 0 && x2<=0)
		return;
	if(x1<= 0 && x2>0)
		x1 = 0;

	if(x1 >= nPageWidth && x2 > nPageWidth)
		return;
	if(x1 < nPageWidth && x2 >= nPageWidth)
		x2 = nPageWidth;

	int dlt = (m_pDoc->m_AbsSlowMarkCfg.dlt*(y2-y1))/100;
	m_drawRect.SetRect(x1, y1+dlt, x2, y2-dlt);
	if(IsLineLinePosTop(rec.line))
	{
		m_drawRect.OffsetRect(0, -dlt+2);
	}
	else if(IsLineLinePosBottom(rec.line))
	{
		m_drawRect.OffsetRect(0, dlt-2);
	}
	
	m_drawRect.OffsetRect (m_pDoc->x_offset_print,m_pDoc->y_offset_print);
	
	UINT nPreviousMode = pCDC->SetROP2(R2_COPYPEN); 
	UINT nOldMode = pCDC->SetBkMode(TRANSPARENT);
	UINT nOldAlign = pCDC->SetTextAlign(TA_CENTER|TA_BOTTOM);
	
	int dashStyleCount = 2;
	DWORD dashStyleArray[2];
	dashStyleArray[0] = 20* scalerate;
	dashStyleArray[1] = 10* scalerate;

	LOGBRUSH dashlogBrush;
	dashlogBrush.lbStyle = BS_SOLID;
	dashlogBrush.lbColor = m_pDoc->m_AbsSlowMarkCfg.color1;
	dashlogBrush.lbHatch = 0;
	
	CPen dashPen, mPen;
	dashPen.CreatePen(PS_GEOMETRIC|PS_USERSTYLE, scalerate,&dashlogBrush,dashStyleCount,dashStyleArray); 

	CPen *pOldPen = pCDC->SelectObject(&dashPen);
			
	int m_nLogPixelSy = pCDC->GetDeviceCaps(LOGPIXELSY);
	if(m_nLogPixelSy >300)
		m_nLogPixelSy = 400;

	LOGFONT stFont;
	CFont mFont;
	CFont *pOldFont;
	memset(&stFont,0,sizeof(stFont));
	stFont.lfHeight = MulDiv(8,-m_nLogPixelSy,72);	  //12
	stFont.lfWeight = FW_THIN;
	stFont.lfClipPrecision = CLIP_LH_ANGLES;
	mFont.CreateFontIndirect(&stFont);
	pOldFont = pCDC->SelectObject(&mFont);

	pCDC->MoveTo(m_drawRect.left, m_drawRect.CenterPoint().y);//横线
	pCDC->LineTo(m_drawRect.right, m_drawRect.CenterPoint().y);

	mPen.CreatePen(PS_SOLID,scalerate,m_pDoc->m_AbsInhibitMarkCfg.color1);
	pCDC->SelectObject(&mPen);

	pCDC->MoveTo(m_drawRect.left, m_drawRect.top);
	pCDC->LineTo(m_drawRect.left ,m_drawRect.bottom);

	pCDC->MoveTo(m_drawRect.right, m_drawRect.top);
	pCDC->LineTo(m_drawRect.right ,m_drawRect.bottom);

	char time_char_left[2];
	char time_char_right[2];

	itoa(rec.start_time/60 % 10, time_char_left, 10);
	itoa(rec.end_time/60 % 10, time_char_right, 10);
	
	if(x1 == 0)
		strcpy(time_char_left,"0");
	if(x2 == nPageWidth)
		strcpy(time_char_right,"0");

	UINT nOldColor = pCDC->SetTextColor(m_pDoc->m_AbsSlowMarkCfg.color1);
	pCDC->TextOut(m_drawRect.left, m_drawRect.top, time_char_left, 1);
	pCDC->TextOut(m_drawRect.right, m_drawRect.top, time_char_right, 1);

	pCDC->TextOut(m_drawRect.left, m_drawRect.bottom + (pCDC->GetTextExtent(time_char_left)).cy, time_char_left, 1);
	pCDC->TextOut(m_drawRect.right, m_drawRect.bottom + (pCDC->GetTextExtent(time_char_right)).cy, time_char_right, 1);
	

	CString strName,strDistance,strDirection; 

	if(IsLineDirectionUp(rec.line))
		strDirection = "上行";
	else if(IsLineDirectionDown(rec.line))
		strDirection = "下行";
	else if(IsLineDirectionDual(rec.line))
		strDirection = "上下行";
	else
		strDirection = "";

	CRect graph_note_rect = m_drawRect;

	CString strDCmdno, strReason;
	rec.text[255] = 0;
	m_pDoc->ParseMarkReasonText(CString(rec.text), strDCmdno, strReason);

	strDistance.Format("限速 %d km/h ",rec.flag*5);

	CString strKm = GetKiloMDesp();
	if(strKm.IsEmpty())
	{
		strName = strDirection + strReason + strDistance;		
	}
	else
	{
		strName = strDirection + strKm + strReason+strDistance;
	}
		
	if(!strDCmdno.IsEmpty())
		strName = "揭示命令" + strDCmdno +", "+ strName;

	CSize textSize = pCDC->GetTextExtent(strName);
	CPoint myPoint = m_drawRect.CenterPoint();

	CPoint LinePos = m_drawRect.CenterPoint();
	pCDC->SetTextColor(m_pDoc->m_AbsSlowMarkCfg.color3);
	if(IsLineTextPosBottom(rec.line))
		pCDC->TextOut (LinePos.x,LinePos.y + textSize.cy,strName);
	else
		pCDC->TextOut (LinePos.x,LinePos.y,strName);

	if(m_pDoc->m_AbsSlowMarkCfg.draw) // 绘制方向横线 CUIHU 2011.2.26
	{
		int nStyleCount = 2;
		DWORD StyleArray[] = {15,10};

		LOGBRUSH logBrush;
		logBrush.lbStyle = BS_SOLID;
		logBrush.lbColor = m_pDoc->m_AbsInhibitMarkCfg.color2; 
		logBrush.lbHatch = 0;

		CPen newPen;
		newPen.CreatePen(PS_GEOMETRIC|PS_USERSTYLE, 1,&logBrush,nStyleCount,StyleArray);  

		pOldPen = pCDC->SelectObject(&newPen);

		if(IsLineDirectionDown(rec.line))
		{	
			pCDC->MoveTo(m_drawRect.left, m_drawRect.CenterPoint().y + 6);
			pCDC->LineTo(m_drawRect.right, m_drawRect.CenterPoint().y + 6);
		}
		else
		if(IsLineDirectionUp(rec.line))
		{	
			pCDC->MoveTo(m_drawRect.left, m_drawRect.CenterPoint().y - 6);
			pCDC->LineTo(m_drawRect.right, m_drawRect.CenterPoint().y - 6);
		}

		pCDC->SelectObject(&pOldPen);
		newPen.DeleteObject();
	}

	pCDC->SetTextAlign(nOldAlign);
	pCDC->SetBkMode(nOldMode);
	pCDC->SetTextColor(nOldColor);
	pCDC->SelectObject(pOldPen);
	pCDC->SetROP2(nPreviousMode); 
	pCDC->SelectObject (pOldFont);

	mFont.DeleteObject();
	dashPen.DeleteObject();
	mPen.DeleteObject();
}

void CPlotAbsMark::DrawAbsSelfCloseBlock(CDC *pCDC,int nPageWidth)
{
	int scalerate = min(m_pDoc->yMultiple,5);
	nPageWidth = m_pDoc->m_fPixelPerMinute * m_pDoc->m_nViewTimeRange*60; 

	if(rec.type != SELF_CLOSE_MARK) 
		return;

	int nMinIndex = rec.station;
	int nMaxIndex = rec.index;
	if(nMinIndex>nMaxIndex)
	{
		int tmp = nMinIndex;
		nMinIndex=nMaxIndex;
		nMaxIndex=tmp;
	}

	//added by zhaoliang 20100812 for BLOCK_ELE print erro
	if(nMinIndex < 0 || nMinIndex >= m_pDoc->m_nDispDistanceNum)
		return;
	if(nMaxIndex < 0 || nMaxIndex >= m_pDoc->m_nDispDistanceNum)
		return;

	mColor = CLR_RED;

	int y1=0,y2=0;
	if(m_pDoc->m_sDispDistance[nMinIndex].distance.node_info.show_sides)
		y1 = m_pDoc->m_sDispDistance[nMinIndex].nSecondStationYPos;
	else
		y1 = m_pDoc->m_sDispDistance[nMinIndex].nStationYPos;
	y2 = m_pDoc->m_sDispDistance[nMaxIndex].nStationYPos;
	
	//////////////////////////////////////////////////////////////////////
	int x1,x2;
	x1 = m_pDoc->ConvertTimetoXPos(rec.start_time,rec.start_date,m_pDoc->m_nSchdStatus);
	x2 = m_pDoc->ConvertTimetoXPos(rec.end_time, rec.end_date,m_pDoc->m_nSchdStatus);

	if(x1<= 0 && x2<=0)
		return;
	if(x1<= 0 && x2>0)
		x1 = 0;

	if(x1 >= nPageWidth && x2 > nPageWidth)
		return;
	if(x1 < nPageWidth && x2 >= nPageWidth)
		x2 = nPageWidth;

	int dlt = (m_pDoc->m_SelfCloseMarkCfg.dlt*(y2-y1))/100;
	m_drawRect.SetRect(x1, y1+dlt, x2, y2-dlt);
	if(IsLineLinePosTop(rec.line))
		m_drawRect.OffsetRect(0, -dlt+2);
	else if(IsLineLinePosBottom(rec.line))
		m_drawRect.OffsetRect(0, dlt-2);
	
	m_drawRect.OffsetRect (m_pDoc->x_offset_print,m_pDoc->y_offset_print);
	
	CPen mPen,*pOldPen;
	CRect rect;
	UINT nPreviousMode = pCDC->SetROP2(R2_COPYPEN); 
	UINT nOldMode = pCDC->SetBkMode(TRANSPARENT);
	UINT nOldAlign = pCDC->SetTextAlign(TA_CENTER|TA_BOTTOM);
	
	mPen.CreatePen(PS_SOLID,scalerate, m_pDoc->m_SelfCloseMarkCfg.color1);
	pOldPen = pCDC->SelectObject(&mPen);
			
	int m_nLogPixelSy = pCDC->GetDeviceCaps(LOGPIXELSY);
	if(m_nLogPixelSy >300)
		m_nLogPixelSy = 400;

	LOGFONT stFont;
	CFont mFont;
	CFont *pOldFont;
	memset(&stFont,0,sizeof(stFont));
	stFont.lfHeight = MulDiv(8,-m_nLogPixelSy,72);
	stFont.lfWeight = FW_THIN;
	stFont.lfClipPrecision = CLIP_LH_ANGLES;
	mFont.CreateFontIndirect(&stFont);
	pOldFont = pCDC->SelectObject(&mFont);
	
	pCDC->MoveTo(m_drawRect.left, m_drawRect.top);
	pCDC->LineTo(m_drawRect.left ,m_drawRect.bottom);

	pCDC->MoveTo(m_drawRect.left, m_drawRect.CenterPoint().y);
	pCDC->LineTo(m_drawRect.right, m_drawRect.CenterPoint().y);
		
	pCDC->MoveTo(m_drawRect.right, m_drawRect.top);
	pCDC->LineTo(m_drawRect.right ,m_drawRect.bottom);

	char time_char_left[2];
	char time_char_right[2];

	itoa(rec.start_time/60 % 10, time_char_left, 10);
	itoa(rec.end_time/60 % 10, time_char_right, 10);
	
	if(x1 == 0)
		strcpy(time_char_left,"0");
	if(x2 == nPageWidth)
		strcpy(time_char_right,"0");

	UINT nOldColor = pCDC->SetTextColor(m_pDoc->m_SelfCloseMarkCfg.color1);
	pCDC->TextOut(m_drawRect.left, m_drawRect.top, time_char_left, 1);
	pCDC->TextOut(m_drawRect.right, m_drawRect.top, time_char_right, 1);

	pCDC->TextOut(m_drawRect.left, m_drawRect.bottom + (pCDC->GetTextExtent(time_char_left)).cy, time_char_left, 1);
	pCDC->TextOut(m_drawRect.right, m_drawRect.bottom + (pCDC->GetTextExtent(time_char_right)).cy, time_char_right, 1);
	

	CString strName,strDistance,strDirection; 

	if(IsLineDirectionUp(rec.line))
		strDirection = "上行";
	else if(IsLineDirectionDown(rec.line))
		strDirection = "下行";
	else if(IsLineDirectionDual(rec.line))
		strDirection = "上下行";
	else
		strDirection = "";

	CRect graph_note_rect = m_drawRect;
	CString s_kilo,s_meter,e_kilo,e_meter,start_time;

	CString strDCmdno, strReason;
	rec.text[255] = 0;
	m_pDoc->ParseMarkReasonText(CString(rec.text), strDCmdno, strReason);

	start_time  = (CString)OsConvertTimetoString(rec.start_time);
	strDistance.Format("%d",rec.position);
	s_meter.Format("%d",rec.s_meter);
	e_meter.Format("%d",rec.e_meter);
	e_kilo.Format("%d",rec.e_kilo);

	if(rec.position == 0 && rec.e_meter == 0
		&& rec.e_meter == 0 && rec.e_kilo == 0)
	{
		strName = strDirection + strReason;
	}
	else
	{
		strName = strDirection + " K" + strDistance + "+" +s_meter + "m"
			" ~ K" + e_kilo + "+" +e_meter + "m "+ strReason;			
	}

	CString strBlockType;
	if(rec.flag == 0)
	{
		strBlockType = " 半自动闭塞";
	}
	else if(rec.flag == 1)
	{
		strBlockType = " 自动闭塞";
	}
	else if(rec.flag == 2)
	{
		strBlockType = " 电话闭塞";
	}
	else if(rec.flag == 3)
	{
		strBlockType = " 停基改电";
	}
	else if(rec.flag == 4)
	{
		strBlockType = " 停用基本闭塞";
	}
	else
	{
		strBlockType = "";
	}
	
	if(!strDCmdno.IsEmpty())
		strName = "揭示命令" + strDCmdno +", "+ strName;
	strName += strBlockType;

	pCDC->SetTextColor(m_pDoc->m_SelfCloseMarkCfg.color3);
	CSize textSize = pCDC->GetTextExtent(strName);
	CPoint myPoint = m_drawRect.CenterPoint();

	CPoint LinePos = m_drawRect.CenterPoint();
	if(IsLineTextPosBottom(rec.line))
		pCDC->TextOut (LinePos.x,LinePos.y + textSize.cy,strName);
	else
		pCDC->TextOut (LinePos.x,LinePos.y,strName);

	pCDC->SetTextAlign(nOldAlign);
	pCDC->SetBkMode(nOldMode);
	pCDC->SetTextColor(nOldColor);
	pCDC->SelectObject(pOldPen);
	pCDC->SetROP2(nPreviousMode); 
	pCDC->SelectObject (pOldFont);

	mFont.DeleteObject();
	mPen.DeleteObject();
}

void CPlotAbsMark::DrawAbsZhFix(CDC *pCDC,int nPageWidth)
{
	int scalerate = min(m_pDoc->yMultiple,5);

	nPageWidth = m_pDoc->m_fPixelPerMinute * m_pDoc->m_nViewTimeRange*60; 

	if(rec.type != ABS_FIX_MARK) 
		return;

	int	nMinIndex = rec.station;
	int	nMaxIndex = rec.index;
	if(nMinIndex>nMaxIndex)
	{
		int tmp = nMinIndex;
		nMinIndex=nMaxIndex;
		nMaxIndex=tmp;
	}
	//added by zhaoliang 20100812 for BLOCK_ELE print erro
	if(nMinIndex < 0 || nMinIndex >= m_pDoc->m_nDispDistanceNum)
		return;
	if(nMaxIndex < 0 || nMaxIndex >= m_pDoc->m_nDispDistanceNum)
		return;
	

	int y1=0,y2=0;
	
	if(m_pDoc->m_sDispDistance[nMinIndex].distance.node_info.show_sides)
		y1 = m_pDoc->m_sDispDistance[nMinIndex].nSecondStationYPos;
	else
		y1 = m_pDoc->m_sDispDistance[nMinIndex].nStationYPos;
	y2 = m_pDoc->m_sDispDistance[nMaxIndex].nStationYPos;
	

	//////////////////////////////////////////////////////////////////////
	int x1,x2;
	x1 = m_pDoc->ConvertTimetoXPos(rec.start_time,rec.start_date,m_pDoc->m_nSchdStatus);
	x2 = m_pDoc->ConvertTimetoXPos(rec.end_time, rec.end_date,m_pDoc->m_nSchdStatus);

	if(x1<= 0 && x2<=0)
		return;
	if(x1<= 0 && x2>0)
		x1 = 0;

	if(x1 >= nPageWidth && x2 > nPageWidth)
		return;
	if(x1 < nPageWidth && x2 >= nPageWidth)
		x2 = nPageWidth;

	//选择了封锁上方站
	if(rec.slow_time&0x0001)
	{
		y1 = m_pDoc->m_sDispDistance[nMinIndex].nStationYPos;
	}

	//选择了封锁下方站	
	if(rec.slow_time&0x0002)
	{
		if(m_pDoc->m_sDispDistance[nMaxIndex].distance.node_info.show_sides)
			y2 = m_pDoc->m_sDispDistance[nMaxIndex].nSecondStationYPos;
		else
			y2 = m_pDoc->m_sDispDistance[nMaxIndex].nStationYPos;
	}
	if(rec.slow_time&0x0001)
		y1 -= 4*scalerate;
	else
		y1 += 4*scalerate;

	if(rec.slow_time&0x0002)
		y2 += 4*scalerate;
	else 
		y2 -= 4*scalerate;

	m_drawRect.SetRect(x1, y1, x2, y2);
	m_drawRect.OffsetRect (m_pDoc->x_offset_print,m_pDoc->y_offset_print);
	
	UINT nPreviousMode = pCDC->SetROP2(R2_COPYPEN); 
	UINT nOldMode = pCDC->SetBkMode(TRANSPARENT);
	UINT nOldAlign = pCDC->SetTextAlign(TA_CENTER|TA_BOTTOM);
	
	CPen mPen;
	mPen.CreatePen(PS_SOLID,scalerate,m_pDoc->m_AbsZhFixMarkCfg.color1);
	
	int m_nLogPixelSy = pCDC->GetDeviceCaps(LOGPIXELSY);
	if(m_nLogPixelSy >300)
		m_nLogPixelSy = 400;

	LOGFONT stFont;
	memset(&stFont,0,sizeof(stFont));
	stFont.lfHeight = MulDiv(8,-m_nLogPixelSy,72);	  //12
	stFont.lfWeight = FW_THIN;
	stFont.lfClipPrecision = CLIP_LH_ANGLES;

	CFont mFont;
	mFont.CreateFontIndirect(&stFont);

	CPen* pOldPen = pCDC->SelectObject(&mPen);
	CFont *pOldFont = pCDC->SelectObject(&mFont);

	FrameRectExtend(pCDC,m_drawRect);

	char time_char_left[2];
	char time_char_right[2];

	itoa(rec.start_time/60 % 10, time_char_left, 10);
	itoa(rec.end_time/60 % 10, time_char_right, 10);

	if(x1 == 0)
		strcpy(time_char_left,"0");
	if(x2 == nPageWidth)
		strcpy(time_char_right,"0");

	UINT nOldColor = pCDC->SetTextColor(m_pDoc->m_AbsZhFixMarkCfg.color1);
	pCDC->TextOut(m_drawRect.left, m_drawRect.top, time_char_left, 1);
	pCDC->TextOut(m_drawRect.right, m_drawRect.top, time_char_right, 1);

	pCDC->TextOut(m_drawRect.left, m_drawRect.bottom + (pCDC->GetTextExtent(time_char_left)).cy, time_char_left, 1);
	pCDC->TextOut(m_drawRect.right, m_drawRect.bottom + (pCDC->GetTextExtent(time_char_right)).cy, time_char_right, 1);

	CString strName,strDistance,strDirection; 

	if(IsLineDirectionUp(rec.line))
		strDirection = "上行";
	else if(IsLineDirectionDown(rec.line))
		strDirection = "下行";
	else if(IsLineDirectionDual(rec.line))
		strDirection = "上下行";
	else
		strDirection = "";

	CString strDCmdno, strReason;
	rec.text[255] = 0;
	m_pDoc->ParseMarkReasonText(CString(rec.text), strDCmdno, strReason);
	strName = "";
	if(!strDCmdno.IsEmpty())
		strName = "揭示命令" + strDCmdno;
	
	CString strKm = GetKiloMDesp();
	if(strKm.IsEmpty())
	{
		strName = strName + strDirection +  strReason;		
	}
	else
	{
		strName = strName + strDirection + strKm + strReason;
	}

	pCDC->SetTextColor(m_pDoc->m_AbsZhFixMarkCfg.color3);
	CSize textSize = pCDC->GetTextExtent(strName);
	CPoint myPoint = m_drawRect.CenterPoint();

	if(IsLineLinePosTop(rec.line))
		pCDC->TextOut (myPoint.x,myPoint.y - textSize.cy,strName);
	else if(IsLineLinePosBottom(rec.line))
		pCDC->TextOut (myPoint.x,myPoint.y + 2*textSize.cy,strName);
	else
		pCDC->TextOut (myPoint.x,myPoint.y,strName);
	
	pCDC->SetTextAlign(nOldAlign);
	pCDC->SetBkMode(nOldMode);
	pCDC->SetTextColor(nOldColor);
	pCDC->SelectObject(pOldPen);
	pCDC->SetROP2(nPreviousMode); 
	pCDC->SelectObject (pOldFont);

	mFont.DeleteObject();
	mPen.DeleteObject();
}

void CPlotAbsMark::DrawAbsEleFix(CDC *pCDC,int nPageWidth)
{
	int scalerate = min(m_pDoc->yMultiple,5);

	nPageWidth = m_pDoc->m_fPixelPerMinute * m_pDoc->m_nViewTimeRange*60; 

	if(rec.type != ABS_ELE_FIX_MARK) 
		return;

	int	nMinIndex = rec.station;
	int	nMaxIndex = rec.index;
	if(nMinIndex>nMaxIndex)
	{
		int tmp = nMinIndex;
		nMinIndex=nMaxIndex;
		nMaxIndex=tmp;
	}

	//added by zhaoliang 20100812 for BLOCK_ELE print erro
	if(nMinIndex < 0 || nMinIndex >= m_pDoc->m_nDispDistanceNum)
		return;
	if(nMaxIndex < 0 || nMaxIndex >= m_pDoc->m_nDispDistanceNum)
		return;
	

	int y1=0,y2=0;
	
	if(m_pDoc->m_sDispDistance[nMinIndex].distance.node_info.show_sides)
		y1 = m_pDoc->m_sDispDistance[nMinIndex].nSecondStationYPos;
	else
		y1 = m_pDoc->m_sDispDistance[nMinIndex].nStationYPos;
	y2 = m_pDoc->m_sDispDistance[nMaxIndex].nStationYPos;
	

	//////////////////////////////////////////////////////////////////////
	int x1,x2;
	x1 = m_pDoc->ConvertTimetoXPos(rec.start_time,rec.start_date,m_pDoc->m_nSchdStatus);
	x2 = m_pDoc->ConvertTimetoXPos(rec.end_time, rec.end_date,m_pDoc->m_nSchdStatus);

	if(x1<= 0 && x2<=0)
		return;
	if(x1<= 0 && x2>0)
		x1 = 0;

	if(x1 >= nPageWidth && x2 > nPageWidth)
		return;
	if(x1 < nPageWidth && x2 >= nPageWidth)
		x2 = nPageWidth;

	//选择了封锁上方站
	if(rec.slow_time&0x0001)
	{
		y1 = m_pDoc->m_sDispDistance[nMinIndex].nStationYPos;
	}

	//选择了封锁下方站	
	if(rec.slow_time&0x0002)
	{
		if(m_pDoc->m_sDispDistance[nMaxIndex].distance.node_info.show_sides)
			y2 = m_pDoc->m_sDispDistance[nMaxIndex].nSecondStationYPos;
		else
			y2 = m_pDoc->m_sDispDistance[nMaxIndex].nStationYPos;
	}
	if(rec.slow_time&0x0001)
		y1 -= 4*scalerate;
	else
		y1 += 4*scalerate;

	if(rec.slow_time&0x0002)
		y2 += 4*scalerate;
	else 
		y2 -= 4*scalerate;

	m_drawRect.SetRect(x1, y1, x2, y2);
	m_drawRect.OffsetRect (m_pDoc->x_offset_print,m_pDoc->y_offset_print);
	
	UINT nPreviousMode = pCDC->SetROP2(R2_COPYPEN); 
	UINT nOldMode = pCDC->SetBkMode(TRANSPARENT);
	UINT nOldAlign = pCDC->SetTextAlign(TA_CENTER|TA_BOTTOM);
	
	CPen mPen;
	mPen.CreatePen(PS_SOLID,scalerate,m_pDoc->m_AbsEleFixMarkCfg.color1);
	
	int m_nLogPixelSy = pCDC->GetDeviceCaps(LOGPIXELSY);
	if(m_nLogPixelSy >300)
		m_nLogPixelSy = 400;

	LOGFONT stFont;
	memset(&stFont,0,sizeof(stFont));
	stFont.lfHeight = MulDiv(8,-m_nLogPixelSy,72);	  //12
	stFont.lfWeight = FW_THIN;
	stFont.lfClipPrecision = CLIP_LH_ANGLES;

	CFont mFont;
	mFont.CreateFontIndirect(&stFont);

	CPen* pOldPen = pCDC->SelectObject(&mPen);
	CFont *pOldFont = pCDC->SelectObject(&mFont);

	FrameRectExtend(pCDC,m_drawRect);

	char time_char_left[2];
	char time_char_right[2];

	itoa(rec.start_time/60 % 10, time_char_left, 10);
	itoa(rec.end_time/60 % 10, time_char_right, 10);

	if(x1 == 0)
		strcpy(time_char_left,"0");
	if(x2 == nPageWidth)
		strcpy(time_char_right,"0");

	UINT nOldColor = pCDC->SetTextColor(m_pDoc->m_AbsEleFixMarkCfg.color1);
	pCDC->TextOut(m_drawRect.left, m_drawRect.top, time_char_left, 1);
	pCDC->TextOut(m_drawRect.right, m_drawRect.top, time_char_right, 1);

	pCDC->TextOut(m_drawRect.left, m_drawRect.bottom + (pCDC->GetTextExtent(time_char_left)).cy, time_char_left, 1);
	pCDC->TextOut(m_drawRect.right, m_drawRect.bottom + (pCDC->GetTextExtent(time_char_right)).cy, time_char_right, 1);

	CString strName,strDistance,strDirection; 

	if(IsLineDirectionUp(rec.line))
		strDirection = "上行";
	else if(IsLineDirectionDown(rec.line))
		strDirection = "下行";
	else if(IsLineDirectionDual(rec.line))
		strDirection = "上下行";
	else
		strDirection = "";

	CString strDCmdno, strReason;
	rec.text[255] = 0;
	m_pDoc->ParseMarkReasonText(CString(rec.text), strDCmdno, strReason);
	strName = "";
	if(!strDCmdno.IsEmpty())
		strName = "揭示命令" + strDCmdno;
	
	CString strKm = GetKiloMDesp();
	if(strKm.IsEmpty())
	{
		strName = strName + strDirection +  strReason;		
	}
	else
	{
		strName = strName + strDirection + strKm + strReason;
	}

	pCDC->SetTextColor(m_pDoc->m_AbsEleFixMarkCfg.color3);
	CSize textSize = pCDC->GetTextExtent(strName);
	CPoint myPoint = m_drawRect.CenterPoint();

	if(IsLineLinePosTop(rec.line))
		pCDC->TextOut (myPoint.x,myPoint.y - textSize.cy,strName);
	else if(IsLineLinePosBottom(rec.line))
		pCDC->TextOut (myPoint.x,myPoint.y + 2*textSize.cy,strName);
	else
		pCDC->TextOut (myPoint.x,myPoint.y,strName);
	
	pCDC->SetTextAlign(nOldAlign);
	pCDC->SetBkMode(nOldMode);
	pCDC->SetTextColor(nOldColor);
	pCDC->SelectObject(pOldPen);
	pCDC->SetROP2(nPreviousMode); 
	pCDC->SelectObject (pOldFont);

	mFont.DeleteObject();
	mPen.DeleteObject();
}

void CPlotAbsMark::DrawLanzhouAbsEleFix(CDC *pCDC,int nPageWidth)
{
	int scalerate = min(m_pDoc->yMultiple,5);

	nPageWidth = m_pDoc->m_fPixelPerMinute * m_pDoc->m_nViewTimeRange*60; 

	if(rec.type != NEW_ABS_ELE_FIX_MARK) 
		return;

	int	nMinIndex = rec.station;
	int	nMaxIndex = rec.index;
	if(nMinIndex>nMaxIndex)
	{
		int tmp = nMinIndex;
		nMinIndex=nMaxIndex;
		nMaxIndex=tmp;
	}

	//added by zhaoliang 20100812 for BLOCK_ELE print erro
	if(nMinIndex < 0 || nMinIndex >= m_pDoc->m_nDispDistanceNum)
		return;
	if(nMaxIndex < 0 || nMaxIndex >= m_pDoc->m_nDispDistanceNum)
		return;
	

	int y1=0,y2=0;
	
	y1 = m_pDoc->m_sDispDistance[nMinIndex].nSecondStationYPos;
	y2 = m_pDoc->m_sDispDistance[nMaxIndex].nStationYPos;

	
	//////////////////////////////////////////////////////////////////////
	int x1,x2;
	x1 = m_pDoc->ConvertTimetoXPos(rec.start_time,rec.start_date,m_pDoc->m_nSchdStatus);
	x2 = m_pDoc->ConvertTimetoXPos(rec.end_time, rec.end_date,m_pDoc->m_nSchdStatus);

	if(x1<= 0 && x2<=0)
		return;
	if(x1<= 0 && x2>0)
		x1 = 0;

	if(x1 >= nPageWidth && x2 > nPageWidth)
		return;
	if(x1 < nPageWidth && x2 >= nPageWidth)
		x2 = nPageWidth;

	//选择了封锁上方站
	if(rec.slow_time&0x0001)
	{
		y1 = m_pDoc->m_sDispDistance[nMinIndex].nStationYPos;
	}

	//选择了封锁下方站	
	if(rec.slow_time&0x0002)
	{
		y2 = m_pDoc->m_sDispDistance[nMaxIndex].nSecondStationYPos;
	}

	int dlt = m_pDoc->m_AbsEleFixMarkCfg.dlt;

	if(rec.slow_time&0x0001)
	{
		y1 -= 4*scalerate;
	}
	else
	{	
		if((rec.slow_time&0x0008) && (nMinIndex < m_pDoc->m_nDispDistanceNum-1))//上远
		{
			int TmpY = m_pDoc->m_sDispDistance[nMinIndex+1].nStationYPos;
			y1=y1+(abs(TmpY-y1)*dlt)/100;//上远
		}
		else
		{
			y1 += 4*scalerate;
		}
	}
	if(rec.slow_time&0x0002)
	{
		y2 += 4*scalerate;
	}
	else 
	{
		if((rec.slow_time&0x0004) && (nMaxIndex > 0))//下远
		{
			int TmpY = m_pDoc->m_sDispDistance[nMaxIndex-1].nSecondStationYPos;
			y2=y2-(abs(y2-TmpY)*dlt)/100;//下远
		}
		else
		{
			y2 -= 4*scalerate;
		}
	}
	m_drawRect.SetRect(x1, y1, x2, y2);
	m_drawRect.OffsetRect (m_pDoc->x_offset_print,m_pDoc->y_offset_print);
	
	UINT nPreviousMode = pCDC->SetROP2(R2_COPYPEN); 
	UINT nOldMode = pCDC->SetBkMode(TRANSPARENT);
	UINT nOldAlign = pCDC->SetTextAlign(TA_CENTER|TA_BOTTOM);
	
	CPen mPen;
	mPen.CreatePen(PS_SOLID,scalerate,m_pDoc->m_AbsEleFixMarkCfg.color1);
	
	int m_nLogPixelSy = pCDC->GetDeviceCaps(LOGPIXELSY);
	if(m_nLogPixelSy >300)
		m_nLogPixelSy = 400;

	LOGFONT stFont;
	memset(&stFont,0,sizeof(stFont));
	stFont.lfHeight = MulDiv(8,-m_nLogPixelSy,72);	  //12
	stFont.lfWeight = FW_THIN;
	stFont.lfClipPrecision = CLIP_LH_ANGLES;

	CFont mFont;
	mFont.CreateFontIndirect(&stFont);

	CPen* pOldPen = pCDC->SelectObject(&mPen);
	CFont *pOldFont = pCDC->SelectObject(&mFont);

	FrameRectExtend(pCDC,m_drawRect);

	char time_char_left[2];
	char time_char_right[2];

	itoa(rec.start_time/60 % 10, time_char_left, 10);
	itoa(rec.end_time/60 % 10, time_char_right, 10);

	if(x1 == 0)
		strcpy(time_char_left,"0");
	if(x2 == nPageWidth)
		strcpy(time_char_right,"0");

	UINT nOldColor = pCDC->SetTextColor(m_pDoc->m_AbsEleFixMarkCfg.color1);
	pCDC->TextOut(m_drawRect.left, m_drawRect.top, time_char_left, 1);
	pCDC->TextOut(m_drawRect.right, m_drawRect.top, time_char_right, 1);

	pCDC->TextOut(m_drawRect.left, m_drawRect.bottom + (pCDC->GetTextExtent(time_char_left)).cy, time_char_left, 1);
	pCDC->TextOut(m_drawRect.right, m_drawRect.bottom + (pCDC->GetTextExtent(time_char_right)).cy, time_char_right, 1);

	CString strName,strDistance,strDirection; 

	if(IsLineDirectionUp(rec.line))
		strDirection = "上行";
	else if(IsLineDirectionDown(rec.line))
		strDirection = "下行";
	else if(IsLineDirectionDual(rec.line))
		strDirection = "上下行";
	else
		strDirection = "";

	CString strDCmdno, strReason;
	rec.text[255] = 0;
	m_pDoc->ParseMarkReasonText(CString(rec.text), strDCmdno, strReason);
	strName = "";
	if(!strDCmdno.IsEmpty())
		strName = "揭示命令" + strDCmdno;
	
	CString strKm = GetKiloMDesp();
	if(strKm.IsEmpty())
	{
		strName = strName + strDirection +  strReason;		
	}
	else
	{
		strName = strName + strDirection + strKm + strReason;
	}

	pCDC->SetTextColor(m_pDoc->m_AbsEleFixMarkCfg.color3);
	CSize textSize = pCDC->GetTextExtent(strName);
	CPoint myPoint = m_drawRect.CenterPoint();

	if(IsLineLinePosTop(rec.line))
		pCDC->TextOut (myPoint.x,myPoint.y - textSize.cy,strName);
	else if(IsLineLinePosBottom(rec.line))
		pCDC->TextOut (myPoint.x,myPoint.y + 2*textSize.cy,strName);
	else
		pCDC->TextOut (myPoint.x,myPoint.y,strName);
	
	pCDC->SetTextAlign(nOldAlign);
	pCDC->SetBkMode(nOldMode);
	pCDC->SetTextColor(nOldColor);
	pCDC->SelectObject(pOldPen);
	pCDC->SetROP2(nPreviousMode); 
	pCDC->SelectObject (pOldFont);

	mFont.DeleteObject();
	mPen.DeleteObject();
}
