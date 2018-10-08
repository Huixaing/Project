#include "tg_schedule_packdata.h"
#include "icommtask.h"
#include "schd_newserver.h"

BOOL  CTG_SchedulePackData::ReloadData()
{
     if(!m_pComm)
		 return FALSE;
	 CTG_ScheduleInfo info;
	 m_database.GetScheduleInfo(info);
	 m_database.DeleteAllData(info,SYCFLAG_NULL);
	 m_database.SetMarkDataInited(FALSE); 
	 m_database.SetTrainDataInited(FALSE);
	 return m_pComm->SendScheduleLoadMessage_ToServer(info, m_aStationList,m_nStationCount);

}

void  CTG_SchedulePackData::SetStationList(int stats[],int cnt)
{
	if(cnt>MAX_STATION_NUM_IN_PACK)
		cnt=MAX_STATION_NUM_IN_PACK;
	for(int i=0;i<cnt;i++)
		m_aStationList[i]=stats[i];
	m_nStationCount=cnt;
}

void  CTG_SchedulePackData::SetCommInterface(CWintgNewServer *pcomm)
{
	m_pComm=pcomm;
	if(m_pComm) m_pComm->SetDataStore(&m_database);
}
