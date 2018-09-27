#include "StdAfx.h"
#include "sysshare.h"
#include ".\stationpredict.h"
#import  <msxml.dll> named_guids
using namespace MSXML;

extern bool CreateRecursiveDir(const char* filename);
CString GetRecordXML(TrainPredict& predict);
void GetPredictFromXML(TrainPredict& predict, MSXML::IXMLDOMNodePtr pRootNode);

CStationPredict::CStationPredict(STATION_NO nStationNo, STATION_NO nNeibStation)
{
	mLircCtrlMode=SMODE_EMERGENCY;
	
	mStationNo=nStationNo;
	for(UINT i=0; i<MAX_STPLAN_NUM; i++)
	{
		m_stplan[i].InitZero();
	}

	for(int i=0; i<12; i++)
		n_neibstation[i]=0;

	n_neibstation[0]=nNeibStation;
}

CStationPredict::~CStationPredict(void)
{
}

void CStationPredict::AddNeibStation(STATION_NO neibStation)
{
	for(int i=0; i<12; i++)
	{
		if(n_neibstation[i]==neibStation)
		{
			break;
		}
		if(n_neibstation[i]==0)
		{
			n_neibstation[i]=neibStation;
			break;
		}
	}
}

bool CStationPredict::IsMyNeibStation(STATION_NO neibStation)
{
	for(int i=0; i<12; i++)
	{
		if(n_neibstation[i]==0)
		{
			break;
		}

		if(n_neibstation[i]==neibStation && neibStation>0)
		{
			return true;
		}
	}

	return false;
}

int  CStationPredict::GetNeibStation(STATION_NO list[32])
{
	int count=0;
	for(int i=0; i<12; i++)
	{
		if(n_neibstation[i]==0)
		{
			break;
		}

		list[count]=n_neibstation[i];
		count++;
	}

	return count;
}

int CStationPredict::UpdatePredictPlan(TrainPredict  &predict)
{
	if(!IsMyNeibStation(predict.pre_station_no) && !IsMyNeibStation(predict.next_station_no))
		return 0;

	for(UINT i=0; i<MAX_STPLAN_NUM; i++)
	{
		if(m_stplan[i].train_index==predict.train_index && m_stplan[i].rec_index==predict.rec_index)
		{
			m_stplan[i].station_no = predict.station_no;
			m_stplan[i].sort = predict.sort;
			m_stplan[i].start = predict.start;
			m_stplan[i].passenger = predict.passenger;
			
			m_stplan[i].arrive_real = predict.arrive_real;
			m_stplan[i].depart_real = predict.depart_real;

			m_stplan[i].entry = predict.entry;
			m_stplan[i].exit = predict.exit;
			m_stplan[i].next_entry = predict.next_entry;

			m_stplan[i].pre_station_no = predict.pre_station_no;
			m_stplan[i].plan_arrive_time = predict.plan_arrive_time;
			m_stplan[i].arrive_time = predict.arrive_time;

			m_stplan[i].next_station_no = predict.next_station_no;
			m_stplan[i].plan_depart_time = predict.plan_depart_time;
			m_stplan[i].depart_time = predict.depart_time;

			strncpy(m_stplan[i].arrive_train_id, predict.arrive_train_id, 9);
			strncpy(m_stplan[i].depart_train_id, predict.depart_train_id, 9);

			m_stplan[i].arrive_real = predict.arrive_real;
			m_stplan[i].depart_real = predict.depart_real;
			m_stplan[i].finish_predict = 0;
			return 1;
		}
	}

	for(UINT i=0; i<MAX_STPLAN_NUM; i++)
	{
		if(m_stplan[i].train_index==0 && m_stplan[i].station_no==0)
		{
			m_stplan[i]=predict;
			return 2;
		}
		if(m_stplan[i].train_index==0 && m_stplan[i].station_no==65535)
		{
			m_stplan[i]=predict;
			return 2;
		}
	}

	return 3;
}

