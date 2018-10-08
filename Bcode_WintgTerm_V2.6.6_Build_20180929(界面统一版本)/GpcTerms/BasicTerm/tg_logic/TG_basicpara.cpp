#include "stdafx.h"
#include "sysshare.h"
#include "function.h"
#include "tg_basicpara.h"
#include "tg_schedule_base.h"
#include "tg_function_config.h"
#include "inifile.h"
#import  <msxml.dll> named_guids
using namespace MSXML;
#include "../utility/xml_utility.h"
#define ABSTIME_MAP_INDEX(absnum,type,dir)  (absnum*MAX_TRAIN_RUN_TYPE+type)*2+dir

extern BOOL IsCrhTrain(int type, int run_type);

AbsTime::AbsTime()
{
	Init();
}
AbsTime::~AbsTime()
{
	Init();
}
AbsTime::AbsTime(const AbsTime& rhs)
{
	abs_number=rhs.abs_number;
	trainAbsTime.assign(rhs.trainAbsTime.begin(), rhs.trainAbsTime.end());
}
AbsTime& AbsTime::operator=(const AbsTime& rhs)
{
	if (this == &rhs)
	{
		return (*this);		
	}
	trainAbsTime.clear();

	abs_number=rhs.abs_number;
	trainAbsTime.assign(rhs.trainAbsTime.begin(), rhs.trainAbsTime.end());

	return (*this);
}

void AbsTime::Init()
{
	abs_number=0;
	trainAbsTime.clear();
}

CTGBasicParam::CTGBasicParam(CTGFunctionConfig *pcfg)
{
	m_pConfig=pcfg;
}

BOOL  CTGBasicParam::InitData()
{
	do
	{
		CGpcdef *gpcdefptr=m_pConfig->GetGPCDEFx();

		int nSectionList[128], nSectionNum=128;
		nSectionNum=gpcdefptr->GetSectionList(nSectionNum, nSectionList);
		
		for(int i=0;i<nSectionNum;i++)
		{
			for(int secindex=0;secindex<m_pConfig->m_nSectionCount;secindex++)
			{
				if(nSectionList[i]!=m_pConfig->m_aSectionList[secindex].section_id)
					continue;

				int nStationNum,nStationList[512];
				nStationNum = sizeof(nStationList)/sizeof(nStationList[0]);
				int rs=gpcdefptr->GetStationList(m_pConfig->m_aSectionList[secindex].section_id,nStationNum,nStationList);
				if(rs>0)
				{
					for(int n=0;n<nStationNum;n++)
						CreateStation(nStationList[n]);
				}
			}

			// 收发邻台TG_SVR
			int nNeibNum=64, nNeibSectionNo[64];
			nNeibNum=gpcdef.GetNeighberSectionList(nSectionList[i], nNeibNum, nNeibSectionNo);
			for(int j=0; j<nNeibNum; j++)
			{
				int nStationNum,nStationList[512];
				nStationNum = sizeof(nStationList)/sizeof(nStationList[0]);
				int rs=gpcdefptr->GetStationList(nNeibSectionNo[j],nStationNum,nStationList);
				if(rs>0)
				{
					for(int n=0;n<nStationNum;n++)
						CreateStation(nStationList[n]);
				}
			}

			// 查阅邻台TG_SVR
 			int nViewNeibNum=64, nViewNeibSectionNo[64];
 			nViewNeibNum=gpcdef.GetNeighberViewPlanSectionList(nSectionList[i], nViewNeibNum, nViewNeibSectionNo);
 			for(int k=0; k<nViewNeibNum; k++)
 			{
 				int nStationNum,nStationList[512];
 				nStationNum = sizeof(nStationList)/sizeof(nStationList[0]);
 				int rs=gpcdefptr->GetStationList(nViewNeibSectionNo[k],nStationNum,nStationList);
 				if(rs>0)
 				{
 					for(int n=0;n<nStationNum;n++)
 						CreateStation(nStationList[n]);
 				}
 			}
		}

		if(m_aCadStations.size()<=0)
		{
			AfxMessageBox("不能获取车站列表");
			break;
		}
		m_szDataPath=m_pConfig->GetDataPath();
		for(int n=0; n<nSectionNum; n++)
		{
			int nSectionId=nSectionList[n];
			SectionDefine node_section;
			if(GetTGFunctionConfig()->GetSectionConfig(nSectionId, node_section))
			{
				for(int m=0; m<node_section.stdef_num; m++)
				{
					CString str, tmp;
					str.Format("[section]=%d,%d,%d", node_section.con_id, node_section.stinfo[m].index, node_section.stinfo[m].station);
					
					tmp.Format(",%d",node_section.stinfo[m].exit_line_num);
					str+=tmp;
					for(int i=0; i<node_section.stinfo[m].exit_line_num; i++)
					{
						DWORD line=node_section.stinfo[m].exit_line[i];
						m_map_gpc_lineno[line]=line;
						tmp.Format("/%d", line);
						str+=tmp;
					}

					tmp.Format(",%d",node_section.stinfo[m].entry_line_num);
					str+=tmp;
					for(int i=0; i<node_section.stinfo[m].entry_line_num; i++)
					{
						DWORD line=node_section.stinfo[m].entry_line[i];
						m_map_gpc_lineno[line]=line;
						tmp.Format("/%d", line);
						str+=tmp;
					}

					//[section]=7,1,112,2/1123/1124,2/1121/1122
					GpcLogTrainMessage(0, 0, "%s",str);
				}
			}
		}
		if(m_map_gpc_lineno.size()<=0)
		{
			AfxMessageBox("不能读取 区间线别");
			break; 
		}

		if(!LoadStationDef("stationside.def"))
		{
			AfxMessageBox("不能读取 stationside.def");
			break;
		}
		if(!LoadAbsTimeData("run_time.txt"))
		{
			AfxMessageBox("不能读取 abstime.txt");
			return FALSE;
		}
		LoadUserAbsTimeData();	// qiaoyt runtime.txt	
		
		if(!LoadEntryConfig("entry.cfg"))
		{
			AfxMessageBox("不能读取 entry.cfg");
			break;
		}
		if(!LoadPassway("passway.xml"))
		{
			AfxMessageBox("不能读取 passway.xml");
			break; 
		}
		if(!LoadPrioConfig("train_type.xml"))
		{
			AfxMessageBox("不能读取 train_type.xml");
			break; 
		}
		if(!LoadAbsAttrib("tg.def"))
		{
			AfxMessageBox("不能读取 tg.def");
			break; 
		}
		if(!LoadStaNowCar("station_now_car.xml"))
		{
			AfxMessageBox("不能读取 station_now_car.xml,站存车配置信息");
			break;
		}

		LoadLocoTypeConfig("locotype.cfg");
		LoadTmisAbsAttrib("tmis_abs.cfg");
		LoadGapTimeConfig("gap_time.xml");
		LoadLimitStationSideConfig("limit_side.cfg");


		///// 删除无关记录
		for(IterMapAbsTime iter = m_aAbsTimes.begin();iter!=m_aAbsTimes.end(); iter++)
		{
			if(iter->second.trainAbsTime.size()>0)
			{
				std::vector<TrainAbsTime>::iterator it = iter->second.trainAbsTime.begin();
				for(; it != iter->second.trainAbsTime.end(); it++)
				{
					if(it->station1==-1 && it->station2==-1)
					{
						iter->second.trainAbsTime.erase(it);
					}
				}
			}
		}
		return TRUE;
	}while(0);
	return FALSE;
}


void CTGBasicParam::DeinitData()
{
	IterMapStation iter = m_aCadStations.begin();
	while(iter!=m_aCadStations.end())
	{
		if(iter->second.pStationDef)
			delete iter->second.pStationDef;
		iter++;
	}
	m_aCadStations.clear();
}

CTGBasicParam::~CTGBasicParam()
{

}

CTGBasicParam::ABS_STRUCT_PTR CTGBasicParam::GetAbsTimePtr(int absnumber,int type,int dir)
{
	IterMapAbsTime iter = m_aAbsTimes.find(absnumber);
	if (iter != m_aAbsTimes.end())
	{
		for(int i=0; i<iter->second.trainAbsTime.size(); i++)
		{
			if(iter->second.trainAbsTime[i].type == type && iter->second.trainAbsTime[i].direction == dir)
			{
				return &iter->second.trainAbsTime[i];
			}
		}
	}
	
	return NULL;
}

CTGBasicParam::CAD_STATION *CTGBasicParam::GetStationPtr(int stno)
{
	IterMapStation iter = m_aCadStations.find(stno);
	if (iter != m_aCadStations.end())
		return &iter->second;
	else
		return NULL;

}
CTGBasicParam::CAD_STATION *CTGBasicParam::CreateStation(int stno)
{
	CAD_STATION *p=GetStationPtr(stno);
	if(p)
		return p;
	CAD_STATION st;
	st.nStationNo=stno;
	m_aCadStations[stno]=st;
	return &m_aCadStations[stno];
}

BOOL  CTGBasicParam::LoadPassway(const char *file)
{
	char filepath[256];
	sprintf(filepath,"%s/%s",m_szDataPath,file); 
	m_cPasswayInfo.LoadPasswayXML(filepath);
	return TRUE;
}

BOOL  CTGBasicParam::LoadGapTimeConfig(const char *file)
{
	char filepath[256];
	sprintf(filepath,"%s/%s",m_szDataPath,file); 
	m_cGapTimeInfo.LoadFromXML(filepath);
	return TRUE;
}

BOOL  CTGBasicParam::LoadStationDef(const char *cfgfile)
{
	char station_def_file[256];
	sprintf(station_def_file,"%s/%s",m_szDataPath,cfgfile); 
	IterMapStation iter = m_aCadStations.begin();
	for(;iter!=m_aCadStations.end();iter++)
	{
		CAD_STATION *pstat;
		pstat=&iter->second;
		pstat->pStationDef=new CStationDef(pstat->nStationNo, station_def_file);
		if(!pstat->pStationDef)
			continue;
		short line_no;
		CStationDef::READ_FILE_RESULT rs=pstat->pStationDef->GetReadFileRs(&line_no);
		if (CStationDef::READ_FILE_SUCC != rs) 
		{
			delete pstat->pStationDef;
			pstat->pStationDef=NULL;
			continue;
		}
	}

	if(m_aCadStations.size()>0)
		return TRUE;
	else
		return FALSE;
}

BOOL  CTGBasicParam::LoadAbsTimeData(const char* file0)
{
	if(m_aAbsTimes.size()>0)  // 已经读入了，因为是静态
		return TRUE;
	char filepath[256];
	sprintf(filepath,"%s/%s",m_szDataPath,file0); 

	FILE *fp = fopen(filepath, "r");
	if (NULL == fp)
		return FALSE;
	m_aAbsTimes.clear();
	long abs_number;
	USHORT dir,type;
	char  number[12];
	char  ptr[512];
	memset(ptr,0,sizeof(ptr));
	int x;
	int stop_time,start_time,run_time;
	
	while (!feof(fp))
	{
		if((x=GetConfigLine(fp,ptr,sizeof(ptr)))<=0)
		{
			if(x) 
				printf("Error in getting a abs_time line.\r\n");
			break;
		}
		if(strstr(ptr,"ABS_TIME_END"))
			break;
		do
		{
			abs_number=0;
			dir=type=0;
			number[0]=0;
			stop_time=start_time=run_time=0;
			if (GetParmValue(ptr,"ABS_NUMBER",TYPE_LONG,&abs_number,1)) break;

			if(GetParmValue(ptr,"DIRECTION",TYPE_SHORT,&dir,1)) break;

			if(GetParmValue(ptr,"TYPE",TYPE_SHORT,&type,1)) break;

			if(GetParmValue(ptr,"STOP_TIME",TYPE_STRING,number,10)) break;		
			stop_time= (short)(atof(number)*60);

			if(GetParmValue(ptr,"START_TIME",TYPE_STRING,number,10)) break;
			start_time= (short)(atof(number)*60);

			if(GetParmValue(ptr,"RUN_TIME",TYPE_STRING,number,10)) break;
			run_time= (short)(atof(number)*60);

			map<DWORD, DWORD>::iterator it1=m_map_gpc_lineno.find(abs_number);
			if(it1==m_map_gpc_lineno.end())
				break;

			TrainAbsTime p;

			p.abs_number=abs_number;

			p.direction=dir;
			p.type=type;
			p.stop_time=stop_time;
			p.start_time=start_time;
			p.run_time=run_time;

			p.orig_stop_time=stop_time;
			p.orig_start_time=start_time;
			p.orig_run_time=run_time;

			IterMapAbsTime it = m_aAbsTimes.find(p.abs_number);
			if(it == m_aAbsTimes.end())
			{
				AbsTime abs;
				abs.abs_number = p.abs_number;
				abs.trainAbsTime.push_back(p);
				m_aAbsTimes.insert(make_pair(abs.abs_number, abs));
			}
			else
			{
				it->second.trainAbsTime.push_back(p);
			}
			
		}while(0);
	}

	fclose(fp);

	if(m_aAbsTimes.size()>0)
		return TRUE;
	else
		return FALSE;
}
#define USER_ABS_TIME_FILE "user_abs_time.xml"
BOOL  CTGBasicParam::LoadUserAbsTimeData(const char* file0)
{
	char filepath[256];
	if(file0==NULL)
		file0=USER_ABS_TIME_FILE;
	sprintf(filepath,"%s/%s",m_szDataPath,file0); 
	CString xml;
	if(!ReadGenernalXMLFile(xml,filepath))
		return FALSE;
	LOAD_XML(xml,"ABSTIME",pNodeRoot)
		MSXML::IXMLDOMNodeListPtr pList = pNodeRoot->selectNodes( "RECORD" );
	int rec_count = pList->Getlength();
	for(int i=0;i<rec_count;i++)
	{
		int absnum,type,direction,start_time,stop_time,run_time;
		GET_NODE_INTVALUE(absnum,pList->Getitem(i),"absnum");
		GET_NODE_INTVALUE(type,pList->Getitem(i),"type");
		GET_NODE_INTVALUE(direction,pList->Getitem(i),"direction");
		GET_NODE_INTVALUE(start_time,pList->Getitem(i),"start_time");
		GET_NODE_INTVALUE(stop_time,pList->Getitem(i),"stop_time");
		GET_NODE_INTVALUE(run_time,pList->Getitem(i),"run_time");

		ABS_STRUCT_PTR p=GetAbsTimePtr(absnum,type,direction);
		if(p)
		{
			p->start_time=start_time;
			p->stop_time=stop_time;
			p->run_time=run_time;
		}
	}
	return TRUE;
}

