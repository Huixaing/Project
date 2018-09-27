#include "stdafx.h"
#include "tg_passway.h"

#import  <msxml.dll> named_guids
using namespace MSXML;

Passway_Sta::Passway_Sta()
{
	init();
}
Passway_Sta::Passway_Sta(const Passway_Sta& sSource)
{
	this->Sta_id = sSource.Sta_id;
	this->Entry = sSource.Entry;
	this->Exit = sSource.Exit;
	this->Arrive_Side = sSource.Arrive_Side;
	this->Depart_Side = sSource.Depart_Side;
	this->in_sta_index = sSource.in_sta_index;
	this->out_sta_index = sSource.out_sta_index;
	this->change_trainid = sSource.change_trainid;
}

Passway_Sta& Passway_Sta::operator=(const Passway_Sta& sSource)
{
	if (this == &sSource)
	{
		return (*this);		
	}
	this->Arrive_Side.clear();
	this->Depart_Side.clear();
	this->Sta_id = sSource.Sta_id;
	this->Entry = sSource.Entry;
	this->Exit = sSource.Exit;
	this->Arrive_Side = sSource.Arrive_Side;
	this->Depart_Side = sSource.Depart_Side;
	this->in_sta_index = sSource.in_sta_index;
	this->out_sta_index = sSource.out_sta_index;
	this->change_trainid = sSource.change_trainid;

	return (*this);
}
Passway_Sta::~Passway_Sta()
{
	init();
}
void Passway_Sta:: init()
{
	Sta_id = -1;
	Entry  = -1;
	Exit = -1;
	Arrive_Side.clear();   
	Depart_Side.clear();    
	in_sta_index=0;
	out_sta_index=0;
	change_trainid=0;
}


CSingle_Passway::CSingle_Passway()
{
	init();
}
CSingle_Passway::CSingle_Passway(const CSingle_Passway& sSource)
{
	this->Con_id = sSource.Con_id;
	this->Passway_id = sSource.Passway_id;
	this->Passway_Name = sSource.Passway_Name;
	for (unsigned int i=0; i < sSource.m_vector_Passway_Sta_info.size();i++)
	{
		this->m_vector_Passway_Sta_info.push_back(sSource.m_vector_Passway_Sta_info[i]);
	} 
}

CSingle_Passway& CSingle_Passway::operator=(const CSingle_Passway& sSource)
{
	if (this == &sSource)
	{
		return (*this);		
	}
	this->m_vector_Passway_Sta_info.clear();
	this->Con_id = sSource.Con_id;
	this->Passway_id = sSource.Passway_id;
	this->Passway_Name = sSource.Passway_Name;
	for (unsigned int i=0; i < sSource.m_vector_Passway_Sta_info.size();i++)
	{
		this->m_vector_Passway_Sta_info.push_back(sSource.m_vector_Passway_Sta_info[i]);
	} 
	return (*this);
}
CSingle_Passway::~CSingle_Passway()
{
	init();
}
void CSingle_Passway:: init()
{
	Con_id = 0;
	Passway_id  = 0;
	//Passway_Name.empty();
	Passway_Name.clear();
	m_vector_Passway_Sta_info.clear();
}


CConsole_Passway::CConsole_Passway()
{
	init();
}
CConsole_Passway::CConsole_Passway(const CConsole_Passway& sSource)
{
	this->Con_id = sSource.Con_id;
	std::map<short, CSingle_Passway>::const_iterator  it = sSource.m_map_Console_Passway.begin();
	for (;it != sSource.m_map_Console_Passway.end(); it++)
	{
		this->m_map_Console_Passway[it->first] = it->second;
	}
}

CConsole_Passway&  CConsole_Passway:: operator=(const CConsole_Passway& sSource)
{
	if (this == (&sSource))
	{
		return (*this);		
	}
	this->m_map_Console_Passway.clear();
	this->Con_id = sSource.Con_id;
	std::map<short, CSingle_Passway>::const_iterator it = sSource.m_map_Console_Passway.begin();

	for( ; it != sSource.m_map_Console_Passway.end(); it++)
	{
		this->m_map_Console_Passway[it->first] = it->second;
	}
	return (*this);
}

CConsole_Passway::~CConsole_Passway()
{
	init();
}
void CConsole_Passway:: init()
{
	Con_id = 0;
	m_map_Console_Passway.clear();
}

CBureau_Passway::CBureau_Passway()
{
	init();
}
CBureau_Passway::CBureau_Passway(const CBureau_Passway& sSource)
{
	std::map<short, CConsole_Passway>::const_iterator  it = sSource.m_map_Bureau_Passway.begin();
	for (;it != sSource.m_map_Bureau_Passway.end(); it++)
	{
		this->m_map_Bureau_Passway[it->first] = it->second;
	}
}