bool CStationPredict::DeletePredict(TRAIN_INDEX train_index, short rec_index, TrainPredict& predict)
{
	for(UINT i=0; i<MAX_STPLAN_NUM; i++)
	{
		if(m_stplan[i].train_index==train_index && m_stplan[i].rec_index==rec_index)
		{
			predict=m_stplan[i];
			// train_index=0 station_no=65535 表示是释放点可以使用
			m_stplan[i].InitZero();
			m_stplan[i].train_index=0;
			m_stplan[i].station_no=65535; 
			break;
		}

		// train_index=0 station_no=0 表示后续未使用
		if(m_stplan[i].train_index==0 && m_stplan[i].station_no==0)
		{
			break;
		}
	}

	return true;
}

// 手工编写计划处理,不能按照索引，只能按照车次
bool CStationPredict::AddPredictPlan(TrainPredict  &predict)
{
	for(UINT i=0; i<MAX_STPLAN_NUM; i++)
	{
		if(m_stplan[i].train_index==0 && m_stplan[i].station_no==0)
		{
			m_stplan[i]=predict;
			break;
		}
		if(m_stplan[i].train_index==0 && m_stplan[i].station_no==65535)
		{
			m_stplan[i]=predict;
			break;
		}
	}

	if(i<MAX_STPLAN_NUM)
		return true;
	return false;
}

bool CStationPredict::DeletePredict(CString arriveid, CString departid, TrainPredict& predict)
{
	for(UINT i=0; i<MAX_STPLAN_NUM; i++)
	{
		if(arriveid==m_stplan[i].arrive_train_id && departid==m_stplan[i].depart_train_id)
		{
			predict=m_stplan[i];
			// train_index=0 station_no=65535 表示是释放点可以使用
			m_stplan[i].InitZero();
			m_stplan[i].train_index=0;
			m_stplan[i].station_no=65535; 
			break;
		}

		// train_index=0 station_no=0 表示后续未使用
		if(m_stplan[i].train_index==0 && m_stplan[i].station_no==0)
		{
			break;
		}
	}
	return true;
}

int CStationPredict::GetStplanByTrainIndex(TRAIN_INDEX train_index, BYTE rec_index, TrainPredict& predict)
{
    int nRet=-1;
	if(train_index==0)
		return nRet;
	for(UINT i=0; i<MAX_STPLAN_NUM; i++)
	{
		if(train_index == m_stplan[i].train_index && rec_index == m_stplan[i].rec_index)
		{
			nRet = i;
			predict=m_stplan[i];
			break;
		}
		
		// train_index=0 station_no=0 表示后续未使用
		if(m_stplan[i].train_index==0 && m_stplan[i].station_no==0)
		{
			break;
		}
	}
	return nRet;
}

void CStationPredict::GetStplanByTrainNoAndPreStation(CString trainid, STATION_NO pre_station, std::vector<int>& list_idx)
{
	list_idx.clear();
	for(UINT i=0; i<MAX_STPLAN_NUM; i++)
	{
		if(trainid == m_stplan[i].arrive_train_id && m_stplan[i].pre_station_no == pre_station)
		{
			list_idx.push_back(i);
		}
		
		// train_index=0 station_no=0 表示后续未使用
		if(m_stplan[i].train_index==0 && m_stplan[i].station_no==0)
		{
			break;
		}
	}
}

void CStationPredict::GetStplanByTrainNoAndNextStation(CString trainid, STATION_NO next_station, std::vector<int>& list_idx)
{
	list_idx.clear();
	for(UINT i=0; i<MAX_STPLAN_NUM; i++)
	{
		if(trainid == m_stplan[i].depart_train_id && m_stplan[i].next_station_no==next_station)
		{
			list_idx.push_back(i);
		}

		// train_index=0 station_no=0 表示后续未使用
		if(m_stplan[i].train_index==0 && m_stplan[i].station_no==0)
		{
			break;
		}
	}
}

