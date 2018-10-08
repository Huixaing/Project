#include "stdafx.h"
#include "math.h"
#include "TGForm.h"
#include "tgdoc.h"

void CTGDoc::DrawSingleTrain(CDC* pCDC, CTG_TRAIN* single,int nDrawMode,bool bShowSecond)
{
	if(!m_bAcceptCadData)
		return;

	if(NULL==single)
		return;

	if(m_ptgctrl->mConfigTg.m_context.IsSecretTrainNotVisible() && IsTrainSecret(single->myTrainInfo))
		return;

	if(!m_bShowSingleTrain)
	{
		if(m_ptgctrl->IsWorkMode())
		{
			if(!IsTrainShow(single))
			{
				if(GetShowTrainType() != TYPE::ALL)
				{
					if(single->m_nTrainType != GetShowTrainType())
						return;
				}
			}
		}
		else
		{
			if (IsOnlyShowMark()) 
			{
				return;
			}
			if(!IsTrainShow(single))
			{
				if(GetShowTrainType() != TYPE::ALL)
				{
					if(single->m_nTrainType != GetShowTrainType())
						return;
				}

				if(GetShowTrainDirect() != TRAIN_DIRECT::DUAL)
				{
					if(!single->mbDoubleDirect)
					{
						if((single->m_nDirectFlag+1) != GetShowTrainDirect())
							return;
					}
				}
			}
		}
	}
	// 优化效率,选中信息只能画在屏幕
	if(pCDC==m_pLayer1 || pCDC==m_pLayer2 || pCDC==m_pLayer3)  
	{
		nDrawMode=DRAW_NORMAL;
	}
	single->m_nUpdateTime=GetBeginTime();
	single->InitShowTrainIDRect();
	single->EmptyRect();

	const TgTRAIN_MORE_INFO& train_info = single->myTrainInfo;

	int usDrawCount = single->GetRecordNum();

	// prepare gdi resource
	int nPreviousMode = pCDC->SetROP2(R2_COPYPEN);
	int nOldBackMode = pCDC->SetBkMode(TRANSPARENT);
	int nOldAlign = pCDC->SetTextAlign(TA_RIGHT | TA_BOTTOM);    

	//该车线条的中间点个数
	single->m_nPointCount = 0;	
	// 用来判断挂机车的个数
 
	BOOL bDrawUniteTrainFlag = FALSE;	

	TgTRAIN_RECORD* first = NULL;
	TgTRAIN_RECORD* second = single->GetRecordByPosition(0);

	if((GetEarlyLateTime() == single->m_nTrainType) || GetEarlyLateTime() == 3)
	{
		if(single->myTrainInfo.whole_static<=0)
		   InitEarlyLastTime(single);
	}
	
	for(int nRecIndex=0; nRecIndex<usDrawCount; nRecIndex++)
	{
		//for actual time or plan time codeing
		if(this->GetTgType() == TG_TYPE::BASIC_TG)
		{
			// 基本图默认均为实际点
			first_a_time_status = true;second_a_time_status = true;
			first_d_time_status = true;second_d_time_status = true;
		}
		else
		{
			// 非基本图默认均为计划点
			first_a_time_status = false;second_a_time_status = false;
			first_d_time_status = false;second_d_time_status = false;
		}
		first = second;
		second = single->GetRecordByPosition(nRecIndex+1);
		if(NULL == first) 
			continue;

		m_nFirstArlMinuteOffset = GetOffsetMinuteFromGraphBegin(first->arrive);
		m_nFirstDepMinuteOffset = GetOffsetMinuteFromGraphBegin(first->depart);
		m_nFirstArlSecond=first->arrive%60;  // 记录秒
		m_nFirstDepSecond=first->depart%60; // 记录秒

		// 2012.5.2 列车到发时刻分钟相同即小于1分钟,仍需显示为到开
		if(m_nFirstArlMinuteOffset==m_nFirstDepMinuteOffset && first->arrive!=first->depart)
			m_nFirstDepMinuteOffset+=1;

		if(NULL != second)
		{
			m_nSecondArlMinuteOffset = GetOffsetMinuteFromGraphBegin(second->arrive);
			m_nSecondDepMinuteOffset = GetOffsetMinuteFromGraphBegin(second->depart);

			m_nSecondArlSecond=second->arrive%60; // 记录秒
			m_nSecondDepSecond=second->depart%60; // 记录秒
		}

		//for actual time or plan time codeing   
		if(this->GetTgType() != TG_TYPE::BASIC_TG)
		{
			// 非基本图按照实际情况设置
			if(first->adjust_status & 0x0001)
				first_a_time_status = true;//real time
			else
				first_a_time_status = false;// plan time 
			if(first->adjust_status & 0x0002)
				first_d_time_status = true;//real time
			else
				first_d_time_status = false;// plan time 
		
			if((nRecIndex+1 == usDrawCount) && IsFlagTerminal(first->flag))
			{
				first_d_time_status = first_a_time_status;
			}

			if((nRecIndex == 0) && IsFlagStart(first->flag))
			{
				first_a_time_status = first_d_time_status;
			}
		}
		if(second != NULL)
		{
			//for actual time or plan time codeing   
			if(this->GetTgType() != TG_TYPE::BASIC_TG)
			{
				// 非基本图按照实际情况设置
				if(second->adjust_status & 0x0001)//first bit data
					second_a_time_status = true;//real time
				else
					second_a_time_status = false;// plan time 
				if(second->adjust_status & 0x0002)//second bit data
					second_d_time_status = true;//real time
				else
					second_d_time_status = false;// plan time 
				if((nRecIndex+1 == usDrawCount) && IsFlagTerminal(second->flag))
					second_d_time_status = second_a_time_status;	
			}
			if(IsFlagStopAbs(first->flag))
			{
				CPoint ptEnd, ptStart;
				int x = m_nFirstDepMinuteOffset*m_fPixelPerMinute+(m_fPixelPerMinute*m_nFirstDepSecond)/60;
				ptEnd.x = x + first->stop_abs_s_time*m_fPixelPerMinute;
				ptStart.x = x + first->stop_abs_e_time*m_fPixelPerMinute;

				int tmp = 0;
				if(first->out_sta_direct == DOWN_DIRECT)
					tmp = UP_DIRECT;
				else
					tmp = DOWN_DIRECT;
				// 区间折返
				if(first->station == second->station && IsSetTrainFlag(train_info.adjust_flag))
				{
					int ptlStartY;
					if(first->out_sta_direct == DOWN_DIRECT)//down direct 
						ptlStartY = GetVPosition(first->out_station_index, POS_FLAG::DOWN);
					else
						ptlStartY = GetVPosition(first->out_station_index, POS_FLAG::UP);

					int absY = GetStationAbsPosition(single, first);
					int dlt = absY - ptlStartY;
					if(IsAbsDrawUp(first->stop_reason))
						ptEnd.y = ptlStartY + dlt/2;
					else if(IsAbsDrawBottom(first->stop_reason))
						ptEnd.y = ptlStartY + dlt/2 + dlt;
					else
						ptEnd.y = ptlStartY + dlt;
					
					ptStart.y = ptEnd.y;

					if(first_a_time_status)
						DrawMarkOfEnd(pCDC, single, ptEnd, tmp, first->real_color, 0, first->real_width);
					else
						DrawMarkOfEnd(pCDC, single, ptEnd, tmp, first->schd_color, 0, first->schd_width);

					CString trainid = second->arrive_train_id;
					if(second_a_time_status)
						DrawMarkOfStart(pCDC,single,trainid,ptStart,tmp,second->real_color,second->real_width,second);
					else
						DrawMarkOfStart(pCDC,single,trainid,ptStart,tmp,second->schd_color,second->schd_width,second);
					// 画区间分部运行线
					DrawAbsPartRunLine(pCDC,single,first,ptStart);
				}
			}
		}

		// draw train other trainid
		if(this->m_ptgctrl->mConfigTg.m_context.IsShowMutiTrainSeparate() && NULL != first)
		{
			DrawMutiTrainSeparate(pCDC, single, first);
		}
			
		//draw unite train mark
		if(IsFalgBeUnitedByOther(first->flag))
			bDrawUniteTrainFlag = DrawUniteTrainMark(pCDC, first);

		//draw Divide Train	and Combine Train
		if(IsFlagDivide(first->flag))
			DrawDivideOrCombineTrainMark(pCDC, first);
        
		// 绘制始发接入标志
		if(nRecIndex == 0)
		{
			DrawTrainStartOrAcceptMark(pCDC, single, first, train_info);
		}
		// 绘制交出终到标志
		if(nRecIndex == usDrawCount - 1)
		{
			DrawTrainEndOrDepartMark(pCDC, single, first);
		}

		// 绘制站内停车
		first->bShowSecond=bShowSecond; // 是否显示秒
		DrawStopLine(pCDC, single, first, nDrawMode);

		if(IsFlagStopAbs(first->flag) || IsFlagStopSignal(first->flag))
		{
			DrawAbsStopLine(pCDC, single, first, second);
		}

		// 绘制两站之间的连接线		
		CPoint ptlStart, ptlEnd;
		if(DrawAbsLine(pCDC, single, first, second, ptlStart, ptlEnd))
		{
			//draw unite train 
			if(bDrawUniteTrainFlag)
				DrawUniteTrainAbsLine(pCDC, first, ptlStart, ptlEnd);
			// 画补机途中折返线
			DrawEngineLine(pCDC, single, first, ptlStart, ptlEnd);

			// 输出区间信息
			DrawAbsText(pCDC, single, first, ptlStart, ptlEnd, false);
			
			DrawBadMark(pCDC, single, ptlStart, ptlEnd, first);

			if((first->no_meet_reason&0xffff)!=0)
				DrawNoMeetMark(pCDC, ptlStart, ptlEnd, first->no_meet_reason, first->schd_color, this->GetTrainShowId(single));

			// 区间运行分析
			if(m_bShowFastOrSlowTime)
			{
				// 区间运缓是否和基本图比较
				if(m_ptgctrl->mConfigTg.IsRunFastSlowCompareWithBase())
				{
					if(NULL != first && NULL != second)
					{
						int plan_run_time=second->plan_arrive-first->plan_depart;
						int run_time=second->arrive-first->depart;
						int fast_slow=run_time-plan_run_time;
						fast_slow = fast_slow/60;
						if(fast_slow>=0)
							DrawFastOrSlowTime(pCDC, first, ptlStart, ptlEnd, FALSE, fast_slow, first->in_sta_direct);
						else
							DrawFastOrSlowTime(pCDC, first, ptlStart, ptlEnd, TRUE, -1*fast_slow, first->in_sta_direct);
					}
				}
				else
				{
					DrawFastOrSlowTime(pCDC, first, ptlStart, ptlEnd, IsRunFast(first), GetFastMinute(first), first->in_sta_direct);
				}
			}
		}
	}//end for first to last code 

	// 绘制进路触发标志
	if(this->GetTgType() == TG_TYPE::WORK_TG)
		DrawTrainRouteOKMark(pCDC,single);

	//draw train every point rect 
	if(single->m_nPointCount > 0 &&	(nDrawMode & DRAW_SCREEN))
	{
		DrawTrainFocusRect(pCDC, single);
	}
	// restore gdi resource
	pCDC->SetROP2(nPreviousMode);
	pCDC->SetTextAlign(nOldAlign);
	pCDC->SetBkMode(nOldBackMode);

	DrawByPassFlag(pCDC,single);

	// 绘制行车股道
	DrawTrainWorkSide(pCDC,single,nDrawMode);

	// 绘制技术停点发车时机标记
	DrawTrainTechDepartMark(pCDC,single);

	// 绘制列车进路非自触符号
	DrawTrainTrigerMark(pCDC,single);

	// 绘制文本信息
	DrawTrainWorkText(pCDC,single);

	// 绘制重点事项
	if(m_bShowPlanNoteText)
	{
		DrawTrainNoteText(pCDC,single);
	}

	if(mDisplayUnknownText>0 && (train_info.type==255 || train_info.run_type==255))
	{
		DrawUnknowTrainFlag(pCDC, single);
	}
	// 更新列车矩形
	SetTrainRect(single);
	//for(int i=0; i<single->days; i++)
	//	ShowRect(pCDC, single->myPosition[i]);
}

