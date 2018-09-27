
#include "stdafx.h"

#include "tg_work_schdule_holder.h"
#include "tg_addtrain_logic.h"
#include "tg_autoadjust.h"
#include "function.h"
#include "tg_filebase.h"
#include "sysshare.h"
#include "tgx.h"
#include "tg_BinXml.h"
#include "tg_other_structdef.h"
extern TG_STATION_STATUS *GetStationStatusPtr(int station); // defined in tg_logic_main.cpp
extern void  RemoveTrainAbsTimeType(CTG_SingleTrain* ptrain);
extern void  SetTrainAbsTimeType(CTG_SingleTrain* ptrain, BYTE run_type);
extern BYTE  GetTrainAbsTimeType(const CTG_SingleTrain* ptrain);
extern BOOL  SetNewTrainAbsRunTime(CTG_SingleTrain& train, int rec_index, BYTE run_type);
extern BOOL  ReqBasicName_ToServer(int con_id, bool bload);
#define MAX_STORED_HOLDER_NUM  24
static struct{
	CWorkScheduleHolder *pDataHolder;
}g_data_holders[MAX_STORED_HOLDER_NUM];

static BOOL RegisterHolder(CWorkScheduleHolder *p)
{
	int i;
	for(i=0;i<MAX_STORED_HOLDER_NUM;i++)
	{
		if(g_data_holders[i].pDataHolder==NULL)
			break;
	}
	if(i>=MAX_STORED_HOLDER_NUM)
		return FALSE;
	g_data_holders[i].pDataHolder=p;
	return TRUE;
}

static BOOL UnRegisterHolder(CWorkScheduleHolder *p)
{
	int i;
	for(i=0;i<MAX_STORED_HOLDER_NUM;i++)
	{
		if(g_data_holders[i].pDataHolder==p)
			break;
	}
	if(i>=MAX_STORED_HOLDER_NUM)
		return FALSE;
	g_data_holders[i].pDataHolder=NULL;
	return TRUE;
}

static int  GetEntryByTmisAbsNumber(int station,int tmisabsnum)
{
	long absnum=GetTGBasicParam()->GetAbsNumByTmisNum(tmisabsnum);
	if(absnum<0)
		absnum=tmisabsnum;
	ushort st1=0,etr1=255,st2=0,etr2=255;
	GetTGBasicParam()->ConvertAbsnumToStationEntry(&st1,&etr1,&st2,&etr2,absnum);
	if(station==st1)
		return etr1;
	if(station==st2)
		return etr2;
	return -1;
}

BOOL  CWorkScheduleHolder::InitTGHolders()
{
	int i;
	for(i=0;i<MAX_STORED_HOLDER_NUM;i++)
	{
		g_data_holders[i].pDataHolder=NULL;
	}
	return TRUE;
}

static TIME g_polling_time=0;
BOOL  CWorkScheduleHolder::Polling(TIME curtm)
{
	int i;
	for(i=0;i<MAX_STORED_HOLDER_NUM;i++)
	{
		if(g_data_holders[i].pDataHolder)
		{
			g_data_holders[i].pDataHolder->PollingData(curtm); 	
		}
	}

	return TRUE;
}

// 2013.05.07处理运行图与Commsvr闪断
void CWorkScheduleHolder::GlobalRefreshCommStatus()
{
	if(GetTGFunctionConfig()->IsClosePollingCommStatus())
		return;

	int i;
	for(i=0;i<MAX_STORED_HOLDER_NUM;i++)
	{
		if(g_data_holders[i].pDataHolder)
			g_data_holders[i].pDataHolder->RefreshCommStatus(); 	
	}
}

BOOL  CWorkScheduleHolder::GlobalSetCommInterfaceStatus(IScheduleComm *p,BOOL va)
{
	int i;
	for(i=0;i<MAX_STORED_HOLDER_NUM;i++)
	{
		if(g_data_holders[i].pDataHolder)
		{
			g_data_holders[i].pDataHolder->InitCommInterfaceStatus(p,va);
			g_data_holders[i].pDataHolder->PostCommStatus(p,va);
		}
	}
	return TRUE;
}

CWorkScheduleHolder::CWorkScheduleHolder()
{
	RegisterHolder(this); 
	
	m_pMasterServerCommBasic=NULL;
	m_pMasterServerCommBasicPrev=NULL;
	m_pMasterServerCommBasicBackup=NULL;
	m_bQueryMode=TRUE;
	for(int i=0;i<MAX_COMM_INTF_NUM;i++)
	{
		m_aCommArray[i].nSchType=CTG_ScheduleInfo::SCHEDULE_TYPE_NULL; 
        m_aCommArray[i].nSectionID=0;
		m_aCommArray[i].pCommInterface=NULL;
		m_aCommArray[i].bActive=FALSE;
	}
}
CWorkScheduleHolder::~CWorkScheduleHolder()
{
    UnRegisterHolder(this);
}

BOOL  CWorkScheduleHolder::InitScheduleData(int section)
{
	CTG_ScheduleInfo basic_schd;
	basic_schd.m_nType= CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC; 
	basic_schd.m_nSectionID=section;
	basic_schd.m_tStartTime=0; 
	basic_schd.m_tEndTime=86400*3;
	m_cBasicSchduleData.SetScheduleInfo(basic_schd); 
	m_cBasicSchduleData.DeleteAllData(basic_schd,SYCFLAG_NULL);

	basic_schd.m_nType= CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_PREV; 
	basic_schd.m_nSectionID=section;
	basic_schd.m_tStartTime=0; 
	basic_schd.m_tEndTime=86400*3;
	m_cBasicPrevSchduleData.SetScheduleInfo(basic_schd); 
	m_cBasicPrevSchduleData.DeleteAllData(basic_schd,SYCFLAG_NULL);

	basic_schd.m_nType= CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_BACKUP; 
	basic_schd.m_nSectionID=section;
	basic_schd.m_tStartTime=0; 
	basic_schd.m_tEndTime=86400*3;
	m_cBasicBackupSchduleData.SetScheduleInfo(basic_schd); 
	m_cBasicBackupSchduleData.DeleteAllData(basic_schd,SYCFLAG_NULL);
	

	return TRUE;
}

