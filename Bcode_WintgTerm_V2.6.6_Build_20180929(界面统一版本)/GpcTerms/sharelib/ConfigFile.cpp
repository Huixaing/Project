// ConfigFile.cpp: implementation of the CConfigFile class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ConfigFile.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CConfigFile::CConfigFile()
{
	fp = NULL;
	ZeroMemory(m_cFileName,sizeof(m_cFileName));
	m_nIntervalChar = 0;
}

short CConfigFile::SetFileName(const char *cFileName)
{
	strcpy(m_cFileName, cFileName);
	fp = fopen(m_cFileName, "r");
	if (fp != NULL)
		return 0;
	return -1;
}

short CConfigFile::GetNextLine(char *buf)
{
	char line[512];
	CString temp;
	short rs;
	if(fp == NULL)
	{
		rs = -1;
	}
	else if (fgets(line, 512, fp) == NULL)
	{
		fclose(fp);
		fp = NULL;
		rs = -1;
	}
	else
	{
		temp.Format("%s", line);
		int np = temp.Find("//");
		if(np != -1)
		{
			temp = temp.Left(np);
		}
		temp.TrimLeft();
		temp.TrimRight();
		strcpy(buf, temp);
		strcpy(m_cTempLine, temp);
		rs = 0;
	}
	return rs;
}


short CConfigFile::GetLineType(char *buf)
{
	CString temp;
	temp.Format( "%s", buf);
	short nType;
	if (temp.IsEmpty()){
		nType = 5;
	}else if ((temp.ReverseFind('[') == 0) &&(temp.Find(']') == temp.GetLength() - 1)){
		nType = 1;
	}else if (temp.GetAt(0) == ';'){
		nType = 2;
	}else if ((temp.Find('=') > 0) && (temp.ReverseFind('=') == temp.Find('='))){
		nType = 0;
	}else if (temp.Find('=') > 0) {
		nType = 10;
	}
	else
	{
		nType = -1;
	}
	return nType;
}

short CConfigFile::GetFirstLine(char *buf)
{
	if(fp != NULL)
	{
		fclose(fp);
	}
	fp = NULL;
	fp = fopen(m_cFileName,"r");
	if (fp != NULL)
		GetNextLine(buf);
	else
		return -1;
	return 0;
}

short CConfigFile::GetSelectSection(char *sectionname)
{
	char temp[512];
	char tempname[512];
	
	ZeroMemory(temp,sizeof(temp));
	ZeroMemory(tempname,sizeof(tempname));
	ReOpenFile();
	short i = 0;
	while (GetNextLine(temp) >= 0)
	{
		GetSectionName(temp, tempname);
		if (strcmp(sectionname, tempname) == 0)
			return i;
		i++;
	}
	return -1;
}

short CConfigFile::ReOpenFile()
{
	if (fp != NULL) fclose(fp);
	fp = NULL;
	fp = fopen(m_cFileName,"r");
	if (fp == NULL)
		return -1;
	return 0;
}

short CConfigFile::CloseFile(){
	if(fp != NULL)
		fclose(fp);
	fp = NULL;
	return 0;
}

short CConfigFile::GetSectionName(char *sectionline, char *sectionname)
{
	if (GetLineType(sectionline) == 1)
	{
		CString temp;
		temp.Format("%s", sectionline);
		{
			CString t_sSet("[] ");
			
			int np = temp.FindOneOf(t_sSet);
			while(np != -1){
				temp = temp.Left(np) + temp.Mid(np+1);
				np = temp.FindOneOf(t_sSet);
			}		
		}
		strcpy(sectionname, temp);
		return 0;
	}
	return -1;
}

short CConfigFile::GetSectionData(char *sectionname, char *dataname, char *buf)
{
	if (GetSelectSection(sectionname) < 0)
		return -1;
	char temp[512];
	int linetype;
	char tempdataname[512];
	while (GetNextLine(temp) >= 0)
	{
		linetype = GetLineType(temp);
		if (linetype == 1)
			break;
		if (linetype == 0)
		{
			GetDataName(temp, tempdataname);
			if (strcmp(tempdataname, dataname) == 0)
			{
				strcpy(buf, temp);
				return 0;
			}
		}
	}
	return -1;
}

