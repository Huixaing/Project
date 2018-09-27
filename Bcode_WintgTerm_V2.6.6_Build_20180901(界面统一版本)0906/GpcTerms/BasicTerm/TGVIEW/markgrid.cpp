#include "stdafx.h"
#include "markgrid.h"
#include "tg_schedule_base.h"
#include "tgpspdata.h"


BOOL  CMarkGridView::CreateGrid(CWnd *parent,const RECT &rect,UINT ctrlid)
{
	return CXMLDataGrid::CreateGrid(parent,rect,ctrlid,g_mmplan_info,count_of_mark_def());
}

int   CMarkGridView::InsertMark(const char *id,const CTG_SingleMark *mark)
{
	int row=CXMLDataGrid::Insert1EmptyRow(id);
	for(int i=1;i<m_nFieldNum;i++)
	{
		const char *strvalue=TGPSP_GetItemValue(mark,m_FiledInfo[i].id,0);
		int   intvalue=TGPSP_GetItemIntValue(mark,m_FiledInfo[i].id,0);
		if(!strvalue)
		{
			if(intvalue!=-1)
				strvalue=GetCTCValueDispString(m_FiledInfo[i].data_type,intvalue,0);
		}
		if(strvalue)		GetCell(row,i)->SetText(strvalue);
	}
	return row;
}