BOOL CWorkScheduleHolder::LoadDataFromLocalFiles(CTG_ScheduleInfo::SCHEDULE_TYPE type)
{
	//DeleteAllData();
	CTG_ScheduleInfo info;
	if(type==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC)
	{
		m_cBasicSchduleData.GetScheduleInfo(info);
	    m_cBasicSchduleData.DeleteAllData(info,SYCFLAG_NULL); 
		TG_LoadFromFiles(&m_cBasicSchduleData);
	}
	if(type==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_PREV)
	{
		m_cBasicPrevSchduleData.GetScheduleInfo(info);
	    m_cBasicPrevSchduleData.DeleteAllData(info,SYCFLAG_NULL); 
		TG_LoadFromFiles(&m_cBasicPrevSchduleData);
	}
	if(type==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_BACKUP)
	{
		m_cBasicBackupSchduleData.GetScheduleInfo(info);
	    m_cBasicBackupSchduleData.DeleteAllData(info,SYCFLAG_NULL); 
		TG_LoadFromFiles(&m_cBasicBackupSchduleData);
	}

	return TRUE;
}
BOOL CWorkScheduleHolder::SaveDataToLocalFiles(CTG_ScheduleInfo::SCHEDULE_TYPE  type, CString& str)
{
	BOOL bResult=FALSE;
	CTG_ScheduleInfo schdinfo;
	if(type==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC)
	{
		bResult=TG_SaveToFiles(&m_cBasicSchduleData);
		m_cBasicSchduleData.GetScheduleInfo(schdinfo);
		str=TG_GetScheduleBaseFilePath(schdinfo);
	}
	if(type==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_PREV)
	{	
		bResult=TG_SaveToFiles(&m_cBasicPrevSchduleData);
		m_cBasicPrevSchduleData.GetScheduleInfo(schdinfo);
		str=TG_GetScheduleBaseFilePath(schdinfo);
	}
	if(type==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_BACKUP)
	{
		bResult=TG_SaveToFiles(&m_cBasicBackupSchduleData);
		m_cBasicBackupSchduleData.GetScheduleInfo(schdinfo);
		str=TG_GetScheduleBaseFilePath(schdinfo);
	}
	
	return bResult;
}

void  CWorkScheduleHolder::SetUserID(int id)
{
	m_cBasicSchduleData.SetUserSpeical(id);
	m_cBasicPrevSchduleData.SetUserSpeical(id);
}

BOOL CWorkScheduleHolder::ReloadData(CTG_ScheduleInfo::SCHEDULE_TYPE  tp,BOOL delfirst)  // 重新获取
{
	if(tp==CTG_ScheduleInfo::SCHEDULE_TYPE_NULL)
	{
		if(delfirst)
			DeleteAllData();// 20090704
		
		InitCommInterfaceStatus(m_pMasterServerCommBasic,FALSE);
		if(m_pMasterServerCommBasic)
		{
			m_pMasterServerCommBasic->SetTrainDataInited(FALSE);
		}
		else
			return FALSE;
	}
	else
	{
		// 注意: 下面没有考虑 Database的情况，因为Database Comm 看起来没有什么前途
		CTG_ScheduleInfo info;
		if(tp==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC)
		{
			m_cBasicSchduleData.GetScheduleInfo(info);
			if(delfirst)
			{
				m_cBasicSchduleData.DeleteAllData(info,SYCFLAG_NULL);
				GpcLogTrainMessage(0, 0, "[deleteall] SCHEDULE_TYPE_BASIC 中数据");
			}
			ReadSchedule(&m_cBasicSchduleData,m_pMasterServerCommBasic);  // 读入基本图
		}
		else if(tp==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_PREV)
		{
			m_cBasicPrevSchduleData.GetScheduleInfo(info);
			if(delfirst)
			{
				m_cBasicPrevSchduleData.DeleteAllData(info,SYCFLAG_NULL);
				GpcLogTrainMessage(0, 0, "[deleteall] SCHEDULE_TYPE_BASIC_PREV 中数据");
			}
			ReadSchedule(&m_cBasicPrevSchduleData,m_pMasterServerCommBasicPrev);  // 读入基本图

		}
		else if(tp==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_BACKUP)
		{
			m_cBasicBackupSchduleData.GetScheduleInfo(info);
			if(delfirst)
			{
				m_cBasicBackupSchduleData.DeleteAllData(info,SYCFLAG_NULL);
				GpcLogTrainMessage(0, 0, "[deleteall] SCHEDULE_TYPE_BASIC_BACKUP 中数据");
			}
			ReadSchedule(&m_cBasicBackupSchduleData,m_pMasterServerCommBasicBackup);  // 读入基本图
		}
	}
	return TRUE;
}

BOOL  CWorkScheduleHolder::RegisterCommInterface(CTG_ScheduleInfo::SCHEDULE_TYPE sch_type,IScheduleComm *pcomm, bool bMMI)
{
	int i;
	for(i=0;i<MAX_COMM_INTF_NUM;i++)
	{
		if(m_aCommArray[i].pCommInterface==NULL)
			break;
	}
	if(i>=MAX_COMM_INTF_NUM)
		return FALSE;

	 m_aCommArray[i].pCommInterface=pcomm;
	 m_aCommArray[i].nSchType=sch_type;
	 pcomm->m_bForMMI=bMMI;
	 
	 if(sch_type==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC)
	 {
		 m_cBasicSchduleData.SetNeedSyncFlag(i,TRUE);
		 pcomm->SetDataStore(&m_cBasicSchduleData);
	 }
	 else
	 if(sch_type==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_PREV)
	 {
		 m_cBasicPrevSchduleData.SetNeedSyncFlag(i,TRUE);
		 pcomm->SetDataStore(&m_cBasicPrevSchduleData);
	 }
	 else
	 if(sch_type==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_BACKUP)
	 {
		 m_cBasicBackupSchduleData.SetNeedSyncFlag(i,TRUE);
		 pcomm->SetDataStore(&m_cBasicBackupSchduleData);
	 }
	 
	 if(pcomm->GetInterfaceFlag()==SYCFLAG_NULL)
		pcomm->SetInterfaceFlag(i); 
	 return TRUE;
}

