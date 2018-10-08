#include "StdAfx.h"
#include ".\traintypecfg.h"
#import  <msxml.dll> named_guids
using namespace MSXML;

TrainIdDesp::TrainIdDesp()
{
	beginNo = -1;
	endNo = -1;
	prefix = "";
}

TrainAttribute::TrainAttribute()
{
	type = 0;
	priority = 0;
	run_type = 0;
	name.Empty();
	trainid_desp.clear();
}

TrainAttribute::~TrainAttribute()
{
	trainid_desp.clear();
}

TrainAttribute::TrainAttribute(const TrainAttribute& rhs)
{
	type = rhs.type;
	priority = rhs.priority;
	run_type = rhs.run_type;
	name = rhs.name;

	trainid_desp.clear();
	for(int i = 0; i < rhs.trainid_desp.size(); i++)
		trainid_desp.push_back(rhs.trainid_desp[i]);
}
TrainAttribute& TrainAttribute::operator=(const TrainAttribute& rhs)
{
	if(this == &rhs)
		return *this;

	type = rhs.type;
	priority = rhs.priority;
	run_type = rhs.run_type;
	name = rhs.name;

	trainid_desp.clear();

	for(int i = 0; i < rhs.trainid_desp.size(); i++)
		trainid_desp.push_back(rhs.trainid_desp[i]);

	return *this;
}

TrainTypeCfg::TrainTypeCfg(void)
{
}

TrainTypeCfg::~TrainTypeCfg(void)
{
	mTrainAttributeMap.clear();
	mPriorityNameMap.clear();
	mRunTypeNameMap.clear();
	mAbsTimeTypeNameMap.clear();
}

int TrainTypeCfg::GetTrainTypeByTrainId(const CString& trainid)
{
	if(trainid.IsEmpty())
		return 255;
    
	// 1. 分解
	CString tmpTrainId = trainid;
	do
	{
		char ch = tmpTrainId.GetAt(tmpTrainId.GetLength()-1);
		if( ch <'0' || ch > '9')
			tmpTrainId = tmpTrainId.Left(tmpTrainId.GetLength()-1);
		else
		    break;
	}while(1);

	if(tmpTrainId.IsEmpty())
		return 255;

	int i = -1;
	for(i = tmpTrainId.GetLength()-1; i > -1; i--)
	{
		char ch = tmpTrainId.GetAt(i);
		if('0' <= ch && ch <= '9')
		{
			continue;
		}
		else
		{
			break;
		}
	}
    
	long num = 0;
	CString prefix;
	if(i == 0)
	{
		prefix = "";
		num = atol(tmpTrainId);
	}
	else
	{
		prefix = tmpTrainId.Left(tmpTrainId.GetLength()-i);
		num = atol(tmpTrainId.Right(i));
	}

	// 2. 查表
	std::map<int, TrainAttribute>::iterator it = mTrainAttributeMap.begin();
	for(; it != mTrainAttributeMap.end(); it++)
	{
		for(int i = 0; i < (*it).second.trainid_desp.size(); i++)
		{
			const TrainIdDesp& desp = (*it).second.trainid_desp[i];
			if(desp.prefix == prefix && desp.beginNo <= num && num <= desp.endNo)
			{
				return (*it).second.type;
			}
		}
	}

	return 255;
}

