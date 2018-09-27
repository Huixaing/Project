#include "stdafx.h"
#include <stdio.h>
#include "utility/xml_utility.h"
#include "tg_function_config.h"
#include "sysshare.h"

BOOL  TD_GenRequestMessage_1_BZ(CString &token,CString &dest,DWORD id,int stno,const char *stname,int flag)
{
	token="REQ_BZ";
	dest="<REQ_BZ>"; 
	InsertXMLNodeValue(dest,"STID",stno);
	if(stname)
      InsertXMLNodeValue(dest,"STNM",stname);
	InsertXMLNodeValue(dest,"GTID",id);
	InsertXMLNodeValue(dest,"ADFLAG",flag);
	dest+="</REQ_BZ>"; 
	return TRUE;
}

BOOL  TD_GenRequestMessage_2_YL_BY_ID(CString &token,CString &dest,int con_id,const char *stname,int cnt,DWORD ids[])
{
	CString tmpstr;
	token="REQ_YL_BY_ID";
	dest="<REQ_YL_BY_ID>"; 
	char dispid[10];
	if(GetTGFunctionConfig()->GetTmisSectionID(con_id,dispid)<0)
	{
		AfxMessageBox("无法获取TMIS的调度台简码");
		return FALSE;
	}
	InsertXMLNodeValue(dest,"DISID",dispid);
	InsertXMLNodeValue(dest,"STNM",stname);
	for(int i=0;i<cnt;i++)
	{
		CString t;
		t.Format("%d",ids[i]); 
		tmpstr+=t;
		tmpstr+=",";
	}
	InsertXMLNodeValue(dest,"GTID",tmpstr);
	dest+="</REQ_YL_BY_ID>";
	return TRUE;
}

BOOL  TD_GenRequestMessage_3_YL_BY_TIME(CString &token,CString &dest,int con_id,time_t starttm,time_t endtm,BOOL onlyOrigStart,BOOL Passenger,BOOL freight,const char *stname)
{
	token="REQ_YL_BY_TIME";
	dest="<REQ_YL_BY_TIME>";
	char dispid[10];
	if(GetTGFunctionConfig()->GetTmisSectionID(con_id,dispid)<0)
	{
		AfxMessageBox("无法获取TMIS的调度台简码");
		return FALSE;
	}
	InsertXMLNodeValue(dest,"DISID",dispid);
	InsertXMLNodeTime(dest,"STTM",starttm);
	InsertXMLNodeTime(dest,"EDTM",endtm);
	InsertXMLNodeValue(dest,"STFG",onlyOrigStart);
	InsertXMLNodeValue(dest,"PAFG",Passenger);
	InsertXMLNodeValue(dest,"CAFG",freight);
	InsertXMLNodeValue(dest,"STNM",stname);

	if(!GetTGFunctionConfig()->IsGpcSendReqOtherMsg()) //TD自己请求train外的其它：客调，营业和乘务，运行图不在发送请求
	{
		InsertXMLNodeValue(dest,"AQ_MSG",1); 
	}
	dest+="</REQ_YL_BY_TIME>";
	return TRUE;
}

BOOL  TD_GenRequestMessage_4_YL_BY_TRAINNUM(CString &token,CString &dest,int con_id,time_t starttm,time_t endtm,const char *trainno)
{
	token="REQ_YL_BY_TRAINNUM";
	dest="<REQ_YL_BY_TRAINNUM>";
	char dispid[10];
	if(GetTGFunctionConfig()->GetTmisSectionID(con_id,dispid)<0)
	{
		AfxMessageBox("无法获取TMIS的调度台简码");
		return FALSE;
	}
	InsertXMLNodeValue(dest,"DISID",dispid);
	InsertXMLNodeTime(dest,"STTM",starttm);
	InsertXMLNodeTime(dest,"EDTM",endtm);
	InsertXMLNodeValue(dest,"TNO",trainno);
	dest+="</REQ_YL_BY_TRAINNUM>";
	return TRUE;
}

BOOL  TD_GenRequestMessage_5_YL_BY_TRAINNUM_TIME(CString &token,CString &dest,int con_id,time_t starttm,const char *trainno)
{
	token="REQ_YL_BY_TRAINNUM_TIME";
	dest="<REQ_YL_BY_TRAINNUM_TIME>";
	char dispid[10];
	if(GetTGFunctionConfig()->GetTmisSectionID(con_id,dispid)<0)
	{
		AfxMessageBox("无法获取TMIS的调度台简码");
		return FALSE;
	}
	InsertXMLNodeValue(dest,"DISID",dispid);
	InsertXMLNodeTime(dest,"STTM",starttm);
	InsertXMLNodeValue(dest,"TNO",trainno);
	dest+="</REQ_YL_BY_TRAINNUM_TIME>";
	return TRUE;
}

