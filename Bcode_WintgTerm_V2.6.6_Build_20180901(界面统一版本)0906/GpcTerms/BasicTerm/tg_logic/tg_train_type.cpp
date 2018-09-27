#include "stdafx.h"
#include "tg_structdef.h"
#include "tg_train_type.h"


#import  <msxml.dll> named_guids
using namespace MSXML;

RUN_TYPES::RUN_TYPES()
{
	init();
}
RUN_TYPES::RUN_TYPES(const RUN_TYPES& sSource)
{
	this->id = sSource.id;
	this->name = sSource.name;
}

RUN_TYPES& RUN_TYPES::operator=(const RUN_TYPES& sSource)
{
	if (this == &sSource)
	{
		return (*this);		
	}
	this->name.clear();
	this->id = sSource.id;
	this->name = sSource.name;
	return (*this);
}
RUN_TYPES::~RUN_TYPES()
{
	init();
}
void RUN_TYPES:: init()
{
	id = -1;
	name.clear();     
}

TRAIN_TYPES_ID_NAME_RUNTYPE_PRI::TRAIN_TYPES_ID_NAME_RUNTYPE_PRI()
{
	init();
}
TRAIN_TYPES_ID_NAME_RUNTYPE_PRI::TRAIN_TYPES_ID_NAME_RUNTYPE_PRI(const TRAIN_TYPES_ID_NAME_RUNTYPE_PRI& sSource)
{
	this->id = sSource.id;
	this->name = sSource.name;
	this->run_type = sSource.run_type;
	this->priority = sSource.priority;
	this->loco_type  = sSource.loco_type;	
}

TRAIN_TYPES_ID_NAME_RUNTYPE_PRI& TRAIN_TYPES_ID_NAME_RUNTYPE_PRI::operator=(const TRAIN_TYPES_ID_NAME_RUNTYPE_PRI& sSource)
{
	if (this == &sSource)
	{
		return (*this);		
	}
	this->name.clear();
	this->id = sSource.id;
	this->name = sSource.name;
	this->run_type = sSource.run_type;
	this->priority = sSource.priority;
	this->loco_type  = sSource.loco_type;	
	return (*this);
}
TRAIN_TYPES_ID_NAME_RUNTYPE_PRI::~TRAIN_TYPES_ID_NAME_RUNTYPE_PRI()
{
	init();
}
void TRAIN_TYPES_ID_NAME_RUNTYPE_PRI:: init()
{
	id = 255;
	name.clear(); 
	run_type = -1;
	priority = -1;    
	loco_type=0;
}

PRIORITIES::PRIORITIES()
{
	init();
}
PRIORITIES::PRIORITIES(const PRIORITIES& sSource)
{
	this->id = sSource.id;
	this->name = sSource.name;
}

PRIORITIES& PRIORITIES::operator=(const PRIORITIES& sSource)
{
	if (this == &sSource)
	{
		return (*this);		
	}
	this->name.clear();
	this->id = sSource.id;
	this->name = sSource.name;
	return (*this);
}
PRIORITIES::~PRIORITIES()
{
	init();
}
void PRIORITIES:: init()
{
	id = -1;
	name.clear();     
}

TRAIN_TYPES::TRAIN_TYPES()
{
	init();
}
TRAIN_TYPES::TRAIN_TYPES(const TRAIN_TYPES& sSource)
{
	this->prefix = sSource.prefix;
	this->start = sSource.start;
	this->end = sSource.end;
	this->id = sSource.id;
	this->name = sSource.name;
	this->run_type = sSource.run_type;
	this->priority = sSource.priority;	
}

