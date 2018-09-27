#include "StdAfx.h"
#include "tg_BinXml.h"
#include "utility/xml_utility.h"
#import  <msxml.dll> named_guids
using namespace MSXML;
CString GetValidStr(CString str)  // 转义 " < > & ' 等特殊字符，否则引起xml解析失败
{
    CString ret="";
	for(int i=0; i<str.GetLength(); i++)
	{
		char ch=str.GetAt(i);

		if(ch=='<')
			ret+="&lt;";
		else if(ch=='>')
			ret+="&gt;";
		else if(ch=='&')
			ret+="&amp;";
		else if(ch=='\"')
			ret+="&quot;";
		else if(ch=='\'')
			ret+="&apos;";
		else
			ret+=ch;
	}

	return ret;
}

void ZeroStringTail(char *str,int limit)  // 去除尾部乱字符，当站名中出现引号时，导致和服务器无法同步 2010.6.23
{
     int len=strlen(str);
	 if(limit>len)
		 memset(str+len,0,limit-len);
}

#define  ZERO_STR_TAIL(x)  ZeroStringTail(x,sizeof(x));

int GetDCmdPlanResult(CString xml, CString& warning, CString& id)
{
	int result = -1;

	MSXML::IXMLDOMDocumentPtr pXMLDoc;
	HRESULT hr = pXMLDoc.CreateInstance(MSXML::CLSID_DOMDocument);
	if(FAILED(hr))
	{
		_com_error er(hr);
		AfxMessageBox(er.ErrorMessage());
		return -1;
	}
	
	if(VARIANT_TRUE != pXMLDoc->loadXML( _bstr_t( xml ) ) )
	{
		return -1;
	}
	
	
	MSXML::IXMLDOMNodePtr pRootNode, pNode;
 
	pRootNode = pXMLDoc->selectSingleNode( "Msg_Info");
	if(pRootNode != NULL)
	{
		pNode = pRootNode->selectSingleNode( "Result" );
		if(pNode)
		{
			_variant_t v;
			v = pNode->GetnodeTypedValue();
			result = atol(CString(v.bstrVal));
			
			SysFreeString(v.bstrVal);
			v.Clear();
		}
		pNode = pRootNode->selectSingleNode( "SectionPlan_ID" );
		if(pNode)
		{
			_variant_t v;
			v = pNode->GetnodeTypedValue();
			id = CString(v.bstrVal);
			
			SysFreeString(v.bstrVal);
			v.Clear();
		}
		pNode = pRootNode->selectSingleNode( "Message" );
		if(pNode)
		{
			_variant_t v;
			v = pNode->GetnodeTypedValue();
			warning = CString(v.bstrVal);
			
			SysFreeString(v.bstrVal);
			v.Clear();
		}
	}
	
	return result;
}

int fromDeleteXml(int& showdlg, std::vector<ULONG>& list_index, CString xml)
{
	showdlg=0;
	MSXML::IXMLDOMDocumentPtr pXMLDoc;
	HRESULT hr = pXMLDoc.CreateInstance(MSXML::CLSID_DOMDocument);
	if(FAILED(hr))
	{
		_com_error er(hr);
		AfxMessageBox(er.ErrorMessage());
		return false;
	}
	
	if(VARIANT_TRUE != pXMLDoc->loadXML( _bstr_t( xml ) ) )
	{
		return false;
	}
	
	_variant_t v;
	MSXML::IXMLDOMNodePtr pRootNode = NULL;
    MSXML::IXMLDOMNodePtr pNode, pInfo, pChildNode;

	pRootNode = pXMLDoc->selectSingleNode( "TRAINS");
	if(pRootNode != NULL)
	{
		MSXML::IXMLDOMNodeListPtr pRecAbsList = pRootNode->selectNodes( "train_index" );
	
		for(int idx = 0; idx < pRecAbsList->Getlength(); idx++)
		{
			pNode = pRecAbsList->Getitem(idx);
			if(pNode)
			{
				v = pNode->GetnodeTypedValue();
				ULONG index = atol(CString(v.bstrVal));
				list_index.push_back(index);
				SysFreeString(v.bstrVal);
				v.Clear();
			}
		}
        
		return 1;
	}
	else
	{
		pRootNode = pXMLDoc->selectSingleNode( "BLOCKS");
		if(pRootNode != NULL)
		{
			MSXML::IXMLDOMNodeListPtr pRecAbsList = pRootNode->selectNodes( "block_index" );

			for(int idx = 0; idx < pRecAbsList->Getlength(); idx++)
			{
				pNode = pRecAbsList->Getitem(idx);
				if(pNode)
				{
					v = pNode->GetnodeTypedValue();
					ULONG index = atol(CString(v.bstrVal));
					list_index.push_back(index);
					SysFreeString(v.bstrVal);
					v.Clear();

					pChildNode= pNode->selectSingleNode( "@flag" );
					if(pChildNode)
					{
						v = pChildNode->GetnodeTypedValue();
						showdlg = atol(CString(v.bstrVal));
						SysFreeString(v.bstrVal);
						v.Clear();
					}
				}
			}
			return 2;
		}
	}

	return -1;
}

