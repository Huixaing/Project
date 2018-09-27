#include "stdafx.h"
#include "resource.h"
#include <io.h>
#include <afxinet.h>
#include "sysshare.h"
#include "TGPspData.h"
#include "td_data_def.h"
#include "utility/ctc_data_type_def.h"
#include "tg_schedule_base.h"
#include "utility/xml_utility.h"
#include "td_frame.h"
#include "tg_basicpara.h"
#import  <msxml.dll> named_guids
using namespace MSXML;

BOOL InitBZRecordFromXML(MSXML::IXMLDOMNodePtr pNodeRoot, CTD_BZInfo*obj);
BOOL GenDcmdFromXML(MSXML::IXMLDOMNodePtr pNodeRoot, int conid, CTD_MPLANDCMD* dcmd);
BOOL GenMarkFromXML(MSXML::IXMLDOMNodePtr pNodeRoot, CTD_MPLANMARK* mark);

#define  STR_CPY2(x,y)  { strncpy(x,y,sizeof(x)-1);x[sizeof(x)-1]=0;TruncString(x,sizeof(x));} //2010 南昌修改
struct {
	int opms_type;
	int ctc_type;
}g_mark_type_map[]
={
	{1,TG_SIDE_INHIBIT},
	{2,TG_ABS_INHIBIT},
	{3,TG_SIDE_SLOW},
	{4,TG_ABS_SLOW},
	{5,TG_SECTION_ELEC_FIX}
};
#define MARK_TYPE_MAP_NUM  sizeof(g_mark_type_map)/sizeof(g_mark_type_map[0])


static int CovertStationNameToNo(const char *stname)
{
	return gpcdef.GetStationNoByName(stname);
}

static bool IsStationNo(int station)
{
	if(CString("管外")==gpcdef.GetStationName(station))
		return false;
	return true;
}

//=========================================================================


BOOL TD_ImportTrainInfoFromTD(CTG_SingleTrain &ctctrain,CTD_TrainInfo &tdtrain,int mode)
{
	CString start_sta_name=tdtrain.STST, end_sta_name=tdtrain.ENDST;
	
	BYTE len = sizeof(ctctrain.GetTrainInfo()->m_data.start_sta_name);
	start_sta_name = start_sta_name.Trim();
	if(start_sta_name.GetLength()>=len)
		start_sta_name="";

	len = sizeof(ctctrain.GetTrainInfo()->m_data.end_sta_name);
	end_sta_name = end_sta_name.Trim();
	if(end_sta_name.GetLength()>=len)
		end_sta_name="";
	
	ctctrain.SetShiftTrainIndex(tdtrain.GTID);
	STR_CPY2(ctctrain.GetTrainInfo()->m_data.train_name,tdtrain.TNO); 
	STR_CPY2(ctctrain.GetTrainInfo()->m_data.start_sta_name, start_sta_name);
	STR_CPY2(ctctrain.GetTrainInfo()->m_data.end_sta_name, end_sta_name);
	ctctrain.GetTrainInfo()->m_data.sborder_actual_time=tdtrain.FJTM;
	ctctrain.GetTrainInfo()->m_data.sborder_plan_time=tdtrain.FJTDTM;
	ctctrain.GetTrainInfo()->m_data.start_actual_time=tdtrain.STTM;
	ctctrain.GetTrainInfo()->m_data.start_plan_time=tdtrain.STTM;

	if(GetTGFunctionConfig()->IsEnableNewCzh())
	{
		CTG_TrainRelation relation;
		if(mode==1)
		{
			if(tdtrain.EMU.GetLength()>0 && tdtrain.EMU.GetLength()<30)
				relation.czh_no_str = tdtrain.EMU;
			else
				relation.czh_no_str = "";
			relation.prev_train_index=tdtrain.RTID;
			relation.prev_shift_index=tdtrain.RTID;
			relation.next_shift_index=tdtrain.DGID;
			relation.prev_basic_index=0;
			GpcLogTrainMessage(0, 0, "TID=%s,GID=%u 使用OPMS传来的交路勾连关系:%s RTID:%u DGID: %u.",
				tdtrain.TNO, tdtrain.GTID, tdtrain.EMU, tdtrain.RTID, tdtrain.DGID);
		}
		else if(mode==2)
		{
			GpcLogTrainMessage(0, 0, "TID=%s,GID=%u 清空列车交路信息,不使用OPMS传来的交路勾连关系:%s RTID:%u DGID: %u.",
				tdtrain.TNO, tdtrain.GTID, tdtrain.EMU, tdtrain.RTID, tdtrain.DGID);
		}
		else
		{
			GpcLogTrainMessage(0, 0, "TID=%s,GID=%u 不使用OPMS传来的交路勾连关系:%s RTID:%u DGID: %u.",
				tdtrain.TNO, tdtrain.GTID, tdtrain.EMU, tdtrain.RTID, tdtrain.DGID);
		}
		    
		int from_st=CovertStationNameToNo(tdtrain.FROMST);
		if(from_st>0)
			relation.prev_dest_sta=from_st;
		else
			relation.prev_dest_sta=0;

		int to_st=CovertStationNameToNo(tdtrain.TOST);
		if(to_st>0)
			relation.next_dest_sta=to_st;
		else
			relation.next_dest_sta=0;

		ctctrain.GetTrainInfoExpand()->UpdateTrainRelation(relation);
	}
		
	// 2014.4.12 上海局（暂时未做操劳时间和避让条件）
	//HALT

	BYTE run_type=0,prio=0,locotype=0;
#ifdef USE_OPMS_TRAIN_TYPE
	if(GetTGBasicParam()->GetTrainTypeDef(tdtrain.TRAINTYPE,&run_type,&prio,&locotype))
	{
		ctctrain.GetTrainInfo()->m_data.type=tdtrain.TRAINTYPE;
		ctctrain.GetTrainInfo()->m_data.run_type = run_type;
		ctctrain.GetTrainInfo()->m_data.priority = prio;
		ctctrain.SetDefaultAttribByDefinedFlag(locotype);
	}
	else
#endif
		ctctrain.GetTrainInfo()->m_data.type=255;
	return TRUE;
}
static BOOL TD_LoadTrainInfo(MSXML::IXMLDOMNodePtr pNodeRoot,CTD_TrainInfo &traininfo)
{
	GET_NODE_INTVALUE(traininfo.SCHTYP,pNodeRoot,"SCHTYP")
		GET_NODE_INTVALUE(traininfo.YLFG,pNodeRoot,"YLFG")
		GET_NODE_INTVALUE(traininfo.LYLFG,pNodeRoot,"LYLFG")
		GET_NODE_STRVALUE(traininfo.TNO,pNodeRoot,"TNO")
		GET_NODE_STRVALUE(traininfo.STST,pNodeRoot,"STST")
		GET_NODE_STRVALUE(traininfo.ENDST,pNodeRoot,"ENDST")
		GET_NODE_INTVALUE(traininfo.IFLAG,pNodeRoot,"IFLAG")
		GET_NODE_INTVALUE(traininfo.OFLAG,pNodeRoot,"OFLAG")
		GET_NODE_INTVALUE(traininfo.GTID,pNodeRoot,"GTID")
		GET_NODE_TIMEVALUE(traininfo.LSTTM,pNodeRoot,"LSTTM")
		GET_NODE_TIMEVALUE(traininfo.LEDTM,pNodeRoot,"LEDTM")
		GET_NODE_TIMEVALUE(traininfo.STTM,pNodeRoot,"STTM")
		GET_NODE_TIMEVALUE(traininfo.FJTM,pNodeRoot,"FJTM")
		GET_NODE_INTVALUE(traininfo.SEQ,pNodeRoot,"SEQ")
		GET_NODE_INTVALUE(traininfo.COUNT,pNodeRoot,"COUNT")
		GET_NODE_TIMEVALUE(traininfo.FJTDTM,pNodeRoot,"FJTDTM")
		GET_NODE_INTVALUE(traininfo.TRAINTYPE,pNodeRoot,"TRAINTYPE")
		GET_NODE_STRVALUE(traininfo.FROMST,pNodeRoot,"FROMST")
		GET_NODE_STRVALUE(traininfo.TOST,pNodeRoot,"TOST")

		GET_NODE_INTVALUE(traininfo.RTID,pNodeRoot,"ROUTEID")
		if(traininfo.RTID==0xFFFFFFFF)    //!!!
			traininfo.RTID=0;
		GET_NODE_STRVALUE(traininfo.EMU,pNodeRoot,"EMUNO")
		GET_NODE_INTVALUE(traininfo.DGID,pNodeRoot,"DGID")
		if(traininfo.DGID==0xFFFFFFFF)
			traininfo.DGID=0;
		GET_NODE_INTVALUE(traininfo.HALT,pNodeRoot,"HALT")
		if(traininfo.HALT==-1)
			traininfo.HALT=0;
		traininfo.m_nStationCount=0;
	return TRUE;
}

void SetTrainLastStationExit(CTD_TrainInfo& traininfo, CTG_SingleTrain &newtrain, int schd_type); //schd_type:1日班计划、调整图 2基本图 主要针对接入和交出
void SetTrainFirstStationEntry(CTD_TrainInfo& traininfo, CTG_SingleTrain &newtrain, int schd_type); //schd_type:1日班计划、调整图 2基本图 主要针对接入和交出

static int  GetEntryByTmisAbsNumber(int station,int tmisabsnum)
{
	long absnum=GetTGBasicParam()->GetAbsNumByTmisNum(tmisabsnum);
	if(absnum<0)
		absnum=tmisabsnum;
	ushort st1=0,etr1=255,st2=0,etr2=255;
	GetTGBasicParam()->ConvertAbsnumToStationEntry(&st1,&etr1,&st2,&etr2,absnum);
	if(station==st1)
		return etr1;
	if(station==st2)
		return etr2;
	return -1;
}

BOOL TD_ImportRecordInfoFromTD(CTG_TrainRecord &ctcrecord,CTD_TrainRecordInfo &tdrecord,CTD_TrainInfo &info, bool bAcceptSide, CStringArray& list)
{
	GpcLogTrainMessage(0, 0, "TDMS信息:TID=%s,ST=%s(%d) ARTID=%s DPTID=%s ENTRY=%d, EXIT=%d ARND=%d DPND=%d SIDE=%d",
		info.TNO,tdrecord.STNM,tdrecord.STID,tdrecord.ARTNO,tdrecord.DPTNO,tdrecord.ENTRY_LINE,tdrecord.EXIT_LINE,
		tdrecord.ARND,tdrecord.DPND,tdrecord.SIDENO);

	// 增加节点号的导入   20090908处理基本图
	if(GetTGFunctionConfig()->IsEnableOPMSNodeIndex())
	{
		GpcLogTrainMessage(0, 0, "使用TDMS站序,TID=%s,ST=%s ARND=%d DPND=%d",info.TNO,tdrecord.STNM,tdrecord.ARND,tdrecord.DPND);
		if(tdrecord.ARND>=1 && tdrecord.ARND<=254)
			ctcrecord.SetArriveTGIndex(tdrecord.ARND-1);  // TDMS传来的节点号从1开始,我们内部从0开始
		else
		{
			ctcrecord.SetArriveTGIndex(254);  // 253<=nd<=0:为合理值 254：为非法值
			CString text;
			text.Format("车次:%s %s 站序错误为%d", info.TNO,tdrecord.STNM, tdrecord.ARND);
			list.Add(text);
		}
		
		if(tdrecord.DPND>=1 && tdrecord.DPND<=254)
			ctcrecord.SetDepartTGIndex(tdrecord.DPND-1);  // TDMS传来的节点号从1开始,我们内部从0开始
		else
		{
			ctcrecord.SetDepartTGIndex(254);  // 253<=nd<=0:为合理值 254：为非法值
			CString text;
			text.Format("车次:%s %s 站序错误为%d", info.TNO,tdrecord.STNM, tdrecord.DPND);
			list.Add(text);
		}
	}
	else
	{
		GpcLogTrainMessage(0, 0, "不使用TDMS站序,TID=%s,ST=%s ARND=%d DPND=%d",info.TNO,tdrecord.STNM,tdrecord.ARND,tdrecord.DPND);
	}

	if(bAcceptSide)
	{
		ctcrecord.m_data.arrive_side = tdrecord.SIDENO;
		ctcrecord.m_data.depart_side = tdrecord.SIDENO;
		GpcLogTrainMessage(0, 0, "使用TDMS股道,TID=%s,ST=%s(%d) SIDENO=%d",info.TNO,tdrecord.STNM,tdrecord.STID,tdrecord.SIDENO);
	}
	else
	{
		GpcLogTrainMessage(0, 0, "保留原股道,不使用OPMS传来的股道,TID=%s,ST=%s(%d) SIDENO=%d",info.TNO,tdrecord.STNM,tdrecord.STID,tdrecord.SIDENO);
	}
	/////////////////////////////////////////////
	if(tdrecord.STID>0) 
		ctcrecord.m_data.station=tdrecord.STID;
	else
		ctcrecord.m_data.station=CovertStationNameToNo(tdrecord.STNM);
	// 检查车站站号是否未知
	if(!IsStationNo(ctcrecord.m_data.station))
	{
		CString text;
		text.Format("车次:%s %s 站号错误为%d,运行线断线", info.TNO, tdrecord.STNM, tdrecord.STID);
		list.Add(text);
	}
	
	STR_CPY(ctcrecord.m_data.arrive_train_id,tdrecord.ARTNO);
	STR_CPY(ctcrecord.m_data.depart_train_id,tdrecord.DPTNO);

	if(tdrecord.SARTM>0)
		ctcrecord.m_data.plan_arrive=tdrecord.SARTM;
	if(tdrecord.SDPTM>0)
		ctcrecord.m_data.plan_depart=tdrecord.SDPTM;
	if(!ctcrecord.IsActualArriveTimeRecord()) // 计划点才更新 20090811
	{
		if(tdrecord.ARTM>0)
			ctcrecord.m_data.arrive=tdrecord.ARTM;
	}
	if(!ctcrecord.IsActualDepartTimeRecord())  // 计划点才更新 20090811
	{
		if(tdrecord.DPTM>0)
			ctcrecord.m_data.depart=tdrecord.DPTM;
	}
	if(tdrecord.BSTM>=0)
		ctcrecord.m_data.min_stop_time=tdrecord.BSTM*60;
	else
		ctcrecord.m_data.min_stop_time=0;

	// OPMS 解析IMPINFO
						// bit 0:  超限
						// bit 1:  重点警卫
						// bit 2:  毒品
						// bit 3:  军运
	if(tdrecord.IMPINFO!=0xFFFF)  // 当XML中没有IMPINFO时，赋值为65535,此时不可以用
	{
		// 低2个字节和OPMS对应
		ctcrecord.m_data.rank&=0xFFFF0000;
		if(tdrecord.IMPINFO & 0x01)
		{
			TG_SetSpaceRank(ctcrecord.m_data);
			ctcrecord.SetArriveSpace(1);
			ctcrecord.SetDepartSpace(1);
		}
		if(tdrecord.IMPINFO & 0x02)
			TG_SetSpecialGaurdRank(ctcrecord.m_data);
		if(tdrecord.IMPINFO & 0x04)
			TG_SetGoustRank(ctcrecord.m_data);
		if(tdrecord.IMPINFO & 0x08)
			TG_SetMilitaryRank(ctcrecord.m_data);
	}

	int entry=GetEntryByTmisAbsNumber(ctcrecord.m_data.station,tdrecord.ENTRY_LINE);
	int exit=GetEntryByTmisAbsNumber(ctcrecord.m_data.station,tdrecord.EXIT_LINE);

	if(GetTGFunctionConfig()->IsEnableTDMS5_0()) //TD5.0
	{
		TG_TrainOPMSRecordInfo opms;
		opms.work_flag=tdrecord.WORKFLAG;
		opms.oper_flag=tdrecord.OPERFLAG;
		opms.important_flag=0;
		if(tdrecord.OPERINFO.GetLength()>0 && tdrecord.OPERINFO.GetLength()<250)
		{
			STR_CPY(opms.oper_info,tdrecord.OPERINFO);
		}
		else
		{
			opms.oper_info[0]=0;
		}
		opms.note_text[0]=0;
		ctcrecord.SetRecordKK(opms);
	}
	
	ctcrecord.m_data.entry = entry;
	ctcrecord.m_data.exit = exit;
	ctcrecord.tdms_entry = entry;
	ctcrecord.tdms_exit  = exit;
	CString sginfo;
	if(tdrecord.SGFLAG==1)
		sginfo="换乘,";
	else if(tdrecord.SGFLAG==2)
		sginfo="换挂,";
	else if(tdrecord.SGFLAG==2) //???
		sginfo="换乘换挂,";
	sginfo+=tdrecord.SGINFO;

	ctcrecord.m_strNoteText=tdrecord.NOTE_TEXT; // 2017.01.07 济南接收重点事项

	// qiaoyt 1103 运输信息
	if(GetTGFunctionConfig()->IsEnableTDMS5_0()) //TD5.0
		ctcrecord.SetStationText(tdrecord.SGINFO,STATION_TXT_INFO_TYPE_OPMS); 
	else
		ctcrecord.SetStationText(tdrecord.SGINFO+tdrecord.OPERINFO,STATION_TXT_INFO_TYPE_OPMS); 
	if(ctcrecord.IsOpmsSetSationText())
	{
		ctcrecord.SetStationText(tdrecord.SGINFO,STATION_TXT_INFO_TYPE_SHUAIGUA);
		ctcrecord.SetOpmsSationTextState("1");
	}

	if (tdrecord.OPERFLAG==1)
	{
		TG_ClearFlagMustNoPass(ctcrecord.m_data);
		TG_SetFlagMustNoPass(ctcrecord.m_data);
	}
	else
		TG_ClearFlagMustNoPass(ctcrecord.m_data);

	if(tdrecord.STPINFO==1) // 
	{
		TG_ClearFlagMustStop(ctcrecord.m_data);		
		TG_SetFlagMustNotStop(ctcrecord.m_data);
	}
	else if(tdrecord.STPINFO==2) // 
	{
		TG_ClearFlagMustNotStop(ctcrecord.m_data);
		TG_SetFlagMustStop(ctcrecord.m_data);
	}
	else
	{
		TG_ClearFlagMustStop(ctcrecord.m_data);		
		TG_ClearFlagMustNotStop(ctcrecord.m_data);
	}
	TG_SetTechStopTrain(ctcrecord.m_data); //默认技术停点
	if(tdrecord.m_nNotMeetCount>0)
	{
		// can_not_meet: 低字节： 存在老禁会标志 次低字节 存新禁会标志
		if (GetTGFunctionConfig()->IsNotMeetBySpeed()) // 按照上海局 新模式: 0\禁会200km.h以上列车 1\禁会120-200km.h的列车 2\禁会120以下列车 3\禁会所有列车
		{
			if(tdrecord.METINFO[0]==0) // 禁会200km/h以上列车
			{
				ctcrecord.m_data.can_not_meet=NOTMEET_200_orUp;
			}
			else if(tdrecord.METINFO[0]==1) // 禁会120-200km/h的列车
			{
				ctcrecord.m_data.can_not_meet=NOTMEET_120_200;
			}
			else if(tdrecord.METINFO[0]==2) // 禁会120km/h以下列车
			{
				ctcrecord.m_data.can_not_meet=NOTMEET_120_Low;
			}
			else if(tdrecord.METINFO[0]==3) // 禁会所有列车
			{
				ctcrecord.m_data.can_not_meet=NOTMEET_ALL;
			}
		}
		else
		{
			if(tdrecord.METINFO[0]==1) // 禁会动车
			{
				ctcrecord.m_data.can_not_meet=NOTMEET_CRH;// 注意此处和ctc_data_type_def 中定义以及OCX中一致
			}
			else if(tdrecord.METINFO[0]==4) // 禁会直特
			{
				ctcrecord.m_data.can_not_meet=NOTMEET_SEPECIAL_Z; // 注意此处和ctc_data_type_def 中定义以及OCX中一致
			}
			else if(tdrecord.METINFO[0]==2) // 禁会客车
			{
				ctcrecord.m_data.can_not_meet=NOTMEET_PASSENGER; // 注意此处和ctc_data_type_def 中定义以及OCX中一致
			}
			else if(tdrecord.METINFO[0]==3) // 禁会列车
			{
				ctcrecord.m_data.can_not_meet=NOTMEET_FREIGHT; // 注意此处和ctc_data_type_def 中定义以及OCX中一致
			}
		}
	}

	GpcLogTrainMessage(0, 0, "CTC信息:TID=%s ST=%s(%d) ARTID=%s DPTID=%s ENTRY=%d EXIT=%d SIDE=%d/%d", 
		info.TNO,tdrecord.STNM,ctcrecord.m_data.station, 
		ctcrecord.m_data.arrive_train_id,ctcrecord.m_data.depart_train_id,
		ctcrecord.m_data.entry,ctcrecord.m_data.exit,
		ctcrecord.m_data.arrive_side,ctcrecord.m_data.depart_side);	
	return TRUE;
}

