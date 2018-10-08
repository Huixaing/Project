#include "stdafx.h"
#include "math.h"
#include "TGForm.h"
#include "tgdoc.h"

/* ----------------------------------------------- */
//
//   该文件用来完成站内停车线的绘制
//
/* ----------------------------------------------- */
void CTGDoc::DrawStopLine(CDC*pCDC, CTG_TRAIN* single, TgTRAIN_RECORD* first, int nDrawMode)
{
	if(NULL == first || NULL == single)
		return;
	
	const COLORREF lColor=first->schd_color;
	const COLORREF lActualColor=first->real_color; 
	if(m_nTrainLeapLineFlag == 2)// 接入交出
	{
		DrawStopLine2(pCDC, single, first, lColor, lActualColor, nDrawMode);
	}
	else if(m_nTrainLeapLineFlag == 3) // 直连线
	{
		DrawStopLine3(pCDC, single, first, lColor, lActualColor, nDrawMode);
	}
	else // 无表示
	{
		DrawStopLine1(pCDC, single, first, lColor, lActualColor, nDrawMode);
	}

	return;
}

void CTGDoc::PressureStopLine(CDC * pCDC,TgTRAIN_RECORD* first, BYTE in_index, BYTE out_index, const CPoint& ptSide, const CPoint& ptDepSide)
{
	if(!IsPressureOutStation(first->stop_condition))
		return;

	CFont* pOldFont = pCDC->SelectObject(&fontPressure);
	UINT oldTextAlign = pCDC->SetTextAlign(TA_RIGHT|TA_TOP);
	COLORREF cl = pCDC->SetTextColor(RGB(0,0,0));

	if(in_index == out_index)
	{
		if(first->in_sta_direct == DOWN_DIRECT)
			oldTextAlign = pCDC->SetTextAlign(TA_RIGHT|TA_BOTTOM);
		else
			oldTextAlign = pCDC->SetTextAlign(TA_RIGHT|TA_TOP);
	}
	else
	{
		if(first->out_sta_direct == DOWN_DIRECT)
			oldTextAlign = pCDC->SetTextAlign(TA_RIGHT|TA_BOTTOM);
		else
			oldTextAlign = pCDC->SetTextAlign(TA_RIGHT|TA_TOP);
	}
	
	CString text;
	if(IsBeforePressure(first->stop_condition))
	{
		text="压前";
	}
	if(IsAfterPressure(first->stop_condition))
	{
		if(!text.IsEmpty())
		    text+="压后";
		else
			text="压后";
	}
	if(!text.IsEmpty())
	    pCDC->TextOut(ptDepSide.x, ptDepSide.y, text);

	pCDC->SetTextAlign(oldTextAlign);
	pCDC->SelectObject(pOldFont);
	pCDC->SetTextColor(cl);

	return;
}

void CTGDoc::GetRrcArlRect(CPoint pt, CRect& rect, int nFlagX, int nFlagY)
{
	double nRadius = 12.0;
	rect.left = pt.x - nRadius;
	rect.top  = pt.y - nRadius;

	rect.right   = pt.x + nRadius;
	rect.bottom  = pt.y + nRadius;

	rect.OffsetRect(nFlagX*18, 9*nFlagY);
	return;
}

void CTGDoc::GetRrcRect(CPoint pt, double dltX, double dltY, CRect& rect, int nFlag)
{
	double l = sqrt(dltX*dltX + dltY*dltY);

	double nRadius = 12.0;
	double H = nRadius/4;
	double L = (nRadius*l)/dltY + H*dltX/dltY;

	rect.left = pt.x + L - nRadius;
	rect.top  = pt.y + nFlag*H - nRadius;

	rect.right   = pt.x + L + nRadius;
	rect.bottom  = pt.y + nFlag*H + nRadius;

	if(abs(pt.x-rect.CenterPoint().x)>4*nRadius)
	{
		double H = -10;
		double L = (nRadius*l)/dltY + H*dltX/dltY;

		rect.left = pt.x + L - nRadius;
		rect.top  = pt.y + nFlag*H - nRadius;

		rect.right   = pt.x + L + nRadius;;
		rect.bottom  = pt.y + nFlag*H + nRadius;
	}
	return;
}

BYTE CTGDoc::GetLateTime(long plan_time, long real_time, CString& strDeltm)
{
	long delta=0, hour=0, minute=0;

	delta = real_time - plan_time;
	if(delta%60 > 0) 
		delta = delta - delta%60;
	else if(delta%60 < 0)
		delta = delta - delta%60 - 60;  // 2015.1.12 hjz

	delta = delta/60;
	hour = delta/60;
	minute = delta%60;

	CString temp;
	if(hour!=0)
		temp.Format("%d.%0.2d", hour, abs(minute));					
	else
		temp.Format("%d",minute);

	if(temp.Left(1) == "-")//早点
	{
		strDeltm=temp.Right(temp.GetLength()-1);
		return STATISTIC_FLAG::EARLY;				
	}
	else if(temp == "0")
	{
		strDeltm="";
		return STATISTIC_FLAG::RIGHT;
	}
	else
	{
		strDeltm=temp;
		return STATISTIC_FLAG::LATE;
	}
}

void CTGDoc::DrawStatStationArrival(CDC * pCDC,CTG_TRAIN *single, TgTRAIN_RECORD* first, CPoint point, int nDrawMode)
{
	if(single == NULL || first == NULL)  
		return;

	if((GetEarlyLateTime() != single->m_nTrainType) && GetEarlyLateTime() != 3)
		return;
	
	bool bInhibit=this->m_ptgctrl->mConfigTg.m_tgdef.IsInhibitTrainType(m_nCurrentSectionNo, single->myTrainInfo.type);
	// 禁止统计早晚点列车,不配置画黑圈
	if(bInhibit && !this->m_ptgctrl->mConfigTg.m_context.IsEnableInhibitDrawBlackArc())
		return;
	
	if(0==first->static_arrive)
		return;

	const TgTRAIN_MORE_INFO& info = single->myTrainInfo;
	if(info.type == 255) 
		return;

	int nWidth = 1; // 上海加粗，其它不需要
	if(this->m_ptgctrl->mConfigTg.m_context.IsHrbEarlyLateTimeBold())
		nWidth = 2;
	if(this->m_ptgctrl->mConfigTg.m_context.IsSelTrainBold())
	{
		if(m_pView->m_nFocusTrainIndex==single->GetTrainIndex() && (nDrawMode & DRAW_SCREEN) )
			nWidth = 2;
	}
	CPen nPen,nPen_red,nPen_Black; //晚点用篮圈,早点用红圈
	nPen.CreatePen ( PS_JOIN_ROUND, nWidth, CLR_BLUE);  
	nPen_red.CreatePen ( PS_JOIN_ROUND, nWidth, CLR_RED);
	nPen_Black.CreatePen ( PS_JOIN_ROUND, nWidth, CLR_BLACK);

	CFont* pOldFont   = pCDC->SelectObject(&fontEarlyLastTime);
	CPen*  nOldPen    = pCDC->SelectObject(&nPen);
	int  nOldColor    = pCDC->SetTextColor(CLR_BLUE);
	UINT nOldBackMode = pCDC->SetBkMode(TRANSPARENT);
	UINT nOldTextAlign= pCDC->SetTextAlign(TA_CENTER);

	int nDirectFlag = first->in_sta_direct;
	
	bool bNoEarlylatetime=false;
	std::map<ULONG, ULONG>::iterator it = m_noearlylatetime.find(single->GetTrainIndex());
	if(it != m_noearlylatetime.end())
	{
		bNoEarlylatetime=true;
	}

	// 统计到点
	if((first->static_arrive>0) && ((first->rec_index > 0) || IsFlagAccept(first->flag)))
	{
		CString strArrivalTime;
		BYTE nRetFlag = GetLateTime(first->plan_arrive, first->arrive, strArrivalTime);
		if(bNoEarlylatetime)
		{
			pCDC->SelectObject(&nPen_red);
			pCDC->SetTextColor(CLR_RED);
		}
		else if(bInhibit)
		{
			pCDC->SelectObject(&nPen_Black);
			pCDC->SetTextColor(CLR_BLACK);
		}
		else
		{
			if(STATISTIC_FLAG::LATE == nRetFlag)
			{
				pCDC->SelectObject(&nPen);
				pCDC->SetTextColor(CLR_BLUE);
			}
			else if(STATISTIC_FLAG::EARLY == nRetFlag)
			{
				pCDC->SelectObject(&nPen_red);
				pCDC->SetTextColor(CLR_RED);
			}
			else
			{
				pCDC->SelectObject(&nPen_red);
				pCDC->SetTextColor(CLR_RED);
			}

			// 哈尔滨调度员反映，按照技规列车晚点5分钟（含5分钟）以内，应显示红圈蓝字，5分钟以外显示为蓝圈蓝字
			if(this->m_ptgctrl->mConfigTg.m_context.IsHrbStaticEarlyLateTimeColor() && STATISTIC_FLAG::LATE == nRetFlag)
			{
				if(strArrivalTime.Find('.')<0)
				{
					int tm = atoi(strArrivalTime);
					if(tm<=5)
					{
						pCDC->SelectObject(&nPen_red);
						pCDC->SetTextColor(CLR_BLUE);
					}
				}
			}
		}

		CRect rect;

		CPoint ptPreDept(0,0);
		TgTRAIN_RECORD *pre = single->GetRecordByPosition(first->rec_index-1);
		if(NULL != pre)
		{
			if (IsSetTrainFlag(single->myTrainInfo.adjust_flag))
			{
				ptPreDept.x = m_fPixelPerMinute*GetOffsetMinuteFromGraphBegin(pre->depart+pre->stop_abs_e_time*60);

				if (first->in_station_index == pre->out_station_index)
				{
					if(nDirectFlag == 1)
					{
						if(pre->out_station_index>=0 && pre->out_station_index<m_nDispDistanceNum-1)
						{
							ptPreDept.y = this->m_sDispDistance[pre->out_station_index+1].nStationYPos;
						}
						else
						{
							ptPreDept.y = this->m_nPageHeight; // 20161009 下方越界无法获取,采用下边沿值
						}
					}
					else
					{
						if(pre->out_station_index>0 && pre->out_station_index<m_nDispDistanceNum)
						{
							ptPreDept.y = this->m_sDispDistance[pre->out_station_index-1].nStationYPos;
						}
						else
						{
							ptPreDept.y = 0;  // 20161009 上方越界无法获取,采用上边沿值
						}
					}
					if(nDirectFlag == 1)
						GetRrcRect(point,  point.x - ptPreDept.x, point.y - ptPreDept.y, rect, 1);
					else
						GetRrcRect(point,  point.x - ptPreDept.x, ptPreDept.y - point.y, rect, -1);
				}
				else
				{
					if(nDirectFlag == 1)
						ptPreDept.y = this->m_sDispDistance[pre->out_station_index].nSecondStationYPos;
					else
						ptPreDept.y = this->m_sDispDistance[pre->out_station_index].nStationYPos;

					if(nDirectFlag == 0)
						GetRrcRect(point,  point.x - ptPreDept.x, point.y - ptPreDept.y, rect, 1);
					else
						GetRrcRect(point,  point.x - ptPreDept.x, ptPreDept.y - point.y, rect, -1);
				}
			}
			else
			{
				ptPreDept.x = GetXPositionConsiderSecond(pre->depart); //1

				if(nDirectFlag == 1)
					ptPreDept.y = this->m_sDispDistance[pre->out_station_index].nSecondStationYPos;
				else
					ptPreDept.y = this->m_sDispDistance[pre->out_station_index].nStationYPos;

				if(nDirectFlag == 0)
					GetRrcRect(point,  point.x - ptPreDept.x, point.y - ptPreDept.y, rect, 1);
				else
					GetRrcRect(point,  point.x - ptPreDept.x, ptPreDept.y - point.y, rect, -1);
			}
		}
		else
		{
			int nRecNum = single->GetRecordNum();
			if(nRecNum == 1)
			{
				if(nDirectFlag == 0)
					GetRrcArlRect(point,  rect, -1, -1);
				else
					GetRrcArlRect(point,  rect, -1, 1);
			}
			else
			{
				if(nDirectFlag == 0)
					GetRrcArlRect(point,  rect, -1, 1);
				else
					GetRrcArlRect(point,  rect, -1, -1);
			}
		}

		CSize tx_size = pCDC->GetTextExtent(strArrivalTime);
		if(rect.Width()<tx_size.cx)
		{
			int dlt=(tx_size.cx-rect.Width())/2+1;
			rect.left -= dlt;
			rect.right += dlt;
			rect.top -= dlt;
			rect.bottom += dlt;
		}

		rect.OffsetRect(1,0);

		if(m_bIsVerifyTrainRect)
		{
		#ifdef DRAW_TRAIN_RECT	
			ShowRect(pCDC, rect);
		#endif
		single->UnionRect(rect);
		}

		pCDC->Arc(&rect, CPoint(rect.left, (rect.top + rect.bottom )/2), CPoint(rect.left, (rect.top + rect.bottom )/2));
		if(!bNoEarlylatetime&&!bInhibit)
		{
			pCDC->TextOut(rect.CenterPoint().x, rect.CenterPoint().y - tx_size.cy/2,strArrivalTime);
		}
	}

	pCDC->SelectObject(pOldFont);
	pCDC->SelectObject(nOldPen);
	pCDC->SetTextColor(nOldColor);
	pCDC->SetBkMode(nOldBackMode);
	pCDC->SetTextAlign(nOldTextAlign);
	nPen_red.DeleteObject();
	nPen.DeleteObject();
	nPen_Black.DeleteObject();
}

