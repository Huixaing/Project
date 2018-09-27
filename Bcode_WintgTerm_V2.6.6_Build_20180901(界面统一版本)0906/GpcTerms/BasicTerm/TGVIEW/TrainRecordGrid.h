#pragma once
#include "utility/xmlview.h"
class CTG_SingleTrain;
class  CTrainRecordGridView:public CXMLDataGrid{
public:
	BOOL  CreateGrid(CWnd *parent,const RECT &rect,UINT ctrlid);
	int   InsertRecord(const char *id,const CTG_SingleTrain *ptrain,int station,int rec_index);
	void  RefreshRecord(const CTG_SingleTrain *ptrain,int index,int station,int rec_index);
	void  RefreshStationRecord(const CTG_SingleTrain *ptrain,int station, int rec_index);
};