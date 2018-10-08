#include "StdAfx.h"
#include ".\tgdef.h"


#define SECTION_NODE         ("[SECTION_NODE]")
#define SECTION_DISPLAY      ("[SECTION_DISPLAY]")
#define DISPLAY_TYPE_INHIBIT ("[DISPLAY_TYPE_INHIBIT]")
#define LINE_DEFINE          ("[LINE_DEFINE]")
#define DEFINE_PATH		     ("[DEFINE_PATH]")

#define LOCO_NO_DISPLAY		 ("[LOCO_NO_DISPLAY]")

#define IN_OUT_STATION_INDEX   ("[IN_OUT_STATION_INDEX]")
#define BIANZHU_STATION        ("[BIANZHU_STATION]")
#define FENJIE_STATION         ("[FENJIE_STATION]")
#define ENTRY_DEFINE           ("[ENTRY_DEFINE]")

#define HEIGHT_OF_WHOLE_SCREEN ("[HEIGHT_OF_WHOLE_SCREEN]")
#define CHECK_AAB		       ("[CHECK_AAB]")
#define NOT_CHECK_ABB_SECTION	   ("[NOT_CHECK_AAB_SECTION]")
#define STATION_STOP_LINE      ("[STATION_STOP_LINE]")
#define REVERSE_ABS            ("[REVERSE_ABS]")
#define DRAW_ABS_START_START  ("[DRAW_ABS_START_START]")
#define TRAIN_DISPLAY			("[TRAIN_DISPLAY]")
#define STATION_LINE_COLOR		("[STATION_LINE_COLOR]")
#define STATION_KILOMK		("[STATION_KILOMK]")

#define SPECIAL_EXIT_ENTRY		("[SPECIAL_EXIT_ENTRY]")
#define SPECIAL_MOVE           ("[SPECIAL_MOVE]")

#define SECTION_NODE_ENTRY  ("[SECTION_NODE_ENTRY]")
#define CZH_CFG  ("[CZH_CFG]")
#define STATION_NEXTSTATION_LINENO  ("[STATION_NEXTSTATION_LINENO]")
#define STATION_DIRECTION_TIP  ("[STATION_DIRECTION_TIP]")
#define STATION_GIVEOUT_TIP  ("[STATION_GIVEOUT_TIP]")
#define STATION_ACCEPT_TIP  ("[STATION_ACCEPT_TIP]")
CTGDef::CTGDef(void)
{
	m_nCheckMinLate = 1;
	Clear();
}

CTGDef::~CTGDef(void)
{
	Clear();
}

void CTGDef::Clear()
{
	m_mapStationKilo.clear();
	m_mapCanDrawABSStart.clear();
	m_mapInhibitTrainType.clear();
	m_mapSection.clear();
	m_mapRoute.clear();
	m_mapAbsLine.clear(); 
	m_section_display_info.clear();

	m_infteraceStation.clear();
	m_bianzuStation.clear();
	m_inOutStationIndexSection.clear();
	m_mapEntryDefine.clear();
	
	m_vecSectionStopLine.clear();
	m_vecSpecialExitEntry.clear();
	
	m_vecNodeEntry.clear();

	token = NULL;
	m_nTgErrorline = 0;
	m_ScreenHeight = 0;
	m_entryset.clear();
	m_direction_tip.clear();
	m_giveout_tip.clear();
	m_accept_tip.clear();
}

int CTGDef::GetStationStopLine(int section, int station, int in_index, int out_index)
{
	int nStopLine = 1;
	for(int i=0; i<m_vecSectionStopLine.size(); i++)
	{
		if(section == m_vecSectionStopLine[i].section)
		{
			for(int k=0; k<m_vecSectionStopLine[i].station_stop_line.size(); k++)
			{
				StationStopLine& line = m_vecSectionStopLine[i].station_stop_line[k];
				if(line.station == station && line.in_index == in_index && line.out_index == out_index)
				{
					nStopLine = line.pass_stop_flag;
					break;
				}
			}
			break;
		}
	}

	return nStopLine;
}

WORD CTGDef::GetSectionIDs(WORD id[],const WORD nMax)
{
    int nIndex = 0;
	std::map<WORD, section_node_info>::iterator it = m_mapSection.begin();
	while(it != m_mapSection.end())
	{
		if(nMax <= nIndex)
			break;
		id[nIndex] = it->first;
		nIndex++;
		it++;
	} 
	return nIndex;
}

int CTGDef::GetSectionNodeCount(int sectionid)
{
	std::map<WORD, section_node_info>::const_iterator it = m_mapSection.find(sectionid);
	if(it != m_mapSection.end())
	{
		return (int)it->second.m_mapNodes.size();
	}
	return 0;
}

const node_info* CTGDef::GetNodeByIndex(int sectionid, int index)
{
	std::map<WORD, section_node_info>::iterator it = m_mapSection.find(sectionid);
	if(it != m_mapSection.end())
	{
		std::map<int, node_info>::iterator tmp = it->second.m_mapNodes.find(index);
		if(tmp != it->second.m_mapNodes.end())
		{
			return &tmp->second;
		}
	} 
	return NULL;
}

bool CTGDef::IsMutilStation(int sectionid, int sta)
{
	std::map<WORD, section_node_info>::const_iterator ittmp = m_mapSection.find(sectionid);
	if(ittmp != m_mapSection.end())
	{
		for(int i = 0; i <  ittmp->second.m_vectMutilSta.size(); ++i)
		{
			if(ittmp->second.m_vectMutilSta[i] == sta)
			{
				return true;
			}
		}
	}
	return false;
}

const SectionDisplay* CTGDef::GetSectionDisplay(int sectionid)
{
	for(UINT idx = 0; idx < m_section_display_info.size(); idx++)
	{
		if(m_section_display_info[idx].nSection == sectionid)
		{
			return &m_section_display_info[idx];
		}
	}
	return NULL;
}

int  CTGDef::GetSectionHeight(int nSectionNo)
{
	int height = 1000;
	for(UINT idx = 0; idx < m_section_display_info.size(); idx++)
	{
		if(m_section_display_info[idx].nSection == nSectionNo)
		{
			height = m_section_display_info[idx].nScreenTotalHeight;
			break;
		}
	}

	return height;
}
bool  CTGDef::IsInhibitTrainType(int sectionid, WORD type)
{
	std::map<WORD, INHIBIT_TRAIN>::const_iterator it = m_mapInhibitTrainType.find(sectionid);

	if(it != m_mapInhibitTrainType.end())
	{
		for(int idx = 0; idx < it->second.nTypeCount; idx++)
		{
			if(it->second.trainType[idx] == type)
			{
				return true;
			}
		}
	}
	return false;
}

const ABS_LINE* CTGDef::GetLineDefine(int lineno)
{
	std::map<int, ABS_LINE>::const_iterator it = m_mapAbsLine.find(lineno);
	if(it != m_mapAbsLine.end())
	{
		return &it->second;
	}

	return NULL;
}

int  CTGDef::GetLineDirection(int lineNo)
{
	const ABS_LINE* line = this->GetLineDefine(lineNo);
	if(line)
		return line->nDirect;
	return 3;
}

bool  CTGDef::IsBianzuStation(int sta)
{
	if(0 == m_bianzuStation.size())
		return true;

	for(UINT idx = 0; idx < m_bianzuStation.size(); idx++)
	{
		if(m_bianzuStation[idx] == sta)
			return true;
	}
	return false;
}

bool  CTGDef::IsInterfaceStation(int sta)
{
	if(0 == m_infteraceStation.size())
		return true;

	for(UINT idx = 0; idx < m_infteraceStation.size(); idx++)
	{
		if(m_infteraceStation[idx] == sta)
			return true;
	}
	return false;
}

