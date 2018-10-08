// TgDocument.cpp : 实现文件
//

#include "stdafx.h"
#include "sysshare.h"
#include "TgDocument.h"
#include "function.h"
#include "icommtask.h"
#include "utility/xml_utility.h"
#include "comutil.h"
#include "utility/ctc_data_type_def.h"
#include "WsMaintanPlan.h"
#include "MyWsDCMD.h"

#import  <msxml.dll> named_guids
using namespace MSXML;

extern BOOL GenDcmdFromXML(MSXML::IXMLDOMNodePtr pNodeRoot, int conid, CTD_MPLANDCMD* dcmd);

void LoadMMPlanCmdSaveList();

char g_list_node_from_ws[]="CASCOMMIPLAN/diffgr:diffgram/ds_MaintainPlan_Index_List/MAINTAIN_PLAN_INDEX";
char g_info_node_from_ws[]="CASCOMMIPLAN/diffgr:diffgram/ds_MaintainPlan_Index/MAINTAIN_PLAN_INDEX/INFO";
///////// 施工计划 /////
static WS_MaintainPlan::CWS_MaintainPlan *g_pWS_MMPlanservice=NULL;


#define FAILED_WS_CALL(x)  {;}

static WS_DCMD::CWS_DCMD *g_pWS_DCMDservice=NULL;

void    CMMPlanListInfo::SetID(const char *s)
{
	m_id=s;
	m_currentID=GetMainPlanID();
}
int   CMMPlanListInfo::GetSubPlanIDs(CStringArray &subids)
{
	return ConvertToSubStringArray(m_id,',',subids);
}

CString CMMPlanListInfo::GetMainPlanID() const
{
	int x=m_id.Find(',');
	if(x<0)
		return m_id;
	else
		return m_id.Left(x);
}

void  CMMPlanListInfo::GenListInfoXML(CString &dest,const char *node)
{
	if(node)
	{
		dest+="<";dest+=node;dest+=">";
	}
	InsertXMLNodeValue(dest,"PLANID",GetMainPlanID());
	InsertXMLNodeValue(dest,"SUBID",GET_JHH_FROM_PLANID(GetCurrentSubID()));
	InsertXMLNodeValue(dest,"TM","");
	InsertXMLNodeValue(dest,"WORKPOS",m_topic);
	InsertXMLNodeValue(dest,"TRNINFO",m_trninfo);
	if(node)
	{
		dest+="</";dest+=node;dest+=">";
	}
}

const char *CMMPlanListInfo::GetSubIDByJHH(long jhh)
{
	static CString tmpstr;
	CStringArray subids;
	GetSubPlanIDs(subids);
	int subindex;
	for(subindex=0;subindex<subids.GetCount();subindex++)
	{
		if(atoi(GET_JHH_FROM_PLANID(subids.GetAt(subindex)))==jhh)
		{
			tmpstr=subids.GetAt(subindex);
			return (const char *)tmpstr;
		}
	}
	return NULL;
}

BOOL  CTgDocument::InitWS()
{
	HRESULT hr;
	bool result2;

	g_pWS_DCMDservice=new WS_DCMD::CWS_DCMD(GetTGFunctionConfig()->GetWSDCMDurl1());

	hr=g_pWS_DCMDservice->IsReady(&result2);
	if(FAILED(hr)  || result2==FALSE)
	{
		delete g_pWS_DCMDservice;
		g_pWS_DCMDservice=new WS_DCMD::CWS_DCMD(GetTGFunctionConfig()->GetWSDCMDurl2());
		hr=g_pWS_DCMDservice->IsReady(&result2);
	}

	if(GetTGFunctionConfig()->IsMMPlanWSEnabled())
	{
		g_pWS_MMPlanservice=new WS_MaintainPlan::CWS_MaintainPlan();
		bool result;
		hr=g_pWS_MMPlanservice->IsReady(&result);
		if(FAILED(hr) || result==FALSE)
		{
			AfxMessageBox("WEB 服务初始化不成功，将导致施工计划无法保存!");
			delete g_pWS_MMPlanservice;
			g_pWS_MMPlanservice=NULL;
		}
	}

	return TRUE;
}

BOOL  CTgDocument::DeleteWS()
{
	delete g_pWS_DCMDservice;
	g_pWS_DCMDservice=NULL;

	delete g_pWS_MMPlanservice;
	g_pWS_MMPlanservice =NULL;

	return TRUE;
}

BOOL  CheckDCMDService()
{
	if(g_pWS_DCMDservice==NULL)
		return FALSE;
	else
	{
		bool result_test;
		g_pWS_DCMDservice->IsReady(&result_test);
		return TRUE;
	}
}
BOOL  CheckMMPlanService()
{
	if(g_pWS_MMPlanservice==NULL)
		return FALSE;
	else
		return TRUE;
}

int   CTgDocument::AddMMPlan(const char* id,BOOL frommsg)
{
	int i;
	for(i=0;i<m_nCountOfPlan;i++)
	{
		if(m_cMMplans[i]->GetID()==id)
			return i;
	}
	if(i<MAX_MMPLAN_NUM)
	{
		m_cMMplans[i]=new CMMPlanListInfo;
		m_cMMplans[i]->SetID(id);
		m_cMMplans[i]->m_bUpdatedFromMsg=frommsg;
		m_nCountOfPlan++;
		return i;
	}	
	return -1;
}


int   CTgDocument::FindMMPlan(const char *id)
{
	int i;
	for(i=0;i<m_nCountOfPlan;i++)
	{
		if(m_cMMplans[i]->GetID()==id)
			return i;
	}
	return -1;
}
int   CTgDocument::FindMMPlanByMainID(const char *id)
{
	int i;
	for(i=0;i<m_nCountOfPlan;i++)
	{
		if(m_cMMplans[i]->GetMainPlanID()==id)
			return i;
	}
	return -1;
}

int   CTgDocument::FindMMPlanByCurrentSubID(const char *id)
{
	int i;
	for(i=0;i<m_nCountOfPlan;i++)
	{
		if(m_cMMplans[i]->GetCurrentSubID()==id)
			return i;
	}
	return -1;
}

int   CTgDocument::FindMMPlanBySubID(const char *id)
{
	int i;
	for(i=0;i<m_nCountOfPlan;i++)
	{
		CStringArray st;
		m_cMMplans[i]->GetSubPlanIDs(st);
		for(int j=0;j<st.GetCount();j++)
			if(st[j]==id)
				return i;

	}
	return -1;
}

void  CTgDocument::ClearAllMMPlan()
{
	for(int i=0;i<m_nCountOfPlan;i++)
		delete m_cMMplans[i];
	m_nCountOfPlan=0;
	m_cMMplanMarks.clear();
}
int CTgDocument::GetMPlanCount()
{
	return m_nCountOfPlan;
}

CMMPlanListInfo *CTgDocument::GetMPlanPtr(int i)
{
	if(i<0 || i>=m_nCountOfPlan)
		return NULL;
	return m_cMMplans[i];
}

int  CTgDocument::GetMMPlanState(CString planid)
{
	for(int i=0; i<m_nCountOfPlan; i++)
	{
		if(m_cMMplans[i]->GetID() == planid)
		{
			return m_cMMplans[i]->m_nStatus;
		}
	}
	return 0;
}


BOOL  CTgDocument::WriteMPLANFile(const char *planid,const char *node,const char *xml)
{
	CString  filenameprefix;
	filenameprefix.Format("MMPLAN-%s",planid);
	return WriteXMLFile(xml,node,filenameprefix);
}

BOOL  CTgDocument::ReadMPLANFile(CString &xml,const char *planid,const char *node)
{
	CString  filenameprefix;
	filenameprefix.Format("MMPLAN-%s",planid);
	return ReadXMLFile(xml,node,filenameprefix);
}

