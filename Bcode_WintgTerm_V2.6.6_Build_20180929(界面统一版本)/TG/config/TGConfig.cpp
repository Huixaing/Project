#include "StdAfx.h"
#include "TGCtrl.h"
#include ".\tgconfig.h"
#import  <msxml.dll> named_guids
using namespace MSXML;
	
COLORREF SIDE_CLR::upSideClr=CLR_LAWNGREEN;
COLORREF SIDE_CLR::downSideClr=CLR_LAWNGREEN;
int SIDE_CLR::prior_up=1;
int SIDE_CLR::prior_down=1;

CTgConfig::CTgConfig(void)
{
	m_bUseBitmap = true;
	bAllStationActive = false;
	bBasicCanMoveWholeTrain = false;
	m_bShowRuntimeWnd=false;
	m_bCanMutiMoveOrDel=false;
	
	m_bDisenableSheetTab=false;
	m_bDisableModifyPlanTm=true;
	m_bFastOrSlowTimeWithBasic=false;
	m_bEraseBeforeModifyTrain=false;
	m_nNotMeetInfoCount=0;
	
	station.clear();
	up_color=0;
	down_color=0;
	mEntryBySide.count=0;
	memset(&mEntryBySide, 0, sizeof(mEntryBySide));
}

CTgConfig::~CTgConfig(void)
{
	station.clear();
}

void CTgConfig::SetCurrentSectionID(short conid)
{
	nCurrentSection = conid;
}

short CTgConfig::GetCurrentSectionID()
{
	return nCurrentSection;
}

int CTgConfig::GetTrainTypeByTrainNO(CString trainId)
{
	return m_trainType.GetTrainTypeByTrainNO(trainId);
}

// 设置车站信息
//<STATION_SET>
//  <STATION id="1" name="罗岗线路所" klio_mark="451.679" arrive="1" depart="1" property="1"/>
//</STATION_SET>
bool  CTgConfig::SetStationInfo(CString xml,COleControl* ctrl)
{
	CTGCtrl* pCtrl = (CTGCtrl*)ctrl;
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
		pCtrl->FIRE_TG_LOG("TG SetStationInfo loadXML fail!");
		return false; 
	}
	
	MSXML::IXMLDOMNodePtr pNode,pRootNode;

	pRootNode = pXMLDoc->selectSingleNode( "STATION_SET");
	if(pRootNode == NULL)
	{
		pCtrl->FIRE_TG_LOG("TG SetStationInfo can't find <STATION_SET> in xml!");
		return false;
	}

	station.clear();
	MSXML::IXMLDOMNodeListPtr stationlist = pRootNode->selectNodes( "STATION");
	if(NULL != stationlist)
	{
		for(int idx = 0; idx < stationlist->Getlength(); idx++)
		{
			TGStation sta;
			
			_variant_t v;
			// id
			pNode = stationlist->Getitem(idx)->selectSingleNode("@id");
			if(pNode)
			{
				v = pNode->GetnodeTypedValue();
				sta.id = atoi(CString(v.bstrVal));
				SysFreeString(v.bstrVal);
				v.Clear();
			}
			// name
			pNode = stationlist->Getitem(idx)->selectSingleNode("@name");
			if(pNode)
			{
				v = pNode->GetnodeTypedValue();
				CString name(v.bstrVal);
				if(name.GetLength() >= 64)
				{
					strncpy(sta.name, name.GetBuffer(), 63);
					sta.name[63] = 0;
				}
				else
				{
					strncpy(sta.name, name.GetBuffer(), name.GetLength());
					sta.name[name.GetLength()] = 0;
				}
				SysFreeString(v.bstrVal);
				v.Clear();
			}
	
			// klio_mark
			pNode = stationlist->Getitem(idx)->selectSingleNode("@klio_mark");
			if(pNode)
			{
				v = pNode->GetnodeTypedValue();
				CString kmmark(v.bstrVal);
				if(kmmark.GetLength() >= 64)
				{
					strncpy(sta.fKmMark, kmmark.GetBuffer(), 63);
					sta.fKmMark[63] = 0;
				}
				else
				{
					strncpy(sta.fKmMark, kmmark.GetBuffer(), kmmark.GetLength());
					sta.fKmMark[kmmark.GetLength()] = 0;
				}
				SysFreeString(v.bstrVal);
				v.Clear();
			}
			
			// ad_time
			pNode = stationlist->Getitem(idx)->selectSingleNode("@ad_time");
			if(pNode)
			{
				v = pNode->GetnodeTypedValue();
				int tmp = atoi(CString(v.bstrVal));
				sta.adtime = (tmp == 0) ? false:true;
				SysFreeString(v.bstrVal);
				v.Clear();
			}

			// staclass
			pNode = stationlist->Getitem(idx)->selectSingleNode("@staclass");
			if(pNode)
			{
				v = pNode->GetnodeTypedValue();
				sta.staclass = atoi(CString(v.bstrVal));
				SysFreeString(v.bstrVal);
				v.Clear();
			}

			// property
			pNode = stationlist->Getitem(idx)->selectSingleNode("@property");
			if(pNode)
			{
				v = pNode->GetnodeTypedValue();
				sta.property = atoi(CString(v.bstrVal));
				if(sta.property>0)
					sta.property=1;
				SysFreeString(v.bstrVal);
				v.Clear();
			}
			// 加入向量
			station.insert(make_pair(sta.id, sta));
		}
	}
	else
	{
		pCtrl->FIRE_TG_LOG("TG SetStationInfo can't find <SECTION> in xml!");
		return false;
	}

	return true;
}

