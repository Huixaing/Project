#pragma once
#include "cadstruct.h"
class CSingleTrain;
class CWintgDoc;
class CMultiAbsMark;
///////////// CALL BY WINTG APP
BOOL  DB_Init(CString strDataPath); //new!!!
BOOL  DB_StartLocalDataServer(const char *cfgfilename);
BOOL  DB_EndLocalDataServer();
BOOL  DB_InitData(int conid);
BOOL  DB_DeinitData();
BOOL  DB_InLocalDBSyncMode();    // 是否本地同步后备模式
BOOL  DB_InLocalDBActiveMode();  // 是否单机模式
BOOL  DB_InDirectDBMode();   // 是否数据库直接模式,和单机不一样,并非使用本地数据库备份
void  DB_SetLocalDBMode(int mode); //// 0: nothing  1: 本地数据同步后备模式  2: 本地激活模式(单机版)
int   DB_GetLocalDBMode();
void  DB_SetLocalDBToSyncMode(); //设置为本地同步后备模式
void  DB_SetLocalDBToActiveMode();//设置为单机模式
void  DB_SetLocalDBToIdleMode(); // 本地数据库脱离模式，原有模式
BOOL DB_LocalDBStatusOK(); //判断本地数据库是否可用
int   CallMessageLoop();// 调用消息循环


//////////////////////////// 处理GPC的响应,CALL BY  forms
BOOL  DB_LoadTGInitData(int con_id,int schd_type,int shift_id,unsigned long starttime,unsigned long endtime);
BOOL  DB_ProcessTGRequestMessage(BYTE *ptr,int msglen);
BOOL  DB_RequestBasicSchedule(int con_id,int startdate);
BOOL  DB_RequestWorkSchedule(int con_id,int shift_id,unsigned long  starttime,unsigned long endtime);
BOOL  DB_RequestHistorySchedule(int con_id,int shift_id,unsigned long  starttime,unsigned long endtime);
BOOL   DB_RequestSideBlockInfo(HWND hwnd,int schd_type,int station);
BOOL   DB_RequestAbsBlockInfo(HWND hwnd,int schd_type,int station);
BOOL   DB_RequestSideInhibitInfo(HWND hwnd,int schd_type,int station);
BOOL   DB_RequestAbsSlowMoveInfo(HWND hwnd,int schd_type,int station);
BOOL   DB_UpdateSideInhibitInfo(HWND hwnd,int schd_type,int station);
BOOL   DB_UpdateAbsInhibitInfo(HWND hwnd,int schd_type,int station);
BOOL   DB_UpdateAbsSlowMoveInfo(HWND hwnd,int schd_type,int station);
BOOL   DB_UpdateSchdule_SingleTrain(int con_id, int schd_type,int oper_type,int change_sub_type,const char *train_id,CSingleTrain *pTrain);
BOOL   DB_UpdateSchdule_TrainID(HWND hwnd, int con_id, int usSchdType,unsigned long train_index,const char *oldid,const char *newid);
BOOL   DB_UpdateSchdule_BlockMark(int con_id, int schd_type,int oper_type,const struct _tagMarkItem *pItem);
BOOL   DB_UpdateSchdule_DeleteTrain(int con_id,int schd_type,unsigned long train_index);

/////////////  同步服务器来的数据, call by db_msgproc.cpp ///////////
int Gen_TG_MARK_ITEM_Message(int con_id,int sche_type,int oper_type,const struct _tagMarkItem *rec,BYTE *buf);
BOOL  DB_SyncSchedule_SingleTrain(int con_id,int schd_type,CSingleTrain *pTrain,BOOL delete_first=TRUE);
BOOL  DB_SyncSchedule_DeleteAll(int con_id,int schd_type);
BOOL  DB_SyncWorkSchedule_CurrentWorkDocTrains(unsigned long train_index,BOOL delete_old=TRUE); //是否需要删除旧记录，如果是新增车次，则无需删除，否则开销很大
BOOL  DB_SyncWorkSchedule_CurrentWorkDocMarks(int con_id,CMultiAbsMark *pMarks,unsigned long mark_index);
BOOL  DB_SyncSchedule_Mark(int con_id,int schd_type,const struct _tagMarkItem *pItem,BOOL delete_first);
BOOL  DB_SyncSchedule_DeleteTrain(int con_id,int schd_type,unsigned long trainIndex);
BOOL  DB_SyncSchedule_DeleteMark(int con_id,int schd_type,unsigned long mark_index);
BOOL  DB_RestoreWorkScheduleToCAD(int con_id,unsigned long  starttime,unsigned long endtime);
BOOL  DB_ProcessServerTGMessage(int source_eid,BYTE *ptr,int msglen);

///////////// 静态数据查询调用, call by dialogs,forms...
int   DB_GetPasswayInfoCount(int con_id);  
BOOL  DB_RequestTrainTypeInfo(TRAIN_TYPE_INFO  type_info[],int *limit);
///////////// 当前班加车（从全局的DOC中查找班别和日期） ///////////////
BOOL  DB_AddRoutedTrain(int con_id,int iRouteNo,int iDateTime,const char *strTrainId,int iStartFlag,int iEndFlag ); //按径路加车

///////////// 杂项数
BOOL DB_UpdateLineupSchedule(CLBTH::MsgGroupTrainWorkFormat *pMsg);
BOOL DB_DeleteAllLineupSchedule(int con_id,int shift_id);
BOOL  DB_RequestLineupSchedule(int con_id,int shift_id,HWND hwnd); // 暂时通过消息的方式，返回结果
BOOL  DB_UpdateNoteInfo(int con_id,int shd_type,int shift_id,const char *notestr,int notelen);
BOOL  DB_RequestNoteInfo(int con_id,int shd_type,int shift_id,HWND hwnd); // 暂时通过消息的方式，返回结果
BOOL DB_UpdateTrainSubaoInfo(CLBTH::MsgSubaoFormat *pMsgSubaoFormat,HWND hwnd);
BOOL DB_QueryTrainSubaoInfo(CLBTH::MsgSubaoQuery *pMsgSubaoQuery,HWND hwnd);
BOOL DB_QueryStationStoreCarInfo(CLBTH::MsgSimpleStationCarQuery *pmsg,HWND hwnd);
BOOL DB_UpdateStationStoreCarInfo(CLBTH::MsgSimpleStationCarFormat *pMsgSimpleStationCarFormat,HWND hwnd);
BOOL DB_QueryStationStoreCarInfo_Simple(int stationIndex,int phaseIndex,struct _tagStationStoreTrain *pinfo);
BOOL DB_UpdateStationStoreCarInfo_Simple(int stationIndex,int phaseIndex,time_t inputTime,const struct _tagStationStoreTrain *pinfo,HWND hwnd);
BOOL DB_DelteStationStoreCarInfo(CLBTH::MsgSimpleStationCarFormat *pBuf);
///////////// NEW /////////
BOOL  DB_PollingData(void);
BOOL  DB_ProcessServerCommStatus(long server_id,BOOL  active_flag);

int   DB_GetEntityStatus(long eid);
void  DB_SetEntityStatus(long eid,int status);
