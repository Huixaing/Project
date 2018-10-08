#include "stdafx.h"
#include "math.h"
#include "tgdoc.h"
#include "tgForm.h"
//#define IsArriveActualByManual(rec) (!((rec).adjust_status & 0x0400) && ((rec).adjust_status & 0x0001))
//#define IsArriveActualByAuto(rec)   (((rec).adjust_status & 0x0400) && ((rec).adjust_status & 0x0001))
//#define IsDepartActualByManual(rec) (!((rec).adjust_status & 0x0800) && ((rec).adjust_status & 0x0002))
//#define IsDepartActualByAuto(rec)   (((rec).adjust_status & 0x0800) && ((rec).adjust_status & 0x0002))

/* ----------------------------------------------- */
//
//   该文件用来完成到发点的绘制
//
/* ----------------------------------------------- */
// 按照到发点位置,获取底图背景颜色
COLORREF CTGDoc::GetSecondBkColor(LONG x)
{
	BYTE InitColor=m_pView->m_InitColor;
	COLORREF clr=RGB(255-InitColor,255-InitColor,255-InitColor);
	if(GetTgType() == TG_TYPE::BASIC_TG)
	{
		clr=RGB(255-InitColor,255-InitColor,255-InitColor);
	}
	else if(x>0)
	{
		USHORT nBlock=m_nBlockNum;
		if(m_ptgctrl->IsNewMode())
		{
			int uStartX = GetHistoryRegionWidth() * m_fPixelPerMinute;
			if(nBlock == BACKGROUD::BLOCK::FIVE)
			{	
				int uNear = uStartX + 10*m_fPixelPerMinute;
				int uPlan = uStartX + m_fPixelPerHour*3;
				if(0<x && x<uStartX)
				{
					clr=RGB(255-InitColor-30,255-InitColor-30,255-InitColor-30);   // 实际区
				}
				else if(uStartX<x && x<uNear)
				{
					clr=RGB(255-InitColor-20,255-InitColor-20,255-InitColor-20);   // 临近区
				}
				else if(uNear<x && x<uPlan)
				{
					clr=RGB(255-InitColor-10,255-InitColor-10,255-InitColor-10);   // 阶段计划区
				}
				else
				{
					clr=RGB(255-InitColor,255-InitColor,255-InitColor);    // 计划区
				}
			}
			else if(nBlock == BACKGROUD::BLOCK::TWO)
			{
				if(0<x && x<uStartX)
				{
					clr=RGB(255-InitColor-40,255-InitColor-40,255-InitColor-40);
				}
				else
				{
					clr=RGB(255-InitColor,255-InitColor,255-InitColor);
				}
			}
			else
			{
				clr=RGB(255-InitColor,255-InitColor,255-InitColor);
			}
		}
		else
		{
			int nTmp = 12*m_fPixelPerHour;
			if(nBlock == BACKGROUD::BLOCK::FIVE)
			{
				int uStartX = GetHistoryRegionWidth() * m_fPixelPerMinute;
				int uNear = uStartX + 10*m_fPixelPerMinute;
				int uPlan = uStartX + m_fPixelPerHour*3;
				if(0<x && x<nTmp)
				{
					clr=RGB(255-InitColor-40,255-InitColor-40,255-InitColor-40);    // 历史区
				}
				else if(nTmp<x && x<uStartX)
				{
					clr=RGB(255-InitColor-30,255-InitColor-30,255-InitColor-30);   // 实际区
				}
				else if(uStartX<x && x<uNear)
				{
					clr=RGB(255-InitColor-20,255-InitColor-20,255-InitColor-20);   // 临近区
				}
				else if(uNear<x && x<uPlan)
				{
					clr=RGB(255-InitColor-10,255-InitColor-10,255-InitColor-10);   // 阶段计划区
				}
				else
				{
					clr=RGB(255-InitColor,255-InitColor,255-InitColor);    // 计划区
				}
			}
			else if(nBlock == BACKGROUD::BLOCK::TWO)
			{
				if(0<x && x<nTmp)
				{
					clr=RGB(255-InitColor-40,255-InitColor-40,255-InitColor-40);
				}
				else
				{
					clr=RGB(255-InitColor,255-InitColor,255-InitColor);
				}
			}
			else
			{
				clr=RGB(255-InitColor,255-InitColor,255-InitColor);
			}
		}
	}

	return clr;
}