BOOL  CWorkScheduleHolder::UnRegisterCommInterface(IScheduleComm *pcomm)
{
	int i;
	for(i=0;i<MAX_COMM_INTF_NUM;i++)
	{
		if(m_aCommArray[i].pCommInterface && m_aCommArray[i].pCommInterface==pcomm)
		{
			m_aCommArray[i].pCommInterface=NULL;
			if(m_aCommArray[i].nSchType==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC)
			{
				 pcomm->ClearDataStore(&m_cBasicSchduleData);
				 m_cBasicSchduleData.SetNeedSyncFlag(i,FALSE);
			}
			else
			if(m_aCommArray[i].nSchType==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_PREV)
			{
				 pcomm->ClearDataStore(&m_cBasicPrevSchduleData);
				 m_cBasicPrevSchduleData.SetNeedSyncFlag(i,FALSE);
			}
			else
			if(m_aCommArray[i].nSchType==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_BACKUP)
			{
				pcomm->ClearDataStore(&m_cBasicBackupSchduleData);
				m_cBasicBackupSchduleData.SetNeedSyncFlag(i,FALSE);
			}
			else
			if(m_aCommArray[i].nSchType==CTG_ScheduleInfo::SCHEDULE_TYPE_NULL)
			{
				pcomm->ClearDataStore(&m_cBasicSchduleData);
				pcomm->ClearDataStore(&m_cBasicPrevSchduleData);
				pcomm->ClearDataStore(&m_cBasicBackupSchduleData);
				m_cBasicSchduleData.SetNeedSyncFlag(i,FALSE);
				m_cBasicPrevSchduleData.SetNeedSyncFlag(i,FALSE);
				m_cBasicBackupSchduleData.SetNeedSyncFlag(i,FALSE);
			}
	 		m_aCommArray[i].nSchType=CTG_ScheduleInfo::SCHEDULE_TYPE_NULL;
		}
	}
	return TRUE;
}



void  CWorkScheduleHolder::DeleteAllData()
{
	CTG_ScheduleInfo info;
	m_cBasicSchduleData.GetScheduleInfo(info);
	m_cBasicSchduleData.DeleteAllData(info,SYCFLAG_NULL);
	
	m_cBasicPrevSchduleData.GetScheduleInfo(info);
	m_cBasicPrevSchduleData.DeleteAllData(info,SYCFLAG_NULL);
	m_cBasicBackupSchduleData.GetScheduleInfo(info);
	m_cBasicBackupSchduleData.DeleteAllData(info,SYCFLAG_NULL);
	GpcLogTrainMessage(0, 0, "[deleteall] all schd_type 中数据");
}

BOOL  CWorkScheduleHolder::ReadSchedule(CTG_TrainScheduleBase *pData,IScheduleComm *pcomm)
{
	 if(!pcomm)
		 return FALSE;
	 CTG_ScheduleInfo info;
	 pData->GetScheduleInfo(info);
	 if(info.m_nType!=CTG_ScheduleInfo::SCHEDULE_TYPE_NULL) 
		return pcomm->CopyAllData(info,pData,TRUE);
	 else
		 return FALSE;
}

BOOL  CWorkScheduleHolder::WriteSchedule(CTG_TrainScheduleBase *pData,IScheduleComm *pcomm,BOOL allwrite)
{
	 if(!pcomm || !pData)
		 return FALSE;

	 CTG_ScheduleInfo info;
	 pData->GetScheduleInfo(info);
	 return pData->CopyAllData(info,pcomm,allwrite);
}

BOOL  CWorkScheduleHolder::WriteSchedule(CTG_ScheduleInfo::SCHEDULE_TYPE  type,IScheduleComm *pcomm,BOOL allwrite)
{
	 if(type== CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC) 
		 return WriteSchedule(&m_cBasicSchduleData,pcomm,allwrite);
	 else if(type== CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_PREV) 
		 return WriteSchedule(&m_cBasicPrevSchduleData,pcomm,allwrite);
	 else if(type== CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_BACKUP)
		 return WriteSchedule(&m_cBasicBackupSchduleData,pcomm,allwrite);
	 else
	 {
		 WriteSchedule(&m_cBasicBackupSchduleData,pcomm,allwrite);
		 WriteSchedule(&m_cBasicPrevSchduleData,pcomm,allwrite);
		 return WriteSchedule(&m_cBasicSchduleData,pcomm,allwrite);
	 }
}

IScheduleComm  *CWorkScheduleHolder::GetServerCommByScheduleType(CTG_ScheduleInfo::SCHEDULE_TYPE schd_type)
{
	if(schd_type==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC)
		return m_pMasterServerCommBasic;
	else
	if(schd_type==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_PREV)
		return m_pMasterServerCommBasicPrev;
	else
	if(schd_type==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_BACKUP)
		return m_pMasterServerCommBasicBackup;
	else
		return m_pMasterServerCommBasic;

}
void  CWorkScheduleHolder::PostCommStatus(IScheduleComm *pcomm,BOOL active)
{
	if(pcomm==m_pMasterServerCommBasic)
	{
		if(active)
		{
			ReadSchedule(&m_cBasicSchduleData,pcomm);
			ReqBasicScheduleName();
		}
	}
	else
	if(pcomm==m_pMasterServerCommBasicPrev)
	{
		if(active)
				ReadSchedule(&m_cBasicPrevSchduleData,pcomm);
	}
	return;
}
void  CWorkScheduleHolder::InitCommInterfaceStatus(IScheduleComm *pcomm,BOOL va)
{
	if(!pcomm)
		return;
	int i=pcomm->GetInterfaceFlag();
	if(i>=0 && i<MAX_COMM_INTF_NUM)
	{
		if(m_aCommArray[i].pCommInterface==pcomm)
			m_aCommArray[i].bActive=va;
	}
}