TRAIN_TYPES& TRAIN_TYPES::operator=(const TRAIN_TYPES& sSource)
{
	if (this == &sSource)
	{
		return (*this);		
	}
	this->name.clear();
	this->prefix.clear();	
	this->prefix = sSource.prefix;
	this->start = sSource.start;
	this->end = sSource.end;
	this->id = sSource.id;
	this->name = sSource.name;
	this->run_type = sSource.run_type;
	this->priority = sSource.priority;
	return (*this);
}
TRAIN_TYPES::~TRAIN_TYPES()
{
	init();
}
void TRAIN_TYPES:: init()
{
	id =  (BYTE)-1;
	name.clear();  
	prefix.clear();
	start = 0;
	end = 0;
	run_type = (BYTE)-1;
	priority = (BYTE)-1;
}


void CGetTrain_TypeInfor::LoadTrain_TypeXML(const string pathname)
{
	train_type_Quantity = 0;
	MSXML::IXMLDOMDocumentPtr pXMLDoc;
	HRESULT hr = pXMLDoc.CreateInstance(MSXML::CLSID_DOMDocument);
	if(FAILED(hr))
	{
		_com_error er(hr);
		AfxMessageBox(er.ErrorMessage());
		return;
	}

	if(VARIANT_TRUE != pXMLDoc->load( _bstr_t( pathname.data() ) ) )
	{
		CString  prompt;
		prompt.Format("配置train_type.xml %s不存在或者不符合xml语法(可以用浏览器检查是否能打开)", pathname);
		AfxMessageBox(prompt);
		return; 
	}
	MSXML::IXMLDOMNodePtr pRootNode,pChildNode,pNode_RUN_TYPES,pNode_RUN_TYPE,pNode;
	MSXML::IXMLDOMNodePtr pNode_PRIORITIES,pNode_PRIORITIE;
	MSXML::IXMLDOMNodePtr pNode_TRAIN_TYPES,pNode_TRAIN_TYPE,pNode_ABS_TIME_TYPE;
	MSXML::IXMLDOMNodePtr pNode_TRAIN_ID_RANGE;
	pRootNode = pXMLDoc->selectSingleNode( "TRAIN_TYPE_DEF");
	if(pRootNode == NULL)
	{
		return;
	}
	// RUN_TYPES
	MSXML::IXMLDOMNodeListPtr list_pNode_RUN_TYPES = pRootNode->selectNodes( "RUN_TYPES");
	if (list_pNode_RUN_TYPES == NULL ||list_pNode_RUN_TYPES->Getlength() == 0 )
	{
		return;
	}
	pNode_RUN_TYPES = list_pNode_RUN_TYPES->Getitem(0);
	MSXML::IXMLDOMNodeListPtr list = pNode_RUN_TYPES->selectNodes( "RUN_TYPE");
	if(NULL != list)
	{
		for(int idx = 0; idx < list->Getlength(); idx++)
		{
			pNode_RUN_TYPE = list->Getitem(idx);
			if(pNode_RUN_TYPE)
			{
				RUN_TYPES tmp_RUN_TYPE;
				// @id
				pChildNode = pNode_RUN_TYPE->selectSingleNode("@id");
				if(pChildNode)
				{
					_variant_t v;
					v = pChildNode->GetnodeTypedValue();
					tmp_RUN_TYPE.id = (WORD)atoi(CString(v.bstrVal));	
					SysFreeString(v.bstrVal);
					v.Clear();
				}
				//@name
				pChildNode = pNode_RUN_TYPE->selectSingleNode("@name");
				if(pChildNode)
				{
					_variant_t v;
					v = pChildNode->GetnodeTypedValue();										
					char   *p=_com_util::ConvertBSTRToString(v.bstrVal);   
					tmp_RUN_TYPE.name = p;	
					delete  p;   											
					SysFreeString(v.bstrVal);
					v.Clear();
				}
				
				m_map_RUN_TYPES.insert(make_pair(tmp_RUN_TYPE.id,tmp_RUN_TYPE));
			}
		}	

		// 用户没有配置255，自动添加255，未知列车
		std::map<short, RUN_TYPES>::iterator it = m_map_RUN_TYPES.find(255);
		if(it == m_map_RUN_TYPES.end())
		{
			RUN_TYPES tmp_RUN_TYPE;
			tmp_RUN_TYPE.id = 255;
			tmp_RUN_TYPE.name = "未知列车";
			m_map_RUN_TYPES.insert(make_pair(tmp_RUN_TYPE.id,tmp_RUN_TYPE));
		}
	}
	// ABS_TIME_TYPES
	MSXML::IXMLDOMNodePtr pNode_ABS_TIME_TYPES = pRootNode->selectSingleNode( "ABS_TIME_TYPES");
	if (pNode_ABS_TIME_TYPES)
	{
		MSXML::IXMLDOMNodeListPtr list = pNode_ABS_TIME_TYPES->selectNodes( "ABS_TIME_TYPE");
		for(int idx = 0; idx < list->Getlength(); idx++)
		{
			pNode = list->Getitem(idx);
			if(pNode)
			{
				RUN_TYPES tmp_RUN_TYPE;
				// @id
				pChildNode = pNode->selectSingleNode("@id");
				if(pChildNode)
				{
					_variant_t v;
					v = pChildNode->GetnodeTypedValue();
					tmp_RUN_TYPE.id = (WORD)atoi(CString(v.bstrVal));	
					SysFreeString(v.bstrVal);
					v.Clear();
				}
				//@name
				pChildNode = pNode->selectSingleNode("@name");
				if(pChildNode)
				{
					_variant_t v;
					v = pChildNode->GetnodeTypedValue();										
					char   *p=_com_util::ConvertBSTRToString(v.bstrVal);   
					tmp_RUN_TYPE.name = p;	
					delete  p;   											
					SysFreeString(v.bstrVal);
					v.Clear();
				}
				m_map_ABS_TIME_TYPES.insert(make_pair(tmp_RUN_TYPE.id,tmp_RUN_TYPE));
			}
		}	
	}	
	//PRIORITIES
	MSXML::IXMLDOMNodeListPtr list_pNode_PRIORITIES = pRootNode->selectNodes( "PRIORITIES");
	if (list_pNode_PRIORITIES == NULL ||list_pNode_PRIORITIES->Getlength() == 0 )
	{
		return;
	}
	pNode_PRIORITIES = list_pNode_PRIORITIES->Getitem(0);
	list = pNode_PRIORITIES->selectNodes( "PRIORITY");
	if(NULL != list)
	{
		for(int idx = 0; idx < list->Getlength(); idx++)
		{
			pNode_PRIORITIE = list->Getitem(idx);
			if(pNode_PRIORITIE)
			{
				PRIORITIES tmp_PRIORITIE;
				// @id
				pChildNode = pNode_PRIORITIE->selectSingleNode("@id");
				if(pChildNode)
				{
					_variant_t v;
					v = pChildNode->GetnodeTypedValue();
					tmp_PRIORITIE.id = (WORD)atoi(CString(v.bstrVal));	
					SysFreeString(v.bstrVal);
					v.Clear();
				}
				//@name
				pChildNode = pNode_PRIORITIE->selectSingleNode("@name");
				if(pChildNode)
				{
					_variant_t v;
					v = pChildNode->GetnodeTypedValue();										
					char   *p=_com_util::ConvertBSTRToString(v.bstrVal);   
					tmp_PRIORITIE.name = p;	
					delete  p;   											
					SysFreeString(v.bstrVal);
					v.Clear();
				}
				m_map_PRIORITIES.insert(make_pair(tmp_PRIORITIE.id,tmp_PRIORITIE));	
			}
		}	
	}	
	//TRAIN_TYPES 
	MSXML::IXMLDOMNodeListPtr list_pNode_TRAIN_TYPES = pRootNode->selectNodes( "TRAIN_TYPES");
	if (list_pNode_TRAIN_TYPES == NULL ||list_pNode_TRAIN_TYPES->Getlength() == 0 )
	{
		return;
	}
	pNode_TRAIN_TYPES = list_pNode_TRAIN_TYPES->Getitem(0);
	//读取默认列车属性default_train_type
	pChildNode = pNode_TRAIN_TYPES->selectSingleNode("@default_train_type");
	if(pChildNode)
	{
		_variant_t v;
		v = pChildNode->GetnodeTypedValue();
		m_default_train_type = (BYTE)(WORD)atoi(CString(v.bstrVal));	
		SysFreeString(v.bstrVal);
		v.Clear();
	}
	list = pNode_TRAIN_TYPES->selectNodes( "TRAIN_TYPE");
	if(NULL != list)
	{
		for(int idx = 0; idx < list->Getlength(); idx++)
		{
			BYTE	    tmp_id = -1;
			string       tmp_name;
			BYTE    	tmp_run_type = -1;	
			BYTE	    tmp_priority = -1;
			BYTE        tmp_loco_type=0;
			tmp_name.clear();
			pNode_TRAIN_TYPE = list->Getitem(idx);
			if(pNode_TRAIN_TYPE)
			{
				TRAIN_TYPES  tmp_TRAIN_TYPE;
				TRAIN_TYPES_ID_NAME_RUNTYPE_PRI  tmp_TRAIN_TYPE_DETAILS;
				// @id
				pChildNode = pNode_TRAIN_TYPE->selectSingleNode("@id");
				if(pChildNode)
				{
					_variant_t v;
					v = pChildNode->GetnodeTypedValue();
					tmp_id = (BYTE)(WORD)atoi(CString(v.bstrVal));	
					SysFreeString(v.bstrVal);
					v.Clear();
				}
				//@name
				pChildNode = pNode_TRAIN_TYPE->selectSingleNode("@name");
				if(pChildNode)
				{
					_variant_t v;
					v = pChildNode->GetnodeTypedValue();										
					char   *p=_com_util::ConvertBSTRToString(v.bstrVal);  
					tmp_name.clear(); 
					tmp_name = p;	
					delete  p;   											
					SysFreeString(v.bstrVal);
					v.Clear();
				}
				//@run_type
				pChildNode = pNode_TRAIN_TYPE->selectSingleNode("@run_type");
				if(pChildNode)
				{
					_variant_t v;
					v = pChildNode->GetnodeTypedValue();
					tmp_run_type = (BYTE)(WORD)atoi(CString(v.bstrVal));	
					SysFreeString(v.bstrVal);
					v.Clear();
				}
				//@priority
				pChildNode = pNode_TRAIN_TYPE->selectSingleNode("@priority");
				if(pChildNode)
				{
					_variant_t v;
					v = pChildNode->GetnodeTypedValue();
					tmp_priority = (BYTE)(WORD)atoi(CString(v.bstrVal));	
					SysFreeString(v.bstrVal);
					v.Clear();
				}
				//@loco_type
				pChildNode = pNode_TRAIN_TYPE->selectSingleNode("@loco_type");
				if(pChildNode)
				{
					_variant_t v;
					v = pChildNode->GetnodeTypedValue();
					tmp_loco_type = (BYTE)(WORD)atoi(CString(v.bstrVal));	
					SysFreeString(v.bstrVal);
					v.Clear();
				}
				tmp_TRAIN_TYPE_DETAILS.id = tmp_id;
				tmp_TRAIN_TYPE_DETAILS.name = tmp_name;
				tmp_TRAIN_TYPE_DETAILS.run_type = tmp_run_type;
				tmp_TRAIN_TYPE_DETAILS.priority = tmp_priority;
				tmp_TRAIN_TYPE_DETAILS.loco_type = tmp_loco_type;
				m_map_TRAIN_TYPE_DETAILS.insert(make_pair(tmp_id,tmp_TRAIN_TYPE_DETAILS));
				MSXML::IXMLDOMNodeListPtr list_TRAIN_ID_RANGE = pNode_TRAIN_TYPE->selectNodes( "TRAIN_ID_RANGE");
				if(NULL != list_TRAIN_ID_RANGE)
				{
							for(int idx_trn_id_range = 0; idx_trn_id_range < list_TRAIN_ID_RANGE->Getlength(); idx_trn_id_range++)
							{
									pNode_TRAIN_ID_RANGE = list_TRAIN_ID_RANGE->Getitem(idx_trn_id_range);
									if(pNode_TRAIN_ID_RANGE)
									{
												// @prefix
												pChildNode = pNode_TRAIN_ID_RANGE->selectSingleNode("@prefix");
												if(pChildNode)
												{
														_variant_t v;
														v = pChildNode->GetnodeTypedValue();
														char   *p1=_com_util::ConvertBSTRToString(v.bstrVal);  
														tmp_TRAIN_TYPE.prefix = p1;	
														delete  p1;   		
														 SysFreeString(v.bstrVal);
														v.Clear();
												}
												//@start
												pChildNode = pNode_TRAIN_ID_RANGE->selectSingleNode("@start");
												if(pChildNode)
												{
													_variant_t v;
													v = pChildNode->GetnodeTypedValue();
													tmp_TRAIN_TYPE.start = (ULONG)atol(CString(v.bstrVal));	
													SysFreeString(v.bstrVal);
													v.Clear();
												}
												//@end
												pChildNode = pNode_TRAIN_ID_RANGE->selectSingleNode("@end");
												if(pChildNode)
												{
													_variant_t v;
													v = pChildNode->GetnodeTypedValue();
													tmp_TRAIN_TYPE.end = (ULONG)atol(CString(v.bstrVal));	
													SysFreeString(v.bstrVal);
													v.Clear();
												}
									}
									tmp_TRAIN_TYPE.id = tmp_id;
									tmp_TRAIN_TYPE.name = tmp_name;
									tmp_TRAIN_TYPE.run_type = tmp_run_type;
									tmp_TRAIN_TYPE.priority = tmp_priority;
									m_map_TRAIN_TYPES.insert(make_pair(train_type_Quantity,tmp_TRAIN_TYPE));
									train_type_Quantity ++;	
						    }
				}
			}
		}	
	}	
}