BOOL  CTGBasicParam::SaveUserAbsTimeData(const char* file0)
{
	char filepath[256];
	if(file0==NULL)
		file0=USER_ABS_TIME_FILE;
	sprintf(filepath,"%s/%s",m_szDataPath,file0); 
	CString xml;
	xml="<ABSTIME>";

	IterMapAbsTime iter=m_aAbsTimes.begin();
	for(;iter!=m_aAbsTimes.end(); iter++)
	{
		std::vector<TrainAbsTime>::iterator it = iter->second.trainAbsTime.begin();
		for(; it != iter->second.trainAbsTime.end(); it++)
		{
			if(it->orig_run_time!=it->run_time
			|| it->orig_start_time!=it->start_time
			|| it->orig_stop_time!=it->stop_time)
			{
				xml+="<RECORD>";
				InsertXMLNodeValue(xml,"absnum",it->abs_number);
				InsertXMLNodeValue(xml,"direction",it->direction);
				InsertXMLNodeValue(xml,"type",it->type);
				InsertXMLNodeValue(xml,"start_time",it->start_time);
				InsertXMLNodeValue(xml,"stop_time",it->stop_time);
				InsertXMLNodeValue(xml,"run_time",it->run_time);
				xml+="</RECORD>";
			}
		}
	}
	xml+="</ABSTIME>";
	return WriteGenernalXMLFile(xml,filepath);
}

BOOL CTGBasicParam::SetAbsTimeData(TrainAbsTime& tm)
{
	IterMapAbsTime iter=m_aAbsTimes.find(tm.abs_number);
	if(iter!=m_aAbsTimes.end())
	{
		std::vector<TrainAbsTime>::iterator it = iter->second.trainAbsTime.begin();
		for(; it != iter->second.trainAbsTime.end(); it++)
		{
			if(it->direction == tm.direction &&	it->type == tm.type)
			{
				it->run_time=tm.run_time;			
				it->start_time=tm.start_time;
				it->stop_time=tm.stop_time;

				return TRUE;
			}
		}
	}
	
	return FALSE;
}

BOOL CTGBasicParam::LoadEntryConfig(const char *cfgfile)
{
	// *********** lyc 2001/4/30 ************
	int  i = 0;

	int  entrys,station;  // CUIHU!!! short 型变量不能用于scanf %d
	int  entry_num,abs_num,dir;
	int  station_index = 0;

	//2004-05-22
	int  entry_count = 0;

	FILE* fp;
	char seps[]=" ,=\t\n", *ptr, *substr;
	char szFileName[MAX_PATH], strLine[256];
	sprintf(szFileName,"%s\\%s",m_szDataPath,cfgfile);
	fp = fopen(szFileName,"r");
	if (fp == NULL)
		return RET_FAILED;
	CAD_STATION *pstat=NULL;
	while(!feof(fp)) 
	{
		ptr = fgets(strLine, 256, fp);
		if (ptr == NULL) 
			continue;
		while (*ptr == ' ' || *ptr == '\t')
			ptr ++;
		if (*ptr == ';')	
			continue;
		if (*ptr == '\n')	
			continue;
		if(substr=strstr(ptr, "STATION_COUNT")) 
		{		
			strtok(substr, seps);
			//special_station_count = (int)atoi(strtok(NULL, seps));
		} 
		else if(substr=strstr(ptr, "STATION_ID")) 
		{
			entry_count = 0;
			sscanf(substr,"STATION_ID=%d, ENTRY_COUNT=%d\n",&station,&entrys);
			pstat=GetStationPtr(station);
			if(pstat)
			{
				pstat->nEntryCount=0;
			}
		} 
		else if(substr=strstr(ptr, "ENTRY")) 
		{
			sscanf(substr,"ENTRY=%d, ABS_NUM=%d, DIRECTION=%d\n",
				&entry_num,&abs_num,&dir);
			if(pstat && pstat->nEntryCount<MAX_ENTRY_COUNT)
			{
				pstat->entry[pstat->nEntryCount].entry_num=entry_num;
				pstat->entry[pstat->nEntryCount].abs_num = abs_num;
				pstat->entry[pstat->nEntryCount].dir = dir;
				pstat->nEntryCount++;
				SetAbsTimeStationInfo(abs_num,pstat->nStationNo,entry_num);
			}
		} 
		else continue;
	}

	fclose(fp);
	return TRUE;
}

#define GPC_ABS_LINE         ("[LINE_DEFINE]")
#define LOCO_NO_DISPLAY		 ("[LOCO_NO_DISPLAY]")

BOOL  CTGBasicParam::LoadAbsAttrib(const char *cfgfile)
{
	char *ptr, szFileName[128], strLine[512];
	FILE *fp;
	char *token;
	char seps[]=" ,\t\\/=\n";
	sprintf(szFileName,"%s\\%s",gpcdef.GetDataDir(),cfgfile);
	if((fp=fopen(szFileName, "r"))==NULL)
		return FALSE;

	int count=0;
	while (!feof(fp))
	{
		if(!(ptr = fgets(strLine, 511, fp)))
			break;
		if (*ptr == '[' && !strncmp(ptr, GPC_ABS_LINE, strlen(GPC_ABS_LINE)) )
		{//read route info 
			ptr = ptr + strlen(GPC_ABS_LINE);
			do
			{
				// load line number
				token = strtok(ptr, seps);
				if(token==NULL)
					break;
				long absnum=atoi(token);
				map<DWORD, DWORD>::iterator it=m_map_gpc_lineno.find(absnum);
				if(it==m_map_gpc_lineno.end())
					break;

				AbsAttrib  abs_data;
				memset(&abs_data,0,sizeof(abs_data));
				//line number
				abs_data.line_no=absnum;
				// load line name 
				token = strtok(NULL, seps);
				if(token==NULL) break;
				strncpy(abs_data.abs_name,token,sizeof(abs_data.abs_name)-1);
				// load abs no 
				token = strtok(NULL, seps);
				if(token==NULL) break;
				abs_data.abs_no = atoi(token);

				// load start station no 
				token = strtok(NULL, seps);
				if(token==NULL) break;
				abs_data.start_station = atoi(token);

				// load end station no 
				token = strtok(NULL, seps);
				if(token==NULL) break;
				abs_data.end_station = atoi(token);

				// load line direction
				token = strtok(NULL, seps);
				if(token==NULL) break;
				abs_data.direction = atoi(token);

				abs_data.cfgorder=count;
				count++;
				m_aAbsAttrib[absnum]=abs_data;
			}while(0);
		} // if

		else if (*ptr == '[' && !strncmp(ptr, LOCO_NO_DISPLAY, strlen(LOCO_NO_DISPLAY)) )
		{//read lanzhou loco plot
			ptr = ptr + strlen(LOCO_NO_DISPLAY);
			do
			{
				LocoNoDisplay disp;
				token = strtok(ptr, seps);
				if(token==NULL)
				{
					fclose(fp);
					return false;
				}
				disp.nSection = (int)atoi(token);

				token = strtok(NULL, seps);
				if(token==NULL)
				{
					fclose(fp);
					return false;
				}
				disp.nStation = (int)atoi(token);

				token = strtok(NULL, seps);
				if(token==NULL)
				{
					fclose(fp);
					return false;
				}
				disp.nArrivalIndex = (int)atoi(token);

				token = strtok(NULL, seps);
				if(token==NULL)
				{
					fclose(fp);
					return false;
				}
				disp.nDepartIndex = (int)atoi(token);

				token = strtok(NULL, seps);
				if(token==NULL)
				{
					fclose(fp);
					return false;
				}
				disp.nDirect = (int)atoi(token);

				token = strtok(NULL, seps);
				if(token==NULL)
				{
					fclose(fp);
					return false;
				}
				disp.nAddShow = (int)atoi(token);

				token = strtok(NULL, seps);
				if(token==NULL) 
				{
					fclose(fp);
					return false;
				}
				disp.nSubShow = (int)atoi(token);

				gpcdef.m_loco_no_display_list.push_back(disp);
			}while(0);
		} // if
	} // while
	fclose(fp);

	map<long,AbsAttrib>::iterator it= m_aAbsAttrib.begin();
	for(; it != m_aAbsAttrib.end(); it++)
	{
		GpcLogTrainMessage(0,0,"%d=%s(%d),%d,%d,%d",it->second.line_no,it->second.abs_name,it->second.abs_no,
			it->second.start_station,it->second.end_station,it->second.direction);
	}

	if(m_aAbsAttrib.size()>0)
		return TRUE;
	else
		return FALSE;
}

const CTGBasicParam::AbsAttrib *CTGBasicParam::GetAbsAttrib(long absnum)
{
	map<long,AbsAttrib>::const_iterator it=m_aAbsAttrib.find(absnum);
	if(it!=m_aAbsAttrib.end())
		return &it->second; 
	else
		return NULL;
}

BOOL  CTGBasicParam::ConvertStationEntryToAbsnum(ushort station, ushort entry_num,ushort *abs_num, ushort *dir)
{
	ushort usAbsNo,usDir;
	CAD_STATION *pstat=GetStationPtr(station);
	if(!pstat)
		return FALSE;
	int j;

	for (j=0;j<pstat->nEntryCount;j++)
		if (pstat->entry[j].entry_num==entry_num)
			break;
	// special_station 
	if (j >= pstat->nEntryCount)
		// find station record, but no such entryno
		return FALSE;
	else{
		usAbsNo = pstat->entry[j].abs_num;
		usDir = pstat->entry[j].dir;
	}

	if(abs_num != NULL)*abs_num = usAbsNo;
	if(dir != NULL)
		*dir = usDir;
	return TRUE;
}

BOOL  CTGBasicParam::ConvertAbsnumToStationEntry(ushort *station1, ushort *entry_num1,ushort *station2, ushort *entry_num2,ushort abs_num)
{
	BYTE   j;
	BOOL  rc=FALSE;
	IterMapStation iter=m_aCadStations.begin();
	CAD_STATION *pstat=NULL;
	*station1=0;
	*station2=0;
	while(!rc && iter!=m_aCadStations.end())
	{
		pstat=&iter->second;
		for (j=0;j<pstat->nEntryCount;j++)
			if (pstat->entry[j].abs_num==abs_num)
				break;
		if (j < pstat->nEntryCount)
		{
			if(*station1==0)
			{
				*station1=pstat->nStationNo;
				*entry_num1=pstat->entry[j].entry_num;
			}
			else
			{
				*station2=pstat->nStationNo;
				*entry_num2=pstat->entry[j].entry_num;
				rc=TRUE;
				break;
			}

		}
		iter++;
	}
	return rc;
}

BOOL  CTGBasicParam::GetTrainTypeDef(short  train_type,BYTE *run_type,BYTE *prio,BYTE *locotype)
{
	return m_cTrainTypeInfo.GetTrainTypeDef(train_type,run_type,prio,locotype);
}

CString RemoveChineseChar(CString str)
{
	CString ret;
		
	for(int i=0; i<str.GetLength(); i++)
	{
		char ch = str.GetAt(i);
		if(('0' <= ch && ch <='9') || ('A' <= ch && ch <= 'Z') || ('a' <= ch && ch <= 'z'))
		{
			ret += str.GetAt(i);
		}
	}

	return ret;
}

BOOL  CTGBasicParam::GetTrainPrio(const TRAIN_NO train_no,BYTE *type, BYTE *run_type, BYTE *prio)
{
	CString strTrainId = train_no;
	if(GetTGFunctionConfig()->IsRemoveTrainIDChineseChar())
	    strTrainId = RemoveChineseChar(train_no);
	if(strTrainId.IsEmpty())
		strTrainId=train_no;

	if(m_cTrainTypeInfo.GetPrioFromTrainId(strTrainId,type,run_type,prio))
		return TRUE;
	else
		return FALSE;
}

int   CTGBasicParam::CalculateTrainRunTime(STATION_NO station, ENTRY_ID entry, TRAIN_IN_OUT in_out, BYTE trn_type)
{
	int dir;
	dir=IsTrainDirectionDown(station, entry, in_out)?1:0;
	ushort abs_num,st_dir;
	if(ConvertStationEntryToAbsnum(station,entry,&abs_num,&st_dir))
	{
		TrainAbsTime *absptr=GetAbsTimePtr(abs_num,trn_type,dir);
		if(absptr)
			return absptr->run_time; 
	}
	//m_Task.SysLog("CalTrainRunTime invalid sta %d entry %d in_out %d train_type %d run_time %d", station, entry, in_out, trn_type, run_time);
	return  m_pConfig->GetDefaultRunTime();
}

