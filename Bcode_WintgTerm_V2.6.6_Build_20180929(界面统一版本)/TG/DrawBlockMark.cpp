#include "stdafx.h"
#include "math.h"
#include "tgdoc.h"

void CTGDoc::DrawSingleMark(CTg_BLOCK& block, CDC *pDC)
{
	if(!m_bAcceptCadData)
		return;

	if (IsOnlyShowTrain()) 
	{
		return;
	}

	int nDirect=0;
	//电网检修，区间缓行，区间封锁，区间检修进行上行行的判断
	if(block.type == ABS_ELE_FIX_MARK || block.type == ABS_SLOW_MARK 
		|| block.type == ABS_INHIBIT_MARK || block.type == ABS_FIX_MARK
		||  block.type ==SELF_CLOSE_MARK || block.type == NEW_ABS_ELE_FIX_MARK)
	{
		int tmp = 0x0003 & block.line;
		switch(tmp)
		{
		case 0:
			nDirect = 1; //上行
			break;
		case 1:
			nDirect = 2; //下行
			break;
		case 2:
			nDirect = 3; //上下行
			break;
		default:
			nDirect = 0; //单线,始终显示
		}

		if( (nDirect != 0) && (nDirect & this->GetShowTrainDirect()) == 0)
			return;
	}

	UINT nPreviousMode = pDC->SetROP2(R2_COPYPEN); 
	UINT nOldMode = pDC->SetBkMode(TRANSPARENT);

	// 判断施工符号时间非法
	if(block.start_time>block.end_time)
	{
		long tmp=block.end_time;
		block.end_time=block.start_time;
		block.start_time=tmp;

		this->SendUpdateMark(block);
		return;
	}
	else if(block.start_time==block.end_time)
	{
		block.end_time=block.start_time+15*60;

		this->SendUpdateMark(block);
		return;
	}

	switch(block.type)
	{
		//----------------------ABS-----------------------
	case ABS_INHIBIT_MARK:
		{
			// G9
			if(this->m_ptgctrl->mConfigTg.m_context.IsDrawLanzhouAbsInhibitMark())
				DrawLanzhouAbsInhibitMark(block, pDC, nDirect);
			else
				DrawAbsInhibitMark(block, pDC, nDirect);
		}
		break;
	case ABS_SLOW_MARK:
		{
			// G9
			if(this->m_ptgctrl->mConfigTg.m_context.IsDrawLanzhouAbsSlowMark())
				DrawLanzhouAbsSlowMark(block, pDC, nDirect);
			else
				DrawAbsSlowMark(block, pDC, nDirect);
		}
		break;
	case ABS_FIX_MARK:
		DrawAbsZhFixMark(block, pDC, nDirect);
		break;
	case NEW_ABS_ELE_FIX_MARK:
		DrawAbsLanzhouEleFixMark(block, pDC, nDirect);//zhaoliang
		break;
	case ABS_ELE_FIX_MARK:
		DrawAbsEleFixMark(block, pDC, nDirect);
		break;
	case SELF_CLOSE_MARK:
		DrawAbsSelfCloseMark(block, pDC, nDirect);
		break;
		//---------------------Side-----------------------
	case SIDE_INHIBIT_MARK:
		DrawSideInhibitMark(block, pDC);
		break;
	case SIDE_SLOW_MARK:
		DrawSideSlowMark(block, pDC);
		break;
	case SIDE_OCCUPY_MARK:
		DrawSideOccupyMark(block, pDC);
		break;
	case SIDE_TIANCHUANG_MARK:
		DrawSideTianchuangMark(block, pDC);
		break;
		//---------------------Text-----------------------
	case GRAPH_NOTE_MARK:
		DrawNoteTextMark(block, pDC);
		break;
	case NEW_GRAPH_NOTE_MARK:
		DrawNoteTextMark2(block, pDC);
		break;
	default:
		{
			CString info;
			info.Format("未知标记 %d", block.type);
			sysprint(5,0, "施工标志类型未知，删除block[%d], type =%d ",block.mark_index, block.type);
			this->SendDeleteBlockXml(block.mark_index);
		}
	}
	block.m_nUpdateTime=GetBeginTime();
	pDC->SetROP2(nPreviousMode); 
	pDC->SetBkMode(nOldMode);
}

void CTGDoc::DrawAbsInhibitMark(CTg_BLOCK& block, CDC *pDC, int nDirect)
{
	COLORREF color = m_ptgctrl->mConfigTg.m_AbsInhibitMarkCfg.color1;
	
	int tmp = 0x0003 & block.line;
	switch(tmp)
	{
	case 0: //上行
		if(m_ptgctrl->mConfigTg.up_color>0)
			color = m_ptgctrl->mConfigTg.up_color;
		break;
	case 1: //下行
		if(m_ptgctrl->mConfigTg.down_color>0)
			color = m_ptgctrl->mConfigTg.down_color;
		break;
	}

	if (m_ptgctrl->mConfigTg.m_context.IsCanModifyBlockStatus() && IsBlockComplete(block.adjust_status))
		color = m_ptgctrl->mConfigTg.m_AbsInhibitMarkCfg.off_color;

	UINT nMinIndex = (block.station < block.index)?block.station:block.index;
	UINT nMaxIndex = (block.station > block.index)?block.station:block.index;

	if(nMinIndex<0||nMinIndex>=512 || nMaxIndex<0||nMaxIndex>=512)
	{
		sysprint(5,0, "站序1 %d  站序2 %d 越界(0<=index<512), 不显示该施工符号block %d, type %d ", block.index, block.station, block.mark_index, block.type);
		return ;
	}
	
	if(nMinIndex >=this->m_nDispDistanceNum)
	{
		sysprint(5,0, "站序1 %d  站序2 %d 不在有效显示区内, 不显示该施工符号block %d, type %d ", block.index, block.station, block.mark_index, block.type);
		return ;
	}

	if(nMaxIndex >=this->m_nDispDistanceNum)
	{
		nMaxIndex=this->m_nDispDistanceNum-1;
		if(nMinIndex>=nMaxIndex)
		{
			sysprint(5,0, "由于站序1 %d  站序2 %d 不在有效显示区内, 不显示该施工符号block %d, type %d ", block.index, block.station, block.mark_index, block.type);
			return ;
		}
	}

	int y1,y2;
	if(this->m_sDispDistance[nMinIndex].show_sation_sep)
		y1 = this->GetVPosition(nMinIndex, POS_FLAG::DOWN);
	else
		y1 = this->GetVPosition(nMinIndex, POS_FLAG::UP);

	y2 = this->GetVPosition(nMaxIndex, POS_FLAG::UP);
	if(y2>=y1+20)
	{
		y1 += 5;
		y2 -= 5;
	}

	block.station1 = this->m_sDispDistance[nMinIndex].station_number;
	block.station2 = this->m_sDispDistance[nMaxIndex].station_number;

	int x1 = this->GetOffsetMinuteFromGraphBegin(block.start_time)*m_fPixelPerMinute;
	int x2 = this->GetOffsetMinuteFromGraphBegin(block.end_time)*m_fPixelPerMinute;

	if(x1 < mGraphBeginPixel) x1 = mGraphBeginPixel;
	if(x2 > mGraphEndPixel)   x2 = mGraphEndPixel+10;  //放开，太原 修改施工符号起止时间 

	if(x1<0) 
		x1 = 0;
	if(x2<0) 
		x2 = 0;

	if(x1 == 0 && x2 == 0)
		return ;

	block.m_drawRect.left	= x1;
	block.m_drawRect.right  = x2;
	block.m_drawRect.top	= y1;
	block.m_drawRect.bottom = y2;

	int Ymiddle = block.m_drawRect.CenterPoint().y;
	int Xmiddle = block.m_drawRect.CenterPoint().x;
	int Ydlt    = abs(y2-y1)/8;

	CPen nPen, *pOldPen;
	nPen.CreatePen(PS_SOLID, 2, color);  
	pOldPen = pDC->SelectObject(&nPen);
	// 绘制竖线
	pDC->MoveTo(x1,y1);  pDC->LineTo(x1,y2);
	pDC->MoveTo(x2,y1);  pDC->LineTo(x2,y2);

	// 绘制中间横线
	CPoint startp,endp;
	if(IsLineLinePosTop(block.line))
	{
		startp.x = block.m_drawRect.left;
		endp.x = block.m_drawRect.right;
		startp.y = endp.y = block.m_drawRect.top + abs(block.m_drawRect.Height())/3;
	}
	else if(IsLineLinePosBottom(block.line))
	{
		startp.x = block.m_drawRect.left;
		endp.x = block.m_drawRect.right;
		startp.y = endp.y = block.m_drawRect.top + (2*abs(block.m_drawRect.Height()))/3;
	}
	else 
	{
		startp.x = block.m_drawRect.left;
		endp.x = block.m_drawRect.right;
		startp.y = endp.y = block.m_drawRect.top + abs(block.m_drawRect.Height())/2;
	}
	pDC->MoveTo(startp);  pDC->LineTo(endp);

	// 绘制方向横线
	if(this->m_ptgctrl->mConfigTg.m_AbsInhibitMarkCfg.draw == 1)
	{
		int nStyleCount = 2;
		DWORD StyleArray[] = {15,10};

		LOGBRUSH logBrush;
		logBrush.lbStyle = BS_SOLID;
		logBrush.lbColor = m_ptgctrl->mConfigTg.m_AbsInhibitMarkCfg.color2; 
		logBrush.lbHatch = 0;

		CPen nPen;
		nPen.CreatePen(PS_GEOMETRIC|PS_USERSTYLE, 1,&logBrush,nStyleCount,StyleArray);  

		pOldPen = pDC->SelectObject(&nPen);

		if(nDirect == 2) 
		{	
			pDC->MoveTo(startp.x, startp.y + 6);
			pDC->LineTo(endp.x, endp.y + 6);
		}
		else if(nDirect == 1)
		{	
			pDC->MoveTo(startp.x, startp.y - 6);
			pDC->LineTo(endp.x, endp.y - 6);
		}

		pDC->SelectObject(pOldPen);
		nPen.DeleteObject();
	}

	pDC->SelectObject(pOldPen);
	nPen.DeleteObject();

	block.cRect.left  = Xmiddle-5;
	block.cRect.right = Xmiddle+5;
	block.cRect.top		= endp.y-5;
	block.cRect.bottom  = endp.y+5;

	block.lRect.left  = x1-5;
	block.lRect.right = x1+5;
	block.lRect.top		= endp.y-5;
	block.lRect.bottom  = endp.y+5;

	block.rRect.left  = x2-5;
	block.rRect.right = x2+5;
	block.rRect.top		= endp.y-5;
	block.rRect.bottom  = endp.y+5;

	// 绘制焦点
	if(block.m_bFocus == TRUE)
	{
		CBrush  m_brushBackground;
		m_brushBackground.CreateSolidBrush(color);
		if(block.bMoveCenter == FALSE)
			pDC->FrameRect(block.cRect,&m_brushBackground); 
		else
			pDC->FillRect(block.cRect,&m_brushBackground);
		if(block.bMoveLeft == FALSE)
			pDC->FrameRect(block.lRect,&m_brushBackground); 
		else
			pDC->FillRect(block.lRect,&m_brushBackground);
		if(block.bMoveRight == FALSE)
			pDC->FrameRect(block.rRect,&m_brushBackground); 
		else
			pDC->FillRect(block.rRect,&m_brushBackground);
		m_brushBackground.DeleteObject();
	}

	CString strName;
	CString strDirection=GetABSStrDirectDesp(nDirect, block.ele_number[0], block.count);

	CString dcmdno, reason, head;
	this->ParseMarkReasonText(block.text, dcmdno, reason);
	if(!dcmdno.IsEmpty())
		head = "揭示命令号:" + dcmdno + " ";
	else
		head = "";

	CString strKm = block.GetKiloMDesp();
	if(strKm.IsEmpty())
	{
		strName = head + strDirection + reason;			
	}
	else
	{
		strName = head + strDirection + strKm + reason;
	}

	COLORREF oldTextColor = pDC->SetTextColor(m_ptgctrl->mConfigTg.m_AbsInhibitMarkCfg.color3);
	UINT oldAlign = pDC->SetTextAlign(TA_CENTER|TA_BOTTOM);
	CFont* oldFont = pDC->SelectObject(&mBlockFont);

	CSize sz = pDC->GetTextExtent(strName);
	CRect rect(Xmiddle-sz.cx/2, y1-30, Xmiddle+sz.cx/2, y2+30);
	
	// 输出文本
	if(IsLineTextPosBottom(block.line))
	{
		pDC->TextOut (Xmiddle, block.m_drawRect.bottom, strName);
	}
	else if(IsLineTextPosLeft(block.line))
	{
		pDC->SetTextAlign(TA_LEFT|TA_TOP);
		pDC->TextOut (block.m_drawRect.left, block.m_drawRect.CenterPoint().y, strName);

		rect.left = block.m_drawRect.left;
		rect.right= block.m_drawRect.left+sz.cx;
	}
	else if(IsLineTextPosMiddle(block.line))
	{
		pDC->SetTextAlign(TA_CENTER|TA_TOP);
		pDC->TextOut (block.m_drawRect.CenterPoint().x, block.m_drawRect.CenterPoint().y-sz.cy/2, strName);
	}
	else if(IsLineTextPosRight(block.line))
	{
		pDC->SetTextAlign(TA_RIGHT|TA_TOP);
		pDC->TextOut (block.m_drawRect.right, block.m_drawRect.CenterPoint().y, strName);

		rect.left = block.m_drawRect.right-sz.cx;
		rect.right= block.m_drawRect.right;
	}
	else // IsLineTextPosTop
	{
		pDC->SetTextAlign(TA_CENTER|TA_TOP);
		pDC->TextOut (Xmiddle, block.m_drawRect.top, strName);
	}

	pDC->SetTextAlign(TA_CENTER|TA_BOTTOM);
	// 刷新矩形
	block.myPosition.UnionRect(&block.m_drawRect, &rect);
	if(block.myPosition.left>20)
		block.myPosition.left-=20;
	else
		block.myPosition.left=0;
	block.myPosition.right+=20;

	// 输出时间
	pDC->SelectObject(&mBlockFontTime);
	pDC->SetTextColor(m_ptgctrl->mConfigTg.m_AbsInhibitMarkCfg.color3);
	CString time_char_left;
	CString time_char_right;
	time_char_left.Format("%d", block.start_time/60 % 10);
	time_char_right.Format("%d", block.end_time/60 % 10);

	sz = pDC->GetTextExtent(time_char_left);
	if(this->m_ptgctrl->mConfigTg.m_AbsInhibitMarkCfg.time_pos == 0)
	{
		pDC->TextOut(x1, y1, time_char_left);
		pDC->TextOut(x1, y2+sz.cy, time_char_left);
		pDC->TextOut(x2, y1, time_char_right);
		pDC->TextOut(x2, y2+sz.cy, time_char_right);
	}
	else
	{
		pDC->TextOut(x1-sz.cx, Ymiddle+sz.cy/2, time_char_left);
		pDC->TextOut(x2+sz.cx, Ymiddle+sz.cy/2, time_char_right);
	}
	pDC->SetTextAlign(oldAlign);
	pDC->SetTextColor(oldTextColor);
	pDC->SelectObject(oldFont);
}

