#include "stdafx.h"
#include "TrainRecordGrid.h"
#include "tg_schedule_base.h"
#include "tgpspdata.h"


BOOL  CTrainRecordGridView::CreateGrid(CWnd *parent,const RECT &rect,UINT ctrlid)
{
	return CXMLDataGrid::CreateGrid(parent,rect,ctrlid,g_recordinfo_def,count_of_recordinfo_def());
}

int   CTrainRecordGridView::InsertRecord(const char *id,const CTG_SingleTrain *ptrain,int station,int rec_index)
{
	int row=CXMLDataGrid::Insert1EmptyRow(id);
	RefreshRecord(ptrain,row,station,rec_index);
	return row;
}

void  CTrainRecordGridView::RefreshRecord(const CTG_SingleTrain *ptrain,int row,int station,int rec_index)
{
	GetCell(row,1)->SetData(station);
	for(int i=1;i<m_nFieldNum;i++)
	{
		const char *strvalue=TGPSP_GetItemValue(ptrain,TGPSP_MAKE_LONG_ID(rec_index,m_FiledInfo[i].id),station);
		int   intvalue=TGPSP_GetItemIntValue(ptrain,TGPSP_MAKE_LONG_ID(rec_index,m_FiledInfo[i].id),station);
		if(!strvalue)
		{
			if(intvalue!=-1)
				strvalue=GetCTCValueDispString(m_FiledInfo[i].data_type,intvalue,0);
		}
		if(strvalue)		
			GetCell(row,i)->SetText(strvalue);
	}
	const CTG_TrainRecord *prec=ptrain->GetTrainRecordByRecIndex_Const(rec_index);
	if(prec)
	{
		const char *ptmpstr=prec->GetStationText(STATION_TXT_INFO_TYPE_SHUAIGUA);
		if(prec->HasImportantFlag() || ptmpstr && strlen(ptmpstr)>0)
		{
			GetCell(row,0)->SetBackClr(RGB(255,0,0));
		}
		else
			GetCell(row,0)->SetBackClr(GetSysColor(COLOR_3DFACE));
	}
}

void  CTrainRecordGridView::RefreshStationRecord(const CTG_SingleTrain *ptrain,int station, int rec_index)
{
	for(int i=1;i<GetRowCount();i++)
	{
		if(GetCell(i,1)->GetData()==station)
		{
			RefreshRecord(ptrain,i,station,rec_index);
			break;
		}
	}
}