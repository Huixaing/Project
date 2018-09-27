// DlgMemory.cpp : 实现文件
//

#include "stdafx.h"
#include "resource.h"
#include "DlgMemory.h"
#include <list>
#include "./GridCtrl_src/gridcellcheck.h"
#include "utility/xml_utility.h"
#include "sysshare.h"
#include "utility/ctc_data_type_def.h"
#include "tg_function_config.h"
#include "tg_schedule_base.h"
#include "tgpspdata.h"
#include "tgmsg.h"
#include ".\dlgmemory.h"

#import  <msxml.dll> named_guids
using namespace MSXML;

#define IDC_NOTE_GRID  6001
/////////// Global stuff ///////////////
#define TEYUN_NOTIFY_TYPE 2
#define MARK_NOTIFY_TYPE  1
#define TEXT_NOTIFY_TYPE  0
struct MEMORY_NOTIFY_DEF{
  DWORD   nID;
  int     nType;
  DWORD   nObjectIndex;
  char    szNotestr[256];
  time_t  tBeginNotifyTime;
  //time_t  tEndNotifyTime;
  int     nClrType;
  BOOL    bFinished;
  MEMORY_NOTIFY_DEF()
  {
    nID=0;
	memset(szNotestr,0,sizeof(szNotestr));
    tBeginNotifyTime=0;
    //tEndNotifyTime=0;

    bFinished=FALSE;
	nType=TEXT_NOTIFY_TYPE;
	nObjectIndex=0;
	nClrType=1;
  }
};

list<MEMORY_NOTIFY_DEF> g_notify_list;
DWORD g_bMemoryNotifing = 0;
DWORD g_bMemoryNotifyWaiting = 0;

BOOL   CheckAndLaunchMemoryNotify()
{
	time_t tm;
	time(&tm);

	list<MEMORY_NOTIFY_DEF>::iterator iter=g_notify_list.begin();
	while(iter!=g_notify_list.end())
	{
		MEMORY_NOTIFY_DEF& item = *iter;

		if(!item.bFinished)
		{
			if(item.tBeginNotifyTime <= tm +60)
			{
				g_bMemoryNotifing = 1;
				return TRUE;
			}
		}

		iter++;
	}

	g_bMemoryNotifing = 0;

	return FALSE;
}

void RemoveMemoryNotifyByObject(int type,DWORD objindex)
{
	list<MEMORY_NOTIFY_DEF>::iterator iter=g_notify_list.begin();
	while(iter!=g_notify_list.end())
	{
		if(iter->nType==type && iter->nObjectIndex==objindex)  
		{
			 g_notify_list.erase(iter);
			 break;
		}
		iter++;
	}
}

DWORD  AddToMemoryNotify(const char *desc,time_t begintime,int repeat,int type,DWORD objindex, int clrtype=1)
{
	MEMORY_NOTIFY_DEF memo;
	DWORD id=begintime;
	list<MEMORY_NOTIFY_DEF>::iterator iter=g_notify_list.begin();
	list<MEMORY_NOTIFY_DEF>::iterator iter_insert=iter;
	while(iter!=g_notify_list.end())
	{
		if(type==0 && iter->tBeginNotifyTime==begintime && !stricmp(iter->szNotestr,desc)) 
			return iter->nID;
		if(type>0 && iter->nType==type && iter->nObjectIndex==objindex) 
		{
			iter->tBeginNotifyTime=begintime;
			//iter->tEndNotifyTime=begintime;
			strncpy(iter->szNotestr,desc,sizeof(iter->szNotestr)-1);
			return iter->nID;
		}
		if(iter->nID>=id)
			id=iter->nID+1;
		if(iter->tBeginNotifyTime<begintime)
		{
			iter++;
			iter_insert=iter;
		}
		else
			iter++;
	}
	memo.nID=id;
	memo.nType=type;
	memo.nObjectIndex=objindex; 
	memo.tBeginNotifyTime=begintime;
	//memo.tEndNotifyTime=begintime;
	strncpy(memo.szNotestr,desc,sizeof(memo.szNotestr)-1);
	memo.nClrType=clrtype;
	g_notify_list.insert(iter_insert,memo);
	return id;
}