CGetTrain_TypeInfor::CGetTrain_TypeInfor()
{
	::CoInitialize(0);
	init();
}

void CGetTrain_TypeInfor:: init()
{
	m_default_train_type = (BYTE)-1;
	train_type_Quantity = 0;
	m_map_PRIORITIES.clear();
	m_map_RUN_TYPES.clear();
	m_map_TRAIN_TYPES.clear();
}


CGetTrain_TypeInfor::~CGetTrain_TypeInfor()
{

}

/********************************************************************************************************************************
*功能：给一个train_id（K1200），得到列车类型id（train_type_id）,运行类型id（run_type_id），
			  列车优先级id（prioritiy_id）
*返回值：sucess：0
*					      fail：-1
*参数传递：列车类型id（train_type_id）,运行类型id（run_type_id），列车优先级id（prioritiy_id）
*作者：cf
*********************************************************************************************************************************/
bool CGetTrain_TypeInfor::GetPrioFromTrainId(const char *train_id,BYTE* train_type_id, BYTE* run_type_id, BYTE* prioritiy_id)
{
	TRAIN_NO prefix;
	ULONG ii_train_id;
	BYTE trn_type = 0;
	BYTE map_id = 0;
	string map_name;
	map_name.clear();
	BYTE map_run_type = -1;
	string map_prefix;
	map_prefix.clear();
	TRAIN_NO   map_prefix_to_compare;
	BYTE map_priority = -1;
	ULONG map_start = 0;
	ULONG map_end = 0;
	memset(prefix, 0, sizeof(TRAIN_NO));
	if(train_id != NULL && 0 < strlen(train_id) && strlen(train_id) < sizeof(TRAIN_NO))
	{
		ii_train_id = GetPrefixAndDigitalTrainId(train_id, prefix);
		prefix[sizeof(TRAIN_NO)-1]=0;
		std::map<short,TRAIN_TYPES >::const_iterator it = m_map_TRAIN_TYPES.begin();
		for(; it != m_map_TRAIN_TYPES.end(); ++it)
		{
			map_id = it->second.id;
			map_name = it->second.name;
			map_run_type= it->second.run_type;
			map_prefix= it->second.prefix;
			strncpy(map_prefix_to_compare,map_prefix.data(),sizeof(map_prefix_to_compare)-1);
			map_prefix_to_compare[sizeof(map_prefix_to_compare)-1] = '\0';
			map_priority= it->second.priority;
			map_start= it->second.start;
			map_end= it->second.end;		
			if (map_start <= ii_train_id && map_end >= ii_train_id &&
					strcmp(map_prefix_to_compare, prefix)==0)
			{
				*train_type_id = map_id;
				*run_type_id = map_run_type;
				*prioritiy_id = map_priority;
				return true;
			}		
		}  
	}
	
	//未找到属性，附一个属性编号为255(m_default_train_type)的“未知列车”类型
	std::map<BYTE,TRAIN_TYPES_ID_NAME_RUNTYPE_PRI>::const_iterator it_type_default = m_map_TRAIN_TYPE_DETAILS.find(m_default_train_type);
	if (it_type_default != m_map_TRAIN_TYPE_DETAILS.end())
	{
		*train_type_id = m_default_train_type;
		*run_type_id = it_type_default->second.run_type;
		*prioritiy_id = it_type_default->second.priority;
		return true;
	}
	else
	{		
		return false;
	}
}