void CTGDoc::DrawAbsSlowMark(CTg_BLOCK& block, CDC *pDC, int nDirect)
{
	COLORREF color = m_ptgctrl->mConfigTg.m_AbsSlowMarkCfg.color1;

	int tmp = 0x0003 & block.line;
	switch(tmp)
	{
	case 0: //上行
		if(m_ptgctrl->mConfigTg.up_color>0)
			color = m_ptgctrl->mConfigTg.up_color;
		break;
	case 1: //下行
		if(m_ptgctrl->mConfigTg.down_color>0)
			color = m_ptgctrl->mConfigTg.down_color;
		break;
	}

	if (m_ptgctrl->mConfigTg.m_context.IsCanModifyBlockStatus() && IsBlockComplete(block.adjust_status))
		color = m_ptgctrl->mConfigTg.m_AbsSlowMarkCfg.off_color;


	UINT nMinIndex = (block.station < block.index)?block.station:block.index;
	UINT nMaxIndex = (block.station > block.index)?block.station:block.index;

	if(nMinIndex<0||nMinIndex>=512 || nMaxIndex<0||nMaxIndex>=512)
	{
		sysprint(5,0, "站序1 %d  站序2 %d 越界(0<=index<512), 不显示该施工符号block %d, type %d ", block.index, block.station, block.mark_index, block.type);
		return ;
	}
	
	if(nMinIndex >=this->m_nDispDistanceNum)
	{
		sysprint(5,0, "站序1 %d  站序2 %d 不在有效显示区内, 不显示该施工符号block %d, type %d ", block.index, block.station, block.mark_index, block.type);
		return ;
	}

	if(nMaxIndex >=this->m_nDispDistanceNum)
	{
		nMaxIndex=this->m_nDispDistanceNum-1;
		if(nMinIndex>=nMaxIndex)
		{
			sysprint(5,0, "由于站序1 %d  站序2 %d 不在有效显示区内, 不显示该施工符号block %d, type %d ", block.index, block.station, block.mark_index, block.type);
			return ;
		}
	}

	int y1,y2;
	if(this->m_sDispDistance[nMinIndex].show_sation_sep)
		y1 = this->GetVPosition(nMinIndex, POS_FLAG::DOWN);
	else
		y1 = this->GetVPosition(nMinIndex, POS_FLAG::UP);

	y2 = this->GetVPosition(nMaxIndex, POS_FLAG::UP);
	if(y2>=y1+20)
	{
		y1 += 5;
		y2 -= 5;
	}

	block.station1 = this->m_sDispDistance[nMinIndex].station_number;
	block.station2 = this->m_sDispDistance[nMaxIndex].station_number;

	int x1 = this->GetOffsetMinuteFromGraphBegin(block.start_time)*m_fPixelPerMinute;
	int x2 = this->GetOffsetMinuteFromGraphBegin(block.end_time)*m_fPixelPerMinute;

	if(x1 < mGraphBeginPixel) x1 = mGraphBeginPixel;
	if(x2 > mGraphEndPixel)   x2 = mGraphEndPixel+10;   //放开，太原 修改施工符号起止时间 

	if(x1<0) 
		x1 = 0;
	if(x2<0) 
		x2 = 0;

	if(x1 == 0 && x2 == 0)
		return ;

	block.m_drawRect.left	= x1;
	block.m_drawRect.right  = x2;
	block.m_drawRect.top	= y1;
	block.m_drawRect.bottom = y2;

	int Ymiddle = block.m_drawRect.CenterPoint().y;
	int Xmiddle = block.m_drawRect.CenterPoint().x;
	int Ydlt    = abs(y2-y1)/8;

	CPen nPen, *pOldPen;
	nPen.CreatePen(PS_SOLID, 2, color);  
	pOldPen = pDC->SelectObject(&nPen);

	// 绘制竖线
	pDC->MoveTo(x1,y1);  pDC->LineTo(x1,y2);
	pDC->MoveTo(x2,y1);  pDC->LineTo(x2,y2);
	pDC->SelectObject(pOldPen);
	nPen.DeleteObject();

	// 绘制中间横线
	CPoint startp,endp;
	if(IsLineLinePosTop(block.line))
	{
		startp.x = block.m_drawRect.left;
		endp.x = block.m_drawRect.right;
		startp.y = endp.y = block.m_drawRect.top + abs(block.m_drawRect.Height())/3;
	}
	else if(IsLineLinePosBottom(block.line))
	{
		startp.x = block.m_drawRect.left;
		endp.x = block.m_drawRect.right;
		startp.y = endp.y = block.m_drawRect.top + (2*abs(block.m_drawRect.Height()))/3;
	}
	else 
	{
		startp.x = block.m_drawRect.left;
		endp.x = block.m_drawRect.right;
		startp.y = endp.y = block.m_drawRect.top + abs(block.m_drawRect.Height())/2;
	}

	int nStyleCount = 2;
	DWORD StyleArray[] = {15,10};

	LOGBRUSH logBrush;
	logBrush.lbStyle = BS_SOLID;
	logBrush.lbColor = color; 
	logBrush.lbHatch = 0;
	nPen.CreatePen(PS_GEOMETRIC|PS_USERSTYLE, 2,&logBrush,nStyleCount,StyleArray);  

	pOldPen = pDC->SelectObject(&nPen);

	pDC->MoveTo(startp);  pDC->LineTo(endp);

	pDC->SelectObject(pOldPen);
	nPen.DeleteObject();

	// 绘制方向横线
	if(this->m_ptgctrl->mConfigTg.m_AbsSlowMarkCfg.draw == 1)
	{
		int nStyleCount = 2;
		DWORD StyleArray[] = {10,10};

		LOGBRUSH logBrush;
		logBrush.lbStyle = BS_SOLID;
		logBrush.lbColor = m_ptgctrl->mConfigTg.m_AbsSlowMarkCfg.color2; 
		logBrush.lbHatch = 0;

		nPen.CreatePen(PS_GEOMETRIC|PS_USERSTYLE, 1,&logBrush,nStyleCount,StyleArray);  

		pOldPen = pDC->SelectObject(&nPen);

		if(nDirect == 2) 
		{	
			pDC->MoveTo(startp.x, startp.y + 6);
			pDC->LineTo(endp.x, endp.y + 6);
		}
		else if(nDirect == 1)
		{	
			pDC->MoveTo(startp.x, startp.y - 6);
			pDC->LineTo(endp.x, endp.y - 6);
		}

		pDC->SelectObject(pOldPen);
		nPen.DeleteObject();
	}

	block.cRect.left  = Xmiddle-5;
	block.cRect.right = Xmiddle+5;
	block.cRect.top		= endp.y-5;
	block.cRect.bottom  = endp.y+5;

	block.lRect.left  = x1-5;
	block.lRect.right = x1+5;
	block.lRect.top		= endp.y-5;
	block.lRect.bottom  = endp.y+5;

	block.rRect.left  = x2-5;
	block.rRect.right = x2+5;
	block.rRect.top		= endp.y-5;
	block.rRect.bottom  = endp.y+5;

	// 绘制焦点
	if(block.m_bFocus == TRUE)
	{
		CBrush  m_brushBackground;
		m_brushBackground.CreateSolidBrush(color);
		if(block.bMoveCenter == FALSE)
			pDC->FrameRect(block.cRect,&m_brushBackground); 
		else
			pDC->FillRect(block.cRect,&m_brushBackground);
		if(block.bMoveLeft == FALSE)
			pDC->FrameRect(block.lRect,&m_brushBackground); 
		else
			pDC->FillRect(block.lRect,&m_brushBackground);
		if(block.bMoveRight == FALSE)
			pDC->FrameRect(block.rRect,&m_brushBackground); 
		else
			pDC->FillRect(block.rRect,&m_brushBackground);
		m_brushBackground.DeleteObject();
	}

	CString strName,strDistance;
	CString strDirection=GetABSStrDirectDesp(nDirect, block.ele_number[0], block.count);
	
	if(block.slow_time > 0)
		strDistance.Format("限速 %d km/h 附加时分 %d分钟",  GetSlowSpeed(block.flag), block.slow_time/60);
	else
		strDistance.Format("限速 %d km/h ",  GetSlowSpeed(block.flag));
	
	CString dcmdno, reason, head;
	this->ParseMarkReasonText(block.text, dcmdno, reason);
	if(!dcmdno.IsEmpty())
		head = "揭示命令号:" + dcmdno + " ";
	else
		head = "";

	CString strKm = block.GetKiloMDesp();
	if(strKm.IsEmpty())
	{
		strName = head + strDirection +  reason + strDistance;		
	}
	else
	{
		strName = head + strDirection + strKm + reason + strDistance;
	}

	COLORREF oldTextColor = pDC->SetTextColor(m_ptgctrl->mConfigTg.m_AbsSlowMarkCfg.color3);
	UINT oldAlign = pDC->SetTextAlign(TA_CENTER|TA_BOTTOM);
	CFont* oldFont = pDC->SelectObject(&mBlockFont);

	CSize sz = pDC->GetTextExtent(strName);
	CRect rect(Xmiddle-sz.cx/2, y1-30, Xmiddle+sz.cx/2, y2+30);
	// 输出文本
	if(IsLineTextPosBottom(block.line))
	{
		pDC->TextOut (Xmiddle, block.m_drawRect.bottom, strName);
	}
	else if(IsLineTextPosLeft(block.line))
	{
		pDC->SetTextAlign(TA_LEFT|TA_TOP);
		pDC->TextOut (block.m_drawRect.left, block.m_drawRect.CenterPoint().y, strName);

		rect.left = block.m_drawRect.left;
		rect.right= block.m_drawRect.left+sz.cx;
	}
	else if(IsLineTextPosMiddle(block.line))
	{
		pDC->SetTextAlign(TA_CENTER|TA_TOP);
		pDC->TextOut (block.m_drawRect.CenterPoint().x, block.m_drawRect.CenterPoint().y-sz.cy/2, strName);
	}
	else if(IsLineTextPosRight(block.line))
	{
		pDC->SetTextAlign(TA_RIGHT|TA_TOP);
		pDC->TextOut (block.m_drawRect.right, block.m_drawRect.CenterPoint().y, strName);

		rect.left = block.m_drawRect.right-sz.cx;
		rect.right= block.m_drawRect.right;
	}
	else // IsLineTextPosTop
	{
		pDC->SetTextAlign(TA_CENTER|TA_TOP);
		pDC->TextOut (Xmiddle, block.m_drawRect.top, strName);
	}

	pDC->SetTextAlign(TA_CENTER|TA_BOTTOM);

	// 刷新矩形
	block.myPosition.UnionRect(&block.m_drawRect, &rect);
	if(block.myPosition.left>20)
		block.myPosition.left-=20;
	else
		block.myPosition.left=0;
	block.myPosition.right+=20;

	// 输出时间
	pDC->SelectObject(&mBlockFontTime);
	pDC->SetTextColor(m_ptgctrl->mConfigTg.m_AbsSlowMarkCfg.color3);
	CString time_char_left;
	CString time_char_right;
	time_char_left.Format("%d", block.start_time/60 % 10);
	time_char_right.Format("%d", block.end_time/60 % 10);

	sz = pDC->GetTextExtent(time_char_left);

	if(this->m_ptgctrl->mConfigTg.m_AbsSlowMarkCfg.time_pos == 0)
	{
		pDC->TextOut(x1, y1, time_char_left);
		pDC->TextOut(x1, y2+sz.cy, time_char_left);
		pDC->TextOut(x2, y1, time_char_right);
		pDC->TextOut(x2, y2+sz.cy, time_char_right);
	}
	else
	{
		pDC->TextOut(x1-sz.cx, Ymiddle+sz.cy/2, time_char_left);
		pDC->TextOut(x2+sz.cx, Ymiddle+sz.cy/2, time_char_right);
	}
	pDC->SetTextAlign(oldAlign);
	pDC->SetTextColor(oldTextColor);
	pDC->SelectObject(oldFont);
}

void CTGDoc::DrawAbsZhFixMark(CTg_BLOCK& block, CDC *pDC, int nDirect)
{
	COLORREF color = m_ptgctrl->mConfigTg.m_AbsZhFixMarkCfg.color1;
	
	int tmp = 0x0003 & block.line;
	switch(tmp)
	{
	case 0: //上行
		if(m_ptgctrl->mConfigTg.up_color>0)
			color = m_ptgctrl->mConfigTg.up_color;
		break;
	case 1: //下行
		if(m_ptgctrl->mConfigTg.down_color>0)
			color = m_ptgctrl->mConfigTg.down_color;
		break;
	}

	if (m_ptgctrl->mConfigTg.m_context.IsCanModifyBlockStatus() && IsBlockComplete(block.adjust_status))
		color = m_ptgctrl->mConfigTg.m_AbsZhFixMarkCfg.off_color;

	UINT nMinIndex = (block.station < block.index)?block.station:block.index;
	UINT nMaxIndex = (block.station > block.index)?block.station:block.index;

	if(nMinIndex<0||nMinIndex>=512 || nMaxIndex<0||nMaxIndex>=512)
	{
		sysprint(5,0, "站序1 %d  站序2 %d 越界(0<=index<512), 不显示该施工符号block %d, type %d ", block.index, block.station, block.mark_index, block.type);
		return ;
	}
	
	if(nMinIndex >=this->m_nDispDistanceNum)
	{
		sysprint(5,0, "站序1 %d  站序2 %d 不在有效显示区内, 不显示该施工符号block %d, type %d ", block.index, block.station, block.mark_index, block.type);
		return ;
	}

	if(nMaxIndex >=this->m_nDispDistanceNum)
	{
		nMaxIndex=this->m_nDispDistanceNum-1;
		if(nMinIndex>=nMaxIndex)
		{
			sysprint(5,0, "由于站序1 %d  站序2 %d 不在有效显示区内, 不显示该施工符号block %d, type %d ", block.index, block.station, block.mark_index, block.type);
			return ;
		}
	}

	int y1,y2;
	if(this->m_sDispDistance[nMinIndex].show_sation_sep)
		y1 = this->GetVPosition(nMinIndex, POS_FLAG::DOWN);
	else
		y1 = this->GetVPosition(nMinIndex, POS_FLAG::UP);

	y2 = this->GetVPosition(nMaxIndex, POS_FLAG::UP);

	if(y2>=y1+20)
	{
		y1 += 5;
		y2 -= 5;
	}

	block.station1 = this->m_sDispDistance[nMinIndex].station_number;
	block.station2 = this->m_sDispDistance[nMaxIndex].station_number;

	int x1 = this->GetOffsetMinuteFromGraphBegin(block.start_time)*m_fPixelPerMinute;
	int x2 = this->GetOffsetMinuteFromGraphBegin(block.end_time)*m_fPixelPerMinute;

	if(x1 < mGraphBeginPixel) x1 = mGraphBeginPixel;
	//if(x2 > mGraphEndPixel)   x2 = mGraphEndPixel+10;

	if(x1<0) 
		x1 = 0;
	if(x2<0) 
		x2 = 0;

	if(x1 == 0 && x2 == 0)
		return ;

	//选择了封锁上方站
	if(block.slow_time&0x0001)
	{
		y1 = this->GetVPosition(nMinIndex, POS_FLAG::UP);
	}

	//选择了封锁下方站	
	if(block.slow_time&0x0002)
	{
		if(this->m_sDispDistance[nMaxIndex].show_sation_sep)
			y2 = this->GetVPosition(nMaxIndex, POS_FLAG::DOWN);
		else
			y2 = this->GetVPosition(nMaxIndex, POS_FLAG::UP);
	}
	if(block.slow_time&0x0001)
		y1 -= 4;
	else
		y1 += 2;

	if(block.slow_time&0x0002)
		y2 += 4;
	else 
		y2 -= 2;

	block.m_drawRect.left	= x1;
	block.m_drawRect.right  = x2;
	block.m_drawRect.top	= y1;
	block.m_drawRect.bottom = y2;

	int Ymiddle = block.m_drawRect.CenterPoint().y;
	int Xmiddle = block.m_drawRect.CenterPoint().x;
	int Ydlt    = abs(y2-y1)/8;

	CPen nPen, *pOldPen;
	nPen.CreatePen(PS_SOLID, 2, color);  
	pOldPen = pDC->SelectObject(&nPen);

	// 绘制竖线
	pDC->MoveTo(x1,y1);  
	pDC->LineTo(x1,y2);
	pDC->LineTo(x2,y2);
	pDC->LineTo(x2,y1);
	pDC->LineTo(x1,y1);

	pDC->SelectObject(pOldPen);
	nPen.DeleteObject();

	block.cRect.left  = Xmiddle-6;
	block.cRect.right = Xmiddle+6;
	block.cRect.top		= Ymiddle-6;
	block.cRect.bottom  = Ymiddle+6;

	block.lRect.left  = x1-5;
	block.lRect.right = x1+5;
	block.lRect.top		= Ymiddle-5;
	block.lRect.bottom  = Ymiddle+5;

	block.rRect.left  = x2-5;
	block.rRect.right = x2+5;
	block.rRect.top		= Ymiddle-5;
	block.rRect.bottom  = Ymiddle+5;

	// 绘制焦点
	if(block.m_bFocus == TRUE)
	{
		CBrush  m_brushBackground;
		m_brushBackground.CreateSolidBrush(color);
		if(block.bMoveCenter == FALSE)
			pDC->FrameRect(block.cRect,&m_brushBackground); 
		else
			pDC->FillRect(block.cRect,&m_brushBackground);

		if(block.bMoveLeft == FALSE)
			pDC->FrameRect(block.lRect,&m_brushBackground); 
		else
			pDC->FillRect(block.lRect,&m_brushBackground);

		if(block.bMoveRight == FALSE)
			pDC->FrameRect(block.rRect,&m_brushBackground); 
		else
			pDC->FillRect(block.rRect,&m_brushBackground);

		m_brushBackground.DeleteObject();
	}

	CString strName;
	CString strDirection=GetABSStrDirectDesp(nDirect, block.ele_number[0], block.count);

	CString dcmdno, reason, head;
	this->ParseMarkReasonText(block.text, dcmdno, reason);
	if(!dcmdno.IsEmpty())
		head = "揭示命令号:" + dcmdno + " ";
	else
		head = "";

	CString strKm = block.GetKiloMDesp();
	if(strKm.IsEmpty())
	{
		strName = head + strDirection +  reason;		
	}
	else
	{
		strName = head + strDirection + strKm + reason;
	}

	COLORREF oldTextColor = pDC->SetTextColor(m_ptgctrl->mConfigTg.m_AbsZhFixMarkCfg.color3);
	UINT oldAlign = pDC->SetTextAlign(TA_CENTER|TA_BOTTOM);
	CFont *oldFont = pDC->SelectObject(&mBlockFont);

	CSize sz = pDC->GetTextExtent(strName);
	CRect rect(Xmiddle-sz.cx/2, y1-30, Xmiddle+sz.cx/2, y2+30);
	// 输出文本
	if(IsLineTextPosBottom(block.line))
	{
		pDC->TextOut (Xmiddle, block.m_drawRect.bottom-block.m_drawRect.Height()/6, strName);
	}
	else if(IsLineTextPosLeft(block.line))
	{
		pDC->SetTextAlign(TA_LEFT|TA_TOP);
		pDC->TextOut (block.m_drawRect.left, block.m_drawRect.CenterPoint().y, strName);

		rect.left = block.m_drawRect.left;
		rect.right= block.m_drawRect.left+sz.cx;
	}
	else if(IsLineTextPosMiddle(block.line))
	{
		pDC->SetTextAlign(TA_CENTER|TA_TOP);
		pDC->TextOut (block.m_drawRect.CenterPoint().x, block.m_drawRect.CenterPoint().y-sz.cy/2, strName);
	}
	else if(IsLineTextPosRight(block.line))
	{
		pDC->SetTextAlign(TA_RIGHT|TA_TOP);
		pDC->TextOut (block.m_drawRect.right, block.m_drawRect.CenterPoint().y, strName);

		rect.left = block.m_drawRect.right-sz.cx;
		rect.right= block.m_drawRect.right;
	}
	else // IsLineTextPosTop
	{
		pDC->SetTextAlign(TA_CENTER|TA_TOP);
		pDC->TextOut (Xmiddle, block.m_drawRect.top+block.m_drawRect.Height()/6, strName);
	}
	pDC->SetTextAlign(TA_CENTER|TA_BOTTOM);

	// 输出文本
	CPoint startp,endp;

	// 刷新矩形
	block.myPosition.UnionRect(&block.m_drawRect, &rect);
	if(block.myPosition.left>20)
		block.myPosition.left-=20;
	else
		block.myPosition.left=0;
	block.myPosition.right+=20;

	// 输出时间
	pDC->SelectObject(&mBlockFontTime);

	pDC->SetTextColor(m_ptgctrl->mConfigTg.m_AbsZhFixMarkCfg.color3);
	CString time_char_left;
	CString time_char_right;
	time_char_left.Format("%d", block.start_time/60 % 10);
	time_char_right.Format("%d", block.end_time/60 % 10);

	sz = pDC->GetTextExtent(time_char_left);
	if(this->m_ptgctrl->mConfigTg.m_AbsZhFixMarkCfg.time_pos == 0)
	{
		pDC->TextOut(x1, y1, time_char_left);
		pDC->TextOut(x1, y2+sz.cy, time_char_left);
		pDC->TextOut(x2, y1, time_char_right);
		pDC->TextOut(x2, y2+sz.cy, time_char_right);
	}
	else
	{
		pDC->TextOut(x1-sz.cx, Ymiddle+sz.cy/2, time_char_left);
		pDC->TextOut(x2+sz.cx, Ymiddle+sz.cy/2, time_char_right);
	}
	pDC->SetTextAlign(oldAlign);
	pDC->SetTextColor(oldTextColor);
	pDC->SelectObject(oldFont);
}

