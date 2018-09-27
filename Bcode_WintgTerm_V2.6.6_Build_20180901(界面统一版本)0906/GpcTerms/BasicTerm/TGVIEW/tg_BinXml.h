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
#endif