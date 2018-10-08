// AlarmSoundConfig.cpp: implementation of the CAlarmSoundConfig class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AlarmSoundConfig.h"
#include <algorithm>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif



// ================================================================
// alarm sound file setting
// ================================================================
IMPLEMENT_COMPLEXDATA_PART(CAlarmSoundConfig, IDINFO, idInfo, ALARM_ID, "Alarm ID define")

int CAlarmSoundConfig::IDINFO::loadFrom(casco::clbth::LOAD_PARAM& param)
{
	char ptr[256];
	param.in.getline(ptr, 256, '\n');
	char *token;
	char seps[]=" ,\t\\/=\n\r()";

	token = strtok(ptr, "= \t\r\n");
	if(NULL == token)
		return casco::clbth::S_XLY_FALSE;
	nID = atoi(token);

	token = strtok(NULL, "= \t\r\n");
	if(NULL == token)
		return casco::clbth::S_XLY_FALSE;
	nFlag = atoi(token) & 0x03;

	token = strtok(NULL, seps);
	if(NULL == token)
		return casco::clbth::S_XLY_OK;
	nLevel = atoi(token);

	token = strtok(NULL, seps);
	if(NULL == token)
		return casco::clbth::S_XLY_OK;
	nType = atoi(token);

	token = strtok(NULL, ",\r\n");
	if(token)
	{
		szAlarmTitle = token;
	}

	return casco::clbth::S_XLY_OK;
}

string CAlarmSoundConfig::IDINFO::formatString()
{
	return "; 格式: id=flag，level, type, title\n"
		"; flag: 0x01位（0-不显示，1-显示）；0x02位（0-可变更，1-不可变更）\n"
		"; leve: 0-自律机定义，其它为相应级别\n"
		"; type: 0-自律机定义，1-TRAIN，2-SYS\n"
		"; title：报警名称";
}

bool CAlarmSoundConfig::IDINFO::save(ofstream& out)
{
		out << "\t" << nID << "=" << nFlag << ",\t" 
			<< nLevel << ",\t" << nType << ",\t"
			<< szAlarmTitle;
	return true;
}

int CAlarmSoundConfig::IDINFO::IsValidData(myt& unit, string& err)
{
	return casco::clbth::S_XLY_OK;
}



// ================================================================
// alarm level setting
// ================================================================
IMPLEMENT_COMPLEXDATA_PART(CAlarmSoundConfig, LEVEL_INFO, levelInfo, ALARM_LEVEL, "Alarm LEVEL define")

int CAlarmSoundConfig::LEVEL_INFO::loadFrom(casco::clbth::LOAD_PARAM& param)
{
	char ptr[256];
	param.in.getline(ptr, 256, '\n');
	char *token;
	char seps[]=" ,\t\\/=\n\r()";

	token = strtok(ptr, "= \t\r\n");
	if(NULL == token)
		return casco::clbth::S_XLY_FALSE;
	nLevel = atoi(token);

	token = strtok(NULL, "= \t\r\n");
	if(NULL == token)
		return casco::clbth::S_XLY_FALSE;
	nFlag = atoi(token) & 0x03;

	token = strtok(NULL, seps);
	if(NULL == token)
		return casco::clbth::S_XLY_OK;
	nIcon = atoi(token);
	if(0 > nIcon || 2 < nIcon)
		nIcon = 2;

	token = strtok(NULL, ",\r\n");
	if(token)
		szTitle = token;

	return casco::clbth::S_XLY_OK;
}

string CAlarmSoundConfig::LEVEL_INFO::formatString()
{
	return "; 格式: level=flag，ico, title\n"
		"; level: 级别1、2、3，其它级别未定义，同于3级\n"
		"; flag: 0x01位（0-不显示，1-显示）；0x02位（0-可变更，1-不可变更）\n"
		"; ico: 未定义\n"
		"; title: 未定义";

}

bool CAlarmSoundConfig::LEVEL_INFO::save(ofstream& out)
{
		out << "\t" << nLevel << "=" << nFlag << ",\t" 
			<< nIcon << ",\t" << szTitle;
	return true;
}

int CAlarmSoundConfig::LEVEL_INFO::IsValidData(myt& unit, string& err)
{
	return casco::clbth::S_XLY_OK;
}



// ================================================================
// alarm type setting
// ================================================================
IMPLEMENT_COMPLEXDATA_PART(CAlarmSoundConfig, TYPE_INFO, typeInfo, ALARM_TYPE, "Alarm TYPE define")

int CAlarmSoundConfig::TYPE_INFO::loadFrom(casco::clbth::LOAD_PARAM& param)
{
	char ptr[256];
	param.in.getline(ptr, 256, '\n');
	char *token;
	char seps[]=" ,\t\\/=\n\r()";

	token = strtok(ptr, "= \t\r\n");
	if(NULL == token)
		return casco::clbth::S_XLY_FALSE;
	nType = atoi(token);

	token = strtok(NULL, "= \t\r\n");
	if(NULL == token)
		return casco::clbth::S_XLY_FALSE;
	nFlag = atoi(token) & 0x03;

	token = strtok(NULL, seps);
	if(NULL == token)
		return casco::clbth::S_XLY_OK;
	nIcon = atoi(token);
	if(0 > nIcon || 2 < nIcon)
		nIcon = 2;

	token = strtok(NULL, ",\r\n");
	if(token)
		szTitle = token;

	return casco::clbth::S_XLY_OK;
}

string CAlarmSoundConfig::TYPE_INFO::formatString()
{
	return "; 格式: type=flag，ico, title\n"
		"; type: 报警类型\n"
		"; flag: 0x01位（0-不显示，1-显示）；0x02位（0-可变更，1-不可变更）\n"
		"; ico: 未定义\n"
		"; title: 未定义";

}

