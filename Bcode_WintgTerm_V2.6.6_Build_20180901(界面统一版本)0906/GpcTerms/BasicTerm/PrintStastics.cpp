// PrintStastics.cpp : implementation file
//

#include "stdafx.h"
#include "PrintStastics.h"

#include "oscalls.h"
#include "sysshare.h"
#include "msg_id.h"
#include "function.h"
#include "dbfunc.h"
#include "messages.h"
//#include "tgmsg.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#define LINE_SPACE 30
/////////////////////////////////////////////////////////////////////////////
// CPrintStastics dialog



CPrintStastics::CPrintStastics(CWnd* pParent /*=NULL*/)
	: CDialog(CPrintStastics::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPrintStastics)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
		m_brushBackground.CreateSolidBrush(CLR_WHITE);
		if((value= (char*)malloc(2048))==NULL) {
			sysprint(0,REPORT.ThreadId,"[REPORT]: alloc value err");
		}
		if((value2= (char*)malloc(2048))==NULL) {
			sysprint(0,REPORT.ThreadId,"[REPORT]: alloc value err");
		}

}
CPrintStastics::~CPrintStastics()
{
	if(value)
		free(value);
	if(value2)
		free(value2);

}

void CPrintStastics::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPrintStastics)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPrintStastics, CDialog)
	//{{AFX_MSG_MAP(CPrintStastics)
	ON_BN_CLICKED(IDC_CLOSE, OnClose)
	ON_BN_CLICKED(IDC_NEXT, OnNext)
	ON_BN_CLICKED(IDC_PREV, OnPrev)
	ON_BN_CLICKED(IDC_PRINT, OnPrint)
	ON_MESSAGE(	UM_PUN_STATISTICS,parsemsg)
	ON_MESSAGE(	UM_PUN_STATISTICS_2,parsemsg_2)

	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPrintStastics message handlers
