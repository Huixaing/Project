// Plot24Doc.cpp: implementation of the CPlot24Doc class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "Plot24Doc.h"
#include "Plot24View.h"
#include "math.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern unsigned long Get_NextShiftid(unsigned long CurShiftid);
extern unsigned long Get_PreShiftid(unsigned long CurShiftid);
extern void OsSetDrawMarkFlag(int nDrawFlag);
extern void OsSetGraphMarkFlag(int nDrawFlag);

#include "basic_schd_config.h"
extern basic_schd_config* GetBasicConfig();
/////////////////////////////////////////////////////////////////////////////
// CPlot24Doc

IMPLEMENT_DYNCREATE(CPlot24Doc, CPlotDoc)

CPlot24Doc::CPlot24Doc()
{
	m_shiftidFirst = 0;
	m_printFormat = 0;
	m_phaseStationWidth = 0;

	m_VectTrainsSubao.clear();
}



BOOL CPlot24Doc::InitData(PRINTINFORM *pPrintInform, CTgDocument *pdoc)
{
	int theStatus = 1;
	
	m_LocalSavedPrintInform=*pPrintInform;
	m_sPrintInform=&m_LocalSavedPrintInform;
	m_nCurrentSectionNo = m_sPrintInform->usSectionNo;
	m_nSchdStatus = m_sPrintInform->usSchdlType;

	int nBasicFlag = 0;
	if(m_nSchdStatus == 1)
	{
		m_nSchdStatus = BASIC_SCHEDULE;
		nBasicFlag = 0;
	}
	else if(m_nSchdStatus == 2)
	{
		m_nSchdStatus = BASIC_SCHEDULE;
		nBasicFlag = 1;
	}
	else if(m_nSchdStatus == 10)
	{
		m_nSchdStatus = BASIC_SCHEDULE;
		nBasicFlag = 2;
	}
	else if(m_nSchdStatus == 4)
		m_nSchdStatus = SHIFT_SCHEDULE;
	else
		m_nSchdStatus = WORK_SCHEDULE;

	m_pTgDoc=pdoc; 

	//在WINTGDOC.CPP中释放内存
	memset(m_sDispDistance, 0, sizeof(m_sDispDistance));



	m_nDisplayMode = FOUR_HOUR_DISPLAY;
	m_nViewFormat = TEN_MINUTE_GRID;
	m_nBasicPlotFlag=0;
	if(m_nSchdStatus == BASIC_SCHEDULE)
	{
		if(pPrintInform->sBasicFlag==1 || pPrintInform->sBasicFlag==2)
		{
			m_nBasicPlotFlag=pPrintInform->sBasicFlag;
			m_nViewTimeRange = WHOLE_DAY_MINUTE/2;
		}
		else
			m_nViewTimeRange = WHOLE_DAY_MINUTE;
	}
	else
		m_nViewTimeRange = (m_sPrintInform->usEndTime - m_sPrintInform->usStartTime) / 60
		+ OsGetTwoDateOffset(PLOT.ThreadId,m_sPrintInform->usEndDate,
		m_sPrintInform->usStartDate) * WHOLE_DAY_MINUTE;
	if(m_nViewTimeRange % 60 == 0)
	{
		m_nViewTimeRange /= 60;
	}
	else 
	{
		m_nViewTimeRange /= 60;
		m_nViewTimeRange += 1;
	}



	m_nHorizontalPos = m_nVerticalPos = 0;
	m_nHistoryGraphWidth = 0;
	CTime t = CTime::GetCurrentTime( );
	m_usCurrentTime = t.GetHour( ) * 60 + t.GetMinute( );
	m_usCurrentDate = (t.GetYear( ) - 1990) * 380 + (t.GetMonth( ) - 1)* 31 + t.GetDay( ) - 1;

	m_usCurrentTime *= 60;
	m_usCurrentTime += t.GetSecond( );


	//aded by qianguorong 2003-11-26
	if(m_nSchdStatus == BASIC_SCHEDULE)
		m_nShiftId = 2;
	else
		m_nShiftId = m_sPrintInform->usShiftid; 


	m_nShiftDate = (m_nShiftId/1000000 - 1990) * 380 +
		(m_nShiftId/10000-(m_nShiftId/1000000)*100 - 1) * 31 + 
		m_nShiftId/100 - (m_nShiftId/10000)*100  - 1 ;
	m_nShiftNo = m_nShiftId - m_nShiftId/100*100;


	//打印模式：1 12小时， 0：24小时
	m_printFormat = gpcdef.GetPrintFormat();

	//打印列车种类，3 客货，2 货车，1 客车
	m_nShowTrainBasicFlag = 3;//暂时简单赋值 m_pDoc->m_nShowTrainBasicFlag;

	int nFlag = 0;
	if(m_nSchdStatus == BASIC_SCHEDULE)
		nFlag = 2;
	else
		nFlag = m_printFormat;
	CString dataPath = gpcdef.GetDataDir();
	CString plotxml = dataPath + "\\plot.xml";
	LoadPlotXML(plotxml, nFlag, m_nShiftNo);
	CString markcfgxml = dataPath + "\\tg\\MARK_STYLE.xml";
	LoadMarkCfgXML(markcfgxml);

	m_lineCfg.LoadTgLineStyleXML(dataPath +"\\tg\\LINE_STYLE.xml");
	m_lineCfg.LoadTgLineColorXML(dataPath +"\\tg\\LINE_COLOR.xml");

	if(m_nSchdStatus == BASIC_SCHEDULE)
	{
		m_nPageWidth = g_basic_total_page_width;     //打印页码的总宽度
		m_nPageHeight = g_basic_total_page_height;  //总高度
	}
	else
	{		
		if(m_printFormat == 1)
		{
			m_nPageWidth  = g_zz_total_page_width;     //打印页码的总宽度
			m_nPageHeight = g_zz_total_page_height;  //总高度
		}
		else
		{
			m_nPageWidth = g_hh_total_page_width;     //打印页码的总宽度
			m_nPageHeight = g_hh_total_page_height;  //总高度
		}
	}

	int height, width;
	if(!GetOffsetAndSize(AREA_TYPE::TRAIN, x_offset_print, y_offset_print, height, width))
	{
		x_offset_print = 70;
		y_offset_print = 70;
	}

	//设置m_sTGGraphStartTime结构中的绘图的起始日期和时间
	GetGraphStartTime(m_nShiftId,m_nSchdStatus);

	if(m_nShiftNo == 2)
	{
		m_shiftidFirst = Get_PreShiftid(m_nShiftId);		
	}
	else
	{
		m_shiftidFirst = m_nShiftId;    
	}

	m_nOperatorStatus = NORMAL_STATUS;	
		
	CWorkScheduleHolder *pholder=m_pTgDoc->GetTGDataHolder();
	CTG_TrainScheduleBase *pdata=NULL;
	if(m_nSchdStatus == WORK_SCHEDULE)
		pdata = pholder->GetWorkScheduleData();
	else if(m_nSchdStatus == SHIFT_SCHEDULE)
		pdata = pholder->GetShiftScheduleData();
	else if(m_nSchdStatus == BASIC_SCHEDULE)
	{
		if(nBasicFlag == 0)
			pdata = pholder->GetBasicScheduleData();
		else if(nBasicFlag == 1)
			pdata = pholder->GetBasicPrevScheduleData();
		else if(nBasicFlag == 2)
			pdata = pholder->GetBasicBackupScheduleData();
	}
	else
		pdata = pholder->GetWorkScheduleData();

	if(pdata==NULL)
	{
		AfxMessageBox("没数据源");
		return TRUE;
	}
	//else 
	//	return TRUE;

	CTG_ScheduleInfo schd;
	pdata->GetScheduleInfo(schd); 
	CTG_TrainPtrGroup group;

	if(m_nSchdStatus == BASIC_SCHEDULE)
		pdata->FindTrains(group,NULL_STATION);
	else
		pdata->FindTrains(group,NULL_STATION,long_time(m_sPrintInform->usStartDate,m_sPrintInform->usStartTime),long_time(m_sPrintInform->usEndDate,m_sPrintInform->usEndTime));

	m_vecSchdTrains.clear();
	int GroupCount = group.GetTrainCount();
	for (int i=0;i< GroupCount ;i++)
	{
		CTG_SingleTrain * train = group.GetTrainPtr(i);//暂时简单赋值 m_pDoc->m_sSchdTrain[i];
		if(NULL == train)
		{
			continue;
		}
		
		CTG_SingleTrain single = *train;
		single.GetRecordList()->PurgeRecords();
		//int nRealRecNum = train->GetRecordList()->GetRecordCount();
		int nRecNum = train->GetTrainRecordCount();//min(train->GetTrainInfo()->m_data.delete_flag, nRealRecNum);
		if(nRecNum <= 0)
			continue;

		for(int idx = 0; idx < nRecNum; idx++)
		{
			CTG_TrainRecord rd = train->GetRecordList()->m_listRecords[idx];
			
			if(m_nSchdStatus == BASIC_SCHEDULE)//基本图打印时为实际线
				rd.m_data.adjust_status |= 0x3;

			if(rd.m_data.special_time_1&0x8000)
				rd.m_data.special_time_1 = rd.m_data.special_time_1&0x7FFF;
			else
				rd.m_data.special_time_1 = rd.m_data.special_time_1/60;
			

			if(rd.m_data.special_time_2&0x8000)
				rd.m_data.special_time_2 = rd.m_data.special_time_2&0x7FFF;
			else
				rd.m_data.special_time_2 = rd.m_data.special_time_2/60;
	
			single.GetRecordList()->AddRecord(rd);
		}
		
		train_additional_info[m_vecSchdTrains.size()].m_nTrainIndexforCompare = single.GetTrainIndex();
		
		m_vecSchdTrains.push_back(single);
				
		single_train_inform_width = 0;//SINGLE_TRAIN_INFORM_WIDTH;//单个列车编组情况宽度
	}

	if (GetTGFunctionConfig()->IsEnableCzh() || GetTGFunctionConfig()->IsEnableNewCzh())
	{
		LoadTrainCZHRelationXML();
		SetTrainBeforeAfterIndex();
	}

	m_nAbsList.DeleteContents();

	CTG_ObjectPtrGroup  mark_group;
	pdata->FindMarks(mark_group);
	CTG_SingleMark *block;
	for(int i=0; i<mark_group.GetCount();i++)
	{
		block =  (CTG_SingleMark *)mark_group.GetPtr(i);	
		if(NULL == block)
			continue;
		CPlotAbsMark* figure = new CPlotAbsMark(this);
		if(figure == NULL)
			continue;

		figure->rec.mark_index = block->m_data.gpc_index;

		TimeToDataAndMinute(block->m_data.start_time, &figure->rec.start_date, &figure->rec.start_time);
		TimeToDataAndMinute(block->m_data.end_time, &figure->rec.end_date, &figure->rec.end_time);

		figure->rec.type = block->m_data.type;           
		figure->rec.flag = block->m_data.flag;  
		figure->rec.line = block->m_data.line;
		figure->rec.station = block->m_data.station;
		figure->rec.index = block->m_data.index;
		figure->rec.position = block->m_data.position;       
		figure->rec.count = block->m_data.ele_count;
		for(int i=0; i<figure->rec.count && i<64; i++)
			figure->rec.ele_number[i] = block->m_data.ele_number[i];
		strncpy(figure->rec.text, block->m_data.text, 255);
		figure->rec.text[255] = 0;

		figure->rec.s_meter = block->m_data.s_meter;
		figure->rec.e_meter = block->m_data.e_meter;
		figure->rec.e_kilo = block->m_data.e_kilo;       
		figure->rec.slow_time = block->m_data.slow_time;

		m_nAbsList.AddFigure(figure);
	}

	m_nShowTrainEarlyLateFlag = 3;
	m_nVerticalOffsetHeight = time_axis_height // time string
		+ 2*header_title_height // header title height
		+ draw_mark_height;// up draw mark height	

	do 
	{
		//读取tg.cfg的配置
		if(!LoadTGProfile())
		{
			sysprint(0,PMTG.ThreadId,"[PMTG]: tg.def failed!");
			break;
		}

		//初始化区段车站的坐标
		InitStationVerticalPos(m_nSchdStatus);

		//初始化时间单元的像素点
		InitTimeUnit( );

		theStatus = 0;
		m_EnvironInitFlag = 3;
	}while(0);
	if(theStatus)
		return FALSE;
	sysprint(0,PLOT.ThreadId,"[PLOT]: doc init successfully");

	return TRUE;
}

CPlot24Doc::~CPlot24Doc()
{
}

BEGIN_MESSAGE_MAP(CPlot24Doc, CPlotDoc)
	//{{AFX_MSG_MAP(CPlot24Doc)
	// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPlot24Doc diagnostics

#ifdef _DEBUG
void CPlot24Doc::AssertValid() const
{
	CPlotDoc::AssertValid();
}