///// BEGIN文件中数据状态恢复到当前内存中计划
void CStationPredict::LoadStplanFromFile()
{
	CString fileName;
	fileName.Format("%s\\predict\\%d.xml", gpcdef.GetWorkPathName(), mStationNo);
	
	MSXML::IXMLDOMDocumentPtr pXMLDoc;
	HRESULT hr = pXMLDoc.CreateInstance(MSXML::CLSID_DOMDocument);
	if(FAILED(hr))
	{
		_com_error er(hr);
		AfxMessageBox(er.ErrorMessage());
		return;
	}
	
	if(VARIANT_TRUE != pXMLDoc->load( _bstr_t( fileName ) ) )
	{
		CString  prompt;
		prompt.Format("预告计划 %s不存在或者不符合xml语法(可以用浏览器检查是否能打开)", fileName);
		//AfxMessageBox(prompt);
		return;
	}
	
	MSXML::IXMLDOMNodePtr pChildNode,pNode,pRootNode;

	pRootNode = pXMLDoc->selectSingleNode( "PREDICT_PLAN");
	if(pRootNode == NULL)
		return;

	MSXML::IXMLDOMNodeListPtr pPlan = pRootNode->selectNodes( "PLAN");
	if(NULL == pPlan)
		return;
	
	ULONG nBeginTime=0;
	CTime curtm=CTime::GetCurrentTime();
	int  nHour=curtm.GetHour();
	if(nHour>=8 && nHour<18)
	{
		CTime tm(curtm.GetYear(), curtm.GetMonth(), curtm.GetDay(), 6,0,0);
		nBeginTime=tm.GetTime();
	}
	else if(nHour>=20 && nHour<23)
	{
		CTime tm(curtm.GetYear(), curtm.GetMonth(), curtm.GetDay(), 18,0,0);
		nBeginTime=tm.GetTime();
	}
	else if(0<=nHour && nHour<6)
	{
		CTime tm(curtm.GetYear(), curtm.GetMonth(), curtm.GetDay(), 0,0,0);
		nBeginTime=tm.GetTime()-6*3600;
	}
	else
	{
		nBeginTime=0;
	}

	for(int idx = 0; idx < pPlan->Getlength(); idx++)
	{
		pNode = pPlan->Getitem(idx);
		if(pNode==NULL)
			continue;
		
		TrainPredict predict;
		GetPredictFromXML(predict, pNode);
		if(nBeginTime>0)
		{
			if(predict.start==1)
			{
				if(predict.depart_time<nBeginTime)
					continue;
			}
			else if(predict.start==2)
			{
				if(predict.arrive_time<nBeginTime)
					continue;
			}
			else
			{
				if(predict.depart_time<nBeginTime)
					continue;
			}
		}
		AddPredictPlan(predict);
	}
}

void CStationPredict::SaveStplanToFile()
{
	CString fileName;
	fileName.Format("%s\\predict\\%d.xml", gpcdef.GetWorkPathName(), mStationNo);
	CreateRecursiveDir(fileName);

	CStdioFile file;
	if(!file.Open(fileName, CFile::modeCreate|CFile::modeWrite))
		return;

	file.WriteString("<PREDICT_PLAN>\n");

	for(UINT i=0; i<MAX_STPLAN_NUM; i++)
	{
		if(m_stplan[i].train_index==0 && m_stplan[i].station_no==0)
			break;
		
		if(m_stplan[i].train_index==0 && m_stplan[i].station_no==65535)
			continue;
		
		file.WriteString(GetRecordXML(m_stplan[i]));
	}
	file.Flush();
	
	file.WriteString("</PREDICT_PLAN>\n");
	file.Close();
}

