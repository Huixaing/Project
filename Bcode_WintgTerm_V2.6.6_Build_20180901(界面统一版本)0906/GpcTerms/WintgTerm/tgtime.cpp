#define INCL_WIN
#define INCL_GPI


#include <time.h>
#include "oscalls.h"
#include "tgtime.h"
#include "tgtype.h"

USHORT GetTwoHourAxisOffset(TGTIME tgTime);
USHORT GetHalfHourAxisOffset(TGTIME);
USHORT GetOneHourAxisOffset(TGTIME);
USHORT GetTenMinuteAxisOffset(TGTIME);
USHORT GetTimeStringOffset(TGTIME *tgTime, SHORT sDispMode);
VOID   TGClock(TGTIME *);
VOID   AddHour(TGTIME *, SHORT);
VOID   AddMinute(TGTIME *, SHORT);
USHORT TGtimeConverttoMinute(TGTIME);
VOID   MinuteConvertoTGtime(USHORT usMinute, TGTIME *tgTime);
VOID   TGTimerRunning(void);

extern BOOL    fProlongTime;

USHORT GetTimeStringOffset(TGTIME *tgTime, SHORT sDispMode)
{
	USHORT rc=0;

	switch (sDispMode) {
		case FOUR_HOUR_DISPLAY:
			rc = tgTime->hour * 2 + ((tgTime->minute >= 30)? 1:0);
			if ((tgTime->minute != 0) && (tgTime->minute != 30))
				rc += 1;
		break;
		case EIGHT_HOUR_DISPLAY:
			rc = tgTime->hour * 2;
			if (tgTime->minute != 0)
				rc += 2;
		break;
		case TWELVE_HOUR_DISPLAY :
			rc = tgTime->hour * 2;
			if((tgTime->hour % 2) != 0)
				rc += 2;
			else if(tgTime->minute != 0)
				rc += 4;
		break;
		case TWENTY_FOUR_HOUR_DISPLAY:
			rc = 0;
		break;
	}
	return (rc);
}

VOID   AddHour(TGTIME *ptgt, SHORT sHour)
{
	SHORT sh, sd;

	sh = ((*ptgt).hour + sHour)%24;
	sd = ((*ptgt).hour + sHour)/24;
	if (sh < 0)
		sh += 24;
	(*ptgt).hour = sh;
	(*ptgt).day += sd;
}

VOID AddMinute(TGTIME *ptgt, SHORT sMinute)
{
	SHORT sh, sm, sSumMinute;

	sSumMinute = ((*ptgt).minute + sMinute);
	sh = sSumMinute / 60;
// add by song
	if(sh != 0)
		AddHour(ptgt, sh);

	sm = sSumMinute % 60;
	if (sm < 0) {
		AddHour(ptgt, -1);
		sm += 60;
	}
	(*ptgt).minute = sm;
}

USHORT GetHalfHourAxisOffset(TGTIME tgTime)
{
	USHORT usrc;

	if (tgTime.minute < 30)
		usrc = tgTime.minute / 2;
	else
		usrc = (tgTime.minute - 30) / 2;

	if (usrc == 0 && (tgTime.minute == 0 || tgTime.minute == 30))
		return 0;
	else
		return (15 - usrc);
}

USHORT GetOneHourAxisOffset(TGTIME tgTime)
{
	USHORT usrc;

	usrc = tgTime.minute / 2;
	if (usrc == 0 && tgTime.minute == 0)
		return 0;
	return (30 - usrc);
}

USHORT GetTwoHourAxisOffset(TGTIME tgTime)
{
	USHORT usrc;

	usrc = ((tgTime.hour % 2) * 60 + tgTime.minute) / 2;
	if(usrc == 0 && tgTime.minute == 0)
		return 0;
	return (60 - usrc);
}

USHORT GetTenMinuteAxisOffset(TGTIME tgTime)
{
    USHORT usrc;

    usrc = (tgTime.minute % 10) / 2;

    return (5 - usrc);
}

VOID TGClock(TGTIME *ptgt)
{
	(*ptgt).minute += 2;
	if ((*ptgt).minute == 60) {
		(*ptgt).minute = 0;
		(*ptgt).hour ++;
		if ((*ptgt).hour == 24) {
			(*ptgt).hour = 0;
			(*ptgt).day++;
			}
		}
}

USHORT TGtimeConverttoMinute(TGTIME tgTime)
{
	USHORT rc;

	rc = tgTime.hour * 60 + tgTime.minute;
	return (rc);
}

VOID MinuteConvertoTGtime(USHORT usMinute, TGTIME *tgTime)
{
	(*tgTime).day = (usMinute / (60 * 24) ) % 365;
	(*tgTime).hour = (usMinute / 60 ) % 24;
	(*tgTime).minute = usMinute % 60;
}

VOID TGTimerRunning(void)
{
	return;
//    DosBeep(261, 100);
}