BOOL  TD_GenRequestMessage_6_GID_BY_TRAINNUM(CString &token,CString &dest,int con_id,time_t starttm,time_t endtm,const char *trainnos) //多个车次在trainnos中逗号隔开
{
	token="REQ_GID_BY_TRAINNUM";
	dest="<REQ_GID_BY_TRAINNUM>";
	char dispid[10];
	if(GetTGFunctionConfig()->GetTmisSectionID(con_id,dispid)<0)
	{
		AfxMessageBox("无法获取TMIS的调度台简码");
		return FALSE;
	}
	InsertXMLNodeValue(dest,"DISID",dispid);
	InsertXMLNodeTime(dest,"STTM",starttm);
	InsertXMLNodeTime(dest,"EDTM",endtm);
	InsertXMLNodeValue(dest,"TNO",trainnos);
	dest+="</REQ_GID_BY_TRAINNUM>";
	return TRUE;
}

BOOL  TD_GenRequestMessage_7_TFG(CString &token,CString &dest,int con_id,DWORD gtid)
{
	token="REQ_TFG";
	dest="<REQ_TFG>";
	char dispid[10];
	if(GetTGFunctionConfig()->GetTmisSectionID(con_id,dispid)<0)
	{
		AfxMessageBox("无法获取TMIS的调度台简码");
		return FALSE;
	}
	InsertXMLNodeValue(dest,"DISID",dispid);
	InsertXMLNodeValue(dest,"GTID",gtid);
	dest+="</REQ_TFG>";
	return TRUE;
}

BOOL  TD_GenRequestMessage_8_NEWYL(CString &token,CString &dest,int con_id,int cnt,DWORD ids[])
{
	token="REQ_NEWYL";
	dest="<REQ_NEWYL>";
	char dispid[10];
	if(GetTGFunctionConfig()->GetTmisSectionID(con_id,dispid)<0)
	{
		AfxMessageBox("无法获取TMIS的调度台简码");
		return FALSE;
	}
	InsertXMLNodeValue(dest,"DISID",dispid);
	CString tmpstr;
	for(int i=0;i<cnt;i++)
	{
		CString t;
		t.Format("%d",ids[i]); 
		tmpstr+=t;
		tmpstr+=",";
	}
	InsertXMLNodeValue(dest,"GTID",tmpstr);
	dest+="</REQ_NEWYL>";
	return TRUE;
}


BOOL  TD_GenRequestMessage_9_SEND_PLAN(CString &token,CString &dest,int con_id,const char *jht,DWORD msgid,const char *msgdata)
{
	token="SEND_PLAN";
	dest="<SEND_PLAN>";

	char dispid[10];
	char jhdisid[10];
	if(GetTGFunctionConfig()->GetTmisSectionID(con_id,dispid,jhdisid)<0)
	{
		AfxMessageBox("无法获取TMIS的调度台简码");
		return FALSE;
	}
	if(jht)
		InsertXMLNodeValue(dest,"RDISID",jht);
	else
		InsertXMLNodeValue(dest,"RDISID",jhdisid);
	InsertXMLNodeValue(dest,"LDISID",dispid);
	InsertXMLNodeValue(dest,"JHID",msgid);
	//InsertXMLNodeValue(dest,"PLANID",jhdisid);
	CString cs;
	cs=gpcdef.GetSectionName(con_id);
	InsertXMLNodeValue(dest,"DISNM",cs);
	cs=gpcdef.GetCurrentUsername();
	InsertXMLNodeValue(dest,"MNM",cs);
	InsertXMLNodeValue(dest,"MSG",msgdata);
	dest+="</SEND_PLAN>";
	return TRUE;
}

BOOL  TD_GenRequestMessage_10_SEND_ADPLAN(CString &token,CString &dest,int con_id,int cnt,DWORD ids[])
{
	token="SEND_ADPLAN";
	dest="<SEND_ADPLAN>";
	char dispid[10];
	if(GetTGFunctionConfig()->GetTmisSectionID(con_id,dispid)<0)
	{
		AfxMessageBox("无法获取TMIS的调度台简码");
		return FALSE;
	}
	InsertXMLNodeValue(dest,"DISID",dispid);
	CString tmpstr;
	for(int i=0;i<cnt;i++)
	{
		CString t;
		t.Format("%d",ids[i]); 
		tmpstr+=t;
		tmpstr+=",";
	}
	InsertXMLNodeValue(dest,"GID",tmpstr);
	dest+="</SEND_ADPLAN>";
	return TRUE;
}

