#include "stdafx.h"
#include "traingrid.h"
#include "tg_schedule_base.h"
#include "tgpspdata.h"

PropertyInfoPara g_opms_train_list_def[]=
{
	{0,ID_PD_TRAIN_SHIFT_INDEX,CTC_DATA_INT,"GTID","全局ID"},
	{0,ID_PD_TRAIN_ID,CTC_DATA_STR,"TNO","车次号"},
	{1,ID_PD_TRAIN_OPER,CTC_DATA_STR,"oper_id","操作"},
	{0,ID_PD_TRAIN_FIRST_STA_NAME,CTC_DATA_STR,"LSTST","首站"},
	{0,ID_PD_TRAIN_IN_SECTION_TIME,CTC_DATA_DATETIME,"LSTTM","本台开始"},
	{0,ID_PD_TRAIN_ROUTE_NAME,CTC_DATA_STR,"LEDTM","径路"},
};

int count_of_opms_train_list_def()
{
	return sizeof(g_opms_train_list_def)/sizeof(g_opms_train_list_def[0]);
}

BOOL CTrainGridView::CreateGrid(CWnd *parent,const RECT &rect,UINT ctrlid)
{
	return CXMLDataGrid::CreateGrid(parent,rect,ctrlid,g_opms_train_list_def,count_of_opms_train_list_def());
}

int CTrainGridView::InsertTrain(const char *id, CString& operstr, const CTG_SingleTrain *ptrain)
{
	int row=CXMLDataGrid::Insert1EmptyRow(id);
	for(int i=1;i<m_nFieldNum;i++)
	{
		if(m_FiledInfo[i].id==ID_PD_TRAIN_OPER)
		{
			GetCell(row,i)->SetText(operstr);
			if(ptrain->m_nTipToDdy!=0)// 不为0表示有提示标志,设置浅红色背景
			{
				GetCell(row,0)->SetBackClr(RGB(255,100,164));
			}
		}
		else
		{
			const char *strvalue=TGPSP_GetItemValue(ptrain,m_FiledInfo[i].id,0);
			int   intvalue=TGPSP_GetItemIntValue(ptrain,m_FiledInfo[i].id,0);
			if(!strvalue)
			{
				if(intvalue!=-1)
					strvalue=GetCTCValueDispString(m_FiledInfo[i].data_type,intvalue,0);
			}
			if(strvalue)		
			{
				GetCell(row,i)->SetText(strvalue);
			}
		}
	}
	return row;
}