// 读取配置TG.def
bool CTGDef::LoadTGProfile(CString pathname)
{
	Clear();


	char seps[]=" ,\t\\/=\n";

	FILE *fp=NULL;

	char *ptr, strLine[1024];
	if((fp=fopen(pathname, "r"))==NULL)
		return false;

	// read file
	m_nTgErrorline = 0;
	while (!feof(fp))	
	{
		ptr = fgets(strLine, 1024, fp);
		m_nTgErrorline++;
		if (ptr == NULL) 
			continue;
		while (*ptr == ' ' || *ptr == '\t')
			ptr ++;
		if (*ptr == ';')	
			continue;
		if (*ptr == '\n')	
			continue;

		
		if (*ptr == '[' && !strncmp(ptr, STATION_LINE_COLOR, strlen(STATION_LINE_COLOR)) ) 
		{
			if(!ReadStationLineColorDisplay(ptr,seps)) 
			{
				fclose(fp);
				return false;
			}
        }

		else if (*ptr == '[' && !strncmp(ptr, STATION_KILOMK, strlen(STATION_KILOMK)) ) 
		{
			if(!ReadStationKiloMk(ptr,seps)) 
			{
				fclose(fp);
				return false;
			}
        }

		else if (*ptr == '[' && !strncmp(ptr, TRAIN_DISPLAY, strlen(TRAIN_DISPLAY)) ) 
		{
			if(!ReadTrainDisplay(ptr,seps)) 
			{
				fclose(fp);
				return false;
			}
        }

		else if (*ptr == '[' && !strncmp(ptr, DRAW_ABS_START_START, strlen(DRAW_ABS_START_START)) ) 
		{
			if(!ReadDrawAbsStart(ptr,seps)) 
			{
				fclose(fp);
				return false;
			}
        }

        else if (*ptr == '[' && !strncmp(ptr, IN_OUT_STATION_INDEX, strlen(IN_OUT_STATION_INDEX)) ) 
		{
			if(!ReadInOutStation(ptr,seps)) 
			{
				fclose(fp);
				return false;
			}
        }

		else if (*ptr == '[' && !strncmp(ptr, ENTRY_DEFINE, strlen(ENTRY_DEFINE)) ) 
		{
			if(!ReadEntryDefine(ptr)) 
			{
				fclose(fp);
				return false;
			}
        }

		else if (*ptr == '[' && !strncmp(ptr, SECTION_NODE, strlen(SECTION_NODE)) ) 
		{ 
			if(!ReadSection(ptr,seps)) 
			{
				fclose(fp);
				return false;
			}
		}
		
		else if (*ptr == '[' && !strncmp(ptr, CZH_CFG, strlen(CZH_CFG)) )
		{
			if(!ReadCzhConfig(ptr,seps)) 
			{
				fclose(fp);
				return false;
			}
		}

		else if (*ptr == '[' && !strncmp(ptr, STATION_NEXTSTATION_LINENO, strlen(STATION_NEXTSTATION_LINENO)) )
		{
			if(!ReadFromToLineNo(ptr,seps)) 
			{
				fclose(fp);
				return false;
			}
		}
			
		else if (*ptr == '[' && !strncmp(ptr, STATION_DIRECTION_TIP, strlen(STATION_DIRECTION_TIP)) )
		{
			if(!ReadDirectionTip(ptr,seps)) 
			{
				fclose(fp);
				return false;
			}
		}

		else if (*ptr == '[' && !strncmp(ptr, STATION_GIVEOUT_TIP, strlen(STATION_GIVEOUT_TIP)) )
		{
			if(!ReadGiveoutTip(ptr,seps)) 
			{
				fclose(fp);
				return false;
			}
		}

		else if (*ptr == '[' && !strncmp(ptr, STATION_ACCEPT_TIP, strlen(STATION_ACCEPT_TIP)) )
		{
			if(!ReadAcceptTip(ptr,seps)) 
			{
				fclose(fp);
				return false;
			}
		}

		else if (*ptr == '[' && !strncmp(ptr, SECTION_NODE_ENTRY, strlen(SECTION_NODE_ENTRY)) ) 
		{ 
			if(!ReadSectionNodeEntry(ptr,seps)) 
			{
				fclose(fp);
				return false;
			}
		}
		
		else if (*ptr == '[' && !strncmp(ptr, "[VIRTUAL_STATION]", strlen("[VIRTUAL_STATION]")) ) 
		{ 
			if(!ReadVirtualStation(ptr,seps)) 
			{
				fclose(fp);
				return false;
			}
		}

		else if (*ptr == '[' && !strncmp(ptr, DEFINE_PATH, strlen(DEFINE_PATH)) ) 
		{
			if(!ReadDefinePath(ptr,seps)) 
			{
				fclose(fp);
				return false;
			}
		}

		else if (*ptr == '[' && !strncmp(ptr, LINE_DEFINE, strlen(LINE_DEFINE)) ) 
		{
			if(!ReadABSLine(ptr,seps)) 
			{
				fclose(fp);
				return false;
			}
		}

		else if (*ptr == '[' && !strncmp(ptr, HEIGHT_OF_WHOLE_SCREEN, strlen(HEIGHT_OF_WHOLE_SCREEN)) ) 
		{
			if(!ReadScreenHeight(ptr,seps))
			{
				fclose(fp);
				return false;
			}
		}
		
		else if (*ptr == '[' && !strncmp(ptr, SECTION_DISPLAY, strlen(SECTION_DISPLAY)) ) 
		{  
			if(!ReadSectionDisplay(ptr,seps))
			{
				fclose(fp);
				return false;
			}
		}

		else if (*ptr == '[' && !strncmp(ptr, LOCO_NO_DISPLAY, strlen(LOCO_NO_DISPLAY)) ) 
		{  
			if(!ReadLocoNoDisplay(ptr,seps))
			{
				fclose(fp);
				return false;
			}
		}

		else if (*ptr == '[' && !strncmp(ptr, DISPLAY_TYPE_INHIBIT, strlen(DISPLAY_TYPE_INHIBIT)) ) 
		{
			if(!ReadDisplayTypeInhibit(ptr,seps))
			{
				fclose(fp);
				return false;
			}
		}
		else if (*ptr == '[' && !strncmp(ptr, STATION_STOP_LINE, strlen(STATION_STOP_LINE)) ) 
		{
			if(!ReadSectionStopLine(ptr,seps))
			{
				fclose(fp);
				return false;
			}
		}
		else if (*ptr == '[' && !strncmp(ptr, REVERSE_ABS, strlen(REVERSE_ABS)) ) 
		{
			if(!ReadAbsDirect(ptr,seps))
			{
				fclose(fp);
				return false;
			}
		}
		else if (*ptr == '[' && !strncmp(ptr, SPECIAL_EXIT_ENTRY, strlen(SPECIAL_EXIT_ENTRY)) ) 
		{
			if(!ReadSpecialExitEntry(ptr,seps))
			{
				fclose(fp);
				return false;
			}
		}
		else if (*ptr == '[' && !strncmp(ptr, CHECK_AAB, strlen(CHECK_AAB)) ) 
		{
			ptr = ptr + strlen(CHECK_AAB);
			token = strtok(ptr, seps);
			if(token==NULL) 
			{
				fclose(fp);
				return false;
			}
			m_nCheckMinLate = (int)atoi(token);
		}
		else if (*ptr == '[' && !strncmp(ptr, NOT_CHECK_ABB_SECTION, strlen(NOT_CHECK_ABB_SECTION)) ) 
		{
			ptr = ptr + strlen(NOT_CHECK_ABB_SECTION);

			token = strtok(ptr, seps);
			if(token==NULL) return false;
			int count = (int)atoi(token);
			int section = 0;
			for(int j= 0;j<count;j++)
			{
				token = strtok(NULL, seps);
				if(token==NULL) 
				{
					CString msg;
					msg.Format("配置文件 TG.def中NOT_CHECK_ABB_SECTION个数错误");
					MessageBox(NULL, msg, "错误", MB_ICONHAND|MB_OK);
					fclose(fp);
					return false;
				}
				section = (int)atoi(token);
				m_vecNoCheckMinLateSectionList.push_back(section);
			}
		}
				
		else if (*ptr == '[' && !strncmp(ptr, BIANZHU_STATION, strlen(BIANZHU_STATION)) ) 
		{
			ptr = ptr + strlen(BIANZHU_STATION);

			token = strtok(ptr, seps);
			if(token==NULL) return false;
			int count = (int)atoi(token);
			int station = 0;
			for(int j= 0;j<count;j++)
			{
				token = strtok(NULL, seps);
				if(token==NULL) 
				{
					CString msg;
					msg.Format("配置文件 TG.def中BIANZHU_STATION个数错误");
					MessageBox(NULL, msg, "错误", MB_ICONHAND|MB_OK);
					fclose(fp);
					return false;
				}
				station = (int)atoi(token);
				m_bianzuStation.push_back(station);
			}
		}
		else if (*ptr == '[' && !strncmp(ptr, FENJIE_STATION, strlen(FENJIE_STATION)) ) 
		{
			ptr = ptr + strlen(FENJIE_STATION);

			token = strtok(ptr, seps);
			if(token==NULL) return false;
			int count = (int)atoi(token);
			int station = 0;
			for(int j= 0;j<count;j++)
			{
				token = strtok(NULL, seps);
				if(token==NULL) 
				{
					CString msg;
					msg.Format("配置文件 TG.def中的FENJIE_STATION个数错误");
					MessageBox(NULL, msg, "错误", MB_ICONHAND|MB_OK);
					fclose(fp);
					return false;
				}
				station = (int)atoi(token);
				m_infteraceStation.push_back(station);
			}
		}
		else if (*ptr == '[' && !strncmp(ptr, SPECIAL_MOVE, strlen(SPECIAL_MOVE)) )
		{
			ptr = ptr + strlen(SPECIAL_MOVE);
			int tmppresta=0, tmpcurrsta=0, tmpsectionid=0;
			
			token = strtok(ptr, seps);
			if(token) 
				tmpsectionid=atoi(token);
			
			token = strtok(NULL, seps);
			if(token) 
				tmppresta=atoi(token);
				
			token = strtok(NULL, seps);
			if(token) 
				tmpcurrsta=atoi(token);
		}
	}

	// 处理颜色
	std::map<int, CON_STATION_COLOR>::iterator it = m_mapStationLineColor.begin();
	while(it != m_mapStationLineColor.end())
	{
		std::map<WORD, section_node_info>::iterator it_con = m_mapSection.find(it->second.con_id);
		if(it_con != m_mapSection.end())
		{
			for(int i=0; i<it->second.station_num; i++)
			{
				std::map<int, node_info>::iterator itnode = it_con->second.m_mapNodes.find(it->second.station_clr[i].index);
				if(itnode != it_con->second.m_mapNodes.end())
				{
					itnode->second.clrStationLine=it->second.station_clr[i].clr;
				}
			}
		}

		it++;
	}

	fclose(fp);

	return true;
}

