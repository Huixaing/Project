#include "StdAfx.h"
#include ".\loadgpcdef.h"

//extern UINT nSectionCount;
//extern SectionInfo mSectionList[100];
CLoadGPCDef::CLoadGPCDef(void)
{
	::CoInitialize(NULL);
	memset(mMapTable.dwAppCtrl,   0, sizeof(mMapTable.dwAppCtrl));
	memset(mMapTable.dwMarkCtrl,  0, sizeof(mMapTable.dwMarkCtrl));
	memset(mMapTable.dwTrainCtrl, 0, sizeof(mMapTable.dwTrainCtrl));
}

CLoadGPCDef::~CLoadGPCDef(void)
{
}

bool CLoadGPCDef::Load(CString pathname)
{
	char szFileName[128];

	sprintf(szFileName,"%s\\%s",pathname, "gpc.def");
	if(!LoadGPCDEF(szFileName))
		return false;
	
	sprintf(szFileName,"%s\\%s",pathname, "tgno.xml");
	if(!LoadTGNOXML(szFileName))
		return false;
	
	return true;
}

BOOL CLoadGPCDef::LoadGPCDEF(CString szFileName)
{
	FILE *fp=NULL;
	int result=0;
	char *ptr, strLine[512];
 
	mVectUser.clear();
	mVectStation.clear();
	mVectSection.clear();

 	if((fp=fopen(szFileName, "r"))==NULL)
		return 0;

	// read file
	int	lineNo = 0;
 	while (!feof(fp))	
	{
		lineNo ++;
		ptr = fgets(strLine, 512, fp);
		if (ptr == NULL) 
		continue;
		while (*ptr == ' ' || *ptr == '\t')
			ptr ++;
		if (*ptr == ';' || *ptr == 0 || *ptr == '\n')
			continue;

		if (*ptr == '[') 
		{  
			if (!strncmp(ptr, "[USER]", strlen("[USER]")))
				result=LoadUser(ptr+strlen("[USER]"));
			else
			if (!strncmp(ptr, "[SECTION]", strlen("[SECTION]")))
				result=LoadSectInfo(ptr+strlen("[SECTION]"));
			else 
			if	(!strncmp(ptr, "[STATION]", strlen("[STATION]"))) 
				result=LoadStationInfo(ptr+strlen("[STATION]"));
		}
		else
			result = 0;
	}

	// close gpc.def 
	fclose(fp);
	
	return 1; 
}

int CLoadGPCDef::LoadUser(char *ptr)
{
	char *token;
	char seps[]=" ,\t\\/=\n";

	GPCUser  user;

	// load console name
	memset(user.desp, 0, sizeof(user.desp));
	token = strtok(ptr, seps);
	if(token==NULL) return 0;
	strncpy( user.desp, token, 49);     //调度台名

	// load gpc id 
	token = strtok(NULL, seps);
	if(token==NULL) return 0;
	user.id = (int)atoi(token);      //gpcid
	
	// load user name
	memset(user.name,0,sizeof(user.name));
	token = strtok(NULL, seps);
	if(token==NULL) return 0;
	strncpy( user.name, token, 9);        //username
	
	// load user password
	token = strtok(NULL, seps);
	if(token==NULL) return 0;
	//strcpy( tmp.UserRealName, token );    //userRealname
	
	// load user identify
	token = strtok(NULL, seps);
	if(token==NULL) return 0;
	user.right = tolower(*token)-'a';
	
	// load sections list
	token = strtok(NULL, seps);
	if(token==NULL) return 0;
	int nSectionNum = (int)atoi(token);             //区段数目
	for(int i=0; i<nSectionNum;)
	{
		token = strtok(NULL, seps);
		if(token==NULL) return 0;
		
		if(strchr(token, '-')) 
		{
			int nStart=0, nEnd=0;
			sscanf(token, "%d-%d", &nStart, &nEnd);
			for(int j=nStart; j<=nEnd; j++)
			{
				user.ary_section.Add(j);     //区段列表
				i++;
			}
		} 
		else 
		{
			user.ary_section.Add((int)atoi(token));     //区段列表
			i++;
		}
	}
	
	mVectUser.push_back(user);
	return 1;
}