void CTGDoc::DrawStatStationDepart(CDC * pCDC,CTG_TRAIN *single, TgTRAIN_RECORD* first, CPoint point,int nDrawMode)
{
	if(single == NULL || first == NULL)  
		return;

	if((GetEarlyLateTime() != single->m_nTrainType) && GetEarlyLateTime() != 3)
		return;

	bool bInhibit=this->m_ptgctrl->mConfigTg.m_tgdef.IsInhibitTrainType(m_nCurrentSectionNo, single->myTrainInfo.type);
	// 禁止统计早晚点列车,不配置画黑圈
	if(bInhibit && !this->m_ptgctrl->mConfigTg.m_context.IsEnableInhibitDrawBlackArc())
		return;

	if(first->static_depart==0)
		return;

	const TgTRAIN_MORE_INFO& info = single->myTrainInfo;
	if(info.type == 255) 
		return;

	int nWidth = 1; // 上海加粗，其它不需要
	if(this->m_ptgctrl->mConfigTg.m_context.IsHrbEarlyLateTimeBold())
		nWidth = 2;
	if(this->m_ptgctrl->mConfigTg.m_context.IsSelTrainBold())
	{
		if(m_pView->m_nFocusTrainIndex==single->GetTrainIndex() && (nDrawMode & DRAW_SCREEN) )
			nWidth = 2;
	}

	CPen nPen,nPen_red,nPen_Black; //晚点用篮圈,早点用红圈
	nPen.CreatePen ( PS_JOIN_ROUND, nWidth, CLR_BLUE);  
	nPen_red.CreatePen ( PS_JOIN_ROUND, nWidth, CLR_RED);
	nPen_Black.CreatePen ( PS_JOIN_ROUND, nWidth, CLR_BLACK);

	CFont* pOldFont   = pCDC->SelectObject(&fontEarlyLastTime);
	CPen*  nOldPen    = pCDC->SelectObject(&nPen);
	int  nOldColor    = pCDC->SetTextColor(CLR_BLUE);
	UINT nOldBackMode = pCDC->SetBkMode(TRANSPARENT);
	UINT nOldTextAlign= pCDC->SetTextAlign(TA_CENTER);

	int nDirectFlag = first->out_sta_direct;

	// 发点之差
	bool bNoEarlylatetime  = false;
	std::map<ULONG, ULONG>::iterator it = m_noearlylatetime.find(single->GetTrainIndex());
	if(it != m_noearlylatetime.end())
	{
		bNoEarlylatetime = true; 
	}

	// 统计发点
	int nRecNum = single->GetRecordNum();
	if((first->static_depart>0) && ((nRecNum-1 > first->rec_index) || IsFlagGiveout(first->flag)))
	{
		CString strDepartTime;
		BYTE nRetFlag = GetLateTime(first->plan_depart, first->depart, strDepartTime);
		if(bNoEarlylatetime)
		{
			pCDC->SelectObject(&nPen_red);
			pCDC->SetTextColor(CLR_RED);
		}
		else if(bInhibit)
		{
			pCDC->SelectObject(&nPen_Black);
			pCDC->SetTextColor(CLR_BLACK);
		}
		else
		{
			if(STATISTIC_FLAG::LATE == nRetFlag)
			{
				pCDC->SelectObject(&nPen);
				pCDC->SetTextColor(CLR_BLUE);
			}
			else if(STATISTIC_FLAG::EARLY == nRetFlag)
			{
				pCDC->SelectObject(&nPen_red);
				pCDC->SetTextColor(CLR_RED);
			}
			else
			{
				pCDC->SelectObject(&nPen_red);
				pCDC->SetTextColor(CLR_RED);
			}

			// 哈尔滨调度员反映，按照技规列车晚点5分钟（含5分钟）以内，应显示红圈蓝字，5分钟以外显示为蓝圈蓝字
			if(this->m_ptgctrl->mConfigTg.m_context.IsHrbStaticEarlyLateTimeColor() && STATISTIC_FLAG::LATE == nRetFlag)
			{
				if(strDepartTime.Find('.')<0)
				{
					int tm = atoi(strDepartTime);
					if(tm<=5)
					{
						pCDC->SelectObject(&nPen_red);
						pCDC->SetTextColor(CLR_BLUE);
					}
				}
			}
		}

		CRect rect;

		CPoint ptNextArl(0,0);
		TgTRAIN_RECORD *next = single->GetRecordByPosition(first->rec_index+1);
		if(NULL != next)
		{
			if (IsSetTrainFlag(single->myTrainInfo.adjust_flag))
			{
				ptNextArl.x = m_fPixelPerMinute*GetOffsetMinuteFromGraphBegin(first->depart+first->stop_abs_s_time*60);			
				if (first->out_station_index == next->in_station_index)
				{
					if(nDirectFlag == 1)
					{
						if(next->in_station_index>=0 && next->in_station_index<m_nDispDistanceNum-1)
						{
							ptNextArl.y = this->m_sDispDistance[next->in_station_index+1].nStationYPos;
						}
						else
						{
							ptNextArl.y = this->m_nPageHeight; // 20161009 下方越界无法获取,采用下边沿值
						}
					}
					else
					{
						if(next->in_station_index>=0 && next->in_station_index<m_nDispDistanceNum-1)
						{
							ptNextArl.y = this->m_sDispDistance[next->in_station_index-1].nStationYPos;
						}
						else
						{
							ptNextArl.y = 0; // 20161009 上方越界无法获取,采用上边沿值
						}
					}
				}
				else
				{
					if(nDirectFlag == 1)
						ptNextArl.y = this->m_sDispDistance[next->in_station_index].nStationYPos;
					else
						ptNextArl.y = this->m_sDispDistance[next->in_station_index].nSecondStationYPos;
				}

				if(nDirectFlag == 0)
					GetRrcRect(point, ptNextArl.x - point.x, point.y - ptNextArl.y, rect, -1);
				else
					GetRrcRect(point, ptNextArl.x - point.x, ptNextArl.y - point.y, rect, 1);
			}
			else
			{
				ptNextArl.x = GetXPositionConsiderSecond(next->arrive); //1
				if(nDirectFlag == 1)
					ptNextArl.y = this->m_sDispDistance[next->in_station_index].nStationYPos;//nSecondStationYPos;
				else
					ptNextArl.y = this->m_sDispDistance[next->in_station_index].nSecondStationYPos;

				if(nDirectFlag == 0)
					GetRrcRect(point, ptNextArl.x - point.x, point.y - ptNextArl.y, rect, -1);
				else
					GetRrcRect(point, ptNextArl.x - point.x, ptNextArl.y - point.y, rect, 1);
			}
		}
		else
		{
			if(nRecNum == 1)
			{
				if(nDirectFlag == 0)
					GetRrcArlRect(point,  rect, 1, 1);
				else
					GetRrcArlRect(point,  rect, 1, -1);
			}
			else
			{
				if(nDirectFlag == 0)
					GetRrcArlRect(point,  rect, 1, -1);
				else
					GetRrcArlRect(point,  rect, 1, 1);
			}
		}

		CSize tx_size = pCDC->GetTextExtent(strDepartTime);
		if(rect.Width()<tx_size.cx)
		{
			int dlt=(tx_size.cx-rect.Width())/2+1;
			rect.left -= dlt;
			rect.right += dlt;
			rect.top -= dlt;
			rect.bottom += dlt;
		}

		rect.OffsetRect(1,0);

		if(m_bIsVerifyTrainRect)
		{
		#ifdef DRAW_TRAIN_RECT	
			ShowRect(pCDC, rect);
		#endif
		single->UnionRect(rect);
		}

		pCDC->Arc(&rect, CPoint(rect.left, (rect.top + rect.bottom )/2), CPoint(rect.left, (rect.top + rect.bottom )/2));

		if(!bNoEarlylatetime && !bInhibit)
		{
			pCDC->TextOut(rect.CenterPoint().x, rect.CenterPoint().y - tx_size.cy/2,strDepartTime);
		}
	}

	pCDC->SelectObject(pOldFont);
	pCDC->SelectObject(nOldPen);
	pCDC->SetTextColor(nOldColor);
	pCDC->SetBkMode(nOldBackMode);
	pCDC->SetTextAlign(nOldTextAlign);
	nPen_red.DeleteObject();
	nPen.DeleteObject();
	nPen_Black.DeleteObject();
}

void CTGDoc::DrawStopSidePoint(CDC*pCDC, const CPoint& pt_side, COLORREF lColor)
{
	if(!this->m_ptgctrl->mConfigTg.m_context.IsDrawStopSidePoint())
		return;

	CSize ssSize = CSize(6,6);

	CBrush tempBrush(lColor);
	CBrush *pOldBrush;
	pOldBrush = pCDC->SelectObject(&tempBrush);

	pCDC->RoundRect(CRect(CPoint(pt_side.x - 3, pt_side.y - 3),ssSize), pt_side);

	pCDC->SelectObject(pOldBrush);
	tempBrush.DeleteObject();

	return;
}

