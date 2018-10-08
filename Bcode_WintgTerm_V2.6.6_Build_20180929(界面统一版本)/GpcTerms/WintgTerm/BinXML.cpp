#include "StdAfx.h"
#include "binxml.h"

CString ifTRAINRECORD::toXMLString()
{
	assert(recs.size() > 0);
	CString xml,tmp;
	for(int idx = 0; idx < recs.size(); idx++)
	{

		TRAINRECORD record = recs[idx];
		xml += "<TRAINRECORD ";
		{
			tmp.Format(" rec_index=\"%d\"",record.rec_index);
			xml += tmp;
			tmp.Format(" station=\"%d\"",record.station);
			xml += tmp;
			tmp.Format(" entry=\"%d\"",record.entry);
			xml += tmp;
			tmp.Format(" exit=\"%d\"",record.exit);
			xml += tmp;
			tmp.Format(" arrive_side=\"%d\"",record.arrive_side);

			xml += tmp;
			tmp.Format(" plan_arrive_side=\"%d\"",record.plan_arrive_side);
			xml += tmp;
			tmp.Format(" depart_side=\"%d\"",record.depart_side);
			xml += tmp;
			tmp.Format(" plan_depart_side=\"%d\"",record.plan_depart_side);
			xml += tmp;
			tmp.Format(" status=\"%d\"",record.status);
			xml += tmp;
			tmp.Format(" min_stop_time=\"%d\"",record.min_stop_time);
			xml += tmp;

			tmp.Format(" arrival_date=\"%d\"",record.arrival_date);
			xml += tmp;
			tmp.Format(" arrival_time=\"%d\"",record.arrival_time);
			xml += tmp;
			tmp.Format(" plan_arrival_date=\"%d\"",record.plan_arrival_date);
			xml += tmp;
			tmp.Format(" plan_arrival_time=\"%d\"",record.plan_arrival_time);
			xml += tmp;
			tmp.Format(" depart_date=\"%d\"",record.depart_date);

			xml += tmp;
			tmp.Format(" depart_time=\"%d\"",record.depart_time);
			xml += tmp;
			tmp.Format(" plan_depart_date=\"%d\"",record.plan_depart_date);
			xml += tmp;
			tmp.Format(" plan_depart_time=\"%d\"",record.plan_depart_time);
			xml += tmp;
			tmp.Format(" additional_abs_time=\"%d\"",record.additional_abs_time);
			xml += tmp;
			tmp.Format(" stop_reason=\"%d\"",record.stop_reason);
			xml += tmp;

			tmp.Format(" late_reason=\"%d\"",record.late_reason);
			xml += tmp;
			tmp.Format(" early_depart_time=\"%d\"",record.early_depart_time);
			xml += tmp;
			tmp.Format(" run_fast_slow_time=\"%d\"",record.run_fast_slow_time);
			xml += tmp;
			tmp.Format(" stat_flag =\"%d\"",record.stat_flag );
			xml += tmp;
			tmp.Format(" speed_limit=\"%d\"",record.speed_limit);

			xml += tmp;
			tmp.Format(" stop_condition=\"%d\"",record.stop_condition);
			xml += tmp;
			tmp.Format(" adjust_status=\"%d\"",record.adjust_status);
			xml += tmp;
			tmp.Format(" side_utility=\"%d\"",record.side_utility);
			xml += tmp;
			tmp.Format(" stop_abs_position=\"%d\"",record.stop_abs_position);
			xml += tmp;
			tmp.Format(" stop_abs_s_time=\"%d\"",record.stop_abs_s_time);
			xml += tmp;

			tmp.Format(" stop_abs_e_time=\"%d\"",record.stop_abs_e_time);
			xml += tmp;
			tmp.Format(" special_train_index=\"%d\"",record.special_train_index);
			xml += tmp;
			tmp.Format(" data_source=\"%d\"",record.data_source);
			xml += tmp;
			tmp.Format(" flag=\"%d\"",record.flag);
			xml += tmp;
			tmp.Format(" spare=\"%d\"",record.spare);
			xml += tmp;

			tmp.Format(" local_stat_arrival=\"%d\"",record.local_stat_arrival);
			xml += tmp;
			tmp.Format(" local_stat_depart=\"%d\"",record.local_stat_depart);
			xml += tmp;
		}
		xml += "/>";
	}
	return xml;
}
bool ifTRAINRECORD::fromXML(MSXML::IXMLDOMNodePtr pRoot)
{
	MSXML::IXMLDOMNodePtr pNode,pRootNode;
	
	MSXML::IXMLDOMNodeListPtr pRecordList = pRoot->selectNodes( "TRAINRECORD");
	if(NULL == pRecordList)
	{
		return false;
	}

	int record_count = pRecordList->Getlength();
	if(record_count == 0)
	{
		return false;
	}

	_variant_t v;
	for(int i = 0; i < record_count; i++)
	{
		TRAINRECORD record;
		memset(&record, 0 , sizeof(TRAINRECORD));

		pNode = pRecordList->Getitem(i)->selectSingleNode( "@rec_index" );
		if(pNode)
		{
			v = pNode->GetnodeTypedValue();
			record.rec_index = atoi(CString(v.bstrVal));
			SysFreeString(v.bstrVal);
			v.Clear();
		}
		pNode = pRecordList->Getitem(i)->selectSingleNode( "@station" );
		if(pNode)
		{
			v = pNode->GetnodeTypedValue();
			record.station = atoi(CString(v.bstrVal));
			SysFreeString(v.bstrVal);
			v.Clear();
		}
		pNode = pRecordList->Getitem(i)->selectSingleNode( "@entry" );
		if(pNode)
		{
			v = pNode->GetnodeTypedValue();
			record.entry = atoi(CString(v.bstrVal));
			SysFreeString(v.bstrVal);
			v.Clear();
		}
		pNode = pRecordList->Getitem(i)->selectSingleNode( "@exit" );
		if(pNode)
		{
			v = pNode->GetnodeTypedValue();
			record.exit = atoi(CString(v.bstrVal));
			SysFreeString(v.bstrVal);
			v.Clear();
		}
		pNode = pRecordList->Getitem(i)->selectSingleNode( "@arrive_side" );
		if(pNode)
		{
			v = pNode->GetnodeTypedValue();
			record.arrive_side = atoi(CString(v.bstrVal));
			SysFreeString(v.bstrVal);
			v.Clear();
		}
		pNode = pRecordList->Getitem(i)->selectSingleNode( "@plan_arrive_side" );
		if(pNode)
		{
			v = pNode->GetnodeTypedValue();
			record.plan_arrive_side = atoi(CString(v.bstrVal));
			SysFreeString(v.bstrVal);
			v.Clear();
		}
		pNode = pRecordList->Getitem(i)->selectSingleNode( "@depart_side" );
		if(pNode)
		{
			v = pNode->GetnodeTypedValue();
			record.depart_side = atoi(CString(v.bstrVal));
			SysFreeString(v.bstrVal);
			v.Clear();
		}
		pNode = pRecordList->Getitem(i)->selectSingleNode( "@plan_depart_side" );
		if(pNode)
		{
			v = pNode->GetnodeTypedValue();
			record.plan_depart_side = atoi(CString(v.bstrVal));
			SysFreeString(v.bstrVal);
			v.Clear();
		}
		pNode = pRecordList->Getitem(i)->selectSingleNode( "@status" );
		if(pNode)
		{
			v = pNode->GetnodeTypedValue();
			record.status = atoi(CString(v.bstrVal));
			SysFreeString(v.bstrVal);
			v.Clear();
		}
		pNode = pRecordList->Getitem(i)->selectSingleNode( "@min_stop_time" );
		if(pNode)
		{
			v = pNode->GetnodeTypedValue();
			record.min_stop_time = atoi(CString(v.bstrVal));
			SysFreeString(v.bstrVal);
			v.Clear();
		}
		pNode = pRecordList->Getitem(i)->selectSingleNode( "@arrival_date" );
		if(pNode)
		{
			v = pNode->GetnodeTypedValue();
			record.arrival_date = atoi(CString(v.bstrVal));
			SysFreeString(v.bstrVal);
			v.Clear();
		}
		pNode = pRecordList->Getitem(i)->selectSingleNode( "@arrival_time" );
		if(pNode)
		{
			v = pNode->GetnodeTypedValue();
			record.arrival_time = atoi(CString(v.bstrVal));
			SysFreeString(v.bstrVal);
			v.Clear();
		}
		pNode = pRecordList->Getitem(i)->selectSingleNode( "@plan_arrival_date" );
		if(pNode)
		{
			v = pNode->GetnodeTypedValue();
			record.plan_arrival_date = atoi(CString(v.bstrVal));
			SysFreeString(v.bstrVal);
			v.Clear();
		}
		pNode = pRecordList->Getitem(i)->selectSingleNode( "@plan_arrival_time" );
		if(pNode)
		{
			v = pNode->GetnodeTypedValue();
			record.plan_arrival_time = atoi(CString(v.bstrVal));
			SysFreeString(v.bstrVal);
			v.Clear();
		}

		pNode = pRecordList->Getitem(i)->selectSingleNode( "@depart_date" );
		if(pNode)
		{
			v = pNode->GetnodeTypedValue();
			record.depart_date = atoi(CString(v.bstrVal));
			SysFreeString(v.bstrVal);
			v.Clear();
		}
		pNode = pRecordList->Getitem(i)->selectSingleNode( "@depart_time" );
		if(pNode)
		{
			v = pNode->GetnodeTypedValue();
			record.depart_time = atoi(CString(v.bstrVal));
			SysFreeString(v.bstrVal);
			v.Clear();
		}
		pNode = pRecordList->Getitem(i)->selectSingleNode( "@plan_depart_date" );
		if(pNode)
		{
			v = pNode->GetnodeTypedValue();
			record.plan_depart_date = atoi(CString(v.bstrVal));
			SysFreeString(v.bstrVal);
			v.Clear();
		}
		pNode = pRecordList->Getitem(i)->selectSingleNode( "@plan_depart_time" );
		if(pNode)
		{
			v = pNode->GetnodeTypedValue();
			record.plan_depart_time = atoi(CString(v.bstrVal));
			SysFreeString(v.bstrVal);
			v.Clear();
		}
		pNode = pRecordList->Getitem(i)->selectSingleNode( "@additional_abs_time" );
		if(pNode)
		{
			v = pNode->GetnodeTypedValue();
			record.additional_abs_time = atoi(CString(v.bstrVal));
			SysFreeString(v.bstrVal);
			v.Clear();
		}
		pNode = pRecordList->Getitem(i)->selectSingleNode( "@stop_reason" );
		if(pNode)
		{
			v = pNode->GetnodeTypedValue();
			record.stop_reason = atoi(CString(v.bstrVal));
			SysFreeString(v.bstrVal);
			v.Clear();
		}
		pNode = pRecordList->Getitem(i)->selectSingleNode( "@late_reason" );
		if(pNode)
		{
			v = pNode->GetnodeTypedValue();
			record.late_reason = atoi(CString(v.bstrVal));
			SysFreeString(v.bstrVal);
			v.Clear();
		}
		pNode = pRecordList->Getitem(i)->selectSingleNode( "@early_depart_time" );
		if(pNode)
		{
			v = pNode->GetnodeTypedValue();
			record.early_depart_time = atoi(CString(v.bstrVal));
			SysFreeString(v.bstrVal);
			v.Clear();
		}
		pNode = pRecordList->Getitem(i)->selectSingleNode( "@run_fast_slow_time" );
		if(pNode)
		{
			v = pNode->GetnodeTypedValue();
			record.run_fast_slow_time = atoi(CString(v.bstrVal));
			SysFreeString(v.bstrVal);
			v.Clear();
		}
		pNode = pRecordList->Getitem(i)->selectSingleNode( "@stat_flag" );
		if(pNode)
		{
			v = pNode->GetnodeTypedValue();
			record.stat_flag = atoi(CString(v.bstrVal));
			SysFreeString(v.bstrVal);
			v.Clear();
		}
		pNode = pRecordList->Getitem(i)->selectSingleNode( "@speed_limit" );
		if(pNode)
		{
			v = pNode->GetnodeTypedValue();
			record.speed_limit = atoi(CString(v.bstrVal));
			SysFreeString(v.bstrVal);
			v.Clear();
		}
		pNode = pRecordList->Getitem(i)->selectSingleNode( "@stop_condition" );
		if(pNode)
		{
			v = pNode->GetnodeTypedValue();
			record.stop_condition = atoi(CString(v.bstrVal));
			SysFreeString(v.bstrVal);
			v.Clear();
		}
		pNode = pRecordList->Getitem(i)->selectSingleNode( "@adjust_status" );
		if(pNode)
		{
			v = pNode->GetnodeTypedValue();
			record.adjust_status = atoi(CString(v.bstrVal));
			SysFreeString(v.bstrVal);
			v.Clear();
		}
		pNode = pRecordList->Getitem(i)->selectSingleNode( "@side_utility" );
		if(pNode)
		{
			v = pNode->GetnodeTypedValue();
			record.side_utility = atoi(CString(v.bstrVal));
			SysFreeString(v.bstrVal);
			v.Clear();
		}
		pNode = pRecordList->Getitem(i)->selectSingleNode( "@stop_abs_position" );
		if(pNode)
		{
			v = pNode->GetnodeTypedValue();
			record.stop_abs_position = atoi(CString(v.bstrVal));
			SysFreeString(v.bstrVal);
			v.Clear();
		}
		pNode = pRecordList->Getitem(i)->selectSingleNode( "@stop_abs_s_time" );
		if(pNode)
		{
			v = pNode->GetnodeTypedValue();
			record.stop_abs_s_time = atoi(CString(v.bstrVal));
			SysFreeString(v.bstrVal);
			v.Clear();
		}
		pNode = pRecordList->Getitem(i)->selectSingleNode( "@stop_abs_e_time" );
		if(pNode)
		{
			v = pNode->GetnodeTypedValue();
			record.stop_abs_e_time = atoi(CString(v.bstrVal));
			SysFreeString(v.bstrVal);
			v.Clear();
		}
		pNode = pRecordList->Getitem(i)->selectSingleNode( "@special_train_index" );
		if(pNode)
		{
			v = pNode->GetnodeTypedValue();
			record.special_train_index = atoi(CString(v.bstrVal));
			SysFreeString(v.bstrVal);
			v.Clear();
		}
		pNode = pRecordList->Getitem(i)->selectSingleNode( "@data_source" );
		if(pNode)
		{
			v = pNode->GetnodeTypedValue();
			record.data_source = atoi(CString(v.bstrVal));
			SysFreeString(v.bstrVal);
			v.Clear();
		}
		pNode = pRecordList->Getitem(i)->selectSingleNode( "@flag" );
		if(pNode)
		{
			v = pNode->GetnodeTypedValue();
			record.flag = atoi(CString(v.bstrVal));
			SysFreeString(v.bstrVal);
			v.Clear();
		}
		pNode = pRecordList->Getitem(i)->selectSingleNode( "@spare" );
		if(pNode)
		{
			v = pNode->GetnodeTypedValue();
			record.spare = atoi(CString(v.bstrVal));
			SysFreeString(v.bstrVal);
			v.Clear();
		}
		pNode = pRecordList->Getitem(i)->selectSingleNode( "@local_stat_arrival" );
		if(pNode)
		{
			v = pNode->GetnodeTypedValue();
			record.local_stat_arrival = atoi(CString(v.bstrVal));
			SysFreeString(v.bstrVal);
			v.Clear();
		}
		pNode = pRecordList->Getitem(i)->selectSingleNode( "@local_stat_depart" );
		if(pNode)
		{
			v = pNode->GetnodeTypedValue();
			record.local_stat_depart = atoi(CString(v.bstrVal));
			SysFreeString(v.bstrVal);
			v.Clear();
		}
		
		recs.push_back(record);
	}

	return true;
}