bool CTGDef::ReadEntryDefine(char* ptr)
{
	char seps[]=" ,()\t\\/=\n";
	
	if (*ptr != '[' || strncmp(ptr, ENTRY_DEFINE, strlen(ENTRY_DEFINE))) 
	{ 
		return false;
	}
	ptr = ptr + strlen(ENTRY_DEFINE);

	token = strtok(ptr, seps);
	if(token==NULL) 
	{
		AfxMessageBox("TG.def中[ENTRY_DEFINE]不是有效配置项,如果不需要请注释掉");
		return false;
	}
    
	// 台号
	int section = atoi(token);
	SingleEntryDefine define;

	// 当前台名称
	token = strtok(NULL, seps);
	
	// 当前站
	token = strtok(NULL, seps);
    if(NULL != token)
		define.currentSta.station = atoi(token);
	else
		define.currentSta.station = 0;

	token = strtok(NULL, seps);
    if(NULL != token)
		define.currentSta.entry = atoi(token);
	else
		define.currentSta.entry = -1;

	token = strtok(NULL, seps);
    if(NULL != token)
		define.currentSta.exit = atoi(token);
	else
		define.currentSta.exit = -1;
	
	// 前方站...
	define.beforeNum = 0;
    do
	{
		token = strtok(NULL, seps);
		if(NULL != token)
			define.before[define.beforeNum].station = atoi(token);
		else
			break;
		
		token = strtok(NULL, seps);
		if(NULL != token)
			define.before[define.beforeNum].entry = atoi(token);
		else
			break;
		
		token = strtok(NULL, seps);
		if(NULL != token)
			define.before[define.beforeNum].exit = atoi(token);
		else
			break;

		define.beforeNum++;
	}while(NULL != token);

	if(define.beforeNum <= 0 || define.currentSta.station == 0 || define.currentSta.entry == -1 || define.currentSta.exit == -1)
		return false;

	std::map<int, SectionEntryDefine>::iterator it = m_mapEntryDefine.find(section);  
	if(it != m_mapEntryDefine.end())
	{
		it->second.define[it->second.defineNum] = define;
		it->second.defineNum++;
	}
	else
	{
		SectionEntryDefine sectDefine;
		sectDefine.section = section;
		sectDefine.defineNum = 0;
		sectDefine.define[sectDefine.defineNum] = define;
		sectDefine.defineNum++;

		m_mapEntryDefine.insert(make_pair(section, sectDefine));
	}

	return true;
}

bool CTGDef::IsCanDrawABSStart(int conid) 
{ 
	std::map<int, int>::iterator it = m_mapCanDrawABSStart.find(conid);
	if(it != m_mapCanDrawABSStart.end())
	{
		return (it->second>0);
	}
	return false;
}

bool CTGDef::ReadDrawAbsStart(char* ptr,char* seps)
{
	if (*ptr != '[' || strncmp(ptr, DRAW_ABS_START_START, strlen(DRAW_ABS_START_START))) 
	{ 
		return false;
	}
	ptr = ptr + strlen(DRAW_ABS_START_START);
	
	token = strtok(ptr, seps);
	if(token==NULL) 
	{
		AfxMessageBox("TG.def中[DRAW_ABS_START_START]不是有效配置项,如果不需要请注释掉");
		return false;
	}

	int con_id = atoi(token);
    
	token = strtok(NULL, seps);
	if(token != NULL) 
	{
		int iscan = (int)atoi(token);
		m_mapCanDrawABSStart.insert(make_pair(con_id, iscan));
	}

	return true;
}

bool CTGDef::ReadInOutStation(char* ptr,char* seps)
{
	if (*ptr != '[' || strncmp(ptr, IN_OUT_STATION_INDEX, strlen(IN_OUT_STATION_INDEX))) 
	{ 
		return false;
	}
	ptr = ptr + strlen(IN_OUT_STATION_INDEX);

	m_inOutStationIndexSection.clear();
		
	token = strtok(ptr, seps);
	if(token==NULL) 
	{
		AfxMessageBox("TG.def中[IN_OUT_STATION_INDEX]不是有效配置项,如果不需要请注释掉");
		return false;
	}

	int nNum = atoi(token);
        
	for (int i=0; i<nNum; i++) 
	{
		token = strtok(NULL, seps);
		if(token==NULL) 
			break;
		int section = (int)atoi(token);
		m_inOutStationIndexSection.push_back(section);
	}

	if (m_inOutStationIndexSection.size() != nNum) 
	{
		AfxMessageBox("TG.def中[IN_OUT_STATION_INDEX]的区段数目和实际配置数目不一致");
		return false;
	}

	return true;
}

bool CTGDef::ReadSectionNodeEntry(char* ptr,char* seps)
{
	if (*ptr != '[' || strncmp(ptr, SECTION_NODE_ENTRY, strlen(SECTION_NODE_ENTRY))) 
	{ 
		return false;
	}
	ptr = ptr + strlen(SECTION_NODE_ENTRY);

	NodeEntryDefine node;

	// section no 
	token = strtok(ptr, seps);
	if(token==NULL) 
		return true;
	node.conid = (int)atoi(token);

	token = strtok(NULL, seps);
	if(token==NULL) 
		return true;
	node.sub_table = (int)atoi(token);

	token = strtok(NULL, seps);
	if(token==NULL) 
		return true;
	node.index = (int)atoi(token);

	//station
	token = strtok(NULL, seps);
	if(token==NULL) 
		return true;
	node.station =  (int)atoi(token);

	//entry
	token = strtok(NULL, seps);
	if(token==NULL) 
		return true;
	node.entry = (int)atoi(token);

	//exit
	token = strtok(NULL, seps);
	if(token==NULL) 
		node.exit = 0;
	else
		node.exit = (int)atoi(token);

	for(int i=0; i<m_vecNodeEntry.size(); i++)
	{
		if(m_vecNodeEntry[i].conid==node.conid && m_vecNodeEntry[i].sub_table==node.sub_table && m_vecNodeEntry[i].index==node.index)
		{
			break;
		}
	}
	if(i>=m_vecNodeEntry.size())
		m_vecNodeEntry.push_back(node);

	return true;
}

bool CTGDef::ReadCzhConfig(char* ptr,char* seps)
{
	if (*ptr != '[' || strncmp(ptr, CZH_CFG, strlen(CZH_CFG))) 
	{ 
		return false;
	}
	ptr = ptr + strlen(CZH_CFG);

	do
	{
		node_info node;

		// section no 
		token = strtok(ptr, seps);
		if(token==NULL) 
			break;

		int nSectionNo = (int)atoi(token);

		token = strtok(NULL, seps);
		if(token==NULL) 
			break;
		node.sub_section = (int)atoi(token);

		token = strtok(NULL, seps);
		if(token==NULL) 
			break;
		node.index = (int)atoi(token);

		//name
		token = strtok(NULL, seps);
		if(token==NULL) break;
		strncpy(node.text, (const char*)token,31);
		node.text[31] = 0;

		//staton number
		token = strtok(NULL, seps);
		if(token==NULL) break;
		node.station_number =  (int)atoi(token);

		// down line number
		int nCzhPrintFlag=0;
		token = strtok(NULL, seps);
		if(token==NULL) break;
			nCzhPrintFlag =  (int)atoi(token);
		
		std::map<WORD, section_node_info>::iterator it = 
									this->m_mapSection.find(nSectionNo);
		if(it != this->m_mapSection.end())
		{
			std::map<int, node_info>::iterator tmp = it->second.m_mapNodes.find(node.index-1);
			if(tmp != it->second.m_mapNodes.end())
			{
				if(tmp->second.station_number==node.station_number && tmp->second.sub_section==node.sub_section)
				{
					tmp->second.m_nCzhPrintFlag=nCzhPrintFlag;
				}
			}
		}
		return true;
	}while(0);

	return false;
}