bool TgTRAIN_RECORDfromXML(CTG_TrainRecord& record, MSXML::IXMLDOMNodePtr pRoot);
CString toXMLString(const CTG_SingleTrain &train,int flag)
{
	const CTG_TrainInfo *pTrainInfo=train.GetTrainInfo_Const();
	CString xml,tmp;
	const char *ptmpstr;
	tmp.Format("<TRAIN train_index = \"%d\">", train.GetTrainIndex());
	xml = tmp;

	tmp.Format("<LOGIC flag= \"%d\"/>", flag);
	xml += tmp;

	xml += " <INFO";
	{
		tmp.Format(" self_update = \"%d\"",train.IsSelfUpdate());
		xml += tmp;

		tmp.Format(" train_name = \"%s\"", GetValidStr(pTrainInfo->m_data.train_name));
		xml += tmp;

		tmp.Format(" train_key = \"%u\"", pTrainInfo->m_data.train_key);
		xml += tmp;

		tmp.Format(" basic_train_index = \"%u\"", pTrainInfo->m_data.basic_train_index);
		xml += tmp;

		tmp.Format(" gpc_train_index = \"%u\"", pTrainInfo->m_data.gpc_train_index);
		xml += tmp;

		tmp.Format(" shift_train_index = \"%u\"", pTrainInfo->m_data.shift_train_index);
		xml += tmp;

		tmp.Format(" shift_index_string = \"%s\"", GetValidStr(pTrainInfo->m_data.shift_string));
		xml += tmp;

		tmp.Format(" update_time = \"%d\"", pTrainInfo->m_data.update_time);
		xml += tmp;

		tmp.Format(" type = \"%d\"", pTrainInfo->m_data.type);
		xml += tmp;

		tmp.Format(" run_type = \"%d\"", pTrainInfo->m_data.run_type);
		xml += tmp;

		tmp.Format(" start_plan_time = \"%d\"", pTrainInfo->m_data.start_plan_time);
		xml += tmp;

		tmp.Format(" start_actual_time = \"%d\"", pTrainInfo->m_data.start_actual_time);
		xml += tmp;

		tmp.Format(" sborder_plan_time = \"%d\"", pTrainInfo->m_data.sborder_plan_time);
		xml += tmp;

		tmp.Format(" sborder_actual_time = \"%d\"", pTrainInfo->m_data.sborder_actual_time);
		xml += tmp;

		tmp.Format(" eborder_plan_time = \"%d\"", pTrainInfo->m_data.eborder_plan_time);
		xml += tmp;

		tmp.Format(" eborder_actual_time = \"%d\"", pTrainInfo->m_data.eborder_actual_time);
		xml += tmp;
		       
		tmp.Format(" sborder_sta = \"%d\"", pTrainInfo->m_data.sborder_sta);
		xml += tmp;

		tmp.Format(" eborder_sta = \"%d\"", pTrainInfo->m_data.eborder_sta);
		xml += tmp;
			  
		tmp.Format(" start_sta_name = \"%s\"", GetValidStr(pTrainInfo->m_data.start_sta_name));
		xml += tmp;

		tmp.Format(" end_sta_name = \"%s\"", GetValidStr(pTrainInfo->m_data.end_sta_name));
		xml += tmp;
			 
		tmp.Format(" from_conid = \"%d\"", pTrainInfo->m_data.from_conid);
		xml += tmp;

		tmp.Format(" to_conid = \"%d\"", pTrainInfo->m_data.to_conid);
		xml += tmp;

		tmp.Format(" priority = \"%d\"", pTrainInfo->m_data.priority);
		xml += tmp;

		tmp.Format(" train_length = \"%d\"", pTrainInfo->m_data.train_length);
		xml += tmp;
			
		tmp.Format(" train_weight = \"%d\"", pTrainInfo->m_data.train_weight);
		xml += tmp;

		tmp.Format(" status = \"%d\"", pTrainInfo->m_data.status);
		xml += tmp;
			
		tmp.Format(" space_rank = \"%d\"", pTrainInfo->m_data.space_rank);
		xml += tmp;

		tmp.Format(" length_rank = \"%d\"", pTrainInfo->m_data.length_rank);
		xml += tmp;

		tmp.Format(" super_rank = \"%d\"", pTrainInfo->m_data.super_rank);
		xml += tmp;

		tmp.Format(" military_rank = \"%d\"", pTrainInfo->m_data.military_rank);
		xml += tmp;
			
		tmp.Format(" engine_utility = \"%d\"", pTrainInfo->m_data.engine_utility);
		xml += tmp;

		tmp.Format(" side_utility = \"%d\"", pTrainInfo->m_data.side_utility);
		xml += tmp;

		tmp.Format(" adjust_begin_sta = \"%d\"", pTrainInfo->m_data.adjust_begin_sta);
		xml += tmp;

		tmp.Format(" adjust_end_sta = \"%d\"", pTrainInfo->m_data.adjust_end_sta);
		xml += tmp;

		tmp.Format(" adjust_flag = \"%d\"", pTrainInfo->m_data.adjust_flag);
		xml += tmp;

		tmp.Format(" train_source = \"%d\"", pTrainInfo->m_data.train_source);
		xml += tmp;
		
		tmp.Format(" delete_flag = \"%d\"", pTrainInfo->m_data.delete_flag);
		xml += tmp;

		const char* ticket = train.GetImportInfo(TRAIN_TXT_INFO_TYPE_CREW);
		if(ticket)
		{
			CString tmp1;
			XMLCopyValueString(tmp1,ticket); 
			tmp.Format(" ticket = \"%s\"", tmp1);
			xml += tmp;
		}
				
		xml += "/>";
	}
	
	xml += "<RECORDS>";
	{
		for(int recindex=0; recindex<train.GetTrainRecordCount(); recindex++)
		{
			const CTG_TrainRecord *prec=train.GetTrainRecord_Const(recindex);  //// 521计划优化修改
			xml += "<RECORD";
	
			tmp.Format(" rec_index = \"%d\"", prec->GetIndex()); //recindex);
			xml += tmp;
			
			ptmpstr=prec->GetOpmsSationTextState();
			if(ptmpstr)
				tmp.Format(" opms_give_station_text = \"%s\"", GetValidStr(ptmpstr));
			else
				tmp.Format(" opms_give_station_text = \"1\"");
			xml += tmp;

			ptmpstr=prec->GetStationText(STATION_TXT_INFO_TYPE_OPMS);
			if(ptmpstr)
				tmp.Format(" opms_station_text = \"%s\"", GetValidStr(ptmpstr));
			else
				tmp.Format(" opms_station_text = \"\"");
			xml += tmp;

			ptmpstr=prec->GetStationText(STATION_TXT_INFO_TYPE_SHUAIGUA);
			if(ptmpstr)
				tmp.Format(" station_text = \"%s\"", GetValidStr(ptmpstr));
			else
				tmp.Format(" station_text = \"\"");
			xml += tmp;

			tmp.Format(" abs_text = \"%s\"", GetValidStr(prec->m_strAbsText.c_str()));
			xml += tmp;

			tmp.Format(" alarm_text = \"%s\"", GetValidStr(prec->m_strAlarmText.c_str()));
			xml += tmp;

			tmp.Format(" note_text = \"%s\"", GetValidStr(prec->m_strNoteText.c_str()));
			xml += tmp;

			tmp.Format(" arrive_train_id = \"%s\"", GetValidStr(prec->m_data.arrive_train_id));
			xml += tmp;

			tmp.Format(" original_arrive_train_id = \"%s\"", GetValidStr(prec->m_data.original_arrive_train_id));
			xml += tmp;

			tmp.Format(" depart_train_id = \"%s\"", GetValidStr(prec->m_data.depart_train_id));
			xml += tmp;

			tmp.Format(" original_depart_train_id = \"%s\"", GetValidStr(prec->m_data.original_depart_train_id));
			xml += tmp;

			tmp.Format(" station = \"%d\"", prec->m_data.station);
			xml += tmp;

			if(prec->m_data.entry==255)
				tmp.Format(" entry = \"%d\"", 0);
			else
				tmp.Format(" entry = \"%d\"", prec->m_data.entry);
			xml += tmp;

			if(prec->m_data.exit==255)
				tmp.Format(" exit = \"%d\"", 0);
			else
				tmp.Format(" exit = \"%d\"", prec->m_data.exit);
			xml += tmp;

			tmp.Format(" arrive_side = \"%d\"", prec->m_data.arrive_side);
			xml += tmp;

			tmp.Format(" plan_arrive_side = \"%d\"", prec->m_data.plan_arrive_side);
			xml += tmp;

			tmp.Format(" depart_side = \"%d\"", prec->m_data.depart_side);
			xml += tmp;

			tmp.Format(" plan_depart_side = \"%d\"", prec->m_data.plan_depart_side);
			xml += tmp;
			   
			tmp.Format(" min_stop_time = \"%d\"", prec->m_data.min_stop_time);
			xml += tmp;

			tmp.Format(" arrive = \"%d\"", prec->m_data.arrive);
			xml += tmp;

			tmp.Format(" depart = \"%d\"", prec->m_data.depart);
			xml += tmp;

			tmp.Format(" plan_arrive = \"%d\"", prec->m_data.plan_arrive);
			xml += tmp;

			tmp.Format(" plan_depart = \"%d\"", prec->m_data.plan_depart);
			xml += tmp;
			
			tmp.Format(" stop_reason = \"%d\"", prec->m_data.stop_reason);
			xml += tmp;

			tmp.Format(" late_reason = \"%d\"", prec->m_data.late_reason);
			xml += tmp;

			tmp.Format(" early_depart_time = \"%d\"", prec->m_data.early_depart_time);
			xml += tmp;

			tmp.Format(" run_fast_slow_time = \"%d\"", prec->m_data.run_fast_slow_time);
			xml += tmp;

			tmp.Format(" route_flag = \"%d\"", prec->m_data.route_flag);
			xml += tmp;

			tmp.Format(" speed_limit = \"%d\"", prec->m_data.speed_limit);
			xml += tmp;

			tmp.Format(" stop_condition = \"%d\"", prec->m_data.stop_condition);
			xml += tmp;

			tmp.Format(" adjust_status = \"%d\"", prec->m_data.adjust_status);
			xml += tmp;

			tmp.Format(" spec_run_time = \"%d\"", prec->m_data.spec_run_time);
			xml += tmp;

			tmp.Format(" special_position = \"%d\"", prec->m_data.special_position);
			xml += tmp;

			tmp.Format(" special_time_1 = \"%d\"", prec->m_data.special_time_1);
			xml += tmp;

			tmp.Format(" special_time_2 = \"%d\"", prec->m_data.special_time_2);
			xml += tmp;    

			tmp.Format(" special_train_index = \"%d\"", prec->m_data.special_train_index);
			xml += tmp;

			tmp.Format(" data_source = \"%d\"", prec->m_data.data_source);
			xml += tmp;

			tmp.Format(" flag = \"%d\"", prec->m_data.flag);
			xml += tmp;

			tmp.Format(" super_interval_ltime = \"%d\"", prec->m_data.super_interval_ltime);
			xml += tmp;

			tmp.Format(" in_station_index = \"%d\"", prec->GetArriveTGIndex());
			xml += tmp;

			tmp.Format(" out_station_index = \"%d\"", prec->GetDepartTGIndex());
			xml += tmp;

			tmp.Format(" can_not_meet = \"%d\"", prec->m_data.can_not_meet);
			xml += tmp;

			tmp.Format(" rank = \"%d\"", prec->m_data.rank);
			xml += tmp;

			tmp.Format(" send_plan_time = \"%d\"", prec->m_data.send_plan_time);
			xml += tmp;

			xml += ">";

			for (int i=0; i<prec->m_listAbsStop.size(); i++)
			{
				xml += "<ABS_STOP";
				
				tmp.Format(" flag = \"%d\"", prec->m_listAbsStop[i].flag);
				xml += tmp;

				tmp.Format(" position = \"%d\"", prec->m_listAbsStop[i].position);
				xml += tmp;

				tmp.Format(" start_time = \"%d\"", prec->m_listAbsStop[i].start_time);
				xml += tmp;

				tmp.Format(" stop_time = \"%d\"", prec->m_listAbsStop[i].stop_time);
				xml += tmp;
					
				xml += "/>";	   
			}
		
			xml += "</RECORD>";
		}
	}
	xml += "</RECORDS>";

	xml += "<ENGINES>";
	for(int i=0; i<train.GetEngineCount(); i++)
	{
		const CTG_TrainEngineInfo *pEngineInfo=train.GetEngine_Const(i);
		xml += "<ENGINE ";

		tmp.Format(" engine_id = \"%s\"", GetValidStr(pEngineInfo->m_data.engine_id));
		xml += tmp;

		tmp.Format(" engine_type = \"%s\"", GetValidStr(pEngineInfo->m_data.engine_type));
		xml += tmp;

		tmp.Format(" engine_status = \"%d\"", pEngineInfo->m_data.engine_status);
		xml += tmp;

		tmp.Format(" attach_sta = \"%d\"", pEngineInfo->m_data.attach_sta);
		xml += tmp;

		tmp.Format(" unattach_sta = \"%d\"", pEngineInfo->m_data.unattach_sta);
		xml += tmp;
		
		xml += "/>";
	}
    xml += "</ENGINES>";

	xml += "<ExtraInfo>";
	xml += "<OPMSInfo>";
	XMLCopyValueString(xml,pTrainInfo->m_info.c_str()); 
	xml += "</OPMSInfo>";
	xml += "</ExtraInfo>";

	const TG_TrainInfoExpand* pTrainExpand=train.GetTrainInfoExpand_Const();

	const CTG_TrainRelation* pRL=pTrainExpand->GetTrainRelation_Const();
	tmp.Format("<RELATION prev_train_index=\"%u\" prev_basic_index=\"%u\" prev_shift_index=\"%u\" next_shift_index=\"%u\" czh_no=\"%s\" prev_dest_sta=\"%d\" next_dest_sta=\"%d\"/>", 
		pRL->prev_train_index, pRL->prev_basic_index,
		pRL->prev_shift_index, pRL->next_shift_index, GetValidStr(pRL->czh_no_str),
		pRL->prev_dest_sta, pRL->next_dest_sta);
	xml += tmp;

	xml += "<KEDIAO>";
	{
		const CTG_TrainKediaoInfo* pKD=pTrainExpand->GetTrainKediao_Const();
		tmp.Format("<type>%d</type>", pKD->type);
		xml += tmp;
		CString kd_content=pKD->content;
		tmp.Format("<context>%s</context>", GetValidStr(kd_content));
		xml += tmp;
	}
	xml += "</KEDIAO>";

	xml += "<YY_LIST>";
	{
		const CTG_TrainYingyeList* plist=pTrainExpand->GetTrainYYList_Const();
		if(plist)
		{
			for(int i=0; i<plist->m_listRecords.size(); i++)
			{
				xml += "<RECORD ";

				tmp.Format(" arrive_time = \"%d\"", plist->m_listRecords[i].arrive_time);
				xml += tmp;

				tmp.Format(" depart_time = \"%d\"", plist->m_listRecords[i].depart_time);
				xml += tmp;

				tmp.Format(" station_no = \"%d\"", plist->m_listRecords[i].station_no);
				xml += tmp;

				tmp.Format(" station_name = \"%s\"", GetValidStr(plist->m_listRecords[i].station_name));
				xml += tmp;

				xml += "/>";
			}
		}
	}
	xml += "</YY_LIST>";
	xml += "<CREW_LIST>";
	{
		const CTG_TrainCrewList* plist=pTrainExpand->GetTrainCrewList_Const();
		if(plist)
		{
			for(int i=0; i<plist->m_listRecords.size(); i++)
			{
				xml += "<RECORD ";

				tmp.Format(" crew_type = \"%d\"", plist->m_listRecords[i].crew_type);
				xml += tmp;

				tmp.Format(" crew_bureau = \"%s\"", GetValidStr(plist->m_listRecords[i].crew_bureau));
				xml += tmp;

				tmp.Format(" crew_info = \"%s\"", GetValidStr(plist->m_listRecords[i].crew_info));
				xml += tmp;

				xml += "/>";
			}
		}
	}
	xml += "</CREW_LIST>";

	xml += "<ENGINE_CREW_LIST>";
	{
		const CTG_EngineCrewList* plist=pTrainExpand->GetTrainEngineCrewList_Const();
		if(plist)
		{
			for(int i=0; i<plist->m_listRecords.size(); i++)
			{
				xml += "<RECORD ";

				tmp.Format(" overwork_time = \"%d\"", plist->m_listRecords[i].overwork_time);
				xml += tmp;

				tmp.Format(" station_no = \"%d\"", plist->m_listRecords[i].station_no);
				xml += tmp;

				tmp.Format(" station_name = \"%s\"", GetValidStr(plist->m_listRecords[i].station_name));
				xml += tmp;

				xml += "/>";
			}
		}
	}
	xml += "</ENGINE_CREW_LIST>";
	xml += "<TICKET_LIST>";
	{
		const CTG_TrainTicketList* plist=pTrainExpand->GetTrainTicketList_Const();
		if(plist)
		{
			for(int i=0; i<plist->m_listRecords.size(); i++)
			{
				xml += "<RECORD ";

				tmp.Format(" down_station = \"%d\"", plist->m_listRecords[i].down_station);
				xml += tmp;

				tmp.Format(" down_station_index = \"%d\"", plist->m_listRecords[i].down_station_index);
				xml += tmp;

				tmp.Format(" down_station_name = \"%s\"", GetValidStr(plist->m_listRecords[i].down_station_name));
				xml += tmp;

				tmp.Format(" on_station = \"%d\"", plist->m_listRecords[i].on_station);
				xml += tmp;

				tmp.Format(" on_station_index = \"%d\"", plist->m_listRecords[i].on_station_index);
				xml += tmp;

				tmp.Format(" on_station_name = \"%s\"", GetValidStr(plist->m_listRecords[i].on_station_name));
				xml += tmp;

				tmp.Format(" sale = \"%d\"", plist->m_listRecords[i].sale);
				xml += tmp;

				tmp.Format(" seat = \"%s\"", GetValidStr(plist->m_listRecords[i].seat));
				xml += tmp;

				xml += "/>";
			}
		}
	}
	xml += "</TICKET_LIST>";
	xml += "</TRAIN>";

	return xml;
}

