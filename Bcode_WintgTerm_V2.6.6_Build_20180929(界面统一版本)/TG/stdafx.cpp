// stdafx.cpp : 仅包括标准包含文件的源文件
//  TG.pch 将作为预编译头
//  stdafx.obj 将包含预编译类型信息

#include "stdafx.h"
#include "math.h"

#define EMIN  1E-10

CString ConvertTimetoString(long uValue)
{
	long uHour,uMinute,uSecond;
	char string[12];
	memset(string,0x20,12);

	if(uValue != -1)
	{
		uHour = (uValue / 60) / 60;
		if(uHour < 10)
		{
			string[0] = 0x30;
			string[1] = 0x30 + uHour;
		}
		else 
		{
			string[0] = 0x30 + uHour/10;
			string[1] = 0x30 + uHour%10;
		}
		string[2] = ':';

		uMinute = (uValue / 60) % 60;
		if(uMinute < 10)
		{
			string[3] = 0x30;
			string[4] = 0x30 + uMinute;
		}
		else 
		{
			string[3] = 0x30 + uMinute/10;
			string[4] = 0x30 + uMinute%10;
		}
		uSecond = uValue % 60;
		
		string[5] = ':';

		uSecond = uValue % 60;
		if(uSecond < 10)
		{
			string[6] = 0x30;
			string[7] = 0x30 + uSecond;
		}
		else 
		{
			string[6] = 0x30 + uSecond/10;
			string[7] = 0x30 + uSecond%10;
		}
		string[8] = 0x00;
	} 
	else 
		strcpy(string,"--:--");
	return string;
}
long ConvertStringtoTime(char *theString)
{
	char theStatus = 1;
	long uValue = 0;
	WORD uTempValue;
	char *ptr;
	char string[12];

	do {
		if(theString == NULL || strlen(theString) == 0)
			break;
		if(strlen(theString) > 9)
			break;
		strcpy(string,theString);
		if((ptr = strtok(string,"/:-;.,")) == NULL)
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
float CalDisp(float x1,float y1,float x2,float y2)
{
	return (float)sqrt((x1-x2)*(x1 -x2) + (y1 - y2)*(y1 - y2));
}

float PointLine(int m_fPixelPerHour,float xx,float yy,float x1,float y1,float x2,float y2)
{
	float a,b,c, ang1,ang2,ang;
	a = CalDisp(x1,y1,xx,yy);

	//if(a == 0) return 0.0;
	if(abs(a-0) < EMIN) return 0.0;
	b = CalDisp(x2,y2,xx,yy);
	//if(b == 0) return 0.0;
	if(abs(b-0)< EMIN) return 0.0;
	c = CalDisp(x1,y1,x2,y2);

	//add by cf after 10-6 for temporary use
	if(c > 12 * m_fPixelPerHour)
	{
		CRect tempRect;
		tempRect.bottom = yy - 4;
		tempRect.left = xx -4;
		tempRect.right = xx + 4;
		tempRect.top = yy + 4;

		CPoint temp1, temp2;
		temp1.x = x1;
		temp1.y = y1;
		temp2.x = x2;
		temp2.y = y2;
		if(!PtInRect(&tempRect,temp1) && !PtInRect(&tempRect,temp2))
		{
			return c;
		}
	}
	//end of cf 

	//if(c == 0.0) return a;
	if(abs(c-0)< EMIN) return a;
	if(a <b)
	{
		//if(y1 == y2)
		if(abs(y1-y2)<EMIN)
		{
			if(x1<x2)
				ang1 = 0;
			else
				ang1 = (float)PI;
		}
		else
		{
			ang1 = (float)acos((x2 -x1)/c);
			if(y1>y2) ang1 = (float)PI*2 - ang1;
		}

		ang2 = (float)acos((xx - x1)/a);
		if(y1>yy) ang2 = (float)PI*2 - ang2;
		ang = ang2 -ang1;
		if(ang<0) ang = -ang;
		if(ang>PI) ang = (float)PI*2 -ang;
		if(ang>PI/2) return a;
		else
			return (a * (float)sin(ang));
	}
	else
	{
		//if(y1 == y2)
		if(abs(y1-y2)< EMIN)
		{
			if(x1<x2)
				ang1 = (float)PI;
			else
				ang1 = 0;
		}
		else
		{
			ang1 = (float)acos((x1 -x2)/c);
			if(y2>y1) ang1 = (float)PI*2 - ang1;
		}

		ang2 = (float)acos((xx - x2)/b);
		if(y2>yy) ang2 = (float)PI*2 - ang2;
		ang = ang2 -ang1;
		if(ang<0) ang = -ang;
		if(ang>PI) ang = (float)PI*2 -ang;
		if(ang>PI/2) return b;
		else
			return (b * (float)sin(ang));
	}

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

// 转义 " < > & ' 等特殊字符，否则引起xml解析失败
CString GetValidStr(CString str)
{
	CString ret="";
	for(int i=0; i<str.GetLength(); i++)
	{
		char ch=str.GetAt(i);

		if(ch=='<')
			ret+="&lt;";
		else if(ch=='>')
			ret+="&gt;";
		else if(ch=='&')
			ret+="&amp;";
		else if(ch=='\"')
			ret+="&quot;";
		else if(ch=='\'')
			ret+="&apos;";
		else
			ret+=ch;
	}

	return ret;
}
