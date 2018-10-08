#ifndef _PMTG_TIME_HEADER
#define _PMTG_TIME_HEADER
#include "defines.h"
typedef struct _TGTIME {
	long day;
	USHORT hour;
	USHORT minute;
} TGTIME;

extern USHORT GetTwoHourAxisOffset(TGTIME tgTime);
extern USHORT GetHalfHourAxisOffset(TGTIME);
extern USHORT GetOneHourAxisOffset(TGTIME);
extern USHORT GetTenMinuteAxisOffset(TGTIME);
extern USHORT GetTimeStringOffset(TGTIME *tgTime, SHORT sDispMode);
extern VOID   TGClock(TGTIME *);
extern VOID   AddHour(TGTIME *, SHORT);
extern VOID   AddMinute(TGTIME *, SHORT);
extern USHORT TGtimeConverttoMinute(TGTIME);
extern VOID   MinuteConvertoTGtime(USHORT usMinute, TGTIME *tgTime);

#endif