void CPrintStastics::parsemsg(WPARAM wp,LPARAM lp)
{

	memcpy(value,(char*)wp,(ushort)lp);
	
}
void CPrintStastics::parsemsg_2(WPARAM wp,LPARAM lp)
{

	memcpy(value2,(char*)wp,(ushort)lp);
	
	CDC *sPrintDC=&m_dcMemory;
	if(*value ==1) //speed statistics
	output(sPrintDC);
	else //punctual statistics
	output2(sPrintDC);
}
void CPrintStastics::output(CDC* sPrintDC)
{
	int i,j,k;
	
	time_t nTimeValue;
	char *buffer=value2;
	ushort  *ptr =(ushort*)(value+1);

	int command = *(int*)buffer;
	buffer+=4;

	double fVar;

	
struct stat_result {
		ushort down_runspeed;
		ushort down_techspeed;
		ushort down_tvlspeed;
		
		ushort up_runspeed;
		ushort up_techspeed;
		ushort up_tvlspeed;
		
		ushort total_runspeed;
		ushort total_techspeed;
		ushort total_tvlspeed;
}*pstat;

	pstat = (struct stat_result*)ptr;
	int nStationPunctualityInformNum = 0;
	int nStationIndex[1024];
	int nTrainDirectFlag;

	CString text,csTemp;
	struct tm *tmp;
	int nPageNum = 1,yPos;
	CFont ftFont,*pOldFont;
	int MAX_LINE_PER_PAGE;
	int nWidth,nHeight;
	//int nTextPosition[ ] = {10,360,710,1060,1410,1760,2110};
	int nTextPosition[ ] = {10,260,520,760,1010,1260,1510,1760,2010,2260};
	char *train_type[]={ 
		"普通货车",
		"普通客车",
		"快速列车",
		"高速列车"
		/*
		"旅游列车",
		"跨局特别旅客快车",
		"管内特别旅客快车",
		"直通旅客快车",
		"管内旅客快车",
		"直通旅客列车",
		"管内旅客列车",
		"市郊旅客列车",
		"混合列车",
		"回送客车底列车",
		"石油直达列车",
		"集装箱快运直达列车",
		"空车直达列车",
		"快运货物列车",
		"始发直达列车",
		"技术直达列车",
		"直通货物列车",
		"区段货物列车",
		"摘挂列车",
		"区段小运转列车",
		"枢纽小运转列车",
		"保温列车",
		"超限货物列车",
		"单机",
		"轻油动车，轨道车",
		"路用列车"*/
		};
	CSize sPagenoTextExtent;
#ifdef DEBUG_SPEED
	CTime sCurr = CTime::GetCurrentTime( );
	sysprint(0,REPORT.ThreadId,"[REPORT]: begin print sta %d:%d:%d",
		sCurr.GetHour( ),sCurr.GetMinute( ),sCurr.GetSecond( ));
#endif
	if(sPrintDC==&m_dcMemory) {
		nWidth=m_nPageWidth;
		nHeight=m_nPageHeight;
		m_dcMemory.FillRect(CRect(0,0,
			m_nPageWidth,
			m_nPageHeight),&m_brushBackground);
		m_dcMemory.SetBkColor(CLR_WHITE);
	}
	else {
		nWidth = sPrintDC->GetDeviceCaps(HORZRES);
		nHeight = sPrintDC->GetDeviceCaps(VERTRES);
	}

		

	// print title
	ftFont.CreatePointFont(600,"简体魏碑");
	pOldFont = sPrintDC->SelectObject(&ftFont);
	text.Format("%s 区间的列车运行速度(公里/小时)",gpcdef.GetSectionName(*(int *)buffer));
	buffer += 4;

	CSize sTextExtent = sPrintDC->GetTextExtent(text);
	sPrintDC->TextOut((nWidth - sTextExtent.cx) / 2,20,text);
	yPos = 20 + sTextExtent.cy + 5;

	ftFont.DeleteObject( );
	// print time range
	ftFont.CreatePointFont(300,"简体魏碑");
	sPrintDC->SelectObject(&ftFont);
	tmp = localtime((time_t *)buffer);
	text.Format("(打印时间范围 %d年%d月%d日%d时%d分--",tmp->tm_year + 1900,
		tmp->tm_mon + 1,tmp->tm_mday,tmp->tm_hour,tmp->tm_min);// time inform
	buffer += sizeof(time_t);
	tmp = localtime((time_t *)buffer);
	csTemp.Format("--%d年%d月%d日%d时%d分)",tmp->tm_year + 1900,
		tmp->tm_mon + 1,tmp->tm_mday,tmp->tm_hour,tmp->tm_min);// time inform
	text += csTemp;

	sTextExtent = sPrintDC->GetTextExtent(text);
	sPrintDC->TextOut((nWidth - sTextExtent.cx) / 2,yPos,text);
	yPos += sTextExtent.cy + LINE_SPACE;
	ftFont.DeleteObject( );

	// print column title
	ftFont.CreatePointFont(400,"简体魏碑");
	sPrintDC->SelectObject(&ftFont);
	ftFont.DeleteObject();

	text.Format("第 %d 页",nPageNum);
	sPagenoTextExtent = sPrintDC->GetTextExtent(text);
	
	text.Format("统计项目");
	sTextExtent = sPrintDC->GetTextExtent(text);
	MAX_LINE_PER_PAGE = (nHeight - 50) / (sTextExtent.cy + 10);
	sPrintDC->TextOut(nTextPosition[0],yPos,text);
	text.Format("上行运行");
	sPrintDC->TextOut(nTextPosition[1],yPos,text);
	text.Format("下行运行");
	sPrintDC->TextOut(nTextPosition[2],yPos,text);
	text.Format("总共运行");
	sPrintDC->TextOut(nTextPosition[3],yPos,text);

	text.Format("上行技术");
	sPrintDC->TextOut(nTextPosition[4],yPos,text);
	text.Format("下行技术");
	sPrintDC->TextOut(nTextPosition[5],yPos,text);
	text.Format("总共技术");
	sPrintDC->TextOut(nTextPosition[6],yPos,text);

	text.Format("上行旅行");
	sPrintDC->TextOut(nTextPosition[7],yPos,text);
	text.Format("下行旅行");
	sPrintDC->TextOut(nTextPosition[8],yPos,text);
	text.Format("总共旅行");
	sPrintDC->TextOut(nTextPosition[9],yPos,text);

	yPos += sTextExtent.cy + 5;	

	text.Format("速    度");
	sPrintDC->TextOut(nTextPosition[1],yPos,text);
	text.Format("速    度");
	sPrintDC->TextOut(nTextPosition[2],yPos,text);
	text.Format("速    度");
	sPrintDC->TextOut(nTextPosition[3],yPos,text);

	text.Format("速    度");
	sPrintDC->TextOut(nTextPosition[4],yPos,text);
	text.Format("速    度");
	sPrintDC->TextOut(nTextPosition[5],yPos,text);
	text.Format("速    度");
	sPrintDC->TextOut(nTextPosition[6],yPos,text);

	text.Format("速    度");
	sPrintDC->TextOut(nTextPosition[7],yPos,text);
	text.Format("速    度");
	sPrintDC->TextOut(nTextPosition[8],yPos,text);
	text.Format("速    度");
	sPrintDC->TextOut(nTextPosition[9],yPos,text);
	ftFont.DeleteObject( );
	yPos += sTextExtent.cy + LINE_SPACE / 2;
	

	sPrintDC->MoveTo(10,yPos);
	sPrintDC->LineTo(nWidth - 10,yPos);
	
	yPos += sTextExtent.cy ;
	
	int index = 0;
	ushort total_cnt = *ptr;
	ptr++;

	for (index =0;index<total_cnt;index=index++) {
	
		if (index>=4) break;  //当超过4种类型的车时，数据出错
	pstat=(struct stat_result*)ptr;
	
	text.Format("%s",train_type[index]);
	sPrintDC->TextOut(nTextPosition[0],yPos,text);
	text.Format("%d",pstat->up_runspeed);
	sPrintDC->TextOut(nTextPosition[1],yPos,text);

	text.Format("%d",pstat->down_runspeed);
	sPrintDC->TextOut(nTextPosition[2],yPos,text);

	text.Format("%d",pstat->total_runspeed);
	sPrintDC->TextOut(nTextPosition[3],yPos,text);
	
	text.Format("%d",pstat->up_techspeed);
	sPrintDC->TextOut(nTextPosition[4],yPos,text);

	text.Format("%d",pstat->down_techspeed);
	sPrintDC->TextOut(nTextPosition[5],yPos,text);

	text.Format("%d",pstat->total_techspeed);
	sPrintDC->TextOut(nTextPosition[6],yPos,text);
	
	text.Format("%d",pstat->up_tvlspeed);
	sPrintDC->TextOut(nTextPosition[7],yPos,text);

	text.Format("%d",pstat->down_tvlspeed);
	sPrintDC->TextOut(nTextPosition[8],yPos,text);

	text.Format("%d",pstat->total_tvlspeed);
	sPrintDC->TextOut(nTextPosition[9],yPos,text);
	
	
	yPos += sTextExtent.cy + 5;
	ptr+=9;
	}
	
	ShowWindow(SW_SHOW);
	UpdateWindow();

}
//punctual statistics.
void CPrintStastics::output2(CDC* sPrintDC)
{
	int i,j,k;
	
	int pass_train_flag,pass_normal_flag;
	time_t nTimeValue;
	char *buffer=value2;
	

	char *ptr =(char*)(value+1);

	int command = *(int*)buffer;
	buffer+=4;

	double fVar;

	
	struct _tagStationPunctualityInform {
		ushort usStationNo;
		ushort spare;
		ushort usUpArriveTotalTrainNum;
		ushort usUpDepartTotalTrainNum;
		ushort usDownArriveTotalTrainNum;
		ushort usDownDepartTotalTrainNum;
		ushort usUpArriveNormalTrainNum;
		ushort usUpDepartNormalTrainNum;
		ushort usDownArriveNormalTrainNum;
		ushort usDownDepartNormalTrainNum;
		ushort usTotalPassTrainNum;
		ushort usNormalPassTrainNum;
	} info[64];

	struct stat_result {
		ushort total;
		ushort ontime;

	} *pstat;
	
	char * train_type[]={
		"上行客车",
		"下行客车",
		"上行货车",
		"下行货车",
		"上行所有列车",
		"下行所有列车",
		"上行客车",
		"下行客车",
		"上行货车",
		"下行货车",
		"上行所有列车",
		"下行所有列车"};


	pstat = (struct stat_result*)ptr;
	int nStationPunctualityInformNum = 0;
	int nStationIndex[1024];
	int nTrainDirectFlag;

	CString text,csTemp;
	struct tm *tmp;
	int nPageNum = 1,yPos;
	CFont ftFont,*pOldFont;
	int MAX_LINE_PER_PAGE;
	int nWidth,nHeight;
	int nTextPosition[ ] = {10,360,710,1060,1410,1760,2110};
	int offset=0;
	CSize sPagenoTextExtent;

#ifdef DEBUG_SPEED
	CTime sCurr = CTime::GetCurrentTime( );
	sysprint(0,REPORT.ThreadId,"[REPORT]: begin print sta %d:%d:%d",
		sCurr.GetHour( ),sCurr.GetMinute( ),sCurr.GetSecond( ));
#endif
	if(sPrintDC==&m_dcMemory) {
		offset=0;
		nWidth=m_nPageWidth;
		nHeight=m_nPageHeight;
		m_dcMemory.FillRect(CRect(0,0,
			m_nPageWidth,
			m_nPageHeight),&m_brushBackground);
		m_dcMemory.SetBkColor(CLR_WHITE);
	}
	else {
		offset=200;
		nWidth = sPrintDC->GetDeviceCaps(HORZRES);
		nHeight = sPrintDC->GetDeviceCaps(VERTRES);
	}

		

	// print title
	ftFont.CreatePointFont(600,"简体魏碑");
	pOldFont = sPrintDC->SelectObject(&ftFont);
	text.Format("%s 区间的列车准点率",gpcdef.GetSectionName(*(int *)buffer));
	buffer += 4;

	CSize sTextExtent = sPrintDC->GetTextExtent(text);
	sPrintDC->TextOut((nWidth - sTextExtent.cx) / 2,20,text);
	yPos = 20 + sTextExtent.cy + 5;

	ftFont.DeleteObject( );
	// print time range
	ftFont.CreatePointFont(300,"简体魏碑");
	sPrintDC->SelectObject(&ftFont);
	tmp = localtime((time_t *)buffer);
	text.Format("(打印时间范围 %d年%d月%d日%d时%d分--",tmp->tm_year + 1900,
		tmp->tm_mon + 1,tmp->tm_mday,tmp->tm_hour,tmp->tm_min);// time inform
	buffer += sizeof(time_t);
	tmp = localtime((time_t *)buffer);
	csTemp.Format("--%d年%d月%d日%d时%d分)",tmp->tm_year + 1900,
		tmp->tm_mon + 1,tmp->tm_mday,tmp->tm_hour,tmp->tm_min);// time inform
	text += csTemp;

	sTextExtent = sPrintDC->GetTextExtent(text);
	sPrintDC->TextOut((nWidth - sTextExtent.cx) / 2,yPos,text);
	yPos += sTextExtent.cy + LINE_SPACE;
	ftFont.DeleteObject( );


	if(sPrintDC==&m_dcMemory) {
		//draw window
		ftFont.CreatePointFont(400,"简体魏碑");
	}
	else {
		//print.
		ftFont.CreatePointFont(600,"简体魏碑");
	}

	// print column title

	sPrintDC->SelectObject(&ftFont);
	ftFont.DeleteObject();
	text.Format("第 %d 页",nPageNum);
	sPagenoTextExtent = sPrintDC->GetTextExtent(text);
	
	text.Format("统计项目");
	sTextExtent = sPrintDC->GetTextExtent(text);
	MAX_LINE_PER_PAGE = (nHeight - 50) / (sTextExtent.cy + 10);
	sPrintDC->TextOut(nTextPosition[0],yPos,text);
	text.Format("总列数");
	sPrintDC->TextOut(nTextPosition[1]+offset,yPos,text);
	text.Format("准点列数");
	sPrintDC->TextOut(nTextPosition[2]+offset*2,yPos,text);
	text.Format("准点率");
	sPrintDC->TextOut(nTextPosition[3]+offset*3,yPos,text);
	

	yPos += sTextExtent.cy + LINE_SPACE / 2;

	sPrintDC->MoveTo(10,yPos);
	sPrintDC->LineTo(nWidth - 10,yPos);
	
	yPos += sTextExtent.cy ;
	text.Format("发车准点率");
	sPrintDC->TextOut(nTextPosition[0],yPos,text);
	
	yPos += sTextExtent.cy ;
	int index =0;
	int total_len = 48;
	for (index =0;index <total_len/sizeof(struct stat_result);index++) {
		
		pstat = (struct stat_result*)ptr;
	text.Format("%s",train_type[index]);
	sPrintDC->TextOut(nTextPosition[0],yPos,text);
	
	text.Format("%d",pstat->total);
	sPrintDC->TextOut(nTextPosition[1]+offset,yPos,text);

	text.Format("%d",pstat->ontime);
	sPrintDC->TextOut(nTextPosition[2]+offset*2,yPos,text);
	int a=60;
	float b = a/100.0;

	text.Format("%d%%",(int)(pstat->ontime*100/(pstat->total*1.0)));
	sPrintDC->TextOut(nTextPosition[3]+offset*3,yPos,text);
	ptr += 4;
	yPos += sTextExtent.cy + 5;
	if (index == ((total_len/sizeof(struct stat_result)+1)/2-1)){
			yPos += sTextExtent.cy ;
			text.Format("运行准点率");
			sPrintDC->TextOut(nTextPosition[0],yPos,text);
			yPos += sTextExtent.cy ;
	}

	

	}

	ShowWindow(SW_SHOW);
	UpdateWindow();

}

