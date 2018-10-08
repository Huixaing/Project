#include "stdafx.h"
#include "math.h"
#include "TGForm.h"
#include "tgdoc.h"

int CTGDoc::GetEngineCount(const CTG_TRAIN *single, UINT rec_idx)
{
	int nCountOfEngine = 0;
	int cnt = single->myEngine.size();
	if( 0 == cnt)
	{
		nCountOfEngine = 0;
	}
	else
	{
		for(int eCount = 0; eCount < cnt;eCount++)
		{
			if(IsTractionEngineExt(single->myEngine[eCount].engine_status))
				continue;

			if(single->myEngine[eCount].attach_sta <= rec_idx)
			{
				nCountOfEngine ++;
			}
			if(single->myEngine[eCount].unattach_sta <= rec_idx)
			{
				nCountOfEngine --;
			}
		}
	}

	return nCountOfEngine;
}

void CTGDoc::DrawTrainRescuePart(CDC*pCDC, CTG_TRAIN* single, TgTRAIN_RECORD* first)
{
	if(first == NULL || first->special_train_index <= 0)
		return;
    
	long nFlag = first->flag;
	CTG_TRAIN* otherTrain = this->GetTrainByIndex(first->special_train_index);
	if(NULL == otherTrain)
		return;
    
	TgTRAIN_RECORD* otherRec = NULL, *tmprec = NULL;
	if(IsFlagRescued(first->flag))
	{
		for(int i=0; i<otherTrain->GetRecordNum(); i++)
		{
			TgTRAIN_RECORD* rec = otherTrain->GetRecordByPosition(i);
			if(NULL == rec)
				continue;
			if(IsFlagRescue(rec->flag))
			{
				otherRec = rec;
				break;
			}
		}
	}

	if(NULL == otherRec || first == NULL)
		return;

	CPoint ptBegin(0,0), ptEnd(0,0);

	int index = first->out_station_index;
	if(index <= 0)
		index = first->in_station_index;
    
	int direct_flag = UP_MARK;
	if(first->out_sta_direct == UP_MARK)
		direct_flag = DOWN_MARK;
	else
		direct_flag = UP_MARK;

	ptEnd.y = GetStationAbsPosition(single, first);
		
	ptBegin.y = ptEnd.y; 
	ptBegin.x = GetXPositionConsiderSecond(first->depart) + first->stop_abs_e_time*m_fPixelPerMinute;
	ptEnd.x   = GetXPositionConsiderSecond(otherRec->depart) + otherRec->stop_abs_s_time*m_fPixelPerMinute;
	
	ptBegin.Offset (x_point_offset,y_point_offset);
	ptEnd.Offset (x_point_offset,y_point_offset);

	//for actual time or plan time codeing   
	CPen mPen,*pOldPen;
	if(first_d_time_status)
		mPen.CreatePen(PS_DOT, 1, first->real_color);
	else
		mPen.CreatePen(PS_DOT, 1, first->schd_color);
	pOldPen = pCDC->SelectObject(&mPen);

	pCDC->MoveTo(ptBegin);  pCDC->LineTo(ptEnd);
	
	pCDC->SelectObject(pOldPen);
}

void CTGDoc::DrawMutiTrainSeparate(CDC*pCDC, CTG_TRAIN* single, TgTRAIN_RECORD* first)
{
	if(NULL == single || NULL == first)
		return;
   
	if(IsFlagStart(first->flag) || IsFlagTerminal(first->flag) || first->arrive_train_id == first->depart_train_id)
		return;
	
	CString strTrainId = first->depart_train_id;
	if(strTrainId.IsEmpty())
		return;

	int direct_flag;
	if(IsFlagStopAbs(first->flag))
	{
		if(IsFlagStopAbsUp(first->flag))
			direct_flag = DOWN_MARK;
		else
			direct_flag = UP_MARK; 
	}
	else
	{
		if(first->out_sta_direct == UP_DIRECT)
			direct_flag = UP_MARK;
		else
			direct_flag = DOWN_MARK;
	}

	UINT nOldAlign;
	UINT nOldMode = pCDC->SetBkMode(TRANSPARENT);	
	UINT nOldColor;

	CFont *pOldFont = pCDC->SelectObject(&fontReturnTrain);
	if(first_d_time_status)
		nOldColor = pCDC->SetTextColor(first->real_color);
	else
		nOldColor = pCDC->SetTextColor(first->schd_color);

	CPoint ptlStart;
	ptlStart.x =  m_nFirstArlMinuteOffset * m_fPixelPerMinute-5;	
    CSize trainidSize = pCDC->GetTextExtent(strTrainId);
	ptlStart.x = ptlStart.x + trainidSize.cx/4; 	

	if(direct_flag == DOWN_MARK)
		nOldAlign = pCDC->SetTextAlign(TA_LEFT|TA_BOTTOM);
	else
		nOldAlign = pCDC->SetTextAlign(TA_LEFT|TA_TOP);

	CSize sz=pCDC->GetTextExtent(strTrainId);
	if(first->out_station_index >= 0 && first->out_station_index < this->m_nDispDistanceNum)
	{
		if(direct_flag == DOWN_MARK)
			ptlStart.y = this->GetVPosition(first->out_station_index, POS_FLAG::DOWN);
		else
			ptlStart.y = this->GetVPosition(first->out_station_index, POS_FLAG::UP);

		ptlStart.x =  m_nFirstDepMinuteOffset * m_fPixelPerMinute-5;
		ptlStart.x = ptlStart.x + trainidSize.cx/4; 	
		ptlStart.Offset (x_point_offset,y_point_offset);
		pCDC->TextOut(ptlStart.x, ptlStart.y, strTrainId); //ok
		
		if(m_bIsVerifyTrainRect)
		{
		CRect rect=CRect(ptlStart.x,ptlStart.y-sz.cy,ptlStart.x+sz.cx,ptlStart.y+sz.cy);
		#ifdef DRAW_TRAIN_RECT	
			ShowRect(pCDC, rect);
		#endif
		single->UnionRect(rect);
		}
	}
	else
	{
		if(first->in_station_index >= 0 && first->in_station_index < this->m_nDispDistanceNum)
		{
			if(direct_flag == DOWN_MARK)
				ptlStart.y = this->GetVPosition(first->in_station_index, POS_FLAG::DOWN);
			else
				ptlStart.y = this->GetVPosition(first->in_station_index, POS_FLAG::UP);
			
			ptlStart.x =  m_nFirstDepMinuteOffset * m_fPixelPerMinute-5;
			ptlStart.x = ptlStart.x + trainidSize.cx/4; 	
			ptlStart.Offset (x_point_offset,y_point_offset);
			pCDC->TextOut(ptlStart.x, ptlStart.y, strTrainId); //ok

			if(m_bIsVerifyTrainRect)
			{
			CRect rect=CRect(ptlStart.x,ptlStart.y-sz.cy,ptlStart.x+sz.cx,ptlStart.y+sz.cy);
			#ifdef DRAW_TRAIN_RECT	
				ShowRect(pCDC, rect);
			#endif
			single->UnionRect(rect);
			}
		}
	}

	pCDC->SelectObject(pOldFont);
	pCDC->SetTextAlign(nOldAlign);
	pCDC->SetBkMode(nOldMode);
	pCDC->SetTextColor(nOldColor);	
}