int   CTGBasicParam::CalculateTrainStopExtra(STATION_NO station, ENTRY_ID entry, TRAIN_IN_OUT in_out, BYTE trn_type)
{
	short train_stop_extra =-1;
	int dir;;
	dir=IsTrainDirectionDown(station, entry, in_out)?1:0;
	ushort abs_num,st_dir;
	if(ConvertStationEntryToAbsnum(station,entry,&abs_num,&st_dir)) 
	{
		TrainAbsTime *absptr=GetAbsTimePtr(abs_num,trn_type,dir);
		if(absptr)
			train_stop_extra=absptr->stop_time;
	}
	if (train_stop_extra <= 0 || train_stop_extra > 30*60) 
	{
		//m_Task.ErrLog("CalculateTrainStopExtra invalid sta %d entry %d in_out %d train_type %d stopextra %d", station, entry, in_out, trn_type, train_stop_extra);
		train_stop_extra = m_pConfig->GetDefaultStopExtra();
	}
	return train_stop_extra;
}

int   CTGBasicParam::CalculateTrainStartExtra(STATION_NO station, ENTRY_ID entry, TRAIN_IN_OUT in_out, BYTE trn_type)
{
	short train_start_extra =-1;
	int dir;;
	dir=IsTrainDirectionDown(station, entry, in_out)?1:0;
	ushort abs_num,st_dir;
	if(ConvertStationEntryToAbsnum(station,entry,&abs_num,&st_dir)) 
	{
		TrainAbsTime *absptr=GetAbsTimePtr(abs_num,trn_type,dir);
		if(absptr)
			train_start_extra=absptr->start_time;
	}
	if (train_start_extra <= 0 || train_start_extra > 30*60) 
	{
		//m_Task.ErrLog("CalculateTrainStopExtra invalid sta %d entry %d in_out %d train_type %d stopextra %d", station, entry, in_out, trn_type, train_stop_extra);
		train_start_extra = m_pConfig->GetDefaultStartExtra();
	}
	return train_start_extra;
}

BOOL CTGBasicParam::IsAbsDirectionDown(STATION_NO station, ENTRY_ID entry) 
{
	// 20090918 XIAN 区间方向用 TG.DEF 中定义的区间默认方向，暂不用 StationSide.def 中定义的
	ABS_STRUCT_PTR ptr=NULL;
	ushort absnum1=0xffff,absnum2=0xffff;
	ushort dir1=0xffff,dir2=0xffff;

	if(!ConvertStationEntryToAbsnum(station,entry,&absnum1,&dir1))
		return FALSE;
	const AbsAttrib *pattr=GetAbsAttrib(absnum1);
	if(NULL != pattr)
	{
		//CString text;
		//text.Format("请注意: 在entry.cfg中能够找到车站%d的线号%d,但是在TG.def的[LINE_DEFINE]中找不到线号%d"，
		//	station，absnum1，absnum1);
		if(pattr->direction==1)
			return TRUE;
		else
			return FALSE;
	}

	return FALSE;


	/*
	const CStationDef* sta_def_ptr=GetStationDef(station);
	if (NULL == sta_def_ptr) 
	{
	return FALSE;
	}

	if (sta_def_ptr->isDownEntry(entry))
	return TRUE;
	else
	return FALSE;
	*/
}

BOOL CTGBasicParam::IsAbsDirectionUp(STATION_NO station, ENTRY_ID entry) 
{
	// 20090918 XIAN 区间方向用 TG.DEF 中定义的区间默认方向，暂不用 StationSide.def 中定义的
	ABS_STRUCT_PTR ptr=NULL;
	ushort absnum1=0xffff,absnum2=0xffff;
	ushort dir1=0xffff,dir2=0xffff;

	if(!ConvertStationEntryToAbsnum(station,entry,&absnum1,&dir1))
		return FALSE;
	const AbsAttrib *pattr=GetAbsAttrib(absnum1);
	if(NULL != pattr)
	{
		if(pattr->direction==0)
			return TRUE;
		else
			return FALSE;
	}

	return FALSE;
	/*
	const CStationDef* sta_def_ptr=GetStationDef(station);
	if (NULL == sta_def_ptr) 
	{
	return FALSE;
	}

	if (sta_def_ptr->isUpEntry(entry))
	return TRUE;
	else
	return FALSE;*/
}
CString CTGBasicParam::GetAbsDirectInfo(const TG_TrainRecord &rec, TRAIN_IN_OUT in_out)
{
	ABS_STRUCT_PTR ptr=NULL;
	ushort absnum1=0xffff,absnum2=0xffff;
	ushort dir1=0xffff,dir2=0xffff;
	CString text;
	if(in_out==TRAIN_OUT_STATION)
	{
		if(!ConvertStationEntryToAbsnum(rec.station, rec.exit, &absnum1,&dir1))
		{
			text.Format("在配置文件entry.cfg中找不到车站%d,出口%d对应的线号", rec.station, rec.exit);
			return text;
		}
	}
	else
	{
		if(!ConvertStationEntryToAbsnum(rec.station, rec.entry, &absnum1,&dir1))
		{
			text.Format("在配置文件entry.cfg中找不到车站%d,入口%d对应的线号", rec.station, rec.entry);
			return text;
		}
	}

	const AbsAttrib *pattr=GetAbsAttrib(absnum1);
	if(NULL == pattr)
	{
		text.Format("在配置文件TG.def中找不到车站%d的线号%d", rec.station, absnum1);
		return text;
	}

	text.Format("在配置文件TG.def中车站%d的线号%d的配置方向为%d.(0:上行区间 1:下行区间 2:双向)", rec.station, absnum1,pattr->direction);
	return text;
}

BOOL  CTGBasicParam::IsAbsDirectionDown(const TG_TrainRecord &rec, TRAIN_IN_OUT in_out)
{
	if(in_out==TRAIN_OUT_STATION)
		return IsAbsDirectionDown(rec.station,rec.exit);
	else
		return IsAbsDirectionDown(rec.station,rec.entry);
}
BOOL  CTGBasicParam::IsAbsDirectionUp(const TG_TrainRecord &rec, TRAIN_IN_OUT in_out)
{
	if(in_out==TRAIN_OUT_STATION)
		return IsAbsDirectionUp(rec.station,rec.exit);
	else
		return IsAbsDirectionUp(rec.station,rec.entry);
}

CString CTGBasicParam::GetTrainDirectInfo(const TG_TrainRecord &rec, TRAIN_IN_OUT in_out)
{
	CString text, tmp;
	const CStationDef* sta_def_ptr=GetStationDef(rec.station);
	if (NULL == sta_def_ptr) 
	{
		text.Format("在StationSide.def中找不到车站%d的定义", rec.station);
		return text;
	}

	int entry = 0;
	if(in_out==TRAIN_OUT_STATION)
	{
		entry = rec.exit;
		text.Format("车站%d的出口%d在StationSide.def中定义的方向为: ", rec.station, entry);
	}
	else
	{
		entry = rec.entry;
		text.Format("车站%d的入口%d在StationSide.def中定义的方向为: ", rec.station, entry);
	}

	tmp.Format("%d (1,2:出口上行, 入口下行, 34:出口下行, 入口上行,其他:双行)", sta_def_ptr->GetEntryDir(entry));

	return  text+tmp;
}

BOOL CTGBasicParam::IsTrainDirectionDown(const TG_TrainRecord &rec, TRAIN_IN_OUT in_out) 
{
	if(in_out==TRAIN_OUT_STATION)
		return IsTrainDirectionDown(rec.station,rec.exit,in_out);
	else
		return IsTrainDirectionDown(rec.station,rec.entry,in_out);
}

BOOL CTGBasicParam::IsTrainDirectionUp(const TG_TrainRecord &rec, TRAIN_IN_OUT in_out) 
{
	if(in_out==TRAIN_OUT_STATION)
		return IsTrainDirectionUp(rec.station,rec.exit,in_out);
	else
		return IsTrainDirectionUp(rec.station,rec.entry,in_out);
}



BOOL CTGBasicParam::IsTrainDirectionDown(STATION_NO station, ENTRY_ID entry, TRAIN_IN_OUT in_out) 
{
	BYTE dir=GetTrainDirection(station, entry, in_out);
	if (dir == 1)
		return TRUE;
	else
		return FALSE;
}

BOOL CTGBasicParam::IsTrainDirectionUp(STATION_NO station, ENTRY_ID entry, TRAIN_IN_OUT in_out) 
{
	BYTE dir=GetTrainDirection(station, entry, in_out);
	if (dir == 0)
		return TRUE;
	else
		return FALSE;
}


BOOL CTGBasicParam::IsStopSide(const TG_TrainRecord &rec) 
{
	STATION_NO station = rec.station;

	SIDE_ID arrive_side = rec.arrive_side;
	SIDE_ID depart_side = rec.depart_side;
	const CStationDef* sta_def_ptr = GetStationDef(station);
	if (NULL == sta_def_ptr)
	{
		//m_Task.ErrLog("IsStopSide sta %d arrive_side %d depart_side %d get_station_def invalid", station, arrive_side, depart_side);
		return FALSE;		
	}

	if (TG_IsFlagStart(rec.flag)) 
	{
		return IsStopSide(station, depart_side, rec.exit);
	}
	else if (TG_IsFlagTerminal(rec.flag)) 
	{
		return IsStopSide(station, arrive_side, rec.entry);
	}
	else if (arrive_side == depart_side)
	{
		if (!VerifyStationSide(station, arrive_side))
		{
			//m_Task.ErrLog("IsStopSide sta %d arrive_side %d invalid", station, arrive_side);
			return FALSE;
		}

		if (sta_def_ptr->isStopSide(arrive_side, rec.entry, rec.exit))
			return TRUE;
	}
	else 
	{
		if (IsStopSide(station, depart_side, rec.exit) && IsStopSide(station, arrive_side, rec.entry))
		{
			return TRUE;
		}
	}

	return FALSE;
}

BOOL  CTGBasicParam::IsStopSide(STATION_NO station, SIDE_ID side, ENTRY_ID entry,ENTRY_ID exit)
{
	const CStationDef* sta_def_ptr=GetStationDef(station);
	if (NULL == sta_def_ptr) 
		return FALSE;
	if(sta_def_ptr->isStopSide(side,entry,exit))
		return TRUE;
	else
		return FALSE;
}
BOOL  CTGBasicParam::IsStopSide(STATION_NO station, SIDE_ID side, ENTRY_ID entry) 
{
	const CStationDef* sta_def_ptr = GetStationDef(station);
	if (NULL == sta_def_ptr)
	{

		return FALSE;		
	}
	if (!VerifyStationSide(station, side))
	{
		//m_Task.ErrLog("IsStopSide sta %d side %d invalid", station, side);
		return FALSE;
	}
	if (!VerifyStationEntry(station, entry))
	{
		//m_Task.ErrLog("IsStopSide sta %d entry %d invalid", station, entry);
		return FALSE;
	}

	if (!sta_def_ptr->isStopSide(side, entry))
		return TRUE;

	return FALSE;
}
BOOL  CTGBasicParam::IsMainSide(STATION_NO station, SIDE_ID side, ENTRY_ID entry,ENTRY_ID exit)
{
	const CStationDef* sta_def_ptr=GetStationDef(station);
	if (NULL == sta_def_ptr) 
		return FALSE;
	if(sta_def_ptr->isMainSide(side,entry,exit))
		return TRUE;
	else
		return FALSE;
}

BOOL CTGBasicParam::IsMainSide(const TG_TrainRecord &rec) 
{
	STATION_NO station = rec.station;

	if (rec.arrive_side != rec.depart_side) 
	{
		//m_Task.ErrLog("IsMainSide sta %d arrive_side %d depart_side %d invalid", station, rec.arrive_side, rec.depart_side);
		return FALSE;
	}

	SIDE_ID side = rec.arrive_side;
	if (!VerifyStationSide(station, side)) 
	{
		//m_Task.ErrLog("IsMainSide sta %d side %d invalid", station, side);
		return FALSE;
	}
	const CStationDef* sta_def_ptr=GetStationDef(station);
	if (NULL == sta_def_ptr)
	{
		//m_Task.ErrLog("IsMainSide sta %d side %d get_station_def invalid", station, side);
		return FALSE;
	}

	if (TG_IsFlagStart(rec.flag)) 
	{
		if (sta_def_ptr->isMainSide(side, rec.exit))
			return TRUE;
	} 
	else if (TG_IsFlagTerminal(rec.flag)) 
	{
		if (sta_def_ptr->isMainSide(side, rec.entry))
			return TRUE;
	} 
	else 
	{
		if (sta_def_ptr->isMainSide(side, rec.entry, rec.exit))
			return TRUE;
	}

	return FALSE;	
}

const CStationDef*  CTGBasicParam::GetStationDef(STATION_NO sta) 
{
	const CAD_STATION *pstat=GetStationPtr(sta);
	if(!pstat)
		return NULL;
	return pstat->pStationDef;
}

