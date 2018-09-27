// TgDocument.cpp : 实现文件
//

#include "stdafx.h"
#include "sysshare.h"
#include "tg.h"
#include "TgDocument.h"
#include "schd_newserver.h"
#include "schd_database.h"
#include "stplan/stplan.h"
#include "function.h"
#include "icommtask.h"
#include "utility/xml_utility.h"
#include "dlgwait.h"
#include "tgmsg.h"
#include "NoticeNeibRecvPlanDlg.h"
#define MAX_STORED_DOC_NUM 48
#define MAX_DYN_STORED_DOC_NUM 48

#import  <msxml.dll> named_guids
using namespace MSXML;

static struct
{
	int  nSectionID;
	CTG_ScheduleInfo::SCHEDULE_TYPE  nScheduleType;
	CTgDocument *pTgDoc;
}g_tgdocs[MAX_STORED_DOC_NUM], g_dyntgdocs[MAX_DYN_STORED_DOC_NUM];

extern TG_STATION_STATUS *GetStationStatusPtr(int station); // defined in tg_logic_main.cpp
extern int DB_GetEntityStatus(long eid);

static BOOL UnRegisterDynDoc(CTgDocument *p)
{
	int i;
	for(i=0;i<MAX_DYN_STORED_DOC_NUM;i++)
	{
		if(g_dyntgdocs[i].pTgDoc==p)
		{
			g_dyntgdocs[i].pTgDoc=NULL;
			g_dyntgdocs[i].nSectionID=0;
			g_dyntgdocs[i].nScheduleType=CTG_ScheduleInfo::SCHEDULE_TYPE_NULL;
			break;
		}
	}
	if(i>=MAX_DYN_STORED_DOC_NUM)
		return FALSE;

	return TRUE;
}

static BOOL UnRegisterDoc(CTgDocument *p)
{
	int i;
	for(i=0;i<MAX_STORED_DOC_NUM;i++)
	{
		if(g_tgdocs[i].pTgDoc==p)
			break;
	}
	if(i>=MAX_STORED_DOC_NUM)
		return FALSE;
	g_tgdocs[i].pTgDoc=NULL;
	return TRUE;
}

// CTgDocument
BOOL CTgDocument::IsSendCloseWithSvr(int con_id, CTG_ScheduleInfo::SCHEDULE_TYPE type)
{
	int i;
	for(i=0;i<MAX_STORED_DOC_NUM;i++)
	{
		if(g_tgdocs[i].nSectionID==con_id && g_tgdocs[i].nScheduleType == type)
			return FALSE;
	}

	return TRUE;
}

BOOL CTgDocument::InitTGDocuments()
{
	int i;
	for(i=0;i<MAX_STORED_DOC_NUM;i++)
	{
		g_tgdocs[i].pTgDoc=NULL;

	}

	for(i=0;i<MAX_DYN_STORED_DOC_NUM;i++)
	{
		g_dyntgdocs[i].pTgDoc=NULL;

	}
	return TRUE;
}

CTgDocument *CTgDocument::CreateTgDyncDocument(int section_id,CTG_ScheduleInfo::SCHEDULE_TYPE type,int shift_id,BOOL localmode,BOOL newserver)  // STATIC!!
{
	int i;
	CTgDocument *pRet=NULL;
	pRet=new CTgDocument();
	if(!pRet)
		return NULL;
	if(!pRet->InitLogicDataDync(section_id,type,localmode,newserver))
	{
		delete pRet;
		return NULL;
	}
	pRet->LoadSchedule(shift_id);  
	pRet->SetAutoDelete(TRUE); 

	if(type == CTG_ScheduleInfo::SCHEDULE_TYPE_WORK)
	{
		for(i=0;i<MAX_DYN_STORED_DOC_NUM;i++)
		{
			if(NULL == g_dyntgdocs[i].pTgDoc)
			{
				g_dyntgdocs[i].pTgDoc=pRet;
				g_dyntgdocs[i].nSectionID=section_id;
				g_dyntgdocs[i].nScheduleType=type;
				break;
			}
		}
	}
	return pRet;
}

CTgDocument *CTgDocument::CreateTgDocument(int section_id,CTG_ScheduleInfo::SCHEDULE_TYPE type,int shift_id,BOOL localmode,BOOL newserver)  // STATIC!!
{
	int i;
	CTgDocument *pRet=NULL;
	int  noused=-1;
	for(i=0;i<MAX_STORED_DOC_NUM;i++)
	{
		if(g_tgdocs[i].pTgDoc!=NULL && g_tgdocs[i].nScheduleType==type && g_tgdocs[i].nSectionID==section_id)
		{
			pRet=g_tgdocs[i].pTgDoc;
			break;
		}
		if((g_tgdocs[i].pTgDoc!=NULL)&&(IS_BASIC_SCHD(type)) && (g_tgdocs[i].nSectionID==section_id)
			&& g_tgdocs[i].nScheduleType==CTG_ScheduleInfo::SCHEDULE_TYPE_WORK)
		{
			pRet=g_tgdocs[i].pTgDoc;
			break;
		}
		if(g_tgdocs[i].pTgDoc!=NULL && (type==CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT) && (g_tgdocs[i].nSectionID==section_id)
			&& g_tgdocs[i].nScheduleType==CTG_ScheduleInfo::SCHEDULE_TYPE_WORK)
		{
			pRet=g_tgdocs[i].pTgDoc;
			break;
		}
		if(g_tgdocs[i].pTgDoc==NULL && noused==-1)
			noused=i;
	}
	if(pRet)
		return pRet;
	if(noused<0)
		return NULL;
	pRet=new CTgDocument();
	if(!pRet)
		return NULL;
	if(type==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC ||type==CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT )
		type=CTG_ScheduleInfo::SCHEDULE_TYPE_WORK;   // 单独调阅基本图时，按当前实际图处理？ NA???
	if(!pRet->InitLogicData(section_id,type,localmode,newserver))
	{
		delete pRet;
		return NULL;
	}
	pRet->LoadSchedule(shift_id);  
	g_tgdocs[noused].pTgDoc=pRet;
	g_tgdocs[noused].nSectionID=section_id;
	g_tgdocs[noused].nScheduleType=type;
	return pRet;
}

void CTgDocument::DeleteAllTgDocuments()
{
	int i;
	for(i=0;i<MAX_STORED_DOC_NUM;i++)
	{
		if(g_tgdocs[i].pTgDoc)
		{
			delete g_tgdocs[i].pTgDoc;
			g_tgdocs[i].pTgDoc=NULL;
		}
	}
}

BOOL  CTgDocument::GlobalProcessMessage(long eid,BaseMsg *pBuf)
{
	int i;
	for(i=0;i<MAX_STORED_DOC_NUM;i++)
	{
		if(g_tgdocs[i].pTgDoc)
		{
			g_tgdocs[i].pTgDoc->ProcessGenMessage(eid,pBuf);
		}
	}
	return TRUE;
}

BOOL CTgDocument::GlobalAutoShiftSchedule()
{
	if( !gpcdef.IsNewAutoMoveMode())
	{
		//太原 20101017  调度员权限不让自动交接班
		if(gpcdef.GetUserIdentify() == GPC_DISPATCHER)
			return TRUE;
	}
	
	int i;
	for(i=0;i<MAX_STORED_DOC_NUM;i++)
	{
		if(g_tgdocs[i].pTgDoc)
		{
			g_tgdocs[i].pTgDoc->AutoShiftSchedule(); 
		}
	}
	for(i=0;i<MAX_DYN_STORED_DOC_NUM;i++)
	{
		if(g_dyntgdocs[i].pTgDoc)
		{
			g_dyntgdocs[i].pTgDoc->AutoShiftSchedule(); 
		}
	}
	return TRUE;
}

BOOL CTgDocument::GlobalLoadFromLocalFiles()
{
	int i;
	for(i=0;i<MAX_STORED_DOC_NUM;i++)
	{
		if(g_tgdocs[i].pTgDoc)
		{
			g_tgdocs[i].pTgDoc->GetTGDataHolder()->LoadDataFromLocalFiles();
		}
	}
	return TRUE;
}

BOOL CTgDocument::GlobalSaveToLocalFiles()
{
	int i;
	for(i=0;i<MAX_STORED_DOC_NUM;i++)
	{
		if(g_tgdocs[i].pTgDoc)
		{
			g_tgdocs[i].pTgDoc->GetTGDataHolder()->SaveDataToLocalFiles();
		}
	}
	return TRUE;
}

BOOL CTgDocument::GlobalProcessStationStatusInform(int station)
{
	int i;
	for(i=0;i<MAX_STORED_DOC_NUM;i++)
	{
		if(g_tgdocs[i].pTgDoc)
		{
			g_tgdocs[i].pTgDoc->ProcessStationStatusInform(station);
		}
	}
	return TRUE;
}

IMPLEMENT_DYNCREATE(CTgDocument, CDocument)

CTgDocument::CTgDocument()
{
	m_pCrewNoticeDlg=NULL;
	m_bAutoDelete=FALSE;
	m_pTrainDepartPredictData=NULL;
	m_pCurrentWorkScheduleData=NULL;
	m_pWintgNewServerWork=NULL;
	m_pWintgNewServerShift=NULL;
	m_pWintgNewServerBasic=NULL;
	m_pWintgNewServerBasicPrev=NULL;
	m_pWintgNewServerBasicBackup=NULL;
	m_tmPlanSendRange=3;
	m_nReceiveNeighberLastActivePage=0;
	memset(&m_cPhaseNotes,0,sizeof(m_cPhaseNotes));
	m_pActiveObject=NULL;
	m_nCountOfPlan=0;
	m_cMMplanMarks.clear();
	m_pWaitWnd=NULL;
	m_bQueryMode=TRUE;
	m_pStplan=NULL;
	m_nActiveObjectIndex=0;
	m_nActiveObjectType=TG_OBJ_NULL;
	m_nActiveObjectScheduleType=CTG_ScheduleInfo::SCHEDULE_TYPE_NULL; 
	//	m_bEnableProcessShiftInfo=TRUE;
	//	m_bEnableProcessShiftTrains=FALSE;

	m_nRecvMsgId=0;
	m_nRecvMsgCnt=0;

	for(int i=0;i<MAX_REGISTERED_WND_NUM;i++)
	{
		m_aRegisterdWnd[i].nMsgID=0;
		m_aRegisterdWnd[i].hWnd=NULL;
	}

	basic_train_info_num=0;
	memset(basic_train_info, 0, sizeof(basic_train_info));

	m_pTyWaitDlg=NULL;
}

BOOL CTgDocument::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	return TRUE;
}

CTgDocument::~CTgDocument()
{
	UnRegisterDoc(this);
	UnRegisterDynDoc(this);
	DeinitData();
	delete m_pTyWaitDlg;
}

BEGIN_MESSAGE_MAP(CTgDocument, CDocument)
END_MESSAGE_MAP()


// CTgDocument 诊断

#ifdef _DEBUG
void CTgDocument::AssertValid() const
{
	CDocument::AssertValid();
}

void CTgDocument::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CTgDocument 序列化

void CTgDocument::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: 在此添加存储代码
	}
	else
	{
		// TODO: 在此添加加载代码
	}
}
// override
void CTgDocument::OnCloseDocument() 
{
	if(m_bAutoDelete)
		CDocument::OnCloseDocument();
}
BOOL CTgDocument::CanCloseFrame(CFrameWnd* pFrame) 
{
	// TODO: Add your specialized code here and/or call the base class

	//return CDocument::CanCloseFrame(pFrame);
	return TRUE;
}

// CTgDocument 命令