static BOOL TD_LoadTrainRecordInfo(MSXML::IXMLDOMNodePtr pNodeRoot,CTD_TrainRecordInfo &recordinfo)
{
	    GET_NODE_STRVALUE(recordinfo.STNM,pNodeRoot,"STNM")
		GET_NODE_INTVALUE(recordinfo.STID,pNodeRoot,"STID")
		// 在Import中判断是否需要从站名转换
		//if(recordinfo.STID<=0)
		//	recordinfo.STID=CovertStationNameToNo(recordinfo.STNM);
		GET_NODE_INTVALUE(recordinfo.NODESEQ,pNodeRoot,"NODESEQ")
		GET_NODE_STRVALUE(recordinfo.ARTNO,pNodeRoot,"ARTNO")
		GET_NODE_STRVALUE(recordinfo.DPTNO,pNodeRoot,"DPTNO")
		
		GET_NODE_INTVALUE(recordinfo.SIDENO,pNodeRoot,"SIDENM")

		GET_NODE_INTVALUE(recordinfo.ARND,pNodeRoot,"ARND")
		GET_NODE_INTVALUE(recordinfo.DPND,pNodeRoot,"DPND")
		GET_NODE_INTVALUE(recordinfo.ENTRY_LINE,pNodeRoot,"ENTRY")
		GET_NODE_INTVALUE(recordinfo.EXIT_LINE,pNodeRoot,"EXIT")
		GET_NODE_INTVALUE(recordinfo.IMPINFO,pNodeRoot,"IMPINFO")
		GET_NODE_INTVALUE(recordinfo.STPINFO,pNodeRoot,"STPINFO")
		GET_NODE_STRVALUE(recordinfo.SGINFO,pNodeRoot,"SGINFO")

		GET_NODE_INTVALUE(recordinfo.SGFLAG,pNodeRoot,"SGFLAG")

		GET_NODE_TIMEVALUE(recordinfo.ARTM,pNodeRoot,"ARTM")
		GET_NODE_TIMEVALUE(recordinfo.DPTM,pNodeRoot,"DPTM")
		GET_NODE_TIMEVALUE(recordinfo.SARTM,pNodeRoot,"SARTM")
		GET_NODE_TIMEVALUE(recordinfo.SDPTM,pNodeRoot,"SDPTM")

		GET_NODE_INTVALUE(recordinfo.BSTM,pNodeRoot,"BSTM")

		//added by zhaoliang 20100529
		GET_NODE_INTVALUE(recordinfo.ARR_ENGINE_NUM,pNodeRoot,"ARR_ENGINE_NUM")//机车数量
		GET_NODE_STRVALUE(recordinfo.ARR_ENGINE_NAME,pNodeRoot,"ARR_ENGINE_NAME")//机车名称=机车类型+机车号
		GET_NODE_INTVALUE(recordinfo.DEP_ENGINE_NUM,pNodeRoot,"DEP_ENGINE_NUM")
		GET_NODE_STRVALUE(recordinfo.DEP_ENGINE_NAME,pNodeRoot,"DEP_ENGINE_NAME")

		GET_NODE_INTVALUE(recordinfo.OPERFLAG,pNodeRoot,"OPERFLAG")
		GET_NODE_STRVALUE(recordinfo.OPERINFO,pNodeRoot,"OPERINFO")
		GET_NODE_INTVALUE(recordinfo.WORKFLAG,pNodeRoot,"WORKFLAG")

		GET_NODE_TIMEVALUE(recordinfo.CWCL,pNodeRoot,"CWCL")

		// 2017.01.07 济南接收重点事项
		if(GetTGFunctionConfig()->IsAcceptImportNote())
		{
			GET_NODE_STRVALUE(recordinfo.NOTE_TEXT,pNodeRoot,"NOTE_TEXT")
		}
		else
		{
			recordinfo.NOTE_TEXT="";
		}

		CString tmp;
		GET_NODE_STRVALUE(tmp,pNodeRoot,"METINFO")
		int y=0,x=0;
		recordinfo.m_nNotMeetCount=0;
		while((x=tmp.Find(',',y))>=0 && recordinfo.m_nNotMeetCount<20)
		{
			recordinfo.METINFO[recordinfo.m_nNotMeetCount++]=atoi(tmp.Mid(y,x-y));
			y=x+1;
		}
		return TRUE;
}

// ptrainbase非空，且存在原车，则保留原信息，并且对于原有记录，更新记录中信息，获取计划中停车信息，如果非禁停，则保留原来的停车信息，且平移后记录，对于计划时间内>sttm的始发局间接入的，更新始发信息
// 不存在原车，则全部采用计划中信息，存在很多时间为0的记录，则需要在中计算
BOOL TD_LoadTrainFromXML(const CTG_ScheduleInfo& schdinfo, CTG_TrainScheduleBase *ptrainbase,CTG_SingleTrain &newtrain,const char *xml,const char *node,int *curindex,int *totalcount,int *pflag,time_t sttm,time_t endtm,int mode, CStringArray &train_alarm)
{
	CTD_TrainInfo traininfo;
	CTD_TrainRecordInfo recordinfo;
	GpcLogTrainMessage(0, 0, "===================================");
	if (GetTGFunctionConfig()->IsEnableLogTDMessage())
		GpcLogTrainMessage(0, 0, "%s", xml);
	
	LOAD_XML(xml,node,pRootNode)
	if(!TD_LoadTrainInfo(pRootNode,traininfo))
		return FALSE;
	GpcLogTrainMessage(0, 0, "OPMS车站,TID=%s FROMST=%s TOST=%s",traininfo.TNO,traininfo.FROMST,traininfo.TOST);
	if(pflag)
	{
		*pflag=traininfo.LYLFG;
		if(traininfo.YLFG==0)
		{
			*pflag=0; 
		}
		if (traininfo.HALT==1)
		{
			*pflag=0;
		}
	}
	if(curindex)
		*curindex=traininfo.SEQ;
	if(totalcount)
		*totalcount=traininfo.COUNT;
	BOOL ign_time=FALSE;
	CTG_SingleTrain *prev=NULL;
	if(ptrainbase)
	{
		prev=ptrainbase->GetSingleTrainByShiftIndex(traininfo.GTID, NULL_STATION);
	}
	if(prev)
		newtrain=*prev;

	MSXML::IXMLDOMNodeListPtr pList = pRootNode->selectNodes( "ST_RECORD" );
	int rec_count = pList->Getlength();

	CTG_TrainRecordList reclist;
	if(newtrain.GetTrainRecordCount()>0)
		reclist=*newtrain.GetRecordList();  // 保留原有信息

	newtrain.GetRecordList()->PurgeRecords();  

	if(newtrain.HasActualRecord() >= 0)
		ign_time=TRUE;

	BOOL bUseOPMSShiftSchdSide = GetTGFunctionConfig()->IsEnableOPMSShiftSchdSide(schdinfo.m_nSectionID);
	for(int idx = 0; idx < rec_count; idx++)
	{
		if(!TD_LoadTrainRecordInfo(pList->Getitem(idx),recordinfo))
			return FALSE;
		
		GpcLogTrainMessage(0, 0, "OPMS传来的股道,TID=%s,ST=%s(%d) SIDENO=%d",
					traininfo.TNO,recordinfo.STNM,recordinfo.STID,recordinfo.SIDENO);

		if( !bUseOPMSShiftSchdSide)
		{
			recordinfo.SIDENO=255;
		}
		int stop_time;
		if(recordinfo.DPTM>recordinfo.ARTM)
			stop_time=recordinfo.DPTM-recordinfo.ARTM;
		else
			stop_time=0;

		bool bAcceptSide=true;
		CTG_TrainRecord  newrecord;

		if(idx<reclist.GetRecordCount())
		{
			if(reclist[idx].GetStationNo()== recordinfo.STID)
			{
				newrecord=reclist[idx];

				if (rec_count>=reclist.GetRecordCount() && reclist[idx].IsTerminalRec() && (idx+1==reclist.GetRecordCount())) // 20161123 hjz 接收调整计划时 如果新线有延长 将旧线的终到点改为自触
				{
					if(!newrecord.IsActualDepartTimeRecord())
						TG_SetAutoMakeDepartRoute(newrecord.m_data);	
					TG_ClearFlagGiveoutTerminal(newrecord.m_data.flag);
				}

				bAcceptSide=false;
				if(idx==0 && sttm>0)
				{
					if(recordinfo.DPTM<sttm || newrecord.m_data.depart<sttm)
						ign_time=TRUE;

				}
				if(idx==0)
				{
					// 非分界站,且非始发---局内接入列车
					if(!GetTGFunctionConfig()->IsFJKStation(recordinfo.STID) && traininfo.IFLAG!=1)  // 非分界站,且非始发
						ign_time=TRUE;
				}
			}
		}
	
		if(ign_time && !GetTGFunctionConfig()->IsEnableRecvAdjustPlanTime())
		{
			recordinfo.ARTM=0;
			recordinfo.DPTM=0;
		}
		TD_ImportRecordInfoFromTD(newrecord,recordinfo,traininfo,bAcceptSide, train_alarm);
				
		if(newrecord.m_data.entry==255|| newrecord.m_data.exit==255)
		{
			GpcLogTrainMessage(0, 0, "OPMS传来线别转换为出入口错误,TID=%s,ST=%s (%d,%d)=>(%d,%d)",
				traininfo.TNO,recordinfo.STNM,recordinfo.ENTRY_LINE,recordinfo.EXIT_LINE,
				newrecord.m_data.entry, newrecord.m_data.exit);
		}

		int prev_stop_time=newrecord.m_data.depart-newrecord.m_data.arrive;
		if(prev_stop_time<stop_time)
		{
			if(!newrecord.IsActualDepartTimeRecord())  // 非实际点才更新 20090811
			{
				newrecord.m_data.depart=newrecord.m_data.arrive+stop_time;
				for(int x=idx+1;x<reclist.GetRecordCount();x++)
				{
					reclist[x].m_data.arrive+=stop_time-prev_stop_time;
					if(!reclist[x].IsTerminalRec())
						reclist[x].m_data.depart+=stop_time-prev_stop_time;
				}
			}
		}
		newrecord.rec_index=idx;
		newtrain.GetRecordList()->AddRecord(newrecord);
	}
	/*added by zhaoliang 20100530
	遍历节点
	找到第一组机车跳出
	将此组机车最为该运行线机车号
	对每个节点，优先判断发点机车
	机车号码之间通过 ， 号隔开
	机车ID与机车类型用 / 隔开
	int     ARR_ENGINE_NUM;//机车数量
	CString ARR_ENGINE_NAME;//机车名称=机车类型+机车号
	int     DEP_ENGINE_NUM;
	CString DEP_ENGINE_NAME;
	*/
	for(int idx = 0; idx < rec_count; idx++)
	{
		if(!TD_LoadTrainRecordInfo(pList->Getitem(idx),recordinfo))//recordinfo 存放 节点信息
			return FALSE;
		CString engine_name;
		engine_name.Empty();
		if (!recordinfo.DEP_ENGINE_NAME.IsEmpty())//根据兰州特殊情况，对每个节点先判断发点机车
		{
			engine_name = recordinfo.DEP_ENGINE_NAME;
		} 
		else if(!recordinfo.ARR_ENGINE_NAME.IsEmpty()&&idx != 0)//判断到点机车,且第一个节点不判断到点机车
		{
			engine_name = recordinfo.ARR_ENGINE_NAME;
		}

		if (!engine_name.IsEmpty())//非空
		{
			CString strenginetmp,strengname,StrEngine_Style,StrEngine_ID;
			int Engine_Style;
			
			CStringArray EnginenameArray;//存放所有机车号
			CTG_TrainEngineList engineList;
			CTG_TrainEngineInfo engineInfo;
			strenginetmp.Empty();
			strengname.Empty();
			StrEngine_Style.Empty();
			StrEngine_ID.Empty();
			strenginetmp=engine_name;
			//提出机车号
			int cnt = ConvertToSubStringArray(strenginetmp,';',EnginenameArray);
			//if (strenginetmp.GetAt(strenginetmp.GetLength()-1))
			//{
			//	strenginetmp.Delete(strenginetmp.GetLength()-1,strenginetmp.GetLength()-1);
			//}
			//while (true)
			//{
			//	if (strenginetmp.Find (';') == -1)//协议变化
			//	{
			//		EnginenameArray.Add(strenginetmp);
			//		break;    
			//	} 
			//	else
			//	{
			//		strengname=strenginetmp.Left( strenginetmp.Find (';'));//抽出左侧第一个机车
			//		strenginetmp=strenginetmp.Right(strenginetmp.GetLength()-strenginetmp.Find (';')-1);
			//		EnginenameArray.Add(strengname);
			//	}
			//}
			//保存机车号
			for (int i=0;i < EnginenameArray.GetSize();i++)
			{ 
				strengname = EnginenameArray.GetAt(i);//依次取机车
				//解析机车

				Engine_Style=atoi(strengname.Left(3)); //前三位 机车类型
				StrEngine_ID=strengname.Right(5);      //后五位 机车号

				int section = schdinfo.m_nSectionID;//gpcdef.GetUserGpcId();
				StrEngine_Style = GetTGBasicParam()->GetLocoPreName(section, strengname.Left(3));
				//读入机车
				ZeroMemory(&engineInfo.m_data, 0);
				strncpy(engineInfo.m_data.engine_id, strengname, 8);          //engine  ID
				engineInfo.m_data.engine_id[8]='\0';
				strncpy(engineInfo.m_data.engine_type, StrEngine_Style,9);   //engine  type
				engineInfo.m_data.engine_id[9]='\0';
				
				//2015.1.9 hjz增加日班计划接收机车号后 电力属性的判断
				int cascoType = GetTGBasicParam()->GetCascoIdByLocoId(section, Engine_Style);
				if(cascoType >= 128)
					TG_SetElecEngineExt(engineInfo.m_data.engine_status);
				else
					TG_SetDieselEngineExt(engineInfo.m_data.engine_status);

				engineInfo.m_data.attach_sta = 0;                                //attach  station
				engineInfo.m_data.unattach_sta = rec_count - 1;                  //unattach  station
				//存放机车
				engineList.AddRecord(engineInfo);
				newtrain.UpdateEngineInfo(engineList, 1); 

			}
			//释放 CStringArray
			EnginenameArray.RemoveAll();
			EnginenameArray.FreeExtra();

			break;
		}
	}
    //end for engine 

	newtrain.GetTrainInfo()->m_data.delete_flag=newtrain.GetTrainRecordCountWithDeleted();     

	TD_ImportTrainInfoFromTD(newtrain,traininfo,mode);

	SetTrainFirstStationEntry(traininfo, newtrain,1);
	SetTrainLastStationExit(traininfo, newtrain,1);
	
	for(int idx=0;idx<newtrain.GetTrainRecordCount();idx++)
	{
		CTG_TrainRecord *prec=newtrain.GetTrainRecord(idx);
		if(!prec || prec->GetStationNo()<=0)
		{
			*pflag=0;    // 非本台车站
			break;
		}
	}
	return TRUE;
}

