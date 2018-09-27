#ifndef TGMSG_HEADER
#define TGMSG_HEADER
#include "oscalls.h"
#include "dbfunc.h"

#define UM_TGMSG_ADDTRAIN		(WM_USER + 1)
#define UM_TGMSG_SCHDCHANGE	    (WM_USER + 2)
#define UM_TGMSG_SIDEINHIBIT	(WM_USER + 3)
#define UM_TGMSG_ABSINHIBIT		(WM_USER + 4)
#define UM_TGMSG_ABSSLOW		(WM_USER + 5)
#define UM_TGMSG_TRAININFO		(WM_USER + 6)
#define UM_TGMSG_SCHDSTATUS		(WM_USER + 7)
#define UM_TGMSG_WEATHERINFO	(WM_USER + 8)
#define UM_TGMSG_STATIONINFO	(WM_USER + 9)
#define UM_TGMSG_HISTORY_ADD	(WM_USER + 10)
#define UM_TGMSG_CONFLICT_ADD	(WM_USER + 11)
#define UM_TGMSG_CONFLICT_DELETE (WM_USER + 12)
#define UM_TGMSG_HISTORYTRAIN	(WM_USER + 13)
#define UM_PLOT_ADDTRAIN		(WM_USER + 14)
#define UM_INFORM_SCHDSTATUS	(WM_USER + 15)
//2002/4/4
#define UM_TGMSG_TRAIN_INFO		(WM_USER + 16)
 

#define UM_SHIFT_COMMAND_MSG	(WM_USER + 20)
#define UM_STATION_CARS_MSG		(WM_USER + 30)

#define UM_TGMSG_SIDESLOW   	(WM_USER + 31)
#define UM_TGMSG_ABSTASK    	(WM_USER + 32)
#define UM_TGMSG_TEXT_NODE    	(WM_USER + 33)

#define  UM_SIDE_OCCUPY_MSG      (WM_USER + 40)
#define  UM_ABS_TIME_MSG         (WM_USER + 41)
#define  UM_INTERVAL_TIME_MSG    (WM_USER + 42)

////////////////////////////////////////////////////////////////////////////////
#define  UM_LOGIN_MSG            (WM_USER + 43)  
#define  UM_NOTEBOOK_MSG         (WM_USER + 44)  
#define  UM_SUBAO_MSG            (WM_USER + 45)  
#define  UM_CHANGE_PASSWORD_MSG  (WM_USER + 46)  
#define  UM_GIVE_DDY_NAME_MSG    (WM_USER + 47)  

#define  UM_AUTHORITY_MSG        (WM_USER + 48) //added by Qianguorong 2004-03-25
/////////////////////////////////////////////////////////////////////////
//DMIS CAPTURE
#define  UM_STATION_RECV_STATUS_MSG    (WM_USER + 50)
#define  UM_TGMSG_MARK_ITEM_INFO_MSG   (WM_USER + 51)
#define  UM_DEFINE_BASIC_GRAPHIC_MSG   (WM_USER + 52)
#define  UM_NEIGHBER_RECV_STATUS_MSG   (WM_USER + 54)

#define  UM_STATION_CARS_MSG_FLAG      (WM_USER + 56)

#define UM_TCC_INITCOMPLETE			(WM_USER + 57)

#define UM_STATUS_BAR_UPDATE		(WM_USER + 58)
#define UM_TG_PROPERTY_UPDATE		(WM_USER + 59)
#define UM_TG_OBJECT_CHANGED		(WM_USER + 60)

#define UM_TG_TRAIN_SELECTED		(WM_USER + 61)
struct  TRAIN_SELECT_PARA{
	    short    section_no;
	    short    station_no;
        char     train_type; //1 : passenger 2 freight  3 all
		char     list_type;  // 0:simple trainid list 1:traininfo list
	    DWORD    train_index;
		time_t   starttime;
		time_t   endtime;
		TRAIN_SELECT_PARA()
		{
			section_no=0;
			station_no=0;
			train_type=0;
			train_index=0;
			starttime=0;
			endtime=0;
			list_type=0;
		}
};

#define UM_TG_XML_MSG     		(WM_USER + 62)

struct XML_MSG_PARA{
	CString token;
	CString content;
};

#define UM_TG_MPLAN_SELECTED	(WM_USER + 63)

struct MPLAN_SELECT_PARA{
	CString id;
};

#define UM_MEMORY_NOIFY	        (WM_USER + 64)

#define UM_GET_FRAME_TYPE       (WM_USER + 1000)
#define UM_SET_FRAME_ACTIVE     (WM_USER + 1001)

//机调窗口用的托盘消息
#define UM_JIDIAO_ICON WM_USER+55
 
#define LINE_OPER_DELETE	0x0001
#define LINE_OPER_ADD		0x0002
#define LINE_OPER_INSERT	0x0002
#define LINE_OPER_CHANGE	0x0004
#define LINE_OPER_ORIGINAL	0x1000
#define LINE_OPER_SHOWING	0x0100

#define OPER_NOTHING    0x0000
#define OPER_ADD	    0x0001
#define OPER_DELETE		0x0002
#define OPER_MODIFY 	0x0003
#define OPER_INIT    	0x0004
#define OPER_ADJUST    	0x0005
 
#define    STPC_STATUS						  0x06d0

int WriteMessage(PROCESSID pid,ushort usQueueHandle,
				 ushort usCount,char *pbBuffer);

typedef struct _tagReportCommand {
	int nCommand;  
	int nSectionNo;
	ushort usFirstStationNo,usLastStationNo;
	time_t tStartTime;
	time_t tEndTime;
} REPORTCOMMAND;

typedef struct _tagLoadQueueEle {
	ushort usOperCode;
	ushort usDestineCode;
	time_t tOperTime;
	REPORTCOMMAND *other;
}  LOADQUEUEELE;
 
typedef struct _tagLoadQueueInfo {
	LOADQUEUEELE ele;
	int nOrder;
	struct _tagLoadQueueInfo *prev,*next;
} LOADQUEUEINFO;

int OsLoadQueueInit( );
int OsLoadQueueInsertEle(LOADQUEUEELE *rec);
int OsLoadQueueQueryEle(LOADQUEUEELE *rec);
int OsLoadQueueDeleteEle(LOADQUEUEELE *rec);
int OsLoadQueuePurgeEle( );
int OsLoadQueueInsertEle(ushort usOperCode,
	ushort usDestineCode,
	time_t tOperTime,
	REPORTCOMMAND *other);
int OsLoadQueueSearchEle(ushort usOperCode,
	int nCommand,
	LOADQUEUEELE *rec);
bool LocalTGRequestMessageProcess(unsigned char *ptr,int length);

int SendTGCommMessage(int section_id,ushort usMsgSize,const char *abBufferMsg,long dest_entity=0); // 默认发往tg_svr,当dest_entity==0
bool SendTGBMsg(int section_id,CLBTH::BaseMsg* pOneMsg,long dest_entity,short flag=0); // 默认以本机ID,自动LABEL,不带序号发出
 
#endif