void CTGDoc::DrawAbsEleFixMark(CTg_BLOCK& block, CDC *pDC, int nDirect)
{
	COLORREF color = m_ptgctrl->mConfigTg.m_AbsEleFixMarkCfg.color1;

	int tmp = 0x0003 & block.line;
	switch(tmp)
	{
	case 0: //上行
		if(m_ptgctrl->mConfigTg.up_color>0)
			color = m_ptgctrl->mConfigTg.up_color;
		break;
	case 1: //下行
		if(m_ptgctrl->mConfigTg.down_color>0)
			color = m_ptgctrl->mConfigTg.down_color;
		break;
	}

	if (m_ptgctrl->mConfigTg.m_context.IsCanModifyBlockStatus() && IsBlockComplete(block.adjust_status))
		color = m_ptgctrl->mConfigTg.m_AbsEleFixMarkCfg.off_color;

	UINT nMinIndex = (block.station < block.index)?block.station:block.index;
	UINT nMaxIndex = (block.station > block.index)?block.station:block.index;

	if(nMinIndex<0 || nMinIndex>=512 || nMaxIndex<0 || nMaxIndex>=512)
	{
		sysprint(5,0, "站序1 %d  站序2 %d 越界(0<=index<512), 不显示该施工符号block %d, type %d ", block.index, block.station, block.mark_index, block.type);
		return ;
	}

	if(nMinIndex >=this->m_nDispDistanceNum)
	{
		sysprint(5,0, "站序1 %d  站序2 %d 不在有效显示区内, 不显示该施工符号block %d, type %d ", block.index, block.station, block.mark_index, block.type);
		return ;
	}

	if(nMaxIndex >=this->m_nDispDistanceNum)
	{
		nMaxIndex=this->m_nDispDistanceNum-1;
		if(nMinIndex>=nMaxIndex)
		{
			sysprint(5,0, "由于站序1 %d  站序2 %d 不在有效显示区内, 不显示该施工符号block %d, type %d ", block.index, block.station, block.mark_index, block.type);
			return ;
		}
	}

	int y1,y2;
	if(this->m_sDispDistance[nMinIndex].show_sation_sep)
		y1 = this->GetVPosition(nMinIndex, POS_FLAG::DOWN);
	else
		y1 = this->GetVPosition(nMinIndex, POS_FLAG::UP);

	y2 = this->GetVPosition(nMaxIndex, POS_FLAG::UP);

	if(y2>=y1+20)
	{
		y1 += 5;
		y2 -= 5;
	}

	block.station1 = this->m_sDispDistance[nMinIndex].station_number;
	block.station2 = this->m_sDispDistance[nMaxIndex].station_number;

	int x1 = this->GetOffsetMinuteFromGraphBegin(block.start_time)*m_fPixelPerMinute;
	int x2 = this->GetOffsetMinuteFromGraphBegin(block.end_time)*m_fPixelPerMinute;

	if(x1 < mGraphBeginPixel) x1 = mGraphBeginPixel;
	//if(x2 > mGraphEndPixel)   x2 = mGraphEndPixel+10;

	if(x1<0) 
		x1 = 0;
	if(x2<0) 
		x2 = 0;

	if(x1 == 0 && x2 == 0)
		return ;

	//选择了封锁上方站
	if(block.slow_time&0x0001)
	{
		y1 = this->GetVPosition(nMinIndex, POS_FLAG::UP);
	}

	//选择了封锁下方站	
	if(block.slow_time&0x0002)
	{
		if(this->m_sDispDistance[nMaxIndex].show_sation_sep)
			y2 = this->GetVPosition(nMaxIndex, POS_FLAG::DOWN);
		else
			y2 = this->GetVPosition(nMaxIndex, POS_FLAG::UP);
	}
	if(block.slow_time&0x0001)
		y1 -= 4;
	else
		y1 += 2;

	if(block.slow_time&0x0002)
		y2 += 4;
	else 
		y2 -= 2;

	block.m_drawRect.left	= x1;
	block.m_drawRect.right  = x2;
	block.m_drawRect.top	= y1;
	block.m_drawRect.bottom = y2;

	int Ymiddle = block.m_drawRect.CenterPoint().y;
	int Xmiddle = block.m_drawRect.CenterPoint().x;
	int Ydlt    = abs(y2-y1)/8;

	CPen nPen, *pOldPen;
	nPen.CreatePen(PS_SOLID, 2, color);  
	pOldPen = pDC->SelectObject(&nPen);

	// 绘制竖线
	pDC->MoveTo(x1,y1);  
	pDC->LineTo(x1,y2);
	pDC->LineTo(x2,y2);
	pDC->LineTo(x2,y1);
	pDC->LineTo(x1,y1);

	pDC->SelectObject(pOldPen);
	nPen.DeleteObject();

	block.cRect.left  = Xmiddle-6;
	block.cRect.right = Xmiddle+6;
	block.cRect.top		= Ymiddle-6;
	block.cRect.bottom  = Ymiddle+6;

	block.lRect.left  = x1-5;
	block.lRect.right = x1+5;
	block.lRect.top		= Ymiddle-5;
	block.lRect.bottom  = Ymiddle+5;

	block.rRect.left  = x2-5;
	block.rRect.right = x2+5;
	block.rRect.top		= Ymiddle-5;
	block.rRect.bottom  = Ymiddle+5;

	// 绘制焦点
	if(block.m_bFocus == TRUE)
	{
		CBrush  m_brushBackground;
		m_brushBackground.CreateSolidBrush(color);
		if(block.bMoveCenter == FALSE)
			pDC->FrameRect(block.cRect,&m_brushBackground); 
		else
			pDC->FillRect(block.cRect,&m_brushBackground);

		if(block.bMoveLeft == FALSE)
			pDC->FrameRect(block.lRect,&m_brushBackground); 
		else
			pDC->FillRect(block.lRect,&m_brushBackground);

		if(block.bMoveRight == FALSE)
			pDC->FrameRect(block.rRect,&m_brushBackground); 
		else
			pDC->FillRect(block.rRect,&m_brushBackground);

		m_brushBackground.DeleteObject();
	}

	CString strName;
	CString strDirection=GetABSStrDirectDesp(nDirect, block.ele_number[0], block.count);

	CString dcmdno, reason, head;
	this->ParseMarkReasonText(block.text, dcmdno, reason);
	if(!dcmdno.IsEmpty())
		head = "揭示命令号:" + dcmdno + " ";
	else
		head = "";

	CString strKm = block.GetKiloMDesp();
	if(strKm.IsEmpty())
	{
		strName = head + strDirection +  reason;		
	}
	else
	{
		strName = head + strDirection + strKm + reason;
	}

	COLORREF oldTextColor = pDC->SetTextColor(m_ptgctrl->mConfigTg.m_AbsEleFixMarkCfg.color3);
	UINT oldAlign = pDC->SetTextAlign(TA_CENTER|TA_BOTTOM);
	CFont *oldFont = pDC->SelectObject(&mBlockFont);
	CSize sz = pDC->GetTextExtent(strName);
	CRect rect(Xmiddle-sz.cx/2, y1-30, Xmiddle+sz.cx/2, y2+30);
	// 输出文本
	if(IsLineTextPosBottom(block.line))
	{
		pDC->TextOut (Xmiddle, block.m_drawRect.bottom-block.m_drawRect.Height()/6, strName);
	}
	else if(IsLineTextPosLeft(block.line))
	{
		pDC->SetTextAlign(TA_LEFT|TA_TOP);
		pDC->TextOut (block.m_drawRect.left, block.m_drawRect.CenterPoint().y, strName);

		rect.left = block.m_drawRect.left;
		rect.right= block.m_drawRect.left+sz.cx;
	}
	else if(IsLineTextPosMiddle(block.line))
	{
		pDC->SetTextAlign(TA_CENTER|TA_TOP);
		pDC->TextOut (block.m_drawRect.CenterPoint().x, block.m_drawRect.CenterPoint().y-sz.cy/2, strName);
	}
	else if(IsLineTextPosRight(block.line))
	{
		pDC->SetTextAlign(TA_RIGHT|TA_TOP);
		pDC->TextOut (block.m_drawRect.right, block.m_drawRect.CenterPoint().y, strName);

		rect.left = block.m_drawRect.right-sz.cx;
		rect.right= block.m_drawRect.right;
	}
	else // IsLineTextPosTop
	{
		pDC->SetTextAlign(TA_CENTER|TA_TOP);
		pDC->TextOut (Xmiddle, block.m_drawRect.top+block.m_drawRect.Height()/6, strName);
	}
	pDC->SetTextAlign(TA_CENTER|TA_BOTTOM);

	// 输出文本
	CPoint startp,endp;

	// 刷新矩形
	block.myPosition.UnionRect(&block.m_drawRect, &rect);
	if(block.myPosition.left>20)
		block.myPosition.left-=20;
	else
		block.myPosition.left=0;
	block.myPosition.right+=20;

	// 输出时间
	pDC->SelectObject(&mBlockFontTime);
	pDC->SetTextColor(m_ptgctrl->mConfigTg.m_AbsEleFixMarkCfg.color3);
	CString time_char_left;
	CString time_char_right;
	time_char_left.Format("%d", block.start_time/60 % 10);
	time_char_right.Format("%d", block.end_time/60 % 10);

	sz = pDC->GetTextExtent(time_char_left);
	if(this->m_ptgctrl->mConfigTg.m_AbsEleFixMarkCfg.time_pos == 0)
	{
		pDC->TextOut(x1, y1, time_char_left);
		pDC->TextOut(x1, y2+sz.cy, time_char_left);
		pDC->TextOut(x2, y1, time_char_right);
		pDC->TextOut(x2, y2+sz.cy, time_char_right);
	}
	else
	{
		pDC->TextOut(x1-sz.cx, Ymiddle+sz.cy/2, time_char_left);
		pDC->TextOut(x2+sz.cx, Ymiddle+sz.cy/2, time_char_right);
	}
	pDC->SetTextAlign(oldAlign);
	pDC->SetTextColor(oldTextColor);
	pDC->SelectObject(oldFont);
}

void CTGDoc::DrawAbsSelfCloseMark(CTg_BLOCK& block, CDC *pDC, int nDirect)
{
	COLORREF color = m_ptgctrl->mConfigTg.m_SelfCloseMarkCfg.color1;
	COLORREF color2 = m_ptgctrl->mConfigTg.m_SelfCloseMarkCfg.color1;

	int tmp = 0x0003 & block.line;
	switch(tmp)
	{
	case 0: //上行
		if(m_ptgctrl->mConfigTg.up_color>0)
			color = m_ptgctrl->mConfigTg.up_color;
		break;
	case 1: //下行
		if(m_ptgctrl->mConfigTg.down_color>0)
			color = m_ptgctrl->mConfigTg.down_color;
		break;
	}

	if (m_ptgctrl->mConfigTg.m_context.IsCanModifyBlockStatus() && IsBlockComplete(block.adjust_status))
	{
		color = m_ptgctrl->mConfigTg.m_SelfCloseMarkCfg.off_color;
		color2 = m_ptgctrl->mConfigTg.m_SelfCloseMarkCfg.off_color;
	}

	UINT nMinIndex = (block.station < block.index)?block.station:block.index;
	UINT nMaxIndex = (block.station > block.index)?block.station:block.index;

	if(nMinIndex<0||nMinIndex>=512 || nMaxIndex<0||nMaxIndex>=512)
	{
		sysprint(5,0, "站序1 %d  站序2 %d 越界(0<=index<512), 不显示该施工符号block %d, type %d ", block.index, block.station, block.mark_index, block.type);
		return ;
	}
	
	if(nMinIndex >=this->m_nDispDistanceNum)
	{
		sysprint(5,0, "站序1 %d  站序2 %d 不在有效显示区内, 不显示该施工符号block %d, type %d ", block.index, block.station, block.mark_index, block.type);
		return ;
	}

	if(nMaxIndex >=this->m_nDispDistanceNum)
	{
		nMaxIndex=this->m_nDispDistanceNum-1;
		if(nMinIndex>=nMaxIndex)
		{
			sysprint(5,0, "由于站序1 %d  站序2 %d 不在有效显示区内, 不显示该施工符号block %d, type %d ", block.index, block.station, block.mark_index, block.type);
			return ;
		}
	}

	int y1,y2;
	if(this->m_sDispDistance[nMinIndex].show_sation_sep)
		y1 = this->GetVPosition(nMinIndex, POS_FLAG::DOWN);
	else
		y1 = this->GetVPosition(nMinIndex, POS_FLAG::UP);

	y2 = this->GetVPosition(nMaxIndex, POS_FLAG::UP);

	if(y2>=y1+20)
	{
		y1 += 5;
		y2 -= 5;
	}

	block.station1 = this->m_sDispDistance[nMinIndex].station_number;
	block.station2 = this->m_sDispDistance[nMaxIndex].station_number;

	int x1 = this->GetOffsetMinuteFromGraphBegin(block.start_time)*m_fPixelPerMinute;
	int x2 = this->GetOffsetMinuteFromGraphBegin(block.end_time)*m_fPixelPerMinute;

	if(x1 < mGraphBeginPixel) x1 = mGraphBeginPixel;
	//if(x2 > mGraphEndPixel)   x2 = mGraphEndPixel+10;

	if(x1<0) 
		x1 = 0;
	if(x2<0) 
		x2 = 0;

	if(x1 == 0 && x2 == 0)
		return ;

	block.m_drawRect.left	= x1;
	block.m_drawRect.right  = x2;
	block.m_drawRect.top	= y1;
	block.m_drawRect.bottom = y2;

	int Ymiddle = block.m_drawRect.CenterPoint().y;
	int Xmiddle = block.m_drawRect.CenterPoint().x;
	int Ydlt    = abs(y2-y1)/8;

	CPen nPen, *pOldPen;
	nPen.CreatePen(PS_SOLID, 2, color);  
	pOldPen = pDC->SelectObject(&nPen);

	// 绘制竖线
	pDC->MoveTo(x1,y1);  pDC->LineTo(x1,y2);
	pDC->MoveTo(x2,y1);  pDC->LineTo(x2,y2);
	pDC->SelectObject(pOldPen);
	nPen.DeleteObject();

	block.cRect.left  = Xmiddle-6;
	block.cRect.right = Xmiddle+6;
	block.cRect.top		= Ymiddle-6;
	block.cRect.bottom  = Ymiddle+6;

	block.lRect.left  = x1-5;
	block.lRect.right = x1+5;
	block.lRect.top		= Ymiddle-5;
	block.lRect.bottom  = Ymiddle+5;

	block.rRect.left  = x2-5;
	block.rRect.right = x2+5;
	block.rRect.top		= Ymiddle-5;
	block.rRect.bottom  = Ymiddle+5;

	// 中间两虚线
	if(this->m_ptgctrl->mConfigTg.m_SelfCloseMarkCfg.draw == 2)
	{
		// 绘制中间横线
		CPoint startp,endp;

		int nStyleCount = 2;
		DWORD StyleArray[] = {15,10};

		LOGBRUSH logBrush;
		logBrush.lbStyle = BS_SOLID;
		logBrush.lbColor = color2; 
		logBrush.lbHatch = 0;

	    CPen nPen, *pOldPen;
		nPen.CreatePen(PS_GEOMETRIC|PS_USERSTYLE, 1, &logBrush, nStyleCount, StyleArray);  

		pOldPen = pDC->SelectObject(&nPen);

		startp.x = block.m_drawRect.left;
		endp.x = block.m_drawRect.right;
		startp.y = endp.y = block.m_drawRect.top + abs(block.m_drawRect.Height())/3;
		pDC->MoveTo(startp);  pDC->LineTo(endp);

		startp.x = block.m_drawRect.left;
		endp.x = block.m_drawRect.right;
		startp.y = endp.y = block.m_drawRect.top + 2*abs(block.m_drawRect.Height())/3;
		pDC->MoveTo(startp);  pDC->LineTo(endp);

		pDC->SelectObject(pOldPen);
		nPen.DeleteObject();
	}
	else if(this->m_ptgctrl->mConfigTg.m_SelfCloseMarkCfg.draw == 3)
	{
		// 绘制中间横线
		CPoint startp,endp;
		int nStyleCount = 2;
		DWORD StyleArray[] = {15,10};

		CPen nPen, *pOldPen;
		LOGBRUSH logBrush;
		logBrush.lbStyle = BS_SOLID;
		logBrush.lbColor = color2; 
		logBrush.lbHatch = 0;
		nPen.CreatePen(PS_GEOMETRIC|PS_USERSTYLE, 1, &logBrush, nStyleCount, StyleArray);
		pOldPen = pDC->SelectObject(&nPen);

		startp.x = block.m_drawRect.left;
		endp.x = block.m_drawRect.right;
		startp.y = endp.y = block.m_drawRect.CenterPoint().y;
		pDC->MoveTo(startp);  pDC->LineTo(endp);

		pDC->SelectObject(pOldPen);
		nPen.DeleteObject();
	}
	else
	{
		// 绘制中间横线
		CPoint startp,endp;
		CPen nPen, *pOldPen;
		nPen.CreatePen(PS_SOLID, 2, color2);  
		pOldPen = pDC->SelectObject(&nPen);

		startp.x = block.m_drawRect.left;
		endp.x = block.m_drawRect.right;
		startp.y = endp.y = block.m_drawRect.CenterPoint().y;
		pDC->MoveTo(startp);  pDC->LineTo(endp);

		pDC->SelectObject(pOldPen);
		nPen.DeleteObject();
	}

	// 绘制焦点
	if(block.m_bFocus == TRUE)
	{
		CBrush  m_brushBackground;
		m_brushBackground.CreateSolidBrush(m_ptgctrl->mConfigTg.m_SelfCloseMarkCfg.color1);
		if(block.bMoveCenter == FALSE)
			pDC->FrameRect(block.cRect,&m_brushBackground); 
		else
			pDC->FillRect(block.cRect,&m_brushBackground);
		if(block.bMoveLeft == FALSE)
			pDC->FrameRect(block.lRect,&m_brushBackground); 
		else
			pDC->FillRect(block.lRect,&m_brushBackground);
		if(block.bMoveRight == FALSE)
			pDC->FrameRect(block.rRect,&m_brushBackground); 
		else
			pDC->FillRect(block.rRect,&m_brushBackground);
		m_brushBackground.DeleteObject();
	}

	CString strName;
	CString strDirection=GetABSStrDirectDesp(nDirect, block.ele_number[0], block.count);

	CString strBlockType;
	if(block.flag == 0)
	{
		strBlockType = " 半自动闭塞";
	}
	else if(block.flag == 1)
	{
		strBlockType = " 自动闭塞";
	}
	else if(block.flag == 2)
	{
		strBlockType = " 电话闭塞";
	}
	else if(block.flag == 3)
	{
		strBlockType = " 停基改电";
	}
	else if(block.flag == 4)
	{
		strBlockType = " 停用基本闭塞";
	}
	else
	{
		strBlockType = "";
	}

	CString dcmdno, reason, head;
	this->ParseMarkReasonText(block.text, dcmdno, reason);
	if(!dcmdno.IsEmpty())
		head = "揭示命令号:" + dcmdno + " ";
	else
		head = "";

	strName = head + strDirection +  reason + strBlockType;			

	COLORREF oldTextColor = pDC->SetTextColor(m_ptgctrl->mConfigTg.m_SelfCloseMarkCfg.color3);
	UINT oldAlign = pDC->SetTextAlign(TA_CENTER|TA_BOTTOM);
	CFont *oldFont = pDC->SelectObject(&mBlockFont);

	CSize sz = pDC->GetTextExtent(strName);
	CRect rect(Xmiddle-sz.cx/2, y1-30, Xmiddle+sz.cx/2, y2+30);
	// 输出文本
	if(IsLineTextPosBottom(block.line))
	{
		pDC->TextOut (Xmiddle, block.m_drawRect.bottom, strName);
	}
	else if(IsLineTextPosLeft(block.line))
	{
		pDC->SetTextAlign(TA_LEFT|TA_TOP);
		pDC->TextOut (block.m_drawRect.left, block.m_drawRect.CenterPoint().y, strName);

		rect.left = block.m_drawRect.left;
		rect.right= block.m_drawRect.left+sz.cx;
	}
	else if(IsLineTextPosMiddle(block.line))
	{
		pDC->SetTextAlign(TA_CENTER|TA_TOP);
		pDC->TextOut (block.m_drawRect.CenterPoint().x, block.m_drawRect.CenterPoint().y-sz.cy/2, strName);
	}
	else if(IsLineTextPosRight(block.line))
	{
		pDC->SetTextAlign(TA_RIGHT|TA_TOP);
		pDC->TextOut (block.m_drawRect.right, block.m_drawRect.CenterPoint().y, strName);

		rect.left = block.m_drawRect.right-sz.cx;
		rect.right= block.m_drawRect.right;
	}
	else // IsLineTextPosTop
	{
		pDC->SetTextAlign(TA_CENTER|TA_TOP);
		pDC->TextOut (Xmiddle, block.m_drawRect.top, strName);
	}
	pDC->SetTextAlign(TA_CENTER|TA_BOTTOM);

	// 刷新矩形
	block.myPosition.UnionRect(&block.m_drawRect, &rect);
	if(block.myPosition.left>20)
		block.myPosition.left-=20;
	else
		block.myPosition.left=0;
	block.myPosition.right+=20;

	// 输出时间
	pDC->SelectObject(&mBlockFontTime);
	pDC->SetTextColor(m_ptgctrl->mConfigTg.m_SelfCloseMarkCfg.color3);
	CString time_char_left;
	CString time_char_right;
	time_char_left.Format("%d", block.start_time/60 % 10);
	time_char_right.Format("%d", block.end_time/60 % 10);

	sz = pDC->GetTextExtent(time_char_left);
	if(this->m_ptgctrl->mConfigTg.m_SelfCloseMarkCfg.time_pos == 0)
	{
		pDC->TextOut(x1, y1, time_char_left);
		pDC->TextOut(x1, y2+sz.cy, time_char_left);
		pDC->TextOut(x2, y1, time_char_right);
		pDC->TextOut(x2, y2+sz.cy, time_char_right);
	}
	else
	{
		pDC->TextOut(x1-sz.cx, Ymiddle+sz.cy/2, time_char_left);
		pDC->TextOut(x2+sz.cx, Ymiddle+sz.cy/2, time_char_right);
	}
	pDC->SetTextAlign(oldAlign);
	pDC->SetTextColor(oldTextColor);
	pDC->SelectObject(oldFont);
}

