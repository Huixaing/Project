#if !defined(_MYDATA_H)
#define _MYDATA_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "utility/ctc_data_type_def.h"
class CTG_LogicObject;
class CTG_SingleMark;
class CTG_SingleTrain;
class CTG_TrainRecord;
#define ID_PD_TRAIN_INFO            0
#define ID_PD_TRAIN_INDEX		    1
#define ID_PD_TRAIN_ID  		    2
#define ID_PD_TRAIN_TYPE		    3
#define ID_PD_TRAIN_RUN_TYPE        4
#define ID_PD_TRAIN_PRIORITY        5
#define ID_PD_TRAIN_SUPER_RANK      6
#define ID_PD_TRAIN_LENGTH_RANK     7
#define ID_PD_TRAIN_MILITARY_RANK   8
#define ID_PD_TRAIN_SPEED_LIMIT     9
#define ID_PD_TRAIN_ELEC            110
#define ID_PD_TRAIN_IMPORTRANK      111
#define ID_PD_TRAIN_TRACT_ENGINE    112
#define ID_PD_TRAIN_ATTACH_ENGINE   113
#define ID_PD_TRAIN_WEIGHT          114
#define ID_PD_TRAIN_LENGTH          115
#define ID_PD_TRAIN_ROUTE_NAME      116
#define ID_PD_TRAIN_COUNT			117

#define ID_PD_TRAIN_START_PLAN_TIME    10     // 始发计划时间
#define ID_PD_TRAIN_START_ACTUAL_TIME  11   // 始发实际时间
#define ID_PD_TRAIN_SBORDER_PLAN_TIME  12   // 分界口接入计划时间
#define ID_PD_TRAIN_SBORDER_ACTUAL_TIME 13 // 分界口接入实际时间
#define ID_PD_TRAIN_EBORDER_PLAN_TIME   14  // 分界口交出计划时间
#define ID_PD_TRAIN_EBORDER_ACTUAL_TIME 15 // 分界口交出实际时间
#define ID_PD_TRAIN_SBORDER_STA         16 // 分界口接入站号
#define ID_PD_TRAIN_EBORDER_STA         17  // 分界口交出站号
#define ID_PD_TRAIN_START_STA_NAME      18  // 起始车站名
#define ID_PD_TRAIN_END_STA_NAME        19 // 终到车站名
#define ID_PD_TRAIN_ABS_TIME_TYPE		20

#define ID_PD_TRAIN_EXT_INFO            101
#define ID_PD_TRAIN_SHIFT_INDEX         102  // OPMS 全局ID
#define ID_PD_TRAIN_IN_SECTION_TIME     103  // 本台起始时间
#define ID_PD_TRAIN_OUT_SECTION_TIME    104  // 本台结束时间
#define ID_PD_TRAIN_IN_SECTION_FLAG     105  // 本台进出标志
#define ID_PD_TRAIN_OUT_SECTION_FLAG    106  // 本台进出标志
#define ID_PD_TRAIN_IMPORTANT_FLAG      107  // 本台进出标志
#define ID_PD_TRAIN_FIRST_STA_NAME      108  // 接入站
#define ID_PD_TRAIN_OPER                150  // 操作标志
#define ID_PD_MPLAN_DCMD_INDEX          151  // 操作标志
#define ID_PD_TRAIN_SBORDER_DELAYTIME   152
#define ID_PD_MPLAN_DCMD_CONTENT        153   // 施工调度命令正文

#define ID_PD_TYPLAN_DCMD_ID			180
#define ID_PD_TYPLAN_DCMD_CONTENT		181

#define ID_PD_IN_STATION_SIDE_OPER      200  // 接入车站股道
#define ID_PD_IN_STATION_WORK_OPER      201  // 接入车站作业

