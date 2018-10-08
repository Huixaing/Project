#pragma once
#include <vector>
#include "utility/xmlview.h"
#include "tg_basicpara.h"
#include "tg_schedule_base.h"

#import  <msxml.dll> named_guids
using namespace MSXML;

#define SetPassNotMainSide(side_valide)          (side_valide |= 0x0001)              //bit 0
#define IsPassNotMainSide(side_valide)           (side_valide & 0x0001)              //bit 0

#define SetSideDisconnectEntry(side_valide)          (side_valide |= 0x0002)              //bit 1
#define IsSideDisconnectEntry(side_valide)           (side_valide & 0x0002)              //bit 1

#define SetSideNotSame(side_valide)          (side_valide |= 0x0004)              //bit 2
#define IsSideNotSame(side_valide)           (side_valide & 0x0004)              //bit 2

#define SetNotHighPlatform(side_valide)          (side_valide |= 0x0008)              //bit 3
#define IsNotHighPlatform(side_valide)           (side_valide & 0x0008)              //bit 3

#define SetNoPlatform(side_valide)          (side_valide |= 0x0010)              //bit 4
#define IsNoPlatform(side_valide)           (side_valide & 0x0010)              //bit 4

#define SetNotStopThisClassTrain(side_valide)          (side_valide |= 0x0020)              //bit 5
#define IsNotStopThisClassTrain(side_valide)           (side_valide & 0x0020)              //bit 5

#define SetNotStopCRHTrain(side_valide)          (side_valide |= 0x0040)              //bit 6
#define IsNotStopCRHTrain(side_valide)           (side_valide & 0x0040)              //bit 6

#define SetNotGiveSide(side_valide)          (side_valide |= 0x0080)              //bit 7
#define IsNotGiveSide(side_valide)           (side_valide & 0x0080)              //bit 7

#define SetNotSpaceTrain(side_valide)          (side_valide |= 0x0100)              //bit 8
#define IsNotSpaceTrain(side_valide)           (side_valide & 0x0100)              //bit 8

#define SetNotCheckSide(side_valide)          (side_valide |= 0x80000000)              //bit 31
#define IsNotCheckSide(side_valide)           (side_valide & 0x80000000)              //bit 31

class NodeRecord
{
public:
	NodeRecord()
	{
		memset(this, 0, sizeof(NodeRecord));
	}

public:
	DWORD train_index;
	TIME arrive;
	TIME depart;
	BYTE arrive_side;
	BYTE depart_side;
	BYTE entry;
	BYTE exit;
	BYTE accept;
	BYTE giveout;
	BYTE pass_stay;
};

class ResultInfo
{
public:
	ResultInfo()
	{
		memset(this, 0, sizeof(ResultInfo));
	}
    //bool IsSame(const ResultInfo rhs);
    char trainid[10];
	BYTE flag; // 1:目标有 2:目标和基本图都有 3:基本图有
	DWORD side_valide;

	NodeRecord obj;
	NodeRecord basic;
};

class ResultList
{
private:
	ResultList(const ResultList& rhs);
	ResultList& ResultList::operator=(const ResultList& rhs);
public:
	ResultList();
	~ResultList();
	void clear();

public:
	int station;
	CString stationName;
	std::vector<ResultInfo> list_trains;
};

class ResultInfoForWork
{
public:
	ResultInfoForWork()
	{
		memset(this, 0, sizeof(ResultInfo));
	}
    //bool IsSame(const ResultInfo rhs);
    char trainid[10];
	BYTE flag; // 1:目标有 2:目标和基本图都有 3:基本图有 4:日班有 
			   // 5:目标和日班图都有 
	           // 6:目标,基本图和日班图都有
			   // 7:基本图和日班图都有

	BYTE  arrive_real;
	BYTE  depart_real;
	DWORD side_valide;

	NodeRecord work;
	NodeRecord basic;
	NodeRecord shift;
};

class ResultListForWork
{
private:
	ResultListForWork(const ResultListForWork& rhs);
	ResultListForWork& operator=(const ResultListForWork& rhs);
public:
	ResultListForWork();
	~ResultListForWork();
	void clear();

public:
	int station;
	CString stationName;
	std::vector<ResultInfoForWork> list_trains;
};

class  CStationSelGrid:public CXMLDataGrid
{
private:
	struct PropertyInfoPara  m_result_def[10];
	int m_result_col_count;
public:
	BOOL  CreateGrid(CWnd *parent,const RECT &rect,UINT ctrlid);
	void  AddRow(int station, int iSel);
	int   GetSelectStation(int station_list[]);
};

class  CFindSchduleGrid:public CXMLDataGrid
{
public:
	virtual void  OnSelRowCol(int nRow, int nCol);
	BOOL  InsertRows(const char *xml,const char *rootnode);

	void SetFindTrainViewHWnd(HWND hViewWnd){ m_hViewWnd=hViewWnd;}
private:
	HWND m_hViewWnd;
};

CString GetTimeStr(TIME tm1);
CString GetDateTimeStr(TIME tm1);
CString GetStationNeedAlarm(DWORD side_valide);
CString GetSideName(int station, BYTE arrive_side, BYTE depart_side);
DWORD CheckSideInfo(CTG_SingleTrain* ptrain, CTG_TrainRecord* prec, int rec_index);
void GetNodeRecord(NodeRecord& result, const TG_TrainRecord& data, int m, int count);
CString GetStationPassStayInfo(BYTE accept, BYTE giveout, BYTE pass_stay);

CString GetNodeStrValue(MSXML::IXMLDOMNodePtr pRoot, CString node);
int     GetNodeIntValue(MSXML::IXMLDOMNodePtr pRoot, CString node);