void CTGDoc::DrawTrainFocusRect(CDC*pCDC, CTG_TRAIN* single)
{ 
	TgTRAIN_RECORD* first=single->GetFirstRecord();
	if(first==NULL)
		return;

	CPen myPen;
	myPen.CreatePen(PS_SOLID, first->schd_width, first->schd_color);
	CPen* oldPen = pCDC->SelectObject(&myPen);

	CPoint p[5];
	
	for (int m=0;m<single->m_nPointCount;m++)
	{
		CPoint pt = single->m_pPerPointInfo[m].point;
		CRect m_rect(CPoint(pt.x - 5,pt.y - 5), CSize(10,10));
	
		if(this->m_ptgctrl->mConfigTg.m_context.IsEnableReverseShowSelTrainRect())
		{
			if(m_pView->m_nFocusRectIndex != m)
				pCDC->FillRect(m_rect, &m_brushBlue);
			else
				pCDC->FillRect(m_rect, &m_brushBlack);
		}
		else
		{
			if(m_pView->m_nFocusRectIndex == m && m_pView->m_nFocusTrainIndex==single->m_nTrainIndex)
				pCDC->FillRect(m_rect, &m_brushBlue);
		}

		p[0]= m_rect.TopLeft();
		p[1].x = p[0].x+ m_rect.Width();
		p[1].y = p[0].y;
		p[2]= m_rect.BottomRight();

		p[3].x = p[0].x;
		p[3].y = p[2].y;
		p[4]=p[0];
		p[4].y--;

		pCDC->Polyline(p,5);
	}

	pCDC->SelectObject(oldPen);
}

