#ifndef  __INCLUDE__BASIC_PARAM__
#define  __INCLUDE__BASIC_PARAM__
#include "tg_structdef.h"
#include "tg_passway.h"
#include "stationdef.h"
#include "tg_function_config.h"
#include "tg_train_type.h"
#include "tg_gaptime_config.h"
#include "locoltype.h"
#include <map>

#define MAX_LIMIT_COUNT 256

struct TrainAbsTime 
{
	long  abs_number;
	char  direction;
	char  type;
	short stop_time;
	short start_time;
	short run_time;
	int   station1;
	int   station2;
	short entry1;
	short entry2;
	/// 用户改变前的原始数据
	short orig_stop_time;
	short orig_start_time;
	short orig_run_time;

	// 1 down direction, 0 up direction
	TrainAbsTime()
	{
		abs_number=0;
		direction=0;
		type=0;
		stop_time=0;
		start_time=0;
		run_time=0;
		station1=-1;
		station2=-1;
		entry1=-1;
		entry2=-1;
		orig_stop_time=0;
		orig_start_time=0;
		orig_run_time=0;
	}
};

class AbsTime
{
public:
	long  abs_number;
	std::vector<TrainAbsTime> trainAbsTime;
public:
	AbsTime();
	AbsTime(const AbsTime& rhs);
	AbsTime& operator=(const AbsTime& rhs);
	~AbsTime();

	void Init();
};

class CTGBasicParam{
	enum{MAX_PRIOITY_COUNT=200};
public:

	typedef TrainAbsTime *ABS_STRUCT_PTR;
	map<long, AbsTime> m_aAbsTimes;   // 区间运行时分
	typedef map<long,AbsTime>::iterator IterMapAbsTime;
	
	struct AbsLineInfo
	{
		int line_no;
		int direct;
	};

	struct AbsAttrib{
		long   line_no;
		ushort direction;
		long   abs_no;
		ushort start_station;
		ushort end_station;
		char abs_name[64];
		long  rela_data; // 用于和TMIS对应
		int   cfgorder; // tg.def中配置顺序
		AbsAttrib()
		{
			line_no=0;
			direction=0;
			abs_no=0;
			start_station=0;
			end_station=0;
			memset(abs_name,0,sizeof(abs_name));
			rela_data=0; 
			cfgorder=0; 
		}
	};
	map<long,AbsAttrib> m_aAbsAttrib;   // 存放tg.def中区间线别
	
	map<DWORD, DWORD> m_map_gpc_lineno;
	map<long,AbsAttrib> m_aTmisAbsAttrib;   // TMIS的区间编号表
	
	struct StationEntryAbsnum{
		ushort entry_num;
		ushort abs_num;
		ushort dir;
		ushort jiefa_flag;
		ushort usSpared;
	};
	typedef struct CadStation{
		int         nStationNo;
		CStationDef *pStationDef;
		int   nEntryCount;
		struct StationEntryAbsnum entry[MAX_ENTRY_COUNT];
		CadStation()
		{
			nStationNo=0;
			pStationDef=NULL;
			nEntryCount=0;
			memset(entry,0,sizeof(entry));
		}
	}CAD_STATION;
	map<int,CAD_STATION> m_aCadStations;
	typedef map<int,CAD_STATION>::iterator IterMapStation;
	
	CString    m_szDataPath;
	CTGFunctionConfig *m_pConfig;

	CLocolType m_aLocoTypes;
	