CBureau_Passway&  CBureau_Passway:: operator=(const CBureau_Passway& sSource)
{
	if (this == (&sSource))
	{
		return (*this);		
	}
	this->m_map_Bureau_Passway.clear();
	std::map<short, CConsole_Passway>::const_iterator it = sSource.m_map_Bureau_Passway.begin();

	for( ; it != sSource.m_map_Bureau_Passway.end(); it++)
	{
		this->m_map_Bureau_Passway[it->first] = it->second;
	}
	return (*this);
}

CBureau_Passway::~CBureau_Passway()
{
	init();
}
void CBureau_Passway:: init()
{
	m_map_Bureau_Passway.clear();
}


CGetPasswayInfor::CGetPasswayInfor()
{
	::CoInitialize(0);
}
CGetPasswayInfor::~CGetPasswayInfor()
{

}
bool  CGetPasswayInfor::LoadPasswayXML(const string pathname)
{
	MSXML::IXMLDOMDocumentPtr pXMLDoc;
	HRESULT hr = pXMLDoc.CreateInstance(MSXML::CLSID_DOMDocument);
	if(FAILED(hr))
	{
		_com_error er(hr);
		AfxMessageBox(er.ErrorMessage());
		return false;
	}
	//char path[512];
	//path = pathname.data();
	if(VARIANT_TRUE != pXMLDoc->load( _bstr_t( pathname.data() ) ) )
	{
		CString  prompt;
		prompt.Format("配置passway.xml不存在或者不符合xml语法(可以用浏览器检查是否能打开)", pathname);
		AfxMessageBox(prompt);
		return false; 
	}
	MSXML::IXMLDOMNodePtr pRootNode,pChildNode,pNode_CONSOLE_PASSWAY;
	MSXML::IXMLDOMNodePtr pNode_PASSWAY,pNode_PASSWAY_ELE;

	pRootNode = pXMLDoc->selectSingleNode( "PASSWAYS");
	if(pRootNode == NULL)
	{
		return false;
	}
	// CONSOLE_PASSWAY
	MSXML::IXMLDOMNodeListPtr list = pRootNode->selectNodes( "CONSOLE_PASSWAY");
	CBureau_Passway tmp_Bureau_Passway;
	if(NULL != list)
	{
		for(int idx = 0; idx < list->Getlength(); idx++)
		{
			pNode_CONSOLE_PASSWAY = list->Getitem(idx);
			if(pNode_CONSOLE_PASSWAY)
			{
				CConsole_Passway tmp_Console_Passway;
				// @con_id
				pChildNode = pNode_CONSOLE_PASSWAY->selectSingleNode("@con_id");
				if(pChildNode)
				{
					_variant_t v;
					v = pChildNode->GetnodeTypedValue();
					tmp_Console_Passway.Con_id = (WORD)atoi(CString(v.bstrVal));	
					SysFreeString(v.bstrVal);
					v.Clear();
				}
				//get passway
				MSXML::IXMLDOMNodeListPtr list_PASSWAY = pNode_CONSOLE_PASSWAY->selectNodes( "PASSWAY");
				if (NULL != list_PASSWAY)
				{
					for (int idx_passway = 0; idx_passway < list_PASSWAY->Getlength();idx_passway++)
					{
						CSingle_Passway tmp_Single_Passway;	
						pNode_PASSWAY = list_PASSWAY->Getitem(idx_passway);
						if (pNode_PASSWAY)
						{
							//con_id
							tmp_Single_Passway.Con_id = tmp_Console_Passway.Con_id;
							//@id
							pChildNode = pNode_PASSWAY->selectSingleNode("@id");
							if(pChildNode)
							{
								_variant_t v;
								v = pChildNode->GetnodeTypedValue();
								tmp_Single_Passway.Passway_id= (WORD)atoi(CString(v.bstrVal));	
								SysFreeString(v.bstrVal);
								v.Clear();
							}
							//@name
							pChildNode = pNode_PASSWAY->selectSingleNode("@name");
							if(pChildNode)
							{
								_variant_t v;
								v = pChildNode->GetnodeTypedValue();										
								char   *p=_com_util::ConvertBSTRToString(v.bstrVal);   
								tmp_Single_Passway.Passway_Name = p;	
								delete  p;   											
								SysFreeString(v.bstrVal);
								v.Clear();
							}
							//get passway_ele里面的每条记录到tmp_Single_Passway的vector变量里面
							MSXML::IXMLDOMNodeListPtr list_PASSWAY_ELE = pNode_PASSWAY->selectNodes( "PASSWAY_ELE");
							if(NULL != list_PASSWAY_ELE)
							{
								for(int idx_passway_ele = 0; idx_passway_ele < list_PASSWAY_ELE->Getlength(); idx_passway_ele++)
								{
									pNode_PASSWAY_ELE = list_PASSWAY_ELE->Getitem(idx_passway_ele);
									if(pNode_PASSWAY_ELE)
									{
										Passway_Sta  tmp_Passway_Sta;		
										tmp_Passway_Sta.Arrive_Side = "";
										tmp_Passway_Sta.Depart_Side = "";
										tmp_Passway_Sta.Entry = -1;
										tmp_Passway_Sta.Exit = -1;
										tmp_Passway_Sta.Sta_id = -1;
										tmp_Passway_Sta.in_sta_index = 0;
										tmp_Passway_Sta.out_sta_index = 0;

										//@sta_id
										pChildNode = pNode_PASSWAY_ELE->selectSingleNode("@sta_id");
										_variant_t v;
										v = pChildNode->GetnodeTypedValue();
										tmp_Passway_Sta.Sta_id = (WORD)atoi(CString(v.bstrVal));	
										SysFreeString(v.bstrVal);
										v.Clear();
										//@entry="1" 
										pChildNode = pNode_PASSWAY_ELE->selectSingleNode("@entry");
										v = pChildNode->GetnodeTypedValue();
										tmp_Passway_Sta.Entry = (WORD)atoi(CString(v.bstrVal));	
										SysFreeString(v.bstrVal);
										v.Clear();
										//@arrive_side="1" 
										pChildNode = pNode_PASSWAY_ELE->selectSingleNode("@arrive_side");
										v = pChildNode->GetnodeTypedValue();
										char   *p=_com_util::ConvertBSTRToString(v.bstrVal);   
										tmp_Passway_Sta.Arrive_Side  = p;	
										delete  p;   	
										SysFreeString(v.bstrVal);
										v.Clear();
										//@depart_side="1" 
										pChildNode = pNode_PASSWAY_ELE->selectSingleNode("@depart_side");
										v = pChildNode->GetnodeTypedValue();
										p=_com_util::ConvertBSTRToString(v.bstrVal);   
										tmp_Passway_Sta.Depart_Side  = p;	
										delete  p; 
										SysFreeString(v.bstrVal);
										v.Clear();
										//@exit="3"
										pChildNode = pNode_PASSWAY_ELE->selectSingleNode("@exit");
										v = pChildNode->GetnodeTypedValue();
										tmp_Passway_Sta.Exit = (WORD)atoi(CString(v.bstrVal));	
										SysFreeString(v.bstrVal);
										v.Clear();

										//@in_sta_index
										pChildNode = pNode_PASSWAY_ELE->selectSingleNode("@in_sta_index");
										if(NULL != pChildNode)
										{
											v = pChildNode->GetnodeTypedValue();
											tmp_Passway_Sta.in_sta_index = (WORD)atoi(CString(v.bstrVal));	
											SysFreeString(v.bstrVal);
											v.Clear();
										}

										//@out_sta_index
										pChildNode = pNode_PASSWAY_ELE->selectSingleNode("@out_sta_index");
										if(NULL != pChildNode)
										{
											v = pChildNode->GetnodeTypedValue();
											tmp_Passway_Sta.out_sta_index = (WORD)atoi(CString(v.bstrVal));	
											SysFreeString(v.bstrVal);
											v.Clear();
										}
										//@change_trainid
										tmp_Passway_Sta.change_trainid=0;
										pChildNode = pNode_PASSWAY_ELE->selectSingleNode("@change_trainid");
										if(NULL != pChildNode)
										{
											v = pChildNode->GetnodeTypedValue();
											tmp_Passway_Sta.change_trainid = (WORD)atoi(CString(v.bstrVal));	
											SysFreeString(v.bstrVal);
											v.Clear();
										}

										//插入单条passway_ele到vector
										tmp_Single_Passway.m_vector_Passway_Sta_info.push_back(tmp_Passway_Sta);
									}

								}
							}
							//把读取的一条PASSWAY和其分支PASSWAY_ELE放到tmp_Console_Passway的map里
							tmp_Console_Passway.m_map_Console_Passway.insert(make_pair(tmp_Single_Passway.Passway_id,tmp_Single_Passway));
						}
					}
				}
				//把单个行调台的CONSOLE_PASSWAY的n条PASSWAY保存到tmp_Bureau_Passway
				tmp_Bureau_Passway.m_map_Bureau_Passway.insert(make_pair(tmp_Console_Passway.Con_id,tmp_Console_Passway));				
			}			
		}
	}	
	//文件读取完毕后，写入CGetPasswayInfor的成员变量m_Bureau_Passway中
	m_Bureau_Passway = tmp_Bureau_Passway;
	return true;
}


