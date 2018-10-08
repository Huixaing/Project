#pragma once
#include <vector>
#include <afxtempl.h>
using namespace std;

#include ".\ClientSetCfg\tgcontext.h"

#include ".\OcxLoadCfg\EntryCfg.h"
#include ".\OcxLoadCfg\LineCfg.h"
#include ".\OcxLoadCfg\TGDef.h"
#include ".\OcxLoadCfg\SingleTrainCfg.h"
#include ".\OcxLoadCfg\locoltype.h"
#include ".\OcxLoadCfg\traintype.h"

struct TGSection
{
	int id;
	CString name;
	TGSection()
	{
		id=0;
	}
	TGSection(const TGSection& rhs)
	{
		id=rhs.id;
		name=rhs.name;
	}
};

struct TGStation
{
	int  id;
	char name[64];
	char fKmMark[64];
	bool adtime;
	BYTE staclass;
	BYTE property;   //车站性质:-1, 无设备, 0,TDCS, 1, CTC
	TGStation();
	TGStation(const TGStation& rhs);
};

struct BlockCfg
{
	BYTE draw;
	BYTE init;
	BYTE time_pos;
	COLORREF color1;
	COLORREF color2;
	COLORREF color3;
	COLORREF off_color;
	BYTE dlt;

	BlockCfg()
	{
		draw = 1;
		init = 0;
		time_pos = 1;
		color1 = RGB(255,0,0);
		color2 = RGB(0,0,255);
		color3 = color1;
		off_color = color1;
		dlt=0;
	}
};

//指定特定列车在特定站使用特定密码
struct TrainId_Sta_Pwd
{
	char   train_id[10];
	short  station;
	char   pwd[10];
};

struct SIDE_CLR
{
	int type;
	int prior;
	COLORREF clr;
	
	static COLORREF upSideClr;
	static COLORREF downSideClr;
	static int prior_up;
	static int prior_down;
};

struct LateInfo
{
	BYTE flag;
	ULONG shiftid;
};

struct EntryBySide
{
	int station1;
	int station2;
	int exit;
	int entry;
	int side_num;
	int side[64];
	char abs_text[64];
};

struct EntryBySideSection
{
	int count;
	EntryBySide info[16];
};

class NotMeetInfo
{
public:
	WORD	nNoMeetFlag;
	CString	sName;
	CString sShortName;
	
	NotMeetInfo()
	{
		nNoMeetFlag=0;
	}
};

class CTgConfig
{
public:
	CTgConfig(void);
	~CTgConfig(void);
public:
	// 设置车站信息
	bool  SetStationInfo(CString xml,COleControl* ctrl); 
	// 设置区段信息
	bool  SetSectionInfo(CString xml,COleControl* ctrl); 
	// 加载控件配置
	bool  LoadTGFile(COleControl* ctrl);
    // 加载施工符号配置
	void LoadMarkCfgXML(CString FileName, COleControl* ctrl);
	// 加载特殊车站特殊列车密码
	UINT load_train_sta_pwd(CString FileName);
	// 加载股道颜色
	bool LoadSideColorXML(CString fileName);
	// 添加非本台管理车站
	void AddNotThisSectionStation(int station);
	// 加载不受封闭运行图限制的车站列表 no_limit_station.xml
	void AddNoLimitStation(int station);
public:
	void GetStationList(int nSection, int& stationNum, int* stationList);

	short   GetCurrentSectionID();
	void    SetCurrentSectionID(short conid);

	const TGStation* GetStation(int stano);
	
	int GetTrainTypeByTrainNO(CString trainId);

	bool IsCanMoveBasciTrainWhole() { return bBasicCanMoveWholeTrain;}
	void SetIsCanMoveBasciTrainWhole(bool b) {bBasicCanMoveWholeTrain = b;}

	CString get_train_sta_pwd(const CStringArray& trainid, short sta);

	bool isPlatform(int stano, int sideno);
	bool isHighPlatform(int stano, int sideno);
	COLORREF GetSideColor(int nProperty, int nDirect, COLORREF lColor);

	bool IsCrhStopStation(int station) { return m_context.IsCrhStopStation(station);}
	bool IsNotThisSectionStation(int station);

	// 是否显示区间运行时分选择窗口
	void SetShowRuntimeWnd() { m_bShowRuntimeWnd=true; }
	bool IsShowRuntimeWnd() { return m_bShowRuntimeWnd; }