BOOL  CTgDocument::InitLogicData(int section_id,CTG_ScheduleInfo::SCHEDULE_TYPE type,BOOL  localmode,BOOL newserver) //type
{
	do
	{
		if(localmode)
			m_pCurrentWorkScheduleData=new CWorkScheduleHolder(CWorkScheduleHolder::LOCAL_MODE);
		else
			m_pCurrentWorkScheduleData=new CWorkScheduleHolder(CWorkScheduleHolder::SERVER_MODE);
		if(!m_pCurrentWorkScheduleData)
			break;
		if(newserver)
		{
			m_pWintgNewServerWork=new CWintgNewServer();
			ASSERT(m_pWintgNewServerWork!=NULL);

			m_pTrainDepartPredictData=NULL;
			if(gpcdef.IsEnablePredict())
			{
				m_pTrainDepartPredictData=new CTrainDepartPredictHolder(this, section_id);
				ASSERT(m_pTrainDepartPredictData!=NULL);
			}
			
			m_pWintgNewServerWork->Init(section_id,SERVER_TYPE_TG);
			m_pWintgNewServerWork->PollingAndCheckStatus(); //把默认状态置活
            
			m_pWintgNewServerShift=new CWintgNewServer();
			ASSERT(m_pWintgNewServerShift!=NULL);
			m_pWintgNewServerShift->Init(section_id,SERVER_TYPE_SHIFT_TG);
			m_pWintgNewServerShift->PollingAndCheckStatus(); //把默认状态置活

			m_pWintgNewServerBasic=new CWintgNewServer();
			ASSERT(m_pWintgNewServerBasic!=NULL);
			m_pWintgNewServerBasic->Init(section_id,SERVER_TYPE_BASIC_TG);
			m_pWintgNewServerBasic->PollingAndCheckStatus(); //把默认状态置活

			m_pWintgNewServerBasicPrev=new CWintgNewServer();
			ASSERT(m_pWintgNewServerBasicPrev!=NULL);
			m_pWintgNewServerBasicPrev->Init(section_id,SERVER_TYPE_BASIC_TG);
			m_pWintgNewServerBasicPrev->PollingAndCheckStatus(); // 用的时候再激活？

			m_pWintgNewServerBasicBackup=new CWintgNewServer();
			ASSERT(m_pWintgNewServerBasicBackup!=NULL);
			m_pWintgNewServerBasicBackup->Init(section_id,SERVER_TYPE_BASIC_TG);
			m_pWintgNewServerBasicBackup->PollingAndCheckStatus(); // 用的时候再激活？

		}

		// 20110420
		if(m_pWintgNewServerWork)
		{
			m_pCurrentWorkScheduleData->RegisterCommInterface(CTG_ScheduleInfo::SCHEDULE_TYPE_WORK,m_pWintgNewServerWork); 
			m_pCurrentWorkScheduleData->SetServerCommWork(m_pWintgNewServerWork); 
		}
		if(m_pWintgNewServerShift)
		{
			m_pCurrentWorkScheduleData->RegisterCommInterface(CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT,m_pWintgNewServerShift); 
			m_pCurrentWorkScheduleData->SetServerCommShift(m_pWintgNewServerShift); 
		}
		if(m_pWintgNewServerBasic)
		{
			m_pCurrentWorkScheduleData->RegisterCommInterface(CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC,m_pWintgNewServerBasic); 
			m_pCurrentWorkScheduleData->SetServerCommBasic(m_pWintgNewServerBasic); 
		}
		if(m_pWintgNewServerBasicPrev)
		{
			m_pCurrentWorkScheduleData->RegisterCommInterface(CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_PREV,m_pWintgNewServerBasicPrev); 
			m_pCurrentWorkScheduleData->SetServerCommBasicPrev(m_pWintgNewServerBasicPrev); 
			m_pCurrentWorkScheduleData->InitCommInterfaceStatus(m_pWintgNewServerBasicPrev,m_pWintgNewServerBasicPrev->CommReady()); // 防止重复发送SCHEDULE_LOAD,因为在CreateTgDocument中直接调用了LoadSchedule
		}
		if(m_pWintgNewServerBasicBackup)
		{
			m_pCurrentWorkScheduleData->RegisterCommInterface(CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_BACKUP,m_pWintgNewServerBasicBackup); 
			m_pCurrentWorkScheduleData->SetServerCommBasicBackup(m_pWintgNewServerBasicBackup); 
			m_pCurrentWorkScheduleData->InitCommInterfaceStatus(m_pWintgNewServerBasicBackup,m_pWintgNewServerBasicBackup->CommReady()); // 防止重复发送SCHEDULE_LOAD,因为在CreateTgDocument中直接调用了LoadSchedule
		}

		ASSERT(type==CTG_ScheduleInfo::SCHEDULE_TYPE_WORK || type==CTG_ScheduleInfo::SCHEDULE_TYPE_HIST || type==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_PREV || type==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_BACKUP);

		/////2012.04.13 nc局增加防止失效GPC导致trainindex重复  hjz
		int tdid = gpcdef.GetTdIdByUserID(gpcdef.GetUserGpcId());
		if(tdid>0)
		{
			m_pCurrentWorkScheduleData->SetUserID(tdid+1);
			int tmp=tdid%64;
			sysprint(5,0,"[USER]=gpc%d 列车索引生成号 :%d ",   gpcdef.GetUserGpcId(), (tmp==0)?64:tmp);
		}
		else
		{
			m_pCurrentWorkScheduleData->SetUserID(gpcdef.GetUserGpcId()+1);
			int tmp=gpcdef.GetUserGpcId()%64;
			sysprint(5,0,"[USER]=gpc%d 列车索引生成号 :%d ",   gpcdef.GetUserGpcId(), (tmp==0)?64:tmp);
		}

		if(gpcdef.IsNewAutoMoveMode())
			m_pCurrentWorkScheduleData->InitScheduleData_NEW(section_id, type);
		else
			m_pCurrentWorkScheduleData->InitScheduleData(section_id,type);

		m_nCurrentSectionNo=section_id;
		m_nStationCount=sizeof(m_aStationList)/sizeof(m_aStationList[0]);
		m_nStationCount=gpcdef.GetStationList(section_id,m_nStationCount,m_aStationList);
		////////// 有关计划列表
		InitPlanInfo(section_id);
		/////////////////////////
		m_pCurrentWorkScheduleData->SetFileSaveFlag(GetTGFunctionConfig()->GetFileSaveFlag());


		return TRUE;
		// break;
	}while(0);
	return TRUE;
}


BOOL  CTgDocument::InitLogicDataDync(int section_id,CTG_ScheduleInfo::SCHEDULE_TYPE type,BOOL  localmode,BOOL newserver) //type
{
	do
	{
		if(localmode)
			m_pCurrentWorkScheduleData=new CWorkScheduleHolder(CWorkScheduleHolder::LOCAL_MODE);
		else
			m_pCurrentWorkScheduleData=new CWorkScheduleHolder(CWorkScheduleHolder::SERVER_MODE);
		if(!m_pCurrentWorkScheduleData)
			break;
		if(newserver)
		{
		    // 20110420
			if(type==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC)
			{
				m_pWintgNewServerBasic=new CWintgNewServer();
				ASSERT(m_pWintgNewServerBasic!=NULL);
				m_pWintgNewServerBasic->Init(section_id,SERVER_TYPE_BASIC_TG);
				m_pWintgNewServerBasic->PollingAndCheckStatus(); //把默认状态置活
				m_pCurrentWorkScheduleData->RegisterCommInterface(type,m_pWintgNewServerBasic); 
				m_pCurrentWorkScheduleData->SetServerCommBasic(m_pWintgNewServerBasic); 
			}
			else if(type==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_PREV)
			{
				m_pWintgNewServerBasicPrev=new CWintgNewServer();
				ASSERT(m_pWintgNewServerBasicPrev!=NULL);
				m_pWintgNewServerBasicPrev->Init(section_id,SERVER_TYPE_BASIC_TG);
				m_pWintgNewServerBasicPrev->PollingAndCheckStatus(); //把默认状态置活
				m_pCurrentWorkScheduleData->RegisterCommInterface(type,m_pWintgNewServerBasicPrev); 
				m_pCurrentWorkScheduleData->SetServerCommBasicPrev(m_pWintgNewServerBasicPrev); 
				m_pCurrentWorkScheduleData->InitCommInterfaceStatus(m_pWintgNewServerBasicPrev,m_pWintgNewServerBasicPrev->CommReady()); // 防止重复发送SCHEDULE_LOAD,因为在CreateTgDocument中直接调用了LoadSchedule
			}
			else if(type==CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT) 
			{
				m_pWintgNewServerShift=new CWintgNewServer();
				ASSERT(m_pWintgNewServerShift!=NULL);
				m_pWintgNewServerShift->Init(section_id,SERVER_TYPE_SHIFT_TG);
				m_pWintgNewServerShift->PollingAndCheckStatus(); //把默认状态置活
				m_pCurrentWorkScheduleData->RegisterCommInterface(CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT,m_pWintgNewServerShift); 
				m_pCurrentWorkScheduleData->SetServerCommShift(m_pWintgNewServerShift); 
			}
			else if(type==CTG_ScheduleInfo::SCHEDULE_TYPE_WORK) 
			{
				m_pWintgNewServerWork=new CWintgNewServer();
				ASSERT(m_pWintgNewServerWork!=NULL);

				m_pTrainDepartPredictData=NULL;

				m_pWintgNewServerWork->Init(section_id,SERVER_TYPE_TG);
				m_pWintgNewServerWork->PollingAndCheckStatus(); //把默认状态置活
				m_pCurrentWorkScheduleData->RegisterCommInterface(CTG_ScheduleInfo::SCHEDULE_TYPE_WORK,m_pWintgNewServerWork); 
				m_pCurrentWorkScheduleData->SetServerCommWork(m_pWintgNewServerWork); 
			}
			else if(type==CTG_ScheduleInfo::SCHEDULE_TYPE_HIST) 
			{
				m_pWintgNewServerWork=new CWintgNewServer();
				ASSERT(m_pWintgNewServerWork!=NULL);
				m_pWintgNewServerWork->Init(section_id,SERVER_TYPE_HISTDATA);
				m_pWintgNewServerWork->PollingAndCheckStatus(); //把默认状态置活
				m_pCurrentWorkScheduleData->RegisterCommInterface(CTG_ScheduleInfo::SCHEDULE_TYPE_HIST,m_pWintgNewServerWork); 
				m_pCurrentWorkScheduleData->SetServerCommWork(m_pWintgNewServerWork); 
			}
			else if(type==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_BACKUP)
			{
				m_pWintgNewServerBasicBackup=new CWintgNewServer();
				ASSERT(m_pWintgNewServerBasicBackup!=NULL);
				m_pWintgNewServerBasicBackup->Init(section_id,SERVER_TYPE_BASIC_TG);
				m_pWintgNewServerBasicBackup->PollingAndCheckStatus(); //把默认状态置活
				m_pCurrentWorkScheduleData->RegisterCommInterface(type,m_pWintgNewServerBasicBackup); 
				m_pCurrentWorkScheduleData->SetServerCommBasicBackup(m_pWintgNewServerBasicBackup); 
				m_pCurrentWorkScheduleData->InitCommInterfaceStatus(m_pWintgNewServerBasicBackup,m_pWintgNewServerBasicBackup->CommReady()); // 防止重复发送SCHEDULE_LOAD,因为在CreateTgDocument中直接调用了LoadSchedule
			}
		}

		if(gpcdef.IsNewAutoMoveMode())
			m_pCurrentWorkScheduleData->InitScheduleData_NEW(section_id, type);
		else
			m_pCurrentWorkScheduleData->InitScheduleData(section_id,type);

		m_nCurrentSectionNo=section_id;
		m_nStationCount=sizeof(m_aStationList)/sizeof(m_aStationList[0]);
		m_nStationCount=gpcdef.GetStationList(section_id,m_nStationCount,m_aStationList);
		return TRUE;
		// break;
	}while(0);
	return TRUE;
}

BOOL  CTgDocument::DeinitData()
{
	if(m_pWintgNewServerWork)
	{
		m_pWintgNewServerWork->Close(); 
		m_pCurrentWorkScheduleData->UnRegisterCommInterface(m_pWintgNewServerWork); 
		delete m_pWintgNewServerWork;
	}
	if(m_pWintgNewServerShift)
	{
		m_pWintgNewServerShift->Close(); 
		m_pCurrentWorkScheduleData->UnRegisterCommInterface(m_pWintgNewServerShift); 
		delete m_pWintgNewServerShift;
	}
	if(m_pWintgNewServerBasic)
	{
		m_pWintgNewServerBasic->Close(); 
		m_pCurrentWorkScheduleData->UnRegisterCommInterface(m_pWintgNewServerBasic); 
		delete m_pWintgNewServerBasic;
	}
	if(m_pWintgNewServerBasicPrev)
	{
		m_pWintgNewServerBasicPrev->Close(); 
		m_pCurrentWorkScheduleData->UnRegisterCommInterface(m_pWintgNewServerBasicPrev); 
		delete m_pWintgNewServerBasicPrev;
	}
	if(m_pWintgNewServerBasicBackup)
	{
		m_pWintgNewServerBasicBackup->Close(); 
		m_pCurrentWorkScheduleData->UnRegisterCommInterface(m_pWintgNewServerBasicBackup); 
		delete m_pWintgNewServerBasicBackup;
	}

	if(m_pStplan)
		delete m_pStplan;
	if(m_pCurrentWorkScheduleData)
		delete m_pCurrentWorkScheduleData;
	if(m_pTrainDepartPredictData)
	{
		m_pTrainDepartPredictData->UnRegisterMMI();
		delete m_pTrainDepartPredictData;
	}
	m_pTrainDepartPredictData=NULL;

	m_pStplan=NULL;
	m_pWintgNewServerWork=NULL;
	m_pWintgNewServerShift=NULL;
	m_pWintgNewServerBasic=NULL;
	m_pWintgNewServerBasicPrev=NULL;
	m_pWintgNewServerBasicBackup=NULL;
	m_pCurrentWorkScheduleData=NULL;
	return TRUE;
}

BOOL  CTgDocument::PollingData(void)
{
	return TRUE;
}
BOOL  CTgDocument::LoadSchedule(int shift_id)
{
	return ShiftSchedule(shift_id); // 程序启动加载
}

BOOL  CTgDocument::GetScheduleInfo(CTG_ScheduleInfo &info)
{
	if(m_pCurrentWorkScheduleData)
	{
		m_pCurrentWorkScheduleData->GetWorkScheduleInfo(info);
		return TRUE;
	}
	return FALSE;
}
CTG_ScheduleInfo::SCHEDULE_TYPE  CTgDocument::GetScheduleType()
{
	CTG_ScheduleInfo info;
	m_pCurrentWorkScheduleData->GetWorkScheduleInfo(info);
	return info.m_nType;
}

long  CTgDocument::GetCurrentShiftID()
{
	CTG_ScheduleInfo info;
	m_pCurrentWorkScheduleData->GetWorkScheduleInfo(info);
	return info.m_nShiftID;
}

int   CTgDocument::GetCurrentSectionNo()
{
	CTG_ScheduleInfo info;
	m_pCurrentWorkScheduleData->GetWorkScheduleInfo(info);
	return info.m_nSectionID;
}

BOOL  CTgDocument::SetLocalDatabaseMode(BOOL flag)
{
	if(flag)
		return m_pCurrentWorkScheduleData->SwitchToLocalMode();
	else
		return m_pCurrentWorkScheduleData->SwitchToServerMode();

}

/////////////////////// 以下从WINTGDOC 移植
BOOL CTgDocument::InitPlanInfo(int section_id)
{
	gpcdef.GetNeighberSectionList(section_id,m_nCurrNeighberSectionCount,m_nCurrNeighberSectionList);
	gpcdef.GetNeighberGpcList(section_id,m_nCurrNeighberGpcCount,m_nCurrNeighberGpcList);
	gpcdef.GetNeighberSectionPropertyList(section_id,m_nCurrNeighberSectionCount,m_nCurrNeighberSectionPropertyList);

	//for view plan neighber use
	gpcdef.GetNeighberViewPlanSectionList(section_id,m_nCurrNeighberViewPlanSectionCount,m_nCurrNeighberViewPlanSectionList);
	gpcdef.GetNeighberViewPlanGpcList(section_id,m_nCurrNeighberViewPlanGpcCount,m_nCurrNeighberViewPlanGpcList);

	InitStationPlanInfo(section_id);

	m_pStplan=new cStPlan(this,GetTGBasicParam());
	return TRUE;
}

void CTgDocument::ClearPhasePlanInfo()
{
	InitStationPlanInfo(GetCurrentSectionNo());
}