// 下面处理车组连接线
void CTGDoc::DrawAllCheZuHaoLine(CDC * pCDC)
{
	if(!m_bShowAllCZHLine)
		return;
	for(UINT i=0; i<myChezuhao.m_TrainCheZuHaoNum; i++)
	{
		if(myChezuhao.m_TrainCheZuHao[i].czh!="")
		{
			DrawTrainConnectLine(&myChezuhao.m_TrainCheZuHao[i], pCDC);
		}
	}
}

int CTGDoc::GetStationSideVPos(const int index, const int side_no, int direct)
{
	if(index<0||index>=m_nDispDistanceNum)
		return 0;

	if(m_sDispDistance[index].show_sation_sep)
	{
		int num_of_side = sizeof(m_sDispDistance[index].side)/sizeof(m_sDispDistance[index].side[0]);
		
		for(int i=0; i < num_of_side && i < m_sDispDistance[index].number_of_side; i++)
		{
			int sideno = m_sDispDistance[index].side[i].side_number;
			if(sideno == -1)
				continue;

			if(sideno == side_no)
			{
				return m_sDispDistance[index].nSideYPos[sideno];
			}
		}	
	}

	if(direct==UP_DIRECT)
	{
		return m_sDispDistance[index].nStationYPos;
	}
	else
	{
		return m_sDispDistance[index].nSecondStationYPos;
	}
}

