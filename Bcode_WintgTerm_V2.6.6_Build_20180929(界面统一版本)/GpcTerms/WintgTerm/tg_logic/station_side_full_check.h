#pragma once

class SideOccupyInfo
{
	struct OccupyTime
	{
		TIME btm;
		TIME etm;
	};
public:
	int sideno;
	int count;
	OccupyTime occupy[512];
public:
	SideOccupyInfo()
	{
		Init();
	}
	void Init()
	{
		sideno=0;
		count=0;
		memset(occupy, 0, sizeof(occupy));
	}
	void AddOccupy(TIME a, TIME d)
	{
		if(count<512)
		{
			occupy[count].btm=a;
			occupy[count].etm=d;
			count++;
		}
	}
};

class StationOccupyInfo
{
public:
	int station;
	int side_count;
	SideOccupyInfo side_list[256];
	
public:
	StationOccupyInfo();
	void Init();
	void AddSideOccupy(int sideno, TIME a, TIME d);
	//bool CheckWithSide(int i);
	void GetSideOccupyFull(CStringArray& list);
};

class StationSideOccupyCheck
{
public:
	StationSideOccupyCheck();

public:
	void Init();
	void AddStationSideOccupy(int station, int sideno, TIME a, TIME d); 
	void GetStationSideOccupyFull(CStringArray& list);
private:
	int m_nStationCount;
	StationOccupyInfo stationOccupy[128];
};

extern StationSideOccupyCheck* GetSideFullCheck();