int CTgDocument::InitStationPlanInfo(int section_id)
{
	m_StationPlanInfo_ImportantNew.init();
	GPCENTITY entityInfo;
	GPCPLANRECEIVER rec; CString Entityname;

	int nEntityList[256];
	int nEntityNum = sizeof(nEntityList)/sizeof(nEntityList[0]);
	nEntityNum=gpcdef.GetPhaseNoteReceiverList(section_id,nEntityList,nEntityNum);


	////下发记事的内容
	//该区段的实体总数目

	for(int m=0;m<STAGE_PER_WORK;m++)
	{
		m_StationPlanInfo_ImportantNew.start_st_weather[m] = 0;
		m_StationPlanInfo_ImportantNew.end_st_weather[m] = 0;
	}	    	
	m =0;

	CString strText;
	for(int j= 0;j<nEntityNum;j++)
	{
		gpcdef.GetEntityInfo(nEntityList[j],entityInfo);
		strText.Format("%s",rec.sentity);

		if(entityInfo.nGroupId ==  EQUIP_LIRC)
			continue;
		strText.Format("%s",gpcdef.GetStationName(entityInfo.nSpecial1));//rec.sentity);		 		 
		//站号
		m_StationPlanInfo_ImportantNew.m_StationPlan[m].nStation = rec.nstationNo;			
		strcpy(m_StationPlanInfo_ImportantNew.m_StationPlan[m].EntityName, strText);//nStationList[k];

		m_StationPlanInfo_ImportantNew.m_StationPlan[m].Entityid = entityInfo.nEntityId;
		m_StationPlanInfo_ImportantNew.m_StationPlan[m].nEntityIndex = nEntityList[j];   // BUG!!! k

		for(int l=0;l<STAGE_PER_WORK;l++)
		{
			m_StationPlanInfo_ImportantNew.m_StationPlan[m].m_StageInfo[l].usSendDate = 0;
			m_StationPlanInfo_ImportantNew.m_StationPlan[m].m_StageInfo[l].usSendTime= 0;

			m_StationPlanInfo_ImportantNew.m_StationPlan[m].m_StageInfo[l].edit_time = 0;
			m_StationPlanInfo_ImportantNew.m_StationPlan[m].m_StageInfo[l].send_time= 0;
			m_StationPlanInfo_ImportantNew.m_StationPlan[m].m_StageInfo[l].recv_time= 0;


			m_StationPlanInfo_ImportantNew.m_StationPlan[m].m_StageInfo[l].bReceiveFlag =  CLBTH::MsgWordPhasePlanStatus::WORDPHASE_INIT;
			m_StationPlanInfo_ImportantNew.m_StationPlan[m].m_StageInfo[l].bSignInFlag =  CLBTH::MsgWordPhasePlanStatus::WORDPHASE_INIT;
			m_StationPlanInfo_ImportantNew.m_StationPlan[m].m_StageInfo[l].strSignName[0] = 0x00;
		}
		m++;	
	}		
	m_StationPlanInfo_ImportantNew.nEntityNum = m;
	
	return TRUE;
}

TIME  CTgDocument::GetCurrentTGDLongTime()
{
	CTime t = CTime::GetCurrentTime( );
	return t.GetTime(); 
}

int CTgDocument::GetCurrentTGTime()
{
	CTime t = CTime::GetCurrentTime( );
	int m_usCurrentTime = (t.GetHour( ) * 60 + t.GetMinute( ))*60 + t.GetSecond( );
	return m_usCurrentTime;
}
int CTgDocument::GetCurrentTGDate()
{
	CTime t = CTime::GetCurrentTime( );
	int m_usCurrentDate = (t.GetYear( ) - 1990) * 380 + (t.GetMonth( ) - 1)* 31 + t.GetDay( ) - 1;
	return m_usCurrentDate;
}

BOOL  CTgDocument::ProcessGenMessage(long eid,BaseMsg *pmsg)
{
	if(pmsg == NULL) return FALSE;

	if(IsQueryMode())
		return FALSE;
	if(pmsg->isKindOf(MsgNoticeNeigbourReceiveSchd::getTypeIDStatic()))
		return ProcessMessage((const MsgNoticeNeigbourReceiveSchd *)pmsg);
	
	if(pmsg->isKindOf(MsgStringMessage::getTypeIDStatic()))
		return ProcessMessage((MsgStringMessage *)pmsg);
	
	if(pmsg->isKindOf(MsgTrainDepartPredict::getTypeIDStatic()))
	{
		if(m_pTrainDepartPredictData)
			return m_pTrainDepartPredictData->ProcessPredictByNeibMsg((MsgTrainDepartPredict*)pmsg);
	}
	
	return FALSE;
}

void CTgDocument::GenDCmd(CString plan_id, long block_index, CString dcmdcontent, int con_id, long type)
{
	if(plan_id == "")
		plan_id.Format("%d", block_index);

	CString xml, tmp;
	xml = "";
	tmp.Format("<BLOCK abs_or_station=\"%d\" tmis_index=\"%s\">", type, plan_id);
	xml += tmp;
	xml += dcmdcontent;
	xml += " </BLOCK>";

	long nEntityOfDCMD=GetSpecialEntityID(GPCTERM_TYPE_DCMD,con_id);

	CString token,content;
	GenXMLMessage_InformDCMDToFront(token,content);
	TGSendStringMessage(token,content,nEntityOfDCMD);
	sysprint(5,0,"向调度命令终端 %d 发送最前端XML\n %s", nEntityOfDCMD, xml);

	token="GEN_BLOCK_DCMD";
	TGSendStringMessage(token, xml, nEntityOfDCMD);

	sysprint(5,0,"向调度命令终端 %d 发送生成命令XML\n %s", nEntityOfDCMD, xml);
}

BOOL  CTgDocument::ProcessStationStatusInform( int station)
{
	if(!IsMyStation(station))
		return FALSE;
	StationStatusInform inform;
	inform.station_no=station;
	UpdateAllViews(NULL,TGDATA_STATION_STATUS_CHANGE,&inform);

	return TRUE;
}

BOOL CTgDocument::ShiftSchedule(ULONG newShiftID, bool bIgnore)
{
	CTG_ScheduleInfo prev_schd;
	m_pCurrentWorkScheduleData->GetWorkScheduleInfo(prev_schd);
	CTG_ScheduleInfo newschd(prev_schd.m_nType,prev_schd.m_nSectionID, newShiftID);

	if(gpcdef.IsNewAutoMoveMode())
	{
		newschd.ShiftSchedule(bIgnore);
		m_pCurrentWorkScheduleData->ShiftWorkSchedule_NEW(newschd);
	}
	else
	{
		m_pCurrentWorkScheduleData->ShiftWorkSchedule(newschd);
	}
	m_pCurrentWorkScheduleData->ReloadData();

	if (GetTGFunctionConfig()->IsStplanTrainIndexSaveInList())
		m_pCurrentWorkScheduleData->GetWorkScheduleData()->RemoveTrainInSendPlanList();

	TIME st,et;
	newschd.GetShiftManagerRange(st,et);
	CTime cst=st;
	CTime cet=et;
	CTime cmplanst=CTime(cst.GetYear(),cst.GetMonth(),cst.GetDay(),0,0,0);   
	CTime cmplanet=CTime(cst.GetYear(),cst.GetMonth(),cst.GetDay(),23,59,59);
	m_tMMplanStartTime=cmplanst.GetTime(); 
	m_tMMplanEndTime=cmplanet.GetTime(); 
	UpdateAllViews(NULL,TGDATA_SHIFT_SCHEDULE,NULL);
	LoadMMListFromWS();
	LoadTYListFromFile();
	m_listPlanInform.clear();
	LoadPhasePlanFromFile();
	return TRUE;
}

BOOL CTgDocument::AutoShiftSchedule()
{
	CTG_ScheduleInfo prev_schd;
	m_pCurrentWorkScheduleData->GetWorkScheduleInfo(prev_schd);

	if(prev_schd.m_nType!=CTG_ScheduleInfo::SCHEDULE_TYPE_WORK)
		return FALSE;

	int newShiftID;
	newShiftID=CTG_ScheduleInfo::Get_CurrentShiftid(); 
	if(prev_schd.m_nShiftID ==newShiftID)
		return FALSE;

	GpcLogTrainMessage(0, 0, "AutoShiftSchedule: 自动交接班 ShiftID %d", newShiftID);
	return ShiftSchedule(newShiftID); // 自动交接班
}

void CTgDocument::OnReciveOPMSCrewNotice(const char* token, const char* content)
{
	GpcLogTrainMessage(0,0,"收到TD发来的 乘务信息变化通知 node:%s xml:%s",  token, content);
	if(m_pCrewNoticeDlg==NULL)
	{
		m_pCrewNoticeDlg=new CDlgCrewNotice(this);
		if(m_pCrewNoticeDlg)
		{
			if(!m_pCrewNoticeDlg->Create(IDD_CREW_PLAN_NOTICE_DLG))
			{
				delete m_pCrewNoticeDlg;
				m_pCrewNoticeDlg=NULL;
			}
		}
	}

	if(m_pCrewNoticeDlg)
	{
		m_pCrewNoticeDlg->SetXML(token, content);
		m_pCrewNoticeDlg->ShowWindow(SW_SHOW);
	}
}

//#define TOKEN_1_ACK_BZ             "ACK_BZ"



// 未处理 2
// #define TOKEN_11_ACK_OTHER_MSG "ACK_OTHER_MSG"
//#define TOKEN_7_ACK_TFG  "ACK_TFG"

BOOL CTgDocument::ProcessMessage(MsgStringMessage *pmsg)
{
	const char *token;
	const char *content;
	token=pmsg->GetToken();
	content=pmsg->GetContent();
	if(!token || !content)
		return FALSE;
	if(!stricmp(token,TOKEN_1_ACK_BZ))
	{
	}
	else if(!stricmp(token,TOKEN_21_CREW_PLAN_INFORM))
	{
		OnReciveOPMSCrewNotice(token,  content);
	}
	else if(!stricmp(token,TOKEN_3_ACK_YL_BY_TIME) ||
		!stricmp(token,TOKEN_15_ACK_KEDIAO_INFO) ||
		!stricmp(token,TOKEN_16_ACK_YINGYE_INFO))
	{
		// 在TgEditView中处理
	}
	else if(!stricmp(token,TOKEN_2_ACK_YL_BY_ID) ||
		!stricmp(token,TOKEN_4_ACK_YL_BY_TRAINNUM) ||
		!stricmp(token,TOKEN_5_ACK_YL_BY_TRAINNUM_TIME) ||
		!stricmp(token,TOKEN_8_ACK_NEWYL) ||
		!stricmp(token,TOKEN_15_ACK_KEDIAO_INFO) ||
		!stricmp(token,TOKEN_16_ACK_YINGYE_INFO) ||
		!stricmp(token,TOKEN_9_ACK_PLAN) ||
		!stricmp(token,TOKEN_10_ACK_ADPLAN))

	{
		// 在DlgTrainList.CPP 中处理
	}
	else if(!stricmp(token,TOKEN_13_ACK_WKINFO) ||
		!stricmp(token,TOKEN_14_ACK_RSINFO) || 
		!stricmp(token,TOKEN_17_ACK_SG_WX_INFO) || 
		!stricmp(token,TOKEN_18_ACK_WKDES) ||
		!stricmp(token,TOKEN_12_ACK_CMD) ||
		!stricmp(token,"TD_RESULT"))
		return ProcessXML_MMPlanMsgFromTD(content,token);
	else if(!stricmp(token,TOKEN_SS_PLAN_INFORM) ||
		!stricmp(token,TOKEN_KD_PLAN_INFORM) || 
		!stricmp(token,TOKEN_JH_PLAN_INFORM) ||
		!stricmp(token,TOKEN_22_ACK_STN_PLAN))
		return ProcessXML_PlanInform(content,token);
	else if(!stricmp(token,TOKEN_24_ACK_FTN_PLAN)) 
		return ProcessXML_FTNMsg(content,token);
	else if(!stricmp(token,TOKEN_23_ACK_SPEC_PLAN))
		return ProcessXML_TYPlanMsg(content,token);
	else if(!stricmp(token,TOKEN_26_ACK_SPEC_UPDATE_PLAN)) // 特运更新
		return ProcessXML_Msg_TEYUN_Update(content,token);
	else if(!stricmp(token,TOKEN_25_AD_PLAN_INFORM)) // 客调调整计划
		return ProcessXML_KDPlanMsg(content,token);

	return FALSE;
}

CString  CTgDocument::IsWaitWindowExist()
{
	CString str="";
	if(m_pWaitWnd)
	{
		if(::IsWindow(m_pWaitWnd->m_hWnd))
		{
			str=((CDlgWait *)m_pWaitWnd)->GetDisplayStr();
		}
	}

	return str;
}

void CTgDocument::OpenWaitWindow(const char *cap, const char *text)
{
	if(cap)
	{
		if(m_pWaitWnd)
		{
			((CDlgWait *)m_pWaitWnd)->UpdateDisplay(cap, text); 
		}
		else
			m_pWaitWnd=new CDlgWait(cap);
	}
	else
	{
		if(m_pWaitWnd)
		{
			delete m_pWaitWnd;
			m_pWaitWnd=NULL;
		}
	}
}

BOOL CTgDocument::IsMyStation(int station) const
{
	for(int i=0;i<m_nStationCount;i++)
	{
		if(m_aStationList[i]==station)
			return TRUE;
	}
	return FALSE;
}

void CTgDocument::SetQueryMode(BOOL va)
{
	m_bQueryMode=va;
	if(m_pCurrentWorkScheduleData)
		m_pCurrentWorkScheduleData->SetQueryMode(va); 
	if(m_pWintgNewServerWork)
		m_pWintgNewServerWork->SetQueryMode(va); 
	if(m_pWintgNewServerShift)
		m_pWintgNewServerShift->SetQueryMode(va); 
	if(m_pWintgNewServerBasic)
		m_pWintgNewServerBasic->SetQueryMode(va); 
	if(m_pWintgNewServerBasicPrev)
		m_pWintgNewServerBasicPrev->SetQueryMode(va); 
	if(m_pWintgNewServerBasicBackup)
		m_pWintgNewServerBasicBackup->SetQueryMode(va); 
}