void CTGDoc::DrawTrainConnectLine(TRAIN_CHEZUHAO* pTrainsCZH, CDC* pCDC)
{
	if(pTrainsCZH==NULL)
		return;

	if(GetShowTrainType() == TYPE::FREIGHT)
	{
		return;
	}

	if(GetShowTrainDirect() != TRAIN_DIRECT::DUAL)
	{
		return;
	}

	if(this->GetTgType()==TG_TYPE::BASIC_TG)
	{
		DrawBasicTrainConnectLine(pTrainsCZH, pCDC);
		return;
	}

	int linewidth=2;
	COLORREF clr=RGB(255,0,255);
	this->m_ptgctrl->mConfigTg.m_context.GetCZHLineCfg(linewidth, clr);

	int left=0;
	int top=0;
	int right=0;
	int bottom=0;
	int y1=0,y2=0;
	bool bFirst=true;
	CPoint ptlStart,ptlEnd;

	CPen mThinPen, *pOldPen;
	mThinPen.CreatePen(PS_SOLID, linewidth, clr);//255,0,255

	pOldPen = pCDC->SelectObject(&mThinPen);
	int nOldBackMode = pCDC->SetBkMode(TRANSPARENT);

	std::map<ULONG, TRAIN_CZH_INFO>::iterator it, next_it;
	
	it = pTrainsCZH->trains.begin();
	for(; it!=pTrainsCZH->trains.end(); it++)
	{
		next_it = it;
		next_it++;
		if(next_it==pTrainsCZH->trains.end())
			break;

		if(it->second.end_station == next_it->second.begin_station)
		{
			CTG_TRAIN* pPreTrain=this->myPDataManage->GetTrain(it->second.train_index);
			if(pPreTrain==NULL)
				continue;

			CTG_TRAIN* pNextTrain=this->myPDataManage->GetTrain(next_it->second.train_index);
			if(pNextTrain==NULL)
				continue;

			TgTRAIN_RECORD* last=pPreTrain->GetLastRecord();
			TgTRAIN_RECORD* first=pNextTrain->GetFirstRecord();
			if(last==NULL || first==NULL)
				continue;

			if(IsFlagTerminal(last->flag) && IsFlagStart(first->flag))
			{
				y1=last->in_station_index;
				y2=first->out_station_index;
				
				ptlEnd.x =GetXPositionConsiderSecond(last->arrive);
				ptlStart.x =GetXPositionConsiderSecond(first->depart);

				ptlEnd.y = this->GetStationSideVPos(y1, last->arrive_side, last->out_sta_direct);
				ptlStart.y   = this->GetStationSideVPos(y2, first->depart_side, last->out_sta_direct);

				int in_index=y1;
				int out_index=y2;
				if(out_index==in_index && in_index>=0)
				{
					if(m_sDispDistance[in_index].m_nCzhPrintFlag!=2)  // 动车库,不会连接线
					{
						pCDC->MoveTo(ptlEnd);
						pCDC->LineTo(ptlStart);
					}
				}
				else if(in_index>=0 && out_index>=0 && in_index<m_nDispDistanceNum && out_index<m_nDispDistanceNum)
				{
					CPoint ptMiddle, ptTrainId;
					if(m_sDispDistance[in_index].show_sation_sep)
					{
						ptMiddle.x=ptlStart.x;
						ptMiddle.y=ptlEnd.y;
						ptTrainId=ptMiddle;
					}
					else if(m_sDispDistance[out_index].show_sation_sep)
					{
						ptMiddle.x=ptlEnd.x;
						ptMiddle.y=ptlStart.y;
						ptTrainId.x=ptlStart.x;
						ptTrainId.y=ptlStart.y;
					}
					else if(m_sDispDistance[out_index].m_nCzhPrintFlag==1)
					{
						ptMiddle.x=ptlEnd.x;
						ptMiddle.y=ptlStart.y;
						ptTrainId.x=ptlStart.x;
						ptTrainId.y=ptlStart.y;
					}
					else if(m_sDispDistance[in_index].m_nCzhPrintFlag==1)
					{
						ptMiddle.x=ptlStart.x;
						ptMiddle.y=ptlEnd.y;
						ptTrainId=ptMiddle;
					}
					else
					{
						ptMiddle.x=ptlStart.x;
						ptMiddle.y=ptlEnd.y;
						ptTrainId=ptMiddle;
					}

					pCDC->MoveTo(ptlEnd);
					pCDC->LineTo(ptMiddle);
					pCDC->LineTo(ptlStart);
				}
				
				if(ptlEnd.x<=ptlStart.x)
				{
					left=ptlEnd.x-5;
					right=ptlStart.x+5;
				}
				else
				{
					left=ptlStart.x-5;
					right=ptlEnd.x+5;
				}

				if(ptlEnd.y<=ptlStart.y)
				{
					top=ptlEnd.y-5;
					bottom=ptlStart.y+5;
				}
				else
				{
					top=ptlStart.y-5;
					bottom=ptlEnd.y+5;
				}
				if(left<0) left=0;
				if(top<0) top=0;

				if(bFirst)
				{
					pTrainsCZH->rect.left=left;
					pTrainsCZH->rect.right=right;
					pTrainsCZH->rect.top=top;
					pTrainsCZH->rect.bottom=bottom;
					bFirst=false;
				}
				else
				{
					pTrainsCZH->SetRect(left, top, right, bottom);
				}
			}
		}
	}

	pCDC->SelectObject(pOldPen);
	pCDC->SetBkMode(nOldBackMode);
	mThinPen.DeleteObject();

	return;
}