	struct CarKind
	{
		int  count;
		char name[24];
		char shortName[16];
		CarKind()
		{
			count=0;
			memset(this, 0, sizeof(CarKind));
		}
	};
	struct CarType
	{
		char name[24];
		char shortName[12];
		char plot_short_name[12];
		std::vector<CarKind> carKind;
		CarType()
		{
			memset(name, 0, sizeof(name));
			memset(shortName, 0, sizeof(shortName));
			memset(plot_short_name, 0, sizeof(plot_short_name));
			
			carKind.clear();
		}
		~CarType(){}
		CarType(const CarType& rhs)
		{
			memset(name, 0, sizeof(name));
			memset(shortName, 0, sizeof(shortName));
			memset(plot_short_name, 0, sizeof(plot_short_name));

			strncpy(name, rhs.name, sizeof(rhs.name));
			strncpy(shortName, rhs.shortName, sizeof(rhs.shortName));
			strncpy(plot_short_name, rhs.plot_short_name, sizeof(rhs.plot_short_name));
			
			carKind.assign(rhs.carKind.begin(), rhs.carKind.end());
		}
		CarType& operator=(const CarType& rhs)
		{
			if(this == &rhs)
				return *this;
			memset(name, 0, sizeof(name));
			memset(shortName, 0, sizeof(shortName));
			memset(plot_short_name, 0, sizeof(plot_short_name));

			strncpy(name, rhs.name, sizeof(rhs.name));
			strncpy(shortName, rhs.shortName, sizeof(rhs.shortName));
			strncpy(plot_short_name, rhs.plot_short_name, sizeof(rhs.plot_short_name));

			carKind.clear();

			carKind.assign(rhs.carKind.begin(), rhs.carKind.end());

			return *this;
		}
	};
	struct LimitCfg
	{
		int nSide1;
		int nSide2;

		//超限车标记
		//=1 不能同时接两趟超限车 =2 不能接超限车和其他任何车
		int nFlag;

		LimitCfg()
		{
			nSide1=0;
			nSide2=0;
			nFlag=2;
		}
	};
	struct StaSideLimit
	{
		int nStation;
		int nLimitCount;
		LimitCfg mSideList[MAX_LIMIT_COUNT];

		StaSideLimit()
		{
			nStation=0;
			nLimitCount=0;
		}
	};

	// 站存车配置
public:
	std::vector<CarType> m_carKindType;// 站存车配置
	CStringArray m_aryWorkExplain;// 站存车配置
	CStringArray m_aryCarrApply;// 站存车配置
	std::map<int, int> m_convert;// 站存车配置
	void ResetCarCount();

	void  GetCarTypeAndKindIndex(BYTE type, BYTE& nSel, BYTE& nSubSel);
    WORD  GetCarriageType(BYTE nSel, BYTE nSubSel);
	CString GetCarTypeShortName(BYTE nSel);
	CString GetCarriageExplain(BYTE index);
	CString GetCarShortName(BYTE nSel, BYTE nSubSel);
public:
	CGetPasswayInfor   m_cPasswayInfo;
	CGetTrain_TypeInfor m_cTrainTypeInfo;
	CGetGapTimeInfor    m_cGapTimeInfo;
public:
	CTGBasicParam(CTGFunctionConfig *pcfg);
	~CTGBasicParam();
	int  GetAbstimeCount() {return m_aAbsTimes.size();}
 	
    void DeinitData();
    BOOL InitData();
  	BOOL  LoadAbsTimeData(const char* cfgfile);
	BOOL  LoadUserAbsTimeData(const char* file0=NULL); // 装载用户改变后的数据
	BOOL  SaveUserAbsTimeData(const char* file0=NULL); // 保存用户改变后的数据

	BOOL  SetAbsTimeData(TrainAbsTime& tm);
	
private:
	BOOL  LoadAbsAttrib(const char *cfgfile);
	BOOL  LoadTmisAbsAttrib(const char *cfgfile);
	BOOL  LoadStationDef(const char *cfgfile);
	BOOL  LoadEntryConfig(const char *cfgfile);
	BOOL  LoadPassway(const char *cfgfile);
	BOOL  LoadPrioConfig(const char *filename);
	BOOL  LoadLocoTypeConfig(const char *filename);
	BOOL  LoadGapTimeConfig(const char *file);
	BOOL  LoadStaNowCar(CString filename);

	CAD_STATION *GetStationPtr(int stno);
	