CTG_TrainScheduleBase *CTgDocument::GetScheduleData(CTG_ScheduleInfo::SCHEDULE_TYPE type)
{
	if(type==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC) 
		return m_pCurrentWorkScheduleData->GetBasicScheduleData();
	else if(type==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_PREV) 
		return m_pCurrentWorkScheduleData->GetBasicPrevScheduleData();
	else if(type==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_BACKUP) 
		return m_pCurrentWorkScheduleData->GetBasicBackupScheduleData();
	else if(type==CTG_ScheduleInfo::SCHEDULE_TYPE_SHIFT) 
		return m_pCurrentWorkScheduleData->GetShiftScheduleData();
	else
		return m_pCurrentWorkScheduleData->GetWorkScheduleData();
}
BOOL  CTgDocument::GetShiftScheduleInfo(CTG_ScheduleInfo &info)
{
	if(m_pCurrentWorkScheduleData)
	{
		m_pCurrentWorkScheduleData->GetShiftScheduleInfo(info);
		return TRUE;
	}
	return FALSE;
}

BOOL  CTgDocument::GetBasicScheduleInfo(CTG_ScheduleInfo &info)
{
	if(m_pCurrentWorkScheduleData)
	{
		m_pCurrentWorkScheduleData->GetBasicScheduleInfo(info);
		return TRUE;
	}
	return FALSE;
}
BOOL  CTgDocument::GetBasicPrevScheduleInfo(CTG_ScheduleInfo &info)
{
	if(m_pCurrentWorkScheduleData)
	{
		m_pCurrentWorkScheduleData->GetBasicPrevScheduleInfo(info);
		return TRUE;
	}
	return FALSE;
}

//以原系统input兼容的方式保存



//把基本图信息存为bsinfo文件
/*
[BASIC_INFO]
CON_ID= 1, TRAIN_INDEX=   0, TRAIN_ID_COUNT=1, ADJUST_FLAG=0, TRAIN_TYPE=  0, RUN_TYPE=1, DIRECTION=1, TRAIN_PRIORITY=1, TRAIN_ID_1=      Z3, ORIG_TRAIN_ID_1=        , BEGIN_STA_1=  0, END_STA_1=  0, TRAIN_ID_2=        , ORIG_TRAIN_ID_2=        , BEGIN_STA_2=  0, END_STA_2=  0, TRAIN_ID_3=        , ORIG_TRAIN_ID_3=        , BEGIN_STA_3=  0, END_STA_3=  0, TRAIN_ID_4=        , ORIG_TRAIN_ID_4=        , BEGIN_STA_4=  0, END_STA_4=  8
[BASIC_INFO_END]
*/

// 临时车次存储
#define MAX_IDS_NUM_PER_TRAIN  4
struct{
	char  train_id[24];
	short start_index;
	short end_index;
}ol_trainids[MAX_IDS_NUM_PER_TRAIN];
int ol_trainids_count=0;;

static void CacuOlTrainIDs(const CTG_SingleTrain* single)
{
	ol_trainids_count=0;
	for(int i=0;i<single->GetTrainRecordCount();i++)
	{
		const CTG_TrainRecord *prec=single->GetTrainRecord_Const(i);
		if(prec)
		{
			if(ol_trainids_count==0)
			{
				strcpy(ol_trainids[ol_trainids_count].train_id,prec->GetArriveTID());
				ol_trainids[ol_trainids_count].start_index=i;
				ol_trainids[ol_trainids_count].end_index=i;
				ol_trainids_count++;

				if (stricmp(prec->GetArriveTID(),prec->GetDepartTID())) // by wuxiang
				{
					strcpy(ol_trainids[ol_trainids_count].train_id,prec->GetDepartTID());
					ol_trainids[ol_trainids_count].start_index=i;
					ol_trainids[ol_trainids_count].end_index=i;
					ol_trainids_count++;
				}
			}
			else if(!stricmp(ol_trainids[ol_trainids_count-1].train_id,prec->GetDepartTID()))
			{
				ol_trainids[ol_trainids_count-1].end_index=i;
			}
			else
			{
				if(ol_trainids_count>=MAX_IDS_NUM_PER_TRAIN)
					return;
				strcpy(ol_trainids[ol_trainids_count].train_id,prec->GetDepartTID());
				ol_trainids[ol_trainids_count].start_index=i;
				ol_trainids[ol_trainids_count].end_index=i;
				ol_trainids_count++;
			}
		}
	}
}

BOOL CTgDocument::olSaveTrainInfo(CTG_ScheduleInfo::SCHEDULE_TYPE tp,const char * infoName)
{
	CStdioFile infoFile;
	if(!infoFile.Open(infoName,CFile::modeCreate|CFile::modeWrite))
	{
		AfxMessageBox("创建文件失败!");
		return FALSE;
	}
	CTG_TrainScheduleBase *pData=GetScheduleData(tp);
	if(!pData)
	{
		AfxMessageBox("内部错误：非法运行图类型");
		return FALSE;
	}
	CString txt_eachline;
	CString eachcol;
	CTG_SingleTrain* single = NULL;
	TG_TrainInfo * traininfo = NULL;
	infoFile.WriteString("[BASIC_INFO]\r\n");
	CTG_TrainPtrGroup  group;
	pData->FindTrains(group);
	for(int i=0; i<group.GetTrainCount(); i++)
	{
		single = group.GetTrainPtr(i);
		traininfo =&single->GetTrainInfo()->m_data;

		CacuOlTrainIDs(single);

		txt_eachline.Empty();

		//CON_ID= 1, 
		eachcol.Format("CON_ID=%2d, ",GetCurrentSectionNo());
		txt_eachline += eachcol;

		//TRAIN_INDEX=   0, 
		eachcol.Format("TRAIN_INDEX=%4ld, ",single->GetTrainIndex());
		txt_eachline += eachcol;

		//TRAIN_ID_COUNT=1, 
		eachcol.Format("TRAIN_ID_COUNT=%d, ",ol_trainids_count);
		txt_eachline += eachcol;
		//ADJUST_FLAG=0, 
		eachcol.Format("ADJUST_FLAG=%d, ",traininfo->adjust_flag);
		txt_eachline += eachcol;
		//TRAIN_TYPE=  0, 
		eachcol.Format("TRAIN_TYPE=%3d, ",traininfo->type);
		txt_eachline += eachcol;
		//RUN_TYPE=1, 
		eachcol.Format("RUN_TYPE=%d, ",traininfo->run_type);
		txt_eachline += eachcol;
		//DIRECTION=1, 
		eachcol.Format("DIRECTION=%d, ",single->IsDownDirection()?1:0);
		txt_eachline += eachcol;
		//TRAIN_PRIORITY=1, 
		eachcol.Format("TRAIN_PRIORITY=%d, ",traininfo->priority);
		txt_eachline += eachcol;



		for(int j=0; j<MAX_IDS_NUM_PER_TRAIN;j++)
		{
			if(j>=ol_trainids_count)
			{
				//TRAIN_ID_1=      Z3, 
				eachcol.Format("TRAIN_ID_%d=        , ",j+1);
				txt_eachline += eachcol;
				//ORIG_TRAIN_ID_1=        , 
				eachcol.Format("ORIG_TRAIN_ID_%d=        , ",j+1);
				txt_eachline += eachcol;
				//BEGIN_STA_1=  0, 
				eachcol.Format("BEGIN_STA_%d=   , ",j+1);
				txt_eachline += eachcol;
				//END_STA_1=  0, 
				eachcol.Format("END_STA_%d=   , ",j+1);					
				txt_eachline += eachcol;			

			}
			else
			{
				//TRAIN_ID_1=      Z3, 
				eachcol.Format("TRAIN_ID_%d=%8s, ",j+1,ol_trainids[j].train_id);
				txt_eachline += eachcol;
				//ORIG_TRAIN_ID_1=        , 
				eachcol.Format("ORIG_TRAIN_ID_%d=%8s, ",j+1,"");//ol_trainids[j].train_id);
				txt_eachline += eachcol;
				//BEGIN_STA_1=  0, 
				eachcol.Format("BEGIN_STA_%d=%3d, ",j+1,ol_trainids[j].start_index);
				txt_eachline += eachcol;
				//END_STA_1=  0, 
				eachcol.Format("END_STA_%d=%3d, ",j+1,ol_trainids[j].end_index);
				txt_eachline += eachcol;
			}

		}
		txt_eachline +="\r\n";
		infoFile.WriteString(txt_eachline);
	}
	infoFile.WriteString("[BASIC_INFO_END]\r\n");
	infoFile.Close();
	return TRUE;
}

