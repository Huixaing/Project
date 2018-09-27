
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
 
#include <oscalls.h>
#include "function.h"
#include "memshare.h"

//#define HIGH_BYTE_FIRST    //  for AIX UNIX CAD 
#define DRAW_WIDEN_LINE_BY_POINT

void Draw3DRect(CDC * pCDC, CRect &rect, COLORREF clrTopLeft, COLORREF clrBottomRight,int nWidth);
void Draw3DRect(CDC * pCDC, CRect &rect, int nWidth, int nFlag);


const char *OsConvertDatetoString(long uValue);
const char *OsConvertTimetoString(long uValue);
long OsGetNextDate(USHORT uProcessId,long uCurrentDate);
long OsGetPrevDate(USHORT uProcessId,long uCurrentDate);
long OsGetCurrDate(USHORT uProcessId);
long OsGetCurrTime(USHORT uProcessId);
long OsConvertStringtoDate(const char *theString);
long OsConvertStringtoTime(const char *theString);
short OsConvertIntegertoDateTime(time_t lTimeValue,
	long *uDate,long *uTime);
short OsGetTwoDateOffset(ushort uProcessId,long usFirstDate,long usSecondDate);
BOOL isNumberInShortArray(ushort usNumber,ushort *usArray,ushort usCount);
BOOL isNumberInIntArray(int usNumber,int *usArray,int usCount);
short OsGetTrainDirectionFlag(char *abTrainId);
short OsMemoryCompare(char *pbSource,char *pbDestine,ushort usLength);
short OsMemoryCopy(char *pbDestine,char *pbSource,ushort usLength);
short OsStringCopy(char *pbDestine,char *pbSource);
int IsAllSpace(const char *pbString);
int OsConvertTimestamptoTimet(char timestamp);
CString ChangeFromHalf2Full(CString &input);
CString ChangeAlpha(CString &input);
   
char *GetStringNextItem(char *szSrc,char cBreak);
int ReadProfile(FILE *fp,char *szSectionName,char *szKeyword,char *szBuffer);
int LocateKeyword(FILE *fp,char *szString,char *szBuffer);
int LocateSection(FILE *fp,char *szString);
char *ReadStringFromFile(char * abBuffer, int nLength, FILE * fp);
 

const char *OsConvertDatetoString(long uValue)
{
	static char buff1[12],buff2[12];
	short uYear,uMonth,uDate;
	static int nFlag = 0;
	char *string;

	if(nFlag == 0)
	{
		string = buff1;
		nFlag = 1;
	}else {
		string = buff2;
		nFlag = 0;
	}
	memset(string,0x20,12);
	if(uValue != -1)
	{
		uYear = uValue / 380 + 1990;
		uMonth = (uValue % 380) / 31 + 1;
		uDate = (uValue % 380) % 31 + 1;
		sprintf(string,"%02d/%02d/%04d",uMonth,uDate,uYear);
		string[10] = 0x00;
	} else {
		strcpy(string,"--:--:--");
	}
	return string;
}

const char *OsConvertTimetoString(long uValue)
{
	static char buff1[12],buff2[12];
	short uHour,uMinute,uSecond;
	char *string;
	static int nFlag = 0;

	if(nFlag == 0)
	{
		string = buff1;
		nFlag = 1;
	}else {
		string = buff2;
		nFlag = 0;
	}
	memset(string,0x20,12);
	if(uValue != -1)
	{
		uHour = (uValue / 60) / 60;
		if(uHour < 10)
		{
			string[0] = 0x30;
			string[1] = 0x30 + uHour;
		}else {
			string[0] = 0x30 + uHour/10;
			string[1] = 0x30 + uHour%10;
		}
		string[2] = ':';

		uMinute = (uValue / 60) % 60;
		if(uMinute < 10)
		{
			string[3] = 0x30;
			string[4] = 0x30 + uMinute;
		}else {
			string[3] = 0x30 + uMinute/10;
			string[4] = 0x30 + uMinute%10;
		}
		uSecond = uValue % 60;
		if(uSecond != 0)
		{
			string[5] = ':';

			uSecond = uValue % 60;
			if(uSecond < 10)
			{
				string[6] = 0x30;
				string[7] = 0x30 + uSecond;
			}else {
				string[6] = 0x30 + uSecond/10;
				string[7] = 0x30 + uSecond%10;
			}
			string[8] = 0x00;
		} else 
			string[5] = 0x00;
	} else 
		strcpy(string,"--:--");
	return string;
}

long OsGetNextDate(ushort uProcessId,long uCurrentDate)
{
	struct tm *tp,st;
	time_t timeval;
	long uNextDate;

	if(uCurrentDate < 0)
		return -1;
	else if(uCurrentDate >= 0 && uCurrentDate <= 5)
		uNextDate = uCurrentDate + 1;
	else if(uCurrentDate > 5){
		st.tm_year = uCurrentDate/380 + 90;
		st.tm_mon = (uCurrentDate%380)/31;
		st.tm_mday = (uCurrentDate%380)%31 + 1;
		st.tm_hour = 12;
		st.tm_min = 0;
		st.tm_sec = 0;
		st.tm_isdst = -1;

		/********************************************
		sysprint(0,uProcessId,"[TG]: current date is %d year %d mon %d date",
			tp->tm_year + 1900,tp->tm_mon + 1,tp->tm_mday);
		*********************************************/
		timeval = mktime(&st);
		timeval += 86400;
		if((tp = localtime(&timeval)) == NULL)
		{
			sysprint(0,uProcessId,
				"[%2d]: mallocate memory failed in file %s at line %d",
				uProcessId,__FILE__,__LINE__);
			return -1;
		}

		/************************************
		sysprint(0,uProcessId,"[TG]: next date is %d year %d mon %d date",
			tp->tm_year+1900,tp->tm_mon+1,tp->tm_mday);
		************************************/
		uNextDate = (tp->tm_year - 90)*380 +
			(tp->tm_mon)*31 + tp->tm_mday - 1;
	}
	return uNextDate;
}

long OsGetPrevDate(USHORT uProcessId,long uCurrentDate)
{
	struct tm *tp,st;
	time_t timeval;
	long uPrevDate;
	
	if(uCurrentDate < 0)
		return -1;
	else if(uCurrentDate >= 1 && uCurrentDate <= 5)
		uPrevDate = uCurrentDate - 1;
	else if(uCurrentDate == 0)
		uPrevDate = 0;
	else if(uCurrentDate > 5){
		st.tm_year = uCurrentDate/380 + 90;
		st.tm_mon = (uCurrentDate%380)/31;
		st.tm_mday = (uCurrentDate%380)%31 + 1;
		st.tm_hour = 12;
		st.tm_min = 0;
		st.tm_sec = 0;
		st.tm_isdst = -1;

		/******************************
		sysprint(0,uProcessId,"[TG]: current date is %d year %d mon %d date",
			tp->tm_year+1900,tp->tm_mon+1,tp->tm_mday);
		******************************/
		timeval = mktime(&st);
		timeval -= 86400;
		if((tp = localtime(&timeval)) == NULL)
		{
			sysprint(0,uProcessId,
				"[%2d]: %d mallocate memory failed in file %s at line %d",
				uProcessId,uCurrentDate,__FILE__,__LINE__);
			return -1;
		}
		/**********************************
		sysprint(0,uProcessId,"[TG]: prev date is %d year %d mon %d date",
			tp->tm_year+1900,tp->tm_mon+1,tp->tm_mday);
		**********************************/
		uPrevDate = (tp->tm_year - 90)*380 +
			(tp->tm_mon)*31 + tp->tm_mday - 1;	
	}
	return uPrevDate;
}

long OsGetCurrDate(USHORT uProcessId)
{
	struct tm *tp;
	time_t timeval;
	long uCurrDate;

	time(&timeval);
	if((tp = localtime(&timeval)) == NULL)
	{
		sysprint(0,uProcessId,
			"[%2d]: mallocate memory failed in file %s at line %d",
			uProcessId,__FILE__,__LINE__);
		return -1;
	}
	uCurrDate = (tp->tm_year - 90)*380 +
		(tp->tm_mon)*31 + tp->tm_mday - 1;
	return uCurrDate;
}

