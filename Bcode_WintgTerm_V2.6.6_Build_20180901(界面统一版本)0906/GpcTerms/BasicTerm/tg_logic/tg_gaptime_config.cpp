#include "stdafx.h"
#include "utility/xml_utility.h"
#include "tg_gaptime_config.h"

#import  <msxml.dll> named_guids
using namespace MSXML;

BOOL   CGetGapTimeInfor::LoadFromXML(const char *pathname)
{
    CString xml;
	ReadGenernalXMLFile(xml,pathname);
	if(xml.IsEmpty())
		return FALSE;
	LOAD_XML(xml,"GAP_TIME",pNodeRoot)

	MSXML::IXMLDOMNodeListPtr list_ELE = pNodeRoot->selectNodes( "GAP_DEF");
    if(NULL==list_ELE)
			return FALSE;
	for(int idx = 0; idx < list_ELE->Getlength(); idx++)
	{
		MSXML::IXMLDOMNodePtr pNode_ELE = list_ELE->Getitem(idx);
	   if(NULL==pNode_ELE)
	    		continue;
		AdjustGapTime x;
		CString strValue;
		int intValue;
		GET_NODE_INTVALUE(intValue,pNode_ELE,"@station")
		x.station=intValue;
		GET_NODE_STRVALUE(strValue,pNode_ELE,"@gap_type")
		if(strValue=="OCCUPY_SAME_SIDE")
			x.gap_type =OCCUPY_SAME_SIDE_GAP;
		else
		if(strValue=="OCCUPY_SIDE_AFTER_TERMINAL")
			x.gap_type =OCCUPY_SIDE_AFTER_TERMINAL_GAP;
		else
		if(strValue=="CONTINUE_DEPART")
			x.gap_type =CONTINUE_DEPART_GAP;
		else
		if(strValue=="MEET_TRAIN")
			x.gap_type =MEET_TRAIN_GAP;
		else
		if(strValue=="DIFF_ARRIVE")
			x.gap_type =DIFF_ARRIVE_GAP;
		else
		if(strValue=="ABS_BLOCK")
			x.gap_type =ABS_BLOCK_GAP;
		else
		if(strValue=="SIDE_BLOCK")
			x.gap_type =SIDE_BLOCK_GAP;
		else
		if(strValue=="TRACE")
			x.gap_type =TRACE_GAP;
		
		GET_NODE_STRVALUE(strValue,pNode_ELE,"@act1")
		if(strValue=="PASS")
			x.act1=STATION_PASS;
		else
		if(strValue=="DEPART")
			x.act1=STATION_DEPART;
		GET_NODE_STRVALUE(strValue,pNode_ELE,"@act2")
		if(strValue=="PASS")
			x.act2=STATION_PASS;
		else
		if(strValue=="DEPART")
			x.act2=STATION_DEPART;
		GET_NODE_STRVALUE(strValue,pNode_ELE,"@run_type1")
		if(strValue=="PASSENGER")
			x.run_type1=PASSANGER_TRAIN;
		else
		if(strValue=="FREIGHT")
			x.run_type1=FREIGHT_TRAIN;
		GET_NODE_STRVALUE(strValue,pNode_ELE,"@run_type2")
		if(strValue=="PASSENGER")
			x.run_type2=PASSANGER_TRAIN;
		else
		if(strValue=="FREIGHT")
			x.run_type2=FREIGHT_TRAIN;
		GET_NODE_INTVALUE(intValue,pNode_ELE,"@gap_time")
		x.gap_time=intValue;
	
		m_aAdjustGapTimeConfig.push_back(x);
	}
	return TRUE;
}


int  CGetGapTimeInfor::GetTrainGapTime(STATION_NO station, GAP_TYPE gap_type, ARRIVE_DEPART_STATUS act1, ARRIVE_DEPART_STATUS act2, RUN_TYPE run_type1, RUN_TYPE run_type2) const
{
	int ret_value=0;
	for(int i=0;i<m_aAdjustGapTimeConfig.size();i++)
	{
		AdjustGapTime x=m_aAdjustGapTimeConfig[i];
		if(x.gap_type==gap_type &&
			x.station==station && 
			x.act1==act1 &&
			x.act2==act2 &&
			x.run_type1==run_type1 &&
			x.run_type2==run_type2)
		{
			ret_value=x.gap_time;
			break;
		}
		if(x.gap_type==gap_type &&
			x.station==NULL_STATION &&
			x.act1==STATION_NULLSTATUS &&
			x.act2==STATION_NULLSTATUS &&
			x.run_type1==NULL_RUNTYPE &&
			x.run_type2==NULL_RUNTYPE)
		{
			ret_value=x.gap_time;  // default
		}
	}
	return ret_value;
}