#define ID_PD_ST_INFO              20
#define ID_PD_ST_STATION           21
#define ID_PD_ST_SCHD_ARRIVE_TIME  22
#define ID_PD_ST_SCHD_DEPART_TIME  23
#define ID_PD_ST_ARRIVE_TIME       24
#define ID_PD_ST_DEPART_TIME       25
#define ID_PD_ST_ENTRY_ID          26
#define ID_PD_ST_EXIT_ID           27
#define ID_PD_ST_STOP_TIME         28
#define ID_PD_ST_MIN_STOP_TIME     29
#define ID_PD_ST_ARRIVE_SIDE_ID    30
#define ID_PD_ST_DEPART_SIDE_ID    34
#define ID_PD_ST_PLAN_ARRIVE_SIDE_ID 65
#define ID_PD_ST_PLAN_DEPART_SIDE_ID 66
#define ID_PD_ST_FORCE_SIDE        31
#define ID_PD_ST_NEXT_ABS_TIME     32
#define ID_PD_ST_FORCE_NEXT_TIME   33
#define ID_PD_ST_DEPART_NOT_EARLY_TIME    35
#define ID_PD_ST_DEPART_BY_PLAN    36
#define ID_PD_ST_TECH_STOP         37
#define ID_PD_ST_ARRIVE_TRAINID    38
#define ID_PD_ST_DEPART_TRAINID    39
#define ID_PD_ST_STATION_RECINDEX  40
#define ID_PD_ST_AUTOTRIG          41   // 1: TRIG arrive, 2 TRIG depart 3 ALL 0 NONE
#define ID_PD_ST_MANUALSET_PASS    42   // 1：规定通过
#define ID_PD_ST_NOT_MEET          43   // 禁会
#define ID_PD_ST_ELEC_LOCO         44   // 禁会
#define ID_PD_ST_IMPINFO           46   // 特运
#define ID_PD_ST_STPINFO           47   // 禁停


#define ID_PD_ST_MARK_DATE          49
#define ID_PD_ST_MARK_START_TIME    50  // start_time;
#define ID_PD_ST_MARK_END_TIME      51  //     end_time;
#define ID_PD_ST_MARK_TYPE          52  //  BYTE       type;           //  ABS SLOW,      ABS_INHIBIT,  ELECTRO_FIX,  SIDE_SLOW,    SIDE_INHIBIT,  GRAPH_NOTE, ABS_ELEC_FIX, REGIST_BLOCK_TYPE
#define ID_PD_ST_MARK_FLAG          53  // flag;           //  limit_speed    flag          0             limit_speed   type           0,                        block_type(0-half,1-auto,2-tel)
#define ID_PD_ST_MARK_DIR           54 //           USHORT     dir;            // used by gpc
#define ID_PD_ST_MARK_STATION       55 //           STATION_NO station;        //  0              0             0             station       station        top_position
#define ID_PD_ST_MARK_POSITION      56  //      USHORT     position;       //  limit_length   position      position      limit_length  position       bottom_position
#define ID_PD_ST_MARK_POSITION      56  //      USHORT     position;       //  limit_length   position      position      limit_length  position       bottom_position
                                  // bit 0:  1 -- inhibit first sta, 0 -- not;  bit 1: 1 -- inhibit second sta, 0 -- not ;  when type=SECTION_ELEC_FIX
#define ID_PD_ST_MARK_TEXT          57 //   char      text[MAX_SCHD_BLOCK_TEXT_LEN];
#define ID_PD_ST_MARK_SMETER        58 //            USHORT    s_meter;  //first sta num ,when type = SECTION_ELEC_FIX
#define ID_PD_ST_MARK_EMETER        59 //            USHORT    e_meter;  //second sta num ,when type = SECTION_ELEC_FIX
#define ID_PD_ST_MARK_EKILO         60 //            USHORT    e_kilo;
#define ID_PD_ST_MARK_SLOWTIME      61 //           USHORT    slow_time;
#define ID_PD_ST_MARK_STATION1      62 
#define ID_PD_ST_MARK_STATION2      63
#define ID_PD_ST_MARK_MPLAN_ID      64

#define ID_PD_CON_ID          71
#define ID_PD_CON_NAME        72
#define ID_PD_CON_SHIFT       73
#define ID_PD_CON_START_TIME  74
#define ID_PD_CON_END_TIME    75