bool CTGDef::ReadSection(char* ptr,char* seps)
{
	if (*ptr != '[' || strncmp(ptr, SECTION_NODE, strlen(SECTION_NODE))) 
	{ 
		return false;
	}
	ptr = ptr + strlen(SECTION_NODE);

	do
	{
		node_info node;

		// section no 
		token = strtok(ptr, seps);
		if(token==NULL) 
			break;

		int nSectionNo = (int)atoi(token);

		token = strtok(NULL, seps);
		if(token==NULL) 
			break;
		node.sub_section = (int)atoi(token);

		token = strtok(NULL, seps);
		if(token==NULL) 
			break;
		node.index = (int)atoi(token);

		//name
		token = strtok(NULL, seps);
		if(token==NULL) break;
		strncpy(node.text, (const char*)token,31);
		node.text[31] = 0;

		//staton number
		token = strtok(NULL, seps);
		if(token==NULL) break;
		node.station_number =  (int)atoi(token);


		// down line number
		token = strtok(NULL, seps);
		if(token==NULL) break;
		node.down_line_count =  (int)atoi(token);
		//down abs line list
		if(node.down_line_count > LINE_MAX_CNT)
		{
			CString msg;
			msg.Format("配置文件 TG.def中SECTION %d SUB_SECTION %d INDEX %d 的车站%s,下方线号已经溢出(max =20)", nSectionNo, node.sub_section, node.index, node.text);
			MessageBox(NULL, msg, "错误", MB_ICONHAND|MB_OK);
			break;
		}
		for(int i = 0; i < node.down_line_count; i++) 
		{
			token = strtok(NULL, seps);
			if(token==NULL) 
			{
				CString msg;
				msg.Format("配置文件 TG.def中SECTION %d SUB_SECTION %d INDEX %d 的车站%s,下方线号配置数目和实际线号配置不一致", nSectionNo, node.sub_section, node.index, node.text);
				MessageBox(NULL, msg, "错误", MB_ICONHAND|MB_OK);
				break;
			}
			node.down_line_list[i] = (int)atoi(token);
		}

		//up line number
		token = strtok(NULL, seps);
		if(token==NULL) 
			break;
		node.up_line_count =  (int)atoi(token);
		//up abs line list
		if(node.up_line_count > LINE_MAX_CNT)
		{
			CString msg;
			msg.Format("配置文件 TG.def中SECTION %d SUB_SECTION %d INDEX %d 的车站%s,上方线号已经溢出(max =20)", nSectionNo, node.sub_section, node.index, node.text);
			MessageBox(NULL, msg, "错误", MB_ICONHAND|MB_OK);
			break;
		}
		for(int i = 0; i < node.up_line_count; i++) 
		{
			token = strtok(NULL, seps);
			if(token==NULL) 
			{
				CString msg;
				msg.Format("配置文件 TG.def中SECTION %d SUB_SECTION %d INDEX %d 的车站%s,上方线号配置数目和实际线号配置不一致", nSectionNo, node.sub_section, node.index, node.text);
				MessageBox(NULL, msg, "错误", MB_ICONHAND|MB_OK);
				break;
			}
			node.up_line_list[i] = (int)atoi(token);
		}


		//staton space
		token = strtok(NULL, seps);
		if(token==NULL) break;
		node.space =  (int)atoi(token);


		//是否显示股道，0不显示，1股道展开画股道线，2股道展开空白
		token = strtok(NULL, seps);
		if(token==NULL) break;
		node.show_sation_sep = atoi(token);

		//mode为1表示空白，0表示正常显示股道线
		if(node.show_sation_sep == 0) //不显示股道，双击时候先改变为有故道线的形式
			node.show_sides_mode = 1;
		else if(node.show_sation_sep == 1)
			node.show_sides_mode = 0;
		else
			node.show_sides_mode = 1;

		// 1粗线　0细线
		token = strtok(NULL, seps);
		if(token==NULL) break;
		node.isNeedThick = atoi(token);

		//show statistic flag
		token = strtok(NULL, seps);
		if(token==NULL) break;
		node.show_statistic =  ((int)atoi(token) == 0)?false:true;	 
		
		// 上行客车
		token = strtok(NULL, seps);
		if(token==NULL) break;
		node.up_person_train_flag = (int)atoi(token);		 
		
		// 上行货车 
		token = strtok(NULL, seps);
		if(token==NULL) break;
		node.up_goods_train_flag = (int)atoi(token);		 			     

		// 下行客车
		token = strtok(NULL,seps);
		if(token == NULL) break;
		node.down_person_train_flag = (int)atoi(token);		 			     

		// 下行货车
		token = strtok(NULL,seps);
		if(token == NULL) break;
		node.down_goods_train_flag = (int)atoi(token);		 			     
		//*****************************************************************
		//禁止同时接车方向类型
		token = strtok(NULL,seps);
		node.inhibit_sametime_dir = 3;
		if(token != NULL)
			node.inhibit_sametime_dir = (int)atoi(token);		 			     

		// electric_arms;   //0:不绘制(默认) 1:上 2:下 3:上下
		token = strtok(NULL,seps);
		node.electric_arms = 0;	
		if(token != NULL)
			node.electric_arms = (int)atoi(token);		 			     

		std::map<WORD, section_node_info>::iterator it = 
									this->m_mapSection.find(nSectionNo);
		if(it != this->m_mapSection.end())
		{
			if((it->second.m_mapNodes.size() + 1) != node.index)
			{
				CString msg;
				msg.Format("配置文件 TG.def中SECTION %d SUB_SECTION %d INDEX %d 的车站%s,序号不连续", nSectionNo, node.sub_section, node.index, node.text);
				MessageBox(NULL, msg, "错误", MB_ICONHAND|MB_OK);
				return false;
			}
			
			// 
			std::map<int, node_info>::iterator node_it = it->second.m_mapNodes.begin();
			for( ; node_it != it->second.m_mapNodes.end(); node_it++)
			{
				if(node_it->second.station_number == node.station_number)
				{
					it->second.m_vectMutilSta.push_back(node.station_number);
					break;
				}
			}

			it->second.m_mapNodes.insert(make_pair(node.index-1, node));
			it->second.node_count = (WORD)it->second.m_mapNodes.size();
		}
		else 
		{
			if(1 != node.index)
			{
				CString msg;
				msg.Format("配置文件 TG.def中SECTION %d SUB_SECTION %d INDEX %d 的车站%s,序号不是从１开始", nSectionNo, node.sub_section, node.index, node.text);
				MessageBox(NULL, msg, "错误", MB_ICONHAND|MB_OK);
				return false;
			}

			section_node_info info;
			info.section_id = nSectionNo;
			info.m_mapNodes.insert(make_pair(node.index-1, node));
			info.node_count = (WORD)info.m_mapNodes.size();
			this->m_mapSection.insert(make_pair(info.section_id, info));
		}

		return true;
	}while(0);

	return false;
}


bool CTGDef::ReadVirtualStation(char* ptr,char* seps)
{
	if (*ptr != '[' && !strncmp(ptr, "[VIRTUAL_STATION]", strlen("[VIRTUAL_STATION]")) ) 
		return false;

	ptr = ptr + strlen("[VIRTUAL_STATION]");

	do
	{
		TGVirtualStation node;
		// 区段号 
		char* token = strtok(ptr, seps);
		if(token==NULL) 
			break;
		int nSectionNo = (int)atoi(token);

		//序号1
		token = strtok(NULL, seps);
		if(token==NULL) 
			break;
		node.perStationIndex = (int)atoi(token);
		node.perStationIndex = node.perStationIndex -1;

		//序号2
		token = strtok(NULL, seps);
		if(token==NULL) 
			break;
		node.nextStationIndex = (int)atoi(token);
		node.nextStationIndex = node.nextStationIndex -1;

		// 车站名称
		token = strtok(NULL, seps);
		if(token==NULL)
			break;
		strncpy(node.strStatinName, token, 63);
		node.strStatinName[63]=0;

		// 站号
		token = strtok(NULL, seps);
		if(token==NULL)
			break;
		node.nStation = (int)atoi(token);
		
		// 比例
		token = strtok(NULL, seps);
		if(token==NULL)
			break;
		node.nPercent = (int)atoi(token);
		if(node.nPercent<=0 || node.nPercent>=100)
		{
			CString msg;
			msg.Format("配置文件 TG.def中section %d 配置虚拟车站比例为%d (合法值为1-99的任意整数)",  nSectionNo, node.nPercent);
			MessageBox(NULL, msg, "错误", MB_ICONHAND|MB_OK);
			break;
		}
		
		std::map<int , SectionVirtualStation>::iterator it = m_TGVirtualStation.find(nSectionNo);
		if(it != m_TGVirtualStation.end())
		{
			if(it->second.count+1>=128)
			{
				CString msg;
				msg.Format("配置文件 TG.def中section %d 配置虚拟车站个数超过128",  nSectionNo);
				MessageBox(NULL, msg, "错误", MB_ICONHAND|MB_OK);
				break;
			}

			it->second.m_virtualStation[it->second.count]=node;
			it->second.count++;
		}
		else
		{
			SectionVirtualStation sectioninfo;
			sectioninfo.conid=nSectionNo;
			sectioninfo.count=1;
			sectioninfo.m_virtualStation[0]=node;
            
			m_TGVirtualStation[nSectionNo]=sectioninfo;
		}

		return true;
	}while(0);

	return false;
}

