#if !defined(_AFX_STATION_DEF_H_INCLUDED_)
#define _AFX_STATION_DEF_H_INCLUDED_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <vector>


#define SHUNT_SURPORT

#ifndef SHUNT_SURPORT
	#define AFX_EXT_CLASS
	#include "ctc_defines.h"
#else
	#include "stdAfx.h"
	#include <direct.h>
	#include "../BaseMsg/ctc_defines.h"
#endif

USING_NAMESPACE_CLBTH;
#define	INIT_STA_NO	0

#define  MAX_BACKUP_SIDE_NUM           32
#define  MAX_DISCONN_SIDE_NUM          32
#define  MAX_SIDE_ENTRY_GROUP_COUNT	   32

#ifndef STA_SUCCESS
#define STA_SUCCESS		1
#endif

#ifndef STA_FAILED
#define STA_FAILED		0
#endif

#ifndef GET_KEY_PROCESS
#define	GET_KEY_PROCESS(key, seps, token)								\
	((NULL == (token = strtok(NULL, seps))) ? STA_FAILED : GetKey(key, token))

#define	GET_FIRST_KEY_PROCESS(ptr, key, seps, token)					\
	((NULL == (token = strtok(ptr , seps))) ? STA_FAILED : GetKey(key, token))

#endif

#ifndef JUDGE_SIDE_PROPERTY
#define JUDGE_SIDE_PROPERTY(side_no, key, varValue)						\
	SIDE_T side;														\
	if(STA_FAILED == GetSideInfo(side_no, side)) return STA_FAILED;		\
	return (key == varValue) ? STA_SUCCESS : STA_FAILED;
#endif

#ifndef GET_SIDE_PROPERTY_CHAR
#define GET_SIDE_PROPERTY_CHAR(side_no, key, varValue)					\
	SIDE_T side;														\
	memset(varValue, 0, sizeof(varValue));								\
	if(STA_FAILED == GetSideInfo(side_no, side)) return STA_FAILED;		\
	strcpy(varValue,key);												\
	return STA_SUCCESS;
#endif

#ifndef GET_SIDE_PROPERTY_NUM
#define GET_SIDE_PROPERTY_NUM(side_no, key, varValue)					\
	varValue = 0;														\
	SIDE_T side;														\
	if(STA_FAILED == GetSideInfo(side_no, side)) return STA_FAILED;		\
	varValue = key;														\
	return STA_SUCCESS;
#endif

class  CStationDef
{
public:
	CStationDef(STATION_NO station_no, const char *strFileName);
#ifdef SHUNT_SURPORT
	CStationDef(STATION_NO station_no);
	short AssertTleSide(SIDE_NO side_no);
#endif
	enum READ_FILE_RESULT
	{
		READ_FILE_SUCC,

		OPEN_FILE_ERR,
		TOO_LONG_LINE,
		STATION_NOT_FOUND,
		READ_UNKNOWN_SECTION,

		READ_STA_NO,
		READ_STA_NAME,
		READ_TLE_NAME,
		READ_STA_PROP,
		READ_SIDE_CNT,
		READ_STA_GRADE,
		READ_STA_TECK,
		READ_STA_OPER,
		READ_STA_BACK_GROUP,
		READ_STA_ENTRY_CNT,

		READ_SIDE_CNT_SMALL,
		READ_SIDE_OVERFLOW,
		READ_SIDE_NO,
		READ_SIDE_NO_DUPLICATE,
		READ_SIDE_DISPLAY_NAME,
		READ_SIDE_TLE_NAME,
		READ_SIDE_IN_TLE,
		READ_SIDE_PROP,
		READ_SIDE_REGION,
		READ_SIDE_DIR,
		READ_SIDE_PRIO_DIR,
		READ_SIDE_TRAIN_TYPE,
		READ_SIDE_EXCEED,
		READ_SIDE_LEN,
		READ_SIDE_STORE_CAR,
		READ_SIDE_CATENARY,
		READ_SIDE_PLATFORM,
		READ_SIDE_PASSENGER,
		READ_SIDE_UPSLOPE,
		READ_SIDE_CURVE,
		READ_SIDE_START,
		READ_SIDE_SEPARATE,
		READ_SIDE_CIRCUIT,
		READ_SIDE_SPEEDLIMIT,
		READ_SIDE_ENTRY1,
		READ_SIDE_ENTRY2,
		READ_SIDE_NEIGBOR1,
		READ_SIDE_WIDTH1,
		READ_SIDE_NEIGBOR2,
		READ_SIDE_WIDTH2,
		READ_SIDE_SHUNT_SLIDE,
		READ_SIDE_SHUNT_REQ,
		READ_SIDE_SHUNT_EXCETIME,
		READ_SIDE_SHUNT_LIGHT,
		READ_SIDE_SHUNT_WATER,
		READ_SIDE_SHUNT_FREIGHT,
		READ_SIDE_SHUNT_PREVENT,
		READ_SIDE_SHUNT_REMARK,
		READ_SIDE_ENTRY_EXIST,

