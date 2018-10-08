// MyData.cpp : implementation file
//
/////////////////////////////////////////////////////////////////////////////
//
// Copyright ?1999, Stefan Belopotocan, http://welcome.to/StefanBelopotocan
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TGPspData.h"
#include "tg_basicpara.h"
#include "tg_schedule_base.h"
#include "sysshare.h"
#include "utility/ctc_data_type_def.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// 施工计划传输原始信息
PropertyInfoPara g_mmplan_info[]=
{
	{0,0,CTC_DATA_STR,"PLANID","ID"},
	{2,ID_PD_ST_MARK_TYPE,CTC_DATA_MARK_TYPE,"TYPE","符号类型"},
	//{5,ID_PD_ST_MARK_DATE,CTC_DATA_DATETIME,"TM","日期"},
	{4,ID_PD_ST_MARK_START_TIME,CTC_DATA_DATETIME,"STTM","起始时间"},
	{4,ID_PD_ST_MARK_END_TIME,CTC_DATA_DATETIME,"EDTM","结束时间"},
	{2,ID_PD_ST_MARK_STATION1,CTC_DATA_STR,"STST","起始站"},
	{2,ID_PD_ST_MARK_STATION2,CTC_DATA_STR,"EDST","结束站"},
	{2,ID_PD_ST_MARK_TEXT,CTC_DATA_STR,"text","标注"},
	{2,ID_PD_ST_MARK_SMETER,CTC_DATA_INT,"STKM","起始位置(Km)"},
	{2,ID_PD_ST_MARK_SMETER,CTC_DATA_INT,"STM","起始位置(m)"},
	{2,ID_PD_ST_MARK_SMETER,CTC_DATA_INT,"ETKM","结束位置(Km)"},
	{2,ID_PD_ST_MARK_SMETER,CTC_DATA_INT,"ETM","结束位置(m)"},
	{2,ID_PD_ST_MARK_SLOWTIME,CTC_DATA_INT,"OTHER","限速"},
///////////////////////////////////////////////////////////////////////////
	
	{0,0,CTC_DATA_STR,"ID","计划ID"},
	{0,0,CTC_DATA_STR,"FLAG","类型"},  // 2 施工  3 维修
	{0,0,CTC_DATA_STR,"JHH","计划ID"}, 
	{0,0,CTC_DATA_STR,"DBH","电报号"}, 
	{0,0,CTC_DATA_STR,"MID","月计划ID"},
	{0,0,CTC_DATA_STR,"WORKPOS","施工地点"},
	{0,0,CTC_DATA_STR,"LINENM","施工线路"},
	{0,0,CTC_DATA_DIRECTION,"DIR","上下行"}, // 0 上,1下 ,2上下, 3单线
	{0,0,CTC_DATA_STR,"WORKKM","施工起始里程"},
	
	//{2,0,CTC_DATA_DATETIME,"STTM","起始时间"},
	//{2,0,CTC_DATA_DATETIME,"EDTM","结束时间"},

	{2,0,CTC_DATA_STR,"WORKTP","施工类型"},
	{2,0,CTC_DATA_STR,"PJNM","施工项目"},

	{0,0,CTC_DATA_STR,"UNITNM","施工单位"},
	{0,0,CTC_DATA_STR,"LEADER","施工负责人"},

	{0,0,CTC_DATA_STR,"RANK","施工等级"},
	{0,0,CTC_DATA_STR,"WKCON","施工条件"},
	{0,0,CTC_DATA_STR,"SPLIM","限速条件"},
	{0,0,CTC_DATA_STR,"LIMINFO","行车限制卡(供电臂)"}
};
int    count_of_mark_def()
{
    return 12;  // 注意,此数值和上述定义有直接关系
}
int    count_of_g_mmplan_info()
{
    return  sizeof(g_mmplan_info)/sizeof(g_mmplan_info[0]);
}

PropertyInfoPara g_mmplan_note_view_info[]=
{
	{0,0,CTC_DATA_STR,"TITLE","施工项目"}, // 包括ID,电报号,月计划ID,类型,等级,地点,里程等
	{1,0,CTC_DATA_STR,"UNITNM","施工单位"}, // 包括负责人
	{1,0,CTC_DATA_STR,"WKCON","施工条件"},
	{0,0,CTC_DATA_STR,"SPLIM","限速条件"},
	{0,0,CTC_DATA_STR,"LIMINFO","行车限制卡(供电臂)"}
};

int    count_of_g_mmplan_note_view_info()
{
    return  sizeof(g_mmplan_note_view_info)/sizeof(g_mmplan_note_view_info[0]);
}