// 2013.05.07处理运行图与Commsvr闪断
void CWorkScheduleHolder::RefreshCommStatus()
{
	for(int i=0;i<MAX_COMM_INTF_NUM;i++)
	{
		if(m_aCommArray[i].pCommInterface)
		{
			m_aCommArray[i].pCommInterface->PollingAndCheckStatus(); //检查连接状态
			PostCommStatus(m_aCommArray[i].pCommInterface,FALSE);
			m_aCommArray[i].bActive=FALSE;
		}
	}
}

BOOL  CWorkScheduleHolder::PollingData(TIME curtm)
{
	m_cBasicSchduleData.PollingForUpdate(TRUE,curtm);
	m_cBasicPrevSchduleData.PollingForUpdate(TRUE,curtm);
    for(int i=0;i<MAX_COMM_INTF_NUM;i++)
	{
		if(m_aCommArray[i].pCommInterface)
		{
			m_aCommArray[i].pCommInterface->PollingAndCheckStatus(); //检查连接状态
			if(m_aCommArray[i].pCommInterface->CommReady() && !m_aCommArray[i].bActive)  // 状态变活
			{
				PostCommStatus(m_aCommArray[i].pCommInterface,TRUE);
				m_aCommArray[i].bActive=TRUE;
			}
			else
			if(!m_aCommArray[i].pCommInterface->CommReady() && m_aCommArray[i].bActive)  // 状态变死
			{
				PostCommStatus(m_aCommArray[i].pCommInterface,FALSE);
				m_aCommArray[i].bActive=FALSE;
			}
		}
	}

	for(int i=0;i<MAX_COMM_INTF_NUM;i++)
	{
		if(m_aCommArray[i].pCommInterface)
		{
			if(m_aCommArray[i].pCommInterface->CommReady())
			{
				if(m_aCommArray[i].nSchType==CTG_ScheduleInfo::SCHEDULE_TYPE_NULL)
				{
					PollingForUpdate(&m_cBasicSchduleData,m_aCommArray[i].pCommInterface,FALSE);
					PollingForUpdate(&m_cBasicPrevSchduleData,m_aCommArray[i].pCommInterface,FALSE);
				}
				else
				if(m_aCommArray[i].nSchType==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC)
				{
					PollingForUpdate(&m_cBasicSchduleData,m_aCommArray[i].pCommInterface,FALSE);
				}
				else
				if(m_aCommArray[i].nSchType==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_PREV)
				{
					PollingForUpdate(&m_cBasicPrevSchduleData,m_aCommArray[i].pCommInterface,FALSE);
				}
				else
				if(m_aCommArray[i].nSchType==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_BACKUP)
				{
					PollingForUpdate(&m_cBasicBackupSchduleData,m_aCommArray[i].pCommInterface,FALSE);
				}
			}
		}
	}
	return TRUE;
}

BOOL   CWorkScheduleHolder::PollingForUpdate(CTG_TrainScheduleBase *pData,IScheduleComm *pcomm,BOOL all_flag)
{
	CTG_SingleTrain *ptrain;
	CTG_SingleMark  *pmark;
	BOOL rc=FALSE;
	int sync_flag;
	if(!pcomm || !pData)
		return FALSE;

	int nCountOnceSend=99999;
	if((all_flag==FALSE) && pcomm->m_bForMMI) // 如果是往界面同步每次至多10个
	{
		all_flag=TRUE;
		nCountOnceSend=20;
	}

	sync_flag=pcomm->GetInterfaceFlag();
	do
	{
		CTG_ScheduleInfo schd;
		CTG_SingleTrain  srctrain;
		CTG_SingleMark   srcmark;
		BOOL   bSyncComm=pcomm->IsAsyncMode();
		pData->Lock(); 
		ptrain=pData->Find1NoSyncTrain(&schd,sync_flag,!bSyncComm); //对于异步COMM,不查找正在等待应答的对象
		pmark=pData->Find1NoSyncMark(&schd,sync_flag,!bSyncComm);
		if(ptrain)
		{
			srctrain=*ptrain;
			if(pcomm->IsAsyncMode()) // 异步模式
				ptrain->StartSyncing(sync_flag);
			else
				ptrain->SetSyncStatus(sync_flag,TRUE); 
		}
		
		if(pmark)
		{
			srcmark=*pmark;
			if(pcomm->IsAsyncMode()) // 异步模式		
				pmark->StartSyncing(sync_flag);
			else
				pmark->SetSyncStatus(sync_flag,TRUE); 
		}
		pData->UnLock(); 

		if(ptrain)
		{

			if(srctrain.IsDeletedObject())
			{
				pcomm->DeleteTrain(schd,srctrain,SYCFLAG_MEMORY);
			}
			else
			{
				pcomm->UpdateTrain(schd,srctrain,SYCFLAG_MEMORY);
			}

			rc=TRUE;
		}

		if(pmark)
		{
			if(srcmark.IsDeletedObject())
				pcomm->DeleteMark(schd,srcmark,SYCFLAG_MEMORY);
			else
				pcomm->UpdateMark(schd,srcmark,SYCFLAG_MEMORY);
			rc=TRUE;
		}
		if(!ptrain && !pmark)
		{
			break;
		}
		else 
		{
			//GpcLogTrainMessage(0, 0, "同步更新[%d] nCountOnceSend %ld ", schd.m_nType, nCountOnceSend);
			nCountOnceSend--;
			if(nCountOnceSend<0)
			{
				break;
			}
		}
	}while(all_flag);
	return rc;
}

BOOL  CWorkScheduleHolder::GenTrain_FreeDraw(CTG_SingleTrain &srctrain,const CTG_ScheduleInfo &schd,BOOL qualify_runtime,BYTE run_type) // train中是界面生成的初始数据
{
	CTG_TrainScheduleBase *pCurData;
	CTG_TrainScheduleBase *pBasicData;
	pBasicData=&m_cBasicSchduleData;
	if(schd.m_nType==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_PREV)   
		pCurData=&m_cBasicPrevSchduleData;
	else
	if(schd.m_nType==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_BACKUP)   
		pCurData=&m_cBasicBackupSchduleData;
	else
	if(schd.m_nType==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC)   // 基本图车次
	{
		pCurData=&m_cBasicSchduleData;
		pBasicData=NULL;
	}
	else
		return FALSE;
	CTGAutoAdjustLogic adjust_logic(GetTGFunctionConfig(),GetTGBasicParam(),pBasicData,pCurData);
    CTGAddTrainLogic  logic(pBasicData,GetTGBasicParam(),&adjust_logic,GetTGFunctionConfig(),&schd);
	if(!logic.GenTrain_FreeDraw(&srctrain,qualify_runtime,run_type))
		return FALSE;
	
	return TRUE;
}