/********************************************************************************************************************************
*功能：给一个train_id（K1200），返回前缀prefix（K）和后面的数字（1200）
*返回值：sucess：train_id后面的数字（1200）
*					      fail：0
*参数传递：前缀prefix
*作者：casco前辈
*********************************************************************************************************************************/
ULONG CGetTrain_TypeInfor::GetPrefixAndDigitalTrainId(const char *train_id, char *prefix)
{
	if ((train_id == NULL) || (strlen(train_id) == 0) || (strlen(train_id) > sizeof(TRAIN_NO)))
		return 0;	
	TRAIN_NO tmp;
	//char tmp[TRAIN_ID_LEN];
	int firstDigitIdx=0, lastDigitIdx=-1;
	int trainidlen = strlen(train_id);
	for(int i=trainidlen-1; i>=0; i--)
	{	
		if((lastDigitIdx<0) && iswdigit(train_id[i])){
			lastDigitIdx = i;
		}
		if((lastDigitIdx>-1) && (!iswdigit(train_id[i])))
		{
			firstDigitIdx = i + 1;
			break;
		}
	}
	if (lastDigitIdx<0)
	{
		if (prefix != NULL)
			strncpy(prefix, train_id, sizeof(TRAIN_NO)-1);
		return 0;
	}

	if (firstDigitIdx == 0)
		if (trainidlen>1 && strncmp(train_id, "00", 2) == 0)
			firstDigitIdx = 2;
		else if (strncmp(train_id, "0", 1) == 0)
			firstDigitIdx = 1;
	if (prefix != NULL)
	{
		if (firstDigitIdx == 0)
			*prefix = '\0';
		else
		{
			strncpy(prefix, train_id, firstDigitIdx);
			prefix[firstDigitIdx] = '\0';
		}
	}
	memset(tmp,0,sizeof(tmp));
	strncpy(tmp, train_id+firstDigitIdx, lastDigitIdx-firstDigitIdx+1);
	return atoi(tmp);
}