/*
<?xml version="1.0" standalone="yes"?>
<ds_MaintainPlan_Index_List xmlns="http://tempuri.org/ds_MaintainPlan_Index_List.xsd">
<MAINTAIN_PLAN_INDEX>
<ID>22222222-2222-3333-444444444444</ID>
<PLAN_ID>55555555-6666-6666-121212121212</PLAN_ID>
<STATUS>1</STATUS>
<TITLE>这是标题字段，256个字节那么长</TITLE>
<MAINTAIN_DATE>2009-05-21T00:00:00+08:00</MAINTAIN_DATE>
</MAINTAIN_PLAN_INDEX>
<MAINTAIN_PLAN_INDEX>
<ID>22222222-2222-3333-555555555555</ID>
<PLAN_ID>55555555-6666-6666-121212121212</PLAN_ID>
<STATUS>1</STATUS>
<TITLE>？？？?？？？？?Σ？256？？？?？？？？？？</TITLE>
<MAINTAIN_DATE>2009-05-01T00:00:00+08:00</MAINTAIN_DATE>
</MAINTAIN_PLAN_INDEX>
</ds_MaintainPlan_Index_List>
*/

BOOL CTgDocument::LoadMMListFromWS()
{
	return LoadMMListFromFile();
}

/*
<?xml version="1.0" standalone="yes"?>
<ds_MaintainPlan_Index xmlns="http://tempuri.org/ds_MaintainPlan_Index.xsd">
<MAINTAIN_PLAN_INDEX>
<ID>22222222-2222-3333-444444444444</ID>
<PLAN_ID>55555555-6666-6666-121212121212</PLAN_ID>
<STATUS>1</STATUS>
<TITLE>这是标题字段，256个字节那么长</TITLE>
<MAINTAIN_DATE>2009-05-21T00:00:00+08:00</MAINTAIN_DATE>
<INFO>这是CLOB格式的大数据字段，可以填写过万字节的内容。收到以后，按字符串处理。                                   </INFO>
</MAINTAIN_PLAN_INDEX>
</ds_MaintainPlan_Index>
*/

BOOL CTgDocument::LoadMMInfoFromWS(CString &dest,const char *idstr)
{
	if(!CheckMMPlanService())
		return FALSE;
	BSTR condition;
	BSTR result_dataset;

	//LoadPlanData("");
	condition=_com_util::ConvertStringToBSTR(idstr);
	HRESULT hr=g_pWS_MMPlanservice->Get_MaintainIndex_By_ID(condition,&result_dataset);
	if(FAILED(hr))
	{
		AfxMessageBox("WEB 服务调用不成功!");
		FAILED_WS_CALL(g_pWS_MMPlanservice);
		return FALSE;
	}

	//hr=pservice->Update_MaintainPlan2(result,&result_info); 
	const char *result_str=_com_util::ConvertBSTRToString(result_dataset);
	dest="<CASCOMMIPLAN> ";
	dest+=result_str;
	dest+="</CASCOMMIPLAN> ";

	//CString result_str;

	return TRUE;
}

/*
<?xml version="1.0" standalone="yes"?>
<ds_MaintainPlan_Index xmlns="http://tempuri.org/ds_MaintainPlan_Index.xsd">
<MAINTAIN_PLAN_INDEX>
<ID>22222222-2222-3333-555555555555</ID>
<PLAN_ID>55555555-6666-6666-121212121212</PLAN_ID>
<STATUS>1</STATUS>
<TITLE>这是标题字段，256个字节那么长</TITLE>
<MAINTAIN_DATE>2009-05-01T00:00:00+08:00</MAINTAIN_DATE>
<starttime>2009-05-11T00:00:00+08:00</starttime>
<endtime>2009-05-20T00:00:00+08:00</endtime>
<INFO>这是CLOB格式的大数据字段，可以填写过万字节的内容。收到以后，按字符串处理。</INFO>
</MAINTAIN_PLAN_INDEX>
</ds_MaintainPlan_Index>
*/
BOOL CTgDocument::SaveMMInfoToWS(int index)
{
	BSTR condition;

	WS_MaintainPlan::Result result_info;
	CString dataxml;
	CString info;
	CMMPlanListInfo *pMMplan=GetMPlanPtr(index); 
	if(!pMMplan)
		return FALSE;
	CStringArray subids;
	pMMplan->GetSubPlanIDs(subids);
	for(int subindex=0;subindex<subids.GetCount();subindex++)
	{
		if(!ReadMPLANFile(info,subids[subindex] ,XMLTYPE_MPLAN_INFO))
		{
			// 出错处理？
			continue;
		}
	}

	CString   strguid=GenGUIDString();

	dataxml="<?xml version=\"1.0\" standalone=\"yes\"?>";
	dataxml+="<ds_MaintainPlan_Index xmlns=\"http://tempuri.org/ds_MaintainPlan_Index.xsd\">";
	dataxml+="<MAINTAIN_PLAN_INDEX>";


	dataxml+="<ID>"; dataxml+=strguid; dataxml+="</ID>";
	dataxml+="<PLAN_ID>"; dataxml+=pMMplan->GetMainPlanID(); dataxml+="</PLAN_ID>";
	CString markinfo;
	
	dataxml+="<TITLE>"; 
	XMLCopyValueString(dataxml,markinfo); 
	dataxml+="</TITLE>";
	
	dataxml+="<INFO>"; 
	XMLCopyValueString(dataxml,info);
	dataxml+="</INFO>";
	dataxml+="</MAINTAIN_PLAN_INDEX>";
	dataxml+="</ds_MaintainPlan_Index>";

	condition=_com_util::ConvertStringToBSTR(dataxml);

	HRESULT hr=g_pWS_MMPlanservice->Update_MaintainIndex(condition,&result_info);
	if(FAILED(hr))
	{
		AfxMessageBox("WEB 服务调用不成功!");
		FAILED_WS_CALL(g_pWS_MMPlanservice);
		return FALSE;
	}
	if(result_info.succeed==false)
	{
		CString errinfo=_com_util::ConvertBSTRToString(result_info.message);
		AfxMessageBox(errinfo);
		return FALSE;
	}

	return TRUE;
}

//=================================================================
//                 处理服务器来的施工信息
//=================================================================
BOOL   CTgDocument::ProcessXML_MMPlanMsgFromTD(const char *xml,const char *node)
{
	GpcLogTrainMessage(0,0,"处理收到的施工信息conid %d token: %s", m_nCurrentSectionNo, node);
	if(!stricmp(node,XMLTYPE_MPLAN_LIST))
		return ProcessXML_Msg_List(xml,node, true, m_nCurrentSectionNo);

	else if(!stricmp(node,XMLTYPE_MPLAN_MARK))
		return ProcessXML_Msg_Mark(xml,node,true, m_nCurrentSectionNo);

	else if(!stricmp(node,XMLTYPE_MPLAN_INFO))
		return ProcessXML_Msg_SG_WX(xml,node,true, m_nCurrentSectionNo);

	else if(!stricmp(node,XMLTYPE_MPLAN_JIESHI))
		return ProcessXML_Msg_WKDES(xml,node,true, m_nCurrentSectionNo);
	
	else if(!stricmp(node,XMLTYPE_MPLAN_DCMD))
		return ProcessXML_Msg_DCMD(xml,node,true, m_nCurrentSectionNo);
	
	else if(!stricmp(node, "TD_RESULT"))
		return ProcessXML_Msg_TD_RESULT(xml,node);
	
	return TRUE;
}

// 施工符号
BOOL CTgDocument::ProcessXML_Msg_Mark(const char *xml,const char *node,bool bIsFromTD, int conid)
{
	BOOL rt=FALSE;
	CTD_MPlanMarkList marklist;
	if(bIsFromTD)
	   rt=marklist.LoadMarkFromTDXML(xml,node);
	else
	   rt=marklist.LoadMarkFromCADXML(xml,node,conid);

	if(!rt)
	{
		GpcLogTrainMessage(0, 0, "ProcessXML_Msg_Mark 处理施工标记的XML失败 TD:%d", bIsFromTD);
		AfxMessageBox("处理施工标记的XML错误");

		return FALSE;
	}
	char dispid[10];
	if(GetTGFunctionConfig()->GetTmisSectionID(conid,dispid)<0)
	{
		GpcLogTrainMessage(0, 0, "无法获取区段 %d 的调度台简码,检查tg_logic.cfg中配置[TD_SECTION]", conid);
		return FALSE;
	}
	marklist.mark_con_id=conid;
	marklist.strDisID=dispid;

	for(int i=0; i<marklist.m_nCount&&i<MAX_MPLAN_MARK_NUM; i++)
	{		
		AddMPlanMark(&marklist.m_list[i]);
	}

	GenMarkFromMMPlanByPlanId(1);  //仅班计划

	MMplanInform inform;
	inform.change_flag=MMplanInform::MMPLAN_LIST_CHANGED;
	inform.index=-1;	
	UpdateAllViews(NULL,TGDATA_MPLAN_UPDATED,&inform);
	SaveMMMarkToFile();
	return TRUE;
}