///// END文件中数据状态恢复到当前内存中计划
CString GetRecordXML(TrainPredict& predict)
{
	CString xml, tmp;

	xml.Format("\n<PLAN trainindex=\"%d\" station=\"%d\" recindex=\"%d\" passenger=\"%d\" sort=\"%d\" start=\"%d\" entry=\"%d\" exit=\"%d\" next_entry=\"%d\">",
				predict.train_index,predict.station_no,predict.rec_index,predict.passenger,predict.sort,predict.start,
				predict.entry,predict.exit,predict.next_entry);

	
	// 到达信息
	xml += "\n<ARRIVE>";
		tmp.Format("<arrive_real>%d</arrive_real>", predict.arrive_real);
		xml += tmp;

		tmp.Format("<trainid>%s</trainid>", predict.arrive_train_id);
		xml += tmp;

		tmp.Format("<station>%d</station>", predict.pre_station_no);
		xml += tmp;

		tmp.Format("<plan_arrive_time>%d</plan_arrive_time>", predict.plan_arrive_time);
		xml += tmp;

		tmp.Format("<arrive_time>%d</arrive_time>", predict.arrive_time);
		xml += tmp;

		tmp.Format("<neib_yugao_time>%d</neib_yugao_time>", predict.neib_yugao_time);
		xml += tmp;

		tmp.Format("<tongyi_neib_time>%d</tongyi_neib_time>", predict.tongyi_neib_time);
		xml += tmp;

		tmp.Format("<neib_cancel_time>%d</neib_cancel_time>", predict.neib_cancel_time);
		xml += tmp;
	xml += "</ARRIVE>";

	// 出发信息
	xml += "\n<DEPART>";
		tmp.Format("<depart_real>%d</depart_real>", predict.depart_real);
		xml += tmp;

	    tmp.Format("<trainid>%s</trainid>", predict.depart_train_id);
		xml += tmp;

		tmp.Format("<station>%d</station>", predict.next_station_no);
		xml += tmp;

		tmp.Format("<plan_depart_time>%d</plan_depart_time>", predict.plan_depart_time);
		xml += tmp;

		tmp.Format("<depart_time>%d</depart_time>", predict.depart_time);
		xml += tmp;

		tmp.Format("<yugao_neib_time>%d</yugao_neib_time>", predict.yugao_neib_time);
		xml += tmp;

		tmp.Format("<neib_tongyi_time>%d</neib_tongyi_time>", predict.neib_tongyi_time);
		xml += tmp;

		tmp.Format("<cancel_neib_time>%d</cancel_neib_time>", predict.cancel_neib_time);
		xml += tmp;
	xml += "</DEPART>";

	xml += "</PLAN>";

	return xml;
}

