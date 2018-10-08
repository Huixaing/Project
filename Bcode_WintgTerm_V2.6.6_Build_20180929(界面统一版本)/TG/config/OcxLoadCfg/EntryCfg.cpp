#include "StdAfx.h"
#include "TGCtrl.h"
#include ".\entrycfg.h"

EntryCfg::EntryCfg(void)
{
	stationMap.clear();
}

EntryCfg::~EntryCfg(void)
{
	stationMap.clear();
}

int EntryCfg::GetAbsByStationNoAndEntry(WORD station, WORD entry)
{
	std::map<int, SpecialStation>::const_iterator it = stationMap.find(station);
    
	if (it != stationMap.end())
	{
		for(int i=0; i < it->second.entryVect.size(); i++) 
		{
			if(it->second.entryVect[i].entry_num == entry)
				return it->second.entryVect[i].abs_num;
		}
	}

	return -1;
}

bool EntryCfg::ConvertStationEntryToAbsnum(WORD station, WORD entry_num,
										  WORD *abs_num,WORD *dir)
{
	if(abs_num == NULL || dir == NULL)
		return false;

	std::map<int, SpecialStation>::const_iterator it = stationMap.find(station);
    
	if (it == stationMap.end())
	{
		if(entry_num == 1 || entry_num == 2) 
		{
			*abs_num = station;
			if (entry_num == 1)
				*dir = 0;
			else
				*dir = 1;
		}
		else
		{
			*abs_num = station + 1;
			if (entry_num == 3)
				*dir = 0;
			else
				*dir = 1;
		}

		return true;
	}

	std::vector<StationEntryAbsnum>::const_iterator ittmp 
								= it->second.entryVect.begin();
	while (ittmp != it->second.entryVect.end())
	{
		if ((*ittmp).entry_num == entry_num)
		{
			*abs_num = (*ittmp).abs_num;
			*dir = (*ittmp).dir;

			return true;
		}
		ittmp++;
	}

	return false;
}

bool EntryCfg::ConvertAbsnumStationToEntry(WORD station,WORD abs_num,int *entry_num)
{
	if(entry_num == NULL)
		return false;
	*entry_num = -1;

	std::map<int, SpecialStation>::const_iterator it = stationMap.find(station);

	if (it == stationMap.end())
	{
		return false;
	}

	std::vector<StationEntryAbsnum>::const_iterator ittmp 
		= it->second.entryVect.begin();
	while (ittmp != it->second.entryVect.end())
	{
		if ((*ittmp).abs_num == abs_num)
		{
			*entry_num = (*ittmp).entry_num;
			
			return true;
		}
		ittmp++;
	}

	return false;
}

bool EntryCfg::Loadentry(CString pathname, COleControl* ctrl)
{
	stationMap.clear();
    CTGCtrl* ptrl = (CTGCtrl*)ctrl;

	int entrys = -1;
	int station;  
	int entry_num,abs_num,dir;	
    int station_count = 0;

	FILE* fp;
	char seps[]=" ,=\t\n", *ptr, *substr;
	char strLine[512];
	fp = fopen(pathname,"r");
	if (fp == NULL)
	{
		CString tmp;
		tmp.Format("严重错误:%s打开失败", pathname);
		ptrl->FIRE_TG_LOG(tmp);
		AfxMessageBox(tmp);
		return false;
	}

	SpecialStation spStation;
	while(!feof(fp)) 
	{
		ptr = fgets(strLine, 512, fp);
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
			station_count = (int)atoi(strtok(NULL, seps));
		} 
		else if(substr=strstr(ptr, "STATION_ID")) 
		{
			if (entrys >= 0)
			{
				if (entrys != spStation.entryVect.size())
				{
					CString tmp;
					tmp.Format("严重错误:%d车站,出入口配置数目(%d)与实际配置(%d)不一致", 
						spStation.station, entrys, spStation.entryVect.size());
					ptrl->FIRE_TG_LOG(tmp);
					AfxMessageBox(tmp);
					return false;
				}
				entrys = -1;
				stationMap.insert(make_pair(spStation.station, spStation));
				spStation.Clear();
			}

			sscanf(substr,"STATION_ID=%d, ENTRY_COUNT=%d\n",&station,&entrys);
			spStation.station = station;
		} 
		else if(substr=strstr(ptr, "ENTRY")) 
		{
			sscanf(substr,"ENTRY=%d, ABS_NUM=%d, DIRECTION=%d\n",
				&entry_num,&abs_num,&dir);
			
			StationEntryAbsnum absInfo;
			absInfo.entry_num = entry_num;
			absInfo.abs_num = abs_num;
			absInfo.dir = dir;
			
			spStation.entryVect.push_back(absInfo);
		} 
		else 
			continue;
	}
	
	fclose(fp);

	if (entrys >= 0)
	{
		if (entrys != spStation.entryVect.size())
		{
			CString tmp;
			tmp.Format("严重错误:%d车站,出入口配置数目(%d)与实际配置(%d)不一致", 
				spStation.station, entrys, spStation.entryVect.size());
			ptrl->FIRE_TG_LOG(tmp);
			AfxMessageBox(tmp);
			return false;
		}
		entrys = -1;
		stationMap.insert(make_pair(spStation.station, spStation));
		spStation.Clear();
	}

	if(station_count != stationMap.size())
	{
		CString tmp;
		tmp.Format("严重错误:%s, 出入口配置车站数目(%d)与实际车站配置数目(%d)不一致", 
			pathname, station_count, stationMap.size());
		ptrl->FIRE_TG_LOG(tmp);
		AfxMessageBox(tmp);
		return false;
	}
	
	return true;
}


SpecialStation::SpecialStation()
{
	station = 0;
	entryVect.clear();
}
SpecialStation::SpecialStation(const SpecialStation& rhs)
{
	station = rhs.station;

	std::vector<StationEntryAbsnum>::const_iterator it = rhs.entryVect.begin();
	while (it != rhs.entryVect.end())
	{
		entryVect.push_back(*it);
		it++;
	}
}
SpecialStation& SpecialStation::operator=(const SpecialStation& rhs)
{
	if (this == &rhs)
	{
		return *this;
	}
	station = rhs.station;
	entryVect.clear();
    
	std::vector<StationEntryAbsnum>::const_iterator it = rhs.entryVect.begin();
	while (it != rhs.entryVect.end())
	{
		entryVect.push_back(*it);
		it++;
	}
	
	return *this;
}
SpecialStation::~SpecialStation()
{
	station = 0;
	entryVect.clear();
}

void SpecialStation::Clear()
{
	station = 0;
	entryVect.clear();
}