// 施工符号中间结果列表
BOOL CTgDocument::ProcessXML_Msg_List(const char *xml,const char *node,bool bIsFromTD, int conid)
{
	BOOL rt=FALSE;
	CTD_MPlanMidList midlist;
	if(bIsFromTD)
	   rt=midlist.LoadRSINFOFromTDXML(xml,node,conid);
	else
	   rt=midlist.LoadRSINFOFromCADXML(xml,node,conid);

	if(!rt)
	{
		GpcLogTrainMessage(0, 0, "ProcessXML_Msg_List 处理施工标记的XML失败 TD:%d", bIsFromTD);
		AfxMessageBox("处理施工标记的XML错误");

		return FALSE;
	}

	CString  plan_id;

	for(int i=0;i<midlist.m_nCount;i++)
	{
		plan_id=midlist.m_list[i].PLANID;
		int index=FindMMPlan(plan_id);
		if(index<0)
		{
			index=AddMMPlan(plan_id,TRUE);
			if(index<0)
			{
				AfxMessageBox("施工计划太多无法处理");
				return FALSE;
			}
		}
		m_cMMplans[index]->m_bUpdatedFromMsg=TRUE;
		m_cMMplans[index]->m_nFlag=midlist.m_list[i].FLAG;
		m_cMMplans[index]->m_topic=midlist.m_list[i].PNM; 
		m_cMMplans[index]->m_trninfo=midlist.m_list[i].TRNINFO; 
		m_cMMplans[index]->m_worktime=midlist.m_list[i].TM; //2010.3.20
		if(!midlist.m_list[i].WKDES.IsEmpty())
		{
			CString jieshi;
			jieshi="<";
			jieshi+=XMLTYPE_MPLAN_JIESHI;
			jieshi+=">";	
			XMLCopyValueString(jieshi,midlist.m_list[i].WKDES); 
			jieshi+="</";
			jieshi+=XMLTYPE_MPLAN_JIESHI;
			jieshi+=">";	
			WriteMPLANFile(plan_id,XMLTYPE_MPLAN_JIESHI,jieshi);
		}
		if(!midlist.m_list[i].TRNINFO.IsEmpty())
		{
			CString trninfo;
			trninfo="<";
			trninfo+=XMLTYPE_MPLAN_TRNINFO;
			trninfo+=">";	
			CStringArray subitems;
			int subitemcount=ConvertToSubStringArray(midlist.m_list[i].TRNINFO,'@',subitems);
			for(int subindex=0;subindex<subitemcount;subindex++)
			{
				trninfo+="<RECORD>";
				CStringArray fields;
				int fcnt=ConvertToSubStringArray(subitems.ElementAt(subindex),',',fields);
				InsertXMLNodeValue(trninfo,"ID",subindex+1); 
				if(fcnt>0)
					InsertXMLNodeValue(trninfo,"DPTNO",fields.GetAt(0)); 
				if(fcnt>1)
					InsertXMLNodeValue(trninfo,"DPTST",fields.GetAt(1));
				if(fcnt>2)
					InsertXMLNodeValue(trninfo,"ARVST",fields.GetAt(2));
				if(fcnt>3)
					InsertXMLNodeValue(trninfo,"ARVNO",fields.GetAt(3));
				if(fcnt>4)
					InsertXMLNodeValue(trninfo,"STPTM",fields.GetAt(4));
				if(fcnt>5)
					InsertXMLNodeValue(trninfo,"INFO",fields.GetAt(5));
				trninfo+="</RECORD>";
			}
			trninfo+="</";
			trninfo+=XMLTYPE_MPLAN_TRNINFO;
			trninfo+=">";	
			WriteMPLANFile(plan_id,XMLTYPE_MPLAN_TRNINFO,trninfo);
		}
	}

	MMplanInform inform;
	inform.change_flag=MMplanInform::MMPLAN_LIST_CHANGED;
	inform.index=-1;	
	UpdateAllViews(NULL,TGDATA_MPLAN_UPDATED,&inform);
	SaveMMListToFile();
	return TRUE;	 
}

// 维修信息
BOOL CTgDocument::ProcessXML_Msg_SG_WX(const char *xml,const char *node,bool bIsFromTD,int conid)
{
	BOOL rt=FALSE;
	CTD_MPLAN_SG_WX sgwx;
	if(bIsFromTD)
		rt=sgwx.LoadSG_WXFromTDXML(xml, node);
	else
		rt=sgwx.LoadSG_WXFromCADXML(xml, node, conid);

	if(!rt)
	{
		GpcLogTrainMessage(0, 0, "ProcessXML_Msg_SG_WX 处理施工标记的XML失败 TD:%d", bIsFromTD);
		AfxMessageBox("处理施工标记的XML错误");
		return FALSE;
	}
	char dispid[10];
	if(GetTGFunctionConfig()->GetTmisSectionID(conid,dispid)<0)
	{
		GpcLogTrainMessage(0, 0, "无法获取区段 %d 的调度台简码,检查tg_logic.cfg中配置[TD_SECTION]", conid);
		return FALSE;
	}
	sgwx.m_con_id=conid;
	sgwx.DISID=dispid;

	CString plan_id=sgwx.PLANID;
	if(plan_id.IsEmpty())
	{
		GpcLogTrainMessage(0, 0, "施工维修计划号为:%s是空,不能处理", plan_id);
		return FALSE;
	}

	int index=FindMMPlanBySubID(plan_id);
	if(index>=0)
	{
		m_cMMplans[index]->m_bUpdatedFromMsg=TRUE;
		WriteMPLANFile(plan_id,node,xml);
		MMplanInform inform;
		inform.change_flag=MMplanInform::MMPLAN_INFO_CHANGED;
		inform.index=index;	
		inform.id=plan_id;
		UpdateAllViews(NULL,TGDATA_MPLAN_UPDATED,&inform);
	}
	return TRUE;
}

// 揭示
BOOL CTgDocument::ProcessXML_Msg_WKDES(const char *xml,const char *node,bool bIsFromTD,int conid)
{
	BOOL rt=FALSE;
	CMPLAN_WKDES wkdes;
	if(bIsFromTD)
		rt=wkdes.LoadWKDESFromTDXML(xml,node);
	else
		rt=wkdes.LoadWKDESFromCADXML(xml,node,conid);

	if(!rt)
	{
		GpcLogTrainMessage(0, 0, "ProcessXML_Msg_WKDES 处理施工标记的XML失败 TD:%d", bIsFromTD);
		AfxMessageBox("处理施工标记的XML错误");
		return FALSE;
	}
	char dispid[10];
	if(GetTGFunctionConfig()->GetTmisSectionID(conid,dispid)<0)
	{
		GpcLogTrainMessage(0, 0, "无法获取区段 %d 的调度台简码,检查tg_logic.cfg中配置[TD_SECTION]", conid);
		return FALSE;
	}
	wkdes.m_nSectionID=conid;
	wkdes.strDisID=dispid;
	
	CString  plan_id=wkdes.PLANID;
	if(plan_id.IsEmpty())
		return FALSE;

	int index=FindMMPlanBySubID(plan_id);
	if(index>=0)
	{
		m_cMMplans[index]->m_bUpdatedFromMsg=TRUE;
		WriteMPLANFile(plan_id,node,xml);
		MMplanInform inform;
		inform.change_flag=MMplanInform::MMPLAN_INFO_CHANGED;
		inform.index=index;	
		inform.id=plan_id;
		UpdateAllViews(NULL,TGDATA_MPLAN_UPDATED,&inform);
	}
	return TRUE;
}
// 施工命令信息
BOOL CTgDocument::ProcessXML_Msg_DCMD(const char *xml,const char *node,bool bIsFromTD,int conid)
{
	BOOL rt=FALSE;
	CTD_MPlanDCMDList dcmdlist;
	if(bIsFromTD)
		rt=dcmdlist.LoadDcmdFromTDXML(xml,node,conid);
	else
		rt=dcmdlist.LoadDcmdFromCADXML(xml,node,conid);

	if(!rt)
	{
		GpcLogTrainMessage(0, 0, "ProcessXML_Msg_DCMD 处理施工标记的XML失败 TD:%d", bIsFromTD);
		AfxMessageBox("处理施工标记的XML错误");
		return FALSE;
	}
	char dispid[10];
	if(GetTGFunctionConfig()->GetTmisSectionID(conid,dispid)<0)
	{
		GpcLogTrainMessage(0, 0, "无法获取区段 %d 的调度台简码,检查tg_logic.cfg中配置[TD_SECTION]", conid);
		return FALSE;
	}
	dcmdlist.m_nSectionID=conid;
	dcmdlist.strDisID=dispid;

	int index=-1;
	for(int i=0;i<dcmdlist.m_nCount&&i<MAX_MPLAN_DCMD_NUM;i++)
	{
		index=FindMMPlanByMainID(dcmdlist.m_list[i].PLANID);
		if(index>=0)
		{
			m_cMMplans[index]->m_bUpdatedFromMsg=TRUE;
		}
	}

	if(index>=0)
	{
		WriteMPLANFile(m_cMMplans[index]->GetMainPlanID(),node,xml);

		MMplanInform inform;
		inform.change_flag=MMplanInform::MMPLAN_DCMD_CHANGED;
		inform.index=index;	
		inform.id=m_cMMplans[index]->GetMainPlanID();
		UpdateAllViews(NULL,TGDATA_MPLAN_UPDATED,&inform);
	}
	return TRUE;
}
// TD返回处理结果
BOOL CTgDocument::ProcessXML_Msg_TD_RESULT(const char *xml,const char *node)
{
	MMplanInform inform;
	LOAD_XML(xml,node,pNodeRoot);
	GET_NODE_STRVALUE(inform.token, pNodeRoot,"ACTION_TYPE")
	GET_NODE_STRVALUE(inform.id, pNodeRoot,"ACTION_DATA")
	GET_NODE_INTVALUE(inform.index, pNodeRoot,"ISCANCLE")
	GET_NODE_INTVALUE(inform.nResult, pNodeRoot,"SUCCESS")
	
	inform.change_flag=MMplanInform::MMPLAN_WORK_STATE_CHANGE;
		
	UpdateAllViews(NULL,TGDATA_MPLAN_UPDATED,&inform); // TD返回施工状态
	
	return TRUE;
}