int CLoadGPCDef::LoadSectInfo(char *ptr)
{
	char *token;
	char seps[]=" ,\t\\/=\n";
	int i, j, nStart, nEnd;

	TGSection section;

	// load section number
	token = strtok(ptr, seps);
	if(token==NULL) return 0;
	section.id = (int)atoi(token);
	
	// load section name
	token = strtok(NULL, seps);
	if(token==NULL) return 0;
	strcpy(section.name, token);
	
	// load station list
	token = strtok(NULL, seps);
	if(token==NULL) return 0;
	
	int nStationNum = (int)atoi(token);
	for(i=0; i<nStationNum;) 
	{
		token = strtok(NULL, seps);
		if(token==NULL) 
			break;
		if(strchr(token, '-')) 
		{
			sscanf(token, "%d-%d", &nStart, &nEnd);
			if(nStart <= nEnd)
			{
				for(j=nStart; j<=nEnd; j++)
				{
					section.ary_station.Add(j);
					i++;
				}
			}
			else
			{
				for(j=nEnd; j<=nStart; j++)
				{
					section.ary_station.Add(j);
					i++;
				}
			}
		} 
		else 
		{
 			section.ary_station.Add((int)atoi(token));
			i++;
		}
	}

	//load schedule on doubleclick about self station & next station 
	token = strtok(NULL, seps);
	if(token==NULL)
		section.selfstd = 0;
	else
		section.selfstd = (int)atoi(token);
	
	token = strtok(NULL, seps);
	if(token==NULL)
		section.nextstd = 0;//(int)atoi(token);
	else
		section.nextstd = (int)atoi(token);
	
	mVectSection.push_back(section);
	
	// scuccess return
	return 1;
}

int CLoadGPCDef::LoadStationInfo(char *ptr)
{
	char *token;
	char seps[]=" ,\t\\/=\n*";

	TGStation station;
	// load station number
	token = strtok(ptr, seps);
	if(token==NULL) return 0;
	station.id = (int)atoi(token);

	// load station name
	token = strtok(NULL, seps);
	if(token==NULL) return 0;
	strncpy(station.name, token,63);
	station.name[63]=0;
	// load station page number
	token = strtok(NULL, seps);
	if(token==NULL) return 0;
	
	// load page width and height
	token = strtok(NULL, seps); 
	if(token==NULL) return 0;
	token = strtok(NULL, seps); 
	if(token==NULL) return 0;
	
	// load station distance sign 
	token = strtok(NULL, seps); 
	if(token==NULL) return 0;
	strncpy(station.fKmMark, token, 63);
	station.fKmMark[63]=0;
	// load draw draw train start or arrive time flag
	token = strtok(NULL, seps);
	if(token==NULL) return 0;
	station.adtime= atoi(token);
	// load station class
	token = strtok(NULL, seps);
	if(token==NULL) return 0;
	station.staclass= atoi(token);

	//车站性质:-1, 无设备, 0,TDCS, 1, CTC
	token = strtok(NULL,seps);
	if(token== NULL)
	  station.property = -1;
	else
      station.property = (int)atoi(token);

	mVectStation.push_back(station);
	return 1;
}