bool CWorkScheduleHolder::TGOCXSetTrainExitEntry(CTG_SingleTrain& train, CTgX* pTG)
{
	GpcLogTrainMessage(0, 0, "TGOCXSetTrainExitEntry 画图控件开始计算出入口");
	if(pTG==NULL)
	{
		GpcLogTrainMessage(0, 0, "控件指针为空");
		return false;
	}
	
	// 保存首站接入口、未站交出口
	CTG_TrainRecord *pfirst_rec=train.GetTrainRecord(0);
	CTG_TrainRecord *plast_rec=train.GetTrainRecord(train.GetTrainRecordCount()-1);
	if(pfirst_rec==NULL || plast_rec==NULL)
	{
		GpcLogTrainMessage(0, 0, "列车记录异常");
		return false;
	}
	int entry=pfirst_rec->GetEntry();
	int	exit=plast_rec->GetExit();

	bool bResult = TGOCXSetTrainExitEntry(train, pTG, entry, exit);
	return bResult;
}

BOOL  CWorkScheduleHolder::GenTrainBasicSchedule_OPMS(CTG_SingleTrain &srctrain,CTG_SingleTrain &othertrain,const CTG_ScheduleInfo &schd)
{
	CTG_TrainScheduleBase *pCurData;
	CTG_TrainScheduleBase *pBasicData;
	pBasicData=&m_cBasicSchduleData;
	if(schd.m_nType==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC)
	{
		pCurData=&m_cBasicSchduleData;
		pBasicData=NULL;
	}
	else if(schd.m_nType==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_BACKUP)
	{
		pCurData=&m_cBasicBackupSchduleData;
		pBasicData=NULL;
	}
	else
	if(schd.m_nType==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_PREV)
		pCurData=&m_cBasicPrevSchduleData;
	else
		return FALSE;
	CTGAutoAdjustLogic adjust_logic(GetTGFunctionConfig(),GetTGBasicParam(),pBasicData,pCurData);
    CTGAddTrainLogic  logic(pBasicData,GetTGBasicParam(),&adjust_logic,GetTGFunctionConfig(),&schd);
	if(!logic.GenTrainBasic_OPMS(&srctrain))
		return FALSE;
	srctrain.SetTrainIndex(srctrain.GetShiftTrainIndex()); 
	for(int i=0;i<srctrain.GetTrainRecordCount()-1;i++)
	{
		CTG_TrainRecord *prec=srctrain.GetTrainRecord(i);
		CTG_TrainRecord *nextprec=srctrain.GetTrainRecord(i+1);
		int station1next=-1;
		int station2pre=-1;
		int first_entry=-1;
		int first_exit=-1;
		int second_entry=-1;
		int second_exit=-1;
		if(GetTGFunctionConfig()->MatchSplitTrain(schd.m_nSectionID,prec->GetStationNo(),nextprec->GetStationNo(),&station1next,&station2pre,&first_entry,&first_exit,&second_entry,&second_exit))
		{
			//yuzhu begin 折返列车分成两列
			int absnum=GetTGBasicParam()->GetAbsNumberBetweenStations(prec->GetStationNo(),station1next,srctrain.IsDownDirection()?1:0);
			if(absnum>0)
				prec->m_data.exit=GetEntryByTmisAbsNumber(prec->GetStationNo(),absnum);

			if(station1next == station2pre)
			{
				absnum=GetTGBasicParam()->GetAbsNumberBetweenStations(station2pre,nextprec->GetStationNo(),!srctrain.IsDownDirection()?1:0);
				if(absnum>0)
					nextprec->m_data.entry=GetEntryByTmisAbsNumber(nextprec->GetStationNo(),absnum);
			}
			else
			{
				absnum=GetTGBasicParam()->GetAbsNumberBetweenStations(station2pre,nextprec->GetStationNo(),srctrain.IsDownDirection()?1:0);
				if(absnum>0)
					nextprec->m_data.entry=GetEntryByTmisAbsNumber(nextprec->GetStationNo(),absnum);
			}
			//yuzhu end
			 logic.SplitTrain(&srctrain,&othertrain,i+1,BASIC,first_entry,first_exit,second_entry,second_exit);
			 break;
		}
	}

	othertrain.SetShiftTrainIndex(othertrain.GetShiftTrainIndex() | 0x40000000L); //20141204 车次分割 30bit为1,解决南宁全局ID分割重复
	othertrain.SetTrainIndex(othertrain.GetShiftTrainIndex());
	if(othertrain.GetTrainRecordCount()>0)
	{
		TRAIN_INDEX tidx1=srctrain.GetShiftTrainIndex();
		TRAIN_INDEX tidx2=othertrain.GetShiftTrainIndex();
		if(tidx1==tidx2)
		{
			GpcLogTrainMessage(0, 0, "基本图车次分割, 丢失运行线 %s, 由于OPMS全局ID %d 的bit30位为1", srctrain.GetTrainID(), tidx1);
		}
	}

	return TRUE;
}