void CTGDoc::DrawStopLine3LeapLine(CDC*pCDC, CTG_TRAIN* single, TgTRAIN_RECORD* first, const int lColor, int lActualColor, int nDrawMode, int in_index, int out_index)
{
	CPoint ptlStart, ptlEnd, ptlSide, ptlDepSide;

	WORD nRecIndex = first->rec_index;

	// 默认跳跃车站,停车股道绘制在入口车站
	bool bDrawStopLineAtInStation=true; 
		
	// 只有在入口车站股道不展开, 出口车站展开的情况下, 停车股道绘制在出口车站
	if(this->m_sDispDistance[in_index].show_sation_sep==0 && this->m_sDispDistance[out_index].show_sation_sep>0)
		bDrawStopLineAtInStation=false;
	
	CPen penActual, penSched, *pOldPen;
	penSched.CreatePen(PS_SOLID, first->schd_width, lColor);
	penActual.CreatePen(PS_SOLID, first->real_width, lActualColor);
	pOldPen=pCDC->SelectObject(&penActual);

	CPoint ptLineBegin, ptLineEnd;

	// 停车股道绘制在入口车站
	if(bDrawStopLineAtInStation)
	{
		// 第一个车站表示
		int nSubSection = this->GetSubSection(in_index);

		ptlStart.x = m_nFirstArlMinuteOffset*m_fPixelPerMinute+(m_fPixelPerMinute*m_nFirstArlSecond)/60; // 计算位置考虑秒,以下相同
		ptlSide.x  = ptlStart.x;
		ptlEnd.x     = m_nFirstDepMinuteOffset*m_fPixelPerMinute+(m_fPixelPerMinute*m_nFirstDepSecond)/60;	
		ptlDepSide.x = ptlEnd.x;

		if(first->in_sta_direct == UP_DIRECT)
			ptlStart.y = this->GetVPosition(in_index, POS_FLAG::DOWN);
		else
			ptlStart.y = this->GetVPosition(in_index, POS_FLAG::UP);

		if(in_index < out_index)
			ptlEnd.y  =  this->GetVPosition(in_index, POS_FLAG::DOWN);
		else
			ptlEnd.y  =  this->GetVPosition(in_index, POS_FLAG::UP);

		ptlSide.y = GetStationSideVPos(first, in_index, first->arrive_side);
		if(first->depart_side!=first->arrive_side)
			ptlDepSide.y = GetStationSideVPos(first, in_index, first->depart_side);
		else
			ptlDepSide.y = ptlSide.y;
		ptlEnd.Offset (x_point_offset,y_point_offset);
		ptlStart.Offset (x_point_offset,y_point_offset);
		ptlSide.Offset (x_point_offset,y_point_offset);
		ptlDepSide.Offset (x_point_offset,y_point_offset);

		// 第一个车站
		{
			// 通过车站
			if(ptlEnd.x == ptlStart.x)
			{	
				if(m_sDispDistance[in_index].show_sation_sep > 0)
				{
					if(first_a_time_status)
						pCDC->SelectObject(&penActual);
					else
						pCDC->SelectObject(&penSched);

					if(!IsFlagStart(first->flag))
					{
						single->AddPerPointInfo(nRecIndex, UNION_POSITION, ptlStart, nSubSection);
						pCDC->MoveTo(ptlStart);  	
						pCDC->LineTo(ptlSide); 
					}

					if(0==m_sDispDistance[in_index].show_sides_mode && first->arrive_side > 0)
					{
						DrawStopSidePoint(pCDC, ptlSide, lColor);
					}
					single->AddPerPointInfo(nRecIndex, UNION_POSITION, ptlSide, nSubSection);

					if(first_d_time_status!=first_a_time_status)
					{
						if(first_d_time_status)
							pCDC->SelectObject(&penActual);
						else
							pCDC->SelectObject(&penSched);
					}

					if(!IsFlagTerminal(first->flag))
					{
						single->AddPerPointInfo(nRecIndex, UNION_POSITION, ptlEnd, nSubSection);
						pCDC->MoveTo(ptlSide);  	
						pCDC->LineTo(ptlEnd); 
					}
				}
				else
				{
					single->AddPerPointInfo(nRecIndex, UNION_POSITION, ptlStart, nSubSection);
				}
				
				DrawArrivalTime(pCDC, first, ptlStart,  lColor, lActualColor, nDrawMode);
			}
			else  // 到开车
			{
				if(m_sDispDistance[in_index].show_sation_sep > 0)
				{
					//绘制停车线
					if(first_a_time_status)
						pCDC->SelectObject(&penActual);
					else
						pCDC->SelectObject(&penSched);
					// 绘制 入口股道 到 停车股道 竖连线
					pCDC->MoveTo(ptlStart);  
					pCDC->LineTo(ptlSide);

					if(first_d_time_status != first_a_time_status)
					{
						if(first_d_time_status)
							pCDC->SelectObject(&penActual);
						else
							pCDC->SelectObject(&penSched);
					}
					// 绘制 股道到点 出口股道 竖连线
					pCDC->MoveTo(ptlDepSide);  
					pCDC->LineTo(ptlEnd);

					// 设置选中点
					single->AddPerPointInfo(nRecIndex, ARRIVAL_POSITION, ptlStart, nSubSection);

					// 设置选中点
					single->AddPerPointInfo(nRecIndex, ARRIVAL_POSITION, ptlSide, nSubSection);
					// 设置选中点
					single->AddPerPointInfo(nRecIndex, DEPART_POSITION, ptlDepSide, nSubSection);

					// 设置选中点
					single->AddPerPointInfo(nRecIndex, DEPART_POSITION, ptlEnd, nSubSection);
				}
				else
				{
					// 设置选中点
					single->AddPerPointInfo(nRecIndex, ARRIVAL_POSITION, ptlStart, nSubSection);

					// 设置选中点
					single->AddPerPointInfo(nRecIndex, DEPART_POSITION, ptlEnd, nSubSection);
					if(first_d_time_status)
						pCDC->SelectObject(&penActual);
					else
						pCDC->SelectObject(&penSched);
				}

				if(IsPressureOutStation(first->stop_condition))
					PressureStopLine(pCDC, first, in_index, out_index, ptlSide, ptlDepSide);
				if(m_bStopStationWaveLine && JudgeNeedWaveLine(first,single->m_nDirectFlag))
				{
					if(first->in_sta_direct == DOWN_DIRECT)
						DrawChordLine(pCDC,ptlSide,ptlDepSide,DOWN_DIRECT);
					else
						DrawChordLine(pCDC,ptlSide,ptlDepSide,UP_DIRECT);
				}
				else
				{	
					if(this->GetTgType() == TG_TYPE::BASIC_TG)
						DrawBasicStationStopLine(pCDC,ptlSide,ptlDepSide);
					else
					{
						pCDC->MoveTo(ptlSide);
						pCDC->LineTo(ptlDepSide);
					}
				}
				DrawArrivalTime(pCDC, first, ptlStart,  lColor, lActualColor, nDrawMode);
				DrawDepartTime(pCDC, first, ptlEnd,  lColor, lActualColor, nDrawMode);
			}
			DrawStatStationArrival(pCDC, single, first, ptlStart, nDrawMode);
			DrawStatStationDepart2(pCDC, single, first, ptlEnd, nDrawMode);
		}
		
		ptLineBegin = ptlEnd;

		// 第二个车站表示
		nSubSection = this->GetSubSection(out_index);

		ptlStart.x = m_nFirstArlMinuteOffset*m_fPixelPerMinute+(m_fPixelPerMinute*m_nFirstArlSecond)/60;
		ptlSide.x  = ptlStart.x;
		ptlEnd.x     = m_nFirstDepMinuteOffset*m_fPixelPerMinute+(m_fPixelPerMinute*m_nFirstDepSecond)/60;	
		ptlDepSide.x = ptlEnd.x;

		if(in_index < out_index)
			ptlStart.y = this->GetVPosition(out_index, POS_FLAG::UP);
		else
			ptlStart.y = this->GetVPosition(out_index, POS_FLAG::DOWN);

		if(first->out_sta_direct == UP_DIRECT)
			ptlEnd.y  =  this->GetVPosition(out_index, POS_FLAG::UP);
		else
			ptlEnd.y = this->GetVPosition(out_index, POS_FLAG::DOWN);

		ptlSide.y = GetStationSideVPos(first, out_index, first->arrive_side);
		if(first->depart_side!=first->arrive_side)
			ptlDepSide.y = GetStationSideVPos(first, out_index, first->depart_side);
		else
			ptlDepSide.y = ptlSide.y;
		ptlEnd.Offset (x_point_offset,y_point_offset);
		ptlStart.Offset (x_point_offset,y_point_offset);
		ptlSide.Offset (x_point_offset,y_point_offset);
		ptlDepSide.Offset (x_point_offset,y_point_offset);

		// 第二个车站
		{
			if(in_index < out_index)
			{
				if(first->out_sta_direct == UP_DIRECT)
					ptlSide.y = ptlEnd.y;
			}
			else
			{
				if(first->out_sta_direct == DOWN_DIRECT)
					ptlSide.y = ptlEnd.y;
			}

			ptlStart.x = ptlEnd.x;
			ptlSide.x  = ptlEnd.x;
			if(m_sDispDistance[out_index].show_sation_sep > 0)
			{
				if(first_a_time_status)
					pCDC->SelectObject(&penActual);
				else
					pCDC->SelectObject(&penSched);

				if(!IsFlagStart(first->flag))
				{
					single->AddPerPointInfo(nRecIndex, UNION_POSITION, ptlStart, nSubSection);
					pCDC->MoveTo(ptlStart);  	
					pCDC->LineTo(ptlSide); 
				}

				if(0==m_sDispDistance[out_index].show_sides_mode && first->arrive_side > 0)
				{
					DrawStopSidePoint(pCDC, ptlSide, lColor);
				}
				single->AddPerPointInfo(nRecIndex, UNION_POSITION, ptlSide, nSubSection);
				
				if(first_d_time_status!=first_a_time_status)
				{
					if(first_d_time_status)
						pCDC->SelectObject(&penActual);
					else
						pCDC->SelectObject(&penSched);
				}

				if(!IsFlagTerminal(first->flag))
				{
					single->AddPerPointInfo(nRecIndex, UNION_POSITION, ptlEnd, nSubSection);
					pCDC->MoveTo(ptlDepSide);  	
					pCDC->LineTo(ptlEnd); 
				}
			}
			else
			{
				single->AddPerPointInfo(nRecIndex, UNION_POSITION, ptlStart, nSubSection);
			}

			DrawDepartTime(pCDC, first, ptlEnd,  lColor, lActualColor, nDrawMode);
			DrawStatStationDepart(pCDC, single, first, ptlEnd, nDrawMode);
		}
		
		ptLineEnd = ptlStart;

		// 车站连线直线
		if(first_d_time_status)
			pCDC->SelectObject(&penActual);
		else
			pCDC->SelectObject(&penSched);
		pCDC->MoveTo(ptLineBegin);  	
		pCDC->LineTo(ptLineEnd); 
	}
	// 停车股道绘制在出口车站
	else
	{
		// 第一个车站表示---绘制通过
		int nSubSection = this->GetSubSection(in_index);

		ptlStart.x = m_nFirstArlMinuteOffset*m_fPixelPerMinute+(m_fPixelPerMinute*m_nFirstArlSecond)/60;
		ptlSide.x  = ptlStart.x;
		ptlEnd.x     = m_nFirstDepMinuteOffset*m_fPixelPerMinute+(m_fPixelPerMinute*m_nFirstDepSecond)/60;	
		ptlDepSide.x = ptlEnd.x;

		if(first->in_sta_direct == UP_DIRECT)
			ptlStart.y = this->GetVPosition(in_index, POS_FLAG::DOWN);
		else
			ptlStart.y = this->GetVPosition(in_index, POS_FLAG::UP);

		if(in_index < out_index)
			ptlEnd.y  =  this->GetVPosition(in_index, POS_FLAG::DOWN);
		else
			ptlEnd.y  =  this->GetVPosition(in_index, POS_FLAG::UP);

		ptlSide.y = GetStationSideVPos(first, in_index, first->arrive_side);
		ptlDepSide.y = GetStationSideVPos(first, in_index, first->depart_side);

		ptlEnd.Offset (x_point_offset,y_point_offset);
		ptlStart.Offset (x_point_offset,y_point_offset);
		ptlSide.Offset (x_point_offset,y_point_offset);
		ptlDepSide.Offset (x_point_offset,y_point_offset);

		// 第一个车站
		{
			if(in_index < out_index)
			{
				if(first->in_sta_direct == UP_DIRECT)
					ptlSide.y = ptlEnd.y;
			}
			else
			{
				if(first->in_sta_direct == DOWN_DIRECT)
					ptlSide.y = ptlEnd.y;
			}

			ptlEnd.x = ptlStart.x;
			ptlDepSide.x = ptlStart.x;

			if(m_sDispDistance[in_index].show_sation_sep > 0)
			{
				if(first_a_time_status)
					pCDC->SelectObject(&penActual);
				else
					pCDC->SelectObject(&penSched);

				if(!IsFlagStart(first->flag))
				{
					single->AddPerPointInfo(nRecIndex, UNION_POSITION, ptlStart, nSubSection);
					pCDC->MoveTo(ptlStart);  	
					pCDC->LineTo(ptlSide); 
				}

				if(0==m_sDispDistance[in_index].show_sides_mode && first->arrive_side > 0)
				{
					DrawStopSidePoint(pCDC, ptlSide, lColor);
				}
				single->AddPerPointInfo(nRecIndex, UNION_POSITION, ptlSide, nSubSection);
				
				if(first_d_time_status!=first_a_time_status)
				{
					if(first_d_time_status)
						pCDC->SelectObject(&penActual);
					else
						pCDC->SelectObject(&penSched);
				}

				if(!IsFlagTerminal(first->flag))
				{
					single->AddPerPointInfo(nRecIndex, UNION_POSITION, ptlEnd, nSubSection);
					pCDC->MoveTo(ptlSide);  	
					pCDC->LineTo(ptlEnd); 
				}
			}
			else
			{
				single->AddPerPointInfo(nRecIndex, UNION_POSITION, ptlStart, nSubSection);
			}

			DrawArrivalTime(pCDC, first, ptlEnd,  lColor, lActualColor, nDrawMode);
			DrawStatStationArrival(pCDC, single, first, ptlEnd, nDrawMode);
		}

		ptLineBegin = ptlEnd;

		// 第二个车站表示
		nSubSection = this->GetSubSection(out_index);

		ptlStart.x = m_nFirstArlMinuteOffset*m_fPixelPerMinute+(m_fPixelPerMinute*m_nFirstArlSecond)/60;
		ptlSide.x  = ptlStart.x;
		ptlEnd.x     = m_nFirstDepMinuteOffset*m_fPixelPerMinute+(m_fPixelPerMinute*m_nFirstDepSecond)/60;	
		ptlDepSide.x = ptlEnd.x;

		if(in_index < out_index)
			ptlStart.y = this->GetVPosition(out_index, POS_FLAG::UP);
		else
			ptlStart.y = this->GetVPosition(out_index, POS_FLAG::DOWN);

		if(first->out_sta_direct == UP_DIRECT)
			ptlEnd.y  =  this->GetVPosition(out_index, POS_FLAG::UP);
		else
			ptlEnd.y = this->GetVPosition(out_index, POS_FLAG::DOWN);

		ptlSide.y = GetStationSideVPos(first, out_index, first->arrive_side);
		ptlDepSide.y = GetStationSideVPos(first, out_index, first->depart_side);

		ptlEnd.Offset (x_point_offset,y_point_offset);
		ptlStart.Offset (x_point_offset,y_point_offset);
		ptlSide.Offset (x_point_offset,y_point_offset);
		ptlDepSide.Offset (x_point_offset,y_point_offset);
		// 第二个车站
		{
			// 通过车站
			if(ptlEnd.x == ptlStart.x)
			{	
				if(m_sDispDistance[out_index].show_sation_sep > 0)
				{
					if(first_a_time_status)
						pCDC->SelectObject(&penActual);
					else
						pCDC->SelectObject(&penSched);
					if(!IsFlagStart(first->flag))
					{
						single->AddPerPointInfo(nRecIndex, UNION_POSITION, ptlStart, nSubSection);
						pCDC->MoveTo(ptlStart);  	
						pCDC->LineTo(ptlSide); 
					}

					if(0==m_sDispDistance[out_index].show_sides_mode && first->arrive_side > 0)
					{
						DrawStopSidePoint(pCDC, ptlSide, lColor);
					}
					single->AddPerPointInfo(nRecIndex, UNION_POSITION, ptlSide, nSubSection);

					if(first_d_time_status!=first_a_time_status)
					{
						if(first_d_time_status)
							pCDC->SelectObject(&penActual);
						else
							pCDC->SelectObject(&penSched);
					}

					if(!IsFlagTerminal(first->flag))
					{
						single->AddPerPointInfo(nRecIndex, UNION_POSITION, ptlEnd, nSubSection);
						pCDC->MoveTo(ptlSide);  	
						pCDC->LineTo(ptlEnd); 
					}
				}
				else
				{
					single->AddPerPointInfo(nRecIndex, UNION_POSITION, ptlStart, nSubSection);
				}

				DrawArrivalTime(pCDC, first, ptlStart,  lColor, lActualColor, nDrawMode);
			}
			else  // 到开车
			{
				if(m_sDispDistance[out_index].show_sation_sep > 0)
				{
					if(first_a_time_status)
						pCDC->SelectObject(&penActual);
					else
						pCDC->SelectObject(&penSched);
					// 绘制 入口股道 到 停车股道 竖连线
					pCDC->MoveTo(ptlStart);  
					pCDC->LineTo(ptlSide);

					if(first_d_time_status != first_a_time_status)
					{
						if(first_d_time_status)
							pCDC->SelectObject(&penActual);
						else
							pCDC->SelectObject(&penSched);
					}
					// 绘制 股道到点 出口股道 竖连线
					pCDC->MoveTo(ptlDepSide);  
					pCDC->LineTo(ptlEnd);

					// 设置选中点
					single->AddPerPointInfo(nRecIndex, ARRIVAL_POSITION, ptlStart, nSubSection);

					// 设置选中点
					single->AddPerPointInfo(nRecIndex, ARRIVAL_POSITION, ptlSide, nSubSection);
					// 设置选中点
					single->AddPerPointInfo(nRecIndex, DEPART_POSITION, ptlDepSide, nSubSection);

					// 设置选中点
					single->AddPerPointInfo(nRecIndex, DEPART_POSITION, ptlEnd, nSubSection);
				}
				else
				{
					// 设置选中点
					single->AddPerPointInfo(nRecIndex, ARRIVAL_POSITION, ptlStart, nSubSection);

					// 设置选中点
					single->AddPerPointInfo(nRecIndex, DEPART_POSITION, ptlEnd, nSubSection);
					if(first_d_time_status)
						pCDC->SelectObject(&penActual);
					else
						pCDC->SelectObject(&penSched);
				}

				if(IsPressureOutStation(first->stop_condition))
					PressureStopLine(pCDC, first, in_index, out_index, ptlSide, ptlDepSide);

				if(m_bStopStationWaveLine && JudgeNeedWaveLine(first,single->m_nDirectFlag))
				{
					if(first->out_sta_direct == DOWN_DIRECT)
						DrawChordLine(pCDC,ptlSide,ptlDepSide,DOWN_DIRECT);
					else
						DrawChordLine(pCDC,ptlSide,ptlDepSide,UP_DIRECT);
				}
				else
				{	
					if(this->GetTgType() == TG_TYPE::BASIC_TG)
						DrawBasicStationStopLine(pCDC,ptlSide,ptlDepSide);
					else
					{
						pCDC->MoveTo(ptlSide);
						pCDC->LineTo(ptlDepSide);
					}
				}
				DrawArrivalTime(pCDC, first, ptlStart,  lColor, lActualColor, nDrawMode);
				DrawDepartTime(pCDC, first, ptlEnd,  lColor, lActualColor, nDrawMode);
			}
			DrawStatStationArrival(pCDC, single, first, ptlStart, nDrawMode);
			DrawStatStationDepart(pCDC, single, first, ptlEnd, nDrawMode);
		}
		
		ptLineEnd = ptlStart;

		// 车站连线直线
		if(first_a_time_status)
			pCDC->SelectObject(&penActual);
		else
			pCDC->SelectObject(&penSched);
		pCDC->MoveTo(ptLineBegin);  	
		pCDC->LineTo(ptLineEnd); 
	}

	pCDC->SelectObject(pOldPen);
	penSched.DeleteObject();
	penActual.DeleteObject();

	return;
}

