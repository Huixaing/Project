#pragma once
#include <map>
#include <vector>
using namespace std;

struct SIDE_INFO
{
	int  sideno;
	char sidename[20];
	bool bCrhSide;  // 是否动车组股道
	BYTE nPlatform; // 站台 0: 无 1:有 2:高站台
	short side_property;
	short direct; // 1:上行; 2:下行;
	SIDE_INFO()
	{
		sideno = -1;
		memset(sidename, 0, 20);
		bCrhSide = false;
		nPlatform=0;
		side_property=21;
		direct=0;
	}
};

struct LINE_INFO 
{                	
	int  stano;
	bool fHigh;                                                     
	std::vector<SIDE_INFO> vecSideInfo;
	
	LINE_INFO();
	~LINE_INFO();
	LINE_INFO(const LINE_INFO& rhs);
	LINE_INFO& operator=(const LINE_INFO& rhs);
	void Clear();
};   

class CLineCfg
{
public:
	CLineCfg(void);
	~CLineCfg(void);

	void ClearLineCfg();
	void AddLineCfg(const LINE_INFO& line);
public:
	bool IsCRHSide(int stano, int sideno);
	bool  GetStaLineCfg(int staid, LINE_INFO& line);
private:
	std::map<int, LINE_INFO> mapStaSideInfo;
};