const TGStation* CTgConfig::GetStation(int stano)
{
	std::map<int,TGStation>::const_iterator it = this->station.find(stano);
	if(it != this->station.end())
	{
		return &(it->second);
	}

	return NULL;
}

bool  CTgConfig::SetSectionInfo(CString xml,COleControl* ctrl)
{
	CTGCtrl* pCtrl = (CTGCtrl*)ctrl;
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
		pCtrl->FIRE_TG_LOG("TG SetSectionInfo loadXML fail!");
		return false; 
	}
	
	MSXML::IXMLDOMNodePtr pNode,pRootNode;

	pRootNode = pXMLDoc->selectSingleNode( "SECTION_SET");
	if(pRootNode == NULL)
	{
		pCtrl->FIRE_TG_LOG("TG SetSectionInfo can't find <SECTION_SET> in xml!");
		return false;
	}

	sections.clear();
	MSXML::IXMLDOMNodeListPtr stationlist = pRootNode->selectNodes( "SECTION");
	if(NULL != stationlist)
	{
		for(int idx = 0; idx < stationlist->Getlength(); idx++)
		{
			TGSection sta;
			
			_variant_t v;
			// id
			pNode = stationlist->Getitem(idx)->selectSingleNode("@id");
			if(pNode)
			{
				v = pNode->GetnodeTypedValue();
				sta.id = atoi(CString(v.bstrVal));
				SysFreeString(v.bstrVal);
				v.Clear();
			}
			// name
			pNode = stationlist->Getitem(idx)->selectSingleNode("@name");
			if(pNode)
			{
				v = pNode->GetnodeTypedValue();
				sta.name=CString(v.bstrVal);
				SysFreeString(v.bstrVal);
				v.Clear();
			}
			// 加入向量
			std::map<int, TGSection>::iterator it = sections.find(sta.id);
			if(it == sections.end())
				sections.insert(make_pair(sta.id, sta));
		}
	}

	return true;
}

CString CTgConfig::GetSectionName(int id)
{
	std::map<int, TGSection>::iterator it = sections.find(id);
	if(it != sections.end())
	{
		return it->second.name;
	}

	return "未知台";
}

void CTgConfig::GetStationList(int nSection, int& stationNum, int* stationList)
{
	ASSERT(stationList != NULL);
	this->m_tgdef.GetStationList(nSection, stationNum, stationList);
}