//===========================================================================
void CTgDocument::SaveMMPlanToWS()
{
	for(int i=0;i<m_nCountOfPlan;i++)
	{
		if(m_cMMplans[i] && m_cMMplans[i]->m_bUpdatedFromMsg)
			SaveMMInfoToWS(i);
	}
}

BOOL CTgDocument::ReadDCMDFromFiles(int index,CString *pdest,CStringArray *parray)
{
	CMMPlanListInfo *pMMplan=GetMPlanPtr(index); 
	if(!pMMplan)
		return FALSE;
	CFileFind find;
	CString   target;
	target.Format("%s/MMPLAN-%s-%s*.xml",GetXMLFilePath(),pMMplan->GetMainPlanID(),XMLTYPE_MPLAN_DCMD);
	if(find.FindFile(target))
	{
		BOOL  ffrc=TRUE;
		CTime ctm = CTime::GetCurrentTime();
		ULONG create_time = ctm.GetTime();
		while(ffrc)
		{
			ffrc=find.FindNextFile();
			CString dcmd;
			CTD_MPLANDCMD dcmdobj; 
			ReadXMLFile(dcmd,find.GetFileTitle());
			LOAD_XML(dcmd,XMLTYPE_MPLAN_DCMD,pRootNode);
			GenDcmdFromXML(pRootNode,GetCurrentSectionNo(),&dcmdobj);
			
			CString dcmdsub;
			dcmdobj.SaveToXML(dcmdsub,"DCMD", create_time++, dcmdobj.CMDTP, dcmdobj.SUBTP); // DCMD和WS中规定的一致
			if(pdest)
				*pdest+=dcmdsub;
			if(parray)
				parray->Add(dcmdsub);
		}
	}
	find.Close();
	return TRUE;
}
#define MAX_DCMD_SAVED_LIST 1024
static struct{
	char  plan_id[32];
	char  cmd_index[32];
	time_t  save_time;
}g_cmd_save_list[MAX_DCMD_SAVED_LIST];
int g_cmd_save_coun=0;

BOOL IsDCMDSaved(const char *planid,const char *cmdindex)
{
	for(int i=0;i<g_cmd_save_coun;i++)
	{
		if(!stricmp(g_cmd_save_list[i].cmd_index,cmdindex))
			return TRUE;
	}
	return FALSE;
}

void SetDCMDSaved(const char *planid,const char *cmdindex)
{
	int i;
	for(i=0;i<g_cmd_save_coun;i++)
	{
		if(!stricmp(g_cmd_save_list[i].plan_id,planid) && !stricmp(g_cmd_save_list[i].cmd_index,cmdindex))
			break;
	}
	if(i>=g_cmd_save_coun)
	{
		if(g_cmd_save_coun>=MAX_DCMD_SAVED_LIST)
			return;
		g_cmd_save_coun++;
		STR_CPY(g_cmd_save_list[i].plan_id,planid);
		STR_CPY(g_cmd_save_list[i].cmd_index,cmdindex);
	}
	CTime curtm=CTime::GetCurrentTime();
	g_cmd_save_list[i].save_time=curtm.GetTime();
}

void LoadMMPlanCmdSaveList()
{
	CString filename;
	g_cmd_save_coun=0;
	filename.Format("%s/xml/cmd_save_list.sav",gpcdef.GetDataDir());
	FILE *fp;
	char plan_id[32];
	char cmd_index[32];
	int  save_time;
	char line[128];
	fp=fopen(filename,"rt");
	if(!fp)
		return;
	CTime curtm=CTime::GetCurrentTime();
	while(fgets(line,128,fp))
	{
		if(sscanf(line,"%s %s %d",plan_id,cmd_index,&save_time)==3)
		{
			if(g_cmd_save_coun<MAX_DCMD_SAVED_LIST && save_time>(curtm.GetTime()-48))
			{
				STR_CPY(g_cmd_save_list[g_cmd_save_coun].plan_id,plan_id);
				STR_CPY(g_cmd_save_list[g_cmd_save_coun].cmd_index,cmd_index);
				g_cmd_save_list[g_cmd_save_coun].save_time=save_time;
				g_cmd_save_coun++;
			}
		}
	}

}

void SaveMMPlanCmdSaveList()
{
	CString filename;
	filename.Format("%s/xml/cmd_save_list.sav",gpcdef.GetDataDir());
	FILE *fp;
	fp=fopen(filename,"wt");
	if(!fp)
		return;
	for(int i=0;i<g_cmd_save_coun;i++)
	{
		fprintf(fp,"%s %s %d\n",g_cmd_save_list[i].plan_id,g_cmd_save_list[i].cmd_index,g_cmd_save_list[i].save_time);
	}
	fclose(fp);
}

int CTgDocument::SaveDCMDToWS(const char *planid)
{
	int i=FindMMPlanByMainID(planid);
	if(i>=0)
		return SaveDCMDToWS(i);
	else
		return -1;
}

