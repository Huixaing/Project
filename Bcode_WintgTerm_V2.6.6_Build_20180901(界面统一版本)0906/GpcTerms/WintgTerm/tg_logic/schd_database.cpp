#include "..\basemsg\clbth.h"
#include "tg_schedule_base.h"
#include "schd_database.h"
#include "../database/caddataserver_schd.h"
#include "tg_datacopy.h"
#include "db_msgproc.h"
#include "../database/db_util.h"

void   CSchduleDatabaseInterface::PollingAndCheckStatus()
{
	if(m_pDatabaseServer)
		SetCommReady(TRUE);
	else
		SetCommReady(FALSE);
}

BOOL   CSchduleDatabaseInterface::InitDatabase(const char *db_name,int db_type)
{
	if(db_name)
		m_pDatabaseServer=GetDatabaseByName(db_name);
	else
		m_pDatabaseServer=GetDatabaseByType(db_type);
	if(m_pDatabaseServer)
	{
		return TRUE;
	}
	else
		return FALSE;
}

BOOL   CSchduleDatabaseInterface::UpdateTrain(const CTG_ScheduleInfo & schd,const CTG_SingleTrain  &srctrain,int source_flag)
{
	CWorkScheduleDataServer *pDb_svr=m_pDatabaseServer;
	if(!pDb_svr)
		return FALSE;
	int con_id=schd.m_nSectionID; 
	CADDATA_SINGLE_TRAIN train;

	if(srctrain.IsDeletedObject())
	{
		pDb_svr->Update_DeleteSingleTrainFromWorkSchedule(con_id,srctrain.GetTrainIndex());
		return TRUE;
	}


	TG_CopyData(&train,&srctrain);  // 复制数据


	if(train.m_nTrainIndex==0)
	{
		train.m_nTrainIndex=pDb_svr->GetAvaTrainIndex(con_id); 
	}

	train.m_nTrainInfo.train_index=train.m_nTrainIndex; 
	
	train.m_nSyncFlag = srctrain.GetSyncStatus();

	
	pDb_svr->Update_DeleteSingleTrainFromWorkSchedule(con_id,train.m_nTrainIndex); // 删除

	pDb_svr->Update_InsertSingleTrainToWorkSchedule(con_id,&train); // 插入

	return TRUE;
}


BOOL   CSchduleDatabaseInterface::UpdateTrainInfo(const CTG_ScheduleInfo &schd,const CTG_SingleTrain  &srctrain,int source_flag)
{
	CWorkScheduleDataServer *pDb_svr=m_pDatabaseServer;
	if(!pDb_svr)
		return FALSE;
	if(srctrain.IsDeletedObject())
		return FALSE;

	CADDATA_SINGLE_TRAIN train;
	int con_id=schd.m_nSectionID; 

	TG_CopyData(&train,&srctrain);  // 复制数据
	pDb_svr->Update_UpdateTrainInfo(con_id,&train);
	return TRUE;
}

BOOL   CSchduleDatabaseInterface::UpdateTrainRecords(const CTG_ScheduleInfo &schd,const CTG_SingleTrain  &srctrain,int source_flag)
{
	CWorkScheduleDataServer *pDb_svr=m_pDatabaseServer;
	if(!pDb_svr)
		return FALSE;
	if(srctrain.IsDeletedObject())
		return FALSE;

	CADDATA_SINGLE_TRAIN train;
	int con_id=schd.m_nSectionID; 

	TG_CopyData(&train,&srctrain);  // 复制数据
	pDb_svr->Update_UpdateTrainRecord(con_id,&train,FALSE); // replace
	return TRUE;
}

BOOL   CSchduleDatabaseInterface::UpdateEngineInfo(const CTG_ScheduleInfo &schd,const CTG_SingleTrain  &train,int source_flag)
{
	return UpdateTrainInfo(schd,train,source_flag);
}

BOOL   CSchduleDatabaseInterface::UpdateMark(const CTG_ScheduleInfo &schd,TRAIN_INDEX block_index,const CTG_SingleMark  &block_info,int source_flag)
{
	return 	UpdateMark(schd,block_info,source_flag); 
	
}