// 加载控件配置
bool CTgConfig::LoadTGFile(COleControl* ctrl)
{
	CTGCtrl* ptrl = (CTGCtrl*)ctrl;
	CString path = m_context.GetDataPath();

	int theStatus = 1;
	do 
	{
		CString FileName = path + "\\tg.def";
		if(!this->m_tgdef.LoadTGProfile(FileName))
		{
			CString  prompt;
			prompt.Format("配置文件%s 第 %d 行有错", FileName, this->m_tgdef.GetErrLineNo());
			ptrl->FIRE_TG_LOG(prompt);
			AfxMessageBox(prompt);
			break;
		}

		FileName = path + "\\entry.cfg";
		if(!m_entrycfg.Loadentry(FileName, ctrl))
			break;

		FileName = path + "\\tg\\LINE_STYLE.xml";
		this->m_SingleTrainCfg.LoadTgLineStyleXML(FileName, ctrl);

		FileName = path + "\\tg\\LINE_COLOR.xml";
		this->m_SingleTrainCfg.LoadTgLineColorXML(FileName, ctrl);

		FileName = path + "\\tg\\LINE_TRAINID.xml";
		this->m_SingleTrainCfg.LoadTgTrainIDXML(FileName, ctrl);
        
		FileName = path + "\\tg\\AbsLineDescription.xml";
		this->m_SingleTrainCfg.LoadAbsLineDespFile(FileName, ctrl);

		FileName = path + "\\tg\\MARK_STYLE.xml";
		this->LoadMarkCfgXML(FileName, ctrl);

		FileName = path + "\\tg\\SIDE_COLOR.xml";
		this->LoadSideColorXML(FileName);

		//FileName = path + "\\locotype.xml";
		this->m_locoType.LoadLocolTypeXml(path); 

		FileName = path + "\\TGNO.xml";
		this->m_context.LoadTGNoXML(FileName);

		FileName = path + "\\TRAIN_TYPE.xml";
		this->m_trainType.LoadTrain_TypeXML(FileName);

		FileName = path + "\\train_sta_pwd.cfg";
		UINT cnt = this->load_train_sta_pwd(FileName);

		FileName = path + "\\EntryBySide.xml";
		LoadEntryBySide(FileName);
		theStatus = 0;

	}while(0);

	
	if(theStatus)
	{
		ptrl->FIRE_TG_LOG("[TG]: fail LoadTGCfg End");
		return false;
	}
	else
	{
		return true;
	}
}

void InitBlockCfg(BlockCfg& blockcfg, MSXML::IXMLDOMNodePtr pNode)
{
	if(NULL == pNode)
		return;
	MSXML::IXMLDOMNodePtr pChildNode;
	// @draw
	pChildNode = pNode->selectSingleNode("@draw");
	if(pChildNode)
	{
		_variant_t v;
		v = pChildNode->GetnodeTypedValue();
		blockcfg.draw = atoi(CString(v.bstrVal));			
		SysFreeString(v.bstrVal);
		v.Clear();
	}

	// @time
	pChildNode = pNode->selectSingleNode("@time");
	if(pChildNode)
	{
		_variant_t v;
		v = pChildNode->GetnodeTypedValue();
		blockcfg.time_pos = atoi(CString(v.bstrVal));	
		SysFreeString(v.bstrVal);
		v.Clear();
	}

	// @color
	pChildNode = pNode->selectSingleNode("@color");
	if(pChildNode)
	{
		_variant_t v;
		v = pChildNode->GetnodeTypedValue();

		CString rgb(v.bstrVal);
		int R,G,B;
		sscanf(rgb.GetBuffer(), "%d-%d-%d", &R,&G,&B);
		blockcfg.color1 = RGB(R,G,B);	

		SysFreeString(v.bstrVal);
		v.Clear();
	}

	// @color1
	pChildNode = pNode->selectSingleNode("@color1");
	if(pChildNode)
	{
		_variant_t v;
		v = pChildNode->GetnodeTypedValue();

		CString rgb(v.bstrVal);
		int R,G,B;
		sscanf(rgb.GetBuffer(), "%d-%d-%d", &R,&G,&B);
		blockcfg.color2 = RGB(R,G,B);	

		SysFreeString(v.bstrVal);
		v.Clear();
	}

	// @color2
	pChildNode = pNode->selectSingleNode("@color2");
	if(pChildNode)
	{
		_variant_t v;
		v = pChildNode->GetnodeTypedValue();

		CString rgb(v.bstrVal);
		int R,G,B;
		sscanf(rgb.GetBuffer(), "%d-%d-%d", &R,&G,&B);
		blockcfg.color3 = RGB(R,G,B);	

		SysFreeString(v.bstrVal);
		v.Clear();
	}
	else
	{
		blockcfg.color3 = blockcfg.color1;
	}

	// @off_color
	pChildNode = pNode->selectSingleNode("@off_color");
	if(pChildNode)
	{
		_variant_t v;
		v = pChildNode->GetnodeTypedValue();

		CString rgb(v.bstrVal);
		int R,G,B;
		sscanf(rgb.GetBuffer(), "%d-%d-%d", &R,&G,&B);
		blockcfg.off_color = RGB(R,G,B);	

		SysFreeString(v.bstrVal);
		v.Clear();
	}
	else
	{
		blockcfg.off_color = blockcfg.color1;
	}

	// @dlt
	pChildNode = pNode->selectSingleNode("@dlt");
	if(pChildNode)
	{
		_variant_t v;
		v = pChildNode->GetnodeTypedValue();
		blockcfg.dlt = atoi(CString(v.bstrVal));	
		SysFreeString(v.bstrVal);
		v.Clear();
	}
}