void CGetPasswayInfor::print()
{

}

bool CGetPasswayInfor::GetConsolePassway_Conid_To_CConsole_Passway(const short con_id,CConsole_Passway &console_passway)
{
	short id = con_id;
	std::map<short,CConsole_Passway>::const_iterator it = m_Bureau_Passway.m_map_Bureau_Passway.find(id);
	if (it != m_Bureau_Passway.m_map_Bureau_Passway.end())
	{
		console_passway = it->second;
		return true;
	}
	return false;
}

bool CGetPasswayInfor::GetConsolePassway(short Console_id,CConsole_Passway &Console_Passway)
{
	if (!GetConsolePassway_Conid_To_CConsole_Passway(Console_id,Console_Passway))
	{
		return false;
	}		
	return true;
}

bool CGetPasswayInfor::GetSinglePassway(short con_id,short  PasswayId,CSingle_Passway &Single_Passway)
{
	CConsole_Passway Console_Passway;
	if(!GetConsolePassway_Conid_To_CConsole_Passway(con_id,Console_Passway))
	{
		return false;
	}
	std::map<short,CSingle_Passway>::const_iterator it = Console_Passway.m_map_Console_Passway.find(PasswayId);
	if (it != Console_Passway.m_map_Console_Passway.end())
	{
		Single_Passway = it->second;
		return true;
	}
	return false;
}