short CConfigFile::GetDataName(char *dataline, char *dataname)
{
	CString temp, tempname;
	int  evalnum;
	temp.Format("%s", dataline);
	int linetype = GetLineType(dataline);
	if ((linetype == 0) || (linetype == 10))
	{
		evalnum = temp.FindOneOf("=");
		tempname = temp.Left(evalnum);
		tempname.TrimLeft();
		tempname.TrimRight();
		strcpy(dataname, tempname);
		return 0;
	}
	return -1;
}

short CConfigFile::GetDataValue(char *dataline,const char* dataname, char *param)
{
	int t_nType = GetLineType(dataline);
	if ((t_nType != 0) && (t_nType != 10))
		return -1;
	char indata[512],*pindata = indata,*ptmp;
	ZeroMemory(indata,sizeof(indata));
	strncpy(indata,dataline,511);
	CString temp, tempparam;
	int i, j, k = 0;
	temp.Format( "%s", indata);
	
	int rt = -1;
	char indataname[512];
	while (1)
	{
		ZeroMemory(indataname,sizeof(indataname));
		GetDataName(pindata,indataname);
		if (strcmp(indataname,dataname) == 0)
		{
			temp.Format("%s",pindata);
			i = temp.Find(',');
			if(i == -1)
			{
				i = temp.GetLength();
			}
			j = temp.Find('=');		
			if(i > j)
			{
				tempparam = temp.Mid(j+1,i-j-1);
				tempparam.TrimLeft();
				tempparam.TrimRight();
				strcpy(param, tempparam);
				rt = 0;
				break;
			}
		}
		ptmp = strchr(pindata,',');
		if(ptmp == NULL)
		{
			break;
		}
		ptmp++;
		if(strchr(ptmp,'=') == NULL)
		{
			break;
		}
		pindata = ptmp;
	}
	return rt;
}

short CConfigFile::GetDataValue(char *dataline, const char* dataname, short *param)
{
	char temp[512];
	short rc = GetDataValue(dataline, dataname, temp);
	if (rc >= 0)
	{
		*param = (short)atoi(temp);
	}
	return rc;	
}

short CConfigFile::GetDataValue(char *dataline, const char* dataname, unsigned short *param)
{
	char temp[512];
	short rc = GetDataValue(dataline, dataname, temp);
	if (rc >= 0)
	{
		*param = (unsigned short)atoi(temp);
	}
	return rc;
}

short CConfigFile::GetDataValue(char *dataline, const char* dataname, long *param)
{
	char temp[512];
	short rc = GetDataValue(dataline, dataname, temp);
	if (rc >= 0)
	{
		*param = (long)atoi(temp);
	}
	return rc;
}

short CConfigFile::GetDataValue(char *dataline, const char* dataname, BYTE *param)
{
	char temp[512];
	short rc = GetDataValue(dataline, dataname, temp);
	if (rc >= 0)
	{
		*param = (BYTE)atoi(temp);
	}
	return rc;
}

short CConfigFile::GetDataValue(char *dataline, const char* dataname, int *param)
{
	char temp[512];
	short rc = GetDataValue(dataline, dataname, temp);
	if (rc >= 0)
	{
		*param = (int)atoi(temp);
	}
	return rc;
}

short CConfigFile::GetDataValue(char *dataline, const char* dataname, unsigned int *param)
{
	char temp[512];
	short rc = GetDataValue(dataline, dataname, temp);
	if (rc >= 0)
	{
		*param = (unsigned int)atoi(temp);
	}
	return rc;
}

short CConfigFile::GetDataValue(char *dataline, const char* dataname, unsigned long *param)
{
	char temp[512];
	short rc = GetDataValue(dataline, dataname, temp);
	if (rc >= 0)
	{
		*param = (unsigned long)atoi(temp);
	}
	return rc;
}