void CPrintStastics::OnClose() 
{
	// TODO: Add your control notification handler code here
	ShowWindow(SW_HIDE);
	UpdateWindow();
}

void CPrintStastics::OnNext() 
{
	// TODO: Add your control notification handler code here
	
}

void CPrintStastics::OnPrev() 
{
	// TODO: Add your control notification handler code here
	
}

void CPrintStastics::OnPrint() 
{
	// TODO: Add your control notification handler code here
	int theStatus = 1;
    if(gpcdef.bGpcTrain == TRUE) return;

	do {
		// prepare print device
		CPrintDialog prDialog(TRUE);

		prDialog.GetDefaults( );
		//prDialog.DoModal();
		
		// set printer name and print information
		char abPrinterName[64];
		DEVMODE *pDevMode;
		DEVNAMES *hDevNames;
		HANDLE hDevMode;
		char *lpszBuffer,*lpszBase;

		hDevMode = prDialog.m_pd.hDevNames;
		if((hDevNames = (DEVNAMES *)GlobalLock(hDevMode)) == NULL)
		{
			sysprint(0,REPORT.ThreadId,"[REPORT]: havn't default print");
			break;
		}

		lpszBuffer = (char *)hDevNames;
		lpszBase = lpszBuffer;

		// set printer name
		lpszBuffer = lpszBase + hDevNames->wDeviceOffset;
		strcpy(lpszBuffer,"HP LaserJet 5L (NET)");
		lpszBuffer[strlen(lpszBuffer)] = 0x00;
		hDevNames->wOutputOffset = strlen(lpszBuffer) + hDevNames->wDeviceOffset + 1;
		// set printer port
		lpszBuffer = lpszBase + hDevNames->wOutputOffset;
		strcpy(lpszBuffer,"\\\casco-triumph\\HP LaserJet 5L");
		lpszBuffer[strlen(lpszBuffer)] = 0x00;
		//change default flag
		hDevNames->wDefault = FALSE;
		GlobalUnlock(hDevMode);

		// modify device mode information
		hDevMode = prDialog.m_pd.hDevMode;
		// select laser print ,using A4 paper;
		if((pDevMode = (DEVMODE *)GlobalLock(hDevMode)) == NULL)
			break;
		strcpy((char *)pDevMode->dmDeviceName,abPrinterName);
		pDevMode->dmFields |= ( DM_PAPERLENGTH | DM_PAPERWIDTH | DM_SCALE);		
		pDevMode->dmOrientation =  DMORIENT_PORTRAIT;
		pDevMode->dmScale = 100;
		pDevMode->dmPaperSize = DMPAPER_A4;
		pDevMode->dmPaperLength = 2970;
		pDevMode->dmPaperWidth = 2100;

		GlobalUnlock(hDevMode);

		HDC prnDC;

		if((prnDC = prDialog.CreatePrinterDC( )) == NULL)
		{
			sysprint(0,REPORT.ThreadId,"[REPORT]: create print dc failed");
			break;
		}

		CDC printDC;

		printDC.Attach(prnDC);

		DOCINFO printDocumentName;
		char sPrintDocName[ ] = "report";
		printDocumentName.cbSize = sizeof(DOCINFO);
		printDocumentName.lpszDocName = sPrintDocName;
		printDocumentName.lpszOutput = NULL;
		printDC.StartDoc(&printDocumentName);

		CRect clipBox;

/*		printDC.GetClipBox(&clipBox);
		printDC.StretchBlt(clipBox.left,clipBox.top,
			clipBox.Width( ),clipBox.Height( ),
			&m_dcMemory,clipBox.left,clipBox.top,m_nPageWidth,m_nPageHeight,
			SRCCOPY);
*/
			if(*value ==1) //speed statistics
				output(&printDC);
			else //punctual statistics
				output2(&printDC);

		printDC.EndDoc( );
		printDC.Detach( );
		DeleteDC(prnDC);
		theStatus = 0;
	}while(0);
}