// 把基本图存为可视文本
BOOL CTgDocument::olSaveTrainToFile(CTG_ScheduleInfo::SCHEDULE_TYPE tp,const char * fileName)
{
	CStdioFile infoFile;
	bool isSucess=true;
	if(!infoFile.Open(fileName,CFile::modeCreate|CFile::modeWrite))
	{
		AfxMessageBox("创建可视文本文件失败!");
		return FALSE;
	}
	CTG_TrainScheduleBase *pData=GetScheduleData(tp);
	if(!pData)
	{
		AfxMessageBox("内部错误：非法运行图类型");
		return FALSE;
	}
	CString txt_eachline;
	CString eachcol;
	CTG_SingleTrain* single = NULL;
	TG_TrainRecord * rec = NULL;
	TG_TrainRecord * pre_rec = NULL;

	int recNum = 0;

	CTG_TrainPtrGroup  group;
	pData->FindTrains(group);
	CString tmp;
	tmp = "<?xml version=\"1.0\" encoding=\"GB2312\" ?>";
	infoFile.WriteString(tmp);
	infoFile.WriteString("\n<TRAINS>");
	for(int i=0; i<group.GetTrainCount(); i++)
	{
		single = group.GetTrainPtr(i);
		tmp.Format("\n<TRAIN name=\'%s\'>",  single->GetTrainID());
		infoFile.WriteString(tmp);
		recNum = single->GetTrainRecordCount();
		for(int j=0; j<recNum;j++)
		{
			rec = &single->GetTrainRecord(j)->m_data;
			if(j == 0)
			{
				CString intext = "";
				if(single->GetTrainRecord(j)->IsStartRec())
					intext = "始发";
				else
					intext = "接入";

				tmp.Format("\n<STATION name=\'%s\' in=\'%s\'>", gpcdef.GetStationName(rec->station,0), intext);
			}
			if(j == recNum-1)
			{
				CString outtext = "";
				if(single->GetTrainRecord(j)->IsTerminalRec())
					outtext = "终到";
				else
					outtext = "交出";

				tmp.Format("\n<STATION name=\'%s\' in=\'%s\'>", gpcdef.GetStationName(rec->station,0), outtext);
			}
			if(j > 0 && j < recNum-1)
			{
				CString passtext = "";
				if(single->GetTrainRecord(j)->IsPassRec())
					passtext = "通过";
				else
					passtext = "到开";
				tmp.Format("\n<STATION name=\'%s\' in=\'%s\'>", gpcdef.GetStationName(rec->station,0), passtext);
			}

			infoFile.WriteString(tmp);

			const CStationDef* pstationside=GetTGBasicParam()->GetStationDef(rec->station);
			if(pstationside)
			{
				SIDE_NAME sideName;
				memset(sideName, 0, sizeof(sideName));
				if(rec->arrive_side == rec->depart_side)
				{
					if(STA_SUCCESS == pstationside->GetSideName(rec->arrive_side, sideName))
						tmp.Format("\n<SIDE>%s</SIDE>", sideName);
					else
						tmp.Format("\n<SIDE>%s</SIDE>", "?G");
				}
				else
				{
					if(STA_SUCCESS == pstationside->GetSideName(rec->arrive_side, sideName))
						tmp.Format("\n<SIDE>%s</SIDE>", sideName);
					else
						tmp.Format("\n<SIDE>%s</SIDE>", "?G");
					infoFile.WriteString(tmp);
					memset(sideName, 0, sizeof(sideName));
					if(STA_SUCCESS == pstationside->GetSideName(rec->depart_side, sideName))
						tmp.Format("\n<SIDE>%s</SIDE>", sideName);
					else
						tmp.Format("\n<SIDE>%s</SIDE>", "?G");
				}
			}
			else
			{
				if(rec->arrive_side == rec->depart_side)
				{
					tmp.Format("\n<SIDE>%s</SIDE>", "?G");
				}
				else
				{
					tmp.Format("\n<SIDE>%s</SIDE>", "?G");
					infoFile.WriteString(tmp);
					tmp.Format("\n<SIDE>%s</SIDE>", "?G");
				}
			}
			infoFile.WriteString(tmp);

			tmp.Format("\n<ARRIVE_TIME>%d:%d:%d</ARRIVE_TIME>", rec->arrive/3600, (rec->arrive%3600)/60, rec->arrive%60);
			infoFile.WriteString(tmp);

			tmp.Format("\n<DEPART_TIME>%d:%d:%d</DEPART_TIME>", rec->depart/3600, (rec->depart%3600)/60, rec->depart%60);
			infoFile.WriteString(tmp);

			infoFile.WriteString("\n</STATION>");
		}
		infoFile.WriteString("\n</TRAIN>");
	}
	infoFile.WriteString("\n</TRAINS>");
	infoFile.Close();
	return TRUE;	
}
//把基本图信息存为tntime文件
/*
[BASIC_SCHEDULE]
CON_ID= 1, TRAIN_INDEX=   0, TRAIN_ID=      Z3, INDEX=  0, STATION=  1, ENTRY= 1, EXIT= 6, SIDE= 1, PLAN_SIDE=0, ARRIVE_TIME= 12195, DEPART_TIME= 12195, ARRIVE_DATE=0, PLAN_ARRIVE=03:23:15, DEPART_DATE=0, PLAN_DEPART=03:23:15, MIN_STOP_TIME=    0, FLAG=8, STOP_CONDITION=0, ADJUST_STATUS=0
[BASIC_SCHEDULE_END]
*/
BOOL CTgDocument::olSaveTrainRecords(CTG_ScheduleInfo::SCHEDULE_TYPE tp,const char * recordsName)
{
	CStdioFile infoFile;
	bool isSucess=true;
	if(!infoFile.Open(recordsName,CFile::modeCreate|CFile::modeWrite))
	{
		AfxMessageBox("创建文件失败!");
		return FALSE;
	}
	CTG_TrainScheduleBase *pData=GetScheduleData(tp);
	if(!pData)
	{
		AfxMessageBox("内部错误：非法运行图类型");
		return FALSE;
	}
	CString txt_eachline;
	CString eachcol;
	CTG_SingleTrain* single = NULL;
	TG_TrainRecord * rec = NULL;
	TG_TrainRecord * pre_rec = NULL;

	int recNum = 0;
	CTime adtime;
	infoFile.WriteString("[BASIC_SCHEDULE]\r\n");

	CTG_TrainPtrGroup  group;
	pData->FindTrains(group);
	for(int i=0; i<group.GetTrainCount(); i++)
	{
		single = group.GetTrainPtr(i);

		ushort uOne_day = 0;
		recNum = single->GetTrainRecordCount();
		if(recNum==1){
			rec = &single->GetTrainRecord(0)->m_data;
			//July 30,2005 lyc
			if(rec->arrive > rec->depart){
				rec->depart = rec->depart+24*3600;
			}

			//end 

			txt_eachline.Empty();
			//CON_ID= 1, 
			eachcol.Format("CON_ID=%2d, ",GetCurrentSectionNo());
			txt_eachline += eachcol;			
			//TRAIN_INDEX=   0, 
			eachcol.Format("TRAIN_INDEX=%4ld, ",single->GetTrainIndex());
			txt_eachline += eachcol;
			//TRAIN_ID=      Z3, 
			eachcol.Format("TRAIN_ID=%8s, ",single->GetTrainID());
			txt_eachline += eachcol;

			//INDEX=  0, 
			eachcol.Format("INDEX=%3d, ",0);
			txt_eachline += eachcol;

			//STATION=  1, 
			eachcol.Format("STATION=%3d, ",rec->station);
			txt_eachline += eachcol;

			//ENTRY= 1, 
			eachcol.Format("ENTRY=%2d, ", short(rec->entry));
			txt_eachline += eachcol;

			//EXIT= 6, 
			eachcol.Format("EXIT=%2d, ",short(rec->exit));
			txt_eachline += eachcol;

			//SIDE= 1, 
			eachcol.Format("SIDE=%2d, ",short(rec->arrive_side));
			txt_eachline += eachcol;

			//PLAN_SIDE=0, 
			eachcol.Format("PLAN_SIDE=%2d, ",short(rec->plan_arrive_side));
			txt_eachline += eachcol;

			//ARRIVE_TIME= 12195, 
			eachcol.Format("ARRIVE_TIME=%6d, ",rec->arrive);
			txt_eachline += eachcol;

			//DEPART_TIME= 12195, 
			eachcol.Format("DEPART_TIME=%6d, ",rec->depart);
			txt_eachline += eachcol;

			//ARRIVE_DATE=0, 
			eachcol.Format("ARRIVE_DATE=%d, ",0);
			txt_eachline += eachcol;

			//PLAN_ARRIVE=03:23:15, 
			eachcol.Format("PLAN_ARRIVE=%s, ",""); //OsConvertTimetoString(rec->arrival_time));
			txt_eachline += eachcol;

			//DEPART_DATE=0, 
			eachcol.Format("DEPART_DATE=%d, ",0);
			txt_eachline += eachcol;

			//PLAN_DEPART=03:23:15, 
			eachcol.Format("PLAN_DEPART=%s, ",""); //OsConvertTimetoString(rec->depart_time));
			txt_eachline += eachcol;

			//MIN_STOP_TIME=    0, 
			eachcol.Format("MIN_STOP_TIME=%5d, ",rec->min_stop_time);
			txt_eachline += eachcol;

			//FLAG=8, 
			eachcol.Format("FLAG=%d, ",rec->flag);
			txt_eachline += eachcol;

			//STOP_CONDITION=0, 
			eachcol.Format("STOP_CONDITION=%d, ",rec->stop_condition);
			txt_eachline += eachcol;

			//ADJUST_STATUS=0
			eachcol.Format("ADJUST_STATUS=%d",rec->adjust_status);
			txt_eachline += eachcol;

			txt_eachline +="\r\n";
			infoFile.WriteString(txt_eachline); // by wuxiang
			continue;
		}
		for(int j=0; j<recNum;j++)
		{
			rec = &single->GetTrainRecord(j)->m_data;
			//July 30,2005 lyc
			if(j == 0){
				if(rec->arrive > rec->depart){
					rec->depart += 24*3600;
				}
			}else{
				pre_rec = &single->GetTrainRecord(j-1)->m_data;
				if(rec->arrive < pre_rec->depart){
					{
						rec->depart+=24*3600;
						rec->arrive+=24*3600;
					}
				}else if((rec->arrive > pre_rec->depart)&&
					(rec->arrive > rec->depart))
				{
					rec->depart+=24*3600;
				}
			}
			// end 

			txt_eachline.Empty();
			//CON_ID= 1, 
			eachcol.Format("CON_ID=%2d, ",GetCurrentSectionNo());
			txt_eachline += eachcol;			
			//TRAIN_INDEX=   0, 
			eachcol.Format("TRAIN_INDEX=%4ld, ",single->GetTrainIndex());
			txt_eachline += eachcol;
			//TRAIN_ID=      Z3, 
			eachcol.Format("TRAIN_ID=%8s, ",single->GetTrainID());
			txt_eachline += eachcol;

			//INDEX=  0, 
			eachcol.Format("INDEX=%3d, ",j);
			txt_eachline += eachcol;

			//STATION=  1, 
			eachcol.Format("STATION=%3d, ",rec->station);
			txt_eachline += eachcol;

			//ENTRY= 1, 
			eachcol.Format("ENTRY=%2d, ",short(rec->entry));
			txt_eachline += eachcol;

			//EXIT= 6, 
			eachcol.Format("EXIT=%2d, ",short(rec->exit));
			txt_eachline += eachcol;

			//SIDE= 1, 
			eachcol.Format("SIDE=%2d, ",short(rec->arrive_side));
			txt_eachline += eachcol;

			//PLAN_SIDE=0, 
			eachcol.Format("PLAN_SIDE=%2d, ",short(rec->plan_arrive_side));
			txt_eachline += eachcol;
			//此点跨越了0点,wangjinhai,wuhan
			/*		    if(last_date == rec->arrival_date && last_time >rec->arrival_time  || rec->arrival_date ==1)
			uOne_day= 1;
			if(uOne_day == 1)
			{
			rec->arrival_date =1;
			rec->depart_date = 1;
			}
			//ARRIVE_TIME= 12195, 
			*/			eachcol.Format("ARRIVE_TIME=%6d, ",rec->arrive);
			txt_eachline += eachcol;

			//DEPART_TIME= 12195, 
			eachcol.Format("DEPART_TIME=%6d, ",rec->depart);
			txt_eachline += eachcol;


			//ARRIVE_DATE=0, 
			eachcol.Format("ARRIVE_DATE=%d, ",0);//rec->arrival_date);
			txt_eachline += eachcol;

			//PLAN_ARRIVE=03:23:15, 
			eachcol.Format("PLAN_ARRIVE=%s, ",""); //OsConvertTimetoString(rec->arrival_time));
			txt_eachline += eachcol;

			//DEPART_DATE=0, 
			eachcol.Format("DEPART_DATE=%d, ",0); //rec->depart_date);
			txt_eachline += eachcol;

			//PLAN_DEPART=03:23:15, 
			eachcol.Format("PLAN_DEPART=%s, ",""); //OsConvertTimetoString(rec->depart_time));
			txt_eachline += eachcol;

			//MIN_STOP_TIME=    0, 
			eachcol.Format("MIN_STOP_TIME=%5d, ",rec->min_stop_time);
			txt_eachline += eachcol;

			//FLAG=8, 
			eachcol.Format("FLAG=%d, ",rec->flag);
			txt_eachline += eachcol;

			//STOP_CONDITION=0, 
			eachcol.Format("STOP_CONDITION=%d, ",rec->stop_condition);
			txt_eachline += eachcol;

			//ADJUST_STATUS=0
			eachcol.Format("ADJUST_STATUS=%d",rec->adjust_status);
			txt_eachline += eachcol;

			txt_eachline +="\r\n";

			infoFile.WriteString(txt_eachline);

		}		
	}
	infoFile.WriteString("[BASIC_SCHEDULE_END]\r\n");
	infoFile.Close();
	return TRUE;	
}
//把基本图信息存为tntime文件
/*
[BASIC_SCHEDULE]
CON_ID= 1, TRAIN_INDEX=   0, TRAIN_ID=      Z3, INDEX=  0, STATION=  1, ENTRY= 1, EXIT= 6, SIDE= 1 
[BASIC_SCHEDULE_END]
*/
BOOL CTgDocument::olSaveTrainSideandEntry(CTG_ScheduleInfo::SCHEDULE_TYPE tp,const char * recordsName)
{
#if 0
	CStdioFile infoFile;
	CStdioFile logFile;
	bool isSucess=true;
	if(!infoFile.Open(recordsName,CFile::modeCreate|CFile::modeWrite))
	{
		AfxMessageBox("创建文件"+recordsName+"失败!");
		return FALSE;
	}
	if(!logFile.Open(recordsName+".log",CFile::modeCreate|CFile::modeWrite))
	{
		AfxMessageBox("创建文件"+recordsName+".log失败!");
		return FALSE;
	}
	CTG_TrainScheduleBase *pData=GetScheduleData(tp);
	if(!pData)
	{
		AfxMessageBox("内部错误：非法运行图类型");
		return FALSE;
	}
	CString txt_eachline;
	CString eachcol;
	CTG_SingleTrain* single = NULL;
	TG_TrainInfo * traininfo = NULL;
	TG_TrainRecord * rec = NULL;
	TG_TrainRecord * pre_rec = NULL;

	int recNum = 0;
	CTime adtime;
	infoFile.WriteString("[BASIC_SCHEDULE]\r\n");
	CTG_TrainPtrGroup  group;
	for(int i=0; i<group.GetTrainCount(); i++)
	{
		single = m_sBasicTrain[i];
		traininfo = &single->m_nTrainInfo;

		ushort uOne_day = 0;
		recNum = single->GetRecordNum();
		if(recNum==1)
		{
			rec = single->GetIndexRecord(0);
			//July 30,2005 lyc
			if(rec->arrival_time > rec->depart_time)
			{
				rec->depart_date = rec->arrival_date + 1;					
			}
			//end 

			txt_eachline.Empty();


			//TRAIN_ID=   Z3, 
			eachcol.Format("%8s, ",traininfo->train_id[0]); //TRAINID=
			txt_eachline += eachcol;

			//INDEX=  0, 
			eachcol.Format("%3d, ",0); //INDEX=
			txt_eachline += eachcol;

			//PASS OR STAY=  1, 
			if(rec->arrival_time == rec->depart_time)
				eachcol.Format("%1d, ",1); //IS_PASS=
			else
				eachcol.Format("%1d, ",0);//IS_PASS=
			txt_eachline += eachcol;

			//STATION=  2, 
			eachcol.Format("%3d, ",rec->station);//STATION=
			txt_eachline += eachcol;

			//STATION name 3
			eachcol.Format("%10s, ",gpcdef.GetStationName(rec->station)); //STATION_NAME=
			txt_eachline += eachcol;

			//SIDE= 4, 
			eachcol.Format("%2d, ",short(rec->arrive_side)); //SIDE=
			txt_eachline += eachcol;

			//ENTRY=5, 
			eachcol.Format("%2d, ", short(rec->entry)); //ENTRY=
			txt_eachline += eachcol;

			//EXIT= 6, 
			eachcol.Format("%2d, ",short(rec->exit)); //EXIT=
			txt_eachline += eachcol;

			//TENICAL STOP= 7, 
			if(IsTechStopTrain(*rec))
				eachcol.Format("%2d, ",short(1));  
			else
				eachcol.Format("%2d, ",short(0));  

			txt_eachline += eachcol;

			txt_eachline +="\r\n";
			logFile.WriteString(txt_eachline);
			isSucess=false;
			continue;
		}
		for(int j=0; j<recNum;j++)
		{
			rec = single->GetIndexRecord(j);

			if(j == 0){
				if(rec->arrival_time > rec->depart_time){
					rec->depart_date = rec->arrival_date + 1;					
				}
			}else{
				pre_rec = single->GetIndexRecord(j-1);
				if(rec->arrival_time < pre_rec->depart_time){
					rec->depart_date = rec->arrival_date = pre_rec->depart_date + 1;
				}else if((rec->arrival_time > pre_rec->depart_time)&&
					(rec->arrival_time > rec->depart_time)){
						rec->depart_date = rec->arrival_date + 1;
					}else{
						rec->depart_date = rec->arrival_date = pre_rec->depart_date;
					}

			}
			// end 

			txt_eachline.Empty();		

			//TRAIN_ID=      Z3, 
			eachcol.Format("%8s, ",traininfo->train_id[0]); //TRAIN_ID=
			txt_eachline += eachcol;

			//INDEX=  0, 
			eachcol.Format("%3d, ",j); //INDEX=
			txt_eachline += eachcol;

			//PASS OR STAY=  1, 
			if(rec->arrival_time == rec->depart_time)
				eachcol.Format("%1d, ",1);//IS_PASS=
			else
				eachcol.Format("%1d, ",0); //IS_PASS=
			txt_eachline += eachcol;

			//STATION=  2, 
			eachcol.Format("%3d, ",rec->station);//STATION=
			txt_eachline += eachcol;

			//STATION= 3, 
			eachcol.Format("%10s, ",gpcdef.GetStationName(rec->station));//STATION_NAME=
			txt_eachline += eachcol;

			//SIDE= 4, 
			eachcol.Format("%2d, ",short(rec->arrive_side));//SIDE=
			txt_eachline += eachcol;

			//ENTRY= 5, 
			eachcol.Format("%2d, ",short(rec->entry)); //ENTRY=
			txt_eachline += eachcol;

			//EXIT= 6, 
			eachcol.Format("%2d, ",short(rec->exit)); //EXIT=
			txt_eachline += eachcol;

			//TENICAL STOP= 7, 
			if(IsTechStopTrain(*rec))
				eachcol.Format("%2d, ",short(1));  
			else
				eachcol.Format("%2d, ",short(0));  

			txt_eachline += eachcol;

			txt_eachline +="\r\n";
			CString tmpstr(traininfo->train_id[0]);

			if(tmpstr==""){				
				logFile.WriteString(txt_eachline);
				isSucess=false;
			}else{
				infoFile.WriteString(txt_eachline);
			}


		}
		infoFile.WriteString("\r\n");
	}
	infoFile.WriteString("[BASIC_SCHEDULE_END]\r\n");
	infoFile.Close();
	logFile.Close();
	if(isSucess)
		logFile.Remove(recordsName+".log");
#endif
	return TRUE;	
}