BOOL TD_LoadShiftScheduleTrainFromXML(const CTG_ScheduleInfo& schdinfo, CTG_SingleTrain &newtrain,const char *xml,const char *node,int *curindex,int *totalcount, int mode, CStringArray &train_alarm)
{
	CTD_TrainInfo traininfo;
	CTD_TrainRecordInfo recordinfo;
	if (GetTGFunctionConfig()->IsEnableLogTDMessage())
		GpcLogTrainMessage(0, 0, "%s", xml);
	LOAD_XML(xml,node,pRootNode)
		if(!TD_LoadTrainInfo(pRootNode,traininfo))
			return FALSE;
	GpcLogTrainMessage(0, 0, "OPMS车站,TID=%s FROMST=%s TOST=%s",traininfo.TNO,traininfo.FROMST,traininfo.TOST);
	
	if(curindex)
		*curindex=traininfo.SEQ;
	if(totalcount)
		*totalcount=traininfo.COUNT;

	MSXML::IXMLDOMNodeListPtr pList = pRootNode->selectNodes( "ST_RECORD" );
	int rec_count = pList->Getlength();
	newtrain.GetRecordList()->PurgeRecords();  

	BOOL bUseOPMSShiftSchdSide = GetTGFunctionConfig()->IsEnableOPMSShiftSchdSide(schdinfo.m_nSectionID);
	for(int idx = 0; idx < rec_count; idx++)
	{
		CString node_text;
		if(!TD_LoadTrainRecordInfo(pList->Getitem(idx),recordinfo))
			return FALSE;

		//GpcLogTrainMessage(0, 0, "OPMS传来的股道,TID=%s,ST=%s(%d) SIDENO=%d",
		//			traininfo.TNO,recordinfo.STNM,recordinfo.STID,recordinfo.SIDENO);

		if( !bUseOPMSShiftSchdSide)
		{
			recordinfo.SIDENO=255;
		}
		int stop_time;
		if(recordinfo.DPTM>recordinfo.ARTM)
			stop_time=recordinfo.DPTM-recordinfo.ARTM;
		else
			stop_time=0;

		CTG_TrainRecord  newrecord;
		TD_ImportRecordInfoFromTD(newrecord,recordinfo,traininfo, true, train_alarm);
	
		newrecord.rec_index=idx;
		newtrain.GetRecordList()->AddRecord(newrecord);
	}
	/*added by zhaoliang 20100530
	遍历节点
	找到第一组机车跳出
	将此组机车最为该运行线机车号
	对每个节点，优先判断发点机车
	机车号码之间通过 ， 号隔开
	机车ID与机车类型用 / 隔开
	int     ARR_ENGINE_NUM;//机车数量
	CString ARR_ENGINE_NAME;//机车名称=机车类型+机车号
	int     DEP_ENGINE_NUM;
	CString DEP_ENGINE_NAME;
	*/
	for(int idx = 0; idx < rec_count; idx++)
	{
		if(!TD_LoadTrainRecordInfo(pList->Getitem(idx),recordinfo))//recordinfo 存放 节点信息
			return FALSE;
		CString engine_name;
		engine_name.Empty();
		if (!recordinfo.DEP_ENGINE_NAME.IsEmpty())//根据兰州特殊情况，对每个节点先判断发点机车
		{
			engine_name = recordinfo.DEP_ENGINE_NAME;
		} 
		else if(!recordinfo.ARR_ENGINE_NAME.IsEmpty()&&idx != 0)//判断到点机车,且第一个节点不判断到点机车
		{
			engine_name = recordinfo.ARR_ENGINE_NAME;
		}

		if (!engine_name.IsEmpty())//非空
		{
			CString strenginetmp,strengname,StrEngine_Style,StrEngine_ID;
			int Engine_Style;

			CStringArray EnginenameArray;//存放所有机车号
			CTG_TrainEngineList engineList;
			CTG_TrainEngineInfo engineInfo;
			strenginetmp.Empty();
			strengname.Empty();
			StrEngine_Style.Empty();
			StrEngine_ID.Empty();
			strenginetmp=engine_name;
			//提出机车号
			
			int cnt = ConvertToSubStringArray(strenginetmp,';',EnginenameArray);
			//保存机车号
			for (int i=0;i < EnginenameArray.GetSize();i++)
			{ 
				strengname = EnginenameArray.GetAt(i);//依次取机车
				//解析机车

				Engine_Style=atoi(strengname.Left(3)); //前三位 机车类型
				//StrEngine_ID=strengname.Right(5);      //后五位 机车号

				int section = schdinfo.m_nSectionID;//gpcdef.GetUserGpcId();
				StrEngine_Style = GetTGBasicParam()->GetLocoPreName(section, strengname.Left(3));
				//读入机车
				ZeroMemory(&engineInfo.m_data, 0);
				strncpy(engineInfo.m_data.engine_id, strengname, 8);          //engine  ID
				engineInfo.m_data.engine_id[8]='\0';
				strncpy(engineInfo.m_data.engine_type, StrEngine_Style,9);   //engine  type
				engineInfo.m_data.engine_id[9]='\0';

				int cascoType = GetTGBasicParam()->GetCascoIdByLocoId(section, Engine_Style);
				if(cascoType >= 128)
					TG_SetElecEngineExt(engineInfo.m_data.engine_status);
				else
					TG_SetDieselEngineExt(engineInfo.m_data.engine_status);


				engineInfo.m_data.attach_sta = 0;                                //attach  station
				engineInfo.m_data.unattach_sta = rec_count - 1;                  //unattach  station
				//存放机车
				engineList.AddRecord(engineInfo);
				newtrain.UpdateEngineInfo(engineList, 1); 

			}
			//释放 CStringArray
			EnginenameArray.RemoveAll();
			EnginenameArray.FreeExtra();

			break;
		}
	}
	//end for engine 

	newtrain.GetTrainInfo()->m_data.delete_flag=newtrain.GetTrainRecordCountWithDeleted();     

	TD_ImportTrainInfoFromTD(newtrain,traininfo,mode);

	SetTrainFirstStationEntry(traininfo, newtrain,1);
	SetTrainLastStationExit(traininfo, newtrain,1);
	
	

	return TRUE;
}


BOOL TD_LoadBasicScheduleTrainFromXML(CTG_SingleTrain &newtrain,const char *xml,const char *node,int *curindex,int *totalcount,int *ptype,CStringArray& train_alarm)
{
	CTD_TrainInfo traininfo;
	CTD_TrainRecordInfo recordinfo;

	GpcLogTrainMessage(0, 0, "===========================================================");
	if (GetTGFunctionConfig()->IsEnableLogTDMessage())
		GpcLogTrainMessage(0, 0, "%s", xml);
	LOAD_XML(xml,node,pRootNode)
		if(!TD_LoadTrainInfo(pRootNode,traininfo))
			return FALSE;
	GpcLogTrainMessage(0, 0, "OPMS车站,TID=%s FROMST=%s TOST=%s",traininfo.TNO,traininfo.FROMST,traininfo.TOST);
	if(traininfo.IFLAG==0)   // 基本图定义和日班计划相反？！
		traininfo.IFLAG=1;
	else
		traininfo.IFLAG=0;
	if(traininfo.OFLAG==0)   // 基本图定义和日班计划相反？！
		traininfo.OFLAG=1;
	else
		traininfo.OFLAG=0;

	if(ptype)
		*ptype=traininfo.SCHTYP;
	if(curindex)
		*curindex=traininfo.SEQ;
	if(totalcount)
		*totalcount=traininfo.COUNT;

	MSXML::IXMLDOMNodeListPtr pList = pRootNode->selectNodes( "ST_RECORD" );
	int rec_count = pList->Getlength();
	newtrain.GetRecordList()->PurgeRecords();  

	CTime basictm(2000,1,1,0,0,0);
	long basictime = basictm.GetTime();

	std::map<int, CTG_TrainRecord> tmpRecordMap; // 使用map是为了排序
	for(int idx = 0; idx < rec_count; idx++)
	{
		if(!TD_LoadTrainRecordInfo(pList->Getitem(idx),recordinfo))
			return FALSE;

		GpcLogTrainMessage(0, 0, "OPMS传来的基本图股道,TID=%s,ST=%s(%d) SIDENO=%d",
					traininfo.TNO,recordinfo.STNM,recordinfo.STID,recordinfo.SIDENO);

		CTG_TrainRecord  newrecord;
		
		TD_ImportRecordInfoFromTD(newrecord,recordinfo,traininfo,true,train_alarm);

		/// 基本图日期约定为 2000.1.1,减去之
		if(newrecord.m_data.arrive>=basictime)
			newrecord.m_data.arrive-=basictime;
		if(newrecord.m_data.depart>=basictime)
			newrecord.m_data.depart-=basictime;

		// 2010.03.02 xml中节点顺序不一定是实际列车节点顺序,应该用TD送的顺序
		//newrecord.rec_index=idx; 
		//newtrain.GetRecordList()->AddRecord(newrecord);
		if(recordinfo.NODESEQ >= 0)
		{
			newrecord.rec_index = recordinfo.NODESEQ;
			tmpRecordMap.insert(make_pair(newrecord.rec_index, newrecord));	
		}
	}

	if(tmpRecordMap.size()<=0)
		return FALSE;

	std::map<int, CTG_TrainRecord>::iterator it = tmpRecordMap.begin();
	while(it != tmpRecordMap.end())
	{
		newtrain.GetRecordList()->AddRecord(it->second);
		it++;
	}
	tmpRecordMap.clear();

	newtrain.SetTrainRecordCountToDefault();
	for (int i=0; i<newtrain.GetTrainRecordCount(); i++)
	{
		CTG_TrainRecord *prec=newtrain.GetTrainRecord(i);
		if(NULL != prec)
		{
			prec->m_data.arrive = prec->m_data.arrive%86400;
			prec->m_data.depart = prec->m_data.depart%86400;
		}
	}

	int day = 0;
	int before = -1;
	for (int i=0; i<newtrain.GetTrainRecordCount(); i++)
	{
		CTG_TrainRecord *prec=newtrain.GetTrainRecord(i);
		if(NULL != prec)
		{
			if(before > prec->m_data.arrive)
			{
				day++;
			}
			before = prec->m_data.arrive;
			prec->m_data.arrive += day*86400;
			if(before > prec->m_data.depart)
			{
				day++;
			}
			before = prec->m_data.depart;
			prec->m_data.depart += day*86400;
		}
	}

	for (int i=0; i<newtrain.GetTrainRecordCount(); i++)
	{
		CTG_TrainRecord *prec=newtrain.GetTrainRecord(i);
		if(NULL != prec)
		{
			prec->m_data.plan_arrive  = prec->m_data.arrive;
			prec->m_data.plan_depart  = prec->m_data.depart;
		}
	}

	TD_ImportTrainInfoFromTD(newtrain,traininfo,0); 
	// 初始化基本图交路关系
	newtrain.InitTrainExpand();
	
	SetTrainFirstStationEntry(traininfo, newtrain,2);
	SetTrainLastStationExit(traininfo, newtrain,2);
	
	return TRUE;
}

void TD_SetTrainTicketInfoFromXML(CTG_TrainScheduleBase *ptrainbase,const char *xml,const char *node)
{
	if(0!=stricmp(node,TOKEN_20_ACK_CREW_INFO))
		return;
	
	CTD_TrainTicketList ticketList;
	if(!ticketList.LoadCrewFromTDXML(xml, node))
		return;

	DWORD gtid;
	CString info;
	for(int i=0; i<ticketList.m_nCount; i++)
	{
		gtid=ticketList.m_list[i].GID;
		info=ticketList.m_list[i].GetCrewInfo();
		if(info.IsEmpty())
			continue;

		//////////////////////////
		CTG_SingleTrain *ptrain=ptrainbase->GetSingleTrainByShiftIndex(gtid,NULL_STATION);
		if(!ptrain)
			continue;
		const char *prevstr=ptrain->GetImportInfo(TRAIN_TXT_INFO_TYPE_CREW);
		if(prevstr && !stricmp(prevstr,info))  // 重复!
			continue;
		ptrain->SetImportInfo(info,TRAIN_TXT_INFO_TYPE_CREW);
	}

	return;
}

CTG_SingleTrain *TD_GetTrainInfoFromXML(CTG_TrainScheduleBase *ptrainbase,const char *xml,const char *node)
{
	CString info;
	DWORD gtid;
	int infoflag;
	if(!stricmp(node,TOKEN_15_ACK_KEDIAO_INFO))
	{
		LOAD_XML(xml,node,pNodeRoot)	  
			GET_NODE_INTVALUE(gtid,pNodeRoot,"GTID")
			GET_NODE_STRVALUE(info,pNodeRoot,"KDINFO")
			infoflag=TRAIN_TXT_INFO_TYPE_KEDIAO;
	}
	else if(!stricmp(node,TOKEN_16_ACK_YINGYE_INFO))
	{
		LOAD_XML(xml,node,pNodeRoot)	  
			GET_NODE_INTVALUE(gtid,pNodeRoot,"GTID")
			GET_NODE_STRVALUE(info,pNodeRoot,"YYINFO")
			infoflag=TRAIN_TXT_INFO_TYPE_YINGYE;
	}
	else
		return NULL;
	
	CTG_SingleTrain *ptrain=ptrainbase->GetSingleTrainByShiftIndex(gtid,NULL_STATION);
	if(!ptrain)
		return NULL;
	const char *prevstr=ptrain->GetImportInfo(infoflag);
	if(prevstr && !stricmp(prevstr,info))  // 重复!
		return NULL;
	ptrain->SetImportInfo(info,infoflag);
	return ptrain;
}

//===========================================================================

CTD_MPLANMARK::CTD_MPLANMARK()
{
	m_nGMID=0;
	m_nCTCType=0;
	m_nTDType=0;

	m_tStartTime=0;
	m_tEndTime=0;

	STKM=STM=ETKM=ETM=0;
	LIM=0;
	DIR=-1;
	STTB=EDTB=-1;
}
CTD_MPLANMARK &CTD_MPLANMARK::operator=(const CTD_MPLANMARK &other)
{
	m_nGMID=other.m_nGMID;
	m_nCTCType=other.m_nCTCType;
	m_nTDType=other.m_nTDType;
	
	m_tStartTime=other.m_tStartTime;
	m_tEndTime=other.m_tEndTime;
	
	PLANID=other.PLANID;
	STTB=other.STTB;
	EDTB=other.EDTB;
	STST=other.STST;
	EDST=other.EDST;
	STKM=other.STKM;
	STM=other.STM;
	ETKM=other.ETKM;
	ETM=other.ETM;
	LIM=other.LIM;
	DIR=other.DIR;
	OTHER=other.OTHER;
	CONTENT=other.CONTENT; 
	REASON=other.REASON;
	PLANCD=other.PLANCD;
	return *this;
}

BOOL CTD_MPLANMARK::operator== (const CTD_MPLANMARK &rhs) const
{
	if(rhs.m_nGMID != m_nGMID)
		return FALSE;

	if(rhs.PLANID != PLANID)
		return FALSE;

	if(rhs.m_nTDType != m_nTDType)
		return FALSE;
	if(rhs.m_nCTCType != m_nCTCType)
		return FALSE;
	if(rhs.m_tStartTime != m_tStartTime)
		return FALSE;
	if(rhs.m_tEndTime != m_tEndTime)
		return FALSE;
	
	if(rhs.STST != STST)
		return FALSE;
	if(rhs.EDST != EDST)
		return FALSE;
	if(rhs.OTHER != OTHER)
		return FALSE;

	if(rhs.REASON != REASON)
		return FALSE;

	if(rhs.STKM != STKM)
		return FALSE;
	if(rhs.STM != STM)
		return FALSE;
	if(rhs.ETKM != ETKM)
		return FALSE;
	if(rhs.ETM != ETM)
		return FALSE;

	if(rhs.LIM != LIM)
		return FALSE;
	if(rhs.DIR != DIR)
		return FALSE;

	if(rhs.CONTENT != CONTENT)
		return FALSE;

	if(rhs.STTB != STTB)
		return FALSE;
	if(rhs.EDTB != EDTB)
		return FALSE;

	if(rhs.PLANCD != PLANCD)
		return FALSE;
	
	return TRUE;
}

BOOL GenMarkFromXML(MSXML::IXMLDOMNodePtr pNodeRoot, CTD_MPLANMARK* mark)
{
	GET_NODE_STRVALUE(mark->PLANID,pNodeRoot,"PLANID")
	GET_NODE_INTVALUE(mark->m_nTDType,pNodeRoot,"TYPE")
	GET_NODE_TIMEVALUE(mark->m_tStartTime,pNodeRoot,"STTM")
	GET_NODE_TIMEVALUE(mark->m_tEndTime,pNodeRoot,"EDTM")

	GET_NODE_INTVALUE(mark->m_nGMID,pNodeRoot,"GMID")
	GET_NODE_STRVALUE(mark->STST,pNodeRoot,"STST")
	GET_NODE_STRVALUE(mark->EDST,pNodeRoot,"EDST")
    
	GET_NODE_INTVALUE(mark->STTB,pNodeRoot,"STTB")
	GET_NODE_INTVALUE(mark->EDTB,pNodeRoot,"EDTB")
		
	GET_NODE_INTVALUE(mark->STKM,pNodeRoot,"STKM")
	GET_NODE_INTVALUE(mark->STM,pNodeRoot,"STM")
	GET_NODE_INTVALUE(mark->ETKM,pNodeRoot,"EDKM")
	GET_NODE_INTVALUE(mark->ETM,pNodeRoot,"EDM")	 

	GET_NODE_INTVALUE(mark->LIM,pNodeRoot,"LIM")	 
	GET_NODE_STRVALUE(mark->OTHER,pNodeRoot,"OTHER")
	GET_NODE_INTVALUE(mark->DIR,pNodeRoot,"DIR")

	GET_NODE_STRVALUE(mark->PLANCD,pNodeRoot,"PLANCD")
	GET_NODE_STRVALUE(mark->CONTENT,pNodeRoot,"CONTENT")
	GET_NODE_STRVALUE(mark->REASON,pNodeRoot,"REASON")
	
	return TRUE;
}