void CPlot24Doc::Dump(CDumpContext& dc) const
{
	CPlotDoc::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CPlot24Doc serialization

void CPlot24Doc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CPlot24Doc commands
void CPlot24Doc::DrawJiShi12(CDC *pCDC, COLORREF clr)
{
	int scaleRate = min(yMultiple,5);

	CRect drawRect;
	CString jishi_text;

	UINT nOldAlign,nOldBackMode;
	CPen mThickPen;
	CPen mBlockPen,*pOldPen;
	UINT OldTextColor;


	LOGFONT stFont;
	CFont mFont;
	CFont *pOldFont;

	//设置各种属性
	mThickPen.CreatePen(PS_SOLID,(int)(1*scaleRate),clr);
	nOldAlign = pCDC->SetTextAlign(TA_LEFT|TA_TOP);
	nOldBackMode = pCDC->SetBkMode(TRANSPARENT);
	pOldPen = pCDC->SelectObject(&mThickPen);
	OldTextColor = pCDC->SetTextColor(clr);
	int m_nLogPixelSy = pCDC->GetDeviceCaps(LOGPIXELSY);


	//设置字体
	memset(&stFont,0,sizeof(stFont));
	stFont.lfHeight = MulDiv(12,m_nLogPixelSy,72);	 
	stFont.lfWeight = FW_THIN;
	stFont.lfClipPrecision = CLIP_LH_ANGLES;
	mFont.CreateFontIndirect(&stFont);
	pOldFont = pCDC->SelectObject(&mFont);


	int offset_x, offset_y, height, width;
	if(!GetOffsetAndSize(AREA_TYPE::NOTE, offset_x, offset_y, height, width))
		return;
	drawRect.top    = offset_y;
	drawRect.bottom = offset_y + height;
	drawRect.left   = offset_x;    
	drawRect.right  = offset_x + width;	

	pCDC->Rectangle(&drawRect);

	int xWidth,yHeight;
	xWidth = drawRect.Width();
	yHeight = drawRect.Height();

	pCDC->MoveTo(drawRect.left,drawRect.top + yHeight * 1/5);
	pCDC->LineTo(drawRect.right,drawRect.top + yHeight * 1/5);

	//画记事标题
	CRect tempRect,inflate_Rect;
	tempRect=drawRect;
	tempRect.InflateRect(0,0,0,-yHeight * 4/5);
	pCDC->DrawText("记          事",&tempRect,DT_VCENTER|DT_CENTER|DT_SINGLELINE|DT_NOCLIP);


	pCDC->SelectObject(pOldFont);
	mFont.DeleteObject();

	memset(&stFont,0,sizeof(stFont));
	stFont.lfHeight = MulDiv(9,m_nLogPixelSy,72);	 
	stFont.lfWeight = FW_THIN;
	stFont.lfClipPrecision = CLIP_LH_ANGLES;
	strcpy(stFont.lfFaceName,"宋体");
	mFont.CreateFontIndirect(&stFont);
	pOldFont = pCDC->SelectObject(&mFont);
	pCDC->SetTextColor(OldTextColor);
	OldTextColor = pCDC->SetTextColor(CLR_BLACK);

	jishi_text = parsecontent[0];

	/////////add by lxl 20100315////////////////////////////////////////////////////
	if(GetTGFunctionConfig()->IsEnablePrintNoteTrainStopOneHour())
	{
		CString oneHourTrains =GetNoteTextOfTaiYuan();
		jishi_text += oneHourTrains;
	}
   //////////////////////////////////////////////////////////////////////////

	inflate_Rect.CopyRect(drawRect);
	inflate_Rect.InflateRect(0,-yHeight * 1/5,0,0);
	inflate_Rect.InflateRect(-5*fWidthMulti,-5*fHeightMulti,-5*fWidthMulti,-5*fHeightMulti);

	pCDC->DrawText(jishi_text,&inflate_Rect,DT_WORDBREAK|DT_EXTERNALLEADING);

	pCDC->SetTextAlign(nOldAlign);
	pCDC->SetBkMode(nOldBackMode);
	pCDC->SelectObject(pOldPen);
	pCDC->SetTextColor(OldTextColor);
	pCDC->SelectObject(pOldFont);

	mThickPen.DeleteObject();
	mFont.DeleteObject();

}

void CPlot24Doc::DrawJiShi24(CDC *pCDC, COLORREF clr)
{
	int scaleRate = min(yMultiple,5);

	CRect drawRect;
	CString jishi_text;

	UINT nOldAlign,nOldBackMode;
	CPen mThickPen;
	CPen mBlockPen,*pOldPen;
	UINT OldTextColor;


	LOGFONT stFont;
	CFont mFont;
	CFont *pOldFont;

	//设置各种属性
	mThickPen.CreatePen(PS_SOLID,(int)(1*scaleRate),clr);
	nOldAlign = pCDC->SetTextAlign(TA_LEFT|TA_TOP);
	nOldBackMode = pCDC->SetBkMode(TRANSPARENT);
	pOldPen = pCDC->SelectObject(&mThickPen);
	OldTextColor = pCDC->SetTextColor(clr);
	int m_nLogPixelSy = pCDC->GetDeviceCaps(LOGPIXELSY);

	//设置字体
	memset(&stFont,0,sizeof(stFont));
	stFont.lfHeight = MulDiv(12,m_nLogPixelSy,72);	 
	stFont.lfWeight = FW_THIN;
	stFont.lfClipPrecision = CLIP_LH_ANGLES;
	mFont.CreateFontIndirect(&stFont);
	pOldFont = pCDC->SelectObject(&mFont);

	int offset_x, offset_y, height, width;
	if(!GetOffsetAndSize(AREA_TYPE::NOTE, offset_x, offset_y, height, width))
		return;
	drawRect.top    = offset_y;
	drawRect.bottom = offset_y + height;
	drawRect.left   = offset_x;    
	drawRect.right  = offset_x + width;	


	pCDC->Rectangle(&drawRect);

	int xWidth,yHeight;
	xWidth = drawRect.Width();
	yHeight = drawRect.Height();

	pCDC->MoveTo(drawRect.left,drawRect.top + yHeight * 1/5);
	pCDC->LineTo(drawRect.right,drawRect.top + yHeight * 1/5);

	//画中间竖线
	pCDC->MoveTo(drawRect.CenterPoint().x,drawRect.top + yHeight * 1/5);
	pCDC->LineTo(drawRect.CenterPoint().x,drawRect.bottom);


	//画记事标题
	CRect tempRect,inflate_Rect;
	tempRect=drawRect;
	tempRect.InflateRect(0,0,0,-yHeight * 4/5);
	pCDC->DrawText("记          事",&tempRect,DT_VCENTER|DT_CENTER|DT_SINGLELINE|DT_NOCLIP);

	pCDC->SelectObject(pOldFont);
	mFont.DeleteObject();

	memset(&stFont,0,sizeof(stFont));
	stFont.lfHeight = MulDiv(9,m_nLogPixelSy,72);	 
	stFont.lfWeight = FW_THIN;
	stFont.lfClipPrecision = CLIP_LH_ANGLES;
	strcpy(stFont.lfFaceName,"宋体");
	mFont.CreateFontIndirect(&stFont);
	pOldFont = pCDC->SelectObject(&mFont);
	pCDC->SetTextColor(OldTextColor);
	OldTextColor = pCDC->SetTextColor(CLR_BLACK);

	jishi_text = parsecontent[0];


	inflate_Rect.CopyRect(drawRect);
	//左边矩形画第一班数据，右边画第二班数据，两半数据用$$分割
	inflate_Rect.InflateRect(0,-yHeight * 1/5,-xWidth/2,0);
	inflate_Rect.InflateRect(-5*fWidthMulti,-5*fHeightMulti,-5*fWidthMulti,-5*fHeightMulti);
	//判断有几班数据
	CString nextJishi;

	if(jishi_text.Find("$$")>=0)
	{	
		nextJishi = jishi_text.Mid(jishi_text.Find("$$")+2);		
		jishi_text = jishi_text.Left(jishi_text.Find("$$"));
	}
	else
	{
		nextJishi.Empty();
	}

	pCDC->DrawText(jishi_text,&inflate_Rect,DT_WORDBREAK|DT_EXTERNALLEADING);

	inflate_Rect.OffsetRect(xWidth/2,0);
	pCDC->DrawText(nextJishi,&inflate_Rect,DT_WORDBREAK|DT_EXTERNALLEADING);

	pCDC->SetTextAlign(nOldAlign);
	pCDC->SetBkMode(nOldBackMode);
	pCDC->SelectObject(pOldPen);
	pCDC->SetTextColor(OldTextColor);
	pCDC->SelectObject(pOldFont);

	BOOL ret = mThickPen.DeleteObject();
	ret = mFont.DeleteObject();
}

//统计站内停留时间超过1小时的车次，add by lxl 20100315
CString CPlot24Doc::GetNoteTextOfTaiYuan( )
{
	long beginTime = long_time(m_sPrintInform->usStartDate,m_sPrintInform->usStartTime);
	long endTime = long_time(m_sPrintInform->usEndDate,m_sPrintInform->usEndTime);
	int hours,mins;

	CTG_SingleTrain* m_hTrain ;
	CString timeoutTrain ="";
	CString tmpStr;
	bool bFound;

	for(int i=0;i < m_vecSchdTrains.size();i++)     //遍历本班所有运行线
	{
		m_hTrain = &m_vecSchdTrains[i];   //第i趟车
		CTG_TrainRecordList* list = m_vecSchdTrains[i].GetRecordList();     //获取节点个数    
		bFound = false;	
		for(int j=0;j<m_vecSchdTrains[i].GetTrainRecordCount();j++)     //遍历第i趟车的所有节点号
		{
			CTG_TrainRecord *rec = m_vecSchdTrains[i].GetTrainRecord(j);   
			if(rec == NULL)
				continue;
			long dlt = rec->GetDepartTime() - rec->GetArriveTime();
			if(dlt >= 3600)
			{
				if(rec->GetArriveTime() >= beginTime &&rec->GetDepartTime() <= endTime)

					bFound=true;

					hours = dlt/3600;
					mins = (dlt/60)%60;
					tmpStr.Format("\n站内停留1小时以上的车次：\n%s %s %d小时%d分\t,", m_vecSchdTrains[i].GetTrainID(),gpcdef.GetStationName(rec->GetStationNo()),hours, mins);	
				}
			}
		if(bFound == true)
		{
			timeoutTrain+=tmpStr;
			timeoutTrain+=" ";
		}
		else  timeoutTrain="";

	}
	return timeoutTrain;

}
int CPlot24Doc::InitTimeUnit()
{
	int ChangeFlag = 0;

	int x_offset_print, y_offset_print, height, width;
	if(!GetOffsetAndSize(AREA_TYPE::TRAIN, x_offset_print, y_offset_print, height, width))
		return ChangeFlag;
	m_fPixelPerMinute = width;
	m_fPixelPerMinute = m_fPixelPerMinute/ (m_nViewTimeRange*60); 

	m_fPixelPerHour = m_fPixelPerMinute * 60;	
	m_fPixelPerHalfHour = m_fPixelPerHour / 2;
	m_fPixelPerTenMinute = m_fPixelPerHour / 6;
	m_fPixelPerTwoMinute = m_fPixelPerHour / 30;
	m_fPixelPerMinute = m_fPixelPerTwoMinute / 2;

	m_fMaxPixelPerMinute = 2*m_fPixelPerMinute;

	if(m_nSchdStatus == WORK_SCHEDULE 
		|| m_nSchdStatus == HISTORY_SCHEDULE 
		|| m_nSchdStatus == SHIFT_SCHEDULE)
	{
		m_usThreeHourMaxValue = 4;        //8
		m_usOneHourMaxValue = 12;         //24
		m_usHalfHourMaxValue = 24;        //48
		m_usTenMinuteMaxValue = 72;      //144
		m_usTwoMinuteMaxValue = 360;     //720 
	} 
	else 
	{
		m_usThreeHourMaxValue = m_nViewTimeRange / 3;
		m_usOneHourMaxValue = m_nViewTimeRange;
		m_usHalfHourMaxValue = m_nViewTimeRange * 2;
		m_usTenMinuteMaxValue = m_nViewTimeRange * 6;
		m_usTwoMinuteMaxValue = m_usTenMinuteMaxValue * 5;
	}

	//用于过表打印时候计算过表的作标点
	m_usMemoryPageWidth = (int)(m_nViewTimeRange * m_fPixelPerHour); 

	return ChangeFlag;
}

void CPlot24Doc::DrawVirtulStationAixis(CDC *pDC, ULONG lColor)  //2012.09.03 增加绘制虚拟站
{
	if (m_nDispDistanceNum <= 0) // 很重要!!!
		return;

	if(m_virtualStation.size()<=0)
		return;

	CPen mStationPen;
	mStationPen.CreatePen(PS_DOT,1,lColor);//虚线

	UINT nOldAlign    = pDC->SetTextAlign(TA_LEFT|TA_BASELINE);
	UINT nOldBackMode = pDC->SetBkMode(TRANSPARENT);
	CPen* pOldPen=pDC->SelectObject(&mStationPen);

	int nWidth =  m_fPixelPerMinute * m_nViewTimeRange*60;
	for (int i=0; i<m_virtualStation.size(); i++)
	{
		int perStationIndex=m_virtualStation[i].perStationIndex;
		int nextStationIndex=m_virtualStation[i].nextStationIndex;
		int nPercent=m_virtualStation[i].nPercent;
		if(nextStationIndex<=perStationIndex)
			continue;

		if(perStationIndex<0)
			continue;

		if(nextStationIndex>=m_nDispDistanceNum)
			continue;

		int delta=m_sDispDistance[nextStationIndex].nStationYPos - m_sDispDistance[perStationIndex].nSecondStationYPos;

		pDC->MoveTo(x_offset_print , m_sDispDistance[perStationIndex].nSecondStationYPos + (delta*nPercent)/100 +y_offset_print);
		pDC->LineTo(nWidth + x_offset_print, m_sDispDistance[perStationIndex].nSecondStationYPos + (delta*nPercent)/100 +y_offset_print);
	}

	pDC->SetTextAlign(nOldAlign);
	pDC->SetBkMode(nOldBackMode);
	pDC->SelectObject(pOldPen);

	mStationPen.DeleteObject();
	return;
}

void CPlot24Doc::DrawStationAixis(CDC *pDC, LONG lColor)
{
	int scaleRate = min(yMultiple,5);

	int i;

	int x_offset_print, y_offset_print, height, width;
	if(!GetOffsetAndSize(AREA_TYPE::TRAIN, x_offset_print, y_offset_print, height, width))
		return;

	if(m_bShowVirtualStationOnPlot)
		DrawVirtulStationAixis(pDC,lColor);

	int nWidth = width;
	UINT nOldAlign,nOldBackMode;
	CPen mThickPen,nDotPen;
	int nPrevRouteSetNo,nCurrRouteSetNo;

	nPrevRouteSetNo=-1; 
	nCurrRouteSetNo=-1; 
	CPen *pOldPen;
	

	nOldAlign = pDC->SetTextAlign(TA_LEFT|TA_BASELINE);
	nOldBackMode = pDC->SetBkMode(TRANSPARENT);
	mThickPen.CreatePen(PS_SOLID,(int)(1*scaleRate),lColor);

	int nStyleCount = 2;
	DWORD StyleArray[2];
	StyleArray[0] = 10* scaleRate;
	StyleArray[1] = 5* scaleRate;
	LOGBRUSH logBrush;
	logBrush.lbStyle = BS_SOLID;
	logBrush.lbColor = lColor;
	logBrush.lbHatch = 0;
	nDotPen.CreatePen(PS_GEOMETRIC|PS_USERSTYLE, (int)1 * scaleRate,&logBrush,nStyleCount,StyleArray);

	pOldPen = pDC->SelectObject(&mThickPen);
	nWidth = m_fPixelPerMinute * m_nViewTimeRange*60; 

	UINT OldTextColor = pDC->SetTextColor(lColor);

	
	for(i=0;i < m_nDispDistanceNum;i++)
	{	
		pDC->SelectObject(pOldPen);
		mThickPen.DeleteObject();
		if(m_sDispDistance[i].distance.node_info.isNeedThick)
		{
			mThickPen.CreatePen(PS_SOLID,(int)(2*scaleRate),lColor);
		}
		else
		{
			mThickPen.CreatePen(PS_SOLID,(int)(1*scaleRate),lColor);
		}

		pOldPen = pDC->SelectObject(&mThickPen);

		pDC->MoveTo(x_offset_print,m_sDispDistance[i].nStationYPos+y_offset_print);
		pDC->LineTo(nWidth + x_offset_print,m_sDispDistance[i].nStationYPos+y_offset_print);
		if(m_sDispDistance[i].distance.node_info.show_sides)
		{
			if(m_sDispDistance[i].distance.node_info.show_sides_mode !=1) 
			{
				CPen mSidePen, mMainSidePen;
				mSidePen.CreatePen(PS_SOLID,(int)(1*scaleRate), lColor);
				mMainSidePen.CreatePen(PS_SOLID,(int)(1*scaleRate), RGB(0,0,255));
				pOldPen = pDC->SelectObject(&mSidePen);
				for (int ii = 1;ii <= m_sDispDistance[i].distance.number_of_side;ii++)
				{
					int nSide = m_sDispDistance[i].distance.side[ii].side_number;
					int nProperty = m_sDispDistance[i].distance.side[ii].side_property;
					if (GetTGFunctionConfig()->IsEnablePlotMainSide())
					{
						if (nProperty==1)
							pDC->SelectObject(&mMainSidePen);
						else
							pDC->SelectObject(&mSidePen);
					}
					pDC->MoveTo(x_offset_print, m_sDispDistance[i].nSideYPos[nSide]+y_offset_print);
					pDC->LineTo(nWidth + x_offset_print, m_sDispDistance[i].nSideYPos[nSide]+y_offset_print);
				}
				pDC->SelectObject(pOldPen);
				mSidePen.DeleteObject();
				mMainSidePen.DeleteObject();
			}
		}
		pDC->MoveTo(x_offset_print,m_sDispDistance[i].nSecondStationYPos+y_offset_print);
		pDC->LineTo(nWidth + x_offset_print,m_sDispDistance[i].nSecondStationYPos+y_offset_print);
		//
		nPrevRouteSetNo = nCurrRouteSetNo;
		nCurrRouteSetNo = m_sDispDistance[i].nRouteSetNo;
	}

	pDC->SetTextAlign(nOldAlign);
	pDC->SetBkMode(nOldBackMode);
	pDC->SelectObject(pOldPen);
	pDC->SetTextColor(OldTextColor);
	
	
	mThickPen.DeleteObject();
	nDotPen.DeleteObject();
}

int CPlot24Doc::InitStationVerticalPos(int schd_type)
{
	Pstruct _node_info *pNodeInfo;
	short i,k;

	int m_nCurDispIndex = 0;
	int m_nCurStationId = 0;
	int nCurSubSectionNo = 0;
	int nTotalHightOfSection;


	int nTotalHightOfSides = 0;
	int fTotalDistance = 0;
	int fDeltaDistance = 0;
		
	m_nDispDistanceNum =  section_node_info.node_count;
	//初始化一些参数，如出入口号，股道个数等
	for (i = 0; i< section_node_info.node_count ;i++)
	{
		pNodeInfo = &section_node_info.node_info[i];

		m_nCurStationId = section_node_info.node_info[i].station_number;

		nCurSubSectionNo =  section_node_info.node_info[i].sub_section;

		m_sDispDistance[m_nCurDispIndex].nStationNo  = m_nCurStationId;
		m_sDispDistance[m_nCurDispIndex].nRouteNo  = nCurSubSectionNo;
		m_sDispDistance[m_nCurDispIndex].nRouteSetNo  = nCurSubSectionNo;

		m_sDispDistance[m_nCurDispIndex].distance.station_number = m_nCurStationId;
		strcpy(m_sDispDistance[m_nCurDispIndex].distance.station_name, pNodeInfo->text);
		strcpy(m_sDispDistance[m_nCurDispIndex].distance.distance, pNodeInfo->kilo_mark);
		m_sDispDistance[m_nCurDispIndex].distance.deltadistance = pNodeInfo->space;

		// add entry&side linestatus 
		m_sDispDistance[m_nCurDispIndex].distance.fHigh = false;

		m_sDispDistance[m_nCurDispIndex].distance.number_of_entry = 0;

		for(k=1;k<=m_sDispDistance[m_nCurDispIndex].distance.number_of_entry;k++)
		{  
			m_sDispDistance[m_nCurDispIndex].distance.entry[k].entry_number = k;
			m_sDispDistance[m_nCurDispIndex].distance.entry[k].fhighlow = false;
		}

		m_sDispDistance[m_nCurDispIndex].distance.number_of_side = 0;
		memset(m_sDispDistance[m_nCurDispIndex].distance.side, 0, 
			sizeof(m_sDispDistance[m_nCurDispIndex].distance.side));
		

		m_sDispDistance[m_nCurDispIndex].distance.entry[0].fhighlow = m_sDispDistance[m_nCurDispIndex].distance.side[0].fhighlow = true;
		m_sDispDistance[m_nCurDispIndex].distance.entry[MAX_ENTRY-1].fhighlow = m_sDispDistance[m_nCurDispIndex].distance.side[MAX_ENTRY-1].fhighlow = false;
		
		memcpy(&m_sDispDistance[m_nCurDispIndex].distance.node_info,pNodeInfo,sizeof(Pstruct _node_info));
		m_nCurDispIndex += 1;

	}

	nTotalHightOfSides = 0;
	fTotalDistance = 0;

	//如果调度员在阶段计划窗口里展开了股道，我们打印的时候也应该打印展开的股道。
	m_nCurDispIndex =0;
	for (i = 0; i<section_node_info.node_count;i++)
	{
		fTotalDistance += m_sDispDistance[m_nCurDispIndex].distance.deltadistance;

		if(mPlotStyle.side_sep > 0)
		{
			if(m_tgNode[i].sta_sep > 0)
				m_sDispDistance[m_nCurDispIndex].distance.node_info.show_sides=true;
			else
				m_sDispDistance[m_nCurDispIndex].distance.node_info.show_sides=false;
		}

		m_sDispDistance[m_nCurDispIndex].distance.node_info.show_sides_mode=0;

		m_sDispDistance[m_nCurDispIndex].distance.number_of_side = 0;
		for(int k=0;k<m_tgNode[i].side_num;k++)
		{
			int no = m_tgNode[i].SIDES[k].side_no;
			m_sDispDistance[m_nCurDispIndex].distance.side[k+1].side_number = no;
			strncpy(m_sDispDistance[m_nCurDispIndex].distance.side[k+1].sidename, m_tgNode[i].SIDES[k].side_name,9);
			m_sDispDistance[m_nCurDispIndex].distance.side[k+1].sidename[9]=0;
			m_sDispDistance[m_nCurDispIndex].distance.side[k+1].side_property= m_tgNode[i].SIDES[k].side_property;

			m_sDispDistance[m_nCurDispIndex].distance.number_of_side++;
		}

		if(m_sDispDistance[m_nCurDispIndex].distance.node_info.show_sides)
			nTotalHightOfSides += (1+m_sDispDistance[m_nCurDispIndex].distance.number_of_side) * step_of_side;

		m_nCurDispIndex += 1;
	}

	
	int x_offset_print, y_offset_print, height, width;
	if(!GetOffsetAndSize(AREA_TYPE::TRAIN, x_offset_print, y_offset_print, height, width))
		return FALSE;
	nTotalHightOfSection = height -
		(section_node_info.sub_section_count-1)*vertical_route_space
		- nTotalHightOfSides-24;
	
	//根据比例来计算各个车站的Y轴坐标
	m_nCurDispIndex =0;
	for (i = 0; i< section_node_info.node_count;i++)
	{
		fDeltaDistance = m_sDispDistance[m_nCurDispIndex].distance.deltadistance;
		if(i == 0 && fDeltaDistance == 0)
		{
			m_nStationYPos[m_nCurDispIndex] = 0;
			m_sDispDistance[m_nCurDispIndex].nStationYPos = m_nStationYPos[m_nCurDispIndex];
			m_sDispDistance[m_nCurDispIndex].nSecondStationYPos = m_sDispDistance[m_nCurDispIndex].nStationYPos;
			if(m_sDispDistance[m_nCurDispIndex].distance.node_info.show_sides)
			{
				m_nStationYPos[m_nCurDispIndex] += (1+m_sDispDistance[m_nCurDispIndex].distance.number_of_side) * step_of_side;
				for(int ii = 1; ii <= m_sDispDistance[m_nCurDispIndex].distance.number_of_side; ii++)
				{
					m_sDispDistance[m_nCurDispIndex].nSideYPos[m_sDispDistance[m_nCurDispIndex].distance.side[ii].side_number] = m_sDispDistance[m_nCurDispIndex].nStationYPos + ii*step_of_side;
				}
			}
			m_sDispDistance[m_nCurDispIndex].nSecondStationYPos = m_nStationYPos[m_nCurDispIndex];
		}
		else if(i != 0 && fDeltaDistance ==0)
		{
			m_nStationYPos[m_nCurDispIndex] = m_nStationYPos[m_nCurDispIndex - 1] + vertical_route_space;
			m_sDispDistance[m_nCurDispIndex].nStationYPos = m_nStationYPos[m_nCurDispIndex];
			//begin
			if(m_sDispDistance[m_nCurDispIndex].distance.node_info.show_sides)
			{
				m_nStationYPos[m_nCurDispIndex] += (1+m_sDispDistance[m_nCurDispIndex].distance.number_of_side) * step_of_side;
				for(int ii = 1; ii <= m_sDispDistance[m_nCurDispIndex].distance.number_of_side; ii++)
				{
					m_sDispDistance[m_nCurDispIndex].nSideYPos[m_sDispDistance[m_nCurDispIndex].distance.side[ii].side_number] = m_sDispDistance[m_nCurDispIndex].nStationYPos + ii*step_of_side;
				}
			}
			m_sDispDistance[m_nCurDispIndex].nSecondStationYPos = m_nStationYPos[m_nCurDispIndex];
			//end 	
		}
		else
		{
			m_nStationYPos[m_nCurDispIndex] = m_nStationYPos[m_nCurDispIndex-1] + (int)(fDeltaDistance * nTotalHightOfSection / fTotalDistance);
			m_sDispDistance[m_nCurDispIndex].nStationYPos = m_nStationYPos[m_nCurDispIndex];
			if(m_sDispDistance[m_nCurDispIndex].distance.node_info.show_sides)
			{
				m_nStationYPos[m_nCurDispIndex] += (1+m_sDispDistance[m_nCurDispIndex].distance.number_of_side) * step_of_side;

				for(int ii = 1; ii <= m_sDispDistance[m_nCurDispIndex].distance.number_of_side; ii++)
				{
					m_sDispDistance[m_nCurDispIndex].nSideYPos[m_sDispDistance[m_nCurDispIndex].distance.side[ii].side_number] = m_sDispDistance[m_nCurDispIndex].nStationYPos + ii*step_of_side;
				}
			}
			m_sDispDistance[m_nCurDispIndex].nSecondStationYPos = m_nStationYPos[m_nCurDispIndex];
		}
		m_nCurDispIndex += 1;
	}	

	return TRUE;
}

void CPlot24Doc::ShowTimeString(CDC *pCDC, long lColor)
{
	int scaleRate = min(yMultiple,5);

	int nOldAlign = pCDC->SetTextAlign(TA_LEFT | TA_TOP);
	char timestr[6];
	POINT ptlLeft;
	CSize textSize;
	ptlLeft.x = 0;
	TGTIME tgTimeTemp = m_sTGGraphStartTime;

	UINT OldTextColor = pCDC->SetTextColor(lColor);
	UINT nOldBackMode = pCDC->SetBkMode(TRANSPARENT);
	LOGFONT stFont;
	CFont  m_fontTimeString;
	int m_nLogPixelSy = pCDC->GetDeviceCaps(LOGPIXELSY);

	memset(&stFont,0,sizeof(stFont));
	stFont.lfHeight = MulDiv(12,m_nLogPixelSy,72);
	stFont.lfWeight = FW_THIN; //FW_NORMAL;
	stFont.lfClipPrecision = CLIP_LH_ANGLES;
	m_fontTimeString.CreateFontIndirect(&stFont);
	CFont *pOldFont = pCDC->SelectObject(&m_fontTimeString);	

	int k;
	int baseHour = m_sTGGraphStartTime.hour;

	CPen mThinPen,mDashPen,mBoldPen,*pOldPen;

	mThinPen.CreatePen(PS_SOLID, 1*scaleRate, lColor);
	mDashPen.CreatePen(PS_DASH,1*scaleRate,lColor);
	mBoldPen.CreatePen(PS_SOLID,2*scaleRate,lColor);

	pOldPen = pCDC->SelectObject(&mThinPen);

	int nWidth;
	int offset_x, offset_y, height;
	if(!GetOffsetAndSize(AREA_TYPE::TOP_TIME, offset_x, offset_y, height, nWidth))
		return;

	if(this->mPlotStyle.id == 5 && m_nSchdStatus != BASIC_SCHEDULE)
	{
		CString str, timestr;
		str.Format("%d", m_nShiftId);
		CString year = str.Left(4); 
		str = str.Right(str.GetLength()-4);
		CString month = str.Left(2);
		str = str.Right(str.GetLength()-2);
		CString day = str.Left(2);
		if(m_nShiftId%2==0)
		{
			timestr.Format("%s/%s/%s 06:00", month, day, year);
		}
		else
		{
			timestr.Format("%s/%s/%s 18:00", month, day, year);
		}

		pCDC->TextOut(offset_x, offset_y-height*4, timestr);
	}

	ptlLeft.x = offset_x;
	ptlLeft.y = offset_y;
	
	pCDC->MoveTo(ptlLeft.x,ptlLeft.y);
	pCDC->LineTo(nWidth + ptlLeft.x,ptlLeft.y);

	// bottom time line
	if(!GetOffsetAndSize(AREA_TYPE::BOTTOM_TIME, offset_x, offset_y, height, nWidth))
		return;

	pCDC->MoveTo(ptlLeft.x,	offset_y);
	pCDC->LineTo(nWidth + ptlLeft.x, offset_y);
	
	int  nYOffset; 
	//draw top time string 
	CPoint basicPoint = ptlLeft;
	for(k=0; k<= m_nViewTimeRange; k++)	
	{
		sprintf(timestr, "%d", (k+baseHour) % 24);
		textSize = pCDC->GetTextExtent(timestr);
		// draw top timestring line
		pCDC->TextOut(ptlLeft.x - textSize.cx/2, ptlLeft.y - 2*textSize.cy , timestr);

		pCDC->TextOut(ptlLeft.x - textSize.cx/2, offset_y - 2*textSize.cy, timestr);

		if(k==m_nViewTimeRange) //最后一次只画数值就行了
			continue;

		for(int i=0; i<7; i++) //有一次是重复的
		{
			if(i%3 == 0) //粗体，半小时的倍数
			{
				nYOffset = 8*yMultiple;
				pCDC->SelectObject(pOldPen);
				pOldPen =pCDC->SelectObject(&mBoldPen);
			}
			else
			{
				nYOffset = 4*yMultiple;
				pCDC->SelectObject(pOldPen);
				pOldPen = pCDC->SelectObject(&mThinPen);
			}

			pCDC->MoveTo (ptlLeft.x + m_fPixelPerTenMinute*i, ptlLeft.y - nYOffset);
			pCDC->LineTo (ptlLeft.x + m_fPixelPerTenMinute*i, ptlLeft.y);

			pCDC->MoveTo (ptlLeft.x + m_fPixelPerTenMinute*i, offset_y - nYOffset);
			pCDC->LineTo (ptlLeft.x + m_fPixelPerTenMinute*i, offset_y);
		}

		ptlLeft.x = basicPoint.x + m_fPixelPerHour*(k+1);
	}

	pCDC->SetTextAlign(nOldAlign);
	pCDC->SetTextColor(OldTextColor);
	pCDC->SelectObject(pOldPen);
	pCDC->SetBkMode(nOldBackMode);
	pCDC->SelectObject(pOldFont);

	m_fontTimeString.DeleteObject();
	mThinPen.DeleteObject();
	mDashPen.DeleteObject();
	mBoldPen.DeleteObject();
}

void CPlot24Doc::GetGraphStartTime(ulong nShiftId, int schd_type)
{
	long nShiftDate;
	int  nShiftNo;

	nShiftDate = (nShiftId/1000000 - 1990) * 380 +
		(nShiftId/10000-(nShiftId/1000000)*100 - 1) * 31 + 
		nShiftId/100 - (nShiftId/10000)*100  - 1 ;
	nShiftNo = nShiftId - nShiftId/100*100;

	switch(schd_type){
	case WORK_SCHEDULE:
	case HISTORY_SCHEDULE:
	case SHIFT_SCHEDULE:
		if(m_printFormat == 1)//打印十二小时运行图
		{
			if(nShiftNo == 1){
				m_sTGGraphStartTime.day = OsGetPrevDate(PMTG.ThreadId,nShiftDate);
				m_sTGGraphStartTime.hour = 18;   //0
				m_sTGGraphStartTime.minute = 0;
			}else{
				m_sTGGraphStartTime.day = nShiftDate;
				m_sTGGraphStartTime.hour = 6;  //12
				m_sTGGraphStartTime.minute = 0;
			}
		}
		else//打印二十四小时运行图
		{
			m_sTGGraphStartTime.day = OsGetPrevDate(PMTG.ThreadId,nShiftDate);
			m_sTGGraphStartTime.hour = 18;
			m_sTGGraphStartTime.minute = 0;
		}

		break;
	case BASIC_SCHEDULE:
		m_sTGGraphStartTime.day = 0;
		m_sTGGraphStartTime.hour =  m_sPrintInform->usStartTime/3600;
		m_sTGGraphStartTime.minute = 0;
		break;
	default:
		break;
	}

}

void CPlot24Doc::ShowVirtualStationString(CDC * pDC, long lColor)
{
	if (!m_bShowVirtualStationOnPlot)
		return;
	if(m_virtualStation.size()<=0)
		return;

	int m_nLogPixelSy = pDC->GetDeviceCaps(LOGPIXELSY);
	CFont m_fontStation;
	LOGFONT stFont;

	memset(&stFont,0,sizeof(stFont));
	stFont.lfHeight = MulDiv(11,m_nLogPixelSy,72);
	stFont.lfWeight = FW_THIN; ;
	stFont.lfClipPrecision = CLIP_LH_ANGLES;
	m_fontStation.CreateFontIndirect(&stFont);

	UINT nOldAlign = pDC->SetTextAlign(TA_RIGHT|TA_BASELINE);
	UINT nOldBackMode = pDC->SetBkMode(TRANSPARENT);
	COLORREF nOldColor = pDC->SetTextColor(lColor);
	CFont* pFont=pDC->SelectObject(&m_fontStation);
	CSize textSize = pDC->GetTextExtent("A");

	int x_offset_print, y_offset_print, height, width;
	if(!GetOffsetAndSize(AREA_TYPE::TRAIN, x_offset_print, y_offset_print, height, width))
		return;

	int xOffset_left  = x_offset_print - 10; //右对齐
	int xOffset_right = x_offset_print + m_fPixelPerMinute * m_nViewTimeRange*60 + 10; //左对齐

	for (int i=0; i<m_virtualStation.size(); i++)
	{
		int perStationIndex=m_virtualStation[i].perStationIndex;
		int nextStationIndex=m_virtualStation[i].nextStationIndex;
		CString strStatinName=m_virtualStation[i].strStatinName;
		int nPercent=m_virtualStation[i].nPercent;

		if(nextStationIndex<=perStationIndex)
			continue;

		if(perStationIndex<0)
			continue;

		if(nextStationIndex>=m_nDispDistanceNum)
			continue;

		int delta = m_sDispDistance[nextStationIndex].nStationYPos - m_sDispDistance[perStationIndex].nSecondStationYPos;
		pDC->SetTextAlign(TA_RIGHT|TA_BASELINE);
		int StNameY = m_sDispDistance[perStationIndex].nSecondStationYPos + (delta*nPercent)/100 +textSize.cy/2 + y_offset_print;
		pDC->TextOut(xOffset_left,	StNameY,  strStatinName);

		if(this->mPlotStyle.id != 5)
		{
				pDC->SetTextAlign(TA_LEFT|TA_BASELINE);
				pDC->TextOut(xOffset_right,	StNameY, strStatinName);
		}
	}

	pDC->SetTextAlign(nOldAlign);
	pDC->SetBkMode(nOldBackMode);
	pDC->SetTextColor(nOldColor);
	pDC->SelectObject(pFont);

	m_fontStation.DeleteObject();
	return;
}

void CPlot24Doc::ShowStationString(CDC *pDC, long lColor, int flag)
{
	int i;
	UINT nOldAlign,nOldBackMode,nOldColor;
	int textOffset = 502;

	int x_offset_print, y_offset_print, height, width;
	if(!GetOffsetAndSize(AREA_TYPE::TRAIN, x_offset_print, y_offset_print, height, width))
		return;

	CFont m_fontStation,m_fontSide;
	LOGFONT stFont;

	int m_nLogPixelSy = pDC->GetDeviceCaps(LOGPIXELSY);

	memset(&stFont,0,sizeof(stFont));
	stFont.lfHeight = MulDiv(12,m_nLogPixelSy,72);
	stFont.lfWeight = FW_THIN; 
	stFont.lfClipPrecision = CLIP_LH_ANGLES;
	m_fontStation.CreateFontIndirect(&stFont);

	stFont.lfHeight = MulDiv(8,m_nLogPixelSy,72);
	m_fontSide.CreateFontIndirect(&stFont);

	CFont *pOldFont = pDC->SelectObject(&m_fontStation);
	CPen mThickPen,*pOldPen;

	mThickPen.CreatePen(PS_INSIDEFRAME,10,CLR_GRAY);
	nOldAlign = pDC->SetTextAlign(TA_RIGHT|TA_BASELINE);
	nOldBackMode = pDC->SetBkMode(TRANSPARENT);
	nOldColor = pDC->SetTextColor(lColor);
	pOldPen = pDC->SelectObject(&mThickPen);
	CSize textSize = pDC->GetTextExtent("A");
	// draw the text on the memory device context(the bitmap)
	int nStationNo = 0;
	CString str;
	int delta;

	int xOffset_left  = x_offset_print - 10; //右对齐
	int xOffset_right = x_offset_print + m_fPixelPerMinute * m_nViewTimeRange*60 + 10; //左对齐
	

	for(i=0;i < m_nDispDistanceNum;i++)
	{
		pDC->SelectObject(pOldFont);
		pOldFont = pDC->SelectObject(&m_fontStation);

		CString strInhibitDir;
		if(m_sDispDistance[i].distance.node_info.inhibit_sametime_dir == 0)
			strInhibitDir = "▼";
		else if(m_sDispDistance[i].distance.node_info.inhibit_sametime_dir == 1)
			strInhibitDir = "▲";
		else if(m_sDispDistance[i].distance.node_info.inhibit_sametime_dir == 2)
			strInhibitDir = "■";
		else
			strInhibitDir = "";
		nStationNo = m_sDispDistance[i].distance.node_info.station_number;
		str.Format("%s",m_sDispDistance[i].distance.node_info.text);
		str = str + strInhibitDir;

		delta = m_fPixelPerHour* m_nViewTimeRange;

		
		pDC->SetTextAlign(TA_RIGHT|TA_BASELINE);
		pDC->TextOut(xOffset_left
			,m_sDispDistance[i].nStationYPos + textSize.cy/2 + y_offset_print
			,str);


		CString dist_stone;
			dist_stone.Format("K%.3f",m_sDispDistance[i].distance.distance);
			dist_stone.Replace(".","+");
		if(m_bShowdistanceStone)
		{
			
			//下移一个字节高度		 
			pDC->TextOut(xOffset_left
				,m_sDispDistance[i].nStationYPos + 3*textSize.cy/2 + y_offset_print
				,dist_stone);
		}

		if(this->mPlotStyle.id != 5)
		{
			pDC->SetTextAlign(TA_LEFT|TA_BASELINE);
			pDC->TextOut(xOffset_right
				,m_sDispDistance[i].nStationYPos + textSize.cy/2 + y_offset_print
				,str);
		}


		//下移一个字节高度	
		if(m_bShowdistanceStone)
		{
		pDC->TextOut(xOffset_right
			,m_sDispDistance[i].nStationYPos + 3*textSize.cy/2 + y_offset_print
			,dist_stone);
		}

		//see if draw sides on the station
		pDC->SelectObject(&pOldFont);
		pOldFont = pDC->SelectObject(&m_fontSide);
		if(m_sDispDistance[i].distance.node_info.show_sides
			&& m_sDispDistance[i].distance.node_info.show_sides_mode ==0)
		{  
			int sideno,position;
			CString str;
			for(int ii = 1; ii <= m_sDispDistance[i].distance.number_of_side; ii++)
			{
				sideno = m_sDispDistance[i].distance.side[ii].side_number;

				str.Format("%s",m_sDispDistance[i].distance.side[ii].sidename);

				if(sideno < 512 && sideno >=0)
				{
					position =  m_sDispDistance[i].nSideYPos[sideno];
					pDC->SetTextAlign(TA_RIGHT|TA_BASELINE);
					pDC->TextOut(xOffset_left+8,	position + y_offset_print+3, _T(str));
					if(this->mPlotStyle.id != 5)
					{
						pDC->SetTextAlign(TA_LEFT|TA_BASELINE);
						pDC->TextOut(xOffset_right-8,	position + y_offset_print+3, _T(str));
					}
				}
			}
		}
	}

	pDC->SelectObject(pOldFont);
	pDC->SelectObject(pOldPen);
	pDC->SetTextAlign(nOldAlign);
	pDC->SetBkMode(nOldBackMode);
	pDC->SetTextColor(nOldColor);

	m_fontStation.DeleteObject();
	m_fontSide.DeleteObject();
	mThickPen.DeleteObject();
}

void CPlot24Doc::DrawTrainInform24(CDC *pCDC, COLORREF clrBackground)
{
	int x_offset_print, y_offset_print, frameHeight, frameWidth;

	if(m_printFormat == 1)
	{
		if(!GetOffsetAndSize(AREA_TYPE::QB, x_offset_print, y_offset_print, frameHeight, frameWidth))
			return;

		int shiftFlag;
		if(m_nShiftNo == 1)
			shiftFlag = SHIFT_FIRST;
		else
			shiftFlag = SHIFT_SECOND;

		if(this->mPlotStyle.id == 1 || this->mPlotStyle.id == 3)
			DrawSubao24Xian(pCDC,clrBackground, x_offset_print,y_offset_print, frameWidth, frameHeight, shiftFlag);
		else if(this->mPlotStyle.id == 5)
			DrawSubao24Lanzhou(pCDC,clrBackground,x_offset_print,y_offset_print,frameWidth,frameHeight,shiftFlag);
		else
			DrawSubao24(pCDC,clrBackground,x_offset_print,y_offset_print,frameWidth,frameHeight,shiftFlag);
	}
	else
	{
		if(!GetOffsetAndSize(AREA_TYPE::QB_RIGHT, x_offset_print, y_offset_print, frameHeight, frameWidth))
			return;
		DrawSubao24(pCDC,clrBackground, x_offset_print, y_offset_print, frameWidth, frameHeight, SHIFT_FIRST);

		if(!GetOffsetAndSize(AREA_TYPE::QB_LEFT, x_offset_print, y_offset_print, frameHeight, frameWidth))
			return;
		DrawSubao24(pCDC,clrBackground, x_offset_print, y_offset_print, frameWidth, frameHeight, SHIFT_SECOND);
	}
}

void CPlot24Doc::DrawSubao24Lanzhou(CDC *pCDC, COLORREF clrBackground, int xOffset, int yOffset, int frameWidth, int frameHeight,int shiftFlag)
{
	int scaleRate = min(yMultiple,5);
	int m_nLogPixelSy = pCDC->GetDeviceCaps(LOGPIXELSY);
	frameWidth -= 1;
	int rowNum = 80;
	double colRate[] = {0.0, 0.21, 0.40, 0.13, 0.13,0.13};
	char* colTitle[] ={"车次","机车","现车","总重", "换长",""};

	double logUnitsPeRow = (double)frameHeight / rowNum;

	int i = 0;

	CBrush  m_brushBack;
	m_brushBack.CreateSolidBrush(CLR_WHITE); 
	pCDC->FillRect(CRect(xOffset,yOffset,  xOffset+frameWidth, yOffset+frameHeight),&m_brushBack);                   
	m_brushBack.DeleteObject( );


	CPen mThinPen,*pOldPen;
	UINT OldTextColor;
	UINT nOldBackMode;

	mThinPen.CreatePen(PS_SOLID, 1*scaleRate, clrBackground);

	pOldPen = pCDC->SelectObject(&mThinPen);
	OldTextColor = pCDC->SetTextColor(clrBackground);
	nOldBackMode = pCDC->SetBkMode(TRANSPARENT);

	for(i=0; i<rowNum+1; i++) //因为包括了标题栏的那一行
	{
		pCDC->MoveTo(xOffset,yOffset+i*logUnitsPeRow);
		pCDC->LineTo(xOffset+frameWidth,yOffset+i*logUnitsPeRow);
	}

	int xOffsetRow, yOffsetRow;
	xOffsetRow = xOffset;
	yOffsetRow = yOffset;

	//不画最右边一列，因为可能存在偏差
	for(i=0; i<sizeof(colRate)/sizeof(colRate[0])-1; i++)
	{
		pCDC->MoveTo(xOffsetRow+colRate[i]*frameWidth,yOffsetRow);
		pCDC->LineTo(xOffsetRow+colRate[i]*frameWidth,yOffsetRow+frameHeight);

		xOffsetRow += colRate[i]*frameWidth;
	}

	//补画最右边一列
	pCDC->MoveTo(xOffset+frameWidth,yOffset);
	pCDC->LineTo(xOffset+frameWidth,yOffset+frameHeight);	

	//画标题栏，把标题栏绘在向上填充的第一行
	CRect rect;
	CPoint lt,rb;
	int rectWidth;
	lt.x = xOffset;
	lt.y = rb.y = yOffset;
	rb.y += logUnitsPeRow;

	//创建使用的字体
	CFont titleFont;
	LOGFONT stFont;
	CFont * pOldFont;

	memset(&stFont,0,sizeof(stFont));
	stFont.lfHeight = MulDiv(12,m_nLogPixelSy,72);
	stFont.lfWeight = FW_THIN;
	stFont.lfClipPrecision = CLIP_LH_ANGLES;
	titleFont.CreateFontIndirect(&stFont);

	pOldFont = pCDC->SelectObject(&titleFont);

	for(i=0; i<sizeof(colRate)/sizeof(colRate[0]); i++)
	{
		lt.x += colRate[i]*frameWidth;
		rectWidth = colRate[i+1]*frameWidth;
		rb.x = lt.x + rectWidth;
		rect.SetRect(lt,rb);	
		pCDC->DrawText(colTitle[i],rect,DT_CENTER|DT_VCENTER|DT_SINGLELINE);  //CUIHU!!!  colTitle比colRate少一项！
	}

	pCDC->SelectObject(pOldFont);
	titleFont.DeleteObject();

	CFont bigTitleFont;
	memset(&stFont,0,sizeof(stFont));
	stFont.lfHeight = MulDiv(16,m_nLogPixelSy,72);
	stFont.lfWeight = FW_BOLD;
	stFont.lfClipPrecision = CLIP_LH_ANGLES;
	bigTitleFont.CreateFontIndirect(&stFont);

	pOldFont = pCDC->SelectObject(&bigTitleFont);
	CPoint titleCenter;
	titleCenter.x = xOffset + frameWidth/2;
	titleCenter.y = yOffset-8*fHeightMulti;

	UINT oldTextAlign;
	oldTextAlign = pCDC->SetTextAlign(TA_CENTER|TA_BOTTOM);
	pCDC->TextOut(titleCenter.x,titleCenter.y,"列  车  编  组  表");

	pCDC->SetTextAlign(oldTextAlign);
	pCDC->SelectObject(pOldFont);
	bigTitleFont.DeleteObject();

	memset(&stFont,0,sizeof(stFont));
	stFont.lfHeight = MulDiv(6,m_nLogPixelSy,72);
	stFont.lfWeight = FW_THIN;
	stFont.lfClipPrecision = CLIP_LH_ANGLES;
	
	strcpy(stFont.lfFaceName,"宋体");
	titleFont.CreateFontIndirect(&stFont);	
	pCDC->SelectObject(&titleFont);

	pCDC->SetTextColor(CLR_BLACK);

	int nSubaoCount = m_VectTrainsSubao.size();
	if(nSubaoCount>0) //绘制受到的实际速报数据
	{
		int printedNum = 0;
		CString text;

		yOffset += logUnitsPeRow; //向下偏移一行

		//下行编组
		bool find;
		for(i=0; i<nSubaoCount; i++)
		{
			//根据车次号判断上下行
			if(strlen(m_VectTrainsSubao[i].Train_id) == 0)
				continue;

			char otherDigit; int realNum=0;
			otherDigit= '\0';
			int trainidlen = strlen(m_VectTrainsSubao[i].Train_id);
			find=false;
			for(int m=0; m<trainidlen; m++)
			{	
				realNum = trainidlen - m -1;
				otherDigit= m_VectTrainsSubao[i].Train_id[realNum];
				if((otherDigit>= 0x30 && otherDigit<= 0x39))
				{
					find=true;
					break;
				}
			}
			if(find&&(otherDigit-'0')%2==0)
				continue;

			if(m_VectTrainsSubao[i].shift_id%2 != shiftFlag) //第一班，打在左边
				continue;			

			if(m_VectTrainsSubao[i].Print_flag != 0) //不打印
				continue;			

			if(printedNum > min(nSubaoCount,rowNum))
				continue;

			lt.y = yOffset+printedNum*logUnitsPeRow;
			rb.y = lt.y + logUnitsPeRow;
			lt.x = rb.x = xOffset;

			//"车次","机车","现车","总重","换长"
			for(int j=0; j<sizeof(colRate)/sizeof(colRate[0])-1;j++)
			{		
				lt.x = rb.x;
				rectWidth = frameWidth*colRate[j+1];
				rb.x = lt.x + rectWidth; 
				rect.SetRect(lt,rb);
				switch (j)
				{
				case 0:
					text.Format("%s",m_VectTrainsSubao[i].Train_id);
					break;
				case 1:
					text="";
					for(int k=0; k<m_VectTrainsSubao[i].tract_engine_count;k++)
					{
						CString tmp;
						tmp.Format("%s",gpcdef.GetEngineDesp(m_VectTrainsSubao[i].tract_engine_id[k]));
						if(!tmp.IsEmpty())
						{
							if(k==0)
							  text = tmp;
							else
							{
								text += "/";
								text += tmp;
							}
						}
					}
					break;				
				case 4:
					text.Format("%.1f",((float)m_VectTrainsSubao[i].Cal_len)/10);
					break;
				case 3:
					text.Format("%d",((int)m_VectTrainsSubao[i].Total_weight)/10);
					break;
				case 2:  
					text.Format("%d", m_VectTrainsSubao[i].K_count);// 兰州增加辆数 2016.8.24
					break;
				default:
					text="";//added by zhaoliang 打印现车清空。否则打印机车号。
				}

				pCDC->DrawText(text,rect,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
			}
			printedNum++;			
		}
		if(printedNum>0)
			printedNum++;

		//上行编组
		for(i=0; i<nSubaoCount; i++)
		{
			//根据车次号判断上下行
			if(strlen(m_VectTrainsSubao[i].Train_id) == 0)
				continue;
			char otherDigit; int realNum=0;
			otherDigit= '\0';
			int trainidlen = strlen(m_VectTrainsSubao[i].Train_id);
			find=false;
			for(int m=0; m<trainidlen; m++)
			{	
				realNum = trainidlen - m -1;
				otherDigit= m_VectTrainsSubao[i].Train_id[realNum];
				if( (otherDigit>= 0x30 && otherDigit<=0x39)){
					find=true;
					break;
				}
			}
			if(!(find&&(otherDigit-'0')%2==0))
				continue;

			if(m_VectTrainsSubao[i].shift_id%2 != shiftFlag) //第一班，打在左边
				continue;			

			if(m_VectTrainsSubao[i].Print_flag != 0) //不打印
				continue;			

			if(printedNum > min(nSubaoCount,rowNum))
				continue;

			lt.y = yOffset+printedNum*logUnitsPeRow;
			rb.y = lt.y + logUnitsPeRow;
			lt.x = rb.x = xOffset;

			//"车次","机车","现车","总重","换长"
			for(int j=0; j<sizeof(colRate)/sizeof(colRate[0])-1;j++)
			{		
				lt.x = rb.x;
				rectWidth = frameWidth*colRate[j+1];
				rb.x = lt.x + rectWidth; 
				rect.SetRect(lt,rb);
				switch (j)
				{
				case 0:
					text.Format("%s",m_VectTrainsSubao[i].Train_id);
					break;
				case 1:
					text="";
					for(int k=0; k<m_VectTrainsSubao[i].tract_engine_count;k++)
					{
						CString tmp;
						tmp.Format("%s", gpcdef.GetEngineDesp(m_VectTrainsSubao[i].tract_engine_id[k]));
						if(!tmp.IsEmpty())
						{
							if(k==0)
							  text = tmp;
							else
							{
								text += "/";
								text += tmp;
							}
						}
					}
					break;				
				case 4:
					text.Format("%.1f",((float)m_VectTrainsSubao[i].Cal_len)/10);
					break;
				case 3:
					text.Format("%d",((int)m_VectTrainsSubao[i].Total_weight)/10);
					break;
				case 2:  
					text.Format("%d", m_VectTrainsSubao[i].K_count);// 兰州增加辆数 2016.8.24
					break;
				default:
					text="";
				}

				pCDC->DrawText(text,rect,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
			}
			printedNum++;			
		}
	}

	pCDC->SelectObject(pOldPen);
	pCDC->SelectObject(pOldFont);
	pCDC->SetBkMode(nOldBackMode);
	pCDC->SetTextColor(OldTextColor);
	titleFont.DeleteObject();
	mThinPen.DeleteObject();
}

void CPlot24Doc::DrawSubao24(CDC *pCDC, COLORREF clrBackground, int xOffset, int yOffset, int frameWidth, int frameHeight,int shiftFlag)
{
	int scaleRate = min(yMultiple,5);
	int m_nLogPixelSy = pCDC->GetDeviceCaps(LOGPIXELSY);
    int nSubaoCount = m_VectTrainsSubao.size();
	int rowNum = m_nSubaoRowNum;
	
	if(nSubaoCount>80)
	{
		if(nSubaoCount<m_nSubaoRowNum)
			rowNum=nSubaoCount;
		else
			rowNum=m_nSubaoRowNum;
	}
	else
	{
		rowNum=80;
	}
	rowNum+=2;
		
	double colRate[] = {0.0, 0.07, 0.11, 0.11, 0.11, 0.11, 0.14, 0.07, 0.09, 0.11, 0.08};
	char* colTitle[] ={"序","车次","机车1","机车2","司机","车长","辆数","计长","总重" ,"备注","" };

	double logUnitsPeRow = (double)frameHeight / rowNum;

	int i = 0;

	CBrush  m_brushBack;
	m_brushBack.CreateSolidBrush(CLR_WHITE); 
	pCDC->FillRect(CRect(xOffset,yOffset,  xOffset+frameWidth, yOffset+frameHeight),&m_brushBack);                   
	m_brushBack.DeleteObject( );


	CPen mThinPen,*pOldPen;
	UINT OldTextColor;
	UINT nOldBackMode;

	mThinPen.CreatePen(PS_SOLID, 1*scaleRate, clrBackground);

	pOldPen = pCDC->SelectObject(&mThinPen);
	OldTextColor = pCDC->SetTextColor(clrBackground);
	nOldBackMode = pCDC->SetBkMode(TRANSPARENT);

	for(i=0; i<rowNum+1; i++) //因为包括了标题栏的那一行
	{
		pCDC->MoveTo(xOffset,yOffset+i*logUnitsPeRow);
		pCDC->LineTo(xOffset+frameWidth,yOffset+i*logUnitsPeRow);
	}

	int xOffsetRow, yOffsetRow;
	xOffsetRow = xOffset;
	yOffsetRow = yOffset;

	//不画最右边一列，因为可能存在偏差
	for(i=0; i<sizeof(colRate)/sizeof(colRate[0])-1; i++)
	{
		pCDC->MoveTo(xOffsetRow+colRate[i]*frameWidth,yOffsetRow);
		pCDC->LineTo(xOffsetRow+colRate[i]*frameWidth,yOffsetRow+frameHeight);

		xOffsetRow += colRate[i]*frameWidth;
	}

	//补画最右边一列
	pCDC->MoveTo(xOffset+frameWidth,yOffset);
	pCDC->LineTo(xOffset+frameWidth,yOffset+frameHeight);	

	//画标题栏，把标题栏绘在向上填充的第一行
	CRect rect;
	CPoint lt,rb;
	int rectWidth;
	lt.x = xOffset;
	lt.y = rb.y = yOffset;
	rb.y += logUnitsPeRow;

	//创建使用的字体
	CFont titleFont;
	LOGFONT stFont;
	CFont * pOldFont;

	memset(&stFont,0,sizeof(stFont));
	stFont.lfHeight = MulDiv(12,m_nLogPixelSy,72);
	stFont.lfWeight = FW_THIN;
	stFont.lfClipPrecision = CLIP_LH_ANGLES;
	//	strcpy(stFont.lfFaceName,"Arial");
	titleFont.CreateFontIndirect(&stFont);

	pOldFont = pCDC->SelectObject(&titleFont);

	for(i=0; i<sizeof(colRate)/sizeof(colRate[0]); i++)
	{
		lt.x += colRate[i]*frameWidth;
		rectWidth = colRate[i+1]*frameWidth;
		rb.x = lt.x + rectWidth;
		rect.SetRect(lt,rb);	
		pCDC->DrawText(colTitle[i],rect,DT_CENTER|DT_VCENTER|DT_SINGLELINE);  //CUIHU!!!  colTitle比colRate少一项！
	}

	pCDC->SelectObject(pOldFont);
	titleFont.DeleteObject();

	CFont bigTitleFont;
	memset(&stFont,0,sizeof(stFont));
	stFont.lfHeight = MulDiv(16,m_nLogPixelSy,72);
	stFont.lfWeight = FW_BOLD;
	stFont.lfClipPrecision = CLIP_LH_ANGLES;
	bigTitleFont.CreateFontIndirect(&stFont);

	pOldFont = pCDC->SelectObject(&bigTitleFont);
	CPoint titleCenter;
	titleCenter.x = xOffset + frameWidth/2;
	titleCenter.y = yOffset-10*fHeightMulti;

	UINT oldTextAlign;
	oldTextAlign = pCDC->SetTextAlign(TA_CENTER|TA_BOTTOM);
	pCDC->TextOut(titleCenter.x,titleCenter.y,"列  车  编  组  表");

	pCDC->SetTextAlign(oldTextAlign);
	pCDC->SelectObject(pOldFont);
	bigTitleFont.DeleteObject();

	memset(&stFont,0,sizeof(stFont));
	stFont.lfHeight = MulDiv(6,m_nLogPixelSy,72);
	stFont.lfWeight = FW_THIN;
	stFont.lfClipPrecision = CLIP_LH_ANGLES;
	//	strcpy(stFont.lfFaceName,"Arial");
	strcpy(stFont.lfFaceName,"宋体");
	titleFont.CreateFontIndirect(&stFont);	
	pCDC->SelectObject(&titleFont);

	pCDC->SetTextColor(CLR_BLACK);

	if(nSubaoCount>0) //绘制受到的实际速报数据
	{
		int printedNum = 0;
		CString text;

		yOffset += logUnitsPeRow; //向下偏移一行

		//下行编组
		bool find;
		for(i=0; i<nSubaoCount; i++)
		{
			//根据车次号判断上下行
			if(strlen(m_VectTrainsSubao[i].Train_id) == 0)
				continue;

			char otherDigit; int realNum=0;
			otherDigit= '\0';
			int trainidlen = strlen(m_VectTrainsSubao[i].Train_id);
			find=false;
			for(int m=0; m<trainidlen; m++)
			{	
				realNum = trainidlen - m -1;
				otherDigit= m_VectTrainsSubao[i].Train_id[realNum];
				if((otherDigit>= 0x30 && otherDigit<= 0x39)){
					find=true;
					break;
				}
			}
			if(find&&(otherDigit-'0')%2==0)
				continue;

			if(m_VectTrainsSubao[i].shift_id%2 != shiftFlag) //第一班，打在左边
				continue;			

			if(m_VectTrainsSubao[i].Print_flag != 0) //不打印
				continue;			

			lt.y = yOffset+printedNum*logUnitsPeRow;
			rb.y = lt.y + logUnitsPeRow;
			lt.x = rb.x = xOffset;

			//"序号","车次","机车1","机车2","司机","车长","辆数","计长","总重","备注"
			for(int j=0; j<sizeof(colRate)/sizeof(colRate[0])-1;j++)
			{		
				lt.x = rb.x;
				rectWidth = frameWidth*colRate[j+1];
				rb.x = lt.x + rectWidth; 
				rect.SetRect(lt,rb);
				switch (j)
				{
				case 0:
					text.Format("%d",printedNum+1);
					break;
				case 1:
					text.Format("%s",m_VectTrainsSubao[i].Train_id);
					break;
				case 2:
					text = gpcdef.GetEngineDesp(m_VectTrainsSubao[i].tract_engine_id[0]);
					break;
				case 3:
					text = gpcdef.GetEngineDesp(m_VectTrainsSubao[i].tract_engine_id[1]);
					break;				
				case 4:
					text.Format("%s",m_VectTrainsSubao[i].Driver_name);					
					break;				
				case 5:
					text.Format("%s",m_VectTrainsSubao[i].Train_leader_name);					
					break;
				case 6:
					text.Format("%d",m_VectTrainsSubao[i].K_count);			 	
					break;
				case 7:
					text.Format("%.1f",((float)m_VectTrainsSubao[i].Cal_len)/10);
					break;
				case 8:
					text.Format("%d",((int)m_VectTrainsSubao[i].Total_weight)/10);
					break;
				case 9:
					text="";
					break;
				}

				pCDC->DrawText(text,rect,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
			}
			printedNum++;			
		}
		if(printedNum>0)
			printedNum++;
		//上行编组
		for(i=0; i<nSubaoCount; i++)
		{
			//根据车次号判断上下行
			if(strlen(m_VectTrainsSubao[i].Train_id) == 0)
				continue;
			char otherDigit; int realNum=0;
			otherDigit= '\0';
			int trainidlen = strlen(m_VectTrainsSubao[i].Train_id);
			find=false;
			for(int m=0; m<trainidlen; m++)
			{	
				realNum = trainidlen - m -1;
				otherDigit= m_VectTrainsSubao[i].Train_id[realNum];
				if( (otherDigit>= 0x30 && otherDigit<=0x39)){
					find=true;
					break;
				}
			}
			if(!(find&&(otherDigit-'0')%2==0))
				continue;

			if(m_VectTrainsSubao[i].shift_id%2 != shiftFlag) //第一班，打在左边
				continue;			

			if(m_VectTrainsSubao[i].Print_flag != 0) //不打印
				continue;			

			lt.y = yOffset+printedNum*logUnitsPeRow;
			rb.y = lt.y + logUnitsPeRow;
			lt.x = rb.x = xOffset;

			//"序号","车次","机车1","机车2","司机","车长","辆数","计长","总重","备注"
			for(int j=0; j<sizeof(colRate)/sizeof(colRate[0])-1;j++)
			{		
				lt.x = rb.x;
				rectWidth = frameWidth*colRate[j+1];
				rb.x = lt.x + rectWidth; 
				rect.SetRect(lt,rb);
				switch (j)
				{
				case 0:
					text.Format("%d",printedNum+1);
					break;
				case 1:
					text.Format("%s",m_VectTrainsSubao[i].Train_id);
					break;
				case 2:
					text = gpcdef.GetEngineDesp(m_VectTrainsSubao[i].tract_engine_id[0]);
					break;
				case 3:
					text = gpcdef.GetEngineDesp(m_VectTrainsSubao[i].tract_engine_id[1]);
					break;				
				case 4:
					text.Format("%s",m_VectTrainsSubao[i].Driver_name);					
					break;				
				case 5:
					text.Format("%s",m_VectTrainsSubao[i].Train_leader_name);					
					break;
				case 6:
					text.Format("%d",m_VectTrainsSubao[i].K_count);			 	
					break;
				case 7:
					text.Format("%.1f",((float)m_VectTrainsSubao[i].Cal_len)/10);
					break;
				case 8:
					text.Format("%d",((int)m_VectTrainsSubao[i].Total_weight)/10);
					break;
				case 9:
					text="";
					break;
				}

				pCDC->DrawText(text,rect,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
			}
			printedNum++;			
		}
	}

	pCDC->SelectObject(pOldPen);
	pCDC->SelectObject(pOldFont);
	pCDC->SetBkMode(nOldBackMode);
	pCDC->SetTextColor(OldTextColor);
	titleFont.DeleteObject();
	mThinPen.DeleteObject();
}

void CPlot24Doc::DrawSubao24Xian(CDC *pCDC, COLORREF clrBackground, int xOffset, int yOffset, int frameWidth, int frameHeight,int shiftFlag)
{
	//2005-03-07 西安分局修改了速报的位置
	//2004-04-01
	int scaleRate = min(yMultiple,5);

	int m_nLogPixelSy = pCDC->GetDeviceCaps(LOGPIXELSY);
	//在柳局打大小图时候用的一个变量

	const int colNum = 60; //列数
	int rowNum = 0;	//行数

	char* rowTitle[] =
	{
		"序号","车次","机车1","机车2","司机","辆数","计长","总重"
	};

	rowNum = sizeof(rowTitle)/sizeof(rowTitle[0]);


	double logUnitsPerRow = (double)frameHeight / rowNum;

	int i = 0;

	CPen mThinPen,*pOldPen;
	UINT OldTextColor;
	UINT nOldBackMode;

	mThinPen.CreatePen(PS_SOLID, 1*scaleRate, clrBackground);

	pOldPen = pCDC->SelectObject(&mThinPen);
	OldTextColor = pCDC->SetTextColor(clrBackground);
	nOldBackMode = pCDC->SetBkMode(TRANSPARENT);

	//画行
	for(i=0; i<rowNum+1; i++)
	{
		pCDC->MoveTo(xOffset,yOffset+i*logUnitsPerRow);
		pCDC->LineTo(xOffset+frameWidth,yOffset+i*logUnitsPerRow);
	}

	int xOffsetRow, yOffsetRow;
	xOffsetRow = xOffset;
	yOffsetRow = yOffset;

	double logUnitsPerCol = (double)frameWidth / colNum;

	//画列
	for(i=0; i<colNum+1; i++)
	{
		pCDC->MoveTo(xOffset+i*logUnitsPerCol,yOffset);
		pCDC->LineTo(xOffset+i*logUnitsPerCol,yOffset+frameHeight);		
	}

	//画标题栏，把标题栏绘在向上填充的第一行
	CRect rect;
	CPoint lt,rb;

	//创建使用的字体
	CFont titleFont;
	LOGFONT stFont;
	CFont * pOldFont;

	memset(&stFont,0,sizeof(stFont));
	stFont.lfHeight = MulDiv(9,m_nLogPixelSy,72);
	stFont.lfWeight = FW_THIN;
	stFont.lfClipPrecision = CLIP_LH_ANGLES;
	//	strcpy(stFont.lfFaceName,"Arial");
	titleFont.CreateFontIndirect(&stFont);

	pOldFont = pCDC->SelectObject(&titleFont);

	for(i=0; i<rowNum; i++)
	{
		lt.x = xOffset ;                       
		lt.y = yOffset + i*logUnitsPerRow;;

		rb.x = lt.x + logUnitsPerCol;
		rb.y = lt.y + logUnitsPerRow;
		rect.SetRect(lt,rb);	
		pCDC->DrawText(rowTitle[i],rect,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
	}

	pCDC->SelectObject(pOldFont);
	titleFont.DeleteObject();

	memset(&stFont,0,sizeof(stFont));
	stFont.lfHeight = MulDiv(6,m_nLogPixelSy,72);
	stFont.lfWeight = FW_THIN;
	stFont.lfClipPrecision = CLIP_LH_ANGLES;
	//	strcpy(stFont.lfFaceName,"Arial");
	strcpy(stFont.lfFaceName,"宋体");
	titleFont.CreateFontIndirect(&stFont);	
	pCDC->SelectObject(&titleFont);

	pCDC->SetTextColor(CLR_BLACK);

	int nSubaoCount = m_VectTrainsSubao.size();
	if(nSubaoCount>0) //绘制受到的实际速报数据
	{
		int printedNum = 0;
		CString text;

		//pCDC->SelectObject(&titleFont);
		xOffset += logUnitsPerCol; //向下偏移一行
		for(int i=0; i<nSubaoCount; i++)
		{
			if(m_VectTrainsSubao[i].shift_id%2 != shiftFlag) //第一班，打在左边
				continue;			

			if(m_VectTrainsSubao[i].Print_flag != 0) //不打印
				continue;			

			if(printedNum >= min(nSubaoCount,colNum))
				continue;

			lt.x = xOffset + printedNum*logUnitsPerCol;
			rb.x = lt.x + logUnitsPerCol;

			lt.y = yOffset;
			rb.y = lt.y + logUnitsPerRow;


			//"序号","机车1","机车2","车次","司机","辆数","计长","总重"
			CPoint lt2,rb2;
			for(int j=0; j<rowNum;j++)
			{
				lt2 = lt;
				rb2 = rb;
				lt2.y = lt.y + j*logUnitsPerRow;
				rb2.y = rb.y + j*logUnitsPerRow;

				rect.SetRect(lt2,rb2);				


				switch (j)
				{
				case 0:
					text.Format("%d",printedNum+1);
					break;
				case 1:
					text.Format("%s",m_VectTrainsSubao[i].Train_id);
					break;
				case 2:
					text = gpcdef.GetEngineDesp(m_VectTrainsSubao[i].tract_engine_id[0]);
					break;
				case 3:
					text = gpcdef.GetEngineDesp(m_VectTrainsSubao[i].tract_engine_id[1]);
					break;				
				case 4:
					text.Format("%s",m_VectTrainsSubao[i].Driver_name);					
					break;				
				case 5:
					text.Format("%d",m_VectTrainsSubao[i].K_count);			 	
					break;
				case 6:
					text.Format("%.1f",((float)m_VectTrainsSubao[i].Cal_len)/10);
					break;
				case 7:
					text.Format("%.1f",((float)m_VectTrainsSubao[i].Total_weight)/10);					
					break;	
				}


				pCDC->DrawText(text,rect,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
			}
			//pCDC->DrawText(text,rect,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
			printedNum++;			

		}

	}

	pCDC->SelectObject(pOldPen);
	pCDC->SelectObject(pOldFont);
	pCDC->SetBkMode(nOldBackMode);
	pCDC->SetTextColor(OldTextColor);
	titleFont.DeleteObject();
	mThinPen.DeleteObject();

}

struct YTag
{
	int nTopY;
	int nBottomY;
};
YTag AixisY[128];
int CPlot24Doc::CalcAixixY()
{
	int nAixixYNum = -1;
	int routeSetNo = 9999;
	BYTE nMax = sizeof(AixisY)/sizeof(AixisY[0]);
	for(BYTE i = 0; i < nMax; i++)
	{
		AixisY[i].nTopY = 0;
		AixisY[i].nBottomY = 0;
	}
	//memset(AixisY, 0, sizeof(AixisY));

	if (m_nDispDistanceNum <= 0) // 很重要!!!
		return nAixixYNum;
	
	for(int i = 0; i < m_nDispDistanceNum; i++)
	{
		if(routeSetNo == m_sDispDistance[i].nRouteSetNo)
		{
			if(m_sDispDistance[i].distance.node_info.show_sides)
			{
				AixisY[nAixixYNum].nBottomY =  m_sDispDistance[i].nStationYPos;

				if (nAixixYNum+1 > nMax)
				{
					CString str;
					str.Format("运行图分段过多>%d", sizeof(AixisY)/sizeof(AixisY[0]));
					AfxMessageBox(str);
				}

				nAixixYNum++;
				AixisY[nAixixYNum].nTopY =  m_sDispDistance[i].nSecondStationYPos;
				AixisY[nAixixYNum].nBottomY =  m_sDispDistance[i].nSecondStationYPos;
			}
			else
			{
				AixisY[nAixixYNum].nBottomY =  m_sDispDistance[i].nSecondStationYPos;
			}
		} 
		else 
		{
			routeSetNo = m_sDispDistance[i].nRouteSetNo;

			if (nAixixYNum+1 > nMax)
			{
				CString str;
				str.Format("运行图分段过多>%d", sizeof(AixisY)/sizeof(AixisY[0]));
				AfxMessageBox(str);
			}

			nAixixYNum++;
			if(m_sDispDistance[i].distance.node_info.show_sides)
			{
				AixisY[nAixixYNum].nBottomY = m_sDispDistance[i].nSecondStationYPos;
				AixisY[nAixixYNum].nTopY = m_sDispDistance[i].nSecondStationYPos;
			}
			else
			{
				AixisY[nAixixYNum].nBottomY = m_sDispDistance[i].nSecondStationYPos;
				AixisY[nAixixYNum].nTopY = m_sDispDistance[i].nStationYPos;
			}
		}
	}

	return nAixixYNum;
}

void CPlot24Doc::DrawTimeAixis(CDC* pDC,  long lColor)
{
	int nAixixYNum = CalcAixixY();

	CPen mThinPen,mDashPen,mBoldPen,*pOldPen;

	mThinPen.CreatePen(PS_SOLID, 1, lColor);
	mDashPen.CreatePen(PS_DASH,  2, lColor);
	mBoldPen.CreatePen(PS_SOLID, 4, lColor);   //20100618时间轴打印加粗

	pOldPen = pDC->SelectObject(&mThinPen);

	int x_offset_print, y_offset_print, height, width;
	if(!GetOffsetAndSize(AREA_TYPE::TRAIN, x_offset_print, y_offset_print, height, width))
		return;

	for(int i=0; i<= m_nViewTimeRange*30 ;i+=5)
	{
		int XPos = i*m_fPixelPerTwoMinute;
		
		// meet with one hour line
		if(i % 30 == 0) 
		{
			pDC->SelectObject(&mBoldPen);	
		}
		// meet with half hour line
		else if(i % 15 == 0) 
		{
			pDC->SelectObject(&mDashPen);
		}
		// meet with ten or two minute line
		else if(i % 5 == 0)
		{ 
			pDC->SelectObject(&mThinPen);
		}
		else 
			continue;
	
		for(int k = 0; k <= nAixixYNum; k++)
		{
			pDC->MoveTo(XPos+x_offset_print, AixisY[k].nTopY + y_offset_print);			
			pDC->LineTo(XPos+x_offset_print, AixisY[k].nBottomY + y_offset_print);
		}
	}

	pDC->SelectObject(pOldPen);
	mThinPen.DeleteObject();
	mDashPen.DeleteObject();
	mBoldPen.DeleteObject();
}

void CPlot24Doc::DrawLanzhouDDYName12(CDC *pCDC, COLORREF clr)
{
	int scaleRate = min(yMultiple,5);
	int nLogPixelSy = pCDC->GetDeviceCaps(LOGPIXELSY);

	//设置各种属性
	CPen mThickPen;
	mThickPen.CreatePen(PS_SOLID,(int)(1*scaleRate),clr);
	int nOldBackMode = pCDC->SetBkMode(TRANSPARENT);
	CPen* pOldPen = pCDC->SelectObject(&mThickPen);
	COLORREF OldTextColor = pCDC->SetTextColor(clr);
	

	int xOffset,yOffset;
	
	int offset_x, offset_y, height, width;
	if(!GetOffsetAndSize(AREA_TYPE::NAME, offset_x, offset_y, height, width))
		return;

	CRect drawRect;
	drawRect.top    = offset_y;
	drawRect.bottom = offset_y + height;
	drawRect.left   = offset_x;    
	drawRect.right  = offset_x + width;	

	pCDC->Rectangle(&drawRect);
	xOffset = offset_x;
	yOffset = offset_y;

	pCDC->MoveTo(drawRect.left,  drawRect.top+drawRect.Height()/4);    
	pCDC->LineTo(drawRect.right, drawRect.top+drawRect.Height()/4);

	pCDC->MoveTo(drawRect.left,  drawRect.top+drawRect.Height()/2);  
	pCDC->LineTo(drawRect.right, drawRect.top+drawRect.Height()/2);

	pCDC->MoveTo(drawRect.left,  drawRect.top+3*drawRect.Height()/4);  
	pCDC->LineTo(drawRect.right, drawRect.top+3*drawRect.Height()/4);

	pCDC->MoveTo(drawRect.left+drawRect.Width()/2, drawRect.top+drawRect.Height()/4);  
	pCDC->LineTo(drawRect.left+drawRect.Width()/2, drawRect.bottom);
	
	// 时间 姓名
	CString time1, time2, time3, name1="", name2="", name3="";
	if(m_nShiftNo == 1)
	{
		time1 = "18点--20点";
		time2 = "20点--8点";
		time3 = "8点--18点";
		name1 = this->m_ddyName.name1;
		name2 = this->m_ddyName.name2;
	}
	else
	{
		time1 = "6点--8点";
		time2 = "8点--20点";
		time3 = "20点--6点";
		name1 = this->m_ddyName.name1;
		name2 = this->m_ddyName.name2;
	}
	
	//创建使用的字体
	CFont textFont;
	LOGFONT stFont;
	
	memset(&stFont,0,sizeof(stFont));
	stFont.lfHeight = MulDiv(12,nLogPixelSy,72);	 
	stFont.lfWeight = FW_NORMAL;
	stFont.lfClipPrecision = CLIP_LH_ANGLES;
	textFont.CreateFontIndirect(&stFont);

	CFont *pOldFont = pCDC->SelectObject(&textFont);
		
	UINT nFormat = DT_VCENTER|DT_SINGLELINE|DT_CENTER;
	// 值班列车调度员
	CRect rect;
	rect.SetRect(drawRect.left, drawRect.top, drawRect.right, drawRect.top + drawRect.Height()/4);
	rect.InflateRect(-1,-1,-1,-1);
	pCDC->DrawText("值班列车调度员",rect, nFormat);

	// time1--name1
	rect.SetRect(drawRect.left,                    
								drawRect.top+drawRect.Height()/4, 
				 drawRect.left+drawRect.Width()/2, 
				                drawRect.top + drawRect.Height()/2);
	rect.InflateRect(-1,-1,-1,-1);
	pCDC->DrawText(time1,rect, nFormat);

	rect.SetRect(drawRect.left+drawRect.Width()/2, 
								drawRect.top+drawRect.Height()/4, 
				 drawRect.right, 
								drawRect.top + drawRect.Height()/2);
	rect.InflateRect(-1,-1,-1,-1);
	pCDC->DrawText(name1,rect, nFormat);

	// time2--name2
	rect.SetRect(drawRect.left, 
								drawRect.top+drawRect.Height()/2, 
				 drawRect.left+drawRect.Width()/2, 
				                drawRect.top + 3*drawRect.Height()/4);
	rect.InflateRect(-1,-1,-1,-1);
	pCDC->DrawText(time2,rect, nFormat);

	rect.SetRect(drawRect.left+drawRect.Width()/2, 
								drawRect.top+drawRect.Height()/2, 
				 drawRect.right, 
								drawRect.top + 3*drawRect.Height()/4);
	rect.InflateRect(-1,-1,-1,-1);
	pCDC->DrawText(name2,rect, nFormat);

	// time3--name3
	rect.SetRect(drawRect.left, 
								drawRect.top+3*drawRect.Height()/4, 
				 drawRect.left+drawRect.Width()/2, 
				                drawRect.bottom);
	rect.InflateRect(-1,-1,-1,-1);
	pCDC->DrawText(time3,rect, nFormat);

	rect.SetRect(drawRect.left+drawRect.Width()/2, 
		                       drawRect.top+3*drawRect.Height()/4, 
				 drawRect.right, 
				               drawRect.bottom);
	rect.InflateRect(-1,-1,-1,-1);
	pCDC->DrawText(name3,rect, nFormat);
	

	pCDC->SelectObject(pOldFont);
	pCDC->SetTextColor(OldTextColor);
	pCDC->SelectObject(pOldPen);
	pCDC->SetBkMode(nOldBackMode);
	mThickPen.DeleteObject();
	textFont.DeleteObject();

	return;
}

void CPlot24Doc::DrawDDYName12(CDC *pCDC, COLORREF clrBackground)
{
	int scaleRate = min(yMultiple,5);
	int nLogPixelSy = pCDC->GetDeviceCaps(LOGPIXELSY);

	int xOffset,yOffset;
	//int width,height;

	int offset_x, offset_y, height, width;
	if(!GetOffsetAndSize(AREA_TYPE::NAME, offset_x, offset_y, height, width))
		return;

	xOffset = offset_x;
	yOffset = offset_y;

	CString title = "值 班 调 度 员：";

	CString dispatcher;
	dispatcher.Empty();

	if(m_nShiftNo == 1)
	{
		dispatcher += "18点--20点：";
		dispatcher += this->m_ddyName.name1;
		dispatcher += "     ";
		dispatcher += "20点--6点：";
		dispatcher += this->m_ddyName.name2;
	}
	else
	{
		dispatcher += "6点--8点：";
		dispatcher += this->m_ddyName.name1;
		dispatcher += "     ";
		dispatcher += "8点--18点：";
		dispatcher += this->m_ddyName.name2;
	}

	title += dispatcher;


	CRect rect;
	rect.SetRect(xOffset,yOffset,xOffset+width,yOffset+height);
	rect.InflateRect(-5,-5,-5,-5);
	CBrush bkBrush(CLR_WHITE);
	pCDC->FillRect(rect,&bkBrush);		

	//创建使用的字体
	CFont titleFont;
	LOGFONT stFont;
	CFont * pOldFont;

	memset(&stFont,0,sizeof(stFont));
	stFont.lfHeight = MulDiv(14,nLogPixelSy,72);	 
	stFont.lfWeight = FW_BOLD;
	stFont.lfClipPrecision = CLIP_LH_ANGLES;
	titleFont.CreateFontIndirect(&stFont);
	pOldFont = pCDC->SelectObject(&titleFont);

	UINT OldTextColor = pCDC->SetTextColor(CLR_BLACK);	
	pCDC->DrawText(title,rect,DT_LEFT|DT_SINGLELINE|DT_VCENTER);

	pCDC->SetTextColor(OldTextColor);
	pCDC->SelectObject(pOldFont);

	titleFont.DeleteObject();
}
void CPlot24Doc::DrawDDYName24(CDC *pCDC, COLORREF clr)
{
	if(m_printFormat == 1)
	{
		if(this->mPlotStyle.id == 5)
			DrawLanzhouDDYName12(pCDC,clr);
		else
			DrawDDYName12(pCDC,clr);
		return;
	}
	//2004-04-01
	int scaleRate = min(yMultiple,5);

	int m_nLogPixelSy = pCDC->GetDeviceCaps(LOGPIXELSY);

	int xOffset, yOffset, frameHeight, frameWidth;
	if(!GetOffsetAndSize(AREA_TYPE::NAME, xOffset, yOffset, frameHeight, frameWidth))
		return;

	char* colTitle[] =
	{
		"18点--20点","20点--8点","8点--18点"
	};
	char bitTitle[] = "值 班 调 度 员 姓 名";

	sysprint(5,PMTG.ThreadId,"打印24小时图前班值班调度员姓名:%d  %s %s", this->m_ddyName_Pre.nShiftId, this->m_ddyName_Pre.name1, this->m_ddyName_Pre.name2);
	sysprint(5,PMTG.ThreadId,"打印24小时图本版值班调度员姓名:%d  %s %s", this->m_ddyName.nShiftId, this->m_ddyName.name1, this->m_ddyName.name2);
	
	int logUnitsPerRow,logUnitsPerCol; //每行每列所占的逻辑单位
	//总共是三行三列
	logUnitsPerCol = frameWidth/3;
	logUnitsPerRow = 0.8*frameHeight;


	CPen mThinPen,*pOldPen;
	mThinPen.CreatePen(PS_SOLID, 1*scaleRate, clr);

	UINT OldTextColor;
	UINT nOldBackMode;
	pOldPen = pCDC->SelectObject(&mThinPen);
	OldTextColor = pCDC->SetTextColor(clr);
	nOldBackMode = pCDC->SetBkMode(TRANSPARENT);

	for(int i=1; i<=3; i++) //第一行不画，调度员姓名这个标题不需要边框，共三条横线
	{
		pCDC->MoveTo(xOffset,yOffset+i*logUnitsPerRow);
		pCDC->LineTo(xOffset+frameWidth,yOffset+i*logUnitsPerRow);
	}

	//四条竖线

	int xOffsetRow, yOffsetRow;
	xOffsetRow = xOffset;
	yOffsetRow = yOffset + logUnitsPerRow;

	//不画最右边一列，因为可能存在偏差
	for(i=0; i<3; i++)
	{
		pCDC->MoveTo(xOffsetRow+i*logUnitsPerCol,yOffsetRow);
		pCDC->LineTo(xOffsetRow+i*logUnitsPerCol,yOffsetRow+2*logUnitsPerRow);		
	}

	//补画最右边一列
	pCDC->MoveTo(xOffset+frameWidth,yOffsetRow);
	pCDC->LineTo(xOffset+frameWidth,yOffsetRow+2*logUnitsPerRow);	

	//画标题栏，把标题栏绘第二行
	CRect rect;
	CPoint lt,rb;
	int rectWidth;
	lt.x = xOffset;
	lt.y = rb.y = yOffset+logUnitsPerRow;
	rb.y += logUnitsPerRow;

	//创建使用的字体
	CFont titleFont;
	LOGFONT stFont;
	CFont * pOldFont;

	memset(&stFont,0,sizeof(stFont));
	stFont.lfHeight = MulDiv(12,m_nLogPixelSy,72);	 
	stFont.lfWeight = FW_THIN;
	stFont.lfClipPrecision = CLIP_LH_ANGLES;
	//	strcpy(stFont.lfFaceName,"Arial");
	titleFont.CreateFontIndirect(&stFont);

	pOldFont = pCDC->SelectObject(&titleFont);


	for(i=0; i<3; i++)
	{		
		rectWidth = logUnitsPerCol;
		rb.x = lt.x + rectWidth;
		rect.SetRect(lt,rb);
		pCDC->DrawText(colTitle[i],rect,DT_CENTER|DT_SINGLELINE|DT_VCENTER);
		lt.x += logUnitsPerCol;	
	}


	pCDC->SetTextColor(CLR_BLACK);
	lt.x = xOffset;
	lt.y = rb.y = yOffset+2*logUnitsPerRow;
	rb.y += logUnitsPerRow;
	//输出调度员姓名
	for(i=0; i<3; i++)
	{		
		rectWidth = logUnitsPerCol;
		rb.x = lt.x + rectWidth;
		rect.SetRect(lt,rb);
		/*
		switch (i)
		{
		case 0: //第一班调度员
			pCDC->DrawText(this->m_ddyName.name1, rect, DT_CENTER|DT_SINGLELINE|DT_VCENTER);
			break;
		case 1:
			if(this->m_ddyName.name2.IsEmpty())
				this->m_ddyName.name2 = this->m_ddyName.name1;
			pCDC->DrawText(this->m_ddyName.name2,rect,DT_CENTER|DT_SINGLELINE|DT_VCENTER);
			break;
		case 2:
			pCDC->DrawText(this->m_ddyName.name2,rect,DT_CENTER|DT_SINGLELINE|DT_VCENTER);
			break;
		default:
			break;
		}
		*/
		switch (i)
		{
		case 0: //第一班调度员
			pCDC->DrawText(this->m_ddyName_Pre.name1, rect, DT_CENTER|DT_SINGLELINE|DT_VCENTER);
			break;
		case 1:
			{
				CString name;
				CString name1=this->m_ddyName_Pre.name2;
				CString name2=this->m_ddyName.name1;
				if(name1==name2)
					name=name1;
				else
				{
					name=name1;
					if(name=="")
						name=name2;
					else if(name2!="")
						name+="\\"+name2;
				}

				pCDC->DrawText(name,rect,DT_CENTER|DT_SINGLELINE|DT_VCENTER);
			}
			break;
		case 2:
			pCDC->DrawText(this->m_ddyName.name2,rect,DT_CENTER|DT_SINGLELINE|DT_VCENTER);
			break;
		default:
			break;
		}
		lt.x += logUnitsPerCol;	
	}
	pCDC->SetTextColor(clr);

	CFont bigTitleFont;
	memset(&stFont,0,sizeof(stFont));
	stFont.lfHeight = MulDiv(16,m_nLogPixelSy,72);	 
	stFont.lfWeight = FW_BOLD;
	stFont.lfClipPrecision = CLIP_LH_ANGLES;
	bigTitleFont.CreateFontIndirect(&stFont);

	pCDC->SelectObject(pOldFont);
	pOldFont = pCDC->SelectObject(&bigTitleFont);

	CPoint titleCenter;
	titleCenter.x = xOffset + frameWidth/2;
	titleCenter.y = yOffset+10;
	UINT oldTextAlign;

	oldTextAlign = pCDC->SetTextAlign(TA_CENTER|TA_BASELINE);
	pCDC->TextOut(titleCenter.x,titleCenter.y,bitTitle);

	pCDC->SetTextAlign(oldTextAlign);	

	pCDC->SelectObject(pOldPen);
	pCDC->SelectObject(pOldFont);
	pCDC->SetBkMode(nOldBackMode);
	pCDC->SetTextColor(OldTextColor);

	bigTitleFont.DeleteObject();
	titleFont.DeleteObject();
	mThinPen.DeleteObject();
}

void CPlot24Doc::DrawPhaseStoreTrainFrame24(CDC *pCDC, COLORREF clr)
{
	if(!IsPrintPhaseCar(m_sPrintInform->usSectionNo))
		return;
	//2004-04-01
	int scaleRate = min(yMultiple,5);

	CRect drawRect;
	int i;

	UINT nOldAlign;
	CPen mThickPen;
	CPen *pOldPen;
	UINT OldTextColor;
	CRect contentRect;
	int m_nLogPixelSy = pCDC->GetDeviceCaps(LOGPIXELSY);

	LOGFONT stFont;
	CFont mFont,mFont2;
	CFont *pOldFont;
	CSize strExtent;

	//设置各种属性
	mThickPen.CreatePen(PS_SOLID,(int)(1*scaleRate),clr);
	nOldAlign = pCDC->SetTextAlign(TA_LEFT|TA_TOP);
	pOldPen = pCDC->SelectObject(&mThickPen);
	OldTextColor = pCDC->SetTextColor(clr);

	//2005-03-08根据打印格式，24小时打印4个阶段，12小时打印三个阶段
	int offset_x, offset_y, height, width;
	if(!GetOffsetAndSize(AREA_TYPE::CAR, offset_x, offset_y, height, width))
		return;

	drawRect.top    = offset_y;
	drawRect.bottom = offset_y + height;  
	drawRect.left   = offset_x;
	drawRect.right  = offset_x + width;

	int phaseCount = 4;

	if(m_printFormat == 1)
		phaseCount = 3;
	else
		phaseCount = 4;

	memset(&stFont,0,sizeof(stFont));
	stFont.lfHeight = MulDiv(16,m_nLogPixelSy,72);	 
	stFont.lfWeight = FW_BOLD;
	stFont.lfClipPrecision = CLIP_LH_ANGLES;
	mFont.CreateFontIndirect(&stFont);

	pOldFont = pCDC->SelectObject(&mFont);

	CRect titleRect = drawRect;
	CPoint titleCenter = drawRect.CenterPoint();
	titleCenter.y = drawRect.top - 10*fHeightMulti;

	UINT oldTextAlign;
	oldTextAlign = pCDC->SetTextAlign(TA_CENTER|TA_BOTTOM);
	pCDC->TextOut(titleCenter.x,titleCenter.y,"站 存 车");

	pCDC->SetTextAlign(oldTextAlign);

	pCDC->Rectangle(&drawRect);

	pCDC->MoveTo(drawRect.left  , drawRect.top +80*fHeightMulti);
	pCDC->LineTo(drawRect.right , drawRect.top +80*fHeightMulti);


	CString str_text;

	memset(&stFont,0,sizeof(stFont));
	stFont.lfHeight = MulDiv(12,m_nLogPixelSy,72);	 
	stFont.lfWeight = FW_THIN;
	stFont.lfClipPrecision = CLIP_LH_ANGLES;
	mFont2.CreateFontIndirect(&stFont);

	pCDC->SelectObject(pOldFont);	
	pOldFont = pCDC->SelectObject(&mFont2);

	str_text = "站名";
	strExtent = pCDC->GetTextExtent(str_text);

	////wangjinhai ,old is 3
	int x_first = 2*strExtent.cx; //车站名称宽度

	m_phaseStationWidth = x_first;

	int y_first =  80*fHeightMulti;

	int x_step = (drawRect.Width() -x_first )/phaseCount ;

	//draw columns 
	for(i=0;i<phaseCount;i++)
	{
		pCDC->MoveTo(drawRect.left +x_first +x_step *i,drawRect.top);
		pCDC->LineTo(drawRect.left +x_first +x_step *i,drawRect.bottom);
	}

	//获得每个车站的打印比例

	int nPrintNum;
	float nStationPrintRate[128];

	nPrintNum = sizeof(nStationPrintRate)/sizeof(nStationPrintRate[0]);
	gpcdef.GetSectionPhaseStorePrintRate(m_nCurrentSectionNo,nPrintNum,nStationPrintRate);

	int nStationNum;
	int nStationList[128];
	nStationNum=sizeof(nStationList)/sizeof(nStationList[0]);
	gpcdef.GetSectionPhaseStoreStationList(m_nCurrentSectionNo,nStationNum,nStationList);

	if(nPrintNum != nStationNum)
	{
		sysprint(0,PMTG.ThreadId,"section %d printrate num %d is not same as station num %d",m_nCurrentSectionNo,nPrintNum,nStationNum);
		//return;
	}

	if(nStationNum == 0)
	{
		CString msg;
		msg.Format("gpc.def中找不到区段%d的[SECTION_PHASE_STORE_STATION]配置", m_nCurrentSectionNo);
		AfxMessageBox(msg);

		return;
	}
	//缺省的车站个数
	int nRowCount = nStationNum;
	int y_step = (drawRect.Height() -y_first)/nRowCount;

	//画行
	float totalRate = 0.0;
	for(i=0;i<nRowCount-1;i++)
	{
		totalRate += nStationPrintRate[i];

		pCDC->MoveTo(drawRect.left , drawRect.top +y_first +totalRate*(drawRect.Height()-y_first));
		pCDC->LineTo(drawRect.right, drawRect.top +y_first +totalRate*(drawRect.Height()-y_first));
	}
	//draw bias line;
	pCDC->MoveTo( drawRect.left, drawRect.top);
	pCDC->LineTo( drawRect.left + x_first, drawRect.top +y_first);

	contentRect = drawRect;
	contentRect.left = contentRect.left + 10*fWidthMulti;
	contentRect.top = contentRect.top + 10 * fHeightMulti;
	contentRect.bottom = drawRect.top + (y_first-10*fHeightMulti);
	contentRect.right = drawRect.left + (x_first-10*fWidthMulti);

	pCDC->DrawText("站名",&contentRect,DT_BOTTOM|DT_LEFT|DT_SINGLELINE|DT_WORDBREAK|DT_EXTERNALLEADING|DT_NOCLIP);
	pCDC->DrawText("时间",&contentRect,DT_TOP|DT_RIGHT|DT_SINGLELINE|DT_WORDBREAK|DT_EXTERNALLEADING|DT_NOCLIP);
	int nYear=m_sPrintInform->usShiftid/1000000;
	int nMonth=(m_sPrintInform->usShiftid-nYear*1000000 )/10000;
	int nDay=(m_sPrintInform->usShiftid-nYear*1000000-nMonth*10000 )/100;
	unsigned long banbie=m_sPrintInform->usShiftid-nYear*1000000-nMonth*10000-nDay*100;
	CString str,str_1,str_2,str_3,str_4;

	//一班18、0、6；二班6、12、18
	if(m_printFormat == 1)//十二小时打印
	{
		if(m_nShiftNo == 1)
		{
			str_1.Format("十八点"); str_2.Format("零 点");
			str_3.Format("六 点"); str_4.Empty();
		}
		else
		{
			str_1.Format("六 点"); str_2.Format("十二点");
			str_3.Format("十八点"); str_4.Empty();

		}
	}
	else//二十四小时打印
	{
		str_1.Format("十八点"); str_2.Format("零 点");
		str_3.Format("六 点"); str_4.Format("十二点");
	}

	contentRect = drawRect;
	contentRect.bottom = drawRect.top + y_first;

	for(i=0;i<phaseCount;i++)
	{
		contentRect.left = drawRect.left + x_first + i*x_step;	 
		contentRect.right =  contentRect.left  + x_step;

		if(i==0)
			str = str_1;
		else if(i==1)
			str = str_2;
		else if(i==2)
			str= str_3;
		else
			str = str_4;
		pCDC->DrawText(str,&contentRect,DT_VCENTER|DT_CENTER|DT_SINGLELINE|DT_WORDBREAK|DT_EXTERNALLEADING|DT_NOCLIP);

	}
	//show station name;

	contentRect = drawRect;
	contentRect.left = drawRect.left;	 
	contentRect.right =  contentRect.left  + x_first;

	totalRate = 0.0;
	for(i=0;i<nStationNum;i++)
	{	
		contentRect.top    = drawRect.top + y_first + totalRate*(drawRect.Height()-y_first);
		contentRect.bottom = contentRect.top  + nStationPrintRate[i]*(drawRect.Height()-y_first);

		totalRate += nStationPrintRate[i];

		str = gpcdef.GetStationName(nStationList[i]);
		if(str.GetLength()>8)
			str = str.Left(8);
		pCDC->DrawText(str,&contentRect,DT_VCENTER|DT_CENTER|DT_SINGLELINE|DT_WORDBREAK|DT_EXTERNALLEADING|DT_NOCLIP);
	}

	pCDC->SelectObject(pOldFont);
	pCDC->SetTextAlign(nOldAlign);
	pCDC->SelectObject(pOldPen);
	pCDC->SetTextColor(OldTextColor);

	mFont.DeleteObject();
	mFont2.DeleteObject();	
	mThickPen.DeleteObject();
}

void CPlot24Doc::ShowPhaseStoreTrain24(CDC *pCDC, COLORREF clr, int nStationNo, int phase)
{
	std::map<int, CString>::iterator it;
	switch(phase)
	{
	case CLBTH::MsgSimpleStationCarCtrl::STACAR_PHASE_0:
		{
			it = this->m_car_phase_0.find(nStationNo);
			if(it != this->m_car_phase_0.end())
				this->ShowPhaseStoreTrain24(pCDC, clr, nStationNo, 0, it->second);
		}
		break;
	case CLBTH::MsgSimpleStationCarCtrl::STACAR_PHASE_6:
		{
			it = this->m_car_phase_6.find(nStationNo);
			if(it != this->m_car_phase_6.end())
				this->ShowPhaseStoreTrain24(pCDC, clr, nStationNo, 6, it->second);
		}
		break;
	case CLBTH::MsgSimpleStationCarCtrl::STACAR_PHASE_12:
		{
			it = this->m_car_phase_12.find(nStationNo);
			if(it != this->m_car_phase_12.end())
				this->ShowPhaseStoreTrain24(pCDC, clr, nStationNo, 12, it->second);
		}
		break;
	case CLBTH::MsgSimpleStationCarCtrl::STACAR_PHASE_18:
		{
			it = this->m_car_phase_18.find(nStationNo);
			if(it != this->m_car_phase_18.end())
				this->ShowPhaseStoreTrain24(pCDC, clr, nStationNo, 18, it->second);
		}
		break;			
	}
}

void CPlot24Doc::ShowPhaseStoreTrain24(CDC *pCDC, COLORREF clr, int nStationNo, int phase, CString &stext)
{
	CRect drawRect;

	UINT nOldAlign;
	UINT OldTextColor;

	CRect contentRect;
	int m_nLogPixelSy = pCDC->GetDeviceCaps(LOGPIXELSY);

	//设置各种属性	
	nOldAlign = pCDC->SetTextAlign(TA_LEFT|TA_TOP);
	OldTextColor = pCDC->SetTextColor(clr);

	LOGFONT stFont;
	CFont mFont;
	CFont *pOldFont;
	memset(&stFont,0,sizeof(stFont));
	stFont.lfHeight = MulDiv(8,m_nLogPixelSy,72);	 
	stFont.lfWeight = FW_THIN;//FW_NORMAL;
	stFont.lfClipPrecision = CLIP_LH_ANGLES;
	strcpy(stFont.lfFaceName,"宋体");
	mFont.CreateFontIndirect(&stFont);
	pOldFont = pCDC->SelectObject(&mFont);

	//2005-03-08根据打印格式，24小时打印4个阶段，12小时打印三个阶段
	int phaseCount = 4;

	int offset_x, offset_y, height, width;
	if(!GetOffsetAndSize(AREA_TYPE::CAR, offset_x, offset_y, height, width))
		return;

	drawRect.top    = offset_y;
	drawRect.bottom = offset_y + height;  
	drawRect.left   = offset_x;
	drawRect.right  = offset_x + width;

	if(m_printFormat == 1)
		phaseCount = 3;
	else
		phaseCount = 4;	


	int x_first = m_phaseStationWidth; //车站名称宽度


	//int x_first = 0.2*drawRect.Width();
	int y_first =  80*fHeightMulti;

	int x_step = (drawRect.Width() -x_first )/phaseCount ;

	//获得每个车站的打印比例

	int nPrintNum;
	float nStationPrintRate[128];

	nPrintNum = sizeof(nStationPrintRate)/sizeof(nStationPrintRate[0]);
	gpcdef.GetSectionPhaseStorePrintRate(m_nCurrentSectionNo,nPrintNum,nStationPrintRate);

	int nStationNum;
	int nStationList[128];
	nStationNum=sizeof(nStationList)/sizeof(nStationList[0]);
	gpcdef.GetSectionPhaseStoreStationList(m_nCurrentSectionNo,nStationNum,nStationList);


	CRect temp = drawRect;
	temp.right = temp.left + x_first;
	temp.bottom = temp.top + y_first;

	//缺省的车站个数
	int nRowCount = nStationNum;
	int y_step = (drawRect.Height() -y_first)/nRowCount;


	int nYear=m_sPrintInform->usShiftid/1000000;
	int nMonth=(m_sPrintInform->usShiftid-nYear*1000000 )/10000;
	int nDay=(m_sPrintInform->usShiftid-nYear*1000000-nMonth*10000 )/100;
	unsigned long banbie=m_sPrintInform->usShiftid-nYear*1000000-nMonth*10000-nDay*100;
	CString str,str_1,str_2,str_3,str_4;
	int j;

	if(m_printFormat == 1)//十二小时
	{
		if(m_nShiftNo == 1)
		{
			if(phase == 18)
				j = 0;
			else if(phase == 0)
				j = 1;
			else if(phase == 6)
				j = 2;
			else//不用
			{
				j = 0;
				stext.Empty();
			}

		}
		else
		{
			if(phase ==6)
				j=0;
			else if(phase ==12)
				j=1;
			else if(phase == 18)
				j=2;
			else//不用
			{
				j = 0;
				stext.Empty();
			}
		}
	}
	else//二十四小时打印
	{
		str_1.Format("十八点"); str_2.Format("零 点"); 
		str_3.Format("六 点");str_4.Format("十二点");
		if(phase ==18)
			j=0;
		else if(phase ==0)
			j=1;
		else if(phase == 6)
			j=2;
		else
			j=3;

	}
	

	float totalRate = 0.0;

	contentRect = drawRect;
	contentRect.left = drawRect.left + x_first + x_step*j;
	contentRect.right =  contentRect.left  +  x_step;

	for(int i=0;i<nStationNum;i++)
	{

		//totalRate += nStationPrintRate[i];

		if(nStationList[i] != nStationNo)
		{
			totalRate += nStationPrintRate[i];
			continue;		
		}

		contentRect.top    = drawRect.top + y_first + totalRate*(drawRect.Height()-y_first);
		contentRect.bottom = contentRect.top  + nStationPrintRate[i]*(drawRect.Height()-y_first);		


		//contentRect.top    = drawRect.top + y_first + i*y_step;
		//contentRect.bottom = contentRect.top  + y_step;	

		str =  stext;
		contentRect.InflateRect(-5*fWidthMulti,-5*fHeightMulti);
		pCDC->DrawText(str,&contentRect, DT_VCENTER|DT_CENTER|DT_WORDBREAK|DT_EXTERNALLEADING|DT_NOCLIP);

		totalRate += nStationPrintRate[i];

	}


	pCDC->SelectObject(pOldFont);
	pCDC->SetTextAlign(nOldAlign);
	pCDC->SetTextColor(OldTextColor);

	mFont.DeleteObject();

}
void CPlot24Doc::ChangeDocumentArea(int nWidth ,int nHeight )
{
	//	double fWidthMulti,fHeightMulti;

	if(m_nSchdStatus != BASIC_SCHEDULE)
	{
		if(m_printFormat == 1)
		{
			fWidthMulti = (double)nWidth / g_zz_total_page_width;  
			fHeightMulti = (double)nHeight / g_zz_total_page_height;
		}
		else
		{
			fWidthMulti = (double)nWidth / g_hh_total_page_width;  
			fHeightMulti = (double)nHeight / g_hh_total_page_height;		
		}
	}
	else
	{
		fWidthMulti = (double)nWidth / g_basic_total_page_width;  
		fHeightMulti = (double)nHeight / g_basic_total_page_height;
	}


	xMultiple = fWidthMulti;
	yMultiple = fHeightMulti;  

	station_axis_width = (int) (STATION_AXIS_WIDTH * fWidthMulti);
	header_title_height = (int)(HEADER_TITLE_HEIGHT * fHeightMulti);
	draw_mark_height = (int)(DRAW_MARK_HEIGHT * fHeightMulti);
	distance_text_width = (int)(DISTANCE_TEXT_WIDTH * fWidthMulti);
	footer_title_height = (int)(FOOTER_TITLE_HEIGHT * fHeightMulti);
	train_inform_height = (int)(TRAIN_INFORM_HEIGHT * fHeightMulti);

	single_train_height = (int)(SINGLE_TRAIN_HEIGHT * fHeightMulti);                 //单列编组高度
	single_train_inform_width = (int)(SINGLE_TRAIN_INFORM_WIDTH * fWidthMulti);      //编组宽度

	step_of_side = (int)(m_space.step_of_side * fHeightMulti);
	
	vertical_route_space = (int)(m_space.vertical_route_space * fHeightMulti);
	train_mark_height = (int)(m_space.train_mark_height * fHeightMulti);
	time_axis_height = (int)(TIME_AXIS_HEIGHT * fHeightMulti);

	int height, width;
		if(!GetOffsetAndSize(AREA_TYPE::TRAIN, x_offset_print, y_offset_print, height, width))
		{
			x_offset_print = 70;
			y_offset_print = 70;
		}

	if(m_nSchdStatus != BASIC_SCHEDULE)
	{
		if(m_printFormat == 1)
		{
			m_nPageWidth = g_zz_total_page_width* fWidthMulti;     //打印页码的总宽度
			m_nPageHeight = g_zz_total_page_height* fHeightMulti;  //总高度
		}
		else
		{
			m_nPageWidth = g_hh_total_page_width* fWidthMulti;     //打印页码的总宽度
			m_nPageHeight = g_hh_total_page_height* fHeightMulti;  //总高度
		}

		
	}
	else
	{
		m_nPageWidth = g_basic_total_page_width* fWidthMulti;     //打印页码的总宽度
		m_nPageHeight = g_basic_total_page_height* fHeightMulti;  //总高度
	}

	InitStationVerticalPos(m_nSchdStatus);
	InitTimeUnit();

	for(int i=0; i<8;i++)
		nColumnWidth[i] = m_nSubaoWidth[i]*xMultiple;
}

void CPlot24Doc::DrawPhasePlanFrame(CDC *pDC, LONG lColor, CRect *pRect)
{
	if(this->mPlotStyle.id == 5)
		return;

	int scaleRate = min(yMultiple,5);

	int i,nWidth;
	int m_nLogPixelSy = pDC->GetDeviceCaps(LOGPIXELSY);

	LOGFONT stFont;
	CFont mFont;
	CFont *pOldFont;
	memset(&stFont,0,sizeof(stFont));
	stFont.lfHeight = MulDiv(12,m_nLogPixelSy,72);	 
	stFont.lfWeight = FW_THIN;
	stFont.lfClipPrecision = CLIP_LH_ANGLES;
	mFont.CreateFontIndirect(&stFont);
	pOldFont = pDC->SelectObject(&mFont);

	UINT nOldAlign,nOldBackMode;
	CPen mThickPen;

	CPen *pOldPen;
	int xPos;
	int xPos_minor;

	CString str_text,strCommander,strSendTime;
	str_text.Format("气候");
	strCommander = "批准人/时间";
	strSendTime = "下达时间";

	mThickPen.CreatePen(PS_SOLID,(int)(1*scaleRate),lColor);

	nOldAlign = pDC->SetTextAlign(TA_LEFT|TA_TOP);
	nOldBackMode = pDC->SetBkMode(TRANSPARENT);
	pOldPen = pDC->SelectObject(&mThickPen);

	UINT OldTextColor = pDC->SetTextColor(lColor);

	CRect phaseRect;

	int offset_x, offset_y, height, width;
	if(!GetOffsetAndSize(AREA_TYPE::PHASE, offset_x, offset_y, height, width))
		return;
	phaseRect.top = offset_y;
	phaseRect.bottom = offset_y + height;	

	phaseRect.left = offset_x;
	phaseRect.right = offset_x + width;


	int phaseCount = 0;
	if(m_printFormat == 1)
		phaseCount = 3;
	else
		phaseCount = 6;

	nWidth = phaseRect.Width(); //m_fPixelPerMinute * m_nViewTimeRange*60; 

	//first draw frame  of phase;
	pDC->Rectangle(phaseRect);

	int xoffset, yoffset;
	xoffset = phaseRect.left;
	yoffset = phaseRect.top;

	int xwidth, yheight;
	xwidth = phaseRect.Width();
	yheight = phaseRect.Height();

	CSize size_text ;
	size_text = pDC->GetTextExtent(str_text);

	//initial the width of the threee phase
	xPos = nWidth / phaseCount;
	xPos_minor = xPos - 2 *size_text.cx;
	xPos_minor = xPos_minor / 4;

	CRect tempRect;
	tempRect = phaseRect;	

	//draw two horizen lines 
	pDC->MoveTo(xoffset, yoffset+ yheight*1/5);
	pDC->LineTo(xoffset + nWidth, yoffset + yheight*1/5);

	pDC->MoveTo(xoffset , yoffset + yheight*4/5);
	pDC->LineTo(xoffset + nWidth , yoffset +  yheight*4/5);


	//draw three phase
	for(i =0;i<phaseCount;i++)
	{
		pDC->MoveTo(xoffset + i * xPos , yoffset);
		pDC->LineTo(xoffset + i * xPos , yheight + yoffset);

		pDC->MoveTo(xoffset + i * xPos + 2 * size_text.cx , yoffset);
		pDC->LineTo(xoffset + i * xPos + 2 * size_text.cx , yheight+ yoffset);

		tempRect.left = xoffset + i * xPos ;
		tempRect.right = tempRect.left + 2 * size_text.cx;
		tempRect.top = yoffset + yheight*1/5;
		tempRect.bottom = tempRect.top +yheight*3/5/2;
		pDC->DrawText("阶段",tempRect,DT_CENTER|DT_NOCLIP|DT_VCENTER|DT_SINGLELINE);
		tempRect.OffsetRect(0,yheight*3/5/2);
		pDC->DrawText("记事",tempRect,DT_CENTER|DT_NOCLIP|DT_VCENTER|DT_SINGLELINE);

		for(int j= 0;j<3;j++)
		{
			pDC->MoveTo(xoffset + (j+1) * xPos_minor + i * xPos + 2 * size_text.cx , yoffset);
			pDC->LineTo(xoffset + (j+1) * xPos_minor + i * xPos + 2 * size_text.cx , yoffset + yheight*1/5);

			pDC->MoveTo(xoffset + (j+1) * xPos_minor + i * xPos + 2 * size_text.cx , yoffset + yheight*4/5);
			pDC->LineTo(xoffset + (j+1) * xPos_minor + i * xPos + 2 * size_text.cx , yoffset + yheight);
		}
	}

	CSize sizeCommanderText = pDC->GetTextExtent(strCommander);
	CSize sizeSendTimeText = pDC->GetTextExtent(strSendTime);
	//draw title 
	for(i=0;i<phaseCount;i++)
	{
		tempRect.left = xoffset+ i * xPos;
		tempRect.right = tempRect.left + 2 *size_text.cx;

		tempRect.top = yoffset;
		tempRect.bottom = tempRect.top + yheight*1/5;

		pDC->DrawText(str_text,&tempRect,DT_VCENTER|DT_CENTER|DT_NOCLIP|DT_SINGLELINE);

		tempRect.left = xoffset+ i * xPos + size_text.cx *2;
		tempRect.right = tempRect.left + xPos_minor;

		tempRect.top = yoffset + yheight * 4/5;
		tempRect.bottom = tempRect.top + yheight*1/5;

		pDC->DrawText(strCommander,&tempRect,DT_VCENTER|DT_CENTER|DT_SINGLELINE);

		tempRect.left += 2*xPos_minor;
		tempRect.right += 2*xPos_minor;

		pDC->DrawText(strSendTime,&tempRect,DT_VCENTER|DT_CENTER|DT_SINGLELINE);	

	}



	pDC->SelectObject(pOldFont);
	pDC->SetTextAlign(nOldAlign);
	pDC->SetBkMode(nOldBackMode);
	pDC->SelectObject(pOldPen);
	pDC->SetTextColor(OldTextColor);

	mFont.DeleteObject();
	mThickPen.DeleteObject();
}

void CPlot24Doc::DrawPhasePlanFrameXian(CDC *pDC, LONG lColor, CRect *pRect)
{
	int scaleRate = min(yMultiple,5);

	int i;
	int m_nLogPixelSy = pDC->GetDeviceCaps(LOGPIXELSY);

	LOGFONT stFont;
	CFont mFont;
	CFont *pOldFont;
	memset(&stFont,0,sizeof(stFont));
	stFont.lfHeight = MulDiv(12,m_nLogPixelSy,72);	 
	stFont.lfWeight = FW_THIN;
	stFont.lfClipPrecision = CLIP_LH_ANGLES;
	mFont.CreateFontIndirect(&stFont);
	pOldFont = pDC->SelectObject(&mFont);

	UINT nOldAlign,nOldBackMode;
	CPen mThickPen;

	CPen *pOldPen;
	int xPos;
	int xPos_minor;

	CString str_text,strCommander,strSendTime;
	str_text.Format("气候");
	strCommander = "批准人/时间";
	strSendTime = "下达时间";

	mThickPen.CreatePen(PS_SOLID,(int)(1*scaleRate),lColor);

	nOldAlign = pDC->SetTextAlign(TA_LEFT|TA_TOP);
	nOldBackMode = pDC->SetBkMode(TRANSPARENT);
	pOldPen = pDC->SelectObject(&mThickPen);

	UINT OldTextColor = pDC->SetTextColor(lColor);

	CRect phaseRect;

	int offset_x, offset_y, height, width;
	if(!GetOffsetAndSize(AREA_TYPE::PHASE, offset_x, offset_y, height, width))
		return;
	phaseRect.top = offset_y;
	phaseRect.bottom = offset_y+height;	

	phaseRect.left = offset_x;
	phaseRect.right = offset_x + width;

	int phaseCount = 0;
	if(m_printFormat == 1)
		phaseCount = 3;

	else
		phaseCount = 6;

	//first draw frame  of phase;
	pDC->Rectangle(phaseRect);

	int xoffset, yoffset;
	xoffset = phaseRect.left;
	yoffset = phaseRect.top;

	int xwidth, yheight;
	xwidth = phaseRect.Width();
	yheight = phaseRect.Height();

	CSize size_text ;
	size_text = pDC->GetTextExtent(str_text);

	//initial the width of the threee phase
	xPos = width / phaseCount;
	xPos_minor = xPos - 2 *size_text.cx;
	xPos_minor = xPos_minor / 4;

	CRect tempRect;
	tempRect = phaseRect;	

	for(i =0;i<phaseCount;i++)
	{
		pDC->MoveTo(xoffset + i * xPos , yoffset);
		pDC->LineTo(xoffset + i * xPos , yheight + yoffset);

		pDC->MoveTo(xoffset + i * xPos + 2 * size_text.cx , yoffset);
		pDC->LineTo(xoffset + i * xPos + 2 * size_text.cx , yheight+ yoffset);


		for(int j= 0;j<3;j++)
		{
			pDC->MoveTo(xoffset + (j+1) * xPos_minor + i * xPos + 2 * size_text.cx , yoffset);
			pDC->LineTo(xoffset + (j+1) * xPos_minor + i * xPos + 2 * size_text.cx , yoffset + yheight);			
		}
	}

	CSize sizeCommanderText = pDC->GetTextExtent(strCommander);
	CSize sizeSendTimeText = pDC->GetTextExtent(strSendTime);
	//draw title 
	for(i=0;i<phaseCount;i++)
	{
		tempRect.left = xoffset+ i * xPos;
		tempRect.right = tempRect.left + 2 *size_text.cx;

		tempRect.top = yoffset;
		tempRect.bottom = tempRect.top + yheight;

		pDC->DrawText(str_text,&tempRect,DT_VCENTER|DT_CENTER|DT_NOCLIP|DT_SINGLELINE);
	}

	pDC->SelectObject(pOldFont);
	pDC->SetTextAlign(nOldAlign);
	pDC->SetBkMode(nOldBackMode);
	pDC->SelectObject(pOldPen);
	pDC->SetTextColor(OldTextColor);

	mFont.DeleteObject();
	mThickPen.DeleteObject();

}
void CPlot24Doc::FillPhaseContent(CDC *pCDC, COLORREF clr)
{
	if(this->mPlotStyle.id == 5)
		return;

	UINT nOldAlign,nOldBackMode;
	CPen mThickPen;
	CPen *pOldPen;
	UINT OldTextColor;

	mThickPen.CreatePen(PS_SOLID,(int)(1*yMultiple),clr);
	int m_nLogPixelSy = pCDC->GetDeviceCaps(LOGPIXELSY);

	LOGFONT stFont;
	CFont mFont;
	CFont *pOldFont;
	memset(&stFont,0,sizeof(stFont));
	stFont.lfHeight = MulDiv(12,m_nLogPixelSy,72);	 
	stFont.lfWeight = FW_THIN;
	stFont.lfClipPrecision = CLIP_LH_ANGLES;
	strcpy(stFont.lfFaceName,"宋体");
	mFont.CreateFontIndirect(&stFont);
	pOldFont = pCDC->SelectObject(&mFont);

	nOldAlign = pCDC->SetTextAlign(TA_LEFT|TA_TOP);
	nOldBackMode = pCDC->SetBkMode(TRANSPARENT);
	pOldPen = pCDC->SelectObject(&mThickPen);
	OldTextColor = pCDC->SetTextColor(CLR_BLACK);

	CString str_text;
	str_text.Format("气候");
	CSize text_size;
	text_size = pCDC->GetTextExtent(str_text);

	pCDC->SelectObject(pOldFont);
	mFont.DeleteObject();

	memset(&stFont,0,sizeof(stFont));
	stFont.lfHeight = MulDiv(9,m_nLogPixelSy,72);
	stFont.lfWeight = FW_THIN;
	stFont.lfClipPrecision = CLIP_LH_ANGLES;
	strcpy(stFont.lfFaceName,"宋体");
	mFont.CreateFontIndirect(&stFont);
	pOldFont = pCDC->SelectObject(&mFont);

	CRect phaseRect;

	int offset_x, offset_y, height, width;
	if(!GetOffsetAndSize(AREA_TYPE::PHASE, offset_x, offset_y, height, width))
		return;
	phaseRect.top = offset_y;
	phaseRect.bottom = offset_y + height;	

	phaseRect.left = offset_x;
	phaseRect.right = offset_x + width;

	CRect inflate_Rect;

	int phaseCount = 0;

	if(m_printFormat == 1)
		phaseCount = 3;
	else
		phaseCount = 6;


	int	nTotalWidth = phaseRect.Width(); 
	int nRectWidth = nTotalWidth/phaseCount;


	int xoffset,yoffset;
	xoffset = phaseRect.left;
	yoffset = phaseRect.top;

	int xwidth,yheight;
	xwidth = phaseRect.Width();
	yheight = phaseRect.Height();

	int nTitilRectWidth = nRectWidth - 2 * text_size.cx;
	nTitilRectWidth = nTitilRectWidth / 4;

	
	int station_index = 1;
	
	CString stationName;
	CString weather_content;
	char *  weather[]={"晴","阴","小雨","中雨","大雨","小雾","中雾","大雾"
		,"小雪","中雪","大雪"};
	int index_start=0;
	int index_end=0;


	CRect drawRect,commandRect,timeRect;	
	CString phase_content, strCommandInfo,strSendTime ;
	NOTEBOOK* notes = NULL; 

	CString m_Start_station,m_End_station;
	int num=128, stationNo[128];
	memset(stationNo, 0, sizeof(stationNo));

	num=gpcdef.GetStationList(this->m_nCurrentSectionNo, num, stationNo);
	
	m_Start_station.Format("%s", gpcdef.GetStationName(stationNo[0]));
	if(num > 0)
		m_End_station.Format("%s", gpcdef.GetStationName(stationNo[num-1]));
	else
		m_End_station.Format("%s", gpcdef.GetStationName(stationNo[0]));

	//如果十二小时，那么phaseCount为3，如果二十四小时，那么phaseCount为6
	int shiftCount = (phaseCount==3)?1:2;


	for(int shift=0; shift<shiftCount; shift++)
	{
		if(shift == 0)
		{
			if(m_shiftid_Notes2%2 == 1)
				notes = &NOTES2;
			else
				notes = &NOTES;

		}
		else
		{
			if(m_shiftid_Notes2%2 == 1)
				notes = &NOTES;
			else
				notes = &NOTES2;

			xoffset += nTotalWidth/2;
		}

		//十二小时打印总是使用第一个的值
		if(m_printFormat == 1)
		{
			notes = &NOTES;
		}
		for(int i = 0;i<3;i++)
		{

			if(i == 0)
			{
				phase_content.Format("%s",notes->note1);	
				strCommandInfo.Format("%s",notes->command_info1);
				strSendTime.Format("%s",notes->send_time1);
			}
			else if(i==1)
			{
				phase_content.Format("%s",notes->note2);	
				strCommandInfo.Format("%s",notes->command_info2);
				strSendTime.Format("%s",notes->send_time2);

			}
			else
			{
				phase_content.Format("%s",notes->note3);	
				strCommandInfo.Format("%s",notes->command_info3);
				strSendTime.Format("%s",notes->send_time3);
			}

			drawRect.left = xoffset + 2*text_size.cx + i*nRectWidth;
			drawRect.right = xoffset + i*nRectWidth + nRectWidth ;
			drawRect.top = yoffset+yheight*1/5;
			drawRect.bottom = drawRect.top + yheight*3/5;

			drawRect.InflateRect(-5*fWidthMulti,-5*fHeightMulti,-5*fWidthMulti,-5*fHeightMulti);
			pCDC->DrawText(phase_content,&drawRect,DT_WORDBREAK|DT_EXTERNALLEADING);


			commandRect.left = xoffset + 2*text_size.cx + i*nRectWidth + nTitilRectWidth;
			commandRect.right = commandRect.left + nTitilRectWidth;

			commandRect.top = yoffset + yheight*4/5;
			commandRect.bottom = commandRect.top + yheight*1/5;


			pCDC->DrawText(strCommandInfo,&commandRect,DT_CENTER|DT_VCENTER|DT_SINGLELINE);


			timeRect.left = xoffset + 2*text_size.cx + i*nRectWidth + 3*nTitilRectWidth;
			timeRect.right = timeRect.left + nTitilRectWidth;

			timeRect.top = yoffset + yheight*4/5;
			timeRect.bottom = commandRect.top + yheight*1/5;

			pCDC->DrawText(strSendTime,&timeRect,DT_CENTER|DT_VCENTER|DT_SINGLELINE);

			drawRect.left = xoffset + 2 *text_size.cx + i * nRectWidth;
			drawRect.right = drawRect.left + nTitilRectWidth;
			drawRect.top = yoffset;
			drawRect.bottom = drawRect.top + yheight * 1/5;

			inflate_Rect.CopyRect(drawRect);
			stationName = m_Start_station;
			pCDC->DrawText(stationName,&inflate_Rect,DT_CENTER|DT_SINGLELINE|DT_VCENTER|DT_WORDBREAK|DT_EXTERNALLEADING|DT_NOCLIP);


			switch(i)
			{
			case 0:
				index_start = notes->start_wea1;
				break;
			case 1:
				index_start = notes->start_wea2;
				break;
			case 2:
				index_start = notes->start_wea3;
				break;
			}
			weather_content.Empty();
			if(index_start<11 && index_start >= 0) //天气的index
			{
				weather_content.Format("%s",weather[index_start]);
			}

			drawRect.OffsetRect(nTitilRectWidth,0);
			inflate_Rect.CopyRect(drawRect);		
			pCDC->DrawText(weather_content,&inflate_Rect,DT_WORDBREAK|DT_NOCLIP|DT_EXTERNALLEADING|DT_SINGLELINE|DT_VCENTER|DT_CENTER);

			drawRect.OffsetRect(nTitilRectWidth,0);
			stationName =  m_End_station;
			inflate_Rect.CopyRect(drawRect);
			pCDC->DrawText(stationName,&inflate_Rect,DT_WORDBREAK|DT_NOCLIP|DT_EXTERNALLEADING|DT_SINGLELINE|DT_CENTER|DT_VCENTER);


			switch(i)
			{
			case 0:
				index_end = notes->end_wea1;
				break;
			case 1:
				index_end = notes->end_wea2;
				break;
			case 2:
				index_end = notes->end_wea3;
				break;
			}

			drawRect.OffsetRect(nTitilRectWidth,0);
			inflate_Rect.CopyRect(drawRect);		
			weather_content.Empty();
			if(index_end < 11 && index_end >= 0)
			{
				weather_content.Format("%s",weather[index_end]);
			}
			pCDC->DrawText(weather_content,&inflate_Rect,DT_WORDBREAK|DT_NOCLIP|DT_EXTERNALLEADING|DT_SINGLELINE|DT_CENTER|DT_VCENTER);
		}
	}

	pCDC->SelectObject(pOldFont);
	pCDC->SetTextAlign(nOldAlign);
	pCDC->SetBkMode(nOldBackMode);
	pCDC->SelectObject(pOldPen);
	pCDC->SetTextColor(OldTextColor);
	mThickPen.DeleteObject();
	mFont.DeleteObject();

}

void CPlot24Doc::FillPhaseContentXian(CDC *pCDC, COLORREF clr)
{
	UINT nOldAlign,nOldBackMode;
	CPen mThickPen;
	CPen *pOldPen;
	UINT OldTextColor;

	mThickPen.CreatePen(PS_SOLID,(int)(1*yMultiple),clr);
	int m_nLogPixelSy = pCDC->GetDeviceCaps(LOGPIXELSY);

	LOGFONT stFont;
	CFont mFont;
	CFont *pOldFont;
	memset(&stFont,0,sizeof(stFont));
	stFont.lfHeight = MulDiv(12,m_nLogPixelSy,72);	 
	stFont.lfWeight = FW_THIN;
	stFont.lfClipPrecision = CLIP_LH_ANGLES;
	strcpy(stFont.lfFaceName,"宋体");
	mFont.CreateFontIndirect(&stFont);
	pOldFont = pCDC->SelectObject(&mFont);

	nOldAlign = pCDC->SetTextAlign(TA_LEFT|TA_TOP);
	nOldBackMode = pCDC->SetBkMode(TRANSPARENT);
	pOldPen = pCDC->SelectObject(&mThickPen);
	OldTextColor = pCDC->SetTextColor(CLR_BLACK);

	CString str_text;
	str_text.Format("气候");
	CSize text_size;
	text_size = pCDC->GetTextExtent(str_text);

	pCDC->SelectObject(pOldFont);
	mFont.DeleteObject();

	memset(&stFont,0,sizeof(stFont));
	stFont.lfHeight = MulDiv(9,m_nLogPixelSy,72);
	stFont.lfWeight = FW_THIN;
	stFont.lfClipPrecision = CLIP_LH_ANGLES;
	strcpy(stFont.lfFaceName,"宋体");
	mFont.CreateFontIndirect(&stFont);
	pOldFont = pCDC->SelectObject(&mFont);

	CRect inflate_Rect;
	CRect phaseRect;
	int phaseCount = 0;

	int offset_x, offset_y, height, width;
	if(!GetOffsetAndSize(AREA_TYPE::PHASE, offset_x, offset_y, height, width))
		return;
	int nWidth = m_fPixelPerMinute * m_nViewTimeRange*60;

	phaseRect.top = offset_y;
	phaseRect.bottom = offset_y + height;	
	phaseRect.left = offset_x;
	phaseRect.right = offset_x + nWidth;

	if(m_printFormat == 1)
		phaseCount = 3;
	else
		phaseCount = 6;


	int	nTotalWidth = m_fPixelPerMinute * m_nViewTimeRange*60; 
	int nRectWidth = nTotalWidth/phaseCount;


	int xoffset,yoffset;
	xoffset = phaseRect.left;
	yoffset = phaseRect.top;

	int xwidth,yheight;
	xwidth = phaseRect.Width();
	yheight = phaseRect.Height();

	int nTitilRectWidth = nRectWidth - 2 * text_size.cx;
	nTitilRectWidth = nTitilRectWidth / 4;

	int nStationList[128],nStationNum;

	nStationNum = sizeof(nStationList) / sizeof(nStationList[0]);
	gpcdef.GetStationList(m_sPrintInform->usSectionNo,nStationNum,nStationList);

	CString stationName;
	CString weather_content;
	char *  weather[]={"晴","阴","小雨","中雨","大雨","小雾","中雾","大雾"
		,"小雪","中雪","大雪"};
	int index_start;
	int index_end;


	CRect drawRect,commandRect,timeRect;	
	NOTEBOOK* notes = NULL; 

	//如果十二小时，那么phaseCount为3，如果二十四小时，那么phaseCount为6
	int shiftCount = (phaseCount==3)?1:2;


	for(int shift=0; shift<shiftCount; shift++)
	{
		if(shift == 0)
		{
			if(m_shiftid_Notes2%2 == 1)
				notes = &NOTES2;
			else
				notes = &NOTES;

		}
		else
		{
			if(m_shiftid_Notes2%2 == 1)
				notes = &NOTES;
			else
				notes = &NOTES2;

			xoffset += nTotalWidth/2;
		}

		//十二小时打印总是使用第一个的值
		if(m_printFormat == 1)
		{
			notes = &NOTES;
		}
		for(int i = 0;i<3;i++)
		{				
			drawRect.left = xoffset + 2 *text_size.cx + i * nRectWidth;
			drawRect.right = drawRect.left + nTitilRectWidth;
			drawRect.top = yoffset;
			drawRect.bottom = drawRect.top + yheight;

			inflate_Rect.CopyRect(drawRect);
			stationName = CString(gpcdef.GetStationName(nStationList[0]));
			pCDC->DrawText(stationName,&inflate_Rect,DT_CENTER|DT_SINGLELINE|DT_VCENTER|DT_WORDBREAK|DT_EXTERNALLEADING|DT_NOCLIP);


			switch(i)
			{
			case 0:
				index_start = notes->start_wea1;
				break;
			case 1:
				index_start = notes->start_wea2;
				break;
			case 2:
				index_start = notes->start_wea3;
				break;
			}
			weather_content.Empty();
			if(index_start<11 && index_start >= 0) //天气的index
			{
				weather_content.Format("%s",weather[index_start]);
			}

			drawRect.OffsetRect(nTitilRectWidth,0);
			inflate_Rect.CopyRect(drawRect);		
			pCDC->DrawText(weather_content,&inflate_Rect,DT_WORDBREAK|DT_NOCLIP|DT_EXTERNALLEADING|DT_SINGLELINE|DT_VCENTER|DT_CENTER);

			drawRect.OffsetRect(nTitilRectWidth,0);
			stationName = CString(gpcdef.GetStationName(nStationList[nStationNum-1]));
			inflate_Rect.CopyRect(drawRect);
			pCDC->DrawText(stationName,&inflate_Rect,DT_WORDBREAK|DT_NOCLIP|DT_EXTERNALLEADING|DT_SINGLELINE|DT_CENTER|DT_VCENTER);


			switch(i)
			{
			case 0:
				index_end = notes->end_wea1;
				break;
			case 1:
				index_end = notes->end_wea2;
				break;
			case 2:
				index_end = notes->end_wea3;
				break;
			}

			drawRect.OffsetRect(nTitilRectWidth,0);
			inflate_Rect.CopyRect(drawRect);		
			weather_content.Empty();
			if(index_end < 11 && index_end >= 0)
			{
				weather_content.Format("%s",weather[index_end]);
			}
			pCDC->DrawText(weather_content,&inflate_Rect,DT_WORDBREAK|DT_NOCLIP|DT_EXTERNALLEADING|DT_SINGLELINE|DT_CENTER|DT_VCENTER);
		}
	}

	pCDC->SelectObject(pOldFont);
	pCDC->SetTextAlign(nOldAlign);
	pCDC->SetBkMode(nOldBackMode);
	pCDC->SelectObject(pOldPen);
	pCDC->SetTextColor(OldTextColor);
	mThickPen.DeleteObject();
	mFont.DeleteObject();
}

void CPlot24Doc::DrawLanzhouJiShi(CDC *pCDC,COLORREF clr)
{
	int scaleRate = min(yMultiple,5);

	CRect drawRect;
	CString jishi_text;

	UINT nOldAlign,nOldBackMode;
	CPen mThickPen;
	CPen mBlockPen,*pOldPen;
	UINT OldTextColor;


	LOGFONT stFont;
	CFont mFont;
	CFont *pOldFont;

	//设置各种属性
	mThickPen.CreatePen(PS_SOLID,(int)(1*scaleRate),clr);
	
	nOldBackMode = pCDC->SetBkMode(TRANSPARENT);
	pOldPen = pCDC->SelectObject(&mThickPen);
	OldTextColor = pCDC->SetTextColor(clr);
	int m_nLogPixelSy = pCDC->GetDeviceCaps(LOGPIXELSY);


	//设置字体
	memset(&stFont,0,sizeof(stFont));
	stFont.lfHeight = MulDiv(12,m_nLogPixelSy,72);	 
	stFont.lfWeight = FW_THIN;
	stFont.lfClipPrecision = CLIP_LH_ANGLES;
	mFont.CreateFontIndirect(&stFont);
	pOldFont = pCDC->SelectObject(&mFont);


	int offset_x, offset_y, height, width;
	if(!GetOffsetAndSize(AREA_TYPE::NOTE, offset_x, offset_y, height, width))
		return;
	drawRect.top    = offset_y;
	drawRect.bottom = offset_y + height;
	drawRect.left   = offset_x;    
	drawRect.right  = offset_x + width;	

	pCDC->Rectangle(&drawRect);

	int xWidth,yHeight;
	xWidth = drawRect.Width();
	yHeight = drawRect.Height();

	pCDC->MoveTo(drawRect.left+20*scaleRate,drawRect.top);
	pCDC->LineTo(drawRect.left+20*scaleRate,drawRect.bottom);

	//画记事标题
	nOldAlign = pCDC->SetTextAlign(TA_CENTER);
	pCDC->TextOut(drawRect.left+10*scaleRate, drawRect.top+drawRect.Height()/4,"记");
	pCDC->TextOut(drawRect.left+10*scaleRate, drawRect.top+3*drawRect.Height()/4,"事");

	pCDC->SelectObject(pOldFont);
	mFont.DeleteObject();
	pCDC->SetTextAlign(TA_LEFT|TA_TOP);

	memset(&stFont,0,sizeof(stFont));
	stFont.lfHeight = MulDiv(9,m_nLogPixelSy,72);	 
	stFont.lfWeight = FW_THIN;
	stFont.lfClipPrecision = CLIP_LH_ANGLES;
	strcpy(stFont.lfFaceName,"宋体");
	mFont.CreateFontIndirect(&stFont);
	pOldFont = pCDC->SelectObject(&mFont);
	pCDC->SetTextColor(OldTextColor);
	OldTextColor = pCDC->SetTextColor(CLR_BLACK);

	CRect inflate_Rect;
	inflate_Rect.CopyRect(drawRect);
	inflate_Rect.left += 20*scaleRate;
	inflate_Rect.InflateRect(-5*fWidthMulti,-5*fHeightMulti,-5*fWidthMulti,-5*fHeightMulti);

    jishi_text = parsecontent[0];//added by zhaoliang
	pCDC->DrawText(jishi_text,&inflate_Rect,DT_WORDBREAK|DT_EXTERNALLEADING);

	pCDC->SetTextAlign(nOldAlign);
	pCDC->SetBkMode(nOldBackMode);
	pCDC->SelectObject(pOldPen);
	pCDC->SetTextColor(OldTextColor);
	pCDC->SelectObject(pOldFont);

	mThickPen.DeleteObject();
	mFont.DeleteObject();

}

void CPlot24Doc::DrawJiShi(CDC *pCDC, COLORREF clr)
{
	if(m_printFormat == 1)
	{
		if(this->mPlotStyle.id == 5)
			this->DrawLanzhouJiShi(pCDC,CLR_PLOT);
		else
		    this->DrawJiShi12(pCDC,CLR_PLOT);
	}
	else
	{
		DrawJiShi24(pCDC,clr);
	}

	return;
}

//绘制标题头
void CPlot24Doc::DrawLanzhouHeaderTitle(CDC * pCDC, COLORREF clrColor)
{
	int scalerate = min(yMultiple,5);
	CString title,temp;
	title = CString(gpcdef.GetSectionName(m_sPrintInform->usSectionNo ));
	int xoffset, yoffset;

	int offset_x, offset_y, height, width;
	if(!GetOffsetAndSize(AREA_TYPE::TITLE, offset_x, offset_y, height, width))
		return;
	xoffset = offset_x;
	yoffset = offset_y;
	
	CFont fontHeaderTitle;	
	LOGFONT stFont;
	CFont * pOldFont;
	int m_nLogPixelSy = pCDC->GetDeviceCaps(LOGPIXELSY);

	memset(&stFont,0,sizeof(stFont));
	stFont.lfHeight = MulDiv(30,-m_nLogPixelSy,72);
	stFont.lfWeight = FW_THIN; //FW_NORMAL;
	stFont.lfClipPrecision = CLIP_LH_ANGLES;
	fontHeaderTitle.CreateFontIndirect(&stFont);

	if(m_nSchdStatus == BASIC_SCHEDULE)
	{
		CString name="", name1="";
		basic_schd_config* pbasic=GetBasicConfig();
		if(m_sPrintInform->usSchdlType == 1)
		{
			name1=" 常用基本图";
			name=GetBasicConfig()->GetBasicNameByType(m_sPrintInform->usSectionNo, CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC);
		}
		else if(m_sPrintInform->usSchdlType == 2)
		{
			name1=" 预览基本图";
			name=GetBasicConfig()->GetBasicNameByType(m_sPrintInform->usSectionNo, CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_PREV);
		}
		else if(m_sPrintInform->usSchdlType == 10)
		{
			name1=" 归档基本图";
			name=GetBasicConfig()->GetBasicNameByType(m_sPrintInform->usSectionNo, CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_BACKUP);
		}
		
		if(name.IsEmpty())
			title += " 基 本 运 行 图";
		else
			title += name1+"("+name+")";

		if(m_nBasicPlotFlag==1)
			title += "(18点-06点)";
		else if(m_nBasicPlotFlag==2)
			title += "(06点-18点)";
	}
	else if(m_nSchdStatus == SHIFT_SCHEDULE)
		title += " 日  班  运  行  图";
	else 
		title += " 列  车  运  行  图";

	pOldFont = pCDC->SelectObject(&fontHeaderTitle);	
	UINT nOldBackMode = pCDC->SetBkMode(TRANSPARENT);
	UINT nOldColor = pCDC->SetTextColor(clrColor);	
	UINT nOldAlign = pCDC->SetTextAlign(TA_CENTER|TA_TOP);

	pCDC->TextOut(xoffset,yoffset,title);

	pCDC->SetBkMode(nOldBackMode);
	pCDC->SetTextColor(nOldColor);
	pCDC->SetTextAlign(nOldAlign);
	pCDC->SelectObject(pOldFont);
	fontHeaderTitle.DeleteObject();
}

//绘制标题头
void CPlot24Doc::DrawHeaderTitle(CDC * pCDC, COLORREF clrColor)
{
	if(this->mPlotStyle.id == 5)
	{
		this->DrawLanzhouHeaderTitle(pCDC,CLR_PLOT);
		return;
	}

	int scalerate = min(yMultiple,5);
	CString title,temp;
	title = CString(gpcdef.GetSectionName(m_sPrintInform->usSectionNo ));
	int xoffset, yoffset;


		int offset_x, offset_y, height, width;
		if(!GetOffsetAndSize(AREA_TYPE::TITLE, offset_x, offset_y, height, width))
			return;
		xoffset = offset_x;
		yoffset = offset_y;
	
	CFont fontHeaderTitle;	
	LOGFONT stFont;
	CFont * pOldFont;
	int m_nLogPixelSy = pCDC->GetDeviceCaps(LOGPIXELSY);

	memset(&stFont,0,sizeof(stFont));
	stFont.lfHeight = MulDiv(30,-m_nLogPixelSy,72);
	stFont.lfWeight = FW_THIN; //FW_NORMAL;
	stFont.lfClipPrecision = CLIP_LH_ANGLES;
	fontHeaderTitle.CreateFontIndirect(&stFont);

	if(m_nSchdStatus == BASIC_SCHEDULE)
	{
		CString name="",name1="";
		basic_schd_config* pbasic=GetBasicConfig();
		if(m_sPrintInform->usSchdlType == 1)
		{
			name1=" 常用基本图";
			name=GetBasicConfig()->GetBasicNameByType(m_sPrintInform->usSectionNo, CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC);
		}
		else if(m_sPrintInform->usSchdlType == 2)
		{
			name1=" 预览基本图";
			name=GetBasicConfig()->GetBasicNameByType(m_sPrintInform->usSectionNo, CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_PREV);
		}
		else if(m_sPrintInform->usSchdlType == 10)
		{
			name1=" 归档基本图";
			name=GetBasicConfig()->GetBasicNameByType(m_sPrintInform->usSectionNo, CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_BACKUP);
		}
		
		if(name.IsEmpty())
			title += " 基 本 运 行 图";
		else
			title += name1+"("+name+")";
	}
	else if(m_nSchdStatus == SHIFT_SCHEDULE)
		title += " 日 班 运 行 图";
	else 
		title += " 列 车 运 行 图";

	if(m_nSchdStatus == BASIC_SCHEDULE)
	{
		temp.Format("(%s----%s)",
			OsConvertTimetoString(m_sPrintInform->usStartTime),
			OsConvertTimetoString(m_sPrintInform->usEndTime));
	} 
	else
	{
		CTime ctStartTimeValue = CTime(long_time(m_sPrintInform->usStartDate,m_sPrintInform->usStartTime));
		CTime ctEndTimeValue = CTime(long_time(m_sPrintInform->usEndDate,m_sPrintInform->usEndTime));
		temp.Format("(%04d年%02d月%02d日 %02d:%02d一一%04d年%02d月%02d日 %02d:%02d)",
			ctStartTimeValue.GetYear( ),
			ctStartTimeValue.GetMonth( ),
			ctStartTimeValue.GetDay( ),
			ctStartTimeValue.GetHour( ),
			ctStartTimeValue.GetMinute( ),
			ctEndTimeValue.GetYear( ),
			ctEndTimeValue.GetMonth( ),
			ctEndTimeValue.GetDay( ),
			ctEndTimeValue.GetHour( ),
			ctEndTimeValue.GetMinute( ));
	}
	title += temp;

	pOldFont = pCDC->SelectObject(&fontHeaderTitle);	
	UINT nOldBackMode = pCDC->SetBkMode(TRANSPARENT);
	UINT nOldColor = pCDC->SetTextColor(clrColor);	
	UINT nOldAlign = pCDC->SetTextAlign(TA_CENTER|TA_TOP);

	pCDC->TextOut(xoffset,yoffset,title);

	pCDC->SetBkMode(nOldBackMode);
	pCDC->SetTextColor(nOldColor);
	pCDC->SetTextAlign(nOldAlign);
	pCDC->SelectObject(pOldFont);
	fontHeaderTitle.DeleteObject();

}
void CPlot24Doc::DrawFooterTitle(CDC *pCDC, COLORREF clrColor)
{
	CString foot;
	CFont *pOldFont;
	CTime ctCurrentTime = CTime::GetCurrentTime( );
	CFont m_fontFoot;
	LOGFONT stFont;
	int m_nLogPixelSy = pCDC->GetDeviceCaps(LOGPIXELSY);

	memset(&stFont,0,sizeof(stFont));
	stFont.lfHeight = MulDiv(12,m_nLogPixelSy,72);
	stFont.lfWeight = FW_THIN; //FW_NORMAL;
	stFont.lfClipPrecision = CLIP_LH_ANGLES;
	m_fontFoot.CreateFontIndirect(&stFont);

	pOldFont = pCDC->SelectObject(&m_fontFoot);
	UINT nOldAlign = pCDC->SetTextAlign(TA_CENTER|TA_BOTTOM);
	int nOldBackMode = pCDC->SetBkMode(TRANSPARENT);
	int nOldColor = pCDC->SetTextColor(clrColor);

	//int x = m_nPageWidth/2;
	//int y = 0;
	//if(m_nSchdStatus == BASIC_SCHEDULE)
	//{
	//	y = g_basic_footer_yoffset*m_nPageHeight;
	//}
	//else
	//{
		int offset_x, offset_y, height, width;
		if(!GetOffsetAndSize(AREA_TYPE::FOOT, offset_x, offset_y, height, width))
			return;

	//	x = offset_x;
	//	y = offset_y;
	//}


	foot.Format("打印时间:%04d年%02d月%02d日%02d时%02d分",
		ctCurrentTime.GetYear( ),
		ctCurrentTime.GetMonth( ),
		ctCurrentTime.GetDay( ),
		ctCurrentTime.GetHour( ),
		ctCurrentTime.GetMinute( ));

	CSize mSize = pCDC->GetTextExtent(foot);

	pCDC->TextOut(offset_x, offset_y, foot);

	pCDC->SetBkMode(nOldBackMode);
	pCDC->SetTextColor(nOldColor);
	pCDC->SelectObject(pOldFont);
	pCDC->SetTextAlign(nOldAlign);
	m_fontFoot.DeleteObject();

}

void CPlot24Doc::OnCloseDocument() 
{
	CDocument::OnCloseDocument();
}
BOOL CPlot24Doc::CanCloseFrame(CFrameWnd* pFrame) 
{

	return TRUE;
}

void CPlot24Doc::DrawSchdTrain(CDC *pCDC,int nFlag)
{
	int nSchdTrainNum = m_vecSchdTrains.size();

	if(nSchdTrainNum <= 0)
		return;
	int i;

	int nDrawMode = 0x00000000;// redraw all schedule train, so copypen mode

	if(m_nSchdStatus == BASIC_SCHEDULE)
	{  	 
		nDrawMode = DRAW_NORMAL;// redraw all schedule train, so copypen mode
		if(nFlag == 0)
		{
			for(i=0;i < nSchdTrainNum;i++)
				DrawSingleTrain(pCDC, &m_vecSchdTrains[i],nDrawMode,0,0,TRUE,BASIC_SCHEDULE); 
		}
		else 
		{ 
			for(i=0;i < nSchdTrainNum;i++)
				DrawSingleTrain(pCDC, &m_vecSchdTrains[i],nDrawMode,0,0,TRUE,BASIC_SCHEDULE);
		}
	}
	else if(m_nSchdStatus == WORK_SCHEDULE || m_nSchdStatus == SHIFT_SCHEDULE)
	{
		if(nFlag == 0)
		{
			for(i=0;i < nSchdTrainNum;i++)
				DrawSingleTrain(pCDC, &m_vecSchdTrains[i],nDrawMode,0,-1,TRUE,WORK_SCHEDULE);
		}
		else 
		{ 
			for(i=0;i < nSchdTrainNum;i++)
				DrawSingleTrain(pCDC, &m_vecSchdTrains[i],nDrawMode,0,-1,TRUE,WORK_SCHEDULE);
		}
	}
	else if(m_nSchdStatus == HISTORY_SCHEDULE)
	{
		if(nFlag == 0)
		{
			for(i=0;i < nSchdTrainNum;i++)
				DrawSingleTrain(pCDC, &m_vecSchdTrains[i],nDrawMode,0,-1,TRUE,HISTORY_SCHEDULE);
		}
		else 
		{ 
			for(i=0;i < nSchdTrainNum;i++)
				DrawSingleTrain(pCDC, &m_vecSchdTrains[i],nDrawMode,0,-1,TRUE,HISTORY_SCHEDULE);
		}
	}
	else
		;

}

void CPlot24Doc::DrawStationCar(CDC *pCDC, COLORREF clr)
{
	std::map<int, CString>::iterator it = m_car_phase_0.begin();
	for(; it != m_car_phase_0.end(); it++)
	{
		if(!it->second.IsEmpty())
		{
			ShowPhaseStoreTrain24(pCDC, clr, it->first, 0, it->second);
		}
	}

	it = m_car_phase_6.begin();
	for(; it != m_car_phase_6.end(); it++)
	{
		if(!it->second.IsEmpty())
		{
			ShowPhaseStoreTrain24(pCDC, clr, it->first, 6, it->second);
		}
	}

	it = m_car_phase_12.begin();
	for(; it != m_car_phase_12.end(); it++)
	{
		if(!it->second.IsEmpty())
		{
			ShowPhaseStoreTrain24(pCDC, clr, it->first, 12, it->second);
		}
	}

	it = m_car_phase_18.begin();
	for(; it != m_car_phase_18.end(); it++)
	{
		if(!it->second.IsEmpty())
		{
			ShowPhaseStoreTrain24(pCDC, clr, it->first, 18, it->second);
		}
	}
}

void CPlot24Doc::PlotReDraw(CView* view)
{
	CPlot24View* pView = (CPlot24View*)view;
	if(NULL == pView)
		return;

	int height, width;
	if(!GetOffsetAndSize(AREA_TYPE::TRAIN, x_offset_print, y_offset_print, height, width))
		return;

	pView->m_dcMemory.FillRect(CRect(0,0,m_nPageWidth,m_nPageHeight)
		,&m_brushBackground);
	pView->m_dcMemory.SetBkColor(CLR_WHITE);                    //WHITE

	//车站轴
	this->DrawStationAixis(&pView->m_dcMemory, CLR_PLOT);
	//时间轴
	this->DrawTimeAixis(&pView->m_dcMemory,CLR_PLOT); //CLR_DARKGREEN
	//上下的时间线
	this->ShowTimeString(&pView->m_dcMemory,CLR_PLOT);
	//左右的车站名称
	this->ShowStationString(&pView->m_dcMemory,CLR_PLOT,TRUE);
	//虚拟车站
	this->ShowVirtualStationString(&pView->m_dcMemory,CLR_PLOT);
	//标题
	this->DrawHeaderTitle(&pView->m_dcMemory,CLR_PLOT);

	if(this->m_nSchdStatus != BASIC_SCHEDULE)
	{
		//脚注
		this->DrawFooterTitle(&pView->m_dcMemory,CLR_PLOT);			

		//编组信息
		this->DrawTrainInform24(&pView->m_dcMemory,CLR_PLOT);  //列车编组

		//调度员姓名
		this->DrawDDYName24(&pView->m_dcMemory,CLR_PLOT);			

		//绘制站存车框架
		if(this->IsPrintPhaseCar(m_sPrintInform->usSectionNo))  //客专台不输出站存车
		{
			this->DrawPhaseStoreTrainFrame24(&pView->m_dcMemory,CLR_PLOT);
			// 显示站存车
			this->DrawStationCar(&pView->m_dcMemory,CLR_BLACK);
		}

		//绘制的图形，如区间，股道停车等(暂时无用)
		this->DrawTrainMark(&pView->m_dcMemory);

		//记事
		this->DrawJiShi(&pView->m_dcMemory,CLR_PLOT);

		//阶段计划框架
		if(mPlotStyle.id == 1)
		{
			this->DrawPhasePlanFrameXian(&pView->m_dcMemory,CLR_PLOT,NULL);
			this->FillPhaseContentXian(&pView->m_dcMemory,CLR_PLOT);
		}
		else
		{
			this->DrawPhasePlanFrame(&pView->m_dcMemory,CLR_PLOT,NULL);
			this->FillPhaseContent(&pView->m_dcMemory,CLR_PLOT);
		}
	}		

	//画车		
	this->DrawSchdTrain(&pView->m_dcMemory);	
	pView->Invalidate();
}