void CPrintStastics::Create(CWnd* pParent)
{
	CDialog::Create(CPrintStastics::IDD,pParent);
}

BOOL CPrintStastics::OnInitDialog() 
{
	
	CDialog::OnInitDialog();
	
	m_nPageWidth=2500;
	m_nPageHeight=1500;
	do {
	// TODO: Add extra initialization here
		CDC *pDC;
		pDC = GetDC( );
		if(pDC == NULL)
			break;

		// for schedule
		if(m_MemBitmap.CreateCompatibleBitmap(pDC,
			m_nPageWidth,
			m_nPageHeight ) == 0)
			break;
		if(m_dcMemory.CreateCompatibleDC(pDC) == 0)
			break;
		// history and work schedule bitmap width 24 hour
		m_pOldBitmap = m_dcMemory.SelectObject(&m_MemBitmap);

		m_dcMemory.FillRect(CRect(0,0,
			m_nPageWidth,
			m_nPageHeight),&m_brushBackground);
		m_dcMemory.SetBkColor(CLR_WHITE);

	ReleaseDC(pDC);
	}while(0);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPrintStastics::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	CDC *pCDC = &dc;

	if(!pCDC->IsPrinting( ))
	{
		CRect clipBox;

		GetClientRect(&clipBox);
		//pCDC->BitBlt(clipBox.left,clipBox.top,
		//	clipBox.Width( ),clipBox.Height( ),
		//	&m_dcMemory,clipBox.left,clipBox.top,
		//	SRCCOPY);
		pCDC->StretchBlt(0,50, clipBox.right-clipBox.left,clipBox.bottom-clipBox.top-50,&m_dcMemory,
			             0,0,m_nPageWidth,m_nPageHeight,SRCCOPY);
	} else {
	}	
	// Do not call CDialog::OnPaint() for painting messages
}