/********************************************************************************************************************************
*功能：提供列车类型id，得到该类型对应的名称
*返回值：sucess：返回该类型对应名称的指针
*					      fail：NULL
*参数传递：
*作者：cf
*********************************************************************************************************************************/
const char* CGetTrain_TypeInfor::GetTrainTypeName(const BYTE train_type_id)
{
	std::map<BYTE,TRAIN_TYPES_ID_NAME_RUNTYPE_PRI>::const_iterator it = m_map_TRAIN_TYPE_DETAILS.find(train_type_id);
	if (it != m_map_TRAIN_TYPE_DETAILS.end())
	{
		return ( it->second.name.data());	
	}
	return NULL;
}

const char* CGetTrain_TypeInfor::GetAbsTimeTypeName(int index, int *value_id)
{
	std::map<short,RUN_TYPES>::const_iterator it = m_map_ABS_TIME_TYPES.begin(); 
	int i=0;
	while(it != m_map_ABS_TIME_TYPES.end() && i<index)
	{
		it++;
		i++;
	}
	if(it != m_map_ABS_TIME_TYPES.end())	
	{
		if (NULL != value_id)
		*value_id=it->first; 
		return ( it->second.name.data());	
	}
	else
		return NULL;
}

int CGetTrain_TypeInfor::GetSpecRunTypeName(CStringArray& name_list, CWordArray& run_type_list)
{
	name_list.RemoveAll();
	run_type_list.RemoveAll();
	std::map<short, RUN_TYPES>::const_iterator it = m_map_ABS_TIME_TYPES.begin();
	for (;it != m_map_ABS_TIME_TYPES.end(); it++)
	{
		CString name=it->second.name.data();
		name_list.Add(name);
		run_type_list.Add(it->second.id);
	}
	return run_type_list.GetCount();
}