bool CAlarmSoundConfig::TYPE_INFO::save(ofstream& out)
{
		out << "\t" << nType << "=" << nFlag << ",\t" 
			<< nIcon << ",\t" << szTitle;
	return true;
}

int CAlarmSoundConfig::TYPE_INFO::IsValidData(myt& unit, string& err)
{
	return casco::clbth::S_XLY_OK;
}



// ================================================================
// alarm sound file setting
// ================================================================
IMPLEMENT_COMPLEXDATA_PART(CAlarmSoundConfig, IDFILEINFO, idfileInfo, ALARMIDS_SOUNDFILE, "Alarm sound file define")

int CAlarmSoundConfig::IDFILEINFO::loadFrom(casco::clbth::LOAD_PARAM& param)
{
	char ptr[256];
	param.in.getline(ptr, 256, '\n');
	char *token;
	char seps[]=" ,\t\\/=\n\r()";

	static int __id = 0;
	nID = __id++;

	int nAlarmID = 0;
	
	token = strtok(ptr, "= /,");
	if(NULL == token)
		return casco::clbth::S_XLY_FALSE;
	int num = atoi(token);
	for(int i=0; i<num; i++)
	{
		token = strtok(NULL, "/ ,");
		if(NULL == token)
			return casco::clbth::S_XLY_FALSE;
		nAlarmID = atoi(token);
		alarm_ids.push_back(nAlarmID);
	}

	token = strtok(NULL, " \n\r\t");
	if(NULL == token)
		return casco::clbth::S_XLY_FALSE;
	soundfile = token;
	
	return casco::clbth::S_XLY_OK;
}

string CAlarmSoundConfig::IDFILEINFO::formatString()
{
	return "; 格式: =个数{/AlarmID}, 声音文件名称";
}

bool CAlarmSoundConfig::IDFILEINFO::save(ofstream& out)
{
	int num = alarm_ids.size();
	out << "\t=" << num;
	for(int i=0; i<num; i++)
	{
		out << "/" << alarm_ids[i];
	}

	out << ",\t" << soundfile.c_str();
	return true;
}

int CAlarmSoundConfig::IDFILEINFO::IsValidData(myt& unit, string& err)
{
	return casco::clbth::S_XLY_OK;
}
// ================================================================








//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//IMPLEMENT_SINGLETON(CConfigData)

CAlarmSoundConfig::CAlarmSoundConfig()
{
	INITIALIZE_SIMPLEDATA(this);
}

CAlarmSoundConfig::~CAlarmSoundConfig()
{
	
}

#include <sys/types.h>
#include <sys/stat.h>
#include <io.h>
#include <gpcdef.h>
#include <sysshare.h>

BOOL CAlarmSoundConfig::loadProfile(CString szPathName)
{
	CString szPath;
	szPath.Format("%s\\sound\\alarm", szPathName);

	m_szFilename = szPath + "\\Alarm.CFG";

	m_szLocalFilename.Format("%s\\Alarm_%s.CFG", szPath, gpcdef.GetCurUserName());

	CString filename;
	if(0 != _access(m_szLocalFilename, 0))
	{
		filename = m_szFilename;
		sysprint(0, 0, "[ALARM] 使用通用配置文件 %s", filename);
	}
	else
	{
		filename = m_szLocalFilename;
		sysprint(0, 0, "[ALARM] 使用个性化配置文件 %s", filename);
	}

	if(0 != _access(filename, 04))
	{
		sysprint(0, 0, "[ALARM] 配置文件 %s 不可读", filename);
		return FALSE;
	}

	if(load((LPCSTR)filename))
	{
		return TRUE;
	}

	sysprint(5, 0, "[ALARM] 配置文件 %s 不正确", filename);

	return FALSE;
}

bool CAlarmSoundConfig::saveProfile()
{
	//if(0 != _access(m_szLocalFilename, 0))
	//{
	//	if(0 != _access(m_szFilename, 04))
	//	{
	//		sysprint(5, 0, "[ALARM] 通用配置文件 %s 不可读，无法生成个性化配置 %s", m_szFilename, m_szLocalFilename);
	//		return false;
	//	}

	//	if(!CopyFile(m_szFilename, m_szLocalFilename, FALSE))
	//	{
	//		sysprint(5, 0, "[ALARM] 通用配置文件 %s 到个性化配文件 %s 出错", m_szFilename, m_szLocalFilename);
	//		return false;
	//	}
	//	sysprint(0, 0, "[ALARM] 复制通用配置文件到个性化配置文件 %s", m_szLocalFilename);
	//}

	if(0 == _access(m_szLocalFilename, 0))
	{
		if(0 != _access(m_szLocalFilename, 06))
		{
			if(0 != _chmod(m_szLocalFilename, _S_IREAD | _S_IWRITE))
			{
				sysprint(5, 0, "[ALARM] 个性化配置文件 %s 不可写，并且无法修改文件可写属性", m_szLocalFilename);
				return false;
			}
		}
	}

	if(save((LPCSTR)m_szLocalFilename))
	{
		sysprint(5, 0, "[ALARM] 保存个性化配置文件 %s",  m_szLocalFilename);
	}
	else
	{
		sysprint(5, 0, "[ALARM] 保存个性化配置文件 %s 失败",  m_szLocalFilename);
		return false;
	}

	return true;
}

CString CAlarmSoundConfig::GetSoundFileName(int nAlarmID)
{
	CString sndFile;

	for(int i=0; i<idfileInfo.size(); i++)
	{
		const vector<int>& vec = idfileInfo[i].alarm_ids;
		if(vec.end() != find(vec.begin(), vec.end(), nAlarmID))
		{
			sndFile = idfileInfo[i].soundfile.c_str();
			return sndFile;
		}
	}
	return sndFile;
}