// 方向
BYTE CTGBasicParam::GetTrainDirection(STATION_NO station, ENTRY_ID entry, TRAIN_IN_OUT in_out) 
{
	const CStationDef* sta_def_ptr=GetStationDef(station);
	if (NULL != sta_def_ptr) 
	{
		switch(sta_def_ptr->GetEntryDir(entry))
		{
		case 1: // 出口上, 入口下
		case 2:
			if (TRAIN_INTO_STATION == in_out)
				return 1;
			else 
				return 0;

		case 3: // 出口下, 入口上
		case 4:
			if (TRAIN_INTO_STATION == in_out)
				return 0;
			else 
				return 1;

		case 5:
			return 0xFF;

		default:
			break;
		}

	}
	return  0xFF;
}

SIDE_ID CTGBasicParam::GetMainSide(const TG_TrainRecord &rec)
{
	return GetMainSide(rec.station,rec.entry,rec.exit);
}

SIDE_ID CTGBasicParam::GetMainSide(STATION_NO station, ENTRY_ID entry, ENTRY_ID exit) 
{
	const CStationDef* sta_def = GetStationDef(station);
	if (NULL == sta_def) 
	{
		GpcLogTrainMessage(0,0,"get main side err sta %d entry %d exit %d", station, entry, exit);
		return NULL_SIDE;
	}

	short index = 0;
	SIDE_ID side;
	while (sta_def->GetReceDeptSideNo(index, side))
	{
		if (sta_def->isMainSide(side, (char)entry, (char)exit)) // 重点测试 
			return (SIDE_ID)side;
	}

	return NULL_SIDE;
}

BOOL  CTGBasicParam::GetStopOrderSide(const TG_TrainInfo &coninfo, TG_TrainRecord &rec, SIDE_ID* side, short *side_count)
{
	short max_side_count = *side_count;
	*side_count = 0;
	int station=rec.station;

	const CStationDef* sta_def = GetStationDef(station);
	if (NULL == sta_def) 
	{
		GpcLogTrainMessage(0,0,"can't find station %d form stationSide.def", rec.station);
		return FALSE;
	}
	
	SIDE_ID main_side = GetMainSide(rec);	
	if (NULL_SIDE == main_side)
	{
		GpcLogTrainMessage(0,0, "can't get main side,for sta %d entry %d exit %d", rec.station, rec.entry, rec.exit);
		return FALSE;
	}

	bool bSelBackupSideByCfgQueue=GetTGFunctionConfig()->IsSelBackupSideByCfgQueue();
	SIDE_NO other_main_side[MAX_BACKUP_SIDE_NUM];
	short other_main_side_count = 0;

	SIDE_NO backup[MAX_BACKUP_SIDE_NUM];
	short count = sta_def->GetBackupSideInGroupByMainSide(0, main_side, backup);
	if (count > 0 && count < MAX_BACKUP_SIDE_NUM)
	{
		if (1 == count && backup[0] == main_side)
		{
			// use the main side to stop
			side[*side_count] = backup[0];
			*side_count = 1;
		}
		else 
		{
			for(int i=0; i<count; i++)
			{
				// except main side
				if (sta_def->isMainSide(backup[i], rec.entry, rec.exit))
				{
					GpcLogTrainMessage(0,0, "train %s station %d ,side %d is main side,entry %d exit %d", 
						coninfo.train_name, rec.station, backup[i], rec.entry, rec.exit);
					continue;
				}
				// if connect to entry
				if (IS_VALID_ENTRY(rec.entry) && !sta_def->isEntrySideConnect(backup[i], rec.entry))
				{
					GpcLogTrainMessage(0,0, "train %s station %d, side %d isn't connect entry %d",
						coninfo.train_name, rec.station, backup[i], rec.entry);
					continue;
				}
				// if connect to entry
				if (IS_VALID_ENTRY(rec.exit)  && !sta_def->isEntrySideConnect(backup[i], rec.exit))
				{
					GpcLogTrainMessage(0,0, "train %s station %d, side %d isn't connect exit %d",
						coninfo.train_name, rec.station, backup[i], rec.exit);
					continue;
				}

				// except cannot used side
				if (!check_side_utility(sta_def, coninfo, rec, backup[i]))
				{
					GpcLogTrainMessage(0,0, "train %s station %d, side %d isn't use",
						coninfo.train_name, rec.station,backup[i]);
					continue;
				}

				// keep main side of other entry
				if (sta_def->isMainSide(backup[i]) && other_main_side_count < MAX_BACKUP_SIDE_NUM)
				{
					other_main_side[other_main_side_count] = backup[i];
					other_main_side_count++;
					
					if(!bSelBackupSideByCfgQueue)
						continue;
				}

				side[*side_count] = backup[i];
				(*side_count)++;

				if (*side_count >= max_side_count) break;
			}
		}
	}

	if(!bSelBackupSideByCfgQueue)
	{
		int tmp_side_count=*side_count;
		for(int j=tmp_side_count; j<max_side_count && j<other_main_side_count+tmp_side_count; j++)
		{
			side[j] = other_main_side[j-tmp_side_count];
			(*side_count) ++;
		}
	}
	
	//for(int j=*side_count; j<max_side_count && j<other_main_side_count+(*side_count); j++)
	//{
	//	side[j] = other_main_side[j-*side_count];
	//	(*side_count) ++;
	//}

	if (*side_count)
		return TRUE;
	else
	{
		GpcLogTrainMessage(0,0, "train %s station %d, GetBackupSideInGroupByMainSide %d",coninfo.train_name, rec.station,count);
		return FALSE;
	}
}

BOOL  CTGBasicParam::check_side_utility(const CStationDef* sta_def,const TG_TrainInfo &coninfo,const  TG_TrainRecord &rec, SIDE_ID side) 
{
	if (!TG_IsFlagStart(rec.flag)) 
	{
		if (!verify_side_train_arrive_direction(sta_def, rec, side))
		{
			GpcLogTrainMessage(0,0, "trainid %s station %d side %d ,verify_side_train_arrive_direction isn't pass",
				coninfo.train_name, rec.station, side);
			return FALSE;
		}

		if (!verify_side_train_arrive_type(sta_def, rec, side, coninfo.type, coninfo.run_type))
		{
			GpcLogTrainMessage(0,0, "trainid %s station %d side %d ,verify_side_train_arrive_type isn't pass",
				coninfo.train_name, rec.station, side);
			return FALSE;
		}
	}

	if (!TG_IsFlagTerminal(rec.flag)) 
	{
		if (!verify_side_train_depart_direction(sta_def, rec, side))
		{
			GpcLogTrainMessage(0,0, "trainid %s station %d side %d ,verify_side_train_depart_direction isn't pass",
				coninfo.train_name, rec.station, side);
			return FALSE;
		}

		if (!verify_side_train_depart_type(sta_def, rec, side, coninfo.type, coninfo.run_type))
		{
			GpcLogTrainMessage(0,0, "trainid %s station %d side %d ,verify_side_train_depart_type isn't pass",
				coninfo.train_name, rec.station, side);
			return FALSE;
		}
	}

	if (!verify_side_train_length(sta_def, rec, side, coninfo.train_length, coninfo.length_rank))
	{
		GpcLogTrainMessage(0,0, "trainid %s station %d side %d ,verify_side_train_length isn't pass",
			coninfo.train_name, rec.station, side);
		return FALSE;
	}

	if (!verify_side_train_space(sta_def, rec, side, coninfo.space_rank)) 
	{
		GpcLogTrainMessage(0,0, "trainid %s station %d side %d ,verify_side_train_space isn't pass",
			coninfo.train_name, rec.station, side);
		return FALSE;
	}
	// NA??? 是否在TrainInfo中增加电力标志？
	//if (!verify_side_train_elec(sta_def, rec, side, coninfo.engine_count, coninfo.engine_type))
	//	return FALSE;

	if (!verify_side_train_equipment(sta_def, rec, side, coninfo))
	{
		GpcLogTrainMessage(0,0, "trainid %s station %d side %d ,verify_side_train_equipment isn't pass",
			coninfo.train_name, rec.station, side);
		return FALSE;
	}

	if (!verify_side_start_difficult(sta_def, rec, side, coninfo))
	{
		GpcLogTrainMessage(0,0, "trainid %s station %d side %d ,verify_side_start_difficult isn't pass",
			coninfo.train_name, rec.station, side);
		return FALSE;
	}

	if (!verify_side_not_stop_crh(sta_def, side, coninfo))
	{
		GpcLogTrainMessage(0,0, "trainid %s station %d side %d ,verify_side_not_stop_crh isn't pass",
			coninfo.train_name, rec.station, side);
		return FALSE;
	}
	
	return TRUE;
}

BOOL  CTGBasicParam::verify_side_train_arrive_direction(const CStationDef* sta_def_ptr,const TG_TrainRecord &rec, SIDE_ID side)
{
	if (!VerifyStationEntry(rec.station, rec.entry))
		return FALSE;

	if (IsTrainDirectionDown(rec, TRAIN_INTO_STATION)) 
	{
		if (sta_def_ptr->isDownReceTrain(side))
			return TRUE;
	} 
	else 
	{
		if (sta_def_ptr->isUpReceTrain(side))
			return TRUE;
	}

	return FALSE;
}
BOOL  CTGBasicParam::verify_side_train_depart_direction(const CStationDef* sta_def_ptr, const TG_TrainRecord &rec, SIDE_ID side)
{
	if (!VerifyStationEntry(rec.station, rec.exit))
		return FALSE;

	if (IsTrainDirectionDown(rec, TRAIN_OUT_STATION)) 
	{
		if (sta_def_ptr->isDownDepartTrain(side))
			return TRUE;
	} 
	else 
	{
		if (sta_def_ptr->isUpDepartTrain(side))
			return TRUE;
	}

	return FALSE;		
}
BOOL  CTGBasicParam::verify_side_train_arrive_type(const CStationDef* sta_def_ptr, const TG_TrainRecord &rec, SIDE_ID side, BYTE type, BYTE run_type)
{
	CStationDef::RECEIVE_TRAIN_TYPE t = CStationDef::RECV_PASSENGER;
	if (TG_IsFreightTrainType(type))
		t = CStationDef::RECV_CARGO;

	if (sta_def_ptr->isReceiveTrain(side, t))
		return TRUE;

	return FALSE;
}
BOOL  CTGBasicParam::verify_side_train_depart_type(const CStationDef* sta_def_ptr, const TG_TrainRecord &rec, SIDE_ID side, BYTE type, BYTE run_type)
{
	CStationDef::RECEIVE_TRAIN_TYPE t = CStationDef::RECV_PASSENGER;
	if (TG_IsFreightTrainType(type))
		t = CStationDef::RECV_CARGO;

	if (sta_def_ptr->isDepartTrain(side, t))
		return TRUE;

	return FALSE;
}
BOOL  CTGBasicParam::verify_side_train_length(const CStationDef* sta_def_ptr,const TG_TrainRecord &rec, SIDE_ID side, USHORT length, BYTE length_rank)
{
	if (sta_def_ptr->isReceiveLengthTrain(side, length))
		return TRUE;

	return FALSE;
}
BOOL  CTGBasicParam::verify_side_train_space(const CStationDef* sta_def_ptr, const TG_TrainRecord &rec, SIDE_ID side, BYTE space_rank)
{
	// not implement now, need to make sure true factors.
	if (0xFF == space_rank || 0 == space_rank)
		return TRUE;

	if (sta_def_ptr->isReceiveExceedTrain(side, space_rank)) 
		return TRUE;

	return FALSE;
}
BOOL  CTGBasicParam::verify_side_train_elec(const CStationDef* sta_def_ptr,const  TG_TrainRecord &rec, SIDE_ID side, BYTE engine_count, const BYTE *engine_type)
{
	short need_elec=FALSE;
	for(int i=0; i<engine_count; i++) 
	{
		if (TG_IsTractionEngineExt(engine_type[i]) && TG_IsElecEngineExt(engine_type[i]))
			need_elec=TRUE;
	}
	if (!need_elec)
		return TRUE;

	if (sta_def_ptr->isCatenary(side))
		return TRUE;

	return FALSE;
}
BOOL  CTGBasicParam::verify_side_train_equipment(const CStationDef* sta_def_ptr,const TG_TrainRecord &rec, SIDE_ID side,const  TG_TrainInfo &coninfo)
{
	if (TG_IsTrainPass(rec) || TG_IsTechStopTrain(rec))
		return TRUE;
	
	if (TG_IsPassengerTrain(&coninfo)) 
	{
		if (sta_def_ptr->isPassenerEquipment(side))
			return TRUE;
	}
	else 
	{
		return TRUE;
	}

	GpcLogTrainMessage(0,0, "客车 非技术停点 没有客运设备");
	return FALSE;
}
BOOL  CTGBasicParam::verify_side_start_difficult(const CStationDef* sta_def_ptr,const  TG_TrainRecord &rec, SIDE_ID side,const TG_TrainInfo &coninfo)
{
	if (TG_IsTrainPass(rec))
		return TRUE;

	STATION_NO sta = rec.station;
	if (IS_VALID_ENTRY(rec.entry)) 
	{
		if (!VerifyStationEntry(sta, rec.entry))
			return TRUE;

		if (IsTrainDirectionDown(rec, TRAIN_INTO_STATION)) 
		{
			if (sta_def_ptr->isDownStartDifficuent(side, coninfo.train_weight))
				return FALSE;
		} 
		else 
		{
			if (sta_def_ptr->isUpStartDifficuent(side, coninfo.train_weight))
				return FALSE;			
		}
	}
	if (IS_VALID_ENTRY(rec.exit) )
	{
		if (!VerifyStationEntry(sta, rec.exit))
			return FALSE;

		if (IsTrainDirectionDown(rec, TRAIN_OUT_STATION)) 
		{
			if (sta_def_ptr->isDownStartDifficuent(side, coninfo.train_weight))
				return FALSE;
		}
		else 
		{
			if (sta_def_ptr->isUpStartDifficuent(side, coninfo.train_weight))
				return FALSE;			
		}
	}

	return TRUE;	
}
BOOL  CTGBasicParam::VerifySideTrainArriveDirection(const TG_TrainRecord &rec)
{
	if (!IS_VALID_ENTRY(rec.entry))
		return TRUE;

	STATION_NO sta = rec.station;
	const CStationDef* sta_def_ptr=GetStationDef(sta);
	if (NULL == sta_def_ptr) 
	{
		//		m_Task.ErrLog("get sta %d sta_def err", sta);
		return FALSE;
	}

	return verify_side_train_arrive_direction(sta_def_ptr, rec, rec.arrive_side);
}
BOOL  CTGBasicParam::VerifySideTrainDepartDirection(const TG_TrainRecord &rec)
{
	if (!IS_VALID_ENTRY(rec.exit) )
		return TRUE;

	STATION_NO sta = rec.station;
	const CStationDef* sta_def_ptr=GetStationDef(sta);
	if (NULL == sta_def_ptr) 
	{
		//m_Task.ErrLog("get sta %d sta_def err", sta);
		return FALSE;
	}

	return verify_side_train_depart_direction(sta_def_ptr, rec, rec.depart_side);
}
BOOL  CTGBasicParam::VerifySideTrainArriveType(const TG_TrainRecord &rec, BYTE type, BYTE run_type)
{
	STATION_NO sta = rec.station;
	const CStationDef* sta_def_ptr=GetStationDef(sta);
	if (NULL == sta_def_ptr) 
	{
		//m_Task.ErrLog("get sta %d sta_def err", sta);
		return FALSE;
	}
	return verify_side_train_arrive_type(sta_def_ptr, rec, rec.arrive_side, type, run_type);
}
BOOL  CTGBasicParam::VerifySideTrainDepartType(const TG_TrainRecord &rec, BYTE type, BYTE run_type)
{
	STATION_NO sta = rec.station;
	const CStationDef* sta_def_ptr=GetStationDef(sta);
	if (NULL == sta_def_ptr) 
	{
		//m_Task.ErrLog("get sta %d sta_def err", sta);
		return FALSE;
	}
	return verify_side_train_depart_type(sta_def_ptr, rec, rec.depart_side, type, run_type);
}