	BOOL  SetAbsTimeStationInfo(int absnumber,int stno,int entry);
	CAD_STATION *CreateStation(int stno);
	
	BYTE  GetTrainDirection(STATION_NO station, ENTRY_ID entry, TRAIN_IN_OUT in_out);
	BOOL  check_side_utility(const CStationDef* sta_def, const TG_TrainInfo &coninfo, const TG_TrainRecord &rec, SIDE_ID side);
	BOOL verify_side_train_arrive_direction(const CStationDef* sta_def_ptr, const TG_TrainRecord &rec, SIDE_ID side);
	BOOL verify_side_train_depart_direction(const CStationDef* sta_def_ptr, const TG_TrainRecord &rec, SIDE_ID side);
	BOOL verify_side_train_arrive_type(const CStationDef* sta_def_ptr, const TG_TrainRecord &rec, SIDE_ID side, BYTE type, BYTE run_type);
	BOOL verify_side_train_depart_type(const CStationDef* sta_def_ptr, const TG_TrainRecord &rec, SIDE_ID side, BYTE type, BYTE run_type);
	BOOL verify_side_train_length(const CStationDef* sta_def_ptr, const TG_TrainRecord &rec, SIDE_ID side, USHORT length, BYTE length_rank);
	BOOL verify_side_train_space(const CStationDef* sta_def_ptr, const TG_TrainRecord &rec, SIDE_ID side, BYTE space_rank);
	BOOL verify_side_train_elec(const CStationDef* sta_def_ptr, const TG_TrainRecord &rec, SIDE_ID side, BYTE engine_count, const BYTE *engine_type);
	BOOL verify_side_train_equipment(const CStationDef* sta_def_ptr, const TG_TrainRecord &rec, SIDE_ID side, const TG_TrainInfo &coninfo);
	BOOL verify_side_start_difficult(const CStationDef* sta_def_ptr, const TG_TrainRecord &rec, SIDE_ID side, const TG_TrainInfo &coninfo);
	BOOL verify_side_not_stop_crh(const CStationDef* sta_def_ptr, SIDE_ID side, const TG_TrainInfo &coninfo);
	
	BOOL VerifySideTrainDirection(const TG_TrainRecord &rec);
	BOOL VerifySideTrainArriveDirection(const TG_TrainRecord &rec);
	BOOL VerifySideTrainDepartDirection(const TG_TrainRecord &rec);
	BOOL VerifySideTrainArriveType(const TG_TrainRecord &rec, BYTE type, BYTE run_type);
	BOOL VerifySideTrainDepartType(const TG_TrainRecord &rec, BYTE type, BYTE run_type);
	BOOL VerifySideTrainLength(const TG_TrainRecord &rec, USHORT length, BYTE length_rank);
	BOOL VerifySideTrainSpace(const TG_TrainRecord &rec, BYTE space_rank);
	BOOL VerifySideTrainElec(const TG_TrainRecord &rec, BYTE engine_count, const BYTE *engine_type);
	BOOL VerifySideTrainEquipment(const TG_TrainRecord &rec, const TG_TrainInfo &coninfo);
	BOOL VerifySideStartDifficult(const TG_TrainRecord &rec, const TG_TrainInfo &coninfo);
	BOOL VerifyArriveDepartTime(TIME ltime, long diff=0);

public:
	BOOL VerifyNotCRHStopSide(int side, int station, const TG_TrainInfo &coninfo);

public:
	ABS_STRUCT_PTR GetAbsTimePtr(int absnumber,int type,int dir);
	BOOL  GetTrainPrio(const TRAIN_NO train_no,BYTE *type, BYTE *run_type, BYTE *prio);
	BOOL  GetTrainTypeDef(short  train_type,BYTE *run_type,BYTE *prio,BYTE *locotype);
	int   CalculateTrainRunTime(STATION_NO station, ENTRY_ID entry, TRAIN_IN_OUT in_out, BYTE trn_type);
	int   CalculateTrainStopExtra(STATION_NO station, ENTRY_ID entry, TRAIN_IN_OUT in_out, BYTE trn_type);
	int   CalculateTrainStartExtra(STATION_NO station, ENTRY_ID entry, TRAIN_IN_OUT in_out, BYTE trn_type);