// -2:调用webserver失败 -1:没有调度命令 0:发送失败 1:转发成功
int CTgDocument::SaveDCMDToWS(int i)
{
	if(!CheckDCMDService())
		return -2;

	CString header;
	header="<?xml version=\"1.0\" standalone=\"yes\"?>";
	header+="<ds_dcmd xmlns=\"http://tempuri.org/ds_dcmd.xsd\">";

	CString dcmd;
	CMMPlanListInfo *pMMplan=GetMPlanPtr(i); 
	if(!pMMplan)
		return -1;
	if(!ReadMPLANFile(dcmd,pMMplan->GetMainPlanID(),XMLTYPE_MPLAN_DCMD))
		return -1;
	LOAD_XML(dcmd,XMLTYPE_MPLAN_DCMD,pRootNode);

	MSXML::IXMLDOMNodeListPtr pRecAbsList = pRootNode->selectNodes(_bstr_t("RECORD"));//ds_MaintainPlan_List/
	int rec_count = pRecAbsList->Getlength();
	int rc=1;

	CTime ctm = CTime::GetCurrentTime();
	ULONG create_time = ctm.GetTime();
	for(int i=0;i<rec_count;i++)
	{
		CTD_MPLANDCMD dcmd;
		GenDcmdFromXML(pRecAbsList->Getitem(i), GetCurrentSectionNo(), &dcmd);
		if(IsDCMDSaved(dcmd.PLANID,dcmd.CMDINDEX))
			continue;
		CString subdcmd;
		dcmd.SaveToXML(subdcmd,"DCMD", create_time+i, dcmd.CMDTP, dcmd.SUBTP); 
		CString dataxml=header;
		dataxml+=subdcmd;
		dataxml+="</ds_dcmd>";

		BSTR bstr_xml;

		WS_DCMD::Result result_info;
	
		bstr_xml=_com_util::ConvertStringToBSTR(dataxml); 
		HRESULT hr=g_pWS_DCMDservice->Update_Dcmd_By_XML(bstr_xml,&result_info);
		if(FAILED(hr))
		{  
		 	FAILED_WS_CALL(g_pWS_DCMDservice);
			rc=-2;
			break; // 只要有一条命令调用WebService失败, 就认为该施工计划转存命令失败
		}
 		else
		{
			if(result_info.succeed==true)
			{
				SetDCMDSaved(dcmd.PLANID,dcmd.CMDINDEX);
				rc=1;
			}
			else
			{
				CString cap=_com_util::ConvertBSTRToString(result_info.message);
				GpcLogTrainMessage(0, 0, "施工计划转存调度命令失败:%s",  cap);

				AfxMessageBox(cap);
				rc=0;
				break; // 只要有一条命令转存失败, 就认为该施工计划转存命令失败
			}
		}
	}
	if(rc==1)
	{
		SaveMMPlanCmdSaveList();
	}
	return rc;
}

BOOL  CTgDocument::RequestMMPlanInfoFromOPMS(int index,int mask)
{
	for(int i=0;i<m_nCountOfPlan;i++)
	{
		if(index>=0 && index!=i)
			continue;
		CMMPlanListInfo *pMMplan=GetMPlanPtr(i); 
		if(!pMMplan)
			return FALSE;
		time_t nDay=pMMplan->m_worktime;
		CString token,dest;
		long  eid=GetSpecialEntityID(SERVER_TYPE_TDCOMM);
		if (gpcdef.IsEnableMMPlanFromCAD())
			eid=GetSpecialEntityID(SERVER_TYPE_MMPLAN);
		if(mask & 0x01)
		{
			CStringArray subids;
			pMMplan->GetSubPlanIDs(subids);
			for(int subindex=0;subindex<subids.GetCount();subindex++)
			{
				if(TD_GenRequestMessage_17_SG_WX_INFO(token,dest,GetCurrentSectionNo(),nDay,subids[subindex],pMMplan->m_nFlag))
				{
					TGSendStringMessage(token,dest,eid);
					GpcLogTrainMessage(0, 0, "向OPMS获取施工维修信息:eid:%d token:%s xml:%s", 
					eid, token, dest);
				}
			}
		}
		if(mask & 0x02)
		{
			if(TD_GenRequestMessage_12_CMD(token,dest,GetCurrentSectionNo(),nDay,pMMplan->GetMainPlanID()))
			{
				TGSendStringMessage(token,dest,eid);
				GpcLogTrainMessage(0, 0, "向OPMS获取施工调令:eid:%d token:%s xml:%s", 
					eid, token, dest);
			}
		}
		if(mask & 0x04)
		{
			if(TD_GenRequestMessage_18_WKDES(token,dest,GetCurrentSectionNo(),nDay,pMMplan->GetMainPlanID()))
			{
				TGSendStringMessage(token,dest,eid);
				GpcLogTrainMessage(0, 0, "向OPMS获取施工揭示:eid:%d token:%s xml:%s", 
					eid, token, dest);
			}
		}
	}
	return TRUE;
}

BOOL  CTgDocument::RequestMMPlanFromOPMS(time_t start,time_t end,CView *pv, long eid)
{
	if(start==0)
		start=m_tMMplanStartTime;
	if(end==0)
		end=m_tMMplanEndTime;

	CTime cst(start);
	CTime cet(end);
	GpcLogTrainMessage(0, 0, "获取施工计划时间范围 %s -- %s", cst.Format("%Y-%m-%d %H:%M:%S"), cet.Format("%Y-%m-%d 23:59:59"));

	// 保存施工状态
	ReadMMPlanListStateFromFile();
	if(planidcancel.IsEmpty())
	{
		GpcLogTrainMessage(0, 0, "没有 取消施工");
	}

	ClearAllMMPlan();
	
	CString token,dest;
	
	if(TD_GenRequestMessage_14_RSINFO(token,dest,GetCurrentSectionNo(),cst.GetTime()))
	{
		TGSendStringMessage(token,dest,eid);
		GpcLogTrainMessage(0, 0, "向TD(%d)请求 %s %s", eid, token, dest);
	}
	if(TD_GenRequestMessage_13_WKINFO(token,dest,GetCurrentSectionNo(),cst.GetTime()))
	{
		TGSendStringMessage(token,dest,eid);
		GpcLogTrainMessage(0, 0, "向TD(%d)请求 %s %s", eid, token, dest);
	}

	if(cst.GetDay()!=cet.GetDay())
	{
		CTime endtm(cet.GetYear(), cet.GetMonth(), cet.GetDay(), 0, 0,0);
		if(TD_GenRequestMessage_14_RSINFO(token,dest,GetCurrentSectionNo(),endtm.GetTime() ))
		{
			TGSendStringMessage(token,dest,eid);
			GpcLogTrainMessage(0, 0, "向TD(%d)请求 %s %s", eid, token, dest);
		}
		if(TD_GenRequestMessage_13_WKINFO(token,dest,GetCurrentSectionNo(),endtm.GetTime() ))
		{
			TGSendStringMessage(token,dest,eid);
			GpcLogTrainMessage(0, 0, "向TD(%d)请求 %s %s", eid, token, dest);
		}
	}
	
	LoadMMPlanCmdSaveList();
	MMplanInform inform;
	inform.change_flag=MMplanInform::MMPLAN_LIST_CHANGED;
	inform.index=-1;	
	UpdateAllViews(pv,TGDATA_MPLAN_UPDATED,&inform);
	return TRUE;
}

#define LOCAL_SAVE_PLAN_LIST_NODE "LIST"
#define LOCAL_SAVE_PLAN_MARK_NODE "MARK"
void  CTgDocument::SendMMPlanStateToServer(int index)
{
	if(0)
	{
		CString xml, dest;
		xml="<MMPLAN_STATE>";

		if(m_nCountOfPlan>index&&index>=0)
		{
			dest="";
			InsertXMLNodeValue(dest,"PLANID",m_cMMplans[index]->GetID());
			InsertXMLNodeValue(dest,"ISCANCEL",m_cMMplans[index]->m_nStatus);
			InsertXMLNodeValue(dest,"STTM",m_cMMplans[index]->m_stTm);
			InsertXMLNodeValue(dest,"EDTM",m_cMMplans[index]->m_edTm);

			GpcLogTrainMessage(0,0,"施工兑现:%s", dest);  
				
			xml+="<RECORD>"+dest+"</RECORD>";
		}
		
		xml+="</MMPLAN_STATE>";

		// 获取施工服务器entity
		long  eid=GetSpecialEntityID(SERVER_TYPE_MMPLAN);
		if(eid>0)
		{
			CString token="MMPLAN_STATE";
			TGSendStringMessage(token, xml, eid);
			GpcLogTrainMessage(0, 0, "向施工服务器 entity %d, 发送施工兑现保存 %s", eid, token);
		}
		else
		{
			GpcLogTrainMessage(0, 0, "发送施工兑现保存, 无法获取施工服务器entity, 请检查entity.cfg中是否配置:eid=svr_mmplan 施工计划服务   101    2   12   0");
		}
	}
	else
	{
		GpcLogTrainMessage(0, 0, "不允许向施工服务器发送(MMPLAN_STATE) index %d", index);
	}
}
// 施工结果兑现
BOOL CTgDocument::ProcessXML_Msg_List_State(const char *xml,const char *node, int con_id)
{
	CMPlanListState planstate;
	BOOL rt=planstate.LoadMPlanStateFromCADXML(xml, node, con_id);
	if(!rt)
	{
		GpcLogTrainMessage(0, 0, "ProcessXML_Msg_List_State 处理施工标记的XML失败");
		AfxMessageBox("处理施工标记的XML错误");
		return FALSE;
	}

	for(int i=0;i<planstate.m_nCount;i++)
	{
		CString plan_id=planstate.m_list[i].PLANID;
		int index=FindMMPlan(plan_id);
		if(index<0)
		{
			GpcLogTrainMessage(0, 0, "施工兑现 找不到 planid %s的施工, 设置施工兑现失败", plan_id);
		}
		else
		{
			m_cMMplans[index]->m_bUpdatedFromMsg=TRUE;
			m_cMMplans[index]->m_nStatus =planstate.m_list[i].m_nStatus;
			m_cMMplans[index]->m_stTm =planstate.m_list[i].STTM; 
			m_cMMplans[index]->m_edTm=planstate.m_list[i].EDTM;

			GpcLogTrainMessage(0, 0, "施工兑现 planid %s的施工, %s %d %s %s", plan_id,
				m_cMMplans[index]->m_nStatus,
				m_cMMplans[index]->m_stTm,
				m_cMMplans[index]->m_edTm);
		}
	}

	MMplanInform inform;
	inform.change_flag=MMplanInform::MMPLAN_LIST_CHANGED;
	inform.index=-1;	
	UpdateAllViews(NULL,TGDATA_MPLAN_UPDATED,&inform);
	SaveMMListToFile();

	return TRUE;
}