void CTGDoc::DrawArrivalTime(CDC* pCDC,  TgTRAIN_RECORD* rec,  CPoint point,  int lColor, int lActualColor,int nDrawMode, int flag)
{
	if(NULL == rec)
		return;

	CFont* pOldFont = NULL;
	if(nDrawMode & DRAW_SCREEN)
		pOldFont = pCDC->SelectObject(&fontADTimeBold);
	else
		pOldFont = pCDC->SelectObject(&fontADTime);

	CString strSecond="";
	CString a_time_char = this->GetTimeString(rec->arrive, strSecond, rec->bShowSecond);
	CSize sz=pCDC->GetTextExtent(a_time_char);

	CSize szSecond(0,0);
	if(strSecond!="")
	{	
		CFont* pOldFt = pCDC->SelectObject(&fontSecondTime);
		szSecond=pCDC->GetTextExtent(strSecond);
		pCDC->SelectObject(pOldFt);
	}

	CPoint pt=point, pointOrg=point;
	UINT oldTextAlign = pCDC->SetTextAlign(TA_LEFT|TA_BOTTOM);	
	if(rec->arrive == rec->depart)
	{
		pt=pointOrg;
		point=pointOrg;
		if(rec->in_sta_direct == DOWN_DIRECT)
		{
			pCDC->SetTextAlign(TA_RIGHT|TA_TOP);
			point.x=pointOrg.x-szSecond.cx;
		}
		else
		{
			pCDC->SetTextAlign(TA_RIGHT|TA_BOTTOM);
			pt.y = pointOrg.y-(sz.cy-szSecond.cy);
			point.x=pointOrg.x-szSecond.cx;
		}
	}
	else
	{
		pt=pointOrg;
		point=pointOrg;
		if(flag==0)
		{
			if(rec->in_sta_direct == DOWN_DIRECT)
			{
				pCDC->SetTextAlign(TA_LEFT|TA_BOTTOM);
				pt.x=pointOrg.x+sz.cx;
				pt.y=pointOrg.y-(sz.cy-szSecond.cy);
			}
			else
			{
				pCDC->SetTextAlign(TA_LEFT|TA_TOP);
				pt.x=pointOrg.x+sz.cx;
				pt.y=pointOrg.y;
			}
		}
		else if(flag==2)
		{
			if(rec->in_sta_direct == DOWN_DIRECT)
			{
				pCDC->SetTextAlign(TA_RIGHT|TA_TOP);
				point.x=pointOrg.x-szSecond.cx;
			}
			else
			{
				pCDC->SetTextAlign(TA_RIGHT|TA_BOTTOM);
				pt.y = pointOrg.y-(sz.cy-szSecond.cy);
				point.x=pointOrg.x-szSecond.cx;
			}
		}
		else
		{
			if(rec->in_sta_direct == rec->out_sta_direct)
			{
				if(rec->in_sta_direct == DOWN_DIRECT)
				{
					pCDC->SetTextAlign(TA_LEFT|TA_BOTTOM);
					pt.x=pointOrg.x+sz.cx;
					pt.y=pointOrg.y-(sz.cy-szSecond.cy);
				}
				else
				{
					pCDC->SetTextAlign(TA_LEFT|TA_TOP);
					pt.x=pointOrg.x+sz.cx;
					pt.y=pointOrg.y;
				}
			}
			else
			{
				if(rec->out_sta_direct == DOWN_DIRECT)
				{
					pCDC->SetTextAlign(TA_LEFT|TA_BOTTOM);
					pt.x=pointOrg.x+sz.cx;
					pt.y=pointOrg.y-(sz.cy-szSecond.cy);
				}
				else
				{
					pCDC->SetTextAlign(TA_LEFT|TA_TOP);
					pt.x=pointOrg.x+sz.cx;
					pt.y=pointOrg.y;
				}
			}
		}
	}
		
	if(IsFlagTerminal(rec->flag))
	{
		pt=pointOrg;
		point=pointOrg;
		if(rec->in_sta_direct == DOWN_DIRECT)
		{
			pCDC->SetTextAlign(TA_LEFT|TA_BOTTOM);
			pt.x=pointOrg.x+sz.cx;
			pt.y=pointOrg.y-(sz.cy-szSecond.cy);
		}
		else
		{
			pCDC->SetTextAlign(TA_LEFT|TA_TOP);
			pt.x=pointOrg.x+sz.cx;
			pt.y=pointOrg.y;
		}
	}
	
    COLORREF oldColor = pCDC->SetTextColor(lColor);
	if(IsFlagStart(rec->flag) && rec->rec_index==0)
	{
		if(IsActualDepartTime(rec->adjust_status))
		{
			SetActualArrivalTime(rec->adjust_status);
			if(IsDepartActualByManual(*rec))
			   SetArriveActualByManual(*rec);
		}
	}

	if(m_nManualRealColor>0 && IsArriveActualByManual(*rec))
	{
		pCDC->SetTextColor(m_nManualRealColor);
	}
	else
	{
		if(IsActualArrivalTime(rec->adjust_status) || (this->GetTgType() == TG_TYPE::BASIC_TG))
			pCDC->SetTextColor(lActualColor);
		else
			pCDC->SetTextColor(lColor);
	}
	if(m_nADTimeSystle == 3)
	{
		if(strSecond!="")
		{
			int nOldBackMode = pCDC->SetBkMode(OPAQUE);
			COLORREF clr=pCDC->SetBkColor(GetSecondBkColor(pt.x));

			pCDC->TextOut(pointOrg.x, pointOrg.y, " "); // 用空字符覆盖原有到点--分
			pCDC->TextOut(point.x, point.y, a_time_char);

			pCDC->SelectObject(&fontSecondTime);
			pCDC->TextOut(pt.x, pt.y, strSecond);

			pCDC->SetBkColor(clr);
			pCDC->SetBkMode(nOldBackMode);
		}
		else
		{
			pCDC->TextOut(point.x, point.y, a_time_char);
		}
	}
	else
	{
		pCDC->TextOut(point.x, point.y, a_time_char);
		if(strSecond!="")
		{
			pCDC->SelectObject(&fontSecondTime);
			pCDC->TextOut(pt.x, pt.y, strSecond);
		}
	}

	pCDC->SelectObject(pOldFont);
	pCDC->SetTextAlign(oldTextAlign);
	pCDC->SetTextColor(oldColor);
	return;
}

