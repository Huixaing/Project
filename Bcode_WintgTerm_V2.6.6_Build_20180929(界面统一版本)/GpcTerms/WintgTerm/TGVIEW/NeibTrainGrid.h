#pragma once
#include "utility/xmlview.h"

class  CNeibTrainGridView:public CXMLDataGrid
{
public:
	BOOL  CreateGrid(CWnd *parent,const RECT &rect,UINT ctrlid);

public:
	int   InsertTrainInfo(CString& cap, CString& xml);
	int   UpdateTrainInfo(int row, CString& xml);
	BOOL  IsColSetCheck(int row, int col);
private:
	virtual void OnColumnSelect(int row, int col); 
	bool  UpdateGridInfo(int row, CString& xml);
};