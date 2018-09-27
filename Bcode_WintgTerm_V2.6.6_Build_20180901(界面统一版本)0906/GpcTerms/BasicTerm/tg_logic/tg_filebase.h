#pragma once
BOOL  TG_LoadFromFiles(CTG_TrainScheduleBase *pdatabase,TIME sttm=0,TIME endtm=0,const char *trainno=NULL);
BOOL  TG_SaveToFiles(CTG_TrainScheduleBase *pdatabase);
BOOL  TG_SaveTrainToFile(const CTG_ScheduleInfo  &schdinfo,const CTG_SingleTrain *ptrain);
BOOL  TG_SaveMarkToFile(const CTG_ScheduleInfo  &schdinfo,const CTG_SingleMark *pmark);
BOOL  TG_DeleteMarkFile(const CTG_ScheduleInfo  &schdinfo,TRAIN_INDEX index);
BOOL  TG_DeleteTrainFile(const CTG_ScheduleInfo  &schdinfo,TRAIN_INDEX index);
const char *TG_GetScheduleBaseFilePath(const CTG_ScheduleInfo  &schdinfo);