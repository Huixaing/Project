#pragma once
BOOL WriteXMLFile(const char *xml,const char *objname,const char *prefix);
BOOL  GenXMLDataSet(CString &dest,const char *objname,const char *xml);
const char *GetObjectXSD(const char *objname);
BOOL ReadXMLFile(CString &dest,const char *xmlfile);
BOOL ReadXMLFile(CString &xml,const char *objname,const char *prefix);
BOOL  GetXMLNodeValue(CString &dest,const char *xml,const char *node);
BOOL  InsertXMLNodeValue(CString &dest,const char *node,const char *value);
BOOL  InsertXMLNodeValue(CString &dest,const char *node,int value);
time_t XMLStringToTime(const char *str);
const char *TimeToXMLString(time_t t);
const char *GenGUIDString();
void  XMLCopyValueString(CString &dest,const char *src);  // 转义
BOOL  InsertXMLNodeTime(CString &dest,const char *node,time_t tm);
const char *GetXMLFilePath();
int ConvertToSubStringArray(const CString &str,int tapch,CStringArray &array);
BOOL ReadGenernalXMLFile(CString &dest,const char *filepath);
BOOL WriteGenernalXMLFile(const char *xml,const char *filepath);
int TruncString(char *str,int limit); // 截取长度，避免半个汉字
#include "my_xml_macro.h"