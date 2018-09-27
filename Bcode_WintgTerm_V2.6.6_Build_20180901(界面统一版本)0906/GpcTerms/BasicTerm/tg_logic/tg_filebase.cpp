#include "stdafx.h"
#include "sysshare.h"
#include "function.h"
#include "utility/xml_utility.h"
#include "tgview/tg_BinXml.h"
#include "tg_function_config.h"

extern bool CreateRecursiveDir(const char* filename);
const char *TG_GetScheduleBaseFilePath(const CTG_ScheduleInfo  &schdinfo)
{
	static char tmp[256];
	sprintf(tmp,"%s/section%d/schedule%d",gpcdef.GetXMLDir(),schdinfo.m_nSectionID,schdinfo.m_nType);
	return tmp;
}

BOOL  TG_DeleteTrainFile(const CTG_ScheduleInfo  &schdinfo,TRAIN_INDEX index)
{
	CString filepath;
	filepath.Format("%s/TRAIN-%d.xml",TG_GetScheduleBaseFilePath(schdinfo),index);
	unlink(filepath);
	return TRUE;
}
BOOL  TG_DeleteMarkFile(const CTG_ScheduleInfo  &schdinfo,TRAIN_INDEX index)
{
	CString filepath;
	filepath.Format("%s/MARK-%d.xml",TG_GetScheduleBaseFilePath(schdinfo),index);
	unlink(filepath);
	return TRUE;
}
BOOL  TG_SaveTrainToFile(const CTG_ScheduleInfo  &schdinfo,const CTG_SingleTrain *ptrain)
{
	CString filepath;
	filepath.Format("%s/TRAIN-%d.xml",TG_GetScheduleBaseFilePath(schdinfo),ptrain->GetTrainIndex());
	CreateRecursiveDir(filepath);
	CString xml=toXMLString(*ptrain);
	if(xml.IsEmpty())
		return FALSE;
	return WriteGenernalXMLFile(xml,filepath);
}

 
BOOL  TG_SaveMarkToFile(const CTG_ScheduleInfo  &schdinfo,const CTG_SingleMark *pmark)
{
	CString filepath;
	filepath.Format("%s/MARK-%d.xml",TG_GetScheduleBaseFilePath(schdinfo),pmark->GetBlockIndex());
    CreateRecursiveDir(filepath);
	CString xml=toXMLString(*pmark);
	if(xml.IsEmpty())
		return FALSE;
	return WriteGenernalXMLFile(xml,filepath);
}


BOOL  TG_SaveTrainToFiles(CTG_TrainScheduleBase *pdatabase)
{
    CTG_TrainPtrGroup result;
	CTG_ScheduleInfo schdinfo;
	pdatabase->GetScheduleInfo(schdinfo);
	if(pdatabase->FindTrainsNotOrder(result,NULL_STATION,NULL_TIME,NULL_TIME,TRUE))
	{
		for(int i=0;i<result.GetTrainCount();i++)
		{
		  CTG_SingleTrain *ptrain=result.GetTrainPtr(i);
		  if(ptrain)
			TG_SaveTrainToFile(schdinfo,ptrain);
		}
	}
	return TRUE;
}
BOOL  TG_SaveMarkToFiles(CTG_TrainScheduleBase *pdatabase)
{
    CTG_ObjectPtrGroup result;
	CTG_ScheduleInfo schdinfo;
	pdatabase->GetScheduleInfo(schdinfo);
	if(pdatabase->FindMarks(result,NULL_TIME,NULL_TIME))
	{
		for(int i=0;i<result.GetCount();i++)
		{
		  CTG_SingleMark *pmark=(CTG_SingleMark *)result.GetPtr(i);
		  if(pmark)
			  TG_SaveMarkToFile(schdinfo,pmark);
		}
	}
	return TRUE;
}

BOOL  TG_LoadTrainFromFiles(CTG_TrainScheduleBase *pdatabase,TIME sttm,TIME endtm,const char *trainno)
{
	CTG_ScheduleInfo schdinfo;
	CString filepath;
	pdatabase->GetScheduleInfo(schdinfo);
	filepath.Format("%s/TRAIN-*.xml",TG_GetScheduleBaseFilePath(schdinfo));
	if(IS_BASIC_SCHD(schdinfo.m_nType)) //2010.2.26 基本图时间范围
	{
		schdinfo.m_tStartTime=0;
		schdinfo.m_tEndTime=86400*3;
	}
	
    if(sttm<=0 )
		sttm=schdinfo.m_tStartTime;
	if(endtm<=0)
		endtm=schdinfo.m_tEndTime;
	
	CFileFind find;
	if(find.FindFile(filepath))
	{
		BOOL  ffrc=TRUE;
		while(ffrc)
		{
			ffrc=find.FindNextFile();
			CString xml;
			if(ReadGenernalXMLFile(xml,find.GetFilePath()))
			{
				CTG_SingleTrain train;
				CMMIOperFlag oper_flag;
				if(fromXML(train,oper_flag,xml,true))
				{
					if(trainno && strlen(trainno)>0 &&  !train.MatchTrainID(trainno))
						continue;
					if(train.IfInTimeScope(sttm,endtm))
					{
						CTG_SingleTrain *prev=pdatabase->GetSingleTrain(train.GetTrainIndex());
						train.SetTrainStSpace();
						if(prev)
							pdatabase->UpdateTrain(schdinfo,train,SYCFLAG_NULL);
						else
							pdatabase->AppendTrain(schdinfo,train,SYCFLAG_NULL);
					}
				}
			}
		}
	}
	find.Close();
    return TRUE;
}

 BOOL  TG_LoadMarkFromFiles(CTG_TrainScheduleBase *pdatabase,TIME sttm,TIME endtm)
{
	CTG_ScheduleInfo schdinfo;
	CString filepath;
	pdatabase->GetScheduleInfo(schdinfo);
	filepath.Format("%s/MARK-*.xml",TG_GetScheduleBaseFilePath(schdinfo));
    if(sttm<=0)
		sttm=schdinfo.m_tStartTime;
	if(endtm<=0)
		endtm=schdinfo.m_tEndTime;
  
	CFileFind find;
	if(find.FindFile(filepath))
	{
		BOOL  ffrc=TRUE;
		while(ffrc)
		{
			ffrc=find.FindNextFile();
			CString xml;
			if(ReadGenernalXMLFile(xml,find.GetFilePath()))
			{
				CTG_SingleMark mark;
				if(fromXML(mark,xml))
				{
					if(mark.IfInTimeScope(sttm,endtm))
					{
						CTG_SingleMark *prev=pdatabase->GetSingleMark(mark.GetBlockIndex());
						if(prev)
							pdatabase->UpdateMark(schdinfo,mark,SYCFLAG_NULL);
						else
							pdatabase->AppendMark(schdinfo,mark,SYCFLAG_NULL);
					}
				}
			}
		}
	}
	find.Close();
    return TRUE;
}
BOOL  TG_SaveToFiles(CTG_TrainScheduleBase *pdatabase)
{
	TG_SaveTrainToFiles(pdatabase);
	return TRUE;
}
BOOL  TG_LoadFromFiles(CTG_TrainScheduleBase *pdatabase,TIME sttm,TIME endtm,const char *trainno)
{
	TG_LoadTrainFromFiles(pdatabase,sttm,endtm,trainno);
	return TRUE;
}