long OsGetCurrTime(USHORT uProcessId)
{
	struct tm *tp;
	time_t timeval;
	long uCurrTime;

	time(&timeval);
	if((tp = localtime(&timeval)) == NULL)
	{
		sysprint(0,uProcessId,
			"[%2d]: mallocate memory failed in file %s at line %d",
			uProcessId,__FILE__,__LINE__);
		return -1;
	}
	uCurrTime = (tp->tm_hour * 60 + tp->tm_min) * 60 + tp->tm_sec;
	return uCurrTime;
}

short OsConvertIntegertoDateTime(time_t lTimeValue,
	long *uDate,long *uTime)
{
	struct tm *tp;

	if((tp = localtime(&lTimeValue)) == NULL)
		return -1;
	if(uDate != NULL)
		*uDate = (tp->tm_year - 90) * 380 +
			tp->tm_mon * 31 + tp->tm_mday - 1;
	if(uTime != NULL)
		*uTime = (tp->tm_hour * 60 + tp->tm_min) * 60 + tp->tm_sec;
	return 0;
}
long OsConvertStringtoDate(const char *theString)
{
	char theStatus = 1;
	long uValue = 0;
	unsigned short uTempValue=0; 
	char *ptr;
	char string[12];

	do {
		if(theString == NULL || strlen(theString) == 0)
			break;
		if(strlen(theString) > 11)
			break;
		strcpy(string,theString);
		if((ptr = strtok(string,"/-:;.,")) == NULL)
			break;
		if(sscanf(ptr,"%hu",&uTempValue) != 1)
			break;
		if(uTempValue > 12 || uTempValue == 0)
			break;
		uTempValue -= 1;
		uValue = uTempValue * 31;
		if((ptr = strtok(NULL,"/-:;.,")) == NULL)
			break;
		if(sscanf(ptr,"%hu",&uTempValue) != 1)
			break;
		if(uTempValue > 31 || uTempValue == 0)
			break;
		uValue += uTempValue - 1;
		if((ptr = strtok(NULL,"/-:;.,")) == NULL)
			break;
		if(sscanf(ptr,"%hu",&uTempValue) != 1)
			break;
		if(uTempValue < 1900)
			uValue += (uTempValue - 90) * 380;
		else
			uValue += (uTempValue - 1990) * 380;
		theStatus = 0;
	}while(0);
	if(theStatus)
		return -1;
	else return uValue;
}

/******************************************
	date format hour separator minute
*******************************************/
long OsConvertStringtoTime(const char *theString)
{
	char theStatus = 1;
	long uValue = 0;
	unsigned short  uTempValue; 
	char *ptr;
	char string2[12];

	do {
		if(theString == NULL || strlen(theString) == 0)
			break;
		if(strlen(theString) > 9)
			break;
		strcpy(string2,theString);
		if((ptr = strtok(string2,"/:-;.,")) == NULL)
			break;
		if(sscanf(ptr,"%hu",&uTempValue) != 1)
			break;
		if(uTempValue > 23)
			break;
		uValue = uTempValue * 3600;

		if((ptr = strtok(NULL,"/:-;.,")) == NULL)
			break;
		if(sscanf(ptr,"%hu",&uTempValue) != 1)
			break;
		if(uTempValue > 59)
			break;
		uValue += uTempValue * 60;

		if((ptr = strtok(NULL,"/:-;.,")) != NULL)
		{
			if(sscanf(ptr,"%hu",&uTempValue) != 1)
				break;
			if(uTempValue > 59)
				break;
			uValue += uTempValue;
		}

		theStatus = 0;
	}while(0);
	if(theStatus)
		return -1;
	else return uValue;
}
/*------------------------------------------------
	parameter: ushort usFirstDate
		ushort usSecondDate;
	return value equal usFirstDate - usSecondDate;
	if(usFirstDate equals usSecondDate)
		return value is ZERO
	if(usFirstDate great than usSecondDate)
		return value great than ZERO
	if(usFirstDate less than usSecondDate)
		return value less than ZERO
-------------------------------------------------*/
short OsGetTwoDateOffset(ushort uProcessId,long usFirstDate,long usSecondDate)
{
	short sDiff = 0;
	long i;

	if((int)(usFirstDate - usSecondDate) > 10)
	{
//		sysprint(0,uProcessId,"[warning]; offset over range");
		return 255;
	}
	if((int)(usFirstDate - usSecondDate) < -10)
	{
//		sysprint(0,uProcessId,"[warning]; offset over range");
		return -255;
	}
	if(usFirstDate > usSecondDate)
	{
		i = usSecondDate;
		do {
			if((i = OsGetNextDate(uProcessId,i)) == -1)
				return -1;
			sDiff += 1;
		}while(i < usFirstDate);
	} else if(usFirstDate < usSecondDate) {
		i = usFirstDate;
		do {
			if((i = OsGetNextDate(uProcessId,i)) == -1)
				return -1;
			sDiff -= 1;
		}while(i < usSecondDate);
	}
	return sDiff;
}

BOOL isNumberInShortArray(ushort usNumber,ushort *usArray,ushort usCount)
{
	short i;

	if(usArray == NULL || usCount == 0)
		return FALSE;
	for(i=0;i < usCount;i++)
	{
		if(usArray[i] == usNumber)
			return TRUE;
	}
	return FALSE;
}

BOOL isNumberInIntArray(int usNumber,int *usArray,int usCount)
{
	int i;

	if(usArray == NULL || usCount == 0)
		return FALSE;
	for(i=0;i < usCount;i++)
	{
		if(usArray[i] == usNumber)
			return TRUE;
	}
	return FALSE;
}

short OsGetTrainDirectionFlag(char *abTrainId)
{
	short uLength,i;
	if(abTrainId == NULL ||
		(uLength = strlen(abTrainId)) == 0)
		return 0;
	for(i=uLength;i > 0;i--)
	{
		if(abTrainId[i] > 0x30 && abTrainId[i] < 0x39)
		  return(abTrainId[i] % 2);
	}
	return 0;
}

short OsStringCopy(char *pbDestine,char *pbSource)
{
	short  i = 0;
	char *pbDest,*pbSrc;

	if(pbDestine == NULL || pbSource == NULL)
		return -1;
	pbDest = pbDestine;
	pbSrc = pbSource;

	while(*pbSrc != 0x00)
	{
		*pbDest++ = *pbSrc++;
		i += 1;
	}
	*pbDest = 0x00;
	return i;
}

short OsMemoryCopy(char *pbDestine,char *pbSource,ushort usLength)
{
	ushort i;

	if(pbDestine == NULL || pbSource == NULL || usLength == 0)
		return -1;
	for(i=0;i < usLength;i++)
		pbDestine[i] = pbSource[i];
	return 0;
}

short OsMemoryMove(char *pbDestine,char *pbSource,ushort usLength)
{
	char pbBuffer[1024];

	if(usLength > 1024)
		return -1;
	OsMemoryCopy(pbBuffer,pbSource,usLength);
	OsMemoryCopy(pbDestine,pbBuffer,usLength);
	return 0;
}

short OsMemoryCompare(char *pbSource,char *pbDestine,ushort usLength)
{
	ushort i;

	if(pbSource == NULL || pbDestine == NULL)
		return -1;
	for(i=0;i < usLength;i++)
	{
		if(pbSource[i] != pbDestine[i])
		{
			if(pbSource[i] >= 0x61 && pbSource[i] <= 0x7a)
			{
				if(pbDestine[i] == pbSource[i] - 0x20)
					continue;
			} else if(pbSource[i] >= 0x41 && pbSource[i] <= 0x5a) {
				if(pbDestine[i] == pbSource[i] + 0x20)
					continue;
			}
			if(pbSource[i] > pbDestine[i])
				return(1);
			else return(-1);
		}
	}
	return(0);
}