bool fromXML(CTG_SingleTrain &train,CMMIOperFlag &oper_flag,const char *xml,bool fromFile)
{
	CTG_TrainInfo *pTrainInfo=train.GetTrainInfo();
	MSXML::IXMLDOMDocumentPtr pXMLDoc;
	HRESULT hr = pXMLDoc.CreateInstance(MSXML::CLSID_DOMDocument);
	if(FAILED(hr))
	{
		_com_error er(hr);
		AfxMessageBox(er.ErrorMessage());
		return false;
	}
	
	if(VARIANT_TRUE != pXMLDoc->loadXML( _bstr_t( xml ) ) )
	{
		return false;
	}
	
	MSXML::IXMLDOMNodePtr pRootNode = pXMLDoc->selectSingleNode( "TRAIN");
	if(pRootNode == NULL)
	{
		return false;
	}


	MSXML::IXMLDOMNodePtr pNode, pInfo;
	_variant_t v;
		
	pNode = pRootNode->selectSingleNode( "@train_index" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		train.SetTrainIndex(atol(CString((v.bstrVal))));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	pNode = pRootNode->selectSingleNode( "@prev_train_index" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		train.SetPrevTrainIndex(atol(CString((v.bstrVal))));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	pInfo = pRootNode->selectSingleNode( "LOGIC");
	if(pInfo)
	{
		pNode = pInfo->selectSingleNode( "@flag" );
		if(pNode)
		{
			v = pNode->GetnodeTypedValue();
			oper_flag.nFlag = atol(CString((v.bstrVal)));
			SysFreeString(v.bstrVal);
			v.Clear();
		}
		pNode = pInfo->selectSingleNode( "@station" );
		if(pNode)
		{
			v = pNode->GetnodeTypedValue();
			oper_flag.m_nStation = atol(CString((v.bstrVal)));
			SysFreeString(v.bstrVal);
			v.Clear();
		}
		pNode = pInfo->selectSingleNode( "@rec_index" );
		if(pNode)
		{
			v = pNode->GetnodeTypedValue();
			oper_flag.m_nRecIndex = atol(CString((v.bstrVal)));
			SysFreeString(v.bstrVal);
			v.Clear();
		}
		pNode = pInfo->selectSingleNode( "@train_id" );
		if(pNode)
		{
			v = pNode->GetnodeTypedValue();
			CString tmp = CString(v.bstrVal);
			strncpy(oper_flag.m_trainId, tmp.GetBuffer(), sizeof(oper_flag.m_trainId)-1);
			oper_flag.m_trainId[sizeof(oper_flag.m_trainId)-1] = 0;
			SysFreeString(v.bstrVal);
			v.Clear();
		}
		oper_flag.m_nRun_type = 255;
		pNode = pInfo->selectSingleNode( "@run_type" );
		if(pNode)
		{
			v = pNode->GetnodeTypedValue();
			oper_flag.m_nRun_type = atol(CString((v.bstrVal)));
			SysFreeString(v.bstrVal);
			v.Clear();
		}
	}
	// INFO
	pInfo = pRootNode->selectSingleNode( "INFO");
	if(pInfo == NULL)
	{
		return false;
	}
	
	pNode = pInfo->selectSingleNode( "@train_name" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		lstrcpyn(pTrainInfo->m_data.train_name, CString(v.bstrVal), sizeof(pTrainInfo->m_data.train_name));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	pNode = pInfo->selectSingleNode( "@train_key" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		pTrainInfo->m_data.train_key = atol(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	pNode = pInfo->selectSingleNode( "@basic_train_index" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		pTrainInfo->m_data.basic_train_index = atol(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	pNode = pInfo->selectSingleNode( "@gpc_train_index" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		pTrainInfo->m_data.gpc_train_index = atol(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	pNode = pInfo->selectSingleNode( "@shift_train_index" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		pTrainInfo->m_data.shift_train_index = atol(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}

	memset(pTrainInfo->m_data.shift_string, 0, 32);
	pNode = pInfo->selectSingleNode( "@shift_index_string" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		CString strCZH = CString(v.bstrVal);
		SysFreeString(v.bstrVal);
		v.Clear();

		UINT len=strCZH.GetLength();
		if(len>0)
		{
			if(len<32)
				strncpy(pTrainInfo->m_data.shift_string, strCZH.GetBuffer(), len);
			else
				strncpy(pTrainInfo->m_data.shift_string, strCZH.GetBuffer(), 31);
		}
		pTrainInfo->m_data.shift_string[31]=0;
	}

	pNode = pInfo->selectSingleNode( "@type" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		pTrainInfo->m_data.type = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	pNode = pInfo->selectSingleNode( "@run_type" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		pTrainInfo->m_data.run_type = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	pNode = pInfo->selectSingleNode( "@start_plan_time" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		pTrainInfo->m_data.start_plan_time = atol(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	pNode = pInfo->selectSingleNode( "@start_actual_time" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		pTrainInfo->m_data.start_actual_time = atol(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}

	pNode = pInfo->selectSingleNode( "@sborder_plan_time" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		pTrainInfo->m_data.sborder_plan_time = atol(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	pNode = pInfo->selectSingleNode( "@sborder_actual_time" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		pTrainInfo->m_data.sborder_actual_time = atol(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	pNode = pInfo->selectSingleNode( "@eborder_plan_time" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		pTrainInfo->m_data.eborder_plan_time = atol(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	pNode = pInfo->selectSingleNode( "@eborder_actual_time" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		pTrainInfo->m_data.eborder_actual_time = atol(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	pNode = pInfo->selectSingleNode( "@sborder_sta" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		pTrainInfo->m_data.sborder_sta = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	pNode = pInfo->selectSingleNode( "@eborder_sta" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		pTrainInfo->m_data.eborder_sta = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	pNode = pInfo->selectSingleNode( "@start_sta_name" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		lstrcpyn(pTrainInfo->m_data.start_sta_name, CString(v.bstrVal), sizeof(pTrainInfo->m_data.start_sta_name));
		ZERO_STR_TAIL(pTrainInfo->m_data.start_sta_name);
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	pNode = pInfo->selectSingleNode( "@end_sta_name" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		lstrcpyn(pTrainInfo->m_data.end_sta_name, CString(v.bstrVal), sizeof(pTrainInfo->m_data.end_sta_name));
		ZERO_STR_TAIL(pTrainInfo->m_data.end_sta_name);
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	pNode = pInfo->selectSingleNode( "@train_name" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		lstrcpyn(pTrainInfo->m_data.train_name, CString(v.bstrVal), sizeof(pTrainInfo->m_data.train_name));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	pNode = pInfo->selectSingleNode( "@frotrain.m_conid" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		pTrainInfo->m_data.from_conid = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	pNode = pInfo->selectSingleNode( "@to_conid" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		pTrainInfo->m_data.to_conid = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	pNode = pInfo->selectSingleNode( "@priority" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		pTrainInfo->m_data.priority = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	pNode = pInfo->selectSingleNode( "@train_length" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		pTrainInfo->m_data.train_length = atol(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	pNode = pInfo->selectSingleNode( "@train_weight" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		pTrainInfo->m_data.train_weight = atol(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	pNode = pInfo->selectSingleNode( "@status" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		pTrainInfo->m_data.status = atol(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}

	pNode = pInfo->selectSingleNode( "@space_rank" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		pTrainInfo->m_data.space_rank = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	pNode = pInfo->selectSingleNode( "@length_rank" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		pTrainInfo->m_data.length_rank = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	pNode = pInfo->selectSingleNode( "@super_rank" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		pTrainInfo->m_data.super_rank = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	pNode = pInfo->selectSingleNode( "@military_rank" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		pTrainInfo->m_data.military_rank = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}

	pNode = pInfo->selectSingleNode( "@engine_utility" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		pTrainInfo->m_data.engine_utility = atol(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	pNode = pInfo->selectSingleNode( "@side_utility" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		pTrainInfo->m_data.side_utility = atol(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}

	pNode = pInfo->selectSingleNode( "@adjust_begin_sta" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		pTrainInfo->m_data.adjust_begin_sta = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	pNode = pInfo->selectSingleNode( "@adjust_end_sta" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		pTrainInfo->m_data.adjust_end_sta = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}

	pNode = pInfo->selectSingleNode( "@adjust_flag" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		pTrainInfo->m_data.adjust_flag = atol(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	pNode = pInfo->selectSingleNode( "@train_source" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		pTrainInfo->m_data.train_source = atol(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}

	pNode = pRootNode->selectSingleNode( "ExtraInfo/OPMSInfo" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		pTrainInfo->m_info=(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	

	// RECORDS
	MSXML::IXMLDOMNodeListPtr pRecAbsList = pRootNode->selectNodes( "RECORDS/RECORD" );
	int rec_count = pRecAbsList->Getlength();
	CTG_TrainRecordList oldrecord_list;
	oldrecord_list=*train.GetRecordList();  // 保留原记录中的信息
	train.GetRecordList()->PurgeRecords();  
	for(int idx = 0; idx < rec_count; idx++)
	{
		CTG_TrainRecord record;
		if(idx<oldrecord_list.GetRecordCount())
			record=oldrecord_list[idx];  // 保留原信息的基础上更新.
		if(TgTRAIN_RECORDfromXML(record, pRecAbsList->Getitem(idx)))
		    train.GetRecordList()->AddRecord(record);
	}
	train.SetTrainRecordCountToDefault();
	//train.GetTrainInfo()->m_data.delete_flag=rec_count;   
	//train.SetTrainRecordCount(rec_count);  // 2009.06.01 // 2009.06.2 造成死机！
	// ENGINES
	train.GetEngineList()->PurgeRecords();   //是否需要保存什么信息
	MSXML::IXMLDOMNodeListPtr pEngineAbsList = pRootNode->selectNodes( "ENGINES/ENGINE" );
	int engine_count = pEngineAbsList->Getlength();
	for(int idx = 0; idx < engine_count; idx++)
	{
		CTG_TrainEngineInfo tgEngineInfo;
		pNode = pEngineAbsList->Getitem(idx)->selectSingleNode( "@engine_id" );
		if(pNode)
		{
			v = pNode->GetnodeTypedValue();		
			lstrcpyn(tgEngineInfo.m_data.engine_id, CString(v.bstrVal), sizeof(tgEngineInfo.m_data.engine_id));
			SysFreeString(v.bstrVal);
			v.Clear();
		}
		pNode = pEngineAbsList->Getitem(idx)->selectSingleNode( "@engine_type" );
		if(pNode)
		{
			v = pNode->GetnodeTypedValue();		
			lstrcpyn(tgEngineInfo.m_data.engine_type, CString(v.bstrVal), sizeof(tgEngineInfo.m_data.engine_type));
			SysFreeString(v.bstrVal);
			v.Clear();
		}
		pNode = pEngineAbsList->Getitem(idx)->selectSingleNode( "@engine_status" );
		if(pNode)
		{
			v = pNode->GetnodeTypedValue();
			tgEngineInfo.m_data.engine_status = atol(CString(v.bstrVal));
			SysFreeString(v.bstrVal);
			v.Clear();
		}
		pNode = pEngineAbsList->Getitem(idx)->selectSingleNode( "@attach_sta" );
		if(pNode)
		{
			v = pNode->GetnodeTypedValue();
			tgEngineInfo.m_data.attach_sta = atoi(CString(v.bstrVal));
			SysFreeString(v.bstrVal);
			v.Clear();
		}
		pNode = pEngineAbsList->Getitem(idx)->selectSingleNode( "@unattach_sta" );
		if(pNode)
		{
			v = pNode->GetnodeTypedValue();
			tgEngineInfo.m_data.unattach_sta = atoi(CString(v.bstrVal));
			SysFreeString(v.bstrVal);
			v.Clear();
		}
		train.GetEngineList()->AddRecord(tgEngineInfo);
	}

	if(!fromFile)
	{
		return true; // 从OCX发出的xml中解析不到Expand,直接返回;只有从文件中保存的xml才能读到Expand
	}

	//Expand
	TG_TrainInfoExpand* pTrainExpand=train.GetTrainInfoExpand();
	pInfo = pRootNode->selectSingleNode( "RELATION");
	if(pInfo)
	{
		CTG_TrainRelation relation;
		pNode = pInfo->selectSingleNode( "@prev_train_index" );
		if(pNode)
		{
			v = pNode->GetnodeTypedValue();
			relation.prev_train_index = atol(CString((v.bstrVal)));
			SysFreeString(v.bstrVal);
			v.Clear();
		}
		pNode = pInfo->selectSingleNode( "@prev_basic_index" );
		if(pNode)
		{
			v = pNode->GetnodeTypedValue();
			relation.prev_basic_index = atol(CString((v.bstrVal)));
			SysFreeString(v.bstrVal);
			v.Clear();
		}
		pNode = pInfo->selectSingleNode( "@prev_shift_index" );
		if(pNode)
		{
			v = pNode->GetnodeTypedValue();
			relation.prev_shift_index = atol(CString((v.bstrVal)));
			SysFreeString(v.bstrVal);
			v.Clear();
		}
		pNode = pInfo->selectSingleNode( "@next_shift_index" );
		if(pNode)
		{
			v = pNode->GetnodeTypedValue();
			relation.next_shift_index = atol(CString((v.bstrVal)));
			SysFreeString(v.bstrVal);
			v.Clear();
		}
		pNode = pInfo->selectSingleNode( "@czh_no" );
		if(pNode)
		{
			v = pNode->GetnodeTypedValue();
			relation.czh_no_str=CString((v.bstrVal));
			SysFreeString(v.bstrVal);
			v.Clear();
		}
		pNode = pInfo->selectSingleNode( "@prev_dest_sta" );
		if(pNode)
		{
			v = pNode->GetnodeTypedValue();
			relation.prev_dest_sta = atol(CString((v.bstrVal)));
			SysFreeString(v.bstrVal);
			v.Clear();
		}
		pNode = pInfo->selectSingleNode( "@next_dest_sta" );
		if(pNode)
		{
			v = pNode->GetnodeTypedValue();
			relation.next_dest_sta = atol(CString((v.bstrVal)));
			SysFreeString(v.bstrVal);
			v.Clear();
		}
		pTrainExpand->UpdateTrainRelation(relation);
	}

	pInfo = pRootNode->selectSingleNode( "KEDIAO");
	if(pInfo)
	{
		CTG_TrainKediaoInfo kd;
		pNode = pInfo->selectSingleNode( "type" );
		if(pNode)
		{
			v = pNode->GetnodeTypedValue();
			kd.type = atol(CString((v.bstrVal)));
			SysFreeString(v.bstrVal);
			v.Clear();
		}
		pNode = pInfo->selectSingleNode( "context" );
		if(pNode)
		{
			v = pNode->GetnodeTypedValue();
			kd.content=CString(v.bstrVal);
			SysFreeString(v.bstrVal);
			v.Clear();
		}
		pTrainExpand->UpdateTrainKediao(kd);
	}

	MSXML::IXMLDOMNodePtr pChildNode;
	MSXML::IXMLDOMNodeListPtr pList;
	// YY_LIST
	pList = pRootNode->selectNodes( "YY_LIST/RECORD" );
	if(pList!=NULL)
	{
		CTG_TrainYingyeList yy;
		rec_count = pList->Getlength(); 
		for(int idx = 0; idx < rec_count; idx++)
		{
			TG_TrainYingyeInfo record;
			pNode = pList->Getitem(idx);

			pChildNode = pNode->selectSingleNode( "@arrive_time" );
			if(pChildNode)
			{
				v = pChildNode->GetnodeTypedValue();
				record.arrive_time=atol(CString((v.bstrVal)));
				SysFreeString(v.bstrVal);
				v.Clear();
			}
			pChildNode = pNode->selectSingleNode( "@depart_time" );
			if(pChildNode)
			{
				v = pChildNode->GetnodeTypedValue();
				record.depart_time=atol(CString((v.bstrVal)));
				SysFreeString(v.bstrVal);
				v.Clear();
			}
			pChildNode = pNode->selectSingleNode( "@station_no" );
			if(pChildNode)
			{
				v = pChildNode->GetnodeTypedValue();
				record.station_no=atol(CString((v.bstrVal)));
				SysFreeString(v.bstrVal);
				v.Clear();
			}
			pChildNode = pNode->selectSingleNode( "@station_name" );
			if(pChildNode)
			{
				v = pChildNode->GetnodeTypedValue();
				STR_CPY(record.station_name, CString(v.bstrVal).GetBuffer());
				SysFreeString(v.bstrVal);
				v.Clear();
			}

			yy.AddRecord(record);
		}
		pTrainExpand->UpdateTrainYingye(yy);
	}
	// CREW_LIST
	pList = pRootNode->selectNodes( "CREW_LIST/RECORD" );
	if(pList!=NULL)
	{
		CTG_TrainCrewList   crew;
		rec_count = pList->Getlength(); 
		for(int idx = 0; idx < rec_count; idx++)
		{
			TG_TrainCrewInfo record;
			pNode = pList->Getitem(idx);

			pChildNode = pNode->selectSingleNode( "@crew_type" );
			if(pChildNode)
			{
				v = pChildNode->GetnodeTypedValue();
				record.crew_type=atol(CString((v.bstrVal)));
				SysFreeString(v.bstrVal);
				v.Clear();
			}
			pChildNode = pNode->selectSingleNode( "@crew_bureau" );
			if(pChildNode)
			{
				v = pChildNode->GetnodeTypedValue();
				STR_CPY(record.crew_bureau, CString(v.bstrVal).GetBuffer());
				SysFreeString(v.bstrVal);
				v.Clear();
			}
			pChildNode = pNode->selectSingleNode( "@crew_info" );
			if(pChildNode)
			{
				v = pChildNode->GetnodeTypedValue();
				STR_CPY(record.crew_info, CString(v.bstrVal).GetBuffer());
				SysFreeString(v.bstrVal);
				v.Clear();
			}

			crew.AddRecord(record);
		}
		pTrainExpand->UpdateTrainCrew(crew);
	}

	// ENGINE_CREW_LIST
	pList = pRootNode->selectNodes( "ENGINE_CREW_LIST/RECORD" );
	if(pList!=NULL)
	{
		CTG_EngineCrewList  engine;
		rec_count = pList->Getlength(); 
		for(int idx = 0; idx < rec_count; idx++)
		{
			TG_EngineCrewInfo record;
			pNode = pList->Getitem(idx);

			pChildNode = pNode->selectSingleNode( "@overwork_time" );
			if(pChildNode)
			{
				v = pChildNode->GetnodeTypedValue();
				record.overwork_time=atol(CString((v.bstrVal)));
				SysFreeString(v.bstrVal);
				v.Clear();
			}
			pChildNode = pNode->selectSingleNode( "@station_no" );
			if(pChildNode)
			{
				v = pChildNode->GetnodeTypedValue();
				record.station_no=atol(CString((v.bstrVal)));
				SysFreeString(v.bstrVal);
				v.Clear();
			}
			pChildNode = pNode->selectSingleNode( "@station_name" );
			if(pChildNode)
			{
				v = pChildNode->GetnodeTypedValue();
				STR_CPY(record.station_name, CString(v.bstrVal).GetBuffer());
				SysFreeString(v.bstrVal);
				v.Clear();
			}

			engine.AddRecord(record);
		}
		pTrainExpand->UpdateTrainEngineCrew(engine);
	}
	// TICKET_LIST
	pList = pRootNode->selectNodes( "TICKET_LIST/RECORD" );
	if(pList!=NULL)
	{
		CTG_TrainTicketList ticket;
		rec_count = pList->Getlength(); 
		for(int idx = 0; idx < rec_count; idx++)
		{
			TG_TrainTicketInfo record;
			pNode = pList->Getitem(idx);

			pChildNode = pNode->selectSingleNode( "@down_station" );
			if(pChildNode)
			{
				v = pChildNode->GetnodeTypedValue();
				record.down_station=atol(CString((v.bstrVal)));
				SysFreeString(v.bstrVal);
				v.Clear();
			}
			pChildNode = pNode->selectSingleNode( "@down_station_index" );
			if(pChildNode)
			{
				v = pChildNode->GetnodeTypedValue();
				record.down_station_index=atol(CString((v.bstrVal)));
				SysFreeString(v.bstrVal);
				v.Clear();
			}
			pChildNode = pNode->selectSingleNode( "@down_station_name" );
			if(pChildNode)
			{
				v = pChildNode->GetnodeTypedValue();
				STR_CPY(record.down_station_name, CString(v.bstrVal).GetBuffer());
				SysFreeString(v.bstrVal);
				v.Clear();
			}

			pChildNode = pNode->selectSingleNode( "@on_station" );
			if(pChildNode)
			{
				v = pChildNode->GetnodeTypedValue();
				record.on_station=atol(CString((v.bstrVal)));
				SysFreeString(v.bstrVal);
				v.Clear();
			}
			pChildNode = pNode->selectSingleNode( "@on_station_index" );
			if(pChildNode)
			{
				v = pChildNode->GetnodeTypedValue();
				record.on_station_index=atol(CString((v.bstrVal)));
				SysFreeString(v.bstrVal);
				v.Clear();
			}
			pChildNode = pNode->selectSingleNode( "@on_station_name" );
			if(pChildNode)
			{
				v = pChildNode->GetnodeTypedValue();
				STR_CPY(record.on_station_name, CString(v.bstrVal).GetBuffer());
				SysFreeString(v.bstrVal);
				v.Clear();
			}

			pChildNode = pNode->selectSingleNode( "@sale" );
			if(pChildNode)
			{
				v = pChildNode->GetnodeTypedValue();
				record.sale=atol(CString((v.bstrVal)));
				SysFreeString(v.bstrVal);
				v.Clear();
			}
			pChildNode = pNode->selectSingleNode( "@seat" );
			if(pChildNode)
			{
				v = pChildNode->GetnodeTypedValue();
				STR_CPY(record.seat, CString(v.bstrVal).GetBuffer());
				SysFreeString(v.bstrVal);
				v.Clear();
			}

			ticket.AddRecord(record);
		}
		pTrainExpand->UpdateTrainTicket(ticket);
	}
	return true;
}

bool fromXML(TrainForExitEntry &train,const char *xml)
{
	MSXML::IXMLDOMDocumentPtr pXMLDoc;
	HRESULT hr = pXMLDoc.CreateInstance(MSXML::CLSID_DOMDocument);
	if(FAILED(hr))
	{
		_com_error er(hr);
		AfxMessageBox(er.ErrorMessage());
		return false;
	}
	
	if(VARIANT_TRUE != pXMLDoc->loadXML( _bstr_t( xml ) ) )
	{
		return false;
	}
	
	MSXML::IXMLDOMNodePtr pRootNode = pXMLDoc->selectSingleNode( "TRAIN");
	if(pRootNode == NULL)
	{
		return false;
	}

	MSXML::IXMLDOMNodePtr pNode, pChild;
	_variant_t v;

	// RECORDS
	MSXML::IXMLDOMNodeListPtr pRecAbsList = pRootNode->selectNodes( "RECORD" );
	int rec_count = pRecAbsList->Getlength();
	for(int idx = 0; idx < rec_count; idx++)
	{
		pNode = pRecAbsList->Getitem(idx);
		if(pNode!=NULL)
		{
			TmpRecord record;
			pChild = pNode->selectSingleNode( "@station" );
			if(pChild)
			{
				v = pChild->GetnodeTypedValue();
				record.station = atoi(CString(v.bstrVal));
				SysFreeString(v.bstrVal);
				v.Clear();
			}
			pChild = pNode->selectSingleNode( "@entry" );
			if(pChild)
			{
				v = pChild->GetnodeTypedValue();
				record.entry = atoi(CString(v.bstrVal));
				SysFreeString(v.bstrVal);
				v.Clear();
			}
			pChild = pNode->selectSingleNode( "@exit" );
			if(pChild)
			{
				v = pChild->GetnodeTypedValue();
				record.exit = atoi(CString(v.bstrVal));
				SysFreeString(v.bstrVal);
				v.Clear();
			}
			pChild = pNode->selectSingleNode( "@limit_speed" );
			if(pChild)
			{
				v = pChild->GetnodeTypedValue();
				record.limit_speed = atoi(CString(v.bstrVal));
				SysFreeString(v.bstrVal);
				v.Clear();
			}
			pChild = pNode->selectSingleNode( "@in_index" );
			if(pChild)
			{
				v = pChild->GetnodeTypedValue();
				record.in_index = atoi(CString(v.bstrVal));
				SysFreeString(v.bstrVal);
				v.Clear();
			}
			pChild = pNode->selectSingleNode( "@out_index" );
			if(pChild)
			{
				v = pChild->GetnodeTypedValue();
				record.out_index = atoi(CString(v.bstrVal));
				SysFreeString(v.bstrVal);
				v.Clear();
			}

			train.AddRecord(record);
		}
		else
		{
			return false;
		}
	}

	return true;
}

bool TgTRAIN_RECORDfromXML(CTG_TrainRecord& record, MSXML::IXMLDOMNodePtr pRoot)
{
	if(pRoot == NULL)  return false;
    
	_variant_t v;
	MSXML::IXMLDOMNodePtr pNode;

	
	pNode = pRoot->selectSingleNode( "@station" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		int tmpstation= atoi(CString(v.bstrVal));
		if(record.m_data.station!=0 && record.m_data.station!=tmpstation)
		{
			record.Init();  // CUIHU: 当原始记录中站号不一致时，信息全部丢弃
		}
		record.m_data.station=tmpstation;
		SysFreeString(v.bstrVal);
		v.Clear();
	}

	pNode = pRoot->selectSingleNode( "@modify_abs_runtime" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		record.m_modify_abs_time=atoi(CString(v.bstrVal))>0?TRUE:FALSE;
		SysFreeString(v.bstrVal);
		v.Clear();
	}

	pNode = pRoot->selectSingleNode( "@opms_station_text" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		record.SetStationText(CString(v.bstrVal), STATION_TXT_INFO_TYPE_OPMS);
		SysFreeString(v.bstrVal);
		v.Clear();
	}

	pNode = pRoot->selectSingleNode( "@station_text" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		record.SetStationText(CString(v.bstrVal), STATION_TXT_INFO_TYPE_SHUAIGUA);
		SysFreeString(v.bstrVal);
		v.Clear();
	}

	pNode = pRoot->selectSingleNode( "@opms_give_station_text" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		record.SetOpmsSationTextState(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}

	pNode = pRoot->selectSingleNode( "@abs_text" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		record.m_strAbsText = CString(v.bstrVal);
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	pNode = pRoot->selectSingleNode( "@alarm_text" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		record.m_strAlarmText = CString(v.bstrVal);
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	pNode = pRoot->selectSingleNode( "@note_text" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		record.m_strNoteText = CString(v.bstrVal);
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	
	pNode = pRoot->selectSingleNode( "@arrive_train_id" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		lstrcpyn(record.m_data.arrive_train_id, CString(v.bstrVal), sizeof(record.m_data.arrive_train_id));
		SysFreeString(v.bstrVal);
		v.Clear();
	}

	pNode = pRoot->selectSingleNode( "@original_arrive_train_id" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		lstrcpyn(record.m_data.original_arrive_train_id, CString(v.bstrVal), sizeof(record.m_data.original_arrive_train_id));
		SysFreeString(v.bstrVal);
		v.Clear();
	}

	pNode = pRoot->selectSingleNode( "@depart_train_id" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		lstrcpyn(record.m_data.depart_train_id, CString(v.bstrVal), sizeof(record.m_data.depart_train_id));
		SysFreeString(v.bstrVal);
		v.Clear();
	}

	pNode = pRoot->selectSingleNode( "@original_depart_train_id" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		lstrcpyn(record.m_data.original_depart_train_id, CString(v.bstrVal), sizeof(record.m_data.original_depart_train_id));
		SysFreeString(v.bstrVal);
		v.Clear();
	}

	pNode = pRoot->selectSingleNode( "@rec_index" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		//record.m_data.rec_index = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}

	pNode = pRoot->selectSingleNode( "@entry" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		record.m_data.entry = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	pNode = pRoot->selectSingleNode( "@exit" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		record.m_data.exit = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}


	pNode = pRoot->selectSingleNode( "@arrive_side" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		int sideno=atoi(CString(v.bstrVal));
		if(IS_VALID_SIDE(sideno))  // 20161105 非法股道固定设置为255
			record.m_data.arrive_side = sideno;
		else
			record.m_data.arrive_side = 255;
	
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	pNode = pRoot->selectSingleNode( "@plan_arrive_side" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		int sideno=atoi(CString(v.bstrVal));
		if(IS_VALID_SIDE(sideno))  // 20161105 非法股道固定设置为255
			record.m_data.plan_arrive_side = sideno;
		else
			record.m_data.plan_arrive_side = 255;

		SysFreeString(v.bstrVal);
		v.Clear();
	}	
	pNode = pRoot->selectSingleNode( "@depart_side" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		int sideno=atoi(CString(v.bstrVal));
		if(IS_VALID_SIDE(sideno))  // 20161105 非法股道固定设置为255
			record.m_data.depart_side = sideno;
		else
			record.m_data.depart_side = 255;

		SysFreeString(v.bstrVal);
		v.Clear();
	}
	pNode = pRoot->selectSingleNode( "@plan_depart_side" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		int sideno=atoi(CString(v.bstrVal));
		if(IS_VALID_SIDE(sideno))   // 20161105 非法股道固定设置为255
			record.m_data.plan_depart_side = sideno;
		else
			record.m_data.plan_depart_side = 255;

		SysFreeString(v.bstrVal);
		v.Clear();
	}			
	pNode = pRoot->selectSingleNode( "@min_stop_time" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		record.m_data.min_stop_time = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	
	pNode = pRoot->selectSingleNode( "@arrive" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		record.m_data.arrive = atol(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}	
	pNode = pRoot->selectSingleNode( "@depart" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		record.m_data.depart = atol(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}	
	pNode = pRoot->selectSingleNode( "@plan_arrive" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		record.m_data.plan_arrive = atol(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}	
	pNode = pRoot->selectSingleNode( "@plan_depart" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		record.m_data.plan_depart = atol(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}	
	
	pNode = pRoot->selectSingleNode( "@stop_reason" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		record.m_data.stop_reason = atol(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	pNode = pRoot->selectSingleNode( "@late_reason" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		record.m_data.late_reason = atol(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	pNode = pRoot->selectSingleNode( "@early_depart_time" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		record.m_data.early_depart_time = atol(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	pNode = pRoot->selectSingleNode( "@run_fast_slow_time" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		record.m_data.run_fast_slow_time = atol(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}

	pNode = pRoot->selectSingleNode( "@early_depart_time" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		record.m_data.early_depart_time = atol(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	pNode = pRoot->selectSingleNode( "@route_flag" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		record.m_data.route_flag = atol(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	pNode = pRoot->selectSingleNode( "@speed_limit" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		record.m_data.speed_limit = atol(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	pNode = pRoot->selectSingleNode( "@stop_condition" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		record.m_data.stop_condition = atol(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	pNode = pRoot->selectSingleNode( "@adjust_status" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		record.m_data.adjust_status = atol(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	pNode = pRoot->selectSingleNode( "@spec_run_time" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		record.m_data.spec_run_time = atol(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	pNode = pRoot->selectSingleNode( "@special_position" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		record.m_data.special_position = atol(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	pNode = pRoot->selectSingleNode( "@special_time_1" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		record.m_data.special_time_1 = atol(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	pNode = pRoot->selectSingleNode( "@special_time_2" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		record.m_data.special_time_2 = atol(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	
	pNode = pRoot->selectSingleNode( "@special_train_index" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		record.m_data.special_train_index = atol(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	pNode = pRoot->selectSingleNode( "@data_source" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		record.m_data.data_source = atol(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	pNode = pRoot->selectSingleNode( "@flag" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		record.m_data.flag = atol(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}

	pNode = pRoot->selectSingleNode( "@super_interval_ltime" );
	if(pNode)
		{
		v = pNode->GetnodeTypedValue();
		record.m_data.super_interval_ltime = atol(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
		}

	pNode = pRoot->selectSingleNode( "@in_station_index" );
	if(pNode)
		{
		v = pNode->GetnodeTypedValue();
		record.SetArriveTGIndex(atoi(CString(v.bstrVal)));
		SysFreeString(v.bstrVal);
		v.Clear();
		}
		
    pNode = pRoot->selectSingleNode( "@out_station_index" );
	if(pNode)
		{
		v = pNode->GetnodeTypedValue();
		record.SetDepartTGIndex(atoi(CString(v.bstrVal)));
		SysFreeString(v.bstrVal);
		v.Clear();
		}

	pNode = pRoot->selectSingleNode( "@can_not_meet" );
	if(pNode)
		{
		v = pNode->GetnodeTypedValue();
		record.m_data.can_not_meet=atol(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
		}
	pNode = pRoot->selectSingleNode( "@rank" );
	if(pNode)
		{
		v = pNode->GetnodeTypedValue();
		record.m_data.rank=atol(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
		}

	MSXML::IXMLDOMNodeListPtr pAbsStopList = pRoot->selectNodes( "ABS_STOP" );
	int abs_count = pAbsStopList->Getlength();
	for(int j= 0; j < abs_count; j++)
	{
		TG_TrainAbsStop absstop;

		pNode = pAbsStopList->Getitem(j)->selectSingleNode( "@flag" );
		if(pNode)
		{
			v = pNode->GetnodeTypedValue();
			absstop.flag = atoi(CString(v.bstrVal));
			SysFreeString(v.bstrVal);
			v.Clear();
		}
		pNode = pAbsStopList->Getitem(j)->selectSingleNode( "@position" );
		if(pNode)
		{
			v = pNode->GetnodeTypedValue();
			absstop.position = atoi(CString(v.bstrVal));
			SysFreeString(v.bstrVal);
			v.Clear();
		}
		pNode = pAbsStopList->Getitem(j)->selectSingleNode( "@start_time" );
		if(pNode)
		{
			v = pNode->GetnodeTypedValue();
			absstop.start_time = atol(CString(v.bstrVal));
			SysFreeString(v.bstrVal);
			v.Clear();
		}
		pNode = pAbsStopList->Getitem(j)->selectSingleNode( "@stop_time" );
		if(pNode)
		{
			v = pNode->GetnodeTypedValue();
			absstop.stop_time = atol(CString(v.bstrVal));
			SysFreeString(v.bstrVal);
			v.Clear();
		}

		
		record.m_listAbsStop.push_back(absstop);
	}

	return true;
}
CString toXMLString(const CTG_SingleMark &mark)
{
	CString xml,tmp;
	xml = "<MARK";
	{
		tmp.Format(" self_update = \"%d\"",mark.IsSelfUpdate());
		xml += tmp;

		tmp.Format(" mark_index=\"%d\"", mark.m_nBlockIndex);
		xml += tmp;

		tmp.Format(" basic_index = \"%d\"", mark.m_data.basic_index);
		xml += tmp;

		tmp.Format(" gpc_index = \"%d\"", mark.m_data.gpc_index);
		xml += tmp;

		tmp.Format(" shift_index = \"%d\"", mark.m_data.shift_index);
		xml += tmp;

		tmp.Format(" shift_index_string = \"%s\"", GetValidStr(mark.m_data.shift_string));
		xml += tmp;

		tmp.Format(" update_time = \"%d\"", mark.m_data.update_time);
		xml += tmp;

		tmp.Format(" start_time=\"%d\"",mark.m_data.start_time);
		xml += tmp;
		tmp.Format(" end_time=\"%d\"",mark.m_data.end_time);
		xml += tmp;

		tmp.Format(" type=\"%d\"",mark.m_data.type);
		xml += tmp;
		tmp.Format(" flag=\"%d\"",mark.m_data.flag);
		xml += tmp;

		tmp.Format(" line=\"%d\"",mark.m_data.line);
		xml += tmp;
		tmp.Format(" station=\"%d\"",mark.m_data.station);
		xml += tmp;
		tmp.Format(" direction=\"%d\"",mark.m_data.direction);
		xml += tmp;
		tmp.Format(" station1=\"%d\"",mark.m_data.station1);
		xml += tmp;
		tmp.Format(" station2=\"%d\"",mark.m_data.station2);
		xml += tmp;
		tmp.Format(" index=\"%d\"",mark.m_data.index);
		xml += tmp;

		tmp.Format(" position=\"%d\"",mark.m_data.position);
		xml += tmp;
		tmp.Format(" text=\"%s\"",GetValidStr(mark.m_data.text));
		xml += tmp;
		tmp.Format(" s_meter=\"%d\"",mark.m_data.s_meter);
		xml += tmp;
		tmp.Format(" e_meter=\"%d\"",mark.m_data.e_meter);
		xml += tmp;
		tmp.Format(" e_kilo=\"%d\"",mark.m_data.e_kilo);
		xml += tmp;
		tmp.Format(" slow_time=\"%d\"",mark.m_data.slow_time);
		xml += tmp;
		tmp.Format(" adjust_status=\"%d\"",mark.m_data.adjust_status);
		xml += tmp;
		
		xml += ">";

		for(int j= 0; j<mark.m_data.ele_count && (j < 512); j++)
		{
			tmp.Format("<ele_number ele_val=\"%d\" />",mark.m_data.ele_number[j]);
			xml += tmp;
		}
	}
	xml += "</MARK>";

	return xml;
}

bool fromXML(CTG_SingleMark &mark,const char *xml)
{
	MSXML::IXMLDOMDocumentPtr pXMLDoc;
	HRESULT hr = pXMLDoc.CreateInstance(MSXML::CLSID_DOMDocument);
	if(FAILED(hr))
	{
		_com_error er(hr);
		AfxMessageBox(er.ErrorMessage());
		return false;
	}
	
	if(VARIANT_TRUE != pXMLDoc->loadXML( _bstr_t( xml ) ) )
	{
		return false;
	}
	
	MSXML::IXMLDOMNodePtr pRootNode = pXMLDoc->selectSingleNode( "MARK");
	if(pRootNode == NULL)
	{
		return false;
	}


	MSXML::IXMLDOMNodePtr pNode;

	_variant_t v;
	pNode = pRootNode->selectSingleNode( "@mark_index" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		mark.m_nBlockIndex = atol(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	pNode = pRootNode->selectSingleNode( "@basic_index" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		mark.m_data.basic_index = atol(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	pNode = pRootNode->selectSingleNode( "@gpc_index" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		mark.m_data.gpc_index= atol(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	pNode = pRootNode->selectSingleNode( "@shift_index" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		mark.m_data.shift_index = atol(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	// <start_date>
	pNode = pRootNode->selectSingleNode( "@start_time" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		mark.m_data.start_time = atol(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	// <start_time>
	pNode = pRootNode->selectSingleNode( "@end_time" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		mark.m_data.end_time = atol(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	// <type>
	pNode = pRootNode->selectSingleNode( "@type" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		mark.m_data.type = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	// <flag>
	pNode = pRootNode->selectSingleNode( "@flag" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		mark.m_data.flag = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	// <dir>
	pNode = pRootNode->selectSingleNode( "@line" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		mark.m_data.line = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	// <station>
	// 站内封锁时的车站
	pNode = pRootNode->selectSingleNode( "@station" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		mark.m_data.station = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	// 区间封锁的车站
	pNode = pRootNode->selectSingleNode( "@station1" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		mark.m_data.station1 = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	pNode = pRootNode->selectSingleNode( "@station2" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		mark.m_data.station2 = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	pNode = pRootNode->selectSingleNode( "@direction" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		mark.m_data.direction  = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	// <index>
	pNode = pRootNode->selectSingleNode( "@index" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		mark.m_data.index = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}

	// <position>
	pNode = pRootNode->selectSingleNode( "@position" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		mark.m_data.position = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	// <text>
	pNode = pRootNode->selectSingleNode( "@text" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		lstrcpyn(mark.m_data.text, CString(v.bstrVal), sizeof(mark.m_data.text));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	// <s_meter>
	pNode = pRootNode->selectSingleNode( "@s_meter" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		mark.m_data.s_meter = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	// <e_meter>
	pNode = pRootNode->selectSingleNode( "@e_meter" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		mark.m_data.e_meter = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	// <e_kilo>
	pNode = pRootNode->selectSingleNode( "@e_kilo" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		mark.m_data.e_kilo = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	// <slow_time>
	pNode = pRootNode->selectSingleNode( "@slow_time" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		mark.m_data.slow_time = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	// <slow_time>
	pNode = pRootNode->selectSingleNode( "@adjust_status" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		mark.m_data.adjust_status = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	// <abs>
	MSXML::IXMLDOMNodeListPtr pAbsList = pRootNode->selectNodes( "ele_number" );
	int abs_no_count = pAbsList->Getlength();
	for(int idx = 0; idx < abs_no_count; idx++)
	{
		pNode = pAbsList->Getitem(idx)->selectSingleNode( "@ele_val" );
		if(pNode)
		{
			v = pNode->GetnodeTypedValue();
			mark.m_data.ele_number[idx] = atoi(CString(v.bstrVal));
			SysFreeString(v.bstrVal);
			v.Clear();
		}
	}
	mark.m_data.ele_count = abs_no_count;

	return true;
}

bool AlarmfromXml(CString xml, CStringArray& group, vector<int>& list_station, vector<int>& list_rec_index)
{
	group.RemoveAll();
	list_station.clear();
	list_rec_index.clear();
	MSXML::IXMLDOMDocumentPtr pXMLDoc;
	HRESULT hr = pXMLDoc.CreateInstance(MSXML::CLSID_DOMDocument);
	if(FAILED(hr))
	{
		_com_error er(hr);
		AfxMessageBox(er.ErrorMessage());
		return false;
	}
	
	if(VARIANT_TRUE != pXMLDoc->loadXML( _bstr_t( xml ) ) )
	{
		return false;
	}
	
	MSXML::IXMLDOMNodePtr pRootNode = pXMLDoc->selectSingleNode( "root");
	if(pRootNode == NULL)
	{
		return false;
	}
	
	MSXML::IXMLDOMNodeListPtr pAlarmList = pRootNode->selectNodes( "record" );
	if(pAlarmList == NULL)
	{
		return false;
	}
	int count = pAlarmList->Getlength();
	
	MSXML::IXMLDOMNodePtr pNode, pChildNode;
	for(int idx = 0; idx < count; idx++)
	{
		pNode = pAlarmList->Getitem(idx);
		if(NULL==pNode)
		{
			continue;
		}
        
		int station=-1, rec_index=-1;
		pChildNode=pNode->selectSingleNode("@station");
		if(pChildNode)
		{
		   _variant_t v;
			v = pChildNode->GetnodeTypedValue();		
			station = atoi(CString(v.bstrVal));
			SysFreeString(v.bstrVal);
			v.Clear();
		}
		pChildNode=pNode->selectSingleNode("@rec_index");
		if(pChildNode)
		{
		   _variant_t v;
			v = pChildNode->GetnodeTypedValue();		
			rec_index = atoi(CString(v.bstrVal));
			SysFreeString(v.bstrVal);
			v.Clear();
		}

		if(station>0 && rec_index>=0)
		{
			list_station.push_back(station);
			list_rec_index.push_back(rec_index);
		}

		MSXML::IXMLDOMNodeListPtr pList = pNode->selectNodes( "alarm" );
		int count = pList->Getlength();
		for(int n = 0; n < count; n++)
		{
			pChildNode = pList->Getitem(n);
			if(pChildNode)
			{
				_variant_t v;
				v = pChildNode->GetnodeTypedValue();		
				CString str = v.bstrVal;
				SysFreeString(v.bstrVal);
				v.Clear();

				if(str!="")
				{
					group.Add(str);
				}
			}
		}
	}
	return true;
}

bool GetPrevAndAfterTrainIndex(CString xml, TRAIN_INDEX& prev_train_index, TRAIN_INDEX& next_train_index)
{
	prev_train_index=0;
	next_train_index=0;
	MSXML::IXMLDOMDocumentPtr pXMLDoc;
	HRESULT hr = pXMLDoc.CreateInstance(MSXML::CLSID_DOMDocument);
	if(FAILED(hr))
	{
		_com_error er(hr);
		AfxMessageBox(er.ErrorMessage());
		return false;
	}
	
	if(VARIANT_TRUE != pXMLDoc->loadXML( _bstr_t( xml ) ) )
	{
		return false;
	}
	
	MSXML::IXMLDOMNodePtr pRootNode = pXMLDoc->selectSingleNode( "root");
	if(pRootNode == NULL)
	{
		return false;
	}
	
	MSXML::IXMLDOMNodePtr pNode;
	pNode = pRootNode->selectSingleNode( "prev_train_index");
	if(pNode)
	{
		_variant_t v;
		v = pNode->GetnodeTypedValue();		
		CString str = v.bstrVal;
		prev_train_index = atol(str);
		SysFreeString(v.bstrVal);
		v.Clear();
	}

	pNode = pRootNode->selectSingleNode( "next_train_index");
	if(pNode)
	{
		_variant_t v;
		v = pNode->GetnodeTypedValue();		
		CString str = v.bstrVal;
		next_train_index = atol(str);
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	
	return true;
}