BOOL  CWorkScheduleHolder::GenTrain_AutoAdjustManualUpdate(CTG_SingleTrain &srctrain,const CTG_ScheduleInfo &schd,const CMMIOperFlag &oper_flag,CWnd* pParentWnd)
{
	CTG_TrainScheduleBase *pCurData;
	CTG_TrainScheduleBase *pBasicData;
	pBasicData=&m_cBasicSchduleData;
	if(schd.m_nType==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_PREV)   
		pCurData=&m_cBasicPrevSchduleData;
	else
		return FALSE;

	CTGAutoAdjustLogic adjust_logic(GetTGFunctionConfig(),GetTGBasicParam(),pBasicData,pCurData);
	if(!adjust_logic.AdjustTrainForManualUpdate(&srctrain,oper_flag,schd.m_nSectionID,pParentWnd))
		return FALSE;

	if(schd.m_nType==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_PREV ||
		schd.m_nType==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_BACKUP)
	{
		int offset=0;
		CTG_TrainRecord *rec=srctrain.GetTrainRecord(oper_flag.m_nRecIndex);
		CTG_TrainRecord *pnext=srctrain.GetTrainRecord(oper_flag.m_nRecIndex+1);
		if(rec!=NULL && pnext!=NULL)
			offset=rec->m_data.depart-pnext->m_data.arrive;

		if(offset>0)
		{
			offset+=60;
			for(int i=oper_flag.m_nRecIndex+1;i<srctrain.GetTrainRecordCount();i++)
			{
				CTG_TrainRecord *prec=srctrain.GetTrainRecord(i);
				if(!prec)
					continue; 
				prec->m_data.arrive+=offset;
				prec->m_data.depart+=offset;
			}
		}
	}

	if(oper_flag.nFlag & CMMIOperFlag::CONTINUE_DRAW_TRAIN)  // 继续画线
	{
		if(oper_flag.m_nRecIndex>=0) // 防止从第一个点继续画线后初始化不正确 2014.4.30
		{
			CTGAddTrainLogic  logic(pBasicData,GetTGBasicParam(),&adjust_logic,GetTGFunctionConfig(),&schd);
			logic.SetDefaultAttribForNewRecords(&srctrain,oper_flag.m_nRecIndex);
			CTGAddTrainLogic::AutoChangeTrainID(&srctrain, schd.m_nSectionID);
		} 
	}
	if(oper_flag.nFlag & CMMIOperFlag::ENGINE_INFO_CHANGED)  // 如果是机车信息变化则设置电力
	{
		srctrain.SetElecFlagByEngineInfo(); 
	}

	CTG_SingleTrain *pold=pCurData->GetSingleTrain(srctrain.GetTrainIndex());
	if(NULL != pold)
	{	
		// 2014.05.27 沈阳 区间时分类型
		bool enable=GetTGBasicParam()->IsEnableABSTimeType();
		if(oper_flag.nFlag & CMMIOperFlag::TRAIN_ATTRIB_CHANGED)
		{
			if(enable)
			{
				BYTE nOldAbsTimeType=GetTrainAbsTimeType(pold);
				BYTE nNewAbsTimeType=GetTrainAbsTimeType(&srctrain);
				
				if(nNewAbsTimeType!=nOldAbsTimeType)
				{
					if(SetNewTrainAbsRunTime(srctrain,oper_flag.m_nRecIndex,nNewAbsTimeType))
					{
						SetTrainAbsTimeType(&srctrain, nNewAbsTimeType);
						GpcLogTrainMessage(0,0,"调度员修改属性窗口设定 train:%s 使用区间时分类型 %d 重新计算区间时分", srctrain.GetTrainID(), nNewAbsTimeType);
					}
					else
					{
						SetTrainAbsTimeType(&srctrain, nOldAbsTimeType);
					}
				}
			}
		}

		if(srctrain.GetTrainType()!=pold->GetTrainType())  // 类型改变 20140918
		{
			BYTE run_type=0,prio=0,locotype=0;
			BOOL elec=TRUE;
			if(GetTGBasicParam()->GetTrainTypeDef(srctrain.GetTrainType(),&run_type,&prio,&locotype))
			{
				if(!enable)
				{
					if(oper_flag.m_nStation==0) // 0：修改run_type 1：不修改
						srctrain.GetTrainInfo()->m_data.run_type= run_type;
					srctrain.GetTrainInfo()->m_data.priority = prio;
				}
				srctrain.SetDefaultAttribByDefinedFlag(locotype);
			}
		}

		if(oper_flag.nFlag & CMMIOperFlag::TRAIN_ATTRIB_CHANGED)
		{
			// 仅处理OCX设置超限标志
			if(srctrain.GetTrainInfo()->m_data.space_rank!=pold->GetTrainInfo()->m_data.space_rank)
				srctrain.SetSpaceRank(srctrain.GetTrainInfo()->m_data.space_rank, 0, true, true); // 设置记录的超限标志 
		}
	}

	return TRUE;
}


BOOL  CWorkScheduleHolder::IsSyncedWithServer(CTG_ScheduleInfo::SCHEDULE_TYPE  schd_type,const CTG_LogicObject *pobj)
{
	IScheduleComm *pcomm=GetServerCommByScheduleType(schd_type);
	if(pcomm)
		return pobj->CheckSyncStatus(pcomm->GetInterfaceFlag());
	else
		return FALSE;
}

int   CWorkScheduleHolder::GetNoSyncedObjectCount(CTG_ScheduleInfo::SCHEDULE_TYPE  type,IScheduleComm *pcomm,CString &Info)
{
	if(!pcomm)
		return -1;
	
	if(type==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC)
		return (m_cBasicSchduleData.GetNoSyncTrainCount(pcomm->GetInterfaceFlag(),Info)
		+m_cBasicSchduleData.GetNoSyncMarkCount(pcomm->GetInterfaceFlag(),Info));
	else
	if(type==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_PREV)
		return (m_cBasicPrevSchduleData.GetNoSyncTrainCount(pcomm->GetInterfaceFlag(),Info)
		+m_cBasicPrevSchduleData.GetNoSyncMarkCount(pcomm->GetInterfaceFlag(),Info));
	else
	if(type==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_BACKUP)
		return (m_cBasicBackupSchduleData.GetNoSyncTrainCount(pcomm->GetInterfaceFlag(),Info)
		+m_cBasicBackupSchduleData.GetNoSyncMarkCount(pcomm->GetInterfaceFlag(),Info));
	else
		return -2;
}

int   CWorkScheduleHolder::GetServerNoSyncedObjectCount(CTG_ScheduleInfo::SCHEDULE_TYPE  type, CString &Info)
{
	IScheduleComm *pcomm;
	pcomm=GetServerCommByScheduleType(type);
	if(pcomm)
		return GetNoSyncedObjectCount(type,pcomm,Info);
	else
		return -2;
}

