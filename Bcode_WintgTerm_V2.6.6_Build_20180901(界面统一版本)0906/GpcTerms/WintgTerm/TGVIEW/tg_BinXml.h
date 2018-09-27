#ifndef __TG_BIN_XML_H__
#define __TG_BIN_XML_H__

#include "tg_schedule_base.h"

int GetDCmdPlanResult(CString xml, CString& warning, CString& id);
int fromDeleteXml(int& showdlg, std::vector<DWORD>& index, CString xml);
CString toXMLString(const CTG_SingleTrain &train,int flag=0);
bool fromXML(CTG_SingleTrain &train,CMMIOperFlag &oper_flag,const char *xml,bool fromFile);
bool fromXML(TrainForExitEntry &train,const char *xml);

CString toXMLString(const CTG_SingleMark &mark);
bool fromXML(CTG_SingleMark &mark,const char *xml);
bool AlarmfromXml(CString xml, CStringArray& group, vector<int>& list_station, vector<int>& list_rec_index);
bool GetPrevAndAfterTrainIndex(CString xml, TRAIN_INDEX& prev_train_index, TRAIN_INDEX& next_train_index);
#endif