void CTgConfig::LoadMarkCfgXML(CString pathname, COleControl* ctrl)
{
	CTGCtrl* pCtrl = (CTGCtrl*)ctrl;

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
		pCtrl->FIRE_TG_LOG(prompt);
		return; 
	}
	
	MSXML::IXMLDOMNodePtr pChildNode,pNode,pRootNode;

	pRootNode = pXMLDoc->selectSingleNode( "MARK_GRAPH");
	if(pRootNode == NULL)
		return;

	up_color=0, down_color=0;
	pNode = pRootNode->selectSingleNode( "COLOR");
	if(pNode)
	{
		pChildNode = pNode->selectSingleNode("@upcolor");
		if(pChildNode)
		{
			_variant_t v;
			v = pChildNode->GetnodeTypedValue();

			CString rgb(v.bstrVal);
			int R,G,B;
			sscanf(rgb.GetBuffer(), "%d-%d-%d", &R,&G,&B);
			up_color = RGB(R,G,B);	

			SysFreeString(v.bstrVal);
			v.Clear();
		}
		pChildNode = pNode->selectSingleNode("@downcolor");
		if(pChildNode)
		{
			_variant_t v;
			v = pChildNode->GetnodeTypedValue();

			CString rgb(v.bstrVal);
			int R,G,B;
			sscanf(rgb.GetBuffer(), "%d-%d-%d", &R,&G,&B);
			down_color = RGB(R,G,B);	

			SysFreeString(v.bstrVal);
			v.Clear();
		}
	}
	


	_variant_t v;
	
	// -----------------------区间---------------------------
	// 区间封锁
	pNode = pRootNode->selectSingleNode( "ABS_INHIBIT_MARK");
	InitBlockCfg(m_AbsInhibitMarkCfg, pNode);
	// 区间慢行
	pNode = pRootNode->selectSingleNode( "ABS_SLOW_MARK");
	InitBlockCfg(m_AbsSlowMarkCfg, pNode);
	// 电网检修
	pNode = pRootNode->selectSingleNode( "ABS_ELE_FIX_MARK");
	InitBlockCfg(m_AbsEleFixMarkCfg, pNode);
	// 电网检修
	pNode = pRootNode->selectSingleNode( "ABS_ZH_FIX_MARK");
	InitBlockCfg(m_AbsZhFixMarkCfg, pNode);
	
	// 区间变更闭塞方式
	pNode = pRootNode->selectSingleNode( "SELF_CLOSE_MARK");
	InitBlockCfg(m_SelfCloseMarkCfg, pNode);

	// -----------------------站内---------------------------
	// 站内股道封锁
	pNode = pRootNode->selectSingleNode( "STATION_SIDE_INHIBIT");
	InitBlockCfg(m_SideInhibitMarkCfg, pNode);
	// 站内股道慢行
	pNode = pRootNode->selectSingleNode( "STATION_SIDE_SLOW");
	InitBlockCfg(m_SideSlowMarkCfg, pNode);
	// 站内股道占用
	pNode = pRootNode->selectSingleNode( "STATION_SIDE_TIANCHUANG");
	InitBlockCfg(m_SideTianchuangMarkCfg, pNode);
	// 站内股道天窗
	pNode = pRootNode->selectSingleNode( "STATION_SIDE_OCCUPY");
	InitBlockCfg(m_SideOccupyMarkCfg, pNode);
}

TGStation::TGStation()
{
	this->adtime = false;
	this->id = 0;
	this->property = 0;
	memset(this->name,0, 64);
	memset(this->fKmMark,0, 64);
}
TGStation::TGStation(const TGStation& rhs)
{
	this->adtime = rhs.adtime;
	this->id = rhs.id;
	this->property = rhs.property;
	strcpy(this->name,rhs.name);
	strcpy(this->fKmMark,rhs.fKmMark);
}