void CTGDoc::DrawTrainStartOrAcceptMark(CDC*pCDC, CTG_TRAIN* single, TgTRAIN_RECORD* first, const TgTRAIN_MORE_INFO& train_info)
{
	if (first == NULL) 
		return;

	WORD nRecIndex = first->rec_index;
	int direct_flag;
	
	if(first->out_sta_direct == UP_DIRECT)
		direct_flag = DOWN_MARK;
	else
		direct_flag = UP_MARK;

	// prepair trainid
	CString tempTrain,hattrainid;
	tempTrain = this->GetTrainShowId(first->station, *first, single, (!TrainHaveActualPoint(single) && IsTrainFromShift(train_info)));
	hattrainid = first->original_arrive_train_id;

	if(!hattrainid.IsEmpty())
		tempTrain += "(" + hattrainid + ")";
	if(tempTrain.IsEmpty())
		tempTrain = "无车次";
	if(IsTrainSecret(single->myTrainInfo))
		tempTrain += "秘";
	CPoint ptlStart;
	if(IsFlagStart(first->flag))
	{
		if(first->out_station_index < 0 || first->out_station_index >= this->m_nDispDistanceNum)
			return;

		ptlStart.x =  m_nFirstDepMinuteOffset * m_fPixelPerMinute+(m_fPixelPerMinute*m_nFirstDepSecond)/60;	// 计算位置考虑秒,以下相同
		if(IsShowSidePos(first->depart_side, first->out_station_index))
		{
			ptlStart.y =  m_sDispDistance[first->out_station_index].nSideYPos[first->depart_side];
		}
		else
		{
			if(first->out_sta_direct == UP_DIRECT)
				ptlStart.y = this->GetVPosition(first->out_station_index, POS_FLAG::UP);
			else
				ptlStart.y = this->GetVPosition(first->out_station_index, POS_FLAG::DOWN);
		}
		ptlStart.Offset (x_point_offset,y_point_offset);


		if(first_a_time_status)
		{
			DrawMarkOfStart(pCDC, single, tempTrain, ptlStart, direct_flag, first->real_color, first->real_width, first);
		}
		else
		{
			DrawMarkOfStart(pCDC, single, tempTrain, ptlStart, direct_flag, first->schd_color, first->schd_width, first);
		}

		// 克服单点车始发终到选不中
		if(single->GetRecordNum() == 1)
		{
			int nSubSection = this->GetSubSection(first->out_station_index);
			if(IsFlagTerminal(first->flag))
			{
				single->AddPerPointInfo(nRecIndex, UNION_POSITION, ptlStart, nSubSection);
			}
		}
	}
	//绘制区间始发的标记
	else if(IsFlagAbsStart(first->flag))
	{	
		int idx = first->in_station_index;
		if(idx == -1)
			idx = first->out_station_index;

		int stop_abs_time = train_info.stop_abs_time;
			
		ptlStart.x = (m_nFirstArlMinuteOffset - stop_abs_time) * m_fPixelPerMinute;

		int text_align;
		if(IsAbsStartStopDown(train_info.adjust_flag))
		{
			direct_flag = DOWN_MARK;
			text_align = pCDC->SetTextAlign(TA_RIGHT|TA_BOTTOM);
			ptlStart.y =  this->GetVPosition(idx, POS_FLAG::DOWN);
			int tmp=GetAbsPreStationIndexByNextStationAndEntry(first->station, first->entry, false, first->in_station_index);
			if(tmp==-1)
				tmp=idx+1;
			ptlStart.y += (this->GetVPosition(tmp, POS_FLAG::UP) - this->GetVPosition(idx, POS_FLAG::DOWN))/2;
		}
		else
		{
			direct_flag = UP_MARK; 
			text_align = pCDC->SetTextAlign(TA_RIGHT|TA_TOP);
			ptlStart.y =  this->GetVPosition(idx, POS_FLAG::UP);
			int tmp=GetAbsPreStationIndexByNextStationAndEntry(first->station, first->entry, true, first->in_station_index);
			if(tmp==-1)
				tmp=idx-1;
			ptlStart.y -= (this->GetVPosition(idx, POS_FLAG::UP) - this->GetVPosition(tmp, POS_FLAG::DOWN))/2;
		}

		ptlStart.Offset(x_point_offset,y_point_offset);
		
		if(IsAbsAttainEngineReturn(train_info.adjust_flag))
		{
			;
		}
		else
		{
		CString strAbsArlTime;
		int usTimeValue = (first->arrive/60 - stop_abs_time); // 20161031 上海局自动平移模式 区间停车到点会变化
		strAbsArlTime.Format("%d", usTimeValue%10);
		
		CFont *pOldFont = pCDC->SelectObject(&fontAbsTime);

		UINT nOldColor;
		if(first_d_time_status)
			nOldColor = pCDC->SetTextColor(first->real_color);
		else
			nOldColor = pCDC->SetTextColor(first->schd_color);
		pCDC->TextOut(ptlStart.x, ptlStart.y, strAbsArlTime);

		if(m_bIsVerifyTrainRect)
		{
		CSize sz=pCDC->GetTextExtent(strAbsArlTime);
		CRect rect=CRect(ptlStart.x-sz.cx,ptlStart.y-sz.cy,ptlStart.x,ptlStart.y+sz.cy);
		#ifdef DRAW_TRAIN_RECT	
			ShowRect(pCDC, rect);
		#endif
		single->UnionRect(rect);
		}

		pCDC->SetTextColor(nOldColor);
		pCDC->SelectObject(pOldFont);  
		pCDC->SetTextAlign(text_align);

		if(first_a_time_status)
		{
			DrawMarkOfStart(pCDC,single,tempTrain,ptlStart,direct_flag,first->real_color,first->real_width, first);
		}
		else
		{
			DrawMarkOfStart(pCDC,single,tempTrain,ptlStart,direct_flag,first->schd_color,first->schd_width, first);
		}
		single->AddPerPointInfo(nRecIndex, ABS_START_POSITION, ptlStart, this->GetSubSection(idx));
		}
		// 绘制区间停车点到车站连线
		CPoint ptlEnd;
		ptlEnd.x = m_nFirstArlMinuteOffset*m_fPixelPerMinute+(m_fPixelPerMinute*m_nFirstArlSecond)/60;
		if(IsAbsStartStopUp(train_info.adjust_flag))
			ptlEnd.y = this->GetVPosition(first->in_station_index, POS_FLAG::UP);
		else
			ptlEnd.y = this->GetVPosition(first->in_station_index, POS_FLAG::DOWN);
		ptlEnd.Offset(x_point_offset,y_point_offset);
		
		if(IsAbsAttainEngineReturn(train_info.adjust_flag))
		{
			if(this->GetTgType() == TG_TYPE::BASIC_TG)
				DrawBasicSchdLine(pCDC, ptlStart, ptlEnd, first, true);
			else if(first_d_time_status)
				DrawActualLine(pCDC, ptlStart, ptlEnd, first, true);
			else
				DrawSchdLine(pCDC, ptlStart, ptlEnd, first, true);
		}
		else
		{
			if(this->GetTgType() == TG_TYPE::BASIC_TG)
				DrawBasicSchdLine(pCDC, ptlStart, ptlEnd, first, false);
			else if(first_d_time_status)
				DrawActualLine(pCDC, ptlStart, ptlEnd, first, true);
			else
				DrawSchdLine(pCDC, ptlStart, ptlEnd, first, false);
		}
	}
	else
	{  
		ptlStart.x =  m_nFirstArlMinuteOffset*m_fPixelPerMinute+(m_fPixelPerMinute*m_nFirstArlSecond)/60;
		if(first->in_sta_direct == UP_DIRECT)
			ptlStart.y = this->GetVPosition(first->in_station_index, POS_FLAG::DOWN);
		else
			ptlStart.y = this->GetVPosition(first->in_station_index, POS_FLAG::UP);
		ptlStart.Offset (x_point_offset,y_point_offset);

		if(first_a_time_status)
		{
			DrawMarkOfArrive(pCDC,single,tempTrain,ptlStart,direct_flag,first->real_color,first->real_width,first);
		}
		else
		{
			DrawMarkOfArrive(pCDC,single,tempTrain,ptlStart,direct_flag,first->schd_color,first->schd_width,first);
		}

		// 输出接入方向提示名称
		DrawTrainAcceptDirection(pCDC, first, direct_flag, ptlStart, single);
	}
}