void CTGDoc::FrameRectExtend(CDC *pDC, CRect &rc)
{ 	
	pDC->MoveTo(rc.TopLeft());
	pDC->LineTo(rc.right,rc.top);
	pDC->LineTo(rc.right,rc.bottom);
	pDC->LineTo(rc.left,rc.bottom);
	pDC->LineTo(rc.TopLeft());
}

void CTGDoc::DrawSideInhibitMark(CTg_BLOCK& block, CDC *pDC)
{
	UINT nIndex = block.index;
	// 股道停电子类型
	WORD nSubType=(block.slow_time & 0x0001);
	
	if(nIndex < 0 || nIndex >= this->m_nDispDistanceNum || nIndex>=512)
	{
		sysprint(5,0, "由于站序 %d 不在有效显示区内, 不显示该施工符号block %d, type %d ",block.index, block.mark_index, block.type);
		return ;
	}
	
	int x1,x2,y1,y2;
	if(!IsShowStationMark(block, nIndex, y1, y2))
		return;

	if(y1<=0 || y2<=0) 
		return;

	block.station1 = block.station;
	block.station2 = block.station;
	x1 = this->GetOffsetMinuteFromGraphBegin(block.start_time)*m_fPixelPerMinute;
	x2 = this->GetOffsetMinuteFromGraphBegin(block.end_time)*m_fPixelPerMinute;

	if(x1 < mGraphBeginPixel) x1 = mGraphBeginPixel;
	if(x2 > mGraphEndPixel)   x2 = mGraphEndPixel+10;   //放开，太原 修改施工符号起止时间 

	if(x1<0) 
		x1 = 0;
	if(x2<0) 
		x2 = 0;

	if(x1 == 0 && x2 == 0)
		return ;

	block.m_drawRect.SetRect(x1, y1-4, x2, y2+4);
	int Ymiddle = block.m_drawRect.CenterPoint().y - 2;
	int Xmiddle = block.m_drawRect.CenterPoint().x;
	
	CPen nPen, *pOldPen;	
	COLORREF color = m_ptgctrl->mConfigTg.m_SideInhibitMarkCfg.color1;

	if (m_ptgctrl->mConfigTg.m_context.IsCanModifyBlockStatus() && IsBlockComplete(block.adjust_status))
		color = m_ptgctrl->mConfigTg.m_SideInhibitMarkCfg.off_color;

    nPen.CreatePen(PS_SOLID, 2, color);
	pOldPen = pDC->SelectObject(&nPen);

	// 绘制竖线
	pDC->MoveTo(block.m_drawRect.left, block.m_drawRect.top);  
	pDC->LineTo(block.m_drawRect.left, block.m_drawRect.bottom);

	pDC->MoveTo(block.m_drawRect.right, block.m_drawRect.top);  
	pDC->LineTo(block.m_drawRect.right, block.m_drawRect.bottom);

	// 绘制横线
	pDC->MoveTo(block.m_drawRect.left, Ymiddle);  
	pDC->LineTo(block.m_drawRect.right, Ymiddle);

	pDC->SelectObject(pOldPen);
	nPen.DeleteObject();

	block.cRect.SetRect(Xmiddle-5,Ymiddle-5,Xmiddle+5,Ymiddle+5);
	block.lRect.SetRect(x1-5,Ymiddle-5,x1+5,Ymiddle+5);
	block.rRect.SetRect(x2-5,Ymiddle-5,x2+5,Ymiddle+5);

	// 绘制焦点
	if(block.m_bFocus == TRUE)
	{
		CBrush  m_brushBackground;
		m_brushBackground.CreateSolidBrush(color);
		if(block.bMoveCenter == FALSE)
			pDC->FrameRect(block.cRect,&m_brushBackground); 
		else
			pDC->FillRect(block.cRect,&m_brushBackground);
		if(block.bMoveLeft == FALSE)
			pDC->FrameRect(block.lRect,&m_brushBackground); 
		else
			pDC->FillRect(block.lRect,&m_brushBackground);
		if(block.bMoveRight == FALSE)
			pDC->FrameRect(block.rRect,&m_brushBackground); 
		else
			pDC->FillRect(block.rRect,&m_brushBackground);
		m_brushBackground.DeleteObject();
	}
	// draw 
	CString strScope = this->GetStationSidesName(block.station, block.count, block.ele_number);
	
	CString dcmdno, reason, head;
	this->ParseMarkReasonText(block.text, dcmdno, reason);
	if(!dcmdno.IsEmpty())
		head = "揭示命令号:" + dcmdno + "  ";
	else
		head = "";

	CString strKm = block.GetKiloMDesp();
	if(strKm.IsEmpty())
	{
		strScope = head + strScope +  reason;		
	}
	else
	{
		strScope = head + strScope + strKm + reason;
	}

	if(nSubType==0x0001)
	{
		strScope += "股道停电";
	}
	COLORREF oldTextColor = pDC->SetTextColor(m_ptgctrl->mConfigTg.m_SideInhibitMarkCfg.color3);
	UINT oldAlign = pDC->SetTextAlign(TA_CENTER|TA_BOTTOM);
	CFont *oldFont = pDC->SelectObject(&mBlockFont);

	CSize sz = pDC->GetTextExtent(strScope);
	// 输出文本
	if(IsLineTextPosBottom(block.line))
		pDC->TextOut (Xmiddle, Ymiddle+sz.cy+2, strScope);
	else if(IsLineTextPosMiddle(block.line))
		pDC->TextOut (Xmiddle, Ymiddle+sz.cy/2, strScope);
	else
		pDC->TextOut (Xmiddle, Ymiddle, strScope);

	// 刷新矩形
	int tmpX1 = x1, tmpX2 = x2;
	if(abs(x2-x1) < sz.cx)
	{
		tmpX1 = Xmiddle - sz.cx/2;
		tmpX2 = Xmiddle + sz.cx/2;
	}
	block.myPosition.SetRect(tmpX1-10, y1-20, tmpX2+10, y2+20);

	// 输出时间
	pDC->SelectObject(&mBlockFontTime);
	pDC->SetTextColor(m_ptgctrl->mConfigTg.m_SideInhibitMarkCfg.color3);

	CString time_char_left;
	CString time_char_right;
	time_char_left.Format("%d", block.start_time/60 % 10);
	time_char_right.Format("%d", block.end_time/60 % 10);

	sz = pDC->GetTextExtent(time_char_left);
	if(this->m_ptgctrl->mConfigTg.m_SideInhibitMarkCfg.time_pos == 0)
	{
		pDC->TextOut(x1, y1-2, time_char_left);
		pDC->TextOut(x1, y2+sz.cy+4, time_char_left);
		pDC->TextOut(x2, y1-2, time_char_right);
		pDC->TextOut(x2, y2+sz.cy+4, time_char_right);
	}
	else
	{
		pDC->TextOut(x1-sz.cx, Ymiddle+sz.cy/2, time_char_left);
		pDC->TextOut(x2+sz.cx, Ymiddle+sz.cy/2, time_char_right);
	}

	pDC->SetTextAlign(oldAlign);
	pDC->SetTextColor(oldTextColor);
	pDC->SelectObject(oldFont);
}

void CTGDoc::DrawSideSlowMark(CTg_BLOCK& block, CDC *pDC)
{
	UINT nIndex = block.index;
	if(nIndex < 0 || nIndex >= this->m_nDispDistanceNum || nIndex>=512)
	{
		sysprint(5,0, "由于站序 %d 不在有效显示区内, 不显示该施工符号block %d, type %d ", block.index, block.mark_index, block.type);
		return ;
	}

	int x1,x2,y1,y2;
	if(!IsShowStationMark(block, nIndex, y1, y2))
		return;

	if(y1<=0 || y2<=0) 
		return;
	
	block.station1 = block.station;
	block.station2 = block.station;

	x1 = this->GetOffsetMinuteFromGraphBegin(block.start_time)*m_fPixelPerMinute;
	x2 = this->GetOffsetMinuteFromGraphBegin(block.end_time)*m_fPixelPerMinute;

	if(x1 < mGraphBeginPixel) x1 = mGraphBeginPixel;
	if(x2 > mGraphEndPixel)   x2 = mGraphEndPixel+10;   //放开，太原 修改施工符号起止时间 

	if(x1<0) 
		x1 = 0;
	if(x2<0) 
		x2 = 0;

	if(x1 == 0 && x2 == 0)
		return ;

	block.m_drawRect.SetRect(x1, y1-4, x2, y2+4);
	int Ymiddle = block.m_drawRect.CenterPoint().y - 2;
	int Xmiddle = block.m_drawRect.CenterPoint().x;
	
	CPen nPen, *pOldPen;
	COLORREF color = m_ptgctrl->mConfigTg.m_SideSlowMarkCfg.color1;
	if (m_ptgctrl->mConfigTg.m_context.IsCanModifyBlockStatus() && IsBlockComplete(block.adjust_status))
		color=m_ptgctrl->mConfigTg.m_SideSlowMarkCfg.off_color;

	nPen.CreatePen(PS_SOLID, 2, color);  
	pOldPen = pDC->SelectObject(&nPen);

	// 绘制竖线
	pDC->MoveTo(block.m_drawRect.left, block.m_drawRect.top);  
	pDC->LineTo(block.m_drawRect.left, block.m_drawRect.bottom);

	pDC->MoveTo(block.m_drawRect.right, block.m_drawRect.top);  
	pDC->LineTo(block.m_drawRect.right, block.m_drawRect.bottom);
	pDC->SelectObject(pOldPen);
	nPen.DeleteObject();

	// 绘制横线
	int nStyleCount = 2;
	DWORD StyleArray[] = {15,10};

	LOGBRUSH logBrush;
	logBrush.lbStyle = BS_SOLID;
	logBrush.lbColor = color; 
	logBrush.lbHatch = 0;
	nPen.CreatePen(PS_GEOMETRIC|PS_USERSTYLE, 2,&logBrush,nStyleCount,StyleArray);  

	pOldPen = pDC->SelectObject(&nPen);

	pDC->MoveTo(block.m_drawRect.left, Ymiddle);  
	pDC->LineTo(block.m_drawRect.right, Ymiddle);

	pDC->SelectObject(pOldPen);
	nPen.DeleteObject();

	block.cRect.SetRect(Xmiddle-5,Ymiddle-5,Xmiddle+5,Ymiddle+5);
	block.lRect.SetRect(x1-5,Ymiddle-5,x1+5,Ymiddle+5);
	block.rRect.SetRect(x2-5,Ymiddle-5,x2+5,Ymiddle+5);

	// 绘制焦点
	if(block.m_bFocus == TRUE)
	{
		CBrush  m_brushBackground;
		m_brushBackground.CreateSolidBrush(color);
		if(block.bMoveCenter == FALSE)
			pDC->FrameRect(block.cRect,&m_brushBackground); 
		else
			pDC->FillRect(block.cRect,&m_brushBackground);
		if(block.bMoveLeft == FALSE)
			pDC->FrameRect(block.lRect,&m_brushBackground); 
		else
			pDC->FillRect(block.lRect,&m_brushBackground);
		if(block.bMoveRight == FALSE)
			pDC->FrameRect(block.rRect,&m_brushBackground); 
		else
			pDC->FillRect(block.rRect,&m_brushBackground);
		m_brushBackground.DeleteObject();
	}
	// draw 
	CString strScope = this->GetStationSidesName(block.station, block.count, block.ele_number);		

	CString dcmdno, reason, head;
	this->ParseMarkReasonText(block.text, dcmdno, reason);
	if(!dcmdno.IsEmpty())
		head = "揭示命令号:" + dcmdno + "  ";
	else
		head = "";

	CString strName, strSpeed;
	strSpeed.Format(" 限速%dkm/h", GetSlowSpeed(block.flag));

	CString strKm = block.GetKiloMDesp();
	if(strKm.IsEmpty())
	{
		strName = head + strScope +  reason;		
	}
	else
	{
		strName = head + strScope + strKm + reason + strSpeed;
	}

	COLORREF oldTextColor = pDC->SetTextColor(m_ptgctrl->mConfigTg.m_SideSlowMarkCfg.color3);
	UINT oldAlign = pDC->SetTextAlign(TA_CENTER|TA_BOTTOM);
	CFont *oldFont = pDC->SelectObject(&mBlockFont);

	CSize sz = pDC->GetTextExtent(strName);
	// 输出文本
	if(IsLineTextPosBottom(block.line))
		pDC->TextOut (Xmiddle, Ymiddle+sz.cy+2, strName);
	else if(IsLineTextPosMiddle(block.line))
		pDC->TextOut (Xmiddle, Ymiddle+sz.cy/2, strName);
	else
		pDC->TextOut (Xmiddle, Ymiddle, strName);

	// 刷新矩形
	int tmpX1 = x1, tmpX2 = x2;
	if(abs(x2-x1) < sz.cx)
	{
		tmpX1 = Xmiddle - sz.cx/2;
		tmpX2 = Xmiddle + sz.cx/2;
	}
	block.myPosition.SetRect(tmpX1-10, y1-20, tmpX2+10, y2+20);
	// 输出时间
	pDC->SelectObject(&mBlockFontTime);
	pDC->SetTextColor(m_ptgctrl->mConfigTg.m_SideSlowMarkCfg.color3);

	CString time_char_left;
	CString time_char_right;
	time_char_left.Format("%d", block.start_time/60 % 10);
	time_char_right.Format("%d", block.end_time/60 % 10);

	sz = pDC->GetTextExtent(time_char_left);
	if(this->m_ptgctrl->mConfigTg.m_SideSlowMarkCfg.time_pos == 0)
	{
		pDC->TextOut(x1, y1-2, time_char_left);
		pDC->TextOut(x1, y2+sz.cy+4, time_char_left);
		pDC->TextOut(x2, y1-2, time_char_right);
		pDC->TextOut(x2, y2+sz.cy+4, time_char_right);
	}
	else
	{
		pDC->TextOut(x1-sz.cx, Ymiddle+sz.cy/2, time_char_left);
		pDC->TextOut(x2+sz.cx, Ymiddle+sz.cy/2, time_char_right);
	}
	
	pDC->SetTextAlign(oldAlign);
	pDC->SetTextColor(oldTextColor);
	pDC->SelectObject(oldFont);
}

