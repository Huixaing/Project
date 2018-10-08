#ifndef _DBFUNC_HEADER
#define _DBFUNC_HEADER

#include "defines.h"
#include "util.h"

#include "cadstruct.h"
//#include <vector> 
//using namespace std ;

//#include <sqlca.h>

/* return codes */
////#define DEBUG_MODE

#define ALREADY_EXIT		-2
#define NO_DATA_LEFT		10
#define DATA_LEFT			11
#define NO_MORE_DATA		1
#define RET_NO_MORE_DATA	1
#define MAX_SCHD_STATIONS	96
#define NOTIME		0xffffffff

#define MAX_TRAIN_ID_NUM   8
#define MAX_ENGINEER_NUM   8
#define MAX_LOAD_NUM     8
#define MAX_STATION_TEXT_NUM   32
#define MAX_ABS_TEXT_NUM    32


#define    WORK_SCHEDULE                      0x05
#define    UNCHANGE_HISTORY_SCHEDULE          0X08
#define    HISTORY_SCHEDULE                   0X09

#define    SHIFT_SCHEDULE                     0x04

#define		NORMAL_SCHEDULE					  0x00
#define     YEAR_SCHEDULE                     0x00
	

#define     OTHER_SCHEDULE                    0x00
#define	    BASIC_SCHEDULE					  0X01
#define     SPRING_SCHEDULE                   0x02
#define     SUMMER_SCHEDULE                   0x03
#define    SUB_BUREAU_SCHEDULE                0x07          ////分局计划图
#define     FALL_SCHEDULE                     0x012
#define		WINTER_SCHEDULE					  0x013
#define     AUTUMN_SCHEDULE                   FALL_SCHEDULE
#define     NEIGHBER_SCHEDULE                 0X0b
#define     TMIS_SCHEDULE                     0x0a


#define		CURR_SHIFT						  0X06
#define		LAST_SHIFT						  10
#define		NEXT_SHIFT						  11
#define		PREV_SHIFT						  LAST_SHIFT


#define HSTY_ORIG   0
#define HSTY_MOD    1

#define ABS_SLOW_OPERATE		1
#define ABS_INHIBIT_OPERATE		2
#define SIDE_INHIBIT_OPERATE	3
/* operation types */
#ifndef SCHD_OPERATETYPE
#define SCHD_OPERATETYPE
#define SCHD_CHANGE				            2
#define SCHD_DELETE				            3
#define SCHD_INSERT				            1
#define SCHD_RENAME				            4
#define SCHD_HISTORY_RENAME		            5
#define SCHD_MID_STOP		            	6
#define SCHD_MID_START		            	7

#define SCHD_CHANGE_RECORD  			         2
#define SCHD_CHANGE_TRAIN_INFO                   8
    #define WHOLE_TRAIN_INFO               1
    #define STATION_ABS_LOAD_TEXT          2
    #define WHOLE_CONSOLE_TRAIN_INFO       3
    #define ENGINE_ADJUST_BEGIN_STATION    4
    #define CHANGE_TRAIN_ID                5
    #define CHANGE_MISC_TRAIN_INFO         6  
    #define CHANGE_BASIC_TRAIN_INFO        7  
     // wangjinhai
  #define WHOLE_TRAIN_INFO_INSERT               8


#define  SCHD_INSERT_TRAINRECORD                 9

//ADD BY CF AFTER 9-4
#define SCHD_TIME_PASS_STAY_FLAG           17  //content same as SCHD_TIME_STATUS_FLAG 
#define   SCHD_PASS_STAY_THIS_NORMAL_LATTER_NORMAL                  18
#define   SCHD_PASS_STAY_THIS_MEET_GAP_LATTER_NORMAL             19
#define   SCHD_PASS_STAY_THIS_NORMAL_LATTER_THROUGH               20
#define   SCHD_PASS_STAY_THIS_MEET_GAP_LATTER_THROUGH          21