BOOL  CTD_MPLANMARK::SaveToXML(CString &dest,const char *node)
{
	if(node)
	{
		dest+="<";dest+=node;dest+=">";
	}

	InsertXMLNodeValue(dest,"PLANID",PLANID);

	InsertXMLNodeValue(dest,"TYPE",m_nTDType);	
	InsertXMLNodeTime(dest,"STTM",m_tStartTime);
	InsertXMLNodeTime(dest,"EDTM",m_tEndTime);

	InsertXMLNodeValue(dest,"STST",STST);
	InsertXMLNodeValue(dest,"EDST",EDST);

	InsertXMLNodeValue(dest,"STTB", STTB);
	InsertXMLNodeValue(dest,"EDTB", EDTB);

	InsertXMLNodeValue(dest,"STKM",STKM);
	InsertXMLNodeValue(dest,"STM",STM);
	InsertXMLNodeValue(dest,"EDKM",ETKM);
	InsertXMLNodeValue(dest,"EDM",ETM);

	InsertXMLNodeValue(dest,"LIM",LIM);
	InsertXMLNodeValue(dest,"OTHER",OTHER);
	InsertXMLNodeValue(dest,"DIR",DIR);
	
	InsertXMLNodeValue(dest,"PLANCD",PLANCD);
	InsertXMLNodeValue(dest,"CONTENT",CONTENT);
	InsertXMLNodeValue(dest,"REASON",REASON);
	  	
	if(node)
	{
		dest+="</";dest+=node;dest+=">";
	}
	return TRUE;
}

const char *CTD_MPLANMARK::GetListTitle()
{
	static char tmp[256];
	const char *ptmpstr=GetCTCValueDispString(CTC_DATA_MARK_TYPE,m_nCTCType,NULL);
	if(!ptmpstr) 
		ptmpstr="未知类型";
	sprintf(tmp,"%s,%s,%s",STST,EDST,ptmpstr);
	return tmp;
}

BOOL  CTD_MPLANMARK::GenListInfoXML(CString &dest,const char *node)
{
	CString title;
	title=GetListTitle();
	if(node)
	{
		dest+="<";dest+=node;dest+=">";
	}
	InsertXMLNodeValue(dest,"PLANID",PLANID);
	InsertXMLNodeValue(dest,"TITLE",title);
	InsertXMLNodeTime(dest,"STTM",m_tStartTime);
	InsertXMLNodeTime(dest,"EDTM",m_tEndTime);
	if(node)
	{
		dest+="</";dest+=node;dest+=">";
	}
	return TRUE;
}
BOOL  CTD_MPLANMARK::GenMark(CTG_SingleMark &mark) const 
{
	if(m_nCTCType<0)
		return FALSE;
	STR_CPY(mark.m_data.shift_string,PLANID);
	mark.m_data.start_time=  m_tStartTime;
	mark.m_data.end_time=  m_tEndTime;
	mark.m_data.type = m_nCTCType;
	mark.m_data.position=STKM;
	mark.m_data.s_meter =STM;
	mark.m_data.e_kilo =ETKM;
	mark.m_data.e_meter =ETM;
	mark.m_data.shift_index=m_nGMID;
	
	// 站号、站序
	if(TG_IS_ABS_MARK(m_nCTCType))  // 区间
	{
		mark.m_data.station1=CovertStationNameToNo(STST);
		mark.m_data.station2=CovertStationNameToNo(EDST);

		mark.m_data.station=STTB;
		mark.m_data.index=EDTB;
	}
	else
	{
		mark.m_data.station1=CovertStationNameToNo(STST);

		mark.m_data.index=STTB;
	}

	// 符号方向
	mark.m_data.line=255;
	if(m_nCTCType == TG_ABS_INHIBIT||m_nCTCType == TG_ABS_SLOW)
	{
		if(-1 == DIR)
			mark.m_data.line=2; // 上下行
	}
	else if((m_nCTCType == TG_SECTION_ELEC_FIX) || (m_nCTCType == TG_SECTION_ELEC_FIX_LANZHOU))
	{
		if(DIR & 0x100) //包含上一站
			mark.m_data.slow_time|=0x1;
		if(DIR & 0x200) //包含下一站
			mark.m_data.slow_time|=0x2;
		
		mark.m_data.line=(DIR&0xFF);
	}
	else
	{
		mark.m_data.line=0;
	}

	// 区间号
	if(m_nCTCType == TG_ABS_INHIBIT||m_nCTCType == TG_ABS_SLOW||m_nCTCType == TG_SECTION_ELEC_FIX||m_nCTCType == TG_SECTION_ELEC_FIX_LANZHOU) 
	{
		CStringArray sub_absnum;
		ConvertToSubStringArray(CONTENT,',',sub_absnum);
		mark.m_data.ele_count=sub_absnum.GetCount();
		for(int i=0;i<mark.m_data.ele_count&&i<512;i++)
		{
			mark.m_data.ele_number[i]=atoi(sub_absnum.GetAt(i)); 
		}
	}
	else
	{
		mark.m_data.ele_count=0;
		mark.m_data.ele_number[0]=0;
	}
	
	mark.m_data.flag=LIM;
	mark.m_data.direction=255;
	STR_CPY2(mark.m_data.text,REASON);

	if(TG_IS_ABS_MARK(m_nCTCType))
	{
		if(mark.m_data.station1<=0 || mark.m_data.station2<=0 )
			return FALSE;
	}
	else
	{
		if(mark.m_data.station1<=0)
			return FALSE;
	}
	return TRUE;
}

CTD_MPlanMarkList::CTD_MPlanMarkList()
{
	m_nCount=0;
	mark_con_id=0;
}

BOOL CTD_MPlanMarkList::LoadMarkFromTDXML(const char *xml,const char *node)
{
	m_nCount=0;
	LOAD_XML(xml,node,pRoot)
	
	MSXML::IXMLDOMNodePtr pNodeMark;
	MSXML::IXMLDOMNodeListPtr pNodeList = pRoot->selectNodes("RECORD");
	if(pNodeList==NULL)
		return FALSE;

	int rec_count = pNodeList->Getlength();
	for(int i=0;i<rec_count&&i<MAX_MPLAN_MARK_NUM;i++)
	{
		pNodeMark = pNodeList->Getitem(i);
		if(pNodeMark==NULL)
			continue;

		if(!GenMarkFromXML(pNodeMark, &m_list[m_nCount]))
			continue;

		// 转化类型
		int tmp_mark_type=m_list[m_nCount].m_nTDType;
		m_list[m_nCount].m_nCTCType=-1;
		for(int i=0;i<MARK_TYPE_MAP_NUM;i++)
		{
			if(g_mark_type_map[i].opms_type==tmp_mark_type)
			{
				m_list[m_nCount].m_nCTCType=g_mark_type_map[i].ctc_type;
				GpcLogTrainMessage(0, 0, "LoadMarkFromTDXML TD发来施工类型:%d, 转化为CTC中施工类型%d", 
									tmp_mark_type, m_list[m_nCount].m_nCTCType);
				break;
			}
		}

		if(m_list[m_nCount].m_nCTCType==-1)
		{
			GpcLogTrainMessage(0, 0, "LoadMarkFromTDXML TD发来施工类型:%d, 但是无法转化为CTC中施工类型, 丢弃不处理", 
							tmp_mark_type);
			//continue;
		}

		m_nCount++;
	}

	return TRUE;
}

BOOL CTD_MPlanMarkList::LoadMarkFromCADXML(const char *xml,const char *node, int conid)
{
	m_nCount=0;
	LOAD_XML(xml,node,pRoot)
	
	GET_NODE_INTVALUE(mark_con_id,pRoot,"CONID")

	if(mark_con_id != conid)
	{
		GpcLogTrainMessage(0, 0, "LoadMarkFromCADXML 本区段:%d, 但是CAD发来施工符号的XML中区段号为:%d,非本台不处理", 
							mark_con_id, conid);
		return FALSE;
	}

	MSXML::IXMLDOMNodePtr pNodeMark;
	MSXML::IXMLDOMNodeListPtr pNodeList = pRoot->selectNodes("RECORD");
	if(pNodeList==NULL)
		return FALSE;

	int rec_count = pNodeList->Getlength();
	for(int i=0;i<rec_count&&i<MAX_MPLAN_MARK_NUM;i++)
	{
		pNodeMark = pNodeList->Getitem(i);
		if(pNodeMark==NULL)
			continue;

		if(!GenMarkFromXML(pNodeMark, &m_list[m_nCount]))
			continue;

		// 转化类型
		int tmp_mark_type=m_list[m_nCount].m_nTDType;
		m_list[m_nCount].m_nCTCType=-1;
		for(int i=0;i<MARK_TYPE_MAP_NUM;i++)
		{
			if(g_mark_type_map[i].opms_type==tmp_mark_type)
			{
				m_list[m_nCount].m_nCTCType=g_mark_type_map[i].ctc_type;
				GpcLogTrainMessage(0, 0, "LoadMarkFromCADXML TD发来施工类型:%d, 转化为CTC中施工类型%d", 
									tmp_mark_type, m_list[m_nCount].m_nCTCType);
				break;
			}
		}

		if(m_list[m_nCount].m_nCTCType==-1)
		{
			GpcLogTrainMessage(0, 0, "LoadMarkFromCADXML TD发来施工类型:%d, 但是无法转化为CTC中施工类型, 丢弃不处理", 
							tmp_mark_type);
			continue;
		}

		m_nCount++;
	}

	return TRUE;
}

BOOL CTD_MPlanMarkList::SaveXMLToString(CString& dest, const char* node)
{
	if(node)
	{
		dest+="<";dest+=node;dest+=">";
	}
	InsertXMLNodeValue(dest,"DISID",strDisID);
	InsertXMLNodeValue(dest,"CONID",mark_con_id);
	for(int i=0; i<m_nCount&&i<MAX_MPLAN_MARK_NUM; i++)
	{		
		m_list[i].SaveToXML(dest, "RECORD");
	}
	  	
	if(node)
	{
		dest+="</";dest+=node;dest+=">";
	}

	return TRUE;
}

CTD_MPlanDCMDList::CTD_MPlanDCMDList()
{
	m_nCount=0;
	strDisID="";
	m_nSectionID=0;
}

BOOL CTD_MPlanDCMDList::LoadDcmdFromTDXML(const char *xml,const char *node, int conid)
{
	LOAD_XML(xml,node,pRoot)
	
	m_nCount=0;
	MSXML::IXMLDOMNodePtr pNodeDcmd;
	MSXML::IXMLDOMNodeListPtr pNodeList = pRoot->selectNodes("RECORD");
	if(pNodeList==NULL)
		return FALSE;

	int rec_count = pNodeList->Getlength();
	for(int i=0;i<rec_count&&i<MAX_MPLAN_DCMD_NUM;i++)
	{
		pNodeDcmd = pNodeList->Getitem(i);
		if(pNodeDcmd==NULL)
			continue;

		if(!GenDcmdFromXML(pNodeDcmd, conid, &m_list[m_nCount]))
			continue;

		m_nCount++;
	}

	return TRUE;
}

BOOL CTD_MPlanDCMDList::LoadDcmdFromCADXML(const char *xml,const char *node, int conid)
{
	LOAD_XML(xml,node,pRoot)
	GET_NODE_STRVALUE(strDisID,pRoot,"DISID")
	GET_NODE_INTVALUE(m_nSectionID,pRoot,"CONID")

	if(m_nSectionID != conid)
	{
		GpcLogTrainMessage(0, 0, "LoadDcmdFromCADXML 本区段:%d, 但是CAD发来的施工命令XML中区段号为:%d,非本台不处理", 
							m_nSectionID, conid);
		return FALSE;
	}

	m_nCount=0;
	MSXML::IXMLDOMNodePtr pNodeDcmd;
	MSXML::IXMLDOMNodeListPtr pNodeList = pRoot->selectNodes("RECORD");
	if(pNodeList==NULL)
		return FALSE;

	int rec_count = pNodeList->Getlength();
	for(int i=0;i<rec_count&&i<MAX_MPLAN_DCMD_NUM;i++)
	{
		pNodeDcmd = pNodeList->Getitem(i);
		if(pNodeDcmd==NULL)
			continue;

		if(!GenDcmdFromXML(pNodeDcmd, conid, &m_list[m_nCount]))
			continue;

		m_nCount++;
	}

	return TRUE;
}

BOOL CTD_MPlanDCMDList::SaveDCmdsToXML(CString&dest, const char* node)
{
	if(node)
	{
		dest+="<";dest+=node;dest+=">";
	}
	
	InsertXMLNodeValue(dest,"DISID",strDisID);
	InsertXMLNodeValue(dest,"CONID",m_nSectionID);
	for(int i=0; i<m_nCount&&i<MAX_MPLAN_DCMD_NUM; i++)
	{		
		m_list[i].SaveDCmdToXML(dest, "RECORD");
	}
	  	
	if(node)
	{
		dest+="</";dest+=node;dest+=">";
	}

	return TRUE;
}

CTD_MPLAN_SG_WX::CTD_MPLAN_SG_WX()
{
	m_con_id=0;
	m_count=0;
	m_flag=0;
}

BOOL CTD_MPLAN_SG_WX::GenMplan_SG_WX(const char *xml,const char *node)
{
	LOAD_XML(xml,node,pNodeRoot)
	GET_NODE_STRVALUE(DISID, pNodeRoot, "DISID")
	GET_NODE_INTVALUE(m_con_id, pNodeRoot, "CONID")

	GET_NODE_STRVALUE(PLANID,pNodeRoot,"PLANID")
	GET_NODE_STRVALUE(TM,pNodeRoot,"TM")

	GET_NODE_INTVALUE(m_count,pNodeRoot,"COUNT")
	GET_NODE_INTVALUE(m_flag,pNodeRoot,"FLAG")
	GET_NODE_STRVALUE(MID,pNodeRoot,"MID")
	GET_NODE_STRVALUE(WORKPOS,pNodeRoot,"WORKPOS")
	GET_NODE_STRVALUE(LINENM,pNodeRoot,"LINENM")
	GET_NODE_STRVALUE(DIR,pNodeRoot,"DIR")
	GET_NODE_STRVALUE(WORKKM,pNodeRoot,"WORKKM")

	GET_NODE_STRVALUE(STTM,pNodeRoot,"STTM")
	GET_NODE_STRVALUE(EDTM,pNodeRoot,"EDTM")
	
	GET_NODE_STRVALUE(WORKTP,pNodeRoot,"WORKTP")
	GET_NODE_STRVALUE(PJNM,pNodeRoot,"PJNM")
	GET_NODE_STRVALUE(UNITNM,pNodeRoot,"UNITNM")
	GET_NODE_STRVALUE(LEADER,pNodeRoot,"LEADER")

	GET_NODE_STRVALUE(RANK,pNodeRoot,"RANK")
	GET_NODE_STRVALUE(TELID,pNodeRoot,"TELID")
	
	GET_NODE_STRVALUE(WKCON,pNodeRoot,"WKCON")
	GET_NODE_STRVALUE(SPLIM,pNodeRoot,"SPLIM")
	GET_NODE_STRVALUE(LIMINFO,pNodeRoot,"LIMINFO")

	return TRUE;
}

BOOL CTD_MPLAN_SG_WX::LoadSG_WXFromTDXML(const char *xml,const char *node)
{
	if(!GenMplan_SG_WX(xml, node))
		return FALSE;

	return TRUE;
}

BOOL CTD_MPLAN_SG_WX::LoadSG_WXFromCADXML(const char *xml,const char *node, int conid)
{
	if(!GenMplan_SG_WX(xml, node))
		return FALSE;

	if(m_con_id != conid)
	{
		GpcLogTrainMessage(0, 0, "施工维修 CAD发来消息台号:%d, 但本台号:%d, 非本台不处理", m_con_id, conid);
		return FALSE;
	}
	return TRUE;
}

BOOL CTD_MPLAN_SG_WX::GenViewInfoXML(CString &dest,const char *node)
{
	CString title;
	title.Format("月计划:%s       电报号:%s\r\n  地点: %s,     线路:%s  %s\r\n  类型:%s,    项目:%s,    等级:%s",
		MID,DBH,WORKPOS,LINENM,WORKKM,WORKTP,PJNM,RANK);
	CString unitstr;
	unitstr.Format("单位名称:%s\r\n施工负责人:%s",UNITNM,LEADER);

	if(node)
	{
		dest+="<";dest+=node;dest+=">";
	}
	InsertXMLNodeValue(dest,"TITLE",title);
	InsertXMLNodeValue(dest,"UNITNM",unitstr);
	InsertXMLNodeValue(dest,"WKCON",WKCON);
	InsertXMLNodeValue(dest,"SPLIM",SPLIM);
	InsertXMLNodeValue(dest,"LIMINFO",LIMINFO);
	if(node)
	{
		dest+="</";dest+=node;dest+=">";
	}
	return TRUE;
}

