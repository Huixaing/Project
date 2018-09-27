// ConfigFile.h: interface for the CConfigFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONFIGFILE_H__B41B6DAC_6249_4002_9B47_B6CA22BED0DB__INCLUDED_)
#define AFX_CONFIGFILE_H__B41B6DAC_6249_4002_9B47_B6CA22BED0DB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class  CConfigFile
{
public:
	short GetSectionDataLine(char *dataline);
	short GetSectionData(char *sectionname, char *dataname, char *buf);

	short GetDataValue(char *dataline, const char* dataname, char *param);
	short GetDataValue(char *dataline, const char* dataname, short *param);
	short GetDataValue(char *dataline, const char* dataname, unsigned short *param);
	short GetDataValue(char *dataline, const char* dataname, long *param);
	short GetDataValue(char *dataline, const char* dataname, BYTE *param);
	short GetDataValue(char *dataline, const char* dataname, int *param);
	short GetDataValue(char *dataline, const char* dataname, unsigned int *param);
	short GetDataValue(char *dataline, const char* dataname, unsigned long *param);
	
	short GetDataValue(char *dataline, int index, char *param);
	short GetDataValue(char *dataline, int index, short *param);
	short GetDataValue(char *dataline, int index, unsigned short *param);
	short GetDataValue(char *dataline, int index, long *param);
	short GetDataValue(char *dataline, int index, BYTE *param);
	short GetDataValue(char *dataline, int index, int *param);
	short GetDataValue(char *dataline, int index, unsigned int *param);
	short GetDataValue(char *dataline, int index, unsigned long *param);
	short GetDataName(char *dataline, char *dataname);
	short GetSectionName(char *buf, char *sectionname);
	short ReOpenFile();
	short CloseFile();
	short GetSelectSection(char *sectionname);
	short GetFirstLine(char *buf);
	short GetLineType(char *buf);
	short GetNextLine(char *buf);
	short SetFileName(const char *cFileName);
public:
	CConfigFile();
	virtual ~CConfigFile(){};
	void SetIntervalChar(char nChar);
private:

protected:
	char m_cTempLine[512];
	int m_nTempPoint;
	FILE * fp;
	char m_cFileName[128];
	char m_nIntervalChar;
};

#endif // !defined(AFX_CONFIGFILE_H__B41B6DAC_6249_4002_9B47_B6CA22BED0DB__INCLUDED_)