// train_sta_pwd.cfg
UINT CTgConfig::load_train_sta_pwd(CString filename)
{
	FILE *fp = fopen(filename, "r");
	if (NULL == fp)
		return 0;

	char *ptr, strLine[512];
	while (!feof(fp))
	{
		ptr = fgets(strLine, sizeof(strLine), fp);
		if (ptr == NULL) 
			continue;

		while (*ptr == ' ' || *ptr == '\t')
			ptr ++;
		if (*ptr == ';')	
			continue;
		if (*ptr == '\n')	
			continue;

		char *token;
		char seps[]=" ,\t\\/=\n";
		TrainId_Sta_Pwd data;
		memset(&data, 0x00, sizeof(data));

		token = strtok(strLine, seps);
		if(token==NULL) continue;
		strncpy(data.train_id, token, sizeof(data.train_id)-1);
		data.train_id[sizeof(data.train_id)-1]=0;
		
		token = strtok(NULL, seps);
		if(token==NULL) continue;
		data.station = atol(token);

		token = strtok(NULL, seps);
		if(token==NULL) continue;
		strncpy(data.pwd, token, sizeof(data.pwd)-1);
		data.pwd[sizeof(data.pwd)-1]=0;

		train_sta_pwd.push_back(data);
	}
	fclose(fp);
	return train_sta_pwd.size();
}

CString CTgConfig::get_train_sta_pwd(const CStringArray& trainid, short sta)
{
	if(trainid.GetCount() <= 0)
		return "1234";

	for(vector<TrainId_Sta_Pwd>::iterator iter = train_sta_pwd.begin(); 
		iter < train_sta_pwd.end();
		iter ++)
	{
		if((*iter).station != sta) continue;

		int i=0;
		for (i=0; i<trainid.GetCount(); i++)
		{
			if(trainid.GetAt(0) == (*iter).train_id)
				break;
		}
		
		if (i>=trainid.GetCount())
			continue;

		return (*iter).pwd;
	}

	return "1234";
}

bool CTgConfig::isPlatform(int stano, int sideno)
{
	LINE_INFO side;
	if(!m_lineCfg.GetStaLineCfg(stano, side))
		return false;

	for(int i=0; i<side.vecSideInfo.size(); i++)
	{
		if(side.vecSideInfo[i].sideno == sideno)
			return (side.vecSideInfo[i].nPlatform>0);
	}

	return false;
}
	
bool CTgConfig::isHighPlatform(int stano, int sideno)
{
	LINE_INFO side;
	if(!m_lineCfg.GetStaLineCfg(stano, side))
		return false;

	for(int i=0; i<side.vecSideInfo.size(); i++)
	{
		if(side.vecSideInfo[i].sideno == sideno)
			return (side.vecSideInfo[i].nPlatform>1);
	}

	return false;
}

