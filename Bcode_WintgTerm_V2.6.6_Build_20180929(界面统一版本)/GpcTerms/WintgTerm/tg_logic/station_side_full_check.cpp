#include "stdafx.h"
#include "sysshare.h"
#include "function.h"
#include "tg_basicpara.h"
#include "station_side_full_check.h"

/////////////////////////////////////////////////////////////////////////////////
//   车站股道满线检查
/////////////////////////////////////////////////////////////////////////////////
StationOccupyInfo::StationOccupyInfo()
{
	Init();
}
void StationOccupyInfo::Init()
{
	station=0;
	side_count=0;
	for(int i=0; i<256; i++)
	{
		side_list[i].Init();
	}
}

void StationOccupyInfo::AddSideOccupy(int sideno, TIME a, TIME d)
{
	for(int m=0; m<side_count; m++)
	{
		if(side_list[m].sideno == sideno)
		{
			side_list[m].AddOccupy(a, d);
			return;
		}
	}

	for(int m=0; m<side_count; m++)
	{
		if(side_list[m].sideno == 0)
		{
			side_list[m].sideno = sideno;
			side_list[m].AddOccupy(a, d);
			break;
		}
	}
}
//void StationOccupyInfo::GetSideOccupyFull(CStringArray& list)
//{
//	for(int m=0; m<side_list[0].count; m++)
//	{
//		TIME atm=side_list[0].occupy[m].btm;
//		TIME dtm=side_list[0].occupy[m].etm;
//		if(atm<dtm)
//		{
//			// 清空股道占用时间
//			btm = atm;
//			etm = dtm;
//			for(int i=1; i<side_count; i++)
//			{
//				if(!CheckWithSide(i))
//				{
//					break;
//				}
//			}
//
//			if(i>=side_count)
//			{
//				CTime b(btm), e(etm);
//				CString str;
//				str.Format("车站 %s %s-%s 股道满线", 
//					gpcdef.GetStationName(station), b.Format("%H:%M"), e.Format("%H:%M"));
//				list.Add(str);
//			}
//		}
//	}
//}

void StationOccupyInfo::GetSideOccupyFull(CStringArray& list)
{
	if(side_list[0].count<=0)
		return;
	TIME minBegin = side_list[0].occupy[0].btm;
	TIME maxEnd   = side_list[0].occupy[0].etm;
	for(int n=0; n<side_count; n++)
	{
		for(int m=0; m<side_list[n].count; m++)
		{
			if(side_list[n].occupy[m].btm<minBegin)
				minBegin=side_list[n].occupy[m].btm;
			if(side_list[n].occupy[m].etm>maxEnd)
				maxEnd=side_list[n].occupy[m].etm;
		}
	}
	
	// 步长10秒检查
	for(TIME T=minBegin; T<=maxEnd; T=T+10)
	{
		// 保存股道占满时间
		TIME btm=0, etm=0; 
		for(int n=0; n<side_count; n++)
		{
			for(int m=0; m<side_list[n].count; m++)
			{
				if(side_list[n].occupy[m].btm < T && T < side_list[n].occupy[m].etm)
				{
					if(side_list[n].occupy[m].btm>btm || btm==0)
						btm = side_list[n].occupy[m].btm;
					if(side_list[n].occupy[m].etm<etm || etm==0)
						etm = side_list[n].occupy[m].etm;
					break;
				}
			}

			if(m>=side_list[n].count)
			{
				break;
			}
		}

		if(n>=side_count)
		{
			T = etm;
			CTime b(btm), e(etm);
			CString str;
			str.Format("%s 站 %s 计划满线", 
					gpcdef.GetStationName(station), b.Format("%H:%M"));
			list.Add(str);
		}
	}
}

//bool StationOccupyInfo::CheckWithSide(int i)
//{
//	for(int m=0; m<side_list[i].count; m++)
//	{
//		TIME btm1=side_list[i].occupy[m].btm;
//		TIME etm1=side_list[i].occupy[m].etm;
//		if(btm1<etm1)
//		{
//			if(etm<=btm1 || etm1<=btm)
//			{
//				;// 不相交
//			}
//			else
//			{
//				if(btm<btm1 && btm1<etm)
//				{
//					btm = btm1;
//				}
//				else if(btm1<btm && btm<etm1 && btm1<etm && etm<etm1)
//				{
//					; // 相交时间不变
//				}
//				else if(btm<etm1 && etm1<etm)
//				{
//					etm = etm1;
//				}
//				else if(btm<btm1 && btm1<etm && btm<etm1 && etm1<etm)
//				{
//					btm = btm1;
//					etm = etm1;
//				}
//
//				return true;
//			}
//		}
//	}
//
//	return false;
//}

///////////////////////////////////////////////////////////////
StationSideOccupyCheck::StationSideOccupyCheck()
{
	Init();
}

void StationSideOccupyCheck::Init()
{
	m_nStationCount=0;
	for(int n=0; n<128; n++)
		stationOccupy[n].Init();
}

void StationSideOccupyCheck::AddStationSideOccupy(int station, int sideno, TIME a, TIME d)
{
	if(a>=d)
		return;
	for(int m=0; m<m_nStationCount; m++)
	{
		if(stationOccupy[m].station == station)
		{
			stationOccupy[m].AddSideOccupy(sideno, a, d);
			return;
		}
	}
    
	if(m_nStationCount<128)
	{
		stationOccupy[m_nStationCount].Init();
		stationOccupy[m_nStationCount].side_count = GetTGBasicParam()->GetStationSideCount(station);
		stationOccupy[m_nStationCount].station = station;
		stationOccupy[m_nStationCount].AddSideOccupy(sideno, a, d);
		m_nStationCount++;
	}
}

void StationSideOccupyCheck::GetStationSideOccupyFull(CStringArray& list)
{
	list.RemoveAll();
	for(int i=0; i<m_nStationCount; i++)
	{
		stationOccupy[i].GetSideOccupyFull(list);
	}
}