/********************************************************************************************************************************
*功能：给一个行调台id号，返回经路数量,经路id，经路名称
*返回值：sucess：函数返回值：true；
传递参数：Passway_Quantity >0;Passway_id[0]和Passway_Name[0]对应一条经路;
fail：函数返回值：false；
传递参数：Passway_Quantity=0;
*作者：cf
*********************************************************************************************************************************/
bool CGetPasswayInfor::GetConsolePasswayArray(const short Console_id,short &Passway_Quantity, short Passway_id[], string Passway_Name[])
{
	CConsole_Passway Console_Passway;
	UINT index;
	index = 0;
	Passway_Quantity = 0;
	if (!GetConsolePassway_Conid_To_CConsole_Passway(Console_id,Console_Passway))
	{
		return false;
	}		
	Passway_Quantity = Console_Passway.m_map_Console_Passway.size();
	std::map<short,CSingle_Passway>::const_iterator it = Console_Passway.m_map_Console_Passway.begin();
	for (;it != Console_Passway.m_map_Console_Passway.end();++it)
	{
		Passway_id[index]= it->second.Passway_id;
		Passway_Name[index] = it->second.Passway_Name;
		index ++;		
	}
	return true;
}

/********************************************************************************************************************************
*功能：给一个行调台号（con_id），经路id号（PasswayId），返回经路名称和该经路经过的
所有车站信息
*返回值：sucess：函数返回值：true；
传递参数:所经车站数量：Station_Quantity,车站号：sta_id[], 
车站入口： entry[],到达股道： arrive_side[],
出发股道：depart_side[], 车站出口： exit[]
fail：函数返回值：false；
传递参数：Passway_Quantity=0;
*作者：cf
*********************************************************************************************************************************/
bool CGetPasswayInfor::GetSinglePasswayContent_From_PasswayId(const short  PasswayId, const short con_id, string &PasswayName,
															  short &Station_Quantity,short sta_id[], short entry[],string arrive_side[],
															  string  depart_side[], short exit[])
{
	CConsole_Passway Console_Passway;
	PasswayName = "";
	Station_Quantity = 0;
	if(!GetConsolePassway_Conid_To_CConsole_Passway(con_id,Console_Passway))
	{
		return false;
	}
	std::map<short,CSingle_Passway>::const_iterator it = Console_Passway.m_map_Console_Passway.find(PasswayId);
	if (it != Console_Passway.m_map_Console_Passway.end())
	{
		CSingle_Passway Single_Passway;
		Single_Passway = it->second;
		PasswayName = Single_Passway.Passway_Name;
		Station_Quantity = Single_Passway.m_vector_Passway_Sta_info.size();
		for(short i = 0; i< Station_Quantity;i++ )
		{
			Passway_Sta p_sta;
			p_sta = Single_Passway.m_vector_Passway_Sta_info[i];
			sta_id[i] = p_sta.Sta_id;
			entry[i] = p_sta.Entry;
			arrive_side[i] = p_sta.Arrive_Side;
			depart_side[i] = p_sta.Depart_Side;
			exit[i] = p_sta.Exit;			
		}
		return true;	//在std::map<short,CSingle_Passway>找到了该passway_id对应的passway内容
	}
	else
	{
		return false;//在std::map<short,CSingle_Passway>没找到该passway_id对应的passway内容
	}

}

bool CGetPasswayInfor::getDataDir(char * const strPath)
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