bool CTgConfig::LoadSideColorXML(CString fileName)
{
	m_sideClr.clear();

	SIDE_CLR side;
	side.type=1;
	side.prior=2;
	side.clr=CLR_BLUE;
	
	MSXML::IXMLDOMDocumentPtr pXMLDoc;
	HRESULT hr = pXMLDoc.CreateInstance(MSXML::CLSID_DOMDocument);
	if(FAILED(hr))
	{
		m_sideClr.push_back(side);
		return false;
	}
	
	if(VARIANT_TRUE != pXMLDoc->load( _bstr_t( fileName ) ) )
	{	
		m_sideClr.push_back(side);
		return false; 
	}
	
	MSXML::IXMLDOMNodePtr pNode,pRootNode,pChildNode;

	pRootNode = pXMLDoc->selectSingleNode( "SIDE");
	if(pRootNode == NULL)
	{
		m_sideClr.push_back(side);
		return false;
	}

	pNode = pRootNode->selectSingleNode("DIRECT/UP_COLOR");
	if(pNode)
	{
		// prior
		pChildNode = pNode->selectSingleNode("@prior");
		if(pChildNode)
		{
			_variant_t v;
			v = pChildNode->GetnodeTypedValue();
			SIDE_CLR::prior_up = atoi(CString(v.bstrVal));
			SysFreeString(v.bstrVal);
			v.Clear();
		}

		// color
		pChildNode = pNode->selectSingleNode("@color");
		if(pChildNode)
		{
			_variant_t v;
			v = pChildNode->GetnodeTypedValue();
			CString rgb(v.bstrVal);
			int R,G,B;
			sscanf(rgb.GetBuffer(), "%d-%d-%d", &R,&G,&B);
	
			SIDE_CLR::upSideClr = RGB(R,G,B);		
			SysFreeString(v.bstrVal);
			v.Clear();
		}
	}

	pNode = pRootNode->selectSingleNode("DIRECT/DOWN_COLOR");
	if(pNode)
	{
		// prior
		pChildNode = pNode->selectSingleNode("@prior");
		if(pChildNode)
		{
			_variant_t v;
			v = pChildNode->GetnodeTypedValue();
			SIDE_CLR::prior_down = atoi(CString(v.bstrVal));
			SysFreeString(v.bstrVal);
			v.Clear();
		}

		// color
		pChildNode = pNode->selectSingleNode("@color");
		if(pChildNode)
		{
			_variant_t v;
			v = pChildNode->GetnodeTypedValue();
			CString rgb(v.bstrVal);
			int R,G,B;
			sscanf(rgb.GetBuffer(), "%d-%d-%d", &R,&G,&B);
	
			SIDE_CLR::downSideClr = RGB(R,G,B);		
			SysFreeString(v.bstrVal);
			v.Clear();
		}
	}

	MSXML::IXMLDOMNodeListPtr colorlist = pRootNode->selectNodes( "TYPE/COLOR");
	if(NULL != colorlist)
	{
		for(int idx = 0; idx < colorlist->Getlength(); idx++)
		{
			SIDE_CLR sideClr;
			
			_variant_t v;
			// type
			sideClr.type=-1;
			pNode = colorlist->Getitem(idx)->selectSingleNode("@type");
			if(pNode)
			{
				v = pNode->GetnodeTypedValue();
				sideClr.type = atoi(CString(v.bstrVal));
				SysFreeString(v.bstrVal);
				v.Clear();
			}
			// prior
			sideClr.prior=-1;
			pNode = colorlist->Getitem(idx)->selectSingleNode("@prior");
			if(pNode)
			{
				v = pNode->GetnodeTypedValue();
				sideClr.prior = atoi(CString(v.bstrVal));
				SysFreeString(v.bstrVal);
				v.Clear();
			}
			// color
			sideClr.clr=255;
			pNode = colorlist->Getitem(idx)->selectSingleNode("@color");
			if(pNode)
			{
				_variant_t v;
				v = pNode->GetnodeTypedValue();
				CString rgb(v.bstrVal);
				int R,G,B;
				sscanf(rgb.GetBuffer(), "%d-%d-%d", &R,&G,&B);
		
				sideClr.clr = RGB(R,G,B);		
				SysFreeString(v.bstrVal);
				v.Clear();
			
				if(sideClr.type>0 && sideClr.prior>0)
				{
					m_sideClr.push_back(sideClr);
				}
			}
		}
	}

	return false;
}

COLORREF CTgConfig::GetSideColor(int nProperty, int nDirect, COLORREF lColor)
{
	for(int i=0; i<m_sideClr.size(); i++)
	{
		if(m_sideClr[i].type == nProperty)
		{
			if(nDirect == 1) // 上
			{
				if(m_sideClr[i].prior>SIDE_CLR::prior_up)
					return m_sideClr[i].clr;
				else
					return SIDE_CLR::upSideClr;
			}
			else if(nDirect == 2) // 下
			{
				if(m_sideClr[i].prior>SIDE_CLR::prior_down)
					return m_sideClr[i].clr;
				else
					return SIDE_CLR::downSideClr;
			}

			return m_sideClr[i].clr;
		}
	}

	return lColor;
}

void CTgConfig::AddNotThisSectionStation(int station)
{
	UINT i=0;
	for(i=0; i<m_notThisSectionStation.size(); i++)
	{
		if(m_notThisSectionStation[i]==station)
			break;
	}

	if(i>=m_notThisSectionStation.size())
		m_notThisSectionStation.push_back(station);
}

bool CTgConfig::IsNotThisSectionStation(int station)
{
	for(UINT i=0; i<m_notThisSectionStation.size(); i++)
	{
		if(m_notThisSectionStation[i]==station)
			return true;
	}
	
	return false;
}

// 加载不受封闭运行图限制的车站列表 no_limit_station.xml
void CTgConfig::AddNoLimitStation(int station)
{
	UINT i=0;
	for(i=0; i<m_noLimitStation.size(); i++)
	{
		if(m_noLimitStation[i]==station)
			break;
	}

	if(i>=m_noLimitStation.size())
		m_noLimitStation.push_back(station);
	
}

