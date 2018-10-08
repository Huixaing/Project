#pragma once
#define TOKEN_1_ACK_BZ             "ACK_BZ"
#define TOKEN_2_ACK_YL_BY_ID       "ACK_YL_BY_ID"
#define TOKEN_3_ACK_YL_BY_TIME     "ACK_YL_BY_TIME"
#define TOKEN_4_ACK_YL_BY_TRAINNUM "ACK_YL_BY_TRAINNUM"
#define TOKEN_5_ACK_YL_BY_TRAINNUM_TIME  "ACK_YL_BY_TRAINNUM_TIME"
#define TOKEN_6_ACK_GID_BY_TRAINNUM "ACK_GID_BY_TRAINNUM"
#define TOKEN_7_ACK_TFG  "ACK_TFG"
#define TOKEN_8_ACK_NEWYL "ACK_NEWYL"
#define TOKEN_9_ACK_PLAN  "ACK_PLAN"
#define TOKEN_10_ACK_ADPLAN "ACK_ADPLAN"
#define TOKEN_11_ACK_OTHER_MSG "ACK_OTHER_MSG"
#define TOKEN_12_ACK_CMD "ACK_CMD"
#define TOKEN_13_ACK_WKINFO "ACK_WKINFO"
#define TOKEN_14_ACK_RSINFO "ACK_RSINFO"
#define TOKEN_15_ACK_KEDIAO_INFO "ACK_KEDIAO_INFO"
#define TOKEN_16_ACK_YINGYE_INFO "ACK_YINGYE_INFO"
#define TOKEN_17_ACK_SG_WX_INFO "ACK_SG_WX_INFO"
#define TOKEN_18_ACK_WKDES "ACK_WKDES"
#define TOKEN_19_ACK_BASICSCHEDULE  "ACK_BASICSCHEDULE"

#define TOKEN_20_ACK_CREW_INFO "ACK_CREW_INFO"
#define TOKEN_21_CREW_PLAN_INFORM "CREW_PLAN_INFORM"

#define TOKEN_22_ACK_STN_PLAN "ACK_STN_PLAN"
#define TOKEN_23_ACK_SPEC_PLAN "ACK_SPEC_PLAN"
#define TOKEN_24_ACK_FTN_PLAN "ACK_PHASE_PLAN"
#define TOKEN_25_AD_PLAN_INFORM "AD_PLAN_INFORM"
#define TOKEN_26_ACK_SPEC_UPDATE_PLAN "ACK_SPEC_UPDATE_PLAN"
#define TOKEN_30_ACK_PASSENGER_TICKET_INFO "ACK_PASSENGER_TICKET_INFO"
#define TOKEN_31_ACK_ENGINE_CREW_INFO "ACK_ENGINE_CREW_INFO"

#define TOKEN_SECTIONPLAN_DCMD_RESULT  "SECTIONPLAN_DCMD_RESULT"


#define TOKEN_SS_PLAN_INFORM  "SS_PLAN_INFORM"
#define TOKEN_KD_PLAN_INFORM  "KD_PLAN_INFORM"
#define TOKEN_JH_PLAN_INFORM  "JH_PLAN_INFORM"
//<JH_PLAN_INFORM><JHTNM>9</JHTNM><JHID>0</JHID><COUNT>36</COUNT><GTID>65461,66030,65484,65481,65558,65446,65559,66025,-1,65926,65927,65918,65930,-1,-1,66007,66785,65453,66784,65439,65432,65482,66783,65472,66782,65448,66015,-1,65441,65475,65443,-1,65458,65456,65451,65444,</GTID>
//<STTM>2010-07-24T13:11:00</STTM><ENDTM>2010-07-24T16:11:00</ENDTM>
//</JH_PLAN_INFORM>
BOOL  TD_GenRequestMessage_1_BZ(CString &token,CString &dest,DWORD id,int stno,const char *stname,int flag);
BOOL  TD_GenRequestMessage_2_YL_BY_ID(CString &token,CString &dest,int con_id,const char *stname,int cnt,DWORD ids[]);
BOOL  TD_GenRequestMessage_3_YL_BY_TIME(CString &token,CString &dest,int con_id,time_t starttm,time_t endtm,BOOL onlyOrigStart,BOOL Passenger,BOOL freight,const char *stname);
BOOL  TD_GenRequestMessage_4_YL_BY_TRAINNUM(CString &token,CString &dest,int con_id,time_t starttm,time_t endtm,const char *trainno);
BOOL  TD_GenRequestMessage_5_YL_BY_TRAINNUM_TIME(CString &token,CString &dest,int con_id,time_t starttm,const char *trainno);
BOOL  TD_GenRequestMessage_6_GID_BY_TRAINNUM(CString &token,CString &dest,int con_id,time_t starttm,time_t endtm,const char *trainnos); //多个车次在trainnos中逗号隔开
BOOL  TD_GenRequestMessage_7_TFG(CString &token,CString &dest,int con_id,DWORD gtid);
BOOL  TD_GenRequestMessage_8_NEWYL(CString &token,CString &dest,int con_id,int cnt,DWORD ids[]);
BOOL  TD_GenRequestMessage_9_SEND_PLAN(CString &token,CString &dest,int con_id,const char *jht,DWORD msgid,const char *msgdata);
BOOL  TD_GenRequestMessage_10_SEND_ADPLAN(CString &token,CString &dest,int con_id,int cnt,DWORD ids[]);
BOOL  TD_GenRequestMessage_12_CMD(CString &token,CString &dest,int con_id,DWORD nDay,const char *id);
BOOL  TD_GenRequestMessage_13_WKINFO(CString &token,CString &dest,int con_id,time_t onlydate);
BOOL  TD_GenRequestMessage_14_RSINFO(CString &token,CString &dest,int con_id,time_t onlydate);
BOOL  TD_GenRequestMessage_15_KEDIAO_INFO(CString &token,CString &dest,DWORD gtid);
BOOL  TD_GenRequestMessage_16_YINGYE_INFO(CString &token,CString &dest,DWORD gtid);
BOOL  TD_GenRequestMessage_17_SG_WX_INFO(CString &token,CString &dest,int con_id,DWORD nDay, const char *id,int flag); // flag 0:施工 1:维修
BOOL  TD_GenRequestMessage_18_WKDES(CString &token,CString &dest,int con_id,DWORD nDay,const char *id); 
BOOL  TD_GenRequestMessage_19_BASICSCHEDULE(CString &token,CString &dest,int con_id,int type); // type :0 预览 1: 在用

BOOL  TD_GenRequestMessage_20_CREW_INFO(CString &token,CString &dest,DWORD gtid);

//回执 type 0:施工符号 1:单趟车 2:乘务信息 3:特运信息   flag 0:失败 1:成功
BOOL  TD_GenAckMessage_GPCACKINFO(CString &token,CString &dest,int con_id,int type,bool flag,CString name,long src, CString msgid, CString jht, CString result="");
/////////////////// 
BOOL  GenXMLMessage_InformDCMDToFront(CString &token,CString &dest);
BOOL  GenXMLMessage_InformDCMDUpdated(CString &token,CString &dest,time_t starttm,time_t endtm);
BOOL  GenXMLMessage_SelectDCMD(CString &token,CString &dest,time_t starttm,time_t endtm,const char *planid);
BOOL  GenXMLMessage_InformDCMDLogin(CString &token,CString &dest,const char *username);

bool TGSendStringMessage(const char *token,const char *content,long eid);