BOOL  CTGBasicParam::VerifySideTrainLength(const TG_TrainRecord &rec, USHORT length, BYTE length_rank)
{
	STATION_NO sta = rec.station;
	const CStationDef* sta_def_ptr=GetStationDef(sta);
	if (NULL == sta_def_ptr) 
	{
		//m_Task.ErrLog("get sta %d sta_def err", sta);
		return TRUE;
	}

	if (!verify_side_train_length(sta_def_ptr, rec, rec.arrive_side, length, length_rank))
		return FALSE;

	if (rec.arrive_side == rec.depart_side) return TRUE;
	return verify_side_train_length(sta_def_ptr, rec, rec.depart_side, length, length_rank);
}
BOOL  CTGBasicParam::VerifySideTrainSpace(const TG_TrainRecord &rec, BYTE space_rank)
{
	STATION_NO sta = rec.station;
	const CStationDef* sta_def_ptr=GetStationDef(sta);
	if (NULL == sta_def_ptr) 
	{
		//m_Task.ErrLog("get sta %d sta_def err", sta);
		return TRUE;
	}

	if (!verify_side_train_space(sta_def_ptr, rec, rec.arrive_side, space_rank))
		return FALSE;

	if (rec.arrive_side == rec.depart_side) return TRUE;
	return verify_side_train_space(sta_def_ptr, rec, rec.depart_side, space_rank);
}
BOOL  CTGBasicParam::VerifySideTrainElec(const TG_TrainRecord &rec, BYTE engine_count,const  BYTE *engine_type)
{
	STATION_NO sta = rec.station;
	const CStationDef* sta_def_ptr=GetStationDef(sta);
	if (NULL == sta_def_ptr) 
	{
		//m_Task.ErrLog("get sta %d sta_def err", sta);
		return TRUE;
	}

	if (!verify_side_train_elec(sta_def_ptr, rec, rec.arrive_side, engine_count, engine_type))
		return FALSE;

	if (rec.arrive_side == rec.depart_side) return TRUE;
	return verify_side_train_elec(sta_def_ptr, rec, rec.depart_side, engine_count, engine_type);
}
BOOL  CTGBasicParam::VerifySideTrainEquipment(const TG_TrainRecord &rec, const TG_TrainInfo &coninfo)
{
	STATION_NO sta = rec.station;
	const CStationDef* sta_def_ptr=GetStationDef(sta);
	if (NULL == sta_def_ptr) 
	{
		//m_Task.ErrLog("get sta %d sta_def err", sta);
		return FALSE;
	}

	if (!verify_side_train_equipment(sta_def_ptr, rec, rec.arrive_side, coninfo))
		return FALSE;

	if (rec.arrive_side == rec.depart_side) return TRUE;
	return verify_side_train_equipment(sta_def_ptr, rec, rec.depart_side, coninfo);
}
BOOL  CTGBasicParam::VerifySideStartDifficult(const TG_TrainRecord &rec, const TG_TrainInfo &coninfo) 
{
	STATION_NO sta = rec.station;
	const CStationDef* sta_def_ptr=GetStationDef(sta);
	if (NULL == sta_def_ptr) 
	{
		//m_Task.ErrLog("get sta %d sta_def err", sta);
		return TRUE;
	}

	if (!verify_side_start_difficult(sta_def_ptr, rec, rec.arrive_side, coninfo))
		return FALSE;

	if (rec.arrive_side == rec.depart_side) return TRUE;
	return verify_side_start_difficult(sta_def_ptr, rec, rec.depart_side, coninfo);
}

BOOL  CTGBasicParam::VerifyArriveDepartTime(TIME ltime, long diff) 
{
	if (ltime < 1000000000) return FALSE;

	if (0 == diff ) return TRUE;

	TIME ttime = GetCurrentTime();
	if (abs (ltime - ttime) > diff) return FALSE;

	return TRUE;
}

BOOL  CTGBasicParam::VerifyStationNumber(STATION_NO sta) 
{
	CAD_STATION *pstat=GetStationPtr(sta);
	if(!pstat)
		return FALSE;
	else
		return TRUE;
}
BOOL  CTGBasicParam::VerifyStationEntry(STATION_NO sta, ENTRY_ID entry) 
{
	const CStationDef* sta_def_ptr = GetStationDef(sta);
	if (NULL == sta_def_ptr) 
	{
		//m_Task.ErrLog("VerifyStationSide get sta_def invalid sta %d entry %d", sta, entry);
		return FALSE;
	}

	if (sta_def_ptr->IsStationEntry(entry))
		return TRUE;
	else
		return FALSE;
}
BOOL  CTGBasicParam::VerifyStationSide(STATION_NO sta, SIDE_ID side)  
{
	const CStationDef* sta_def_ptr = GetStationDef(sta);
	if (NULL == sta_def_ptr) 
	{
		//m_Task.ErrLog("VerifyStationSide get sta_def invalid sta %d side %d", sta, side);
		return FALSE;
	}

	if (sta_def_ptr->isReceDeptSide(side) || sta_def_ptr->isMainSide(side))
		return TRUE;

	return FALSE;
}
BOOL  CTGBasicParam::VerifyStationAbs(STATION_NO sta, int abs_number)  
{
	const CAD_STATION *cad_sta = GetStationPtr(sta);
	if (NULL == cad_sta)
		return FALSE;

	for(int i=0; i<cad_sta->nEntryCount; i++) 
	{
		if (cad_sta->entry[i].abs_num == abs_number)
			return TRUE;

	}
	return FALSE;
}
BOOL  CTGBasicParam::VerifyEntryAndSide(STATION_NO sta, ENTRY_ID entry, SIDE_ID side)  
{
	const CStationDef* sta_def_ptr = GetStationDef(sta);
	if (NULL == sta_def_ptr) 
	{
		//m_Task.ErrLog("VerifyEntryAndSide get sta_def invalid sta %d entry %d side %d", sta,entry, side);
		return FALSE;
	}

	if (!VerifyStationEntry(sta, entry) || !VerifyStationSide(sta, side))
		return FALSE;

	if (sta_def_ptr->isEntrySideConnect(side, entry))
		return TRUE;

	return FALSE;
}
BOOL  CTGBasicParam::VerifyEntryAndAbs(STATION_NO prev_sta, ENTRY_ID prev_entry, STATION_NO next_sta, ENTRY_ID next_entry)  
{
	if (!VerifyStationEntry(prev_sta, prev_entry) || !VerifyStationEntry(next_sta, next_entry))
		return FALSE;

	ushort abs_num,abs_num2;
	ushort dir,dir2;
	if(!ConvertStationEntryToAbsnum(prev_sta,prev_entry,&abs_num,&dir))
		return FALSE;
	if(!ConvertStationEntryToAbsnum(next_sta,next_entry,&abs_num2,&dir2))
		return FALSE;
	if(abs_num==abs_num2)
		return TRUE;
	else
		return FALSE;

	return FALSE;	
}



BOOL CTGBasicParam::GetStationList(int& stationNum, int* stationList)
{
	int count=m_aCadStations.size();
	if(count>stationNum)
		count=stationNum;
	IterMapStation iter = m_aCadStations.begin();
	int i;
	for(i=0;i<count;i++)
	{
		if(iter==m_aCadStations.end())
			break;
		stationList[i]=iter->second.nStationNo;
		iter++;
	}
	stationNum=i;
	return TRUE;
}

BOOL  CTGBasicParam::SetAbsTimeStationInfo(int absnumber,int nStationNo,int entry_num)
{
	for(int type=0;type<MAX_TRAIN_RUN_TYPE;type++) 
	{
		for(int dir=0;dir<2;dir++)
		{
			TrainAbsTime *absptr=GetAbsTimePtr(absnumber,type,dir);
			if(absptr)
			{
				if(absptr->station1==-1)
				{
					absptr->station1=nStationNo;
					absptr->entry1=entry_num;
				}
				else if(absptr->station2==-1)
				{
					absptr->station2=nStationNo;
					absptr->entry2=entry_num;
				}
			}
		}
	}
	return TRUE;
}

CString CTGBasicParam::GetPasswayName(int con_id, int passway_id)
{
	CSingle_Passway SinglePassway;
	if(!m_cPasswayInfo.GetSinglePassway(con_id,passway_id,SinglePassway))
		return "";

	return SinglePassway.Passway_Name.c_str();
}

BOOL CTGBasicParam::GetPasswayFlag(int con_id,int iRouteNo,int *inflag,int *outflag)  //  
{
	CSingle_Passway SinglePassway;
	if(!m_cPasswayInfo.GetSinglePassway(con_id,iRouteNo,SinglePassway))
		return FALSE;
	int rec_count=SinglePassway.m_vector_Passway_Sta_info.size();
	if(rec_count<=0)
		return FALSE;
	const Passway_Sta &p_sta= SinglePassway.m_vector_Passway_Sta_info[0];
	if(p_sta.Entry<=0)
		*inflag=SCHD_START;
	else
		*inflag=0;
	const Passway_Sta &p_sta2= SinglePassway.m_vector_Passway_Sta_info[rec_count-1];
	if(p_sta2.Exit<=0)
		*outflag=SCHD_TERMINAL;
	else
		*outflag=0;
	return TRUE;
}

BOOL CTGBasicParam::GetPasswaySta(int con_id,int iRouteNo,TG_PassWaySta sta[],int *limit)  // return 实际数目
{
	CSingle_Passway SinglePassway;
	if(!m_cPasswayInfo.GetSinglePassway(con_id,iRouteNo,SinglePassway))
		return FALSE;
	int rec_count=SinglePassway.m_vector_Passway_Sta_info.size();
	if(rec_count<=0)
		return FALSE;
	for(int i=0;i<rec_count;i++)
	{
		const Passway_Sta &p_sta= SinglePassway.m_vector_Passway_Sta_info[i];
		sta[i].Pass_way_id =iRouteNo;
		sta[i].Entry =p_sta.Entry;
		sta[i].Exit =p_sta.Exit;
		sta[i].Station =p_sta.Sta_id; 
		sta[i].in_sta_idx =p_sta.in_sta_index;
		sta[i].out_sta_idx =p_sta.out_sta_index;
		//sta[i].ArriveSide=p_sta.Arrive_Side; 
		//sta[i].DepartSide=p_sta.Depart_Side; 
	}
	*limit=rec_count;
	return TRUE;
}