void CTGDoc::DrawStopLine3(CDC*pCDC, CTG_TRAIN* single, TgTRAIN_RECORD* first, COLORREF lColor, COLORREF lActualColor, int nDrawMode)
{
	if(NULL == single ||  NULL == first)
		return;

	CPoint ptlStart, ptlEnd, ptlSide, ptlDepSide;
	WORD nRecIndex = first->rec_index;

	int in_index = -1, out_index = -1;
	if( 0 <= first->in_station_index && first->in_station_index < m_nDispDistanceNum)
		in_index = first->in_station_index;

	if(0 <= first->out_station_index && first->out_station_index < m_nDispDistanceNum)
		out_index = first->out_station_index;

	if(in_index == -1)
		in_index = out_index;
	if(out_index == -1)
		out_index = in_index;

	if(in_index == -1 || out_index == -1)
	{
		sysprint(0,0,"列车: %s 在车站: %d 的 in out index 均为-1,无法绘制站内停车", this->GetTrainShowId(single), first->station);
		return;
	}

	// 不是跳跃车站
	if(in_index == out_index)
	{
		ptlStart.x = m_nFirstArlMinuteOffset*m_fPixelPerMinute+(m_fPixelPerMinute*m_nFirstArlSecond)/60;
		ptlSide.x  = m_nFirstArlMinuteOffset*m_fPixelPerMinute+(m_fPixelPerMinute*m_nFirstArlSecond)/60;
		ptlEnd.x     = m_nFirstDepMinuteOffset*m_fPixelPerMinute+(m_fPixelPerMinute*m_nFirstDepSecond)/60;	
		ptlDepSide.x = m_nFirstDepMinuteOffset*m_fPixelPerMinute+(m_fPixelPerMinute*m_nFirstDepSecond)/60;	

		if(first->in_sta_direct == DOWN_DIRECT)
			ptlStart.y = this->GetVPosition(out_index, POS_FLAG::UP);
		else
			ptlStart.y = this->GetVPosition(out_index, POS_FLAG::DOWN);

		if(first->out_sta_direct == UP_DIRECT)
			ptlEnd.y  =  this->GetVPosition(out_index, POS_FLAG::UP);
		else
			ptlEnd.y  =  this->GetVPosition(out_index, POS_FLAG::DOWN);

		ptlSide.y = GetStationSideVPos(first, out_index, first->arrive_side);
		if(first->depart_side==first->arrive_side)
			ptlDepSide.y = ptlSide.y;
		else
			ptlDepSide.y = GetStationSideVPos(first, out_index, first->depart_side);

		ptlEnd.Offset (x_point_offset,y_point_offset);
		ptlStart.Offset (x_point_offset,y_point_offset);
		ptlSide.Offset (x_point_offset,y_point_offset);
		ptlDepSide.Offset (x_point_offset,y_point_offset);

		// 通过车站
		int nSubSection = this->GetSubSection(out_index);
		if(ptlEnd.x == ptlStart.x)
		{	
			if(m_sDispDistance[out_index].show_sation_sep > 0)
			{
				CPen penActual, penSched, *pOldPen;
				penSched.CreatePen(PS_SOLID, first->schd_width, lColor);
				penActual.CreatePen(PS_SOLID, first->real_width, lActualColor);

				if(first_a_time_status)
					pOldPen=pCDC->SelectObject(&penActual);
				else
					pOldPen=pCDC->SelectObject(&penSched);

				if(!IsFlagStart(first->flag))
				{
					single->AddPerPointInfo(nRecIndex, UNION_POSITION, ptlStart, nSubSection);
					pCDC->MoveTo(ptlStart);  	
					pCDC->LineTo(ptlSide); 
				}

				if(0==m_sDispDistance[out_index].show_sides_mode && first->arrive_side > 0)
				{
					DrawStopSidePoint(pCDC, ptlSide, first->schd_color);
				}
				single->AddPerPointInfo(nRecIndex, UNION_POSITION, ptlSide, nSubSection);

				if(first_d_time_status != first_a_time_status)
				{
					if(first_d_time_status)
						pCDC->SelectObject(&penActual);
					else
						pCDC->SelectObject(&penSched);
				}

				if(!IsFlagTerminal(first->flag))
				{
					single->AddPerPointInfo(nRecIndex, UNION_POSITION, ptlEnd, nSubSection);
					pCDC->MoveTo(ptlSide);  	
					pCDC->LineTo(ptlEnd); 
				}
				pCDC->SelectObject(pOldPen);
				penSched.DeleteObject();
				penActual.DeleteObject();
			}
			else
			{
				single->AddPerPointInfo(nRecIndex, UNION_POSITION, ptlStart, nSubSection);
			}

			if(IsFlagTerminal(first->flag))
				DrawArrivalTime(pCDC, first, ptlStart,  lColor, lActualColor, nDrawMode);
			else
				DrawArrivalTime(pCDC, first, ptlEnd,  lColor, lActualColor, nDrawMode);
		}
		else  // 到开车
		{
			CPen penActual, penSched, *pOldPen=NULL;
			penSched.CreatePen(PS_SOLID, first->schd_width, lColor);
			penActual.CreatePen(PS_SOLID, first->real_width, lActualColor);

			if(m_sDispDistance[in_index].show_sation_sep > 0)
			{
				// 绘制 入口股道 到 停车股道 竖连线
				if(first_a_time_status)
					pOldPen=pCDC->SelectObject(&penActual);
				else
					pOldPen=pCDC->SelectObject(&penSched);
				// 绘制 入口股道 到 停车股道 竖连线
				pCDC->MoveTo(ptlStart);  
				pCDC->LineTo(ptlSide);

				// 绘制 股道到点 出口股道 竖连线
				if(first_d_time_status != first_a_time_status)
				{
					if(first_d_time_status)
						pCDC->SelectObject(&penActual);
					else
						pCDC->SelectObject(&penSched);
				}
				pCDC->MoveTo(ptlDepSide);  
				pCDC->LineTo(ptlEnd);

				// 设置选中点
				single->AddPerPointInfo(nRecIndex, ARRIVAL_POSITION, ptlStart, nSubSection);

				// 设置选中点
				single->AddPerPointInfo(nRecIndex, ARRIVAL_POSITION, ptlSide, nSubSection);
				// 设置选中点
				single->AddPerPointInfo(nRecIndex, DEPART_POSITION, ptlDepSide, nSubSection);

				// 设置选中点
				single->AddPerPointInfo(nRecIndex, DEPART_POSITION, ptlEnd, nSubSection);
			}
			else
			{
				// 设置选中点
				single->AddPerPointInfo(nRecIndex, ARRIVAL_POSITION, ptlStart, nSubSection);

				// 设置选中点
				single->AddPerPointInfo(nRecIndex, DEPART_POSITION, ptlEnd, nSubSection);
				// 绘制 停车站平线
				if(first_d_time_status)
					pOldPen=pCDC->SelectObject(&penActual);
				else
					pOldPen=pCDC->SelectObject(&penSched);
			}

			if(IsPressureOutStation(first->stop_condition))
				PressureStopLine(pCDC, first, in_index, out_index, ptlSide, ptlDepSide);
			
			if(m_bStopStationWaveLine && JudgeNeedWaveLine(first,single->m_nDirectFlag))
			{
				if(first->in_sta_direct == DOWN_DIRECT)
					DrawChordLine(pCDC,ptlSide,ptlDepSide,DOWN_DIRECT);
				else
					DrawChordLine(pCDC,ptlSide,ptlDepSide,UP_DIRECT);
			}
			else
			{	
				if(this->GetTgType() == TG_TYPE::BASIC_TG)
					DrawBasicStationStopLine(pCDC,ptlSide,ptlDepSide);
				else
				{
					pCDC->MoveTo(ptlSide);  
					pCDC->LineTo(ptlDepSide);
				}
			}
			if(pOldPen)
				pCDC->SelectObject(pOldPen);
			penSched.DeleteObject();
			penActual.DeleteObject();

			DrawArrivalTime(pCDC, first, ptlStart,  lColor, lActualColor, nDrawMode);
			DrawDepartTime(pCDC, first, ptlEnd,  lColor, lActualColor, nDrawMode, 1);
		}

		DrawStatStationArrival(pCDC, single, first, ptlStart, nDrawMode);
		DrawStatStationDepart(pCDC, single, first, ptlEnd, nDrawMode);
	}
	// 跳跃车站
	else 
	{
		DrawStopLine3LeapLine(pCDC, single, first, lColor, lActualColor, nDrawMode, in_index, out_index);
	}

	return;
}