bool TrainTypeCfg::ReadTrainType(CString strFileName)
{
	mTrainAttributeMap.clear();
	mPriorityNameMap.clear();
	mRunTypeNameMap.clear();

	MSXML::IXMLDOMDocumentPtr pXMLDoc;
	HRESULT hr = pXMLDoc.CreateInstance(MSXML::CLSID_DOMDocument);
	if(FAILED(hr))
	{
		_com_error er(hr);
		AfxMessageBox(er.ErrorMessage());
		return false;
	}

	if(VARIANT_TRUE != pXMLDoc->load( _bstr_t( strFileName ) ) )
	{
		CString  prompt;
		prompt.Format("配置%s不存在或者不符合xml语法(可以用浏览器检查是否能打开)", strFileName);
		AfxMessageBox(prompt);
		return false; 
	}

	MSXML::IXMLDOMNodePtr pChildNode,pNode,pRootNode,pTmpNode;

	pRootNode = pXMLDoc->selectSingleNode( "TRAIN_TYPE_DEF");
	if(pRootNode == NULL)
		return false;

	// RUN_TYPES/RUN_TYPE
	MSXML::IXMLDOMNodeListPtr list = pRootNode->selectNodes( "RUN_TYPES/RUN_TYPE");
	if(NULL != list)
	{
		int id = 0;
		CString name = "";
		for(int idx = 0; idx < list->Getlength(); idx++)
		{
			pNode = list->Getitem(idx);
			if(pNode)
			{
				id = 0;
				name = "";
				// @id
				pChildNode = pNode->selectSingleNode("@id");
				if(pChildNode)
				{
					_variant_t v;
					v = pChildNode->GetnodeTypedValue();
					id = atoi(CString(v.bstrVal));	
					SysFreeString(v.bstrVal);
					v.Clear();
				}
				else
				{
					continue;
				}

				// @name
				pChildNode = pNode->selectSingleNode("@name");
				if(pChildNode)
				{
					_variant_t v;
					v = pChildNode->GetnodeTypedValue();
					name = CString(v.bstrVal);	
					SysFreeString(v.bstrVal);
					v.Clear();
				}

				if(!name.IsEmpty())
				{
					mRunTypeNameMap[id] = name;
				}
			}
		}
		// 用户没有配置255，自动添加255，位置列车
		std::map<int, CString>::iterator it = mRunTypeNameMap.find(255);
		if(it == mRunTypeNameMap.end())
		{
			mRunTypeNameMap[255] = "未知列车";
		}
	}
	// ABS_TIME_TYPES/ABS_TIME_TYPE
	list = pRootNode->selectNodes( "ABS_TIME_TYPES/ABS_TIME_TYPE");
	if(NULL != list)
	{
		for(int idx = 0; idx < list->Getlength(); idx++)
		{
			pNode = list->Getitem(idx);
			if(pNode)
			{
				RunTypeNode nd;

				// @id
				pChildNode = pNode->selectSingleNode("@id");
				if(pChildNode)
				{
					_variant_t v;
					v = pChildNode->GetnodeTypedValue();
					nd.run_type = atoi(CString(v.bstrVal));	
					SysFreeString(v.bstrVal);
					v.Clear();
				}
				
				// @name
				pChildNode = pNode->selectSingleNode("@name");
				if(pChildNode)
				{
					_variant_t v;
					v = pChildNode->GetnodeTypedValue();
					CString name = CString(v.bstrVal);
					int len=sizeof(nd.name)-1;
					strncpy(nd.name, name, len);
					nd.name[len]=0;
					
					SysFreeString(v.bstrVal);
					v.Clear();
				}
				
				mAbsTimeTypeNameMap[nd.run_type] = nd;
			}
		}
	}
	
	// PRIORITIES/PRIORITY
	list = pRootNode->selectNodes( "PRIORITIES/PRIORITY");
	if(NULL != list)
	{
		int id = 0;
		CString name = "";
		for(int idx = 0; idx < list->Getlength(); idx++)
		{
			pNode = list->Getitem(idx);
			if(pNode)
			{
				id = 0;
				name = "";
				// @id
				pChildNode = pNode->selectSingleNode("@id");
				if(pChildNode)
				{
					_variant_t v;
					v = pChildNode->GetnodeTypedValue();
					id = atoi(CString(v.bstrVal));	
					SysFreeString(v.bstrVal);
					v.Clear();
				}
				else
				{
					continue;
				}

				// @name
				pChildNode = pNode->selectSingleNode("@name");
				if(pChildNode)
				{
					_variant_t v;
					v = pChildNode->GetnodeTypedValue();
					name = CString(v.bstrVal);	
					SysFreeString(v.bstrVal);
					v.Clear();
				}

				if(!name.IsEmpty())
				{
					mPriorityNameMap[id] = name;
				}
			}
		}
	}

	// TRAIN_TYPES/TRAIN_TYPE
	list = pRootNode->selectNodes( "TRAIN_TYPES/TRAIN_TYPE");
	if(NULL != list)
	{
		TrainAttribute attr;
		for(int idx = 0; idx < list->Getlength(); idx++)
		{
			pNode = list->Getitem(idx);
			if(pNode)
			{
				// @id
				pChildNode = pNode->selectSingleNode("@id");
				if(pChildNode)
				{
					_variant_t v;
					v = pChildNode->GetnodeTypedValue();
					attr.type = atoi(CString(v.bstrVal));	
					SysFreeString(v.bstrVal);
					v.Clear();
				}
				else
				{
					attr.type = 0;
				}

				// @run_type
				pChildNode = pNode->selectSingleNode("@run_type");
				if(pChildNode)
				{
					_variant_t v;
					v = pChildNode->GetnodeTypedValue();
					attr.run_type = atoi(CString(v.bstrVal));	
					SysFreeString(v.bstrVal);
					v.Clear();
				}
				else
				{
					attr.run_type = 0;
				}

				// @priority
				pChildNode = pNode->selectSingleNode("@priority");
				if(pChildNode)
				{
					_variant_t v;
					v = pChildNode->GetnodeTypedValue();
					attr.priority = atoi(CString(v.bstrVal));	
					SysFreeString(v.bstrVal);
					v.Clear();
				}
				else
				{
					attr.priority = 0;
				}

				// @name
				pChildNode = pNode->selectSingleNode("@name");
				if(pChildNode)
				{
					_variant_t v;
					v = pChildNode->GetnodeTypedValue();
					attr.name = CString(v.bstrVal);	
					SysFreeString(v.bstrVal);
					v.Clear();
				}
				else
				{
					attr.name.Empty();
				}

				// TRAIN_ID_RANGE
				MSXML::IXMLDOMNodeListPtr range = pNode->selectNodes( "TRAIN_ID_RANGE");
				for(int idx = 0; idx < range->Getlength(); idx++)
				{
					pNode = range->Getitem(idx);
					if(pNode)
					{
						TrainIdDesp desp;
						desp.beginNo = -1;
						desp.endNo   = -1;
						// prefix
						pChildNode = pNode->selectSingleNode("@prefix");
						if(pChildNode)
						{
							_variant_t v;
							v = pChildNode->GetnodeTypedValue();
							desp.prefix = CString(v.bstrVal);	
							SysFreeString(v.bstrVal);
							v.Clear();
						}
						else
						{
							desp.prefix.Empty();
						}

						// start
						pChildNode = pNode->selectSingleNode("@start");
						if(pChildNode)
						{
							_variant_t v;
							v = pChildNode->GetnodeTypedValue();
							desp.beginNo = atoi(CString(v.bstrVal));	
							SysFreeString(v.bstrVal);
							v.Clear();
						}

						// end
						pChildNode = pNode->selectSingleNode("@end");
						if(pChildNode)
						{
							_variant_t v;
							v = pChildNode->GetnodeTypedValue();
							desp.endNo = atoi(CString(v.bstrVal));	
							SysFreeString(v.bstrVal);
							v.Clear();
						}
						
						if(desp.endNo != -1 && desp.beginNo != -1)
						{
							attr.trainid_desp.push_back(desp);
						}
					}
				}

				// 
				mTrainAttributeMap[attr.type] = attr;
			}
		}
	}
	return true;
}

