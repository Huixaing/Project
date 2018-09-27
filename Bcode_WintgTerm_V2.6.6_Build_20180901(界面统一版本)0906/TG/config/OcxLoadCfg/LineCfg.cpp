#include "StdAfx.h"
#include "TGCtrl.h"
#include ".\linecfg.h"

CLineCfg::CLineCfg(void)
{
}

CLineCfg::~CLineCfg(void)
{
	this->mapStaSideInfo.clear();
}

bool CLineCfg::IsCRHSide(int stano, int sideno)
{
	std::map<int, LINE_INFO>::const_iterator itsta = mapStaSideInfo.find(stano);
	if(itsta != mapStaSideInfo.end())
	{
		for(int i = 0; i < itsta->second.vecSideInfo.size(); i++)
		{
			if(sideno == itsta->second.vecSideInfo[i].sideno)
			{
				return itsta->second.vecSideInfo[i].bCrhSide;
			}
		}
	}

	return false;
}


bool CLineCfg::GetStaLineCfg(int staid, LINE_INFO& line)
{
	std::map<int, LINE_INFO>::const_iterator it = mapStaSideInfo.find(staid);
	if(it != mapStaSideInfo.end())
	{
		line = it->second;
		return true;
	}

	return false;
}

void CLineCfg::ClearLineCfg()
{
	mapStaSideInfo.clear();
}

void CLineCfg::AddLineCfg(const LINE_INFO& line)
{
	std::pair<std::map<int, LINE_INFO>::iterator, bool> rst = this->mapStaSideInfo.insert(make_pair(line.stano, line));
	if(!rst.second)
	{
		CString msg;
		msg.Format("配置文件StationSide.def中 STATION %d 已经存在, 站号重复", line.stano);
		MessageBox(NULL, msg, "错误", MB_ICONHAND|MB_OK);
	}
}

LINE_INFO::LINE_INFO()
{
	Clear();  
}
LINE_INFO::~LINE_INFO()
{
	Clear();
}
LINE_INFO::LINE_INFO(const LINE_INFO& rhs)
{
	this->stano = rhs.stano;
	this->fHigh = rhs.fHigh;

	for(int i=0; i<rhs.vecSideInfo.size();i++)
	{
		this->vecSideInfo.push_back(rhs.vecSideInfo[i]);
	}
}
LINE_INFO& LINE_INFO::operator=(const LINE_INFO& rhs)
{
	if(this == (&rhs))
		return *this;

	Clear();

	this->stano = rhs.stano;
	this->fHigh = rhs.fHigh;

	for(int i=0; i<rhs.vecSideInfo.size();i++)
	{
		this->vecSideInfo.push_back(rhs.vecSideInfo[i]);
	}

	return *this;
}

void LINE_INFO::Clear()
{
	fHigh = false;                                                     
	this->stano = 0;
	this->vecSideInfo.clear();
}