BOOL CTD_MPLAN_SG_WX::SaveSG_WXToXML(CString &dest,const char *node)
{
	if(node)
	{
		dest+="<";dest+=node;dest+=">";
	}

	InsertXMLNodeValue(dest,"DISID",DISID);
	InsertXMLNodeValue(dest,"CONID",m_con_id);
	InsertXMLNodeValue(dest,"PLANID",PLANID);
	InsertXMLNodeValue(dest,"TM",TM);

	InsertXMLNodeValue(dest,"COUNT",m_count);

	if(m_count>0)
	{
		InsertXMLNodeValue(dest,"FLAG",m_flag);

		InsertXMLNodeValue(dest,"MID",MID);
		InsertXMLNodeValue(dest,"WORKPOS",WORKPOS);
		InsertXMLNodeValue(dest,"LINENM",LINENM);
		
		InsertXMLNodeValue(dest,"DIR",DIR);
		InsertXMLNodeValue(dest,"WORKKM",WORKKM);
		InsertXMLNodeValue(dest,"STTM",STTM);
		InsertXMLNodeValue(dest,"EDTM",EDTM);

		InsertXMLNodeValue(dest,"WORKTP",WORKTP);
		InsertXMLNodeValue(dest,"PJNM",PJNM);
		InsertXMLNodeValue(dest,"UNITNM",UNITNM);
		InsertXMLNodeValue(dest,"LEADER",LEADER);
		InsertXMLNodeValue(dest,"RANK",RANK);
		InsertXMLNodeValue(dest,"TELID",TELID);
		InsertXMLNodeValue(dest,"WKCON",WKCON);
		InsertXMLNodeValue(dest,"SPLIM",SPLIM);
		InsertXMLNodeValue(dest,"LIMINFO",LIMINFO);
	}
	if(node)
	{
		dest+="</";dest+=node;dest+=">";
	}
	return TRUE;
}

CMPLAN_WKDES::CMPLAN_WKDES()
{
	m_nSectionID=0;
}

BOOL CMPLAN_WKDES::LoadWKDESFromTDXML(const char *xml,const char *node)
{
	LOAD_XML(xml,node,pNodeRoot)

	GET_NODE_STRVALUE(strDisID, pNodeRoot, "DISID")
	GET_NODE_STRVALUE(PLANID,pNodeRoot,"PLANID")
	GET_NODE_STRVALUE(CMDID,pNodeRoot,"CMDID")
	GET_NODE_STRVALUE(CONTENT,pNodeRoot,"CONTENT")

	return TRUE;
}

BOOL CMPLAN_WKDES::LoadWKDESFromCADXML(const char *xml,const char *node, int conid)
{
	LOAD_XML(xml,node,pNodeRoot)

	GET_NODE_INTVALUE(m_nSectionID, pNodeRoot, "CONID")
	GET_NODE_STRVALUE(strDisID, pNodeRoot, "DISID")

	GET_NODE_STRVALUE(PLANID,pNodeRoot,"PLANID")
	GET_NODE_STRVALUE(CMDID,pNodeRoot,"CMDID")
	GET_NODE_STRVALUE(CONTENT,pNodeRoot,"CONTENT")

	if(m_nSectionID != conid)
	{
		GpcLogTrainMessage(0, 0, "施工揭示 CAD发来消息台号:%d, 但本台号:%d, 非本台不处理", m_nSectionID, conid);
		return FALSE;
	}
	return TRUE;
}

BOOL CMPLAN_WKDES::SaveWKDESToXML(CString &dest,const char *node)
{
	if(node)
	{
		dest+="<";dest+=node;dest+=">";
	}

	InsertXMLNodeValue(dest,"CONID",m_nSectionID);
	InsertXMLNodeValue(dest,"DISID",strDisID);

	InsertXMLNodeValue(dest,"PLANID",PLANID);
	InsertXMLNodeValue(dest,"CMDID",CMDID);
	InsertXMLNodeValue(dest,"CONTENT",CONTENT);

	if(node)
	{
		dest+="</";dest+=node;dest+=">";
	}
	return TRUE;
}

CTD_MPLANDCMD::CTD_MPLANDCMD()
{
	m_nSectionID=0;
}

BOOL GenDcmdFromXML(MSXML::IXMLDOMNodePtr pNodeRoot, int conid, CTD_MPLANDCMD* dcmd)
{
	dcmd->m_nSectionID=conid;
	GET_NODE_STRVALUE(dcmd->PLANID,pNodeRoot,"PLANID")
	GET_NODE_STRVALUE(dcmd->CMDINDEX,pNodeRoot,"CMDINDEX")
	GET_NODE_STRVALUE(dcmd->CMDTP,pNodeRoot,"CMDTP")

	GET_NODE_STRVALUE(dcmd->RECVUNIT,pNodeRoot,"RECVUNIT")
	GET_NODE_STRVALUE(dcmd->TOPIC,pNodeRoot,"TOPIC")
	GET_NODE_STRVALUE(dcmd->CONTENT,pNodeRoot,"CONTENT")
	GET_NODE_STRVALUE(dcmd->PRNM,pNodeRoot,"PRNM")
	
	GET_NODE_STRVALUE(dcmd->SUBTP,pNodeRoot,"SUBTYPE")
	GET_NODE_STRVALUE(dcmd->RECVER,pNodeRoot,"RECVER")

	return TRUE;
}

BOOL CTD_MPLANDCMD::SaveDCmdToXML(CString &xml,const char *node)
{
	CString dest="";
	if(node)
	{
		dest+="<";dest+=node;dest+=">";
	}

	InsertXMLNodeValue(dest,"PLANID",PLANID);
	InsertXMLNodeValue(dest,"CMDINDEX",CMDINDEX);
	InsertXMLNodeValue(dest,"CMDTP",CMDTP);

	InsertXMLNodeValue(dest,"RECVUNIT",RECVUNIT);
	InsertXMLNodeValue(dest,"TOPIC",TOPIC);
	InsertXMLNodeValue(dest,"CONTENT",CONTENT);
	InsertXMLNodeValue(dest,"PRNM",PRNM);

	InsertXMLNodeValue(dest,"SUBTYPE",SUBTP);
	InsertXMLNodeValue(dest,"RECVER",RECVER);

	if(node)
	{
		dest+="</";dest+=node;dest+=">";
	}

	xml += dest;

	return TRUE;
}

BOOL CTD_MPLANDCMD::SaveToXML(CString &dest,const char *node, long create_time, CString dcmdtype, CString dcmdsubtype)
{
	if(create_time <= 0)
	{
		CTime ctm=CTime::GetCurrentTime();
		create_time = ctm.GetTime();
	}

	if(node)
	{
		dest+="<";dest+=node;dest+=">";
	}
	InsertXMLNodeValue(dest,"JHH",PLANID);
	//InsertXMLNodeValue(dest,"DCMD_ID",GUID);
	InsertXMLNodeValue(dest,"DCMD_ID",CMDINDEX);
	InsertXMLNodeValue(dest,"DCMD_STYLE_TITLE",TOPIC);

	// 按照老蒋调度命令要求增加配置
	long nDcmdPrefixNum = GetTGFunctionConfig()->GetDcmdTempleteNo(dcmdtype, dcmdsubtype);
	GpcLogTrainMessage(0, 0, "调度命令类型转换:dcmdtype:%s dcmdsubtype:%s-->templete_no: %d", 
		dcmdtype, dcmdsubtype, nDcmdPrefixNum);
	if(nDcmdPrefixNum >= 0)
	{
		InsertXMLNodeValue(dest,"CMD_DEST",nDcmdPrefixNum);
	}
	else
	{
		InsertXMLNodeValue(dest,"CMD_DEST",0);
	}
	
	// 上海局
	InsertXMLNodeValue(dest,"CMDTP", dcmdtype);
	InsertXMLNodeValue(dest,"SUBTYPE", dcmdsubtype);

	InsertXMLNodeValue(dest,"DCMD_NUM","00000000");

	InsertXMLNodeTime(dest,"CREATE_TIME", create_time);
	InsertXMLNodeTime(dest,"SEND_TIME", create_time); 
	InsertXMLNodeTime(dest,"EDIT_TIME", create_time); 
	InsertXMLNodeTime(dest,"AUTHOR_TIME", create_time);
	InsertXMLNodeValue(dest,"SENDER_TYPE",1);
	InsertXMLNodeValue(dest,"SENDER_ID",m_nSectionID);
	InsertXMLNodeValue(dest,"STATUS",0);  // 待发箱
	InsertXMLNodeValue(dest,"NEED_AUTHOR",0);  
	InsertXMLNodeValue(dest,"SEND_ENTITY",GetLocalEntityID());  
	InsertXMLNodeValue(dest,"CONTENT",CONTENT);

	if(node)
	{
		dest+="</";dest+=node;dest+=">";
	}
	return TRUE;
}



BOOL CTD_PlanInform::LoadFromXML(const char *xml,const char *node) //获取计划下达信息
{
	if(!stricmp(node,TOKEN_SS_PLAN_INFORM))  // 日班计划通知
	{
		inform_type=SHIFT_PLAN_INFORM;
		return TRUE;
	}
	LOAD_XML(xml,node,pNodeRoot)
	if(stricmp(node,TOKEN_22_ACK_STN_PLAN) && stricmp(node,TOKEN_24_ACK_FTN_PLAN))
	{
		GET_NODE_INTVALUE(COUNT,pNodeRoot,"COUNT")
		if(COUNT>MAX_NFORM_TRAIN_NUM)
		{
			AfxMessageBox("计划中COUNT数目超过128");
			COUNT=MAX_NFORM_TRAIN_NUM;
		}
	}
	if(!stricmp(node,TOKEN_KD_PLAN_INFORM))
	{
		inform_type=KD_PLAN_INFORM;
		MSXML::IXMLDOMNodeListPtr pList = pNodeRoot->selectNodes( "RECORD" );
		int rec_count = pList->Getlength();
		if(rec_count!=COUNT)
		{
			AfxMessageBox("没有获取足够的记录,COUNT和实际数目不一致");
			COUNT=rec_count;
		}
		for(int i=0;i<rec_count;i++)
		{
			GET_NODE_INTVALUE(trains[i].GTID,pList->Getitem(i),"GTID");
			GET_NODE_INTVALUE(trains[i].KDID,pList->Getitem(i),"KDID");
			GET_NODE_INTVALUE(trains[i].OPER,pList->Getitem(i),"OPER");
			GET_NODE_STRVALUE(trains[i].TNO,pList->Getitem(i),"TNO");
			trains[i].LocalAdded=FALSE;
		}
		STTM=0;
		ENDTM=0;
	}
	else if(!stricmp(node,TOKEN_JH_PLAN_INFORM))
	{
		inform_type=JH_PLAN_INFORM;
		GET_NODE_INTVALUE(JHID,pNodeRoot,"JHID")
		GET_NODE_STRVALUE(JHTNM,pNodeRoot,"JHTNM")
		CString GTIDS;
		GET_NODE_STRVALUE(GTIDS,pNodeRoot,"GTID")

		GET_NODE_TIMEVALUE(STTM,pNodeRoot,"STTM")
		GET_NODE_TIMEVALUE(ENDTM,pNodeRoot,"ENDTM")
		if(!GTIDS.IsEmpty())
		{
			CStringArray array;
			int cnt=ConvertToSubStringArray(GTIDS,',',array);
			for(int i=0;i<cnt;i++)
			{
				trains[i].GTID=atoi(array[i]);
				trains[i].KDID=0;
				trains[i].OPER=OPER_UPDATE_TRAIN;
				trains[i].TNO="";
				trains[i].LocalAdded=FALSE;
			}
			if(cnt!=COUNT)
			{
				AfxMessageBox("没有获取足够的记录,COUNT和实际数目不一致");
				COUNT=cnt;
			}
		}
	}
	else if(!stricmp(node,TOKEN_22_ACK_STN_PLAN))
	{
		inform_type=STN_PLAN_INFORM;
		GET_NODE_STRVALUE(MSGID,pNodeRoot,"MSGID")
		GET_NODE_INTVALUE(JHID,pNodeRoot,"SRCCODE")
		GET_NODE_STRVALUE(JHTNM,pNodeRoot,"JHTNM")

		GET_NODE_TIMEVALUE(STTM,pNodeRoot,"LSTTM")
		GET_NODE_TIMEVALUE(ENDTM,pNodeRoot,"LEDTM")
		CString GTID;
		GET_NODE_STRVALUE(GTID,pNodeRoot,"GTID")
		int HALT=0;
		GET_NODE_INTVALUE(HALT,pNodeRoot,"HALT")
		CString TNO;
		GET_NODE_STRVALUE(TNO,pNodeRoot,"TNO")

		COUNT =1;
		if(!GTID.IsEmpty())
		{
			trains[0].GTID=atoi(GTID);
			trains[0].KDID=0;
			if (HALT==0)
				trains[0].OPER=OPER_UPDATE_TRAIN;
			else
				trains[0].OPER=OPER_DELETE_TRAIN;
			trains[0].TNO=TNO;
			trains[0].LocalAdded=FALSE;
		}
	}
	else if(!stricmp(node,TOKEN_24_ACK_FTN_PLAN))
	{
		inform_type=FTN_PLAN_INFORM;
		GET_NODE_STRVALUE(MSGID,pNodeRoot,"MSGID")
		GET_NODE_INTVALUE(JHID,pNodeRoot,"SRCCODE")
		GET_NODE_STRVALUE(JHTNM,pNodeRoot,"JHTNM")

		GET_NODE_TIMEVALUE(STTM,pNodeRoot,"STTM")
		GET_NODE_TIMEVALUE(ENDTM,pNodeRoot,"EDTM")
		GET_NODE_INTVALUE(COUNT,pNodeRoot,"COUNT")
		int cnt=0;

		MSXML::IXMLDOMNodeListPtr pTrainList = pNodeRoot->selectNodes("ST_TRAIN");
		if(COUNT!=0)
		{
			cnt=pTrainList->Getlength();
			for(int i=0;i<cnt;i++)
			{
				if (pTrainList->Getitem(i))
				{
					long gtid=0;
					CString trainNo;
					GET_NODE_INTVALUE(gtid, pTrainList->Getitem(i), "GTID")
					GET_NODE_STRVALUE(trainNo, pTrainList->Getitem(i), "TNO")

					int HALT=0;
					GET_NODE_INTVALUE(HALT,pTrainList->Getitem(i),"HALT")

					trains[i].GTID=gtid;
					trains[i].KDID=0;
					if (HALT==0)
						trains[i].OPER=OPER_UPDATE_TRAIN;
					else
						trains[i].OPER=OPER_DELETE_TRAIN;
					trains[i].TNO=trainNo;
					trains[i].LocalAdded=FALSE;
				}
			}
		}
		//COUNT = cnt;
	}
	else if(!stricmp(node,TOKEN_25_AD_PLAN_INFORM))
	{
		inform_type=AD_PLAN_INFO;
		GET_NODE_INTVALUE(COUNT,pNodeRoot,"COUNT")
		int cnt=0;

		MSXML::IXMLDOMNodeListPtr pTrainList = pNodeRoot->selectNodes("ST_TRAIN");
		if(COUNT!=0)
		{
			cnt=pTrainList->Getlength();
			for(int i=0;i<cnt;i++)
			{
				if (pTrainList->Getitem(i))
				{
					long gtid=0;
					CString trainNo;
					GET_NODE_INTVALUE(gtid, pTrainList->Getitem(i), "GTID")
					GET_NODE_STRVALUE(trainNo, pTrainList->Getitem(i), "TNO")

					int OPER=0;
					GET_NODE_INTVALUE(OPER,pTrainList->Getitem(i),"OPER")

					int YLFG, LYLFG;
					GET_NODE_INTVALUE(YLFG,pTrainList->Getitem(i),"YLFG")
					GET_NODE_INTVALUE(LYLFG,pTrainList->Getitem(i),"LYLFG")

					trains[i].GTID=gtid;
					trains[i].KDID=0;
					if (OPER==0x01 || OPER==0x02)
						trains[i].OPER=OPER_UPDATE_TRAIN;
					else
						trains[i].OPER=OPER_DELETE_TRAIN;
					trains[i].TNO=trainNo;
					trains[i].LocalAdded=FALSE;
				}
			}
		}
	}

	return TRUE;
}


BOOL  CTD_Ack::LoadFromXML(const char *xml,const char *node) //获取计划下达信息
{
	LOAD_XML(xml,node,pNodeRoot)
	GET_NODE_INTVALUE(ACKL,pNodeRoot,"ACKFL")
	return TRUE;
}

// 施工符号中间结果
CMPLAN_RSINFO::CMPLAN_RSINFO()
{
	TM=0;
	FLAG=0;
	m_nSectionID=0;
}

BOOL CMPLAN_RSINFO::SaveRSINFOToXML(CString& xml, const char* node)
{
	CString dest="";
	if(node)
	{
		dest+="<";dest+=node;dest+=">";
	}

	InsertXMLNodeValue(dest,"DISID",strDisID);
	InsertXMLNodeValue(dest,"CONID",m_nSectionID);	
	InsertXMLNodeTime(dest,"TM",TM);
	InsertXMLNodeValue(dest,"PLANID",PLANID);
	

	InsertXMLNodeValue(dest,"TRNINFO",TRNINFO);
	InsertXMLNodeValue(dest,"FLAG",FLAG);
	InsertXMLNodeValue(dest,"PNM",PNM);
	InsertXMLNodeValue(dest,"WKDES",WKDES);

	if(node)
	{
		dest+="</";dest+=node;dest+=">";
	}

	xml+=dest;

	return TRUE;
}

CTD_MPlanMidList::CTD_MPlanMidList()
{ 
	m_nCount=0;
}

BOOL CTD_MPlanMidList::LoadRSINFOFromTDXML(const char *xml,const char *node, int con_id) 
{
	LOAD_XML(xml,node,pNodeRoot)
	
	char dispid[10];
	if(GetTGFunctionConfig()->GetTmisSectionID(con_id,dispid)<0)
	{
		GpcLogTrainMessage(0, 0, "无法获取区段 %d 的调度台简码,检查tg_logic.cfg中配置[TD_SECTION]", con_id);
	}

	m_nCount=0;

	MSXML::IXMLDOMNodeListPtr pList = pNodeRoot->selectNodes( "RECORD" );
	int rec_count = pList->Getlength();
	for(int i=0;i<rec_count&&i<MAX_MPLAN_NODE_NUM;i++)
	{
		m_list[m_nCount].m_nSectionID=con_id;
		GET_NODE_STRVALUE(m_list[m_nCount].strDisID,pList->Getitem(i),"DISID")
		GET_NODE_TIMEVALUE(m_list[m_nCount].TM,pList->Getitem(i),"TM")
			
		GET_NODE_STRVALUE(m_list[m_nCount].PLANID,pList->Getitem(i),"PLANID")
		GET_NODE_STRVALUE(m_list[m_nCount].PNM,pList->Getitem(i),"PNM")
		GET_NODE_STRVALUE(m_list[m_nCount].TRNINFO,pList->Getitem(i),"TRNINFO")
		GET_NODE_INTVALUE(m_list[m_nCount].FLAG,pList->Getitem(i),"FLAG")
		GET_NODE_STRVALUE(m_list[m_nCount].WKDES,pList->Getitem(i),"WKDES")

		if(m_list[m_nCount].strDisID != dispid)
		{
			GpcLogTrainMessage(0, 0, "本区段:%d 调度台简码为:%s, 但是TD发来的XML中调度台简码为:%s,非本台不处理", 
							con_id, dispid, m_list[m_nCount].strDisID);
			continue;
		}
		m_nCount++;
	}

	return TRUE;
}

