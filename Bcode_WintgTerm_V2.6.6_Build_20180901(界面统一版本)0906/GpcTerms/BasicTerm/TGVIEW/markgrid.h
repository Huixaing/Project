#pragma once
#include "utility/xmlview.h"
class CTG_SingleMark;
class  CMarkGridView:public CXMLDataGrid{
public:
	BOOL  CreateGrid(CWnd *parent,const RECT &rect,UINT ctrlid);
	int   InsertMark(const char *id,const CTG_SingleMark *mark);
};