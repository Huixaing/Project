//
#ifndef TG_TRAIN_H
#define TG_TRAIN_H

#include "vector"
#include "map"

#include "Tg_BLOCK.h"
#include "Tg_TRAIN.h"

using namespace std;

const int SCHD_APPEND = 1;
const int SCHD_DELETE = 2;
const int SCHD_UPDATE = 3;
const int MAX_CZH_MEM = 4096;
const int MAX_CZH_TRAIN = 48;

struct TrainInfo
{
	TRAIN_INDEX train_index;
	char  trainname[32];
	DWORD stTime;
	DWORD edTime;
	DWORD begin_station;
	short begin_side;
	DWORD end_station;
	short end_side;
	TrainInfo()
	{
		train_index=0;
		memset(trainname, 0, sizeof(trainname));
		stTime=edTime=0;
		begin_station=end_station=0;
		begin_side=end_side=0;
	}
};

class TrainGroupInfo
{
public:
	CString strName;
	int train_count;
	TrainInfo trains[MAX_CZH_TRAIN+1];
	
	TrainGroupInfo()
	{
		train_count=0;
	}
	TrainGroupInfo(const TrainGroupInfo& rhs)
	{
		strName=rhs.strName;
		train_count=rhs.train_count;
		for(int i=0; i<train_count; i++)
			trains[i]=rhs.trains[i];
	}
	TrainGroupInfo& operator=(const TrainGroupInfo& rhs)
	{
		if(this == &rhs)
			return (*this);

		strName=rhs.strName;
		train_count=rhs.train_count;
		for(int i=0; i<train_count; i++)
			trains[i]=rhs.trains[i];
		return *this;
	}

	bool IsInThisGroup(TRAIN_INDEX train_index)
	{
		for(int i=0; i<train_count; i++)
		{
			if(trains[i].train_index==train_index)
			{
				return true;
			}
		}
		return false;
	}
};

class CTGForm;
class CTG_Mange
{
	friend class CTGDoc;
	friend class CTGForm;
public:
	CTG_Mange();
	~CTG_Mange();

private:
    void NoticeDlgUpdate();
	void DrawPrevTrain(TRAIN_INDEX prev_train_index1, TRAIN_INDEX prev_train_index2);
	void InitTrainDrawInfo(CTG_TRAIN& train);
	void TrytoModifyOtherTrain(CTG_TRAIN& train);
	void TrytoDeleteOtherTrain(CTG_TRAIN& train);
public:
	void BeginLoad();
	void EndLoad();
	
	void RemoveAllTrain();
	void RemoveAllMark();

	void DeleteTrain(const TRAIN_KEY key, DWORD nEraseFlag);

	void DeleteMark(const MARK_KEY key);
	
	void GenTrainIndex();

	void UpdateTrain(CTG_TRAIN& train);
	void UpdateMark(CTg_BLOCK& mark);
	void SetBatchDrawEnv(LONG nFlag);
	void BatchAddTrain(CTG_TRAIN& train);
	void BatchAddBlock(CTg_BLOCK& block);
	void BatchDraw(LONG nFlag);
	BYTE IsBatchStatus(void);
	bool DataIsExistInOCX(ULONG nTrainIndex, ULONG data_type);
public:
	CTG_TRAIN* GetTrain(TRAIN_KEY key);
	CTg_BLOCK* GetMark(MARK_KEY key);
	
	void SetMyForm(CTGForm* form);
	void SetMyDoc(CTGDoc* doc);

	int FindInvisibleMark(DWORD btm, DWORD etm, ULONG mark_index[], const ULONG index_cnt, int nDispDistanceNum);

	int GetInValidTrainCount(int trainIndex[],int num=2048);

	const CTG_TRAIN* GetTrainByPoint(CPoint& pt);
	
	void GenTrainCZHRelation(CString czh);
	void SetTrainIndexRelation(DWORD prev_train_index, DWORD this_train_index);
	
	bool GetMaybePrevTrains(int station, long depart, CString& text, CStringArray& list_trainno, CDWordArray& list_trainindex);
	void GetMaybeNextTrains(int station, long arrive, CString& trainid, CStringArray& list_trainno, CDWordArray& list_trainindex);

	void GetTrainRelation(vector<TrainGroupInfo>& list, bool bSortByName=false);
	bool GenTrainInfo(CTG_TRAIN* ptrain, TrainInfo& info);

public:
	//下面函数代码是单趟车显示使用
	void SetSingleTrain(bool bSingleTrainMode);
	void ClearSingleTrain();
	void AppendSingleTrain(TRAIN_INDEX index, const CTG_TRAIN& train);
	void UpdateSingleTrain(TRAIN_INDEX index, const CTG_TRAIN& train);

public:
	CString CheckTrainSide(TRAIN_INDEX train_index, int station, short sideno, DWORD arrive, DWORD depart);
	void GetPrevAndAfterTrainIndex(TRAIN_INDEX train_index, TRAIN_INDEX& prev_train_index, TRAIN_INDEX& next_train_index);
private:
	void GetTrainGroupInfo(vector<TrainGroupInfo>& list, bool bSortByName);
private:
	map<TRAIN_KEY, CTG_TRAIN>  myTrainTmp; // 单趟车使用,临时暂存
	map<TRAIN_KEY, CTG_TRAIN>  myTrain;
	map<MARK_KEY,  CTg_BLOCK>  myMark;

	struct TrainGroup
	{
		int group_index;
		int train_count;
		TRAIN_INDEX prev_train_index[MAX_CZH_TRAIN+1];
		TRAIN_INDEX this_train_index[MAX_CZH_TRAIN+1];
		TrainGroup()
		{
			group_index=0;
			train_count=0;
			memset(prev_train_index, 0, sizeof(prev_train_index));
			memset(this_train_index, 0, sizeof(this_train_index));
		}
	};

	int m_nTrainGroupCount;
	TrainGroup m_trainGroup[MAX_CZH_MEM+1];
	
	bool m_bBatchDraw; // false:单一画线 true:整体画线
private:
	CTGForm* myPForm;
	CTGDoc* myPDoc;
};
#endif