	void SetDisenableSheetTab(int val) 
	{
		if(val>0)
			m_bDisenableSheetTab=true;
		else
			m_bDisenableSheetTab=false; 
	}
	bool IsDisenableSheetTab() { return m_bDisenableSheetTab; }
	bool IsCanMutiMoveOrDel() { return m_bCanMutiMoveOrDel;}
	void SetDisableModifyPlanTm(short val)
	{
		if(val>0)
			m_bDisableModifyPlanTm=true;
		else
			m_bDisableModifyPlanTm=false;
	}
	// 是否禁止修改图定点
	bool IsDisableModifyPlanTm() { return m_bDisableModifyPlanTm;}

	// 区间运缓是否和基本图比较
	void SetRunFastSlowCompareWithBase(short val) 
	{ 
		if(val>0)
			m_bFastOrSlowTimeWithBasic=true;
		else
			m_bFastOrSlowTimeWithBasic=false;
	}
	bool IsRunFastSlowCompareWithBase() { return m_bFastOrSlowTimeWithBasic;}
	
	void SetEraseBeforeModifyTrain(short val) 
	{ 
		if(val>0)
			m_bEraseBeforeModifyTrain=true;
		else
			m_bEraseBeforeModifyTrain=false;
	}
	// 用户操作时，是否先删除
	bool IsEraseBeforeModifyTrain() { return m_bEraseBeforeModifyTrain;}
	
public:
	bool IsNoLimitStation(int station);

	void LoadEntryBySide(CString strFileName); // 处理徐州东--徐州东车场AB线问题
	bool IsThisStationExitBySide(int station1, int station2, int side, int& exit, int& entry, CString& str); // 处理徐州东--徐州东车场AB线问题
	bool IsThisStationEntryBySide(int station1, int station2, int side, int& exit, int& entry, CString& str); // 处理徐州东--徐州东车场AB线问题

public:
	CString GetSectionName(int id);
	BYTE getNotMeetInfoCount() { return m_nNotMeetInfoCount;}
	bool getNotMeetInfoByNo(BYTE idx, NotMeetInfo& info) const;
	CString getNotMeetName(WORD wdFlag) const;
	CString getNotMeetDisplayName(WORD wdFlag) const;
	void AddNotMeetInfo(WORD nNotMeetFlag, CString name, CString short_name);
public:
	COLORREF up_color, down_color;
	// -----------------------------
	bool m_bUseBitmap;
	bool m_bDrawCurrentFlag;
	bool m_bCanMutiMoveOrDel;

	CTgContext m_context;
	SingleTrainCfg m_SingleTrainCfg;

	// -----------------------------
	CLineCfg m_lineCfg;
	CTGDef   m_tgdef;
	EntryCfg m_entrycfg;
    CLocolType m_locoType;
	CTrainTypeInfo m_trainType;
	// ------------------------------
	BlockCfg m_SelfCloseMarkCfg;
	BlockCfg m_AbsInhibitMarkCfg;
	BlockCfg m_AbsSlowMarkCfg;
	BlockCfg m_AbsEleFixMarkCfg;
	BlockCfg m_AbsZhFixMarkCfg;

	BlockCfg m_SideInhibitMarkCfg;
	BlockCfg m_SideSlowMarkCfg;
	BlockCfg m_SideTianchuangMarkCfg;
	BlockCfg m_SideOccupyMarkCfg;

	bool bAllStationActive;
	int  nCurrentSection;
	std::map<int, TGStation> station;

	bool bBasicCanMoveWholeTrain;
	std::map<int, TGSection> sections;
private:
	EntryBySideSection mEntryBySide; // 处理徐州东--徐州东车场AB线问题

private:
	vector<TrainId_Sta_Pwd> train_sta_pwd;
	vector<SIDE_CLR> m_sideClr;
	vector<int> m_notThisSectionStation;
public:
	std::map<CString, LateInfo> m_earlylatetimelog;
	
	std::vector<int> m_noLimitStation; //不受封闭运行图限制的车站列表 int 站号

private:
	bool m_bShowRuntimeWnd; // 是否显示区间运行时分选择窗口
	bool m_bDisenableSheetTab; // 是否禁用右侧属性窗口， 打开对话框中电力属性标志
	bool m_bEraseBeforeModifyTrain; // 是否优化挪线
	bool m_bDisableModifyPlanTm; // 是否禁止修改图定时间
	bool m_bFastOrSlowTimeWithBasic; // 区间运缓是否和基本图比较
	// 禁会标志信息
	BYTE m_nNotMeetInfoCount;
	NotMeetInfo mNotMeetInfo[8];
};

