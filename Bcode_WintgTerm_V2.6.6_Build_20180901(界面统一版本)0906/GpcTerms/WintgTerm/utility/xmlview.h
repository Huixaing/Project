#pragma once
#include "GridCtrl_src/GridCtrl.h"
#include "utility/ctc_data_type_def.h"

#define ROW_COLOR_LEVEL_1 RGB(128,255,255)
#define ROW_COLOR_LEVEL_2 RGB(126,192,238)

class CXMLDataGrid:public CGridCtrl
{
public:
	CXMLDataGrid();
	virtual ~CXMLDataGrid();
protected:
	BOOL  CreateGridBody(CWnd *parent,const RECT &rect,UINT ctrlid,const PropertyInfoPara fielddef[],int fieldcnt,int rowcnt);
	virtual void  BeginEditCell(int row,int col);
	virtual void  EndEditCell(int row,int col);
	virtual void  OnColumnSelect(int row, int col); 
	void  SaveVerticalToXML(CString &dest,const char *subnode);
public:
	enum{MAX_FIELD_IN_GRID=64};
	PropertyInfoPara m_FiledInfo[MAX_FIELD_IN_GRID];
	int  m_nFieldNum;

	BOOL  m_bSingleLine;
	BOOL  m_bVertical;
	BOOL  m_bRowSelectable;
	BOOL  CreateGrid(CWnd *parent,const RECT &rect,UINT ctrlid,const PropertyInfoPara fielddef[],int fieldcnt,const char *xml,const char *rootnode);
	BOOL  CreateGrid(CWnd *parent,const RECT &rect,UINT ctrlid,const PropertyInfoPara fielddef[],int fieldcnt);
	BOOL  CreateVerticalGrid(CWnd *parent,const RECT &rect,UINT ctrlid,const PropertyInfoPara fielddef[],int fieldcnt);
	BOOL  InsertRows(const char *xml,const char *rootnode);
	BOOL  InitVerticalGrid(const char *xml,const char *rootnode);
	void  SetFiledPara(long para,int index=-1);
	void  SaveRowToXML(CString &dest,int row,const char *rootnode);
	void  SaveToXML(CString &dest,const char *head,const char *rootnode);
	int   Insert1EmptyRow(const char *headstr,int row=-1);
	BOOL  GetFieldString(int row,const char *attr_name,CString &dest);
	void  SetRowSelectable(BOOL flag);
	BOOL  IsRowSelected(int row);
	void  SetRowSelected(int row,BOOL va);
	DECLARE_MESSAGE_MAP()

	BOOL  OnBeginEdit(NMHDR* pNMHDR, LRESULT* pResult);
	BOOL  OnEndEdit(NMHDR* pNMHDR, LRESULT* pResult);
	BOOL  OnColumnSelectChanged(NMHDR* pNMHDR, LRESULT* pResult); 
public:
	int   GetColumnByID(int id);
	void  SetAllRowSelected(BOOL va);
};