void  RemoveMarkMemoryNotify(DWORD markindex)
{
	RemoveMemoryNotifyByObject(MARK_NOTIFY_TYPE,markindex); 
}

DWORD  AddMarkToMemoryNotify(const CTG_SingleMark *pmark)
{
	//if(pmark->GetMarkType()==TG_GRAPH_NOTE || pmark->GetMarkType()==TG_SIDE_OCCUPY || pmark->GetMarkType()==TG_CHANGE_BLOCK_TYPE)
	//	return 0xFFFFFFFF;
	if(pmark->GetMarkType()!=TG_ABS_SLOW &&  pmark->GetMarkType()!=TG_SIDE_SLOW)
		return 0xFFFFFFFF;
	CTime curtm=CTime::GetCurrentTime(); 
	if(pmark->GetStartTime()<curtm.GetTime())
		return 0;
    char  notestr[512];
	CString pmarkstr=GetObjectWorkInfo(pmark);
	if(pmarkstr.IsEmpty())
		return 0;
	if(pmarkstr.GetLength()>100)
		sprintf(notestr,"请及时下达施工命令");
	else
		sprintf(notestr,"请及时下达施工或者限速命令:%s",pmarkstr);
	return AddToMemoryNotify(notestr,pmark->GetStartTime()-3600,1,MARK_NOTIFY_TYPE,pmark->GetBlockIndex());
}

void  SetMemoryNotifyFinished(DWORD id,BOOL finished)
{
    list<MEMORY_NOTIFY_DEF>::iterator iter=g_notify_list.begin();
	while(iter!=g_notify_list.end())
	{
		if(iter->nID==id)
		{
			if(iter->bFinished != finished)
			{
				iter->bFinished = finished;
				if(!finished)
				{
					time_t tm;
					time(&tm);
					iter->tBeginNotifyTime = tm;	//重新开始提醒
					//iter->tEndNotifyTime = tBeginNotifyTime;
				}
			}
			break;
		}
		iter++;
	}
}

MEMORY_NOTIFY_DEF *GetMemoryByID(DWORD id)
{
	list<MEMORY_NOTIFY_DEF>::iterator iter=g_notify_list.begin();
	while(iter!=g_notify_list.end())
	{
		if(iter->nID==id)
		{
			return &*iter;
		}
		iter++;
	}
	return NULL;
}

void EraseMemoryByID(DWORD id)
{
	list<MEMORY_NOTIFY_DEF>::iterator iter=g_notify_list.begin();
	while(iter!=g_notify_list.end())
	{
		if(iter->nID==id)
		{
			g_notify_list.erase(iter);
			break;
			 
		}
		iter++;
	}
}