bool CTgConfig::IsNoLimitStation(int station)
{
	for (int i=0; i<m_noLimitStation.size();i++)
	{
		if (m_noLimitStation[i] == station)
			return true;
	}

	return false;
}

void CTgConfig::LoadEntryBySide(CString strFileName)
{
	mEntryBySide.count=0;
	memset(&mEntryBySide, 0, sizeof(mEntryBySide));

	MSXML::IXMLDOMDocumentPtr pXMLDoc;
	HRESULT hr = pXMLDoc.CreateInstance(MSXML::CLSID_DOMDocument);
	if(FAILED(hr))
	{
		_com_error er(hr);
		AfxMessageBox(er.ErrorMessage());
		return;
	}
	
	if(VARIANT_TRUE != pXMLDoc->load( _bstr_t( strFileName ) ) )
	{
		return; 
	}
	
	MSXML::IXMLDOMNodePtr pChildNode,pNode,pRootNode;

	pRootNode = pXMLDoc->selectSingleNode( "BUREAU");
	if(pRootNode == NULL)
		return;

	// CON_ID
	MSXML::IXMLDOMNodeListPtr list = pRootNode->selectNodes( "CON_ID");
	if(NULL == list)
		return;

	int sectionid=GetCurrentSectionID();
	for(int idx = 0; idx < list->Getlength(); idx++)
	{
		pNode = list->Getitem(idx);
		if(NULL == pNode)
			continue;

		// @id
		int con_id=0;
		pChildNode = pNode->selectSingleNode("@id");
		if(pChildNode)
		{
			_variant_t v;
			v = pChildNode->GetnodeTypedValue();
			con_id = atoi(CString(v.bstrVal));	
			SysFreeString(v.bstrVal);
			v.Clear();
		}
		if(sectionid!=con_id)
			continue;
		
		MSXML::IXMLDOMNodeListPtr liststa = pNode->selectNodes( "STATION");
		for(int idx = 0; idx < liststa->Getlength(); idx++)
		{
			pNode = liststa->Getitem(idx);
			int id=0, nextid=0;
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
			pChildNode = pNode->selectSingleNode("@next_id");
			if(pChildNode)
			{
				_variant_t v;
				v = pChildNode->GetnodeTypedValue();
				nextid = atoi(CString(v.bstrVal));	
				SysFreeString(v.bstrVal);
				v.Clear();
			}
			if(id==0 || nextid==0)
				continue;
				
			// 出入口个数为0
			MSXML::IXMLDOMNodeListPtr listexit = pNode->selectNodes( "ENTRY_SIDE");
			for(int idx = 0; idx < listexit->Getlength(); idx++)
			{
				int exit=0, entry=0;
				char text[64];
				strcpy(text, "未知区间");
				pNode = listexit->Getitem(idx);
				// @exit
				pChildNode = pNode->selectSingleNode("@exit");
				if(pChildNode)
				{
					_variant_t v;
					v = pChildNode->GetnodeTypedValue();
					exit = atoi(CString(v.bstrVal));	
					SysFreeString(v.bstrVal);
					v.Clear();
				}
				// @entry
				pChildNode = pNode->selectSingleNode("@entry");
				if(pChildNode)
				{
					_variant_t v;
					v = pChildNode->GetnodeTypedValue();
					entry = atoi(CString(v.bstrVal));	
					SysFreeString(v.bstrVal);
					v.Clear();
				}
				// @abs_text
				pChildNode = pNode->selectSingleNode("@abs_text");
				if(pChildNode)
				{
					_variant_t v;
					v = pChildNode->GetnodeTypedValue();
					CString tmp(v.bstrVal);
					strncpy(text, tmp, sizeof(text)-1);
					text[sizeof(text)-1]=0;
					SysFreeString(v.bstrVal);
					v.Clear();
				}
				
				if(exit==0 || entry==0)
					continue;
				
				EntryBySide ss;
				ss.station1=id;
				ss.station2=nextid;
				ss.exit=exit;
				ss.entry=entry;
				strncpy(ss.abs_text, text, sizeof(ss.abs_text)-1);
				ss.abs_text[sizeof(ss.abs_text)-1]=0;
				ss.side_num=0;
				for(int a=0; a<64; a++)
					ss.side[a]=0;
				MSXML::IXMLDOMNodeListPtr listside = pNode->selectNodes( "SIDE");
				for(int idx = 0; idx < listside->Getlength(); idx++)
				{
					int side=0;
					pNode = listside->Getitem(idx);
					// @id
					pChildNode = pNode->selectSingleNode("@id");
					if(pChildNode)
					{
						_variant_t v;
						v = pChildNode->GetnodeTypedValue();
						side = atoi(CString(v.bstrVal));	
						SysFreeString(v.bstrVal);
						v.Clear();
					}
					if(side>0)
					{
						ss.side[ss.side_num]=side;
						ss.side_num++;
					}
				}
			
				if(ss.side_num>0)
				{
					mEntryBySide.info[mEntryBySide.count]=ss;
					mEntryBySide.count++;
				}
			}
		}

		break;
	}
	for(int i=0; i<mEntryBySide.count; i++)
	{
		m_SingleTrainCfg.AddAbsLineDesp(sectionid, mEntryBySide.info[i].station1, mEntryBySide.info[i].exit, mEntryBySide.info[i].abs_text);
		m_SingleTrainCfg.AddAbsLineDesp(sectionid, mEntryBySide.info[i].station2, mEntryBySide.info[i].entry, mEntryBySide.info[i].abs_text);
	}
}