CString CLoadGPCDef::GetSectionXML()
{
	CString xml = "<SECTION_SET>", tmp;
	for(int i = 0; i < mVectSection.size(); i++)
	{
		tmp.Format("\n<SECTION id=\"%d\" name=\"%s\" selfstd=\"%d\" nextstd=\"%d\">",
			mVectSection[i].id, mVectSection[i].name,
			mVectSection[i].selfstd, mVectSection[i].nextstd);
        xml += tmp;
        
		for(int j = 0; j < mVectSection[i].ary_station.GetCount(); j++)
		{
			tmp.Format("\n<station id=\"%d\"/>", mVectSection[i].ary_station[j]);
			xml += tmp;
		}
	    xml += "\n</SECTION>";
	}
    xml += "\n</SECTION_SET>";
    SaveXML("c:\\SECTION_SET.xml", xml);
	return xml;
}
CString CLoadGPCDef::GetStationXML()
{
	CString xml = "<STATION_SET>", tmp;
	for(int i = 0; i < mVectStation.size(); i++)
	{
		tmp.Format("\n<STATION id=\"%d\" name=\"%s\" klio_mark=\"%s\" ad_time=\"%d\" staclass=\"%d\" property=\"%d\"/>",
			mVectStation[i].id, mVectStation[i].name,
			mVectStation[i].fKmMark, mVectStation[i].adtime, mVectStation[i].staclass, mVectStation[i].property);
        xml += tmp;
	}
    xml += "\n</STATION_SET>";

	SaveXML("c:\\STATION_SET.xml", xml);
	return xml;
}

CString CLoadGPCDef::GetCurrSectionSetXML(const int gpcid, const int section_id)
{
	CString xml, tmp;
	
	for(int idx=0; idx<mVectUser.size(); idx++)
	{
		if(mVectUser[idx].id == gpcid && mVectUser[idx].ary_section.GetCount() > 0)
		{
			int con_id = -1;
			for(int k=0;k<mVectUser[idx].ary_section.GetCount();k++)
			{
				if(mVectUser[idx].ary_section.GetAt(k) == section_id)
				{
					con_id = section_id;
					break;
				}
			}

			if(con_id == -1)
				con_id = mVectUser[idx].ary_section.GetAt(0);

			xml.Format("<CURRENT_SECTION id=\"%d\">", con_id);
			for(int i = 0; i < mVectUser[idx].ary_section.GetCount(); i++)
			{
				tmp.Format("\n<SECTION id=\"%d\"/>", mVectUser[idx].ary_section.GetAt(i));
				xml += tmp;
			}
			xml += "\n</CURRENT_SECTION>";
			break;
		}
	}
	
	SaveXML("c:\\CURRENT_SECTION.xml", xml);
	return xml;
}

bool CLoadGPCDef::GetUserRight(const int gpcid, RIGHT& right)
{
	char ch = 'A';

	for(int idx=0; idx<mVectUser.size(); idx++)
	{
		if(mVectUser[idx].id == gpcid)
		{
			ch += mVectUser[idx].right;
			break;
		}
	}

	
	for(int idx = 0; idx < this->mUserRight.size(); idx++)
	{
		if(mUserRight[idx].id == ch)
		{
			right = mUserRight[idx];
			return true;
		}
	}
	
	CString msg;
	//msg.Format("不存在类型为 %C的权限配置,请修改对应权限", 'B');
	//AfxMessageBox(msg);
	return false;
}

