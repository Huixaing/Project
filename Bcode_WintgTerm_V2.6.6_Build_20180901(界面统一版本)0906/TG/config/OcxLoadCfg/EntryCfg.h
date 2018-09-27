#pragma once

struct StationEntryAbsnum
{
	int entry_num;
	int abs_num;
	int dir;
	int usSpared;
	StationEntryAbsnum()
	{
		entry_num = 0;
		abs_num = 0;
		dir = 0;
		usSpared = 0;
	}
	~StationEntryAbsnum()
	{
		entry_num = 0;
		abs_num = 0;
		dir = 0;
		usSpared = 0;
	}
};

struct SpecialStation
{
	int station;
	std::vector<StationEntryAbsnum> entryVect;

	SpecialStation();
	SpecialStation(const SpecialStation& rhs);
	SpecialStation& operator=(const SpecialStation& rhs);
	~SpecialStation();
	void Clear();
};
class EntryCfg
{
public:
	EntryCfg(void);
	~EntryCfg(void);
public:
	bool Loadentry(CString pathname, COleControl* ctrl);
	bool ConvertAbsnumStationToEntry(WORD station,WORD abs_num,int *entry_num);
	bool ConvertStationEntryToAbsnum(WORD station, WORD entry_num,WORD *abs_num,WORD *dir);
	int GetAbsByStationNoAndEntry(WORD station, WORD entry);
private:
    std::map<int, SpecialStation> stationMap;
};
