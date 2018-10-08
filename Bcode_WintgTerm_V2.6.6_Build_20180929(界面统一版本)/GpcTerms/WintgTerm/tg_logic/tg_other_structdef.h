#ifndef __INCLUDE__NEIGHBOR_PLAN__
#define __INCLUDE__NEIGHBOR_PLAN__
#include "tg_structdef.h"


#define STAGE_PER_WORK  3
#define MAX_STATIONS_PER_SECTION     128


struct TG_STATIONPLAN_IMPORTANT
{
	time_t   send_time ;//
    time_t   recv_time ;//
	time_t   edit_time ;//

	long usSendDate;
	long usSendTime;
	ushort bSignInFlag;
	ushort bReceiveFlag;

	char strSignName[16];
	char strRecvName[16];
};
//new defined 
//一个实体的阶段记事
struct TG_STATIONPLANINFO_STAGE_NEW
{
	int nStation; 
	int nEntityIndex;
	DWORD Entityid;
	char  EntityName[32];
	TG_STATIONPLAN_IMPORTANT m_StageInfo[STAGE_PER_WORK];
};
//多个车站的阶段记事
struct TG_STATIONPLANINFO_IMPORTANTNEW
{
    int nEntityNum;
	CString text_content[STAGE_PER_WORK];
	ushort  start_st_weather[STAGE_PER_WORK];
	ushort  end_st_weather[STAGE_PER_WORK];
	
	CString text_commander[STAGE_PER_WORK];
	CString text_command_time[STAGE_PER_WORK];
	CString text_send_time[STAGE_PER_WORK];
	
	time_t   send_time[STAGE_PER_WORK];//
    time_t   recv_time[STAGE_PER_WORK];//
	time_t   edit_time[STAGE_PER_WORK];//
	
    TG_STATIONPLANINFO_STAGE_NEW m_StationPlan[MAX_STATIONS_PER_SECTION];

	void init()
	{
		nEntityNum=0;
		memset(&m_StationPlan, 0, MAX_STATIONS_PER_SECTION*sizeof(TG_STATIONPLANINFO_STAGE_NEW));
		for(int i=0; i<STAGE_PER_WORK; i++)
		{
			text_content[i]="";
			text_commander[i]="";
			text_command_time[i]="";
			text_send_time[i]="";
			start_st_weather[i]=0;
			end_st_weather[i]=0;
			send_time[i]=0;
			recv_time[i]=0;
			edit_time[i]=0;
		}
	}
};

struct TG_NOTEBOOK
{
	 short ddt_id;
	 unsigned long  shift_id;

     short note_len;
	 char note[2500];
};

struct TG_STATION_STATUS
{
	int station_no;
	STATION_CTRL_MODE  ctrl_mode;
	ROUTE_CTRL_MODE    route_mode;
	enum{ENTITY_ACTIVE,ENTITY_INACTIVE} stpc_status,lirc_status;
	enum PLAN_SEND_STATUS{SENDNULL=0,NEED_RESEND=3,SENDING=1,SENDOK=2} stpc_plan_status,lirc_plan_status;
	
	TG_STATION_STATUS()
	{
		station_no=0;
		ctrl_mode=SMODE_EMERGENCY;
		route_mode=RMODE_PLAN_CTRL;
		stpc_status=ENTITY_INACTIVE;
		lirc_status=ENTITY_INACTIVE;
		stpc_plan_status=SENDNULL;
		lirc_plan_status=SENDNULL;
	}
};
#endif