void CTGDoc::DrawDepartTime(CDC* pCDC,  TgTRAIN_RECORD* rec,  CPoint point,  int lColor, int lActualColor,int nDrawMode, int flag)
{
	if(NULL == rec)
		return;

	CFont* pOldFont = NULL;
	if(nDrawMode & DRAW_SCREEN)
		pOldFont = pCDC->SelectObject(&fontADTimeBold);
	else
		pOldFont = pCDC->SelectObject(&fontADTime);

	CString strSecond="";
	CString d_time_char = this->GetTimeString(rec->depart, strSecond, rec->bShowSecond);
	CSize sz=pCDC->GetTextExtent(d_time_char);
	
	CSize szSecond(0,0);
	if(strSecond!="")
	{	
		CFont* pOldFt = pCDC->SelectObject(&fontSecondTime);
		szSecond=pCDC->GetTextExtent(strSecond);
		pCDC->SelectObject(pOldFt);
	}

	CPoint pt=point, pointOrg=point;
	UINT oldTextAlign = pCDC->SetTextAlign(TA_RIGHT|TA_TOP);
	if(rec->in_sta_direct == DOWN_DIRECT)
	{
		pCDC->SetTextAlign(TA_RIGHT|TA_TOP);
		point.x=pointOrg.x-szSecond.cx;
	}
	else
	{
		pCDC->SetTextAlign(TA_RIGHT|TA_BOTTOM);
		pt.y = pointOrg.y-(sz.cy-szSecond.cy);
		point.x=pointOrg.x-szSecond.cx;
	}
	if(flag==0)
	{
		pt=pointOrg;
		point=pointOrg;
		if(rec->in_sta_direct == DOWN_DIRECT)
		{
			pCDC->SetTextAlign(TA_RIGHT|TA_TOP);
			point.x=pointOrg.x-szSecond.cx;
		}
		else
		{
			pCDC->SetTextAlign(TA_RIGHT|TA_BOTTOM);
			pt.y = pointOrg.y-(sz.cy-szSecond.cy);
			point.x=pointOrg.x-szSecond.cx;
		}
	}
	else
	{
		pt=pointOrg;
		point=pointOrg;
		if(rec->in_sta_direct == rec->out_sta_direct)
		{
			if(rec->in_sta_direct == DOWN_DIRECT)
			{
				pCDC->SetTextAlign(TA_RIGHT|TA_TOP);
				point.x=pointOrg.x-szSecond.cx;
			}
			else
			{
				pCDC->SetTextAlign(TA_RIGHT|TA_BOTTOM);
				pt.y = pointOrg.y-(sz.cy-szSecond.cy);
				point.x=pointOrg.x-szSecond.cx;
			}
		}
		else
		{
			if(rec->out_sta_direct == DOWN_DIRECT)
			{
				pCDC->SetTextAlign(TA_RIGHT|TA_TOP);
				point.x=pointOrg.x-szSecond.cx;
			}
			else
			{
				pCDC->SetTextAlign(TA_RIGHT|TA_BOTTOM);
				pt.y = pointOrg.y-(sz.cy-szSecond.cy);
				point.x=pointOrg.x-szSecond.cx;
			}
		}
	}

    COLORREF oldColor = pCDC->SetTextColor(lColor);
	if(m_nManualRealColor>0 && IsDepartActualByManual(*rec))
	{
		pCDC->SetTextColor(m_nManualRealColor);
	}
	else
	{
		if(IsActualDepartTime(rec->adjust_status) || (this->GetTgType() == TG_TYPE::BASIC_TG))
			pCDC->SetTextColor(lActualColor);
		else
			pCDC->SetTextColor(lColor);
	}

	if(m_nADTimeSystle == 3)
	{
		if(strSecond!="")
		{
			int nOldBackMode = pCDC->SetBkMode(OPAQUE);
			COLORREF clr=pCDC->SetBkColor(GetSecondBkColor(pt.x));
			
			pCDC->TextOut(pointOrg.x, pointOrg.y, " "); // 用空字符覆盖原有发点--分
			pCDC->TextOut(point.x, point.y, d_time_char);
						
			pCDC->SelectObject(&fontSecondTime);
			pCDC->TextOut(pt.x, pt.y, strSecond);

			pCDC->SetBkColor(clr);
			pCDC->SetBkMode(nOldBackMode);
		}
		else
		{
			pCDC->TextOut(point.x, point.y, d_time_char);
		}
	}
	else
	{
		pCDC->TextOut(point.x, point.y, d_time_char);
		if(strSecond!="")
		{
			pCDC->SelectObject(&fontSecondTime);
			pCDC->TextOut(pt.x, pt.y, strSecond);
		}
	}
	pCDC->SelectObject(pOldFont);
	pCDC->SetTextAlign(oldTextAlign);
	pCDC->SetTextColor(oldColor);
	return;
}