BOOL CTGBasicParam::GetPasswayInfo(int con_id,TG_PassWayInfo info[],int *limit)  // return 实际数目
{
	CConsole_Passway Console_Passway;
	if(!m_cPasswayInfo.GetConsolePassway(con_id,Console_Passway))
		return FALSE;
	int max_num = *limit;
	*limit=0;
	if(max_num < Console_Passway.m_map_Console_Passway.size())
	{
		AfxMessageBox("Passway数目太多不能正确获取");
		return FALSE;
	}

	std::map<short,CSingle_Passway>::const_iterator it = Console_Passway.m_map_Console_Passway.begin();
	int i=0;
	for( ;it != Console_Passway.m_map_Console_Passway.end(); it++,i++)
	{
		info[i].Console_id=con_id;
		info[i].Pass_way_id = it->second.Passway_id;
		strncpy(info[i].Pass_way_name,it->second.Passway_Name.c_str(),sizeof(info[i].Pass_way_name)-1);  
		int nStaCount = it->second.m_vector_Passway_Sta_info.size();
		if(nStaCount > 0)
		{
			info[i].Begin_sta = it->second.m_vector_Passway_Sta_info[0].Sta_id;
			info[i].End_sta = it->second.m_vector_Passway_Sta_info[nStaCount-1].Sta_id; 
		}
		else
		{
			info[i].Begin_sta=0;
			info[i].End_sta=0; 
		}
	}
	*limit=i;
	return TRUE;
}

///////////////////////////////////
//1 列车类型

#define TRAIN_TYPE  ("TRAIN_TYPE")
#define ABS_TYPE    ("ABS_TYPE")
#define PRIO_DATA  ("RUN_PRIORITY")

BOOL CTGBasicParam::LoadPrioConfig(const char *filename)
{
	BOOL bRetValue = TRUE;

	char  strFileName[256];
	sprintf(strFileName,"%s\\%s",m_szDataPath,filename);
	m_cTrainTypeInfo.LoadTrain_TypeXML(strFileName);
	return TRUE;
}


const char *CTGBasicParam::GetTrainTypeName(int index,int *value)
{
	return m_cTrainTypeInfo.GetTrainTypeName(index,value);
}
const char *CTGBasicParam::GetAbsTypeName(int index,int *value)
{
	return m_cTrainTypeInfo.GetRunTypeName(index,value);
}
const char *CTGBasicParam::GetPrioName(int index,int *value)
{
	return m_cTrainTypeInfo.GetPrioTypeName(index,value);
}
const char *CTGBasicParam::GetTrainTypeNameById(int type_id)
{
	return m_cTrainTypeInfo.GetRunTypeName(type_id);
}
const char *CTGBasicParam::GetAbsTimeTypeNameByIndex(int index,int *value)
{
	return m_cTrainTypeInfo.GetAbsTimeTypeName(index,value);
}
const char *CTGBasicParam::GetAbsTimeTypeNameById(int type_id)
{
	return m_cTrainTypeInfo.GetAbsRunTypeName(type_id);
}
bool  CTGBasicParam::IsEnableABSTimeType()
{
	return m_cTrainTypeInfo.IsEnableABSTimeType();
}
int   CTGBasicParam::GetAllAbsTimeTypeName(CStringArray& name_list, CWordArray& run_type_list)
{
	return m_cTrainTypeInfo.GetSpecRunTypeName(name_list, run_type_list);
}

const char *CTGBasicParam::GetSideNameBySideNo(STATION_NO station,int sideno,int *value)
{
	const CStationDef* sta_def_ptr = GetStationDef(station);
	if (NULL == sta_def_ptr)
	{
		return NULL;		
	}
	static 	SIDE_NAME side_names[MAX_SIDE_COUNT];
	static  SIDE_NO    side_nos[MAX_SIDE_COUNT];
	short side_cnt=MAX_SIDE_COUNT;

	if(sta_def_ptr->GetReceDeptSideNameAndNo(side_names,side_nos,side_cnt)==STA_SUCCESS)
	{
		for(int index=0; index<side_cnt; index++)
		{
			if(side_nos[index]==sideno)
			{
				*value=side_nos[index];
				return side_names[index];
			}
		}
	}
	return NULL;
}

const char *CTGBasicParam::GetSideNameByIndex(STATION_NO station,int index,int *value)
{
	const CStationDef* sta_def_ptr = GetStationDef(station);
	if (NULL == sta_def_ptr)
	{
		return NULL;		
	}
	static 	SIDE_NAME side_names[MAX_SIDE_COUNT];
	static  SIDE_NO    side_nos[MAX_SIDE_COUNT];
	short side_cnt=MAX_SIDE_COUNT;

	if(sta_def_ptr->GetReceDeptSideNameAndNo(side_names,side_nos,side_cnt)==STA_SUCCESS)
	{
		if(index>=0 && index<side_cnt)
		{
			*value=side_nos[index];
			return side_names[index];
		}
	}
	return NULL;
}

int CTGBasicParam::GetSideIDByName(STATION_NO station,const char *name)
{
	const CStationDef* sta_def_ptr = GetStationDef(station);
	if (NULL == sta_def_ptr)
	{
		return NULL_SIDE;		
	}
	static 	SIDE_NAME side_names[MAX_SIDE_COUNT] ;
	static  SIDE_NO    side_nos[MAX_SIDE_COUNT];
	short side_cnt=MAX_SIDE_COUNT;

	if(sta_def_ptr->GetReceDeptSideNameAndNo(side_names,side_nos,side_cnt)==STA_SUCCESS)
	{
		for(int index=0;index<side_cnt;index++)
		{
			if(!stricmp(side_names[index],name))
				return side_nos[index];
		}
	}
	return NULL_SIDE;
}

const char *CTGBasicParam::GetSideNameByID(STATION_NO station,int sideid)
{
	const CStationDef* sta_def_ptr = GetStationDef(station);
	if (NULL == sta_def_ptr)
	{
		return NULL;		
	}
	static 	SIDE_NAME side_names[MAX_SIDE_COUNT];
	static  SIDE_NO    side_nos[MAX_SIDE_COUNT];
	short side_cnt=MAX_SIDE_COUNT;

	if(sta_def_ptr->GetReceDeptSideNameAndNo(side_names,side_nos,side_cnt)==STA_SUCCESS)
	{
		for(int index=0;index<side_cnt;index++)
		{
			if(side_nos[index]==sideid)
				return side_names[index];
		}
	}
	return NULL;
}


const char *CTGBasicParam::GetAbsName(int absnum)
{
	static char absname[128];
	const AbsAttrib *pattr=GetAbsAttrib(absnum);
	if(!pattr)
	{
		sprintf(absname,"ABS %d",absnum);
	}
	else
	{
		CGpcdef *gpcdefptr=m_pConfig->GetGPCDEFx();
		// 20090826 去除方向
		if(pattr->direction==0)
			sprintf(absname,"%s -> %s  %s",gpcdefptr->GetStationName(pattr->start_station),gpcdefptr->GetStationName(pattr->end_station),pattr->abs_name);
		else if(pattr->direction==1)
			sprintf(absname,"%s -> %s  %s",gpcdefptr->GetStationName(pattr->start_station),gpcdefptr->GetStationName(pattr->end_station),pattr->abs_name);
		else
			sprintf(absname,"%s <-> %s  %s",gpcdefptr->GetStationName(pattr->start_station),gpcdefptr->GetStationName(pattr->end_station),pattr->abs_name);
	}
	return absname;
}
const char *CTGBasicParam::GetAbsNameBetweenStations(STATION_NO st1,STATION_NO st2,int entry1,int entry2,int  *absnumber,int *default_direction)
{
	ABS_STRUCT_PTR ptr=NULL;
	ushort absnum1=0xffff,absnum2=0xffff;
	ushort dir1=0xffff,dir2=0xffff;

	if(!ConvertStationEntryToAbsnum(st1,entry1,&absnum1,&dir1))
		return NULL;
	if(!ConvertStationEntryToAbsnum(st2,entry2,&absnum2,&dir2))
		return NULL;
	if(absnum1!=absnum2 || dir1!=dir2)
		return NULL;

	const AbsAttrib *pattr=GetAbsAttrib(absnum1);
	if(!pattr)
		return NULL;
	if(absnumber)
		*absnumber=absnum1; 
	if(default_direction)
		*default_direction=pattr->direction;
	return GetAbsName(absnum1);

}

const char *CTGBasicParam::GetAbsNameBetweenStations(STATION_NO st1,STATION_NO st2,int index,int *absnumber) //穷举区间
{
	ABS_STRUCT_PTR ptr=NULL;
	int   absnumcount=-1;

	CAD_STATION *pstat1=GetStationPtr(st1);
	if(!pstat1)
		return FALSE;
	CAD_STATION *pstat2=NULL;
	if(st2>0)
		pstat2=GetStationPtr(st2);
	int j,i;

	for (i=0;i<pstat1->nEntryCount;i++)
	{
		if(pstat2)
		{
			for (j=0;j<pstat2->nEntryCount;j++)
			{
				if(pstat1->entry[i].abs_num == pstat2->entry[j].abs_num)  // 此处没有检查方向dir
				{
					absnumcount++;
					if(absnumcount==index)
					{
						break;
					}
				}
			}

		}
		else
		{
			absnumcount++;
		}
		if(absnumcount==index)
			break;
	}
	if(absnumcount==index)
	{
		static char absname[128];
		const AbsAttrib *pattr=GetAbsAttrib(pstat1->entry[i].abs_num);
		//if(!pattr)
		//	return NULL;
		if(absnumber)
			*absnumber=pstat1->entry[i].abs_num; 
		return GetAbsName(pstat1->entry[i].abs_num);
	}
	return NULL;

}

const char *CTGBasicParam::GetAbsAttrib(int absnum,int *direction)
{
	const AbsAttrib *pattr=GetAbsAttrib(absnum);
	if(!pattr)
		return NULL;
	*direction=pattr->direction;
	return pattr->abs_name; 
}

long CTGBasicParam::GetAbsNumberBetweenStations(int st1,int st2,int direction)
{
	int list_absline[128], list_direct[128];
	int count=GetAbsNumberBetweenStationsByCfgOrder(st1, st2, list_absline, list_direct);

	int abs_line=-1, abs_line_s=-1;
	if(count==0)
	{
		GpcLogTrainMessage(0, 0,"自动推算, 没有可选线号");
	}
	else if(count==1)
	{
		abs_line=list_absline[0];
	}
	else
	{
		for(int n=0; n<count; n++)
		{
			if(list_direct[n]==direction)
				abs_line=list_absline[n];
			else if(list_direct[n]==3)
				abs_line_s=list_absline[n];
		}
	}
	
	if(abs_line>0)
		return abs_line;   // 优先同向
	if(abs_line_s>0)
		return abs_line_s; // 双线备用
	return -1;
}
BOOL  CTGBasicParam::LoadTmisAbsAttrib(const char *cfgfile)
{
	char *ptr, szFileName[128], strLine[256];
	FILE *fp;
	char seps[]=" ,\t\\/=\n";
	sprintf(szFileName,"%s\\%s",gpcdef.GetDataDir(),cfgfile);
	if((fp=fopen(szFileName, "r"))==NULL)
		return FALSE;

	while (!feof(fp))
	{
		if(!(ptr = fgets(strLine, 256, fp)))
			break;

		int absnum;
		char dirstr[24];
		char stname1[24];
		char stname2[24];
		if(sscanf(strLine,"%d %s %s %s",&absnum,dirstr,stname1,stname2)!=4)
			continue;

		AbsAttrib  abs_data;
		abs_data.abs_no =0;

		abs_data.line_no = absnum;
		abs_data.start_station = gpcdef.GetStationNoByName(stname1);
		abs_data.end_station = gpcdef.GetStationNoByName(stname2);
		if(!stricmp(dirstr,"单线"))
			abs_data.direction = 3;
		else
			if(!stricmp(dirstr,"上行"))
				abs_data.direction = 0;
			else
				if(!stricmp(dirstr,"下行"))
					abs_data.direction = 1;
				else
					continue;

		abs_data.rela_data=GetAbsNumberBetweenStations(abs_data.start_station,abs_data.end_station,abs_data.direction);
		if(abs_data.rela_data<0)
		{
			CString cap;
			cap.Format("无法找到和TMIS区间编号 %d 对应的ABSNUM.",absnum);
			AfxMessageBox(cap);
		}
		m_aTmisAbsAttrib[absnum]=abs_data;
	} // while
	fclose(fp);
	if(m_aTmisAbsAttrib.size()>0)
		return TRUE;
	else
		return FALSE;
}

BOOL CTGBasicParam::LoadLimitStationSideConfig(const char *cfgfile)
{
	m_StaSideLimitList.clear();

	int  station=0;

	FILE* fp;
	char seps[]=" ,\t\\/=\n";
	char *ptr, *substr;
	char szFileName[MAX_PATH], strLine[256];
	sprintf(szFileName,"%s\\%s",m_szDataPath,cfgfile);
	fp = fopen(szFileName,"r");
	if (fp == NULL)
		return RET_FAILED;
	StaSideLimit *pstat=NULL;
	while(!feof(fp)) 
	{
		ptr = fgets(strLine, 256, fp);
		if (ptr == NULL) 
			continue;
		while (*ptr == ' ' || *ptr == '\t')
			ptr ++;
		if (*ptr == ';')	
			continue;
		if (*ptr == '\n')	
			continue;
	
		if(substr=strstr(ptr, "STATION_ID")) 
		{
			sscanf(substr,"STATION_ID=%d ",&station);
			pstat=GetStaLimitCfg(station);
			if(!pstat)
			{
				StaSideLimit pst;
				pst.nLimitCount=0;
				pst.nStation=station;
				m_StaSideLimitList.insert(make_pair(station, pst));

				pstat = GetStaLimitCfg(station);
			}
		} 
		else if(substr=strstr(ptr, "DENY_OVER_LIMIT")) 
		{
			int side1=0;
			int side2=0;
			int flag=2;
			ptr += strlen("DENY_OVER_LIMIT");
// 			if(sscanf(ptr,"=%d,%d,%d",&side1,&side2,&flag)>=2)
// 				continue;
			char *token;
			token = strtok(ptr, seps);
			if(token==NULL) continue;
			side1 = (int)atoi(token); 

			token = strtok(NULL, seps);
			if(token==NULL) continue;
			side2 = (int)atoi(token); 

			token = strtok(NULL, seps);
			if (token!=NULL)
				flag = (int)atoi(token); 


			if(pstat && pstat->nLimitCount<MAX_LIMIT_COUNT)
			{
				pstat->mSideList[pstat->nLimitCount].nSide1=side1;
				pstat->mSideList[pstat->nLimitCount].nSide2=side2;
				pstat->mSideList[pstat->nLimitCount].nFlag=flag;
				pstat->nLimitCount++;
			}
		} 
	}

	fclose(fp);
	GpcLogTrainMessage(0, 0,"读取limit_side.cfg成功!");
	return TRUE;
}

