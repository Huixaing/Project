#include "stdafx.h"
#include "resource.h"
#include "plotDoc.h"

void CPlotDoc::DrawSingleTrain(CDC * pCDC, CTG_SingleTrain * single,int nDrawMode ,int nStartNo,int nPointNum,BOOL isPrint,int schd_type)
{
	if(1==m_nDrawTrainConsiderStationSep)
	{
		// 优化过车站展开
		DrawSingleTrain_New(pCDC, single, nDrawMode, nStartNo, nPointNum, isPrint, schd_type);
		return;
	}

	int scaleRate = min(yMultiple,5);

	
	if(m_nShowTrainFlag != 2)
	{
		if(TG_IsPassengerTrainType(single->GetTrainType()) && m_nShowTrainFlag == 1)
		{
			return;
		}
		
		if(TG_IsFreightTrainType(single->GetTrainType())  && m_nShowTrainFlag == 0)
		{
			return;
		}
	}

	CString str_prefix;
	str_prefix.Format("%s",single->GetTrainID());
	if(str_prefix.Left(1) == "E" || str_prefix.Left(1) == "e")
		return;	
	int m_nLogPixelSy = pCDC->GetDeviceCaps(LOGPIXELSY);

	nStartNo = 0; 		
	int m_nPointCount = 0;
	TRAINPERPOINTINFO m_pPerPointInfo[300];

	int locCurrentSectionNo = 0; //当前要绘制的区段

	//小矩形的大小
	CSize sSize = CSize(8,8);
	ushort uDis = 4;
	CPoint critical_point;

	BOOL bad_Mark = FALSE;
	BOOL exception_Mark = FALSE;
	BOOL reverse_Mark = FALSE;
	BOOL stopReason_Mark = FALSE;
	BOOL bShowReverse = FALSE;

	CPoint single_ptFirst,single_ptSecond,single_ptThird;

	int	x_point_offset = x_offset_print;
	int	y_point_offset = y_offset_print;

	ushort nDirectFlag = 0;
	int m_nDirectFlag_prefix = 0;//列车上下行
	m_nDirectFlag_prefix = GetTrainRecDirect(single, 0);
	if(m_nDirectFlag_prefix == DOWN_DIRECT)
	{
		nDirectFlag = 2;
	}
	else
	{
		nDirectFlag = 1;
		m_nDirectFlag_prefix = UP_DIRECT;
	}

	if(schd_type == BASIC_SCHEDULE)
	{
		locCurrentSectionNo = m_nCurrentSectionNo;
		bShowReverse = FALSE;
	}
	else
	{
		locCurrentSectionNo = m_nCurrentSectionNo;
		bShowReverse = m_bShowReverse;
	}

	int usDrawCount = single->GetTrainRecordCount();

	int circleFlag = 0;
	int historyWidth = (int)(m_nHistoryGraphWidth*m_fPixelPerMinute);

	int wholedayWidth = (int)(WHOLE_DAY_MINUTE* m_fPixelPerMinute);
	CRect resultRect, rangeRect(0, 0, m_nPageWidth, m_nPageHeight);
	if(m_nSchdStatus == WORK_SCHEDULE || m_nSchdStatus == SHIFT_SCHEDULE)
	{
		rangeRect.SetRect(0, 0, m_nPageWidth, m_nPageHeight);
	}
	else if(m_nSchdStatus==HISTORY_SCHEDULE)
		rangeRect.SetRect(0, 0, historyWidth, m_nPageHeight);
	else if(m_nSchdStatus==BASIC_SCHEDULE)
		rangeRect.SetRect(0, 0, wholedayWidth, m_nPageHeight);


	if(isPrint == TRUE)
		rangeRect.SetRect(0,0,m_fPixelPerHour* m_nViewTimeRange,m_nPageHeight);

	// prepare gdi resource
	// line attribute
	CPen mThinPen, *pOldPen;
	int factor = single->GetTrainInfo()->m_data.type;

	COLORREF lColor = GetSchdColor(factor);
	COLORREF lActualColor = GetActualLineColor(factor);

	int nPreviousMode = pCDC->SetROP2(R2_COPYPEN);
	mThinPen.CreatePen(PS_SOLID, m_nLineWidthRate*scaleRate, lColor);
	pOldPen = pCDC->SelectObject(&mThinPen);
	// text attribute
	int nOldColor = pCDC->SetTextColor(lColor);
	int nOldBackMode = pCDC->SetBkMode(TRANSPARENT);
	int nOldAlign = pCDC->SetTextAlign(TA_RIGHT | TA_BOTTOM);

	// begin draw train line
	char time_char[2];
	CPoint ptlStart, ptlEnd;
	CTG_TrainRecord *first=NULL, *second=NULL;
	double flFirstMinuteOffset=0, flSecondMinuteOffset=0;
	long usStartDate=0, usStartTime=0, usEndDate=0, usEndTime=0;

	ushort usRouteSetNo = -1;

	//过表时改车次时候使用的变量
	int critial_record_index;
	CString NewTrainId = ""; //过表时候使用的新车次号

	ushort first_a_time_status=0xFFFF,second_a_time_status=0xFFFF;
	ushort first_d_time_status=0xFFFF,second_d_time_status=0xFFFF;

	second = single->GetTrainRecord(nStartNo);
	int first_primary_entry=0,first_primary_exit=0,first_bottom_entry=0,first_bottom_exit=0,first_top_entry=0,first_top_exit=0; 
	int second_primary_entry=0,second_primary_exit=0,second_bottom_entry=0,second_bottom_exit=0,second_top_entry=0,second_top_exit=0; 
	for(int i=nStartNo; i<usDrawCount; i++)//first
	{
		bad_Mark = FALSE;//MARK OF BAD ADD BY CF AFTER 7-18
		exception_Mark = FALSE;//MARK OF BAD ADD BY CF AFTER 7-18
		reverse_Mark  = FALSE;//MARK OF REVERSE ADD BY CF AFTER 7-18
		stopReason_Mark = FALSE;//MARK OF REVERSE ADD BY CF AFTER 7-18

		first = second;
		second = single->GetTrainRecord(i+1);
		if( first == NULL ) continue;

		//for actual time or plan time codeing   
		if(first->m_data.adjust_status & 0x0001)//first bit data
			first_a_time_status = 1;//real time
		else
			first_a_time_status = 0;// plan time 
		if(first->m_data.adjust_status & 0x0002)//second bit data
			first_d_time_status = 1;//real time
		else
			first_d_time_status = 0;// plan time 

		if(i==0 && IsFlagStart(first->m_data.flag))
			first_a_time_status = first_d_time_status;

		if(bShowReverse) //2004-03-20是否显示反向行车标志
		{
			if(IsFlagReverse(first->m_data.flag))
			{
				reverse_Mark  = TRUE;
			}
			if(second!=NULL)
			{
				if(first->m_data.station==second->m_data.station && IsFlagStopAbs(first->m_data.flag))
				{
					reverse_Mark  = FALSE;
				}
			}
		}

		if(second != NULL)
		{
			//for actual time or plan time codeing   
			if(second->m_data.adjust_status & 0x0001)//first bit data
				second_a_time_status = 1;//real time
			else
				second_a_time_status = 0;// plan time 
			if(second->m_data.adjust_status & 0x0002)//second bit data
				second_d_time_status = 1;//real time
			else
				second_d_time_status = 0;// plan time 
			if(IsFlagTerminal(second->m_data.flag))
				second_d_time_status = second_a_time_status;
		}

		int nCurIndex = 0;
		int first_index,second_index;
		first_index = second_index = -1;

		//对于绘制接入交出点的列车有用
		int nDirectFlag_pre =  nDirectFlag;//上半截车的方向
		int nDirectFlag_next = nDirectFlag;//下半截车的方向

		//加上这里是为了判断此车站是否有挂机车
		int m_nCountOfEngine = GetEngineCount(single, i);

		int first_sta_in_index = first->GetArriveTGIndex();
		int first_sta_out_index = first->GetDepartTGIndex();


		first_primary_entry = GetStationVPosByIndex(single,first,PRIMARY_TRACK,ENTRY_POSITION);
		first_primary_exit = GetStationVPosByIndex(single,first,PRIMARY_TRACK,EXIT_POSITION);

		first_bottom_entry = GetStationVPosByIndex(single, first, BOTTOM_TRACK, ENTRY_POSITION);
		first_bottom_exit = GetStationVPosByIndex(single, first, BOTTOM_TRACK, EXIT_POSITION);

		first_top_entry = GetStationVPosByIndex(single, first, TOP_TRACK, ENTRY_POSITION);
		first_top_exit = GetStationVPosByIndex(single, first, TOP_TRACK, EXIT_POSITION);

		if(second != NULL)
		{
			int second_sta_in_index = second->GetArriveTGIndex();
			int second_sta_out_index = second->GetDepartTGIndex();

			second_primary_entry = GetStationVPosByIndex(single,second,PRIMARY_TRACK,ENTRY_POSITION);
			second_primary_exit = GetStationVPosByIndex(single,second,PRIMARY_TRACK,EXIT_POSITION);
			second_bottom_entry = GetStationVPosByIndex(single, second, BOTTOM_TRACK, ENTRY_POSITION);
			second_bottom_exit = GetStationVPosByIndex(single, second, BOTTOM_TRACK, EXIT_POSITION);
			second_top_entry = GetStationVPosByIndex(single, second, TOP_TRACK, ENTRY_POSITION);
			second_top_exit = GetStationVPosByIndex(single, second, TOP_TRACK, EXIT_POSITION);
			if(first_primary_exit > second_primary_entry)
			{
				nDirectFlag = 1;
			}
			else
			{
				nDirectFlag = 2;
			}

			if(IsFlagStopAbs(first->m_data.flag)||IsFlagStopSignal(first->m_data.flag))
			{
				if(IsFlagStopAbsUp(first->m_data.flag))
					nDirectFlag = 1;
				else
					nDirectFlag = 2;
			}

			if(!(IsFlagStart(first->m_data.flag)||IsFlagTerminal(first->m_data.flag))
				&& first_primary_entry != first_primary_exit
				&& (m_nTrainLeapLineFlag==2 || m_nTrainLeapLineFlag==4))
			{
				CTG_TrainRecord *pre=NULL;
				CTG_TrainRecord *next=NULL;

				if(i-1>=0)
					pre = single->GetTrainRecord(i-1); 

				if((i+1)<single->GetTrainRecordCount())
					next = single->GetTrainRecord(i+1); 

				if(pre != NULL )
				{
					int pre_sta_index = pre->GetDepartTGIndex();
					if(m_sDispDistance[pre_sta_index].nStationYPos > first_primary_entry)					
					{
						nDirectFlag_pre = UP_DIRECT;// up direct
						nDirectFlag = 1;
					}
					else							
					{
						nDirectFlag_pre = DOWN_DIRECT;// down direct 
						nDirectFlag = 2;
					}
				}

				//判断接入点的方向
				if(next != NULL )
				{
					int next_sta_index = next->GetArriveTGIndex();
					if(first_primary_exit > m_sDispDistance[next_sta_index].nStationYPos)					
						nDirectFlag_next = UP_DIRECT;// up direct 
					else							
						nDirectFlag_next = DOWN_DIRECT;// down direct 						
				}	
			}
			//2005-01-28
			else
			{
				nDirectFlag_pre = nDirectFlag_next = nDirectFlag==1?UP_DIRECT:DOWN_DIRECT;
			}
		}
		else
		{
			CTG_TrainRecord* first_pre = single->GetTrainRecord(i-1);
			if(first_pre != NULL)
			{
				int first_pre_sta_in_index = first_pre->GetArriveTGIndex();
				int first_pre_sta_out_index = first_pre->GetDepartTGIndex();

				int pre_primary_exit = GetStationVPosByIndex(single,first_pre,PRIMARY_TRACK,EXIT_POSITION);
				
				if(pre_primary_exit > first_primary_exit)
				{
					nDirectFlag = 1;
				}
				else
				{
					nDirectFlag = 2;
				}
			}
			else
			{
				//这里判断往返开的车最后一个点交出符号的结尾位置判断。
				if((i==usDrawCount-1)&&!IsFlagTerminal(first->m_data.flag)&&(first->m_data.exit != (ushort) -1))
				{
					int flag=GetEntryExitDirection(single,first,EXIT_POSITION,locCurrentSectionNo,schd_type);
					if(flag==UP_MARK)
					{
						nDirectFlag_next=UP_MARK;
						nDirectFlag=1;
					}
					else
					{
						nDirectFlag_next=DOWN_MARK;
						nDirectFlag=2;
					}
				}
			}
		} 
		// check whether valid or not
		if(first->m_data.arrive==NOTIME && first->m_data.depart==NOTIME)
			break;

		// draw stop station line
		usStartTime = minute_time(first->m_data.arrive,&usStartDate);
		if(first->m_data.depart == NOTIME || first->m_data.depart == 0)
		{
			usEndDate = usStartDate;
			usEndTime = usStartTime;
		}
		else
		{
			usEndTime = minute_time(first->m_data.depart,&usEndDate);
		}
		if(usEndTime>=WHOLE_DAY_SECOND)
		{
			usEndTime -= WHOLE_DAY_SECOND;
			usEndDate = OsGetNextDate(PMTG.ThreadId, usEndDate);
		}

		//draw decompose train
		if(IsFalgCombine(first->m_data.flag))
		{
			int ii,jj, nRecNum;
			CPoint ptFirst, ptSecond, ptThird;
			CTG_TrainRecord *rec;
			CTG_SingleTrain *pFirstTrain = NULL;

			//begin to compute ptFirst, ptSecond, ptThird
			if (i == usDrawCount-1)                        //如果是被分解的列车
			{
				for(jj = 0;jj < m_vecSchdTrains.size();jj++)
				{
					if(m_vecSchdTrains[jj].GetTrainIndex() == first->m_data.special_train_index)
					{
						pFirstTrain = &m_vecSchdTrains[jj];
						break;
					}
				}
				do 
				{
					if(pFirstTrain == NULL) continue;
					nRecNum = pFirstTrain ->GetTrainRecordCount() ;		
					for(ii = 0;ii < nRecNum;ii++)
					{
						rec = pFirstTrain->GetTrainRecord(ii);
						if(IsFlagEnd(rec->m_data.flag))
							break;
					}

					if(ii >= nRecNum) continue;
					long time_depart,date_depart,time_arrival,date_arrival;
					time_arrival = minute_time(first->m_data.arrive,&date_arrival);
					if(first->m_data.depart == NOTIME || first->m_data.depart == 0)
					{
						date_depart = date_arrival;
						time_depart = time_arrival;
					}
					else
					{
						time_depart = minute_time(first->m_data.depart,&date_depart);
					}
					long time_depart_rec,date_depart_rec,time_arrival_rec,date_arrival_rec;
					time_arrival_rec = minute_time(rec->m_data.arrive,&date_arrival_rec);
					if(rec->m_data.depart == NOTIME || rec->m_data.depart == 0)
					{
						date_depart_rec = date_arrival_rec;
						time_depart_rec = time_arrival_rec;
					}
					else
					{
						time_depart_rec = minute_time(rec->m_data.depart,&date_depart_rec);
					}
					flFirstMinuteOffset = GetMinuteWidthToPlotStartTime(time_depart/60,date_depart,isPrint,m_nSchdStatus);
					flSecondMinuteOffset = GetMinuteWidthToPlotStartTime(time_depart_rec/60,date_depart_rec,isPrint,m_nSchdStatus);
					CPoint fPoint,sPoint;
					if(nDirectFlag == 2)
					{// down direct
						fPoint.y = sPoint.y = first_bottom_exit  + 4;
					}
					else
					{
						fPoint.y = sPoint.y = first_top_exit - 4;
					}
					fPoint.x = (int)(flFirstMinuteOffset * m_fPixelPerMinute);
					sPoint.x = (int)(flSecondMinuteOffset * m_fPixelPerMinute);
					fPoint.Offset (x_point_offset,y_point_offset);
					sPoint.Offset (x_point_offset,y_point_offset);
					ptFirst = fPoint;
					ptSecond = sPoint;
				} while(0);
				if (NULL == rec) continue;

				for(jj = 0;jj < m_vecSchdTrains.size();jj++)
				{
					if(m_vecSchdTrains[jj].GetTrainIndex() == rec->m_data.special_train_index)
					{
						pFirstTrain = &m_vecSchdTrains[jj];
						break;
					}
				}

				do 
				{
					if (NULL == pFirstTrain) continue;
					nRecNum = pFirstTrain ->GetTrainRecordCount() ;		
					for(ii = 0;ii < nRecNum;ii++)
					{
						rec = pFirstTrain->GetTrainRecord(ii);
						if(IsFlagEnd(rec->m_data.flag))
							break;
					}

					if(ii >= nRecNum) continue;

					long time_depart,date_depart,time_arrival,date_arrival;
					time_arrival = minute_time(first->m_data.arrive,&date_arrival);
					if(first->m_data.depart == NOTIME || first->m_data.depart == 0)
					{
						date_depart = date_arrival;
						time_depart = time_arrival;
					}
					else
					{
						time_depart = minute_time(first->m_data.depart,&date_depart);
					}
					long time_depart_rec,date_depart_rec,time_arrival_rec,date_arrival_rec;
					time_arrival_rec = minute_time(rec->m_data.arrive,&date_arrival_rec);
					if(rec->m_data.depart == NOTIME || rec->m_data.depart == 0)
					{
						date_depart_rec = date_arrival_rec;
						time_depart_rec = time_arrival_rec;
					}
					else
					{
						time_depart_rec = minute_time(rec->m_data.depart,&date_depart_rec);
					}
					flFirstMinuteOffset = GetMinuteWidthToPlotStartTime(time_depart/60,date_depart,isPrint,m_nSchdStatus);
					flSecondMinuteOffset = GetMinuteWidthToPlotStartTime(time_depart_rec/60,date_depart_rec,isPrint,m_nSchdStatus);
					CPoint fPoint,sPoint;
					if(nDirectFlag == 2){// down direct
						fPoint.y = sPoint.y = first_bottom_exit  + 4;
					}else{
						fPoint.y = sPoint.y = first_top_exit - 4;
					}
					fPoint.x = (int)(flFirstMinuteOffset * m_fPixelPerMinute);
					sPoint.x = (int)(flSecondMinuteOffset * m_fPixelPerMinute);
					fPoint.Offset (x_point_offset,y_point_offset);
					sPoint.Offset (x_point_offset,y_point_offset);
					ptThird = sPoint;
				} while(0);
			}

			if (i == 0)						//如果是分解后的列车，有可能是分解后的第一列车，也有可能是分解后的第二列车
			{	
				for(jj = 0;jj < m_vecSchdTrains.size();jj++)
				{
					if(m_vecSchdTrains[jj].GetTrainIndex() == first->m_data.special_train_index){
						pFirstTrain = &m_vecSchdTrains[jj];
						break;
					}
				}
				do 
				{
					if (NULL == pFirstTrain) continue;
					nRecNum = pFirstTrain ->GetTrainRecordCount() ;		
					for(ii = 0;ii < nRecNum;ii++)
					{
						rec = pFirstTrain->GetTrainRecord(ii);
						if(IsFlagEnd(rec->m_data.flag))
							break;
					}

					if(ii >= nRecNum) continue;

					if (ii == 0)						//如果是分解后的第二列车
					{
						long time_depart,date_depart,time_arrival,date_arrival;
						time_arrival = minute_time(first->m_data.arrive,&date_arrival);
						if(first->m_data.depart == NOTIME || first->m_data.depart == 0)
						{
							date_depart = date_arrival;
							time_depart = time_arrival;
						}
						else
						{
							time_depart = minute_time(first->m_data.depart,&date_depart);
						}
						long time_depart_rec,date_depart_rec,time_arrival_rec,date_arrival_rec;
						time_arrival_rec = minute_time(rec->m_data.arrive,&date_arrival_rec);
						if(rec->m_data.depart == NOTIME || rec->m_data.depart == 0)
						{
							date_depart_rec = date_arrival_rec;
							time_depart_rec = time_arrival_rec;
						}
						else
						{
							time_depart_rec = minute_time(rec->m_data.depart,&date_depart_rec);
						}
						flFirstMinuteOffset = GetMinuteWidthToPlotStartTime(time_depart/60,date_depart,isPrint,m_nSchdStatus);
						flSecondMinuteOffset = GetMinuteWidthToPlotStartTime(time_depart_rec/60,date_depart_rec,isPrint,m_nSchdStatus);
						CPoint fPoint,sPoint;
						if(nDirectFlag == 2){// down direct
							fPoint.y = sPoint.y = first_bottom_exit  + 4;
						}else{
							fPoint.y = sPoint.y = first_top_exit - 4;
						}
						fPoint.x = (int)(flFirstMinuteOffset * m_fPixelPerMinute);
						sPoint.x = (int)(flSecondMinuteOffset * m_fPixelPerMinute);
						fPoint.Offset (x_point_offset,y_point_offset);
						sPoint.Offset (x_point_offset,y_point_offset);
						ptSecond = fPoint;
						ptThird = sPoint;

						for(jj = 0;jj < m_vecSchdTrains.size();jj++)
						{
							if(m_vecSchdTrains[jj].GetTrainIndex() == rec->m_data.special_train_index){
								pFirstTrain = &m_vecSchdTrains[jj];
								break;
							}
						}
						if(NULL != pFirstTrain)
						{
							nRecNum = pFirstTrain ->GetTrainRecordCount() ;
							for(ii = 0;ii < nRecNum;ii++)
							{
								rec = pFirstTrain->GetTrainRecord(ii);
								if(IsFlagEnd(rec->m_data.flag))
									break;
							}
							long time_depart,date_depart,time_arrival,date_arrival;
							time_arrival = minute_time(first->m_data.arrive,&date_arrival);
							if(first->m_data.depart == NOTIME || first->m_data.depart == 0)
							{
								date_depart = date_arrival;
								time_depart = time_arrival;
							}
							else
							{
								time_depart = minute_time(first->m_data.depart,&date_depart);
							}
							long time_depart_rec,date_depart_rec,time_arrival_rec,date_arrival_rec;
							time_arrival_rec = minute_time(rec->m_data.arrive,&date_arrival_rec);
							if(rec->m_data.depart == NOTIME || rec->m_data.depart == 0)
							{
								date_depart_rec = date_arrival_rec;
								time_depart_rec = time_arrival_rec;
							}
							else
							{
								time_depart_rec = minute_time(rec->m_data.depart,&date_depart_rec);
							}
							flFirstMinuteOffset = GetMinuteWidthToPlotStartTime(time_depart/60,date_depart,isPrint,m_nSchdStatus);
							flSecondMinuteOffset = GetMinuteWidthToPlotStartTime(time_depart_rec/ 60,date_depart_rec,isPrint,m_nSchdStatus);
							CPoint fPoint,sPoint;
							if(nDirectFlag == 2){// down direct
								fPoint.y = sPoint.y = first_bottom_exit  + 4;
							}else{
								fPoint.y = sPoint.y = first_top_exit - 4;
							}
							fPoint.x = (int)(flFirstMinuteOffset * m_fPixelPerMinute);
							sPoint.x = (int)(flSecondMinuteOffset * m_fPixelPerMinute);
							fPoint.Offset (x_point_offset,y_point_offset);
							sPoint.Offset (x_point_offset,y_point_offset);
							ptFirst = sPoint;
						}
					} 
					else if (ii == nRecNum - 1)						//如果是被分解列车
					{
						long time_depart,date_depart,time_arrival,date_arrival;
						time_arrival = minute_time(first->m_data.arrive,&date_arrival);
						if(first->m_data.depart == NOTIME || first->m_data.depart == 0)
						{
							date_depart = date_arrival;
							time_depart = time_arrival;
						}
						else
						{
							time_depart = minute_time(first->m_data.depart,&date_depart);
						}
						long time_depart_rec,date_depart_rec,time_arrival_rec,date_arrival_rec;
						time_arrival_rec = minute_time(rec->m_data.arrive,&date_arrival_rec);
						if(rec->m_data.depart == NOTIME || rec->m_data.depart == 0)
						{
							date_depart_rec = date_arrival_rec;
							time_depart_rec = time_arrival_rec;
						}
						else
						{
							time_depart_rec = minute_time(rec->m_data.depart,&date_depart_rec);
						}
						flFirstMinuteOffset = GetMinuteWidthToPlotStartTime(time_depart/60,date_depart,isPrint,m_nSchdStatus);
						flSecondMinuteOffset = GetMinuteWidthToPlotStartTime(time_depart_rec/60,date_depart_rec,isPrint,m_nSchdStatus);
						CPoint fPoint,sPoint;
						if(nDirectFlag == 2){// down direct
							fPoint.y = sPoint.y = first_bottom_exit  + 4;
						}else{
							fPoint.y = sPoint.y = first_top_exit - 4;
						}
						fPoint.x = (int)(flFirstMinuteOffset * m_fPixelPerMinute);
						sPoint.x = (int)(flSecondMinuteOffset * m_fPixelPerMinute);
						fPoint.Offset (x_point_offset,y_point_offset);
						sPoint.Offset (x_point_offset,y_point_offset);
						ptThird = fPoint;
						ptFirst = sPoint;

						for(jj = 0;jj < m_vecSchdTrains.size();jj++)
						{
							if(m_vecSchdTrains[jj].GetTrainIndex() == rec->m_data.special_train_index){
								pFirstTrain = &m_vecSchdTrains[jj];
								break;
							}
						}
						if(NULL != pFirstTrain)
						{
							nRecNum = pFirstTrain ->GetTrainRecordCount() ;
							for(ii = 0;ii < nRecNum;ii++)
							{
								rec = pFirstTrain->GetTrainRecord(ii);
								if(IsFlagEnd(rec->m_data.flag))
									break;
							}

							long time_depart,date_depart,time_arrival,date_arrival;
							time_arrival = minute_time(first->m_data.arrive,&date_arrival);
							if(first->m_data.depart == NOTIME || first->m_data.depart == 0)
							{
								date_depart = date_arrival;
								time_depart = time_arrival;
							}
							else
							{
								time_depart = minute_time(first->m_data.depart,&date_depart);
							}
							long time_depart_rec,date_depart_rec,time_arrival_rec,date_arrival_rec;
							time_arrival_rec = minute_time(rec->m_data.arrive,&date_arrival_rec);
							if(rec->m_data.depart == NOTIME || rec->m_data.depart == 0)
							{
								date_depart_rec = date_arrival_rec;
								time_depart_rec = time_arrival_rec;
							}
							else
							{
								time_depart_rec = minute_time(rec->m_data.depart,&date_depart_rec);
							}
							flFirstMinuteOffset = GetMinuteWidthToPlotStartTime(time_depart/60,date_depart,isPrint,m_nSchdStatus);
							flSecondMinuteOffset = GetMinuteWidthToPlotStartTime(time_depart_rec/60,date_depart_rec,isPrint,m_nSchdStatus);
							CPoint fPoint,sPoint;
							if(nDirectFlag == 2){// down direct
								fPoint.y = sPoint.y = first_bottom_exit  + 4;
							}else{
								fPoint.y = sPoint.y = first_top_exit - 4;
							}
							fPoint.x = (int)(flFirstMinuteOffset * m_fPixelPerMinute);
							sPoint.x = (int)(flSecondMinuteOffset * m_fPixelPerMinute);
							fPoint.Offset (x_point_offset,y_point_offset);
							sPoint.Offset (x_point_offset,y_point_offset);
							ptSecond = sPoint;
						}
					}
				} while(0);
			}
			single_ptFirst = ptFirst;
			single_ptSecond = ptSecond;
			single_ptThird = ptThird;
			//end to compute ptFirst, ptSecond, ptThird

			//begin to draw the line among divided trains
			CPen mPen,*pOldPen;
			mPen.CreatePen(PS_DOT,1,RGB(128,0,255));
			pOldPen = pCDC->SelectObject(&mPen);
			do 
			{
				if((single_ptFirst.x-x_point_offset) > rangeRect.right) break;
				if((single_ptSecond.x-x_point_offset) > rangeRect.right) single_ptSecond.x = rangeRect.right + x_point_offset;
				if((single_ptThird.x-x_point_offset) > rangeRect.right) single_ptThird.x = rangeRect.right + x_point_offset;

				if((single_ptThird.x-x_point_offset) < rangeRect.left) break;
				if((single_ptFirst.x-x_point_offset) < rangeRect.left)	single_ptFirst.x = rangeRect.left + x_point_offset;
				if((single_ptSecond.x-x_point_offset) < rangeRect.left)	single_ptSecond.x = rangeRect.left + x_point_offset;
				if(single_ptSecond.x <= single_ptThird.x)
				{
					if(single_ptFirst.x<=single_ptSecond.x)
					{

						if (m_bStopStationWaveLine)				//画波浪线
							DrawChordLine(pCDC,single_ptFirst,single_ptSecond,m_nDirectFlag_prefix);	
						else									//画直线
						{
							pCDC->MoveTo(single_ptFirst);
							pCDC->LineTo(single_ptSecond);
						}
					}
					else
					{
						if (m_bStopStationWaveLine)
							DrawChordLine(pCDC,single_ptSecond,single_ptFirst,m_nDirectFlag_prefix);
						else
						{
							pCDC->MoveTo(single_ptSecond);
							pCDC->LineTo(single_ptFirst);
						}
					}
				}
				else
				{
					if(single_ptFirst.x<=single_ptThird.x)
					{
						if (m_bStopStationWaveLine)
							DrawChordLine(pCDC,single_ptFirst,single_ptThird,m_nDirectFlag_prefix);
						else
						{
							pCDC->MoveTo(single_ptFirst);
							pCDC->LineTo(single_ptThird);
						}
					}
					else
					{
						if (m_bStopStationWaveLine)
							DrawChordLine(pCDC,single_ptThird,single_ptFirst,m_nDirectFlag_prefix);
						else
						{
							pCDC->MoveTo(single_ptThird);
							pCDC->LineTo(single_ptFirst);
						}
					}
				}
			} while(0);
			pCDC->SelectObject(pOldPen);
			mPen.DeleteObject();
			//end to draw the line among divided trains
		}
		//end of draw decompose train

		//draw Divide Train						added by zzg in ty 2008.05.10
		if(IsFlagDivide(first->m_data.flag))
		{
			int ii,jj, nRecNum;
			CPoint ptFirst, ptSecond, ptThird;
			CTG_TrainRecord *rec;
			CTG_SingleTrain *pFirstTrain = NULL;

			//begin to compute ptFirst, ptSecond, ptThird
			if (i == usDrawCount-1)                        //如果是被分解的列车
			{
				for(jj = 0;jj < m_vecSchdTrains.size();jj++)
				{
					if(m_vecSchdTrains[jj].GetTrainIndex() == first->m_data.special_train_index){
						pFirstTrain = &m_vecSchdTrains[jj];
						break;
					}
				}
				do 
				{
					if(pFirstTrain == NULL) continue;
					nRecNum = pFirstTrain ->GetTrainRecordCount() ;		
					for(ii = 0;ii < nRecNum;ii++)
					{
						rec = pFirstTrain->GetTrainRecord(ii);
						if(IsFlagDivide(rec->m_data.flag))
							break;
					}

					if(ii >= nRecNum) continue;
					long time_depart,date_depart,time_arrival,date_arrival;
					time_arrival = minute_time(first->m_data.arrive,&date_arrival);
					if(first->m_data.depart == NOTIME || first->m_data.depart == 0)
					{
						date_depart = date_arrival;
						time_depart = time_arrival;
					}
					else
					{
						time_depart = minute_time(first->m_data.depart,&date_depart);
					}
					long time_depart_rec,date_depart_rec,time_arrival_rec,date_arrival_rec;
					time_arrival_rec = minute_time(rec->m_data.arrive,&date_arrival_rec);
					if(rec->m_data.depart == NOTIME || rec->m_data.depart == 0)
					{
						date_depart_rec = date_arrival_rec;
						time_depart_rec = time_arrival_rec;
					}
					else
					{
						time_depart_rec = minute_time(rec->m_data.depart,&date_depart_rec);
					}
					flFirstMinuteOffset = GetMinuteWidthToPlotStartTime(time_depart/60,date_depart,isPrint,m_nSchdStatus);
					flSecondMinuteOffset = GetMinuteWidthToPlotStartTime(time_depart_rec/60,date_depart_rec,isPrint,m_nSchdStatus);
					CPoint fPoint,sPoint;
					if(nDirectFlag == 2){// down direct
						fPoint.y = sPoint.y = first_bottom_exit  + 4;
					}else{
						fPoint.y = sPoint.y = first_top_exit - 4;
					}
					fPoint.x = (int)(flFirstMinuteOffset * m_fPixelPerMinute);
					sPoint.x = (int)(flSecondMinuteOffset * m_fPixelPerMinute);
					fPoint.Offset (x_point_offset,y_point_offset);
					sPoint.Offset (x_point_offset,y_point_offset);
					ptFirst = fPoint;
					ptSecond = sPoint;
				} while(0);
				if (NULL == rec) continue;

				for(jj = 0;jj < m_vecSchdTrains.size();jj++)
				{
					if(m_vecSchdTrains[jj].GetTrainIndex() == rec->m_data.special_train_index){
						pFirstTrain = &m_vecSchdTrains[jj];
						break;
					}
				}

				do 
				{
					if (NULL == pFirstTrain) continue;
					nRecNum = pFirstTrain ->GetTrainRecordCount() ;		
					for(ii = 0;ii < nRecNum;ii++)
					{
						rec = pFirstTrain->GetTrainRecord(ii);
						if(IsFlagDivide(rec->m_data.flag))
							break;
					}

					if(ii >= nRecNum) continue;

					long time_depart,date_depart,time_arrival,date_arrival;
					time_arrival = minute_time(first->m_data.arrive,&date_arrival);
					if(first->m_data.depart == NOTIME || first->m_data.depart == 0)
					{
						date_depart = date_arrival;
						time_depart = time_arrival;
					}
					else
					{
						time_depart = minute_time(first->m_data.depart,&date_depart);
					}
					long time_depart_rec,date_depart_rec,time_arrival_rec,date_arrival_rec;
					time_arrival_rec = minute_time(rec->m_data.arrive,&date_arrival_rec);
					if(rec->m_data.depart == NOTIME || rec->m_data.depart == 0)
					{
						date_depart_rec = date_arrival_rec;
						time_depart_rec = time_arrival_rec;
					}
					else
					{
						time_depart_rec = minute_time(rec->m_data.depart,&date_depart_rec);
					}
					flFirstMinuteOffset = GetMinuteWidthToPlotStartTime(time_depart/60,date_depart,isPrint,m_nSchdStatus);
					flSecondMinuteOffset = GetMinuteWidthToPlotStartTime(time_depart_rec/60,date_depart_rec,isPrint,m_nSchdStatus);
					CPoint fPoint,sPoint;
					if(nDirectFlag == 2){// down direct
						fPoint.y = sPoint.y = first_bottom_exit  + 4;
					}else{
						fPoint.y = sPoint.y = first_top_exit - 4;
					}
					fPoint.x = (int)(flFirstMinuteOffset * m_fPixelPerMinute);
					sPoint.x = (int)(flSecondMinuteOffset * m_fPixelPerMinute);
					fPoint.Offset (x_point_offset,y_point_offset);
					sPoint.Offset (x_point_offset,y_point_offset);
					ptThird = sPoint;
				} while(0);
			}

			if (i == 0)						//如果是分解后的列车，有可能是分解后的第一列车，也有可能是分解后的第二列车
			{	
				for(jj = 0;jj < m_vecSchdTrains.size();jj++)
				{
					if(m_vecSchdTrains[jj].GetTrainIndex() == first->m_data.special_train_index){
						pFirstTrain = &m_vecSchdTrains[jj];
						break;
					}
				}
				do 
				{
					if (NULL == pFirstTrain) continue;
					nRecNum = pFirstTrain ->GetTrainRecordCount() ;		
					for(ii = 0;ii < nRecNum;ii++)
					{
						rec = pFirstTrain->GetTrainRecord(ii);
						if(IsFlagDivide(rec->m_data.flag))
							break;
					}

					if(ii >= nRecNum) continue;

					if (ii == 0)						//如果是分解后的第二列车
					{
						long time_depart,date_depart,time_arrival,date_arrival;
						time_arrival = minute_time(first->m_data.arrive,&date_arrival);
						if(first->m_data.depart == NOTIME || first->m_data.depart == 0)
						{
							date_depart = date_arrival;
							time_depart = time_arrival;
						}
						else
						{
							time_depart = minute_time(first->m_data.depart,&date_depart);
						}
						long time_depart_rec,date_depart_rec,time_arrival_rec,date_arrival_rec;
						time_arrival_rec = minute_time(rec->m_data.arrive,&date_arrival_rec);
						if(rec->m_data.depart == NOTIME || rec->m_data.depart == 0)
						{
							date_depart_rec = date_arrival_rec;
							time_depart_rec = time_arrival_rec;
						}
						else
						{
							time_depart_rec = minute_time(rec->m_data.depart,&date_depart_rec);
						}
						flFirstMinuteOffset = GetMinuteWidthToPlotStartTime(time_depart/60,date_depart,isPrint,m_nSchdStatus);
						flSecondMinuteOffset = GetMinuteWidthToPlotStartTime(time_depart_rec/60,date_depart_rec,isPrint,m_nSchdStatus);
						CPoint fPoint,sPoint;
						if(nDirectFlag == 2){// down direct
							fPoint.y = sPoint.y = first_bottom_exit  + 4;
						}else{
							fPoint.y = sPoint.y = first_top_exit - 4;
						}
						fPoint.x = (int)(flFirstMinuteOffset * m_fPixelPerMinute);
						sPoint.x = (int)(flSecondMinuteOffset * m_fPixelPerMinute);
						fPoint.Offset (x_point_offset,y_point_offset);
						sPoint.Offset (x_point_offset,y_point_offset);
						ptSecond = fPoint;
						ptThird = sPoint;

						for(jj = 0;jj < m_vecSchdTrains.size();jj++)
						{
							if(m_vecSchdTrains[jj].GetTrainIndex() == rec->m_data.special_train_index){
								pFirstTrain = &m_vecSchdTrains[jj];
								break;
							}
						}
						if(NULL != pFirstTrain)
						{
							nRecNum = pFirstTrain ->GetTrainRecordCount() ;
							for(ii = 0;ii < nRecNum;ii++)
							{
								rec = pFirstTrain->GetTrainRecord(ii);
								if(IsFlagDivide(rec->m_data.flag))
									break;
							}
							long time_depart,date_depart,time_arrival,date_arrival;
							time_arrival = minute_time(first->m_data.arrive,&date_arrival);
							if(first->m_data.depart == NOTIME || first->m_data.depart == 0)
							{
								date_depart = date_arrival;
								time_depart = time_arrival;
							}
							else
							{
								time_depart = minute_time(first->m_data.depart,&date_depart);
							}
							long time_depart_rec,date_depart_rec,time_arrival_rec,date_arrival_rec;
							time_arrival_rec = minute_time(rec->m_data.arrive,&date_arrival_rec);
							if(rec->m_data.depart == NOTIME || rec->m_data.depart == 0)
							{
								date_depart_rec = date_arrival_rec;
								time_depart_rec = time_arrival_rec;
							}
							else
							{
								time_depart_rec = minute_time(rec->m_data.depart,&date_depart_rec);
							}
							flFirstMinuteOffset = GetMinuteWidthToPlotStartTime(time_depart/60,date_depart,isPrint,m_nSchdStatus);
							flSecondMinuteOffset = GetMinuteWidthToPlotStartTime(time_depart_rec/ 60,date_depart_rec,isPrint,m_nSchdStatus);
							CPoint fPoint,sPoint;
							if(nDirectFlag == 2){// down direct
								fPoint.y = sPoint.y = first_bottom_exit  + 4;
							}else{
								fPoint.y = sPoint.y = first_top_exit - 4;
							}
							fPoint.x = (int)(flFirstMinuteOffset * m_fPixelPerMinute);
							sPoint.x = (int)(flSecondMinuteOffset * m_fPixelPerMinute);
							fPoint.Offset (x_point_offset,y_point_offset);
							sPoint.Offset (x_point_offset,y_point_offset);
							ptFirst = sPoint;
						}
					} 
					else if (ii == nRecNum - 1)						//如果是被分解列车
					{
						long time_depart,date_depart,time_arrival,date_arrival;
						time_arrival = minute_time(first->m_data.arrive,&date_arrival);
						if(first->m_data.depart == NOTIME || first->m_data.depart == 0)
						{
							date_depart = date_arrival;
							time_depart = time_arrival;
						}
						else
						{
							time_depart = minute_time(first->m_data.depart,&date_depart);
						}
						long time_depart_rec,date_depart_rec,time_arrival_rec,date_arrival_rec;
						time_arrival_rec = minute_time(rec->m_data.arrive,&date_arrival_rec);
						if(rec->m_data.depart == NOTIME || rec->m_data.depart == 0)
						{
							date_depart_rec = date_arrival_rec;
							time_depart_rec = time_arrival_rec;
						}
						else
						{
							time_depart_rec = minute_time(rec->m_data.depart,&date_depart_rec);
						}
						flFirstMinuteOffset = GetMinuteWidthToPlotStartTime(time_depart/60,date_depart,isPrint,m_nSchdStatus);
						flSecondMinuteOffset = GetMinuteWidthToPlotStartTime(time_depart_rec/60,date_depart_rec,isPrint,m_nSchdStatus);
						CPoint fPoint,sPoint;
						if(nDirectFlag == 2){// down direct
							fPoint.y = sPoint.y = first_bottom_exit  + 4;
						}else{
							fPoint.y = sPoint.y = first_top_exit - 4;
						}
						fPoint.x = (int)(flFirstMinuteOffset * m_fPixelPerMinute);
						sPoint.x = (int)(flSecondMinuteOffset * m_fPixelPerMinute);
						fPoint.Offset (x_point_offset,y_point_offset);
						sPoint.Offset (x_point_offset,y_point_offset);
						ptThird = fPoint;
						ptFirst = sPoint;

						for(jj = 0;jj < m_vecSchdTrains.size();jj++)
						{
							if(m_vecSchdTrains[jj].GetTrainIndex() == rec->m_data.special_train_index){
								pFirstTrain = &m_vecSchdTrains[jj];
								break;
							}
						}
						if(NULL != pFirstTrain)
						{
							nRecNum = pFirstTrain ->GetTrainRecordCount() ;
							for(ii = 0;ii < nRecNum;ii++)
							{
								rec = pFirstTrain->GetTrainRecord(ii);
								if(IsFlagDivide(rec->m_data.flag))
									break;
							}

							long time_depart,date_depart,time_arrival,date_arrival;
							time_arrival = minute_time(first->m_data.arrive,&date_arrival);
							if(first->m_data.depart == NOTIME || first->m_data.depart == 0)
							{
								date_depart = date_arrival;
								time_depart = time_arrival;
							}
							else
							{
								time_depart = minute_time(first->m_data.depart,&date_depart);
							}
							long time_depart_rec,date_depart_rec,time_arrival_rec,date_arrival_rec;
							time_arrival_rec = minute_time(rec->m_data.arrive,&date_arrival_rec);
							if(rec->m_data.depart == NOTIME || rec->m_data.depart == 0)
							{
								date_depart_rec = date_arrival_rec;
								time_depart_rec = time_arrival_rec;
							}
							else
							{
								time_depart_rec = minute_time(rec->m_data.depart,&date_depart_rec);
							}
							flFirstMinuteOffset = GetMinuteWidthToPlotStartTime(time_depart/60,date_depart,isPrint,m_nSchdStatus);
							flSecondMinuteOffset = GetMinuteWidthToPlotStartTime(time_depart_rec/60,date_depart_rec,isPrint,m_nSchdStatus);
							CPoint fPoint,sPoint;
							if(nDirectFlag == 2){// down direct
								fPoint.y = sPoint.y = first_bottom_exit  + 4;
							}else{
								fPoint.y = sPoint.y = first_top_exit - 4;
							}
							fPoint.x = (int)(flFirstMinuteOffset * m_fPixelPerMinute);
							sPoint.x = (int)(flSecondMinuteOffset * m_fPixelPerMinute);
							fPoint.Offset (x_point_offset,y_point_offset);
							sPoint.Offset (x_point_offset,y_point_offset);
							ptSecond = sPoint;
						}
					}
				} while(0);
			}
			single_ptFirst = ptFirst;
			single_ptSecond = ptSecond;
			single_ptThird = ptThird;
			//end to compute ptFirst, ptSecond, ptThird

			//begin to draw the line among divided trains
			CPen mPen,*pOldPen;
			mPen.CreatePen(PS_DOT,1,RGB(128,0,255));
			pOldPen = pCDC->SelectObject(&mPen);
			do 
			{
				if((single_ptFirst.x-x_point_offset) > rangeRect.right) break;
				if((single_ptSecond.x-x_point_offset) > rangeRect.right) single_ptSecond.x = rangeRect.right + x_point_offset;
				if((single_ptThird.x-x_point_offset) > rangeRect.right) single_ptThird.x = rangeRect.right + x_point_offset;

				if((single_ptThird.x-x_point_offset) < rangeRect.left) break;
				if((single_ptFirst.x-x_point_offset) < rangeRect.left)	single_ptFirst.x = rangeRect.left + x_point_offset;
				if((single_ptSecond.x-x_point_offset) < rangeRect.left)	single_ptSecond.x = rangeRect.left + x_point_offset;

				if(single_ptFirst.x<=single_ptSecond.x)
				{

					if (m_bStopStationWaveLine)				//画波浪线
						DrawChordLine(pCDC,single_ptFirst,single_ptSecond,m_nDirectFlag_prefix);	
					else									//画直线
					{
						pCDC->MoveTo(single_ptFirst);
						pCDC->LineTo(single_ptSecond);
					}
				}
				else
				{
					if (m_bStopStationWaveLine)
						DrawChordLine(pCDC,single_ptSecond,single_ptFirst,m_nDirectFlag_prefix);
					else
					{
						pCDC->MoveTo(single_ptSecond);
						pCDC->LineTo(single_ptFirst);
					}
				}

				if(single_ptFirst.x<=single_ptThird.x)
				{
					if (m_bStopStationWaveLine)
						DrawChordLine(pCDC,single_ptFirst,single_ptThird,m_nDirectFlag_prefix);
					else
					{
						pCDC->MoveTo(single_ptFirst);
						pCDC->LineTo(single_ptThird);
					}
				}
				else
				{
					if (m_bStopStationWaveLine)
						DrawChordLine(pCDC,single_ptThird,single_ptFirst,m_nDirectFlag_prefix);
					else
					{
						pCDC->MoveTo(single_ptThird);
						pCDC->LineTo(single_ptFirst);
					}
				}
			} while(0);
			pCDC->SelectObject(pOldPen);
			mPen.DeleteObject();
			//end to draw the line among divided trains
		}
		//end of draw Divide Train

		//draw Combine Train						added by zzg in ty 2008.05.11
		//if(IsFlagUnite(first->m_data.flag))
		if(IsFlagUnitOtherTrain(first->m_data.flag))
		{
			int ii, jj, nRecNum;
			CPoint ptFirst, ptSecond, ptThird;
			CTG_TrainRecord *rec;
			CTG_SingleTrain *pFirstTrain = NULL;

			if (i == usDrawCount-1)							//如果是被组合的列车
			{
				for(jj = 0;jj < m_vecSchdTrains.size();jj++)
				{
					if(m_vecSchdTrains[jj].GetTrainIndex() == first->m_data.special_train_index){
						pFirstTrain = &m_vecSchdTrains[jj];
						break;
					}
				}
				do 
				{
					if (NULL == pFirstTrain) continue;
					nRecNum = pFirstTrain ->GetTrainRecordCount() ;		
					for(ii = 0;ii < nRecNum;ii++)
					{
						rec = pFirstTrain->GetTrainRecord(ii);
						if(IsFalgBeUnitedByOther(rec->m_data.flag))
							break;
					}

					if(ii >= nRecNum) continue;

					//绘制合并列车的虚线
					for(int combine_num = ii;combine_num<nRecNum-1;combine_num++)
					{
						CTG_TrainRecord *rec_combine1,*rec_combine2;
						rec_combine1 = pFirstTrain->GetTrainRecord(combine_num);
						rec_combine2 = pFirstTrain->GetTrainRecord(combine_num+1);
						long time_depart,date_depart,time_arrival,date_arrival;
						time_arrival = minute_time(rec_combine1->m_data.arrive,&date_arrival);
						if(rec_combine1->m_data.depart == NOTIME || rec_combine1->m_data.depart == 0)
						{
							date_depart = date_arrival;
							time_depart = time_arrival;
						}
						else
						{
							time_depart = minute_time(rec_combine1->m_data.depart,&date_depart);
						}
						long time_arrival_rec,date_arrival_rec;
						time_arrival_rec = minute_time(rec_combine2->m_data.arrive,&date_arrival_rec);
						flFirstMinuteOffset = GetMinuteWidthToPlotStartTime(time_depart/60,date_depart,isPrint,m_nSchdStatus);
						flSecondMinuteOffset = GetMinuteWidthToPlotStartTime(time_arrival_rec/60,date_arrival_rec,isPrint,m_nSchdStatus);
						CPoint fPoint,sPoint;

						if(nDirectFlag == 2)
						{// down direct
							fPoint.y = GetStationVPos(pFirstTrain,rec_combine1, BOTTOM_TRACK, EXIT_POSITION,schd_type);
							sPoint.y = GetStationVPos(pFirstTrain,rec_combine2, TOP_TRACK, ENTRY_POSITION,schd_type);
						}
						else
						{
							fPoint.y = GetStationVPos(pFirstTrain,rec_combine1, TOP_TRACK, EXIT_POSITION,schd_type);
							sPoint.y = GetStationVPos(pFirstTrain,rec_combine2, BOTTOM_TRACK, ENTRY_POSITION,schd_type);
						}
						fPoint.x = (int)(flFirstMinuteOffset * m_fPixelPerMinute)-4;
						sPoint.x = (int)(flSecondMinuteOffset * m_fPixelPerMinute)-4;
						if( sPoint.x > rangeRect.right)
							break;
						fPoint.Offset (x_point_offset,y_point_offset);
						sPoint.Offset (x_point_offset,y_point_offset);
						CPen mPen,*pOldPen;
						mPen.CreatePen(PS_DOT,1,lColor);
						pOldPen = pCDC->SelectObject(&mPen);
						pCDC->MoveTo(fPoint);
						pCDC->LineTo(sPoint);
						pCDC->SelectObject(pOldPen);
						mPen.DeleteObject();
					}

					//		if (ii == 0)							//如果single指向的列车是组合后的列车
					if(ii !=nRecNum - 1)
					{
						long time_depart,date_depart,time_arrival,date_arrival;
						time_arrival = minute_time(first->m_data.arrive,&date_arrival);
						if(first->m_data.depart == NOTIME || first->m_data.depart == 0)
						{
							date_depart = date_arrival;
							time_depart = time_arrival;
						}
						else
						{
							time_depart = minute_time(first->m_data.depart,&date_depart);
						}
						long time_depart_rec,date_depart_rec,time_arrival_rec,date_arrival_rec;
						time_arrival_rec = minute_time(rec->m_data.arrive,&date_arrival_rec);
						if(rec->m_data.depart == NOTIME || rec->m_data.depart == 0)
						{
							date_depart_rec = date_arrival_rec;
							time_depart_rec = time_arrival_rec;
						}
						else
						{
							time_depart_rec = minute_time(rec->m_data.depart,&date_depart_rec);
						}
						flFirstMinuteOffset = GetMinuteWidthToPlotStartTime(time_depart/60,date_depart,isPrint,m_nSchdStatus);
						flSecondMinuteOffset = GetMinuteWidthToPlotStartTime(time_depart_rec/60,date_depart_rec,isPrint,m_nSchdStatus);
						CPoint fPoint,sPoint;
						if(nDirectFlag == 2){// down direct
							fPoint.y = sPoint.y = first_bottom_exit  + 4;
						}else{
							fPoint.y = sPoint.y = first_top_exit - 4;
						}
						fPoint.x = (int)(flFirstMinuteOffset * m_fPixelPerMinute);
						sPoint.x = (int)(flSecondMinuteOffset * m_fPixelPerMinute);
						fPoint.Offset (x_point_offset,y_point_offset);
						sPoint.Offset (x_point_offset,y_point_offset);

						ptSecond = fPoint;
						ptThird = sPoint;

						for(jj = 0;jj < m_vecSchdTrains.size();jj++)
						{
							if(m_vecSchdTrains[jj].GetTrainIndex() == rec->m_data.special_train_index){
								pFirstTrain = &m_vecSchdTrains[jj];
								break;
							}
						}
						if(NULL != pFirstTrain)
						{
							nRecNum = pFirstTrain ->GetTrainRecordCount() ;
							for(ii = 0;ii < nRecNum;ii++)
							{
								rec = pFirstTrain->GetTrainRecord(ii);
								if(IsFlagUnitOtherTrain(rec->m_data.flag))
									break;
							}

							long time_depart,date_depart,time_arrival,date_arrival;
							time_arrival = minute_time(first->m_data.arrive,&date_arrival);
							if(first->m_data.depart == NOTIME || first->m_data.depart == 0)
							{
								date_depart = date_arrival;
								time_depart = time_arrival;
							}
							else
							{
								time_depart = minute_time(first->m_data.depart,&date_depart);
							}
							long time_depart_rec,date_depart_rec,time_arrival_rec,date_arrival_rec;
							time_arrival_rec = minute_time(rec->m_data.arrive,&date_arrival_rec);
							if(rec->m_data.depart == NOTIME || rec->m_data.depart == 0)
							{
								date_depart_rec = date_arrival_rec;
								time_depart_rec = time_arrival_rec;
							}
							else
							{
								time_depart_rec = minute_time(rec->m_data.depart,&date_depart_rec);
							}
							flFirstMinuteOffset = GetMinuteWidthToPlotStartTime(time_depart/60,date_depart,isPrint,m_nSchdStatus);
							flSecondMinuteOffset = GetMinuteWidthToPlotStartTime(time_depart_rec/60,date_depart_rec,isPrint,m_nSchdStatus);
							CPoint fPoint,sPoint;
							if(nDirectFlag == 2){// down direct
								fPoint.y = sPoint.y = first_bottom_exit  + 4;
							}else{
								fPoint.y = sPoint.y = first_top_exit - 4;
							}
							fPoint.x = (int)(flFirstMinuteOffset * m_fPixelPerMinute);
							sPoint.x = (int)(flSecondMinuteOffset * m_fPixelPerMinute);
							fPoint.Offset (x_point_offset,y_point_offset);
							sPoint.Offset (x_point_offset,y_point_offset);
							ptFirst = sPoint;
						}
					} 
					//	else if (ii == nRecNum - 1)							//如果single指向的是被组合的第一列车
					else
					{
						long time_depart,date_depart,time_arrival,date_arrival;
						time_arrival = minute_time(first->m_data.arrive,&date_arrival);
						if(first->m_data.depart == NOTIME || first->m_data.depart == 0)
						{
							date_depart = date_arrival;
							time_depart = time_arrival;
						}
						else
						{
							time_depart = minute_time(first->m_data.depart,&date_depart);
						}
						long time_depart_rec,date_depart_rec,time_arrival_rec,date_arrival_rec;
						time_arrival_rec = minute_time(rec->m_data.arrive,&date_arrival_rec);
						if(rec->m_data.depart == NOTIME || rec->m_data.depart == 0)
						{
							date_depart_rec = date_arrival_rec;
							time_depart_rec = time_arrival_rec;
						}
						else
						{
							time_depart_rec = minute_time(rec->m_data.depart,&date_depart_rec);
						}
						flFirstMinuteOffset = GetMinuteWidthToPlotStartTime(time_depart/60,date_depart,isPrint,m_nSchdStatus);
						flSecondMinuteOffset = GetMinuteWidthToPlotStartTime(date_depart_rec/60,time_depart_rec,isPrint,m_nSchdStatus);
						CPoint fPoint,sPoint;
						if(nDirectFlag == 2){// down direct
							fPoint.y = sPoint.y = first_bottom_exit  + 4;
						}else{
							fPoint.y = sPoint.y = first_top_exit - 4;
						}
						fPoint.x = (int)(flFirstMinuteOffset * m_fPixelPerMinute);
						sPoint.x = (int)(flSecondMinuteOffset * m_fPixelPerMinute);
						fPoint.Offset (x_point_offset,y_point_offset);
						sPoint.Offset (x_point_offset,y_point_offset);

						ptFirst = fPoint;
						ptSecond = sPoint;

						for(jj = 0;jj < m_vecSchdTrains.size();jj++)
						{
							if(m_vecSchdTrains[jj].GetTrainIndex() == rec->m_data.special_train_index){
								pFirstTrain = &m_vecSchdTrains[jj];
								break;
							}
						}
						if(NULL != pFirstTrain)
						{
							nRecNum = pFirstTrain ->GetTrainRecordCount() ;
							for(ii = 0;ii < nRecNum;ii++)
							{
								rec = pFirstTrain->GetTrainRecord(ii);
								if(IsFalgBeUnitedByOther(rec->m_data.flag))
									break;
							}
							long time_depart,date_depart,time_arrival,date_arrival;
							time_arrival = minute_time(first->m_data.arrive,&date_arrival);
							if(first->m_data.depart == NOTIME || first->m_data.depart == 0)
							{
								date_depart = date_arrival;
								time_depart = time_arrival;
							}
							else
							{
								time_depart = minute_time(first->m_data.depart,&date_depart);
							}
							long time_depart_rec,date_depart_rec,time_arrival_rec,date_arrival_rec;
							time_arrival_rec = minute_time(rec->m_data.arrive,&date_arrival_rec);
							if(rec->m_data.depart == NOTIME || rec->m_data.depart == 0)
							{
								date_depart_rec = date_arrival_rec;
								time_depart_rec = time_arrival_rec;
							}
							else
							{
								time_depart_rec = minute_time(rec->m_data.depart,&date_depart_rec);
							}
							flFirstMinuteOffset = GetMinuteWidthToPlotStartTime(time_depart/60,date_depart,isPrint,m_nSchdStatus);
							flSecondMinuteOffset = GetMinuteWidthToPlotStartTime(time_depart_rec/60,date_depart_rec,isPrint,m_nSchdStatus);
							CPoint fPoint,sPoint;
							if(nDirectFlag == 2){// down direct
								fPoint.y = sPoint.y = first_bottom_exit  + 4;
							}else{
								fPoint.y = sPoint.y = first_top_exit - 4;
							}
							fPoint.x = (int)(flFirstMinuteOffset * m_fPixelPerMinute);
							sPoint.x = (int)(flSecondMinuteOffset * m_fPixelPerMinute);
							fPoint.Offset (x_point_offset,y_point_offset);
							sPoint.Offset (x_point_offset,y_point_offset);
							//2009 blm unite train
							CPen mPen,*pOldPen;
							mPen.CreatePen(PS_DOT,1,lColor);
							pOldPen = pCDC->SelectObject(&mPen);
							pCDC->MoveTo(fPoint);
							pCDC->LineTo(sPoint);
							pCDC->SelectObject(pOldPen);
							mPen.DeleteObject();

							ptThird = sPoint;
						}
					}

				} while(0);
			}

			if (i == 0)										//如果single是组合后的列车
			{
				for(jj = 0;jj < m_vecSchdTrains.size();jj++)
				{
					if(m_vecSchdTrains[jj].GetTrainIndex() == first->m_data.special_train_index){
						pFirstTrain = &m_vecSchdTrains[jj];
						break;
					}
				}
				do 
				{
					if(pFirstTrain == NULL) continue;
					nRecNum = pFirstTrain ->GetTrainRecordCount() ;		
					for(ii = 0;ii < nRecNum;ii++)
					{
						rec = pFirstTrain->GetTrainRecord(ii);
						if(IsFalgBeUnitedByOther(rec->m_data.flag))
							break;
					}

					if(ii >= nRecNum) continue;

					long time_depart,date_depart,time_arrival,date_arrival;
					time_arrival = minute_time(first->m_data.arrive,&date_arrival);
					if(first->m_data.depart == NOTIME || first->m_data.depart == 0)
					{
						date_depart = date_arrival;
						time_depart = time_arrival;
					}
					else
					{
						time_depart = minute_time(first->m_data.depart,&date_depart);
					}
					long time_depart_rec,date_depart_rec,time_arrival_rec,date_arrival_rec;
					time_arrival_rec = minute_time(rec->m_data.arrive,&date_arrival_rec);
					if(rec->m_data.depart == NOTIME || rec->m_data.depart == 0)
					{
						date_depart_rec = date_arrival_rec;
						time_depart_rec = time_arrival_rec;
					}
					else
					{
						time_depart_rec = minute_time(rec->m_data.depart,&date_depart_rec);
					}
					flFirstMinuteOffset = GetMinuteWidthToPlotStartTime(time_depart/60,date_depart,isPrint,m_nSchdStatus);
					flSecondMinuteOffset = GetMinuteWidthToPlotStartTime(time_depart_rec/60,date_depart_rec,isPrint,m_nSchdStatus);
					CPoint fPoint,sPoint;
					if(nDirectFlag == 2){// down direct
						fPoint.y = sPoint.y = first_bottom_exit  + 4;
					}else{
						fPoint.y = sPoint.y = first_top_exit - 4;
					}
					fPoint.x = (int)(flFirstMinuteOffset * m_fPixelPerMinute);
					sPoint.x = (int)(flSecondMinuteOffset * m_fPixelPerMinute);
					fPoint.Offset (x_point_offset,y_point_offset);
					sPoint.Offset (x_point_offset,y_point_offset);
					ptThird = fPoint;
					ptFirst = sPoint;
				} while(0);

				if (NULL == rec) continue;

				for(jj = 0;jj < m_vecSchdTrains.size();jj++)
				{
					if(m_vecSchdTrains[jj].GetTrainIndex() == rec->m_data.special_train_index){
						pFirstTrain = &m_vecSchdTrains[jj];
						break;
					}
				}

				do 
				{
					if (NULL == pFirstTrain) continue;
					nRecNum = pFirstTrain ->GetTrainRecordCount() ;		
					for(ii = 0;ii < nRecNum;ii++)
					{
						rec = pFirstTrain->GetTrainRecord(ii);
						if(IsFalgBeUnitedByOther(rec->m_data.flag))
							break;
					}

					if(ii >= nRecNum) continue;

					long time_depart,date_depart,time_arrival,date_arrival;
					time_arrival = minute_time(first->m_data.arrive,&date_arrival);
					if(first->m_data.depart == NOTIME || first->m_data.depart == 0)
					{
						date_depart = date_arrival;
						time_depart = time_arrival;
					}
					else
					{
						time_depart = minute_time(first->m_data.depart,&date_depart);
					}
					long time_depart_rec,date_depart_rec,time_arrival_rec,date_arrival_rec;
					time_arrival_rec = minute_time(rec->m_data.arrive,&date_arrival_rec);
					if(rec->m_data.depart == NOTIME || rec->m_data.depart == 0)
					{
						date_depart_rec = date_arrival_rec;
						time_depart_rec = time_arrival_rec;
					}
					else
					{
						time_depart_rec = minute_time(rec->m_data.depart,&date_depart_rec);
					}
					flFirstMinuteOffset = GetMinuteWidthToPlotStartTime(time_depart/60,date_depart,isPrint,m_nSchdStatus);
					flSecondMinuteOffset = GetMinuteWidthToPlotStartTime(time_depart_rec/ 60,date_depart_rec,isPrint,m_nSchdStatus);
					CPoint fPoint,sPoint;
					if(nDirectFlag == 2){// down direct
						fPoint.y = sPoint.y = first_bottom_exit  + 4;
					}else{
						fPoint.y = sPoint.y = first_top_exit - 4;
					}
					fPoint.x = (int)(flFirstMinuteOffset * m_fPixelPerMinute);
					sPoint.x = (int)(flSecondMinuteOffset * m_fPixelPerMinute);
					fPoint.Offset (x_point_offset,y_point_offset);
					sPoint.Offset (x_point_offset,y_point_offset);
					ptSecond = sPoint;
				} while(0);
			}
			single_ptFirst = ptFirst;
			single_ptSecond = ptSecond;
			single_ptThird = ptThird;
			//end to compute ptFirst, ptSecond, ptThird

			//begin to draw the line among combined trains
			CPen mPen,*pOldPen;
			//		mPen.CreatePen(PS_SOLID,1,RGB(128,0,255));
			mPen.CreatePen(PS_DOT,1,lColor);
			pOldPen = pCDC->SelectObject(&mPen);
			do 
			{
				if((single_ptFirst.x-x_point_offset) > rangeRect.right) break;
				if((single_ptThird.x-x_point_offset) > rangeRect.right) single_ptThird.x = rangeRect.right + x_point_offset;

				if((single_ptThird.x-x_point_offset) < rangeRect.left) break;
				if((single_ptFirst.x-x_point_offset) < rangeRect.left)	single_ptFirst.x = rangeRect.left + x_point_offset;
				if((single_ptSecond.x-x_point_offset) < rangeRect.left)	single_ptSecond.x = rangeRect.left + x_point_offset;

				if(single_ptFirst.x<=single_ptThird.x)
				{
					if (m_bStopStationWaveLine)				//画波浪线
						DrawChordLine(pCDC,single_ptFirst,single_ptThird,m_nDirectFlag_prefix);	
					else									//画直线
					{
						pCDC->MoveTo(single_ptFirst);
						pCDC->LineTo(single_ptThird);
					}
				}
				else
				{
					if (m_bStopStationWaveLine)
						DrawChordLine(pCDC,single_ptThird,single_ptFirst,m_nDirectFlag_prefix);
					else
					{
						pCDC->MoveTo(single_ptThird);
						pCDC->LineTo(single_ptFirst);
					}
				}

				if(single_ptSecond.x<=single_ptThird.x)
				{
					if (m_bStopStationWaveLine)
						DrawChordLine(pCDC,single_ptSecond,single_ptThird,m_nDirectFlag_prefix);
					else
					{
						pCDC->MoveTo(single_ptSecond);
						pCDC->LineTo(single_ptThird);
					}
				}
				else
				{
					if (m_bStopStationWaveLine)
						DrawChordLine(pCDC,single_ptThird,single_ptSecond,m_nDirectFlag_prefix);
					else
					{
						pCDC->MoveTo(single_ptThird);
						pCDC->LineTo(single_ptSecond);
					}
				}
			} while(0);
			pCDC->SelectObject(pOldPen);
			mPen.DeleteObject();
		}
		//end of draw Combine Train
		// draw train other trainid
		if(mPlotStyle.train_id>0 && NULL != first)
		{
			DrawMutiTrainSeparate(pCDC, single, first, lColor, lActualColor);
		}
		//draw train start,accept,terminal,giveout,etc mark
		if(i == 0)
		{
			int direct_flag=-1; // BUG!!! UMR
			if(IsFlagStopAbs(first->m_data.flag))
			{
				if(IsFlagStopAbsUp(first->m_data.flag))
					direct_flag = DOWN_MARK;
				else
					direct_flag = UP_MARK; 
			}
			else
			{
				if(nDirectFlag == 1)
					direct_flag = DOWN_MARK;
				else
					direct_flag = UP_MARK;
			}

			long time_arrival,date_arrival;
			time_arrival = minute_time(first->m_data.arrive,&date_arrival);

			flFirstMinuteOffset = GetMinuteWidthToPlotStartTime(time_arrival/60,date_arrival,isPrint,m_nSchdStatus);
			ptlStart.x =  (int)(flFirstMinuteOffset * m_fPixelPerMinute);
			if(nDirectFlag == 2)// DOWN_DIRECT
				ptlStart.y = first_top_entry;
			else
				ptlStart.y = first_bottom_entry;
			ptlStart.Offset (x_point_offset,y_point_offset);
			if(IsFlagStart(first->m_data.flag))
			{
				// 太原接入交出问题
				if(second!=NULL)
				{
					int out_index=first->GetDepartTGIndex();
					int in_index=second->GetArriveTGIndex();
					if (out_index==in_index) 
					{
						if (nDirectFlag==1)//上行车
							direct_flag = DOWN_MARK;
						else//下行车
							direct_flag = UP_MARK;
					}	
					else if(out_index>in_index)
						direct_flag = DOWN_MARK;
					else
						direct_flag = UP_MARK;
				}
				else
				{
					int flag = GetEntryExitDirection(single,first,(ushort)EXIT_POSITION,locCurrentSectionNo);
					if(flag != -1)
					{
						if(flag == UP_MARK)
							direct_flag = DOWN_MARK;
						else
							direct_flag = UP_MARK;
					}
				}
			
				
				long time_depart,date_depart,time_arrival,date_arrival;
				time_arrival = minute_time(first->m_data.arrive,&date_arrival);
				if(first->m_data.depart == NOTIME || first->m_data.depart == 0)
				{
					date_depart = date_arrival;
					time_depart = time_arrival;
				}
				else
				{
					time_depart = minute_time(first->m_data.depart,&date_depart);
				}
				flFirstMinuteOffset = GetMinuteWidthToPlotStartTime(time_depart/60,date_depart,isPrint,m_nSchdStatus);
				ptlStart.x =  (int)(flFirstMinuteOffset * m_fPixelPerMinute);		
				ptlStart.y =  (direct_flag == UP_MARK) ? first_primary_entry : first_primary_exit;
				ptlStart.Offset (x_point_offset,y_point_offset);

				if(ptlStart.x >= x_point_offset && ptlStart.x<= rangeRect.right + x_point_offset && ptlStart.y >= y_point_offset)
				{
					bool bDrawStart=true;
					if(m_bShowCzhConnectLineOnPlot)
					{
						if(single->m_nBeforeTrainIndex>0 && false==IsCRHStopStation(first->m_data.station))
						{
							CTG_SingleTrain*ptrain=GetTrain(single->m_nBeforeTrainIndex);
							if(ptrain)
							{
								CTG_TrainRecord*prec=ptrain->GetTrainLastRecord();
								if(prec!=NULL&&prec->IsTerminalRec() && prec->m_data.station==first->m_data.station)
								{
									bDrawStart=false;
									CString strTrainID=single->GetTrainID();
									if(BASIC_SCHEDULE == m_nSchdStatus)
									{
										DrawBasicTrainConnectLine(pCDC, single, first, prec, ptlStart, lActualColor, isPrint, strTrainID);
									}
									else
									{
										if(first_a_time_status)
											DrawTrainConnectLine(pCDC, single, first, prec, ptlStart, lActualColor, isPrint, strTrainID);
										else
											DrawTrainConnectLine(pCDC, single, first, prec, ptlStart, lColor, isPrint, strTrainID);
									}
								}
							}
							else
							{
								bDrawStart=false;
								if(second_d_time_status)
									DrawOutScopeConnectLine(pCDC, single, ptlStart, lActualColor, LEFT_MARK, rangeRect);
								else
									DrawOutScopeConnectLine(pCDC, single, ptlStart, lColor, LEFT_MARK, rangeRect);
							}
						}
					}
					if(bDrawStart)
					{
						if(first_a_time_status)
						{
							CString tempTrain(single->GetTrainRecord(0)->m_data.original_arrive_train_id);
							if(tempTrain.IsEmpty())
							{
								DrawMarkOfStart(pCDC,single,CString(single->GetTrainID()),ptlStart,10,direct_flag, lActualColor,TRUE,schd_type );
							}
							else
							{
								tempTrain.Empty();
								tempTrain = single->GetTrainID();
								tempTrain += "(";
								tempTrain += single->GetTrainRecord(0)->m_data.original_arrive_train_id;
								tempTrain += ")";
								DrawMarkOfStart(pCDC,single,tempTrain,ptlStart,10,direct_flag, lActualColor,TRUE,schd_type);
							}
						}
						else
						{
							CString tempTrain(single->GetTrainRecord(0)->m_data.original_arrive_train_id);
							if(tempTrain.IsEmpty())
							{
								DrawMarkOfStart(pCDC,single,CString(single->GetTrainID()),ptlStart,10,direct_flag, lColor,FALSE,schd_type );
							}
							else
							{
								tempTrain.Empty();
								tempTrain = single->GetTrainID();
								tempTrain += "(";
								tempTrain += single->GetTrainRecord(0)->m_data.original_arrive_train_id;
								tempTrain += ")";
								DrawMarkOfStart(pCDC,single,tempTrain,ptlStart,10,direct_flag, lColor,TRUE,schd_type);
							}
						}
					}
				}
				else if(BASIC_SCHEDULE == m_nSchdStatus && m_nViewTimeRange==12 && m_bShowCzhConnectLineOnPlot)
				{
					if(single->m_nBeforeTrainIndex>0 && false==IsCRHStopStation(first->m_data.station))
					{
						CTG_SingleTrain*ptrain=GetTrain(single->m_nBeforeTrainIndex);
						if(ptrain)
						{
							CTG_TrainRecord*prec=ptrain->GetTrainLastRecord();
							if(prec!=NULL&&prec->IsTerminalRec()&&prec->m_data.station==first->m_data.station)
							{
								if(first_a_time_status)
									DrawBasicTrainOutScopeConnectLine(pCDC, single, first, prec, ptlStart, lActualColor, rangeRect);
								else
									DrawBasicTrainOutScopeConnectLine(pCDC, single, first, prec, ptlStart, lColor, rangeRect);
							}
						}
					}
				}
			}
			else if(IsFlagAbsStart(first->m_data.flag) )
			{
				if(ptlStart.x<rangeRect.right + x_point_offset && ptlStart.y>= y_point_offset)
				{
					direct_flag = DOWN_MARK;
					int nDirect = UP_DIRECT;
					if(IsAbsStartStopUp(single->GetTrainInfo()->m_data))
					{
						direct_flag = UP_MARK;
						nDirect = UP_DIRECT;
					}
					else
					{	
						direct_flag = DOWN_MARK; 
						nDirect = DOWN_DIRECT;
					}

					long time_depart,date_depart,time_arrival,date_arrival;
					time_arrival = minute_time(first->m_data.arrive,&date_arrival);
					if(first->m_data.depart == NOTIME || first->m_data.depart == 0)
					{
						date_depart = date_arrival;
						time_depart = time_arrival;
					}
					else
					{
						time_depart = minute_time(first->m_data.depart,&date_depart);
					}
					flFirstMinuteOffset = GetMinuteWidthToPlotStartTime(time_arrival/60,date_arrival,isPrint,m_nSchdStatus);
					long stop_abs_time = (single->GetTrainInfo()->m_data.adjust_flag&0xffff0000);
					stop_abs_time = stop_abs_time>>16;
					ptlStart.x =(int)(flFirstMinuteOffset - stop_abs_time)  * m_fPixelPerMinute;
					ptlStart.y = GetAbsVPos(single,first,0, ENTRY_POSITION,nDirect,m_nSchdStatus); 

					ptlEnd.x = flFirstMinuteOffset * m_fPixelPerMinute;
					ptlEnd.y = GetStationVPos(single,first,PRIMARY_TRACK,ENTRY_POSITION,m_nSchdStatus);

					ptlStart.Offset (x_point_offset,y_point_offset);
					ptlEnd.Offset (x_point_offset,y_point_offset);

					if(first_a_time_status)
					{
						if(IsAbsAttainEngineReturn(single->GetTrainInfo()->m_data.adjust_flag))
						{
						}
						else
						{
						CString tempTrain(single->GetTrainRecord(0)->m_data.original_arrive_train_id);
						if(tempTrain.IsEmpty())
						{
							DrawMarkOfStart(pCDC,single,CString(single->GetTrainID()),ptlStart,10,
								direct_flag,lActualColor,TRUE,schd_type );
						}
						else
						{
							tempTrain.Empty();
							tempTrain = single->GetTrainID();
							tempTrain += "(";
							tempTrain += single->GetTrainRecord(0)->m_data.original_arrive_train_id;
							tempTrain += ")";
							DrawMarkOfStart(pCDC,single,tempTrain,ptlStart,10,direct_flag,lActualColor,TRUE,schd_type);
						}
						}

						DrawActualLine(pCDC, ptlStart, ptlEnd, single->GetTrainInfo()->m_data.type,FALSE,m_nCountOfEngine);
					}
					else
					{
						if(IsAbsAttainEngineReturn(single->GetTrainInfo()->m_data.adjust_flag))
						{
						}
						else
						{
						CString tempTrain(single->GetTrainRecord(0)->m_data.original_arrive_train_id);
						if(tempTrain.IsEmpty())
						{ 
							DrawMarkOfStart(pCDC,single,CString(single->GetTrainID()),
								ptlStart,10,direct_flag,lColor,FALSE,schd_type );
						}
						else
						{
							tempTrain.Empty();
							tempTrain = single->GetTrainID();
							tempTrain += "(";
							tempTrain += single->GetTrainRecord(0)->m_data.original_arrive_train_id;
							tempTrain += ")";
							DrawMarkOfStart(pCDC,single,tempTrain,ptlStart,10,direct_flag,lColor,FALSE,schd_type);
						}
						}
						DrawActualLine(pCDC, ptlStart, ptlEnd, single->GetTrainInfo()->m_data.type,FALSE,m_nCountOfEngine, 0,FALSE);
					}

					CFont* pOldFont, font;
					LOGFONT stFont;
					memset(&stFont,0,sizeof(stFont));
					stFont.lfHeight = MulDiv(m_font.time_char,m_nLogPixelSy,72); // time_char
					stFont.lfWeight = FW_NORMAL;
					stFont.lfClipPrecision = CLIP_LH_ANGLES;
					strcpy(stFont.lfFaceName,"Times New Roman");
					font.CreateFontIndirect(&stFont);

					int text_align;
					UINT nOldColor, nColor;

					pOldFont = pCDC->SelectObject(&font);
					if(manualRealColor>0 && TG_IsDepartActualByManual(first->m_data))
					{
						nColor = manualRealColor;
					}
					else
					{
						if(first_a_time_status == 1)
							nColor = lActualColor;
						else
							nColor = lColor;
					}
					
					nOldColor = pCDC->SetTextColor(nColor);

					CPoint point = ptlStart;
					
					if(nDirect  == UP_DIRECT) //下行
						text_align = pCDC->SetTextAlign(TA_RIGHT|TA_TOP);
					else
						text_align = pCDC->SetTextAlign(TA_RIGHT|TA_BOTTOM);	

					int dlt = ((int)flFirstMinuteOffset - stop_abs_time)%10;
					if(dlt<0)
						dlt=0;
					sprintf(time_char, "%d",  dlt);
					time_char[1]=0;
					if(point.x >0 && point.x < rangeRect.right && !IsAbsAttainEngineReturn(single->GetTrainInfo()->m_data.adjust_flag))				
					{
						pCDC->TextOut((int)point.x, point.y, time_char, 1); 
					}

					pCDC->SetTextColor(nOldColor);
				}
			}
			else
			{  
				if(ptlStart.x >= x_point_offset && ptlStart.x<= rangeRect.right + x_point_offset && ptlStart.y >= y_point_offset)
				{
					// 太原接入交出问题
					if(second!=NULL)
					{
						int out_index=first->GetDepartTGIndex();
						int in_index=second->GetArriveTGIndex();
						if (out_index==in_index) 
						{
							if (nDirectFlag==1)//上行车
								direct_flag = DOWN_MARK;
							else//下行车
								direct_flag = UP_MARK;
						}
						else if(out_index>in_index)
							direct_flag = DOWN_MARK;
						else
							direct_flag = UP_MARK;
					}
					else
					{
						int flag = GetEntryExitDirection(single,first,(ushort)ENTRY_POSITION,locCurrentSectionNo,WORK_SCHEDULE,direct_flag);//yuzhu 20110529 处理西安局包西一台张桥打印问题
						if(flag != -1)
							direct_flag = flag;
					}

					ptlStart.y = (direct_flag == UP_MARK ?  first_top_entry : first_bottom_entry);
					ptlStart.Offset (0,y_point_offset);
					if(first_a_time_status)
					{
						CString tempTrain(single->GetTrainRecord(0)->m_data.original_arrive_train_id);
						if(tempTrain.IsEmpty())
						{
							tempTrain = single->GetTrainID();
							DrawMarkOfArrive(pCDC,single,tempTrain,ptlStart,10,direct_flag,lActualColor,true,schd_type,0);
						}
						else
						{
							tempTrain.Empty();
							tempTrain = single->GetTrainID();
							tempTrain += "(";
							tempTrain += single->GetTrainRecord(0)->m_data.original_arrive_train_id;
							tempTrain += ")";
							DrawMarkOfArrive(pCDC,single,tempTrain,ptlStart,10,direct_flag,lActualColor,true,schd_type,0);
						}
					}
					else
					{
						CString tempTrain(single->GetTrainRecord(0)->m_data.original_arrive_train_id);
						if(tempTrain.IsEmpty())
						{
							tempTrain = single->GetTrainID();
							DrawMarkOfArrive(pCDC,single,tempTrain,ptlStart,10,direct_flag,lColor,false,schd_type,0);
						}
						else
						{
							tempTrain.Empty();
							tempTrain = single->GetTrainID();
							tempTrain += "(";
							tempTrain += single->GetTrainRecord(0)->m_data.original_arrive_train_id;
							tempTrain += ")";
							DrawMarkOfArrive(pCDC,single,tempTrain,ptlStart,10,direct_flag,lColor,false,schd_type,0);
						}
					}
				}
			}

			//one train record
			if(usDrawCount == 1)
			{
				int direct_flag=-1;
				if(IsFlagStopAbs(first->m_data.flag))
				{
					if(IsFlagStopAbsUp(first->m_data.flag))
						direct_flag = DOWN_MARK;
					else
						direct_flag = UP_MARK; 
				}
				else
				{
					if(nDirectFlag == 1)
						direct_flag = UP_MARK;
					else
						direct_flag = DOWN_MARK;

				}

				long time_depart,date_depart,time_arrival,date_arrival;
				time_arrival = minute_time(first->m_data.arrive,&date_arrival);
				if(first->m_data.depart == NOTIME || first->m_data.depart == 0)
				{
					date_depart = date_arrival;
					time_depart = time_arrival;
				}
				else
				{
					time_depart = minute_time(first->m_data.depart,&date_depart);
				}
				flFirstMinuteOffset = GetMinuteWidthToPlotStartTime(time_depart/60,date_depart,isPrint,m_nSchdStatus);
				if(IsFlagAbsTerminal(first->m_data.flag))
				{
					WORD nDirect = UP_DIRECT;
					if(IsFlagStopAbsUp(first->m_data.flag))
					{
						direct_flag = UP_MARK;
						nDirect = UP_DIRECT;
					}
					else
					{
						direct_flag = DOWN_MARK;
						nDirect = DOWN_DIRECT;
					}

					flFirstMinuteOffset = GetMinuteWidthToPlotStartTime(time_depart/60,date_depart,isPrint,m_nSchdStatus);
					ptlStart.x = flFirstMinuteOffset * m_fPixelPerMinute;
					ptlStart.y = GetStationVPos(single,first,PRIMARY_TRACK,EXIT_POSITION,m_nSchdStatus);
					ptlEnd.x = (flFirstMinuteOffset + first->m_data.special_time_1)  * m_fPixelPerMinute;
					ptlEnd.y = GetAbsVPos(single,first,0, EXIT_POSITION,IsFlagStopAbsUp(first->m_data.flag)?UP_DIRECT:DOWN_DIRECT,m_nSchdStatus); 
					
					int dlt = (ptlEnd.y - ptlStart.y)*2/3;
					if(IsAbsDrawUp(first->m_data.stop_reason))
					{
						ptlEnd.y = ptlStart.y + dlt;
					}
					else if(IsAbsDrawBottom(first->m_data.stop_reason))
					{
						ptlEnd.y = ptlStart.y + dlt*2;
					}

					ptlStart.Offset (x_point_offset,y_point_offset);
					ptlEnd.Offset (x_point_offset,y_point_offset);

					if(ptlStart.x<m_nPageWidth && ptlStart.y > 0)
					{
						if(first_d_time_status)
						{
							DrawMarkOfEnd(pCDC,single,single->GetTrainID(),ptlEnd,20,direct_flag,lActualColor,first->m_data.flag,true);
							DrawActualLine(pCDC, ptlStart, ptlEnd, single->GetTrainInfo()->m_data.type,FALSE,m_nCountOfEngine);
						}
						else
						{
							DrawMarkOfEnd(pCDC,single,single->GetTrainID(),ptlEnd,20,direct_flag,lColor,first->m_data.flag,false);
							DrawActualLine(pCDC, ptlStart, ptlEnd, single->GetTrainInfo()->m_data.type,FALSE,m_nCountOfEngine, FALSE);

						}
					}
				}
				else if(IsFlagTerminal(first->m_data.flag))  //区间终到
				{
					int flag = GetEntryExitDirection(single,first,(ushort)ENTRY_POSITION,locCurrentSectionNo);
					if(flag != -1)
					{
						if(flag == UP_MARK)
							direct_flag = DOWN_MARK;
						else
							direct_flag = UP_MARK;
					}
					if( IsFlagAbsTerminal(first->m_data.flag))
					{	
						flag = GetEntryExitDirection(single,first,(ushort)EXIT_POSITION,locCurrentSectionNo,schd_type);
						direct_flag = flag;
					}

					long time_arrival,date_arrival;
					time_arrival = minute_time(first->m_data.arrive,&date_arrival);
					flFirstMinuteOffset = GetMinuteWidthToPlotStartTime(time_arrival/60,date_arrival,isPrint,m_nSchdStatus); 

					ptlStart.x =  (int)(flFirstMinuteOffset * m_fPixelPerMinute); 			
					ptlStart.y =  first_primary_entry;
					//stop abs mark
					if(IsFlagStopAbs(first->m_data.flag))
					{
						long time_depart,date_depart,time_arrival,date_arrival;
						time_arrival = minute_time(first->m_data.arrive,&date_arrival);
						if(first->m_data.depart == NOTIME || first->m_data.depart == 0)
						{
							date_depart = date_arrival;
							time_depart = time_arrival;
						}
						else
						{
							time_depart = minute_time(first->m_data.depart,&date_depart);
						}
						flFirstMinuteOffset = GetMinuteWidthToPlotStartTime(time_depart/60,date_depart,isPrint,schd_type);			
						ptlStart.x =(int)(flFirstMinuteOffset + first->m_data.special_time_1)  * m_fPixelPerMinute;
						ptlStart.y = GetAbsVPos(single,first,0, EXIT_POSITION,IsFlagStopAbsUp(first->m_data.flag)?UP_DIRECT:DOWN_DIRECT,schd_type);//will
					}
					if(IsFlagAbsStart(first->m_data.flag))
					{
						ptlStart.x =  (int)(flFirstMinuteOffset * m_fPixelPerMinute);
						ptlStart.y = (direct_flag == UP_MARK ?  first_top_exit : first_bottom_exit);
					}
					ptlStart.Offset (x_point_offset,y_point_offset);

					if(ptlStart.x >= x_point_offset && ptlStart.x<= rangeRect.right + x_point_offset && ptlStart.y >=y_point_offset)
					{
						if(first_a_time_status)
							DrawMarkOfEnd(pCDC,single,single->GetTrainID(),ptlStart,20,direct_flag,lActualColor,first->m_data.flag,true);
						else
							DrawMarkOfEnd(pCDC,single,single->GetTrainID(),ptlStart,20,direct_flag,lColor,first->m_data.flag,false);   						 
					}
				}
				else 
				{ 
					if(ptlStart.x >= x_point_offset && ptlStart.x<= rangeRect.right + x_point_offset&& ptlStart.y >0)
					{
						int flag = GetEntryExitDirection(single,first,(ushort)EXIT_POSITION,locCurrentSectionNo);
						if(flag != -1)
							direct_flag = flag;

						flFirstMinuteOffset = GetMinuteWidthToPlotStartTime(time_depart/60,date_depart,isPrint,m_nSchdStatus);
						ptlStart.x = flFirstMinuteOffset * m_fPixelPerMinute;
						ptlStart.y = first_primary_entry;
						ptlStart.Offset (x_point_offset,y_point_offset);
						if(first_a_time_status)
							DrawMarkOfDepart(pCDC,single,single->GetTrainID(),ptlStart,20,direct_flag,lActualColor,true);
						else
							DrawMarkOfDepart(pCDC,single,single->GetTrainID(),ptlStart,20,direct_flag,lColor,false);
					}
				}
			}
		}

		if(i == usDrawCount - 2)
		{
			int direct_flag=-1; 
			if(IsFlagStopAbs(first->m_data.flag) && first->m_data.station == second->m_data.station)
			{
				if(IsFlagStopAbsUp(first->m_data.flag))
					direct_flag = DOWN_MARK;
				else
					direct_flag = UP_MARK; 
			}
			else
			{
				if(nDirectFlag == 1)
					direct_flag = UP_MARK;
				else
					direct_flag = DOWN_MARK;
			}

			long time_depart,date_depart,time_arrival,date_arrival;
			time_arrival = minute_time(second->m_data.arrive,&date_arrival);
			if(second->m_data.depart == NOTIME || second->m_data.depart == 0)
			{
				date_depart = date_arrival;
				time_depart = time_arrival;
			}
			else
			{
				time_depart = minute_time(second->m_data.depart,&date_depart);
			}
			flFirstMinuteOffset = GetMinuteWidthToPlotStartTime(time_depart/60,date_depart,isPrint,m_nSchdStatus);
			ptlStart.x =  (int)(flFirstMinuteOffset * m_fPixelPerMinute);	 
			ptlStart.y = (direct_flag == UP_MARK ?  second_top_entry : second_bottom_entry);

			ptlStart.Offset (x_point_offset,y_point_offset);

			if(IsFlagAbsTerminal(second->m_data.flag))
			{
				WORD nDirect = UP_DIRECT;
				if(IsFlagStopAbsUp(second->m_data.flag))
				{
					direct_flag = UP_MARK;
					nDirect = UP_DIRECT;
				}
				else
				{
					direct_flag = DOWN_MARK;
					nDirect = DOWN_DIRECT;
				}

				flFirstMinuteOffset = GetMinuteWidthToPlotStartTime(time_depart/60,date_depart,isPrint,m_nSchdStatus);
				ptlStart.x = (flFirstMinuteOffset + second->m_data.special_time_1)  * m_fPixelPerMinute;
				ptlStart.y = GetAbsVPos(single, second, 0, EXIT_POSITION, nDirect, m_nSchdStatus); 
				int tmpY = (direct_flag == UP_MARK ?  second_top_entry : second_bottom_entry);
				int dlt = (ptlStart.y - tmpY)*2/3;
				if(IsAbsDrawUp(second->m_data.stop_reason))
				{
					ptlStart.y = tmpY + dlt;
				}
				else if(IsAbsDrawBottom(second->m_data.stop_reason))
				{
					ptlStart.y = tmpY + dlt*2;
				}

				ptlStart.Offset (x_point_offset,y_point_offset);

				if(ptlStart.x<m_nPageWidth && ptlStart.y > 0 && !IsFlagTrainWholeHelp(second->m_data.flag))
				{
					if(second_d_time_status)
						DrawMarkOfEnd(pCDC,single,single->GetTrainID(),ptlStart,20,direct_flag,lActualColor,second->m_data.flag,true);
					else
						DrawMarkOfEnd(pCDC,single,single->GetTrainID(),ptlStart,20,direct_flag,lColor,second->m_data.flag,false);
				}
			}
			else if(IsFlagTerminal(second->m_data.flag))
			{
				//终到标志不按照出入口号来判断，直接根据车在运行图方向来画，判断区间停车是为了避免折返的结束标志不对
				if(first!= NULL && second != NULL && !IsFlagStopAbs(first->m_data.flag))
				{
					if(first_primary_exit> second_primary_entry)
					{
						direct_flag = UP_MARK; 
					}
					else
					{
						direct_flag = DOWN_MARK;
					}
				}
				else
				{
					int flag = GetEntryExitDirection(single,second,(ushort)ENTRY_POSITION,locCurrentSectionNo);
					if(flag != -1){
						if(flag == UP_MARK)
							direct_flag = DOWN_MARK;
						else
							direct_flag = UP_MARK;
					}

				}

				long time_arrival,date_arrival;
				time_arrival = minute_time(second->m_data.arrive,&date_arrival);
				flFirstMinuteOffset = GetMinuteWidthToPlotStartTime(time_arrival/60,date_arrival,isPrint,m_nSchdStatus);

				ptlStart.x =  flFirstMinuteOffset * m_fPixelPerMinute; 			
				//ptlStart.y =  (direct_flag == UP_MARK ?  second_top_entry : second_bottom_entry);
				ptlStart.y =  (direct_flag == UP_MARK ?  second_primary_entry : second_primary_exit);
				

				int nDirect = UP_DIRECT;
				if(IsFlagStopAbs(second->m_data.flag))
				{
					if(IsFlagStopAbsUp(second->m_data.flag))
						nDirect = UP_DIRECT;
					else
						nDirect = DOWN_DIRECT;

					ptlStart.x =  (flFirstMinuteOffset+second->m_data.special_time_1) * m_fPixelPerMinute;
					ptlStart.y =  GetAbsVPos(single,second,0, EXIT_POSITION,nDirect,m_nSchdStatus);
				}

				if(IsFlagRescued(second->m_data.flag))
				{
					CTG_SingleTrain* pTrain = NULL;
					for(int idx=0; idx<m_vecSchdTrains.size(); idx++)
					{
						if(m_vecSchdTrains[idx].GetTrainIndex() == second->m_data.special_train_index)
						{
							pTrain = &m_vecSchdTrains[idx];
							break;
						}
					}
					do
					{
						if(NULL == pTrain)
							break;

						CTG_TrainRecord* otherRec = NULL;
						for(int i=0; i<pTrain->GetTrainRecordCount(); i++)
						{
							CTG_TrainRecord* rec = pTrain->GetTrainRecord(i);
							if(NULL == rec)
								continue;
							if(IsFlagRescue(rec->m_data.flag))
							{
								otherRec = rec;
								break;
							}
						}

						if(NULL == otherRec || second == NULL)
							return;

						long time_arrival,date_arrival;
						time_arrival = minute_time(otherRec->m_data.arrive,&date_arrival);
						int tmpFirstMinuteOffset = GetMinuteWidthToPlotStartTime(time_arrival/60,date_arrival,isPrint,m_nSchdStatus);

						CPoint ptBegin, ptEnd;
						ptBegin = ptlStart;
						ptEnd.x = (tmpFirstMinuteOffset + otherRec->m_data.special_time_1) * m_fPixelPerMinute;
						ptEnd.y = ptBegin.y;

						ptBegin.Offset (x_point_offset,y_point_offset);
						ptEnd.Offset (x_point_offset,y_point_offset);

						//for actual time or plan time codeing   
						int first_d_time_status = 0;
						if(otherRec->m_data.adjust_status & 0x0002)
							first_d_time_status = 1;//real time
						else
							first_d_time_status = 0;// plan time 

						CPen mPen,*pOldPen;
						if(1 == first_d_time_status)
							mPen.CreatePen(PS_DOT, 1, lActualColor);
						else
							mPen.CreatePen(PS_DOT, 1, lColor);

						pOldPen = pCDC->SelectObject(&mPen);
						pCDC->MoveTo(ptBegin);  pCDC->LineTo(ptEnd);
						pCDC->SelectObject(pOldPen);

					}while(0);
				}

				ptlStart.Offset (x_point_offset,y_point_offset);
				if(ptlStart.x >= x_point_offset && ptlStart.x<= rangeRect.right + x_point_offset)
				{	
					bool bDrawTerminalFlag=true;
					if(m_bShowCzhConnectLineOnPlot)
					{
						if(single->m_nNextTrainIndex>0 && (false==IsCRHStopStation(second->m_data.station)))
						{
							bDrawTerminalFlag=false;

							CTG_SingleTrain*ptrain=GetTrain(single->m_nNextTrainIndex);
							if(ptrain==NULL)
							{
								if(second_a_time_status)
									DrawOutScopeConnectLine(pCDC, single, ptlStart, lActualColor, RIGHT_MARK, rangeRect);
								else
									DrawOutScopeConnectLine(pCDC, single, ptlStart, lColor, RIGHT_MARK, rangeRect);
							}
						}
					}
					if(bDrawTerminalFlag)
					{
						if(second_d_time_status)
							DrawMarkOfEnd(pCDC,single,single->GetTrainID(),ptlStart,20,direct_flag,lActualColor,second->m_data.flag,true);
						else
							DrawMarkOfEnd(pCDC,single,single->GetTrainID(),ptlStart,20,direct_flag,lColor,second->m_data.flag,false);
					}
				}
			}
			else 
			{ 
				if(ptlStart.x >= x_point_offset && ptlStart.x<= rangeRect.right + x_point_offset && ptlStart.y >= y_point_offset)
				{
					// 太原接入交出问题
					if(first!=NULL)
					{
						int out_index=first->GetDepartTGIndex();
						int in_index=second->GetArriveTGIndex();
						if (out_index==in_index) 
						{
							if (nDirectFlag==1)//上行车
								direct_flag = DOWN_MARK;
							else//下行车
								direct_flag = UP_MARK;
						}
						else if(out_index>in_index)
							direct_flag = UP_MARK;
						else
							direct_flag = DOWN_MARK;
					}
					else
					{
						int flag = GetEntryExitDirection(single,second,(ushort)EXIT_POSITION,locCurrentSectionNo,WORK_SCHEDULE,direct_flag);//yuzhu 20110529 处理西安局包西一台张桥打印问题
						if(flag != -1)
							direct_flag = flag;
					}

					if(second_d_time_status)
						DrawMarkOfDepart(pCDC,single,single->GetTrainID(),ptlStart,20,direct_flag,lActualColor,true);
					else
						DrawMarkOfDepart(pCDC,single,single->GetTrainID(),ptlStart,20,direct_flag,lColor,false);
				}
			}
		}

		if(usStartTime <= usEndTime || usStartDate <= usEndDate)  //////////////
		{	
			//有关断点跳跃的绘制： 交出符号    
			do
			{
				if(m_nTrainLeapLineFlag == 1)
					break;
				if(usDrawCount == 1) break;

				long time_depart,date_depart,time_arrival,date_arrival;
				time_arrival = minute_time(first->m_data.arrive,&date_arrival);
				if(first->m_data.depart == NOTIME || first->m_data.depart == 0)
				{
					date_depart = date_arrival;
					time_depart = time_arrival;
				}
				else
				{
					time_depart = minute_time(first->m_data.depart,&date_depart);
				}
				flSecondMinuteOffset = GetMinuteWidthToPlotStartTime(time_depart/60,date_depart,isPrint,m_nSchdStatus);

				ptlStart.x =  (int)(flFirstMinuteOffset * m_fPixelPerMinute);
				ptlEnd.x = (int)(flSecondMinuteOffset * m_fPixelPerMinute);

				if(IsFlagStart(first->m_data.flag))
					ptlStart.x = ptlEnd.x;
				if(second == NULL && IsFlagTerminal(first->m_data.flag))
					ptlEnd.x =	ptlStart.x ;

				ptlStart.y = ptlEnd.y =  first_primary_entry;
				if(ptlStart.y == -1) continue;

				int index1 = first->GetArriveTGIndex();
				int index2 = first->GetDepartTGIndex();
				if(second != NULL)
				{
					if((index1 != index2) && !(IsFlagTerminal(first->m_data.flag)||IsFlagStart(first->m_data.flag)))
					{ 
						CPoint pt_a,pt_d,pt_c; //接入，交出点
						pt_a.x = ptlEnd.x;
						pt_d.x = ptlEnd.x;
						pt_c.x = ptlStart.x;

						//绘制开车时间用
						int direct_flag = nDirectFlag; 

						//对于交出点，上行车，去顶点，下行车，去底部点
						//2005-01-26
						//	if(m_pDoc->m_nTrainLeapLineFlag == 2)	
						if(m_nTrainLeapLineFlag==2||m_nTrainLeapLineFlag==3||m_nTrainLeapLineFlag==4)
						{
							if(nDirectFlag_pre == UP_DIRECT)
								pt_d.y =  GetStationVPos(single,first,TOP_TRACK,ENTRY_POSITION,schd_type);
							else
								pt_d.y =  GetStationVPos(single,first,BOTTOM_TRACK,ENTRY_POSITION,schd_type);
						}
						else
						{
							pt_d.y =  first_primary_entry;
						}
						pt_c.y=pt_d.y;

						//交出符号
						if(nDirectFlag_pre == UP_DIRECT)
							direct_flag = UP_MARK;
						else
							direct_flag = DOWN_MARK;


						pt_d.Offset (x_point_offset,y_point_offset);  

						//绘制交出标志
						if(m_nTrainLeapLineFlag == 2)
						{
							if(pt_d.x >= x_point_offset && pt_d.x<rangeRect.right+ x_point_offset && pt_d.y > 0)
							{
								if(second_d_time_status)
									DrawMarkOfDepart(pCDC,single,single->GetTrainID(),pt_d,10,
									direct_flag,lActualColor,true);
								else
									DrawMarkOfDepart(pCDC,single,single->GetTrainID(),pt_d,10,
									direct_flag,lColor,false);					

							}
						}
						else if(m_nTrainLeapLineFlag==4)
						{
							long ctime_arrival,cdate_arrival;
							ctime_arrival = minute_time(first->m_data.arrive,&cdate_arrival);
							long Offset = GetMinuteWidthToPlotStartTime(ctime_arrival/60,cdate_arrival,isPrint,m_nSchdStatus);
							pt_c.x =  (int)(Offset * m_fPixelPerMinute);
							pt_c.Offset (x_point_offset,y_point_offset);
							if(pt_c.x >= x_point_offset && pt_c.x<rangeRect.right+ x_point_offset && pt_c.y > 0)
							{
								if(second_d_time_status)
									DrawMarkOfDepart(pCDC,single,single->GetTrainID(),pt_c,10,
									direct_flag,lActualColor,true);
								else
									DrawMarkOfDepart(pCDC,single,single->GetTrainID(),pt_c,10,
									direct_flag,lColor,false);					

							}
						}

						//if(m_nTrainLeapLineFlag==2||m_nTrainLeapLineFlag==3||m_nTrainLeapLineFlag==4)
						//{
						//	if(nDirectFlag_pre == UP_DIRECT)
							//	pt_a.y =  GetStationVPos(single,first,BOTTOM_TRACK,EXIT_POSITION,schd_type);
							//else
								//pt_a.y =  GetStationVPos(single,first,TOP_TRACK,EXIT_POSITION,schd_type);
						//}
						//else
						//{
							//pt_a.y =  first_primary_entry;
					//	}
					//	pt_c.y=pt_a.y;

						if(nDirectFlag_next == DOWN_DIRECT)
							direct_flag = UP_MARK;
						else
							direct_flag = DOWN_MARK;

						//对于接入点，上行车，去顶点，下行车，去底部点
						if(nDirectFlag_pre == UP_DIRECT)
							pt_a.y =  first_top_exit;
						else
							pt_a.y =  first_bottom_exit;

						pt_a.Offset (x_point_offset,y_point_offset);  
						//绘制接入标志
						if(m_nTrainLeapLineFlag == 2 || m_nTrainLeapLineFlag==4)
						{
							if(pt_a.x >= x_point_offset && pt_a.x<rangeRect.right + x_point_offset&& pt_a.y > 0)
							{
								CString strTrainId=first->GetDepartTID();
								if(strTrainId.IsEmpty())
									strTrainId=first->GetArriveTID();
								if(second_d_time_status)
									DrawMarkOfArrive(pCDC,single,strTrainId,pt_a,10,
									direct_flag,lActualColor,true,schd_type, first->rec_index);
								else
									DrawMarkOfArrive(pCDC,single,strTrainId,pt_a,10,
									direct_flag,lColor,false,schd_type, first->rec_index);
							}
						}

						//如果不绘制接入交出，那么绘制连线
						if(m_nTrainLeapLineFlag != 2 && m_nTrainLeapLineFlag!=4)
						{						
							if(pt_a.x >= x_point_offset && pt_a.x<rangeRect.right+ x_point_offset && pt_a.y > 0 && pt_d.y> 0)
							{
								if(first_d_time_status) //绘制实际线
								{
									CPen myPen,*pMyOldPen;
									myPen.CreatePen(PS_SOLID, scaleRate, lActualColor);
									pMyOldPen = pCDC->SelectObject(&myPen);
									pCDC->MoveTo(pt_d);
									pCDC->LineTo(pt_a);
									pCDC->SelectObject(pMyOldPen); 
								}else{
									pCDC->MoveTo(pt_d);
									pCDC->LineTo(pt_a);
								}
							}
						}

						//绘制发车时间时分
						//下行车到达点 TA_RIGHT|TA_TOP
						//上行车到达点 TA_RIGHT|TA_BOTTOM					
						if((m_nTrainLeapLineFlag!=4)&&(pt_d.x >= x_point_offset && pt_d.x<rangeRect.right+ x_point_offset && pt_d.y > 0))
						{						
							CFont* pOldFont, font;
							LOGFONT stFont;
							memset(&stFont,0,sizeof(stFont));
							stFont.lfHeight = MulDiv(m_font.time_char,m_nLogPixelSy,72); // time_char
							stFont.lfWeight = FW_NORMAL;
							stFont.lfClipPrecision = CLIP_LH_ANGLES;
							strcpy(stFont.lfFaceName,"Times New Roman");
							font.CreateFontIndirect(&stFont);

							pOldFont = pCDC->SelectObject(&font);						

							COLORREF Color = GetActualLineColor(factor);

							UINT nOldColor;
							if(manualRealColor>0 && TG_IsDepartActualByManual(first->m_data))
							{
								nOldColor = pCDC->SetTextColor(manualRealColor);
							}
							else
							{
								if(first_d_time_status == 1)
									nOldColor = pCDC->SetTextColor(Color);
							}
							CPoint point = ptlStart;
							if(nDirectFlag_pre == DOWN_DIRECT)//下行车 
							{   
								pCDC->SetTextAlign(TA_RIGHT|TA_TOP);
							}
							else
							{//up direct
								pCDC->SetTextAlign(TA_RIGHT|TA_BOTTOM);
							}

							long time_depart,date_depart,time_arrival,date_arrival;
							time_arrival = minute_time(first->m_data.arrive,&date_arrival);
							if(first->m_data.depart == NOTIME || first->m_data.depart == 0)
							{
								date_depart = date_arrival;
								time_depart = time_arrival;
							}
							else
							{
								time_depart = minute_time(first->m_data.depart,&date_depart);
							}
							itoa(time_depart/60 % 10, time_char, 10);

							pCDC->TextOut(pt_d.x,pt_d.y,time_char,1);

							if(manualRealColor>0 && TG_IsDepartActualByManual(first->m_data))
							{
								pCDC->SetTextColor(nOldColor);
							}
							else
							{
								if(first_d_time_status == 1)
									pCDC->SetTextColor(nOldColor);
							}
							
							pCDC->SelectObject(pOldFont);   

							font.DeleteObject();
						}										
					}
				}
			}while(0);

			long time_depart,date_depart,time_arrival,date_arrival;
			time_arrival = minute_time(first->m_data.arrive,&date_arrival);
			if(first->m_data.depart == NOTIME || first->m_data.depart == 0)
			{
				date_depart = date_arrival;
				time_depart = time_arrival;
			}
			else
			{
				time_depart = minute_time(first->m_data.depart,&date_depart);
			}
			flFirstMinuteOffset = GetMinuteWidthToPlotStartTime(time_arrival/60,date_arrival,isPrint,m_nSchdStatus);
			flSecondMinuteOffset = GetMinuteWidthToPlotStartTime(time_depart/60,date_depart,isPrint,m_nSchdStatus);

			CPoint tmpArl(99999,99999);
			// DOWN_DIRECT
			if(nDirectFlag == 2)
			{
				ptlStart.x =  (int)(flFirstMinuteOffset * m_fPixelPerMinute);
				ptlEnd.x = (int)(flSecondMinuteOffset * m_fPixelPerMinute);
				ushort usEntryFlag = ENTRY_POSITION;
				if(IsFlagStart(first->m_data.flag))
				{
					ptlStart.x = ptlEnd.x;
					usEntryFlag = EXIT_POSITION;
				}
				if(second == NULL && IsFlagTerminal(first->m_data.flag))
					ptlEnd.x =	ptlStart.x ;

				ptlStart.y = ptlEnd.y =  GetStationVPos(single,first,PRIMARY_TRACK,usEntryFlag,schd_type);
				if(ptlStart.y == -1) continue;

				//draw side point new added ////////////////////
				if (m_nDrawSidePoint)
				{
					if(GetStationVPos(single,first,PRIMARY_TRACK,usEntryFlag,schd_type) != 
						GetStationVPos(single,first,TOP_TRACK,usEntryFlag,schd_type))
					{					
						int i=-1;
						if ((ptlStart.x == ptlEnd.x)&&(GetNodeIndex(single,first,i,(ushort)usEntryFlag)>-1)){
							if(m_sDispDistance[i].distance.node_info.show_sides
								&&m_sDispDistance[i].distance.node_info.show_sides_mode ==0)
							{
								CPoint pt = ptlStart;
								CSize ssSize = CSize(8,8);
								int scaleRate = (fHeightMulti>5?5:fHeightMulti);

								int uuDis=4*scaleRate;
								ssSize.cx *= scaleRate;
								ssSize.cy *= scaleRate;

								CBrush tempBrush(lColor);
								CBrush *pOldBrush;
								pOldBrush = pCDC->SelectObject(&tempBrush);
								if(ptlStart.x > 0 && ptlStart.x < rangeRect.right)
								{  pt.Offset (x_point_offset,y_point_offset);
								pCDC->RoundRect(CRect(CPoint(pt.x - uuDis,pt.y - uuDis),ssSize),pt);
								}
								pCDC->SelectObject(pOldBrush);
							}
						}
					}
				}

				do
				{
					//6:00 or 18:00 position	
					if(first)
					{
						int in_index = first->GetArriveTGIndex();
						int out_index = first->GetDepartTGIndex();
						if((in_index != out_index)&&(m_nTrainLeapLineFlag==4))
						{
							break;
						}
					}
					if((ptlStart.x < 0 && ptlEnd.x < 0))
					{
						break;
					}
					else if ((ptlStart.x < 0 && ptlEnd.x >= 0 )) 
					{
						ptlStart.x = 0;
						critical_point = ptlStart;
						critical_point.Offset (x_point_offset,y_point_offset);
						//判断是否在该车站改了车次号，如果改了，使用新车次号
						NewTrainId = "";
						for(critial_record_index =0; critial_record_index<single->GetTrainRecordCount(); critial_record_index++)//查找在本站是否该了车次号
						{
							if(i == single->GetTrainRecord(critial_record_index)->m_data.station && single->GetTrainRecord(critial_record_index)->m_data.arrive_train_id != single->GetTrainRecord(critial_record_index)->m_data.depart_train_id
								&& strlen(single->GetTrainRecord(critial_record_index)->m_data.arrive_train_id)>0 && strlen(single->GetTrainRecord(critial_record_index)->m_data.depart_train_id)>0)
							{
								NewTrainId.Format("%s",single->GetTrainRecord(critial_record_index)->m_data.depart_train_id);
								break;
							}
						}

						//判断新车次号结束							
						if(NewTrainId.IsEmpty())
							NewTrainId = single->GetTrainID();

						if(first_a_time_status)
							DrawMarkOfCritical(pCDC,single,NewTrainId,critical_point,50,LEFT_MARK, lActualColor);
						else							
							DrawMarkOfCritical(pCDC,single,NewTrainId,critical_point,50,LEFT_MARK, lColor);
					}

					if((ptlStart.x > rangeRect.right) && ptlEnd.x >= rangeRect.right)
					{
						break;
					}
					else if((ptlStart.x < rangeRect.right) && ptlEnd.x >= rangeRect.right)
					{
						ptlEnd.x = rangeRect.right;
						critical_point = ptlEnd;
						critical_point.Offset (x_point_offset,y_point_offset);
						//判断是否在该车站改了车次号，如果改了，使用新车次号
						NewTrainId = "";
						//	for(critial_record_index =1; critial_record_index<single->GetTrainInfo()->train_id_count; critial_record_index++)//查找在本站是否该了车次号
						for(critial_record_index =0; critial_record_index<single->GetTrainRecordCount(); critial_record_index++)//查找在本站是否该了车次号
						{
							if(i == single->GetTrainRecord(critial_record_index)->m_data.station && single->GetTrainRecord(critial_record_index)->m_data.arrive_train_id != single->GetTrainRecord(critial_record_index)->m_data.depart_train_id
								&& strlen(single->GetTrainRecord(critial_record_index)->m_data.arrive_train_id)>0 && strlen(single->GetTrainRecord(critial_record_index)->m_data.depart_train_id)>0)
								//if(i >= single->GetTrainInfo()->begin_sta[critial_record_index] && i<= single->GetTrainInfo()->end_sta[critial_record_index])//找到了
							{
								NewTrainId.Format("%s",single->GetTrainRecord(critial_record_index)->m_data.depart_train_id);
								break;
							}
						}
						//判断新车次号结束							
						if(NewTrainId.IsEmpty())
							NewTrainId = single->GetTrainID();

						if(first_a_time_status)
							DrawMarkOfCritical(pCDC,single,NewTrainId,critical_point,50,RIGHT_MARK, lActualColor);
						else							
							DrawMarkOfCritical(pCDC,single,NewTrainId,critical_point,50,RIGHT_MARK, lColor);

					}

					if((m_nSchdStatus == WORK_SCHEDULE || m_nSchdStatus == SHIFT_SCHEDULE ||m_nSchdStatus == HISTORY_SCHEDULE) == TRUE? TRUE:(ptlEnd.x >= ptlStart.x))
					{     
						//	if(ptlEnd.x >= ptlStart.x){
						if(ptlEnd.x <= rangeRect.right)
						{
							if(first_a_time_status && first_d_time_status)
							{

								if(m_bStopStationWaveLine && JudgeNeedWaveLine(first,m_nSchdStatus,m_nDirectFlag_prefix))
									DrawActualStopStationLine(pCDC,CPoint(ptlStart.x + x_point_offset,ptlStart.y + y_point_offset),
									CPoint(ptlEnd.x+x_point_offset,ptlEnd.y+ y_point_offset),single->GetTrainInfo()->m_data.type,DOWN_DIRECT);//draw real time line
								else
									DrawActualLine(pCDC,CPoint(ptlStart.x + x_point_offset,ptlStart.y + y_point_offset),
									CPoint(ptlEnd.x+x_point_offset,ptlEnd.y+ y_point_offset),single->GetTrainInfo()->m_data.type);//single->m_carriage_count);//draw real time line

								int yStation;
								if(i == 0 && !IsFlagStart(first->m_data.flag))
								{
									yStation=first_top_entry;
									////wangjinhai added 
									int flag = GetEntryExitDirection(single,first,(ushort)ENTRY_POSITION,locCurrentSectionNo,schd_type);
									int direct_flag=-1;  // BUG!!! UMR
									if(flag != -1)
										direct_flag = flag;						   
									yStation = (direct_flag == UP_MARK ? first_top_entry : first_bottom_entry);
									////

									DrawActualLine(pCDC,CPoint(ptlStart.x + x_point_offset,yStation + y_point_offset),
										CPoint(ptlStart.x+x_point_offset,ptlStart.y+ y_point_offset),single->GetTrainInfo()->m_data.type,FALSE,m_nCountOfEngine);//single->m_carriage_count);
								}

							}
							else
							{
								//车站有停留,下行车，三条直线
								int yStation;  
								if(i == 0 && !IsFlagStart(first->m_data.flag) )
								{  ////
									yStation=first_top_entry;
									////wangjinhai added 
									int flag = GetEntryExitDirection(single,first,(ushort)ENTRY_POSITION,locCurrentSectionNo,schd_type);
									int direct_flag=-1;  // BUG!!! UMR
									if(flag != -1)
										direct_flag = flag;						   
									yStation = (direct_flag == UP_MARK ?  first_top_entry : first_bottom_entry);
									////
									
									DrawActualLine(pCDC,CPoint(ptlStart.x + x_point_offset,yStation + y_point_offset),
										CPoint(ptlStart.x+x_point_offset,ptlStart.y+ y_point_offset),single->GetTrainInfo()->m_data.type,FALSE,m_nCountOfEngine,0,FALSE);

									if(yStation != ptlStart.y)
									{
										m_pPerPointInfo[m_nPointCount].record_index = i;
										m_pPerPointInfo[m_nPointCount].start_depart_flag = ARRIVAL_POSITION;
										m_pPerPointInfo[m_nPointCount++].rect  = CRect(CPoint(ptlStart.x - uDis,yStation - uDis),sSize); ////ptlStart.y
									}
								}

								if(!(second == NULL && IsFlagTerminal(first->m_data.flag)) && !IsFlagUnitOtherTrain(first->m_data.flag) && !IsFlagEnd(first->m_data.flag))
								{
									if(m_bStopStationWaveLine && JudgeNeedWaveLine(first,schd_type,m_nDirectFlag_prefix))
										DrawStopStationLine(pCDC,CPoint(ptlStart.x+x_point_offset, ptlStart.y+y_point_offset),
										CPoint(ptlEnd.x + x_point_offset,ptlEnd.y+y_point_offset),DOWN_DIRECT);
									else
									{	
										DrawActualLine(pCDC,CPoint(ptlStart.x + x_point_offset,ptlStart.y + y_point_offset),
											CPoint(ptlEnd.x+x_point_offset,ptlEnd.y+ y_point_offset),single->GetTrainInfo()->m_data.type,0,0,0,FALSE);

									}
								}

							}

							if(usStartTime != usEndTime)
							{
								m_pPerPointInfo[m_nPointCount].record_index = i;
								m_pPerPointInfo[m_nPointCount].start_depart_flag = ARRIVAL_POSITION;
								m_pPerPointInfo[m_nPointCount++].rect  = CRect(CPoint(ptlStart.x - uDis,ptlStart.y - uDis),sSize);/////////////////ptlStart.y
							}
						}
					}
					else
					{
						//基本图的过表
						CPoint criticalstart,criticalend;

						int basicMemoryWidth = m_fPixelPerHour*24;

						criticalstart =  CPoint(basicMemoryWidth+x_point_offset,ptlEnd.y+ y_point_offset);
						criticalend = CPoint(0 + x_point_offset,ptlEnd.y + y_point_offset);

						if(first_a_time_status && first_d_time_status)
						{
							DrawActualLine(pCDC,CPoint(ptlStart.x + x_point_offset,ptlStart.y + y_point_offset),
								criticalstart,single->GetTrainInfo()->m_data.type,FALSE,m_nCountOfEngine);//single->m_carriage_count);
							DrawActualLine(pCDC,criticalend,
								CPoint(ptlEnd.x+x_point_offset,ptlEnd.y+ y_point_offset),single->GetTrainInfo()->m_data.type,FALSE,m_nCountOfEngine);

							DrawMarkOfCritical(pCDC,single,single->GetTrainID(),criticalstart,50,RIGHT_MARK, lActualColor);
							DrawMarkOfCritical(pCDC,single,single->GetTrainID(),criticalend,50,LEFT_MARK, lActualColor);

						}
						else
						{
							DrawActualLine(pCDC,CPoint(ptlStart.x + x_point_offset,ptlStart.y + y_point_offset),
								criticalstart,single->GetTrainInfo()->m_data.type,FALSE,m_nCountOfEngine,0,FALSE);
							DrawActualLine(pCDC,criticalend,
								CPoint(ptlEnd.x+x_point_offset,ptlEnd.y+ y_point_offset),single->GetTrainInfo()->m_data.type,FALSE,m_nCountOfEngine,0,FALSE);
						}
					}
				}while(0);
				
				tmpArl=ptlStart;
				ptlStart = ptlEnd;
				//point rect information
				if(second == NULL){
					m_pPerPointInfo[m_nPointCount].record_index = i;
					m_pPerPointInfo[m_nPointCount].start_depart_flag = DEPART_POSITION;
					m_pPerPointInfo[m_nPointCount++].rect  = CRect(CPoint(ptlStart.x - uDis,ptlStart.y - uDis),sSize);
				}
			}
			else
			{//UP_DIRECT
				ptlStart.x =  (int)(flFirstMinuteOffset * m_fPixelPerMinute);
				ptlEnd.x = (int)(flSecondMinuteOffset * m_fPixelPerMinute);
				ushort usEntryFlag = ENTRY_POSITION;
				if(IsFlagStart(first->m_data.flag)){
					ptlStart.x = ptlEnd.x;
					usEntryFlag = EXIT_POSITION;
				}
				if(second == NULL && IsFlagTerminal(first->m_data.flag))
					ptlEnd.x =	ptlStart.x ;

				ptlStart.y = ptlEnd.y =  GetStationVPos(single,first,PRIMARY_TRACK, usEntryFlag,schd_type);
				if(ptlEnd.y == -1) continue;

				//draw side point 放开
				if (m_nDrawSidePoint)
				{
					if(GetStationVPos(single,first,PRIMARY_TRACK,usEntryFlag,m_nSchdStatus) != 
						GetStationVPos(single,first,TOP_TRACK,usEntryFlag,m_nSchdStatus))
					{
						int i=-1;
						if ((ptlStart.x == ptlEnd.x)&&(GetNodeIndex(single,first,i,(ushort)usEntryFlag)>-1))
						{
							if(m_sDispDistance[i].distance.node_info.show_sides
								&&m_sDispDistance[i].distance.node_info.show_sides_mode ==0)
							{
								CPoint pt = ptlStart;
								CSize ssSize = CSize(8,8);
								int scaleRate = (fHeightMulti>5?5:fHeightMulti);

								int uuDis=4*scaleRate;
								ssSize.cx *= scaleRate;
								ssSize.cy *= scaleRate;

								CBrush tempBrush(lColor);
								CBrush *pOldBrush;
								pOldBrush = pCDC->SelectObject(&tempBrush);
								if(ptlStart.x > 0 && ptlStart.x < rangeRect.right)
								{  
									pt.Offset (x_point_offset,y_point_offset);
									pCDC->RoundRect(CRect(CPoint(pt.x - uuDis,pt.y - uuDis),ssSize),pt);
								}
								pCDC->SelectObject(pOldBrush);
							}
						}
					}
				}

				//6:00 or 18:00 position
				do
				{
					if(first)
					{
						int in_index = first->GetArriveTGIndex();
						int out_index = first->GetDepartTGIndex();
						if((in_index != out_index)&&(m_nTrainLeapLineFlag==4))
						{
							break;
						}
					}
					if((ptlStart.x < 0 && ptlEnd.x < 0 ))
					{
						break;
					}
					else if ((ptlStart.x < 0 && ptlEnd.x >= 0 )) 
					{
						ptlStart.x = 0;
						critical_point = ptlStart;
						critical_point.Offset (x_point_offset,y_point_offset);
						//判断是否在该车站改了车次号，如果改了，使用新车次号
						NewTrainId = "";
						for(critial_record_index =0; critial_record_index<single->GetTrainRecordCount(); critial_record_index++)//查找在本站是否该了车次号
						{
							if(i == single->GetTrainRecord(critial_record_index)->m_data.station && single->GetTrainRecord(critial_record_index)->m_data.arrive_train_id != single->GetTrainRecord(critial_record_index)->m_data.depart_train_id
								&& strlen(single->GetTrainRecord(critial_record_index)->m_data.arrive_train_id)>0 && strlen(single->GetTrainRecord(critial_record_index)->m_data.depart_train_id)>0)
							{
								NewTrainId.Format("%s",single->GetTrainRecord(critial_record_index)->m_data.depart_train_id);
								break;
							}
						}

						//判断新车次号结束							
						if(NewTrainId.IsEmpty())
							NewTrainId = single->GetTrainID();

						if(first_a_time_status)
							DrawMarkOfCritical(pCDC,single,NewTrainId,critical_point,50,LEFT_MARK, lActualColor);
						else							
							DrawMarkOfCritical(pCDC,single,NewTrainId,critical_point,50,LEFT_MARK, lColor);
					}

					if((ptlStart.x > rangeRect.right) && 
						ptlEnd.x >= rangeRect.right)
					{
						break;
					}
					else if((ptlStart.x < rangeRect.right) && 
						ptlEnd.x >= rangeRect.right)
					{
						ptlEnd.x = rangeRect.right;
						critical_point = ptlEnd;
						critical_point.Offset (x_point_offset,y_point_offset);
						//判断是否在该车站改了车次号，如果改了，使用新车次号
						NewTrainId = "";
						for(critial_record_index =0; critial_record_index<single->GetTrainRecordCount(); critial_record_index++)//查找在本站是否该了车次号
						{
							if(i == single->GetTrainRecord(critial_record_index)->m_data.station && single->GetTrainRecord(critial_record_index)->m_data.arrive_train_id != single->GetTrainRecord(critial_record_index)->m_data.depart_train_id
								&& strlen(single->GetTrainRecord(critial_record_index)->m_data.arrive_train_id)>0 && strlen(single->GetTrainRecord(critial_record_index)->m_data.depart_train_id)>0)
							{
								NewTrainId.Format("%s",single->GetTrainRecord(critial_record_index)->m_data.depart_train_id);
								break;
							}
						}

						//判断新车次号结束							
						if(NewTrainId.IsEmpty())
							NewTrainId = single->GetTrainID();

						if(first_a_time_status)
							DrawMarkOfCritical(pCDC,single,NewTrainId,critical_point,50,RIGHT_MARK, lActualColor);
						else							
							DrawMarkOfCritical(pCDC,single,NewTrainId,critical_point,50,RIGHT_MARK, lColor);

					}

					if((m_nSchdStatus == WORK_SCHEDULE|| m_nSchdStatus == SHIFT_SCHEDULE ||m_nSchdStatus == HISTORY_SCHEDULE) == TRUE? TRUE:(ptlEnd.x >= ptlStart.x))
					{

						if(ptlEnd.x <= rangeRect.right)
						{
							if(first_a_time_status && first_d_time_status)
							{
								int yStation;
								if(i == 0 && !IsFlagStart(first->m_data.flag))
								{							
									yStation = first_bottom_entry;
									DrawActualLine(pCDC,CPoint(ptlStart.x + x_point_offset,yStation + y_point_offset),
										CPoint(ptlStart.x+x_point_offset,ptlStart.y+ y_point_offset),single->GetTrainInfo()->m_data.type,FALSE,m_nCountOfEngine);//single->m_carriage_count);//draw real time line
								}
								if(m_bStopStationWaveLine && JudgeNeedWaveLine(first,schd_type,m_nDirectFlag_prefix))
									DrawActualStopStationLine(pCDC,CPoint(ptlStart.x+x_point_offset, ptlStart.y+y_point_offset),
									CPoint(ptlEnd.x + x_point_offset,ptlEnd.y+y_point_offset),single->GetTrainInfo()->m_data.type,UP_DIRECT);
								else
									DrawActualLine(pCDC,CPoint(ptlStart.x + x_point_offset,ptlStart.y + y_point_offset),
									CPoint(ptlEnd.x+x_point_offset,ptlEnd.y+ y_point_offset),single->GetTrainInfo()->m_data.type,FALSE,m_nCountOfEngine);//single->m_carriage_count);//draw real time line
							}
							else
							{
								// 车站有停留，上行车的情况，三条线
								int yStation;
								if(i == 0 && !IsFlagStart(first->m_data.flag))
								{  						
									yStation = first_bottom_entry;

									int flag = GetEntryExitDirection(single,first,(ushort)ENTRY_POSITION,locCurrentSectionNo);////,m_nSchdStatus);
									int direct_flag=-1; // // BUG!!! UMR
									if(flag != -1)
										direct_flag = flag;						   
									yStation = (direct_flag == UP_MARK ?  first_top_entry : first_bottom_entry);


									DrawActualLine(pCDC,CPoint(ptlStart.x + x_point_offset,yStation + y_point_offset),
										CPoint(ptlStart.x+x_point_offset,ptlStart.y+ y_point_offset),single->GetTrainInfo()->m_data.type,FALSE,m_nCountOfEngine,0,FALSE);

									if(yStation != ptlStart.y)
									{
										m_pPerPointInfo[m_nPointCount].record_index = i;
										m_pPerPointInfo[m_nPointCount].start_depart_flag = ARRIVAL_POSITION;
										m_pPerPointInfo[m_nPointCount++].rect  = CRect(CPoint(ptlStart.x - uDis,yStation - uDis),sSize); ////ptlStart.y
									}
								}

								if(!(second == NULL && IsFlagTerminal(first->m_data.flag)) && !IsFlagUnitOtherTrain(first->m_data.flag)&& !IsFlagEnd(first->m_data.flag))
								{
									if(m_bStopStationWaveLine && JudgeNeedWaveLine(first,schd_type,m_nDirectFlag_prefix))
										DrawStopStationLine(pCDC,CPoint(ptlStart.x+x_point_offset, ptlStart.y+y_point_offset),
										CPoint(ptlEnd.x + x_point_offset,ptlEnd.y+y_point_offset),UP_DIRECT);
									else
										DrawActualLine(pCDC,CPoint(ptlStart.x + x_point_offset,ptlStart.y + y_point_offset),
										CPoint(ptlEnd.x+x_point_offset,ptlEnd.y+ y_point_offset),single->GetTrainInfo()->m_data.type,FALSE,m_nCountOfEngine,0,FALSE);

								}
							}

							if(usStartTime != usEndTime)
							{
								m_pPerPointInfo[m_nPointCount].record_index = i;
								if(IsFlagStart(first->m_data.flag))
									m_pPerPointInfo[m_nPointCount].start_depart_flag = UNION_POSITION;
								else
									m_pPerPointInfo[m_nPointCount].start_depart_flag = ARRIVAL_POSITION;
								m_pPerPointInfo[m_nPointCount++].rect  = CRect(CPoint(ptlStart.x - uDis,ptlStart.y - uDis),sSize); ////ptlStart.y
							}
						}
					}
					else
					{
						//基本图的过表
						CPoint criticalstart,criticalend;
						int basicMemoryWidth = m_fPixelPerHour*24;

						criticalstart =  CPoint(basicMemoryWidth+x_point_offset,ptlEnd.y+ y_point_offset);
						criticalend = CPoint(0 + x_point_offset,ptlEnd.y + y_point_offset);

						if(first_a_time_status && first_d_time_status)
						{
							DrawActualLine(pCDC,CPoint(ptlStart.x + x_point_offset,ptlStart.y + y_point_offset),
								criticalstart,single->GetTrainInfo()->m_data.type,FALSE,m_nCountOfEngine);//single->m_carriage_count);//draw real time line
							DrawActualLine(pCDC,criticalend,
								CPoint(ptlEnd.x+x_point_offset,ptlEnd.y+ y_point_offset),single->GetTrainInfo()->m_data.type,FALSE,m_nCountOfEngine);//single->m_carriage_count);//draw real time line

							DrawMarkOfCritical(pCDC,single,single->GetTrainID(),criticalstart,50,RIGHT_MARK, lActualColor);
							DrawMarkOfCritical(pCDC,single,single->GetTrainID(),criticalend,50,LEFT_MARK, lActualColor);
						}
						else
						{/////
							DrawActualLine(pCDC,CPoint(ptlStart.x + x_point_offset,ptlStart.y + y_point_offset),
								criticalstart,single->GetTrainInfo()->m_data.type,FALSE,m_nCountOfEngine,0,FALSE);
							DrawActualLine(pCDC,criticalend,
								CPoint(ptlEnd.x+x_point_offset,ptlEnd.y+ y_point_offset),single->GetTrainInfo()->m_data.type,FALSE,m_nCountOfEngine,0,FALSE);
						}
					}	
				}while(0);

				tmpArl=ptlStart;
				ptlStart = ptlEnd; 
			}
			if(ptlStart.y == -1) continue; 


			// draw arrive minute text
			time_arrival = minute_time(first->m_data.arrive,&date_arrival);
			itoa(time_arrival/60 % 10, time_char, 10);

            // 直接调用函数 PtInRects, 该函数定义在plotdoc.cpp/h中
			if((PtInRects(rangeRect, ptlStart) || PtInRects(rangeRect, tmpArl)) && gpcdef.GetFlagDrawTrainTime(first->m_data.station))
			{
				CFont* pOldFont, font;
				LOGFONT stFont;
				memset(&stFont,0,sizeof(stFont));
				stFont.lfHeight = MulDiv(m_font.time_char,m_nLogPixelSy,72); // time_char
				stFont.lfWeight = FW_NORMAL;
				stFont.lfClipPrecision = CLIP_LH_ANGLES;
				strcpy(stFont.lfFaceName,"Times New Roman");
				font.CreateFontIndirect(&stFont);

				pOldFont = pCDC->SelectObject(&font);
				COLORREF Color = GetActualLineColor(factor);

				UINT nOldColor;
				if(manualRealColor>0 && TG_IsArriveActualByManual(first->m_data))
				{
					nOldColor = pCDC->SetTextColor(manualRealColor);
				}
				else
				{
					if(first_a_time_status == 1)
						nOldColor = pCDC->SetTextColor(Color);
				}
				CPoint point;
				point.x = (int)(flFirstMinuteOffset * m_fPixelPerMinute);

				if(nDirectFlag == 2)//down direct
				{
					if(IsFlagStart(first->m_data.flag))   ////start  
					{   
						if(first->m_data.arrive == first->m_data.depart)
						{
							pCDC->SetTextAlign(TA_CENTER|TA_TOP);
							point.y =  first_bottom_exit;
							pCDC->SetTextAlign(TA_CENTER|TA_TOP);
							if(point.y >=0)
							{
								pCDC->TextOut((int)point.x + x_point_offset, point.y + y_point_offset, time_char, 1);  //4
							}
						}
					}
					else
					{
						point.y =  first_bottom_entry;
						int yStation = first_top_entry;
						if(first->m_data.arrive == first->m_data.depart)
						{ 
							point.y =  first_bottom_exit;
							if(point.x >=0)    //下行通过点 加边界值
							{
								if(second!=NULL )
								{
									//2004-11-16
									if(nDirectFlag_pre != nDirectFlag_next)
										pCDC->SetTextAlign(TA_CENTER|TA_BOTTOM);
									else
										pCDC->SetTextAlign(TA_CENTER|TA_TOP);

									pCDC->TextOut((int)point.x + x_point_offset, point.y + y_point_offset, time_char, 1);  //4
								}
								else
								{ 
									if(!IsFlagTerminal(first->m_data.flag))
									{
										int flag = GetEntryExitDirection(single,first,(ushort)EXIT_POSITION,locCurrentSectionNo);
										if(flag != -1)
										{
											if(flag != UP_MARK)
												pCDC->SetTextAlign(TA_CENTER|TA_TOP);
											else
												pCDC->SetTextAlign(TA_CENTER|TA_BOTTOM);
										}
										pCDC->TextOut((int)point.x  + x_point_offset,point.y + y_point_offset, time_char, 1);	
									}
									else
									{
										pCDC->SetTextAlign(TA_CENTER|TA_BOTTOM);
										pCDC->TextOut((int)point.x + x_point_offset, yStation+ y_point_offset, time_char, 1);  //4
									}
								}
							}
						}
						else
						{
							if(point.x >=0)    //下行到开的到点 加边界值
							{
								int flag = GetEntryExitDirection(single,first,(ushort)ENTRY_POSITION,locCurrentSectionNo);
								if(flag != -1)
								{
									if(flag != UP_MARK)
										pCDC->SetTextAlign(TA_CENTER|TA_TOP);
									else
										pCDC->SetTextAlign(TA_CENTER|TA_BOTTOM);
								}
								pCDC->TextOut((int)point.x +1 + x_point_offset, yStation +1 + y_point_offset, time_char, 1);   //2
							}
						}
					}
				}
				else  //up direct
				{ 
					pCDC->SetTextAlign(TA_CENTER|TA_BOTTOM);
					int flag = GetEntryExitDirection(single,first,(ushort)ENTRY_POSITION,locCurrentSectionNo);
					if(first->m_data.arrive != first->m_data.depart)
					{
						if(flag != -1)
						{
							if(flag != UP_MARK)
								pCDC->SetTextAlign(TA_CENTER|TA_TOP);
							else
								pCDC->SetTextAlign(TA_CENTER|TA_BOTTOM);
						}
					}

					if(IsFlagStart(first->m_data.flag))
					{  
						if(first->m_data.arrive == first->m_data.depart)
						{
							pCDC->SetTextAlign(TA_CENTER|TA_BOTTOM);
							point.y = first_top_exit;
							pCDC->SetTextAlign(TA_CENTER|TA_BOTTOM);
							if(point.x >=0)   //始发点加边界值
							{
								pCDC->TextOut((int)point.x + x_point_offset-3,point.y + y_point_offset, time_char, 1);	
							}
						}
					}
					else
					{   ////Arival 
						point.y =  first_top_entry;
						int yStation = first_bottom_entry;
						if(first->m_data.arrive == first->m_data.depart)  
						{  
							point.y =  first_top_exit;
							if(point.x >=0)    //上行通过点加边界值
							{
								if(second!=NULL )
								{
									if(nDirectFlag_pre != nDirectFlag_next)
										pCDC->SetTextAlign(TA_CENTER|TA_TOP);
									else
										pCDC->SetTextAlign(TA_CENTER|TA_BOTTOM);

									pCDC->TextOut((int)point.x + x_point_offset,point.y + y_point_offset, time_char, 1);	
								}
								else								 
								{ 
									if(!IsFlagTerminal(first->m_data.flag))
									{
										int flag = GetEntryExitDirection(single,first,(ushort)EXIT_POSITION,locCurrentSectionNo);
										if(flag != -1)
										{
											if(flag != UP_MARK)
												pCDC->SetTextAlign(TA_CENTER|TA_TOP);
											else
												pCDC->SetTextAlign(TA_CENTER|TA_BOTTOM);
										}
										pCDC->TextOut((int)point.x  + x_point_offset,point.y + y_point_offset, time_char, 1);	
									}
									else
									{
										pCDC->SetTextAlign(TA_CENTER|TA_TOP);
										pCDC->TextOut((int)point.x + x_point_offset,yStation + y_point_offset, time_char, 1);	
									}
								}
							}
						}
						else
						{
							if(point.x >=0)  //上行到开的到点加边界值
							{
								int flag = GetEntryExitDirection(single,first,(ushort)ENTRY_POSITION,locCurrentSectionNo);
								if(flag != -1)
								{
									if(flag != UP_MARK)
										pCDC->SetTextAlign(TA_CENTER|TA_TOP);
									else
										pCDC->SetTextAlign(TA_CENTER|TA_BOTTOM);
								}
								pCDC->TextOut((int)point.x + x_point_offset,yStation +1 + y_point_offset, time_char, 1);	
							}
						}
					}
				}
				pCDC->SelectObject(pOldFont);   
				font.DeleteObject();    

				if(manualRealColor>0 && TG_IsArriveActualByManual(first->m_data))
				{
					pCDC->SetTextColor(nOldColor);
				}
				else
				{
					if(first_a_time_status == 1)
						pCDC->SetTextColor(nOldColor);
				}
			}
		}//end of depart time != arrival time code

		// draw inter station line
		long date_depart,time_depart;
		time_depart = minute_time(first->m_data.depart,&date_depart);
		flFirstMinuteOffset = GetMinuteWidthToPlotStartTime(time_depart/60,date_depart,isPrint,m_nSchdStatus);       
		//draw side content 
		if(nDirectFlag == 2)
		{//DOWN_DIRECT
			ushort usEntryFlag = ENTRY_POSITION;
			if(IsFlagStart(first->m_data.flag))
				usEntryFlag = EXIT_POSITION;
			ptlStart.x = ptlEnd.x = (int)(flFirstMinuteOffset * m_fPixelPerMinute);
			ptlStart.y  =  GetStationVPos(single,first,PRIMARY_TRACK,usEntryFlag,m_nSchdStatus);
			if(ptlStart.y == -1) continue;          
			ptlEnd.y = GetStationVPos(single,first,BOTTOM_TRACK, usEntryFlag,m_nSchdStatus);
			if(ptlEnd.y == -1) continue;
			if(ptlEnd.x <= rangeRect.right && ptlEnd.x >= 0)
			{
				if(!IsFlagTerminal(first->m_data.flag) && !IsFlagAbsTerminal(first->m_data.flag))
				{
					if(first_d_time_status && second_a_time_status)
					{
						DrawActualLine(pCDC,CPoint(ptlStart.x + x_point_offset,ptlStart.y + y_point_offset),
							CPoint(ptlEnd.x+x_point_offset,ptlEnd.y+ y_point_offset),single->GetTrainInfo()->m_data.type,FALSE,m_nCountOfEngine);//single->m_carriage_count);//draw real time line

					}
					else
					{
						DrawActualLine(pCDC,CPoint(ptlStart.x + x_point_offset,ptlStart.y + y_point_offset),
							CPoint(ptlEnd.x+x_point_offset,ptlEnd.y+ y_point_offset),single->GetTrainInfo()->m_data.type,FALSE,m_nCountOfEngine,0,FALSE);
					}
				}

				//point rect information
				if(!(second == NULL && IsFlagTerminal(first->m_data.flag) && first->m_data.arrive != first->m_data.depart))
				{
					m_pPerPointInfo[m_nPointCount].record_index = i;
					if(first->m_data.arrive == first->m_data.depart)
						m_pPerPointInfo[m_nPointCount].start_depart_flag = UNION_POSITION;
					else
						m_pPerPointInfo[m_nPointCount].start_depart_flag = DEPART_POSITION;
					m_pPerPointInfo[m_nPointCount++].rect  = CRect(CPoint(ptlStart.x - uDis,ptlStart.y - uDis),sSize);
				}

				if( ptlStart.y != ptlEnd.y && !(second == NULL && IsFlagTerminal(first->m_data.flag)) )
				{  
					m_pPerPointInfo[m_nPointCount].record_index = i;
					if(first->m_data.arrive == first->m_data.depart)
						m_pPerPointInfo[m_nPointCount].start_depart_flag = UNION_POSITION;
					else
						m_pPerPointInfo[m_nPointCount].start_depart_flag = DEPART_POSITION;
					m_pPerPointInfo[m_nPointCount++].rect  = CRect(CPoint(ptlEnd.x - uDis,ptlEnd.y - uDis),sSize);

				}
			}
			//for record exit position != entry position 
			if(ptlEnd.y != first_bottom_exit && second != NULL)
			{
				ptlEnd.y = first_bottom_exit;
				m_pPerPointInfo[m_nPointCount].record_index = i;
				if(first->m_data.arrive == first->m_data.depart)
					m_pPerPointInfo[m_nPointCount].start_depart_flag = UNION_POSITION;
				else
					m_pPerPointInfo[m_nPointCount].start_depart_flag = DEPART_POSITION;
				m_pPerPointInfo[m_nPointCount++].rect  = CRect(CPoint(ptlEnd.x - uDis,ptlEnd.y - uDis),sSize);
			}
			if(second !=NULL)
				ptlStart = ptlEnd;
		}
		else//UP_DIRECT
		{
			ushort usEntryFlag = ENTRY_POSITION;
			if(IsFlagStart(first->m_data.flag))
				usEntryFlag = EXIT_POSITION;
			ptlStart.x = ptlEnd.x = (int)(flFirstMinuteOffset * m_fPixelPerMinute);
			ptlStart.y  =  GetStationVPos(single,first, PRIMARY_TRACK, usEntryFlag,m_nSchdStatus);
			if(ptlStart.y == -1) continue; 			
			ptlEnd.y = GetStationVPos(single,first, TOP_TRACK,usEntryFlag,m_nSchdStatus);
			if(ptlEnd.y == -1) continue;
			if(ptlEnd.x <= rangeRect.right && ptlEnd.x >= 0)
			{
				if(!IsFlagTerminal(first->m_data.flag) && !IsFlagAbsTerminal(first->m_data.flag))
				{
					if(first_d_time_status && second_a_time_status)
					{
						DrawActualLine(pCDC,CPoint(ptlStart.x + x_point_offset,ptlStart.y + y_point_offset),
							CPoint(ptlEnd.x+x_point_offset,ptlEnd.y+ y_point_offset),single->GetTrainInfo()->m_data.type,FALSE,m_nCountOfEngine);//single->m_carriage_count);//draw real time line

					}
					else
					{
						if(m_bStopStationWaveLine && JudgeNeedWaveLine(first,m_nSchdStatus,m_nDirectFlag_prefix))
							DrawStopStationLine(pCDC,CPoint(ptlStart.x +x_point_offset,ptlStart.y+y_point_offset),
							CPoint(ptlEnd.x+x_point_offset,ptlEnd.y+y_point_offset),UP_DIRECT);
						else
						{
							DrawActualLine(pCDC,CPoint(ptlStart.x + x_point_offset,ptlStart.y + y_point_offset),
								CPoint(ptlEnd.x+x_point_offset,ptlEnd.y+ y_point_offset),single->GetTrainInfo()->m_data.type,FALSE,m_nCountOfEngine,0,FALSE);
						}
					}
				}
				//point rect information
				bool temp = IsFlagTerminal(first->m_data.flag);
				if(!(second == NULL && IsFlagTerminal(first->m_data.flag) && first->m_data.arrive != first->m_data.depart))
				{//
					m_pPerPointInfo[m_nPointCount].record_index = i;
					if(first->m_data.arrive == first->m_data.depart)
						m_pPerPointInfo[m_nPointCount].start_depart_flag = UNION_POSITION;
					else
						m_pPerPointInfo[m_nPointCount].start_depart_flag = DEPART_POSITION;
					m_pPerPointInfo[m_nPointCount++].rect  = CRect(CPoint(ptlStart.x - uDis,ptlStart.y - uDis),sSize);
				}
				if(ptlStart.y != ptlEnd.y && !(second == NULL && IsFlagTerminal(first->m_data.flag)) )
				{
					m_pPerPointInfo[m_nPointCount].record_index = i;
					if(first->m_data.arrive == first->m_data.depart)
						m_pPerPointInfo[m_nPointCount].start_depart_flag = UNION_POSITION;
					else
						m_pPerPointInfo[m_nPointCount].start_depart_flag = DEPART_POSITION;
					m_pPerPointInfo[m_nPointCount++].rect  = CRect(CPoint(ptlEnd.x - uDis,ptlEnd.y - uDis),sSize);	
				}
			}
			//for record exit position != entry position 
			if(ptlEnd.y != first_top_exit && second != NULL)
			{
				ptlEnd.y = first_top_exit;
				m_pPerPointInfo[m_nPointCount].record_index = i;
				if(first->m_data.arrive == first->m_data.depart)
					m_pPerPointInfo[m_nPointCount].start_depart_flag = UNION_POSITION;
				else
					m_pPerPointInfo[m_nPointCount].start_depart_flag = DEPART_POSITION;
				m_pPerPointInfo[m_nPointCount++].rect  = CRect(CPoint(ptlEnd.x - uDis,ptlEnd.y - uDis),sSize);	
			}

			if(second != NULL)
				ptlStart = ptlEnd;
		}

		// draw depart&terminal minute text	
		time_depart = minute_time(first->m_data.depart,&date_depart);
		if(time_depart==NOTIME) break;
		itoa(time_depart/60 % 10, time_char, 10);
		//draw route depart ok flag;		
		if(rangeRect.PtInRect(ptlStart) )
		{		          
			CPoint point = ptlStart;
			if(nDirectFlag == 2)//down direct 
			{ 
				point.y = first_bottom_exit;
			}
			else
			{//up direct
				point.y = first_top_exit;
			}

		}
		//end of draw route depart ok

        // 直接调用函数 PtInRects, 该函数定义在plotdoc.cpp/h中
		if(PtInRects(rangeRect, ptlStart) && gpcdef.GetFlagDrawTrainTime(first->m_data.station) && usStartTime != usEndTime)
		{
			CFont* pOldFont, font;
			LOGFONT stFont;
			memset(&stFont,0,sizeof(stFont));
			stFont.lfHeight = MulDiv(m_font.time_char,m_nLogPixelSy,72); // time_char
			stFont.lfWeight = FW_NORMAL;
			stFont.lfClipPrecision = CLIP_LH_ANGLES;
			strcpy(stFont.lfFaceName,"Times New Roman");
			font.CreateFontIndirect(&stFont);

			pOldFont = pCDC->SelectObject(&font);
			COLORREF Color = GetActualLineColor(factor);

			UINT nOldColor;
			if(manualRealColor>0 && TG_IsDepartActualByManual(first->m_data))
			{
				nOldColor = pCDC->SetTextColor(manualRealColor);
			}
			else
			{
				if(first_d_time_status == 1)
					nOldColor = pCDC->SetTextColor(Color);
			}
			
			CPoint point = ptlStart;
			if(point.x >=0)  //出发点的左边界值
			{
				if(nDirectFlag == 2)
				{
					point.y = first_bottom_exit;

					//down direct
					pCDC->SetTextAlign(TA_CENTER|TA_TOP);
					int flag = GetEntryExitDirection(single,first,(ushort)EXIT_POSITION,locCurrentSectionNo);
					if(flag != -1){
						if(flag != UP_MARK)
							pCDC->SetTextAlign(TA_CENTER|TA_TOP);
						else
							pCDC->SetTextAlign(TA_CENTER|TA_BOTTOM);
					}
					if(second == NULL && !IsFlagTerminal(first->m_data.flag))
					{// last, depart 
						point.y = first_bottom_exit;
						if(point.y >= 0)
							pCDC->TextOut(point.x+ x_point_offset, point.y+y_point_offset, time_char, 1);

					}
					else if(second == NULL && IsFlagTerminal(first->m_data.flag))
					{// last point,terminal 
						//不会运行到这里来,终到的车不必显示发车点
						if(usStartTime == usEndTime)
						{
							pCDC->SetTextAlign(TA_CENTER|TA_BOTTOM);
							point.y = first_primary_exit;
							if(point.y >= 0)
								pCDC->TextOut(point.x+ x_point_offset, point.y+y_point_offset, time_char, 1);
						}
					}
					else
					{ //not last ,depart  
						point.y = first_bottom_exit;
						if(point.y >= 0)
							pCDC->TextOut(point.x + x_point_offset, point.y+y_point_offset, time_char, 1);
					}
				} 
				else 
				{//up direct
					point.y = first_top_exit;

					pCDC->SetTextAlign(TA_CENTER|TA_BOTTOM);
					int flag = GetEntryExitDirection(single,first,(ushort)EXIT_POSITION,locCurrentSectionNo);
					if(flag != -1)
					{
						if(flag != UP_MARK)
							pCDC->SetTextAlign(TA_CENTER|TA_TOP);
						else
							pCDC->SetTextAlign(TA_CENTER|TA_BOTTOM);
					}
					if(second == NULL && !IsFlagTerminal(first->m_data.flag)  )
					{///last, depart 
						point.y = first_top_exit;
						if(point.y >= 0)
							pCDC->TextOut(point.x + x_point_offset, point.y + y_point_offset, time_char, 1);
					}
					else if(second == NULL && IsFlagTerminal(first->m_data.flag)  )
					{///last point,terminal 
						//不会运行到这里来,终到的车不必显示发车点
						if(usStartTime == usEndTime)
						{
							pCDC->SetTextAlign(TA_CENTER|TA_TOP);
							point.y = first_bottom_entry;
							if(point.y >= 0)
								pCDC->TextOut(point.x + x_point_offset, point.y + y_point_offset, time_char, 1);
						}
					}
					else  
					{//not last ,depart  
						point.y = first_top_exit;
						if(point.y >= 0)
							pCDC->TextOut(point.x + x_point_offset, point.y + y_point_offset, time_char, 1);
					}
				}
			}

			if(manualRealColor>0 && TG_IsDepartActualByManual(first->m_data))
			{
				pCDC->SetTextColor(nOldColor);
			}
			else
			{
				if(first_d_time_status == 1)
					pCDC->SetTextColor(nOldColor);
			}
			
			pCDC->SelectObject(pOldFont);   
			font.DeleteObject();
		}

		//这儿这个点为上行车在该车站的顶点，如果该车站为绘制接入交出标记的，
		//而且上截和下半截在图上的行车方向不一直，那么变换交出点的坐标值
		//2005-01-26
		if(nDirectFlag_pre != nDirectFlag_next && second != NULL && (m_nTrainLeapLineFlag==2||m_nTrainLeapLineFlag==4))
		{
			if(nDirectFlag_next == UP_DIRECT)
			{
				nDirectFlag = 1;//上行
			}
			else
			{
				nDirectFlag = 2;//下行
			}
		}

		if(ptlEnd.y == -1) continue;

		//draw stop abs lines
		if(IsFlagStopAbs(first->m_data.flag) || IsFlagStopSignal(first->m_data.flag))
		{
			time_depart = minute_time(first->m_data.depart,&date_depart);
			flSecondMinuteOffset = GetMinuteWidthToPlotStartTime(time_depart/60 ,date_depart,isPrint,m_nSchdStatus);			
			ptlEnd.x =(int)(flSecondMinuteOffset + first->m_data.special_time_1)  * m_fPixelPerMinute;

			if(IsFlagStopAbs(first->m_data.flag))
			{
				if(IsFlagAbsStart(first->m_data.flag) && i==0)
					ptlEnd.y = GetAbsVPos(single,first,2, EXIT_POSITION,IsFlagStopAbsUp(first->m_data.flag)?UP_DIRECT:DOWN_DIRECT,m_nSchdStatus);//will
				else
				    ptlEnd.y = GetAbsVPos(single,first,0, EXIT_POSITION,IsFlagStopAbsUp(first->m_data.flag)?UP_DIRECT:DOWN_DIRECT,m_nSchdStatus);//will
			}
			else 
			{
				ptlEnd.y = GetAbsVPos(single,first,1, EXIT_POSITION,IsFlagStopAbsUp(first->m_data.flag)?UP_DIRECT:DOWN_DIRECT,m_nSchdStatus);//will
			}

			int dlt = (ptlEnd.y - ptlStart.y)*2/3;
			if(IsAbsDrawUp(first->m_data.stop_reason))
			{
				ptlEnd.y = ptlStart.y + dlt;
			}
			else if(IsAbsDrawBottom(first->m_data.stop_reason))
			{
				ptlEnd.y = ptlStart.y + dlt*2;
			}

			if(second!=NULL)
			{
				if(first->m_data.station==second->m_data.station)
				{
					int scaleRate = min(yMultiple,5);
					int nLineWidth = scaleRate*m_nLineWidthRate;
					CPoint fPoint=ptlEnd,sPoint=ptlEnd;

					time_depart = minute_time(first->m_data.depart,&date_depart);
					int nMinuteOffset = GetMinuteWidthToPlotStartTime(time_depart/60 ,date_depart,isPrint,m_nSchdStatus);
					sPoint.x = (nMinuteOffset + first->m_data.special_time_1)  * m_fPixelPerMinute;
					int stop_abs_time = (single->GetTrainInfo()->m_data.adjust_flag)>>16;
					sPoint.x = sPoint.x-stop_abs_time*m_fPixelPerMinute;
							
					CPen mPen;
					mPen.CreatePen(PS_DOT, nLineWidth, RGB(0,0,255));
					CPen *pOldPen = pCDC->SelectObject(&mPen);

					fPoint.Offset(x_point_offset, y_point_offset);
					sPoint.Offset(x_point_offset, y_point_offset);
					pCDC->MoveTo(fPoint); pCDC->LineTo(sPoint);

					pCDC->SelectObject(pOldPen);
					mPen.DeleteObject();
				}
			}
			if(ptlEnd.x < rangeRect.right && ptlEnd.x > 0)
			{
				if(first_d_time_status && second_a_time_status)
				{
					DrawActualLine(pCDC,CPoint(ptlStart.x + x_point_offset,ptlStart.y + y_point_offset),
						CPoint(ptlEnd.x+x_point_offset,ptlEnd.y+ y_point_offset),single->GetTrainInfo()->m_data.type,TRUE,m_nCountOfEngine);//single->m_carriage_count);//draw real time line

				}
				else
				{
					DrawActualLine(pCDC,CPoint(ptlStart.x + x_point_offset,ptlStart.y + y_point_offset),
						CPoint(ptlEnd.x+x_point_offset,ptlEnd.y+ y_point_offset),single->GetTrainInfo()->m_data.type,TRUE,m_nCountOfEngine,0,FALSE);//single->m_carriage_count);//draw real time line

				}
			}
			else if(ptlEnd.x > rangeRect.right && (ptlStart.x >= 0 && ptlStart.x < rangeRect.right))
			{
				ptlEnd.x = rangeRect.right;
				if(first_d_time_status && second_a_time_status)
				{
					DrawActualLine(pCDC,CPoint(ptlStart.x + x_point_offset,ptlStart.y + y_point_offset),
						CPoint(ptlEnd.x+x_point_offset,ptlEnd.y+ y_point_offset),single->GetTrainInfo()->m_data.type,TRUE,m_nCountOfEngine);//single->m_carriage_count);//draw real time line

				}
				else
				{
					DrawActualLine(pCDC,CPoint(ptlStart.x + x_point_offset,ptlStart.y + y_point_offset),
						CPoint(ptlEnd.x+x_point_offset,ptlEnd.y+ y_point_offset),single->GetTrainInfo()->m_data.type,TRUE,m_nCountOfEngine,0,FALSE);//single->m_carriage_count);//draw real time line


				}
				critical_point = ptlEnd;
				critical_point.Offset (x_point_offset,y_point_offset);
				//判断是否在该车站改了车次号，如果改了，使用新车次号

				NewTrainId = "";
				for(critial_record_index =0; critial_record_index<single->GetTrainRecordCount(); critial_record_index++)//查找在本站是否该了车次号
				{
					if(i == single->GetTrainRecord(critial_record_index)->m_data.station && single->GetTrainRecord(critial_record_index)->m_data.arrive_train_id != single->GetTrainRecord(critial_record_index)->m_data.depart_train_id
						&& strlen(single->GetTrainRecord(critial_record_index)->m_data.arrive_train_id)>0 && strlen(single->GetTrainRecord(critial_record_index)->m_data.depart_train_id)>0)
					{
						NewTrainId.Format("%s",single->GetTrainRecord(critial_record_index)->m_data.depart_train_id);
						break;
					}
				}
				//判断新车次号结束							
				if(NewTrainId.IsEmpty())
					NewTrainId = single->GetTrainID();

				if(first_a_time_status)
					DrawMarkOfCritical(pCDC,single,NewTrainId,critical_point,50,RIGHT_MARK, lActualColor);
				else							
					DrawMarkOfCritical(pCDC,single,NewTrainId,critical_point,50,RIGHT_MARK, lColor);

			}
			//			int nOldAlign = pCDC->SetTextAlign(TA_CENTER);

			//draw 区间停车，区间折返的第一个点时间 add by cf 
			if(rangeRect.PtInRect(ptlStart) && gpcdef.GetFlagDrawTrainTime(first->m_data.station))
			{
				long usTimeValue;
				usTimeValue = flSecondMinuteOffset + first->m_data.special_time_1;

				itoa((int)usTimeValue % 10, time_char, 10);

				CFont* pOldFont, font;
				LOGFONT stFont;
				memset(&stFont,0,sizeof(stFont));
				stFont.lfHeight = MulDiv(m_font.time_char,m_nLogPixelSy,72); // time_char
				stFont.lfWeight = FW_NORMAL;
				stFont.lfClipPrecision = CLIP_LH_ANGLES;
				strcpy(stFont.lfFaceName,"Times New Roman");
				font.CreateFontIndirect(&stFont);

				int text_align;

				pOldFont = pCDC->SelectObject(&font);

				COLORREF Color = GetActualLineColor(factor);

				UINT nOldColor;

				COLORREF nColor = lActualColor;
				if(first_a_time_status == 1)
				{
					nOldColor = pCDC->SetTextColor(Color);
					nColor = lActualColor;
				}
				else
				{
					nColor = lColor;
				}

				CPoint point;
				point = ptlEnd;
				int direct_flag = DOWN_MARK;
				if(nDirectFlag  == 2) //下行
				{
					text_align = pCDC->SetTextAlign(TA_LEFT|TA_BOTTOM);
					direct_flag = DOWN_MARK;
				}
				else
				{
					text_align = pCDC->SetTextAlign(TA_LEFT|TA_TOP);
					direct_flag = UP_MARK;
				}			

				if(point.x >0 && point.x < rangeRect.right)				
				{
					pCDC->TextOut((int)point.x + x_point_offset, point.y + y_point_offset, time_char, 1); 


					//if(usDrawCount == 2)
					{
						CPoint abspt(point.x + x_point_offset, point.y + y_point_offset);
						CTG_TrainRecord* next = single->GetTrainRecord(i+1);
						if(NULL != first && NULL != next && (first->m_data.station == next->m_data.station))
						{
							if(first_a_time_status)
								DrawMarkOfEnd(pCDC, single, single->GetTrainID(), abspt,20,direct_flag, nColor, first->m_data.flag,true);
							else
								DrawMarkOfEnd(pCDC, single, single->GetTrainID(), abspt,20,direct_flag, nColor, first->m_data.flag,false);
						}
					}

				}
				pCDC->SetTextAlign(text_align);
				if(first_a_time_status == 1)
					pCDC->SetTextColor(nOldColor);
				pCDC->SelectObject(pOldFont);  

				font.DeleteObject();
			}
			//end

			ptlStart = ptlEnd;
			//point rect information
			m_pPerPointInfo[m_nPointCount].record_index = i;
			m_pPerPointInfo[m_nPointCount].start_depart_flag = ABS_ARRIVAL_POSITION;
			m_pPerPointInfo[m_nPointCount++].rect  = CRect(CPoint(ptlStart.x - uDis,ptlStart.y - uDis),sSize);
			//2 绘制从区间停车第一个点第二个点的连线
			ptlEnd.x =(int)(flSecondMinuteOffset + first->m_data.special_time_2)  * m_fPixelPerMinute;
			
			if(ptlStart.x < 0 && ptlEnd.x >=0)
			{
				ptlStart.x = 0;
				critical_point = ptlStart;
				critical_point.Offset (x_point_offset,y_point_offset);

				//判断是否在该车站改了车次号，如果改了，使用新车次号
				NewTrainId = "";
				for(critial_record_index =0; critial_record_index<single->GetTrainRecordCount(); critial_record_index++)//查找在本站是否该了车次号
				{
					if(i == single->GetTrainRecord(critial_record_index)->m_data.station && single->GetTrainRecord(critial_record_index)->m_data.arrive_train_id != single->GetTrainRecord(critial_record_index)->m_data.depart_train_id
						&& strlen(single->GetTrainRecord(critial_record_index)->m_data.arrive_train_id)>0 && strlen(single->GetTrainRecord(critial_record_index)->m_data.depart_train_id)>0)
					{
						NewTrainId.Format("%s",single->GetTrainRecord(critial_record_index)->m_data.depart_train_id);
						break;
					}
				}
				//判断新车次号结束							
				if(NewTrainId.IsEmpty())
					NewTrainId = single->GetTrainID();

				if(first_a_time_status)
					DrawMarkOfCritical(pCDC,single,NewTrainId,critical_point,50,LEFT_MARK, lActualColor);
				else							
					DrawMarkOfCritical(pCDC,single,NewTrainId,critical_point,50,LEFT_MARK, lColor);

			}

			bool bDrawFlag = TRUE;
			if((ptlEnd.x > 0 && ptlEnd.x < rangeRect.right) &&
				(ptlStart.x > 0 && ptlStart.x < rangeRect.right))
			{

			}
			else if(ptlEnd.x < 0  && ptlStart.x < 0)
			{
				//do nothing 
				bDrawFlag = FALSE;
			}
			else if( ptlEnd.x > rangeRect.right && ptlStart.x >= rangeRect.right)
			{
				//do nothing 
				bDrawFlag = FALSE;
			}
			else if((ptlEnd.x > 0 && ptlEnd.x < rangeRect.right) &&
				ptlStart.x < 0)
			{
				ptlStart.x = 0;
				critical_point = ptlStart;
				critical_point.Offset (x_point_offset,y_point_offset);

				//判断是否在该车站改了车次号，如果改了，使用新车次号
				NewTrainId = "";
				for(critial_record_index =0; critial_record_index<single->GetTrainRecordCount(); critial_record_index++)//查找在本站是否该了车次号
				{
					if(i == single->GetTrainRecord(critial_record_index)->m_data.station && single->GetTrainRecord(critial_record_index)->m_data.arrive_train_id != single->GetTrainRecord(critial_record_index)->m_data.depart_train_id
						&& strlen(single->GetTrainRecord(critial_record_index)->m_data.arrive_train_id)>0 && strlen(single->GetTrainRecord(critial_record_index)->m_data.depart_train_id)>0)
					{
						NewTrainId.Format("%s",single->GetTrainRecord(critial_record_index)->m_data.depart_train_id);
						break;
					}
				}
				//判断新车次号结束							
				if(NewTrainId.IsEmpty())
					NewTrainId = single->GetTrainID();

				if(first_a_time_status)
					DrawMarkOfCritical(pCDC,single,NewTrainId,critical_point,50,LEFT_MARK, lActualColor);
				else							
					DrawMarkOfCritical(pCDC,single,NewTrainId,critical_point,50,LEFT_MARK, lColor);


			}
			else if( ptlEnd.x > rangeRect.right &&	ptlStart.x < 0)
			{
				ptlStart.x = 0;
				ptlEnd.x =  rangeRect.right;

				critical_point = ptlStart;
				critical_point.Offset (x_point_offset,y_point_offset);
				//判断是否在该车站改了车次号，如果改了，使用新车次号
				NewTrainId = "";
				for(critial_record_index =0; critial_record_index<single->GetTrainRecordCount(); critial_record_index++)//查找在本站是否该了车次号
				{
					if(i == single->GetTrainRecord(critial_record_index)->m_data.station && single->GetTrainRecord(critial_record_index)->m_data.arrive_train_id != single->GetTrainRecord(critial_record_index)->m_data.depart_train_id
						&& strlen(single->GetTrainRecord(critial_record_index)->m_data.arrive_train_id)>0 && strlen(single->GetTrainRecord(critial_record_index)->m_data.depart_train_id)>0)
					{
						NewTrainId.Format("%s",single->GetTrainRecord(critial_record_index)->m_data.depart_train_id);
						break;
					}
				}
				//判断新车次号结束							
				if(NewTrainId.IsEmpty())
					NewTrainId =single->GetTrainID();

				if(first_a_time_status)
					DrawMarkOfCritical(pCDC,single,NewTrainId,critical_point,50,LEFT_MARK, lActualColor);
				else							
					DrawMarkOfCritical(pCDC,single,NewTrainId,critical_point,50,LEFT_MARK, lColor);

				critical_point = ptlEnd;
				critical_point.Offset (x_point_offset,y_point_offset);

				//判断是否在该车站改了车次号，如果改了，使用新车次号
				NewTrainId = "";
				for(critial_record_index =0; critial_record_index<single->GetTrainRecordCount(); critial_record_index++)//查找在本站是否该了车次号
				{
					if(i == single->GetTrainRecord(critial_record_index)->m_data.station && single->GetTrainRecord(critial_record_index)->m_data.arrive_train_id != single->GetTrainRecord(critial_record_index)->m_data.depart_train_id
						&& strlen(single->GetTrainRecord(critial_record_index)->m_data.arrive_train_id)>0 && strlen(single->GetTrainRecord(critial_record_index)->m_data.depart_train_id)>0)
					{
						NewTrainId.Format("%s",single->GetTrainRecord(critial_record_index)->m_data.depart_train_id);
						break;
					}
				}
				//判断新车次号结束							
				if(NewTrainId.IsEmpty())
					NewTrainId = single->GetTrainID();

				if(first_a_time_status)
					DrawMarkOfCritical(pCDC,single,NewTrainId,critical_point,50,RIGHT_MARK, lActualColor);
				else							
					DrawMarkOfCritical(pCDC,single,NewTrainId,critical_point,50,RIGHT_MARK, lColor);

			}
			else if(ptlEnd.x > rangeRect.right && ptlStart.x < rangeRect.right)
			{
				ptlEnd.x =  rangeRect.right;

				critical_point = ptlEnd;
				critical_point.Offset (x_point_offset,y_point_offset);
				//判断是否在该车站改了车次号，如果改了，使用新车次号
				NewTrainId = "";
				for(critial_record_index =0; critial_record_index<single->GetTrainRecordCount(); critial_record_index++)//查找在本站是否该了车次号
				{
					if(i == single->GetTrainRecord(critial_record_index)->m_data.station && single->GetTrainRecord(critial_record_index)->m_data.arrive_train_id != single->GetTrainRecord(critial_record_index)->m_data.depart_train_id
						&& strlen(single->GetTrainRecord(critial_record_index)->m_data.arrive_train_id)>0 && strlen(single->GetTrainRecord(critial_record_index)->m_data.depart_train_id)>0)
					{
						NewTrainId.Format("%s",single->GetTrainRecord(critial_record_index)->m_data.depart_train_id);
						break;
					}
				}
				//判断新车次号结束							
				if(NewTrainId.IsEmpty())
					NewTrainId = single->GetTrainID();

				if(first_a_time_status)
					DrawMarkOfCritical(pCDC,single,NewTrainId,critical_point,50,RIGHT_MARK, lActualColor);
				else							
					DrawMarkOfCritical(pCDC,single,NewTrainId,critical_point,50,RIGHT_MARK, lColor);


			}
			//draw line
			if(bDrawFlag)
			{
				if(first_d_time_status && second_a_time_status)
				{
					DrawActualLine(pCDC,CPoint(ptlStart.x + x_point_offset,ptlStart.y + y_point_offset),
						CPoint(ptlEnd.x+x_point_offset,ptlEnd.y+ y_point_offset),single->GetTrainInfo()->m_data.type,FALSE,m_nCountOfEngine);//single->m_carriage_count);//draw real time line

				}
				else
				{
					DrawActualLine(pCDC,CPoint(ptlStart.x + x_point_offset,ptlStart.y + y_point_offset),
						CPoint(ptlEnd.x+x_point_offset,ptlEnd.y+ y_point_offset),single->GetTrainInfo()->m_data.type,FALSE,m_nCountOfEngine,0,FALSE);//single->m_carriage_count);//draw real time line
				}
				
				CPoint mid;
				mid.x = (ptlStart.x + ptlEnd.x)/2 + x_point_offset;
				mid.y = (ptlStart.y + ptlEnd.y)/2 + y_point_offset;

				CFont mFont,*pOldFont;
				LOGFONT stFont;

				memset(&stFont,0,sizeof(stFont));
				stFont.lfHeight = MulDiv(m_font.station_text,-m_nLogPixelSy,72);
				stFont.lfWeight = FW_THIN;
				stFont.lfClipPrecision = CLIP_LH_ANGLES;
				strcpy(stFont.lfFaceName,"Arial");
				mFont.CreateFontIndirect(&stFont);	
				pOldFont = pCDC->SelectObject(&mFont);			
				COLORREF cl = pCDC->SetTextColor(RGB(255,0,0));
				UINT alg = pCDC->SetTextAlign(TA_CENTER);
				CString strAbsText = first->GetAbsText();
				CSize sz = pCDC->GetTextExtent(strAbsText);
				// 上行
				if( first_primary_exit > second_primary_entry)
				{
					alg = pCDC->SetTextAlign(TA_TOP|TA_CENTER);
					pCDC->TextOut(mid.x, mid.y-sz.cy, strAbsText);
				}
				else
				{
					alg = pCDC->SetTextAlign(TA_BOTTOM|TA_CENTER);
					pCDC->TextOut(mid.x, mid.y+sz.cy, strAbsText);
				}

				
				pCDC->SetTextAlign(alg);
				pCDC->SelectObject(pOldFont);

				//draw triangle for stop abs sigal
				if(IsFlagStopSignal(first->m_data.flag))
				{
					CPen mMyPen;
					mMyPen.CreatePen(PS_SOLID, scaleRate, RGB(255,0,0));
					pOldPen = pCDC->SelectObject(&mMyPen);

					int delta = 20;
					CPoint ptl[4];
					
					ptl[0].x = (ptlStart.x + ptlEnd.x) /2;
					ptl[0].y = ptlStart.y;
					ptl[1].x = (ptlStart.x + ptlEnd.x) /2 + delta/2;
					ptl[1].y = ptlStart.y + delta;
					ptl[2].x = (ptlStart.x + ptlEnd.x) /2 - delta/2;
					ptl[2].y = ptlStart.y + delta;
					ptl[3].x = (ptlStart.x + ptlEnd.x) /2;
					ptl[3].y = ptlStart.y;

					for(int i=0;i<4;i++)
						ptl[i].Offset (x_point_offset,y_point_offset);

					if( first_primary_exit < second_primary_entry)
					{
						for(int i=0;i<4;i++)
							ptl[i].Offset (0,-delta-1);
					}
					
					pCDC->Polyline(ptl,4);

					CFont mFont,*pOldFont;
					LOGFONT stFont;

					memset(&stFont,0,sizeof(stFont));
					stFont.lfHeight = MulDiv(8,-m_nLogPixelSy,72);
					stFont.lfWeight = FW_THIN;
					stFont.lfClipPrecision = CLIP_LH_ANGLES;
					strcpy(stFont.lfFaceName,"Arial");
					mFont.CreateFontIndirect(&stFont);	
					pOldFont = pCDC->SelectObject(&mFont);	

					CString stopTime;
					stopTime.Format("%d", first->m_data.special_time_2-first->m_data.special_time_1);
					UINT alg = pCDC->SetTextAlign(TA_CENTER|TA_TOP);
					pCDC->TextOut(ptl[0].x, ptl[0].y+delta/3, stopTime);
					pCDC->SetTextAlign(alg);
					pCDC->SelectObject(pOldFont);
					pCDC->SelectObject(pOldPen);
					mMyPen.DeleteObject();
				}
				pCDC->SetTextColor(cl);
			}

			//draw 区间停车，区间折返的第二个点时间 add by cf 
			if(rangeRect.PtInRect(ptlStart) && gpcdef.GetFlagDrawTrainTime(first->m_data.station))
			{
				long usTimeValue;
				usTimeValue = flSecondMinuteOffset + first->m_data.special_time_2;
				//ConvertXPostoTime(ptlEnd.x,&usDateValue,&usTimeValue);
				itoa((int)usTimeValue % 10, time_char, 10);
				//********************************************************************************	

				CFont* pOldFont, font;
				LOGFONT stFont;
				memset(&stFont,0,sizeof(stFont));
				stFont.lfHeight = MulDiv(m_font.time_char-1,m_nLogPixelSy,72); // time_char
				stFont.lfWeight = FW_NORMAL;
				stFont.lfClipPrecision = CLIP_LH_ANGLES;
				strcpy(stFont.lfFaceName,"Times New Roman");
				font.CreateFontIndirect(&stFont);

				int text_align;

				pOldFont = pCDC->SelectObject(&font);

				COLORREF Color = GetActualLineColor(factor);

				UINT nOldColor;
				COLORREF ncolor;
				if(first_a_time_status == 1)
				{
					nOldColor = pCDC->SetTextColor(Color);
					ncolor=Color;
				}
				else 
				{
					ncolor=lColor;
				}
				CPoint point;
				point.x = ptlEnd.x;
				point.y = ptlEnd.y;
				//******************************************************************************
				point.x = ptlEnd.x ;//+ stFont.lfHeight + 3;
				int nflag=UP_DIRECT;

				if(nDirectFlag == 2) //下行
				{
					text_align = pCDC->SetTextAlign(TA_RIGHT|TA_TOP);
					nflag=UP_DIRECT;
				}
				else
				{
					text_align = pCDC->SetTextAlign(TA_RIGHT|TA_BOTTOM);
					nflag=DOWN_DIRECT;
				}


				if(point.x >0 && point.x < rangeRect.right)		
				{
					if(first->m_data.special_time_1 != first->m_data.special_time_2)
					{
						pCDC->TextOut((int)point.x + x_point_offset, point.y + y_point_offset, time_char, 1);  
						CPoint abspt(point.x + x_point_offset, point.y + y_point_offset);
						//if(NULL !=second && (usDrawCount == 2) &&(first->m_data.station == second->m_data.station))
						if(NULL !=second &&(first->m_data.station == second->m_data.station))
							this->DrawMarkOfStart(pCDC,single,second->GetArriveTID(),abspt,20,nflag,ncolor,first_a_time_status == 1,schd_type);
					}
				}

				pCDC->SetTextAlign(text_align);
				if(first_a_time_status == 1)
					pCDC->SetTextColor(nOldColor);

				pCDC->SelectObject(pOldFont);   

				font.DeleteObject();
			}
			
			ptlStart = ptlEnd;        
			//point rect information
			m_pPerPointInfo[m_nPointCount].record_index = i;
			m_pPerPointInfo[m_nPointCount].start_depart_flag = ABS_DEPART_POSITION;
			m_pPerPointInfo[m_nPointCount++].rect  = CRect(CPoint(ptlStart.x - uDis,ptlStart.y - uDis),sSize);

		}
		//end 
		if(second==NULL)  break; 
		long time_arrival,date_arrival;
		time_arrival = minute_time(second->m_data.arrive,&date_arrival);
		flSecondMinuteOffset = GetMinuteWidthToPlotStartTime(time_arrival/60,date_arrival,isPrint,m_nSchdStatus);

		if(IsFlagAnLocReturn(second->m_data.flag))
			nDirectFlag = (nDirectFlag==2)?1:2;

		//DOWN_DIRECT 
		if(nDirectFlag == 2)
		{
			ptlEnd.x = (int)(flSecondMinuteOffset * m_fPixelPerMinute);
			ptlEnd.y = second_top_entry;
			if(ptlEnd.y == -1) continue;
			//6:00 or 18:00 position	
			if((ptlStart.x < 0 && ptlEnd.x < 0 ))
			{
				continue;
			}
			else if ((ptlStart.x < 0 && ptlEnd.x >= 0 )) 
			{
				ptlStart.y = ptlEnd.y - (double)ptlEnd.x / (double)(ptlEnd.x - ptlStart.x )*(double)abs(ptlEnd.y - ptlStart.y);
				ptlStart.x = 0;
				critical_point = ptlStart;
				critical_point.Offset (x_point_offset,y_point_offset);
				//判断是否在该车站改了车次号，如果改了，使用新车次号
				NewTrainId = "";
				for(critial_record_index =0; critial_record_index<single->GetTrainRecordCount(); critial_record_index++)//查找在本站是否该了车次号
				{
					if(i == single->GetTrainRecord(critial_record_index)->m_data.station && single->GetTrainRecord(critial_record_index)->m_data.arrive_train_id != single->GetTrainRecord(critial_record_index)->m_data.depart_train_id
						&& strlen(single->GetTrainRecord(critial_record_index)->m_data.arrive_train_id)>0 && strlen(single->GetTrainRecord(critial_record_index)->m_data.depart_train_id)>0)
					{
						NewTrainId.Format("%s",single->GetTrainRecord(critial_record_index)->m_data.depart_train_id);
						break;
					}
				}
				//判断新车次号结束							
				if(NewTrainId.IsEmpty())
					NewTrainId = single->GetTrainID();

				if(first_a_time_status)
					DrawMarkOfCritical(pCDC,single,NewTrainId,critical_point,50,LEFT_MARK, lActualColor);
				else							
					DrawMarkOfCritical(pCDC,single,NewTrainId,critical_point,50,LEFT_MARK, lColor);

			}
			if((ptlStart.x > rangeRect.right) && ptlEnd.x > rangeRect.right)
			{
			}
			else if((ptlStart.x < rangeRect.right) && ptlEnd.x >= rangeRect.right)
			{
				ptlEnd.y = ptlEnd.y - (double)(ptlEnd.x - rangeRect.right) / (double)(ptlEnd.x - ptlStart.x )*(double)abs(ptlEnd.y - ptlStart.y);
				ptlEnd.x = rangeRect.right;
				critical_point = ptlEnd;
				critical_point.Offset (x_point_offset,y_point_offset);
				//判断是否在该车站改了车次号，如果改了，使用新车次号
				NewTrainId = "";
				for(critial_record_index =0; critial_record_index<single->GetTrainRecordCount(); critial_record_index++)//查找在本站是否该了车次号
				{
					if(i == single->GetTrainRecord(critial_record_index)->m_data.station && single->GetTrainRecord(critial_record_index)->m_data.arrive_train_id != single->GetTrainRecord(critial_record_index)->m_data.depart_train_id
						&& strlen(single->GetTrainRecord(critial_record_index)->m_data.arrive_train_id)>0 && strlen(single->GetTrainRecord(critial_record_index)->m_data.depart_train_id)>0)
					{
						NewTrainId.Format("%s",single->GetTrainRecord(critial_record_index)->m_data.depart_train_id);
						break;
					}
				}
				//判断新车次号结束							
				if(NewTrainId.IsEmpty())
					NewTrainId = single->GetTrainID();

				if(first_a_time_status)
					DrawMarkOfCritical(pCDC,single,NewTrainId,critical_point,50,RIGHT_MARK, lActualColor);
				else							
					DrawMarkOfCritical(pCDC,single,NewTrainId,critical_point,50,RIGHT_MARK, lColor);
			}

			if((m_nSchdStatus == WORK_SCHEDULE|| m_nSchdStatus == SHIFT_SCHEDULE ||m_nSchdStatus == HISTORY_SCHEDULE) == TRUE? TRUE:(ptlEnd.x >= ptlStart.x))
			{	
				if(ptlEnd.x <= rangeRect.right)
				{
					if(!IsFlagUnitOtherTrain(first->m_data.flag)&& !IsFlagEnd(first->m_data.flag))
					{
						if(first_d_time_status && second_a_time_status)
						{
							DrawActualLine(pCDC,CPoint(ptlStart.x + x_point_offset,ptlStart.y + y_point_offset),
								CPoint(ptlEnd.x+x_point_offset,ptlEnd.y+ y_point_offset),single->GetTrainInfo()->m_data.type,TRUE,m_nCountOfEngine);//single->m_carriage_count);//draw real time line
						}
						else
						{
							DrawActualLine(pCDC,CPoint(ptlStart.x + x_point_offset,ptlStart.y + y_point_offset),
								CPoint(ptlEnd.x+x_point_offset,ptlEnd.y+ y_point_offset),single->GetTrainInfo()->m_data.type,TRUE,m_nCountOfEngine,0,FALSE);
						}
					}

					if(exception_Mark)
					{
						DrawBadMark(pCDC,CPoint(ptlStart.x + x_point_offset,ptlStart.y + y_point_offset),
							CPoint(ptlEnd.x+x_point_offset,ptlEnd.y+ y_point_offset),EXCEPTION_MARK,single->GetTrainID());
					}
					if(bad_Mark)
					{
						DrawBadMark(pCDC,CPoint(ptlStart.x + x_point_offset,ptlStart.y + y_point_offset),
							CPoint(ptlEnd.x+x_point_offset,ptlEnd.y+ y_point_offset),BAD_MARK,single->GetTrainID());
					}
					if(reverse_Mark)
					{
						DrawBadMark(pCDC,CPoint(ptlStart.x + x_point_offset,ptlStart.y + y_point_offset),
							CPoint(ptlEnd.x+x_point_offset,ptlEnd.y+ y_point_offset),REVERSE_MARK,single->GetTrainID());
					}
					if(stopReason_Mark)
					{
						DrawBadMark(pCDC,CPoint(ptlStart.x + x_point_offset,ptlStart.y + y_point_offset),
							CPoint(ptlEnd.x+x_point_offset,ptlEnd.y+ y_point_offset),STOPREASTON_MARK,single->GetTrainID());
					}

					if(m_bShowFastOrSlowTime)
					{
						if(!IsFlagStopAbs(first->m_data.flag) && !IsFlagStopSignal(first->m_data.flag))
						{
							DrawFastOrSlowTime(pCDC,CPoint(ptlStart.x + x_point_offset,ptlStart.y + y_point_offset),
								CPoint(ptlEnd.x+x_point_offset,ptlEnd.y+ y_point_offset),IsRunFast_Plot(first),GetFastMinute_Plot(first),nDirectFlag);
						}
					}
				}
			}
			else
			{
				//基本图的过表
				if(second_a_time_status )
				{ ////m_nPageWidth	  
					CPoint criticalstart,criticalend;

					int basicMemoryWidth = m_fPixelPerHour*24;
					criticalstart = CPoint(basicMemoryWidth + x_point_offset,ptlStart.y + abs(ptlEnd.y - ptlStart.y)*(basicMemoryWidth - ptlStart.x )/(basicMemoryWidth - ptlStart.x + ptlEnd.x) + y_point_offset);
					criticalend = CPoint(0 + x_point_offset,ptlStart.y + abs(ptlEnd.y - ptlStart.y)*(basicMemoryWidth - ptlStart.x )/(basicMemoryWidth - ptlStart.x + ptlEnd.x) + y_point_offset);

					DrawActualLine(pCDC,CPoint(ptlStart.x + x_point_offset,ptlStart.y + y_point_offset)
						,criticalstart,single->GetTrainInfo()->m_data.type,FALSE,m_nCountOfEngine);//single->m_carriage_count);
					DrawActualLine(pCDC,criticalend,
						CPoint(ptlEnd.x+x_point_offset,ptlEnd.y+ y_point_offset),single->GetTrainInfo()->m_data.type,FALSE,m_nCountOfEngine);//single->m_carriage_count);//draw real time line

					DrawMarkOfCritical(pCDC,single,single->GetTrainID(),criticalstart,50,RIGHT_MARK, lActualColor);

					DrawMarkOfCritical(pCDC,single,single->GetTrainID(),criticalend,50,LEFT_MARK, lActualColor);
					/////ptlStart.x
				}
				else
				{////m_nPageWidth
					CPoint criticalstart,criticalend;

					int basicMemoryWidth = m_fPixelPerHour*24;
					criticalstart = CPoint(basicMemoryWidth + x_point_offset,ptlStart.y + abs(ptlEnd.y - ptlStart.y)*(basicMemoryWidth - ptlStart.x )/(basicMemoryWidth - ptlStart.x + ptlEnd.x) + y_point_offset);
					criticalend = CPoint(0 + x_point_offset,ptlStart.y + abs(ptlEnd.y - ptlStart.y)*(basicMemoryWidth - ptlStart.x )/(basicMemoryWidth - ptlStart.x + ptlEnd.x) + y_point_offset);

					DrawActualLine(pCDC,CPoint(ptlStart.x + x_point_offset,ptlStart.y + y_point_offset)
						,criticalstart,single->GetTrainInfo()->m_data.type,FALSE,m_nCountOfEngine,0,FALSE);//single->m_carriage_count);
					DrawActualLine(pCDC,criticalend,
						CPoint(ptlEnd.x+x_point_offset,ptlEnd.y+ y_point_offset),single->GetTrainInfo()->m_data.type,FALSE,m_nCountOfEngine,0,FALSE);
				}
			}
			ptlStart = ptlEnd;
			ptlEnd.y = second_primary_entry;
			if(ptlEnd.y == -1) continue;
			if(ptlEnd.x < rangeRect.right)
			{
				if(!IsFlagUnitOtherTrain(first->m_data.flag)&& !IsFlagEnd(first->m_data.flag))
				{
					if(first_d_time_status && second_a_time_status )
					{
						DrawActualLine(pCDC,CPoint(ptlStart.x + x_point_offset,ptlStart.y + y_point_offset),//////////////////////////////
							CPoint(ptlEnd.x+x_point_offset,ptlEnd.y+ y_point_offset),single->GetTrainInfo()->m_data.type,FALSE,m_nCountOfEngine);//single->m_carriage_count);//draw real time line
					}
					else
					{
						DrawActualLine(pCDC,CPoint(ptlStart.x + x_point_offset,ptlStart.y + y_point_offset),//////////////////////////////
							CPoint(ptlEnd.x+x_point_offset,ptlEnd.y+ y_point_offset),single->GetTrainInfo()->m_data.type,FALSE,m_nCountOfEngine,0,FALSE);//single->m_carriage_count);//draw real time line
					}
				}
			}
			if( ptlStart.y != ptlEnd.y)
			{
				//point rect information
				m_pPerPointInfo[m_nPointCount].record_index = i + 1;
				m_pPerPointInfo[m_nPointCount].start_depart_flag = ARRIVAL_POSITION;
				m_pPerPointInfo[m_nPointCount++].rect  = CRect(CPoint(ptlStart.x - uDis,ptlStart.y - uDis),sSize);
			}

			ptlStart = ptlEnd;
		}
		else
		{//UP_DIRECT
			ptlEnd.x =(int)(flSecondMinuteOffset * m_fPixelPerMinute);
			ptlEnd.y = second_bottom_entry;
			if(ptlEnd.y == -1) continue;
			//6:00 or 18:00 position	
			if((ptlStart.x < 0 && ptlEnd.x < 0 ))
			{
				continue;
			}
			else if ((ptlStart.x < 0 && ptlEnd.x >= 0 )) 
			{
				ptlStart.y = ptlEnd.y + (double)abs(ptlEnd.y - ptlStart.y)*(double)ptlEnd.x / (double)(ptlEnd.x - ptlStart.x );
				ptlStart.x = 0;
				critical_point = ptlStart;
				critical_point.Offset (x_point_offset,y_point_offset);
				//判断是否在该车站改了车次号，如果改了，使用新车次号
				NewTrainId = "";
				for(critial_record_index =0; critial_record_index<single->GetTrainRecordCount(); critial_record_index++)//查找在本站是否该了车次号
				{
					if(i == single->GetTrainRecord(critial_record_index)->m_data.station && single->GetTrainRecord(critial_record_index)->m_data.arrive_train_id != single->GetTrainRecord(critial_record_index)->m_data.depart_train_id
						&& strlen(single->GetTrainRecord(critial_record_index)->m_data.arrive_train_id)>0 && strlen(single->GetTrainRecord(critial_record_index)->m_data.depart_train_id)>0)
					{
						NewTrainId.Format("%s",single->GetTrainRecord(critial_record_index)->m_data.depart_train_id);
						break;
					}
				}
				//判断新车次号结束							
				if(NewTrainId.IsEmpty())
					NewTrainId = single->GetTrainID();

				if(first_a_time_status)
					DrawMarkOfCritical(pCDC,single,NewTrainId,critical_point,50,LEFT_MARK, lActualColor);
				else							
					DrawMarkOfCritical(pCDC,single,NewTrainId,critical_point,50,LEFT_MARK, lColor);

			}
			if((ptlStart.x > rangeRect.right) && ptlEnd.x >= rangeRect.right)
			{
				continue;
			}
			else if((ptlStart.x < rangeRect.right) && ptlEnd.x >= rangeRect.right)
			{
				ptlEnd.y = ptlEnd.y - (double)(ptlEnd.y - ptlStart.y)*(double)(ptlEnd.x - rangeRect.right) / (double)(ptlEnd.x - ptlStart.x );
				ptlEnd.x = rangeRect.right;
				critical_point = ptlEnd;
				critical_point.Offset (x_point_offset,y_point_offset);
				//判断是否在该车站改了车次号，如果改了，使用新车次号
				NewTrainId = "";
				for(critial_record_index =0; critial_record_index<single->GetTrainRecordCount(); critial_record_index++)//查找在本站是否该了车次号
				{
					if(i == single->GetTrainRecord(critial_record_index)->m_data.station && single->GetTrainRecord(critial_record_index)->m_data.arrive_train_id != single->GetTrainRecord(critial_record_index)->m_data.depart_train_id
						&& strlen(single->GetTrainRecord(critial_record_index)->m_data.arrive_train_id)>0 && strlen(single->GetTrainRecord(critial_record_index)->m_data.depart_train_id)>0)
					{
						NewTrainId.Format("%s",single->GetTrainRecord(critial_record_index)->m_data.depart_train_id);
						break;
					}
				}
				//判断新车次号结束							
				if(NewTrainId.IsEmpty())
					NewTrainId = single->GetTrainID();

				if(first_a_time_status)
					DrawMarkOfCritical(pCDC,single,NewTrainId,critical_point,50,RIGHT_MARK, lActualColor);
				else							
					DrawMarkOfCritical(pCDC,single,NewTrainId,critical_point,50,RIGHT_MARK, lColor);

			}

			if((m_nSchdStatus == WORK_SCHEDULE || m_nSchdStatus == SHIFT_SCHEDULE||m_nSchdStatus == HISTORY_SCHEDULE) == TRUE? TRUE:(ptlEnd.x >= ptlStart.x))
			{				
				if(ptlEnd.x <= rangeRect.right)
				{
					if(!IsFlagUnitOtherTrain(first->m_data.flag)&& !IsFlagEnd(first->m_data.flag))
					{
						if(first_d_time_status && second_a_time_status)
						{
							DrawActualLine(pCDC,CPoint(ptlStart.x + x_point_offset,ptlStart.y + y_point_offset),
								CPoint(ptlEnd.x+x_point_offset,ptlEnd.y+ y_point_offset),single->GetTrainInfo()->m_data.type,TRUE,m_nCountOfEngine);//single->m_carriage_count);//draw real time line

						}
						else
						{
							DrawActualLine(pCDC,CPoint(ptlStart.x + x_point_offset,ptlStart.y + y_point_offset),
								CPoint(ptlEnd.x+x_point_offset,ptlEnd.y+ y_point_offset),single->GetTrainInfo()->m_data.type,TRUE,m_nCountOfEngine,0,FALSE);
						}
					}

					if(exception_Mark)
					{
						DrawBadMark(pCDC,CPoint(ptlStart.x + x_point_offset,ptlStart.y + y_point_offset),
							CPoint(ptlEnd.x+x_point_offset,ptlEnd.y+ y_point_offset),EXCEPTION_MARK ,single->GetTrainID());
					}

					if(bad_Mark)
					{
						DrawBadMark(pCDC,CPoint(ptlStart.x + x_point_offset,ptlStart.y + y_point_offset),
							CPoint(ptlEnd.x+x_point_offset,ptlEnd.y+ y_point_offset),BAD_MARK ,single->GetTrainID());
					}
					if(reverse_Mark)
					{
						DrawBadMark(pCDC,CPoint(ptlStart.x + x_point_offset,ptlStart.y + y_point_offset),
							CPoint(ptlEnd.x+x_point_offset,ptlEnd.y+ y_point_offset),REVERSE_MARK,single->GetTrainID());
					}
					if(stopReason_Mark)
					{
						DrawBadMark(pCDC,CPoint(ptlStart.x + x_point_offset,ptlStart.y + y_point_offset),
							CPoint(ptlEnd.x+x_point_offset,ptlEnd.y+ y_point_offset),STOPREASTON_MARK,single->GetTrainID());
					}

					if(m_bShowFastOrSlowTime)
					{
						if(!IsFlagStopAbs(first->m_data.flag) && !IsFlagStopSignal(first->m_data.flag))
						{
							DrawFastOrSlowTime(pCDC,CPoint(ptlStart.x + x_point_offset,ptlStart.y + y_point_offset),
								CPoint(ptlEnd.x+x_point_offset,ptlEnd.y+ y_point_offset),IsRunFast_Plot(first),GetFastMinute_Plot(first),nDirectFlag);
						}
					}
				}
			}
			else
			{
				//基本图的过表
				CPoint criticalstart,criticalend;
				if(first_d_time_status && second_a_time_status)
				{  		
					int basicMemoryWidth = m_fPixelPerHour*24;
					criticalstart = CPoint(basicMemoryWidth + x_point_offset
						,ptlStart.y - abs(ptlEnd.y - ptlStart.y)*(basicMemoryWidth - ptlStart.x )/(basicMemoryWidth - ptlStart.x + ptlEnd.x) + y_point_offset);
					criticalend = CPoint(0 + x_point_offset,ptlStart.y - abs(ptlEnd.y - ptlStart.y)*(basicMemoryWidth - ptlStart.x )/(basicMemoryWidth - ptlStart.x + ptlEnd.x) + y_point_offset);

					DrawActualLine(pCDC,CPoint(ptlStart.x + x_point_offset,ptlStart.y + y_point_offset),
						criticalstart,single->GetTrainInfo()->m_data.type,FALSE,m_nCountOfEngine);//single->m_carriage_count);//draw real time line

					DrawActualLine(pCDC,criticalend,
						CPoint(ptlEnd.x+x_point_offset,ptlEnd.y+ y_point_offset),single->GetTrainInfo()->m_data.type,FALSE,m_nCountOfEngine);//single->m_carriage_count);//draw real time line

					DrawMarkOfCritical(pCDC,single,single->GetTrainID(),criticalstart,50,RIGHT_MARK, lActualColor);

					DrawMarkOfCritical(pCDC,single,single->GetTrainID(),criticalend,50,LEFT_MARK, lActualColor);


				}
				else
				{ 
					int temy = ptlStart.y - abs(ptlEnd.y - ptlStart.y)*(m_usMemoryPageWidth - ptlStart.x )/(m_usMemoryPageWidth - ptlStart.x + ptlEnd.x);
					int basicMemoryWidth = m_fPixelPerHour*24;
					criticalstart = CPoint(basicMemoryWidth + x_point_offset
						,ptlStart.y - abs(ptlEnd.y - ptlStart.y)*(basicMemoryWidth - ptlStart.x )/(basicMemoryWidth - ptlStart.x + ptlEnd.x) + y_point_offset);
					criticalend = CPoint(0 + x_point_offset,ptlStart.y - abs(ptlEnd.y - ptlStart.y)*(basicMemoryWidth - ptlStart.x )/(basicMemoryWidth - ptlStart.x + ptlEnd.x) + y_point_offset);

					DrawActualLine(pCDC,CPoint(ptlStart.x + x_point_offset,ptlStart.y + y_point_offset),
						criticalstart,single->GetTrainInfo()->m_data.type,FALSE,m_nCountOfEngine,0,FALSE);//single->m_carriage_count);//draw real time line

					DrawActualLine(pCDC,criticalend,
						CPoint(ptlEnd.x+x_point_offset,ptlEnd.y+ y_point_offset),single->GetTrainInfo()->m_data.type,FALSE,m_nCountOfEngine,0,FALSE);//single->m_carriage_count);//draw real time line

				}
			}

			ptlStart = ptlEnd;
			ptlEnd.y = second_primary_entry;
			if(ptlEnd.y == -1) continue;
			if(ptlEnd.x < rangeRect.right && ptlStart.y != ptlEnd.y)
			{
				if(first_d_time_status && second_a_time_status)
				{
					DrawActualLine(pCDC,CPoint(ptlStart.x + x_point_offset,ptlStart.y + y_point_offset),
						CPoint(ptlEnd.x+x_point_offset,ptlEnd.y+ y_point_offset),single->GetTrainInfo()->m_data.type,FALSE,m_nCountOfEngine);//single->m_carriage_count);//draw real time line
				}
				else
				{
					DrawActualLine(pCDC,CPoint(ptlStart.x + x_point_offset,ptlStart.y + y_point_offset),
						CPoint(ptlEnd.x+x_point_offset,ptlEnd.y+ y_point_offset),single->GetTrainInfo()->m_data.type,FALSE,m_nCountOfEngine,0,FALSE);//single->m_carriage_count);//draw real time line
				}
			}
			if( ptlStart.y != ptlEnd.y)
			{
				//point rect information
				m_pPerPointInfo[m_nPointCount].record_index = i + 1;
				m_pPerPointInfo[m_nPointCount].start_depart_flag = ARRIVAL_POSITION;
				m_pPerPointInfo[m_nPointCount++].rect  = CRect(CPoint(ptlStart.x - uDis,ptlStart.y - uDis),sSize);
			}

			ptlStart = ptlEnd;
		}

		if(ptlEnd.y == -1 ) 
			continue; 
	}//end for first to last code 


	// restore gdi resource
	pCDC->SetROP2(nPreviousMode);
	pCDC->SelectObject(pOldPen);
	pCDC->SetTextColor(nOldColor);
	pCDC->SetTextAlign(nOldAlign);
	pCDC->SetBkMode(nOldBackMode); 

	mThinPen.DeleteObject();

	DrawTrainText(pCDC,single,RGB(255,0,0),schd_type);

	if(m_nShowTrainEarlyLateFlag == 3)
		DrawTrainEarlyLastTime(pCDC,single,lColor,isPrint,m_nSchdStatus);
	else if(m_nShowTrainEarlyLateFlag == 1)
	{
		if(single->IsPassengerTrain())
			DrawTrainEarlyLastTime(pCDC,single,lColor,isPrint,m_nSchdStatus);
	}
	else if(m_nShowTrainEarlyLateFlag == 2)
	{
		if(!single->IsPassengerTrain()) 
			DrawTrainEarlyLastTime(pCDC,single,lColor,isPrint,m_nSchdStatus);
	}

}

int CPlotDoc::GetEngineCount(CTG_SingleTrain *single, UINT rec_idx)
{
	int nCountOfEngine = 0;
	int cnt = single->GetEngineCount();
	if( 0 == cnt)
	{
		nCountOfEngine = 0;
	}
	else
	{
		CTG_TrainEngineList* list = single->GetEngineList();

		for(int eCount = 0; eCount < cnt;eCount++)
		{
			if(IsTractionEngineExt(list->m_listRecords[eCount].m_data.engine_status))
				continue;

			if(list->m_listRecords[eCount].m_data.attach_sta <= rec_idx)
			{
				nCountOfEngine ++;
			}
			if(list->m_listRecords[eCount].m_data.unattach_sta <= rec_idx)
			{
				nCountOfEngine --;
			}
		}
	}

	return nCountOfEngine;
}

void CPlotDoc::DrawMutiTrainSeparate(CDC*pCDC, CTG_SingleTrain* single, CTG_TrainRecord* first, const int lColor, const int lActualColor)
{
	if(NULL == single || NULL == first)
		return;
    
	int historyWidth = (int)(m_nHistoryGraphWidth*m_fPixelPerMinute);

	CRect resultRect, rangeRect(0, 0, m_nPageWidth, m_nPageHeight);
	if(m_nSchdStatus == WORK_SCHEDULE || m_nSchdStatus == SHIFT_SCHEDULE)
	{
		rangeRect.SetRect(0, 0, m_nPageWidth, m_nPageHeight);
	}
	else if(m_nSchdStatus==HISTORY_SCHEDULE)
		rangeRect.SetRect(0, 0, historyWidth, m_nPageHeight);
	else if(m_nSchdStatus==BASIC_SCHEDULE)
		rangeRect.SetRect(0, 0, m_nPageWidth, m_nPageHeight);

	rangeRect.SetRect(0,0,m_fPixelPerHour* m_nViewTimeRange,m_nPageHeight);

	int first_d_time_status=0;
	if(first->m_data.adjust_status & 0x0002)//second bit data
		first_d_time_status = 1;//real time
	else
		first_d_time_status = 0;// plan time 

	int	x_point_offset = x_offset_print;
	int	y_point_offset = y_offset_print;

	CTG_TrainRecord* second = single->GetTrainRecordByRecIndex(first->GetIndex()+1);
	
	CPoint ptlStart;
	long time_depart,date_depart;
	time_depart = minute_time(first->m_data.depart,&date_depart);
	ptlStart.x = GetMinuteWidthToPlotStartTime(time_depart/60,date_depart,1,m_nSchdStatus)* m_fPixelPerMinute;
	ptlStart.y = GetStationVPosByIndex(single,first,PRIMARY_TRACK,EXIT_POSITION);
	if(second != NULL)
	{
		if(first->GetDepartTGIndex() > second->GetArriveTGIndex())
		{
			ptlStart.y = GetStationVPosByIndex(single,first,TOP_TRACK,EXIT_POSITION);
		}
		else
		{
			ptlStart.y = GetStationVPosByIndex(single,first,BOTTOM_TRACK,EXIT_POSITION);
		}
	}

	if(ptlStart.x < 0 || ptlStart.x > rangeRect.right)
	{
		return;
	}

	// 输出技术停点
	if(!TG_IsTechStopTrain(first->m_data) && m_bShowNotTechStopFlag)
	{
		CFont font;
		LOGFONT stFont;
		memset(&stFont,0,sizeof(stFont));
		int temp = -pCDC->GetDeviceCaps(LOGPIXELSY);

		stFont.lfHeight = MulDiv(6,-pCDC->GetDeviceCaps(LOGPIXELSY),72);
		stFont.lfWeight = FW_THIN;
		stFont.lfClipPrecision = CLIP_LH_ANGLES;

		strcpy(stFont.lfFaceName,"Arial Narrow");
		font.CreateFontIndirect(&stFont);
		CFont *pOldFont = pCDC->SelectObject(&font);
		UINT nOldColor = pCDC->SetTextColor(RGB(0,0,255));
		UINT nOldAlign = pCDC->SetTextAlign(TA_LEFT|TA_TOP);
		pCDC->TextOut(ptlStart.x+x_point_offset, ptlStart.y+y_point_offset, "▽");
		pCDC->SelectObject(pOldFont);
		pCDC->SetTextColor(nOldColor);
		pCDC->SetTextAlign(nOldAlign);
		font.DeleteObject();
	}

	// 输出折角车次
	if(CString(first->m_data.arrive_train_id) == first->m_data.depart_train_id)
		return;
	
	if(m_bShowCzhConnectLineOnPlot)
	{
		CTG_TrainRecord* last=single->GetTrainLastRecord();
		if((last==first) && last->IsTerminalRec())
			return;
	}
	CString strTrainId = first->m_data.depart_train_id;
	if(strTrainId.IsEmpty())
		return;

	UINT nOldAlign;
	UINT nOldMode = pCDC->SetBkMode(TRANSPARENT);	
	UINT nOldColor;
	CFont font;
	LOGFONT stFont;
	memset(&stFont,0,sizeof(stFont));
	int temp = -pCDC->GetDeviceCaps(LOGPIXELSY);

	stFont.lfHeight = MulDiv(m_font.train_id,-pCDC->GetDeviceCaps(LOGPIXELSY),72);
	stFont.lfWeight = FW_THIN;
	stFont.lfClipPrecision = CLIP_LH_ANGLES;

	strcpy(stFont.lfFaceName,"Arial Narrow");
	font.CreateFontIndirect(&stFont);
	CFont *pOldFont = pCDC->SelectObject(&font);
	if(first_d_time_status || (m_nSchdStatus == BASIC_SCHEDULE))
		nOldColor = pCDC->SetTextColor(lActualColor);
	else
		nOldColor = pCDC->SetTextColor(lColor);

    CSize trainidSize = pCDC->GetTextExtent(strTrainId);
	ptlStart.x = ptlStart.x + 4; 	

	nOldAlign = pCDC->SetTextAlign(TA_LEFT|TA_TOP);
	
	pCDC->TextOut(ptlStart.x+x_point_offset+trainidSize.cx/4, ptlStart.y+y_point_offset, strTrainId);

	pCDC->SelectObject(pOldFont);
	pCDC->SetTextAlign(nOldAlign);
	pCDC->SetBkMode(nOldMode);
	pCDC->SetTextColor(nOldColor);
	font.DeleteObject();	
}

void CPlotDoc::DrawBasicTrainConnectLine(CDC* pCDC, CTG_SingleTrain* train, CTG_TrainRecord* first, CTG_TrainRecord* prec, const CPoint& ptStart, const int lColor, BOOL isPrint, CString strTrainID)
{
	int scaleRate = min(yMultiple,5);

	int	x_point_offset = x_offset_print;
	int	y_point_offset = y_offset_print;

	CFont font;
	LOGFONT stFont;
	memset(&stFont,0,sizeof(stFont));
	int m_nLogPixelSy = pCDC->GetDeviceCaps(LOGPIXELSY);
	stFont.lfHeight = MulDiv(m_font.train_id,m_nLogPixelSy,72);
	stFont.lfWeight = FW_NORMAL;
	stFont.lfClipPrecision = CLIP_LH_ANGLES;
	strcpy(stFont.lfFaceName,"Times New Roman");
	font.CreateFontIndirect(&stFont);
	CPen mPen;
	mPen.CreatePen(PS_INSIDEFRAME, scaleRate*m_nLineWidthRate, lColor); //2
	
	CPen* pOldPen = pCDC->SelectObject(&mPen);
	CFont* pOldFont = pCDC->SelectObject(&font);
	UINT nOldAlign = pCDC->SetTextAlign(TA_RIGHT|TA_TOP);
	UINT nOldColor = pCDC->SetTextColor(lColor);

	CPoint ptEnd;
	long date_arrive = 0;
	long time_arrive = minute_time(prec->m_data.arrive,&date_arrive);
	ptEnd.x = (GetMinuteWidthToPlotStartTime(time_arrive/60,date_arrive,isPrint,m_nSchdStatus)* m_fPixelPerMinute);
	ptEnd.y = GetStationVPosByIndex(train, prec, PRIMARY_TRACK, EXIT_POSITION);
	ptEnd.Offset(x_point_offset,y_point_offset);

	int out_index=first->GetDepartTGIndex();
	int in_index=prec->GetArriveTGIndex();

	if(ptEnd.x<=ptStart.x)
	{
		if(out_index==in_index && in_index>=0)
		{
			pCDC->MoveTo(ptEnd);
			pCDC->LineTo(ptStart);
			pCDC->TextOut(ptStart.x, ptStart.y, strTrainID);
		}
		else if(in_index>=0 && out_index>=0 && in_index<m_nDispDistanceNum && out_index<m_nDispDistanceNum)
		{
			CPoint ptMiddle, ptTrainId;
			if(m_sDispDistance[in_index].distance.node_info.show_sides)
			{
				ptMiddle.x=ptStart.x;
				ptMiddle.y=ptEnd.y;
				ptTrainId=ptMiddle;
			}
			else if(m_sDispDistance[out_index].distance.node_info.show_sides)
			{
				ptMiddle.x=ptEnd.x;
				ptMiddle.y=ptStart.y;
				ptTrainId.x=ptStart.x;
				ptTrainId.y=ptStart.y;
			}
			else if(IsCRHReturnStation(first->m_data.station, out_index))
			{
				ptMiddle.x=ptEnd.x;
				ptMiddle.y=ptStart.y;
				ptTrainId.x=ptStart.x;
				ptTrainId.y=ptStart.y;
			}
			else if(IsCRHReturnStation(prec->m_data.station, in_index))
			{
				ptMiddle.x=ptStart.x;
				ptMiddle.y=ptEnd.y;
				ptTrainId=ptMiddle;
			}
			else
			{
				ptMiddle.x=ptStart.x;
				ptMiddle.y=ptEnd.y;
				ptTrainId=ptMiddle;
			}
			pCDC->MoveTo(ptEnd);
			pCDC->LineTo(ptMiddle);
			pCDC->LineTo(ptStart);
		
			pCDC->TextOut(ptTrainId.x, ptTrainId.y, strTrainID);
		}
	}
	else // 基本图过表
	{
		CString strBeforeTrainID=train->m_cBeforeTrainName, strComposeID;
		if(strBeforeTrainID!="")
		{
			strComposeID=strBeforeTrainID;
			strComposeID+="-";
			strComposeID+=strTrainID;
		}
		
		int nLength = 46;
		nLength = nLength*yMultiple;

		if(out_index==in_index && in_index>=0)
		{
			CPoint criticalstart;
			criticalstart.x = x_point_offset-nLength;
			criticalstart.y = ptStart.y;
			pCDC->SetTextAlign(TA_RIGHT|TA_TOP);
			pCDC->TextOut(ptStart.x, ptStart.y, strTrainID);
			pCDC->SetTextAlign(TA_LEFT|TA_TOP);
			pCDC->TextOut(criticalstart.x, criticalstart.y, strComposeID);

			pCDC->MoveTo(criticalstart);
			pCDC->LineTo(ptStart);
		
			CPoint criticalend;
			criticalend.x = (m_fPixelPerHour*24)+x_point_offset+nLength;
			criticalend.y = ptStart.y;
			
			pCDC->SetTextAlign(TA_LEFT|TA_TOP);
			pCDC->TextOut(ptEnd.x, ptEnd.y, strBeforeTrainID);
			pCDC->SetTextAlign(TA_RIGHT|TA_TOP);
			pCDC->TextOut(criticalend.x, criticalend.y, strComposeID);
			
			pCDC->MoveTo(ptEnd);
			pCDC->LineTo(criticalend);
		}
		else if(in_index>=0 && out_index>=0 && in_index<m_nDispDistanceNum && out_index<m_nDispDistanceNum)
		{
			CPoint ptMiddle;
			if(m_sDispDistance[in_index].distance.node_info.show_sides)
			{
				CPoint criticalstart;
				criticalstart.x = x_point_offset-nLength;
				criticalstart.y = ptEnd.y;

				ptMiddle.x=ptStart.x;
				ptMiddle.y=ptEnd.y;
				
				pCDC->MoveTo(ptStart);
				pCDC->LineTo(ptMiddle);
				pCDC->LineTo(criticalstart);
				pCDC->SetTextAlign(TA_RIGHT|TA_TOP);
				pCDC->TextOut(ptMiddle.x, ptMiddle.y, strTrainID);
				pCDC->SetTextAlign(TA_LEFT|TA_TOP);
				pCDC->TextOut(criticalstart.x, criticalstart.y, strComposeID);

				CPoint criticalend;
				criticalend.x = (m_fPixelPerHour*24)+x_point_offset+nLength;
				criticalend.y = ptEnd.y;
				
				pCDC->SetTextAlign(TA_LEFT|TA_TOP);
				pCDC->TextOut(ptEnd.x, ptEnd.y, strBeforeTrainID);
				pCDC->SetTextAlign(TA_RIGHT|TA_TOP);
				pCDC->TextOut(criticalend.x, criticalend.y, strComposeID);
				
				pCDC->MoveTo(ptEnd);
				pCDC->LineTo(criticalend);
			}
			else if(m_sDispDistance[out_index].distance.node_info.show_sides)
			{
				CPoint criticalstart;
				criticalstart.x = x_point_offset-nLength;
				criticalstart.y = ptStart.y;
				
				pCDC->SetTextAlign(TA_RIGHT|TA_TOP);
				pCDC->TextOut(ptStart.x, ptStart.y, strTrainID);
				pCDC->SetTextAlign(TA_LEFT|TA_TOP);
				pCDC->TextOut(criticalstart.x, criticalstart.y, strComposeID);
				
				pCDC->MoveTo(ptStart);
				pCDC->LineTo(criticalstart);

				CPoint criticalend;
				criticalend.x = (m_fPixelPerHour*24)+x_point_offset+nLength;
				criticalend.y = ptStart.y;

				ptMiddle.x=ptEnd.x;
				ptMiddle.y=ptStart.y;
				pCDC->SetTextAlign(TA_LEFT|TA_TOP);
				pCDC->TextOut(ptMiddle.x, ptMiddle.y, strBeforeTrainID);
				pCDC->SetTextAlign(TA_RIGHT|TA_TOP);
				pCDC->TextOut(criticalend.x, criticalend.y, strComposeID);

				pCDC->MoveTo(ptEnd);
				pCDC->LineTo(ptMiddle);
				pCDC->LineTo(criticalend);
			}
			else if(IsCRHReturnStation(first->m_data.station, out_index))
			{
				CPoint criticalstart;
				criticalstart.x = x_point_offset-nLength;
				criticalstart.y = ptStart.y;
				
				pCDC->SetTextAlign(TA_RIGHT|TA_TOP);
				pCDC->TextOut(ptStart.x, ptStart.y, strTrainID);
				pCDC->SetTextAlign(TA_LEFT|TA_TOP);
				pCDC->TextOut(criticalstart.x, criticalstart.y, strComposeID);
				
				pCDC->MoveTo(ptStart);
				pCDC->LineTo(criticalstart);

				CPoint criticalend;
				criticalend.x = (m_fPixelPerHour*24)+x_point_offset+nLength;
				criticalend.y = ptStart.y;

				ptMiddle.x=ptEnd.x;
				ptMiddle.y=ptStart.y;
				pCDC->SetTextAlign(TA_LEFT|TA_TOP);
				pCDC->TextOut(ptMiddle.x, ptMiddle.y, strBeforeTrainID);
				pCDC->SetTextAlign(TA_RIGHT|TA_TOP);
				pCDC->TextOut(criticalend.x, criticalend.y, strComposeID);

				pCDC->MoveTo(ptEnd);
				pCDC->LineTo(ptMiddle);
				pCDC->LineTo(criticalend);
			}
			else if(IsCRHReturnStation(prec->m_data.station, in_index))
			{
				CPoint criticalstart;
				criticalstart.x = x_point_offset-nLength;
				criticalstart.y = ptEnd.y;

				ptMiddle.x=ptStart.x;
				ptMiddle.y=ptEnd.y;
				
				pCDC->MoveTo(ptStart);
				pCDC->LineTo(ptMiddle);
				pCDC->LineTo(criticalstart);
				pCDC->SetTextAlign(TA_RIGHT|TA_TOP);
				pCDC->TextOut(ptMiddle.x, ptMiddle.y, strTrainID);
				pCDC->SetTextAlign(TA_LEFT|TA_TOP);
				pCDC->TextOut(criticalstart.x, criticalstart.y, strComposeID);

				CPoint criticalend;
				criticalend.x = (m_fPixelPerHour*24)+x_point_offset+nLength;
				criticalend.y = ptEnd.y;
				
				pCDC->SetTextAlign(TA_LEFT|TA_TOP);
				pCDC->TextOut(ptEnd.x, ptEnd.y, strBeforeTrainID);
				pCDC->SetTextAlign(TA_RIGHT|TA_TOP);
				pCDC->TextOut(criticalend.x, criticalend.y, strComposeID);
				
				pCDC->MoveTo(ptEnd);
				pCDC->LineTo(criticalend);
			}
			else
			{
				CPoint criticalstart;
				criticalstart.x = x_point_offset-nLength;
				criticalstart.y = ptEnd.y;

				ptMiddle.x=ptStart.x;
				ptMiddle.y=ptEnd.y;
				
				pCDC->MoveTo(ptStart);
				pCDC->LineTo(ptMiddle);
				pCDC->LineTo(criticalstart);
				
				pCDC->SetTextAlign(TA_RIGHT|TA_TOP);
				pCDC->TextOut(ptMiddle.x, ptMiddle.y, strTrainID);
				pCDC->SetTextAlign(TA_LEFT|TA_TOP);
				pCDC->TextOut(criticalstart.x, criticalstart.y, strComposeID);

				CPoint criticalend;
				criticalend.x = (m_fPixelPerHour*24)+x_point_offset+nLength;
				criticalend.y = ptEnd.y;
				
				pCDC->SetTextAlign(TA_LEFT|TA_TOP);
				pCDC->TextOut(ptEnd.x, ptEnd.y, strBeforeTrainID);
				pCDC->SetTextAlign(TA_RIGHT|TA_TOP);
				pCDC->TextOut(criticalend.x, criticalend.y, strComposeID);
				
				pCDC->MoveTo(ptEnd);
				pCDC->LineTo(criticalend);
			}
		}
	}
	pCDC->SelectObject(pOldFont);
	pCDC->SetTextAlign(nOldAlign);
	pCDC->SetTextColor(nOldColor);
	pCDC->SelectObject(pOldPen);

	font.DeleteObject();
	mPen.DeleteObject();
	
	return;
}

void CPlotDoc::DrawTrainConnectLine(CDC* pCDC, CTG_SingleTrain* train, CTG_TrainRecord* first, CTG_TrainRecord* prec, const CPoint& ptStart, const int lColor, BOOL isPrint, CString strTrainID)
{
	int scaleRate = min(yMultiple,5);

	int	x_point_offset = x_offset_print;
	int	y_point_offset = y_offset_print;

	CFont font;
	LOGFONT stFont;
	memset(&stFont,0,sizeof(stFont));
	int m_nLogPixelSy = pCDC->GetDeviceCaps(LOGPIXELSY);
	stFont.lfHeight = MulDiv(m_font.train_id,m_nLogPixelSy,72);
	stFont.lfWeight = FW_NORMAL;
	stFont.lfClipPrecision = CLIP_LH_ANGLES;
	strcpy(stFont.lfFaceName,"Times New Roman");
	font.CreateFontIndirect(&stFont);
	CPen mPen;
	mPen.CreatePen(PS_INSIDEFRAME, scaleRate*m_nLineWidthRate, lColor); //2
	
	CPen* pOldPen = pCDC->SelectObject(&mPen);
	CFont* pOldFont = pCDC->SelectObject(&font);
	UINT nOldAlign = pCDC->SetTextAlign(TA_RIGHT|TA_TOP);
	UINT nOldColor = pCDC->SetTextColor(lColor);

	CPoint ptEnd;
	long date_arrive = 0;
	long time_arrive = minute_time(prec->m_data.arrive,&date_arrive);
	ptEnd.x = (GetMinuteWidthToPlotStartTime(time_arrive/60,date_arrive,isPrint,m_nSchdStatus)* m_fPixelPerMinute);
	ptEnd.y = GetStationVPosByIndex(train, prec, PRIMARY_TRACK, EXIT_POSITION);
	ptEnd.Offset(x_point_offset,y_point_offset);

	int out_index=first->GetDepartTGIndex();
	int in_index=prec->GetArriveTGIndex();
	if(out_index==in_index && in_index>=0)
	{
		pCDC->MoveTo(ptEnd);
		pCDC->LineTo(ptStart);
		pCDC->TextOut(ptStart.x, ptStart.y, strTrainID);
	}
	else if(in_index>=0 && out_index>=0 && in_index<m_nDispDistanceNum && out_index<m_nDispDistanceNum)
	{
		CPoint ptMiddle, ptTrainId;
		if(m_sDispDistance[in_index].distance.node_info.show_sides)
		{
			ptMiddle.x=ptStart.x;
			ptMiddle.y=ptEnd.y;
			ptTrainId=ptMiddle;
		}
		else if(m_sDispDistance[out_index].distance.node_info.show_sides)
		{
			ptMiddle.x=ptEnd.x;
			ptMiddle.y=ptStart.y;
			ptTrainId.x=ptStart.x;
			ptTrainId.y=ptStart.y;
		}
		else if(IsCRHReturnStation(first->m_data.station, out_index))
		{
			ptMiddle.x=ptEnd.x;
			ptMiddle.y=ptStart.y;
			ptTrainId.x=ptStart.x;
			ptTrainId.y=ptStart.y;
		}
		else if(IsCRHReturnStation(prec->m_data.station, in_index))
		{
			ptMiddle.x=ptStart.x;
			ptMiddle.y=ptEnd.y;
			ptTrainId=ptMiddle;
		}
		else
		{
			ptMiddle.x=ptStart.x;
			ptMiddle.y=ptEnd.y;
			ptTrainId=ptMiddle;
		}

		pCDC->MoveTo(ptEnd);
		pCDC->LineTo(ptMiddle);
		pCDC->LineTo(ptStart);
	
		pCDC->TextOut(ptTrainId.x, ptTrainId.y, strTrainID);
	}
	
	pCDC->SelectObject(pOldFont);
	pCDC->SetTextAlign(nOldAlign);
	pCDC->SetTextColor(nOldColor);
	pCDC->SelectObject(pOldPen);

	font.DeleteObject();
	mPen.DeleteObject();
	
	return;
}


void CPlotDoc::DrawOutScopeConnectLine(CDC* pCDC, CTG_SingleTrain* train, const CPoint& ptlStart, const int lColor, int nDirect, const CRect& rangeRect)
{
	int scaleRate = min(yMultiple,5);
	int	x_point_offset = x_offset_print;
	int	y_point_offset = y_offset_print;

	CFont font;
	LOGFONT stFont;
	memset(&stFont,0,sizeof(stFont));
	int m_nLogPixelSy = pCDC->GetDeviceCaps(LOGPIXELSY);
	stFont.lfHeight = MulDiv(m_font.train_id,m_nLogPixelSy,72);
	stFont.lfWeight = FW_NORMAL;
	stFont.lfClipPrecision = CLIP_LH_ANGLES;
	strcpy(stFont.lfFaceName,"Times New Roman");
	font.CreateFontIndirect(&stFont);
	CPen mPen;
	mPen.CreatePen(PS_INSIDEFRAME, scaleRate*m_nLineWidthRate, lColor); //2
	
	CPen* pOldPen = pCDC->SelectObject(&mPen);
	CFont* pOldFont = pCDC->SelectObject(&font);
	UINT nOldAlign = pCDC->SetTextAlign(TA_RIGHT|TA_TOP);
	UINT nOldColor = pCDC->SetTextColor(lColor);

	int nLength = 46;
	nLength = nLength*yMultiple;

	if(nDirect==LEFT_MARK)
	{
		CString strComposeTrainID;
		CString strThisTrainID=train->GetTrainID();
		CString strBeforeTrainID=train->m_cBeforeTrainName;
		if(strBeforeTrainID!="")
		{
			strComposeTrainID=strBeforeTrainID;
			strComposeTrainID+="-";
			strComposeTrainID+=strThisTrainID;
		}
		
		pCDC->SetTextAlign(TA_RIGHT|TA_TOP);
		pCDC->TextOut(ptlStart.x, ptlStart.y, strThisTrainID);

		pCDC->SetTextAlign(TA_LEFT|TA_TOP);

		CPoint ptlEnd;
		ptlEnd.x = rangeRect.left;
		ptlEnd.Offset(x_point_offset,y_point_offset);
		ptlEnd.x = ptlEnd.x - nLength;
		ptlEnd.y = ptlStart.y;
		pCDC->TextOut(ptlEnd.x, ptlEnd.y, strComposeTrainID);

		pCDC->MoveTo(ptlStart);
		pCDC->LineTo(ptlEnd);
	}
	else
	{
		CString strComposeTrainID;
		CString strThisTrainID=train->GetTrainID();
		CString strAfterTrainID=train->m_cNextTrainName;
		if(strAfterTrainID!="")
		{
			strComposeTrainID=strThisTrainID;
			strComposeTrainID+="-";
			strComposeTrainID+=strAfterTrainID;
		}
		
		pCDC->SetTextAlign(TA_LEFT|TA_TOP);
		pCDC->TextOut(ptlStart.x, ptlStart.y, strThisTrainID);

		CPoint ptlEnd;
		ptlEnd.x = rangeRect.right;
		ptlEnd.Offset(x_point_offset,y_point_offset);
		ptlEnd.x = ptlEnd.x + nLength;
		ptlEnd.y = ptlStart.y;

		pCDC->SetTextAlign(TA_RIGHT|TA_TOP);
		pCDC->TextOut(ptlEnd.x, ptlEnd.y, strComposeTrainID);

		pCDC->MoveTo(ptlStart);
		pCDC->LineTo(ptlEnd);
	}

	pCDC->SelectObject(pOldFont);
	pCDC->SetTextAlign(nOldAlign);
	pCDC->SetTextColor(nOldColor);
	pCDC->SelectObject(pOldPen);

	font.DeleteObject();
	mPen.DeleteObject();
}

void CPlotDoc::DrawBasicTrainOutScopeConnectLine(CDC* pCDC, CTG_SingleTrain* train, CTG_TrainRecord* first, CTG_TrainRecord* prec, const CPoint& ptStart, const int lColor, const CRect& rangeRect)
{
	int scaleRate = min(yMultiple,5);

	int	x_point_offset = x_offset_print;
	int	y_point_offset = y_offset_print;

	BOOL isPrint=TRUE;
	CPoint ptEnd;
	long date_arrive = 0;
	long time_arrive = minute_time(prec->m_data.arrive,&date_arrive);
	ptEnd.x = (GetMinuteWidthToPlotStartTime(time_arrive/60,date_arrive,isPrint,m_nSchdStatus)* m_fPixelPerMinute);
	ptEnd.y = GetStationVPosByIndex(train, prec, PRIMARY_TRACK, EXIT_POSITION);
	ptEnd.Offset(x_point_offset,y_point_offset);
	
	if(ptEnd.x < x_point_offset || ptEnd.x >= rangeRect.right + x_point_offset)
	{
		return;
	}

	CFont font;
	LOGFONT stFont;
	memset(&stFont,0,sizeof(stFont));
	int m_nLogPixelSy = pCDC->GetDeviceCaps(LOGPIXELSY);
	stFont.lfHeight = MulDiv(m_font.train_id,m_nLogPixelSy,72);
	stFont.lfWeight = FW_NORMAL;
	stFont.lfClipPrecision = CLIP_LH_ANGLES;
	strcpy(stFont.lfFaceName,"Times New Roman");
	font.CreateFontIndirect(&stFont);
	CPen mPen;
	mPen.CreatePen(PS_INSIDEFRAME, scaleRate*m_nLineWidthRate, lColor); //2
	
	CPen* pOldPen = pCDC->SelectObject(&mPen);
	CFont* pOldFont = pCDC->SelectObject(&font);
	UINT nOldAlign = pCDC->SetTextAlign(TA_RIGHT|TA_TOP);
	UINT nOldColor = pCDC->SetTextColor(lColor);

	
	CString strTrainID;
	CString strThisTrainID=train->GetTrainID();
	CString strBeforeTrainID=train->m_cBeforeTrainName;
	if(strBeforeTrainID!="")
	{
		strTrainID=strBeforeTrainID;
		strTrainID+="-";
		strTrainID+=strThisTrainID;
	}
	else
		strTrainID=strBeforeTrainID;

	int nLength = 46;
	nLength = nLength*yMultiple;
	CPoint criticalend;
	criticalend.x = (m_fPixelPerHour*12)+x_point_offset+nLength;
	
	int out_index=first->GetDepartTGIndex();
	int in_index=prec->GetArriveTGIndex();
	if(out_index==in_index && in_index>=0)
	{
		pCDC->MoveTo(ptEnd);
		criticalend.y = ptEnd.y;
		pCDC->LineTo(criticalend);
		pCDC->TextOut(criticalend.x, criticalend.y, strTrainID);
	}
	else if(in_index>=0 && out_index>=0 && in_index<m_nDispDistanceNum && out_index<m_nDispDistanceNum)
	{
		CPoint ptMiddle, ptTrainId;
		if(m_sDispDistance[in_index].distance.node_info.show_sides)
		{
			ptMiddle.x=criticalend.x;
			ptMiddle.y=ptEnd.y;
		}
		else if(m_sDispDistance[out_index].distance.node_info.show_sides)
		{
			ptMiddle.x=ptEnd.x;
			ptMiddle.y=ptStart.y;
		}
		else if(IsCRHReturnStation(first->m_data.station, out_index))
		{
			ptMiddle.x=ptEnd.x;
			ptMiddle.y=ptStart.y;
		}
		else if(IsCRHReturnStation(prec->m_data.station, in_index))
		{
			ptMiddle.x=criticalend.x;
			ptMiddle.y=ptEnd.y;
		}
		else
		{
			ptMiddle.x=criticalend.x;
			ptMiddle.y=ptEnd.y;
		}
		criticalend.y = ptMiddle.y;
		
		pCDC->MoveTo(ptEnd);
		pCDC->LineTo(ptMiddle);
		pCDC->LineTo(criticalend);
	
		pCDC->TextOut(criticalend.x, criticalend.y, strTrainID);
	}
	
	pCDC->SelectObject(pOldFont);
	pCDC->SetTextAlign(nOldAlign);
	pCDC->SetTextColor(nOldColor);
	pCDC->SelectObject(pOldPen);

	font.DeleteObject();
	mPen.DeleteObject();
	
	return;
}

void CPlotDoc::DrawSingleTrain_New(CDC * pCDC, CTG_SingleTrain * single,int nDrawMode ,int nStartNo,int nPointNum,BOOL isPrint,int schd_type)
{
	int scaleRate = min(yMultiple,5);
	if(m_nShowTrainFlag != 2)
	{
		if(TG_IsPassengerTrainType(single->GetTrainType()) && m_nShowTrainFlag == 1)
		{
			return;
		}
		
		if(TG_IsFreightTrainType(single->GetTrainType())  && m_nShowTrainFlag == 0)
		{
			return;
		}
	}

	CString str_prefix;
	str_prefix.Format("%s",single->GetTrainID());
	if(str_prefix.Left(1) == "E" || str_prefix.Left(1) == "e")
		return;	
	int m_nLogPixelSy = pCDC->GetDeviceCaps(LOGPIXELSY);

	nStartNo = 0; 		
	int m_nPointCount = 0;
	TRAINPERPOINTINFO m_pPerPointInfo[300];

	int locCurrentSectionNo = 0; //当前要绘制的区段

	//小矩形的大小
	CSize sSize = CSize(8,8);
	ushort uDis = 4;
	CPoint critical_point;

	BOOL bad_Mark = FALSE;
	BOOL exception_Mark = FALSE;
	BOOL reverse_Mark = FALSE;
	BOOL stopReason_Mark = FALSE;
	BOOL bShowReverse = FALSE;

	CPoint single_ptFirst,single_ptSecond,single_ptThird;

	int	x_point_offset = x_offset_print;
	int	y_point_offset = y_offset_print;

	ushort nDirectFlag = 0;
	int m_nDirectFlag_prefix = 0;//列车上下行
	m_nDirectFlag_prefix = GetTrainRecDirect(single, 0);
	if(m_nDirectFlag_prefix == DOWN_DIRECT)
	{
		nDirectFlag = 2;
	}
	else
	{
		nDirectFlag = 1;
		m_nDirectFlag_prefix = UP_DIRECT;
	}

	if(schd_type == BASIC_SCHEDULE)
	{
		locCurrentSectionNo = m_nCurrentSectionNo;
		bShowReverse = FALSE;
	}
	else
	{
		locCurrentSectionNo = m_nCurrentSectionNo;
		bShowReverse = m_bShowReverse;
	}

	int usDrawCount = single->GetTrainRecordCount();

	int circleFlag = 0;
	int historyWidth = (int)(m_nHistoryGraphWidth*m_fPixelPerMinute);

	int wholedayWidth = (int)(WHOLE_DAY_MINUTE* m_fPixelPerMinute);
	CRect resultRect, rangeRect(0, 0, m_nPageWidth, m_nPageHeight);
	if(m_nSchdStatus == WORK_SCHEDULE || m_nSchdStatus == SHIFT_SCHEDULE)
	{
		rangeRect.SetRect(0, 0, m_nPageWidth, m_nPageHeight);
	}
	else if(m_nSchdStatus==HISTORY_SCHEDULE)
		rangeRect.SetRect(0, 0, historyWidth, m_nPageHeight);
	else if(m_nSchdStatus==BASIC_SCHEDULE)
		rangeRect.SetRect(0, 0, wholedayWidth, m_nPageHeight);


	if(isPrint == TRUE)
		rangeRect.SetRect(0,0,m_fPixelPerHour* m_nViewTimeRange,m_nPageHeight);

	// prepare gdi resource
	// line attribute
	CPen mThinPen, *pOldPen;
	int factor = single->GetTrainInfo()->m_data.type;

	COLORREF lColor = GetSchdColor(factor);
	COLORREF lActualColor = GetActualLineColor(factor);

	int nPreviousMode = pCDC->SetROP2(R2_COPYPEN);
	mThinPen.CreatePen(PS_SOLID, m_nLineWidthRate*scaleRate, lColor);
	pOldPen = pCDC->SelectObject(&mThinPen);
	// text attribute
	int nOldColor = pCDC->SetTextColor(lColor);
	int nOldBackMode = pCDC->SetBkMode(TRANSPARENT);
	int nOldAlign = pCDC->SetTextAlign(TA_RIGHT | TA_BOTTOM);

	// begin draw train line
	char time_char[2];
	CPoint ptlStart, ptlEnd;
	CTG_TrainRecord *first=NULL, *second=NULL;
	double flFirstMinuteOffset=0, flSecondMinuteOffset=0;
	long usStartDate=0, usStartTime=0, usEndDate=0, usEndTime=0;

	ushort usRouteSetNo = -1;

	//过表时改车次时候使用的变量
	int critial_record_index;
	CString NewTrainId = ""; //过表时候使用的新车次号

	ushort first_a_time_status=0xFFFF,second_a_time_status=0xFFFF;
	ushort first_d_time_status=0xFFFF,second_d_time_status=0xFFFF;

	second = single->GetTrainRecord(nStartNo);
	int first_primary_entry=0,first_primary_exit=0,first_bottom_entry=0,first_bottom_exit=0,first_top_entry=0,first_top_exit=0; 
	int second_primary_entry=0,second_primary_exit=0,second_bottom_entry=0,second_bottom_exit=0,second_top_entry=0,second_top_exit=0; 
	for(int i=nStartNo; i<usDrawCount; i++)//first
	{
		bad_Mark = FALSE;//MARK OF BAD ADD BY CF AFTER 7-18
		exception_Mark = FALSE;//MARK OF BAD ADD BY CF AFTER 7-18
		reverse_Mark  = FALSE;//MARK OF REVERSE ADD BY CF AFTER 7-18
		stopReason_Mark = FALSE;//MARK OF REVERSE ADD BY CF AFTER 7-18

		first = second;
		second = single->GetTrainRecord(i+1);
		if( first == NULL ) continue;

		//for actual time or plan time codeing   
		if(first->m_data.adjust_status & 0x0001)//first bit data
			first_a_time_status = 1;//real time
		else
			first_a_time_status = 0;// plan time 
		if(first->m_data.adjust_status & 0x0002)//second bit data
			first_d_time_status = 1;//real time
		else
			first_d_time_status = 0;// plan time 

		if(i==0 && IsFlagStart(first->m_data.flag))
			first_a_time_status = first_d_time_status;

		if(bShowReverse) //2004-03-20是否显示反向行车标志
		{
			if(IsFlagReverse(first->m_data.flag))
			{
				reverse_Mark  = TRUE;
			}
			if(second!=NULL)
			{
				if(first->m_data.station==second->m_data.station && IsFlagStopAbs(first->m_data.flag))
				{
					reverse_Mark  = FALSE;
				}
			}
		}

		if(second != NULL)
		{
			//for actual time or plan time codeing   
			if(second->m_data.adjust_status & 0x0001)//first bit data
				second_a_time_status = 1;//real time
			else
				second_a_time_status = 0;// plan time 
			if(second->m_data.adjust_status & 0x0002)//second bit data
				second_d_time_status = 1;//real time
			else
				second_d_time_status = 0;// plan time 
			if(IsFlagTerminal(second->m_data.flag))
				second_d_time_status = second_a_time_status;
		}

		int nCurIndex = 0;
		int first_index,second_index;
		first_index = second_index = -1;

		//对于绘制接入交出点的列车有用
		int nDirectFlag_pre =  nDirectFlag;//上半截车的方向
		int nDirectFlag_next = nDirectFlag;//下半截车的方向

		//加上这里是为了判断此车站是否有挂机车
		int m_nCountOfEngine = GetEngineCount(single, i);

		int first_in_index = first->GetArriveTGIndex();
		int first_out_index = first->GetDepartTGIndex();
		if(first_in_index<0 || first_in_index>=128 || first_out_index<0 || first_out_index>=128)
			continue;
	
		first_primary_entry = GetStationVPosByIndex(single,first,PRIMARY_TRACK,ENTRY_POSITION);
		first_primary_exit = GetStationVPosByIndex(single,first,PRIMARY_TRACK,EXIT_POSITION);

		first_bottom_entry = GetStationVPosByIndex(single, first, BOTTOM_TRACK, ENTRY_POSITION);
		first_bottom_exit = GetStationVPosByIndex(single, first, BOTTOM_TRACK, EXIT_POSITION);

		first_top_entry = GetStationVPosByIndex(single, first, TOP_TRACK, ENTRY_POSITION);
		first_top_exit = GetStationVPosByIndex(single, first, TOP_TRACK, EXIT_POSITION);

		if(second != NULL)
		{
			int second_sta_in_index = second->GetArriveTGIndex();
			int second_sta_out_index = second->GetDepartTGIndex();

			second_primary_entry = GetStationVPosByIndex(single,second,PRIMARY_TRACK,ENTRY_POSITION);
			second_primary_exit = GetStationVPosByIndex(single,second,PRIMARY_TRACK,EXIT_POSITION);
			second_bottom_entry = GetStationVPosByIndex(single, second, BOTTOM_TRACK, ENTRY_POSITION);
			second_bottom_exit = GetStationVPosByIndex(single, second, BOTTOM_TRACK, EXIT_POSITION);
			second_top_entry = GetStationVPosByIndex(single, second, TOP_TRACK, ENTRY_POSITION);
			second_top_exit = GetStationVPosByIndex(single, second, TOP_TRACK, EXIT_POSITION);
			if(first_primary_exit > second_primary_entry)
			{
				nDirectFlag = 1;
			}
			else
			{
				nDirectFlag = 2;
			}

			if(IsFlagStopAbs(first->m_data.flag)||IsFlagStopSignal(first->m_data.flag))
			{
				if(IsFlagStopAbsUp(first->m_data.flag))
					nDirectFlag = 1;
				else
					nDirectFlag = 2;
			}

			if(!(IsFlagStart(first->m_data.flag)||IsFlagTerminal(first->m_data.flag))
				&& first_primary_entry != first_primary_exit)
			{
				CTG_TrainRecord *pre=NULL;
				CTG_TrainRecord *next=NULL;

				if(i-1>=0)
					pre = single->GetTrainRecord(i-1); 

				if((i+1)<single->GetTrainRecordCount())
					next = single->GetTrainRecord(i+1); 

				if(pre != NULL )
				{
					int pre_sta_index = pre->GetDepartTGIndex();
					if(m_sDispDistance[pre_sta_index].nStationYPos > first_primary_entry)					
					{
						nDirectFlag_pre = UP_DIRECT;// up direct
						nDirectFlag = 1;
					}
					else							
					{
						nDirectFlag_pre = DOWN_DIRECT;// down direct 
						nDirectFlag = 2;
					}
				}

				//判断接入点的方向
				if(next != NULL )
				{
					int next_sta_index = next->GetArriveTGIndex();
					if(first_primary_exit > m_sDispDistance[next_sta_index].nStationYPos)					
						nDirectFlag_next = UP_DIRECT;// up direct 
					else							
						nDirectFlag_next = DOWN_DIRECT;// down direct 						
				}	
			}
			//2005-01-28
			else
			{
				nDirectFlag_pre = nDirectFlag_next = nDirectFlag==1?UP_DIRECT:DOWN_DIRECT;
			}
		}
		else
		{
			CTG_TrainRecord* first_pre = single->GetTrainRecord(i-1);
			if(first_pre != NULL)
			{
				int first_pre_sta_in_index = first_pre->GetArriveTGIndex();
				int first_pre_sta_out_index = first_pre->GetDepartTGIndex();

				int pre_primary_exit = GetStationVPosByIndex(single,first_pre,PRIMARY_TRACK,EXIT_POSITION);
				
				if(pre_primary_exit > first_primary_exit)
				{
					nDirectFlag = 1;
				}
				else
				{
					nDirectFlag = 2;
				}
			}
			else
			{
				//这里判断往返开的车最后一个点交出符号的结尾位置判断。
				if((i==usDrawCount-1)&&!IsFlagTerminal(first->m_data.flag)&&(first->m_data.exit != (ushort) -1))
				{
					int flag=GetEntryExitDirection(single,first,EXIT_POSITION,locCurrentSectionNo,schd_type);
					if(flag==UP_MARK)
					{
						nDirectFlag_next=UP_MARK;
						nDirectFlag=1;
					}
					else
					{
						nDirectFlag_next=DOWN_MARK;
						nDirectFlag=2;
					}
				}
			}
		} 
		// check whether valid or not
		if(first->m_data.arrive==NOTIME && first->m_data.depart==NOTIME)
			break;

		// draw stop station line
		usStartTime = minute_time(first->m_data.arrive,&usStartDate);
		if(first->m_data.depart == NOTIME || first->m_data.depart == 0)
		{
			usEndDate = usStartDate;
			usEndTime = usStartTime;
		}
		else
		{
			usEndTime = minute_time(first->m_data.depart,&usEndDate);
		}
		if(usEndTime>=WHOLE_DAY_SECOND)
		{
			usEndTime -= WHOLE_DAY_SECOND;
			usEndDate = OsGetNextDate(PMTG.ThreadId, usEndDate);
		}

		//draw decompose train
		if(IsFalgCombine(first->m_data.flag))
		{
			int ii,jj, nRecNum;
			CPoint ptFirst, ptSecond, ptThird;
			CTG_TrainRecord *rec;
			CTG_SingleTrain *pFirstTrain = NULL;

			//begin to compute ptFirst, ptSecond, ptThird
			if (i == usDrawCount-1)                        //如果是被分解的列车
			{
				for(jj = 0;jj < m_vecSchdTrains.size();jj++)
				{
					if(m_vecSchdTrains[jj].GetTrainIndex() == first->m_data.special_train_index)
					{
						pFirstTrain = &m_vecSchdTrains[jj];
						break;
					}
				}
				do 
				{
					if(pFirstTrain == NULL) continue;
					nRecNum = pFirstTrain ->GetTrainRecordCount() ;		
					for(ii = 0;ii < nRecNum;ii++)
					{
						rec = pFirstTrain->GetTrainRecord(ii);
						if(IsFlagEnd(rec->m_data.flag))
							break;
					}

					if(ii >= nRecNum) continue;
					long time_depart,date_depart,time_arrival,date_arrival;
					time_arrival = minute_time(first->m_data.arrive,&date_arrival);
					if(first->m_data.depart == NOTIME || first->m_data.depart == 0)
					{
						date_depart = date_arrival;
						time_depart = time_arrival;
					}
					else
					{
						time_depart = minute_time(first->m_data.depart,&date_depart);
					}
					long time_depart_rec,date_depart_rec,time_arrival_rec,date_arrival_rec;
					time_arrival_rec = minute_time(rec->m_data.arrive,&date_arrival_rec);
					if(rec->m_data.depart == NOTIME || rec->m_data.depart == 0)
					{
						date_depart_rec = date_arrival_rec;
						time_depart_rec = time_arrival_rec;
					}
					else
					{
						time_depart_rec = minute_time(rec->m_data.depart,&date_depart_rec);
					}
					flFirstMinuteOffset = GetMinuteWidthToPlotStartTime(time_depart/60,date_depart,isPrint,m_nSchdStatus);
					flSecondMinuteOffset = GetMinuteWidthToPlotStartTime(time_depart_rec/60,date_depart_rec,isPrint,m_nSchdStatus);
					CPoint fPoint,sPoint;
					if(nDirectFlag == 2)
					{// down direct
						fPoint.y = sPoint.y = first_bottom_exit  + 4;
					}
					else
					{
						fPoint.y = sPoint.y = first_top_exit - 4;
					}
					fPoint.x = (int)(flFirstMinuteOffset * m_fPixelPerMinute);
					sPoint.x = (int)(flSecondMinuteOffset * m_fPixelPerMinute);
					fPoint.Offset (x_point_offset,y_point_offset);
					sPoint.Offset (x_point_offset,y_point_offset);
					ptFirst = fPoint;
					ptSecond = sPoint;
				} while(0);
				if (NULL == rec) continue;

				for(jj = 0;jj < m_vecSchdTrains.size();jj++)
				{
					if(m_vecSchdTrains[jj].GetTrainIndex() == rec->m_data.special_train_index)
					{
						pFirstTrain = &m_vecSchdTrains[jj];
						break;
					}
				}

				do 
				{
					if (NULL == pFirstTrain) continue;
					nRecNum = pFirstTrain ->GetTrainRecordCount() ;		
					for(ii = 0;ii < nRecNum;ii++)
					{
						rec = pFirstTrain->GetTrainRecord(ii);
						if(IsFlagEnd(rec->m_data.flag))
							break;
					}

					if(ii >= nRecNum) continue;

					long time_depart,date_depart,time_arrival,date_arrival;
					time_arrival = minute_time(first->m_data.arrive,&date_arrival);
					if(first->m_data.depart == NOTIME || first->m_data.depart == 0)
					{
						date_depart = date_arrival;
						time_depart = time_arrival;
					}
					else
					{
						time_depart = minute_time(first->m_data.depart,&date_depart);
					}
					long time_depart_rec,date_depart_rec,time_arrival_rec,date_arrival_rec;
					time_arrival_rec = minute_time(rec->m_data.arrive,&date_arrival_rec);
					if(rec->m_data.depart == NOTIME || rec->m_data.depart == 0)
					{
						date_depart_rec = date_arrival_rec;
						time_depart_rec = time_arrival_rec;
					}
					else
					{
						time_depart_rec = minute_time(rec->m_data.depart,&date_depart_rec);
					}
					flFirstMinuteOffset = GetMinuteWidthToPlotStartTime(time_depart/60,date_depart,isPrint,m_nSchdStatus);
					flSecondMinuteOffset = GetMinuteWidthToPlotStartTime(time_depart_rec/60,date_depart_rec,isPrint,m_nSchdStatus);
					CPoint fPoint,sPoint;
					if(nDirectFlag == 2){// down direct
						fPoint.y = sPoint.y = first_bottom_exit  + 4;
					}else{
						fPoint.y = sPoint.y = first_top_exit - 4;
					}
					fPoint.x = (int)(flFirstMinuteOffset * m_fPixelPerMinute);
					sPoint.x = (int)(flSecondMinuteOffset * m_fPixelPerMinute);
					fPoint.Offset (x_point_offset,y_point_offset);
					sPoint.Offset (x_point_offset,y_point_offset);
					ptThird = sPoint;
				} while(0);
			}

			if (i == 0)						//如果是分解后的列车，有可能是分解后的第一列车，也有可能是分解后的第二列车
			{	
				for(jj = 0;jj < m_vecSchdTrains.size();jj++)
				{
					if(m_vecSchdTrains[jj].GetTrainIndex() == first->m_data.special_train_index){
						pFirstTrain = &m_vecSchdTrains[jj];
						break;
					}
				}
				do 
				{
					if (NULL == pFirstTrain) continue;
					nRecNum = pFirstTrain ->GetTrainRecordCount() ;		
					for(ii = 0;ii < nRecNum;ii++)
					{
						rec = pFirstTrain->GetTrainRecord(ii);
						if(IsFlagEnd(rec->m_data.flag))
							break;
					}

					if(ii >= nRecNum) continue;

					if (ii == 0)						//如果是分解后的第二列车
					{
						long time_depart,date_depart,time_arrival,date_arrival;
						time_arrival = minute_time(first->m_data.arrive,&date_arrival);
						if(first->m_data.depart == NOTIME || first->m_data.depart == 0)
						{
							date_depart = date_arrival;
							time_depart = time_arrival;
						}
						else
						{
							time_depart = minute_time(first->m_data.depart,&date_depart);
						}
						long time_depart_rec,date_depart_rec,time_arrival_rec,date_arrival_rec;
						time_arrival_rec = minute_time(rec->m_data.arrive,&date_arrival_rec);
						if(rec->m_data.depart == NOTIME || rec->m_data.depart == 0)
						{
							date_depart_rec = date_arrival_rec;
							time_depart_rec = time_arrival_rec;
						}
						else
						{
							time_depart_rec = minute_time(rec->m_data.depart,&date_depart_rec);
						}
						flFirstMinuteOffset = GetMinuteWidthToPlotStartTime(time_depart/60,date_depart,isPrint,m_nSchdStatus);
						flSecondMinuteOffset = GetMinuteWidthToPlotStartTime(time_depart_rec/60,date_depart_rec,isPrint,m_nSchdStatus);
						CPoint fPoint,sPoint;
						if(nDirectFlag == 2){// down direct
							fPoint.y = sPoint.y = first_bottom_exit  + 4;
						}else{
							fPoint.y = sPoint.y = first_top_exit - 4;
						}
						fPoint.x = (int)(flFirstMinuteOffset * m_fPixelPerMinute);
						sPoint.x = (int)(flSecondMinuteOffset * m_fPixelPerMinute);
						fPoint.Offset (x_point_offset,y_point_offset);
						sPoint.Offset (x_point_offset,y_point_offset);
						ptSecond = fPoint;
						ptThird = sPoint;

						for(jj = 0;jj < m_vecSchdTrains.size();jj++)
						{
							if(m_vecSchdTrains[jj].GetTrainIndex() == rec->m_data.special_train_index){
								pFirstTrain = &m_vecSchdTrains[jj];
								break;
							}
						}
						if(NULL != pFirstTrain)
						{
							nRecNum = pFirstTrain ->GetTrainRecordCount() ;
							for(ii = 0;ii < nRecNum;ii++)
							{
								rec = pFirstTrain->GetTrainRecord(ii);
								if(IsFlagEnd(rec->m_data.flag))
									break;
							}
							long time_depart,date_depart,time_arrival,date_arrival;
							time_arrival = minute_time(first->m_data.arrive,&date_arrival);
							if(first->m_data.depart == NOTIME || first->m_data.depart == 0)
							{
								date_depart = date_arrival;
								time_depart = time_arrival;
							}
							else
							{
								time_depart = minute_time(first->m_data.depart,&date_depart);
							}
							long time_depart_rec,date_depart_rec,time_arrival_rec,date_arrival_rec;
							time_arrival_rec = minute_time(rec->m_data.arrive,&date_arrival_rec);
							if(rec->m_data.depart == NOTIME || rec->m_data.depart == 0)
							{
								date_depart_rec = date_arrival_rec;
								time_depart_rec = time_arrival_rec;
							}
							else
							{
								time_depart_rec = minute_time(rec->m_data.depart,&date_depart_rec);
							}
							flFirstMinuteOffset = GetMinuteWidthToPlotStartTime(time_depart/60,date_depart,isPrint,m_nSchdStatus);
							flSecondMinuteOffset = GetMinuteWidthToPlotStartTime(time_depart_rec/ 60,date_depart_rec,isPrint,m_nSchdStatus);
							CPoint fPoint,sPoint;
							if(nDirectFlag == 2){// down direct
								fPoint.y = sPoint.y = first_bottom_exit  + 4;
							}else{
								fPoint.y = sPoint.y = first_top_exit - 4;
							}
							fPoint.x = (int)(flFirstMinuteOffset * m_fPixelPerMinute);
							sPoint.x = (int)(flSecondMinuteOffset * m_fPixelPerMinute);
							fPoint.Offset (x_point_offset,y_point_offset);
							sPoint.Offset (x_point_offset,y_point_offset);
							ptFirst = sPoint;
						}
					} 
					else if (ii == nRecNum - 1)						//如果是被分解列车
					{
						long time_depart,date_depart,time_arrival,date_arrival;
						time_arrival = minute_time(first->m_data.arrive,&date_arrival);
						if(first->m_data.depart == NOTIME || first->m_data.depart == 0)
						{
							date_depart = date_arrival;
							time_depart = time_arrival;
						}
						else
						{
							time_depart = minute_time(first->m_data.depart,&date_depart);
						}
						long time_depart_rec,date_depart_rec,time_arrival_rec,date_arrival_rec;
						time_arrival_rec = minute_time(rec->m_data.arrive,&date_arrival_rec);
						if(rec->m_data.depart == NOTIME || rec->m_data.depart == 0)
						{
							date_depart_rec = date_arrival_rec;
							time_depart_rec = time_arrival_rec;
						}
						else
						{
							time_depart_rec = minute_time(rec->m_data.depart,&date_depart_rec);
						}
						flFirstMinuteOffset = GetMinuteWidthToPlotStartTime(time_depart/60,date_depart,isPrint,m_nSchdStatus);
						flSecondMinuteOffset = GetMinuteWidthToPlotStartTime(time_depart_rec/60,date_depart_rec,isPrint,m_nSchdStatus);
						CPoint fPoint,sPoint;
						if(nDirectFlag == 2){// down direct
							fPoint.y = sPoint.y = first_bottom_exit  + 4;
						}else{
							fPoint.y = sPoint.y = first_top_exit - 4;
						}
						fPoint.x = (int)(flFirstMinuteOffset * m_fPixelPerMinute);
						sPoint.x = (int)(flSecondMinuteOffset * m_fPixelPerMinute);
						fPoint.Offset (x_point_offset,y_point_offset);
						sPoint.Offset (x_point_offset,y_point_offset);
						ptThird = fPoint;
						ptFirst = sPoint;

						for(jj = 0;jj < m_vecSchdTrains.size();jj++)
						{
							if(m_vecSchdTrains[jj].GetTrainIndex() == rec->m_data.special_train_index){
								pFirstTrain = &m_vecSchdTrains[jj];
								break;
							}
						}
						if(NULL != pFirstTrain)
						{
							nRecNum = pFirstTrain ->GetTrainRecordCount() ;
							for(ii = 0;ii < nRecNum;ii++)
							{
								rec = pFirstTrain->GetTrainRecord(ii);
								if(IsFlagEnd(rec->m_data.flag))
									break;
							}

							long time_depart,date_depart,time_arrival,date_arrival;
							time_arrival = minute_time(first->m_data.arrive,&date_arrival);
							if(first->m_data.depart == NOTIME || first->m_data.depart == 0)
							{
								date_depart = date_arrival;
								time_depart = time_arrival;
							}
							else
							{
								time_depart = minute_time(first->m_data.depart,&date_depart);
							}
							long time_depart_rec,date_depart_rec,time_arrival_rec,date_arrival_rec;
							time_arrival_rec = minute_time(rec->m_data.arrive,&date_arrival_rec);
							if(rec->m_data.depart == NOTIME || rec->m_data.depart == 0)
							{
								date_depart_rec = date_arrival_rec;
								time_depart_rec = time_arrival_rec;
							}
							else
							{
								time_depart_rec = minute_time(rec->m_data.depart,&date_depart_rec);
							}
							flFirstMinuteOffset = GetMinuteWidthToPlotStartTime(time_depart/60,date_depart,isPrint,m_nSchdStatus);
							flSecondMinuteOffset = GetMinuteWidthToPlotStartTime(time_depart_rec/60,date_depart_rec,isPrint,m_nSchdStatus);
							CPoint fPoint,sPoint;
							if(nDirectFlag == 2){// down direct
								fPoint.y = sPoint.y = first_bottom_exit  + 4;
							}else{
								fPoint.y = sPoint.y = first_top_exit - 4;
							}
							fPoint.x = (int)(flFirstMinuteOffset * m_fPixelPerMinute);
							sPoint.x = (int)(flSecondMinuteOffset * m_fPixelPerMinute);
							fPoint.Offset (x_point_offset,y_point_offset);
							sPoint.Offset (x_point_offset,y_point_offset);
							ptSecond = sPoint;
						}
					}
				} while(0);
			}
			single_ptFirst = ptFirst;
			single_ptSecond = ptSecond;
			single_ptThird = ptThird;
			//end to compute ptFirst, ptSecond, ptThird

			//begin to draw the line among divided trains
			CPen mPen,*pOldPen;
			mPen.CreatePen(PS_DOT,1,RGB(128,0,255));
			pOldPen = pCDC->SelectObject(&mPen);
			do 
			{
				if((single_ptFirst.x-x_point_offset) > rangeRect.right) break;
				if((single_ptSecond.x-x_point_offset) > rangeRect.right) single_ptSecond.x = rangeRect.right + x_point_offset;
				if((single_ptThird.x-x_point_offset) > rangeRect.right) single_ptThird.x = rangeRect.right + x_point_offset;

				if((single_ptThird.x-x_point_offset) < rangeRect.left) break;
				if((single_ptFirst.x-x_point_offset) < rangeRect.left)	single_ptFirst.x = rangeRect.left + x_point_offset;
				if((single_ptSecond.x-x_point_offset) < rangeRect.left)	single_ptSecond.x = rangeRect.left + x_point_offset;
				if(single_ptSecond.x <= single_ptThird.x)
				{
					if(single_ptFirst.x<=single_ptSecond.x)
					{

						if (m_bStopStationWaveLine)				//画波浪线
							DrawChordLine(pCDC,single_ptFirst,single_ptSecond,m_nDirectFlag_prefix);	
						else									//画直线
						{
							pCDC->MoveTo(single_ptFirst);
							pCDC->LineTo(single_ptSecond);
						}
					}
					else
					{
						if (m_bStopStationWaveLine)
							DrawChordLine(pCDC,single_ptSecond,single_ptFirst,m_nDirectFlag_prefix);
						else
						{
							pCDC->MoveTo(single_ptSecond);
							pCDC->LineTo(single_ptFirst);
						}
					}
				}
				else
				{
					if(single_ptFirst.x<=single_ptThird.x)
					{
						if (m_bStopStationWaveLine)
							DrawChordLine(pCDC,single_ptFirst,single_ptThird,m_nDirectFlag_prefix);
						else
						{
							pCDC->MoveTo(single_ptFirst);
							pCDC->LineTo(single_ptThird);
						}
					}
					else
					{
						if (m_bStopStationWaveLine)
							DrawChordLine(pCDC,single_ptThird,single_ptFirst,m_nDirectFlag_prefix);
						else
						{
							pCDC->MoveTo(single_ptThird);
							pCDC->LineTo(single_ptFirst);
						}
					}
				}
			} while(0);
			pCDC->SelectObject(pOldPen);
			mPen.DeleteObject();
			//end to draw the line among divided trains
		}
		//end of draw decompose train

		//draw Divide Train						added by zzg in ty 2008.05.10
		if(IsFlagDivide(first->m_data.flag))
		{
			int ii,jj, nRecNum;
			CPoint ptFirst, ptSecond, ptThird;
			CTG_TrainRecord *rec;
			CTG_SingleTrain *pFirstTrain = NULL;

			//begin to compute ptFirst, ptSecond, ptThird
			if (i == usDrawCount-1)                        //如果是被分解的列车
			{
				for(jj = 0;jj < m_vecSchdTrains.size();jj++)
				{
					if(m_vecSchdTrains[jj].GetTrainIndex() == first->m_data.special_train_index){
						pFirstTrain = &m_vecSchdTrains[jj];
						break;
					}
				}
				do 
				{
					if(pFirstTrain == NULL) continue;
					nRecNum = pFirstTrain ->GetTrainRecordCount() ;		
					for(ii = 0;ii < nRecNum;ii++)
					{
						rec = pFirstTrain->GetTrainRecord(ii);
						if(IsFlagDivide(rec->m_data.flag))
							break;
					}

					if(ii >= nRecNum) continue;
					long time_depart,date_depart,time_arrival,date_arrival;
					time_arrival = minute_time(first->m_data.arrive,&date_arrival);
					if(first->m_data.depart == NOTIME || first->m_data.depart == 0)
					{
						date_depart = date_arrival;
						time_depart = time_arrival;
					}
					else
					{
						time_depart = minute_time(first->m_data.depart,&date_depart);
					}
					long time_depart_rec,date_depart_rec,time_arrival_rec,date_arrival_rec;
					time_arrival_rec = minute_time(rec->m_data.arrive,&date_arrival_rec);
					if(rec->m_data.depart == NOTIME || rec->m_data.depart == 0)
					{
						date_depart_rec = date_arrival_rec;
						time_depart_rec = time_arrival_rec;
					}
					else
					{
						time_depart_rec = minute_time(rec->m_data.depart,&date_depart_rec);
					}
					flFirstMinuteOffset = GetMinuteWidthToPlotStartTime(time_depart/60,date_depart,isPrint,m_nSchdStatus);
					flSecondMinuteOffset = GetMinuteWidthToPlotStartTime(time_depart_rec/60,date_depart_rec,isPrint,m_nSchdStatus);
					CPoint fPoint,sPoint;
					if(nDirectFlag == 2){// down direct
						fPoint.y = sPoint.y = first_bottom_exit  + 4;
					}else{
						fPoint.y = sPoint.y = first_top_exit - 4;
					}
					fPoint.x = (int)(flFirstMinuteOffset * m_fPixelPerMinute);
					sPoint.x = (int)(flSecondMinuteOffset * m_fPixelPerMinute);
					fPoint.Offset (x_point_offset,y_point_offset);
					sPoint.Offset (x_point_offset,y_point_offset);
					ptFirst = fPoint;
					ptSecond = sPoint;
				} while(0);
				if (NULL == rec) continue;

				for(jj = 0;jj < m_vecSchdTrains.size();jj++)
				{
					if(m_vecSchdTrains[jj].GetTrainIndex() == rec->m_data.special_train_index){
						pFirstTrain = &m_vecSchdTrains[jj];
						break;
					}
				}

				do 
				{
					if (NULL == pFirstTrain) continue;
					nRecNum = pFirstTrain ->GetTrainRecordCount() ;		
					for(ii = 0;ii < nRecNum;ii++)
					{
						rec = pFirstTrain->GetTrainRecord(ii);
						if(IsFlagDivide(rec->m_data.flag))
							break;
					}

					if(ii >= nRecNum) continue;

					long time_depart,date_depart,time_arrival,date_arrival;
					time_arrival = minute_time(first->m_data.arrive,&date_arrival);
					if(first->m_data.depart == NOTIME || first->m_data.depart == 0)
					{
						date_depart = date_arrival;
						time_depart = time_arrival;
					}
					else
					{
						time_depart = minute_time(first->m_data.depart,&date_depart);
					}
					long time_depart_rec,date_depart_rec,time_arrival_rec,date_arrival_rec;
					time_arrival_rec = minute_time(rec->m_data.arrive,&date_arrival_rec);
					if(rec->m_data.depart == NOTIME || rec->m_data.depart == 0)
					{
						date_depart_rec = date_arrival_rec;
						time_depart_rec = time_arrival_rec;
					}
					else
					{
						time_depart_rec = minute_time(rec->m_data.depart,&date_depart_rec);
					}
					flFirstMinuteOffset = GetMinuteWidthToPlotStartTime(time_depart/60,date_depart,isPrint,m_nSchdStatus);
					flSecondMinuteOffset = GetMinuteWidthToPlotStartTime(time_depart_rec/60,date_depart_rec,isPrint,m_nSchdStatus);
					CPoint fPoint,sPoint;
					if(nDirectFlag == 2){// down direct
						fPoint.y = sPoint.y = first_bottom_exit  + 4;
					}else{
						fPoint.y = sPoint.y = first_top_exit - 4;
					}
					fPoint.x = (int)(flFirstMinuteOffset * m_fPixelPerMinute);
					sPoint.x = (int)(flSecondMinuteOffset * m_fPixelPerMinute);
					fPoint.Offset (x_point_offset,y_point_offset);
					sPoint.Offset (x_point_offset,y_point_offset);
					ptThird = sPoint;
				} while(0);
			}

			if (i == 0)						//如果是分解后的列车，有可能是分解后的第一列车，也有可能是分解后的第二列车
			{	
				for(jj = 0;jj < m_vecSchdTrains.size();jj++)
				{
					if(m_vecSchdTrains[jj].GetTrainIndex() == first->m_data.special_train_index){
						pFirstTrain = &m_vecSchdTrains[jj];
						break;
					}
				}
				do 
				{
					if (NULL == pFirstTrain) continue;
					nRecNum = pFirstTrain ->GetTrainRecordCount() ;		
					for(ii = 0;ii < nRecNum;ii++)
					{
						rec = pFirstTrain->GetTrainRecord(ii);
						if(IsFlagDivide(rec->m_data.flag))
							break;
					}

					if(ii >= nRecNum) continue;

					if (ii == 0)						//如果是分解后的第二列车
					{
						long time_depart,date_depart,time_arrival,date_arrival;
						time_arrival = minute_time(first->m_data.arrive,&date_arrival);
						if(first->m_data.depart == NOTIME || first->m_data.depart == 0)
						{
							date_depart = date_arrival;
							time_depart = time_arrival;
						}
						else
						{
							time_depart = minute_time(first->m_data.depart,&date_depart);
						}
						long time_depart_rec,date_depart_rec,time_arrival_rec,date_arrival_rec;
						time_arrival_rec = minute_time(rec->m_data.arrive,&date_arrival_rec);
						if(rec->m_data.depart == NOTIME || rec->m_data.depart == 0)
						{
							date_depart_rec = date_arrival_rec;
							time_depart_rec = time_arrival_rec;
						}
						else
						{
							time_depart_rec = minute_time(rec->m_data.depart,&date_depart_rec);
						}
						flFirstMinuteOffset = GetMinuteWidthToPlotStartTime(time_depart/60,date_depart,isPrint,m_nSchdStatus);
						flSecondMinuteOffset = GetMinuteWidthToPlotStartTime(time_depart_rec/60,date_depart_rec,isPrint,m_nSchdStatus);
						CPoint fPoint,sPoint;
						if(nDirectFlag == 2){// down direct
							fPoint.y = sPoint.y = first_bottom_exit  + 4;
						}else{
							fPoint.y = sPoint.y = first_top_exit - 4;
						}
						fPoint.x = (int)(flFirstMinuteOffset * m_fPixelPerMinute);
						sPoint.x = (int)(flSecondMinuteOffset * m_fPixelPerMinute);
						fPoint.Offset (x_point_offset,y_point_offset);
						sPoint.Offset (x_point_offset,y_point_offset);
						ptSecond = fPoint;
						ptThird = sPoint;

						for(jj = 0;jj < m_vecSchdTrains.size();jj++)
						{
							if(m_vecSchdTrains[jj].GetTrainIndex() == rec->m_data.special_train_index){
								pFirstTrain = &m_vecSchdTrains[jj];
								break;
							}
						}
						if(NULL != pFirstTrain)
						{
							nRecNum = pFirstTrain ->GetTrainRecordCount() ;
							for(ii = 0;ii < nRecNum;ii++)
							{
								rec = pFirstTrain->GetTrainRecord(ii);
								if(IsFlagDivide(rec->m_data.flag))
									break;
							}
							long time_depart,date_depart,time_arrival,date_arrival;
							time_arrival = minute_time(first->m_data.arrive,&date_arrival);
							if(first->m_data.depart == NOTIME || first->m_data.depart == 0)
							{
								date_depart = date_arrival;
								time_depart = time_arrival;
							}
							else
							{
								time_depart = minute_time(first->m_data.depart,&date_depart);
							}
							long time_depart_rec,date_depart_rec,time_arrival_rec,date_arrival_rec;
							time_arrival_rec = minute_time(rec->m_data.arrive,&date_arrival_rec);
							if(rec->m_data.depart == NOTIME || rec->m_data.depart == 0)
							{
								date_depart_rec = date_arrival_rec;
								time_depart_rec = time_arrival_rec;
							}
							else
							{
								time_depart_rec = minute_time(rec->m_data.depart,&date_depart_rec);
							}
							flFirstMinuteOffset = GetMinuteWidthToPlotStartTime(time_depart/60,date_depart,isPrint,m_nSchdStatus);
							flSecondMinuteOffset = GetMinuteWidthToPlotStartTime(time_depart_rec/ 60,date_depart_rec,isPrint,m_nSchdStatus);
							CPoint fPoint,sPoint;
							if(nDirectFlag == 2){// down direct
								fPoint.y = sPoint.y = first_bottom_exit  + 4;
							}else{
								fPoint.y = sPoint.y = first_top_exit - 4;
							}
							fPoint.x = (int)(flFirstMinuteOffset * m_fPixelPerMinute);
							sPoint.x = (int)(flSecondMinuteOffset * m_fPixelPerMinute);
							fPoint.Offset (x_point_offset,y_point_offset);
							sPoint.Offset (x_point_offset,y_point_offset);
							ptFirst = sPoint;
						}
					} 
					else if (ii == nRecNum - 1)						//如果是被分解列车
					{
						long time_depart,date_depart,time_arrival,date_arrival;
						time_arrival = minute_time(first->m_data.arrive,&date_arrival);
						if(first->m_data.depart == NOTIME || first->m_data.depart == 0)
						{
							date_depart = date_arrival;
							time_depart = time_arrival;
						}
						else
						{
							time_depart = minute_time(first->m_data.depart,&date_depart);
						}
						long time_depart_rec,date_depart_rec,time_arrival_rec,date_arrival_rec;
						time_arrival_rec = minute_time(rec->m_data.arrive,&date_arrival_rec);
						if(rec->m_data.depart == NOTIME || rec->m_data.depart == 0)
						{
							date_depart_rec = date_arrival_rec;
							time_depart_rec = time_arrival_rec;
						}
						else
						{
							time_depart_rec = minute_time(rec->m_data.depart,&date_depart_rec);
						}
						flFirstMinuteOffset = GetMinuteWidthToPlotStartTime(time_depart/60,date_depart,isPrint,m_nSchdStatus);
						flSecondMinuteOffset = GetMinuteWidthToPlotStartTime(time_depart_rec/60,date_depart_rec,isPrint,m_nSchdStatus);
						CPoint fPoint,sPoint;
						if(nDirectFlag == 2){// down direct
							fPoint.y = sPoint.y = first_bottom_exit  + 4;
						}else{
							fPoint.y = sPoint.y = first_top_exit - 4;
						}
						fPoint.x = (int)(flFirstMinuteOffset * m_fPixelPerMinute);
						sPoint.x = (int)(flSecondMinuteOffset * m_fPixelPerMinute);
						fPoint.Offset (x_point_offset,y_point_offset);
						sPoint.Offset (x_point_offset,y_point_offset);
						ptThird = fPoint;
						ptFirst = sPoint;

						for(jj = 0;jj < m_vecSchdTrains.size();jj++)
						{
							if(m_vecSchdTrains[jj].GetTrainIndex() == rec->m_data.special_train_index){
								pFirstTrain = &m_vecSchdTrains[jj];
								break;
							}
						}
						if(NULL != pFirstTrain)
						{
							nRecNum = pFirstTrain ->GetTrainRecordCount() ;
							for(ii = 0;ii < nRecNum;ii++)
							{
								rec = pFirstTrain->GetTrainRecord(ii);
								if(IsFlagDivide(rec->m_data.flag))
									break;
							}

							long time_depart,date_depart,time_arrival,date_arrival;
							time_arrival = minute_time(first->m_data.arrive,&date_arrival);
							if(first->m_data.depart == NOTIME || first->m_data.depart == 0)
							{
								date_depart = date_arrival;
								time_depart = time_arrival;
							}
							else
							{
								time_depart = minute_time(first->m_data.depart,&date_depart);
							}
							long time_depart_rec,date_depart_rec,time_arrival_rec,date_arrival_rec;
							time_arrival_rec = minute_time(rec->m_data.arrive,&date_arrival_rec);
							if(rec->m_data.depart == NOTIME || rec->m_data.depart == 0)
							{
								date_depart_rec = date_arrival_rec;
								time_depart_rec = time_arrival_rec;
							}
							else
							{
								time_depart_rec = minute_time(rec->m_data.depart,&date_depart_rec);
							}
							flFirstMinuteOffset = GetMinuteWidthToPlotStartTime(time_depart/60,date_depart,isPrint,m_nSchdStatus);
							flSecondMinuteOffset = GetMinuteWidthToPlotStartTime(time_depart_rec/60,date_depart_rec,isPrint,m_nSchdStatus);
							CPoint fPoint,sPoint;
							if(nDirectFlag == 2){// down direct
								fPoint.y = sPoint.y = first_bottom_exit  + 4;
							}else{
								fPoint.y = sPoint.y = first_top_exit - 4;
							}
							fPoint.x = (int)(flFirstMinuteOffset * m_fPixelPerMinute);
							sPoint.x = (int)(flSecondMinuteOffset * m_fPixelPerMinute);
							fPoint.Offset (x_point_offset,y_point_offset);
							sPoint.Offset (x_point_offset,y_point_offset);
							ptSecond = sPoint;
						}
					}
				} while(0);
			}
			single_ptFirst = ptFirst;
			single_ptSecond = ptSecond;
			single_ptThird = ptThird;
			//end to compute ptFirst, ptSecond, ptThird

			//begin to draw the line among divided trains
			CPen mPen,*pOldPen;
			mPen.CreatePen(PS_DOT,1,RGB(128,0,255));
			pOldPen = pCDC->SelectObject(&mPen);
			do 
			{
				if((single_ptFirst.x-x_point_offset) > rangeRect.right) break;
				if((single_ptSecond.x-x_point_offset) > rangeRect.right) single_ptSecond.x = rangeRect.right + x_point_offset;
				if((single_ptThird.x-x_point_offset) > rangeRect.right) single_ptThird.x = rangeRect.right + x_point_offset;

				if((single_ptThird.x-x_point_offset) < rangeRect.left) break;
				if((single_ptFirst.x-x_point_offset) < rangeRect.left)	single_ptFirst.x = rangeRect.left + x_point_offset;
				if((single_ptSecond.x-x_point_offset) < rangeRect.left)	single_ptSecond.x = rangeRect.left + x_point_offset;

				if(single_ptFirst.x<=single_ptSecond.x)
				{

					if (m_bStopStationWaveLine)				//画波浪线
						DrawChordLine(pCDC,single_ptFirst,single_ptSecond,m_nDirectFlag_prefix);	
					else									//画直线
					{
						pCDC->MoveTo(single_ptFirst);
						pCDC->LineTo(single_ptSecond);
					}
				}
				else
				{
					if (m_bStopStationWaveLine)
						DrawChordLine(pCDC,single_ptSecond,single_ptFirst,m_nDirectFlag_prefix);
					else
					{
						pCDC->MoveTo(single_ptSecond);
						pCDC->LineTo(single_ptFirst);
					}
				}

				if(single_ptFirst.x<=single_ptThird.x)
				{
					if (m_bStopStationWaveLine)
						DrawChordLine(pCDC,single_ptFirst,single_ptThird,m_nDirectFlag_prefix);
					else
					{
						pCDC->MoveTo(single_ptFirst);
						pCDC->LineTo(single_ptThird);
					}
				}
				else
				{
					if (m_bStopStationWaveLine)
						DrawChordLine(pCDC,single_ptThird,single_ptFirst,m_nDirectFlag_prefix);
					else
					{
						pCDC->MoveTo(single_ptThird);
						pCDC->LineTo(single_ptFirst);
					}
				}
			} while(0);
			pCDC->SelectObject(pOldPen);
			mPen.DeleteObject();
			//end to draw the line among divided trains
		}
		//end of draw Divide Train

		//draw Combine Train						added by zzg in ty 2008.05.11
		//if(IsFlagUnite(first->m_data.flag))
		if(IsFlagUnitOtherTrain(first->m_data.flag))
		{
			int ii, jj, nRecNum;
			CPoint ptFirst, ptSecond, ptThird;
			CTG_TrainRecord *rec;
			CTG_SingleTrain *pFirstTrain = NULL;

			if (i == usDrawCount-1)							//如果是被组合的列车
			{
				for(jj = 0;jj < m_vecSchdTrains.size();jj++)
				{
					if(m_vecSchdTrains[jj].GetTrainIndex() == first->m_data.special_train_index){
						pFirstTrain = &m_vecSchdTrains[jj];
						break;
					}
				}
				do 
				{
					if (NULL == pFirstTrain) continue;
					nRecNum = pFirstTrain ->GetTrainRecordCount() ;		
					for(ii = 0;ii < nRecNum;ii++)
					{
						rec = pFirstTrain->GetTrainRecord(ii);
						if(IsFalgBeUnitedByOther(rec->m_data.flag))
							break;
					}

					if(ii >= nRecNum) continue;

					//绘制合并列车的虚线
					for(int combine_num = ii;combine_num<nRecNum-1;combine_num++)
					{
						CTG_TrainRecord *rec_combine1,*rec_combine2;
						rec_combine1 = pFirstTrain->GetTrainRecord(combine_num);
						rec_combine2 = pFirstTrain->GetTrainRecord(combine_num+1);
						long time_depart,date_depart,time_arrival,date_arrival;
						time_arrival = minute_time(rec_combine1->m_data.arrive,&date_arrival);
						if(rec_combine1->m_data.depart == NOTIME || rec_combine1->m_data.depart == 0)
						{
							date_depart = date_arrival;
							time_depart = time_arrival;
						}
						else
						{
							time_depart = minute_time(rec_combine1->m_data.depart,&date_depart);
						}
						long time_arrival_rec,date_arrival_rec;
						time_arrival_rec = minute_time(rec_combine2->m_data.arrive,&date_arrival_rec);
						flFirstMinuteOffset = GetMinuteWidthToPlotStartTime(time_depart/60,date_depart,isPrint,m_nSchdStatus);
						flSecondMinuteOffset = GetMinuteWidthToPlotStartTime(time_arrival_rec/60,date_arrival_rec,isPrint,m_nSchdStatus);
						CPoint fPoint,sPoint;

						if(nDirectFlag == 2)
						{// down direct
							fPoint.y = GetStationVPos(pFirstTrain,rec_combine1, BOTTOM_TRACK, EXIT_POSITION,schd_type);
							sPoint.y = GetStationVPos(pFirstTrain,rec_combine2, TOP_TRACK, ENTRY_POSITION,schd_type);
						}
						else
						{
							fPoint.y = GetStationVPos(pFirstTrain,rec_combine1, TOP_TRACK, EXIT_POSITION,schd_type);
							sPoint.y = GetStationVPos(pFirstTrain,rec_combine2, BOTTOM_TRACK, ENTRY_POSITION,schd_type);
						}
						fPoint.x = (int)(flFirstMinuteOffset * m_fPixelPerMinute)-4;
						sPoint.x = (int)(flSecondMinuteOffset * m_fPixelPerMinute)-4;
						if( sPoint.x > rangeRect.right)
							break;
						fPoint.Offset (x_point_offset,y_point_offset);
						sPoint.Offset (x_point_offset,y_point_offset);
						CPen mPen,*pOldPen;
						mPen.CreatePen(PS_DOT,1,lColor);
						pOldPen = pCDC->SelectObject(&mPen);
						pCDC->MoveTo(fPoint);
						pCDC->LineTo(sPoint);
						pCDC->SelectObject(pOldPen);
						mPen.DeleteObject();
					}

					//		if (ii == 0)							//如果single指向的列车是组合后的列车
					if(ii !=nRecNum - 1)
					{
						long time_depart,date_depart,time_arrival,date_arrival;
						time_arrival = minute_time(first->m_data.arrive,&date_arrival);
						if(first->m_data.depart == NOTIME || first->m_data.depart == 0)
						{
							date_depart = date_arrival;
							time_depart = time_arrival;
						}
						else
						{
							time_depart = minute_time(first->m_data.depart,&date_depart);
						}
						long time_depart_rec,date_depart_rec,time_arrival_rec,date_arrival_rec;
						time_arrival_rec = minute_time(rec->m_data.arrive,&date_arrival_rec);
						if(rec->m_data.depart == NOTIME || rec->m_data.depart == 0)
						{
							date_depart_rec = date_arrival_rec;
							time_depart_rec = time_arrival_rec;
						}
						else
						{
							time_depart_rec = minute_time(rec->m_data.depart,&date_depart_rec);
						}
						flFirstMinuteOffset = GetMinuteWidthToPlotStartTime(time_depart/60,date_depart,isPrint,m_nSchdStatus);
						flSecondMinuteOffset = GetMinuteWidthToPlotStartTime(time_depart_rec/60,date_depart_rec,isPrint,m_nSchdStatus);
						CPoint fPoint,sPoint;
						if(nDirectFlag == 2){// down direct
							fPoint.y = sPoint.y = first_bottom_exit  + 4;
						}else{
							fPoint.y = sPoint.y = first_top_exit - 4;
						}
						fPoint.x = (int)(flFirstMinuteOffset * m_fPixelPerMinute);
						sPoint.x = (int)(flSecondMinuteOffset * m_fPixelPerMinute);
						fPoint.Offset (x_point_offset,y_point_offset);
						sPoint.Offset (x_point_offset,y_point_offset);

						ptSecond = fPoint;
						ptThird = sPoint;

						for(jj = 0;jj < m_vecSchdTrains.size();jj++)
						{
							if(m_vecSchdTrains[jj].GetTrainIndex() == rec->m_data.special_train_index){
								pFirstTrain = &m_vecSchdTrains[jj];
								break;
							}
						}
						if(NULL != pFirstTrain)
						{
							nRecNum = pFirstTrain ->GetTrainRecordCount() ;
							for(ii = 0;ii < nRecNum;ii++)
							{
								rec = pFirstTrain->GetTrainRecord(ii);
								if(IsFlagUnitOtherTrain(rec->m_data.flag))
									break;
							}

							long time_depart,date_depart,time_arrival,date_arrival;
							time_arrival = minute_time(first->m_data.arrive,&date_arrival);
							if(first->m_data.depart == NOTIME || first->m_data.depart == 0)
							{
								date_depart = date_arrival;
								time_depart = time_arrival;
							}
							else
							{
								time_depart = minute_time(first->m_data.depart,&date_depart);
							}
							long time_depart_rec,date_depart_rec,time_arrival_rec,date_arrival_rec;
							time_arrival_rec = minute_time(rec->m_data.arrive,&date_arrival_rec);
							if(rec->m_data.depart == NOTIME || rec->m_data.depart == 0)
							{
								date_depart_rec = date_arrival_rec;
								time_depart_rec = time_arrival_rec;
							}
							else
							{
								time_depart_rec = minute_time(rec->m_data.depart,&date_depart_rec);
							}
							flFirstMinuteOffset = GetMinuteWidthToPlotStartTime(time_depart/60,date_depart,isPrint,m_nSchdStatus);
							flSecondMinuteOffset = GetMinuteWidthToPlotStartTime(time_depart_rec/60,date_depart_rec,isPrint,m_nSchdStatus);
							CPoint fPoint,sPoint;
							if(nDirectFlag == 2){// down direct
								fPoint.y = sPoint.y = first_bottom_exit  + 4;
							}else{
								fPoint.y = sPoint.y = first_top_exit - 4;
							}
							fPoint.x = (int)(flFirstMinuteOffset * m_fPixelPerMinute);
							sPoint.x = (int)(flSecondMinuteOffset * m_fPixelPerMinute);
							fPoint.Offset (x_point_offset,y_point_offset);
							sPoint.Offset (x_point_offset,y_point_offset);
							ptFirst = sPoint;
						}
					} 
					//	else if (ii == nRecNum - 1)							//如果single指向的是被组合的第一列车
					else
					{
						long time_depart,date_depart,time_arrival,date_arrival;
						time_arrival = minute_time(first->m_data.arrive,&date_arrival);
						if(first->m_data.depart == NOTIME || first->m_data.depart == 0)
						{
							date_depart = date_arrival;
							time_depart = time_arrival;
						}
						else
						{
							time_depart = minute_time(first->m_data.depart,&date_depart);
						}
						long time_depart_rec,date_depart_rec,time_arrival_rec,date_arrival_rec;
						time_arrival_rec = minute_time(rec->m_data.arrive,&date_arrival_rec);
						if(rec->m_data.depart == NOTIME || rec->m_data.depart == 0)
						{
							date_depart_rec = date_arrival_rec;
							time_depart_rec = time_arrival_rec;
						}
						else
						{
							time_depart_rec = minute_time(rec->m_data.depart,&date_depart_rec);
						}
						flFirstMinuteOffset = GetMinuteWidthToPlotStartTime(time_depart/60,date_depart,isPrint,m_nSchdStatus);
						flSecondMinuteOffset = GetMinuteWidthToPlotStartTime(date_depart_rec/60,time_depart_rec,isPrint,m_nSchdStatus);
						CPoint fPoint,sPoint;
						if(nDirectFlag == 2){// down direct
							fPoint.y = sPoint.y = first_bottom_exit  + 4;
						}else{
							fPoint.y = sPoint.y = first_top_exit - 4;
						}
						fPoint.x = (int)(flFirstMinuteOffset * m_fPixelPerMinute);
						sPoint.x = (int)(flSecondMinuteOffset * m_fPixelPerMinute);
						fPoint.Offset (x_point_offset,y_point_offset);
						sPoint.Offset (x_point_offset,y_point_offset);

						ptFirst = fPoint;
						ptSecond = sPoint;

						for(jj = 0;jj < m_vecSchdTrains.size();jj++)
						{
							if(m_vecSchdTrains[jj].GetTrainIndex() == rec->m_data.special_train_index){
								pFirstTrain = &m_vecSchdTrains[jj];
								break;
							}
						}
						if(NULL != pFirstTrain)
						{
							nRecNum = pFirstTrain ->GetTrainRecordCount() ;
							for(ii = 0;ii < nRecNum;ii++)
							{
								rec = pFirstTrain->GetTrainRecord(ii);
								if(IsFalgBeUnitedByOther(rec->m_data.flag))
									break;
							}
							long time_depart,date_depart,time_arrival,date_arrival;
							time_arrival = minute_time(first->m_data.arrive,&date_arrival);
							if(first->m_data.depart == NOTIME || first->m_data.depart == 0)
							{
								date_depart = date_arrival;
								time_depart = time_arrival;
							}
							else
							{
								time_depart = minute_time(first->m_data.depart,&date_depart);
							}
							long time_depart_rec,date_depart_rec,time_arrival_rec,date_arrival_rec;
							time_arrival_rec = minute_time(rec->m_data.arrive,&date_arrival_rec);
							if(rec->m_data.depart == NOTIME || rec->m_data.depart == 0)
							{
								date_depart_rec = date_arrival_rec;
								time_depart_rec = time_arrival_rec;
							}
							else
							{
								time_depart_rec = minute_time(rec->m_data.depart,&date_depart_rec);
							}
							flFirstMinuteOffset = GetMinuteWidthToPlotStartTime(time_depart/60,date_depart,isPrint,m_nSchdStatus);
							flSecondMinuteOffset = GetMinuteWidthToPlotStartTime(time_depart_rec/60,date_depart_rec,isPrint,m_nSchdStatus);
							CPoint fPoint,sPoint;
							if(nDirectFlag == 2){// down direct
								fPoint.y = sPoint.y = first_bottom_exit  + 4;
							}else{
								fPoint.y = sPoint.y = first_top_exit - 4;
							}
							fPoint.x = (int)(flFirstMinuteOffset * m_fPixelPerMinute);
							sPoint.x = (int)(flSecondMinuteOffset * m_fPixelPerMinute);
							fPoint.Offset (x_point_offset,y_point_offset);
							sPoint.Offset (x_point_offset,y_point_offset);
							//2009 blm unite train
							CPen mPen,*pOldPen;
							mPen.CreatePen(PS_DOT,1,lColor);
							pOldPen = pCDC->SelectObject(&mPen);
							pCDC->MoveTo(fPoint);
							pCDC->LineTo(sPoint);
							pCDC->SelectObject(pOldPen);
							mPen.DeleteObject();

							ptThird = sPoint;
						}
					}

				} while(0);
			}

			if (i == 0)										//如果single是组合后的列车
			{
				for(jj = 0;jj < m_vecSchdTrains.size();jj++)
				{
					if(m_vecSchdTrains[jj].GetTrainIndex() == first->m_data.special_train_index){
						pFirstTrain = &m_vecSchdTrains[jj];
						break;
					}
				}
				do 
				{
					if(pFirstTrain == NULL) continue;
					nRecNum = pFirstTrain ->GetTrainRecordCount() ;		
					for(ii = 0;ii < nRecNum;ii++)
					{
						rec = pFirstTrain->GetTrainRecord(ii);
						if(IsFalgBeUnitedByOther(rec->m_data.flag))
							break;
					}

					if(ii >= nRecNum) continue;

					long time_depart,date_depart,time_arrival,date_arrival;
					time_arrival = minute_time(first->m_data.arrive,&date_arrival);
					if(first->m_data.depart == NOTIME || first->m_data.depart == 0)
					{
						date_depart = date_arrival;
						time_depart = time_arrival;
					}
					else
					{
						time_depart = minute_time(first->m_data.depart,&date_depart);
					}
					long time_depart_rec,date_depart_rec,time_arrival_rec,date_arrival_rec;
					time_arrival_rec = minute_time(rec->m_data.arrive,&date_arrival_rec);
					if(rec->m_data.depart == NOTIME || rec->m_data.depart == 0)
					{
						date_depart_rec = date_arrival_rec;
						time_depart_rec = time_arrival_rec;
					}
					else
					{
						time_depart_rec = minute_time(rec->m_data.depart,&date_depart_rec);
					}
					flFirstMinuteOffset = GetMinuteWidthToPlotStartTime(time_depart/60,date_depart,isPrint,m_nSchdStatus);
					flSecondMinuteOffset = GetMinuteWidthToPlotStartTime(time_depart_rec/60,date_depart_rec,isPrint,m_nSchdStatus);
					CPoint fPoint,sPoint;
					if(nDirectFlag == 2){// down direct
						fPoint.y = sPoint.y = first_bottom_exit  + 4;
					}else{
						fPoint.y = sPoint.y = first_top_exit - 4;
					}
					fPoint.x = (int)(flFirstMinuteOffset * m_fPixelPerMinute);
					sPoint.x = (int)(flSecondMinuteOffset * m_fPixelPerMinute);
					fPoint.Offset (x_point_offset,y_point_offset);
					sPoint.Offset (x_point_offset,y_point_offset);
					ptThird = fPoint;
					ptFirst = sPoint;
				} while(0);

				if (NULL == rec) continue;

				for(jj = 0;jj < m_vecSchdTrains.size();jj++)
				{
					if(m_vecSchdTrains[jj].GetTrainIndex() == rec->m_data.special_train_index){
						pFirstTrain = &m_vecSchdTrains[jj];
						break;
					}
				}

				do 
				{
					if (NULL == pFirstTrain) continue;
					nRecNum = pFirstTrain ->GetTrainRecordCount() ;		
					for(ii = 0;ii < nRecNum;ii++)
					{
						rec = pFirstTrain->GetTrainRecord(ii);
						if(IsFalgBeUnitedByOther(rec->m_data.flag))
							break;
					}

					if(ii >= nRecNum) continue;

					long time_depart,date_depart,time_arrival,date_arrival;
					time_arrival = minute_time(first->m_data.arrive,&date_arrival);
					if(first->m_data.depart == NOTIME || first->m_data.depart == 0)
					{
						date_depart = date_arrival;
						time_depart = time_arrival;
					}
					else
					{
						time_depart = minute_time(first->m_data.depart,&date_depart);
					}
					long time_depart_rec,date_depart_rec,time_arrival_rec,date_arrival_rec;
					time_arrival_rec = minute_time(rec->m_data.arrive,&date_arrival_rec);
					if(rec->m_data.depart == NOTIME || rec->m_data.depart == 0)
					{
						date_depart_rec = date_arrival_rec;
						time_depart_rec = time_arrival_rec;
					}
					else
					{
						time_depart_rec = minute_time(rec->m_data.depart,&date_depart_rec);
					}
					flFirstMinuteOffset = GetMinuteWidthToPlotStartTime(time_depart/60,date_depart,isPrint,m_nSchdStatus);
					flSecondMinuteOffset = GetMinuteWidthToPlotStartTime(time_depart_rec/ 60,date_depart_rec,isPrint,m_nSchdStatus);
					CPoint fPoint,sPoint;
					if(nDirectFlag == 2){// down direct
						fPoint.y = sPoint.y = first_bottom_exit  + 4;
					}else{
						fPoint.y = sPoint.y = first_top_exit - 4;
					}
					fPoint.x = (int)(flFirstMinuteOffset * m_fPixelPerMinute);
					sPoint.x = (int)(flSecondMinuteOffset * m_fPixelPerMinute);
					fPoint.Offset (x_point_offset,y_point_offset);
					sPoint.Offset (x_point_offset,y_point_offset);
					ptSecond = sPoint;
				} while(0);
			}
			single_ptFirst = ptFirst;
			single_ptSecond = ptSecond;
			single_ptThird = ptThird;
			//end to compute ptFirst, ptSecond, ptThird

			//begin to draw the line among combined trains
			CPen mPen,*pOldPen;
			//		mPen.CreatePen(PS_SOLID,1,RGB(128,0,255));
			mPen.CreatePen(PS_DOT,1,lColor);
			pOldPen = pCDC->SelectObject(&mPen);
			do 
			{
				if((single_ptFirst.x-x_point_offset) > rangeRect.right) break;
				if((single_ptThird.x-x_point_offset) > rangeRect.right) single_ptThird.x = rangeRect.right + x_point_offset;

				if((single_ptThird.x-x_point_offset) < rangeRect.left) break;
				if((single_ptFirst.x-x_point_offset) < rangeRect.left)	single_ptFirst.x = rangeRect.left + x_point_offset;
				if((single_ptSecond.x-x_point_offset) < rangeRect.left)	single_ptSecond.x = rangeRect.left + x_point_offset;

				if(single_ptFirst.x<=single_ptThird.x)
				{
					if (m_bStopStationWaveLine)				//画波浪线
						DrawChordLine(pCDC,single_ptFirst,single_ptThird,m_nDirectFlag_prefix);	
					else									//画直线
					{
						pCDC->MoveTo(single_ptFirst);
						pCDC->LineTo(single_ptThird);
					}
				}
				else
				{
					if (m_bStopStationWaveLine)
						DrawChordLine(pCDC,single_ptThird,single_ptFirst,m_nDirectFlag_prefix);
					else
					{
						pCDC->MoveTo(single_ptThird);
						pCDC->LineTo(single_ptFirst);
					}
				}

				if(single_ptSecond.x<=single_ptThird.x)
				{
					if (m_bStopStationWaveLine)
						DrawChordLine(pCDC,single_ptSecond,single_ptThird,m_nDirectFlag_prefix);
					else
					{
						pCDC->MoveTo(single_ptSecond);
						pCDC->LineTo(single_ptThird);
					}
				}
				else
				{
					if (m_bStopStationWaveLine)
						DrawChordLine(pCDC,single_ptThird,single_ptSecond,m_nDirectFlag_prefix);
					else
					{
						pCDC->MoveTo(single_ptThird);
						pCDC->LineTo(single_ptSecond);
					}
				}
			} while(0);
			pCDC->SelectObject(pOldPen);
			mPen.DeleteObject();
		}
		//end of draw Combine Train
		// draw train other trainid
		if(mPlotStyle.train_id>0 && NULL != first)
		{
			DrawMutiTrainSeparate(pCDC, single, first, lColor, lActualColor);
		}
		//draw train start,accept,terminal,giveout,etc mark
		if(i == 0)
		{
			int direct_flag=-1; // BUG!!! UMR
			if(IsFlagStopAbs(first->m_data.flag))
			{
				if(IsFlagStopAbsUp(first->m_data.flag))
					direct_flag = DOWN_MARK;
				else
					direct_flag = UP_MARK; 
			}
			else
			{
				if(nDirectFlag == 1)
					direct_flag = DOWN_MARK;
				else
					direct_flag = UP_MARK;
			}

			long time_arrival,date_arrival;
			time_arrival = minute_time(first->m_data.arrive,&date_arrival);

			flFirstMinuteOffset = GetMinuteWidthToPlotStartTime(time_arrival/60,date_arrival,isPrint,m_nSchdStatus);
			ptlStart.x =  (int)(flFirstMinuteOffset * m_fPixelPerMinute);
			if(nDirectFlag == 2)// DOWN_DIRECT
				ptlStart.y = first_top_entry;
			else
				ptlStart.y = first_bottom_entry;
			ptlStart.Offset (x_point_offset,y_point_offset);
			if(IsFlagStart(first->m_data.flag))
			{
				// 太原接入交出问题
				if(second!=NULL)
				{
					int out_index=first->GetDepartTGIndex();
					int in_index=second->GetArriveTGIndex();
					if (out_index==in_index) 
					{
						if (nDirectFlag==1)//上行车
							direct_flag = DOWN_MARK;
						else//下行车
							direct_flag = UP_MARK;
					}	
					else if(out_index>in_index)
						direct_flag = DOWN_MARK;
					else
						direct_flag = UP_MARK;
				}
				else
				{
					int flag = GetEntryExitDirection(single,first,(ushort)EXIT_POSITION,locCurrentSectionNo);
					if(flag != -1)
					{
						if(flag == UP_MARK)
							direct_flag = DOWN_MARK;
						else
							direct_flag = UP_MARK;
					}
				}
			
				
				long time_depart,date_depart,time_arrival,date_arrival;
				time_arrival = minute_time(first->m_data.arrive,&date_arrival);
				if(first->m_data.depart == NOTIME || first->m_data.depart == 0)
				{
					date_depart = date_arrival;
					time_depart = time_arrival;
				}
				else
				{
					time_depart = minute_time(first->m_data.depart,&date_depart);
				}
				flFirstMinuteOffset = GetMinuteWidthToPlotStartTime(time_depart/60,date_depart,isPrint,m_nSchdStatus);
				ptlStart.x =  (int)(flFirstMinuteOffset * m_fPixelPerMinute);		
				ptlStart.y =  (direct_flag == UP_MARK) ? first_primary_entry : first_primary_exit;
				ptlStart.Offset (x_point_offset,y_point_offset);

				if(ptlStart.x >= x_point_offset && ptlStart.x<= rangeRect.right + x_point_offset && ptlStart.y >= y_point_offset)
				{
					bool bDrawStart=true;
					if(m_bShowCzhConnectLineOnPlot)
					{
						if(single->m_nBeforeTrainIndex>0 && false==IsCRHStopStation(first->m_data.station))
						{
							CTG_SingleTrain*ptrain=GetTrain(single->m_nBeforeTrainIndex);
							if(ptrain)
							{
								CTG_TrainRecord*prec=ptrain->GetTrainLastRecord();
								if(prec!=NULL&&prec->IsTerminalRec() && prec->m_data.station==first->m_data.station)
								{
									bDrawStart=false;
									CString strTrainID=single->GetTrainID();
									if(BASIC_SCHEDULE == m_nSchdStatus)
									{
										DrawBasicTrainConnectLine(pCDC, single, first, prec, ptlStart, lActualColor, isPrint, strTrainID);
									}
									else
									{
										if(first_a_time_status)
											DrawTrainConnectLine(pCDC, single, first, prec, ptlStart, lActualColor, isPrint, strTrainID);
										else
											DrawTrainConnectLine(pCDC, single, first, prec, ptlStart, lColor, isPrint, strTrainID);
									}
								}
							}
							else
							{
								bDrawStart=false;
								if(second_d_time_status)
									DrawOutScopeConnectLine(pCDC, single, ptlStart, lActualColor, LEFT_MARK, rangeRect);
								else
									DrawOutScopeConnectLine(pCDC, single, ptlStart, lColor, LEFT_MARK, rangeRect);
							}
						}
					}
					if(bDrawStart)
					{
						if(first_a_time_status)
						{
							CString tempTrain(single->GetTrainRecord(0)->m_data.original_arrive_train_id);
							if(tempTrain.IsEmpty())
							{
								DrawMarkOfStart(pCDC,single,CString(single->GetTrainID()),ptlStart,10,direct_flag, lActualColor,TRUE,schd_type );
							}
							else
							{
								tempTrain.Empty();
								tempTrain = single->GetTrainID();
								tempTrain += "(";
								tempTrain += single->GetTrainRecord(0)->m_data.original_arrive_train_id;
								tempTrain += ")";
								DrawMarkOfStart(pCDC,single,tempTrain,ptlStart,10,direct_flag, lActualColor,TRUE,schd_type);
							}
						}
						else
						{
							CString tempTrain(single->GetTrainRecord(0)->m_data.original_arrive_train_id);
							if(tempTrain.IsEmpty())
							{
								DrawMarkOfStart(pCDC,single,CString(single->GetTrainID()),ptlStart,10,direct_flag, lColor,FALSE,schd_type );
							}
							else
							{
								tempTrain.Empty();
								tempTrain = single->GetTrainID();
								tempTrain += "(";
								tempTrain += single->GetTrainRecord(0)->m_data.original_arrive_train_id;
								tempTrain += ")";
								DrawMarkOfStart(pCDC,single,tempTrain,ptlStart,10,direct_flag, lColor,TRUE,schd_type);
							}
						}
					}
				}
				else if(BASIC_SCHEDULE == m_nSchdStatus && m_nViewTimeRange==12 && m_bShowCzhConnectLineOnPlot)
				{
					if(single->m_nBeforeTrainIndex>0 && false==IsCRHStopStation(first->m_data.station))
					{
						CTG_SingleTrain*ptrain=GetTrain(single->m_nBeforeTrainIndex);
						if(ptrain)
						{
							CTG_TrainRecord*prec=ptrain->GetTrainLastRecord();
							if(prec!=NULL&&prec->IsTerminalRec()&&prec->m_data.station==first->m_data.station)
							{
								if(first_a_time_status)
									DrawBasicTrainOutScopeConnectLine(pCDC, single, first, prec, ptlStart, lActualColor, rangeRect);
								else
									DrawBasicTrainOutScopeConnectLine(pCDC, single, first, prec, ptlStart, lColor, rangeRect);
							}
						}
					}
				}
			}
			else if(IsFlagAbsStart(first->m_data.flag) )
			{
				if(ptlStart.x<rangeRect.right + x_point_offset && ptlStart.y>= y_point_offset)
				{
					direct_flag = DOWN_MARK;
					int nDirect = UP_DIRECT;
					if(IsAbsStartStopUp(single->GetTrainInfo()->m_data))
					{
						direct_flag = UP_MARK;
						nDirect = UP_DIRECT;
					}
					else
					{	
						direct_flag = DOWN_MARK; 
						nDirect = DOWN_DIRECT;
					}

					long time_depart,date_depart,time_arrival,date_arrival;
					time_arrival = minute_time(first->m_data.arrive,&date_arrival);
					if(first->m_data.depart == NOTIME || first->m_data.depart == 0)
					{
						date_depart = date_arrival;
						time_depart = time_arrival;
					}
					else
					{
						time_depart = minute_time(first->m_data.depart,&date_depart);
					}
					flFirstMinuteOffset = GetMinuteWidthToPlotStartTime(time_arrival/60,date_arrival,isPrint,m_nSchdStatus);
					long stop_abs_time = (single->GetTrainInfo()->m_data.adjust_flag&0xffff0000);
					stop_abs_time = stop_abs_time>>16;
					ptlStart.x =(int)(flFirstMinuteOffset - stop_abs_time)  * m_fPixelPerMinute;
					ptlStart.y = GetAbsVPos(single,first,0, ENTRY_POSITION,nDirect,m_nSchdStatus); 

					ptlEnd.x = flFirstMinuteOffset * m_fPixelPerMinute;
					ptlEnd.y = GetStationVPos(single,first,PRIMARY_TRACK,ENTRY_POSITION,m_nSchdStatus);

					ptlStart.Offset (x_point_offset,y_point_offset);
					ptlEnd.Offset (x_point_offset,y_point_offset);

					if(first_a_time_status)
					{
						if(IsAbsAttainEngineReturn(single->GetTrainInfo()->m_data.adjust_flag))
						{
						}
						else
						{
						CString tempTrain(single->GetTrainRecord(0)->m_data.original_arrive_train_id);
						if(tempTrain.IsEmpty())
						{
							DrawMarkOfStart(pCDC,single,CString(single->GetTrainID()),ptlStart,10,
								direct_flag,lActualColor,TRUE,schd_type );
						}
						else
						{
							tempTrain.Empty();
							tempTrain = single->GetTrainID();
							tempTrain += "(";
							tempTrain += single->GetTrainRecord(0)->m_data.original_arrive_train_id;
							tempTrain += ")";
							DrawMarkOfStart(pCDC,single,tempTrain,ptlStart,10,direct_flag,lActualColor,TRUE,schd_type);
						}
						}

						DrawActualLine(pCDC, ptlStart, ptlEnd, single->GetTrainInfo()->m_data.type,FALSE,m_nCountOfEngine);
					}
					else
					{
						if(IsAbsAttainEngineReturn(single->GetTrainInfo()->m_data.adjust_flag))
						{
						}
						else
						{
						CString tempTrain(single->GetTrainRecord(0)->m_data.original_arrive_train_id);
						if(tempTrain.IsEmpty())
						{ 
							DrawMarkOfStart(pCDC,single,CString(single->GetTrainID()),
								ptlStart,10,direct_flag,lColor,FALSE,schd_type );
						}
						else
						{
							tempTrain.Empty();
							tempTrain = single->GetTrainID();
							tempTrain += "(";
							tempTrain += single->GetTrainRecord(0)->m_data.original_arrive_train_id;
							tempTrain += ")";
							DrawMarkOfStart(pCDC,single,tempTrain,ptlStart,10,direct_flag,lColor,FALSE,schd_type);
						}
						}
						DrawActualLine(pCDC, ptlStart, ptlEnd, single->GetTrainInfo()->m_data.type,FALSE,m_nCountOfEngine, 0,FALSE);
					}

					CFont* pOldFont, font;
					LOGFONT stFont;
					memset(&stFont,0,sizeof(stFont));
					stFont.lfHeight = MulDiv(m_font.time_char,m_nLogPixelSy,72); // time_char
					stFont.lfWeight = FW_NORMAL;
					stFont.lfClipPrecision = CLIP_LH_ANGLES;
					strcpy(stFont.lfFaceName,"Times New Roman");
					font.CreateFontIndirect(&stFont);

					int text_align;
					UINT nOldColor, nColor;

					pOldFont = pCDC->SelectObject(&font);
					if(manualRealColor>0 && TG_IsDepartActualByManual(first->m_data))
					{
						nColor = manualRealColor;
					}
					else
					{
						if(first_a_time_status == 1)
							nColor = lActualColor;
						else
							nColor = lColor;
					}
					
					nOldColor = pCDC->SetTextColor(nColor);

					CPoint point = ptlStart;
					
					if(nDirect  == UP_DIRECT) //下行
						text_align = pCDC->SetTextAlign(TA_RIGHT|TA_TOP);
					else
						text_align = pCDC->SetTextAlign(TA_RIGHT|TA_BOTTOM);	

					int dlt = ((int)flFirstMinuteOffset - stop_abs_time)%10;
					if(dlt<0)
						dlt=0;
					sprintf(time_char, "%d",  dlt);
					time_char[1]=0;
					if(point.x >0 && point.x < rangeRect.right && !IsAbsAttainEngineReturn(single->GetTrainInfo()->m_data.adjust_flag))				
					{
						pCDC->TextOut((int)point.x, point.y, time_char, 1); 
					}

					pCDC->SetTextColor(nOldColor);
				}
			}
			else
			{  
				if(ptlStart.x >= x_point_offset && ptlStart.x<= rangeRect.right + x_point_offset && ptlStart.y >= y_point_offset)
				{
					// 太原接入交出问题
					if(second!=NULL)
					{
						int out_index=first->GetDepartTGIndex();
						int in_index=second->GetArriveTGIndex();
						if (out_index==in_index) 
						{
							if (nDirectFlag==1)//上行车
								direct_flag = DOWN_MARK;
							else//下行车
								direct_flag = UP_MARK;
						}
						else if(out_index>in_index)
							direct_flag = DOWN_MARK;
						else
							direct_flag = UP_MARK;
					}
					else
					{
						int flag = GetEntryExitDirection(single,first,(ushort)ENTRY_POSITION,locCurrentSectionNo,WORK_SCHEDULE,direct_flag);//yuzhu 20110529 处理西安局包西一台张桥打印问题
						if(flag != -1)
							direct_flag = flag;
					}

					ptlStart.y = (direct_flag == UP_MARK ?  first_top_entry : first_bottom_entry);
					ptlStart.Offset (0,y_point_offset);
					if(first_a_time_status)
					{
						CString tempTrain(single->GetTrainRecord(0)->m_data.original_arrive_train_id);
						if(tempTrain.IsEmpty())
						{
							tempTrain = single->GetTrainID();
							DrawMarkOfArrive(pCDC,single,tempTrain,ptlStart,10,direct_flag,lActualColor,true,schd_type,0);
						}
						else
						{
							tempTrain.Empty();
							tempTrain = single->GetTrainID();
							tempTrain += "(";
							tempTrain += single->GetTrainRecord(0)->m_data.original_arrive_train_id;
							tempTrain += ")";
							DrawMarkOfArrive(pCDC,single,tempTrain,ptlStart,10,direct_flag,lActualColor,true,schd_type,0);
						}
					}
					else
					{
						CString tempTrain(single->GetTrainRecord(0)->m_data.original_arrive_train_id);
						if(tempTrain.IsEmpty())
						{
							tempTrain = single->GetTrainID();
							DrawMarkOfArrive(pCDC,single,tempTrain,ptlStart,10,direct_flag,lColor,false,schd_type,0);
						}
						else
						{
							tempTrain.Empty();
							tempTrain = single->GetTrainID();
							tempTrain += "(";
							tempTrain += single->GetTrainRecord(0)->m_data.original_arrive_train_id;
							tempTrain += ")";
							DrawMarkOfArrive(pCDC,single,tempTrain,ptlStart,10,direct_flag,lColor,false,schd_type,0);
						}
					}
				}
			}

			//one train record
			if(usDrawCount == 1)
			{
				int direct_flag=-1;
				if(IsFlagStopAbs(first->m_data.flag))
				{
					if(IsFlagStopAbsUp(first->m_data.flag))
						direct_flag = DOWN_MARK;
					else
						direct_flag = UP_MARK; 
				}
				else
				{
					if(nDirectFlag == 1)
						direct_flag = UP_MARK;
					else
						direct_flag = DOWN_MARK;

				}

				long time_depart,date_depart,time_arrival,date_arrival;
				time_arrival = minute_time(first->m_data.arrive,&date_arrival);
				if(first->m_data.depart == NOTIME || first->m_data.depart == 0)
				{
					date_depart = date_arrival;
					time_depart = time_arrival;
				}
				else
				{
					time_depart = minute_time(first->m_data.depart,&date_depart);
				}
				flFirstMinuteOffset = GetMinuteWidthToPlotStartTime(time_depart/60,date_depart,isPrint,m_nSchdStatus);
				if(IsFlagAbsTerminal(first->m_data.flag))
				{
					WORD nDirect = UP_DIRECT;
					if(IsFlagStopAbsUp(first->m_data.flag))
					{
						direct_flag = UP_MARK;
						nDirect = UP_DIRECT;
					}
					else
					{
						direct_flag = DOWN_MARK;
						nDirect = DOWN_DIRECT;
					}

					flFirstMinuteOffset = GetMinuteWidthToPlotStartTime(time_depart/60,date_depart,isPrint,m_nSchdStatus);
					ptlStart.x = flFirstMinuteOffset * m_fPixelPerMinute;
					ptlStart.y = GetStationVPos(single,first,PRIMARY_TRACK,EXIT_POSITION,m_nSchdStatus);
					ptlEnd.x = (flFirstMinuteOffset + first->m_data.special_time_1)  * m_fPixelPerMinute;
					ptlEnd.y = GetAbsVPos(single,first,0, EXIT_POSITION,IsFlagStopAbsUp(first->m_data.flag)?UP_DIRECT:DOWN_DIRECT,m_nSchdStatus); 
					
					int dlt = (ptlEnd.y - ptlStart.y)*2/3;
					if(IsAbsDrawUp(first->m_data.stop_reason))
					{
						ptlEnd.y = ptlStart.y + dlt;
					}
					else if(IsAbsDrawBottom(first->m_data.stop_reason))
					{
						ptlEnd.y = ptlStart.y + dlt*2;
					}

					ptlStart.Offset (x_point_offset,y_point_offset);
					ptlEnd.Offset (x_point_offset,y_point_offset);

					if(ptlStart.x<m_nPageWidth && ptlStart.y > 0)
					{
						if(first_d_time_status)
						{
							DrawMarkOfEnd(pCDC,single,single->GetTrainID(),ptlEnd,20,direct_flag,lActualColor,first->m_data.flag,true);
							DrawActualLine(pCDC, ptlStart, ptlEnd, single->GetTrainInfo()->m_data.type,FALSE,m_nCountOfEngine);
						}
						else
						{
							DrawMarkOfEnd(pCDC,single,single->GetTrainID(),ptlEnd,20,direct_flag,lColor,first->m_data.flag,false);
							DrawActualLine(pCDC, ptlStart, ptlEnd, single->GetTrainInfo()->m_data.type,FALSE,m_nCountOfEngine, FALSE);

						}
					}
				}
				else if(IsFlagTerminal(first->m_data.flag))  //区间终到
				{
					int flag = GetEntryExitDirection(single,first,(ushort)ENTRY_POSITION,locCurrentSectionNo);
					if(flag != -1)
					{
						if(flag == UP_MARK)
							direct_flag = DOWN_MARK;
						else
							direct_flag = UP_MARK;
					}
					if( IsFlagAbsTerminal(first->m_data.flag))
					{	
						flag = GetEntryExitDirection(single,first,(ushort)EXIT_POSITION,locCurrentSectionNo,schd_type);
						direct_flag = flag;
					}

					long time_arrival,date_arrival;
					time_arrival = minute_time(first->m_data.arrive,&date_arrival);
					flFirstMinuteOffset = GetMinuteWidthToPlotStartTime(time_arrival/60,date_arrival,isPrint,m_nSchdStatus); 

					ptlStart.x =  (int)(flFirstMinuteOffset * m_fPixelPerMinute); 			
					ptlStart.y =  first_primary_entry;
					//stop abs mark
					if(IsFlagStopAbs(first->m_data.flag))
					{
						long time_depart,date_depart,time_arrival,date_arrival;
						time_arrival = minute_time(first->m_data.arrive,&date_arrival);
						if(first->m_data.depart == NOTIME || first->m_data.depart == 0)
						{
							date_depart = date_arrival;
							time_depart = time_arrival;
						}
						else
						{
							time_depart = minute_time(first->m_data.depart,&date_depart);
						}
						flFirstMinuteOffset = GetMinuteWidthToPlotStartTime(time_depart/60,date_depart,isPrint,schd_type);			
						ptlStart.x =(int)(flFirstMinuteOffset + first->m_data.special_time_1)  * m_fPixelPerMinute;
						ptlStart.y = GetAbsVPos(single,first,0, EXIT_POSITION,IsFlagStopAbsUp(first->m_data.flag)?UP_DIRECT:DOWN_DIRECT,schd_type);//will
					}
					if(IsFlagAbsStart(first->m_data.flag))
					{
						ptlStart.x =  (int)(flFirstMinuteOffset * m_fPixelPerMinute);
						ptlStart.y = (direct_flag == UP_MARK ?  first_top_exit : first_bottom_exit);
					}
					ptlStart.Offset (x_point_offset,y_point_offset);

					if(ptlStart.x >= x_point_offset && ptlStart.x<= rangeRect.right + x_point_offset && ptlStart.y >=y_point_offset)
					{
						if(first_a_time_status)
							DrawMarkOfEnd(pCDC,single,single->GetTrainID(),ptlStart,20,direct_flag,lActualColor,first->m_data.flag,true);
						else
							DrawMarkOfEnd(pCDC,single,single->GetTrainID(),ptlStart,20,direct_flag,lColor,first->m_data.flag,false);   						 
					}
				}
				else 
				{ 
					if(ptlStart.x >= x_point_offset && ptlStart.x<= rangeRect.right + x_point_offset&& ptlStart.y >0)
					{
						int flag = GetEntryExitDirection(single,first,(ushort)EXIT_POSITION,locCurrentSectionNo);
						if(flag != -1)
							direct_flag = flag;

						flFirstMinuteOffset = GetMinuteWidthToPlotStartTime(time_depart/60,date_depart,isPrint,m_nSchdStatus);
						ptlStart.x = flFirstMinuteOffset * m_fPixelPerMinute;
						ptlStart.y = first_primary_entry;
						ptlStart.Offset (x_point_offset,y_point_offset);
						if(first_a_time_status)
							DrawMarkOfDepart(pCDC,single,single->GetTrainID(),ptlStart,20,direct_flag,lActualColor,true);
						else
							DrawMarkOfDepart(pCDC,single,single->GetTrainID(),ptlStart,20,direct_flag,lColor,false);
					}
				}
			}
		}

		if(i == usDrawCount - 2)
		{
			int direct_flag=-1; 
			if(IsFlagStopAbs(first->m_data.flag) && first->m_data.station == second->m_data.station)
			{
				if(IsFlagStopAbsUp(first->m_data.flag))
					direct_flag = DOWN_MARK;
				else
					direct_flag = UP_MARK; 
			}
			else
			{
				if(nDirectFlag == 1)
					direct_flag = UP_MARK;
				else
					direct_flag = DOWN_MARK;
			}

			long time_depart,date_depart,time_arrival,date_arrival;
			time_arrival = minute_time(second->m_data.arrive,&date_arrival);
			if(second->m_data.depart == NOTIME || second->m_data.depart == 0)
			{
				date_depart = date_arrival;
				time_depart = time_arrival;
			}
			else
			{
				time_depart = minute_time(second->m_data.depart,&date_depart);
			}
			flFirstMinuteOffset = GetMinuteWidthToPlotStartTime(time_depart/60,date_depart,isPrint,m_nSchdStatus);
			ptlStart.x =  (int)(flFirstMinuteOffset * m_fPixelPerMinute);	 
			ptlStart.y = (direct_flag == UP_MARK ?  second_top_entry : second_bottom_entry);

			ptlStart.Offset (x_point_offset,y_point_offset);

			if(IsFlagAbsTerminal(second->m_data.flag))
			{
				WORD nDirect = UP_DIRECT;
				if(IsFlagStopAbsUp(second->m_data.flag))
				{
					direct_flag = UP_MARK;
					nDirect = UP_DIRECT;
				}
				else
				{
					direct_flag = DOWN_MARK;
					nDirect = DOWN_DIRECT;
				}

				flFirstMinuteOffset = GetMinuteWidthToPlotStartTime(time_depart/60,date_depart,isPrint,m_nSchdStatus);
				ptlStart.x = (flFirstMinuteOffset + second->m_data.special_time_1)  * m_fPixelPerMinute;
				ptlStart.y = GetAbsVPos(single, second, 0, EXIT_POSITION, nDirect, m_nSchdStatus); 
				int tmpY = (direct_flag == UP_MARK ?  second_top_entry : second_bottom_entry);
				int dlt = (ptlStart.y - tmpY)*2/3;
				if(IsAbsDrawUp(second->m_data.stop_reason))
				{
					ptlStart.y = tmpY + dlt;
				}
				else if(IsAbsDrawBottom(second->m_data.stop_reason))
				{
					ptlStart.y = tmpY + dlt*2;
				}

				ptlStart.Offset (x_point_offset,y_point_offset);

				if(ptlStart.x<m_nPageWidth && ptlStart.y > 0 && !IsFlagTrainWholeHelp(second->m_data.flag))
				{
					if(second_d_time_status)
						DrawMarkOfEnd(pCDC,single,single->GetTrainID(),ptlStart,20,direct_flag,lActualColor,second->m_data.flag,true);
					else
						DrawMarkOfEnd(pCDC,single,single->GetTrainID(),ptlStart,20,direct_flag,lColor,second->m_data.flag,false);
				}
			}
			else if(IsFlagTerminal(second->m_data.flag))
			{
				//终到标志不按照出入口号来判断，直接根据车在运行图方向来画，判断区间停车是为了避免折返的结束标志不对
				if(first!= NULL && second != NULL && !IsFlagStopAbs(first->m_data.flag))
				{
					if(first_primary_exit> second_primary_entry)
					{
						direct_flag = UP_MARK; 
					}
					else
					{
						direct_flag = DOWN_MARK;
					}
				}
				else
				{
					int flag = GetEntryExitDirection(single,second,(ushort)ENTRY_POSITION,locCurrentSectionNo);
					if(flag != -1){
						if(flag == UP_MARK)
							direct_flag = DOWN_MARK;
						else
							direct_flag = UP_MARK;
					}

				}

				long time_arrival,date_arrival;
				time_arrival = minute_time(second->m_data.arrive,&date_arrival);
				flFirstMinuteOffset = GetMinuteWidthToPlotStartTime(time_arrival/60,date_arrival,isPrint,m_nSchdStatus);

				ptlStart.x =  flFirstMinuteOffset * m_fPixelPerMinute; 			
				//ptlStart.y =  (direct_flag == UP_MARK ?  second_top_entry : second_bottom_entry);
				ptlStart.y =  (direct_flag == UP_MARK ?  second_primary_entry : second_primary_exit);
				

				int nDirect = UP_DIRECT;
				if(IsFlagStopAbs(second->m_data.flag))
				{
					if(IsFlagStopAbsUp(second->m_data.flag))
						nDirect = UP_DIRECT;
					else
						nDirect = DOWN_DIRECT;

					ptlStart.x =  (flFirstMinuteOffset+second->m_data.special_time_1) * m_fPixelPerMinute;
					ptlStart.y =  GetAbsVPos(single,second,0, EXIT_POSITION,nDirect,m_nSchdStatus);
				}

				if(IsFlagRescued(second->m_data.flag))
				{
					CTG_SingleTrain* pTrain = NULL;
					for(int idx=0; idx<m_vecSchdTrains.size(); idx++)
					{
						if(m_vecSchdTrains[idx].GetTrainIndex() == second->m_data.special_train_index)
						{
							pTrain = &m_vecSchdTrains[idx];
							break;
						}
					}
					do
					{
						if(NULL == pTrain)
							break;

						CTG_TrainRecord* otherRec = NULL;
						for(int i=0; i<pTrain->GetTrainRecordCount(); i++)
						{
							CTG_TrainRecord* rec = pTrain->GetTrainRecord(i);
							if(NULL == rec)
								continue;
							if(IsFlagRescue(rec->m_data.flag))
							{
								otherRec = rec;
								break;
							}
						}

						if(NULL == otherRec || second == NULL)
							return;

						long time_arrival,date_arrival;
						time_arrival = minute_time(otherRec->m_data.arrive,&date_arrival);
						int tmpFirstMinuteOffset = GetMinuteWidthToPlotStartTime(time_arrival/60,date_arrival,isPrint,m_nSchdStatus);

						CPoint ptBegin, ptEnd;
						ptBegin = ptlStart;
						ptEnd.x = (tmpFirstMinuteOffset + otherRec->m_data.special_time_1) * m_fPixelPerMinute;
						ptEnd.y = ptBegin.y;

						ptBegin.Offset (x_point_offset,y_point_offset);
						ptEnd.Offset (x_point_offset,y_point_offset);

						//for actual time or plan time codeing   
						int first_d_time_status = 0;
						if(otherRec->m_data.adjust_status & 0x0002)
							first_d_time_status = 1;//real time
						else
							first_d_time_status = 0;// plan time 

						CPen mPen,*pOldPen;
						if(1 == first_d_time_status)
							mPen.CreatePen(PS_DOT, 1, lActualColor);
						else
							mPen.CreatePen(PS_DOT, 1, lColor);

						pOldPen = pCDC->SelectObject(&mPen);
						pCDC->MoveTo(ptBegin);  pCDC->LineTo(ptEnd);
						pCDC->SelectObject(pOldPen);

					}while(0);
				}

				ptlStart.Offset (x_point_offset,y_point_offset);
				if(ptlStart.x >= x_point_offset && ptlStart.x<= rangeRect.right + x_point_offset)
				{	
					bool bDrawTerminalFlag=true;
					if(m_bShowCzhConnectLineOnPlot)
					{
						if(single->m_nNextTrainIndex>0 && (false==IsCRHStopStation(second->m_data.station)))
						{
							bDrawTerminalFlag=false;

							CTG_SingleTrain*ptrain=GetTrain(single->m_nNextTrainIndex);
							if(ptrain==NULL)
							{
								if(second_a_time_status)
									DrawOutScopeConnectLine(pCDC, single, ptlStart, lActualColor, RIGHT_MARK, rangeRect);
								else
									DrawOutScopeConnectLine(pCDC, single, ptlStart, lColor, RIGHT_MARK, rangeRect);
							}
						}
					}
					if(bDrawTerminalFlag)
					{
						if(second_d_time_status)
							DrawMarkOfEnd(pCDC,single,single->GetTrainID(),ptlStart,20,direct_flag,lActualColor,second->m_data.flag,true);
						else
							DrawMarkOfEnd(pCDC,single,single->GetTrainID(),ptlStart,20,direct_flag,lColor,second->m_data.flag,false);
					}
				}
			}
			else 
			{ 
				if(ptlStart.x >= x_point_offset && ptlStart.x<= rangeRect.right + x_point_offset && ptlStart.y >= y_point_offset)
				{
					// 太原接入交出问题
					if(first!=NULL)
					{
						int out_index=first->GetDepartTGIndex();
						int in_index=second->GetArriveTGIndex();
						if (out_index==in_index) 
						{
							if (nDirectFlag==1)//上行车
								direct_flag = DOWN_MARK;
							else//下行车
								direct_flag = UP_MARK;
						}
						else if(out_index>in_index)
							direct_flag = UP_MARK;
						else
							direct_flag = DOWN_MARK;
					}
					else
					{
						int flag = GetEntryExitDirection(single,second,(ushort)EXIT_POSITION,locCurrentSectionNo,WORK_SCHEDULE,direct_flag);//yuzhu 20110529 处理西安局包西一台张桥打印问题
						if(flag != -1)
							direct_flag = flag;
					}

					if(second_d_time_status)
						DrawMarkOfDepart(pCDC,single,single->GetTrainID(),ptlStart,20,direct_flag,lActualColor,true);
					else
						DrawMarkOfDepart(pCDC,single,single->GetTrainID(),ptlStart,20,direct_flag,lColor,false);
				}
			}
		}

		if(usStartTime <= usEndTime || usStartDate <= usEndDate)  //////////////
		{	
			//有关断点跳跃的绘制： 交出符号    
			do
			{
				if(m_nTrainLeapLineFlag == 3)
				{
					break;
				}
				if(usDrawCount == 1) break;

				long time_depart,date_depart,time_arrival,date_arrival;
				time_arrival = minute_time(first->m_data.arrive,&date_arrival);
				if(first->m_data.depart == NOTIME || first->m_data.depart == 0)
				{
					date_depart = date_arrival;
					time_depart = time_arrival;
				}
				else
				{
					time_depart = minute_time(first->m_data.depart,&date_depart);
				}
				flSecondMinuteOffset = GetMinuteWidthToPlotStartTime(time_depart/60,date_depart,isPrint,m_nSchdStatus);

				ptlStart.x =  (int)(flFirstMinuteOffset * m_fPixelPerMinute);
				ptlEnd.x = (int)(flSecondMinuteOffset * m_fPixelPerMinute);

				if(IsFlagStart(first->m_data.flag))
					ptlStart.x = ptlEnd.x;
				if(second == NULL && IsFlagTerminal(first->m_data.flag))
					ptlEnd.x =	ptlStart.x ;

				ptlStart.y = ptlEnd.y =  first_primary_entry;
				if(ptlStart.y == -1) continue;

				int index1 = first->GetArriveTGIndex();
				int index2 = first->GetDepartTGIndex();
				if(second != NULL)
				{
					if((index1 != index2) && (0<=index2 && index2<128) && !(IsFlagTerminal(first->m_data.flag)||IsFlagStart(first->m_data.flag)))
					{ 
						CPoint pt_a,pt_d,pt_c; //接入，交出点
						pt_a.x = ptlEnd.x;
						pt_d.x = ptlEnd.x;
						pt_c.x = ptlStart.x;

						//绘制开车时间用
						int direct_flag = nDirectFlag; 

						//对于交出点，上行车，去顶点，下行车，去底部点	
						if(m_nTrainLeapLineFlag==2||m_nTrainLeapLineFlag==1)
						{
							if(nDirectFlag_pre == UP_DIRECT)
								pt_d.y =  GetStationVPos(single,first,TOP_TRACK,ENTRY_POSITION,schd_type);
							else
								pt_d.y =  GetStationVPos(single,first,BOTTOM_TRACK,ENTRY_POSITION,schd_type);
						}
						else
						{
							pt_d.y =  first_primary_entry;
						}
						pt_c.y=pt_d.y;

						//交出符号
						if(nDirectFlag_pre == UP_DIRECT)
							direct_flag = UP_MARK;
						else
							direct_flag = DOWN_MARK;


						pt_d.Offset (x_point_offset,y_point_offset);  

						//绘制交出标志
						if(m_nTrainLeapLineFlag == 2)
						{
							if(pt_d.x >= x_point_offset && pt_d.x<rangeRect.right+ x_point_offset && pt_d.y > 0)
							{
								if(second_d_time_status)
									DrawMarkOfDepart(pCDC,single,single->GetTrainID(),pt_d,10,
									direct_flag,lActualColor,true);
								else
									DrawMarkOfDepart(pCDC,single,single->GetTrainID(),pt_d,10,
									direct_flag,lColor,false);					

							}
						}
						
						if(nDirectFlag_next == DOWN_DIRECT)
							direct_flag = UP_MARK;
						else
							direct_flag = DOWN_MARK;

						//对于接入点，上行车，去顶点，下行车，去底部点
						//pt_a.x=ptlStart.x;
						if(m_sDispDistance[index2].distance.node_info.show_sides)
						{
							long time_arrival,date_arrival;
							time_arrival = minute_time(first->m_data.arrive,&date_arrival);
							long Offset = GetMinuteWidthToPlotStartTime(time_arrival/60,date_arrival,isPrint,m_nSchdStatus);
							pt_a.x =  (int)(Offset * m_fPixelPerMinute);
							if(nDirectFlag_next == UP_DIRECT)
								pt_a.y =  first_bottom_exit;
							else
								pt_a.y =  first_top_exit;
						}
						else
						{
							if(nDirectFlag_pre == UP_DIRECT)
								pt_a.y =  first_top_exit;
							else
								pt_a.y =  first_bottom_exit;
						}
						CPoint pt_a1=pt_a;
						pt_a.Offset (x_point_offset,y_point_offset);  
						//绘制接入标志
						if(m_nTrainLeapLineFlag == 2)
						{
							long time_arrival,date_arrival;
							time_arrival = minute_time(first->m_data.arrive,&date_arrival);
							long Offset = GetMinuteWidthToPlotStartTime(time_arrival/60,date_arrival,isPrint,m_nSchdStatus);
							pt_a1.x =  (int)(Offset * m_fPixelPerMinute);
							pt_a1.Offset (x_point_offset,y_point_offset); 
							
							if(pt_a1.x >= x_point_offset && pt_a1.x<rangeRect.right + x_point_offset&& pt_a1.y > 0)
							{
								CString strTrainId=first->GetDepartTID();
								if(strTrainId.IsEmpty())
									strTrainId=first->GetArriveTID();
								if(first_a_time_status)
									DrawMarkOfArrive(pCDC,single,strTrainId,pt_a1,10,
									direct_flag,lActualColor,true,schd_type, first->rec_index);
								else
									DrawMarkOfArrive(pCDC,single,strTrainId,pt_a1,10,
									direct_flag,lColor,false,schd_type, first->rec_index);
							}
						}
						
						//绘制发车时间时分
						//下行车到达点 TA_RIGHT|TA_TOP
						//上行车到达点 TA_RIGHT|TA_BOTTOM	
						if(nDirectFlag_next == DOWN_DIRECT)
							pt_d.y = first_bottom_exit+y_point_offset;
						else
							pt_d.y = first_top_exit+y_point_offset;
						if((pt_d.x >= x_point_offset && pt_d.x<rangeRect.right+ x_point_offset && pt_d.y > 0))
						{						
							CFont* pOldFont, font;
							LOGFONT stFont;
							memset(&stFont,0,sizeof(stFont));
							stFont.lfHeight = MulDiv(m_font.time_char,m_nLogPixelSy,72); // time_char
							stFont.lfWeight = FW_NORMAL;
							stFont.lfClipPrecision = CLIP_LH_ANGLES;
							strcpy(stFont.lfFaceName,"Times New Roman");
							font.CreateFontIndirect(&stFont);

							pOldFont = pCDC->SelectObject(&font);						

							COLORREF Color = GetActualLineColor(factor);

							UINT nOldColor;
							if(manualRealColor>0 && TG_IsDepartActualByManual(first->m_data))
							{
								nOldColor = pCDC->SetTextColor(manualRealColor);
							}
							else
							{
								if(first_d_time_status == 1)
									nOldColor = pCDC->SetTextColor(Color);
							}
							CPoint point = ptlStart;
							if(nDirectFlag_next == DOWN_DIRECT)//下行车 
							{   
								pCDC->SetTextAlign(TA_RIGHT|TA_TOP);
							}
							else
							{//up direct
								pCDC->SetTextAlign(TA_RIGHT|TA_BOTTOM);
							}

							long time_depart,date_depart,time_arrival,date_arrival;
							time_arrival = minute_time(first->m_data.arrive,&date_arrival);
							if(first->m_data.depart == NOTIME || first->m_data.depart == 0)
							{
								date_depart = date_arrival;
								time_depart = time_arrival;
							}
							else
							{
								time_depart = minute_time(first->m_data.depart,&date_depart);
							}
							itoa(time_depart/60 % 10, time_char, 10);

							pCDC->TextOut(pt_d.x,pt_d.y,time_char,1);

							if(manualRealColor>0 && TG_IsDepartActualByManual(first->m_data))
							{
								pCDC->SetTextColor(nOldColor);
							}
							else
							{
								if(first_d_time_status == 1)
									pCDC->SetTextColor(nOldColor);
							}
							
							pCDC->SelectObject(pOldFont);   

							font.DeleteObject();
						}

						long time_arrival,date_arrival;
						time_arrival = minute_time(first->m_data.arrive,&date_arrival);
						long Offset = GetMinuteWidthToPlotStartTime(time_arrival/60,date_arrival,isPrint,m_nSchdStatus);
						pt_a.x =  (int)(Offset * m_fPixelPerMinute)+x_point_offset;
						if((pt_a.x >= x_point_offset && pt_a.x<rangeRect.right+ x_point_offset && pt_a.y > 0))
						{						
							CFont* pOldFont, font;
							LOGFONT stFont;
							memset(&stFont,0,sizeof(stFont));
							stFont.lfHeight = MulDiv(m_font.time_char,m_nLogPixelSy,72); // time_char
							stFont.lfWeight = FW_NORMAL;
							stFont.lfClipPrecision = CLIP_LH_ANGLES;
							strcpy(stFont.lfFaceName,"Times New Roman");
							font.CreateFontIndirect(&stFont);

							pOldFont = pCDC->SelectObject(&font);						

							COLORREF Color = GetActualLineColor(factor);

							UINT nOldColor;
							if(manualRealColor>0 && TG_IsArriveActualByManual(first->m_data))
							{
								nOldColor = pCDC->SetTextColor(manualRealColor);
							}
							else
							{
								if(first_a_time_status == 1)
									nOldColor = pCDC->SetTextColor(Color);
							}
							CPoint point = ptlStart;
							if(nDirectFlag_next == DOWN_DIRECT)//下行车 
							{   
								pCDC->SetTextAlign(TA_RIGHT|TA_BOTTOM);
							}
							else
							{//up direct
								pCDC->SetTextAlign(TA_RIGHT|TA_TOP);
							}

							long time_depart,date_depart,time_arrival,date_arrival;
							time_depart = minute_time(first->m_data.depart,&date_depart);
							if(first->m_data.arrive == NOTIME || first->m_data.arrive == 0)
							{
								date_arrival = date_depart;
								time_arrival = time_depart;
							}
							else
							{
								time_arrival = minute_time(first->m_data.arrive,&date_arrival);
							}
							itoa(time_arrival/60 % 10, time_char, 10);				

							pCDC->TextOut(pt_a.x,pt_a.y,time_char,1);

							if(manualRealColor>0 && TG_IsArriveActualByManual(first->m_data))
							{
								pCDC->SetTextColor(nOldColor);
							}
							else
							{
								if(first_a_time_status == 1)
									pCDC->SetTextColor(nOldColor);
							}
							
							pCDC->SelectObject(pOldFont);   

							font.DeleteObject();
						}			
					}
				}
			}while(0);

			long time_depart,date_depart,time_arrival,date_arrival;
			time_arrival = minute_time(first->m_data.arrive,&date_arrival);
			if(first->m_data.depart == NOTIME || first->m_data.depart == 0)
			{
				date_depart = date_arrival;
				time_depart = time_arrival;
			}
			else
			{
				time_depart = minute_time(first->m_data.depart,&date_depart);
			}
			flFirstMinuteOffset = GetMinuteWidthToPlotStartTime(time_arrival/60,date_arrival,isPrint,m_nSchdStatus);
			flSecondMinuteOffset = GetMinuteWidthToPlotStartTime(time_depart/60,date_depart,isPrint,m_nSchdStatus);

			CPoint tmpArl(99999,99999);
			// DOWN_DIRECT
			if(nDirectFlag == 2)
			{
				ptlStart.x =  (int)(flFirstMinuteOffset * m_fPixelPerMinute);
				ptlEnd.x = (int)(flSecondMinuteOffset * m_fPixelPerMinute);
				ushort usEntryFlag = ENTRY_POSITION;
				if(IsFlagStart(first->m_data.flag))
				{
					ptlStart.x = ptlEnd.x;
					usEntryFlag = EXIT_POSITION;
				}
				if(second == NULL && IsFlagTerminal(first->m_data.flag))
					ptlEnd.x =	ptlStart.x ;

				ptlStart.y = ptlEnd.y =  GetStationVPos(single,first,PRIMARY_TRACK,usEntryFlag,schd_type);
				if(ptlStart.y == -1) continue;

				//draw side point new added ////////////////////
				if (m_nDrawSidePoint)
				{
					if(GetStationVPos(single,first,PRIMARY_TRACK,usEntryFlag,schd_type) != 
						GetStationVPos(single,first,TOP_TRACK,usEntryFlag,schd_type))
					{					
						int i=-1;
						if ((ptlStart.x == ptlEnd.x)&&(GetNodeIndex(single,first,i,(ushort)usEntryFlag)>-1)){
							if(m_sDispDistance[i].distance.node_info.show_sides
								&&m_sDispDistance[i].distance.node_info.show_sides_mode ==0)
							{
								CPoint pt = ptlStart;
								CSize ssSize = CSize(8,8);
								int scaleRate = (fHeightMulti>5?5:fHeightMulti);

								int uuDis=4*scaleRate;
								ssSize.cx *= scaleRate;
								ssSize.cy *= scaleRate;

								CBrush tempBrush(lColor);
								CBrush *pOldBrush;
								pOldBrush = pCDC->SelectObject(&tempBrush);
								if(ptlStart.x > 0 && ptlStart.x < rangeRect.right)
								{  pt.Offset (x_point_offset,y_point_offset);
								pCDC->RoundRect(CRect(CPoint(pt.x - uuDis,pt.y - uuDis),ssSize),pt);
								}
								pCDC->SelectObject(pOldBrush);
							}
						}
					}
				}

				do
				{
					if((ptlStart.x < 0 && ptlEnd.x < 0))
					{
						break;
					}
					else if ((ptlStart.x < 0 && ptlEnd.x >= 0 )) 
					{
						ptlStart.x = 0;
						critical_point = ptlStart;
						critical_point.Offset (x_point_offset,y_point_offset);
						//判断是否在该车站改了车次号，如果改了，使用新车次号
						NewTrainId = "";
						for(critial_record_index =0; critial_record_index<single->GetTrainRecordCount(); critial_record_index++)//查找在本站是否该了车次号
						{
							if(i == single->GetTrainRecord(critial_record_index)->m_data.station && single->GetTrainRecord(critial_record_index)->m_data.arrive_train_id != single->GetTrainRecord(critial_record_index)->m_data.depart_train_id
								&& strlen(single->GetTrainRecord(critial_record_index)->m_data.arrive_train_id)>0 && strlen(single->GetTrainRecord(critial_record_index)->m_data.depart_train_id)>0)
							{
								NewTrainId.Format("%s",single->GetTrainRecord(critial_record_index)->m_data.depart_train_id);
								break;
							}
						}

						//判断新车次号结束							
						if(NewTrainId.IsEmpty())
							NewTrainId = single->GetTrainID();

						if(first_a_time_status)
							DrawMarkOfCritical(pCDC,single,NewTrainId,critical_point,50,LEFT_MARK, lActualColor);
						else							
							DrawMarkOfCritical(pCDC,single,NewTrainId,critical_point,50,LEFT_MARK, lColor);
					}

					if((ptlStart.x > rangeRect.right) && ptlEnd.x >= rangeRect.right)
					{
						break;
					}
					else if((ptlStart.x < rangeRect.right) && ptlEnd.x >= rangeRect.right)
					{
						ptlEnd.x = rangeRect.right;
						critical_point = ptlEnd;
						critical_point.Offset (x_point_offset,y_point_offset);
						//判断是否在该车站改了车次号，如果改了，使用新车次号
						NewTrainId = "";
						//	for(critial_record_index =1; critial_record_index<single->GetTrainInfo()->train_id_count; critial_record_index++)//查找在本站是否该了车次号
						for(critial_record_index =0; critial_record_index<single->GetTrainRecordCount(); critial_record_index++)//查找在本站是否该了车次号
						{
							if(i == single->GetTrainRecord(critial_record_index)->m_data.station && single->GetTrainRecord(critial_record_index)->m_data.arrive_train_id != single->GetTrainRecord(critial_record_index)->m_data.depart_train_id
								&& strlen(single->GetTrainRecord(critial_record_index)->m_data.arrive_train_id)>0 && strlen(single->GetTrainRecord(critial_record_index)->m_data.depart_train_id)>0)
								//if(i >= single->GetTrainInfo()->begin_sta[critial_record_index] && i<= single->GetTrainInfo()->end_sta[critial_record_index])//找到了
							{
								NewTrainId.Format("%s",single->GetTrainRecord(critial_record_index)->m_data.depart_train_id);
								break;
							}
						}
						//判断新车次号结束							
						if(NewTrainId.IsEmpty())
							NewTrainId = single->GetTrainID();

						if(first_a_time_status)
							DrawMarkOfCritical(pCDC,single,NewTrainId,critical_point,50,RIGHT_MARK, lActualColor);
						else							
							DrawMarkOfCritical(pCDC,single,NewTrainId,critical_point,50,RIGHT_MARK, lColor);

					}

					if((m_nSchdStatus == WORK_SCHEDULE || m_nSchdStatus == SHIFT_SCHEDULE ||m_nSchdStatus == HISTORY_SCHEDULE) == TRUE? TRUE:(ptlEnd.x >= ptlStart.x))
					{     
						//	if(ptlEnd.x >= ptlStart.x){
						if(ptlEnd.x <= rangeRect.right)
						{
							int index1 = first->GetArriveTGIndex();
							int index2 = first->GetDepartTGIndex();
							bool bDrawStopLine=true;
							if(m_nTrainLeapLineFlag==3 && index1!=index2 &&  index1>=0 && index1<128 && index2>=0 && index2<128)
							{
								if(m_sDispDistance[index1].distance.node_info.show_sides==false && m_sDispDistance[index2].distance.node_info.show_sides)
									bDrawStopLine=false;
							}
							if(bDrawStopLine)
							{
								if(first_a_time_status && first_d_time_status)
								{

									if(m_bStopStationWaveLine && JudgeNeedWaveLine(first,m_nSchdStatus,m_nDirectFlag_prefix))
										DrawActualStopStationLine(pCDC,CPoint(ptlStart.x + x_point_offset,ptlStart.y + y_point_offset),
										CPoint(ptlEnd.x+x_point_offset,ptlEnd.y+ y_point_offset),single->GetTrainInfo()->m_data.type,DOWN_DIRECT);//draw real time line
									else
										DrawActualLine(pCDC,CPoint(ptlStart.x + x_point_offset,ptlStart.y + y_point_offset),
										CPoint(ptlEnd.x+x_point_offset,ptlEnd.y+ y_point_offset),single->GetTrainInfo()->m_data.type);//single->m_carriage_count);//draw real time line

									int yStation;
									if(i == 0 && !IsFlagStart(first->m_data.flag))
									{
										yStation=first_top_entry;
										////wangjinhai added 
										int flag = GetEntryExitDirection(single,first,(ushort)ENTRY_POSITION,locCurrentSectionNo,schd_type);
										int direct_flag=-1;  // BUG!!! UMR
										if(flag != -1)
											direct_flag = flag;						   
										yStation = (direct_flag == UP_MARK ? first_top_entry : first_bottom_entry);
										////

										DrawActualLine(pCDC,CPoint(ptlStart.x + x_point_offset,yStation + y_point_offset),
											CPoint(ptlStart.x+x_point_offset,ptlStart.y+ y_point_offset),single->GetTrainInfo()->m_data.type,FALSE,m_nCountOfEngine);//single->m_carriage_count);
									}

								}
								else
								{
									//车站有停车线
									int yStation;  
									if(i == 0 && !IsFlagStart(first->m_data.flag) )
									{  ////
										yStation=first_top_entry;
										////wangjinhai added 
										int flag = GetEntryExitDirection(single,first,(ushort)ENTRY_POSITION,locCurrentSectionNo,schd_type);
										int direct_flag=-1;  // BUG!!! UMR
										if(flag != -1)
											direct_flag = flag;						   
										yStation = (direct_flag == UP_MARK ?  first_top_entry : first_bottom_entry);
										////
										
										DrawActualLine(pCDC,CPoint(ptlStart.x + x_point_offset,yStation + y_point_offset),
											CPoint(ptlStart.x+x_point_offset,ptlStart.y+ y_point_offset),single->GetTrainInfo()->m_data.type,FALSE,m_nCountOfEngine,0,FALSE);

										if(yStation != ptlStart.y)
										{
											m_pPerPointInfo[m_nPointCount].record_index = i;
											m_pPerPointInfo[m_nPointCount].start_depart_flag = ARRIVAL_POSITION;
											m_pPerPointInfo[m_nPointCount++].rect  = CRect(CPoint(ptlStart.x - uDis,yStation - uDis),sSize); ////ptlStart.y
										}
									}

									if(!(second == NULL && IsFlagTerminal(first->m_data.flag)) && !IsFlagUnitOtherTrain(first->m_data.flag) && !IsFlagEnd(first->m_data.flag))
									{
										if(m_bStopStationWaveLine && JudgeNeedWaveLine(first,schd_type,m_nDirectFlag_prefix))
											DrawStopStationLine(pCDC,CPoint(ptlStart.x+x_point_offset, ptlStart.y+y_point_offset),
											CPoint(ptlEnd.x + x_point_offset,ptlEnd.y+y_point_offset),DOWN_DIRECT);
										else
										{	
											DrawActualLine(pCDC,CPoint(ptlStart.x + x_point_offset,ptlStart.y + y_point_offset),
												CPoint(ptlEnd.x+x_point_offset,ptlEnd.y+ y_point_offset),single->GetTrainInfo()->m_data.type,0,0,0,FALSE);
										}
									}
								}
							}
							
							if(index1!=index2 && index1>=0 && index1<128 && index2>=0 && index2<128)
							{
								bool bTextArriveTm=false;
								if(m_sDispDistance[index2].distance.node_info.show_sides)
								{
									if(m_nTrainLeapLineFlag==3)
									{
										if(m_sDispDistance[index1].distance.node_info.show_sides)
										{
											CPoint p_entry,p_exit;
											p_entry.x=ptlEnd.x;
											p_exit.x=ptlEnd.x;

											if(first_bottom_exit<first_top_entry)
												p_entry.y=first_top_entry;
											else
												p_entry.y=first_bottom_entry;

											if(nDirectFlag_next == DOWN_DIRECT)
												p_exit.y=first_bottom_exit;
											else
												p_exit.y=first_top_exit;
																						
											p_entry.Offset(x_point_offset, y_point_offset);
											p_exit.Offset(x_point_offset, y_point_offset);
																				
											if(first_a_time_status && first_d_time_status)
											{
												DrawActualLine(pCDC,p_entry, p_exit, single->GetTrainInfo()->m_data.type,FALSE,0);
							
											}
											else
											{
												DrawActualLine(pCDC,p_entry, p_exit, single->GetTrainInfo()->m_data.type,FALSE,0,0,FALSE);
											}
										}
										else
										{
											bTextArriveTm=true;
											CPoint p_a,p_d,p_side_a,p_side_d;
											p_a.x=ptlStart.x;
											p_d.x=ptlEnd.x;
											
											if(nDirectFlag_next == DOWN_DIRECT)
												p_d.y = first_bottom_exit;
											else
												p_d.y = first_top_exit;

											if(nDirectFlag_pre == DOWN_DIRECT)
												p_a.y = first_top_entry;
											else
												p_a.y = first_bottom_entry;
																				
											p_side_a.x=p_a.x;
											p_side_d.x=p_d.x;
											p_side_a.y=p_side_d.y=first_primary_exit;

											p_a.Offset(x_point_offset, y_point_offset);
											p_side_a.Offset(x_point_offset, y_point_offset);
											p_side_d.Offset(x_point_offset, y_point_offset);
											p_d.Offset(x_point_offset, y_point_offset);
											
											if(first_a_time_status && first_d_time_status)
											{
												DrawActualLine(pCDC, p_a, p_side_a,single->GetTrainInfo()->m_data.type,FALSE,0);
												
												DrawActualLine(pCDC, p_side_d, p_d,single->GetTrainInfo()->m_data.type,FALSE,0);

												DrawActualLine(pCDC,p_side_a, p_side_d, single->GetTrainInfo()->m_data.type,FALSE,0);
							
											}
											else
											{
												DrawActualLine(pCDC, p_a, p_side_a,single->GetTrainInfo()->m_data.type,FALSE,0,0,FALSE);
												
												DrawActualLine(pCDC, p_side_d, p_d,single->GetTrainInfo()->m_data.type,FALSE,0,0,FALSE);

												DrawActualLine(pCDC,p_side_a, p_side_d, single->GetTrainInfo()->m_data.type,FALSE,0,0,FALSE);
											}
										}

									}
									else
									{
										CPoint p_a,p_d,p_side_a,p_side_d;
										p_a.x=ptlStart.x;
										p_d.x=ptlEnd.x;
										
										if(nDirectFlag_next == DOWN_DIRECT)
										{
											p_a.y = first_top_exit;
											p_d.y = first_bottom_exit;
										}
										else
										{
											p_a.y = first_bottom_exit;
											p_d.y = first_top_exit;
										}
										
										p_side_a.x=p_a.x;
										p_side_d.x=p_d.x;
										p_side_a.y=p_side_d.y=first_primary_exit;

										p_a.Offset(x_point_offset, y_point_offset);
										p_side_a.Offset(x_point_offset, y_point_offset);
										p_side_d.Offset(x_point_offset, y_point_offset);
										p_d.Offset(x_point_offset, y_point_offset);
										
										if(first_a_time_status && first_d_time_status)
										{
											DrawActualLine(pCDC, p_a, p_side_a,single->GetTrainInfo()->m_data.type,FALSE,0);
											
											DrawActualLine(pCDC, p_side_d, p_d,single->GetTrainInfo()->m_data.type,FALSE,0);

											DrawActualLine(pCDC,p_side_a, p_side_d, single->GetTrainInfo()->m_data.type,FALSE,0);
						
										}
										else
										{
											DrawActualLine(pCDC, p_a, p_side_a,single->GetTrainInfo()->m_data.type,FALSE,0,0,FALSE);
											
											DrawActualLine(pCDC, p_side_d, p_d,single->GetTrainInfo()->m_data.type,FALSE,0,0,FALSE);

											DrawActualLine(pCDC,p_side_a, p_side_d, single->GetTrainInfo()->m_data.type,FALSE,0,0,FALSE);
										}
									}
								}
								else 
								{
									CPoint p_entry,p_exit;
									if(m_nTrainLeapLineFlag==3)
									{
										p_entry.x=ptlEnd.x;
										p_exit.x=ptlEnd.x;

										if(first_bottom_exit<first_top_entry)
										{
											p_entry.y=first_top_entry;
											p_exit.y=first_bottom_exit;
										}
										else
										{
											p_entry.y=first_bottom_entry;
											p_exit.y=first_bottom_exit;
										}
									}
									else
									{
										p_entry.x=ptlStart.x;
										p_exit.x=ptlEnd.x;
										p_entry.y=p_exit.y=first_primary_exit;
									}								

									p_entry.Offset(x_point_offset, y_point_offset);
									p_exit.Offset(x_point_offset, y_point_offset);

									if(first_a_time_status && first_d_time_status)
									{
										DrawActualLine(pCDC,p_entry, p_exit, single->GetTrainInfo()->m_data.type,FALSE,0);
					
									}
									else
									{
										DrawActualLine(pCDC,p_entry, p_exit, single->GetTrainInfo()->m_data.type,FALSE,0,0,FALSE);
									}
								}

								//绘制到发车时间时分
								//下行车到达点 TA_RIGHT|TA_TOP
								//上行车到达点 TA_RIGHT|TA_BOTTOM	
								if(m_nTrainLeapLineFlag==3)
								{
									// 到点
									CPoint pt_a;
									pt_a.x = ptlStart.x;
									if(first_bottom_exit<first_top_entry)
										pt_a.y=first_bottom_exit;
									else
										pt_a.y=first_top_exit;

									pt_a.Offset(x_point_offset, y_point_offset);
									if(bTextArriveTm && (pt_a.x >= x_point_offset && pt_a.x<rangeRect.right+ x_point_offset && pt_a.y > 0))
									{						
										CFont* pOldFont, font;
										LOGFONT stFont;
										memset(&stFont,0,sizeof(stFont));
										stFont.lfHeight = MulDiv(m_font.time_char,m_nLogPixelSy,72); // time_char
										stFont.lfWeight = FW_NORMAL;
										stFont.lfClipPrecision = CLIP_LH_ANGLES;
										strcpy(stFont.lfFaceName,"Times New Roman");
										font.CreateFontIndirect(&stFont);

										pOldFont = pCDC->SelectObject(&font);						

										COLORREF Color = GetActualLineColor(factor);

										UINT nOldColor;
										if(manualRealColor>0 && TG_IsArriveActualByManual(first->m_data))
										{
											nOldColor = pCDC->SetTextColor(manualRealColor);
										}
										else
										{
											if(first_a_time_status == 1)
												nOldColor = pCDC->SetTextColor(Color);
										}
										CPoint point = ptlStart;
										if(first_bottom_exit>first_top_entry)//下行车 
										{   
											pCDC->SetTextAlign(TA_LEFT|TA_BOTTOM);
										}
										else
										{//up direct
											pCDC->SetTextAlign(TA_RIGHT|TA_TOP);
										}

										long time_depart,date_depart,time_arrival,date_arrival;
										time_depart = minute_time(first->m_data.depart,&date_depart);
										if(first->m_data.arrive == NOTIME || first->m_data.arrive == 0)
										{
											date_arrival = date_depart;
											time_arrival = time_depart;
										}
										else
										{
											time_arrival = minute_time(first->m_data.arrive,&date_arrival);
										}
										itoa(time_arrival/60 % 10, time_char, 10);				

										pCDC->TextOut(pt_a.x,pt_a.y,time_char,1);

										if(manualRealColor>0 && TG_IsArriveActualByManual(first->m_data))
										{
											pCDC->SetTextColor(nOldColor);
										}
										else
										{
											if(first_a_time_status == 1)
												pCDC->SetTextColor(nOldColor);
										}
										
										pCDC->SelectObject(pOldFont);   

										font.DeleteObject();
									}

									// 发点
									CPoint pt_d;
									pt_d.x = ptlEnd.x;
									if(nDirectFlag_next == DOWN_DIRECT)
										pt_d.y = first_bottom_exit;
									else
										pt_d.y = first_top_exit;
									pt_d.Offset(x_point_offset, y_point_offset);
									if((pt_d.x >= x_point_offset && pt_d.x<rangeRect.right+ x_point_offset && pt_d.y > 0))
									{						
										CFont* pOldFont, font;
										LOGFONT stFont;
										memset(&stFont,0,sizeof(stFont));
										stFont.lfHeight = MulDiv(m_font.time_char,m_nLogPixelSy,72); // time_char
										stFont.lfWeight = FW_NORMAL;
										stFont.lfClipPrecision = CLIP_LH_ANGLES;
										strcpy(stFont.lfFaceName,"Times New Roman");
										font.CreateFontIndirect(&stFont);

										pOldFont = pCDC->SelectObject(&font);						

										COLORREF Color = GetActualLineColor(factor);

										UINT nOldColor;
										if(manualRealColor>0 && TG_IsDepartActualByManual(first->m_data))
										{
											nOldColor = pCDC->SetTextColor(manualRealColor);
										}
										else
										{
											if(first_d_time_status == 1)
												nOldColor = pCDC->SetTextColor(Color);
										}
										CPoint point = ptlStart;
										if(nDirectFlag_next == DOWN_DIRECT)//下行车 
										{   
											pCDC->SetTextAlign(TA_RIGHT|TA_TOP);
										}
										else
										{//up direct
											pCDC->SetTextAlign(TA_RIGHT|TA_BOTTOM);
										}

										long time_depart,date_depart,time_arrival,date_arrival;
										time_arrival = minute_time(first->m_data.arrive,&date_arrival);
										if(first->m_data.depart == NOTIME || first->m_data.depart == 0)
										{
											date_depart = date_arrival;
											time_depart = time_arrival;
										}
										else
										{
											time_depart = minute_time(first->m_data.depart,&date_depart);
										}
										itoa(time_depart/60 % 10, time_char, 10);

										pCDC->TextOut(pt_d.x,pt_d.y,time_char,1);

										if(manualRealColor>0 && TG_IsDepartActualByManual(first->m_data))
										{
											pCDC->SetTextColor(nOldColor);
										}
										else
										{
											if(first_d_time_status == 1)
												pCDC->SetTextColor(nOldColor);
										}
										
										pCDC->SelectObject(pOldFont);   

										font.DeleteObject();
									}
								}
							}

							if(usStartTime != usEndTime)
							{
								m_pPerPointInfo[m_nPointCount].record_index = i;
								m_pPerPointInfo[m_nPointCount].start_depart_flag = ARRIVAL_POSITION;
								m_pPerPointInfo[m_nPointCount++].rect  = CRect(CPoint(ptlStart.x - uDis,ptlStart.y - uDis),sSize);/////////////////ptlStart.y
							}
						}
					}
					else
					{
						//基本图的过表
						CPoint criticalstart,criticalend;

						int basicMemoryWidth = m_fPixelPerHour*24;

						criticalstart =  CPoint(basicMemoryWidth+x_point_offset,ptlEnd.y+ y_point_offset);
						criticalend = CPoint(0 + x_point_offset,ptlEnd.y + y_point_offset);

						if(first_a_time_status && first_d_time_status)
						{
							DrawActualLine(pCDC,CPoint(ptlStart.x + x_point_offset,ptlStart.y + y_point_offset),
								criticalstart,single->GetTrainInfo()->m_data.type,FALSE,m_nCountOfEngine);//single->m_carriage_count);
							DrawActualLine(pCDC,criticalend,
								CPoint(ptlEnd.x+x_point_offset,ptlEnd.y+ y_point_offset),single->GetTrainInfo()->m_data.type,FALSE,m_nCountOfEngine);

							DrawMarkOfCritical(pCDC,single,single->GetTrainID(),criticalstart,50,RIGHT_MARK, lActualColor);
							DrawMarkOfCritical(pCDC,single,single->GetTrainID(),criticalend,50,LEFT_MARK, lActualColor);

						}
						else
						{
							DrawActualLine(pCDC,CPoint(ptlStart.x + x_point_offset,ptlStart.y + y_point_offset),
								criticalstart,single->GetTrainInfo()->m_data.type,FALSE,m_nCountOfEngine,0,FALSE);
							DrawActualLine(pCDC,criticalend,
								CPoint(ptlEnd.x+x_point_offset,ptlEnd.y+ y_point_offset),single->GetTrainInfo()->m_data.type,FALSE,m_nCountOfEngine,0,FALSE);
						}
					}
				}while(0);
				
				tmpArl=ptlStart;
				ptlStart = ptlEnd;
				//point rect information
				if(second == NULL){
					m_pPerPointInfo[m_nPointCount].record_index = i;
					m_pPerPointInfo[m_nPointCount].start_depart_flag = DEPART_POSITION;
					m_pPerPointInfo[m_nPointCount++].rect  = CRect(CPoint(ptlStart.x - uDis,ptlStart.y - uDis),sSize);
				}
			}
			else
			{//UP_DIRECT
				ptlStart.x =  (int)(flFirstMinuteOffset * m_fPixelPerMinute);
				ptlEnd.x = (int)(flSecondMinuteOffset * m_fPixelPerMinute);
				ushort usEntryFlag = ENTRY_POSITION;
				if(IsFlagStart(first->m_data.flag)){
					ptlStart.x = ptlEnd.x;
					usEntryFlag = EXIT_POSITION;
				}
				if(second == NULL && IsFlagTerminal(first->m_data.flag))
					ptlEnd.x =	ptlStart.x ;

				ptlStart.y = ptlEnd.y =  GetStationVPos(single,first,PRIMARY_TRACK, usEntryFlag,schd_type);
				if(ptlEnd.y == -1) continue;

				//draw side point 放开
				if (m_nDrawSidePoint)
				{
					if(GetStationVPos(single,first,PRIMARY_TRACK,usEntryFlag,m_nSchdStatus) != 
						GetStationVPos(single,first,TOP_TRACK,usEntryFlag,m_nSchdStatus))
					{
						int i=-1;
						if ((ptlStart.x == ptlEnd.x)&&(GetNodeIndex(single,first,i,(ushort)usEntryFlag)>-1))
						{
							if(m_sDispDistance[i].distance.node_info.show_sides
								&&m_sDispDistance[i].distance.node_info.show_sides_mode ==0)
							{
								CPoint pt = ptlStart;
								CSize ssSize = CSize(8,8);
								int scaleRate = (fHeightMulti>5?5:fHeightMulti);

								int uuDis=4*scaleRate;
								ssSize.cx *= scaleRate;
								ssSize.cy *= scaleRate;

								CBrush tempBrush(lColor);
								CBrush *pOldBrush;
								pOldBrush = pCDC->SelectObject(&tempBrush);
								if(ptlStart.x > 0 && ptlStart.x < rangeRect.right)
								{  
									pt.Offset (x_point_offset,y_point_offset);
									pCDC->RoundRect(CRect(CPoint(pt.x - uuDis,pt.y - uuDis),ssSize),pt);
								}
								pCDC->SelectObject(pOldBrush);
							}
						}
					}
				}

				//6:00 or 18:00 position
				do
				{
					if((ptlStart.x < 0 && ptlEnd.x < 0 ))
					{
						break;
					}
					else if ((ptlStart.x < 0 && ptlEnd.x >= 0 )) 
					{
						ptlStart.x = 0;
						critical_point = ptlStart;
						critical_point.Offset (x_point_offset,y_point_offset);
						//判断是否在该车站改了车次号，如果改了，使用新车次号
						NewTrainId = "";
						for(critial_record_index =0; critial_record_index<single->GetTrainRecordCount(); critial_record_index++)//查找在本站是否该了车次号
						{
							if(i == single->GetTrainRecord(critial_record_index)->m_data.station && single->GetTrainRecord(critial_record_index)->m_data.arrive_train_id != single->GetTrainRecord(critial_record_index)->m_data.depart_train_id
								&& strlen(single->GetTrainRecord(critial_record_index)->m_data.arrive_train_id)>0 && strlen(single->GetTrainRecord(critial_record_index)->m_data.depart_train_id)>0)
							{
								NewTrainId.Format("%s",single->GetTrainRecord(critial_record_index)->m_data.depart_train_id);
								break;
							}
						}

						//判断新车次号结束							
						if(NewTrainId.IsEmpty())
							NewTrainId = single->GetTrainID();

						if(first_a_time_status)
							DrawMarkOfCritical(pCDC,single,NewTrainId,critical_point,50,LEFT_MARK, lActualColor);
						else							
							DrawMarkOfCritical(pCDC,single,NewTrainId,critical_point,50,LEFT_MARK, lColor);
					}

					if((ptlStart.x > rangeRect.right) && 
						ptlEnd.x >= rangeRect.right)
					{
						break;
					}
					else if((ptlStart.x < rangeRect.right) && 
						ptlEnd.x >= rangeRect.right)
					{
						ptlEnd.x = rangeRect.right;
						critical_point = ptlEnd;
						critical_point.Offset (x_point_offset,y_point_offset);
						//判断是否在该车站改了车次号，如果改了，使用新车次号
						NewTrainId = "";
						for(critial_record_index =0; critial_record_index<single->GetTrainRecordCount(); critial_record_index++)//查找在本站是否该了车次号
						{
							if(i == single->GetTrainRecord(critial_record_index)->m_data.station && single->GetTrainRecord(critial_record_index)->m_data.arrive_train_id != single->GetTrainRecord(critial_record_index)->m_data.depart_train_id
								&& strlen(single->GetTrainRecord(critial_record_index)->m_data.arrive_train_id)>0 && strlen(single->GetTrainRecord(critial_record_index)->m_data.depart_train_id)>0)
							{
								NewTrainId.Format("%s",single->GetTrainRecord(critial_record_index)->m_data.depart_train_id);
								break;
							}
						}

						//判断新车次号结束							
						if(NewTrainId.IsEmpty())
							NewTrainId = single->GetTrainID();

						if(first_a_time_status)
							DrawMarkOfCritical(pCDC,single,NewTrainId,critical_point,50,RIGHT_MARK, lActualColor);
						else							
							DrawMarkOfCritical(pCDC,single,NewTrainId,critical_point,50,RIGHT_MARK, lColor);

					}

					if((m_nSchdStatus == WORK_SCHEDULE|| m_nSchdStatus == SHIFT_SCHEDULE ||m_nSchdStatus == HISTORY_SCHEDULE) == TRUE? TRUE:(ptlEnd.x >= ptlStart.x))
					{

						if(ptlEnd.x <= rangeRect.right)
						{
							int index1 = first->GetArriveTGIndex();
							int index2 = first->GetDepartTGIndex();
							bool bDrawStopLine=true;
							if(m_nTrainLeapLineFlag==3 && index1!=index2 &&  index1>=0 && index1<128 && index2>=0 && index2<128)
							{
								if(m_sDispDistance[index1].distance.node_info.show_sides==false && m_sDispDistance[index2].distance.node_info.show_sides)
									bDrawStopLine=false;
							}
							if(bDrawStopLine)
							{
								if(first_a_time_status && first_d_time_status)
								{
									int yStation;
									if(i == 0 && !IsFlagStart(first->m_data.flag))
									{							
										yStation = first_bottom_entry;
										DrawActualLine(pCDC,CPoint(ptlStart.x + x_point_offset,yStation + y_point_offset),
											CPoint(ptlStart.x+x_point_offset,ptlStart.y+ y_point_offset),single->GetTrainInfo()->m_data.type,FALSE,m_nCountOfEngine);//single->m_carriage_count);//draw real time line
									}
									if(m_bStopStationWaveLine && JudgeNeedWaveLine(first,schd_type,m_nDirectFlag_prefix))
										DrawActualStopStationLine(pCDC,CPoint(ptlStart.x+x_point_offset, ptlStart.y+y_point_offset),
										CPoint(ptlEnd.x + x_point_offset,ptlEnd.y+y_point_offset),single->GetTrainInfo()->m_data.type,UP_DIRECT);
									else
										DrawActualLine(pCDC,CPoint(ptlStart.x + x_point_offset,ptlStart.y + y_point_offset),
										CPoint(ptlEnd.x+x_point_offset,ptlEnd.y+ y_point_offset),single->GetTrainInfo()->m_data.type,FALSE,m_nCountOfEngine);//single->m_carriage_count);//draw real time line
								}
								else
								{
									// 车站有停留，上行车的情况，三条线
									int yStation;
									if(i == 0 && !IsFlagStart(first->m_data.flag))
									{  						
										yStation = first_bottom_entry;

										int flag = GetEntryExitDirection(single,first,(ushort)ENTRY_POSITION,locCurrentSectionNo);////,m_nSchdStatus);
										int direct_flag=-1; // // BUG!!! UMR
										if(flag != -1)
											direct_flag = flag;						   
										yStation = (direct_flag == UP_MARK ?  first_top_entry : first_bottom_entry);


										DrawActualLine(pCDC,CPoint(ptlStart.x + x_point_offset,yStation + y_point_offset),
											CPoint(ptlStart.x+x_point_offset,ptlStart.y+ y_point_offset),single->GetTrainInfo()->m_data.type,FALSE,m_nCountOfEngine,0,FALSE);

										if(yStation != ptlStart.y)
										{
											m_pPerPointInfo[m_nPointCount].record_index = i;
											m_pPerPointInfo[m_nPointCount].start_depart_flag = ARRIVAL_POSITION;
											m_pPerPointInfo[m_nPointCount++].rect  = CRect(CPoint(ptlStart.x - uDis,yStation - uDis),sSize); ////ptlStart.y
										}
									}

									if(!(second == NULL && IsFlagTerminal(first->m_data.flag)) && !IsFlagUnitOtherTrain(first->m_data.flag)&& !IsFlagEnd(first->m_data.flag))
									{
										if(m_bStopStationWaveLine && JudgeNeedWaveLine(first,schd_type,m_nDirectFlag_prefix))
											DrawStopStationLine(pCDC,CPoint(ptlStart.x+x_point_offset, ptlStart.y+y_point_offset),
											CPoint(ptlEnd.x + x_point_offset,ptlEnd.y+y_point_offset),UP_DIRECT);
										else
											DrawActualLine(pCDC,CPoint(ptlStart.x + x_point_offset,ptlStart.y + y_point_offset),
											CPoint(ptlEnd.x+x_point_offset,ptlEnd.y+ y_point_offset),single->GetTrainInfo()->m_data.type,FALSE,m_nCountOfEngine,0,FALSE);
										
									}
								}
							}
							
							if(index1!=index2 && index1>=0 && index1<128 && index2>=0 && index2<128)
							{
								bool bTextArriveTm=false;
								if(m_sDispDistance[index2].distance.node_info.show_sides)
								{
									if(m_nTrainLeapLineFlag==3)
									{
										if(m_sDispDistance[index1].distance.node_info.show_sides)
										{
											CPoint p_entry,p_exit;
											p_entry.x=ptlEnd.x;
											p_exit.x=ptlEnd.x;

											if(first_bottom_exit<first_top_entry)
												p_entry.y=first_top_entry;
											else
												p_entry.y=first_bottom_entry;

											if(nDirectFlag_next == DOWN_DIRECT)
												p_exit.y=first_bottom_exit;
											else
												p_exit.y=first_top_exit;
																						
											p_entry.Offset(x_point_offset, y_point_offset);
											p_exit.Offset(x_point_offset, y_point_offset);
																				
											if(first_a_time_status && first_d_time_status)
											{
												DrawActualLine(pCDC,p_entry, p_exit, single->GetTrainInfo()->m_data.type,FALSE,0);
							
											}
											else
											{
												DrawActualLine(pCDC,p_entry, p_exit, single->GetTrainInfo()->m_data.type,FALSE,0,0,FALSE);
											}
										}
										else
										{
											bTextArriveTm=true;
											CPoint p_a,p_d,p_side_a,p_side_d;
											p_a.x=ptlStart.x;
											p_d.x=ptlEnd.x;
											
											if(nDirectFlag_next == DOWN_DIRECT)
												p_d.y = first_bottom_exit;
											else
												p_d.y = first_top_exit;

											if(nDirectFlag_pre == DOWN_DIRECT)
												p_a.y = first_top_entry;
											else
												p_a.y = first_bottom_entry;
																						
											p_side_a.x=p_a.x;
											p_side_d.x=p_d.x;
											p_side_a.y=p_side_d.y=first_primary_exit;

											p_a.Offset(x_point_offset, y_point_offset);
											p_side_a.Offset(x_point_offset, y_point_offset);
											p_side_d.Offset(x_point_offset, y_point_offset);
											p_d.Offset(x_point_offset, y_point_offset);
											
											if(first_a_time_status && first_d_time_status)
											{
												DrawActualLine(pCDC, p_a, p_side_a,single->GetTrainInfo()->m_data.type,FALSE,0);
												
												DrawActualLine(pCDC, p_side_d, p_d,single->GetTrainInfo()->m_data.type,FALSE,0);

												DrawActualLine(pCDC,p_side_a, p_side_d, single->GetTrainInfo()->m_data.type,FALSE,0);
							
											}
											else
											{
												DrawActualLine(pCDC, p_a, p_side_a,single->GetTrainInfo()->m_data.type,FALSE,0,0,FALSE);
												
												DrawActualLine(pCDC, p_side_d, p_d,single->GetTrainInfo()->m_data.type,FALSE,0,0,FALSE);

												DrawActualLine(pCDC,p_side_a, p_side_d, single->GetTrainInfo()->m_data.type,FALSE,0,0,FALSE);
											}

											CPoint p_entry,p_exit;
											p_entry.x=ptlStart.x;
											p_exit.x=ptlStart.x;

											if(first_bottom_exit<first_top_entry)
											{
												p_entry.y=first_top_entry;
												p_exit.y=first_bottom_exit;
											}
											else
											{
												p_entry.y=first_bottom_entry;
												p_exit.y=first_top_exit;
											}

											p_entry.Offset(x_point_offset, y_point_offset);
											p_exit.Offset(x_point_offset, y_point_offset);
																				
											if(first_a_time_status && first_d_time_status)
											{
												DrawActualLine(pCDC,p_entry, p_exit, single->GetTrainInfo()->m_data.type,FALSE,0);
							
											}
											else
											{
												DrawActualLine(pCDC,p_entry, p_exit, single->GetTrainInfo()->m_data.type,FALSE,0,0,FALSE);
											}
										}
									}
									else
									{
										CPoint p_a,p_d,p_side_a,p_side_d;
										p_a.x=ptlStart.x;
										p_d.x=ptlEnd.x;
										
										if(nDirectFlag_next == DOWN_DIRECT)
										{
											p_a.y = first_top_exit;
											p_d.y = first_bottom_exit;
										}
										else
										{
											p_a.y = first_bottom_exit;
											p_d.y = first_top_exit;
										}

										p_side_a.x=p_a.x;
										p_side_d.x=p_d.x;
										p_side_a.y=p_side_d.y=first_primary_exit;

										p_a.Offset(x_point_offset, y_point_offset);
										p_side_a.Offset(x_point_offset, y_point_offset);
										p_side_d.Offset(x_point_offset, y_point_offset);
										p_d.Offset(x_point_offset, y_point_offset);
										
										if(first_a_time_status && first_d_time_status)
										{
											DrawActualLine(pCDC, p_a, p_side_a,single->GetTrainInfo()->m_data.type,FALSE,0);
											
											DrawActualLine(pCDC, p_side_d, p_d,single->GetTrainInfo()->m_data.type,FALSE,0);

											DrawActualLine(pCDC,p_side_a, p_side_d, single->GetTrainInfo()->m_data.type,FALSE,0);
						
										}
										else
										{
											DrawActualLine(pCDC, p_a, p_side_a,single->GetTrainInfo()->m_data.type,FALSE,0,0,FALSE);
											
											DrawActualLine(pCDC, p_side_d, p_d,single->GetTrainInfo()->m_data.type,FALSE,0,0,FALSE);

											DrawActualLine(pCDC,p_side_a, p_side_d, single->GetTrainInfo()->m_data.type,FALSE,0,0,FALSE);
										}
									}
								}
								else
								{
									CPoint p_entry,p_exit;
									if(m_nTrainLeapLineFlag==3)
									{
										p_entry.x=ptlEnd.x;
										p_exit.x=ptlEnd.x;

										if(first_bottom_exit<first_top_entry)
										{
											p_entry.y=first_top_entry;
											p_exit.y=first_bottom_exit;
										}
										else
										{
											p_entry.y=first_bottom_entry;
											p_exit.y=first_bottom_exit;
										}
									}
									else
									{
										p_entry.x=ptlStart.x;
										p_exit.x=ptlEnd.x;
										p_entry.y=p_exit.y=first_primary_exit;
									}

									p_entry.Offset(x_point_offset, y_point_offset);
									p_exit.Offset(x_point_offset, y_point_offset);
																		
									if(first_a_time_status && first_d_time_status)
									{
										DrawActualLine(pCDC,p_entry, p_exit, single->GetTrainInfo()->m_data.type,FALSE,0);
					
									}
									else
									{
										DrawActualLine(pCDC,p_entry, p_exit, single->GetTrainInfo()->m_data.type,FALSE,0,0,FALSE);
									}
								}

								//绘制到发车时间时分
								//下行车到达点 TA_RIGHT|TA_TOP
								//上行车到达点 TA_RIGHT|TA_BOTTOM	
								if(m_nTrainLeapLineFlag==3)
								{
									// 到点
									CPoint pt_a;
									pt_a.x = ptlStart.x;
									if(first_bottom_exit<first_top_entry)
										pt_a.y=first_bottom_exit;
									else
										pt_a.y=first_top_exit;

									pt_a.Offset(x_point_offset, y_point_offset);
									if(bTextArriveTm && (pt_a.x >= x_point_offset && pt_a.x<rangeRect.right+ x_point_offset && pt_a.y > 0))
									{						
										CFont* pOldFont, font;
										LOGFONT stFont;
										memset(&stFont,0,sizeof(stFont));
										stFont.lfHeight = MulDiv(m_font.time_char,m_nLogPixelSy,72); // time_char
										stFont.lfWeight = FW_NORMAL;
										stFont.lfClipPrecision = CLIP_LH_ANGLES;
										strcpy(stFont.lfFaceName,"Times New Roman");
										font.CreateFontIndirect(&stFont);

										pOldFont = pCDC->SelectObject(&font);						

										COLORREF Color = GetActualLineColor(factor);

										UINT nOldColor;
										if(manualRealColor>0 && TG_IsArriveActualByManual(first->m_data))
										{
											nOldColor = pCDC->SetTextColor(manualRealColor);
										}
										else
										{
											if(first_a_time_status == 1)
												nOldColor = pCDC->SetTextColor(Color);
										}
										CPoint point = ptlStart;
										if(first_bottom_exit>first_top_entry)//下行车 
										{   
											pCDC->SetTextAlign(TA_LEFT|TA_BOTTOM);
										}
										else
										{//up direct
											pCDC->SetTextAlign(TA_RIGHT|TA_TOP);
										}

										long time_depart,date_depart,time_arrival,date_arrival;
										time_depart = minute_time(first->m_data.depart,&date_depart);
										if(first->m_data.arrive == NOTIME || first->m_data.arrive == 0)
										{
											date_arrival = date_depart;
											time_arrival = time_depart;
										}
										else
										{
											time_arrival = minute_time(first->m_data.arrive,&date_arrival);
										}
										itoa(time_arrival/60 % 10, time_char, 10);				

										pCDC->TextOut(pt_a.x,pt_a.y,time_char,1);

										if(manualRealColor>0 && TG_IsArriveActualByManual(first->m_data))
										{
											pCDC->SetTextColor(nOldColor);
										}
										else
										{
											if(first_a_time_status == 1)
												pCDC->SetTextColor(nOldColor);
										}
										
										pCDC->SelectObject(pOldFont);   

										font.DeleteObject();
									}

									// 发点
									CPoint pt_d;
									pt_d.x = ptlEnd.x;
									if(nDirectFlag_next == DOWN_DIRECT)
										pt_d.y = first_bottom_exit;
									else
										pt_d.y = first_top_exit;
									pt_d.Offset(x_point_offset, y_point_offset);
									if((pt_d.x >= x_point_offset && pt_d.x<rangeRect.right+ x_point_offset && pt_d.y > 0))
									{						
										CFont* pOldFont, font;
										LOGFONT stFont;
										memset(&stFont,0,sizeof(stFont));
										stFont.lfHeight = MulDiv(m_font.time_char,m_nLogPixelSy,72); // time_char
										stFont.lfWeight = FW_NORMAL;
										stFont.lfClipPrecision = CLIP_LH_ANGLES;
										strcpy(stFont.lfFaceName,"Times New Roman");
										font.CreateFontIndirect(&stFont);

										pOldFont = pCDC->SelectObject(&font);						

										COLORREF Color = GetActualLineColor(factor);

										UINT nOldColor;
										if(manualRealColor>0 && TG_IsDepartActualByManual(first->m_data))
										{
											nOldColor = pCDC->SetTextColor(manualRealColor);
										}
										else
										{
											if(first_d_time_status == 1)
												nOldColor = pCDC->SetTextColor(Color);
										}
										CPoint point = ptlStart;
										if(nDirectFlag_next == DOWN_DIRECT)//下行车 
										{   
											pCDC->SetTextAlign(TA_RIGHT|TA_TOP);
										}
										else
										{//up direct
											pCDC->SetTextAlign(TA_RIGHT|TA_BOTTOM);
										}

										long time_depart,date_depart,time_arrival,date_arrival;
										time_arrival = minute_time(first->m_data.arrive,&date_arrival);
										if(first->m_data.depart == NOTIME || first->m_data.depart == 0)
										{
											date_depart = date_arrival;
											time_depart = time_arrival;
										}
										else
										{
											time_depart = minute_time(first->m_data.depart,&date_depart);
										}
										itoa(time_depart/60 % 10, time_char, 10);

										pCDC->TextOut(pt_d.x,pt_d.y,time_char,1);

										if(manualRealColor>0 && TG_IsDepartActualByManual(first->m_data))
										{
											pCDC->SetTextColor(nOldColor);
										}
										else
										{
											if(first_d_time_status == 1)
												pCDC->SetTextColor(nOldColor);
										}
										
										pCDC->SelectObject(pOldFont);   

										font.DeleteObject();
									}
								}
							}
							if(usStartTime != usEndTime)
							{
								m_pPerPointInfo[m_nPointCount].record_index = i;
								if(IsFlagStart(first->m_data.flag))
									m_pPerPointInfo[m_nPointCount].start_depart_flag = UNION_POSITION;
								else
									m_pPerPointInfo[m_nPointCount].start_depart_flag = ARRIVAL_POSITION;
								m_pPerPointInfo[m_nPointCount++].rect  = CRect(CPoint(ptlStart.x - uDis,ptlStart.y - uDis),sSize); ////ptlStart.y
							}
						}
					}
					else
					{
						//基本图的过表
						CPoint criticalstart,criticalend;
						int basicMemoryWidth = m_fPixelPerHour*24;

						criticalstart =  CPoint(basicMemoryWidth+x_point_offset,ptlEnd.y+ y_point_offset);
						criticalend = CPoint(0 + x_point_offset,ptlEnd.y + y_point_offset);

						if(first_a_time_status && first_d_time_status)
						{
							DrawActualLine(pCDC,CPoint(ptlStart.x + x_point_offset,ptlStart.y + y_point_offset),
								criticalstart,single->GetTrainInfo()->m_data.type,FALSE,m_nCountOfEngine);//single->m_carriage_count);//draw real time line
							DrawActualLine(pCDC,criticalend,
								CPoint(ptlEnd.x+x_point_offset,ptlEnd.y+ y_point_offset),single->GetTrainInfo()->m_data.type,FALSE,m_nCountOfEngine);//single->m_carriage_count);//draw real time line

							DrawMarkOfCritical(pCDC,single,single->GetTrainID(),criticalstart,50,RIGHT_MARK, lActualColor);
							DrawMarkOfCritical(pCDC,single,single->GetTrainID(),criticalend,50,LEFT_MARK, lActualColor);
						}
						else
						{/////
							DrawActualLine(pCDC,CPoint(ptlStart.x + x_point_offset,ptlStart.y + y_point_offset),
								criticalstart,single->GetTrainInfo()->m_data.type,FALSE,m_nCountOfEngine,0,FALSE);
							DrawActualLine(pCDC,criticalend,
								CPoint(ptlEnd.x+x_point_offset,ptlEnd.y+ y_point_offset),single->GetTrainInfo()->m_data.type,FALSE,m_nCountOfEngine,0,FALSE);
						}
					}	
				}while(0);

				tmpArl=ptlStart;
				ptlStart = ptlEnd; 
			}
			if(ptlStart.y == -1) continue; 


			// draw arrive minute text
			time_arrival = minute_time(first->m_data.arrive,&date_arrival);
			itoa(time_arrival/60 % 10, time_char, 10);

            // 直接调用函数 PtInRects, 该函数定义在plotdoc.cpp/h中
			if((PtInRects(rangeRect, ptlStart) || PtInRects(rangeRect, tmpArl)) && gpcdef.GetFlagDrawTrainTime(first->m_data.station))
			{
				CFont* pOldFont, font;
				LOGFONT stFont;
				memset(&stFont,0,sizeof(stFont));
				stFont.lfHeight = MulDiv(m_font.time_char,m_nLogPixelSy,72); // time_char
				stFont.lfWeight = FW_NORMAL;
				stFont.lfClipPrecision = CLIP_LH_ANGLES;
				strcpy(stFont.lfFaceName,"Times New Roman");
				font.CreateFontIndirect(&stFont);

				pOldFont = pCDC->SelectObject(&font);
				COLORREF Color = GetActualLineColor(factor);

				UINT nOldColor;
				if(manualRealColor>0 && TG_IsArriveActualByManual(first->m_data))
				{
					nOldColor = pCDC->SetTextColor(manualRealColor);
				}
				else
				{
					if(first_a_time_status == 1)
						nOldColor = pCDC->SetTextColor(Color);
				}
				CPoint point;
				point.x = (int)(flFirstMinuteOffset * m_fPixelPerMinute);

				if(nDirectFlag == 2)//down direct
				{
					if(IsFlagStart(first->m_data.flag))   ////start  
					{   
						if(first->m_data.arrive == first->m_data.depart)
						{
							pCDC->SetTextAlign(TA_CENTER|TA_TOP);
							point.y =  first_bottom_exit;
							pCDC->SetTextAlign(TA_CENTER|TA_TOP);
							if(point.y >=0)
							{
								pCDC->TextOut((int)point.x + x_point_offset, point.y + y_point_offset, time_char, 1);  //4
							}
						}
					}
					else
					{
						point.y =  first_bottom_entry;
						int yStation = first_top_entry;
						if(first->m_data.arrive == first->m_data.depart)
						{ 
							point.y =  first_bottom_exit;
							if(point.x >=0)    //下行通过点 加边界值
							{
								if(second!=NULL )
								{
									//2004-11-16
									if(nDirectFlag_pre != nDirectFlag_next)
										pCDC->SetTextAlign(TA_CENTER|TA_BOTTOM);
									else
										pCDC->SetTextAlign(TA_CENTER|TA_TOP);

									pCDC->TextOut((int)point.x + x_point_offset, point.y + y_point_offset, time_char, 1);  //4
								}
								else
								{ 
									if(!IsFlagTerminal(first->m_data.flag))
									{
										int flag = GetEntryExitDirection(single,first,(ushort)EXIT_POSITION,locCurrentSectionNo);
										if(flag != -1)
										{
											if(flag != UP_MARK)
												pCDC->SetTextAlign(TA_CENTER|TA_TOP);
											else
												pCDC->SetTextAlign(TA_CENTER|TA_BOTTOM);
										}
										pCDC->TextOut((int)point.x  + x_point_offset,point.y + y_point_offset, time_char, 1);	
									}
									else
									{
										pCDC->SetTextAlign(TA_CENTER|TA_BOTTOM);
										pCDC->TextOut((int)point.x + x_point_offset, yStation+ y_point_offset, time_char, 1);  //4
									}
								}
							}
						}
						else
						{
							if(point.x >=0)    //下行到开的到点 加边界值
							{
								int flag = GetEntryExitDirection(single,first,(ushort)ENTRY_POSITION,locCurrentSectionNo);
								if(flag != -1)
								{
									if(flag != UP_MARK)
										pCDC->SetTextAlign(TA_CENTER|TA_TOP);
									else
										pCDC->SetTextAlign(TA_CENTER|TA_BOTTOM);
								}
								pCDC->TextOut((int)point.x +1 + x_point_offset, yStation +1 + y_point_offset, time_char, 1);   //2
							}
						}
					}
				}
				else  //up direct
				{ 
					pCDC->SetTextAlign(TA_CENTER|TA_BOTTOM);
					int flag = GetEntryExitDirection(single,first,(ushort)ENTRY_POSITION,locCurrentSectionNo);
					if(first->m_data.arrive != first->m_data.depart)
					{
						if(flag != -1)
						{
							if(flag != UP_MARK)
								pCDC->SetTextAlign(TA_CENTER|TA_TOP);
							else
								pCDC->SetTextAlign(TA_CENTER|TA_BOTTOM);
						}
					}

					if(IsFlagStart(first->m_data.flag))
					{  
						if(first->m_data.arrive == first->m_data.depart)
						{
							pCDC->SetTextAlign(TA_CENTER|TA_BOTTOM);
							point.y = first_top_exit;
							pCDC->SetTextAlign(TA_CENTER|TA_BOTTOM);
							if(point.x >=0)   //始发点加边界值
							{
								pCDC->TextOut((int)point.x + x_point_offset-3,point.y + y_point_offset, time_char, 1);	
							}
						}
					}
					else
					{   ////Arival 
						point.y =  first_top_entry;
						int yStation = first_bottom_entry;
						if(first->m_data.arrive == first->m_data.depart)  
						{  
							point.y =  first_top_exit;
							if(point.x >=0)    //上行通过点加边界值
							{
								if(second!=NULL )
								{
									if(nDirectFlag_pre != nDirectFlag_next)
										pCDC->SetTextAlign(TA_CENTER|TA_TOP);
									else
										pCDC->SetTextAlign(TA_CENTER|TA_BOTTOM);

									pCDC->TextOut((int)point.x + x_point_offset,point.y + y_point_offset, time_char, 1);	
								}
								else								 
								{ 
									if(!IsFlagTerminal(first->m_data.flag))
									{
										int flag = GetEntryExitDirection(single,first,(ushort)EXIT_POSITION,locCurrentSectionNo);
										if(flag != -1)
										{
											if(flag != UP_MARK)
												pCDC->SetTextAlign(TA_CENTER|TA_TOP);
											else
												pCDC->SetTextAlign(TA_CENTER|TA_BOTTOM);
										}
										pCDC->TextOut((int)point.x  + x_point_offset,point.y + y_point_offset, time_char, 1);	
									}
									else
									{
										pCDC->SetTextAlign(TA_CENTER|TA_TOP);
										pCDC->TextOut((int)point.x + x_point_offset,yStation + y_point_offset, time_char, 1);	
									}
								}
							}
						}
						else
						{
							if(point.x >=0)  //上行到开的到点加边界值
							{
								int flag = GetEntryExitDirection(single,first,(ushort)ENTRY_POSITION,locCurrentSectionNo);
								if(flag != -1)
								{
									if(flag != UP_MARK)
										pCDC->SetTextAlign(TA_CENTER|TA_TOP);
									else
										pCDC->SetTextAlign(TA_CENTER|TA_BOTTOM);
								}
								pCDC->TextOut((int)point.x + x_point_offset,yStation +1 + y_point_offset, time_char, 1);	
							}
						}
					}
				}
				pCDC->SelectObject(pOldFont);   
				font.DeleteObject();    

				if(manualRealColor>0 && TG_IsArriveActualByManual(first->m_data))
				{
					pCDC->SetTextColor(nOldColor);
				}
				else
				{
					if(first_a_time_status == 1)
						pCDC->SetTextColor(nOldColor);
				}
			}
		}//end of depart time != arrival time code

		// draw inter station line
		long date_depart,time_depart;
		time_depart = minute_time(first->m_data.depart,&date_depart);
		flFirstMinuteOffset = GetMinuteWidthToPlotStartTime(time_depart/60,date_depart,isPrint,m_nSchdStatus);       
		//draw side content 
		if(nDirectFlag == 2)
		{//DOWN_DIRECT
			ushort usEntryFlag = ENTRY_POSITION;
			if(IsFlagStart(first->m_data.flag))
				usEntryFlag = EXIT_POSITION;
			ptlStart.x = ptlEnd.x = (int)(flFirstMinuteOffset * m_fPixelPerMinute);
			ptlStart.y  =  GetStationVPos(single,first,PRIMARY_TRACK,usEntryFlag,m_nSchdStatus);
			if(ptlStart.y == -1) continue;          
			ptlEnd.y = GetStationVPos(single,first,BOTTOM_TRACK, usEntryFlag,m_nSchdStatus);
			if(ptlEnd.y == -1) continue;
			if(ptlEnd.x <= rangeRect.right && ptlEnd.x >= 0)
			{
				if(!IsFlagTerminal(first->m_data.flag) && !IsFlagAbsTerminal(first->m_data.flag))
				{
					// 绘制股道到发点竖线
					if(m_nTrainLeapLineFlag==3)
					{
						if(first_bottom_exit<first_top_entry)
							ptlEnd.y=first_top_entry;
						else
							ptlEnd.y=first_bottom_entry;
					}

					if(first_d_time_status && second_a_time_status)
					{
						DrawActualLine(pCDC,CPoint(ptlStart.x + x_point_offset,ptlStart.y + y_point_offset),
							CPoint(ptlEnd.x+x_point_offset,ptlEnd.y+ y_point_offset),single->GetTrainInfo()->m_data.type,FALSE,m_nCountOfEngine);//single->m_carriage_count);//draw real time line

					}
					else
					{
						DrawActualLine(pCDC,CPoint(ptlStart.x + x_point_offset,ptlStart.y + y_point_offset),
							CPoint(ptlEnd.x+x_point_offset,ptlEnd.y+ y_point_offset),single->GetTrainInfo()->m_data.type,FALSE,m_nCountOfEngine,0,FALSE);
					}
				}

				//point rect information
				if(!(second == NULL && IsFlagTerminal(first->m_data.flag) && first->m_data.arrive != first->m_data.depart))
				{
					m_pPerPointInfo[m_nPointCount].record_index = i;
					if(first->m_data.arrive == first->m_data.depart)
						m_pPerPointInfo[m_nPointCount].start_depart_flag = UNION_POSITION;
					else
						m_pPerPointInfo[m_nPointCount].start_depart_flag = DEPART_POSITION;
					m_pPerPointInfo[m_nPointCount++].rect  = CRect(CPoint(ptlStart.x - uDis,ptlStart.y - uDis),sSize);
				}

				if( ptlStart.y != ptlEnd.y && !(second == NULL && IsFlagTerminal(first->m_data.flag)) )
				{  
					m_pPerPointInfo[m_nPointCount].record_index = i;
					if(first->m_data.arrive == first->m_data.depart)
						m_pPerPointInfo[m_nPointCount].start_depart_flag = UNION_POSITION;
					else
						m_pPerPointInfo[m_nPointCount].start_depart_flag = DEPART_POSITION;
					m_pPerPointInfo[m_nPointCount++].rect  = CRect(CPoint(ptlEnd.x - uDis,ptlEnd.y - uDis),sSize);

				}
			}
			//for record exit position != entry position 
			if(ptlEnd.y != first_bottom_exit && second != NULL)
			{
				ptlEnd.y = first_bottom_exit;
				m_pPerPointInfo[m_nPointCount].record_index = i;
				if(first->m_data.arrive == first->m_data.depart)
					m_pPerPointInfo[m_nPointCount].start_depart_flag = UNION_POSITION;
				else
					m_pPerPointInfo[m_nPointCount].start_depart_flag = DEPART_POSITION;
				m_pPerPointInfo[m_nPointCount++].rect  = CRect(CPoint(ptlEnd.x - uDis,ptlEnd.y - uDis),sSize);
			}
			if(second !=NULL)
				ptlStart = ptlEnd;
		}
		else//UP_DIRECT
		{
			ushort usEntryFlag = ENTRY_POSITION;
			if(IsFlagStart(first->m_data.flag))
				usEntryFlag = EXIT_POSITION;
			ptlStart.x = ptlEnd.x = (int)(flFirstMinuteOffset * m_fPixelPerMinute);
			ptlStart.y  =  GetStationVPos(single,first, PRIMARY_TRACK, usEntryFlag,m_nSchdStatus);
			if(ptlStart.y == -1) continue; 			
			ptlEnd.y = GetStationVPos(single,first, TOP_TRACK,usEntryFlag,m_nSchdStatus);
			if(ptlEnd.y == -1) continue;
			if(ptlEnd.x <= rangeRect.right && ptlEnd.x >= 0)
			{
				if(!IsFlagTerminal(first->m_data.flag) && !IsFlagAbsTerminal(first->m_data.flag))
				{
					// 绘制股道到发点竖线
					if(m_nTrainLeapLineFlag==3)
					{
						if(first_bottom_exit<first_top_entry)
							ptlEnd.y=first_top_entry;
						else
							ptlEnd.y=first_bottom_entry;
					}

					if(first_d_time_status && second_a_time_status)
					{
						DrawActualLine(pCDC,CPoint(ptlStart.x + x_point_offset,ptlStart.y + y_point_offset),
							CPoint(ptlEnd.x+x_point_offset,ptlEnd.y+ y_point_offset),single->GetTrainInfo()->m_data.type,FALSE,m_nCountOfEngine);//single->m_carriage_count);//draw real time line

					}
					else
					{
						if(m_bStopStationWaveLine && JudgeNeedWaveLine(first,m_nSchdStatus,m_nDirectFlag_prefix))
							DrawStopStationLine(pCDC,CPoint(ptlStart.x +x_point_offset,ptlStart.y+y_point_offset),
							CPoint(ptlEnd.x+x_point_offset,ptlEnd.y+y_point_offset),UP_DIRECT);
						else
						{
							DrawActualLine(pCDC,CPoint(ptlStart.x + x_point_offset,ptlStart.y + y_point_offset),
								CPoint(ptlEnd.x+x_point_offset,ptlEnd.y+ y_point_offset),single->GetTrainInfo()->m_data.type,FALSE,m_nCountOfEngine,0,FALSE);
						}
					}
				}
				//point rect information
				bool temp = IsFlagTerminal(first->m_data.flag);
				if(!(second == NULL && IsFlagTerminal(first->m_data.flag) && first->m_data.arrive != first->m_data.depart))
				{//
					m_pPerPointInfo[m_nPointCount].record_index = i;
					if(first->m_data.arrive == first->m_data.depart)
						m_pPerPointInfo[m_nPointCount].start_depart_flag = UNION_POSITION;
					else
						m_pPerPointInfo[m_nPointCount].start_depart_flag = DEPART_POSITION;
					m_pPerPointInfo[m_nPointCount++].rect  = CRect(CPoint(ptlStart.x - uDis,ptlStart.y - uDis),sSize);
				}
				if(ptlStart.y != ptlEnd.y && !(second == NULL && IsFlagTerminal(first->m_data.flag)) )
				{
					m_pPerPointInfo[m_nPointCount].record_index = i;
					if(first->m_data.arrive == first->m_data.depart)
						m_pPerPointInfo[m_nPointCount].start_depart_flag = UNION_POSITION;
					else
						m_pPerPointInfo[m_nPointCount].start_depart_flag = DEPART_POSITION;
					m_pPerPointInfo[m_nPointCount++].rect  = CRect(CPoint(ptlEnd.x - uDis,ptlEnd.y - uDis),sSize);	
				}
			}
			//for record exit position != entry position 
			if(ptlEnd.y != first_top_exit && second != NULL)
			{
				ptlEnd.y = first_top_exit;
				m_pPerPointInfo[m_nPointCount].record_index = i;
				if(first->m_data.arrive == first->m_data.depart)
					m_pPerPointInfo[m_nPointCount].start_depart_flag = UNION_POSITION;
				else
					m_pPerPointInfo[m_nPointCount].start_depart_flag = DEPART_POSITION;
				m_pPerPointInfo[m_nPointCount++].rect  = CRect(CPoint(ptlEnd.x - uDis,ptlEnd.y - uDis),sSize);	
			}

			if(second != NULL)
				ptlStart = ptlEnd;
		}

		// draw depart&terminal minute text	
		time_depart = minute_time(first->m_data.depart,&date_depart);
		if(time_depart==NOTIME) break;
		itoa(time_depart/60 % 10, time_char, 10);
				
		bool bTrainLeapLineDrawDepart=true;
		if(m_nTrainLeapLineFlag==3)
		{
			int index1 = first->GetArriveTGIndex();
			int index2 = first->GetDepartTGIndex();
			if(index1!=index2 && index1>=0 && index1<128 && index2>=0 && index2<128)
			{
				if(m_sDispDistance[index1].distance.node_info.show_sides==false && m_sDispDistance[index2].distance.node_info.show_sides)
				{
					bTrainLeapLineDrawDepart=false;
				}
			}
		}
        // 直接调用函数 PtInRects, 该函数定义在plotdoc.cpp/h中
		if(bTrainLeapLineDrawDepart && PtInRects(rangeRect, ptlStart) && gpcdef.GetFlagDrawTrainTime(first->m_data.station) && usStartTime != usEndTime)
		{
			CFont* pOldFont, font;
			LOGFONT stFont;
			memset(&stFont,0,sizeof(stFont));
			stFont.lfHeight = MulDiv(m_font.time_char,m_nLogPixelSy,72); // time_char
			stFont.lfWeight = FW_NORMAL;
			stFont.lfClipPrecision = CLIP_LH_ANGLES;
			strcpy(stFont.lfFaceName,"Times New Roman");
			font.CreateFontIndirect(&stFont);

			pOldFont = pCDC->SelectObject(&font);
			COLORREF Color = GetActualLineColor(factor);

			UINT nOldColor;
			if(manualRealColor>0 && TG_IsDepartActualByManual(first->m_data))
			{
				nOldColor = pCDC->SetTextColor(manualRealColor);
			}
			else
			{
				if(first_d_time_status == 1)
					nOldColor = pCDC->SetTextColor(Color);
			}
			
			CPoint point = ptlStart;
			if(point.x >=0)  //出发点的左边界值
			{
				if(nDirectFlag == 2)
				{
					point.y = first_bottom_entry;

					if(nDirectFlag_pre == DOWN_DIRECT)//下行车 
					{   
						pCDC->SetTextAlign(TA_RIGHT|TA_TOP);
					}
					else
					{//up direct
						pCDC->SetTextAlign(TA_RIGHT|TA_BOTTOM);
					}

					if(second == NULL && !IsFlagTerminal(first->m_data.flag))
					{// last, depart 
						point.y = first_bottom_exit;
						if(point.y >= 0)
							pCDC->TextOut(point.x+ x_point_offset, point.y+y_point_offset, time_char, 1);

					}
					else if(second == NULL && IsFlagTerminal(first->m_data.flag))
					{// last point,terminal 
						//不会运行到这里来,终到的车不必显示发车点
						if(usStartTime == usEndTime)
						{
							pCDC->SetTextAlign(TA_CENTER|TA_BOTTOM);
							point.y = first_primary_exit;
							if(point.y >= 0)
								pCDC->TextOut(point.x+ x_point_offset, point.y+y_point_offset, time_char, 1);
						}
					}
					else
					{ //not last ,depart  
						if(m_nTrainLeapLineFlag==3)
						{
							if(first_bottom_exit<first_top_entry)
							{
								point.y=first_top_entry;
								pCDC->SetTextAlign(TA_RIGHT|TA_BOTTOM);
							}
							else
							{
								point.y=first_bottom_entry;
								pCDC->SetTextAlign(TA_RIGHT|TA_TOP);
							}
						}
						else
						{
							point.y = first_bottom_entry;
						}
						if(point.y >= 0)
							pCDC->TextOut(point.x + x_point_offset, point.y+y_point_offset, time_char, 1);
					}
				} 
				else 
				{//up direct
					point.y = first_top_entry;

					if(nDirectFlag_pre == DOWN_DIRECT)//下行车 
					{   
						pCDC->SetTextAlign(TA_RIGHT|TA_TOP);
					}
					else
					{//up direct
						pCDC->SetTextAlign(TA_RIGHT|TA_BOTTOM);
					}

					if(second == NULL && !IsFlagTerminal(first->m_data.flag)  )
					{///last, depart 
						point.y = first_top_exit;
						if(point.y >= 0)
							pCDC->TextOut(point.x + x_point_offset, point.y + y_point_offset, time_char, 1);
					}
					else if(second == NULL && IsFlagTerminal(first->m_data.flag)  )
					{///last point,terminal 
						//不会运行到这里来,终到的车不必显示发车点
						if(usStartTime == usEndTime)
						{
							pCDC->SetTextAlign(TA_CENTER|TA_TOP);
							point.y = first_bottom_entry;
							if(point.y >= 0)
								pCDC->TextOut(point.x + x_point_offset, point.y + y_point_offset, time_char, 1);
						}
					}
					else  
					{//not last ,depart  
						if(m_nTrainLeapLineFlag==3)
						{
							if(first_bottom_exit<first_top_entry)
							{
								point.y=first_top_entry;
								pCDC->SetTextAlign(TA_RIGHT|TA_BOTTOM);
							}
							else
							{
								point.y=first_bottom_entry;
								pCDC->SetTextAlign(TA_RIGHT|TA_TOP);
							}
						}
						else
						{
							point.y = first_top_entry;
						}
						if(point.y >= 0)
							pCDC->TextOut(point.x + x_point_offset, point.y + y_point_offset, time_char, 1);
					}
				}
			}

			if(manualRealColor>0 && TG_IsDepartActualByManual(first->m_data))
			{
				pCDC->SetTextColor(nOldColor);
			}
			else
			{
				if(first_d_time_status == 1)
					pCDC->SetTextColor(nOldColor);
			}
			
			pCDC->SelectObject(pOldFont);   
			font.DeleteObject();
		}

		//这儿这个点为上行车在该车站的顶点，如果该车站为绘制接入交出标记的，
		//而且上截和下半截在图上的行车方向不一直，那么变换交出点的坐标值
		//2005-01-26
		if(nDirectFlag_pre != nDirectFlag_next && second != NULL && (m_nTrainLeapLineFlag==2||m_nTrainLeapLineFlag==1))
		{
			if(nDirectFlag_next == UP_DIRECT)
			{
				nDirectFlag = 1;//上行
			}
			else
			{
				nDirectFlag = 2;//下行
			}
		}

		if(ptlEnd.y == -1) continue;

		//draw stop abs lines
		if(IsFlagStopAbs(first->m_data.flag) || IsFlagStopSignal(first->m_data.flag))
		{
			time_depart = minute_time(first->m_data.depart,&date_depart);
			flSecondMinuteOffset = GetMinuteWidthToPlotStartTime(time_depart/60 ,date_depart,isPrint,m_nSchdStatus);			
			ptlEnd.x =(int)(flSecondMinuteOffset + first->m_data.special_time_1)  * m_fPixelPerMinute;

			if(IsFlagStopAbs(first->m_data.flag))
			{
				if(IsFlagAbsStart(first->m_data.flag) && i==0)
					ptlEnd.y = GetAbsVPos(single,first,2, EXIT_POSITION,IsFlagStopAbsUp(first->m_data.flag)?UP_DIRECT:DOWN_DIRECT,m_nSchdStatus);//will
				else
				    ptlEnd.y = GetAbsVPos(single,first,0, EXIT_POSITION,IsFlagStopAbsUp(first->m_data.flag)?UP_DIRECT:DOWN_DIRECT,m_nSchdStatus);//will
			}
			else 
			{
				ptlEnd.y = GetAbsVPos(single,first,1, EXIT_POSITION,IsFlagStopAbsUp(first->m_data.flag)?UP_DIRECT:DOWN_DIRECT,m_nSchdStatus);//will
			}

			int dlt = (ptlEnd.y - ptlStart.y)*2/3;
			if(IsAbsDrawUp(first->m_data.stop_reason))
			{
				ptlEnd.y = ptlStart.y + dlt;
			}
			else if(IsAbsDrawBottom(first->m_data.stop_reason))
			{
				ptlEnd.y = ptlStart.y + dlt*2;
			}

			if(ptlEnd.x < rangeRect.right && ptlEnd.x > 0)
			{
				if(first_d_time_status && second_a_time_status)
				{
					DrawActualLine(pCDC,CPoint(ptlStart.x + x_point_offset,ptlStart.y + y_point_offset),
						CPoint(ptlEnd.x+x_point_offset,ptlEnd.y+ y_point_offset),single->GetTrainInfo()->m_data.type,TRUE,m_nCountOfEngine);//single->m_carriage_count);//draw real time line

				}
				else
				{
					DrawActualLine(pCDC,CPoint(ptlStart.x + x_point_offset,ptlStart.y + y_point_offset),
						CPoint(ptlEnd.x+x_point_offset,ptlEnd.y+ y_point_offset),single->GetTrainInfo()->m_data.type,TRUE,m_nCountOfEngine,0,FALSE);//single->m_carriage_count);//draw real time line

				}
			}
			else if(ptlEnd.x > rangeRect.right && (ptlStart.x >= 0 && ptlStart.x < rangeRect.right))
			{
				ptlEnd.x = rangeRect.right;
				if(first_d_time_status && second_a_time_status)
				{
					DrawActualLine(pCDC,CPoint(ptlStart.x + x_point_offset,ptlStart.y + y_point_offset),
						CPoint(ptlEnd.x+x_point_offset,ptlEnd.y+ y_point_offset),single->GetTrainInfo()->m_data.type,TRUE,m_nCountOfEngine);//single->m_carriage_count);//draw real time line

				}
				else
				{
					DrawActualLine(pCDC,CPoint(ptlStart.x + x_point_offset,ptlStart.y + y_point_offset),
						CPoint(ptlEnd.x+x_point_offset,ptlEnd.y+ y_point_offset),single->GetTrainInfo()->m_data.type,TRUE,m_nCountOfEngine,0,FALSE);//single->m_carriage_count);//draw real time line


				}
				critical_point = ptlEnd;
				critical_point.Offset (x_point_offset,y_point_offset);
				//判断是否在该车站改了车次号，如果改了，使用新车次号

				NewTrainId = "";
				for(critial_record_index =0; critial_record_index<single->GetTrainRecordCount(); critial_record_index++)//查找在本站是否该了车次号
				{
					if(i == single->GetTrainRecord(critial_record_index)->m_data.station && single->GetTrainRecord(critial_record_index)->m_data.arrive_train_id != single->GetTrainRecord(critial_record_index)->m_data.depart_train_id
						&& strlen(single->GetTrainRecord(critial_record_index)->m_data.arrive_train_id)>0 && strlen(single->GetTrainRecord(critial_record_index)->m_data.depart_train_id)>0)
					{
						NewTrainId.Format("%s",single->GetTrainRecord(critial_record_index)->m_data.depart_train_id);
						break;
					}
				}
				//判断新车次号结束							
				if(NewTrainId.IsEmpty())
					NewTrainId = single->GetTrainID();

				if(first_a_time_status)
					DrawMarkOfCritical(pCDC,single,NewTrainId,critical_point,50,RIGHT_MARK, lActualColor);
				else							
					DrawMarkOfCritical(pCDC,single,NewTrainId,critical_point,50,RIGHT_MARK, lColor);

			}
			//			int nOldAlign = pCDC->SetTextAlign(TA_CENTER);

			//draw 区间停车，区间折返的第一个点时间 add by cf 
			if(rangeRect.PtInRect(ptlStart) && gpcdef.GetFlagDrawTrainTime(first->m_data.station))
			{
				long usTimeValue;
				usTimeValue = flSecondMinuteOffset + first->m_data.special_time_1;

				itoa((int)usTimeValue % 10, time_char, 10);

				CFont* pOldFont, font;
				LOGFONT stFont;
				memset(&stFont,0,sizeof(stFont));
				stFont.lfHeight = MulDiv(m_font.time_char,m_nLogPixelSy,72); // time_char
				stFont.lfWeight = FW_NORMAL;
				stFont.lfClipPrecision = CLIP_LH_ANGLES;
				strcpy(stFont.lfFaceName,"Times New Roman");
				font.CreateFontIndirect(&stFont);

				int text_align;

				pOldFont = pCDC->SelectObject(&font);

				COLORREF Color = GetActualLineColor(factor);

				UINT nOldColor;

				COLORREF nColor = lActualColor;
				if(first_a_time_status == 1)
				{
					nOldColor = pCDC->SetTextColor(Color);
					nColor = lActualColor;
				}
				else
				{
					nColor = lColor;
				}

				CPoint point;
				point = ptlEnd;
				int direct_flag = DOWN_MARK;
				if(nDirectFlag  == 2) //下行
				{
					text_align = pCDC->SetTextAlign(TA_LEFT|TA_BOTTOM);
					direct_flag = DOWN_MARK;
				}
				else
				{
					text_align = pCDC->SetTextAlign(TA_LEFT|TA_TOP);
					direct_flag = UP_MARK;
				}			

				if(point.x >0 && point.x < rangeRect.right)				
				{
					pCDC->TextOut((int)point.x + x_point_offset, point.y + y_point_offset, time_char, 1); 


					//if(usDrawCount == 2)
					{
						CPoint abspt(point.x + x_point_offset, point.y + y_point_offset);
						CTG_TrainRecord* next = single->GetTrainRecord(i+1);
						if(NULL != first && NULL != next && (first->m_data.station == next->m_data.station))
						{
							if(first_a_time_status)
								DrawMarkOfEnd(pCDC, single, single->GetTrainID(), abspt,20,direct_flag, nColor, first->m_data.flag,true);
							else
								DrawMarkOfEnd(pCDC, single, single->GetTrainID(), abspt,20,direct_flag, nColor, first->m_data.flag,false);
						}
					}

				}
				pCDC->SetTextAlign(text_align);
				if(first_a_time_status == 1)
					pCDC->SetTextColor(nOldColor);
				pCDC->SelectObject(pOldFont);  

				font.DeleteObject();
			}
			//end

			ptlStart = ptlEnd;
			//point rect information
			m_pPerPointInfo[m_nPointCount].record_index = i;
			m_pPerPointInfo[m_nPointCount].start_depart_flag = ABS_ARRIVAL_POSITION;
			m_pPerPointInfo[m_nPointCount++].rect  = CRect(CPoint(ptlStart.x - uDis,ptlStart.y - uDis),sSize);
			//2 绘制从区间停车第一个点第二个点的连线
			ptlEnd.x =(int)(flSecondMinuteOffset + first->m_data.special_time_2)  * m_fPixelPerMinute;
			
			if(ptlStart.x < 0 && ptlEnd.x >=0)
			{
				ptlStart.x = 0;
				critical_point = ptlStart;
				critical_point.Offset (x_point_offset,y_point_offset);

				//判断是否在该车站改了车次号，如果改了，使用新车次号
				NewTrainId = "";
				for(critial_record_index =0; critial_record_index<single->GetTrainRecordCount(); critial_record_index++)//查找在本站是否该了车次号
				{
					if(i == single->GetTrainRecord(critial_record_index)->m_data.station && single->GetTrainRecord(critial_record_index)->m_data.arrive_train_id != single->GetTrainRecord(critial_record_index)->m_data.depart_train_id
						&& strlen(single->GetTrainRecord(critial_record_index)->m_data.arrive_train_id)>0 && strlen(single->GetTrainRecord(critial_record_index)->m_data.depart_train_id)>0)
					{
						NewTrainId.Format("%s",single->GetTrainRecord(critial_record_index)->m_data.depart_train_id);
						break;
					}
				}
				//判断新车次号结束							
				if(NewTrainId.IsEmpty())
					NewTrainId = single->GetTrainID();

				if(first_a_time_status)
					DrawMarkOfCritical(pCDC,single,NewTrainId,critical_point,50,LEFT_MARK, lActualColor);
				else							
					DrawMarkOfCritical(pCDC,single,NewTrainId,critical_point,50,LEFT_MARK, lColor);

			}

			bool bDrawFlag = TRUE;
			if((ptlEnd.x > 0 && ptlEnd.x < rangeRect.right) &&
				(ptlStart.x > 0 && ptlStart.x < rangeRect.right))
			{

			}
			else if(ptlEnd.x < 0  && ptlStart.x < 0)
			{
				//do nothing 
				bDrawFlag = FALSE;
			}
			else if( ptlEnd.x > rangeRect.right && ptlStart.x >= rangeRect.right)
			{
				//do nothing 
				bDrawFlag = FALSE;
			}
			else if((ptlEnd.x > 0 && ptlEnd.x < rangeRect.right) &&
				ptlStart.x < 0)
			{
				ptlStart.x = 0;
				critical_point = ptlStart;
				critical_point.Offset (x_point_offset,y_point_offset);

				//判断是否在该车站改了车次号，如果改了，使用新车次号
				NewTrainId = "";
				for(critial_record_index =0; critial_record_index<single->GetTrainRecordCount(); critial_record_index++)//查找在本站是否该了车次号
				{
					if(i == single->GetTrainRecord(critial_record_index)->m_data.station && single->GetTrainRecord(critial_record_index)->m_data.arrive_train_id != single->GetTrainRecord(critial_record_index)->m_data.depart_train_id
						&& strlen(single->GetTrainRecord(critial_record_index)->m_data.arrive_train_id)>0 && strlen(single->GetTrainRecord(critial_record_index)->m_data.depart_train_id)>0)
					{
						NewTrainId.Format("%s",single->GetTrainRecord(critial_record_index)->m_data.depart_train_id);
						break;
					}
				}
				//判断新车次号结束							
				if(NewTrainId.IsEmpty())
					NewTrainId = single->GetTrainID();

				if(first_a_time_status)
					DrawMarkOfCritical(pCDC,single,NewTrainId,critical_point,50,LEFT_MARK, lActualColor);
				else							
					DrawMarkOfCritical(pCDC,single,NewTrainId,critical_point,50,LEFT_MARK, lColor);


			}
			else if( ptlEnd.x > rangeRect.right &&	ptlStart.x < 0)
			{
				ptlStart.x = 0;
				ptlEnd.x =  rangeRect.right;

				critical_point = ptlStart;
				critical_point.Offset (x_point_offset,y_point_offset);
				//判断是否在该车站改了车次号，如果改了，使用新车次号
				NewTrainId = "";
				for(critial_record_index =0; critial_record_index<single->GetTrainRecordCount(); critial_record_index++)//查找在本站是否该了车次号
				{
					if(i == single->GetTrainRecord(critial_record_index)->m_data.station && single->GetTrainRecord(critial_record_index)->m_data.arrive_train_id != single->GetTrainRecord(critial_record_index)->m_data.depart_train_id
						&& strlen(single->GetTrainRecord(critial_record_index)->m_data.arrive_train_id)>0 && strlen(single->GetTrainRecord(critial_record_index)->m_data.depart_train_id)>0)
					{
						NewTrainId.Format("%s",single->GetTrainRecord(critial_record_index)->m_data.depart_train_id);
						break;
					}
				}
				//判断新车次号结束							
				if(NewTrainId.IsEmpty())
					NewTrainId =single->GetTrainID();

				if(first_a_time_status)
					DrawMarkOfCritical(pCDC,single,NewTrainId,critical_point,50,LEFT_MARK, lActualColor);
				else							
					DrawMarkOfCritical(pCDC,single,NewTrainId,critical_point,50,LEFT_MARK, lColor);

				critical_point = ptlEnd;
				critical_point.Offset (x_point_offset,y_point_offset);

				//判断是否在该车站改了车次号，如果改了，使用新车次号
				NewTrainId = "";
				for(critial_record_index =0; critial_record_index<single->GetTrainRecordCount(); critial_record_index++)//查找在本站是否该了车次号
				{
					if(i == single->GetTrainRecord(critial_record_index)->m_data.station && single->GetTrainRecord(critial_record_index)->m_data.arrive_train_id != single->GetTrainRecord(critial_record_index)->m_data.depart_train_id
						&& strlen(single->GetTrainRecord(critial_record_index)->m_data.arrive_train_id)>0 && strlen(single->GetTrainRecord(critial_record_index)->m_data.depart_train_id)>0)
					{
						NewTrainId.Format("%s",single->GetTrainRecord(critial_record_index)->m_data.depart_train_id);
						break;
					}
				}
				//判断新车次号结束							
				if(NewTrainId.IsEmpty())
					NewTrainId = single->GetTrainID();

				if(first_a_time_status)
					DrawMarkOfCritical(pCDC,single,NewTrainId,critical_point,50,RIGHT_MARK, lActualColor);
				else							
					DrawMarkOfCritical(pCDC,single,NewTrainId,critical_point,50,RIGHT_MARK, lColor);

			}
			else if(ptlEnd.x > rangeRect.right && ptlStart.x < rangeRect.right)
			{
				ptlEnd.x =  rangeRect.right;

				critical_point = ptlEnd;
				critical_point.Offset (x_point_offset,y_point_offset);
				//判断是否在该车站改了车次号，如果改了，使用新车次号
				NewTrainId = "";
				for(critial_record_index =0; critial_record_index<single->GetTrainRecordCount(); critial_record_index++)//查找在本站是否该了车次号
				{
					if(i == single->GetTrainRecord(critial_record_index)->m_data.station && single->GetTrainRecord(critial_record_index)->m_data.arrive_train_id != single->GetTrainRecord(critial_record_index)->m_data.depart_train_id
						&& strlen(single->GetTrainRecord(critial_record_index)->m_data.arrive_train_id)>0 && strlen(single->GetTrainRecord(critial_record_index)->m_data.depart_train_id)>0)
					{
						NewTrainId.Format("%s",single->GetTrainRecord(critial_record_index)->m_data.depart_train_id);
						break;
					}
				}
				//判断新车次号结束							
				if(NewTrainId.IsEmpty())
					NewTrainId = single->GetTrainID();

				if(first_a_time_status)
					DrawMarkOfCritical(pCDC,single,NewTrainId,critical_point,50,RIGHT_MARK, lActualColor);
				else							
					DrawMarkOfCritical(pCDC,single,NewTrainId,critical_point,50,RIGHT_MARK, lColor);


			}
			//draw line
			if(bDrawFlag)
			{
				if(first_d_time_status && second_a_time_status)
				{
					DrawActualLine(pCDC,CPoint(ptlStart.x + x_point_offset,ptlStart.y + y_point_offset),
						CPoint(ptlEnd.x+x_point_offset,ptlEnd.y+ y_point_offset),single->GetTrainInfo()->m_data.type,FALSE,m_nCountOfEngine);//single->m_carriage_count);//draw real time line

				}
				else
				{
					DrawActualLine(pCDC,CPoint(ptlStart.x + x_point_offset,ptlStart.y + y_point_offset),
						CPoint(ptlEnd.x+x_point_offset,ptlEnd.y+ y_point_offset),single->GetTrainInfo()->m_data.type,FALSE,m_nCountOfEngine,0,FALSE);//single->m_carriage_count);//draw real time line
				}
				
				CPoint mid;
				mid.x = (ptlStart.x + ptlEnd.x)/2 + x_point_offset;
				mid.y = (ptlStart.y + ptlEnd.y)/2 + y_point_offset;

				CFont mFont,*pOldFont;
				LOGFONT stFont;

				memset(&stFont,0,sizeof(stFont));
				stFont.lfHeight = MulDiv(m_font.station_text,-m_nLogPixelSy,72);
				stFont.lfWeight = FW_THIN;
				stFont.lfClipPrecision = CLIP_LH_ANGLES;
				strcpy(stFont.lfFaceName,"Arial");
				mFont.CreateFontIndirect(&stFont);	
				pOldFont = pCDC->SelectObject(&mFont);			
				COLORREF cl = pCDC->SetTextColor(RGB(255,0,0));
				UINT alg = pCDC->SetTextAlign(TA_CENTER);
				CString strAbsText = first->GetAbsText();
				CSize sz = pCDC->GetTextExtent(strAbsText);
				// 上行
				if( first_primary_exit > second_primary_entry)
				{
					alg = pCDC->SetTextAlign(TA_TOP|TA_CENTER);
					pCDC->TextOut(mid.x, mid.y-sz.cy, strAbsText);
				}
				else
				{
					alg = pCDC->SetTextAlign(TA_BOTTOM|TA_CENTER);
					pCDC->TextOut(mid.x, mid.y+sz.cy, strAbsText);
				}

				
				pCDC->SetTextAlign(alg);
				pCDC->SelectObject(pOldFont);

				//draw triangle for stop abs sigal
				if(IsFlagStopSignal(first->m_data.flag))
				{
					CPen mMyPen;
					mMyPen.CreatePen(PS_SOLID, scaleRate, RGB(255,0,0));
					pOldPen = pCDC->SelectObject(&mMyPen);

					int delta = 20;
					CPoint ptl[4];
					
					ptl[0].x = (ptlStart.x + ptlEnd.x) /2;
					ptl[0].y = ptlStart.y;
					ptl[1].x = (ptlStart.x + ptlEnd.x) /2 + delta/2;
					ptl[1].y = ptlStart.y + delta;
					ptl[2].x = (ptlStart.x + ptlEnd.x) /2 - delta/2;
					ptl[2].y = ptlStart.y + delta;
					ptl[3].x = (ptlStart.x + ptlEnd.x) /2;
					ptl[3].y = ptlStart.y;

					for(int i=0;i<4;i++)
						ptl[i].Offset (x_point_offset,y_point_offset);

					if( first_primary_exit < second_primary_entry)
					{
						for(int i=0;i<4;i++)
							ptl[i].Offset (0,-delta-1);
					}
					
					pCDC->Polyline(ptl,4);

					CFont mFont,*pOldFont;
					LOGFONT stFont;

					memset(&stFont,0,sizeof(stFont));
					stFont.lfHeight = MulDiv(8,-m_nLogPixelSy,72);
					stFont.lfWeight = FW_THIN;
					stFont.lfClipPrecision = CLIP_LH_ANGLES;
					strcpy(stFont.lfFaceName,"Arial");
					mFont.CreateFontIndirect(&stFont);	
					pOldFont = pCDC->SelectObject(&mFont);	

					CString stopTime;
					stopTime.Format("%d", first->m_data.special_time_2-first->m_data.special_time_1);
					UINT alg = pCDC->SetTextAlign(TA_CENTER|TA_TOP);
					pCDC->TextOut(ptl[0].x, ptl[0].y+delta/3, stopTime);
					pCDC->SetTextAlign(alg);
					pCDC->SelectObject(pOldFont);
					pCDC->SelectObject(pOldPen);
					mMyPen.DeleteObject();
				}
				pCDC->SetTextColor(cl);
			}

			//draw 区间停车，区间折返的第二个点时间 add by cf 
			if(rangeRect.PtInRect(ptlStart) && gpcdef.GetFlagDrawTrainTime(first->m_data.station))
			{
				long usTimeValue;
				usTimeValue = flSecondMinuteOffset + first->m_data.special_time_2;
				//ConvertXPostoTime(ptlEnd.x,&usDateValue,&usTimeValue);
				itoa((int)usTimeValue % 10, time_char, 10);
				//********************************************************************************	

				CFont* pOldFont, font;
				LOGFONT stFont;
				memset(&stFont,0,sizeof(stFont));
				stFont.lfHeight = MulDiv(m_font.time_char-1,m_nLogPixelSy,72); // time_char
				stFont.lfWeight = FW_NORMAL;
				stFont.lfClipPrecision = CLIP_LH_ANGLES;
				strcpy(stFont.lfFaceName,"Times New Roman");
				font.CreateFontIndirect(&stFont);

				int text_align;

				pOldFont = pCDC->SelectObject(&font);

				COLORREF Color = GetActualLineColor(factor);

				UINT nOldColor;
				COLORREF ncolor;
				if(first_a_time_status == 1)
				{
					nOldColor = pCDC->SetTextColor(Color);
					ncolor=Color;
				}
				else 
				{
					ncolor=lColor;
				}
				CPoint point;
				point.x = ptlEnd.x;
				point.y = ptlEnd.y;
				//******************************************************************************
				point.x = ptlEnd.x ;//+ stFont.lfHeight + 3;
				int nflag=UP_DIRECT;

				if(nDirectFlag == 2) //下行
				{
					text_align = pCDC->SetTextAlign(TA_RIGHT|TA_TOP);
					nflag=UP_DIRECT;
				}
				else
				{
					text_align = pCDC->SetTextAlign(TA_RIGHT|TA_BOTTOM);
					nflag=DOWN_DIRECT;
				}


				if(point.x >0 && point.x < rangeRect.right)		
				{
					if(first->m_data.special_time_1 != first->m_data.special_time_2)
					{
						pCDC->TextOut((int)point.x + x_point_offset, point.y + y_point_offset, time_char, 1);  
						CPoint abspt(point.x + x_point_offset, point.y + y_point_offset);
						//if(NULL !=second && (usDrawCount == 2) &&(first->m_data.station == second->m_data.station))
						if(NULL !=second &&(first->m_data.station == second->m_data.station))
							this->DrawMarkOfStart(pCDC,single,second->GetArriveTID(),abspt,20,nflag,ncolor,first_a_time_status == 1,schd_type);
					}
				}

				pCDC->SetTextAlign(text_align);
				if(first_a_time_status == 1)
					pCDC->SetTextColor(nOldColor);

				pCDC->SelectObject(pOldFont);   

				font.DeleteObject();
			}
			
			ptlStart = ptlEnd;        
			//point rect information
			m_pPerPointInfo[m_nPointCount].record_index = i;
			m_pPerPointInfo[m_nPointCount].start_depart_flag = ABS_DEPART_POSITION;
			m_pPerPointInfo[m_nPointCount++].rect  = CRect(CPoint(ptlStart.x - uDis,ptlStart.y - uDis),sSize);

		}
		//end 
		if(second==NULL)  break; 
		long time_arrival,date_arrival;
		time_arrival = minute_time(second->m_data.arrive,&date_arrival);
		flSecondMinuteOffset = GetMinuteWidthToPlotStartTime(time_arrival/60,date_arrival,isPrint,m_nSchdStatus);

		if(IsFlagAnLocReturn(second->m_data.flag))
			nDirectFlag = (nDirectFlag==2)?1:2;

		//DOWN_DIRECT 
		if(nDirectFlag == 2)
		{
			ptlEnd.x = (int)(flSecondMinuteOffset * m_fPixelPerMinute);
			ptlEnd.y = second_top_entry;
			if(ptlEnd.y == -1) continue;
			//6:00 or 18:00 position	
			if((ptlStart.x < 0 && ptlEnd.x < 0 ))
			{
				continue;
			}
			else if ((ptlStart.x < 0 && ptlEnd.x >= 0 )) 
			{
				ptlStart.y = ptlEnd.y - (double)ptlEnd.x / (double)(ptlEnd.x - ptlStart.x )*(double)abs(ptlEnd.y - ptlStart.y);
				ptlStart.x = 0;
				critical_point = ptlStart;
				critical_point.Offset (x_point_offset,y_point_offset);
				//判断是否在该车站改了车次号，如果改了，使用新车次号
				NewTrainId = "";
				for(critial_record_index =0; critial_record_index<single->GetTrainRecordCount(); critial_record_index++)//查找在本站是否该了车次号
				{
					if(i == single->GetTrainRecord(critial_record_index)->m_data.station && single->GetTrainRecord(critial_record_index)->m_data.arrive_train_id != single->GetTrainRecord(critial_record_index)->m_data.depart_train_id
						&& strlen(single->GetTrainRecord(critial_record_index)->m_data.arrive_train_id)>0 && strlen(single->GetTrainRecord(critial_record_index)->m_data.depart_train_id)>0)
					{
						NewTrainId.Format("%s",single->GetTrainRecord(critial_record_index)->m_data.depart_train_id);
						break;
					}
				}
				//判断新车次号结束							
				if(NewTrainId.IsEmpty())
					NewTrainId = single->GetTrainID();

				if(first_a_time_status)
					DrawMarkOfCritical(pCDC,single,NewTrainId,critical_point,50,LEFT_MARK, lActualColor);
				else							
					DrawMarkOfCritical(pCDC,single,NewTrainId,critical_point,50,LEFT_MARK, lColor);

			}
			if((ptlStart.x > rangeRect.right) && ptlEnd.x > rangeRect.right)
			{
			}
			else if((ptlStart.x < rangeRect.right) && ptlEnd.x >= rangeRect.right)
			{
				ptlEnd.y = ptlEnd.y - (double)(ptlEnd.x - rangeRect.right) / (double)(ptlEnd.x - ptlStart.x )*(double)abs(ptlEnd.y - ptlStart.y);
				ptlEnd.x = rangeRect.right;
				critical_point = ptlEnd;
				critical_point.Offset (x_point_offset,y_point_offset);
				//判断是否在该车站改了车次号，如果改了，使用新车次号
				NewTrainId = "";
				for(critial_record_index =0; critial_record_index<single->GetTrainRecordCount(); critial_record_index++)//查找在本站是否该了车次号
				{
					if(i == single->GetTrainRecord(critial_record_index)->m_data.station && single->GetTrainRecord(critial_record_index)->m_data.arrive_train_id != single->GetTrainRecord(critial_record_index)->m_data.depart_train_id
						&& strlen(single->GetTrainRecord(critial_record_index)->m_data.arrive_train_id)>0 && strlen(single->GetTrainRecord(critial_record_index)->m_data.depart_train_id)>0)
					{
						NewTrainId.Format("%s",single->GetTrainRecord(critial_record_index)->m_data.depart_train_id);
						break;
					}
				}
				//判断新车次号结束							
				if(NewTrainId.IsEmpty())
					NewTrainId = single->GetTrainID();

				if(first_a_time_status)
					DrawMarkOfCritical(pCDC,single,NewTrainId,critical_point,50,RIGHT_MARK, lActualColor);
				else							
					DrawMarkOfCritical(pCDC,single,NewTrainId,critical_point,50,RIGHT_MARK, lColor);
			}

			if((m_nSchdStatus == WORK_SCHEDULE|| m_nSchdStatus == SHIFT_SCHEDULE ||m_nSchdStatus == HISTORY_SCHEDULE) == TRUE? TRUE:(ptlEnd.x >= ptlStart.x))
			{	
				if(ptlEnd.x <= rangeRect.right)
				{
					if(!IsFlagUnitOtherTrain(first->m_data.flag)&& !IsFlagEnd(first->m_data.flag))
					{
						int index2 = first->GetDepartTGIndex();
						if(index2>=0 && index2<128 && m_sDispDistance[index2].distance.node_info.show_sides)
						{
							if(first_bottom_exit < second_top_entry)
							{
								ptlStart.y = first_bottom_exit;
							}
							else
							{
								ptlStart.y = first_top_exit;
							}
						}

						if(first_d_time_status && second_a_time_status)
						{
							DrawActualLine(pCDC,CPoint(ptlStart.x + x_point_offset,ptlStart.y + y_point_offset),
								CPoint(ptlEnd.x+x_point_offset,ptlEnd.y+ y_point_offset),single->GetTrainInfo()->m_data.type,TRUE,m_nCountOfEngine);//single->m_carriage_count);//draw real time line
						}
						else
						{
							DrawActualLine(pCDC,CPoint(ptlStart.x + x_point_offset,ptlStart.y + y_point_offset),
								CPoint(ptlEnd.x+x_point_offset,ptlEnd.y+ y_point_offset),single->GetTrainInfo()->m_data.type,TRUE,m_nCountOfEngine,0,FALSE);
						}
					}

					if(exception_Mark)
					{
						DrawBadMark(pCDC,CPoint(ptlStart.x + x_point_offset,ptlStart.y + y_point_offset),
							CPoint(ptlEnd.x+x_point_offset,ptlEnd.y+ y_point_offset),EXCEPTION_MARK,single->GetTrainID());
					}
					if(bad_Mark)
					{
						DrawBadMark(pCDC,CPoint(ptlStart.x + x_point_offset,ptlStart.y + y_point_offset),
							CPoint(ptlEnd.x+x_point_offset,ptlEnd.y+ y_point_offset),BAD_MARK,single->GetTrainID());
					}
					if(reverse_Mark)
					{
						DrawBadMark(pCDC,CPoint(ptlStart.x + x_point_offset,ptlStart.y + y_point_offset),
							CPoint(ptlEnd.x+x_point_offset,ptlEnd.y+ y_point_offset),REVERSE_MARK,single->GetTrainID());
					}
					if(stopReason_Mark)
					{
						DrawBadMark(pCDC,CPoint(ptlStart.x + x_point_offset,ptlStart.y + y_point_offset),
							CPoint(ptlEnd.x+x_point_offset,ptlEnd.y+ y_point_offset),STOPREASTON_MARK,single->GetTrainID());
					}

					if(m_bShowFastOrSlowTime)
					{
						if(!IsFlagStopAbs(first->m_data.flag) && !IsFlagStopSignal(first->m_data.flag))
						{
							DrawFastOrSlowTime(pCDC,CPoint(ptlStart.x + x_point_offset,ptlStart.y + y_point_offset),
								CPoint(ptlEnd.x+x_point_offset,ptlEnd.y+ y_point_offset),IsRunFast_Plot(first),GetFastMinute_Plot(first),nDirectFlag);
						}
					}
				}
			}
			else
			{
				//基本图的过表
				if(second_a_time_status )
				{ ////m_nPageWidth	  
					CPoint criticalstart,criticalend;

					int basicMemoryWidth = m_fPixelPerHour*24;
					criticalstart = CPoint(basicMemoryWidth + x_point_offset,ptlStart.y + abs(ptlEnd.y - ptlStart.y)*(basicMemoryWidth - ptlStart.x )/(basicMemoryWidth - ptlStart.x + ptlEnd.x) + y_point_offset);
					criticalend = CPoint(0 + x_point_offset,ptlStart.y + abs(ptlEnd.y - ptlStart.y)*(basicMemoryWidth - ptlStart.x )/(basicMemoryWidth - ptlStart.x + ptlEnd.x) + y_point_offset);

					DrawActualLine(pCDC,CPoint(ptlStart.x + x_point_offset,ptlStart.y + y_point_offset)
						,criticalstart,single->GetTrainInfo()->m_data.type,FALSE,m_nCountOfEngine);//single->m_carriage_count);
					DrawActualLine(pCDC,criticalend,
						CPoint(ptlEnd.x+x_point_offset,ptlEnd.y+ y_point_offset),single->GetTrainInfo()->m_data.type,FALSE,m_nCountOfEngine);//single->m_carriage_count);//draw real time line

					DrawMarkOfCritical(pCDC,single,single->GetTrainID(),criticalstart,50,RIGHT_MARK, lActualColor);

					DrawMarkOfCritical(pCDC,single,single->GetTrainID(),criticalend,50,LEFT_MARK, lActualColor);
					/////ptlStart.x
				}
				else
				{////m_nPageWidth
					CPoint criticalstart,criticalend;

					int basicMemoryWidth = m_fPixelPerHour*24;
					criticalstart = CPoint(basicMemoryWidth + x_point_offset,ptlStart.y + abs(ptlEnd.y - ptlStart.y)*(basicMemoryWidth - ptlStart.x )/(basicMemoryWidth - ptlStart.x + ptlEnd.x) + y_point_offset);
					criticalend = CPoint(0 + x_point_offset,ptlStart.y + abs(ptlEnd.y - ptlStart.y)*(basicMemoryWidth - ptlStart.x )/(basicMemoryWidth - ptlStart.x + ptlEnd.x) + y_point_offset);

					DrawActualLine(pCDC,CPoint(ptlStart.x + x_point_offset,ptlStart.y + y_point_offset)
						,criticalstart,single->GetTrainInfo()->m_data.type,FALSE,m_nCountOfEngine,0,FALSE);//single->m_carriage_count);
					DrawActualLine(pCDC,criticalend,
						CPoint(ptlEnd.x+x_point_offset,ptlEnd.y+ y_point_offset),single->GetTrainInfo()->m_data.type,FALSE,m_nCountOfEngine,0,FALSE);
				}
			}
			ptlStart = ptlEnd;
			ptlEnd.y = second_primary_entry;
			if(ptlEnd.y == -1) continue;
			if(ptlEnd.x < rangeRect.right)
			{
				if(!IsFlagUnitOtherTrain(first->m_data.flag)&& !IsFlagEnd(first->m_data.flag))
				{
					// 绘制车站到点到股道竖线
					if(first_d_time_status && second_a_time_status )
					{
						DrawActualLine(pCDC,CPoint(ptlStart.x + x_point_offset,ptlStart.y + y_point_offset),//////////////////////////////
							CPoint(ptlEnd.x+x_point_offset,ptlEnd.y+ y_point_offset),single->GetTrainInfo()->m_data.type,FALSE,m_nCountOfEngine);//single->m_carriage_count);//draw real time line
					}
					else
					{
						DrawActualLine(pCDC,CPoint(ptlStart.x + x_point_offset,ptlStart.y + y_point_offset),//////////////////////////////
							CPoint(ptlEnd.x+x_point_offset,ptlEnd.y+ y_point_offset),single->GetTrainInfo()->m_data.type,FALSE,m_nCountOfEngine,0,FALSE);//single->m_carriage_count);//draw real time line
					}
				}
			}
			if( ptlStart.y != ptlEnd.y)
			{
				//point rect information
				m_pPerPointInfo[m_nPointCount].record_index = i + 1;
				m_pPerPointInfo[m_nPointCount].start_depart_flag = ARRIVAL_POSITION;
				m_pPerPointInfo[m_nPointCount++].rect  = CRect(CPoint(ptlStart.x - uDis,ptlStart.y - uDis),sSize);
			}

			ptlStart = ptlEnd;
		}
		else
		{//UP_DIRECT
			ptlEnd.x =(int)(flSecondMinuteOffset * m_fPixelPerMinute);
			ptlEnd.y = second_bottom_entry;
			if(ptlEnd.y == -1) continue;
			//6:00 or 18:00 position	
			if((ptlStart.x < 0 && ptlEnd.x < 0 ))
			{
				continue;
			}
			else if ((ptlStart.x < 0 && ptlEnd.x >= 0 )) 
			{
				ptlStart.y = ptlEnd.y + (double)abs(ptlEnd.y - ptlStart.y)*(double)ptlEnd.x / (double)(ptlEnd.x - ptlStart.x );
				ptlStart.x = 0;
				critical_point = ptlStart;
				critical_point.Offset (x_point_offset,y_point_offset);
				//判断是否在该车站改了车次号，如果改了，使用新车次号
				NewTrainId = "";
				for(critial_record_index =0; critial_record_index<single->GetTrainRecordCount(); critial_record_index++)//查找在本站是否该了车次号
				{
					if(i == single->GetTrainRecord(critial_record_index)->m_data.station && single->GetTrainRecord(critial_record_index)->m_data.arrive_train_id != single->GetTrainRecord(critial_record_index)->m_data.depart_train_id
						&& strlen(single->GetTrainRecord(critial_record_index)->m_data.arrive_train_id)>0 && strlen(single->GetTrainRecord(critial_record_index)->m_data.depart_train_id)>0)
					{
						NewTrainId.Format("%s",single->GetTrainRecord(critial_record_index)->m_data.depart_train_id);
						break;
					}
				}
				//判断新车次号结束							
				if(NewTrainId.IsEmpty())
					NewTrainId = single->GetTrainID();

				if(first_a_time_status)
					DrawMarkOfCritical(pCDC,single,NewTrainId,critical_point,50,LEFT_MARK, lActualColor);
				else							
					DrawMarkOfCritical(pCDC,single,NewTrainId,critical_point,50,LEFT_MARK, lColor);

			}
			if((ptlStart.x > rangeRect.right) && ptlEnd.x >= rangeRect.right)
			{
				continue;
			}
			else if((ptlStart.x < rangeRect.right) && ptlEnd.x >= rangeRect.right)
			{
				ptlEnd.y = ptlEnd.y - (double)(ptlEnd.y - ptlStart.y)*(double)(ptlEnd.x - rangeRect.right) / (double)(ptlEnd.x - ptlStart.x );
				ptlEnd.x = rangeRect.right;
				critical_point = ptlEnd;
				critical_point.Offset (x_point_offset,y_point_offset);
				//判断是否在该车站改了车次号，如果改了，使用新车次号
				NewTrainId = "";
				for(critial_record_index =0; critial_record_index<single->GetTrainRecordCount(); critial_record_index++)//查找在本站是否该了车次号
				{
					if(i == single->GetTrainRecord(critial_record_index)->m_data.station && single->GetTrainRecord(critial_record_index)->m_data.arrive_train_id != single->GetTrainRecord(critial_record_index)->m_data.depart_train_id
						&& strlen(single->GetTrainRecord(critial_record_index)->m_data.arrive_train_id)>0 && strlen(single->GetTrainRecord(critial_record_index)->m_data.depart_train_id)>0)
					{
						NewTrainId.Format("%s",single->GetTrainRecord(critial_record_index)->m_data.depart_train_id);
						break;
					}
				}
				//判断新车次号结束							
				if(NewTrainId.IsEmpty())
					NewTrainId = single->GetTrainID();

				if(first_a_time_status)
					DrawMarkOfCritical(pCDC,single,NewTrainId,critical_point,50,RIGHT_MARK, lActualColor);
				else							
					DrawMarkOfCritical(pCDC,single,NewTrainId,critical_point,50,RIGHT_MARK, lColor);

			}

			if((m_nSchdStatus == WORK_SCHEDULE || m_nSchdStatus == SHIFT_SCHEDULE||m_nSchdStatus == HISTORY_SCHEDULE) == TRUE? TRUE:(ptlEnd.x >= ptlStart.x))
			{				
				if(ptlEnd.x <= rangeRect.right)
				{
					if(!IsFlagUnitOtherTrain(first->m_data.flag)&& !IsFlagEnd(first->m_data.flag))
					{
						int index2 = first->GetDepartTGIndex();
						if(index2>=0 && index2<128 && m_sDispDistance[index2].distance.node_info.show_sides)
						{
							if(first_bottom_exit < second_top_entry)
							{
								ptlStart.y = first_bottom_exit;
							}
							else
							{
								ptlStart.y = first_top_exit;
							}
						}

						if(first_d_time_status && second_a_time_status)
						{
							DrawActualLine(pCDC,CPoint(ptlStart.x + x_point_offset,ptlStart.y + y_point_offset),
								CPoint(ptlEnd.x+x_point_offset,ptlEnd.y+ y_point_offset),single->GetTrainInfo()->m_data.type,TRUE,m_nCountOfEngine);//single->m_carriage_count);//draw real time line

						}
						else
						{
							DrawActualLine(pCDC,CPoint(ptlStart.x + x_point_offset,ptlStart.y + y_point_offset),
								CPoint(ptlEnd.x+x_point_offset,ptlEnd.y+ y_point_offset),single->GetTrainInfo()->m_data.type,TRUE,m_nCountOfEngine,0,FALSE);
						}
					}

					if(exception_Mark)
					{
						DrawBadMark(pCDC,CPoint(ptlStart.x + x_point_offset,ptlStart.y + y_point_offset),
							CPoint(ptlEnd.x+x_point_offset,ptlEnd.y+ y_point_offset),EXCEPTION_MARK ,single->GetTrainID());
					}

					if(bad_Mark)
					{
						DrawBadMark(pCDC,CPoint(ptlStart.x + x_point_offset,ptlStart.y + y_point_offset),
							CPoint(ptlEnd.x+x_point_offset,ptlEnd.y+ y_point_offset),BAD_MARK ,single->GetTrainID());
					}
					if(reverse_Mark)
					{
						DrawBadMark(pCDC,CPoint(ptlStart.x + x_point_offset,ptlStart.y + y_point_offset),
							CPoint(ptlEnd.x+x_point_offset,ptlEnd.y+ y_point_offset),REVERSE_MARK,single->GetTrainID());
					}
					if(stopReason_Mark)
					{
						DrawBadMark(pCDC,CPoint(ptlStart.x + x_point_offset,ptlStart.y + y_point_offset),
							CPoint(ptlEnd.x+x_point_offset,ptlEnd.y+ y_point_offset),STOPREASTON_MARK,single->GetTrainID());
					}

					if(m_bShowFastOrSlowTime)
					{
						if(!IsFlagStopAbs(first->m_data.flag) && !IsFlagStopSignal(first->m_data.flag))
						{
							DrawFastOrSlowTime(pCDC,CPoint(ptlStart.x + x_point_offset,ptlStart.y + y_point_offset),
								CPoint(ptlEnd.x+x_point_offset,ptlEnd.y+ y_point_offset),IsRunFast_Plot(first),GetFastMinute_Plot(first),nDirectFlag);
						}
					}
				}
			}
			else
			{
				//基本图的过表
				CPoint criticalstart,criticalend;
				if(first_d_time_status && second_a_time_status)
				{  		
					int basicMemoryWidth = m_fPixelPerHour*24;
					criticalstart = CPoint(basicMemoryWidth + x_point_offset
						,ptlStart.y - abs(ptlEnd.y - ptlStart.y)*(basicMemoryWidth - ptlStart.x )/(basicMemoryWidth - ptlStart.x + ptlEnd.x) + y_point_offset);
					criticalend = CPoint(0 + x_point_offset,ptlStart.y - abs(ptlEnd.y - ptlStart.y)*(basicMemoryWidth - ptlStart.x )/(basicMemoryWidth - ptlStart.x + ptlEnd.x) + y_point_offset);

					DrawActualLine(pCDC,CPoint(ptlStart.x + x_point_offset,ptlStart.y + y_point_offset),
						criticalstart,single->GetTrainInfo()->m_data.type,FALSE,m_nCountOfEngine);//single->m_carriage_count);//draw real time line

					DrawActualLine(pCDC,criticalend,
						CPoint(ptlEnd.x+x_point_offset,ptlEnd.y+ y_point_offset),single->GetTrainInfo()->m_data.type,FALSE,m_nCountOfEngine);//single->m_carriage_count);//draw real time line

					DrawMarkOfCritical(pCDC,single,single->GetTrainID(),criticalstart,50,RIGHT_MARK, lActualColor);

					DrawMarkOfCritical(pCDC,single,single->GetTrainID(),criticalend,50,LEFT_MARK, lActualColor);


				}
				else
				{ 
					int temy = ptlStart.y - abs(ptlEnd.y - ptlStart.y)*(m_usMemoryPageWidth - ptlStart.x )/(m_usMemoryPageWidth - ptlStart.x + ptlEnd.x);
					int basicMemoryWidth = m_fPixelPerHour*24;
					criticalstart = CPoint(basicMemoryWidth + x_point_offset
						,ptlStart.y - abs(ptlEnd.y - ptlStart.y)*(basicMemoryWidth - ptlStart.x )/(basicMemoryWidth - ptlStart.x + ptlEnd.x) + y_point_offset);
					criticalend = CPoint(0 + x_point_offset,ptlStart.y - abs(ptlEnd.y - ptlStart.y)*(basicMemoryWidth - ptlStart.x )/(basicMemoryWidth - ptlStart.x + ptlEnd.x) + y_point_offset);

					DrawActualLine(pCDC,CPoint(ptlStart.x + x_point_offset,ptlStart.y + y_point_offset),
						criticalstart,single->GetTrainInfo()->m_data.type,FALSE,m_nCountOfEngine,0,FALSE);//single->m_carriage_count);//draw real time line

					DrawActualLine(pCDC,criticalend,
						CPoint(ptlEnd.x+x_point_offset,ptlEnd.y+ y_point_offset),single->GetTrainInfo()->m_data.type,FALSE,m_nCountOfEngine,0,FALSE);//single->m_carriage_count);//draw real time line

				}
			}

			ptlStart = ptlEnd;
			ptlEnd.y = second_primary_entry;
			if(ptlEnd.y == -1) continue;
			if(ptlEnd.x < rangeRect.right && ptlStart.y != ptlEnd.y)
			{
				// 绘制车站到点到股道竖线
				if(first_d_time_status && second_a_time_status)
				{
					DrawActualLine(pCDC,CPoint(ptlStart.x + x_point_offset,ptlStart.y + y_point_offset),
						CPoint(ptlEnd.x+x_point_offset,ptlEnd.y+ y_point_offset),single->GetTrainInfo()->m_data.type,FALSE,m_nCountOfEngine);//single->m_carriage_count);//draw real time line
				}
				else
				{
					DrawActualLine(pCDC,CPoint(ptlStart.x + x_point_offset,ptlStart.y + y_point_offset),
						CPoint(ptlEnd.x+x_point_offset,ptlEnd.y+ y_point_offset),single->GetTrainInfo()->m_data.type,FALSE,m_nCountOfEngine,0,FALSE);//single->m_carriage_count);//draw real time line
				}
			}
			if( ptlStart.y != ptlEnd.y)
			{
				//point rect information
				m_pPerPointInfo[m_nPointCount].record_index = i + 1;
				m_pPerPointInfo[m_nPointCount].start_depart_flag = ARRIVAL_POSITION;
				m_pPerPointInfo[m_nPointCount++].rect  = CRect(CPoint(ptlStart.x - uDis,ptlStart.y - uDis),sSize);
			}

			ptlStart = ptlEnd;
		}

		if(ptlEnd.y == -1 ) 
			continue; 
	}//end for first to last code 

	// restore gdi resource
	pCDC->SetROP2(nPreviousMode);
	pCDC->SelectObject(pOldPen);
	pCDC->SetTextColor(nOldColor);
	pCDC->SetTextAlign(nOldAlign);
	pCDC->SetBkMode(nOldBackMode); 

	mThinPen.DeleteObject();

	DrawTrainText(pCDC,single,RGB(255,0,0),schd_type);

	if(m_nShowTrainEarlyLateFlag == 3)
		DrawTrainEarlyLastTime(pCDC,single,lColor,isPrint,m_nSchdStatus);
	else if(m_nShowTrainEarlyLateFlag == 1)
	{
		if(single->IsPassengerTrain())
			DrawTrainEarlyLastTime(pCDC,single,lColor,isPrint,m_nSchdStatus);
	}
	else if(m_nShowTrainEarlyLateFlag == 2)
	{
		if(!single->IsPassengerTrain()) 
			DrawTrainEarlyLastTime(pCDC,single,lColor,isPrint,m_nSchdStatus);
	}
}
