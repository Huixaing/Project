// AlarmSoundConfig.h: interface for the CAlarmSoundConfig class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ALARMSOUNDCONFIG_H__A9F9A9BB_320D_4737_9F74_41C9A73B276F__INCLUDED_)
#define AFX_ALARMSOUNDCONFIG_H__A9F9A9BB_320D_4737_9F74_41C9A73B276F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning(push)
#pragma warning(disable:4786)
#pragma warning(disable:4800)

#include "CascoConfig.h"
#include <map>
#include <algorithm>

using namespace std;



class CAlarmSoundConfig : public casco::clbth::IOConfig<CAlarmSoundConfig> 
{
public:
	CAlarmSoundConfig();
	virtual ~CAlarmSoundConfig();

	// Accessors
public:
	
public:
	struct IDINFO : casco::clbth::BaseComplexData<IDINFO>
	{
		int nID;
		int nFlag;	//0过滤，1不可过滤，2不过滤
		int nLevel;
		int nType;
		string szAlarmTitle;
		
		int getID(){ return nID; }
		int loadFrom(casco::clbth::LOAD_PARAM& param);
		static string formatString();
		bool save(ofstream& out);
		int IsValidData(myt& unit, string& err);

		bool isCannotChanged() const { return nFlag & 0x02; }
		bool isHide() const { return 0 ==(nFlag & 0x01); }
	};
	struct LEVEL_INFO : casco::clbth::BaseComplexData<LEVEL_INFO>
	{
		int nLevel;
		int nFlag;	//0过滤，1不可过滤，2不过滤
		int nIcon;
		string szTitle;
		
		int getID(){ return nLevel; }
		int loadFrom(casco::clbth::LOAD_PARAM& param);
		static string formatString();
		bool save(ofstream& out);
		int IsValidData(myt& unit, string& err);

		bool isCannotChanged() const { return nFlag & 0x02; }
		bool isHide() const { return 0 ==(nFlag & 0x01); }
	};
	struct TYPE_INFO : casco::clbth::BaseComplexData<TYPE_INFO>
	{
		int nType;
		int nFlag;	//0过滤，1不可过滤，2不过滤
		int nIcon;
		string szTitle;
		
		int getID(){ return nType; }
		int loadFrom(casco::clbth::LOAD_PARAM& param);
		static string formatString();
		bool save(ofstream& out);
		int IsValidData(myt& unit, string& err);

		bool isCannotChanged() const { return nFlag & 0x02; }
		bool isHide() const { return 0 ==(nFlag & 0x01); }
	};
	// 报警声音文件定义
	struct IDFILEINFO : casco::clbth::BaseComplexData<IDFILEINFO>
	{
		int nID;

		vector<int> alarm_ids;
		string soundfile;
		
		int getID(){ return nID; }
		int loadFrom(casco::clbth::LOAD_PARAM& param);
		static string formatString();
		bool save(ofstream& out);
		int IsValidData(myt& unit, string& err);
	};
	
	BEGIN_DATA
		DECLARE_COMPLEXDATA_PART(IDINFO, idInfo)
		DECLARE_COMPLEXDATA_PART(LEVEL_INFO, levelInfo)
		DECLARE_COMPLEXDATA_PART(TYPE_INFO, typeInfo)
		DECLARE_COMPLEXDATA_PART(IDFILEINFO, idfileInfo)
	END_DATA
		
		
		//Attributes whick is functor
	public:
		
		// Attributes
	public:
		BOOL  loadProfile(CString szFileName);
		bool  saveProfile();
		
	public:
		CString GetSoundFileName(int nAlarmID);
	private:
		CString m_szFilename, m_szLocalFilename;
};

#pragma warning(pop)

#endif // !defined(AFX_ALARMSOUNDCONFIG_H__A9F9A9BB_320D_4737_9F74_41C9A73B276F__INCLUDED_)