		READ_ENTRY_CNT_SMALL,
		READ_ENTRY_OVERFLOW,
		READ_ENTRY_NO,
		READ_ENTRY_NO_DUPLICATE,
		READ_ENTRY_NAME,
		READ_ENTRY_CATENARY,
		READ_ENTRY_DIR,
		READ_ENTRY_DISCONN_SIDE_OVERFLOW,

		READ_BACKUP_CNT_SMALL,
		READ_BACKUP_OVERFLOW,
		READ_BACKUP_TRAINTYPE,
		READ_BACKUP_MAIN_SIDE_NO,
		READ_BACKUP_SIDE_OVERFLOW,

		READ_OMSIDE_OVERFLOW,
		READ_OMSIDE_ID,
		READ_OMSIDE_ENTRY1,
		READ_OMSIDE_ENTRY2,
		READ_OMSIDE_NOT_EXIST,

	};
	enum RECEIVE_TRAIN_DIR
	{
		UP_DIR,
		DOWN_DIR,
		DOUBLE_DIR
	};

	enum RECEIVE_TRAIN_TYPE
	{
		RECV_NONE			= 0,
		RECV_PASSENGER	= 1,
		RECV_CARGO		= 2
	};

	~CStationDef();
	READ_FILE_RESULT GetReadFileRs(short *line_no=NULL);
	const char* GetReadFileReason(READ_FILE_RESULT rs);

	STATION_NO GetStationNo();

	void ResetStation();
	short GetStationName(STAT_NAME& station_name);
	short GetSideCount();
	short SetCurSide(SIDE_NO side_no);
	short SetCurSide(SIDE_NAME side_name);

	short GetReceDeptSideNameAndNo(SIDE_NAME* side_name, SIDE_NO* side_no, short& side_cnt) const; // 20161105 同时获取股道号和股道名称
	short GetReceDeptSideName(SIDE_NAME* side_name, short& side_cnt) const;
	short GetReceDeptSideNo(SIDE_NO* side_no, short& side_cnt) const;
	short GetReceDeptSideCnt() const;
	short GetReceDeptSideNo(short &index, SIDE_NO& side) const;

	short GetSideName(SIDE_NAME* side_name, short& side_cnt);
	short GetNextSideName(SIDE_NAME& side_name);
	short GetPreSideName(SIDE_NAME& side_name);

	short GetSideNo(SIDE_NAME side_name, SIDE_NO& side_no) const;
	short GetSideName(SIDE_NO side_no, SIDE_NAME& side_name) const;
	short GetSideProperty(SIDE_NO side_no, short& side_property) const;

	// side property
	short isTleSide(SIDE_NO side_no) const;
	short isMainSide(SIDE_NO side_no) const;
	short isMainSide(SIDE_NO side_no, short entry) const;
	short isMainSide(SIDE_NO side_no, short entry, short exit) const;
	short isStopSide(SIDE_NO side_no) const;
	short isStopSide(SIDE_NO side_no, short entry) const;
	short isStopSide(SIDE_NO side_no, short entry, short exit) const;
	short isReceDeptSide(SIDE_NO side_no) const;                        // is Reception-departure track
	short isLocomotiveMovseSide(SIDE_NO side_no) const;
	short isFreightSide(SIDE_NO side_no) const;
	short isTypeOfSide(SIDE_NO side_no, short type) const;
	short isRegion(SIDE_NO side_no, SIDE_REGION_FLAG region) const;
	short isCRHSide(SIDE_NO side_no) const;

	// Reception-departure train
	short isUpReceTrain(SIDE_NO side_no) const;
	short isDownReceTrain(SIDE_NO side_no) const;
	short isUpPrioReceTrain(SIDE_NO side_no) const;
	short isDownPrioReceTrain(SIDE_NO side_no) const;
	short isReceiveTrain(SIDE_NO side_no, BYTE TrainType) const;
	short isReceiveTrain(SIDE_NO side_no, BYTE TrainType, RECEIVE_TRAIN_DIR dir) const;
	short isUpDepartTrain(SIDE_NO side_no) const;
	short isDownDepartTrain(SIDE_NO side_no) const;
	short isUpPrioDepartTrain(SIDE_NO side_no) const;
	short isDownPrioDepartTrain(SIDE_NO side_no) const;
	short isDepartTrain(SIDE_NO side_no, BYTE TrainType) const;
	short isDepartTrain(SIDE_NO side_no, BYTE TrainType, RECEIVE_TRAIN_DIR dir) const;