BOOL CTgDocument::ReadMMPlanListStateFromFile()
{
	planidcancel.RemoveAll();

	CString  filenameprefix;
	CTime ctm=m_tMMplanEndTime;
	filenameprefix.Format("MMPLAN-Section%02d-%04d%02d%02d",GetCurrentSectionNo(),ctm.GetYear(),ctm.GetMonth(),ctm.GetDay());
	
	CString  dest;
	if(!ReadXMLFile(dest,LOCAL_SAVE_PLAN_LIST_NODE,filenameprefix))
		return FALSE;
	
	LOAD_XML(dest,LOCAL_SAVE_PLAN_LIST_NODE,pNodeRoot);
	MSXML::IXMLDOMNodeListPtr pList = pNodeRoot->selectNodes( "SUB_NODE" );
	int rec_count = pList->Getlength();
	for(int i = 0; i <rec_count && m_nCountOfPlan<MAX_MMPLAN_NUM ; i++)
	{
		CString id;
		int isCancel=0;
		GET_NODE_STRVALUE(id,pList->Getitem(i),"ID")
		GET_NODE_INTVALUE(isCancel,pList->Getitem(i),"ISCANCEL")

		if(!id.IsEmpty() && isCancel==1)
		{
			planidcancel.Add(id);
			GpcLogTrainMessage(0, 0, "取消施工 planid: %s", id);
		}
	}

	return TRUE;
}

void   CTgDocument::MPlanListToXML(CString &dest)
{
	CStringArray planid, sttm, edtm;
    int nNumTm = GetMPlanTime(planid, sttm, edtm);

	dest = "<?xml version=\"1.0\" encoding=\"GB2312\"?>";
	dest+="<";dest+=LOCAL_SAVE_PLAN_LIST_NODE;dest+=">";
	for(int i=0;i<m_nCountOfPlan;i++)
	{
		CString strSTTM="", strEDTM="";
		for(int n=0; n<nNumTm; n++)
		{
			if(m_cMMplans[i]->GetID() == planid.GetAt(n))
			{
				strSTTM = sttm.GetAt(n);
				strEDTM = edtm.GetAt(n);
				break;
			}
		}
		int isCancel=0;
		for(int m=0; m<planidcancel.GetCount(); m++)
		{
			if(m_cMMplans[i]->GetID() == planidcancel.GetAt(m))
			{
				isCancel=1;
				break;
			}
		}

		dest+="<SUB_NODE>";
		
		InsertXMLNodeValue(dest,"GUID",m_cMMplans[i]->m_guid);
		InsertXMLNodeValue(dest,"ID",m_cMMplans[i]->GetID());
		InsertXMLNodeValue(dest,"TOPIC",m_cMMplans[i]->m_topic);
		InsertXMLNodeValue(dest,"FLAG",m_cMMplans[i]->m_nFlag);
		InsertXMLNodeValue(dest,"TRNINFO",m_cMMplans[i]->m_trninfo);
		InsertXMLNodeValue(dest,"STTM", strSTTM);
		InsertXMLNodeValue(dest,"EDTM", strEDTM);
		InsertXMLNodeValue(dest,"ISCANCEL", isCancel);
		m_cMMplans[i]->m_nStatus=isCancel;
		m_cMMplans[i]->m_stTm=strSTTM;
		m_cMMplans[i]->m_edTm=strEDTM;
		
		dest+="</SUB_NODE>";

		GpcLogTrainMessage(0,0,"中间结果 planid %s,STTM %s, EDTM %s,ISCANCEL %d", m_cMMplans[i]->GetID(),  
			m_cMMplans[i]->m_stTm,  m_cMMplans[i]->m_edTm, m_cMMplans[i]->m_nStatus);
	}
	dest+="</";dest+=LOCAL_SAVE_PLAN_LIST_NODE;dest+=">";
}

// 保存施工中间结果到本地文件
BOOL   CTgDocument::SaveMMListToFile()
{
	CString  filenameprefix, listxml;
	CTime ctm=m_tMMplanEndTime;
	filenameprefix.Format("MMPLAN-Section%02d-%04d%02d%02d",GetCurrentSectionNo(),ctm.GetYear(),ctm.GetMonth(),ctm.GetDay());
	
	GpcLogTrainMessage(0,0,"BEGIN保存施工中间结果 %s num=%d", filenameprefix,  m_nCountOfPlan);
	
	MPlanListToXML(listxml);
	
	if(!WriteXMLFile(listxml,LOCAL_SAVE_PLAN_LIST_NODE,filenameprefix))
	{
		GpcLogTrainMessage(0,0,"END保存施工中间结果,失败 %s", filenameprefix);
		return FALSE;
	}
	else
	{
		GpcLogTrainMessage(0,0,"END保存施工中间结果,成功 %s", filenameprefix);
		return TRUE;
	}
}

// 保存施工符号到本地文件
BOOL   CTgDocument::SaveMMMarkToFile()
{
	CString  filenameprefix, marksxml;
	CTime ctm=m_tMMplanEndTime;

	filenameprefix.Format("MMMARK-Section%02d-%04d%02d%02d",GetCurrentSectionNo(),ctm.GetYear(),ctm.GetMonth(),ctm.GetDay());
	
	GpcLogTrainMessage(0,0,"BEGIN保存施工符号 %s", filenameprefix);

	SaveMMListToFile();
	MPlanMarksToXML(marksxml,LOCAL_SAVE_PLAN_MARK_NODE);
	if(!WriteXMLFile(marksxml,LOCAL_SAVE_PLAN_MARK_NODE,filenameprefix))
	{
		GpcLogTrainMessage(0,0,"END保存施工符号,失败 %s", filenameprefix);
		return TRUE;
	}
	else
	{
		GpcLogTrainMessage(0,0,"END保存施工符号,成功 %s", filenameprefix);
		return FALSE;
	}
}