// for dmis capture
#define SCHD_TIME_STATUS_FLAG              10
	#define FOCUS_RECORD                   1
	#define BEFORE_FOCUS_RECORD            2
	#define BEHIND_FOCUS_RECORD            3
	#define WHOLE_TRAIN_RECORD             4

#define SCHD_TURN_REALITY                  11
#define SCHD_CHANGE_ONE_TRAINRECORD        12
    #define PRE_ONE_RECORD 6                  //add by cf after 7-18 
	#define PRE_FOCUS_AFTER_ONE_RECORD  7
#define SCHD_CHANGE_WHOLE_TRAINRECORD      12 //add by cf after 7-18
#define SCHD_CHANGE_SIDE                   13
#define SCHD_ADJUST_SUB                    14 //add by cf

#define SCHD_INSERT_TRAIN                   14
#define SCHD_REPORT_POINT                   15
#define  SCHD_CHANGE_TRAIN                  16


#endif

#define SCHD_ARRIVE		0x41
#define SCHD_DEPART		0x44

#define SCHD_UPDATE_TRAIN_ATTRIBUTE	0x00
#define SCHD_UPDATE_TRAIN_INFORM	0X01


 

//added by qianguorong 2003-08-14 ends here
typedef Pstruct _tagStpcReportRecord{
    char train_id[10];
	BYTE flag;
	ushort station;
	BYTE a_d_flag;
	BYTE side;
	long usDate;
	long usTime;
}STPC_REPORT_RECORD;








extern int OsConnectToDatabase(ushort usProcessId);
extern int OsDisconnectToDatabase(ushort usProcessId);
extern int SchdDeleteTrain(PROCESSID pid, ushort usSchdType, byte *train_id);
extern int SchdInsertTrainLong(PROCESSID pid, ushort usSchdType, byte *train,
     Pstruct TRAIN_INFO *loc, ushort count, Pstruct TRAINRECORD_LONG (*rec)[]);
extern int SchdInsertTrain(PROCESSID pid, ushort usSchdType, byte *train,
	Pstruct TRAIN_INFO *loc, ushort count, Pstruct TRAINRECORD (*rec)[]);
extern int SchdChangeTrainLong(PROCESSID pid, ushort usSchdType,  byte *train,
	Pstruct TRAIN_INFO *loc, ushort count, Pstruct TRAINRECORD_LONG (*rec)[]);
extern int SchdChangeTrain(PROCESSID pid, ushort usSchdType,  byte *train,
	Pstruct TRAIN_INFO *loc, ushort count, Pstruct TRAINRECORD (*rec)[]);
extern int SchdRenameTrain (PROCESSID pid, ushort usSchdType, byte *train,
	  byte *new_train);

extern int StartSchdBlockOperLong(PROCESSID pid, ushort usSchdType,
	   ushort station_count, ushort *station_list, byte *train,
	   long stime, long etime);
extern int StartSchdBlockOper(PROCESSID pid, ushort usSchdType, ushort station_count,
	   ushort *station_list, byte *train, long startdate,
	   long starttime, long enddate, long endtime);
extern int SchdGetNextTrainLong(PROCESSID pid, byte *train,
	  ushort *record_count, Pstruct TRAINRECORD_LONG (*rec)[]);

extern int SchdGetNextTrain(PROCESSID pid,byte nSchdType, byte *train,
	  ushort *record_count, Pstruct TRAINRECORD (*rec)[]);
extern int SchdTrainInfo(PROCESSID pid, ushort usSchdType, byte *train, ushort *count,
	  Pstruct TRAIN_INFO (*buffer)[ ]);

extern int EndSchdBlockOper(PROCESSID pid);
extern int SchdPurgeLong(PROCESSID pid, ushort usSchdType, long ltime,
	  ushort station_count, BYTE  *station_list);
extern int SchdPurge(PROCESSID pid, ushort usSchdType, long date, long time,
	  ushort station_count, BYTE  *station_list);