void CTGDoc::DrawBasicTrainConnectLine(TRAIN_CHEZUHAO* pTrainsCZH, CDC* pCDC)
{
	if(pTrainsCZH==NULL)
		return;

	int linewidth=2;
	COLORREF clr=RGB(255,0,0);

	CPen mThinPen, *pOldPen;
	mThinPen.CreatePen(PS_SOLID, linewidth, clr);
	pOldPen = pCDC->SelectObject(&mThinPen);
	int nOldBackMode = pCDC->SetBkMode(TRANSPARENT);

	std::map<ULONG, TRAIN_CZH_INFO>::iterator it, next_it;
	
	it = pTrainsCZH->trains.begin();
	for(; it!=pTrainsCZH->trains.end(); it++)
	{
		next_it = it;
		next_it++;
		if(next_it==pTrainsCZH->trains.end())
		{
			if(this->m_ptgctrl->mConfigTg.m_context.IsEnableCZH())
			{
				next_it = pTrainsCZH->trains.begin();
				if(next_it!=pTrainsCZH->trains.end())
					DrawBasicTrainConnectLine(it, next_it, pTrainsCZH, pCDC);
			}
			else if(m_ptgctrl->mConfigTg.m_context.IsEnableNewCZH())
			{
				;//对于新车组列车前后关系明确,不能尾连首
			}
			break;
		}

		DrawBasicTrainConnectLine(it, next_it, pTrainsCZH, pCDC);
	}

	pCDC->SelectObject(pOldPen);
	pCDC->SetBkMode(nOldBackMode);
	mThinPen.DeleteObject();

	return;
}