PropertyInfoPara g_mmplan_note_list_info[]=
{
	{0,0,CTC_DATA_STR,"PLANID","施工标识"},
	{1,ID_PD_MPLAN_LIST_SUBID,CTC_DATA_STR,"SUBID","计划号"},
	{1,0,CTC_DATA_STR,"TM","施工时间"},
	{1,0,CTC_DATA_STR,"TYPE","类型"},
	{1,0,CTC_DATA_STR,"WORKPOS","施工项目"},
//	{1,0,CTC_DATA_STR,"TRNINFO","路用车"},
};

int    count_of_g_mmplan_note_list_info()
{
    return  sizeof(g_mmplan_note_list_info)/sizeof(g_mmplan_note_list_info[0]);
}

struct PropertyInfoPara  g_recordinfo_def[]=
{
	{3,ID_PD_ST_STATION_RECINDEX,CTC_DATA_INT,"index","站序"},
	{3,ID_PD_ST_STATION,CTC_DATA_STATION,"station_name","车站名称"},
	{3,ID_PD_ST_ARRIVE_TRAINID,CTC_DATA_STR,"arrive_id","到达车次"},
	{3,ID_PD_ST_DEPART_TRAINID,CTC_DATA_STR,"depart_id","出发车次"},
	{1,ID_PD_ST_IMPINFO,CTC_DATA_STR,"impinfo","特运信息"},
	{1,ID_PD_ST_STPINFO,CTC_DATA_STR,"stpinfo","禁停信息"},
	{2,ID_PD_ST_MIN_STOP_TIME,CTC_DATA_INT,"min_stop_time","营业时分"},
	{4,ID_PD_ST_SCHD_ARRIVE_TIME,CTC_DATA_DATETIME,"plan_arrive","图定到点"},
	{4,ID_PD_ST_SCHD_DEPART_TIME,CTC_DATA_DATETIME,"plan_depart","图定发点"},
	{4,ID_PD_ST_ARRIVE_TIME,CTC_DATA_DATETIME,"arrive","实际到点"},
	{4,ID_PD_ST_DEPART_TIME,CTC_DATA_DATETIME,"depar","实际发点"},
	{3,ID_PD_ST_ARRIVE_SIDE_ID,CTC_DATA_SIDE,"arrive_side","接车股道"},
	{3,ID_PD_ST_DEPART_SIDE_ID,CTC_DATA_SIDE,"depart_side","发车股道"},
};

int    count_of_recordinfo_def()
{
	return sizeof(g_recordinfo_def)/sizeof(g_recordinfo_def[0]);
}

PropertyInfoPara g_mmplan_dcmd_def[]=
{
	{0,0,CTC_DATA_STR,"PLANID","施工计划ID"},
    {0,0,CTC_DATA_INT,"CMDINDEX","命令序号"}, // 用CMD_DEST 作为CMDINDEX
	{0,0,CTC_DATA_STR,"PRNM","项目名称"},
	{0,0,CTC_DATA_INT,"CMDTP","命令类型"},
	{0,0,CTC_DATA_INT,"SUBTP","命令子类型"},
	{0,0,CTC_DATA_STR,"TOPIC","命令标题"},
	{0,0,CTC_DATA_STR,"CONTENT","命令正文"},
	//{0,0,CTC_DATA_DATETIME,"SDTM","发令时间"},
	//{0,0,CTC_DATA_STR,"SDUNIT","发令机构"},
	//{0,0,CTC_DATA_STR,"SENDER","发令者"},
	//{0,0,CTC_DATA_STR,"RECVUNIT","受令机构"},
};

int    count_of_mmplan_dcmd_def()
{
	return sizeof(g_mmplan_dcmd_def)/sizeof(g_mmplan_dcmd_def[0]);
}

PropertyInfoPara g_dcmd_viewlist_def[]=
{
	{0,ID_PD_MPLAN_DCMD_INDEX,CTC_DATA_STR,"CMDINDEX","命令ID"},
	{0,0,CTC_DATA_STR,"PRNM","施工项目"},
	{0,0,CTC_DATA_STR,"RECVER","受令单位"},
	{0,0,CTC_DATA_STR,"TOPIC","标题"},
	{0,ID_PD_MPLAN_DCMD_CONTENT,CTC_DATA_STR,"CONTENT","命令正文"},
};

int    count_of_viewlist_def()
{
	return sizeof(g_dcmd_viewlist_def)/sizeof(g_dcmd_viewlist_def[0]);
}