BOOL  CWorkScheduleHolder::GenMark_BasicSchedule(CTG_SingleMark &mark,const CTG_ScheduleInfo &schd,TRAIN_INDEX index)
{
		CTG_SingleMark *pshift_mark=m_cBasicSchduleData.GetSingleMark(index);
		if(!pshift_mark)
			return FALSE;
		mark=*pshift_mark;
		return TRUE;
}

void CWorkScheduleHolder::SetFileSaveFlag()
{
	m_cBasicSchduleData.SetFileSaveFlag(TRUE);
	m_cBasicPrevSchduleData.SetFileSaveFlag(FALSE); // 无需保存
}
void CWorkScheduleHolder::SetQueryMode(BOOL va)
{
	m_bQueryMode=va;
}

// 获取车站控制模式,下发阶段计划检查使用
bool CWorkScheduleHolder::IsNeedCheckCtcModePlan(int station)
{
	bool bret = false;
	TG_STATION_STATUS * pst = GetStationStatusPtr(station);
	if(NULL != pst)
	{
		switch(pst->ctrl_mode)
		{
		case SMODE_DISTRIBUTED: // 分散自律
		case SMODE_CENTER_CTRL: // 中心完全控制
			bret = true;
			break;
		case SMODE_STATION_CTRL: // 车站完全控制
			bret = GetTGFunctionConfig()->IsCheckCtcModeStaCtrlStPlan();
			break;
		case SMODE_EMERGENCY: // 非常站控		
			break;
		default:
			;
		}
	}

	return bret;
}

int  CWorkScheduleHolder::IsCheckStationSide(int station)
{
	if(GetTGFunctionConfig()->IsCheckAllStationStPlan())
		return true;
	
	// 只检查CTC车站
	if(gpcdef.IsCtcStation(station))
		return IsNeedCheckCtcModePlan(station);
	else
		return false;
}

void CWorkScheduleHolder::ReqBasicScheduleName()
{
	int nSectionList[64];
	int nSectionNum=sizeof(nSectionList)/sizeof(nSectionList[0]);
	nSectionNum=gpcdef.GetSectionList(nSectionNum, nSectionList);
	for(int ns=0; ns<nSectionNum; ns++)
	{
		ReqBasicName_ToServer(nSectionList[ns], true);
	}
}

int CWorkScheduleHolder::CalcTrainLastSt_NextStTime(CTG_SingleTrain &train)
{
	int tmarrive=0;
	// 货物列车,记录上一段推算的下一站时间
	CTG_TrainRecord *pLast=train.GetTrainLastRecord();
	if(pLast)
	{
		tmarrive=pLast->m_data.depart;

		STATION_NO station=pLast->GetStationNo();
		ENTRY_ID entry=pLast->GetEntryNumber(TRAIN_OUT_STATION);
		BYTE  trn_type=train.GetRunType();

		tmarrive +=GetTGBasicParam()->CalculateTrainRunTime(station, entry, TRAIN_OUT_STATION, trn_type);
	}

	return tmarrive;
}

// 区间运行时分推算时,首站时间不能为零,该函数试图设置首站时间
void CWorkScheduleHolder::SetTrainFirstStationZeroTMToArriveTm(CTG_SingleTrain &train, int arrivetm)
{
	CTG_TrainRecord *pFirst=train.GetTrainFirstRecord();
	if(pFirst && arrivetm>0)
	{
		if(pFirst->m_data.arrive==0 || pFirst->m_data.depart==0)
		{
			pFirst->m_data.arrive=arrivetm;
			if(pFirst->m_data.min_stop_time>=0)
				pFirst->m_data.depart=pFirst->m_data.arrive+pFirst->m_data.min_stop_time;
			else
				pFirst->m_data.depart=pFirst->m_data.arrive;
		}
	}
}