void CTGDoc::DrawBasicTrainConnectLine(std::map<ULONG, TRAIN_CZH_INFO>::iterator& it, std::map<ULONG, TRAIN_CZH_INFO>::iterator& next_it, TRAIN_CHEZUHAO* pTrainsCZH, CDC* pCDC)
{
	if(pTrainsCZH==NULL)
		return;

	int left=0;
	int top=0;
	int right=0;
	int bottom=0;
	int y1=0,y2=0;
	CPoint ptlStart,ptlEnd;

	int nWholeDayPixel = this->m_fPixelPerHour*24;

	if(it->second.end_station == next_it->second.begin_station)
	{
		CTG_TRAIN* pPreTrain=this->myPDataManage->GetTrain(it->second.train_index);
		if(pPreTrain==NULL)
			return;

		CTG_TRAIN* pNextTrain=this->myPDataManage->GetTrain(next_it->second.train_index);
		if(pNextTrain==NULL)
			return;

		TgTRAIN_RECORD* last=pPreTrain->GetLastRecord();
		TgTRAIN_RECORD* first=pNextTrain->GetFirstRecord();
		if(last==NULL || first==NULL)
			return;

		if(IsFlagTerminal(last->flag) && IsFlagStart(first->flag))
		{
			y1=last->in_station_index;
			y2=first->out_station_index;
			
			ptlEnd.x = GetXPositionConsiderSecond(last->arrive);
			ptlStart.x = GetXPositionConsiderSecond(first->depart);

			ptlEnd.y = this->GetStationSideVPos(y1, last->arrive_side, last->out_sta_direct);
			ptlStart.y   = this->GetStationSideVPos(y2, first->depart_side, last->out_sta_direct);

			CPoint ptTmpStart(ptlStart), ptTmpEnd(ptlEnd);
			ptTmpStart.x = ptTmpStart.x%nWholeDayPixel;
			ptTmpEnd.x = ptTmpEnd.x%nWholeDayPixel;

			int in_index=y1;
			int out_index=y2;
			if((out_index==in_index) && (in_index>=0))
			{
				if(m_sDispDistance[in_index].m_nCzhPrintFlag!=2)  // 动车库,不会连接线
				{
					if(ptTmpEnd.x <= ptTmpStart.x)
					{
						pCDC->MoveTo(ptTmpEnd);
						pCDC->LineTo(ptTmpStart);

						if(ptTmpEnd.x<=ptTmpStart.x)
						{
							left=ptTmpEnd.x-5;
							right=ptTmpStart.x+5;
						}
						else
						{
							left=ptTmpStart.x-5;
							right=ptTmpEnd.x+5;
						}

						if(ptTmpEnd.y<=ptTmpStart.y)
						{
							top=ptTmpEnd.y-5;
							bottom=ptTmpStart.y+5;
						}
						else
						{
							top=ptTmpStart.y-5;
							bottom=ptTmpEnd.y+5;
						}
						if(left<0) left=0;
						if(top<0) top=0;

						pTrainsCZH->SetRect(left, top, right, bottom);
					}
					else
					{
						pCDC->MoveTo(0, ptTmpStart.y);
						pCDC->LineTo(ptTmpStart);

						pCDC->MoveTo(nWholeDayPixel, ptTmpEnd.y);
						pCDC->LineTo(ptTmpEnd);
						

						if(ptTmpEnd.y<=ptTmpStart.y)
						{
							top=ptTmpEnd.y-5;
							bottom=ptTmpStart.y+5;
						}
						else
						{
							top=ptTmpStart.y-5;
							bottom=ptTmpEnd.y+5;
						}
						left=0;
						right=nWholeDayPixel;

						pTrainsCZH->SetRect(left, top, right, bottom);
					}
				}
			}
			else if(in_index>=0 && out_index>=0 && in_index<m_nDispDistanceNum && out_index<m_nDispDistanceNum)
			{
				if(ptTmpEnd.x <= ptTmpStart.x)
				{
					CPoint ptMiddle, ptTrainId;
					if(m_sDispDistance[in_index].show_sation_sep)
					{
						ptMiddle.x=ptlStart.x;
						ptMiddle.y=ptlEnd.y;
						ptTrainId=ptMiddle;
					}
					else if(m_sDispDistance[out_index].show_sation_sep)
					{
						ptMiddle.x=ptlEnd.x;
						ptMiddle.y=ptlStart.y;
						ptTrainId.x=ptlStart.x;
						ptTrainId.y=ptlStart.y;
					}
					else if(m_sDispDistance[out_index].m_nCzhPrintFlag==1)
					{
						ptMiddle.x=ptlEnd.x;
						ptMiddle.y=ptlStart.y;
						ptTrainId.x=ptlStart.x;
						ptTrainId.y=ptlStart.y;
					}
					else if(m_sDispDistance[in_index].m_nCzhPrintFlag==1)
					{
						ptMiddle.x=ptlStart.x;
						ptMiddle.y=ptlEnd.y;
						ptTrainId=ptMiddle;
					}
					else
					{
						ptMiddle.x=ptlStart.x;
						ptMiddle.y=ptlEnd.y;
						ptTrainId=ptMiddle;
					}

					pCDC->MoveTo(ptlEnd);
					pCDC->LineTo(ptMiddle);
					pCDC->LineTo(ptlStart);

					if(ptTmpEnd.x<=ptTmpStart.x)
					{
						left=ptTmpEnd.x-5;
						right=ptTmpStart.x+5;
					}
					else
					{
						left=ptTmpStart.x-5;
						right=ptTmpEnd.x+5;
					}

					if(ptTmpEnd.y<=ptTmpStart.y)
					{
						top=ptTmpEnd.y-5;
						bottom=ptTmpStart.y+5;
					}
					else
					{
						top=ptTmpStart.y-5;
						bottom=ptTmpEnd.y+5;
					}
					if(left<0) left=0;
					if(top<0) top=0;

					pTrainsCZH->SetRect(left, top, right, bottom);
				}
				else
				{
					CPoint ptMiddle, ptTrainId;
					if(m_sDispDistance[in_index].show_sation_sep)
					{
						ptMiddle.x=ptlStart.x;
						ptMiddle.y=ptlEnd.y;
						ptTrainId=ptMiddle;

						pCDC->MoveTo(ptlEnd);
						pCDC->LineTo(nWholeDayPixel, ptlEnd.y);

						pCDC->MoveTo(ptlStart);
						pCDC->LineTo(ptMiddle); pCDC->LineTo(0, ptMiddle.y);
					}
					else if(m_sDispDistance[out_index].show_sation_sep)
					{
						ptMiddle.x=ptlEnd.x;
						ptMiddle.y=ptlStart.y;
						ptTrainId=ptMiddle;
						
						pCDC->MoveTo(ptlEnd);
						pCDC->LineTo(ptMiddle); pCDC->LineTo(nWholeDayPixel, ptMiddle.y);
						pCDC->MoveTo(ptlStart);
						pCDC->LineTo(0, ptlStart.y);
					}
					else if(m_sDispDistance[out_index].m_nCzhPrintFlag==1)
					{
						ptMiddle.x=ptlEnd.x;
						ptMiddle.y=ptlStart.y;
						ptTrainId=ptMiddle;
						
						pCDC->MoveTo(ptlEnd);
						pCDC->LineTo(ptMiddle); pCDC->LineTo(nWholeDayPixel, ptMiddle.y);
						pCDC->MoveTo(ptlStart);
						pCDC->LineTo(0, ptlStart.y);
					}
					else if(m_sDispDistance[in_index].m_nCzhPrintFlag==1)
					{
						ptMiddle.x=ptlStart.x;
						ptMiddle.y=ptlEnd.y;
						ptTrainId=ptMiddle;

						pCDC->MoveTo(ptlEnd);
						pCDC->LineTo(nWholeDayPixel, ptlEnd.y);

						pCDC->MoveTo(ptlStart);
						pCDC->LineTo(ptMiddle); pCDC->LineTo(0, ptMiddle.y);
					}
					else
					{
						ptMiddle.x=ptlStart.x;
						ptMiddle.y=ptlEnd.y;
						ptTrainId=ptMiddle;

						pCDC->MoveTo(ptlEnd);
						pCDC->LineTo(nWholeDayPixel, ptlEnd.y);

						pCDC->MoveTo(ptlStart);
						pCDC->LineTo(ptMiddle); pCDC->LineTo(0, ptMiddle.y);
					}
					
					if(ptTmpEnd.y<=ptTmpStart.y)
					{
						top=ptTmpEnd.y-5;
						bottom=ptTmpStart.y+5;
					}
					else
					{
						top=ptTmpStart.y-5;
						bottom=ptTmpEnd.y+5;
					}
					left=0;
					right=nWholeDayPixel;

					pTrainsCZH->SetRect(left, top, right, bottom);
				}
			}
		}
	}
	return;
}