// 保存施工中间结果到本地文件
BOOL  CTgDocument::LoadMMListFromFile()
{
	BOOL rc=TRUE;
	CString  filenameprefix;
	CTime ctm=m_tMMplanEndTime;
	filenameprefix.Format("MMPLAN-Section%02d-%04d%02d%02d",GetCurrentSectionNo(),ctm.GetYear(),ctm.GetMonth(),ctm.GetDay());	  
	CString  dest;
	ClearAllMMPlan();
	if(!ReadXMLFile(dest,LOCAL_SAVE_PLAN_LIST_NODE,filenameprefix))
		rc=FALSE;
	
	LOAD_XML(dest,LOCAL_SAVE_PLAN_LIST_NODE,pNodeRoot);
	MSXML::IXMLDOMNodeListPtr pList = pNodeRoot->selectNodes( "SUB_NODE" );
	int rec_count = pList->Getlength();

	for(int i = 0; i <rec_count && m_nCountOfPlan<MAX_MMPLAN_NUM ; i++)
	{
		CString  guid,id,topic,trninfo;
		CString strSTTM="", strEDTM="";
		int flag, isCancel=0;
		GET_NODE_STRVALUE(guid,pList->Getitem(i),"GUID")
		GET_NODE_STRVALUE(id,pList->Getitem(i),"ID")
		GET_NODE_STRVALUE(topic,pList->Getitem(i),"TOPIC")
		GET_NODE_INTVALUE(flag,pList->Getitem(i),"FLAG")
		GET_NODE_STRVALUE(trninfo,pList->Getitem(i),"TRNINFO")
		GET_NODE_INTVALUE(isCancel,pList->Getitem(i),"ISCANCEL")
		GET_NODE_STRVALUE(strSTTM,pList->Getitem(i),"STTM");
		GET_NODE_STRVALUE(strEDTM,pList->Getitem(i),"EDTM");

		int index=FindMMPlan(id);
		if(index<0)
		{
			index=AddMMPlan(id,TRUE);
			if(index<0)
				return FALSE;
		}
		if(isCancel == -1)
			m_cMMplans[index]->m_nStatus=0;
		else
			m_cMMplans[index]->m_nStatus=isCancel;
		m_cMMplans[index]->m_stTm=strSTTM;
		m_cMMplans[index]->m_edTm=strEDTM;

		m_cMMplans[index]->m_bUpdatedFromMsg=TRUE;
		m_cMMplans[index]->m_nFlag=flag;
		m_cMMplans[index]->m_topic=topic; 
		m_cMMplans[index]->m_trninfo=trninfo; 

		m_cMMplans[index]->m_guid =guid;
	}

    // 2010.3.20 now,read marks
	filenameprefix.Format("MMMARK-Section%02d-%04d%02d%02d",GetCurrentSectionNo(),ctm.GetYear(),ctm.GetMonth(),ctm.GetDay());
	dest.Empty();
	if(!ReadXMLFile(dest,LOCAL_SAVE_PLAN_MARK_NODE,filenameprefix))
		rc=FALSE;
	
	CTD_MPlanMarkList marklist;
	BOOL rt=marklist.LoadMarkFromTDXML(dest,LOCAL_SAVE_PLAN_MARK_NODE);
	for(int i=0; i<marklist.m_nCount&&i<MAX_MPLAN_MARK_NUM; i++)
	{		
		AddMPlanMark(&marklist.m_list[i]);
	}
	
	MMplanInform inform;
	inform.change_flag=MMplanInform::MMPLAN_LIST_CHANGED;
	inform.index=-1;	
	UpdateAllViews(NULL,TGDATA_MPLAN_UPDATED,&inform);
	return rc;
}

BOOL  CTgDocument::GenMarkFromMMPlan(const CTD_MPLANMARK *ptd_mark,int flag) // flag:  1 shift  2 work 3 all
{
	if(ptd_mark->m_tStartTime==0 || ptd_mark->m_tEndTime==0)
	{
		GpcLogTrainMessage(0,0,"施工时间为空,不能生成运行图符号!  ID=%s",ptd_mark->PLANID);
		return FALSE;
	}

	if(TG_IS_ABS_MARK(ptd_mark->m_nCTCType))
	{
		int station1=gpcdef.GetStationNoByName(ptd_mark->STST);
		int station2=gpcdef.GetStationNoByName(ptd_mark->EDST);
		if (station1<=0 || station2<=0)
		{
			GpcLogTrainMessage(0,0,"施工符号中车站名不正确,不能生成运行图符号!  ID=%s ST1=%s  ST2=%s",ptd_mark->PLANID,ptd_mark->STST,ptd_mark->EDST);
			return FALSE;
		}

		if (!GetTGBasicParam()->VerifyStationNumber(station1) || !GetTGBasicParam()->VerifyStationNumber(station2))
		{
			GpcLogTrainMessage(0,0,"施工符号中车站不属于该调度台管辖范围,不能生成运行图符号!  ID=%s ST1=%s  ST2=%s",ptd_mark->PLANID,ptd_mark->STST,ptd_mark->EDST);
			return FALSE;
		}

		if (!GetTGBasicParam()->VerifySectionStationIndex(this->GetCurrentSectionNo(),station1,ptd_mark->STTB)
			|| !GetTGBasicParam()->VerifySectionStationIndex(this->GetCurrentSectionNo(),station2,ptd_mark->EDTB))
		{
			GpcLogTrainMessage(0,0,"施工符号中车站站序非法,不能生成运行图符号!  ID=%s ST1=%s  ST2=%s TB1=%d TB2=%d",ptd_mark->PLANID,ptd_mark->STST,ptd_mark->EDST, ptd_mark->STTB, ptd_mark->EDTB);
			return FALSE;
		}
	}
	else if(TG_IS_INSTATION_MARK(ptd_mark->m_nCTCType))
	{
		int station=gpcdef.GetStationNoByName(ptd_mark->STST);
		if (station<=0)
		{
			GpcLogTrainMessage(0,0,"施工符号中车站名不正确,不能生成运行图符号!  ID=%s ST=%s",ptd_mark->PLANID,ptd_mark->STST);
			return FALSE;
		}

		if (!GetTGBasicParam()->VerifyStationNumber(station))
		{
			GpcLogTrainMessage(0,0,"施工符号中车站不属于该调度台管辖范围,不能生成运行图符号!  ID=%s ST=%s",ptd_mark->PLANID,ptd_mark->STST);
			return FALSE;
		}

		if (!GetTGBasicParam()->VerifySectionStationIndex(this->GetCurrentSectionNo(),station,ptd_mark->STTB))
		{
			GpcLogTrainMessage(0,0,"施工符号中车站站序非法,不能生成运行图符号!  ID=%s ST=%s TB=%d",ptd_mark->PLANID,ptd_mark->STST, ptd_mark->STTB);
			return FALSE;
		}
	}
	else
	{
		GpcLogTrainMessage(0,0,"施工符号中类型 %d 不正确,不能生成运行图符号!  ID=%s TYPE=%d",ptd_mark->m_nTDType,ptd_mark->PLANID,ptd_mark->m_nTDType);
		return FALSE;
	}

	CTG_ScheduleInfo schdinfo;
	CTG_TrainScheduleBase *pData;
	int errcount=0;
	for(int i=0;i<2;i++)
	{
		if(i==0 && (flag & 1))
		{
			pData=GetTGDataHolder()->GetShiftScheduleData();
		}
		else if(i==1 && (flag & 2))
		{
			pData=GetTGDataHolder()->GetWorkScheduleData();
		}
		else 
		{
			continue;
		}

		CTG_SingleMark mark;
		mark.SetBlockIndex(NULL_TRAININDEX); //新增对象,默认INDEX
		mark.SetGPCIndex(pData->GetAvaGPCIndex());

		if(!ptd_mark->GenMark(mark))
		{
			GpcLogTrainMessage(0,0,"符号中车站或者类型不正确,不能生成运行图符号!  ID=%s TP=%d ST1=%s  ST2=%s",ptd_mark->PLANID,ptd_mark->m_nTDType,ptd_mark->STST,ptd_mark->EDST);
			continue;
		}

		pData->GetScheduleInfo(schdinfo);

		// 2010.3.20 上图考虑重复，一个施工ID可有多个符号,
		CTG_ObjectPtrGroup group;
		pData->FindMarksByShiftString(group,ptd_mark->PLANID);
		int  markindex;
		for(markindex=0;markindex<group.GetCount();markindex++)
		{
			CTG_SingleMark *prev_mark=(CTG_SingleMark *)group.GetPtr(markindex);
			if(prev_mark)
			{
				if(prev_mark->GetMarkType()==mark.GetMarkType() 
					&& prev_mark->m_data.station==mark.m_data.station 
					&& prev_mark->m_data.index==mark.m_data.index) //同一施工计划中重复施工符号
					break;
			}
		}
		if(markindex<group.GetCount())
		{
			if(!(flag & 0x80))  // flag : 0x80 means Force genmark
			{
				GpcLogTrainMessage(0,0,"根据OPMS生成符号信息,但已经存在  ID=%s FLAG=%d",ptd_mark->PLANID,i);
				errcount++;
				continue;
			}
		}

		if(mark.GetBlockIndex()==NULL_TRAININDEX)
		{
			pData->AppendMark(schdinfo,mark,SYCFLAG_NULL); 
			GpcLogTrainMessage(0,0,"根据OPMS生成符号信息  ID=%s FLAG=%d",ptd_mark->PLANID,i);
		}
		else
		{
			pData->UpdateMark(schdinfo,mark,SYCFLAG_NULL);
			GpcLogTrainMessage(0,0,"根据OPMS更新符号信息  ID=%s FLAG=%d",ptd_mark->PLANID,i);
		}
	}
	if(errcount>0)
		return FALSE;

	return TRUE;
}