CString ifONETRAINRECORD::toXMLString()
{
	CString xml,tmp;

	xml = "<TRAINRECORD ";
	{
		tmp.Format(" rec_index=\"%d\"",rec.rec_index);
		xml += tmp;
		tmp.Format(" station=\"%d\"",rec.station);
		xml += tmp;
		tmp.Format(" entry=\"%d\"",rec.entry);
		xml += tmp;
		tmp.Format(" exit=\"%d\"",rec.exit);
		xml += tmp;
		tmp.Format(" arrive_side=\"%d\"",rec.arrive_side);

		xml += tmp;
		tmp.Format(" plan_arrive_side=\"%d\"",rec.plan_arrive_side);
		xml += tmp;
		tmp.Format(" depart_side=\"%d\"",rec.depart_side);
		xml += tmp;
		tmp.Format(" plan_depart_side=\"%d\"",rec.plan_depart_side);
		xml += tmp;
		tmp.Format(" status=\"%d\"",rec.status);
		xml += tmp;
		tmp.Format(" min_stop_time=\"%d\"",rec.min_stop_time);
		xml += tmp;

		tmp.Format(" arrival_date=\"%d\"",rec.arrival_date);
		xml += tmp;
		tmp.Format(" arrival_time=\"%d\"",rec.arrival_time);
		xml += tmp;
		tmp.Format(" plan_arrival_date=\"%d\"",rec.plan_arrival_date);
		xml += tmp;
		tmp.Format(" plan_arrival_time=\"%d\"",rec.plan_arrival_time);
		xml += tmp;
		tmp.Format(" depart_date=\"%d\"",rec.depart_date);

		xml += tmp;
		tmp.Format(" depart_time=\"%d\"",rec.depart_time);
		xml += tmp;
		tmp.Format(" plan_depart_date=\"%d\"",rec.plan_depart_date);
		xml += tmp;
		tmp.Format(" plan_depart_time=\"%d\"",rec.plan_depart_time);
		xml += tmp;
		tmp.Format(" additional_abs_time=\"%d\"",rec.additional_abs_time);
		xml += tmp;
		tmp.Format(" stop_reason=\"%d\"",rec.stop_reason);
		xml += tmp;

		tmp.Format(" late_reason=\"%d\"",rec.late_reason);
		xml += tmp;
		tmp.Format(" early_depart_time=\"%d\"",rec.early_depart_time);
		xml += tmp;
		tmp.Format(" run_fast_slow_time=\"%d\"",rec.run_fast_slow_time);
		xml += tmp;
		tmp.Format(" stat_flag =\"%d\"",rec.stat_flag );
		xml += tmp;
		tmp.Format(" speed_limit=\"%d\"",rec.speed_limit);

		xml += tmp;
		tmp.Format(" stop_condition=\"%d\"",rec.stop_condition);
		xml += tmp;
		tmp.Format(" adjust_status=\"%d\"",rec.adjust_status);
		xml += tmp;
		tmp.Format(" side_utility=\"%d\"",rec.side_utility);
		xml += tmp;
		tmp.Format(" stop_abs_position=\"%d\"",rec.stop_abs_position);
		xml += tmp;
		tmp.Format(" stop_abs_s_time=\"%d\"",rec.stop_abs_s_time);
		xml += tmp;

		tmp.Format(" stop_abs_e_time=\"%d\"",rec.stop_abs_e_time);
		xml += tmp;
		tmp.Format(" special_train_index=\"%d\"",rec.special_train_index);
		xml += tmp;
		tmp.Format(" data_source=\"%d\"",rec.data_source);
		xml += tmp;
		tmp.Format(" flag=\"%d\"",rec.flag);
		xml += tmp;
		tmp.Format(" spare=\"%d\"",rec.spare);
		xml += tmp;
	}
	xml += "/>";
	return xml;
}
bool ifONETRAINRECORD::fromXML(MSXML::IXMLDOMNodePtr pRoot)
{
	MSXML::IXMLDOMNodePtr pNode;
	_variant_t v;
	pNode = pRoot->selectSingleNode( "@rec_index" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		rec.rec_index = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	pNode = pRoot->selectSingleNode( "@station" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		rec.station = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	pNode = pRoot->selectSingleNode( "@entry" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		rec.entry = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	pNode = pRoot->selectSingleNode( "@exit" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		rec.exit = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	pNode = pRoot->selectSingleNode( "@arrive_side" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		rec.arrive_side = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	pNode = pRoot->selectSingleNode( "@plan_arrive_side" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		rec.plan_arrive_side = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	pNode = pRoot->selectSingleNode( "@depart_side" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		rec.depart_side = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	pNode = pRoot->selectSingleNode( "@plan_depart_side" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		rec.plan_depart_side = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	pNode = pRoot->selectSingleNode( "@status" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		rec.status = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	pNode = pRoot->selectSingleNode( "@min_stop_time" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		rec.min_stop_time = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	pNode = pRoot->selectSingleNode( "@arrival_date" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		rec.arrival_date = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	pNode = pRoot->selectSingleNode( "@arrival_time" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		rec.arrival_time = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	pNode = pRoot->selectSingleNode( "@plan_arrival_date" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		rec.plan_arrival_date = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	pNode = pRoot->selectSingleNode( "@plan_arrival_time" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		rec.plan_arrival_time = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}

	pNode = pRoot->selectSingleNode( "@depart_date" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		rec.depart_date = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	pNode = pRoot->selectSingleNode( "@depart_time" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		rec.depart_time = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	pNode = pRoot->selectSingleNode( "@plan_depart_date" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		rec.plan_depart_date = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	pNode = pRoot->selectSingleNode( "@plan_depart_time" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		rec.plan_depart_time = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	pNode = pRoot->selectSingleNode( "@additional_abs_time" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		rec.additional_abs_time = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	pNode = pRoot->selectSingleNode( "@stop_reason" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		rec.stop_reason = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	pNode = pRoot->selectSingleNode( "@late_reason" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		rec.late_reason = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	pNode = pRoot->selectSingleNode( "@early_depart_time" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		rec.early_depart_time = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	pNode = pRoot->selectSingleNode( "@run_fast_slow_time" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		rec.run_fast_slow_time = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	pNode = pRoot->selectSingleNode( "@stat_flag" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		rec.stat_flag = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	pNode = pRoot->selectSingleNode( "@speed_limit" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		rec.speed_limit = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	pNode = pRoot->selectSingleNode( "@stop_condition" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		rec.stop_condition = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	pNode = pRoot->selectSingleNode( "@adjust_status" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		rec.adjust_status = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	pNode = pRoot->selectSingleNode( "@side_utility" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		rec.side_utility = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	pNode = pRoot->selectSingleNode( "@stop_abs_position" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		rec.stop_abs_position = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	pNode = pRoot->selectSingleNode( "@stop_abs_s_time" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		rec.stop_abs_s_time = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	pNode = pRoot->selectSingleNode( "@stop_abs_e_time" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		rec.stop_abs_e_time = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	pNode = pRoot->selectSingleNode( "@special_train_index" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		rec.special_train_index = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	pNode = pRoot->selectSingleNode( "@data_source" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		rec.data_source = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	pNode = pRoot->selectSingleNode( "@flag" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		rec.flag = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	pNode = pRoot->selectSingleNode( "@spare" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		rec.spare = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	return true;
}

CString ifTRAININFO::toXMLString()
{
	CString xml,tmp;
	xml = "<MORE_TRAIN_INFO";
	{
		tmp.Format(" sborder_plan_date=\"%d\"",info.sborder_plan_date);
		xml += tmp;
		tmp.Format(" sborder_plan_time=\"%d\"",info.sborder_plan_time);
		xml += tmp;
		tmp.Format(" sborder_actual_date=\"%d\"",info.sborder_actual_date);
		xml += tmp;
		tmp.Format(" sborder_actual_time=\"%d\"",info.sborder_actual_time);
		xml += tmp;
		tmp.Format(" eborder_plan_date=\"%d\"",info.eborder_plan_date);
		xml += tmp;
		tmp.Format(" eborder_plan_time=\"%d\"",info.eborder_plan_time);
		xml += tmp;
		tmp.Format(" eborder_actual_date=\"%d\"",info.eborder_actual_date);
		xml += tmp;
		tmp.Format(" eborder_actual_time=\"%d\"",info.eborder_actual_time);
		xml += tmp;
		tmp.Format(" sborder_sta=\"%d\"",info.sborder_sta);
		xml += tmp;
		tmp.Format(" eborder_sta=\"%d\"",info.eborder_sta);
		xml += tmp;
		tmp.Format(" start_sta_name=\"%s\"",info.start_sta_name);
		xml += tmp;
		tmp.Format(" end_sta_name=\"%s\"",info.end_sta_name);
		xml += tmp;
		tmp.Format(" basic_train_index=\"%d\"",info.basic_train_index);
		xml += tmp;
		tmp.Format(" direction=\"%d\"",info.direction);
		xml += tmp;

		tmp.Format(" type=\"%d\"",info.type);
		xml += tmp;
		tmp.Format(" run_type=\"%d\"",info.run_type);
		xml += tmp;
		tmp.Format(" length=\"%d\"",info.length);
		xml += tmp;
		tmp.Format(" train_weight=\"%d\"",info.train_weight);
		xml += tmp;
		tmp.Format(" priority=\"%d\"",info.priority);
		xml += tmp;
		tmp.Format(" engine_utility=\"%d\"",info.engine_utility);
		xml += tmp;
		tmp.Format(" side_utility=\"%d\"",info.side_utility);
		xml += tmp;
		tmp.Format(" space_rank=\"%d\"",info.space_rank);
		xml += tmp;
		tmp.Format(" length_rank=\"%d\"",info.length_rank);
		xml += tmp;
		tmp.Format(" super_rank=\"%d\"",info.super_rank);
		xml += tmp;
		tmp.Format(" military_rank=\"%d\"",info.military_rank);
		xml += tmp;
		tmp.Format(" speed_limit=\"%d\"",info.speed_limit);
		xml += tmp;
		tmp.Format(" no_meet_flag=\"%d\"",info.no_meet_flag);
		xml += tmp;
		tmp.Format(" super_interval_ltime=\"%d\"",info.super_interval_ltime);
		xml += tmp;
		tmp.Format(" status=\"%d\"",info.status);
		xml += tmp;

		tmp.Format(" adjust_begin_sta=\"%d\"",info.adjust_begin_sta);
		xml += tmp;
		tmp.Format(" adjust_end_sta=\"%d\"",info.adjust_end_sta);
		xml += tmp;
		tmp.Format(" train_source=\"%d\"",info.train_source);
		xml += tmp;
		//tmp.Format(" spare=\"%s\"",info.spare);
		//xml += tmp;
		tmp.Format(" ip_index=\"%d\"",info.ip_index);
		xml += tmp;
		tmp.Format(" width_limit=\"%d\"",info.width_limit);
		xml += tmp;
		tmp.Format(" over_length_flag=\"%d\"",info.over_length_flag);
		xml += tmp; 

		xml += ">";

		for(int idx = 0; idx < info.train_id_count; idx++)
		{
			xml += "<train_id_info";

			tmp.Format(" train_id=\"%s\"",info.train_id[idx]);
			xml += tmp;
			tmp.Format(" original_train_id=\"%s\"",info.original_train_id[idx]);
			xml += tmp;
			tmp.Format(" begin_sta=\"%d\"",info.begin_sta[idx]);
			xml += tmp;
			tmp.Format(" end_sta=\"%d\"",info.end_sta[idx]);
			xml += tmp;

			xml += "/>";
		}

		for(int idx = 0; idx < info.engine_count; idx++)
		{
			xml += "<engine_info";

			tmp.Format(" engine_id=\"%s\"",info.engine_id[idx]);
			xml += tmp;
			tmp.Format(" engine_type=\"%d\"",info.engine_type[idx]);
			xml += tmp;
			tmp.Format(" attach_station=\"%d\"",info.attach_station[idx]);
			xml += tmp;
			tmp.Format(" unattach_station=\"%d\"",info.unattach_station[idx]);
			xml += tmp;

			xml += "/>";
		}

		for(int idx = 0; idx < info.station_text_count; idx++)
		{
			xml += "<station_text_info";

			tmp.Format(" station_text_id=\"%d\"",info.station_text_id[idx]);
			xml += tmp;
			tmp.Format(" station_text=\"%s\"",info.station_text[idx]);
			xml += tmp;

			xml += "/>";
		}

		for(int idx = 0; idx < info.abs_text_count; idx++)
		{
			xml += "<abs_text_info";

			tmp.Format(" abs_text_id=\"%d\"",info.abs_text_id[idx]);
			xml += tmp;
			tmp.Format(" abs_text=\"%s\"",info.abs_text[idx]);
			xml += tmp;

			xml += "/>";
		}

		for(int idx = 0; idx < info.load_text_count; idx++)
		{
			xml += "<load_text_info";

			tmp.Format(" load_text_id=\"%d\"",info.load_text_id[idx]);
			xml += tmp;
			tmp.Format(" load_text=\"%s\"",info.load_text[idx]);
			xml += tmp;

			xml += "/>";
		}		
	}
	xml += "</MORE_TRAIN_INFO>";

	return xml;
}

bool ifTRAININFO::fromXML(MSXML::IXMLDOMNodePtr pRootNode, int& result)
{
	MSXML::IXMLDOMNodePtr pNode;

	_variant_t v;
	memset(&info, 0, sizeof(TRAIN_INFO));

	pNode = pRootNode->selectSingleNode( "@sborder_plan_date" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		info.sborder_plan_date = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	pNode = pRootNode->selectSingleNode( "@sborder_plan_time" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		info.sborder_plan_time = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	pNode = pRootNode->selectSingleNode( "@sborder_actual_date" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		info.sborder_actual_date = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	pNode = pRootNode->selectSingleNode( "@sborder_actual_time" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		info.sborder_actual_time = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	pNode = pRootNode->selectSingleNode( "@eborder_plan_date" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		info.eborder_plan_date = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	pNode = pRootNode->selectSingleNode( "@eborder_plan_time" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		info.eborder_plan_time = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	pNode = pRootNode->selectSingleNode( "@eborder_actual_date" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		info.eborder_actual_date = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	pNode = pRootNode->selectSingleNode( "@eborder_actual_time" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		info.eborder_actual_time = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}

	pNode = pRootNode->selectSingleNode( "@sborder_sta" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		info.sborder_sta = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	pNode = pRootNode->selectSingleNode( "@eborder_sta" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		info.eborder_sta = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	pNode = pRootNode->selectSingleNode( "@start_sta_name" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		CString staName = CString(v.bstrVal);
		if(staName.GetLength() >= sizeof(info.start_sta_name))
		{
			result = RETURN_VAL::STA_TEXT_TOO_LONG;
			return false;
		}
		lstrcpyn((char*)info.start_sta_name, staName, sizeof(info.start_sta_name));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	pNode = pRootNode->selectSingleNode( "@end_sta_name" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		CString staName = CString(v.bstrVal);
		if(staName.GetLength() >= sizeof(info.end_sta_name))
		{
			result = RETURN_VAL::STA_NAME_TOO_LONG;
			return false;
		}
		lstrcpyn((char*)info.end_sta_name, staName, sizeof(info.end_sta_name));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	pNode = pRootNode->selectSingleNode( "@basic_train_index" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		info.basic_train_index = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	pNode = pRootNode->selectSingleNode( "@direction" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		info.direction = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	pNode = pRootNode->selectSingleNode( "@type" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		info.type = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	pNode = pRootNode->selectSingleNode( "@run_type" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		info.run_type = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	pNode = pRootNode->selectSingleNode( "@length" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		info.length = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	pNode = pRootNode->selectSingleNode( "@train_weight" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		info.train_weight = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	pNode = pRootNode->selectSingleNode( "@priority" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		info.priority = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	pNode = pRootNode->selectSingleNode( "@engine_utility" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		info.engine_utility = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	pNode = pRootNode->selectSingleNode( "@side_utility" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		info.side_utility = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	pNode = pRootNode->selectSingleNode( "@space_rank" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		info.space_rank = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	pNode = pRootNode->selectSingleNode( "@length_rank" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		info.length_rank = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	pNode = pRootNode->selectSingleNode( "@super_rank" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		info.super_rank = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	pNode = pRootNode->selectSingleNode( "@military_rank" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		info.military_rank = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	pNode = pRootNode->selectSingleNode( "@speed_limit" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		info.speed_limit = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	pNode = pRootNode->selectSingleNode( "@no_meet_flag" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		info.no_meet_flag = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	pNode = pRootNode->selectSingleNode( "@super_interval_ltime" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		info.super_interval_ltime = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	pNode = pRootNode->selectSingleNode( "@status" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		info.status = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	pNode = pRootNode->selectSingleNode( "@adjust_begin_sta" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		info.adjust_begin_sta = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	pNode = pRootNode->selectSingleNode( "@adjust_end_sta" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		info.adjust_end_sta = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	pNode = pRootNode->selectSingleNode( "@adjust_flag" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		info.adjust_flag = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	pNode = pRootNode->selectSingleNode( "@train_source" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		info.train_source = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	/*pNode = pRootNode->selectSingleNode( "@spare" );
	if(pNode)
	{
		CString spare = CString(v.bstrVal);
		if(spare.GetLength() > sizeof(info.spare))
		{
			result = RETURN_VAL::SPARE_TOO_LONG;
			return false;
		}
		lstrcpyn((char*)info.spare, spare, sizeof(info.spare));
		SysFreeString(v.bstrVal);
		v.Clear();
	}*/
	pNode = pRootNode->selectSingleNode( "@ip_index" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		info.ip_index = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	pNode = pRootNode->selectSingleNode( "@width_limit" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		info.width_limit = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	pNode = pRootNode->selectSingleNode( "@over_length_flag" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		info.over_length_flag = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}


	pNode = pRootNode->selectSingleNode( "@sborder_plan_date" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		info.sborder_plan_date = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	pNode = pRootNode->selectSingleNode( "@sborder_plan_date" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		info.sborder_plan_date = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	pNode = pRootNode->selectSingleNode( "@sborder_plan_date" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		info.sborder_plan_date = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	pNode = pRootNode->selectSingleNode( "@sborder_plan_date" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		info.sborder_plan_date = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	pNode = pRootNode->selectSingleNode( "@sborder_plan_date" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		info.sborder_plan_date = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	pNode = pRootNode->selectSingleNode( "@sborder_plan_date" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		info.sborder_plan_date = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	pNode = pRootNode->selectSingleNode( "@sborder_plan_date" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		info.sborder_plan_date = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	pNode = pRootNode->selectSingleNode( "@sborder_plan_date" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		info.sborder_plan_date = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	pNode = pRootNode->selectSingleNode( "@sborder_plan_date" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		info.sborder_plan_date = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	pNode = pRootNode->selectSingleNode( "@sborder_plan_date" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		info.sborder_plan_date = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	pNode = pRootNode->selectSingleNode( "@sborder_plan_date" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		info.sborder_plan_date = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	pNode = pRootNode->selectSingleNode( "@sborder_plan_date" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		info.sborder_plan_date = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	pNode = pRootNode->selectSingleNode( "@sborder_plan_date" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		info.sborder_plan_date = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	pNode = pRootNode->selectSingleNode( "@sborder_plan_date" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		info.sborder_plan_date = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	pNode = pRootNode->selectSingleNode( "@sborder_plan_date" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		info.sborder_plan_date = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	pNode = pRootNode->selectSingleNode( "@sborder_plan_date" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		info.sborder_plan_date = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	pNode = pRootNode->selectSingleNode( "@sborder_plan_date" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		info.sborder_plan_date = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	pNode = pRootNode->selectSingleNode( "@sborder_plan_date" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		info.sborder_plan_date = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	pNode = pRootNode->selectSingleNode( "@sborder_plan_date" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		info.sborder_plan_date = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}

	// train_id_info
	info.train_id_count = 0;
	MSXML::IXMLDOMNodeListPtr pTrain_id_info_List = pRootNode->selectNodes( "train_id_info");
	if(NULL != pTrain_id_info_List)
	{
		info.train_id_count = pTrain_id_info_List->Getlength();
		if(info.train_id_count > MAX_TRAIN_ID_NUM)
		{
			result = RETURN_VAL::TRAIN_ID_TOO_MORE;
			return false;
		}
		for(int idx = 0; idx < info.train_id_count; idx++)
		{
			CString train_id, original_train_id;
			pNode = pTrain_id_info_List->Getitem(idx)->selectSingleNode( "@train_id" );
			if(pNode)
			{
				v = pNode->GetnodeTypedValue();
				train_id = CString(v.bstrVal);
				SysFreeString(v.bstrVal);
				v.Clear();
			}
			pNode = pTrain_id_info_List->Getitem(idx)->selectSingleNode( "@original_train_id" );
			if(pNode)
			{
				v = pNode->GetnodeTypedValue();
				original_train_id = CString(v.bstrVal);
				SysFreeString(v.bstrVal);
				v.Clear();
			}
			if(train_id.GetLength() >= sizeof(info.train_id[0]) || original_train_id.GetLength() >= sizeof(info.original_train_id[0]))
			{
				result = RETURN_VAL::TRAIN_ID_TOO_LONG;
				return false;
			}
			lstrcpyn(info.train_id[idx], train_id, sizeof(info.train_id[0]));
			lstrcpyn(info.original_train_id[idx], original_train_id, sizeof(info.original_train_id[0]));

			pNode = pTrain_id_info_List->Getitem(idx)->selectSingleNode( "@begin_sta" );
			if(pNode)
			{
				v = pNode->GetnodeTypedValue();
				info.begin_sta[idx] = atoi(CString(v.bstrVal));
				SysFreeString(v.bstrVal);
				v.Clear();
			}
			pNode = pTrain_id_info_List->Getitem(idx)->selectSingleNode( "@end_sta" );
			if(pNode)
			{
				v = pNode->GetnodeTypedValue();
				info.end_sta[idx] = atoi(CString(v.bstrVal));
				SysFreeString(v.bstrVal);
				v.Clear();
			}
		}
	}

	// engine_id_info
	info.engine_count = 0;
	MSXML::IXMLDOMNodeListPtr pEngine_id_info_List = pRootNode->selectNodes( "engine_info");
	if(NULL != pEngine_id_info_List)
	{
		info.engine_count = pEngine_id_info_List->Getlength();
		if(info.engine_count > MAX_ENGINEER_NUM)
		{
			result = RETURN_VAL::ENGINE_ID_TOO_MORE;
			return false;
		}
		for(int idx = 0; idx < info.engine_count; idx++)
		{
			CString engine_id;
			pNode = pEngine_id_info_List->Getitem(idx)->selectSingleNode( "@engine_id" );
			if(pNode)
			{
				v = pNode->GetnodeTypedValue();
				engine_id = CString(v.bstrVal);
				SysFreeString(v.bstrVal);
				v.Clear();
			}
			if(engine_id.GetLength() >= sizeof(info.engine_id[0]))
			{
				result = RETURN_VAL::ENGINE_ID_TOO_LONG;
				return false;
			}
			lstrcpyn(info.engine_id[idx], engine_id, sizeof(info.engine_id[0]));

			pNode = pEngine_id_info_List->Getitem(idx)->selectSingleNode( "@engine_type" );
			if(pNode)
			{
				v = pNode->GetnodeTypedValue();
				info.engine_type[idx] = atoi(CString(v.bstrVal));
				SysFreeString(v.bstrVal);
				v.Clear();
			}

			pNode = pEngine_id_info_List->Getitem(idx)->selectSingleNode( "@attach_station" );
			if(pNode)
			{
				v = pNode->GetnodeTypedValue();
				info.attach_station[idx] = atoi(CString(v.bstrVal));
				SysFreeString(v.bstrVal);
				v.Clear();
			}

			pNode = pEngine_id_info_List->Getitem(idx)->selectSingleNode( "@unattach_station" );
			if(pNode)
			{
				v = pNode->GetnodeTypedValue();
				info.unattach_station[idx] = atoi(CString(v.bstrVal));
				SysFreeString(v.bstrVal);
				v.Clear();
			}
		}
	}


	// abs_text_info
	info.station_text_count = 0;
	MSXML::IXMLDOMNodeListPtr pStation_text_info_List = pRootNode->selectNodes( "station_text_info");
	if(NULL != pStation_text_info_List)
	{
		info.station_text_count = pStation_text_info_List->Getlength();
		if(info.station_text_count > MAX_STATION_TEXT_NUM)
		{
			result = RETURN_VAL::STA_TEXT_TOO_MORE;
			return false;
		}
		for(int idx = 0; idx < info.station_text_count; idx++)
		{
			CString abs_text;
			pNode = pStation_text_info_List->Getitem(idx)->selectSingleNode( "@station_text_id" );
			if(pNode)
			{
				v = pNode->GetnodeTypedValue();
				abs_text = CString(v.bstrVal);
				SysFreeString(v.bstrVal);
				v.Clear();
			}
			if(abs_text.GetLength() >= sizeof(info.abs_text[0]))
			{
				result = RETURN_VAL::ABS_TEXT_TOO_LONG;
				return false;
			}
			lstrcpyn(info.station_text[idx], abs_text, sizeof(info.station_text[0]));

			pNode = pStation_text_info_List->Getitem(idx)->selectSingleNode( "@station_text_id" );
			if(pNode)
			{
				v = pNode->GetnodeTypedValue();
				info.station_text_id[idx] = atoi(CString(v.bstrVal));
				SysFreeString(v.bstrVal);
				v.Clear();
			}
		}
	}


	// abs_text_info
	info.abs_text_count = 0;
	MSXML::IXMLDOMNodeListPtr pAbs_text_id_info_List = pRootNode->selectNodes( "abs_text_info");
	if(NULL != pAbs_text_id_info_List)
	{
		info.abs_text_count = pAbs_text_id_info_List->Getlength();
		if(info.station_text_count > MAX_ABS_TEXT_NUM)
		{
			result = RETURN_VAL::STA_TEXT_TOO_MORE;
			return false;
		}
		for(int idx = 0; idx < info.abs_text_count; idx++)
		{
			CString abs_text;
			pNode = pAbs_text_id_info_List->Getitem(idx)->selectSingleNode( "@abs_text" );
			if(pNode)
			{
				v = pNode->GetnodeTypedValue();
				abs_text = CString(v.bstrVal);
				SysFreeString(v.bstrVal);
				v.Clear();
			}
			if(abs_text.GetLength() >= sizeof(info.abs_text[0]))
			{
				result = RETURN_VAL::ABS_TEXT_TOO_LONG;
				return false;
			}
			lstrcpyn(info.abs_text[idx], abs_text, sizeof(info.abs_text[0]));

			pNode = pAbs_text_id_info_List->Getitem(idx)->selectSingleNode( "@abs_text_id" );
			if(pNode)
			{
				v = pNode->GetnodeTypedValue();
				info.abs_text_id[idx] = atoi(CString(v.bstrVal));
				SysFreeString(v.bstrVal);
				v.Clear();
			}
		}
	}


	// load_text_info
	info.load_text_count = 0;
	MSXML::IXMLDOMNodeListPtr pLoad_text_id_info_List = pRootNode->selectNodes( "load_text_info");
	if(NULL != pLoad_text_id_info_List)
	{
		info.load_text_count = pLoad_text_id_info_List->Getlength();
		if(info.load_text_count > MAX_LOAD_NUM)
		{
			result = RETURN_VAL::LOAD_TEXT_TOO_MORE;
			return false;
		}
		for(int idx = 0; idx < info.load_text_count; idx++)
		{
			CString load_text;
			pNode = pLoad_text_id_info_List->Getitem(idx)->selectSingleNode( "@load_text" );
			if(pNode)
			{
				v = pNode->GetnodeTypedValue();
				load_text = CString(v.bstrVal);
				SysFreeString(v.bstrVal);
				v.Clear();
			}
			if(load_text.GetLength() >= sizeof(info.load_text[0]))
			{
				result = RETURN_VAL::LOAD_TEXT_TOO_LONG;
				return false;
			}
			lstrcpyn(info.load_text[idx], load_text, sizeof(info.load_text[0]));

			pNode = pLoad_text_id_info_List->Getitem(idx)->selectSingleNode( "@load_text_id" );
			if(pNode)
			{
				v = pNode->GetnodeTypedValue();
				info.load_text_id[idx] = atoi(CString(v.bstrVal));
				SysFreeString(v.bstrVal);
				v.Clear();
			}
		}
	}

	return true;
}

CString ifMARKITEM::toXMLString()
{
	CString xml,tmp;
	xml = "<mark";
	{
		tmp.Format(" start_date=\"%d\"",item.start_date);
		xml += tmp;
		tmp.Format(" start_time=\"%d\"",item.start_time);
		xml += tmp;

		tmp.Format(" end_date=\"%d\"",item.end_date);
		xml += tmp;
		tmp.Format(" end_time=\"%d\"",item.end_time);
		xml += tmp;

		tmp.Format(" type=\"%d\"",item.type);
		xml += tmp;
		tmp.Format(" flag=\"%d\"",item.flag);
		xml += tmp;
		tmp.Format(" line=\"%d\"",item.line);
		xml += tmp;

		tmp.Format(" station=\"%d\"",item.station);
		xml += tmp;
		tmp.Format(" index=\"%d\"",item.index);
		xml += tmp;

		tmp.Format(" position=\"%d\"",item.position);
		xml += tmp;

		tmp.Format(" text=\"%s\"",item.text);
		xml += tmp;
		tmp.Format(" s_meter=\"%d\"",item.s_meter);
		xml += tmp;
		tmp.Format(" e_meter=\"%d\"",item.e_meter);
		xml += tmp;
		tmp.Format(" e_kilo=\"%d\"",item.e_kilo);
		xml += tmp;
		tmp.Format(" slow_time=\"%d\"",item.slow_time);
		xml += tmp;

		xml += ">";

		for(int j= 0; j<item.count && (j < 128); j++)
		{
			tmp.Format("<abs ele_number=\"%d\" />",item.ele_number[j]);
			xml += tmp;
		}
	}
	xml += "</mark>";

	return xml;
}
bool ifMARKITEM::fromXML(MSXML::IXMLDOMNodePtr pRootNode, long nMarkIndex, int& result)
{
	MSXML::IXMLDOMNodePtr pNode;

	_variant_t v;
	item.mark_index = nMarkIndex;

	// <start_date>
	pNode = pRootNode->selectSingleNode( "@start_date" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		item.start_date = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	// <start_time>
	pNode = pRootNode->selectSingleNode( "@start_time" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		item.start_time = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	// <end_date>
	pNode = pRootNode->selectSingleNode( "@end_date" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		item.end_date = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	// <end_time>
	pNode = pRootNode->selectSingleNode( "@end_time" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		item.end_time = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	// <type>
	pNode = pRootNode->selectSingleNode( "@type" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		item.type = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	// <flag>
	pNode = pRootNode->selectSingleNode( "@flag" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		item.flag = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	// <line>
	pNode = pRootNode->selectSingleNode( "@line" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		item.line = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	// <station>
	pNode = pRootNode->selectSingleNode( "@station" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		item.station = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	// <index>
	pNode = pRootNode->selectSingleNode( "@index" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		item.index = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}

	// <position>
	pNode = pRootNode->selectSingleNode( "@position" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		item.position = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}

	// <text>
	pNode = pRootNode->selectSingleNode( "@text" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		CString text = CString(v.bstrVal);
		int length = text.GetLength();
		if(length >= sizeof(item.text))
		{
			result = RETURN_VAL::TEXT_TOO_LONG;
			return false;
		}
		else
		{
			strncpy(item.text, text, length);
			item.text[length] = 0;
		}
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	// <s_meter>
	pNode = pRootNode->selectSingleNode( "@s_meter" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		item.s_meter = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	// <e_meter>
	pNode = pRootNode->selectSingleNode( "@e_meter" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		item.e_meter = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	// <e_kilo>
	pNode = pRootNode->selectSingleNode( "@e_kilo" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		item.e_kilo = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	// <slow_time>
	pNode = pRootNode->selectSingleNode( "@slow_time" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		item.slow_time = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	// <abs>
	MSXML::IXMLDOMNodeListPtr pAbsList = pRootNode->selectNodes( "abs" );
	int abs_no_count = pAbsList->Getlength();
	for(int idx = 0; idx < abs_no_count; idx++)
	{
		pNode = pAbsList->Getitem(idx)->selectSingleNode( "@ele_number" );
		if(pNode)
		{
			v = pNode->GetnodeTypedValue();
			item.ele_number[idx] = atoi(CString(v.bstrVal));
			SysFreeString(v.bstrVal);
			v.Clear();
		}
	}
	item.count = abs_no_count;
}

CString ifADDONETRAINBYROUTE::toXMLString()
{
	CString xml,tmp;
	xml = "<ADD_TRAIN_BY_ROUTE";

	tmp.Format(" complex_status=\"%d\"", data.complex_status);
	xml += tmp;

	tmp.Format(" date=\"%d\"", data.date);
	xml += tmp;

	tmp.Format(" time=\"%d\"", data.time);
	xml += tmp;

	tmp.Format(" start_mode=\"%d\"", data.start_mode);
	xml += tmp;

	tmp.Format(" end_mode=\"%d\"", data.end_mode);
	xml += tmp;

	xml += "/>";

	return xml;
}

bool ifADDONETRAINBYROUTE::fromXML(MSXML::IXMLDOMNodePtr pRootNode)
{
	MSXML::IXMLDOMNodePtr pNode;

	_variant_t v;
	memset(&data, 0, sizeof(ADDONETRAINBYROUTE));

	pNode = pRootNode->selectSingleNode( "@complex_status" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		data.complex_status = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}

	pNode = pRootNode->selectSingleNode( "@date" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		data.date = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}

	pNode = pRootNode->selectSingleNode( "@time" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		data.time = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}

	pNode = pRootNode->selectSingleNode( "@start_mode" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		data.start_mode = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}

	pNode = pRootNode->selectSingleNode( "@end_mode" );
	if(pNode)
	{
		v = pNode->GetnodeTypedValue();
		data.end_mode = atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}

	return true;
}