	BOOL IsAbsDirectionDown(STATION_NO station, ENTRY_ID entry) ;
	BOOL IsAbsDirectionUp(STATION_NO station, ENTRY_ID entry) ;
	BOOL  IsAbsDirectionUp(const TG_TrainRecord &rec, TRAIN_IN_OUT in_out);
	BOOL  IsAbsDirectionDown(const TG_TrainRecord &rec, TRAIN_IN_OUT in_out);
	BOOL IsTrainDirectionDown(STATION_NO station, ENTRY_ID entry, TRAIN_IN_OUT in_out);
	BOOL IsTrainDirectionUp(STATION_NO station, ENTRY_ID entry, TRAIN_IN_OUT in_out);
	BOOL IsTrainDirectionUp(const TG_TrainRecord &rec, TRAIN_IN_OUT in_out);
	BOOL IsTrainDirectionDown(const TG_TrainRecord &rec, TRAIN_IN_OUT in_out);

	BOOL VerifySectionStationIndex(int section, int station, int index);

	BOOL VerifyStationNumber(STATION_NO sta);
	BOOL VerifyStationEntry(STATION_NO sta, ENTRY_ID entry);
	BOOL VerifyStationSide(STATION_NO sta, SIDE_ID side);
	BOOL VerifyStationAbs(STATION_NO sta, int abs_number);
	BOOL VerifyEntryAndSide(STATION_NO sta, ENTRY_ID entry, SIDE_ID side);
	BOOL VerifyEntryAndAbs(STATION_NO prev_sta, ENTRY_ID prev_entry, STATION_NO next_sta, ENTRY_ID next_entry);
	BOOL  GetStopOrderSide(const TG_TrainInfo &coninfo, TG_TrainRecord &rec, SIDE_ID* side, short *side_count);

	BOOL  IsStopSide(STATION_NO station, SIDE_ID side, ENTRY_ID entry,ENTRY_ID exit);
	BOOL  IsStopSide(STATION_NO station, SIDE_ID side, ENTRY_ID entry);
	BOOL  IsStopSide(const TG_TrainRecord &rec);
	BOOL  IsMainSide(const TG_TrainRecord &rec);
	BOOL  IsMainSide(STATION_NO station, SIDE_ID side, ENTRY_ID entry,ENTRY_ID exit);
	SIDE_ID GetMainSide(STATION_NO station, ENTRY_ID entry, ENTRY_ID exit);
	SIDE_ID GetMainSide(const TG_TrainRecord &rec);

	const CGetPasswayInfor *GetPasswayInfor() {return &m_cPasswayInfo;}
	const CStationDef*      GetStationDef(STATION_NO sta); 
	int  GetStationSideCount(STATION_NO sta);
	BOOL GetStationList(int& stationNum, int* stationList);
	BOOL GetPasswayInfo(int con_id,TG_PassWayInfo info[],int *limit);  // return 实际数目
	BOOL GetPasswaySta(int con_id,int iRouteNo,TG_PassWaySta sta[],int *limit);  // return 实际数目
	BOOL GetPasswayFlag(int con_id,int iRouteNo,int *inflag,int *outflag);  //  判断径路的接入交出属性，如果是0 则都可以
	////////// PRIO
	const char *GetTrainTypeName(int index,int *value);
	const char *GetAbsTypeName(int index,int *value);
	const char *GetPrioName(int index,int *value);

	const char *GetAbsTimeTypeNameByIndex(int index,int *value);
	const char *GetAbsTimeTypeNameById(int type_id);
	const char *GetTrainTypeNameById(int type_id);
	int   GetAllAbsTimeTypeName(CStringArray& name_list, CWordArray& run_type_list);
	bool  IsEnableABSTimeType();
		