BOOL  TD_GenRequestMessage_12_CMD(CString &token,CString &dest,int con_id,DWORD nDay,const char *id)
{
	token="REQ_CMD";
	dest="<REQ_CMD>";
	char dispid[10];
	if(GetTGFunctionConfig()->GetTmisSectionID(con_id,dispid)<0)
	{
		AfxMessageBox("无法获取TMIS的调度台简码");
		return FALSE;
	}
	InsertXMLNodeValue(dest,"CONID",con_id);
	InsertXMLNodeValue(dest,"DISID",dispid);
	InsertXMLNodeTime(dest,"TM",nDay);
	InsertXMLNodeValue(dest,"PLANID",id);
	dest+="</REQ_CMD>";
	return TRUE;
}

BOOL  TD_GenRequestMessage_13_WKINFO(CString &token,CString &dest,int con_id,time_t onlydate)
{
	token="REQ_WKINFO";
	dest="<REQ_WKINFO>";
	char dispid[10];
	if(GetTGFunctionConfig()->GetTmisSectionID(con_id,dispid)<0)
	{
		AfxMessageBox("无法获取TMIS的调度台简码");
		return FALSE;
	}
	InsertXMLNodeValue(dest,"CONID",con_id);
	InsertXMLNodeValue(dest,"DISID",dispid);
	InsertXMLNodeTime(dest,"TM",onlydate);
	dest+="</REQ_WKINFO>";
	return TRUE;
}

BOOL  TD_GenRequestMessage_14_RSINFO(CString &token,CString &dest,int con_id,time_t onlydate)
{
	token="REQ_RSINFO";
	dest="<REQ_RSINFO>";
	char dispid[10];
	if(GetTGFunctionConfig()->GetTmisSectionID(con_id,dispid)<0)
	{
		AfxMessageBox("无法获取TMIS的调度台简码");
		return FALSE;
	}
	InsertXMLNodeValue(dest,"CONID",con_id);
	InsertXMLNodeValue(dest,"DISID",dispid);
	InsertXMLNodeTime(dest,"TM",onlydate);
	dest+="</REQ_RSINFO>";
	return TRUE;
}

BOOL  TD_GenRequestMessage_20_CREW_INFO(CString &token,CString &dest,DWORD gtid)
{
	token="REQ_CREW_INFO";
	dest="<REQ_CREW_INFO>";
	InsertXMLNodeValue(dest,"GID",gtid);
	dest+="</REQ_CREW_INFO>";
	return TRUE;
}
BOOL  TD_GenRequestMessage_15_KEDIAO_INFO(CString &token,CString &dest,DWORD gtid)
{
	token="REQ_KEDIAO_INFO";
	dest="<REQ_KEDIAO_INFO>";
	InsertXMLNodeValue(dest,"GTID",gtid);
	dest+="</REQ_KEDIAO_INFO>";
	return TRUE;
}

BOOL  TD_GenRequestMessage_16_YINGYE_INFO(CString &token,CString &dest,DWORD gtid)
{
	token="REQ_YINGYE_INFO";
	dest="<REQ_YINGYE_INFO>";
	InsertXMLNodeValue(dest,"GTID",gtid);
	dest+="</REQ_YINGYE_INFO>";
	return TRUE;
}

BOOL  TD_GenRequestMessage_17_SG_WX_INFO(CString &token,CString &dest,int con_id,DWORD nDay,const char *id,int flag) // flag 0:施工 1:维修
{
	token="REQ_SG_WX_INFO";
	dest="<REQ_SG_WX_INFO>";
	char dispid[10];
	if(GetTGFunctionConfig()->GetTmisSectionID(con_id,dispid)<0)
	{
		AfxMessageBox("无法获取TMIS的调度台简码");
		return FALSE;
	}
	InsertXMLNodeValue(dest,"CONID",con_id);
	InsertXMLNodeValue(dest,"DISID",dispid);
	InsertXMLNodeTime(dest,"TM",nDay);
	InsertXMLNodeValue(dest,"PLANID",id);
	InsertXMLNodeValue(dest,"FLAG",flag);
	dest+="</REQ_SG_WX_INFO>";
	return TRUE;
}
BOOL  TD_GenRequestMessage_18_WKDES(CString &token,CString &dest,int con_id,DWORD nDay,const char *id) 
{
	token="REQ_WKDES";
	dest="<REQ_WKDES>";
	char dispid[10];
	if(GetTGFunctionConfig()->GetTmisSectionID(con_id,dispid)<0)
	{
		AfxMessageBox("无法获取TMIS的调度台简码");
		return FALSE;
	}
	InsertXMLNodeValue(dest,"CONID",con_id);
	InsertXMLNodeValue(dest,"DISID",dispid);
	InsertXMLNodeTime(dest,"TM",nDay);
	InsertXMLNodeValue(dest,"PLANID",id);
	dest+="</REQ_WKDES>";
	return TRUE;
}