void CTGDoc::DrawSideOccupyMark(CTg_BLOCK& block, CDC *pDC)
{
	UINT nIndex = block.index;
	if(nIndex < 0 || nIndex >= this->m_nDispDistanceNum || nIndex>=512)
	{
		sysprint(5,0, "由于站序 %d 不在有效显示区内, 不显示该施工符号block %d, type %d ", block.index, block.mark_index, block.type);
		return ;
	}

	int x1,x2,y1,y2;
	if(!IsShowStationMark(block, nIndex, y1, y2))
		return;

	if(y1<=0 || y2<=0) 
		return;
	
	block.station1 = block.station;
	block.station2 = block.station;

	x1 = this->GetOffsetMinuteFromGraphBegin(block.start_time)*m_fPixelPerMinute;
	x2 = this->GetOffsetMinuteFromGraphBegin(block.end_time)*m_fPixelPerMinute;

	if(x1 < mGraphBeginPixel) x1 = mGraphBeginPixel;
	if(x2 > mGraphEndPixel)   x2 = mGraphEndPixel+10;

	if(x1<0) 
		x1 = 0;
	if(x2<0) 
		x2 = 0;

	if(x1 == 0 && x2 == 0)
		return ;

	block.m_drawRect.SetRect(x1, y1-4, x2, y2+4);
	int Ymiddle = block.m_drawRect.CenterPoint().y - 2;
	int Xmiddle = block.m_drawRect.CenterPoint().x;
	
	CPen nPen, *pOldPen;
	COLORREF color = m_ptgctrl->mConfigTg.m_SideOccupyMarkCfg.color1;

	if (m_ptgctrl->mConfigTg.m_context.IsCanModifyBlockStatus() && IsBlockComplete(block.adjust_status))
		color = m_ptgctrl->mConfigTg.m_SideOccupyMarkCfg.off_color;
	nPen.CreatePen(PS_SOLID, 2, color);

	pOldPen = pDC->SelectObject(&nPen);

	// 绘制竖线
	pDC->MoveTo(block.m_drawRect.left, block.m_drawRect.top);  
	pDC->LineTo(block.m_drawRect.left, block.m_drawRect.bottom);

	pDC->MoveTo(block.m_drawRect.right, block.m_drawRect.top);  
	pDC->LineTo(block.m_drawRect.right, block.m_drawRect.bottom);

	// 绘制横线
	pDC->MoveTo(block.m_drawRect.left, Ymiddle);  
	pDC->LineTo(block.m_drawRect.right, Ymiddle);

	pDC->SelectObject(pOldPen);
	nPen.DeleteObject();

	block.cRect.SetRect(Xmiddle-5,Ymiddle-5,Xmiddle+5,Ymiddle+5);
	block.lRect.SetRect(x1-5,Ymiddle-5,x1+5,Ymiddle+5);
	block.rRect.SetRect(x2-5,Ymiddle-5,x2+5,Ymiddle+5);

	// 绘制焦点
	if(block.m_bFocus == TRUE)
	{
		CBrush  m_brushBackground;
		m_brushBackground.CreateSolidBrush(color);
		if(block.bMoveCenter == FALSE)
			pDC->FrameRect(block.cRect,&m_brushBackground); 
		else
			pDC->FillRect(block.cRect,&m_brushBackground);
		if(block.bMoveLeft == FALSE)
			pDC->FrameRect(block.lRect,&m_brushBackground); 
		else
			pDC->FillRect(block.lRect,&m_brushBackground);
		if(block.bMoveRight == FALSE)
			pDC->FrameRect(block.rRect,&m_brushBackground); 
		else
			pDC->FillRect(block.rRect,&m_brushBackground);
		m_brushBackground.DeleteObject();
	}
	// draw 
	CString strScope = this->GetStationSidesName(block.station, block.count, block.ele_number);				

	CString dcmdno, reason, head;
	this->ParseMarkReasonText(block.text, dcmdno, reason);
	if(!dcmdno.IsEmpty())
		head = "揭示命令号:" + dcmdno + "  ";
	else
		head = "";

	CString strKm = block.GetKiloMDesp();
	if(strKm.IsEmpty())
	{
		strScope = head + strScope +  reason;		
	}
	else
	{
		strScope = head + strScope + strKm + reason;
	}

	COLORREF oldTextColor = pDC->SetTextColor(m_ptgctrl->mConfigTg.m_SideOccupyMarkCfg.color3);
	UINT oldAlign = pDC->SetTextAlign(TA_CENTER|TA_BOTTOM);
	CFont *oldFont = pDC->SelectObject(&mBlockFont);

	CSize sz = pDC->GetTextExtent(strScope);
	// 输出文本
	if(IsLineTextPosBottom(block.line))
		pDC->TextOut (Xmiddle, Ymiddle+sz.cy+2, strScope);
	else if(IsLineTextPosMiddle(block.line))
		pDC->TextOut (Xmiddle, Ymiddle+sz.cy/2, strScope);
	else
		pDC->TextOut (Xmiddle, Ymiddle, strScope);

	// 刷新矩形
	int tmpX1 = x1, tmpX2 = x2;
	if(abs(x2-x1) < sz.cx)
	{
		tmpX1 = Xmiddle - sz.cx/2;
		tmpX2 = Xmiddle + sz.cx/2;
	}
	block.myPosition.SetRect(tmpX1-10, y1-20, tmpX2+10, y2+20);

	// 输出时间
	pDC->SelectObject(&mBlockFontTime);
	pDC->SetTextColor(m_ptgctrl->mConfigTg.m_SideOccupyMarkCfg.color3);

	CString time_char_left;
	CString time_char_right;
	time_char_left.Format("%d", block.start_time/60 % 10);
	time_char_right.Format("%d", block.end_time/60 % 10);

	sz = pDC->GetTextExtent(time_char_left);
	if(this->m_ptgctrl->mConfigTg.m_SideOccupyMarkCfg.time_pos == 0)
	{
		pDC->TextOut(x1, y1-2, time_char_left);
		pDC->TextOut(x1, y2+sz.cy+4, time_char_left);
		pDC->TextOut(x2, y1-2, time_char_right);
		pDC->TextOut(x2, y2+sz.cy+4, time_char_right);
	}
	else
	{
		pDC->TextOut(x1-sz.cx, Ymiddle+sz.cy/2, time_char_left);
		pDC->TextOut(x2+sz.cx, Ymiddle+sz.cy/2, time_char_right);
	}

	pDC->SetTextAlign(oldAlign);
	pDC->SetTextColor(oldTextColor);
	pDC->SelectObject(oldFont);
}

void CTGDoc::DrawSideTianchuangMark(CTg_BLOCK& block, CDC *pDC)
{
	UINT nIndex = block.index;
	if(nIndex < 0 || nIndex >= this->m_nDispDistanceNum || nIndex>=512)
	{
		sysprint(5,0, "由于站序 %d 不在有效显示区内, 不显示该施工符号block %d, type %d ", block.index, block.mark_index, block.type);
		return ;
	}
	
	int x1,x2,y1,y2;
	if(!IsShowStationMark(block, nIndex, y1, y2))
		return;

	if(y1<=0 || y2<=0) 
		return;

	block.station1 = block.station;
	block.station2 = block.station;

	x1 = this->GetOffsetMinuteFromGraphBegin(block.start_time)*m_fPixelPerMinute;
	x2 = this->GetOffsetMinuteFromGraphBegin(block.end_time)*m_fPixelPerMinute;

	if(x1 < mGraphBeginPixel) x1 = mGraphBeginPixel;
	if(x2 > mGraphEndPixel)   x2 = mGraphEndPixel+10;

	if(x1<0) 
		x1 = 0;
	if(x2<0) 
		x2 = 0;

	if(x1 == 0 && x2 == 0)
		return ;

	block.m_drawRect.SetRect(x1, y1-4, x2, y2+4);
	int Ymiddle = block.m_drawRect.CenterPoint().y - 2;
	int Xmiddle = block.m_drawRect.CenterPoint().x;

	CPen nPen, *pOldPen;
	COLORREF color = m_ptgctrl->mConfigTg.m_SideTianchuangMarkCfg.color1;
	if (m_ptgctrl->mConfigTg.m_context.IsCanModifyBlockStatus() && IsBlockComplete(block.adjust_status))
		color = m_ptgctrl->mConfigTg.m_SideTianchuangMarkCfg.off_color;	

	nPen.CreatePen(PS_SOLID, 2, color);

	pOldPen = pDC->SelectObject(&nPen);

	// 绘制竖线
	pDC->MoveTo(block.m_drawRect.left, block.m_drawRect.top);  
	pDC->LineTo(block.m_drawRect.left, block.m_drawRect.bottom);

	pDC->MoveTo(block.m_drawRect.right, block.m_drawRect.top);  
	pDC->LineTo(block.m_drawRect.right, block.m_drawRect.bottom);

	// 绘制横线
	pDC->MoveTo(block.m_drawRect.left, Ymiddle);  
	pDC->LineTo(block.m_drawRect.right, Ymiddle);

	pDC->SelectObject(pOldPen);
	nPen.DeleteObject();

	block.cRect.SetRect(Xmiddle-5,Ymiddle-5,Xmiddle+5,Ymiddle+5);
	block.lRect.SetRect(x1-5,Ymiddle-5,x1+5,Ymiddle+5);
	block.rRect.SetRect(x2-5,Ymiddle-5,x2+5,Ymiddle+5);

	// 绘制焦点
	if(block.m_bFocus == TRUE)
	{
		CBrush  m_brushBackground;
		m_brushBackground.CreateSolidBrush(color);
		if(block.bMoveCenter == FALSE)
			pDC->FrameRect(block.cRect,&m_brushBackground); 
		else
			pDC->FillRect(block.cRect,&m_brushBackground);
		if(block.bMoveLeft == FALSE)
			pDC->FrameRect(block.lRect,&m_brushBackground); 
		else
			pDC->FillRect(block.lRect,&m_brushBackground);
		if(block.bMoveRight == FALSE)
			pDC->FrameRect(block.rRect,&m_brushBackground); 
		else
			pDC->FillRect(block.rRect,&m_brushBackground);
		m_brushBackground.DeleteObject();
	}
	// draw 
	CString strScope = this->GetStationSidesName(block.station, block.count, block.ele_number);			
	
	CString dcmdno, reason, head;
	this->ParseMarkReasonText(block.text, dcmdno, reason);
	if(!dcmdno.IsEmpty())
		head = "揭示命令号:" + dcmdno + "  ";
	else
		head = "";

	CString strKm = block.GetKiloMDesp();
	if(strKm.IsEmpty())
	{
		strScope = head + strScope +  reason;		
	}
	else
	{
		strScope = head + strScope + strKm + reason;
	}
	
	COLORREF oldTextColor = pDC->SetTextColor(m_ptgctrl->mConfigTg.m_SideTianchuangMarkCfg.color3);
	UINT oldAlign = pDC->SetTextAlign(TA_CENTER|TA_BOTTOM);
	CFont *oldFont = pDC->SelectObject(&mBlockFont);

	CSize sz = pDC->GetTextExtent(strScope);
	// 输出文本
	if(IsLineTextPosBottom(block.line))
		pDC->TextOut (Xmiddle, Ymiddle+sz.cy+2, strScope);
	else if(IsLineTextPosMiddle(block.line))
		pDC->TextOut (Xmiddle, Ymiddle+sz.cy/2, strScope);
	else
		pDC->TextOut (Xmiddle, Ymiddle, strScope);

	// 刷新矩形
	int tmpX1 = x1, tmpX2 = x2;
	if(abs(x2-x1) < sz.cx)
	{
		tmpX1 = Xmiddle - sz.cx/2;
		tmpX2 = Xmiddle + sz.cx/2;
	}
	block.myPosition.SetRect(tmpX1-10, y1-20, tmpX2+10, y2+20);

	// 输出时间
	pDC->SelectObject(&mBlockFontTime);
	pDC->SetTextColor(m_ptgctrl->mConfigTg.m_SideTianchuangMarkCfg.color3);

	CString time_char_left;
	CString time_char_right;
	time_char_left.Format("%d", block.start_time/60 % 10);
	time_char_right.Format("%d", block.end_time/60 % 10);

	sz = pDC->GetTextExtent(time_char_left);
	if(this->m_ptgctrl->mConfigTg.m_SideTianchuangMarkCfg.time_pos == 0)
	{
		pDC->TextOut(x1, y1-2, time_char_left);
		pDC->TextOut(x1, y2+sz.cy+4, time_char_left);
		pDC->TextOut(x2, y1-2, time_char_right);
		pDC->TextOut(x2, y2+sz.cy+4, time_char_right);
	}
	else
	{
		pDC->TextOut(x1-sz.cx, Ymiddle+sz.cy/2, time_char_left);
		pDC->TextOut(x2+sz.cx, Ymiddle+sz.cy/2, time_char_right);
	}

	pDC->SetTextAlign(oldAlign);
	pDC->SetTextColor(oldTextColor);
	pDC->SelectObject(oldFont);
}

void CTGDoc::DrawNoteTextMark(CTg_BLOCK& block, CDC *pDC)
{
	int nRatio = block.line;

	int y1 = block.station;
	int y2 = block.position;

	int x1 = this->GetOffsetMinuteFromGraphBegin(block.start_time)*m_fPixelPerMinute;
	int x2 = this->GetOffsetMinuteFromGraphBegin(block.end_time)*m_fPixelPerMinute;
	
	if(x1<0) x1 = 0;
	if(x2<0) x2 = 0;

	block.station1 = 0;
	block.station2 = 0;

	if(x1 == 0 && x2 == 0)
		return ;

	if(abs(y2-y1) < 20)
		y2 = y1 + 20;
	if(abs(x2-x1) < 20)
		x2 = x1 + 20;
	// 图表注解
	block.m_drawRect.left  = x1;
	block.m_drawRect.right = x2;
	block.m_drawRect.top    = y1;
	block.m_drawRect.bottom = y2;

	int Ymiddle = block.m_drawRect.CenterPoint().y;
	int Xmiddle = block.m_drawRect.CenterPoint().x;

	block.cRect.SetRect(Xmiddle-8, Ymiddle-8, Xmiddle+8, Ymiddle+8);
	block.lRect.SetRect(x1-5, Ymiddle-5, x1+5, Ymiddle+5);
	block.rRect.SetRect(x2-5, Ymiddle-5, x2+5, Ymiddle+5);
	block.tRect.SetRect(Xmiddle-5, y1-5, Xmiddle+5, y1+5);
	block.bRect.SetRect(Xmiddle-5, y2-5, Xmiddle+5, y2+5);

	if(block.m_bFocus == TRUE)
	{
		pDC->FrameRect(block.m_drawRect,&m_brushBlack); 
		if(block.bMoveCenter == FALSE)
			pDC->FrameRect(block.cRect,&m_brushBlack);  
		else
			pDC->FillRect(block.cRect,&m_brushBlack); 
		if(block.bMoveLeft == FALSE)
			pDC->FrameRect(block.lRect,&m_brushBlack);  
		else
			pDC->FillRect(block.lRect,&m_brushBlack);
		if(block.bMoveRight == FALSE)
			pDC->FrameRect(block.rRect,&m_brushBlack); 
		else
			pDC->FillRect(block.rRect,&m_brushBlack);

		if(block.bMoveTop == FALSE)
			pDC->FrameRect(block.tRect,&m_brushBlack);  
		else
			pDC->FillRect(block.tRect,&m_brushBlack);
		if(block.bMoveBottom == FALSE)
			pDC->FrameRect(block.bRect,&m_brushBlack); 
		else
			pDC->FillRect(block.bRect,&m_brushBlack);
	}

	CString strName = CString(block.text);		
	if(IsRectBlue(block.slow_time))
	{
		CPen *oldPen = NULL;
		CPen bluePen;
		bluePen.CreatePen(PS_INSIDEFRAME,2,CLR_BLUE);
		oldPen = pDC->SelectObject(&bluePen);

		FrameRectExtend(pDC,block.m_drawRect);

		pDC->SelectObject(oldPen);
		bluePen.DeleteObject();
	}
	else if(IsRectRed(block.slow_time))
	{
		CPen *oldPen = NULL;
		CPen redPen;
		redPen.CreatePen(PS_INSIDEFRAME,2,CLR_RED);
		oldPen = pDC->SelectObject(&redPen);

		FrameRectExtend(pDC,block.m_drawRect);

		pDC->SelectObject(oldPen);
		redPen.DeleteObject();
	}
	else if(IsCircleRed(block.slow_time))
	{
		CPen *oldPen = NULL;
		CPen redPen;
		redPen.CreatePen(PS_INSIDEFRAME,2,CLR_RED);
		oldPen = pDC->SelectObject(&redPen);

		pDC->Arc(block.m_drawRect,
			CPoint(block.m_drawRect.right, block.m_drawRect.CenterPoint().y),
			CPoint(block.m_drawRect.right, block.m_drawRect.CenterPoint().y));

		pDC->SelectObject(oldPen);
		redPen.DeleteObject();
	}
	else if(IsCircleBlue(block.slow_time))
	{
		CPen *oldPen = NULL;
		CPen bluePen;
		bluePen.CreatePen(PS_INSIDEFRAME,2,CLR_BLUE);
		oldPen = pDC->SelectObject(&bluePen);

		pDC->Arc(block.m_drawRect,
			CPoint(block.m_drawRect.right, block.m_drawRect.CenterPoint().y),
			CPoint(block.m_drawRect.right, block.m_drawRect.CenterPoint().y));

		pDC->SelectObject(oldPen);
		bluePen.DeleteObject();
	}

	COLORREF oldTextColor = pDC->SetTextColor(RGB(255,0,0));
	if (IsWordBlue(block.slow_time))
		oldTextColor = pDC->SetTextColor(RGB(0,0,255));

	UINT oldAlign = pDC->SetTextAlign(TA_CENTER|TA_BOTTOM);
	CFont* pOldFont = pDC->SelectObject(&mBlockFont);

	CSize textSize = pDC->GetTextExtent(strName);
	CPoint myPoint = block.m_drawRect.CenterPoint();

	pDC->TextOut (myPoint.x,myPoint.y+textSize.cy/2,strName);

	// 刷新矩形
	int tmpX1 = x1, tmpX2 = x2;
	if(abs(x2-x1) < textSize.cx)
	{
		tmpX1 = myPoint.x - textSize.cx/2;
		tmpX2 = myPoint.x + textSize.cx/2;
	}
	block.myPosition.SetRect(tmpX1-10, y1-20, tmpX2+10, y2+20);

	pDC->SetTextColor(oldTextColor);
	pDC->SetTextAlign(oldAlign);
	pDC->SelectObject(pOldFont);
}