BOOL CWorkScheduleHolder::GenTrainBasicSchedule_OPMSx(CTG_SingleTrain &srctrain, std::vector<CTG_SingleTrain>& train_list,const CTG_ScheduleInfo &schd, bool& bOldSplitTrain)
{
	train_list.clear();
	int station_list[NEW_SPLIT_ST_NUM], entry_list[NEW_SPLIT_ST_NUM];
	int nStationCount=GetTGFunctionConfig()->GetNewSplitStation(schd.m_nSectionID, station_list, entry_list, NEW_SPLIT_ST_NUM);
	if(nStationCount<=0)
	{
		CTG_SingleTrain othertrain;
		if(FALSE==GenTrainBasicSchedule_OPMS(srctrain, othertrain, schd))
		{
			return FALSE;
		}
		
		if(othertrain.GetTrainRecordCount()>0)
		{
			train_list.push_back(othertrain);
		}
		bOldSplitTrain=true;  // 老模式定义为[TD_SPLIT]生效的列车切割模式
	}
	else
	{
		bOldSplitTrain=false;  // 新模式定义为金丽温使用的[TD_SPLIT_EXTERN]生效的列车切割模式
		CTG_TrainScheduleBase *pCurData;
		CTG_TrainScheduleBase *pBasicData;
		pBasicData=&m_cBasicSchduleData;
		if(schd.m_nType==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC)
		{
			pCurData=&m_cBasicSchduleData;
			pBasicData=NULL;
		}
		else if(schd.m_nType==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_BACKUP)
		{
			pCurData=&m_cBasicBackupSchduleData;
			pBasicData=NULL;
		}
		else
		if(schd.m_nType==CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC_PREV)
			pCurData=&m_cBasicPrevSchduleData;
		else
			return FALSE;
		
		CTGAutoAdjustLogic adjust_logic(GetTGFunctionConfig(),GetTGBasicParam(),pBasicData,pCurData);
		CTGAddTrainLogic  logic(pBasicData,GetTGBasicParam(),&adjust_logic,GetTGFunctionConfig(),&schd);
		if(!logic.GenTrainBasic_OPMS(&srctrain))
			return FALSE;

		srctrain.SetTrainIndex(srctrain.GetShiftTrainIndex());
		TRAIN_INDEX shift_index=srctrain.GetShiftTrainIndex();

		CTG_SingleTrain othertrain;
		CTG_SingleTrain newtrain=srctrain;
		// 一次分割
		for(int m=1; m<nStationCount; m++)
		{
			for(int i=0;i<newtrain.GetTrainRecordCount()-1;i++)
			{
				CTG_TrainRecord *prec=newtrain.GetTrainRecord(i);
				CTG_TrainRecord *nextprec=newtrain.GetTrainRecord(i+1);

				if((prec!=NULL)&&(nextprec!=NULL)
					&&(prec->m_data.station==station_list[m-1]) 
					&& (nextprec->m_data.station==station_list[m]))
				{
					logic.SplitTrain(&newtrain,&othertrain,i+1,BASIC,-1,-1,-1,-1);
					srctrain=newtrain;

					if(entry_list[m-1]>0)
					{
						CTG_TrainRecord *plast=srctrain.GetTrainLastRecord();
						if(plast)
						{
							plast->m_data.exit=entry_list[m-1];
						}
					}
					srctrain.SetShiftTrainIndex(shift_index); //20141204 车次分割 30bit为1,解决南宁全局ID分割重复
					srctrain.SetTrainIndex(srctrain.GetShiftTrainIndex());

					if(entry_list[m]>0)
					{
						CTG_TrainRecord *pfirst=othertrain.GetTrainFirstRecord();
						if(pfirst)
						{
							pfirst->m_data.entry=entry_list[m];
						}
					}
					othertrain.SetShiftTrainIndex(shift_index | 0x40000000L); //20141204 车次分割 30bit为1,解决南宁全局ID分割重复
					othertrain.SetTrainIndex(othertrain.GetShiftTrainIndex());

					m=nStationCount;
					break;
				}
			}
		}

		// 二次分割
		newtrain=othertrain;
		for(int m=1; m<nStationCount; m++)
		{
			for(int i=0;i<newtrain.GetTrainRecordCount()-1;i++)
			{
				CTG_TrainRecord *prec=newtrain.GetTrainRecord(i);
				CTG_TrainRecord *nextprec=newtrain.GetTrainRecord(i+1);

				if((prec!=NULL)&&(nextprec!=NULL)
					&&(prec->m_data.station==station_list[m-1]) 
					&& (nextprec->m_data.station==station_list[m]))
				{
					logic.SplitTrain(&newtrain,&othertrain,i+1,BASIC,-1,-1,-1,-1);

					if(entry_list[m-1]>0)
					{
						CTG_TrainRecord *plast=newtrain.GetTrainLastRecord();
						if(plast)
						{
							plast->m_data.exit=entry_list[m-1];
						}
					}
					newtrain.SetShiftTrainIndex(shift_index | 0x40000000L); //20141204 车次分割 30bit为1,解决南宁全局ID分割重复
					newtrain.SetTrainIndex(newtrain.GetShiftTrainIndex());
					train_list.push_back(newtrain);

					if(entry_list[m]>0)
					{
						CTG_TrainRecord *pfirst=othertrain.GetTrainFirstRecord();
						if(pfirst)
						{
							pfirst->m_data.entry=entry_list[m];
						}
					}
					othertrain.SetShiftTrainIndex(shift_index | 0x80000000L); //20141204 车次分割 30bit为1,解决南宁全局ID分割重复
					othertrain.SetTrainIndex(othertrain.GetShiftTrainIndex());

					m=nStationCount;
					break;
				}
			}
		}

		if(othertrain.GetTrainRecordCount()>0)
		{
			train_list.push_back(othertrain);
		}
	}
	return TRUE;
}
bool  CWorkScheduleHolder::TGOCXSetTrainExitEntry(CTG_SingleTrain& train, CTgX* pTG, int first_entry, int last_exit)
{
	bool bSuccess=false;
	CString newxml=pTG->SendTrainMsgToOCX(train, CTgX::SEND_FLAG::Gen_ExitEntry); // 0615
	if(newxml!="")
	{
		TrainForExitEntry train_tmp;
		if(fromXML(train_tmp,newxml))
		{
			if(SetTrainExitEntry(train, train_tmp))
			{
				GpcLogTrainMessage(0, 0, "返回设置出入口成功");
				bSuccess = true;
			}
			else
			{
				GpcLogTrainMessage(0, 0, "返回设置出入口失败");
				bSuccess = false;
			}

			if (IS_VALID_ENTRY(first_entry))
			{
				CTG_TrainRecord *pfirst_rec=train.GetTrainRecord(0);
				if(pfirst_rec)
					pfirst_rec->m_data.entry=first_entry;
			}
			if (IS_VALID_ENTRY(last_exit))
			{
				CTG_TrainRecord *plast_rec=train.GetTrainRecord(train.GetTrainRecordCount()-1);
				if(plast_rec)
					plast_rec->m_data.exit=last_exit;
			}
		}
		else
		{
			GpcLogTrainMessage(0, 0, "[Error] OCX生成出入口成功,但是解析失败");
			bSuccess = false;
		}
	}
	else
	{
		GpcLogTrainMessage(0, 0, "[Error] OCX生成出入口失败");
		bSuccess = false;
	}

	return bSuccess;
}

void CWorkScheduleHolder::CheckTrainNo(CTG_SingleTrain& newtrain, CStringArray& train_alarm)
{
	for (int idx =0; idx< newtrain.GetTrainRecordCount(); idx++)
	{
		CTG_TrainRecord *rec = newtrain.GetTrainRecordByRecIndex(idx);
		CTG_TrainRecord *next = newtrain.GetTrainRecordByRecIndex(idx+1);
		if (NULL!=rec && NULL!=next)
		{
			if (strcmp(rec->GetDepartTID(), next->GetArriveTID()))
			{
				CString str1=gpcdef.GetStationName(rec->m_data.station);
				CString str2=gpcdef.GetStationName(next->m_data.station);
				CString temp;
				temp.Format("%s 出发车次 %s 和 %s 到达车次 %s 不同,请确认",
					str1, rec->GetDepartTID(), str2, next->GetArriveTID());
				train_alarm.Add(temp);
			}
		}
	}
}