BOOL  CTgDocument::GenMarkFromMMPlanByPlanId(const CString planid,int flag, int&s, int&f) // flag:  1 shift  2 work 3 all 0x80: force gen(ignore dup)
{
	s=0;f=0;
	BOOL rc=FALSE;
	list<CTD_MPLANMARK>::iterator iter;
	iter=m_cMMplanMarks.begin();
	while(iter!=m_cMMplanMarks.end())
	{
		if(iter->PLANID==planid)
		{
			if(GenMarkFromMMPlan(&*iter,flag))
				s++;
			else
				f++;
			rc=TRUE;
		}
		iter++;
	}
	return rc;
}

BOOL  CTgDocument::GenMarkFromMMPlanByPlanId(int flag) // flag:  1 shift  2 work 3 all 0x80: force gen(ignore dup)
{
	BOOL rc=TRUE;
	list<CTD_MPLANMARK>::iterator iter;
	iter=m_cMMplanMarks.begin();
	while(iter!=m_cMMplanMarks.end())
	{
		if(!GenMarkFromMMPlan(&*iter,flag))
			rc=FALSE;
		iter++;
	}
	return rc;
}


// type 显示施工符号的类型 0:全部 1:封锁/慢行 2:天窗修
BOOL  CTgDocument::GetMarkDesc(const char *planid,CStringArray &result, int type) // flag:  1 shift  2 work 3 all
{
	BOOL rc=TRUE;
	list<CTD_MPLANMARK>::iterator iter;
	iter=m_cMMplanMarks.begin();
	while(iter!=m_cMMplanMarks.end())
	{
		if(planid==NULL || iter->PLANID==planid)
		{
			if (type==2 && iter->m_nTDType!=5)
			{
				iter++;
				continue;
			}
			else if (type==1 && (iter->m_nTDType<1 || iter->m_nTDType>4))
			{
				iter++;
				continue;
			}

			CString strMarkType = GetCTCValueDispString(CTC_DATA_MARK_TYPE,iter->m_nCTCType,0);
			CString strBeginTime = GetCTCValueDispString(CTC_DATA_DATETIME,iter->m_tStartTime,0);
			CString strEndTime = GetCTCValueDispString(CTC_DATA_DATETIME,iter->m_tEndTime,0);
			
			CString desc;
			desc.Format("运行图标记:%s,%s-%s",	strMarkType,strBeginTime,strEndTime);
			result.Add(desc);
		}
		iter++;
	}
	return rc;
}
BOOL   CTgDocument::SaveMemMMPlanListToFile()
{
	BOOL rc=TRUE;
	CString  filenameprefix;
	CTime ctm=m_tMMplanEndTime;
	filenameprefix.Format("MMPLAN-Section%02d-%04d%02d%02d",GetCurrentSectionNo(),ctm.GetYear(),ctm.GetMonth(),ctm.GetDay());

	CString  dest;
	dest+="<";dest+=LOCAL_SAVE_PLAN_LIST_NODE;dest+=">";
	for(int i=0;i<m_nCountOfPlan;i++)
	{
		dest+="<SUB_NODE>";
	
		InsertXMLNodeValue(dest,"GUID",m_cMMplans[i]->m_guid);
		InsertXMLNodeValue(dest,"ID",m_cMMplans[i]->GetID());
		InsertXMLNodeValue(dest,"TOPIC",m_cMMplans[i]->m_topic);
		InsertXMLNodeValue(dest,"FLAG",m_cMMplans[i]->m_nFlag);
		InsertXMLNodeValue(dest,"TRNINFO",m_cMMplans[i]->m_trninfo);
		InsertXMLNodeValue(dest,"STTM", m_cMMplans[i]->m_stTm);
		InsertXMLNodeValue(dest,"EDTM", m_cMMplans[i]->m_edTm);
		InsertXMLNodeValue(dest,"ISCANCEL", m_cMMplans[i]->m_nStatus);
		
		dest+="</SUB_NODE>";
	}
	dest+="</";dest+=LOCAL_SAVE_PLAN_LIST_NODE;dest+=">";
	if(!WriteXMLFile(dest,LOCAL_SAVE_PLAN_LIST_NODE,filenameprefix))
		rc=FALSE;

	return rc;
}

int  CTgDocument::GetMPlanTime(CStringArray& planid, CStringArray& sttm, CStringArray& edtm)
{
	int ret = 0;
	planid.RemoveAll();
	sttm.RemoveAll();
	edtm.RemoveAll();

	list<CTD_MPLANMARK>::iterator iter;
	iter=m_cMMplanMarks.begin();
	
	while(iter!=m_cMMplanMarks.end())
	{
		CString plan = iter->PLANID;
		if(!plan.IsEmpty() && iter->m_tStartTime>0 && iter->m_tEndTime>0)
		{
			int i=0;
			for(i=0; i<planid.GetCount(); i++)
			{
				if(planid.GetAt(i) == plan)
					break;
			}
	
			if(i>=planid.GetCount())
			{
				ret++;
				planid.Add(plan);
				
				sttm.Add(TimeToXMLString(iter->m_tStartTime));
				edtm.Add(TimeToXMLString(iter->m_tEndTime));
			}
		}

		iter++;
	}

	return ret;
}

// 2010.3.20 施工符号单独存储
void  CTgDocument::AddMPlanMark(CTD_MPLANMARK *p)
{
	list<CTD_MPLANMARK>::iterator iter;
	iter=m_cMMplanMarks.begin();
	while(iter!=m_cMMplanMarks.end())
	{
		if((*iter) == (*p))
			return;
		iter++;
	}

	m_cMMplanMarks.push_back(*p);
}
void  CTgDocument::MPlanMarksToXML(CString &xml,const char *node)
{
	xml = "<?xml version=\"1.0\" encoding=\"GB2312\"?>";
	list<CTD_MPLANMARK>::iterator iter;
	iter=m_cMMplanMarks.begin();
	if(node)
	{
		xml+="<";
		xml+=node;
		xml+=">";
	}
	while(iter!=m_cMMplanMarks.end())
	{
		iter->SaveToXML(xml,"RECORD");
		iter++;
	}
	if(node)
	{
		xml+="</";
		xml+=node;
		xml+=">";
	}
}

time_t CTgDocument::GetMMPlanStartTime(const CMMPlanListInfo *mPlan)
{
	time_t rc=0;
	list<CTD_MPLANMARK>::iterator iter;
	iter=m_cMMplanMarks.begin();
	while(iter!=m_cMMplanMarks.end())
	{
		if(iter->PLANID==mPlan->GetMainPlanID())
			if(rc==0 || rc>iter->m_tStartTime)
				rc=iter->m_tStartTime;
		iter++;
	}
   return rc;
}
time_t CTgDocument::GetMMPlanEndTime(const CMMPlanListInfo *mPlan)
{
	time_t rc=0;
	list<CTD_MPLANMARK>::iterator iter;
	iter=m_cMMplanMarks.begin();
	while(iter!=m_cMMplanMarks.end())
	{
		if(iter->PLANID==mPlan->GetMainPlanID())
			if(rc==0 || rc<iter->m_tEndTime)
				rc=iter->m_tEndTime;
		iter++;
	}
   return rc;
}

void  CTgDocument::DeleteMarksByMMPlanID(CTG_ScheduleInfo::SCHEDULE_TYPE sche_type,const char *planid)
 {
		 CTG_TrainScheduleBase *pData=GetScheduleData(sche_type);
		 if(!pData)
			 return;
		 CTG_ScheduleInfo schdinfo;
		 pData->GetScheduleInfo(schdinfo);
		 CTG_ObjectPtrGroup group;
         pData->FindMarksByShiftString(group,planid);
		 int  markindex;
		 for(markindex=0;markindex<group.GetCount();markindex++)
		 {
             CTG_SingleMark *prev_mark=(CTG_SingleMark *)group.GetPtr(markindex);
			 if(prev_mark)
			 {
				pData->DeleteMark(schdinfo,prev_mark->GetBlockIndex(),SYCFLAG_NULL);
			 }
		 }
}