BOOL  GenXMLMessage_SelectDCMD(CString &token,CString &dest,time_t starttm,time_t endtm,const char *planid)
{
	token="Select_DCMD_By_PlanID";
	dest="<Select_DCMD_By_PlanID>";
	InsertXMLNodeTime(dest,"Start_Time",starttm);
	InsertXMLNodeTime(dest,"End_Time",endtm);
	InsertXMLNodeValue(dest,"Plan_ID",planid);
	dest+="</Select_DCMD_By_PlanID>";
	return TRUE;
}

BOOL  GenXMLMessage_InformDCMDUpdated(CString &token,CString &dest,time_t starttm,time_t endtm)
{
	token="REFLUSH_DCMD_LIST";
	dest="<REFLUSH_DCMD_LIST>";
	dest+="</REFLUSH_DCMD_LIST>";
	return TRUE;
}

BOOL  GenXMLMessage_InformDCMDToFront(CString &token,CString &dest)
{
	token="WINDOW_TOPMOST";
	dest="<WINDOW_TOPMOST>";
	dest+="</WINDOW_TOPMOST>";
	return TRUE;
}

BOOL  GenXMLMessage_InformDCMDLogin(CString &token,CString &dest,const char *username)
{
	token="LOGIN";
	dest="<LOGIN>";
	dest+="<USERNAME>";
	dest+=username;
	dest+="</USERNAME>";
	dest+="<ADMIN>";
	dest+="1";
	dest+="</ADMIN>";
	dest+="</LOGIN>";
	return TRUE;
}
BOOL  TD_GenRequestMessage_19_BASICSCHEDULE(CString &token,CString &dest,int con_id,int type)
{
	CString tmpstr;
	token="REQ_BASICSCHEDULE";
	dest="<REQ_BASICSCHEDULE>"; 
	char dispid[10];
	if(GetTGFunctionConfig()->GetTmisSectionID(con_id,dispid)<0)
	{
		AfxMessageBox("无法获取TMIS的调度台简码");
		return FALSE;
	}
	InsertXMLNodeValue(dest,"DISID",dispid);
	InsertXMLNodeValue(dest,"SCHTYP",type);
	dest+="</REQ_BASICSCHEDULE>";
	return TRUE;
}

BOOL TD_GenAckMessage_GPCACKINFO(CString &token,CString &dest,int con_id,int type,bool flag,CString name, long src, CString msgid, CString jht, CString result)
{
	
	token="GPC_ACK_INFO";
	dest="<GPC_ACK_INFO>"; 

	CString tmname,msgidname;
	tmname.Format("%s",gpcdef.GetSectionName(con_id));
	if(tmname=="")
	{
		AfxMessageBox("无法获取调度台号");
		return FALSE;
	}
	
	
	InsertXMLNodeValue(dest,"DSID",gpcdef.GetSectionName(con_id));
	InsertXMLNodeValue(dest,"TYPE",type);
	InsertXMLNodeValue(dest,"RSLT",0);
	InsertXMLNodeValue(dest,"SRCCODE",src);
	InsertXMLNodeValue(dest,"MSGID",msgid);
	InsertXMLNodeValue(dest,"DPNM",name);
	InsertXMLNodeValue(dest,"SGID",0);
	InsertXMLNodeValue(dest,"JHTNM",jht);

	if (flag)
		InsertXMLNodeValue(dest,"CONTENT","签收成功!");
	else
	{
		CString tmpstr;
		tmpstr.Format("未签收, 失败原因:\n %s", result);
		InsertXMLNodeValue(dest,"CONTENT",tmpstr);
	}
	dest+="</GPC_ACK_INFO>";
	return TRUE;
}