long CTGBasicParam::GetAbsNumByTmisNum(int tmisno)
{
	map<long,AbsAttrib>::const_iterator it=m_aTmisAbsAttrib.find(tmisno);
	if(it!=m_aTmisAbsAttrib.end())
		return it->second.rela_data;
	else
		return -1;
}


int CTGBasicParam::GetEntryExit(ushort con_id,ushort station, ushort onestation,ushort oneentry)
{
	CAD_STATION *pstat=GetStationPtr(station);
	if(!pstat)
		return -1;
	int j;
	int rc=-1;
	int dir=-1;
	for (j=0;j<pstat->nEntryCount;j++)
	{
		if (pstat->entry[j].entry_num==oneentry)
			dir=pstat->entry[j].dir;
	}
	int stationlist[128];
	int stationcount=128;
	CGpcdef *gpcdefptr=m_pConfig->GetGPCDEFx();
	stationcount=gpcdefptr->GetStationList(con_id,stationcount,stationlist); 

	for (j=0;j<pstat->nEntryCount;j++)
	{
		if (pstat->entry[j].entry_num==oneentry)
			continue;

		ushort station1, entry_num1,station2,entry_num2;
		if(ConvertAbsnumToStationEntry(&station1,&entry_num1,&station2,&entry_num2,pstat->entry[j].abs_num))
		{
			int anst;
			if(station1==station)
				anst=station2;
			else
				anst=station1;
			int stindex;
			for(stindex=0;stindex<stationcount;stindex++)
				if(stationlist[stindex]==anst)
					break;
			if(stindex<stationcount) // 本区段
				continue;
		}

		if(pstat->entry[j].dir==dir)
			rc=pstat->entry[j].entry_num;
		else
			if(rc<0)
				rc=pstat->entry[j].entry_num;

	}
	return rc;
}

BOOL  CTGBasicParam::LoadLocoTypeConfig(const char *cfgfile)
{
	return m_aLocoTypes.LoadLocolTypeXml(gpcdef.GetDataDir());
}

int CTGBasicParam::GetLocoAllPref(int con_id, CStringArray& arypref)
{
	return m_aLocoTypes.GetLocoAllPref(con_id, arypref);
}

CString CTGBasicParam::GetLocoName(int con_id, int index,int& value)
{
	return m_aLocoTypes.GetLocoName(con_id, index, value);
}

CString CTGBasicParam::GetLocoPreName(int con_id, CString type)
{
	return m_aLocoTypes.GetLocoPref(con_id, type);
}

CString CTGBasicParam::GetLocoIdByPref(int con_id, CString pref)
{
	return m_aLocoTypes.GetLocoIdByPref(con_id, pref);
}

int CTGBasicParam::GetCascoIdByLocoId(int con_id, int type)
{
	return m_aLocoTypes.GetCascoIdByLocoId(con_id, type);
}

BOOL CTGBasicParam::LoadStaNowCar(CString filename)
{
	m_carKindType.clear();
	m_aryWorkExplain.RemoveAll();
	m_aryCarrApply.RemoveAll();
	m_convert.clear();

	MSXML::IXMLDOMDocumentPtr pXMLDoc;
	HRESULT hr = pXMLDoc.CreateInstance(MSXML::CLSID_DOMDocument);
	if(FAILED(hr))
	{
		_com_error er(hr);
		AfxMessageBox(er.ErrorMessage());
		return FALSE;
	}

	CString pathname = gpcdef.GetDataDir();
	pathname += "\\" + filename;
	if(VARIANT_TRUE != pXMLDoc->load( _bstr_t( pathname ) ) )
	{
		CString  prompt;
		prompt.Format("配置%s不存在或者不符合xml语法(可以用浏览器检查是否能打开)", pathname);
		return FALSE;
	}

	MSXML::IXMLDOMNodePtr pChildNode,pNode,pRootNode,pSubRootNode;

	pRootNode = pXMLDoc->selectSingleNode( "station_car");
	if(pRootNode == NULL)
		return FALSE;

	MSXML::IXMLDOMNodeListPtr pListType = pRootNode->selectNodes( "car_type");
	if(NULL != pListType)
	{
		for(int idx = 0; idx < pListType->Getlength(); idx++)
		{
			pNode = pListType->Getitem(idx);
			if(pNode)
			{
				CarType car_type;
				pChildNode = pNode->selectSingleNode("@name");
				if(pChildNode)
				{
					_variant_t v;
					v = pChildNode->GetnodeTypedValue();
					strncpy(car_type.name, CString(v.bstrVal), sizeof(car_type.name));
					car_type.name[sizeof(car_type.name)-1]=0;
					SysFreeString(v.bstrVal);
					v.Clear();
				}
				pChildNode = pNode->selectSingleNode("@short_name");
				if(pChildNode)
				{
					_variant_t v;
					v = pChildNode->GetnodeTypedValue();
					strncpy(car_type.shortName, CString(v.bstrVal), sizeof(car_type.shortName));
					car_type.shortName[sizeof(car_type.shortName)-1]=0;
					SysFreeString(v.bstrVal);
					v.Clear();
				}
				pChildNode = pNode->selectSingleNode("@plot_short_name");
				if(pChildNode)
				{
					_variant_t v;
					v = pChildNode->GetnodeTypedValue();
					strncpy(car_type.plot_short_name, CString(v.bstrVal), sizeof(car_type.plot_short_name));
					car_type.plot_short_name[sizeof(car_type.plot_short_name)-1]=0;
					SysFreeString(v.bstrVal);
					v.Clear();
				}

				MSXML::IXMLDOMNodeListPtr pListKind = pNode->selectNodes( "car_kind");
				if(NULL != pListKind)
				{
					for(int i = 0; i < pListKind->Getlength(); i++)
					{
						pSubRootNode = pListKind->Getitem(i);
						if(pSubRootNode)
						{
							CarKind car_kind;
							CString name = "";
							pChildNode = pSubRootNode->selectSingleNode("@name");
							if(pChildNode)
							{
								_variant_t v;
								v = pChildNode->GetnodeTypedValue();

								name = CString(v.bstrVal);
								strncpy(car_kind.name, name.GetBuffer(), sizeof(car_kind.name)-1);
								car_kind.name[sizeof(car_kind.name)-1]=0;

								SysFreeString(v.bstrVal);
								v.Clear();
							}
							pChildNode = pSubRootNode->selectSingleNode("@short_name");
							if(pChildNode)
							{
								_variant_t v;
								v = pChildNode->GetnodeTypedValue();

								name = CString(v.bstrVal);
								strncpy(car_kind.shortName, name.GetBuffer(), sizeof(car_kind.shortName)-1);
								car_kind.name[sizeof(car_kind.name)-1]=0;

								SysFreeString(v.bstrVal);
								v.Clear();
							}

							car_type.carKind.push_back(car_kind);
						}
					}
				}

				MSXML::IXMLDOMNodeListPtr pListCommon = pNode->selectNodes( "common");
				if(NULL != pListCommon)
				{
					for(int i = 0; i < pListCommon->Getlength(); i++)
					{
						pSubRootNode = pListCommon->Getitem(i);
						if(NULL == pSubRootNode)
							continue;
						
						pChildNode = pSubRootNode->selectSingleNode("@name");
						if(NULL == pChildNode)
							continue;

						_variant_t v;
						v = pChildNode->GetnodeTypedValue();
						pNode = pRootNode->selectSingleNode(v.bstrVal);
						SysFreeString(v.bstrVal);
						v.Clear();

						if(NULL == pNode)
							continue;

						MSXML::IXMLDOMNodeListPtr pKind = pNode->selectNodes("car_kind");
						if(NULL == pKind)
							continue;

						for(int i = 0; i < pKind->Getlength(); i++)
						{
							pSubRootNode = pKind->Getitem(i);
							CarKind car_kind;
							CString name = "";
							pChildNode = pSubRootNode->selectSingleNode("@name");
							if(pChildNode)
							{
								_variant_t v;
								v = pChildNode->GetnodeTypedValue();

								name = CString(v.bstrVal);
								strncpy(car_kind.name, name.GetBuffer(), sizeof(car_kind.name)-1);
								car_kind.name[sizeof(car_kind.name)-1]=0;

								SysFreeString(v.bstrVal);
								v.Clear();
							}
							pChildNode = pSubRootNode->selectSingleNode("@short_name");
							if(pChildNode)
							{
								_variant_t v;
								v = pChildNode->GetnodeTypedValue();

								name = CString(v.bstrVal);
								strncpy(car_kind.shortName, name.GetBuffer(), sizeof(car_kind.shortName)-1);
								car_kind.name[sizeof(car_kind.name)-1]=0;

								SysFreeString(v.bstrVal);
								v.Clear();
							}

							car_type.carKind.push_back(car_kind);
						}
					}
				}

				m_carKindType.push_back(car_type);
			}
		}
	}		


	pSubRootNode = pRootNode->selectSingleNode("work_explain");
	if(pSubRootNode != NULL)
	{
		MSXML::IXMLDOMNodeListPtr pListExplain = pSubRootNode->selectNodes( "car_explain");
		if(NULL != pListExplain)
		{
			for(int idx = 0; idx < pListExplain->Getlength(); idx++)
			{
				pNode = pListExplain->Getitem(idx);
				if(pNode)
				{
					pChildNode = pNode->selectSingleNode("@name");
					if(pChildNode)
					{
						_variant_t v;
						v = pChildNode->GetnodeTypedValue();

						m_aryWorkExplain.Add(CString(v.bstrVal));

						SysFreeString(v.bstrVal);
						v.Clear();
					}
				}
			}
		}
	}
	pSubRootNode = pRootNode->selectSingleNode("carriage_apply");
	if(pSubRootNode != NULL)
	{
		MSXML::IXMLDOMNodeListPtr pListApply = pSubRootNode->selectNodes( "car_apply");
		if(NULL != pListApply)
		{
			for(int idx = 0; idx < pListApply->Getlength(); idx++)
			{
				pNode = pListApply->Getitem(idx);
				if(pNode)
				{
					pChildNode = pNode->selectSingleNode("@name");
					if(pChildNode)
					{
						_variant_t v;
						v = pChildNode->GetnodeTypedValue();

						m_aryCarrApply.Add(CString(v.bstrVal));

						SysFreeString(v.bstrVal);
						v.Clear();
					}
				}
			}
		}
	}

	
	pSubRootNode = pRootNode->selectSingleNode("convert");
	if(pSubRootNode != NULL)
	{
		MSXML::IXMLDOMNodeListPtr pListConvert = pSubRootNode->selectNodes( "map");
		if(NULL != pListConvert)
		{
			for(int idx = 0; idx < pListConvert->Getlength(); idx++)
			{
				pNode = pListConvert->Getitem(idx);
				if(pNode)
				{
					int orgid=-1, id=-1;
					pChildNode = pNode->selectSingleNode("@orgid");
					if(pChildNode)
					{
						_variant_t v;
						v = pChildNode->GetnodeTypedValue();

						orgid = atoi((CString(v.bstrVal)));

						SysFreeString(v.bstrVal);
						v.Clear();
					}
					pChildNode = pNode->selectSingleNode("@id");
					if(pChildNode)
					{
						_variant_t v;
						v = pChildNode->GetnodeTypedValue();

						id = atoi((CString(v.bstrVal)));

						SysFreeString(v.bstrVal);
						v.Clear();
					}

					if(orgid >= 0 && id >= 0)
					{
						m_convert.insert(make_pair(orgid, id));
					}
				}
			}
		}
	}

	return TRUE;
}
void CTGBasicParam::GetCarTypeAndKindIndex(BYTE type, BYTE& nSel, BYTE& nSubSel)
{
	nSel=0;
	nSubSel=0;

	BYTE nBit = m_carKindType.size()/2+m_carKindType.size()%2;
	if(nBit==2)
	{
		nSel = (type&~0x3F)>>6;
		nSubSel = type&0x3F;
		if(m_convert.size()>0)
		{
			std::map<int, int>::iterator it = m_convert.find(nSel);
			if(it != m_convert.end())
				nSel = it->second;
		}
	}
	else
	{
		nSel = (type&~0x7F)>>7;
		nSubSel = type&0x7F;
	}

	return;
}

WORD CTGBasicParam::GetCarriageType(BYTE nSel, BYTE nSubSel)
{
	BYTE nBit = m_carKindType.size()/2+m_carKindType.size()%2;

	BYTE ret = nSel;
	if(nBit==2)
	{
		if(m_convert.size()>0)
		{
			std::map<int, int>::iterator it = m_convert.begin();
			while(it != m_convert.end())
			{
				if(it->second == nSel)
				{
					ret = it->first;
					break;
				}

				it++;
			}
		}

		ret <<= 6;
		ret &= ~0x3F;
		ret += (nSubSel&0x3f);
	}
	else
	{
		ret <<= 7;
		ret &= ~0x7F;
		ret += (nSubSel&0x7f);
	}
	
	return ret;
}