PropertyInfoPara g_mmplan_jieshi_def[]=
{
	{0,0,CTC_DATA_STR,"PLANID","施工计划ID"},
	{0,0,CTC_DATA_STR,"CMDID","揭示命令号"},
	{0,0,CTC_DATA_STR,"CONTENT","命令内容"},
};

int    count_of_jieshi_def()
{
	return sizeof(g_mmplan_jieshi_def)/sizeof(g_mmplan_jieshi_def[0]);
}

PropertyInfoPara g_quebao_def[]=
{
	//{0,0,CTC_DATA_STR,"RKTM","入库时间"},
	{0,0,CTC_DATA_STR,"IDX","序号"},
	{0,0,CTC_DATA_STR,"TYPE","车型"},
	{0,0,CTC_DATA_STR,"YZ","罐车油种"},
	{0,0,CTC_DATA_STR,"CNO","车号"},
	{0,0,CTC_DATA_STR,"SWT","自重"},
	{0,0,CTC_DATA_STR,"LEN","换长"},
	{0,0,CTC_DATA_STR,"LDFLAG","载重标志"},

	{0,0,CTC_DATA_STR,"LD","载重"},
	{0,0,CTC_DATA_STR,"FIRSTNM","第一到站"},
	{0,0,CTC_DATA_STR,"LSTSTNM","最后到站"},
	{0,0,CTC_DATA_STR,"OBJNM","货物名称"},

	{0,0,CTC_DATA_STR,"RCVNM","收货人"},
	{0,0,CTC_DATA_STR,"FZNM","发站名"},
	{0,0,CTC_DATA_STR,"PBCNT","蓬布数"},
	{0,0,CTC_DATA_STR,"HSFLAG","回送标志"},
	{0,0,CTC_DATA_STR,"ZBFLAG","自备标志"},
	{0,0,CTC_DATA_STR,"JYFLAG","军用标志"},
};

int    count_of_quebao_def()
{
	return sizeof(g_quebao_def)/sizeof(g_quebao_def[0]);
}

PropertyInfoPara g_mplan_trninfo_def[]=
{
	{0,0,CTC_DATA_STR,"ID","序号"},
	{0,0,CTC_DATA_STR,"DPTNO","出发车次"},
	{0,0,CTC_DATA_STR,"DPTST","出发车站"},
	{0,0,CTC_DATA_STR,"ARVST","接入车站"},
	{0,0,CTC_DATA_STR,"ARVNO","接入车次"},
	{0,0,CTC_DATA_STR,"STPTM","区间停车时分"},
	{0,0,CTC_DATA_STR,"INFO","区间信息"},
};

int    count_of_mplan_trninfo_def()
{
	return sizeof(g_mplan_trninfo_def)/sizeof(g_mplan_trninfo_def[0]);
}

PropertyInfoPara g_typlan_def[]=
{
	{0,0,CTC_DATA_STR,"TNO","特运车次"},
	{0,0,CTC_DATA_DATETIME,"STTM","开始时间"}, // 用CMD_DEST 作为CMDINDEX
	{0,0,CTC_DATA_DATETIME,"EDTM","结束时间"},
	{0,0,CTC_DATA_STR,"STST","起始车站"},
	{0,0,CTC_DATA_STR,"EDST","结束车站"},
	{0,0,CTC_DATA_INT,"NOTECNT","通知单数量"},
};

int    count_of_typlan_def()
{
	return sizeof(g_typlan_def)/sizeof(g_typlan_def[0]);
}

PropertyInfoPara g_typlan_dcmd_def[]=
{
	{0,ID_PD_TYPLAN_DCMD_ID,CTC_DATA_STR,"CID","命令序号"}, // 用CMD_DEST 作为CMDINDEX
	{0,0,CTC_DATA_STR,"CRECV","受令处所"},
	{0,0,CTC_DATA_INT,"CTYPE","命令类型"},
	{0,0,CTC_DATA_STR,"CSID","命令子类型"},
	{0,0,CTC_DATA_STR,"CTITLE","命令标题"},
	{0,0,CTC_DATA_STR,"CPNM","施工项目"},
	{0,0,CTC_DATA_INT,"CRCNT","受令机构数量"},
	{0,0,CTC_DATA_STR,"CRPT","受令机构"},
	{0,ID_PD_TYPLAN_DCMD_CONTENT,CTC_DATA_STR,"CCONTENT","命令内容"},
};

int    count_of_typlan_dcmd_def()
{
	return sizeof(g_typlan_dcmd_def)/sizeof(g_typlan_dcmd_def[0]);
}