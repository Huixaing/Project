#include "stdafx.h"
#include "traintype.h"

#import  <msxml.dll> named_guids
using namespace MSXML;

TRAIN_ID_DEFINES::TRAIN_ID_DEFINES()
{
	id = 0;
	memset(name, 0, sizeof(name));
	run_type = 0;
	priority = 0;
	loco_type = 0;
	trainId.clear();
}
TRAIN_ID_DEFINES::TRAIN_ID_DEFINES(const TRAIN_ID_DEFINES& sSource)
{
	this->id = sSource.id;
	strncpy(this->name, sSource.name, sizeof(sSource.name));
	this->run_type = sSource.run_type;
	this->priority = sSource.priority;
	this->loco_type = sSource.loco_type;

	for(UINT i=0; i<sSource.trainId.size(); i++)
	{
		this->trainId.push_back(sSource.trainId[i]);
	}
}

TRAIN_ID_DEFINES& TRAIN_ID_DEFINES::operator=(const TRAIN_ID_DEFINES& sSource)
{
	if (this == &sSource)
	{
		return (*this);		
	}
	trainId.clear();

	id = sSource.id;
	strncpy(name, sSource.name, sizeof(sSource.name));
	run_type = sSource.run_type;
	priority = sSource.priority;
	loco_type = sSource.loco_type;

	for(UINT i=0; i<sSource.trainId.size(); i++)
	{
		trainId.push_back(sSource.trainId[i]);
	}

	return (*this);
}
TRAIN_ID_DEFINES::~TRAIN_ID_DEFINES()
{
	trainId.clear();
}

CTrainTypeInfo::CTrainTypeInfo()
{
	::CoInitialize(0);
	m_default_train_type = 255;
	m_map_train_id_defines.clear();
}

CTrainTypeInfo::~CTrainTypeInfo()
{
	m_default_train_type = 255;
	m_map_train_id_defines.clear();
}