CString CTGBasicParam::GetCarTypeShortName(BYTE nSel)
{
	if(m_carKindType.size() > nSel)
	{
		return m_carKindType[nSel].shortName;//modified by zhaoliang
	}
	return "";
}
void CTGBasicParam::ResetCarCount()
{
	for(int i=0;i < m_carKindType.size();i++)    
	{  
		for(int k=0; k<m_carKindType[i].carKind.size(); k++)
		{   
			m_carKindType[i].carKind[k].count=0;
		}
	}
}

CString CTGBasicParam::GetCarShortName(BYTE nSel, BYTE nSubSel)
{
	if(m_carKindType.size() > nSel)
	{
		return m_carKindType[nSel].carKind[nSubSel].shortName;
	}

	return "";
}

CString CTGBasicParam::GetCarriageExplain(BYTE index)
{
	if(index>=m_aryWorkExplain.GetCount())
		return "";

	return m_aryWorkExplain.GetAt(index);
}

int CTGBasicParam::GetEntryByCTCAbsLineNumber(int station, int ctcabsline)
{
	IterMapStation iter=m_aCadStations.find(station);
	if(iter==m_aCadStations.end())
		return 255;
	
	CadStation& st=iter->second;
	for(int n=0; n<st.nEntryCount; n++)
	{
		if(ctcabsline==st.entry[n].abs_num)
		{
			return st.entry[n].entry_num;
		}
	}
	
	return 255;
}

bool CTGBasicParam::IsTrainPassNotUseMainSide(TG_TrainRecord& rec)
{
	const CStationDef* pSideDef = GetStationDef(rec.station);
	if(pSideDef==NULL)
		return false;
	if(TG_IsFlagStart(rec.flag) || TG_IsFlagTerminal(rec.flag))
		return false;
	
	if(rec.arrive == rec.depart)
	{
		if(!pSideDef->isMainSide(rec.arrive_side,rec.entry) || !pSideDef->isMainSide(rec.depart_side,rec.exit))
		{
			return true;
		}
	}

	return false;
}

bool CTGBasicParam::IsTrainSideNotConnectWithEntry(int station, int sideno, int entry)
{
	const CStationDef* pSideDef = GetStationDef(station);
	if(pSideDef==NULL)
		return false;

	if (!pSideDef->isEntrySideConnect(sideno, entry))
		return true;
	
	return false;
}

int CTGBasicParam::GetAcceptEntry(ushort con_id,ushort station, ushort onestation,ushort oneentry,int in_index,CString trainid)
{
	CAD_STATION *pstat=GetStationPtr(station);
	if(!pstat)
		return -1;

	int abslinelist[128];
	int abslinecount=0;

	int dir=GetTGFunctionConfig()->GetTrainDirection(trainid);

	SectionDefine node_section;
	if(GetTGFunctionConfig()->GetSectionConfig(con_id, node_section))
	{
		for(int i=0; i<node_section.stdef_num; i++)
		{
			if(node_section.stinfo[i].index==(in_index+1) && node_section.stinfo[i].station==station)
			{
				if(dir==0)
				{
					abslinecount=node_section.stinfo[i].exit_line_num;
					for(int m=0; m<abslinecount; m++)
					{
						abslinelist[m]=node_section.stinfo[i].exit_line[m];
						GpcLogTrainMessage(0, 0, "试图使用区间号 %d 推算入口", abslinelist[m]);
					}
				}
				else
				{
					abslinecount=node_section.stinfo[i].entry_line_num;
					for(int m=0; m<abslinecount; m++)
					{
						abslinelist[m]=node_section.stinfo[i].entry_line[m];
						GpcLogTrainMessage(0, 0, "试图使用区间号 %d 推算入口", abslinelist[m]);
					}
				}
				break;
			}
		}
	}

	if(abslinecount==0)
	{
		GpcLogTrainMessage(0, 0, "没有试图推算入口的区间号");
		return GetEntryExit(con_id, station, onestation, oneentry);
	}
	
	for(int i=0; i<abslinecount; i++)
	{
		int abs_line=abslinelist[i];
		const CTGBasicParam::AbsAttrib * ptgline=GetAbsAttrib(abs_line);
		if (ptgline)
		{
			if(ptgline->direction==dir)
			{
				for (int j=0;j<pstat->nEntryCount;j++)
				{
					if (pstat->entry[j].abs_num==abs_line && pstat->entry[j].entry_num==oneentry)
					{
						GpcLogTrainMessage(0, 0, "区间号 %d 不符合条件(即 出口使用该区间,入口不能再使用该区间)", abs_line);
						continue;
					}

					if (pstat->entry[j].abs_num==abs_line && pstat->entry[j].entry_num!=oneentry)
					{
						GpcLogTrainMessage(0, 0, "区间号 %d 符合条件(即 列车接入车次方向和tg.def中区间线别一致)", abs_line);
						return pstat->entry[j].entry_num;
					}
				}
			}
			else
			{
				GpcLogTrainMessage(0, 0, "区间号 %d 不符合条件(即 列车接入车次方向 %d 和tg.def中区间线别方向 %d 不一致)", abs_line, dir, ptgline->direction);
			}
		}
		else
		{
			GpcLogTrainMessage(0, 0, "区间号 %d 不符合条件(即 在tg.def中找不到该区间线别)", abs_line);
		}
	}
		
	return -1;
}

int CTGBasicParam::GetGiveoutExit(ushort con_id,ushort station, ushort onestation,ushort oneentry,int out_index,CString trainid)
{
	CAD_STATION *pstat=GetStationPtr(station);
	if(!pstat)
		return -1;

	int abslinelist[128];
	int abslinecount=0;

	int dir=GetTGFunctionConfig()->GetTrainDirection(trainid);

	SectionDefine node_section;
	if(GetTGFunctionConfig()->GetSectionConfig(con_id, node_section))
	{
		for(int i=0; i<node_section.stdef_num; i++)
		{
			if(node_section.stinfo[i].index==(out_index+1) && node_section.stinfo[i].station==station)
			{
				if(dir==1)
				{
					abslinecount=node_section.stinfo[i].exit_line_num;
					for(int m=0; m<abslinecount; m++)
					{
						abslinelist[m]=node_section.stinfo[i].exit_line[m];
						GpcLogTrainMessage(0, 0, "试图使用区间号 %d 推算出口", abslinelist[m]);
					}
				}
				else
				{
					abslinecount=node_section.stinfo[i].entry_line_num;
					for(int m=0; m<abslinecount; m++)
					{
						abslinelist[m]=node_section.stinfo[i].entry_line[m];
						GpcLogTrainMessage(0, 0, "试图使用区间号 %d 推算出口", abslinelist[m]);
					}
				}
				break;
			}
		}
	}

	if(abslinecount==0)
	{
		GpcLogTrainMessage(0, 0, "没有试图推算出口的区间号");
		return GetEntryExit(con_id, station, onestation, oneentry);
	}

	for(int i=0; i<abslinecount; i++)
	{
		int abs_line=abslinelist[i];
		const CTGBasicParam::AbsAttrib * ptgline=GetAbsAttrib(abs_line);
		if (ptgline)
		{
			if(ptgline->direction==dir)
			{
				for (int j=0;j<pstat->nEntryCount;j++)
				{
					if (pstat->entry[j].abs_num==abs_line && pstat->entry[j].entry_num==oneentry)
					{
						GpcLogTrainMessage(0, 0, "区间号 %d 不符合条件(即 入口使用该区间,出口不能再使用该区间)", abs_line);
						continue;
					}

					if (pstat->entry[j].abs_num==abs_line && pstat->entry[j].entry_num!=oneentry)
					{
						GpcLogTrainMessage(0, 0, "区间号 %d 符合条件(即 列车交出车次方向和tg.def中区间线别一致)", abs_line);
						return pstat->entry[j].entry_num;
					}
				}
			}
			else
			{
				GpcLogTrainMessage(0, 0, "区间号 %d 不符合条件(即 列车交出车次方向 %d 和tg.def中区间线别方向 %d 不一致)", abs_line, dir, ptgline->direction);
			}
		}
		else
		{
			GpcLogTrainMessage(0, 0, "区间号 %d 不符合条件(即 在tg.def中找不到该区间线别)", abs_line);
		}
	}
	
	return -1;
}

long CTGBasicParam::GetAbsNumberBetweenStationsByCfgOrder(int st1,int st2, int list_absline[],int list_direct[])
{
	map<int, AbsLineInfo> m_tmpAbsInfo;
	map<long,AbsAttrib>::iterator it= m_aAbsAttrib.begin();
	for(; it != m_aAbsAttrib.end(); it++)
	{
		if((it->second.start_station==st1 && it->second.end_station==st2) 
			|| ((it->second.start_station==st2 && it->second.end_station==st1) && it->second.direction==3))
		{
			AbsLineInfo info;
			info.line_no=it->second.line_no;
			info.direct=it->second.direction;
			m_tmpAbsInfo[it->second.cfgorder]=info;
		}
	}

	int count=0;
	map<int, AbsLineInfo>::iterator it1 = m_tmpAbsInfo.begin();
	for(; it1!=m_tmpAbsInfo.end() && count<128; it1++)
	{
		GpcLogTrainMessage(0, 0,"自动推算, 可选区间线别:line_no:%d, direct=%d",
				it1->second.line_no, it1->second.direct);

		list_absline[count]=it1->second.line_no;
		list_direct[count]=it1->second.direct;
		count++;
	}

	return count;
}

BOOL CTGBasicParam::VerifyNotCRHStopSide(int side, int station, const TG_TrainInfo &coninfo)
{
	const CStationDef* sta_def = GetStationDef(station);
	if (NULL == sta_def) 
	{
		GpcLogTrainMessage(0,0,"can't find station %d form stationSide.def", station);
		return TRUE;
	}

	return verify_side_not_stop_crh(sta_def, side, coninfo);
}

BOOL CTGBasicParam::verify_side_not_stop_crh(const CStationDef* sta_def_ptr, SIDE_ID side, const TG_TrainInfo &coninfo)
{
	if(IsCrhTrain(coninfo.type, coninfo.run_type))
	{
		if(STA_FAILED == sta_def_ptr->isCRHSide(side))
			return FALSE;
	}
	
	return TRUE;
}

BOOL CTGBasicParam::VerifySectionStationIndex(int section, int station, int index)
{
	SectionDefine node_section;
	if(GetTGFunctionConfig()->GetSectionConfig(section, node_section))
	{
		for(int i=0; i<node_section.stdef_num; i++)
		{
			if(node_section.stinfo[i].index==index && node_section.stinfo[i].station==station)
				return TRUE;
		}
	}
	return FALSE;
}

CTGBasicParam::StaSideLimit *CTGBasicParam::GetStaLimitCfg(int stno)
{
	std::map<int, StaSideLimit>::iterator iter = m_StaSideLimitList.find(stno);
	if (iter != m_StaSideLimitList.end())
		return &iter->second;
	else
		return NULL;
}

BOOL  CTGBasicParam::IsTwoStationSideLimit(int station, int side1, int side2)
{
	std::map<int, StaSideLimit>::iterator iter = m_StaSideLimitList.find(station);
	if (iter != m_StaSideLimitList.end())
	{
		for (int i=0;i<iter->second.nLimitCount;i++)
		{
			LimitCfg tmpCfg;
			tmpCfg = iter->second.mSideList[i];
			
			if ((tmpCfg.nSide1==side1&&tmpCfg.nSide2==side2) || (tmpCfg.nSide2==side1&&tmpCfg.nSide1==side2))
				return TRUE;
		}
	}

	return FALSE;
}

BOOL  CTGBasicParam::GetTrainTypeAndPrio(const char* train_no,BYTE *type, BYTE *run_type, BYTE *prio)
{
	CString strTrainId = train_no;
	if(GetTGFunctionConfig()->IsRemoveTrainIDChineseChar())
	    strTrainId = RemoveChineseChar(train_no);
	if(strTrainId.IsEmpty())
		strTrainId=train_no;

	if(m_cTrainTypeInfo.GetPrioFromTrainId(strTrainId,type,run_type,prio))
		return TRUE;
	else
		return FALSE;
}

const char *CTGBasicParam::GetAbsNameByStationEntry(STATION_NO st, int entry)
{
	static char absname[128];
	memset(absname, 0, 128);
	ABS_STRUCT_PTR ptr=NULL;
	ushort absnum1=0xffff,absnum2=0xffff;
	ushort dir1=0xffff,dir2=0xffff;

	if(!ConvertStationEntryToAbsnum(st,entry,&absnum1,&dir1))
		return absname;   // return NULL; 需要判断返回值，否则按照字符串输出会死机

	const AbsAttrib *pattr=GetAbsAttrib(absnum1);
	if(!pattr)
	{
		sprintf(absname,"ABS %d",absnum1);
	}
	else
	{
		if(pattr->direction==0)
			sprintf(absname,"%s(上行)",pattr->abs_name);
		else if(pattr->direction==1)
			sprintf(absname,"%s(下行)",pattr->abs_name);
		else
			sprintf(absname,"%s(双向)",pattr->abs_name);
	}
	return absname;
}