extern int TrainInfoPurge(PROCESSID pid, long ltime);
extern int ConvertStationEntryToAbsnum(ushort station, ushort entry_num,
	     ushort *abs_num,ushort *dir);
extern int ConvertAbsnumStationToEntry(ushort station,ushort abs_num);
extern int ConvertAbsnumToStationEntry(ushort *station, ushort *entry_num,
	     ushort abs_num,ushort dir);
//lyc 2001/9/27
extern int ConvertAbsNoToStationId(int nAbsNo,int& nFirstStation,int& nSecondStation,BOOL bHigh = FALSE);
extern ushort *ConvertStationIdToAbsNo(ushort& nAbsCount,int nFirstStation,int nSecondStation,int nDirect);
extern int ConvertAbsNoToDirect(int nAbsNo);

extern int SchdGetOneTrainLong(PROCESSID pid, ushort usSchdType, byte *train, byte flag,
	  long ltime, ushort *count, Pstruct TRAINRECORD_LONG (*rec)[],
	  ushort nStationNum,ushort *nStationList);
extern int SchdGetOneTrain(PROCESSID pid, ushort usSchdType, byte *train, byte flag,
	  long ltime, ushort *count, Pstruct TRAINRECORD (*rec)[],
	  ushort nStationNum,ushort *nStationList);

extern int ShiftSchedulePurge(PROCESSID pid, long ltime);
extern int SchdQueryAbsInform(PROCESSID pid,ushort stno,int *nAbsNumber,int *nAbsList);
extern int SchdStartQueryChange(PROCESSID pid,ushort usSchdType);
extern int SchdQueryChange(PROCESSID pid,Pstruct SchdOperate *schdResult);
extern int SchdEndQueryChange(PROCESSID pid);
extern int SchdGetAbsInhibit(PROCESSID pid,ushort stno,int *count,Pstruct AbsInhibit (*absInhibit)[ ]);
extern int SchdGetSideInhibit(PROCESSID pid,ushort stno,int *count,Pstruct SideInhibit (*sideInhibit)[ ]);
extern int SchdGetAbsSlow(PROCESSID pid,ushort stno,int *count,Pstruct AbsSlow (*absSlow)[ ]);
extern int SchdPurgeScheduleChange(PROCESSID pid);
extern int SchdStartQueryConflict(PROCESSID pid,ushort nStationNumber,ushort *nStationList);
extern int SchdQueryConflict(PROCESSID pid,ushort *count,CONFLICTINFORM (*rec)[ ]);
extern int SchdEndQueryConflict(PROCESSID pid);
extern int SchdWriteLineOper(PROCESSID pid,ushort opertype,ushort count,char *buff);

extern int SchdQueryWeatherInfo(PROCESSID pid,ushort nStationCount,ushort *nStationList,
	ushort *count,WEATHERINFORM (*rec)[ ]);
extern int SchdWriteWeatherInfo(PROCESSID pid,ushort count,
	WEATHERINFORM (*rec)[ ]);
extern int SchdQueryStationInfo(PROCESSID pid,ushort nStationCount,ushort *nStationList,
	ushort *count,Pstruct SideInform (*rec)[ ]);
extern int SchdWriteStationInfo(PROCESSID pid,ushort count,
	Pstruct SideInform (*rec)[ ]);
extern int SchdWriteTrainInfo(PROCESSID pid,ushort usSchdType,
	ushort count,Pstruct TRAIN_INFO (*rec)[ ],ushort bUpdateFlag);
extern int SchdBeginLateReason(PROCESSID pid,ushort nStationNum,ushort *nStationList);
extern int SchdEndLateReason(PROCESSID pid);



extern int SchdMiddleStop(PROCESSID pid,
			USHORT usSchdType,char *pbTrainId,ushort usStationNo);
extern int SchdMiddleStart(PROCESSID pid,
			USHORT usSchdType,char *pbTrainId,ushort usStationNo);


#endif