void CTGDoc::DrawStopLine2(CDC*pCDC, CTG_TRAIN* single, TgTRAIN_RECORD* first,COLORREF lColor, COLORREF lActualColor, int nDrawMode)
{
	if(single == NULL || first == NULL)
		return;

	CPoint ptlStart, ptlEnd, ptlSide, ptlDepSide;
	WORD nRecIndex = first->rec_index;

	int in_index = -1, out_index = -1;
	if( 0 <= first->in_station_index && first->in_station_index < m_nDispDistanceNum)
		in_index = first->in_station_index;

	if(0 <= first->out_station_index && first->out_station_index < m_nDispDistanceNum)
		out_index = first->out_station_index;

	if(in_index == -1)
		in_index = out_index;
	if(out_index == -1)
		out_index = in_index;

	if(in_index == -1)
	{
		sysprint(0,0,"列车: %s 在车站: %d 的 in out index 均为-1,无法绘制站内停车", this->GetTrainShowId(single), first->station);
		return;
	}

	// 不是跳跃车站
	if(in_index == out_index)
	{
		ptlStart.x = m_nFirstArlMinuteOffset*m_fPixelPerMinute+(m_fPixelPerMinute*m_nFirstArlSecond)/60;
		ptlSide.x  = ptlStart.x;
		ptlEnd.x     = m_nFirstDepMinuteOffset*m_fPixelPerMinute+(m_fPixelPerMinute*m_nFirstDepSecond)/60;	
		ptlDepSide.x = ptlEnd.x;

		ptlSide.y = GetStationSideVPos(first, in_index, first->arrive_side);
		if(first->depart_side==first->arrive_side)
			ptlDepSide.y = ptlSide.y;
		else
			ptlDepSide.y = GetStationSideVPos(first, in_index, first->depart_side);

		if(IsFlagStart(first->flag))
		{
			if(first->out_sta_direct == UP_DIRECT)
			{
				ptlStart.y = this->GetVPosition(in_index, POS_FLAG::DOWN);
				ptlEnd.y  =  this->GetVPosition(in_index, POS_FLAG::UP);
			}
			else
			{
				ptlStart.y = this->GetVPosition(in_index, POS_FLAG::UP);
				ptlEnd.y  =  this->GetVPosition(in_index, POS_FLAG::DOWN);;
			}
		}
		else
		{
			if(first->in_sta_direct == UP_DIRECT)
				ptlStart.y = this->GetVPosition(in_index, POS_FLAG::DOWN);
			else
				ptlStart.y = this->GetVPosition(in_index, POS_FLAG::UP);

			if(first->out_sta_direct == UP_DIRECT)
				ptlEnd.y   = this->GetVPosition(in_index, POS_FLAG::UP);
			else
				ptlEnd.y   = this->GetVPosition(in_index, POS_FLAG::DOWN);
		}

		ptlEnd.Offset (x_point_offset,y_point_offset);
		ptlStart.Offset (x_point_offset,y_point_offset);
		ptlSide.Offset (x_point_offset,y_point_offset);
		ptlDepSide.Offset (x_point_offset,y_point_offset);

		int nSubSection = this->GetSubSection(in_index);
	
		// 通过
		if(ptlEnd.x == ptlStart.x)
		{
			if(m_sDispDistance[in_index].show_sation_sep > 0)
			{
				CPen penActual, penSched, *pOldPen;
				penSched.CreatePen(PS_SOLID, first->schd_width, first->schd_color);
				penActual.CreatePen(PS_SOLID, first->real_width, first->real_color);

				if(first_a_time_status)
					pOldPen=pCDC->SelectObject(&penActual);
				else
					pOldPen=pCDC->SelectObject(&penSched);

				if(!IsFlagStart(first->flag))
				{
					single->AddPerPointInfo(nRecIndex, UNION_POSITION, ptlStart, nSubSection);
					pCDC->MoveTo(ptlStart);  	
					pCDC->LineTo(ptlSide); 
				}

				if(0==m_sDispDistance[in_index].show_sides_mode && first->arrive_side > 0)
				{
					DrawStopSidePoint(pCDC, ptlSide, lColor);
				}
				single->AddPerPointInfo(nRecIndex, UNION_POSITION, ptlSide, nSubSection);

				if(first_d_time_status!=first_a_time_status)
				{
					if(first_d_time_status)
						pCDC->SelectObject(&penActual);
					else
						pCDC->SelectObject(&penSched);
				}
				if(!IsFlagTerminal(first->flag))
				{
					single->AddPerPointInfo(nRecIndex, UNION_POSITION, ptlEnd, nSubSection);
					pCDC->MoveTo(ptlSide);  	
					pCDC->LineTo(ptlEnd); 
				}

				pCDC->SelectObject(pOldPen);
				penSched.DeleteObject();
				penActual.DeleteObject();
			}
			else
			{
				single->AddPerPointInfo(nRecIndex, UNION_POSITION, ptlStart, nSubSection);
			}
			if(IsFlagTerminal(first->flag))
				DrawArrivalTime(pCDC, first, ptlStart,  lColor, lActualColor, nDrawMode);
			else
				DrawArrivalTime(pCDC, first, ptlEnd,  lColor, lActualColor, nDrawMode);
		}
		// 到开
		else
		{
			CPen penActual, penSched, *pOldPen=NULL;
			penSched.CreatePen(PS_SOLID, first->schd_width, first->schd_color);
			penActual.CreatePen(PS_SOLID, first->real_width, first->real_color);
			// 绘制 股道发点 股道到点 横线
			if(m_sDispDistance[in_index].show_sation_sep > 0)
			{
				if(first_a_time_status)
					pOldPen=pCDC->SelectObject(&penActual);
				else
					pOldPen=pCDC->SelectObject(&penSched);
				// 绘制 入口股道 到 停车股道 竖连线
				pCDC->MoveTo(ptlStart);  
				pCDC->LineTo(ptlSide);

				if(first_d_time_status!=first_a_time_status)
				{
					if(first_d_time_status)
						pCDC->SelectObject(&penActual);
					else
						pCDC->SelectObject(&penSched);
				}
				// 绘制 股道到点 出口股道 竖连线
				pCDC->MoveTo(ptlDepSide);  
				pCDC->LineTo(ptlEnd);

				// 设置选中点
				single->AddPerPointInfo(nRecIndex, ARRIVAL_POSITION, ptlStart, nSubSection);

				// 设置选中点
				single->AddPerPointInfo(nRecIndex, ARRIVAL_POSITION, ptlSide, nSubSection);
				// 设置选中点
				single->AddPerPointInfo(nRecIndex, DEPART_POSITION, ptlDepSide, nSubSection);

				// 设置选中点
				single->AddPerPointInfo(nRecIndex, DEPART_POSITION, ptlEnd, nSubSection);
			}
			else
			{
				// 设置选中点
				single->AddPerPointInfo(nRecIndex, ARRIVAL_POSITION, ptlStart, nSubSection);

				// 设置选中点
				single->AddPerPointInfo(nRecIndex, DEPART_POSITION, ptlEnd, nSubSection);
			
				// 画停车线
				if(first_d_time_status)
					pOldPen=pCDC->SelectObject(&penActual);
				else
					pOldPen=pCDC->SelectObject(&penSched);
			}

			if(IsPressureOutStation(first->stop_condition))
				PressureStopLine(pCDC, first, in_index, out_index, ptlSide, ptlDepSide);

			if(m_bStopStationWaveLine && JudgeNeedWaveLine(first,single->m_nDirectFlag))
			{
				if(first->in_sta_direct == DOWN_DIRECT)
					DrawChordLine(pCDC,ptlSide,ptlDepSide,DOWN_DIRECT);
				else
					DrawChordLine(pCDC,ptlSide,ptlDepSide,UP_DIRECT);
			}
			else
			{	
				if(this->GetTgType() == TG_TYPE::BASIC_TG)
					DrawBasicStationStopLine(pCDC,ptlSide,ptlDepSide);
				else
				{
					pCDC->MoveTo(ptlSide);
					pCDC->LineTo(ptlDepSide);
				}
			}
			if(pOldPen)
				pCDC->SelectObject(pOldPen);
			penSched.DeleteObject();
			penActual.DeleteObject();

			DrawArrivalTime(pCDC, first, ptlStart,  lColor, lActualColor, nDrawMode);
			DrawDepartTime(pCDC, first, ptlEnd,  lColor, lActualColor, nDrawMode,1);
		}

		DrawStatStationArrival(pCDC, single, first, ptlStart, nDrawMode);
		DrawStatStationDepart(pCDC, single, first, ptlEnd, nDrawMode);
	}
	// 跳跃车站
	else
	{
		bool bDrawLine=false;
		// 同一分段内相同车站间画连线
	    if(this->m_sDispDistance[in_index].sub_section == this->m_sDispDistance[out_index].sub_section)
			bDrawLine = this->m_ptgctrl->mConfigTg.m_context.IsEnableLeapLineInSameSubTable();
		if(bDrawLine)
		{
			DrawStopLine3LeapLine(pCDC, single, first, lColor, lActualColor, nDrawMode, in_index, out_index);
		}
		// end同一分段内相同车站间画连线
		// 同一分段内相同车站间画接入交出
		else 
		{
			// 第一个站停车表示
			ptlStart.x = m_nFirstArlMinuteOffset*m_fPixelPerMinute+(m_fPixelPerMinute*m_nFirstArlSecond)/60;
			ptlSide.x  = ptlStart.x;
			ptlEnd.x     = m_nFirstDepMinuteOffset*m_fPixelPerMinute+(m_fPixelPerMinute*m_nFirstDepSecond)/60;	
			ptlDepSide.x = ptlEnd.x;

			ptlSide.y = GetStationSideVPos(first, in_index, first->arrive_side);
			if(first->depart_side==first->arrive_side)
				ptlDepSide.y = ptlSide.y;
			else
				ptlDepSide.y = GetStationSideVPos(first, in_index, first->depart_side);

			if(IsFlagStart(first->flag))
			{
				if(first->out_sta_direct == UP_DIRECT)
				{
					ptlStart.y = this->GetVPosition(in_index, POS_FLAG::DOWN);
					ptlEnd.y  =  this->GetVPosition(in_index, POS_FLAG::UP);
				}
				else
				{
					ptlStart.y = this->GetVPosition(in_index, POS_FLAG::UP);
					ptlEnd.y  =  this->GetVPosition(in_index, POS_FLAG::DOWN);;
				}
			}
			else
			{
				if(first->in_sta_direct == UP_DIRECT)
				{
					ptlStart.y = this->GetVPosition(in_index, POS_FLAG::DOWN);
					ptlEnd.y   = this->GetVPosition(in_index, POS_FLAG::UP);
				}
				else
				{
					ptlStart.y = this->GetVPosition(in_index, POS_FLAG::UP);
					ptlEnd.y   = this->GetVPosition(in_index, POS_FLAG::DOWN);
				}
			}

			ptlEnd.Offset (x_point_offset,y_point_offset);
			ptlStart.Offset (x_point_offset,y_point_offset);
			ptlSide.Offset (x_point_offset,y_point_offset);
			ptlDepSide.Offset (x_point_offset,y_point_offset);

			int nSubSection = this->GetSubSection(in_index);
			// 通过
			if(ptlEnd.x == ptlStart.x)
			{
				if(m_sDispDistance[in_index].show_sation_sep > 0)
				{
					CPen penActual, penSched, *pOldPen;
					penSched.CreatePen(PS_SOLID, first->schd_width, first->schd_color);
					penActual.CreatePen(PS_SOLID, first->real_width, first->real_color);

					if(first_a_time_status)
						pOldPen=pCDC->SelectObject(&penActual);
					else
						pOldPen=pCDC->SelectObject(&penSched);

					if(!IsFlagStart(first->flag))
					{
						single->AddPerPointInfo(nRecIndex, UNION_POSITION, ptlStart, nSubSection);
						pCDC->MoveTo(ptlStart);  	
						pCDC->LineTo(ptlSide); 
					}

					if(0==m_sDispDistance[in_index].show_sides_mode && first->arrive_side > 0)
					{
						DrawStopSidePoint(pCDC, ptlSide, lColor);
					}
					single->AddPerPointInfo(nRecIndex, UNION_POSITION, ptlSide, nSubSection);
					
					if(first_d_time_status!=first_a_time_status)
					{
						if(first_d_time_status)
							pCDC->SelectObject(&penActual);
						else
							pCDC->SelectObject(&penSched);
					}

					if(!IsFlagTerminal(first->flag))
					{
						single->AddPerPointInfo(nRecIndex, UNION_POSITION, ptlEnd,  nSubSection);
						pCDC->MoveTo(ptlSide);  	
						pCDC->LineTo(ptlEnd); 
					}

					pCDC->SelectObject(pOldPen);
					penSched.DeleteObject();
					penActual.DeleteObject();
				}
				else
				{
					single->AddPerPointInfo(nRecIndex, UNION_POSITION, ptlEnd,  nSubSection); 
				}
				DrawArrivalTime(pCDC, first, ptlStart,  lColor, lActualColor, nDrawMode);
			}
			// 到开
			else
			{
				CPen penActual, penSched, *pOldPen=NULL;
				penSched.CreatePen(PS_SOLID, first->schd_width, first->schd_color);
				penActual.CreatePen(PS_SOLID, first->real_width, first->real_color);

				// 绘制 股道发点 股道到点 横线
				if(m_sDispDistance[in_index].show_sation_sep > 0)
				{
					if(first_a_time_status)
						pOldPen=pCDC->SelectObject(&penActual);
					else
						pOldPen=pCDC->SelectObject(&penSched);
					// 绘制 入口股道 到 停车股道 竖连线
					pCDC->MoveTo(ptlStart);  
					pCDC->LineTo(ptlSide);

					if(first_d_time_status!=first_a_time_status)
					{
						if(first_d_time_status)
							pCDC->SelectObject(&penActual);
						else
							pCDC->SelectObject(&penSched);
					}
					// 绘制 股道到点 出口股道 竖连线
					pCDC->MoveTo(ptlDepSide);  
					pCDC->LineTo(ptlEnd);

					// 设置选中点
					single->AddPerPointInfo(nRecIndex, ARRIVAL_POSITION, ptlStart, nSubSection);

					// 设置选中点
					single->AddPerPointInfo(nRecIndex, ARRIVAL_POSITION, ptlSide, nSubSection);
					// 设置选中点
					single->AddPerPointInfo(nRecIndex, DEPART_POSITION, ptlDepSide, nSubSection);

					// 设置选中点
					single->AddPerPointInfo(nRecIndex, DEPART_POSITION, ptlEnd, nSubSection);
				}
				else
				{
					// 设置选中点
					single->AddPerPointInfo(nRecIndex, ARRIVAL_POSITION, ptlStart, nSubSection);

					// 设置选中点
					single->AddPerPointInfo(nRecIndex, DEPART_POSITION, ptlEnd, nSubSection);
				
					if(first_d_time_status)
						pOldPen=pCDC->SelectObject(&penActual);
					else
						pOldPen=pCDC->SelectObject(&penSched);
				}

				if(IsPressureOutStation(first->stop_condition))
					PressureStopLine(pCDC, first, in_index, out_index, ptlSide, ptlDepSide);

				if(m_bStopStationWaveLine && JudgeNeedWaveLine(first,single->m_nDirectFlag))
				{
					if(first->in_sta_direct == DOWN_DIRECT)
						DrawChordLine(pCDC,ptlSide,ptlDepSide,DOWN_DIRECT);
					else
						DrawChordLine(pCDC,ptlSide,ptlDepSide,UP_DIRECT);
				}
				else
				{	
					if(this->GetTgType() == TG_TYPE::BASIC_TG)
						DrawBasicStationStopLine(pCDC,ptlSide,ptlDepSide);
					else
					{
						pCDC->MoveTo(ptlSide);
						pCDC->LineTo(ptlDepSide);
					}
				}

				if(pOldPen)
					pCDC->SelectObject(pOldPen);
				penSched.DeleteObject();
				penActual.DeleteObject();
				
				DrawArrivalTime(pCDC, first, ptlStart,  lColor, lActualColor, nDrawMode);
				DrawDepartTime(pCDC, first, ptlEnd,  lColor, lActualColor, nDrawMode);
			}
			// 绘制交出符号
			int direct_flag = DOWN_MARK;
			if(first->in_sta_direct == UP_DIRECT)
				direct_flag = UP_MARK;
			else
				direct_flag = DOWN_MARK;

			if(first_d_time_status) 
				DrawMarkOfDepart(pCDC, single, ptlEnd, direct_flag, lActualColor, first->real_width,first);             
			else
				DrawMarkOfDepart(pCDC, single, ptlEnd, direct_flag, lColor, first->schd_width,first);	

			DrawStatStationArrival(pCDC, single, first, ptlStart, nDrawMode);


			// 第二个站停车表示
			CPoint pt_secondA, pt_secondD;
			CPoint pt_sideA,   pt_sideD;

			pt_secondA.x = m_nFirstArlMinuteOffset*m_fPixelPerMinute+(m_fPixelPerMinute*m_nFirstArlSecond)/60;
			pt_secondD.x = m_nFirstDepMinuteOffset*m_fPixelPerMinute+(m_fPixelPerMinute*m_nFirstDepSecond)/60;	
			pt_sideA.x = pt_secondA.x;
			pt_sideD.x = pt_secondD.x;
			pt_sideA.y = GetStationSideVPos(first, out_index, first->arrive_side);
			if(first->depart_side==first->arrive_side)
				pt_sideD.y = pt_sideA.y;
			else
				pt_sideD.y = GetStationSideVPos(first, out_index, first->depart_side);

			if(UP_DIRECT == first->out_sta_direct)
			{
				pt_secondA.y = this->GetVPosition(out_index, POS_FLAG::DOWN);
				pt_secondD.y = this->GetVPosition(out_index, POS_FLAG::UP);
			}
			else
			{
				pt_secondA.y = this->GetVPosition(out_index, POS_FLAG::UP);
				pt_secondD.y = this->GetVPosition(out_index, POS_FLAG::DOWN);
			}

			pt_secondA.Offset(x_point_offset, y_point_offset);
			pt_secondD.Offset(x_point_offset, y_point_offset);
			pt_sideA.Offset(x_point_offset, y_point_offset);
			pt_sideD.Offset(x_point_offset, y_point_offset);

			//绘制接入标志
			if(first->out_sta_direct == UP_DIRECT)
				direct_flag = DOWN_MARK;
			else
				direct_flag = UP_MARK;

			CString strText = first->arrive_train_id;
			if(!this->m_ptgctrl->mConfigTg.m_context.IsShowMutiTrainSeparate())
				strText = GetTrainShowId(first->station, *first, single, NULL);
			if(first_a_time_status)
				DrawMarkOfArrive(pCDC,single,strText,pt_secondA, direct_flag, lActualColor, first->real_width,first);
			else
				DrawMarkOfArrive(pCDC,single,strText,pt_secondA, direct_flag, lColor, first->schd_width,first);

			nSubSection = this->GetSubSection(out_index);
			if(pt_secondA.x != pt_secondD.x)
			{
				CPen penActual, penSched, *pOldPen=NULL;
				penSched.CreatePen(PS_SOLID, first->schd_width, first->schd_color);
				penActual.CreatePen(PS_SOLID, first->real_width, first->real_color);

				if(m_sDispDistance[out_index].show_sation_sep > 0)
				{
					if(first_a_time_status)
						pOldPen=pCDC->SelectObject(&penActual);
					else
						pOldPen=pCDC->SelectObject(&penSched);
					pCDC->MoveTo(pt_secondA);  
					pCDC->LineTo(pt_sideA);

					if(first_d_time_status!=first_a_time_status)
					{
						if(first_d_time_status)
							pCDC->SelectObject(&penActual);
						else
							pCDC->SelectObject(&penSched);
					}
					pCDC->MoveTo(pt_secondD);  
					pCDC->LineTo(pt_sideD);

					single->AddPerPointInfo(nRecIndex, ARRIVAL_POSITION, pt_secondA, nSubSection);
					single->AddPerPointInfo(nRecIndex, DEPART_POSITION, pt_secondD,  nSubSection);
					single->AddPerPointInfo(nRecIndex, ARRIVAL_POSITION, pt_sideA,  nSubSection);
					single->AddPerPointInfo(nRecIndex, DEPART_POSITION, pt_sideD, nSubSection);
				}
				else
				{
					single->AddPerPointInfo(nRecIndex, ARRIVAL_POSITION, pt_secondA, nSubSection);
					single->AddPerPointInfo(nRecIndex, DEPART_POSITION, pt_secondD,  nSubSection);

					if(first_d_time_status)
						pOldPen=pCDC->SelectObject(&penActual);
					else
						pOldPen=pCDC->SelectObject(&penSched);
				}
				if(IsPressureOutStation(first->stop_condition))
					PressureStopLine(pCDC, first, in_index, out_index, pt_sideA, pt_sideD);

				if(m_bStopStationWaveLine && JudgeNeedWaveLine(first, single->m_nDirectFlag))
				{
					if(first->out_sta_direct == DOWN_DIRECT)
						DrawChordLine(pCDC, pt_sideA, pt_sideD, DOWN_DIRECT);
					else
						DrawChordLine(pCDC, pt_sideA, pt_sideD, UP_DIRECT);
				}
				else
				{	
					if(this->GetTgType() == TG_TYPE::BASIC_TG)
						DrawBasicStationStopLine(pCDC, pt_sideA, pt_sideD);
					else
					{
						pCDC->MoveTo(pt_sideA);
						pCDC->LineTo(pt_sideD);   
					}
				}

				if(pOldPen)
					pCDC->SelectObject(pOldPen);
				penSched.DeleteObject();
				penActual.DeleteObject();
				
				DrawArrivalTime(pCDC, first, pt_secondA,  lColor, lActualColor, nDrawMode,1);
				DrawDepartTime(pCDC, first, pt_secondD,  lColor, lActualColor, nDrawMode,1);
			}
			else
			{
				// 通过竖线
				if(m_sDispDistance[out_index].show_sation_sep > 0)
				{
					single->AddPerPointInfo(nRecIndex, UNION_POSITION, pt_secondA,  nSubSection);

					CPen penActual, penSched, *pOldPen;
					penSched.CreatePen(PS_SOLID, first->schd_width, first->schd_color);
					penActual.CreatePen(PS_SOLID, first->real_width, first->real_color);

					if(first_a_time_status)
						pOldPen=pCDC->SelectObject(&penActual);
					else
						pOldPen=pCDC->SelectObject(&penSched);

					pCDC->MoveTo(pt_secondA);	
					pCDC->LineTo(pt_sideA);

					if(m_sDispDistance[out_index].show_sides_mode == 0 && first->arrive_side > 0)
					{
						DrawStopSidePoint(pCDC, pt_sideD, lColor);
					}
					single->AddPerPointInfo(nRecIndex, UNION_POSITION, pt_sideD,  nSubSection);

					if(first_d_time_status!=first_a_time_status)
					{
						if(first_d_time_status)
							pCDC->SelectObject(&penActual);
						else
							pCDC->SelectObject(&penSched);
					}
					pCDC->MoveTo(pt_sideD);		
					pCDC->LineTo(pt_secondD);

					pCDC->SelectObject(pOldPen);
					penSched.DeleteObject();
					penActual.DeleteObject();

					single->AddPerPointInfo(nRecIndex, UNION_POSITION, pt_secondD,  nSubSection);
				}
				else
				{
					single->AddPerPointInfo(nRecIndex, UNION_POSITION, pt_secondA,  nSubSection);
				}
								
				DrawDepartTime(pCDC, first, pt_secondD,  lColor, lActualColor, nDrawMode, 1);
			}
			DrawStatStationDepart(pCDC, single, first,  pt_secondD, nDrawMode);
		}
		// end同一分段内相同车站间画接入交出
	}
	return;
}