bool CTgConfig::IsThisStationExitBySide(int station1, int station2, int side, int& exit, int& entry, CString& str)
{
	exit=0;
	entry=0;
	str="";
	if(mEntryBySide.count<=0)
		return false;

	for(int n=0; n<mEntryBySide.count; n++)
	{
		if(mEntryBySide.info[n].station1==station1 && mEntryBySide.info[n].station2==station2)
		{
			for(int k=0; k<mEntryBySide.info[n].side_num; k++)
			{
				if(mEntryBySide.info[n].side[k]==side)
				{
					exit=mEntryBySide.info[n].exit;
					entry=mEntryBySide.info[n].entry;
					str=mEntryBySide.info[n].abs_text;
					return true;
				}
			}
		}
	}

	return false;
}

bool CTgConfig::IsThisStationEntryBySide(int station1, int station2, int side, int& exit, int& entry, CString& str)
{
	exit=0;
	entry=0;
	str="";
	if(mEntryBySide.count<=0)
		return false;

	for(int n=0; n<mEntryBySide.count; n++)
	{
		if(mEntryBySide.info[n].station1==station2 && mEntryBySide.info[n].station2==station1)
		{
			for(int k=0; k<mEntryBySide.info[n].side_num; k++)
			{
				if(mEntryBySide.info[n].side[k]==side)
				{
					exit=mEntryBySide.info[n].entry;
					entry=mEntryBySide.info[n].exit;
					str=mEntryBySide.info[n].abs_text;
					return true;
				}
			}
		}
	}

	return false;
}

void CTgConfig::AddNotMeetInfo(WORD nNotMeetFlag, CString name, CString short_name)
{
	if(m_nNotMeetInfoCount<8)
	{
		mNotMeetInfo[m_nNotMeetInfoCount].nNoMeetFlag=nNotMeetFlag;
		mNotMeetInfo[m_nNotMeetInfoCount].sName=name;
		mNotMeetInfo[m_nNotMeetInfoCount].sShortName=short_name;
		m_nNotMeetInfoCount++;
	}
}

bool CTgConfig::getNotMeetInfoByNo(BYTE idx, NotMeetInfo& info) const
{
	bool bFind=false;
	if(idx<m_nNotMeetInfoCount)
	{
		info=mNotMeetInfo[idx];
		bFind=true;
	}

	return bFind;
}

CString CTgConfig::getNotMeetName(WORD wdFlag) const
{
	CString str="未知";
	for(BYTE i=0; i<m_nNotMeetInfoCount&&i<8; i++)
	{
		if(mNotMeetInfo[i].nNoMeetFlag==wdFlag)
		{
			str=mNotMeetInfo[i].sName;
			break;
		}
	}
	return str;
}

CString CTgConfig::getNotMeetDisplayName(WORD wdFlag) const
{
	CString str="未知";
	for(BYTE i=0; i<m_nNotMeetInfoCount&&i<8; i++)
	{
		if(mNotMeetInfo[i].nNoMeetFlag==wdFlag)
		{
			str=mNotMeetInfo[i].sShortName;
			break;
		}
	}
	return str;
}