BOOL  CTgDocument::ProcessMessage(const MsgNoticeNeigbourReceiveSchd *pBuf)
{
	if(pBuf == NULL) 
		return FALSE;

	GPCENTITY entityInfo;
	char sEntityExpress[15];
	sprintf(sEntityExpress, "gpc%d", gpcdef.GetDispatchNoFromDefaultUser());
	if(!gpcdef.GetEntityInfo(sEntityExpress, entityInfo))
	{
		GpcLogTrainMessage(0, 0, "接收到邻台计划接收通知消息, 找不到[%s]的实体号, 接收通知失败", sEntityExpress);
		return FALSE;
	}

	if (pBuf->receive_id != entityInfo.nEntityId)
	{
		GpcLogTrainMessage(0, 0, "邻台计划通知消息, this_entity %d recv_entity %d send_entity %d, 接收通知失败", 
			entityInfo.nEntityId, pBuf->receive_id, pBuf->send_id);
		return FALSE;
	}

	CWorkScheduleHolder *pholder=this->GetTGDataHolder();
	if(!pholder)
		return FALSE;

	GpcLogTrainMessage(0, 0, "邻台计划通知消息, this_entity %d recv_entity %d send_entity %d, notice_conid %d", 
		entityInfo.nEntityId, pBuf->receive_id, pBuf->send_id, pBuf->con_id);

	int nSectionNo=pBuf->con_id;
	if(nSectionNo > 0)
	{
		// 增加接收邻台
		RecvNeigPlan m_recvNeigPlan;

		m_recvNeigPlan.nHour = pBuf->hour;
		m_recvNeigPlan.nSendSectionNo = nSectionNo;
		strcpy(m_recvNeigPlan.strSendSectionName, entityInfo.sName);
		m_recvNeigPlan.strSendSectionName[strlen(entityInfo.sName)] = 0;

		m_vectNeigPlan.push_back(m_recvNeigPlan);
	}

	return TRUE;
}

void CTgDocument::NoticeNeighbourReceivePlan(CTG_ScheduleInfo& schd_info, DWORD entity)
{
	int nCurrentSectionId = schd_info.m_nSectionID;

	int GpcList[32];
	int nGpcNum = sizeof(GpcList)/sizeof(GpcList[0]);
	gpcdef.GetNeighberGpcList(nCurrentSectionId, nGpcNum, GpcList);

	GpcLogTrainMessage(0, 0, "发送邻台计划接收通知消息, 在gpc.def的[SECNEIGHBOR]=%d 中找到 %d个GPC", nCurrentSectionId, nGpcNum);
	if(nGpcNum <= 0)
	{
		AfxMessageBox("请注意, 本台没有邻台");
		return;
	}

	CNoticeNeibRecvPlanDlg dlg;
	for(int i=0; i<nGpcNum; i++)
	{
		GPCENTITY entityInfo;
		char sEntityExpress[15];
		int nGpcNo = GpcList[i];
		memset(sEntityExpress, 0, sizeof(sEntityExpress)/sizeof(sEntityExpress[0]));

		sprintf(sEntityExpress, "gpc%d", nGpcNo);

		if(!gpcdef.GetEntityInfo(sEntityExpress, entityInfo))
			continue;

		dlg.m_nSectionEntityId.Add(entityInfo.nEntityId);
		dlg.m_strSectionName.Add(entityInfo.sName);
	}

	if(dlg.DoModal() == IDOK)
	{
		TIME curtm = time(NULL);
		CLBTH::MsgNoticeNeigbourReceiveSchd msg;
		for(int i=0; i<dlg.m_nSectionEntityId.GetCount(); i++)
		{
			// 没有选中该台
			if(dlg.m_nSectionEntityId[i] == 0)
				continue;

			// 事件处理13: 发邻台计划
			GPCENTITY eventEntity;
			if(gpcdef.GetEntityInfo(136,0,0,eventEntity))
			{
				MsgEventData data;
				data.nSrcEntityID = GetLocalEntityID();
				data.tmEventTime = time(NULL);
				data.nEventRank = NORMAL_EVENT;

				data.nEventID =  EVENT_GPC_SEND_NEIBPLAN;
				data.SetKey(KeyConId, gpcdef.GetCurrentSectNo());
				data.SetKey(KeyNeibConId, GpcList[i]);

				SendBMsg(&data, eventEntity.nEntityId);
			}

			msg.send_id = entity;
			msg.receive_id = dlg.m_nSectionEntityId[i];
			msg.nShiftId = schd_info.m_nShiftID;
			msg.schd_type = schd_info.m_nType;
			msg.hour = dlg.m_nSelHour;
			msg.con_id = nCurrentSectionId;

			if(SendBMsg(&msg, dlg.m_nSectionEntityId[i], 0))
			{
				GpcLogTrainMessage(0, 0, "向[%s %d]发送邻台计划通知消息con_id:%d shiftid:%d schd_type:%d hour:%d", 
					dlg.m_strSectionName[i], 
					dlg.m_nSectionEntityId[i],
					msg.con_id,	msg.nShiftId, msg.schd_type, msg.hour);
			}
			else
			{
				GpcLogTrainMessage(0, 0, "向[%s %d]发送邻台计划通知消息, 失败", 
					dlg.m_strSectionName[i],dlg.m_nSectionEntityId[i]);
			}
		}
	}

	return;
}

// 设置接入交出台
void CTgDocument::SetTrainFromToConid(int conid, CTG_SingleTrain *ptrain)
{
	if(NULL == ptrain)
		return;

	int rec_count=ptrain->GetTrainRecordCount();
	if(rec_count<=0)
		return;

	CTG_TrainRecord* pRec = NULL;

	// 清除台间站标志
	for(int k=0; k<rec_count; k++)
	{
		pRec = ptrain->GetTrainRecordByRecIndex(k);
		if(NULL != pRec)
			TG_SetNoNeighbourStationFlag(pRec->m_data);
	}
	
	pRec = NULL;
	CONID from_conid = NULL_CONID;
	int i=0;
	for(i=0; i<rec_count; i++)
	{
		pRec = ptrain->GetTrainRecordByRecIndex(i);
		if(NULL != pRec)
		{
			if(GetTGFunctionConfig()->IsNotCalFromConid(conid, pRec->GetStationNo(), pRec->GetEntry()))
			{
				from_conid=NULL_CONID;
				break;
			}
			from_conid = GetTGFunctionConfig()->GetTrainRunFromConid(conid, pRec->GetStationNo(), pRec->GetExit(), pRec->GetEntry());

			if(from_conid>0 && from_conid != NULL_CONID)
			{
				if((i==0) && pRec->IsStartRec())
					from_conid=NULL_CONID;
				else
				{
					GpcLogTrainMessage(0, 0, "[FROM_TO_SET] %s FROM CONID:%d(this_conid %d fjksta %d entry %d exit %d)", 
						ptrain->GetTrainID(), from_conid, conid, pRec->GetStationNo(), pRec->GetEntry(), pRec->GetExit());
				}
				break;
			}
		}
	}

	if(from_conid>0 && from_conid != NULL_CONID)
	{
		// 如果匹配车站比较靠后,使用首站
		if(i>3)
			pRec = ptrain->GetTrainRecordByRecIndex(0);
	
		if(NULL != pRec)
			TG_SetNeighbourStationFlag(pRec->m_data);
	}

	pRec = NULL;
	CONID to_conid = NULL_CONID;
	for(i=rec_count-1; i>=0; i--)
	{
		pRec = ptrain->GetTrainRecordByRecIndex(i);
		if(NULL != pRec)
		{
			CTG_TrainRecord* pPreRec = ptrain->GetTrainRecordByRecIndex(i-1);
			if(pPreRec)
			{
				if(GetTGFunctionConfig()->IsLimitSectionCalToConid(conid, pPreRec->GetStationNo(), pRec->GetStationNo()))
				{
					break;
				}
			}
			to_conid = GetTGFunctionConfig()->GetTrainRunToConid(conid, pRec->GetStationNo(), pRec->GetExit());

			if(to_conid>0 && to_conid != NULL_CONID)
			{
				if((i==rec_count-1) && pRec->IsTerminalRec())
					to_conid=NULL_CONID;
				else
				{
					GpcLogTrainMessage(0, 0, "[FROM_TO_SET] %s TO CONID:%d(this_conid %d fjksta %d exit %d)", 
						ptrain->GetTrainID(), to_conid, conid, pRec->GetStationNo(), pRec->GetExit());
				}
				break;
			}
		}
	}

	if(to_conid>0 && to_conid != NULL_CONID)
	{
		// 如果匹配车站比较靠前,使用尾站
		if(rec_count-i>3)
			pRec = ptrain->GetTrainRecordByRecIndex(rec_count-1);
		
		if(NULL != pRec)
			TG_SetNeighbourStationFlag(pRec->m_data);
	}

	ptrain->SetFromToConId(from_conid, to_conid);
	return;
}

// 去掉货车非技术停点
bool CTgDocument::ResetGoodsTrainStopTech(CTG_SingleTrain *ptrain)
{
	if(ptrain->IsPassengerTrain())
		return false;

	if(GetTGFunctionConfig()->GoodsTrainHaveTechStop(ptrain->GetTrainInfo()->m_data.type))
		return false;

	CTG_TrainRecord* pRec = NULL;
	int rec_count=ptrain->GetTrainRecordCount();
	for(int i=0; i<rec_count; i++)
	{
		pRec = ptrain->GetTrainRecordByRecIndex(i);
		if(NULL != pRec)
		{
			if(TG_IsTechStopTrain(pRec->m_data))
				continue;

			// 货车 应设置为技术停点
			TG_SetTechStopTrain(pRec->m_data);
			GpcLogTrainMessage(0, 0, "tg_logic.cfg中GoodTrainHaveTechStop没有配制货车type:%d 保留非技术停点, 程序清除为技术停点 %s 车站%d", 
				ptrain->GetTrainInfo()->m_data.type, ptrain->GetTrainID(),  pRec->GetStationNo());
		}
	}

	return true;
}

// 设置反向行车
void CTgDocument::SetTrainReverseFlag(CTG_SingleTrain *ptrain)
{
	CTG_TrainRecordList &rec=*ptrain->GetRecordList();
	int rec_count=rec.GetRecordCount();
	if(rec_count<=0)
		return;

	for (int i=0; i<rec_count; i++) 
	{
		CString trainid = ptrain->GetTrainID();
		if(rec[i].m_data.plan_arrive <= 0)
		{
			GpcLogTrainMessage(0, 0, "列车: %s plan_arrive %d arrive %d, 将实际点初始化计划点 %d", 
				trainid, rec[i].m_data.plan_arrive, rec[i].m_data.arrive,i);
			rec[i].m_data.plan_arrive = rec[i].m_data.arrive;
		}
		if(rec[i].m_data.plan_depart <= 0)
		{
			GpcLogTrainMessage(0, 0, "列车: %s plan_depart %d depart %d, 将实际点初始化计划点 %d", 
				trainid, rec[i].m_data.plan_depart, rec[i].m_data.depart,i);
			rec[i].m_data.plan_depart = rec[i].m_data.depart;
		}

		if (i<rec_count-1) 
		{
			BOOL bIsDownAbs = GetTGBasicParam()->IsAbsDirectionDown(rec[i].m_data, TRAIN_OUT_STATION);
			BOOL bIsUpTrain = GetTGBasicParam()->IsTrainDirectionUp(rec[i].m_data, TRAIN_OUT_STATION);

			if(bIsDownAbs && bIsUpTrain)
			{
				TG_SetFlagReverse(rec[i].m_data.flag);
				GpcLogTrainMessage(0, 0, "%s 在车站%d反向行车", ptrain->GetTrainID(), rec[i].m_data.station);
				GpcLogTrainMessage(0, 0, "区间方向: %s", GetTGBasicParam()->GetAbsDirectInfo(rec[i].m_data, TRAIN_OUT_STATION));
				GpcLogTrainMessage(0, 0, "列车方向: %s", GetTGBasicParam()->GetTrainDirectInfo(rec[i].m_data, TRAIN_OUT_STATION));

			}
			else
			{
				BOOL bIsUpAbs     = GetTGBasicParam()->IsAbsDirectionUp(rec[i].m_data, TRAIN_OUT_STATION);
				BOOL bIsDownTrain = GetTGBasicParam()->IsTrainDirectionDown(rec[i].m_data, TRAIN_OUT_STATION);
				if(bIsUpAbs && bIsDownTrain)
				{
					TG_SetFlagReverse(rec[i].m_data.flag);
					GpcLogTrainMessage(0, 0, "%s 在车站%d反向行车", ptrain->GetTrainID(), rec[i].m_data.station);
					GpcLogTrainMessage(0, 0, "区间方向: %s", GetTGBasicParam()->GetAbsDirectInfo(rec[i].m_data, TRAIN_OUT_STATION));
					GpcLogTrainMessage(0, 0, "列车方向: %s", GetTGBasicParam()->GetTrainDirectInfo(rec[i].m_data, TRAIN_OUT_STATION));
				}
				else
				{
					TG_ClearFlagReverse(rec[i].m_data.flag);
				}
			}
		}
	}
}