bool CTGDef::ReadDefinePath(char* ptr,char* seps)
{
	if (*ptr != '[' || strncmp(ptr, DEFINE_PATH, strlen(DEFINE_PATH)) ) 
	{
		return false;
	}
	ptr = ptr + strlen(DEFINE_PATH);
	//section no 
	token = strtok(ptr, seps);
	if(token==NULL) return false;
	int nSectionNo = (int)atoi(token);

	RouteDefine path;
	
	// load route name 
	token = strtok(NULL, seps);
	if(token==NULL) return false;
	strncpy(path.routeName, token,99);
	path.routeName[99] = 0;

	// load 
	token = strtok(NULL, seps);
	if(token==NULL) return false;
	path.routeFlag = atol(token);
	path.allSame = (path.routeFlag==0x1)?true:false;

	// load station count 
	token = strtok(NULL, seps);
	if(token==NULL) return false;
	path.station_num = (WORD)atoi(token);
	if(path.station_num > sizeof(path.station_no)/sizeof(path.station_no[0]))
	{
		CString msg;
		msg.Format("配置文件 TG.def中DEFINE_PATH %d, %s 配置车站个数超过预定%d个,请修改配置", nSectionNo, path.routeName, sizeof(path.station_no)/sizeof(path.station_no[0]));
		MessageBox(NULL, msg, "错误", MB_ICONHAND|MB_OK);
		return false;
	}
	for(int j=0;j<path.station_num;j++)
	{
		token = strtok(NULL, seps);
		if(token==NULL) return false;
		
		path.station_no[j] = (WORD)atoi(token);
	}

	
	// load in_station_index
	token = strtok(NULL, seps);
	if(token==NULL) return false;
	int uCount = (int)atoi(token);

	if(uCount != path.station_num)
	{
		CString msg;
		msg.Format("配置文件 TG.def中DEFINE_PATH %d, %s 入口索引个数和和车站个数不等", nSectionNo, path.routeName);
		MessageBox(NULL, msg, "错误", MB_ICONHAND|MB_OK);
		return false;
	}

	// load index list 
	for(int j =0;j<uCount;j++)
	{
		token = strtok(NULL, seps);
		if(token==NULL) return false;

		path.in_station_index[j] = (WORD)atoi(token);
	}

	// load out_station_index
	token = strtok(NULL, seps);
	if(token==NULL) return false;
	uCount = (int)atoi(token);

	if(uCount != path.station_num)
	{
		CString msg;
		msg.Format("配置文件 TG.def中DEFINE_PATH %d, %s 出口索引个数和和车站个数不等", nSectionNo, path.routeName);
		MessageBox(NULL, msg, "错误", MB_ICONHAND|MB_OK);
		return false;
	}

	// load index list 
	for(int j =0;j<uCount;j++)
	{
		token = strtok(NULL, seps);
		if(token==NULL) return false;

		path.out_station_index[j] = (WORD)atoi(token);
	}

	std::map<WORD, section_path_define>::iterator it = m_mapRoute.find(nSectionNo);
	if(it != m_mapRoute.end())
	{
		it->second.m_route.push_back(path);
		for(int n=0;n<path.station_num;n++)
		{
			WORD stano = path.station_no[n];
			it->second.m_station[stano] = stano;
		}
	}
	else
	{
		section_path_define section_path;
		section_path.section_id = nSectionNo;

		for(int n=0;n<path.station_num;n++)
		{
			WORD stano = path.station_no[n];
			section_path.m_station[stano] = stano;
		}
		section_path.m_route.push_back(path);

		m_mapRoute.insert(make_pair(nSectionNo, section_path));
	}

	return true;
}

bool CTGDef::ReadABSLine(char* ptr,char* seps)
{
	ptr = ptr + strlen(LINE_DEFINE);

	ABS_LINE line;
	// load line number
	token = strtok(ptr, seps);
	if(token==NULL) return false;
	line.nLineNo = (int)atoi(token);

	// load line name 
	token = strtok(NULL, seps);
	if(token==NULL) return false;
	strncpy(line.sLineName,token,31);
	line.sLineName[31] = 0;

	// load abs no 
	token = strtok(NULL, seps);
	if(token==NULL) return false;
	line.nAbsNo = (int)atoi(token);

	// load start station no 
	token = strtok(NULL, seps);
	if(token==NULL) return false;
	line.nStartStation = (int)atoi(token);

	// load end station no 
	token = strtok(NULL, seps);
	if(token==NULL) return false;
	line.nEndStation = (int)atoi(token);

	// load line direction
	token = strtok(NULL, seps);
	if(token==NULL) return false;
	line.nDirect = (int)atoi(token);

	std::pair<std::map<int, ABS_LINE>::iterator, bool> rs = m_mapAbsLine.insert(make_pair(line.nLineNo, line));
	if(!rs.second)
	{
		CString msg;
		msg.Format("配置文件 TG.def中LINE_DEFINE 线号%d存在重复", line.nLineNo);
		MessageBox(NULL, msg, "错误", MB_ICONHAND|MB_OK);
		return false;	
	}
	return true;
}

bool CTGDef::ReadScreenHeight(char* ptr,char* seps)
{
	ptr = ptr + strlen(HEIGHT_OF_WHOLE_SCREEN);
	token = strtok(ptr, seps);
	if(token==NULL) 
		m_ScreenHeight = 0;
	else
	    m_ScreenHeight = (int)atoi(token);

	return true;
}

bool CTGDef::ReadSectionDisplay(char* ptr,char* seps)
{
	ptr = ptr + strlen(SECTION_DISPLAY);

	SectionDisplay disp;
	token = strtok(ptr, seps);
	if(token==NULL) return false;
	disp.nSection = (int)atoi(token);

	token = strtok(NULL, seps);
	if(token==NULL) return false;
	disp.nScreenTotalHeight = (int)atoi(token);

	token = strtok(NULL, seps);
	if(token==NULL) return false;
	disp.nSideHeight = (int)atoi(token);

	token = strtok(NULL, seps);
	if(token==NULL) return false;
	disp.nUpSpaceHeight = (int)atoi(token);

	token = strtok(NULL, seps);
	if(token==NULL) return false;
	disp.nDownSpaceHeight = (int)atoi(token);

	token = strtok(NULL, seps);
	if(token!=NULL) 
	{
		disp.nSpaceSubSectionCnt = (int)atoi(token);
	}
	else
	{
		disp.nSpaceSubSectionCnt = 0;
	}
	

	if(disp.nSpaceSubSectionCnt>0)
	{
		token = strtok(NULL, seps);
		if(token==NULL) return false;
		int distance = (int)atoi(token);
		disp.nSpaceBetweenSubSection[0] = distance;
	
		for(int idx = 1; idx < disp.nSpaceSubSectionCnt; idx++)
		{
			token = strtok(NULL, seps);
			if(token==NULL) 
				disp.nSpaceBetweenSubSection[idx] = distance;
			else
				disp.nSpaceBetweenSubSection[idx] = (int)atoi(token);

		}
	}

	this->m_section_display_info.push_back(disp);
	return true;
}

bool CTGDef::ReadLocoNoDisplay(char* ptr,char* seps)
{
	ptr = ptr + strlen(LOCO_NO_DISPLAY);

	LocoNoDisplay disp;
	token = strtok(ptr, seps);
	if(token==NULL) return false;
	disp.nSection = (int)atoi(token);

	token = strtok(NULL, seps);
	if(token==NULL) return false;
	disp.nStation = (int)atoi(token);

	token = strtok(NULL, seps);
	if(token==NULL) return false;
	disp.nArrivalIndex = (int)atoi(token);

	token = strtok(NULL, seps);
	if(token==NULL) return false;
	disp.nDepartIndex = (int)atoi(token);

	token = strtok(NULL, seps);
	if(token==NULL) return false;
	disp.nDirect = (int)atoi(token);

	token = strtok(NULL, seps);
	if(token==NULL) return false;
	disp.nAddShow = (int)atoi(token);

	token = strtok(NULL, seps);
	if(token==NULL) return false;
	disp.nSubShow = (int)atoi(token);

	this->m_loco_no_display_list.push_back(disp);
	return true;
}

// 判断兰州局 某站的某方向 的+或者 -机车 是否显示  add_or_sub： 0+ 1-
bool CTGDef::IsLocoNoDisplay(int conid, int station, int arrival_index, int depart_index, int direction, int add_or_sub)
{
	LocoNoDisplay disp;
	for (int i=0; i<m_loco_no_display_list.size(); i++)
	{
		disp = m_loco_no_display_list[i];
		if (disp.nSection == conid && disp.nStation == station && disp.nArrivalIndex == arrival_index
			&& disp.nDepartIndex == depart_index && disp.nDirect == direction)
		{
			if (add_or_sub==0)
				return disp.nAddShow;
			else
				return disp.nSubShow;
		}
	}
	return true;
}
bool CTGDef::ReadDisplayTypeInhibit(char* ptr,char* seps)
{
	ptr = ptr + strlen(DISPLAY_TYPE_INHIBIT);

	token = strtok(ptr, seps);
	if(token==NULL) 
		return false;
	WORD nSection = (WORD)atoi(token);

	INHIBIT_TRAIN traintype;
	token = strtok(NULL, seps);
	
	if(token==NULL) 
		return false;
	traintype.nTypeCount = (int)atoi(token);

	for(int j = 0;j < traintype.nTypeCount; j++)
	{
		token = strtok(NULL, seps);
		if(token==NULL) 
		{
			CString msg;
			msg.Format("配置文件 TG.def中SECTION %d 的DISPLAY_TYPE_INHIBIT个数错误", nSection);
			MessageBox(NULL, msg, "错误", MB_ICONHAND|MB_OK);
			return false;
		}

		traintype.trainType[j] = (WORD)atoi(token);
	}


	this->m_mapInhibitTrainType.insert(make_pair(nSection, traintype));

	return true;
}

