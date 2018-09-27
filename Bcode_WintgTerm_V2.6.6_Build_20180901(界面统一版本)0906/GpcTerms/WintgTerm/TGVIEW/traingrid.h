#pragma once
#include "utility/xmlview.h"
class CTG_SingleTrain;
class CTrainGridView:public CXMLDataGrid
{
	enum 
	{
		DATA_OPER_ADD=1,
		DATA_OPER_UPDATE=2,
		DATA_OPER_DELETE=3
	};
public:
	BOOL  CreateGrid(CWnd *parent,const RECT &rect,UINT ctrlid);
	int   InsertTrain(const char *id,CString& operstr,const CTG_SingleTrain *ptrain);
};