int  CTgDocument::GetTrainIDDirection(CString strTrainId)
{
	// 去掉非数字
	CString strLastNumTrainId="0";
	int len = strTrainId.GetLength();
	for(int i= len-1;i>=0;i--)
	{
		char lastchar = strTrainId.GetAt(i);
		if(0x30 <= lastchar && lastchar <= 0x39)
		{
			strLastNumTrainId = lastchar;
			break;
		}
	}

	len = atoi(strLastNumTrainId);
	if(len%2 == 0)
		return UP_DIRECT;
	else
		return DOWN_DIRECT;
}

BOOL CTgDocument::ReleaseBasicTrainInfo()
{
	for(int i=0; i<basic_train_info_num; i++)
	{
		delete basic_train_info[i];
	}
	basic_train_info_num=0;
	memset(basic_train_info, 0, sizeof(basic_train_info));
	return TRUE;
}

BOOL CTgDocument::LoadTrainSideTechToFile(const CString& fileName)
{
	basic_train_info_num=0;
	memset(basic_train_info, 0, sizeof(basic_train_info));

	MSXML::IXMLDOMDocumentPtr pXMLDoc;
	HRESULT hr = pXMLDoc.CreateInstance(MSXML::CLSID_DOMDocument);
	if(FAILED(hr))
	{
		_com_error er(hr);
		AfxMessageBox(er.ErrorMessage());
		return FALSE;
	}

	if(VARIANT_TRUE != pXMLDoc->load( _bstr_t( fileName ) ) )
	{
		CString  prompt;
		prompt.Format("股道作业文件 %s不存在或者不符合xml语法(可以用浏览器检查是否能打开)", fileName);
		AfxMessageBox(prompt);

		return FALSE; 
	}

	MSXML::IXMLDOMNodePtr pChildNode,pNode,pRootNode,pNodeTrain,pNodeStation;
	MSXML::IXMLDOMNodeListPtr list, liststation;

	pRootNode = pXMLDoc->selectSingleNode( "TRAINS");
	if(pRootNode == NULL)
		return FALSE;

	list = pRootNode->selectNodes( "TRAIN");
	if(NULL == list)
		return FALSE;


	for(int i=0; i<list->Getlength() && i < 2048; i++)
	{
		pNodeTrain = list->Getitem(i);

		liststation = pNodeTrain->selectNodes( "STATION");
		if(NULL == liststation)
			continue;

		pNode = pNodeTrain->selectSingleNode("@name");
		if(NULL == pNode)
			continue;

		NODE_TRAIN* ptrain = new NODE_TRAIN();
		if(ptrain == NULL)
			continue;

		_variant_t v;
		v = pNode->GetnodeTypedValue();
		CString name(v.bstrVal);			
		SysFreeString(v.bstrVal);
		v.Clear();

		memset(ptrain->name, 0, 16);
		strncpy(ptrain->name, name.GetBuffer(), 15);

		ptrain->num=0;
		for(int i=0; i<liststation->Getlength(); i++)
		{
			pNodeStation = liststation->Getitem(i);

			pNode = pNodeStation->selectSingleNode("@station");
			if(NULL != pNode)
			{
				_variant_t v;
				v = pNode->GetnodeTypedValue();
				ptrain->node[ptrain->num].station = atoi(CString(v.bstrVal));			
				SysFreeString(v.bstrVal);
				v.Clear();
			}
			pNode = pNodeStation->selectSingleNode("@arrive_trainid");
			if(NULL != pNode)
			{
				_variant_t v;
				v = pNode->GetnodeTypedValue();
				CString name(v.bstrVal);
				memset(ptrain->node[ptrain->num].arrive_trainid, 0, sizeof(TRAIN_NO));
				strncpy(ptrain->node[ptrain->num].arrive_trainid, name.GetBuffer(), sizeof(TRAIN_NO)-1);			
				SysFreeString(v.bstrVal);
				v.Clear();
			}
			pNode = pNodeStation->selectSingleNode("@depart_trainid");
			if(NULL != pNode)
			{
				_variant_t v;
				v = pNode->GetnodeTypedValue();
				CString name(v.bstrVal);
				memset(ptrain->node[ptrain->num].depart_trainid, 0, sizeof(TRAIN_NO));
				strncpy(ptrain->node[ptrain->num].depart_trainid, name.GetBuffer(), sizeof(TRAIN_NO)-1);			
				SysFreeString(v.bstrVal);
				v.Clear();
			}
			pNode = pNodeStation->selectSingleNode("@arrive_side");
			if(NULL != pNode)
			{
				_variant_t v;
				v = pNode->GetnodeTypedValue();
				ptrain->node[ptrain->num].arrive_side = (BYTE)atoi(CString(v.bstrVal));			
				SysFreeString(v.bstrVal);
				v.Clear();
			}
			pNode = pNodeStation->selectSingleNode("@depart_side");
			if(NULL != pNode)
			{
				_variant_t v;
				v = pNode->GetnodeTypedValue();
				ptrain->node[ptrain->num].depart_side = (BYTE)atoi(CString(v.bstrVal));			
				SysFreeString(v.bstrVal);
				v.Clear();
			}
			pNode = pNodeStation->selectSingleNode("@tech_stop");
			if(NULL != pNode)
			{
				_variant_t v;
				v = pNode->GetnodeTypedValue();
				ptrain->node[ptrain->num].tech_stop = (BYTE)atoi(CString(v.bstrVal));			
				SysFreeString(v.bstrVal);
				v.Clear();
			}
			ptrain->num++;
		}
		basic_train_info[basic_train_info_num++] = ptrain;
	}

	return TRUE;
}

BOOL CTgDocument::SaveTrainSideTechToFile(CTG_ScheduleInfo::SCHEDULE_TYPE tp,const char * fileName)
{
	CStdioFile infoFile;
	bool isSucess=true;
	if(!infoFile.Open(fileName,CFile::modeCreate|CFile::modeWrite))
	{
		AfxMessageBox("创建可视文本文件失败!");
		return FALSE;
	}
	CTG_TrainScheduleBase *pData=GetScheduleData(tp);
	if(!pData)
	{
		AfxMessageBox("内部错误：非法运行图类型");
		return FALSE;
	}
	CString txt_eachline;
	CString eachcol;
	CTG_SingleTrain* single = NULL;
	TG_TrainRecord * rec = NULL;
	const CTG_TrainRecord * prec = NULL;

	int recNum = 0;

	CTG_TrainPtrGroup  group;
	pData->FindTrains(group);
	CString tmp;
	tmp = "<?xml version=\"1.0\" encoding=\"GB2312\" ?>";
	infoFile.WriteString(tmp);
	infoFile.WriteString("\n<TRAINS>");
	CDlgWait *pw=new CDlgWait("正在保存基本图信息,可能需要1-2分钟时间,请稍侯...");
	for(int i=0; i<group.GetTrainCount(); i++)
	{
		single = group.GetTrainPtr(i);
		if(NULL == single)
			continue;

		pw->UpdateDisplay(single->GetTrainID());
		tmp.Format("\n<TRAIN name=\"%s\">",  single->GetTrainID());
		infoFile.WriteString(tmp);
		recNum = single->GetTrainRecordCount();
		for(int j=0; j<recNum;j++)
		{
			prec = single->GetTrainRecord_Const(j);
			if(prec == NULL)
				continue;

			int tech_stop=0;
			if(TG_IsTechStopTrain(prec->m_data))
				tech_stop=1;

			tmp.Format("\n<STATION station =\"%d\" arrive_trainid=\"%s\" depart_trainid=\"%s\" arrive_side=\"%d\" depart_side=\"%d\" tech_stop=\"%d\"/>", 
				prec->GetStationNo(), prec->GetArriveTID(), prec->GetDepartTID(), prec->m_data.arrive_side,  prec->m_data.depart_side, tech_stop);

			infoFile.WriteString(tmp);
		}
		infoFile.WriteString("\n</TRAIN>");
	}
	infoFile.WriteString("\n</TRAINS>");
	infoFile.Close();

	delete pw;
	return TRUE;	
}



BOOL CTgDocument::InitBasicInfo(CTG_SingleTrain * ptrain)
{
	if(ptrain==NULL)
		return FALSE;

	CString trainid= ptrain->GetTrainID();
	// 按照车次查找列车
	int i=0;
	for(i=0; i<basic_train_info_num&&i<2048; i++)
	{
		if(NULL == basic_train_info[i])
			continue;

		if(trainid == basic_train_info[i]->name)
			break;
	}

	if(i>=basic_train_info_num||i>=2048)
		return FALSE;

	CTG_TrainRecord* prec = NULL;
	int station;
	CString arrive_trainid,depart_trainid;

	for(int k=0; k<ptrain->GetTrainRecordCount(); k++)
	{
		prec = ptrain->GetTrainRecordByRecIndex(k);

		if(NULL == prec)
			continue;

		station = prec->GetStationNo();
		arrive_trainid = prec->GetArriveTID();
		depart_trainid = prec->GetDepartTID();
		if(arrive_trainid.IsEmpty() && depart_trainid.IsEmpty())
			continue;
		for(int n=0; n<basic_train_info[i]->num&&n<128; n++)
		{
			if(basic_train_info[i]->node[n].station == station)
			{
				if( (arrive_trainid.IsEmpty() || (!arrive_trainid.IsEmpty() && arrive_trainid == basic_train_info[i]->node[n].arrive_trainid))
					&&(depart_trainid.IsEmpty() || (!depart_trainid.IsEmpty() && depart_trainid == basic_train_info[i]->node[n].depart_trainid)))
				{
					if(basic_train_info[i]->node[n].arrive_side != 255)
					{
						prec->m_data.arrive_side=basic_train_info[i]->node[n].arrive_side;
						prec->m_data.plan_arrive_side=prec->m_data.arrive_side;
					}

					if(basic_train_info[i]->node[n].depart_side != 255)
					{
						prec->m_data.depart_side=basic_train_info[i]->node[n].depart_side;
						prec->m_data.plan_depart_side=prec->m_data.depart_side;
					}

					if(basic_train_info[i]->node[n].tech_stop==1)
						TG_SetTechStopTrain(prec->m_data);
					else
						TG_ClearTechStopTrain(prec->m_data);
					break;
				}
			}
		}
	}

	return TRUE;
}

void CTgDocument::SendGpcAckMsg(int type, bool isOk, long src, CString msgid, CString jht, CString result)
{
	CString token,content;
	if(isOk)
		TD_GenAckMessage_GPCACKINFO(token, content, GetCurrentSectionNo(), type, 1,gpcdef.GetCurrentUsername(),src,msgid,jht, result);
	else
		TD_GenAckMessage_GPCACKINFO(token, content, GetCurrentSectionNo(), type, 0,gpcdef.GetCurrentUsername(),src,msgid,jht, result);
	if(!token.IsEmpty())
	{
		long  eid=GetSpecialEntityID(SERVER_TYPE_TDCOMM); // 从TD获取,回执TD
		if(gpcdef.IsEnableMMPlanFromCAD()) // 从服务器获取,回执服务器
			eid=GetSpecialEntityID(SERVER_TYPE_MMPLAN); 

		TGSendStringMessage(token,content,eid);

		CString text;
		text.Format("向 TD 发送回执消息: 类型:%d  状态:%d 源码:%d 消息码:%u 计划台号:%s 消息内容: %s", type, isOk, src, msgid, jht, content);
		GpcLogTrainMessage(0,0,text);
	}

	
}

// 检查TDMS股道是否满足站细
void  CTgDocument::CheckTdmsSideByStationside(int iSection, CTG_SingleTrain& train) 
{
	if(!GetTGFunctionConfig()->IsCheckTDMSSide())
		return;

	bool bCheckTdmsSide=false;
	BOOL bUseOPMSShiftSchdSide = GetTGFunctionConfig()->IsEnableOPMSShiftSchdSide(iSection);
	if(bUseOPMSShiftSchdSide)
	{
		int train_type=train.GetTrainType();
		if(train.IsPassengerTrain()) // 客车
		{
			if(train.IsCRHTrain())
			{
				bCheckTdmsSide=true;// 动车组必须使用TDMS股道
			}
			else
			{
				if(GetTGFunctionConfig()->IsPessagerTrainNotUseOPMSSide(iSection, train_type))
				{
					//GpcLogTrainMessage(0, 0, "列车GID:%u 车次:%s train_type:%d 配置不使用TDMS股道", 
					//	ptrain->GetShiftTrainIndex(), ptrain->GetTrainID(), train_type);
					;
				}
				else
				{
					bCheckTdmsSide=true;
				}
			}
		}
		else
		{
			if(GetTGFunctionConfig()->IsGoodTrainUseOPMSSide(train_type))
			{
				bCheckTdmsSide=true;
			}
		}
	}
	
	if(!bCheckTdmsSide)
		return;

	for(int rec_index=0; rec_index<train.GetTrainRecordCount(); rec_index++)
	{
		CTG_TrainRecord *precord=train.GetTrainRecord(rec_index);
		if(!precord)
			continue;
		int station = precord->GetStationNo();
		const char* strStaName = gpcdef.GetStationName(station);
		// 股道封锁
		if(!strStaName)
			return;
	
		if(!gpcdef.IsCtcStation(station))  
		{
			continue;
		}

		const CStationDef* pSideDef = GetTGBasicParam()->GetStationDef(station);
		if(!pSideDef)
		{
			continue;
		}

		bool bStart=false, bTerminal=false;
		if(precord->IsStartRec() && (0==rec_index)) // 始发车
		{
			bStart=true;
		}
		if(precord->IsTerminalRec() && (train.GetTrainRecordCount()==rec_index+1)) // 终到车
		{
			bTerminal=true;
		}

		BYTE nSideNo=precord->m_data.arrive_side;
		
		CString strSideName;
		CStationDef::SIDE_T side;
		if(STA_FAILED == pSideDef->GetSideInfo(nSideNo, side))
		{
			continue;
		}
		else
		{
			SIDE_NAME side_name;
			memset(side_name, 0, sizeof(side_name));
			pSideDef->GetSideName(nSideNo, side_name);
			strSideName=side_name;
		}

		// 检查股道和出入口是否连通
		int entry = precord->GetEntry();
		int exit = precord->GetExit();
		CStationDef::ENTRY_T entryT;
		if(!bStart && (STA_SUCCESS == pSideDef->GetEntryInfo(entry, entryT)))
		{
			if (STA_FAILED==pSideDef->isEntrySideConnect(nSideNo, entry))
			{
				CString strText;
				strText.Format("TDMS股道检查:车次%s %s 入口%d和%s 不连通,设置为无股道",
									train.GetTrainID(), strStaName, entry, strSideName);
				GpcLogTrainMessage(0, 0, "%s", strText);

				precord->m_data.arrive_side=255;
				precord->m_data.depart_side=255;
				continue;
			}
		}

		if(!bTerminal && (STA_SUCCESS == pSideDef->GetEntryInfo(exit, entryT)))
		{
			if (STA_FAILED == pSideDef->isEntrySideConnect(nSideNo, exit))
			{
				CString strText;
				strText.Format("TDMS股道检查:车次%s %s %s 和出口%d 不连通,设置为无股道",
									train.GetTrainID(), strStaName, strSideName, exit);
				GpcLogTrainMessage(0, 0, "%s", strText);

				precord->m_data.arrive_side=255;
				precord->m_data.depart_side=255;
				continue;
			}
		}

		// 客车办理客运作业站台检查
		if(train.IsPassengerTrain())
		{
			bool bCheckPlatform=false;
			if(bStart || bTerminal)
			{
				bCheckPlatform=true;
			}
			else
			{
				if (precord->IsPassRec())
					bCheckPlatform=false;
				else
					bCheckPlatform=true;
			}
			if (bCheckPlatform && !TG_IsTechStopTrain(precord->m_data))
			{
				if (!pSideDef->isPlatform(nSideNo) )
				{
					CString strText;
					strText.Format("TDMS股道检查:%s %s 无站台,客车 %s 不能办理客运作业,设置为无股道", 
						strStaName, strSideName, train.GetTrainID());
					GpcLogTrainMessage(0, 0, "%s", strText);

					precord->m_data.arrive_side=255;
					precord->m_data.depart_side=255;
					continue;
				}
			}
		}
	}
}