#define ID_PD_CON_PASSENGER_TRAIN_COUNT    76
#define ID_PD_CON_FREIGHT_TRAIN_COUNT    77
#define ID_PD_CON_TRAIN_COUNT    78
#define ID_PD_CON_MARK_COUNT     79

#define ID_PD_CON_PASSENGER_TRAIN_COUNT_BASIC    86
#define ID_PD_CON_FREIGHT_TRAIN_COUNT_BASIC    87
#define ID_PD_CON_TRAIN_COUNT_BASIC    88
#define ID_PD_CON_MARK_COUNT_BASIC     89

#define ID_PD_CON_PASSENGER_TRAIN_COUNT_SHIFT    96
#define ID_PD_CON_FREIGHT_TRAIN_COUNT_SHIFT    97
#define ID_PD_CON_TRAIN_COUNT_SHIFT    98
#define ID_PD_CON_MARK_COUNT_SHIFT     99

#define ID_PD_MPLAN_LIST_SUBID        120

#define ID_PD_ST_ARRIVE_RANK		121
#define ID_PD_ST_DEPART_RANK		122

#define TGPSP_MAKE_LONG_ID(station,id)   ((station<<16)+id)
#define TGPSP_GET_STATION_ID(id)  ((id>>16)&0xFFFF)
#define TGPSP_GET_PROP_ID(id)  (id&0xFFFF)


////////////////////////// Global fielddef
extern PropertyInfoPara g_mmplan_info[];
int    count_of_g_mmplan_info();
int    count_of_mark_def(); // 仅符号

int    count_of_g_mmplan_note_list_info();
extern PropertyInfoPara g_mmplan_note_list_info[];

int    count_of_g_mmplan_note_view_info();
extern PropertyInfoPara g_mmplan_note_view_info[];

int    count_of_mark_def();


extern struct PropertyInfoPara  g_recordinfo_def[];
int    count_of_recordinfo_def();

int    count_of_mmplan_dcmd_def();
extern PropertyInfoPara g_mmplan_dcmd_def[];

int    count_of_jieshi_def();
extern PropertyInfoPara g_mmplan_jieshi_def[];

int    count_of_viewlist_def();
extern PropertyInfoPara g_dcmd_viewlist_def[];

int    count_of_opms_train_list_def();
extern PropertyInfoPara g_opms_train_list_def[];

int    count_of_quebao_def();
extern PropertyInfoPara g_quebao_def[];

int    count_of_mplan_trninfo_def();
extern PropertyInfoPara g_mplan_trninfo_def[];

extern PropertyInfoPara g_typlan_def[];
int    count_of_typlan_def();

extern PropertyInfoPara g_typlan_dcmd_def[];
int    count_of_typlan_dcmd_def();

long  TGPSP_GetItemIntValue(const CTG_LogicObject *pobj,int id, int stationid);


const char *TGPSP_GetItemValue(const CTG_LogicObject *pobj,int id, int stationid);
const char *TGPSP_GetItemChoice(const CTG_LogicObject *pobj,CTC_DATA_DEF datatp, int sid, int stationid, int index,int *value);
BOOL  TGPSP_SetItemValue(CTG_LogicObject *pobj,int id, int stationid,const char *strvalue);

const char *GetObjectImportantInfo(const CTG_LogicObject *p);
const char *GetObjectAllInfo(const CTG_LogicObject *p);
const char *GetObjectWorkInfo(const CTG_LogicObject *p);
const char *GetTrainShortImportantInfo(const CTG_TrainRecord *prec, BYTE space_rank);
const char *GetTrainStopInfo(const CTG_TrainRecord *prec);
BOOL  GetMarkStation(const CTG_SingleMark  *pmark,int &station1,int &station2);
const char *GetTrainEngineDesc(const CTG_SingleTrain *ptrain,int flag); // flag:1 TRACT, 2:ATTACH, 3: ALL
const char *Compare2Trains(const CTG_SingleTrain *psrctrain,const CTG_SingleTrain *desttrain,DWORD &flag, int nThisSection, WORD& nTipToDdy);
#endif // !defined(AFX_PROPERTYLISTCTRLDLG_H__466FB4E7_6072_11D3_A7E3_006008C8B630__INCLUDED_)