	const char *GetSideNameByID(STATION_NO station,int sideid);
	int   GetSideIDByName(STATION_NO station,const SIDE_NAME name,int *value);
	const char *GetSideNameByIndex(STATION_NO station,int index,int *value);
	const char *GetSideNameBySideNo(STATION_NO station,int sideno,int *value);
	int   GetSideIDByName(STATION_NO station,const char *name);
	const char *GetAbsNameBetweenStations(STATION_NO st1,STATION_NO st2,int index,int *absnumber=NULL);
	const char *GetAbsNameBetweenStations(STATION_NO st1,STATION_NO st2,int entry1,int entry2,int *absnumber=NULL,int *default_direction=NULL);
	BOOL  ConvertAbsnumToStationEntry(ushort *station1, ushort *entry_num1,ushort *station2, ushort *entry_num2,ushort abs_num);
	const char *GetAbsAttrib(int absnum,int *direction);
	long   GetAbsNumberBetweenStations(int st1,int st2,int direction);
	long   GetAbsNumberBetweenStationsByCfgOrder(int st1,int st2,int list_absline[],int list_direct[]);
	long   GetAbsNumByTmisNum(int tmisno);
	const char *GetAbsName(int absnum);
	const char *GetAbsNameByStationEntry(STATION_NO st, int entry);

	BOOL  ConvertStationDirectionToAbsnum(ushort station, ushort dir, ushort *entry_num,ushort *abs_num);
	BOOL  ConvertStationEntryToAbsnum(ushort station, ushort entry_num,ushort *abs_num,ushort *dir);

	int    GetLocoAllPref(int con_id, CStringArray& arypref);
	CString GetLocoName(int con_id, int index,int& value);
	CString GetLocoPreName(int con_id, CString type);
	int		GetCascoIdByLocoId(int con_id, int type);
	CString GetLocoIdByPref(int con_id, CString pref);
	const AbsAttrib *GetAbsAttrib(long absnum);

	int   GetTrainGapTime(STATION_NO station, GAP_TYPE gap_type, ARRIVE_DEPART_STATUS act1, ARRIVE_DEPART_STATUS act2, RUN_TYPE run_type1, RUN_TYPE run_type2) const
	{
		return m_cGapTimeInfo.GetTrainGapTime(station,gap_type,act1,act2,run_type1,run_type2);
	}
	
	int    GetAcceptEntry(ushort con_id,ushort station, ushort onestation,ushort oneentry, int in_index, CString trainid);
	int    GetGiveoutExit(ushort con_id,ushort station, ushort onestation,ushort oneentry, int out_index, CString trainid);

	BOOL  GetTrainTypeAndPrio(const char* train_no,BYTE *type, BYTE *run_type, BYTE *prio);

private:
	int    GetEntryExit(ushort con_id,ushort station, ushort onestation,ushort oneentry);

public:
	bool IsTrainPassNotUseMainSide(TG_TrainRecord& prec);
	bool IsTrainSideNotConnectWithEntry(int station, int sideno, int entry);

	int GetEntryByCTCAbsLineNumber(int station, int ctcabsline);
	CString GetAbsDirectInfo(const TG_TrainRecord &rec, TRAIN_IN_OUT in_out);
	CString GetTrainDirectInfo(const TG_TrainRecord &rec, TRAIN_IN_OUT in_out);

	CString GetPasswayName(int con_id, int passway_id);

private:
	BOOL  LoadLimitStationSideConfig(const char *cfgfile);
	bool  m_EnableLimitStationSide;
	std::map<int, StaSideLimit> m_StaSideLimitList;

public:
	BOOL  IsTwoStationSideLimit(int station, int side1, int side2);
	StaSideLimit *GetStaLimitCfg(int stno);
	BOOL  EnableStaSideLimitCfg() { return m_StaSideLimitList.size()>0?TRUE:FALSE;}
	BOOL  IsStationSideLimitOtherTrain(int stno);
};

CTGBasicParam *GetTGBasicParam();
#endif