#define MEMORY_XML_NODE "MEMORY"
BOOL LoadMemoryFromFile()
{
	time_t tm;
	time(&tm);

	CString xml;
	g_notify_list.clear();
	if(!ReadXMLFile(xml,MEMORY_XML_NODE,"SAVE"))
		return FALSE;
	LOAD_XML(xml,MEMORY_XML_NODE,pNodeRoot)
	MSXML::IXMLDOMNodeListPtr pList = pNodeRoot->selectNodes( "RECORD" );
	int rec_count = pList->Getlength();
    for(int i=0;i<rec_count;i++)
	{
		   MEMORY_NOTIFY_DEF memo;
		   GET_NODE_INTVALUE(memo.nID,pList->Getitem(i),"Id");
		   GET_NODE_INTVALUE(memo.nType,pList->Getitem(i),"Type");
		   GET_NODE_INTVALUE(memo.nObjectIndex,pList->Getitem(i),"ObjectIndex");
		   GET_NODE_TIMEVALUE(memo.tBeginNotifyTime,pList->Getitem(i),"BeginTime");
		   //GET_NODE_TIMEVALUE(memo.tEndNotifyTime,pList->Getitem(i),"EndNotifyTime");
		   GET_NODE_INTVALUE(memo.nClrType,pList->Getitem(i),"ClrType");
		   CString tmpstr;
		   GET_NODE_STRVALUE(tmpstr,pList->Getitem(i),"NoteStr");
		   strncpy(memo.szNotestr,tmpstr,sizeof(memo.szNotestr)-1);
		   int tmp=0;
		   GET_NODE_INTVALUE(tmp,pList->Getitem(i),"Finished");
		   if(tmp>0)
			   memo.bFinished=TRUE;
		   else
			   memo.bFinished=FALSE;

		   if(!memo.bFinished)
				g_notify_list.push_back(memo);
	 }
	  return TRUE;
}
BOOL SaveMemoryToXML(CString &xml)
{
	time_t curtm;
	time(&curtm);

	xml="<";
	xml+=MEMORY_XML_NODE;
	xml+=">";
    list<MEMORY_NOTIFY_DEF>::iterator iter=g_notify_list.begin();
	while(iter!=g_notify_list.end())
	{
		MEMORY_NOTIFY_DEF& memo = *iter;

		xml+="<RECORD>";
	
	    InsertXMLNodeTime(xml,"BeginTime",iter->tBeginNotifyTime); 
	    //InsertXMLNodeTime(xml,"EndNotifyTime",iter->tEndNotifyTime); 
	    InsertXMLNodeValue(xml,"Id",iter->nID);
		InsertXMLNodeValue(xml,"NoteStr",iter->szNotestr);
		InsertXMLNodeValue(xml,"Finished",iter->bFinished);
		InsertXMLNodeValue(xml,"ITEMDATA1",iter->nID);
		InsertXMLNodeValue(xml,"ObjectIndex",iter->nObjectIndex);
		InsertXMLNodeValue(xml,"Type",iter->nType);
		InsertXMLNodeValue(xml,"ClrType",iter->nClrType);
				
		InsertXMLNodeValue(xml,"BeginTimeDisp",GetCTCValueDispString(CTC_DATA_DATETIME,iter->tBeginNotifyTime,0)); 
		//InsertXMLNodeValue(xml,"EndNotifyTimeDisp",GetCTCValueDispString(CTC_DATA_DATETIME,iter->tEndNotifyTime,0)); 

		if(memo.bFinished)
		{
			InsertXMLNodeValue(xml,"COLOR","GRAY");
			InsertXMLNodeValue(xml,"SELECTED",1);
		}
		else
		{
			if(curtm > memo.tBeginNotifyTime)	//正在提醒
			{
				if(iter->nClrType==1)
					InsertXMLNodeValue(xml,"COLOR","RED");
				else if(iter->nClrType==2)
					InsertXMLNodeValue(xml,"COLOR","GREEN");
				else if(iter->nClrType==3)
					InsertXMLNodeValue(xml,"COLOR","BLUE");
			}
			else	//等待提醒
				InsertXMLNodeValue(xml,"COLOR","WHITE");
		}
		xml+="</RECORD>";
		iter++;
	}

	xml+="</";
	xml+=MEMORY_XML_NODE;
	xml+=">";
	return TRUE;
}

BOOL SaveMemoryToFile()
{
	CString xml;
	SaveMemoryToXML(xml);
	return WriteXMLFile(xml,MEMORY_XML_NODE,"SAVE");
}

BOOL  LoadDefaultNotify()
{
	CTime curtm=CTime::GetCurrentTime();
	for(int i=0;i<GetTGFunctionConfig()->GetNoifyCount();i++)
	{
		int daytime;
		char desc[128];
		if(GetTGFunctionConfig()->GetNotifyDesc(i,&daytime,desc,128))
		{
			CTime notitm=CTime(curtm.GetYear(),curtm.GetMonth(),curtm.GetDay(),daytime/60,daytime%60,0);  
			if(notitm<curtm)
			{
				notitm+=CTimeSpan(1,0,0,0);
			}
			AddToMemoryNotify(desc,notitm.GetTime(),1,0,0);
		}
	}
	return TRUE;
}

////////////////////////////////////////////

// CDlgMemory 对话框