BOOL CTD_MPlanMidList::LoadRSINFOFromCADXML(const char *xml,const char *node, int con_id) 
{
	LOAD_XML(xml,node,pNodeRoot)
	
	char dispid[10];
	if(GetTGFunctionConfig()->GetTmisSectionID(con_id,dispid)<0)
	{
		GpcLogTrainMessage(0, 0, "无法获取区段 %d 的调度台简码,检查tg_logic.cfg中配置[TD_SECTION]", con_id);
	}

	m_nCount=0;

	MSXML::IXMLDOMNodeListPtr pList = pNodeRoot->selectNodes( "RECORD" );
	int rec_count = pList->Getlength();
	for(int i=0;i<rec_count&&i<MAX_MPLAN_NODE_NUM;i++)
	{
		m_list[m_nCount].strDisID=dispid;
		GET_NODE_INTVALUE(m_list[m_nCount].m_nSectionID,pList->Getitem(i),"CONID")
		GET_NODE_TIMEVALUE(m_list[m_nCount].TM,pList->Getitem(i),"TM")
		
		GET_NODE_STRVALUE(m_list[m_nCount].PLANID,pList->Getitem(i),"PLANID")
		GET_NODE_STRVALUE(m_list[m_nCount].PNM,pList->Getitem(i),"PNM")
		GET_NODE_STRVALUE(m_list[m_nCount].TRNINFO,pList->Getitem(i),"TRNINFO")
		GET_NODE_INTVALUE(m_list[m_nCount].FLAG,pList->Getitem(i),"FLAG")
		GET_NODE_STRVALUE(m_list[m_nCount].WKDES,pList->Getitem(i),"WKDES")

		if(m_list[m_nCount].m_nSectionID != con_id)
		{
			GpcLogTrainMessage(0, 0, "本区段:%d 调度台简码为:%s, 但是CAD发来的XML中区段号为:%d,非本台不处理", 
							con_id, dispid, m_list[m_nCount].m_nSectionID);
			continue;
		}
		m_nCount++;
	}

	return TRUE;
}

BOOL  CTD_MPlanMidList::SaveRSINFOToXML(CString&dest, const char* node)
{
	if(node)
	{
		dest+="<";dest+=node;dest+=">";
	}

	for(int i=0;i<m_nCount;i++)
	{
		m_list[i].SaveRSINFOToXML(dest, "RECORD");
	}
	
	if(node)
	{
		dest+="</";dest+=node;dest+=">";
	}

	return TRUE;
}

// 施工结果兑现
CMPlanListState::CMPlanListState()
{
	m_nCount=0;
}

BOOL CMPlanListState::LoadMPlanStateFromCADXML(const char *xml,const char *node, int con_id) 
{
	LOAD_XML(xml,node,pNodeRoot)
	
	m_nCount = 0;
	
	MSXML::IXMLDOMNodeListPtr pList = pNodeRoot->selectNodes( "RECORD" );
	int rec_count = pList->Getlength();
	for(int i=0;i<rec_count&&i<MAX_MPLAN_NODE_NUM;i++)
	{
		GET_NODE_INTVALUE(m_list[m_nCount].m_nSectionID,pList->Getitem(i),"CONID")
		GET_NODE_STRVALUE(m_list[m_nCount].DISID,pList->Getitem(i),"DISID")

		GET_NODE_STRVALUE(m_list[m_nCount].PLANID,pList->Getitem(i),"PLANID")
		GET_NODE_INTVALUE(m_list[m_nCount].m_nStatus,pList->Getitem(i),"ISCANCEL")
		GET_NODE_STRVALUE(m_list[m_nCount].STTM,pList->Getitem(i),"STTM")
		GET_NODE_STRVALUE(m_list[m_nCount].EDTM,pList->Getitem(i),"EDTM")

		if(con_id != m_list[m_nCount].m_nSectionID)
		{
			GpcLogTrainMessage(0, 0, "本区段:%d, 但是CAD发来的XML中区段号为:%d,非本台不处理", 
							con_id, m_list[m_nCount].m_nSectionID);
			continue;
		}

		m_nCount++;
	}

	return TRUE;
}

CTD_BZInfo::CTD_BZInfo()
{
	SWT=0;
	LEN=0;
	LD=0;
	PBCNT=0;
}

BOOL InitBZRecordFromXML(MSXML::IXMLDOMNodePtr pNodeRoot, CTD_BZInfo*obj)
{
	GET_NODE_STRVALUE(obj->TYPE,pNodeRoot,"TYPE")
	GET_NODE_STRVALUE(obj->CNO,pNodeRoot,"CNO")
	GET_NODE_STRVALUE(obj->FIRSTNM,pNodeRoot,"FIRSTNM")
	GET_NODE_INTVALUE(obj->LD,pNodeRoot,"LD")
	GET_NODE_INTVALUE(obj->LEN,pNodeRoot,"LEN")
	GET_NODE_INTVALUE(obj->SWT,pNodeRoot,"SWT")
	GET_NODE_INTVALUE(obj->PBCNT,pNodeRoot,"PBCNT")
	return TRUE;
}

//===================================================================
#define MAX_SUB_SUM_COUNT  128
static struct{
	char    direction[64];
	int     count;
	int     weight;
}sub_zhongche_sumary[MAX_SUB_SUM_COUNT];
int sub_sum_zhongche_count;

static struct{
	char    type[8];
	int     count;
}sub_qingche_sumary[MAX_SUB_SUM_COUNT];
int sub_sum_qingche_count;

void SumZhongChe(const char *dir,int ld)
{
	int i;
	for(i=0;i<sub_sum_zhongche_count;i++)
	{
		if(!stricmp(sub_zhongche_sumary[i].direction,dir))
			break;
	}
	if(i>=sub_sum_zhongche_count)
	{
		if(sub_sum_zhongche_count>=MAX_SUB_SUM_COUNT)
			return;
		sub_zhongche_sumary[i].count=0;
		STR_CPY(sub_zhongche_sumary[i].direction,dir);
		sub_sum_zhongche_count++;
	}
	sub_zhongche_sumary[i].count++;
	sub_zhongche_sumary[i].weight+=ld;
}
void SumQingChe(const char *type)
{
	int i;
	for(i=0;i<sub_sum_qingche_count;i++)
	{
		if(!stricmp(sub_qingche_sumary[i].type,type))
			break;
	}
	if(i>=sub_sum_qingche_count)
	{
		if(sub_sum_qingche_count>=MAX_SUB_SUM_COUNT)
			return;
		sub_qingche_sumary[i].count=0;
		STR_CPY(sub_qingche_sumary[i].type,type);
		sub_sum_qingche_count++;
	}
	sub_qingche_sumary[i].count++;
}
BOOL TD_BZGenHeaderXML(CString &hdr,const char *xml,const char *node)
{
	int sumWeight=0;
	int sumLength=0;
	int sumPBCnt=0;
	int sumSelfWeight=0;
	sub_sum_zhongche_count=0;
	sub_sum_qingche_count=0;
	int zhong_car_count=0;
	int qing_car_count=0;
	LOAD_XML(xml,node,pNodeRoot)
	MSXML::IXMLDOMNodeListPtr pRecAbsList = pNodeRoot->selectNodes("BZ_RECORD");
	int rec_count = pRecAbsList->Getlength();
	for(int i = 0; i <rec_count; i++)
	{
		CTD_BZInfo subrec;
		if(!InitBZRecordFromXML(pRecAbsList->Getitem(i), &subrec))
			continue;
		sumWeight+=subrec.LD;
		sumLength+=subrec.LEN;
		sumPBCnt+=subrec.PBCNT;
		sumSelfWeight+=subrec.SWT;
		if(subrec.LD==0)
		{
			SumQingChe(subrec.TYPE);
			qing_car_count++;
		}
		else
		{
			SumZhongChe(subrec.FIRSTNM,subrec.LD);
			zhong_car_count++;
		}
	}
	hdr="<BZ_HEADER>";
	InsertXMLNodeValue(hdr,"COUNT",rec_count);
	CString wgtstr;
	wgtstr.Format("%d.%dt",sumSelfWeight/10,sumSelfWeight%10);
	InsertXMLNodeValue(hdr,"SWT",wgtstr);
	wgtstr.Format("%d.%dt",sumWeight/10,sumWeight%10);
	InsertXMLNodeValue(hdr,"LOAD",wgtstr);
	InsertXMLNodeValue(hdr,"PBCNT",sumPBCnt);
	InsertXMLNodeValue(hdr,"LENGTH",sumLength);
	InsertXMLNodeValue(hdr,"LD_COUNT",zhong_car_count);
	InsertXMLNodeValue(hdr,"EPY_COUNT",qing_car_count);
	for(int i=0;i<sub_sum_zhongche_count;i++)
	{
		hdr+="<BZ_ZHONGCHE>";
		InsertXMLNodeValue(hdr,"FIRSTNM",sub_zhongche_sumary[i].direction);
		InsertXMLNodeValue(hdr,"COUNT",sub_zhongche_sumary[i].count);
		hdr+="</BZ_ZHONGCHE>";
	}
	for(int i=0;i<sub_sum_qingche_count;i++)
	{
		hdr+="<BZ_QINGCHE>";
		InsertXMLNodeValue(hdr,"TYPE",sub_qingche_sumary[i].type);
		InsertXMLNodeValue(hdr,"COUNT",sub_qingche_sumary[i].count);
		hdr+="</BZ_QINGCHE>";
	}
	hdr+="</BZ_HEADER>";
	return TRUE;
}
CTD_TrainCrew::CTD_TrainCrew()
{
	GID=0;
	CREW="";
	TICKET="";
	TNO="";
}

CString CTD_TrainCrew::GetCrewInfo()
{
	CString xml="";
	if(TICKET!="" || CREW!="")
	{
		xml = "列车:";
		xml += TNO;
		xml += ";乘务:";
		xml += CREW;
		xml += ";车底:";
		xml += TICKET;
	}
	
	return xml;
}

CTD_TrainTicketList::CTD_TrainTicketList()
{
	m_nCount=0;
}

BOOL  CTD_TrainTicketList::LoadCrewFromTDXML(const char *xml,const char *node)
{
	m_nCount=0;

	LOAD_XML(xml,node,pNodeRoot)	
	if(pNodeRoot==NULL)
		return FALSE;

	MSXML::IXMLDOMNodeListPtr pList = pNodeRoot->selectNodes( "CREW_RECORD" );
	int rec_count = pList->Getlength();
	MSXML::IXMLDOMNodePtr pNode;
	_variant_t v;
	for(int i=0;i<rec_count&&m_nCount<512;i++)
	{
		MSXML::IXMLDOMNodePtr pNodeRecord = pList->Getitem(i);
		if(pNodeRecord)
		{
			m_list[m_nCount].GID=0;
			m_list[m_nCount].TICKET="";
			m_list[m_nCount].CREW="";
			m_list[m_nCount].TNO="";

			pNode=pNodeRecord->selectSingleNode("GID");
			if(pNode)
			{
				v = pNode->GetnodeTypedValue();
				m_list[m_nCount].GID = _atoi64(CString((v.bstrVal)));
				SysFreeString(v.bstrVal);
                v.Clear();
			}

			pNode=pNodeRecord->selectSingleNode("TNO");
			if(pNode)
			{
				v = pNode->GetnodeTypedValue();
				m_list[m_nCount].TNO = v.bstrVal;
				SysFreeString(v.bstrVal);
                v.Clear();
			}

			pNode=pNodeRecord->selectSingleNode("TICKET");
			if(pNode)
			{
				v = pNode->GetnodeTypedValue();
				m_list[m_nCount].TICKET = v.bstrVal;
				SysFreeString(v.bstrVal);
                v.Clear();
			}

			pNode=pNodeRecord->selectSingleNode("CREW");
			if(pNode)
			{
				v = pNode->GetnodeTypedValue();
				m_list[m_nCount].CREW = v.bstrVal;
				SysFreeString(v.bstrVal);
                v.Clear();
			}
			
			m_nCount++;
		}
	}

	return TRUE;
}

CTD_CrewNotice::CTD_CrewNotice()
{
	m_count=0;
	m_sendcount=0;
}

void SetTrainFirstStationEntry(CTD_TrainInfo& traininfo, CTG_SingleTrain &newtrain, int schd_type) //schd_type:1日班计划、调整图 2基本图 主要针对接入和交出
{
	if(newtrain.GetTrainRecordCount()<=0)
		return;

	CTG_TrainRecord *prec=newtrain.GetTrainRecord(0);
	if(prec==NULL)
		return;

	if(traininfo.IFLAG==1) 
		TG_SetFlagStart(prec->m_data.flag);
	else
		TG_SetFlagAccept(prec->m_data.flag);

	if (!GetTGFunctionConfig()->IsCalBasicSchdExitEntry() && schd_type==2)
	{
		GpcLogTrainMessage(0, 0, "保留TDMS传来基本图接入站入口,TID=%s,ST=%s ",traininfo.TNO,traininfo.FROMST);
		return;
	}

	if (!GetTGFunctionConfig()->IsCalShiftSchdExitEntry() && schd_type==1)
	{
		GpcLogTrainMessage(0, 0, "保留TDMS传来日班计划接入站入口,TID=%s,ST=%s ",traininfo.TNO,traininfo.FROMST);
		return;
	}

	if(traininfo.IFLAG!=1) 
	{
		// 按照前方站设置列车接入方向
		int from_st=CovertStationNameToNo(traininfo.FROMST);
		if(from_st>0)
		{
		  int to_st=prec->GetStationNo();
		  int absnum=GetTGFunctionConfig()->GetLineNo(from_st, to_st);
		  if(absnum>0)
		  {
			prec->m_data.entry=GetTGBasicParam()->GetEntryByCTCAbsLineNumber(to_st,absnum);
				GpcLogTrainMessage(0, 0, "根据 tg_logic.cfg中[STATION_NEXTSTATION_LINENO]配置:%d,%d,%d,设置车站 %d 入口 %d.",
					from_st, to_st, absnum, prec->m_data.station, prec->m_data.entry);
		  }
		  else
		  {
			CString strTrainID=newtrain.GetTrainID();
			if(!GetTGFunctionConfig()->IsCalExitEntryByTrainId())
			{
				strTrainID=prec->GetArriveTID();
			}
			int nDirection=GetTGFunctionConfig()->GetTrainDirection(strTrainID);

			GpcLogTrainMessage(0, 0, "TID=%s, 前方站 %s, 车次 %s 列车方向 %d(0:上行 1:下行)",
						traininfo.TNO, traininfo.FROMST, strTrainID, nDirection);
			int to_st=prec->GetStationNo();
			if(nDirection!=-1)
			{
				int absnum=GetTGBasicParam()->GetAbsNumberBetweenStations(from_st, to_st, nDirection);
				if(absnum>0)
				{
					prec->m_data.entry=GetTGBasicParam()->GetEntryByCTCAbsLineNumber(to_st,absnum);
					GpcLogTrainMessage(0, 0, "TID=%s, 按照前方站 %s, 接入车次 %s 的方向找到线号%d, 算出车站:%d 入口 %d",
						traininfo.TNO, traininfo.FROMST, strTrainID, absnum, to_st, prec->m_data.entry);
				}
				else
				{
					GpcLogTrainMessage(0, 0, "TID=%s, 按照前方站 %d, 车站 %d, 找不到 %d 的区间线别",
											traininfo.TNO, from_st, to_st, nDirection);
				}
		    }
		  }
		}
		else
		{
			GpcLogTrainMessage(0, 0, "OPMS传来接入前站错误,TID=%s,ST=%s ",traininfo.TNO,traininfo.FROMST);
		}
	}

	return;
}