void GetPredictFromXML(TrainPredict& predict, MSXML::IXMLDOMNodePtr pRootNode)
{
	predict.InitZero();

	MSXML::IXMLDOMNodePtr pChildNode,pNode; 

	pChildNode = pRootNode->selectSingleNode("@trainindex");
	if(pChildNode)
	{
		_variant_t v;
		v = pChildNode->GetnodeTypedValue();
		predict.train_index = (DWORD)atol(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	pChildNode = pRootNode->selectSingleNode("@station");
	if(pChildNode)
	{
		_variant_t v;
		v = pChildNode->GetnodeTypedValue();
		predict.station_no = (WORD)atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	pChildNode = pRootNode->selectSingleNode("@recindex");
	if(pChildNode)
	{
		_variant_t v;
		v = pChildNode->GetnodeTypedValue();
		predict.rec_index = (BYTE)atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	pChildNode = pRootNode->selectSingleNode("@passenger");
	if(pChildNode)
	{
		_variant_t v;
		v = pChildNode->GetnodeTypedValue();
		predict.passenger = (BYTE)atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	pChildNode = pRootNode->selectSingleNode("@sort");
	if(pChildNode)
	{
		_variant_t v;
		v = pChildNode->GetnodeTypedValue();
		predict.sort = (DWORD)atol(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	pChildNode = pRootNode->selectSingleNode("@start");
	if(pChildNode)
	{
		_variant_t v;
		v = pChildNode->GetnodeTypedValue();
		predict.start = (BYTE)atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	
	pChildNode = pRootNode->selectSingleNode("@entry");
	if(pChildNode)
	{
		_variant_t v;
		v = pChildNode->GetnodeTypedValue();
		predict.entry = (BYTE)atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	pChildNode = pRootNode->selectSingleNode("@exit");
	if(pChildNode)
	{
		_variant_t v;
		v = pChildNode->GetnodeTypedValue();
		predict.exit = (BYTE)atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	pChildNode = pRootNode->selectSingleNode("@next_entry");
	if(pChildNode)
	{
		_variant_t v;
		v = pChildNode->GetnodeTypedValue();
		predict.next_entry = (BYTE)atoi(CString(v.bstrVal));
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	
	
	// ARRIVE信息
	pNode = pRootNode->selectSingleNode("ARRIVE");
    if(pNode)
	{
		pChildNode = pNode->selectSingleNode("arrive_real");
		if(pChildNode)
		{
			_variant_t v;
			v = pChildNode->GetnodeTypedValue();
			predict.arrive_real = (BYTE)atoi(CString(v.bstrVal));
			SysFreeString(v.bstrVal);
			v.Clear();
		}
		
		pChildNode = pNode->selectSingleNode("trainid");
		if(pChildNode)
		{
			_variant_t v;
			v = pChildNode->GetnodeTypedValue();
			CString tmp = v.bstrVal;
			strncpy(predict.arrive_train_id, tmp, sizeof(TRAIN_NO)-1);
			predict.arrive_train_id[sizeof(TRAIN_NO)-1]=0;
			SysFreeString(v.bstrVal);
			v.Clear();
		}
		
		pChildNode = pNode->selectSingleNode("station");
		if(pChildNode)
		{
			_variant_t v;
			v = pChildNode->GetnodeTypedValue();
			predict.pre_station_no = (WORD)atoi(CString(v.bstrVal));
			SysFreeString(v.bstrVal);
			v.Clear();
		}
		pChildNode = pNode->selectSingleNode("plan_arrive_time");
		if(pChildNode)
		{
			_variant_t v;
			v = pChildNode->GetnodeTypedValue();
			predict.plan_arrive_time = atol(CString(v.bstrVal));
			SysFreeString(v.bstrVal);
			v.Clear();
		}
		pChildNode = pNode->selectSingleNode("arrive_time");
		if(pChildNode)
		{
			_variant_t v;
			v = pChildNode->GetnodeTypedValue();
			predict.arrive_time = atol(CString(v.bstrVal));
			SysFreeString(v.bstrVal);
			v.Clear();
		}
		pChildNode = pNode->selectSingleNode("neib_yugao_time");
		if(pChildNode)
		{
			_variant_t v;
			v = pChildNode->GetnodeTypedValue();
			predict.neib_yugao_time = atol(CString(v.bstrVal));
			SysFreeString(v.bstrVal);
			v.Clear();
		}
		pChildNode = pNode->selectSingleNode("tongyi_neib_time");
		if(pChildNode)
		{
			_variant_t v;
			v = pChildNode->GetnodeTypedValue();
			predict.tongyi_neib_time = atol(CString(v.bstrVal));
			SysFreeString(v.bstrVal);
			v.Clear();
		}
		pChildNode = pNode->selectSingleNode("neib_cancel_time");
		if(pChildNode)
		{
			_variant_t v;
			v = pChildNode->GetnodeTypedValue();
			predict.neib_cancel_time = atol(CString(v.bstrVal));
			SysFreeString(v.bstrVal);
			v.Clear();
		}
	}

	// DEPART信息
	pNode = pRootNode->selectSingleNode("DEPART");
    if(pNode)
	{
		pChildNode = pNode->selectSingleNode("depart_real");
		if(pChildNode)
		{
			_variant_t v;
			v = pChildNode->GetnodeTypedValue();
			predict.depart_real = (BYTE)atoi(CString(v.bstrVal));
			SysFreeString(v.bstrVal);
			v.Clear();
		}
		
		pChildNode = pNode->selectSingleNode("trainid");
		if(pChildNode)
		{
			_variant_t v;
			v = pChildNode->GetnodeTypedValue();
			CString tmp = v.bstrVal;
			strncpy(predict.depart_train_id, tmp, sizeof(TRAIN_NO)-1);
			predict.depart_train_id[sizeof(TRAIN_NO)-1]=0;
			SysFreeString(v.bstrVal);
			v.Clear();
		}
		pChildNode = pNode->selectSingleNode("station");
		if(pChildNode)
		{
			_variant_t v;
			v = pChildNode->GetnodeTypedValue();
			predict.next_station_no = (WORD)atoi(CString(v.bstrVal));
			SysFreeString(v.bstrVal);
			v.Clear();
		}
		pChildNode = pNode->selectSingleNode("plan_depart_time");
		if(pChildNode)
		{
			_variant_t v;
			v = pChildNode->GetnodeTypedValue();
			predict.plan_depart_time = atol(CString(v.bstrVal));
			SysFreeString(v.bstrVal);
			v.Clear();
		}
		pChildNode = pNode->selectSingleNode("depart_time");
		if(pChildNode)
		{
			_variant_t v;
			v = pChildNode->GetnodeTypedValue();
			predict.depart_time = atol(CString(v.bstrVal));
			SysFreeString(v.bstrVal);
			v.Clear();
		}
		pChildNode = pNode->selectSingleNode("yugao_neib_time");
		if(pChildNode)
		{
			_variant_t v;
			v = pChildNode->GetnodeTypedValue();
			predict.yugao_neib_time = atol(CString(v.bstrVal));
			SysFreeString(v.bstrVal);
			v.Clear();
		}
		pChildNode = pNode->selectSingleNode("neib_tongyi_time");
		if(pChildNode)
		{
			_variant_t v;
			v = pChildNode->GetnodeTypedValue();
			predict.neib_tongyi_time = atol(CString(v.bstrVal));
			SysFreeString(v.bstrVal);
			v.Clear();
		}
		pChildNode = pNode->selectSingleNode("cancel_neib_time");
		if(pChildNode)
		{
			_variant_t v;
			v = pChildNode->GetnodeTypedValue();
			predict.cancel_neib_time = atol(CString(v.bstrVal));
			SysFreeString(v.bstrVal);
			v.Clear();
		}
	}
}

void CStationPredict::SetFinishPredict()
{
	for(UINT i=0; i<MAX_STPLAN_NUM; i++)
	{
		if(m_stplan[i].train_index==0 && m_stplan[i].station_no==0)
		{
			break;
		}
		if(m_stplan[i].train_index==0 && m_stplan[i].station_no==65535)
		{
			continue;
		}

		if(m_stplan[i].start==1)
		{
			if(m_stplan[i].depart_real==1)
			{
				m_stplan[i].finish_predict=1;
			}
		}
		else if(m_stplan[i].start==2)
		{
			if(m_stplan[i].arrive_real==1)
			{
				m_stplan[i].finish_predict=1;
			}
		}
		else
		{
			if(IsMyNeibStation(m_stplan[i].pre_station_no))
			{
				if(IsMyNeibStation(m_stplan[i].next_station_no))
				{
					if(m_stplan[i].depart_real==1)
					{
						m_stplan[i].finish_predict=1;
					}
				}
				else
				{
					if(m_stplan[i].arrive_real==1)
					{
						m_stplan[i].finish_predict=1;
					}
				}
			}
			else if(IsMyNeibStation(m_stplan[i].next_station_no))
			{
				if(m_stplan[i].depart_real==1)
				{
					m_stplan[i].finish_predict=1;
				}
			}
			else
			{
				m_stplan[i].finish_predict=1;
			}
		}
	}
}