IMPLEMENT_DYNAMIC(CDlgMemory, CDialog)
CDlgMemory::CDlgMemory(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgMemory::IDD, pParent)
{
	m_szNote="";
	m_currentRow=-1;
}

BOOL CDlgMemory::CreateMe(CWnd* pParent)
{
	LoadMemoryFromFile();
	LoadDefaultNotify();
	return Create(CDlgMemory::IDD, pParent);
}

CDlgMemory::~CDlgMemory()
{
}

void CDlgMemory::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgMemory)
	DDX_Text(pDX, IDC_ED_NOTESTR, m_szNote);
	DDX_Control(pDX, IDC_NOTIFY_DATE, m_cBeginNotifyTime);
	DDX_Control(pDX, IDC_NOTIFY_TIME, m_cEndNotifyTime);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgMemory, CDialog)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BTN_ADD, OnBnClickedBtnAdd)
	ON_BN_CLICKED(IDC_BTN_EDIT, OnBnClickedBtnEdit)
	ON_BN_CLICKED(IDC_BTN_DELETE, OnBnClickedBtnDelete)
	ON_BN_CLICKED(IDC_BTN_CLOSE, OnBnClickedBtnClose)
	ON_NOTIFY(GVN_SELCHANGING,IDC_NOTE_GRID,OnGridSelectChange)
	ON_BN_CLICKED(IDC_BTN_EDIT2, OnBnClickedBtnEdit2)
END_MESSAGE_MAP()

PropertyInfoPara memory_note_def[]=
{
	{1,0,CTC_DATA_BOOL,"Finished","是否完成"},
	{1,0,CTC_DATA_DATETIME,"BeginTimeDisp","开始提醒.时间"}, // 包括负责人
	//{0,0,CTC_DATA_DATETIME,"EndNotifyTimeDisp","结束提醒.时间"},
	//{0,0,CTC_DATA_INT,"Repeat","重复(分)"},
	{0,0,CTC_DATA_STR,"NoteStr","事情概述"},
};

BOOL CDlgMemory::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	//time(&tmStart);
	//tmEnd = tmStart;
	
	//CTime ct(tmEnd);
	//mTimeEnd.SetTime(&ct);
	m_cBeginNotifyTime.SetFormat("yyy'-'MM'-'dd  HH':'mm");
	
	//ct -= CTimeSpan(0,1,59,0);
	//mTimeBegin.SetTime(&ct);
	m_cEndNotifyTime.SetFormat("yyy'-'MM'-'dd  HH':'mm");
	
	SetTimer(1973,60000,NULL);
	// TODO: Add extra initialization here
	CRect rect;
	GetDlgItem(IDC_GRID_FRAME)->GetWindowRect(&rect);
	ScreenToClient(&rect);
	m_grid.m_bRowSelectable=TRUE;
	m_grid.CreateGrid(this,rect,IDC_NOTE_GRID,memory_note_def,sizeof(memory_note_def)/sizeof(memory_note_def[0]));
	m_grid.AutoSizeColumns();
	m_grid.ExpandLastColumn();
	RefreshShow();
	InitDlgPosition(this);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgMemory::RefreshShow()
{
	CString xml;
	SaveMemoryToXML(xml);
	CString nodestr=MEMORY_XML_NODE;
	nodestr+="/RECORD";
	m_grid.DeleteNonFixedRows();
	m_grid.InsertRows(xml,nodestr);

	CheckAndLaunchMemoryNotify();	//wsg
}