void SetTrainLastStationExit(CTD_TrainInfo& traininfo, CTG_SingleTrain &newtrain, int schd_type) //schd_type:1日班计划、调整图 2基本图 主要针对接入和交出
{
	if(newtrain.GetTrainRecordCount()<=0)
		return;

	CTG_TrainRecord *prec=newtrain.GetTrainRecord(newtrain.GetTrainRecordCount()-1);
	if(prec==NULL)
		return;

	if(traininfo.OFLAG==1)
		TG_SetFlagTerminal(prec->m_data.flag);
	else
		TG_SetFlagGiveout(prec->m_data.flag);
	
	if (!GetTGFunctionConfig()->IsCalBasicSchdExitEntry() && schd_type==2)
	{
		GpcLogTrainMessage(0, 0, "保留TDMS传来基本图交出站出口,TID=%s,ST=%s ",traininfo.TNO,traininfo.TOST);
		return;
	}

	if (!GetTGFunctionConfig()->IsCalShiftSchdExitEntry() && schd_type==1)
	{
		GpcLogTrainMessage(0, 0, "保留TDMS传来日班计划交出站出口,TID=%s,ST=%s ",traininfo.TNO,traininfo.TOST);
		return;
	}

	if (traininfo.OFLAG!=1)
	{
		// 按照后方站设置列车交出方向
		int to_st=CovertStationNameToNo(traininfo.TOST);
		if(to_st>0)
		{
		  int from_st=prec->GetStationNo();
		  int absnum=GetTGFunctionConfig()->GetLineNo(from_st, to_st);
		  if(absnum>0)
		  {
			  prec->m_data.exit=GetTGBasicParam()->GetEntryByCTCAbsLineNumber(from_st,absnum);
				GpcLogTrainMessage(0, 0, "根据 tg_logic.cfg中[STATION_NEXTSTATION_LINENO]配置:%d,%d,%d,设置车站 %d 出口 %d.",
					from_st, to_st, absnum, prec->m_data.station, prec->m_data.exit);
		  }
		  else
		  {	
			CString strTrainID=newtrain.GetTrainID();
			if(!GetTGFunctionConfig()->IsCalExitEntryByTrainId())
			{
				strTrainID=prec->GetDepartTID();
			}
			int nDirection=GetTGFunctionConfig()->GetTrainDirection(strTrainID);

			GpcLogTrainMessage(0, 0, "TID=%s, 后方站 %s, 车次 %s 列车方向 %d(0:上行 1:下行)",
						traininfo.TNO, traininfo.TOST, strTrainID, nDirection);
			
			int from_st=prec->GetStationNo();
			if(nDirection!=-1)
			{
				int absnum=GetTGBasicParam()->GetAbsNumberBetweenStations(from_st, to_st, nDirection);
				if(absnum>0)
				{
					prec->m_data.exit=GetTGBasicParam()->GetEntryByCTCAbsLineNumber(from_st,absnum);
					GpcLogTrainMessage(0, 0, "TID=%s, 按照后方站 %s, 车次 %s 的方向找到线号%d, 算出车站:%d 出口:%d",
						traininfo.TNO,traininfo.TOST, strTrainID, absnum, from_st, prec->m_data.exit);
				}
				else
				{
					GpcLogTrainMessage(0, 0, "TID=%s, 按照车站 %d, 后方站 %d, 找不到 %d 的区间线别",
											traininfo.TNO, from_st, to_st, nDirection);
				}
			}
		  }
		}
		else
		{
			GpcLogTrainMessage(0, 0, "OPMS传来交出站错误,TID=%s,ST=%s ",traininfo.TNO,traininfo.TOST);
		}
	}

	return;
}

BOOL  CTD_CrewNotice::LoadCrewNoticeFromTDXML(const char *xml,const char *node)
{
	m_count=0;
	m_sendcount=0;

	LOAD_XML(xml,node,pNodeRoot)	
	if(pNodeRoot==NULL)
		return FALSE;

	GET_NODE_INTVALUE(m_sendcount,pNodeRoot,"COUNT")

	CString GTIDS;
	GET_NODE_STRVALUE(GTIDS,pNodeRoot,"GTID")
	if(!GTIDS.IsEmpty())
	{
		CStringArray array;
		int cnt=ConvertToSubStringArray(GTIDS,',',array);
		for(int i=0;i<cnt&&i<sizeof(m_gtid)/sizeof(m_gtid[0]);i++)
		{
			long nGTID=atol(array[i]);
			m_gtid[m_count]=nGTID;
			m_count++;
		}
	}

	return TRUE;
}

CTD_TYDCMD::CTD_TYDCMD()
{
	CTYPE=0;
	CRCNT=0;

}

CTD_TYDCMD::CTD_TYDCMD(const CTD_TYDCMD& rhs)
{
	CID=rhs.CID;
	CTYPE=rhs.CTYPE;

	CRECV=rhs.CRECV;
	CTITLE=rhs.CTITLE;
	CCONTENT=rhs.CCONTENT;
	CPNM=rhs.CPNM;
	CSID=rhs.CSID;
	CRCNT=rhs.CRCNT;
	CRPT=rhs.CRPT;
}

BOOL CTD_TYDCMD::LoadTYDcmd(const char *xml,const char *node)
{
	CTYPE=0;
	CRCNT=0;

	LOAD_XML(xml,node,pNodeRoot)	
	if(pNodeRoot==NULL)
		return FALSE;

	GET_NODE_STRVALUE(CID,pNodeRoot,"CID")
	GET_NODE_INTVALUE(CTYPE,pNodeRoot,"CTYPE")
	GET_NODE_STRVALUE(CRECV,pNodeRoot,"CRECV")
	GET_NODE_STRVALUE(CTITLE,pNodeRoot,"CTITLE")
	GET_NODE_STRVALUE(CCONTENT,pNodeRoot,"CCONTENT")

	GET_NODE_STRVALUE(CPNM,pNodeRoot,"CPNM")
	GET_NODE_STRVALUE(CSID,pNodeRoot,"CSID")
	GET_NODE_INTVALUE(CRCNT,pNodeRoot,"CRCNT")
	GET_NODE_STRVALUE(CRPT,pNodeRoot,"CRPT")

	return TRUE;
}

BOOL CTD_TYDCMD::SaveTYDcmdToXML(CString &dest,const char *node)
{
	if(node)
		dest+="<";dest+=node;dest+=">";

	InsertXMLNodeValue(dest,"CID",CID);
	InsertXMLNodeValue(dest,"CTYPE",CTYPE);
	InsertXMLNodeValue(dest,"CRECV",CRECV);
	InsertXMLNodeValue(dest,"CTITLE",CTITLE);
	InsertXMLNodeValue(dest,"CCONTENT",CCONTENT);
	InsertXMLNodeValue(dest,"CPNM",CPNM);
	InsertXMLNodeValue(dest,"CSID",CSID);
	InsertXMLNodeValue(dest,"CRCNT",CRCNT);
	InsertXMLNodeValue(dest,"CRPT",CRPT);

	if(node)
		dest+="</";dest+=node;dest+=">";

	return TRUE;
}

CTD_TYNOTIFY::CTD_TYNOTIFY()
{
	NTM=0;
	NGID=0;
}

CTD_TYNOTIFY::CTD_TYNOTIFY(const CTD_TYNOTIFY& rhs)
{
	NTITLE=rhs.NTITLE;
	NCONTENT=rhs.NCONTENT;
	NTM=rhs.NTM;
	NGID=rhs.NGID;
}

