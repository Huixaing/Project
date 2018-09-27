#pragma once
#include "tg_schedule_base.h"
class CWintgNewServer;
class CTG_SchedulePackData{
	enum{MAX_STATION_NUM_IN_PACK=64};
	CTG_TrainScheduleBase  m_database;
	CWintgNewServer       *m_pComm;
	int                    m_aStationList[MAX_STATION_NUM_IN_PACK];
	int                    m_nStationCount;
public:
	CTG_SchedulePackData()
	{
		m_pComm=NULL;
	}
	void  SetStationList(int stats[],int cnt);
	void  SetScheduleInfo(const CTG_ScheduleInfo &schd){m_database.SetScheduleInfo(schd);};
	void  GetScheduleInfo(CTG_ScheduleInfo &schd){m_database.GetScheduleInfo(schd);}
	CTG_TrainScheduleBase  *GetScheduleData() {return &m_database;}
	BOOL  IsDataOK() {return m_database.IsTrainDataInited();}
	BOOL  ReloadData();
	void  SetCommInterface(CWintgNewServer *pcomm);
};

