#pragma once
enum CTC_DATA_DEF{
	CTC_DATA_STR=0,
	CTC_DATA_INT=1,
	CTC_DATA_DATETIME=2,
	CTC_DATA_DIRECTION=20,
	CTC_DATA_BOOL=21,
	CTC_DATA_RANK=22,
	CTC_DATA_MARK_TYPE=23,
	CTC_DATA_STATION=27,
	CTC_DATA_TRAIN_TYPE=24,
	CTC_DATA_TRAIN_RUN_TYPE=25,
	CTC_DATA_TRAIN_PRIORITY=26,
	CTC_DATA_ENTRY=27,
	CTC_DATA_SIDE=28,
	CTC_DATA_MMPLAN_STATUS=29,
	CTC_DATA_TRAIN_ABS_TIME_TYPE=30,
	CTC_DATA_NOT_MEET=31,
	CTC_DATA_SPACE=32,

	CTC_DATA_DEPART_COND=45,
};
struct PropertyInfoPara{
    int flag;   // 0:显示操作类型 1:STATIC 2: EDIT 3: LIST,4:BOOL
	int id;     //  数据指示标示
	CTC_DATA_DEF data_type;
	char attr_name[20];  // 数据字段名
	char caption[24];    // 显示名
	long  para;
};

const char *GetCTCValueDispString(CTC_DATA_DEF tp, int value,long para=0);
const char *GetCTCTypeChoiceString(CTC_DATA_DEF tp, int index, int *value,long para=0);