const char* CGetTrain_TypeInfor::GetAbsRunTypeName(const BYTE run_type_id)
{
	std::map<short, RUN_TYPES>::const_iterator it = m_map_ABS_TIME_TYPES.find(run_type_id);
	if (it != m_map_ABS_TIME_TYPES.end())
	{
		return ( it->second.name.data());	
	}
	return NULL;
}

bool  CGetTrain_TypeInfor::IsEnableABSTimeType()
{
	if(m_map_ABS_TIME_TYPES.empty())
		return false;
	return true;
}
/********************************************************************************************************************************
*功能：提供列车运行类型id，得到该类型对应的名称
*返回值：sucess：返回该类型对应名称的指针
*					      fail：NULL
*参数传递：
*作者：cf
*********************************************************************************************************************************/
const char* CGetTrain_TypeInfor::GetRunTypeName(const BYTE run_type_id)
{
	std::map<short, RUN_TYPES>::const_iterator it = m_map_RUN_TYPES.find(run_type_id);
	if (it != m_map_RUN_TYPES.end())
	{
		return ( it->second.name.data());	
	}
	return NULL;
}

/********************************************************************************************************************************
*功能：提供列车优先级id，得到该类型对应的名称
*返回值：sucess：返回该类型对应名称的指针
*					      fail：NULL
*参数传递：
*作者：cf
*********************************************************************************************************************************/
const char* CGetTrain_TypeInfor::GetPrioTypeName(const BYTE prioritiy_id)
{
	std::map<short, PRIORITIES>::const_iterator it = m_map_PRIORITIES.find(prioritiy_id);
	if (it != m_map_PRIORITIES.end())
	{
		return ( it->second.name.data());	
	}
	return NULL;
}