void CTgDocument::CheckTrainSideInfo(CTG_SingleTrain& train, CStringArray& ary_list)
{
	ary_list.RemoveAll();
	TG_TrainInfo *traininfo=&train.GetTrainInfo()->m_data;
	for(int rec_index=0; rec_index<train.GetTrainRecordCount(); rec_index++)
	{
		CTG_TrainRecord *precord=train.GetTrainRecordNoDelete(rec_index);
		if(!precord)
			continue;
		
		TG_TrainRecord *rec=&precord->m_data; 
		const char* strStaName = gpcdef.GetStationName(precord->GetStationNo());
		const char* strTrainName = train.GetTrainID();
		// 股道封锁
		if(!strStaName || !strTrainName)
			continue;
	
		if(!gpcdef.IsCtcStation(precord->m_data.station))  // 沈阳局TDCS车站,在人工操作时不需要往下面检查计划 2014.05.19
		{
			continue;
		}
		CString strText;
		const CStationDef* pSideDef = GetTGBasicParam()->GetStationDef(rec->station);
		if(!pSideDef)
		{
			strText.Format("获取 %s 站细失败,请核对 %s 是否满足站细", strStaName, strTrainName);
			ary_list.Add(strText);
			continue;
		}

		SIDE_NAME arrive_side_name, depart_side_name;
		memset(arrive_side_name, 0, sizeof(arrive_side_name));
		memset(depart_side_name, 0, sizeof(depart_side_name));

		bool bStart=false, bTerminal=false;
		bool bArriveSideOK=true, bDepartSideOK=true;

		if(precord->IsStartRec() && (0==rec_index)) // 始发车
		{
			bStart=true;
		}
		if(precord->IsTerminalRec() && (train.GetTrainRecordCount()==rec_index+1)) // 终到车
		{
			bTerminal=true;
		}
		CStationDef::SIDE_T side;
		if(STA_FAILED == pSideDef->GetSideInfo(rec->arrive_side, side))
		{
			strcpy(arrive_side_name, "?G");
			bArriveSideOK=false;
			if(!bStart)
			{
				strText.Format("%s次在%s站未分配接车股道", strTrainName, strStaName);
				ary_list.Add(strText);
			}
		}
		else
		{
			pSideDef->GetSideName(rec->arrive_side, arrive_side_name);
		}

		if(STA_FAILED == pSideDef->GetSideInfo(rec->depart_side, side))
		{
			strcpy(depart_side_name, "?G");
			bDepartSideOK=false;
			if(!bTerminal)
			{
				strText.Format("%s次在%s站未分配发车股道", strTrainName, strStaName);
				ary_list.Add(strText);
			}
		}
		else
		{
			pSideDef->GetSideName(rec->depart_side, depart_side_name);
		}

		if(bArriveSideOK && bDepartSideOK && !bStart && !bTerminal)
		{
			if(rec->arrive_side != rec->depart_side)// 到达出发股道是否一致
			{
				if(train.IsCRHTrain())
				{
					strText.Format("旅客列车%s次在%s站到发股道不一致", strTrainName, strStaName); 
					ary_list.Add(strText);
				}
				else
				{
					strText.Format("列车%s次在%s站到发股道不一致", strTrainName, strStaName); 
					ary_list.Add(strText);
				}
			}
		}

		if(bStart) // 始发检查发车股道
		{
			if(bDepartSideOK)
			{
				// 检查超限车固定股道
				if (STA_FAILED==pSideDef->isReceiveExceedTrain(rec->depart_side, train.GetSpaceRank(rec_index)) )
				{
					strText.Format("超限列车%s次在%s站禁止由%s发车", strTrainName, strStaName, depart_side_name);
					ary_list.Add(strText);
				}
				
				// 检查股道电力网
				if (train.IsElectricTrain())
				{
					if (STA_FAILED==pSideDef->isCatenary(rec->depart_side) )
					{
						strText.Format("%s次在%s站禁止由无接触网的%s发车", strTrainName, strStaName, depart_side_name);
						ary_list.Add(strText);
					}
				}
				// 检查股道接发车性质
				BYTE nTrainType = train.IsPassengerTrain() ? CStationDef::RECV_PASSENGER : CStationDef::RECV_CARGO;
				if (STA_FAILED==pSideDef->isReceiveTrain(rec->depart_side, nTrainType) )
				{
					strText.Format("%s次在%s站禁止由%s发车,违反客货列车接发条件", strTrainName, strStaName, depart_side_name);
					ary_list.Add(strText);
				}
				// 检查客车
				if ( train.IsPassengerTrain())
				{
					// 客运作业
					if (!TG_IsTechStopTrain(*rec))
					{
						if ( !pSideDef->isPlatform(rec->depart_side) )
						{
							strText.Format("%s站%s无站台,%s次不能办理客运作业", strStaName, depart_side_name, strTrainName);
							ary_list.Add(strText);
						}
						else if (train.IsCRHTrain() && !pSideDef->isHighPlatform(rec->depart_side) )
						{
							strText.Format("动车组%s次在%s站接入非高站台股道", strTrainName, strStaName);
							ary_list.Add(strText);
						}
					}
				}
			}
		}
		else if(bTerminal)
		{
			if(bArriveSideOK)
			{
				// 检查超限车固定股道
				if (STA_FAILED==pSideDef->isReceiveExceedTrain(rec->arrive_side, train.GetSpaceRank(rec_index)) )
				{
					strText.Format("超限列车%s次在%s站禁止接入%s", strTrainName, strStaName, arrive_side_name);
					ary_list.Add(strText);
				}

				// 检查股道电力网
				if (train.IsElectricTrain())
				{
					if (STA_FAILED==pSideDef->isCatenary(rec->arrive_side) )
					{
						strText.Format("%s次在%s站禁止接入无接触网的%s", strTrainName, strStaName, arrive_side_name);
						ary_list.Add(strText);
					}
				}

				// 检查股道接发车性质
				BYTE nTrainType = train.IsPassengerTrain() ? CStationDef::RECV_PASSENGER : CStationDef::RECV_CARGO;
				if (STA_FAILED==pSideDef->isReceiveTrain(rec->arrive_side, nTrainType) )
				{
					strText.Format("%s次在%s站禁止接入%s,违反客货列车接发条件", strTrainName, strStaName, arrive_side_name);
					ary_list.Add(strText);
				}
				// 检查客车
				if ( train.IsPassengerTrain())
				{
					// 客运作业
					if (!TG_IsTechStopTrain(*rec))
					{
						if ( !pSideDef->isPlatform(rec->arrive_side) )
						{
							strText.Format("%s站%s无站台,%s次不能办理客运作业", strStaName, arrive_side_name, strTrainName);
							ary_list.Add(strText);
						}
						else if (train.IsCRHTrain() && !pSideDef->isHighPlatform(rec->arrive_side) )
						{
							strText.Format("动车组%s次在%s站接入非高站台股道", strTrainName, strStaName);
							ary_list.Add(strText);
						}
					}
				}
			}
		}
		else
		{
			// 检查超限车固定股道
			if (bArriveSideOK && STA_FAILED==pSideDef->isReceiveExceedTrain(rec->arrive_side, train.GetSpaceRank(rec_index)) )
			{
				strText.Format("超限列车%s次在%s站禁止接入%s", strTrainName, strStaName, arrive_side_name);
				ary_list.Add(strText);
			}
			if (bDepartSideOK && STA_FAILED==pSideDef->isReceiveExceedTrain(rec->depart_side, train.GetSpaceRank(rec_index)) )
			{
				strText.Format("超限列车%s次在%s站禁止由%s发车", strTrainName, strStaName, depart_side_name);
				ary_list.Add(strText);
			}

			// 检查股道电力网
			if (train.IsElectricTrain())
			{
				if (bArriveSideOK && STA_FAILED==pSideDef->isCatenary(rec->arrive_side) )
				{
					strText.Format("%s次在%s站禁止接入无接触网的%s", strTrainName, strStaName, arrive_side_name);
					ary_list.Add(strText);
				}
				if (bDepartSideOK && STA_FAILED==pSideDef->isCatenary(rec->depart_side) )
				{
					strText.Format("%s次在%s站禁止由无接触网的%s发车", strTrainName, strStaName, depart_side_name);
					ary_list.Add(strText);
				}
			}

			// 检查股道接发车性质
			BYTE nTrainType = train.IsPassengerTrain() ? CStationDef::RECV_PASSENGER : CStationDef::RECV_CARGO;
			if (bArriveSideOK && STA_FAILED==pSideDef->isReceiveTrain(rec->arrive_side, nTrainType) )
			{
				strText.Format("%s次在%s站禁止接入%s,违反客货列车接发条件", strTrainName, strStaName, arrive_side_name);
				ary_list.Add(strText);
			}
			if (bDepartSideOK && (rec->arrive_side != rec->depart_side) && STA_FAILED==pSideDef->isReceiveTrain(rec->depart_side, nTrainType) )
			{
				strText.Format("%s次在%s站禁止由%s发车,违反客货列车接发条件", strTrainName, strStaName, depart_side_name);
				ary_list.Add(strText);
			}

			// 检查客车
			if ( train.IsPassengerTrain())
			{
				// 侧线通过
				if (bArriveSideOK && bDepartSideOK && GetTGBasicParam()->IsTrainPassNotUseMainSide(precord->m_data))
				{
					strText.Format("%s次在%s站%s侧线通过", strTrainName, strStaName, arrive_side_name);
					ary_list.Add(strText);
				}
				// 客运作业
				if (!precord->IsPassRec() && !TG_IsTechStopTrain(*rec))
				{
					if (bArriveSideOK)
					{
						if (!pSideDef->isPlatform(rec->arrive_side) )
						{
							strText.Format("%s站%s无站台,%s次不能办理客运作业", strStaName, arrive_side_name, strTrainName);
							ary_list.Add(strText);
						}
						else if (train.IsCRHTrain() && !pSideDef->isHighPlatform(rec->arrive_side) )
						{
							strText.Format("动车组%s次在%s站接入非高站台股道", strTrainName, strStaName);
							ary_list.Add(strText);
						}
					}
					if (bDepartSideOK && (rec->arrive_side != rec->depart_side))
					{
						if (!pSideDef->isPlatform(rec->depart_side) )
						{
							strText.Format("%s %s 无站台,客车 %s 不能办理客运作业", strStaName, depart_side_name, strTrainName);
							ary_list.Add(strText);
						}
						else if (train.IsCRHTrain() && !pSideDef->isHighPlatform(rec->depart_side) )
						{
							strText.Format("动车组%s次在%s站接入非高站台股道", strTrainName, strStaName);
							ary_list.Add(strText);
						}
					}
				}
			}
		}

		// 检查股道和出入口是否连通
		if(!bStart)
		{
			CStationDef::ENTRY_T entry;
			if (STA_FAILED == pSideDef->GetEntryInfo(rec->entry, entry))
			{
				strText.Format("%s次在%s站入口线别错误",strTrainName,strStaName); // stplan warn 20161019
				ary_list.Add(strText);
			}
			else
			{
				if (bArriveSideOK && STA_FAILED==pSideDef->isEntrySideConnect(rec->arrive_side, rec->entry))
				{
					strText.Format("%s次在%s站不能经由%d口接入%s",strTrainName, strStaName, rec->entry, arrive_side_name);
					ary_list.Add(strText);
				}
			}
		}

		if(!bTerminal)
		{
			CStationDef::ENTRY_T entry;
			if (STA_FAILED == pSideDef->GetEntryInfo(rec->exit, entry))
			{
				strText.Format("%s次在%s站出口线别错误",strTrainName,strStaName); // stplan warn 20161019
				ary_list.Add(strText);
			}
			else
			{
				if (bDepartSideOK && STA_FAILED == pSideDef->isEntrySideConnect(rec->depart_side, rec->exit))
				{
					strText.Format("%s次在%s站不能经由%s发往%d口",strTrainName, strStaName, depart_side_name, rec->exit);
					ary_list.Add(strText);
				}
			}
		}
	}
	return;
}