BOOL   CSchduleDatabaseInterface::UpdateMark(const CTG_ScheduleInfo &schd,const CTG_SingleMark  &block_info,int source_flag)
{
    CWorkScheduleDataServer *pDb_svr=m_pDatabaseServer;
	if(!pDb_svr)
		return FALSE;
	int con_id=schd.m_nSectionID; 
	if(block_info.IsDeletedObject())
	{
		pDb_svr->Update_DeleteBlockMark(con_id,block_info.m_nBlockIndex,FALSE);
		return TRUE;
	}

	CADDATA_ABS_MARK mark_item;

	TG_CopyData(&mark_item,&block_info);

	if(mark_item.m_data.mark_index==0)
		mark_item.m_data.mark_index=pDb_svr->GetAvaTrainIndex(con_id); 
	else
		pDb_svr->Update_DeleteBlockMark(con_id,mark_item.m_data.mark_index ,FALSE);

	pDb_svr->Update_InsertBlockMark(con_id,&mark_item);

	return TRUE;
}

BOOL   CSchduleDatabaseInterface::DeleteTrain(const CTG_ScheduleInfo &schd,const CTG_SingleTrain  &srctrain,int source_flag)
{
	CWorkScheduleDataServer *pDb_svr=m_pDatabaseServer;
	if(!pDb_svr)
		return FALSE;
	int con_id=schd.m_nSectionID; 

	pDb_svr->Update_DeleteSingleTrainFromWorkSchedule(con_id,srctrain.GetTrainIndex());
	return TRUE;
}

BOOL   CSchduleDatabaseInterface::DeleteMark(const CTG_ScheduleInfo &schd,const CTG_SingleMark  &block_info,int source_flag)
{
	CWorkScheduleDataServer *pDb_svr=m_pDatabaseServer;
	if(!pDb_svr)
		return FALSE;
	int con_id=schd.m_nSectionID; 
	
	pDb_svr->Update_DeleteBlockMark(con_id,block_info.m_nBlockIndex,FALSE);
	return TRUE;
	
}

BOOL   CSchduleDatabaseInterface::DeleteAllData(const CTG_ScheduleInfo &schd,int source_flag)
{
	CWorkScheduleDataServer *pDb_svr=m_pDatabaseServer;
	if(!pDb_svr)
		return FALSE;
	pDb_svr->Update_DeleteWorkSchedule(schd.m_nSectionID);
	return TRUE;
}


TRAIN_INDEX   CSchduleDatabaseInterface::AppendTrain(const CTG_ScheduleInfo &schd,const CTG_SingleTrain  &srctrain,int source_flag)
{
	 UpdateTrain(schd,srctrain,source_flag);
	 return srctrain.GetTrainIndex();
}

TRAIN_INDEX   CSchduleDatabaseInterface::AppendMark(const CTG_ScheduleInfo &schd,const CTG_SingleMark  &mark,int source_flag)
{
	 UpdateMark(schd,mark,source_flag);
	 return mark.GetBlockIndex();
}

	//////  TrainIndex based function
BOOL   CSchduleDatabaseInterface::UpdateTrainRecords(const CTG_ScheduleInfo &schd,TRAIN_INDEX index,const CTG_TrainRecordList &trainrecord,int source_flag)
{
	CTG_SingleTrain srctrain;
	srctrain.SetTrainIndex(index); 
	srctrain.UpdateTrainRecords(trainrecord,source_flag);
	return UpdateTrainRecords(schd,srctrain,source_flag);
}

BOOL   CSchduleDatabaseInterface::UpdateEngineInfo(const CTG_ScheduleInfo &schd,TRAIN_INDEX index,const CTG_TrainEngineList &trainengine,int source_flag)
{
	CTG_SingleTrain srctrain;
	srctrain.SetTrainIndex(index); 
	srctrain.UpdateEngineInfo(trainengine,source_flag);
	return UpdateTrainInfo(schd,srctrain,source_flag);
}

BOOL   CSchduleDatabaseInterface::UpdateTrainInfo(const CTG_ScheduleInfo &schd,TRAIN_INDEX index,const CTG_TrainInfo  &traininfo,int source_flag)
{
	CTG_SingleTrain srctrain;
	srctrain.SetTrainIndex(index); 
	srctrain.UpdateTrainInfo(traininfo,source_flag);
	return UpdateTrainInfo(schd,srctrain,source_flag);
}