int CTrainTypeInfo::GetTrainTypeByTrainNO(CString trainid)
{
	char train_id[30], prefix[20];
	memset(train_id, 0, sizeof(train_id)/sizeof(train_id[0]));
	memset(prefix, 0, sizeof(prefix)/sizeof(prefix[0]));
	strncpy(train_id, trainid.GetBuffer(), sizeof(train_id)/sizeof(train_id[0])-1);

	ULONG digtal = GetPrefixAndDigitalTrainId(train_id, prefix);
	std::map<int, TRAIN_ID_DEFINES>::iterator it = m_map_train_id_defines.begin();
	while(it != m_map_train_id_defines.end())
	{
		for(UINT i=0; i<it->second.trainId.size(); i++)
		{
			if(digtal >= it->second.trainId[i].start && digtal <= it->second.trainId[i].end)
			{
				if(strcmp(it->second.trainId[i].prefix, prefix) == 0)
					return it->second.id;
			}
		}
		it++;
	}

	return 255;
}
/********************************************************************************************************************************
*功能：给一个train_id（K1200），返回前缀prefix（K）和后面的数字（1200）
*返回值：sucess：train_id后面的数字（1200）
*					      fail：0
*参数传递：前缀prefix
*作者：casco前辈
*********************************************************************************************************************************/
ULONG CTrainTypeInfo::GetPrefixAndDigitalTrainId(const char *train_id, char *prefix)
{
	if (train_id == NULL)
		return 0;

	if (strlen(train_id) == 0)
		return 0;

	char tmp[12];
	int firstDigitIdx=0, lastDigitIdx=-1;
	int trainidlen = strlen(train_id);
	for(int i=trainidlen-1; i>=0; i--)
	{	
		if((lastDigitIdx<0) && iswdigit(train_id[i])){
			lastDigitIdx = i;
		}
		if ((lastDigitIdx>-1) && (!iswdigit(train_id[i])))
		{
			firstDigitIdx = i + 1;
			break;
		}
	}
	if (lastDigitIdx<0)
	{
		if (prefix != NULL)
			strcpy(prefix, train_id);
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

void CTrainTypeInfo::LoadTrain_TypeXML(const CString pathname)
{
	MSXML::IXMLDOMDocumentPtr pXMLDoc;
	HRESULT hr = pXMLDoc.CreateInstance(MSXML::CLSID_DOMDocument);
	if(FAILED(hr))
	{
		_com_error er(hr);
		AfxMessageBox(er.ErrorMessage());
		return;
	}

	if(VARIANT_TRUE != pXMLDoc->load( _bstr_t( pathname ) ) )
	{
		CString  prompt;
		prompt.Format("配置%s不存在或者不符合xml语法(可以用浏览器检查是否能打开)", pathname);
		AfxMessageBox(prompt);
		return; 
	}
	MSXML::IXMLDOMNodePtr pRootNode,pChildNode,pNode,pTrainTypes;

	MSXML::IXMLDOMNodePtr pNode_TRAIN_TYPES,pNode_TRAIN_TYPE;
	MSXML::IXMLDOMNodePtr pNode_TRAIN_ID_RANGE;
	pRootNode = pXMLDoc->selectSingleNode( "TRAIN_TYPE_DEF");
	if(pRootNode == NULL)
	{
		return;
	}
	
	m_map_train_id_defines.clear();

	pTrainTypes = pRootNode->selectSingleNode( "TRAIN_TYPES");
	if(pTrainTypes == NULL)
	{
		return;
	}
	
	//读取默认列车属性default_train_type
	pNode = pTrainTypes->selectSingleNode("@default_train_type");
	if(pNode)
	{
		_variant_t v;
		v = pNode->GetnodeTypedValue();
		m_default_train_type = atoi(CString(v.bstrVal));	
		SysFreeString(v.bstrVal);
		v.Clear();
	}

	MSXML::IXMLDOMNodeListPtr list_TRAIN_TYPE = pTrainTypes->selectNodes( "TRAIN_TYPE");
	if(NULL != list_TRAIN_TYPE)
	{
		for(int idx = 0; idx < list_TRAIN_TYPE->Getlength(); idx++)
		{
			TRAIN_ID_DEFINES trainIdDefines;
			pNode_TRAIN_TYPE = list_TRAIN_TYPE->Getitem(idx);
			if(pNode_TRAIN_TYPE)
			{
				// @id
				pChildNode = pNode_TRAIN_TYPE->selectSingleNode("@id");
				if(pChildNode)
				{
					_variant_t v;
					v = pChildNode->GetnodeTypedValue();
					trainIdDefines.id = atoi(CString(v.bstrVal));	
					SysFreeString(v.bstrVal);
					v.Clear();
				}
				//@name
				pChildNode = pNode_TRAIN_TYPE->selectSingleNode("@name");
				if(pChildNode)
				{
					_variant_t v;
					v = pChildNode->GetnodeTypedValue();										
					CString tmp_name = CString(v.bstrVal); 
					strncpy(trainIdDefines.name, tmp_name.GetBuffer(), sizeof(trainIdDefines.name)-1);
					trainIdDefines.name[sizeof(trainIdDefines.name)-1]=0;
					SysFreeString(v.bstrVal);
					v.Clear();
				}
				//@run_type
				pChildNode = pNode_TRAIN_TYPE->selectSingleNode("@run_type");
				if(pChildNode)
				{
					_variant_t v;
					v = pChildNode->GetnodeTypedValue();
					trainIdDefines.run_type = atoi(CString(v.bstrVal));	
					SysFreeString(v.bstrVal);
					v.Clear();
				}
				//@priority
				pChildNode = pNode_TRAIN_TYPE->selectSingleNode("@priority");
				if(pChildNode)
				{
					_variant_t v;
					v = pChildNode->GetnodeTypedValue();
					trainIdDefines.priority = atoi(CString(v.bstrVal));	
					SysFreeString(v.bstrVal);
					v.Clear();
				}
				//@loco_type
				pChildNode = pNode_TRAIN_TYPE->selectSingleNode("@loco_type");
				if(pChildNode)
				{
					_variant_t v;
					v = pChildNode->GetnodeTypedValue();
					trainIdDefines.loco_type = atoi(CString(v.bstrVal));	
					SysFreeString(v.bstrVal);
					v.Clear();
				}
				
				MSXML::IXMLDOMNodeListPtr list_TRAIN_ID_RANGE = pNode_TRAIN_TYPE->selectNodes( "TRAIN_ID_RANGE");
				if(NULL != list_TRAIN_ID_RANGE)
				{
					for(int idx_trn_id_range = 0; idx_trn_id_range < list_TRAIN_ID_RANGE->Getlength(); idx_trn_id_range++)
					{
						pNode_TRAIN_ID_RANGE = list_TRAIN_ID_RANGE->Getitem(idx_trn_id_range);
						if(pNode_TRAIN_ID_RANGE)
						{
							TRAIN_ID_DEFINE trainIdDefine;
							// @prefix
							pChildNode = pNode_TRAIN_ID_RANGE->selectSingleNode("@prefix");
							if(pChildNode)
							{
								_variant_t v;
								v = pChildNode->GetnodeTypedValue();

								CString tmp = CString(v.bstrVal);
								strncpy(trainIdDefine.prefix, tmp.GetBuffer(), 9);	
								trainIdDefine.prefix[9] = 0;
								  		
								SysFreeString(v.bstrVal);
								v.Clear();
							}
							//@start
							pChildNode = pNode_TRAIN_ID_RANGE->selectSingleNode("@start");
							if(pChildNode)
							{
								_variant_t v;
								v = pChildNode->GetnodeTypedValue();
								trainIdDefine.start = (ULONG)atol(CString(v.bstrVal));	
								SysFreeString(v.bstrVal);
								v.Clear();
							}
							//@end
							pChildNode = pNode_TRAIN_ID_RANGE->selectSingleNode("@end");
							if(pChildNode)
							{
								_variant_t v;
								v = pChildNode->GetnodeTypedValue();
								trainIdDefine.end = (ULONG)atol(CString(v.bstrVal));	
								SysFreeString(v.bstrVal);
								v.Clear();
							}

							trainIdDefines.trainId.push_back(trainIdDefine);
						}
					}
				}
			}

			m_map_train_id_defines.insert(make_pair(trainIdDefines.id, trainIdDefines));
		}	
	}	
}