bool CLoadGPCDef::SetRight(MSXML::IXMLDOMNodeListPtr list, long& right, BYTE flag)
{
	bool bRet = false;
	right = 0;

	if(NULL == list || flag > 2)
		return bRet;

	MSXML::IXMLDOMNodePtr pChildNode,pNode;
	for(int idx = 0; idx < list->Getlength(); idx++)
	{
		pNode = list->Getitem(idx);
		if(pNode)
		{
			bRet = true;

			int  id = 0;
			// @id
			pChildNode = pNode->selectSingleNode("@id");
			if(pChildNode)
			{
				_variant_t v;
				v = pChildNode->GetnodeTypedValue();
				id = atoi(CString(v.bstrVal));
				SysFreeString(v.bstrVal);
				v.Clear();
                
				if(flag ==TBL::APP && id>0 && id < sizeof(mMapTable.dwAppCtrl)/sizeof(mMapTable.dwAppCtrl[0]))
					right += mMapTable.dwAppCtrl[id];
				if(flag ==TBL::MARK && id>0 && id < sizeof(mMapTable.dwMarkCtrl)/sizeof(mMapTable.dwMarkCtrl[0]))
					right += mMapTable.dwMarkCtrl[id];
				if(flag ==TBL::TRAIN && id>0 && id < sizeof(mMapTable.dwTrainCtrl)/sizeof(mMapTable.dwTrainCtrl[0]))
					right += mMapTable.dwTrainCtrl[id];
			}
			else
			{
				int begin_id = 0, end_id = 0;
				// @begin_id
				pChildNode = pNode->selectSingleNode("@begin_id");
				if(pChildNode)
				{
					_variant_t v;
					v = pChildNode->GetnodeTypedValue();
					begin_id = atoi(CString(v.bstrVal));
					SysFreeString(v.bstrVal);
					v.Clear();
				}
				// @end_id
				pChildNode = pNode->selectSingleNode("@end_id");
				if(pChildNode)
				{
					_variant_t v;
					v = pChildNode->GetnodeTypedValue();
					end_id = atoi(CString(v.bstrVal));
					SysFreeString(v.bstrVal);
					v.Clear();
				}
				
				for(int idx=begin_id; idx<=end_id; idx++)
				{
					if(flag ==TBL::APP && idx>0 && idx < sizeof(mMapTable.dwAppCtrl)/sizeof(mMapTable.dwAppCtrl[0]))
						right += mMapTable.dwAppCtrl[idx];
					if(flag ==TBL::MARK && idx>0 && idx < sizeof(mMapTable.dwMarkCtrl)/sizeof(mMapTable.dwMarkCtrl[0]))
						right += mMapTable.dwMarkCtrl[idx];
					if(flag ==TBL::TRAIN && idx>0 && idx < sizeof(mMapTable.dwTrainCtrl)/sizeof(mMapTable.dwTrainCtrl[0]))
						right += mMapTable.dwTrainCtrl[idx];
				}
			}
		}
	}

	return bRet;
}