BOOL   CSchduleDatabaseInterface::DeleteTrain(const CTG_ScheduleInfo &schd,TRAIN_INDEX index,int source_flag)
{
	CWorkScheduleDataServer *pDb_svr=m_pDatabaseServer;
	if(!pDb_svr)
		return FALSE;
	int con_id=schd.m_nSectionID; 
	
	pDb_svr->Update_DeleteSingleTrainFromWorkSchedule(con_id,index);
	return TRUE;
}

BOOL   CSchduleDatabaseInterface::DeleteMark(const CTG_ScheduleInfo &schd,TRAIN_INDEX index,int source_flag)
{
	CWorkScheduleDataServer *pDb_svr=m_pDatabaseServer;
	if(!pDb_svr)
		return FALSE;
	int con_id=schd.m_nSectionID; 
	
	pDb_svr->Update_DeleteBlockMark(con_id,index,FALSE);
	return TRUE;
}

////////  COPY
BOOL   CSchduleDatabaseInterface::CopyAllData(const CTG_ScheduleInfo &schd,IScheduleComm *pOut,BOOL all_flag)
{
	if(schd.m_nType == CTG_ScheduleInfo::SCHEDULE_TYPE_BASIC) 
		return CopyBasicScheduleData(schd,pOut);

	CWorkScheduleDataServer *pDb_svr=m_pDatabaseServer;
	if(!pDb_svr)
		return FALSE;
	int con_id=schd.m_nSectionID;   

	pOut->BeginCopyData(); 
	pDb_svr->Query_ReadBlockMark(con_id,schd.m_tStartTime,schd.m_tEndTime);
	int cnt;
	int shift_id=0;
	cnt=pDb_svr->LoadWorkScheduleTrainIndex(con_id,shift_id,schd.m_tStartTime,schd.m_tEndTime);
	if(cnt>0)
	{
		    CADDATA_SINGLE_TRAIN singletrain;
			list<CADDATA_SINGLE_TRAIN_HEAD>::iterator iter = pDb_svr->m_listRealTrains.begin();
			while(iter != pDb_svr->m_listRealTrains.end())
			{
				singletrain.Init(); 
				if(pDb_svr->Query_ReadWorkScheduleSingleTrain(con_id,iter->m_nTrainIndex,&singletrain)<=0)
					break;
				CTG_SingleTrain train;
				TG_CopyData(&train,&singletrain);
				pOut->AppendTrain(schd,train,GetInterfaceFlag()); 
				iter++;  //
			}
		    // 读取标记信息
			list<CADDATA_ABS_MARK>::iterator mark_iter=pDb_svr->m_listAbsMark.begin();
 			while(mark_iter!=pDb_svr->m_listAbsMark.end())
			{
				//if(mark_iter->m_nSyncFlag != SYNC_FLAG_NOT_SYNCED_DELETE)
				CTG_SingleMark mark;
				TG_CopyData(&mark,&*mark_iter);
				pOut->AppendMark(schd,mark,GetInterfaceFlag()); 
 				mark_iter++;
			}
	}
	pOut->SetDataOK(TRUE); 
	pOut->EndCopyData();
	return TRUE;
}

BOOL   CSchduleDatabaseInterface::CopyBasicScheduleData(const CTG_ScheduleInfo &schd,IScheduleComm *pOut)
{
	CCADDataServer *pDb_svr=m_pDatabaseServer;
	if(!pDb_svr)
		return FALSE;
	int  startdate=0; //CTG_ScheduleInfo::Get_ShiftDateBasicSchedule(schd.m_nShiftID); 
	int  con_id=schd.m_nSectionID; 
	int cnt=pDb_svr->Query_StartToReadBasicSchedule(con_id);
	if(cnt>0)
	{
			//SendMessage(hwnd,UM_TGMSG_SCHDSTATUS, 1,0);
			CADDATA_SINGLE_TRAIN singletrain;
			while(1)
			{
				singletrain.Init(); 
				if(pDb_svr->Query_ReadBasicScheduleSingleTrain(con_id,startdate,&singletrain)<=0)
					break;
				CTG_SingleTrain train;
				TG_CopyData(&train,&singletrain);
				//train.SetTrainIndex(train.GetBasicTrainIndex());   
                pOut->AppendTrain(schd,train,GetInterfaceFlag()); 
			}
			pDb_svr->Query_EndReading();
			pOut->SetDataOK(TRUE);
			return TRUE;
	}
	return FALSE;
	
}