bool CTGDef::ReadAbsDirect(char* ptr,char* seps)
{
	ptr = ptr + strlen(REVERSE_ABS);

	CString msg;
    ABS_DIRECT absdirect;

	token = strtok(ptr, seps); // 台号
	if(token==NULL) 
	{	
		msg.Format("配置文件 TG.def中[REVERSE_ABS] 配置不完整, 如果不需要请去掉");
		MessageBox(NULL, msg, "错误", MB_ICONHAND|MB_OK);
		return false;
	}
	absdirect.conid = atoi(token);

	token = strtok(NULL, seps); // 区段名称
	if(token==NULL) 
	{
		msg.Format("配置文件 TG.def中[REVERSE_ABS] 的区段%d 没有配置区段名称", absdirect.conid);
		MessageBox(NULL, msg, "错误", MB_ICONHAND|MB_OK);
		return false;
	}

	char name[100];
	strncpy(name, token, sizeof(name)-1);
	name[sizeof(name)-1] = 0;

	
	token = strtok(NULL, seps); // 车站1
	if(token==NULL) 
	{
		msg.Format("配置文件 TG.def中[REVERSE_ABS] 的区段%d %s 没有配置开始车站站号", absdirect.conid, name);
		MessageBox(NULL, msg, "错误", MB_ICONHAND|MB_OK);
		return false;
	}
	absdirect.station1 = atoi(token);

	token = strtok(NULL, seps); // 车站2
	if(token==NULL) 
	{
		msg.Format("配置文件 TG.def中[REVERSE_ABS] 的区段%d %s 没有配置结束车站站号", absdirect.conid, name);
		MessageBox(NULL, msg, "错误", MB_ICONHAND|MB_OK);
		return false;
	}
	absdirect.station2 = atoi(token);

	token = strtok(NULL, seps); // 车站1的站序
	if(token==NULL) 
	{
		msg.Format("配置文件 TG.def中[REVERSE_ABS] 的区段%d %s 没有配置开始车站站序", absdirect.conid, name);
		MessageBox(NULL, msg, "错误", MB_ICONHAND|MB_OK);
		return false;
	}
	absdirect.index1 = atoi(token);

	token = strtok(NULL, seps); // 车站2的站序
	if(token==NULL) 
	{
		msg.Format("配置文件 TG.def中[REVERSE_ABS] 的区段%d %s 没有配置结束车站站序", absdirect.conid, name);
		MessageBox(NULL, msg, "错误", MB_ICONHAND|MB_OK);
		return false;
	}
	absdirect.index2 = atoi(token);

	token = strtok(NULL, seps); // 线号
	if(token==NULL) 
	{
		msg.Format("配置文件 TG.def中[REVERSE_ABS] 的区段%d %s 没有配置区间方向", absdirect.conid, name);
		MessageBox(NULL, msg, "错误", MB_ICONHAND|MB_OK);
		return false;
	}
	absdirect.lineno = atoi(token);

	m_vecAbsDirect.push_back(absdirect);
		
	return true;
}

bool CTGDef::ReadSectionStopLine(char* ptr,char* seps)
{
	ptr = ptr + strlen(STATION_STOP_LINE);

	CString msg;

	token = strtok(ptr, seps);
	if(token==NULL) 
	{	
		msg.Format("配置文件 TG.def中STATION_STOP_LINE 配置不完整, 如果不需要请去掉");
		MessageBox(NULL, msg, "错误", MB_ICONHAND|MB_OK);
		return false;
	}
	int section = atoi(token);

	token = strtok(NULL, seps);
	if(token==NULL) 
	{
		msg.Format("配置文件 TG.def中STATION_STOP_LINE 的区段%d 没有配置区段名称", section);
		MessageBox(NULL, msg, "错误", MB_ICONHAND|MB_OK);
		return false;
	}

	char name[50];
	strncpy(name, token, sizeof(name)-1);
	name[sizeof(name)-1] = 0;

	StationStopLine stationStopLine;
	token = strtok(NULL, seps);
	if(token==NULL) 
	{
		msg.Format("配置文件 TG.def中STATION_STOP_LINE 的区段%d 没有配置车站站号", section);
		MessageBox(NULL, msg, "错误", MB_ICONHAND|MB_OK);
		return false;
	}
	stationStopLine.station = atoi(token);

	token = strtok(NULL, seps);
	if(token==NULL) 
	{
		msg.Format("配置文件 TG.def中STATION_STOP_LINE 的区段%d 车站%d, 没有配置入口站序", section, stationStopLine.station);
		MessageBox(NULL, msg, "错误", MB_ICONHAND|MB_OK);
		return false;
	}
	stationStopLine.in_index = atoi(token);

	token = strtok(NULL, seps);
	if(token==NULL) 
	{
		msg.Format("配置文件 TG.def中STATION_STOP_LINE 的区段%d 车站%d, 入口站序 %d, 没有配置出口站序", 
			section, stationStopLine.station, stationStopLine.in_index);
		MessageBox(NULL, msg, "错误", MB_ICONHAND|MB_OK);
		return false;
	}
	stationStopLine.out_index = atoi(token);

	token = strtok(NULL, seps);
	if(token==NULL) 
	{
		msg.Format("配置文件 TG.def中STATION_STOP_LINE 的区段%d 车站%d, 入口站序 %d 出口站序%d, 没有配置停车标志", 
			section, stationStopLine.station, stationStopLine.in_index, stationStopLine.out_index);
		MessageBox(NULL, msg, "错误", MB_ICONHAND|MB_OK);
		return false;
	}
	stationStopLine.pass_stop_flag = atoi(token);

	bool bFind = false;
	for(int i = 0; i < m_vecSectionStopLine.size(); i++)
	{
		if(m_vecSectionStopLine[i].section == section)
		{
			m_vecSectionStopLine[i].station_stop_line.push_back(stationStopLine);
			bFind = true;
			break;
		}
	}

	if(!bFind)
	{
		SectionStopLine sectionStopLine;
		sectionStopLine.section = section;
		strncpy(sectionStopLine.name, name, sizeof(sectionStopLine.name)-1);
		sectionStopLine.name[sizeof(sectionStopLine.name)-1] = 0;
		sectionStopLine.station_stop_line.push_back(stationStopLine);

		m_vecSectionStopLine.push_back(sectionStopLine);
	}
		
	return true;
}

section_node_info::section_node_info()
{
	section_id = 0;
	node_count = 0;
	sub_section_count = 0;
}
section_node_info::section_node_info(const section_node_info& rhs)
{
	this->section_id = rhs.section_id;
	this->node_count = rhs.node_count;
	this->m_mapNodes.clear();
	this->m_vectMutilSta.clear();

	std::map<int, node_info>::const_iterator it = rhs.m_mapNodes.begin();
	for( ; it != rhs.m_mapNodes.end(); it++)
		this->m_mapNodes.insert(make_pair(it->first,it->second));
	
	for(int i = 0; i < rhs.m_vectMutilSta.size(); i++)
		this->m_vectMutilSta.push_back(rhs.m_vectMutilSta[i]);
}

INHIBIT_TRAIN::INHIBIT_TRAIN()
{
	nTypeCount = 0;
}

section_path_define::section_path_define()
{
	section_id = 0;
	m_route.clear();
	m_station.clear();
}

section_path_define::~section_path_define()
{
	section_id = 0;
	m_route.clear();
	m_station.clear();
}

section_path_define::section_path_define(const section_path_define& rhs)
{
	this->section_id = rhs.section_id;
	for(int i=0; i < rhs.m_route.size(); i++)
	{
		this->m_route.push_back(rhs.m_route[i]);
	}

	std::map<WORD, WORD>::const_iterator it = rhs.m_station.begin();
	for(; it != rhs.m_station.end(); it++)
	{
		this->m_station[it->first] = it->second;
	}
}

section_path_define& section_path_define::operator=(const section_path_define& rhs)
{
	if(this == &rhs)
		return *this;

	this->section_id = rhs.section_id;
	this->m_route.clear();
	this->m_station.clear();

	for(int i=0; i < rhs.m_route.size(); i++)
	{
		this->m_route.push_back(rhs.m_route[i]);
	}

	std::map<WORD, WORD>::const_iterator it = rhs.m_station.begin();
	for(; it != rhs.m_station.end(); it++)
	{
		this->m_station[it->first] = it->second;
	}

	return *this;
}

bool CTGDef::GetRouteDefine(WORD section_id, section_path_define& route_define)
{
	std::map<WORD, section_path_define>::iterator it = m_mapRoute.find(section_id);
	if(it != m_mapRoute.end())
	{
		route_define = it->second;
		return true;
	}

	return false;
}

bool CTGDef::IsDefinePathStation(WORD section_id, WORD sta_no)
{
	std::map<WORD, section_path_define>::iterator it = m_mapRoute.find(section_id);
	if(it != m_mapRoute.end())
	{
		std::map<WORD, WORD>::iterator it_sta = it->second.m_station.find(sta_no);
		if(it_sta != it->second.m_station.end())
			return true;
	}

	return false;
}

bool CTGDef::IsCalcInOutStaSection(int section)
{
	for (int i=0;i<m_inOutStationIndexSection.size();i++) 
	{
		if(m_inOutStationIndexSection[i] == section)
			return false;
	}

	return true;
}

