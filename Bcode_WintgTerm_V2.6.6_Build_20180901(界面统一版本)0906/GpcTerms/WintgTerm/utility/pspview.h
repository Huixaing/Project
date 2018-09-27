#pragma once
#include "ctc_data_type_def.h"
#include "../PropTable/PropTree.h"
class CCTCPspCtrl:public CPropTree{
	enum{MAX_PSP_NUM=256};
	struct PropertyInfoPara pInfo[MAX_PSP_NUM];
	CPropTreeItem *pItem[MAX_PSP_NUM];
	int    infoCount;
	int  GetInfoIndex(int id);
protected:
virtual  const char *GetValue_String(void *p,int id);
 virtual  int         GetValue_Integer(void *p,int id);
 virtual  const char *GetItemListChoice(void *p,int id,int index,int *value);
 virtual  BOOL        SaveValue_String(void *p,int id,const char *value);
 virtual  BOOL        SaveValue_Integer(void *p,int id,int value);
 public:
	BOOL  InitProperty(struct PropertyInfoPara *pInfo0,int infoCount0);
	BOOL  LoadProperty(void *p);
	BOOL  SaveProperty(void *p);
 
};