void CTGDoc::SetBasicTrainRect(CTG_TRAIN* ptrain)
{
	if(ptrain==NULL)
		return;
	UINT RecordNum = ptrain->GetRecordNum();
	if(RecordNum < 1)
		return;

	// -----------------------------------------------------------
	for(int i = 0; i < MAX_DAY_OF_BASIC_TRAIN; i++)
		ptrain->myPosition[i].SetRectEmpty();

	ptrain->days = 0;

	
	SetBasicTrainDate(ptrain);

	int mix_index=999, max_index=0;
	for(UINT idx = 0; idx < RecordNum; idx++)
	{
		TgTRAIN_RECORD* curr = ptrain->GetRecordByPosition(idx);
		if(curr == NULL)
			continue;

		if(curr->in_station_index >=0)
		{
			if(curr->in_station_index > max_index)
				max_index = curr->in_station_index;

			if(curr->in_station_index < mix_index)
				mix_index = curr->in_station_index;
		}

		if(curr->out_station_index >=0)
		{
			if(curr->out_station_index > max_index)
				max_index = curr->out_station_index;

			if(curr->out_station_index < mix_index)
				mix_index = curr->out_station_index;
		}
	}

	int top=0,bottom=0;
	if(mix_index < this->m_nDispDistanceNum && max_index < this->m_nDispDistanceNum)
	{
		top = this->m_sDispDistance[mix_index].nStationYPos - 150;
		bottom = this->m_sDispDistance[max_index].nSecondStationYPos + 150;
	}
	else
	{
		top = this->m_sDispDistance[0].nStationYPos - 150;
		bottom = this->m_sDispDistance[this->m_nDispDistanceNum-1].nSecondStationYPos + 150;
	}
		
	int left = 9999999,right=0;
	int arl=0, dep=0;
	for(UINT idx = 0; idx < RecordNum; idx++)
	{
		TgTRAIN_RECORD* curr = ptrain->GetRecordByPosition(idx);
		if(curr == NULL)
			continue;
		
		if(idx == 0)
		{
			if(IsFlagAbsStart(curr->flag))
			{
				arl = this->GetOffsetPixelFromGraphBegin(curr->arrive);
				arl = arl - ptrain->myTrainInfo.stop_abs_time*m_fPixelPerMinute;
				dep = arl;
			}
			else if(IsFlagStart(curr->flag))
			{
				dep = this->GetOffsetPixelFromGraphBegin(curr->depart);
				arl = dep;
			}
			else
			{
				arl = this->GetOffsetPixelFromGraphBegin(curr->arrive);
				if(IsFlagGiveout(curr->flag))
					dep = this->GetOffsetPixelFromGraphBegin(curr->depart);
				else
					dep = arl;
			}
		}
		if(idx == RecordNum-1)
		{
			arl = this->GetOffsetPixelFromGraphBegin(curr->arrive);
			dep = this->GetOffsetPixelFromGraphBegin(curr->depart);

			if(IsFlagAbsTerminal(curr->flag))
				dep = dep + curr->stop_abs_s_time*m_fPixelPerMinute;
			else if(IsFlagGiveout(curr->flag))
				dep = this->GetOffsetPixelFromGraphBegin(curr->depart);
			else
				dep = arl;
		}
		if(idx > 0 && idx < RecordNum-1)
		{
			arl = this->GetOffsetPixelFromGraphBegin(curr->arrive);
			dep = this->GetOffsetPixelFromGraphBegin(curr->depart);
		}

		if(arl < left)
			left = arl;
		if(dep > right)
			right = dep;
	}

	left -= 120;
	right += 80;

	int nWholePixel=24*this->m_fPixelPerHour;
	if(top<0) top=0;
	if(left<0)	left=0;
	if(right>nWholePixel) right=nWholePixel;
	if(bottom>m_nPageHeight) bottom=m_nPageHeight;

	ptrain->days=1;

	TgTRAIN_RECORD* first = ptrain->GetFirstRecord();
	TgTRAIN_RECORD* last  = ptrain->GetLastRecord();
	if(first!=NULL && last!=NULL && first!=last)
	{
		long nleft=0,nright=0,dlt;
		if(IsFlagStart(first->flag))
		{
			dlt=first->depart;
			nleft = this->GetOffsetPixelFromGraphBegin(first->depart);
		}
		else
		{
			dlt=first->arrive;
			nleft = this->GetOffsetPixelFromGraphBegin(first->arrive);
		}

		if(IsFlagTerminal(last->flag))
		{
			dlt=last->arrive-dlt;
			nright = this->GetOffsetPixelFromGraphBegin(last->arrive);
		}
		else
		{
			dlt=last->depart-dlt;
			nright = this->GetOffsetPixelFromGraphBegin(last->depart);
		}

		if(dlt>=86400)
			ptrain->myPosition[0].SetRect(0, top, nWholePixel, bottom);
		else
		{
			if(nright<nleft)
			{
				nleft-=120;
				if(nleft<0)
					nleft=0;

				nright+=100;
				if(nright>nWholePixel)
					nright=nWholePixel;
				ptrain->myPosition[0].SetRect(nleft, top, nWholePixel, bottom);
				ptrain->myPosition[1].SetRect(0, top, nright, bottom);
				ptrain->days=2;
			}
			else
				ptrain->myPosition[0].SetRect(left, top, right, bottom);
		}
	}
	else
	{
		ptrain->myPosition[0].SetRect(left, top, right, bottom);
	}
}