/********************************************************************************************************************************
*功能：提供铁道部规定所有运行类型名称，放在vector数组中
*返回值：sucess：true
*					      fail：false
*参数传递：通过vector传递
*作者：cf
*********************************************************************************************************************************/
bool CGetTrain_TypeInfor::GetRunTypeNameArray (vector <string> &vec)
{
	vec.clear();
	std::map<short,RUN_TYPES >::const_iterator it = m_map_RUN_TYPES.begin();
	if (m_map_RUN_TYPES.size() <= 0)
	{
	     return false;
	}
	for(; it != m_map_RUN_TYPES.end(); ++it)
	{
		vec.push_back(it->second.name);
	}
	return true;
}

/********************************************************************************************************************************
*功能：提供铁道部规定所有列车类型名称，放在vector数组中
*返回值：sucess：true
*					      fail：false
*参数传递：通过vector传递
*作者：cf
*********************************************************************************************************************************/
bool CGetTrain_TypeInfor::GetTrainTypeNameArray (vector <string> &vec)
{
	vec.clear();
	std::map<BYTE,TRAIN_TYPES_ID_NAME_RUNTYPE_PRI> ::const_iterator it = m_map_TRAIN_TYPE_DETAILS.begin();
	if (m_map_TRAIN_TYPE_DETAILS.size() <= 0)
	{
		return false;
	}
	for(; it != m_map_TRAIN_TYPE_DETAILS.end(); ++it)
	{
		if ("未知列车"  ==  it->second.name)
		{
			continue;
		}
		vec.push_back(it->second.name);
	}
	return true;
}