WORD CTGDoc::GetStationAbsPosition(CTG_TRAIN* single, TgTRAIN_RECORD* rec)
{
	if(NULL == rec)
		return 0;

	int index = -1;
	TgTRAIN_RECORD* next = single->GetRecordByPosition(rec->rec_index+1);
	if(NULL != next && next->station != rec->station)
	{
		index = next->in_station_index;
	}
	else
	{
		int abs_no = GetAbsByStationNoAndEntry(rec->station, rec->exit);
		if(abs_no != -1)
		{
			WORD sta_no = -1;
			const ABS_LINE* line = this->m_ptgctrl->mConfigTg.m_tgdef.GetLineDefine(abs_no);
			if(line!=NULL)
			{
				if(line->nStartStation == rec->station)
					sta_no = line->nEndStation;
				else if(line->nEndStation == rec->station)
					sta_no = line->nStartStation;
			}

			if(sta_no > 0)
			{
				if(IsFlagStopAbsUp(rec->flag))
				{
					for(int i = rec->out_station_index-1; i >= 0; i--)
					{
						if(m_sDispDistance[i].station_number == sta_no)
						{
							index = i;
							break;
						}
					}
				}
				else
				{
					for(int i = rec->out_station_index+1; i < this->m_nDispDistanceNum; i++)
					{
						if(m_sDispDistance[i].station_number == sta_no)
						{
							index = i;
							break;
						}
					}
				}
			}
		}
	}

	if(index < 0 || index >= this->m_nDispDistanceNum)
	{
		if(IsFlagStopAbsUp(rec->flag))
			index = rec->out_station_index-1;
		else
			index = rec->out_station_index+1;
	}

	WORD abs_pos = 0;
	if(index < 0 || index >= this->m_nDispDistanceNum)
	{
		if(IsFlagStopAbsUp(rec->flag))
			abs_pos = (2*this->GetVPosition(rec->out_station_index, POS_FLAG::UP)-20)/2;
		else
			abs_pos = (2*this->GetVPosition(rec->out_station_index, POS_FLAG::DOWN)+20)/2;
	}
	else
	{
		if(IsFlagStopAbsUp(rec->flag))
			abs_pos = (this->GetVPosition(index, POS_FLAG::DOWN) + this->GetVPosition(rec->out_station_index, POS_FLAG::UP))/2;
		else
			abs_pos = (this->GetVPosition(index, POS_FLAG::UP) + this->GetVPosition(rec->out_station_index, POS_FLAG::DOWN))/2;
	}

	return abs_pos;
}