void CTGDoc::DrawStopLine1(CDC*pCDC, CTG_TRAIN* single, TgTRAIN_RECORD* first, COLORREF lColor, COLORREF lActualColor, int nDrawMode)
{
	if(single == NULL || first == NULL)
		return;

	CPoint ptlStart, ptlEnd, ptlSide, ptlDepSide;
	WORD nRecIndex = first->rec_index;

	int in_index = -1, out_index = -1;
	if( 0 <= first->in_station_index && first->in_station_index < m_nDispDistanceNum)
		in_index = first->in_station_index;

	if(0 <= first->out_station_index && first->out_station_index < m_nDispDistanceNum)
		out_index = first->out_station_index;

	if(in_index == -1)
		in_index = out_index;
	if(out_index == -1)
		out_index = in_index;

	if(in_index == -1)
	{
		sysprint(0,0,"列车: %s 在车站: %d 的 in out index 均为-1,无法绘制站内停车", this->GetTrainShowId(single), first->station);
		return;
	}

	// 不是跳跃车站
	if(in_index == out_index)
	{
		ptlStart.x = m_nFirstArlMinuteOffset*m_fPixelPerMinute+(m_fPixelPerMinute*m_nFirstArlSecond)/60;
		ptlSide.x  = ptlStart.x;
		ptlEnd.x     = m_nFirstDepMinuteOffset*m_fPixelPerMinute+(m_fPixelPerMinute*m_nFirstDepSecond)/60;	
		ptlDepSide.x = ptlEnd.x;

		ptlSide.y = GetStationSideVPos(first, in_index, first->arrive_side);
		if(first->depart_side==first->arrive_side)
			ptlDepSide.y = ptlSide.y;
		else
			ptlDepSide.y = GetStationSideVPos(first, in_index, first->depart_side);

		if(IsFlagStart(first->flag))
		{
			if(first->out_sta_direct == UP_DIRECT)
			{
				ptlStart.y = this->GetVPosition(in_index, POS_FLAG::DOWN);
				ptlEnd.y  =  this->GetVPosition(in_index, POS_FLAG::UP);
			}
			else
			{
				ptlStart.y = this->GetVPosition(in_index, POS_FLAG::UP);
				ptlEnd.y  =  this->GetVPosition(in_index, POS_FLAG::DOWN);;
			}
		}
		else
		{
			if(first->in_sta_direct == UP_DIRECT)
				ptlStart.y = this->GetVPosition(in_index, POS_FLAG::DOWN);
			else
				ptlStart.y = this->GetVPosition(in_index, POS_FLAG::UP);

			if(first->out_sta_direct == UP_DIRECT)
				ptlEnd.y   = this->GetVPosition(in_index, POS_FLAG::UP);
			else
				ptlEnd.y   = this->GetVPosition(in_index, POS_FLAG::DOWN);
		}

		ptlEnd.Offset (x_point_offset,y_point_offset);
		ptlStart.Offset (x_point_offset,y_point_offset);
		ptlSide.Offset (x_point_offset,y_point_offset);
		ptlDepSide.Offset (x_point_offset,y_point_offset);

		int nSubSection = this->GetSubSection(in_index);
					
		// 通过
		if(ptlEnd.x == ptlStart.x)
		{
			if(m_sDispDistance[in_index].show_sation_sep > 0)
			{
				CPen penActual, penSched, *pOldPen;
				penSched.CreatePen(PS_SOLID, first->schd_width, first->schd_color);
				penActual.CreatePen(PS_SOLID, first->real_width, first->real_color);

				if(first_a_time_status)
					pOldPen=pCDC->SelectObject(&penActual);
				else
					pOldPen=pCDC->SelectObject(&penSched);

				if(!IsFlagStart(first->flag))
				{
					single->AddPerPointInfo(nRecIndex, UNION_POSITION, ptlStart, nSubSection);
					pCDC->MoveTo(ptlStart);  	
					pCDC->LineTo(ptlSide); 
				}
			
				if(0==m_sDispDistance[in_index].show_sides_mode && first->arrive_side > 0)
				{
					DrawStopSidePoint(pCDC, ptlSide, lColor);
				}
				single->AddPerPointInfo(nRecIndex, UNION_POSITION, ptlSide, nSubSection);
			
				if(first_d_time_status!=first_a_time_status)
				{
					if(first_d_time_status)
						pCDC->SelectObject(&penActual);
					else
						pCDC->SelectObject(&penSched);
				}

				if(!IsFlagTerminal(first->flag))
				{
					single->AddPerPointInfo(nRecIndex, UNION_POSITION, ptlEnd, nSubSection);
					pCDC->MoveTo(ptlSide);  	
					pCDC->LineTo(ptlEnd); 
				}

				pCDC->SelectObject(pOldPen);
				penSched.DeleteObject();
				penActual.DeleteObject();
			}
			else
			{
				single->AddPerPointInfo(nRecIndex, UNION_POSITION, ptlStart, nSubSection);
			}

			if(IsFlagTerminal(first->flag))
				DrawArrivalTime(pCDC, first, ptlStart,  lColor, lActualColor, nDrawMode);
			else
				DrawArrivalTime(pCDC, first, ptlEnd,  lColor, lActualColor, nDrawMode);
		}
		// 到开
		else
		{
			CPen penActual, penSched, *pOldPen=NULL;
			penSched.CreatePen(PS_SOLID, first->schd_width, first->schd_color);
			penActual.CreatePen(PS_SOLID, first->real_width, first->real_color);

			// 绘制 股道发点 股道到点 横线
			if(m_sDispDistance[in_index].show_sation_sep > 0)
			{
				if(first_a_time_status)
					pOldPen=pCDC->SelectObject(&penActual);
				else
					pOldPen=pCDC->SelectObject(&penSched);
				// 绘制 入口股道 到 停车股道 竖连线
				pCDC->MoveTo(ptlStart);  
				pCDC->LineTo(ptlSide);

				if(first_d_time_status!=first_a_time_status)
				{
					if(first_d_time_status)
						pCDC->SelectObject(&penActual);
					else
						pCDC->SelectObject(&penSched);
				}

				// 绘制 股道到点 出口股道 竖连线
				pCDC->MoveTo(ptlDepSide);  
				pCDC->LineTo(ptlEnd);

				// 设置选中点
				single->AddPerPointInfo(nRecIndex, ARRIVAL_POSITION, ptlStart, nSubSection);

				// 设置选中点
				single->AddPerPointInfo(nRecIndex, ARRIVAL_POSITION, ptlSide, nSubSection);
				// 设置选中点
				single->AddPerPointInfo(nRecIndex, DEPART_POSITION, ptlDepSide, nSubSection);

				// 设置选中点
				single->AddPerPointInfo(nRecIndex, DEPART_POSITION, ptlEnd, nSubSection);
			}
			else
			{
				// 设置选中点
				single->AddPerPointInfo(nRecIndex, ARRIVAL_POSITION, ptlStart, nSubSection);

				// 设置选中点
				single->AddPerPointInfo(nRecIndex, DEPART_POSITION, ptlEnd, nSubSection);

				if(first_d_time_status)
					pOldPen=pCDC->SelectObject(&penActual);
				else
					pOldPen=pCDC->SelectObject(&penSched);
			}

			if(IsPressureOutStation(first->stop_condition))
				PressureStopLine(pCDC, first, in_index, out_index, ptlSide, ptlDepSide);
			if(m_bStopStationWaveLine && JudgeNeedWaveLine(first,single->m_nDirectFlag))
			{
				if(first->in_sta_direct == DOWN_DIRECT)
					DrawChordLine(pCDC,ptlSide,ptlDepSide,DOWN_DIRECT);
				else
					DrawChordLine(pCDC,ptlSide,ptlDepSide,UP_DIRECT);
			}
			else
			{	
				if(this->GetTgType() == TG_TYPE::BASIC_TG)
					DrawBasicStationStopLine(pCDC,ptlSide,ptlDepSide);
				else
				{
					pCDC->MoveTo(ptlSide);
					pCDC->LineTo(ptlDepSide);
				}
			}

			if(pOldPen)
				pCDC->SelectObject(pOldPen);
			penSched.DeleteObject();
			penActual.DeleteObject();

			DrawArrivalTime(pCDC, first, ptlStart,  lColor, lActualColor, nDrawMode);
			DrawDepartTime(pCDC, first, ptlEnd,  lColor, lActualColor, nDrawMode, 1);
		}

		DrawStatStationArrival(pCDC, single, first, ptlStart, nDrawMode);
		DrawStatStationDepart(pCDC, single, first, ptlEnd, nDrawMode);		
	}
	// 跳跃车站
	else
	{
		// 第一个站停车表示
		ptlStart.x = m_nFirstArlMinuteOffset*m_fPixelPerMinute+(m_fPixelPerMinute*m_nFirstArlSecond)/60;
		ptlSide.x  = ptlStart.x;
		ptlEnd.x     = m_nFirstDepMinuteOffset*m_fPixelPerMinute+(m_fPixelPerMinute*m_nFirstDepSecond)/60;	
		ptlDepSide.x = ptlEnd.x;

		ptlSide.y = GetStationSideVPos(first, in_index, first->arrive_side);
		ptlDepSide.y = GetStationSideVPos(first, in_index, first->depart_side);

		if(IsFlagStart(first->flag))
		{
			if(first->out_sta_direct == UP_DIRECT)
			{
				ptlStart.y = this->GetVPosition(in_index, POS_FLAG::DOWN);
				ptlEnd.y  =  this->GetVPosition(in_index, POS_FLAG::UP);
			}
			else
			{
				ptlStart.y = this->GetVPosition(in_index, POS_FLAG::UP);
				ptlEnd.y  =  this->GetVPosition(in_index, POS_FLAG::DOWN);;
			}
		}
		else
		{
			if(first->in_sta_direct == UP_DIRECT)
			{
				ptlStart.y = this->GetVPosition(in_index, POS_FLAG::DOWN);
				ptlEnd.y   = this->GetVPosition(in_index, POS_FLAG::UP);
			}
			else
			{
				ptlStart.y = this->GetVPosition(in_index, POS_FLAG::UP);
				ptlEnd.y   = this->GetVPosition(in_index, POS_FLAG::DOWN);
			}
		}

		ptlEnd.Offset (x_point_offset,y_point_offset);
		ptlStart.Offset (x_point_offset,y_point_offset);
		ptlSide.Offset (x_point_offset,y_point_offset);
		ptlDepSide.Offset (x_point_offset,y_point_offset);

		int nSubSection = this->GetSubSection(in_index);
		// 通过
		if(ptlEnd.x == ptlStart.x)
		{
			if(m_sDispDistance[in_index].show_sation_sep > 0)
			{
				CPen penActual, penSched, *pOldPen;
				penSched.CreatePen(PS_SOLID, first->schd_width, first->schd_color);
				penActual.CreatePen(PS_SOLID, first->real_width, first->real_color);

				if(first_a_time_status)
					pOldPen=pCDC->SelectObject(&penActual);
				else
					pOldPen=pCDC->SelectObject(&penSched);

				if(!IsFlagStart(first->flag))
				{
					single->AddPerPointInfo(nRecIndex, UNION_POSITION, ptlStart, nSubSection);
					pCDC->MoveTo(ptlStart);  	
					pCDC->LineTo(ptlSide); 
				}

			
				if(0==m_sDispDistance[in_index].show_sides_mode && first->arrive_side > 0)
				{
					DrawStopSidePoint(pCDC, ptlSide, lColor);
				}
				single->AddPerPointInfo(nRecIndex, UNION_POSITION, ptlSide, nSubSection);
			
				if(first_d_time_status!=first_a_time_status)
				{
					if(first_d_time_status)
						pCDC->SelectObject(&penActual);
					else
						pCDC->SelectObject(&penSched);
				}

				if(!IsFlagTerminal(first->flag))
				{
					single->AddPerPointInfo(nRecIndex, UNION_POSITION, ptlEnd,  nSubSection);
					pCDC->MoveTo(ptlSide);  	
					pCDC->LineTo(ptlEnd); 
				}

				pCDC->SelectObject(pOldPen);
				penSched.DeleteObject();
				penActual.DeleteObject();
			}
			else
			{
				single->AddPerPointInfo(nRecIndex, UNION_POSITION, ptlStart, nSubSection);
			}

			DrawArrivalTime(pCDC, first, ptlStart,  lColor, lActualColor, nDrawMode);
		}
		// 到开
		else
		{
			CPen penActual, penSched, *pOldPen=NULL;
			penSched.CreatePen(PS_SOLID, first->schd_width, first->schd_color);
			penActual.CreatePen(PS_SOLID, first->real_width, first->real_color);

			// 绘制 股道发点 股道到点 横线
			if(m_sDispDistance[in_index].show_sation_sep > 0)
			{
				if(first_a_time_status)
					pOldPen=pCDC->SelectObject(&penActual);
				else
					pOldPen=pCDC->SelectObject(&penSched);
				// 绘制 入口股道 到 停车股道 竖连线
				pCDC->MoveTo(ptlStart);  
				pCDC->LineTo(ptlSide);

				if(first_d_time_status!=first_a_time_status)
				{
					if(first_d_time_status)
						pCDC->SelectObject(&penActual);
					else
						pCDC->SelectObject(&penSched);
				}
				// 绘制 股道到点 出口股道 竖连线
				pCDC->MoveTo(ptlDepSide);  
				pCDC->LineTo(ptlEnd);

				// 设置选中点
				single->AddPerPointInfo(nRecIndex, ARRIVAL_POSITION, ptlStart, nSubSection);

				// 设置选中点
				single->AddPerPointInfo(nRecIndex, ARRIVAL_POSITION, ptlSide, nSubSection);
				// 设置选中点
				single->AddPerPointInfo(nRecIndex, DEPART_POSITION, ptlDepSide, nSubSection);

				// 设置选中点
				single->AddPerPointInfo(nRecIndex, DEPART_POSITION, ptlEnd, nSubSection);
			}
			else
			{
				// 设置选中点
				single->AddPerPointInfo(nRecIndex, ARRIVAL_POSITION, ptlStart, nSubSection);

				// 设置选中点
				single->AddPerPointInfo(nRecIndex, DEPART_POSITION, ptlEnd, nSubSection);

				if(first_d_time_status)
					pOldPen=pCDC->SelectObject(&penActual);
				else
					pOldPen=pCDC->SelectObject(&penSched);
			}

			if(IsPressureOutStation(first->stop_condition))
				PressureStopLine(pCDC, first, in_index, out_index, ptlSide, ptlDepSide);
			if(m_bStopStationWaveLine && JudgeNeedWaveLine(first,single->m_nDirectFlag))
			{
				if(first->in_sta_direct == DOWN_DIRECT)
					DrawChordLine(pCDC,ptlSide,ptlDepSide,DOWN_DIRECT);
				else
					DrawChordLine(pCDC,ptlSide,ptlDepSide,UP_DIRECT);
			}
			else
			{	
				if(this->GetTgType() == TG_TYPE::BASIC_TG)
					DrawBasicStationStopLine(pCDC,ptlSide,ptlDepSide);
				else
				{
					pCDC->MoveTo(ptlSide);
					pCDC->LineTo(ptlDepSide);
				}
			}

			if(pOldPen)
				pCDC->SelectObject(pOldPen);
			penSched.DeleteObject();
			penActual.DeleteObject();

			DrawArrivalTime(pCDC, first, ptlStart,  lColor, lActualColor, nDrawMode);
			DrawDepartTime(pCDC, first, ptlEnd,  lColor, lActualColor, nDrawMode);
		}
		DrawStatStationArrival(pCDC, single, first, ptlStart, nDrawMode);

		// 第二个站停车表示
		CPoint pt_secondA, pt_secondD;
		CPoint pt_sideA,   pt_sideD;

		pt_secondA.x = m_nFirstArlMinuteOffset*m_fPixelPerMinute+(m_fPixelPerMinute*m_nFirstArlSecond)/60;
		pt_secondD.x = m_nFirstDepMinuteOffset*m_fPixelPerMinute+(m_fPixelPerMinute*m_nFirstDepSecond)/60;
		pt_sideA.x = pt_secondA.x;
		pt_sideD.x = pt_secondD.x;
		pt_sideA.y = GetStationSideVPos(first, out_index, first->arrive_side);
		pt_sideD.y = pt_sideA.y;

		if(UP_DIRECT == first->out_sta_direct)
		{
			pt_secondA.y = this->GetVPosition(out_index, POS_FLAG::DOWN);
			pt_secondD.y = this->GetVPosition(out_index, POS_FLAG::UP);
		}
		else
		{
			pt_secondA.y = this->GetVPosition(out_index, POS_FLAG::UP);
			pt_secondD.y = this->GetVPosition(out_index, POS_FLAG::DOWN);
		}

		pt_secondA.Offset(x_point_offset, y_point_offset);
		pt_secondD.Offset(x_point_offset, y_point_offset);
		pt_sideA.Offset(x_point_offset, y_point_offset);
		pt_sideD.Offset(x_point_offset, y_point_offset);

		nSubSection = this->GetSubSection(out_index);
		if(pt_secondA.x != pt_secondD.x)
		{
			CPen penActual, penSched, *pOldPen=NULL;
			penSched.CreatePen(PS_SOLID, first->schd_width, first->schd_color);
			penActual.CreatePen(PS_SOLID, first->real_width, first->real_color);

			if(m_sDispDistance[out_index].show_sation_sep > 0)
			{
				if(first_a_time_status)
					pOldPen=pCDC->SelectObject(&penActual);
				else
					pOldPen=pCDC->SelectObject(&penSched);
				pCDC->MoveTo(pt_secondA);
				pCDC->LineTo(pt_sideA);

				if(first_d_time_status!=first_a_time_status)
				{
					if(first_d_time_status)
						pCDC->SelectObject(&penActual);
					else
						pCDC->SelectObject(&penSched);
				}
				pCDC->MoveTo(pt_secondD);
				pCDC->LineTo(pt_sideD);

				single->AddPerPointInfo(nRecIndex, ARRIVAL_POSITION, pt_secondA, nSubSection);
				single->AddPerPointInfo(nRecIndex, DEPART_POSITION, pt_secondD,  nSubSection);
				single->AddPerPointInfo(nRecIndex, ARRIVAL_POSITION, pt_sideA,  nSubSection);
				single->AddPerPointInfo(nRecIndex, DEPART_POSITION, pt_sideD, nSubSection);
			}
			else
			{
				single->AddPerPointInfo(nRecIndex, ARRIVAL_POSITION, pt_secondA, nSubSection);
				single->AddPerPointInfo(nRecIndex, DEPART_POSITION, pt_secondD,  nSubSection);

				if(first_d_time_status)
					pOldPen=pCDC->SelectObject(&penActual);
				else
					pOldPen=pCDC->SelectObject(&penSched);
			}

			if(IsPressureOutStation(first->stop_condition))
				PressureStopLine(pCDC, first, in_index, out_index, pt_sideA, pt_sideD);
			if(m_bStopStationWaveLine && JudgeNeedWaveLine(first, single->m_nDirectFlag))
			{
				if(first->out_sta_direct == DOWN_DIRECT)
					DrawChordLine(pCDC, pt_sideA, pt_sideD, DOWN_DIRECT);
				else
					DrawChordLine(pCDC, pt_sideA, pt_sideD, UP_DIRECT);
			}
			else
			{	
				if(this->GetTgType() == TG_TYPE::BASIC_TG)
					DrawBasicStationStopLine(pCDC, pt_sideA, pt_sideD);
				else
				{
					pCDC->MoveTo(pt_sideA);
					pCDC->LineTo(pt_sideD);   
				}
			}

			if(pOldPen)
				pCDC->SelectObject(pOldPen);
			penSched.DeleteObject();
			penActual.DeleteObject();

			DrawArrivalTime(pCDC, first, pt_secondA,  lColor, lActualColor, nDrawMode);
			DrawDepartTime(pCDC, first, pt_secondD,  lColor, lActualColor, nDrawMode);
		}
		else
		{
			// 通过竖线
			if(m_sDispDistance[out_index].show_sation_sep > 0)
			{
				CPen penActual, penSched, *pOldPen;
				penSched.CreatePen(PS_SOLID, first->schd_width, first->schd_color);
				penActual.CreatePen(PS_SOLID, first->real_width, first->real_color);

				if(first_a_time_status)
					pOldPen=pCDC->SelectObject(&penActual);
				else
					pOldPen=pCDC->SelectObject(&penSched);
				pCDC->MoveTo(pt_secondA);	
				pCDC->LineTo(pt_sideA);
				single->AddPerPointInfo(nRecIndex, UNION_POSITION, pt_secondA,  nSubSection);

				if(m_sDispDistance[out_index].show_sides_mode == 0 && first->arrive_side > 0)
				{
					DrawStopSidePoint(pCDC, pt_sideD, lColor);
				}
				single->AddPerPointInfo(nRecIndex, UNION_POSITION, pt_sideD,  nSubSection);
				
				if(first_d_time_status!=first_a_time_status)
				{
					if(first_d_time_status)
						pCDC->SelectObject(&penActual);
					else
						pCDC->SelectObject(&penSched);
				}
				pCDC->MoveTo(pt_sideD);		
				pCDC->LineTo(pt_secondD);
				
				pCDC->SelectObject(pOldPen);
				penSched.DeleteObject();
				penActual.DeleteObject();

				single->AddPerPointInfo(nRecIndex, UNION_POSITION, pt_secondD,  nSubSection);
			}
			else
			{
				single->AddPerPointInfo(nRecIndex, UNION_POSITION, pt_secondA,  nSubSection);
			}
			
			DrawDepartTime(pCDC, first, pt_secondD,  lColor, lActualColor, nDrawMode);
		}
		DrawStatStationDepart(pCDC, single, first,  pt_secondD, nDrawMode);
	}

	
	return;
}