BOOL CLoadGPCDef::LoadTGNOXML(CString pathname)
{
	memset(mMapTable.dwAppCtrl,   0, sizeof(mMapTable.dwAppCtrl)/sizeof(mMapTable.dwAppCtrl[0]));
	memset(mMapTable.dwMarkCtrl,  0, sizeof(mMapTable.dwMarkCtrl)/sizeof(mMapTable.dwMarkCtrl[0]));
	memset(mMapTable.dwTrainCtrl, 0, sizeof(mMapTable.dwTrainCtrl)/sizeof(mMapTable.dwTrainCtrl[0]));

	MSXML::IXMLDOMDocumentPtr pXMLDoc;
	HRESULT hr = pXMLDoc.CreateInstance(MSXML::CLSID_DOMDocument);
	if(FAILED(hr))
	{
		_com_error er(hr);
		AfxMessageBox(er.ErrorMessage());
		return false;
	}
	
	if(VARIANT_TRUE != pXMLDoc->load( _bstr_t( pathname ) ) )
	{
		CString  prompt;
		prompt.Format("配置%s不存在或者不符合xml语法(可以用浏览器检查是否能打开)", pathname);
		AfxMessageBox(prompt);
		
		return false; 
	}
	
	MSXML::IXMLDOMNodePtr pChildNode,pNode,pRootNode;

	pRootNode = pXMLDoc->selectSingleNode( "GPC_TG_DEFINE");
	if(pRootNode == NULL)
		return false;
  
    // TABLE
	MSXML::IXMLDOMNodeListPtr list;
	
	list= pRootNode->selectNodes( "TABLE/APP_CTRL/CTRL");
	if(NULL != list)
	{
		for(int idx = 0; idx < list->Getlength(); idx++)
		{
			pNode = list->Getitem(idx);
			if(pNode)
			{
				int  id = 0;
				long set = 0;
				
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

				// @set
				pChildNode = pNode->selectSingleNode("@set");
				if(pChildNode)
				{
					_variant_t v;
					v = pChildNode->GetnodeTypedValue();
					sscanf(CString(v.bstrVal), "%x", &set);
					SysFreeString(v.bstrVal);
					v.Clear();
				}

				if(id>0)
					mMapTable.dwAppCtrl[id] = set;
			}
		}
	}

	list= pRootNode->selectNodes( "TABLE/MARK_CTRL/CTRL");
	if(NULL != list)
	{
		for(int idx = 0; idx < list->Getlength(); idx++)
		{
			pNode = list->Getitem(idx);
			if(pNode)
			{
				int  id = 0;
				long set = 0;
				
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

				// @set
				pChildNode = pNode->selectSingleNode("@set");
				if(pChildNode)
				{
					_variant_t v;
					v = pChildNode->GetnodeTypedValue();
					sscanf(CString(v.bstrVal), "%x", &set);
					SysFreeString(v.bstrVal);
					v.Clear();
				}

				if(id>0)
					mMapTable.dwMarkCtrl[id] = set;
			}
		}
	}

	list= pRootNode->selectNodes( "TABLE/TRAIN_CTRL/CTRL");
	if(NULL != list)
	{
		for(int idx = 0; idx < list->Getlength(); idx++)
		{
			pNode = list->Getitem(idx);
			if(pNode)
			{
				int  id = 0;
				long set = 0;
				
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

				// @set
				pChildNode = pNode->selectSingleNode("@set");
				if(pChildNode)
				{
					_variant_t v;
					v = pChildNode->GetnodeTypedValue();
					sscanf(CString(v.bstrVal), "%x", &set);
					SysFreeString(v.bstrVal);
					v.Clear();
				}

				if(id>0)
					mMapTable.dwTrainCtrl[id] = set;
			}
		}
	}

	mUserRight.clear();

	long train=0;
	long mark=0;
	long app=0;

	// RIGHT
    list= pRootNode->selectNodes( "RIGHT/APP/CTRL");
	if(NULL != list)
	{
		SetRight(list, app, TBL::APP);
	}

	list= pRootNode->selectNodes( "RIGHT/MARK/CTRL");
	if(NULL != list)
	{
		SetRight(list, mark, TBL::MARK);
	}

	list= pRootNode->selectNodes( "RIGHT/TRAIN/CTRL");
	if(NULL != list)
	{
		SetRight(list, train, TBL::TRAIN);
	}

	list= pRootNode->selectNodes( "RIGHT/USERS/USER");
	if(NULL != list)
	{
		for(int idx = 0; idx < list->Getlength(); idx++)
		{
			pNode = list->Getitem(idx);
			if(pNode)
			{
				RIGHT right;
				// @id
				pChildNode = pNode->selectSingleNode("@id");
				if(pChildNode)
				{
					_variant_t v;
					v = pChildNode->GetnodeTypedValue();
					CString tmp = CString(v.bstrVal);
					if(tmp == "A" || tmp == "a")
					{
						right.id = 'A';
					}
					else
					if(tmp == "B" || tmp == "b")
					{
						right.id = 'B';
					}
					else
					if(tmp == "C" || tmp == "c")
					{
						right.id = 'C';
					}
					else
					if(tmp == "R" || tmp == "r")
					{
						right.id = 'R';
					}
					else
					if(tmp == "Q" || tmp == "q")
					{
						right.id = 'Q';
					}
					else
					if(tmp == "X" || tmp == "x")
					{
						right.id = 'X';
					}
					else
					{
						right.id = ' ';
					}
					SysFreeString(v.bstrVal);
					v.Clear();
				}
				else
				{
					right.id = ' ';
				}

				//@operator
				pChildNode = pNode->selectSingleNode("@operator");
				if(pChildNode)
				{
					_variant_t v;
					v = pChildNode->GetnodeTypedValue();
					right.operate = atoi(CString(v.bstrVal));
					SysFreeString(v.bstrVal);
					v.Clear();
				}
				else
				{
					right.operate = 0;
				}
				
				// WinTg_Term控制权限,暂时定为32位权限
				right.wintg_operate = 0;
				MSXML::IXMLDOMNodePtr pTmpNode, pTmpChildNode;
				pTmpNode = pNode->selectSingleNode("WINTG_TERM");
				if(pTmpNode)
				{
					for(int i=0;i<32;i++)
					{
						CString strFlag;
						strFlag.Format("bit%d", i);
						pTmpChildNode = pTmpNode->selectSingleNode(strFlag.AllocSysString());
						if(pTmpChildNode == NULL)
							continue;

						int set = 0;
						pChildNode = pTmpChildNode->selectSingleNode("@set");
						if(pChildNode)
						{
							_variant_t v;
							v = pChildNode->GetnodeTypedValue();
							set = atoi(CString(v.bstrVal));
							SysFreeString(v.bstrVal);
							v.Clear();
						}
						if(set!=0)
						{
							right.wintg_operate |= (1<<i);
						}
					}
				}

				right.train = train;
				right.mark  = mark;
				right.app   = app;

				MSXML::IXMLDOMNodeListPtr tmplist;
				// 列车菜单定制
				tmplist= pNode->selectNodes( "TRAIN/CTRL");
				if(NULL != tmplist)
				{
					long tmp = 0;
					if(SetRight(tmplist, tmp, TBL::TRAIN))
					    right.train = tmp;
				}

				// 标记菜单定制
				tmplist= pNode->selectNodes( "MARK/CTRL");
				if(NULL != tmplist)
				{
					long tmp = 0;
					if(SetRight(tmplist, tmp, TBL::MARK))
					    right.mark  = tmp;
				}

				// 应用定制
				tmplist= pNode->selectNodes( "APP/CTRL");
				if(NULL != tmplist)
				{
					long tmp = 0;
					if(SetRight(tmplist, tmp, TBL::APP))
						right.app  = tmp;
				}

				

				if(right.id != ' ')
				{
					mUserRight.push_back(right);
				}
			}
		}
	}

    
	// LineWidth
	this->m_LineWidth = 1;
	pNode = pRootNode->selectSingleNode("LineWidth");
	if(pNode)
	{
		// @set
		pChildNode = pNode->selectSingleNode("@set");
		if(pChildNode)
		{
			_variant_t v;
			v = pChildNode->GetnodeTypedValue();
			this->m_LineWidth = atoi(CString(v.bstrVal));
			SysFreeString(v.bstrVal);
			v.Clear();
		}
	}

	// LeapLineFlag
	this->m_nLeapLineFlag = 2;
	pNode = pRootNode->selectSingleNode("LeapLineFlag");
	if(pNode)
	{
		// @set
		pChildNode = pNode->selectSingleNode("@set");
		if(pChildNode)
		{
			_variant_t v;
			v = pChildNode->GetnodeTypedValue();
			this->m_nLeapLineFlag = atoi(CString(v.bstrVal));
			SysFreeString(v.bstrVal);
			v.Clear();
		}
	}

	// TenicalStop
	this->m_nDrawTenicalStop = 1;
	pNode = pRootNode->selectSingleNode("TenicalStop");
	if(pNode)
	{
		// @set
		pChildNode = pNode->selectSingleNode("@set");
		if(pChildNode)
		{
			_variant_t v;
			v = pChildNode->GetnodeTypedValue();
			this->m_nDrawTenicalStop = atoi(CString(v.bstrVal));
			SysFreeString(v.bstrVal);
			v.Clear();
		}
	}

	// BackgroudBlock
	this->m_backgroudNum = 2;
	pNode = pRootNode->selectSingleNode("BackgroudBlock");
	if(pNode)
	{
		// @set
		pChildNode = pNode->selectSingleNode("@set");
		if(pChildNode)
		{
			_variant_t v;
			v = pChildNode->GetnodeTypedValue();
			this->m_backgroudNum = atoi(CString(v.bstrVal));
			if(this->m_backgroudNum != 1 && this->m_backgroudNum != 2 && this->m_backgroudNum != 5)
			{
				this->m_backgroudNum = 2;
			}
			SysFreeString(v.bstrVal);
			v.Clear();
		}
	}

	// 进路准备好标志 0: 不绘制 1: 紫色线 2: 圆圈
	this->m_nDrawRouteOkFlag = 1;
	pNode = pRootNode->selectSingleNode("RouteOkMark");
	if(pNode)
	{
		// @set
		pChildNode = pNode->selectSingleNode("@set");
		if(pChildNode)
		{
			_variant_t v;
			v = pChildNode->GetnodeTypedValue();
			this->m_nDrawRouteOkFlag = atoi(CString(v.bstrVal));
			SysFreeString(v.bstrVal);
			v.Clear();
		}
	}

	return true;
}