/********************************************************************************************************************************
*功能：提供铁道部规定所有列车优先级名称，放在vector数组中
*返回值：sucess：true
*					      fail：false
*参数传递：通过vector传递
*作者：cf
*********************************************************************************************************************************/
bool CGetTrain_TypeInfor::GetPrioTypeNameArray (vector <string> &vec)
{
	vec.clear();
	std::map<short,PRIORITIES >::const_iterator it = m_map_PRIORITIES.begin();
	if (m_map_PRIORITIES.size() <= 0)
	{
		return false;
	}
	for(; it != m_map_PRIORITIES.end(); ++it)
	{
		vec.push_back(it->second.name);
	}
	return true;
}

bool CGetTrain_TypeInfor::getDataDir(char * const strPath)
{
	strcpy(strPath, "");
	char sPath[128], *psDest;

	if(_getcwd(sPath, 128) == NULL)
		return false;

	psDest = strrchr(sPath, '\\');
	int nResult = psDest - sPath;
	char path1[128];
	strncpy(path1, sPath, nResult);
	path1[nResult] = 0x00;

	strcpy(strPath, path1);
	return true;
}

BOOL CGetTrain_TypeInfor::GetTrainTypeDef(BYTE  train_type,BYTE *run_type,BYTE *prio,BYTE *locotype)
{
	std::map<BYTE,TRAIN_TYPES_ID_NAME_RUNTYPE_PRI>::const_iterator it = m_map_TRAIN_TYPE_DETAILS.find(train_type);
	if(it!=m_map_TRAIN_TYPE_DETAILS.end())
	{
		if(run_type)
			*run_type=it->second.run_type;  
		if(prio)
			*prio=it->second.priority; 
		if(locotype)
			*locotype=it->second.loco_type;
		return TRUE;
	}
	else
		return FALSE;
}

const char* CGetTrain_TypeInfor::GetTrainTypeName(int index,int *value_id)
{
	std::map<BYTE,TRAIN_TYPES_ID_NAME_RUNTYPE_PRI>::const_iterator it = m_map_TRAIN_TYPE_DETAILS.begin(); 
	int i=0;
	while(it != m_map_TRAIN_TYPE_DETAILS.end() && i<index)
	{
		it++;
		i++;
	}
	if(it != m_map_TRAIN_TYPE_DETAILS.end())	
	{
		*value_id=it->first; 
		return ( it->second.name.data());	
	}
	else
		return NULL;
}

const char* CGetTrain_TypeInfor::GetRunTypeName(int index, int *value_id)
{
	std::map<short,RUN_TYPES>::const_iterator it = m_map_RUN_TYPES.begin(); 
	int i=0;
	while(it != m_map_RUN_TYPES.end() && i<index)
	{
		it++;
		i++;
	}
	if(it != m_map_RUN_TYPES.end())	
	{
		if (NULL != value_id)
		*value_id=it->first; 
		return ( it->second.name.data());	
	}
	else
		return NULL;
}

const char* CGetTrain_TypeInfor::GetPrioTypeName(int index,int *value_id)
{
	std::map<short,PRIORITIES>::const_iterator it = m_map_PRIORITIES.begin(); 
	int i=0;
	while(it != m_map_PRIORITIES.end() && i<index)
	{
		it++;
		i++;
	}
	if(it != m_map_PRIORITIES.end())	
	{
		*value_id=it->first; 
		return ( it->second.name.data());	
	}
	else
		return NULL;
}