void CTGDoc::DrawTrainEndOrDepartMark(CDC * pCDC, CTG_TRAIN* single, TgTRAIN_RECORD* first)
{
	if(NULL == first || NULL == single)
		return;

	CPoint ptlStart;
	int direct_flag = UP_MARK;

	if(IsFlagAbsTerminal(first->flag))
	{
		WORD nDirect = UP_DIRECT;
		if(IsFlagStopAbsUp(first->flag))
		{
			direct_flag = UP_MARK;
			nDirect = UP_DIRECT;
		}
		else
		{
			direct_flag = DOWN_MARK;
			nDirect = DOWN_DIRECT;
		}
		
		if(IsFlagRescued(first->flag))
		{
			DrawTrainRescuePart(pCDC, single, first);
		}

		ptlStart.x = (m_nFirstDepMinuteOffset + first->stop_abs_s_time)  * m_fPixelPerMinute + (m_fPixelPerMinute*m_nFirstDepSecond)/60;
		ptlStart.y = GetStationAbsPosition(single, first);

		int pos=0;
		if(first->out_sta_direct == UP_DIRECT)
			pos = this->GetVPosition(first->in_station_index, POS_FLAG::DOWN);
		else
			pos = this->GetVPosition(first->in_station_index, POS_FLAG::UP);

		int dlt = (ptlStart.y - pos)*2/3;
		if(IsAbsDrawUp(first->stop_reason))
			ptlStart.y = pos + dlt;
		else if(IsAbsDrawBottom(first->stop_reason))
			ptlStart.y = pos + dlt*2;
	
		ptlStart.Offset (x_point_offset,y_point_offset);
		
		if(ptlStart.x<m_nPageWidth && ptlStart.y > 0 && !IsFlagTrainWholeHelp(first->flag))
		{
			if(first_d_time_status)
				DrawMarkOfEnd(pCDC,single,ptlStart,direct_flag,first->real_color,first->flag,first->real_width);
			else
				DrawMarkOfEnd(pCDC,single,ptlStart,direct_flag,first->schd_color,first->flag,first->schd_width);
		}
	}
	else if(IsFlagTerminal(first->flag))
	{		
		if(first->in_station_index < 0 || first->in_station_index >= this->m_nDispDistanceNum)
			return;

		if(first->in_sta_direct == UP_DIRECT)
			direct_flag = UP_MARK;
		else
			direct_flag = DOWN_MARK;

		ptlStart.x = m_nFirstDepMinuteOffset * m_fPixelPerMinute+(m_fPixelPerMinute*m_nFirstDepSecond)/60;	

		if(IsShowSidePos(first->arrive_side, first->in_station_index))
		{
			ptlStart.y =  m_sDispDistance[first->in_station_index].nSideYPos[first->arrive_side];
		}
		else
		{
			if(first->in_sta_direct == UP_DIRECT)
				ptlStart.y = this->GetVPosition(first->in_station_index, POS_FLAG::DOWN);
			else
				ptlStart.y = this->GetVPosition(first->in_station_index, POS_FLAG::UP);
		}
		
		if(IsFlagStopAbs(first->flag))
		{
			ptlStart.x = (m_nFirstDepMinuteOffset + first->stop_abs_s_time)  * m_fPixelPerMinute;
			ptlStart.y = GetStationAbsPosition(single, first);
			if(IsFlagStopAbsUp(first->flag))
			{
				direct_flag = UP_MARK;
			}
			else
			{
				direct_flag = DOWN_MARK;
			}
			
			if(IsFlagRescued(first->flag))
			{
				DrawTrainRescuePart(pCDC, single, first);
			}
		}
		ptlStart.Offset (x_point_offset,y_point_offset);

		if(ptlStart.x<m_nPageWidth && ptlStart.y > 0)
		{
			if(first_d_time_status)
				DrawMarkOfEnd(pCDC,single,ptlStart,direct_flag,first->real_color,first->flag,first->real_width);
			else
				DrawMarkOfEnd(pCDC,single,ptlStart,direct_flag,first->schd_color,first->flag,first->schd_width);
		}

		if(m_bShowReturnTrainid || 
			(m_bShowSelCZHLine && (m_CurSelCZH==single->myTrainInfo.shift_index_string) && (single->m_nTrainIndex==m_pView->m_nFocusTrainIndex)))
		{
			if(m_ptgctrl->mConfigTg.m_context.IsSetCZHReturnTrainID())
			{
				if(first->arrive_train_id != first->depart_train_id && first->depart_train_id!="")
				{
					if(first_d_time_status)
						DrawMarkOfZheFanTrainID(pCDC,ptlStart,direct_flag,first->real_color,first->depart_train_id,single);
					else
						DrawMarkOfZheFanTrainID(pCDC,ptlStart,direct_flag,first->schd_color,first->depart_train_id,single);
				}
			}
			else
			{
				CString strReturnTrainID;
				if(myChezuhao.GetReturnTrainID(single, strReturnTrainID))
				{
					if(first_d_time_status)
						DrawMarkOfZheFanTrainID(pCDC,ptlStart,direct_flag,first->real_color, strReturnTrainID,single);
					else
						DrawMarkOfZheFanTrainID(pCDC,ptlStart,direct_flag,first->schd_color, strReturnTrainID,single);
				}
			}
		}
	}
	else 
	{ 
		if(first->out_station_index < 0 || first->out_station_index >= this->m_nDispDistanceNum)
			return;

		if(first->in_sta_direct == UP_DIRECT)
			direct_flag = UP_MARK;
		else
			direct_flag = DOWN_MARK;

		ptlStart.x =  m_nFirstDepMinuteOffset * m_fPixelPerMinute+(m_fPixelPerMinute*m_nFirstDepSecond)/60;	

		if(first->out_sta_direct == UP_DIRECT)
			ptlStart.y = this->GetVPosition(first->out_station_index, POS_FLAG::UP);
		else
			ptlStart.y = this->GetVPosition(first->out_station_index, POS_FLAG::DOWN);
		ptlStart.Offset (x_point_offset,y_point_offset);


		if(ptlStart.x<m_nPageWidth && ptlStart.y > 0)
		{
			if(first_d_time_status)
				DrawMarkOfDepart(pCDC,single,ptlStart,direct_flag,first->real_color,first->real_width,first);
			else
				DrawMarkOfDepart(pCDC,single,ptlStart,direct_flag,first->schd_color,first->schd_width,first);
			// 输出交出方向提示
			DrawTrainGiveoutDirection(pCDC, first, direct_flag, ptlStart, single);
		}
	}

	return;
}

