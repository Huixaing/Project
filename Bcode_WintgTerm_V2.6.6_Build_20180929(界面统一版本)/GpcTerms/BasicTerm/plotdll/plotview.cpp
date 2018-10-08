// PlotView.cpp : implementation file
//

#include "stdafx.h"


#include "bureau.h"
#include "PlotView.h"
#include "dlgwait.h"
#include "dbfunc.h"
#include "function.h"
#include "tgmsg.h"
#include "plotdoc.h"    ///
#include <winspool.h>

#include <io.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//#define CLR_PLOT RGB(100,200,100)

/////////////////////////////////////////////////////////////////////////////
// CPlotView
CView *clsPlotView; 

IMPLEMENT_DYNCREATE(CPlotView, CScrollView)

CPlotView::CPlotView()
{
	//{{AFX_DATA_INIT(CPlotView)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_brushBackground.CreateSolidBrush(CLR_WHITE);

	nFlag=0;
	m_pOldBitmap = NULL;
}

CPlotView::~CPlotView()
{
	m_brushBackground.DeleteObject();

	if(m_pOldBitmap != NULL)
	{  		
		m_dcMemory.SelectObject(m_pOldBitmap);
		m_dcMemory.DeleteDC( );
		m_MemBitmap.DeleteObject();
	}

	clsPlotView = NULL;
}

BEGIN_MESSAGE_MAP(CPlotView, CScrollView)
	//{{AFX_MSG_MAP(CPlotView)
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPlotView diagnostics

#ifdef _DEBUG
void CPlotView::AssertValid() const
{
	CScrollView::AssertValid();
}

void CPlotView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}

CPlotDoc *CPlotView::GetDocument( )
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CPlotDoc)));
	return (CPlotDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CPlotView message handlers

void CPlotView::OnInitialUpdate() 
{
	CScrollView::OnInitialUpdate();
}

void CPlotView::OnDraw(CDC* pCDC) 
{
	// TODO: Add your specialized code here and/or call the base class
	if(!pCDC->IsPrinting( ))
	{
		CRect clipBox;

		pCDC->GetClipBox(&clipBox);
		pCDC->BitBlt(clipBox.left,clipBox.top,
			clipBox.Width( ),clipBox.Height( ),
			&m_dcMemory,clipBox.left,clipBox.top,
			SRCCOPY);
	} 
	else 
	{
	}
}

BOOL CPlotView::OnEraseBkgnd(CDC* pCDC) 
{
	// TODO: Add your message handler code here and/or call default
	CRect ClipRect;
	pCDC->GetClipBox(&ClipRect);
	pCDC->FillRect(&ClipRect,&m_brushBackground);
	return TRUE;	
}

BOOL CPlotView::OnPreparePrinting(CPrintInfo* pInfo) 
{
	return DoPreparePrinting(pInfo);
}

void CPlotView::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo) 
{
	CScrollView::OnBeginPrinting(pDC, pInfo);
}

int CPlotView::IsSideInfoExist(ushort nStationNo,ushort phase,char *sSideName)
{	
	int id = GetStationidInList(nStationNo);	
	if(id == -1) return -1;
	_tagSideStoredTrain *sideinfo;
	for(int j=0;j<Info[id][phase].usSideNum;j++)
	{
		sideinfo = &Info[id][phase].sideinfo[j];
		if(!strcmp(sideinfo->pszSideName,sSideName))
			return j;		   
	}
	return -1;
}
int CPlotView::GetStationidInList(int nStationNo)
{ 
	CPlotDoc *pDoc = GetDocument();
	int nStationNum;
	int nStationList[64];
	nStationNum=sizeof(nStationList)/sizeof(nStationList[0]);
	gpcdef.GetSectionPhaseStoreStationList(pDoc->m_nCurrentSectionNo,nStationNum,nStationList);


	if(nStationNo <=0) return -1;
	for(int i=0;i<nStationNum;i++)
		if(nStationList[i] == nStationNo)
			return i;
	//not found this station in list;
	if(i == nStationNum)
		return -1;

	return i;
}

bool CPlotView::CreateRecursiveDir(char* filename)
{
	if(strlen(filename)>MAX_PATH)
		return false;

	char filepath[MAX_PATH+1];
	char newfilename[MAX_PATH+1];

	memset(filepath,0,sizeof(filepath));
	memset(newfilename,0,sizeof(newfilename));
	strcpy(newfilename,filename);

	//replace the / 
	int filenamelen = strlen(newfilename);
	for(int i=0; i<filenamelen;i++)
	{
		if(newfilename[i] ==  '/')
			newfilename[i] = '\\';
	}
	
	char* string = newfilename;
	char seps[]   = "\\";	
	char *token;
	
	//去掉尾部的文件名字
	char* rdelim;
	rdelim = strrchr(string,'\\');
	
	if(rdelim != NULL)
	{
		*rdelim = '\0';
	}
	else
	{
		return true;
	}
	
	if(strlen(string)<2)
		return false;
	
	//for network address create, exp. \\172.20.48.51\sharedocs\
	//copy the first two character,
	if(strncmp(string,"\\\\",2) == 0)
	{
		strncpy(filepath,string,2);
		filepath[2] = '\0';
		string += 2;
	}
	else
	{
		strncpy(filepath,string,3);
		filepath[3] = '\0';
		string += 3;
	}


	/* Establish string and get the first token: */
	token = strtok( string, seps );
	while( token != NULL )
	{
		//判断该目录是否已经存在，如果不存在，建立
		strcat(filepath,token);
		strcat(filepath,"\\.");
		if( (_access(filepath, 0)) != -1 )//存在
		{
			filepath[strlen(filepath)-1]='\0';
		}
		else
		{
			filepath[strlen(filepath)-1]='\0';
			mkdir(filepath);
		}
		/* Get next token: */
		token = strtok( NULL, seps );
	}
	
	
	return true;
}
