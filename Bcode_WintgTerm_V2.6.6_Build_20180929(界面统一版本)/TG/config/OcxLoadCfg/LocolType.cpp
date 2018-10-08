#include "StdAfx.h"
#include ".\locoltype.h"
#import  <msxml.dll> named_guids
using namespace MSXML;

CLocolType::CLocolType(void)
{
	mapLocol.clear();
}

CLocolType::~CLocolType(void)
{
	mapLocol.clear();
}

int CLocolType::GetLocoAllPref(int con_id, CStringArray& aryName, CStringArray& arypref, CWordArray& aryid, CWordArray& aryCascoid)
{
	arypref.RemoveAll();
	std::map<int, CON_ID_LOCOL>::const_iterator it = mapLocol.find(con_id);
	if(it == mapLocol.end())
		it = mapLocol.find(0);

	if(it != mapLocol.end())
	{
		for(int i=0;i<it->second.vecLocol.size();i++)
		{
			arypref.Add(it->second.vecLocol[i].pref);
			aryName.Add(it->second.vecLocol[i].name);
			aryid.Add(it->second.vecLocol[i].type);
			aryCascoid.Add(it->second.vecLocol[i].casco_id);
		}
	}

	return arypref.GetCount();
}

CString CLocolType::GetLocoName(int con_id, CString pref)
{
	CString name = "";
	std::map<int, CON_ID_LOCOL>::const_iterator it = mapLocol.find(con_id);
	if(it == mapLocol.end())
		it = mapLocol.find(0);

	if(it != mapLocol.end())
	{
		for(int i=0;i<it->second.vecLocol.size();i++)
		{
			if(it->second.vecLocol[i].pref == pref)
				return it->second.vecLocol[i].name;
		}
	}

	return name;
}

CString CLocolType::GetLocoType(int con_id, CString pref)
{
	CString type = "";
	std::map<int, CON_ID_LOCOL>::const_iterator it = mapLocol.find(con_id);
	if(it == mapLocol.end())
		it = mapLocol.find(0);

	if(it != mapLocol.end())
	{
		for(int i=0;i<it->second.vecLocol.size();i++)
		{
			if(it->second.vecLocol[i].pref == pref)
				return it->second.vecLocol[i].szType;
		}
	}

	return type;
}

int CLocolType::GetCascoId(int con_id, CString pref)
{
	int cascoId = 0;
	std::map<int, CON_ID_LOCOL>::const_iterator it = mapLocol.find(con_id);
	if(it == mapLocol.end())
		it = mapLocol.find(0);

	if(it != mapLocol.end())
	{
		for(int i=0;i<it->second.vecLocol.size();i++)
		{
			if(it->second.vecLocol[i].pref == pref)
				return it->second.vecLocol[i].casco_id;
		}
	}

	return cascoId;
}

bool CLocolType::GetLocoInfo(int con_id, CString pref, CString& name, CString& type, int& cascoId)
{
	std::map<int, CON_ID_LOCOL>::const_iterator it = mapLocol.find(con_id);
	if(it == mapLocol.end())
		it = mapLocol.find(0);

	if(it != mapLocol.end())
	{
		for(int i=0;i<it->second.vecLocol.size();i++)
		{
			if(it->second.vecLocol[i].pref == pref)
			{
				name = it->second.vecLocol[i].name;
				type = it->second.vecLocol[i].szType;
				cascoId = it->second.vecLocol[i].casco_id;
				return true;
			}
		}
	}

	return false;
}