bool CTGDoc::IsShowSidePos(int nSideNo, int index)
{
	if(m_sDispDistance[index].show_sation_sep==0)
		return false;

	for(int idx=0; idx<m_sDispDistance[index].number_of_side; idx++)
	{
		if(m_sDispDistance[index].side[idx].side_number == nSideNo)
			return true;
		if(m_sDispDistance[index].side[idx].side_number == -1)
			continue;
	}

	return false;
}
void CTGDoc::DrawAbsStopLine(CDC*pCDC, CTG_TRAIN* single, TgTRAIN_RECORD* first, TgTRAIN_RECORD* second)
{
	if(NULL == first)
		return;

	WORD nRecIndex = first->rec_index;

	if(!IsFlagStopAbs(first->flag) && !IsFlagStopSignal(first->flag))
		return;
    
	if(first->out_station_index < 0 || first->out_station_index >= this->m_nDispDistanceNum)
		return;

	CPoint ptlStart, ptlEnd;
	//1. 绘制从车站到区间停车第一个点的连线
	ptlStart.x =  m_nFirstDepMinuteOffset * m_fPixelPerMinute+(m_fPixelPerMinute*m_nFirstDepSecond)/60;	
	if(first->out_sta_direct == DOWN_DIRECT)//down direct 
		ptlStart.y = this->GetVPosition(first->out_station_index, POS_FLAG::DOWN);
	else
		ptlStart.y = this->GetVPosition(first->out_station_index, POS_FLAG::UP);

	ptlEnd.x = (m_nFirstDepMinuteOffset + first->stop_abs_s_time)  * m_fPixelPerMinute+(m_fPixelPerMinute*m_nFirstDepSecond)/60;

	if(IsFlagStopAbs(first->flag))
	{
		int absY = GetStationAbsPosition(single, first);
		int dlt = absY - ptlStart.y;
	
		if(IsAbsDrawUp(first->stop_reason))
			ptlEnd.y = ptlStart.y + dlt/2;
		else if(IsAbsDrawBottom(first->stop_reason))
			ptlEnd.y = ptlStart.y + dlt/2 + dlt;
		else
			ptlEnd.y = ptlStart.y + dlt;
	}
	else if(IsFlagStopSignal(first->flag))
		ptlEnd.y = ptlStart.y + GetAbsYPosition(single, first->rec_index, 1); 
	else
		return;

	ptlStart.Offset(x_point_offset, y_point_offset);
	ptlEnd.Offset(x_point_offset, y_point_offset);
   
	// 区间停车到点
	CString strAbsArlTime;
	int usTimeValue = first->depart/60 + first->stop_abs_s_time;  // 20161031 上海局自动平移模式 区间停车到点会变化
	strAbsArlTime.Format("%d", usTimeValue%10);
	int text_align;
	if(first->out_sta_direct == DOWN_DIRECT)
		text_align = pCDC->SetTextAlign(TA_LEFT|TA_BOTTOM);//TA_TOP   YUZHU
	else
		text_align = pCDC->SetTextAlign(TA_LEFT|TA_TOP);//TA_BOTTOM   YUZHU
	
	CFont *pOldFont = pCDC->SelectObject(&fontAbsTime);

	UINT nOldColor;
	if(first_d_time_status && second_a_time_status)
		nOldColor = pCDC->SetTextColor(first->real_color);
	else
		nOldColor = pCDC->SetTextColor(first->schd_color);

	pCDC->TextOut(ptlEnd.x, ptlEnd.y, strAbsArlTime);

	if(m_bIsVerifyTrainRect)
	{
	CSize sz=pCDC->GetTextExtent(strAbsArlTime);
	CRect rect=CRect(ptlEnd.x,ptlEnd.y-sz.cy,ptlEnd.x+sz.cx,ptlEnd.y+sz.cy);
	#ifdef DRAW_TRAIN_RECT	
		ShowRect(pCDC, rect);
	#endif
	single->UnionRect(rect);
	}
	pCDC->SetTextColor(nOldColor);
	pCDC->SelectObject(pOldFont);  
	pCDC->SetTextAlign(text_align);
	
	
	// 绘制车站发点到区间停车到点
	if(this->GetTgType() == TG_TYPE::BASIC_TG)
		DrawBasicSchdLine(pCDC, ptlStart, ptlEnd, first, false);
	else if(first_d_time_status)
		DrawActualLine(pCDC, ptlStart, ptlEnd, first, true);
	else
		DrawSchdLine(pCDC, ptlStart, ptlEnd, first, true);
	//DrawBadMark(pCDC, single, ptlStart, ptlEnd, first);
	single->AddPerPointInfo(nRecIndex, ABS_ARRIVAL_POSITION, ptlEnd, this->GetSubSection(first->out_station_index));

	// 输出区间信息
	if(!first->szAbsText.IsEmpty())
	{
		if(second!=NULL)
		{
			if(IsFlagTrainPartRun(first->flag))
			{
				DrawAbsText(pCDC, single, first, ptlStart, ptlEnd, false);
			}
			else
			{
				DrawAbsText(pCDC, single, first, ptlEnd, ptlEnd, true);
			}
		}
		else
		{
			if(IsFlagTrainWholeHelp(first->flag))
			{
				DrawAbsText(pCDC, single, first, ptlStart, ptlEnd, false);
			}
			else
			{
				DrawAbsText(pCDC, single, first, ptlEnd, ptlEnd, true);
			}
		}
	}

	if(second!=NULL)
	{
		ptlStart = ptlEnd;
		//2. 绘制从区间停车第一个点第二个点的连线
		ptlEnd.x = (m_nFirstDepMinuteOffset + first->stop_abs_e_time) * m_fPixelPerMinute + (m_fPixelPerMinute*m_nFirstDepSecond)/60;
		ptlEnd.Offset(x_point_offset, y_point_offset);

		// 区间停车发点
		CString strAbsDepTime;
		usTimeValue = first->depart/60 + first->stop_abs_e_time; // 20161031 上海局自动平移模式 区间停车发点会变化
		strAbsDepTime.Format("%d", usTimeValue%10);
		if(first->out_station_index == second->in_station_index) // 区间折返
		{
			if(first->out_sta_direct == DOWN_DIRECT)
				text_align = pCDC->SetTextAlign(TA_RIGHT|TA_BOTTOM);
			else
				text_align = pCDC->SetTextAlign(TA_RIGHT|TA_TOP);
		}
		else // 区间停车前行
		{
			if(first->out_sta_direct == DOWN_DIRECT)
				text_align = pCDC->SetTextAlign(TA_RIGHT|TA_TOP);
			else
				text_align = pCDC->SetTextAlign(TA_RIGHT|TA_BOTTOM);
		}
		pOldFont = pCDC->SelectObject(&fontAbsTime);

		if(first_d_time_status && second_a_time_status)
			nOldColor = pCDC->SetTextColor(second->real_color);
		else
			nOldColor = pCDC->SetTextColor(second->schd_color);

		pCDC->TextOut(ptlEnd.x, ptlEnd.y, strAbsDepTime);  
		
		if(m_bIsVerifyTrainRect)
		{
		CSize sz=pCDC->GetTextExtent(strAbsDepTime);
		CRect rect=CRect(ptlEnd.x-sz.cx,ptlEnd.y-sz.cy,ptlEnd.x,ptlEnd.y+sz.cy);
		#ifdef DRAW_TRAIN_RECT	
			ShowRect(pCDC, rect);
		#endif
		single->UnionRect(rect);
		}

		pCDC->SetTextColor(nOldColor);
		pCDC->SelectObject(pOldFont);  
		pCDC->SetTextAlign(text_align);

		if(this->GetTgType() == TG_TYPE::BASIC_TG)
			DrawBasicSchdLine(pCDC, ptlStart, ptlEnd, first, false);
		else if(first_d_time_status && second_a_time_status)
			DrawActualLine(pCDC, ptlStart, ptlEnd, first, false);
		else
			DrawSchdLine(pCDC, ptlStart, ptlEnd, first, false);

		single->AddPerPointInfo(nRecIndex, ABS_DEPART_POSITION, ptlEnd, this->GetSubSection(first->out_station_index));

		//3. 绘制机外停车的小三角标志
		if(IsFlagStopSignal(first->flag))
		{
			DrawStopOutofSignalMark(pCDC, ptlStart, ptlEnd, first->out_sta_direct);
		}
		ptlStart = ptlEnd;

		//4. 绘制区间停车发车点到下一站
		if(second->in_station_index < 0 || second->in_station_index >= this->m_nDispDistanceNum)
			return;
		ptlEnd.x =m_nSecondArlMinuteOffset * m_fPixelPerMinute+(m_fPixelPerMinute*m_nSecondArlSecond)/60;
		if(second->in_sta_direct == DOWN_DIRECT)
			ptlEnd.y = this->GetVPosition(second->in_station_index, POS_FLAG::UP);
		else
			ptlEnd.y = this->GetVPosition(second->in_station_index, POS_FLAG::DOWN);

		ptlEnd.Offset(x_point_offset, y_point_offset);

		if(this->GetTgType() == TG_TYPE::BASIC_TG)
			DrawBasicSchdLine(pCDC, ptlStart, ptlEnd, second, false);
		else if(first_d_time_status && second_a_time_status)
			DrawActualLine(pCDC, ptlStart, ptlEnd, second, true);
		else
			DrawSchdLine(pCDC, ptlStart, ptlEnd, second, true);
		//DrawBadMark(pCDC, single, ptlStart, ptlEnd, second);
	}
	
	return;
}