void CTGDef::GetStationList(WORD nSection, int& stationNum, int* stationList)
{
	stationNum=0;

	if (stationList==NULL) 
	{
		return;
	}
	

	std::map<WORD,section_node_info>::const_iterator it = m_mapSection.find(nSection);
	if (it != m_mapSection.end()) 
	{
		std::map<int, node_info>::const_iterator it_sta = it->second.m_mapNodes.begin();
		for (;it_sta !=it->second.m_mapNodes.end(); it_sta++) 
		{
			int stano = it_sta->second.station_number;
			for(int i = 0; i < stationNum; i++)
			{
				if (stano == stationList[i]) 
				{
					break;
				}
			}

			if (i >= stationNum) 
			{
				stationList[stationNum] = stano;
				stationNum++;
			}
		}
	}
}
	
const SectionEntryDefine* CTGDef::GetSectionEntryDefine(const int nSection)
{
	std::map<int, SectionEntryDefine>::const_iterator it = m_mapEntryDefine.find(nSection);  
	if(it != m_mapEntryDefine.end() && it->second.section == nSection)
	{
		return &it->second;
	}

	return NULL;
}

int CTGDef::GetInitAbsLineNo(int conid, int sta1, int sta2, int index1, int index2)
{
	for(int idx=0; idx<m_vecAbsDirect.size();idx++)
	{
		if(m_vecAbsDirect[idx].index1 == index1 && 
			m_vecAbsDirect[idx].index2 == index2 && 
			m_vecAbsDirect[idx].station1 == sta1 && 
			m_vecAbsDirect[idx].station2 == sta2 &&
			m_vecAbsDirect[idx].conid == conid)
		{
			return m_vecAbsDirect[idx].lineno;
		}
	}

	return -1;
}

bool CTGDef::ReadStationKiloMk(char* ptr,char* seps)
{
	if (*ptr != '[' || strncmp(ptr, STATION_KILOMK, strlen(STATION_KILOMK))) 
	{ 
		return false;
	}
	ptr = ptr + strlen(STATION_KILOMK);
	
	token = strtok(ptr, seps);
	if(token==NULL) 
	{
		AfxMessageBox("TG.def中[STATION_KILOMK]不是有效配置项,如果不需要请注释掉");
		return false;
	}

	STA_KILO_MK sta;
	// 台号
	int con_id = atoi(token);
        
	// 车站序号
	sta.index = 0;
	token = strtok(NULL, seps);
	if(token != NULL) 
		sta.index = atoi(token);

	// 车站号
	sta.station = 0;
	token = strtok(NULL, seps);
	if(token != NULL) 
		sta.station = atoi(token);	

	// 车站公里标
	token = strtok(NULL, seps);
	if(token != NULL) 
		strncpy(sta.kilo, token, sizeof(sta.kilo)-1);
	sta.kilo[63]=0;

	std::map<int, SECTION_STA_KILO>::iterator it=m_mapStationKilo.find(con_id);
	if(it!=m_mapStationKilo.end())
	{
		if(it->second.num<32)
		{
			it->second.sta_kilo[it->second.num]=sta;
			it->second.num++;
		}
	}
	else
	{
		SECTION_STA_KILO section;
		section.con_id=con_id;
		section.num=1;
		section.sta_kilo[0]=sta;

		m_mapStationKilo.insert(make_pair(section.con_id, section));
	}

	return true;
}

bool CTGDef::ReadTrainDisplay(char* ptr,char* seps)
{
	if (*ptr != '[' || strncmp(ptr, TRAIN_DISPLAY, strlen(TRAIN_DISPLAY))) 
	{ 
		return false;
	}
	ptr = ptr + strlen(TRAIN_DISPLAY);
	
	token = strtok(ptr, seps);
	if(token==NULL) 
	{
		AfxMessageBox("TG.def中[TRAIN_DISPLAY]不是有效配置项,如果不需要请注释掉");
		return false;
	}

	// 台号
	int con_id = atoi(token);
        
	// 开始车站
	int begin_station = 0;
	token = strtok(NULL, seps);
	if(token != NULL) 
		begin_station = atoi(token);	

	// 结束车站
	int end_station = 0;
	token = strtok(NULL, seps);
	if(token != NULL) 
		end_station = atoi(token);
	
	// 开始车站站序
	int begin_index = 0;
	token = strtok(NULL, seps);
	if(token != NULL) 
		begin_index = atoi(token);
	
	// 结束车站站序
	int end_index = 0;
	token = strtok(NULL, seps);
	if(token != NULL) 
		end_index = atoi(token);
	
	std::map<int, TRAIN_DIRECT_DEFINE>::iterator it = m_mapShowDirect.find(con_id);
	if(it != m_mapShowDirect.end())
	{
		if(it->second.direct_num+1 <= sizeof(it->second.direct)/sizeof(it->second.direct[0]))
		{
			it->second.direct[it->second.direct_num].begin_station=begin_station;
			it->second.direct[it->second.direct_num].end_station=end_station;
			it->second.direct[it->second.direct_num].begin_index=begin_index;
			it->second.direct[it->second.direct_num].end_index=end_index;
			it->second.direct_num++;
		}
	}
	else
	{
		TRAIN_DIRECT_DEFINE train_define;
		train_define.con_id=con_id;
		train_define.direct_num=1;
		train_define.direct[0].begin_station=begin_station;
		train_define.direct[0].end_station=end_station;
		train_define.direct[0].begin_index=begin_index;
		train_define.direct[0].end_index=end_index;
		
		m_mapShowDirect.insert(make_pair(con_id, train_define));
	}
	
	return true;
}
	
bool CTGDef::GetTrainDirectDefine(int con_id, TRAIN_DIRECT_DEFINE& define)
{
	std::map<int, TRAIN_DIRECT_DEFINE>::iterator it = m_mapShowDirect.find(con_id);
	if(it != m_mapShowDirect.end())
	{
		define.con_id = it->second.con_id;
		define.direct_num = it->second.direct_num;
		for(int i = 0; i < it->second.direct_num; i++)
		{
			define.direct[i].begin_station = it->second.direct[i].begin_station;
			define.direct[i].end_station = it->second.direct[i].end_station;
			define.direct[i].begin_index = it->second.direct[i].begin_index;
			define.direct[i].end_index = it->second.direct[i].end_index;
		}
		
		return true;
	}

	return false;
}

bool CTGDef::ReadStationLineColorDisplay(char* ptr,char* seps)
{
	if (*ptr != '[' || strncmp(ptr, STATION_LINE_COLOR, strlen(STATION_LINE_COLOR))) 
	{ 
		return false;
	}
	ptr = ptr + strlen(STATION_LINE_COLOR);
	
	token = strtok(ptr, seps);
	if(token==NULL) 
		return false;
	
	CString str;

	int con_id = atoi(token);
        
	token = strtok(NULL, seps);
	if(token==NULL) 
	{
		str.Format("tg.def中[STATION_LINE_COLOR]项,%d台 缺少站序配置", 
			con_id);
		AfxMessageBox(str);
		return false;
	}

	int index = atoi(token);
        
	

	token = strtok(NULL, seps);
	if(token==NULL) 
	{
		str.Format("tg.def中[STATION_LINE_COLOR]项,%d台 站序%d RGB中缺少red配置", 
			con_id, index);
		AfxMessageBox(str);
		return false;
	}

	
	int red = atoi(token);
	if(red < 0 || red > 255)
	{
		str.Format("tg.def中[STATION_LINE_COLOR]项,%d台 站序%d RGB中red配置不正确(合法值0<=red<=255)", 
			con_id, index);
		AfxMessageBox(str);
		return false;
	}
	token = strtok(NULL, seps);
	if(token==NULL) 
	{
		str.Format("tg.def中[STATION_LINE_COLOR]项,%d台 站序%d RGB中缺少green配置", 
			con_id, index);
		AfxMessageBox(str);
		return false;
	}

	
	int green = atoi(token);
	if(green < 0 || green > 255)
	{
		str.Format("tg.def中[STATION_LINE_COLOR]项,%d台 站序%d RGB中green配置不正确(合法值0<=green<=255)", 
			con_id, index);
		AfxMessageBox(str);
		return false;
	}

	token = strtok(NULL, seps);
	if(token==NULL) 
	{
		str.Format("tg.def中[STATION_LINE_COLOR]项,%d台 站序%d RGB中缺少blue配置", 
			con_id, index);
		AfxMessageBox(str);
		return false;
	}

	int blue = atoi(token);
	if(blue < 0 || blue > 255)
	{
		str.Format("tg.def中[STATION_LINE_COLOR]项,%d台 站序%d RGB中blue配置不正确(合法值0<=blue<=255)", 
			con_id, index);
		AfxMessageBox(str);
		return false;
	}

	COLORREF clr = RGB(red,green,blue);

	std::map<int, CON_STATION_COLOR>::iterator it = m_mapStationLineColor.find(con_id);
	if(it == m_mapStationLineColor.end())
	{
		CON_STATION_COLOR con_clr;
		con_clr.con_id = con_id;
		con_clr.station_clr[0].index=index;
		con_clr.station_clr[0].clr=clr;
		con_clr.station_num=1;
		m_mapStationLineColor.insert(make_pair(con_id, con_clr));
	}
	else
	{
		if(it->second.station_num<255)
		{
			it->second.station_clr[it->second.station_num].index=index;
			it->second.station_clr[it->second.station_num].clr=clr;
			it->second.station_num++;
		}
	}

	return true;
}

