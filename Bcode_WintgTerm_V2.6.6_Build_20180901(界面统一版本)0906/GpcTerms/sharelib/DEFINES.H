#ifndef _user_DEFINES_HEADERS

/* tell every one we included this one */
#define _user_DEFINES_HEADERS

#include "stdafx.h"

/* variable types to be passed arround */
#define  TYPE_BYTE         00
#define  TYPE_SHORT        01
#define  TYPE_LONG         02
#define  TYPE_BYTE_ARRAY   03
#define  TYPE_SHORT_ARRAY  04
#define  TYPE_LONG_ARRAY   05
#define  TYPE_STRING       06
#define  TYPE_STRING_ARRAY 07

#define RET_FAILED		-1
#define RET_SUCCESS		0

#ifndef  ERROR
#define  ERROR             -1
#endif

#define  OK                0

#define DATABASE_CONNECTED      1
#define DATABASE_CONNECTING     2
#define DATABASE_UNCONNECT      3

#define MAX_STATIONS_PER_SECTION     128
/* for IBM cset/2 this is already defined */
#ifndef _Packed
#define _Packed
#endif

#ifndef Pstruct
#define Pstruct struct
#endif

#ifndef Punion
#define Punion  union
#endif

#ifndef ushort
typedef unsigned short ushort;
//#define ushort unsigned short
#endif


#ifndef ulong
typedef unsigned long ulong;
#endif

#ifndef uint
typedef unsigned int uint;
#endif

#ifndef PROCESSID
#define PROCESSID USHORT
#endif

#ifndef byte
typedef unsigned char byte;
#endif

/*
#ifndef SHORT
#define SHORT short
#endif

#ifndef VOID
#define VOID  void
#endif

#ifndef LONG
#define LONG long
#endif

#ifndef ULONG
typedef unsigned long ULONG;
#endif

#ifndef USHORT
typedef unsigned short USHORT;
#endif

#ifndef BOOL
#define BOOL	int
#endif

#ifndef BYTE
typedef unsigned char BYTE;
#endif

typedef void *HANDLE;
*/
typedef Pstruct _DATE_TIME {
	BYTE   hours;
	BYTE   minutes;
	BYTE   seconds;
	BYTE   hundredths;
	BYTE   day;
	BYTE   month;
	USHORT  year;
	SHORT   timezone;
	BYTE   weekday;
} DATE_TIME;

typedef  Pstruct _QUEUERESULT {
	USHORT pidProcess;
	USHORT usEventCode;
} QUEUERESULT;


typedef Pstruct _RES_SEM {
	USHORT SemHandle;
	BYTE   LockFlag;
	BYTE   spare;
} RES_SEM;

Pstruct _tagDatabaseConfig {
	int ws_id;
	int bRollbackFlag;
	int bConnectFlag;
	char dbname[64];
	char dbusername[32];
	char dbpassword[32];
	char normal_schedule[32];
	char schedule_change[32];
	char icon_change[32];
	char electronic_change[32];
	RES_SEM hSem;
};

typedef struct _APP_STATUS {
    BYTE   AppName[32];
    BYTE   State;                 /* 0:Down, 1:active 2: active local */
    BYTE   LinkCount;             /* total links so far */
    } APP_STATUS;

typedef struct _SOCKET_STATUS {
    USHORT  Error;
    BYTE    State;
    BYTE    RcvMsgCount;
    BYTE    XmitWaiting;
    } SOCKET_STATUS;
#endif