bool CTGDoc::GenDefaultBlock(CTg_BLOCK& block)
{
	bool bResult = false; 

	if(block.direction != 0xFF)
		return false;
	block.direction=0;

	block.station1 = block.station1&0x7fff;
	block.station2 = block.station2&0x7fff;

	if(block.type == ABS_INHIBIT_MARK || block.type == ABS_SLOW_MARK || block.type == ABS_ELE_FIX_MARK)
	{
		block.station -= 1;
		block.index -= 1;
		if((0 > block.station || block.station >= this->m_nDispDistanceNum) ||
			(0 > block.index || block.index >= this->m_nDispDistanceNum))
		{
			sysprint(5,0, "生成block[%d], 类型%d, 车站%d和车站%d, 站序index1:%d index2:%d, 站序非法", 
			block.mark_index, block.type, 
			block.station1, block.station2,
			block.station, block.index);

			return false;
		}
		if(this->m_sDispDistance[block.station].station_number != block.station1)
		{
			sysprint(5,0, "生成block[%d], 类型%d, 车站%d的站序和, TDMS送的站序%d不一致,修正站号%d", 
			block.mark_index, block.type, 
			block.station1,block.station,this->m_sDispDistance[block.station].station_number);

			block.station1 = this->m_sDispDistance[block.station].station_number;
		}
		if(this->m_sDispDistance[block.index].station_number != block.station2)
		{
			sysprint(5,0, "生成block[%d], 类型%d, 车站%d的站序和, TDMS送的站序%d不一致,修正站号为%d", 
			block.mark_index, block.type, 
			block.station2,block.index,this->m_sDispDistance[block.index].station_number);

			block.station2 = this->m_sDispDistance[block.index].station_number;
		}
		
		m_linecount = 0;
		memset(m_line_list,0,sizeof(m_line_list));

		int maxidx = (block.station >= block.index)?block.station : block.index;
		int minidx = (block.station <= block.index)?block.station : block.index;
		for(int idx = minidx; idx < maxidx; idx++)
		{
			if(!GetABSLineNoByIndex(idx, idx+1))
			{
				sysprint(5,0, "警告,生成多区间block[%d], 类型%d, 索引index1:%d 和 index2:%d间没有区间线号", 
					block.mark_index, block.type, idx, idx+1);
			}
		}

		sysprint(5,0, "生成block[%d], 类型%d, 车站%d和车站%d, 站序index1:%d index2:%d", 
			block.mark_index, block.type, 
			block.station1, block.station2,
			block.station, block.index);

		if(block.line == 255)
			SetBlockDriect(block);

		SetLineLinePosMiddle(block.line);
		// 沈阳局特殊模式
		if(m_ptgctrl->mConfigTg.m_context.IsEnableDiffShowBlockPostion())
		{
			if (IsLineDirectionUp(block.line))
				SetLineLinePosTop(block.line);
			else if (IsLineDirectionDown(block.line))
				SetLineLinePosBottom(block.line);
		}

		SetLineTextPosTop(block.line);
		//if(this->m_ptgctrl->mConfigTg.m_context.IsCanRemoveTDMSKiloMark())
		//   block.SetReasonTextFromTDMS();
		bResult = true;
	}
	else if( block.type == SIDE_INHIBIT_MARK || block.type == SIDE_SLOW_MARK)
	{
		block.index -= 1;
		if(0 > block.index || block.index >= this->m_nDispDistanceNum)
		{
			sysprint(5,0, "失败block[%d], 类型%d(4-股道慢行 5-股道封锁),车站%d站序%d非法", 
			block.mark_index, block.type, 
			block.station1,block.index);

			return false;
		}
		if(this->m_sDispDistance[block.index].station_number != block.station1)
		{
			sysprint(5,0, "失败block[%d], 类型%d(4-股道慢行 5-股道封锁),车站%d和TDMS送的站序%d不匹配,修正站号为%d", 
			block.mark_index, block.type, 
			block.station1,block.index,this->m_sDispDistance[block.index].station_number);

			block.station1 = this->m_sDispDistance[block.index].station_number;
		}
		block.station = block.station1;
		sysprint(5,0, "生成block[%d], 类型%d(4-股道慢行 5-股道封锁),车站%d,站序%d", 
								block.mark_index, block.type, block.station1, block.index);

		// 设置股道
		for(int k=0; k<block.count&&k<MAX_SCHD_BLOCK_ELE_COUNT;k++)
		{
			int nSideNo = block.ele_number[k];
			int i=0;
			for(i=0; i<this->m_sDispDistance[block.index].number_of_side; i++)
			{
				if(nSideNo == this->m_sDispDistance[block.index].side[i].side_number)
				{
					this->sysprint(5,0, "站内标记,包含股道(%d %s)", nSideNo, this->m_sDispDistance[block.index].side[i].sidename);
					break;
				}
			}

			if(i>=this->m_sDispDistance[block.index].number_of_side)
			{
				this->sysprint(5,0, "站内标记,TDMS送来非法股道(%d)", nSideNo);
			}
		}

		// 文本位置上面
		SetLineTextPosTop(block.line);
		//if(this->m_ptgctrl->mConfigTg.m_context.IsCanRemoveTDMSKiloMark())
		//	block.SetReasonTextFromTDMS();
		bResult = true;
	}
	else
	{
		bResult = false;
		this->sysprint(5,0, "生成标记失败,遇到未知类型%d(1-区间封锁 2-综合天窗修　3-区间慢行 4-股道慢行 5-股道封锁 7-图解标注 9-变更闭塞 10-股道天窗 11-电网检修 12-股道占用)", block.type);
	}
	return bResult;
}

bool CTGDoc::GetIndexByStation(WORD stano1, WORD stano2, int& index1, int& index2)
{
	for(int idx = 0; idx < this->m_nDispDistanceNum-1; idx++)
	{
		if(this->m_sDispDistance[idx].station_number != stano1)
			continue;

		for(int idx1 = idx+1; idx1 < this->m_nDispDistanceNum; idx1++)
		{
			if(this->m_sDispDistance[idx1].station_number == stano2)
			{
				if(this->m_sDispDistance[idx].sub_section == this->m_sDispDistance[idx1].sub_section)
				{
					index1 = idx;
					index2 = idx1;
					return true;
				}
			}
		}
	}

	for(int idx = 0; idx < this->m_nDispDistanceNum-1; idx++)
	{
		if(this->m_sDispDistance[idx].station_number != stano2)
			continue;

		for(int idx1 = idx+1; idx1 < this->m_nDispDistanceNum; idx1++)
		{
			if(this->m_sDispDistance[idx1].station_number == stano1)
			{
				if(this->m_sDispDistance[idx].sub_section == this->m_sDispDistance[idx1].sub_section)
				{
					index1 = idx;
					index2 = idx1;
					return true;
				}
			}
		}
	}

	index1 = -1;
	index2 = -1;
	return false;
}

bool CTGDoc::GetABSLineNoByIndex(int index1, int index2)
{
	if(index1 < 0 || index1 >= this->m_nDispDistanceNum)
		return false;
	if(index2 < 0 || index2 >= this->m_nDispDistanceNum)
		return false;
	if(index1 == index2)
		return false;

	int up_idx   = index1;
	int down_idx = index2;
	if(index1 > index2)
	{
		up_idx   = index2;
		down_idx = index1;
	}

	DispDistance *node_up = &this->m_sDispDistance[up_idx];
	DispDistance *node_dn = &this->m_sDispDistance[down_idx];

	int nStationUp = node_up->station_number;
	int nStationDn = node_dn->station_number;
	for(int i=0; i<node_up->down_line_count; i++)
	{
		for(int j=0; j<node_dn->up_line_count; j++)
		{
			if(node_up->down_line_list[i] != node_dn->up_line_list[j])
				continue;

			int line_no = node_up->down_line_list[i];
			const ABS_LINE* line = this->m_ptgctrl->mConfigTg.m_tgdef.GetLineDefine(line_no);
			if(line!=NULL)
			{
				if((nStationUp == line->nStartStation && nStationDn == line->nEndStation)
					|| (nStationDn == line->nStartStation && nStationUp == line->nEndStation))
				{
					m_line_list[m_linecount].lineNo = line_no;
					m_line_list[m_linecount].dir    = line->nDirect;
					m_linecount++;
				}
				else
				{
					CString str;
					str.Format("line %d, 不位于%d和%d之间,请检查TG.def配置", line_no, nStationUp, nStationDn);
					this->sysprint(5,0, "[TG] %s", str);
				}
			}
			else
			{
				CString str;
				str.Format("找不到 line %d,  它位于%d和%d之间,请检查TG.def配置", line_no, nStationUp, nStationDn);
				this->sysprint(5,0, "[TG] %s", str);
			}

			if(m_linecount >= sizeof(m_line_list)/sizeof(m_line_list[0]))
			{
				CString str;
				str.Format("找不到 line %d,  它位于%d和%d之间,请检查TG.def配置", line_no, nStationUp, nStationDn);
				this->sysprint(5,0, "[TG] %s", str);
				return true;
			}
			break;
		}
	}	

	if(m_linecount <= 0)
		return false;
	else
		return true;
}

void CTGDoc::SetBlockDriect(CTg_BLOCK& block)
{
	if(block.line != 255)
		return;
	block.line &= 0xFFFC;

	int nUpFlag=0, nDownFlag=0, nOneDirFlag=0;
	for(int i = 0; i < block.count && i < sizeof(block.ele_number)/sizeof(block.ele_number[0]); i++)
	{
		int absNo = block.ele_number[i];
		bool bFind = false;
		for(int i = 0; i < m_linecount; i++)
		{
			if(absNo == m_line_list[i].lineNo)
			{
				bFind = true;
				break;
			}
		}

		if(!bFind)
		{
			CString msgErr;
			msgErr.Format("接收到TMIS区间号%d, 但是它不是车站%d和%d之间的区间号", absNo,
				block.station1, block.station2);
			this->sysprint(5,0, "[TG] %s", msgErr);
			//continue;
		}

		const ABS_LINE* line = m_ptgctrl->mConfigTg.m_tgdef.GetLineDefine(absNo);
		if(line!=NULL)
		{
			switch(line->nDirect)
			{
			case 0:
				nUpFlag=1;
				break;
			case 1:
				nDownFlag=1;
				break;
			case 3:
				nOneDirFlag=1;
				break;
			}

			CString msg;
			msg.Format("Block(%d)存在区间号%d:方向%d由车站%d到车站%d", 
				block.mark_index, absNo, line->nDirect,line->nStartStation,line->nEndStation);

			this->sysprint(4,0, "[TG] %s", msg);
		}
		else
		{
			CString msg;
			msg.Format("Block(%d)存在区间号%d,但是TG.def中没有它的配置", 
				block.mark_index, absNo);

			this->sysprint(5,0, "[TG] %s", msg);
		}
	}

	if(nUpFlag == 1)
	{
		if(nDownFlag == 1)
		{
			block.line |= 0x0002; // 上下行
		}
		else
		{
			; // 上行
		}
	}
	else
	{
		if(nDownFlag == 1)
		{
			block.line |= 0x0001; // 下行
		}
		else
		{
			if(nOneDirFlag == 1)
			{
				block.line |= 0x0003; // 单线
			}
			else
			{
				;//上行
			}
		}
	}
}

CString CTGDoc::GetStationSidesName(const int nStaNo, const USHORT nSidesNum, const USHORT nSidesNo[])
{	
	int nIndex;
	for(nIndex=0; nIndex < this->m_nDispDistanceNum; nIndex++)
	{
		if(this->m_sDispDistance[nIndex].station_number == nStaNo)
			break;
	}  	
	if(nIndex >= this->m_nDispDistanceNum)
		return " ";

	if(nIndex < 0 || nIndex >= this->m_nDispDistanceNum)
		return " ";
	
	if(nSidesNum == m_sDispDistance[nIndex].number_of_side)
		return " ";

	CString strScope, str;
	for(USHORT i = 0; i < nSidesNum; i++)
	{
		str = "";

		for(int p=0; p<m_sDispDistance[nIndex].number_of_side; p++)
		{
			if(m_sDispDistance[nIndex].side[p].side_number == nSidesNo[i])  
			{
				if(i == nSidesNum -1)
					str.Format("%s ",this->m_sDispDistance[nIndex].side[p].sidename);
				else
					str.Format("%s,",this->m_sDispDistance[nIndex].side[p].sidename);

				break;
			}
		}

		strScope += str;
	}

	return strScope;
}

CString CTGDoc::GetABSStrDirectDesp(int nDirect, USHORT nAbsNo, USHORT nAbsNum)
{
	bool bSingleAbs = false;
	if(nAbsNum == 1)
	{
		if(3 == this->m_ptgctrl->mConfigTg.m_tgdef.GetLineDirection(nAbsNo))
			bSingleAbs = true;
	}

	CString strDirection = "";

	if(!bSingleAbs)
	{
		if(nDirect == 1)
			strDirection = "上行";
		else if(nDirect == 2)
			strDirection = "下行";
		else if(nDirect == 3)
			strDirection = "上下行";
		else
			strDirection = "";
	}

	return strDirection;
}

void  CTGDoc::SetGraphPosition(CTg_BLOCK* block, CRect rect)
{
	block->station = 0;
	block->position = 0;
	block->index=0;
	// 计算ID1和ID2
	BYTE nID1=0xFF, nID2=0xFF;
   
	for(int idx=this->m_nDispDistanceNum-1; idx>=0; idx--)
	{
		if(this->m_sDispDistance[idx].nSecondStationYPos <= rect.top)
		{
			nID1 = (BYTE)idx;
			break;
		}
	}

	for(int idx=0; idx<this->m_nDispDistanceNum; idx++)
	{
		if(this->m_sDispDistance[idx].nStationYPos >= rect.bottom)
		{
			nID2 = idx;
			break;
		}
	}
	int Y1=0, Y2=this->m_nPageHeight;
	if(0xFF == nID1 && 0xFF == nID2)
	{
		Y1=0;
		Y2=this->m_nPageHeight;
	}
	if(0xFF == nID1)
	{
		Y1 = 0;
		Y2 = this->m_sDispDistance[nID2].nStationYPos;
	}
	else if(0xFF == nID2)
	{
		Y1 = this->m_sDispDistance[nID1].nSecondStationYPos;
		Y2 = this->m_nPageHeight;
	}
	else
	{
		Y1 = this->m_sDispDistance[nID1].nSecondStationYPos;
		Y2 = this->m_sDispDistance[nID2].nStationYPos;
	}

	block->s_meter = nID2;
	block->s_meter = (block->s_meter<<8);
	block->s_meter = (block->s_meter&0xFF00) + nID1;

	// 中心位置比例
	WORD MidRate = abs((rect.CenterPoint().y-Y1)*100)/abs(Y2-Y1);
	block->e_kilo = rect.Height();
	block->e_meter = MidRate;
}

void CTGDoc::DrawNoteTextMark2(CTg_BLOCK& block, CDC *pDC)
{
	int nID1   = block.s_meter&0x00FF; 
	int nID2 = (block.s_meter&0xFF00)>>8;
	int nMidRate   = block.e_meter;
	if(nMidRate > 100 || nMidRate <= 0)
	{
		nMidRate=50;
		return;
	}	
	int Y1, Y2;
	// 位于最上站平线上方---最上站平线和底图最上边界确定位置
	if(nID1 == 0xFF)
	{
		Y1 = 0; // 底图最上边界位置
		if(nID2 < 0 || nID2 >= this->m_nDispDistanceNum||nID2>=512)
			return;

		Y2 = this->m_sDispDistance[nID2].nStationYPos; 
	}
	// 位于最下站平线下方---最下站平线和底图最下边界确定位置
	else if(nID2 == 0xFF)
	{
		if(nID1 < 0 || nID1 >= this->m_nDispDistanceNum||nID1>=512)
			return;
		Y1 = this->m_sDispDistance[nID1].nSecondStationYPos;
		Y2 = m_nPageHeight;
	}
	// 位于两个站平线间---两个站平线确定位置
	else
	{
		if(nID1 < 0 || nID1 >= this->m_nDispDistanceNum || nID1>=512)
			return;
		if(nID2 < 0 || nID2 >= this->m_nDispDistanceNum || nID2>=512)
			return;

		Y1 = this->m_sDispDistance[nID1].nSecondStationYPos; // 上站平线位置
		Y2 = this->m_sDispDistance[nID2].nStationYPos; // 下站平线位置
	}

	int MidY = Y1 + ((Y2-Y1)*nMidRate)/100;
	
	int y1 = MidY - block.e_kilo/2;
	int y2 = MidY + block.e_kilo/2;
	if(y1 < 0)
	{
		y1 = 0;
		y2 += abs(MidY - block.e_kilo/2);
	}
	if(y2>this->m_nPageHeight)
	{
		y1 -= y2-this->m_nPageHeight;
		y2 = this->m_nPageHeight;
	}
	
	int x1 = this->GetOffsetMinuteFromGraphBegin(block.start_time)*m_fPixelPerMinute;
	int x2 = this->GetOffsetMinuteFromGraphBegin(block.end_time)*m_fPixelPerMinute;
	
	if(x1<0) x1 = 0;
	if(x2<0) x2 = 0;

	block.station1 = 0;
	block.station2 = 0;

	if(x1 == 0 && x2 == 0)
		return;

	if(abs(y2-y1) < 20)
		y2 = y1 + 20;
	if(abs(x2-x1) < 20)
		x2 = x1 + 20;
	// 图表注解
	block.m_drawRect.left  = x1;
	block.m_drawRect.right = x2;
	block.m_drawRect.top    = y1;
	block.m_drawRect.bottom = y2;

	int Ymiddle = block.m_drawRect.CenterPoint().y;
	int Xmiddle = block.m_drawRect.CenterPoint().x;

	block.cRect.SetRect(Xmiddle-8, Ymiddle-8, Xmiddle+8, Ymiddle+8);
	block.lRect.SetRect(x1-5, Ymiddle-5, x1+5, Ymiddle+5);
	block.rRect.SetRect(x2-5, Ymiddle-5, x2+5, Ymiddle+5);
	block.tRect.SetRect(Xmiddle-5, y1-5, Xmiddle+5, y1+5);
	block.bRect.SetRect(Xmiddle-5, y2-5, Xmiddle+5, y2+5);

	if(block.m_bFocus == TRUE)
	{
		pDC->FrameRect(block.m_drawRect,&m_brushBlack); 
		if(block.bMoveCenter == FALSE)
			pDC->FrameRect(block.cRect,&m_brushBlack);  
		else
			pDC->FillRect(block.cRect,&m_brushBlack); 
		if(block.bMoveLeft == FALSE)
			pDC->FrameRect(block.lRect,&m_brushBlack);  
		else
			pDC->FillRect(block.lRect,&m_brushBlack);
		if(block.bMoveRight == FALSE)
			pDC->FrameRect(block.rRect,&m_brushBlack); 
		else
			pDC->FillRect(block.rRect,&m_brushBlack);

		if(block.bMoveTop == FALSE)
			pDC->FrameRect(block.tRect,&m_brushBlack);  
		else
			pDC->FillRect(block.tRect,&m_brushBlack);
		if(block.bMoveBottom == FALSE)
			pDC->FrameRect(block.bRect,&m_brushBlack); 
		else
			pDC->FillRect(block.bRect,&m_brushBlack);
	}

	CString strName = CString(block.text);		
	if(IsRectBlue(block.slow_time))
	{
		CPen bluePen;
		bluePen.CreatePen(PS_INSIDEFRAME,2,CLR_BLUE);
		CPen *oldPen = pDC->SelectObject(&bluePen);

		FrameRectExtend(pDC,block.m_drawRect);

		pDC->SelectObject(oldPen);
		bluePen.DeleteObject();
	}
	else if(IsRectRed(block.slow_time))
	{
		CPen redPen;
		redPen.CreatePen(PS_INSIDEFRAME,2,CLR_RED);
		CPen *oldPen = pDC->SelectObject(&redPen);

		FrameRectExtend(pDC,block.m_drawRect);

		pDC->SelectObject(oldPen);
		redPen.DeleteObject();
	}
	else if(IsCircleRed(block.slow_time))
	{
		CPen redPen;
		redPen.CreatePen(PS_INSIDEFRAME,2,CLR_RED);
		CPen *oldPen = pDC->SelectObject(&redPen);

		pDC->Arc(block.m_drawRect,
			CPoint(block.m_drawRect.right, block.m_drawRect.CenterPoint().y),
			CPoint(block.m_drawRect.right, block.m_drawRect.CenterPoint().y));

		pDC->SelectObject(oldPen);
		redPen.DeleteObject();
	}
	else if(IsCircleBlue(block.slow_time))
	{
		CPen bluePen;
		bluePen.CreatePen(PS_INSIDEFRAME,2,CLR_BLUE);
		CPen *oldPen = pDC->SelectObject(&bluePen);

		pDC->Arc(block.m_drawRect,
			CPoint(block.m_drawRect.right, block.m_drawRect.CenterPoint().y),
			CPoint(block.m_drawRect.right, block.m_drawRect.CenterPoint().y));

		pDC->SelectObject(oldPen);
		bluePen.DeleteObject();
	}

	CFont m_Font;
	m_Font.CreateFont(block.m_drawRect.Height(), 0,0,0, FW_NORMAL, 0,0,0, DEFAULT_CHARSET,
		OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_DONTCARE, "Arial");

	COLORREF oldTextColor = pDC->SetTextColor(RGB(255,0,0));
	if (IsWordBlue(block.slow_time))
		oldTextColor = pDC->SetTextColor(RGB(0,0,255));

	UINT oldAlign = pDC->SetTextAlign(TA_CENTER|TA_BOTTOM);
	CFont* pOldFont = pDC->SelectObject(&m_Font);

	CSize textSize = pDC->GetTextExtent(strName);
	CPoint myPoint = block.m_drawRect.CenterPoint();

	pDC->TextOut (myPoint.x,myPoint.y+textSize.cy/2,strName);

	// 刷新矩形
	int tmpX1 = x1, tmpX2 = x2;
	if(abs(x2-x1) < textSize.cx)
	{
		tmpX1 = myPoint.x - textSize.cx/2;
		tmpX2 = myPoint.x + textSize.cx/2;
	}
	block.myPosition.SetRect(tmpX1-10, y1-20, tmpX2+10, y2+20);

	pDC->SetTextColor(oldTextColor);
	pDC->SetTextAlign(oldAlign);
	pDC->SelectObject(pOldFont);
	m_Font.DeleteObject();
}

