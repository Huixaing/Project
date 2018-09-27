#include "stdafx.h"
#include "ctc_data_type_def.h"
#include <string.h>
#include "tg_structdef.h"
#include "sysshare.h"

struct IDNameStruct
{
	int value;
	char *name;
};


const char *GetTypeStringByIndex(int index,int *value,IDNameStruct *info,int infocount)
{
	if(index>=0 && index<infocount)
	{
		*value=info[index].value; 
		return info[index].name; 
	}
	else
		return NULL;
}	

const char *GetTypeStringByValue(int value,IDNameStruct *info,int infocount)
{
	for(int i=0;i<infocount;i++)
	{
		if(value==info[i].value)
			return info[i].name; 
	}
	return NULL;
}	

#define BEGIN_DEF_TYPE_INFO(def) \
	IDNameStruct def[]={ 
#define SET_TYPE_INFO(value,name) {value,name}, 
#define END_DEF_TYPE  }; 

#define TYPE_INFO_COUNT(def) sizeof(def)/sizeof(def[0])
#define GET_TYPE_STRING(def,index,value)  GetTypeStringByIndex(index,value,def,sizeof(def)/sizeof(def[0]))
#define GET_VALUE_STRING(def,value)  GetTypeStringByValue(value,def,sizeof(def)/sizeof(def[0]))

BEGIN_DEF_TYPE_INFO(DirectionInfos)
SET_TYPE_INFO(0,"上行")
SET_TYPE_INFO(1,"下行")
SET_TYPE_INFO(2,"双向")
END_DEF_TYPE

BEGIN_DEF_TYPE_INFO(DepartCondInfos)
SET_TYPE_INFO(0,"按照待避")
SET_TYPE_INFO(1,"按照计划")
END_DEF_TYPE

BEGIN_DEF_TYPE_INFO(BoolInfos)
SET_TYPE_INFO(0,"否")
SET_TYPE_INFO(1,"是")
END_DEF_TYPE

BEGIN_DEF_TYPE_INFO(RankInfos)
SET_TYPE_INFO(0,"正常")
SET_TYPE_INFO(1,"超级")
SET_TYPE_INFO(2,"一级")
SET_TYPE_INFO(3,"二级")
SET_TYPE_INFO(4,"三级")
END_DEF_TYPE

BEGIN_DEF_TYPE_INFO(SpaceInfos)
SET_TYPE_INFO(0,"正常")
SET_TYPE_INFO(1,"一级")
SET_TYPE_INFO(2,"二级")
SET_TYPE_INFO(3,"三级")
END_DEF_TYPE

BEGIN_DEF_TYPE_INFO(BlockTypeInfos)
SET_TYPE_INFO(TG_ABS_INHIBIT,"区间封锁")
SET_TYPE_INFO(TG_ABS_SLOW,"区间慢行")
SET_TYPE_INFO(TG_ABS_FIX_MARK,"区间综合天窗修")
SET_TYPE_INFO(TG_SECTION_ELEC_FIX,"电网检修")
SET_TYPE_INFO(TG_SECTION_ELEC_FIX_LANZHOU,"兰州电网检修")
SET_TYPE_INFO(TG_CHANGE_BLOCK_TYPE,"区间变更闭塞方式")
SET_TYPE_INFO(TG_SIDE_INHIBIT,"站内封锁")
SET_TYPE_INFO(TG_SIDE_SLOW,"站内慢行")
SET_TYPE_INFO(TG_SIDE_TIANCHUANG,"站内天窗修")
SET_TYPE_INFO(TG_SIDE_OCCUPY,"股道占用")
SET_TYPE_INFO(TG_GRAPH_NOTE,"记事注释")

END_DEF_TYPE

BEGIN_DEF_TYPE_INFO(MMPlanStatusInfo)
SET_TYPE_INFO(0,"未开始")
SET_TYPE_INFO(1,"施工开始")
SET_TYPE_INFO(2,"施工结束")
END_DEF_TYPE

// 注意此处定义应和TG.OCX 一致
BEGIN_DEF_TYPE_INFO(NotMeetTypeInfo) 
SET_TYPE_INFO(0,"无")
SET_TYPE_INFO(NOTMEET_CRH,"禁会动车")
SET_TYPE_INFO(NOTMEET_SEPECIAL_Z,"禁会直特")
SET_TYPE_INFO(NOTMEET_PASSENGER,"禁会客车")
SET_TYPE_INFO(NOTMEET_FREIGHT,"禁会列车")
SET_TYPE_INFO(NOTMEET_SPACE,"禁会超限")

SET_TYPE_INFO(NOTMEET_200_orUp,"禁会200及以上")
SET_TYPE_INFO(NOTMEET_120_200,"禁会120-200")
SET_TYPE_INFO(NOTMEET_120_Low,"禁会120以下")
SET_TYPE_INFO(NOTMEET_ALL,"禁会所有")
END_DEF_TYPE

const char *GetCTCTypeChoiceString(CTC_DATA_DEF tp, int index, int *value,long para)
{
	switch(tp)
	{
	case CTC_DATA_DIRECTION:
		return GET_TYPE_STRING(DirectionInfos,index,value);
	case CTC_DATA_DEPART_COND:
		return GET_TYPE_STRING(DepartCondInfos,index,value);
	case CTC_DATA_BOOL:
		return GET_TYPE_STRING(BoolInfos,index,value);
	case CTC_DATA_SPACE:
		return GET_TYPE_STRING(SpaceInfos,index,value);
	case CTC_DATA_RANK:
		return GET_TYPE_STRING(RankInfos,index,value);
	case CTC_DATA_MARK_TYPE:
		return GET_TYPE_STRING(BlockTypeInfos,index,value);
	case CTC_DATA_MMPLAN_STATUS:
		return GET_TYPE_STRING(MMPlanStatusInfo,index,value);
	case CTC_DATA_STATION:
		{
			int nStationList[128];
			int stationCount=128;
			stationCount=gpcdef.GetStationList(para,stationCount,nStationList);
			if(index>=0 && index<stationCount)
				return gpcdef.GetStationName(nStationList[index]);
			else
				return NULL;
		}
		break;

	case CTC_DATA_NOT_MEET:
		return GET_TYPE_STRING(NotMeetTypeInfo,index,value);
		break;
	}
	return NULL;
}

const char *GetCTCValueDispString(CTC_DATA_DEF tp, int value,long para)
{
	static char tmp[128];
	memset(tmp, 0, sizeof(tmp)/sizeof(tmp[0]));

	switch(tp)
	{
	case CTC_DATA_DIRECTION:
		return GET_VALUE_STRING(DirectionInfos,value);
	case CTC_DATA_DEPART_COND:
		return GET_VALUE_STRING(DepartCondInfos,value);
	case CTC_DATA_BOOL:
		return GET_VALUE_STRING(BoolInfos,value);
	case CTC_DATA_SPACE:
		return GET_VALUE_STRING(SpaceInfos,value);
	case CTC_DATA_RANK:
		return GET_VALUE_STRING(RankInfos,value);
	case CTC_DATA_MARK_TYPE:
		return GET_VALUE_STRING(BlockTypeInfos,value);
	case CTC_DATA_MMPLAN_STATUS:
		return GET_VALUE_STRING(MMPlanStatusInfo,value);
	case CTC_DATA_DATETIME:
		{
			CTime ctm=value;
			strcpy(tmp,ctm.Format("%m月%d日%H:%M"));
			return tmp;
		}

	default:
		sprintf(tmp,"%d",value);
		return tmp;
	}
	return NULL;
}