	// side len etc.
	short GetSideLen(SIDE_NO side_no, short& side_len) const;
	short GetSideMaxStoreCar(SIDE_NO side_no, short& max_store_car, short caclute_type = 10) const;

	short isReceiveLengthTrain(SIDE_NO side_no, short len) const;
	short isReceiveExceedTrain(SIDE_NO side_no, short exceed_grade) const;
	short isCatenary(SIDE_NO side_no) const;							// 接触网
	short GetSideCatenaryNumber(SIDE_NO side_no, short& nNumber) const;
	short isPassenerEquipment(SIDE_NO side_no) const;
	short isFreightEquipment(SIDE_NO side_no) const;
	short GetSideUpSlope(SIDE_NO side_no, float& slope) const;
	short GetSideCurveRadius(SIDE_NO side_no, short& curve_radius) const;

	// side equipement
	short isUpStartDifficuent(SIDE_NO side_no, short weight) const;
	short isDownStartDifficuent(SIDE_NO side_no, short weight) const;
	short isUpSeparate(SIDE_NO side_no) const;
	short isDownSeparate(SIDE_NO side_no) const;
	short isCircuit(SIDE_NO side_no) const;
	short isSpeedLimit(SIDE_NO side_no, short& speed) const;
	short GetNeighborSide1(SIDE_NO side_no, SIDE_NO& neighbor_side_no) const;
	short GetNeighborSideWidth1(SIDE_NO side_no, float width) const;
	short GetNeighborSide2(SIDE_NO side_no, SIDE_NO& neighbor_side_no) const;
	short GetNeighborSideWidth2(SIDE_NO side_no, float width) const;
	short isPlatform(SIDE_NO side_no) const;							// 站台
	short isHighPlatform(SIDE_NO side_no) const;

#ifdef SHUNT_SURPORT
	short GetSlideLimit(SIDE_NO side_no, short nSlideLimit) const;
	short GetFreightRequest(SIDE_NO side_no, short& nFreightRequest) const;
	short GetShuntExcessiceTime(SIDE_NO side_no, SHUNT_TIME& nShuntExcessiveTime) const;
	short isLight(SIDE_NO side_no) const;
	short isWater(SIDE_NO side_no) const;
	short GetFreightEquipment(SIDE_NO side_no, short& nEqument) const;
	short isPreventSlip(SIDE_NO side_no) const;
	short GetSideRemark(SIDE_NO side_no, SIDE_REMARK& remark) const;
#endif

	// entry function
	short GetEntryLineName(ENTRY_ID entry_id, char* strName) const;
	short isCatenaryEntry(ENTRY_ID entry_id) const;
	short GetEntryCatenaryNumber(ENTRY_ID entry_id, short nzNumber[], int& nCount) const;
	short isUpEntry(ENTRY_ID entry_id) const;
	short isDownEntry(ENTRY_ID entry_id) const;
	short isUpDownEntry(ENTRY_ID entry_id) const;
	short GetEntryDir(ENTRY_ID entry_id) const;
	short isEntrySideConnect(SIDE_NO side_no, ENTRY_ID entry_id) const;
	