// 兰州画法 G21
void CTGDoc::DrawLanzhouAbsInhibitMark(CTg_BLOCK& block, CDC *pDC, int nDirect)
{
	UINT nMinIndex = (block.station < block.index)?block.station:block.index;
	UINT nMaxIndex = (block.station > block.index)?block.station:block.index;
	
	if(nMinIndex<0||nMinIndex>=512 || nMaxIndex<0||nMaxIndex>=512)
	{
		sysprint(5,0, "站序1 %d  站序2 %d 越界(0<=index<512), 不显示该施工符号block %d, type %d ", block.index, block.station, block.mark_index, block.type);
		return ;
	}
	
	if(nMinIndex >=this->m_nDispDistanceNum)
	{
		sysprint(5,0, "站序1 %d  站序2 %d 不在有效显示区内, 不显示该施工符号block %d, type %d ", block.index, block.station, block.mark_index, block.type);
		return ;
	}

	if(nMaxIndex >=this->m_nDispDistanceNum)
	{
		nMaxIndex=this->m_nDispDistanceNum-1;
		if(nMinIndex>=nMaxIndex)
		{
			sysprint(5,0, "由于站序1 %d  站序2 %d 不在有效显示区内, 不显示该施工符号block %d, type %d ", block.index, block.station, block.mark_index, block.type);
			return ;
		}
	}

	int y1,y2;
	if(this->m_sDispDistance[nMinIndex].show_sation_sep)
		y1 = this->GetVPosition(nMinIndex, POS_FLAG::DOWN);
	else
		y1 = this->GetVPosition(nMinIndex, POS_FLAG::UP);

	y2 = this->GetVPosition(nMaxIndex, POS_FLAG::UP);

	block.station1 = this->m_sDispDistance[nMinIndex].station_number;
	block.station2 = this->m_sDispDistance[nMaxIndex].station_number;

	int x1 = this->GetOffsetMinuteFromGraphBegin(block.start_time)*m_fPixelPerMinute;
	int x2 = this->GetOffsetMinuteFromGraphBegin(block.end_time)*m_fPixelPerMinute;

	if(x1 < mGraphBeginPixel) x1 = mGraphBeginPixel;
	if(x2 > mGraphEndPixel)   x2 = mGraphEndPixel+10;

	if(x1<0) 
		x1 = 0;
	if(x2<0) 
		x2 = 0;

	if(x1 == 0 && x2 == 0)
		return ;

	int dlt = (m_ptgctrl->mConfigTg.m_AbsInhibitMarkCfg.dlt*(y2-y1))/100;
	block.m_drawRect.left	= x1;
	block.m_drawRect.right  = x2;
	block.m_drawRect.top	= y1+dlt;
	block.m_drawRect.bottom = y2-dlt;

	if(IsLineLinePosTop(block.line))
	{
		block.m_drawRect.OffsetRect(0, -dlt+2);
	}
	else if(IsLineLinePosBottom(block.line))
	{
		block.m_drawRect.OffsetRect(0, dlt-2);
	}
		
	y1 = block.m_drawRect.top;
	y2 = block.m_drawRect.bottom;

	int Ymiddle = block.m_drawRect.CenterPoint().y;
	int Xmiddle = block.m_drawRect.CenterPoint().x;
	int Ydlt    = abs(y2-y1)/8;

	COLORREF color = m_ptgctrl->mConfigTg.m_AbsInhibitMarkCfg.color1;
	if (m_ptgctrl->mConfigTg.m_context.IsCanModifyBlockStatus() && IsBlockComplete(block.adjust_status))
		color = m_ptgctrl->mConfigTg.m_AbsInhibitMarkCfg.off_color;

	CPen nPen, *pOldPen;
	nPen.CreatePen(PS_SOLID, 2, color);  
	pOldPen = pDC->SelectObject(&nPen);
	// 绘制竖线
	pDC->MoveTo(x1,y1);  pDC->LineTo(x1,y2);
	pDC->MoveTo(x2,y1);  pDC->LineTo(x2,y2);

	// 绘制中间横线
	CPoint startp,endp;
	startp.x = block.m_drawRect.left;
	endp.x = block.m_drawRect.right;
	startp.y = endp.y = block.m_drawRect.top + abs(block.m_drawRect.Height())/2;
	
	pDC->MoveTo(startp);  pDC->LineTo(endp);

	// 绘制方向横线
	if(this->m_ptgctrl->mConfigTg.m_AbsInhibitMarkCfg.draw == 1)
	{
		int nStyleCount = 2;
		DWORD StyleArray[] = {15,10};

		LOGBRUSH logBrush;
		logBrush.lbStyle = BS_SOLID;
		logBrush.lbColor = m_ptgctrl->mConfigTg.m_AbsInhibitMarkCfg.color2; 
		logBrush.lbHatch = 0;

		CPen nPen;
		nPen.CreatePen(PS_GEOMETRIC|PS_USERSTYLE, 1,&logBrush,nStyleCount,StyleArray);  

		pDC->SelectObject(&nPen);

		if(nDirect == 2) 
		{	
			pDC->MoveTo(startp.x, startp.y + 6);
			pDC->LineTo(endp.x, endp.y + 6);
		}
		else if(nDirect == 1)
		{	
			pDC->MoveTo(startp.x, startp.y - 6);
			pDC->LineTo(endp.x, endp.y - 6);
		}

		pDC->SelectObject(pOldPen);
		nPen.DeleteObject();
	}

	pDC->SelectObject(pOldPen);
	nPen.DeleteObject();

	block.cRect.left  = Xmiddle-5;
	block.cRect.right = Xmiddle+5;
	block.cRect.top		= endp.y-5;
	block.cRect.bottom  = endp.y+5;

	block.lRect.left  = x1-5;
	block.lRect.right = x1+5;
	block.lRect.top		= endp.y-5;
	block.lRect.bottom  = endp.y+5;

	block.rRect.left  = x2-5;
	block.rRect.right = x2+5;
	block.rRect.top		= endp.y-5;
	block.rRect.bottom  = endp.y+5;

	// 绘制焦点
	if(block.m_bFocus == TRUE)
	{
		CBrush  m_brushBackground;
		m_brushBackground.CreateSolidBrush(color);
		if(block.bMoveCenter == FALSE)
			pDC->FrameRect(block.cRect,&m_brushBackground); 
		else
			pDC->FillRect(block.cRect,&m_brushBackground);
		if(block.bMoveLeft == FALSE)
			pDC->FrameRect(block.lRect,&m_brushBackground); 
		else
			pDC->FillRect(block.lRect,&m_brushBackground);
		if(block.bMoveRight == FALSE)
			pDC->FrameRect(block.rRect,&m_brushBackground); 
		else
			pDC->FillRect(block.rRect,&m_brushBackground);
		m_brushBackground.DeleteObject();
	}

	CString strName;
	CString strDirection=GetABSStrDirectDesp(nDirect, block.ele_number[0], block.count);

	CString s_kilo,s_meter,e_kilo,e_meter;
	s_kilo.Format("%d",block.position);
	s_meter.Format("%d",block.s_meter);
	e_meter.Format("%d",block.e_meter);
	e_kilo.Format("%d",block.e_kilo);

	CString dcmdno, reason, head;
	this->ParseMarkReasonText(block.text, dcmdno, reason);
	if(!dcmdno.IsEmpty())
		head = "揭示命令号:" + dcmdno + " ";
	else
		head = "";

	CString strKm = block.GetKiloMDesp();
	if(strKm.IsEmpty())
	{
		strName = head + strDirection + reason;			
	}
	else
	{
		strName = head + strDirection + strKm + reason;
	}

	COLORREF oldTextColor = pDC->SetTextColor(m_ptgctrl->mConfigTg.m_AbsInhibitMarkCfg.color3);
	UINT oldAlign = pDC->SetTextAlign(TA_CENTER|TA_BOTTOM);
	CFont *pOldFont = pDC->SelectObject(&mBlockFont);
	CSize sz = pDC->GetTextExtent(strName);
	CRect rect(Xmiddle-sz.cx/2, y1-30, Xmiddle+sz.cx/2, y2+30);
	// 输出文本
	if(IsLineTextPosBottom(block.line))
	{
		pDC->TextOut (Xmiddle, block.m_drawRect.bottom, strName);
	}
	else if(IsLineTextPosLeft(block.line))
	{
		pDC->SetTextAlign(TA_LEFT|TA_TOP);
		pDC->TextOut (block.m_drawRect.left, block.m_drawRect.CenterPoint().y, strName);

		rect.left = block.m_drawRect.left;
		rect.right= block.m_drawRect.left+sz.cx;
	}
	else if(IsLineTextPosMiddle(block.line))
	{
		pDC->SetTextAlign(TA_CENTER|TA_TOP);
		pDC->TextOut (block.m_drawRect.CenterPoint().x, block.m_drawRect.CenterPoint().y-sz.cy/2, strName);
	}
	else if(IsLineTextPosRight(block.line))
	{
		pDC->SetTextAlign(TA_RIGHT|TA_TOP);
		pDC->TextOut (block.m_drawRect.right, block.m_drawRect.CenterPoint().y, strName);

		rect.left = block.m_drawRect.right-sz.cx;
		rect.right= block.m_drawRect.right;
	}
	else // IsLineTextPosTop
	{
		pDC->SetTextAlign(TA_CENTER|TA_TOP);
		pDC->TextOut (Xmiddle, block.m_drawRect.top, strName);
	}

	pDC->SetTextAlign(TA_CENTER|TA_BOTTOM);
	// 刷新矩形
	block.myPosition.UnionRect(&block.m_drawRect, &rect);
	if(block.myPosition.left>20)
		block.myPosition.left-=20;
	else
		block.myPosition.left=0;
	block.myPosition.right+=20;

	// 输出时间
	pDC->SetTextColor(m_ptgctrl->mConfigTg.m_AbsInhibitMarkCfg.color1);
	CString time_char_left;
	CString time_char_right;
	time_char_left.Format("%d", block.start_time/60 % 10);
	time_char_right.Format("%d", block.end_time/60 % 10);

	sz = pDC->GetTextExtent(time_char_left);
	if(this->m_ptgctrl->mConfigTg.m_AbsInhibitMarkCfg.time_pos == 0)
	{
		pDC->TextOut(x1, y1, time_char_left);
		pDC->TextOut(x1, y2+sz.cy, time_char_left);
		pDC->TextOut(x2, y1, time_char_right);
		pDC->TextOut(x2, y2+sz.cy, time_char_right);
	}
	else
	{
		pDC->TextOut(x1-sz.cx, Ymiddle+sz.cy/2, time_char_left);
		pDC->TextOut(x2+sz.cx, Ymiddle+sz.cy/2, time_char_right);
	}
	pDC->SetTextAlign(oldAlign);
	pDC->SetTextColor(oldTextColor);
	pDC->SelectObject(pOldFont);
}