void CDlgMemory::OnTimer(UINT nIDEvent) 
{
	if(1973 == nIDEvent)
	    CheckAndLaunchMemoryNotify();
	return ;

	if(g_bMemoryNotifing == 0)
	{
		if(g_bMemoryNotifyWaiting)
		{
			time_t tm;
			time(&tm);

			g_bMemoryNotifyWaiting = 0;

			list<MEMORY_NOTIFY_DEF>::iterator iter=g_notify_list.begin();
			for(; iter!=g_notify_list.end(); iter++)
			{
				MEMORY_NOTIFY_DEF& item = *iter;

				if(item.bFinished)
					continue;

				if(item.tBeginNotifyTime > tm)
					g_bMemoryNotifyWaiting ++;
				else
					g_bMemoryNotifing ++;
			}
		}

		if(CWnd *pWnd = GetParent())
		{
			if(g_bMemoryNotifing)
				pWnd->SendMessage(UM_MEMORY_NOIFY, 1, 1);
		}
	}
	

	CDialog::OnTimer(nIDEvent);
}
// CDlgMemory 消息处理程序
void CDlgMemory::OnGridSelectChange(NMHDR* pNMHDR, LRESULT* pResult) 
 {
	 NM_GRIDVIEW *pv=(NM_GRIDVIEW *)pNMHDR;
	 m_currentRow=pv->iRow;
	 if(m_currentRow>=1)
	 {
		 DWORD id=m_grid.GetCell(m_currentRow,1)->GetData();
		 MEMORY_NOTIFY_DEF *pdef=GetMemoryByID(id);
		 
		 if(pdef)
		 {
			 m_szNote=pdef->szNotestr;
			 m_cBeginNotifyTime.SetTime(pdef->tBeginNotifyTime);
			 UpdateData(FALSE);
		 }
	 }
 }
void CDlgMemory::OnBnClickedBtnAdd()
{
	 UpdateData(TRUE);

	 CTime ctBegin, ctEnd;
	 m_cBeginNotifyTime.GetTime(ctBegin);
     m_cEndNotifyTime.GetTime(ctEnd);
	
	 DWORD id=AddToMemoryNotify(m_szNote,ctBegin.GetTime(),0,0,0);
	 SetMemoryNotifyFinished(id, false);
	 RefreshShow();
	 SaveMemoryToFile();
	 m_currentRow=-1;
}

void CDlgMemory::OnBnClickedBtnEdit()
{
	UpdateData(TRUE);
	if(m_grid.GetRowCount()<=1)
	{
		MessageBox("没有可以修改的备忘!!!", "提示", MB_OK);
		return;
	}
	if(m_currentRow<1)
	{
		MessageBox("请先选中要修改的备忘!!!", "提示", MB_OK);
		return;
	}
	if(m_currentRow < m_grid.GetRowCount())
	{
		DWORD id=m_grid.GetCell(m_currentRow,1)->GetData();
		MEMORY_NOTIFY_DEF *pdef=GetMemoryByID(id);
		if(pdef)
		{
			strncpy(pdef->szNotestr,m_szNote,sizeof(pdef->szNotestr)-1);
			CTime ctBegin, ctEnd;
			m_cBeginNotifyTime.GetTime(ctBegin);
			m_cEndNotifyTime.GetTime(ctEnd);
			if(ctEnd < ctBegin)
				ctEnd = ctBegin;
			
			pdef->tBeginNotifyTime=ctBegin.GetTime();

			RefreshShow();
			SaveMemoryToFile();
		}
	}
}

void CDlgMemory::OnBnClickedBtnDelete()
{
	for(int row=1; row<m_grid.GetRowCount(); row++)
	{
		CGridCellCheck* pCheck = (CGridCellCheck*)m_grid.GetCell(row, 0);
		if(pCheck->GetCheck())
		{
			DWORD id=m_grid.GetCell(row, 1)->GetData();
			EraseMemoryByID(id);
		}
	}

	RefreshShow();
	SaveMemoryToFile();
	m_currentRow=-1;
}

void CDlgMemory::OnBnClickedBtnClose()
{
	ShowWindow(SW_HIDE);
}


// 不要提醒
void CDlgMemory::OnBnClickedBtnEdit2()
{
	for(int row=1; row<m_grid.GetRowCount(); row++)
	{
		CGridCellCheck* pCheck = (CGridCellCheck*)m_grid.GetCell(row, 0);
		if(pCheck->GetCheck())
		{
			DWORD id=m_grid.GetCell(row, 1)->GetData();
			MEMORY_NOTIFY_DEF *pdef=GetMemoryByID(id);
			if(pdef)
			{
				pdef->bFinished=TRUE;
			}
		}
	}

	RefreshShow();
	SaveMemoryToFile();
	m_currentRow=-1;
}