//-----------------------------------------------------------
TGStation::TGStation()
{
	memset(this->name,0, 64);
	this->id = 0;
	memset(this->fKmMark,0,64);
	this->adtime = 0;
	this->staclass = 0;
	this->property = 0;
}
TGStation::TGStation(const TGStation& rhs)
{
	strncpy(this->name,rhs.name, 64);
	this->id = rhs.id;
	strncpy(this->fKmMark,rhs.fKmMark, 64);
	this->adtime = rhs.adtime;
	this->staclass = rhs.staclass;
	this->property = rhs.property;
}

TGStation& TGStation::operator=(const TGStation& rhs)
{
	if(this == &rhs)
		return (*this);
	strncpy(this->name,rhs.name, 64);
	this->id = rhs.id;
	strncpy(this->fKmMark,rhs.fKmMark, 64);
	this->adtime = rhs.adtime;
	this->staclass = rhs.staclass;
	this->property = rhs.property;

	return (*this);
}

TGSection::TGSection()
{
	this->id = 0;
	this->selfstd = 0;
	this->nextstd = 0;
	memset(this->name,0, 50);
	ary_station.RemoveAll();
}

TGSection::TGSection(const TGSection& rhs)
{
	this->id = rhs.id;
	this->selfstd = rhs.selfstd;
	this->nextstd = rhs.nextstd;
	strncpy(this->name,rhs.name, 50);

	this->ary_station.Append(rhs.ary_station);
}