void CTGDoc::SetWorkTrainRect(CTG_TRAIN* ptrain)
{
	if(ptrain==NULL)
		return;
	
	UINT RecordNum = ptrain->GetRecordNum();
	if(RecordNum < 1)
		return;

	// -----------------------------------------------------------
	for(int i = 0; i < MAX_DAY_OF_BASIC_TRAIN; i++)
		ptrain->myPosition[i].SetRectEmpty();

	ptrain->days = 0;

	int mix_index=999, max_index=0;
	for(UINT idx = 0; idx < RecordNum; idx++)
	{
		TgTRAIN_RECORD* curr = ptrain->GetRecordByPosition(idx);
		if(curr == NULL)
			continue;

		if(curr->in_station_index >=0)
		{
			if(curr->in_station_index > max_index)
				max_index = curr->in_station_index;

			if(curr->in_station_index < mix_index)
				mix_index = curr->in_station_index;
		}

		if(curr->out_station_index >=0)
		{
			if(curr->out_station_index > max_index)
				max_index = curr->out_station_index;

			if(curr->out_station_index < mix_index)
				mix_index = curr->out_station_index;
		}
	}
	int top,bottom;
	if(mix_index>=0 && mix_index < this->m_nDispDistanceNum && max_index>=0 && max_index < this->m_nDispDistanceNum)
	{
		top = this->m_sDispDistance[mix_index].nStationYPos-20;
		bottom = this->m_sDispDistance[max_index].nSecondStationYPos+20;
	}
	else
	{
		top = this->m_sDispDistance[0].nStationYPos-20;
		bottom = this->m_sDispDistance[this->m_nDispDistanceNum-1].nSecondStationYPos+20;
	}
	if(top<0)
		top=0;
	
	int left = 9999999,right=0;
    int arl=0, dep=0;
	for(UINT idx = 0; idx < RecordNum; idx++)
	{
		TgTRAIN_RECORD* curr = ptrain->GetRecordByPosition(idx);
		if(curr == NULL)
			continue;
		
		if(idx == 0)
		{
			if(IsFlagAbsStart(curr->flag))
			{
				arl = this->GetOffsetPixelFromGraphBegin(curr->arrive);
				dep = arl;
				arl = arl - ptrain->myTrainInfo.stop_abs_time*m_fPixelPerMinute;	
			}
			else if(IsFlagStart(curr->flag))
			{
				dep = this->GetOffsetPixelFromGraphBegin(curr->depart);
				arl = dep;
			}
			else
			{
				arl = this->GetOffsetPixelFromGraphBegin(curr->arrive);
				if(IsFlagGiveout(curr->flag))
					dep = this->GetOffsetPixelFromGraphBegin(curr->depart);
				else
					dep = arl;
			}
		}
		if(idx == RecordNum-1)
		{
			arl = this->GetOffsetPixelFromGraphBegin(curr->arrive);
			dep = this->GetOffsetPixelFromGraphBegin(curr->depart);

			if(IsFlagAbsTerminal(curr->flag))
				dep = dep + curr->stop_abs_s_time*m_fPixelPerMinute;
			else if(IsFlagGiveout(curr->flag))
				dep = this->GetOffsetPixelFromGraphBegin(curr->depart);
			else
				dep = arl;
		}
		if(idx > 0 && idx < RecordNum-1)
		{
			arl = this->GetOffsetPixelFromGraphBegin(curr->arrive);
			dep = this->GetOffsetPixelFromGraphBegin(curr->depart);
		}

		if(arl < left)
			left = arl;
		if(dep > right)
			right = dep;
	}

	if(1 == RecordNum) // 2016-12-14 单点区间始发
	{
		TgTRAIN_RECORD* curr = ptrain->GetFirstRecord();
		if(curr != NULL)
		{
			if(IsFlagAbsStart(curr->flag))
			{
				arl = this->GetOffsetPixelFromGraphBegin(curr->arrive);
				dep = this->GetOffsetPixelFromGraphBegin(curr->depart);
				arl = arl - ptrain->myTrainInfo.stop_abs_time*m_fPixelPerMinute;	
			}
			
			if(IsFlagAbsTerminal(curr->flag))
			{
				arl = this->GetOffsetPixelFromGraphBegin(curr->arrive);
				dep = this->GetOffsetPixelFromGraphBegin(curr->depart);
				dep = dep + curr->stop_abs_s_time*m_fPixelPerMinute;
			}

			if(arl < left)
				left = arl;
			if(dep > right)
				right = dep;
		}
	}
	
	TgTRAIN_RECORD* first = ptrain->GetFirstRecord();
	if(NULL != first)
	{
		// recuse train
		if(IsFlagRescue(first->flag) && first->special_train_index > 0)
		{
			CTG_TRAIN* rescuedtrain = this->GetTrainByIndex(first->special_train_index);
			if(NULL != rescuedtrain)
			{
				TgTRAIN_RECORD* last = rescuedtrain->GetLastRecord();
				if(last != NULL && IsFlagRescued(last->flag))
				{
					int pos = this->GetOffsetPixelFromGraphBegin(last->depart)+m_fPixelPerMinute*last->stop_abs_e_time;
					if(pos < left)
						left = pos;
					if(pos > right)
						right = pos;
				}
			}
		}

		// divide train
		if(IsFlagDivide(first->flag) && first->special_train_index > 0)
		{
			ULONG train_index = 0;
			CTG_TRAIN* dividetrain1 = this->GetTrainByIndex(first->special_train_index);
			if(NULL != dividetrain1)
			{
				TgTRAIN_RECORD* first = dividetrain1->GetFirstRecord();
				TgTRAIN_RECORD* last = dividetrain1->GetLastRecord();
				if(first != NULL && IsFlagDivide(first->flag) && first->special_train_index > 0)
				{
					train_index = first->special_train_index;
					int pos = this->GetOffsetPixelFromGraphBegin(first->depart);
					if(pos < left)
						left = pos;
					if(pos > right)
						right = pos;
				}
				else if(last != NULL && IsFlagDivided(last->flag) && last->special_train_index > 0)
				{
					train_index = last->special_train_index;
					int pos = this->GetOffsetPixelFromGraphBegin(last->arrive);
					if(pos < left)
						left = pos;
					if(pos > right)
						right = pos;
				}
			}

			CTG_TRAIN* dividetrain2 = this->GetTrainByIndex(train_index);
			if(NULL != dividetrain2)
			{
				TgTRAIN_RECORD* first = dividetrain2->GetFirstRecord();
				TgTRAIN_RECORD* last = dividetrain2->GetLastRecord();
				if(first != NULL && IsFlagDivide(first->flag) && first->special_train_index == ptrain->m_nTrainIndex)
				{
					int pos = this->GetOffsetPixelFromGraphBegin(first->depart);
					if(pos < left)
						left = pos;
					if(pos > right)
						right = pos;
				}
				else if(last != NULL && IsFlagDivided(last->flag) && last->special_train_index == ptrain->m_nTrainIndex)
				{
					int pos = this->GetOffsetPixelFromGraphBegin(last->arrive);
					if(pos < left)
						left = pos;
					if(pos > right)
						right = pos;
				}
			}
		}
	}
	
	TgTRAIN_RECORD* last = ptrain->GetLastRecord();
	if(last != NULL)
	{
		// rescue train
		if(IsFlagRescued(last->flag) && last->special_train_index > 0)
		{
			CTG_TRAIN* rescuetrain = this->GetTrainByIndex(last->special_train_index);
			if(NULL != rescuetrain)
			{
				TgTRAIN_RECORD* first = rescuetrain->GetFirstRecord();
				if(first != NULL && IsFlagRescue(first->flag))
				{
					int pos = this->GetOffsetPixelFromGraphBegin(first->depart)+m_fPixelPerMinute*first->stop_abs_s_time;
					if(pos < left)
						left = pos;
					if(pos > right)
						right = pos;
				}
			}
		}

		// unit train
		if(IsFlagUnitOtherTrain(last->flag) && last->special_train_index > 0)
		{
			CTG_TRAIN* unittrain = this->GetTrainByIndex(last->special_train_index);
			if(NULL != unittrain)
			{
				TgTRAIN_RECORD* midrec = unittrain->GetStationRecord(last->station);
				if(midrec != NULL && IsFalgBeUnitedByOther(midrec->flag))
				{
					int pos = this->GetOffsetPixelFromGraphBegin(midrec->depart);
					if(pos < left)
						left = pos;
					if(pos > right)
						right = pos;
				}
			}
		}
		// divided train
		if(IsFlagDivided(last->flag) && last->special_train_index > 0)
		{
			CTG_TRAIN* dividetrain1 = this->GetTrainByIndex(last->special_train_index);
			ULONG train_index = 0;
			if(NULL != dividetrain1)
			{
				TgTRAIN_RECORD* first = dividetrain1->GetFirstRecord();
				if(first != NULL && IsFlagDivide(first->flag))
				{
					train_index = first->special_train_index;
					int pos = this->GetOffsetPixelFromGraphBegin(first->depart);
					if(pos < left)
						left = pos;
					if(pos > right)
						right = pos;
				}
			}
			
			CTG_TRAIN* dividetrain2 = this->GetTrainByIndex(train_index);
			if(NULL != dividetrain2)
			{
				TgTRAIN_RECORD* first = dividetrain2->GetFirstRecord();
				if(first != NULL && IsFlagDivide(first->flag) && first->special_train_index == ptrain->m_nTrainIndex)
				{
					int pos = this->GetOffsetPixelFromGraphBegin(first->depart);
					if(pos < left)
						left = pos;
					if(pos > right)
						right = pos;
				}
			}
		}
	}
	
	// unit train
	for(UINT idx = 0; idx < RecordNum; idx++)
	{
		TgTRAIN_RECORD* midrec = ptrain->GetRecordByPosition(idx);
		if(midrec == NULL)
			continue;
		if(IsFalgBeUnitedByOther(midrec->flag))
		{
			CTG_TRAIN* unitedtrain = this->GetTrainByIndex(midrec->special_train_index);
			if(NULL != unitedtrain)
			{
				TgTRAIN_RECORD* last = unitedtrain->GetLastRecord();
				if(last != NULL && IsFlagUnitOtherTrain(last->flag))
				{
					int pos = this->GetOffsetPixelFromGraphBegin(last->depart)+m_fPixelPerMinute*last->stop_abs_s_time;
					if(pos < left)
						left = pos;
					if(pos > right)
						right = pos;
				}
				break;
			}
		}
	}

	// 合并其它矩形信息
	CRect rect(left, top, right, bottom);
	CRect rect_now=ptrain->GetTrainRect();

	if(!rect_now.IsRectEmpty())
		rect.UnionRect(&rect_now, &rect);
	
	if(ptrain->mbDoubleDirect)
	{
		if(rect.top>m_nTrainZoomRangeY)
			rect.top-=m_nTrainZoomRangeY;
		else
			rect.top=0;
		rect.bottom+=m_nTrainZoomRangeY;
	}
	else
	{
		if(ptrain->m_nDirectFlag==0)
		{
			if(rect.top>40)
				rect.top-=40;
			else
				rect.top=0;
			rect.bottom+=m_nTrainZoomRangeY;
		}
		else
		{
			if(rect.top>m_nTrainZoomRangeY)
				rect.top-=m_nTrainZoomRangeY;
			else
				rect.top=0;
			rect.bottom+=40;
		}
	}
	if(rect.left>m_nTrainZoomRangeX)
		rect.left-=m_nTrainZoomRangeX;
	else
		rect.left=0;
	rect.right+=m_nTrainZoomRangeX;

	ptrain->myPosition[0]=rect;
	ptrain->days=1;
}