bool CTGDoc::DrawAbsLine(CDC*pCDC, CTG_TRAIN* single, TgTRAIN_RECORD* first, TgTRAIN_RECORD* second, CPoint& ptlStart, CPoint& ptlEnd)
{
	if(NULL == first || NULL == second)
		return false;

	if(IsFlagStopAbs(first->flag) || IsFlagStopSignal(first->flag))
		return false;

	if((first->out_station_index < 0 || first->out_station_index >= this->m_nDispDistanceNum ||
		second->in_station_index < 0 || second->in_station_index >= this->m_nDispDistanceNum))
		return false;

	ptlStart.x = m_nFirstDepMinuteOffset * m_fPixelPerMinute+(m_fPixelPerMinute*m_nFirstDepSecond)/60;	
	if(first->out_sta_direct == DOWN_DIRECT)
		ptlStart.y = this->GetVPosition(first->out_station_index, POS_FLAG::DOWN);
	else
		ptlStart.y = this->GetVPosition(first->out_station_index, POS_FLAG::UP);

	ptlEnd.x =m_nSecondArlMinuteOffset * m_fPixelPerMinute+(m_fPixelPerMinute*m_nSecondArlSecond)/60;
	if(first->out_sta_direct == DOWN_DIRECT)
		ptlEnd.y = this->GetVPosition(second->in_station_index, POS_FLAG::UP);
	else
		ptlEnd.y = this->GetVPosition(second->in_station_index, POS_FLAG::DOWN);

	ptlStart.Offset(x_point_offset, y_point_offset);
	ptlEnd.Offset(x_point_offset, y_point_offset);

	bool bDrawLine=true;
	if(this->m_sDispDistance[first->out_station_index].sub_section != this->m_sDispDistance[second->in_station_index].sub_section)
	{
		if(this->m_ptgctrl->mConfigTg.m_context.IsEnableNoDrawLineInNoSameSubTable())
		{
			// 绘制交出符号
			int direct_flag=UP_MARK;
			if(first->in_sta_direct == UP_DIRECT)
				direct_flag = UP_MARK;
			else
				direct_flag = DOWN_MARK;

			if(ptlStart.x<m_nPageWidth && ptlStart.y > 0)
			{
				if(first_d_time_status)
					DrawMarkOfDepart(pCDC,single,ptlStart,direct_flag,first->real_color,first->real_width,first);
				else
					DrawMarkOfDepart(pCDC,single,ptlStart,direct_flag,first->schd_color,first->schd_width,first);
			}

			// 绘制接入符号
			if(first->out_sta_direct == UP_DIRECT)
				direct_flag = DOWN_MARK;
			else
				direct_flag = UP_MARK;
			
			if(first_a_time_status)
			{
				DrawMarkOfArrive(pCDC,single,second->arrive_train_id,ptlEnd,direct_flag,first->real_color,first->real_width,first);
			}
			else
			{
				DrawMarkOfArrive(pCDC,single,second->arrive_train_id,ptlEnd,direct_flag,first->schd_color,first->schd_width,first);
			}

			bDrawLine=false;
		}
	}
	
	if(bDrawLine)
	{
		if(this->GetTgType() != TG_TYPE::BASIC_TG)
		{
			// 绘制外包黄线
			if(line_style.elec_rank.decorate > 0)
			{
				if(m_bElecTrainDrawGreenDecorate)
				{
					if(IsElectricEngineTrain(*first))
					{
						DrawTrainDecorateGreenLine(pCDC, ptlStart, ptlEnd);
					}
				}
				else
				{
					if(!IsElectricEngineTrain(*first))
					{
						DrawTrainDecorateGreenLine(pCDC, ptlStart, ptlEnd);
					}
				}
			}
		}

	    if(this->GetTgType() == TG_TYPE::BASIC_TG)
			DrawBasicSchdLine(pCDC, ptlStart, ptlEnd, first, true);
	    else if(first_d_time_status && second_a_time_status)
		    DrawActualLine(pCDC, ptlStart, ptlEnd, first, true);
	    else
			DrawSchdLine(pCDC, ptlStart, ptlEnd, first, true);
	}

	return bDrawLine;
}