short CConfigFile::GetDataValue(char *dataline, int index, char *param)
{
	if (GetLineType(dataline) != 0)
		return -1;
	if (index < 0)
		return -2;
	char indata[512];
	ZeroMemory(indata,sizeof(indata));
	strncpy(indata,dataline,511);
	if(m_nIntervalChar != 0)
	{
		char *p = indata;
		int nFlag = 0;
		while(*p != NULL)
		{
			if(*p == '=')
			{
				nFlag = 1;
			}
			else if((*p == m_nIntervalChar) && (nFlag == 2))
			{
				*p = ',';
				nFlag = 1;
			}
			else if((*p != m_nIntervalChar) && (nFlag == 1))
			{
				nFlag = 2;
			}
			p++;
		}
	}
	CString temp, tempparam;
	int i, j, k = 0;
	temp.Format( "%s", indata);
	
	int num = temp.Replace(',', '&');
	temp.Replace('&', ',');
	if (index > num)
		return -3;
	i = j = temp.Find('=');
	tempparam = temp.Left(i);
	j = i + 1;
	while ((i < temp.GetLength()) && (k <= index))
	{
		if ((j >= temp.ReverseFind( ',')) || (temp.ReverseFind( ',') < 0))
			i = temp.GetLength();
		else
			i = temp.Find( ",", j);
		if (k == index)
		{
			tempparam = temp.Mid( j, i - j);
			tempparam.TrimLeft();
			tempparam.TrimRight();
			strcpy(param, tempparam);
			break;
		}
		k++;
		j = i + 1;
	}
	j = temp.Find( ",", i);
	return 0;
}

short CConfigFile::GetDataValue(char *dataline, int index, short *param)
{
	char temp[512];
	short rc = GetDataValue(dataline, index, temp);
	if (rc >= 0)
	{
		*param = (short)atoi(temp);
	}
	return rc;
}

short CConfigFile::GetDataValue(char *dataline, int index, unsigned short *param)
{
	char temp[512];
	short rc = GetDataValue(dataline, index, temp);
	if (rc >= 0)
	{
		*param = (unsigned short)atoi(temp);
	}
	return rc;
}

short CConfigFile::GetDataValue(char *dataline, int index, int *param)
{
	char temp[512];
	short rc = GetDataValue(dataline, index, temp);
	if (rc >= 0)
	{
		*param = (int)atoi(temp);
	}
	return rc;
}

short CConfigFile::GetDataValue(char *dataline, int index, unsigned int *param)
{
	char temp[512];
	short rc = GetDataValue(dataline, index, temp);
	if (rc >= 0)
	{
		*param = (unsigned int)atoi(temp);
	}
	return rc;
}

short CConfigFile::GetDataValue(char *dataline, int index, unsigned long *param)
{
	char temp[512];
	short rc = GetDataValue(dataline, index, temp);
	if (rc >= 0)
	{
		*param = (unsigned long)atoi(temp);
	}
	return rc;
}

short CConfigFile::GetDataValue(char *dataline, int index, long *param)
{
	char temp[512];
	short rc = GetDataValue(dataline, index, temp);
	if (rc >= 0)
	{
		*param = (long)atoi(temp);
	}
	return rc;
}

short CConfigFile::GetDataValue(char *dataline, int index, BYTE *param)
{
	char temp[512];
	short rc = GetDataValue(dataline, index, temp);
	if (rc >= 0)
	{
		*param = (BYTE)atoi(temp);
	}
	return rc;
}

short CConfigFile::GetSectionDataLine(char *dataline)
{
	char temp[512];
	while (GetNextLine(temp) >= 0)
	{
		int linetype = GetLineType(temp);
		if (linetype == 1)
			return -2;
		if ((linetype == 0)||(linetype==10))
		{
			strcpy(dataline, temp);
			return 0;
		}
	}
	return -1;
}

void CConfigFile::SetIntervalChar(char nChar)
{
	m_nIntervalChar = nChar;
}