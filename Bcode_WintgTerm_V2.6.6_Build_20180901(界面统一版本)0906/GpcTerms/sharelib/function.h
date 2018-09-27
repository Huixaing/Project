
#ifndef _FUNCTION_HEADER_
#define _FUNCTION_HEADER_
#include "stdafx.h"
#include "defines.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

//#define max(x,y) (x > y) ? x:y
//#define min(x,y) (x < y) ? x:y

#include <oscalls.h>

#define TIME_RANGE_OVERLAPED	1
#define TIME_RANGE_SEPERATED	2
#define TIME_RANGE_EQUALIZED	3
#define TIME_RANGE_INCLUDE		4
#define TIME_RANGE_DURING		5

#define TIME_EARLY				1
#define TIME_EQUAL				2
#define TIME_LATE				3

extern int isTimeRangeOverlap(int usStartDate1,int usStartTime1,
	int usEndDate1,int usEndTime1,
	int usStartDate2,int usStartTime2,
	int usEndDate2,int usEndTime2,int *nTimeFlag);

extern const char *OsConvertDatetoString(long uValue);
extern const char *OsConvertTimetoString(long uValue);
extern CString ChangeFromHalf2Full(CString &input);
extern CString ChangeAlpha(CString &input);
extern long OsGetNextDate(ushort uProcessId,long uCurrentDate);
extern long OsGetPrevDate(USHORT uProcessId,long uCurrentDate);
extern long OsConvertStringtoDate(const char *theString);
extern long OsGetCurrDate(USHORT uProcessId);
extern long OsGetCurrTime(USHORT uProcessId);
extern long OsConvertStringtoTime(const char *theString);

extern BOOL isNumberInIntArray(int usNumber,int *usArray,int usCount);
extern BOOL isNumberInShortArray(ushort usNumber,ushort *usArray,ushort usCount);

extern short OsGetTwoDateOffset(ushort uProcessId,long usFirstDate,long usSecondDate);
extern short OsMemoryCompare(char *pbSource,char *pbDestine,ushort usLength);
extern short OsMemoryCopy(char *pbDestine,char *pbSource,ushort usLength);
extern short OsStringCopy(char *pbDestine,char *pbSource);
extern double distance(int x1,int y1,int x2,int y2);
extern double DisFromPointToLine(int x1,int y1,int x2,int y2,int x,int y);
extern long  minute_time(long long_time, long *date);
extern long long_time(long date, long ttime);
extern int IsAllSpace(const char *pbString);
extern int OsConvertTimestamptoTimet(char *timestamp);
extern char *OsConvertTimettoTimestamp(time_t nValue);
extern char *TrimSpace(char *pbString);
extern char *TrimRightSpace(char *pbString);
extern void Draw3DRect(CDC * pCDC, CRect &rect, COLORREF clrTopLeft, COLORREF clrBottomRight,int nWidth);
//extern void Draw3DRect(CDC * pCDC, CRect &rect, int nWidth, int nFlag);
extern uint NettoHostUInteger(uint usInputValue);
extern int NettoHostInteger(int usInputValue);
extern short NettoHostShort(short usInputValue);
extern ushort NettoHostUShort(ushort usInputValue);

/*************************************************************/
extern unsigned long  NettoHostULong(unsigned long  usInputValue);
extern float NettoHostFloat(float usInputValue);
/* 定义用户信息 added by Wang Jinhai */

extern void ResizeWndToFixSize(CWnd* pWnd, INT cx, INT cy);

unsigned long Get_PreShiftid(unsigned long CurShiftid);
unsigned long Get_NextShiftid(unsigned long CurShiftid);
unsigned long  Get_CurrentShiftid();
SHORT GetConfigLine(FILE *fptr, char buf[], USHORT bufsize);
ushort GetParmValue(char *buffer,char  * field_name,BYTE field_type, void *field_add, USHORT size);
const char* StringDate(long ltime);
const char* StringTime(long ltime);
const char* BasicStringTime(int ltime);
void ReplaceSubStr(char *str,const char *key,const char *content); // must strlen<256 

time_t  ConvertToLongTime(time_t t,time_t reftm,int hour_range_left,int hour_range_right); //按照时分计算合理的time_t, reft 是参考时间范围
time_t  ConvertToLongTime(int hour,int min,int sec,time_t reftm,int hour_range_left,int hour_range_right); //按照时分计算合理的time_t, reft 是参考时间范围
#endif