void CTGDoc::DrawLanzhouAbsSlowMark(CTg_BLOCK& block, CDC *pDC, int nDirect)
{
	UINT nMinIndex = (block.station < block.index)?block.station:block.index;
	UINT nMaxIndex = (block.station > block.index)?block.station:block.index;

	if(nMinIndex<0||nMinIndex>=512 || nMaxIndex<0||nMaxIndex>=512)
	{
		sysprint(5,0, "站序1 %d  站序2 %d 越界(0<=index<512), 不显示该施工符号block %d, type %d ", block.index, block.station, block.mark_index, block.type);
		return ;
	}
	
	if(nMinIndex >=this->m_nDispDistanceNum)
	{
		sysprint(5,0, "站序1 %d  站序2 %d 不在有效显示区内, 不显示该施工符号block %d, type %d ", block.index, block.station, block.mark_index, block.type);
		return ;
	}

	if(nMaxIndex >=this->m_nDispDistanceNum)
	{
		nMaxIndex=this->m_nDispDistanceNum-1;
		if(nMinIndex>=nMaxIndex)
		{
			sysprint(5,0, "由于站序1 %d  站序2 %d 不在有效显示区内, 不显示该施工符号block %d, type %d ", block.index, block.station, block.mark_index, block.type);
			return ;
		}
	}

	int y1,y2;
	if(this->m_sDispDistance[nMinIndex].show_sation_sep)
		y1 = this->GetVPosition(nMinIndex, POS_FLAG::DOWN);
	else
		y1 = this->GetVPosition(nMinIndex, POS_FLAG::UP);

	y2 = this->GetVPosition(nMaxIndex, POS_FLAG::UP);

	block.station1 = this->m_sDispDistance[nMinIndex].station_number;
	block.station2 = this->m_sDispDistance[nMaxIndex].station_number;

	int x1 = this->GetOffsetMinuteFromGraphBegin(block.start_time)*m_fPixelPerMinute;
	int x2 = this->GetOffsetMinuteFromGraphBegin(block.end_time)*m_fPixelPerMinute;

	if(x1 < mGraphBeginPixel) x1 = mGraphBeginPixel;
	if(x2 > mGraphEndPixel)   x2 = mGraphEndPixel+10;

	if(x1<0) 
		x1 = 0;
	if(x2<0) 
		x2 = 0;

	if(x1 == 0 && x2 == 0)
		return ;

	int dlt = (m_ptgctrl->mConfigTg.m_AbsInhibitMarkCfg.dlt*(y2-y1))/100;
	block.m_drawRect.left	= x1;
	block.m_drawRect.right  = x2;
	block.m_drawRect.top	= y1+dlt;
	block.m_drawRect.bottom = y2-dlt;

	if(IsLineLinePosTop(block.line))
	{
		block.m_drawRect.OffsetRect(0, -dlt+2);
	}
	else if(IsLineLinePosBottom(block.line))
	{
		block.m_drawRect.OffsetRect(0, dlt-2);
	}
		
	y1 = block.m_drawRect.top;
	y2 = block.m_drawRect.bottom;

	int Ymiddle = block.m_drawRect.CenterPoint().y;
	int Xmiddle = block.m_drawRect.CenterPoint().x;
	int Ydlt    = abs(y2-y1)/8;

	COLORREF color = m_ptgctrl->mConfigTg.m_AbsSlowMarkCfg.color1;
	if (m_ptgctrl->mConfigTg.m_context.IsCanModifyBlockStatus() && IsBlockComplete(block.adjust_status))
		color = m_ptgctrl->mConfigTg.m_AbsSlowMarkCfg.off_color;

	CPen nPen, *pOldPen;
	nPen.CreatePen(PS_SOLID, 2, color);  
	pOldPen = pDC->SelectObject(&nPen);

	// 绘制竖线
	pDC->MoveTo(x1,y1);  pDC->LineTo(x1,y2);
	pDC->MoveTo(x2,y1);  pDC->LineTo(x2,y2);
	pDC->SelectObject(pOldPen);
	nPen.DeleteObject();

	// 绘制中间横线
	CPoint startp,endp;
	startp.x = block.m_drawRect.left;
	endp.x = block.m_drawRect.right;
	startp.y = endp.y = block.m_drawRect.top + abs(block.m_drawRect.Height())/2;
	

	int nStyleCount = 2;
	DWORD StyleArray[] = {15,10};

	LOGBRUSH logBrush;
	logBrush.lbStyle = BS_SOLID;
	logBrush.lbColor = color; 
	logBrush.lbHatch = 0;
	nPen.CreatePen(PS_GEOMETRIC|PS_USERSTYLE, 2,&logBrush,nStyleCount,StyleArray);  

	pOldPen = pDC->SelectObject(&nPen);

	pDC->MoveTo(startp);  pDC->LineTo(endp);

	pDC->SelectObject(pOldPen);
	nPen.DeleteObject();

	// 绘制方向横线
	if(this->m_ptgctrl->mConfigTg.m_AbsSlowMarkCfg.draw == 1)
	{
		int nStyleCount = 2;
		DWORD StyleArray[] = {10,10};

		LOGBRUSH logBrush;
		logBrush.lbStyle = BS_SOLID;
		logBrush.lbColor = m_ptgctrl->mConfigTg.m_AbsSlowMarkCfg.color2; 
		logBrush.lbHatch = 0;

		nPen.CreatePen(PS_GEOMETRIC|PS_USERSTYLE, 1,&logBrush,nStyleCount,StyleArray);  

		pOldPen = pDC->SelectObject(&nPen);

		if(nDirect == 2) 
		{	
			pDC->MoveTo(startp.x, startp.y + 6);
			pDC->LineTo(endp.x, endp.y + 6);
		}
		else if(nDirect == 1)
		{	
			pDC->MoveTo(startp.x, startp.y - 6);
			pDC->LineTo(endp.x, endp.y - 6);
		}

		pDC->SelectObject(pOldPen);
		nPen.DeleteObject();
	}

	block.cRect.left  = Xmiddle-5;
	block.cRect.right = Xmiddle+5;
	block.cRect.top		= endp.y-5;
	block.cRect.bottom  = endp.y+5;

	block.lRect.left  = x1-5;
	block.lRect.right = x1+5;
	block.lRect.top		= endp.y-5;
	block.lRect.bottom  = endp.y+5;

	block.rRect.left  = x2-5;
	block.rRect.right = x2+5;
	block.rRect.top		= endp.y-5;
	block.rRect.bottom  = endp.y+5;

	// 绘制焦点
	if(block.m_bFocus == TRUE)
	{
		CBrush  m_brushBackground;
		m_brushBackground.CreateSolidBrush(color);
		if(block.bMoveCenter == FALSE)
			pDC->FrameRect(block.cRect,&m_brushBackground); 
		else
			pDC->FillRect(block.cRect,&m_brushBackground);
		if(block.bMoveLeft == FALSE)
			pDC->FrameRect(block.lRect,&m_brushBackground); 
		else
			pDC->FillRect(block.lRect,&m_brushBackground);
		if(block.bMoveRight == FALSE)
			pDC->FrameRect(block.rRect,&m_brushBackground); 
		else
			pDC->FillRect(block.rRect,&m_brushBackground);
		m_brushBackground.DeleteObject();
	}

	CString strName;
	CString strDirection=GetABSStrDirectDesp(nDirect, block.ele_number[0], block.count);
	
	CString s_kilo,s_meter,e_kilo,e_meter,strDistance;
	if(block.slow_time > 0)
		strDistance.Format("限速 %d km/h 附加时分 %d分钟",  GetSlowSpeed(block.flag), block.slow_time/60);
	else
		strDistance.Format("限速 %d km/h ",  GetSlowSpeed(block.flag));
	s_kilo.Format("%d",block.position);
	s_meter.Format("%d",block.s_meter);
	e_meter.Format("%d",block.e_meter);
	e_kilo.Format("%d",block.e_kilo);

	CString dcmdno, reason, head;
	this->ParseMarkReasonText(block.text, dcmdno, reason);
	if(!dcmdno.IsEmpty())
		head = "揭示命令号:" + dcmdno + " ";
	else
		head = "";

	CString strKm = block.GetKiloMDesp();
	if(strKm.IsEmpty())
	{
		strName = head + strDirection +  reason + strDistance;		
	}
	else
	{
		strName = head + strDirection + strKm + reason + strDistance;
	}
	
	COLORREF oldTextColor = pDC->SetTextColor(m_ptgctrl->mConfigTg.m_AbsSlowMarkCfg.color3);
	UINT oldAlign = pDC->SetTextAlign(TA_CENTER|TA_BOTTOM);
	CFont *pOldFont = pDC->SelectObject(&mBlockFont);

	CSize sz = pDC->GetTextExtent(strName);
	CRect rect(Xmiddle-sz.cx/2, y1-30, Xmiddle+sz.cx/2, y2+30);
	// 输出文本
	if(IsLineTextPosBottom(block.line))
	{
		pDC->TextOut (Xmiddle, block.m_drawRect.bottom, strName);
	}
	else if(IsLineTextPosLeft(block.line))
	{
		pDC->SetTextAlign(TA_LEFT|TA_TOP);
		pDC->TextOut (block.m_drawRect.left, block.m_drawRect.CenterPoint().y, strName);

		rect.left = block.m_drawRect.left;
		rect.right= block.m_drawRect.left+sz.cx;
	}
	else if(IsLineTextPosMiddle(block.line))
	{
		pDC->SetTextAlign(TA_CENTER|TA_TOP);
		pDC->TextOut (block.m_drawRect.CenterPoint().x, block.m_drawRect.CenterPoint().y-sz.cy/2, strName);
	}
	else if(IsLineTextPosRight(block.line))
	{
		pDC->SetTextAlign(TA_RIGHT|TA_TOP);
		pDC->TextOut (block.m_drawRect.right, block.m_drawRect.CenterPoint().y, strName);

		rect.left = block.m_drawRect.right-sz.cx;
		rect.right= block.m_drawRect.right;
	}
	else // IsLineTextPosTop
	{
		pDC->SetTextAlign(TA_CENTER|TA_TOP);
		pDC->TextOut (Xmiddle, block.m_drawRect.top, strName);
	}

	pDC->SetTextAlign(TA_CENTER|TA_BOTTOM);

	// 刷新矩形
	block.myPosition.UnionRect(&block.m_drawRect, &rect);
	if(block.myPosition.left>20)
		block.myPosition.left-=20;
	else
		block.myPosition.left=0;
	block.myPosition.right+=20;

	// 输出时间
	pDC->SetTextColor(m_ptgctrl->mConfigTg.m_AbsSlowMarkCfg.color1);
	CString time_char_left;
	CString time_char_right;
	time_char_left.Format("%d", block.start_time/60 % 10);
	time_char_right.Format("%d", block.end_time/60 % 10);

	sz = pDC->GetTextExtent(time_char_left);

	if(this->m_ptgctrl->mConfigTg.m_AbsSlowMarkCfg.time_pos == 0)
	{
		pDC->TextOut(x1, y1, time_char_left);
		pDC->TextOut(x1, y2+sz.cy, time_char_left);
		pDC->TextOut(x2, y1, time_char_right);
		pDC->TextOut(x2, y2+sz.cy, time_char_right);
	}
	else
	{
		pDC->TextOut(x1-sz.cx, Ymiddle+sz.cy/2, time_char_left);
		pDC->TextOut(x2+sz.cx, Ymiddle+sz.cy/2, time_char_right);
	}
	pDC->SetTextAlign(oldAlign);
	pDC->SetTextColor(oldTextColor);
	pDC->SelectObject(pOldFont);
}

// G9
void CTGDoc::DrawAbsLanzhouEleFixMark(CTg_BLOCK& block, CDC *pDC, int nDirect)
{
	//区间号
	UINT nMinIndex = (block.station < block.index)?block.station:block.index;
	UINT nMaxIndex = (block.station > block.index)?block.station:block.index;

	if(nMinIndex<0||nMinIndex>=512 || nMaxIndex<0||nMaxIndex>=512)
	{
		sysprint(5,0, "站序1 %d  站序2 %d 越界(0<=index<512), 不显示该施工符号block %d, type %d ", block.index, block.station, block.mark_index, block.type);
		return ;
	}
	
	if(nMinIndex >=this->m_nDispDistanceNum)
	{
		sysprint(5,0, "站序1 %d  站序2 %d 不在有效显示区内, 不显示该施工符号block %d, type %d ", block.index, block.station, block.mark_index, block.type);
		return ;
	}

	if(nMaxIndex >=this->m_nDispDistanceNum)
	{
		nMaxIndex=this->m_nDispDistanceNum-1;
		if(nMinIndex>=nMaxIndex)
		{
			sysprint(5,0, "由于站序1 %d  站序2 %d 不在有效显示区内, 不显示该施工符号block %d, type %d ", block.index, block.station, block.mark_index, block.type);
			return ;
		}
	}

	int y1 = this->GetVPosition(nMinIndex, POS_FLAG::DOWN);
	int y2 = this->GetVPosition(nMaxIndex, POS_FLAG::UP);

	block.station1 = this->m_sDispDistance[nMinIndex].station_number;
	block.station2 = this->m_sDispDistance[nMaxIndex].station_number;

	int x1 = this->GetOffsetMinuteFromGraphBegin(block.start_time)*m_fPixelPerMinute;
	int x2 = this->GetOffsetMinuteFromGraphBegin(block.end_time)*m_fPixelPerMinute;

	if(x1 < mGraphBeginPixel) x1 = mGraphBeginPixel;
	if(x2 > mGraphEndPixel)   x2 = mGraphEndPixel+10;

	if(x1<0) 
		x1 = 0;
	if(x2<0) 
		x2 = 0;

	if(x1 == 0 && x2 == 0)
		return ;

	block.myPosition.SetRect(x1-10, y1-10, x2+10, y2+10);

	int dlt = m_ptgctrl->mConfigTg.m_AbsEleFixMarkCfg.dlt;

	//选择了封锁上方站
	if(block.slow_time&0x0001)
	{
		y1 = this->GetVPosition(nMinIndex, POS_FLAG::UP);
	}

	//选择了封锁下方站	
	if(block.slow_time&0x0002)
	{
		y2 = this->GetVPosition(nMaxIndex, POS_FLAG::DOWN);
	}

	if(block.slow_time&0x0001)
	{
		y1 -= 4;
	}
	else
	{
		if((block.slow_time&0x0008) && (nMinIndex < this->m_nDispDistanceNum-1))//上远
		{
			int TmpY = this->GetVPosition(nMinIndex+1, POS_FLAG::UP);
			y1=y1+(abs(TmpY-y1)*dlt)/100;//上远
		}
		else
		{
			y1 += 4;
		}
	}
	if(block.slow_time&0x0002)
	{
		y2 += 4;
	}
	else 
	{
		if((block.slow_time&0x0004) && (nMaxIndex > 0))//下远
		{
			int TmpY = this->GetVPosition(nMaxIndex-1, POS_FLAG::DOWN);
			y2=y2-(abs(y2-TmpY)*dlt)/100;//下远
		}
		else
		{
			y2 -= 4;
		}
	}

	block.m_drawRect.left	= x1;
	block.m_drawRect.right  = x2;
	block.m_drawRect.top	= y1;
	block.m_drawRect.bottom = y2;

	int Ymiddle = block.m_drawRect.CenterPoint().y;
	int Xmiddle = block.m_drawRect.CenterPoint().x;
	int Ydlt    = abs(y2-y1)/8;

	COLORREF color = m_ptgctrl->mConfigTg.m_AbsEleFixMarkCfg.color1;
	if (m_ptgctrl->mConfigTg.m_context.IsCanModifyBlockStatus() && IsBlockComplete(block.adjust_status))
		color = m_ptgctrl->mConfigTg.m_AbsEleFixMarkCfg.off_color;

	CPen nPen, *pOldPen;
	nPen.CreatePen(PS_SOLID, 2, color);  
	pOldPen = pDC->SelectObject(&nPen);

	// 绘制竖线
	pDC->MoveTo(x1,y1);  
	pDC->LineTo(x1,y2);
	pDC->LineTo(x2,y2);
	pDC->LineTo(x2,y1);
	pDC->LineTo(x1,y1);

	pDC->SelectObject(pOldPen);
	nPen.DeleteObject();

	block.cRect.left  = Xmiddle-6;
	block.cRect.right = Xmiddle+6;
	block.cRect.top		= Ymiddle-6;
	block.cRect.bottom  = Ymiddle+6;

	block.lRect.left  = x1-5;
	block.lRect.right = x1+5;
	block.lRect.top		= Ymiddle-5;
	block.lRect.bottom  = Ymiddle+5;

	block.rRect.left  = x2-5;
	block.rRect.right = x2+5;
	block.rRect.top		= Ymiddle-5;
	block.rRect.bottom  = Ymiddle+5;

	// 绘制焦点
	if(block.m_bFocus == TRUE)
	{
		CBrush  m_brushBackground;
		m_brushBackground.CreateSolidBrush(color);
		if(block.bMoveCenter == FALSE)
			pDC->FrameRect(block.cRect,&m_brushBackground); 
		else
			pDC->FillRect(block.cRect,&m_brushBackground);

		if(block.bMoveLeft == FALSE)
			pDC->FrameRect(block.lRect,&m_brushBackground); 
		else
			pDC->FillRect(block.lRect,&m_brushBackground);

		if(block.bMoveRight == FALSE)
			pDC->FrameRect(block.rRect,&m_brushBackground); 
		else
			pDC->FillRect(block.rRect,&m_brushBackground);

		m_brushBackground.DeleteObject();
	}

	CString strName;
	CString strDirection=GetABSStrDirectDesp(nDirect, block.ele_number[0], block.count);

	CString dcmdno, reason, head;
	this->ParseMarkReasonText(block.text, dcmdno, reason);
	if(!dcmdno.IsEmpty())
		head = "揭示命令号:" + dcmdno + " ";
	else
		head = "";

	CString strKm = block.GetKiloMDesp();
	if(strKm.IsEmpty())
	{
		strName = head + strDirection +  reason;		
	}
	else
	{
		strName = head + strDirection + strKm + reason;
	}

	COLORREF oldTextColor = pDC->SetTextColor(m_ptgctrl->mConfigTg.m_AbsEleFixMarkCfg.color3);
	UINT oldAlign = pDC->SetTextAlign(TA_CENTER|TA_BOTTOM);
	CFont *pOldFont = pDC->SelectObject(&mBlockFont);

	CSize sz = pDC->GetTextExtent(strName);
	CRect rect(Xmiddle-sz.cx/2, y1-30, Xmiddle+sz.cx/2, y2+30);
	// 输出文本
	if(IsLineTextPosBottom(block.line))
	{
		pDC->TextOut (Xmiddle, block.m_drawRect.bottom-block.m_drawRect.Height()/6, strName);
	}
	else if(IsLineTextPosLeft(block.line))
	{
		pDC->SetTextAlign(TA_LEFT|TA_TOP);
		pDC->TextOut (block.m_drawRect.left, block.m_drawRect.CenterPoint().y, strName);

		rect.left = block.m_drawRect.left;
		rect.right= block.m_drawRect.left+sz.cx;
	}
	else if(IsLineTextPosMiddle(block.line))
	{
		pDC->SetTextAlign(TA_CENTER|TA_TOP);
		pDC->TextOut (block.m_drawRect.CenterPoint().x, block.m_drawRect.CenterPoint().y-sz.cy/2, strName);
	}
	else if(IsLineTextPosRight(block.line))
	{
		pDC->SetTextAlign(TA_RIGHT|TA_TOP);
		pDC->TextOut (block.m_drawRect.right, block.m_drawRect.CenterPoint().y, strName);

		rect.left = block.m_drawRect.right-sz.cx;
		rect.right= block.m_drawRect.right;
	}
	else // IsLineTextPosTop
	{
		pDC->SetTextAlign(TA_CENTER|TA_TOP);
		pDC->TextOut (Xmiddle, block.m_drawRect.top+block.m_drawRect.Height()/6, strName);
	}
	pDC->SetTextAlign(TA_CENTER|TA_BOTTOM);

	// 输出文本
	CPoint startp,endp;

	// 刷新矩形
	block.myPosition.UnionRect(&block.m_drawRect, &rect);
	if(block.myPosition.left>20)
		block.myPosition.left-=20;
	else
		block.myPosition.left=0;
	block.myPosition.right+=20;
	
	// 输出时间
	pDC->SetTextColor(m_ptgctrl->mConfigTg.m_AbsEleFixMarkCfg.color1);
	CString time_char_left;
	CString time_char_right;
	time_char_left.Format("%d", block.start_time/60 % 10);
	time_char_right.Format("%d", block.end_time/60 % 10);

	sz = pDC->GetTextExtent(time_char_left);
	if(this->m_ptgctrl->mConfigTg.m_AbsEleFixMarkCfg.time_pos == 0)
	{
		pDC->TextOut(x1, y1, time_char_left);
		pDC->TextOut(x1, y2+sz.cy, time_char_left);
		pDC->TextOut(x2, y1, time_char_right);
		pDC->TextOut(x2, y2+sz.cy, time_char_right);
	}
	else
	{
		pDC->TextOut(x1-sz.cx, Ymiddle+sz.cy/2, time_char_left);
		pDC->TextOut(x2+sz.cx, Ymiddle+sz.cy/2, time_char_right);
	}
	pDC->SetTextAlign(oldAlign);
	pDC->SetTextColor(oldTextColor);
	pDC->SelectObject(pOldFont);
}

bool CTGDoc::IsShowStationMark(CTg_BLOCK& block, UINT nIndex, int& y1, int& y2)
{
	y1=-1;
	y2=-1;
	if(nIndex < 0 || nIndex >= this->m_nDispDistanceNum || nIndex>=512)
		return false;

	if(this->m_ptgctrl->mConfigTg.m_context.m_nStationBlockShowMode>0)
	{
		if(this->m_sDispDistance[nIndex].show_sation_sep<=0)
			return false;

		if(block.count > 0)
		{
			y1 = this->m_sDispDistance[nIndex].nSideYPos[block.ele_number[0]];
		}
		else
		{
			y1 = this->m_sDispDistance[nIndex].nStationYPos;
		}
		y1 += 2;
		y2 = y1;
	}
	else
	{
		if(this->m_sDispDistance[nIndex].show_sation_sep && block.count > 0)
		{
			y1 = this->m_sDispDistance[nIndex].nSideYPos[block.ele_number[0]];
			y1 += 2;

			y2 = y1;
		}
		else
		{
			y1 = this->m_sDispDistance[nIndex].nStationYPos;
			y2 = y1;
		}
	}

	return true;
}

// 绘制站内施工符号选中股道
int CTGDoc::GetSideUseByYPos(int yPos, int index)
{
	int nSideNo = -1;
	if(index>=0 && index<m_nDispDistanceNum)
	{
		int number_of_side = sizeof(m_sDispDistance[0].side)/sizeof(m_sDispDistance[0].side[0]);
	
		int dlt = 99;
		for(int i=0; i < number_of_side; i++)
		{
			int y1 = m_sDispDistance[index].nSideYPos[i];
			if(y1 <= 0)
				continue;
				
			if( abs(yPos - y1) < dlt)
			{
				dlt = abs(yPos - y1);
				nSideNo = i;
			}
		}
	}

	return nSideNo;
}

int CTGDoc::ConvertYPostoSideUseStationNo(int yPos, int& node_index)
{
	int nStationNo = -1;
	node_index = -1;
	for(int i = 0; i < m_nDispDistanceNum; i++)
	{
		int y1 = m_sDispDistance[i].nStationYPos - 6;
		int y2 = m_sDispDistance[i].nSecondStationYPos + 6;

		if( yPos > y1 && yPos < y2)
		{
			node_index = i;
			nStationNo = m_sDispDistance[i].station_number;
			break;
		}
	}

	return nStationNo;
}