double distance(int x1,int y1,int x2,int y2)
{
	return sqrt(double(x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
}

double DisFromPointToLine(int x1,int y1,int x2,int y2,int x,int y)
{
	return abs((y1 - y2)*x + (x1 - x2)*y + y1*x2 - x1*y2)/sqrt(double(x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
}


/* ------------------------------------------------------------------------
 *  convert the long time to date and minute
 *  return  minute
 * ------------------------------------------------------------------------ */
long  minute_time(LONG long_time, long *date)
{
	struct tm *tm_;
	long minute;

	if(long_time == -1)
	{
		*date = -1;
		minute =  -1;
	} else if(long_time < 86400 * 3) {
		*date = long_time / 86400;
		minute =  long_time % 86400;
	} else {
		time_t t = time_t(long_time);
		tm_ = localtime(&t);
		if(date != NULL)
			*date= (tm_->tm_year - 90)*380+tm_->tm_mon*31+tm_->tm_mday-1;
		minute = tm_->tm_hour*3600+tm_->tm_min*60+tm_->tm_sec;
	}
	return minute;
}

/* ------------------------------------------------------------------------
 *  convert date and minute(seconds) to the long time
 *  return  long time (second)
 * ------------------------------------------------------------------------ */
LONG long_time(long date, long ttime)
{
	struct tm when;
	time_t result;

	if(date == -1 || ttime == -1)
		return -1;
	else if(date < 3) {
		return (date * 86400 + ttime);
	} else {
		when.tm_hour= ttime/3600;
		when.tm_min = (ttime%3600)/60;
		when.tm_sec = (ttime%3600)%60;
		when.tm_mday= (date%380)%31+1;
		when.tm_mon = (date%380)/31;
		when.tm_year= date/380+90;
		when.tm_isdst=-1;

		result = mktime(&when);
		return(result);
	}
}

char *TrimSpace(char *pbString)
{
	static char localBuffer[1024];
	int nLength,i,nLeft,nRight;

	do {
		localBuffer[0] = 0x00;
		if(pbString == NULL || 
			(nLength = strlen(pbString)) == 0 ||
			nLength >= sizeof(localBuffer))
			break;
		for(i=0;i <= nLength;i++)
		{
			if(pbString[i] != 0x20 
				&& pbString[i] != '\t'
				&& pbString[i] != '\n' 
				&& pbString[i] != 0x07)
				break;
		}
		if(i > nLength)
			break;
		nLeft = i;
		for(i=nLength - 1;i > nLeft;i--)
		{
			if(pbString[i] != 0x20 
				&& pbString[i] != '\t'
				&& pbString[i] != '\n' 
				&& pbString[i] != 0x07)
				break;
		}
		nRight = i + 1;
		if(nRight == nLeft)
			nRight++; 

		strncpy(localBuffer,pbString + nLeft,nRight - nLeft);
		localBuffer[nRight - nLeft] = 0x00;
	}while(0);
	return localBuffer;
}

char *TrimRightSpace(char *pbString)
{
	static char localBuffer[1024];
	int nLength,i;

	do {
		localBuffer[0] = localBuffer[1] = localBuffer[2] = 
			localBuffer[3] = localBuffer[4] = localBuffer[5] = 
			localBuffer[6] = localBuffer[7] = localBuffer[8] = 0x20;
		localBuffer[9] = 0x00;
		if(pbString == NULL || 
			(nLength = strlen(pbString)) == 0 ||
			nLength >= sizeof(localBuffer))
			break;
		for(i=0;i < 9; i++)
		{
			if(pbString[i] == '\t'
				&& pbString[i] == '\n' 
				&& pbString[i] == 0x00)
				break;
			localBuffer[i] = pbString[i];
		}
	}while(0);
	return localBuffer;
}

int IsAllSpace(const char *pbString)
{
	int nLength,i;

	if(pbString == NULL || (nLength = strlen(pbString)) == 0)
		return 1;
	for(i=0;i < nLength;i++)
	{
		if(pbString[i] != 0x20 
			&& pbString[i] != '\t'
			&& pbString[i] != '\n' 
			&& pbString[i] != 0x07)
			break;
	}
	if(i < nLength)
		return 0;
	return 1;
}

int OsConvertTimestamptoTimet(char *timestamp)
{
	time_t t;
	char *ptr;
	struct tm tp;
	int theStatus = 1;

	do {
		if((ptr = strtok(timestamp,"-.")) == NULL)
			break;
		sscanf(ptr,"%d",&t);
		tp.tm_year = t - 1900;
		if((ptr = strtok(NULL,"-.")) == NULL)
			break;
		sscanf(ptr,"%d",&t);
		tp.tm_mon = t - 1;
		if((ptr = strtok(NULL,"-.")) == NULL)
			break;
		sscanf(ptr,"%d",&t);
		tp.tm_mday = t;
		if((ptr = strtok(NULL,"-.")) == NULL)
			break;
		sscanf(ptr,"%d",&t);
		tp.tm_hour = t;
		if((ptr = strtok(NULL,"-.")) == NULL)
			break;
		sscanf(ptr,"%d",&t);
		tp.tm_min = t;
		if((ptr = strtok(NULL,"-.")) == NULL)
			break;
		sscanf(ptr,"%d",&t);
		tp.tm_sec = t;

		tp.tm_isdst = -1;
		t = mktime(&tp);
		theStatus = 0;
	}while(0);
	if(theStatus)
		return -1;
	return t;
}

char *OsConvertTimettoTimestamp(time_t nValue)
{
	static char sRetValue[26];
	char sDate[16],sTime[16];
	struct tm *tp;

	if((tp = localtime(&nValue)) == NULL)
		return NULL;
	sprintf(sDate,"%04d-%02d-%02d",tp->tm_year + 1900,tp->tm_mon + 1,tp->tm_mday);
	sprintf(sTime,"%02d.%02d.%02d",tp->tm_hour,tp->tm_min,tp->tm_sec);
	sprintf(sRetValue,"%s-%s.000000",sDate,sTime); 
	return sRetValue;
}

void Draw3DRect(CDC * pCDC, CRect &rect, COLORREF clrTopLeft, COLORREF clrBottomRight,int nWidth)
{
#ifndef DRAW_WIDEN_LINE_BY_POINT	
	//method of drawing width line 
	CPen  mPen,*pOldPen;
	int nOldMode;

	if(nWidth > 6)
		nWidth = 6;
	mPen.CreatePen(PS_INSIDEFRAME,nWidth,clrTopLeft);
	pOldPen = pCDC->SelectObject(&mPen);
	nOldMode = pCDC->SetROP2(R2_COPYPEN);
	pCDC->MoveTo(rect.left,rect.bottom);
	pCDC->LineTo(rect.left,rect.top);// left line
	pCDC->LineTo(rect.right,rect.top);// top line
	pCDC->SelectObject((pOldPen);
	mPen.DeleteObject( );
	mPen.CreatePen(PS_INSIDEFRAME,nWidth,clrBottomRight);
	pOldPen = pCDC->SelectObject(&mPen);
	pCDC->MoveTo(rect.right,rect.top);
	pCDC->LineTo(rect.right,rect.bottom);// right line
	pCDC->LineTo(rect.left,rect.bottom);// bottom line
	pCDC->SelectObject(pOldPen);
	mPen.DeleteObject( );
	pCDC->SetROP2(nOldMode);
#else
	// method of drawing color point
	register int i,j,count;
	if(nWidth > 6)
		nWidth = 6;
	if(nWidth / 2 == 1)
		nWidth = nWidth / 2 + 1;
	else nWidth /= 2;
	// drawing top line
	count = 0;
	for(j = rect.top - nWidth;j <= rect.top + nWidth;j++) 
	{
		for(i = rect.left - nWidth;i <= rect.right + nWidth - count;i++)
			pCDC->SetPixel(i,j,clrTopLeft);
		count += 1;
	}
	// drawing left line
	count = 0;
	for(i = rect.left - nWidth;i <= rect.left + nWidth;i++)
	{
		for(j = rect.top + nWidth;j <= rect.bottom + nWidth - count;j++)
			pCDC->SetPixel(i,j,clrTopLeft);
		count += 1;
	}
	// drawing bottom line
	count = 0;
	for(j = rect.bottom + nWidth;j >= rect.bottom - nWidth;j--) 
	{
		for(i = rect.left - nWidth + count;i <= rect.right + nWidth;i++)
			pCDC->SetPixel(i,j,clrBottomRight);
		count += 1;
	}
	// drawing right line
	count = 0;
	for(i = rect.right + nWidth;i >= rect.right - nWidth;i--)
	{
		for(j = rect.top - nWidth + count;j <= rect.bottom - nWidth;j++)
			pCDC->SetPixel(i,j,clrBottomRight);
		count += 1;
	}
#endif	
}

void Draw3DRect(CDC * pCDC, CRect &rect, int nWidth, int nFlag)
{
#ifndef DRAW_WIDEN_LINE_BY_POINT	
// method of drawing width line
	CPen  mPen,*pOldPen;
	int nOldMode;

	if(nFlag)
	{// up mode button
		mPen.CreatePen(PS_INSIDEFRAME,nWidth,CLR_PALEGRAY);
		pOldPen = pCDC->SelectObject(&mPen);
		nOldMode = pCDC->SetROP2(R2_COPYPEN);
		pCDC->MoveTo(rect.left,rect.bottom);
		pCDC->LineTo(rect.left,rect.top);// left line
		pCDC->LineTo(rect.right,rect.top);// top line
		pCDC->SelectObject(pOldPen);
		mPen.DeleteObject( );
		mPen.CreatePen(PS_INSIDEFRAME,nWidth,CLR_GRAY);
		pOldPen = pCDC->SelectObject(&mPen);
		pCDC->MoveTo(rect.right,rect.top);
		pCDC->LineTo(rect.right,rect.bottom);// right line
		pCDC->LineTo(rect.left,rect.bottom);// bottom line
	} else {// down mode button
		mPen.CreatePen(PS_INSIDEFRAME,nWidth,CLR_GRAY);
		pOldPen = pCDC->SelectObject(&mPen);
		nOldMode = pCDC->SetROP2(R2_COPYPEN);
		pCDC->MoveTo(rect.left,rect.bottom);
		pCDC->LineTo(rect.left,rect.top);
		pCDC->LineTo(rect.right,rect.top);
		pCDC->SelectObject(pOldPen);
		mPen.DeleteObject( );
		mPen.CreatePen(PS_INSIDEFRAME,nWidth,CLR_PALEGRAY);
		pOldPen = pCDC->SelectObject(&mPen);
		pCDC->MoveTo(rect.right,rect.top);
		pCDC->LineTo(rect.right,rect.bottom);
		pCDC->LineTo(rect.left,rect.bottom);
	}
	pCDC->SelectObject(pOldPen);
	mPen.DeleteObject( );
	pCDC->SetROP2(nOldMode);
#else
	// method of drawing color point
	register int i,j,count;
	COLORREF clrTopLeft,clrBottomRight;
	if(nFlag)
	{// draw up mode 3Drect
		clrTopLeft = CLR_PALEGRAY;
		clrBottomRight = CLR_GRAY;
	} else {// draw down mode 3Drect
		clrTopLeft = CLR_GRAY;
		clrBottomRight = CLR_PALEGRAY;
	}
	if(nWidth > 6)
		nWidth = 6;
	if(nWidth / 2 == 1)
		nWidth = nWidth / 2 + 1;
	else nWidth /= 2;
	// drawing top line
	count = 0;
	for(j = rect.top - nWidth;j <= rect.top + nWidth;j++) 
	{
		for(i = rect.left - nWidth;i <= rect.right + nWidth - count;i++)
			pCDC->SetPixel(i,j,clrTopLeft);
		count += 1;
	}
	// drawing left line
	count = 0;
	for(i = rect.left - nWidth;i <= rect.left + nWidth;i++)
	{
		for(j = rect.top + nWidth;j <= rect.bottom + nWidth - count;j++)
			pCDC->SetPixel(i,j,clrTopLeft);
		count += 1;
	}
	// drawing bottom line
	count = 0;
	for(j = rect.bottom + nWidth;j >= rect.bottom - nWidth;j--) 
	{
		for(i = rect.left - nWidth + count;i <= rect.right + nWidth;i++)
			pCDC->SetPixel(i,j,clrBottomRight);
		count += 1;
	}
	// drawing right line
	count = 0;
	for(i = rect.right + nWidth;i >= rect.right - nWidth;i--)
	{
		for(j = rect.top - nWidth + count;j <= rect.bottom - nWidth;j++)
			pCDC->SetPixel(i,j,clrBottomRight);
		count += 1;
	}
#endif	
}


// written by wsg, 2003/03/19
// Just for following switch high-low bytes function
// Had better remove from precompile to config files
template<typename T>
T  NettoHostValue(T&  val)
{
	if(!(1 == gpcdef.m_usCadOsSystemFlag))
		return  val;

	T  tempVal = val;
//#ifdef HIGH_BYTE_FIRST
	unsigned char *p = reinterpret_cast<unsigned char *>(&tempVal);
	unsigned char *q = p + sizeof(T) - 1;
	for(; p<q; p++, q--)
	{
		*p = *p ^ *q;
		*q = *p ^ *q;
		*p = *p ^ *q;
	}
//#endif
	return  tempVal;
}

USHORT NettoHostUShort(USHORT usInputValue)
{
	return NettoHostValue(usInputValue);
}

SHORT NettoHostShort(SHORT usInputValue)
{
	return NettoHostValue(usInputValue);
}

INT NettoHostInteger(INT usInputValue)
{
	return NettoHostValue(usInputValue);
}

UINT NettoHostUInteger(UINT usInputValue)
{
	return NettoHostValue(usInputValue);
}

ULONG  NettoHostULong(ULONG  usInputValue)
{
	return NettoHostValue(usInputValue);
}

FLOAT NettoHostFloat(FLOAT usInputValue)
{
	return NettoHostValue(usInputValue);
}

/*
#ifdef HIGH_BYTE_FIRST
ushort NettoHostUShort(ushort usInputValue)
{
	ushort usOutputValue;
	char *abInput,*abOutput;

	abOutput = (char *)&usOutputValue;
	abInput = (char *)&usInputValue;

	abOutput[0] = abInput[1];
	abOutput[1] = abInput[0];

	return usOutputValue;
}



short NettoHostShort(short usInputValue)
{
	short usOutputValue;
	char *abInput,*abOutput;

	abOutput = (char *)&usOutputValue;
	abInput = (char *)&usInputValue;

	abOutput[0] = abInput[1];
	abOutput[1] = abInput[0];

	return usOutputValue;
}


int NettoHostInteger(int usInputValue)
{
	int usOutputValue;
	char *abInput,*abOutput;

	abOutput = (char *)&usOutputValue;
	abInput = (char *)&usInputValue;

	abOutput[0] = abInput[3];
	abOutput[1] = abInput[2];
	abOutput[2] = abInput[1];
	abOutput[3] = abInput[0];

	return usOutputValue;
}

uint NettoHostUInteger(uint usInputValue)
{
	uint usOutputValue;
	char *abInput,*abOutput;

	abOutput = (char *)&usOutputValue;
	abInput = (char *)&usInputValue;

	abOutput[0] = abInput[3];
	abOutput[1] = abInput[2];
	abOutput[2] = abInput[1];
	abOutput[3] = abInput[0];

	return usOutputValue;
}

float NettoHostFloat(float usInputValue)
{
	float usOutputValue;
	char *abInput,*abOutput;

	abOutput = (char *)&usOutputValue;
	abInput = (char *)&usInputValue;

	abOutput[0] = abInput[3];
	abOutput[1] = abInput[2];
	abOutput[2] = abInput[1];
	abOutput[3] = abInput[0];

	return usOutputValue;
}

unsigned long  NettoHostULong(unsigned long  usInputValue)
{
	unsigned long  usOutputValue;
	char *abInput,*abOutput;

	abOutput = (char *)&usOutputValue;
	abInput = (char *)&usInputValue;

	abOutput[0] = abInput[3];
	abOutput[1] = abInput[2];
	abOutput[2] = abInput[1];
	abOutput[3] = abInput[0];

	return usOutputValue;
}

//*********************************************
#else
ushort NettoHostUShort(ushort usInputValue)
{
	return usInputValue;
}

short NettoHostShort(short usInputValue)
{
	return usInputValue;
}

int NettoHostInteger(int usInputValue)
{
	return usInputValue;
}

uint NettoHostUInteger(uint usInputValue)
{
	return usInputValue;
}

unsigned long  NettoHostULong(unsigned long  usInputValue)
{
	return usInputValue;
}

float NettoHostFloat(float usInputValue)
{
    return usInputValue;
}
#endif
*/

int isTimeRangeOverlap(int usStartDate1,int usStartTime1,
	int usEndDate1,int usEndTime1,
	int usStartDate2,int usStartTime2,
	int usEndDate2,int usEndTime2,int *nTimeFlag)
{
	int theResult = RET_FAILED;
	int nFlag;

	do {
		if(usStartDate1 > usEndDate1 ||
			(usStartDate1 == usEndDate1 && usStartTime1 > usEndTime1))
			break;
		if(usStartDate2 > usEndDate2 ||
			(usStartDate2 == usEndDate2 && usStartTime2 > usEndTime2))
			break;
		if(usStartDate1 == usStartDate2 && usStartTime1 == usStartTime2)
		{
			if(usEndDate1 == usEndDate2 && usEndTime1 == usEndTime2)
			{
				theResult = TIME_RANGE_EQUALIZED;
				nFlag = TIME_EQUAL;
			} else if(usEndDate1 < usEndDate2 ||
				(usEndDate1 == usEndDate2 && usEndTime1 < usEndTime2)){
				theResult = TIME_RANGE_DURING;
				nFlag = TIME_EARLY;
			} else {
				theResult = TIME_RANGE_INCLUDE;
				nFlag = TIME_LATE;
			}
		} else	if(usStartDate1 < usStartDate2 ||
			(usStartDate1 == usStartDate2 && usStartTime1 < usStartTime2))
		{
			if(usEndDate1 < usStartDate2 ||
				(usEndDate1 == usStartDate2 && usEndTime1 < usStartTime2))
			{
				theResult = TIME_RANGE_SEPERATED;
				nFlag = TIME_EARLY;
			} else {
				if(usEndDate1 < usEndDate2 ||
					(usEndDate1 == usEndDate2 && usEndTime1 < usEndTime2)){
					theResult = TIME_RANGE_OVERLAPED;
					nFlag = TIME_EARLY;
				} else {
					theResult = TIME_RANGE_INCLUDE;
					nFlag = TIME_LATE;
				}
			}
		} else {
			if(usStartDate1 < usEndDate2 ||
				(usStartDate1 == usEndDate2 && usStartTime1 < usEndTime2)){
				if(usEndDate1 < usEndDate2 ||
					(usEndDate1 == usEndDate2 && usEndTime1 <= usEndTime2)){
					theResult = TIME_RANGE_DURING;
					nFlag = TIME_LATE;
				} else {
					theResult = TIME_RANGE_OVERLAPED;
					nFlag = TIME_LATE;
				}
			}else if(usStartDate1 == usEndDate2 && usStartTime1 == usEndTime2){
				theResult = TIME_RANGE_OVERLAPED;
				nFlag = TIME_LATE;
			} else {
				theResult = TIME_RANGE_SEPERATED;
				nFlag = TIME_LATE;
			}
		}
		if(nTimeFlag != NULL)
			*nTimeFlag = nFlag;
	}while(0);
	
	return theResult;
}

// read file function
char * GetStringNextItem(char *szSrc,char cBreak)
{
	char *p;

	p = strchr(szSrc, cBreak);
	if(p == NULL)
		return (char *)NULL;
	p++;
	while(*p==' ')
		p++;
	if(*p == 0x00 || *p == '\n')
		return NULL;
	return p;
}

int ReadProfile(FILE *fp,char *szSectionName,char *szKeyword,char *szBuffer)
{
   if((fp == NULL) || (szSectionName == NULL) || 
	   (szBuffer == NULL) || (szKeyword == NULL))
       return (-1);
    fseek(fp, 0, SEEK_SET);
    if(LocateSection(fp, szSectionName))
       return (-1);
    if(LocateKeyword(fp, szKeyword, szBuffer))
       return (-1);
    return 0;
}

int LocateKeyword(FILE *fp,char *szString,char *szBuffer)
{
	if(szBuffer == NULL)
		return (-1);
	while ( ReadStringFromFile(szBuffer, 256, fp) != NULL) {
		if ( strstr(szBuffer, szString) != NULL ) {
			return (0);
		}
	}
	return (-1);
}

int LocateSection(FILE *fp,char *szString)
{
	char szBuffer[256];

	while ( ReadStringFromFile(szBuffer, 256, fp) != NULL) {
		strupr(szBuffer);
		if ( strstr(szBuffer, szString) != NULL )
			return (0);
	}
	return (-1);
}

char * ReadStringFromFile(char * abBuffer, int nLength, FILE * fp)
{
	if(abBuffer == NULL || nLength == 0 || fp == NULL)
		return NULL;
	do {
		if(fgets(abBuffer,nLength,fp) == NULL)
			return NULL;
		strcpy(abBuffer,TrimSpace(abBuffer));
		//remove remark line and space line
	}while(abBuffer[0] == '#' || abBuffer[0] == ';' || abBuffer[0] == 0x00);
	return abBuffer;
}



//////////////////////////////////////////////////////////
//		change child window size and position 
//		reseting every control screen(window) position 
//////////////////////////////////////////////////////////
void ResizeWndToFixSize(CWnd* pWnd, INT cx, INT cy)
{
	if( cx==0 || cy==0 )
		return;

	//CHANGE Wnd DIMENSION
	CRect originalWndRect, originalClientRect;
	CRect newWndRect, newClientRect;
	pWnd->GetWindowRect(&originalWndRect);
	pWnd->GetClientRect( &originalClientRect );

	newWndRect.left = originalWndRect.left;
	newWndRect.right = newWndRect.left + cx;
	newWndRect.top = originalWndRect.top;
	newWndRect.bottom = newWndRect.top + cy;

	pWnd->SetWindowPos(NULL,
		newWndRect.left,newWndRect.top,newWndRect.Width(),newWndRect.Height(),
		SWP_NOZORDER);

	pWnd->GetClientRect( &newClientRect );

	double	fHorizontalMultipleValue 
		= (double)newClientRect.Width( ) / originalClientRect.Width();
	double	fVerticalMultipleValue 
		= (double)newClientRect.Height( ) / originalClientRect.Height();

	CWnd *pChildWnd = pWnd->GetWindow(GW_CHILD);
	while(pChildWnd != NULL)
	{
		CRect rect;

		pChildWnd->GetWindowRect(&rect);
		if(pChildWnd->IsKindOf(RUNTIME_CLASS(CComboBox)))
		{
			CRect dropRect;
			((CComboBox *)pChildWnd)->GetDroppedControlRect(&dropRect);
			rect.UnionRect(&rect, &dropRect);
		}

		pWnd->ScreenToClient( &rect );
		rect.left *= fHorizontalMultipleValue;
		rect.right *= fHorizontalMultipleValue;
		rect.top *= fVerticalMultipleValue;
		rect.bottom *= fVerticalMultipleValue;

		pChildWnd->SetWindowPos(NULL,rect.left,rect.top, rect.Width( ),rect.Height( ), SWP_NOZORDER);
		pChildWnd = pChildWnd->GetWindow(GW_HWNDNEXT);
	}
/*	if( fXMultiple<=0 || fYMultiple<=0 )
		fXMultiple = fYMultiple = 0.95;

		//CHANGE DIALOG DIMENSION
		int screenx = GetSystemMetrics(SM_CXSCREEN);
		int screeny = GetSystemMetrics(SM_CYSCREEN);

		screenx = screeny * 4/3;	//单屏宽度

		int nDialogWidth = screenx  * fXMultiple;
		int nDialogHeight = screeny * fYMultiple;

		if( nDialogWidth >= screenx )	nDialogWidth = screenx - 1;
		if( nDialogHeight >= screeny )	nDialogHeight = screeny - 1;

		CRect tmp, rclWindowRect;
		pWnd->GetWindowRect(&tmp);

		rclWindowRect.left = (tmp.left / screenx) * screenx + (screenx - nDialogWidth)/2;
		rclWindowRect.right = rclWindowRect.left + nDialogWidth - 1;
		rclWindowRect.top = (screeny - nDialogHeight)/2;
		rclWindowRect.bottom = rclWindowRect.top + nDialogHeight - 1;

		pWnd->GetClientRect( &tmp );
		pWnd->SetWindowPos(NULL,
			rclWindowRect.left,rclWindowRect.top,rclWindowRect.Width( ),rclWindowRect.Height( ),
			SWP_NOZORDER);

		CRect tmp2;
		pWnd->GetClientRect( &tmp2 );

		double	fHorizontalMultipleValue = (double)tmp2.Width( ) / tmp.Width();
		double	fVerticalMultipleValue = (double)tmp2.Height( ) / tmp.Height();
	
		CWnd *pChildWnd = pWnd->GetWindow(GW_CHILD);
		while(pChildWnd != NULL)
		{
			CRect rect;

			pChildWnd->GetWindowRect(&rect);
			if(pChildWnd->IsKindOf(RUNTIME_CLASS(CComboBox)))
			{
				CRect dropRect;
				((CComboBox *)pChildWnd)->GetDroppedControlRect(&dropRect);
				rect.UnionRect(&rect, &dropRect);
			}

			pWnd->ScreenToClient( &rect );
			rect.left *= fHorizontalMultipleValue;
			rect.right *= fHorizontalMultipleValue;
			rect.top *= fVerticalMultipleValue;
			rect.bottom *= fVerticalMultipleValue;

			pChildWnd->SetWindowPos(NULL,rect.left,rect.top, rect.Width( ),rect.Height( ), SWP_NOZORDER);
			pChildWnd = pChildWnd->GetWindow(GW_HWNDNEXT);
		}
*/
/*		宋工程序段
		int screenx = GetSystemMetrics(SM_CXSCREEN);
		int screeny = GetSystemMetrics(SM_CYSCREEN);

		CRect tmp;
		CRect rclClientRect,rclWindowRect;
		screenx /= gpcdef.GetScreenNum();

		int nDialogWidth = screenx  * 19/20;
		int nDialogHeight = screeny * 19/20;

		GetWindowRect(&tmp);
		rclWindowRect.left = (tmp.left / screenx) * screenx + (screenx - nDialogWidth)/2;
		rclWindowRect.right = rclWindowRect.left + nDialogWidth;
		rclWindowRect.top = (screenx - nDialogWidth)/2;
		rclWindowRect.bottom = rclWindowRect.top + nDialogHeight;
		SetWindowPos(NULL,
			rclWindowRect.left,rclWindowRect.top,rclWindowRect.Width( ),rclWindowRect.Height( ),
			SWP_NOZORDER);

		CRect rclOriginalRect,rclResultRect;
		int nHorizontalStartPosition,nVerticalStartPosition;
		double fHorizontalMultipleValue,fVerticalMultipleValue;

//		CWnd *pWnd;
//		GetWindowRect(&rclWindowRect);
		//testing 
	//	rclWindowRect.bottom -= 50;
	//	rclWindowRect.right -= 100;

//		nHorizontalStartPosition = rclWindowRect.left;
//		nVerticalStartPosition = rclWindowRect.top;
//		pWnd = GetDlgItem(IDC_CMD_NAME);
//		pWnd->GetWindowRect(&rclWindowRect);
//		ScreenToClient(&rclWindowRect);

 		GetClientRect(&rclClientRect);

	 	fHorizontalMultipleValue = (double)rclClientRect.Width( ) / tmp.Width();
		fVerticalMultipleValue = (double)rclClientRect.Height( ) / tmp.Height();
//	 	fHorizontalMultipleValue = (double)rclClientRect.Width( ) / (double)(rclWindowRect.right + 25);
//		fVerticalMultipleValue = (double)rclClientRect.Height( ) / (double)(rclWindowRect.bottom + 25);

		nHorizontalStartPosition = rclClientRect.left;
		nVerticalStartPosition = rclClientRect.top;
		
		CWnd *pChildWnd = GetWindow(GW_CHILD);
		while(pChildWnd != NULL)
		{
			pChildWnd->GetWindowRect(&rclOriginalRect);
			if(pChildWnd->IsKindOf(RUNTIME_CLASS(CComboBox)))
			{
				CRect dropRect;
				((CComboBox *)pChildWnd)->GetDroppedControlRect(&dropRect);
				rclOriginalRect.UnionRect(&rclOriginalRect, &dropRect);
			}
			rclResultRect = rclOriginalRect;
			rclResultRect.left = (int)((rclResultRect.left - nHorizontalStartPosition) * fHorizontalMultipleValue) + nHorizontalStartPosition;
			rclResultRect.right = (int)((rclResultRect.right - nHorizontalStartPosition) * fHorizontalMultipleValue) + nHorizontalStartPosition;
			rclResultRect.top = (int)((rclResultRect.top - nVerticalStartPosition) * fVerticalMultipleValue) + nVerticalStartPosition;
			rclResultRect.bottom = (int)((rclResultRect.bottom - nVerticalStartPosition) * fVerticalMultipleValue) + nVerticalStartPosition;
			ScreenToClient(&rclResultRect);
			pChildWnd->SetWindowPos(NULL,rclResultRect.left,rclResultRect.top,
				rclResultRect.Width( ),rclResultRect.Height( ),
				SWP_NOZORDER);
			pChildWnd = pChildWnd->GetWindow(GW_HWNDNEXT);
		}
*/
}


struct tagCharTable{
	char text1[3];
	char text2[2];
}DmisCharTable[]=
{{"Ａ","A"},{"Ｂ","B"},{"Ｃ","C"},
{"Ｄ","D"},{"Ｅ","E"},{"Ｆ","F"},
{"Ｇ","G"},{"Ｈ","H"},{"Ｉ","I"},
{"Ｊ","J"},{"Ｋ","K"},{"Ｌ","L"},
{"Ｍ","M"},{"Ｎ","N"},{"Ｏ","O"},
{"Ｐ","P"},{"Ｑ","Q"},{"Ｒ","R"},
{"Ｓ","S"},{"Ｔ","T"},{"Ｕ","U"},
{"Ｖ","V"},{"Ｗ","W"},{"Ｘ","X"},
{"Ｙ","Y"},{"Ｚ","Z"},{"ａ","A"},
{"ｂ","B"},{"ｃ","C"},{"ｄ","D"},
{"ｅ","E"},{"ｆ","F"},{"ｇ","G"},
{"ｈ","H"},{"ｉ","I"},{"ｊ","J"},
{"ｋ","K"},{"ｌ","L"},{"ｍ","M"},
{"ｎ","N"},{"ｏ","O"},{"ｐ","P"},
{"ｑ","Q"},{"ｒ","R"},{"ｓ","S"},
{"ｔ","T"},{"ｕ","U"},{"ｖ","V"},
{"ｗ","W"},{"ｘ","X"},{"ｙ","Y"},
{"ｚ","Z"},{"１","1"},{"２","2"},
{"３","3"},{"４","4"},{"５","5"},
{"６","6"},{"７","7"},{"８","8"},
{"９","9"},{"０","0"},{"—","-"},
{"＋","+"},{"－","-"}};

CString ChangeAlpha(CString &input)
{
	CString temp = input;
	CString tmp;
	CString t_sReturnValue("");

	while(!temp.IsEmpty()){
		if(IsCharAlphaNumeric(temp.GetAt(0))){
			tmp = temp.Left(1);
			t_sReturnValue += tmp;
			temp = temp.Mid(1);
		}else if((BYTE)temp.GetAt(0)>=0xA0){
			tmp = temp.Left(2);
			for(int i = 0;i<sizeof(DmisCharTable)/sizeof(DmisCharTable[0]);i++){
				CString t_stmp;
				t_stmp.Format("%s",DmisCharTable[i].text1);
				if(t_stmp == tmp){
					tmp.Format("%s",DmisCharTable[i].text2);
					break;
				}
			}
			t_sReturnValue += tmp;
			temp = temp.Mid(2);
		}else{
			tmp = temp.Left(1);
			t_sReturnValue += tmp;
			temp = temp.Mid(1);
		}
	}
	return t_sReturnValue;
}
/*************************************************************
*Purpose:把半角字符转换成全角字符
*input:	CString &input 要转换的字符串
*output:none
*return:CString 转换以后的返回值
*
***************************************************************/

CString ChangeFromHalf2Full(CString &input){
	CString temp = input;
	temp.MakeUpper();//转成大写
	CString firstchar;
	CString convertedchar;
	BOOL charconverted = FALSE;
	CString t_sReturnValue("");

	while(!temp.IsEmpty())
	{
		firstchar = temp.Left(1);
		charconverted = FALSE;
		for(int i = 0;i<sizeof(DmisCharTable)/sizeof(DmisCharTable[0]);i++)
		{
			convertedchar.Format("%s",DmisCharTable[i].text2);
			if(convertedchar == firstchar)
			{
				convertedchar.Format("%s",DmisCharTable[i].text1);
				charconverted = TRUE;
				break;
			}
		}
		if(charconverted)
		{
			t_sReturnValue += convertedchar;
		}
		else
			t_sReturnValue += firstchar;

		temp = temp.Mid(1);
		
	}
	return t_sReturnValue;
}
///      shift_id ///////////

unsigned long  Get_CurrentShiftid()
{
	CTime t=CTime::GetCurrentTime();    //Initiae global time;
	int nYear  = t.GetYear();
	int nMonth = t.GetMonth();
	int nDay   = t.GetDay();
	int nHour  = t.GetHour();
	int nbanbie;        //1 or 2
	long usDate = (nYear - 1990) * 380 + (nMonth - 1) * 31 + nDay - 1;
	if(nHour>=18)
	{
		nbanbie=1;
		CTime tm;
		tm=CTime(long_time(OsGetNextDate(PMTG.ThreadId,usDate),0));
		nDay=tm.GetDay();
		nYear=tm.GetYear();
		nMonth=tm.GetMonth();
	}
	else if(nHour<6)
	{
		nbanbie=1;
		nDay=t.GetDay();
	}
	else
	{  
		nbanbie=2;
		nDay=t.GetDay();
	}
	unsigned long nshiftid=nYear*1000000 + nMonth*10000 + nDay*100 + nbanbie;
    return nshiftid;
}

unsigned long Get_NextShiftid(unsigned long CurShiftid)
{ 
	int CurYear,NextYear;
	int CurMonth,NextMonth;
    int CurDay,NextDay;
    int CurBanbie,NextBanbie;
	
    CurBanbie=CurShiftid-(CurShiftid/10)*10;
    NextBanbie=CurBanbie==1 ? 2 :1;
    
	CurYear  = CurShiftid/1000000;
	CurMonth = CurShiftid/10000-CurYear*100;
	CurDay   = CurShiftid/100-(CurShiftid/10000)*100;
    if(NextBanbie==1)
	{
		long usDate = (CurYear - 1990) * 380 + (CurMonth - 1) * 31 + CurDay - 1;
		CTime tm=CTime(long_time(OsGetNextDate(PMTG.ThreadId,usDate),0));
		//OsGetPrevDate
		NextYear=tm.GetYear();
		NextMonth=tm.GetMonth();
		NextDay=tm.GetDay();
    }
	else
	{
		NextYear=CurYear;
		NextMonth=CurMonth;
		NextDay=CurDay;		
	}  
    unsigned long nshiftid=NextYear*1000000 + NextMonth*10000
		+ NextDay*100 + NextBanbie;
    return nshiftid;
}

unsigned long Get_PreShiftid(unsigned long CurShiftid)
{
	int CurYear,PreYear;
	int CurMonth,PreMonth;
    int CurDay,PreDay;
    int CurBanbie,PreBanbie;
	
    CurBanbie=CurShiftid-(CurShiftid/10)*10;
    PreBanbie=CurBanbie==1 ? 2 :1;
    
	CurYear  = CurShiftid/1000000;
	CurMonth = CurShiftid/10000-CurYear*100;
	CurDay   = CurShiftid/100-(CurShiftid/10000)*100;
    if(PreBanbie==2)
	{
		long usDate = (CurYear - 1990) * 380 + (CurMonth - 1) * 31 + CurDay - 1;
		CTime tm=CTime(long_time(OsGetPrevDate(PMTG.ThreadId,usDate),0));
		//OsGetPrevDate
		PreYear=tm.GetYear();
		PreMonth=tm.GetMonth();
		PreDay=tm.GetDay();
    }
	else
	{
		PreYear=CurYear;
		PreMonth=CurMonth;
		PreDay=CurDay;		
	}  
    unsigned long nshiftid=PreYear*1000000 + PreMonth*10000
		+ PreDay*100 + PreBanbie;
    return nshiftid;
	
}


ushort GetParmValue(char *buffer,char  * field_name,BYTE field_type, void *field_add, USHORT size)
{
	const char   *ptr,*ptr1;     
	USHORT rc=-1;          
	USHORT y,*ptr_short;   
	ULONG  *ptr_long;      
	BYTE   *ptr_byte;     
	int x;                 
	BYTE  field_len;       

	do{
		/* first check if the field is in the configuration line */
		field_len = strlen(field_name);
		ptr = (char*)buffer;
		do 
		{
			if((ptr=strstr((const char *)ptr,field_name))==NULL)
			{
				rc =-1;
				break;
			}
			if (ptr != (char*)buffer)
			{  // to exclude field_name in another field_name
				if (*(BYTE*)(ptr-1) == ',' || *(BYTE*)(ptr-1) == 0 )
					rc =0;
			}
			else
				rc = 0;

			if (rc) {
				ptr += field_len;
				continue;
			}

			if (*(BYTE*)(ptr+field_len) == '='){
				rc=0;
				break;
			}
			ptr += field_len;

		} while (ptr < (char*)(buffer+strlen((const char*)buffer)));

		if (rc)
			break;

		ptr+=field_len+1;   /* skip field_name and '=' */
		if (*ptr == 0)
			break;

		switch ( field_type )
		{
			case TYPE_STRING:
				/* string value, find break character or null terminator */
				if((ptr1=strpbrk(ptr,",; \r\n\t"))==NULL)
				{
					/* no break character, find null */
					if((x=strlen(ptr))==(USHORT)0)
						break;
				}
				else x=ptr1-ptr;
				if(x >= size)
					break;      /* not enough space */
				memcpy(field_add,ptr,x);
				*((BYTE *)field_add+x) = 0;
				rc=0;
				break;

			case TYPE_BYTE:
				size=1;

			case TYPE_BYTE_ARRAY:
				ptr_byte=(BYTE*)field_add; y=0;
				while(1){
					x=atoi(ptr);
					*ptr_byte=(BYTE)x;
					ptr_byte++;
					if(++y < size){
						if((ptr1=strpbrk(ptr,",; \r\n\t"))==NULL)
							break;
						ptr=ptr1+1;
					}
					else break;
				}
				if(y>=size)
					rc=0;
				break;

			case TYPE_SHORT:
				size=1;

			case TYPE_SHORT_ARRAY:
				ptr_short=(USHORT*)field_add; y=0;
				while(1){
					x=atoi(ptr);
					*ptr_short=(USHORT)x; 
					ptr_short++;
					if(++y<size){
						if((ptr1=strpbrk(ptr,",; \r\n\t"))==NULL)
							break;
						ptr=ptr1+1;
					}
					else break;
				}
				if(y>=size)
					rc=0;
				break;

			case TYPE_LONG:
				size=1;

			case TYPE_LONG_ARRAY:
				ptr_long=(ULONG*)field_add; y=0;
				while(1){
					x=atoi(ptr);
					*ptr_long=(ULONG)x;
					ptr_long++;
					if(++y < size){
						if((ptr1=strpbrk(ptr,",; \r\n\t"))==NULL)
							break;
						ptr=ptr1+1;
					}
					else break;
				}
				if(y>=size)
					rc=0;
				break;
			default:
				/* invalid parameter type */
				break;
		};

	}while(0);
	return(rc);
}

SHORT GetConfigLine(FILE *fptr, char buf[], USHORT bufsize)
/*----------------------------------------------------------------------------
*  Description: The function is to get one record from the document
*      system.def except the space and coment. The record ends with
*      only new line sign "cr-lf", while the '&' is not before the
*      "cr-lf". Comments may not go across line boundry.
*      EXIT Conditions:
*              -8 none terminating comment
*              -7 file cann't be opened
*              -6 left of equality not valid.(,=xy,)
*              -5 invalid char
*              -4 right of equality error (,=xy,)
*              -3 double equates (,xy=yy=zz,)
*              -2 empty field (,,)
*              -1 sting too long
*              0  end of file.
*              i: # of characters returned.
*---------------------------------------------------------------------------*/
{
	char    line[512],
		*ptr,
		s0,   /* a blank has been inserted */
		s1,   /* flag s1 if you collected the destination arg */
		s2,   /* flag s2 if the = is found. */
		s3;   /* flag s3 if the source arg is found */
	SHORT rs;       /* the back value */
	USHORT i=0,c;

	do{
		if (NULL == fptr)
		{
			_snprintf(buf, bufsize, "code err, please first open file");
			rs = -7;
			break;
		}

		s0=s1=s2=s3=0; rs=0; bufsize--;
		do {
			/* The current position is at the end of the file. */
			if (fgets(line,sizeof(line),fptr)==NULL)  
			{				
				rs=0;
				break;
			}
			/* The line is too long */
			if (line[strlen(line)-1] != '\n' )
			{
				_snprintf(buf, bufsize, "line is too long max(%d) at %s", sizeof(line)-1, line);
				rs=-8;
				break;
			}

			ptr=line;
			if((c=*(ptr)) == '#')
				continue;

			while (rs==(SHORT)0 && (c=*(ptr++))!='\n') 
			{
				if (i>=bufsize){
					rs=-1;
					break;
				}

				/* test for error or blank flush or tab till endof line */
				if( c==' ' || c ==(BYTE)9  )  /* blank is found */
				{
					if(s1 && !s2) {
						buf[i++]=' ';
						s1=0; s0=1;
					}           
					continue;  /* skip this blank */
				}
				buf[i++]=(BYTE)c;            /* save the char */
				switch(c)  
				{
					case '=':
						/* delete any previous blank */
						if(s0){
							s0=0;
							s1=1;
							buf[i-2]=(BYTE)c;
							i--;
						}
						if (!s1)
							rs=-6;
						else if (s2||s3)
							rs=-3;
						else s2=1;
						break;
					case ',':     /* allow null variables */
						s0=s1=s2=s3=0;
						break;
					case '&':
						/* flush till end of the line */
						while (*ptr !='\n') ptr++;
						break;
					case '/':
						if (*ptr=='*'){
							i--;       /* remove the byte from buffer */
							do{
								while((c=*(++ptr))!='*' && c!='\n');
								if (c=='\n' || (c=*(++ptr))=='/')
									break;
							} while(1);
							if(c!='/') rs=-8;  /* comment error */
							else ptr++;     /* skip this char */
							break;
						}
					default:
						if(s1==(BYTE)0) {
							s1++;
							s0=0;
						}
						else if(s2 && s3==(BYTE)0)
							s3++;
				};
			}
			if (rs || buf[i-1]!='&')
				break;
			else i--;      /* remove the & sign form the buffer */
		} while(1);

		/* skip blank lines */
		if(rs) break;
		if(i)
		{
			rs=i;
			break;
		}

	}while(1);

	if (rs>=0) buf[i]=0;

	return(rs);
} /* endof GetConfigParm() */



const short MAX_TIME_STRING_LEN = 9;
const short MAX_TIME_STRING_COUNT = 4;

const char* StringTime(long ltime)
{
	// get available space
	static short  last_index=0;
	static char   dest[MAX_TIME_STRING_LEN * MAX_TIME_STRING_COUNT];

	char* ptr = dest + MAX_TIME_STRING_LEN * last_index;
	last_index = (last_index + 1) % MAX_TIME_STRING_COUNT;

	// translate time
	if (ltime <= 0) 
	{
		sprintf(ptr, "XX:XX:XX");
	} 
	else 
	{
		struct tm *when  = localtime( (time_t*) &ltime );
	
		strftime(ptr, MAX_TIME_STRING_LEN, "%H:%M:%S", when);
	}
	return ptr;
}
const char* StringDate(long ltime)
{
	// get available space
	static short  last_index=0;
	static char   dest[MAX_TIME_STRING_LEN * MAX_TIME_STRING_COUNT];

	char* ptr = dest + MAX_TIME_STRING_LEN * last_index;
	last_index = (last_index + 1) % MAX_TIME_STRING_COUNT;

	// translate date
	if (ltime <= 0) 
	{
		sprintf(ptr, "XX/XX/XX");
	} 
	else 
	{
		struct tm *when  = localtime( (time_t*) &ltime );

		strftime(ptr, MAX_TIME_STRING_LEN, "%y/%m/%d", when);
	}
	return ptr;
}

time_t  ConvertToLongTime(time_t t,time_t reftm,int hour_range_left,int hour_range_right)
{
	   if(t>(reftm+hour_range_right*3600))
		   t-=3600*24;
	   else
	   if(t<(reftm-hour_range_left*3600))
		   t+=3600*24;
	   if(t>(reftm+hour_range_right*3600) || t<(reftm-hour_range_left*3600))
		   return 0;
	   else
		   return t;
}

time_t  ConvertToLongTime(int hour,int min,int sec,time_t reftm,int hour_range_left,int hour_range_right)  
{
		CTime curtm;
		if(reftm==0)
			curtm=CTime::GetCurrentTime();
		else
			curtm=reftm;
		CTime todaytm=CTime(curtm.GetYear(),curtm.GetMonth(),curtm.GetDay(),0,0,0);
		time_t  todayhour=todaytm.GetTime()/3600;
		time_t  curhour=curtm.GetTime()/3600;
		time_t  rt_tm=todayhour+hour;
		
		if(rt_tm>curhour+hour_range_right) rt_tm-=24;
		else if(rt_tm<curhour-hour_range_left) rt_tm+=24;
		if((rt_tm>=curhour-hour_range_left) && (rt_tm<=curhour+hour_range_right))
		{
			CTime ret_time=rt_tm*3600+min*60+sec;
			return ret_time.GetTime();
		}
		else
			return 0;

}

const char* BasicStringTime(int ltime)
{
	// translate time
	static char ptr[32];
	memset(ptr, 0, sizeof(ptr));
	if (ltime < 0) 
	{
		sprintf(ptr, "XX:XX:XX");
	}
	else
	{
		int hour=ltime/3600;
		int minute=(ltime%3600)/60;
		int second=ltime%60;
		sprintf(ptr, "%02d:%02d:%02d",hour,minute,second);
	}

	return ptr;
}
