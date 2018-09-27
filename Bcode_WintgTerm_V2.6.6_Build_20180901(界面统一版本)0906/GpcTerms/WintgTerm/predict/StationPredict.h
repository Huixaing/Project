#pragma once
#include <map>
#include "tg_schedule_base.h"
#include <afxtempl.h>
using namespace std;

struct TrainPredict
{
	enum 
	{
		WAIT_NEIB_YUGAO,
		NEIB_WAIT_YUGAO,
		NEIB_YUGAO,
		YUGAO_NEIB,
		TONGYI_NEIB,
		NEIB_TONGYI,
		NEIB_CANCEL,
		CANCEL_NEIB,
		ARRIVE_REAL,
		DEPART_REAL,
	};

	// train_index=0 station_no=0 表示未使用
	// train_index=0 station_no=65535 表示是释放点可以使用
    TRAIN_INDEX    train_index;
	STATION_NO     station_no;
	BYTE     rec_index;   
	
	
	ULONG    sort; // 排序用
	BYTE     start; // 0: 到发 1: 始发 2:终到
	BYTE     passenger; // 1: 客 2:货
	
	BYTE     entry;
	BYTE     exit;
	BYTE     next_entry;

	TRAIN_NO arrive_train_id;
	BYTE     arrive_real; // 0:计划 1:实际
	STATION_NO     pre_station_no;
	ULONG    plan_arrive_time;
	ULONG    arrive_time;
	ULONG    neib_tongyi_time;
	ULONG    neib_yugao_time;
	ULONG    neib_cancel_time;
	
	
	TRAIN_NO depart_train_id;
	BYTE     depart_real; // 0:计划 1:实际
	STATION_NO     next_station_no;
	ULONG    plan_depart_time;
	ULONG    depart_time;
	ULONG    tongyi_neib_time;
	ULONG    yugao_neib_time;
	ULONG    cancel_neib_time;
	
	BYTE     finish_predict;
	
	TrainPredict()
	{
		InitZero();
	}
	void InitZero()
	{
		memset(this, 0, sizeof(TrainPredict));
	}
};

#define MAX_STPLAN_NUM 2048

class CStationPredict
{
public:
	CStationPredict(STATION_NO nStationNo, STATION_NO nNeibStation);
	virtual ~CStationPredict(void);

public:
	bool AddPredictPlan(TrainPredict  &predict);
	bool DeletePredict(TRAIN_INDEX train_index, short rec_index, TrainPredict& predict);
	bool DeletePredict(CString arriveid, CString departid, TrainPredict& predict);
	int  UpdatePredictPlan(TrainPredict  &predict); // 该函数只要下计划驱动调用，其它不能调用，未考虑train_index=0
	void SetFinishPredict();
	
public:
	void LoadStplanFromFile();
	void SaveStplanToFile();
	
public:
	int  GetStplanByTrainIndex(TRAIN_INDEX train_index, BYTE rec_index, TrainPredict& predict);
	void GetStplanByTrainNoAndNextStation(CString trainid, STATION_NO next_station, std::vector<int>& list_idx);
	void GetStplanByTrainNoAndPreStation(CString trainid, STATION_NO before_station, std::vector<int>& list_idx);

	void AddNeibStation(STATION_NO neibStation);
	bool IsMyNeibStation(STATION_NO neibStation);
public:
	STATION_NO  GetStationNo() {return mStationNo;}
	int  GetNeibStation(STATION_NO list[32]);
public:
	TrainPredict m_stplan[MAX_STPLAN_NUM];
	
private:
	STATION_NO  n_neibstation[12];
	int  mLircCtrlMode;
	STATION_NO  mStationNo;
};
