/*--------------------------------------------------------------*/
/* Header file for history module                               */
/*                                                              */
/* Copyright (C) 1996, Casco Signal LTD.                        */
/* Coded by Zengrun. Date of Revision: Mar. 27, 1996.           */
/*--------------------------------------------------------------*/
#ifndef _HISTORY_HEADER
#define _HISTORY_HEADER
#include "defines.h"
#define ORDER_BY_TIME			1
#define ORDER_BY_TRAIN_ID		2
#define ORDER_BY_STATION_NUMBER	3
#define RETRIEVE_LATEST			4
extern int alloc_read_long(PROCESSID pid, byte mode, byte *train, long s_long,
	    long e_long, ushort station_count, ushort *station_list,
	    byte hsty_type);
extern int alloc_read(PROCESSID pid, byte mode, byte *train, long start_date,
	    long start_time, long end_date,  long end_time,
	    ushort station_count, ushort *station_list, byte hsty_type);
extern int free_read(PROCESSID pid);
extern int db_read_train_record_long(PROCESSID pid, ushort *station_number,
	    byte *train_id, long *ltime, ushort *a_d_flag, ushort *side_exit,
	    ushort  *late_reason);
extern int db_read_train_record(PROCESSID pid, ushort *station_number,
	    byte *train_id, long *date, long *time, ushort *a_d_flag,
	    ushort *side_exit, ushort *late_reason);
extern int db_record_train_long(PROCESSID pid, byte station_number,
	    byte *train_id, long ltime, byte a_d_flag, byte side_exit,
	    byte route, byte hsty_type);
extern int db_record_train(PROCESSID pid, byte station_number,
	    byte *train_id, ushort date, ushort time, byte a_d_flag,
	    byte side_exit, byte route, byte hsty_type);
extern int train_record_purge_long(PROCESSID pid, long ltime, byte hsty_type);
extern int train_record_purge(PROCESSID pid, ushort date, ushort time, byte hsty_type);
extern int db_erase_one_record_long(PROCESSID pid, byte station_number,
	    byte *train_id, byte a_d_flag, long ltime, byte hsty_type);
extern int db_erase_one_record(PROCESSID pid, byte station_number, byte *train_id,
	    byte a_d_flag, ushort date, ushort time, byte hsty_type);
extern int db_start_block_oper(PROCESSID pid,ushort nStationNumber,ushort *nStationList);
extern int read_train_history(PROCESSID pid,ushort *count,Pstruct TRAIN_HISTORY (*rec)[ ]);
extern int db_end_block_oper(PROCESSID pid);
// following function for change history record
extern int db_change_trainid(PROCESSID pid, char *old_id, char *new_id, byte hsty_type);
extern int db_delete_train(PROCESSID pid,char *pbTrainId,ushort hsty_type);
extern int db_insert_train_long(PROCESSID pid,char *pbTrainId,ushort count,
	Pstruct TRAINRECORD_LONG (*rec)[ ]);
extern int db_insert_train(PROCESSID pid,char *pbTrainId,ushort count,
	Pstruct TRAINRECORD (*rec)[ ]);
extern int db_change_train_long(PROCESSID pid,char *pbTrainId,ushort count,
	Pstruct TRAINRECORD_LONG (*rec)[ ]);
extern int db_change_train(PROCESSID pid,char *pbTrainId,ushort count,
	Pstruct TRAINRECORD (*rec)[ ]);
extern int db_middle_stop(PROCESSID pid,char *pbTrainId,ushort nStationNo);
extern int db_middle_start(PROCESSID pid,char *pbTrainId,ushort nStationNo);
extern int db_get_one_train_long(PROCESSID pid,char *pbTrainId,ushort *count,
	Pstruct TRAINRECORD_LONG (*rec)[ ],ushort flag,ushort nStationNum,ushort *nStationList);
extern int db_get_one_train(PROCESSID pid,char *pbTrainId,ushort *count,
	Pstruct TRAINRECORD (*rec)[ ],ushort flag,ushort nStationNum,ushort *nStationList);
extern int db_start_get_train(PROCESSID pid,ushort hstflag,ushort nStationNum,ushort *nStationList,
		int nStartDate,int nStartTime,int nEndDate,int nEndTime);
extern int db_query_next_record_long(PROCESSID pid,Pstruct TRAIN_HISTORY_LONG *rec);
extern int db_query_next_record(PROCESSID pid,Pstruct TRAIN_HISTORY *rec);
extern int db_query_next_train(PROCESSID pid,char *pbTrainId,
	ushort *count,Pstruct TRAINRECORD (*rec)[ ]);
extern int db_query_next_train_long(PROCESSID pid,char *pbTrainId,
	ushort *count,Pstruct TRAINRECORD_LONG (*rec)[ ]);
extern int db_end_get_train(PROCESSID pid);
#endif