	//  side group function
	short GetBackupSideInGroup(short train_type,const SIDE_NO  side_no, SIDE_NO backup[MAX_BACKUP_SIDE_NUM]) const ;  // return count of found backup_sides which stored in backup
	short GetBackupSideInGroupByMainSide(short train_type,const SIDE_NO  side1, SIDE_NO backup[MAX_BACKUP_SIDE_NUM]) const;

protected:
	const STATION_NO	m_nNo;
	STAT_NAME			m_sName;
	STAT_NAME			m_sTleName;
	STATION_PROP		m_nProperty;
	STATION_GRADE		m_nGrade;
	STATION_TECHNICAL	m_nTechnical;
	STATION_OPERATION	m_nOperation;
	short				m_nReceDeptSideCnt;
public:
	typedef struct
	{
		SIDE_NO				no;						// 股道号
		SIDE_NAME			display_name;			// 股道名
		SIDE_NAME			tle_side_name;			// tle股道名称
		short				is_tle_side;			// 是否是tle中股道
		short				side_property;			// 股道性质
		SIDE_REGION_FLAG	side_region_flag;		// 股道分区标志
		short				rece_depa_dir;			// 接发车方向
		short				prio_rece_depa_dir;		// 优先接发车方向
		RECE_TRAIN_TYPE		rece_train_type;		// 车辆类型
		short				exceed_delimitation;	// 超限级别
		short				len;					// 有效长
		short				max_store_car_cnt;		// 容车数
		short				catenary;				// 接触网
		short				platform;				// 站台
		short				passenger_equipment;	// 客运设备
		short				freight_equipment;		// 货运设备
		float				up_slope;				// 上行坡度
		short				curve_radius;			// 曲线半径 unit: m
		short				starting_difficuent;	// 启动困难
		short				separate;				// 隔开设备
		short				circuit;				// 轨道电路
		short				speed_limit;			// 限速
		struct
		{
			short				entry_1;				// 相邻出入口1
			short				entry_2;				// 相邻出入口2
		}  rela_entry_group[MAX_SIDE_ENTRY_GROUP_COUNT];
		short				rela_entry_group_count;
		SIDE_NO				neighbor_side_1;		// 相邻股道1
		float				width_1;				// 线间距1
		SIDE_NO				neighbor_side_2;		// 相邻股道2
		float				width_2;				// 线间距2
		short			no_crh_side;			// 是否禁接高速动车
#ifdef SHUNT_SURPORT
		short				shunt_slide_limit;		// 溜放限制
		short				freight_request;		// 货运要求
		SHUNT_TIME			shunt_excessive_time;	// 调车作业时间: (秒)
		short				light;					// 照明
		short				water;					// 给水
		short				prevent_slip;			// 防溜逸
		SIDE_REMARK			remark;					// 附注
#endif
	}SIDE_T;

	struct ENTRY_T
	{
		ENTRY_ID			entry_id;
		string				entry_name;			// 线路名称
		vector<short>		catenary;				// 接触网
		short               dir;                    // 方向
		std::vector<SIDE_NO>	disconn_sides;//[MAX_DISCONN_SIDE_NUM];  // 不能到达的到发股道号
		ENTRY_T() {
			entry_id = -1;
			dir = 0;
		}
		ENTRY_T(const ENTRY_T& rhs){ operator=(rhs); }
		ENTRY_T& operator=(const ENTRY_T& rhs){
			entry_id = rhs.entry_id;
			entry_name = rhs.entry_name;
			catenary = rhs.catenary;
			dir = rhs.dir;
			disconn_sides = rhs.disconn_sides;
			return *this;
		}
	} ;

	typedef  struct{
		short              train_type;
		SIDE_NO            main_side_no;           // 
		SIDE_NO            backup_sides[MAX_BACKUP_SIDE_NUM];
		short              count;
	}MAIN_SIDE_BACKUP_GROUP;

	short GetSideInfo(SIDE_NO side_no, SIDE_T& side) const;
	short GetSideInfo(SIDE_NAME side_name, SIDE_T& side) const;
	short GetSideInfoByTleName(SIDE_NAME side_name, SIDE_T& side) const;

	short GetEntryInfo(ENTRY_ID entry_id, ENTRY_T& entry) const;
	short   IsStationEntry(ENTRY_ID entry_id) const;
private:
#ifndef SHUNT_SURPORT
	CStationDef();
#endif

	READ_FILE_RESULT m_LoadFlag;
	short            m_LineNo;

	SIDE_T	*m_side;
	short  m_nCurSide;
	short  m_nSideCnt;

	std::vector<ENTRY_T>	m_entry;
	short  m_nCurEntry;
	short  m_nEntryCnt;

	MAIN_SIDE_BACKUP_GROUP *m_back_group;
	short m_nCurBackupGroup;
	short m_nBackupCnt;

	void init();
	void _ResetReadCount();
	void _ResetReadCursor();
	void _CreateStruct();

	short ASSERT_SIDE(SIDE_NO nSideNo);
	short GetKey(short& nKey, char * ptr);
	short GetKey(float& fKey, char * ptr);
	short GetKey(ulong& ulKey, char * ptr);
	short GetKey(char* sKey, char * ptr);

	short FLoadStation(const char *szFileName);
	short _FLoadStation(char *ptr, STATION_NO& sta);
	short _FLoadStationSide(char *ptr);
	short _FLoadStationEntry(char *ptr);
	short _FLoadBackupGroup(char *ptr);
	short _FLoadEntryGroup(char* ptr);
	short _FLoadNotCRHSide(char* ptr);

	short	isRelaEntry( const SIDE_T& rSide, short entry ) const;
	short	isRelaEntryGroup( const SIDE_T& rSide, short entry1, short entry2 ) const;
};

#endif // _AFX_STATION_DEF_H_INCLUDED_