bool CLocolType::LoadLocolTypeXml(CString path)
{
	mapLocol.clear();
	
	MSXML::IXMLDOMDocumentPtr pXMLDoc;
	HRESULT hr = pXMLDoc.CreateInstance(MSXML::CLSID_DOMDocument);
	if(FAILED(hr))
	{
		_com_error er(hr);
		AfxMessageBox(er.ErrorMessage());
		return false;
	}
	
	CString pathname = path + "\\locotype.xml";
	if(VARIANT_TRUE != pXMLDoc->load( _bstr_t( pathname ) ) )
	{
		CString  prompt;
		prompt.Format("配置%s不存在或者不符合xml语法(可以用浏览器检查是否能打开)", pathname);
		return ReadFile(path);
	}
	
	MSXML::IXMLDOMNodePtr pChildNode,pNode,pRootNode;

	pRootNode = pXMLDoc->selectSingleNode( "LOCOTYPE");
	if(pRootNode == NULL)
		return false;

	MSXML::IXMLDOMNodeListPtr pListCon = pRootNode->selectNodes( "CON_ID");
	if(NULL != pListCon)
	{
		for(int idx = 0; idx < pListCon->Getlength(); idx++)
		{
			pNode = pListCon->Getitem(idx);
			if(pNode)
			{
				int con_id = 0;
				CON_ID_LOCOL locol;
				pChildNode = pNode->selectSingleNode("@ID");
				if(pChildNode)
				{
					_variant_t v;
					v = pChildNode->GetnodeTypedValue();
					locol.con_id = atoi(CString(v.bstrVal));
					SysFreeString(v.bstrVal);
					v.Clear();
				}

				MSXML::IXMLDOMNodeListPtr pListEngine = pNode->selectNodes( "ENGINE");
				if(NULL != pListEngine)
				{
					for(int idx = 0; idx < pListEngine->Getlength(); idx++)
					{
						pNode = pListEngine->Getitem(idx);
						if(pNode)
						{
							LOCOL_TYPE locol_type;
							pChildNode = pNode->selectSingleNode("@pref");
							if(pChildNode)
							{
								_variant_t v;
								v = pChildNode->GetnodeTypedValue();
								CString pref = CString(v.bstrVal);

								strncpy(locol_type.pref, pref.GetBuffer(), 9);
								locol_type.pref[9]=0;

								SysFreeString(v.bstrVal);
								v.Clear();
							}
							pChildNode = pNode->selectSingleNode("@locoid");
							if(pChildNode)
							{
								_variant_t v;
								v = pChildNode->GetnodeTypedValue();

								locol_type.type = atoi(CString(v.bstrVal));
								CString szType;
								szType.Format("%0.3d", locol_type.type);
								if(szType.GetLength()>3)
									szType = szType.Left(3);

								strncpy(locol_type.szType, szType.GetBuffer(), 4);
								locol_type.szType[4]=0;

								SysFreeString(v.bstrVal);
								v.Clear();
							}
							pChildNode = pNode->selectSingleNode("@name");
							if(pChildNode)
							{
								_variant_t v;
								v = pChildNode->GetnodeTypedValue();
								CString name = CString(v.bstrVal);

								strncpy(locol_type.name, name.GetBuffer(), 19);
								locol_type.name[19]=0;

								SysFreeString(v.bstrVal);
								v.Clear();
							}
							pChildNode = pNode->selectSingleNode("@casco_id");
							if(pChildNode)
							{
								_variant_t v;
								v = pChildNode->GetnodeTypedValue();

								CString name = CString(v.bstrVal);
								locol_type.casco_id = atoi(name);

								SysFreeString(v.bstrVal);
								v.Clear();
							}
							
							if(locol_type.pref[0] !=0)
								locol.vecLocol.push_back(locol_type);
						}
					}
				}

				mapLocol.insert(make_pair(locol.con_id, locol));
			}
		}
	}

	std::map<int, LOCOL_TYPE> tmpAllLoco;
	
	std::map<int, CON_ID_LOCOL>::iterator it = mapLocol.begin();
	for(; it!=mapLocol.end(); it++)
	{
		for(int i=0; i<it->second.vecLocol.size(); i++)
		{
			tmpAllLoco[it->second.vecLocol[i].type] = it->second.vecLocol[i];
		}
	}
	
	CON_ID_LOCOL locol;
	locol.con_id = 0;
	
	std::map<int, LOCOL_TYPE>::iterator it1 = tmpAllLoco.begin();
	for(; it1!=tmpAllLoco.end(); it1++)
	{
		locol.vecLocol.push_back(it1->second);
	}
	tmpAllLoco.clear();

	mapLocol.insert(make_pair(locol.con_id, locol));

	return true;
}


bool CLocolType::ReadFile(CString path)
{
	char seps[]=" ,\t\\/=\n";

	FILE *fp=NULL;

	CString pathname = path + "\\locotype.cfg";
	char *ptr, strLine[512];
	if((fp=fopen(pathname, "r"))==NULL)
	{
		AfxMessageBox("缺少配置文件locotype.cfg");
		return false;
	}

	// read file
	int m_nTgErrorline = 0;
	
	CON_ID_LOCOL locol;
	locol.con_id = 0;
	
	while (!feof(fp))	
	{
		ptr = fgets(strLine, 512, fp);
		m_nTgErrorline++;
		if (ptr == NULL) 
			continue;
		while (*ptr == ' ' || *ptr == '\t')
			ptr ++;
		if (*ptr == ';')	
			continue;
		if (*ptr == '\n')	
			continue;
		if (*ptr == '[')	
			continue;

	    // 解析
		LOCOL_TYPE engine;

		char* token = strtok(ptr, seps);
		if(token==NULL) break;
		strncpy(engine.szType, token, 4);
		engine.pref[4] = 0;
		
		engine.type = atoi(engine.szType);
	
		// no
		token = strtok(NULL, seps);
		if(token==NULL) break;
		engine.casco_id = atoi(token);
		
		// 字母name
		token = strtok(NULL, seps);
		if(token==NULL) break;
		strncpy(engine.pref, token, 9);
		engine.pref[9] = 0;

		// 汉字name
		token = strtok(NULL, seps);
		if(token!=NULL)
		{
			strncpy(engine.name, token, 19);
			engine.name[19] = 0;
		}
		else
		{
			memset(engine.name, 0, 20);
		}
			
		locol.vecLocol.push_back(engine);
	}

	if(!feof(fp))
	{
		fclose(fp);
		CString msg;
		msg.Format("文件%s的第%d有错", pathname, m_nTgErrorline);
		AfxMessageBox(msg);

		return false;
	}

	mapLocol.insert(make_pair(locol.con_id, locol));
	fclose(fp);
	return true;
}