BOOL CTD_TYNOTIFY::LoadTYNOTIFY(const char *xml,const char *node)
{
	NTM=0;
	NGID=0;

	LOAD_XML(xml,node,pNodeRoot)	
	if(pNodeRoot==NULL)
		return FALSE;

	GET_NODE_STRVALUE(NTITLE,pNodeRoot,"NTITLE")
	GET_NODE_STRVALUE(NCONTENT,pNodeRoot,"NCONTENT")
	GET_NODE_TIMEVALUE(NTM,pNodeRoot,"NTM")
	//GET_NODE_INTVALUE(NGID,pNodeRoot,"NGID")

	MSXML::IXMLDOMNodePtr pNode = pNodeRoot->selectSingleNode("NGID");
	if(pNode)
	{
		_variant_t v;
		v = pNode->GetnodeTypedValue();
		NGID = _atoi64(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}


	return TRUE;
}

BOOL CTD_TYNOTIFY::LoadTYNOTIFYX(const char *xml,const char *node)
{
	NTM=0;
	NGID=0;

	LOAD_XML(xml,node,pNodeRoot)	
		if(pNodeRoot==NULL)
			return FALSE;

	GET_NODE_STRVALUE(NTITLE,pNodeRoot,"NTITLE")
	GET_NODE_STRVALUE(NCONTENT,pNodeRoot,"NCONTENT")
	GET_NODE_INTVALUE(NTM,pNodeRoot,"NTM")
	//GET_NODE_INTVALUE(NGID,pNodeRoot,"NGID")

		MSXML::IXMLDOMNodePtr pNode = pNodeRoot->selectSingleNode("NGID");
	if(pNode)
	{
		_variant_t v;
		v = pNode->GetnodeTypedValue();
		NGID = _atoi64(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}


	return TRUE;
}

BOOL CTD_SPECPLAN::LoadSPECPLANFromFile(const char *xml,const char *node)
{
	LOAD_XML(xml,node,pNodeRoot)	
	if(pNodeRoot==NULL)
		return FALSE;

	GET_NODE_INTVALUE(SRCCODE,pNodeRoot,"SRCCODE")
	GET_NODE_STRVALUE(MSGID,pNodeRoot,"MSGID")
	GET_NODE_STRVALUE(JHTNM,pNodeRoot,"JHTNM")
	GET_NODE_INTVALUE(DATE,pNodeRoot,"DATE")
	GET_NODE_INTVALUE(COUNT,pNodeRoot,"COUNT")
	GET_NODE_INTVALUE(SEQ,pNodeRoot,"SEQ")
	GET_NODE_STRVALUE(CONTENT,pNodeRoot,"CONTENT")
	GET_NODE_INTVALUE(TRACKID,pNodeRoot,"TRACKID")

	GET_NODE_STRVALUE(TNO,pNodeRoot,"TNO")
	GET_NODE_INTVALUE(STTM,pNodeRoot,"STTM")
	GET_NODE_INTVALUE(EDTM,pNodeRoot,"EDTM")
	GET_NODE_STRVALUE(STST,pNodeRoot,"STST")
	GET_NODE_STRVALUE(EDST,pNodeRoot,"EDST")

	GET_NODE_INTVALUE(NOTECNT,pNodeRoot,"NOTECNT")

	if (DATE<0)
		DATE=0;
	if (STTM<0)
		STTM=0;
	if (EDTM<0)
		EDTM=0;

	int count=0;
	MSXML::IXMLDOMNodeListPtr pList = pNodeRoot->selectNodes( "NOTE" );
	int rec_count = pList->Getlength();
	MSXML::IXMLDOMNodePtr pNode;
	_variant_t v;
	for(int i=0;i<rec_count&&count<MAX_TY_NOTIFY_NUM;i++)
	{
		MSXML::IXMLDOMNodePtr pNodeRecord = pList->Getitem(i);
		if(pNodeRecord)
		{
			CTD_TYNOTIFY rec;
			rec.LoadTYNOTIFYX(pNodeRecord->xml,"NOTE");

			TYNOTIFY.push_back(rec);
			count++;
		}
	}

	if (count != NOTECNT)
		return FALSE;

	GET_NODE_INTVALUE(CMDCNT,pNodeRoot,"CMDCNT");
	count=0;
	MSXML::IXMLDOMNodeListPtr pDCMDList = pNodeRoot->selectNodes( "NCMD" );
	int cmd_count = pDCMDList->Getlength();
	for(int m=0;m<cmd_count&&count<MAX_TY_DCMD_NUM;m++)
	{
		MSXML::IXMLDOMNodePtr pNodeRecord = pDCMDList->Getitem(m);
		if(pNodeRecord)
		{
			CTD_TYDCMD Rec;
			Rec.LoadTYDcmd(pNodeRecord->xml,"NCMD");

			TYDCMDLIST.push_back(Rec);
			count++;
		}
	}

	if (count != CMDCNT)
		return FALSE;

	return TRUE;
}

BOOL CTD_TYNOTIFY::SaveTYNOTIFYToXML(CString &dest,const char *node)
{
	if(node)
		dest+="<";dest+=node;dest+=">";

	InsertXMLNodeValue(dest,"NTITLE",NTITLE);
	InsertXMLNodeValue(dest,"NCONTENT",NCONTENT);
	InsertXMLNodeValue(dest,"NTM",NTM);
	//InsertXMLNodeValue(dest,"NGID",NGID);
	CString tmp;
	tmp.Format("<NGID>%u</NGID>", NGID);
	dest += tmp;

	if(node)
		dest+="</";dest+=node;dest+=">";

	return TRUE;
}

CTD_SPECPLAN::CTD_SPECPLAN()
{
	SRCCODE=0;
	DATE=0;
	COUNT=0;
	SEQ=0;
	TRACKID=0;
	STTM=0;
	EDTM=0;

	NOTECNT=0;
	TRACKID=0;
	TYNOTIFY.clear();
}

BOOL CTD_SPECPLAN::LoadSPECPLAN(const char *xml,const char *node)
{
	LOAD_XML(xml,node,pNodeRoot)	
	if(pNodeRoot==NULL)
		return FALSE;

	GET_NODE_INTVALUE(SRCCODE,pNodeRoot,"SRCCODE")
	GET_NODE_STRVALUE(MSGID,pNodeRoot,"MSGID")
	GET_NODE_STRVALUE(JHTNM,pNodeRoot,"JHTNM")
	GET_NODE_TIMEVALUE(DATE,pNodeRoot,"DATE")
	GET_NODE_INTVALUE(COUNT,pNodeRoot,"COUNT")
	GET_NODE_INTVALUE(SEQ,pNodeRoot,"SEQ")
	GET_NODE_STRVALUE(CONTENT,pNodeRoot,"CONTENT")
	GET_NODE_INTVALUE(TRACKID,pNodeRoot,"TRACKID")
	
	GET_NODE_STRVALUE(TNO,pNodeRoot,"TNO")
	GET_NODE_TIMEVALUE(STTM,pNodeRoot,"STTM")
	GET_NODE_TIMEVALUE(EDTM,pNodeRoot,"EDTM")
	GET_NODE_STRVALUE(STST,pNodeRoot,"STST")
	GET_NODE_STRVALUE(EDST,pNodeRoot,"EDST")

	GET_NODE_INTVALUE(NOTECNT,pNodeRoot,"NOTECNT")

	if (DATE<0)
		DATE=0;
	if (STTM<0)
		STTM=0;
	if (EDTM<0)
		EDTM=0;

	int count=0;
	MSXML::IXMLDOMNodeListPtr pList = pNodeRoot->selectNodes( "NOTE" );
	int rec_count = pList->Getlength();
	MSXML::IXMLDOMNodePtr pNode;
	_variant_t v;
	for(int i=0;i<rec_count&&count<MAX_TY_NOTIFY_NUM;i++)
	{
		MSXML::IXMLDOMNodePtr pNodeRecord = pList->Getitem(i);
		if(pNodeRecord)
		{
			CTD_TYNOTIFY rec;
			rec.LoadTYNOTIFY(pNodeRecord->xml,"NOTE");

			TYNOTIFY.push_back(rec);
			count++;
		}
	}

	if (count != NOTECNT)
		return FALSE;

	GET_NODE_INTVALUE(CMDCNT,pNodeRoot,"CMDCNT");
	count=0;
	MSXML::IXMLDOMNodeListPtr pDCMDList = pNodeRoot->selectNodes( "NCMD" );
	int cmd_count = pDCMDList->Getlength();
	for(int m=0;m<cmd_count&&count<MAX_TY_DCMD_NUM;m++)
	{
		MSXML::IXMLDOMNodePtr pNodeRecord = pDCMDList->Getitem(m);
		if(pNodeRecord)
		{
			CTD_TYDCMD Rec;
			Rec.LoadTYDcmd(pNodeRecord->xml,"NCMD");

			TYDCMDLIST.push_back(Rec);
			count++;
		}
	}

	if (count != CMDCNT)
		return FALSE;

	return TRUE;
}

BOOL CTD_SPECPLAN::SaveSPECPLANToFile(CString &dest,const char *node)
{
	if(node)
		dest+="<";dest+=node;dest+=">";

	InsertXMLNodeValue(dest,"SRCCODE",SRCCODE);
	InsertXMLNodeValue(dest,"MSGID",MSGID);
	InsertXMLNodeValue(dest,"JHTNM",JHTNM);
	InsertXMLNodeValue(dest,"DATE",DATE);
	InsertXMLNodeValue(dest,"CONTENT",CONTENT);
	InsertXMLNodeValue(dest,"TNO",TNO);
	InsertXMLNodeValue(dest,"STTM",STTM);
	InsertXMLNodeValue(dest,"EDTM",EDTM);
	InsertXMLNodeValue(dest,"STST",STST);
	InsertXMLNodeValue(dest,"EDST",EDST);

	InsertXMLNodeValue(dest,"NOTECNT",NOTECNT);
	InsertXMLNodeValue(dest,"CMDCNT",CMDCNT);
	InsertXMLNodeValue(dest,"TRACKID",TRACKID);

	if(NOTECNT>0)
	{
		for(int i=0;i<NOTECNT;i++)
		{
			CString xml;
			TYNOTIFY[i].SaveTYNOTIFYToXML(xml,"NOTE");
			dest+=xml;
		}
	}

	if (CMDCNT>0)
	{
		for (int m=0;m<CMDCNT;m++)
		{
			CString xml;
			TYDCMDLIST[m].SaveTYDcmdToXML(xml,"NCMD");
			dest+=xml;
		}
	}

	if(node)
		dest+="</";dest+=node;dest+=">";

	return TRUE;
}

// 多次切分列车时,用于调整计划，直接由xml生成TD类型列车
BOOL TD_LoadTrainFromXMLx(const CTG_ScheduleInfo& schdinfo,CTD_Train &tdtrain,const char *xml,const char *node,int *pflag)
{
	CTD_TrainInfo traininfo;
	CTD_TrainRecordInfo recordinfo;
	GpcLogTrainMessage(0, 0, "===================================");
	if (GetTGFunctionConfig()->IsEnableLogTDMessage())
		GpcLogTrainMessage(0, 0, "%s", xml);
	LOAD_XML(xml,node,pRootNode)
	if(!TD_LoadTrainInfo(pRootNode, traininfo))
		return FALSE;
	GpcLogTrainMessage(0, 0, "OPMS车站,TID=%s FROMST=%s TOST=%s",traininfo.TNO,traininfo.FROMST,traininfo.TOST);
	if(pflag)
	{
		*pflag=traininfo.LYLFG;
		if(traininfo.YLFG==0)
		{
			*pflag=0; 
		}
		if (traininfo.HALT==1)
		{
			*pflag=0;
		}
	}
	tdtrain.m_TDInfo=traininfo;
	
	MSXML::IXMLDOMNodeListPtr pList = pRootNode->selectNodes( "ST_RECORD" );
	int rec_count = pList->Getlength();
	for(int idx = 0; idx < rec_count; idx++)
	{
		CTD_TrainRecordInfo recordinfo;
		if(!TD_LoadTrainRecordInfo(pList->Getitem(idx),recordinfo))
			return FALSE;

		tdtrain.m_TDRecordList.push_back(recordinfo);
	}
	/*added by zhaoliang 20100530
	遍历节点
	找到第一组机车跳出
	将此组机车最为该运行线机车号
	对每个节点，优先判断发点机车
	机车号码之间通过 ， 号隔开
	机车ID与机车类型用 / 隔开
	int     ARR_ENGINE_NUM;//机车数量
	CString ARR_ENGINE_NAME;//机车名称=机车类型+机车号
	int     DEP_ENGINE_NUM;
	CString DEP_ENGINE_NAME;
	*/
	for(int idx = 0; idx < rec_count; idx++)
	{
		if(!TD_LoadTrainRecordInfo(pList->Getitem(idx),recordinfo))//recordinfo 存放 节点信息
			return FALSE;
		CString engine_name;
		engine_name.Empty();
		if (!recordinfo.DEP_ENGINE_NAME.IsEmpty())//根据兰州特殊情况，对每个节点先判断发点机车
		{
			engine_name = recordinfo.DEP_ENGINE_NAME;
		} 
		else if(!recordinfo.ARR_ENGINE_NAME.IsEmpty()&&idx != 0)//判断到点机车,且第一个节点不判断到点机车
		{
			engine_name = recordinfo.ARR_ENGINE_NAME;
		}

		if (!engine_name.IsEmpty())//非空
		{
			CString strenginetmp,strengname,StrEngine_Style,StrEngine_ID;
			int Engine_Style;
			
			CStringArray EnginenameArray;//存放所有机车号
			CTG_TrainEngineList engineList;
			CTG_TrainEngineInfo engineInfo;
			strenginetmp.Empty();
			strengname.Empty();
			StrEngine_Style.Empty();
			StrEngine_ID.Empty();
			strenginetmp=engine_name;
			//提出机车号
			int cnt = ConvertToSubStringArray(strenginetmp,';',EnginenameArray);
			
			for (int i=0;i < EnginenameArray.GetSize();i++)
			{ 
				strengname = EnginenameArray.GetAt(i);//依次取机车
				//解析机车

				Engine_Style=atoi(strengname.Left(3)); //前三位 机车类型
				StrEngine_ID=strengname.Right(5);      //后五位 机车号

				int section = schdinfo.m_nSectionID;//gpcdef.GetUserGpcId();
				StrEngine_Style = GetTGBasicParam()->GetLocoPreName(section, strengname.Left(3));
				//读入机车
				ZeroMemory(&engineInfo.m_data, 0);
				strncpy(engineInfo.m_data.engine_id, strengname, 8);          //engine  ID
				engineInfo.m_data.engine_id[8]='\0';
				strncpy(engineInfo.m_data.engine_type, StrEngine_Style,9);   //engine  type
				engineInfo.m_data.engine_id[9]='\0';
				
				//2015.1.9 hjz增加日班计划接收机车号后 电力属性的判断
				int cascoType = GetTGBasicParam()->GetCascoIdByLocoId(section, Engine_Style);
				if(cascoType >= 128)
					TG_SetElecEngineExt(engineInfo.m_data.engine_status);
				else
					TG_SetDieselEngineExt(engineInfo.m_data.engine_status);

				engineInfo.m_data.attach_sta = 0;                                //attach  station
				engineInfo.m_data.unattach_sta = rec_count - 1;                  //unattach  station
				//存放机车
				engineList.AddRecord(engineInfo);
			}
			tdtrain.m_cEngineList=engineList; 
			//释放 CStringArray
			EnginenameArray.RemoveAll();
			EnginenameArray.FreeExtra();

			break;
		}
	}
    //end for engine 

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
CTD_TrainInfo::CTD_TrainInfo()
{
	GTID=0;
	IFLAG=0;
	OFLAG=0;
	LSTTM=0;
	LEDTM=0;
	STTM=0;
	FJTM=0;
	TRAINTYPE=152; // 直通货车
	m_nStationCount=0;

	RTID=0;
	HALT=0;
	DGID=0;
}
CTD_TrainInfo::CTD_TrainInfo(const CTD_TrainInfo &src)
{
	SetValue(src);
}

CTD_TrainInfo& CTD_TrainInfo::operator= (const CTD_TrainInfo &src)
{
	SetValue(src);
	return *this;
}

void CTD_TrainInfo::SetValue(const CTD_TrainInfo &src)
{
	SCHTYP=src.SCHTYP;
	YLFG=src.YLFG; 
	LYLFG=src.LYLFG; 
	TRAINTYPE=src.TRAINTYPE;
	GTID=src.GTID;

	TNO=src.TNO; 
	IFLAG=src.IFLAG; 
	OFLAG=src.OFLAG; 

	STST=src.STST; 
	ENDST=src.ENDST; 
	FROMST=src.FROMST;
	TOST=src.TOST;

	LSTTM=src.LSTTM; 
	LEDTM=src.LEDTM; 
	STTM=src.STTM;
	FJTM=src.FJTM;
	FJTDTM=src.FJTDTM;

	m_nStationCount=src.m_nStationCount;
	SEQ=src.SEQ;
	COUNT=src.COUNT;

	RTID=src.RTID;
	EMU=src.EMU;
	DGID=src.DGID;
	HALT=src.HALT;
}

//////////////////////////////////////////////////////////////
CTD_TrainRecordInfo::CTD_TrainRecordInfo()
{
	STID=0;
	NODESEQ=0;

	ARND=0;
	DPND=0;
	ENTRY_LINE=0;
	EXIT_LINE=0;

	SIDENO=5555;

	ARTM=0;
	DPTM=0;
	SARTM=0;
	SDPTM=0;
	BSTM=0;

	IMPINFO=0;
	STPINFO=0;
	memset(METINFO,0,sizeof(METINFO));
	m_nNotMeetCount=0;
	SGFLAG=0;

	ARR_ENGINE_NUM=0;
	DEP_ENGINE_NUM=0;

	OPERFLAG=0;
	WORKFLAG=0;
	CWCL=0;
}
CTD_TrainRecordInfo::CTD_TrainRecordInfo(const CTD_TrainRecordInfo &src)
{
	SetValue(src);
}

CTD_TrainRecordInfo& CTD_TrainRecordInfo::operator= (const CTD_TrainRecordInfo &src)
{
	SetValue(src);
	return *this;
}

void CTD_TrainRecordInfo::SetValue(const CTD_TrainRecordInfo &src)
{
	STID=src.STID;
	NODESEQ=src.NODESEQ; // 节点顺序
	STNM=src.STNM; //站名 *

	ARND=src.ARND;
	DPND=src.DPND; 
	ENTRY_LINE=src.ENTRY_LINE; 
	EXIT_LINE=src.EXIT_LINE;
	SIDENO=src.SIDENO;

	STNM=src.STNM;

	ARTNO=src.ARTNO;
	DPTNO=src.DPTNO;

	ARTM=src.ARTM;
	DPTM=src.DPTM; 
	SARTM=src.SARTM; 
	SDPTM=src.SDPTM;
	BSTM=src.BSTM;
	
	IMPINFO=src.IMPINFO; 
	STPINFO=src.STPINFO;
	for(int i=0; i<sizeof(METINFO)/sizeof(METINFO[0]); i++)
	{
		METINFO[i]=src.METINFO[i];
	}
	m_nNotMeetCount=src.m_nNotMeetCount;

	SGINFO=src.SGINFO; 
	SGFLAG=src.SGFLAG;

	ARR_ENGINE_NUM=src.ARR_ENGINE_NUM; 
	ARR_ENGINE_NAME=src.ARR_ENGINE_NAME;
	DEP_ENGINE_NUM=src.DEP_ENGINE_NUM; 
	DEP_ENGINE_NAME=src.DEP_ENGINE_NAME;

	OPERFLAG=src.OPERFLAG;
	OPERINFO=src.OPERINFO; 
	WORKFLAG=src.WORKFLAG;
	CWCL=src.CWCL;
	NOTE_TEXT=src.NOTE_TEXT;
}

/////////////////////////////////////////////////////
CTD_Train::CTD_Train()
{
	m_TDRecordList.clear();
}

CTD_Train::CTD_Train(const CTD_Train &src)
{
	m_TDInfo=src.m_TDInfo;
	m_cEngineList=src.m_cEngineList;
	m_TDRecordList.clear();
	m_TDRecordList.assign(src.m_TDRecordList.begin(), src.m_TDRecordList.end());
}

CTD_Train& CTD_Train::operator= (const CTD_Train &src)
{
	m_TDInfo=src.m_TDInfo;
	m_cEngineList=src.m_cEngineList;
	m_TDRecordList.clear();
	m_TDRecordList.assign(src.m_TDRecordList.begin(), src.m_TDRecordList.end());

	return *this;
}

CTD_TrainRecordInfo* CTD_Train::GetTrainTDRecord(WORD idx)
{
	if(idx<m_TDRecordList.size())
	{
		return &m_TDRecordList[idx];
	}

	return NULL;
}

bool TD_GetKediaoInfoFromXML(CTG_TrainKediaoInfo& kd,const char *xml,const char *node)
{
	LOAD_XML(xml,node,pNodeRoot)	  
	GET_NODE_INTVALUE(kd.gtid,pNodeRoot,"GTID")
	GET_NODE_INTVALUE(kd.msgid,pNodeRoot,"MSGID")
	GET_NODE_INTVALUE(kd.type,pNodeRoot,"TYPE")
	GET_NODE_STRVALUE(kd.content,pNodeRoot,"KDINFO")
	if(kd.type==-1) // 防止TDi过来解析失败发生-1，这个需要防护正确值是0,1,2
		kd.type=0;
	
	if(kd.content.GetLength()>2000)
	{
		kd.content="请注意,命令内容太长,不能接收,请通过其他手段查阅";
	}
	return true;
}

static BOOL TD_LoadTrainYYRecordInfo(MSXML::IXMLDOMNodePtr pNodeRoot,TG_TrainYingyeInfo &recordinfo)
{
	memset(recordinfo.station_name, 0, sizeof(recordinfo.station_name));
	CString str;
	GET_NODE_STRVALUE(str,pNodeRoot,"STNM")
	if(str.GetLength()>0 && str.GetLength()<sizeof(recordinfo.station_name))
		STR_CPY(recordinfo.station_name, str);
	
	GET_NODE_TIMEVALUE(recordinfo.arrive_time,pNodeRoot,"ARTM")
	GET_NODE_TIMEVALUE(recordinfo.depart_time,pNodeRoot,"DPTM")
	int stno=CovertStationNameToNo(recordinfo.station_name);
	if(stno>0)
		recordinfo.station_no=stno;
	else
		recordinfo.station_no=0;
	return TRUE;
}

bool TD_GetYingyeInfoFromXML(CTG_TrainYingyeList& yy,const char *xml,const char *node)
{
	LOAD_XML(xml,node,pNodeRoot)	  
	GET_NODE_INTVALUE(yy.gtid,pNodeRoot,"GTID")
	GET_NODE_INTVALUE(yy.msgid,pNodeRoot,"MSGID")
				
	MSXML::IXMLDOMNodeListPtr pList = pNodeRoot->selectNodes( "ST_RECORD" );
	int rec_count = pList->Getlength();
	for(int i=0; i<rec_count; i++)
	{
		TG_TrainYingyeInfo recordinfo;
		if(!TD_LoadTrainYYRecordInfo(pList->Getitem(i),recordinfo))
			return false;
		yy.AddRecord(recordinfo);
	}

	return true;
}

static BOOL TD_LoadTrainTicketRecordInfo(MSXML::IXMLDOMNodePtr pNodeRoot,TG_TrainTicketInfo &recordinfo)
{
	memset(recordinfo.down_station_name, 0, sizeof(recordinfo.down_station_name));
	memset(recordinfo.on_station_name, 0, sizeof(recordinfo.on_station_name));
	memset(recordinfo.seat, 0, sizeof(recordinfo.seat));
	GET_NODE_INTVALUE(recordinfo.on_station_index,pNodeRoot,"ON_SEQ")
	CString str;
	GET_NODE_STRVALUE(str,pNodeRoot,"ON_STNM")
	if(str.GetLength()>0 && str.GetLength()<sizeof(recordinfo.on_station_name))
		STR_CPY(recordinfo.on_station_name, str);

	GET_NODE_INTVALUE(recordinfo.down_station_index,pNodeRoot,"DOWN_SEQ")
	
	GET_NODE_STRVALUE(str,pNodeRoot,"DOWN_STNM")
	if(str.GetLength()>0 && str.GetLength()<sizeof(recordinfo.down_station_name))
		STR_CPY(recordinfo.down_station_name, str);

	GET_NODE_INTVALUE(recordinfo.sale,pNodeRoot,"SALE")
	
	GET_NODE_STRVALUE(str,pNodeRoot,"SEAT")
	if(str.GetLength()>0 && str.GetLength()<sizeof(recordinfo.seat))
		STR_CPY(recordinfo.seat, str);

	int stno=CovertStationNameToNo(recordinfo.on_station_name);
	if(stno>0)
		recordinfo.on_station=stno;
	else
		recordinfo.on_station=0;

	stno=CovertStationNameToNo(recordinfo.down_station_name);
	if(stno>0)
		recordinfo.down_station=stno;
	else
		recordinfo.down_station=0;
	return TRUE;
}

bool TD_GetTicketInfoFromXML(CTG_TrainTicketList& tk,const char *xml,const char *node)
{
	LOAD_XML(xml,node,pNodeRoot)	  
	GET_NODE_INTVALUE(tk.gtid,pNodeRoot,"GTID")
	GET_NODE_INTVALUE(tk.msgid,pNodeRoot,"MSGID")
	GET_NODE_STRVALUE(tk.trainid,pNodeRoot,"TNO")
	
	MSXML::IXMLDOMNodeListPtr pList = pNodeRoot->selectNodes( "ST_RECORD" );
	int rec_count = pList->Getlength();
	for(int i=0; i<rec_count; i++)
	{
		TG_TrainTicketInfo recordinfo;
		if(!TD_LoadTrainTicketRecordInfo(pList->Getitem(i),recordinfo))
			return false;
		tk.AddRecord(recordinfo);
	}
	return true;
}

struct CREW_INFO_NODE
{
	int  type;
	char *name;
	char *name_bru;
} 
crew_ary[5]=
{//0:未知 1：司机	2：机械师	3：乘警	4：保洁	5：餐饮
	{1, "DRIVER", "DRIVERBU"},
	{2, "ENGINE","ENGINEBU"},
	{3, "POLICE", "POLICEBU"},
	{4, "CLEANER","CLEANERBU"},
	{5, "SERVICE","SERVICEBU"}
};
bool TD_GetCrewInfoFromXML(CTG_TrainCrewList& crew,const char *xml,const char *node)
{
	LOAD_XML(xml,node,pNodeRoot)	  
	GET_NODE_INTVALUE(crew.gtid,pNodeRoot,"GTID")
	GET_NODE_INTVALUE(crew.msgid,pNodeRoot,"MSGID")
	GET_NODE_STRVALUE(crew.trainid,pNodeRoot,"TNO")
	GET_NODE_STRVALUE(crew.emuno,pNodeRoot,"EMUNO")

	for(int i=0; i<=4; i++)
	{
		TG_TrainCrewInfo info;
		info.crew_type=crew_ary[i].type;
		CString str;
		GET_NODE_STRVALUE(str,pNodeRoot, crew_ary[i].name)
		if(str.GetLength()>0 && str.GetLength()<sizeof(info.crew_info))
		{	
			STR_CPY(info.crew_info, str);
		}
		else
			info.crew_info[0]=0;
		GET_NODE_STRVALUE(str,pNodeRoot, crew_ary[i].name_bru)
		if(str.GetLength()>0 && str.GetLength()<sizeof(info.crew_bureau))
		{
			STR_CPY(info.crew_bureau, str);
		}
		else
			info.crew_bureau[0]=0;
		crew.AddRecord(info);
	}

	return true;
}

static BOOL TD_LoadEngineRecordInfo(MSXML::IXMLDOMNodePtr pNodeRoot,TG_EngineCrewInfo &recordinfo)
{
	//GET_NODE_INTVALUE(recordinfo.flag,pNodeRoot,"SGFLAG")
	//GET_NODE_STRVALUE(recordinfo.arrive_engine,pNodeRoot,"ARR_ENGINE_NAME")
	//GET_NODE_STRVALUE(recordinfo.arrive_engine,pNodeRoot,"DEP_ENGINE_NAME")

	CString str;
	GET_NODE_STRVALUE(str,pNodeRoot,"STNM")
	if(str.GetLength()>0 && str.GetLength()<sizeof(recordinfo.station_name))
	{
		STR_CPY(recordinfo.station_name, str);
	}
	else
		recordinfo.station_name[0]=0;
	int stno=CovertStationNameToNo(recordinfo.station_name);
	if(stno>0)
		recordinfo.station_no=stno;
	else
		recordinfo.station_no=0;
	GET_NODE_TIMEVALUE(recordinfo.overwork_time,pNodeRoot,"CWCLTM")

	return TRUE;
}

bool TD_LoadEngineCrewInfoFromXML(CTG_EngineCrewList& engine,const char *xml,const char *node, int* seq, int*count)
{
	LOAD_XML(xml,node,pNodeRoot)
	GET_NODE_INTVALUE((*count),pNodeRoot,"COUNT")
	GET_NODE_INTVALUE((*seq),pNodeRoot,"SEQ")
	GET_NODE_INTVALUE(engine.gtid,pNodeRoot,"GTID")
	GET_NODE_INTVALUE(engine.msgid,pNodeRoot,"MSGID")
	GET_NODE_STRVALUE(engine.trainid,pNodeRoot,"TNO")

	MSXML::IXMLDOMNodeListPtr pList = pNodeRoot->selectNodes( "ST_RECORD" );
	int rec_count = pList->Getlength();
	for(int i=0; i<rec_count; i++)
	{
		TG_EngineCrewInfo record;
		if(!TD_LoadEngineRecordInfo(pList->Getitem(i),record))
			return false;
		engine.AddRecord(record);
	}

	return true;
}