//draw Divide Train	and Combine Train
void CTGDoc::DrawDivideOrCombineTrainMark(CDC*pCDC, TgTRAIN_RECORD *first)
{
	if(NULL == first)
		return;

	if(!IsFlagDivide(first->flag))
		return;

	do
	{
		CTG_TRAIN *pFirstTrain = GetTrainByIndex(first->special_train_index);
		if(pFirstTrain == NULL)
			break;

		TgTRAIN_RECORD *rec = pFirstTrain->GetLastRecord();
		if(rec == NULL)
			break;

		if(rec->special_train_index <= 0)
		{
			rec = pFirstTrain->GetFirstRecord();
		}
		if(rec->special_train_index <= 0)
		{
			int i=0;
			for(; i<pFirstTrain->GetRecordNum(); i++)
			{
				rec = pFirstTrain->GetRecordByRecIndex(i);
				if(IsFlagDivided(rec->flag))
					break;
			}
			if(i>=pFirstTrain->GetRecordNum())
				return;
		}

		CPoint ptFirst, ptSecond;
		if(first->in_sta_direct == 2)
		{
			ptFirst.y = ptSecond.y = this->GetVPosition(first->in_station_index, POS_FLAG::DOWN)+3;
		}
		else
		{
			ptFirst.y = ptSecond.y = this->GetVPosition(first->in_station_index, POS_FLAG::UP)-3;
		}

		ptFirst.x = GetXPositionConsiderSecond(first->depart);
		ptSecond.x = GetXPositionConsiderSecond(rec->arrive);

		CPen mPen,*pOldPen;
		mPen.CreatePen(PS_DOT,1,RGB(0,0,255));
		pOldPen = pCDC->SelectObject(&mPen);

		if (m_bStopStationWaveLine)	
		{
			if(ptFirst.x<=ptSecond.x)
				DrawChordLine(pCDC,ptFirst,ptSecond, first->in_sta_direct);	
			else
				DrawChordLine(pCDC,ptSecond,ptFirst, first->in_sta_direct);
		}
		else
		{
			pCDC->MoveTo(ptFirst);
			pCDC->LineTo(ptSecond);
		}
		pCDC->SelectObject(pOldPen);
		mPen.DeleteObject();

	}while(0);

	return;
}