TGSection& TGSection::operator=(const TGSection& rhs)
{
	if(this == &rhs)
		return (*this);
	this->id = rhs.id;
	this->selfstd = rhs.selfstd;
	this->nextstd = rhs.nextstd;
	strncpy(this->name,rhs.name, 50);

	this->ary_station.RemoveAll();
	this->ary_station.Append(rhs.ary_station);
	return (*this);
}

GPCUser::GPCUser()
{
	desp[0]=0;
	name[0]=0;
	id=-1;
	right=0;
	ary_section.RemoveAll();
}

GPCUser::GPCUser(const GPCUser& rhs)
{
	this->id = rhs.id;
	this->right = rhs.right;
	strncpy(this->name,rhs.name, 10);
	strncpy(this->desp,rhs.desp, 50);

	this->ary_section.Append(rhs.ary_section);
}

GPCUser& GPCUser::operator=(const GPCUser& rhs)
{
	if(this == &rhs)
		return (*this);

	this->id = rhs.id;
	this->right = rhs.right;
	strncpy(this->name,rhs.name, 10);
	strncpy(this->desp,rhs.desp, 50);

	this->ary_section.Append(rhs.ary_section);

	return (*this);
}

void CLoadGPCDef::SaveXML(CString filename, CString xml)
{
	CStdioFile file;
	if(file.Open(filename, CFile::modeCreate | CFile::modeReadWrite | CFile::typeText))
	{
		file.WriteString("<?xml version=\"1.0\" encoding=\"GB2312\" ?>");
		file.WriteString(xml);
		file.Close();
	}
}

int CLoadGPCDef::GetSectionListCount()
{
	return mVectSection.size();
}
int CLoadGPCDef::GetSectionNameByIndex(int index, CString &name)
{
	int section_no=0;
	if (index<0 || index>=mVectSection.size())
		return section_no;

	section_no = mVectSection[index].id;
	if (section_no!=0)
		name.Format("%s", mVectSection[index].name);

	return section_no;
}

CString CLoadGPCDef::GetSectionNameById(int section)
{
	for (int i=0; i<mVectSection.size(); i++)
	{
		if (section == mVectSection[i].id)
			return mVectSection[i].name;
	}
	return "";
}