void CTGDoc::DrawStatStationDepart2(CDC * pCDC,CTG_TRAIN *single, TgTRAIN_RECORD* first, CPoint point,int nDrawMode)
{
	if(single == NULL || first == NULL)  
		return;

	if((GetEarlyLateTime() != single->m_nTrainType) && GetEarlyLateTime() != 3)
		return;

	bool bInhibit=this->m_ptgctrl->mConfigTg.m_tgdef.IsInhibitTrainType(m_nCurrentSectionNo, single->myTrainInfo.type);
	// 禁止统计早晚点列车,不配置画黑圈
	if(bInhibit && !this->m_ptgctrl->mConfigTg.m_context.IsEnableInhibitDrawBlackArc())
		return;

	if(first->static_depart==0)
		return;

	const TgTRAIN_MORE_INFO& info = single->myTrainInfo;
	if(info.type == 255) 
		return;

	int nWidth = 1; // 上海加粗，其它不需要
	if(this->m_ptgctrl->mConfigTg.m_context.IsHrbEarlyLateTimeBold())
		nWidth = 2;
	if(this->m_ptgctrl->mConfigTg.m_context.IsSelTrainBold())
	{
		if(m_pView->m_nFocusTrainIndex==single->GetTrainIndex() && (nDrawMode & DRAW_SCREEN) )
			nWidth = 2;
	}

	CPen nPen,nPen_red,nPen_Black; //晚点用篮圈,早点用红圈,禁止统计黑圈
	nPen.CreatePen ( PS_JOIN_ROUND, nWidth, CLR_BLUE);  
	nPen_red.CreatePen ( PS_JOIN_ROUND, nWidth, CLR_RED);
	nPen_Black.CreatePen ( PS_JOIN_ROUND, nWidth, CLR_BLACK);

	CFont* pOldFont   = pCDC->SelectObject(&fontEarlyLastTime);
	CPen*  nOldPen    = pCDC->SelectObject(&nPen);
	int  nOldColor    = pCDC->SetTextColor(CLR_BLUE);
	UINT nOldBackMode = pCDC->SetBkMode(TRANSPARENT);
	UINT nOldTextAlign= pCDC->SetTextAlign(TA_CENTER);

	int nDirectFlag = first->out_sta_direct;

	// 发点之差
	bool bNoEarlylatetime=false;
	std::map<ULONG, ULONG>::iterator it = m_noearlylatetime.find(single->GetTrainIndex());
	if(it != m_noearlylatetime.end())
	{
		bNoEarlylatetime=true;
	}
	
	// 统计发点
	int nRecNum = single->GetRecordNum();
	if((first->static_depart>0) && ((nRecNum-1 > first->rec_index) || IsFlagGiveout(first->flag)))
	{
		CString strDepartTime;
		BYTE nRetFlag = GetLateTime(first->plan_depart, first->depart, strDepartTime);
		if(bNoEarlylatetime)
		{
			pCDC->SelectObject(&nPen_red);
			pCDC->SetTextColor(CLR_RED);
		}
		else if(bInhibit)
		{
			pCDC->SelectObject(&nPen_Black);
			pCDC->SetTextColor(CLR_BLACK);
		}
		else
		{
			if(STATISTIC_FLAG::LATE == nRetFlag)
			{
				pCDC->SelectObject(&nPen);
				pCDC->SetTextColor(CLR_BLUE);
			}
			else if(STATISTIC_FLAG::EARLY == nRetFlag)
			{
				pCDC->SelectObject(&nPen_red);
				pCDC->SetTextColor(CLR_RED);
			}
			else
			{
				pCDC->SelectObject(&nPen_red);
				pCDC->SetTextColor(CLR_RED);
			}

			// 哈尔滨调度员反映，按照技规列车晚点5分钟（含5分钟）以内，应显示红圈蓝字，5分钟以外显示为蓝圈蓝字
			if(this->m_ptgctrl->mConfigTg.m_context.IsHrbStaticEarlyLateTimeColor() && STATISTIC_FLAG::LATE == nRetFlag)
			{
				if(strDepartTime.Find('.')<0)
				{
					int tm = atoi(strDepartTime);
					if(tm<=5)
					{
						pCDC->SelectObject(&nPen_red);
						pCDC->SetTextColor(CLR_BLUE);
					}
				}
			}
		}

		CRect rect;

		if(first->in_station_index>=0 && first->out_station_index>=0 && first->in_station_index != first->out_station_index)
		{
			double nRadius = 24.0;

			long x = GetXPositionConsiderSecond(first->depart);
			if(first->out_station_index < first->in_station_index)
			{
				long y=this->GetVPosition(first->in_station_index, POS_FLAG::UP);
				rect.left = x;
				rect.right = x + nRadius;

				rect.top = y - nRadius;
				rect.bottom  = y;
			}
			else
			{
				long y=this->GetVPosition(first->in_station_index, POS_FLAG::DOWN);
				rect.left = x;
				rect.right = x + nRadius;

				rect.top = y;
				rect.bottom  = y+nRadius;
			}
		
			CSize tx_size = pCDC->GetTextExtent(strDepartTime);
			if(rect.Width()<tx_size.cx)
			{
				int dlt=(tx_size.cx-rect.Width())/2+1;
				rect.left -= dlt;
				rect.right += dlt;
				rect.top -= dlt;
				rect.bottom += dlt;
			}

			rect.OffsetRect(1,0);

			if(m_bIsVerifyTrainRect)
			{
			#ifdef DRAW_TRAIN_RECT	
				ShowRect(pCDC, rect);
			#endif
			single->UnionRect(rect);
			}

			pCDC->Arc(&rect, CPoint(rect.left, (rect.top + rect.bottom )/2), CPoint(rect.left, (rect.top + rect.bottom )/2));

			if(!bNoEarlylatetime&&!bInhibit)
			{
				pCDC->TextOut(rect.CenterPoint().x, rect.CenterPoint().y - tx_size.cy/2,strDepartTime);
			}
		}
	}

	pCDC->SelectObject(pOldFont);
	pCDC->SelectObject(nOldPen);
	pCDC->SetTextColor(nOldColor);
	pCDC->SetBkMode(nOldBackMode);
	pCDC->SetTextAlign(nOldTextAlign);
	nPen_red.DeleteObject();
	nPen.DeleteObject();
	nPen_Black.DeleteObject();
}