BOOL CTGDoc::DrawUniteTrainMark(CDC*pCDC, TgTRAIN_RECORD *first)
{
	if(NULL == first)
		return FALSE;

	if(!IsFalgBeUnitedByOther(first->flag))
		return FALSE;

	do
	{
		CTG_TRAIN *pTrain = this->myPDataManage->GetTrain(first->special_train_index);

		if(pTrain == NULL) 
			break;
		TgTRAIN_RECORD *rec = pTrain->GetLastRecord();
		if(rec == NULL)
			break;

		if(!IsFlagUnitOtherTrain(rec->flag))
			break;

		CPoint fPoint,sPoint;
		if(first->in_sta_direct == 2)
		{
			fPoint.y = sPoint.y = GetStationVPos(first->out_station_index, BOTTOM_TRACK) + 4;
		}
		else
		{
			fPoint.y = sPoint.y =  GetStationVPos(first->out_station_index, TOP_TRACK) - 4;
		}

		fPoint.x = m_nFirstDepMinuteOffset * m_fPixelPerMinute;
		sPoint.x = GetXPositionConsiderSecond(rec->depart);
		CPen mPen,*pOldPen;
		mPen.CreatePen(PS_DOT,1,first->schd_color);
		pOldPen = pCDC->SelectObject(&mPen);
		pCDC->MoveTo(fPoint);
		pCDC->LineTo(sPoint);

		pCDC->SelectObject(pOldPen);
		mPen.DeleteObject();
		return TRUE; 
	}while(0); 

	return FALSE;
}

void CTGDoc::DrawAbsText(CDC*pCDC, CTG_TRAIN*single, TgTRAIN_RECORD *first, const CPoint& ptlStart, const CPoint& ptlEnd, bool bStopAbs)
{
	if(NULL == first)
		return;

	if(first->szAbsText.IsEmpty())
		return;

	CFont *pOldFont = pCDC->SelectObject(&fontText);	
	CSize sz = pCDC->GetTextExtent(first->szAbsText);
	COLORREF cl = pCDC->SetTextColor(RGB(255,0,0));
	if(this->m_ptgctrl->mConfigTg.m_context.IsHrbTransportColor())
		pCDC->SetTextColor(RGB(0,0,128));
	CRect rect;
	if(bStopAbs)
	{
		if(first->out_sta_direct == DOWN_DIRECT)
		{
			rect=CRect(ptlEnd.x-sz.cx/2, ptlEnd.y, ptlEnd.x+sz.cx/2, ptlEnd.y+sz.cy);
		}
		else
		{
			rect=CRect(ptlEnd.x-sz.cx/2, ptlEnd.y-sz.cy, ptlEnd.x+sz.cx/2, ptlEnd.y);
		}
	}
	else
	{
		int CX = (ptlStart.x + ptlEnd.x)/2;
		int CY = (ptlStart.y + ptlEnd.y)/2;
		rect=CRect(CX-sz.cx/2, CY-sz.cy/2, CX+sz.cx/2, CY+sz.cy/2);
	}
#ifdef DRAW_TRAIN_RECT	
	ShowRect(pCDC, rect);
#endif
	single->UnionRect(rect);

	UINT Aligh = pCDC->SetTextAlign(TA_TOP|TA_LEFT);
	pCDC->TextOut(rect.left, rect.top, first->szAbsText); // ok
	pCDC->SelectObject(pOldFont);	
	pCDC->SetTextAlign(Aligh);
	pCDC->SetTextColor(cl);

	return;
}

void CTGDoc::DrawAbsPartRunLine(CDC* pCDC, CTG_TRAIN* ptrain, TgTRAIN_RECORD* first, const CPoint& ptlStart)
{
	if(first==NULL || ptrain==NULL)
		return;

	CPoint fPoint=ptlStart,sPoint=ptlStart;
	sPoint.x = GetOffsetPixelFromGraphBegin(first->depart+first->stop_abs_s_time*60-ptrain->myTrainInfo.stop_abs_time*60);
			
	CPen mPen;
	if(first_d_time_status)
		mPen.CreatePen(PS_SOLID, first->real_width, first->real_color);
	else
		mPen.CreatePen(PS_SOLID, first->schd_width, first->schd_color);

	CPen *pOldPen = pCDC->SelectObject(&mPen);
	pCDC->MoveTo(fPoint); pCDC->LineTo(sPoint);

	pCDC->SelectObject(pOldPen);
	mPen.DeleteObject();
}

void CTGDoc::DrawEngineLine(CDC*pCDC, CTG_TRAIN*single, TgTRAIN_RECORD *first, const CPoint& ptlStart, const CPoint& ptlEnd)
{
	if(first==NULL)
		return;

	if(IsFlagAttainEngine(first->flag))
	{
		CPoint fPoint,sPoint;

		int dlt = 5;
		
		fPoint.x = ptlStart.x - dlt;
		fPoint.y = ptlStart.y;

		sPoint.x = (ptlStart.x + ptlEnd.x)/2 - dlt;
		sPoint.y = (ptlEnd.y + ptlStart.y)/2;
		
		CPen mPen;
		mPen.CreatePen(PS_DOT, 1, first->schd_color);

		CPen *pOldPen = pCDC->SelectObject(&mPen);
		pCDC->MoveTo(fPoint); pCDC->LineTo(sPoint);

		pCDC->SelectObject(pOldPen);
		mPen.DeleteObject();
	}
}

void CTGDoc::DrawUniteTrainAbsLine(CDC*pCDC, TgTRAIN_RECORD *first, const CPoint& ptlStart, const CPoint& ptlEnd)
{
	if(NULL == first)
		return;

	CPen mPen;
	CPoint fPoint,sPoint;

	int dlt = 0;
	if(first->in_sta_direct == DOWN_DIRECT)
		dlt = -4;
	else
		dlt = 4;

	fPoint.y = ptlStart.y;
	sPoint.y = ptlEnd.y;
	fPoint.x = ptlStart.x + dlt;
	sPoint.x = ptlEnd.x + dlt;

	if(first_d_time_status && second_a_time_status)
		mPen.CreatePen(PS_DOT, 1, first->real_color);
	else
		mPen.CreatePen(PS_DOT, 1, first->schd_color);

	CPen *pOldPen = pCDC->SelectObject(&mPen);
	pCDC->MoveTo(fPoint); pCDC->LineTo(sPoint);

	pCDC->SelectObject(pOldPen);
	mPen.DeleteObject();
}