CString CTGDef::GetKiloMk(int conid, int index, int station)
{
	std::map<int, SECTION_STA_KILO>::iterator it=m_mapStationKilo.find(conid);
	if(it!=m_mapStationKilo.end())
	{
		for(int i=0; i<it->second.num; i++)
		{
			int nIndex=it->second.sta_kilo[i].index;
			int nStation=it->second.sta_kilo[i].station;
			if(nIndex==index && nStation==station)
			{
				return it->second.sta_kilo[i].kilo;
			}
		}
	}

	return "";
}

int CTGDef::GetSpecialExitEntryLineNo(int conid, int sta1, int sta2, int index1, int index2, int absno[])
{
	for(int idx=0; idx<m_vecSpecialExitEntry.size();idx++)
	{
		if(	m_vecSpecialExitEntry[idx].station1 == sta1 && 
			m_vecSpecialExitEntry[idx].station2 == sta2 &&
			m_vecSpecialExitEntry[idx].conid == conid)
		{
			for(int c=0; c<m_vecSpecialExitEntry[idx].lineno_count; c++)
			{
				absno[c]=m_vecSpecialExitEntry[idx].lineno[c];
			}

			return m_vecSpecialExitEntry[idx].lineno_count;
		}
	}

	return -1;
}


bool CTGDef::ReadSpecialExitEntry(char* ptr,char* seps)
{
	ptr = ptr + strlen(SPECIAL_EXIT_ENTRY);

	CString msg;
    SPECIAL_EXIT_ENTRY_INFO specialenrty;

	token = strtok(ptr, seps);
	if(token==NULL) 
	{
		return false;
	}
	specialenrty.conid = atoi(token);

	token = strtok(NULL, seps);
	if(token==NULL) 
	{
		return false;
	}

	char name[100];
	strncpy(name, token, sizeof(name)-1);
	name[sizeof(name)-1] = 0;

	token = strtok(NULL, seps);
	if(token==NULL) 
	{
		return false;
	}
	specialenrty.station1 = atoi(token);

	token = strtok(NULL, seps);
	if(token==NULL) 
	{
		return false;
	}
	specialenrty.station2 = atoi(token);

	token = strtok(NULL, seps);
	if(token==NULL) 
	{
		return false;
	}
	specialenrty.index1 = atoi(token);

	token = strtok(NULL, seps);
	if(token==NULL) 
	{
		return false;
	}
	specialenrty.index2 = atoi(token);

	token = strtok(NULL, seps);
	if(token==NULL) 
	{
		return false;
	}
	int num=atoi(token);
	for(int d=0; d<num&&d<5; d++)
	{
		token = strtok(NULL, seps);
		if(token==NULL) 
			break;
		specialenrty.lineno[d] = atoi(token);
	}
	specialenrty.lineno_count=d;

	m_vecSpecialExitEntry.push_back(specialenrty);
		
	return true;
}

bool CTGDef::GetEntryExit(int conid, int sub, int index, int station, int& entry, int& exit)
{
	for(int i=0; i<m_vecNodeEntry.size(); i++)
	{
		if(m_vecNodeEntry[i].conid==conid && m_vecNodeEntry[i].sub_table==sub 
			&& m_vecNodeEntry[i].index==index && m_vecNodeEntry[i].station==station)
		{
			entry=m_vecNodeEntry[i].entry;
			exit=m_vecNodeEntry[i].exit;

			return true;
		}
	}

	return false;
}

bool CTGDef::IsNoCheckMinLateSection(int section)
{
	for (int i=0; i<m_vecNoCheckMinLateSectionList.size(); i++)
	{
		if (m_vecNoCheckMinLateSectionList[i] = section)
			return true;
	}

	return false;
}

bool CTGDef::ReadFromToLineNo(char* ptr,char* seps)
{
	if (*ptr != '[' || strncmp(ptr, STATION_NEXTSTATION_LINENO, strlen(STATION_NEXTSTATION_LINENO))) 
	{ 
		return false;
	}
	ptr = ptr + strlen(STATION_NEXTSTATION_LINENO);

	do
	{
		Entry_Exit_Default info;

		token = strtok(ptr, seps);
		if(token==NULL) 
			break;
		info.section =  (int)atoi(token);

		token = strtok(NULL, seps);
		if(token==NULL) 
			break;
		info.station = (int)atoi(token);
		
		token = strtok(NULL, seps);
		if(token==NULL) 
			break;
		info.nextstation = (int)atoi(token);

		token = strtok(NULL, seps);
		if(token==NULL) 
			break;
		info.line_num = (int)atoi(token);

		m_entryset.push_back(info);
		
		return true;
	}while(0);
	return false;
}


bool CTGDef::ReadDirectionTip(char* ptr,char* seps)
{
	if (*ptr != '[' || strncmp(ptr, STATION_DIRECTION_TIP, strlen(STATION_DIRECTION_TIP))) 
	{ 
		return false;
	}
	ptr = ptr + strlen(STATION_DIRECTION_TIP);

	do
	{
		Station_Direction_Tip info;

		token = strtok(ptr, seps);
		if(token==NULL) 
			break;
		info.section = (int)atoi(token);

		token = strtok(NULL, seps);
		if(token==NULL) 
			break;
		info.station = (int)atoi(token);

		token = strtok(NULL, seps);
		if(token==NULL) 
			break;
		info.next_station = (int)atoi(token);

		token = strtok(NULL, seps);
		if(token==NULL) break;
		strncpy(info.tip, token, 10);
		info.tip[10]=0;

		m_direction_tip.push_back(info);

		return true;
	}while(0);
	return false;
}

bool CTGDef::ReadGiveoutTip(char* ptr,char* seps)
{
	if (*ptr != '[' || strncmp(ptr, STATION_GIVEOUT_TIP, strlen(STATION_GIVEOUT_TIP))) 
	{ 
		return false;
	}
	ptr = ptr + strlen(STATION_GIVEOUT_TIP);

	do
	{
		StationGiveoutTip info;

		token = strtok(ptr, seps);
		if(token==NULL) 
			break;
		info.section = (int)atoi(token);

		token = strtok(NULL, seps);
		if(token==NULL) 
			break;
		info.station = (int)atoi(token);

		token = strtok(NULL, seps);
		if(token==NULL) 
			break;
		info.exit = (int)atoi(token);

		token = strtok(NULL, seps);
		if(token==NULL) break;
		strncpy(info.tip, token, 10);
		info.tip[10]=0;

		m_giveout_tip.push_back(info);

		return true;
	}while(0);
	return false;
}

bool CTGDef::ReadAcceptTip(char* ptr,char* seps)
{
	if (*ptr != '[' || strncmp(ptr, STATION_ACCEPT_TIP, strlen(STATION_ACCEPT_TIP))) 
	{ 
		return false;
	}
	ptr = ptr + strlen(STATION_ACCEPT_TIP);

	do
	{
		StationAcceptTip info;

		token = strtok(ptr, seps);
		if(token==NULL) 
			break;
		info.section = (int)atoi(token);

		token = strtok(NULL, seps);
		if(token==NULL) 
			break;
		info.station = (int)atoi(token);

		token = strtok(NULL, seps);
		if(token==NULL) 
			break;
		info.entry = (int)atoi(token);

		token = strtok(NULL, seps);
		if(token==NULL) break;
		strncpy(info.tip, token, 10);
		info.tip[10]=0;

		m_accept_tip.push_back(info);

		return true;
	}while(0);
	return false;
}

int  CTGDef::GetLineNo(int section, int from_st[32], int to_st[32], int lineno[32])
{
	int count=0;
	for(int i=0; i<m_entryset.size(); i++)
	{
		if(m_entryset[i].section==section)
		{
			from_st[count]=m_entryset[i].station;
			to_st[count]=m_entryset[i].nextstation;
			lineno[count]=m_entryset[i].line_num;
			count++; 
			if(count>=31)
			{
				break;
			}
		}
	}

	return count;
}

void CTGDef::GetSectionDirectTip(int section, vector<Station_Direction_Tip>& direction_tip)
{
	direction_tip.clear();
	for(int i=0; i<m_direction_tip.size(); i++)
	{
		if(m_direction_tip[i].section==section)
		{
			direction_tip.push_back(m_direction_tip[i]);
		}
	}
}

void CTGDef::GetStationGiveoutTip(int section, vector<StationGiveoutTip>& giveout_list)
{
	giveout_list.clear();

	for(int i=0; i<m_giveout_tip.size(); i++)
	{
		if(m_giveout_tip[i].section==section)
		{
			giveout_list.push_back(m_giveout_tip[i]);
		}
	}
}

void CTGDef::GetStationAcceptTip(int section, vector<StationAcceptTip>& accept_list)
{
	accept_list.clear();

	for(int i=0; i<m_accept_tip.size(); i++)
	{
		if(m_accept_tip[i].section==section)
		{
			accept_list.push_back(m_accept_tip[i]);
		}
	}
}

